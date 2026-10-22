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

#اگر_تعریف_شدہ FEAT_SOCKETSERVER

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
قسمتعریف ساخت
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T لڑیاں;
} ss_جواب_T;

ساکن channel_T    *server_channel = تہی;
ساکن char_u	    *server_پتہ = تہی; // Includes "type:" prefix if any
ساکن bool	    server_is_unix = false;
ساکن char_u	    *server_addr_cwd = تہی; // CWD when server was started, used
					  // to handle relative file paths.
ساکن garray_T	    server_replies;

ساکن channel_T    *client_channels = تہی;
ساکن صحیح	    client_channel_گنتی = 0;

# define FOR_ALL_CLIENTS(ch) \
    برائے (ch = client_channels; ch != تہی; ch = ch->ch_ss_اگلا)

ساکن خالی socketserver_cleanup(خالی);
ساکن char_u *socketserver_create_راستہ(char_u *name, bool quiet);
ساکن char_u *socketserver_get_راستہ(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
ساکن خالی socketserver_accept(channel_T *channel);
ساکن خالی socketserver_close(channel_T *channel);
ساکن ss_جواب_T *socketserver_جوڑو_جواب(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    صحیح
socketserver_آغاز(char_u *name, bool quiet)
{
    char_u	*پتہ = تہی;
    صحیح		پورٹ;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    اگر (server_channel != تہی)
	واپس OK;

    اگر (STRNICMP(name, "channel:", 8) == 0)
    {
	اگر (channel_parse_socketserver_پتہ(name + 8, &پورٹ, &پتہ, quiet)
		== FAIL)
	    واپس FAIL;
	اگر (پتہ != تہی)
	    is_unix = true;
    }
    ورنہ
    {
	پتہ = socketserver_create_راستہ(name, quiet);
	اگر (پتہ == تہی)
	    واپس FAIL;
	is_unix = true;
    }

    اگر (is_unix)
	channel = channel_listen_unix((حرف *)پتہ, تہی, false);
    ورنہ
	channel = channel_listen(پورٹ, تہی);

    اگر (channel == تہی)
    {
	vim_free(پتہ);
	واپس FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    اگر (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    ورنہ
    {
	buf = alloc(MAXPATHL + 1);
	اگر (buf != تہی)
	{
	    buf[0] = NUL;
	    mch_بھرا_Name(پتہ, buf, MAXPATHL, false);
	}
    }
    vim_free(پتہ);

    اگر (buf != تہی)
    {
	server_پتہ = vim_strsave(buf);
	serverName = buf;
	سیٹ_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    اگر (mch_dirname(dirbuf, حجم(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    ورنہ
	server_addr_cwd = تہی;

    ga_init2(&server_replies, حجم(ss_جواب_T), 2);

    ch_log(تہی, "socketserver: started server at %s", name);

    واپس OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    خالی
socketserver_stop(خالی)
{
    اگر (server_channel == تہی)
	واپس;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(تہی, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    ساکن خالی
socketserver_cleanup(خالی)
{
# ifdef UNIX
    اگر (server_is_unix && server_پتہ != تہی)
    {
	char_u *راستہ = server_پتہ;
	char_u dirbuf[MAXPATHL];

	اگر (STRNICMP(راستہ, "channel:unix:", 13) == 0)
	    راستہ += 13;
	ورنہ اگر (STRNICMP(راستہ, "name:", 5) == 0)
	    راستہ += 5;

	اگر (*راستہ == '/')
	    mch_remove(راستہ);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	ورنہ اگر (server_addr_cwd != تہی
		&& mch_dirname(dirbuf, حجم(dirbuf)) == OK)
	{
	    اگر (mch_chdir((حرف *)server_addr_cwd) == 0)
	    {
		mch_remove(راستہ);
		اگر (mch_chdir((حرف *)dirbuf) < 0)
		    emsg(_(e_cannot_go_عقب_مقصد_پچھلا_directory));
	    }
	}
    }
# endif

    server_channel = تہی;
    VIM_CLEAR(server_پتہ);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    برائے (صحیح i = 0; i < server_replies.ga_len; i++)
    {
	ss_جواب_T *جواب = (ss_جواب_T *)server_replies.ga_ڈیٹا + i;

	vim_free(جواب->sender);
	ga_clear_لڑیاں(&جواب->لڑیاں);
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
socketserver_list(خالی)
{
    list_T *list = list_alloc();

    اگر (list == تہی)
	واپس تہی;

# ifdef MSWIN
    // Only support addresses on Windows
    واپس list;
# ورنہ
    string_T	    buf;
    string_T	    راستہ;
    DIR		    *dirp;
    ساخت dirent   *dp;
    ثابت char_u    *معلوم_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    اگر ((buf.string = alloc(MAXPATHL)) == تہی)
	واپس تہی;
    اگر ((راستہ.string = alloc(MAXPATHL)) == تہی)
    {
	vim_free(buf.string);
	واپس تہی;
    }
    buf.لمبائی = 0;
    راستہ.لمبائی = 0;

    برائے (size_t i = 0 ; i < ARRAY_لمبائی(معلوم_dirs); i++)
    {
	ثابت char_u *dir = معلوم_dirs[i];

	اگر (dir == تہی)
	    جاری;

	اگر (STRCMP(dir, "/tmp") == 0 ||
		(معلوم_dirs[1] != تہی && STRCMP(dir, معلوم_dirs[1]) == 0))
	    راستہ.لمبائی = vim_snprintf_safelen((حرف *)راستہ.string, MAXPATHL,
		    "%s/vim-%lu", dir, (غیرعلامتی طویل صحیح)getuid());
	ورنہ
	    راستہ.لمبائی = vim_snprintf_safelen((حرف *)راستہ.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((حرف *)راستہ.string);
	اگر (dirp == تہی)
	    جاری;

	// Loop through directory
	جب ((dp = readdir(dirp)) != تہی)
	{
	    اگر (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		جاری;

	    buf.لمبائی = vim_snprintf_safelen((حرف *)buf.string, MAXPATHL,
		    "%s/%s", راستہ.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.لمبائی - (راستہ.لمبائی + 1));
	}

	closedir(dirp);

	توڑو;
    }

    vim_free(راستہ.string);
    vim_free(buf.string);

    واپس list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    ساکن char_u *
socketserver_create_راستہ(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	واپس vim_strsave(name + 8);
    ورنہ
    {
	اگر (!quiet)
	    semsg(_(e_invalid_دلیل_str), name);
	واپس تہی;
    }
# ورنہ
    char_u  *buf = تہی;
    صحیح	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *راستہ = تہی;
    bool    fatal = false;

    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	واپس vim_strsave(name + 8);
    اگر (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    برائے (صحیح i = 0; i < 1000; i++)
    {
	اگر (buf != تہی)
	{
	    vim_snprintf((حرف *)buf, buflen, "%s%d", name, i);
	    راستہ = socketserver_get_راستہ(buf, true, quiet, true, &fatal);
	}
	ورنہ
	    راستہ = socketserver_get_راستہ(name, true, quiet, true, &fatal);

	اگر (fatal)
	    توڑو;

	اگر (راستہ == تہی)
	{
	    اگر (buf == تہی)
	    {
		buf = alloc(buflen);
		اگر (buf == تہی)
		{
		    semsg(_(e_برآمد_کا_حافظہ_allocating_nr_بائٹیں), buflen);
		    واپس تہی;
		}
	    }
	    جاری;
	}
	توڑو;
    }
    vim_free(buf);

    واپس راستہ;
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
    ساکن char_u *
socketserver_get_راستہ(
	char_u *name,
	bool new غیر_مستعمل,
	bool quiet,
	bool ignore غیر_مستعمل,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	واپس vim_strsave(name + 8);
    ورنہ
    {
	اگر (!quiet)
	    semsg(_(e_invalid_دلیل_str), name);
	اگر (fatal != تہی)
	    *fatal = true;
	واپس تہی;
    }
# ورنہ
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ثابت char_u    *معلوم_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    اگر (name == تہی)
    {
	اگر (fatal != تہی)
	    *fatal = true;
	واپس تہی;
    }

    // Ignore if name is a path
    اگر (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	واپس vim_strsave(name);

    اگر (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	واپس vim_strsave(name + 8);

    اگر (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    اگر (vim_strchr(name, '/') != تہی)
    {
	اگر (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	اگر (fatal != تہی)
	    *fatal = true;
	واپس تہی;
    }

    buf = alloc(MAXPATHL);

    اگر (buf == تہی)
    {
	اگر (fatal != تہی)
	    *fatal = true;
	semsg(_(e_برآمد_کا_حافظہ_allocating_nr_بائٹیں), MAXPATHL);
	واپس تہی;
    }

    برائے (size_t i = 0; i < ARRAY_لمبائی(معلوم_dirs); i++)
    {
	ثابت char_u	*dir = معلوم_dirs[i];
	bool		got = false;

	اگر (dir == تہی)
	    جاری;
	ورنہ اگر (STRCMP(dir, "/tmp") == 0 ||
		(معلوم_dirs[1] != تہی && STRCMP(dir, معلوم_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (غیرعلامتی طویل صحیح)getuid());
	    اگر (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		جاری;

	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (غیرعلامتی طویل صحیح)getuid(), name);
	}
	ورنہ
	{
	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim", dir);
	    اگر (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		جاری;

	    vim_snprintf((حرف *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	اگر (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((حرف *)buf, تہی);
	    emsg_off--;
	    emsg_silent--;

	    اگر (channel != تہی)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    ورنہ
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	اگر (got || (!new && mch_access((حرف *)buf, F_OK) == 0))
	{
	    اگر (server_پتہ != تہی && STRCMP(buf, server_پتہ) == 0)
		// Can't connect to itself
		توڑو;
	    واپس buf;
	}
	توڑو;
    }

    اگر (!quiet)
    {
	اگر (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	ورنہ اگر (!ignore)
	    semsg(_(e_invalid_server_id_مستعمل_str), name);
    }
    اگر (!res && fatal != تہی)
	*fatal = true;

    vim_free(buf);
    واپس تہی;
# endif
}

/*
 * Callback for when client channel is closed
 */
    ساکن خالی
socketserver_client_close(channel_T *channel)
{
    اگر (channel == client_channels)
	client_channels = channel->ch_ss_اگلا;
    اگر (channel->ch_ss_پچھلا != تہی)
	channel->ch_ss_پچھلا->ch_ss_اگلا = channel->ch_ss_اگلا;
    اگر (channel->ch_ss_اگلا != تہی)
	channel->ch_ss_اگلا->ch_ss_پچھلا = channel->ch_ss_پچھلا;
    اگر (client_channel_گنتی > 0)
	--client_channel_گنتی;

    ch_log(تہی, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    ساکن خالی
socketserver_accept(channel_T *channel)
{
    اگر (client_channel_گنتی >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(تہی, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	واپس;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_اگلا = تہی;
    اگر (client_channels == تہی)
    {
	channel->ch_ss_پچھلا = تہی;
	client_channels = channel;
    }
    ورنہ
    {
	channel_T *آخری = client_channels;

	جب (آخری->ch_ss_اگلا != تہی)
	    آخری = آخری->ch_ss_اگلا;
	آخری->ch_ss_اگلا = channel;
	channel->ch_ss_پچھلا = آخری;
    }
    ++client_channel_گنتی;

    // We will read the command from the client later in the input loop.
    ch_log(تہی, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    ساکن خالی
socketserver_close(channel_T *channel غیر_مستعمل)
{
    socketserver_cleanup();

    ch_log(تہی, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    صحیح
سیٹ_ref_اندر_socketserver_channel(صحیح copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);

    FOR_ALL_CLIENTS(channel)
    {
	اگر (abort)
	    توڑو;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
    }
    واپس abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    ساکن صحیح
socketserver_send_پیغام(channel_T *ch, dict_T *پیغام, حرف *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    صحیح		ret;

    اگر (ver)
	dict_جوڑو_نمبر(پیغام, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = پیغام;
    buf = json_encode(&tv, JSON_NL);

    اگر (buf != تہی)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (صحیح)STRLEN(buf), func);
	emsg_silent--;
    }
    ورنہ
	واپس FAIL;
    vim_free(buf);
    واپس ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    ساکن خالی
socketserver_exec(channel_T *channel, dict_T *پیغام_2)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = تہی;
    char_u	*sender = تہی;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_ڈھونڈو(پیغام_2, (char_u *)"type", -1);
    اگر (di == تہی || di->di_tv.v_type != VAR_STRING)
	واپس;
    ورنہ
	type = di->di_tv.vval.v_string;

    di = dict_ڈھونڈو(پیغام_2, (char_u *)"str", -1);
    اگر (di != تہی && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_ڈھونڈو(پیغام_2, (char_u *)"sender", -1);
    اگر (di != تہی && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_ڈھونڈو(پیغام_2, (char_u *)"wait", -1);
    اگر (di != تہی && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_نمبر)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((حرف *)idbuf, حجم(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(تہی, "socketserver_exec(): result: %s",
	    str == تہی ? (char_u *)"(null)" : str);

    اگر (STRCMP(type, "expr") == 0 && str != تہی)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *نتیجہ;
	صحیح	    رمز;

	dict = dict_alloc();
	اگر (dict == تہی)
	    واپس;

	نتیجہ = eval_client_اظہاریہ_مقصد_string(str);
	رمز = نتیجہ == تہی ? -1 : 0;

	dict_جوڑو_string(dict, "type", (char_u *)"reply");
	اگر (نتیجہ != تہی)
	{
	    dict_جوڑو_string(dict, "str", نتیجہ);
	    vim_free(نتیجہ);
	}
	ورنہ
	    // Error occurred, return error message
	    dict_جوڑو_string(dict, "str",
		    (char_u *)_(e_invalid_عبارت_received));

	dict_جوڑو_نمبر(dict, "code", رمز);

	socketserver_send_پیغام(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    ورنہ اگر (STRCMP(type, "keystrokes") == 0 && str != تہی)
    {
	// Execute keystrokes
	server_مقصد_داخلہ_buf(str);
    }
    ورنہ اگر (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	اگر (sender != تہی && str != تہی)
	{
	    ss_جواب_T *جواب;

	    جواب = socketserver_جوڑو_جواب(sender);

	    اگر (جواب != تہی)
		ga_copy_string(&جواب->لڑیاں, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    ورنہ اگر (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_نسخہ_mismatch));
    }
    ورنہ
	ch_error(تہی, "socketserver: unknown command type '%s'", type);
}

    ساکن صحیح
socketserver_get_پیغام(channel_T *channel, typval_T **tv)
{
    jsonq_T *سر = &channel->ch_part[PART_SOCK].ch_json_سر;
    jsonq_T *json_پیغام = سر->jq_اگلا;
    صحیح	    ver;

    اگر (json_پیغام == تہی)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_پیغام = سر->jq_اگلا;
    }
    اگر (json_پیغام == تہی)
	واپس FAIL;
    *tv = json_پیغام->jq_قدر;
    remove_json_گرہ(سر, json_پیغام);

    اگر ((*tv)->v_type != VAR_DICT)
    {
	ch_error(تہی, "socketserver: message is not a JSON object");
	free_tv(*tv);
	واپس FAIL;
    }

    ver = dict_get_نمبر((*tv)->vval.v_dict, "version");

    اگر (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(تہی, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	اگر (dict != تہی)
	{
	    dict_جوڑو_string(dict, "type", (char_u *)"ver");
	    socketserver_send_پیغام(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    خالی
socketserver_parse_messages(خالی)
{
    typval_T	*tv;

    برائے (channel_T *ch = client_channels; ch != تہی;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *اگلا = ch->ch_ss_اگلا;

	// Get the JSON message if there is any from the queue for this channel.
	اگر (socketserver_get_پیغام(ch, &tv) == FAIL)
	{
	    ch = اگلا;
	    جاری;
	}

	socketserver_exec(ch, tv->vval.v_dict);
	free_tv(tv);
	ch = اگلا;
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
    ساکن صحیح
socketserver_wait(channel_T *channel, صحیح timeout)
{
    جب (true)
    {
	صحیح		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	ساخت timeval  tv;
	صحیح		maxfd = -1;

	اگر (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	اگر (channel != تہی)
	{
	    اگر (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		واپس FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	اگر (server_channel != تہی && server_channel->CH_SOCK_FD != INVALID_FD)
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
	    واپس FAIL;

	ret = select(maxfd + 1, &rfds, تہی, تہی, timeout == -1 ? تہی : &tv);

#  ifdef EINTR
	اگر (ret == -1 && errno == EINTR)
	{
	    اگر (got_int)
		توڑو;
	    جاری;
	}
#  endif

	اگر (ret > 0)
	{
	    اگر (server_channel != تہی
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

	    اگر (channel == تہی)
		واپس OK;

	    اگر (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		واپس OK;
	    }
	    جاری;
	}
# ورنہ
	ساخت pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	صحیح		nfd = 0;
	صحیح		channel_idx = -1;
	صحیح		server_idx = -1;

	اگر (channel != تہی)
	{
	    اگر (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		واپس FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	اگر (server_channel != تہی && server_channel->CH_SOCK_FD != INVALID_FD)
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
		توڑو;
	    جاری;
	}
#  endif

	اگر (ret > 0)
	{
	    اگر (server_channel != تہی
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		اگر (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    اگر (channel == تہی)
		واپس OK;

	    اگر (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		واپس OK;
	    }
	    جاری;
	}
# endif
	توڑو;
    }
    واپس FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    ساکن channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*پتہ = تہی;
    صحیح		پورٹ;
    bool	is_unix = false;
    channel_T	*channel;

    اگر (STRNICMP(name, "channel:", 8) == 0)
    {
	اگر (channel_parse_socketserver_پتہ(name + 8, &پورٹ, &پتہ, true)
		== FAIL)
	    واپس تہی;
	اگر (پتہ != تہی)
	    is_unix = true;
    }
    ورنہ اگر (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	صحیح id;

	اگر (name[11] == NUL)
	    واپس تہی;
	id = strtol((حرف *)name + 11, تہی, 10);
	اگر (wait != تہی)
	    *wait = true;

	واپس channel_ڈھونڈو(id);
    }
    ورنہ
    {
	پتہ = socketserver_get_راستہ(name, false, quiet, false, تہی);
	اگر (پتہ == تہی)
	    واپس تہی;
	is_unix = true;
    }

    اگر (is_unix)
	channel = channel_open_unix((حرف *)پتہ, تہی);
    ورنہ
	channel =  channel_open("localhost", پورٹ, 1000, تہی);

    اگر (channel == تہی && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(پتہ);

    واپس channel;
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
	char_u **نتیجہ,
	bool is_اظہاریہ,
	صحیح timeout,
	bool silent,
	channel_T **ch)
{
    صحیح		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = تہی;

    اگر (*name == NUL)
    {
	semsg(_(e_unable_مقصد_send_مقصد_str), name);
	واپس -1;
    }

    // Execute locally if target is ourselves
    اگر (serverName != تہی && STRICMP(name, serverName) == 0)
	واپس send_مقصد_Local_Vim(str, is_اظہاریہ, نتیجہ);

    channel = socketserver_get_channel(name, silent, تہی);
    اگر (channel == تہی)
	واپس -1;

    dict = dict_alloc();
    اگر (dict == تہی)
	جاؤ باہر_نکلو;

    dict_جوڑو_string(dict, "type", (char_u *)(is_اظہاریہ ? "expr" : "keystrokes"));
    dict_جوڑو_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    اگر (server_پتہ != تہی)
	dict_جوڑو_string(dict, "sender", server_پتہ);
    ورنہ اگر (ch != تہی)
	dict_جوڑو_bool(dict, "wait", true);

    اگر (socketserver_send_پیغام(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_مقصد_send_مقصد_str), name);
	dict_unref(dict);
	جاؤ باہر_نکلو;
    }
    dict_unref(dict);

    اگر (!is_اظہاریہ)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	اگر (ch != تہی)
	{
	    *ch = channel;

	    channel->ch_ss_اگلا = client_channels;
	    channel->ch_ss_پچھلا = تہی;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    واپس rcode;
	}
	جاؤ باہر_نکلو;
    }

    اگر (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    جب (socketserver_wait(channel, timeout) == OK)
	اگر (socketserver_get_پیغام(channel, &resp_tv) == OK)
	    توڑو;

    اگر (resp_tv == تہی)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	جاؤ باہر_نکلو;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_ڈھونڈو(dict, (char_u *)"type", -1);
    اگر (di == تہی || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(تہی, "socketserver: unknown reply type");
	free_tv(resp_tv);
	جاؤ باہر_نکلو;
    }

    اگر (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_نسخہ_mismatch));
	free_tv(resp_tv);
	جاؤ باہر_نکلو;
    }

    اگر (نتیجہ != تہی)
    {
	di = dict_ڈھونڈو(dict, (char_u *)"str", -1);
	اگر (di != تہی && di->di_tv.v_type == VAR_STRING)
	    *نتیجہ = vim_strsave(di->di_tv.vval.v_string);
	ورنہ
	{
	    free_tv(resp_tv);
	    جاؤ باہر_نکلو;
	}
    }

    di = dict_ڈھونڈو(dict, (char_u *)"code", -1);
    اگر (di != تہی && di->di_tv.v_type == VAR_نمبر)
	rcode = di->di_tv.vval.v_نمبر;

    free_tv(resp_tv);

باہر_نکلو:
    channel_close(channel, false);
    channel_clear(channel);
    واپس rcode;
}

   ساکن ss_جواب_T *
socketserver_get_جواب(char_u *sender, صحیح *اشاریہ)
{
    برائے (صحیح i = 0; i < server_replies.ga_len; i++)
    {
	ss_جواب_T *جواب = ((ss_جواب_T *)server_replies.ga_ڈیٹا) + i;

	اگر (STRCMP(جواب->sender, sender) == 0)
	{
	    اگر (اشاریہ != تہی)
		*اشاریہ = i;
	    واپس جواب;
	}
    }
    واپس تہی;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    ساکن ss_جواب_T *
socketserver_جوڑو_جواب(char_u *sender)
{
    ss_جواب_T *جواب;

    اگر (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, حجم(ss_جواب_T), 1);

    جواب = socketserver_get_جواب(sender, تہی);

    اگر (جواب == تہی && ga_grow(&server_replies, 1) == OK)
    {
	جواب = ((ss_جواب_T *)server_replies.ga_ڈیٹا) + server_replies.ga_len++;

	جواب->sender = vim_strsave(sender);

	اگر (جواب->sender == تہی)
	    واپس تہی;

	ga_init2(&جواب->لڑیاں, حجم(char_u *), 5);
    }

    واپس جواب;
}

    ساکن خالی
socketserver_remove_جواب(char_u *sender)
{
    صحیح اشاریہ;
    ss_جواب_T *جواب = socketserver_get_جواب(sender, &اشاریہ);

    اگر (جواب != تہی)
    {
	ss_جواب_T  *arr = server_replies.ga_ڈیٹا;
	صحیح	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(جواب->sender);
	ga_clear_لڑیاں(&جواب->لڑیاں);

	// Move all elements after the removed reply forward by one
	اگر (len > 1)
	    mch_memmove(arr + اشاریہ, arr + اشاریہ + 1,
		    حجم(ss_جواب_T) * (len - اشاریہ - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    صحیح
socketserver_send_جواب(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    صحیح		ret = OK;
    bool	wait = false;

    اگر (*client == NUL)
    {
	semsg(_(e_invalid_server_id_مستعمل_str), client);
	واپس FAIL;
    }

    اگر (server_channel == تہی || server_پتہ == تہی)
    {
	emsg(_(e_socket_server_not_online));
	واپس FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    اگر (channel == تہی)
	واپس FAIL;

    dict = dict_alloc();
    اگر (dict == تہی)
    {
	ret = FAIL;
	جاؤ باہر_نکلو;
    }

    dict_جوڑو_string(dict, "type", (char_u *)"notify");
    dict_جوڑو_string(dict, "str", str);
    اگر (server_پتہ != تہی)
	dict_جوڑو_string(dict, "sender", server_پتہ);

    ret = socketserver_send_پیغام(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

باہر_نکلو:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    اگر (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    واپس ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    صحیح
socketserver_read_جواب(
	char_u *client,
	char_u **str,
	صحیح timeout,
	bool remotewait)
{
    ss_جواب_T	*جواب = تہی;
    char_u	*actual;

    اگر (*client == NUL)
    {
	semsg(_(e_invalid_server_id_مستعمل_str), client);
	واپس FAIL;
    }

    اگر (!remotewait && (server_channel == تہی || server_پتہ == تہی))
    {
	emsg(_(e_socket_server_not_online));
	واپس FAIL;
    }

    actual = socketserver_get_راستہ(client, false, false, false, تہی);
    اگر (actual == تہی)
	واپس FAIL;

    جب (true)
    {
	جواب = socketserver_get_جواب(actual, تہی);
	اگر (جواب != تہی)
	    توڑو;
	اگر (socketserver_wait(تہی, timeout) == FAIL)
	    توڑو;
    }

    اگر (جواب == تہی || جواب->لڑیاں.ga_len == 0)
    {
	vim_free(actual);
	واپس FAIL;
    }

    // Consume the string
    *str = ((char_u **)جواب->لڑیاں.ga_ڈیٹا)[0];

    اگر (جواب->لڑیاں.ga_len > 1)
	mch_memmove((char_u **)جواب->لڑیاں.ga_ڈیٹا,
		((char_u **)جواب->لڑیاں.ga_ڈیٹا) + 1,
		حجم(char_u *) * (جواب->لڑیاں.ga_len - 1));
    جواب->لڑیاں.ga_len--;

    اگر (جواب->لڑیاں.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_جواب(actual);

    vim_free(actual);

    واپس OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    صحیح
socketserver_peek_جواب(char_u *sender, char_u **str)
{
    ss_جواب_T	*جواب;
    char_u	*actual;

    اگر (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_مستعمل_str), sender);
	واپس FAIL;
    }

    اگر (server_channel == تہی || server_پتہ == تہی)
    {
	emsg(_(e_socket_server_not_online));
	واپس FAIL;
    }

    actual = socketserver_get_راستہ(sender, false, false, false, تہی);
    اگر (actual == تہی)
	واپس FAIL;

    جواب = socketserver_get_جواب(actual, تہی);
    vim_free(actual);

    اگر (جواب != تہی && جواب->لڑیاں.ga_len > 0)
    {
	اگر (str != تہی)
	    *str = ((char_u **)جواب->لڑیاں.ga_ڈیٹا)[0];
	واپس 1;
    }
    واپس 0;
}

#اگر_ختم // FEAT_SOCKETSERVER
