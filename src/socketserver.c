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

#شامل "vim.h"

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
تعریفنوع ساختار
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

ایستا channel_T    *server_channel = پوچ;
ایستا char_u	    *server_address = پوچ; // Includes "type:" prefix if any
ایستا bool	    server_is_unix = false;
ایستا char_u	    *server_addr_cwd = پوچ; // CWD when server was started, used
					  // to handle relative file paths.
ایستا garray_T	    server_replies;

ایستا channel_T    *client_channels = پوچ;
ایستا صحیح	    client_channel_شمار = 0;

# define FOR_ALL_CLIENTS(ch) \
    برای (ch = client_channels; ch != پوچ; ch = ch->ch_ss_next)

ایستا تهی socketserver_cleanup(تهی);
ایستا char_u *socketserver_create_path(char_u *name, bool quiet);
ایستا char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
ایستا تهی socketserver_accept(channel_T *channel);
ایستا تهی socketserver_close(channel_T *channel);
ایستا ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    صحیح
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = پوچ;
    صحیح		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    اگر (server_channel != پوچ)
	بازگشت OK;

    اگر (STRNICMP(name, "channel:", 8) == 0)
    {
	اگر (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    بازگشت FAIL;
	اگر (address != پوچ)
	    is_unix = true;
    }
    وگرنه
    {
	address = socketserver_create_path(name, quiet);
	اگر (address == پوچ)
	    بازگشت FAIL;
	is_unix = true;
    }

    اگر (is_unix)
	channel = channel_listen_unix((نویسه *)address, پوچ, false);
    وگرنه
	channel = channel_listen(port, پوچ);

    اگر (channel == پوچ)
    {
	vim_free(address);
	بازگشت FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    اگر (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    وگرنه
    {
	buf = alloc(MAXPATHL + 1);
	اگر (buf != پوچ)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    اگر (buf != پوچ)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    اگر (mch_dirname(dirbuf, اندازه(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    وگرنه
	server_addr_cwd = پوچ;

    ga_init2(&server_replies, اندازه(ss_reply_T), 2);

    ch_log(پوچ, "socketserver: started server at %s", name);

    بازگشت OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    تهی
socketserver_stop(تهی)
{
    اگر (server_channel == پوچ)
	بازگشت;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(پوچ, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    ایستا تهی
socketserver_cleanup(تهی)
{
# ifdef UNIX
    اگر (server_is_unix && server_address != پوچ)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	اگر (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	وگرنه اگر (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	اگر (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	وگرنه اگر (server_addr_cwd != پوچ
		&& mch_dirname(dirbuf, اندازه(dirbuf)) == OK)
	{
	    اگر (mch_chdir((نویسه *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		اگر (mch_chdir((نویسه *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = پوچ;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    برای (صحیح i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(تهی)
{
    list_T *list = list_alloc();

    اگر (list == پوچ)
	بازگشت پوچ;

# ifdef MSWIN
    // Only support addresses on Windows
    بازگشت list;
# وگرنه
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    ساختار dirent   *dp;
    ثابت char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    اگر ((buf.string = alloc(MAXPATHL)) == پوچ)
	بازگشت پوچ;
    اگر ((path.string = alloc(MAXPATHL)) == پوچ)
    {
	vim_free(buf.string);
	بازگشت پوچ;
    }
    buf.length = 0;
    path.length = 0;

    برای (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ثابت char_u *dir = known_dirs[i];

	اگر (dir == پوچ)
	    ادامه;

	اگر (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != پوچ && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((نویسه *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long صحیح)getuid());
	وگرنه
	    path.length = vim_snprintf_safelen((نویسه *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((نویسه *)path.string);
	اگر (dirp == پوچ)
	    ادامه;

	// Loop through directory
	وقتی ((dp = readdir(dirp)) != پوچ)
	{
	    اگر (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		ادامه;

	    buf.length = vim_snprintf_safelen((نویسه *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	بشکن;
    }

    vim_free(path.string);
    vim_free(buf.string);

    بازگشت list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    ایستا char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	بازگشت vim_strsave(name + 8);
    وگرنه
    {
	اگر (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	بازگشت پوچ;
    }
# وگرنه
    char_u  *buf = پوچ;
    صحیح	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = پوچ;
    bool    fatal = false;

    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	بازگشت vim_strsave(name + 8);
    اگر (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    برای (صحیح i = 0; i < 1000; i++)
    {
	اگر (buf != پوچ)
	{
	    vim_snprintf((نویسه *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	وگرنه
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	اگر (fatal)
	    بشکن;

	اگر (path == پوچ)
	{
	    اگر (buf == پوچ)
	    {
		buf = alloc(buflen);
		اگر (buf == پوچ)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    بازگشت پوچ;
		}
	    }
	    ادامه;
	}
	بشکن;
    }
    vim_free(buf);

    بازگشت path;
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
    ایستا char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	بازگشت vim_strsave(name + 8);
    وگرنه
    {
	اگر (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	اگر (fatal != پوچ)
	    *fatal = true;
	بازگشت پوچ;
    }
# وگرنه
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ثابت char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    اگر (name == پوچ)
    {
	اگر (fatal != پوچ)
	    *fatal = true;
	بازگشت پوچ;
    }

    // Ignore if name is a path
    اگر (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	بازگشت vim_strsave(name);

    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	بازگشت vim_strsave(name + 8);

    اگر (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    اگر (vim_strchr(name, '/') != پوچ)
    {
	اگر (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	اگر (fatal != پوچ)
	    *fatal = true;
	بازگشت پوچ;
    }

    buf = alloc(MAXPATHL);

    اگر (buf == پوچ)
    {
	اگر (fatal != پوچ)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	بازگشت پوچ;
    }

    برای (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ثابت char_u	*dir = known_dirs[i];
	bool		got = false;

	اگر (dir == پوچ)
	    ادامه;
	وگرنه اگر (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != پوچ && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((نویسه *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long صحیح)getuid());
	    اگر (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ادامه;

	    vim_snprintf((نویسه *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long صحیح)getuid(), name);
	}
	وگرنه
	{
	    vim_snprintf((نویسه *)buf, MAXPATHL, "%s/vim", dir);
	    اگر (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ادامه;

	    vim_snprintf((نویسه *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	اگر (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((نویسه *)buf, پوچ);
	    emsg_off--;
	    emsg_silent--;

	    اگر (channel != پوچ)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    وگرنه
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	اگر (got || (!new && mch_access((نویسه *)buf, F_OK) == 0))
	{
	    اگر (server_address != پوچ && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		بشکن;
	    بازگشت buf;
	}
	بشکن;
    }

    اگر (!quiet)
    {
	اگر (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	وگرنه اگر (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    اگر (!res && fatal != پوچ)
	*fatal = true;

    vim_free(buf);
    بازگشت پوچ;
# endif
}

/*
 * Callback for when client channel is closed
 */
    ایستا تهی
socketserver_client_close(channel_T *channel)
{
    اگر (channel == client_channels)
	client_channels = channel->ch_ss_next;
    اگر (channel->ch_ss_prev != پوچ)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    اگر (channel->ch_ss_next != پوچ)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    اگر (client_channel_شمار > 0)
	--client_channel_شمار;

    ch_log(پوچ, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    ایستا تهی
socketserver_accept(channel_T *channel)
{
    اگر (client_channel_شمار >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(پوچ, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	بازگشت;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = پوچ;
    اگر (client_channels == پوچ)
    {
	channel->ch_ss_prev = پوچ;
	client_channels = channel;
    }
    وگرنه
    {
	channel_T *last = client_channels;

	وقتی (last->ch_ss_next != پوچ)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_شمار;

    // We will read the command from the client later in the input loop.
    ch_log(پوچ, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    ایستا تهی
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(پوچ, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    صحیح
set_ref_in_socketserver_channel(صحیح copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);

    FOR_ALL_CLIENTS(channel)
    {
	اگر (abort)
	    بشکن;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
    }
    بازگشت abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    ایستا صحیح
socketserver_send_message(channel_T *ch, dict_T *پیام, نویسه *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    صحیح		ret;

    اگر (ver)
	dict_add_عدد(پیام, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = پیام;
    buf = json_encode(&tv, JSON_NL);

    اگر (buf != پوچ)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (صحیح)STRLEN(buf), func);
	emsg_silent--;
    }
    وگرنه
	بازگشت FAIL;
    vim_free(buf);
    بازگشت ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    ایستا تهی
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = پوچ;
    char_u	*sender = پوچ;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    اگر (di == پوچ || di->di_tv.v_type != VAR_STRING)
	بازگشت;
    وگرنه
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    اگر (di != پوچ && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    اگر (di != پوچ && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    اگر (di != پوچ && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_عدد)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((نویسه *)idbuf, اندازه(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(پوچ, "socketserver_exec(): result: %s",
	    str == پوچ ? (char_u *)"(null)" : str);

    اگر (STRCMP(type, "expr") == 0 && str != پوچ)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	صحیح	    code;

	dict = dict_alloc();
	اگر (dict == پوچ)
	    بازگشت;

	result = eval_client_عبارت_to_string(str);
	code = result == پوچ ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	اگر (result != پوچ)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	وگرنه
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_عدد(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    وگرنه اگر (STRCMP(type, "keystrokes") == 0 && str != پوچ)
    {
	// Execute keystrokes
	server_to_درون_داد_buf(str);
    }
    وگرنه اگر (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	اگر (sender != پوچ && str != پوچ)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    اگر (reply != پوچ)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    وگرنه اگر (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    وگرنه
	ch_error(پوچ, "socketserver: unknown command type '%s'", type);
}

    ایستا صحیح
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_پیام = head->jq_next;
    صحیح	    ver;

    اگر (json_پیام == پوچ)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_پیام = head->jq_next;
    }
    اگر (json_پیام == پوچ)
	بازگشت FAIL;
    *tv = json_پیام->jq_مقدار;
    remove_json_گره(head, json_پیام);

    اگر ((*tv)->v_type != VAR_DICT)
    {
	ch_error(پوچ, "socketserver: message is not a JSON object");
	free_tv(*tv);
	بازگشت FAIL;
    }

    ver = dict_get_عدد((*tv)->vval.v_dict, "version");

    اگر (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(پوچ, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	اگر (dict != پوچ)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    تهی
socketserver_parse_messages(تهی)
{
    typval_T	*tv;

    برای (channel_T *ch = client_channels; ch != پوچ;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	اگر (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    ادامه;
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
    ایستا صحیح
socketserver_wait(channel_T *channel, صحیح timeout)
{
    وقتی (true)
    {
	صحیح		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	ساختار timeval  tv;
	صحیح		maxfd = -1;

	اگر (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	اگر (channel != پوچ)
	{
	    اگر (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		بازگشت FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	اگر (server_channel != پوچ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    اگر (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    اگر (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (صحیح)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		اگر (maxfd < (صحیح)ch->CH_SOCK_FD)
		    maxfd = (صحیح)ch->CH_SOCK_FD;
	    }
	}

	اگر (maxfd == -1)
	    بازگشت FAIL;

	ret = select(maxfd + 1, &rfds, پوچ, پوچ, timeout == -1 ? پوچ : &tv);

#  ifdef EINTR
	اگر (ret == -1 && errno == EINTR)
	{
	    اگر (got_int)
		بشکن;
	    ادامه;
	}
#  endif

	اگر (ret > 0)
	{
	    اگر (server_channel != پوچ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		اگر (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (صحیح)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    اگر (channel == پوچ)
		بازگشت OK;

	    اگر (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		بازگشت OK;
	    }
	    ادامه;
	}
# وگرنه
	ساختار pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	صحیح		nfd = 0;
	صحیح		channel_idx = -1;
	صحیح		server_idx = -1;

	اگر (channel != پوچ)
	{
	    اگر (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		بازگشت FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	اگر (server_channel != پوچ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    اگر (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	اگر (ret == -1 && errno == EINTR)
	{
	    اگر (got_int)
		بشکن;
	    ادامه;
	}
#  endif

	اگر (ret > 0)
	{
	    اگر (server_channel != پوچ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		اگر (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    اگر (channel == پوچ)
		بازگشت OK;

	    اگر (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		بازگشت OK;
	    }
	    ادامه;
	}
# endif
	بشکن;
    }
    بازگشت FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    ایستا channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = پوچ;
    صحیح		port;
    bool	is_unix = false;
    channel_T	*channel;

    اگر (STRNICMP(name, "channel:", 8) == 0)
    {
	اگر (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    بازگشت پوچ;
	اگر (address != پوچ)
	    is_unix = true;
    }
    وگرنه اگر (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	صحیح id;

	اگر (name[11] == NUL)
	    بازگشت پوچ;
	id = strtol((نویسه *)name + 11, پوچ, 10);
	اگر (wait != پوچ)
	    *wait = true;

	بازگشت channel_find(id);
    }
    وگرنه
    {
	address = socketserver_get_path(name, false, quiet, false, پوچ);
	اگر (address == پوچ)
	    بازگشت پوچ;
	is_unix = true;
    }

    اگر (is_unix)
	channel = channel_open_unix((نویسه *)address, پوچ);
    وگرنه
	channel =  channel_open("localhost", port, 1000, پوچ);

    اگر (channel == پوچ && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    بازگشت channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    صحیح
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_عبارت,
	صحیح timeout,
	bool silent,
	channel_T **ch)
{
    صحیح		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = پوچ;

    اگر (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	بازگشت -1;
    }

    // Execute locally if target is ourselves
    اگر (serverName != پوچ && STRICMP(name, serverName) == 0)
	بازگشت sendToLocalVim(str, is_عبارت, result);

    channel = socketserver_get_channel(name, silent, پوچ);
    اگر (channel == پوچ)
	بازگشت -1;

    dict = dict_alloc();
    اگر (dict == پوچ)
	برو exit;

    dict_add_string(dict, "type", (char_u *)(is_عبارت ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    اگر (server_address != پوچ)
	dict_add_string(dict, "sender", server_address);
    وگرنه اگر (ch != پوچ)
	dict_add_bool(dict, "wait", true);

    اگر (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	برو exit;
    }
    dict_unref(dict);

    اگر (!is_عبارت)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	اگر (ch != پوچ)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = پوچ;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    بازگشت rcode;
	}
	برو exit;
    }

    اگر (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    وقتی (socketserver_wait(channel, timeout) == OK)
	اگر (socketserver_get_message(channel, &resp_tv) == OK)
	    بشکن;

    اگر (resp_tv == پوچ)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	برو exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    اگر (di == پوچ || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(پوچ, "socketserver: unknown reply type");
	free_tv(resp_tv);
	برو exit;
    }

    اگر (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	برو exit;
    }

    اگر (result != پوچ)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	اگر (di != پوچ && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	وگرنه
	{
	    free_tv(resp_tv);
	    برو exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    اگر (di != پوچ && di->di_tv.v_type == VAR_عدد)
	rcode = di->di_tv.vval.v_عدد;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    بازگشت rcode;
}

   ایستا ss_reply_T *
socketserver_get_reply(char_u *sender, صحیح *index)
{
    برای (صحیح i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	اگر (STRCMP(reply->sender, sender) == 0)
	{
	    اگر (index != پوچ)
		*index = i;
	    بازگشت reply;
	}
    }
    بازگشت پوچ;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    ایستا ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    اگر (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, اندازه(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, پوچ);

    اگر (reply == پوچ && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	اگر (reply->sender == پوچ)
	    بازگشت پوچ;

	ga_init2(&reply->strings, اندازه(char_u *), 5);
    }

    بازگشت reply;
}

    ایستا تهی
socketserver_remove_reply(char_u *sender)
{
    صحیح index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    اگر (reply != پوچ)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	صحیح	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	اگر (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    اندازه(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    صحیح
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    صحیح		ret = OK;
    bool	wait = false;

    اگر (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	بازگشت FAIL;
    }

    اگر (server_channel == پوچ || server_address == پوچ)
    {
	emsg(_(e_socket_server_not_online));
	بازگشت FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    اگر (channel == پوچ)
	بازگشت FAIL;

    dict = dict_alloc();
    اگر (dict == پوچ)
    {
	ret = FAIL;
	برو exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    اگر (server_address != پوچ)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    اگر (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    بازگشت ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    صحیح
socketserver_read_reply(
	char_u *client,
	char_u **str,
	صحیح timeout,
	bool remotewait)
{
    ss_reply_T	*reply = پوچ;
    char_u	*actual;

    اگر (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	بازگشت FAIL;
    }

    اگر (!remotewait && (server_channel == پوچ || server_address == پوچ))
    {
	emsg(_(e_socket_server_not_online));
	بازگشت FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, پوچ);
    اگر (actual == پوچ)
	بازگشت FAIL;

    وقتی (true)
    {
	reply = socketserver_get_reply(actual, پوچ);
	اگر (reply != پوچ)
	    بشکن;
	اگر (socketserver_wait(پوچ, timeout) == FAIL)
	    بشکن;
    }

    اگر (reply == پوچ || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	بازگشت FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    اگر (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		اندازه(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    اگر (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    بازگشت OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    صحیح
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    اگر (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	بازگشت FAIL;
    }

    اگر (server_channel == پوچ || server_address == پوچ)
    {
	emsg(_(e_socket_server_not_online));
	بازگشت FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, پوچ);
    اگر (actual == پوچ)
	بازگشت FAIL;

    reply = socketserver_get_reply(actual, پوچ);
    vim_free(actual);

    اگر (reply != پوچ && reply->strings.ga_len > 0)
    {
	اگر (str != پوچ)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	بازگشت 1;
    }
    بازگشت 0;
}

#endif // FEAT_SOCKETSERVER
