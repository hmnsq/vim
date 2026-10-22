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

#만약 defined(FEAT_CLIENTSERVER)

static void cmdsrv_main(정수 *argc, char **argv, char_u *serverName_arg, char_u **serverStr);
static char_u *serverMakeName(char_u *arg, char *cmd);

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

    만약 (ptr != NULL && *ptr != NUL)	// trailing CTRL-V results in nothing
    {
	/*
	 * Add the string to the input stream.
	 * Can't use add_to_input_buf() here, we now have K_SPECIAL bytes.
	 *
	 * First clear typed characters from the typeahead buffer, there could
	 * be half a mapping there.  Then append to the existing string, so
	 * that multiple commands from a client are concatenated.
	 */
	만약 (typebuf.tb_maplen < typebuf.tb_len)
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
    정수		save_dbl = debug_break_level;
    정수		save_ro = redir_off;
    funccal_entry_T funccal_entry;
    정수		did_save_funccal = FALSE;

# 만약 defined(FEAT_EVAL)
    ch_log(NULL, "eval_client_expr_to_string(\"%s\")", expr);
# endif

    // Evaluate the expression at the toplevel, don't use variables local to
    // the calling function. Except when in debug mode.
    만약 (!debug_mode)
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
    만약 (emsg_silent < 0)
	emsg_silent = 0;
    만약 (did_save_funccal)
	restore_funccal();

    // A client can tell us to redraw, but not to display the cursor, so do
    // that here.
    setcursor();
    out_flush_cursor(FALSE, FALSE);

    반환 res;
}

/*
 * Evaluate a command or expression sent to ourselves.
 */
    정수
sendToLocalVim(char_u *cmd, 정수 asExpr, char_u **result)
{
    만약 (asExpr)
    {
	char_u *ret;

	ret = eval_client_expr_to_string(cmd);
	만약 (result != NULL)
	{
	    만약 (ret == NULL)
	    {
		char	*err = _(e_invalid_expression_received);
		size_t	len = STRLEN(cmd) + STRLEN(err) + 5;
		char_u	*msg;

		msg = alloc(len);
		만약 (msg != NULL)
		    vim_snprintf((char *)msg, len, "%s: \"%s\"", err, cmd);
		*result = msg;
	    }
	    아니면
		*result = ret;
	}
	아니면
	    vim_free(ret);
	반환 ret == NULL ? -1 : 0;
    }
    server_to_input_buf(cmd);
    반환 0;
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
    만약 (client_enc == NULL || p_enc == NULL)
	반환 res;

    vimconv_T	vimconv;

    vimconv.vc_type = CONV_NONE;
    만약 (convert_setup(&vimconv, client_enc, p_enc) != FAIL
	    && vimconv.vc_type != CONV_NONE)
    {
	res = string_convert(&vimconv, data, NULL);
	만약 (res == NULL)
	    res = data;
	아니면
	    *tofree = res;
    }
    convert_setup(&vimconv, NULL, NULL);
    반환 res;
}
#endif

#만약 defined(FEAT_CLIENTSERVER) && !defined(NO_VIM_MAIN)

/*
 * Common code for the X command server and the Win32 command server.
 */

static char_u *build_drop_cmd(정수 filec, char **filev, 정수 tabs, 정수 sendReply);

/*
 * Do the client-server stuff, unless "--servername ''" was used.
 */
    void
