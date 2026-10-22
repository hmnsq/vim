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

#إذا defined(FEAT_CLIENTSERVER)

ساكن فراغ cmdsrv_main(صحيح *argc, حرف **argv, char_u *serverName_arg, char_u **serverStr);
ساكن char_u *serverMakeName(char_u *arg, حرف *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    فراغ
server_منزل_داخلا_buf(char_u *str)
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

    إذا (ptr != عدم && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	إذا (typebuf.tb_maplen < typebuf.tb_len)
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
eval_client_اظهار_منزل_string(char_u *اظهار)
{
    char_u	*res;
    صحيح		save_dbl = debug_break_level;
    صحيح		save_ro = redir_off;
    funccal_داخلا_T funccal_داخلا;
    صحيح		did_save_funccal = FALSE;

# إذا defined(FEAT_EVAL)
    ch_log(عدم, "eval_client_expr_to_string(\"%s\")", اظهار);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    إذا (!debug_گهڻائي)
    {
	save_funccal(&funccal_داخلا);
	did_save_funccal = TRUE;
    }

     // Disable debugging, otherwise Vim hangs, waiting for "cont" to be
     // typed.
    debug_break_level = -1;
    redir_off = 0;
    // Do not display error message, otherwise Vim hangs, waiting for "cont"
    // to be typed.  Do generate errors so that try/catch works.
    ++emsg_silent;

    res = eval_منزل_string(اظهار, TRUE, FALSE);

    debug_break_level = save_dbl;
    redir_off = save_ro;
    --emsg_silent;
    إذا (emsg_silent < 0)
	emsg_silent = 0;
    إذا (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    ٻاهريون_flush_cursor(FALSE, FALSE);

    أرجع res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    صحيح
send_منزل_Local_Vim(char_u *cmd, صحيح as_اظهار, char_u **نتيجو)
{
    إذا (as_اظهار)
    {
	char_u *ret;

	ret = eval_client_اظهار_منزل_string(cmd);
	إذا (نتيجو != عدم)
	{
	    إذا (ret == عدم)
	    {
		حرف	*err = _(e_invalid_اظهار_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*نياپو;

		نياپو = alloc(len);
		إذا (نياپو != عدم)
		    vim_snprintf((حرف *)نياپو, len, "%s: \"%s\"", err, cmd);
		*نتيجو = نياپو;
	    }
	    وإلا
		*نتيجو = ret;
	}
	وإلا
	    vim_free(ret);
	أرجع ret == عدم ? -1 : 0;
    }
    server_منزل_داخلا_buf(cmd);
    أرجع 0;
}

/*
 * If conversion is needed, convert "data" from "client_enc" to 'encoding' and
 * return an allocated string.  Otherwise return "data".
 * "*tofree" is set to the result when it needs to be freed later.
 */
    char_u *
serverConvert(
    char_u *client_enc اڻ_استعمال_ٿيل,
    char_u *معلومات,
    char_u **tofree)
{
    char_u	*res = معلومات;

    *tofree = عدم;
    إذا (client_enc == عدم || p_enc == عدم)
	أرجع res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    إذا (convert_تياري(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, معلومات, عدم);
	إذا (res == عدم)
	    res = معلومات;
	وإلا
	    *tofree = res;
    }
    convert_تياري(&vimconv, عدم, عدم);
    أرجع res;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ساكن char_u *build_drop_cmd(صحيح filec, حرف **filev, صحيح tabs, صحيح send_جواب);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    فراغ
exec_on_server(mparm_T *parmp)
{
    صحيح made_name = FALSE;

    إذا (parmp->serverName_arg != عدم && *parmp->serverName_arg == NUL)
	أرجع;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    إذا (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    إذا (!made_name && parmp->servername == عدم)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    إذا (parmp->servername != عدم && clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
    {
	server_سيٽ_Name(parmp->servername);
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
# إذا defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    إذا (
#  إذا defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != عدم
#   endif
#   إذا defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   إذا defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != عدم && (
#  إذا defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   إذا defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_طريقو != CLIENTSERVER_طريقو_SOCKET &&
#   endif
#   إذا defined(FEAT_AUTOSERVERNAME)
		    1
#   وإلا
		    gui.اندريون_use
#   endif
#   ifdef UNIX
		 && getuid() != پاڙ_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != عدم))
    {

#  ifdef FEAT_SOCKETSERVER
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
	    socketserver_شروعات(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
	{
	    (فراغ)server_رجسٽر_Name(X_DISPLAY, parmp->servername);
	    وقت_نياپو("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    وإلا
    {
#  ifdef FEAT_X11
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
	    server_Delayed_شروعات_Name = parmp->servername;
	وإلا
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    إذا (parmp->serverStr != عدم)
    {
	char_u *p;

	server_منزل_داخلا_buf(serverConvert(parmp->serverStrEnc,
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
    دري	srv;

    تياري_رڪن_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = عدم;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    إذا (sname == عدم)
	أرجع;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    لكل (i = 1; i < Argc; i++)
    {
	res = عدم;
	إذا (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    لكل (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    اكسر;
	}

	إذا (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	وإلا إذا (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    حرف	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    طالما (*p != NUL)
	    {
		إذا (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		وإلا إذا (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		وإلا إذا (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		وإلا
		{
		    argtype = ARGTYPE_OTHER;
		    اكسر;
		}
	    }
	}
	وإلا
	    argtype = ARGTYPE_OTHER;

	إذا (argtype != ARGTYPE_OTHER)
	{
	    إذا (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    إذا (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    وإلا
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		إذا (*serverStr == عدم)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    اكسر;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
		ret = socketserver_send(sname, *serverStr, عدم, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
	    {
		إذا (xterm_dpy == عدم)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		وإلا
		    ret = server_Send_منزل_Vim(xterm_dpy, sname, *serverStr,
			    عدم, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
		// Win32 always works?
		ret = server_Send_منزل_Vim(sname, *serverStr, عدم, &srv, 0, 0,
			silent);
# endif
	    إذا (ret < 0)
	    {
		إذا (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		وإلا إذا (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		اكسر;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    إذا (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		سيٽ_Foreground_دري(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    إذا (argtype == ARGTYPE_EDIT_WAIT)
	    {
		صحيح	انگ_Files = *argc - i - 1;
		char_u  *پورو = alloc(انگ_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		صحيح	ڳڻپ = 0;
		خارجي HWND پيغام_دري;
# endif

		إذا (انگ_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --انگ_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cb_سائيز = حجم(ni);
		ni.hWnd = پيغام_دري;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = ڀريو_Icon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), ڳڻپ, انگ_Files);
		Shell_NotifyIcon(NIM_جوڙيو, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(پورو, 0, انگ_Files);
		طالما (memchr(پورو, 0, انگ_Files) != عدم)
		{
		    char_u  *p = عدم;
		    صحيح	    j;
# ifdef MSWIN
		    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
			p = server_Get_جواب(srv, عدم, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET
			    && (ch == عدم
				|| socketserver_read_جواب(sname, &p, -1, true)
				== FAIL))
			    اكسر;
# endif
# ifdef FEAT_X11
		    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11
			    && server_Read_جواب(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    اكسر;
# endif
		    إذا (p == عدم)
			اكسر;
		    j = atoi((حرف *)p);
		    vim_free(p);
		    إذا (j >= 0 && j < انگ_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++ڳڻپ;
			sprintf(ni.szTip, _("%d of %d edited"),
							     ڳڻپ, انگ_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			پورو[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		إذا (ch != عدم)
		{
		    channel_close(ch, false);
		    channel_clear(ch);
		}
# endif
# ifdef FEAT_GUI_MSWIN
		Shell_NotifyIcon(NIM_DELETE, &ni);
# endif
		vim_free(پورو);
	    }
	}
	وإلا إذا (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    صحيح حالت = OK;

	    إذا (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN
		    && server_Send_منزل_Vim(sname, (char_u *)argv[i + 1],
						  &res, عدم, 1, 0, FALSE) < 0)
		حالت = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, عدم) < 0)
		حالت = FAIL;
# endif
# ifdef FEAT_X11
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
	    {
		إذا (xterm_dpy == عدم)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		وإلا إذا (server_Send_منزل_Vim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    عدم, 1, 0, 1, FALSE) < 0)
		    حالت = FAIL;
	    }
# endif
	    إذا (حالت == FAIL)
	    {
		إذا (res != عدم && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((حرف *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	وإلا إذا (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = عدم;
	    garray_T	ga;

# ifdef MSWIN
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
		// Win32 always works?
		list = server_Get_Vim_نالا();
# endif
# ifdef FEAT_SOCKETSERVER
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11 &&
		    xterm_dpy != عدم)
		list = server_Get_Vim_نالا(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    إذا (list != عدم)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_معلومات;

	    إذا (did_emsg)
		mch_errmsg("\n");
	}
	وإلا إذا (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    استمر;
	}
	وإلا
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    استمر;
	}
	didone = TRUE;
	إذا (res != عدم && *res != NUL)
	{
	    mch_نياپو((حرف *)res);
	    إذا (res[STRLEN(res) - 1] != '\n')
		mch_نياپو("\n");
	}
	vim_free(res);
    }

    إذا (didone)
    {
	display_errors();	// display any collected messages
	ٻاهر_نڪرو(exiterr);	// Mission accomplished - get out
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
    صحيح		send_جواب)
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

    إذا (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    إذا (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    إذا (cwd == عدم)
	أرجع عدم;
    إذا (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	أرجع عدم;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_اندريون_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# وإلا
	    رستو_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    إذا (cdp.string == عدم)
	أرجع عدم;
    cdp.ڊيگهائي = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.ڊيگهائي);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].ڊيگهائي);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    إذا (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    لكل (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		رستو_ESC_CHARS
# وإلا
		(char_u *)" \t%#"
# endif
		);
	إذا (p == عدم)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_معلومات);
	    أرجع عدم;
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
# وإلا
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.ڊيگهائي);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].ڊيگهائي);

    إذا (send_جواب)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    إذا (inicmd != عدم)
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
    أرجع ga.ga_معلومات;
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

    إذا (arg != عدم && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
	{
	    إذا (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    وإلا
		p = vim_strsave_مٿي(arg);
	}
	وإلا
	    p = vim_strsave_مٿي(arg);
# وإلا
	p = vim_strsave_مٿي(arg);
# endif
    }
    وإلا
    {
	p = vim_strsave_مٿي(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	إذا (p != عدم && vim_strchr(p, '.') != عدم)
	    *vim_strchr(p, '.') = NUL;
    }
    أرجع p;
}
#جيڪڏهن_پورو // FEAT_CLIENTSERVER

#إذا defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ساكن فراغ
make_connection(فراغ)
{
    إذا (X_DISPLAY == عدم
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
    {
	x_force_connect = TRUE;
	تياري_رڪن_clip();
	x_force_connect = FALSE;
    }
}

    ساكن صحيح
check_connection(فراغ)
{
    إذا (clientserver_طريقو != CLIENTSERVER_طريقو_X11)
	أرجع OK;
    make_connection();
    إذا (X_DISPLAY == عدم)
    {
	emsg(_(e_no_connection_منزل_x_server));
	أرجع FAIL;
    }
    أرجع OK;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CLIENTSERVER
    ساكن فراغ
remote_common(typval_T *argvars, typval_T *rettv, صحيح اظهار)
{
    char_u	*server_name;
    char_u	*ڪنجيون;
    char_u	*r = عدم;
    char_u	buf[NUMBUFLEN];
    صحيح		timeout = 0;
# ifdef MSWIN
    HWND	w;
# وإلا
#  ifdef FEAT_X11
    دري	w;
#  endif
# endif
    صحيح		ret = OK;

    إذا (check_restricted() || check_secure())
	أرجع;

# ifdef FEAT_X11
    إذا (check_connection() == FAIL)
	أرجع;
# endif
    إذا (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_عدد(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    إذا (server_name == عدم)
	أرجع;		// type error; errmsg already given
    ڪنجيون = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN
	    && server_Send_منزل_Vim(server_name, ڪنجيون, &r, &w, اظهار,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET
	&& socketserver_send(server_name, ڪنجيون, &r, اظهار,
	    timeout * 1000, true, عدم) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
	إذا (server_Send_منزل_Vim(X_DISPLAY, server_name, ڪنجيون, &r, &w, اظهار, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    إذا (ret == FAIL)
    {
	إذا (r != عدم)
	{
	    emsg((حرف *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	وإلا
	    semsg(_(e_unable_منزل_send_منزل_str), server_name);
	أرجع;
    }

    rettv->vval.v_string = r;

    إذا (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# إذا defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = عدم;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	إذا (idvar != عدم && *idvar != NUL)
	{
# ifdef MSWIN
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
	    {
		sprintf((حرف *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
	    {
		sprintf((حرف *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
		str = server_name;
# endif
	    إذا (str == عدم)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    سيٽ_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#جيڪڏهن_پورو

#إذا defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    فراغ
f_remote_اظهار(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

# ifdef FEAT_CLIENTSERVER
    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_عدد_arg(argvars, 3) == FAIL)))
	أرجع;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    فراغ
f_remote_foreground(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_CLIENTSERVER
    إذا (اندريون_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	أرجع;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	إذا (server_name != عدم)
	    serverForeground(server_name);
    }
#  وإلا
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
f_remote_peek(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = عدم;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_عدد = -1;
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	أرجع;

    serverid = tv_get_string_chk(&argvars[0]);
    إذا (serverid == عدم)
	أرجع;		// type error; errmsg already given
#  ifdef MSWIN
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
    {
	لڙي_مان_پڙهو((ثابت حرف *)serverid, SCANF_HEX_LONG_U, &n);
	إذا (n == 0)
	    rettv->vval.v_عدد = -1;
	وإلا
	{
	    s = server_Get_جواب((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_عدد = (s != عدم);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
	rettv->vval.v_عدد = socketserver_peek_جواب(serverid, &s);
#  endif
#  ifdef FEAT_X11
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
    {
	إذا (check_connection() == FAIL)
	    أرجع;

	rettv->vval.v_عدد = server_Peek_جواب(X_DISPLAY,
		server_Str_منزل_Win(serverid), &s);
    }
#  endif

    إذا (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_عدد > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	إذا (retvar != عدم)
	    سيٽ_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# وإلا
    rettv->vval.v_عدد = -1;
# endif
}

    فراغ
f_remote_read(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    char_u	*r = عدم;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_عدد_arg(argvars, 1) == FAIL))
	أرجع;

    serverid = tv_get_string_chk(&argvars[0]);
    إذا (serverid != عدم && !check_restricted() && !check_secure())
    {
	صحيح timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	إذا (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_عدد(&argvars[1]);

#  ifdef MSWIN
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
	{
	    لڙي_مان_پڙهو((حرف *)serverid, SCANF_HEX_LONG_U, &n);
	    إذا (n != 0)
		r = server_Get_جواب((HWND)n, FALSE, TRUE, TRUE, timeout);
	    إذا (r == عدم)
		emsg(_(e_unable_منزل_read_server_جواب));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET &&
		socketserver_read_جواب(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_منزل_read_server_جواب));
#  endif
#  ifdef FEAT_X11
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11 &&
		(check_connection() == FAIL
		|| server_Read_جواب(X_DISPLAY, server_Str_منزل_Win(serverid),
						       &r, FALSE, timeout) < 0))
	    emsg(_(e_unable_منزل_read_server_جواب));
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
f_remote_send(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

# ifdef FEAT_CLIENTSERVER
    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	أرجع;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    فراغ
f_remote_startserver(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv اڻ_استعمال_ٿيل)
{
# ifdef FEAT_CLIENTSERVER
    إذا (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (serverName != عدم)
    {
	emsg(_(e_already_started_server));
	أرجع;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    إذا (server == عدم)
	أرجع;
#  ifdef MSWIN
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
	server_سيٽ_Name(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
	socketserver_شروعات(server, true);
#  endif
#  ifdef FEAT_X11
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11 &&
	    check_connection() == OK)
	server_رجسٽر_Name(X_DISPLAY, server);
#  endif

# وإلا
    emsg(_(e_clientserver_feature_not_موجود));
# endif
}

    فراغ
f_server2client(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*جواب;
    صحيح		ret = OK;

    rettv->vval.v_عدد = -1;
    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    server = tv_get_string_chk(&argvars[0]);
    جواب = tv_get_string_buf_chk(&argvars[1], buf);
    إذا (server == عدم || جواب == عدم)
	أرجع;

#  ifdef FEAT_SOCKETSERVER
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET &&
	    socketserver_send_جواب(server, جواب) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11 &&
	    server_Send_جواب(server, جواب) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN
	    && server_Send_جواب(server, جواب) < 0)
	ret = FAIL;
#  endif

    إذا (ret == FAIL)
    {
	emsg(_(e_unable_منزل_send_منزل_client));
	أرجع;
    }
    rettv->vval.v_عدد = 0;
# وإلا
    rettv->vval.v_عدد = -1;
# endif
}

    فراغ
f_serverlist(typval_T *argvars اڻ_استعمال_ٿيل, typval_T *rettv)
{
    list_T  *list = عدم;
    bool    use_list = false;

    إذا (check_for_opt_dict_arg(argvars, 0) == FAIL)
	أرجع;

    إذا (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_MSWIN)
	list = server_Get_Vim_نالا();
#  endif
#  ifdef FEAT_SOCKETSERVER
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11)
    {
	make_connection();
	إذا (X_DISPLAY != عدم)
	    list = server_Get_Vim_نالا(X_DISPLAY);
    }
#  endif
# endif
    إذا (use_list)
    {
	إذا (list == عدم)
	{
	    (فراغ)rettv_list_alloc(rettv);
	    أرجع;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    وإلا
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	إذا (list != عدم)
	{
	    list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
	    ga_append(&ga, NUL);
	    list_free(list);
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = (char_u *)ga.ga_معلومات;
    }
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_CLIENTSERVER_BACKENDS
/*
 * Check the $VIM_CLIENTSERVER environment variable and set the method
 */
    فراغ
check_clientserver_طريقو_env(فراغ)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    إذا (env == عدم)
	أرجع;

    إذا (STRICMP(env, "socket") == 0)
	clientserver_طريقو = CLIENTSERVER_طريقو_SOCKET;
# ifdef FEAT_X11
    وإلا إذا (STRICMP(env, "x11") == 0)
	clientserver_طريقو = CLIENTSERVER_طريقو_X11;
# endif
# ifdef MSWIN
    وإلا إذا (STRICMP(env, "mswin") == 0)
	clientserver_طريقو = CLIENTSERVER_طريقو_MSWIN;
# endif
}
#جيڪڏهن_پورو
