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

#شامل "vim.h"

#اگر defined(FEAT_JOB_CHANNEL)

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
تعریفنوع ساختار sockaddr_un {
    ADDRESS_FAMILY sun_family;
    نویسه sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#وگرنه
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

ایستا تهی channel_read(channel_T *channel, ch_part_T part, نویسه *func);
ایستا تهی channel_wait_any(long msec);
ایستا ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
ایستا صحیح channel_get_timeout(channel_T *channel, ch_part_T part);
ایستا channel_T *channel_open_stdio(تهی);
ایستا ch_part_T channel_part_send(channel_T *channel);
ایستا ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    برای ((ch) = first_channel; (ch) != پوچ; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ایستا صحیح safe_to_invoke_callback = 0;

#ifdef MSWIN
    ایستا صحیح
fd_read(sock_T fd, نویسه *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    اگر (!ReadFile(h, buf, (DWORD)len, &nread, پوچ))
	بازگشت -1;
    بازگشت (صحیح)nread;
}

    ایستا صحیح
fd_write(sock_T fd, نویسه *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    وقتی (todo > 0)
    {
	اگر (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	وگرنه
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	اگر (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    اگر (err != ERROR_IO_PENDING)
		بازگشت -1;
	    اگر (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		بازگشت -1;
	    FlushFileBuffers(h);
	}
	وگرنه اگر (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    بشکن;
	todo -= nwrite;
	done += nwrite;
    }
    بازگشت (صحیح)done;
}

    ایستا تهی
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(پیام) (تهی)semsg("%s: %s", پیام, strerror_win32(errno))

    ایستا نویسه *
strerror_win32(صحیح eno)
{
    ایستا LPVOID msgbuf = پوچ;
    char_u *ptr;

    اگر (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = پوچ;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	پوچ,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	پوچ);
    اگر (msgbuf != پوچ)
	// chomp \r or \n
	برای (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    انتخاب (*ptr)
	    {
		حالت '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    بشکن;
		حالت '\n':
		    اگر (*(ptr + 1) == '\0')
			*ptr = '\0';
		    وگرنه
			*ptr = ' ';
		    بشکن;
	    }
    بازگشت msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
ایستا channel_T *first_channel = پوچ;
ایستا صحیح next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(تهی)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    اگر (channel == پوچ)
	بازگشت پوچ;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    برای (part = PART_SOCK; part < PART_شمار; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_درون_داد_Handler = (Xt_درون_داد_Id)پوچ;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_درون_داد_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    اگر (first_channel != پوچ)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    بازگشت channel;
}

    صحیح
has_any_channel(تهی)
{
    بازگشت first_channel != پوچ;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    صحیح
channel_still_useful(channel_T *channel)
{
    صحیح has_sock_پیام;
    صحیح	has_out_پیام;
    صحیح	has_err_پیام;

    // If the job was killed the channel is not expected to work anymore.
    اگر (channel->ch_job_killed && channel->ch_job == پوچ)
	بازگشت FALSE;

    // If there is a close callback it may still need to be invoked.
    اگر (channel->ch_close_cb.cb_name != پوچ)
	بازگشت TRUE;

    // If reading from or a buffer it's still useful.
    اگر (channel->ch_part[PART_IN].ch_bufref.br_buf != پوچ)
	بازگشت TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_پیام = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != پوچ
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != پوچ;
    has_out_پیام = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != پوچ
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != پوچ;
    has_err_پیام = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != پوچ
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != پوچ;
    بازگشت (channel->ch_callback.cb_name != پوچ && (has_sock_پیام
		|| has_out_پیام || has_err_پیام))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != پوچ
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != پوچ)
		    && has_out_پیام)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != پوچ
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != پوچ)
		    && has_err_پیام);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    صحیح
channel_can_close(channel_T *channel)
{
    بازگشت channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    ایستا تهی
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    ایستا تهی
channel_free_channel(channel_T *channel)
{
    اگر (channel->ch_next != پوچ)
	channel->ch_next->ch_prev = channel->ch_prev;
    اگر (channel->ch_prev == پوچ)
	first_channel = channel->ch_next;
    وگرنه
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    ایستا تهی
channel_free(channel_T *channel)
{
    اگر (in_free_unref_اقلام)
	بازگشت;

    اگر (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    وگرنه
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
    ایستا صحیح
channel_may_free(channel_T *channel)
{
    اگر (!channel_still_useful(channel))
    {
	channel_free(channel);
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    صحیح
channel_unref(channel_T *channel)
{
    اگر (channel != پوچ && --channel->ch_refcount <= 0)
	بازگشت channel_may_free(channel);
    بازگشت FALSE;
}

    صحیح
free_unused_channels_contents(صحیح copyID, صحیح mask)
{
    صحیح		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	اگر (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    بازگشت did_free;
}

    تهی
free_unused_channels(صحیح copyID, صحیح mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    برای (ch = first_channel; ch != پوچ; ch = ch_next)
    {
	ch_next = ch->ch_next;
	اگر (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#اگر defined(FEAT_GUI)

# اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    ایستا channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    اگر (fd == INVALID_FD)
	بازگشت پوچ;

    FOR_ALL_CHANNELS(channel)
    {
	برای (part = PART_SOCK; part < PART_IN; ++part)
	    اگر (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		بازگشت channel;
	    }
    }
    بازگشت پوچ;
}

    ایستا تهی
channel_read_fd(صحیح fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    اگر (channel == پوچ)
	ch_error(پوچ, "Channel for fd %d not found", fd);
    وگرنه
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ایستا تهی
messageFromServerX11(XtPointer clientData,
		  صحیح *unused1 UNUSED,
		  Xt_درون_داد_Id *unused2 UNUSED)
{
    channel_read_fd((صحیح)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  اگر GTK_CHECK_VERSION(3,0,0)
    ایستا gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    بازگشت TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  وگرنه
    ایستا تهی
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_درون_داد_Condition unused2 UNUSED)
{
    channel_read_fd((صحیح)(long)clientData);
}
#  endif
# endif

    ایستا تهی
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    اگر (!CH_HAS_GUI)
	بازگشت;

    // gets stuck in handling events for a not connected channel
    اگر (channel->ch_keep_open)
	بازگشت;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    اگر (channel->ch_part[part].ch_درون_داد_Handler == (Xt_درون_داد_Id)پوچ)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_درون_داد_Handler = Xt_App_Add_درون_داد(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_درون_داد_Read_Mask + Xt_درون_داد_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# وگرنه
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    اگر (channel->ch_part[part].ch_درون_داد_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   اگر GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_درون_داد_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   وگرنه
	channel->ch_part[part].ch_درون_داد_Handler = gdk_درون_داد_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_درون_داد_Condition)
			     ((صحیح)GDK_درون_داد_READ + (صحیح)GDK_درون_داد_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ایستا تهی
channel_gui_register(channel_T *channel)
{
    اگر (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    اگر (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    اگر (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    تهی
channel_gui_register_all(تهی)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    ایستا تهی
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    اگر (channel->ch_part[part].ch_درون_داد_Handler != (Xt_درون_داد_Id)پوچ)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_درون_داد(channel->ch_part[part].ch_درون_داد_Handler);
	channel->ch_part[part].ch_درون_داد_Handler = (Xt_درون_داد_Id)پوچ;
    }
# وگرنه
#  ifdef FEAT_GUI_GTK
    اگر (channel->ch_part[part].ch_درون_داد_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   اگر GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_درون_داد_Handler);
#   وگرنه
	gdk_درون_داد_remove(channel->ch_part[part].ch_درون_داد_Handler);
#   endif
	channel->ch_part[part].ch_درون_داد_Handler = 0;
    }
#  endif
# endif
}

    ایستا تهی
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    برای (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    ایستا صحیح
channel_connect(
	channel_T *channel,
	ثابت ساختار sockaddr *server_addr,
	صحیح server_addrlen,
	صحیح *waittime)
{
    صحیح		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    وقتی (TRUE)
    {
	long	elapsed_msec = 0;
	صحیح	waitnow;
	صحیح	ret;

	اگر (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	اگر (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    بازگشت -1;
	}

	اگر (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    اگر (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#وگرنه
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		بازگشت -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	اگر (ret == 0)
	    // The connection could be established.
	    بشکن;

	SOCK_ERRNO;
	اگر (*waittime < 0 || (errno != EWOULDBLOCK
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
	    بازگشت -1;
	}
	وگرنه اگر (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    بازگشت -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    ساختار timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    صحیح			so_error = 0;
	    socklen_t		so_error_len = اندازه(so_error);
	    ساختار timeval	start_tv;
	    ساختار timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, پوچ);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, پوچ, &tv);
	    اگر (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		بازگشت -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    اگر (FD_ISSET(sd, &wfds))
		بشکن;
	    elapsed_msec = waitnow;
	    اگر (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		ادامه;
	    }
#وگرنه
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    اگر (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		اگر (ret < 0 || (so_error != 0
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
		    بازگشت -1;
		}
		وگرنه اگر (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    بازگشت -1;
		}
	    }

	    اگر (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		بشکن;

	    gettimeofday(&end_tv, پوچ);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	اگر (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    اگر (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    اگر (!got_int)
	    {
		اگر (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		ادامه;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	بازگشت -1;
    }

    اگر (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#وگرنه
	(تهی)fcntl(sd, F_SETFL, 0);
#endif
    }

    بازگشت sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ثابت نویسه *path,
	تهی (*nb_close_cb)(تهی))
{
    channel_T		*channel = پوچ;
    صحیح			sd = -1;
    size_t		path_len = STRLEN(path);
    ساختار sockaddr_un	server;
    size_t		server_len;
    صحیح			waittime = -1;

    اگر (*path == NUL || path_len >= اندازه(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	بازگشت پوچ;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    اگر (channel == پوچ)
    {
	ch_error(پوچ, "Cannot allocate channel.");
	بازگشت پوچ;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, اندازه(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(ساختار sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (ساختار sockaddr *)&server, (صحیح)server_len,
								   &waittime);

    اگر (sd < 0)
    {
	channel_free(channel);
	بازگشت پوچ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (نویسه *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    بازگشت channel;
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
	ثابت نویسه *hostname,
	صحیح port,
	صحیح waittime,
	تهی (*nb_close_cb)(تهی))
{
    صحیح			sd = -1;
    channel_T		*channel = پوچ;
#ifdef FEAT_IPV6
    صحیح			err;
    ساختار addrinfo	hints;
    ساختار addrinfo	*res = پوچ;
    ساختار addrinfo	*addr = پوچ;
#وگرنه
    ساختار sockaddr_in	server;
    ساختار hostent	*host = پوچ;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    اگر (channel == پوچ)
    {
	ch_error(پوچ, "Cannot allocate channel.");
	بازگشت پوچ;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# اگر defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    اگر ((err = getaddrinfo(hostname, پوچ, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	بازگشت پوچ;
    }

    // Count the number of addresses for timeout distribution
    صحیح addr_شمار = 0;
    برای (addr = res; addr != پوچ; addr = addr->ai_next)
	addr_شمار++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    اگر (waittime == 0)
	waittime = 15;

    صحیح addr_index = 0;
    برای (addr = res; addr != پوچ; addr = addr->ai_next, addr_index++)
    {
	ثابت نویسه  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ثابت تهی  *src = پوچ;
	نویسه	    buf[NUMBUFLEN];
# endif
	صحیح	    try_waittime;
	صحیح	    before_waittime;
	صحیح	    consumed;
	صحیح	    remaining_addrs;

	اگر (addr->ai_family == AF_INET6)
	{
	    ساختار sockaddr_in6 *sai = (ساختار sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	وگرنه اگر (addr->ai_family == AF_INET)
	{
	    ساختار sockaddr_in *sai = (ساختار sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	اگر (src != پوچ)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, اندازه(buf));
	    اگر (dst == پوچ)
		dst = hostname;
	    وگرنه اگر (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	اگر (addr->ai_next == پوچ)
	    try_waittime = waittime;
	وگرنه اگر (addr_index == 0)
	{
	    اگر (waittime > 500)
		try_waittime = 250;
	    وگرنه اگر (waittime > 30)
		try_waittime = waittime / 2;
	    وگرنه
		try_waittime = waittime;
	}
	وگرنه
	{
	    remaining_addrs = addr_شمار - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (صحیح)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	اگر (waittime < 0)
	    waittime = 0;

	اگر (sd >= 0)
	    بشکن;

	// If we have no time left, stop trying
	اگر (waittime <= 0 && addr->ai_next != پوچ)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    بشکن;
	}
    }

    freeaddrinfo(res);
#وگرنه
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    اگر ((host = gethostbyname(hostname)) == پوچ)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	بازگشت پوچ;
    }
    {
	نویسه *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], اندازه(p));
	memcpy((نویسه *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    اگر (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (ساختار sockaddr *)&server, اندازه(server),
								   &waittime);
#endif

    اگر (sd < 0)
    {
	channel_free(channel);
	بازگشت پوچ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (نویسه *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    بازگشت channel;
}

    ایستا تهی
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    اگر (callback->cb_name != پوچ && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    وگرنه
	cbp->cb_name = پوچ;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ایستا تهی
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_مقدار_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_مقدار_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    اگر (curbuf->b_ml.ml_mfp == پوچ)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ایستا buf_T *
channel_find_buffer(char_u *name, صحیح err, صحیح پیام)
{
    buf_T *buf = پوچ;
    buf_T *save_curbuf = curbuf;

    اگر (name != پوچ && *name != NUL)
    {
	buf = buflist_findname(name);
	اگر (buf == پوچ)
	    buf = buflist_findname_exp(name);
    }

    اگر (buf != پوچ)
	بازگشت buf;

    buf = buflist_new(name == پوچ || *name == NUL ? پوچ : name,
	    پوچ, (linenr_T)0, BLN_LISTED | BLN_NEW);
    اگر (buf == پوچ)
	بازگشت پوچ;
    prepare_buffer(buf);

    curbuf = buf;
    اگر (پیام)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    بازگشت buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ایستا تهی
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    اگر (opt->jo_set & JO_MODE)
	برای (part = PART_SOCK; part < PART_شمار; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    اگر (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    اگر (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    اگر (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    اگر (opt->jo_set & JO_TIMEOUT)
	برای (part = PART_SOCK; part < PART_شمار; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    اگر (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    اگر (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    اگر (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    اگر (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    اگر (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    اگر (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    اگر (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    اگر ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	اگر (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	اگر (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    اگر (buf == پوچ)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	وگرنه
	{
	    صحیح پیام = TRUE;

	    اگر (opt->jo_set2 & JO2_OUT_پیام)
		پیام = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, پیام);
	}
	اگر (buf != پوچ)
	{
	    اگر (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    اگر (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    وگرنه
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (نویسه *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		اگر (buf->b_ml.ml_mfp == پوچ)
		    prepare_buffer(buf);
	    }
	}
    }

    اگر ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	اگر (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	اگر (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	وگرنه اگر (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    اگر (buf == پوچ)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	وگرنه
	{
	    صحیح پیام = TRUE;

	    اگر (opt->jo_set2 & JO2_ERR_پیام)
		پیام = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, پیام);
	}
	اگر (buf != پوچ)
	{
	    اگر (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    اگر (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    وگرنه
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (نویسه *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		اگر (buf->b_ml.ml_mfp == پوچ)
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
    صحیح
channel_parse_socketserver_address(
	char_u	*address,
	صحیح	*port,
	char_u	**unix_path,
	bool	quiet)
{
    نویسه    *rest;
    long    val;

    اگر (*address == NUL)
	برو fail;

    اگر (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	بازگشت OK;
    }

    val = strtol((نویسه *)(address), &rest, 10);
    اگر (val < 0 || val >= 65536 || *rest != NUL)
    {
	اگر (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	بازگشت FAIL;
    }
    *port = (صحیح)val;

    بازگشت OK;
fail:
    اگر (!quiet)
	semsg(_(e_invalid_argument_str), address);
    بازگشت FAIL;
}

/*
 * Implements ch_open().
 */
    ایستا channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    نویسه	*rest;
    صحیح		port = 0;
    صحیح		is_ipv6 = FALSE;
    صحیح		is_unix = FALSE;
    صحیح		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = پوچ;

    اگر (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت پوچ;

    address = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	بازگشت پوچ;

    اگر (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	بازگشت پوچ;
    }

    اگر (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    وگرنه اگر (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    وگرنه اگر (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	اگر (p == پوچ || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    بازگشت پوچ;
	}
    }
    وگرنه
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	اگر (p == پوچ)
	{
	    semsg(_(e_invalid_argument_str), address);
	    بازگشت پوچ;
	}
    }

    اگر (!is_unix && !is_stdio)
    {
	port = strtol((نویسه *)(p + 1), &rest, 10);
	اگر (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    بازگشت پوچ;
	}
	اگر (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	وگرنه
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    اگر (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	برو theend;
    اگر (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	برو theend;
    }

    اگر (is_stdio)
	channel = channel_open_stdio();
    وگرنه اگر (is_unix)
	channel = channel_open_unix((نویسه *)address, پوچ);
    وگرنه
	channel = channel_open((نویسه *)address, port, opt.jo_waittime, پوچ);
    اگر (channel != پوچ)
    {
	صحیح	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	اگر (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    بازگشت channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
ایستا channel_T *stdio_channel = پوچ;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    ایستا channel_T *
channel_open_stdio(تهی)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    اگر (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	بازگشت پوچ;
    }
    اگر (stdio_channel != پوچ)
    {
	emsg(_(e_cannot_open_stdio_channel));
	بازگشت پوچ;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	صحیح	nul_fd;

	اگر (!DuplicateHandle(proc, GetStdHandle(STD_درون_داد_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    بازگشت پوچ;
	}
	اگر (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    بازگشت پوچ;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, پوچ, OPEN_EXISTING, 0,
		پوچ);
	اگر (hnul != INVALID_HANDLE_مقدار)
	{
	    SetStdHandle(STD_درون_داد_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	اگر (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#وگرنه
    {
	صحیح	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	اگر (in_fd < 0 || out_fd < 0)
	{
	    اگر (in_fd >= 0)
		close(in_fd);
	    اگر (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    بازگشت پوچ;
	}
	(تهی)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(تهی)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	اگر (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    اگر (channel == پوچ)
    {
	ch_error(پوچ, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	بازگشت پوچ;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    بازگشت channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    نویسه	*rest;
    صحیح		port;
    صحیح		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = پوچ;

    اگر (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	بازگشت پوچ;

    arg = tv_get_string(&argvars[0]);
    اگر (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	بازگشت پوچ;
    }

    اگر (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    وگرنه
    {
	port = strtol((نویسه *)(arg), &rest, 10);
	اگر (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    بازگشت پوچ;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    اگر (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	برو theend;
    اگر (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	برو theend;
    }

    اگر (is_unix)
	channel = channel_listen_unix((نویسه *)arg, پوچ, true);
    وگرنه
	channel = channel_listen(port, پوچ);
    اگر (channel != پوچ)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    بازگشت channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	صحیح port_in,
	تهی (*nb_close_cb)(تهی))
{
    صحیح			sd = -1;
    ساختار sockaddr_in	server;
#ifndef FEAT_IPV6
    ساختار hostent	*host;
#endif
    صحیح			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    اگر (channel == پوچ)
    {
	ch_error(پوچ, "Cannot allocate channel.");
	بازگشت پوچ;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((نویسه *)&server, 0, اندازه(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    اگر (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	بازگشت پوچ;
    }

#ifdef MSWIN
    اگر (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ثابت نویسه *)&val, اندازه(val)) < 0)
#وگرنه
    اگر (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, اندازه(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	بازگشت پوچ;
    }

    // Bind the socket to the port
    اگر (bind(sd, (ساختار sockaddr *)&server, اندازه(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	بازگشت پوچ;
    }

    اگر (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	بازگشت پوچ;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    اگر (port_in == 0)
    {
	ساختار sockaddr_in	addr;
	socklen_t		addr_len = اندازه(addr);

	اگر (getsockname(sd, (ساختار sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (نویسه *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    بازگشت channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	نویسه *path,
	تهی (*nb_close_cb)(تهی),
	bool replace)
{
    صحیح			sd = -1;
    ساختار sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    اگر (path == پوچ || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == پوچ ? (نویسه *)"" : path);
	بازگشت پوچ;
    }

    path_len = STRLEN(path);
    اگر (path_len >= اندازه(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	بازگشت پوچ;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    اگر (channel == پوچ)
    {
	ch_error(پوچ, "Cannot allocate channel.");
	بازگشت پوچ;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, اندازه(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    اگر (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	بازگشت پوچ;
    }

    اگر (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(ساختار sockaddr_un, sun_path) + path_len + 1;
    اگر (bind(sd, (ساختار sockaddr *)&server, (صحیح)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	بازگشت پوچ;
    }

    اگر (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	بازگشت پوچ;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (نویسه *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    بازگشت channel;
}

     تهی
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    اگر (*fd == INVALID_FD)
	بازگشت;

    اگر (part == PART_SOCK)
	sock_close(*fd);
    وگرنه
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	اگر ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    اگر (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    تهی
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    اگر (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#اگر defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	اگر (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    اگر (out != INVALID_FD)
    {
#اگر defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#اگر defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    اگر (err != INVALID_FD)
    {
#اگر defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#اگر defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    تهی
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    اگر (job->jv_in_buf == پوچ)
	بازگشت;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (نویسه *)in_part->ch_bufref.br_buf->b_ffname);
    اگر (options->jo_set & JO_IN_بالاترین)
    {
	اگر (options->jo_in_بالاترین == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_بالاترین =
		in_part->ch_bufref.br_buf->b_ml.ml_سطر_شمار + 1;
	}
	وگرنه
	    in_part->ch_buf_بالاترین = options->jo_in_بالاترین;
    }
    وگرنه
	in_part->ch_buf_بالاترین = 1;
    اگر (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    وگرنه
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_سطر_شمار;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ایستا تهی
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	صحیح	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    اگر (item == پوچ)
	بازگشت;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = پوچ;
    اگر (item->cq_prev == پوچ)
	head->cq_next = item;
    وگرنه
	item->cq_prev->cq_next = item;
}

    ایستا تهی
write_buf_سطر(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *سطر = ml_get_buf(buf, lnum, FALSE);
    صحیح	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    صحیح	    i;

    // Need to make a copy to be able to append a NL.
    اگر ((p = alloc(len + 2)) == پوچ)
	بازگشت;
    memcpy((نویسه *)p, (نویسه *)سطر, len);

    اگر (channel->ch_write_text_mode)
	p[len] = CAR;
    وگرنه
    {
	برای (i = 0; i < len; ++i)
	    اگر (p[i] == NL)
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
    ایستا صحیح
can_write_buf_سطر(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    اگر (in_part->ch_fd == INVALID_FD)
	بازگشت FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    اگر (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    وگرنه اگر (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# اگر defined(HAVE_SELECT)
	ساختار timeval	tval;
	fd_set		wfds;
	صحیح		ret;

	FD_ZERO(&wfds);
	FD_SET((صحیح)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	برای (;;)
	{
	    ret = select((صحیح)in_part->ch_fd + 1, پوچ, &wfds, پوچ, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    اگر (ret == -1 && errno == EINTR)
		ادامه;
#  endif
	    اگر (ret <= 0 || in_part->ch_block_write == 1)
	    {
		اگر (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		وگرنه
		    ch_log(channel, "Input not ready for writing");
		بازگشت FALSE;
	    }
	    بشکن;
	}
# وگرنه
	ساختار pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	اگر (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    بازگشت FALSE;
	}
	اگر (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    بازگشت FALSE;
	}
# endif
    }
#endif
    بازگشت TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    تهی
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    صحیح		written = 0;

    اگر (buf == پوچ || in_part->ch_buf_append)
	بازگشت;  // no buffer or using appending
    اگر (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == پوچ)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = پوچ;
	بازگشت;
    }

    برای (lnum = in_part->ch_buf_بالاترین; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_سطر_شمار; ++lnum)
    {
	اگر (!can_write_buf_سطر(channel))
	    بشکن;
	write_buf_سطر(buf, lnum, channel);
	++written;
    }

    اگر (written == 1)
	ch_log(channel, "written line %d to channel", (صحیح)lnum - 1);
    وگرنه اگر (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_بالاترین = lnum;
    اگر (lnum > buf->b_ml.ml_سطر_شمار || lnum > in_part->ch_buf_bot)
    {
#اگر defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	اگر (channel->ch_job != پوچ)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = پوچ;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	اگر (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    وگرنه
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_سطر_شمار - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    تهی
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	برای (part = PART_SOCK; part < PART_شمار; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    اگر (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = پوچ;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    تهی
channel_write_درون_داد(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    اگر (in_part->ch_writeque.wq_next != پوچ)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    وگرنه اگر (in_part->ch_bufref.br_buf != پوچ)
    {
	اگر (in_part->ch_buf_append)
	    channel_write_new_سطرها(in_part->ch_bufref.br_buf);
	وگرنه
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    تهی
channel_write_any_سطرها(تهی)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_درون_داد(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    تهی
channel_write_new_سطرها(buf_T *buf)
{
    channel_T	*channel;
    صحیح		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	صحیح	    written = 0;

	اگر (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    اگر (in_part->ch_fd == INVALID_FD)
		ادامه;  // pipe was closed
	    found_one = TRUE;
	    برای (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_سطر_شمار;
								       ++lnum)
	    {
		اگر (!can_write_buf_سطر(channel))
		    بشکن;
		write_buf_سطر(buf, lnum, channel);
		++written;
	    }

	    اگر (written == 1)
		ch_log(channel, "written line %d to channel", (صحیح)lnum - 1);
	    وگرنه اگر (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    اگر (lnum < buf->b_ml.ml_سطر_شمار)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_سطر_شمار - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    اگر (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ایستا تهی
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    اگر (safe_to_invoke_callback == 0)
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

    بازگشت head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *گره)
{
    char_u  *buffer = گره->rq_buffer;
    long_u  i;

    برای (i = 0; i < گره->rq_buflen; ++i)
	اگر (buffer[i] == NL)
	    بازگشت buffer + i;
    بازگشت پوچ;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, صحیح *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *گره = head->rq_next;
    char_u *p;

    اگر (گره == پوچ)
	بازگشت پوچ;
    اگر (outlen != پوچ)
	*outlen += گره->rq_buflen;
    // dispose of the node but keep the buffer
    p = گره->rq_buffer;
    head->rq_next = گره->rq_next;
    اگر (گره->rq_next == پوچ)
	head->rq_prev = پوچ;
    وگرنه
	گره->rq_next->rq_prev = پوچ;
    vim_free(گره);
    بازگشت p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ایستا char_u *
channel_get_all(channel_T *channel, ch_part_T part, صحیح *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *گره;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    برای (گره = head->rq_next; گره != پوچ; گره = گره->rq_next)
	len += گره->rq_buflen;
    res = alloc(len + 1);
    اگر (res == پوچ)
	بازگشت پوچ;
    p = res;
    برای (گره = head->rq_next; گره != پوچ; گره = گره->rq_next)
    {
	mch_memmove(p, گره->rq_buffer, گره->rq_buflen);
	p += گره->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    انجامبده
    {
	p = channel_get(channel, part, پوچ);
	vim_free(p);
    } وقتی (p != پوچ);

    اگر (outlen != پوچ)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	بازگشت res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    وقتی (p < res + len)
    {
	اگر (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	وگرنه اگر (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    اگر (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		وقتی (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		اگر (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    بازگشت res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    تهی
channel_consume(channel_T *channel, ch_part_T part, صحیح len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *گره = head->rq_next;
    char_u *buf = گره->rq_buffer;

    اگر (len < 0 || (long_u)len > گره->rq_buflen)
	بازگشت;
    mch_memmove(buf, buf + len, گره->rq_buflen - len);
    گره->rq_buflen -= len;
    گره->rq_buffer[گره->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    صحیح
channel_collapse(channel_T *channel, ch_part_T part, صحیح want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*گره = head->rq_next;
    readq_T	*last_گره;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    اگر (گره == پوچ || گره->rq_next == پوچ)
	بازگشت FAIL;

    last_گره = گره->rq_next;
    len = گره->rq_buflen + last_گره->rq_buflen;
    اگر (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	وقتی (last_گره->rq_next != پوچ
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_گره) == پوچ))
	{
	    last_گره = last_گره->rq_next;
	    len += last_گره->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    اگر (newbuf == پوچ)
	بازگشت FAIL;	    // out of memory
    mch_memmove(p, گره->rq_buffer, گره->rq_buflen);
    p += گره->rq_buflen;
    vim_free(گره->rq_buffer);
    گره->rq_buffer = newbuf;
    برای (n = گره; n != last_گره; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    گره->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    برای (n = گره->rq_next; n != last_گره; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    گره->rq_next = last_گره->rq_next;
    اگر (last_گره->rq_next == پوچ)
	head->rq_prev = گره;
    وگرنه
	last_گره->rq_next->rq_prev = گره;
    vim_free(last_گره);
    بازگشت OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    ایستا صحیح
channel_save(channel_T *channel, ch_part_T part, char_u *buf, صحیح len,
						      صحیح prepend, نویسه *lead)
{
    readq_T *گره;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    صحیح	    i;

    گره = ALLOC_ONE(readq_T);
    اگر (گره == پوچ)
	بازگشت FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    گره->rq_buffer = alloc(len + 1);
    اگر (گره->rq_buffer == پوچ)
    {
	vim_free(گره);
	بازگشت FAIL;	    // out of memory
    }

    اگر (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = گره->rq_buffer;
	برای (i = 0; i < len; ++i)
	    اگر (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	گره->rq_buflen = (long_u)(p - گره->rq_buffer);
    }
    وگرنه
    {
	mch_memmove(گره->rq_buffer, buf, len);
	گره->rq_buffer[len] = NUL;
	گره->rq_buflen = (long_u)len;
    }

    اگر (prepend)
    {
	// prepend node to the head of the queue
	گره->rq_next = head->rq_next;
	گره->rq_prev = پوچ;
	اگر (head->rq_next == پوچ)
	    head->rq_prev = گره;
	وگرنه
	    head->rq_next->rq_prev = گره;
	head->rq_next = گره;
    }
    وگرنه
    {
	// append node to the tail of the queue
	گره->rq_next = پوچ;
	گره->rq_prev = head->rq_prev;
	اگر (head->rq_prev == پوچ)
	    head->rq_next = گره;
	وگرنه
	    head->rq_prev->rq_next = گره;
	head->rq_prev = گره;
    }

    اگر (ch_log_active() && lead != پوچ)
	ch_log_literal(lead, channel, part, buf, len);

    بازگشت OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    صحیح
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, پوچ);
    صحیح		keeplen;
    صحیح		addlen;
    char_u	*p;

    اگر (next == پوچ)
	بازگشت FALSE;

    keeplen = reader->js_end - reader->js_buf;
    اگر (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (صحیح)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	اگر (p == پوچ)
	{
	    vim_free(next);
	    بازگشت FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    بازگشت TRUE;
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
    ایستا صحیح
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*سطر_start;
    char_u	*p;
    int_u	hdr_len;
    صحیح		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    وقتی (TRUE)
    {
	سطر_start = p;
	وقتی (*p != NUL && *p != '\n')
	    p++;
	اگر (*p == NUL)			// partial header
	    بازگشت MAYBE;
	p++;

	// process the content length field (if present)
	اگر ((p - سطر_start > 16)
		&& STRNICMP(سطر_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((نویسه *)سطر_start + 16, پوچ, 10);
	    اگر (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		بازگشت FAIL;
	}

	اگر ((p - سطر_start) == 2 && سطر_start[0] == '\r' &&
		سطر_start[1] == '\n')
	    // reached the empty line
	    بشکن;
    }

    اگر (payload_len == -1)
	// Content-Length field is not present in the header
	بازگشت FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    اگر (jsbuf_len < hdr_len + payload_len)
	بازگشت MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    بازگشت OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    صحیح
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    صحیح		status = OK;
    صحیح		ret;

    اگر (channel_peek(channel, part) == پوچ)
	بازگشت FALSE;

    reader.js_buf = channel_get(channel, part, پوچ);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    اگر (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    اگر (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    اگر (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	اگر (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	وگرنه اگر (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    اگر (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    وگرنه
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	وگرنه
	{
	    item = ALLOC_ONE(jsonq_T);
	    اگر (item == پوچ)
		clear_tv(&listtv);
	    وگرنه
	    {
		item->jq_no_callback = FALSE;
		item->jq_مقدار = alloc_tv();
		اگر (item->jq_مقدار == پوچ)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		وگرنه
		{
		    *item->jq_مقدار = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = پوچ;
		    اگر (item->jq_prev == پوچ)
			head->jq_next = item;
		    وگرنه
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    اگر (status == OK)
	chanpart->ch_wait_len = 0;
    وگرنه اگر (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	اگر (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (صحیح)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_شمار() + 100L;
#وگرنه
	    gettimeofday(&chanpart->ch_deadline, پوچ);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    اگر (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	وگرنه
	{
	    صحیح timeout;
#ifdef MSWIN
	    timeout = (صحیح)(Get_Tick_شمار() - chanpart->ch_deadline) > 0;
#وگرنه
	    {
		ساختار timeval now_tv;

		gettimeofday(&now_tv, پوچ);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    اگر (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    وگرنه
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    اگر (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    وگرنه اگر (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(صحیح)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, پوچ);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    وگرنه
	ret = FALSE;

    vim_free(reader.js_buf);
    بازگشت ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ایستا تهی
remove_cb_گره(cbq_T *head, cbq_T *گره)
{
    اگر (گره->cq_prev == پوچ)
	head->cq_next = گره->cq_next;
    وگرنه
	گره->cq_prev->cq_next = گره->cq_next;
    اگر (گره->cq_next == پوچ)
	head->cq_prev = گره->cq_prev;
    وگرنه
	گره->cq_next->cq_prev = گره->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    تهی
remove_json_گره(jsonq_T *head, jsonq_T *گره)
{
    اگر (گره->jq_prev == پوچ)
	head->jq_next = گره->jq_next;
    وگرنه
	گره->jq_prev->jq_next = گره->jq_next;
    اگر (گره->jq_next == پوچ)
	head->jq_prev = گره->jq_prev;
    وگرنه
	گره->jq_next->jq_prev = گره->jq_prev;
    vim_free(گره);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ایستا تهی
channel_add_block_id(chanpart_T *chanpart, صحیح id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    اگر (gap->ga_growsize == 0)
	ga_init2(gap, اندازه(صحیح), 10);
    اگر (ga_grow(gap, 1) == OK)
    {
	((صحیح *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ایستا تهی
channel_remove_block_id(chanpart_T *chanpart, صحیح id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    صحیح		i;

    برای (i = 0; i < gap->ga_len; ++i)
	اگر (((صحیح *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    اگر (i < gap->ga_len)
	    {
		صحیح *p = ((صحیح *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * اندازه(صحیح));
	    }
	    بازگشت;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ایستا صحیح
channel_has_block_id(chanpart_T *chanpart, صحیح id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    صحیح		i;

    برای (i = 0; i < gap->ga_len; ++i)
	اگر (((صحیح *)gap->ga_data)[i] == id)
	    بازگشت TRUE;
    بازگشت FALSE;
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
    ایستا صحیح
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	صحیح	    id,
	صحیح	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    وقتی (item != پوچ)
    {
	list_T	    *l;
	typval_T    *tv;

	اگر (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_مقدار->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	وگرنه
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    اگر (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		اگر (id > 0)
		{
		    اگر (item->jq_مقدار->v_type != VAR_DICT)
			برو nextitem;
		    d = item->jq_مقدار->vval.v_dict;
		    اگر (d == پوچ)
			برو nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    اگر (dict_has_کلید(d, "method"))
			برو nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    اگر (di == پوچ)
			برو nextitem;
		    tv = &di->di_tv;
		}
		وگرنه
		    tv = item->jq_مقدار;
	    }
	    وگرنه
	    {
		اگر (id > 0)
		{
		    اگر (item->jq_مقدار->v_type != VAR_DICT)
			برو nextitem;
		    d = item->jq_مقدار->vval.v_dict;
		    اگر (d == پوچ)
			برو nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    اگر (di == پوچ)
			برو nextitem;
		    tv = &di->di_tv;
		}
		وگرنه
		    tv = item->jq_مقدار;
	    }
	}

	اگر ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_عدد && tv->vval.v_عدد == id)
	      || (id <= 0 && (tv->v_type != VAR_عدد
		 || tv->vval.v_عدد == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_عدد)))))
	{
	    *rettv = item->jq_مقدار;
	    اگر (tv->v_type == VAR_عدد)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_عدد);
	    remove_json_گره(head, item);
	    بازگشت OK;
	}
nextitem:
	item = item->jq_next;
    }
    بازگشت FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ایستا تهی
channel_افزودن_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    اگر (head->jq_prev != پوچ && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = پوچ;
    وگرنه وقتی (item != پوچ && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    اگر (newitem == پوچ)
    {
	clear_tv(rettv);
	بازگشت;
    }

    newitem->jq_مقدار = alloc_tv();
    اگر (newitem->jq_مقدار == پوچ)
    {
	vim_free(newitem);
	clear_tv(rettv);
	بازگشت;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_مقدار = *rettv;
    اگر (item == پوچ)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = پوچ;
	اگر (newitem->jq_prev == پوچ)
	    head->jq_next = newitem;
	وگرنه
	    newitem->jq_prev->jq_next = newitem;
    }
    وگرنه
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	اگر (newitem->jq_next == پوچ)
	    head->jq_prev = newitem;
	وگرنه
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    ایستا تهی
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    صحیح	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    اگر (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	اگر (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	بازگشت;
    }
    arg = argv[1].vval.v_string;
    اگر (arg == پوچ)
	arg = (char_u *)"";

    اگر (STRCMP(cmd, "ex") == 0)
    {
	صحیح	called_emsg_before = called_emsg;
	char_u	*p = arg;
	صحیح	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (نویسه *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	اگر (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	اگر (do_emsg_silent)
	    --emsg_silent;
	اگر (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (نویسه *)get_vim_var_str(VV_ERRMSG));
    }
    وگرنه اگر (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (نویسه *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_شمار = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    وگرنه اگر (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    وگرنه اگر (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	صحیح is_call = cmd[0] == 'c';
	صحیح id_idx = is_call ? 3 : 2;

	اگر (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_عدد)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    اگر (p_verbose > 2)
		emsg(_(e_last_argument_for_عبارت_call_must_be_عدد));
	}
	وگرنه اگر (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    اگر (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	وگرنه
	{
	    typval_T	*tv = پوچ;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = پوچ;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    اگر (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (نویسه *)arg);
		tv = eval_عبارت(arg, پوچ);
	    }
	    وگرنه
	    {
		ch_log(channel, "Calling '%s'", (نویسه *)arg);
		اگر (func_call(arg, &argv[2], پوچ, پوچ, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    اگر (argv[id_idx].v_type == VAR_عدد)
	    {
		صحیح id = argv[id_idx].vval.v_عدد;

		اگر (tv != پوچ)
		    json = json_encode_nr_عبارت(id, tv, options | JSON_NL);
		اگر (tv == پوچ || (json != پوچ && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_عبارت(id, &err_tv, options | JSON_NL);
		}
		اگر (json != پوچ)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (صحیح)STRLEN(json), (نویسه *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    اگر (tv == &res_tv)
		clear_tv(tv);
	    وگرنه
		free_tv(tv);
	}
    }
    وگرنه اگر (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (نویسه *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    ایستا تهی
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (نویسه *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_گره(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    ایستا تهی
append_to_buffer(
    buf_T	*buffer,
    char_u	*پیام,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_سطر_شمار;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    صحیح		save_p_ma = buffer->b_p_ma;
    صحیح		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    اگر (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	اگر (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	بازگشت;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    اگر (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (صحیح)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    اگر (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	بازگشت;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    اگر (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, پیام, TRUE);
	lnum = 0;
    }
    وگرنه
	ml_append(lnum, پیام, 0, FALSE);
    appended_سطرها_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    اگر (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    وگرنه
	buffer->b_p_ma = save_p_ma;

    اگر (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_buffer == buffer)
	    {
		صحیح move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		اگر (move_cursor || wp->w_topline > buffer->b_ml.ml_سطر_شمار)
		{
		    win_T *save_curwin = curwin;

		    اگر (move_cursor)
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

    اگر (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    اگر (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_سطر_شمار;
	}
    }
}

    ایستا تهی
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *پیام;

    وقتی ((پیام = channel_get(channel, part, پوچ)) != پوچ)
    {
	ch_log(channel, "Dropping message '%s'", (نویسه *)پیام);
	vim_free(پیام);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ایستا صحیح
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    بازگشت ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ایستا صحیح
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*پیام = پوچ;
    blob_T	*blob = پوچ;
    typval_T	*listtv = پوچ;
    typval_T	argv[CH_JSON_MAX_ARGS];
    صحیح		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = پوچ;
    buf_T	*buffer = پوچ;
    char_u	*p;
    صحیح		called_otc;		// one time callbackup
    صحیح		raw_len = 0;

    اگر (channel->ch_nb_close_cb != پوچ
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	بازگشت FALSE;

    // Use a message-specific callback, part callback or channel callback
    برای (cbitem = cbhead->cq_next; cbitem != پوچ; cbitem = cbitem->cq_next)
	اگر (cbitem->cq_seq_nr == 0)
	    بشکن;
    اگر (cbitem != پوچ)
	callback = &cbitem->cq_callback;
    وگرنه اگر (ch_part->ch_callback.cb_name != پوچ)
	callback = &ch_part->ch_callback;
    وگرنه اگر (channel->ch_callback.cb_name != پوچ)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    اگر (buffer != پوچ && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == پوچ))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = پوچ;
	buffer = پوچ;
    }

    اگر (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	صحیح		argc = 0;

	// Get any json message in the queue.
	اگر (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    اگر (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(تهی)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    اگر (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		بازگشت FALSE;
	}

	اگر (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    اگر (d != پوچ)
	    {
		اگر (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		وگرنه
		    di = dict_find(d, (char_u *)"request_seq", -1);
		اگر (di != پوچ && di->di_tv.v_type == VAR_عدد)
		    seq_nr = di->di_tv.vval.v_عدد;
	    }

	    argv[1] = *listtv;
	}
	وگرنه
	{
	    برای (item = listtv->vval.v_list->lv_first;
		    item != پوچ && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    وقتی (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    اگر (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		بازگشت TRUE;
	    }

	    اگر (argv[0].v_type != VAR_عدد)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		بازگشت FALSE;
	    }
	    seq_nr = argv[0].vval.v_عدد;
	}
    }
    وگرنه اگر (channel_peek(channel, part) == پوچ)
    {
	// nothing to read on RAW or NL channel
	بازگشت FALSE;
    }
    وگرنه
    {
	// If there is no callback or buffer drop the message.
	اگر (callback == پوچ && buffer == پوچ)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    اگر (channel->ch_close_cb.cb_name == پوچ && !channel->ch_drop_never)
		drop_messages(channel, part);
	    بازگشت FALSE;
	}

	اگر (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = پوچ;
	    char_u  *buf;
	    readq_T *گره;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    وقتی (TRUE)
	    {
		گره = channel_peek(channel, part);
		nl = channel_first_nl(گره);
		اگر (nl != پوچ)
		    بشکن;
		اگر (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    اگر (ch_part->ch_fd == INVALID_FD && گره->rq_buflen > 0)
			بشکن;
		    بازگشت FALSE; // incomplete message
		}
	    }
	    buf = گره->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    برای (p = buf; (nl == پوچ || p < nl)
					    && p < buf + گره->rq_buflen; ++p)
		اگر (*p == NUL)
		    *p = NL;

	    اگر (nl == پوچ)
	    {
		// get the whole buffer, drop the NL
		پیام = channel_get(channel, part, پوچ);
	    }
	    وگرنه اگر (nl + 1 == buf + گره->rq_buflen)
	    {
		// get the whole buffer
		پیام = channel_get(channel, part, پوچ);
		*nl = NUL;
	    }
	    وگرنه
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		پیام = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (صحیح)(nl - buf) + 1);
	    }
	}
	وگرنه
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    پیام = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : پوچ);
	}

	اگر (پیام == پوچ)
	    بازگشت FALSE; // out of memory (and avoids Coverity warning)

	اگر (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    اگر (blob == پوچ)
	    {
		vim_free(پیام);
		بازگشت FALSE;
	    }
	    اگر (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(پیام);
		بازگشت FALSE;
	    }
	    اگر (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, پیام, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	وگرنه
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = پیام;
	}
    }

    called_otc = FALSE;
    اگر (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	صحیح lsp_req_پیام = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	اگر ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_کلید(argv[1].vval.v_dict, "method"))
	    lsp_req_پیام = TRUE;

	اگر (!lsp_req_پیام)
	{
	    برای (cbitem = cbhead->cq_next; cbitem != پوچ;
		    cbitem = cbitem->cq_next)
	    {
		اگر (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    بشکن;
		}
	    }
	}
    }

    اگر (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	اگر (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    اگر (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_افزودن_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_عدد;
		free_tv(listtv);
		listtv = پوچ;
	    }
	    وگرنه
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    وگرنه اگر (callback != پوچ || buffer != پوچ)
    {
	اگر (buffer != پوچ)
	{
	    اگر (پیام == پوچ)
		// JSON or JS mode: re-encode the message.
		پیام = json_encode(listtv, ch_mode);
	    اگر (پیام != پوچ)
	    {
#ifdef FEAT_TERMINAL
		اگر (buffer->b_term != پوچ)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    اگر (part == PART_ERR)
		    {
			char_u *cp;
			صحیح    extra = 0;

			برای (cp = پیام; *cp != NUL; ++cp)
			    اگر (*cp == NL && (cp == پیام || cp[-1] != CAR))
				++extra;
			اگر (extra > 0)
			{
			    char_u *crlf_پیام = alloc(STRLEN(پیام) + extra + 1);

			    اگر (crlf_پیام != پوچ)
			    {
				char_u *q = crlf_پیام;

				برای (cp = پیام; *cp != NUL; ++cp)
				{
				    اگر (*cp == NL && (cp == پیام || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_پیام, channel);
				vim_free(crlf_پیام);
			    }
			}
			وگرنه
			    write_to_term(buffer, پیام, channel);
		    }
		    وگرنه
			write_to_term(buffer, پیام, channel);
		}
		وگرنه
#endif
		    append_to_buffer(buffer, پیام, channel, part);
	    }
	}

	اگر (callback != پوچ)
	{
	    اگر (cbitem != پوچ)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    وگرنه
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (نویسه *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    وگرنه
	ch_log(channel, "Dropping message %d", seq_nr);

    اگر (listtv != پوچ)
	free_tv(listtv);
    اگر (blob != پوچ)
	blob_unref(blob);
    vim_free(پیام);

    بازگشت TRUE;
}

#اگر defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    صحیح
channel_can_write_to(channel_T *channel)
{
    بازگشت channel != پوچ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    صحیح
channel_is_open(channel_T *channel)
{
    بازگشت channel != پوچ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ایستا تهی *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    اگر (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	اگر (head->jq_next == پوچ)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	بازگشت head->jq_next;
    }
    بازگشت channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ایستا صحیح
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    بازگشت channel_readahead_pointer(channel, part) != پوچ;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ایستا نویسه *
channel_status(channel_T *channel, صحیح req_part)
{
    ch_part_T part;
    صحیح has_readahead = FALSE;

    اگر (channel == پوچ)
	 بازگشت "fail";
    اگر (req_part == PART_OUT)
    {
	اگر (channel->CH_OUT_FD != INVALID_FD)
	    بازگشت "open";
	اگر (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    وگرنه اگر (req_part == PART_ERR)
    {
	اگر (channel->CH_ERR_FD != INVALID_FD)
	    بازگشت "open";
	اگر (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    وگرنه
    {
	اگر (channel_is_open(channel))
	    بازگشت "open";
	برای (part = PART_SOCK; part < PART_IN; ++part)
	    اگر (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		بشکن;
	    }
    }

    اگر (has_readahead)
	بازگشت "buffered";
    بازگشت "closed";
}

    ایستا تهی
channel_part_info(channel_T *channel, dict_T *dict, نویسه *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    نویسه	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    اگر (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    وگرنه اگر (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    وگرنه
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (صحیح)s.length);

    STRCPY(namebuf + tail, "mode");
    انتخاب (chanpart->ch_mode)
    {
	حالت CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    بشکن;
	حالت CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    بشکن;
	حالت CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    بشکن;
	حالت CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    بشکن;
	حالت CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    بشکن;
	حالت CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    بشکن;
	حالت CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    بشکن;
	پیشفرض:
	    STR_LITERAL_SET(s, "");
	    بشکن;
    }
    dict_add_string_len(dict, namebuf, s.string, (صحیح)s.length);

    STRCPY(namebuf + tail, "io");
    اگر (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    وگرنه انتخاب (chanpart->ch_io)
    {
	حالت JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    بشکن;
	حالت JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    بشکن;
	حالت JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    بشکن;
	حالت JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    بشکن;
	حالت JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    بشکن;
	پیشفرض:
	    STR_LITERAL_SET(s, "");
	    بشکن;
    }
    dict_add_string_len(dict, namebuf, s.string, (صحیح)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_عدد(dict, namebuf, chanpart->ch_timeout);
}

    ایستا تهی
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_عدد(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    اگر (channel->ch_hostname != پوچ)
    {
	اگر (channel->ch_port)
	    dict_add_عدد(dict, "port", channel->ch_port);
	وگرنه
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    وگرنه
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
    تهی
channel_close(channel_T *channel, صحیح invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    اگر (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	اگر (channel->ch_close_cb.cb_name != پوچ)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	برای (part = PART_SOCK; part < PART_IN; ++part)
	{
	    اگر (channel->ch_close_cb.cb_name != پوچ
			    || channel->ch_part[part].ch_bufref.br_buf != پوچ)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		اگر (channel->ch_close_cb.cb_name == پوچ)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		وقتی (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	اگر (channel->ch_close_cb.cb_name != پوچ)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (نویسه *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      اگر (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      اگر (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  برای (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = پوچ;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = پوچ;
    channel->ch_ss_close_cb = پوچ;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    ایستا تهی
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    ایستا تهی
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    اگر (wq->wq_next == پوچ)
	wq->wq_prev = پوچ;
    وگرنه
	wq->wq_next->wq_prev = پوچ;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ایستا تهی
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    وقتی (channel_peek(channel, part) != پوچ)
	vim_free(channel_get(channel, part, پوچ));

    وقتی (cb_head->cq_next != پوچ)
    {
	cbq_T *گره = cb_head->cq_next;

	remove_cb_گره(cb_head, گره);
	free_callback(&گره->cq_callback);
	vim_free(گره);
    }

    وقتی (json_head->jq_next != پوچ)
    {
	free_tv(json_head->jq_next->jq_مقدار);
	remove_json_گره(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    وقتی (ch_part->ch_writeque.wq_next != پوچ)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    تهی
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

#اگر defined(EXITFREE)
    تهی
channel_free_all(تهی)
{
    channel_T *channel;

    ch_log(پوچ, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_پیام_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ایستا صحیح
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    اگر (in_part->ch_writeque.wq_next != پوچ)
	بازگشت TRUE;
    اگر (buf == پوچ)
	بازگشت FALSE;
    بازگشت in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_سطر_شمار)
	    : (in_part->ch_buf_بالاترین <= in_part->ch_buf_bot
			    && in_part->ch_buf_بالاترین <= buf->b_ml.ml_سطر_شمار);
}

#اگر defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ایستا صحیح
channel_fill_wfds(صحیح maxfd_arg, fd_set *wfds)
{
    صحیح		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	اگر (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (صحیح)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((صحیح)in_part->ch_fd, wfds);
	    اگر ((صحیح)in_part->ch_fd >= maxfd)
		maxfd = (صحیح)in_part->ch_fd + 1;
	}
    }
    بازگشت maxfd;
}
#وگرنه
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ایستا صحیح
channel_fill_poll_write(صحیح nfd_in, ساختار pollfd *fds)
{
    صحیح		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	اگر (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	وگرنه
	    in_part->ch_poll_idx = -1;
    }
    بازگشت nfd;
}
#endif

تعریفنوع شمارش {
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
    ایستا channel_wait_result
channel_wait(channel_T *channel, sock_T fd, صحیح timeout)
{
    اگر (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    اگر (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	صحیح	sleep_time;
	DWORD	deadline = Get_Tick_شمار() + timeout;
	صحیح	delay = 1;

	// reading from a pipe, not a socket
	وقتی (TRUE)
	{
	    صحیح r = PeekNamedPipe((HANDLE)fd, پوچ, 0, پوچ, &nread, پوچ);

	    اگر (r && nread > 0)
		بازگشت CW_READY;

	    اگر (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, پوچ);
	    }
	    وگرنه اگر (r == 0)
		بازگشت CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_سطرها();

	    sleep_time = deadline - Get_Tick_شمار();
	    اگر (sleep_time <= 0)
		بشکن;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    اگر (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    اگر (delay > 10)
		delay = 10;
	}
    }
    وگرنه
#endif
    {
#اگر defined(HAVE_SELECT)
	ساختار timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	صحیح		ret;
	صحیح		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	برای (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((صحیح)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (صحیح)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, پوچ, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    اگر (ret == -1 && errno == EINTR)
		ادامه;
# endif
	    اگر (ret > 0)
	    {
		اگر (FD_ISSET(fd, &rfds))
		    بازگشت CW_READY;
		channel_write_any_سطرها();
		ادامه;
	    }
	    بشکن;
	}
#وگرنه
	برای (;;)
	{
	    ساختار pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    صحیح		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    اگر (poll(fds, nfd, timeout) > 0)
	    {
		اگر (fds[0].revents & POLLIN)
		    بازگشت CW_READY;
		channel_write_any_سطرها();
		ادامه;
	    }
	    بشکن;
	}
#endif
    }
    بازگشت CW_NOT_READY;
}

    ایستا تهی
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, صحیح is_err, نویسه *func)
{
    نویسه	پیام[] = "%s(): Read %s from ch_part[%d], closing";

    اگر (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, پیام, func, "error", part);
    وگرنه
	ch_log(channel, پیام, func, "EOF", part);

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
    اگر (channel->ch_nb_close_cb != پوچ)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_پیام_RAW,
			      (صحیح)STRLEN_LITERAL(DETACH_پیام_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    اگر (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	اگر (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    ایستا تهی
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    اگر (channel->ch_nb_close_cb != پوچ)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    اگر (channel->ch_ss_close_cb != پوچ)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    ایستا تهی
channel_read(channel_T *channel, ch_part_T part, نویسه *func)
{
    ایستا char_u	*buf = پوچ;
    صحیح			len = 0;
    صحیح			readlen = 0;
    sock_T		fd;
    صحیح			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    اگر (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	بازگشت;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    اگر (buf == پوچ)
    {
	buf = alloc(MAXMSGSIZE);
	اگر (buf == پوچ)
	    بازگشت;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    برای (;;)
    {
	اگر (channel_wait(channel, fd, 0) != CW_READY)
	    بشکن;
	اگر (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    ساختار sockaddr_storage	client;

	    newchannel = add_channel();
	    اگر (newchannel == پوچ)
	    {
		ch_error(پوچ, "Cannot allocate channel.");
		بازگشت;
	    }
	    socklen = اندازه(client);
	    newfd = accept(fd, (ساختار sockaddr*)&client, &socklen);
	    اگر (newfd < 0)
	    {
		ch_error(پوچ, "Cannot accept channel.");
		channel_free(newchannel);
		بازگشت;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    اگر (channel->ch_ss_accept_cb != پوچ)
	    {
		channel->ch_ss_accept_cb(newchannel);
		بازگشت;
	    }
#endif

	    اگر (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		نویسه addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((ساختار sockaddr_in*)&client)->sin_addr,
			addr, اندازه(addr));
		vim_snprintf((نویسه *)namebuf, اندازه(namebuf), "%s:%d",
			addr,
			ntohs(((ساختار sockaddr_in*)&client)->sin_port));
#وگرنه
		vim_snprintf((نویسه *)namebuf, اندازه(namebuf), "%s:%d",
		    inet_ntoa(((ساختار sockaddr_in*)&client)->sin_addr),
		    ntohs(((ساختار sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    وگرنه اگر (client.ss_family == AF_INET6)
	    {
		نویسه addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((ساختار sockaddr_in6*)&client)->sin6_addr,
			addr, اندازه(addr));
		vim_snprintf((نویسه *)namebuf, اندازه(namebuf), "[%s]:%d",
			addr,
			ntohs(((ساختار sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    وگرنه اگر (client.ss_family == AF_UNIX)
		vim_snprintf((نویسه *)namebuf, اندازه(namebuf),
							    "unix:anonymous");
	    وگرنه
		vim_snprintf((نویسه *)namebuf, اندازه(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    بازگشت;
	}
	اگر (use_socket)
	    len = sock_read(fd, (نویسه *)buf, MAXMSGSIZE);
	وگرنه
	    len = fd_read(fd, (نویسه *)buf, MAXMSGSIZE);
	اگر (len <= 0)
	    بشکن;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    اگر (readlen <= 0)
    {
	اگر (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#اگر defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    وگرنه اگر (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    تهی
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
    ایستا char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, صحیح timeout, صحیح raw, صحیح *outlen)
{
    char_u	*buf;
    char_u	*پیام;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*گره;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    وقتی (TRUE)
    {
	گره = channel_peek(channel, part);
	اگر (گره != پوچ)
	{
	    اگر (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(گره) != پوچ))
		// got a complete message
		بشکن;
	    اگر (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		ادامه;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    اگر (raw || fd == INVALID_FD)
		بشکن;
	}

	// Wait for up to the channel timeout.
	اگر (fd == INVALID_FD)
	    بازگشت پوچ;
	اگر (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    بازگشت پوچ;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    اگر (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != پوچ)
    {
	پیام = channel_get_all(channel, part, outlen);
    }
    وگرنه
    {
	char_u *p;

	buf = گره->rq_buffer;
	nl = channel_first_nl(گره);

	// Convert NUL to NL, the internal representation.
	برای (p = buf; (nl == پوچ || p < nl) && p < buf + گره->rq_buflen; ++p)
	    اگر (*p == NUL)
		*p = NL;

	اگر (nl == پوچ)
	{
	    // must be a closed channel with missing NL
	    پیام = channel_get(channel, part, پوچ);
	}
	وگرنه اگر (nl + 1 == buf + گره->rq_buflen)
	{
	    // get the whole buffer
	    پیام = channel_get(channel, part, پوچ);
	    *nl = NUL;
	}
	وگرنه
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    پیام = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (صحیح)(nl - buf) + 1);
	}
    }
    اگر (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != پوچ ? *outlen : (صحیح)STRLEN(پیام));
    بازگشت پیام;
}

ایستا صحیح channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    صحیح
channel_in_blocking_wait(تهی)
{
    بازگشت channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ایستا تهی
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, صحیح timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (تهی)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#وگرنه
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
    ایستا صحیح
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	صحیح	    timeout_arg,
	صحیح	    id,
	typval_T    **rettv)
{
    صحیح		more;
    sock_T	fd;
    صحیح		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    صحیح		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    اگر (id >= 0)
	channel_add_block_id(chanpart, id);

    برای (;;)
    {
	اگر (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (تهی)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	اگر (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    بشکن;
	}

	اگر (!more)
	{
	    تهی *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    تهی *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    اگر (channel_parse_messages())
		ادامه;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    اگر (readahead_ptr != پوچ && readahead_ptr != prev_readahead_ptr)
		ادامه;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (صحیح)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    اگر (timeout <= 0 || fd == INVALID_FD)
	    {
		اگر (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		بشکن;
	    }
	    // If there was an incomplete message use the deadline for that.
	    اگر (chanpart->ch_wait_len > 0)
	    {
		صحیح	چپ;
#ifdef MSWIN
		چپ = chanpart->ch_deadline - Get_Tick_شمار() + 1;
#وگرنه
		{
		    ساختار timeval now_tv;

		    gettimeofday(&now_tv, پوچ);
		    چپ = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		اگر (چپ < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		وگرنه اگر (چپ < timeout)
		    timeout = چپ;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    اگر (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    بازگشت retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, صحیح check_open, صحیح reading, ch_part_T part)
{
    channel_T	*channel = پوچ;
    صحیح		has_readahead = FALSE;

    اگر (tv->v_type == VAR_JOB)
    {
	اگر (tv->vval.v_job != پوچ)
	    channel = tv->vval.v_job->jv_channel;
    }
    وگرنه اگر (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    وگرنه
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	بازگشت پوچ;
    }
    اگر (channel != پوچ && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_شمار ? part : channel_part_read(channel));

    اگر (check_open && (channel == پوچ || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	بازگشت پوچ;
    }
    بازگشت channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ایستا تهی
common_channel_read(typval_T *argvars, typval_T *rettv, صحیح raw, صحیح blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_شمار;
    jobopt_T	opt;
    صحیح		mode;
    صحیح		timeout;
    صحیح		id = -1;
    typval_T	*listtv = پوچ;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    clear_job_options(&opt);
    اگر (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	برو theend;

    اگر (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    اگر (channel == پوچ)
	برو theend;

    اگر (part == PART_شمار)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    اگر (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    اگر (blob || mode == CH_MODE_BLOB)
    {
	صحیح	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	اگر (p != پوچ)
	{
	    blob_T	*b = blob_alloc();

	    اگر (b != پوچ)
	    {
		b->bv_ga.ga_len = outlen;
		اگر (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		وگرنه
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    وگرنه اگر (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, پوچ);
    وگرنه
    {
	اگر (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	اگر (listtv != پوچ)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	وگرنه
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_عدد = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#اگر defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    تهی
channel_handle_events(صحیح only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	اگر (only_keep_open && !channel->ch_keep_open)
	    ادامه;

	// check the socket and pipes
	برای (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    اگر (fd == INVALID_FD)
		ادامه;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    صحیح r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    اگر (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    وگرنه اگر (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	اگر (channel->ch_job != پوچ)
	{
	    job_T *job = channel->ch_job;

	    اگر (job->jv_tty_out != پوچ && job->jv_status == JOB_FINISHED)
		برای (part = PART_SOCK; part < PART_شمار; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#اگر defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    صحیح
channel_any_keep_open(تهی)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	اگر (channel->ch_keep_open)
	    بازگشت TRUE;
    بازگشت FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    تهی
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    صحیح		fd = ch_part->ch_fd;

    اگر (fd == INVALID_FD)
	بازگشت;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#وگرنه
    (تهی)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    صحیح
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	صحیح	  len_arg,
	نویسه	  *fun)
{
    صحیح		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    صحیح		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    اگر (fd == INVALID_FD)
    {
	اگر (!channel->ch_error && fun != پوچ)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	بازگشت FAIL;
    }

    اگر (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    اگر (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_پیام = 0;
    }

    برای (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	صحیح	    len;

	اگر (wq->wq_next != پوچ)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	وگرنه
	{
	    اگر (len_arg == 0)
		// nothing to write, called from channel_select_check()
		بازگشت OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	اگر (part == PART_SOCK)
	    res = sock_write(fd, (نویسه *)buf, len);
	وگرنه
	{
	    res = fd_write(fd, (نویسه *)buf, len);
#ifdef MSWIN
	    اگر (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, پوچ);
	    }
#endif
	}
	اگر (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	اگر (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    اگر (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    اگر (res == len)
	    {
		// Wrote all the buf[len] bytes.
		اگر (entry != پوچ)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    ادامه;
		}
		اگر (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    وگرنه
	    {
		// Wrote only buf[res] bytes, can't write more now.
		اگر (entry != پوچ)
		{
		    اگر (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (نویسه *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		وگرنه
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		اگر (wq->wq_prev != پوچ
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    اگر (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((نویسه *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		وگرنه
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    اگر (last != پوچ)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = پوچ;
			اگر (wq->wq_prev == پوچ)
			    wq->wq_next = last;
			وگرنه
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			اگر (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	وگرنه اگر (res != len)
	{
	    اگر (!channel->ch_error && fun != پوچ)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    بازگشت FAIL;
	}

	channel->ch_error = FALSE;
	بازگشت OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    ایستا channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	صحیح	    len,
	صحیح	    id,
	صحیح	    eval,
	jobopt_T    *opt,
	نویسه	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel == پوچ)
	بازگشت پوچ;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    اگر (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	بازگشت پوچ;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    اگر (opt->jo_callback.cb_name != پوچ && *opt->jo_callback.cb_name != NUL)
    {
	اگر (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    بازگشت پوچ;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    اگر (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == پوچ)
	بازگشت channel;
    بازگشت پوچ;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ایستا تهی
ch_عبارت_common(typval_T *argvars, typval_T *rettv, صحیح eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    صحیح		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحیح		timeout;
    صحیح		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel == پوچ)
	بازگشت;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    اگر (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	بازگشت;
    }

    اگر (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	نویسه		*کلید = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	اگر (rettv_dict_alloc(rettv) == FAIL)
	    بازگشت;

	اگر (check_for_dict_arg(argvars, 1) == FAIL)
	    بازگشت;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)کلید, -1);
	اگر (argvars[2].v_type == VAR_DICT
		&& dict_has_کلید(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	اگر (di != پوچ && di->di_tv.v_type != VAR_عدد
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_مقدار_for_argument_str), کلید);
	    بازگشت;
	}

	اگر (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_پیام_id;
	    اگر (di == پوچ)
		dict_add_عدد(d, "seq", id);
	    وگرنه
		di->di_tv.vval.v_عدد = id;
	}
	وگرنه اگر (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_پیام_id;
	    اگر (di == پوچ)
		dict_add_عدد(d, "id", id);
	    وگرنه
		di->di_tv.vval.v_عدد = id;
	}
	وگرنه
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    اگر (di != پوچ && di->di_tv.v_type == VAR_عدد)
		id = di->di_tv.vval.v_عدد;
	}
	اگر (ch_mode == CH_MODE_LSP && !dict_has_کلید(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_پیام(&argvars[1]);
    }
    وگرنه
    {
	id = ++channel->ch_last_پیام_id;
	text = json_encode_nr_عبارت(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    اگر (text == پوچ)
	بازگشت;

    channel = send_common(argvars, text, (صحیح)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    اگر (channel != پوچ && eval)
    {
	اگر (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	وگرنه
	    timeout = channel_get_timeout(channel, part_read);
	اگر (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    اگر (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_عدد;
		free_tv(listtv);
	    }
	    وگرنه
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_عدد;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    اگر (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	اگر (rettv->vval.v_dict != پوچ)
	    dict_add_عدد(rettv->vval.v_dict, "seq", id);
    }
    وگرنه اگر (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	اگر (rettv->vval.v_dict != پوچ)
	    dict_add_عدد(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ایستا تهی
ch_raw_common(typval_T *argvars, typval_T *rettv, صحیح eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    صحیح		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحیح		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	بازگشت;

    اگر (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    وگرنه
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (صحیح)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    اگر (channel != پوچ && eval)
    {
	اگر (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	وگرنه
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, پوچ);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#اگر defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحیح
channel_poll_setup(صحیح nfd_in, تهی *fds_in, صحیح *towait)
{
    صحیح		nfd = nfd_in;
    channel_T	*channel;
    ساختار	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	برای (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    اگر (ch_part->ch_fd != INVALID_FD)
	    {
		اگر (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    اگر (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		وگرنه
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    وگرنه
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    بازگشت nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحیح
channel_poll_check(صحیح ret_in, تهی *fds_in)
{
    صحیح		ret = ret_in;
    channel_T	*channel;
    ساختار	pollfd *fds = fds_in;
    ch_part_T	part;
    صحیح		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	برای (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    اگر (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    وگرنه اگر (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	اگر (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_درون_داد(channel);
	    --ret;
	}
    }

    بازگشت ret;
}
#endif // UNIX && !HAVE_SELECT

#اگر !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحیح
channel_select_setup(
	صحیح maxfd_in,
	تهی *rfds_in,
	تهی *wfds_in,
	ساختار timeval *tv,
	ساختار timeval **tvp)
{
    صحیح		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	برای (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    اگر (fd != INVALID_FD)
	    {
		اگر (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    اگر (*tvp == پوچ || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		وگرنه
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    اگر ((صحیح)fd >= FD_SETSIZE)
			ادامه;
# endif
		    FD_SET((صحیح)fd, rfds);
		    اگر (maxfd < (صحیح)fd)
			maxfd = (صحیح)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    بازگشت maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحیح
channel_select_check(صحیح ret_in, تهی *rfds_in, تهی *wfds_in)
{
    صحیح		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	برای (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    اگر (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحیح)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    وگرنه اگر (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	اگر (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحیح)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_درون_داد(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	اگر (channel->ch_job != پوچ)
	{
	    job_T *job = channel->ch_job;

	    اگر (job->jv_tty_out != پوچ && job->jv_status == JOB_FINISHED)
		برای (part = PART_SOCK; part < PART_شمار; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    بازگشت ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ایستا تهی
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    اگر (stdio_channel->CH_OUT_FD != INVALID_FD)
	(تهی)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(صحیح)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#وگرنهاگر defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    ساختار timeval	tv;
    ساختار timeval	*tvp = &tv;
    صحیح			maxfd;
    صحیح			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, پوچ, tvp);
    اگر (ret >= 0)
	(تهی)channel_select_check(ret, &rfds, &wfds);
#وگرنه
    ساختار pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    صحیح			nfd;
    صحیح			towait = (صحیح)msec;
    صحیح			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    اگر (ret >= 0)
	(تهی)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    تهی
channel_stdio_loop(تهی)
{
    وقتی (stdio_channel != پوچ && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	اگر (due_time > 0 && due_time < msec)
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
    صحیح
channel_parse_messages(تهی)
{
    channel_T	*channel = first_channel;
    صحیح		ret = FALSE;
    صحیح		r;
    ch_part_T	part = PART_SOCK;
    ایستا صحیح	recursive = 0;
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
    اگر ((did_repeated_پیام & REPEATED_پیام_LOOKING) == 0)
    {
	ch_log(پوچ, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_پیام = REPEATED_پیام_LOOKING;
    }
    وقتی (channel != پوچ)
    {
	اگر (recursive == 1)
	{
	    اگر (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_شمار);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		ادامه;
	    }
	    اگر (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		اگر (channel->ch_job != پوچ)
		    channel->ch_job->jv_channel = پوچ;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		ادامه;
	    }
	    اگر (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		ادامه;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	اگر (part == PART_OUT
		&& channel->ch_job != پوچ
		&& channel->ch_job->jv_tty_out != پوچ
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    ادامه;
	}
#endif
	اگر (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    اگر (r == OK)
		ret = TRUE;
	    اگر (channel_unref(channel) || (r == OK
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
		ادامه;
	    }
	}
	اگر (part < PART_ERR)
	    ++part;
	وگرنه
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    اگر (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    بازگشت ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    صحیح
channel_any_readahead(تهی)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    وقتی (channel != پوچ)
    {
	اگر (channel_has_readahead(channel, part))
	    بازگشت TRUE;
	اگر (part < PART_ERR)
	    ++part;
	وگرنه
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    بازگشت FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    صحیح
set_ref_in_channel(صحیح copyID)
{
    صحیح		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    برای (channel = first_channel; !abort && channel != پوچ;
						   channel = channel->ch_next)
	اگر (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, پوچ, پوچ, پوچ);
	}
    بازگشت abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ایستا ch_part_T
channel_part_send(channel_T *channel)
{
    اگر (channel->CH_SOCK_FD == INVALID_FD)
	بازگشت PART_IN;
    بازگشت PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ایستا ch_part_T
channel_part_read(channel_T *channel)
{
    اگر (channel->CH_SOCK_FD == INVALID_FD)
	بازگشت PART_OUT;
    بازگشت PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ایستا ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    اگر (channel == پوچ)
	بازگشت CH_MODE_JSON;
    بازگشت channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ایستا صحیح
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    بازگشت channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    تهی
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_عدد = 0;
    اگر (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	بازگشت;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel != پوچ)
	rettv->vval.v_عدد = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    تهی
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    اگر (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	بازگشت;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel != پوچ)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    تهی
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    اگر (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	بازگشت;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel != پوچ)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    تهی
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_عدد = -1;

    اگر (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	بازگشت;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel == پوچ)
	بازگشت;

    char_u	*what = tv_get_string(&argvars[1]);
    صحیح		part;
    اگر (STRCMP(what, "err") == 0)
	part = PART_ERR;
    وگرنه اگر (STRCMP(what, "out") == 0)
	part = PART_OUT;
    وگرنه اگر (STRCMP(what, "in") == 0)
	part = PART_IN;
    وگرنه
	part = PART_SOCK;
    اگر (channel->ch_part[part].ch_bufref.br_buf != پوچ)
	rettv->vval.v_عدد =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    تهی
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    اگر (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	بازگشت;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel == پوچ)
	بازگشت;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    اگر (channel->ch_job != پوچ)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    تهی
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    اگر (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	بازگشت;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel != پوچ && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    تهی
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    اگر (check_restricted() || check_secure())
	بازگشت;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    تهی
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    اگر (check_restricted() || check_secure())
	بازگشت;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    تهی
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    تهی
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    تهی
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    تهی
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_عبارت_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    تهی
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_عبارت_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    تهی
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    تهی
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    تهی
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    اگر (check_restricted() || check_secure())
	بازگشت;

    اگر (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel == پوچ)
	بازگشت;
    clear_job_options(&opt);
    اگر (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    تهی
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    صحیح		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = پوچ;

    اگر (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	بازگشت;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	اگر (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    نویسه      *status = channel_status(channel, -1);

    اگر (channel == پوچ)
	vim_snprintf((نویسه *)buf, NUMBUFLEN, "channel %s", status);
    وگرنه
	vim_snprintf((نویسه *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    بازگشت buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(صحیح ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	اگر (ch->ch_id == ch_id)
	    بازگشت ch;
    بازگشت پوچ;
}

#endif // FEAT_JOB_CHANNEL
