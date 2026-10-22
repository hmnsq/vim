/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * socketserver.c: Socketserver clientserver functionality
 */

#अन्तर्भाव "vim.h"

#ifdef FEAT_SOCKETSERVER

// Always greater than one.
# define PROTOCOL_VER 1

/*
 * Any message sent to a Vim server or received from a Vim server is a simple
 * JSON object with the following fields:
 * {
 *   "version": PROTOCOL_VER -- Protocol version, if different, then ignore.
 *
 *   "type": "expr"|"keystrokes"|"notify"|"reply"|"ver" -- The type of message
 *
 *   "str": string -- What to execute for the command or contents of result
 *
 *   ?"code": number -- Return code for expression
 *
 *   ?"sender": string -- Address of client that sent command if it is a server.
 *
 *   ?"wait": bool -- Used by --remote-wait since the client is not a server.
 * }
 */

/*
 * Represents a reply from a server2client call. Each client that calls a
 * server2client call to us has its own ss_reply_T. Each time a client sends
 * data using server2client, Vim creates a ss_reply_T if it doesn't exist and
 * adds the string to the array. When remote_read is called, the server id is
 * used to find the specific ss_reply_T, and a single string is popped from the
 * array.
 */
प्रकारनिर्देश संरचना
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

स्थिर channel_T    *server_channel = रिक्त;
स्थिर char_u	    *server_address = रिक्त; // Includes "type:" prefix if any
स्थिर bool	    server_is_unix = false;
स्थिर char_u	    *server_addr_cwd = रिक्त; // CWD when server was started, used
					  // to handle relative file paths.
स्थिर garray_T	    server_replies;

स्थिर channel_T    *client_channels = रिक्त;
स्थिर पूर्णाङ्क	    client_channel_गणनम् = 0;

# define FOR_ALL_CLIENTS(ch) \
    अत्थाय (ch = client_channels; ch != रिक्त; ch = ch->ch_ss_next)

