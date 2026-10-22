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
typedef struct
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
static ціле	    client_channel_count = 0;

# define FOR_ALL_CLIENTS(ch) \
    для (ch = client_channels; ch != NULL; ch = ch->ch_ss_next)

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
    ціле
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = NULL;
    ціле		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    якщо (server_channel != NULL)
	повернути OK;

    якщо (STRNICMP(name, "channel:", 8) == 0)
    {
	якщо (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    повернути FAIL;
	якщо (address != NULL)
	    is_unix = true;
    }
    інакше
    {
	address = socketserver_create_path(name, quiet);
	якщо (address == NULL)
	    повернути FAIL;
	is_unix = true;
    }

    якщо (is_unix)
	channel = channel_listen_unix((char *)address, NULL, false);
    інакше
	channel = channel_listen(port, NULL);

    якщо (channel == NULL)
    {
	vim_free(address);
	повернути FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    якщо (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    інакше
    {
	buf = alloc(MAXPATHL + 1);
	якщо (buf != NULL)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    якщо (buf != NULL)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    якщо (mch_dirname(dirbuf, sizeof(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    інакше
	server_addr_cwd = NULL;

    ga_init2(&server_replies, sizeof(ss_reply_T), 2);

    ch_log(NULL, "socketserver: started server at %s", name);

    повернути OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    void
socketserver_stop(void)
{
    якщо (server_channel == NULL)
	повернути;

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
    якщо (server_is_unix && server_address != NULL)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	якщо (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	інакше якщо (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	якщо (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	інакше якщо (server_addr_cwd != NULL
		&& mch_dirname(dirbuf, sizeof(dirbuf)) == OK)
	{
	    якщо (mch_chdir((char *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		якщо (mch_chdir((char *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = NULL;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    для (ціле i = 0; i < server_replies.ga_len; i++)
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

    якщо (list == NULL)
	повернути NULL;

# ifdef MSWIN
    // Only support addresses on Windows
    повернути list;
# інакше
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    struct dirent   *dp;
    const char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    якщо ((buf.string = alloc(MAXPATHL)) == NULL)
	повернути NULL;
    якщо ((path.string = alloc(MAXPATHL)) == NULL)
    {
	vim_free(buf.string);
	повернути NULL;
    }
    buf.length = 0;
    path.length = 0;

    для (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	const char_u *dir = known_dirs[i];

	якщо (dir == NULL)
	    continue;

	якщо (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != NULL && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((char *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long ціле)getuid());
	інакше
	    path.length = vim_snprintf_safelen((char *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((char *)path.string);
	якщо (dirp == NULL)
	    continue;

	// Loop through directory
	поки ((dp = readdir(dirp)) != NULL)
	{
	    якщо (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		continue;

	    buf.length = vim_snprintf_safelen((char *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	break;
    }

    vim_free(path.string);
    vim_free(buf.string);

    повернути list;
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
    якщо (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	повернути vim_strsave(name + 8);
    інакше
    {
	якщо (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	повернути NULL;
    }
# інакше
    char_u  *buf = NULL;
    ціле	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = NULL;
    bool    fatal = false;

    якщо (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	повернути vim_strsave(name + 8);
    якщо (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    для (ціле i = 0; i < 1000; i++)
    {
	якщо (buf != NULL)
	{
	    vim_snprintf((char *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	інакше
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	якщо (fatal)
	    break;

	якщо (path == NULL)
	{
	    якщо (buf == NULL)
	    {
		buf = alloc(buflen);
		якщо (buf == NULL)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    повернути NULL;
		}
	    }
	    continue;
	}
	break;
    }
    vim_free(buf);

    повернути path;
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
    якщо (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	повернути vim_strsave(name + 8);
    інакше
    {
	якщо (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	якщо (fatal != NULL)
	    *fatal = true;
	повернути NULL;
    }
# інакше
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    const char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    якщо (name == NULL)
    {
	якщо (fatal != NULL)
	    *fatal = true;
	повернути NULL;
    }

    // Ignore if name is a path
    якщо (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	повернути vim_strsave(name);

    якщо (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	повернути vim_strsave(name + 8);

    якщо (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    якщо (vim_strchr(name, '/') != NULL)
    {
	якщо (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	якщо (fatal != NULL)
	    *fatal = true;
	повернути NULL;
    }

    buf = alloc(MAXPATHL);

    якщо (buf == NULL)
    {
	якщо (fatal != NULL)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	повернути NULL;
    }

    для (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	const char_u	*dir = known_dirs[i];
	bool		got = false;

	якщо (dir == NULL)
	    continue;
	інакше якщо (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != NULL && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long ціле)getuid());
	    якщо (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		continue;

	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long ціле)getuid(), name);
	}
	інакше
	{
	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim", dir);
	    якщо (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		continue;

	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	якщо (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((char *)buf, NULL);
	    emsg_off--;
	    emsg_silent--;

	    якщо (channel != NULL)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    інакше
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	якщо (got || (!new && mch_access((char *)buf, F_OK) == 0))
	{
	    якщо (server_address != NULL && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		break;
	    повернути buf;
	}
	break;
    }

    якщо (!quiet)
    {
	якщо (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	інакше якщо (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    якщо (!res && fatal != NULL)
	*fatal = true;

    vim_free(buf);
    повернути NULL;
# endif
}

/*
 * Callback for when client channel is closed
 */
    static void
socketserver_client_close(channel_T *channel)
{
    якщо (channel == client_channels)
	client_channels = channel->ch_ss_next;
    якщо (channel->ch_ss_prev != NULL)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    якщо (channel->ch_ss_next != NULL)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    якщо (client_channel_count > 0)
	--client_channel_count;

    ch_log(NULL, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    static void
socketserver_accept(channel_T *channel)
{
    якщо (client_channel_count >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(NULL, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	повернути;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = NULL;
    якщо (client_channels == NULL)
    {
	channel->ch_ss_prev = NULL;
	client_channels = channel;
    }
    інакше
    {
	channel_T *last = client_channels;

	поки (last->ch_ss_next != NULL)
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
    ціле
set_ref_in_socketserver_channel(ціле copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);

    FOR_ALL_CLIENTS(channel)
    {
	якщо (abort)
	    break;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    повернути abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    static ціле
socketserver_send_message(channel_T *ch, dict_T *msg, char *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    ціле		ret;

    якщо (ver)
	dict_add_number(msg, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = msg;
    buf = json_encode(&tv, JSON_NL);

    якщо (buf != NULL)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (ціле)STRLEN(buf), func);
	emsg_silent--;
    }
    інакше
	повернути FAIL;
    vim_free(buf);
    повернути ret;
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
    якщо (di == NULL || di->di_tv.v_type != VAR_STRING)
	повернути;
    інакше
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    якщо (di != NULL && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    якщо (di != NULL && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    якщо (di != NULL && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_number)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((char *)idbuf, sizeof(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(NULL, "socketserver_exec(): result: %s",
	    str == NULL ? (char_u *)"(null)" : str);

    якщо (STRCMP(type, "expr") == 0 && str != NULL)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	ціле	    code;

	dict = dict_alloc();
	якщо (dict == NULL)
	    повернути;

	result = eval_client_expr_to_string(str);
	code = result == NULL ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	якщо (result != NULL)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	інакше
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_number(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    інакше якщо (STRCMP(type, "keystrokes") == 0 && str != NULL)
    {
	// Execute keystrokes
	server_to_input_buf(str);
    }
    інакше якщо (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	якщо (sender != NULL && str != NULL)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    якщо (reply != NULL)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    інакше якщо (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    інакше
	ch_error(NULL, "socketserver: unknown command type '%s'", type);
}

    static ціле
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_msg = head->jq_next;
    ціле	    ver;

    якщо (json_msg == NULL)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_msg = head->jq_next;
    }
    якщо (json_msg == NULL)
	повернути FAIL;
    *tv = json_msg->jq_value;
    remove_json_node(head, json_msg);

    якщо ((*tv)->v_type != VAR_DICT)
    {
	ch_error(NULL, "socketserver: message is not a JSON object");
	free_tv(*tv);
	повернути FAIL;
    }

    ver = dict_get_number((*tv)->vval.v_dict, "version");

    якщо (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(NULL, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	якщо (dict != NULL)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	повернути FAIL;
    }

    повернути OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    void
socketserver_parse_messages(void)
{
    typval_T	*tv;

    для (channel_T *ch = client_channels; ch != NULL;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	якщо (socketserver_get_message(ch, &tv) == FAIL)
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
    static ціле
socketserver_wait(channel_T *channel, ціле timeout)
{
    поки (true)
    {
	ціле		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	struct timeval  tv;
	ціле		maxfd = -1;

	якщо (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	якщо (channel != NULL)
	{
	    якщо (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		повернути FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	якщо (server_channel != NULL && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    якщо (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    якщо (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (ціле)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		якщо (maxfd < (ціле)ch->CH_SOCK_FD)
		    maxfd = (ціле)ch->CH_SOCK_FD;
	    }
	}

	якщо (maxfd == -1)
	    повернути FAIL;

	ret = select(maxfd + 1, &rfds, NULL, NULL, timeout == -1 ? NULL : &tv);

#  ifdef EINTR
	якщо (ret == -1 && errno == EINTR)
	{
	    якщо (got_int)
		break;
	    continue;
	}
#  endif

	якщо (ret > 0)
	{
	    якщо (server_channel != NULL
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		якщо (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (ціле)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    якщо (channel == NULL)
		повернути OK;

	    якщо (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		повернути OK;
	    }
	    continue;
	}
# інакше
	struct pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	ціле		nfd = 0;
	ціле		channel_idx = -1;
	ціле		server_idx = -1;

	якщо (channel != NULL)
	{
	    якщо (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		повернути FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	якщо (server_channel != NULL && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    якщо (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	якщо (ret == -1 && errno == EINTR)
	{
	    якщо (got_int)
		break;
	    continue;
	}
#  endif

	якщо (ret > 0)
	{
	    якщо (server_channel != NULL
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		якщо (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    якщо (channel == NULL)
		повернути OK;

	    якщо (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		повернути OK;
	    }
	    continue;
	}
# endif
	break;
    }
    повернути FAIL;
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
    ціле		port;
    bool	is_unix = false;
    channel_T	*channel;

    якщо (STRNICMP(name, "channel:", 8) == 0)
    {
	якщо (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    повернути NULL;
	якщо (address != NULL)
	    is_unix = true;
    }
    інакше якщо (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	ціле id;

	якщо (name[11] == NUL)
	    повернути NULL;
	id = strtol((char *)name + 11, NULL, 10);
	якщо (wait != NULL)
	    *wait = true;

	повернути channel_find(id);
    }
    інакше
    {
	address = socketserver_get_path(name, false, quiet, false, NULL);
	якщо (address == NULL)
	    повернути NULL;
	is_unix = true;
    }

    якщо (is_unix)
	channel = channel_open_unix((char *)address, NULL);
    інакше
	channel =  channel_open("localhost", port, 1000, NULL);

    якщо (channel == NULL && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    повернути channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    ціле
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_expr,
	ціле timeout,
	bool silent,
	channel_T **ch)
{
    ціле		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = NULL;

    якщо (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	повернути -1;
    }

    // Execute locally if target is ourselves
    якщо (serverName != NULL && STRICMP(name, serverName) == 0)
	повернути sendToLocalVim(str, is_expr, result);

    channel = socketserver_get_channel(name, silent, NULL);
    якщо (channel == NULL)
	повернути -1;

    dict = dict_alloc();
    якщо (dict == NULL)
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
    якщо (server_address != NULL)
	dict_add_string(dict, "sender", server_address);
    інакше якщо (ch != NULL)
	dict_add_bool(dict, "wait", true);

    якщо (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	goto exit;
    }
    dict_unref(dict);

    якщо (!is_expr)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	якщо (ch != NULL)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = NULL;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    повернути rcode;
	}
	goto exit;
    }

    якщо (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    поки (socketserver_wait(channel, timeout) == OK)
	якщо (socketserver_get_message(channel, &resp_tv) == OK)
	    break;

    якщо (resp_tv == NULL)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	goto exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    якщо (di == NULL || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(NULL, "socketserver: unknown reply type");
	free_tv(resp_tv);
	goto exit;
    }

    якщо (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	goto exit;
    }

    якщо (result != NULL)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	якщо (di != NULL && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	інакше
	{
	    free_tv(resp_tv);
	    goto exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    якщо (di != NULL && di->di_tv.v_type == VAR_NUMBER)
	rcode = di->di_tv.vval.v_number;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    повернути rcode;
}

   static ss_reply_T *
socketserver_get_reply(char_u *sender, ціле *index)
{
    для (ціле i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	якщо (STRCMP(reply->sender, sender) == 0)
	{
	    якщо (index != NULL)
		*index = i;
	    повернути reply;
	}
    }
    повернути NULL;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    static ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    якщо (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, sizeof(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, NULL);

    якщо (reply == NULL && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	якщо (reply->sender == NULL)
	    повернути NULL;

	ga_init2(&reply->strings, sizeof(char_u *), 5);
    }

    повернути reply;
}

    static void
socketserver_remove_reply(char_u *sender)
{
    ціле index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    якщо (reply != NULL)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	ціле	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	якщо (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    sizeof(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    ціле
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    ціле		ret = OK;
    bool	wait = false;

    якщо (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	повернути FAIL;
    }

    якщо (server_channel == NULL || server_address == NULL)
    {
	emsg(_(e_socket_server_not_online));
	повернути FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    якщо (channel == NULL)
	повернути FAIL;

    dict = dict_alloc();
    якщо (dict == NULL)
    {
	ret = FAIL;
	goto exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    якщо (server_address != NULL)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    якщо (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    повернути ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    ціле
socketserver_read_reply(
	char_u *client,
	char_u **str,
	ціле timeout,
	bool remotewait)
{
    ss_reply_T	*reply = NULL;
    char_u	*actual;

    якщо (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	повернути FAIL;
    }

    якщо (!remotewait && (server_channel == NULL || server_address == NULL))
    {
	emsg(_(e_socket_server_not_online));
	повернути FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, NULL);
    якщо (actual == NULL)
	повернути FAIL;

    поки (true)
    {
	reply = socketserver_get_reply(actual, NULL);
	якщо (reply != NULL)
	    break;
	якщо (socketserver_wait(NULL, timeout) == FAIL)
	    break;
    }

    якщо (reply == NULL || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	повернути FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    якщо (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		sizeof(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    якщо (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    повернути OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    ціле
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    якщо (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	повернути FAIL;
    }

    якщо (server_channel == NULL || server_address == NULL)
    {
	emsg(_(e_socket_server_not_online));
	повернути FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, NULL);
    якщо (actual == NULL)
	повернути FAIL;

    reply = socketserver_get_reply(actual, NULL);
    vim_free(actual);

    якщо (reply != NULL && reply->strings.ga_len > 0)
    {
	якщо (str != NULL)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	повернути 1;
    }
    повернути 0;
}

#endif // FEAT_SOCKETSERVER
