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

ساکن خالی cmdsrv_main(صحیح *argc, حرف **argv, char_u *serverName_arg, char_u **serverStr);
ساکن char_u *serverMakeName(char_u *arg, حرف *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    خالی
server_مقصد_داخلہ_buf(char_u *str)
{
    char_u      *ptr = تہی;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, تہی);
    p_cpo = cpo_save;

    اگر (ptr != تہی && *ptr != NUL)	// trailing CTRL-V results in nothing
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
	(خالی)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
eval_client_اظہاریہ_مقصد_string(char_u *اظہاریہ)
{
    char_u	*res;
    صحیح		save_dbl = debug_break_level;
    صحیح		save_ro = redir_off;
    funccal_اندراج_T funccal_اندراج;
    صحیح		did_save_funccal = FALSE;

# اگر defined(FEAT_EVAL)
    ch_log(تہی, "eval_client_expr_to_string(\"%s\")", اظہاریہ);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    اگر (!debug_منوال)
    {
	save_funccal(&funccal_اندراج);
	did_save_funccal = TRUE;
    }

     // Disable debugging, otherwise Vim hangs, waiting for "cont" to be
     // typed.
    debug_break_level = -1;
    redir_off = 0;
    // Do not display error message, otherwise Vim hangs, waiting for "cont"
    // to be typed.  Do generate errors so that try/catch works.
    ++emsg_silent;

    res = eval_مقصد_string(اظہاریہ, TRUE, FALSE);

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
    برآمد_flush_cursor(FALSE, FALSE);

    واپس res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    صحیح
send_مقصد_Local_Vim(char_u *cmd, صحیح as_اظہاریہ, char_u **نتیجہ)
{
    اگر (as_اظہاریہ)
    {
	char_u *ret;

	ret = eval_client_اظہاریہ_مقصد_string(cmd);
	اگر (نتیجہ != تہی)
	{
	    اگر (ret == تہی)
	    {
		حرف	*err = _(e_invalid_عبارت_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*پیغام;

		پیغام = alloc(len);
		اگر (پیغام != تہی)
		    vim_snprintf((حرف *)پیغام, len, "%s: \"%s\"", err, cmd);
		*نتیجہ = پیغام;
	    }
	    ورنہ
		*نتیجہ = ret;
	}
	ورنہ
	    vim_free(ret);
	واپس ret == تہی ? -1 : 0;
    }
    server_مقصد_داخلہ_buf(cmd);
    واپس 0;
}

/*
 * If conversion is needed, convert "data" from "client_enc" to 'encoding' and
 * return an allocated string.  Otherwise return "data".
 * "*tofree" is set to the result when it needs to be freed later.
 */
    char_u *
serverConvert(
    char_u *client_enc غیر_مستعمل,
    char_u *ڈیٹا,
    char_u **tofree)
{
    char_u	*res = ڈیٹا;

    *tofree = تہی;
    اگر (client_enc == تہی || p_enc == تہی)
	واپس res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    اگر (convert_تیاری(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, ڈیٹا, تہی);
	اگر (res == تہی)
	    res = ڈیٹا;
	ورنہ
	    *tofree = res;
    }
    convert_تیاری(&vimconv, تہی, تہی);
    واپس res;
}
#اگر_ختم

#اگر defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

ساکن char_u *build_drop_cmd(صحیح filec, حرف **filev, صحیح tabs, صحیح send_جواب);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    خالی
exec_on_server(mparm_T *parmp)
{
    صحیح made_name = FALSE;

    اگر (parmp->serverName_arg != تہی && *parmp->serverName_arg == NUL)
	واپس;

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
    اگر (!made_name && parmp->servername == تہی)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    اگر (parmp->servername != تہی && clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
    {
	server_سیٹ_Name(parmp->servername);
#  ifndef FEAT_SOCKETSERVER
	vim_free(parmp->servername);
#  endif
    }
# endif
}
/*
 * Prepare for running as a Vim server.
 */
    خالی
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
	    X_DISPLAY != تہی
#   endif
#   اگر defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   اگر defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != تہی && (
#  اگر defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   اگر defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_طریقہ != CLIENTSERVER_طریقہ_SOCKET &&
#   endif
#   اگر defined(FEAT_AUTOSERVERNAME)
		    1
#   ورنہ
		    gui.اندر_use
#   endif
#   ifdef UNIX
		 && getuid() != جڑ_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != تہی))
    {

#  ifdef FEAT_SOCKETSERVER
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
	    socketserver_آغاز(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
	{
	    (خالی)server_رجسٹر_Name(X_DISPLAY, parmp->servername);
	    وقت_پیغام("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    ورنہ
    {
#  ifdef FEAT_X11
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
	    server_Delayed_آغاز_Name = parmp->servername;
	ورنہ
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    اگر (parmp->serverStr != تہی)
    {
	char_u *p;

	server_مقصد_داخلہ_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    ساکن خالی
cmdsrv_main(
    صحیح		*argc,
    حرف	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    صحیح		i;
    char_u	*sname;
    صحیح		ret = -1;
    صحیح		didone = FALSE;
    صحیح		exiterr = 0;
    حرف	**newArgV = argv + 1;
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
    کھڑکی	srv;

    تیاری_رکن_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = تہی;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    اگر (sname == تہی)
	واپس;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    برائے (i = 1; i < Argc; i++)
    {
	res = تہی;
	اگر (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    برائے (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    توڑو;
	}

	اگر (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	ورنہ اگر (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    حرف	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    جب (*p != NUL)
	    {
		اگر (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		ورنہ اگر (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		ورنہ اگر (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		ورنہ
		{
		    argtype = ARGTYPE_OTHER;
		    توڑو;
		}
	    }
	}
	ورنہ
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
	    ورنہ
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		اگر (*serverStr == تہی)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    توڑو;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
		ret = socketserver_send(sname, *serverStr, تہی, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
	    {
		اگر (xterm_dpy == تہی)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		ورنہ
		    ret = server_Send_مقصد_Vim(xterm_dpy, sname, *serverStr,
			    تہی, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
		// Win32 always works?
		ret = server_Send_مقصد_Vim(sname, *serverStr, تہی, &srv, 0, 0,
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
		ورنہ اگر (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		توڑو;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    اگر (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		سیٹ_Foreground_کھڑکی(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    اگر (argtype == ARGTYPE_EDIT_WAIT)
	    {
		صحیح	عدد_Files = *argc - i - 1;
		char_u  *مکمل = alloc(عدد_Files);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		صحیح	گنتی = 0;
		خارجی HWND پیغام_کھڑکی;
# endif

		اگر (عدد_Files > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --عدد_Files;

# ifdef FEAT_GUI_MSWIN
		ni.cb_سائز = حجم(ni);
		ni.hWnd = پیغام_کھڑکی;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = بھرو_Icon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), گنتی, عدد_Files);
		Shell_NotifyIcon(NIM_جوڑو, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(مکمل, 0, عدد_Files);
		جب (memchr(مکمل, 0, عدد_Files) != تہی)
		{
		    char_u  *p = تہی;
		    صحیح	    j;
# ifdef MSWIN
		    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
			p = server_Get_جواب(srv, تہی, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET
			    && (ch == تہی
				|| socketserver_read_جواب(sname, &p, -1, true)
				== FAIL))
			    توڑو;
# endif
# ifdef FEAT_X11
		    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11
			    && server_Read_جواب(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    توڑو;
# endif
		    اگر (p == تہی)
			توڑو;
		    j = atoi((حرف *)p);
		    vim_free(p);
		    اگر (j >= 0 && j < عدد_Files)
		    {
# ifdef FEAT_GUI_MSWIN
			++گنتی;
			sprintf(ni.szTip, _("%d of %d edited"),
							     گنتی, عدد_Files);
			Shell_NotifyIcon(NIM_MODIFY, &ni);
# endif
			مکمل[j] = 1;
		    }
		}
# ifdef FEAT_SOCKETSERVER
		اگر (ch != تہی)
		{
		    channel_close(ch, false);
		    channel_clear(ch);
		}
# endif
# ifdef FEAT_GUI_MSWIN
		Shell_NotifyIcon(NIM_DELETE, &ni);
# endif
		vim_free(مکمل);
	    }
	}
	ورنہ اگر (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    صحیح حالت_2 = OK;

	    اگر (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN
		    && server_Send_مقصد_Vim(sname, (char_u *)argv[i + 1],
						  &res, تہی, 1, 0, FALSE) < 0)
		حالت_2 = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, تہی) < 0)
		حالت_2 = FAIL;
# endif
# ifdef FEAT_X11
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
	    {
		اگر (xterm_dpy == تہی)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		ورنہ اگر (server_Send_مقصد_Vim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    تہی, 1, 0, 1, FALSE) < 0)
		    حالت_2 = FAIL;
	    }
# endif
	    اگر (حالت_2 == FAIL)
	    {
		اگر (res != تہی && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((حرف *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	ورنہ اگر (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = تہی;
	    garray_T	ga;

# ifdef MSWIN
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
		// Win32 always works?
		list = server_Get_Vim_نام();
# endif
# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11 &&
		    xterm_dpy != تہی)
		list = server_Get_Vim_نام(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    اگر (list != تہی)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_ڈیٹا;

	    اگر (did_emsg)
		mch_errmsg("\n");
	}
	ورنہ اگر (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    جاری;
	}
	ورنہ
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    جاری;
	}
	didone = TRUE;
	اگر (res != تہی && *res != NUL)
	{
	    mch_پیغام((حرف *)res);
	    اگر (res[STRLEN(res) - 1] != '\n')
		mch_پیغام("\n");
	}
	vim_free(res);
    }

    اگر (didone)
    {
	display_errors();	// display any collected messages
	باہر_نکلو(exiterr);	// Mission accomplished - get out
    }

    // Return back into main()
    *argc = newArgC;
    vim_free(sname);
}

/*
 * Build a ":drop" command to send to a Vim server.
 */
    ساکن char_u *
build_drop_cmd(
    صحیح		filec,
    حرف	**filev,
    صحیح		tabs,		// Use ":tab drop" instead of ":drop".
    صحیح		send_جواب)
{
    garray_T	ga;
    صحیح		i;
    char_u	*inicmd = تہی;
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
    اگر (cwd == تہی)
	واپس تہی;
    اگر (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	واپس تہی;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_اندر_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# ورنہ
	    راستہ_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    اگر (cdp.string == تہی)
	واپس تہی;
    cdp.لمبائی = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.لمبائی);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].لمبائی);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    اگر (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    برائے (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		راستہ_ESC_CHARS
# ورنہ
		(char_u *)" \t%#"
# endif
		);
	اگر (p == تہی)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_ڈیٹا);
	    واپس تہی;
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
# ورنہ
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.لمبائی);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].لمبائی);

    اگر (send_جواب)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    اگر (inicmd != تہی)
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
    واپس ga.ga_ڈیٹا;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    ساکن char_u *
serverMakeName(char_u *arg, حرف *cmd)
{
    char_u *p;

    اگر (arg != تہی && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
	{
	    اگر (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    ورنہ
		p = vim_strsave_اوپر(arg);
	}
	ورنہ
	    p = vim_strsave_اوپر(arg);
# ورنہ
	p = vim_strsave_اوپر(arg);
# endif
    }
    ورنہ
    {
	p = vim_strsave_اوپر(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	اگر (p != تہی && vim_strchr(p, '.') != تہی)
	    *vim_strchr(p, '.') = NUL;
    }
    واپس p;
}
#اگر_ختم // FEAT_CLIENTSERVER

#اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    ساکن خالی
make_connection(خالی)
{
    اگر (X_DISPLAY == تہی
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
    {
	x_force_connect = TRUE;
	تیاری_رکن_clip();
	x_force_connect = FALSE;
    }
}

    ساکن صحیح
check_connection(خالی)
{
    اگر (clientserver_طریقہ != CLIENTSERVER_طریقہ_X11)
	واپس OK;
    make_connection();
    اگر (X_DISPLAY == تہی)
    {
	emsg(_(e_no_connection_مقصد_x_server));
	واپس FAIL;
    }
    واپس OK;
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
    ساکن خالی
remote_common(typval_T *argvars, typval_T *rettv, صحیح اظہاریہ)
{
    char_u	*server_name;
    char_u	*کلیدیں;
    char_u	*r = تہی;
    char_u	buf[NUMBUFLEN];
    صحیح		timeout = 0;
# ifdef MSWIN
    HWND	w;
# ورنہ
#  ifdef FEAT_X11
    کھڑکی	w;
#  endif
# endif
    صحیح		ret = OK;

    اگر (check_restricted() || check_secure())
	واپس;

# ifdef FEAT_X11
    اگر (check_connection() == FAIL)
	واپس;
# endif
    اگر (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_نمبر(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    اگر (server_name == تہی)
	واپس;		// type error; errmsg already given
    کلیدیں = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN
	    && server_Send_مقصد_Vim(server_name, کلیدیں, &r, &w, اظہاریہ,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET
	&& socketserver_send(server_name, کلیدیں, &r, اظہاریہ,
	    timeout * 1000, true, تہی) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
	اگر (server_Send_مقصد_Vim(X_DISPLAY, server_name, کلیدیں, &r, &w, اظہاریہ, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    اگر (ret == FAIL)
    {
	اگر (r != تہی)
	{
	    emsg((حرف *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	ورنہ
	    semsg(_(e_unable_مقصد_send_مقصد_str), server_name);
	واپس;
    }

    rettv->vval.v_string = r;

    اگر (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# اگر defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = تہی;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	اگر (idvar != تہی && *idvar != NUL)
	{
# ifdef MSWIN
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
	    {
		sprintf((حرف *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
	    {
		sprintf((حرف *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
		str = server_name;
# endif
	    اگر (str == تہی)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    سیٹ_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#اگر_ختم

#اگر defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    خالی
f_remote_اظہاریہ(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

# ifdef FEAT_CLIENTSERVER
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_نمبر_arg(argvars, 3) == FAIL)))
	واپس;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    خالی
f_remote_foreground(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_CLIENTSERVER
    اگر (اندر_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	واپس;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	اگر (server_name != تہی)
	    serverForeground(server_name);
    }
#  ورنہ
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    خالی
f_remote_peek(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = تہی;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_نمبر = -1;
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	واپس;

    serverid = tv_get_string_chk(&argvars[0]);
    اگر (serverid == تہی)
	واپس;		// type error; errmsg already given
#  ifdef MSWIN
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
    {
	لڑی_سے_پڑھو((ثابت حرف *)serverid, SCANF_HEX_LONG_U, &n);
	اگر (n == 0)
	    rettv->vval.v_نمبر = -1;
	ورنہ
	{
	    s = server_Get_جواب((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_نمبر = (s != تہی);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
	rettv->vval.v_نمبر = socketserver_peek_جواب(serverid, &s);
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
    {
	اگر (check_connection() == FAIL)
	    واپس;

	rettv->vval.v_نمبر = server_Peek_جواب(X_DISPLAY,
		server_Str_مقصد_Win(serverid), &s);
    }
#  endif

    اگر (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_نمبر > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	اگر (retvar != تہی)
	    سیٹ_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# ورنہ
    rettv->vval.v_نمبر = -1;
# endif
}

    خالی
f_remote_read(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    char_u	*r = تہی;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_نمبر_arg(argvars, 1) == FAIL))
	واپس;

    serverid = tv_get_string_chk(&argvars[0]);
    اگر (serverid != تہی && !check_restricted() && !check_secure())
    {
	صحیح timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	اگر (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_نمبر(&argvars[1]);

#  ifdef MSWIN
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
	{
	    لڑی_سے_پڑھو((حرف *)serverid, SCANF_HEX_LONG_U, &n);
	    اگر (n != 0)
		r = server_Get_جواب((HWND)n, FALSE, TRUE, TRUE, timeout);
	    اگر (r == تہی)
		emsg(_(e_unable_مقصد_read_server_جواب));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET &&
		socketserver_read_جواب(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_مقصد_read_server_جواب));
#  endif
#  ifdef FEAT_X11
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11 &&
		(check_connection() == FAIL
		|| server_Read_جواب(X_DISPLAY, server_Str_مقصد_Win(serverid),
						       &r, FALSE, timeout) < 0))
	    emsg(_(e_unable_مقصد_read_server_جواب));
#  endif
    }
# endif
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = r;
}

/*
 * "remote_send()" function
 */
    خالی
f_remote_send(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

# ifdef FEAT_CLIENTSERVER
    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	واپس;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    خالی
f_remote_startserver(typval_T *argvars غیر_مستعمل, typval_T *rettv غیر_مستعمل)
{
# ifdef FEAT_CLIENTSERVER
    اگر (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	واپس;

    اگر (serverName != تہی)
    {
	emsg(_(e_already_started_server));
	واپس;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    اگر (server == تہی)
	واپس;
#  ifdef MSWIN
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
	server_سیٹ_Name(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
	socketserver_آغاز(server, true);
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11 &&
	    check_connection() == OK)
	server_رجسٹر_Name(X_DISPLAY, server);
#  endif

# ورنہ
    emsg(_(e_clientserver_feature_not_دستیاب));
# endif
}

    خالی
f_server2client(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*جواب;
    صحیح		ret = OK;

    rettv->vval.v_نمبر = -1;
    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    server = tv_get_string_chk(&argvars[0]);
    جواب = tv_get_string_buf_chk(&argvars[1], buf);
    اگر (server == تہی || جواب == تہی)
	واپس;

#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET &&
	    socketserver_send_جواب(server, جواب) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11 &&
	    server_Send_جواب(server, جواب) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN
	    && server_Send_جواب(server, جواب) < 0)
	ret = FAIL;
#  endif

    اگر (ret == FAIL)
    {
	emsg(_(e_unable_مقصد_send_مقصد_client));
	واپس;
    }
    rettv->vval.v_نمبر = 0;
# ورنہ
    rettv->vval.v_نمبر = -1;
# endif
}

    خالی
f_serverlist(typval_T *argvars غیر_مستعمل, typval_T *rettv)
{
    list_T  *list = تہی;
    bool    use_list = false;

    اگر (check_for_opt_dict_arg(argvars, 0) == FAIL)
	واپس;

    اگر (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_MSWIN)
	list = server_Get_Vim_نام();
#  endif
#  ifdef FEAT_SOCKETSERVER
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11)
    {
	make_connection();
	اگر (X_DISPLAY != تہی)
	    list = server_Get_Vim_نام(X_DISPLAY);
    }
#  endif
# endif
    اگر (use_list)
    {
	اگر (list == تہی)
	{
	    (خالی)rettv_list_alloc(rettv);
	    واپس;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    ورنہ
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	اگر (list != تہی)
	{
	    list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
	    ga_append(&ga, NUL);
	    list_free(list);
	}

	rettv->v_type = VAR_STRING;
	rettv->vval.v_string = (char_u *)ga.ga_ڈیٹا;
    }
}
#اگر_ختم

#اگر_تعریف_شدہ FEAT_CLIENTSERVER_BACKENDS
/*
 * Check the $VIM_CLIENTSERVER environment variable and set the method
 */
    خالی
check_clientserver_طریقہ_env(خالی)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    اگر (env == تہی)
	واپس;

    اگر (STRICMP(env, "socket") == 0)
	clientserver_طریقہ = CLIENTSERVER_طریقہ_SOCKET;
# ifdef FEAT_X11
    ورنہ اگر (STRICMP(env, "x11") == 0)
	clientserver_طریقہ = CLIENTSERVER_طریقہ_X11;
# endif
# ifdef MSWIN
    ورنہ اگر (STRICMP(env, "mswin") == 0)
	clientserver_طریقہ = CLIENTSERVER_طریقہ_MSWIN;
# endif
}
#اگر_ختم
