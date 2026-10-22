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

#অন্তর্ভাব "vim.h"

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
প্রকারলেখ কাঠামো
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

স্থির channel_T    *server_channel = ফাঁকা;
স্থির char_u	    *server_address = ফাঁকা; // Includes "type:" prefix if any
স্থির bool	    server_is_unix = false;
স্থির char_u	    *server_addr_cwd = ফাঁকা; // CWD when server was started, used
					  // to handle relative file paths.
স্থির garray_T	    server_replies;

স্থির channel_T    *client_channels = ফাঁকা;
স্থির পূর্ণসংখ্যা	    client_channel_গুনতি = 0;

# define FOR_ALL_CLIENTS(ch) \
    জন্য (ch = client_channels; ch != ফাঁকা; ch = ch->ch_ss_পরবর্তী)

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
    পূর্ণসংখ্যা
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = ফাঁকা;
    পূর্ণসংখ্যা		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    যদি (server_channel != ফাঁকা)
	ফেরত OK;

    যদি (STRNICMP(name, "channel:", 8) == 0)
    {
	যদি (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    ফেরত FAIL;
	যদি (address != ফাঁকা)
	    is_unix = true;
    }
    অন্যথা
    {
	address = socketserver_create_path(name, quiet);
	যদি (address == ফাঁকা)
	    ফেরত FAIL;
	is_unix = true;
    }

    যদি (is_unix)
	channel = channel_listen_unix((অক্ষর *)address, ফাঁকা, false);
    অন্যথা
	channel = channel_listen(port, ফাঁকা);

    যদি (channel == ফাঁকা)
    {
	vim_free(address);
	ফেরত FAIL;
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
	যদি (buf != ফাঁকা)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    যদি (buf != ফাঁকা)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    যদি (mch_dirname(dirbuf, আকার(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    অন্যথা
	server_addr_cwd = ফাঁকা;

    ga_init2(&server_replies, আকার(ss_reply_T), 2);

    ch_log(ফাঁকা, "socketserver: started server at %s", name);

    ফেরত OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    শূন্য
socketserver_stop(শূন্য)
{
    যদি (server_channel == ফাঁকা)
	ফেরত;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ফাঁকা, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    স্থির শূন্য
socketserver_cleanup(শূন্য)
{
# ifdef UNIX
    যদি (server_is_unix && server_address != ফাঁকা)
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
	অন্যথা যদি (server_addr_cwd != ফাঁকা
		&& mch_dirname(dirbuf, আকার(dirbuf)) == OK)
	{
	    যদি (mch_chdir((অক্ষর *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		যদি (mch_chdir((অক্ষর *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = ফাঁকা;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    জন্য (পূর্ণসংখ্যা i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = (ss_reply_T *)server_replies.ga_তথ্য + i;

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

    যদি (list == ফাঁকা)
	ফেরত ফাঁকা;

# ifdef MSWIN
    // Only support addresses on Windows
    ফেরত list;
# অন্যথা
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    কাঠামো dirent   *dp;
    ধ্রুবক char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    যদি ((buf.string = alloc(MAXPATHL)) == ফাঁকা)
	ফেরত ফাঁকা;
    যদি ((path.string = alloc(MAXPATHL)) == ফাঁকা)
    {
	vim_free(buf.string);
	ফেরত ফাঁকা;
    }
    buf.দৈর্ঘ্য = 0;
    path.দৈর্ঘ্য = 0;

    জন্য (size_t i = 0 ; i < ARRAY_দৈর্ঘ্য(known_dirs); i++)
    {
	ধ্রুবক char_u *dir = known_dirs[i];

	যদি (dir == ফাঁকা)
	    চালু;

	যদি (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ফাঁকা && STRCMP(dir, known_dirs[1]) == 0))
	    path.দৈর্ঘ্য = vim_snprintf_safelen((অক্ষর *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long পূর্ণসংখ্যা)getuid());
	অন্যথা
	    path.দৈর্ঘ্য = vim_snprintf_safelen((অক্ষর *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((অক্ষর *)path.string);
	যদি (dirp == ফাঁকা)
	    চালু;

	// Loop through directory
	যখন ((dp = readdir(dirp)) != ফাঁকা)
	{
	    যদি (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		চালু;

	    buf.দৈর্ঘ্য = vim_snprintf_safelen((অক্ষর *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.দৈর্ঘ্য - (path.দৈর্ঘ্য + 1));
	}

	closedir(dirp);

	বিরতি;
    }

    vim_free(path.string);
    vim_free(buf.string);

    ফেরত list;
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
	ফেরত vim_strsave(name + 8);
    অন্যথা
    {
	যদি (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ফেরত ফাঁকা;
    }
# অন্যথা
    char_u  *buf = ফাঁকা;
    পূর্ণসংখ্যা	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = ফাঁকা;
    bool    fatal = false;

    যদি (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ফেরত vim_strsave(name + 8);
    যদি (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    জন্য (পূর্ণসংখ্যা i = 0; i < 1000; i++)
    {
	যদি (buf != ফাঁকা)
	{
	    vim_snprintf((অক্ষর *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	অন্যথা
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	যদি (fatal)
	    বিরতি;

	যদি (path == ফাঁকা)
	{
	    যদি (buf == ফাঁকা)
	    {
		buf = alloc(buflen);
		যদি (buf == ফাঁকা)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    ফেরত ফাঁকা;
		}
	    }
	    চালু;
	}
	বিরতি;
    }
    vim_free(buf);

    ফেরত path;
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
	ফেরত vim_strsave(name + 8);
    অন্যথা
    {
	যদি (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	যদি (fatal != ফাঁকা)
	    *fatal = true;
	ফেরত ফাঁকা;
    }
# অন্যথা
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ধ্রুবক char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    যদি (name == ফাঁকা)
    {
	যদি (fatal != ফাঁকা)
	    *fatal = true;
	ফেরত ফাঁকা;
    }

    // Ignore if name is a path
    যদি (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	ফেরত vim_strsave(name);

    যদি (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ফেরত vim_strsave(name + 8);

    যদি (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    যদি (vim_strchr(name, '/') != ফাঁকা)
    {
	যদি (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	যদি (fatal != ফাঁকা)
	    *fatal = true;
	ফেরত ফাঁকা;
    }

    buf = alloc(MAXPATHL);

    যদি (buf == ফাঁকা)
    {
	যদি (fatal != ফাঁকা)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	ফেরত ফাঁকা;
    }

    জন্য (size_t i = 0; i < ARRAY_দৈর্ঘ্য(known_dirs); i++)
    {
	ধ্রুবক char_u	*dir = known_dirs[i];
	bool		got = false;

	যদি (dir == ফাঁকা)
	    চালু;
	অন্যথা যদি (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ফাঁকা && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((অক্ষর *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long পূর্ণসংখ্যা)getuid());
	    যদি (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		চালু;

	    vim_snprintf((অক্ষর *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long পূর্ণসংখ্যা)getuid(), name);
	}
	অন্যথা
	{
	    vim_snprintf((অক্ষর *)buf, MAXPATHL, "%s/vim", dir);
	    যদি (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		চালু;

	    vim_snprintf((অক্ষর *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	যদি (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((অক্ষর *)buf, ফাঁকা);
	    emsg_off--;
	    emsg_silent--;

	    যদি (channel != ফাঁকা)
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
	যদি (got || (!new && mch_access((অক্ষর *)buf, F_OK) == 0))
	{
	    যদি (server_address != ফাঁকা && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		বিরতি;
	    ফেরত buf;
	}
	বিরতি;
    }

    যদি (!quiet)
    {
	যদি (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	অন্যথা যদি (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    যদি (!res && fatal != ফাঁকা)
	*fatal = true;

    vim_free(buf);
    ফেরত ফাঁকা;
# endif
}

/*
 * Callback for when client channel is closed
 */
    স্থির শূন্য
socketserver_client_close(channel_T *channel)
{
    যদি (channel == client_channels)
	client_channels = channel->ch_ss_পরবর্তী;
    যদি (channel->ch_ss_পূর্ব != ফাঁকা)
	channel->ch_ss_পূর্ব->ch_ss_পরবর্তী = channel->ch_ss_পরবর্তী;
    যদি (channel->ch_ss_পরবর্তী != ফাঁকা)
	channel->ch_ss_পরবর্তী->ch_ss_পূর্ব = channel->ch_ss_পূর্ব;
    যদি (client_channel_গুনতি > 0)
	--client_channel_গুনতি;

    ch_log(ফাঁকা, "socketserver: client channel closed");
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
	ch_log(ফাঁকা, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	ফেরত;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_পরবর্তী = ফাঁকা;
    যদি (client_channels == ফাঁকা)
    {
	channel->ch_ss_পূর্ব = ফাঁকা;
	client_channels = channel;
    }
    অন্যথা
    {
	channel_T *last = client_channels;

	যখন (last->ch_ss_পরবর্তী != ফাঁকা)
	    last = last->ch_ss_পরবর্তী;
	last->ch_ss_পরবর্তী = channel;
	channel->ch_ss_পূর্ব = last;
    }
    ++client_channel_গুনতি;

    // We will read the command from the client later in the input loop.
    ch_log(ফাঁকা, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    স্থির শূন্য
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(ফাঁকা, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    পূর্ণসংখ্যা
set_ref_in_socketserver_channel(পূর্ণসংখ্যা copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);

    FOR_ALL_CLIENTS(channel)
    {
	যদি (abort)
	    বিরতি;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);
    }
    ফেরত abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    স্থির পূর্ণসংখ্যা
socketserver_send_message(channel_T *ch, dict_T *বার্তা, অক্ষর *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    পূর্ণসংখ্যা		ret;

    যদি (ver)
	dict_add_সংখ্যা(বার্তা, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = বার্তা;
    buf = json_encode(&tv, JSON_NL);

    যদি (buf != ফাঁকা)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (পূর্ণসংখ্যা)STRLEN(buf), func);
	emsg_silent--;
    }
    অন্যথা
	ফেরত FAIL;
    vim_free(buf);
    ফেরত ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    স্থির শূন্য
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ফাঁকা;
    char_u	*sender = ফাঁকা;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    যদি (di == ফাঁকা || di->di_tv.v_type != VAR_STRING)
	ফেরত;
    অন্যথা
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    যদি (di != ফাঁকা && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    যদি (di != ফাঁকা && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    যদি (di != ফাঁকা && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_সংখ্যা)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((অক্ষর *)idbuf, আকার(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ফাঁকা, "socketserver_exec(): result: %s",
	    str == ফাঁকা ? (char_u *)"(null)" : str);

    যদি (STRCMP(type, "expr") == 0 && str != ফাঁকা)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *ফলাফল;
	পূর্ণসংখ্যা	    code;

	dict = dict_alloc();
	যদি (dict == ফাঁকা)
	    ফেরত;

	ফলাফল = eval_client_রাশিমালা_to_string(str);
	code = ফলাফল == ফাঁকা ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	যদি (ফলাফল != ফাঁকা)
	{
	    dict_add_string(dict, "str", ফলাফল);
	    vim_free(ফলাফল);
	}
	অন্যথা
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_সংখ্যা(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    অন্যথা যদি (STRCMP(type, "keystrokes") == 0 && str != ফাঁকা)
    {
	// Execute keystrokes
	server_to_প্রবেশ_buf(str);
    }
    অন্যথা যদি (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	যদি (sender != ফাঁকা && str != ফাঁকা)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    যদি (reply != ফাঁকা)
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
	ch_error(ফাঁকা, "socketserver: unknown command type '%s'", type);
}

    স্থির পূর্ণসংখ্যা
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *শীর্ষ = &channel->ch_part[PART_SOCK].ch_json_শীর্ষ;
    jsonq_T *json_বার্তা = শীর্ষ->jq_পরবর্তী;
    পূর্ণসংখ্যা	    ver;

    যদি (json_বার্তা == ফাঁকা)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_বার্তা = শীর্ষ->jq_পরবর্তী;
    }
    যদি (json_বার্তা == ফাঁকা)
	ফেরত FAIL;
    *tv = json_বার্তা->jq_মান;
    remove_json_গ্রন্থি(শীর্ষ, json_বার্তা);

    যদি ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ফাঁকা, "socketserver: message is not a JSON object");
	free_tv(*tv);
	ফেরত FAIL;
    }

    ver = dict_get_সংখ্যা((*tv)->vval.v_dict, "version");

    যদি (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ফাঁকা, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	যদি (dict != ফাঁকা)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	ফেরত FAIL;
    }

    ফেরত OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    শূন্য
socketserver_parse_messages(শূন্য)
{
    typval_T	*tv;

    জন্য (channel_T *ch = client_channels; ch != ফাঁকা;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *পরবর্তী = ch->ch_ss_পরবর্তী;

	// Get the JSON message if there is any from the queue for this channel.
	যদি (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = পরবর্তী;
	    চালু;
	}

	socketserver_exec(ch, tv->vval.v_dict);
	free_tv(tv);
	ch = পরবর্তী;
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
    স্থির পূর্ণসংখ্যা
socketserver_wait(channel_T *channel, পূর্ণসংখ্যা timeout)
{
    যখন (true)
    {
	পূর্ণসংখ্যা		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	কাঠামো timeval  tv;
	পূর্ণসংখ্যা		maxfd = -1;

	যদি (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	যদি (channel != ফাঁকা)
	{
	    যদি (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ফেরত FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	যদি (server_channel != ফাঁকা && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    যদি (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    যদি (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (পূর্ণসংখ্যা)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		যদি (maxfd < (পূর্ণসংখ্যা)ch->CH_SOCK_FD)
		    maxfd = (পূর্ণসংখ্যা)ch->CH_SOCK_FD;
	    }
	}

	যদি (maxfd == -1)
	    ফেরত FAIL;

	ret = select(maxfd + 1, &rfds, ফাঁকা, ফাঁকা, timeout == -1 ? ফাঁকা : &tv);

#  ifdef EINTR
	যদি (ret == -1 && errno == EINTR)
	{
	    যদি (got_int)
		বিরতি;
	    চালু;
	}
#  endif

	যদি (ret > 0)
	{
	    যদি (server_channel != ফাঁকা
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		যদি (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (পূর্ণসংখ্যা)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    যদি (channel == ফাঁকা)
		ফেরত OK;

	    যদি (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		ফেরত OK;
	    }
	    চালু;
	}
# অন্যথা
	কাঠামো pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	পূর্ণসংখ্যা		nfd = 0;
	পূর্ণসংখ্যা		channel_idx = -1;
	পূর্ণসংখ্যা		server_idx = -1;

	যদি (channel != ফাঁকা)
	{
	    যদি (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ফেরত FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	যদি (server_channel != ফাঁকা && server_channel->CH_SOCK_FD != INVALID_FD)
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
		বিরতি;
	    চালু;
	}
#  endif

	যদি (ret > 0)
	{
	    যদি (server_channel != ফাঁকা
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		যদি (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    যদি (channel == ফাঁকা)
		ফেরত OK;

	    যদি (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		ফেরত OK;
	    }
	    চালু;
	}
# endif
	বিরতি;
    }
    ফেরত FAIL;
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
    char_u	*address = ফাঁকা;
    পূর্ণসংখ্যা		port;
    bool	is_unix = false;
    channel_T	*channel;

    যদি (STRNICMP(name, "channel:", 8) == 0)
    {
	যদি (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    ফেরত ফাঁকা;
	যদি (address != ফাঁকা)
	    is_unix = true;
    }
    অন্যথা যদি (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	পূর্ণসংখ্যা id;

	যদি (name[11] == NUL)
	    ফেরত ফাঁকা;
	id = strtol((অক্ষর *)name + 11, ফাঁকা, 10);
	যদি (wait != ফাঁকা)
	    *wait = true;

	ফেরত channel_find(id);
    }
    অন্যথা
    {
	address = socketserver_get_path(name, false, quiet, false, ফাঁকা);
	যদি (address == ফাঁকা)
	    ফেরত ফাঁকা;
	is_unix = true;
    }

    যদি (is_unix)
	channel = channel_open_unix((অক্ষর *)address, ফাঁকা);
    অন্যথা
	channel =  channel_open("localhost", port, 1000, ফাঁকা);

    যদি (channel == ফাঁকা && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    ফেরত channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    পূর্ণসংখ্যা
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **ফলাফল,
	bool is_রাশিমালা,
	পূর্ণসংখ্যা timeout,
	bool silent,
	channel_T **ch)
{
    পূর্ণসংখ্যা		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ফাঁকা;

    যদি (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	ফেরত -1;
    }

    // Execute locally if target is ourselves
    যদি (serverName != ফাঁকা && STRICMP(name, serverName) == 0)
	ফেরত sendToLocalVim(str, is_রাশিমালা, ফলাফল);

    channel = socketserver_get_channel(name, silent, ফাঁকা);
    যদি (channel == ফাঁকা)
	ফেরত -1;

    dict = dict_alloc();
    যদি (dict == ফাঁকা)
	যাও exit;

    dict_add_string(dict, "type", (char_u *)(is_রাশিমালা ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    যদি (server_address != ফাঁকা)
	dict_add_string(dict, "sender", server_address);
    অন্যথা যদি (ch != ফাঁকা)
	dict_add_bool(dict, "wait", true);

    যদি (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	যাও exit;
    }
    dict_unref(dict);

    যদি (!is_রাশিমালা)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	যদি (ch != ফাঁকা)
	{
	    *ch = channel;

	    channel->ch_ss_পরবর্তী = client_channels;
	    channel->ch_ss_পূর্ব = ফাঁকা;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    ফেরত rcode;
	}
	যাও exit;
    }

    যদি (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    যখন (socketserver_wait(channel, timeout) == OK)
	যদি (socketserver_get_message(channel, &resp_tv) == OK)
	    বিরতি;

    যদি (resp_tv == ফাঁকা)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	যাও exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    যদি (di == ফাঁকা || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ফাঁকা, "socketserver: unknown reply type");
	free_tv(resp_tv);
	যাও exit;
    }

    যদি (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	যাও exit;
    }

    যদি (ফলাফল != ফাঁকা)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	যদি (di != ফাঁকা && di->di_tv.v_type == VAR_STRING)
	    *ফলাফল = vim_strsave(di->di_tv.vval.v_string);
	অন্যথা
	{
	    free_tv(resp_tv);
	    যাও exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    যদি (di != ফাঁকা && di->di_tv.v_type == VAR_সংখ্যা)
	rcode = di->di_tv.vval.v_সংখ্যা;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    ফেরত rcode;
}

   স্থির ss_reply_T *
socketserver_get_reply(char_u *sender, পূর্ণসংখ্যা *সূচক)
{
    জন্য (পূর্ণসংখ্যা i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_তথ্য) + i;

	যদি (STRCMP(reply->sender, sender) == 0)
	{
	    যদি (সূচক != ফাঁকা)
		*সূচক = i;
	    ফেরত reply;
	}
    }
    ফেরত ফাঁকা;
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
	ga_init2(&server_replies, আকার(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, ফাঁকা);

    যদি (reply == ফাঁকা && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_তথ্য) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	যদি (reply->sender == ফাঁকা)
	    ফেরত ফাঁকা;

	ga_init2(&reply->strings, আকার(char_u *), 5);
    }

    ফেরত reply;
}

    স্থির শূন্য
socketserver_remove_reply(char_u *sender)
{
    পূর্ণসংখ্যা সূচক;
    ss_reply_T *reply = socketserver_get_reply(sender, &সূচক);

    যদি (reply != ফাঁকা)
    {
	ss_reply_T  *arr = server_replies.ga_তথ্য;
	পূর্ণসংখ্যা	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	যদি (len > 1)
	    mch_memmove(arr + সূচক, arr + সূচক + 1,
		    আকার(ss_reply_T) * (len - সূচক - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    পূর্ণসংখ্যা
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    পূর্ণসংখ্যা		ret = OK;
    bool	wait = false;

    যদি (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ফেরত FAIL;
    }

    যদি (server_channel == ফাঁকা || server_address == ফাঁকা)
    {
	emsg(_(e_socket_server_not_online));
	ফেরত FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    যদি (channel == ফাঁকা)
	ফেরত FAIL;

    dict = dict_alloc();
    যদি (dict == ফাঁকা)
    {
	ret = FAIL;
	যাও exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    যদি (server_address != ফাঁকা)
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

    ফেরত ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    পূর্ণসংখ্যা
socketserver_read_reply(
	char_u *client,
	char_u **str,
	পূর্ণসংখ্যা timeout,
	bool remotewait)
{
    ss_reply_T	*reply = ফাঁকা;
    char_u	*actual;

    যদি (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ফেরত FAIL;
    }

    যদি (!remotewait && (server_channel == ফাঁকা || server_address == ফাঁকা))
    {
	emsg(_(e_socket_server_not_online));
	ফেরত FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, ফাঁকা);
    যদি (actual == ফাঁকা)
	ফেরত FAIL;

    যখন (true)
    {
	reply = socketserver_get_reply(actual, ফাঁকা);
	যদি (reply != ফাঁকা)
	    বিরতি;
	যদি (socketserver_wait(ফাঁকা, timeout) == FAIL)
	    বিরতি;
    }

    যদি (reply == ফাঁকা || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	ফেরত FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_তথ্য)[0];

    যদি (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_তথ্য,
		((char_u **)reply->strings.ga_তথ্য) + 1,
		আকার(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    যদি (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    ফেরত OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    পূর্ণসংখ্যা
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    যদি (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	ফেরত FAIL;
    }

    যদি (server_channel == ফাঁকা || server_address == ফাঁকা)
    {
	emsg(_(e_socket_server_not_online));
	ফেরত FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, ফাঁকা);
    যদি (actual == ফাঁকা)
	ফেরত FAIL;

    reply = socketserver_get_reply(actual, ফাঁকা);
    vim_free(actual);

    যদি (reply != ফাঁকা && reply->strings.ga_len > 0)
    {
	যদি (str != ফাঁকা)
	    *str = ((char_u **)reply->strings.ga_তথ্য)[0];
	ফেরত 1;
    }
    ফেরত 0;
}

#endif // FEAT_SOCKETSERVER
