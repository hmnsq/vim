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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_CLIENTSERVER)

ਸਥਿਰ ਖਾਲੀ cmdsrv_main(ਪੂਰਨ *argc, ਅੱਖਰ **argv, char_u *serverName_arg, char_u **serverStr);
ਸਥਿਰ char_u *serverMakeName(char_u *arg, ਅੱਖਰ *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    ਖਾਲੀ
server_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(char_u *str)
{
    char_u      *ptr = ਨਿਰਾ;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ਨਿਰਾ);
    p_cpo = cpo_save;

    ਜੇ (ptr != ਨਿਰਾ && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	ਜੇ (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(ਖਾਲੀ)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_string(char_u *ਵਿਅੰਜਕ)
{
    char_u	*res;
    ਪੂਰਨ		save_dbl = debug_break_level;
    ਪੂਰਨ		save_ro = redir_off;
    funccal_ਇੰਦਰਾਜ_T funccal_ਇੰਦਰਾਜ;
    ਪੂਰਨ		did_save_funccal = FALSE;

# ਜੇ defined(FEAT_EVAL)
    ch_log(ਨਿਰਾ, "eval_client_expr_to_string(\"%s\")", ਵਿਅੰਜਕ);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    ਜੇ (!debug_ਬਹੁਲਕ)
    {
	save_funccal(&funccal_ਇੰਦਰਾਜ);
	did_save_funccal = TRUE;
    }

     // Disable debugging, otherwise Vim hangs, waiting for "cont" to be
     // typed.
    debug_break_level = -1;
    redir_off = 0;
    // Do not display error message, otherwise Vim hangs, waiting for "cont"
    // to be typed.  Do generate errors so that try/catch works.
    ++emsg_silent;

    res = eval_ਮੰਜ਼ਿਲ_string(ਵਿਅੰਜਕ, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    ਜੇ (emsg_silent < 0)
	emsg_silent = 0;
    ਜੇ (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    ਬਾਹਰਲਾ_flush_cursor(FALSE, FALSE);

    ਵਾਪਸ res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    ਪੂਰਨ
send_ਮੰਜ਼ਿਲ_Local_Vim(char_u *cmd, ਪੂਰਨ as_ਵਿਅੰਜਕ, char_u **ਨਤੀਜਾ)
{
    ਜੇ (as_ਵਿਅੰਜਕ)
    {
	char_u *ret;

	ret = eval_client_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_string(cmd);
	ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ)
	{
	    ਜੇ (ret == ਨਿਰਾ)
	    {
		ਅੱਖਰ	*err = _(e_invalid_ਵਿਅੰਜਕ_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*ਸੁਨੇਹਾ;

		ਸੁਨੇਹਾ = alloc(len);
		ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
		    vim_snprintf((ਅੱਖਰ *)ਸੁਨੇਹਾ, len, "%s: \"%s\"", err, cmd);
		*ਨਤੀਜਾ = ਸੁਨੇਹਾ;
	    }
	    ਨਹੀਂਤਾਂ
		*ਨਤੀਜਾ = ret;
	}
	ਨਹੀਂਤਾਂ
	    vim_free(ret);
	ਵਾਪਸ ret == ਨਿਰਾ ? -1 : 0;
    }
    server_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(cmd);
    ਵਾਪਸ 0;
}

/*
 * If conversion is needed, convert "data" from "client_enc" to 'encoding' and
 * return an allocated string.  Otherwise return "data".
 * "*tofree" is set to the result when it needs to be freed later.
 */
    char_u *
serverConvert(
    char_u *client_enc ਅਣਵਰਤਿਆ,
    char_u *ਅੰਕੜੇ,
    char_u **tofree)
{
    char_u	*res = ਅੰਕੜੇ;

    *tofree = ਨਿਰਾ;
    ਜੇ (client_enc == ਨਿਰਾ || p_enc == ਨਿਰਾ)
	ਵਾਪਸ res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ਜੇ (convert_ਤਿਆਰੀ(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, ਅੰਕੜੇ, ਨਿਰਾ);
	ਜੇ (res == ਨਿਰਾ)
	    res = ਅੰਕੜੇ;
	ਨਹੀਂਤਾਂ
	    *tofree = res;
    }
    convert_ਤਿਆਰੀ(&vimconv, ਨਿਰਾ, ਨਿਰਾ);
    ਵਾਪਸ res;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ਸਥਿਰ char_u *build_drop_cmd(ਪੂਰਨ filec, ਅੱਖਰ **filev, ਪੂਰਨ tabs, ਪੂਰਨ send_ਜਵਾਬ);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    ਖਾਲੀ
exec_on_server(mparm_T *parmp)
{
    ਪੂਰਨ made_name = FALSE;

    ਜੇ (parmp->serverName_arg != ਨਿਰਾ && *parmp->serverName_arg == NUL)
	ਵਾਪਸ;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    ਜੇ (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    ਜੇ (!made_name && parmp->servername == ਨਿਰਾ)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    ਜੇ (parmp->servername != ਨਿਰਾ && clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
    {
	server_ਸੈੱਟ_Name(parmp->servername);
#  ifndef FEAT_SOCKETSERVER
	vim_free(parmp->servername);
#  endif
    }
# endif
}
/*
 * Prepare for running as a Vim server.
 */
    ਖਾਲੀ
prepare_server(mparm_T *parmp)
{
# ਜੇ defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    ਜੇ (
#  ਜੇ defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ਨਿਰਾ
#   endif
#   ਜੇ defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   ਜੇ defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ਨਿਰਾ && (
#  ਜੇ defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   ਜੇ defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_ਵਿਧੀ != CLIENTSERVER_ਵਿਧੀ_SOCKET &&
#   endif
#   ਜੇ defined(FEAT_AUTOSERVERNAME)
		    1
#   ਨਹੀਂਤਾਂ
		    gui.ਵਿੱਚ_use
#   endif
#   ifdef UNIX
		 && getuid() != ਜੜ੍ਹ_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ਨਿਰਾ))
    {

#  ifdef FEAT_SOCKETSERVER
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
	    socketserver_ਸ਼ੁਰੂ(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
	{
	    (ਖਾਲੀ)server_ਰਜਿਸਟਰ_Name(X_DISPLAY, parmp->servername);
	    ਸਮਾਂ_ਸੁਨੇਹਾ("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    ਨਹੀਂਤਾਂ
    {
#  ifdef FEAT_X11
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
	    server_Delayed_ਸ਼ੁਰੂ_Name = parmp->servername;
	ਨਹੀਂਤਾਂ
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    ਜੇ (parmp->serverStr != ਨਿਰਾ)
    {
	char_u *p;

	server_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    ਸਥਿਰ ਖਾਲੀ
cmdsrv_main(
    ਪੂਰਨ		*argc,
    ਅੱਖਰ	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    ਪੂਰਨ		i;
    char_u	*sname;
    ਪੂਰਨ		ret = -1;
    ਪੂਰਨ		didone = FALSE;
    ਪੂਰਨ		exiterr = 0;
    ਅੱਖਰ	**newArgV = argv + 1;
    ਪੂਰਨ		newArgC = 1,
		Argc = *argc;
    ਪੂਰਨ		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    ਪੂਰਨ		silent = FALSE;
    ਪੂਰਨ		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    ਖਿੜਕੀ	srv;

    ਤਿਆਰੀ_ਪਦ_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ਨਿਰਾ;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    ਜੇ (sname == ਨਿਰਾ)
	ਵਾਪਸ;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    ਲਈ (i = 1; i < Argc; i++)
    {
	res = ਨਿਰਾ;
	ਜੇ (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    ਲਈ (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    ਰੋਕੋ;
	}

	ਜੇ (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    ਅੱਖਰ	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    ਜਦੋਂ (*p != NUL)
	    {
		ਜੇ (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		ਨਹੀਂਤਾਂ
		{
		    argtype = ARGTYPE_OTHER;
		    ਰੋਕੋ;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    argtype = ARGTYPE_OTHER;

	ਜੇ (argtype != ARGTYPE_OTHER)
	{
	    ਜੇ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ਜੇ (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		ਜੇ (*serverStr == ਨਿਰਾ)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    ਰੋਕੋ;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
		ret = socketserver_send(sname, *serverStr, ਨਿਰਾ, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
	    {
		ਜੇ (xterm_dpy == ਨਿਰਾ)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		ਨਹੀਂਤਾਂ
		    ret = server_Send_ਮੰਜ਼ਿਲ_Vim(xterm_dpy, sname, *serverStr,
			    ਨਿਰਾ, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
		// Win32 always works?
		ret = server_Send_ਮੰਜ਼ਿਲ_Vim(sname, *serverStr, ਨਿਰਾ, &srv, 0, 0,
			silent);
# endif
	    ਜੇ (ret < 0)
	    {
		ਜੇ (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		ਰੋਕੋ;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    ਜੇ (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		ਸੈੱਟ_Foreground_ਖਿੜਕੀ(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    ਜੇ (argtype == ARGTYPE_EDIT_WAIT)
	    {
		ਪੂਰਨ	ਅੰਕ_Files = *argc - i - 1;
		char_u  *ਪੂਰਾ = alloc(ਅੰਕ_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		ਪੂਰਨ	ਗਿਣਤੀ_2 = 0;
		ਬਾਹਰੀ HWND ਸੁਨੇਹਾ_ਖਿੜਕੀ;
# endif

		ਜੇ (ਅੰਕ_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --ਅੰਕ_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cb_ਮਾਪ = ਆਕਾਰ(ni);
		ni.hWnd = ਸੁਨੇਹਾ_ਖਿੜਕੀ;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = ਲੱਦੋ_Icon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), ਗਿਣਤੀ_2, ਅੰਕ_Files);
		Shell_NotifyIcon(NIM_ਜੋੜੋ, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(ਪੂਰਾ, 0, ਅੰਕ_Files);
		ਜਦੋਂ (memchr(ਪੂਰਾ, 0, ਅੰਕ_Files) != ਨਿਰਾ)
		{
		    char_u  *p = ਨਿਰਾ;
		    ਪੂਰਨ	    j;
# ifdef MSWIN
		    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
			p = server_Get_ਜਵਾਬ(srv, ਨਿਰਾ, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET
			    && (ch == ਨਿਰਾ
				|| socketserver_read_ਜਵਾਬ(sname, &p, -1, true)
				== FAIL))
			    ਰੋਕੋ;
# endif
# ifdef FEAT_X11
		    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11
			    && server_Read_ਜਵਾਬ(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    ਰੋਕੋ;
# endif
		    ਜੇ (p == ਨਿਰਾ)
			ਰੋਕੋ;
		    j = atoi((ਅੱਖਰ *)p);
		    vim_free(p);
		    ਜੇ (j >= 0 && j < ਅੰਕ_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++ਗਿਣਤੀ_2;
			sprintf(ni.szTip, _("%d of %d edited"),
							     ਗਿਣਤੀ_2, ਅੰਕ_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			ਪੂਰਾ[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		ਜੇ (ch != ਨਿਰਾ)
		{
		    channel_close(ch, false);
		    channel_clear(ch);
		}
# endif
# ifdef FEAT_GUI_MSWIN
		Shell_NotifyIcon(NIM_DELETE, &ni);
# endif
		vim_free(ਪੂਰਾ);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    ਪੂਰਨ ਸਥਿਤੀ = OK;

	    ਜੇ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN
		    && server_Send_ਮੰਜ਼ਿਲ_Vim(sname, (char_u *)argv[i + 1],
						  &res, ਨਿਰਾ, 1, 0, FALSE) < 0)
		ਸਥਿਤੀ = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ਨਿਰਾ) < 0)
		ਸਥਿਤੀ = FAIL;
# endif
# ifdef FEAT_X11
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
	    {
		ਜੇ (xterm_dpy == ਨਿਰਾ)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		ਨਹੀਂਤਾਂ ਜੇ (server_Send_ਮੰਜ਼ਿਲ_Vim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ਨਿਰਾ, 1, 0, 1, FALSE) < 0)
		    ਸਥਿਤੀ = FAIL;
	    }
# endif
	    ਜੇ (ਸਥਿਤੀ == FAIL)
	    {
		ਜੇ (res != ਨਿਰਾ && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((ਅੱਖਰ *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ਨਿਰਾ;
	    garray_T	ga;

# ifdef MSWIN
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
		// Win32 always works?
		list = server_Get_Vim_ਨਾਂ();
# endif
# ifdef FEAT_SOCKETSERVER
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11 &&
		    xterm_dpy != ਨਿਰਾ)
		list = server_Get_Vim_ਨਾਂ(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    ਜੇ (list != ਨਿਰਾ)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_ਅੰਕੜੇ;

	    ਜੇ (did_emsg)
		mch_errmsg("\n");
	}
	ਨਹੀਂਤਾਂ ਜੇ (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    ਜਾਰੀ;
	}
	ਨਹੀਂਤਾਂ
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    ਜਾਰੀ;
	}
	didone = TRUE;
	ਜੇ (res != ਨਿਰਾ && *res != NUL)
	{
	    mch_ਸੁਨੇਹਾ((ਅੱਖਰ *)res);
	    ਜੇ (res[STRLEN(res) - 1] != '\n')
		mch_ਸੁਨੇਹਾ("\n");
	}
	vim_free(res);
    }

    ਜੇ (didone)
    {
	display_errors();	// display any collected messages
	ਬਾਹਰ_ਨਿਕਲੋ(exiterr);	// Mission accomplished - get out
    }

    // Return back into main()
    *argc = newArgC;
    vim_free(sname);
}

/*
 * Build a ":drop" command to send to a Vim server.
 */
    ਸਥਿਰ char_u *
build_drop_cmd(
    ਪੂਰਨ		filec,
    ਅੱਖਰ	**filev,
    ਪੂਰਨ		tabs,		// Use ":tab drop" instead of ":drop".
    ਪੂਰਨ		send_ਜਵਾਬ)
{
    garray_T	ga;
    ਪੂਰਨ		i;
    char_u	*inicmd = ਨਿਰਾ;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ਅਚਲ string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    ਜੇ (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    ਜੇ (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    ਜੇ (cwd == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	ਵਾਪਸ ਨਿਰਾ;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_ਵਿੱਚ_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# ਨਹੀਂਤਾਂ
	    ਰਸਤਾ_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    ਜੇ (cdp.string == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    cdp.ਲੰਮਾਈ = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.ਲੰਮਾਈ);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].ਲੰਮਾਈ);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    ਜੇ (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    ਲਈ (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		ਰਸਤਾ_ESC_CHARS
# ਨਹੀਂਤਾਂ
		(char_u *)" \t%#"
# endif
		);
	ਜੇ (p == ਨਿਰਾ)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_ਅੰਕੜੇ);
	    ਵਾਪਸ ਨਿਰਾ;
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
# ਨਹੀਂਤਾਂ
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.ਲੰਮਾਈ);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].ਲੰਮਾਈ);

    ਜੇ (send_ਜਵਾਬ)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    ਜੇ (inicmd != ਨਿਰਾ)
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
    ਵਾਪਸ ga.ga_ਅੰਕੜੇ;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    ਸਥਿਰ char_u *
serverMakeName(char_u *arg, ਅੱਖਰ *cmd)
{
    char_u *p;

    ਜੇ (arg != ਨਿਰਾ && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
	{
	    ਜੇ (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    ਨਹੀਂਤਾਂ
		p = vim_strsave_ਉੱਪਰ(arg);
	}
	ਨਹੀਂਤਾਂ
	    p = vim_strsave_ਉੱਪਰ(arg);
# ਨਹੀਂਤਾਂ
	p = vim_strsave_ਉੱਪਰ(arg);
# endif
    }
    ਨਹੀਂਤਾਂ
    {
	p = vim_strsave_ਉੱਪਰ(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	ਜੇ (p != ਨਿਰਾ && vim_strchr(p, '.') != ਨਿਰਾ)
	    *vim_strchr(p, '.') = NUL;
    }
    ਵਾਪਸ p;
}
#ਜੇ_ਅੰਤ // FEAT_CLIENTSERVER

#ਜੇ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ਸਥਿਰ ਖਾਲੀ
make_connection(ਖਾਲੀ)
{
    ਜੇ (X_DISPLAY == ਨਿਰਾ
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
    {
	x_force_connect = TRUE;
	ਤਿਆਰੀ_ਪਦ_clip();
	x_force_connect = FALSE;
    }
}

    ਸਥਿਰ ਪੂਰਨ
check_connection(ਖਾਲੀ)
{
    ਜੇ (clientserver_ਵਿਧੀ != CLIENTSERVER_ਵਿਧੀ_X11)
	ਵਾਪਸ OK;
    make_connection();
    ਜੇ (X_DISPLAY == ਨਿਰਾ)
    {
	emsg(_(e_no_connection_ਮੰਜ਼ਿਲ_x_server));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
    ਸਥਿਰ ਖਾਲੀ
remote_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ ਵਿਅੰਜਕ)
{
    char_u	*server_name;
    char_u	*ਕੁੰਜੀਆਂ;
    char_u	*r = ਨਿਰਾ;
    char_u	buf[NUMBUFLEN];
    ਪੂਰਨ		timeout = 0;
# ifdef MSWIN
    HWND	w;
# ਨਹੀਂਤਾਂ
#  ifdef FEAT_X11
    ਖਿੜਕੀ	w;
#  endif
# endif
    ਪੂਰਨ		ret = OK;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

# ifdef FEAT_X11
    ਜੇ (check_connection() == FAIL)
	ਵਾਪਸ;
# endif
    ਜੇ (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_ਸੰਖਿਆ(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    ਜੇ (server_name == ਨਿਰਾ)
	ਵਾਪਸ;		// type error; errmsg already given
    ਕੁੰਜੀਆਂ = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN
	    && server_Send_ਮੰਜ਼ਿਲ_Vim(server_name, ਕੁੰਜੀਆਂ, &r, &w, ਵਿਅੰਜਕ,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET
	&& socketserver_send(server_name, ਕੁੰਜੀਆਂ, &r, ਵਿਅੰਜਕ,
	    timeout * 1000, true, ਨਿਰਾ) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
	ਜੇ (server_Send_ਮੰਜ਼ਿਲ_Vim(X_DISPLAY, server_name, ਕੁੰਜੀਆਂ, &r, &w, ਵਿਅੰਜਕ, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    ਜੇ (ret == FAIL)
    {
	ਜੇ (r != ਨਿਰਾ)
	{
	    emsg((ਅੱਖਰ *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	ਨਹੀਂਤਾਂ
	    semsg(_(e_unable_ਮੰਜ਼ਿਲ_send_ਮੰਜ਼ਿਲ_str), server_name);
	ਵਾਪਸ;
    }

    rettv->vval.v_string = r;

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# ਜੇ defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ਨਿਰਾ;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	ਜੇ (idvar != ਨਿਰਾ && *idvar != NUL)
	{
# ifdef MSWIN
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
	    {
		sprintf((ਅੱਖਰ *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
	    {
		sprintf((ਅੱਖਰ *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
		str = server_name;
# endif
	    ਜੇ (str == ਨਿਰਾ)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    ਸੈੱਟ_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    ਖਾਲੀ
f_remote_ਵਿਅੰਜਕ(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

# ifdef FEAT_CLIENTSERVER
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 3) == FAIL)))
	ਵਾਪਸ;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    ਖਾਲੀ
f_remote_foreground(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_CLIENTSERVER
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	ਜੇ (server_name != ਨਿਰਾ)
	    serverForeground(server_name);
    }
#  ਨਹੀਂਤਾਂ
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    ਖਾਲੀ
f_remote_peek(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ਨਿਰਾ;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    serverid = tv_get_string_chk(&argvars[0]);
    ਜੇ (serverid == ਨਿਰਾ)
	ਵਾਪਸ;		// type error; errmsg already given
#  ifdef MSWIN
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
    {
	ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅਚਲ ਅੱਖਰ *)serverid, SCANF_HEX_LONG_U, &n);
	ਜੇ (n == 0)
	    rettv->vval.v_ਸੰਖਿਆ = -1;
	ਨਹੀਂਤਾਂ
	{
	    s = server_Get_ਜਵਾਬ((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_ਸੰਖਿਆ = (s != ਨਿਰਾ);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
	rettv->vval.v_ਸੰਖਿਆ = socketserver_peek_ਜਵਾਬ(serverid, &s);
#  endif
#  ifdef FEAT_X11
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
    {
	ਜੇ (check_connection() == FAIL)
	    ਵਾਪਸ;

	rettv->vval.v_ਸੰਖਿਆ = server_Peek_ਜਵਾਬ(X_DISPLAY,
		server_Str_ਮੰਜ਼ਿਲ_Win(serverid), &s);
    }
#  endif

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_ਸੰਖਿਆ > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	ਜੇ (retvar != ਨਿਰਾ)
	    ਸੈੱਟ_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# ਨਹੀਂਤਾਂ
    rettv->vval.v_ਸੰਖਿਆ = -1;
# endif
}

    ਖਾਲੀ
f_remote_read(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    char_u	*r = ਨਿਰਾ;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    serverid = tv_get_string_chk(&argvars[0]);
    ਜੇ (serverid != ਨਿਰਾ && !check_restricted() && !check_secure())
    {
	ਪੂਰਨ timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_ਸੰਖਿਆ(&argvars[1]);

#  ifdef MSWIN
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
	{
	    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ((ਅੱਖਰ *)serverid, SCANF_HEX_LONG_U, &n);
	    ਜੇ (n != 0)
		r = server_Get_ਜਵਾਬ((HWND)n, FALSE, TRUE, TRUE, timeout);
	    ਜੇ (r == ਨਿਰਾ)
		emsg(_(e_unable_ਮੰਜ਼ਿਲ_read_server_ਜਵਾਬ));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET &&
		socketserver_read_ਜਵਾਬ(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_ਮੰਜ਼ਿਲ_read_server_ਜਵਾਬ));
#  endif
#  ifdef FEAT_X11
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11 &&
		(check_connection() == FAIL
		|| server_Read_ਜਵਾਬ(X_DISPLAY, server_Str_ਮੰਜ਼ਿਲ_Win(serverid),
						       &r, FALSE, timeout) < 0))
	    emsg(_(e_unable_ਮੰਜ਼ਿਲ_read_server_ਜਵਾਬ));
#  endif
    }
# endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = r;
}

/*
 * "remote_send()" function
 */
    ਖਾਲੀ
f_remote_send(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

# ifdef FEAT_CLIENTSERVER
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    ਖਾਲੀ
f_remote_startserver(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_CLIENTSERVER
    ਜੇ (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (serverName != ਨਿਰਾ)
    {
	emsg(_(e_already_started_server));
	ਵਾਪਸ;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    ਜੇ (server == ਨਿਰਾ)
	ਵਾਪਸ;
#  ifdef MSWIN
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
	server_ਸੈੱਟ_Name(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
	socketserver_ਸ਼ੁਰੂ(server, true);
#  endif
#  ifdef FEAT_X11
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11 &&
	    check_connection() == OK)
	server_ਰਜਿਸਟਰ_Name(X_DISPLAY, server);
#  endif

# ਨਹੀਂਤਾਂ
    emsg(_(e_clientserver_feature_not_ਉਪਲਬਧ));
# endif
}

    ਖਾਲੀ
f_server2client(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*ਜਵਾਬ;
    ਪੂਰਨ		ret = OK;

    rettv->vval.v_ਸੰਖਿਆ = -1;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    server = tv_get_string_chk(&argvars[0]);
    ਜਵਾਬ = tv_get_string_buf_chk(&argvars[1], buf);
    ਜੇ (server == ਨਿਰਾ || ਜਵਾਬ == ਨਿਰਾ)
	ਵਾਪਸ;

#  ifdef FEAT_SOCKETSERVER
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET &&
	    socketserver_send_ਜਵਾਬ(server, ਜਵਾਬ) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11 &&
	    server_Send_ਜਵਾਬ(server, ਜਵਾਬ) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN
	    && server_Send_ਜਵਾਬ(server, ਜਵਾਬ) < 0)
	ret = FAIL;
#  endif

    ਜੇ (ret == FAIL)
    {
	emsg(_(e_unable_ਮੰਜ਼ਿਲ_send_ਮੰਜ਼ਿਲ_client));
	ਵਾਪਸ;
    }
    rettv->vval.v_ਸੰਖਿਆ = 0;
# ਨਹੀਂਤਾਂ
    rettv->vval.v_ਸੰਖਿਆ = -1;
# endif
}

    ਖਾਲੀ
f_serverlist(typval_T *argvars ਅਣਵਰਤਿਆ, typval_T *rettv)
{
    list_T  *list = ਨਿਰਾ;
    bool    use_list = false;

    ਜੇ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_MSWIN)
	list = server_Get_Vim_ਨਾਂ();
#  endif
#  ifdef FEAT_SOCKETSERVER
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11)
    {
	make_connection();
	ਜੇ (X_DISPLAY != ਨਿਰਾ)
	    list = server_Get_Vim_ਨਾਂ(X_DISPLAY);
    }
#  endif
# endif
    ਜੇ (use_list)
    {
	ਜੇ (list == ਨਿਰਾ)
	{
	    (ਖਾਲੀ)rettv_list_alloc(rettv);
	    ਵਾਪਸ;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    ਨਹੀਂਤਾਂ
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	ਜੇ (list != ਨਿਰਾ)
	{
	    list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
	    ga_append(&ga, NUL);
	    list_free(list);
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = (char_u *)ga.ga_ਅੰਕੜੇ;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER_BACKENDS
/*
 * Check the $VIM_CLIENTSERVER environment variable and set the method
 */
    ਖਾਲੀ
check_clientserver_ਵਿਧੀ_env(ਖਾਲੀ)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    ਜੇ (env == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (STRICMP(env, "socket") == 0)
	clientserver_ਵਿਧੀ = CLIENTSERVER_ਵਿਧੀ_SOCKET;
# ifdef FEAT_X11
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(env, "x11") == 0)
	clientserver_ਵਿਧੀ = CLIENTSERVER_ਵਿਧੀ_X11;
# endif
# ifdef MSWIN
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP(env, "mswin") == 0)
	clientserver_ਵਿਧੀ = CLIENTSERVER_ਵਿਧੀ_MSWIN;
# endif
}
#ਜੇ_ਅੰਤ
