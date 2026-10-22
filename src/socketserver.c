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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_SOCKETSERVER

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
రకనిర్వచనం నిర్మాణం
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T అక్షరశ్రేణులు;
} ss_జవాబు_T;

స్థిర channel_T    *server_channel = ఖాళీ;
స్థిర char_u	    *server_చిరునామా = ఖాళీ; // Includes "type:" prefix if any
స్థిర bool	    server_is_unix = false;
స్థిర char_u	    *server_addr_cwd = ఖాళీ; // CWD when server was started, used
					  // to handle relative file paths.
స్థిర garray_T	    server_replies;

స్థిర channel_T    *client_channels = ఖాళీ;
స్థిర పూర్ణాంకం	    client_channel_లెక్క = 0;

# define FOR_ALL_CLIENTS(ch) \
    కొరకు (ch = client_channels; ch != ఖాళీ; ch = ch->ch_ss_తదుపరి)

స్థిర శూన్య socketserver_cleanup(శూన్య);
స్థిర char_u *socketserver_create_మార్గం(char_u *name, bool quiet);
స్థిర char_u *socketserver_get_మార్గం(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
స్థిర శూన్య socketserver_accept(channel_T *channel);
స్థిర శూన్య socketserver_close(channel_T *channel);
స్థిర ss_జవాబు_T *socketserver_కూడు_జవాబు(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    పూర్ణాంకం
socketserver_ప్రారంభం(char_u *name, bool quiet)
{
    char_u	*చిరునామా = ఖాళీ;
    పూర్ణాంకం		పోర్టు;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    ఒకవేళ (server_channel != ఖాళీ)
	తిరిగి OK;

    ఒకవేళ (STRNICMP(name, "channel:", 8) == 0)
    {
	ఒకవేళ (channel_parse_socketserver_చిరునామా(name + 8, &పోర్టు, &చిరునామా, quiet)
		== FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (చిరునామా != ఖాళీ)
	    is_unix = true;
    }
    లేకపోతే
    {
	చిరునామా = socketserver_create_మార్గం(name, quiet);
	ఒకవేళ (చిరునామా == ఖాళీ)
	    తిరిగి FAIL;
	is_unix = true;
    }

    ఒకవేళ (is_unix)
	channel = channel_listen_unix((అక్షరం *)చిరునామా, ఖాళీ, false);
    లేకపోతే
	channel = channel_listen(పోర్టు, ఖాళీ);

    ఒకవేళ (channel == ఖాళీ)
    {
	vim_free(చిరునామా);
	తిరిగి FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    ఒకవేళ (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    లేకపోతే
    {
	buf = alloc(MAXPATHL + 1);
	ఒకవేళ (buf != ఖాళీ)
	{
	    buf[0] = NUL;
	    mch_నిండిన_Name(చిరునామా, buf, MAXPATHL, false);
	}
    }
    vim_free(చిరునామా);

    ఒకవేళ (buf != ఖాళీ)
    {
	server_చిరునామా = vim_strsave(buf);
	serverName = buf;
	సమితి_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    ఒకవేళ (mch_dirname(dirbuf, పరిమాణం(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    లేకపోతే
	server_addr_cwd = ఖాళీ;

    ga_init2(&server_replies, పరిమాణం(ss_జవాబు_T), 2);

    ch_log(ఖాళీ, "socketserver: started server at %s", name);

    తిరిగి OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    శూన్య
socketserver_stop(శూన్య)
{
    ఒకవేళ (server_channel == ఖాళీ)
	తిరిగి;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(ఖాళీ, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    స్థిర శూన్య
socketserver_cleanup(శూన్య)
{
# ifdef UNIX
    ఒకవేళ (server_is_unix && server_చిరునామా != ఖాళీ)
    {
	char_u *మార్గం = server_చిరునామా;
	char_u dirbuf[MAXPATHL];

	ఒకవేళ (STRNICMP(మార్గం, "channel:unix:", 13) == 0)
	    మార్గం += 13;
	లేకపోతే ఒకవేళ (STRNICMP(మార్గం, "name:", 5) == 0)
	    మార్గం += 5;

	ఒకవేళ (*మార్గం == '/')
	    mch_remove(మార్గం);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	లేకపోతే ఒకవేళ (server_addr_cwd != ఖాళీ
		&& mch_dirname(dirbuf, పరిమాణం(dirbuf)) == OK)
	{
	    ఒకవేళ (mch_chdir((అక్షరం *)server_addr_cwd) == 0)
	    {
		mch_remove(మార్గం);
		ఒకవేళ (mch_chdir((అక్షరం *)dirbuf) < 0)
		    emsg(_(e_cannot_go_వెనుక_గమ్యం_మునుపటి_directory));
	    }
	}
    }
# endif

    server_channel = ఖాళీ;
    VIM_CLEAR(server_చిరునామా);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    కొరకు (పూర్ణాంకం i = 0; i < server_replies.ga_len; i++)
    {
	ss_జవాబు_T *జవాబు = (ss_జవాబు_T *)server_replies.ga_దత్తాంశం + i;

	vim_free(జవాబు->sender);
	ga_clear_అక్షరశ్రేణులు(&జవాబు->అక్షరశ్రేణులు);
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
socketserver_list(శూన్య)
{
    list_T *list = list_alloc();

    ఒకవేళ (list == ఖాళీ)
	తిరిగి ఖాళీ;

# ifdef MSWIN
    // Only support addresses on Windows
    తిరిగి list;
# లేకపోతే
    string_T	    buf;
    string_T	    మార్గం;
    DIR		    *dirp;
    నిర్మాణం dirent   *dp;
    స్థిరరాశి char_u    *తెలిసిన_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ఒకవేళ ((buf.string = alloc(MAXPATHL)) == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ ((మార్గం.string = alloc(MAXPATHL)) == ఖాళీ)
    {
	vim_free(buf.string);
	తిరిగి ఖాళీ;
    }
    buf.నిడివి = 0;
    మార్గం.నిడివి = 0;

    కొరకు (size_t i = 0 ; i < ARRAY_నిడివి(తెలిసిన_dirs); i++)
    {
	స్థిరరాశి char_u *dir = తెలిసిన_dirs[i];

	ఒకవేళ (dir == ఖాళీ)
	    కొనసాగు;

	ఒకవేళ (STRCMP(dir, "/tmp") == 0 ||
		(తెలిసిన_dirs[1] != ఖాళీ && STRCMP(dir, తెలిసిన_dirs[1]) == 0))
	    మార్గం.నిడివి = vim_snprintf_safelen((అక్షరం *)మార్గం.string, MAXPATHL,
		    "%s/vim-%lu", dir, (గుర్తులేని దీర్ఘ పూర్ణాంకం)getuid());
	లేకపోతే
	    మార్గం.నిడివి = vim_snprintf_safelen((అక్షరం *)మార్గం.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = opendir((అక్షరం *)మార్గం.string);
	ఒకవేళ (dirp == ఖాళీ)
	    కొనసాగు;

	// Loop through directory
	అయితే ((dp = readdir(dirp)) != ఖాళీ)
	{
	    ఒకవేళ (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		కొనసాగు;

	    buf.నిడివి = vim_snprintf_safelen((అక్షరం *)buf.string, MAXPATHL,
		    "%s/%s", మార్గం.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.నిడివి - (మార్గం.నిడివి + 1));
	}

	closedir(dirp);

	ఆపు;
    }

    vim_free(మార్గం.string);
    vim_free(buf.string);

    తిరిగి list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    స్థిర char_u *
socketserver_create_మార్గం(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ఒకవేళ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	తిరిగి vim_strsave(name + 8);
    లేకపోతే
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_invalid_వాదం_str), name);
	తిరిగి ఖాళీ;
    }
# లేకపోతే
    char_u  *buf = ఖాళీ;
    పూర్ణాంకం	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *మార్గం = ఖాళీ;
    bool    fatal = false;

    ఒకవేళ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	తిరిగి vim_strsave(name + 8);
    ఒకవేళ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    కొరకు (పూర్ణాంకం i = 0; i < 1000; i++)
    {
	ఒకవేళ (buf != ఖాళీ)
	{
	    vim_snprintf((అక్షరం *)buf, buflen, "%s%d", name, i);
	    మార్గం = socketserver_get_మార్గం(buf, true, quiet, true, &fatal);
	}
	లేకపోతే
	    మార్గం = socketserver_get_మార్గం(name, true, quiet, true, &fatal);

	ఒకవేళ (fatal)
	    ఆపు;

	ఒకవేళ (మార్గం == ఖాళీ)
	{
	    ఒకవేళ (buf == ఖాళీ)
	    {
		buf = alloc(buflen);
		ఒకవేళ (buf == ఖాళీ)
		{
		    semsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం_allocating_nr_బైట్లు), buflen);
		    తిరిగి ఖాళీ;
		}
	    }
	    కొనసాగు;
	}
	ఆపు;
    }
    vim_free(buf);

    తిరిగి మార్గం;
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
    స్థిర char_u *
socketserver_get_మార్గం(
	char_u *name,
	bool new వాడని,
	bool quiet,
	bool ignore వాడని,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    ఒకవేళ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	తిరిగి vim_strsave(name + 8);
    లేకపోతే
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_invalid_వాదం_str), name);
	ఒకవేళ (fatal != ఖాళీ)
	    *fatal = true;
	తిరిగి ఖాళీ;
    }
# లేకపోతే
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    స్థిరరాశి char_u    *తెలిసిన_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    ఒకవేళ (name == ఖాళీ)
    {
	ఒకవేళ (fatal != ఖాళీ)
	    *fatal = true;
	తిరిగి ఖాళీ;
    }

    // Ignore if name is a path
    ఒకవేళ (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	తిరిగి vim_strsave(name);

    ఒకవేళ (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	తిరిగి vim_strsave(name + 8);

    ఒకవేళ (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    ఒకవేళ (vim_strchr(name, '/') != ఖాళీ)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	ఒకవేళ (fatal != ఖాళీ)
	    *fatal = true;
	తిరిగి ఖాళీ;
    }

    buf = alloc(MAXPATHL);

    ఒకవేళ (buf == ఖాళీ)
    {
	ఒకవేళ (fatal != ఖాళీ)
	    *fatal = true;
	semsg(_(e_బహిర్గతం_యొక్క_జ్ఞాపకం_allocating_nr_బైట్లు), MAXPATHL);
	తిరిగి ఖాళీ;
    }

    కొరకు (size_t i = 0; i < ARRAY_నిడివి(తెలిసిన_dirs); i++)
    {
	స్థిరరాశి char_u	*dir = తెలిసిన_dirs[i];
	bool		got = false;

	ఒకవేళ (dir == ఖాళీ)
	    కొనసాగు;
	లేకపోతే ఒకవేళ (STRCMP(dir, "/tmp") == 0 ||
		(తెలిసిన_dirs[1] != ఖాళీ && STRCMP(dir, తెలిసిన_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((అక్షరం *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (గుర్తులేని దీర్ఘ పూర్ణాంకం)getuid());
	    ఒకవేళ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		కొనసాగు;

	    vim_snprintf((అక్షరం *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (గుర్తులేని దీర్ఘ పూర్ణాంకం)getuid(), name);
	}
	లేకపోతే
	{
	    vim_snprintf((అక్షరం *)buf, MAXPATHL, "%s/vim", dir);
	    ఒకవేళ (vim_mkdir(buf, 0700) == -1 && errno != EEXIST)
		కొనసాగు;

	    vim_snprintf((అక్షరం *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	ఒకవేళ (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((అక్షరం *)buf, ఖాళీ);
	    emsg_off--;
	    emsg_silent--;

	    ఒకవేళ (channel != ఖాళీ)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    లేకపోతే
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	ఒకవేళ (got || (!new && mch_access((అక్షరం *)buf, F_OK) == 0))
	{
	    ఒకవేళ (server_చిరునామా != ఖాళీ && STRCMP(buf, server_చిరునామా) == 0)
		// Can't connect to itself
		ఆపు;
	    తిరిగి buf;
	}
	ఆపు;
    }

    ఒకవేళ (!quiet)
    {
	ఒకవేళ (!res)
	    semsg(_("Failed creating socket directory: %s"), strerror(errno));
	లేకపోతే ఒకవేళ (!ignore)
	    semsg(_(e_invalid_server_id_వాడిన_str), name);
    }
    ఒకవేళ (!res && fatal != ఖాళీ)
	*fatal = true;

    vim_free(buf);
    తిరిగి ఖాళీ;
# endif
}

/*
 * Callback for when client channel is closed
 */
    స్థిర శూన్య
socketserver_client_close(channel_T *channel)
{
    ఒకవేళ (channel == client_channels)
	client_channels = channel->ch_ss_తదుపరి;
    ఒకవేళ (channel->ch_ss_prev != ఖాళీ)
	channel->ch_ss_prev->ch_ss_తదుపరి = channel->ch_ss_తదుపరి;
    ఒకవేళ (channel->ch_ss_తదుపరి != ఖాళీ)
	channel->ch_ss_తదుపరి->ch_ss_prev = channel->ch_ss_prev;
    ఒకవేళ (client_channel_లెక్క > 0)
	--client_channel_లెక్క;

    ch_log(ఖాళీ, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    స్థిర శూన్య
socketserver_accept(channel_T *channel)
{
    ఒకవేళ (client_channel_లెక్క >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(ఖాళీ, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	తిరిగి;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_తదుపరి = ఖాళీ;
    ఒకవేళ (client_channels == ఖాళీ)
    {
	channel->ch_ss_prev = ఖాళీ;
	client_channels = channel;
    }
    లేకపోతే
    {
	channel_T *చివరి = client_channels;

	అయితే (చివరి->ch_ss_తదుపరి != ఖాళీ)
	    చివరి = చివరి->ch_ss_తదుపరి;
	చివరి->ch_ss_తదుపరి = channel;
	channel->ch_ss_prev = చివరి;
    }
    ++client_channel_లెక్క;

    // We will read the command from the client later in the input loop.
    ch_log(ఖాళీ, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    స్థిర శూన్య
socketserver_close(channel_T *channel వాడని)
{
    socketserver_cleanup();

    ch_log(ఖాళీ, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    పూర్ణాంకం
సమితి_ref_లోపల_socketserver_channel(పూర్ణాంకం copyID)
{
    bool	abort = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);

    FOR_ALL_CLIENTS(channel)
    {
	ఒకవేళ (abort)
	    ఆపు;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
    }
    తిరిగి abort;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    స్థిర పూర్ణాంకం
socketserver_send_సందేశం(channel_T *ch, dict_T *సందేశం, అక్షరం *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    పూర్ణాంకం		ret;

    ఒకవేళ (ver)
	dict_కూడు_సంఖ్య(సందేశం, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = సందేశం;
    buf = json_encode(&tv, JSON_NL);

    ఒకవేళ (buf != ఖాళీ)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (పూర్ణాంకం)STRLEN(buf), func);
	emsg_silent--;
    }
    లేకపోతే
	తిరిగి FAIL;
    vim_free(buf);
    తిరిగి ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    స్థిర శూన్య
socketserver_exec(channel_T *channel, dict_T *సందేశం_2)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = ఖాళీ;
    char_u	*sender = ఖాళీ;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_కనుగొను(సందేశం_2, (char_u *)"type", -1);
    ఒకవేళ (di == ఖాళీ || di->di_tv.v_type != VAR_STRING)
	తిరిగి;
    లేకపోతే
	type = di->di_tv.vval.v_string;

    di = dict_కనుగొను(సందేశం_2, (char_u *)"str", -1);
    ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_కనుగొను(సందేశం_2, (char_u *)"sender", -1);
    ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_కనుగొను(సందేశం_2, (char_u *)"wait", -1);
    ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_సంఖ్య)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((అక్షరం *)idbuf, పరిమాణం(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(ఖాళీ, "socketserver_exec(): result: %s",
	    str == ఖాళీ ? (char_u *)"(null)" : str);

    ఒకవేళ (STRCMP(type, "expr") == 0 && str != ఖాళీ)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *ఫలితం;
	పూర్ణాంకం	    సంకేతం_2;

	dict = dict_alloc();
	ఒకవేళ (dict == ఖాళీ)
	    తిరిగి;

	ఫలితం = eval_client_సమాసం_గమ్యం_string(str);
	సంకేతం_2 = ఫలితం == ఖాళీ ? -1 : 0;

	dict_కూడు_string(dict, "type", (char_u *)"reply");
	ఒకవేళ (ఫలితం != ఖాళీ)
	{
	    dict_కూడు_string(dict, "str", ఫలితం);
	    vim_free(ఫలితం);
	}
	లేకపోతే
	    // Error occurred, return error message
	    dict_కూడు_string(dict, "str",
		    (char_u *)_(e_invalid_వ్యక్తీకరణ_received));

	dict_కూడు_సంఖ్య(dict, "code", సంకేతం_2);

	socketserver_send_సందేశం(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    లేకపోతే ఒకవేళ (STRCMP(type, "keystrokes") == 0 && str != ఖాళీ)
    {
	// Execute keystrokes
	server_గమ్యం_ఆదానం_buf(str);
    }
    లేకపోతే ఒకవేళ (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	ఒకవేళ (sender != ఖాళీ && str != ఖాళీ)
	{
	    ss_జవాబు_T *జవాబు;

	    జవాబు = socketserver_కూడు_జవాబు(sender);

	    ఒకవేళ (జవాబు != ఖాళీ)
		ga_copy_string(&జవాబు->అక్షరశ్రేణులు, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    లేకపోతే ఒకవేళ (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_సంస్కరణ_mismatch));
    }
    లేకపోతే
	ch_error(ఖాళీ, "socketserver: unknown command type '%s'", type);
}

    స్థిర పూర్ణాంకం
socketserver_get_సందేశం(channel_T *channel, typval_T **tv)
{
    jsonq_T *తల = &channel->ch_part[PART_SOCK].ch_json_తల;
    jsonq_T *json_సందేశం = తల->jq_తదుపరి;
    పూర్ణాంకం	    ver;

    ఒకవేళ (json_సందేశం == ఖాళీ)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_సందేశం = తల->jq_తదుపరి;
    }
    ఒకవేళ (json_సందేశం == ఖాళీ)
	తిరిగి FAIL;
    *tv = json_సందేశం->jq_విలువ;
    remove_json_కణుపు(తల, json_సందేశం);

    ఒకవేళ ((*tv)->v_type != VAR_DICT)
    {
	ch_error(ఖాళీ, "socketserver: message is not a JSON object");
	free_tv(*tv);
	తిరిగి FAIL;
    }

    ver = dict_get_సంఖ్య((*tv)->vval.v_dict, "version");

    ఒకవేళ (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(ఖాళీ, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	ఒకవేళ (dict != ఖాళీ)
	{
	    dict_కూడు_string(dict, "type", (char_u *)"ver");
	    socketserver_send_సందేశం(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    శూన్య
socketserver_parse_messages(శూన్య)
{
    typval_T	*tv;

    కొరకు (channel_T *ch = client_channels; ch != ఖాళీ;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *తదుపరి = ch->ch_ss_తదుపరి;

	// Get the JSON message if there is any from the queue for this channel.
	ఒకవేళ (socketserver_get_సందేశం(ch, &tv) == FAIL)
	{
	    ch = తదుపరి;
	    కొనసాగు;
	}

	socketserver_exec(ch, tv->vval.v_dict);
	free_tv(tv);
	ch = తదుపరి;
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
    స్థిర పూర్ణాంకం
socketserver_wait(channel_T *channel, పూర్ణాంకం timeout)
{
    అయితే (true)
    {
	పూర్ణాంకం		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	నిర్మాణం timeval  tv;
	పూర్ణాంకం		maxfd = -1;

	ఒకవేళ (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	ఒకవేళ (channel != ఖాళీ)
	{
	    ఒకవేళ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		తిరిగి FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	ఒకవేళ (server_channel != ఖాళీ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    ఒకవేళ (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    ఒకవేళ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (పూర్ణాంకం)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		ఒకవేళ (maxfd < (పూర్ణాంకం)ch->CH_SOCK_FD)
		    maxfd = (పూర్ణాంకం)ch->CH_SOCK_FD;
	    }
	}

	ఒకవేళ (maxfd == -1)
	    తిరిగి FAIL;

	ret = select(maxfd + 1, &rfds, ఖాళీ, ఖాళీ, timeout == -1 ? ఖాళీ : &tv);

#  ifdef EINTR
	ఒకవేళ (ret == -1 && errno == EINTR)
	{
	    ఒకవేళ (got_int)
		ఆపు;
	    కొనసాగు;
	}
#  endif

	ఒకవేళ (ret > 0)
	{
	    ఒకవేళ (server_channel != ఖాళీ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ఒకవేళ (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (పూర్ణాంకం)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ఒకవేళ (channel == ఖాళీ)
		తిరిగి OK;

	    ఒకవేళ (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		తిరిగి OK;
	    }
	    కొనసాగు;
	}
# లేకపోతే
	నిర్మాణం pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	పూర్ణాంకం		nfd = 0;
	పూర్ణాంకం		channel_idx = -1;
	పూర్ణాంకం		server_idx = -1;

	ఒకవేళ (channel != ఖాళీ)
	{
	    ఒకవేళ (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		తిరిగి FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	ఒకవేళ (server_channel != ఖాళీ && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    ఒకవేళ (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	ఒకవేళ (ret == -1 && errno == EINTR)
	{
	    ఒకవేళ (got_int)
		ఆపు;
	    కొనసాగు;
	}
#  endif

	ఒకవేళ (ret > 0)
	{
	    ఒకవేళ (server_channel != ఖాళీ
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		ఒకవేళ (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    ఒకవేళ (channel == ఖాళీ)
		తిరిగి OK;

	    ఒకవేళ (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		తిరిగి OK;
	    }
	    కొనసాగు;
	}
# endif
	ఆపు;
    }
    తిరిగి FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    స్థిర channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *wait)
{
    char_u	*చిరునామా = ఖాళీ;
    పూర్ణాంకం		పోర్టు;
    bool	is_unix = false;
    channel_T	*channel;

    ఒకవేళ (STRNICMP(name, "channel:", 8) == 0)
    {
	ఒకవేళ (channel_parse_socketserver_చిరునామా(name + 8, &పోర్టు, &చిరునామా, true)
		== FAIL)
	    తిరిగి ఖాళీ;
	ఒకవేళ (చిరునామా != ఖాళీ)
	    is_unix = true;
    }
    లేకపోతే ఒకవేళ (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	పూర్ణాంకం id;

	ఒకవేళ (name[11] == NUL)
	    తిరిగి ఖాళీ;
	id = strtol((అక్షరం *)name + 11, ఖాళీ, 10);
	ఒకవేళ (wait != ఖాళీ)
	    *wait = true;

	తిరిగి channel_కనుగొను(id);
    }
    లేకపోతే
    {
	చిరునామా = socketserver_get_మార్గం(name, false, quiet, false, ఖాళీ);
	ఒకవేళ (చిరునామా == ఖాళీ)
	    తిరిగి ఖాళీ;
	is_unix = true;
    }

    ఒకవేళ (is_unix)
	channel = channel_open_unix((అక్షరం *)చిరునామా, ఖాళీ);
    లేకపోతే
	channel =  channel_open("localhost", పోర్టు, 1000, ఖాళీ);

    ఒకవేళ (channel == ఖాళీ && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(చిరునామా);

    తిరిగి channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    పూర్ణాంకం
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **ఫలితం,
	bool is_సమాసం,
	పూర్ణాంకం timeout,
	bool silent,
	channel_T **ch)
{
    పూర్ణాంకం		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = ఖాళీ;

    ఒకవేళ (*name == NUL)
    {
	semsg(_(e_unable_గమ్యం_send_గమ్యం_str), name);
	తిరిగి -1;
    }

    // Execute locally if target is ourselves
    ఒకవేళ (serverName != ఖాళీ && STRICMP(name, serverName) == 0)
	తిరిగి send_గమ్యం_Local_Vim(str, is_సమాసం, ఫలితం);

    channel = socketserver_get_channel(name, silent, ఖాళీ);
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి -1;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
	వెళ్ళు నిష్క్రమించు;

    dict_కూడు_string(dict, "type", (char_u *)(is_సమాసం ? "expr" : "keystrokes"));
    dict_కూడు_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    ఒకవేళ (server_చిరునామా != ఖాళీ)
	dict_కూడు_string(dict, "sender", server_చిరునామా);
    లేకపోతే ఒకవేళ (ch != ఖాళీ)
	dict_కూడు_bool(dict, "wait", true);

    ఒకవేళ (socketserver_send_సందేశం(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_గమ్యం_send_గమ్యం_str), name);
	dict_unref(dict);
	వెళ్ళు నిష్క్రమించు;
    }
    dict_unref(dict);

    ఒకవేళ (!is_సమాసం)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	ఒకవేళ (ch != ఖాళీ)
	{
	    *ch = channel;

	    channel->ch_ss_తదుపరి = client_channels;
	    channel->ch_ss_prev = ఖాళీ;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    తిరిగి rcode;
	}
	వెళ్ళు నిష్క్రమించు;
    }

    ఒకవేళ (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    అయితే (socketserver_wait(channel, timeout) == OK)
	ఒకవేళ (socketserver_get_సందేశం(channel, &resp_tv) == OK)
	    ఆపు;

    ఒకవేళ (resp_tv == ఖాళీ)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	వెళ్ళు నిష్క్రమించు;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_కనుగొను(dict, (char_u *)"type", -1);
    ఒకవేళ (di == ఖాళీ || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(ఖాళీ, "socketserver: unknown reply type");
	free_tv(resp_tv);
	వెళ్ళు నిష్క్రమించు;
    }

    ఒకవేళ (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_సంస్కరణ_mismatch));
	free_tv(resp_tv);
	వెళ్ళు నిష్క్రమించు;
    }

    ఒకవేళ (ఫలితం != ఖాళీ)
    {
	di = dict_కనుగొను(dict, (char_u *)"str", -1);
	ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_STRING)
	    *ఫలితం = vim_strsave(di->di_tv.vval.v_string);
	లేకపోతే
	{
	    free_tv(resp_tv);
	    వెళ్ళు నిష్క్రమించు;
	}
    }

    di = dict_కనుగొను(dict, (char_u *)"code", -1);
    ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_సంఖ్య)
	rcode = di->di_tv.vval.v_సంఖ్య;

    free_tv(resp_tv);

నిష్క్రమించు:
    channel_close(channel, false);
    channel_clear(channel);
    తిరిగి rcode;
}

   స్థిర ss_జవాబు_T *
socketserver_get_జవాబు(char_u *sender, పూర్ణాంకం *సూచీ)
{
    కొరకు (పూర్ణాంకం i = 0; i < server_replies.ga_len; i++)
    {
	ss_జవాబు_T *జవాబు = ((ss_జవాబు_T *)server_replies.ga_దత్తాంశం) + i;

	ఒకవేళ (STRCMP(జవాబు->sender, sender) == 0)
	{
	    ఒకవేళ (సూచీ != ఖాళీ)
		*సూచీ = i;
	    తిరిగి జవాబు;
	}
    }
    తిరిగి ఖాళీ;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    స్థిర ss_జవాబు_T *
socketserver_కూడు_జవాబు(char_u *sender)
{
    ss_జవాబు_T *జవాబు;

    ఒకవేళ (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, పరిమాణం(ss_జవాబు_T), 1);

    జవాబు = socketserver_get_జవాబు(sender, ఖాళీ);

    ఒకవేళ (జవాబు == ఖాళీ && ga_grow(&server_replies, 1) == OK)
    {
	జవాబు = ((ss_జవాబు_T *)server_replies.ga_దత్తాంశం) + server_replies.ga_len++;

	జవాబు->sender = vim_strsave(sender);

	ఒకవేళ (జవాబు->sender == ఖాళీ)
	    తిరిగి ఖాళీ;

	ga_init2(&జవాబు->అక్షరశ్రేణులు, పరిమాణం(char_u *), 5);
    }

    తిరిగి జవాబు;
}

    స్థిర శూన్య
socketserver_remove_జవాబు(char_u *sender)
{
    పూర్ణాంకం సూచీ;
    ss_జవాబు_T *జవాబు = socketserver_get_జవాబు(sender, &సూచీ);

    ఒకవేళ (జవాబు != ఖాళీ)
    {
	ss_జవాబు_T  *arr = server_replies.ga_దత్తాంశం;
	పూర్ణాంకం	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(జవాబు->sender);
	ga_clear_అక్షరశ్రేణులు(&జవాబు->అక్షరశ్రేణులు);

	// Move all elements after the removed reply forward by one
	ఒకవేళ (len > 1)
	    mch_memmove(arr + సూచీ, arr + సూచీ + 1,
		    పరిమాణం(ss_జవాబు_T) * (len - సూచీ - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    పూర్ణాంకం
socketserver_send_జవాబు(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    పూర్ణాంకం		ret = OK;
    bool	wait = false;

    ఒకవేళ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_వాడిన_str), client);
	తిరిగి FAIL;
    }

    ఒకవేళ (server_channel == ఖాళీ || server_చిరునామా == ఖాళీ)
    {
	emsg(_(e_socket_server_not_online));
	తిరిగి FAIL;
    }

    channel = socketserver_get_channel(client, false, &wait);
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి FAIL;

    dict = dict_alloc();
    ఒకవేళ (dict == ఖాళీ)
    {
	ret = FAIL;
	వెళ్ళు నిష్క్రమించు;
    }

    dict_కూడు_string(dict, "type", (char_u *)"notify");
    dict_కూడు_string(dict, "str", str);
    ఒకవేళ (server_చిరునామా != ఖాళీ)
	dict_కూడు_string(dict, "sender", server_చిరునామా);

    ret = socketserver_send_సందేశం(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

నిష్క్రమించు:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    ఒకవేళ (!wait)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    తిరిగి ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    పూర్ణాంకం
socketserver_read_జవాబు(
	char_u *client,
	char_u **str,
	పూర్ణాంకం timeout,
	bool remotewait)
{
    ss_జవాబు_T	*జవాబు = ఖాళీ;
    char_u	*actual;

    ఒకవేళ (*client == NUL)
    {
	semsg(_(e_invalid_server_id_వాడిన_str), client);
	తిరిగి FAIL;
    }

    ఒకవేళ (!remotewait && (server_channel == ఖాళీ || server_చిరునామా == ఖాళీ))
    {
	emsg(_(e_socket_server_not_online));
	తిరిగి FAIL;
    }

    actual = socketserver_get_మార్గం(client, false, false, false, ఖాళీ);
    ఒకవేళ (actual == ఖాళీ)
	తిరిగి FAIL;

    అయితే (true)
    {
	జవాబు = socketserver_get_జవాబు(actual, ఖాళీ);
	ఒకవేళ (జవాబు != ఖాళీ)
	    ఆపు;
	ఒకవేళ (socketserver_wait(ఖాళీ, timeout) == FAIL)
	    ఆపు;
    }

    ఒకవేళ (జవాబు == ఖాళీ || జవాబు->అక్షరశ్రేణులు.ga_len == 0)
    {
	vim_free(actual);
	తిరిగి FAIL;
    }

    // Consume the string
    *str = ((char_u **)జవాబు->అక్షరశ్రేణులు.ga_దత్తాంశం)[0];

    ఒకవేళ (జవాబు->అక్షరశ్రేణులు.ga_len > 1)
	mch_memmove((char_u **)జవాబు->అక్షరశ్రేణులు.ga_దత్తాంశం,
		((char_u **)జవాబు->అక్షరశ్రేణులు.ga_దత్తాంశం) + 1,
		పరిమాణం(char_u *) * (జవాబు->అక్షరశ్రేణులు.ga_len - 1));
    జవాబు->అక్షరశ్రేణులు.ga_len--;

    ఒకవేళ (జవాబు->అక్షరశ్రేణులు.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_జవాబు(actual);

    vim_free(actual);

    తిరిగి OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    పూర్ణాంకం
socketserver_peek_జవాబు(char_u *sender, char_u **str)
{
    ss_జవాబు_T	*జవాబు;
    char_u	*actual;

    ఒకవేళ (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_వాడిన_str), sender);
	తిరిగి FAIL;
    }

    ఒకవేళ (server_channel == ఖాళీ || server_చిరునామా == ఖాళీ)
    {
	emsg(_(e_socket_server_not_online));
	తిరిగి FAIL;
    }

    actual = socketserver_get_మార్గం(sender, false, false, false, ఖాళీ);
    ఒకవేళ (actual == ఖాళీ)
	తిరిగి FAIL;

    జవాబు = socketserver_get_జవాబు(actual, ఖాళీ);
    vim_free(actual);

    ఒకవేళ (జవాబు != ఖాళీ && జవాబు->అక్షరశ్రేణులు.ga_len > 0)
    {
	ఒకవేళ (str != ఖాళీ)
	    *str = ((char_u **)జవాబు->అక్షరశ్రేణులు.ga_దత్తాంశం)[0];
	తిరిగి 1;
    }
    తిరిగి 0;
}

#అయితే_ముగింపు // FEAT_SOCKETSERVER
