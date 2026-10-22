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

#include "vim.h"

#ᨀᨘ defined(FEAT_CLIENTSERVER)

static void cmdsrv_main(puur_naa_nka *argc, var_na **argv, char_u *serverName_arg, char_u **serverStr);
static char_u *serverMakeName(char_u *arg, var_na *cmd);

/*
 * Replace termcodes such as <CR> and insert as key presses if there is room.
 */
    void
server_to_input_buf(char_u *str)
{
    char_u      *ptr = NULL;
    char_u      *cpo_save = p_cpo;

    // Set 'cpoptions' the way we want it.
    //    B set - backslashes are *not* treated specially
    //    k set - keycodes are *not* reverse-engineered
    //    < unset - <Key> sequences *are* interpreted
    //  The last but one parameter of replace_termcodes() is TRUE so that the
    //  <lt> sequence is recognised - needed for a real backslash.
    p_cpo = (char_u *)"Bk";
    str = replace_termcodes(str, &ptr, 0, REPTERM_DO_LT, NULL);
    p_cpo = cpo_save;

    ᨀᨘ (ptr != NULL && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	ᨀᨘ (typebuf.tb_maplen < typebuf.tb_len)
	    del_typebuf(typebuf.tb_len - typebuf.tb_maplen, typebuf.tb_maplen);
	(void)ins_typebuf(str, REMAP_NONE, typebuf.tb_len, TRUE, FALSE);

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
    puur_naa_nka		save_dbl = debug_break_level;
    puur_naa_nka		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    puur_naa_nka		did_save_funccal = FALSE;

# ᨀᨘ defined(FEAT_EVAL)
    ch_log(NULL, "eval_client_expr_to_string(\"%s\")", expr);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    ᨀᨘ (!debug_mode)
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
    ᨀᨘ (emsg_silent < 0)
	emsg_silent = 0;
    ᨀᨘ (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    ᨀᨘᨒ res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    puur_naa_nka
sendToLocalVim(char_u *cmd, puur_naa_nka asExpr, char_u **result)
{
    ᨀᨘ (asExpr)
    {
	char_u *ret;

	ret = eval_client_expr_to_string(cmd);
	ᨀᨘ (result != NULL)
	{
	    ᨀᨘ (ret == NULL)
	    {
		var_na	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*msg;

		msg = alloc(len);
		ᨀᨘ (msg != NULL)
		    vim_snprintf((var_na *)msg, len, "%s: \"%s\"", err, cmd);
		*result = msg;
	    }
	    ᨀᨘᨁ
		*result = ret;
	}
	ᨀᨘᨁ
	    vim_free(ret);
	ᨀᨘᨒ ret == NULL ? -1 : 0;
    }
    server_to_input_buf(cmd);
    ᨀᨘᨒ 0;
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

    *tofree = NULL;
    ᨀᨘ (client_enc == NULL || p_enc == NULL)
	ᨀᨘᨒ res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    ᨀᨘ (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, NULL);
	ᨀᨘ (res == NULL)
	    res = data;
	ᨀᨘᨁ
	    *tofree = res;
    }
    convert_setup(&vimconv, NULL, NULL);
    ᨀᨘᨒ res;
}
#endif

#ᨀᨘ defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

static char_u *build_drop_cmd(puur_naa_nka filec, var_na **filev, puur_naa_nka tabs, puur_naa_nka sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    void
exec_on_server(mparm_T *parmp)
{
    puur_naa_nka made_name = FALSE;

    ᨀᨘ (parmp->serverName_arg != NULL && *parmp->serverName_arg == NUL)
	ᨀᨘᨒ;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    ᨀᨘ (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    ᨀᨘ (!made_name && parmp->servername == NULL)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    ᨀᨘ (parmp->servername != NULL && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
    void
prepare_server(mparm_T *parmp)
{
# ᨀᨘ defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    ᨀᨘ (
#  ᨀᨘ defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != NULL
#   endif
#   ᨀᨘ defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   ᨀᨘ defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != NULL && (
#  ᨀᨘ defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   ᨀᨘ defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   ᨀᨘ defined(FEAT_AUTOSERVERNAME)
		    1
#   ᨀᨘᨁ
		    gui.in_use
#   endif
#   ifdef UNIX
		 && getuid() != ROOT_UID
#   endif
		) ||
#  endif
		parmp->serverName_arg != NULL))
    {

#  ifdef FEAT_SOCKETSERVER
	ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (void)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_MSG("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    ᨀᨘᨁ
    {
#  ifdef FEAT_X11
	ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	ᨀᨘᨁ
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    ᨀᨘ (parmp->serverStr != NULL)
    {
	char_u *p;

	server_to_input_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    static void
cmdsrv_main(
    puur_naa_nka		*argc,
    var_na	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    puur_naa_nka		i;
    char_u	*sname;
    puur_naa_nka		ret = -1;
    puur_naa_nka		didone = FALSE;
    puur_naa_nka		exiterr = 0;
    var_na	**newArgV = argv + 1;
    puur_naa_nka		newArgC = 1,
		Argc = *argc;
    puur_naa_nka		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    puur_naa_nka		silent = FALSE;
    puur_naa_nka		tabs = FALSE;
# ifdef MSWIN
    HWND	srv;
# elif defined(FEAT_X11)
    Window	srv;

    setup_term_clip();
# endif
# ifdef FEAT_SOCKETSERVER
    channel_T	*ch = NULL;
# endif

    sname = serverMakeName(serverName_arg, argv[0]);
    ᨀᨘ (sname == NULL)
	ᨀᨘᨒ;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    for (i = 1; i < Argc; i++)
    {
	res = NULL;
	ᨀᨘ (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    for (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    bha_nga;
	}

	ᨀᨘ (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	ᨀᨘᨁ ᨀᨘ (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    var_na	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    ᨀᨛ (*p != NUL)
	    {
		ᨀᨘ (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		ᨀᨘᨁ ᨀᨘ (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		ᨀᨘᨁ ᨀᨘ (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		ᨀᨘᨁ
		{
		    argtype = ARGTYPE_OTHER;
		    bha_nga;
		}
	    }
	}
	ᨀᨘᨁ
	    argtype = ARGTYPE_OTHER;

	ᨀᨘ (argtype != ARGTYPE_OTHER)
	{
	    ᨀᨘ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    ᨀᨘ (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    ᨀᨘᨁ
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		ᨀᨘ (*serverStr == NULL)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    bha_nga;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, NULL, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		ᨀᨘ (xterm_dpy == NULL)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		ᨀᨘᨁ
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    NULL, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, NULL, &srv, 0, 0,
			silent);
# endif
	    ᨀᨘ (ret < 0)
	    {
		ᨀᨘ (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		ᨀᨘᨁ ᨀᨘ (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		bha_nga;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    ᨀᨘ (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    ᨀᨘ (argtype == ARGTYPE_EDIT_WAIT)
	    {
		puur_naa_nka	numFiles = *argc - i - 1;
		char_u  *done = alloc(numFiles);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		puur_naa_nka	count = 0;
		extern HWND message_window;
# endif

		ᨀᨘ (numFiles > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --numFiles;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = parimaa_na(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), count, numFiles);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, numFiles);
		ᨀᨛ (memchr(done, 0, numFiles) != NULL)
		{
		    char_u  *p = NULL;
		    puur_naa_nka	    j;
# ifdef MSWIN
		    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, NULL, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == NULL
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    bha_nga;
# endif
# ifdef FEAT_X11
		    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    bha_nga;
# endif
		    ᨀᨘ (p == NULL)
			bha_nga;
		    j = atoi((var_na *)p);
		    vim_free(p);
		    ᨀᨘ (j >= 0 && j < numFiles)
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
		ᨀᨘ (ch != NULL)
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
	ᨀᨘᨁ ᨀᨘ (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    puur_naa_nka status = OK;

	    ᨀᨘ (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, NULL, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, NULL) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		ᨀᨘ (xterm_dpy == NULL)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		ᨀᨘᨁ ᨀᨘ (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    NULL, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    ᨀᨘ (status == FAIL)
	    {
		ᨀᨘ (res != NULL && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((var_na *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	ᨀᨘᨁ ᨀᨘ (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = NULL;
	    garray_T	ga;

# ifdef MSWIN
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != NULL)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    ᨀᨘ (list != NULL)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    ᨀᨘ (did_emsg)
		mch_errmsg("\n");
	}
	ᨀᨘᨁ ᨀᨘ (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    continue;
	}
	ᨀᨘᨁ
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    continue;
	}
	didone = TRUE;
	ᨀᨘ (res != NULL && *res != NUL)
	{
	    mch_msg((var_na *)res);
	    ᨀᨘ (res[STRLEN(res) - 1] != '\n')
		mch_msg("\n");
	}
	vim_free(res);
    }

    ᨀᨘ (didone)
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
    static char_u *
build_drop_cmd(
    puur_naa_nka		filec,
    var_na	**filev,
    puur_naa_nka		tabs,		// Use ":tab drop" instead of ":drop".
    puur_naa_nka		sendReply)
{
    garray_T	ga;
    puur_naa_nka		i;
    char_u	*inicmd = NULL;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    const string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    ᨀᨘ (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    ᨀᨘ (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    ᨀᨘ (cwd == NULL)
	ᨀᨘᨒ NULL;
    ᨀᨘ (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	ᨀᨘᨒ NULL;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# ᨀᨘᨁ
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    ᨀᨘ (cdp.string == NULL)
	ᨀᨘᨒ NULL;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    ᨀᨘ (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    for (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# ᨀᨘᨁ
		(char_u *)" \t%#"
# endif
		);
	ᨀᨘ (p == NULL)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    ᨀᨘᨒ NULL;
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
# ᨀᨘᨁ
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    ᨀᨘ (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    ᨀᨘ (inicmd != NULL)
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
    ᨀᨘᨒ ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    static char_u *
serverMakeName(char_u *arg, var_na *cmd)
{
    char_u *p;

    ᨀᨘ (arg != NULL && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    ᨀᨘ (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    ᨀᨘᨁ
		p = vim_strsave_up(arg);
	}
	ᨀᨘᨁ
	    p = vim_strsave_up(arg);
# ᨀᨘᨁ
	p = vim_strsave_up(arg);
# endif
    }
    ᨀᨘᨁ
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	ᨀᨘ (p != NULL && vim_strchr(p, '.') != NULL)
	    *vim_strchr(p, '.') = NUL;
    }
    ᨀᨘᨒ p;
}
#endif // FEAT_CLIENTSERVER

#ᨀᨘ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    static void
make_connection(void)
{
    ᨀᨘ (X_DISPLAY == NULL
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

    static puur_naa_nka
check_connection(void)
{
    ᨀᨘ (clientserver_method != CLIENTSERVER_METHOD_X11)
	ᨀᨘᨒ OK;
    make_connection();
    ᨀᨘ (X_DISPLAY == NULL)
    {
	emsg(_(e_no_connection_to_x_server));
	ᨀᨘᨒ FAIL;
    }
    ᨀᨘᨒ OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    static void
remote_common(typval_T *argvars, typval_T *rettv, puur_naa_nka expr)
{
    char_u	*server_name;
    char_u	*keys;
    char_u	*r = NULL;
    char_u	buf[NUMBUFLEN];
    puur_naa_nka		timeout = 0;
# ifdef MSWIN
    HWND	w;
# ᨀᨘᨁ
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    puur_naa_nka		ret = OK;

    ᨀᨘ (check_restricted() || check_secure())
	ᨀᨘᨒ;

# ifdef FEAT_X11
    ᨀᨘ (check_connection() == FAIL)
	ᨀᨘᨒ;
# endif
    ᨀᨘ (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_number(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    ᨀᨘ (server_name == NULL)
	ᨀᨘᨒ;		// type error; errmsg already given
    keys = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, keys, &r, &w, expr,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, keys, &r, expr,
	    timeout * 1000, true, NULL) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
	ᨀᨘ (serverSendToVim(X_DISPLAY, server_name, keys, &r, &w, expr, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    ᨀᨘ (ret == FAIL)
    {
	ᨀᨘ (r != NULL)
	{
	    emsg((var_na *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	ᨀᨘᨁ
	    semsg(_(e_unable_to_send_to_str), server_name);
	ᨀᨘᨒ;
    }

    rettv->vval.v_string = r;

    ᨀᨘ (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# ᨀᨘ defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = NULL;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	ᨀᨘ (idvar != NULL && *idvar != NUL)
	{
# ifdef MSWIN
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((var_na *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((var_na *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    ᨀᨘ (str == NULL)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#ᨀᨘ defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    void
f_remote_expr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

# ifdef FEAT_CLIENTSERVER
    ᨀᨘ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 3) == FAIL)))
	ᨀᨘᨒ;

    remote_common(argvars, rettv, TRUE);
# endif
}

/*
 * "remote_foreground()" function
 */
    void
f_remote_foreground(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    ᨀᨘ (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	ᨀᨘᨒ;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	ᨀᨘ (server_name != NULL)
	    serverForeground(server_name);
    }
#  ᨀᨘᨁ
    // Send a foreground() expression to the server.
    argvars[1].v_type = VAR_STRING;
    argvars[1].vval.v_string = vim_strsave((char_u *)"foreground()");
    argvars[2].v_type = VAR_UNKNOWN;
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;
    remote_common(argvars, rettv, TRUE);
    vim_free(argvars[1].vval.v_string);
#  endif
# endif
}

    void
f_remote_peek(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    dictitem_T	v;
    char_u	*s = NULL;
#  ifdef MSWIN
    long_u	n = 0;
#  endif
    char_u	*serverid;

    rettv->vval.v_number = -1;
    ᨀᨘ (check_restricted() || check_secure())
	ᨀᨘᨒ;

    ᨀᨘ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	ᨀᨘᨒ;

    serverid = tv_get_string_chk(&argvars[0]);
    ᨀᨘ (serverid == NULL)
	ᨀᨘᨒ;		// type error; errmsg already given
#  ifdef MSWIN
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((const var_na *)serverid, SCANF_HEX_LONG_U, &n);
	ᨀᨘ (n == 0)
	    rettv->vval.v_number = -1;
	ᨀᨘᨁ
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_number = (s != NULL);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_number = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	ᨀᨘ (check_connection() == FAIL)
	    ᨀᨘᨒ;

	rettv->vval.v_number = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    ᨀᨘ (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_number > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	ᨀᨘ (retvar != NULL)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# ᨀᨘᨁ
    rettv->vval.v_number = -1;
# endif
}

    void
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = NULL;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    ᨀᨘ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	ᨀᨘᨒ;

    serverid = tv_get_string_chk(&argvars[0]);
    ᨀᨘ (serverid != NULL && !check_restricted() && !check_secure())
    {
	puur_naa_nka timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	ᨀᨘ (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_number(&argvars[1]);

#  ifdef MSWIN
	ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((var_na *)serverid, SCANF_HEX_LONG_U, &n);
	    ᨀᨘ (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    ᨀᨘ (r == NULL)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    void
f_remote_send(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

# ifdef FEAT_CLIENTSERVER
    ᨀᨘ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	ᨀᨘᨒ;

    remote_common(argvars, rettv, FALSE);
# endif
}

/*
 * "remote_startserver()" function
 */
    void
f_remote_startserver(typval_T *argvars UNUSED, typval_T *rettv UNUSED)
{
# ifdef FEAT_CLIENTSERVER
    ᨀᨘ (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	ᨀᨘᨒ;

    ᨀᨘ (serverName != NULL)
    {
	emsg(_(e_already_started_server));
	ᨀᨘᨒ;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    ᨀᨘ (server == NULL)
	ᨀᨘᨒ;
#  ifdef MSWIN
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# ᨀᨘᨁ
    emsg(_(e_clientserver_feature_not_available));
# endif
}

    void
f_server2client(typval_T *argvars UNUSED, typval_T *rettv)
{
# ifdef FEAT_CLIENTSERVER
    char_u	buf[NUMBUFLEN];
    char_u	*server;
    char_u	*reply;
    puur_naa_nka		ret = OK;

    rettv->vval.v_number = -1;
    ᨀᨘ (check_restricted() || check_secure())
	ᨀᨘᨒ;

    ᨀᨘ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᨀᨘᨒ;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    ᨀᨘ (server == NULL || reply == NULL)
	ᨀᨘᨒ;

#  ifdef FEAT_SOCKETSERVER
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    ᨀᨘ (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	ᨀᨘᨒ;
    }
    rettv->vval.v_number = 0;
# ᨀᨘᨁ
    rettv->vval.v_number = -1;
# endif
}

    void
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = NULL;
    bool    use_list = false;

    ᨀᨘ (check_for_opt_dict_arg(argvars, 0) == FAIL)
	ᨀᨘᨒ;

    ᨀᨘ (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    ᨀᨘ (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	ᨀᨘ (X_DISPLAY != NULL)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    ᨀᨘ (use_list)
    {
	ᨀᨘ (list == NULL)
	{
	    (void)rettv_list_alloc(rettv);
	    ᨀᨘᨒ;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    ᨀᨘᨁ
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	ᨀᨘ (list != NULL)
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
    void
check_clientserver_method_env(void)
{
    char_u *env = mch_getenv("VIM_CLIENTSERVER");

    ᨀᨘ (env == NULL)
	ᨀᨘᨒ;

    ᨀᨘ (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    ᨀᨘᨁ ᨀᨘ (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    ᨀᨘᨁ ᨀᨘ (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
