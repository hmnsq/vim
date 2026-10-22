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

#include "vim.h"

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
typedef sa_nracanaa
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

static channel_T    *server_channel = NULL;
static char_u	    *server_address = NULL; // Includes "type:" prefix if any
static bool	    server_is_unix = false;
static char_u	    *server_addr_cwd = NULL; // CWD when server was started, used
					  // to handle relative file paths.
static garray_T	    server_replies;

static channel_T    *client_channels = NULL;
static puur_naa_nka	    client_channel_count = 0;

# define FOR_ALL_CLIENTS(ch) \
    for (ch = client_channels; ch != NULL; ch = ch->ch_ss_next)

static void socketserver_cleanup(void);
static char_u *socketserver_create_path(char_u *name, bool quiet);
static char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
static void socketserver_accept(channel_T *channel);
static void socketserver_close(channel_T *channel);
static ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    puur_naa_nka
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = NULL;
    puur_naa_nka		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    ᰀᰧ (server_channel != NULL)
	ᰀᰧᰌ OK;

    ᰀᰧ (STRNICMP(name, "channel:", 8) == 0)
    {
	ᰀᰧ (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    ᰀᰧᰌ FAIL;
	ᰀᰧ (address != NULL)
	    is_unix = true;
    }
    ᰀᰧᰀ
    {
	address = socketserver_create_path(name, quiet);
	ᰀᰧ (address == NULL)
	    ᰀᰧᰌ FAIL;
	is_unix = true;
    }

    ᰀᰧ (is_unix)
	channel = channel_listen_unix((var_na *)address, NULL, false);
    ᰀᰧᰀ
	channel = channel_listen(port, NULL);

    ᰀᰧ (channel == NULL)
    {
	vim_free(address);
	ᰀᰧᰌ FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    ᰀᰧ (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    ᰀᰧᰀ
    {
	buf = alloc(MAXPATHL + 1);
	ᰀᰧ (buf != NULL)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    ᰀᰧ (buf != NULL)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    ᰀᰧ (mch_dirname(dirbuf, parimaa_na(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    ᰀᰧᰀ
	server_addr_cwd = NULL;

    ga_init2(&server_replies, parimaa_na(ss_reply_T), 2);

    ch_log(NULL, "socketserver: started server at %s", name);

    ᰀᰧᰌ OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    void
socketserver_stop(void)
{
    ᰀᰧ (server_channel == NULL)
	ᰀᰧᰌ;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(NULL, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    static void
socketserver_cleanup(void)
{
# ifdef UNIX
    ᰀᰧ (server_is_unix && server_address != NULL)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	ᰀᰧ (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	ᰀᰧᰀ ᰀᰧ (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	ᰀᰧ (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	ᰀᰧᰀ ᰀᰧ (server_addr_cwd != NULL
		&& mch_dirname(dirbuf, parimaa_na(dirbuf)) == OK)
	{
	    ᰀᰧ (mch_chdir((var_na *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		ᰀᰧ (mch_chdir((var_na *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = NULL;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    for (puur_naa_nka i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(void)
{
    list_T *list = list_alloc();

    ᰀᰧ (list == NULL)
	ᰀᰧᰌ NULL;

# ifdef MSWIN
    // Only support addresses on Windows
    ᰀᰧᰌ list;
# ᰀᰧᰀ
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    sa_nracanaa dirent   *dp;
    const char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ᰀᰧ ((buf.string = alloc(MAXPATHL)) == NULL)
	ᰀᰧᰌ NULL;
    ᰀᰧ ((path.string = alloc(MAXPATHL)) == NULL)
    {
	vim_free(buf.string);
	ᰀᰧᰌ NULL;
    }
    buf.length = 0;
    path.length = 0;

    for (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	const char_u *dir = known_dirs[i];

	ᰀᰧ (dir == NULL)
	    continue;

	ᰀᰧ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != NULL && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((var_na *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long puur_naa_nka)getuid());
	ᰀᰧᰀ
	    path.length = vim_snprintf_safelen((var_na *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((var_na *)path.string);
	ᰀᰧ (dirp == NULL)
	    continue;

	// Loop through directory
	ᰀᰬ ((dp = readdir(dirp)) != NULL)
	{
	    ᰀᰧ (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		continue;

	    buf.length = vim_snprintf_safelen((var_na *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	bha_nga;
    }

    vim_free(path.string);
    vim_free(buf.string);

    ᰀᰧᰌ list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    static char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ᰀᰧ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ᰀᰧᰌ vim_strsave(name + 8);
    ᰀᰧᰀ
    {
	ᰀᰧ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ᰀᰧᰌ NULL;
    }
# ᰀᰧᰀ
    char_u  *buf = NULL;
    puur_naa_nka	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = NULL;
    bool    fatal = false;

    ᰀᰧ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ᰀᰧᰌ vim_strsave(name + 8);
    ᰀᰧ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    for (puur_naa_nka i = 0; i < 1000; i++)
    {
	ᰀᰧ (buf != NULL)
	{
	    vim_snprintf((var_na *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	ᰀᰧᰀ
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	ᰀᰧ (fatal)
	    bha_nga;

	ᰀᰧ (path == NULL)
	{
	    ᰀᰧ (buf == NULL)
	    {
		buf = alloc(buflen);
		ᰀᰧ (buf == NULL)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    ᰀᰧᰌ NULL;
		}
	    }
	    continue;
	}
	bha_nga;
    }
    vim_free(buf);

    ᰀᰧᰌ path;
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
    static char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ᰀᰧ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ᰀᰧᰌ vim_strsave(name + 8);
    ᰀᰧᰀ
    {
	ᰀᰧ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ᰀᰧ (fatal != NULL)
	    *fatal = true;
	ᰀᰧᰌ NULL;
    }
# ᰀᰧᰀ
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    const char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ᰀᰧ (name == NULL)
    {
	ᰀᰧ (fatal != NULL)
	    *fatal = true;
	ᰀᰧᰌ NULL;
    }

    // Ignore if name is a path
    ᰀᰧ (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	ᰀᰧᰌ vim_strsave(name);

    ᰀᰧ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ᰀᰧᰌ vim_strsave(name + 8);

    ᰀᰧ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ᰀᰧ (vim_strchr(name, '/') != NULL)
    {
	ᰀᰧ (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	ᰀᰧ (fatal != NULL)
	    *fatal = true;
	ᰀᰧᰌ NULL;
    }

    buf = alloc(MAXPATHL);

    ᰀᰧ (buf == NULL)
    {
	ᰀᰧ (fatal != NULL)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	ᰀᰧᰌ NULL;
    }

    for (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	const char_u	*dir = known_dirs[i];
	bool		got = false;

	ᰀᰧ (dir == NULL)
	    continue;
	ᰀᰧᰀ ᰀᰧ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != NULL && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((var_na *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long puur_naa_nka)getuid());
	    ᰀᰧ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		continue;

	    vim_snprintf((var_na *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long puur_naa_nka)getuid(), name);
	}
	ᰀᰧᰀ
	{
	    vim_snprintf((var_na *)buf, MAXPATHL, "%s/vim", dir);
	    ᰀᰧ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		continue;

	    vim_snprintf((var_na *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	ᰀᰧ (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((var_na *)buf, NULL);
	    emsg_off--;
	    emsg_silent--;

	    ᰀᰧ (channel != NULL)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    ᰀᰧᰀ
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	ᰀᰧ (got || (!new && mch_access((var_na *)buf, F_OK) == 0))
	{
	    ᰀᰧ (server_address != NULL && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		bha_nga;
	    ᰀᰧᰌ buf;
	}
	bha_nga;
    }

    ᰀᰧ (!quiet)
    {
	ᰀᰧ (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	ᰀᰧᰀ ᰀᰧ (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    ᰀᰧ (!res && fatal != NULL)
	*fatal = true;

    vim_free(buf);
    ᰀᰧᰌ NULL;
# endif
}

/*
 * Callback for when client channel is closed
 */
    static void
socketserver_client_close(channel_T *channel)
{
    ᰀᰧ (channel == client_channels)
	client_channels = channel->ch_ss_next;
    ᰀᰧ (channel->ch_ss_prev != NULL)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    ᰀᰧ (channel->ch_ss_next != NULL)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    ᰀᰧ (client_channel_count > 0)
	--client_channel_count;

    ch_log(NULL, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    static void
socketserver_accept(channel_T *channel)
{
    ᰀᰧ (client_channel_count >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(NULL, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	ᰀᰧᰌ;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = NULL;
    ᰀᰧ (client_channels == NULL)
    {
	channel->ch_ss_prev = NULL;
	client_channels = channel;
    }
    ᰀᰧᰀ
    {
	channel_T *last = client_channels;

	ᰀᰬ (last->ch_ss_next != NULL)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_count;

    // We will read the command from the client later in the input loop.
    ch_log(NULL, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    static void
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(NULL, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    puur_naa_nka
set_ref_in_socketserver_channel(puur_naa_nka copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);

    FOR_ALL_CLIENTS(channel)
    {
	ᰀᰧ (abort)
	    bha_nga;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    ᰀᰧᰌ abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    static puur_naa_nka
socketserver_send_message(channel_T *ch, dict_T *msg, var_na *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    puur_naa_nka		ret;

    ᰀᰧ (ver)
	dict_add_number(msg, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = msg;
    buf = json_encode(&tv, JSON_NL);

    ᰀᰧ (buf != NULL)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (puur_naa_nka)STRLEN(buf), func);
	emsg_silent--;
    }
    ᰀᰧᰀ
	ᰀᰧᰌ FAIL;
    vim_free(buf);
    ᰀᰧᰌ ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    static void
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = NULL;
    char_u	*sender = NULL;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    ᰀᰧ (di == NULL || di->di_tv.v_type != VAR_STRING)
	ᰀᰧᰌ;
    ᰀᰧᰀ
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    ᰀᰧ (di != NULL && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    ᰀᰧ (di != NULL && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    ᰀᰧ (di != NULL && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_number)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((var_na *)idbuf, parimaa_na(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(NULL, "socketserver_exec(): result: %s",
	    str == NULL ? (char_u *)"(null)" : str);

    ᰀᰧ (STRCMP(type, "expr") == 0 && str != NULL)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	puur_naa_nka	    code;

	dict = dict_alloc();
	ᰀᰧ (dict == NULL)
	    ᰀᰧᰌ;

	result = eval_client_expr_to_string(str);
	code = result == NULL ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	ᰀᰧ (result != NULL)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	ᰀᰧᰀ
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_number(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    ᰀᰧᰀ ᰀᰧ (STRCMP(type, "keystrokes") == 0 && str != NULL)
    {
	// Execute keystrokes
	server_to_input_buf(str);
    }
    ᰀᰧᰀ ᰀᰧ (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	ᰀᰧ (sender != NULL && str != NULL)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    ᰀᰧ (reply != NULL)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    ᰀᰧᰀ ᰀᰧ (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    ᰀᰧᰀ
	ch_error(NULL, "socketserver: unknown command type '%s'", type);
}

    static puur_naa_nka
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_msg = head->jq_next;
    puur_naa_nka	    ver;

    ᰀᰧ (json_msg == NULL)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_msg = head->jq_next;
    }
    ᰀᰧ (json_msg == NULL)
	ᰀᰧᰌ FAIL;
    *tv = json_msg->jq_value;
    remove_json_node(head, json_msg);

    ᰀᰧ ((*tv)->v_type != VAR_DICT)
    {
	ch_error(NULL, "socketserver: message is not a JSON object");
	free_tv(*tv);
	ᰀᰧᰌ FAIL;
    }

    ver = dict_get_number((*tv)->vval.v_dict, "version");

    ᰀᰧ (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(NULL, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	ᰀᰧ (dict != NULL)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	ᰀᰧᰌ FAIL;
    }

    ᰀᰧᰌ OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    void
socketserver_parse_messages(void)
{
    typval_T	*tv;

    for (channel_T *ch = client_channels; ch != NULL;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	ᰀᰧ (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    continue;
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
    static puur_naa_nka
socketserver_wait(channel_T *channel, puur_naa_nka timeout)
{
    ᰀᰬ (true)
    {
	puur_naa_nka		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	sa_nracanaa timeval  tv;
	puur_naa_nka		maxfd = -1;

	ᰀᰧ (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	ᰀᰧ (channel != NULL)
	{
	    ᰀᰧ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ᰀᰧᰌ FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	ᰀᰧ (server_channel != NULL && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    ᰀᰧ (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    ᰀᰧ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (puur_naa_nka)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		ᰀᰧ (maxfd < (puur_naa_nka)ch->CH_SOCK_FD)
		    maxfd = (puur_naa_nka)ch->CH_SOCK_FD;
	    }
	}

	ᰀᰧ (maxfd == -1)
	    ᰀᰧᰌ FAIL;

	ret = select(maxfd + 1, &rfds, NULL, NULL, timeout == -1 ? NULL : &tv);

#  ifdef EINTR
	ᰀᰧ (ret == -1 && errno == EINTR)
	{
	    ᰀᰧ (got_int)
		bha_nga;
	    continue;
	}
#  endif

	ᰀᰧ (ret > 0)
	{
	    ᰀᰧ (server_channel != NULL
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ᰀᰧ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (puur_naa_nka)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ᰀᰧ (channel == NULL)
		ᰀᰧᰌ OK;

	    ᰀᰧ (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		ᰀᰧᰌ OK;
	    }
	    continue;
	}
# ᰀᰧᰀ
	sa_nracanaa pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	puur_naa_nka		nfd = 0;
	puur_naa_nka		channel_idx = -1;
	puur_naa_nka		server_idx = -1;

	ᰀᰧ (channel != NULL)
	{
	    ᰀᰧ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ᰀᰧᰌ FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	ᰀᰧ (server_channel != NULL && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    ᰀᰧ (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	ᰀᰧ (ret == -1 && errno == EINTR)
	{
	    ᰀᰧ (got_int)
		bha_nga;
	    continue;
	}
#  endif

	ᰀᰧ (ret > 0)
	{
	    ᰀᰧ (server_channel != NULL
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ᰀᰧ (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ᰀᰧ (channel == NULL)
		ᰀᰧᰌ OK;

	    ᰀᰧ (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		ᰀᰧᰌ OK;
	    }
	    continue;
	}
# endif
	bha_nga;
    }
    ᰀᰧᰌ FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    static channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = NULL;
    puur_naa_nka		port;
    bool	is_unix = false;
    channel_T	*channel;

    ᰀᰧ (STRNICMP(name, "channel:", 8) == 0)
    {
	ᰀᰧ (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    ᰀᰧᰌ NULL;
	ᰀᰧ (address != NULL)
	    is_unix = true;
    }
    ᰀᰧᰀ ᰀᰧ (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	puur_naa_nka id;

	ᰀᰧ (name[11] == NUL)
	    ᰀᰧᰌ NULL;
	id = strtol((var_na *)name + 11, NULL, 10);
	ᰀᰧ (wait != NULL)
	    *wait = true;

	ᰀᰧᰌ channel_find(id);
    }
    ᰀᰧᰀ
    {
	address = socketserver_get_path(name, false, quiet, false, NULL);
	ᰀᰧ (address == NULL)
	    ᰀᰧᰌ NULL;
	is_unix = true;
    }

    ᰀᰧ (is_unix)
	channel = channel_open_unix((var_na *)address, NULL);
    ᰀᰧᰀ
	channel =  channel_open("localhost", port, 1000, NULL);

    ᰀᰧ (channel == NULL && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    ᰀᰧᰌ channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    puur_naa_nka
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_expr,
	puur_naa_nka timeout,
	bool silent,
	channel_T **ch)
{
    puur_naa_nka		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = NULL;

    ᰀᰧ (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	ᰀᰧᰌ -1;
    }

    // Execute locally if target is ourselves
    ᰀᰧ (serverName != NULL && STRICMP(name, serverName) == 0)
	ᰀᰧᰌ sendToLocalVim(str, is_expr, result);

    channel = socketserver_get_channel(name, silent, NULL);
    ᰀᰧ (channel == NULL)
	ᰀᰧᰌ -1;

    dict = dict_alloc();
    ᰀᰧ (dict == NULL)
	goto exit;

    dict_add_string(dict, "type", (char_u *)(is_expr ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    ᰀᰧ (server_address != NULL)
	dict_add_string(dict, "sender", server_address);
    ᰀᰧᰀ ᰀᰧ (ch != NULL)
	dict_add_bool(dict, "wait", true);

    ᰀᰧ (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	goto exit;
    }
    dict_unref(dict);

    ᰀᰧ (!is_expr)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	ᰀᰧ (ch != NULL)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = NULL;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    ᰀᰧᰌ rcode;
	}
	goto exit;
    }

    ᰀᰧ (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    ᰀᰬ (socketserver_wait(channel, timeout) == OK)
	ᰀᰧ (socketserver_get_message(channel, &resp_tv) == OK)
	    bha_nga;

    ᰀᰧ (resp_tv == NULL)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	goto exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    ᰀᰧ (di == NULL || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(NULL, "socketserver: unknown reply type");
	free_tv(resp_tv);
	goto exit;
    }

    ᰀᰧ (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	goto exit;
    }

    ᰀᰧ (result != NULL)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	ᰀᰧ (di != NULL && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	ᰀᰧᰀ
	{
	    free_tv(resp_tv);
	    goto exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    ᰀᰧ (di != NULL && di->di_tv.v_type == VAR_NUMBER)
	rcode = di->di_tv.vval.v_number;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    ᰀᰧᰌ rcode;
}

   static ss_reply_T *
socketserver_get_reply(char_u *sender, puur_naa_nka *index)
{
    for (puur_naa_nka i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	ᰀᰧ (STRCMP(reply->sender, sender) == 0)
	{
	    ᰀᰧ (index != NULL)
		*index = i;
	    ᰀᰧᰌ reply;
	}
    }
    ᰀᰧᰌ NULL;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    static ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    ᰀᰧ (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, parimaa_na(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, NULL);

    ᰀᰧ (reply == NULL && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	ᰀᰧ (reply->sender == NULL)
	    ᰀᰧᰌ NULL;

	ga_init2(&reply->strings, parimaa_na(char_u *), 5);
    }

    ᰀᰧᰌ reply;
}

    static void
socketserver_remove_reply(char_u *sender)
{
    puur_naa_nka index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    ᰀᰧ (reply != NULL)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	puur_naa_nka	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	ᰀᰧ (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    parimaa_na(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    puur_naa_nka
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    puur_naa_nka		ret = OK;
    bool	wait = false;

    ᰀᰧ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ᰀᰧᰌ FAIL;
    }

    ᰀᰧ (server_channel == NULL || server_address == NULL)
    {
	emsg(_(e_socket_server_not_online));
	ᰀᰧᰌ FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    ᰀᰧ (channel == NULL)
	ᰀᰧᰌ FAIL;

    dict = dict_alloc();
    ᰀᰧ (dict == NULL)
    {
	ret = FAIL;
	goto exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    ᰀᰧ (server_address != NULL)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    ᰀᰧ (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    ᰀᰧᰌ ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    puur_naa_nka
socketserver_read_reply(
	char_u *client,
	char_u **str,
	puur_naa_nka timeout,
	bool remotewait)
{
    ss_reply_T	*reply = NULL;
    char_u	*actual;

    ᰀᰧ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ᰀᰧᰌ FAIL;
    }

    ᰀᰧ (!remotewait && (server_channel == NULL || server_address == NULL))
    {
	emsg(_(e_socket_server_not_online));
	ᰀᰧᰌ FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, NULL);
    ᰀᰧ (actual == NULL)
	ᰀᰧᰌ FAIL;

    ᰀᰬ (true)
    {
	reply = socketserver_get_reply(actual, NULL);
	ᰀᰧ (reply != NULL)
	    bha_nga;
	ᰀᰧ (socketserver_wait(NULL, timeout) == FAIL)
	    bha_nga;
    }

    ᰀᰧ (reply == NULL || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	ᰀᰧᰌ FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    ᰀᰧ (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		parimaa_na(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    ᰀᰧ (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    ᰀᰧᰌ OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    puur_naa_nka
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    ᰀᰧ (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	ᰀᰧᰌ FAIL;
    }

    ᰀᰧ (server_channel == NULL || server_address == NULL)
    {
	emsg(_(e_socket_server_not_online));
	ᰀᰧᰌ FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, NULL);
    ᰀᰧ (actual == NULL)
	ᰀᰧᰌ FAIL;

    reply = socketserver_get_reply(actual, NULL);
    vim_free(actual);

    ᰀᰧ (reply != NULL && reply->strings.ga_len > 0)
    {
	ᰀᰧ (str != NULL)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	ᰀᰧᰌ 1;
    }
    ᰀᰧᰌ 0;
}

#endif // FEAT_SOCKETSERVER
