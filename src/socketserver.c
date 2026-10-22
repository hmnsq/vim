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

#ଅନ୍ତର୍ଭାଵ "vim.h"

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

ସ୍ଥିର channel_T    *server_channel = ଖାଲି;
ସ୍ଥିର char_u	    *server_address = ଖାଲି; // Includes "type:" prefix if any
ସ୍ଥିର bool	    server_is_unix = false;
ସ୍ଥିର char_u	    *server_addr_cwd = ଖାଲି; // CWD when server was started, used
					  // to handle relative file paths.
ସ୍ଥିର garray_T	    server_replies;

ସ୍ଥିର channel_T    *client_channels = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    client_channel_ଗଣନା = 0;

# define FOR_ALL_CLIENTS(ch) \
    ପାଇଁ (ch = client_channels; ch != ଖାଲି; ch = ch->ch_ss_next)

ସ୍ଥିର ଶୂନ୍ୟ socketserver_cleanup(ଶୂନ୍ୟ);
ସ୍ଥିର char_u *socketserver_create_path(char_u *name, bool quiet);
ସ୍ଥିର char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
ସ୍ଥିର ଶୂନ୍ୟ socketserver_accept(channel_T *channel);
ସ୍ଥିର ଶୂନ୍ୟ socketserver_close(channel_T *channel);
ସ୍ଥିର ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    ଯଦି (server_channel != ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ଯଦି (STRNICMP(name, "channel:", 8) == 0)
    {
	ଯଦି (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (address != ଖାଲି)
	    is_unix = true;
    }
    ଅନ୍ୟଥା
    {
	address = socketserver_create_path(name, quiet);
	ଯଦି (address == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	is_unix = true;
    }

    ଯଦି (is_unix)
	channel = channel_listen_unix((ଅକ୍ଷର *)address, ଖାଲି, false);
    ଅନ୍ୟଥା
	channel = channel_listen(port, ଖାଲି);

    ଯଦି (channel == ଖାଲି)
    {
	vim_free(address);
	ଫେରନ୍ତୁ FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    ଯଦି (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    ଅନ୍ୟଥା
    {
	buf = alloc(MAXPATHL + 1);
	ଯଦି (buf != ଖାଲି)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    ଯଦି (buf != ଖାଲି)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    ଯଦି (mch_dirname(dirbuf, ଆକାର(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    ଅନ୍ୟଥା
	server_addr_cwd = ଖାଲି;

    ga_init2(&server_replies, ଆକାର(ss_reply_T), 2);

    ch_log(ଖାଲି, "socketserver: started server at %s", name);

    ଫେରନ୍ତୁ OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    ଶୂନ୍ୟ
socketserver_stop(ଶୂନ୍ୟ)
{
    ଯଦି (server_channel == ଖାଲି)
	ଫେରନ୍ତୁ;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ଖାଲି, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
socketserver_cleanup(ଶୂନ୍ୟ)
{
# ifdef UNIX
    ଯଦି (server_is_unix && server_address != ଖାଲି)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	ଯଦି (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	ଅନ୍ୟଥା ଯଦି (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	ଯଦି (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	ଅନ୍ୟଥା ଯଦି (server_addr_cwd != ଖାଲି
		&& mch_dirname(dirbuf, ଆକାର(dirbuf)) == OK)
	{
	    ଯଦି (mch_chdir((ଅକ୍ଷର *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		ଯଦି (mch_chdir((ଅକ୍ଷର *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = ଖାଲି;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(ଶୂନ୍ୟ)
{
    list_T *list = list_alloc();

    ଯଦି (list == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

# ifdef MSWIN
    // Only support addresses on Windows
    ଫେରନ୍ତୁ list;
# ଅନ୍ୟଥା
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    ସଂରଚନା dirent   *dp;
    ଅଚଳ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ଯଦି ((buf.string = alloc(MAXPATHL)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି ((path.string = alloc(MAXPATHL)) == ଖାଲି)
    {
	vim_free(buf.string);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    buf.length = 0;
    path.length = 0;

    ପାଇଁ (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ଅଚଳ char_u *dir = known_dirs[i];

	ଯଦି (dir == ଖାଲି)
	    ଜାରି;

	ଯଦି (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ଖାଲି && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((ଅକ୍ଷର *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getuid());
	ଅନ୍ୟଥା
	    path.length = vim_snprintf_safelen((ଅକ୍ଷର *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((ଅକ୍ଷର *)path.string);
	ଯଦି (dirp == ଖାଲି)
	    ଜାରି;

	// Loop through directory
	ଯେତେବେଳେ ((dp = readdir(dirp)) != ଖାଲି)
	{
	    ଯଦି (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		ଜାରି;

	    buf.length = vim_snprintf_safelen((ଅକ୍ଷର *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	ଭାଙ୍ଗ;
    }

    vim_free(path.string);
    vim_free(buf.string);

    ଫେରନ୍ତୁ list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    ସ୍ଥିର char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ଯଦି (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ଫେରନ୍ତୁ vim_strsave(name + 8);
    ଅନ୍ୟଥା
    {
	ଯଦି (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ଫେରନ୍ତୁ ଖାଲି;
    }
# ଅନ୍ୟଥା
    char_u  *buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = ଖାଲି;
    bool    fatal = false;

    ଯଦି (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ଫେରନ୍ତୁ vim_strsave(name + 8);
    ଯଦି (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 1000; i++)
    {
	ଯଦି (buf != ଖାଲି)
	{
	    vim_snprintf((ଅକ୍ଷର *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	ଅନ୍ୟଥା
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	ଯଦି (fatal)
	    ଭାଙ୍ଗ;

	ଯଦି (path == ଖାଲି)
	{
	    ଯଦି (buf == ଖାଲି)
	    {
		buf = alloc(buflen);
		ଯଦି (buf == ଖାଲି)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    ଫେରନ୍ତୁ ଖାଲି;
		}
	    }
	    ଜାରି;
	}
	ଭାଙ୍ଗ;
    }
    vim_free(buf);

    ଫେରନ୍ତୁ path;
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
    ସ୍ଥିର char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ଯଦି (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ଫେରନ୍ତୁ vim_strsave(name + 8);
    ଅନ୍ୟଥା
    {
	ଯଦି (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ଯଦି (fatal != ଖାଲି)
	    *fatal = true;
	ଫେରନ୍ତୁ ଖାଲି;
    }
# ଅନ୍ୟଥା
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ଅଚଳ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ଯଦି (name == ଖାଲି)
    {
	ଯଦି (fatal != ଖାଲି)
	    *fatal = true;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Ignore if name is a path
    ଯଦି (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	ଫେରନ୍ତୁ vim_strsave(name);

    ଯଦି (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ଫେରନ୍ତୁ vim_strsave(name + 8);

    ଯଦି (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ଯଦି (vim_strchr(name, '/') != ଖାଲି)
    {
	ଯଦି (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	ଯଦି (fatal != ଖାଲି)
	    *fatal = true;
	ଫେରନ୍ତୁ ଖାଲି;
    }

    buf = alloc(MAXPATHL);

    ଯଦି (buf == ଖାଲି)
    {
	ଯଦି (fatal != ଖାଲି)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ପାଇଁ (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ଅଚଳ char_u	*dir = known_dirs[i];
	bool		got = false;

	ଯଦି (dir == ଖାଲି)
	    ଜାରି;
	ଅନ୍ୟଥା ଯଦି (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ଖାଲି && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((ଅକ୍ଷର *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getuid());
	    ଯଦି (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ଜାରି;

	    vim_snprintf((ଅକ୍ଷର *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long ପୂର୍ଣ୍ଣସଂଖ୍ୟା)getuid(), name);
	}
	ଅନ୍ୟଥା
	{
	    vim_snprintf((ଅକ୍ଷର *)buf, MAXPATHL, "%s/vim", dir);
	    ଯଦି (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ଜାରି;

	    vim_snprintf((ଅକ୍ଷର *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	ଯଦି (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((ଅକ୍ଷର *)buf, ଖାଲି);
	    emsg_off--;
	    emsg_silent--;

	    ଯଦି (channel != ଖାଲି)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    ଅନ୍ୟଥା
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	ଯଦି (got || (!new && mch_access((ଅକ୍ଷର *)buf, F_OK) == 0))
	{
	    ଯଦି (server_address != ଖାଲି && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		ଭାଙ୍ଗ;
	    ଫେରନ୍ତୁ buf;
	}
	ଭାଙ୍ଗ;
    }

    ଯଦି (!quiet)
    {
	ଯଦି (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	ଅନ୍ୟଥା ଯଦି (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    ଯଦି (!res && fatal != ଖାଲି)
	*fatal = true;

    vim_free(buf);
    ଫେରନ୍ତୁ ଖାଲି;
# endif
}

/*
 * Callback for when client channel is closed
 */
    ସ୍ଥିର ଶୂନ୍ୟ
socketserver_client_close(channel_T *channel)
{
    ଯଦି (channel == client_channels)
	client_channels = channel->ch_ss_next;
    ଯଦି (channel->ch_ss_prev != ଖାଲି)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    ଯଦି (channel->ch_ss_next != ଖାଲି)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    ଯଦି (client_channel_ଗଣନା > 0)
	--client_channel_ଗଣନା;

    ch_log(ଖାଲି, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
socketserver_accept(channel_T *channel)
{
    ଯଦି (client_channel_ଗଣନା >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(ଖାଲି, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	ଫେରନ୍ତୁ;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = ଖାଲି;
    ଯଦି (client_channels == ଖାଲି)
    {
	channel->ch_ss_prev = ଖାଲି;
	client_channels = channel;
    }
    ଅନ୍ୟଥା
    {
	channel_T *last = client_channels;

	ଯେତେବେଳେ (last->ch_ss_next != ଖାଲି)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_ଗଣନା;

    // We will read the command from the client later in the input loop.
    ch_log(ଖାଲି, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    ସ୍ଥିର ଶୂନ୍ୟ
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(ଖାଲି, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_socketserver_channel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);

    FOR_ALL_CLIENTS(channel)
    {
	ଯଦି (abort)
	    ଭାଙ୍ଗ;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
    }
    ଫେରନ୍ତୁ abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_send_message(channel_T *ch, dict_T *ସନ୍ଦେଶ, ଅକ୍ଷର *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (ver)
	dict_add_ସଂଖ୍ୟା(ସନ୍ଦେଶ, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = ସନ୍ଦେଶ;
    buf = json_encode(&tv, JSON_NL);

    ଯଦି (buf != ଖାଲି)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf), func);
	emsg_silent--;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FAIL;
    vim_free(buf);
    ଫେରନ୍ତୁ ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ଖାଲି;
    char_u	*sender = ଖାଲି;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    ଯଦି (di == ଖାଲି || di->di_tv.v_type != VAR_STRING)
	ଫେରନ୍ତୁ;
    ଅନ୍ୟଥା
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_ସଂଖ୍ୟା)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((ଅକ୍ଷର *)idbuf, ଆକାର(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ଖାଲି, "socketserver_exec(): result: %s",
	    str == ଖାଲି ? (char_u *)"(null)" : str);

    ଯଦି (STRCMP(type, "expr") == 0 && str != ଖାଲି)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    code;

	dict = dict_alloc();
	ଯଦି (dict == ଖାଲି)
	    ଫେରନ୍ତୁ;

	result = eval_client_ପଦସମଷ୍ଟି_to_string(str);
	code = result == ଖାଲି ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	ଯଦି (result != ଖାଲି)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	ଅନ୍ୟଥା
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_ସଂଖ୍ୟା(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(type, "keystrokes") == 0 && str != ଖାଲି)
    {
	// Execute keystrokes
	server_to_ନିବେଶ_buf(str);
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	ଯଦି (sender != ଖାଲି && str != ଖାଲି)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    ଯଦି (reply != ଖାଲି)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    ଅନ୍ୟଥା
	ch_error(ଖାଲି, "socketserver: unknown command type '%s'", type);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_ସନ୍ଦେଶ = head->jq_next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ver;

    ଯଦି (json_ସନ୍ଦେଶ == ଖାଲି)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_ସନ୍ଦେଶ = head->jq_next;
    }
    ଯଦି (json_ସନ୍ଦେଶ == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    *tv = json_ସନ୍ଦେଶ->jq_ମୂଲ୍ୟ;
    remove_json_ଗଣ୍ଠି(head, json_ସନ୍ଦେଶ);

    ଯଦି ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ଖାଲି, "socketserver: message is not a JSON object");
	free_tv(*tv);
	ଫେରନ୍ତୁ FAIL;
    }

    ver = dict_get_ସଂଖ୍ୟା((*tv)->vval.v_dict, "version");

    ଯଦି (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ଖାଲି, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	ଯଦି (dict != ଖାଲି)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    ଶୂନ୍ୟ
socketserver_parse_messages(ଶୂନ୍ୟ)
{
    typval_T	*tv;

    ପାଇଁ (channel_T *ch = client_channels; ch != ଖାଲି;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	ଯଦି (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    ଜାରି;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_wait(channel_T *channel, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout)
{
    ଯେତେବେଳେ (true)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	ସଂରଚନା timeval  tv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxfd = -1;

	ଯଦି (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	ଯଦି (channel != ଖାଲି)
	{
	    ଯଦି (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ଫେରନ୍ତୁ FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	ଯଦି (server_channel != ଖାଲି && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    ଯଦି (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    ଯଦି (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		ଯଦି (maxfd < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch->CH_SOCK_FD)
		    maxfd = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch->CH_SOCK_FD;
	    }
	}

	ଯଦି (maxfd == -1)
	    ଫେରନ୍ତୁ FAIL;

	ret = select(maxfd + 1, &rfds, ଖାଲି, ଖାଲି, timeout == -1 ? ଖାଲି : &tv);

#  ifdef EINTR
	ଯଦି (ret == -1 && errno == EINTR)
	{
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	    ଜାରି;
	}
#  endif

	ଯଦି (ret > 0)
	{
	    ଯଦି (server_channel != ଖାଲି
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ଯଦି (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ଯଦି (channel == ଖାଲି)
		ଫେରନ୍ତୁ OK;

	    ଯଦି (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		ଫେରନ୍ତୁ OK;
	    }
	    ଜାରି;
	}
# ଅନ୍ୟଥା
	ସଂରଚନା pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nfd = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		channel_idx = -1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		server_idx = -1;

	ଯଦି (channel != ଖାଲି)
	{
	    ଯଦି (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ଫେରନ୍ତୁ FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	ଯଦି (server_channel != ଖାଲି && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    ଯଦି (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	ଯଦି (ret == -1 && errno == EINTR)
	{
	    ଯଦି (got_int)
		ଭାଙ୍ଗ;
	    ଜାରି;
	}
#  endif

	ଯଦି (ret > 0)
	{
	    ଯଦି (server_channel != ଖାଲି
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ଯଦି (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ଯଦି (channel == ଖାଲି)
		ଫେରନ୍ତୁ OK;

	    ଯଦି (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		ଫେରନ୍ତୁ OK;
	    }
	    ଜାରି;
	}
# endif
	ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    ସ୍ଥିର channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		port;
    bool	is_unix = false;
    channel_T	*channel;

    ଯଦି (STRNICMP(name, "channel:", 8) == 0)
    {
	ଯଦି (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (address != ଖାଲି)
	    is_unix = true;
    }
    ଅନ୍ୟଥା ଯଦି (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା id;

	ଯଦି (name[11] == NUL)
	    ଫେରନ୍ତୁ ଖାଲି;
	id = strtol((ଅକ୍ଷର *)name + 11, ଖାଲି, 10);
	ଯଦି (wait != ଖାଲି)
	    *wait = true;

	ଫେରନ୍ତୁ channel_find(id);
    }
    ଅନ୍ୟଥା
    {
	address = socketserver_get_path(name, false, quiet, false, ଖାଲି);
	ଯଦି (address == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;
	is_unix = true;
    }

    ଯଦି (is_unix)
	channel = channel_open_unix((ଅକ୍ଷର *)address, ଖାଲି);
    ଅନ୍ୟଥା
	channel =  channel_open("localhost", port, 1000, ଖାଲି);

    ଯଦି (channel == ଖାଲି && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    ଫେରନ୍ତୁ channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_ପଦସମଷ୍ଟି,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout,
	bool silent,
	channel_T **ch)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ଖାଲି;

    ଯଦି (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	ଫେରନ୍ତୁ -1;
    }

    // Execute locally if target is ourselves
    ଯଦି (serverName != ଖାଲି && STRICMP(name, serverName) == 0)
	ଫେରନ୍ତୁ sendToLocalVim(str, is_ପଦସମଷ୍ଟି, result);

    channel = socketserver_get_channel(name, silent, ଖାଲି);
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    dict = dict_alloc();
    ଯଦି (dict == ଖାଲି)
	ଯାଅ exit;

    dict_add_string(dict, "type", (char_u *)(is_ପଦସମଷ୍ଟି ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    ଯଦି (server_address != ଖାଲି)
	dict_add_string(dict, "sender", server_address);
    ଅନ୍ୟଥା ଯଦି (ch != ଖାଲି)
	dict_add_bool(dict, "wait", true);

    ଯଦି (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	ଯାଅ exit;
    }
    dict_unref(dict);

    ଯଦି (!is_ପଦସମଷ୍ଟି)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	ଯଦି (ch != ଖାଲି)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = ଖାଲି;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    ଫେରନ୍ତୁ rcode;
	}
	ଯାଅ exit;
    }

    ଯଦି (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    ଯେତେବେଳେ (socketserver_wait(channel, timeout) == OK)
	ଯଦି (socketserver_get_message(channel, &resp_tv) == OK)
	    ଭାଙ୍ଗ;

    ଯଦି (resp_tv == ଖାଲି)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	ଯାଅ exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    ଯଦି (di == ଖାଲି || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ଖାଲି, "socketserver: unknown reply type");
	free_tv(resp_tv);
	ଯାଅ exit;
    }

    ଯଦି (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	ଯାଅ exit;
    }

    ଯଦି (result != ଖାଲି)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	ଅନ୍ୟଥା
	{
	    free_tv(resp_tv);
	    ଯାଅ exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_ସଂଖ୍ୟା)
	rcode = di->di_tv.vval.v_ସଂଖ୍ୟା;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    ଫେରନ୍ତୁ rcode;
}

   ସ୍ଥିର ss_reply_T *
socketserver_get_reply(char_u *sender, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *index)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	ଯଦି (STRCMP(reply->sender, sender) == 0)
	{
	    ଯଦି (index != ଖାଲି)
		*index = i;
	    ଫେରନ୍ତୁ reply;
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    ସ୍ଥିର ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    ଯଦି (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, ଆକାର(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, ଖାଲି);

    ଯଦି (reply == ଖାଲି && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	ଯଦି (reply->sender == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	ga_init2(&reply->strings, ଆକାର(char_u *), 5);
    }

    ଫେରନ୍ତୁ reply;
}

    ସ୍ଥିର ଶୂନ୍ୟ
socketserver_remove_reply(char_u *sender)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    ଯଦି (reply != ଖାଲି)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	ଯଦି (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    ଆକାର(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    bool	wait = false;

    ଯଦି (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (server_channel == ଖାଲି || server_address == ଖାଲି)
    {
	emsg(_(e_socket_server_not_online));
	ଫେରନ୍ତୁ FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    dict = dict_alloc();
    ଯଦି (dict == ଖାଲି)
    {
	ret = FAIL;
	ଯାଅ exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    ଯଦି (server_address != ଖାଲି)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    ଯଦି (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    ଫେରନ୍ତୁ ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_read_reply(
	char_u *client,
	char_u **str,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout,
	bool remotewait)
{
    ss_reply_T	*reply = ଖାଲି;
    char_u	*actual;

    ଯଦି (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (!remotewait && (server_channel == ଖାଲି || server_address == ଖାଲି))
    {
	emsg(_(e_socket_server_not_online));
	ଫେରନ୍ତୁ FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, ଖାଲି);
    ଯଦି (actual == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯେତେବେଳେ (true)
    {
	reply = socketserver_get_reply(actual, ଖାଲି);
	ଯଦି (reply != ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (socketserver_wait(ଖାଲି, timeout) == FAIL)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (reply == ଖାଲି || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	ଫେରନ୍ତୁ FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    ଯଦି (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		ଆକାର(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    ଯଦି (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    ଫେରନ୍ତୁ OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    ଯଦି (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (server_channel == ଖାଲି || server_address == ଖାଲି)
    {
	emsg(_(e_socket_server_not_online));
	ଫେରନ୍ତୁ FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, ଖାଲି);
    ଯଦି (actual == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    reply = socketserver_get_reply(actual, ଖାଲି);
    vim_free(actual);

    ଯଦି (reply != ଖାଲି && reply->strings.ga_len > 0)
    {
	ଯଦି (str != ଖାଲି)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	ଫେରନ୍ତୁ 1;
    }
    ଫେରନ୍ତୁ 0;
}

#endif // FEAT_SOCKETSERVER
