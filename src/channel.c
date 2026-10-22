/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Implements communication through a socket or any file handle.
 */

#include "vim.h"

#만약 defined(FEAT_JOB_CHANNEL)

// TRUE when netbeans is running with a GUI.
#ifdef FEAT_GUI
# define CH_HAS_GUI (gui.in_use || gui.starting)
#endif

// Note: when making changes here also adjust configure.ac.
#ifdef MSWIN
// WinSock API is separated from C API, thus we can't use read(), write(),
// errno...
# define SOCK_ERRNO errno = WSAGetLastError()
# undef ECONNREFUSED
# define ECONNREFUSED WSAECONNREFUSED
# undef EWOULDBLOCK
# define EWOULDBLOCK WSAEWOULDBLOCK
# undef EINPROGRESS
# define EINPROGRESS WSAEINPROGRESS
# ifdef EINTR
#  undef EINTR
# endif
# define EINTR WSAEINTR
# define sock_write(sd, buf, len) send((SOCKET)sd, buf, len, 0)
# define sock_read(sd, buf, len) recv((SOCKET)sd, buf, len, 0)
# define sock_close(sd) closesocket((SOCKET)sd)
// Support for Unix-domain sockets was added in Windows SDK 17061.
# define UNIX_PATH_MAX 108
typedef struct sockaddr_un {
    ADDRESS_FAMILY sun_family;
    char sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#아니면
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) write(sd, buf, len)
# define sock_read(sd, buf, len) read(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) read(fd, buf, len)
# define fd_write(sd, buf, len) write(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

