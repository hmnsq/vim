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

#अन्तर्भाव "vim.h"

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
प्रकारव्याख्या संरचना
{
    char_u *sender; // Includes "type:" prefix if any
    garray_T strings;
} ss_reply_T;

स्थिर channel_T    *server_channel = रिक्त;
स्थिर char_u	    *server_address = रिक्त; // Includes "type:" prefix if any
स्थिर bool	    server_is_unix = false;
स्थिर char_u	    *server_addr_cwd = रिक्त; // CWD when server was started, used
					  // to handle relative file paths.
स्थिर garray_T	    server_replies;

स्थिर channel_T    *client_channels = रिक्त;
स्थिर पूर्णांक	    client_channel_मोजणी = 0;

# define FOR_ALL_CLIENTS(ch) \
    साठी (ch = client_channels; ch != रिक्त; ch = ch->ch_ss_next)

स्थिर शून्य socketserver_cleanup(शून्य);
स्थिर char_u *socketserver_create_path(char_u *name, bool quiet);
स्थिर char_u *socketserver_get_path(char_u *name, bool new, bool quiet, bool ignore, bool *fatal);
स्थिर शून्य socketserver_accept(channel_T *channel);
स्थिर शून्य socketserver_close(channel_T *channel);
स्थिर ss_reply_T *socketserver_add_reply(char_u *sender);

/*
 * Start the socketserver using the given name. Returns OK on success and FAIL
 * on failure.
 */
    पूर्णांक
