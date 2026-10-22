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

#ಅನ್ತರ್ಭಾವ "vim.h"

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

ಸ್ಥಿರ channel_T    *server_channel = ರಿಕ್ತ;
ಸ್ಥಿರ char_u	    *server_address = ರಿಕ್ತ; // Includes "type:" prefix if any
ಸ್ಥಿರ bool	    server_is_unix = false;
ಸ್ಥಿರ char_u	    *server_addr_cwd = ರಿಕ್ತ; // CWD when server was started, used
					  // to handle relative file paths.
ಸ್ಥಿರ garray_T	    server_replies;

ಸ್ಥಿರ channel_T    *client_channels = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    client_channel_ಎಣಿಕೆ = 0;

# define FOR_ALL_CLIENTS(ch) \
    ಪ್ರತಿ (ch = client_channels; ch != ರಿಕ್ತ; ch = ch->ch_ss_next)

ಸ್ಥಿರ ಶೂನ್ಯ socketserver_cleanup(ಶೂನ್ಯ);
ಸ್ಥಿರ char_u *socketserver_create_path(char_u *name, bool quiet);
ಸ್ಥಿರ char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
ಸ್ಥಿರ ಶೂನ್ಯ socketserver_accept(channel_T *channel);
ಸ್ಥಿರ ಶೂನ್ಯ socketserver_close(channel_T *channel);
ಸ್ಥಿರ ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    ಯದಿ (server_channel != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (STRNICMP(name, "channel:", 8) == 0)
    {
	ಯದಿ (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (address != ರಿಕ್ತ)
	    is_unix = true;
    }
    ಅನ್ಯಥಾ
    {
	address = socketserver_create_path(name, quiet);
	ಯದಿ (address == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	is_unix = true;
    }

    ಯದಿ (is_unix)
	channel = channel_listen_unix((ವರ್ಣ *)address, ರಿಕ್ತ, false);
    ಅನ್ಯಥಾ
	channel = channel_listen(port, ರಿಕ್ತ);

    ಯದಿ (channel == ರಿಕ್ತ)
    {
	vim_free(address);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    ಯದಿ (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    ಅನ್ಯಥಾ
    {
	buf = alloc(MAXPATHL + 1);
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    ಯದಿ (buf != ರಿಕ್ತ)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    ಯದಿ (mch_dirname(dirbuf, ಪರಿಮಾಣ(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    ಅನ್ಯಥಾ
	server_addr_cwd = ರಿಕ್ತ;

    ga_init2(&server_replies, ಪರಿಮಾಣ(ss_reply_T), 2);

    ch_log(ರಿಕ್ತ, "socketserver: started server at %s", name);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    ಶೂನ್ಯ
socketserver_stop(ಶೂನ್ಯ)
{
    ಯದಿ (server_channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ರಿಕ್ತ, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_cleanup(ಶೂನ್ಯ)
{
# ifdef UNIX
    ಯದಿ (server_is_unix && server_address != ರಿಕ್ತ)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	ಯದಿ (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	ಅನ್ಯಥಾ ಯದಿ (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	ಯದಿ (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	ಅನ್ಯಥಾ ಯದಿ (server_addr_cwd != ರಿಕ್ತ
		&& mch_dirname(dirbuf, ಪರಿಮಾಣ(dirbuf)) == OK)
	{
	    ಯದಿ (mch_chdir((ವರ್ಣ *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		ಯದಿ (mch_chdir((ವರ್ಣ *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = ರಿಕ್ತ;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(ಶೂನ್ಯ)
{
    list_T *list = list_alloc();

    ಯದಿ (list == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

# ifdef MSWIN
    // Only support addresses on Windows
    ಪ್ರತ್ಯಾಗಮ list;
# ಅನ್ಯಥಾ
    string_T	    buf;
    string_T	    path;
    DIR		    *dirp;
    ಸಂರಚನಾ dirent   *dp;
    ಧ್ರುವ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ಯದಿ ((buf.string = alloc(MAXPATHL)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ ((path.string = alloc(MAXPATHL)) == ರಿಕ್ತ)
    {
	vim_free(buf.string);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    buf.length = 0;
    path.length = 0;

    ಪ್ರತಿ (size_t i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ಧ್ರುವ char_u *dir = known_dirs[i];

	ಯದಿ (dir == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	ಯದಿ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ರಿಕ್ತ && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((ವರ್ಣ *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (unsigned long ಪೂರ್ಣಾಙ್ಕ)getuid());
	ಅನ್ಯಥಾ
	    path.length = vim_snprintf_safelen((ವರ್ಣ *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((ವರ್ಣ *)path.string);
	ಯದಿ (dirp == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	// Loop through directory
	ಯಾವತ್ ((dp = readdir(dirp)) != ರಿಕ್ತ)
	{
	    ಯದಿ (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		ಅನುವರ್ತ;

	    buf.length = vim_snprintf_safelen((ವರ್ಣ *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	closedir(dirp);

	ಭಙ್ಗ;
    }

    vim_free(path.string);
    vim_free(buf.string);

    ಪ್ರತ್ಯಾಗಮ list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    ಸ್ಥಿರ char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ಯದಿ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(name + 8);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
# ಅನ್ಯಥಾ
    char_u  *buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = ರಿಕ್ತ;
    bool    fatal = false;

    ಯದಿ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(name + 8);
    ಯದಿ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < 1000; i++)
    {
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    vim_snprintf((ವರ್ಣ *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	ಅನ್ಯಥಾ
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	ಯದಿ (fatal)
	    ಭಙ್ಗ;

	ಯದಿ (path == ರಿಕ್ತ)
	{
	    ಯದಿ (buf == ರಿಕ್ತ)
	    {
		buf = alloc(buflen);
		ಯದಿ (buf == ರಿಕ್ತ)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
		}
	    }
	    ಅನುವರ್ತ;
	}
	ಭಙ್ಗ;
    }
    vim_free(buf);

    ಪ್ರತ್ಯಾಗಮ path;
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
    ಸ್ಥಿರ char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ಯದಿ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(name + 8);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	ಯದಿ (fatal != ರಿಕ್ತ)
	    *fatal = true;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
# ಅನ್ಯಥಾ
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ಧ್ರುವ char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ಯದಿ (name == ರಿಕ್ತ)
    {
	ಯದಿ (fatal != ರಿಕ್ತ)
	    *fatal = true;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Ignore if name is a path
    ಯದಿ (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(name);

    ಯದಿ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಪ್ರತ್ಯಾಗಮ vim_strsave(name + 8);

    ಯದಿ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ಯದಿ (vim_strchr(name, '/') != ರಿಕ್ತ)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	ಯದಿ (fatal != ರಿಕ್ತ)
	    *fatal = true;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    buf = alloc(MAXPATHL);

    ಯದಿ (buf == ರಿಕ್ತ)
    {
	ಯದಿ (fatal != ರಿಕ್ತ)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತಿ (size_t i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	ಧ್ರುವ char_u	*dir = known_dirs[i];
	bool		got = false;

	ಯದಿ (dir == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != ರಿಕ್ತ && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((ವರ್ಣ *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (unsigned long ಪೂರ್ಣಾಙ್ಕ)getuid());
	    ಯದಿ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ಅನುವರ್ತ;

	    vim_snprintf((ವರ್ಣ *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (unsigned long ಪೂರ್ಣಾಙ್ಕ)getuid(), name);
	}
	ಅನ್ಯಥಾ
	{
	    vim_snprintf((ವರ್ಣ *)buf, MAXPATHL, "%s/vim", dir);
	    ಯದಿ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ಅನುವರ್ತ;

	    vim_snprintf((ವರ್ಣ *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	ಯದಿ (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((ವರ್ಣ *)buf, ರಿಕ್ತ);
	    emsg_off--;
	    emsg_silent--;

	    ಯದಿ (channel != ರಿಕ್ತ)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    ಅನ್ಯಥಾ
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	ಯದಿ (got || (!new && mch_access((ವರ್ಣ *)buf, F_OK) == 0))
	{
	    ಯದಿ (server_address != ರಿಕ್ತ && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		ಭಙ್ಗ;
	    ಪ್ರತ್ಯಾಗಮ buf;
	}
	ಭಙ್ಗ;
    }

    ಯದಿ (!quiet)
    {
	ಯದಿ (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	ಅನ್ಯಥಾ ಯದಿ (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    ಯದಿ (!res && fatal != ರಿಕ್ತ)
	*fatal = true;

    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
# endif
}

/*
 * Callback for when client channel is closed
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_client_close(channel_T *channel)
{
    ಯದಿ (channel == client_channels)
	client_channels = channel->ch_ss_next;
    ಯದಿ (channel->ch_ss_prev != ರಿಕ್ತ)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    ಯದಿ (channel->ch_ss_next != ರಿಕ್ತ)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    ಯದಿ (client_channel_ಎಣಿಕೆ > 0)
	--client_channel_ಎಣಿಕೆ;

    ch_log(ರಿಕ್ತ, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_accept(channel_T *channel)
{
    ಯದಿ (client_channel_ಎಣಿಕೆ >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(ರಿಕ್ತ, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	ಪ್ರತ್ಯಾಗಮ;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = ರಿಕ್ತ;
    ಯದಿ (client_channels == ರಿಕ್ತ)
    {
	channel->ch_ss_prev = ರಿಕ್ತ;
	client_channels = channel;
    }
    ಅನ್ಯಥಾ
    {
	channel_T *last = client_channels;

	ಯಾವತ್ (last->ch_ss_next != ರಿಕ್ತ)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_ಎಣಿಕೆ;

    // We will read the command from the client later in the input loop.
    ch_log(ರಿಕ್ತ, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(ರಿಕ್ತ, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_socketserver_channel(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);

    FOR_ALL_CLIENTS(channel)
    {
	ಯದಿ (abort)
	    ಭಙ್ಗ;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
socketserver_send_message(channel_T *ch, dict_T *ಸಂದೇಶ, ವರ್ಣ *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (ver)
	dict_add_ಸಂಖ್ಯೆ(ಸಂದೇಶ, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = ಸಂದೇಶ;
    buf = json_encode(&tv, JSON_NL);

    ಯದಿ (buf != ರಿಕ್ತ)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buf), func);
	emsg_silent--;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;
    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ರಿಕ್ತ;
    char_u	*sender = ರಿಕ್ತ;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    ಯದಿ (di == ರಿಕ್ತ || di->di_tv.v_type != VAR_STRING)
	ಪ್ರತ್ಯಾಗಮ;
    ಅನ್ಯಥಾ
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_ಸಂಖ್ಯೆ)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((ವರ್ಣ *)idbuf, ಪರಿಮಾಣ(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ರಿಕ್ತ, "socketserver_exec(): result: %s",
	    str == ರಿಕ್ತ ? (char_u *)"(null)" : str);

    ಯದಿ (STRCMP(type, "expr") == 0 && str != ರಿಕ್ತ)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	ಪೂರ್ಣಾಙ್ಕ	    code;

	dict = dict_alloc();
	ಯದಿ (dict == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	result = eval_client_ಅಭಿವ್ಯಕ್ತಿ_to_string(str);
	code = result == ರಿಕ್ತ ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	ಯದಿ (result != ರಿಕ್ತ)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	ಅನ್ಯಥಾ
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_ಸಂಖ್ಯೆ(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(type, "keystrokes") == 0 && str != ರಿಕ್ತ)
    {
	// Execute keystrokes
	server_to_ಒಳಹರಿವು_buf(str);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	ಯದಿ (sender != ರಿಕ್ತ && str != ರಿಕ್ತ)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    ಯದಿ (reply != ರಿಕ್ತ)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    ಅನ್ಯಥಾ
	ch_error(ರಿಕ್ತ, "socketserver: unknown command type '%s'", type);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_ಸಂದೇಶ = head->jq_next;
    ಪೂರ್ಣಾಙ್ಕ	    ver;

    ಯದಿ (json_ಸಂದೇಶ == ರಿಕ್ತ)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_ಸಂದೇಶ = head->jq_next;
    }
    ಯದಿ (json_ಸಂದೇಶ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *tv = json_ಸಂದೇಶ->jq_ಮೌಲ್ಯ;
    remove_json_ಗಂಟು(head, json_ಸಂದೇಶ);

    ಯದಿ ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ರಿಕ್ತ, "socketserver: message is not a JSON object");
	free_tv(*tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ver = dict_get_ಸಂಖ್ಯೆ((*tv)->vval.v_dict, "version");

    ಯದಿ (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ರಿಕ್ತ, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	ಯದಿ (dict != ರಿಕ್ತ)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    ಶೂನ್ಯ
socketserver_parse_messages(ಶೂನ್ಯ)
{
    typval_T	*tv;

    ಪ್ರತಿ (channel_T *ch = client_channels; ch != ರಿಕ್ತ;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	ಯದಿ (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    ಅನುವರ್ತ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
socketserver_wait(channel_T *channel, ಪೂರ್ಣಾಙ್ಕ timeout)
{
    ಯಾವತ್ (true)
    {
	ಪೂರ್ಣಾಙ್ಕ		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	ಸಂರಚನಾ timeval  tv;
	ಪೂರ್ಣಾಙ್ಕ		maxfd = -1;

	ಯದಿ (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	ಯದಿ (channel != ರಿಕ್ತ)
	{
	    ಯದಿ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	ಯದಿ (server_channel != ರಿಕ್ತ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    ಯದಿ (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    ಯದಿ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (ಪೂರ್ಣಾಙ್ಕ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		ಯದಿ (maxfd < (ಪೂರ್ಣಾಙ್ಕ)ch->CH_SOCK_FD)
		    maxfd = (ಪೂರ್ಣಾಙ್ಕ)ch->CH_SOCK_FD;
	    }
	}

	ಯದಿ (maxfd == -1)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ret = select(maxfd + 1, &rfds, ರಿಕ್ತ, ರಿಕ್ತ, timeout == -1 ? ರಿಕ್ತ : &tv);

#  ifdef EINTR
	ಯದಿ (ret == -1 && errno == EINTR)
	{
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	    ಅನುವರ್ತ;
	}
#  endif

	ಯದಿ (ret > 0)
	{
	    ಯದಿ (server_channel != ರಿಕ್ತ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ಯದಿ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (ಪೂರ್ಣಾಙ್ಕ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ಯದಿ (channel == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ OK;

	    ಯದಿ (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
	    ಅನುವರ್ತ;
	}
# ಅನ್ಯಥಾ
	ಸಂರಚನಾ pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	ಪೂರ್ಣಾಙ್ಕ		nfd = 0;
	ಪೂರ್ಣಾಙ್ಕ		channel_idx = -1;
	ಪೂರ್ಣಾಙ್ಕ		server_idx = -1;

	ಯದಿ (channel != ರಿಕ್ತ)
	{
	    ಯದಿ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	ಯದಿ (server_channel != ರಿಕ್ತ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    ಯದಿ (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	ಯದಿ (ret == -1 && errno == EINTR)
	{
	    ಯದಿ (got_int)
		ಭಙ್ಗ;
	    ಅನುವರ್ತ;
	}
#  endif

	ಯದಿ (ret > 0)
	{
	    ಯದಿ (server_channel != ರಿಕ್ತ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ಯದಿ (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ಯದಿ (channel == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ OK;

	    ಯದಿ (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
	    ಅನುವರ್ತ;
	}
# endif
	ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    ಸ್ಥಿರ channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*address = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		port;
    bool	is_unix = false;
    channel_T	*channel;

    ಯದಿ (STRNICMP(name, "channel:", 8) == 0)
    {
	ಯದಿ (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (address != ರಿಕ್ತ)
	    is_unix = true;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	ಪೂರ್ಣಾಙ್ಕ id;

	ಯದಿ (name[11] == NUL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	id = strtol((ವರ್ಣ *)name + 11, ರಿಕ್ತ, 10);
	ಯದಿ (wait != ರಿಕ್ತ)
	    *wait = true;

	ಪ್ರತ್ಯಾಗಮ channel_find(id);
    }
    ಅನ್ಯಥಾ
    {
	address = socketserver_get_path(name, false, quiet, false, ರಿಕ್ತ);
	ಯದಿ (address == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	is_unix = true;
    }

    ಯದಿ (is_unix)
	channel = channel_open_unix((ವರ್ಣ *)address, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	channel =  channel_open("localhost", port, 1000, ರಿಕ್ತ);

    ಯದಿ (channel == ರಿಕ್ತ && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    ಪ್ರತ್ಯಾಗಮ channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    ಪೂರ್ಣಾಙ್ಕ
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_ಅಭಿವ್ಯಕ್ತಿ,
	ಪೂರ್ಣಾಙ್ಕ timeout,
	bool silent,
	channel_T **ch)
{
    ಪೂರ್ಣಾಙ್ಕ		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ರಿಕ್ತ;

    ಯದಿ (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    // Execute locally if target is ourselves
    ಯದಿ (serverName != ರಿಕ್ತ && STRICMP(name, serverName) == 0)
	ಪ್ರತ್ಯಾಗಮ sendToLocalVim(str, is_ಅಭಿವ್ಯಕ್ತಿ, result);

    channel = socketserver_get_channel(name, silent, ರಿಕ್ತ);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    dict = dict_alloc();
    ಯದಿ (dict == ರಿಕ್ತ)
	ಗಚ್ಛ exit;

    dict_add_string(dict, "type", (char_u *)(is_ಅಭಿವ್ಯಕ್ತಿ ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    ಯದಿ (server_address != ರಿಕ್ತ)
	dict_add_string(dict, "sender", server_address);
    ಅನ್ಯಥಾ ಯದಿ (ch != ರಿಕ್ತ)
	dict_add_bool(dict, "wait", true);

    ಯದಿ (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	ಗಚ್ಛ exit;
    }
    dict_unref(dict);

    ಯದಿ (!is_ಅಭಿವ್ಯಕ್ತಿ)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	ಯದಿ (ch != ರಿಕ್ತ)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = ರಿಕ್ತ;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    ಪ್ರತ್ಯಾಗಮ rcode;
	}
	ಗಚ್ಛ exit;
    }

    ಯದಿ (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    ಯಾವತ್ (socketserver_wait(channel, timeout) == OK)
	ಯದಿ (socketserver_get_message(channel, &resp_tv) == OK)
	    ಭಙ್ಗ;

    ಯದಿ (resp_tv == ರಿಕ್ತ)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	ಗಚ್ಛ exit;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    ಯದಿ (di == ರಿಕ್ತ || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ರಿಕ್ತ, "socketserver: unknown reply type");
	free_tv(resp_tv);
	ಗಚ್ಛ exit;
    }

    ಯದಿ (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	ಗಚ್ಛ exit;
    }

    ಯದಿ (result != ರಿಕ್ತ)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	ಅನ್ಯಥಾ
	{
	    free_tv(resp_tv);
	    ಗಚ್ಛ exit;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	rcode = di->di_tv.vval.v_ಸಂಖ್ಯೆ;

    free_tv(resp_tv);

exit:
    channel_close(channel, false);
    channel_clear(channel);
    ಪ್ರತ್ಯಾಗಮ rcode;
}

   ಸ್ಥಿರ ss_reply_T *
socketserver_get_reply(char_u *sender, ಪೂರ್ಣಾಙ್ಕ *index)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	ಯದಿ (STRCMP(reply->sender, sender) == 0)
	{
	    ಯದಿ (index != ರಿಕ್ತ)
		*index = i;
	    ಪ್ರತ್ಯಾಗಮ reply;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    ಸ್ಥಿರ ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    ಯದಿ (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, ಪರಿಮಾಣ(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, ರಿಕ್ತ);

    ಯದಿ (reply == ರಿಕ್ತ && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	ಯದಿ (reply->sender == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	ga_init2(&reply->strings, ಪರಿಮಾಣ(char_u *), 5);
    }

    ಪ್ರತ್ಯಾಗಮ reply;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_remove_reply(char_u *sender)
{
    ಪೂರ್ಣಾಙ್ಕ index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    ಯದಿ (reply != ರಿಕ್ತ)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	ಪೂರ್ಣಾಙ್ಕ	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	ಯದಿ (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    ಪರಿಮಾಣ(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    bool	wait = false;

    ಯದಿ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (server_channel == ರಿಕ್ತ || server_address == ರಿಕ್ತ)
    {
	emsg(_(e_socket_server_not_online));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    dict = dict_alloc();
    ಯದಿ (dict == ರಿಕ್ತ)
    {
	ret = FAIL;
	ಗಚ್ಛ exit;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    ಯದಿ (server_address != ರಿಕ್ತ)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

exit:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    ಯದಿ (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    ಪೂರ್ಣಾಙ್ಕ
socketserver_read_reply(
	char_u *client,
	char_u **str,
	ಪೂರ್ಣಾಙ್ಕ timeout,
	bool remotewait)
{
    ss_reply_T	*reply = ರಿಕ್ತ;
    char_u	*actual;

    ಯದಿ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (!remotewait && (server_channel == ರಿಕ್ತ || server_address == ರಿಕ್ತ))
    {
	emsg(_(e_socket_server_not_online));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, ರಿಕ್ತ);
    ಯದಿ (actual == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯಾವತ್ (true)
    {
	reply = socketserver_get_reply(actual, ರಿಕ್ತ);
	ಯದಿ (reply != ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (socketserver_wait(ರಿಕ್ತ, timeout) == FAIL)
	    ಭಙ್ಗ;
    }

    ಯದಿ (reply == ರಿಕ್ತ || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    ಯದಿ (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		ಪರಿಮಾಣ(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    ಯದಿ (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    ಪೂರ್ಣಾಙ್ಕ
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    ಯದಿ (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (server_channel == ರಿಕ್ತ || server_address == ರಿಕ್ತ)
    {
	emsg(_(e_socket_server_not_online));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, ರಿಕ್ತ);
    ಯದಿ (actual == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    reply = socketserver_get_reply(actual, ರಿಕ್ತ);
    vim_free(actual);

    ಯದಿ (reply != ರಿಕ್ತ && reply->strings.ga_len > 0)
    {
	ಯದಿ (str != ರಿಕ್ತ)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	ಪ್ರತ್ಯಾಗಮ 1;
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

#endif // FEAT_SOCKETSERVER
