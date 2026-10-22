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
static số_nguyên	    client_channel_count = 0;

# define FOR_ALL_CLIENTS(ch) \
    cho (ch = client_channels; ch != NULL; ch = ch->ch_ss_next)

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
    số_nguyên
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = NULL;
    số_nguyên		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    nếu (server_channel != NULL)
	trả_về OK;

    nếu (STRNICMP(name, "channel:", 8) == 0)
    {
	nếu (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    trả_về FAIL;
	nếu (address != NULL)
	    is_unix = true;
    }
    không_thì
    {
	address = socketserver_create_path(name, quiet);
	nếu (address == NULL)
	    trả_về FAIL;
	is_unix = true;
    }

    nếu (is_unix)
	channel = channel_listen_unix((char *)address, NULL, false);
    không_thì
	channel = channel_listen(port, NULL);

    nếu (channel == NULL)
    {
	vim_free(address);
	trả_về FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    nếu (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    không_thì
    {
	buf = alloc(MAXPATHL + 1);
	nếu (buf != NULL)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    nếu (buf != NULL)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    nếu (mch_dirname(dirbuf, sizeof(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    không_thì
	server_addr_cwd = NULL;

    ga_init2(&server_replies, sizeof(ss_reply_T), 2);

    ch_log(NULL, "socketserver: started server at %s", name);

    trả_về OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    void
socketserver_stop(void)
{
    nếu (server_channel == NULL)
	trả_về;

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
    nếu (server_is_unix && server_address != NULL)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	nếu (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	không_thì nếu (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	nếu (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	không_thì nếu (server_addr_cwd != NULL
		&& mch_dirname(dirbuf, sizeof(dirbuf)) == OK)
	{
	    nếu (mch_chdir((char *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		nếu (mch_chdir((char *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = NULL;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    cho (số_nguyên i = 0; i < server_replies.ga_len; i++)
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

    nếu (list == NULL)
	trả_về NULL;

# ifdef MSWIN
    // Only support addresses on Windows
    trả_về list;
# không_thì
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    struct dirent   *dp;
    const char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    nếu ((buf.string = alloc(MAXPATHL)) == NULL)
	trả_về NULL;
    nếu ((path.string = alloc(MAXPATHL)) == NULL)
    {
	vim_free(buf.string);
	trả_về NULL;
    }
    buf.length = 0;
    path.length = 0;

    cho (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	const char_u *dir = known_dirs[i];

	nếu (dir == NULL)
	    continue;

	nếu (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != NULL && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((char *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long số_nguyên)getuid());
	không_thì
	    path.length = vim_snprintf_safelen((char *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((char *)path.string);
	nếu (dirp == NULL)
	    continue;

	// Loop through directory
	trong_khi ((dp = readdir(dirp)) != NULL)
	{
	    nếu (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
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

    trả_về list;
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
    nếu (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	trả_về vim_strsave(name + 8);
    không_thì
    {
	nếu (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	trả_về NULL;
    }
# không_thì
    char_u  *buf = NULL;
    số_nguyên	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = NULL;
    bool    fatal = false;

    nếu (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	trả_về vim_strsave(name + 8);
    nếu (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    cho (số_nguyên i = 0; i < 1000; i++)
    {
	nếu (buf != NULL)
	{
	    vim_snprintf((char *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	không_thì
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	nếu (fatal)
	    break;

	nếu (path == NULL)
	{
	    nếu (buf == NULL)
	    {
		buf = alloc(buflen);
		nếu (buf == NULL)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    trả_về NULL;
		}
	    }
	    continue;
	}
	break;
    }
    vim_free(buf);

    trả_về path;
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
    nếu (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	trả_về vim_strsave(name + 8);
    không_thì
    {
	nếu (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	nếu (fatal != NULL)
	    *fatal = true;
	trả_về NULL;
    }
# không_thì
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    const char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    nếu (name == NULL)
    {
	nếu (fatal != NULL)
	    *fatal = true;
	trả_về NULL;
    }

    // Ignore if name is a path
    nếu (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	trả_về vim_strsave(name);

    nếu (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	trả_về vim_strsave(name + 8);

    nếu (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    nếu (vim_strchr(name, '/') != NULL)
    {
	nếu (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	nếu (fatal != NULL)
	    *fatal = true;
	trả_về NULL;
    }

    buf = alloc(MAXPATHL);

    nếu (buf == NULL)
    {
	nếu (fatal != NULL)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	trả_về NULL;
    }

    cho (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	const char_u	*dir = known_dirs[i];
	bool		got = false;

	nếu (dir == NULL)
	    continue;
	không_thì nếu (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != NULL && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long số_nguyên)getuid());
	    nếu (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		continue;

	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long số_nguyên)getuid(), name);
	}
	không_thì
	{
	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim", dir);
	    nếu (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		continue;

	    vim_snprintf((char *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	nếu (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((char *)buf, NULL);
	    emsg_off--;
	    emsg_silent--;

	    nếu (channel != NULL)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    không_thì
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	nếu (got || (!new && mch_access((char *)buf, F_OK) == 0))
	{
	    nếu (server_address != NULL && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		break;
	    trả_về buf;
	}
	break;
    }

    nếu (!quiet)
    {
	nếu (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	không_thì nếu (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    nếu (!res && fatal != NULL)
	*fatal = true;

    vim_free(buf);
    trả_về NULL;
# endif
}

/*
 * Callback for when client channel is closed
 */
    static void
socketserver_client_close(channel_T *channel)
{
    nếu (channel == client_channels)
	client_channels = channel->ch_ss_next;
    nếu (channel->ch_ss_prev != NULL)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    nếu (channel->ch_ss_next != NULL)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    nếu (client_channel_count > 0)
	--client_channel_count;

    ch_log(NULL, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    static void
socketserver_accept(channel_T *channel)
{
    nếu (client_channel_count >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(NULL, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	trả_về;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = NULL;
    nếu (client_channels == NULL)
    {
	channel->ch_ss_prev = NULL;
	client_channels = channel;
    }
    không_thì
    {
	channel_T *last = client_channels;

	trong_khi (last->ch_ss_next != NULL)
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
    số_nguyên
set_ref_in_socketserver_channel(số_nguyên copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);

    FOR_ALL_CLIENTS(channel)
    {
	nếu (abort)
	    break;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
    }
    trả_về abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    static số_nguyên
socketserver_send_message(channel_T *ch, dict_T *msg, char *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    số_nguyên		ret;

    nếu (ver)
	dict_add_number(msg, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = msg;
    buf = json_encode(&tv, JSON_NL);

    nếu (buf != NULL)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (số_nguyên)STRLEN(buf), func);
	emsg_silent--;
    }
    không_thì
	trả_về FAIL;
    vim_free(buf);
    trả_về ret;
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
    nếu (di == NULL || di->di_tv.v_type != VAR_STRING)
	trả_về;
    không_thì
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    nếu (di != NULL && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    nếu (di != NULL && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    nếu (di != NULL && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_number)
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

    nếu (STRCMP(type, "expr") == 0 && str != NULL)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	số_nguyên	    code;

	dict = dict_alloc();
	nếu (dict == NULL)
	    trả_về;

	result = eval_client_expr_to_string(str);
	code = result == NULL ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	nếu (result != NULL)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	không_thì
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_number(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    không_thì nếu (STRCMP(type, "keystrokes") == 0 && str != NULL)
    {
	// Execute keystrokes
	server_to_input_buf(str);
    }
    không_thì nếu (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	nếu (sender != NULL && str != NULL)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    nếu (reply != NULL)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    không_thì nếu (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    không_thì
	ch_error(NULL, "socketserver: unknown command type '%s'", type);
}

    static số_nguyên
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_msg = head->jq_next;
    số_nguyên	    ver;

    nếu (json_msg == NULL)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_msg = head->jq_next;
    }
    nếu (json_msg == NULL)
	trả_về FAIL;
    *tv = json_msg->jq_value;
    remove_json_node(head, json_msg);

    nếu ((*tv)->v_type != VAR_DICT)
    {
	ch_error(NULL, "socketserver: message is not a JSON object");
	free_tv(*tv);
	trả_về FAIL;
    }

    ver = dict_get_number((*tv)->vval.v_dict, "version");

    nếu (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(NULL, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	nếu (dict != NULL)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	trả_về FAIL;
    }

    trả_về OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    void
socketserver_parse_messages(void)
{
    typval_T	*tv;

    cho (channel_T *ch = client_channels; ch != NULL;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	nếu (socketserver_get_message(ch, &tv) == FAIL)
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
    static số_nguyên
socketserver_wait(channel_T *channel, số_nguyên timeout)
{
    trong_khi (true)
    {
	số_nguyên		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	struct timeval  tv;
	số_nguyên		maxfd = -1;

	nếu (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	nếu (channel != NULL)
	{
	    nếu (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		trả_về FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	nếu (server_channel != NULL && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    nếu (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    nếu (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (số_nguyên)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		nếu (maxfd < (số_nguyên)ch->CH_SOCK_FD)
		    maxfd = (số_nguyên)ch->CH_SOCK_FD;
	    }
	}

	nếu (maxfd == -1)
	    trả_về FAIL;

	ret = select(maxfd + 1, &rfds, NULL, NULL, timeout == -1 ? NULL : &tv);

#  ifdef EINTR
	nếu (ret == -1 && errno == EINTR)
	{
	    nếu (got_int)
		break;
	    continue;
	}
#  endif

	nếu (ret > 0)
	{
	    nếu (server_channel != NULL
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		nếu (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (số_nguyên)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    nếu (channel == NULL)
		trả_về OK;

	    nếu (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		trả_về OK;
	    }
	    continue;
	}
# không_thì
	struct pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	số_nguyên		nfd = 0;
	số_nguyên		channel_idx = -1;
	số_nguyên		server_idx = -1;

	nếu (channel != NULL)
	{
	    nếu (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		trả_về FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	nếu (server_channel != NULL && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    nếu (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	nếu (ret == -1 && errno == EINTR)
	{
	    nếu (got_int)
		break;
	    continue;
	}
#  endif

	nếu (ret > 0)
	{
	    nếu (server_channel != NULL
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		nếu (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    nếu (channel == NULL)
		trả_về OK;

	    nếu (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		trả_về OK;
	    }
	    continue;
	}
# endif
	break;
    }
    trả_về FAIL;
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
    số_nguyên		port;
    bool	is_unix = false;
    channel_T	*channel;

    nếu (STRNICMP(name, "channel:", 8) == 0)
    {
	nếu (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    trả_về NULL;
	nếu (address != NULL)
	    is_unix = true;
    }
    không_thì nếu (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	số_nguyên id;

	nếu (name[11] == NUL)
	    trả_về NULL;
	id = strtol((char *)name + 11, NULL, 10);
	nếu (wait != NULL)
	    *wait = true;

	trả_về channel_find(id);
    }
    không_thì
    {
	address = socketserver_get_path(name, false, quiet, false, NULL);
	nếu (address == NULL)
	    trả_về NULL;
	is_unix = true;
    }

    nếu (is_unix)
	channel = channel_open_unix((char *)address, NULL);
    không_thì
	channel =  channel_open("localhost", port, 1000, NULL);

    nếu (channel == NULL && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    trả_về channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    số_nguyên
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_expr,
	số_nguyên timeout,
	bool silent,
	channel_T **ch)
{
    số_nguyên		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = NULL;

    nếu (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	trả_về -1;
    }

    // Execute locally if target is ourselves
    nếu (serverName != NULL && STRICMP(name, serverName) == 0)
	trả_về sendToLocalVim(str, is_expr, result);

    channel = socketserver_get_channel(name, silent, NULL);
    nếu (channel == NULL)
	trả_về -1;

    dict = dict_alloc();
    nếu (dict == NULL)
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
    nếu (server_address != NULL)
	dict_add_string(dict, "sender", server_address);
    không_thì nếu (ch != NULL)
	dict_add_bool(dict, "wait", true);

    nếu (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	goto exit;
    }
    dict_unref(dict);

    nếu (!is_expr)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	nếu (ch != NULL)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = NULL;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    trả_về rcode;
	}
	goto exit;
    }

    nếu (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    trong_khi (socketserver_wait(channel, timeout) == OK)
	nếu (socketserver_get_message(channel, &resp_tv) == OK)
	    break;

    nếu (resp_tv == NULL)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	goto exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    nếu (di == NULL || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(NULL, "socketserver: unknown reply type");
	free_tv(resp_tv);
	goto exit;
    }

    nếu (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	goto exit;
    }

    nếu (result != NULL)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	nếu (di != NULL && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	không_thì
	{
	    free_tv(resp_tv);
	    goto exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    nếu (di != NULL && di->di_tv.v_type == VAR_NUMBER)
	rcode = di->di_tv.vval.v_number;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    trả_về rcode;
}

   static ss_reply_T *
socketserver_get_reply(char_u *sender, số_nguyên *index)
{
    cho (số_nguyên i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	nếu (STRCMP(reply->sender, sender) == 0)
	{
	    nếu (index != NULL)
		*index = i;
	    trả_về reply;
	}
    }
    trả_về NULL;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    static ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    nếu (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, sizeof(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, NULL);

    nếu (reply == NULL && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	nếu (reply->sender == NULL)
	    trả_về NULL;

	ga_init2(&reply->strings, sizeof(char_u *), 5);
    }

    trả_về reply;
}

    static void
socketserver_remove_reply(char_u *sender)
{
    số_nguyên index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    nếu (reply != NULL)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	số_nguyên	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	nếu (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    sizeof(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    số_nguyên
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    số_nguyên		ret = OK;
    bool	wait = false;

    nếu (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	trả_về FAIL;
    }

    nếu (server_channel == NULL || server_address == NULL)
    {
	emsg(_(e_socket_server_not_online));
	trả_về FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    nếu (channel == NULL)
	trả_về FAIL;

    dict = dict_alloc();
    nếu (dict == NULL)
    {
	ret = FAIL;
	goto exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    nếu (server_address != NULL)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    nếu (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    trả_về ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    số_nguyên
socketserver_read_reply(
	char_u *client,
	char_u **str,
	số_nguyên timeout,
	bool remotewait)
{
    ss_reply_T	*reply = NULL;
    char_u	*actual;

    nếu (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	trả_về FAIL;
    }

    nếu (!remotewait && (server_channel == NULL || server_address == NULL))
    {
	emsg(_(e_socket_server_not_online));
	trả_về FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, NULL);
    nếu (actual == NULL)
	trả_về FAIL;

    trong_khi (true)
    {
	reply = socketserver_get_reply(actual, NULL);
	nếu (reply != NULL)
	    break;
	nếu (socketserver_wait(NULL, timeout) == FAIL)
	    break;
    }

    nếu (reply == NULL || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	trả_về FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    nếu (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		sizeof(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    nếu (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    trả_về OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    số_nguyên
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    nếu (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	trả_về FAIL;
    }

    nếu (server_channel == NULL || server_address == NULL)
    {
	emsg(_(e_socket_server_not_online));
	trả_về FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, NULL);
    nếu (actual == NULL)
	trả_về FAIL;

    reply = socketserver_get_reply(actual, NULL);
    vim_free(actual);

    nếu (reply != NULL && reply->strings.ga_len > 0)
    {
	nếu (str != NULL)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	trả_về 1;
    }
    trả_về 0;
}

#endif // FEAT_SOCKETSERVER
