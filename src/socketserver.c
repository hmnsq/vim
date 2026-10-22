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

#অন্তর্ভাৱ "vim.h"

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
প্রকারনির্দেশ সংরচনা
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

স্থির channel_T    *server_channel = রিক্ত;
স্থির char_u	    *server_address = রিক্ত; // Includes "type:" prefix if any
স্থির bool	    server_is_unix = false;
স্থির char_u	    *server_addr_cwd = রিক্ত; // CWD when server was started, used
					  // to handle relative file paths.
স্থির garray_T	    server_replies;

স্থির channel_T    *client_channels = রিক্ত;
স্থির পূর্ণাঙ্ক	    client_channel_গুনতি = 0;

# define FOR_ALL_CLIENTS(ch) \
    বাবে (ch = client_channels; ch != রিক্ত; ch = ch->ch_ss_next)

স্থির শূন্য socketserver_cleanup(শূন্য);
স্থির char_u *socketserver_create_path(char_u *name, bool quiet);
স্থির char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
স্থির শূন্য socketserver_accept(channel_T *channel);
স্থির শূন্য socketserver_close(channel_T *channel);
স্থির ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    পূর্ণাঙ্ক
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = রিক্ত;
    পূর্ণাঙ্ক		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    যদি (server_channel != রিক্ত)
	ঘূৰাই OK;

    যদি (STRNICMP(name, "channel:", 8) == 0)
    {
	যদি (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    ঘূৰাই FAIL;
	যদি (address != রিক্ত)
	    is_unix = true;
    }
    অন্যথা
    {
	address = socketserver_create_path(name, quiet);
	যদি (address == রিক্ত)
	    ঘূৰাই FAIL;
	is_unix = true;
    }

    যদি (is_unix)
	channel = channel_listen_unix((ৱর্ণ *)address, রিক্ত, false);
    অন্যথা
	channel = channel_listen(port, রিক্ত);

    যদি (channel == রিক্ত)
    {
	vim_free(address);
	ঘূৰাই FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    যদি (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    অন্যথা
    {
	buf = alloc(MAXPATHL + 1);
	যদি (buf != রিক্ত)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    যদি (buf != রিক্ত)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    যদি (mch_dirname(dirbuf, পরিমাণ(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    অন্যথা
	server_addr_cwd = রিক্ত;

    ga_init2(&server_replies, পরিমাণ(ss_reply_T), 2);

    ch_log(রিক্ত, "socketserver: started server at %s", name);

    ঘূৰাই OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    শূন্য
socketserver_stop(শূন্য)
{
    যদি (server_channel == রিক্ত)
	ঘূৰাই;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(রিক্ত, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    স্থির শূন্য
socketserver_cleanup(শূন্য)
{
# ifdef UNIX
    যদি (server_is_unix && server_address != রিক্ত)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	যদি (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	অন্যথা যদি (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	যদি (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	অন্যথা যদি (server_addr_cwd != রিক্ত
		&& mch_dirname(dirbuf, পরিমাণ(dirbuf)) == OK)
	{
	    যদি (mch_chdir((ৱর্ণ *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		যদি (mch_chdir((ৱর্ণ *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = রিক্ত;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    বাবে (পূর্ণাঙ্ক i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(শূন্য)
{
    list_T *list = list_alloc();

    যদি (list == রিক্ত)
	ঘূৰাই রিক্ত;

# ifdef MSWIN
    // Only support addresses on Windows
    ঘূৰাই list;
# অন্যথা
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    সংরচনা dirent   *dp;
    ধ্রুৱ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    যদি ((buf.string = alloc(MAXPATHL)) == রিক্ত)
	ঘূৰাই রিক্ত;
    যদি ((path.string = alloc(MAXPATHL)) == রিক্ত)
    {
	vim_free(buf.string);
	ঘূৰাই রিক্ত;
    }
    buf.length = 0;
    path.length = 0;

    বাবে (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ধ্রুৱ char_u *dir = known_dirs[i];

	যদি (dir == রিক্ত)
	    অনুৱর্ত;

	যদি (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != রিক্ত && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((ৱর্ণ *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long পূর্ণাঙ্ক)getuid());
	অন্যথা
	    path.length = vim_snprintf_safelen((ৱর্ণ *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((ৱর্ণ *)path.string);
	যদি (dirp == রিক্ত)
	    অনুৱর্ত;

	// Loop through directory
	যেতিয়ালৈ ((dp = readdir(dirp)) != রিক্ত)
	{
	    যদি (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		অনুৱর্ত;

	    buf.length = vim_snprintf_safelen((ৱর্ণ *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	ভঙ্গ;
    }

    vim_free(path.string);
    vim_free(buf.string);

    ঘূৰাই list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    স্থির char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    যদি (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ঘূৰাই vim_strsave(name + 8);
    অন্যথা
    {
	যদি (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ঘূৰাই রিক্ত;
    }
# অন্যথা
    char_u  *buf = রিক্ত;
    পূর্ণাঙ্ক	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = রিক্ত;
    bool    fatal = false;

    যদি (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ঘূৰাই vim_strsave(name + 8);
    যদি (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    বাবে (পূর্ণাঙ্ক i = 0; i < 1000; i++)
    {
	যদি (buf != রিক্ত)
	{
	    vim_snprintf((ৱর্ণ *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	অন্যথা
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	যদি (fatal)
	    ভঙ্গ;

	যদি (path == রিক্ত)
	{
	    যদি (buf == রিক্ত)
	    {
		buf = alloc(buflen);
		যদি (buf == রিক্ত)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    ঘূৰাই রিক্ত;
		}
	    }
	    অনুৱর্ত;
	}
	ভঙ্গ;
    }
    vim_free(buf);

    ঘূৰাই path;
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
    স্থির char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    যদি (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ঘূৰাই vim_strsave(name + 8);
    অন্যথা
    {
	যদি (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	যদি (fatal != রিক্ত)
	    *fatal = true;
	ঘূৰাই রিক্ত;
    }
# অন্যথা
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ধ্রুৱ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    যদি (name == রিক্ত)
    {
	যদি (fatal != রিক্ত)
	    *fatal = true;
	ঘূৰাই রিক্ত;
    }

    // Ignore if name is a path
    যদি (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	ঘূৰাই vim_strsave(name);

    যদি (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ঘূৰাই vim_strsave(name + 8);

    যদি (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    যদি (vim_strchr(name, '/') != রিক্ত)
    {
	যদি (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	যদি (fatal != রিক্ত)
	    *fatal = true;
	ঘূৰাই রিক্ত;
    }

    buf = alloc(MAXPATHL);

    যদি (buf == রিক্ত)
    {
	যদি (fatal != রিক্ত)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	ঘূৰাই রিক্ত;
    }

    বাবে (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ধ্রুৱ char_u	*dir = known_dirs[i];
	bool		got = false;

	যদি (dir == রিক্ত)
	    অনুৱর্ত;
	অন্যথা যদি (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != রিক্ত && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((ৱর্ণ *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long পূর্ণাঙ্ক)getuid());
	    যদি (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		অনুৱর্ত;

	    vim_snprintf((ৱর্ণ *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long পূর্ণাঙ্ক)getuid(), name);
	}
	অন্যথা
	{
	    vim_snprintf((ৱর্ণ *)buf, MAXPATHL, "%s/vim", dir);
	    যদি (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		অনুৱর্ত;

	    vim_snprintf((ৱর্ণ *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	যদি (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((ৱর্ণ *)buf, রিক্ত);
	    emsg_off--;
	    emsg_silent--;

	    যদি (channel != রিক্ত)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    অন্যথা
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	যদি (got || (!new && mch_access((ৱর্ণ *)buf, F_OK) == 0))
	{
	    যদি (server_address != রিক্ত && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		ভঙ্গ;
	    ঘূৰাই buf;
	}
	ভঙ্গ;
    }

    যদি (!quiet)
    {
	যদি (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	অন্যথা যদি (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    যদি (!res && fatal != রিক্ত)
	*fatal = true;

    vim_free(buf);
    ঘূৰাই রিক্ত;
# endif
}

/*
 * Callback for when client channel is closed
 */
    স্থির শূন্য
socketserver_client_close(channel_T *channel)
{
    যদি (channel == client_channels)
	client_channels = channel->ch_ss_next;
    যদি (channel->ch_ss_prev != রিক্ত)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    যদি (channel->ch_ss_next != রিক্ত)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    যদি (client_channel_গুনতি > 0)
	--client_channel_গুনতি;

    ch_log(রিক্ত, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    স্থির শূন্য
socketserver_accept(channel_T *channel)
{
    যদি (client_channel_গুনতি >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(রিক্ত, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	ঘূৰাই;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = রিক্ত;
    যদি (client_channels == রিক্ত)
    {
	channel->ch_ss_prev = রিক্ত;
	client_channels = channel;
    }
    অন্যথা
    {
	channel_T *last = client_channels;

	যেতিয়ালৈ (last->ch_ss_next != রিক্ত)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_গুনতি;

    // We will read the command from the client later in the input loop.
    ch_log(রিক্ত, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    স্থির শূন্য
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(রিক্ত, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    পূর্ণাঙ্ক
set_ref_in_socketserver_channel(পূর্ণাঙ্ক copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);

    FOR_ALL_CLIENTS(channel)
    {
	যদি (abort)
	    ভঙ্গ;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);
    }
    ঘূৰাই abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    স্থির পূর্ণাঙ্ক
socketserver_send_message(channel_T *ch, dict_T *বাৰ্তা, ৱর্ণ *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    পূর্ণাঙ্ক		ret;

    যদি (ver)
	dict_add_সংখ্যা(বাৰ্তা, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = বাৰ্তা;
    buf = json_encode(&tv, JSON_NL);

    যদি (buf != রিক্ত)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (পূর্ণাঙ্ক)STRLEN(buf), func);
	emsg_silent--;
    }
    অন্যথা
	ঘূৰাই FAIL;
    vim_free(buf);
    ঘূৰাই ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    স্থির শূন্য
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = রিক্ত;
    char_u	*sender = রিক্ত;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    যদি (di == রিক্ত || di->di_tv.v_type != VAR_STRING)
	ঘূৰাই;
    অন্যথা
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    যদি (di != রিক্ত && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    যদি (di != রিক্ত && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    যদি (di != রিক্ত && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_সংখ্যা)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((ৱর্ণ *)idbuf, পরিমাণ(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(রিক্ত, "socketserver_exec(): result: %s",
	    str == রিক্ত ? (char_u *)"(null)" : str);

    যদি (STRCMP(type, "expr") == 0 && str != রিক্ত)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	পূর্ণাঙ্ক	    code;

	dict = dict_alloc();
	যদি (dict == রিক্ত)
	    ঘূৰাই;

	result = eval_client_ৰাশিমালা_to_string(str);
	code = result == রিক্ত ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	যদি (result != রিক্ত)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	অন্যথা
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_সংখ্যা(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    অন্যথা যদি (STRCMP(type, "keystrokes") == 0 && str != রিক্ত)
    {
	// Execute keystrokes
	server_to_প্ৰবেশ_buf(str);
    }
    অন্যথা যদি (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	যদি (sender != রিক্ত && str != রিক্ত)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    যদি (reply != রিক্ত)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    অন্যথা যদি (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    অন্যথা
	ch_error(রিক্ত, "socketserver: unknown command type '%s'", type);
}

    স্থির পূর্ণাঙ্ক
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_বাৰ্তা = head->jq_next;
    পূর্ণাঙ্ক	    ver;

    যদি (json_বাৰ্তা == রিক্ত)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_বাৰ্তা = head->jq_next;
    }
    যদি (json_বাৰ্তা == রিক্ত)
	ঘূৰাই FAIL;
    *tv = json_বাৰ্তা->jq_মান;
    remove_json_গ্ৰন্থি(head, json_বাৰ্তা);

    যদি ((*tv)->v_type != VAR_DICT)
    {
	ch_error(রিক্ত, "socketserver: message is not a JSON object");
	free_tv(*tv);
	ঘূৰাই FAIL;
    }

    ver = dict_get_সংখ্যা((*tv)->vval.v_dict, "version");

    যদি (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(রিক্ত, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	যদি (dict != রিক্ত)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    শূন্য
socketserver_parse_messages(শূন্য)
{
    typval_T	*tv;

    বাবে (channel_T *ch = client_channels; ch != রিক্ত;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	যদি (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    অনুৱর্ত;
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
    স্থির পূর্ণাঙ্ক
socketserver_wait(channel_T *channel, পূর্ণাঙ্ক timeout)
{
    যেতিয়ালৈ (true)
    {
	পূর্ণাঙ্ক		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	সংরচনা timeval  tv;
	পূর্ণাঙ্ক		maxfd = -1;

	যদি (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	যদি (channel != রিক্ত)
	{
	    যদি (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ঘূৰাই FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	যদি (server_channel != রিক্ত && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    যদি (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    যদি (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (পূর্ণাঙ্ক)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		যদি (maxfd < (পূর্ণাঙ্ক)ch->CH_SOCK_FD)
		    maxfd = (পূর্ণাঙ্ক)ch->CH_SOCK_FD;
	    }
	}

	যদি (maxfd == -1)
	    ঘূৰাই FAIL;

	ret = select(maxfd + 1, &rfds, রিক্ত, রিক্ত, timeout == -1 ? রিক্ত : &tv);

#  ifdef EINTR
	যদি (ret == -1 && errno == EINTR)
	{
	    যদি (got_int)
		ভঙ্গ;
	    অনুৱর্ত;
	}
#  endif

	যদি (ret > 0)
	{
	    যদি (server_channel != রিক্ত
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		যদি (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (পূর্ণাঙ্ক)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    যদি (channel == রিক্ত)
		ঘূৰাই OK;

	    যদি (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		ঘূৰাই OK;
	    }
	    অনুৱর্ত;
	}
# অন্যথা
	সংরচনা pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	পূর্ণাঙ্ক		nfd = 0;
	পূর্ণাঙ্ক		channel_idx = -1;
	পূর্ণাঙ্ক		server_idx = -1;

	যদি (channel != রিক্ত)
	{
	    যদি (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ঘূৰাই FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	যদি (server_channel != রিক্ত && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    যদি (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	যদি (ret == -1 && errno == EINTR)
	{
	    যদি (got_int)
		ভঙ্গ;
	    অনুৱর্ত;
	}
#  endif

	যদি (ret > 0)
	{
	    যদি (server_channel != রিক্ত
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		যদি (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    যদি (channel == রিক্ত)
		ঘূৰাই OK;

	    যদি (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		ঘূৰাই OK;
	    }
	    অনুৱর্ত;
	}
# endif
	ভঙ্গ;
    }
    ঘূৰাই FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    স্থির channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = রিক্ত;
    পূর্ণাঙ্ক		port;
    bool	is_unix = false;
    channel_T	*channel;

    যদি (STRNICMP(name, "channel:", 8) == 0)
    {
	যদি (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    ঘূৰাই রিক্ত;
	যদি (address != রিক্ত)
	    is_unix = true;
    }
    অন্যথা যদি (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	পূর্ণাঙ্ক id;

	যদি (name[11] == NUL)
	    ঘূৰাই রিক্ত;
	id = strtol((ৱর্ণ *)name + 11, রিক্ত, 10);
	যদি (wait != রিক্ত)
	    *wait = true;

	ঘূৰাই channel_find(id);
    }
    অন্যথা
    {
	address = socketserver_get_path(name, false, quiet, false, রিক্ত);
	যদি (address == রিক্ত)
	    ঘূৰাই রিক্ত;
	is_unix = true;
    }

    যদি (is_unix)
	channel = channel_open_unix((ৱর্ণ *)address, রিক্ত);
    অন্যথা
	channel =  channel_open("localhost", port, 1000, রিক্ত);

    যদি (channel == রিক্ত && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    ঘূৰাই channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    পূর্ণাঙ্ক
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_ৰাশিমালা,
	পূর্ণাঙ্ক timeout,
	bool silent,
	channel_T **ch)
{
    পূর্ণাঙ্ক		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = রিক্ত;

    যদি (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	ঘূৰাই -1;
    }

    // Execute locally if target is ourselves
    যদি (serverName != রিক্ত && STRICMP(name, serverName) == 0)
	ঘূৰাই sendToLocalVim(str, is_ৰাশিমালা, result);

    channel = socketserver_get_channel(name, silent, রিক্ত);
    যদি (channel == রিক্ত)
	ঘূৰাই -1;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
	গচ্ছ exit;

    dict_add_string(dict, "type", (char_u *)(is_ৰাশিমালা ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    যদি (server_address != রিক্ত)
	dict_add_string(dict, "sender", server_address);
    অন্যথা যদি (ch != রিক্ত)
	dict_add_bool(dict, "wait", true);

    যদি (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	গচ্ছ exit;
    }
    dict_unref(dict);

    যদি (!is_ৰাশিমালা)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	যদি (ch != রিক্ত)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = রিক্ত;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    ঘূৰাই rcode;
	}
	গচ্ছ exit;
    }

    যদি (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    যেতিয়ালৈ (socketserver_wait(channel, timeout) == OK)
	যদি (socketserver_get_message(channel, &resp_tv) == OK)
	    ভঙ্গ;

    যদি (resp_tv == রিক্ত)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	গচ্ছ exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    যদি (di == রিক্ত || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(রিক্ত, "socketserver: unknown reply type");
	free_tv(resp_tv);
	গচ্ছ exit;
    }

    যদি (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	গচ্ছ exit;
    }

    যদি (result != রিক্ত)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	যদি (di != রিক্ত && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	অন্যথা
	{
	    free_tv(resp_tv);
	    গচ্ছ exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    যদি (di != রিক্ত && di->di_tv.v_type == VAR_সংখ্যা)
	rcode = di->di_tv.vval.v_সংখ্যা;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    ঘূৰাই rcode;
}

   স্থির ss_reply_T *
socketserver_get_reply(char_u *sender, পূর্ণাঙ্ক *index)
{
    বাবে (পূর্ণাঙ্ক i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	যদি (STRCMP(reply->sender, sender) == 0)
	{
	    যদি (index != রিক্ত)
		*index = i;
	    ঘূৰাই reply;
	}
    }
    ঘূৰাই রিক্ত;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    স্থির ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    যদি (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, পরিমাণ(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, রিক্ত);

    যদি (reply == রিক্ত && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	যদি (reply->sender == রিক্ত)
	    ঘূৰাই রিক্ত;

	ga_init2(&reply->strings, পরিমাণ(char_u *), 5);
    }

    ঘূৰাই reply;
}

    স্থির শূন্য
socketserver_remove_reply(char_u *sender)
{
    পূর্ণাঙ্ক index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    যদি (reply != রিক্ত)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	পূর্ণাঙ্ক	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	যদি (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    পরিমাণ(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    পূর্ণাঙ্ক
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    পূর্ণাঙ্ক		ret = OK;
    bool	wait = false;

    যদি (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ঘূৰাই FAIL;
    }

    যদি (server_channel == রিক্ত || server_address == রিক্ত)
    {
	emsg(_(e_socket_server_not_online));
	ঘূৰাই FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    যদি (channel == রিক্ত)
	ঘূৰাই FAIL;

    dict = dict_alloc();
    যদি (dict == রিক্ত)
    {
	ret = FAIL;
	গচ্ছ exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    যদি (server_address != রিক্ত)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    যদি (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    ঘূৰাই ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    পূর্ণাঙ্ক
socketserver_read_reply(
	char_u *client,
	char_u **str,
	পূর্ণাঙ্ক timeout,
	bool remotewait)
{
    ss_reply_T	*reply = রিক্ত;
    char_u	*actual;

    যদি (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ঘূৰাই FAIL;
    }

    যদি (!remotewait && (server_channel == রিক্ত || server_address == রিক্ত))
    {
	emsg(_(e_socket_server_not_online));
	ঘূৰাই FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, রিক্ত);
    যদি (actual == রিক্ত)
	ঘূৰাই FAIL;

    যেতিয়ালৈ (true)
    {
	reply = socketserver_get_reply(actual, রিক্ত);
	যদি (reply != রিক্ত)
	    ভঙ্গ;
	যদি (socketserver_wait(রিক্ত, timeout) == FAIL)
	    ভঙ্গ;
    }

    যদি (reply == রিক্ত || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	ঘূৰাই FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    যদি (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		পরিমাণ(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    যদি (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    ঘূৰাই OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    পূর্ণাঙ্ক
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    যদি (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	ঘূৰাই FAIL;
    }

    যদি (server_channel == রিক্ত || server_address == রিক্ত)
    {
	emsg(_(e_socket_server_not_online));
	ঘূৰাই FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, রিক্ত);
    যদি (actual == রিক্ত)
	ঘূৰাই FAIL;

    reply = socketserver_get_reply(actual, রিক্ত);
    vim_free(actual);

    যদি (reply != রিক্ত && reply->strings.ga_len > 0)
    {
	যদি (str != রিক্ত)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	ঘূৰাই 1;
    }
    ঘূৰাই 0;
}

#endif // FEAT_SOCKETSERVER
