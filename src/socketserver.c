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
    प्रति (ch = client_channels; ch != रिक्त; ch = ch->ch_ss_next)

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

    जइ (server_channel != रिक्त)
	प्रत्यागम OK;

    जइ (STRNICMP(name, "channel:", 8) == 0)
    {
	जइ (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    प्रत्यागम FAIL;
	जइ (address != रिक्त)
	    is_unix = true;
    }
    अण्णहा
    {
	address = socketserver_create_path(name, quiet);
	जइ (address == रिक्त)
	    प्रत्यागम FAIL;
	is_unix = true;
    }

    जइ (is_unix)
	channel = channel_listen_unix((वर्ण *)address, रिक्त, false);
    अण्णहा
	channel = channel_listen(port, रिक्त);

    जइ (channel == रिक्त)
    {
	vim_free(address);
	प्रत्यागम FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    जइ (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    अण्णहा
    {
	buf = alloc(MAXPATHL + 1);
	जइ (buf != रिक्त)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    जइ (buf != रिक्त)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    जइ (mch_dirname(dirbuf, परिमाण(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    अण्णहा
	server_addr_cwd = रिक्त;

    ga_init2(&server_replies, परिमाण(ss_reply_T), 2);

    ch_log(रिक्त, "socketserver: started server at %s", name);

    प्रत्यागम OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    शून्य
socketserver_stop(शून्य)
{
    जइ (server_channel == रिक्त)
	प्रत्यागम;

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
    जइ (server_is_unix && server_address != रिक्त)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	जइ (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	अण्णहा जइ (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	जइ (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	अण्णहा जइ (server_addr_cwd != रिक्त
		&& mch_dirname(dirbuf, परिमाण(dirbuf)) == OK)
	{
	    जइ (mch_chdir((वर्ण *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		जइ (mch_chdir((वर्ण *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = रिक्त;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    प्रति (पूर्णाङ्क i = 0; i < server_replies.ga_len; i++)
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

    जइ (list == रिक्त)
	प्रत्यागम रिक्त;

# ifdef MSWIN
    // Only support addresses on Windows
    प्रत्यागम list;
# अण्णहा
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    संरचना dirent   *dp;
    ध्रुव char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    जइ ((buf.string = alloc(MAXPATHL)) == रिक्त)
	प्रत्यागम रिक्त;
    जइ ((path.string = alloc(MAXPATHL)) == रिक्त)
    {
	vim_free(buf.string);
	प्रत्यागम रिक्त;
    }
    buf.length = 0;
    path.length = 0;

    प्रति (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ध्रुव char_u *dir = known_dirs[i];

	जइ (dir == रिक्त)
	    अनुवर्त;

	जइ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != रिक्त && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((वर्ण *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long पूर्णाङ्क)getuid());
	अण्णहा
	    path.length = vim_snprintf_safelen((वर्ण *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((वर्ण *)path.string);
	जइ (dirp == रिक्त)
	    अनुवर्त;

	// Loop through directory
	जाव ((dp = readdir(dirp)) != रिक्त)
	{
	    जइ (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
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

    प्रत्यागम list;
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
    जइ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	प्रत्यागम vim_strsave(name + 8);
    अण्णहा
    {
	जइ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	प्रत्यागम रिक्त;
    }
# अण्णहा
    char_u  *buf = रिक्त;
    पूर्णाङ्क	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = रिक्त;
    bool    fatal = false;

    जइ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	प्रत्यागम vim_strsave(name + 8);
    जइ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    प्रति (पूर्णाङ्क i = 0; i < 1000; i++)
    {
	जइ (buf != रिक्त)
	{
	    vim_snprintf((वर्ण *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	अण्णहा
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	जइ (fatal)
	    भङ्ग;

	जइ (path == रिक्त)
	{
	    जइ (buf == रिक्त)
	    {
		buf = alloc(buflen);
		जइ (buf == रिक्त)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    प्रत्यागम रिक्त;
		}
	    }
	    अनुवर्त;
	}
	भङ्ग;
    }
    vim_free(buf);

    प्रत्यागम path;
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
    जइ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	प्रत्यागम vim_strsave(name + 8);
    अण्णहा
    {
	जइ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	जइ (fatal != रिक्त)
	    *fatal = true;
	प्रत्यागम रिक्त;
    }
# अण्णहा
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ध्रुव char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    जइ (name == रिक्त)
    {
	जइ (fatal != रिक्त)
	    *fatal = true;
	प्रत्यागम रिक्त;
    }

    // Ignore if name is a path
    जइ (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	प्रत्यागम vim_strsave(name);

    जइ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	प्रत्यागम vim_strsave(name + 8);

    जइ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    जइ (vim_strchr(name, '/') != रिक्त)
    {
	जइ (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	जइ (fatal != रिक्त)
	    *fatal = true;
	प्रत्यागम रिक्त;
    }

    buf = alloc(MAXPATHL);

    जइ (buf == रिक्त)
    {
	जइ (fatal != रिक्त)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	प्रत्यागम रिक्त;
    }

    प्रति (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ध्रुव char_u	*dir = known_dirs[i];
	bool		got = false;

	जइ (dir == रिक्त)
	    अनुवर्त;
	अण्णहा जइ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != रिक्त && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long पूर्णाङ्क)getuid());
	    जइ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		अनुवर्त;

	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long पूर्णाङ्क)getuid(), name);
	}
	अण्णहा
	{
	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim", dir);
	    जइ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		अनुवर्त;

	    vim_snprintf((वर्ण *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	जइ (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((वर्ण *)buf, रिक्त);
	    emsg_off--;
	    emsg_silent--;

	    जइ (channel != रिक्त)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    अण्णहा
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	जइ (got || (!new && mch_access((वर्ण *)buf, F_OK) == 0))
	{
	    जइ (server_address != रिक्त && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		भङ्ग;
	    प्रत्यागम buf;
	}
	भङ्ग;
    }

    जइ (!quiet)
    {
	जइ (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	अण्णहा जइ (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    जइ (!res && fatal != रिक्त)
	*fatal = true;

    vim_free(buf);
    प्रत्यागम रिक्त;
# endif
}

/*
 * Callback for when client channel is closed
 */
    स्थिर शून्य
socketserver_client_close(channel_T *channel)
{
    जइ (channel == client_channels)
	client_channels = channel->ch_ss_next;
    जइ (channel->ch_ss_prev != रिक्त)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    जइ (channel->ch_ss_next != रिक्त)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    जइ (client_channel_गणनम् > 0)
	--client_channel_गणनम्;

    ch_log(रिक्त, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    स्थिर शून्य
socketserver_accept(channel_T *channel)
{
    जइ (client_channel_गणनम् >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(रिक्त, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	प्रत्यागम;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = रिक्त;
    जइ (client_channels == रिक्त)
    {
	channel->ch_ss_prev = रिक्त;
	client_channels = channel;
    }
    अण्णहा
    {
	channel_T *last = client_channels;

	जाव (last->ch_ss_next != रिक्त)
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
	जइ (abort)
	    भङ्ग;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    प्रत्यागम abort;
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

    जइ (ver)
	dict_add_सङ्ख्या(सन्देशः, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = सन्देशः;
    buf = json_encode(&tv, JSON_NL);

    जइ (buf != रिक्त)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (पूर्णाङ्क)STRLEN(buf), func);
	emsg_silent--;
    }
    अण्णहा
	प्रत्यागम FAIL;
    vim_free(buf);
    प्रत्यागम ret;
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
    जइ (di == रिक्त || di->di_tv.v_type != VAR_STRING)
	प्रत्यागम;
    अण्णहा
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    जइ (di != रिक्त && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    जइ (di != रिक्त && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    जइ (di != रिक्त && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_सङ्ख्या)
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

    जइ (STRCMP(type, "expr") == 0 && str != रिक्त)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	पूर्णाङ्क	    code;

	dict = dict_alloc();
	जइ (dict == रिक्त)
	    प्रत्यागम;

	result = eval_client_व्यञ्जकम्_to_string(str);
	code = result == रिक्त ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	जइ (result != रिक्त)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	अण्णहा
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_सङ्ख्या(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    अण्णहा जइ (STRCMP(type, "keystrokes") == 0 && str != रिक्त)
    {
	// Execute keystrokes
	server_to_निवेशः_buf(str);
    }
    अण्णहा जइ (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	जइ (sender != रिक्त && str != रिक्त)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    जइ (reply != रिक्त)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    अण्णहा जइ (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    अण्णहा
	ch_error(रिक्त, "socketserver: unknown command type '%s'", type);
}

    स्थिर पूर्णाङ्क
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_सन्देशः = head->jq_next;
    पूर्णाङ्क	    ver;

    जइ (json_सन्देशः == रिक्त)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_सन्देशः = head->jq_next;
    }
    जइ (json_सन्देशः == रिक्त)
	प्रत्यागम FAIL;
    *tv = json_सन्देशः->jq_मानम्;
    remove_json_ग्रन्थिः(head, json_सन्देशः);

    जइ ((*tv)->v_type != VAR_DICT)
    {
	ch_error(रिक्त, "socketserver: message is not a JSON object");
	free_tv(*tv);
	प्रत्यागम FAIL;
    }

    ver = dict_get_सङ्ख्या((*tv)->vval.v_dict, "version");

    जइ (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(रिक्त, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	जइ (dict != रिक्त)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    शून्य
socketserver_parse_messages(शून्य)
{
    typval_T	*tv;

    प्रति (channel_T *ch = client_channels; ch != रिक्त;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	जइ (socketserver_get_message(ch, &tv) == FAIL)
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
    जाव (true)
    {
	पूर्णाङ्क		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	संरचना timeval  tv;
	पूर्णाङ्क		maxfd = -1;

	जइ (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	जइ (channel != रिक्त)
	{
	    जइ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		प्रत्यागम FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	जइ (server_channel != रिक्त && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    जइ (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    जइ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (पूर्णाङ्क)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		जइ (maxfd < (पूर्णाङ्क)ch->CH_SOCK_FD)
		    maxfd = (पूर्णाङ्क)ch->CH_SOCK_FD;
	    }
	}

	जइ (maxfd == -1)
	    प्रत्यागम FAIL;

	ret = select(maxfd + 1, &rfds, रिक्त, रिक्त, timeout == -1 ? रिक्त : &tv);

#  ifdef EINTR
	जइ (ret == -1 && errno == EINTR)
	{
	    जइ (got_int)
		भङ्ग;
	    अनुवर्त;
	}
#  endif

	जइ (ret > 0)
	{
	    जइ (server_channel != रिक्त
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		जइ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (पूर्णाङ्क)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    जइ (channel == रिक्त)
		प्रत्यागम OK;

	    जइ (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		प्रत्यागम OK;
	    }
	    अनुवर्त;
	}
# अण्णहा
	संरचना pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	पूर्णाङ्क		nfd = 0;
	पूर्णाङ्क		channel_idx = -1;
	पूर्णाङ्क		server_idx = -1;

	जइ (channel != रिक्त)
	{
	    जइ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		प्रत्यागम FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	जइ (server_channel != रिक्त && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    जइ (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	जइ (ret == -1 && errno == EINTR)
	{
	    जइ (got_int)
		भङ्ग;
	    अनुवर्त;
	}
#  endif

	जइ (ret > 0)
	{
	    जइ (server_channel != रिक्त
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		जइ (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    जइ (channel == रिक्त)
		प्रत्यागम OK;

	    जइ (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		प्रत्यागम OK;
	    }
	    अनुवर्त;
	}
# endif
	भङ्ग;
    }
    प्रत्यागम FAIL;
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

    जइ (STRNICMP(name, "channel:", 8) == 0)
    {
	जइ (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    प्रत्यागम रिक्त;
	जइ (address != रिक्त)
	    is_unix = true;
    }
    अण्णहा जइ (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	पूर्णाङ्क id;

	जइ (name[11] == NUL)
	    प्रत्यागम रिक्त;
	id = strtol((वर्ण *)name + 11, रिक्त, 10);
	जइ (wait != रिक्त)
	    *wait = true;

	प्रत्यागम channel_find(id);
    }
    अण्णहा
    {
	address = socketserver_get_path(name, false, quiet, false, रिक्त);
	जइ (address == रिक्त)
	    प्रत्यागम रिक्त;
	is_unix = true;
    }

    जइ (is_unix)
	channel = channel_open_unix((वर्ण *)address, रिक्त);
    अण्णहा
	channel =  channel_open("localhost", port, 1000, रिक्त);

    जइ (channel == रिक्त && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    प्रत्यागम channel;
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

    जइ (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	प्रत्यागम -1;
    }

    // Execute locally if target is ourselves
    जइ (serverName != रिक्त && STRICMP(name, serverName) == 0)
	प्रत्यागम sendToLocalVim(str, is_व्यञ्जकम्, result);

    channel = socketserver_get_channel(name, silent, रिक्त);
    जइ (channel == रिक्त)
	प्रत्यागम -1;

    dict = dict_alloc();
    जइ (dict == रिक्त)
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
    जइ (server_address != रिक्त)
	dict_add_string(dict, "sender", server_address);
    अण्णहा जइ (ch != रिक्त)
	dict_add_bool(dict, "wait", true);

    जइ (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	गच्छ exit;
    }
    dict_unref(dict);

    जइ (!is_व्यञ्जकम्)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	जइ (ch != रिक्त)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = रिक्त;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    प्रत्यागम rcode;
	}
	गच्छ exit;
    }

    जइ (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    जाव (socketserver_wait(channel, timeout) == OK)
	जइ (socketserver_get_message(channel, &resp_tv) == OK)
	    भङ्ग;

    जइ (resp_tv == रिक्त)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	गच्छ exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    जइ (di == रिक्त || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(रिक्त, "socketserver: unknown reply type");
	free_tv(resp_tv);
	गच्छ exit;
    }

    जइ (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	गच्छ exit;
    }

    जइ (result != रिक्त)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	जइ (di != रिक्त && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	अण्णहा
	{
	    free_tv(resp_tv);
	    गच्छ exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    जइ (di != रिक्त && di->di_tv.v_type == VAR_सङ्ख्या)
	rcode = di->di_tv.vval.v_सङ्ख्या;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    प्रत्यागम rcode;
}

   स्थिर ss_reply_T *
socketserver_get_reply(char_u *sender, पूर्णाङ्क *index)
{
    प्रति (पूर्णाङ्क i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	जइ (STRCMP(reply->sender, sender) == 0)
	{
	    जइ (index != रिक्त)
		*index = i;
	    प्रत्यागम reply;
	}
    }
    प्रत्यागम रिक्त;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    स्थिर ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    जइ (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, परिमाण(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, रिक्त);

    जइ (reply == रिक्त && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	जइ (reply->sender == रिक्त)
	    प्रत्यागम रिक्त;

	ga_init2(&reply->strings, परिमाण(char_u *), 5);
    }

    प्रत्यागम reply;
}

    स्थिर शून्य
socketserver_remove_reply(char_u *sender)
{
    पूर्णाङ्क index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    जइ (reply != रिक्त)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	पूर्णाङ्क	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	जइ (len > 1)
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

    जइ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	प्रत्यागम FAIL;
    }

    जइ (server_channel == रिक्त || server_address == रिक्त)
    {
	emsg(_(e_socket_server_not_online));
	प्रत्यागम FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    जइ (channel == रिक्त)
	प्रत्यागम FAIL;

    dict = dict_alloc();
    जइ (dict == रिक्त)
    {
	ret = FAIL;
	गच्छ exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    जइ (server_address != रिक्त)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    जइ (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    प्रत्यागम ret;
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

    जइ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	प्रत्यागम FAIL;
    }

    जइ (!remotewait && (server_channel == रिक्त || server_address == रिक्त))
    {
	emsg(_(e_socket_server_not_online));
	प्रत्यागम FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, रिक्त);
    जइ (actual == रिक्त)
	प्रत्यागम FAIL;

    जाव (true)
    {
	reply = socketserver_get_reply(actual, रिक्त);
	जइ (reply != रिक्त)
	    भङ्ग;
	जइ (socketserver_wait(रिक्त, timeout) == FAIL)
	    भङ्ग;
    }

    जइ (reply == रिक्त || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	प्रत्यागम FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    जइ (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		परिमाण(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    जइ (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    प्रत्यागम OK;
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

    जइ (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	प्रत्यागम FAIL;
    }

    जइ (server_channel == रिक्त || server_address == रिक्त)
    {
	emsg(_(e_socket_server_not_online));
	प्रत्यागम FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, रिक्त);
    जइ (actual == रिक्त)
	प्रत्यागम FAIL;

    reply = socketserver_get_reply(actual, रिक्त);
    vim_free(actual);

    जइ (reply != रिक्त && reply->strings.ga_len > 0)
    {
	जइ (str != रिक्त)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	प्रत्यागम 1;
    }
    प्रत्यागम 0;
}

#endif // FEAT_SOCKETSERVER