स्थिर शून्य socketserver_cleanup(शून्य);
स्थिर char_u *socketserver_create_path(char_u *name, bool quiet);
स्थिर char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
स्थिर शून्य socketserver_accept(channel_T *channel);
स्थिर शून्य socketserver_close(channel_T *channel);
स्थिर ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    पूर्णाङ्क
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = रिक्त;
    पूर्णाङ्क		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    सचे (server_channel != रिक्त)
	पटिनिवत्त OK;

    सचे (STRNICMP(name, "channel:", 8) == 0)
    {
	सचे (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    पटिनिवत्त FAIL;
	सचे (address != रिक्त)
	    is_unix = true;
    }
    अञ्ञथा
    {
	address = socketserver_create_path(name, quiet);
	सचे (address == रिक्त)
	    पटिनिवत्त FAIL;
	is_unix = true;
    }

    सचे (is_unix)
	channel = channel_listen_unix((वर्ण *)address, रिक्त, false);
    अञ्ञथा
	channel = channel_listen(port, रिक्त);

    सचे (channel == रिक्त)
    {
	vim_free(address);
	पटिनिवत्त FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    सचे (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    अञ्ञथा
    {
	buf = alloc(MAXPATHL + 1);
	सचे (buf != रिक्त)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    सचे (buf != रिक्त)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    सचे (mch_dirname(dirbuf, परिमाण(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    अञ्ञथा
	server_addr_cwd = रिक्त;

    ga_init2(&server_replies, परिमाण(ss_reply_T), 2);

    ch_log(रिक्त, "socketserver: started server at %s", name);

    पटिनिवत्त OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    शून्य
socketserver_stop(शून्य)
{
    सचे (server_channel == रिक्त)
	पटिनिवत्त;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(रिक्त, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    स्थिर शून्य
socketserver_cleanup(शून्य)
{
# ifdef UNIX
    सचे (server_is_unix && server_address != रिक्त)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	सचे (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	अञ्ञथा सचे (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	सचे (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	अञ्ञथा सचे (server_addr_cwd != रिक्त
		&& mch_dirname(dirbuf, परिमाण(dirbuf)) == OK)
	{
	    सचे (mch_chdir((वर्ण *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		सचे (mch_chdir((वर्ण *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = रिक्त;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    अत्थाय (पूर्णाङ्क i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = (ss_reply_T *)server_replies.ga_data + i;

	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);
    }
    ga_clear(&server_replies);
}

/*
 * List available sockets that can be connected to, only in common directories
 * that Vim knows about. Vim instances with custom socket paths will not be
 * detected. Returns a list of strings (with reference count not set) on success
 * and NULL on failure.
 */
    list_T *
socketserver_list(शून्य)
{
    list_T *list = list_alloc();

    सचे (list == रिक्त)
	पटिनिवत्त रिक्त;

# ifdef MSWIN
    // Only support addresses on Windows
    पटिनिवत्त list;
# अञ्ञथा
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    संरचना dirent   *dp;
    ध्रुव char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    सचे ((buf.string = alloc(MAXPATHL)) == रिक्त)
	पटिनिवत्त रिक्त;
    सचे ((path.string = alloc(MAXPATHL)) == रिक्त)
    {
	vim_free(buf.string);
	पटिनिवत्त रिक्त;
    }
    buf.length = 0;
    path.length = 0;

    अत्थाय (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ध्रुव char_u *dir = known_dirs[i];

	सचे (dir == रिक्त)
	    अनुवर्त;

	सचे (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != रिक्त && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((वर्ण *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long पूर्णाङ्क)getuid());
	अञ्ञथा
	    path.length = vim_snprintf_safelen((वर्ण *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((वर्ण *)path.string);
	सचे (dirp == रिक्त)
	    अनुवर्त;

	// Loop through directory
	याव ((dp = readdir(dirp)) != रिक्त)
	{
	    सचे (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		अनुवर्त;

	    buf.length = vim_snprintf_safelen((वर्ण *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	भङ्ग;
    }

    vim_free(path.string);
    vim_free(buf.string);

    पटिनिवत्त list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    स्थिर char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    सचे (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	पटिनिवत्त vim_strsave(name + 8);
    अञ्ञथा
    {
	सचे (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	पटिनिवत्त रिक्त;
    }
# अञ्ञथा
    char_u  *buf = रिक्त;
    पूर्णाङ्क	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = रिक्त;
    bool    fatal = false;

    सचे (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	पटिनिवत्त vim_strsave(name + 8);
    सचे (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    अत्थाय (पूर्णाङ्क i = 0; i < 1000; i++)
    {
	सचे (buf != रिक्त)
	{
	    vim_snprintf((वर्ण *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	अञ्ञथा
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	सचे (fatal)
	    भङ्ग;

	सचे (path == रिक्त)
	{
	    सचे (buf == रिक्त)
	    {
		buf = alloc(buflen);
		सचे (buf == रिक्त)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    पटिनिवत्त रिक्त;
		}
	    }
	    अनुवर्त;
	}
	भङ्ग;
    }
    vim_free(buf);

    पटिनिवत्त path;
# endif
}

/*
 * If "name" is a pathless name such as "VIM", search known directories for the
 * socket named "name", and return the alloc'ed path to it. If "name" starts
 * with a '/', './' or '../', then a copy of "name" is returned.
 *
 * If "name" starts with "channel:", then return the address part
 *
 * If "new" is true, then return a path if the name does not exist in the known
 * location.
 *
 * If "ignore" is true, then don't emit an error message if a suitable path
 * could not be found.
 *
 * If "fatal" is not NULL, then it is set to true if error is fatal.
 *
 * Returns path on success and NULL on failure.
 */
    स्थिर char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    सचे (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	पटिनिवत्त vim_strsave(name + 8);
    अञ्ञथा
    {
	सचे (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	सचे (fatal != रिक्त)
	    *fatal = true;
	पटिनिवत्त रिक्त;
    }
# अञ्ञथा
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ध्रुव char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    सचे (name == रिक्त)
    {
	सचे (fatal != रिक्त)
	    *fatal = true;
	पटिनिवत्त रिक्त;
    }

    // Ignore if name is a path
    सचे (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	पटिनिवत्त vim_strsave(name);

    सचे (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	पटिनिवत्त vim_strsave(name + 8);

    सचे (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    सचे (vim_strchr(name, '/') != रिक्त)
    {
	सचे (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	सचे (fatal != रिक्त)
	    *fatal = true;
	पटिनिवत्त रिक्त;
    }

    buf = alloc(MAXPATHL);

    सचे (buf == रिक्त)
    {
	सचे (fatal != रिक्त)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	पटिनिवत्त रिक्त;
    }

    अत्थाय (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ध्रुव char_u	*dir = known_dirs[i];
	bool		got = false;

	सचे (dir == रिक्त)
	    अनुवर्त;
	अञ्ञथा सचे (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != रिक्त && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long पूर्णाङ्क)getuid());
	    सचे (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		अनुवर्त;

	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long पूर्णाङ्क)getuid(), name);
	}
	अञ्ञथा
	{
	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim", dir);
	    सचे (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		अनुवर्त;

	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	सचे (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((वर्ण *)buf, रिक्त);
	    emsg_off--;
	    emsg_silent--;

	    सचे (channel != रिक्त)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    अञ्ञथा
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	सचे (got || (!new && mch_access((वर्ण *)buf, F_OK) == 0))
	{
	    सचे (server_address != रिक्त && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		भङ्ग;
	    पटिनिवत्त buf;
	}
	भङ्ग;
    }

    सचे (!quiet)
    {
	सचे (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	अञ्ञथा सचे (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    सचे (!res && fatal != रिक्त)
	*fatal = true;

    vim_free(buf);
    पटिनिवत्त रिक्त;
# endif
}

/*
 * Callback for when client channel is closed
 */
    स्थिर शून्य
socketserver_client_close(channel_T *channel)
{
    सचे (channel == client_channels)
	client_channels = channel->ch_ss_next;
    सचे (channel->ch_ss_prev != रिक्त)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    सचे (channel->ch_ss_next != रिक्त)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    सचे (client_channel_गणनम् > 0)
	--client_channel_गणनम्;

    ch_log(रिक्त, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    स्थिर शून्य
socketserver_accept(channel_T *channel)
{
    सचे (client_channel_गणनम् >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(रिक्त, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	पटिनिवत्त;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = रिक्त;
    सचे (client_channels == रिक्त)
    {
	channel->ch_ss_prev = रिक्त;
	client_channels = channel;
    }
    अञ्ञथा
    {
	channel_T *last = client_channels;

	याव (last->ch_ss_next != रिक्त)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_गणनम्;

    // We will read the command from the client later in the input loop.
    ch_log(रिक्त, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    स्थिर शून्य
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(रिक्त, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    पूर्णाङ्क
set_ref_in_socketserver_channel(पूर्णाङ्क copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);

    FOR_ALL_CLIENTS(channel)
    {
	सचे (abort)
	    भङ्ग;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    पटिनिवत्त abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    स्थिर पूर्णाङ्क
socketserver_send_message(channel_T *ch, dict_T *सन्देशः, वर्ण *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    पूर्णाङ्क		ret;

    सचे (ver)
	dict_add_सङ्ख्या(सन्देशः, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = सन्देशः;
    buf = json_encode(&tv, JSON_NL);

    सचे (buf != रिक्त)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (पूर्णाङ्क)STRLEN(buf), func);
	emsg_silent--;
    }
    अञ्ञथा
	पटिनिवत्त FAIL;
    vim_free(buf);
    पटिनिवत्त ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    स्थिर शून्य
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = रिक्त;
    char_u	*sender = रिक्त;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    सचे (di == रिक्त || di->di_tv.v_type != VAR_STRING)
	पटिनिवत्त;
    अञ्ञथा
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    सचे (di != रिक्त && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    सचे (di != रिक्त && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    सचे (di != रिक्त && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_सङ्ख्या)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((वर्ण *)idbuf, परिमाण(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(रिक्त, "socketserver_exec(): result: %s",
	    str == रिक्त ? (char_u *)"(null)" : str);

    सचे (STRCMP(type, "expr") == 0 && str != रिक्त)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	पूर्णाङ्क	    code;

	dict = dict_alloc();
	सचे (dict == रिक्त)
	    पटिनिवत्त;

	result = eval_client_व्यञ्जकम्_to_string(str);
	code = result == रिक्त ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	सचे (result != रिक्त)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	अञ्ञथा
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_सङ्ख्या(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    अञ्ञथा सचे (STRCMP(type, "keystrokes") == 0 && str != रिक्त)
    {
	// Execute keystrokes
	server_to_निवेशः_buf(str);
    }
    अञ्ञथा सचे (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	सचे (sender != रिक्त && str != रिक्त)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    सचे (reply != रिक्त)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    अञ्ञथा सचे (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    अञ्ञथा
	ch_error(रिक्त, "socketserver: unknown command type '%s'", type);
}

    स्थिर पूर्णाङ्क
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_सन्देशः = head->jq_next;
    पूर्णाङ्क	    ver;

    सचे (json_सन्देशः == रिक्त)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_सन्देशः = head->jq_next;
    }
    सचे (json_सन्देशः == रिक्त)
	पटिनिवत्त FAIL;
    *tv = json_सन्देशः->jq_मानम्;
    remove_json_ग्रन्थिः(head, json_सन्देशः);

    सचे ((*tv)->v_type != VAR_DICT)
    {
	ch_error(रिक्त, "socketserver: message is not a JSON object");
	free_tv(*tv);
	पटिनिवत्त FAIL;
    }

    ver = dict_get_सङ्ख्या((*tv)->vval.v_dict, "version");

    सचे (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(रिक्त, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	सचे (dict != रिक्त)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	पटिनिवत्त FAIL;
    }

    पटिनिवत्त OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    शून्य
socketserver_parse_messages(शून्य)
{
    typval_T	*tv;

    अत्थाय (channel_T *ch = client_channels; ch != रिक्त;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	सचे (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    अनुवर्त;
	}

	socketserver_exec(ch, tv->vval.v_dict);
	free_tv(tv);
	ch = next;
    }
}


/*
 * Poll until there is something to read on "channel". Also handle other
 * socketserver channels in the meantime. If "channel" is NULL, then poll all
 * channels once then exit. If "timeout" is -1, then wait forever unless
 * interrupted.
 *
 * Return OK on success and FAIL on failure or timeout.
 */
    स्थिर पूर्णाङ्क
socketserver_wait(channel_T *channel, पूर्णाङ्क timeout)
{
    याव (true)
    {
	पूर्णाङ्क		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	संरचना timeval  tv;
	पूर्णाङ्क		maxfd = -1;

	सचे (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	सचे (channel != रिक्त)
	{
	    सचे (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		पटिनिवत्त FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	सचे (server_channel != रिक्त && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    सचे (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    सचे (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (पूर्णाङ्क)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		सचे (maxfd < (पूर्णाङ्क)ch->CH_SOCK_FD)
		    maxfd = (पूर्णाङ्क)ch->CH_SOCK_FD;
	    }
	}

	सचे (maxfd == -1)
	    पटिनिवत्त FAIL;

	ret = select(maxfd + 1, &rfds, रिक्त, रिक्त, timeout == -1 ? रिक्त : &tv);

#  ifdef EINTR
	सचे (ret == -1 && errno == EINTR)
	{
	    सचे (got_int)
		भङ्ग;
	    अनुवर्त;
	}
#  endif

	सचे (ret > 0)
	{
	    सचे (server_channel != रिक्त
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		सचे (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (पूर्णाङ्क)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    सचे (channel == रिक्त)
		पटिनिवत्त OK;

	    सचे (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		पटिनिवत्त OK;
	    }
	    अनुवर्त;
	}
# अञ्ञथा
	संरचना pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	पूर्णाङ्क		nfd = 0;
	पूर्णाङ्क		channel_idx = -1;
	पूर्णाङ्क		server_idx = -1;

	सचे (channel != रिक्त)
	{
	    सचे (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		पटिनिवत्त FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	सचे (server_channel != रिक्त && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    सचे (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	सचे (ret == -1 && errno == EINTR)
	{
	    सचे (got_int)
		भङ्ग;
	    अनुवर्त;
	}
#  endif

	सचे (ret > 0)
	{
	    सचे (server_channel != रिक्त
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		सचे (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    सचे (channel == रिक्त)
		पटिनिवत्त OK;

	    सचे (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		पटिनिवत्त OK;
	    }
	    अनुवर्त;
	}
# endif
	भङ्ग;
    }
    पटिनिवत्त FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    स्थिर channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = रिक्त;
    पूर्णाङ्क		port;
    bool	is_unix = false;
    channel_T	*channel;

    सचे (STRNICMP(name, "channel:", 8) == 0)
    {
	सचे (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    पटिनिवत्त रिक्त;
	सचे (address != रिक्त)
	    is_unix = true;
    }
    अञ्ञथा सचे (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	पूर्णाङ्क id;

	सचे (name[11] == NUL)
	    पटिनिवत्त रिक्त;
	id = strtol((वर्ण *)name + 11, रिक्त, 10);
	सचे (wait != रिक्त)
	    *wait = true;

	पटिनिवत्त channel_find(id);
    }
    अञ्ञथा
    {
	address = socketserver_get_path(name, false, quiet, false, रिक्त);
	सचे (address == रिक्त)
	    पटिनिवत्त रिक्त;
	is_unix = true;
    }

    सचे (is_unix)
	channel = channel_open_unix((वर्ण *)address, रिक्त);
    अञ्ञथा
	channel =  channel_open("localhost", port, 1000, रिक्त);

    सचे (channel == रिक्त && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    पटिनिवत्त channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    पूर्णाङ्क
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_व्यञ्जकम्,
	पूर्णाङ्क timeout,
	bool silent,
	channel_T **ch)
{
    पूर्णाङ्क		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = रिक्त;

    सचे (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	पटिनिवत्त -1;
    }

    // Execute locally if target is ourselves
    सचे (serverName != रिक्त && STRICMP(name, serverName) == 0)
	पटिनिवत्त sendToLocalVim(str, is_व्यञ्जकम्, result);

    channel = socketserver_get_channel(name, silent, रिक्त);
    सचे (channel == रिक्त)
	पटिनिवत्त -1;

    dict = dict_alloc();
    सचे (dict == रिक्त)
	गच्छ exit;

    dict_add_string(dict, "type", (char_u *)(is_व्यञ्जकम् ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    सचे (server_address != रिक्त)
	dict_add_string(dict, "sender", server_address);
    अञ्ञथा सचे (ch != रिक्त)
	dict_add_bool(dict, "wait", true);

    सचे (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	गच्छ exit;
    }
    dict_unref(dict);

    सचे (!is_व्यञ्जकम्)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	सचे (ch != रिक्त)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = रिक्त;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    पटिनिवत्त rcode;
	}
	गच्छ exit;
    }

    सचे (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    याव (socketserver_wait(channel, timeout) == OK)
	सचे (socketserver_get_message(channel, &resp_tv) == OK)
	    भङ्ग;

    सचे (resp_tv == रिक्त)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	गच्छ exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    सचे (di == रिक्त || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(रिक्त, "socketserver: unknown reply type");
	free_tv(resp_tv);
	गच्छ exit;
    }

    सचे (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	गच्छ exit;
    }

    सचे (result != रिक्त)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	सचे (di != रिक्त && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	अञ्ञथा
	{
	    free_tv(resp_tv);
	    गच्छ exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    सचे (di != रिक्त && di->di_tv.v_type == VAR_सङ्ख्या)
	rcode = di->di_tv.vval.v_सङ्ख्या;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    पटिनिवत्त rcode;
}

   स्थिर ss_reply_T *
socketserver_get_reply(char_u *sender, पूर्णाङ्क *index)
{
    अत्थाय (पूर्णाङ्क i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	सचे (STRCMP(reply->sender, sender) == 0)
	{
	    सचे (index != रिक्त)
		*index = i;
	    पटिनिवत्त reply;
	}
    }
    पटिनिवत्त रिक्त;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    स्थिर ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    सचे (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, परिमाण(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, रिक्त);

    सचे (reply == रिक्त && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	सचे (reply->sender == रिक्त)
	    पटिनिवत्त रिक्त;

	ga_init2(&reply->strings, परिमाण(char_u *), 5);
    }

    पटिनिवत्त reply;
}

    स्थिर शून्य
socketserver_remove_reply(char_u *sender)
{
    पूर्णाङ्क index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    सचे (reply != रिक्त)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	पूर्णाङ्क	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	सचे (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    परिमाण(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    पूर्णाङ्क
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    पूर्णाङ्क		ret = OK;
    bool	wait = false;

    सचे (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	पटिनिवत्त FAIL;
    }

    सचे (server_channel == रिक्त || server_address == रिक्त)
    {
	emsg(_(e_socket_server_not_online));
	पटिनिवत्त FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    सचे (channel == रिक्त)
	पटिनिवत्त FAIL;

    dict = dict_alloc();
    सचे (dict == रिक्त)
    {
	ret = FAIL;
	गच्छ exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    सचे (server_address != रिक्त)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    सचे (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    पटिनिवत्त ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    पूर्णाङ्क
socketserver_read_reply(
	char_u *client,
	char_u **str,
	पूर्णाङ्क timeout,
	bool remotewait)
{
    ss_reply_T	*reply = रिक्त;
    char_u	*actual;

    सचे (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	पटिनिवत्त FAIL;
    }

    सचे (!remotewait && (server_channel == रिक्त || server_address == रिक्त))
    {
	emsg(_(e_socket_server_not_online));
	पटिनिवत्त FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, रिक्त);
    सचे (actual == रिक्त)
	पटिनिवत्त FAIL;

    याव (true)
    {
	reply = socketserver_get_reply(actual, रिक्त);
	सचे (reply != रिक्त)
	    भङ्ग;
	सचे (socketserver_wait(रिक्त, timeout) == FAIL)
	    भङ्ग;
    }

    सचे (reply == रिक्त || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	पटिनिवत्त FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    सचे (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		परिमाण(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    सचे (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    पटिनिवत्त OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    पूर्णाङ्क
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    सचे (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	पटिनिवत्त FAIL;
    }

    सचे (server_channel == रिक्त || server_address == रिक्त)
    {
	emsg(_(e_socket_server_not_online));
	पटिनिवत्त FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, रिक्त);
    सचे (actual == रिक्त)
	पटिनिवत्त FAIL;

    reply = socketserver_get_reply(actual, रिक्त);
    vim_free(actual);

    सचे (reply != रिक्त && reply->strings.ga_len > 0)
    {
	सचे (str != रिक्त)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	पटिनिवत्त 1;
    }
    पटिनिवत्त 0;
}

#endif // FEAT_SOCKETSERVER