exec_on_server(mparm_T *parmp)
{
    정수 made_name = FALSE;

    만약 (parmp->serverName_arg != NULL && *parmp->serverName_arg == NUL)
	반환;

# ifdef MSWIN
    // Initialise the client/server messaging infrastructure.
    serverInitMessaging();
# endif

    /*
     * When a command server argument was found, execute it.  This may
     * exit Vim when it was successful.  Otherwise it's executed further
     * on.  Remember the encoding used here in "serverStrEnc".
     */
    만약 (parmp->serverArg)
    {
	cmdsrv_main(&parmp->argc, parmp->argv,
		parmp->serverName_arg, &parmp->serverStr);
	parmp->serverStrEnc = vim_strsave(p_enc);
    }

    // If we're still running, get the name to register ourselves.
    // On Win32 can register right now, for X11 need to setup the
    // clipboard first, it's further down.
    만약 (!made_name && parmp->servername == NULL)
	parmp->servername = serverMakeName(parmp->serverName_arg,
		parmp->argv[0]);
# ifdef MSWIN
    만약 (parmp->servername != NULL && clientserver_method == CLIENTSERVER_METHOD_MSWIN)
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
# 만약 defined(FEAT_X11) || defined(FEAT_SOCKETSERVER)
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

    만약 (
#  만약 defined(FEAT_X11) || (defined(FEAT_SOCKETSERVER) && !defined(MSWIN))
	    (
#   ifdef FEAT_X11
	    X_DISPLAY != NULL
#   endif
#   만약 defined(FEAT_X11) && defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    ||
#   endif
#   만약 defined(FEAT_SOCKETSERVER) && !defined(MSWIN)
	    clientserver_method == CLIENTSERVER_METHOD_SOCKET
#   endif
	    ) &&
#  endif
	    parmp->servername != NULL && (
#  만약 defined(FEAT_AUTOSERVERNAME) || defined(FEAT_GUI)
		(
#   만약 defined(FEAT_SOCKETSERVER) && defined(MSWIN)
		 clientserver_method != CLIENTSERVER_METHOD_SOCKET &&
#   endif
#   만약 defined(FEAT_AUTOSERVERNAME)
		    1
#   아니면
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
	만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	    socketserver_start(parmp->servername, false);
#  endif
#  ifdef FEAT_X11
	만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
	{
	    (void)serverRegisterName(X_DISPLAY, parmp->servername);
	    TIME_MSG("register x11 server name");
	}
#  endif
	vim_free(parmp->servername);
    }
    아니면
    {
#  ifdef FEAT_X11
	만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
	    serverDelayedStartName = parmp->servername;
	아니면
#  endif
	    vim_free(parmp->servername);
    }
# endif

    /*
     * Execute command ourselves if we're here because the send failed (or
     * else we would have exited above).
     */
    만약 (parmp->serverStr != NULL)
    {
	char_u *p;

	server_to_input_buf(serverConvert(parmp->serverStrEnc,
						       parmp->serverStr, &p));
	vim_free(p);
    }
}

    static void
cmdsrv_main(
    정수		*argc,
    char	**argv,
    char_u	*serverName_arg,
    char_u	**serverStr)
{
    char_u	*res;
    정수		i;
    char_u	*sname;
    정수		ret = -1;
    정수		didone = FALSE;
    정수		exiterr = 0;
    char	**newArgV = argv + 1;
    정수		newArgC = 1,
		Argc = *argc;
    정수		argtype;
# define ARGTYPE_OTHER		0
# define ARGTYPE_EDIT		1
# define ARGTYPE_EDIT_WAIT	2
# define ARGTYPE_SEND		3
    정수		silent = FALSE;
    정수		tabs = FALSE;
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
    만약 (sname == NULL)
	반환;

    /*
     * Execute the command server related arguments and remove them
     * from the argc/argv array; We may have to return into main()
     */
    위해 (i = 1; i < Argc; i++)
    {
	res = NULL;
	만약 (STRCMP(argv[i], "--") == 0)	// end of option arguments
	{
	    위해 (; i < *argc; i++)
	    {
		*newArgV++ = argv[i];
		newArgC++;
	    }
	    break;
	}

	만약 (STRICMP(argv[i], "--remote-send") == 0)
	    argtype = ARGTYPE_SEND;
	아니면 만약 (STRNICMP(argv[i], "--remote", 8) == 0)
	{
	    char	*p = argv[i] + 8;

	    argtype = ARGTYPE_EDIT;
	    동안 (*p != NUL)
	    {
		만약 (STRNICMP(p, "-wait", 5) == 0)
		{
		    argtype = ARGTYPE_EDIT_WAIT;
		    p += 5;
		}
		아니면 만약 (STRNICMP(p, "-silent", 7) == 0)
		{
		    silent = TRUE;
		    p += 7;
		}
		아니면 만약 (STRNICMP(p, "-tab", 4) == 0)
		{
		    tabs = TRUE;
		    p += 4;
		}
		아니면
		{
		    argtype = ARGTYPE_OTHER;
		    break;
		}
	    }
	}
	아니면
	    argtype = ARGTYPE_OTHER;

	만약 (argtype != ARGTYPE_OTHER)
	{
	    만약 (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
	    만약 (argtype == ARGTYPE_SEND)
	    {
		*serverStr = (char_u *)argv[i + 1];
		i++;
	    }
	    아니면
	    {
		*serverStr = build_drop_cmd(*argc - i - 1, argv + i + 1,
					  tabs, argtype == ARGTYPE_EDIT_WAIT);
		만약 (*serverStr == NULL)
		{
		    // Probably out of memory, exit.
		    didone = TRUE;
		    exiterr = 1;
		    break;
		}
		Argc = i;
	    }

# ifdef FEAT_SOCKETSERVER
	    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		ret = socketserver_send(sname, *serverStr, NULL, false, -1,
			silent, &ch);
# endif
# ifdef FEAT_X11
	    만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		만약 (xterm_dpy == NULL)
		{
		    mch_errmsg(_("No display"));
		    ret = -1;
		}
		아니면
		    ret = serverSendToVim(xterm_dpy, sname, *serverStr,
			    NULL, &srv, 0, 0, 0, silent);
	    }
# endif
# ifdef MSWIN
	    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		ret = serverSendToVim(sname, *serverStr, NULL, &srv, 0, 0,
			silent);
# endif
	    만약 (ret < 0)
	    {
		만약 (argtype == ARGTYPE_SEND)
		{
		    // Failed to send, abort.
		    mch_errmsg(_(": Send failed.\n"));
		    didone = TRUE;
		    exiterr = 1;
		}
		아니면 만약 (!silent)
		    // Let vim start normally.
		    mch_errmsg(_(": Send failed. Trying to execute locally\n"));
		break;
	    }

# ifdef FEAT_GUI_MSWIN
	    // Guess that when the server name starts with "g" it's a GUI
	    // server, which we can bring to the foreground here.
	    // Foreground() in the server doesn't work very well.
	    만약 (argtype != ARGTYPE_SEND && TOUPPER_ASC(*sname) == 'G')
		SetForegroundWindow(srv);
# endif

	    /*
	     * For --remote-wait: Wait until the server did edit each
	     * file.  Also detect that the server no longer runs.
	     */
	    만약 (argtype == ARGTYPE_EDIT_WAIT)
	    {
		정수	numFiles = *argc - i - 1;
		char_u  *done = alloc(numFiles);
# ifdef FEAT_GUI_MSWIN
		NOTIFYICONDATA ni;
		정수	count = 0;
		extern HWND message_window;
# endif

		만약 (numFiles > 0 && argv[i + 1][0] == '+')
		    // Skip "+cmd" argument, don't wait for it to be edited.
		    --numFiles;

# ifdef FEAT_GUI_MSWIN
		ni.cbSize = sizeof(ni);
		ni.hWnd = message_window;
		ni.uID = 0;
		ni.uFlags = NIF_ICON|NIF_TIP;
		ni.hIcon = LoadIcon((HINSTANCE)GetModuleHandle(0), "IDR_VIM");
		sprintf(ni.szTip, _("%d of %d edited"), count, numFiles);
		Shell_NotifyIcon(NIM_ADD, &ni);
# endif

		// Wait for all files to unload in remote
		vim_memset(done, 0, numFiles);
		동안 (memchr(done, 0, numFiles) != NULL)
		{
		    char_u  *p = NULL;
		    정수	    j;
# ifdef MSWIN
		    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
			p = serverGetReply(srv, NULL, TRUE, TRUE, 0);
# endif
# ifdef FEAT_SOCKETSERVER
		    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET
			    && (ch == NULL
				|| socketserver_read_reply(sname, &p, -1, true)
				== FAIL))
			    break;
# endif
# ifdef FEAT_X11
		    만약 (clientserver_method == CLIENTSERVER_METHOD_X11
			    && serverReadReply(xterm_dpy, srv, &p, TRUE, -1) < 0)
			    break;
# endif
		    만약 (p == NULL)
			break;
		    j = atoi((char *)p);
		    vim_free(p);
		    만약 (j >= 0 && j < numFiles)
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
		만약 (ch != NULL)
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
	아니면 만약 (STRICMP(argv[i], "--remote-expr") == 0)
	{
	    정수 status = OK;

	    만약 (i == *argc - 1)
		mainerr_arg_missing((char_u *)argv[i]);
# ifdef MSWIN
	    // Win32 always works?
	    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN
		    && serverSendToVim(sname, (char_u *)argv[i + 1],
						  &res, NULL, 1, 0, FALSE) < 0)
		status = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
	    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET
		&& socketserver_send(sname, (char_u *)argv[i + 1],
			    &res, 1, 0, false, NULL) < 0)
		status = FAIL;
# endif
# ifdef FEAT_X11
	    만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		만약 (xterm_dpy == NULL)
		    mch_errmsg(_("No display: Send expression failed.\n"));
		아니면 만약 (serverSendToVim(xterm_dpy, sname,
			    (char_u *)argv[i + 1], &res,
			    NULL, 1, 0, 1, FALSE) < 0)
		    status = FAIL;
	    }
# endif
	    만약 (status == FAIL)
	    {
		만약 (res != NULL && *res != NUL)
		{
		    // Output error from remote
		    mch_errmsg((char *)res);
		    VIM_CLEAR(res);
		}
		mch_errmsg(_(": Send expression failed.\n"));
	    }
	}
	아니면 만약 (STRICMP(argv[i], "--serverlist") == 0)
	{
	    list_T	*list = NULL;
	    garray_T	ga;

# ifdef MSWIN
	    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
		// Win32 always works?
		list = serverGetVimNames();
# endif
# ifdef FEAT_SOCKETSERVER
	    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		list = socketserver_list();
# endif
# ifdef FEAT_X11
	    만약 (clientserver_method == CLIENTSERVER_METHOD_X11 &&
		    xterm_dpy != NULL)
		list = serverGetVimNames(xterm_dpy);
# endif

	    ga_init2(&ga, 1, 80);
	    만약 (list != NULL)
	    {
		list_join(&ga, list, (char_u *)"\n", TRUE, FALSE, 0);
		ga_append(&ga, NUL);
		list_free(list);
	    }
	    res = ga.ga_data;

	    만약 (did_emsg)
		mch_errmsg("\n");
	}
	아니면 만약 (STRICMP(argv[i], "--servername") == 0)
	{
	    // Already processed. Take it out of the command line
	    i++;
	    continue;
	}
	아니면
	{
	    *newArgV++ = argv[i];
	    newArgC++;
	    continue;
	}
	didone = TRUE;
	만약 (res != NULL && *res != NUL)
	{
	    mch_msg((char *)res);
	    만약 (res[STRLEN(res) - 1] != '\n')
		mch_msg("\n");
	}
	vim_free(res);
    }

    만약 (didone)
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
    정수		filec,
    char	**filev,
    정수		tabs,		// Use ":tab drop" instead of ":drop".
    정수		sendReply)
{
    garray_T	ga;
    정수		i;
    char_u	*inicmd = NULL;
    char_u	*p;
    string_T	cdp;
    char_u	*cwd;
    // reset wildignore temporarily
    const string_T wig[] = {
	STR_LITERAL_INIT("<CR><C-\\><C-N>:let g:_wig=&wig|set wig="),
	STR_LITERAL_INIT("<C-\\><C-N>:let &wig=g:_wig|unlet g:_wig<CR>")
    };

    만약 (filec > 0 && filev[0][0] == '+')
    {
	inicmd = (char_u *)filev[0] + 1;
	filev++;
	filec--;
    }
    // Check if we have at least one argument.
    만약 (filec <= 0)
	mainerr_arg_missing((char_u *)filev[-1]);

    // Temporarily cd to the current directory to handle relative file names.
    cwd = alloc(MAXPATHL);
    만약 (cwd == NULL)
	반환 NULL;
    만약 (mch_dirname(cwd, MAXPATHL) != OK)
    {
	vim_free(cwd);
	반환 NULL;
    }
    cdp.string = vim_strsave_escaped_ext(cwd,
# ifdef BACKSLASH_IN_FILENAME
	    (char_u *)"",  // rem_backslash() will tell what chars to escape
# 아니면
	    PATH_ESC_CHARS,
# endif
	    '\\', TRUE);
    vim_free(cwd);
    만약 (cdp.string == NULL)
	반환 NULL;
    cdp.length = STRLEN(cdp.string);
    ga_init2(&ga, 1, 100);
    GA_CONCAT_LITERAL(&ga, "<C-\\><C-N>:cd ");
    ga_concat_len(&ga, cdp.string, cdp.length);
    // reset wildignorecase temporarily
    ga_concat_len(&ga, wig[0].string, wig[0].length);

    // Call inputsave() so that a prompt for an encryption key works.
    GA_CONCAT_LITERAL(&ga, "<CR><C-\\><C-N>:if exists('*inputsave')|call inputsave()|endif|");
    만약 (tabs)
	GA_CONCAT_LITERAL(&ga, "tab ");
    GA_CONCAT_LITERAL(&ga, "drop");
    위해 (i = 0; i < filec; i++)
    {
	// On Unix the shell has already expanded the wildcards, don't want to
	// do it again in the Vim server.  On MS-Windows only escape
	// non-wildcard characters.
	p = vim_strsave_escaped((char_u *)filev[i],
# ifdef UNIX
		PATH_ESC_CHARS
# 아니면
		(char_u *)" \t%#"
# endif
		);
	만약 (p == NULL)
	{
	    vim_free(cdp.string);
	    vim_free(ga.ga_data);
	    반환 NULL;
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
# 아니면
    GA_CONCAT_LITERAL(&ga, "cd -|lcd -|elseif getcwd() ==# '");
# endif
    ga_concat_len(&ga, cdp.string, cdp.length);
    GA_CONCAT_LITERAL(&ga, "'|cd -|endif|endif<CR>");
    vim_free(cdp.string);
    // reset wildignorecase
    ga_concat_len(&ga, wig[1].string, wig[1].length);

    만약 (sendReply)
	GA_CONCAT_LITERAL(&ga, ":call SetupRemoteReplies()<CR>");
    GA_CONCAT_LITERAL(&ga, ":");
    만약 (inicmd != NULL)
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
    반환 ga.ga_data;
}

/*
 * Make our basic server name: use the specified "arg" if given, otherwise use
 * the tail of the command "cmd" we were started with.
 * Return the name in allocated memory.  This doesn't include a serial number.
 */
    static char_u *
serverMakeName(char_u *arg, char *cmd)
{
    char_u *p;

    만약 (arg != NULL && *arg != NUL)
    {
# ifdef FEAT_SOCKETSERVER
	// When using socketserver backend, do not change the name if path or
	// channel address.
	만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	{
	    만약 (STRNICMP(arg, "channel:", 8) == 0 || arg[0] == '/' ||
		    STRNCMP(arg, "./", 2) == 0 || STRNCMP(arg, "../", 3) == 0)
		p = vim_strsave(arg);
	    아니면
		p = vim_strsave_up(arg);
	}
	아니면
	    p = vim_strsave_up(arg);
# 아니면
	p = vim_strsave_up(arg);
# endif
    }
    아니면
    {
	p = vim_strsave_up(gettail((char_u *)cmd));
	// Remove .exe or .bat from the name.
	만약 (p != NULL && vim_strchr(p, '.') != NULL)
	    *vim_strchr(p, '.') = NUL;
    }
    반환 p;
}
#endif // FEAT_CLIENTSERVER

#만약 defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
    static void
make_connection(void)
{
    만약 (X_DISPLAY == NULL
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

    static 정수
check_connection(void)
{
    만약 (clientserver_method != CLIENTSERVER_METHOD_X11)
	반환 OK;
    make_connection();
    만약 (X_DISPLAY == NULL)
    {
	emsg(_(e_no_connection_to_x_server));
	반환 FAIL;
    }
    반환 OK;
}
#endif

#ifdef FEAT_CLIENTSERVER
    static void
remote_common(typval_T *argvars, typval_T *rettv, 정수 expr)
{
    char_u	*server_name;
    char_u	*keys;
    char_u	*r = NULL;
    char_u	buf[NUMBUFLEN];
    정수		timeout = 0;
# ifdef MSWIN
    HWND	w;
# 아니면
#  ifdef FEAT_X11
    Window	w;
#  endif
# endif
    정수		ret = OK;

    만약 (check_restricted() || check_secure())
	반환;

# ifdef FEAT_X11
    만약 (check_connection() == FAIL)
	반환;
# endif
    만약 (argvars[2].v_type != VAR_UNKNOWN
	    && argvars[3].v_type != VAR_UNKNOWN)
	timeout = tv_get_number(&argvars[3]);

    server_name = tv_get_string_chk(&argvars[0]);
    만약 (server_name == NULL)
	반환;		// type error; errmsg already given
    keys = tv_get_string_buf(&argvars[1], buf);
# ifdef MSWIN
    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendToVim(server_name, keys, &r, &w, expr,
		timeout, TRUE) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_SOCKETSERVER
    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET
	&& socketserver_send(server_name, keys, &r, expr,
	    timeout * 1000, true, NULL) < 0)
	ret = FAIL;
# endif
# ifdef FEAT_X11
    만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
	만약 (serverSendToVim(X_DISPLAY, server_name, keys, &r, &w, expr, timeout,
		    0, TRUE) < 0)
	ret = FAIL;
# endif
    만약 (ret == FAIL)
    {
	만약 (r != NULL)
	{
	    emsg((char *)r);	// sending worked but evaluation failed
	    vim_free(r);
	}
	아니면
	    semsg(_(e_unable_to_send_to_str), server_name);
	반환;
    }

    rettv->vval.v_string = r;

    만약 (argvars[2].v_type != VAR_UNKNOWN)
    {
	dictitem_T	v;
# 만약 defined(MSWIN) || defined(FEAT_X11)
	char_u		sbuf[30];
# endif
	char_u		*str = NULL;
	char_u		*idvar;

	idvar = tv_get_string_chk(&argvars[2]);
	만약 (idvar != NULL && *idvar != NUL)
	{
# ifdef MSWIN
	    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	    {
		sprintf((char *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_X11
	    만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
	    {
		sprintf((char *)sbuf, PRINTF_HEX_LONG_U, (long_u)w);
		str = sbuf;
	    }
# endif
# ifdef FEAT_SOCKETSERVER
	    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
		str = server_name;
# endif
	    만약 (str == NULL)
		str = (char_u *)"";

	    v.di_tv.v_type = VAR_STRING;
	    v.di_tv.vval.v_string = vim_strsave(str);
	    set_var(idvar, &v.di_tv, FALSE);
	    vim_free(v.di_tv.vval.v_string);
	}
    }
}
#endif

#만약 defined(FEAT_EVAL)
/*
 * "remote_expr()" function
 */
    void
f_remote_expr(typval_T *argvars UNUSED, typval_T *rettv)
{
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

# ifdef FEAT_CLIENTSERVER
    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_number_arg(argvars, 3) == FAIL)))
	반환;

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
    만약 (in_vim9script() && check_for_string_arg(argvars, 0) == FAIL)
	반환;

#  ifdef MSWIN
    // On Win32 it's done in this application.
    {
	char_u	*server_name = tv_get_string_chk(&argvars[0]);

	만약 (server_name != NULL)
	    serverForeground(server_name);
    }
#  아니면
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
    만약 (check_restricted() || check_secure())
	반환;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_string_arg(argvars, 1) == FAIL))
	반환;

    serverid = tv_get_string_chk(&argvars[0]);
    만약 (serverid == NULL)
	반환;		// type error; errmsg already given
#  ifdef MSWIN
    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
    {
	sscanf((const char *)serverid, SCANF_HEX_LONG_U, &n);
	만약 (n == 0)
	    rettv->vval.v_number = -1;
	아니면
	{
	    s = serverGetReply((HWND)n, FALSE, FALSE, FALSE, 0);
	    rettv->vval.v_number = (s != NULL);
	}
    }
#  endif
#  ifdef FEAT_SOCKETSERVER
    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	rettv->vval.v_number = socketserver_peek_reply(serverid, &s);
#  endif
#  ifdef FEAT_X11
    만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	만약 (check_connection() == FAIL)
	    반환;

	rettv->vval.v_number = serverPeekReply(X_DISPLAY,
		serverStrToWin(serverid), &s);
    }
#  endif

    만약 (argvars[1].v_type != VAR_UNKNOWN && rettv->vval.v_number > 0)
    {
	char_u		*retvar;

	v.di_tv.v_type = VAR_STRING;
	v.di_tv.vval.v_string = vim_strsave(s);
	retvar = tv_get_string_chk(&argvars[1]);
	만약 (retvar != NULL)
	    set_var(retvar, &v.di_tv, FALSE);
	vim_free(v.di_tv.vval.v_string);
    }
# 아니면
    rettv->vval.v_number = -1;
# endif
}

    void
f_remote_read(typval_T *argvars UNUSED, typval_T *rettv)
{
    char_u	*r = NULL;

# ifdef FEAT_CLIENTSERVER
    char_u	*serverid;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_number_arg(argvars, 1) == FAIL))
	반환;

    serverid = tv_get_string_chk(&argvars[0]);
    만약 (serverid != NULL && !check_restricted() && !check_secure())
    {
	정수 timeout = 0;
#  ifdef MSWIN
	// The server's HWND is encoded in the 'id' parameter
	long_u		n = 0;
#  endif

	만약 (argvars[1].v_type != VAR_UNKNOWN)
	    timeout = tv_get_number(&argvars[1]);

#  ifdef MSWIN
	만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	{
	    sscanf((char *)serverid, SCANF_HEX_LONG_U, &n);
	    만약 (n != 0)
		r = serverGetReply((HWND)n, FALSE, TRUE, TRUE, timeout);
	    만약 (r == NULL)
		emsg(_(e_unable_to_read_server_reply));
	}
#  endif
#  ifdef FEAT_SOCKETSERVER
	만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
		socketserver_read_reply(serverid, &r, timeout * 1000, false)
		== FAIL)
	    emsg(_(e_unable_to_read_server_reply));
