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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_CLIENTSERVER)

స్థిర శూన్య cmdsrv_main(పూర్ణాంకం *argc, అక్షరం **argv, char_u *serverName_arg, char_u **serverStr);
స్థిర char_u *serverMakeName(char_u *arg, అక్షరం *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    శూన్య
server_గమ్యం_ఆదానం_buf(char_u *str)
{
    char_u      *ptr = ఖాళీ;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ఖాళీ);
    p_cpo = cpo_save;

    ఒకవేళ (ptr != ఖాళీ && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	ఒకవేళ (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(శూన్య)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_సమాసం_గమ్యం_string(char_u *సమాసం)
{
    char_u	*res;
    పూర్ణాంకం		save_dbl = debug_break_level;
    పూర్ణాంకం		save_ro = redir_off;
    funccal_నమోదు_T funccal_నమోదు;
    పూర్ణాంకం		did_save_funccal = FALSE;

# ఒకవేళ defined(FEAT_EVAL)
    ch_log(ఖాళీ, "eval_client_expr_to_string(\"%s\")", సమాసం);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    ఒకవేళ (!debug_బహుళకం)
    {
	save_funccal(&funccal_నమోదు);
	did_save_funccal = TRUE;
    }

     // Disable debugging, otherwise Vim hangs, waiting for "cont" to be
     // typed.
    debug_break_level = -1;
    redir_off = 0;
    // Do not display error message, otherwise Vim hangs, waiting for "cont"
    // to be typed.  Do generate errors so that try/catch works.
    ++emsg_silent;

    res = eval_గమ్యం_string(సమాసం, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    ఒకవేళ (emsg_silent < 0)
	emsg_silent = 0;
    ఒకవేళ (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    బహిర్గతం_flush_cursor(FALSE, FALSE);

    తిరిగి res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    పూర్ణాంకం
send_గమ్యం_Local_Vim(char_u *cmd, పూర్ణాంకం as_సమాసం, char_u **ఫలితం)
{
    ఒకవేళ (as_సమాసం)
    {
	char_u *ret;

	ret = eval_client_సమాసం_గమ్యం_string(cmd);
	ఒకవేళ (ఫలితం != ఖాళీ)
	{
	    ఒకవేళ (ret == ఖాళీ)
	    {
		అక్షరం	*err = _(e_invalid_వ్యక్తీకరణ_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*సందేశం;

		సందేశం = alloc(len);
		ఒకవేళ (సందేశం != ఖాళీ)
		    vim_snprintf((అక్షరం *)సందేశం, len, "%s: \"%s\"", err, cmd);
		*ఫలితం = సందేశం;
	    }
	    లేకపోతే
		*ఫలితం = ret;
	}
	లేకపోతే
	    vim_free(ret);
	తిరిగి ret == ఖాళీ ? -1 : 0;
    }
    server_గమ్యం_ఆదానం_buf(cmd);
    తిరిగి 0;
}

/*
 * If conversion is needed, convert "data" from "client_enc" to 'encoding' and
 * return an allocated string.  Otherwise return "data".
 * "*tofree" is set to the result when it needs to be freed later.
 */
    char_u *
serverConvert(
    char_u *client_enc వాడని,
    char_u *దత్తాంశం,
    char_u **tofree)
{
    char_u	*res = దత్తాంశం;

    *tofree = ఖాళీ;
    ఒకవేళ (client_enc == ఖాళీ || p_enc == ఖాళీ)
	తిరిగి res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ఒకవేళ (convert_అమరిక(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, దత్తాంశం, ఖాళీ);
	ఒకవేళ (res == ఖాళీ)
	    res = దత్తాంశం;
	లేకపోతే
	    *tofree = res;
    }
    convert_అమరిక(&vimconv, ఖాళీ, ఖాళీ);
    తిరిగి res;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

స్థిర char_u *build_drop_cmd(పూర్ణాంకం filec, అక్షరం **filev, పూర్ణాంకం tabs, పూర్ణాంకం send_జవాబు);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    శూన్య
exec_on_server(mparm_T *parmp)
{
    పూర్ణాంకం made_name = FALSE;

    ఒకవేళ (parmp->serverName_arg != ఖాళీ && *parmp->serverName_arg == NUL)
	తిరిగి;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    ఒకవేళ (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    ఒకవేళ (!made_name && parmp->servername == ఖాళీ)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    ఒకవేళ (parmp->servername != ఖాళీ && clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
    {
	server_సమితి_Name(parmp->servername);
#  ifndef FEAT_SOCKETSERVER
	vim_free(parmp->servername);
#  endif
    }
# endif
}
/*
 * Prepare for running as a Vim server.
 */
    శూన్య
prepare_server(mparm_T *parmp)
{
# ఒకవేళ defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    ఒకవేళ (
#  ఒకవేళ defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ఖాళీ
#   endif
#   ఒకవేళ defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   ఒకవేళ defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ఖాళీ && (
#  ఒకవేళ defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   ఒకవేళ defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_పద్ధతి != CLIENTSERVER_పద్ధతి_SOCKET &&
#   endif
#   ఒకవేళ defined(FEAT_AUTOSERVERNAME)
		    1
#   లేకపోతే
		    gui.లోపల_use
#   endif
#   ifdef UNIX
		 && getuid() != మూలం_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ఖాళీ))
    {

#  ifdef FEAT_SOCKETSERVER
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
	    socketserver_ప్రారంభం(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
	{
	    (శూన్య)server_రిజిస్టరు_Name(X_DISPLAY, parmp->servername);
	    సమయం_సందేశం("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    లేకపోతే
    {
#  ifdef FEAT_X11
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
	    server_Delayed_ప్రారంభం_Name = parmp->servername;
	లేకపోతే
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    ఒకవేళ (parmp->serverStr != ఖాళీ)
    {
	char_u *p;

	server_గమ్యం_ఆదానం_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    స్థిర శూన్య
cmdsrv_main(
    పూర్ణాంకం		*argc,
    అక్షరం	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    పూర్ణాంకం		i;
    char_u	*sname;
    పూర్ణాంకం		ret = -1;
    పూర్ణాంకం		didone = FALSE;
    పూర్ణాంకం		exiterr = 0;
    అక్షరం	**newArgV = argv + 1;
    పూర్ణాంకం		newArgC = 1,
		Argc = *argc;
    పూర్ణాంకం		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    పూర్ణాంకం		silent = FALSE;
    పూర్ణాంకం		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    కిటికీ	srv;

    అమరిక_పదం_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ఖాళీ;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    ఒకవేళ (sname == ఖాళీ)
	తిరిగి;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    కొరకు (i = 1; i < Argc; i++)
    {
	res = ఖాళీ;
	ఒకవేళ (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    కొరకు (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    ఆపు;
	}

	ఒకవేళ (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	లేకపోతే ఒకవేళ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    అక్షరం	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    అయితే (*p != NUL)
	    {
		ఒకవేళ (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		లేకపోతే ఒకవేళ (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		లేకపోతే ఒకవేళ (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		లేకపోతే
		{
		    argtype = ARGTYPE_OTHER;
		    ఆపు;
		}
	    }
	}
	లేకపోతే
	    argtype = ARGTYPE_OTHER;

	ఒకవేళ (argtype != ARGTYPE_OTHER)
	{
	    ఒకవేళ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ఒకవేళ (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    లేకపోతే
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		ఒకవేళ (*serverStr == ఖాళీ)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    ఆపు;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
		ret = socketserver_send(sname, *serverStr, ఖాళీ, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
	    {
		ఒకవేళ (xterm_dpy == ఖాళీ)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		లేకపోతే
		    ret = server_Send_గమ్యం_Vim(xterm_dpy, sname, *serverStr,
			    ఖాళీ, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
		// Win32 always works?
		ret = server_Send_గమ్యం_Vim(sname, *serverStr, ఖాళీ, &srv, 0, 0,
			silent);
# endif
	    ఒకవేళ (ret < 0)
	    {
		ఒకవేళ (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		లేకపోతే ఒకవేళ (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		ఆపు;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    ఒకవేళ (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		సమితి_Foreground_కిటికీ(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    ఒకవేళ (argtype == ARGTYPE_EDIT_WAIT)
	    {
		పూర్ణాంకం	అంకె_Files = *argc - i - 1;
		char_u  *పూర్తి = alloc(అంకె_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		పూర్ణాంకం	లెక్క = 0;
		బాహ్య HWND సందేశం_కిటికీ;
# endif

		ఒకవేళ (అంకె_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --అంకె_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cb_కొలత = పరిమాణం(ni);
		ni.hWnd = సందేశం_కిటికీ;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = ఎక్కించు_Icon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), లెక్క, అంకె_Files);
		Shell_NotifyIcon(NIM_కూడు, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(పూర్తి, 0, అంకె_Files);
		అయితే (memchr(పూర్తి, 0, అంకె_Files) != ఖాళీ)
		{
		    char_u  *p = ఖాళీ;
		    పూర్ణాంకం	    j;
# ifdef MSWIN
		    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
			p = server_Get_జవాబు(srv, ఖాళీ, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET
			    && (ch == ఖాళీ
				|| socketserver_read_జవాబు(sname, &p, -1, true)
				== FAIL))
			    ఆపు;
# endif
# ifdef FEAT_X11
		    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11
			    && server_Read_జవాబు(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    ఆపు;
# endif
		    ఒకవేళ (p == ఖాళీ)
			ఆపు;
		    j = atoi((అక్షరం *)p);
		    vim_free(p);
		    ఒకవేళ (j >= 0 && j < అంకె_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++లెక్క;
			sprintf(ni.szTip, _("%d of %d edited"),
							     లెక్క, అంకె_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			పూర్తి[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		ఒకవేళ (ch != ఖాళీ)
		{
		    channel_close(ch, false);
		    channel_clear(ch);
		}
# endif
# ifdef FEAT_GUI_MSWIN
		Shell_NotifyIcon(NIM_DELETE, &ni);
# endif
		vim_free(పూర్తి);
	    }
	}
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    పూర్ణాంకం స్థితి_2 = OK;

	    ఒకవేళ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN
		    && server_Send_గమ్యం_Vim(sname, (char_u *)argv[i + 1],
						  &res, ఖాళీ, 1, 0, FALSE) < 0)
		స్థితి_2 = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ఖాళీ) < 0)
		స్థితి_2 = FAIL;
# endif
# ifdef FEAT_X11
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
	    {
		ఒకవేళ (xterm_dpy == ఖాళీ)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		లేకపోతే ఒకవేళ (server_Send_గమ్యం_Vim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ఖాళీ, 1, 0, 1, FALSE) < 0)
		    స్థితి_2 = FAIL;
	    }
# endif
	    ఒకవేళ (స్థితి_2 == FAIL)
	    {
		ఒకవేళ (res != ఖాళీ && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((అక్షరం *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ఖాళీ;
	    garray_T	ga;

# ifdef MSWIN
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
		// Win32 always works?
		list = server_Get_Vim_పేర్లు();
# endif
# ifdef FEAT_SOCKETSERVER
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11 &&
		    xterm_dpy != ఖాళీ)
		list = server_Get_Vim_పేర్లు(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    ఒకవేళ (list != ఖాళీ)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_దత్తాంశం;

	    ఒకవేళ (did_emsg)
		mch_errmsg("\n");
	}
	లేకపోతే ఒకవేళ (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    కొనసాగు;
	}
	లేకపోతే
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    కొనసాగు;
	}
	didone = TRUE;
	ఒకవేళ (res != ఖాళీ && *res != NUL)
	{
	    mch_సందేశం((అక్షరం *)res);
	    ఒకవేళ (res[STRLEN(res) - 1] != '\n')
		mch_సందేశం("\n");
	}
	vim_free(res);
    }

    ఒకవేళ (didone)
    {
	display_errors();	// display any collected messages
	నిష్క్రమించు(exiterr);	// Mission accomplished - get out
    }

    // Return back into main()
    *argc = newArgC;
    vim_free(sname);
}

/*
 * Build a ":drop" command to send to a Vim server.
 */
    స్థిర char_u *
build_drop_cmd(
    పూర్ణాంకం		filec,
    అక్షరం	**filev,
    పూర్ణాంకం		tabs,		// Use ":tab drop" instead of ":drop".
    పూర్ణాంకం		send_జవాబు)
{
    garray_T	ga;
    పూర్ణాంకం		i;
    char_u	*inicmd = ఖాళీ;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    స్థిరరాశి string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    ఒకవేళ (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    ఒకవేళ (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    ఒకవేళ (cwd == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	తిరిగి ఖాళీ;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_లోపల_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# లేకపోతే
	    మార్గం_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    ఒకవేళ (cdp.string == ఖాళీ)
	తిరిగి ఖాళీ;
    cdp.నిడివి = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.నిడివి);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].నిడివి);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    ఒకవేళ (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    కొరకు (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		మార్గం_ESC_CHARS
# లేకపోతే
		(char_u *)" \t%#"
# endif
		);
	ఒకవేళ (p == ఖాళీ)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_దత్తాంశం);
	    తిరిగి ఖాళీ;
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
# లేకపోతే
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.నిడివి);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].నిడివి);

    ఒకవేళ (send_జవాబు)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    ఒకవేళ (inicmd != ఖాళీ)
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
    తిరిగి ga.ga_దత్తాంశం;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    స్థిర char_u *
serverMakeName(char_u *arg, అక్షరం *cmd)
{
    char_u *p;

    ఒకవేళ (arg != ఖాళీ && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
	{
	    ఒకవేళ (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    లేకపోతే
		p = vim_strsave_పైకి(arg);
	}
	లేకపోతే
	    p = vim_strsave_పైకి(arg);
# లేకపోతే
	p = vim_strsave_పైకి(arg);
# endif
    }
    లేకపోతే
    {
	p = vim_strsave_పైకి(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	ఒకవేళ (p != ఖాళీ && vim_strchr(p, '.') != ఖాళీ)
	    *vim_strchr(p, '.') = NUL;
    }
    తిరిగి p;
}
#అయితే_ముగింపు // FEAT_CLIENTSERVER

#ఒకవేళ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    స్థిర శూన్య
make_connection(శూన్య)
{
    ఒకవేళ (X_DISPLAY == ఖాళీ
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
    {
	x_force_connect = TRUE;
	అమరిక_పదం_clip();
	x_force_connect = FALSE;
    }
}

    స్థిర పూర్ణాంకం
check_connection(శూన్య)
{
    ఒకవేళ (clientserver_పద్ధతి != CLIENTSERVER_పద్ధతి_X11)
	తిరిగి OK;
    make_connection();
    ఒకవేళ (X_DISPLAY == ఖాళీ)
    {
	emsg(_(e_no_connection_గమ్యం_x_server));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIENTSERVER
    స్థిర శూన్య
remote_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం సమాసం)
{
    char_u	*server_name;
    char_u	*కీలకాలు;
    char_u	*r = ఖాళీ;
    char_u	buf[NUMBUFLEN];
    పూర్ణాంకం		timeout = 0;
# ifdef MSWIN
    HWND	w;
# లేకపోతే
#  ifdef FEAT_X11
    కిటికీ	w;
#  endif
# endif
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

# ifdef FEAT_X11
    ఒకవేళ (check_connection() == FAIL)
	తిరిగి;
# endif
    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_సంఖ్య(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (server_name == ఖాళీ)
	తిరిగి;		// type error; errmsg already given
    కీలకాలు = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN
	    && server_Send_గమ్యం_Vim(server_name, కీలకాలు, &r, &w, సమాసం,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET
	&& socketserver_send(server_name, కీలకాలు, &r, సమాసం,
	    timeout * 1000, true, ఖాళీ) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
	ఒకవేళ (server_Send_గమ్యం_Vim(X_DISPLAY, server_name, కీలకాలు, &r, &w, సమాసం, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    ఒకవేళ (ret == FAIL)
    {
	ఒకవేళ (r != ఖాళీ)
	{
	    emsg((అక్షరం *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	లేకపోతే
	    semsg(_(e_unable_గమ్యం_send_గమ్యం_str), server_name);
	తిరిగి;
    }

    rettv->vval.v_string = r;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# ఒకవేళ defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ఖాళీ;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	ఒకవేళ (idvar != ఖాళీ && *idvar != NUL)
	{
# ifdef MSWIN
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
	    {
		sprintf((అక్షరం *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
	    {
		sprintf((అక్షరం *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
		str = server_name;
# endif
	    ఒకవేళ (str == ఖాళీ)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    సమితి_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    శూన్య
f_remote_సమాసం(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

# ifdef FEAT_CLIENTSERVER
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 3) == FAIL)))
	తిరిగి;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    శూన్య
f_remote_foreground(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_CLIENTSERVER
    ఒకవేళ (లోపల_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	ఒకవేళ (server_name != ఖాళీ)
	    serverForeground(server_name);
    }
#  లేకపోతే
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    శూన్య
f_remote_peek(typval_T *argvars వాడని, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ఖాళీ;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    serverid = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (serverid == ఖాళీ)
	తిరిగి;		// type error; errmsg already given
#  ifdef MSWIN
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
    {
	అక్షరశ్రేణి_చదువు((స్థిరరాశి అక్షరం *)serverid, SCANF_HEX_LONG_U, &n);
	ఒకవేళ (n == 0)
	    rettv->vval.v_సంఖ్య = -1;
	లేకపోతే
	{
	    s = server_Get_జవాబు((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_సంఖ్య = (s != ఖాళీ);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
	rettv->vval.v_సంఖ్య = socketserver_peek_జవాబు(serverid, &s);
#  endif
#  ifdef FEAT_X11
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
    {
	ఒకవేళ (check_connection() == FAIL)
	    తిరిగి;

	rettv->vval.v_సంఖ్య = server_Peek_జవాబు(X_DISPLAY,
		server_Str_గమ్యం_Win(serverid), &s);
    }
#  endif

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_సంఖ్య > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	ఒకవేళ (retvar != ఖాళీ)
	    సమితి_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# లేకపోతే
    rettv->vval.v_సంఖ్య = -1;
# endif
}

    శూన్య
f_remote_read(typval_T *argvars వాడని, typval_T *rettv)
{
    char_u	*r = ఖాళీ;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    serverid = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (serverid != ఖాళీ && !check_restricted() && !check_secure())
    {
	పూర్ణాంకం timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_సంఖ్య(&argvars[1]);

#  ifdef MSWIN
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
	{
	    అక్షరశ్రేణి_చదువు((అక్షరం *)serverid, SCANF_HEX_LONG_U, &n);
	    ఒకవేళ (n != 0)
		r = server_Get_జవాబు((HWND)n, FALSE, TRUE, TRUE, timeout);
	    ఒకవేళ (r == ఖాళీ)
		emsg(_(e_unable_గమ్యం_read_server_జవాబు));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET &&
		socketserver_read_జవాబు(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_గమ్యం_read_server_జవాబు));
#  endif
#  ifdef FEAT_X11
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11 &&
		(check_connection() == FAIL
		|| server_Read_జవాబు(X_DISPLAY, server_Str_గమ్యం_Win(serverid),
						       &r, FALSE, timeout) < 0))
	    emsg(_(e_unable_గమ్యం_read_server_జవాబు));
#  endif
    }
# endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = r;
}

/*
 * "remote_send()" function
 */
    శూన్య
f_remote_send(typval_T *argvars వాడని, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

# ifdef FEAT_CLIENTSERVER
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	తిరిగి;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    శూన్య
f_remote_startserver(typval_T *argvars వాడని, typval_T *rettv వాడని)
{
# ifdef FEAT_CLIENTSERVER
    ఒకవేళ (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (serverName != ఖాళీ)
    {
	emsg(_(e_already_started_server));
	తిరిగి;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    ఒకవేళ (server == ఖాళీ)
	తిరిగి;
#  ifdef MSWIN
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
	server_సమితి_Name(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
	socketserver_ప్రారంభం(server, true);
#  endif
#  ifdef FEAT_X11
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11 &&
	    check_connection() == OK)
	server_రిజిస్టరు_Name(X_DISPLAY, server);
#  endif

# లేకపోతే
    emsg(_(e_clientserver_feature_not_అందుబాటు));
# endif
}

    శూన్య
f_server2client(typval_T *argvars వాడని, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*జవాబు;
    పూర్ణాంకం		ret = OK;

    rettv->vval.v_సంఖ్య = -1;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    server = tv_get_string_chk(&argvars[0]);
    జవాబు = tv_get_string_buf_chk(&argvars[1], buf);
    ఒకవేళ (server == ఖాళీ || జవాబు == ఖాళీ)
	తిరిగి;

#  ifdef FEAT_SOCKETSERVER
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET &&
	    socketserver_send_జవాబు(server, జవాబు) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11 &&
	    server_Send_జవాబు(server, జవాబు) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN
	    && server_Send_జవాబు(server, జవాబు) < 0)
	ret = FAIL;
#  endif

    ఒకవేళ (ret == FAIL)
    {
	emsg(_(e_unable_గమ్యం_send_గమ్యం_client));
	తిరిగి;
    }
    rettv->vval.v_సంఖ్య = 0;
# లేకపోతే
    rettv->vval.v_సంఖ్య = -1;
# endif
}

    శూన్య
f_serverlist(typval_T *argvars వాడని, typval_T *rettv)
{
    list_T  *list = ఖాళీ;
    bool    use_list = false;

    ఒకవేళ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_MSWIN)
	list = server_Get_Vim_పేర్లు();
#  endif
#  ifdef FEAT_SOCKETSERVER
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11)
    {
	make_connection();
	ఒకవేళ (X_DISPLAY != ఖాళీ)
	    list = server_Get_Vim_పేర్లు(X_DISPLAY);
    }
#  endif
# endif
    ఒకవేళ (use_list)
    {
	ఒకవేళ (list == ఖాళీ)
	{
	    (శూన్య)rettv_list_alloc(rettv);
	    తిరిగి;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    లేకపోతే
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	ఒకవేళ (list != ఖాళీ)
	{
	    list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
	    ga_append(&ga, NUL);
	    list_free(list);
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = (char_u *)ga.ga_దత్తాంశం;
    }
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_CLIENTSERVER_BACKENDS
/*
 * Check the $VIM_CLIENTSERVER environment variable and set the method
 */
    శూన్య
check_clientserver_పద్ధతి_env(శూన్య)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    ఒకవేళ (env == ఖాళీ)
	తిరిగి;

    ఒకవేళ (STRICMP(env, "socket") == 0)
	clientserver_పద్ధతి = CLIENTSERVER_పద్ధతి_SOCKET;
# ifdef FEAT_X11
    లేకపోతే ఒకవేళ (STRICMP(env, "x11") == 0)
	clientserver_పద్ధతి = CLIENTSERVER_పద్ధతి_X11;
# endif
# ifdef MSWIN
    లేకపోతే ఒకవేళ (STRICMP(env, "mswin") == 0)
	clientserver_పద్ధతి = CLIENTSERVER_పద్ధతి_MSWIN;
# endif
}
#అయితే_ముగింపు
