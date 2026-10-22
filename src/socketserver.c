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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T ಅಕ್ಷರಸರಣಿಗಳು;
} ss_ಉತ್ತರ_T;

ಸ್ಥಿರ channel_T    *server_channel = ಖಾಲಿ;
ಸ್ಥಿರ char_u	    *server_ವಿಳಾಸ = ಖಾಲಿ; // Includes "type:" prefix if any
ಸ್ಥಿರ bool	    server_is_unix = false;
ಸ್ಥಿರ char_u	    *server_addr_cwd = ಖಾಲಿ; // CWD when server was started, used
					  // to handle relative file paths.
ಸ್ಥಿರ garray_T	    server_replies;

ಸ್ಥಿರ channel_T    *client_channels = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    client_channel_ಎಣಿಕೆ = 0;

# define FOR_ALL_CLIENTS(ch) \
    ಗಾಗಿ (ch = client_channels; ch != ಖಾಲಿ; ch = ch->ch_ss_ಮುಂದಿನ)

ಸ್ಥಿರ ಶೂನ್ಯ socketserver_cleanup(ಶೂನ್ಯ);
ಸ್ಥಿರ char_u *socketserver_create_ಮಾರ್ಗ(char_u *name, bool quiet);
ಸ್ಥಿರ char_u *socketserver_get_ಮಾರ್ಗ(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
ಸ್ಥಿರ ಶೂನ್ಯ socketserver_accept(channel_T *channel);
ಸ್ಥಿರ ಶೂನ್ಯ socketserver_close(channel_T *channel);
ಸ್ಥಿರ ss_ಉತ್ತರ_T *socketserver_ಕೂಡಿಸು_ಉತ್ತರ(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    ಪೂರ್ಣಾಂಕ
socketserver_ಆರಂಭ(char_u *name, bool quiet)
{
    char_u	*ವಿಳಾಸ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಪೋರ್ಟ್;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    ಆದರೆ (server_channel != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (STRNICMP(name, "channel:", 8) == 0)
    {
	ಆದರೆ (channel_parse_socketserver_ವಿಳಾಸ(name + 8, &ಪೋರ್ಟ್, &ವಿಳಾಸ, quiet)
		== FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (ವಿಳಾಸ != ಖಾಲಿ)
	    is_unix = true;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ವಿಳಾಸ = socketserver_create_ಮಾರ್ಗ(name, quiet);
	ಆದರೆ (ವಿಳಾಸ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	is_unix = true;
    }

    ಆದರೆ (is_unix)
	channel = channel_listen_unix((ಅಕ್ಷರ *)ವಿಳಾಸ, ಖಾಲಿ, false);
    ಇಲ್ಲದಿದ್ದರೆ
	channel = channel_listen(ಪೋರ್ಟ್, ಖಾಲಿ);

    ಆದರೆ (channel == ಖಾಲಿ)
    {
	vim_free(ವಿಳಾಸ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    ಆದರೆ (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	buf = alloc(MAXPATHL + 1);
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    buf[0] = NUL;
	    mch_ತುಂಬಿದ_Name(ವಿಳಾಸ, buf, MAXPATHL, false);
	}
    }
    vim_free(ವಿಳಾಸ);

    ಆದರೆ (buf != ಖಾಲಿ)
    {
	server_ವಿಳಾಸ = vim_strsave(buf);
	serverName = buf;
	ಗಣ_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    ಆದರೆ (mch_dirname(dirbuf, ಗಾತ್ರ(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    ಇಲ್ಲದಿದ್ದರೆ
	server_addr_cwd = ಖಾಲಿ;

    ga_init2(&server_replies, ಗಾತ್ರ(ss_ಉತ್ತರ_T), 2);

    ch_log(ಖಾಲಿ, "socketserver: started server at %s", name);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    ಶೂನ್ಯ
socketserver_stop(ಶೂನ್ಯ)
{
    ಆದರೆ (server_channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ಖಾಲಿ, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_cleanup(ಶೂನ್ಯ)
{
# ifdef UNIX
    ಆದರೆ (server_is_unix && server_ವಿಳಾಸ != ಖಾಲಿ)
    {
	char_u *ಮಾರ್ಗ = server_ವಿಳಾಸ;
	char_u dirbuf[MAXPATHL];

	ಆದರೆ (STRNICMP(ಮಾರ್ಗ, "channel:unix:", 13) == 0)
	    ಮಾರ್ಗ += 13;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(ಮಾರ್ಗ, "name:", 5) == 0)
	    ಮಾರ್ಗ += 5;

	ಆದರೆ (*ಮಾರ್ಗ == '/')
	    mch_remove(ಮಾರ್ಗ);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (server_addr_cwd != ಖಾಲಿ
		&& mch_dirname(dirbuf, ಗಾತ್ರ(dirbuf)) == OK)
	{
	    ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)server_addr_cwd) == 0)
	    {
		mch_remove(ಮಾರ್ಗ);
		ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)dirbuf) < 0)
		    emsg(_(e_cannot_go_ಹಿಂಭಾಗ_ಗಮ್ಯ_ಹಿಂದಿನ_directory));
	    }
	}
    }
# endif

    server_channel = ಖಾಲಿ;
    VIM_CLEAR(server_ವಿಳಾಸ);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < server_replies.ga_len; i++)
    {
	ss_ಉತ್ತರ_T *ಉತ್ತರ = (ss_ಉತ್ತರ_T *)server_replies.ga_ದತ್ತಾಂಶ + i;

	vim_free(ಉತ್ತರ->sender);
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು);
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

    ಆದರೆ (list == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

# ifdef MSWIN
    // Only support addresses on Windows
    ಹಿಂತಿರುಗಿ list;
# ಇಲ್ಲದಿದ್ದರೆ
    string_T	    buf;
    string_T	    ಮಾರ್ಗ;
    DIR		    *dirp;
    ರಚನೆ dirent   *dp;
    ಸ್ಥಿರಾಂಕ char_u    *ತಿಳಿದ_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ಆದರೆ ((buf.string = alloc(MAXPATHL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ ((ಮಾರ್ಗ.string = alloc(MAXPATHL)) == ಖಾಲಿ)
    {
	vim_free(buf.string);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    buf.ಉದ್ದಳತೆ = 0;
    ಮಾರ್ಗ.ಉದ್ದಳತೆ = 0;

    ಗಾಗಿ (size_t i = 0 ; i < ARRAY_ಉದ್ದಳತೆ(ತಿಳಿದ_dirs); i++)
    {
	ಸ್ಥಿರಾಂಕ char_u *dir = ತಿಳಿದ_dirs[i];

	ಆದರೆ (dir == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	ಆದರೆ (STRCMP(dir, "/tmp") == 0 ||
		(ತಿಳಿದ_dirs[1] != ಖಾಲಿ && STRCMP(dir, ತಿಳಿದ_dirs[1]) == 0))
	    ಮಾರ್ಗ.ಉದ್ದಳತೆ = vim_snprintf_safelen((ಅಕ್ಷರ *)ಮಾರ್ಗ.string, MAXPATHL,
		    "%s/vim-%lu", dir, (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ)getuid());
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮಾರ್ಗ.ಉದ್ದಳತೆ = vim_snprintf_safelen((ಅಕ್ಷರ *)ಮಾರ್ಗ.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((ಅಕ್ಷರ *)ಮಾರ್ಗ.string);
	ಆದರೆ (dirp == ಖಾಲಿ)
	    ಮುಂದುವರಿ;

	// Loop through directory
	ಆಗ ((dp = readdir(dirp)) != ಖಾಲಿ)
	{
	    ಆದರೆ (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		ಮುಂದುವರಿ;

	    buf.ಉದ್ದಳತೆ = vim_snprintf_safelen((ಅಕ್ಷರ *)buf.string, MAXPATHL,
		    "%s/%s", ಮಾರ್ಗ.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.ಉದ್ದಳತೆ - (ಮಾರ್ಗ.ಉದ್ದಳತೆ + 1));
	}

	closedir(dirp);

	ನಿಲ್ಲಿಸು;
    }

    vim_free(ಮಾರ್ಗ.string);
    vim_free(buf.string);

    ಹಿಂತಿರುಗಿ list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    ಸ್ಥಿರ char_u *
socketserver_create_ಮಾರ್ಗ(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ಆದರೆ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಹಿಂತಿರುಗಿ vim_strsave(name + 8);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_invalid_ವಾದ_str), name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
# ಇಲ್ಲದಿದ್ದರೆ
    char_u  *buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *ಮಾರ್ಗ = ಖಾಲಿ;
    bool    fatal = false;

    ಆದರೆ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಹಿಂತಿರುಗಿ vim_strsave(name + 8);
    ಆದರೆ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < 1000; i++)
    {
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    vim_snprintf((ಅಕ್ಷರ *)buf, buflen, "%s%d", name, i);
	    ಮಾರ್ಗ = socketserver_get_ಮಾರ್ಗ(buf, true, quiet, true, &fatal);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಮಾರ್ಗ = socketserver_get_ಮಾರ್ಗ(name, true, quiet, true, &fatal);

	ಆದರೆ (fatal)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ)
	{
	    ಆದರೆ (buf == ಖಾಲಿ)
	    {
		buf = alloc(buflen);
		ಆದರೆ (buf == ಖಾಲಿ)
		{
		    semsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ_allocating_nr_ಬೈಟುಗಳು), buflen);
		    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
		}
	    }
	    ಮುಂದುವರಿ;
	}
	ನಿಲ್ಲಿಸು;
    }
    vim_free(buf);

    ಹಿಂತಿರುಗಿ ಮಾರ್ಗ;
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
socketserver_get_ಮಾರ್ಗ(
	char_u *name,
	bool new ಬಳಸದ,
	bool quiet,
	bool ignore ಬಳಸದ,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ಆದರೆ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಹಿಂತಿರುಗಿ vim_strsave(name + 8);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_invalid_ವಾದ_str), name);
	ಆದರೆ (fatal != ಖಾಲಿ)
	    *fatal = true;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
# ಇಲ್ಲದಿದ್ದರೆ
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    ಸ್ಥಿರಾಂಕ char_u    *ತಿಳಿದ_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ಆದರೆ (name == ಖಾಲಿ)
    {
	ಆದರೆ (fatal != ಖಾಲಿ)
	    *fatal = true;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Ignore if name is a path
    ಆದರೆ (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	ಹಿಂತಿರುಗಿ vim_strsave(name);

    ಆದರೆ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	ಹಿಂತಿರುಗಿ vim_strsave(name + 8);

    ಆದರೆ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ಆದರೆ (vim_strchr(name, '/') != ಖಾಲಿ)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	ಆದರೆ (fatal != ಖಾಲಿ)
	    *fatal = true;
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    buf = alloc(MAXPATHL);

    ಆದರೆ (buf == ಖಾಲಿ)
    {
	ಆದರೆ (fatal != ಖಾಲಿ)
	    *fatal = true;
	semsg(_(e_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ_allocating_nr_ಬೈಟುಗಳು), MAXPATHL);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾಗಿ (size_t i = 0; i < ARRAY_ಉದ್ದಳತೆ(ತಿಳಿದ_dirs); i++)
    {
	ಸ್ಥಿರಾಂಕ char_u	*dir = ತಿಳಿದ_dirs[i];
	bool		got = false;

	ಆದರೆ (dir == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(dir, "/tmp") == 0 ||
		(ತಿಳಿದ_dirs[1] != ಖಾಲಿ && STRCMP(dir, ತಿಳಿದ_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((ಅಕ್ಷರ *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ)getuid());
	    ಆದರೆ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ಮುಂದುವರಿ;

	    vim_snprintf((ಅಕ್ಷರ *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ)getuid(), name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    vim_snprintf((ಅಕ್ಷರ *)buf, MAXPATHL, "%s/vim", dir);
	    ಆದರೆ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		ಮುಂದುವರಿ;

	    vim_snprintf((ಅಕ್ಷರ *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	ಆದರೆ (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((ಅಕ್ಷರ *)buf, ಖಾಲಿ);
	    emsg_off--;
	    emsg_silent--;

	    ಆದರೆ (channel != ಖಾಲಿ)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	ಆದರೆ (got || (!new && mch_access((ಅಕ್ಷರ *)buf, F_OK) == 0))
	{
	    ಆದರೆ (server_ವಿಳಾಸ != ಖಾಲಿ && STRCMP(buf, server_ವಿಳಾಸ) == 0)
		// Can't connect to itself
		ನಿಲ್ಲಿಸು;
	    ಹಿಂತಿರುಗಿ buf;
	}
	ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (!quiet)
    {
	ಆದರೆ (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ignore)
	    semsg(_(e_invalid_server_id_ಬಳಸಿದ_str), name);
    }
    ಆದರೆ (!res && fatal != ಖಾಲಿ)
	*fatal = true;

    vim_free(buf);
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
# endif
}

/*
 * Callback for when client channel is closed
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_client_close(channel_T *channel)
{
    ಆದರೆ (channel == client_channels)
	client_channels = channel->ch_ss_ಮುಂದಿನ;
    ಆದರೆ (channel->ch_ss_prev != ಖಾಲಿ)
	channel->ch_ss_prev->ch_ss_ಮುಂದಿನ = channel->ch_ss_ಮುಂದಿನ;
    ಆದರೆ (channel->ch_ss_ಮುಂದಿನ != ಖಾಲಿ)
	channel->ch_ss_ಮುಂದಿನ->ch_ss_prev = channel->ch_ss_prev;
    ಆದರೆ (client_channel_ಎಣಿಕೆ > 0)
	--client_channel_ಎಣಿಕೆ;

    ch_log(ಖಾಲಿ, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_accept(channel_T *channel)
{
    ಆದರೆ (client_channel_ಎಣಿಕೆ >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(ಖಾಲಿ, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	ಹಿಂತಿರುಗಿ;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_ಮುಂದಿನ = ಖಾಲಿ;
    ಆದರೆ (client_channels == ಖಾಲಿ)
    {
	channel->ch_ss_prev = ಖಾಲಿ;
	client_channels = channel;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	channel_T *ಕೊನೆಯ = client_channels;

	ಆಗ (ಕೊನೆಯ->ch_ss_ಮುಂದಿನ != ಖಾಲಿ)
	    ಕೊನೆಯ = ಕೊನೆಯ->ch_ss_ಮುಂದಿನ;
	ಕೊನೆಯ->ch_ss_ಮುಂದಿನ = channel;
	channel->ch_ss_prev = ಕೊನೆಯ;
    }
    ++client_channel_ಎಣಿಕೆ;

    // We will read the command from the client later in the input loop.
    ch_log(ಖಾಲಿ, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_close(channel_T *channel ಬಳಸದ)
{
    socketserver_cleanup();

    ch_log(ಖಾಲಿ, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_socketserver_channel(ಪೂರ್ಣಾಂಕ copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);

    FOR_ALL_CLIENTS(channel)
    {
	ಆದರೆ (abort)
	    ನಿಲ್ಲಿಸು;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    }
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
socketserver_send_ಸಂದೇಶ(channel_T *ch, dict_T *ಸಂದೇಶ, ಅಕ್ಷರ *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (ver)
	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(ಸಂದೇಶ, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = ಸಂದೇಶ;
    buf = json_encode(&tv, JSON_NL);

    ಆದರೆ (buf != ಖಾಲಿ)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (ಪೂರ್ಣಾಂಕ)STRLEN(buf), func);
	emsg_silent--;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;
    vim_free(buf);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_exec(channel_T *channel, dict_T *ಸಂದೇಶ_2)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ಖಾಲಿ;
    char_u	*sender = ಖಾಲಿ;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_ಕಂಡುಹಿಡಿ(ಸಂದೇಶ_2, (char_u *)"type", -1);
    ಆದರೆ (di == ಖಾಲಿ || di->di_tv.v_type != VAR_STRING)
	ಹಿಂತಿರುಗಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	type = di->di_tv.vval.v_string;

    di = dict_ಕಂಡುಹಿಡಿ(ಸಂದೇಶ_2, (char_u *)"str", -1);
    ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_ಕಂಡುಹಿಡಿ(ಸಂದೇಶ_2, (char_u *)"sender", -1);
    ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_ಕಂಡುಹಿಡಿ(ಸಂದೇಶ_2, (char_u *)"wait", -1);
    ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_ಸಂಖ್ಯೆ)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((ಅಕ್ಷರ *)idbuf, ಗಾತ್ರ(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ಖಾಲಿ, "socketserver_exec(): result: %s",
	    str == ಖಾಲಿ ? (char_u *)"(null)" : str);

    ಆದರೆ (STRCMP(type, "expr") == 0 && str != ಖಾಲಿ)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *ಫಲಿತಾಂಶ;
	ಪೂರ್ಣಾಂಕ	    ಸಂಕೇತ_2;

	dict = dict_alloc();
	ಆದರೆ (dict == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	ಫಲಿತಾಂಶ = eval_client_ಅಭಿವ್ಯಕ್ತಿ_ಗಮ್ಯ_string(str);
	ಸಂಕೇತ_2 = ಫಲಿತಾಂಶ == ಖಾಲಿ ? -1 : 0;

	dict_ಕೂಡಿಸು_string(dict, "type", (char_u *)"reply");
	ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ)
	{
	    dict_ಕೂಡಿಸು_string(dict, "str", ಫಲಿತಾಂಶ);
	    vim_free(ಫಲಿತಾಂಶ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    // Error occurred, return error message
	    dict_ಕೂಡಿಸು_string(dict, "str",
		    (char_u *)_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_received));

	dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "code", ಸಂಕೇತ_2);

	socketserver_send_ಸಂದೇಶ(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(type, "keystrokes") == 0 && str != ಖಾಲಿ)
    {
	// Execute keystrokes
	server_ಗಮ್ಯ_ಒಳಹರಿವು_buf(str);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	ಆದರೆ (sender != ಖಾಲಿ && str != ಖಾಲಿ)
	{
	    ss_ಉತ್ತರ_T *ಉತ್ತರ;

	    ಉತ್ತರ = socketserver_ಕೂಡಿಸು_ಉತ್ತರ(sender);

	    ಆದರೆ (ಉತ್ತರ != ಖಾಲಿ)
		ga_copy_string(&ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_ಆವೃತ್ತಿ_mismatch));
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ch_error(ಖಾಲಿ, "socketserver: unknown command type '%s'", type);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
socketserver_get_ಸಂದೇಶ(channel_T *channel, typval_T **tv)
{
    jsonq_T *ತಲೆ = &channel->ch_part[PART_SOCK].ch_json_ತಲೆ;
    jsonq_T *json_ಸಂದೇಶ = ತಲೆ->jq_ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ	    ver;

    ಆದರೆ (json_ಸಂದೇಶ == ಖಾಲಿ)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_ಸಂದೇಶ = ತಲೆ->jq_ಮುಂದಿನ;
    }
    ಆದರೆ (json_ಸಂದೇಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    *tv = json_ಸಂದೇಶ->jq_ಮೌಲ್ಯ;
    remove_json_ಗಂಟು(ತಲೆ, json_ಸಂದೇಶ);

    ಆದರೆ ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ಖಾಲಿ, "socketserver: message is not a JSON object");
	free_tv(*tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ver = dict_get_ಸಂಖ್ಯೆ((*tv)->vval.v_dict, "version");

    ಆದರೆ (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ಖಾಲಿ, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	ಆದರೆ (dict != ಖಾಲಿ)
	{
	    dict_ಕೂಡಿಸು_string(dict, "type", (char_u *)"ver");
	    socketserver_send_ಸಂದೇಶ(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    ಶೂನ್ಯ
socketserver_parse_messages(ಶೂನ್ಯ)
{
    typval_T	*tv;

    ಗಾಗಿ (channel_T *ch = client_channels; ch != ಖಾಲಿ;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *ಮುಂದಿನ = ch->ch_ss_ಮುಂದಿನ;

	// Get the JSON message if there is any from the queue for this channel.
	ಆದರೆ (socketserver_get_ಸಂದೇಶ(ch, &tv) == FAIL)
	{
	    ch = ಮುಂದಿನ;
	    ಮುಂದುವರಿ;
	}

	socketserver_exec(ch, tv->vval.v_dict);
	free_tv(tv);
	ch = ಮುಂದಿನ;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
socketserver_wait(channel_T *channel, ಪೂರ್ಣಾಂಕ timeout)
{
    ಆಗ (true)
    {
	ಪೂರ್ಣಾಂಕ		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	ರಚನೆ timeval  tv;
	ಪೂರ್ಣಾಂಕ		maxfd = -1;

	ಆದರೆ (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	ಆದರೆ (channel != ಖಾಲಿ)
	{
	    ಆದರೆ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ಹಿಂತಿರುಗಿ FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	ಆದರೆ (server_channel != ಖಾಲಿ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    ಆದರೆ (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    ಆದರೆ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (ಪೂರ್ಣಾಂಕ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		ಆದರೆ (maxfd < (ಪೂರ್ಣಾಂಕ)ch->CH_SOCK_FD)
		    maxfd = (ಪೂರ್ಣಾಂಕ)ch->CH_SOCK_FD;
	    }
	}

	ಆದರೆ (maxfd == -1)
	    ಹಿಂತಿರುಗಿ FAIL;

	ret = select(maxfd + 1, &rfds, ಖಾಲಿ, ಖಾಲಿ, timeout == -1 ? ಖಾಲಿ : &tv);

#  ifdef EINTR
	ಆದರೆ (ret == -1 && errno == EINTR)
	{
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	    ಮುಂದುವರಿ;
	}
#  endif

	ಆದರೆ (ret > 0)
	{
	    ಆದರೆ (server_channel != ಖಾಲಿ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ಆದರೆ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (ಪೂರ್ಣಾಂಕ)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ಆದರೆ (channel == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ OK;

	    ಆದರೆ (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		ಹಿಂತಿರುಗಿ OK;
	    }
	    ಮುಂದುವರಿ;
	}
# ಇಲ್ಲದಿದ್ದರೆ
	ರಚನೆ pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	ಪೂರ್ಣಾಂಕ		nfd = 0;
	ಪೂರ್ಣಾಂಕ		channel_idx = -1;
	ಪೂರ್ಣಾಂಕ		server_idx = -1;

	ಆದರೆ (channel != ಖಾಲಿ)
	{
	    ಆದರೆ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		ಹಿಂತಿರುಗಿ FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	ಆದರೆ (server_channel != ಖಾಲಿ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    ಆದರೆ (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	ಆದರೆ (ret == -1 && errno == EINTR)
	{
	    ಆದರೆ (got_int)
		ನಿಲ್ಲಿಸು;
	    ಮುಂದುವರಿ;
	}
#  endif

	ಆದರೆ (ret > 0)
	{
	    ಆದರೆ (server_channel != ಖಾಲಿ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ಆದರೆ (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ಆದರೆ (channel == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ OK;

	    ಆದರೆ (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		ಹಿಂತಿರುಗಿ OK;
	    }
	    ಮುಂದುವರಿ;
	}
# endif
	ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FAIL;
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
    char_u	*ವಿಳಾಸ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		ಪೋರ್ಟ್;
    bool	is_unix = false;
    channel_T	*channel;

    ಆದರೆ (STRNICMP(name, "channel:", 8) == 0)
    {
	ಆದರೆ (channel_parse_socketserver_ವಿಳಾಸ(name + 8, &ಪೋರ್ಟ್, &ವಿಳಾಸ, true)
		== FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (ವಿಳಾಸ != ಖಾಲಿ)
	    is_unix = true;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	ಪೂರ್ಣಾಂಕ id;

	ಆದರೆ (name[11] == NUL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	id = strtol((ಅಕ್ಷರ *)name + 11, ಖಾಲಿ, 10);
	ಆದರೆ (wait != ಖಾಲಿ)
	    *wait = true;

	ಹಿಂತಿರುಗಿ channel_ಕಂಡುಹಿಡಿ(id);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ವಿಳಾಸ = socketserver_get_ಮಾರ್ಗ(name, false, quiet, false, ಖಾಲಿ);
	ಆದರೆ (ವಿಳಾಸ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	is_unix = true;
    }

    ಆದರೆ (is_unix)
	channel = channel_open_unix((ಅಕ್ಷರ *)ವಿಳಾಸ, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	channel =  channel_open("localhost", ಪೋರ್ಟ್, 1000, ಖಾಲಿ);

    ಆದರೆ (channel == ಖಾಲಿ && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(ವಿಳಾಸ);

    ಹಿಂತಿರುಗಿ channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    ಪೂರ್ಣಾಂಕ
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **ಫಲಿತಾಂಶ,
	bool is_ಅಭಿವ್ಯಕ್ತಿ,
	ಪೂರ್ಣಾಂಕ timeout,
	bool silent,
	channel_T **ch)
{
    ಪೂರ್ಣಾಂಕ		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ಖಾಲಿ;

    ಆದರೆ (*name == NUL)
    {
	semsg(_(e_unable_ಗಮ್ಯ_send_ಗಮ್ಯ_str), name);
	ಹಿಂತಿರುಗಿ -1;
    }

    // Execute locally if target is ourselves
    ಆದರೆ (serverName != ಖಾಲಿ && STRICMP(name, serverName) == 0)
	ಹಿಂತಿರುಗಿ send_ಗಮ್ಯ_Local_Vim(str, is_ಅಭಿವ್ಯಕ್ತಿ, ಫಲಿತಾಂಶ);

    channel = socketserver_get_channel(name, silent, ಖಾಲಿ);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹೋಗು ನಿರ್ಗಮಿಸು;

    dict_ಕೂಡಿಸು_string(dict, "type", (char_u *)(is_ಅಭಿವ್ಯಕ್ತಿ ? "expr" : "keystrokes"));
    dict_ಕೂಡಿಸು_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    ಆದರೆ (server_ವಿಳಾಸ != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(dict, "sender", server_ವಿಳಾಸ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_bool(dict, "wait", true);

    ಆದರೆ (socketserver_send_ಸಂದೇಶ(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_ಗಮ್ಯ_send_ಗಮ್ಯ_str), name);
	dict_unref(dict);
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }
    dict_unref(dict);

    ಆದರೆ (!is_ಅಭಿವ್ಯಕ್ತಿ)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	ಆದರೆ (ch != ಖಾಲಿ)
	{
	    *ch = channel;

	    channel->ch_ss_ಮುಂದಿನ = client_channels;
	    channel->ch_ss_prev = ಖಾಲಿ;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    ಹಿಂತಿರುಗಿ rcode;
	}
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    ಆದರೆ (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    ಆಗ (socketserver_wait(channel, timeout) == OK)
	ಆದರೆ (socketserver_get_ಸಂದೇಶ(channel, &resp_tv) == OK)
	    ನಿಲ್ಲಿಸು;

    ಆದರೆ (resp_tv == ಖಾಲಿ)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"type", -1);
    ಆದರೆ (di == ಖಾಲಿ || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ಖಾಲಿ, "socketserver: unknown reply type");
	free_tv(resp_tv);
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    ಆದರೆ (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_ಆವೃತ್ತಿ_mismatch));
	free_tv(resp_tv);
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    ಆದರೆ (ಫಲಿತಾಂಶ != ಖಾಲಿ)
    {
	di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"str", -1);
	ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_STRING)
	    *ಫಲಿತಾಂಶ = vim_strsave(di->di_tv.vval.v_string);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    free_tv(resp_tv);
	    ಹೋಗು ನಿರ್ಗಮಿಸು;
	}
    }

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)"code", -1);
    ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
	rcode = di->di_tv.vval.v_ಸಂಖ್ಯೆ;

    free_tv(resp_tv);

ನಿರ್ಗಮಿಸು:
    channel_close(channel, false);
    channel_clear(channel);
    ಹಿಂತಿರುಗಿ rcode;
}

   ಸ್ಥಿರ ss_ಉತ್ತರ_T *
socketserver_get_ಉತ್ತರ(char_u *sender, ಪೂರ್ಣಾಂಕ *ಸೂಚ್ಯಂಕ)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < server_replies.ga_len; i++)
    {
	ss_ಉತ್ತರ_T *ಉತ್ತರ = ((ss_ಉತ್ತರ_T *)server_replies.ga_ದತ್ತಾಂಶ) + i;

	ಆದರೆ (STRCMP(ಉತ್ತರ->sender, sender) == 0)
	{
	    ಆದರೆ (ಸೂಚ್ಯಂಕ != ಖಾಲಿ)
		*ಸೂಚ್ಯಂಕ = i;
	    ಹಿಂತಿರುಗಿ ಉತ್ತರ;
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    ಸ್ಥಿರ ss_ಉತ್ತರ_T *
socketserver_ಕೂಡಿಸು_ಉತ್ತರ(char_u *sender)
{
    ss_ಉತ್ತರ_T *ಉತ್ತರ;

    ಆದರೆ (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, ಗಾತ್ರ(ss_ಉತ್ತರ_T), 1);

    ಉತ್ತರ = socketserver_get_ಉತ್ತರ(sender, ಖಾಲಿ);

    ಆದರೆ (ಉತ್ತರ == ಖಾಲಿ && ga_grow(&server_replies, 1) == OK)
    {
	ಉತ್ತರ = ((ss_ಉತ್ತರ_T *)server_replies.ga_ದತ್ತಾಂಶ) + server_replies.ga_len++;

	ಉತ್ತರ->sender = vim_strsave(sender);

	ಆದರೆ (ಉತ್ತರ->sender == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	ga_init2(&ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು, ಗಾತ್ರ(char_u *), 5);
    }

    ಹಿಂತಿರುಗಿ ಉತ್ತರ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
socketserver_remove_ಉತ್ತರ(char_u *sender)
{
    ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ;
    ss_ಉತ್ತರ_T *ಉತ್ತರ = socketserver_get_ಉತ್ತರ(sender, &ಸೂಚ್ಯಂಕ);

    ಆದರೆ (ಉತ್ತರ != ಖಾಲಿ)
    {
	ss_ಉತ್ತರ_T  *arr = server_replies.ga_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(ಉತ್ತರ->sender);
	ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು);

	// Move all elements after the removed reply forward by one
	ಆದರೆ (len > 1)
	    mch_memmove(arr + ಸೂಚ್ಯಂಕ, arr + ಸೂಚ್ಯಂಕ + 1,
		    ಗಾತ್ರ(ss_ಉತ್ತರ_T) * (len - ಸೂಚ್ಯಂಕ - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    ಪೂರ್ಣಾಂಕ
socketserver_send_ಉತ್ತರ(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    bool	wait = false;

    ಆದರೆ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_ಬಳಸಿದ_str), client);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (server_channel == ಖಾಲಿ || server_ವಿಳಾಸ == ಖಾಲಿ)
    {
	emsg(_(e_socket_server_not_online));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    dict = dict_alloc();
    ಆದರೆ (dict == ಖಾಲಿ)
    {
	ret = FAIL;
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    dict_ಕೂಡಿಸು_string(dict, "type", (char_u *)"notify");
    dict_ಕೂಡಿಸು_string(dict, "str", str);
    ಆದರೆ (server_ವಿಳಾಸ != ಖಾಲಿ)
	dict_ಕೂಡಿಸು_string(dict, "sender", server_ವಿಳಾಸ);

    ret = socketserver_send_ಸಂದೇಶ(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

ನಿರ್ಗಮಿಸು:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    ಆದರೆ (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    ಪೂರ್ಣಾಂಕ
socketserver_read_ಉತ್ತರ(
	char_u *client,
	char_u **str,
	ಪೂರ್ಣಾಂಕ timeout,
	bool remotewait)
{
    ss_ಉತ್ತರ_T	*ಉತ್ತರ = ಖಾಲಿ;
    char_u	*actual;

    ಆದರೆ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_ಬಳಸಿದ_str), client);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!remotewait && (server_channel == ಖಾಲಿ || server_ವಿಳಾಸ == ಖಾಲಿ))
    {
	emsg(_(e_socket_server_not_online));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    actual = socketserver_get_ಮಾರ್ಗ(client, false, false, false, ಖಾಲಿ);
    ಆದರೆ (actual == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆಗ (true)
    {
	ಉತ್ತರ = socketserver_get_ಉತ್ತರ(actual, ಖಾಲಿ);
	ಆದರೆ (ಉತ್ತರ != ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (socketserver_wait(ಖಾಲಿ, timeout) == FAIL)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (ಉತ್ತರ == ಖಾಲಿ || ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_len == 0)
    {
	vim_free(actual);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Consume the string
    *str = ((char_u **)ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_ದತ್ತಾಂಶ)[0];

    ಆದರೆ (ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_len > 1)
	mch_memmove((char_u **)ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_ದತ್ತಾಂಶ,
		((char_u **)ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_ದತ್ತಾಂಶ) + 1,
		ಗಾತ್ರ(char_u *) * (ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_len - 1));
    ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_len--;

    ಆದರೆ (ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_ಉತ್ತರ(actual);

    vim_free(actual);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    ಪೂರ್ಣಾಂಕ
socketserver_peek_ಉತ್ತರ(char_u *sender, char_u **str)
{
    ss_ಉತ್ತರ_T	*ಉತ್ತರ;
    char_u	*actual;

    ಆದರೆ (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_ಬಳಸಿದ_str), sender);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (server_channel == ಖಾಲಿ || server_ವಿಳಾಸ == ಖಾಲಿ)
    {
	emsg(_(e_socket_server_not_online));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    actual = socketserver_get_ಮಾರ್ಗ(sender, false, false, false, ಖಾಲಿ);
    ಆದರೆ (actual == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಉತ್ತರ = socketserver_get_ಉತ್ತರ(actual, ಖಾಲಿ);
    vim_free(actual);

    ಆದರೆ (ಉತ್ತರ != ಖಾಲಿ && ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_len > 0)
    {
	ಆದರೆ (str != ಖಾಲಿ)
	    *str = ((char_u **)ಉತ್ತರ->ಅಕ್ಷರಸರಣಿಗಳು.ga_ದತ್ತಾಂಶ)[0];
	ಹಿಂತಿರುಗಿ 1;
    }
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_SOCKETSERVER
