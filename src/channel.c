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

#ᤀᤡ᤺ defined(FEAT_JOB_CHANNEL)

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
#ᤀᤡ᤺ᤆᤡ
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
static puur_naa_nka channel_get_timeout(channel_T *channel, ch_part_T part);
static channel_T *channel_open_stdio(void);
static ch_part_T channel_part_send(channel_T *channel);
static ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    for ((ch) = first_channel; (ch) != NULL; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
static puur_naa_nka safe_to_invoke_callback = 0;

#ifdef MSWIN
    static puur_naa_nka
fd_read(sock_T fd, var_na *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ᤀᤡ᤺ (!ReadFile(h, buf, (DWORD)len, &nread, NULL))
	ᤀᤡᤴ -1;
    ᤀᤡᤴ (puur_naa_nka)nread;
}

    static puur_naa_nka
fd_write(sock_T fd, var_na *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    ᤀᤠ᤺ (todo > 0)
    {
	ᤀᤡ᤺ (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	ᤀᤡ᤺ᤆᤡ
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	ᤀᤡ᤺ (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    ᤀᤡ᤺ (err != ERROR_IO_PENDING)
		ᤀᤡᤴ -1;
	    ᤀᤡ᤺ (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		ᤀᤡᤴ -1;
	    FlushFileBuffers(h);
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    bha_nga;
	todo -= nwrite;
	done += nwrite;
    }
    ᤀᤡᤴ (puur_naa_nka)done;
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
strerror_win32(puur_naa_nka eno)
{
    static LPVOID msgbuf = NULL;
    char_u *ptr;

    ᤀᤡ᤺ (msgbuf)
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
    ᤀᤡ᤺ (msgbuf != NULL)
	// chomp \r or \n
	for (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    switch (*ptr)
	    {
		pak_sa '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    bha_nga;
		pak_sa '\n':
		    ᤀᤡ᤺ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    ᤀᤡ᤺ᤆᤡ
			*ptr = ' ';
		    bha_nga;
	    }
    ᤀᤡᤴ msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
static channel_T *first_channel = NULL;
static puur_naa_nka next_ch_id = 0;

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

    ᤀᤡ᤺ (channel == NULL)
	ᤀᤡᤴ NULL;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    for (part = PART_SOCK; part < PART_COUNT; ++part)
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

    ᤀᤡ᤺ (first_channel != NULL)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    ᤀᤡᤴ channel;
}

    puur_naa_nka
has_any_channel(void)
{
    ᤀᤡᤴ first_channel != NULL;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    puur_naa_nka
channel_still_useful(channel_T *channel)
{
    puur_naa_nka has_sock_msg;
    puur_naa_nka	has_out_msg;
    puur_naa_nka	has_err_msg;

    // If the job was killed the channel is not expected to work anymore.
    ᤀᤡ᤺ (channel->ch_job_killed && channel->ch_job == NULL)
	ᤀᤡᤴ FALSE;

    // If there is a close callback it may still need to be invoked.
    ᤀᤡ᤺ (channel->ch_close_cb.cb_name != NULL)
	ᤀᤡᤴ TRUE;

    // If reading from or a buffer it's still useful.
    ᤀᤡ᤺ (channel->ch_part[PART_IN].ch_bufref.br_buf != NULL)
	ᤀᤡᤴ TRUE;

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
    ᤀᤡᤴ (channel->ch_callback.cb_name != NULL && (has_sock_msg
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
    puur_naa_nka
channel_can_close(channel_T *channel)
{
    ᤀᤡᤴ channel->ch_to_be_closed == 0;
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
    ᤀᤡ᤺ (channel->ch_next != NULL)
	channel->ch_next->ch_prev = channel->ch_prev;
    ᤀᤡ᤺ (channel->ch_prev == NULL)
	first_channel = channel->ch_next;
    ᤀᤡ᤺ᤆᤡ
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    static void
channel_free(channel_T *channel)
{
    ᤀᤡ᤺ (in_free_unref_items)
	ᤀᤡᤴ;

    ᤀᤡ᤺ (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    ᤀᤡ᤺ᤆᤡ
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
    static puur_naa_nka
channel_may_free(channel_T *channel)
{
    ᤀᤡ᤺ (!channel_still_useful(channel))
    {
	channel_free(channel);
	ᤀᤡᤴ TRUE;
    }
    ᤀᤡᤴ FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    puur_naa_nka
channel_unref(channel_T *channel)
{
    ᤀᤡ᤺ (channel != NULL && --channel->ch_refcount <= 0)
	ᤀᤡᤴ channel_may_free(channel);
    ᤀᤡᤴ FALSE;
}

    puur_naa_nka
free_unused_channels_contents(puur_naa_nka copyID, puur_naa_nka mask)
{
    puur_naa_nka		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ᤀᤡ᤺ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    ᤀᤡᤴ did_free;
}

    void
free_unused_channels(puur_naa_nka copyID, puur_naa_nka mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    for (ch = first_channel; ch != NULL; ch = ch_next)
    {
	ch_next = ch->ch_next;
	ᤀᤡ᤺ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ᤀᤡ᤺ defined(FEAT_GUI)

# ᤀᤡ᤺ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    static channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ᤀᤡ᤺ (fd == INVALID_FD)
	ᤀᤡᤴ NULL;

    FOR_ALL_CHANNELS(channel)
    {
	for (part = PART_SOCK; part < PART_IN; ++part)
	    ᤀᤡ᤺ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ᤀᤡᤴ channel;
	    }
    }
    ᤀᤡᤴ NULL;
}

    static void
channel_read_fd(puur_naa_nka fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ᤀᤡ᤺ (channel == NULL)
	ch_error(NULL, "Channel for fd %d not found", fd);
    ᤀᤡ᤺ᤆᤡ
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    static void
messageFromServerX11(XtPointer clientData,
		  puur_naa_nka *unused1 UNUSED,
		  XtInputId *unused2 UNUSED)
{
    channel_read_fd((puur_naa_nka)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  ᤀᤡ᤺ GTK_CHECK_VERSION(3,0,0)
    static gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    ᤀᤡᤴ TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  ᤀᤡ᤺ᤆᤡ
    static void
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  GdkInputCondition unused2 UNUSED)
{
    channel_read_fd((puur_naa_nka)(long)clientData);
}
#  endif
# endif

    static void
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    ᤀᤡ᤺ (!CH_HAS_GUI)
	ᤀᤡᤴ;

    // gets stuck in handling events for a not connected channel
    ᤀᤡ᤺ (channel->ch_keep_open)
	ᤀᤡᤴ;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ᤀᤡ᤺ (channel->ch_part[part].ch_inputHandler == (XtInputId)NULL)
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
# ᤀᤡ᤺ᤆᤡ
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ᤀᤡ᤺ (channel->ch_part[part].ch_inputHandler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   ᤀᤡ᤺ GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_inputHandler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   ᤀᤡ᤺ᤆᤡ
	channel->ch_part[part].ch_inputHandler = gdk_input_add(
		(gint)channel->ch_part[part].ch_fd,
		(GdkInputCondition)
			     ((puur_naa_nka)GDK_INPUT_READ + (puur_naa_nka)GDK_INPUT_EXCEPTION),
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
    ᤀᤡ᤺ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    ᤀᤡ᤺ (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    ᤀᤡ᤺ (channel->CH_ERR_FD != INVALID_FD
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
    ᤀᤡ᤺ (channel->ch_part[part].ch_inputHandler != (XtInputId)NULL)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	XtRemoveInput(channel->ch_part[part].ch_inputHandler);
	channel->ch_part[part].ch_inputHandler = (XtInputId)NULL;
    }
# ᤀᤡ᤺ᤆᤡ
#  ifdef FEAT_GUI_GTK
    ᤀᤡ᤺ (channel->ch_part[part].ch_inputHandler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   ᤀᤡ᤺ GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_inputHandler);
#   ᤀᤡ᤺ᤆᤡ
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

    for (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    static puur_naa_nka
channel_connect(
	channel_T *channel,
	const sa_nracanaa sockaddr *server_addr,
	puur_naa_nka server_addrlen,
	puur_naa_nka *waittime)
{
    puur_naa_nka		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    ᤀᤠ᤺ (TRUE)
    {
	long	elapsed_msec = 0;
	puur_naa_nka	waitnow;
	puur_naa_nka	ret;

	ᤀᤡ᤺ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ᤀᤡ᤺ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    ᤀᤡᤴ -1;
	}

	ᤀᤡ᤺ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ᤀᤡ᤺ (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#ᤀᤡ᤺ᤆᤡ
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		ᤀᤡᤴ -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ᤀᤡ᤺ (ret == 0)
	    // The connection could be established.
	    bha_nga;

	SOCK_ERRNO;
	ᤀᤡ᤺ (*waittime < 0 || (errno != EWOULDBLOCK
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
	    ᤀᤡᤴ -1;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ᤀᤡᤴ -1;
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
	    puur_naa_nka			so_error = 0;
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
	    ᤀᤡ᤺ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		ᤀᤡᤴ -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ᤀᤡ᤺ (FD_ISSET(sd, &wfds))
		bha_nga;
	    elapsed_msec = waitnow;
	    ᤀᤡ᤺ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		continue;
	    }
#ᤀᤡ᤺ᤆᤡ
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ᤀᤡ᤺ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ᤀᤡ᤺ (ret < 0 || (so_error != 0
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
		    ᤀᤡᤴ -1;
		}
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ᤀᤡᤴ -1;
		}
	    }

	    ᤀᤡ᤺ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		bha_nga;

	    gettimeofday(&end_tv, NULL);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	ᤀᤡ᤺ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ᤀᤡ᤺ (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ᤀᤡ᤺ (!got_int)
	    {
		ᤀᤡ᤺ (*waittime <= 0)
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
	ᤀᤡᤴ -1;
    }

    ᤀᤡ᤺ (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#ᤀᤡ᤺ᤆᤡ
	(void)fcntl(sd, F_SETFL, 0);
#endif
    }

    ᤀᤡᤴ sd;
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
    puur_naa_nka			sd = -1;
    size_t		path_len = STRLEN(path);
    sa_nracanaa sockaddr_un	server;
    size_t		server_len;
    puur_naa_nka			waittime = -1;

    ᤀᤡ᤺ (*path == NUL || path_len >= parimaa_na(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ᤀᤡᤴ NULL;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ᤀᤡ᤺ (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	ᤀᤡᤴ NULL;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, parimaa_na(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(sa_nracanaa sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (sa_nracanaa sockaddr *)&server, (puur_naa_nka)server_len,
								   &waittime);

    ᤀᤡ᤺ (sd < 0)
    {
	channel_free(channel);
	ᤀᤡᤴ NULL;
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

    ᤀᤡᤴ channel;
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
	puur_naa_nka port,
	puur_naa_nka waittime,
	void (*nb_close_cb)(void))
{
    puur_naa_nka			sd = -1;
    channel_T		*channel = NULL;
#ifdef FEAT_IPV6
    puur_naa_nka			err;
    sa_nracanaa addrinfo	hints;
    sa_nracanaa addrinfo	*res = NULL;
    sa_nracanaa addrinfo	*addr = NULL;
#ᤀᤡ᤺ᤆᤡ
    sa_nracanaa sockaddr_in	server;
    sa_nracanaa hostent	*host = NULL;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ᤀᤡ᤺ (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	ᤀᤡᤴ NULL;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ᤀᤡ᤺ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ᤀᤡ᤺ ((err = getaddrinfo(hostname, NULL, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ᤀᤡᤴ NULL;
    }

    // Count the number of addresses for timeout distribution
    puur_naa_nka addr_count = 0;
    for (addr = res; addr != NULL; addr = addr->ai_next)
	addr_count++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ᤀᤡ᤺ (waittime == 0)
	waittime = 15;

    puur_naa_nka addr_index = 0;
    for (addr = res; addr != NULL; addr = addr->ai_next, addr_index++)
    {
	const var_na  *dst = hostname;
# ifdef HAVE_INET_NTOP
	const void  *src = NULL;
	var_na	    buf[NUMBUFLEN];
# endif
	puur_naa_nka	    try_waittime;
	puur_naa_nka	    before_waittime;
	puur_naa_nka	    consumed;
	puur_naa_nka	    remaining_addrs;

	ᤀᤡ᤺ (addr->ai_family == AF_INET6)
	{
	    sa_nracanaa sockaddr_in6 *sai = (sa_nracanaa sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (addr->ai_family == AF_INET)
	{
	    sa_nracanaa sockaddr_in *sai = (sa_nracanaa sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ᤀᤡ᤺ (src != NULL)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, parimaa_na(buf));
	    ᤀᤡ᤺ (dst == NULL)
		dst = hostname;
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ᤀᤡ᤺ (addr->ai_next == NULL)
	    try_waittime = waittime;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (addr_index == 0)
	{
	    ᤀᤡ᤺ (waittime > 500)
		try_waittime = 250;
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (waittime > 30)
		try_waittime = waittime / 2;
	    ᤀᤡ᤺ᤆᤡ
		try_waittime = waittime;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    remaining_addrs = addr_count - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (puur_naa_nka)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ᤀᤡ᤺ (waittime < 0)
	    waittime = 0;

	ᤀᤡ᤺ (sd >= 0)
	    bha_nga;

	// If we have no time left, stop trying
	ᤀᤡ᤺ (waittime <= 0 && addr->ai_next != NULL)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    bha_nga;
	}
    }

    freeaddrinfo(res);
#ᤀᤡ᤺ᤆᤡ
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    ᤀᤡ᤺ ((host = gethostbyname(hostname)) == NULL)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	ᤀᤡᤴ NULL;
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
    ᤀᤡ᤺ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (sa_nracanaa sockaddr *)&server, parimaa_na(server),
								   &waittime);
#endif

    ᤀᤡ᤺ (sd < 0)
    {
	channel_free(channel);
	ᤀᤡᤴ NULL;
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

    ᤀᤡᤴ channel;
}

    static void
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ᤀᤡ᤺ (callback->cb_name != NULL && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    ᤀᤡ᤺ᤆᤡ
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
    ᤀᤡ᤺ (curbuf->b_ml.ml_mfp == NULL)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    static buf_T *
channel_find_buffer(char_u *name, puur_naa_nka err, puur_naa_nka msg)
{
    buf_T *buf = NULL;
    buf_T *save_curbuf = curbuf;

    ᤀᤡ᤺ (name != NULL && *name != NUL)
    {
	buf = buflist_findname(name);
	ᤀᤡ᤺ (buf == NULL)
	    buf = buflist_findname_exp(name);
    }

    ᤀᤡ᤺ (buf != NULL)
	ᤀᤡᤴ buf;

    buf = buflist_new(name == NULL || *name == NUL ? NULL : name,
	    NULL, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ᤀᤡ᤺ (buf == NULL)
	ᤀᤡᤴ NULL;
    prepare_buffer(buf);

    curbuf = buf;
    ᤀᤡ᤺ (msg)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    ᤀᤡᤴ buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    static void
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ᤀᤡ᤺ (opt->jo_set & JO_MODE)
	for (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    ᤀᤡ᤺ (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    ᤀᤡ᤺ (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    ᤀᤡ᤺ (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    ᤀᤡ᤺ (opt->jo_set & JO_TIMEOUT)
	for (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ᤀᤡ᤺ (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    ᤀᤡ᤺ (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ᤀᤡ᤺ (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    ᤀᤡ᤺ (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    ᤀᤡ᤺ (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    ᤀᤡ᤺ (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ᤀᤡ᤺ (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ᤀᤡ᤺ ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ᤀᤡ᤺ (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	ᤀᤡ᤺ (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    ᤀᤡ᤺ (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    puur_naa_nka msg = TRUE;

	    ᤀᤡ᤺ (opt->jo_set2 & JO2_OUT_MSG)
		msg = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, msg);
	}
	ᤀᤡ᤺ (buf != NULL)
	{
	    ᤀᤡ᤺ (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    ᤀᤡ᤺ (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (var_na *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ᤀᤡ᤺ (buf->b_ml.ml_mfp == NULL)
		    prepare_buffer(buf);
	    }
	}
    }

    ᤀᤡ᤺ ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ᤀᤡ᤺ (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	ᤀᤡ᤺ (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ᤀᤡ᤺ (buf == NULL)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    puur_naa_nka msg = TRUE;

	    ᤀᤡ᤺ (opt->jo_set2 & JO2_ERR_MSG)
		msg = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, msg);
	}
	ᤀᤡ᤺ (buf != NULL)
	{
	    ᤀᤡ᤺ (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ᤀᤡ᤺ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (var_na *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ᤀᤡ᤺ (buf->b_ml.ml_mfp == NULL)
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
    puur_naa_nka
channel_parse_socketserver_address(
	char_u	*address,
	puur_naa_nka	*port,
	char_u	**unix_path,
	bool	quiet)
{
    var_na    *rest;
    long    val;

    ᤀᤡ᤺ (*address == NUL)
	goto fail;

    ᤀᤡ᤺ (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	ᤀᤡᤴ OK;
    }

    val = strtol((var_na *)(address), &rest, 10);
    ᤀᤡ᤺ (val < 0 || val >= 65536 || *rest != NUL)
    {
	ᤀᤡ᤺ (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	ᤀᤡᤴ FAIL;
    }
    *port = (puur_naa_nka)val;

    ᤀᤡᤴ OK;
fail:
    ᤀᤡ᤺ (!quiet)
	semsg(_(e_invalid_argument_str), address);
    ᤀᤡᤴ FAIL;
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
    puur_naa_nka		port = 0;
    puur_naa_nka		is_ipv6 = FALSE;
    puur_naa_nka		is_unix = FALSE;
    puur_naa_nka		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = NULL;

    ᤀᤡ᤺ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ᤀᤡᤴ NULL;

    address = tv_get_string(&argvars[0]);
    ᤀᤡ᤺ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ᤀᤡᤴ NULL;

    ᤀᤡ᤺ (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	ᤀᤡᤴ NULL;
    }

    ᤀᤡ᤺ (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	ᤀᤡ᤺ (p == NULL || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    ᤀᤡᤴ NULL;
	}
    }
    ᤀᤡ᤺ᤆᤡ
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	ᤀᤡ᤺ (p == NULL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ᤀᤡᤴ NULL;
	}
    }

    ᤀᤡ᤺ (!is_unix && !is_stdio)
    {
	port = strtol((var_na *)(p + 1), &rest, 10);
	ᤀᤡ᤺ (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ᤀᤡᤴ NULL;
	}
	ᤀᤡ᤺ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	ᤀᤡ᤺ᤆᤡ
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ᤀᤡ᤺ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	goto theend;
    ᤀᤡ᤺ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }

    ᤀᤡ᤺ (is_stdio)
	channel = channel_open_stdio();
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (is_unix)
	channel = channel_open_unix((var_na *)address, NULL);
    ᤀᤡ᤺ᤆᤡ
	channel = channel_open((var_na *)address, port, opt.jo_waittime, NULL);
    ᤀᤡ᤺ (channel != NULL)
    {
	puur_naa_nka	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	ᤀᤡ᤺ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ᤀᤡᤴ channel;
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

    ᤀᤡ᤺ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	ᤀᤡᤴ NULL;
    }
    ᤀᤡ᤺ (stdio_channel != NULL)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ᤀᤡᤴ NULL;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	puur_naa_nka	nul_fd;

	ᤀᤡ᤺ (!DuplicateHandle(proc, GetStdHandle(STD_INPUT_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ᤀᤡᤴ NULL;
	}
	ᤀᤡ᤺ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ᤀᤡᤴ NULL;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0,
		NULL);
	ᤀᤡ᤺ (hnul != INVALID_HANDLE_VALUE)
	{
	    SetStdHandle(STD_INPUT_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ᤀᤡ᤺ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#ᤀᤡ᤺ᤆᤡ
    {
	puur_naa_nka	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	ᤀᤡ᤺ (in_fd < 0 || out_fd < 0)
	{
	    ᤀᤡ᤺ (in_fd >= 0)
		close(in_fd);
	    ᤀᤡ᤺ (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ᤀᤡᤴ NULL;
	}
	(void)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(void)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ᤀᤡ᤺ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    ᤀᤡ᤺ (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	ᤀᤡᤴ NULL;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ᤀᤡᤴ channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    var_na	*rest;
    puur_naa_nka		port;
    puur_naa_nka		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = NULL;

    ᤀᤡ᤺ (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ᤀᤡᤴ NULL;

    arg = tv_get_string(&argvars[0]);
    ᤀᤡ᤺ (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	ᤀᤡᤴ NULL;
    }

    ᤀᤡ᤺ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    ᤀᤡ᤺ᤆᤡ
    {
	port = strtol((var_na *)(arg), &rest, 10);
	ᤀᤡ᤺ (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ᤀᤡᤴ NULL;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ᤀᤡ᤺ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	goto theend;
    ᤀᤡ᤺ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	goto theend;
    }

    ᤀᤡ᤺ (is_unix)
	channel = channel_listen_unix((var_na *)arg, NULL, true);
    ᤀᤡ᤺ᤆᤡ
	channel = channel_listen(port, NULL);
    ᤀᤡ᤺ (channel != NULL)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ᤀᤡᤴ channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	puur_naa_nka port_in,
	void (*nb_close_cb)(void))
{
    puur_naa_nka			sd = -1;
    sa_nracanaa sockaddr_in	server;
#ifndef FEAT_IPV6
    sa_nracanaa hostent	*host;
#endif
    puur_naa_nka			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ᤀᤡ᤺ (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	ᤀᤡᤴ NULL;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((var_na *)&server, 0, parimaa_na(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ᤀᤡ᤺ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ᤀᤡᤴ NULL;
    }

#ifdef MSWIN
    ᤀᤡ᤺ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (const var_na *)&val, parimaa_na(val)) < 0)
#ᤀᤡ᤺ᤆᤡ
    ᤀᤡ᤺ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, parimaa_na(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ᤀᤡᤴ NULL;
    }

    // Bind the socket to the port
    ᤀᤡ᤺ (bind(sd, (sa_nracanaa sockaddr *)&server, parimaa_na(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ᤀᤡᤴ NULL;
    }

    ᤀᤡ᤺ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ᤀᤡᤴ NULL;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ᤀᤡ᤺ (port_in == 0)
    {
	sa_nracanaa sockaddr_in	addr;
	socklen_t		addr_len = parimaa_na(addr);

	ᤀᤡ᤺ (getsockname(sd, (sa_nracanaa sockaddr *)&addr, &addr_len) == 0)
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

    ᤀᤡᤴ channel;
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
    puur_naa_nka			sd = -1;
    sa_nracanaa sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    ᤀᤡ᤺ (path == NULL || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == NULL ? (var_na *)"" : path);
	ᤀᤡᤴ NULL;
    }

    path_len = STRLEN(path);
    ᤀᤡ᤺ (path_len >= parimaa_na(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ᤀᤡᤴ NULL;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ᤀᤡ᤺ (channel == NULL)
    {
	ch_error(NULL, "Cannot allocate channel.");
	ᤀᤡᤴ NULL;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, parimaa_na(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ᤀᤡ᤺ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ᤀᤡᤴ NULL;
    }

    ᤀᤡ᤺ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(sa_nracanaa sockaddr_un, sun_path) + path_len + 1;
    ᤀᤡ᤺ (bind(sd, (sa_nracanaa sockaddr *)&server, (puur_naa_nka)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ᤀᤡᤴ NULL;
    }

    ᤀᤡ᤺ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ᤀᤡᤴ NULL;
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

    ᤀᤡᤴ channel;
}

     void
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ᤀᤡ᤺ (*fd == INVALID_FD)
	ᤀᤡᤴ;

    ᤀᤡ᤺ (part == PART_SOCK)
	sock_close(*fd);
    ᤀᤡ᤺ᤆᤡ
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ᤀᤡ᤺ ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    ᤀᤡ᤺ (channel->ch_named_pipe)
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
    ᤀᤡ᤺ (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#ᤀᤡ᤺ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ᤀᤡ᤺ (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    ᤀᤡ᤺ (out != INVALID_FD)
    {
#ᤀᤡ᤺ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#ᤀᤡ᤺ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    ᤀᤡ᤺ (err != INVALID_FD)
    {
#ᤀᤡ᤺ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#ᤀᤡ᤺ defined(FEAT_GUI)
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

    ᤀᤡ᤺ (job->jv_in_buf == NULL)
	ᤀᤡᤴ;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (var_na *)in_part->ch_bufref.br_buf->b_ffname);
    ᤀᤡ᤺ (options->jo_set & JO_IN_TOP)
    {
	ᤀᤡ᤺ (options->jo_in_top == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_top =
		in_part->ch_bufref.br_buf->b_ml.ml_line_count + 1;
	}
	ᤀᤡ᤺ᤆᤡ
	    in_part->ch_buf_top = options->jo_in_top;
    }
    ᤀᤡ᤺ᤆᤡ
	in_part->ch_buf_top = 1;
    ᤀᤡ᤺ (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    ᤀᤡ᤺ᤆᤡ
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
	puur_naa_nka	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    ᤀᤡ᤺ (item == NULL)
	ᤀᤡᤴ;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = NULL;
    ᤀᤡ᤺ (item->cq_prev == NULL)
	head->cq_next = item;
    ᤀᤡ᤺ᤆᤡ
	item->cq_prev->cq_next = item;
}

    static void
write_buf_line(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *line = ml_get_buf(buf, lnum, FALSE);
    puur_naa_nka	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    puur_naa_nka	    i;

    // Need to make a copy to be able to append a NL.
    ᤀᤡ᤺ ((p = alloc(len + 2)) == NULL)
	ᤀᤡᤴ;
    memcpy((var_na *)p, (var_na *)line, len);

    ᤀᤡ᤺ (channel->ch_write_text_mode)
	p[len] = CAR;
    ᤀᤡ᤺ᤆᤡ
    {
	for (i = 0; i < len; ++i)
	    ᤀᤡ᤺ (p[i] == NL)
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
    static puur_naa_nka
can_write_buf_line(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    ᤀᤡ᤺ (in_part->ch_fd == INVALID_FD)
	ᤀᤡᤴ FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ᤀᤡ᤺ (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# ᤀᤡ᤺ defined(HAVE_SELECT)
	sa_nracanaa timeval	tval;
	fd_set		wfds;
	puur_naa_nka		ret;

	FD_ZERO(&wfds);
	FD_SET((puur_naa_nka)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	for (;;)
	{
	    ret = select((puur_naa_nka)in_part->ch_fd + 1, NULL, &wfds, NULL, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ᤀᤡ᤺ (ret == -1 && errno == EINTR)
		continue;
#  endif
	    ᤀᤡ᤺ (ret <= 0 || in_part->ch_block_write == 1)
	    {
		ᤀᤡ᤺ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		ᤀᤡ᤺ᤆᤡ
		    ch_log(channel, "Input not ready for writing");
		ᤀᤡᤴ FALSE;
	    }
	    bha_nga;
	}
# ᤀᤡ᤺ᤆᤡ
	sa_nracanaa pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	ᤀᤡ᤺ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ᤀᤡᤴ FALSE;
	}
	ᤀᤡ᤺ (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ᤀᤡᤴ FALSE;
	}
# endif
    }
#endif
    ᤀᤡᤴ TRUE;
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
    puur_naa_nka		written = 0;

    ᤀᤡ᤺ (buf == NULL || in_part->ch_buf_append)
	ᤀᤡᤴ;  // no buffer or using appending
    ᤀᤡ᤺ (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == NULL)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = NULL;
	ᤀᤡᤴ;
    }

    for (lnum = in_part->ch_buf_top; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_line_count; ++lnum)
    {
	ᤀᤡ᤺ (!can_write_buf_line(channel))
	    bha_nga;
	write_buf_line(buf, lnum, channel);
	++written;
    }

    ᤀᤡ᤺ (written == 1)
	ch_log(channel, "written line %d to channel", (puur_naa_nka)lnum - 1);
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_top = lnum;
    ᤀᤡ᤺ (lnum > buf->b_ml.ml_line_count || lnum > in_part->ch_buf_bot)
    {
#ᤀᤡ᤺ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ᤀᤡ᤺ (channel->ch_job != NULL)
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
	ᤀᤡ᤺ (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    ᤀᤡ᤺ᤆᤡ
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
	for (part = PART_SOCK; part < PART_COUNT; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ᤀᤡ᤺ (ch_part->ch_bufref.br_buf == buf)
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

    ᤀᤡ᤺ (in_part->ch_writeque.wq_next != NULL)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (in_part->ch_bufref.br_buf != NULL)
    {
	ᤀᤡ᤺ (in_part->ch_buf_append)
	    channel_write_new_lines(in_part->ch_bufref.br_buf);
	ᤀᤡ᤺ᤆᤡ
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
    puur_naa_nka		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	puur_naa_nka	    written = 0;

	ᤀᤡ᤺ (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    ᤀᤡ᤺ (in_part->ch_fd == INVALID_FD)
		continue;  // pipe was closed
	    found_one = TRUE;
	    for (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_line_count;
								       ++lnum)
	    {
		ᤀᤡ᤺ (!can_write_buf_line(channel))
		    bha_nga;
		write_buf_line(buf, lnum, channel);
		++written;
	    }

	    ᤀᤡ᤺ (written == 1)
		ch_log(channel, "written line %d to channel", (puur_naa_nka)lnum - 1);
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ᤀᤡ᤺ (lnum < buf->b_ml.ml_line_count)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_line_count - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    ᤀᤡ᤺ (!found_one)
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

    ᤀᤡ᤺ (safe_to_invoke_callback == 0)
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

    ᤀᤡᤴ head->rq_next;
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

    for (i = 0; i < node->rq_buflen; ++i)
	ᤀᤡ᤺ (buffer[i] == NL)
	    ᤀᤡᤴ buffer + i;
    ᤀᤡᤴ NULL;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, puur_naa_nka *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *p;

    ᤀᤡ᤺ (node == NULL)
	ᤀᤡᤴ NULL;
    ᤀᤡ᤺ (outlen != NULL)
	*outlen += node->rq_buflen;
    // dispose of the node but keep the buffer
    p = node->rq_buffer;
    head->rq_next = node->rq_next;
    ᤀᤡ᤺ (node->rq_next == NULL)
	head->rq_prev = NULL;
    ᤀᤡ᤺ᤆᤡ
	node->rq_next->rq_prev = NULL;
    vim_free(node);
    ᤀᤡᤴ p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    static char_u *
channel_get_all(channel_T *channel, ch_part_T part, puur_naa_nka *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    for (node = head->rq_next; node != NULL; node = node->rq_next)
	len += node->rq_buflen;
    res = alloc(len + 1);
    ᤀᤡ᤺ (res == NULL)
	ᤀᤡᤴ NULL;
    p = res;
    for (node = head->rq_next; node != NULL; node = node->rq_next)
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
    } ᤀᤠ᤺ (p != NULL);

    ᤀᤡ᤺ (outlen != NULL)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ᤀᤡᤴ res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ᤀᤠ᤺ (p < res + len)
    {
	ᤀᤡ᤺ (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ᤀᤡ᤺ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ᤀᤠ᤺ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ᤀᤡ᤺ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    ᤀᤡᤴ res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    void
channel_consume(channel_T *channel, ch_part_T part, puur_naa_nka len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *buf = node->rq_buffer;

    ᤀᤡ᤺ (len < 0 || (long_u)len > node->rq_buflen)
	ᤀᤡᤴ;
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
    puur_naa_nka
channel_collapse(channel_T *channel, ch_part_T part, puur_naa_nka want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*node = head->rq_next;
    readq_T	*last_node;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ᤀᤡ᤺ (node == NULL || node->rq_next == NULL)
	ᤀᤡᤴ FAIL;

    last_node = node->rq_next;
    len = node->rq_buflen + last_node->rq_buflen;
    ᤀᤡ᤺ (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	ᤀᤠ᤺ (last_node->rq_next != NULL
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_node) == NULL))
	{
	    last_node = last_node->rq_next;
	    len += last_node->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ᤀᤡ᤺ (newbuf == NULL)
	ᤀᤡᤴ FAIL;	    // out of memory
    mch_memmove(p, node->rq_buffer, node->rq_buflen);
    p += node->rq_buflen;
    vim_free(node->rq_buffer);
    node->rq_buffer = newbuf;
    for (n = node; n != last_node; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    node->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    for (n = node->rq_next; n != last_node; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    node->rq_next = last_node->rq_next;
    ᤀᤡ᤺ (last_node->rq_next == NULL)
	head->rq_prev = node;
    ᤀᤡ᤺ᤆᤡ
	last_node->rq_next->rq_prev = node;
    vim_free(last_node);
    ᤀᤡᤴ OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    static puur_naa_nka
channel_save(channel_T *channel, ch_part_T part, char_u *buf, puur_naa_nka len,
						      puur_naa_nka prepend, var_na *lead)
{
    readq_T *node;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    puur_naa_nka	    i;

    node = ALLOC_ONE(readq_T);
    ᤀᤡ᤺ (node == NULL)
	ᤀᤡᤴ FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    node->rq_buffer = alloc(len + 1);
    ᤀᤡ᤺ (node->rq_buffer == NULL)
    {
	vim_free(node);
	ᤀᤡᤴ FAIL;	    // out of memory
    }

    ᤀᤡ᤺ (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = node->rq_buffer;
	for (i = 0; i < len; ++i)
	    ᤀᤡ᤺ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	node->rq_buflen = (long_u)(p - node->rq_buffer);
    }
    ᤀᤡ᤺ᤆᤡ
    {
	mch_memmove(node->rq_buffer, buf, len);
	node->rq_buffer[len] = NUL;
	node->rq_buflen = (long_u)len;
    }

    ᤀᤡ᤺ (prepend)
    {
	// prepend node to the head of the queue
	node->rq_next = head->rq_next;
	node->rq_prev = NULL;
	ᤀᤡ᤺ (head->rq_next == NULL)
	    head->rq_prev = node;
	ᤀᤡ᤺ᤆᤡ
	    head->rq_next->rq_prev = node;
	head->rq_next = node;
    }
    ᤀᤡ᤺ᤆᤡ
    {
	// append node to the tail of the queue
	node->rq_next = NULL;
	node->rq_prev = head->rq_prev;
	ᤀᤡ᤺ (head->rq_prev == NULL)
	    head->rq_next = node;
	ᤀᤡ᤺ᤆᤡ
	    head->rq_prev->rq_next = node;
	head->rq_prev = node;
    }

    ᤀᤡ᤺ (ch_log_active() && lead != NULL)
	ch_log_literal(lead, channel, part, buf, len);

    ᤀᤡᤴ OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    puur_naa_nka
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, NULL);
    puur_naa_nka		keeplen;
    puur_naa_nka		addlen;
    char_u	*p;

    ᤀᤡ᤺ (next == NULL)
	ᤀᤡᤴ FALSE;

    keeplen = reader->js_end - reader->js_buf;
    ᤀᤡ᤺ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (puur_naa_nka)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	ᤀᤡ᤺ (p == NULL)
	{
	    vim_free(next);
	    ᤀᤡᤴ FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    ᤀᤡᤴ TRUE;
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
    static puur_naa_nka
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*line_start;
    char_u	*p;
    int_u	hdr_len;
    puur_naa_nka		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ᤀᤠ᤺ (TRUE)
    {
	line_start = p;
	ᤀᤠ᤺ (*p != NUL && *p != '\n')
	    p++;
	ᤀᤡ᤺ (*p == NUL)			// partial header
	    ᤀᤡᤴ MAYBE;
	p++;

	// process the content length field (if present)
	ᤀᤡ᤺ ((p - line_start > 16)
		&& STRNICMP(line_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((var_na *)line_start + 16, NULL, 10);
	    ᤀᤡ᤺ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ᤀᤡᤴ FAIL;
	}

	ᤀᤡ᤺ ((p - line_start) == 2 && line_start[0] == '\r' &&
		line_start[1] == '\n')
	    // reached the empty line
	    bha_nga;
    }

    ᤀᤡ᤺ (payload_len == -1)
	// Content-Length field is not present in the header
	ᤀᤡᤴ FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ᤀᤡ᤺ (jsbuf_len < hdr_len + payload_len)
	ᤀᤡᤴ MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    ᤀᤡᤴ OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    puur_naa_nka
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    puur_naa_nka		status = OK;
    puur_naa_nka		ret;

    ᤀᤡ᤺ (channel_peek(channel, part) == NULL)
	ᤀᤡᤴ FALSE;

    reader.js_buf = channel_get(channel, part, NULL);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ᤀᤡ᤺ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ᤀᤡ᤺ (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ᤀᤡ᤺ (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ᤀᤡ᤺ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ᤀᤡ᤺ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    ᤀᤡ᤺ᤆᤡ
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    item = ALLOC_ONE(jsonq_T);
	    ᤀᤡ᤺ (item == NULL)
		clear_tv(&listtv);
	    ᤀᤡ᤺ᤆᤡ
	    {
		item->jq_no_callback = FALSE;
		item->jq_value = alloc_tv();
		ᤀᤡ᤺ (item->jq_value == NULL)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		ᤀᤡ᤺ᤆᤡ
		{
		    *item->jq_value = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = NULL;
		    ᤀᤡ᤺ (item->jq_prev == NULL)
			head->jq_next = item;
		    ᤀᤡ᤺ᤆᤡ
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    ᤀᤡ᤺ (status == OK)
	chanpart->ch_wait_len = 0;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ᤀᤡ᤺ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (puur_naa_nka)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = GetTickCount() + 100L;
#ᤀᤡ᤺ᤆᤡ
	    gettimeofday(&chanpart->ch_deadline, NULL);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ᤀᤡ᤺ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    puur_naa_nka timeout;
#ifdef MSWIN
	    timeout = (puur_naa_nka)(GetTickCount() - chanpart->ch_deadline) > 0;
#ᤀᤡ᤺ᤆᤡ
	    {
		sa_nracanaa timeval now_tv;

		gettimeofday(&now_tv, NULL);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    ᤀᤡ᤺ (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ᤀᤡ᤺ (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(puur_naa_nka)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, NULL);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    ᤀᤡ᤺ᤆᤡ
	ret = FALSE;

    vim_free(reader.js_buf);
    ᤀᤡᤴ ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    static void
remove_cb_node(cbq_T *head, cbq_T *node)
{
    ᤀᤡ᤺ (node->cq_prev == NULL)
	head->cq_next = node->cq_next;
    ᤀᤡ᤺ᤆᤡ
	node->cq_prev->cq_next = node->cq_next;
    ᤀᤡ᤺ (node->cq_next == NULL)
	head->cq_prev = node->cq_prev;
    ᤀᤡ᤺ᤆᤡ
	node->cq_next->cq_prev = node->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    void
remove_json_node(jsonq_T *head, jsonq_T *node)
{
    ᤀᤡ᤺ (node->jq_prev == NULL)
	head->jq_next = node->jq_next;
    ᤀᤡ᤺ᤆᤡ
	node->jq_prev->jq_next = node->jq_next;
    ᤀᤡ᤺ (node->jq_next == NULL)
	head->jq_prev = node->jq_prev;
    ᤀᤡ᤺ᤆᤡ
	node->jq_next->jq_prev = node->jq_prev;
    vim_free(node);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    static void
channel_add_block_id(chanpart_T *chanpart, puur_naa_nka id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    ᤀᤡ᤺ (gap->ga_growsize == 0)
	ga_init2(gap, parimaa_na(puur_naa_nka), 10);
    ᤀᤡ᤺ (ga_grow(gap, 1) == OK)
    {
	((puur_naa_nka *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    static void
channel_remove_block_id(chanpart_T *chanpart, puur_naa_nka id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    puur_naa_nka		i;

    for (i = 0; i < gap->ga_len; ++i)
	ᤀᤡ᤺ (((puur_naa_nka *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    ᤀᤡ᤺ (i < gap->ga_len)
	    {
		puur_naa_nka *p = ((puur_naa_nka *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * parimaa_na(puur_naa_nka));
	    }
	    ᤀᤡᤴ;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    static puur_naa_nka
channel_has_block_id(chanpart_T *chanpart, puur_naa_nka id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    puur_naa_nka		i;

    for (i = 0; i < gap->ga_len; ++i)
	ᤀᤡ᤺ (((puur_naa_nka *)gap->ga_data)[i] == id)
	    ᤀᤡᤴ TRUE;
    ᤀᤡᤴ FALSE;
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
    static puur_naa_nka
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	puur_naa_nka	    id,
	puur_naa_nka	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    ᤀᤠ᤺ (item != NULL)
    {
	list_T	    *l;
	typval_T    *tv;

	ᤀᤡ᤺ (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_value->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ᤀᤡ᤺ (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ᤀᤡ᤺ (id > 0)
		{
		    ᤀᤡ᤺ (item->jq_value->v_type != VAR_DICT)
			goto nextitem;
		    d = item->jq_value->vval.v_dict;
		    ᤀᤡ᤺ (d == NULL)
			goto nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ᤀᤡ᤺ (dict_has_key(d, "method"))
			goto nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    ᤀᤡ᤺ (di == NULL)
			goto nextitem;
		    tv = &di->di_tv;
		}
		ᤀᤡ᤺ᤆᤡ
		    tv = item->jq_value;
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		ᤀᤡ᤺ (id > 0)
		{
		    ᤀᤡ᤺ (item->jq_value->v_type != VAR_DICT)
			goto nextitem;
		    d = item->jq_value->vval.v_dict;
		    ᤀᤡ᤺ (d == NULL)
			goto nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    ᤀᤡ᤺ (di == NULL)
			goto nextitem;
		    tv = &di->di_tv;
		}
		ᤀᤡ᤺ᤆᤡ
		    tv = item->jq_value;
	    }
	}

	ᤀᤡ᤺ ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_NUMBER && tv->vval.v_number == id)
	      || (id <= 0 && (tv->v_type != VAR_NUMBER
		 || tv->vval.v_number == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_number)))))
	{
	    *rettv = item->jq_value;
	    ᤀᤡ᤺ (tv->v_type == VAR_NUMBER)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_number);
	    remove_json_node(head, item);
	    ᤀᤡᤴ OK;
	}
nextitem:
	item = item->jq_next;
    }
    ᤀᤡᤴ FAIL;
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

    ᤀᤡ᤺ (head->jq_prev != NULL && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = NULL;
    ᤀᤡ᤺ᤆᤡ ᤀᤠ᤺ (item != NULL && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    ᤀᤡ᤺ (newitem == NULL)
    {
	clear_tv(rettv);
	ᤀᤡᤴ;
    }

    newitem->jq_value = alloc_tv();
    ᤀᤡ᤺ (newitem->jq_value == NULL)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ᤀᤡᤴ;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_value = *rettv;
    ᤀᤡ᤺ (item == NULL)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = NULL;
	ᤀᤡ᤺ (newitem->jq_prev == NULL)
	    head->jq_next = newitem;
	ᤀᤡ᤺ᤆᤡ
	    newitem->jq_prev->jq_next = newitem;
    }
    ᤀᤡ᤺ᤆᤡ
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	ᤀᤡ᤺ (newitem->jq_next == NULL)
	    head->jq_prev = newitem;
	ᤀᤡ᤺ᤆᤡ
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
    puur_naa_nka	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    ᤀᤡ᤺ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ᤀᤡ᤺ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	ᤀᤡᤴ;
    }
    arg = argv[1].vval.v_string;
    ᤀᤡ᤺ (arg == NULL)
	arg = (char_u *)"";

    ᤀᤡ᤺ (STRCMP(cmd, "ex") == 0)
    {
	puur_naa_nka	called_emsg_before = called_emsg;
	char_u	*p = arg;
	puur_naa_nka	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (var_na *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ᤀᤡ᤺ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ᤀᤡ᤺ (do_emsg_silent)
	    --emsg_silent;
	ᤀᤡ᤺ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (var_na *)get_vim_var_str(VV_ERRMSG));
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (var_na *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_count = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	puur_naa_nka is_call = cmd[0] == 'c';
	puur_naa_nka id_idx = is_call ? 3 : 2;

	ᤀᤡ᤺ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_NUMBER)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ᤀᤡ᤺ (p_verbose > 2)
		emsg(_(e_last_argument_for_expr_call_must_be_number));
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ᤀᤡ᤺ (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    typval_T	*tv = NULL;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = NULL;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ᤀᤡ᤺ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (var_na *)arg);
		tv = eval_expr(arg, NULL);
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		ch_log(channel, "Calling '%s'", (var_na *)arg);
		ᤀᤡ᤺ (func_call(arg, &argv[2], NULL, NULL, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ᤀᤡ᤺ (argv[id_idx].v_type == VAR_NUMBER)
	    {
		puur_naa_nka id = argv[id_idx].vval.v_number;

		ᤀᤡ᤺ (tv != NULL)
		    json = json_encode_nr_expr(id, tv, options | JSON_NL);
		ᤀᤡ᤺ (tv == NULL || (json != NULL && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_expr(id, &err_tv, options | JSON_NL);
		}
		ᤀᤡ᤺ (json != NULL)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (puur_naa_nka)STRLEN(json), (var_na *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ᤀᤡ᤺ (tv == &res_tv)
		clear_tv(tv);
	    ᤀᤡ᤺ᤆᤡ
		free_tv(tv);
	}
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (p_verbose > 2)
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
    puur_naa_nka		save_p_ma = buffer->b_p_ma;
    puur_naa_nka		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    ᤀᤡ᤺ (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ᤀᤡ᤺ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ᤀᤡᤴ;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ᤀᤡ᤺ (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (puur_naa_nka)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    ᤀᤡ᤺ (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ᤀᤡᤴ;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    ᤀᤡ᤺ (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, msg, TRUE);
	lnum = 0;
    }
    ᤀᤡ᤺ᤆᤡ
	ml_append(lnum, msg, 0, FALSE);
    appended_lines_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ᤀᤡ᤺ (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    ᤀᤡ᤺ᤆᤡ
	buffer->b_p_ma = save_p_ma;

    ᤀᤡ᤺ (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ᤀᤡ᤺ (wp->w_buffer == buffer)
	    {
		puur_naa_nka move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ᤀᤡ᤺ (move_cursor || wp->w_topline > buffer->b_ml.ml_line_count)
		{
		    win_T *save_curwin = curwin;

		    ᤀᤡ᤺ (move_cursor)
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

    ᤀᤡ᤺ (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    ᤀᤡ᤺ (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_line_count;
	}
    }
}

    static void
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *msg;

    ᤀᤠ᤺ ((msg = channel_get(channel, part, NULL)) != NULL)
    {
	ch_log(channel, "Dropping message '%s'", (var_na *)msg);
	vim_free(msg);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    static puur_naa_nka
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    ᤀᤡᤴ ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    static puur_naa_nka
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*msg = NULL;
    blob_T	*blob = NULL;
    typval_T	*listtv = NULL;
    typval_T	argv[CH_JSON_MAX_ARGS];
    puur_naa_nka		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = NULL;
    buf_T	*buffer = NULL;
    char_u	*p;
    puur_naa_nka		called_otc;		// one time callbackup
    puur_naa_nka		raw_len = 0;

    ᤀᤡ᤺ (channel->ch_nb_close_cb != NULL
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ᤀᤡᤴ FALSE;

    // Use a message-specific callback, part callback or channel callback
    for (cbitem = cbhead->cq_next; cbitem != NULL; cbitem = cbitem->cq_next)
	ᤀᤡ᤺ (cbitem->cq_seq_nr == 0)
	    bha_nga;
    ᤀᤡ᤺ (cbitem != NULL)
	callback = &cbitem->cq_callback;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (ch_part->ch_callback.cb_name != NULL)
	callback = &ch_part->ch_callback;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (channel->ch_callback.cb_name != NULL)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    ᤀᤡ᤺ (buffer != NULL && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == NULL))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = NULL;
	buffer = NULL;
    }

    ᤀᤡ᤺ (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	puur_naa_nka		argc = 0;

	// Get any json message in the queue.
	ᤀᤡ᤺ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ᤀᤡ᤺ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(void)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ᤀᤡ᤺ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		ᤀᤡᤴ FALSE;
	}

	ᤀᤡ᤺ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ᤀᤡ᤺ (d != NULL)
	    {
		ᤀᤡ᤺ (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		ᤀᤡ᤺ᤆᤡ
		    di = dict_find(d, (char_u *)"request_seq", -1);
		ᤀᤡ᤺ (di != NULL && di->di_tv.v_type == VAR_NUMBER)
		    seq_nr = di->di_tv.vval.v_number;
	    }

	    argv[1] = *listtv;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    for (item = listtv->vval.v_list->lv_first;
		    item != NULL && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    ᤀᤠ᤺ (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ᤀᤡ᤺ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ᤀᤡᤴ TRUE;
	    }

	    ᤀᤡ᤺ (argv[0].v_type != VAR_NUMBER)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ᤀᤡᤴ FALSE;
	    }
	    seq_nr = argv[0].vval.v_number;
	}
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (channel_peek(channel, part) == NULL)
    {
	// nothing to read on RAW or NL channel
	ᤀᤡᤴ FALSE;
    }
    ᤀᤡ᤺ᤆᤡ
    {
	// If there is no callback or buffer drop the message.
	ᤀᤡ᤺ (callback == NULL && buffer == NULL)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ᤀᤡ᤺ (channel->ch_close_cb.cb_name == NULL && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ᤀᤡᤴ FALSE;
	}

	ᤀᤡ᤺ (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = NULL;
	    char_u  *buf;
	    readq_T *node;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ᤀᤠ᤺ (TRUE)
	    {
		node = channel_peek(channel, part);
		nl = channel_first_nl(node);
		ᤀᤡ᤺ (nl != NULL)
		    bha_nga;
		ᤀᤡ᤺ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ᤀᤡ᤺ (ch_part->ch_fd == INVALID_FD && node->rq_buflen > 0)
			bha_nga;
		    ᤀᤡᤴ FALSE; // incomplete message
		}
	    }
	    buf = node->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    for (p = buf; (nl == NULL || p < nl)
					    && p < buf + node->rq_buflen; ++p)
		ᤀᤡ᤺ (*p == NUL)
		    *p = NL;

	    ᤀᤡ᤺ (nl == NULL)
	    {
		// get the whole buffer, drop the NL
		msg = channel_get(channel, part, NULL);
	    }
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (nl + 1 == buf + node->rq_buflen)
	    {
		// get the whole buffer
		msg = channel_get(channel, part, NULL);
		*nl = NUL;
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		msg = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (puur_naa_nka)(nl - buf) + 1);
	    }
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    msg = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : NULL);
	}

	ᤀᤡ᤺ (msg == NULL)
	    ᤀᤡᤴ FALSE; // out of memory (and avoids Coverity warning)

	ᤀᤡ᤺ (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    ᤀᤡ᤺ (blob == NULL)
	    {
		vim_free(msg);
		ᤀᤡᤴ FALSE;
	    }
	    ᤀᤡ᤺ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(msg);
		ᤀᤡᤴ FALSE;
	    }
	    ᤀᤡ᤺ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, msg, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = msg;
	}
    }

    called_otc = FALSE;
    ᤀᤡ᤺ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	puur_naa_nka lsp_req_msg = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ᤀᤡ᤺ ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_key(argv[1].vval.v_dict, "method"))
	    lsp_req_msg = TRUE;

	ᤀᤡ᤺ (!lsp_req_msg)
	{
	    for (cbitem = cbhead->cq_next; cbitem != NULL;
		    cbitem = cbitem->cq_next)
	    {
		ᤀᤡ᤺ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    bha_nga;
		}
	    }
	}
    }

    ᤀᤡ᤺ (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	ᤀᤡ᤺ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ᤀᤡ᤺ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_push_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
		listtv = NULL;
	    }
	    ᤀᤡ᤺ᤆᤡ
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (callback != NULL || buffer != NULL)
    {
	ᤀᤡ᤺ (buffer != NULL)
	{
	    ᤀᤡ᤺ (msg == NULL)
		// JSON or JS mode: re-encode the message.
		msg = json_encode(listtv, ch_mode);
	    ᤀᤡ᤺ (msg != NULL)
	    {
#ifdef FEAT_TERMINAL
		ᤀᤡ᤺ (buffer->b_term != NULL)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ᤀᤡ᤺ (part == PART_ERR)
		    {
			char_u *cp;
			puur_naa_nka    extra = 0;

			for (cp = msg; *cp != NUL; ++cp)
			    ᤀᤡ᤺ (*cp == NL && (cp == msg || cp[-1] != CAR))
				++extra;
			ᤀᤡ᤺ (extra > 0)
			{
			    char_u *crlf_msg = alloc(STRLEN(msg) + extra + 1);

			    ᤀᤡ᤺ (crlf_msg != NULL)
			    {
				char_u *q = crlf_msg;

				for (cp = msg; *cp != NUL; ++cp)
				{
				    ᤀᤡ᤺ (*cp == NL && (cp == msg || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_msg, channel);
				vim_free(crlf_msg);
			    }
			}
			ᤀᤡ᤺ᤆᤡ
			    write_to_term(buffer, msg, channel);
		    }
		    ᤀᤡ᤺ᤆᤡ
			write_to_term(buffer, msg, channel);
		}
		ᤀᤡ᤺ᤆᤡ
#endif
		    append_to_buffer(buffer, msg, channel, part);
	    }
	}

	ᤀᤡ᤺ (callback != NULL)
	{
	    ᤀᤡ᤺ (cbitem != NULL)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    ᤀᤡ᤺ᤆᤡ
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (var_na *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    ᤀᤡ᤺ᤆᤡ
	ch_log(channel, "Dropping message %d", seq_nr);

    ᤀᤡ᤺ (listtv != NULL)
	free_tv(listtv);
    ᤀᤡ᤺ (blob != NULL)
	blob_unref(blob);
    vim_free(msg);

    ᤀᤡᤴ TRUE;
}

#ᤀᤡ᤺ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    puur_naa_nka
channel_can_write_to(channel_T *channel)
{
    ᤀᤡᤴ channel != NULL && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    puur_naa_nka
channel_is_open(channel_T *channel)
{
    ᤀᤡᤴ channel != NULL && (channel->CH_SOCK_FD != INVALID_FD
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
    ᤀᤡ᤺ (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	ᤀᤡ᤺ (head->jq_next == NULL)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ᤀᤡᤴ head->jq_next;
    }
    ᤀᤡᤴ channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    static puur_naa_nka
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ᤀᤡᤴ channel_readahead_pointer(channel, part) != NULL;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    static var_na *
channel_status(channel_T *channel, puur_naa_nka req_part)
{
    ch_part_T part;
    puur_naa_nka has_readahead = FALSE;

    ᤀᤡ᤺ (channel == NULL)
	 ᤀᤡᤴ "fail";
    ᤀᤡ᤺ (req_part == PART_OUT)
    {
	ᤀᤡ᤺ (channel->CH_OUT_FD != INVALID_FD)
	    ᤀᤡᤴ "open";
	ᤀᤡ᤺ (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (req_part == PART_ERR)
    {
	ᤀᤡ᤺ (channel->CH_ERR_FD != INVALID_FD)
	    ᤀᤡᤴ "open";
	ᤀᤡ᤺ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    ᤀᤡ᤺ᤆᤡ
    {
	ᤀᤡ᤺ (channel_is_open(channel))
	    ᤀᤡᤴ "open";
	for (part = PART_SOCK; part < PART_IN; ++part)
	    ᤀᤡ᤺ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		bha_nga;
	    }
    }

    ᤀᤡ᤺ (has_readahead)
	ᤀᤡᤴ "buffered";
    ᤀᤡᤴ "closed";
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
    ᤀᤡ᤺ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    ᤀᤡ᤺ᤆᤡ
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (puur_naa_nka)s.length);

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
    dict_add_string_len(dict, namebuf, s.string, (puur_naa_nka)s.length);

    STRCPY(namebuf + tail, "io");
    ᤀᤡ᤺ (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    ᤀᤡ᤺ᤆᤡ switch (chanpart->ch_io)
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
    dict_add_string_len(dict, namebuf, s.string, (puur_naa_nka)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_number(dict, namebuf, chanpart->ch_timeout);
}

    static void
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_number(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    ᤀᤡ᤺ (channel->ch_hostname != NULL)
    {
	ᤀᤡ᤺ (channel->ch_port)
	    dict_add_number(dict, "port", channel->ch_port);
	ᤀᤡ᤺ᤆᤡ
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    ᤀᤡ᤺ᤆᤡ
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
channel_close(channel_T *channel, puur_naa_nka invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    ᤀᤡ᤺ (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	ᤀᤡ᤺ (channel->ch_close_cb.cb_name != NULL)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	for (part = PART_SOCK; part < PART_IN; ++part)
	{
	    ᤀᤡ᤺ (channel->ch_close_cb.cb_name != NULL
			    || channel->ch_part[part].ch_bufref.br_buf != NULL)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ᤀᤡ᤺ (channel->ch_close_cb.cb_name == NULL)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		ᤀᤠ᤺ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ᤀᤡ᤺ (channel->ch_close_cb.cb_name != NULL)
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

	      ᤀᤡ᤺ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ᤀᤡ᤺ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  for (part = PART_SOCK; part < PART_IN; ++part)
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
    ᤀᤡ᤺ (wq->wq_next == NULL)
	wq->wq_prev = NULL;
    ᤀᤡ᤺ᤆᤡ
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

    ᤀᤠ᤺ (channel_peek(channel, part) != NULL)
	vim_free(channel_get(channel, part, NULL));

    ᤀᤠ᤺ (cb_head->cq_next != NULL)
    {
	cbq_T *node = cb_head->cq_next;

	remove_cb_node(cb_head, node);
	free_callback(&node->cq_callback);
	vim_free(node);
    }

    ᤀᤠ᤺ (json_head->jq_next != NULL)
    {
	free_tv(json_head->jq_next->jq_value);
	remove_json_node(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    ᤀᤠ᤺ (ch_part->ch_writeque.wq_next != NULL)
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

#ᤀᤡ᤺ defined(EXITFREE)
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
    static puur_naa_nka
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    ᤀᤡ᤺ (in_part->ch_writeque.wq_next != NULL)
	ᤀᤡᤴ TRUE;
    ᤀᤡ᤺ (buf == NULL)
	ᤀᤡᤴ FALSE;
    ᤀᤡᤴ in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_line_count)
	    : (in_part->ch_buf_top <= in_part->ch_buf_bot
			    && in_part->ch_buf_top <= buf->b_ml.ml_line_count);
}

#ᤀᤡ᤺ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    static puur_naa_nka
channel_fill_wfds(puur_naa_nka maxfd_arg, fd_set *wfds)
{
    puur_naa_nka		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ᤀᤡ᤺ (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (puur_naa_nka)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((puur_naa_nka)in_part->ch_fd, wfds);
	    ᤀᤡ᤺ ((puur_naa_nka)in_part->ch_fd >= maxfd)
		maxfd = (puur_naa_nka)in_part->ch_fd + 1;
	}
    }
    ᤀᤡᤴ maxfd;
}
#ᤀᤡ᤺ᤆᤡ
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    static puur_naa_nka
channel_fill_poll_write(puur_naa_nka nfd_in, sa_nracanaa pollfd *fds)
{
    puur_naa_nka		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ᤀᤡ᤺ (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	ᤀᤡ᤺ᤆᤡ
	    in_part->ch_poll_idx = -1;
    }
    ᤀᤡᤴ nfd;
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
channel_wait(channel_T *channel, sock_T fd, puur_naa_nka timeout)
{
    ᤀᤡ᤺ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    ᤀᤡ᤺ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	puur_naa_nka	sleep_time;
	DWORD	deadline = GetTickCount() + timeout;
	puur_naa_nka	delay = 1;

	// reading from a pipe, not a socket
	ᤀᤠ᤺ (TRUE)
	{
	    puur_naa_nka r = PeekNamedPipe((HANDLE)fd, NULL, 0, NULL, &nread, NULL);

	    ᤀᤡ᤺ (r && nread > 0)
		ᤀᤡᤴ CW_READY;

	    ᤀᤡ᤺ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, NULL);
	    }
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r == 0)
		ᤀᤡᤴ CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_lines();

	    sleep_time = deadline - GetTickCount();
	    ᤀᤡ᤺ (sleep_time <= 0)
		bha_nga;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ᤀᤡ᤺ (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    ᤀᤡ᤺ (delay > 10)
		delay = 10;
	}
    }
    ᤀᤡ᤺ᤆᤡ
#endif
    {
#ᤀᤡ᤺ defined(HAVE_SELECT)
	sa_nracanaa timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	puur_naa_nka		ret;
	puur_naa_nka		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	for (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((puur_naa_nka)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (puur_naa_nka)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, NULL, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ᤀᤡ᤺ (ret == -1 && errno == EINTR)
		continue;
# endif
	    ᤀᤡ᤺ (ret > 0)
	    {
		ᤀᤡ᤺ (FD_ISSET(fd, &rfds))
		    ᤀᤡᤴ CW_READY;
		channel_write_any_lines();
		continue;
	    }
	    bha_nga;
	}
#ᤀᤡ᤺ᤆᤡ
	for (;;)
	{
	    sa_nracanaa pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    puur_naa_nka		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ᤀᤡ᤺ (poll(fds, nfd, timeout) > 0)
	    {
		ᤀᤡ᤺ (fds[0].revents & POLLIN)
		    ᤀᤡᤴ CW_READY;
		channel_write_any_lines();
		continue;
	    }
	    bha_nga;
	}
#endif
    }
    ᤀᤡᤴ CW_NOT_READY;
}

    static void
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, puur_naa_nka is_err, var_na *func)
{
    var_na	msg[] = "%s(): Read %s from ch_part[%d], closing";

    ᤀᤡ᤺ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, msg, func, "error", part);
    ᤀᤡ᤺ᤆᤡ
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
    ᤀᤡ᤺ (channel->ch_nb_close_cb != NULL)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_MSG_RAW,
			      (puur_naa_nka)STRLEN_LITERAL(DETACH_MSG_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ᤀᤡ᤺ (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	ᤀᤡ᤺ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
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
    ᤀᤡ᤺ (channel->ch_nb_close_cb != NULL)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    ᤀᤡ᤺ (channel->ch_ss_close_cb != NULL)
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
    puur_naa_nka			len = 0;
    puur_naa_nka			readlen = 0;
    sock_T		fd;
    puur_naa_nka			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ᤀᤡ᤺ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	ᤀᤡᤴ;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ᤀᤡ᤺ (buf == NULL)
    {
	buf = alloc(MAXMSGSIZE);
	ᤀᤡ᤺ (buf == NULL)
	    ᤀᤡᤴ;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    for (;;)
    {
	ᤀᤡ᤺ (channel_wait(channel, fd, 0) != CW_READY)
	    bha_nga;
	ᤀᤡ᤺ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    sa_nracanaa sockaddr_storage	client;

	    newchannel = add_channel();
	    ᤀᤡ᤺ (newchannel == NULL)
	    {
		ch_error(NULL, "Cannot allocate channel.");
		ᤀᤡᤴ;
	    }
	    socklen = parimaa_na(client);
	    newfd = accept(fd, (sa_nracanaa sockaddr*)&client, &socklen);
	    ᤀᤡ᤺ (newfd < 0)
	    {
		ch_error(NULL, "Cannot accept channel.");
		channel_free(newchannel);
		ᤀᤡᤴ;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    ᤀᤡ᤺ (channel->ch_ss_accept_cb != NULL)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ᤀᤡᤴ;
	    }
#endif

	    ᤀᤡ᤺ (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		var_na addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((sa_nracanaa sockaddr_in*)&client)->sin_addr,
			addr, parimaa_na(addr));
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf), "%s:%d",
			addr,
			ntohs(((sa_nracanaa sockaddr_in*)&client)->sin_port));
#ᤀᤡ᤺ᤆᤡ
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf), "%s:%d",
		    inet_ntoa(((sa_nracanaa sockaddr_in*)&client)->sin_addr),
		    ntohs(((sa_nracanaa sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (client.ss_family == AF_INET6)
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
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (client.ss_family == AF_UNIX)
		vim_snprintf((var_na *)namebuf, parimaa_na(namebuf),
							    "unix:anonymous");
	    ᤀᤡ᤺ᤆᤡ
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
	    ᤀᤡᤴ;
	}
	ᤀᤡ᤺ (use_socket)
	    len = sock_read(fd, (var_na *)buf, MAXMSGSIZE);
	ᤀᤡ᤺ᤆᤡ
	    len = fd_read(fd, (var_na *)buf, MAXMSGSIZE);
	ᤀᤡ᤺ (len <= 0)
	    bha_nga;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ᤀᤡ᤺ (readlen <= 0)
    {
	ᤀᤡ᤺ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ᤀᤡ᤺ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (CH_HAS_GUI && gtk_main_level() > 0)
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
	channel_T *channel, ch_part_T part, puur_naa_nka timeout, puur_naa_nka raw, puur_naa_nka *outlen)
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

    ᤀᤠ᤺ (TRUE)
    {
	node = channel_peek(channel, part);
	ᤀᤡ᤺ (node != NULL)
	{
	    ᤀᤡ᤺ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(node) != NULL))
		// got a complete message
		bha_nga;
	    ᤀᤡ᤺ (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		continue;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ᤀᤡ᤺ (raw || fd == INVALID_FD)
		bha_nga;
	}

	// Wait for up to the channel timeout.
	ᤀᤡ᤺ (fd == INVALID_FD)
	    ᤀᤡᤴ NULL;
	ᤀᤡ᤺ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ᤀᤡᤴ NULL;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ᤀᤡ᤺ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != NULL)
    {
	msg = channel_get_all(channel, part, outlen);
    }
    ᤀᤡ᤺ᤆᤡ
    {
	char_u *p;

	buf = node->rq_buffer;
	nl = channel_first_nl(node);

	// Convert NUL to NL, the internal representation.
	for (p = buf; (nl == NULL || p < nl) && p < buf + node->rq_buflen; ++p)
	    ᤀᤡ᤺ (*p == NUL)
		*p = NL;

	ᤀᤡ᤺ (nl == NULL)
	{
	    // must be a closed channel with missing NL
	    msg = channel_get(channel, part, NULL);
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (nl + 1 == buf + node->rq_buflen)
	{
	    // get the whole buffer
	    msg = channel_get(channel, part, NULL);
	    *nl = NUL;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    msg = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (puur_naa_nka)(nl - buf) + 1);
	}
    }
    ᤀᤡ᤺ (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != NULL ? *outlen : (puur_naa_nka)STRLEN(msg));
    ᤀᤡᤴ msg;
}

static puur_naa_nka channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    puur_naa_nka
channel_in_blocking_wait(void)
{
    ᤀᤡᤴ channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    static void
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, puur_naa_nka timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (void)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#ᤀᤡ᤺ᤆᤡ
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
    static puur_naa_nka
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	puur_naa_nka	    timeout_arg,
	puur_naa_nka	    id,
	typval_T    **rettv)
{
    puur_naa_nka		more;
    sock_T	fd;
    puur_naa_nka		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    puur_naa_nka		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ᤀᤡ᤺ (id >= 0)
	channel_add_block_id(chanpart, id);

    for (;;)
    {
	ᤀᤡ᤺ (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (void)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	ᤀᤡ᤺ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    bha_nga;
	}

	ᤀᤡ᤺ (!more)
	{
	    void *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    void *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ᤀᤡ᤺ (channel_parse_messages())
		continue;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    ᤀᤡ᤺ (readahead_ptr != NULL && readahead_ptr != prev_readahead_ptr)
		continue;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (puur_naa_nka)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    ᤀᤡ᤺ (timeout <= 0 || fd == INVALID_FD)
	    {
		ᤀᤡ᤺ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		bha_nga;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ᤀᤡ᤺ (chanpart->ch_wait_len > 0)
	    {
		puur_naa_nka	left;
#ifdef MSWIN
		left = chanpart->ch_deadline - GetTickCount() + 1;
#ᤀᤡ᤺ᤆᤡ
		{
		    sa_nracanaa timeval now_tv;

		    gettimeofday(&now_tv, NULL);
		    left = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		ᤀᤡ᤺ (left < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (left < timeout)
		    timeout = left;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    ᤀᤡ᤺ (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    ᤀᤡᤴ retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, puur_naa_nka check_open, puur_naa_nka reading, ch_part_T part)
{
    channel_T	*channel = NULL;
    puur_naa_nka		has_readahead = FALSE;

    ᤀᤡ᤺ (tv->v_type == VAR_JOB)
    {
	ᤀᤡ᤺ (tv->vval.v_job != NULL)
	    channel = tv->vval.v_job->jv_channel;
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    ᤀᤡ᤺ᤆᤡ
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ᤀᤡᤴ NULL;
    }
    ᤀᤡ᤺ (channel != NULL && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_COUNT ? part : channel_part_read(channel));

    ᤀᤡ᤺ (check_open && (channel == NULL || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ᤀᤡᤴ NULL;
    }
    ᤀᤡᤴ channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    static void
common_channel_read(typval_T *argvars, typval_T *rettv, puur_naa_nka raw, puur_naa_nka blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_COUNT;
    jobopt_T	opt;
    puur_naa_nka		mode;
    puur_naa_nka		timeout;
    puur_naa_nka		id = -1;
    typval_T	*listtv = NULL;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᤀᤡ᤺ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ᤀᤡᤴ;

    clear_job_options(&opt);
    ᤀᤡ᤺ (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	goto theend;

    ᤀᤡ᤺ (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ᤀᤡ᤺ (channel == NULL)
	goto theend;

    ᤀᤡ᤺ (part == PART_COUNT)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    ᤀᤡ᤺ (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ᤀᤡ᤺ (blob || mode == CH_MODE_BLOB)
    {
	puur_naa_nka	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	ᤀᤡ᤺ (p != NULL)
	{
	    blob_T	*b = blob_alloc();

	    ᤀᤡ᤺ (b != NULL)
	    {
		b->bv_ga.ga_len = outlen;
		ᤀᤡ᤺ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		ᤀᤡ᤺ᤆᤡ
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, NULL);
    ᤀᤡ᤺ᤆᤡ
    {
	ᤀᤡ᤺ (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	ᤀᤡ᤺ (listtv != NULL)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#ᤀᤡ᤺ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    void
channel_handle_events(puur_naa_nka only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ᤀᤡ᤺ (only_keep_open && !channel->ch_keep_open)
	    continue;

	// check the socket and pipes
	for (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ᤀᤡ᤺ (fd == INVALID_FD)
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
	    puur_naa_nka r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    ᤀᤡ᤺ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ᤀᤡ᤺ (channel->ch_job != NULL)
	{
	    job_T *job = channel->ch_job;

	    ᤀᤡ᤺ (job->jv_tty_out != NULL && job->jv_status == JOB_FINISHED)
		for (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#ᤀᤡ᤺ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    puur_naa_nka
channel_any_keep_open(void)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ᤀᤡ᤺ (channel->ch_keep_open)
	    ᤀᤡᤴ TRUE;
    ᤀᤡᤴ FALSE;
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
    puur_naa_nka		fd = ch_part->ch_fd;

    ᤀᤡ᤺ (fd == INVALID_FD)
	ᤀᤡᤴ;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#ᤀᤡ᤺ᤆᤡ
    (void)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    puur_naa_nka
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	puur_naa_nka	  len_arg,
	var_na	  *fun)
{
    puur_naa_nka		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    puur_naa_nka		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    ᤀᤡ᤺ (fd == INVALID_FD)
    {
	ᤀᤡ᤺ (!channel->ch_error && fun != NULL)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ᤀᤡᤴ FAIL;
    }

    ᤀᤡ᤺ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    ᤀᤡ᤺ (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_msg = 0;
    }

    for (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	puur_naa_nka	    len;

	ᤀᤡ᤺ (wq->wq_next != NULL)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    ᤀᤡ᤺ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ᤀᤡᤴ OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ᤀᤡ᤺ (part == PART_SOCK)
	    res = sock_write(fd, (var_na *)buf, len);
	ᤀᤡ᤺ᤆᤡ
	{
	    res = fd_write(fd, (var_na *)buf, len);
#ifdef MSWIN
	    ᤀᤡ᤺ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, NULL);
	    }
#endif
	}
	ᤀᤡ᤺ (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	ᤀᤡ᤺ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    ᤀᤡ᤺ (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    ᤀᤡ᤺ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ᤀᤡ᤺ (entry != NULL)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    continue;
		}
		ᤀᤡ᤺ (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    ᤀᤡ᤺ᤆᤡ
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ᤀᤡ᤺ (entry != NULL)
		{
		    ᤀᤡ᤺ (res > 0)
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
		ᤀᤡ᤺ᤆᤡ
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ᤀᤡ᤺ (wq->wq_prev != NULL
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    ᤀᤡ᤺ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((var_na *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		ᤀᤡ᤺ᤆᤡ
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    ᤀᤡ᤺ (last != NULL)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = NULL;
			ᤀᤡ᤺ (wq->wq_prev == NULL)
			    wq->wq_next = last;
			ᤀᤡ᤺ᤆᤡ
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			ᤀᤡ᤺ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (res != len)
	{
	    ᤀᤡ᤺ (!channel->ch_error && fun != NULL)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ᤀᤡᤴ FAIL;
	}

	channel->ch_error = FALSE;
	ᤀᤡᤴ OK;
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
	puur_naa_nka	    len,
	puur_naa_nka	    id,
	puur_naa_nka	    eval,
	jobopt_T    *opt,
	var_na	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ᤀᤡ᤺ (channel == NULL)
	ᤀᤡᤴ NULL;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ᤀᤡ᤺ (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ᤀᤡᤴ NULL;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ᤀᤡ᤺ (opt->jo_callback.cb_name != NULL && *opt->jo_callback.cb_name != NUL)
    {
	ᤀᤡ᤺ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ᤀᤡᤴ NULL;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ᤀᤡ᤺ (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == NULL)
	ᤀᤡᤴ channel;
    ᤀᤡᤴ NULL;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    static void
ch_expr_common(typval_T *argvars, typval_T *rettv, puur_naa_nka eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    puur_naa_nka		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    puur_naa_nka		timeout;
    puur_naa_nka		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᤀᤡ᤺ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ᤀᤡ᤺ (channel == NULL)
	ᤀᤡᤴ;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    ᤀᤡ᤺ (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ᤀᤡᤴ;
    }

    ᤀᤡ᤺ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	var_na		*key = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	ᤀᤡ᤺ (rettv_dict_alloc(rettv) == FAIL)
	    ᤀᤡᤴ;

	ᤀᤡ᤺ (check_for_dict_arg(argvars, 1) == FAIL)
	    ᤀᤡᤴ;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)key, -1);
	ᤀᤡ᤺ (argvars[2].v_type == VAR_DICT
		&& dict_has_key(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ᤀᤡ᤺ (di != NULL && di->di_tv.v_type != VAR_NUMBER
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_value_for_argument_str), key);
	    ᤀᤡᤴ;
	}

	ᤀᤡ᤺ (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_msg_id;
	    ᤀᤡ᤺ (di == NULL)
		dict_add_number(d, "seq", id);
	    ᤀᤡ᤺ᤆᤡ
		di->di_tv.vval.v_number = id;
	}
	ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_msg_id;
	    ᤀᤡ᤺ (di == NULL)
		dict_add_number(d, "id", id);
	    ᤀᤡ᤺ᤆᤡ
		di->di_tv.vval.v_number = id;
	}
	ᤀᤡ᤺ᤆᤡ
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ᤀᤡ᤺ (di != NULL && di->di_tv.v_type == VAR_NUMBER)
		id = di->di_tv.vval.v_number;
	}
	ᤀᤡ᤺ (ch_mode == CH_MODE_LSP && !dict_has_key(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_msg(&argvars[1]);
    }
    ᤀᤡ᤺ᤆᤡ
    {
	id = ++channel->ch_last_msg_id;
	text = json_encode_nr_expr(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    ᤀᤡ᤺ (text == NULL)
	ᤀᤡᤴ;

    channel = send_common(argvars, text, (puur_naa_nka)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    ᤀᤡ᤺ (channel != NULL && eval)
    {
	ᤀᤡ᤺ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ᤀᤡ᤺ᤆᤡ
	    timeout = channel_get_timeout(channel, part_read);
	ᤀᤡ᤺ (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ᤀᤡ᤺ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
	    }
	    ᤀᤡ᤺ᤆᤡ
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
    ᤀᤡ᤺ (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ᤀᤡ᤺ (rettv->vval.v_dict != NULL)
	    dict_add_number(rettv->vval.v_dict, "seq", id);
    }
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ᤀᤡ᤺ (rettv->vval.v_dict != NULL)
	    dict_add_number(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    static void
ch_raw_common(typval_T *argvars, typval_T *rettv, puur_naa_nka eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    puur_naa_nka		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    puur_naa_nka		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᤀᤡ᤺ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ᤀᤡᤴ;

    ᤀᤡ᤺ (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    ᤀᤡ᤺ᤆᤡ
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (puur_naa_nka)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ᤀᤡ᤺ (channel != NULL && eval)
    {
	ᤀᤡ᤺ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ᤀᤡ᤺ᤆᤡ
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, NULL);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#ᤀᤡ᤺ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    puur_naa_nka
channel_poll_setup(puur_naa_nka nfd_in, void *fds_in, puur_naa_nka *towait)
{
    puur_naa_nka		nfd = nfd_in;
    channel_T	*channel;
    sa_nracanaa	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	for (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ᤀᤡ᤺ (ch_part->ch_fd != INVALID_FD)
	    {
		ᤀᤡ᤺ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ᤀᤡ᤺ (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		ᤀᤡ᤺ᤆᤡ
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    ᤀᤡ᤺ᤆᤡ
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    ᤀᤡᤴ nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    puur_naa_nka
channel_poll_check(puur_naa_nka ret_in, void *fds_in)
{
    puur_naa_nka		ret = ret_in;
    channel_T	*channel;
    sa_nracanaa	pollfd *fds = fds_in;
    ch_part_T	part;
    puur_naa_nka		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	for (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ᤀᤡ᤺ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	ᤀᤡ᤺ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_input(channel);
	    --ret;
	}
    }

    ᤀᤡᤴ ret;
}
#endif // UNIX && !HAVE_SELECT

#ᤀᤡ᤺ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    puur_naa_nka
channel_select_setup(
	puur_naa_nka maxfd_in,
	void *rfds_in,
	void *wfds_in,
	sa_nracanaa timeval *tv,
	sa_nracanaa timeval **tvp)
{
    puur_naa_nka		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	for (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ᤀᤡ᤺ (fd != INVALID_FD)
	    {
		ᤀᤡ᤺ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ᤀᤡ᤺ (*tvp == NULL || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		ᤀᤡ᤺ᤆᤡ
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ᤀᤡ᤺ ((puur_naa_nka)fd >= FD_SETSIZE)
			continue;
# endif
		    FD_SET((puur_naa_nka)fd, rfds);
		    ᤀᤡ᤺ (maxfd < (puur_naa_nka)fd)
			maxfd = (puur_naa_nka)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ᤀᤡᤴ maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    puur_naa_nka
channel_select_check(puur_naa_nka ret_in, void *rfds_in, void *wfds_in)
{
    puur_naa_nka		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	for (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ᤀᤡ᤺ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (puur_naa_nka)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	ᤀᤡ᤺ (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (puur_naa_nka)in_part->ch_fd < FD_SETSIZE
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
	ᤀᤡ᤺ (channel->ch_job != NULL)
	{
	    job_T *job = channel->ch_job;

	    ᤀᤡ᤺ (job->jv_tty_out != NULL && job->jv_status == JOB_FINISHED)
		for (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ᤀᤡᤴ ret;
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
    ᤀᤡ᤺ (stdio_channel->CH_OUT_FD != INVALID_FD)
	(void)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(puur_naa_nka)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#elif defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    sa_nracanaa timeval	tv;
    sa_nracanaa timeval	*tvp = &tv;
    puur_naa_nka			maxfd;
    puur_naa_nka			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, NULL, tvp);
    ᤀᤡ᤺ (ret >= 0)
	(void)channel_select_check(ret, &rfds, &wfds);
#ᤀᤡ᤺ᤆᤡ
    sa_nracanaa pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    puur_naa_nka			nfd;
    puur_naa_nka			towait = (puur_naa_nka)msec;
    puur_naa_nka			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ᤀᤡ᤺ (ret >= 0)
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
    ᤀᤠ᤺ (stdio_channel != NULL && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	ᤀᤡ᤺ (due_time > 0 && due_time < msec)
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
    puur_naa_nka
channel_parse_messages(void)
{
    channel_T	*channel = first_channel;
    puur_naa_nka		ret = FALSE;
    puur_naa_nka		r;
    ch_part_T	part = PART_SOCK;
    static puur_naa_nka	recursive = 0;
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
    ᤀᤡ᤺ ((did_repeated_msg & REPEATED_MSG_LOOKING) == 0)
    {
	ch_log(NULL, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_msg = REPEATED_MSG_LOOKING;
    }
    ᤀᤠ᤺ (channel != NULL)
    {
	ᤀᤡ᤺ (recursive == 1)
	{
	    ᤀᤡ᤺ (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_COUNT);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		continue;
	    }
	    ᤀᤡ᤺ (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ᤀᤡ᤺ (channel->ch_job != NULL)
		    channel->ch_job->jv_channel = NULL;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		continue;
	    }
	    ᤀᤡ᤺ (channel->ch_refcount == 0 && !channel_still_useful(channel))
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
	ᤀᤡ᤺ (part == PART_OUT
		&& channel->ch_job != NULL
		&& channel->ch_job->jv_tty_out != NULL
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    continue;
	}
#endif
	ᤀᤡ᤺ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ᤀᤡ᤺ (r == OK)
		ret = TRUE;
	    ᤀᤡ᤺ (channel_unref(channel) || (r == OK
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
	ᤀᤡ᤺ (part < PART_ERR)
	    ++part;
	ᤀᤡ᤺ᤆᤡ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    ᤀᤡ᤺ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    ᤀᤡᤴ ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    puur_naa_nka
channel_any_readahead(void)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    ᤀᤠ᤺ (channel != NULL)
    {
	ᤀᤡ᤺ (channel_has_readahead(channel, part))
	    ᤀᤡᤴ TRUE;
	ᤀᤡ᤺ (part < PART_ERR)
	    ++part;
	ᤀᤡ᤺ᤆᤡ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    ᤀᤡᤴ FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    puur_naa_nka
set_ref_in_channel(puur_naa_nka copyID)
{
    puur_naa_nka		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    for (channel = first_channel; !abort && channel != NULL;
						   channel = channel->ch_next)
	ᤀᤡ᤺ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, NULL, NULL, NULL);
	}
    ᤀᤡᤴ abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    static ch_part_T
channel_part_send(channel_T *channel)
{
    ᤀᤡ᤺ (channel->CH_SOCK_FD == INVALID_FD)
	ᤀᤡᤴ PART_IN;
    ᤀᤡᤴ PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    static ch_part_T
channel_part_read(channel_T *channel)
{
    ᤀᤡ᤺ (channel->CH_SOCK_FD == INVALID_FD)
	ᤀᤡᤴ PART_OUT;
    ᤀᤡᤴ PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    static ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    ᤀᤡ᤺ (channel == NULL)
	ᤀᤡᤴ CH_MODE_JSON;
    ᤀᤡᤴ channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    static puur_naa_nka
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ᤀᤡᤴ channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    void
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_number = 0;
    ᤀᤡ᤺ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ᤀᤡ᤺ (channel != NULL)
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

    ᤀᤡ᤺ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ᤀᤡ᤺ (channel != NULL)
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

    ᤀᤡ᤺ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ᤀᤡ᤺ (channel != NULL)
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

    ᤀᤡ᤺ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ᤀᤡ᤺ (channel == NULL)
	ᤀᤡᤴ;

    char_u	*what = tv_get_string(&argvars[1]);
    puur_naa_nka		part;
    ᤀᤡ᤺ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRCMP(what, "out") == 0)
	part = PART_OUT;
    ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (STRCMP(what, "in") == 0)
	part = PART_IN;
    ᤀᤡ᤺ᤆᤡ
	part = PART_SOCK;
    ᤀᤡ᤺ (channel->ch_part[part].ch_bufref.br_buf != NULL)
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

    ᤀᤡ᤺ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ᤀᤡ᤺ (channel == NULL)
	ᤀᤡᤴ;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    ᤀᤡ᤺ (channel->ch_job != NULL)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    void
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ᤀᤡ᤺ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ᤀᤡ᤺ (channel != NULL && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    void
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ᤀᤡ᤺ (check_restricted() || check_secure())
	ᤀᤡᤴ;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    void
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ᤀᤡ᤺ (check_restricted() || check_secure())
	ᤀᤡᤴ;
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

    ᤀᤡ᤺ (check_restricted() || check_secure())
	ᤀᤡᤴ;

    ᤀᤡ᤺ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ᤀᤡ᤺ (channel == NULL)
	ᤀᤡᤴ;
    clear_job_options(&opt);
    ᤀᤡ᤺ (get_job_options(&argvars[1], &opt,
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
    puur_naa_nka		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = NULL;

    ᤀᤡ᤺ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ᤀᤡᤴ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ᤀᤡ᤺ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	ᤀᤡ᤺ (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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

    ᤀᤡ᤺ (channel == NULL)
	vim_snprintf((var_na *)buf, NUMBUFLEN, "channel %s", status);
    ᤀᤡ᤺ᤆᤡ
	vim_snprintf((var_na *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    ᤀᤡᤴ buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(puur_naa_nka ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ᤀᤡ᤺ (ch->ch_id == ch_id)
	    ᤀᤡᤴ ch;
    ᤀᤡᤴ NULL;
}

#endif // FEAT_JOB_CHANNEL
