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

#ضمن "vim.h"

#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER

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
تعريفنوع بنية
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T لڙيون;
} ss_جواب_T;

ساكن channel_T    *server_channel = عدم;
ساكن char_u	    *server_پتو = عدم; // Includes "type:" prefix if any
ساكن bool	    server_is_unix = false;
ساكن char_u	    *server_addr_cwd = عدم; // CWD when server was started, used
					  // to handle relative file paths.
ساكن garray_T	    server_replies;

ساكن channel_T    *client_channels = عدم;
ساكن صحيح	    client_channel_ڳڻپ = 0;

# define FOR_ALL_CLIENTS(ch) \
    لكل (ch = client_channels; ch != عدم; ch = ch->ch_ss_ايندڙ)

ساكن فراغ socketserver_cleanup(فراغ);
ساكن char_u *socketserver_create_رستو(char_u *name, bool quiet);
ساكن char_u *socketserver_get_رستو(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
ساكن فراغ socketserver_accept(channel_T *channel);
ساكن فراغ socketserver_close(channel_T *channel);
ساكن ss_جواب_T *socketserver_جوڙيو_جواب(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    صحيح
socketserver_شروعات(char_u *name, bool quiet)
{
    char_u	*پتو = عدم;
    صحيح		پورٽ;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    إذا (server_channel != عدم)
	أرجع OK;

    إذا (STRNICMP(name, "channel:", 8) == 0)
    {
	إذا (channel_parse_socketserver_پتو(name + 8, &پورٽ, &پتو, quiet)
		== FAIL)
	    أرجع FAIL;
	إذا (پتو != عدم)
	    is_unix = true;
    }
    وإلا
    {
	پتو = socketserver_create_رستو(name, quiet);
	إذا (پتو == عدم)
	    أرجع FAIL;
	is_unix = true;
    }

    إذا (is_unix)
	channel = channel_listen_unix((حرف *)پتو, عدم, false);
    وإلا
	channel = channel_listen(پورٽ, عدم);

    إذا (channel == عدم)
    {
	vim_free(پتو);
	أرجع FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    إذا (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    وإلا
    {
	buf = alloc(MAXPATHL + 1);
	إذا (buf != عدم)
	{
	    buf[0] = NUL;
	    mch_ڀريل_Name(پتو, buf, MAXPATHL, false);
	}
    }
    vim_free(پتو);

    إذا (buf != عدم)
    {
	server_پتو = vim_strsave(buf);
	serverName = buf;
	سيٽ_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    إذا (mch_dirname(dirbuf, حجم(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    وإلا
	server_addr_cwd = عدم;

    ga_init2(&server_replies, حجم(ss_جواب_T), 2);

    ch_log(عدم, "socketserver: started server at %s", name);

    أرجع OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    فراغ
socketserver_stop(فراغ)
{
    إذا (server_channel == عدم)
	أرجع;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(عدم, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    ساكن فراغ
socketserver_cleanup(فراغ)
{
# ifdef UNIX
    إذا (server_is_unix && server_پتو != عدم)
    {
	char_u *رستو = server_پتو;
	char_u dirbuf[MAXPATHL];

	إذا (STRNICMP(رستو, "channel:unix:", 13) == 0)
	    رستو += 13;
	وإلا إذا (STRNICMP(رستو, "name:", 5) == 0)
	    رستو += 5;

	إذا (*رستو == '/')
	    mch_remove(رستو);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	وإلا إذا (server_addr_cwd != عدم
		&& mch_dirname(dirbuf, حجم(dirbuf)) == OK)
	{
	    إذا (mch_chdir((حرف *)server_addr_cwd) == 0)
	    {
		mch_remove(رستو);
		إذا (mch_chdir((حرف *)dirbuf) < 0)
		    emsg(_(e_cannot_go_پٺ_منزل_اڳيون_directory));
	    }
	}
    }
# endif

    server_channel = عدم;
    VIM_CLEAR(server_پتو);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    لكل (صحيح i = 0; i < server_replies.ga_len; i++)
    {
	ss_جواب_T *جواب = (ss_جواب_T *)server_replies.ga_معلومات + i;

	vim_free(جواب->sender);
	ga_clear_لڙيون(&جواب->لڙيون);
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
socketserver_list(فراغ)
{
    list_T *list = list_alloc();

    إذا (list == عدم)
	أرجع عدم;

# ifdef MSWIN
    // Only support addresses on Windows
    أرجع list;
# وإلا
    string_T	    buf;
    string_T	    رستو;
    DIR		    *dirp;
    بنية dirent   *dp;
    ثابت char_u    *معلوم_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    إذا ((buf.string = alloc(MAXPATHL)) == عدم)
	أرجع عدم;
    إذا ((رستو.string = alloc(MAXPATHL)) == عدم)
    {
	vim_free(buf.string);
	أرجع عدم;
    }
    buf.ڊيگهائي = 0;
    رستو.ڊيگهائي = 0;

    لكل (size_t i = 0 ; i < ARRAY_ڊيگهائي(معلوم_dirs); i++)
    {
	ثابت char_u *dir = معلوم_dirs[i];

	إذا (dir == عدم)
	    استمر;

	إذا (STRCMP(dir, "/tmp") == 0 ||
		(معلوم_dirs[1] != عدم && STRCMP(dir, معلوم_dirs[1]) == 0))
	    رستو.ڊيگهائي = vim_snprintf_safelen((حرف *)رستو.string, MAXPATHL,
		    "%s/vim-%lu", dir, (بنانشان ڊگهو صحيح)getuid());
	وإلا
	    رستو.ڊيگهائي = vim_snprintf_safelen((حرف *)رستو.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((حرف *)رستو.string);
	إذا (dirp == عدم)
	    استمر;

	// Loop through directory
	طالما ((dp = readdir(dirp)) != عدم)
	{
	    إذا (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		استمر;

	    buf.ڊيگهائي = vim_snprintf_safelen((حرف *)buf.string, MAXPATHL,
		    "%s/%s", رستو.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.ڊيگهائي - (رستو.ڊيگهائي + 1));
	}

	closedir(dirp);

	اكسر;
    }

    vim_free(رستو.string);
    vim_free(buf.string);

    أرجع list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    ساكن char_u *
socketserver_create_رستو(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    إذا (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	أرجع vim_strsave(name + 8);
    وإلا
    {
	إذا (!quiet)
	    semsg(_(e_invalid_دليل_str), name);
	أرجع عدم;
    }
# وإلا
    char_u  *buf = عدم;
    صحيح	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *رستو = عدم;
    bool    fatal = false;

    إذا (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	أرجع vim_strsave(name + 8);
    إذا (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    لكل (صحيح i = 0; i < 1000; i++)
    {
	إذا (buf != عدم)
	{
	    vim_snprintf((حرف *)buf, buflen, "%s%d", name, i);
	    رستو = socketserver_get_رستو(buf, true, quiet, true, &fatal);
	}
	وإلا
	    رستو = socketserver_get_رستو(name, true, quiet, true, &fatal);

	إذا (fatal)
	    اكسر;

	إذا (رستو == عدم)
	{
	    إذا (buf == عدم)
	    {
		buf = alloc(buflen);
		إذا (buf == عدم)
		{
		    semsg(_(e_ٻاهريون_جو_يادگيري_allocating_nr_بائيٽون), buflen);
		    أرجع عدم;
		}
	    }
	    استمر;
	}
	اكسر;
    }
    vim_free(buf);

    أرجع رستو;
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
    ساكن char_u *
socketserver_get_رستو(
	char_u *name,
	bool new اڻ_استعمال_ٿيل,
	bool quiet,
	bool ignore اڻ_استعمال_ٿيل,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    إذا (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	أرجع vim_strsave(name + 8);
    وإلا
    {
	إذا (!quiet)
	    semsg(_(e_invalid_دليل_str), name);
	إذا (fatal != عدم)
	    *fatal = true;
	أرجع عدم;
    }
# وإلا
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ثابت char_u    *معلوم_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    إذا (name == عدم)
    {
	إذا (fatal != عدم)
	    *fatal = true;
	أرجع عدم;
    }

    // Ignore if name is a path
    إذا (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	أرجع vim_strsave(name);

    إذا (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	أرجع vim_strsave(name + 8);

    إذا (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    إذا (vim_strchr(name, '/') != عدم)
    {
	إذا (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	إذا (fatal != عدم)
	    *fatal = true;
	أرجع عدم;
    }

    buf = alloc(MAXPATHL);

    إذا (buf == عدم)
    {
	إذا (fatal != عدم)
	    *fatal = true;
	semsg(_(e_ٻاهريون_جو_يادگيري_allocating_nr_بائيٽون), MAXPATHL);
	أرجع عدم;
    }

    لكل (size_t i = 0; i < ARRAY_ڊيگهائي(معلوم_dirs); i++)
    {
	ثابت char_u	*dir = معلوم_dirs[i];
	bool		got = false;

	إذا (dir == عدم)
	    استمر;
	وإلا إذا (STRCMP(dir, "/tmp") == 0 ||
		(معلوم_dirs[1] != عدم && STRCMP(dir, معلوم_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (بنانشان ڊگهو صحيح)getuid());
	    إذا (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		استمر;

	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (بنانشان ڊگهو صحيح)getuid(), name);
	}
	وإلا
	{
	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim", dir);
	    إذا (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		استمر;

	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	إذا (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((حرف *)buf, عدم);
	    emsg_off--;
	    emsg_silent--;

	    إذا (channel != عدم)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    وإلا
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	إذا (got || (!new && mch_access((حرف *)buf, F_OK) == 0))
	{
	    إذا (server_پتو != عدم && STRCMP(buf, server_پتو) == 0)
		// Can't connect to itself
		اكسر;
	    أرجع buf;
	}
	اكسر;
    }

    إذا (!quiet)
    {
	إذا (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	وإلا إذا (!ignore)
	    semsg(_(e_invalid_server_id_استعمال_ٿيل_str), name);
    }
    إذا (!res && fatal != عدم)
	*fatal = true;

    vim_free(buf);
    أرجع عدم;
# endif
}

/*
 * Callback for when client channel is closed
 */
    ساكن فراغ
socketserver_client_close(channel_T *channel)
{
    إذا (channel == client_channels)
	client_channels = channel->ch_ss_ايندڙ;
    إذا (channel->ch_ss_prev != عدم)
	channel->ch_ss_prev->ch_ss_ايندڙ = channel->ch_ss_ايندڙ;
    إذا (channel->ch_ss_ايندڙ != عدم)
	channel->ch_ss_ايندڙ->ch_ss_prev = channel->ch_ss_prev;
    إذا (client_channel_ڳڻپ > 0)
	--client_channel_ڳڻپ;

    ch_log(عدم, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    ساكن فراغ
socketserver_accept(channel_T *channel)
{
    إذا (client_channel_ڳڻپ >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(عدم, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	أرجع;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_ايندڙ = عدم;
    إذا (client_channels == عدم)
    {
	channel->ch_ss_prev = عدم;
	client_channels = channel;
    }
    وإلا
    {
	channel_T *آخري = client_channels;

	طالما (آخري->ch_ss_ايندڙ != عدم)
	    آخري = آخري->ch_ss_ايندڙ;
	آخري->ch_ss_ايندڙ = channel;
	channel->ch_ss_prev = آخري;
    }
    ++client_channel_ڳڻپ;

    // We will read the command from the client later in the input loop.
    ch_log(عدم, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    ساكن فراغ
socketserver_close(channel_T *channel اڻ_استعمال_ٿيل)
{
    socketserver_cleanup();

    ch_log(عدم, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    صحيح
سيٽ_ref_اندريون_socketserver_channel(صحيح copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || سيٽ_ref_اندريون_شي(&tv, copyID, عدم, عدم, عدم);

    FOR_ALL_CLIENTS(channel)
    {
	إذا (abort)
	    اكسر;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || سيٽ_ref_اندريون_شي(&tv, copyID, عدم, عدم, عدم);
    }
    أرجع abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    ساكن صحيح
socketserver_send_پيغام(channel_T *ch, dict_T *نياپو, حرف *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    صحيح		ret;

    إذا (ver)
	dict_جوڙيو_عدد(نياپو, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = نياپو;
    buf = json_encode(&tv, JSON_NL);

    إذا (buf != عدم)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (صحيح)STRLEN(buf), func);
	emsg_silent--;
    }
    وإلا
	أرجع FAIL;
    vim_free(buf);
    أرجع ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    ساكن فراغ
socketserver_exec(channel_T *channel, dict_T *پيغام)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = عدم;
    char_u	*sender = عدم;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_ڳوليو(پيغام, (char_u *)"type", -1);
    إذا (di == عدم || di->di_tv.v_type != VAR_STRING)
	أرجع;
    وإلا
	type = di->di_tv.vval.v_string;

    di = dict_ڳوليو(پيغام, (char_u *)"str", -1);
    إذا (di != عدم && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_ڳوليو(پيغام, (char_u *)"sender", -1);
    إذا (di != عدم && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_ڳوليو(پيغام, (char_u *)"wait", -1);
    إذا (di != عدم && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_عدد)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((حرف *)idbuf, حجم(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(عدم, "socketserver_exec(): result: %s",
	    str == عدم ? (char_u *)"(null)" : str);

    إذا (STRCMP(type, "expr") == 0 && str != عدم)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *نتيجو;
	صحيح	    ڪوڊ;

	dict = dict_alloc();
	إذا (dict == عدم)
	    أرجع;

	نتيجو = eval_client_اظهار_منزل_string(str);
	ڪوڊ = نتيجو == عدم ? -1 : 0;

	dict_جوڙيو_string(dict, "type", (char_u *)"reply");
	إذا (نتيجو != عدم)
	{
	    dict_جوڙيو_string(dict, "str", نتيجو);
	    vim_free(نتيجو);
	}
	وإلا
	    // Error occurred, return error message
	    dict_جوڙيو_string(dict, "str",
		    (char_u *)_(e_invalid_اظهار_received));

	dict_جوڙيو_عدد(dict, "code", ڪوڊ);

	socketserver_send_پيغام(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    وإلا إذا (STRCMP(type, "keystrokes") == 0 && str != عدم)
    {
	// Execute keystrokes
	server_منزل_داخلا_buf(str);
    }
    وإلا إذا (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	إذا (sender != عدم && str != عدم)
	{
	    ss_جواب_T *جواب;

	    جواب = socketserver_جوڙيو_جواب(sender);

	    إذا (جواب != عدم)
		ga_copy_string(&جواب->لڙيون, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    وإلا إذا (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_نسخو_mismatch));
    }
    وإلا
	ch_error(عدم, "socketserver: unknown command type '%s'", type);
}

    ساكن صحيح
socketserver_get_پيغام(channel_T *channel, typval_T **tv)
{
    jsonq_T *سر = &channel->ch_part[PART_SOCK].ch_json_سر;
    jsonq_T *json_نياپو = سر->jq_ايندڙ;
    صحيح	    ver;

    إذا (json_نياپو == عدم)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_نياپو = سر->jq_ايندڙ;
    }
    إذا (json_نياپو == عدم)
	أرجع FAIL;
    *tv = json_نياپو->jq_قدر;
    remove_json_ڳنڍ(سر, json_نياپو);

    إذا ((*tv)->v_type != VAR_DICT)
    {
	ch_error(عدم, "socketserver: message is not a JSON object");
	free_tv(*tv);
	أرجع FAIL;
    }

    ver = dict_get_عدد((*tv)->vval.v_dict, "version");

    إذا (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(عدم, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	إذا (dict != عدم)
	{
	    dict_جوڙيو_string(dict, "type", (char_u *)"ver");
	    socketserver_send_پيغام(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    فراغ
socketserver_parse_messages(فراغ)
{
    typval_T	*tv;

    لكل (channel_T *ch = client_channels; ch != عدم;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *ايندڙ = ch->ch_ss_ايندڙ;

	// Get the JSON message if there is any from the queue for this channel.
	إذا (socketserver_get_پيغام(ch, &tv) == FAIL)
	{
	    ch = ايندڙ;
	    استمر;
	}

	socketserver_exec(ch, tv->vval.v_dict);
	free_tv(tv);
	ch = ايندڙ;
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
    ساكن صحيح
socketserver_wait(channel_T *channel, صحيح timeout)
{
    طالما (true)
    {
	صحيح		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	بنية timeval  tv;
	صحيح		maxfd = -1;

	إذا (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	إذا (channel != عدم)
	{
	    إذا (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		أرجع FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	إذا (server_channel != عدم && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    إذا (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    إذا (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (صحيح)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		إذا (maxfd < (صحيح)ch->CH_SOCK_FD)
		    maxfd = (صحيح)ch->CH_SOCK_FD;
	    }
	}

	إذا (maxfd == -1)
	    أرجع FAIL;

	ret = select(maxfd + 1, &rfds, عدم, عدم, timeout == -1 ? عدم : &tv);

#  ifdef EINTR
	إذا (ret == -1 && errno == EINTR)
	{
	    إذا (got_int)
		اكسر;
	    استمر;
	}
#  endif

	إذا (ret > 0)
	{
	    إذا (server_channel != عدم
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		إذا (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (صحيح)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    إذا (channel == عدم)
		أرجع OK;

	    إذا (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		أرجع OK;
	    }
	    استمر;
	}
# وإلا
	بنية pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	صحيح		nfd = 0;
	صحيح		channel_idx = -1;
	صحيح		server_idx = -1;

	إذا (channel != عدم)
	{
	    إذا (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		أرجع FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	إذا (server_channel != عدم && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    إذا (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	إذا (ret == -1 && errno == EINTR)
	{
	    إذا (got_int)
		اكسر;
	    استمر;
	}
#  endif

	إذا (ret > 0)
	{
	    إذا (server_channel != عدم
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		إذا (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    إذا (channel == عدم)
		أرجع OK;

	    إذا (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		أرجع OK;
	    }
	    استمر;
	}
# endif
	اكسر;
    }
    أرجع FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    ساكن channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*پتو = عدم;
    صحيح		پورٽ;
    bool	is_unix = false;
    channel_T	*channel;

    إذا (STRNICMP(name, "channel:", 8) == 0)
    {
	إذا (channel_parse_socketserver_پتو(name + 8, &پورٽ, &پتو, true)
		== FAIL)
	    أرجع عدم;
	إذا (پتو != عدم)
	    is_unix = true;
    }
    وإلا إذا (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	صحيح id;

	إذا (name[11] == NUL)
	    أرجع عدم;
	id = strtol((حرف *)name + 11, عدم, 10);
	إذا (wait != عدم)
	    *wait = true;

	أرجع channel_ڳوليو(id);
    }
    وإلا
    {
	پتو = socketserver_get_رستو(name, false, quiet, false, عدم);
	إذا (پتو == عدم)
	    أرجع عدم;
	is_unix = true;
    }

    إذا (is_unix)
	channel = channel_open_unix((حرف *)پتو, عدم);
    وإلا
	channel =  channel_open("localhost", پورٽ, 1000, عدم);

    إذا (channel == عدم && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(پتو);

    أرجع channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    صحيح
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **نتيجو,
	bool is_اظهار,
	صحيح timeout,
	bool silent,
	channel_T **ch)
{
    صحيح		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = عدم;

    إذا (*name == NUL)
    {
	semsg(_(e_unable_منزل_send_منزل_str), name);
	أرجع -1;
    }

    // Execute locally if target is ourselves
    إذا (serverName != عدم && STRICMP(name, serverName) == 0)
	أرجع send_منزل_Local_Vim(str, is_اظهار, نتيجو);

    channel = socketserver_get_channel(name, silent, عدم);
    إذا (channel == عدم)
	أرجع -1;

    dict = dict_alloc();
    إذا (dict == عدم)
	اذهب ٻاهر_نڪرو;

    dict_جوڙيو_string(dict, "type", (char_u *)(is_اظهار ? "expr" : "keystrokes"));
    dict_جوڙيو_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    إذا (server_پتو != عدم)
	dict_جوڙيو_string(dict, "sender", server_پتو);
    وإلا إذا (ch != عدم)
	dict_جوڙيو_bool(dict, "wait", true);

    إذا (socketserver_send_پيغام(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_منزل_send_منزل_str), name);
	dict_unref(dict);
	اذهب ٻاهر_نڪرو;
    }
    dict_unref(dict);

    إذا (!is_اظهار)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	إذا (ch != عدم)
	{
	    *ch = channel;

	    channel->ch_ss_ايندڙ = client_channels;
	    channel->ch_ss_prev = عدم;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    أرجع rcode;
	}
	اذهب ٻاهر_نڪرو;
    }

    إذا (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    طالما (socketserver_wait(channel, timeout) == OK)
	إذا (socketserver_get_پيغام(channel, &resp_tv) == OK)
	    اكسر;

    إذا (resp_tv == عدم)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	اذهب ٻاهر_نڪرو;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_ڳوليو(dict, (char_u *)"type", -1);
    إذا (di == عدم || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(عدم, "socketserver: unknown reply type");
	free_tv(resp_tv);
	اذهب ٻاهر_نڪرو;
    }

    إذا (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_نسخو_mismatch));
	free_tv(resp_tv);
	اذهب ٻاهر_نڪرو;
    }

    إذا (نتيجو != عدم)
    {
	di = dict_ڳوليو(dict, (char_u *)"str", -1);
	إذا (di != عدم && di->di_tv.v_type == VAR_STRING)
	    *نتيجو = vim_strsave(di->di_tv.vval.v_string);
	وإلا
	{
	    free_tv(resp_tv);
	    اذهب ٻاهر_نڪرو;
	}
    }

    di = dict_ڳوليو(dict, (char_u *)"code", -1);
    إذا (di != عدم && di->di_tv.v_type == VAR_عدد)
	rcode = di->di_tv.vval.v_عدد;

    free_tv(resp_tv);

ٻاهر_نڪرو:
    channel_close(channel, false);
    channel_clear(channel);
    أرجع rcode;
}

   ساكن ss_جواب_T *
socketserver_get_جواب(char_u *sender, صحيح *اشاريو)
{
    لكل (صحيح i = 0; i < server_replies.ga_len; i++)
    {
	ss_جواب_T *جواب = ((ss_جواب_T *)server_replies.ga_معلومات) + i;

	إذا (STRCMP(جواب->sender, sender) == 0)
	{
	    إذا (اشاريو != عدم)
		*اشاريو = i;
	    أرجع جواب;
	}
    }
    أرجع عدم;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    ساكن ss_جواب_T *
socketserver_جوڙيو_جواب(char_u *sender)
{
    ss_جواب_T *جواب;

    إذا (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, حجم(ss_جواب_T), 1);

    جواب = socketserver_get_جواب(sender, عدم);

    إذا (جواب == عدم && ga_grow(&server_replies, 1) == OK)
    {
	جواب = ((ss_جواب_T *)server_replies.ga_معلومات) + server_replies.ga_len++;

	جواب->sender = vim_strsave(sender);

	إذا (جواب->sender == عدم)
	    أرجع عدم;

	ga_init2(&جواب->لڙيون, حجم(char_u *), 5);
    }

    أرجع جواب;
}

    ساكن فراغ
socketserver_remove_جواب(char_u *sender)
{
    صحيح اشاريو;
    ss_جواب_T *جواب = socketserver_get_جواب(sender, &اشاريو);

    إذا (جواب != عدم)
    {
	ss_جواب_T  *arr = server_replies.ga_معلومات;
	صحيح	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(جواب->sender);
	ga_clear_لڙيون(&جواب->لڙيون);

	// Move all elements after the removed reply forward by one
	إذا (len > 1)
	    mch_memmove(arr + اشاريو, arr + اشاريو + 1,
		    حجم(ss_جواب_T) * (len - اشاريو - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    صحيح
socketserver_send_جواب(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    صحيح		ret = OK;
    bool	wait = false;

    إذا (*client == NUL)
    {
	semsg(_(e_invalid_server_id_استعمال_ٿيل_str), client);
	أرجع FAIL;
    }

    إذا (server_channel == عدم || server_پتو == عدم)
    {
	emsg(_(e_socket_server_not_online));
	أرجع FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    إذا (channel == عدم)
	أرجع FAIL;

    dict = dict_alloc();
    إذا (dict == عدم)
    {
	ret = FAIL;
	اذهب ٻاهر_نڪرو;
    }

    dict_جوڙيو_string(dict, "type", (char_u *)"notify");
    dict_جوڙيو_string(dict, "str", str);
    إذا (server_پتو != عدم)
	dict_جوڙيو_string(dict, "sender", server_پتو);

    ret = socketserver_send_پيغام(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

ٻاهر_نڪرو:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    إذا (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    أرجع ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    صحيح
socketserver_read_جواب(
	char_u *client,
	char_u **str,
	صحيح timeout,
	bool remotewait)
{
    ss_جواب_T	*جواب = عدم;
    char_u	*actual;

    إذا (*client == NUL)
    {
	semsg(_(e_invalid_server_id_استعمال_ٿيل_str), client);
	أرجع FAIL;
    }

    إذا (!remotewait && (server_channel == عدم || server_پتو == عدم))
    {
	emsg(_(e_socket_server_not_online));
	أرجع FAIL;
    }

    actual = socketserver_get_رستو(client, false, false, false, عدم);
    إذا (actual == عدم)
	أرجع FAIL;

    طالما (true)
    {
	جواب = socketserver_get_جواب(actual, عدم);
	إذا (جواب != عدم)
	    اكسر;
	إذا (socketserver_wait(عدم, timeout) == FAIL)
	    اكسر;
    }

    إذا (جواب == عدم || جواب->لڙيون.ga_len == 0)
    {
	vim_free(actual);
	أرجع FAIL;
    }

    // Consume the string
    *str = ((char_u **)جواب->لڙيون.ga_معلومات)[0];

    إذا (جواب->لڙيون.ga_len > 1)
	mch_memmove((char_u **)جواب->لڙيون.ga_معلومات,
		((char_u **)جواب->لڙيون.ga_معلومات) + 1,
		حجم(char_u *) * (جواب->لڙيون.ga_len - 1));
    جواب->لڙيون.ga_len--;

    إذا (جواب->لڙيون.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_جواب(actual);

    vim_free(actual);

    أرجع OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    صحيح
socketserver_peek_جواب(char_u *sender, char_u **str)
{
    ss_جواب_T	*جواب;
    char_u	*actual;

    إذا (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_استعمال_ٿيل_str), sender);
	أرجع FAIL;
    }

    إذا (server_channel == عدم || server_پتو == عدم)
    {
	emsg(_(e_socket_server_not_online));
	أرجع FAIL;
    }

    actual = socketserver_get_رستو(sender, false, false, false, عدم);
    إذا (actual == عدم)
	أرجع FAIL;

    جواب = socketserver_get_جواب(actual, عدم);
    vim_free(actual);

    إذا (جواب != عدم && جواب->لڙيون.ga_len > 0)
    {
	إذا (str != عدم)
	    *str = ((char_u **)جواب->لڙيون.ga_معلومات)[0];
	أرجع 1;
    }
    أرجع 0;
}

#جيڪڏهن_پورو // FEAT_SOCKETSERVER
