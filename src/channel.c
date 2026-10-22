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

#ضمن "vim.h"

#ܐܢ defined(FEAT_JOB_CHANNEL)

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
تعريفنوع بنية sockaddr_un {
    ADDRESS_FAMILY sun_family;
    حرف sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ܐܚܪܢܐ
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) اكتب(sd, buf, len)
# define sock_read(sd, buf, len) اقرأ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) اقرأ(fd, buf, len)
# define fd_write(sd, buf, len) اكتب(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

ساكن فراغ channel_read(channel_T *channel, ch_part_T part, حرف *func);
ساكن فراغ channel_wait_any(long msec);
ساكن ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
ساكن صحيح channel_get_timeout(channel_T *channel, ch_part_T part);
ساكن channel_T *channel_open_stdio(فراغ);
ساكن ch_part_T channel_part_send(channel_T *channel);
ساكن ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    لكل ((ch) = first_channel; (ch) != عدم; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ساكن صحيح safe_to_invoke_callback = 0;

#ifdef MSWIN
    ساكن صحيح
fd_read(sock_T fd, حرف *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ܐܢ (!ReadFile(h, buf, (DWORD)len, &nread, عدم))
	ܦܢܐ -1;
    ܦܢܐ (صحيح)nread;
}

    ساكن صحيح
fd_write(sock_T fd, حرف *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    ܟܡܐ (todo > 0)
    {
	ܐܢ (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	ܐܚܪܢܐ
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	ܐܢ (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    ܐܢ (err != ERROR_IO_PENDING)
		ܦܢܐ -1;
	    ܐܢ (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		ܦܢܐ -1;
	    FlushFileBuffers(h);
	}
	ܐܚܪܢܐ ܐܢ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    اكسر;
	todo -= nwrite;
	done += nwrite;
    }
    ܦܢܐ (صحيح)done;
}

    ساكن فراغ
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(msg) (فراغ)semsg("%s: %s", msg, strerror_win32(errno))

    ساكن حرف *
strerror_win32(صحيح eno)
{
    ساكن LPVOID msgbuf = عدم;
    char_u *ptr;

    ܐܢ (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = عدم;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	عدم,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	عدم);
    ܐܢ (msgbuf != عدم)
	// chomp \r or \n
	لكل (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    بدل (*ptr)
	    {
		حالة '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    اكسر;
		حالة '\n':
		    ܐܢ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    ܐܚܪܢܐ
			*ptr = ' ';
		    اكسر;
	    }
    ܦܢܐ msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
ساكن channel_T *first_channel = عدم;
ساكن صحيح next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(فراغ)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    ܐܢ (channel == عدم)
	ܦܢܐ عدم;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    لكل (part = PART_SOCK; part < PART_COUNT; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_inputHandler = (XtInputId)عدم;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_inputHandler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    ܐܢ (first_channel != عدم)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    ܦܢܐ channel;
}

    صحيح
has_any_channel(فراغ)
{
    ܦܢܐ first_channel != عدم;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    صحيح
channel_still_useful(channel_T *channel)
{
    صحيح has_sock_msg;
    صحيح	has_out_msg;
    صحيح	has_err_msg;

    // If the job was killed the channel is not expected to work anymore.
    ܐܢ (channel->ch_job_killed && channel->ch_job == عدم)
	ܦܢܐ FALSE;

    // If there is a close callback it may still need to be invoked.
    ܐܢ (channel->ch_close_cb.cb_name != عدم)
	ܦܢܐ TRUE;

    // If reading from or a buffer it's still useful.
    ܐܢ (channel->ch_part[PART_IN].ch_bufref.br_buf != عدم)
	ܦܢܐ TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_msg = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != عدم
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != عدم;
    has_out_msg = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != عدم
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != عدم;
    has_err_msg = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != عدم
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != عدم;
    ܦܢܐ (channel->ch_callback.cb_name != عدم && (has_sock_msg
		|| has_out_msg || has_err_msg))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != عدم
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != عدم)
		    && has_out_msg)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != عدم
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != عدم)
		    && has_err_msg);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    صحيح
channel_can_close(channel_T *channel)
{
    ܦܢܐ channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    ساكن فراغ
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    ساكن فراغ
channel_free_channel(channel_T *channel)
{
    ܐܢ (channel->ch_next != عدم)
	channel->ch_next->ch_prev = channel->ch_prev;
    ܐܢ (channel->ch_prev == عدم)
	first_channel = channel->ch_next;
    ܐܚܪܢܐ
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    ساكن فراغ
channel_free(channel_T *channel)
{
    ܐܢ (in_free_unref_items)
	ܦܢܐ;

    ܐܢ (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    ܐܚܪܢܐ
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
    ساكن صحيح
channel_may_free(channel_T *channel)
{
    ܐܢ (!channel_still_useful(channel))
    {
	channel_free(channel);
	ܦܢܐ TRUE;
    }
    ܦܢܐ FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    صحيح
channel_unref(channel_T *channel)
{
    ܐܢ (channel != عدم && --channel->ch_refcount <= 0)
	ܦܢܐ channel_may_free(channel);
    ܦܢܐ FALSE;
}

    صحيح
free_unused_channels_contents(صحيح copyID, صحيح mask)
{
    صحيح		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ܐܢ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    ܦܢܐ did_free;
}

    فراغ
free_unused_channels(صحيح copyID, صحيح mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    لكل (ch = first_channel; ch != عدم; ch = ch_next)
    {
	ch_next = ch->ch_next;
	ܐܢ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ܐܢ defined(FEAT_GUI)

# ܐܢ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    ساكن channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ܐܢ (fd == INVALID_FD)
	ܦܢܐ عدم;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	    ܐܢ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ܦܢܐ channel;
	    }
    }
    ܦܢܐ عدم;
}

    ساكن فراغ
channel_read_fd(صحيح fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ܐܢ (channel == عدم)
	ch_error(عدم, "Channel for fd %d not found", fd);
    ܐܚܪܢܐ
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ساكن فراغ
messageFromServerX11(XtPointer clientData,
		  صحيح *unused1 UNUSED,
		  XtInputId *unused2 UNUSED)
{
    channel_read_fd((صحيح)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  ܐܢ GTK_CHECK_VERSION(3,0,0)
    ساكن gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    ܦܢܐ TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  ܐܚܪܢܐ
    ساكن فراغ
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  GdkInputCondition unused2 UNUSED)
{
    channel_read_fd((صحيح)(long)clientData);
}
#  endif
# endif

    ساكن فراغ
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    ܐܢ (!CH_HAS_GUI)
	ܦܢܐ;

    // gets stuck in handling events for a not connected channel
    ܐܢ (channel->ch_keep_open)
	ܦܢܐ;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ܐܢ (channel->ch_part[part].ch_inputHandler == (XtInputId)عدم)
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
# ܐܚܪܢܐ
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ܐܢ (channel->ch_part[part].ch_inputHandler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   ܐܢ GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_inputHandler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   ܐܚܪܢܐ
	channel->ch_part[part].ch_inputHandler = gdk_input_add(
		(gint)channel->ch_part[part].ch_fd,
		(GdkInputCondition)
			     ((صحيح)GDK_INPUT_READ + (صحيح)GDK_INPUT_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ساكن فراغ
channel_gui_register(channel_T *channel)
{
    ܐܢ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    ܐܢ (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    ܐܢ (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    فراغ
channel_gui_register_all(فراغ)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    ساكن فراغ
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    ܐܢ (channel->ch_part[part].ch_inputHandler != (XtInputId)عدم)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	XtRemoveInput(channel->ch_part[part].ch_inputHandler);
	channel->ch_part[part].ch_inputHandler = (XtInputId)عدم;
    }
# ܐܚܪܢܐ
#  ifdef FEAT_GUI_GTK
    ܐܢ (channel->ch_part[part].ch_inputHandler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   ܐܢ GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_inputHandler);
#   ܐܚܪܢܐ
	gdk_input_remove(channel->ch_part[part].ch_inputHandler);
#   endif
	channel->ch_part[part].ch_inputHandler = 0;
    }
#  endif
# endif
}

    ساكن فراغ
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    لكل (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    ساكن صحيح
channel_connect(
	channel_T *channel,
	ثابت بنية sockaddr *server_addr,
	صحيح server_addrlen,
	صحيح *waittime)
{
    صحيح		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    ܟܡܐ (TRUE)
    {
	long	elapsed_msec = 0;
	صحيح	waitnow;
	صحيح	ret;

	ܐܢ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ܐܢ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    ܦܢܐ -1;
	}

	ܐܢ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ܐܢ (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#ܐܚܪܢܐ
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		ܦܢܐ -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ܐܢ (ret == 0)
	    // The connection could be established.
	    اكسر;

	SOCK_ERRNO;
	ܐܢ (*waittime < 0 || (errno != EWOULDBLOCK
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
	    ܦܢܐ -1;
	}
	ܐܚܪܢܐ ܐܢ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ܦܢܐ -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    بنية timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    صحيح			so_error = 0;
	    socklen_t		so_error_len = حجم(so_error);
	    بنية timeval	start_tv;
	    بنية timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, عدم);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, عدم, &tv);
	    ܐܢ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		ܦܢܐ -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ܐܢ (FD_ISSET(sd, &wfds))
		اكسر;
	    elapsed_msec = waitnow;
	    ܐܢ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		استمر;
	    }
#ܐܚܪܢܐ
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ܐܢ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ܐܢ (ret < 0 || (so_error != 0
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
		    ܦܢܐ -1;
		}
		ܐܚܪܢܐ ܐܢ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ܦܢܐ -1;
		}
	    }

	    ܐܢ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		اكسر;

	    gettimeofday(&end_tv, عدم);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	ܐܢ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ܐܢ (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ܐܢ (!got_int)
	    {
		ܐܢ (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		استمر;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	ܦܢܐ -1;
    }

    ܐܢ (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#ܐܚܪܢܐ
	(فراغ)fcntl(sd, F_SETFL, 0);
#endif
    }

    ܦܢܐ sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ثابت حرف *path,
	فراغ (*nb_close_cb)(فراغ))
{
    channel_T		*channel = عدم;
    صحيح			sd = -1;
    size_t		path_len = STRLEN(path);
    بنية sockaddr_un	server;
    size_t		server_len;
    صحيح			waittime = -1;

    ܐܢ (*path == NUL || path_len >= حجم(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ܦܢܐ عدم;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ܐܢ (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	ܦܢܐ عدم;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, حجم(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(بنية sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (بنية sockaddr *)&server, (صحيح)server_len,
								   &waittime);

    ܐܢ (sd < 0)
    {
	channel_free(channel);
	ܦܢܐ عدم;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ܦܢܐ channel;
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
	ثابت حرف *hostname,
	صحيح port,
	صحيح waittime,
	فراغ (*nb_close_cb)(فراغ))
{
    صحيح			sd = -1;
    channel_T		*channel = عدم;
#ifdef FEAT_IPV6
    صحيح			err;
    بنية addrinfo	hints;
    بنية addrinfo	*res = عدم;
    بنية addrinfo	*addr = عدم;
#ܐܚܪܢܐ
    بنية sockaddr_in	server;
    بنية hostent	*host = عدم;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ܐܢ (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	ܦܢܐ عدم;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ܐܢ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ܐܢ ((err = getaddrinfo(hostname, عدم, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ܦܢܐ عدم;
    }

    // Count the number of addresses for timeout distribution
    صحيح addr_count = 0;
    لكل (addr = res; addr != عدم; addr = addr->ai_next)
	addr_count++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ܐܢ (waittime == 0)
	waittime = 15;

    صحيح addr_index = 0;
    لكل (addr = res; addr != عدم; addr = addr->ai_next, addr_index++)
    {
	ثابت حرف  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ثابت فراغ  *src = عدم;
	حرف	    buf[NUMBUFLEN];
# endif
	صحيح	    try_waittime;
	صحيح	    before_waittime;
	صحيح	    consumed;
	صحيح	    remaining_addrs;

	ܐܢ (addr->ai_family == AF_INET6)
	{
	    بنية sockaddr_in6 *sai = (بنية sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	ܐܚܪܢܐ ܐܢ (addr->ai_family == AF_INET)
	{
	    بنية sockaddr_in *sai = (بنية sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ܐܢ (src != عدم)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, حجم(buf));
	    ܐܢ (dst == عدم)
		dst = hostname;
	    ܐܚܪܢܐ ܐܢ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ܐܢ (addr->ai_next == عدم)
	    try_waittime = waittime;
	ܐܚܪܢܐ ܐܢ (addr_index == 0)
	{
	    ܐܢ (waittime > 500)
		try_waittime = 250;
	    ܐܚܪܢܐ ܐܢ (waittime > 30)
		try_waittime = waittime / 2;
	    ܐܚܪܢܐ
		try_waittime = waittime;
	}
	ܐܚܪܢܐ
	{
	    remaining_addrs = addr_count - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (صحيح)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ܐܢ (waittime < 0)
	    waittime = 0;

	ܐܢ (sd >= 0)
	    اكسر;

	// If we have no time left, stop trying
	ܐܢ (waittime <= 0 && addr->ai_next != عدم)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    اكسر;
	}
    }

    freeaddrinfo(res);
#ܐܚܪܢܐ
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    ܐܢ ((host = gethostbyname(hostname)) == عدم)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	ܦܢܐ عدم;
    }
    {
	حرف *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], حجم(p));
	memcpy((حرف *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    ܐܢ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (بنية sockaddr *)&server, حجم(server),
								   &waittime);
#endif

    ܐܢ (sd < 0)
    {
	channel_free(channel);
	ܦܢܐ عدم;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ܦܢܐ channel;
}

    ساكن فراغ
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ܐܢ (callback->cb_name != عدم && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    ܐܚܪܢܐ
	cbp->cb_name = عدم;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ساكن فراغ
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
    ܐܢ (curbuf->b_ml.ml_mfp == عدم)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ساكن buf_T *
channel_find_buffer(char_u *name, صحيح err, صحيح msg)
{
    buf_T *buf = عدم;
    buf_T *save_curbuf = curbuf;

    ܐܢ (name != عدم && *name != NUL)
    {
	buf = buflist_findname(name);
	ܐܢ (buf == عدم)
	    buf = buflist_findname_exp(name);
    }

    ܐܢ (buf != عدم)
	ܦܢܐ buf;

    buf = buflist_new(name == عدم || *name == NUL ? عدم : name,
	    عدم, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ܐܢ (buf == عدم)
	ܦܢܐ عدم;
    prepare_buffer(buf);

    curbuf = buf;
    ܐܢ (msg)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    ܦܢܐ buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ساكن فراغ
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ܐܢ (opt->jo_set & JO_MODE)
	لكل (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    ܐܢ (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    ܐܢ (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    ܐܢ (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    ܐܢ (opt->jo_set & JO_TIMEOUT)
	لكل (part = PART_SOCK; part < PART_COUNT; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ܐܢ (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    ܐܢ (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ܐܢ (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    ܐܢ (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    ܐܢ (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    ܐܢ (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ܐܢ (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ܐܢ ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ܐܢ (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	ܐܢ (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    ܐܢ (buf == عدم)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	ܐܚܪܢܐ
	{
	    صحيح msg = TRUE;

	    ܐܢ (opt->jo_set2 & JO2_OUT_MSG)
		msg = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, msg);
	}
	ܐܢ (buf != عدم)
	{
	    ܐܢ (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    ܐܢ (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ܐܚܪܢܐ
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (حرف *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ܐܢ (buf->b_ml.ml_mfp == عدم)
		    prepare_buffer(buf);
	    }
	}
    }

    ܐܢ ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ܐܢ (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	ܐܢ (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	ܐܚܪܢܐ ܐܢ (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ܐܢ (buf == عدم)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	ܐܚܪܢܐ
	{
	    صحيح msg = TRUE;

	    ܐܢ (opt->jo_set2 & JO2_ERR_MSG)
		msg = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, msg);
	}
	ܐܢ (buf != عدم)
	{
	    ܐܢ (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ܐܢ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ܐܚܪܢܐ
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (حرف *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ܐܢ (buf->b_ml.ml_mfp == عدم)
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
    صحيح
channel_parse_socketserver_address(
	char_u	*address,
	صحيح	*port,
	char_u	**unix_path,
	bool	quiet)
{
    حرف    *rest;
    long    val;

    ܐܢ (*address == NUL)
	اذهب fail;

    ܐܢ (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	ܦܢܐ OK;
    }

    val = strtol((حرف *)(address), &rest, 10);
    ܐܢ (val < 0 || val >= 65536 || *rest != NUL)
    {
	ܐܢ (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	ܦܢܐ FAIL;
    }
    *port = (صحيح)val;

    ܦܢܐ OK;
fail:
    ܐܢ (!quiet)
	semsg(_(e_invalid_argument_str), address);
    ܦܢܐ FAIL;
}

/*
 * Implements ch_open().
 */
    ساكن channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    حرف	*rest;
    صحيح		port = 0;
    صحيح		is_ipv6 = FALSE;
    صحيح		is_unix = FALSE;
    صحيح		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = عدم;

    ܐܢ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ܦܢܐ عدم;

    address = tv_get_string(&argvars[0]);
    ܐܢ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ܦܢܐ عدم;

    ܐܢ (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	ܦܢܐ عدم;
    }

    ܐܢ (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    ܐܚܪܢܐ ܐܢ (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    ܐܚܪܢܐ ܐܢ (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	ܐܢ (p == عدم || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    ܦܢܐ عدم;
	}
    }
    ܐܚܪܢܐ
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	ܐܢ (p == عدم)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ܦܢܐ عدم;
	}
    }

    ܐܢ (!is_unix && !is_stdio)
    {
	port = strtol((حرف *)(p + 1), &rest, 10);
	ܐܢ (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ܦܢܐ عدم;
	}
	ܐܢ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	ܐܚܪܢܐ
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ܐܢ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	اذهب theend;
    ܐܢ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	اذهب theend;
    }

    ܐܢ (is_stdio)
	channel = channel_open_stdio();
    ܐܚܪܢܐ ܐܢ (is_unix)
	channel = channel_open_unix((حرف *)address, عدم);
    ܐܚܪܢܐ
	channel = channel_open((حرف *)address, port, opt.jo_waittime, عدم);
    ܐܢ (channel != عدم)
    {
	صحيح	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	ܐܢ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ܦܢܐ channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
ساكن channel_T *stdio_channel = عدم;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    ساكن channel_T *
channel_open_stdio(فراغ)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    ܐܢ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	ܦܢܐ عدم;
    }
    ܐܢ (stdio_channel != عدم)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ܦܢܐ عدم;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	صحيح	nul_fd;

	ܐܢ (!DuplicateHandle(proc, GetStdHandle(STD_INPUT_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ܦܢܐ عدم;
	}
	ܐܢ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ܦܢܐ عدم;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, عدم, OPEN_EXISTING, 0,
		عدم);
	ܐܢ (hnul != INVALID_HANDLE_VALUE)
	{
	    SetStdHandle(STD_INPUT_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ܐܢ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#ܐܚܪܢܐ
    {
	صحيح	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	ܐܢ (in_fd < 0 || out_fd < 0)
	{
	    ܐܢ (in_fd >= 0)
		close(in_fd);
	    ܐܢ (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ܦܢܐ عدم;
	}
	(فراغ)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(فراغ)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ܐܢ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    ܐܢ (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	ܦܢܐ عدم;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ܦܢܐ channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    حرف	*rest;
    صحيح		port;
    صحيح		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = عدم;

    ܐܢ (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ܦܢܐ عدم;

    arg = tv_get_string(&argvars[0]);
    ܐܢ (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	ܦܢܐ عدم;
    }

    ܐܢ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    ܐܚܪܢܐ
    {
	port = strtol((حرف *)(arg), &rest, 10);
	ܐܢ (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ܦܢܐ عدم;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ܐܢ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	اذهب theend;
    ܐܢ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	اذهب theend;
    }

    ܐܢ (is_unix)
	channel = channel_listen_unix((حرف *)arg, عدم, true);
    ܐܚܪܢܐ
	channel = channel_listen(port, عدم);
    ܐܢ (channel != عدم)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ܦܢܐ channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	صحيح port_in,
	فراغ (*nb_close_cb)(فراغ))
{
    صحيح			sd = -1;
    بنية sockaddr_in	server;
#ifndef FEAT_IPV6
    بنية hostent	*host;
#endif
    صحيح			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ܐܢ (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	ܦܢܐ عدم;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((حرف *)&server, 0, حجم(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ܐܢ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ܦܢܐ عدم;
    }

#ifdef MSWIN
    ܐܢ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ثابت حرف *)&val, حجم(val)) < 0)
#ܐܚܪܢܐ
    ܐܢ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, حجم(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ܦܢܐ عدم;
    }

    // Bind the socket to the port
    ܐܢ (bind(sd, (بنية sockaddr *)&server, حجم(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ܦܢܐ عدم;
    }

    ܐܢ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ܦܢܐ عدم;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ܐܢ (port_in == 0)
    {
	بنية sockaddr_in	addr;
	socklen_t		addr_len = حجم(addr);

	ܐܢ (getsockname(sd, (بنية sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ܦܢܐ channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	حرف *path,
	فراغ (*nb_close_cb)(فراغ),
	bool replace)
{
    صحيح			sd = -1;
    بنية sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    ܐܢ (path == عدم || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == عدم ? (حرف *)"" : path);
	ܦܢܐ عدم;
    }

    path_len = STRLEN(path);
    ܐܢ (path_len >= حجم(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ܦܢܐ عدم;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ܐܢ (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	ܦܢܐ عدم;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, حجم(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ܐܢ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ܦܢܐ عدم;
    }

    ܐܢ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(بنية sockaddr_un, sun_path) + path_len + 1;
    ܐܢ (bind(sd, (بنية sockaddr *)&server, (صحيح)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ܦܢܐ عدم;
    }

    ܐܢ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ܦܢܐ عدم;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ܦܢܐ channel;
}

     فراغ
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ܐܢ (*fd == INVALID_FD)
	ܦܢܐ;

    ܐܢ (part == PART_SOCK)
	sock_close(*fd);
    ܐܚܪܢܐ
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ܐܢ ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    ܐܢ (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    فراغ
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    ܐܢ (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#ܐܢ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ܐܢ (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    ܐܢ (out != INVALID_FD)
    {
#ܐܢ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#ܐܢ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    ܐܢ (err != INVALID_FD)
    {
#ܐܢ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#ܐܢ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    فراغ
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    ܐܢ (job->jv_in_buf == عدم)
	ܦܢܐ;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (حرف *)in_part->ch_bufref.br_buf->b_ffname);
    ܐܢ (options->jo_set & JO_IN_TOP)
    {
	ܐܢ (options->jo_in_top == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_top =
		in_part->ch_bufref.br_buf->b_ml.ml_line_count + 1;
	}
	ܐܚܪܢܐ
	    in_part->ch_buf_top = options->jo_in_top;
    }
    ܐܚܪܢܐ
	in_part->ch_buf_top = 1;
    ܐܢ (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    ܐܚܪܢܐ
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_line_count;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ساكن فراغ
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	صحيح	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    ܐܢ (item == عدم)
	ܦܢܐ;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = عدم;
    ܐܢ (item->cq_prev == عدم)
	head->cq_next = item;
    ܐܚܪܢܐ
	item->cq_prev->cq_next = item;
}

    ساكن فراغ
write_buf_line(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *line = ml_get_buf(buf, lnum, FALSE);
    صحيح	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    صحيح	    i;

    // Need to make a copy to be able to append a NL.
    ܐܢ ((p = alloc(len + 2)) == عدم)
	ܦܢܐ;
    memcpy((حرف *)p, (حرف *)line, len);

    ܐܢ (channel->ch_write_text_mode)
	p[len] = CAR;
    ܐܚܪܢܐ
    {
	لكل (i = 0; i < len; ++i)
	    ܐܢ (p[i] == NL)
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
    ساكن صحيح
can_write_buf_line(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    ܐܢ (in_part->ch_fd == INVALID_FD)
	ܦܢܐ FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ܐܢ (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    ܐܚܪܢܐ ܐܢ (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# ܐܢ defined(HAVE_SELECT)
	بنية timeval	tval;
	fd_set		wfds;
	صحيح		ret;

	FD_ZERO(&wfds);
	FD_SET((صحيح)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	لكل (;;)
	{
	    ret = select((صحيح)in_part->ch_fd + 1, عدم, &wfds, عدم, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ܐܢ (ret == -1 && errno == EINTR)
		استمر;
#  endif
	    ܐܢ (ret <= 0 || in_part->ch_block_write == 1)
	    {
		ܐܢ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		ܐܚܪܢܐ
		    ch_log(channel, "Input not ready for writing");
		ܦܢܐ FALSE;
	    }
	    اكسر;
	}
# ܐܚܪܢܐ
	بنية pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	ܐܢ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ܦܢܐ FALSE;
	}
	ܐܢ (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ܦܢܐ FALSE;
	}
# endif
    }
#endif
    ܦܢܐ TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    فراغ
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    صحيح		written = 0;

    ܐܢ (buf == عدم || in_part->ch_buf_append)
	ܦܢܐ;  // no buffer or using appending
    ܐܢ (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == عدم)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = عدم;
	ܦܢܐ;
    }

    لكل (lnum = in_part->ch_buf_top; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_line_count; ++lnum)
    {
	ܐܢ (!can_write_buf_line(channel))
	    اكسر;
	write_buf_line(buf, lnum, channel);
	++written;
    }

    ܐܢ (written == 1)
	ch_log(channel, "written line %d to channel", (صحيح)lnum - 1);
    ܐܚܪܢܐ ܐܢ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_top = lnum;
    ܐܢ (lnum > buf->b_ml.ml_line_count || lnum > in_part->ch_buf_bot)
    {
#ܐܢ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ܐܢ (channel->ch_job != عدم)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = عدم;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	ܐܢ (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    ܐܚܪܢܐ
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_line_count - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    فراغ
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	لكل (part = PART_SOCK; part < PART_COUNT; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ܐܢ (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = عدم;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    فراغ
channel_write_input(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    ܐܢ (in_part->ch_writeque.wq_next != عدم)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    ܐܚܪܢܐ ܐܢ (in_part->ch_bufref.br_buf != عدم)
    {
	ܐܢ (in_part->ch_buf_append)
	    channel_write_new_lines(in_part->ch_bufref.br_buf);
	ܐܚܪܢܐ
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    فراغ
channel_write_any_lines(فراغ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_input(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    فراغ
channel_write_new_lines(buf_T *buf)
{
    channel_T	*channel;
    صحيح		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	صحيح	    written = 0;

	ܐܢ (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    ܐܢ (in_part->ch_fd == INVALID_FD)
		استمر;  // pipe was closed
	    found_one = TRUE;
	    لكل (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_line_count;
								       ++lnum)
	    {
		ܐܢ (!can_write_buf_line(channel))
		    اكسر;
		write_buf_line(buf, lnum, channel);
		++written;
	    }

	    ܐܢ (written == 1)
		ch_log(channel, "written line %d to channel", (صحيح)lnum - 1);
	    ܐܚܪܢܐ ܐܢ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ܐܢ (lnum < buf->b_ml.ml_line_count)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_line_count - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    ܐܢ (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ساكن فراغ
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    ܐܢ (safe_to_invoke_callback == 0)
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

    ܦܢܐ head->rq_next;
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

    لكل (i = 0; i < node->rq_buflen; ++i)
	ܐܢ (buffer[i] == NL)
	    ܦܢܐ buffer + i;
    ܦܢܐ عدم;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, صحيح *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *p;

    ܐܢ (node == عدم)
	ܦܢܐ عدم;
    ܐܢ (outlen != عدم)
	*outlen += node->rq_buflen;
    // dispose of the node but keep the buffer
    p = node->rq_buffer;
    head->rq_next = node->rq_next;
    ܐܢ (node->rq_next == عدم)
	head->rq_prev = عدم;
    ܐܚܪܢܐ
	node->rq_next->rq_prev = عدم;
    vim_free(node);
    ܦܢܐ p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ساكن char_u *
channel_get_all(channel_T *channel, ch_part_T part, صحيح *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    لكل (node = head->rq_next; node != عدم; node = node->rq_next)
	len += node->rq_buflen;
    res = alloc(len + 1);
    ܐܢ (res == عدم)
	ܦܢܐ عدم;
    p = res;
    لكل (node = head->rq_next; node != عدم; node = node->rq_next)
    {
	mch_memmove(p, node->rq_buffer, node->rq_buflen);
	p += node->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    ܥܒܕ
    {
	p = channel_get(channel, part, عدم);
	vim_free(p);
    } ܟܡܐ (p != عدم);

    ܐܢ (outlen != عدم)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ܦܢܐ res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ܟܡܐ (p < res + len)
    {
	ܐܢ (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	ܐܚܪܢܐ ܐܢ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ܐܢ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ܟܡܐ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ܐܢ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    ܦܢܐ res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    فراغ
channel_consume(channel_T *channel, ch_part_T part, صحيح len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *node = head->rq_next;
    char_u *buf = node->rq_buffer;

    ܐܢ (len < 0 || (long_u)len > node->rq_buflen)
	ܦܢܐ;
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
    صحيح
channel_collapse(channel_T *channel, ch_part_T part, صحيح want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*node = head->rq_next;
    readq_T	*last_node;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ܐܢ (node == عدم || node->rq_next == عدم)
	ܦܢܐ FAIL;

    last_node = node->rq_next;
    len = node->rq_buflen + last_node->rq_buflen;
    ܐܢ (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	ܟܡܐ (last_node->rq_next != عدم
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_node) == عدم))
	{
	    last_node = last_node->rq_next;
	    len += last_node->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ܐܢ (newbuf == عدم)
	ܦܢܐ FAIL;	    // out of memory
    mch_memmove(p, node->rq_buffer, node->rq_buflen);
    p += node->rq_buflen;
    vim_free(node->rq_buffer);
    node->rq_buffer = newbuf;
    لكل (n = node; n != last_node; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    node->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    لكل (n = node->rq_next; n != last_node; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    node->rq_next = last_node->rq_next;
    ܐܢ (last_node->rq_next == عدم)
	head->rq_prev = node;
    ܐܚܪܢܐ
	last_node->rq_next->rq_prev = node;
    vim_free(last_node);
    ܦܢܐ OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    ساكن صحيح
channel_save(channel_T *channel, ch_part_T part, char_u *buf, صحيح len,
						      صحيح prepend, حرف *lead)
{
    readq_T *node;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    صحيح	    i;

    node = ALLOC_ONE(readq_T);
    ܐܢ (node == عدم)
	ܦܢܐ FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    node->rq_buffer = alloc(len + 1);
    ܐܢ (node->rq_buffer == عدم)
    {
	vim_free(node);
	ܦܢܐ FAIL;	    // out of memory
    }

    ܐܢ (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = node->rq_buffer;
	لكل (i = 0; i < len; ++i)
	    ܐܢ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	node->rq_buflen = (long_u)(p - node->rq_buffer);
    }
    ܐܚܪܢܐ
    {
	mch_memmove(node->rq_buffer, buf, len);
	node->rq_buffer[len] = NUL;
	node->rq_buflen = (long_u)len;
    }

    ܐܢ (prepend)
    {
	// prepend node to the head of the queue
	node->rq_next = head->rq_next;
	node->rq_prev = عدم;
	ܐܢ (head->rq_next == عدم)
	    head->rq_prev = node;
	ܐܚܪܢܐ
	    head->rq_next->rq_prev = node;
	head->rq_next = node;
    }
    ܐܚܪܢܐ
    {
	// append node to the tail of the queue
	node->rq_next = عدم;
	node->rq_prev = head->rq_prev;
	ܐܢ (head->rq_prev == عدم)
	    head->rq_next = node;
	ܐܚܪܢܐ
	    head->rq_prev->rq_next = node;
	head->rq_prev = node;
    }

    ܐܢ (ch_log_active() && lead != عدم)
	ch_log_literal(lead, channel, part, buf, len);

    ܦܢܐ OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    صحيح
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, عدم);
    صحيح		keeplen;
    صحيح		addlen;
    char_u	*p;

    ܐܢ (next == عدم)
	ܦܢܐ FALSE;

    keeplen = reader->js_end - reader->js_buf;
    ܐܢ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (صحيح)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	ܐܢ (p == عدم)
	{
	    vim_free(next);
	    ܦܢܐ FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    ܦܢܐ TRUE;
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
    ساكن صحيح
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*line_start;
    char_u	*p;
    int_u	hdr_len;
    صحيح		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ܟܡܐ (TRUE)
    {
	line_start = p;
	ܟܡܐ (*p != NUL && *p != '\n')
	    p++;
	ܐܢ (*p == NUL)			// partial header
	    ܦܢܐ MAYBE;
	p++;

	// process the content length field (if present)
	ܐܢ ((p - line_start > 16)
		&& STRNICMP(line_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((حرف *)line_start + 16, عدم, 10);
	    ܐܢ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ܦܢܐ FAIL;
	}

	ܐܢ ((p - line_start) == 2 && line_start[0] == '\r' &&
		line_start[1] == '\n')
	    // reached the empty line
	    اكسر;
    }

    ܐܢ (payload_len == -1)
	// Content-Length field is not present in the header
	ܦܢܐ FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ܐܢ (jsbuf_len < hdr_len + payload_len)
	ܦܢܐ MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    ܦܢܐ OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    صحيح
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    صحيح		status = OK;
    صحيح		ret;

    ܐܢ (channel_peek(channel, part) == عدم)
	ܦܢܐ FALSE;

    reader.js_buf = channel_get(channel, part, عدم);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ܐܢ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ܐܢ (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ܐܢ (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ܐܢ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	ܐܚܪܢܐ ܐܢ (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ܐܢ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    ܐܚܪܢܐ
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	ܐܚܪܢܐ
	{
	    item = ALLOC_ONE(jsonq_T);
	    ܐܢ (item == عدم)
		clear_tv(&listtv);
	    ܐܚܪܢܐ
	    {
		item->jq_no_callback = FALSE;
		item->jq_value = alloc_tv();
		ܐܢ (item->jq_value == عدم)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		ܐܚܪܢܐ
		{
		    *item->jq_value = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = عدم;
		    ܐܢ (item->jq_prev == عدم)
			head->jq_next = item;
		    ܐܚܪܢܐ
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    ܐܢ (status == OK)
	chanpart->ch_wait_len = 0;
    ܐܚܪܢܐ ܐܢ (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ܐܢ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (صحيح)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = GetTickCount() + 100L;
#ܐܚܪܢܐ
	    gettimeofday(&chanpart->ch_deadline, عدم);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ܐܢ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	ܐܚܪܢܐ
	{
	    صحيح timeout;
#ifdef MSWIN
	    timeout = (صحيح)(GetTickCount() - chanpart->ch_deadline) > 0;
#ܐܚܪܢܐ
	    {
		بنية timeval now_tv;

		gettimeofday(&now_tv, عدم);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    ܐܢ (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    ܐܚܪܢܐ
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ܐܢ (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    ܐܚܪܢܐ ܐܢ (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(صحيح)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, عدم);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    ܐܚܪܢܐ
	ret = FALSE;

    vim_free(reader.js_buf);
    ܦܢܐ ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ساكن فراغ
remove_cb_node(cbq_T *head, cbq_T *node)
{
    ܐܢ (node->cq_prev == عدم)
	head->cq_next = node->cq_next;
    ܐܚܪܢܐ
	node->cq_prev->cq_next = node->cq_next;
    ܐܢ (node->cq_next == عدم)
	head->cq_prev = node->cq_prev;
    ܐܚܪܢܐ
	node->cq_next->cq_prev = node->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    فراغ
remove_json_node(jsonq_T *head, jsonq_T *node)
{
    ܐܢ (node->jq_prev == عدم)
	head->jq_next = node->jq_next;
    ܐܚܪܢܐ
	node->jq_prev->jq_next = node->jq_next;
    ܐܢ (node->jq_next == عدم)
	head->jq_prev = node->jq_prev;
    ܐܚܪܢܐ
	node->jq_next->jq_prev = node->jq_prev;
    vim_free(node);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ساكن فراغ
channel_add_block_id(chanpart_T *chanpart, صحيح id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    ܐܢ (gap->ga_growsize == 0)
	ga_init2(gap, حجم(صحيح), 10);
    ܐܢ (ga_grow(gap, 1) == OK)
    {
	((صحيح *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ساكن فراغ
channel_remove_block_id(chanpart_T *chanpart, صحيح id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    صحيح		i;

    لكل (i = 0; i < gap->ga_len; ++i)
	ܐܢ (((صحيح *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    ܐܢ (i < gap->ga_len)
	    {
		صحيح *p = ((صحيح *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * حجم(صحيح));
	    }
	    ܦܢܐ;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ساكن صحيح
channel_has_block_id(chanpart_T *chanpart, صحيح id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    صحيح		i;

    لكل (i = 0; i < gap->ga_len; ++i)
	ܐܢ (((صحيح *)gap->ga_data)[i] == id)
	    ܦܢܐ TRUE;
    ܦܢܐ FALSE;
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
    ساكن صحيح
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	صحيح	    id,
	صحيح	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    ܟܡܐ (item != عدم)
    {
	list_T	    *l;
	typval_T    *tv;

	ܐܢ (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_value->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	ܐܚܪܢܐ
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ܐܢ (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ܐܢ (id > 0)
		{
		    ܐܢ (item->jq_value->v_type != VAR_DICT)
			اذهب nextitem;
		    d = item->jq_value->vval.v_dict;
		    ܐܢ (d == عدم)
			اذهب nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ܐܢ (dict_has_key(d, "method"))
			اذهب nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    ܐܢ (di == عدم)
			اذهب nextitem;
		    tv = &di->di_tv;
		}
		ܐܚܪܢܐ
		    tv = item->jq_value;
	    }
	    ܐܚܪܢܐ
	    {
		ܐܢ (id > 0)
		{
		    ܐܢ (item->jq_value->v_type != VAR_DICT)
			اذهب nextitem;
		    d = item->jq_value->vval.v_dict;
		    ܐܢ (d == عدم)
			اذهب nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    ܐܢ (di == عدم)
			اذهب nextitem;
		    tv = &di->di_tv;
		}
		ܐܚܪܢܐ
		    tv = item->jq_value;
	    }
	}

	ܐܢ ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_NUMBER && tv->vval.v_number == id)
	      || (id <= 0 && (tv->v_type != VAR_NUMBER
		 || tv->vval.v_number == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_number)))))
	{
	    *rettv = item->jq_value;
	    ܐܢ (tv->v_type == VAR_NUMBER)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_number);
	    remove_json_node(head, item);
	    ܦܢܐ OK;
	}
nextitem:
	item = item->jq_next;
    }
    ܦܢܐ FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ساكن فراغ
channel_push_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    ܐܢ (head->jq_prev != عدم && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = عدم;
    ܐܚܪܢܐ ܟܡܐ (item != عدم && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    ܐܢ (newitem == عدم)
    {
	clear_tv(rettv);
	ܦܢܐ;
    }

    newitem->jq_value = alloc_tv();
    ܐܢ (newitem->jq_value == عدم)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ܦܢܐ;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_value = *rettv;
    ܐܢ (item == عدم)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = عدم;
	ܐܢ (newitem->jq_prev == عدم)
	    head->jq_next = newitem;
	ܐܚܪܢܐ
	    newitem->jq_prev->jq_next = newitem;
    }
    ܐܚܪܢܐ
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	ܐܢ (newitem->jq_next == عدم)
	    head->jq_prev = newitem;
	ܐܚܪܢܐ
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    ساكن فراغ
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    صحيح	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    ܐܢ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ܐܢ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	ܦܢܐ;
    }
    arg = argv[1].vval.v_string;
    ܐܢ (arg == عدم)
	arg = (char_u *)"";

    ܐܢ (STRCMP(cmd, "ex") == 0)
    {
	صحيح	called_emsg_before = called_emsg;
	char_u	*p = arg;
	صحيح	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (حرف *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ܐܢ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ܐܢ (do_emsg_silent)
	    --emsg_silent;
	ܐܢ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (حرف *)get_vim_var_str(VV_ERRMSG));
    }
    ܐܚܪܢܐ ܐܢ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (حرف *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_count = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    ܐܚܪܢܐ ܐܢ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    ܐܚܪܢܐ ܐܢ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	صحيح is_call = cmd[0] == 'c';
	صحيح id_idx = is_call ? 3 : 2;

	ܐܢ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_NUMBER)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ܐܢ (p_verbose > 2)
		emsg(_(e_last_argument_for_expr_call_must_be_number));
	}
	ܐܚܪܢܐ ܐܢ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ܐܢ (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	ܐܚܪܢܐ
	{
	    typval_T	*tv = عدم;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = عدم;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ܐܢ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (حرف *)arg);
		tv = eval_expr(arg, عدم);
	    }
	    ܐܚܪܢܐ
	    {
		ch_log(channel, "Calling '%s'", (حرف *)arg);
		ܐܢ (func_call(arg, &argv[2], عدم, عدم, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ܐܢ (argv[id_idx].v_type == VAR_NUMBER)
	    {
		صحيح id = argv[id_idx].vval.v_number;

		ܐܢ (tv != عدم)
		    json = json_encode_nr_expr(id, tv, options | JSON_NL);
		ܐܢ (tv == عدم || (json != عدم && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_expr(id, &err_tv, options | JSON_NL);
		}
		ܐܢ (json != عدم)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (صحيح)STRLEN(json), (حرف *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ܐܢ (tv == &res_tv)
		clear_tv(tv);
	    ܐܚܪܢܐ
		free_tv(tv);
	}
    }
    ܐܚܪܢܐ ܐܢ (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (حرف *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    ساكن فراغ
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (حرف *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_node(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    ساكن فراغ
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
    صحيح		save_p_ma = buffer->b_p_ma;
    صحيح		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    ܐܢ (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ܐܢ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ܦܢܐ;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ܐܢ (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (صحيح)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    ܐܢ (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ܦܢܐ;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    ܐܢ (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, msg, TRUE);
	lnum = 0;
    }
    ܐܚܪܢܐ
	ml_append(lnum, msg, 0, FALSE);
    appended_lines_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ܐܢ (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    ܐܚܪܢܐ
	buffer->b_p_ma = save_p_ma;

    ܐܢ (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ܐܢ (wp->w_buffer == buffer)
	    {
		صحيح move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ܐܢ (move_cursor || wp->w_topline > buffer->b_ml.ml_line_count)
		{
		    win_T *save_curwin = curwin;

		    ܐܢ (move_cursor)
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

    ܐܢ (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    ܐܢ (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_line_count;
	}
    }
}

    ساكن فراغ
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *msg;

    ܟܡܐ ((msg = channel_get(channel, part, عدم)) != عدم)
    {
	ch_log(channel, "Dropping message '%s'", (حرف *)msg);
	vim_free(msg);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ساكن صحيح
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    ܦܢܐ ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ساكن صحيح
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*msg = عدم;
    blob_T	*blob = عدم;
    typval_T	*listtv = عدم;
    typval_T	argv[CH_JSON_MAX_ARGS];
    صحيح		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = عدم;
    buf_T	*buffer = عدم;
    char_u	*p;
    صحيح		called_otc;		// one time callbackup
    صحيح		raw_len = 0;

    ܐܢ (channel->ch_nb_close_cb != عدم
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ܦܢܐ FALSE;

    // Use a message-specific callback, part callback or channel callback
    لكل (cbitem = cbhead->cq_next; cbitem != عدم; cbitem = cbitem->cq_next)
	ܐܢ (cbitem->cq_seq_nr == 0)
	    اكسر;
    ܐܢ (cbitem != عدم)
	callback = &cbitem->cq_callback;
    ܐܚܪܢܐ ܐܢ (ch_part->ch_callback.cb_name != عدم)
	callback = &ch_part->ch_callback;
    ܐܚܪܢܐ ܐܢ (channel->ch_callback.cb_name != عدم)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    ܐܢ (buffer != عدم && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == عدم))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = عدم;
	buffer = عدم;
    }

    ܐܢ (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	صحيح		argc = 0;

	// Get any json message in the queue.
	ܐܢ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ܐܢ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(فراغ)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ܐܢ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		ܦܢܐ FALSE;
	}

	ܐܢ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ܐܢ (d != عدم)
	    {
		ܐܢ (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		ܐܚܪܢܐ
		    di = dict_find(d, (char_u *)"request_seq", -1);
		ܐܢ (di != عدم && di->di_tv.v_type == VAR_NUMBER)
		    seq_nr = di->di_tv.vval.v_number;
	    }

	    argv[1] = *listtv;
	}
	ܐܚܪܢܐ
	{
	    لكل (item = listtv->vval.v_list->lv_first;
		    item != عدم && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    ܟܡܐ (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ܐܢ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ܦܢܐ TRUE;
	    }

	    ܐܢ (argv[0].v_type != VAR_NUMBER)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ܦܢܐ FALSE;
	    }
	    seq_nr = argv[0].vval.v_number;
	}
    }
    ܐܚܪܢܐ ܐܢ (channel_peek(channel, part) == عدم)
    {
	// nothing to read on RAW or NL channel
	ܦܢܐ FALSE;
    }
    ܐܚܪܢܐ
    {
	// If there is no callback or buffer drop the message.
	ܐܢ (callback == عدم && buffer == عدم)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ܐܢ (channel->ch_close_cb.cb_name == عدم && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ܦܢܐ FALSE;
	}

	ܐܢ (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = عدم;
	    char_u  *buf;
	    readq_T *node;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ܟܡܐ (TRUE)
	    {
		node = channel_peek(channel, part);
		nl = channel_first_nl(node);
		ܐܢ (nl != عدم)
		    اكسر;
		ܐܢ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ܐܢ (ch_part->ch_fd == INVALID_FD && node->rq_buflen > 0)
			اكسر;
		    ܦܢܐ FALSE; // incomplete message
		}
	    }
	    buf = node->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    لكل (p = buf; (nl == عدم || p < nl)
					    && p < buf + node->rq_buflen; ++p)
		ܐܢ (*p == NUL)
		    *p = NL;

	    ܐܢ (nl == عدم)
	    {
		// get the whole buffer, drop the NL
		msg = channel_get(channel, part, عدم);
	    }
	    ܐܚܪܢܐ ܐܢ (nl + 1 == buf + node->rq_buflen)
	    {
		// get the whole buffer
		msg = channel_get(channel, part, عدم);
		*nl = NUL;
	    }
	    ܐܚܪܢܐ
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		msg = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (صحيح)(nl - buf) + 1);
	    }
	}
	ܐܚܪܢܐ
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    msg = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : عدم);
	}

	ܐܢ (msg == عدم)
	    ܦܢܐ FALSE; // out of memory (and avoids Coverity warning)

	ܐܢ (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    ܐܢ (blob == عدم)
	    {
		vim_free(msg);
		ܦܢܐ FALSE;
	    }
	    ܐܢ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(msg);
		ܦܢܐ FALSE;
	    }
	    ܐܢ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, msg, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	ܐܚܪܢܐ
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = msg;
	}
    }

    called_otc = FALSE;
    ܐܢ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	صحيح lsp_req_msg = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ܐܢ ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_key(argv[1].vval.v_dict, "method"))
	    lsp_req_msg = TRUE;

	ܐܢ (!lsp_req_msg)
	{
	    لكل (cbitem = cbhead->cq_next; cbitem != عدم;
		    cbitem = cbitem->cq_next)
	    {
		ܐܢ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    اكسر;
		}
	    }
	}
    }

    ܐܢ (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	ܐܢ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ܐܢ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_push_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
		listtv = عدم;
	    }
	    ܐܚܪܢܐ
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    ܐܚܪܢܐ ܐܢ (callback != عدم || buffer != عدم)
    {
	ܐܢ (buffer != عدم)
	{
	    ܐܢ (msg == عدم)
		// JSON or JS mode: re-encode the message.
		msg = json_encode(listtv, ch_mode);
	    ܐܢ (msg != عدم)
	    {
#ifdef FEAT_TERMINAL
		ܐܢ (buffer->b_term != عدم)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ܐܢ (part == PART_ERR)
		    {
			char_u *cp;
			صحيح    extra = 0;

			لكل (cp = msg; *cp != NUL; ++cp)
			    ܐܢ (*cp == NL && (cp == msg || cp[-1] != CAR))
				++extra;
			ܐܢ (extra > 0)
			{
			    char_u *crlf_msg = alloc(STRLEN(msg) + extra + 1);

			    ܐܢ (crlf_msg != عدم)
			    {
				char_u *q = crlf_msg;

				لكل (cp = msg; *cp != NUL; ++cp)
				{
				    ܐܢ (*cp == NL && (cp == msg || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_msg, channel);
				vim_free(crlf_msg);
			    }
			}
			ܐܚܪܢܐ
			    write_to_term(buffer, msg, channel);
		    }
		    ܐܚܪܢܐ
			write_to_term(buffer, msg, channel);
		}
		ܐܚܪܢܐ
#endif
		    append_to_buffer(buffer, msg, channel, part);
	    }
	}

	ܐܢ (callback != عدم)
	{
	    ܐܢ (cbitem != عدم)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    ܐܚܪܢܐ
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (حرف *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    ܐܚܪܢܐ
	ch_log(channel, "Dropping message %d", seq_nr);

    ܐܢ (listtv != عدم)
	free_tv(listtv);
    ܐܢ (blob != عدم)
	blob_unref(blob);
    vim_free(msg);

    ܦܢܐ TRUE;
}

#ܐܢ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    صحيح
channel_can_write_to(channel_T *channel)
{
    ܦܢܐ channel != عدم && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    صحيح
channel_is_open(channel_T *channel)
{
    ܦܢܐ channel != عدم && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ساكن فراغ *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    ܐܢ (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	ܐܢ (head->jq_next == عدم)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ܦܢܐ head->jq_next;
    }
    ܦܢܐ channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ساكن صحيح
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ܦܢܐ channel_readahead_pointer(channel, part) != عدم;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ساكن حرف *
channel_status(channel_T *channel, صحيح req_part)
{
    ch_part_T part;
    صحيح has_readahead = FALSE;

    ܐܢ (channel == عدم)
	 ܦܢܐ "fail";
    ܐܢ (req_part == PART_OUT)
    {
	ܐܢ (channel->CH_OUT_FD != INVALID_FD)
	    ܦܢܐ "open";
	ܐܢ (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    ܐܚܪܢܐ ܐܢ (req_part == PART_ERR)
    {
	ܐܢ (channel->CH_ERR_FD != INVALID_FD)
	    ܦܢܐ "open";
	ܐܢ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    ܐܚܪܢܐ
    {
	ܐܢ (channel_is_open(channel))
	    ܦܢܐ "open";
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	    ܐܢ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		اكسر;
	    }
    }

    ܐܢ (has_readahead)
	ܦܢܐ "buffered";
    ܦܢܐ "closed";
}

    ساكن فراغ
channel_part_info(channel_T *channel, dict_T *dict, حرف *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    حرف	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    ܐܢ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    ܐܚܪܢܐ ܐܢ (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    ܐܚܪܢܐ
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (صحيح)s.length);

    STRCPY(namebuf + tail, "mode");
    بدل (chanpart->ch_mode)
    {
	حالة CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    اكسر;
	حالة CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    اكسر;
	حالة CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    اكسر;
	حالة CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    اكسر;
	حالة CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    اكسر;
	حالة CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    اكسر;
	حالة CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    اكسر;
	افتراضي:
	    STR_LITERAL_SET(s, "");
	    اكسر;
    }
    dict_add_string_len(dict, namebuf, s.string, (صحيح)s.length);

    STRCPY(namebuf + tail, "io");
    ܐܢ (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    ܐܚܪܢܐ بدل (chanpart->ch_io)
    {
	حالة JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    اكسر;
	حالة JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    اكسر;
	حالة JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    اكسر;
	حالة JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    اكسر;
	حالة JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    اكسر;
	افتراضي:
	    STR_LITERAL_SET(s, "");
	    اكسر;
    }
    dict_add_string_len(dict, namebuf, s.string, (صحيح)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_number(dict, namebuf, chanpart->ch_timeout);
}

    ساكن فراغ
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_number(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    ܐܢ (channel->ch_hostname != عدم)
    {
	ܐܢ (channel->ch_port)
	    dict_add_number(dict, "port", channel->ch_port);
	ܐܚܪܢܐ
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    ܐܚܪܢܐ
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
    فراغ
channel_close(channel_T *channel, صحيح invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    ܐܢ (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	ܐܢ (channel->ch_close_cb.cb_name != عدم)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	{
	    ܐܢ (channel->ch_close_cb.cb_name != عدم
			    || channel->ch_part[part].ch_bufref.br_buf != عدم)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ܐܢ (channel->ch_close_cb.cb_name == عدم)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		ܟܡܐ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ܐܢ (channel->ch_close_cb.cb_name != عدم)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (حرف *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      ܐܢ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ܐܢ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  لكل (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = عدم;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = عدم;
    channel->ch_ss_close_cb = عدم;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    ساكن فراغ
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    ساكن فراغ
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    ܐܢ (wq->wq_next == عدم)
	wq->wq_prev = عدم;
    ܐܚܪܢܐ
	wq->wq_next->wq_prev = عدم;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ساكن فراغ
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    ܟܡܐ (channel_peek(channel, part) != عدم)
	vim_free(channel_get(channel, part, عدم));

    ܟܡܐ (cb_head->cq_next != عدم)
    {
	cbq_T *node = cb_head->cq_next;

	remove_cb_node(cb_head, node);
	free_callback(&node->cq_callback);
	vim_free(node);
    }

    ܟܡܐ (json_head->jq_next != عدم)
    {
	free_tv(json_head->jq_next->jq_value);
	remove_json_node(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    ܟܡܐ (ch_part->ch_writeque.wq_next != عدم)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    فراغ
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

#ܐܢ defined(EXITFREE)
    فراغ
channel_free_all(فراغ)
{
    channel_T *channel;

    ch_log(عدم, "channel_free_all()");
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
    ساكن صحيح
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    ܐܢ (in_part->ch_writeque.wq_next != عدم)
	ܦܢܐ TRUE;
    ܐܢ (buf == عدم)
	ܦܢܐ FALSE;
    ܦܢܐ in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_line_count)
	    : (in_part->ch_buf_top <= in_part->ch_buf_bot
			    && in_part->ch_buf_top <= buf->b_ml.ml_line_count);
}

#ܐܢ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ساكن صحيح
channel_fill_wfds(صحيح maxfd_arg, fd_set *wfds)
{
    صحيح		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ܐܢ (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (صحيح)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((صحيح)in_part->ch_fd, wfds);
	    ܐܢ ((صحيح)in_part->ch_fd >= maxfd)
		maxfd = (صحيح)in_part->ch_fd + 1;
	}
    }
    ܦܢܐ maxfd;
}
#ܐܚܪܢܐ
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ساكن صحيح
channel_fill_poll_write(صحيح nfd_in, بنية pollfd *fds)
{
    صحيح		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ܐܢ (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	ܐܚܪܢܐ
	    in_part->ch_poll_idx = -1;
    }
    ܦܢܐ nfd;
}
#endif

تعريفنوع تعداد {
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
    ساكن channel_wait_result
channel_wait(channel_T *channel, sock_T fd, صحيح timeout)
{
    ܐܢ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    ܐܢ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	صحيح	sleep_time;
	DWORD	deadline = GetTickCount() + timeout;
	صحيح	delay = 1;

	// reading from a pipe, not a socket
	ܟܡܐ (TRUE)
	{
	    صحيح r = PeekNamedPipe((HANDLE)fd, عدم, 0, عدم, &nread, عدم);

	    ܐܢ (r && nread > 0)
		ܦܢܐ CW_READY;

	    ܐܢ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, عدم);
	    }
	    ܐܚܪܢܐ ܐܢ (r == 0)
		ܦܢܐ CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_lines();

	    sleep_time = deadline - GetTickCount();
	    ܐܢ (sleep_time <= 0)
		اكسر;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ܐܢ (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    ܐܢ (delay > 10)
		delay = 10;
	}
    }
    ܐܚܪܢܐ
#endif
    {
#ܐܢ defined(HAVE_SELECT)
	بنية timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	صحيح		ret;
	صحيح		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	لكل (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((صحيح)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (صحيح)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, عدم, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ܐܢ (ret == -1 && errno == EINTR)
		استمر;
# endif
	    ܐܢ (ret > 0)
	    {
		ܐܢ (FD_ISSET(fd, &rfds))
		    ܦܢܐ CW_READY;
		channel_write_any_lines();
		استمر;
	    }
	    اكسر;
	}
#ܐܚܪܢܐ
	لكل (;;)
	{
	    بنية pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    صحيح		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ܐܢ (poll(fds, nfd, timeout) > 0)
	    {
		ܐܢ (fds[0].revents & POLLIN)
		    ܦܢܐ CW_READY;
		channel_write_any_lines();
		استمر;
	    }
	    اكسر;
	}
#endif
    }
    ܦܢܐ CW_NOT_READY;
}

    ساكن فراغ
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, صحيح is_err, حرف *func)
{
    حرف	msg[] = "%s(): Read %s from ch_part[%d], closing";

    ܐܢ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, msg, func, "error", part);
    ܐܚܪܢܐ
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
    ܐܢ (channel->ch_nb_close_cb != عدم)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_MSG_RAW,
			      (صحيح)STRLEN_LITERAL(DETACH_MSG_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ܐܢ (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	ܐܢ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    ساكن فراغ
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    ܐܢ (channel->ch_nb_close_cb != عدم)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    ܐܢ (channel->ch_ss_close_cb != عدم)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    ساكن فراغ
channel_read(channel_T *channel, ch_part_T part, حرف *func)
{
    ساكن char_u	*buf = عدم;
    صحيح			len = 0;
    صحيح			readlen = 0;
    sock_T		fd;
    صحيح			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ܐܢ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	ܦܢܐ;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ܐܢ (buf == عدم)
    {
	buf = alloc(MAXMSGSIZE);
	ܐܢ (buf == عدم)
	    ܦܢܐ;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    لكل (;;)
    {
	ܐܢ (channel_wait(channel, fd, 0) != CW_READY)
	    اكسر;
	ܐܢ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    بنية sockaddr_storage	client;

	    newchannel = add_channel();
	    ܐܢ (newchannel == عدم)
	    {
		ch_error(عدم, "Cannot allocate channel.");
		ܦܢܐ;
	    }
	    socklen = حجم(client);
	    newfd = accept(fd, (بنية sockaddr*)&client, &socklen);
	    ܐܢ (newfd < 0)
	    {
		ch_error(عدم, "Cannot accept channel.");
		channel_free(newchannel);
		ܦܢܐ;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    ܐܢ (channel->ch_ss_accept_cb != عدم)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ܦܢܐ;
	    }
#endif

	    ܐܢ (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		حرف addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((بنية sockaddr_in*)&client)->sin_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
			addr,
			ntohs(((بنية sockaddr_in*)&client)->sin_port));
#ܐܚܪܢܐ
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
		    inet_ntoa(((بنية sockaddr_in*)&client)->sin_addr),
		    ntohs(((بنية sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    ܐܚܪܢܐ ܐܢ (client.ss_family == AF_INET6)
	    {
		حرف addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((بنية sockaddr_in6*)&client)->sin6_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "[%s]:%d",
			addr,
			ntohs(((بنية sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    ܐܚܪܢܐ ܐܢ (client.ss_family == AF_UNIX)
		vim_snprintf((حرف *)namebuf, حجم(namebuf),
							    "unix:anonymous");
	    ܐܚܪܢܐ
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    ܦܢܐ;
	}
	ܐܢ (use_socket)
	    len = sock_read(fd, (حرف *)buf, MAXMSGSIZE);
	ܐܚܪܢܐ
	    len = fd_read(fd, (حرف *)buf, MAXMSGSIZE);
	ܐܢ (len <= 0)
	    اكسر;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ܐܢ (readlen <= 0)
    {
	ܐܢ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ܐܢ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    ܐܚܪܢܐ ܐܢ (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    فراغ
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
    ساكن char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, صحيح timeout, صحيح raw, صحيح *outlen)
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

    ܟܡܐ (TRUE)
    {
	node = channel_peek(channel, part);
	ܐܢ (node != عدم)
	{
	    ܐܢ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(node) != عدم))
		// got a complete message
		اكسر;
	    ܐܢ (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		استمر;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ܐܢ (raw || fd == INVALID_FD)
		اكسر;
	}

	// Wait for up to the channel timeout.
	ܐܢ (fd == INVALID_FD)
	    ܦܢܐ عدم;
	ܐܢ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ܦܢܐ عدم;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ܐܢ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != عدم)
    {
	msg = channel_get_all(channel, part, outlen);
    }
    ܐܚܪܢܐ
    {
	char_u *p;

	buf = node->rq_buffer;
	nl = channel_first_nl(node);

	// Convert NUL to NL, the internal representation.
	لكل (p = buf; (nl == عدم || p < nl) && p < buf + node->rq_buflen; ++p)
	    ܐܢ (*p == NUL)
		*p = NL;

	ܐܢ (nl == عدم)
	{
	    // must be a closed channel with missing NL
	    msg = channel_get(channel, part, عدم);
	}
	ܐܚܪܢܐ ܐܢ (nl + 1 == buf + node->rq_buflen)
	{
	    // get the whole buffer
	    msg = channel_get(channel, part, عدم);
	    *nl = NUL;
	}
	ܐܚܪܢܐ
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    msg = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (صحيح)(nl - buf) + 1);
	}
    }
    ܐܢ (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != عدم ? *outlen : (صحيح)STRLEN(msg));
    ܦܢܐ msg;
}

ساكن صحيح channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    صحيح
channel_in_blocking_wait(فراغ)
{
    ܦܢܐ channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ساكن فراغ
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, صحيح timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (فراغ)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#ܐܚܪܢܐ
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
    ساكن صحيح
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	صحيح	    timeout_arg,
	صحيح	    id,
	typval_T    **rettv)
{
    صحيح		more;
    sock_T	fd;
    صحيح		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    صحيح		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ܐܢ (id >= 0)
	channel_add_block_id(chanpart, id);

    لكل (;;)
    {
	ܐܢ (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (فراغ)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	ܐܢ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    اكسر;
	}

	ܐܢ (!more)
	{
	    فراغ *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    فراغ *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ܐܢ (channel_parse_messages())
		استمر;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    ܐܢ (readahead_ptr != عدم && readahead_ptr != prev_readahead_ptr)
		استمر;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (صحيح)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    ܐܢ (timeout <= 0 || fd == INVALID_FD)
	    {
		ܐܢ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		اكسر;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ܐܢ (chanpart->ch_wait_len > 0)
	    {
		صحيح	left;
#ifdef MSWIN
		left = chanpart->ch_deadline - GetTickCount() + 1;
#ܐܚܪܢܐ
		{
		    بنية timeval now_tv;

		    gettimeofday(&now_tv, عدم);
		    left = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		ܐܢ (left < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		ܐܚܪܢܐ ܐܢ (left < timeout)
		    timeout = left;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    ܐܢ (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    ܦܢܐ retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, صحيح check_open, صحيح reading, ch_part_T part)
{
    channel_T	*channel = عدم;
    صحيح		has_readahead = FALSE;

    ܐܢ (tv->v_type == VAR_JOB)
    {
	ܐܢ (tv->vval.v_job != عدم)
	    channel = tv->vval.v_job->jv_channel;
    }
    ܐܚܪܢܐ ܐܢ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    ܐܚܪܢܐ
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ܦܢܐ عدم;
    }
    ܐܢ (channel != عدم && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_COUNT ? part : channel_part_read(channel));

    ܐܢ (check_open && (channel == عدم || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ܦܢܐ عدم;
    }
    ܦܢܐ channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ساكن فراغ
common_channel_read(typval_T *argvars, typval_T *rettv, صحيح raw, صحيح blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_COUNT;
    jobopt_T	opt;
    صحيح		mode;
    صحيح		timeout;
    صحيح		id = -1;
    typval_T	*listtv = عدم;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    ܐܢ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    clear_job_options(&opt);
    ܐܢ (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	اذهب theend;

    ܐܢ (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ܐܢ (channel == عدم)
	اذهب theend;

    ܐܢ (part == PART_COUNT)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    ܐܢ (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ܐܢ (blob || mode == CH_MODE_BLOB)
    {
	صحيح	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	ܐܢ (p != عدم)
	{
	    blob_T	*b = blob_alloc();

	    ܐܢ (b != عدم)
	    {
		b->bv_ga.ga_len = outlen;
		ܐܢ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		ܐܚܪܢܐ
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    ܐܚܪܢܐ ܐܢ (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, عدم);
    ܐܚܪܢܐ
    {
	ܐܢ (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	ܐܢ (listtv != عدم)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	ܐܚܪܢܐ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_number = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#ܐܢ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    فراغ
channel_handle_events(صحيح only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ܐܢ (only_keep_open && !channel->ch_keep_open)
	    استمر;

	// check the socket and pipes
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ܐܢ (fd == INVALID_FD)
		استمر;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    صحيح r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    ܐܢ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    ܐܚܪܢܐ ܐܢ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ܐܢ (channel->ch_job != عدم)
	{
	    job_T *job = channel->ch_job;

	    ܐܢ (job->jv_tty_out != عدم && job->jv_status == JOB_FINISHED)
		لكل (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#ܐܢ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    صحيح
channel_any_keep_open(فراغ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ܐܢ (channel->ch_keep_open)
	    ܦܢܐ TRUE;
    ܦܢܐ FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    فراغ
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    صحيح		fd = ch_part->ch_fd;

    ܐܢ (fd == INVALID_FD)
	ܦܢܐ;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#ܐܚܪܢܐ
    (فراغ)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    صحيح
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	صحيح	  len_arg,
	حرف	  *fun)
{
    صحيح		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    صحيح		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    ܐܢ (fd == INVALID_FD)
    {
	ܐܢ (!channel->ch_error && fun != عدم)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ܦܢܐ FAIL;
    }

    ܐܢ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    ܐܢ (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_msg = 0;
    }

    لكل (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	صحيح	    len;

	ܐܢ (wq->wq_next != عدم)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	ܐܚܪܢܐ
	{
	    ܐܢ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ܦܢܐ OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ܐܢ (part == PART_SOCK)
	    res = sock_write(fd, (حرف *)buf, len);
	ܐܚܪܢܐ
	{
	    res = fd_write(fd, (حرف *)buf, len);
#ifdef MSWIN
	    ܐܢ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, عدم);
	    }
#endif
	}
	ܐܢ (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	ܐܢ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    ܐܢ (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    ܐܢ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ܐܢ (entry != عدم)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    استمر;
		}
		ܐܢ (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    ܐܚܪܢܐ
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ܐܢ (entry != عدم)
		{
		    ܐܢ (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (حرف *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		ܐܚܪܢܐ
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ܐܢ (wq->wq_prev != عدم
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    ܐܢ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((حرف *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		ܐܚܪܢܐ
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    ܐܢ (last != عدم)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = عدم;
			ܐܢ (wq->wq_prev == عدم)
			    wq->wq_next = last;
			ܐܚܪܢܐ
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			ܐܢ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	ܐܚܪܢܐ ܐܢ (res != len)
	{
	    ܐܢ (!channel->ch_error && fun != عدم)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ܦܢܐ FAIL;
	}

	channel->ch_error = FALSE;
	ܦܢܐ OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    ساكن channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	صحيح	    len,
	صحيح	    id,
	صحيح	    eval,
	jobopt_T    *opt,
	حرف	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ܐܢ (channel == عدم)
	ܦܢܐ عدم;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ܐܢ (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ܦܢܐ عدم;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ܐܢ (opt->jo_callback.cb_name != عدم && *opt->jo_callback.cb_name != NUL)
    {
	ܐܢ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ܦܢܐ عدم;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ܐܢ (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == عدم)
	ܦܢܐ channel;
    ܦܢܐ عدم;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ساكن فراغ
ch_expr_common(typval_T *argvars, typval_T *rettv, صحيح eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    صحيح		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحيح		timeout;
    صحيح		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    ܐܢ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ܐܢ (channel == عدم)
	ܦܢܐ;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    ܐܢ (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ܦܢܐ;
    }

    ܐܢ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	حرف		*key = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	ܐܢ (rettv_dict_alloc(rettv) == FAIL)
	    ܦܢܐ;

	ܐܢ (check_for_dict_arg(argvars, 1) == FAIL)
	    ܦܢܐ;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)key, -1);
	ܐܢ (argvars[2].v_type == VAR_DICT
		&& dict_has_key(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ܐܢ (di != عدم && di->di_tv.v_type != VAR_NUMBER
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_value_for_argument_str), key);
	    ܦܢܐ;
	}

	ܐܢ (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_msg_id;
	    ܐܢ (di == عدم)
		dict_add_number(d, "seq", id);
	    ܐܚܪܢܐ
		di->di_tv.vval.v_number = id;
	}
	ܐܚܪܢܐ ܐܢ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_msg_id;
	    ܐܢ (di == عدم)
		dict_add_number(d, "id", id);
	    ܐܚܪܢܐ
		di->di_tv.vval.v_number = id;
	}
	ܐܚܪܢܐ
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ܐܢ (di != عدم && di->di_tv.v_type == VAR_NUMBER)
		id = di->di_tv.vval.v_number;
	}
	ܐܢ (ch_mode == CH_MODE_LSP && !dict_has_key(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_msg(&argvars[1]);
    }
    ܐܚܪܢܐ
    {
	id = ++channel->ch_last_msg_id;
	text = json_encode_nr_expr(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    ܐܢ (text == عدم)
	ܦܢܐ;

    channel = send_common(argvars, text, (صحيح)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    ܐܢ (channel != عدم && eval)
    {
	ܐܢ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ܐܚܪܢܐ
	    timeout = channel_get_timeout(channel, part_read);
	ܐܢ (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ܐܢ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_NUMBER;
		free_tv(listtv);
	    }
	    ܐܚܪܢܐ
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
    ܐܢ (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ܐܢ (rettv->vval.v_dict != عدم)
	    dict_add_number(rettv->vval.v_dict, "seq", id);
    }
    ܐܚܪܢܐ ܐܢ (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ܐܢ (rettv->vval.v_dict != عدم)
	    dict_add_number(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ساكن فراغ
ch_raw_common(typval_T *argvars, typval_T *rettv, صحيح eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    صحيح		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحيح		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    ܐܢ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ܦܢܐ;

    ܐܢ (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    ܐܚܪܢܐ
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (صحيح)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ܐܢ (channel != عدم && eval)
    {
	ܐܢ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ܐܚܪܢܐ
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, عدم);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#ܐܢ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحيح
channel_poll_setup(صحيح nfd_in, فراغ *fds_in, صحيح *towait)
{
    صحيح		nfd = nfd_in;
    channel_T	*channel;
    بنية	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ܐܢ (ch_part->ch_fd != INVALID_FD)
	    {
		ܐܢ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ܐܢ (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		ܐܚܪܢܐ
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    ܐܚܪܢܐ
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    ܦܢܐ nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحيح
channel_poll_check(صحيح ret_in, فراغ *fds_in)
{
    صحيح		ret = ret_in;
    channel_T	*channel;
    بنية	pollfd *fds = fds_in;
    ch_part_T	part;
    صحيح		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ܐܢ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    ܐܚܪܢܐ ܐܢ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	ܐܢ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_input(channel);
	    --ret;
	}
    }

    ܦܢܐ ret;
}
#endif // UNIX && !HAVE_SELECT

#ܐܢ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحيح
channel_select_setup(
	صحيح maxfd_in,
	فراغ *rfds_in,
	فراغ *wfds_in,
	بنية timeval *tv,
	بنية timeval **tvp)
{
    صحيح		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ܐܢ (fd != INVALID_FD)
	    {
		ܐܢ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ܐܢ (*tvp == عدم || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		ܐܚܪܢܐ
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ܐܢ ((صحيح)fd >= FD_SETSIZE)
			استمر;
# endif
		    FD_SET((صحيح)fd, rfds);
		    ܐܢ (maxfd < (صحيح)fd)
			maxfd = (صحيح)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ܦܢܐ maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحيح
channel_select_check(صحيح ret_in, فراغ *rfds_in, فراغ *wfds_in)
{
    صحيح		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ܐܢ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحيح)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    ܐܚܪܢܐ ܐܢ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	ܐܢ (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحيح)in_part->ch_fd < FD_SETSIZE
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
	ܐܢ (channel->ch_job != عدم)
	{
	    job_T *job = channel->ch_job;

	    ܐܢ (job->jv_tty_out != عدم && job->jv_status == JOB_FINISHED)
		لكل (part = PART_SOCK; part < PART_COUNT; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ܦܢܐ ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ساكن فراغ
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    ܐܢ (stdio_channel->CH_OUT_FD != INVALID_FD)
	(فراغ)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(صحيح)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#وإلاإذا defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    بنية timeval	tv;
    بنية timeval	*tvp = &tv;
    صحيح			maxfd;
    صحيح			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, عدم, tvp);
    ܐܢ (ret >= 0)
	(فراغ)channel_select_check(ret, &rfds, &wfds);
#ܐܚܪܢܐ
    بنية pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    صحيح			nfd;
    صحيح			towait = (صحيح)msec;
    صحيح			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ܐܢ (ret >= 0)
	(فراغ)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    فراغ
channel_stdio_loop(فراغ)
{
    ܟܡܐ (stdio_channel != عدم && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	ܐܢ (due_time > 0 && due_time < msec)
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
    صحيح
channel_parse_messages(فراغ)
{
    channel_T	*channel = first_channel;
    صحيح		ret = FALSE;
    صحيح		r;
    ch_part_T	part = PART_SOCK;
    ساكن صحيح	recursive = 0;
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
    ܐܢ ((did_repeated_msg & REPEATED_MSG_LOOKING) == 0)
    {
	ch_log(عدم, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_msg = REPEATED_MSG_LOOKING;
    }
    ܟܡܐ (channel != عدم)
    {
	ܐܢ (recursive == 1)
	{
	    ܐܢ (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_COUNT);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		استمر;
	    }
	    ܐܢ (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ܐܢ (channel->ch_job != عدم)
		    channel->ch_job->jv_channel = عدم;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		استمر;
	    }
	    ܐܢ (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		استمر;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	ܐܢ (part == PART_OUT
		&& channel->ch_job != عدم
		&& channel->ch_job->jv_tty_out != عدم
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    استمر;
	}
#endif
	ܐܢ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ܐܢ (r == OK)
		ret = TRUE;
	    ܐܢ (channel_unref(channel) || (r == OK
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
		استمر;
	    }
	}
	ܐܢ (part < PART_ERR)
	    ++part;
	ܐܚܪܢܐ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    ܐܢ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    ܦܢܐ ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    صحيح
channel_any_readahead(فراغ)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    ܟܡܐ (channel != عدم)
    {
	ܐܢ (channel_has_readahead(channel, part))
	    ܦܢܐ TRUE;
	ܐܢ (part < PART_ERR)
	    ++part;
	ܐܚܪܢܐ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    ܦܢܐ FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    صحيح
set_ref_in_channel(صحيح copyID)
{
    صحيح		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    لكل (channel = first_channel; !abort && channel != عدم;
						   channel = channel->ch_next)
	ܐܢ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, عدم, عدم, عدم);
	}
    ܦܢܐ abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ساكن ch_part_T
channel_part_send(channel_T *channel)
{
    ܐܢ (channel->CH_SOCK_FD == INVALID_FD)
	ܦܢܐ PART_IN;
    ܦܢܐ PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ساكن ch_part_T
channel_part_read(channel_T *channel)
{
    ܐܢ (channel->CH_SOCK_FD == INVALID_FD)
	ܦܢܐ PART_OUT;
    ܦܢܐ PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ساكن ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    ܐܢ (channel == عدم)
	ܦܢܐ CH_MODE_JSON;
    ܦܢܐ channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ساكن صحيح
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ܦܢܐ channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    فراغ
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_number = 0;
    ܐܢ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ܐܢ (channel != عدم)
	rettv->vval.v_number = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    فراغ
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ܐܢ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ܐܢ (channel != عدم)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    فراغ
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ܐܢ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ܐܢ (channel != عدم)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    فراغ
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_number = -1;

    ܐܢ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ܐܢ (channel == عدم)
	ܦܢܐ;

    char_u	*what = tv_get_string(&argvars[1]);
    صحيح		part;
    ܐܢ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    ܐܚܪܢܐ ܐܢ (STRCMP(what, "out") == 0)
	part = PART_OUT;
    ܐܚܪܢܐ ܐܢ (STRCMP(what, "in") == 0)
	part = PART_IN;
    ܐܚܪܢܐ
	part = PART_SOCK;
    ܐܢ (channel->ch_part[part].ch_bufref.br_buf != عدم)
	rettv->vval.v_number =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    فراغ
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    ܐܢ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ܐܢ (channel == عدم)
	ܦܢܐ;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    ܐܢ (channel->ch_job != عدم)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    فراغ
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ܐܢ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ܐܢ (channel != عدم && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    فراغ
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ܐܢ (check_restricted() || check_secure())
	ܦܢܐ;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    فراغ
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ܐܢ (check_restricted() || check_secure())
	ܦܢܐ;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    فراغ
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    فراغ
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    فراغ
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    فراغ
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_expr_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    فراغ
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_expr_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    فراغ
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    فراغ
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    فراغ
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    ܐܢ (check_restricted() || check_secure())
	ܦܢܐ;

    ܐܢ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ܐܢ (channel == عدم)
	ܦܢܐ;
    clear_job_options(&opt);
    ܐܢ (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    فراغ
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    صحيح		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    ܐܢ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ܦܢܐ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ܐܢ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	ܐܢ (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    حرف      *status = channel_status(channel, -1);

    ܐܢ (channel == عدم)
	vim_snprintf((حرف *)buf, NUMBUFLEN, "channel %s", status);
    ܐܚܪܢܐ
	vim_snprintf((حرف *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    ܦܢܐ buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(صحيح ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ܐܢ (ch->ch_id == ch_id)
	    ܦܢܐ ch;
    ܦܢܐ عدم;
}

#endif // FEAT_JOB_CHANNEL
