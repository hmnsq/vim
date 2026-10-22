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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_CLIENTSERVER)

ସ୍ଥିର ଶୂନ୍ୟ cmdsrv_main(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *argc, ଅକ୍ଷର **argv, char_u *serverName_arg, char_u **serverStr);
ସ୍ଥିର char_u *serverMakeName(char_u *arg, ଅକ୍ଷର *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    ଶୂନ୍ୟ
server_to_ନିବେଶ_buf(char_u *str)
{
    char_u      *ptr = ଖାଲି;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, ଖାଲି);
    p_cpo = cpo_save;

    ଯଦି (ptr != ଖାଲି && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	ଯଦି (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(ଶୂନ୍ୟ)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_ପଦସମଷ୍ଟି_to_string(char_u *ପଦସମଷ୍ଟି)
{
    char_u	*res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_dbl = debug_break_level;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_save_funccal = FALSE;

# ଯଦି defined(FEAT_EVAL)
    ch_log(ଖାଲି, "eval_client_expr_to_string(\"%s\")", ପଦସମଷ୍ଟି);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    ଯଦି (!debug_mode)
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

    res = eval_to_string(ପଦସମଷ୍ଟି, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    ଯଦି (emsg_silent < 0)
	emsg_silent = 0;
    ଯଦି (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    ଫେରନ୍ତୁ res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sendToLocalVim(char_u *cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା as_ପଦସମଷ୍ଟି, char_u **result)
{
    ଯଦି (as_ପଦସମଷ୍ଟି)
    {
	char_u *ret;

	ret = eval_client_ପଦସମଷ୍ଟି_to_string(cmd);
	ଯଦି (result != ଖାଲି)
	{
	    ଯଦି (ret == ଖାଲି)
	    {
		ଅକ୍ଷର	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*ସନ୍ଦେଶ;

		ସନ୍ଦେଶ = alloc(len);
		ଯଦି (ସନ୍ଦେଶ != ଖାଲି)
		    vim_snprintf((ଅକ୍ଷର *)ସନ୍ଦେଶ, len, "%s: \"%s\"", err, cmd);
		*result = ସନ୍ଦେଶ;
	    }
	    ଅନ୍ୟଥା
		*result = ret;
	}
	ଅନ୍ୟଥା
	    vim_free(ret);
	ଫେରନ୍ତୁ ret == ଖାଲି ? -1 : 0;
    }
    server_to_ନିବେଶ_buf(cmd);
    ଫେରନ୍ତୁ 0;
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

    *tofree = ଖାଲି;
    ଯଦି (client_enc == ଖାଲି || p_enc == ଖାଲି)
	ଫେରନ୍ତୁ res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ଯଦି (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, ଖାଲି);
	ଯଦି (res == ଖାଲି)
	    res = data;
	ଅନ୍ୟଥା
	    *tofree = res;
    }
    convert_setup(&vimconv, ଖାଲି, ଖାଲି);
    ଫେରନ୍ତୁ res;
}
#endif

#ଯଦି defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ସ୍ଥିର char_u *build_drop_cmd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା filec, ଅକ୍ଷର **filev, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tabs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    ଶୂନ୍ୟ
exec_on_server(mparm_T *parmp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା made_name = FALSE;

    ଯଦି (parmp->serverName_arg != ଖାଲି && *parmp->serverName_arg == NUL)
	ଫେରନ୍ତୁ;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    ଯଦି (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    ଯଦି (!made_name && parmp->servername == ଖାଲି)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    ଯଦି (parmp->servername != ଖାଲି && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    ଶୂନ୍ୟ
prepare_server(mparm_T *parmp)
{
# ଯଦି defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    ଯଦି (
#  ଯଦି defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != ଖାଲି
#   endif
#   ଯଦି defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   ଯଦି defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != ଖାଲି && (
#  ଯଦି defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   ଯଦି defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   ଯଦି defined(FEAT_AUTOSERVERNAME)
		    1
#   ଅନ୍ୟଥା
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != ମୂଳ_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != ଖାଲି))
    {

#  ifdef FEAT_SOCKETSERVER
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (ଶୂନ୍ୟ)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_ସନ୍ଦେଶ("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    ଅନ୍ୟଥା
    {
#  ifdef FEAT_X11
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	ଅନ୍ୟଥା
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    ଯଦି (parmp->serverStr != ଖାଲି)
    {
	char_u *p;

	server_to_ନିବେଶ_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
cmdsrv_main(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*argc,
    ଅକ୍ଷର	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*sname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		didone = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exiterr = 0;
    ଅକ୍ଷର	**newArgV = argv + 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		newArgC = 1,
		Argc = *argc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		silent = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = ଖାଲି;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    ଯଦି (sname == ଖାଲି)
	ଫେରନ୍ତୁ;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    ପାଇଁ (i = 1; i < Argc; i++)
    {
	res = ଖାଲି;
	ଯଦି (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    ପାଇଁ (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    ଭାଙ୍ଗ;
	}

	ଯଦି (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	ଅନ୍ୟଥା ଯଦି (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    ଅକ୍ଷର	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    ଯେତେବେଳେ (*p != NUL)
	    {
		ଯଦି (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		ଅନ୍ୟଥା ଯଦି (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		ଅନ୍ୟଥା
		{
		    argtype = ARGTYPE_OTHER;
		    ଭାଙ୍ଗ;
		}
	    }
	}
	ଅନ୍ୟଥା
	    argtype = ARGTYPE_OTHER;

	ଯଦି (argtype != ARGTYPE_OTHER)
	{
	    ଯଦି (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ଯଦି (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    ଅନ୍ୟଥା
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		ଯଦି (*serverStr == ଖାଲି)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    ଭାଙ୍ଗ;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, ଖାଲି, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		ଯଦି (xterm_dpy == ଖାଲି)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		ଅନ୍ୟଥା
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    ଖାଲି, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, ଖାଲି, &srv, 0, 0,
			silent);
# endif
	    ଯଦି (ret < 0)
	    {
		ଯଦି (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		ଅନ୍ୟଥା ଯଦି (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		ଭାଙ୍ଗ;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    ଯଦି (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    ଯଦି (argtype == ARGTYPE_EDIT_WAIT)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଅଙ୍କ_Files = *argc - i - 1;
		char_u  *done = alloc(ଅଙ୍କ_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ଗଣନା_2 = 0;
		ବାହ୍ୟ HWND message_window;
# endif

		ଯଦି (ଅଙ୍କ_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --ଅଙ୍କ_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = ଆକାର(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), ଗଣନା_2, ଅଙ୍କ_Files);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, ଅଙ୍କ_Files);
		ଯେତେବେଳେ (memchr(done, 0, ଅଙ୍କ_Files) != ଖାଲି)
		{
		    char_u  *p = ଖାଲି;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    j;
# ifdef MSWIN
		    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, ଖାଲି, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == ଖାଲି
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    ଭାଙ୍ଗ;
# endif
# ifdef FEAT_X11
		    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    ଭାଙ୍ଗ;
# endif
		    ଯଦି (p == ଖାଲି)
			ଭାଙ୍ଗ;
		    j = atoi((ଅକ୍ଷର *)p);
		    vim_free(p);
		    ଯଦି (j >= 0 && j < ଅଙ୍କ_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++ଗଣନା_2;
			sprintf(ni.szTip, _("%d of %d edited"),
							     ଗଣନା_2, ଅଙ୍କ_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			done[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		ଯଦି (ch != ଖାଲି)
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
	ଅନ୍ୟଥା ଯଦି (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = OK;

	    ଯଦି (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, ଖାଲି, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, ଖାଲି) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		ଯଦି (xterm_dpy == ଖାଲି)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		ଅନ୍ୟଥା ଯଦି (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    ଖାଲି, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    ଯଦି (status == FAIL)
	    {
		ଯଦି (res != ଖାଲି && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((ଅକ୍ଷର *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	ଅନ୍ୟଥା ଯଦି (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = ଖାଲି;
	    garray_T	ga;

# ifdef MSWIN
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != ଖାଲି)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    ଯଦି (list != ଖାଲି)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    ଯଦି (did_emsg)
		mch_errmsg("\n");
	}
	ଅନ୍ୟଥା ଯଦି (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    ଜାରି;
	}
	ଅନ୍ୟଥା
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    ଜାରି;
	}
	didone = TRUE;
	ଯଦି (res != ଖାଲି && *res != NUL)
	{
	    mch_ସନ୍ଦେଶ((ଅକ୍ଷର *)res);
	    ଯଦି (res[STRLEN(res) - 1] != '\n')
		mch_ସନ୍ଦେଶ("\n");
	}
	vim_free(res);
    }

    ଯଦି (didone)
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
    ସ୍ଥିର char_u *
build_drop_cmd(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		filec,
    ଅକ୍ଷର	**filev,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tabs,		// Use ":tab drop" instead of ":drop".
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sendReply)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*inicmd = ଖାଲି;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    ଅଚଳ string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    ଯଦି (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    ଯଦି (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    ଯଦି (cwd == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# ଅନ୍ୟଥା
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    ଯଦି (cdp.string == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    ଯଦି (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    ପାଇଁ (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# ଅନ୍ୟଥା
		(char_u *)" \t%#"
# endif
		);
	ଯଦି (p == ଖାଲି)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    ଫେରନ୍ତୁ ଖାଲି;
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
# ଅନ୍ୟଥା
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    ଯଦି (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    ଯଦି (inicmd != ଖାଲି)
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
    ଫେରନ୍ତୁ ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    ସ୍ଥିର char_u *
serverMakeName(char_u *arg, ଅକ୍ଷର *cmd)
{
    char_u *p;

    ଯଦି (arg != ଖାଲି && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    ଯଦି (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    ଅନ୍ୟଥା
		p = vim_strsave_up(arg);
	}
	ଅନ୍ୟଥା
	    p = vim_strsave_up(arg);
# ଅନ୍ୟଥା
	p = vim_strsave_up(arg);
# endif
    }
    ଅନ୍ୟଥା
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	ଯଦି (p != ଖାଲି && vim_strchr(p, '.') != ଖାଲି)
	    *vim_strchr(p, '.') = NUL;
    }
    ଫେରନ୍ତୁ p;
}
#endif // FEAT_CLIENTSERVER

#ଯଦି defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ସ୍ଥିର ଶୂନ୍ୟ
make_connection(ଶୂନ୍ୟ)
{
    ଯଦି (X_DISPLAY == ଖାଲି
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

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_connection(ଶୂନ୍ୟ)
{
    ଯଦି (clientserver_method != CLIENTSERVER_METHOD_X11)
	ଫେରନ୍ତୁ OK;
    make_connection();
    ଯଦି (X_DISPLAY == ଖାଲି)
    {
	emsg(_(e_no_connection_to_x_server));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    ସ୍ଥିର ଶୂନ୍ୟ
remote_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ପଦସମଷ୍ଟି)
{
    char_u	*server_name;
    char_u	*ଚାବିଗୁଡ଼ିକ;
    char_u	*r = ଖାଲି;
    char_u	buf[NUMBUFLEN];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timeout = 0;
# ifdef MSWIN
    HWND	w;
# ଅନ୍ୟଥା
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

# ifdef FEAT_X11
    ଯଦି (check_connection() == FAIL)
	ଫେରନ୍ତୁ;
# endif
    ଯଦି (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_ସଂଖ୍ୟା(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    ଯଦି (server_name == ଖାଲି)
	ଫେରନ୍ତୁ;		// type error; errmsg already given
    ଚାବିଗୁଡ଼ିକ = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, ଚାବିଗୁଡ଼ିକ, &r, &w, ପଦସମଷ୍ଟି,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, ଚାବିଗୁଡ଼ିକ, &r, ପଦସମଷ୍ଟି,
	    timeout * 1000, true, ଖାଲି) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
	ଯଦି (serverSendToVim(X_DISPLAY, server_name, ଚାବିଗୁଡ଼ିକ, &r, &w, ପଦସମଷ୍ଟି, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    ଯଦି (ret == FAIL)
    {
	ଯଦି (r != ଖାଲି)
	{
	    emsg((ଅକ୍ଷର *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	ଅନ୍ୟଥା
	    semsg(_(e_unable_to_send_to_str), server_name);
	ଫେରନ୍ତୁ;
    }

    rettv->vval.v_string = r;

    ଯଦି (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# ଯଦି defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = ଖାଲି;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	ଯଦି (idvar != ଖାଲି && *idvar != NUL)
	{
# ifdef MSWIN
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((ଅକ୍ଷର *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((ଅକ୍ଷର *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    ଯଦି (str == ଖାଲି)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#ଯଦି defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    ଶୂନ୍ୟ
f_remote_ପଦସମଷ୍ଟି(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

# ifdef FEAT_CLIENTSERVER
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ସଂଖ୍ୟା_arg(argvars, 3) == FAIL)))
	ଫେରନ୍ତୁ;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    ଶୂନ୍ୟ
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    ଯଦି (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	ଯଦି (server_name != ଖାଲି)
	    serverForeground(server_name);
    }
#  ଅନ୍ୟଥା
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    ଶୂନ୍ୟ
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = ଖାଲି;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    serverid = tv_get_string_chk(&argvars[0]);
    ଯଦି (serverid == ଖାଲି)
	ଫେରନ୍ତୁ;		// type error; errmsg already given
#  ifdef MSWIN
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((ଅଚଳ ଅକ୍ଷର *)serverid, SCANF_HEX_LONG_U, &n);
	ଯଦି (n == 0)
	    rettv->vval.v_ସଂଖ୍ୟା = -1;
	ଅନ୍ୟଥା
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_ସଂଖ୍ୟା = (s != ଖାଲି);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_ସଂଖ୍ୟା = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	ଯଦି (check_connection() == FAIL)
	    ଫେରନ୍ତୁ;

	rettv->vval.v_ସଂଖ୍ୟା = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_ସଂଖ୍ୟା > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	ଯଦି (retvar != ଖାଲି)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# ଅନ୍ୟଥା
    rettv->vval.v_ସଂଖ୍ୟା = -1;
# endif
}

    ଶୂନ୍ୟ
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = ଖାଲି;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_ସଂଖ୍ୟା_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    serverid = tv_get_string_chk(&argvars[0]);
    ଯଦି (serverid != ଖାଲି && !check_restricted() && !check_secure())
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_ସଂଖ୍ୟା(&argvars[1]);

#  ifdef MSWIN
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((ଅକ୍ଷର *)serverid, SCANF_HEX_LONG_U, &n);
	    ଯଦି (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    ଯଦି (r == ଖାଲି)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    ଶୂନ୍ୟ
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

# ifdef FEAT_CLIENTSERVER
    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    ଶୂନ୍ୟ
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    ଯଦି (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (serverName != ଖାଲି)
    {
	emsg(_(e_already_started_server));
	ଫେରନ୍ତୁ;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    ଯଦି (server == ଖାଲି)
	ଫେରନ୍ତୁ;
#  ifdef MSWIN
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# ଅନ୍ୟଥା
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    ଶୂନ୍ୟ
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    rettv->vval.v_ସଂଖ୍ୟା = -1;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    ଯଦି (server == ଖାଲି || reply == ଖାଲି)
	ଫେରନ୍ତୁ;

#  ifdef FEAT_SOCKETSERVER
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    ଯଦି (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	ଫେରନ୍ତୁ;
    }
    rettv->vval.v_ସଂଖ୍ୟା = 0;
# ଅନ୍ୟଥା
    rettv->vval.v_ସଂଖ୍ୟା = -1;
# endif
}

    ଶୂନ୍ୟ
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = ଖାଲି;
    bool    use_list = false;

    ଯଦି (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	ଯଦି (X_DISPLAY != ଖାଲି)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    ଯଦି (use_list)
    {
	ଯଦି (list == ଖାଲି)
	{
	    (ଶୂନ୍ୟ)rettv_list_alloc(rettv);
	    ଫେରନ୍ତୁ;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    ଅନ୍ୟଥା
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	ଯଦି (list != ଖାଲି)
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
    ଶୂନ୍ୟ
check_clientserver_method_env(ଶୂନ୍ୟ)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    ଯଦି (env == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    ଅନ୍ୟଥା ଯଦି (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    ଅନ୍ୟଥା ଯଦି (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
