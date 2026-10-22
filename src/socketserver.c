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

#અન્તર્ભાવ "vim.h"

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
પ્રકારવ્યાખ્યા સંરચના
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

સ્થિર channel_T    *server_channel = ખાલી;
સ્થિર char_u	    *server_address = ખાલી; // Includes "type:" prefix if any
સ્થિર bool	    server_is_unix = false;
સ્થિર char_u	    *server_addr_cwd = ખાલી; // CWD when server was started, used
					  // to handle relative file paths.
સ્થિર garray_T	    server_replies;

સ્થિર channel_T    *client_channels = ખાલી;
સ્થિર પૂર્ણાંક	    client_channel_ગણતરી = 0;

# define FOR_ALL_CLIENTS(ch) \
    માટે (ch = client_channels; ch != ખાલી; ch = ch->ch_ss_next)

સ્થિર શૂન્ય socketserver_cleanup(શૂન્ય);
સ્થિર char_u *socketserver_create_path(char_u *name, bool quiet);
સ્થિર char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
સ્થિર શૂન્ય socketserver_accept(channel_T *channel);
સ્થિર શૂન્ય socketserver_close(channel_T *channel);
સ્થિર ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    પૂર્ણાંક
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = ખાલી;
    પૂર્ણાંક		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    જો (server_channel != ખાલી)
	પાછા OK;

    જો (STRNICMP(name, "channel:", 8) == 0)
    {
	જો (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    પાછા FAIL;
	જો (address != ખાલી)
	    is_unix = true;
    }
    નહીંતર
    {
	address = socketserver_create_path(name, quiet);
	જો (address == ખાલી)
	    પાછા FAIL;
	is_unix = true;
    }

    જો (is_unix)
	channel = channel_listen_unix((અક્ષર *)address, ખાલી, false);
    નહીંતર
	channel = channel_listen(port, ખાલી);

    જો (channel == ખાલી)
    {
	vim_free(address);
	પાછા FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    જો (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    નહીંતર
    {
	buf = alloc(MAXPATHL + 1);
	જો (buf != ખાલી)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    જો (buf != ખાલી)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    જો (mch_dirname(dirbuf, કદ(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    નહીંતર
	server_addr_cwd = ખાલી;

    ga_init2(&server_replies, કદ(ss_reply_T), 2);

    ch_log(ખાલી, "socketserver: started server at %s", name);

    પાછા OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    શૂન્ય
socketserver_stop(શૂન્ય)
{
    જો (server_channel == ખાલી)
	પાછા;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ખાલી, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    સ્થિર શૂન્ય
socketserver_cleanup(શૂન્ય)
{
# ifdef UNIX
    જો (server_is_unix && server_address != ખાલી)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	જો (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	નહીંતર જો (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	જો (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	નહીંતર જો (server_addr_cwd != ખાલી
		&& mch_dirname(dirbuf, કદ(dirbuf)) == OK)
	{
	    જો (mch_chdir((અક્ષર *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		જો (mch_chdir((અક્ષર *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = ખાલી;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    માટે (પૂર્ણાંક i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(શૂન્ય)
{
    list_T *list = list_alloc();

    જો (list == ખાલી)
	પાછા ખાલી;

# ifdef MSWIN
    // Only support addresses on Windows
    પાછા list;
# નહીંતર
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    સંરચના dirent   *dp;
    અચલ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    જો ((buf.string = alloc(MAXPATHL)) == ખાલી)
	પાછા ખાલી;
    જો ((path.string = alloc(MAXPATHL)) == ખાલી)
    {
	vim_free(buf.string);
	પાછા ખાલી;
    }
    buf.length = 0;
    path.length = 0;

    માટે (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	અચલ char_u *dir = known_dirs[i];

	જો (dir == ખાલી)
	    ચાલુ;

	જો (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ખાલી && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((અક્ષર *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long પૂર્ણાંક)getuid());
	નહીંતર
	    path.length = vim_snprintf_safelen((અક્ષર *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((અક્ષર *)path.string);
	જો (dirp == ખાલી)
	    ચાલુ;

	// Loop through directory
	જ્યારે ((dp = readdir(dirp)) != ખાલી)
	{
	    જો (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		ચાલુ;

	    buf.length = vim_snprintf_safelen((અક્ષર *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	તોડો;
    }

    vim_free(path.string);
    vim_free(buf.string);

    પાછા list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    સ્થિર char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    જો (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	પાછા vim_strsave(name + 8);
    નહીંતર
    {
	જો (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	પાછા ખાલી;
    }
# નહીંતર
    char_u  *buf = ખાલી;
    પૂર્ણાંક	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = ખાલી;
    bool    fatal = false;

    જો (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	પાછા vim_strsave(name + 8);
    જો (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    માટે (પૂર્ણાંક i = 0; i < 1000; i++)
    {
	જો (buf != ખાલી)
	{
	    vim_snprintf((અક્ષર *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	નહીંતર
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	જો (fatal)
	    તોડો;

	જો (path == ખાલી)
	{
	    જો (buf == ખાલી)
	    {
		buf = alloc(buflen);
		જો (buf == ખાલી)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    પાછા ખાલી;
		}
	    }
	    ચાલુ;
	}
	તોડો;
    }
    vim_free(buf);

    પાછા path;
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
    સ્થિર char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    જો (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	પાછા vim_strsave(name + 8);
    નહીંતર
    {
	જો (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	જો (fatal != ખાલી)
	    *fatal = true;
	પાછા ખાલી;
    }
# નહીંતર
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    અચલ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    જો (name == ખાલી)
    {
	જો (fatal != ખાલી)
	    *fatal = true;
	પાછા ખાલી;
    }

    // Ignore if name is a path
    જો (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	પાછા vim_strsave(name);

    જો (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	પાછા vim_strsave(name + 8);

    જો (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    જો (vim_strchr(name, '/') != ખાલી)
    {
	જો (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	જો (fatal != ખાલી)
	    *fatal = true;
	પાછા ખાલી;
    }

    buf = alloc(MAXPATHL);

    જો (buf == ખાલી)
    {
	જો (fatal != ખાલી)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	પાછા ખાલી;
    }

    માટે (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	અચલ char_u	*dir = known_dirs[i];
	bool		got = false;

	જો (dir == ખાલી)
	    ચાલુ;
	નહીંતર જો (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ખાલી && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((અક્ષર *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long પૂર્ણાંક)getuid());
	    જો (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ચાલુ;

	    vim_snprintf((અક્ષર *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long પૂર્ણાંક)getuid(), name);
	}
	નહીંતર
	{
	    vim_snprintf((અક્ષર *)buf, MAXPATHL, "%s/vim", dir);
	    જો (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ચાલુ;

	    vim_snprintf((અક્ષર *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	જો (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((અક્ષર *)buf, ખાલી);
	    emsg_off--;
	    emsg_silent--;

	    જો (channel != ખાલી)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    નહીંતર
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	જો (got || (!new && mch_access((અક્ષર *)buf, F_OK) == 0))
	{
	    જો (server_address != ખાલી && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		તોડો;
	    પાછા buf;
	}
	તોડો;
    }

    જો (!quiet)
    {
	જો (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	નહીંતર જો (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    જો (!res && fatal != ખાલી)
	*fatal = true;

    vim_free(buf);
    પાછા ખાલી;
# endif
}

/*
 * Callback for when client channel is closed
 */
    સ્થિર શૂન્ય
socketserver_client_close(channel_T *channel)
{
    જો (channel == client_channels)
	client_channels = channel->ch_ss_next;
    જો (channel->ch_ss_prev != ખાલી)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    જો (channel->ch_ss_next != ખાલી)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    જો (client_channel_ગણતરી > 0)
	--client_channel_ગણતરી;

    ch_log(ખાલી, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    સ્થિર શૂન્ય
socketserver_accept(channel_T *channel)
{
    જો (client_channel_ગણતરી >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(ખાલી, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	પાછા;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = ખાલી;
    જો (client_channels == ખાલી)
    {
	channel->ch_ss_prev = ખાલી;
	client_channels = channel;
    }
    નહીંતર
    {
	channel_T *last = client_channels;

	જ્યારે (last->ch_ss_next != ખાલી)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_ગણતરી;

    // We will read the command from the client later in the input loop.
    ch_log(ખાલી, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    સ્થિર શૂન્ય
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(ખાલી, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    પૂર્ણાંક
set_ref_in_socketserver_channel(પૂર્ણાંક copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);

    FOR_ALL_CLIENTS(channel)
    {
	જો (abort)
	    તોડો;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
    }
    પાછા abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    સ્થિર પૂર્ણાંક
socketserver_send_message(channel_T *ch, dict_T *સંદેશ, અક્ષર *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    પૂર્ણાંક		ret;

    જો (ver)
	dict_add_સંખ્યા(સંદેશ, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = સંદેશ;
    buf = json_encode(&tv, JSON_NL);

    જો (buf != ખાલી)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (પૂર્ણાંક)STRLEN(buf), func);
	emsg_silent--;
    }
    નહીંતર
	પાછા FAIL;
    vim_free(buf);
    પાછા ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    સ્થિર શૂન્ય
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ખાલી;
    char_u	*sender = ખાલી;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    જો (di == ખાલી || di->di_tv.v_type != VAR_STRING)
	પાછા;
    નહીંતર
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    જો (di != ખાલી && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    જો (di != ખાલી && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    જો (di != ખાલી && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_સંખ્યા)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((અક્ષર *)idbuf, કદ(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ખાલી, "socketserver_exec(): result: %s",
	    str == ખાલી ? (char_u *)"(null)" : str);

    જો (STRCMP(type, "expr") == 0 && str != ખાલી)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	પૂર્ણાંક	    code;

	dict = dict_alloc();
	જો (dict == ખાલી)
	    પાછા;

	result = eval_client_પદાવલિ_to_string(str);
	code = result == ખાલી ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	જો (result != ખાલી)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	નહીંતર
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_સંખ્યા(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    નહીંતર જો (STRCMP(type, "keystrokes") == 0 && str != ખાલી)
    {
	// Execute keystrokes
	server_to_આગત_buf(str);
    }
    નહીંતર જો (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	જો (sender != ખાલી && str != ખાલી)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    જો (reply != ખાલી)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    નહીંતર જો (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    નહીંતર
	ch_error(ખાલી, "socketserver: unknown command type '%s'", type);
}

    સ્થિર પૂર્ણાંક
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_સંદેશ = head->jq_next;
    પૂર્ણાંક	    ver;

    જો (json_સંદેશ == ખાલી)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_સંદેશ = head->jq_next;
    }
    જો (json_સંદેશ == ખાલી)
	પાછા FAIL;
    *tv = json_સંદેશ->jq_કિંમત;
    remove_json_ગાંઠ(head, json_સંદેશ);

    જો ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ખાલી, "socketserver: message is not a JSON object");
	free_tv(*tv);
	પાછા FAIL;
    }

    ver = dict_get_સંખ્યા((*tv)->vval.v_dict, "version");

    જો (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ખાલી, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	જો (dict != ખાલી)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    શૂન્ય
socketserver_parse_messages(શૂન્ય)
{
    typval_T	*tv;

    માટે (channel_T *ch = client_channels; ch != ખાલી;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	જો (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    ચાલુ;
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
    સ્થિર પૂર્ણાંક
socketserver_wait(channel_T *channel, પૂર્ણાંક timeout)
{
    જ્યારે (true)
    {
	પૂર્ણાંક		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	સંરચના timeval  tv;
	પૂર્ણાંક		maxfd = -1;

	જો (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	જો (channel != ખાલી)
	{
	    જો (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		પાછા FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	જો (server_channel != ખાલી && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    જો (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    જો (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (પૂર્ણાંક)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		જો (maxfd < (પૂર્ણાંક)ch->CH_SOCK_FD)
		    maxfd = (પૂર્ણાંક)ch->CH_SOCK_FD;
	    }
	}

	જો (maxfd == -1)
	    પાછા FAIL;

	ret = select(maxfd + 1, &rfds, ખાલી, ખાલી, timeout == -1 ? ખાલી : &tv);

#  ifdef EINTR
	જો (ret == -1 && errno == EINTR)
	{
	    જો (got_int)
		તોડો;
	    ચાલુ;
	}
#  endif

	જો (ret > 0)
	{
	    જો (server_channel != ખાલી
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		જો (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (પૂર્ણાંક)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    જો (channel == ખાલી)
		પાછા OK;

	    જો (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		પાછા OK;
	    }
	    ચાલુ;
	}
# નહીંતર
	સંરચના pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	પૂર્ણાંક		nfd = 0;
	પૂર્ણાંક		channel_idx = -1;
	પૂર્ણાંક		server_idx = -1;

	જો (channel != ખાલી)
	{
	    જો (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		પાછા FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	જો (server_channel != ખાલી && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    જો (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	જો (ret == -1 && errno == EINTR)
	{
	    જો (got_int)
		તોડો;
	    ચાલુ;
	}
#  endif

	જો (ret > 0)
	{
	    જો (server_channel != ખાલી
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		જો (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    જો (channel == ખાલી)
		પાછા OK;

	    જો (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		પાછા OK;
	    }
	    ચાલુ;
	}
# endif
	તોડો;
    }
    પાછા FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    સ્થિર channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = ખાલી;
    પૂર્ણાંક		port;
    bool	is_unix = false;
    channel_T	*channel;

    જો (STRNICMP(name, "channel:", 8) == 0)
    {
	જો (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    પાછા ખાલી;
	જો (address != ખાલી)
	    is_unix = true;
    }
    નહીંતર જો (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	પૂર્ણાંક id;

	જો (name[11] == NUL)
	    પાછા ખાલી;
	id = strtol((અક્ષર *)name + 11, ખાલી, 10);
	જો (wait != ખાલી)
	    *wait = true;

	પાછા channel_find(id);
    }
    નહીંતર
    {
	address = socketserver_get_path(name, false, quiet, false, ખાલી);
	જો (address == ખાલી)
	    પાછા ખાલી;
	is_unix = true;
    }

    જો (is_unix)
	channel = channel_open_unix((અક્ષર *)address, ખાલી);
    નહીંતર
	channel =  channel_open("localhost", port, 1000, ખાલી);

    જો (channel == ખાલી && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    પાછા channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    પૂર્ણાંક
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_પદાવલિ,
	પૂર્ણાંક timeout,
	bool silent,
	channel_T **ch)
{
    પૂર્ણાંક		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ખાલી;

    જો (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	પાછા -1;
    }

    // Execute locally if target is ourselves
    જો (serverName != ખાલી && STRICMP(name, serverName) == 0)
	પાછા sendToLocalVim(str, is_પદાવલિ, result);

    channel = socketserver_get_channel(name, silent, ખાલી);
    જો (channel == ખાલી)
	પાછા -1;

    dict = dict_alloc();
    જો (dict == ખાલી)
	જાઓ exit;

    dict_add_string(dict, "type", (char_u *)(is_પદાવલિ ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    જો (server_address != ખાલી)
	dict_add_string(dict, "sender", server_address);
    નહીંતર જો (ch != ખાલી)
	dict_add_bool(dict, "wait", true);

    જો (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	જાઓ exit;
    }
    dict_unref(dict);

    જો (!is_પદાવલિ)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	જો (ch != ખાલી)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = ખાલી;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    પાછા rcode;
	}
	જાઓ exit;
    }

    જો (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    જ્યારે (socketserver_wait(channel, timeout) == OK)
	જો (socketserver_get_message(channel, &resp_tv) == OK)
	    તોડો;

    જો (resp_tv == ખાલી)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	જાઓ exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    જો (di == ખાલી || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ખાલી, "socketserver: unknown reply type");
	free_tv(resp_tv);
	જાઓ exit;
    }

    જો (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	જાઓ exit;
    }

    જો (result != ખાલી)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	જો (di != ખાલી && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	નહીંતર
	{
	    free_tv(resp_tv);
	    જાઓ exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    જો (di != ખાલી && di->di_tv.v_type == VAR_સંખ્યા)
	rcode = di->di_tv.vval.v_સંખ્યા;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    પાછા rcode;
}

   સ્થિર ss_reply_T *
socketserver_get_reply(char_u *sender, પૂર્ણાંક *index)
{
    માટે (પૂર્ણાંક i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	જો (STRCMP(reply->sender, sender) == 0)
	{
	    જો (index != ખાલી)
		*index = i;
	    પાછા reply;
	}
    }
    પાછા ખાલી;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    સ્થિર ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    જો (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, કદ(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, ખાલી);

    જો (reply == ખાલી && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	જો (reply->sender == ખાલી)
	    પાછા ખાલી;

	ga_init2(&reply->strings, કદ(char_u *), 5);
    }

    પાછા reply;
}

    સ્થિર શૂન્ય
socketserver_remove_reply(char_u *sender)
{
    પૂર્ણાંક index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    જો (reply != ખાલી)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	પૂર્ણાંક	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	જો (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    કદ(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    પૂર્ણાંક
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    પૂર્ણાંક		ret = OK;
    bool	wait = false;

    જો (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	પાછા FAIL;
    }

    જો (server_channel == ખાલી || server_address == ખાલી)
    {
	emsg(_(e_socket_server_not_online));
	પાછા FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    જો (channel == ખાલી)
	પાછા FAIL;

    dict = dict_alloc();
    જો (dict == ખાલી)
    {
	ret = FAIL;
	જાઓ exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    જો (server_address != ખાલી)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    જો (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    પાછા ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    પૂર્ણાંક
socketserver_read_reply(
	char_u *client,
	char_u **str,
	પૂર્ણાંક timeout,
	bool remotewait)
{
    ss_reply_T	*reply = ખાલી;
    char_u	*actual;

    જો (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	પાછા FAIL;
    }

    જો (!remotewait && (server_channel == ખાલી || server_address == ખાલી))
    {
	emsg(_(e_socket_server_not_online));
	પાછા FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, ખાલી);
    જો (actual == ખાલી)
	પાછા FAIL;

    જ્યારે (true)
    {
	reply = socketserver_get_reply(actual, ખાલી);
	જો (reply != ખાલી)
	    તોડો;
	જો (socketserver_wait(ખાલી, timeout) == FAIL)
	    તોડો;
    }

    જો (reply == ખાલી || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	પાછા FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    જો (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		કદ(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    જો (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    પાછા OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    પૂર્ણાંક
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    જો (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	પાછા FAIL;
    }

    જો (server_channel == ખાલી || server_address == ખાલી)
    {
	emsg(_(e_socket_server_not_online));
	પાછા FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, ખાલી);
    જો (actual == ખાલી)
	પાછા FAIL;

    reply = socketserver_get_reply(actual, ખાલી);
    vim_free(actual);

    જો (reply != ખાલી && reply->strings.ga_len > 0)
    {
	જો (str != ખાલી)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	પાછા 1;
    }
    પાછા 0;
}

#endif // FEAT_SOCKETSERVER