socketserver_start(char_u *name, bool quiet)
{
    char_u	*address = रिक्त;
    पूर्णांक		port;
    bool	is_unix = false;
    channel_T	*channel;
    char_u	*buf;
    char_u	dirbuf[MAXPATHL];

    जर (server_channel != रिक्त)
	परत OK;

    जर (STRNICMP(name, "channel:", 8) == 0)
    {
	जर (channel_parse_socketserver_address(name + 8, &port, &address, quiet)
		== FAIL)
	    परत FAIL;
	जर (address != रिक्त)
	    is_unix = true;
    }
    अन्यथा
    {
	address = socketserver_create_path(name, quiet);
	जर (address == रिक्त)
	    परत FAIL;
	is_unix = true;
    }

    जर (is_unix)
	channel = channel_listen_unix((अक्षर *)address, रिक्त, false);
    अन्यथा
	channel = channel_listen(port, रिक्त);

    जर (channel == रिक्त)
    {
	vim_free(address);
	परत FAIL;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_accept_cb = socketserver_accept;
    channel->ch_ss_close_cb = socketserver_close;

    server_channel = channel;
    server_is_unix = is_unix;

    VIM_CLEAR(serverName);

    जर (STRNICMP(name, "channel:", 8) == 0)
	buf = vim_strsave(name);
    अन्यथा
    {
	buf = alloc(MAXPATHL + 1);
	जर (buf != रिक्त)
	{
	    buf[0] = NUL;
	    mch_FullName(address, buf, MAXPATHL, false);
	}
    }
    vim_free(address);

    जर (buf != रिक्त)
    {
	server_address = vim_strsave(buf);
	serverName = buf;
	set_vim_var_string(VV_SEND_SERVER, serverName, -1);
    }

    vim_free(server_addr_cwd);
    जर (mch_dirname(dirbuf, आकार(dirbuf)) == OK)
	server_addr_cwd = vim_strsave(dirbuf);
    अन्यथा
	server_addr_cwd = रिक्त;

    ga_init2(&server_replies, आकार(ss_reply_T), 2);

    ch_log(रिक्त, "socketserver: started server at %s", name);

    परत OK;
}

/*
 * Stop running the socketserver if it is. Note that this does not stop Vim from
 * becoming a client.
 */
    शून्य
socketserver_stop(शून्य)
{
    जर (server_channel == रिक्त)
	परत;

    channel_close(server_channel, false);
    channel_clear(server_channel);

    socketserver_cleanup();

    ch_log(रिक्त, "socketserver: shutting down server");
}

/*
 * Cleanup server stuff. Do not close client channels, as those are not part of
 * the actual server.
 */
    स्थिर शून्य
socketserver_cleanup(शून्य)
{
# ifdef UNIX
    जर (server_is_unix && server_address != रिक्त)
    {
	char_u *path = server_address;
	char_u dirbuf[MAXPATHL];

	जर (STRNICMP(path, "channel:unix:", 13) == 0)
	    path += 13;
	अन्यथा जर (STRNICMP(path, "name:", 5) == 0)
	    path += 5;

	जर (*path == '/')
	    mch_remove(path);
	// Go to the directory where the server was started. This is to handle
	// when Vim changes directories and the servername is a relative path.
	अन्यथा जर (server_addr_cwd != रिक्त
		&& mch_dirname(dirbuf, आकार(dirbuf)) == OK)
	{
	    जर (mch_chdir((अक्षर *)server_addr_cwd) == 0)
	    {
		mch_remove(path);
		जर (mch_chdir((अक्षर *)dirbuf) < 0)
		    emsg(_(e_cannot_go_back_to_previous_directory));
	    }
	}
    }
# endif

    server_channel = रिक्त;
    VIM_CLEAR(server_address);
    VIM_CLEAR(server_addr_cwd);

    // Free all replies
    साठी (पूर्णांक i = 0; i < server_replies.ga_len; i++)
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
socketserver_list(शून्य)
{
    list_T *list = list_alloc();

    जर (list == रिक्त)
	परत रिक्त;

# ifdef MSWIN
    // Only support addresses on Windows
    परत list;
# अन्यथा
    string_T	    buf;
    string_T	    path;
    सूची		    *dirp;
    संरचना dirent   *dp;
    स्थिरांक char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    जर ((buf.string = alloc(MAXPATHL)) == रिक्त)
	परत रिक्त;
    जर ((path.string = alloc(MAXPATHL)) == रिक्त)
    {
	vim_free(buf.string);
	परत रिक्त;
    }
    buf.length = 0;
    path.length = 0;

    साठी (माप_प्रकार i = 0 ; i < ARRAY_LENGTH(known_dirs); i++)
    {
	स्थिरांक char_u *dir = known_dirs[i];

	जर (dir == रिक्त)
	    सुरूठेवा;

	जर (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != रिक्त && STRCMP(dir, known_dirs[1]) == 0))
	    path.length = vim_snprintf_safelen((अक्षर *)path.string, MAXPATHL,
		    "%s/vim-%lu", dir, (अचिन्हित दीर्घ पूर्णांक)getuid());
	अन्यथा
	    path.length = vim_snprintf_safelen((अक्षर *)path.string, MAXPATHL,
		    "%s/vim", dir);

	dirp = सूची_खोलो((अक्षर *)path.string);
	जर (dirp == रिक्त)
	    सुरूठेवा;

	// Loop through directory
	पर्यंत ((dp = सूची_पढ़ो(dirp)) != रिक्त)
	{
	    जर (STRCMP(dp->d_name, ".") == 0 || STRCMP(dp->d_name, "..") == 0)
		सुरूठेवा;

	    buf.length = vim_snprintf_safelen((अक्षर *)buf.string, MAXPATHL,
		    "%s/%s", path.string, dp->d_name);

	    list_append_string(list, (char_u *)dp->d_name,
		    buf.length - (path.length + 1));
	}

	बंद_सूची(dirp);

	थांबा;
    }

    vim_free(path.string);
    vim_free(buf.string);

    परत list;
# endif
}

/*
 * If "name" is a path (starts with a '/', './', or '../'), it is assumed to be
 * the path to the desired socket. If the socket path is already taken, append
 * an incrementing number to the path until we find a socket filename that can
 * be used. Returns alloced string or NULL on failure.
 */
    स्थिर char_u *
socketserver_create_path(char_u *name, bool quiet)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    जर (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	परत vim_strsave(name + 8);
    अन्यथा
    {
	जर (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	परत रिक्त;
    }
# अन्यथा
    char_u  *buf = रिक्त;
    पूर्णांक	    buflen = STRLEN(name) + NUMBUFLEN;
    char_u  *path = रिक्त;
    bool    fatal = false;

    जर (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	परत vim_strsave(name + 8);
    जर (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    साठी (पूर्णांक i = 0; i < 1000; i++)
    {
	जर (buf != रिक्त)
	{
	    vim_snprintf((अक्षर *)buf, buflen, "%s%d", name, i);
	    path = socketserver_get_path(buf, true, quiet, true, &fatal);
	}
	अन्यथा
	    path = socketserver_get_path(name, true, quiet, true, &fatal);

	जर (fatal)
	    थांबा;

	जर (path == रिक्त)
	{
	    जर (buf == रिक्त)
	    {
		buf = alloc(buflen);
		जर (buf == रिक्त)
		{
		    semsg(_(e_out_of_memory_allocating_nr_bytes), buflen);
		    परत रिक्त;
		}
	    }
	    सुरूठेवा;
	}
	थांबा;
    }
    vim_free(buf);

    परत path;
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
    स्थिर char_u *
socketserver_get_path(
	char_u *name,
	bool new UNUSED,
	bool quiet,
	bool ignore UNUSED,
	bool *fatal)
{
# ifdef MSWIN
    // Only support channel addresses on Windows
    जर (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	परत vim_strsave(name + 8);
    अन्यथा
    {
	जर (!quiet)
	    semsg(_(e_invalid_argument_str), name);
	जर (fatal != रिक्त)
	    *fatal = true;
	परत रिक्त;
    }
# अन्यथा
    char_u	    *buf;
    bool	    res = false;
    channel_T	    *channel;
    स्थिरांक char_u    *known_dirs[] = {
	mch_getenv("XDG_RUNTIME_DIR"),
	mch_getenv("TMPDIR"),
	(char_u *)"/tmp"
    };

    जर (name == रिक्त)
    {
	जर (fatal != रिक्त)
	    *fatal = true;
	परत रिक्त;
    }

    // Ignore if name is a path
    जर (name[0] == '/' || STRNCMP(name, "./", 2) == 0 ||
	    STRNCMP(name, "../", 3) == 0)
	परत vim_strsave(name);

    जर (STRNICMP(name, "channel:", 8) == 0 && STRLEN(name) > 8)
	परत vim_strsave(name + 8);

    जर (STRNICMP(name, "name:", 5) == 0)
	name += 5;

    जर (vim_strchr(name, '/') != रिक्त)
    {
	जर (!quiet)
	    semsg(_(e_socket_name_no_slashes), name);
	जर (fatal != रिक्त)
	    *fatal = true;
	परत रिक्त;
    }

    buf = alloc(MAXPATHL);

    जर (buf == रिक्त)
    {
	जर (fatal != रिक्त)
	    *fatal = true;
	semsg(_(e_out_of_memory_allocating_nr_bytes), MAXPATHL);
	परत रिक्त;
    }

    साठी (माप_प्रकार i = 0; i < ARRAY_LENGTH(known_dirs); i++)
    {
	स्थिरांक char_u	*dir = known_dirs[i];
	bool		got = false;

	जर (dir == रिक्त)
	    सुरूठेवा;
	अन्यथा जर (STRCMP(dir, "/tmp") == 0 ||
		(known_dirs[1] != रिक्त && STRCMP(dir, known_dirs[1]) == 0))
	{
	    // "/tmp" or $TMPDIR, must suffix dir with uid
	    vim_snprintf((अक्षर *)buf, MAXPATHL, "%s/vim-%lu", dir,
		    (अचिन्हित दीर्घ पूर्णांक)getuid());
	    जर (vim_mkdir(buf, 0700) == -1 && त्रुटि_सं != EEXIST)
		सुरूठेवा;

	    vim_snprintf((अक्षर *)buf, MAXPATHL, "%s/vim-%lu/%s", dir,
		    (अचिन्हित दीर्घ पूर्णांक)getuid(), name);
	}
	अन्यथा
	{
	    vim_snprintf((अक्षर *)buf, MAXPATHL, "%s/vim", dir);
	    जर (vim_mkdir(buf, 0700) == -1 && त्रुटि_सं != EEXIST)
		सुरूठेवा;

	    vim_snprintf((अक्षर *)buf, MAXPATHL, "%s/vim/%s", dir, name);
	}

	// If looking for a new socket path, and "buf" currently exists, check
	// if it is a dead socket, if it is then remove it.
	जर (new)
	{
	    emsg_silent++;
	    emsg_off++;
	    channel = channel_open_unix((अक्षर *)buf, रिक्त);
	    emsg_off--;
	    emsg_silent--;

	    जर (channel != रिक्त)
	    {
		channel_close(channel, false);
		channel_clear(channel);
	    }
	    अन्यथा
	    {
		mch_remove(buf);
		got = true;
	    }
	}

	res = true;
	जर (got || (!new && mch_access((अक्षर *)buf, F_OK) == 0))
	{
	    जर (server_address != रिक्त && STRCMP(buf, server_address) == 0)
		// Can't connect to itself
		थांबा;
	    परत buf;
	}
	थांबा;
    }

    जर (!quiet)
    {
	जर (!res)
	    semsg(_("Failed creating socket directory: %s"), म_त्रुटि(त्रुटि_सं));
	अन्यथा जर (!ignore)
	    semsg(_(e_invalid_server_id_used_str), name);
    }
    जर (!res && fatal != रिक्त)
	*fatal = true;

    vim_free(buf);
    परत रिक्त;
# endif
}

/*
 * Callback for when client channel is closed
 */
    स्थिर शून्य
socketserver_client_close(channel_T *channel)
{
    जर (channel == client_channels)
	client_channels = channel->ch_ss_next;
    जर (channel->ch_ss_prev != रिक्त)
	channel->ch_ss_prev->ch_ss_next = channel->ch_ss_next;
    जर (channel->ch_ss_next != रिक्त)
	channel->ch_ss_next->ch_ss_prev = channel->ch_ss_prev;
    जर (client_channel_मोजणी > 0)
	--client_channel_मोजणी;

    ch_log(रिक्त, "socketserver: client channel closed");
}

/*
 * Callback for when server channel accepted new client.
 */
    स्थिर शून्य
socketserver_accept(channel_T *channel)
{
    जर (client_channel_मोजणी >= MAX_CLIENT_CHANNELS)
    {
	// Too many client connections: refuse this one instead of letting the
	// channel's file descriptor overflow the select()/poll() sets.
	ch_log(रिक्त, "socketserver: too many clients, refusing connection");
	channel_close(channel, FALSE);
	channel_unref(channel);
	परत;
    }

    channel->ch_socketserver = true;
    channel->ch_ss_close_cb = socketserver_client_close;

    // Append the client, so that commands are processed in the order the
    // clients were accepted, not reversed.
    channel->ch_ss_next = रिक्त;
    जर (client_channels == रिक्त)
    {
	channel->ch_ss_prev = रिक्त;
	client_channels = channel;
    }
    अन्यथा
    {
	channel_T *last = client_channels;

	पर्यंत (last->ch_ss_next != रिक्त)
	    last = last->ch_ss_next;
	last->ch_ss_next = channel;
	channel->ch_ss_prev = last;
    }
    ++client_channel_मोजणी;

    // We will read the command from the client later in the input loop.
    ch_log(रिक्त, "socketserver: accepted new client");
}

/*
 * Callback for when server channel is closed
 */
    स्थिर शून्य
socketserver_close(channel_T *channel UNUSED)
{
    socketserver_cleanup();

    ch_log(रिक्त, "socketserver: server channel closed");
}

/*
 * Mark references to socketserver channels
 */
    पूर्णांक
set_ref_in_socketserver_channel(पूर्णांक copyID)
{
    bool	पात = false;
    channel_T	*channel;
    typval_T	tv;

    tv.v_type = VAR_CHANNEL;
    tv.vval.v_channel = server_channel;
    पात = पात || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);

    FOR_ALL_CLIENTS(channel)
    {
	जर (पात)
	    थांबा;

	tv.v_type = VAR_CHANNEL;
	tv.vval.v_channel = channel;
	पात = पात || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
    }
    परत पात;
}

/*
 * Serialize "msg" into a string and send it over the given channel "ch". Also
 * adds version header to "msg", if "ver" is true. Returns OK on success and
 * FAIL on failure.
 */
    स्थिर पूर्णांक
socketserver_send_message(channel_T *ch, dict_T *संदेश, अक्षर *func, bool ver)
{
    typval_T	tv;
    char_u	*buf;
    पूर्णांक		ret;

    जर (ver)
	dict_add_संख्या(संदेश, "version", PROTOCOL_VER);

    tv.v_type = VAR_DICT;
    tv.vval.v_dict = संदेश;
    buf = json_encode(&tv, JSON_NL);

    जर (buf != रिक्त)
    {
	emsg_silent++;
	ret = channel_send(ch, PART_SOCK, buf, (पूर्णांक)STRLEN(buf), func);
	emsg_silent--;
    }
    अन्यथा
	परत FAIL;
    vim_free(buf);
    परत ret;
}

/*
 * Execute the JSON message represented by "dict".
 */
    स्थिर शून्य
socketserver_exec(channel_T *channel, dict_T *message)
{
    dictitem_T	*di;
    char_u	*type;
    char_u	*str = रिक्त;
    char_u	*sender = रिक्त;
    char_u	idbuf[11 + NUMBUFLEN];

    di = dict_find(message, (char_u *)"type", -1);
    जर (di == रिक्त || di->di_tv.v_type != VAR_STRING)
	परत;
    अन्यथा
	type = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"str", -1);
    जर (di != रिक्त && di->di_tv.v_type == VAR_STRING)
	str = di->di_tv.vval.v_string;

    di = dict_find(message, (char_u *)"sender", -1);
    जर (di != रिक्त && di->di_tv.v_type == VAR_STRING)
    {
	sender = di->di_tv.vval.v_string;

	// Save in global
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    di = dict_find(message, (char_u *)"wait", -1);
    जर (di != रिक्त && di->di_tv.v_type == VAR_BOOL && di->di_tv.vval.v_संख्या)
    {
	// Client is not a server, but still wants a response later. Save the
	// ID of the channel connection that we will use to send back a response,
	vim_snprintf((अक्षर *)idbuf, आकार(idbuf), "remotewait:%d",
		channel->ch_id);

	sender = idbuf;
	vim_free(client_socket);
	client_socket = vim_strsave(sender);
    }

    ch_log(रिक्त, "socketserver_exec(): result: %s",
	    str == रिक्त ? (char_u *)"(null)" : str);

    जर (STRCMP(type, "expr") == 0 && str != रिक्त)
    {
	// Evaluate expression and send back reply
	dict_T	    *dict;
	char_u	    *result;
	पूर्णांक	    code;

	dict = dict_alloc();
	जर (dict == रिक्त)
	    परत;

	result = eval_client_अभिव्यक्ती_to_string(str);
	code = result == रिक्त ? -1 : 0;

	dict_add_string(dict, "type", (char_u *)"reply");
	जर (result != रिक्त)
	{
	    dict_add_string(dict, "str", result);
	    vim_free(result);
	}
	अन्यथा
	    // Error occurred, return error message
	    dict_add_string(dict, "str",
		    (char_u *)_(e_invalid_expression_received));

	dict_add_संख्या(dict, "code", code);

	socketserver_send_message(channel, dict, "socketserver_exec", true);
	dict_unref(dict);
    }
    अन्यथा जर (STRCMP(type, "keystrokes") == 0 && str != रिक्त)
    {
	// Execute keystrokes
	server_to_आदान_buf(str);
    }
    अन्यथा जर (STRCMP(type, "notify") == 0)
    {
	// Notification, execute autocommands and save the reply for later use
	जर (sender != रिक्त && str != रिक्त)
	{
	    ss_reply_T *reply;

	    reply = socketserver_add_reply(sender);

	    जर (reply != रिक्त)
		ga_copy_string(&reply->strings, str);

	    apply_autocmds(EVENT_REMOTEREPLY, sender, str, TRUE, curbuf);
	}
    }
    अन्यथा जर (STRCMP(type, "ver") == 0)
    {
	// A message we sent previously had the wrong version. Emit an error
	// message to alert the user.
	emsg(_(e_socket_server_version_mismatch));
    }
    अन्यथा
	ch_error(रिक्त, "socketserver: unknown command type '%s'", type);
}

    स्थिर पूर्णांक
socketserver_get_message(channel_T *channel, typval_T **tv)
{
    jsonq_T *head = &channel->ch_part[PART_SOCK].ch_json_head;
    jsonq_T *json_संदेश = head->jq_next;
    पूर्णांक	    ver;

    जर (json_संदेश == रिक्त)
    {
	// Check the readahead buffer
	channel_parse_json(channel, PART_SOCK, true);
	json_संदेश = head->jq_next;
    }
    जर (json_संदेश == रिक्त)
	परत FAIL;
    *tv = json_संदेश->jq_मूल्य;
    remove_json_गाठ(head, json_संदेश);

    जर ((*tv)->v_type != VAR_DICT)
    {
	ch_error(रिक्त, "socketserver: message is not a JSON object");
	free_tv(*tv);
	परत FAIL;
    }

    ver = dict_get_संख्या((*tv)->vval.v_dict, "version");

    जर (ver != 0 && ver != PROTOCOL_VER)
    {
	dict_T *dict;

	ch_error(रिक्त, "socketserver: message has different version %d", ver);
	free_tv(*tv);

	// Send back a special error
	dict = dict_alloc();
	जर (dict != रिक्त)
	{
	    dict_add_string(dict, "type", (char_u *)"ver");
	    socketserver_send_message(channel, dict,
		    "socketserver_get_message", false);
	    dict_unref(dict);
	}
	परत FAIL;
    }

    परत OK;
}

/*
 * Parse any commands in the queue and execute them.
 */
    शून्य
socketserver_parse_messages(शून्य)
{
    typval_T	*tv;

    साठी (channel_T *ch = client_channels; ch != रिक्त;)
    {
	// Make sure to save next channel in case "ch" is freed. Not sure if
	// this can actually happen but be safe.
	channel_T *next = ch->ch_ss_next;

	// Get the JSON message if there is any from the queue for this channel.
	जर (socketserver_get_message(ch, &tv) == FAIL)
	{
	    ch = next;
	    सुरूठेवा;
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
    स्थिर पूर्णांक
socketserver_wait(channel_T *channel, पूर्णांक timeout)
{
    पर्यंत (true)
    {
	पूर्णांक		ret;
	channel_T	*ch;
# ifdef HAVE_SELECT
	fd_set		rfds;
	संरचना timeval  tv;
	पूर्णांक		maxfd = -1;

	जर (timeout != -1)
	{
	    tv.tv_sec = timeout / 1000;
	    tv.tv_usec = (timeout % 1000) * 1000;
	}

	FD_ZERO(&rfds);

	जर (channel != रिक्त)
	{
	    जर (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		परत FAIL;

	    maxfd = channel->CH_SOCK_FD;
	    FD_SET(channel->CH_SOCK_FD, &rfds);
	}
	जर (server_channel != रिक्त && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    FD_SET(server_channel->CH_SOCK_FD, &rfds);
	    जर (server_channel->CH_SOCK_FD > maxfd)
		maxfd = server_channel->CH_SOCK_FD;
	}

	FOR_ALL_CLIENTS(ch)
	{
	    जर (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
		    && (पूर्णांक)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
		    )
	    {
		FD_SET(ch->CH_SOCK_FD, &rfds);
		जर (maxfd < (पूर्णांक)ch->CH_SOCK_FD)
		    maxfd = (पूर्णांक)ch->CH_SOCK_FD;
	    }
	}

	जर (maxfd == -1)
	    परत FAIL;

	ret = select(maxfd + 1, &rfds, रिक्त, रिक्त, timeout == -1 ? रिक्त : &tv);

#  ifdef EINTR
	जर (ret == -1 && त्रुटि_सं == EINTR)
	{
	    जर (got_int)
		थांबा;
	    सुरूठेवा;
	}
#  endif

	जर (ret > 0)
	{
	    जर (server_channel != रिक्त
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(server_channel->CH_SOCK_FD, &rfds))
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		जर (ch->CH_SOCK_FD != INVALID_FD
#  ifdef FD_SETSIZE
			&& (पूर्णांक)ch->CH_SOCK_FD < FD_SETSIZE
#  endif
			&& FD_ISSET(ch->CH_SOCK_FD, &rfds))
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    जर (channel == रिक्त)
		परत OK;

	    जर (channel->CH_SOCK_FD != INVALID_FD
		    && FD_ISSET(channel->CH_SOCK_FD, &rfds))
	    {
		channel_check(channel, PART_SOCK);
		परत OK;
	    }
	    सुरूठेवा;
	}
# अन्यथा
	संरचना pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	पूर्णांक		nfd = 0;
	पूर्णांक		channel_idx = -1;
	पूर्णांक		server_idx = -1;

	जर (channel != रिक्त)
	{
	    जर (channel->CH_SOCK_FD == INVALID_FD)
		// Shouldn't happen
		परत FAIL;

	    channel_idx = nfd;
	    fds[nfd].fd = channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}
	जर (server_channel != रिक्त && server_channel->CH_SOCK_FD != INVALID_FD)
	{
	    server_idx = nfd;
	    fds[nfd].fd = server_channel->CH_SOCK_FD;
	    fds[nfd++].events = POLLIN;
	}

	FOR_ALL_CLIENTS(ch)
	    जर (ch->CH_SOCK_FD != INVALID_FD)
	    {
		fds[nfd].fd = ch->CH_SOCK_FD;
		fds[nfd].events = POLLIN;
		ch->ch_part[PART_SOCK].ch_poll_idx = nfd;
		nfd++;
	    }

	ret = poll(fds, nfd, timeout);

#  ifdef EINTR
	जर (ret == -1 && त्रुटि_सं == EINTR)
	{
	    जर (got_int)
		थांबा;
	    सुरूठेवा;
	}
#  endif

	जर (ret > 0)
	{
	    जर (server_channel != रिक्त
		    && server_channel->CH_SOCK_FD != INVALID_FD
		    && fds[server_idx].revents & POLLIN)
		channel_check(server_channel, PART_SOCK);

	    FOR_ALL_CLIENTS(ch)
		जर (ch->CH_SOCK_FD != INVALID_FD
			&& fds[ch->ch_part[PART_SOCK].ch_poll_idx]
			.revents & POLLIN)
		    channel_check(ch, PART_SOCK);

	    socketserver_parse_messages();

	    जर (channel == रिक्त)
		परत OK;

	    जर (channel->CH_SOCK_FD != INVALID_FD
		    && fds[channel_idx].revents & POLLIN)
	    {
		channel_check(channel, PART_SOCK);
		परत OK;
	    }
	    सुरूठेवा;
	}
# endif
	थांबा;
    }
    परत FAIL;
}

/*
 * Parse "name" and create or get the channel connection for it. If "wait" is
 * not NULL, then if the client name is a channel ID, then it will be set to
 * true. Returns NULL on
 * failure.
 */
    स्थिर channel_T *
socketserver_get_channel(char_u *name, bool quiet, bool *रुको)
{
    char_u	*address = रिक्त;
    पूर्णांक		port;
    bool	is_unix = false;
    channel_T	*channel;

    जर (STRNICMP(name, "channel:", 8) == 0)
    {
	जर (channel_parse_socketserver_address(name + 8, &port, &address, true)
		== FAIL)
	    परत रिक्त;
	जर (address != रिक्त)
	    is_unix = true;
    }
    अन्यथा जर (STRNICMP(name, "remotewait:", 11) == 0)
    {
	// Channel ID name, find channel with that ID.
	पूर्णांक id;

	जर (name[11] == NUL)
	    परत रिक्त;
	id = म_से_दीर्घ((अक्षर *)name + 11, रिक्त, 10);
	जर (रुको != रिक्त)
	    *रुको = true;

	परत channel_find(id);
    }
    अन्यथा
    {
	address = socketserver_get_path(name, false, quiet, false, रिक्त);
	जर (address == रिक्त)
	    परत रिक्त;
	is_unix = true;
    }

    जर (is_unix)
	channel = channel_open_unix((अक्षर *)address, रिक्त);
    अन्यथा
	channel =  channel_open("localhost", port, 1000, रिक्त);

    जर (channel == रिक्त && !quiet)
	semsg(_(e_socket_server_failed_connecting), name);

    vim_free(address);

    परत channel;
}

/*
 * Send command to address "name". If "ch" is not NULL, it is set to the channel
 * for the connection between us and the server, and the channel will not just
 * be closed immediately, this is used for --remote-wait. Returns 0 for OK, -1
 * on error.
 */
    पूर्णांक
socketserver_send(
	char_u *name,
	char_u *str,
	char_u **result,
	bool is_अभिव्यक्ती,
	पूर्णांक timeout,
	bool silent,
	channel_T **ch)
{
    पूर्णांक		rcode = -1;
    channel_T	*channel;
    dict_T	*dict;
    dictitem_T	*di;
    typval_T	*resp_tv = रिक्त;

    जर (*name == NUL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	परत -1;
    }

    // Execute locally if target is ourselves
    जर (serverName != रिक्त && STRICMP(name, serverName) == 0)
	परत sendToLocalVim(str, is_अभिव्यक्ती, result);

    channel = socketserver_get_channel(name, silent, रिक्त);
    जर (channel == रिक्त)
	परत -1;

    dict = dict_alloc();
    जर (dict == रिक्त)
	जा निकास;

    dict_add_string(dict, "type", (char_u *)(is_अभिव्यक्ती ? "expr" : "keystrokes"));
    dict_add_string(dict, "str", str);

    // Tell server who we are so it can save our socket path internally for
    // later use with server2client. Only do this if we are actually a server.
    //
    // If we are not a server, then --remote-wait will not work. To handle this
    // case, we add "wait" to the JSON message set to true, so that the server
    // will create an internal address for our connection to it.  This is
    // only used for --remote-wait, not exposed to user.
    जर (server_address != रिक्त)
	dict_add_string(dict, "sender", server_address);
    अन्यथा जर (ch != रिक्त)
	dict_add_bool(dict, "wait", true);

    जर (socketserver_send_message(channel, dict, "socketserver_send", true)
	    == FAIL)
    {
	semsg(_(e_unable_to_send_to_str), name);
	dict_unref(dict);
	जा निकास;
    }
    dict_unref(dict);

    जर (!is_अभिव्यक्ती)
    {
	// Exit, we aren't waiting for a response
	rcode = 0;

	जर (ch != रिक्त)
	{
	    *ch = channel;

	    channel->ch_ss_next = client_channels;
	    channel->ch_ss_prev = रिक्त;
	    channel->ch_ss_close_cb = socketserver_client_close;
	    client_channels = channel;

	    परत rcode;
	}
	जा निकास;
    }

    जर (timeout == 0)
	timeout = 1000;

    // To handle recursive calls, we must handle any socketserver channels as
    // well.
    पर्यंत (socketserver_wait(channel, timeout) == OK)
	जर (socketserver_get_message(channel, &resp_tv) == OK)
	    थांबा;

    जर (resp_tv == रिक्त)
    {
	// Channel closed, don't make this an error, because this may be the
	// result of the expression (e.g. --remote-expr 'execute("qa!")')
	rcode = 0;
	जा निकास;
    }

    dict = resp_tv->vval.v_dict;

    di = dict_find(dict, (char_u *)"type", -1);
    जर (di == रिक्त || di->di_tv.v_type != VAR_STRING ||
	    (STRCMP(di->di_tv.vval.v_string, "reply") != 0
	     && STRCMP(di->di_tv.vval.v_string, "ver") != 0))
    {
	ch_error(रिक्त, "socketserver: unknown reply type");
	free_tv(resp_tv);
	जा निकास;
    }

    जर (STRCMP(di->di_tv.vval.v_string, "ver") == 0)
    {
	emsg(_(e_socket_server_version_mismatch));
	free_tv(resp_tv);
	जा निकास;
    }

    जर (result != रिक्त)
    {
	di = dict_find(dict, (char_u *)"str", -1);
	जर (di != रिक्त && di->di_tv.v_type == VAR_STRING)
	    *result = vim_strsave(di->di_tv.vval.v_string);
	अन्यथा
	{
	    free_tv(resp_tv);
	    जा निकास;
	}
    }

    di = dict_find(dict, (char_u *)"code", -1);
    जर (di != रिक्त && di->di_tv.v_type == VAR_संख्या)
	rcode = di->di_tv.vval.v_संख्या;

    free_tv(resp_tv);

निकास:
    channel_close(channel, false);
    channel_clear(channel);
    परत rcode;
}

   स्थिर ss_reply_T *
socketserver_get_reply(char_u *sender, पूर्णांक *index)
{
    साठी (पूर्णांक i = 0; i < server_replies.ga_len; i++)
    {
	ss_reply_T *reply = ((ss_reply_T *)server_replies.ga_data) + i;

	जर (STRCMP(reply->sender, sender) == 0)
	{
	    जर (index != रिक्त)
		*index = i;
	    परत reply;
	}
    }
    परत रिक्त;
}

/*
 * Add reply to list of replies. Returns a pointer to the ss_reply_T that was
 * initialized or was found.
 */
    स्थिर ss_reply_T *
socketserver_add_reply(char_u *sender)
{
    ss_reply_T *reply;

    जर (server_replies.ga_growsize == 0)
	ga_init2(&server_replies, आकार(ss_reply_T), 1);

    reply = socketserver_get_reply(sender, रिक्त);

    जर (reply == रिक्त && ga_grow(&server_replies, 1) == OK)
    {
	reply = ((ss_reply_T *)server_replies.ga_data) + server_replies.ga_len++;

	reply->sender = vim_strsave(sender);

	जर (reply->sender == रिक्त)
	    परत रिक्त;

	ga_init2(&reply->strings, आकार(char_u *), 5);
    }

    परत reply;
}

    स्थिर शून्य
socketserver_remove_reply(char_u *sender)
{
    पूर्णांक index;
    ss_reply_T *reply = socketserver_get_reply(sender, &index);

    जर (reply != रिक्त)
    {
	ss_reply_T  *arr = server_replies.ga_data;
	पूर्णांक	    len	 = server_replies.ga_len;

	// Free strings
	vim_free(reply->sender);
	ga_clear_strings(&reply->strings);

	// Move all elements after the removed reply forward by one
	जर (len > 1)
	    mch_memmove(arr + index, arr + index + 1,
		    आकार(ss_reply_T) * (len - index - 1));
	server_replies.ga_len--;
    }
}

/*
 * Send a string to "client" as a reply (notification). Returns OK on success
 * and FAIL on failure.
 */
    पूर्णांक
socketserver_send_reply(char_u *client, char_u *str)
{
    dict_T	*dict;
    channel_T	*channel;
    पूर्णांक		ret = OK;
    bool	रुको = false;

    जर (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	परत FAIL;
    }

    जर (server_channel == रिक्त || server_address == रिक्त)
    {
	emsg(_(e_socket_server_not_online));
	परत FAIL;
    }

    channel = socketserver_get_channel(client, false, &रुको);
    जर (channel == रिक्त)
	परत FAIL;

    dict = dict_alloc();
    जर (dict == रिक्त)
    {
	ret = FAIL;
	जा निकास;
    }

    dict_add_string(dict, "type", (char_u *)"notify");
    dict_add_string(dict, "str", str);
    जर (server_address != रिक्त)
	dict_add_string(dict, "sender", server_address);

    ret = socketserver_send_message(channel, dict,
	    "socketserver_send_reply", true);
    dict_unref(dict);

निकास:
    // Don't want to close the channel if client is referenced by channel ID.
    // This allows --remote-wait to work with multiple files.
    जर (!रुको)
    {
	channel_close(channel, false);
	channel_clear(channel);
    }

    परत ret;
}

/*
 * Wait for reply from "client" and place result in "str". Returns OK on success
 * and FAIL on failure. Timeout is in milliseconds
 */
    पूर्णांक
socketserver_read_reply(
	char_u *client,
	char_u **str,
	पूर्णांक timeout,
	bool remotewait)
{
    ss_reply_T	*reply = रिक्त;
    char_u	*actual;

    जर (*client == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), client);
	परत FAIL;
    }

    जर (!remotewait && (server_channel == रिक्त || server_address == रिक्त))
    {
	emsg(_(e_socket_server_not_online));
	परत FAIL;
    }

    actual = socketserver_get_path(client, false, false, false, रिक्त);
    जर (actual == रिक्त)
	परत FAIL;

    पर्यंत (true)
    {
	reply = socketserver_get_reply(actual, रिक्त);
	जर (reply != रिक्त)
	    थांबा;
	जर (socketserver_wait(रिक्त, timeout) == FAIL)
	    थांबा;
    }

    जर (reply == रिक्त || reply->strings.ga_len == 0)
    {
	vim_free(actual);
	परत FAIL;
    }

    // Consume the string
    *str = ((char_u **)reply->strings.ga_data)[0];

    जर (reply->strings.ga_len > 1)
	mch_memmove((char_u **)reply->strings.ga_data,
		((char_u **)reply->strings.ga_data) + 1,
		आकार(char_u *) * (reply->strings.ga_len - 1));
    reply->strings.ga_len--;

    जर (reply->strings.ga_len < 1)
	// Last string removed, remove the reply
	socketserver_remove_reply(actual);

    vim_free(actual);

    परत OK;
}

/*
 * Check for any replies for "sender". Returns 1 if there is and places the
 * reply in "str" without consuming it (note that a copy is not created).
 * Returns 0 if otherwise and -1 on
 * error.
 */
    पूर्णांक
socketserver_peek_reply(char_u *sender, char_u **str)
{
    ss_reply_T	*reply;
    char_u	*actual;

    जर (*sender == NUL)
    {
	semsg(_(e_invalid_server_id_used_str), sender);
	परत FAIL;
    }

    जर (server_channel == रिक्त || server_address == रिक्त)
    {
	emsg(_(e_socket_server_not_online));
	परत FAIL;
    }

    actual = socketserver_get_path(sender, false, false, false, रिक्त);
    जर (actual == रिक्त)
	परत FAIL;

    reply = socketserver_get_reply(actual, रिक्त);
    vim_free(actual);

    जर (reply != रिक्त && reply->strings.ga_len > 0)
    {
	जर (str != रिक्त)
	    *str = ((char_u **)reply->strings.ga_data)[0];
	परत 1;
    }
    परत 0;
}

#endif // FEAT_SOCKETSERVER
