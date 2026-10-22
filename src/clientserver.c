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

#ಆದರೆ defined(FEAT_CLIENTSERVER)

ಸ್ಥಿರ ಶೂನ್ಯ cmdsrv_main(ಪೂರ್ಣಾಂಕ *argc, ಅಕ್ಷರ **argv, char_u *serverName_arg, char_u **serverStr);
ಸ್ಥಿರ char_u *serverMakeName(char_u *arg, ಅಕ್ಷರ *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    ಶೂನ್ಯ
server_ಗಮ್ಯ_ಒಳಹರಿವು_buf(char_u *str)
{
    char_u      *ptr = ಖಾಲಿ;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ಖಾಲಿ);
    p_cpo = cpo_save;

    ಆದರೆ (ptr != ಖಾಲಿ && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	ಆದರೆ (typebuf.tb_maplen < typebuf.tb_len)
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
eval_client_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_string(char_u *ಅಭಿವ್ಯಕ್ತಿ)
{
    char_u	*res;
    ಪೂರ್ಣಾಂಕ		save_dbl = debug_break_level;
    ಪೂರ್ಣಾಂಕ		save_ro = redir_off;
    funccal_ನಮೂದು_T funccal_ನಮೂದು;
    ಪೂರ್ಣಾಂಕ		did_save_funccal = FALSE;

# ಆದರೆ defined(FEAT_EVAL)
    ch_log(ಖಾಲಿ, "eval_client_expr_to_string(\"%s\")", ಅಭಿವ್ಯಕ್ತಿ);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    ಆದರೆ (!debug_ಬಹುಲಕ)
    {
	save_funccal(&funccal_ನಮೂದು);
	did_save_funccal = TRUE;
    }

     // Disable debugging, otherwise Vim hangs, waiting for "cont" to be
     // typed.
    debug_break_level = -1;
    redir_off = 0;
    // Do not display error message, otherwise Vim hangs, waiting for "cont"
    // to be typed.  Do generate errors so that try/catch works.
    ++emsg_silent;

    res = eval_ಗಮ್ಯ_string(ಅಭಿವ್ಯಕ್ತಿ, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    ಆದರೆ (emsg_silent < 0)
	emsg_silent = 0;
    ಆದರೆ (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    ಹೊರಹೋಗು_flush_cursor(FALSE, FALSE);

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    ಪೂರ್ಣಾಂಕ
send_ಗಮ್ಯ_Local_Vim(char_u *cmd, ಪೂರ್ಣಾಂಕ as_ಅಭಿವ್ಯಕ್ತಿ, char_u **ಫಲಿತಾಂಶ)
{
    ಆದರೆ (as_ಅಭಿವ್ಯಕ್ತಿ)
    {
	char_u *ret;

	ret = eval_client_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_string(cmd);
	ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ)
	{
	    ಆದರೆ (ret == ಖಾಲಿ)
	    {
		ಅಕ್ಷರ	*err = _(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*ಸಂದೇಶ;

		ಸಂದೇಶ = alloc(len);
		ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
		    vim_snprintf((ಅಕ್ಷರ *)ಸಂದೇಶ, len, "%s: \"%s\"", err, cmd);
		*ಫಲಿತಾಂಶ = ಸಂದೇಶ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*ಫಲಿತಾಂಶ = ret;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(ret);
	ಹಿಂತಿರುಗಿ ret == ಖಾಲಿ ? -1 : 0;
    }
    server_ಗಮ್ಯ_ಒಳಹರಿವು_buf(cmd);
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * If conversion is needed, convert "data" from "client_enc" to 'encoding' and
 * return an allocated string.  Otherwise return "data".
 * "*tofree" is set to the result when it needs to be freed later.
 */
    char_u *
serverConvert(
    char_u *client_enc ಬಳಸದ,
    char_u *ದತ್ತಾಂಶ,
    char_u **tofree)
{
    char_u	*res = ದತ್ತಾಂಶ;

    *tofree = ಖಾಲಿ;
    ಆದರೆ (client_enc == ಖಾಲಿ || p_enc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ಆದರೆ (convert_ಸಿದ್ಧತೆ(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, ದತ್ತಾಂಶ, ಖಾಲಿ);
	ಆದರೆ (res == ಖಾಲಿ)
	    res = ದತ್ತಾಂಶ;
	ಇಲ್ಲದಿದ್ದರೆ
	    *tofree = res;
    }
    convert_ಸಿದ್ಧತೆ(&vimconv, ಖಾಲಿ, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ res;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ಸ್ಥಿರ char_u *build_drop_cmd(ಪೂರ್ಣಾಂಕ filec, ಅಕ್ಷರ **filev, ಪೂರ್ಣಾಂಕ tabs, ಪೂರ್ಣಾಂಕ send_ಉತ್ತರ);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    ಶೂನ್ಯ
exec_on_server(mparm_T *parmp)
{
    ಪೂರ್ಣಾಂಕ made_name = FALSE;

    ಆದರೆ (parmp->serverName_arg != ಖಾಲಿ && *parmp->serverName_arg == NUL)
	ಹಿಂತಿರುಗಿ;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    ಆದರೆ (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    ಆದರೆ (!made_name && parmp->servername == ಖಾಲಿ)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    ಆದರೆ (parmp->servername != ಖಾಲಿ && clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
    {
	server_ಗಣ_Name(parmp->servername);
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
# ಆದರೆ defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    ಆದರೆ (
#  ಆದರೆ defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ಖಾಲಿ
#   endif
#   ಆದರೆ defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   ಆದರೆ defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ಖಾಲಿ && (
#  ಆದರೆ defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   ಆದರೆ defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_ವಿಧಾನ != CLIENTSERVER_ವಿಧಾನ_SOCKET &&
#   endif
#   ಆದರೆ defined(FEAT_AUTOSERVERNAME)
		    1
#   ಇಲ್ಲದಿದ್ದರೆ
		    gui.ಒಳ_use
#   endif
#   ifdef UNIX
		 && getuid() != ಬೇರು_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ಖಾಲಿ))
    {

#  ifdef FEAT_SOCKETSERVER
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
	    socketserver_ಆರಂಭ(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
	{
	    (ಶೂನ್ಯ)server_ರಿಜಿಸ್ಟರ್_Name(X_DISPLAY, parmp->servername);
	    ಸಮಯ_ಸಂದೇಶ("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
#  ifdef FEAT_X11
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
	    server_Delayed_ಆರಂಭ_Name = parmp->servername;
	ಇಲ್ಲದಿದ್ದರೆ
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    ಆದರೆ (parmp->serverStr != ಖಾಲಿ)
    {
	char_u *p;

	server_ಗಮ್ಯ_ಒಳಹರಿವು_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
cmdsrv_main(
    ಪೂರ್ಣಾಂಕ		*argc,
    ಅಕ್ಷರ	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*sname;
    ಪೂರ್ಣಾಂಕ		ret = -1;
    ಪೂರ್ಣಾಂಕ		didone = FALSE;
    ಪೂರ್ಣಾಂಕ		exiterr = 0;
    ಅಕ್ಷರ	**newArgV = argv + 1;
    ಪೂರ್ಣಾಂಕ		newArgC = 1,
		Argc = *argc;
    ಪೂರ್ಣಾಂಕ		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    ಪೂರ್ಣಾಂಕ		silent = FALSE;
    ಪೂರ್ಣಾಂಕ		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    ಕಿಟಕಿ	srv;

    ಸಿದ್ಧತೆ_ಪದ_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ಖಾಲಿ;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    ಆದರೆ (sname == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    ಗಾಗಿ (i = 1; i < Argc; i++)
    {
	res = ಖಾಲಿ;
	ಆದರೆ (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    ಗಾಗಿ (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    ಅಕ್ಷರ	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    ಆಗ (*p != NUL)
	    {
		ಆದರೆ (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    argtype = ARGTYPE_OTHER;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    argtype = ARGTYPE_OTHER;

	ಆದರೆ (argtype != ARGTYPE_OTHER)
	{
	    ಆದರೆ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ಆದರೆ (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		ಆದರೆ (*serverStr == ಖಾಲಿ)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    ನಿಲ್ಲಿಸು;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
		ret = socketserver_send(sname, *serverStr, ಖಾಲಿ, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
	    {
		ಆದರೆ (xterm_dpy == ಖಾಲಿ)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    ret = server_Send_ಗಮ್ಯ_Vim(xterm_dpy, sname, *serverStr,
			    ಖಾಲಿ, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
		// Win32 always works?
		ret = server_Send_ಗಮ್ಯ_Vim(sname, *serverStr, ಖಾಲಿ, &srv, 0, 0,
			silent);
# endif
	    ಆದರೆ (ret < 0)
	    {
		ಆದರೆ (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		ನಿಲ್ಲಿಸು;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    ಆದರೆ (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		ಗಣ_Foreground_ಕಿಟಕಿ(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    ಆದರೆ (argtype == ARGTYPE_EDIT_WAIT)
	    {
		ಪೂರ್ಣಾಂಕ	ಅಂಕೆ_Files = *argc - i - 1;
		char_u  *ಮುಗಿದ = alloc(ಅಂಕೆ_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2 = 0;
		ಬಾಹ್ಯ HWND ಸಂದೇಶ_ಕಿಟಕಿ;
# endif

		ಆದರೆ (ಅಂಕೆ_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --ಅಂಕೆ_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cb_ಗಾತ್ರ = ಗಾತ್ರ(ni);
		ni.hWnd = ಸಂದೇಶ_ಕಿಟಕಿ;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = ತುಂಬು_Icon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), ಎಣಿಕೆ_2, ಅಂಕೆ_Files);
		Shell_NotifyIcon(NIM_ಕೂಡಿಸು, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(ಮುಗಿದ, 0, ಅಂಕೆ_Files);
		ಆಗ (memchr(ಮುಗಿದ, 0, ಅಂಕೆ_Files) != ಖಾಲಿ)
		{
		    char_u  *p = ಖಾಲಿ;
		    ಪೂರ್ಣಾಂಕ	    j;
# ifdef MSWIN
		    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
			p = server_Get_ಉತ್ತರ(srv, ಖಾಲಿ, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET
			    && (ch == ಖಾಲಿ
				|| socketserver_read_ಉತ್ತರ(sname, &p, -1, true)
				== FAIL))
			    ನಿಲ್ಲಿಸು;
# endif
# ifdef FEAT_X11
		    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11
			    && server_Read_ಉತ್ತರ(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    ನಿಲ್ಲಿಸು;
# endif
		    ಆದರೆ (p == ಖಾಲಿ)
			ನಿಲ್ಲಿಸು;
		    j = atoi((ಅಕ್ಷರ *)p);
		    vim_free(p);
		    ಆದರೆ (j >= 0 && j < ಅಂಕೆ_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++ಎಣಿಕೆ_2;
			sprintf(ni.szTip, _("%d of %d edited"),
							     ಎಣಿಕೆ_2, ಅಂಕೆ_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			ಮುಗಿದ[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		ಆದರೆ (ch != ಖಾಲಿ)
		{
		    channel_close(ch, false);
		    channel_clear(ch);
		}
# endif
# ifdef FEAT_GUI_MSWIN
		Shell_NotifyIcon(NIM_DELETE, &ni);
# endif
		vim_free(ಮುಗಿದ);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = OK;

	    ಆದರೆ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN
		    && server_Send_ಗಮ್ಯ_Vim(sname, (char_u *)argv[i + 1],
						  &res, ಖಾಲಿ, 1, 0, FALSE) < 0)
		ಸ್ಥಿತಿ_2 = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ಖಾಲಿ) < 0)
		ಸ್ಥಿತಿ_2 = FAIL;
# endif
# ifdef FEAT_X11
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
	    {
		ಆದರೆ (xterm_dpy == ಖಾಲಿ)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (server_Send_ಗಮ್ಯ_Vim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ಖಾಲಿ, 1, 0, 1, FALSE) < 0)
		    ಸ್ಥಿತಿ_2 = FAIL;
	    }
# endif
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
	    {
		ಆದರೆ (res != ಖಾಲಿ && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((ಅಕ್ಷರ *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ಖಾಲಿ;
	    garray_T	ga;

# ifdef MSWIN
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
		// Win32 always works?
		list = server_Get_Vim_ಹೆಸರುಗಳು();
# endif
# ifdef FEAT_SOCKETSERVER
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11 &&
		    xterm_dpy != ಖಾಲಿ)
		list = server_Get_Vim_ಹೆಸರುಗಳು(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    ಆದರೆ (list != ಖಾಲಿ)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_ದತ್ತಾಂಶ;

	    ಆದರೆ (did_emsg)
		mch_errmsg("\n");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    ಮುಂದುವರಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    ಮುಂದುವರಿ;
	}
	didone = TRUE;
	ಆದರೆ (res != ಖಾಲಿ && *res != NUL)
	{
	    mch_ಸಂದೇಶ((ಅಕ್ಷರ *)res);
	    ಆದರೆ (res[STRLEN(res) - 1] != '\n')
		mch_ಸಂದೇಶ("\n");
	}
	vim_free(res);
    }

    ಆದರೆ (didone)
    {
	display_errors();	// display any collected messages
	ನಿರ್ಗಮಿಸು(exiterr);	// Mission accomplished - get out
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
    ಪೂರ್ಣಾಂಕ		filec,
    ಅಕ್ಷರ	**filev,
    ಪೂರ್ಣಾಂಕ		tabs,		// Use ":tab drop" instead of ":drop".
    ಪೂರ್ಣಾಂಕ		send_ಉತ್ತರ)
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*inicmd = ಖಾಲಿ;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ಸ್ಥಿರಾಂಕ string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    ಆದರೆ (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    ಆದರೆ (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    ಆದರೆ (cwd == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_ಒಳ_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# ಇಲ್ಲದಿದ್ದರೆ
	    ಮಾರ್ಗ_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    ಆದರೆ (cdp.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    cdp.ಉದ್ದಳತೆ = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.ಉದ್ದಳತೆ);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].ಉದ್ದಳತೆ);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    ಆದರೆ (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    ಗಾಗಿ (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		ಮಾರ್ಗ_ESC_CHARS
# ಇಲ್ಲದಿದ್ದರೆ
		(char_u *)" \t%#"
# endif
		);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_ದತ್ತಾಂಶ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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
# ಇಲ್ಲದಿದ್ದರೆ
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.ಉದ್ದಳತೆ);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].ಉದ್ದಳತೆ);

    ಆದರೆ (send_ಉತ್ತರ)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    ಆದರೆ (inicmd != ಖಾಲಿ)
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
    ಹಿಂತಿರುಗಿ ga.ga_ದತ್ತಾಂಶ;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    ಸ್ಥಿರ char_u *
serverMakeName(char_u *arg, ಅಕ್ಷರ *cmd)
{
    char_u *p;

    ಆದರೆ (arg != ಖಾಲಿ && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
	{
	    ಆದರೆ (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		p = vim_strsave_ಮೇಲೆ(arg);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    p = vim_strsave_ಮೇಲೆ(arg);
# ಇಲ್ಲದಿದ್ದರೆ
	p = vim_strsave_ಮೇಲೆ(arg);
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	p = vim_strsave_ಮೇಲೆ(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	ಆದರೆ (p != ಖಾಲಿ && vim_strchr(p, '.') != ಖಾಲಿ)
	    *vim_strchr(p, '.') = NUL;
    }
    ಹಿಂತಿರುಗಿ p;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_CLIENTSERVER

#ಆದರೆ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ಸ್ಥಿರ ಶೂನ್ಯ
make_connection(ಶೂನ್ಯ)
{
    ಆದರೆ (X_DISPLAY == ಖಾಲಿ
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
    {
	x_force_connect = TRUE;
	ಸಿದ್ಧತೆ_ಪದ_clip();
	x_force_connect = FALSE;
    }
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_connection(ಶೂನ್ಯ)
{
    ಆದರೆ (clientserver_ವಿಧಾನ != CLIENTSERVER_ವಿಧಾನ_X11)
	ಹಿಂತಿರುಗಿ OK;
    make_connection();
    ಆದರೆ (X_DISPLAY == ಖಾಲಿ)
    {
	emsg(_(e_no_connection_ಗಮ್ಯ_x_server));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
    ಸ್ಥಿರ ಶೂನ್ಯ
remote_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ ಅಭಿವ್ಯಕ್ತಿ)
{
    char_u	*server_name;
    char_u	*ಕೀಲಿಗಳು;
    char_u	*r = ಖಾಲಿ;
    char_u	buf[NUMBUFLEN];
    ಪೂರ್ಣಾಂಕ		timeout = 0;
# ifdef MSWIN
    HWND	w;
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef FEAT_X11
    ಕಿಟಕಿ	w;
#  endif
# endif
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

# ifdef FEAT_X11
    ಆದರೆ (check_connection() == FAIL)
	ಹಿಂತಿರುಗಿ;
# endif
    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_ಸಂಖ್ಯೆ(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (server_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
    ಕೀಲಿಗಳು = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN
	    && server_Send_ಗಮ್ಯ_Vim(server_name, ಕೀಲಿಗಳು, &r, &w, ಅಭಿವ್ಯಕ್ತಿ,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET
	&& socketserver_send(server_name, ಕೀಲಿಗಳು, &r, ಅಭಿವ್ಯಕ್ತಿ,
	    timeout * 1000, true, ಖಾಲಿ) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
	ಆದರೆ (server_Send_ಗಮ್ಯ_Vim(X_DISPLAY, server_name, ಕೀಲಿಗಳು, &r, &w, ಅಭಿವ್ಯಕ್ತಿ, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    ಆದರೆ (ret == FAIL)
    {
	ಆದರೆ (r != ಖಾಲಿ)
	{
	    emsg((ಅಕ್ಷರ *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_unable_ಗಮ್ಯ_send_ಗಮ್ಯ_str), server_name);
	ಹಿಂತಿರುಗಿ;
    }

    rettv->vval.v_string = r;

    ಆದರೆ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# ಆದರೆ defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ಖಾಲಿ;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	ಆದರೆ (idvar != ಖಾಲಿ && *idvar != NUL)
	{
# ifdef MSWIN
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
	    {
		sprintf((ಅಕ್ಷರ *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
	    {
		sprintf((ಅಕ್ಷರ *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
		str = server_name;
# endif
	    ಆದರೆ (str == ಖಾಲಿ)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    ಗಣ_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    ಶೂನ್ಯ
f_remote_ಅಭಿವ್ಯಕ್ತಿ(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

# ifdef FEAT_CLIENTSERVER
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 3) == FAIL)))
	ಹಿಂತಿರುಗಿ;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    ಶೂನ್ಯ
f_remote_foreground(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_CLIENTSERVER
    ಆದರೆ (ಒಳ_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	ಆದರೆ (server_name != ಖಾಲಿ)
	    serverForeground(server_name);
    }
#  ಇಲ್ಲದಿದ್ದರೆ
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    ಶೂನ್ಯ
f_remote_peek(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ಖಾಲಿ;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    serverid = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (serverid == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;		// type error; errmsg already given
#  ifdef MSWIN
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
    {
	ಅಕ್ಷರಸರಣಿ_ಓದು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)serverid, SCANF_HEX_LONG_U, &n);
	ಆದರೆ (n == 0)
	    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    s = server_Get_ಉತ್ತರ((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_ಸಂಖ್ಯೆ = (s != ಖಾಲಿ);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
	rettv->vval.v_ಸಂಖ್ಯೆ = socketserver_peek_ಉತ್ತರ(serverid, &s);
#  endif
#  ifdef FEAT_X11
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
    {
	ಆದರೆ (check_connection() == FAIL)
	    ಹಿಂತಿರುಗಿ;

	rettv->vval.v_ಸಂಖ್ಯೆ = server_Peek_ಉತ್ತರ(X_DISPLAY,
		server_Str_ಗಮ್ಯ_Win(serverid), &s);
    }
#  endif

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_ಸಂಖ್ಯೆ > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	ಆದರೆ (retvar != ಖಾಲಿ)
	    ಗಣ_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# endif
}

    ಶೂನ್ಯ
f_remote_read(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    char_u	*r = ಖಾಲಿ;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ಸಂಖ್ಯೆ_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    serverid = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (serverid != ಖಾಲಿ && !check_restricted() && !check_secure())
    {
	ಪೂರ್ಣಾಂಕ timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_ಸಂಖ್ಯೆ(&argvars[1]);

#  ifdef MSWIN
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
	{
	    ಅಕ್ಷರಸರಣಿ_ಓದು((ಅಕ್ಷರ *)serverid, SCANF_HEX_LONG_U, &n);
	    ಆದರೆ (n != 0)
		r = server_Get_ಉತ್ತರ((HWND)n, FALSE, TRUE, TRUE, timeout);
	    ಆದರೆ (r == ಖಾಲಿ)
		emsg(_(e_unable_ಗಮ್ಯ_read_server_ಉತ್ತರ));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET &&
		socketserver_read_ಉತ್ತರ(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_ಗಮ್ಯ_read_server_ಉತ್ತರ));
#  endif
#  ifdef FEAT_X11
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11 &&
		(check_connection() == FAIL
		|| server_Read_ಉತ್ತರ(X_DISPLAY, server_Str_ಗಮ್ಯ_Win(serverid),
						       &r, FALSE, timeout) < 0))
	    emsg(_(e_unable_ಗಮ್ಯ_read_server_ಉತ್ತರ));
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
f_remote_send(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

# ifdef FEAT_CLIENTSERVER
    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    ಶೂನ್ಯ
f_remote_startserver(typval_T *argvars ಬಳಸದ, typval_T *rettv ಬಳಸದ)
{
# ifdef FEAT_CLIENTSERVER
    ಆದರೆ (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (serverName != ಖಾಲಿ)
    {
	emsg(_(e_already_started_server));
	ಹಿಂತಿರುಗಿ;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    ಆದರೆ (server == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
#  ifdef MSWIN
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
	server_ಗಣ_Name(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
	socketserver_ಆರಂಭ(server, true);
#  endif
#  ifdef FEAT_X11
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11 &&
	    check_connection() == OK)
	server_ರಿಜಿಸ್ಟರ್_Name(X_DISPLAY, server);
#  endif

# ಇಲ್ಲದಿದ್ದರೆ
    emsg(_(e_clientserver_feature_not_ಲಭ್ಯ));
# endif
}

    ಶೂನ್ಯ
f_server2client(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*ಉತ್ತರ;
    ಪೂರ್ಣಾಂಕ		ret = OK;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    server = tv_get_string_chk(&argvars[0]);
    ಉತ್ತರ = tv_get_string_buf_chk(&argvars[1], buf);
    ಆದರೆ (server == ಖಾಲಿ || ಉತ್ತರ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#  ifdef FEAT_SOCKETSERVER
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET &&
	    socketserver_send_ಉತ್ತರ(server, ಉತ್ತರ) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11 &&
	    server_Send_ಉತ್ತರ(server, ಉತ್ತರ) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN
	    && server_Send_ಉತ್ತರ(server, ಉತ್ತರ) < 0)
	ret = FAIL;
#  endif

    ಆದರೆ (ret == FAIL)
    {
	emsg(_(e_unable_ಗಮ್ಯ_send_ಗಮ್ಯ_client));
	ಹಿಂತಿರುಗಿ;
    }
    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
# ಇಲ್ಲದಿದ್ದರೆ
    rettv->vval.v_ಸಂಖ್ಯೆ = -1;
# endif
}

    ಶೂನ್ಯ
f_serverlist(typval_T *argvars ಬಳಸದ, typval_T *rettv)
{
    list_T  *list = ಖಾಲಿ;
    bool    use_list = false;

    ಆದರೆ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_MSWIN)
	list = server_Get_Vim_ಹೆಸರುಗಳು();
#  endif
#  ifdef FEAT_SOCKETSERVER
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11)
    {
	make_connection();
	ಆದರೆ (X_DISPLAY != ಖಾಲಿ)
	    list = server_Get_Vim_ಹೆಸರುಗಳು(X_DISPLAY);
    }
#  endif
# endif
    ಆದರೆ (use_list)
    {
	ಆದರೆ (list == ಖಾಲಿ)
	{
	    (ಶೂನ್ಯ)rettv_list_alloc(rettv);
	    ಹಿಂತಿರುಗಿ;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	ಆದರೆ (list != ಖಾಲಿ)
	{
	    list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
	    ga_append(&ga, NUL);
	    list_free(list);
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = (char_u *)ga.ga_ದತ್ತಾಂಶ;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER_BACKENDS
/*
 * Check the $VIM_CLIENTSERVER environment variable and set the method
 */
    ಶೂನ್ಯ
check_clientserver_ವಿಧಾನ_env(ಶೂನ್ಯ)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    ಆದರೆ (env == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (STRICMP(env, "socket") == 0)
	clientserver_ವಿಧಾನ = CLIENTSERVER_ವಿಧಾನ_SOCKET;
# ifdef FEAT_X11
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(env, "x11") == 0)
	clientserver_ವಿಧಾನ = CLIENTSERVER_ವಿಧಾನ_X11;
# endif
# ifdef MSWIN
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP(env, "mswin") == 0)
	clientserver_ವಿಧಾನ = CLIENTSERVER_ವಿಧಾನ_MSWIN;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ
