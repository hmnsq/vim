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

#ถ้า defined(FEAT_JOB_CHANNEL)

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
typedef sa_nracanaa sockaddr_un {
    ADDRESS_FAMILY sun_family;
    var_na sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#มิฉะนั้น
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

static void channel_read(channel_T *channel, ch_part_T part, var_na *func);
static void channel_wait_any(long msec);
static ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
static จำนวนเต็ม channel_get_timeout(channel_T *channel, ch_part_T part);
static channel_T *channel_open_stdio(void);
static ch_part_T channel_part_send(channel_T *channel);
static ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    สำหรับ ((ch) = first_channel; (ch) != NULL; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
static จำนวนเต็ม safe_to_invoke_callback = 0;

#ifdef MSWIN
    static จำนวนเต็ม
fd_read(sock_T fd, var_na *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ถ้า (!ReadFile(h, buf, (DWORD)len, &nread, NULL))
	คืน -1;
    คืน (จำนวนเต็ม)nread;
}

    static จำนวนเต็ม
fd_write(sock_T fd, var_na *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    ขณะ (todo > 0)
    {
	ถ้า (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	มิฉะนั้น
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	ถ้า (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    ถ้า (err != ERROR_IO_PENDING)
		คืน -1;
	    ถ้า (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		คืน -1;
	    FlushFileBuffers(h);
	}
	มิฉะนั้น ถ้า (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    bha_nga;
	todo -= nwrite;
	done += nwrite;
    }
    คืน (จำนวนเต็ม)done;
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

    static var_na *
strerror_win32(จำนวนเต็ม eno)
{
    static LPVOID msgbuf = NULL;
    char_u *ptr;

    ถ้า (msgbuf)
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
    ถ้า (msgbuf != NULL)
	// chomp \r or \n
	สำหรับ (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    switch (*ptr)
	    {
		pak_sa '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    bha_nga;
		pak_sa '\n':
		    ถ้า (*(ptr + 1) == '\0')
			*ptr = '\0';
		    มิฉะนั้น
			*ptr = ' ';
		    bha_nga;
	    }
    คืน msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
static channel_T *first_channel = NULL;
static จำนวนเต็ม next_ch_id = 0;

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

    ถ้า (channel == NULL)
	คืน NULL;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    สำหรับ (part = PART_SOCK; part < PART_COUNT; ++part)
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

    ถ้า (first_channel != NULL)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    คืน channel;
}

    จำนวนเต็ม
has_any_channel(void)
{
    คืน first_channel != NULL;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    จำนวนเต็ม
channel_still_useful(channel_T *channel)
{
    จำนวนเต็ม has_sock_msg;
    จำนวนเต็ม	has_out_msg;
    จำนวนเต็ม	has_err_msg;

    // If the job was killed the channel is not expected to work anymore.
    ถ้า (channel->ch_job_killed && channel->ch_job == NULL)
	คืน FALSE;

    // If there is a close callback it may still need to be invoked.
    ถ้า (channel->ch_close_cb.cb_name != NULL)
	คืน TRUE;

    // If reading from or a buffer it's still useful.
    ถ้า (channel->ch_part[PART_IN].ch_bufref.br_buf != NULL)
	คืน TRUE;

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
    คืน (channel->ch_callback.cb_name != NULL && (has_sock_msg
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
    จำนวนเต็ม
channel_can_close(channel_T *channel)
{
    คืน channel->ch_to_be_closed == 0;
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
    ถ้า (channel->ch_next != NULL)
	channel->ch_next->ch_prev = channel->ch_prev;
    ถ้า (channel->ch_prev == NULL)
	first_channel = channel->ch_next;
    มิฉะนั้น
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    static void
channel_free(channel_T *channel)
{
    ถ้า (in_free_unref_items)
	คืน;

    ถ้า (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    มิฉะนั้น
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
    static จำนวนเต็ม
channel_may_free(channel_T *channel)
{
    ถ้า (!channel_still_useful(channel))
    {
	channel_free(channel);
	คืน TRUE;
    }
    คืน FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    จำนวนเต็ม
channel_unref(channel_T *channel)
{
    ถ้า (channel != NULL && --channel->ch_refcount <= 0)
	คืน channel_may_free(channel);
    คืน FALSE;
}

    จำนวนเต็ม
free_unused_channels_contents(จำนวนเต็ม copyID, จำนวนเต็ม mask)
{
    จำนวนเต็ม		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ถ้า (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    คืน did_free;
}

    void
free_unused_channels(จำนวนเต็ม copyID, จำนวนเต็ม mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    สำหรับ (ch = first_channel; ch != NULL; ch = ch_next)
    {
	ch_next = ch->ch_next;
	ถ้า (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ถ้า defined(FEAT_GUI)

# ถ้า defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    static channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ถ้า (fd == INVALID_FD)
	คืน NULL;

    FOR_ALL_CHANNELS(channel)
    {
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	    ถ้า (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		คืน channel;
	    }
    }
    คืน NULL;
}

    static void
channel_read_fd(จำนวนเต็ม fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ถ้า (channel == NULL)
	ch_error(NULL, "Channel for fd %d not found", fd);
    มิฉะนั้น
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    static void
messageFromServerX11(XtPointer clientData,
		  จำนวนเต็ม *unused1 UNUSED,
		  XtInputId *unused2 UNUSED)
{
    channel_read_fd((จำนวนเต็ม)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  ถ้า GTK_CHECK_VERSION(3,0,0)
    static gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    คืน TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  มิฉะนั้น
    static void
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  GdkInputCondition unused2 UNUSED)
{
    channel_read_fd((จำนวนเต็ม)(long)clientData);
}
#  endif
# endif

    static void
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    ถ้า (!CH_HAS_GUI)
	คืน;

    // gets stuck in handling events for a not connected channel
    ถ้า (channel->ch_keep_open)
	คืน;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ถ้า (channel->ch_part[part].ch_inputHandler == (XtInputId)NULL)
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
# มิฉะนั้น
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ถ้า (channel->ch_part[part].ch_inputHandler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   ถ้า GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_inputHandler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   มิฉะนั้น
	channel->ch_part[part].ch_inputHandler = gdk_input_add(
		(gint)channel->ch_part[part].ch_fd,
		(GdkInputCondition)
			     ((จำนวนเต็ม)GDK_INPUT_READ + (จำนวนเต็ม)GDK_INPUT_EXCEPTION),
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
    ถ้า (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    ถ้า (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    ถ้า (channel->CH_ERR_FD != INVALID_FD
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
    ถ้า (channel->ch_part[part].ch_inputHandler != (XtInputId)NULL)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	XtRemoveInput(channel->ch_part[part].ch_inputHandler);
	channel->ch_part[part].ch_inputHandler = (XtInputId)NULL;
    }
# มิฉะนั้น
#  ifdef FEAT_GUI_GTK
    ถ้า (channel->ch_part[part].ch_inputHandler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   ถ้า GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_inputHandler);
#   มิฉะนั้น
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

    สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    static จำนวนเต็ม
channel_connect(
	channel_T *channel,
	const sa_nracanaa sockaddr *server_addr,
	จำนวนเต็ม server_addrlen,
	จำนวนเต็ม *waittime)
{
    จำนวนเต็ม		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    ขณะ (TRUE)
    {
	long	elapsed_msec = 0;
	จำนวนเต็ม	waitnow;
	จำนวนเต็ม	ret;

	ถ้า (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ถ้า (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    คืน -1;
	}

	ถ้า (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ถ้า (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#มิฉะนั้น
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		คืน -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ถ้า (ret == 0)
	    // The connection could be established.
	    bha_nga;

	SOCK_ERRNO;
	ถ้า (*waittime < 0 || (errno != EWOULDBLOCK
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
	    คืน -1;
	}
	มิฉะนั้น ถ้า (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    คืน -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    sa_nracanaa timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    จำนวนเต็ม			so_error = 0;
	    socklen_t		so_error_len = parimaa_na(so_error);
	    sa_nracanaa timeval	start_tv;
	    sa_nracanaa timeval	end_tv;
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
	    ถ้า (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		คืน -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ถ้า (FD_ISSET(sd, &wfds))
		bha_nga;
	    elapsed_msec = waitnow;
	    ถ้า (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		continue;
	    }
#มิฉะนั้น
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ถ้า (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ถ้า (ret < 0 || (so_error != 0
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
		    คืน -1;
		}
		มิฉะนั้น ถ้า (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    คืน -1;
		}
	    }

	    ถ้า (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		bha_nga;

	    gettimeofday(&end_tv, NULL);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	ถ้า (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ถ้า (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ถ้า (!got_int)
	    {
		ถ้า (*waittime <= 0)
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
	คืน -1;
    }

    ถ้า (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#มิฉะนั้น
	(void)fcntl(sd, F_SETFL, 0);
#endif
    }

    คืน sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	const var_na *path,
	void (*nb_close_cb)(void))
{
    channel_T		*channel = NULL;
    จำนวนเต็ม			sd = -1;
    size_t		path_len = STRLEN(path);
    sa_nracanaa sockaddr_un	server;
    size_t		server_len;
    จำนวนเต็ม			waittime = -1;

    ถ้า (*path == NUL || path_len >= parimaa_na(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	คืน NULL;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ถ้า (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	คืน NULL;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, parimaa_na(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(sa_nracanaa sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (sa_nracanaa sockaddr *)&server, (จำนวนเต็ม)server_len,
								   &waittime);

    ถ้า (sd < 0)
    {
	channel_free(channel);
	คืน NULL;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (var_na *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    คืน channel;
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
	const var_na *hostname,
	จำนวนเต็ม port,
	จำนวนเต็ม waittime,
	void (*nb_close_cb)(void))
{
    จำนวนเต็ม			sd = -1;
    channel_T		*channel = NULL;
#ifdef FEAT_IPV6
    จำนวนเต็ม			err;
    sa_nracanaa addrinfo	hints;
    sa_nracanaa addrinfo	*res = NULL;
    sa_nracanaa addrinfo	*addr = NULL;
#มิฉะนั้น
    sa_nracanaa sockaddr_in	server;
    sa_nracanaa hostent	*host = NULL;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ถ้า (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	คืน NULL;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ถ้า defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ถ้า ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	คืน NULL;
    }

    // Count the number of addresses for timeout distribution
    จำนวนเต็ม addr_count = 0;
    สำหรับ (addr = res; addr != NULL; addr = addr->ai_next)
	addr_count++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ถ้า (waittime == 0)
	waittime = 15;

    จำนวนเต็ม addr_index = 0;
    สำหรับ (addr = res; addr != NULL; addr = addr->ai_next, addr_index++)
    {
	const var_na  *dst = hostname;
# ifdef HAVE_INET_NTOP
	const void  *src = NULL;
	var_na	    buf[NUMBUFLEN];
# endif
	จำนวนเต็ม	    try_waittime;
	จำนวนเต็ม	    before_waittime;
	จำนวนเต็ม	    consumed;
	จำนวนเต็ม	    remaining_addrs;

	ถ้า (addr->ai_family == AF_INET6)
	{
	    sa_nracanaa sockaddr_in6 *sai = (sa_nracanaa sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	มิฉะนั้น ถ้า (addr->ai_family == AF_INET)
	{
	    sa_nracanaa sockaddr_in *sai = (sa_nracanaa sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ถ้า (src != NULL)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, parimaa_na(buf));
	    ถ้า (dst == NULL)
		dst = hostname;
	    มิฉะนั้น ถ้า (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ถ้า (addr->ai_next == NULL)
	    try_waittime = waittime;
	มิฉะนั้น ถ้า (addr_index == 0)
	{
	    ถ้า (waittime > 500)
		try_waittime = 250;
	    มิฉะนั้น ถ้า (waittime > 30)
		try_waittime = waittime / 2;
	    มิฉะนั้น
		try_waittime = waittime;
	}
	มิฉะนั้น
	{
	    remaining_addrs = addr_count - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (จำนวนเต็ม)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ถ้า (waittime < 0)
	    waittime = 0;

	ถ้า (sd >= 0)
	    bha_nga;

	// If we have no time left, stop trying
	ถ้า (waittime <= 0 && addr->ai_next != NULL)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    bha_nga;
	}
    }

    freeaddrinfo(res);
#มิฉะนั้น
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    ถ้า ((host = gethostbyname(hostname)) == NULL)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	คืน NULL;
    }
    {
	var_na *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], parimaa_na(p));
	memcpy((var_na *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    ถ้า (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (sa_nracanaa sockaddr *)&server, parimaa_na(server),
								   &waittime);
#endif

    ถ้า (sd < 0)
    {
	channel_free(channel);
	คืน NULL;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (var_na *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    คืน channel;
}

    static void
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ถ้า (callback->cb_name != NULL && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    มิฉะนั้น
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
    ถ้า (curbuf->b_ml.ml_mfp == NULL)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    static buf_T *
channel_find_buffer(char_u *name, จำนวนเต็ม err, จำนวนเต็ม msg)
{
    buf_T *buf = NULL;
    buf_T *save_curbuf = curbuf;

    ถ้า (name != NULL && *name != NUL)
    {
	buf = buflist_findname(name);
	ถ้า (buf == NULL)
	    buf = buflist_findname_exp(name);
    }

    ถ้า (buf != NULL)
	คืน buf;

    buf = buflist_new(name == NULL || *name == NUL ? NULL : name,
	    NULL, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ถ้า (buf == NULL)
	คืน NULL;
    prepare_buffer(buf);

    curbuf = buf;
    ถ้า (msg)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    คืน buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    static void
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ถ้า (opt->jo_set & JO_MODE)
	สำหรับ (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    ถ้า (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    ถ้า (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    ถ้า (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    ถ้า (opt->jo_set & JO_TIMEOUT)
	สำหรับ (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ถ้า (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    ถ้า (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ถ้า (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    ถ้า (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    ถ้า (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    ถ้า (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ถ้า (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ถ้า ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ถ้า (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	ถ้า (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    ถ้า (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	มิฉะนั้น
	{
	    จำนวนเต็ม msg = TRUE;

	    ถ้า (opt->jo_set2 & JO2_OUT_MSG)
		msg = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, msg);
	}
	ถ้า (buf != NULL)
	{
	    ถ้า (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    ถ้า (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    มิฉะนั้น
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (var_na *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ถ้า (buf->b_ml.ml_mfp == NULL)
		    prepare_buffer(buf);
	    }
	}
    }

    ถ้า ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ถ้า (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	ถ้า (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	มิฉะนั้น ถ้า (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ถ้า (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	มิฉะนั้น
	{
	    จำนวนเต็ม msg = TRUE;

	    ถ้า (opt->jo_set2 & JO2_ERR_MSG)
		msg = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, msg);
	}
	ถ้า (buf != NULL)
	{
	    ถ้า (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ถ้า (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    มิฉะนั้น
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (var_na *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ถ้า (buf->b_ml.ml_mfp == NULL)
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
    จำนวนเต็ม
channel_parse_socketserver_address(
	char_u	*address,
	จำนวนเต็ม	*port,
	char_u	**unix_path,
	bool	quiet)
{
    var_na    *rest;
    long    val;

    ถ้า (*address == NUL)
	goto fail;

    ถ้า (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	คืน OK;
    }

    val = strtol((var_na *)(address), &rest, 10);
    ถ้า (val < 0 || val >= 65536 || *rest != NUL)
    {
	ถ้า (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	คืน FAIL;
    }
    *port = (จำนวนเต็ม)val;

    คืน OK;
fail:
    ถ้า (!quiet)
	semsg(_(e_invalid_argument_str), address);
    คืน FAIL;
}

/*
 * Implements ch_open().
 */
    static channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    var_na	*rest;
    จำนวนเต็ม		port = 0;
    จำนวนเต็ม		is_ipv6 = FALSE;
    จำนวนเต็ม		is_unix = FALSE;
    จำนวนเต็ม		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = NULL;

    ถ้า (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	คืน NULL;

    address = tv_get_string(&argvars[0]);
    ถ้า (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	คืน NULL;

    ถ้า (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	คืน NULL;
    }

    ถ้า (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    มิฉะนั้น ถ้า (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    มิฉะนั้น ถ้า (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	ถ้า (p == NULL || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    คืน NULL;
	}
    }
    มิฉะนั้น
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	ถ้า (p == NULL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    คืน NULL;
	}
    }

    ถ้า (!is_unix && !is_stdio)
    {
	port = strtol((var_na *)(p + 1), &rest, 10);
	ถ้า (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    คืน NULL;
	}
	ถ้า (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	มิฉะนั้น
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ถ้า (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	goto theend;
    ถ้า (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }

    ถ้า (is_stdio)
	channel = channel_open_stdio();
    มิฉะนั้น ถ้า (is_unix)
	channel = channel_open_unix((var_na *)address, NULL);
    มิฉะนั้น
	channel = channel_open((var_na *)address, port, opt.jo_waittime, NULL);
    ถ้า (channel != NULL)
    {
	จำนวนเต็ม	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	ถ้า (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    คืน channel;
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

    ถ้า (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	คืน NULL;
    }
    ถ้า (stdio_channel != NULL)
    {
	emsg(_(e_cannot_open_stdio_channel));
	คืน NULL;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	จำนวนเต็ม	nul_fd;

	ถ้า (!DuplicateHandle(proc, GetStdHandle(STD_INPUT_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    คืน NULL;
	}
	ถ้า (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    คืน NULL;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
		NULL);
	ถ้า (hnul != INVALID_HANDLE_VALUE)
	{
	    SetStdHandle(STD_INPUT_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ถ้า (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#มิฉะนั้น
    {
	จำนวนเต็ม	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	ถ้า (in_fd < 0 || out_fd < 0)
	{
	    ถ้า (in_fd >= 0)
		close(in_fd);
	    ถ้า (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    คืน NULL;
	}
	(void)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(void)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ถ้า (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    ถ้า (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	คืน NULL;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    คืน channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    var_na	*rest;
    จำนวนเต็ม		port;
    จำนวนเต็ม		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = NULL;

    ถ้า (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	คืน NULL;

    arg = tv_get_string(&argvars[0]);
    ถ้า (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	คืน NULL;
    }

    ถ้า (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    มิฉะนั้น
    {
	port = strtol((var_na *)(arg), &rest, 10);
	ถ้า (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    คืน NULL;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ถ้า (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	goto theend;
    ถ้า (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }

    ถ้า (is_unix)
	channel = channel_listen_unix((var_na *)arg, NULL, true);
    มิฉะนั้น
	channel = channel_listen(port, NULL);
    ถ้า (channel != NULL)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    คืน channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	จำนวนเต็ม port_in,
	void (*nb_close_cb)(void))
{
    จำนวนเต็ม			sd = -1;
    sa_nracanaa sockaddr_in	server;
#ifndef FEAT_IPV6
    sa_nracanaa hostent	*host;
#endif
    จำนวนเต็ม			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ถ้า (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	คืน NULL;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((var_na *)&server, 0, parimaa_na(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ถ้า (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	คืน NULL;
    }

#ifdef MSWIN
    ถ้า (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (const var_na *)&val, parimaa_na(val)) < 0)
#มิฉะนั้น
    ถ้า (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, parimaa_na(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	คืน NULL;
    }

    // Bind the socket to the port
    ถ้า (bind(sd, (sa_nracanaa sockaddr *)&server, parimaa_na(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	คืน NULL;
    }

    ถ้า (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	คืน NULL;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ถ้า (port_in == 0)
    {
	sa_nracanaa sockaddr_in	addr;
	socklen_t		addr_len = parimaa_na(addr);

	ถ้า (getsockname(sd, (sa_nracanaa sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (var_na *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    คืน channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	var_na *path,
	void (*nb_close_cb)(void),
	bool replace)
{
    จำนวนเต็ม			sd = -1;
    sa_nracanaa sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    ถ้า (path == NULL || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == NULL ? (var_na *)"" : path);
	คืน NULL;
    }

    path_len = STRLEN(path);
    ถ้า (path_len >= parimaa_na(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	คืน NULL;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ถ้า (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	คืน NULL;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, parimaa_na(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ถ้า (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	คืน NULL;
    }

    ถ้า (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(sa_nracanaa sockaddr_un, sun_path) + path_len + 1;
    ถ้า (bind(sd, (sa_nracanaa sockaddr *)&server, (จำนวนเต็ม)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	คืน NULL;
    }

    ถ้า (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	คืน NULL;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (var_na *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    คืน channel;
}

     void
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ถ้า (*fd == INVALID_FD)
	คืน;

    ถ้า (part == PART_SOCK)
	sock_close(*fd);
    มิฉะนั้น
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ถ้า ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    ถ้า (channel->ch_named_pipe)
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
    ถ้า (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#ถ้า defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ถ้า (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    ถ้า (out != INVALID_FD)
    {
#ถ้า defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#ถ้า defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    ถ้า (err != INVALID_FD)
    {
#ถ้า defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#ถ้า defined(FEAT_GUI)
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

    ถ้า (job->jv_in_buf == NULL)
	คืน;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (var_na *)in_part->ch_bufref.br_buf->b_ffname);
    ถ้า (options->jo_set & JO_IN_TOP)
    {
	ถ้า (options->jo_in_top == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_top =
		in_part->ch_bufref.br_buf->b_ml.ml_line_count + 1;
	}
	มิฉะนั้น
	    in_part->ch_buf_top = options->jo_in_top;
    }
    มิฉะนั้น
	in_part->ch_buf_top = 1;
    ถ้า (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    มิฉะนั้น
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
	จำนวนเต็ม	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    ถ้า (item == NULL)
	คืน;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = NULL;
    ถ้า (item->cq_prev == NULL)
	head->cq_next = item;
    มิฉะนั้น
	item->cq_prev->cq_next = item;
}

    static void
write_buf_line(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *line = ml_get_buf(buf, lnum, FALSE);
    จำนวนเต็ม	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    จำนวนเต็ม	    i;

    // Need to make a copy to be able to append a NL.
    ถ้า ((p = alloc(len + 2)) == NULL)
	คืน;
    memcpy((var_na *)p, (var_na *)line, len);

    ถ้า (channel->ch_write_text_mode)
	p[len] = CAR;
    มิฉะนั้น
    {
	สำหรับ (i = 0; i < len; ++i)
	    ถ้า (p[i] == NL)
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
    static จำนวนเต็ม
can_write_buf_line(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    ถ้า (in_part->ch_fd == INVALID_FD)
	คืน FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ถ้า (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    มิฉะนั้น ถ้า (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# ถ้า defined(HAVE_SELECT)
	sa_nracanaa timeval	tval;
	fd_set		wfds;
	จำนวนเต็ม		ret;

	FD_ZERO(&wfds);
	FD_SET((จำนวนเต็ม)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	สำหรับ (;;)
	{
	    ret = select((จำนวนเต็ม)in_part->ch_fd + 1, NULL, &wfds, NULL, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ถ้า (ret == -1 && errno == EINTR)
		continue;
#  endif
	    ถ้า (ret <= 0 || in_part->ch_block_write == 1)
	    {
		ถ้า (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		มิฉะนั้น
		    ch_log(channel, "Input not ready for writing");
		คืน FALSE;
	    }
	    bha_nga;
	}
# มิฉะนั้น
	sa_nracanaa pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	ถ้า (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    คืน FALSE;
	}
	ถ้า (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    คืน FALSE;
	}
# endif
    }
#endif
    คืน TRUE;
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
    จำนวนเต็ม		written = 0;

    ถ้า (buf == NULL || in_part->ch_buf_append)
	คืน;  // no buffer or using appending
    ถ้า (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == NULL)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = NULL;
	คืน;
    }

    สำหรับ (lnum = in_part->ch_buf_top; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_line_count; ++lnum)
    {
	ถ้า (!can_write_buf_line(channel))
	    bha_nga;
	write_buf_line(buf, lnum, channel);
	++written;
    }

    ถ้า (written == 1)
	ch_log(channel, "written line %d to channel", (จำนวนเต็ม)lnum - 1);
    มิฉะนั้น ถ้า (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_top = lnum;
    ถ้า (lnum > buf->b_ml.ml_line_count || lnum > in_part->ch_buf_bot)
    {
#ถ้า defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ถ้า (channel->ch_job != NULL)
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
	ถ้า (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    มิฉะนั้น
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
	สำหรับ (part = PART_SOCK; part < PART_COUNT; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ถ้า (ch_part->ch_bufref.br_buf == buf)
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

    ถ้า (in_part->ch_writeque.wq_next != NULL)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    มิฉะนั้น ถ้า (in_part->ch_bufref.br_buf != NULL)
    {
	ถ้า (in_part->ch_buf_append)
	    channel_write_new_lines(in_part->ch_bufref.br_buf);
	มิฉะนั้น
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
    จำนวนเต็ม		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	จำนวนเต็ม	    written = 0;

	ถ้า (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    ถ้า (in_part->ch_fd == INVALID_FD)
		continue;  // pipe was closed
	    found_one = TRUE;
	    สำหรับ (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_line_count;
								       ++lnum)
	    {
		ถ้า (!can_write_buf_line(channel))
		    bha_nga;
		write_buf_line(buf, lnum, channel);
		++written;
	    }

	    ถ้า (written == 1)
		ch_log(channel, "written line %d to channel", (จำนวนเต็ม)lnum - 1);
	    มิฉะนั้น ถ้า (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ถ้า (lnum < buf->b_ml.ml_line_count)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_line_count - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    ถ้า (!found_one)
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

    ถ้า (safe_to_invoke_callback == 0)
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

    คืน head->rq_next;
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

    สำหรับ (i = 0; i < node->rq_buflen; ++i)
	ถ้า (buffer[i] == NL)
	    คืน buffer + i;
    คืน NULL;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, จำนวนเต็ม *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *p;

    ถ้า (node == NULL)
	คืน NULL;
    ถ้า (outlen != NULL)
	*outlen += node->rq_buflen;
    // dispose of the node but keep the buffer
    p = node->rq_buffer;
    head->rq_next = node->rq_next;
    ถ้า (node->rq_next == NULL)
	head->rq_prev = NULL;
    มิฉะนั้น
	node->rq_next->rq_prev = NULL;
    vim_free(node);
    คืน p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    static char_u *
channel_get_all(channel_T *channel, ch_part_T part, จำนวนเต็ม *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    สำหรับ (node = head->rq_next; node != NULL; node = node->rq_next)
	len += node->rq_buflen;
    res = alloc(len + 1);
    ถ้า (res == NULL)
	คืน NULL;
    p = res;
    สำหรับ (node = head->rq_next; node != NULL; node = node->rq_next)
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
    } ขณะ (p != NULL);

    ถ้า (outlen != NULL)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	คืน res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ขณะ (p < res + len)
    {
	ถ้า (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	มิฉะนั้น ถ้า (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ถ้า (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ขณะ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ถ้า (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    คืน res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    void
channel_consume(channel_T *channel, ch_part_T part, จำนวนเต็ม len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *buf = node->rq_buffer;

    ถ้า (len < 0 || (long_u)len > node->rq_buflen)
	คืน;
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
    จำนวนเต็ม
channel_collapse(channel_T *channel, ch_part_T part, จำนวนเต็ม want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*node = head->rq_next;
    readq_T	*last_node;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ถ้า (node == NULL || node->rq_next == NULL)
	คืน FAIL;

    last_node = node->rq_next;
    len = node->rq_buflen + last_node->rq_buflen;
    ถ้า (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	ขณะ (last_node->rq_next != NULL
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_node) == NULL))
	{
	    last_node = last_node->rq_next;
	    len += last_node->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ถ้า (newbuf == NULL)
	คืน FAIL;	    // out of memory
    mch_memmove(p, node->rq_buffer, node->rq_buflen);
    p += node->rq_buflen;
    vim_free(node->rq_buffer);
    node->rq_buffer = newbuf;
    สำหรับ (n = node; n != last_node; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    node->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    สำหรับ (n = node->rq_next; n != last_node; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    node->rq_next = last_node->rq_next;
    ถ้า (last_node->rq_next == NULL)
	head->rq_prev = node;
    มิฉะนั้น
	last_node->rq_next->rq_prev = node;
    vim_free(last_node);
    คืน OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    static จำนวนเต็ม
channel_save(channel_T *channel, ch_part_T part, char_u *buf, จำนวนเต็ม len,
						      จำนวนเต็ม prepend, var_na *lead)
{
    readq_T *node;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    จำนวนเต็ม	    i;

    node = ALLOC_ONE(readq_T);
    ถ้า (node == NULL)
	คืน FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    node->rq_buffer = alloc(len + 1);
    ถ้า (node->rq_buffer == NULL)
    {
	vim_free(node);
	คืน FAIL;	    // out of memory
    }

    ถ้า (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = node->rq_buffer;
	สำหรับ (i = 0; i < len; ++i)
	    ถ้า (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	node->rq_buflen = (long_u)(p - node->rq_buffer);
    }
    มิฉะนั้น
    {
	mch_memmove(node->rq_buffer, buf, len);
	node->rq_buffer[len] = NUL;
	node->rq_buflen = (long_u)len;
    }

    ถ้า (prepend)
    {
	// prepend node to the head of the queue
	node->rq_next = head->rq_next;
	node->rq_prev = NULL;
	ถ้า (head->rq_next == NULL)
	    head->rq_prev = node;
	มิฉะนั้น
	    head->rq_next->rq_prev = node;
	head->rq_next = node;
    }
    มิฉะนั้น
    {
	// append node to the tail of the queue
	node->rq_next = NULL;
	node->rq_prev = head->rq_prev;
	ถ้า (head->rq_prev == NULL)
	    head->rq_next = node;
	มิฉะนั้น
	    head->rq_prev->rq_next = node;
	head->rq_prev = node;
    }

    ถ้า (ch_log_active() && lead != NULL)
	ch_log_literal(lead, channel, part, buf, len);

    คืน OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    จำนวนเต็ม
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, NULL);
    จำนวนเต็ม		keeplen;
    จำนวนเต็ม		addlen;
    char_u	*p;

    ถ้า (next == NULL)
	คืน FALSE;

    keeplen = reader->js_end - reader->js_buf;
    ถ้า (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (จำนวนเต็ม)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	ถ้า (p == NULL)
	{
	    vim_free(next);
	    คืน FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    คืน TRUE;
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
    static จำนวนเต็ม
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*line_start;
    char_u	*p;
    int_u	hdr_len;
    จำนวนเต็ม		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ขณะ (TRUE)
    {
	line_start = p;
	ขณะ (*p != NUL && *p != '\n')
	    p++;
	ถ้า (*p == NUL)			// partial header
	    คืน MAYBE;
	p++;

	// process the content length field (if present)
	ถ้า ((p - line_start > 16)
		&& STRNICMP(line_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((var_na *)line_start + 16, NULL, 10);
	    ถ้า (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		คืน FAIL;
	}

	ถ้า ((p - line_start) == 2 && line_start[0] == '\r' &&
		line_start[1] == '\n')
	    // reached the empty line
	    bha_nga;
    }

    ถ้า (payload_len == -1)
	// Content-Length field is not present in the header
	คืน FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ถ้า (jsbuf_len < hdr_len + payload_len)
	คืน MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    คืน OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    จำนวนเต็ม
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    จำนวนเต็ม		status = OK;
    จำนวนเต็ม		ret;

    ถ้า (channel_peek(channel, part) == NULL)
	คืน FALSE;

    reader.js_buf = channel_get(channel, part, NULL);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ถ้า (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ถ้า (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ถ้า (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ถ้า (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	มิฉะนั้น ถ้า (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ถ้า (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    มิฉะนั้น
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	มิฉะนั้น
	{
	    item = ALLOC_ONE(jsonq_T);
	    ถ้า (item == NULL)
		clear_tv(&listtv);
	    มิฉะนั้น
	    {
		item->jq_no_callback = FALSE;
		item->jq_value = alloc_tv();
		ถ้า (item->jq_value == NULL)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		มิฉะนั้น
		{
		    *item->jq_value = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = NULL;
		    ถ้า (item->jq_prev == NULL)
			head->jq_next = item;
		    มิฉะนั้น
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    ถ้า (status == OK)
	chanpart->ch_wait_len = 0;
    มิฉะนั้น ถ้า (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ถ้า (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (จำนวนเต็ม)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = GetTickCount() + 100L;
#มิฉะนั้น
	    gettimeofday(&chanpart->ch_deadline, NULL);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ถ้า (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	มิฉะนั้น
	{
	    จำนวนเต็ม timeout;
#ifdef MSWIN
	    timeout = (จำนวนเต็ม)(GetTickCount() - chanpart->ch_deadline) > 0;
#มิฉะนั้น
	    {
		sa_nracanaa timeval now_tv;

		gettimeofday(&now_tv, NULL);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    ถ้า (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    มิฉะนั้น
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ถ้า (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    มิฉะนั้น ถ้า (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(จำนวนเต็ม)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, NULL);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    มิฉะนั้น
	ret = FALSE;

    vim_free(reader.js_buf);
    คืน ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    static void
remove_cb_node(cbq_T *head, cbq_T *node)
{
    ถ้า (node->cq_prev == NULL)
	head->cq_next = node->cq_next;
    มิฉะนั้น
	node->cq_prev->cq_next = node->cq_next;
    ถ้า (node->cq_next == NULL)
	head->cq_prev = node->cq_prev;
    มิฉะนั้น
	node->cq_next->cq_prev = node->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    void
remove_json_node(jsonq_T *head, jsonq_T *node)
{
    ถ้า (node->jq_prev == NULL)
	head->jq_next = node->jq_next;
    มิฉะนั้น
	node->jq_prev->jq_next = node->jq_next;
    ถ้า (node->jq_next == NULL)
	head->jq_prev = node->jq_prev;
    มิฉะนั้น
	node->jq_next->jq_prev = node->jq_prev;
    vim_free(node);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    static void
channel_add_block_id(chanpart_T *chanpart, จำนวนเต็ม id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    ถ้า (gap->ga_growsize == 0)
	ga_init2(gap, parimaa_na(จำนวนเต็ม), 10);
    ถ้า (ga_grow(gap, 1) == OK)
    {
	((จำนวนเต็ม *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    static void
channel_remove_block_id(chanpart_T *chanpart, จำนวนเต็ม id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    จำนวนเต็ม		i;

    สำหรับ (i = 0; i < gap->ga_len; ++i)
	ถ้า (((จำนวนเต็ม *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    ถ้า (i < gap->ga_len)
	    {
		จำนวนเต็ม *p = ((จำนวนเต็ม *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * parimaa_na(จำนวนเต็ม));
	    }
	    คืน;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    static จำนวนเต็ม
channel_has_block_id(chanpart_T *chanpart, จำนวนเต็ม id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    จำนวนเต็ม		i;

    สำหรับ (i = 0; i < gap->ga_len; ++i)
	ถ้า (((จำนวนเต็ม *)gap->ga_data)[i] == id)
	    คืน TRUE;
    คืน FALSE;
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
    static จำนวนเต็ม
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	จำนวนเต็ม	    id,
	จำนวนเต็ม	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    ขณะ (item != NULL)
    {
	list_T	    *l;
	typval_T    *tv;

	ถ้า (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_value->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	มิฉะนั้น
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ถ้า (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ถ้า (id > 0)
		{
		    ถ้า (item->jq_value->v_type != VAR_DICT)
			goto nextitem;
		    d = item->jq_value->vval.v_dict;
		    ถ้า (d == NULL)
			goto nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ถ้า (dict_has_key(d, "method"))
			goto nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    ถ้า (di == NULL)
			goto nextitem;
		    tv = &di->di_tv;
		}
		มิฉะนั้น
		    tv = item->jq_value;
	    }
	    มิฉะนั้น
	    {
		ถ้า (id > 0)
		{
		    ถ้า (item->jq_value->v_type != VAR_DICT)
			goto nextitem;
		    d = item->jq_value->vval.v_dict;
		    ถ้า (d == NULL)
			goto nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    ถ้า (di == NULL)
			goto nextitem;
		    tv = &di->di_tv;
		}
		มิฉะนั้น
		    tv = item->jq_value;
	    }
	}

	ถ้า ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_NUMBER && tv->vval.v_number == id)
	      || (id <= 0 && (tv->v_type != VAR_NUMBER
		 || tv->vval.v_number == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_number)))))
	{
	    *rettv = item->jq_value;
	    ถ้า (tv->v_type == VAR_NUMBER)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_number);
	    remove_json_node(head, item);
	    คืน OK;
	}
nextitem:
	item = item->jq_next;
    }
    คืน FAIL;
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

    ถ้า (head->jq_prev != NULL && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = NULL;
    มิฉะนั้น ขณะ (item != NULL && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    ถ้า (newitem == NULL)
    {
	clear_tv(rettv);
	คืน;
    }

    newitem->jq_value = alloc_tv();
    ถ้า (newitem->jq_value == NULL)
    {
	vim_free(newitem);
	clear_tv(rettv);
	คืน;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_value = *rettv;
    ถ้า (item == NULL)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = NULL;
	ถ้า (newitem->jq_prev == NULL)
	    head->jq_next = newitem;
	มิฉะนั้น
	    newitem->jq_prev->jq_next = newitem;
    }
    มิฉะนั้น
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	ถ้า (newitem->jq_next == NULL)
	    head->jq_prev = newitem;
	มิฉะนั้น
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
    จำนวนเต็ม	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    ถ้า (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ถ้า (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	คืน;
    }
    arg = argv[1].vval.v_string;
    ถ้า (arg == NULL)
	arg = (char_u *)"";

    ถ้า (STRCMP(cmd, "ex") == 0)
    {
	จำนวนเต็ม	called_emsg_before = called_emsg;
	char_u	*p = arg;
	จำนวนเต็ม	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (var_na *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ถ้า (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ถ้า (do_emsg_silent)
	    --emsg_silent;
	ถ้า (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (var_na *)get_vim_var_str(VV_ERRMSG));
    }
    มิฉะนั้น ถ้า (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (var_na *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_count = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    มิฉะนั้น ถ้า (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    มิฉะนั้น ถ้า (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	จำนวนเต็ม is_call = cmd[0] == 'c';
	จำนวนเต็ม id_idx = is_call ? 3 : 2;

	ถ้า (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_NUMBER)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ถ้า (p_verbose > 2)
		emsg(_(e_last_argument_for_expr_call_must_be_number));
	}
	มิฉะนั้น ถ้า (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ถ้า (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	มิฉะนั้น
	{
	    typval_T	*tv = NULL;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = NULL;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ถ้า (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (var_na *)arg);
		tv = eval_expr(arg, NULL);
	    }
	    มิฉะนั้น
	    {
		ch_log(channel, "Calling '%s'", (var_na *)arg);
		ถ้า (func_call(arg, &argv[2], NULL, NULL, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ถ้า (argv[id_idx].v_type == VAR_NUMBER)
	    {
		จำนวนเต็ม id = argv[id_idx].vval.v_number;

		ถ้า (tv != NULL)
		    json = json_encode_nr_expr(id, tv, options | JSON_NL);
		ถ้า (tv == NULL || (json != NULL && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_expr(id, &err_tv, options | JSON_NL);
		}
		ถ้า (json != NULL)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (จำนวนเต็ม)STRLEN(json), (var_na *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ถ้า (tv == &res_tv)
		clear_tv(tv);
	    มิฉะนั้น
		free_tv(tv);
	}
    }
    มิฉะนั้น ถ้า (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (var_na *)cmd);
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
					    (var_na *)item->cq_callback.cb_name);
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
    จำนวนเต็ม		save_p_ma = buffer->b_p_ma;
    จำนวนเต็ม		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    ถ้า (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ถ้า (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	คืน;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ถ้า (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (จำนวนเต็ม)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    ถ้า (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	คืน;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    ถ้า (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, msg, TRUE);
	lnum = 0;
    }
    มิฉะนั้น
	ml_append(lnum, msg, 0, FALSE);
    appended_lines_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ถ้า (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    มิฉะนั้น
	buffer->b_p_ma = save_p_ma;

    ถ้า (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ถ้า (wp->w_buffer == buffer)
	    {
		จำนวนเต็ม move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ถ้า (move_cursor || wp->w_topline > buffer->b_ml.ml_line_count)
		{
		    win_T *save_curwin = curwin;

		    ถ้า (move_cursor)
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

    ถ้า (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    ถ้า (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_line_count;
	}
    }
}

    static void
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *msg;

    ขณะ ((msg = channel_get(channel, part, NULL)) != NULL)
    {
	ch_log(channel, "Dropping message '%s'", (var_na *)msg);
	vim_free(msg);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    static จำนวนเต็ม
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    คืน ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    static จำนวนเต็ม
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*msg = NULL;
    blob_T	*blob = NULL;
    typval_T	*listtv = NULL;
    typval_T	argv[CH_JSON_MAX_ARGS];
    จำนวนเต็ม		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = NULL;
    buf_T	*buffer = NULL;
    char_u	*p;
    จำนวนเต็ม		called_otc;		// one time callbackup
    จำนวนเต็ม		raw_len = 0;

    ถ้า (channel->ch_nb_close_cb != NULL
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	คืน FALSE;

    // Use a message-specific callback, part callback or channel callback
    สำหรับ (cbitem = cbhead->cq_next; cbitem != NULL; cbitem = cbitem->cq_next)
	ถ้า (cbitem->cq_seq_nr == 0)
	    bha_nga;
    ถ้า (cbitem != NULL)
	callback = &cbitem->cq_callback;
    มิฉะนั้น ถ้า (ch_part->ch_callback.cb_name != NULL)
	callback = &ch_part->ch_callback;
    มิฉะนั้น ถ้า (channel->ch_callback.cb_name != NULL)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    ถ้า (buffer != NULL && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == NULL))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = NULL;
	buffer = NULL;
    }

    ถ้า (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	จำนวนเต็ม		argc = 0;

	// Get any json message in the queue.
	ถ้า (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ถ้า (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(void)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ถ้า (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		คืน FALSE;
	}

	ถ้า (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ถ้า (d != NULL)
	    {
		ถ้า (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		มิฉะนั้น
		    di = dict_find(d, (char_u *)"request_seq", -1);
		ถ้า (di != NULL && di->di_tv.v_type == VAR_NUMBER)
		    seq_nr = di->di_tv.vval.v_number;
	    }

	    argv[1] = *listtv;
	}
	มิฉะนั้น
	{
	    สำหรับ (item = listtv->vval.v_list->lv_first;
		    item != NULL && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    ขณะ (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ถ้า (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		คืน TRUE;
	    }

	    ถ้า (argv[0].v_type != VAR_NUMBER)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		คืน FALSE;
	    }
	    seq_nr = argv[0].vval.v_number;
	}
    }
    มิฉะนั้น ถ้า (channel_peek(channel, part) == NULL)
    {
	// nothing to read on RAW or NL channel
	คืน FALSE;
    }
    มิฉะนั้น
    {
	// If there is no callback or buffer drop the message.
	ถ้า (callback == NULL && buffer == NULL)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ถ้า (channel->ch_close_cb.cb_name == NULL && !channel->ch_drop_never)
		drop_messages(channel, part);
	    คืน FALSE;
	}

	ถ้า (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = NULL;
	    char_u  *buf;
	    readq_T *node;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ขณะ (TRUE)
	    {
		node = channel_peek(channel, part);
		nl = channel_first_nl(node);
		ถ้า (nl != NULL)
		    bha_nga;
		ถ้า (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ถ้า (ch_part->ch_fd == INVALID_FD && node->rq_buflen > 0)
			bha_nga;
		    คืน FALSE; // incomplete message
		}
	    }
	    buf = node->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    สำหรับ (p = buf; (nl == NULL || p < nl)
					    && p < buf + node->rq_buflen; ++p)
		ถ้า (*p == NUL)
		    *p = NL;

	    ถ้า (nl == NULL)
	    {
		// get the whole buffer, drop the NL
		msg = channel_get(channel, part, NULL);
	    }
	    มิฉะนั้น ถ้า (nl + 1 == buf + node->rq_buflen)
	    {
		// get the whole buffer
		msg = channel_get(channel, part, NULL);
		*nl = NUL;
	    }
	    มิฉะนั้น
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		msg = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (จำนวนเต็ม)(nl - buf) + 1);
	    }
	}
	มิฉะนั้น
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    msg = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : NULL);
	}

	ถ้า (msg == NULL)
	    คืน FALSE; // out of memory (and avoids Coverity warning)

	ถ้า (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    ถ้า (blob == NULL)
	    {
		vim_free(msg);
		คืน FALSE;
	    }
	    ถ้า (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(msg);
		คืน FALSE;
	    }
	    ถ้า (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, msg, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	มิฉะนั้น
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = msg;
	}
    }

    called_otc = FALSE;
    ถ้า (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	จำนวนเต็ม lsp_req_msg = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ถ้า ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_key(argv[1].vval.v_dict, "method"))
	    lsp_req_msg = TRUE;

	ถ้า (!lsp_req_msg)
	{
	    สำหรับ (cbitem = cbhead->cq_next; cbitem != NULL;
		    cbitem = cbitem->cq_next)
	    {
		ถ้า (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    bha_nga;
		}
	    }
	}
    }

    ถ้า (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	ถ้า (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ถ้า (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_push_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
		listtv = NULL;
	    }
	    มิฉะนั้น
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    มิฉะนั้น ถ้า (callback != NULL || buffer != NULL)
    {
	ถ้า (buffer != NULL)
	{
	    ถ้า (msg == NULL)
		// JSON or JS mode: re-encode the message.
		msg = json_encode(listtv, ch_mode);
	    ถ้า (msg != NULL)
	    {
#ifdef FEAT_TERMINAL
		ถ้า (buffer->b_term != NULL)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ถ้า (part == PART_ERR)
		    {
			char_u *cp;
			จำนวนเต็ม    extra = 0;

			สำหรับ (cp = msg; *cp != NUL; ++cp)
			    ถ้า (*cp == NL && (cp == msg || cp[-1] != CAR))
				++extra;
			ถ้า (extra > 0)
			{
			    char_u *crlf_msg = alloc(STRLEN(msg) + extra + 1);

			    ถ้า (crlf_msg != NULL)
			    {
				char_u *q = crlf_msg;

				สำหรับ (cp = msg; *cp != NUL; ++cp)
				{
				    ถ้า (*cp == NL && (cp == msg || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_msg, channel);
				vim_free(crlf_msg);
			    }
			}
			มิฉะนั้น
			    write_to_term(buffer, msg, channel);
		    }
		    มิฉะนั้น
			write_to_term(buffer, msg, channel);
		}
		มิฉะนั้น
#endif
		    append_to_buffer(buffer, msg, channel, part);
	    }
	}

	ถ้า (callback != NULL)
	{
	    ถ้า (cbitem != NULL)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    มิฉะนั้น
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (var_na *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    มิฉะนั้น
	ch_log(channel, "Dropping message %d", seq_nr);

    ถ้า (listtv != NULL)
	free_tv(listtv);
    ถ้า (blob != NULL)
	blob_unref(blob);
    vim_free(msg);

    คืน TRUE;
}

#ถ้า defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    จำนวนเต็ม
channel_can_write_to(channel_T *channel)
{
    คืน channel != NULL && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    จำนวนเต็ม
channel_is_open(channel_T *channel)
{
    คืน channel != NULL && (channel->CH_SOCK_FD != INVALID_FD
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
    ถ้า (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	ถ้า (head->jq_next == NULL)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	คืน head->jq_next;
    }
    คืน channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    static จำนวนเต็ม
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    คืน channel_readahead_pointer(channel, part) != NULL;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    static var_na *
channel_status(channel_T *channel, จำนวนเต็ม req_part)
{
    ch_part_T part;
    จำนวนเต็ม has_readahead = FALSE;

    ถ้า (channel == NULL)
	 คืน "fail";
    ถ้า (req_part == PART_OUT)
    {
	ถ้า (channel->CH_OUT_FD != INVALID_FD)
	    คืน "open";
	ถ้า (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    มิฉะนั้น ถ้า (req_part == PART_ERR)
    {
	ถ้า (channel->CH_ERR_FD != INVALID_FD)
	    คืน "open";
	ถ้า (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    มิฉะนั้น
    {
	ถ้า (channel_is_open(channel))
	    คืน "open";
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	    ถ้า (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		bha_nga;
	    }
    }

    ถ้า (has_readahead)
	คืน "buffered";
    คืน "closed";
}

    static void
channel_part_info(channel_T *channel, dict_T *dict, var_na *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    var_na	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    ถ้า (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    มิฉะนั้น ถ้า (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    มิฉะนั้น
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (จำนวนเต็ม)s.length);

    STRCPY(namebuf + tail, "mode");
    switch (chanpart->ch_mode)
    {
	pak_sa CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    bha_nga;
	pak_sa CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    bha_nga;
	pak_sa CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    bha_nga;
	pak_sa CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    bha_nga;
	pak_sa CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    bha_nga;
	pak_sa CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    bha_nga;
	pak_sa CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    bha_nga;
	default:
	    STR_LITERAL_SET(s, "");
	    bha_nga;
    }
    dict_add_string_len(dict, namebuf, s.string, (จำนวนเต็ม)s.length);

    STRCPY(namebuf + tail, "io");
    ถ้า (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    มิฉะนั้น switch (chanpart->ch_io)
    {
	pak_sa JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    bha_nga;
	pak_sa JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    bha_nga;
	pak_sa JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    bha_nga;
	pak_sa JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    bha_nga;
	pak_sa JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    bha_nga;
	default:
	    STR_LITERAL_SET(s, "");
	    bha_nga;
    }
    dict_add_string_len(dict, namebuf, s.string, (จำนวนเต็ม)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_number(dict, namebuf, chanpart->ch_timeout);
}

    static void
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_number(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    ถ้า (channel->ch_hostname != NULL)
    {
	ถ้า (channel->ch_port)
	    dict_add_number(dict, "port", channel->ch_port);
	มิฉะนั้น
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    มิฉะนั้น
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
channel_close(channel_T *channel, จำนวนเต็ม invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    ถ้า (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	ถ้า (channel->ch_close_cb.cb_name != NULL)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    ถ้า (channel->ch_close_cb.cb_name != NULL
			    || channel->ch_part[part].ch_bufref.br_buf != NULL)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ถ้า (channel->ch_close_cb.cb_name == NULL)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		ขณะ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ถ้า (channel->ch_close_cb.cb_name != NULL)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (var_na *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      ถ้า (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ถ้า (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
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
    ถ้า (wq->wq_next == NULL)
	wq->wq_prev = NULL;
    มิฉะนั้น
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

    ขณะ (channel_peek(channel, part) != NULL)
	vim_free(channel_get(channel, part, NULL));

    ขณะ (cb_head->cq_next != NULL)
    {
	cbq_T *node = cb_head->cq_next;

	remove_cb_node(cb_head, node);
	free_callback(&node->cq_callback);
	vim_free(node);
    }

    ขณะ (json_head->jq_next != NULL)
    {
	free_tv(json_head->jq_next->jq_value);
	remove_json_node(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    ขณะ (ch_part->ch_writeque.wq_next != NULL)
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

#ถ้า defined(EXITFREE)
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
    static จำนวนเต็ม
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    ถ้า (in_part->ch_writeque.wq_next != NULL)
	คืน TRUE;
    ถ้า (buf == NULL)
	คืน FALSE;
    คืน in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_line_count)
	    : (in_part->ch_buf_top <= in_part->ch_buf_bot
			    && in_part->ch_buf_top <= buf->b_ml.ml_line_count);
}

#ถ้า defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    static จำนวนเต็ม
channel_fill_wfds(จำนวนเต็ม maxfd_arg, fd_set *wfds)
{
    จำนวนเต็ม		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ถ้า (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (จำนวนเต็ม)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((จำนวนเต็ม)in_part->ch_fd, wfds);
	    ถ้า ((จำนวนเต็ม)in_part->ch_fd >= maxfd)
		maxfd = (จำนวนเต็ม)in_part->ch_fd + 1;
	}
    }
    คืน maxfd;
}
#มิฉะนั้น
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    static จำนวนเต็ม
channel_fill_poll_write(จำนวนเต็ม nfd_in, sa_nracanaa pollfd *fds)
{
    จำนวนเต็ม		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ถ้า (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	มิฉะนั้น
	    in_part->ch_poll_idx = -1;
    }
    คืน nfd;
}
#endif

typedef ga_nanaa {
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
channel_wait(channel_T *channel, sock_T fd, จำนวนเต็ม timeout)
{
    ถ้า (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    ถ้า (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	จำนวนเต็ม	sleep_time;
	DWORD	deadline = GetTickCount() + timeout;
	จำนวนเต็ม	delay = 1;

	// reading from a pipe, not a socket
	ขณะ (TRUE)
	{
	    จำนวนเต็ม r = PeekNamedPipe((HANDLE)fd, NULL, 0, NULL, &nread, NULL);

	    ถ้า (r && nread > 0)
		คืน CW_READY;

	    ถ้า (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, NULL);
	    }
	    มิฉะนั้น ถ้า (r == 0)
		คืน CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_lines();

	    sleep_time = deadline - GetTickCount();
	    ถ้า (sleep_time <= 0)
		bha_nga;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ถ้า (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    ถ้า (delay > 10)
		delay = 10;
	}
    }
    มิฉะนั้น
#endif
    {
#ถ้า defined(HAVE_SELECT)
	sa_nracanaa timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	จำนวนเต็ม		ret;
	จำนวนเต็ม		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	สำหรับ (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((จำนวนเต็ม)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (จำนวนเต็ม)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, NULL, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ถ้า (ret == -1 && errno == EINTR)
		continue;
# endif
	    ถ้า (ret > 0)
	    {
		ถ้า (FD_ISSET(fd, &rfds))
		    คืน CW_READY;
		channel_write_any_lines();
		continue;
	    }
	    bha_nga;
	}
#มิฉะนั้น
	สำหรับ (;;)
	{
	    sa_nracanaa pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    จำนวนเต็ม		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ถ้า (poll(fds, nfd, timeout) > 0)
	    {
		ถ้า (fds[0].revents & POLLIN)
		    คืน CW_READY;
		channel_write_any_lines();
		continue;
	    }
	    bha_nga;
	}
#endif
    }
    คืน CW_NOT_READY;
}

    static void
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, จำนวนเต็ม is_err, var_na *func)
{
    var_na	msg[] = "%s(): Read %s from ch_part[%d], closing";

    ถ้า (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, msg, func, "error", part);
    มิฉะนั้น
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
    ถ้า (channel->ch_nb_close_cb != NULL)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_MSG_RAW,
			      (จำนวนเต็ม)STRLEN_LITERAL(DETACH_MSG_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ถ้า (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	ถ้า (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
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
    ถ้า (channel->ch_nb_close_cb != NULL)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    ถ้า (channel->ch_ss_close_cb != NULL)
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
channel_read(channel_T *channel, ch_part_T part, var_na *func)
{
    static char_u	*buf = NULL;
    จำนวนเต็ม			len = 0;
    จำนวนเต็ม			readlen = 0;
    sock_T		fd;
    จำนวนเต็ม			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ถ้า (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	คืน;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ถ้า (buf == NULL)
    {
	buf = alloc(MAXMSGSIZE);
	ถ้า (buf == NULL)
	    คืน;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    สำหรับ (;;)
    {
	ถ้า (channel_wait(channel, fd, 0) != CW_READY)
	    bha_nga;
	ถ้า (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    sa_nracanaa sockaddr_storage	client;

	    newchannel = add_channel();
	    ถ้า (newchannel == NULL)
	    {
		ch_error(NULL, "Cannot allocate channel.");
		คืน;
	    }
	    socklen = parimaa_na(client);
	    newfd = accept(fd, (sa_nracanaa sockaddr*)&client, &socklen);
	    ถ้า (newfd < 0)
	    {
		ch_error(NULL, "Cannot accept channel.");
		channel_free(newchannel);
		คืน;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    ถ้า (channel->ch_ss_accept_cb != NULL)
	    {
		channel->ch_ss_accept_cb(newchannel);
		คืน;
	    }
#endif

	    ถ้า (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		var_na addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((sa_nracanaa sockaddr_in*)&client)->sin_addr,
			addr, parimaa_na(addr));
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf), "%s:%d",
			addr,
			ntohs(((sa_nracanaa sockaddr_in*)&client)->sin_port));
#มิฉะนั้น
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf), "%s:%d",
		    inet_ntoa(((sa_nracanaa sockaddr_in*)&client)->sin_addr),
		    ntohs(((sa_nracanaa sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    มิฉะนั้น ถ้า (client.ss_family == AF_INET6)
	    {
		var_na addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((sa_nracanaa sockaddr_in6*)&client)->sin6_addr,
			addr, parimaa_na(addr));
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf), "[%s]:%d",
			addr,
			ntohs(((sa_nracanaa sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    มิฉะนั้น ถ้า (client.ss_family == AF_UNIX)
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf),
							    "unix:anonymous");
	    มิฉะนั้น
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    คืน;
	}
	ถ้า (use_socket)
	    len = sock_read(fd, (var_na *)buf, MAXMSGSIZE);
	มิฉะนั้น
	    len = fd_read(fd, (var_na *)buf, MAXMSGSIZE);
	ถ้า (len <= 0)
	    bha_nga;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ถ้า (readlen <= 0)
    {
	ถ้า (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ถ้า defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    มิฉะนั้น ถ้า (CH_HAS_GUI && gtk_main_level() > 0)
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
	channel_T *channel, ch_part_T part, จำนวนเต็ม timeout, จำนวนเต็ม raw, จำนวนเต็ม *outlen)
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

    ขณะ (TRUE)
    {
	node = channel_peek(channel, part);
	ถ้า (node != NULL)
	{
	    ถ้า (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(node) != NULL))
		// got a complete message
		bha_nga;
	    ถ้า (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		continue;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ถ้า (raw || fd == INVALID_FD)
		bha_nga;
	}

	// Wait for up to the channel timeout.
	ถ้า (fd == INVALID_FD)
	    คืน NULL;
	ถ้า (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    คืน NULL;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ถ้า (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != NULL)
    {
	msg = channel_get_all(channel, part, outlen);
    }
    มิฉะนั้น
    {
	char_u *p;

	buf = node->rq_buffer;
	nl = channel_first_nl(node);

	// Convert NUL to NL, the internal representation.
	สำหรับ (p = buf; (nl == NULL || p < nl) && p < buf + node->rq_buflen; ++p)
	    ถ้า (*p == NUL)
		*p = NL;

	ถ้า (nl == NULL)
	{
	    // must be a closed channel with missing NL
	    msg = channel_get(channel, part, NULL);
	}
	มิฉะนั้น ถ้า (nl + 1 == buf + node->rq_buflen)
	{
	    // get the whole buffer
	    msg = channel_get(channel, part, NULL);
	    *nl = NUL;
	}
	มิฉะนั้น
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    msg = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (จำนวนเต็ม)(nl - buf) + 1);
	}
    }
    ถ้า (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != NULL ? *outlen : (จำนวนเต็ม)STRLEN(msg));
    คืน msg;
}

static จำนวนเต็ม channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    จำนวนเต็ม
channel_in_blocking_wait(void)
{
    คืน channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    static void
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, จำนวนเต็ม timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (void)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#มิฉะนั้น
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
    static จำนวนเต็ม
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	จำนวนเต็ม	    timeout_arg,
	จำนวนเต็ม	    id,
	typval_T    **rettv)
{
    จำนวนเต็ม		more;
    sock_T	fd;
    จำนวนเต็ม		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    จำนวนเต็ม		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ถ้า (id >= 0)
	channel_add_block_id(chanpart, id);

    สำหรับ (;;)
    {
	ถ้า (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (void)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	ถ้า (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    bha_nga;
	}

	ถ้า (!more)
	{
	    void *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    void *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ถ้า (channel_parse_messages())
		continue;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    ถ้า (readahead_ptr != NULL && readahead_ptr != prev_readahead_ptr)
		continue;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (จำนวนเต็ม)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    ถ้า (timeout <= 0 || fd == INVALID_FD)
	    {
		ถ้า (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		bha_nga;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ถ้า (chanpart->ch_wait_len > 0)
	    {
		จำนวนเต็ม	left;
#ifdef MSWIN
		left = chanpart->ch_deadline - GetTickCount() + 1;
#มิฉะนั้น
		{
		    sa_nracanaa timeval now_tv;

		    gettimeofday(&now_tv, NULL);
		    left = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		ถ้า (left < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		มิฉะนั้น ถ้า (left < timeout)
		    timeout = left;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    ถ้า (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    คืน retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, จำนวนเต็ม check_open, จำนวนเต็ม reading, ch_part_T part)
{
    channel_T	*channel = NULL;
    จำนวนเต็ม		has_readahead = FALSE;

    ถ้า (tv->v_type == VAR_JOB)
    {
	ถ้า (tv->vval.v_job != NULL)
	    channel = tv->vval.v_job->jv_channel;
    }
    มิฉะนั้น ถ้า (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    มิฉะนั้น
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	คืน NULL;
    }
    ถ้า (channel != NULL && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_COUNT ? part : channel_part_read(channel));

    ถ้า (check_open && (channel == NULL || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	คืน NULL;
    }
    คืน channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    static void
common_channel_read(typval_T *argvars, typval_T *rettv, จำนวนเต็ม raw, จำนวนเต็ม blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_COUNT;
    jobopt_T	opt;
    จำนวนเต็ม		mode;
    จำนวนเต็ม		timeout;
    จำนวนเต็ม		id = -1;
    typval_T	*listtv = NULL;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ถ้า (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	คืน;

    clear_job_options(&opt);
    ถ้า (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	goto theend;

    ถ้า (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ถ้า (channel == NULL)
	goto theend;

    ถ้า (part == PART_COUNT)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    ถ้า (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ถ้า (blob || mode == CH_MODE_BLOB)
    {
	จำนวนเต็ม	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	ถ้า (p != NULL)
	{
	    blob_T	*b = blob_alloc();

	    ถ้า (b != NULL)
	    {
		b->bv_ga.ga_len = outlen;
		ถ้า (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		มิฉะนั้น
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    มิฉะนั้น ถ้า (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, NULL);
    มิฉะนั้น
    {
	ถ้า (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	ถ้า (listtv != NULL)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	มิฉะนั้น
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#ถ้า defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    void
channel_handle_events(จำนวนเต็ม only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ถ้า (only_keep_open && !channel->ch_keep_open)
	    continue;

	// check the socket and pipes
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ถ้า (fd == INVALID_FD)
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
	    จำนวนเต็ม r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    ถ้า (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    มิฉะนั้น ถ้า (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ถ้า (channel->ch_job != NULL)
	{
	    job_T *job = channel->ch_job;

	    ถ้า (job->jv_tty_out != NULL && job->jv_status == JOB_FINISHED)
		สำหรับ (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#ถ้า defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    จำนวนเต็ม
channel_any_keep_open(void)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ถ้า (channel->ch_keep_open)
	    คืน TRUE;
    คืน FALSE;
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
    จำนวนเต็ม		fd = ch_part->ch_fd;

    ถ้า (fd == INVALID_FD)
	คืน;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#มิฉะนั้น
    (void)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    จำนวนเต็ม
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	จำนวนเต็ม	  len_arg,
	var_na	  *fun)
{
    จำนวนเต็ม		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    จำนวนเต็ม		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    ถ้า (fd == INVALID_FD)
    {
	ถ้า (!channel->ch_error && fun != NULL)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	คืน FAIL;
    }

    ถ้า (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    ถ้า (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_msg = 0;
    }

    สำหรับ (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	จำนวนเต็ม	    len;

	ถ้า (wq->wq_next != NULL)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	มิฉะนั้น
	{
	    ถ้า (len_arg == 0)
		// nothing to write, called from channel_select_check()
		คืน OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ถ้า (part == PART_SOCK)
	    res = sock_write(fd, (var_na *)buf, len);
	มิฉะนั้น
	{
	    res = fd_write(fd, (var_na *)buf, len);
#ifdef MSWIN
	    ถ้า (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, NULL);
	    }
#endif
	}
	ถ้า (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	ถ้า (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    ถ้า (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    ถ้า (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ถ้า (entry != NULL)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    continue;
		}
		ถ้า (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    มิฉะนั้น
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ถ้า (entry != NULL)
		{
		    ถ้า (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (var_na *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		มิฉะนั้น
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ถ้า (wq->wq_prev != NULL
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    ถ้า (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((var_na *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		มิฉะนั้น
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    ถ้า (last != NULL)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = NULL;
			ถ้า (wq->wq_prev == NULL)
			    wq->wq_next = last;
			มิฉะนั้น
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			ถ้า (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	มิฉะนั้น ถ้า (res != len)
	{
	    ถ้า (!channel->ch_error && fun != NULL)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    คืน FAIL;
	}

	channel->ch_error = FALSE;
	คืน OK;
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
	จำนวนเต็ม	    len,
	จำนวนเต็ม	    id,
	จำนวนเต็ม	    eval,
	jobopt_T    *opt,
	var_na	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ถ้า (channel == NULL)
	คืน NULL;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ถ้า (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	คืน NULL;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ถ้า (opt->jo_callback.cb_name != NULL && *opt->jo_callback.cb_name != NUL)
    {
	ถ้า (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    คืน NULL;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ถ้า (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == NULL)
	คืน channel;
    คืน NULL;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    static void
ch_expr_common(typval_T *argvars, typval_T *rettv, จำนวนเต็ม eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    จำนวนเต็ม		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    จำนวนเต็ม		timeout;
    จำนวนเต็ม		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ถ้า (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	คืน;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ถ้า (channel == NULL)
	คืน;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    ถ้า (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	คืน;
    }

    ถ้า (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	var_na		*key = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	ถ้า (rettv_dict_alloc(rettv) == FAIL)
	    คืน;

	ถ้า (check_for_dict_arg(argvars, 1) == FAIL)
	    คืน;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)key, -1);
	ถ้า (argvars[2].v_type == VAR_DICT
		&& dict_has_key(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ถ้า (di != NULL && di->di_tv.v_type != VAR_NUMBER
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_value_for_argument_str), key);
	    คืน;
	}

	ถ้า (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_msg_id;
	    ถ้า (di == NULL)
		dict_add_number(d, "seq", id);
	    มิฉะนั้น
		di->di_tv.vval.v_number = id;
	}
	มิฉะนั้น ถ้า (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_msg_id;
	    ถ้า (di == NULL)
		dict_add_number(d, "id", id);
	    มิฉะนั้น
		di->di_tv.vval.v_number = id;
	}
	มิฉะนั้น
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ถ้า (di != NULL && di->di_tv.v_type == VAR_NUMBER)
		id = di->di_tv.vval.v_number;
	}
	ถ้า (ch_mode == CH_MODE_LSP && !dict_has_key(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_msg(&argvars[1]);
    }
    มิฉะนั้น
    {
	id = ++channel->ch_last_msg_id;
	text = json_encode_nr_expr(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    ถ้า (text == NULL)
	คืน;

    channel = send_common(argvars, text, (จำนวนเต็ม)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    ถ้า (channel != NULL && eval)
    {
	ถ้า (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	มิฉะนั้น
	    timeout = channel_get_timeout(channel, part_read);
	ถ้า (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ถ้า (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
	    }
	    มิฉะนั้น
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
    ถ้า (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ถ้า (rettv->vval.v_dict != NULL)
	    dict_add_number(rettv->vval.v_dict, "seq", id);
    }
    มิฉะนั้น ถ้า (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ถ้า (rettv->vval.v_dict != NULL)
	    dict_add_number(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    static void
ch_raw_common(typval_T *argvars, typval_T *rettv, จำนวนเต็ม eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    จำนวนเต็ม		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    จำนวนเต็ม		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ถ้า (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	คืน;

    ถ้า (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    มิฉะนั้น
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (จำนวนเต็ม)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ถ้า (channel != NULL && eval)
    {
	ถ้า (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	มิฉะนั้น
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, NULL);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#ถ้า defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    จำนวนเต็ม
channel_poll_setup(จำนวนเต็ม nfd_in, void *fds_in, จำนวนเต็ม *towait)
{
    จำนวนเต็ม		nfd = nfd_in;
    channel_T	*channel;
    sa_nracanaa	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ถ้า (ch_part->ch_fd != INVALID_FD)
	    {
		ถ้า (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ถ้า (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		มิฉะนั้น
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    มิฉะนั้น
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    คืน nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    จำนวนเต็ม
channel_poll_check(จำนวนเต็ม ret_in, void *fds_in)
{
    จำนวนเต็ม		ret = ret_in;
    channel_T	*channel;
    sa_nracanaa	pollfd *fds = fds_in;
    ch_part_T	part;
    จำนวนเต็ม		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ถ้า (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    มิฉะนั้น ถ้า (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	ถ้า (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_input(channel);
	    --ret;
	}
    }

    คืน ret;
}
#endif // UNIX && !HAVE_SELECT

#ถ้า !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    จำนวนเต็ม
channel_select_setup(
	จำนวนเต็ม maxfd_in,
	void *rfds_in,
	void *wfds_in,
	sa_nracanaa timeval *tv,
	sa_nracanaa timeval **tvp)
{
    จำนวนเต็ม		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ถ้า (fd != INVALID_FD)
	    {
		ถ้า (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ถ้า (*tvp == NULL || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		มิฉะนั้น
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ถ้า ((จำนวนเต็ม)fd >= FD_SETSIZE)
			continue;
# endif
		    FD_SET((จำนวนเต็ม)fd, rfds);
		    ถ้า (maxfd < (จำนวนเต็ม)fd)
			maxfd = (จำนวนเต็ม)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    คืน maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    จำนวนเต็ม
channel_select_check(จำนวนเต็ม ret_in, void *rfds_in, void *wfds_in)
{
    จำนวนเต็ม		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	สำหรับ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ถ้า (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (จำนวนเต็ม)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    มิฉะนั้น ถ้า (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	ถ้า (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (จำนวนเต็ม)in_part->ch_fd < FD_SETSIZE
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
	ถ้า (channel->ch_job != NULL)
	{
	    job_T *job = channel->ch_job;

	    ถ้า (job->jv_tty_out != NULL && job->jv_status == JOB_FINISHED)
		สำหรับ (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    คืน ret;
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
    ถ้า (stdio_channel->CH_OUT_FD != INVALID_FD)
	(void)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(จำนวนเต็ม)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#elif defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    sa_nracanaa timeval	tv;
    sa_nracanaa timeval	*tvp = &tv;
    จำนวนเต็ม			maxfd;
    จำนวนเต็ม			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, NULL, tvp);
    ถ้า (ret >= 0)
	(void)channel_select_check(ret, &rfds, &wfds);
#มิฉะนั้น
    sa_nracanaa pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    จำนวนเต็ม			nfd;
    จำนวนเต็ม			towait = (จำนวนเต็ม)msec;
    จำนวนเต็ม			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ถ้า (ret >= 0)
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
    ขณะ (stdio_channel != NULL && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	ถ้า (due_time > 0 && due_time < msec)
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
    จำนวนเต็ม
channel_parse_messages(void)
{
    channel_T	*channel = first_channel;
    จำนวนเต็ม		ret = FALSE;
    จำนวนเต็ม		r;
    ch_part_T	part = PART_SOCK;
    static จำนวนเต็ม	recursive = 0;
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
    ถ้า ((did_repeated_msg & REPEATED_MSG_LOOKING) == 0)
    {
	ch_log(NULL, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_msg = REPEATED_MSG_LOOKING;
    }
    ขณะ (channel != NULL)
    {
	ถ้า (recursive == 1)
	{
	    ถ้า (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_COUNT);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		continue;
	    }
	    ถ้า (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ถ้า (channel->ch_job != NULL)
		    channel->ch_job->jv_channel = NULL;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		continue;
	    }
	    ถ้า (channel->ch_refcount == 0 && !channel_still_useful(channel))
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
	ถ้า (part == PART_OUT
		&& channel->ch_job != NULL
		&& channel->ch_job->jv_tty_out != NULL
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    continue;
	}
#endif
	ถ้า (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ถ้า (r == OK)
		ret = TRUE;
	    ถ้า (channel_unref(channel) || (r == OK
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
	ถ้า (part < PART_ERR)
	    ++part;
	มิฉะนั้น
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    ถ้า (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    คืน ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    จำนวนเต็ม
channel_any_readahead(void)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    ขณะ (channel != NULL)
    {
	ถ้า (channel_has_readahead(channel, part))
	    คืน TRUE;
	ถ้า (part < PART_ERR)
	    ++part;
	มิฉะนั้น
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    คืน FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    จำนวนเต็ม
set_ref_in_channel(จำนวนเต็ม copyID)
{
    จำนวนเต็ม		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    สำหรับ (channel = first_channel; !abort && channel != NULL;
						   channel = channel->ch_next)
	ถ้า (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
	}
    คืน abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    static ch_part_T
channel_part_send(channel_T *channel)
{
    ถ้า (channel->CH_SOCK_FD == INVALID_FD)
	คืน PART_IN;
    คืน PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    static ch_part_T
channel_part_read(channel_T *channel)
{
    ถ้า (channel->CH_SOCK_FD == INVALID_FD)
	คืน PART_OUT;
    คืน PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    static ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    ถ้า (channel == NULL)
	คืน CH_MODE_JSON;
    คืน channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    static จำนวนเต็ม
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    คืน channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    void
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_number = 0;
    ถ้า (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	คืน;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ถ้า (channel != NULL)
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

    ถ้า (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	คืน;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ถ้า (channel != NULL)
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

    ถ้า (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	คืน;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ถ้า (channel != NULL)
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

    ถ้า (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	คืน;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ถ้า (channel == NULL)
	คืน;

    char_u	*what = tv_get_string(&argvars[1]);
    จำนวนเต็ม		part;
    ถ้า (STRCMP(what, "err") == 0)
	part = PART_ERR;
    มิฉะนั้น ถ้า (STRCMP(what, "out") == 0)
	part = PART_OUT;
    มิฉะนั้น ถ้า (STRCMP(what, "in") == 0)
	part = PART_IN;
    มิฉะนั้น
	part = PART_SOCK;
    ถ้า (channel->ch_part[part].ch_bufref.br_buf != NULL)
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

    ถ้า (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	คืน;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ถ้า (channel == NULL)
	คืน;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    ถ้า (channel->ch_job != NULL)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    void
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ถ้า (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	คืน;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ถ้า (channel != NULL && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    void
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ถ้า (check_restricted() || check_secure())
	คืน;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    void
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ถ้า (check_restricted() || check_secure())
	คืน;
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

    ถ้า (check_restricted() || check_secure())
	คืน;

    ถ้า (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	คืน;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ถ้า (channel == NULL)
	คืน;
    clear_job_options(&opt);
    ถ้า (get_job_options(&argvars[1], &opt,
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
    จำนวนเต็ม		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ถ้า (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	คืน;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ถ้า (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	ถ้า (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    var_na      *status = channel_status(channel, -1);

    ถ้า (channel == NULL)
	vim_snprintf((var_na *)buf, NUMBUFLEN, "channel %s", status);
    มิฉะนั้น
	vim_snprintf((var_na *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    คืน buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(จำนวนเต็ม ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ถ้า (ch->ch_id == ch_id)
	    คืน ch;
    คืน NULL;
}

#endif // FEAT_JOB_CHANNEL
