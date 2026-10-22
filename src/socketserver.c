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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T ਲੜੀਆਂ;
} ss_ਜਵਾਬ_T;

ਸਥਿਰ channel_T    *server_channel = ਨਿਰਾ;
ਸਥਿਰ char_u	    *server_ਪਤਾ = ਨਿਰਾ; // Includes "type:" prefix if any
ਸਥਿਰ bool	    server_is_unix = false;
ਸਥਿਰ char_u	    *server_addr_cwd = ਨਿਰਾ; // CWD when server was started, used
					  // to handle relative file paths.
ਸਥਿਰ garray_T	    server_replies;

ਸਥਿਰ channel_T    *client_channels = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	    client_channel_ਗਿਣਤੀ = 0;

# define FOR_ALL_CLIENTS(ch) \
    ਲਈ (ch = client_channels; ch != ਨਿਰਾ; ch = ch->ch_ss_ਅਗਲਾ)

ਸਥਿਰ ਖਾਲੀ socketserver_cleanup(ਖਾਲੀ);
ਸਥਿਰ char_u *socketserver_create_ਰਸਤਾ(char_u *name, bool quiet);
ਸਥਿਰ char_u *socketserver_get_ਰਸਤਾ(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
ਸਥਿਰ ਖਾਲੀ socketserver_accept(channel_T *channel);
ਸਥਿਰ ਖਾਲੀ socketserver_close(channel_T *channel);
ਸਥਿਰ ss_ਜਵਾਬ_T *socketserver_ਜੋੜੋ_ਜਵਾਬ(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    ਪੂਰਨ
socketserver_ਸ਼ੁਰੂ(char_u *name, bool quiet)
{
    char_u	*ਪਤਾ = ਨਿਰਾ;
    ਪੂਰਨ		ਪੋਰਟ;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    ਜੇ (server_channel != ਨਿਰਾ)
	ਵਾਪਸ OK;

    ਜੇ (STRNICMP(name, "channel:", 8) == 0)
    {
	ਜੇ (channel_parse_socketserver_ਪਤਾ(name + 8, &ਪੋਰਟ, &ਪਤਾ, quiet)
		== FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (ਪਤਾ != ਨਿਰਾ)
	    is_unix = true;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪਤਾ = socketserver_create_ਰਸਤਾ(name, quiet);
	ਜੇ (ਪਤਾ == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	is_unix = true;
    }

    ਜੇ (is_unix)
	channel = channel_listen_unix((ਅੱਖਰ *)ਪਤਾ, ਨਿਰਾ, false);
    ਨਹੀਂਤਾਂ
	channel = channel_listen(ਪੋਰਟ, ਨਿਰਾ);

    ਜੇ (channel == ਨਿਰਾ)
    {
	vim_free(ਪਤਾ);
	ਵਾਪਸ FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    ਜੇ (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    ਨਹੀਂਤਾਂ
    {
	buf = alloc(MAXPATHL + 1);
	ਜੇ (buf != ਨਿਰਾ)
	{
	    buf[0] = NUL;
	    mch_ਭਰਿਆ_Name(ਪਤਾ, buf, MAXPATHL, false);
	}
    }
    vim_free(ਪਤਾ);

    ਜੇ (buf != ਨਿਰਾ)
    {
	server_ਪਤਾ = vim_strsave(buf);
	serverName = buf;
	ਸੈੱਟ_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    ਜੇ (mch_dirname(dirbuf, ਆਕਾਰ(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    ਨਹੀਂਤਾਂ
	server_addr_cwd = ਨਿਰਾ;

    ga_init2(&server_replies, ਆਕਾਰ(ss_ਜਵਾਬ_T), 2);

    ch_log(ਨਿਰਾ, "socketserver: started server at %s", name);

    ਵਾਪਸ OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    ਖਾਲੀ
socketserver_stop(ਖਾਲੀ)
{
    ਜੇ (server_channel == ਨਿਰਾ)
	ਵਾਪਸ;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ਨਿਰਾ, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    ਸਥਿਰ ਖਾਲੀ
socketserver_cleanup(ਖਾਲੀ)
{
# ifdef UNIX
    ਜੇ (server_is_unix && server_ਪਤਾ != ਨਿਰਾ)
    {
	char_u *ਰਸਤਾ = server_ਪਤਾ;
	char_u dirbuf[MAXPATHL];

	ਜੇ (STRNICMP(ਰਸਤਾ, "channel:unix:", 13) == 0)
	    ਰਸਤਾ += 13;
	ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(ਰਸਤਾ, "name:", 5) == 0)
	    ਰਸਤਾ += 5;

	ਜੇ (*ਰਸਤਾ == '/')
	    mch_remove(ਰਸਤਾ);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	ਨਹੀਂਤਾਂ ਜੇ (server_addr_cwd != ਨਿਰਾ
		&& mch_dirname(dirbuf, ਆਕਾਰ(dirbuf)) == OK)
	{
	    ਜੇ (mch_chdir((ਅੱਖਰ *)server_addr_cwd) == 0)
	    {
		mch_remove(ਰਸਤਾ);
		ਜੇ (mch_chdir((ਅੱਖਰ *)dirbuf) < 0)
		    emsg(_(e_cannot_go_ਪਿੱਠ_ਮੰਜ਼ਿਲ_ਪਿਛਲਾ_directory));
	    }
	}
    }
# endif

    server_channel = ਨਿਰਾ;
    VIM_CLEAR(server_ਪਤਾ);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    ਲਈ (ਪੂਰਨ i = 0; i < server_replies.ga_len; i++)
    {
	ss_ਜਵਾਬ_T *ਜਵਾਬ = (ss_ਜਵਾਬ_T *)server_replies.ga_ਅੰਕੜੇ + i;

	vim_free(ਜਵਾਬ->sender);
	ga_clear_ਲੜੀਆਂ(&ਜਵਾਬ->ਲੜੀਆਂ);
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
socketserver_list(ਖਾਲੀ)
{
    list_T *list = list_alloc();

    ਜੇ (list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

# ifdef MSWIN
    // Only support addresses on Windows
    ਵਾਪਸ list;
# ਨਹੀਂਤਾਂ
    string_T	    buf;
    string_T	    ਰਸਤਾ;
    DIR		    *dirp;
    ਬਣਤਰ dirent   *dp;
    ਅਚਲ char_u    *ਗਿਆਤ_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ਜੇ ((buf.string = alloc(MAXPATHL)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ ((ਰਸਤਾ.string = alloc(MAXPATHL)) == ਨਿਰਾ)
    {
	vim_free(buf.string);
	ਵਾਪਸ ਨਿਰਾ;
    }
    buf.ਲੰਮਾਈ = 0;
    ਰਸਤਾ.ਲੰਮਾਈ = 0;

    ਲਈ (size_t i = 0 ; i < ARRAY_ਲੰਮਾਈ(ਗਿਆਤ_dirs); i++)
    {
	ਅਚਲ char_u *dir = ਗਿਆਤ_dirs[i];

	ਜੇ (dir == ਨਿਰਾ)
	    ਜਾਰੀ;

	ਜੇ (STRCMP(dir, "/tmp") == 0 ||
		(ਗਿਆਤ_dirs[1] != ਨਿਰਾ && STRCMP(dir, ਗਿਆਤ_dirs[1]) == 0))
	    ਰਸਤਾ.ਲੰਮਾਈ = vim_snprintf_safelen((ਅੱਖਰ *)ਰਸਤਾ.string, MAXPATHL,
		    "%s/vim-%lu", dir, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਪੂਰਨ)getuid());
	ਨਹੀਂਤਾਂ
	    ਰਸਤਾ.ਲੰਮਾਈ = vim_snprintf_safelen((ਅੱਖਰ *)ਰਸਤਾ.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((ਅੱਖਰ *)ਰਸਤਾ.string);
	ਜੇ (dirp == ਨਿਰਾ)
	    ਜਾਰੀ;

	// Loop through directory
	ਜਦੋਂ ((dp = readdir(dirp)) != ਨਿਰਾ)
	{
	    ਜੇ (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		ਜਾਰੀ;

	    buf.ਲੰਮਾਈ = vim_snprintf_safelen((ਅੱਖਰ *)buf.string, MAXPATHL,
		    "%s/%s", ਰਸਤਾ.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.ਲੰਮਾਈ - (ਰਸਤਾ.ਲੰਮਾਈ + 1));
	}

	closedir(dirp);

	ਰੋਕੋ;
    }

    vim_free(ਰਸਤਾ.string);
    vim_free(buf.string);

    ਵਾਪਸ list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    ਸਥਿਰ char_u *
socketserver_create_ਰਸਤਾ(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ਜੇ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ਵਾਪਸ vim_strsave(name + 8);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!quiet)
	    semsg(_(e_invalid_ਦਲੀਲ_str), name);
	ਵਾਪਸ ਨਿਰਾ;
    }
# ਨਹੀਂਤਾਂ
    char_u  *buf = ਨਿਰਾ;
    ਪੂਰਨ	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *ਰਸਤਾ = ਨਿਰਾ;
    bool    fatal = false;

    ਜੇ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ਵਾਪਸ vim_strsave(name + 8);
    ਜੇ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ਲਈ (ਪੂਰਨ i = 0; i < 1000; i++)
    {
	ਜੇ (buf != ਨਿਰਾ)
	{
	    vim_snprintf((ਅੱਖਰ *)buf, buflen, "%s%d", name, i);
	    ਰਸਤਾ = socketserver_get_ਰਸਤਾ(buf, true, quiet, true, &fatal);
	}
	ਨਹੀਂਤਾਂ
	    ਰਸਤਾ = socketserver_get_ਰਸਤਾ(name, true, quiet, true, &fatal);

	ਜੇ (fatal)
	    ਰੋਕੋ;

	ਜੇ (ਰਸਤਾ == ਨਿਰਾ)
	{
	    ਜੇ (buf == ਨਿਰਾ)
	    {
		buf = alloc(buflen);
		ਜੇ (buf == ਨਿਰਾ)
		{
		    semsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ_allocating_nr_ਬਾਈਟਾਂ), buflen);
		    ਵਾਪਸ ਨਿਰਾ;
		}
	    }
	    ਜਾਰੀ;
	}
	ਰੋਕੋ;
    }
    vim_free(buf);

    ਵਾਪਸ ਰਸਤਾ;
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
    ਸਥਿਰ char_u *
socketserver_get_ਰਸਤਾ(
	char_u *name,
	bool new ਅਣਵਰਤਿਆ,
	bool quiet,
	bool ignore ਅਣਵਰਤਿਆ,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ਜੇ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ਵਾਪਸ vim_strsave(name + 8);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!quiet)
	    semsg(_(e_invalid_ਦਲੀਲ_str), name);
	ਜੇ (fatal != ਨਿਰਾ)
	    *fatal = true;
	ਵਾਪਸ ਨਿਰਾ;
    }
# ਨਹੀਂਤਾਂ
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ਅਚਲ char_u    *ਗਿਆਤ_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ਜੇ (name == ਨਿਰਾ)
    {
	ਜੇ (fatal != ਨਿਰਾ)
	    *fatal = true;
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Ignore if name is a path
    ਜੇ (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	ਵਾਪਸ vim_strsave(name);

    ਜੇ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ਵਾਪਸ vim_strsave(name + 8);

    ਜੇ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ਜੇ (vim_strchr(name, '/') != ਨਿਰਾ)
    {
	ਜੇ (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	ਜੇ (fatal != ਨਿਰਾ)
	    *fatal = true;
	ਵਾਪਸ ਨਿਰਾ;
    }

    buf = alloc(MAXPATHL);

    ਜੇ (buf == ਨਿਰਾ)
    {
	ਜੇ (fatal != ਨਿਰਾ)
	    *fatal = true;
	semsg(_(e_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ_allocating_nr_ਬਾਈਟਾਂ), MAXPATHL);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਲਈ (size_t i = 0; i < ARRAY_ਲੰਮਾਈ(ਗਿਆਤ_dirs); i++)
    {
	ਅਚਲ char_u	*dir = ਗਿਆਤ_dirs[i];
	bool		got = false;

	ਜੇ (dir == ਨਿਰਾ)
	    ਜਾਰੀ;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(dir, "/tmp") == 0 ||
		(ਗਿਆਤ_dirs[1] != ਨਿਰਾ && STRCMP(dir, ਗਿਆਤ_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((ਅੱਖਰ *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਪੂਰਨ)getuid());
	    ਜੇ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ਜਾਰੀ;

	    vim_snprintf((ਅੱਖਰ *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਪੂਰਨ)getuid(), name);
	}
	ਨਹੀਂਤਾਂ
	{
	    vim_snprintf((ਅੱਖਰ *)buf, MAXPATHL, "%s/vim", dir);
	    ਜੇ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ਜਾਰੀ;

	    vim_snprintf((ਅੱਖਰ *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	ਜੇ (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((ਅੱਖਰ *)buf, ਨਿਰਾ);
	    emsg_off--;
	    emsg_silent--;

	    ਜੇ (channel != ਨਿਰਾ)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	ਜੇ (got || (!new && mch_access((ਅੱਖਰ *)buf, F_OK) == 0))
	{
	    ਜੇ (server_ਪਤਾ != ਨਿਰਾ && STRCMP(buf, server_ਪਤਾ) == 0)
		// Can't connect to itself
		ਰੋਕੋ;
	    ਵਾਪਸ buf;
	}
	ਰੋਕੋ;
    }

    ਜੇ (!quiet)
    {
	ਜੇ (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	ਨਹੀਂਤਾਂ ਜੇ (!ignore)
	    semsg(_(e_invalid_server_id_ਵਰਤਿਆ_str), name);
    }
    ਜੇ (!res && fatal != ਨਿਰਾ)
	*fatal = true;

    vim_free(buf);
    ਵਾਪਸ ਨਿਰਾ;
# endif
}

/*
 * Callback for when client channel is closed
 */
    ਸਥਿਰ ਖਾਲੀ
socketserver_client_close(channel_T *channel)
{
    ਜੇ (channel == client_channels)
	client_channels = channel->ch_ss_ਅਗਲਾ;
    ਜੇ (channel->ch_ss_prev != ਨਿਰਾ)
	channel->ch_ss_prev->ch_ss_ਅਗਲਾ = channel->ch_ss_ਅਗਲਾ;
    ਜੇ (channel->ch_ss_ਅਗਲਾ != ਨਿਰਾ)
	channel->ch_ss_ਅਗਲਾ->ch_ss_prev = channel->ch_ss_prev;
    ਜੇ (client_channel_ਗਿਣਤੀ > 0)
	--client_channel_ਗਿਣਤੀ;

    ch_log(ਨਿਰਾ, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    ਸਥਿਰ ਖਾਲੀ
socketserver_accept(channel_T *channel)
{
    ਜੇ (client_channel_ਗਿਣਤੀ >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(ਨਿਰਾ, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	ਵਾਪਸ;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_ਅਗਲਾ = ਨਿਰਾ;
    ਜੇ (client_channels == ਨਿਰਾ)
    {
	channel->ch_ss_prev = ਨਿਰਾ;
	client_channels = channel;
    }
    ਨਹੀਂਤਾਂ
    {
	channel_T *ਆਖਰੀ = client_channels;

	ਜਦੋਂ (ਆਖਰੀ->ch_ss_ਅਗਲਾ != ਨਿਰਾ)
	    ਆਖਰੀ = ਆਖਰੀ->ch_ss_ਅਗਲਾ;
	ਆਖਰੀ->ch_ss_ਅਗਲਾ = channel;
	channel->ch_ss_prev = ਆਖਰੀ;
    }
    ++client_channel_ਗਿਣਤੀ;

    // We will read the command from the client later in the input loop.
    ch_log(ਨਿਰਾ, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    ਸਥਿਰ ਖਾਲੀ
socketserver_close(channel_T *channel ਅਣਵਰਤਿਆ)
{
    socketserver_cleanup();

    ch_log(ਨਿਰਾ, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_socketserver_channel(ਪੂਰਨ copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);

    FOR_ALL_CLIENTS(channel)
    {
	ਜੇ (abort)
	    ਰੋਕੋ;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    }
    ਵਾਪਸ abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    ਸਥਿਰ ਪੂਰਨ
socketserver_send_ਸੁਨੇਹਾ(channel_T *ch, dict_T *ਸੁਨੇਹਾ, ਅੱਖਰ *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    ਪੂਰਨ		ret;

    ਜੇ (ver)
	dict_ਜੋੜੋ_ਸੰਖਿਆ(ਸੁਨੇਹਾ, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = ਸੁਨੇਹਾ;
    buf = json_encode(&tv, JSON_NL);

    ਜੇ (buf != ਨਿਰਾ)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (ਪੂਰਨ)STRLEN(buf), func);
	emsg_silent--;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;
    vim_free(buf);
    ਵਾਪਸ ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    ਸਥਿਰ ਖਾਲੀ
socketserver_exec(channel_T *channel, dict_T *ਸੁਨੇਹਾ_2)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ਨਿਰਾ;
    char_u	*sender = ਨਿਰਾ;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_ਲੱਭੋ(ਸੁਨੇਹਾ_2, (char_u *)"type", -1);
    ਜੇ (di == ਨਿਰਾ || di->di_tv.v_type != VAR_STRING)
	ਵਾਪਸ;
    ਨਹੀਂਤਾਂ
	type = di->di_tv.vval.v_string;

    di = dict_ਲੱਭੋ(ਸੁਨੇਹਾ_2, (char_u *)"str", -1);
    ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_ਲੱਭੋ(ਸੁਨੇਹਾ_2, (char_u *)"sender", -1);
    ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_ਲੱਭੋ(ਸੁਨੇਹਾ_2, (char_u *)"wait", -1);
    ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_ਸੰਖਿਆ)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((ਅੱਖਰ *)idbuf, ਆਕਾਰ(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ਨਿਰਾ, "socketserver_exec(): result: %s",
	    str == ਨਿਰਾ ? (char_u *)"(null)" : str);

    ਜੇ (STRCMP(type, "expr") == 0 && str != ਨਿਰਾ)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *ਨਤੀਜਾ;
	ਪੂਰਨ	    ਕੋਡ;

	dict = dict_alloc();
	ਜੇ (dict == ਨਿਰਾ)
	    ਵਾਪਸ;

	ਨਤੀਜਾ = eval_client_ਵਿਅੰਜਕ_ਮੰਜ਼ਿਲ_string(str);
	ਕੋਡ = ਨਤੀਜਾ == ਨਿਰਾ ? -1 : 0;

	dict_ਜੋੜੋ_string(dict, "type", (char_u *)"reply");
	ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ)
	{
	    dict_ਜੋੜੋ_string(dict, "str", ਨਤੀਜਾ);
	    vim_free(ਨਤੀਜਾ);
	}
	ਨਹੀਂਤਾਂ
	    // Error occurred, return error message
	    dict_ਜੋੜੋ_string(dict, "str",
		    (char_u *)_(e_invalid_ਵਿਅੰਜਕ_received));

	dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "code", ਕੋਡ);

	socketserver_send_ਸੁਨੇਹਾ(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(type, "keystrokes") == 0 && str != ਨਿਰਾ)
    {
	// Execute keystrokes
	server_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(str);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	ਜੇ (sender != ਨਿਰਾ && str != ਨਿਰਾ)
	{
	    ss_ਜਵਾਬ_T *ਜਵਾਬ;

	    ਜਵਾਬ = socketserver_ਜੋੜੋ_ਜਵਾਬ(sender);

	    ਜੇ (ਜਵਾਬ != ਨਿਰਾ)
		ga_copy_string(&ਜਵਾਬ->ਲੜੀਆਂ, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_ਸੰਸਕਰਣ_mismatch));
    }
    ਨਹੀਂਤਾਂ
	ch_error(ਨਿਰਾ, "socketserver: unknown command type '%s'", type);
}

    ਸਥਿਰ ਪੂਰਨ
socketserver_get_ਸੁਨੇਹਾ(channel_T *channel, typval_T **tv)
{
    jsonq_T *ਸਿਰ = &channel->ch_part[PART_SOCK].ch_json_ਸਿਰ;
    jsonq_T *json_ਸੁਨੇਹਾ = ਸਿਰ->jq_ਅਗਲਾ;
    ਪੂਰਨ	    ver;

    ਜੇ (json_ਸੁਨੇਹਾ == ਨਿਰਾ)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_ਸੁਨੇਹਾ = ਸਿਰ->jq_ਅਗਲਾ;
    }
    ਜੇ (json_ਸੁਨੇਹਾ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    *tv = json_ਸੁਨੇਹਾ->jq_ਮੁੱਲ;
    remove_json_ਗੰਢ(ਸਿਰ, json_ਸੁਨੇਹਾ);

    ਜੇ ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ਨਿਰਾ, "socketserver: message is not a JSON object");
	free_tv(*tv);
	ਵਾਪਸ FAIL;
    }

    ver = dict_get_ਸੰਖਿਆ((*tv)->vval.v_dict, "version");

    ਜੇ (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ਨਿਰਾ, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	ਜੇ (dict != ਨਿਰਾ)
	{
	    dict_ਜੋੜੋ_string(dict, "type", (char_u *)"ver");
	    socketserver_send_ਸੁਨੇਹਾ(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    ਖਾਲੀ
socketserver_parse_messages(ਖਾਲੀ)
{
    typval_T	*tv;

    ਲਈ (channel_T *ch = client_channels; ch != ਨਿਰਾ;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *ਅਗਲਾ = ch->ch_ss_ਅਗਲਾ;

	// Get the JSON message if there is any from the queue for this channel.
	ਜੇ (socketserver_get_ਸੁਨੇਹਾ(ch, &tv) == FAIL)
	{
	    ch = ਅਗਲਾ;
	    ਜਾਰੀ;
	}

	socketserver_exec(ch, tv->vval.v_dict);
	free_tv(tv);
	ch = ਅਗਲਾ;
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
    ਸਥਿਰ ਪੂਰਨ
socketserver_wait(channel_T *channel, ਪੂਰਨ timeout)
{
    ਜਦੋਂ (true)
    {
	ਪੂਰਨ		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	ਬਣਤਰ timeval  tv;
	ਪੂਰਨ		maxfd = -1;

	ਜੇ (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	ਜੇ (channel != ਨਿਰਾ)
	{
	    ਜੇ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ਵਾਪਸ FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	ਜੇ (server_channel != ਨਿਰਾ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    ਜੇ (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    ਜੇ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (ਪੂਰਨ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		ਜੇ (maxfd < (ਪੂਰਨ)ch->CH_SOCK_FD)
		    maxfd = (ਪੂਰਨ)ch->CH_SOCK_FD;
	    }
	}

	ਜੇ (maxfd == -1)
	    ਵਾਪਸ FAIL;

	ret = select(maxfd + 1, &rfds, ਨਿਰਾ, ਨਿਰਾ, timeout == -1 ? ਨਿਰਾ : &tv);

#  ifdef EINTR
	ਜੇ (ret == -1 && errno == EINTR)
	{
	    ਜੇ (got_int)
		ਰੋਕੋ;
	    ਜਾਰੀ;
	}
#  endif

	ਜੇ (ret > 0)
	{
	    ਜੇ (server_channel != ਨਿਰਾ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ਜੇ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (ਪੂਰਨ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ਜੇ (channel == ਨਿਰਾ)
		ਵਾਪਸ OK;

	    ਜੇ (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		ਵਾਪਸ OK;
	    }
	    ਜਾਰੀ;
	}
# ਨਹੀਂਤਾਂ
	ਬਣਤਰ pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	ਪੂਰਨ		nfd = 0;
	ਪੂਰਨ		channel_idx = -1;
	ਪੂਰਨ		server_idx = -1;

	ਜੇ (channel != ਨਿਰਾ)
	{
	    ਜੇ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ਵਾਪਸ FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	ਜੇ (server_channel != ਨਿਰਾ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    ਜੇ (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	ਜੇ (ret == -1 && errno == EINTR)
	{
	    ਜੇ (got_int)
		ਰੋਕੋ;
	    ਜਾਰੀ;
	}
#  endif

	ਜੇ (ret > 0)
	{
	    ਜੇ (server_channel != ਨਿਰਾ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ਜੇ (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ਜੇ (channel == ਨਿਰਾ)
		ਵਾਪਸ OK;

	    ਜੇ (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		ਵਾਪਸ OK;
	    }
	    ਜਾਰੀ;
	}
# endif
	ਰੋਕੋ;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    ਸਥਿਰ channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*ਪਤਾ = ਨਿਰਾ;
    ਪੂਰਨ		ਪੋਰਟ;
    bool	is_unix = false;
    channel_T	*channel;

    ਜੇ (STRNICMP(name, "channel:", 8) == 0)
    {
	ਜੇ (channel_parse_socketserver_ਪਤਾ(name + 8, &ਪੋਰਟ, &ਪਤਾ, true)
		== FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (ਪਤਾ != ਨਿਰਾ)
	    is_unix = true;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	ਪੂਰਨ id;

	ਜੇ (name[11] == NUL)
	    ਵਾਪਸ ਨਿਰਾ;
	id = strtol((ਅੱਖਰ *)name + 11, ਨਿਰਾ, 10);
	ਜੇ (wait != ਨਿਰਾ)
	    *wait = true;

	ਵਾਪਸ channel_ਲੱਭੋ(id);
    }
    ਨਹੀਂਤਾਂ
    {
	ਪਤਾ = socketserver_get_ਰਸਤਾ(name, false, quiet, false, ਨਿਰਾ);
	ਜੇ (ਪਤਾ == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;
	is_unix = true;
    }

    ਜੇ (is_unix)
	channel = channel_open_unix((ਅੱਖਰ *)ਪਤਾ, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	channel =  channel_open("localhost", ਪੋਰਟ, 1000, ਨਿਰਾ);

    ਜੇ (channel == ਨਿਰਾ && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(ਪਤਾ);

    ਵਾਪਸ channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    ਪੂਰਨ
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **ਨਤੀਜਾ,
	bool is_ਵਿਅੰਜਕ,
	ਪੂਰਨ timeout,
	bool silent,
	channel_T **ch)
{
    ਪੂਰਨ		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ਨਿਰਾ;

    ਜੇ (*name == NUL)
    {
	semsg(_(e_unable_ਮੰਜ਼ਿਲ_send_ਮੰਜ਼ਿਲ_str), name);
	ਵਾਪਸ -1;
    }

    // Execute locally if target is ourselves
    ਜੇ (serverName != ਨਿਰਾ && STRICMP(name, serverName) == 0)
	ਵਾਪਸ send_ਮੰਜ਼ਿਲ_Local_Vim(str, is_ਵਿਅੰਜਕ, ਨਤੀਜਾ);

    channel = socketserver_get_channel(name, silent, ਨਿਰਾ);
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ -1;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    dict_ਜੋੜੋ_string(dict, "type", (char_u *)(is_ਵਿਅੰਜਕ ? "expr" : "keystrokes"));
    dict_ਜੋੜੋ_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    ਜੇ (server_ਪਤਾ != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(dict, "sender", server_ਪਤਾ);
    ਨਹੀਂਤਾਂ ਜੇ (ch != ਨਿਰਾ)
	dict_ਜੋੜੋ_bool(dict, "wait", true);

    ਜੇ (socketserver_send_ਸੁਨੇਹਾ(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_ਮੰਜ਼ਿਲ_send_ਮੰਜ਼ਿਲ_str), name);
	dict_unref(dict);
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }
    dict_unref(dict);

    ਜੇ (!is_ਵਿਅੰਜਕ)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	ਜੇ (ch != ਨਿਰਾ)
	{
	    *ch = channel;

	    channel->ch_ss_ਅਗਲਾ = client_channels;
	    channel->ch_ss_prev = ਨਿਰਾ;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    ਵਾਪਸ rcode;
	}
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    ਜੇ (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    ਜਦੋਂ (socketserver_wait(channel, timeout) == OK)
	ਜੇ (socketserver_get_ਸੁਨੇਹਾ(channel, &resp_tv) == OK)
	    ਰੋਕੋ;

    ਜੇ (resp_tv == ਨਿਰਾ)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_ਲੱਭੋ(dict, (char_u *)"type", -1);
    ਜੇ (di == ਨਿਰਾ || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ਨਿਰਾ, "socketserver: unknown reply type");
	free_tv(resp_tv);
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    ਜੇ (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_ਸੰਸਕਰਣ_mismatch));
	free_tv(resp_tv);
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    ਜੇ (ਨਤੀਜਾ != ਨਿਰਾ)
    {
	di = dict_ਲੱਭੋ(dict, (char_u *)"str", -1);
	ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_STRING)
	    *ਨਤੀਜਾ = vim_strsave(di->di_tv.vval.v_string);
	ਨਹੀਂਤਾਂ
	{
	    free_tv(resp_tv);
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
	}
    }

    di = dict_ਲੱਭੋ(dict, (char_u *)"code", -1);
    ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_ਸੰਖਿਆ)
	rcode = di->di_tv.vval.v_ਸੰਖਿਆ;

    free_tv(resp_tv);

ਬਾਹਰ_ਨਿਕਲੋ:
    channel_close(channel, false);
    channel_clear(channel);
    ਵਾਪਸ rcode;
}

   ਸਥਿਰ ss_ਜਵਾਬ_T *
socketserver_get_ਜਵਾਬ(char_u *sender, ਪੂਰਨ *ਸੂਚਕਾਂਕ)
{
    ਲਈ (ਪੂਰਨ i = 0; i < server_replies.ga_len; i++)
    {
	ss_ਜਵਾਬ_T *ਜਵਾਬ = ((ss_ਜਵਾਬ_T *)server_replies.ga_ਅੰਕੜੇ) + i;

	ਜੇ (STRCMP(ਜਵਾਬ->sender, sender) == 0)
	{
	    ਜੇ (ਸੂਚਕਾਂਕ != ਨਿਰਾ)
		*ਸੂਚਕਾਂਕ = i;
	    ਵਾਪਸ ਜਵਾਬ;
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    ਸਥਿਰ ss_ਜਵਾਬ_T *
socketserver_ਜੋੜੋ_ਜਵਾਬ(char_u *sender)
{
    ss_ਜਵਾਬ_T *ਜਵਾਬ;

    ਜੇ (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, ਆਕਾਰ(ss_ਜਵਾਬ_T), 1);

    ਜਵਾਬ = socketserver_get_ਜਵਾਬ(sender, ਨਿਰਾ);

    ਜੇ (ਜਵਾਬ == ਨਿਰਾ && ga_grow(&server_replies, 1) == OK)
    {
	ਜਵਾਬ = ((ss_ਜਵਾਬ_T *)server_replies.ga_ਅੰਕੜੇ) + server_replies.ga_len++;

	ਜਵਾਬ->sender = vim_strsave(sender);

	ਜੇ (ਜਵਾਬ->sender == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	ga_init2(&ਜਵਾਬ->ਲੜੀਆਂ, ਆਕਾਰ(char_u *), 5);
    }

    ਵਾਪਸ ਜਵਾਬ;
}

    ਸਥਿਰ ਖਾਲੀ
socketserver_remove_ਜਵਾਬ(char_u *sender)
{
    ਪੂਰਨ ਸੂਚਕਾਂਕ;
    ss_ਜਵਾਬ_T *ਜਵਾਬ = socketserver_get_ਜਵਾਬ(sender, &ਸੂਚਕਾਂਕ);

    ਜੇ (ਜਵਾਬ != ਨਿਰਾ)
    {
	ss_ਜਵਾਬ_T  *arr = server_replies.ga_ਅੰਕੜੇ;
	ਪੂਰਨ	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(ਜਵਾਬ->sender);
	ga_clear_ਲੜੀਆਂ(&ਜਵਾਬ->ਲੜੀਆਂ);

	// Move all elements after the removed reply forward by one
	ਜੇ (len > 1)
	    mch_memmove(arr + ਸੂਚਕਾਂਕ, arr + ਸੂਚਕਾਂਕ + 1,
		    ਆਕਾਰ(ss_ਜਵਾਬ_T) * (len - ਸੂਚਕਾਂਕ - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    ਪੂਰਨ
socketserver_send_ਜਵਾਬ(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    ਪੂਰਨ		ret = OK;
    bool	wait = false;

    ਜੇ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_ਵਰਤਿਆ_str), client);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (server_channel == ਨਿਰਾ || server_ਪਤਾ == ਨਿਰਾ)
    {
	emsg(_(e_socket_server_not_online));
	ਵਾਪਸ FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    dict = dict_alloc();
    ਜੇ (dict == ਨਿਰਾ)
    {
	ret = FAIL;
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    dict_ਜੋੜੋ_string(dict, "type", (char_u *)"notify");
    dict_ਜੋੜੋ_string(dict, "str", str);
    ਜੇ (server_ਪਤਾ != ਨਿਰਾ)
	dict_ਜੋੜੋ_string(dict, "sender", server_ਪਤਾ);

    ret = socketserver_send_ਸੁਨੇਹਾ(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

ਬਾਹਰ_ਨਿਕਲੋ:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    ਜੇ (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    ਵਾਪਸ ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    ਪੂਰਨ
socketserver_read_ਜਵਾਬ(
	char_u *client,
	char_u **str,
	ਪੂਰਨ timeout,
	bool remotewait)
{
    ss_ਜਵਾਬ_T	*ਜਵਾਬ = ਨਿਰਾ;
    char_u	*actual;

    ਜੇ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_ਵਰਤਿਆ_str), client);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (!remotewait && (server_channel == ਨਿਰਾ || server_ਪਤਾ == ਨਿਰਾ))
    {
	emsg(_(e_socket_server_not_online));
	ਵਾਪਸ FAIL;
    }

    actual = socketserver_get_ਰਸਤਾ(client, false, false, false, ਨਿਰਾ);
    ਜੇ (actual == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜਦੋਂ (true)
    {
	ਜਵਾਬ = socketserver_get_ਜਵਾਬ(actual, ਨਿਰਾ);
	ਜੇ (ਜਵਾਬ != ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (socketserver_wait(ਨਿਰਾ, timeout) == FAIL)
	    ਰੋਕੋ;
    }

    ਜੇ (ਜਵਾਬ == ਨਿਰਾ || ਜਵਾਬ->ਲੜੀਆਂ.ga_len == 0)
    {
	vim_free(actual);
	ਵਾਪਸ FAIL;
    }

    // Consume the string
    *str = ((char_u **)ਜਵਾਬ->ਲੜੀਆਂ.ga_ਅੰਕੜੇ)[0];

    ਜੇ (ਜਵਾਬ->ਲੜੀਆਂ.ga_len > 1)
	mch_memmove((char_u **)ਜਵਾਬ->ਲੜੀਆਂ.ga_ਅੰਕੜੇ,
		((char_u **)ਜਵਾਬ->ਲੜੀਆਂ.ga_ਅੰਕੜੇ) + 1,
		ਆਕਾਰ(char_u *) * (ਜਵਾਬ->ਲੜੀਆਂ.ga_len - 1));
    ਜਵਾਬ->ਲੜੀਆਂ.ga_len--;

    ਜੇ (ਜਵਾਬ->ਲੜੀਆਂ.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_ਜਵਾਬ(actual);

    vim_free(actual);

    ਵਾਪਸ OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    ਪੂਰਨ
socketserver_peek_ਜਵਾਬ(char_u *sender, char_u **str)
{
    ss_ਜਵਾਬ_T	*ਜਵਾਬ;
    char_u	*actual;

    ਜੇ (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_ਵਰਤਿਆ_str), sender);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (server_channel == ਨਿਰਾ || server_ਪਤਾ == ਨਿਰਾ)
    {
	emsg(_(e_socket_server_not_online));
	ਵਾਪਸ FAIL;
    }

    actual = socketserver_get_ਰਸਤਾ(sender, false, false, false, ਨਿਰਾ);
    ਜੇ (actual == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਜਵਾਬ = socketserver_get_ਜਵਾਬ(actual, ਨਿਰਾ);
    vim_free(actual);

    ਜੇ (ਜਵਾਬ != ਨਿਰਾ && ਜਵਾਬ->ਲੜੀਆਂ.ga_len > 0)
    {
	ਜੇ (str != ਨਿਰਾ)
	    *str = ((char_u **)ਜਵਾਬ->ਲੜੀਆਂ.ga_ਅੰਕੜੇ)[0];
	ਵਾਪਸ 1;
    }
    ਵਾਪਸ 0;
}

#ਜੇ_ਅੰਤ // FEAT_SOCKETSERVER
