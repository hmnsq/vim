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

#அந்தர்பாவ "vim.h"

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
வகைவரை கட்டமைப்பு
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

நிலையான channel_T    *server_channel = வெற்று;
நிலையான char_u	    *server_address = வெற்று; // Includes "type:" prefix if any
நிலையான bool	    server_is_unix = false;
நிலையான char_u	    *server_addr_cwd = வெற்று; // CWD when server was started, used
					  // to handle relative file paths.
நிலையான garray_T	    server_replies;

நிலையான channel_T    *client_channels = வெற்று;
நிலையான முழு	    client_channel_மொத்தம் = 0;

# define FOR_ALL_CLIENTS(ch) \
    க்கு (ch = client_channels; ch != வெற்று; ch = ch->ch_ss_next)

நிலையான வெறுமை socketserver_cleanup(வெறுமை);
நிலையான char_u *socketserver_create_path(char_u *name, bool quiet);
நிலையான char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
நிலையான வெறுமை socketserver_accept(channel_T *channel);
நிலையான வெறுமை socketserver_close(channel_T *channel);
நிலையான ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    முழு
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = வெற்று;
    முழு		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    எனில் (server_channel != வெற்று)
	திரும்பு OK;

    எனில் (STRNICMP(name, "channel:", 8) == 0)
    {
	எனில் (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    திரும்பு FAIL;
	எனில் (address != வெற்று)
	    is_unix = true;
    }
    இல்லையெனில்
    {
	address = socketserver_create_path(name, quiet);
	எனில் (address == வெற்று)
	    திரும்பு FAIL;
	is_unix = true;
    }

    எனில் (is_unix)
	channel = channel_listen_unix((எழுத்து *)address, வெற்று, false);
    இல்லையெனில்
	channel = channel_listen(port, வெற்று);

    எனில் (channel == வெற்று)
    {
	vim_free(address);
	திரும்பு FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    எனில் (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    இல்லையெனில்
    {
	buf = alloc(MAXPATHL + 1);
	எனில் (buf != வெற்று)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    எனில் (buf != வெற்று)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    எனில் (mch_dirname(dirbuf, அளவு(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    இல்லையெனில்
	server_addr_cwd = வெற்று;

    ga_init2(&server_replies, அளவு(ss_reply_T), 2);

    ch_log(வெற்று, "socketserver: started server at %s", name);

    திரும்பு OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    வெறுமை
socketserver_stop(வெறுமை)
{
    எனில் (server_channel == வெற்று)
	திரும்பு;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(வெற்று, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    நிலையான வெறுமை
socketserver_cleanup(வெறுமை)
{
# ifdef UNIX
    எனில் (server_is_unix && server_address != வெற்று)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	எனில் (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	இல்லையெனில் எனில் (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	எனில் (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	இல்லையெனில் எனில் (server_addr_cwd != வெற்று
		&& mch_dirname(dirbuf, அளவு(dirbuf)) == OK)
	{
	    எனில் (mch_chdir((எழுத்து *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		எனில் (mch_chdir((எழுத்து *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = வெற்று;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    க்கு (முழு i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(வெறுமை)
{
    list_T *list = list_alloc();

    எனில் (list == வெற்று)
	திரும்பு வெற்று;

# ifdef MSWIN
    // Only support addresses on Windows
    திரும்பு list;
# இல்லையெனில்
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    கட்டமைப்பு dirent   *dp;
    மாறிலி char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    எனில் ((buf.string = alloc(MAXPATHL)) == வெற்று)
	திரும்பு வெற்று;
    எனில் ((path.string = alloc(MAXPATHL)) == வெற்று)
    {
	vim_free(buf.string);
	திரும்பு வெற்று;
    }
    buf.length = 0;
    path.length = 0;

    க்கு (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	மாறிலி char_u *dir = known_dirs[i];

	எனில் (dir == வெற்று)
	    தொடர்;

	எனில் (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != வெற்று && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((எழுத்து *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long முழு)getuid());
	இல்லையெனில்
	    path.length = vim_snprintf_safelen((எழுத்து *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((எழுத்து *)path.string);
	எனில் (dirp == வெற்று)
	    தொடர்;

	// Loop through directory
	போது ((dp = readdir(dirp)) != வெற்று)
	{
	    எனில் (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		தொடர்;

	    buf.length = vim_snprintf_safelen((எழுத்து *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	நிறுத்து;
    }

    vim_free(path.string);
    vim_free(buf.string);

    திரும்பு list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    நிலையான char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    எனில் (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	திரும்பு vim_strsave(name + 8);
    இல்லையெனில்
    {
	எனில் (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	திரும்பு வெற்று;
    }
# இல்லையெனில்
    char_u  *buf = வெற்று;
    முழு	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = வெற்று;
    bool    fatal = false;

    எனில் (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	திரும்பு vim_strsave(name + 8);
    எனில் (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    க்கு (முழு i = 0; i < 1000; i++)
    {
	எனில் (buf != வெற்று)
	{
	    vim_snprintf((எழுத்து *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	இல்லையெனில்
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	எனில் (fatal)
	    நிறுத்து;

	எனில் (path == வெற்று)
	{
	    எனில் (buf == வெற்று)
	    {
		buf = alloc(buflen);
		எனில் (buf == வெற்று)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    திரும்பு வெற்று;
		}
	    }
	    தொடர்;
	}
	நிறுத்து;
    }
    vim_free(buf);

    திரும்பு path;
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
    நிலையான char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    எனில் (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	திரும்பு vim_strsave(name + 8);
    இல்லையெனில்
    {
	எனில் (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	எனில் (fatal != வெற்று)
	    *fatal = true;
	திரும்பு வெற்று;
    }
# இல்லையெனில்
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    மாறிலி char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    எனில் (name == வெற்று)
    {
	எனில் (fatal != வெற்று)
	    *fatal = true;
	திரும்பு வெற்று;
    }

    // Ignore if name is a path
    எனில் (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	திரும்பு vim_strsave(name);

    எனில் (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	திரும்பு vim_strsave(name + 8);

    எனில் (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    எனில் (vim_strchr(name, '/') != வெற்று)
    {
	எனில் (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	எனில் (fatal != வெற்று)
	    *fatal = true;
	திரும்பு வெற்று;
    }

    buf = alloc(MAXPATHL);

    எனில் (buf == வெற்று)
    {
	எனில் (fatal != வெற்று)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	திரும்பு வெற்று;
    }

    க்கு (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	மாறிலி char_u	*dir = known_dirs[i];
	bool		got = false;

	எனில் (dir == வெற்று)
	    தொடர்;
	இல்லையெனில் எனில் (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != வெற்று && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((எழுத்து *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long முழு)getuid());
	    எனில் (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		தொடர்;

	    vim_snprintf((எழுத்து *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long முழு)getuid(), name);
	}
	இல்லையெனில்
	{
	    vim_snprintf((எழுத்து *)buf, MAXPATHL, "%s/vim", dir);
	    எனில் (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		தொடர்;

	    vim_snprintf((எழுத்து *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	எனில் (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((எழுத்து *)buf, வெற்று);
	    emsg_off--;
	    emsg_silent--;

	    எனில் (channel != வெற்று)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    இல்லையெனில்
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	எனில் (got || (!new && mch_access((எழுத்து *)buf, F_OK) == 0))
	{
	    எனில் (server_address != வெற்று && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		நிறுத்து;
	    திரும்பு buf;
	}
	நிறுத்து;
    }

    எனில் (!quiet)
    {
	எனில் (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	இல்லையெனில் எனில் (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    எனில் (!res && fatal != வெற்று)
	*fatal = true;

    vim_free(buf);
    திரும்பு வெற்று;
# endif
}

/*
 * Callback for when client channel is closed
 */
    நிலையான வெறுமை
socketserver_client_close(channel_T *channel)
{
    எனில் (channel == client_channels)
	client_channels = channel->ch_ss_next;
    எனில் (channel->ch_ss_prev != வெற்று)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    எனில் (channel->ch_ss_next != வெற்று)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    எனில் (client_channel_மொத்தம் > 0)
	--client_channel_மொத்தம்;

    ch_log(வெற்று, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    நிலையான வெறுமை
socketserver_accept(channel_T *channel)
{
    எனில் (client_channel_மொத்தம் >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(வெற்று, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	திரும்பு;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = வெற்று;
    எனில் (client_channels == வெற்று)
    {
	channel->ch_ss_prev = வெற்று;
	client_channels = channel;
    }
    இல்லையெனில்
    {
	channel_T *last = client_channels;

	போது (last->ch_ss_next != வெற்று)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_மொத்தம்;

    // We will read the command from the client later in the input loop.
    ch_log(வெற்று, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    நிலையான வெறுமை
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(வெற்று, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    முழு
set_ref_in_socketserver_channel(முழு copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);

    FOR_ALL_CLIENTS(channel)
    {
	எனில் (abort)
	    நிறுத்து;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
    }
    திரும்பு abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    நிலையான முழு
socketserver_send_message(channel_T *ch, dict_T *செய்தி, எழுத்து *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    முழு		ret;

    எனில் (ver)
	dict_add_எண்(செய்தி, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = செய்தி;
    buf = json_encode(&tv, JSON_NL);

    எனில் (buf != வெற்று)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (முழு)STRLEN(buf), func);
	emsg_silent--;
    }
    இல்லையெனில்
	திரும்பு FAIL;
    vim_free(buf);
    திரும்பு ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    நிலையான வெறுமை
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = வெற்று;
    char_u	*sender = வெற்று;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    எனில் (di == வெற்று || di->di_tv.v_type != VAR_STRING)
	திரும்பு;
    இல்லையெனில்
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    எனில் (di != வெற்று && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    எனில் (di != வெற்று && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    எனில் (di != வெற்று && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_எண்)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((எழுத்து *)idbuf, அளவு(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(வெற்று, "socketserver_exec(): result: %s",
	    str == வெற்று ? (char_u *)"(null)" : str);

    எனில் (STRCMP(type, "expr") == 0 && str != வெற்று)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	முழு	    code;

	dict = dict_alloc();
	எனில் (dict == வெற்று)
	    திரும்பு;

	result = eval_client_கோவை_to_string(str);
	code = result == வெற்று ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	எனில் (result != வெற்று)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	இல்லையெனில்
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_எண்(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    இல்லையெனில் எனில் (STRCMP(type, "keystrokes") == 0 && str != வெற்று)
    {
	// Execute keystrokes
	server_to_உள்ளீடு_buf(str);
    }
    இல்லையெனில் எனில் (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	எனில் (sender != வெற்று && str != வெற்று)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    எனில் (reply != வெற்று)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    இல்லையெனில் எனில் (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    இல்லையெனில்
	ch_error(வெற்று, "socketserver: unknown command type '%s'", type);
}

    நிலையான முழு
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_செய்தி = head->jq_next;
    முழு	    ver;

    எனில் (json_செய்தி == வெற்று)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_செய்தி = head->jq_next;
    }
    எனில் (json_செய்தி == வெற்று)
	திரும்பு FAIL;
    *tv = json_செய்தி->jq_மதிப்பு;
    remove_json_முனை(head, json_செய்தி);

    எனில் ((*tv)->v_type != VAR_DICT)
    {
	ch_error(வெற்று, "socketserver: message is not a JSON object");
	free_tv(*tv);
	திரும்பு FAIL;
    }

    ver = dict_get_எண்((*tv)->vval.v_dict, "version");

    எனில் (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(வெற்று, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	எனில் (dict != வெற்று)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    வெறுமை
socketserver_parse_messages(வெறுமை)
{
    typval_T	*tv;

    க்கு (channel_T *ch = client_channels; ch != வெற்று;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	எனில் (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    தொடர்;
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
    நிலையான முழு
socketserver_wait(channel_T *channel, முழு timeout)
{
    போது (true)
    {
	முழு		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	கட்டமைப்பு timeval  tv;
	முழு		maxfd = -1;

	எனில் (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	எனில் (channel != வெற்று)
	{
	    எனில் (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		திரும்பு FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	எனில் (server_channel != வெற்று && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    எனில் (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    எனில் (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (முழு)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		எனில் (maxfd < (முழு)ch->CH_SOCK_FD)
		    maxfd = (முழு)ch->CH_SOCK_FD;
	    }
	}

	எனில் (maxfd == -1)
	    திரும்பு FAIL;

	ret = select(maxfd + 1, &rfds, வெற்று, வெற்று, timeout == -1 ? வெற்று : &tv);

#  ifdef EINTR
	எனில் (ret == -1 && errno == EINTR)
	{
	    எனில் (got_int)
		நிறுத்து;
	    தொடர்;
	}
#  endif

	எனில் (ret > 0)
	{
	    எனில் (server_channel != வெற்று
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		எனில் (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (முழு)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    எனில் (channel == வெற்று)
		திரும்பு OK;

	    எனில் (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		திரும்பு OK;
	    }
	    தொடர்;
	}
# இல்லையெனில்
	கட்டமைப்பு pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	முழு		nfd = 0;
	முழு		channel_idx = -1;
	முழு		server_idx = -1;

	எனில் (channel != வெற்று)
	{
	    எனில் (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		திரும்பு FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	எனில் (server_channel != வெற்று && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    எனில் (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	எனில் (ret == -1 && errno == EINTR)
	{
	    எனில் (got_int)
		நிறுத்து;
	    தொடர்;
	}
#  endif

	எனில் (ret > 0)
	{
	    எனில் (server_channel != வெற்று
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		எனில் (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    எனில் (channel == வெற்று)
		திரும்பு OK;

	    எனில் (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		திரும்பு OK;
	    }
	    தொடர்;
	}
# endif
	நிறுத்து;
    }
    திரும்பு FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    நிலையான channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = வெற்று;
    முழு		port;
    bool	is_unix = false;
    channel_T	*channel;

    எனில் (STRNICMP(name, "channel:", 8) == 0)
    {
	எனில் (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    திரும்பு வெற்று;
	எனில் (address != வெற்று)
	    is_unix = true;
    }
    இல்லையெனில் எனில் (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	முழு id;

	எனில் (name[11] == NUL)
	    திரும்பு வெற்று;
	id = strtol((எழுத்து *)name + 11, வெற்று, 10);
	எனில் (wait != வெற்று)
	    *wait = true;

	திரும்பு channel_find(id);
    }
    இல்லையெனில்
    {
	address = socketserver_get_path(name, false, quiet, false, வெற்று);
	எனில் (address == வெற்று)
	    திரும்பு வெற்று;
	is_unix = true;
    }

    எனில் (is_unix)
	channel = channel_open_unix((எழுத்து *)address, வெற்று);
    இல்லையெனில்
	channel =  channel_open("localhost", port, 1000, வெற்று);

    எனில் (channel == வெற்று && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    திரும்பு channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    முழு
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_கோவை,
	முழு timeout,
	bool silent,
	channel_T **ch)
{
    முழு		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = வெற்று;

    எனில் (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	திரும்பு -1;
    }

    // Execute locally if target is ourselves
    எனில் (serverName != வெற்று && STRICMP(name, serverName) == 0)
	திரும்பு sendToLocalVim(str, is_கோவை, result);

    channel = socketserver_get_channel(name, silent, வெற்று);
    எனில் (channel == வெற்று)
	திரும்பு -1;

    dict = dict_alloc();
    எனில் (dict == வெற்று)
	செல் exit;

    dict_add_string(dict, "type", (char_u *)(is_கோவை ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    எனில் (server_address != வெற்று)
	dict_add_string(dict, "sender", server_address);
    இல்லையெனில் எனில் (ch != வெற்று)
	dict_add_bool(dict, "wait", true);

    எனில் (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	செல் exit;
    }
    dict_unref(dict);

    எனில் (!is_கோவை)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	எனில் (ch != வெற்று)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = வெற்று;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    திரும்பு rcode;
	}
	செல் exit;
    }

    எனில் (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    போது (socketserver_wait(channel, timeout) == OK)
	எனில் (socketserver_get_message(channel, &resp_tv) == OK)
	    நிறுத்து;

    எனில் (resp_tv == வெற்று)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	செல் exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    எனில் (di == வெற்று || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(வெற்று, "socketserver: unknown reply type");
	free_tv(resp_tv);
	செல் exit;
    }

    எனில் (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	செல் exit;
    }

    எனில் (result != வெற்று)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	எனில் (di != வெற்று && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	இல்லையெனில்
	{
	    free_tv(resp_tv);
	    செல் exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    எனில் (di != வெற்று && di->di_tv.v_type == VAR_எண்)
	rcode = di->di_tv.vval.v_எண்;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    திரும்பு rcode;
}

   நிலையான ss_reply_T *
socketserver_get_reply(char_u *sender, முழு *index)
{
    க்கு (முழு i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	எனில் (STRCMP(reply->sender, sender) == 0)
	{
	    எனில் (index != வெற்று)
		*index = i;
	    திரும்பு reply;
	}
    }
    திரும்பு வெற்று;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    நிலையான ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    எனில் (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, அளவு(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, வெற்று);

    எனில் (reply == வெற்று && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	எனில் (reply->sender == வெற்று)
	    திரும்பு வெற்று;

	ga_init2(&reply->strings, அளவு(char_u *), 5);
    }

    திரும்பு reply;
}

    நிலையான வெறுமை
socketserver_remove_reply(char_u *sender)
{
    முழு index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    எனில் (reply != வெற்று)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	முழு	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	எனில் (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    அளவு(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    முழு
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    முழு		ret = OK;
    bool	wait = false;

    எனில் (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	திரும்பு FAIL;
    }

    எனில் (server_channel == வெற்று || server_address == வெற்று)
    {
	emsg(_(e_socket_server_not_online));
	திரும்பு FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    எனில் (channel == வெற்று)
	திரும்பு FAIL;

    dict = dict_alloc();
    எனில் (dict == வெற்று)
    {
	ret = FAIL;
	செல் exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    எனில் (server_address != வெற்று)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    எனில் (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    திரும்பு ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    முழு
socketserver_read_reply(
	char_u *client,
	char_u **str,
	முழு timeout,
	bool remotewait)
{
    ss_reply_T	*reply = வெற்று;
    char_u	*actual;

    எனில் (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	திரும்பு FAIL;
    }

    எனில் (!remotewait && (server_channel == வெற்று || server_address == வெற்று))
    {
	emsg(_(e_socket_server_not_online));
	திரும்பு FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, வெற்று);
    எனில் (actual == வெற்று)
	திரும்பு FAIL;

    போது (true)
    {
	reply = socketserver_get_reply(actual, வெற்று);
	எனில் (reply != வெற்று)
	    நிறுத்து;
	எனில் (socketserver_wait(வெற்று, timeout) == FAIL)
	    நிறுத்து;
    }

    எனில் (reply == வெற்று || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	திரும்பு FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    எனில் (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		அளவு(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    எனில் (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    திரும்பு OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    முழு
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    எனில் (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	திரும்பு FAIL;
    }

    எனில் (server_channel == வெற்று || server_address == வெற்று)
    {
	emsg(_(e_socket_server_not_online));
	திரும்பு FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, வெற்று);
    எனில் (actual == வெற்று)
	திரும்பு FAIL;

    reply = socketserver_get_reply(actual, வெற்று);
    vim_free(actual);

    எனில் (reply != வெற்று && reply->strings.ga_len > 0)
    {
	எனில் (str != வெற்று)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	திரும்பு 1;
    }
    திரும்பு 0;
}

#endif // FEAT_SOCKETSERVER
