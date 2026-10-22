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

#അന്തര്ഭാവ "vim.h"

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
തരനിർവചനം ഘടന
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

സ്ഥിതം channel_T    *server_channel = ശൂന്യ;
സ്ഥിതം char_u	    *server_address = ശൂന്യ; // Includes "type:" prefix if any
സ്ഥിതം bool	    server_is_unix = false;
സ്ഥിതം char_u	    *server_addr_cwd = ശൂന്യ; // CWD when server was started, used
					  // to handle relative file paths.
സ്ഥിതം garray_T	    server_replies;

സ്ഥിതം channel_T    *client_channels = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	    client_channel_എണ്ണം = 0;

# define FOR_ALL_CLIENTS(ch) \
    വേണ്ടി (ch = client_channels; ch != ശൂന്യ; ch = ch->ch_ss_next)

സ്ഥിതം ശൂന്യം socketserver_cleanup(ശൂന്യം);
സ്ഥിതം char_u *socketserver_create_path(char_u *name, bool quiet);
സ്ഥിതം char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
സ്ഥിതം ശൂന്യം socketserver_accept(channel_T *channel);
സ്ഥിതം ശൂന്യം socketserver_close(channel_T *channel);
സ്ഥിതം ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    പൂർണ്ണസംഖ്യ
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    എങ്കിൽ (server_channel != ശൂന്യ)
	തിരികെ OK;

    എങ്കിൽ (STRNICMP(name, "channel:", 8) == 0)
    {
	എങ്കിൽ (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (address != ശൂന്യ)
	    is_unix = true;
    }
    അല്ലെങ്കിൽ
    {
	address = socketserver_create_path(name, quiet);
	എങ്കിൽ (address == ശൂന്യ)
	    തിരികെ FAIL;
	is_unix = true;
    }

    എങ്കിൽ (is_unix)
	channel = channel_listen_unix((അക്ഷരം *)address, ശൂന്യ, false);
    അല്ലെങ്കിൽ
	channel = channel_listen(port, ശൂന്യ);

    എങ്കിൽ (channel == ശൂന്യ)
    {
	vim_free(address);
	തിരികെ FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    എങ്കിൽ (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    അല്ലെങ്കിൽ
    {
	buf = alloc(MAXPATHL + 1);
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    എങ്കിൽ (buf != ശൂന്യ)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    എങ്കിൽ (mch_dirname(dirbuf, വലുപ്പം(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    അല്ലെങ്കിൽ
	server_addr_cwd = ശൂന്യ;

    ga_init2(&server_replies, വലുപ്പം(ss_reply_T), 2);

    ch_log(ശൂന്യ, "socketserver: started server at %s", name);

    തിരികെ OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    ശൂന്യം
socketserver_stop(ശൂന്യം)
{
    എങ്കിൽ (server_channel == ശൂന്യ)
	തിരികെ;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ശൂന്യ, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    സ്ഥിതം ശൂന്യം
socketserver_cleanup(ശൂന്യം)
{
# ifdef UNIX
    എങ്കിൽ (server_is_unix && server_address != ശൂന്യ)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	എങ്കിൽ (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	എങ്കിൽ (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	അല്ലെങ്കിൽ എങ്കിൽ (server_addr_cwd != ശൂന്യ
		&& mch_dirname(dirbuf, വലുപ്പം(dirbuf)) == OK)
	{
	    എങ്കിൽ (mch_chdir((അക്ഷരം *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		എങ്കിൽ (mch_chdir((അക്ഷരം *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = ശൂന്യ;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(ശൂന്യം)
{
    list_T *list = list_alloc();

    എങ്കിൽ (list == ശൂന്യ)
	തിരികെ ശൂന്യ;

# ifdef MSWIN
    // Only support addresses on Windows
    തിരികെ list;
# അല്ലെങ്കിൽ
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    ഘടന dirent   *dp;
    സ്ഥിരം char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    എങ്കിൽ ((buf.string = alloc(MAXPATHL)) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ ((path.string = alloc(MAXPATHL)) == ശൂന്യ)
    {
	vim_free(buf.string);
	തിരികെ ശൂന്യ;
    }
    buf.length = 0;
    path.length = 0;

    വേണ്ടി (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	സ്ഥിരം char_u *dir = known_dirs[i];

	എങ്കിൽ (dir == ശൂന്യ)
	    തുടരുക;

	എങ്കിൽ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ശൂന്യ && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((അക്ഷരം *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long പൂർണ്ണസംഖ്യ)getuid());
	അല്ലെങ്കിൽ
	    path.length = vim_snprintf_safelen((അക്ഷരം *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((അക്ഷരം *)path.string);
	എങ്കിൽ (dirp == ശൂന്യ)
	    തുടരുക;

	// Loop through directory
	ആകുമ്പോൾ ((dp = readdir(dirp)) != ശൂന്യ)
	{
	    എങ്കിൽ (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		തുടരുക;

	    buf.length = vim_snprintf_safelen((അക്ഷരം *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	നിർത്തുക;
    }

    vim_free(path.string);
    vim_free(buf.string);

    തിരികെ list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    സ്ഥിതം char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    എങ്കിൽ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	തിരികെ vim_strsave(name + 8);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	തിരികെ ശൂന്യ;
    }
# അല്ലെങ്കിൽ
    char_u  *buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = ശൂന്യ;
    bool    fatal = false;

    എങ്കിൽ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	തിരികെ vim_strsave(name + 8);
    എങ്കിൽ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < 1000; i++)
    {
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    vim_snprintf((അക്ഷരം *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	അല്ലെങ്കിൽ
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	എങ്കിൽ (fatal)
	    നിർത്തുക;

	എങ്കിൽ (path == ശൂന്യ)
	{
	    എങ്കിൽ (buf == ശൂന്യ)
	    {
		buf = alloc(buflen);
		എങ്കിൽ (buf == ശൂന്യ)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    തിരികെ ശൂന്യ;
		}
	    }
	    തുടരുക;
	}
	നിർത്തുക;
    }
    vim_free(buf);

    തിരികെ path;
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
    സ്ഥിതം char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    എങ്കിൽ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	തിരികെ vim_strsave(name + 8);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	എങ്കിൽ (fatal != ശൂന്യ)
	    *fatal = true;
	തിരികെ ശൂന്യ;
    }
# അല്ലെങ്കിൽ
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    സ്ഥിരം char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    എങ്കിൽ (name == ശൂന്യ)
    {
	എങ്കിൽ (fatal != ശൂന്യ)
	    *fatal = true;
	തിരികെ ശൂന്യ;
    }

    // Ignore if name is a path
    എങ്കിൽ (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	തിരികെ vim_strsave(name);

    എങ്കിൽ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	തിരികെ vim_strsave(name + 8);

    എങ്കിൽ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    എങ്കിൽ (vim_strchr(name, '/') != ശൂന്യ)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	എങ്കിൽ (fatal != ശൂന്യ)
	    *fatal = true;
	തിരികെ ശൂന്യ;
    }

    buf = alloc(MAXPATHL);

    എങ്കിൽ (buf == ശൂന്യ)
    {
	എങ്കിൽ (fatal != ശൂന്യ)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	തിരികെ ശൂന്യ;
    }

    വേണ്ടി (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	സ്ഥിരം char_u	*dir = known_dirs[i];
	bool		got = false;

	എങ്കിൽ (dir == ശൂന്യ)
	    തുടരുക;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ശൂന്യ && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((അക്ഷരം *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long പൂർണ്ണസംഖ്യ)getuid());
	    എങ്കിൽ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		തുടരുക;

	    vim_snprintf((അക്ഷരം *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long പൂർണ്ണസംഖ്യ)getuid(), name);
	}
	അല്ലെങ്കിൽ
	{
	    vim_snprintf((അക്ഷരം *)buf, MAXPATHL, "%s/vim", dir);
	    എങ്കിൽ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		തുടരുക;

	    vim_snprintf((അക്ഷരം *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	എങ്കിൽ (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((അക്ഷരം *)buf, ശൂന്യ);
	    emsg_off--;
	    emsg_silent--;

	    എങ്കിൽ (channel != ശൂന്യ)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    അല്ലെങ്കിൽ
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	എങ്കിൽ (got || (!new && mch_access((അക്ഷരം *)buf, F_OK) == 0))
	{
	    എങ്കിൽ (server_address != ശൂന്യ && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		നിർത്തുക;
	    തിരികെ buf;
	}
	നിർത്തുക;
    }

    എങ്കിൽ (!quiet)
    {
	എങ്കിൽ (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	അല്ലെങ്കിൽ എങ്കിൽ (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    എങ്കിൽ (!res && fatal != ശൂന്യ)
	*fatal = true;

    vim_free(buf);
    തിരികെ ശൂന്യ;
# endif
}

/*
 * Callback for when client channel is closed
 */
    സ്ഥിതം ശൂന്യം
socketserver_client_close(channel_T *channel)
{
    എങ്കിൽ (channel == client_channels)
	client_channels = channel->ch_ss_next;
    എങ്കിൽ (channel->ch_ss_prev != ശൂന്യ)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    എങ്കിൽ (channel->ch_ss_next != ശൂന്യ)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    എങ്കിൽ (client_channel_എണ്ണം > 0)
	--client_channel_എണ്ണം;

    ch_log(ശൂന്യ, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    സ്ഥിതം ശൂന്യം
socketserver_accept(channel_T *channel)
{
    എങ്കിൽ (client_channel_എണ്ണം >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(ശൂന്യ, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	തിരികെ;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = ശൂന്യ;
    എങ്കിൽ (client_channels == ശൂന്യ)
    {
	channel->ch_ss_prev = ശൂന്യ;
	client_channels = channel;
    }
    അല്ലെങ്കിൽ
    {
	channel_T *last = client_channels;

	ആകുമ്പോൾ (last->ch_ss_next != ശൂന്യ)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_എണ്ണം;

    // We will read the command from the client later in the input loop.
    ch_log(ശൂന്യ, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    സ്ഥിതം ശൂന്യം
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(ശൂന്യ, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_socketserver_channel(പൂർണ്ണസംഖ്യ copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);

    FOR_ALL_CLIENTS(channel)
    {
	എങ്കിൽ (abort)
	    നിർത്തുക;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    }
    തിരികെ abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
socketserver_send_message(channel_T *ch, dict_T *സന്ദേശം, അക്ഷരം *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (ver)
	dict_add_സംഖ്യ(സന്ദേശം, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = സന്ദേശം;
    buf = json_encode(&tv, JSON_NL);

    എങ്കിൽ (buf != ശൂന്യ)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (പൂർണ്ണസംഖ്യ)STRLEN(buf), func);
	emsg_silent--;
    }
    അല്ലെങ്കിൽ
	തിരികെ FAIL;
    vim_free(buf);
    തിരികെ ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    സ്ഥിതം ശൂന്യം
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ശൂന്യ;
    char_u	*sender = ശൂന്യ;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    എങ്കിൽ (di == ശൂന്യ || di->di_tv.v_type != VAR_STRING)
	തിരികെ;
    അല്ലെങ്കിൽ
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_സംഖ്യ)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((അക്ഷരം *)idbuf, വലുപ്പം(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ശൂന്യ, "socketserver_exec(): result: %s",
	    str == ശൂന്യ ? (char_u *)"(null)" : str);

    എങ്കിൽ (STRCMP(type, "expr") == 0 && str != ശൂന്യ)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	പൂർണ്ണസംഖ്യ	    code;

	dict = dict_alloc();
	എങ്കിൽ (dict == ശൂന്യ)
	    തിരികെ;

	result = eval_client_വ്യഞ്ജകം_to_string(str);
	code = result == ശൂന്യ ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	എങ്കിൽ (result != ശൂന്യ)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	അല്ലെങ്കിൽ
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_സംഖ്യ(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(type, "keystrokes") == 0 && str != ശൂന്യ)
    {
	// Execute keystrokes
	server_to_നിവേശം_buf(str);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	എങ്കിൽ (sender != ശൂന്യ && str != ശൂന്യ)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    എങ്കിൽ (reply != ശൂന്യ)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    അല്ലെങ്കിൽ
	ch_error(ശൂന്യ, "socketserver: unknown command type '%s'", type);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_സന്ദേശം = head->jq_next;
    പൂർണ്ണസംഖ്യ	    ver;

    എങ്കിൽ (json_സന്ദേശം == ശൂന്യ)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_സന്ദേശം = head->jq_next;
    }
    എങ്കിൽ (json_സന്ദേശം == ശൂന്യ)
	തിരികെ FAIL;
    *tv = json_സന്ദേശം->jq_മൂല്യം;
    remove_json_കണ്ണി(head, json_സന്ദേശം);

    എങ്കിൽ ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ശൂന്യ, "socketserver: message is not a JSON object");
	free_tv(*tv);
	തിരികെ FAIL;
    }

    ver = dict_get_സംഖ്യ((*tv)->vval.v_dict, "version");

    എങ്കിൽ (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ശൂന്യ, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	എങ്കിൽ (dict != ശൂന്യ)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    ശൂന്യം
socketserver_parse_messages(ശൂന്യം)
{
    typval_T	*tv;

    വേണ്ടി (channel_T *ch = client_channels; ch != ശൂന്യ;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	എങ്കിൽ (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    തുടരുക;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
socketserver_wait(channel_T *channel, പൂർണ്ണസംഖ്യ timeout)
{
    ആകുമ്പോൾ (true)
    {
	പൂർണ്ണസംഖ്യ		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	ഘടന timeval  tv;
	പൂർണ്ണസംഖ്യ		maxfd = -1;

	എങ്കിൽ (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	എങ്കിൽ (channel != ശൂന്യ)
	{
	    എങ്കിൽ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		തിരികെ FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	എങ്കിൽ (server_channel != ശൂന്യ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    എങ്കിൽ (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    എങ്കിൽ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (പൂർണ്ണസംഖ്യ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		എങ്കിൽ (maxfd < (പൂർണ്ണസംഖ്യ)ch->CH_SOCK_FD)
		    maxfd = (പൂർണ്ണസംഖ്യ)ch->CH_SOCK_FD;
	    }
	}

	എങ്കിൽ (maxfd == -1)
	    തിരികെ FAIL;

	ret = select(maxfd + 1, &rfds, ശൂന്യ, ശൂന്യ, timeout == -1 ? ശൂന്യ : &tv);

#  ifdef EINTR
	എങ്കിൽ (ret == -1 && errno == EINTR)
	{
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	    തുടരുക;
	}
#  endif

	എങ്കിൽ (ret > 0)
	{
	    എങ്കിൽ (server_channel != ശൂന്യ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		എങ്കിൽ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (പൂർണ്ണസംഖ്യ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    എങ്കിൽ (channel == ശൂന്യ)
		തിരികെ OK;

	    എങ്കിൽ (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		തിരികെ OK;
	    }
	    തുടരുക;
	}
# അല്ലെങ്കിൽ
	ഘടന pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	പൂർണ്ണസംഖ്യ		nfd = 0;
	പൂർണ്ണസംഖ്യ		channel_idx = -1;
	പൂർണ്ണസംഖ്യ		server_idx = -1;

	എങ്കിൽ (channel != ശൂന്യ)
	{
	    എങ്കിൽ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		തിരികെ FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	എങ്കിൽ (server_channel != ശൂന്യ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    എങ്കിൽ (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	എങ്കിൽ (ret == -1 && errno == EINTR)
	{
	    എങ്കിൽ (got_int)
		നിർത്തുക;
	    തുടരുക;
	}
#  endif

	എങ്കിൽ (ret > 0)
	{
	    എങ്കിൽ (server_channel != ശൂന്യ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		എങ്കിൽ (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    എങ്കിൽ (channel == ശൂന്യ)
		തിരികെ OK;

	    എങ്കിൽ (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		തിരികെ OK;
	    }
	    തുടരുക;
	}
# endif
	നിർത്തുക;
    }
    തിരികെ FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    സ്ഥിതം channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		port;
    bool	is_unix = false;
    channel_T	*channel;

    എങ്കിൽ (STRNICMP(name, "channel:", 8) == 0)
    {
	എങ്കിൽ (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (address != ശൂന്യ)
	    is_unix = true;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	പൂർണ്ണസംഖ്യ id;

	എങ്കിൽ (name[11] == NUL)
	    തിരികെ ശൂന്യ;
	id = strtol((അക്ഷരം *)name + 11, ശൂന്യ, 10);
	എങ്കിൽ (wait != ശൂന്യ)
	    *wait = true;

	തിരികെ channel_find(id);
    }
    അല്ലെങ്കിൽ
    {
	address = socketserver_get_path(name, false, quiet, false, ശൂന്യ);
	എങ്കിൽ (address == ശൂന്യ)
	    തിരികെ ശൂന്യ;
	is_unix = true;
    }

    എങ്കിൽ (is_unix)
	channel = channel_open_unix((അക്ഷരം *)address, ശൂന്യ);
    അല്ലെങ്കിൽ
	channel =  channel_open("localhost", port, 1000, ശൂന്യ);

    എങ്കിൽ (channel == ശൂന്യ && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    തിരികെ channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    പൂർണ്ണസംഖ്യ
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_വ്യഞ്ജകം,
	പൂർണ്ണസംഖ്യ timeout,
	bool silent,
	channel_T **ch)
{
    പൂർണ്ണസംഖ്യ		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ശൂന്യ;

    എങ്കിൽ (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	തിരികെ -1;
    }

    // Execute locally if target is ourselves
    എങ്കിൽ (serverName != ശൂന്യ && STRICMP(name, serverName) == 0)
	തിരികെ sendToLocalVim(str, is_വ്യഞ്ജകം, result);

    channel = socketserver_get_channel(name, silent, ശൂന്യ);
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ -1;

    dict = dict_alloc();
    എങ്കിൽ (dict == ശൂന്യ)
	പോകുക exit;

    dict_add_string(dict, "type", (char_u *)(is_വ്യഞ്ജകം ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    എങ്കിൽ (server_address != ശൂന്യ)
	dict_add_string(dict, "sender", server_address);
    അല്ലെങ്കിൽ എങ്കിൽ (ch != ശൂന്യ)
	dict_add_bool(dict, "wait", true);

    എങ്കിൽ (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	പോകുക exit;
    }
    dict_unref(dict);

    എങ്കിൽ (!is_വ്യഞ്ജകം)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	എങ്കിൽ (ch != ശൂന്യ)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = ശൂന്യ;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    തിരികെ rcode;
	}
	പോകുക exit;
    }

    എങ്കിൽ (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    ആകുമ്പോൾ (socketserver_wait(channel, timeout) == OK)
	എങ്കിൽ (socketserver_get_message(channel, &resp_tv) == OK)
	    നിർത്തുക;

    എങ്കിൽ (resp_tv == ശൂന്യ)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	പോകുക exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    എങ്കിൽ (di == ശൂന്യ || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ശൂന്യ, "socketserver: unknown reply type");
	free_tv(resp_tv);
	പോകുക exit;
    }

    എങ്കിൽ (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	പോകുക exit;
    }

    എങ്കിൽ (result != ശൂന്യ)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	അല്ലെങ്കിൽ
	{
	    free_tv(resp_tv);
	    പോകുക exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_സംഖ്യ)
	rcode = di->di_tv.vval.v_സംഖ്യ;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    തിരികെ rcode;
}

   സ്ഥിതം ss_reply_T *
socketserver_get_reply(char_u *sender, പൂർണ്ണസംഖ്യ *index)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	എങ്കിൽ (STRCMP(reply->sender, sender) == 0)
	{
	    എങ്കിൽ (index != ശൂന്യ)
		*index = i;
	    തിരികെ reply;
	}
    }
    തിരികെ ശൂന്യ;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    സ്ഥിതം ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    എങ്കിൽ (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, വലുപ്പം(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, ശൂന്യ);

    എങ്കിൽ (reply == ശൂന്യ && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	എങ്കിൽ (reply->sender == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	ga_init2(&reply->strings, വലുപ്പം(char_u *), 5);
    }

    തിരികെ reply;
}

    സ്ഥിതം ശൂന്യം
socketserver_remove_reply(char_u *sender)
{
    പൂർണ്ണസംഖ്യ index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    എങ്കിൽ (reply != ശൂന്യ)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	പൂർണ്ണസംഖ്യ	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	എങ്കിൽ (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    വലുപ്പം(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    പൂർണ്ണസംഖ്യ
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    പൂർണ്ണസംഖ്യ		ret = OK;
    bool	wait = false;

    എങ്കിൽ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	തിരികെ FAIL;
    }

    എങ്കിൽ (server_channel == ശൂന്യ || server_address == ശൂന്യ)
    {
	emsg(_(e_socket_server_not_online));
	തിരികെ FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ FAIL;

    dict = dict_alloc();
    എങ്കിൽ (dict == ശൂന്യ)
    {
	ret = FAIL;
	പോകുക exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    എങ്കിൽ (server_address != ശൂന്യ)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    എങ്കിൽ (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    തിരികെ ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    പൂർണ്ണസംഖ്യ
socketserver_read_reply(
	char_u *client,
	char_u **str,
	പൂർണ്ണസംഖ്യ timeout,
	bool remotewait)
{
    ss_reply_T	*reply = ശൂന്യ;
    char_u	*actual;

    എങ്കിൽ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	തിരികെ FAIL;
    }

    എങ്കിൽ (!remotewait && (server_channel == ശൂന്യ || server_address == ശൂന്യ))
    {
	emsg(_(e_socket_server_not_online));
	തിരികെ FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, ശൂന്യ);
    എങ്കിൽ (actual == ശൂന്യ)
	തിരികെ FAIL;

    ആകുമ്പോൾ (true)
    {
	reply = socketserver_get_reply(actual, ശൂന്യ);
	എങ്കിൽ (reply != ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ (socketserver_wait(ശൂന്യ, timeout) == FAIL)
	    നിർത്തുക;
    }

    എങ്കിൽ (reply == ശൂന്യ || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	തിരികെ FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    എങ്കിൽ (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		വലുപ്പം(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    എങ്കിൽ (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    തിരികെ OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    പൂർണ്ണസംഖ്യ
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    എങ്കിൽ (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	തിരികെ FAIL;
    }

    എങ്കിൽ (server_channel == ശൂന്യ || server_address == ശൂന്യ)
    {
	emsg(_(e_socket_server_not_online));
	തിരികെ FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, ശൂന്യ);
    എങ്കിൽ (actual == ശൂന്യ)
	തിരികെ FAIL;

    reply = socketserver_get_reply(actual, ശൂന്യ);
    vim_free(actual);

    എങ്കിൽ (reply != ശൂന്യ && reply->strings.ga_len > 0)
    {
	എങ്കിൽ (str != ശൂന്യ)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	തിരികെ 1;
    }
    തിരികെ 0;
}

#endif // FEAT_SOCKETSERVER