#  endif
#  ifdef FEAT_X11
	만약 (clientserver_method == CLIENTSERVER_METHOD_X11 &&
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
    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL
		|| check_for_opt_string_arg(argvars, 2) == FAIL))
	반환;

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
    만약 (check_for_nonempty_string_arg(argvars, 0) == FAIL)
	반환;

    만약 (serverName != NULL)
    {
	emsg(_(e_already_started_server));
	반환;
    }

    char_u *server = tv_get_string_chk(&argvars[0]);
    만약 (server == NULL)
	반환;
#  ifdef MSWIN
    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	serverSetName(server);
#  endif
#  ifdef FEAT_SOCKETSERVER
    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	socketserver_start(server, true);
#  endif
#  ifdef FEAT_X11
    만약 (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    check_connection() == OK)
	serverRegisterName(X_DISPLAY, server);
#  endif

# 아니면
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
    정수		ret = OK;

    rettv->vval.v_number = -1;
    만약 (check_restricted() || check_secure())
	반환;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

    server = tv_get_string_chk(&argvars[0]);
    reply = tv_get_string_buf_chk(&argvars[1], buf);
    만약 (server == NULL || reply == NULL)
	반환;

#  ifdef FEAT_SOCKETSERVER
    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET &&
	    socketserver_send_reply(server, reply) == FAIL)
	ret = FAIL;