static void channel_read(channel_T *channel, ch_part_T part, char *func);
static void channel_wait_any(long msec);
static ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
static 정수 channel_get_timeout(channel_T *channel, ch_part_T part);
static channel_T *channel_open_stdio(void);
static ch_part_T channel_part_send(channel_T *channel);
static ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    위해 ((ch) = first_channel; (ch) != NULL; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
static 정수 safe_to_invoke_callback = 0;

#ifdef MSWIN
    static 정수
fd_read(sock_T fd, char *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    만약 (!ReadFile(h, buf, (DWORD)len, &nread, NULL))
	반환 -1;
    반환 (정수)nread;
}

    static 정수
fd_write(sock_T fd, char *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    동안 (todo > 0)
    {
	만약 (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	아니면
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	만약 (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    만약 (err != ERROR_IO_PENDING)
		반환 -1;
	    만약 (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		반환 -1;
	    FlushFileBuffers(h);
	}
	아니면 만약 (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    break;
	todo -= nwrite;
	done += nwrite;
    }
    반환 (정수)done;
}

    static void
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(msg) (void)semsg("%s: %s", msg, strerror_win32(errno))

    static char *
strerror_win32(정수 eno)
{
    static LPVOID msgbuf = NULL;
    char_u *ptr;

    만약 (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = NULL;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	NULL,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	NULL);
    만약 (msgbuf != NULL)
	// chomp \r or \n
	위해 (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    switch (*ptr)
	    {
		case '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    break;
		case '\n':
		    만약 (*(ptr + 1) == '\0')
			*ptr = '\0';
		    아니면
			*ptr = ' ';
		    break;
	    }
    반환 msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
static channel_T *first_channel = NULL;
static 정수 next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(void)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    만약 (channel == NULL)
	반환 NULL;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    위해 (part = PART_SOCK; part < PART_COUNT; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_inputHandler = (XtInputId)NULL;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_inputHandler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    만약 (first_channel != NULL)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    반환 channel;
}

    정수
has_any_channel(void)
{
    반환 first_channel != NULL;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    정수
channel_still_useful(channel_T *channel)
{
    정수 has_sock_msg;
    정수	has_out_msg;
    정수	has_err_msg;

    // If the job was killed the channel is not expected to work anymore.
    만약 (channel->ch_job_killed && channel->ch_job == NULL)
	반환 FALSE;

    // If there is a close callback it may still need to be invoked.
    만약 (channel->ch_close_cb.cb_name != NULL)
	반환 TRUE;

    // If reading from or a buffer it's still useful.
    만약 (channel->ch_part[PART_IN].ch_bufref.br_buf != NULL)
	반환 TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_msg = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != NULL
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != NULL;
    has_out_msg = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != NULL
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != NULL;
    has_err_msg = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != NULL
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != NULL;
    반환 (channel->ch_callback.cb_name != NULL && (has_sock_msg
		|| has_out_msg || has_err_msg))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != NULL
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != NULL)
		    && has_out_msg)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != NULL
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != NULL)
		    && has_err_msg);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    정수
channel_can_close(channel_T *channel)
{
    반환 channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    static void
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    static void
channel_free_channel(channel_T *channel)
{
    만약 (channel->ch_next != NULL)
	channel->ch_next->ch_prev = channel->ch_prev;
    만약 (channel->ch_prev == NULL)
	first_channel = channel->ch_next;
    아니면
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    static void
channel_free(channel_T *channel)
{
    만약 (in_free_unref_items)
	반환;

    만약 (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    아니면
    {
	channel_free_contents(channel);
	channel_free_channel(channel);
    }
}

/*
 * Close a channel and free all its resources if there is no further action
 * possible, there is no callback to be invoked or the associated job was
 * killed.
 * Return TRUE if the channel was freed.
 */
    static 정수
channel_may_free(channel_T *channel)
{
    만약 (!channel_still_useful(channel))
    {
	channel_free(channel);
	반환 TRUE;
    }
    반환 FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    정수
channel_unref(channel_T *channel)
{
    만약 (channel != NULL && --channel->ch_refcount <= 0)
	반환 channel_may_free(channel);
    반환 FALSE;
}

    정수
free_unused_channels_contents(정수 copyID, 정수 mask)
{
    정수		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	만약 (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    반환 did_free;
}

    void
free_unused_channels(정수 copyID, 정수 mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    위해 (ch = first_channel; ch != NULL; ch = ch_next)
    {
	ch_next = ch->ch_next;
	만약 (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#만약 defined(FEAT_GUI)

# 만약 defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    static channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    만약 (fd == INVALID_FD)
	반환 NULL;

    FOR_ALL_CHANNELS(channel)
    {
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	    만약 (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		반환 channel;
	    }
    }
    반환 NULL;
}

    static void
channel_read_fd(정수 fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    만약 (channel == NULL)
	ch_error(NULL, "Channel for fd %d not found", fd);
    아니면
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    static void
messageFromServerX11(XtPointer clientData,
		  정수 *unused1 UNUSED,
		  XtInputId *unused2 UNUSED)
{
    channel_read_fd((정수)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  만약 GTK_CHECK_VERSION(3,0,0)
    static gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    반환 TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  아니면
    static void
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  GdkInputCondition unused2 UNUSED)
{
    channel_read_fd((정수)(long)clientData);
}
#  endif
# endif

    static void
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    만약 (!CH_HAS_GUI)
	반환;

    // gets stuck in handling events for a not connected channel
    만약 (channel->ch_keep_open)
	반환;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    만약 (channel->ch_part[part].ch_inputHandler == (XtInputId)NULL)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_inputHandler = XtAppAddInput(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(XtInputReadMask + XtInputExceptMask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# 아니면
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    만약 (channel->ch_part[part].ch_inputHandler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   만약 GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_inputHandler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   아니면
	channel->ch_part[part].ch_inputHandler = gdk_input_add(
		(gint)channel->ch_part[part].ch_fd,
		(GdkInputCondition)
			     ((정수)GDK_INPUT_READ + (정수)GDK_INPUT_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    static void
channel_gui_register(channel_T *channel)
{
    만약 (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    만약 (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    만약 (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    void
channel_gui_register_all(void)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    static void
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    만약 (channel->ch_part[part].ch_inputHandler != (XtInputId)NULL)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	XtRemoveInput(channel->ch_part[part].ch_inputHandler);
	channel->ch_part[part].ch_inputHandler = (XtInputId)NULL;
    }
# 아니면
#  ifdef FEAT_GUI_GTK
    만약 (channel->ch_part[part].ch_inputHandler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   만약 GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_inputHandler);
#   아니면
	gdk_input_remove(channel->ch_part[part].ch_inputHandler);
#   endif
	channel->ch_part[part].ch_inputHandler = 0;
    }
#  endif
# endif
}

    static void
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    위해 (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    static 정수
channel_connect(
	channel_T *channel,
	const struct sockaddr *server_addr,
	정수 server_addrlen,
	정수 *waittime)
{
    정수		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    동안 (TRUE)
    {
	long	elapsed_msec = 0;
	정수	waitnow;
	정수	ret;

	만약 (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	만약 (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    반환 -1;
	}

	만약 (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    만약 (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#아니면
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		반환 -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	만약 (ret == 0)
	    // The connection could be established.
	    break;

	SOCK_ERRNO;
	만약 (*waittime < 0 || (errno != EWOULDBLOCK
		&& errno != ECONNREFUSED
#ifdef EINPROGRESS
		&& errno != EINPROGRESS
#endif
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
	    PERROR(_(e_cannot_connect_to_port));
	    sock_close(sd);
	    반환 -1;
	}
	아니면 만약 (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    반환 -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    struct timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    정수			so_error = 0;
	    socklen_t		so_error_len = sizeof(so_error);
	    struct timeval	start_tv;
	    struct timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, NULL);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, NULL, &tv);
	    만약 (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		반환 -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    만약 (FD_ISSET(sd, &wfds))
		break;
	    elapsed_msec = waitnow;
	    만약 (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		continue;
	    }
#아니면
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    만약 (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		만약 (ret < 0 || (so_error != 0
			&& so_error != EWOULDBLOCK
			&& so_error != ECONNREFUSED
# ifdef EINPROGRESS
			&& so_error != EINPROGRESS
# endif
			))
		{
		    ch_error(channel,
			    "channel_connect: Connect failed with errno %d",
			    so_error);
		    PERROR(_(e_cannot_connect_to_port));
		    sock_close(sd);
		    반환 -1;
		}
		아니면 만약 (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    반환 -1;
		}
	    }

	    만약 (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		break;

	    gettimeofday(&end_tv, NULL);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	만약 (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    만약 (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    만약 (!got_int)
	    {
		만약 (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		continue;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	반환 -1;
    }

    만약 (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#아니면
	(void)fcntl(sd, F_SETFL, 0);
#endif
    }

    반환 sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	const char *path,
	void (*nb_close_cb)(void))
{
    channel_T		*channel = NULL;
    정수			sd = -1;
    size_t		path_len = STRLEN(path);
    struct sockaddr_un	server;
    size_t		server_len;
    정수			waittime = -1;

    만약 (*path == NUL || path_len >= sizeof(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	반환 NULL;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    만약 (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	반환 NULL;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, sizeof(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(struct sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (struct sockaddr *)&server, (정수)server_len,
								   &waittime);

    만약 (sd < 0)
    {
	channel_free(channel);
	반환 NULL;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (char *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    반환 channel;
}

/*
 * Open a socket channel to "hostname":"port".
 * "waittime" is the time in msec to wait for the connection.
 * When negative wait forever.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open(
	const char *hostname,
	정수 port,
	정수 waittime,
	void (*nb_close_cb)(void))
{
    정수			sd = -1;
    channel_T		*channel = NULL;
#ifdef FEAT_IPV6
    정수			err;
    struct addrinfo	hints;
    struct addrinfo	*res = NULL;
    struct addrinfo	*addr = NULL;
#아니면
    struct sockaddr_in	server;
    struct hostent	*host = NULL;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    만약 (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	반환 NULL;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# 만약 defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    만약 ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	반환 NULL;
    }

    // Count the number of addresses for timeout distribution
    정수 addr_count = 0;
    위해 (addr = res; addr != NULL; addr = addr->ai_next)
	addr_count++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    만약 (waittime == 0)
	waittime = 15;

    정수 addr_index = 0;
    위해 (addr = res; addr != NULL; addr = addr->ai_next, addr_index++)
    {
	const char  *dst = hostname;
# ifdef HAVE_INET_NTOP
	const void  *src = NULL;
	char	    buf[NUMBUFLEN];
# endif
	정수	    try_waittime;
	정수	    before_waittime;
	정수	    consumed;
	정수	    remaining_addrs;

	만약 (addr->ai_family == AF_INET6)
	{
	    struct sockaddr_in6 *sai = (struct sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	아니면 만약 (addr->ai_family == AF_INET)
	{
	    struct sockaddr_in *sai = (struct sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	만약 (src != NULL)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, sizeof(buf));
	    만약 (dst == NULL)
		dst = hostname;
	    아니면 만약 (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	만약 (addr->ai_next == NULL)
	    try_waittime = waittime;
	아니면 만약 (addr_index == 0)
	{
	    만약 (waittime > 500)
		try_waittime = 250;
	    아니면 만약 (waittime > 30)
		try_waittime = waittime / 2;
	    아니면
		try_waittime = waittime;
	}
	아니면
	{
	    remaining_addrs = addr_count - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (정수)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	만약 (waittime < 0)
	    waittime = 0;

	만약 (sd >= 0)
	    break;

	// If we have no time left, stop trying
	만약 (waittime <= 0 && addr->ai_next != NULL)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    break;
	}
    }

    freeaddrinfo(res);
#아니면
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    만약 ((host = gethostbyname(hostname)) == NULL)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	반환 NULL;
    }
    {
	char *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], sizeof(p));
	memcpy((char *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    만약 (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (struct sockaddr *)&server, sizeof(server),
								   &waittime);
#endif

    만약 (sd < 0)
    {
	channel_free(channel);
	반환 NULL;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (char *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    반환 channel;
}

    static void
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    만약 (callback->cb_name != NULL && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    아니면
	cbp->cb_name = NULL;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	static void
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_value_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_value_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    만약 (curbuf->b_ml.ml_mfp == NULL)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    static buf_T *
channel_find_buffer(char_u *name, 정수 err, 정수 msg)
{
    buf_T *buf = NULL;
    buf_T *save_curbuf = curbuf;

    만약 (name != NULL && *name != NUL)
    {
	buf = buflist_findname(name);
	만약 (buf == NULL)
	    buf = buflist_findname_exp(name);
    }

    만약 (buf != NULL)
	반환 buf;

    buf = buflist_new(name == NULL || *name == NUL ? NULL : name,
	    NULL, (linenr_T)0, BLN_LISTED | BLN_NEW);
    만약 (buf == NULL)
	반환 NULL;
    prepare_buffer(buf);

    curbuf = buf;
    만약 (msg)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    반환 buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    static void
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    만약 (opt->jo_set & JO_MODE)
	위해 (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    만약 (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    만약 (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    만약 (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    만약 (opt->jo_set & JO_TIMEOUT)
	위해 (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    만약 (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    만약 (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    만약 (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    만약 (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    만약 (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    만약 (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    만약 (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    만약 ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	만약 (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	만약 (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    만약 (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	아니면
	{
	    정수 msg = TRUE;

	    만약 (opt->jo_set2 & JO2_OUT_MSG)
		msg = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, msg);
	}
	만약 (buf != NULL)
	{
	    만약 (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    만약 (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    아니면
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (char *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		만약 (buf->b_ml.ml_mfp == NULL)
		    prepare_buffer(buf);
	    }
	}
    }

    만약 ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	만약 (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	만약 (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	아니면 만약 (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    만약 (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	아니면
	{
	    정수 msg = TRUE;

	    만약 (opt->jo_set2 & JO2_ERR_MSG)
		msg = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, msg);
	}
	만약 (buf != NULL)
	{
	    만약 (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    만약 (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    아니면
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (char *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		만약 (buf->b_ml.ml_mfp == NULL)
		    prepare_buffer(buf);
	    }
	}
    }

    channel->ch_part[PART_OUT].ch_io = opt->jo_io[PART_OUT];
    channel->ch_part[PART_ERR].ch_io = opt->jo_io[PART_ERR];
    channel->ch_part[PART_IN].ch_io = opt->jo_io[PART_IN];
}

/*
 * Parse the address for socketserver and store port in "port", or the path in
 * "unix_path" if it is a unix domain socket. Returns OK on success and FAIL on
 * failure.
 */
    정수
channel_parse_socketserver_address(
	char_u	*address,
	정수	*port,
	char_u	**unix_path,
	bool	quiet)
{
    char    *rest;
    long    val;

    만약 (*address == NUL)
	goto fail;

    만약 (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	반환 OK;
    }

    val = strtol((char *)(address), &rest, 10);
    만약 (val < 0 || val >= 65536 || *rest != NUL)
    {
	만약 (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	반환 FAIL;
    }
    *port = (정수)val;

    반환 OK;
fail:
    만약 (!quiet)
	semsg(_(e_invalid_argument_str), address);
    반환 FAIL;
}

/*
 * Implements ch_open().
 */
    static channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    char	*rest;
    정수		port = 0;
    정수		is_ipv6 = FALSE;
    정수		is_unix = FALSE;
    정수		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = NULL;

    만약 (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	반환 NULL;

    address = tv_get_string(&argvars[0]);
    만약 (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	반환 NULL;

    만약 (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	반환 NULL;
    }

    만약 (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    아니면 만약 (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    아니면 만약 (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	만약 (p == NULL || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    반환 NULL;
	}
    }
    아니면
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	만약 (p == NULL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    반환 NULL;
	}
    }

    만약 (!is_unix && !is_stdio)
    {
	port = strtol((char *)(p + 1), &rest, 10);
	만약 (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    반환 NULL;
	}
	만약 (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	아니면
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    만약 (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	goto theend;
    만약 (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }

    만약 (is_stdio)
	channel = channel_open_stdio();
    아니면 만약 (is_unix)
	channel = channel_open_unix((char *)address, NULL);
    아니면
	channel = channel_open((char *)address, port, opt.jo_waittime, NULL);
    만약 (channel != NULL)
    {
	정수	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	만약 (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    반환 channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
static channel_T *stdio_channel = NULL;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    static channel_T *
channel_open_stdio(void)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    만약 (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	반환 NULL;
    }
    만약 (stdio_channel != NULL)
    {
	emsg(_(e_cannot_open_stdio_channel));
	반환 NULL;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	정수	nul_fd;

	만약 (!DuplicateHandle(proc, GetStdHandle(STD_INPUT_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    반환 NULL;
	}
	만약 (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    반환 NULL;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
		NULL);
	만약 (hnul != INVALID_HANDLE_VALUE)
	{
	    SetStdHandle(STD_INPUT_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	만약 (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#아니면
    {
	정수	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	만약 (in_fd < 0 || out_fd < 0)
	{
	    만약 (in_fd >= 0)
		close(in_fd);
	    만약 (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    반환 NULL;
	}
	(void)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(void)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	만약 (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    만약 (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	반환 NULL;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    반환 channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    char	*rest;
    정수		port;
    정수		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = NULL;

    만약 (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	반환 NULL;

    arg = tv_get_string(&argvars[0]);
    만약 (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	반환 NULL;
    }

    만약 (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    아니면
    {
	port = strtol((char *)(arg), &rest, 10);
	만약 (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    반환 NULL;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    만약 (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	goto theend;
    만약 (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }

    만약 (is_unix)
	channel = channel_listen_unix((char *)arg, NULL, true);
    아니면
	channel = channel_listen(port, NULL);
    만약 (channel != NULL)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    반환 channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	정수 port_in,
	void (*nb_close_cb)(void))
{
    정수			sd = -1;
    struct sockaddr_in	server;
#ifndef FEAT_IPV6
    struct hostent	*host;
#endif
    정수			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    만약 (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	반환 NULL;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((char *)&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    만약 (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	반환 NULL;
    }

#ifdef MSWIN
    만약 (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (const char *)&val, sizeof(val)) < 0)
#아니면
    만약 (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, sizeof(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	반환 NULL;
    }

    // Bind the socket to the port
    만약 (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	반환 NULL;
    }

    만약 (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	반환 NULL;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    만약 (port_in == 0)
    {
	struct sockaddr_in	addr;
	socklen_t		addr_len = sizeof(addr);

	만약 (getsockname(sd, (struct sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (char *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    반환 channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	char *path,
	void (*nb_close_cb)(void),
	bool replace)
{
    정수			sd = -1;
    struct sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    만약 (path == NULL || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == NULL ? (char *)"" : path);
	반환 NULL;
    }

    path_len = STRLEN(path);
    만약 (path_len >= sizeof(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	반환 NULL;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    만약 (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	반환 NULL;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, sizeof(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    만약 (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	반환 NULL;
    }

    만약 (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(struct sockaddr_un, sun_path) + path_len + 1;
    만약 (bind(sd, (struct sockaddr *)&server, (정수)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	반환 NULL;
    }

    만약 (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	반환 NULL;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (char *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    반환 channel;
}

     void
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    만약 (*fd == INVALID_FD)
	반환;

    만약 (part == PART_SOCK)
	sock_close(*fd);
    아니면
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	만약 ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    만약 (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    void
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    만약 (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#만약 defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	만약 (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    만약 (out != INVALID_FD)
    {
#만약 defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#만약 defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    만약 (err != INVALID_FD)
    {
#만약 defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#만약 defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    void
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    만약 (job->jv_in_buf == NULL)
	반환;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (char *)in_part->ch_bufref.br_buf->b_ffname);
    만약 (options->jo_set & JO_IN_TOP)
    {
	만약 (options->jo_in_top == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_top =
		in_part->ch_bufref.br_buf->b_ml.ml_line_count + 1;
	}
	아니면
	    in_part->ch_buf_top = options->jo_in_top;
    }
    아니면
	in_part->ch_buf_top = 1;
    만약 (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    아니면
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_line_count;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    static void
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	정수	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    만약 (item == NULL)
	반환;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = NULL;
    만약 (item->cq_prev == NULL)
	head->cq_next = item;
    아니면
	item->cq_prev->cq_next = item;
}

    static void
write_buf_line(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *line = ml_get_buf(buf, lnum, FALSE);
    정수	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    정수	    i;

    // Need to make a copy to be able to append a NL.
    만약 ((p = alloc(len + 2)) == NULL)
	반환;
    memcpy((char *)p, (char *)line, len);

    만약 (channel->ch_write_text_mode)
	p[len] = CAR;
    아니면
    {
	위해 (i = 0; i < len; ++i)
	    만약 (p[i] == NL)
		p[i] = NUL;

	p[len] = NL;
    }
    p[len + 1] = NUL;
    channel_send(channel, PART_IN, p, len + 1, "write_buf_line");
    vim_free(p);
}

/*
 * Return TRUE if "channel" can be written to.
 * Returns FALSE if the input is closed or the write would block.
 */
    static 정수
can_write_buf_line(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    만약 (in_part->ch_fd == INVALID_FD)
	반환 FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    만약 (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    아니면 만약 (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# 만약 defined(HAVE_SELECT)
	struct timeval	tval;
	fd_set		wfds;
	정수		ret;

	FD_ZERO(&wfds);
	FD_SET((정수)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	위해 (;;)
	{
	    ret = select((정수)in_part->ch_fd + 1, NULL, &wfds, NULL, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    만약 (ret == -1 && errno == EINTR)
		continue;
#  endif
	    만약 (ret <= 0 || in_part->ch_block_write == 1)
	    {
		만약 (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		아니면
		    ch_log(channel, "Input not ready for writing");
		반환 FALSE;
	    }
	    break;
	}
# 아니면
	struct pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	만약 (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    반환 FALSE;
	}
	만약 (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    반환 FALSE;
	}
# endif
    }
#endif
    반환 TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    void
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    정수		written = 0;

    만약 (buf == NULL || in_part->ch_buf_append)
	반환;  // no buffer or using appending
    만약 (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == NULL)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = NULL;
	반환;
    }

    위해 (lnum = in_part->ch_buf_top; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_line_count; ++lnum)
    {
	만약 (!can_write_buf_line(channel))
	    break;
	write_buf_line(buf, lnum, channel);
	++written;
    }

    만약 (written == 1)
	ch_log(channel, "written line %d to channel", (정수)lnum - 1);
    아니면 만약 (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_top = lnum;
    만약 (lnum > buf->b_ml.ml_line_count || lnum > in_part->ch_buf_bot)
    {
#만약 defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	만약 (channel->ch_job != NULL)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = NULL;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	만약 (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    아니면
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_line_count - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    void
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	위해 (part = PART_SOCK; part < PART_COUNT; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    만약 (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = NULL;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    void
channel_write_input(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    만약 (in_part->ch_writeque.wq_next != NULL)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    아니면 만약 (in_part->ch_bufref.br_buf != NULL)
    {
	만약 (in_part->ch_buf_append)
	    channel_write_new_lines(in_part->ch_bufref.br_buf);
	아니면
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    void
channel_write_any_lines(void)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_input(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    void
channel_write_new_lines(buf_T *buf)
{
    channel_T	*channel;
    정수		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	정수	    written = 0;

	만약 (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    만약 (in_part->ch_fd == INVALID_FD)
		continue;  // pipe was closed
	    found_one = TRUE;
	    위해 (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_line_count;
								       ++lnum)
	    {
		만약 (!can_write_buf_line(channel))
		    break;
		write_buf_line(buf, lnum, channel);
		++written;
	    }

	    만약 (written == 1)
		ch_log(channel, "written line %d to channel", (정수)lnum - 1);
	    아니면 만약 (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    만약 (lnum < buf->b_ml.ml_line_count)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_line_count - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    만약 (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    static void
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    만약 (safe_to_invoke_callback == 0)
	iemsg("Invoking callback when it is not safe");

    argv[0].v_type = VAR_CHANNEL;
    argv[0].vval.v_channel = channel;

    call_callback(callback, -1, &rettv, 2, argv);
    clear_tv(&rettv);
    channel_need_redraw = TRUE;
}

/*
 * Return the first node from "channel"/"part" without removing it.
 * Returns NULL if there is nothing.
 */
    readq_T *
channel_peek(channel_T *channel, ch_part_T part)
{
    readq_T *head = &channel->ch_part[part].ch_head;

    반환 head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *node)
{
    char_u  *buffer = node->rq_buffer;
    long_u  i;

    위해 (i = 0; i < node->rq_buflen; ++i)
	만약 (buffer[i] == NL)
	    반환 buffer + i;
    반환 NULL;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, 정수 *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *p;

    만약 (node == NULL)
	반환 NULL;
    만약 (outlen != NULL)
	*outlen += node->rq_buflen;
    // dispose of the node but keep the buffer
    p = node->rq_buffer;
    head->rq_next = node->rq_next;
    만약 (node->rq_next == NULL)
	head->rq_prev = NULL;
    아니면
	node->rq_next->rq_prev = NULL;
    vim_free(node);
    반환 p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    static char_u *
channel_get_all(channel_T *channel, ch_part_T part, 정수 *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    위해 (node = head->rq_next; node != NULL; node = node->rq_next)
	len += node->rq_buflen;
    res = alloc(len + 1);
    만약 (res == NULL)
	반환 NULL;
    p = res;
    위해 (node = head->rq_next; node != NULL; node = node->rq_next)
    {
	mch_memmove(p, node->rq_buffer, node->rq_buflen);
	p += node->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    do
    {
	p = channel_get(channel, part, NULL);
	vim_free(p);
    } 동안 (p != NULL);

    만약 (outlen != NULL)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	반환 res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    동안 (p < res + len)
    {
	만약 (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	아니면 만약 (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    만약 (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		동안 (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		만약 (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    반환 res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    void
channel_consume(channel_T *channel, ch_part_T part, 정수 len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *buf = node->rq_buffer;

    만약 (len < 0 || (long_u)len > node->rq_buflen)
	반환;
    mch_memmove(buf, buf + len, node->rq_buflen - len);
    node->rq_buflen -= len;
    node->rq_buffer[node->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    정수
channel_collapse(channel_T *channel, ch_part_T part, 정수 want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*node = head->rq_next;
    readq_T	*last_node;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    만약 (node == NULL || node->rq_next == NULL)
	반환 FAIL;

    last_node = node->rq_next;
    len = node->rq_buflen + last_node->rq_buflen;
    만약 (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	동안 (last_node->rq_next != NULL
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_node) == NULL))
	{
	    last_node = last_node->rq_next;
	    len += last_node->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    만약 (newbuf == NULL)
	반환 FAIL;	    // out of memory
    mch_memmove(p, node->rq_buffer, node->rq_buflen);
    p += node->rq_buflen;
    vim_free(node->rq_buffer);
    node->rq_buffer = newbuf;
    위해 (n = node; n != last_node; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    node->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    위해 (n = node->rq_next; n != last_node; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    node->rq_next = last_node->rq_next;
    만약 (last_node->rq_next == NULL)
	head->rq_prev = node;
    아니면
	last_node->rq_next->rq_prev = node;
    vim_free(last_node);
    반환 OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    static 정수
channel_save(channel_T *channel, ch_part_T part, char_u *buf, 정수 len,
						      정수 prepend, char *lead)
{
    readq_T *node;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    정수	    i;

    node = ALLOC_ONE(readq_T);
    만약 (node == NULL)
	반환 FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    node->rq_buffer = alloc(len + 1);
    만약 (node->rq_buffer == NULL)
    {
	vim_free(node);
	반환 FAIL;	    // out of memory
    }

    만약 (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = node->rq_buffer;
	위해 (i = 0; i < len; ++i)
	    만약 (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	node->rq_buflen = (long_u)(p - node->rq_buffer);
    }
    아니면
    {
	mch_memmove(node->rq_buffer, buf, len);
	node->rq_buffer[len] = NUL;
	node->rq_buflen = (long_u)len;
    }

    만약 (prepend)
    {
	// prepend node to the head of the queue
	node->rq_next = head->rq_next;
	node->rq_prev = NULL;
	만약 (head->rq_next == NULL)
	    head->rq_prev = node;
	아니면
	    head->rq_next->rq_prev = node;
	head->rq_next = node;
    }
    아니면
    {
	// append node to the tail of the queue
	node->rq_next = NULL;
	node->rq_prev = head->rq_prev;
	만약 (head->rq_prev == NULL)
	    head->rq_next = node;
	아니면
	    head->rq_prev->rq_next = node;
	head->rq_prev = node;
    }

    만약 (ch_log_active() && lead != NULL)
	ch_log_literal(lead, channel, part, buf, len);

    반환 OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    정수
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, NULL);
    정수		keeplen;
    정수		addlen;
    char_u	*p;

    만약 (next == NULL)
	반환 FALSE;

    keeplen = reader->js_end - reader->js_buf;
    만약 (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (정수)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	만약 (p == NULL)
	{
	    vim_free(next);
	    반환 FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    반환 TRUE;
}

/*
 * Process the HTTP header in a Language Server Protocol (LSP) message or
 * Debug Adapter Protocol (DAP) message.
 *
 * The message format is described in the LSP specification:
 * https://microsoft.github.io/language-server-protocol/specification
 *
 * For DAP:
 * https://microsoft.github.io/debug-adapter-protocol/specification
 *
 * It has the following two fields:
 *
 *	Content-Length: ...
 *	Content-Type: application/vscode-jsonrpc; charset=utf-8
 *
 * For DAP, there is no "Content-Type" field (as of now).
 *
 * Each field ends with "\r\n". The header ends with an additional "\r\n".
 *
 * Returns OK if a valid header is received and FAIL if some fields in the
 * header are not correct. Returns MAYBE if a partial header is received and
 * need to wait for more data to arrive.
 */
    static 정수
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*line_start;
    char_u	*p;
    int_u	hdr_len;
    정수		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    동안 (TRUE)
    {
	line_start = p;
	동안 (*p != NUL && *p != '\n')
	    p++;
	만약 (*p == NUL)			// partial header
	    반환 MAYBE;
	p++;

	// process the content length field (if present)
	만약 ((p - line_start > 16)
		&& STRNICMP(line_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((char *)line_start + 16, NULL, 10);
	    만약 (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		반환 FAIL;
	}

	만약 ((p - line_start) == 2 && line_start[0] == '\r' &&
		line_start[1] == '\n')
	    // reached the empty line
	    break;
    }

    만약 (payload_len == -1)
	// Content-Length field is not present in the header
	반환 FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    만약 (jsbuf_len < hdr_len + payload_len)
	반환 MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    반환 OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    정수
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    정수		status = OK;
    정수		ret;

    만약 (channel_peek(channel, part) == NULL)
	반환 FALSE;

    reader.js_buf = channel_get(channel, part, NULL);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    만약 (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    만약 (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    만약 (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	만약 (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	아니면 만약 (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    만약 (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    아니면
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	아니면
	{
	    item = ALLOC_ONE(jsonq_T);
	    만약 (item == NULL)
		clear_tv(&listtv);
	    아니면
	    {
		item->jq_no_callback = FALSE;
		item->jq_value = alloc_tv();
		만약 (item->jq_value == NULL)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		아니면
		{
		    *item->jq_value = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = NULL;
		    만약 (item->jq_prev == NULL)
			head->jq_next = item;
		    아니면
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    만약 (status == OK)
	chanpart->ch_wait_len = 0;
    아니면 만약 (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	만약 (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (정수)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = GetTickCount() + 100L;
#아니면
	    gettimeofday(&chanpart->ch_deadline, NULL);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    만약 (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	아니면
	{
	    정수 timeout;
#ifdef MSWIN
	    timeout = (정수)(GetTickCount() - chanpart->ch_deadline) > 0;
#아니면
	    {
		struct timeval now_tv;

		gettimeofday(&now_tv, NULL);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    만약 (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    아니면
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    만약 (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    아니면 만약 (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(정수)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, NULL);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    아니면
	ret = FALSE;

    vim_free(reader.js_buf);
    반환 ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    static void
remove_cb_node(cbq_T *head, cbq_T *node)
{
    만약 (node->cq_prev == NULL)
	head->cq_next = node->cq_next;
    아니면
	node->cq_prev->cq_next = node->cq_next;
    만약 (node->cq_next == NULL)
	head->cq_prev = node->cq_prev;
    아니면
	node->cq_next->cq_prev = node->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    void
remove_json_node(jsonq_T *head, jsonq_T *node)
{
    만약 (node->jq_prev == NULL)
	head->jq_next = node->jq_next;
    아니면
	node->jq_prev->jq_next = node->jq_next;
    만약 (node->jq_next == NULL)
	head->jq_prev = node->jq_prev;
    아니면
	node->jq_next->jq_prev = node->jq_prev;
    vim_free(node);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    static void
channel_add_block_id(chanpart_T *chanpart, 정수 id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    만약 (gap->ga_growsize == 0)
	ga_init2(gap, sizeof(정수), 10);
    만약 (ga_grow(gap, 1) == OK)
    {
	((정수 *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    static void
channel_remove_block_id(chanpart_T *chanpart, 정수 id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    정수		i;

    위해 (i = 0; i < gap->ga_len; ++i)
	만약 (((정수 *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    만약 (i < gap->ga_len)
	    {
		정수 *p = ((정수 *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * sizeof(정수));
	    }
	    반환;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    static 정수
channel_has_block_id(chanpart_T *chanpart, 정수 id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    정수		i;

    위해 (i = 0; i < gap->ga_len; ++i)
	만약 (((정수 *)gap->ga_data)[i] == id)
	    반환 TRUE;
    반환 FALSE;
}

/*
 * Get a message from the JSON queue for channel "channel".
 * When "id" is positive it must match the first number in the list.
 * When "id" is zero or negative just get the first message.  But not one
 * in the ch_block_ids list.
 * When "without_callback" is TRUE also get messages that were pushed back.
 * Return OK when found and return the value in "rettv".
 * Return FAIL otherwise.
 */
    static 정수
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	정수	    id,
	정수	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    동안 (item != NULL)
    {
	list_T	    *l;
	typval_T    *tv;

	만약 (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_value->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	아니면
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    만약 (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		만약 (id > 0)
		{
		    만약 (item->jq_value->v_type != VAR_DICT)
			goto nextitem;
		    d = item->jq_value->vval.v_dict;
		    만약 (d == NULL)
			goto nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    만약 (dict_has_key(d, "method"))
			goto nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    만약 (di == NULL)
			goto nextitem;
		    tv = &di->di_tv;
		}
		아니면
		    tv = item->jq_value;
	    }
	    아니면
	    {
		만약 (id > 0)
		{
		    만약 (item->jq_value->v_type != VAR_DICT)
			goto nextitem;
		    d = item->jq_value->vval.v_dict;
		    만약 (d == NULL)
			goto nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    만약 (di == NULL)
			goto nextitem;
		    tv = &di->di_tv;
		}
		아니면
		    tv = item->jq_value;
	    }
	}

	만약 ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_NUMBER && tv->vval.v_number == id)
	      || (id <= 0 && (tv->v_type != VAR_NUMBER
		 || tv->vval.v_number == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_number)))))
	{
	    *rettv = item->jq_value;
	    만약 (tv->v_type == VAR_NUMBER)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_number);
	    remove_json_node(head, item);
	    반환 OK;
	}
nextitem:
	item = item->jq_next;
    }
    반환 FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    static void
channel_push_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    만약 (head->jq_prev != NULL && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = NULL;
    아니면 동안 (item != NULL && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    만약 (newitem == NULL)
    {
	clear_tv(rettv);
	반환;
    }

    newitem->jq_value = alloc_tv();
    만약 (newitem->jq_value == NULL)
    {
	vim_free(newitem);
	clear_tv(rettv);
	반환;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_value = *rettv;
    만약 (item == NULL)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = NULL;
	만약 (newitem->jq_prev == NULL)
	    head->jq_next = newitem;
	아니면
	    newitem->jq_prev->jq_next = newitem;
    }
    아니면
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	만약 (newitem->jq_next == NULL)
	    head->jq_prev = newitem;
	아니면
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    static void
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    정수	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    만약 (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	만약 (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	반환;
    }
    arg = argv[1].vval.v_string;
    만약 (arg == NULL)
	arg = (char_u *)"";

    만약 (STRCMP(cmd, "ex") == 0)
    {
	정수	called_emsg_before = called_emsg;
	char_u	*p = arg;
	정수	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (char *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	만약 (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	만약 (do_emsg_silent)
	    --emsg_silent;
	만약 (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (char *)get_vim_var_str(VV_ERRMSG));
    }
    아니면 만약 (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (char *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_count = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    아니면 만약 (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    아니면 만약 (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	정수 is_call = cmd[0] == 'c';
	정수 id_idx = is_call ? 3 : 2;

	만약 (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_NUMBER)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    만약 (p_verbose > 2)
		emsg(_(e_last_argument_for_expr_call_must_be_number));
	}
	아니면 만약 (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    만약 (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	아니면
	{
	    typval_T	*tv = NULL;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = NULL;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    만약 (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (char *)arg);
		tv = eval_expr(arg, NULL);
	    }
	    아니면
	    {
		ch_log(channel, "Calling '%s'", (char *)arg);
		만약 (func_call(arg, &argv[2], NULL, NULL, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    만약 (argv[id_idx].v_type == VAR_NUMBER)
	    {
		정수 id = argv[id_idx].vval.v_number;

		만약 (tv != NULL)
		    json = json_encode_nr_expr(id, tv, options | JSON_NL);
		만약 (tv == NULL || (json != NULL && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_expr(id, &err_tv, options | JSON_NL);
		}
		만약 (json != NULL)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (정수)STRLEN(json), (char *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    만약 (tv == &res_tv)
		clear_tv(tv);
	    아니면
		free_tv(tv);
	}
    }
    아니면 만약 (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (char *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    static void
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (char *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_node(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    static void
append_to_buffer(
    buf_T	*buffer,
    char_u	*msg,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_line_count;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    정수		save_p_ma = buffer->b_p_ma;
    정수		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    만약 (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	만약 (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	반환;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    만약 (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (정수)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    만약 (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	반환;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    만약 (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, msg, TRUE);
	lnum = 0;
    }
    아니면
	ml_append(lnum, msg, 0, FALSE);
    appended_lines_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    만약 (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    아니면
	buffer->b_p_ma = save_p_ma;

    만약 (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    만약 (wp->w_buffer == buffer)
	    {
		정수 move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		만약 (move_cursor || wp->w_topline > buffer->b_ml.ml_line_count)
		{
		    win_T *save_curwin = curwin;

		    만약 (move_cursor)
			++wp->w_cursor.lnum;
		    curwin = wp;
		    curbuf = curwin->w_buffer;
		    scroll_cursor_bot(0, FALSE);
		    curwin = save_curwin;
		    curbuf = curwin->w_buffer;
		}
	    }
	}
	redraw_buf_and_status_later(buffer, UPD_VALID);
	channel_need_redraw = TRUE;
    }

    만약 (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    만약 (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_line_count;
	}
    }
}

    static void
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *msg;

    동안 ((msg = channel_get(channel, part, NULL)) != NULL)
    {
	ch_log(channel, "Dropping message '%s'", (char *)msg);
	vim_free(msg);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    static 정수
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    반환 ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    static 정수
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*msg = NULL;
    blob_T	*blob = NULL;
    typval_T	*listtv = NULL;
    typval_T	argv[CH_JSON_MAX_ARGS];
    정수		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = NULL;
    buf_T	*buffer = NULL;
    char_u	*p;
    정수		called_otc;		// one time callbackup
    정수		raw_len = 0;

    만약 (channel->ch_nb_close_cb != NULL
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	반환 FALSE;

    // Use a message-specific callback, part callback or channel callback
    위해 (cbitem = cbhead->cq_next; cbitem != NULL; cbitem = cbitem->cq_next)
	만약 (cbitem->cq_seq_nr == 0)
	    break;
    만약 (cbitem != NULL)
	callback = &cbitem->cq_callback;
    아니면 만약 (ch_part->ch_callback.cb_name != NULL)
	callback = &ch_part->ch_callback;
    아니면 만약 (channel->ch_callback.cb_name != NULL)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    만약 (buffer != NULL && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == NULL))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = NULL;
	buffer = NULL;
    }

    만약 (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	정수		argc = 0;

	// Get any json message in the queue.
	만약 (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    만약 (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(void)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    만약 (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		반환 FALSE;
	}

	만약 (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    만약 (d != NULL)
	    {
		만약 (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		아니면
		    di = dict_find(d, (char_u *)"request_seq", -1);
		만약 (di != NULL && di->di_tv.v_type == VAR_NUMBER)
		    seq_nr = di->di_tv.vval.v_number;
	    }

	    argv[1] = *listtv;
	}
	아니면
	{
	    위해 (item = listtv->vval.v_list->lv_first;
		    item != NULL && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    동안 (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    만약 (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		반환 TRUE;
	    }

	    만약 (argv[0].v_type != VAR_NUMBER)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		반환 FALSE;
	    }
	    seq_nr = argv[0].vval.v_number;
	}
    }
    아니면 만약 (channel_peek(channel, part) == NULL)
    {
	// nothing to read on RAW or NL channel
	반환 FALSE;
    }
    아니면
    {
	// If there is no callback or buffer drop the message.
	만약 (callback == NULL && buffer == NULL)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    만약 (channel->ch_close_cb.cb_name == NULL && !channel->ch_drop_never)
		drop_messages(channel, part);
	    반환 FALSE;
	}

	만약 (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = NULL;
	    char_u  *buf;
	    readq_T *node;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    동안 (TRUE)
	    {
		node = channel_peek(channel, part);
		nl = channel_first_nl(node);
		만약 (nl != NULL)
		    break;
		만약 (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    만약 (ch_part->ch_fd == INVALID_FD && node->rq_buflen > 0)
			break;
		    반환 FALSE; // incomplete message
		}
	    }
	    buf = node->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    위해 (p = buf; (nl == NULL || p < nl)
					    && p < buf + node->rq_buflen; ++p)
		만약 (*p == NUL)
		    *p = NL;

	    만약 (nl == NULL)
	    {
		// get the whole buffer, drop the NL
		msg = channel_get(channel, part, NULL);
	    }
	    아니면 만약 (nl + 1 == buf + node->rq_buflen)
	    {
		// get the whole buffer
		msg = channel_get(channel, part, NULL);
		*nl = NUL;
	    }
	    아니면
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		msg = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (정수)(nl - buf) + 1);
	    }
	}
	아니면
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    msg = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : NULL);
	}

	만약 (msg == NULL)
	    반환 FALSE; // out of memory (and avoids Coverity warning)

	만약 (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    만약 (blob == NULL)
	    {
		vim_free(msg);
		반환 FALSE;
	    }
	    만약 (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(msg);
		반환 FALSE;
	    }
	    만약 (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, msg, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	아니면
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = msg;
	}
    }

    called_otc = FALSE;
    만약 (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	정수 lsp_req_msg = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	만약 ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_key(argv[1].vval.v_dict, "method"))
	    lsp_req_msg = TRUE;

	만약 (!lsp_req_msg)
	{
	    위해 (cbitem = cbhead->cq_next; cbitem != NULL;
		    cbitem = cbitem->cq_next)
	    {
		만약 (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    break;
		}
	    }
	}
    }

    만약 (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	만약 (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    만약 (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_push_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
		listtv = NULL;
	    }
	    아니면
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    아니면 만약 (callback != NULL || buffer != NULL)
    {
	만약 (buffer != NULL)
	{
	    만약 (msg == NULL)
		// JSON or JS mode: re-encode the message.
		msg = json_encode(listtv, ch_mode);
	    만약 (msg != NULL)
	    {
#ifdef FEAT_TERMINAL
		만약 (buffer->b_term != NULL)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    만약 (part == PART_ERR)
		    {
			char_u *cp;
			정수    extra = 0;

			위해 (cp = msg; *cp != NUL; ++cp)
			    만약 (*cp == NL && (cp == msg || cp[-1] != CAR))
				++extra;
			만약 (extra > 0)
			{
			    char_u *crlf_msg = alloc(STRLEN(msg) + extra + 1);

			    만약 (crlf_msg != NULL)
			    {
				char_u *q = crlf_msg;

				위해 (cp = msg; *cp != NUL; ++cp)
				{
				    만약 (*cp == NL && (cp == msg || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_msg, channel);
				vim_free(crlf_msg);
			    }
			}
			아니면
			    write_to_term(buffer, msg, channel);
		    }
		    아니면
			write_to_term(buffer, msg, channel);
		}
		아니면
#endif
		    append_to_buffer(buffer, msg, channel, part);
	    }
	}

	만약 (callback != NULL)
	{
	    만약 (cbitem != NULL)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    아니면
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (char *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    아니면
	ch_log(channel, "Dropping message %d", seq_nr);

    만약 (listtv != NULL)
	free_tv(listtv);
    만약 (blob != NULL)
	blob_unref(blob);
    vim_free(msg);

    반환 TRUE;
}

#만약 defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    정수
channel_can_write_to(channel_T *channel)
{
    반환 channel != NULL && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    정수
channel_is_open(channel_T *channel)
{
    반환 channel != NULL && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    static void *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    만약 (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	만약 (head->jq_next == NULL)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	반환 head->jq_next;
    }
    반환 channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    static 정수
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    반환 channel_readahead_pointer(channel, part) != NULL;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    static char *
channel_status(channel_T *channel, 정수 req_part)
{
    ch_part_T part;
    정수 has_readahead = FALSE;

    만약 (channel == NULL)
	 반환 "fail";
    만약 (req_part == PART_OUT)
    {
	만약 (channel->CH_OUT_FD != INVALID_FD)
	    반환 "open";
	만약 (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    아니면 만약 (req_part == PART_ERR)
    {
	만약 (channel->CH_ERR_FD != INVALID_FD)
	    반환 "open";
	만약 (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    아니면
    {
	만약 (channel_is_open(channel))
	    반환 "open";
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	    만약 (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		break;
	    }
    }

    만약 (has_readahead)
	반환 "buffered";
    반환 "closed";
}

    static void
channel_part_info(channel_T *channel, dict_T *dict, char *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    char	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    만약 (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    아니면 만약 (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    아니면
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (정수)s.length);

    STRCPY(namebuf + tail, "mode");
    switch (chanpart->ch_mode)
    {
	case CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    break;
	case CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    break;
	case CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    break;
	case CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    break;
	case CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    break;
	case CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    break;
	case CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    break;
	default:
	    STR_LITERAL_SET(s, "");
	    break;
    }
    dict_add_string_len(dict, namebuf, s.string, (정수)s.length);

    STRCPY(namebuf + tail, "io");
    만약 (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    아니면 switch (chanpart->ch_io)
    {
	case JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    break;
	case JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    break;
	case JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    break;
	case JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    break;
	case JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    break;
	default:
	    STR_LITERAL_SET(s, "");
	    break;
    }
    dict_add_string_len(dict, namebuf, s.string, (정수)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_number(dict, namebuf, chanpart->ch_timeout);
}

    static void
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_number(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    만약 (channel->ch_hostname != NULL)
    {
	만약 (channel->ch_port)
	    dict_add_number(dict, "port", channel->ch_port);
	아니면
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    아니면
    {
	channel_part_info(channel, dict, "out", PART_OUT);
	channel_part_info(channel, dict, "err", PART_ERR);
	channel_part_info(channel, dict, "in", PART_IN);
    }
}

/*
 * Close channel "channel".
 * Trigger the close callback if "invoke_close_cb" is TRUE.
 * Does not clear the buffers.
 */
    void
channel_close(channel_T *channel, 정수 invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    만약 (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	만약 (channel->ch_close_cb.cb_name != NULL)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	{
	    만약 (channel->ch_close_cb.cb_name != NULL
			    || channel->ch_part[part].ch_bufref.br_buf != NULL)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		만약 (channel->ch_close_cb.cb_name == NULL)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		동안 (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	만약 (channel->ch_close_cb.cb_name != NULL)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (char *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      만약 (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      만약 (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  위해 (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = NULL;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = NULL;
    channel->ch_ss_close_cb = NULL;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    static void
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    static void
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    만약 (wq->wq_next == NULL)
	wq->wq_prev = NULL;
    아니면
	wq->wq_next->wq_prev = NULL;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    static void
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    동안 (channel_peek(channel, part) != NULL)
	vim_free(channel_get(channel, part, NULL));

    동안 (cb_head->cq_next != NULL)
    {
	cbq_T *node = cb_head->cq_next;

	remove_cb_node(cb_head, node);
	free_callback(&node->cq_callback);
	vim_free(node);
    }

    동안 (json_head->jq_next != NULL)
    {
	free_tv(json_head->jq_next->jq_value);
	remove_json_node(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    동안 (ch_part->ch_writeque.wq_next != NULL)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    void
channel_clear(channel_T *channel)
{
    ch_log(channel, "Clearing channel");
    VIM_CLEAR(channel->ch_hostname);
    channel_clear_one(channel, PART_SOCK);
    channel_clear_one(channel, PART_OUT);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_IN);
    free_callback(&channel->ch_callback);
    free_callback(&channel->ch_close_cb);
}

#만약 defined(EXITFREE)
    void
channel_free_all(void)
{
    channel_T *channel;

    ch_log(NULL, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_MSG_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    static 정수
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    만약 (in_part->ch_writeque.wq_next != NULL)
	반환 TRUE;
    만약 (buf == NULL)
	반환 FALSE;
    반환 in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_line_count)
	    : (in_part->ch_buf_top <= in_part->ch_buf_bot
			    && in_part->ch_buf_top <= buf->b_ml.ml_line_count);
}

#만약 defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    static 정수
channel_fill_wfds(정수 maxfd_arg, fd_set *wfds)
{
    정수		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	만약 (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (정수)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((정수)in_part->ch_fd, wfds);
	    만약 ((정수)in_part->ch_fd >= maxfd)
		maxfd = (정수)in_part->ch_fd + 1;
	}
    }
    반환 maxfd;
}
#아니면
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    static 정수
channel_fill_poll_write(정수 nfd_in, struct pollfd *fds)
{
    정수		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	만약 (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	아니면
	    in_part->ch_poll_idx = -1;
    }
    반환 nfd;
}
#endif

typedef enum {
    CW_READY,
    CW_NOT_READY,
    CW_ERROR
} channel_wait_result;

/*
 * Check for reading from "fd" with "timeout" msec.
 * Return CW_READY when there is something to read.
 * Return CW_NOT_READY when there is nothing to read.
 * Return CW_ERROR when there is an error.
 */
    static channel_wait_result
channel_wait(channel_T *channel, sock_T fd, 정수 timeout)
{
    만약 (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    만약 (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	정수	sleep_time;
	DWORD	deadline = GetTickCount() + timeout;
	정수	delay = 1;

	// reading from a pipe, not a socket
	동안 (TRUE)
	{
	    정수 r = PeekNamedPipe((HANDLE)fd, NULL, 0, NULL, &nread, NULL);

	    만약 (r && nread > 0)
		반환 CW_READY;

	    만약 (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, NULL);
	    }
	    아니면 만약 (r == 0)
		반환 CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_lines();

	    sleep_time = deadline - GetTickCount();
	    만약 (sleep_time <= 0)
		break;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    만약 (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    만약 (delay > 10)
		delay = 10;
	}
    }
    아니면
#endif
    {
#만약 defined(HAVE_SELECT)
	struct timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	정수		ret;
	정수		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	위해 (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((정수)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (정수)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, NULL, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    만약 (ret == -1 && errno == EINTR)
		continue;
# endif
	    만약 (ret > 0)
	    {
		만약 (FD_ISSET(fd, &rfds))
		    반환 CW_READY;
		channel_write_any_lines();
		continue;
	    }
	    break;
	}
#아니면
	위해 (;;)
	{
	    struct pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    정수		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    만약 (poll(fds, nfd, timeout) > 0)
	    {
		만약 (fds[0].revents & POLLIN)
		    반환 CW_READY;
		channel_write_any_lines();
		continue;
	    }
	    break;
	}
#endif
    }
    반환 CW_NOT_READY;
}

    static void
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, 정수 is_err, char *func)
{
    char	msg[] = "%s(): Read %s from ch_part[%d], closing";

    만약 (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, msg, func, "error", part);
    아니면
	ch_log(channel, msg, func, "EOF", part);

    // Queue a "DETACH" netbeans message in the command queue in order to
    // terminate the netbeans session later. Do not end the session here
    // directly as we may be running in the context of a call to
    // netbeans_parse_messages():
    //	netbeans_parse_messages
    //	    -> autocmd triggered while processing the netbeans cmd
    //		-> ui_breakcheck
    //		    -> gui event loop or select loop
    //			-> channel_read()
    // Only send "DETACH" for a netbeans channel.
    만약 (channel->ch_nb_close_cb != NULL)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_MSG_RAW,
			      (정수)STRLEN_LITERAL(DETACH_MSG_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    만약 (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	만약 (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    static void
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    만약 (channel->ch_nb_close_cb != NULL)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    만약 (channel->ch_ss_close_cb != NULL)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    static void
channel_read(channel_T *channel, ch_part_T part, char *func)
{
    static char_u	*buf = NULL;
    정수			len = 0;
    정수			readlen = 0;
    sock_T		fd;
    정수			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    만약 (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	반환;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    만약 (buf == NULL)
    {
	buf = alloc(MAXMSGSIZE);
	만약 (buf == NULL)
	    반환;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    위해 (;;)
    {
	만약 (channel_wait(channel, fd, 0) != CW_READY)
	    break;
	만약 (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    struct sockaddr_storage	client;

	    newchannel = add_channel();
	    만약 (newchannel == NULL)
	    {
		ch_error(NULL, "Cannot allocate channel.");
		반환;
	    }
	    socklen = sizeof(client);
	    newfd = accept(fd, (struct sockaddr*)&client, &socklen);
	    만약 (newfd < 0)
	    {
		ch_error(NULL, "Cannot accept channel.");
		channel_free(newchannel);
		반환;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    만약 (channel->ch_ss_accept_cb != NULL)
	    {
		channel->ch_ss_accept_cb(newchannel);
		반환;
	    }
#endif

	    만약 (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		char addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((struct sockaddr_in*)&client)->sin_addr,
			addr, sizeof(addr));
		vim_snprintf((char *)namebuf, sizeof(namebuf), "%s:%d",
			addr,
			ntohs(((struct sockaddr_in*)&client)->sin_port));
#아니면
		vim_snprintf((char *)namebuf, sizeof(namebuf), "%s:%d",
		    inet_ntoa(((struct sockaddr_in*)&client)->sin_addr),
		    ntohs(((struct sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    아니면 만약 (client.ss_family == AF_INET6)
	    {
		char addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((struct sockaddr_in6*)&client)->sin6_addr,
			addr, sizeof(addr));
		vim_snprintf((char *)namebuf, sizeof(namebuf), "[%s]:%d",
			addr,
			ntohs(((struct sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    아니면 만약 (client.ss_family == AF_UNIX)
		vim_snprintf((char *)namebuf, sizeof(namebuf),
							    "unix:anonymous");
	    아니면
		vim_snprintf((char *)namebuf, sizeof(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    반환;
	}
	만약 (use_socket)
	    len = sock_read(fd, (char *)buf, MAXMSGSIZE);
	아니면
	    len = fd_read(fd, (char *)buf, MAXMSGSIZE);
	만약 (len <= 0)
	    break;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    만약 (readlen <= 0)
    {
	만약 (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#만약 defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    아니면 만약 (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    void
channel_check(channel_T *channel, ch_part_T part)
{
    channel_read(channel, part, "channel_check");
}

#endif

/*
 * Read from RAW or NL "channel"/"part".  Blocks until there is something to
 * read or the timeout expires.
 * When "raw" is TRUE don't block waiting on a NL.
 * Does not trigger timers or handle messages.
 * Returns what was read in allocated memory.
 * Returns NULL in case of error or timeout.
 */
    static char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, 정수 timeout, 정수 raw, 정수 *outlen)
{
    char_u	*buf;
    char_u	*msg;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*node;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    동안 (TRUE)
    {
	node = channel_peek(channel, part);
	만약 (node != NULL)
	{
	    만약 (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(node) != NULL))
		// got a complete message
		break;
	    만약 (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		continue;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    만약 (raw || fd == INVALID_FD)
		break;
	}

	// Wait for up to the channel timeout.
	만약 (fd == INVALID_FD)
	    반환 NULL;
	만약 (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    반환 NULL;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    만약 (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != NULL)
    {
	msg = channel_get_all(channel, part, outlen);
    }
    아니면
    {
	char_u *p;

	buf = node->rq_buffer;
	nl = channel_first_nl(node);

	// Convert NUL to NL, the internal representation.
	위해 (p = buf; (nl == NULL || p < nl) && p < buf + node->rq_buflen; ++p)
	    만약 (*p == NUL)
		*p = NL;

	만약 (nl == NULL)
	{
	    // must be a closed channel with missing NL
	    msg = channel_get(channel, part, NULL);
	}
	아니면 만약 (nl + 1 == buf + node->rq_buflen)
	{
	    // get the whole buffer
	    msg = channel_get(channel, part, NULL);
	    *nl = NUL;
	}
	아니면
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    msg = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (정수)(nl - buf) + 1);
	}
    }
    만약 (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != NULL ? *outlen : (정수)STRLEN(msg));
    반환 msg;
}

static 정수 channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    정수
channel_in_blocking_wait(void)
{
    반환 channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    static void
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, 정수 timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (void)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#아니면
    channel_wait_any(timeout);
#endif
}

/*
 * Read one JSON message with ID "id" from "channel"/"part" and store the
 * result in "rettv".
 * When "id" is -1 accept any message;
 * Blocks until the message is received or the timeout is reached.
 * In corner cases this can be called recursively, that is why ch_block_ids is
 * a list.
 */
    static 정수
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	정수	    timeout_arg,
	정수	    id,
	typval_T    **rettv)
{
    정수		more;
    sock_T	fd;
    정수		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    정수		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    만약 (id >= 0)
	channel_add_block_id(chanpart, id);

    위해 (;;)
    {
	만약 (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (void)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	만약 (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    break;
	}

	만약 (!more)
	{
	    void *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    void *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    만약 (channel_parse_messages())
		continue;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    만약 (readahead_ptr != NULL && readahead_ptr != prev_readahead_ptr)
		continue;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (정수)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    만약 (timeout <= 0 || fd == INVALID_FD)
	    {
		만약 (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		break;
	    }
	    // If there was an incomplete message use the deadline for that.
	    만약 (chanpart->ch_wait_len > 0)
	    {
		정수	left;
#ifdef MSWIN
		left = chanpart->ch_deadline - GetTickCount() + 1;
#아니면
		{
		    struct timeval now_tv;

		    gettimeofday(&now_tv, NULL);
		    left = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		만약 (left < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		아니면 만약 (left < timeout)
		    timeout = left;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    만약 (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    반환 retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, 정수 check_open, 정수 reading, ch_part_T part)
{
    channel_T	*channel = NULL;
    정수		has_readahead = FALSE;

    만약 (tv->v_type == VAR_JOB)
    {
	만약 (tv->vval.v_job != NULL)
	    channel = tv->vval.v_job->jv_channel;
    }
    아니면 만약 (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    아니면
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	반환 NULL;
    }
    만약 (channel != NULL && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_COUNT ? part : channel_part_read(channel));

    만약 (check_open && (channel == NULL || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	반환 NULL;
    }
    반환 channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    static void
common_channel_read(typval_T *argvars, typval_T *rettv, 정수 raw, 정수 blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_COUNT;
    jobopt_T	opt;
    정수		mode;
    정수		timeout;
    정수		id = -1;
    typval_T	*listtv = NULL;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    만약 (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	반환;

    clear_job_options(&opt);
    만약 (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	goto theend;

    만약 (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    만약 (channel == NULL)
	goto theend;

    만약 (part == PART_COUNT)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    만약 (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    만약 (blob || mode == CH_MODE_BLOB)
    {
	정수	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	만약 (p != NULL)
	{
	    blob_T	*b = blob_alloc();

	    만약 (b != NULL)
	    {
		b->bv_ga.ga_len = outlen;
		만약 (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		아니면
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    아니면 만약 (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, NULL);
    아니면
    {
	만약 (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	만약 (listtv != NULL)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	아니면
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#만약 defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    void
channel_handle_events(정수 only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	만약 (only_keep_open && !channel->ch_keep_open)
	    continue;

	// check the socket and pipes
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    만약 (fd == INVALID_FD)
		continue;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    정수 r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    만약 (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    아니면 만약 (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	만약 (channel->ch_job != NULL)
	{
	    job_T *job = channel->ch_job;

	    만약 (job->jv_tty_out != NULL && job->jv_status == JOB_FINISHED)
		위해 (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#만약 defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    정수
channel_any_keep_open(void)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	만약 (channel->ch_keep_open)
	    반환 TRUE;
    반환 FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    void
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    정수		fd = ch_part->ch_fd;

    만약 (fd == INVALID_FD)
	반환;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#아니면
    (void)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    정수
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	정수	  len_arg,
	char	  *fun)
{
    정수		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    정수		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    만약 (fd == INVALID_FD)
    {
	만약 (!channel->ch_error && fun != NULL)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	반환 FAIL;
    }

    만약 (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    만약 (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_msg = 0;
    }

    위해 (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	정수	    len;

	만약 (wq->wq_next != NULL)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	아니면
	{
	    만약 (len_arg == 0)
		// nothing to write, called from channel_select_check()
		반환 OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	만약 (part == PART_SOCK)
	    res = sock_write(fd, (char *)buf, len);
	아니면
	{
	    res = fd_write(fd, (char *)buf, len);
#ifdef MSWIN
	    만약 (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, NULL);
	    }
#endif
	}
	만약 (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	만약 (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    만약 (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    만약 (res == len)
	    {
		// Wrote all the buf[len] bytes.
		만약 (entry != NULL)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    continue;
		}
		만약 (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    아니면
	    {
		// Wrote only buf[res] bytes, can't write more now.
		만약 (entry != NULL)
		{
		    만약 (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (char *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		아니면
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		만약 (wq->wq_prev != NULL
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    만약 (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((char *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		아니면
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    만약 (last != NULL)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = NULL;
			만약 (wq->wq_prev == NULL)
			    wq->wq_next = last;
			아니면
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			만약 (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	아니면 만약 (res != len)
	{
	    만약 (!channel->ch_error && fun != NULL)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    반환 FAIL;
	}

	channel->ch_error = FALSE;
	반환 OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    static channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	정수	    len,
	정수	    id,
	정수	    eval,
	jobopt_T    *opt,
	char	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    만약 (channel == NULL)
	반환 NULL;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    만약 (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	반환 NULL;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    만약 (opt->jo_callback.cb_name != NULL && *opt->jo_callback.cb_name != NUL)
    {
	만약 (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    반환 NULL;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    만약 (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == NULL)
	반환 channel;
    반환 NULL;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    static void
ch_expr_common(typval_T *argvars, typval_T *rettv, 정수 eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    정수		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    정수		timeout;
    정수		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    만약 (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	반환;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    만약 (channel == NULL)
	반환;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    만약 (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	반환;
    }

    만약 (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	char		*key = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	만약 (rettv_dict_alloc(rettv) == FAIL)
	    반환;

	만약 (check_for_dict_arg(argvars, 1) == FAIL)
	    반환;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)key, -1);
	만약 (argvars[2].v_type == VAR_DICT
		&& dict_has_key(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	만약 (di != NULL && di->di_tv.v_type != VAR_NUMBER
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_value_for_argument_str), key);
	    반환;
	}

	만약 (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_msg_id;
	    만약 (di == NULL)
		dict_add_number(d, "seq", id);
	    아니면
		di->di_tv.vval.v_number = id;
	}
	아니면 만약 (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_msg_id;
	    만약 (di == NULL)
		dict_add_number(d, "id", id);
	    아니면
		di->di_tv.vval.v_number = id;
	}
	아니면
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    만약 (di != NULL && di->di_tv.v_type == VAR_NUMBER)
		id = di->di_tv.vval.v_number;
	}
	만약 (ch_mode == CH_MODE_LSP && !dict_has_key(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_msg(&argvars[1]);
    }
    아니면
    {
	id = ++channel->ch_last_msg_id;
	text = json_encode_nr_expr(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    만약 (text == NULL)
	반환;

    channel = send_common(argvars, text, (정수)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    만약 (channel != NULL && eval)
    {
	만약 (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	아니면
	    timeout = channel_get_timeout(channel, part_read);
	만약 (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    만약 (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
	    }
	    아니면
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_NUMBER;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    만약 (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	만약 (rettv->vval.v_dict != NULL)
	    dict_add_number(rettv->vval.v_dict, "seq", id);
    }
    아니면 만약 (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	만약 (rettv->vval.v_dict != NULL)
	    dict_add_number(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    static void
ch_raw_common(typval_T *argvars, typval_T *rettv, 정수 eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    정수		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    정수		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    만약 (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	반환;

    만약 (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    아니면
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (정수)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    만약 (channel != NULL && eval)
    {
	만약 (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	아니면
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, NULL);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#만약 defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    정수
channel_poll_setup(정수 nfd_in, void *fds_in, 정수 *towait)
{
    정수		nfd = nfd_in;
    channel_T	*channel;
    struct	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    만약 (ch_part->ch_fd != INVALID_FD)
	    {
		만약 (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    만약 (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		아니면
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    아니면
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    반환 nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    정수
channel_poll_check(정수 ret_in, void *fds_in)
{
    정수		ret = ret_in;
    channel_T	*channel;
    struct	pollfd *fds = fds_in;
    ch_part_T	part;
    정수		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    만약 (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    아니면 만약 (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	만약 (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_input(channel);
	    --ret;
	}
    }

    반환 ret;
}
#endif // UNIX && !HAVE_SELECT

#만약 !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    정수
channel_select_setup(
	정수 maxfd_in,
	void *rfds_in,
	void *wfds_in,
	struct timeval *tv,
	struct timeval **tvp)
{
    정수		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    만약 (fd != INVALID_FD)
	    {
		만약 (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    만약 (*tvp == NULL || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		아니면
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    만약 ((정수)fd >= FD_SETSIZE)
			continue;
# endif
		    FD_SET((정수)fd, rfds);
		    만약 (maxfd < (정수)fd)
			maxfd = (정수)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    반환 maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    정수
channel_select_check(정수 ret_in, void *rfds_in, void *wfds_in)
{
    정수		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	위해 (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    만약 (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (정수)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    아니면 만약 (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	만약 (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (정수)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_input(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	만약 (channel->ch_job != NULL)
	{
	    job_T *job = channel->ch_job;

	    만약 (job->jv_tty_out != NULL && job->jv_status == JOB_FINISHED)
		위해 (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    반환 ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    static void
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    만약 (stdio_channel->CH_OUT_FD != INVALID_FD)
	(void)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(정수)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#elif defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    struct timeval	tv;
    struct timeval	*tvp = &tv;
    정수			maxfd;
    정수			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, NULL, tvp);
    만약 (ret >= 0)
	(void)channel_select_check(ret, &rfds, &wfds);
#아니면
    struct pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    정수			nfd;
    정수			towait = (정수)msec;
    정수			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    만약 (ret >= 0)
	(void)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    void
channel_stdio_loop(void)
{
    동안 (stdio_channel != NULL && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	만약 (due_time > 0 && due_time < msec)
	    msec = due_time;
#endif
	channel_wait_any(msec);
	parse_queued_messages();
    }
}

/*
 * Execute queued up commands.
 * Invoked from the main loop when it's safe to execute received commands,
 * and during a blocking wait for ch_evalexpr().
 * Return TRUE when something was done.
 */
    정수
channel_parse_messages(void)
{
    channel_T	*channel = first_channel;
    정수		ret = FALSE;
    정수		r;
    ch_part_T	part = PART_SOCK;
    static 정수	recursive = 0;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;
#endif

    // The code below may invoke callbacks, which might call us back.
    // In a recursive call channels will not be closed.
    ++recursive;
    ++safe_to_invoke_callback;

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Only do this message when another message was given, otherwise we get
    // lots of them.
    만약 ((did_repeated_msg & REPEATED_MSG_LOOKING) == 0)
    {
	ch_log(NULL, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_msg = REPEATED_MSG_LOOKING;
    }
    동안 (channel != NULL)
    {
	만약 (recursive == 1)
	{
	    만약 (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_COUNT);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		continue;
	    }
	    만약 (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		만약 (channel->ch_job != NULL)
		    channel->ch_job->jv_channel = NULL;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		continue;
	    }
	    만약 (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		continue;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	만약 (part == PART_OUT
		&& channel->ch_job != NULL
		&& channel->ch_job->jv_tty_out != NULL
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    continue;
	}
#endif
	만약 (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    만약 (r == OK)
		ret = TRUE;
	    만약 (channel_unref(channel) || (r == OK
#ifdef ELAPSED_FUNC
			// Limit the time we loop here to 100 msec, otherwise
			// Vim becomes unresponsive when the callback takes
			// more than a bit of time.
			&& ELAPSED_FUNC(start_tv) < 100L
#endif
			))
	    {
		// channel was freed or something was done, start over
		channel = first_channel;
		part = PART_SOCK;
		continue;
	    }
	}
	만약 (part < PART_ERR)
	    ++part;
	아니면
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    만약 (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    반환 ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    정수
channel_any_readahead(void)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    동안 (channel != NULL)
    {
	만약 (channel_has_readahead(channel, part))
	    반환 TRUE;
	만약 (part < PART_ERR)
	    ++part;
	아니면
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    반환 FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    정수
set_ref_in_channel(정수 copyID)
{
    정수		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    위해 (channel = first_channel; !abort && channel != NULL;
						   channel = channel->ch_next)
	만약 (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
	}
    반환 abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    static ch_part_T
channel_part_send(channel_T *channel)
{
    만약 (channel->CH_SOCK_FD == INVALID_FD)
	반환 PART_IN;
    반환 PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    static ch_part_T
channel_part_read(channel_T *channel)
{
    만약 (channel->CH_SOCK_FD == INVALID_FD)
	반환 PART_OUT;
    반환 PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    static ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    만약 (channel == NULL)
	반환 CH_MODE_JSON;
    반환 channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    static 정수
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    반환 channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    void
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_number = 0;
    만약 (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	반환;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    만약 (channel != NULL)
	rettv->vval.v_number = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    void
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    만약 (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	반환;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    만약 (channel != NULL)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    void
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    만약 (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	반환;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    만약 (channel != NULL)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    void
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_number = -1;

    만약 (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	반환;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    만약 (channel == NULL)
	반환;

    char_u	*what = tv_get_string(&argvars[1]);
    정수		part;
    만약 (STRCMP(what, "err") == 0)
	part = PART_ERR;
    아니면 만약 (STRCMP(what, "out") == 0)
	part = PART_OUT;
    아니면 만약 (STRCMP(what, "in") == 0)
	part = PART_IN;
    아니면
	part = PART_SOCK;
    만약 (channel->ch_part[part].ch_bufref.br_buf != NULL)
	rettv->vval.v_number =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    void
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    만약 (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	반환;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    만약 (channel == NULL)
	반환;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    만약 (channel->ch_job != NULL)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    void
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    만약 (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	반환;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    만약 (channel != NULL && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    void
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    만약 (check_restricted() || check_secure())
	반환;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    void
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    만약 (check_restricted() || check_secure())
	반환;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    void
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    void
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    void
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    void
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_expr_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    void
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_expr_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    void
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    void
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    void
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    만약 (check_restricted() || check_secure())
	반환;

    만약 (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	반환;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    만약 (channel == NULL)
	반환;
    clear_job_options(&opt);
    만약 (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    void
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    정수		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    만약 (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	반환;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    만약 (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	만약 (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
						     && (opt.jo_set & JO_PART))
	    part = opt.jo_part;
    }

    rettv->vval.v_string = vim_strsave((char_u *)channel_status(channel, part));
}

/*
 * Get a string with information about the channel in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
channel_to_string_buf(typval_T *varp, char_u *buf)
{
    channel_T *channel = varp->vval.v_channel;
    char      *status = channel_status(channel, -1);

    만약 (channel == NULL)
	vim_snprintf((char *)buf, NUMBUFLEN, "channel %s", status);
    아니면
	vim_snprintf((char *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    반환 buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(정수 ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	만약 (ch->ch_id == ch_id)
	    반환 ch;
    반환 NULL;
}

#endif // FEAT_JOB_CHANNEL