#  endif
#  ifdef FEAT_X11
    만약 (clientserver_method == CLIENTSERVER_METHOD_X11 &&
	    serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif
#  ifdef MSWIN
    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN
	    && serverSendReply(server, reply) < 0)
	ret = FAIL;
#  endif

    만약 (ret == FAIL)
    {
	emsg(_(e_unable_to_send_to_client));
	반환;
    }
    rettv->vval.v_number = 0;
# 아니면
    rettv->vval.v_number = -1;
# endif
}

    void
f_serverlist(typval_T *argvars UNUSED, typval_T *rettv)
{
    list_T  *list = NULL;
    bool    use_list = false;

    만약 (check_for_opt_dict_arg(argvars, 0) == FAIL)
	반환;

    만약 (argvars[0].v_type != VAR_UNKNOWN)
    {
	dict_T *d = argvars[0].vval.v_dict;

	use_list = dict_get_bool(d, "list", false);
    }

# ifdef FEAT_CLIENTSERVER
#  ifdef MSWIN
    만약 (clientserver_method == CLIENTSERVER_METHOD_MSWIN)
	list = serverGetVimNames();
#  endif
#  ifdef FEAT_SOCKETSERVER
    만약 (clientserver_method == CLIENTSERVER_METHOD_SOCKET)
	list = socketserver_list();
#  endif
#  ifdef FEAT_X11
    만약 (clientserver_method == CLIENTSERVER_METHOD_X11)
    {
	make_connection();
	만약 (X_DISPLAY != NULL)
	    list = serverGetVimNames(X_DISPLAY);
    }
#  endif
# endif
    만약 (use_list)
    {
	만약 (list == NULL)
	{
	    (void)rettv_list_alloc(rettv);
	    반환;
	}
	list->lv_refcount++;
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = list;
    }
    아니면
    {
	garray_T ga;

	ga_init2(&ga, 1, 80);

	만약 (list != NULL)
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

    만약 (env == NULL)
	반환;

    만약 (STRICMP(env, "socket") == 0)
	clientserver_method = CLIENTSERVER_METHOD_SOCKET;
# ifdef FEAT_X11
    아니면 만약 (STRICMP(env, "x11") == 0)
	clientserver_method = CLIENTSERVER_METHOD_X11;
# endif
# ifdef MSWIN
    아니면 만약 (STRICMP(env, "mswin") == 0)
	clientserver_method = CLIENTSERVER_METHOD_MSWIN;
# endif
}
#endif
