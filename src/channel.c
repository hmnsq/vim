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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_JOB_CHANNEL)

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
প্রকারনির্দেশ সংরচনা sockaddr_un {
    ADDRESS_FAMILY sun_family;
    ৱর্ণ sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#অন্যথা
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) লিখ(sd, buf, len)
# define sock_read(sd, buf, len) পঠ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) পঠ(fd, buf, len)
# define fd_write(sd, buf, len) লিখ(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

স্থির শূন্য channel_read(channel_T *channel, ch_part_T part, ৱর্ণ *func);
স্থির শূন্য channel_wait_any(long msec);
স্থির ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
স্থির পূর্ণাঙ্ক channel_get_timeout(channel_T *channel, ch_part_T part);
স্থির channel_T *channel_open_stdio(শূন্য);
স্থির ch_part_T channel_part_send(channel_T *channel);
স্থির ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    বাবে ((ch) = first_channel; (ch) != রিক্ত; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
স্থির পূর্ণাঙ্ক safe_to_invoke_callback = 0;

#ifdef MSWIN
    স্থির পূর্ণাঙ্ক
fd_read(sock_T fd, ৱর্ণ *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    যদি (!ReadFile(h, buf, (DWORD)len, &nread, রিক্ত))
	ঘূৰাই -1;
    ঘূৰাই (পূর্ণাঙ্ক)nread;
}

    স্থির পূর্ণাঙ্ক
fd_write(sock_T fd, ৱর্ণ *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    যেতিয়ালৈ (todo > 0)
    {
	যদি (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	অন্যথা
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	যদি (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    যদি (err != ERROR_IO_PENDING)
		ঘূৰাই -1;
	    যদি (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		ঘূৰাই -1;
	    FlushFileBuffers(h);
	}
	অন্যথা যদি (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    ভঙ্গ;
	todo -= nwrite;
	done += nwrite;
    }
    ঘূৰাই (পূর্ণাঙ্ক)done;
}

    স্থির শূন্য
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(বাৰ্তা) (শূন্য)semsg("%s: %s", বাৰ্তা, strerror_win32(errno))

    স্থির ৱর্ণ *
strerror_win32(পূর্ণাঙ্ক eno)
{
    স্থির LPVOID msgbuf = রিক্ত;
    char_u *ptr;

    যদি (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = রিক্ত;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	রিক্ত,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	রিক্ত);
    যদি (msgbuf != রিক্ত)
	// chomp \r or \n
	বাবে (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    ৱিকল্প (*ptr)
	    {
		পক্ষ '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    ভঙ্গ;
		পক্ষ '\n':
		    যদি (*(ptr + 1) == '\0')
			*ptr = '\0';
		    অন্যথা
			*ptr = ' ';
		    ভঙ্গ;
	    }
    ঘূৰাই msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
স্থির channel_T *first_channel = রিক্ত;
স্থির পূর্ণাঙ্ক next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(শূন্য)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    যদি (channel == রিক্ত)
	ঘূৰাই রিক্ত;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    বাবে (part = PART_SOCK; part < PART_গুনতি; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_প্ৰবেশ_Handler = (Xt_প্ৰবেশ_Id)রিক্ত;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_প্ৰবেশ_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    যদি (first_channel != রিক্ত)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    ঘূৰাই channel;
}

    পূর্ণাঙ্ক
has_any_channel(শূন্য)
{
    ঘূৰাই first_channel != রিক্ত;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    পূর্ণাঙ্ক
channel_still_useful(channel_T *channel)
{
    পূর্ণাঙ্ক has_sock_বাৰ্তা;
    পূর্ণাঙ্ক	has_out_বাৰ্তা;
    পূর্ণাঙ্ক	has_err_বাৰ্তা;

    // If the job was killed the channel is not expected to work anymore.
    যদি (channel->ch_job_killed && channel->ch_job == রিক্ত)
	ঘূৰাই FALSE;

    // If there is a close callback it may still need to be invoked.
    যদি (channel->ch_close_cb.cb_name != রিক্ত)
	ঘূৰাই TRUE;

    // If reading from or a buffer it's still useful.
    যদি (channel->ch_part[PART_IN].ch_bufref.br_buf != রিক্ত)
	ঘূৰাই TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_বাৰ্তা = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != রিক্ত
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != রিক্ত;
    has_out_বাৰ্তা = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != রিক্ত
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != রিক্ত;
    has_err_বাৰ্তা = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != রিক্ত
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != রিক্ত;
    ঘূৰাই (channel->ch_callback.cb_name != রিক্ত && (has_sock_বাৰ্তা
		|| has_out_বাৰ্তা || has_err_বাৰ্তা))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != রিক্ত
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != রিক্ত)
		    && has_out_বাৰ্তা)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != রিক্ত
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != রিক্ত)
		    && has_err_বাৰ্তা);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    পূর্ণাঙ্ক
channel_can_close(channel_T *channel)
{
    ঘূৰাই channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    স্থির শূন্য
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    স্থির শূন্য
channel_free_channel(channel_T *channel)
{
    যদি (channel->ch_next != রিক্ত)
	channel->ch_next->ch_prev = channel->ch_prev;
    যদি (channel->ch_prev == রিক্ত)
	first_channel = channel->ch_next;
    অন্যথা
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    স্থির শূন্য
channel_free(channel_T *channel)
{
    যদি (in_free_unref_বস্তুসমূহ)
	ঘূৰাই;

    যদি (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    অন্যথা
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
    স্থির পূর্ণাঙ্ক
channel_may_free(channel_T *channel)
{
    যদি (!channel_still_useful(channel))
    {
	channel_free(channel);
	ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    পূর্ণাঙ্ক
channel_unref(channel_T *channel)
{
    যদি (channel != রিক্ত && --channel->ch_refcount <= 0)
	ঘূৰাই channel_may_free(channel);
    ঘূৰাই FALSE;
}

    পূর্ণাঙ্ক
free_unused_channels_contents(পূর্ণাঙ্ক copyID, পূর্ণাঙ্ক mask)
{
    পূর্ণাঙ্ক		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	যদি (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    ঘূৰাই did_free;
}

    শূন্য
free_unused_channels(পূর্ণাঙ্ক copyID, পূর্ণাঙ্ক mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    বাবে (ch = first_channel; ch != রিক্ত; ch = ch_next)
    {
	ch_next = ch->ch_next;
	যদি (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#যদি defined(FEAT_GUI)

# যদি defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    স্থির channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    যদি (fd == INVALID_FD)
	ঘূৰাই রিক্ত;

    FOR_ALL_CHANNELS(channel)
    {
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	    যদি (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ঘূৰাই channel;
	    }
    }
    ঘূৰাই রিক্ত;
}

    স্থির শূন্য
channel_read_fd(পূর্ণাঙ্ক fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    যদি (channel == রিক্ত)
	ch_error(রিক্ত, "Channel for fd %d not found", fd);
    অন্যথা
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    স্থির শূন্য
messageFromServerX11(XtPointer clientData,
		  পূর্ণাঙ্ক *unused1 UNUSED,
		  Xt_প্ৰবেশ_Id *unused2 UNUSED)
{
    channel_read_fd((পূর্ণাঙ্ক)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  যদি GTK_CHECK_VERSION(3,0,0)
    স্থির gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    ঘূৰাই TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  অন্যথা
    স্থির শূন্য
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_প্ৰবেশ_Condition unused2 UNUSED)
{
    channel_read_fd((পূর্ণাঙ্ক)(long)clientData);
}
#  endif
# endif

    স্থির শূন্য
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    যদি (!CH_HAS_GUI)
	ঘূৰাই;

    // gets stuck in handling events for a not connected channel
    যদি (channel->ch_keep_open)
	ঘূৰাই;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    যদি (channel->ch_part[part].ch_প্ৰবেশ_Handler == (Xt_প্ৰবেশ_Id)রিক্ত)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_প্ৰবেশ_Handler = Xt_App_Add_প্ৰবেশ(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_প্ৰবেশ_Read_Mask + Xt_প্ৰবেশ_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# অন্যথা
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    যদি (channel->ch_part[part].ch_প্ৰবেশ_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   যদি GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_প্ৰবেশ_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   অন্যথা
	channel->ch_part[part].ch_প্ৰবেশ_Handler = gdk_প্ৰবেশ_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_প্ৰবেশ_Condition)
			     ((পূর্ণাঙ্ক)GDK_প্ৰবেশ_READ + (পূর্ণাঙ্ক)GDK_প্ৰবেশ_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    স্থির শূন্য
channel_gui_register(channel_T *channel)
{
    যদি (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    যদি (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    যদি (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    শূন্য
channel_gui_register_all(শূন্য)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    স্থির শূন্য
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    যদি (channel->ch_part[part].ch_প্ৰবেশ_Handler != (Xt_প্ৰবেশ_Id)রিক্ত)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_প্ৰবেশ(channel->ch_part[part].ch_প্ৰবেশ_Handler);
	channel->ch_part[part].ch_প্ৰবেশ_Handler = (Xt_প্ৰবেশ_Id)রিক্ত;
    }
# অন্যথা
#  ifdef FEAT_GUI_GTK
    যদি (channel->ch_part[part].ch_প্ৰবেশ_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   যদি GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_প্ৰবেশ_Handler);
#   অন্যথা
	gdk_প্ৰবেশ_remove(channel->ch_part[part].ch_প্ৰবেশ_Handler);
#   endif
	channel->ch_part[part].ch_প্ৰবেশ_Handler = 0;
    }
#  endif
# endif
}

    স্থির শূন্য
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    বাবে (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    স্থির পূর্ণাঙ্ক
channel_connect(
	channel_T *channel,
	ধ্রুৱ সংরচনা sockaddr *server_addr,
	পূর্ণাঙ্ক server_addrlen,
	পূর্ণাঙ্ক *waittime)
{
    পূর্ণাঙ্ক		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    যেতিয়ালৈ (TRUE)
    {
	long	elapsed_msec = 0;
	পূর্ণাঙ্ক	waitnow;
	পূর্ণাঙ্ক	ret;

	যদি (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	যদি (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    ঘূৰাই -1;
	}

	যদি (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    যদি (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#অন্যথা
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		ঘূৰাই -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	যদি (ret == 0)
	    // The connection could be established.
	    ভঙ্গ;

	SOCK_ERRNO;
	যদি (*waittime < 0 || (errno != EWOULDBLOCK
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
	    ঘূৰাই -1;
	}
	অন্যথা যদি (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ঘূৰাই -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    সংরচনা timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    পূর্ণাঙ্ক			so_error = 0;
	    socklen_t		so_error_len = পরিমাণ(so_error);
	    সংরচনা timeval	start_tv;
	    সংরচনা timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, রিক্ত);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, রিক্ত, &tv);
	    যদি (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		ঘূৰাই -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    যদি (FD_ISSET(sd, &wfds))
		ভঙ্গ;
	    elapsed_msec = waitnow;
	    যদি (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		অনুৱর্ত;
	    }
#অন্যথা
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    যদি (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		যদি (ret < 0 || (so_error != 0
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
		    ঘূৰাই -1;
		}
		অন্যথা যদি (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ঘূৰাই -1;
		}
	    }

	    যদি (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		ভঙ্গ;

	    gettimeofday(&end_tv, রিক্ত);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	যদি (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    যদি (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    যদি (!got_int)
	    {
		যদি (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		অনুৱর্ত;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	ঘূৰাই -1;
    }

    যদি (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#অন্যথা
	(শূন্য)fcntl(sd, F_SETFL, 0);
#endif
    }

    ঘূৰাই sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ধ্রুৱ ৱর্ণ *path,
	শূন্য (*nb_close_cb)(শূন্য))
{
    channel_T		*channel = রিক্ত;
    পূর্ণাঙ্ক			sd = -1;
    size_t		path_len = STRLEN(path);
    সংরচনা sockaddr_un	server;
    size_t		server_len;
    পূর্ণাঙ্ক			waittime = -1;

    যদি (*path == NUL || path_len >= পরিমাণ(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ঘূৰাই রিক্ত;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == রিক্ত)
    {
	ch_error(রিক্ত, "Cannot allocate channel.");
	ঘূৰাই রিক্ত;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, পরিমাণ(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(সংরচনা sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (সংরচনা sockaddr *)&server, (পূর্ণাঙ্ক)server_len,
								   &waittime);

    যদি (sd < 0)
    {
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ৱর্ণ *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ঘূৰাই channel;
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
	ধ্রুৱ ৱর্ণ *hostname,
	পূর্ণাঙ্ক port,
	পূর্ণাঙ্ক waittime,
	শূন্য (*nb_close_cb)(শূন্য))
{
    পূর্ণাঙ্ক			sd = -1;
    channel_T		*channel = রিক্ত;
#ifdef FEAT_IPV6
    পূর্ণাঙ্ক			err;
    সংরচনা addrinfo	hints;
    সংরচনা addrinfo	*res = রিক্ত;
    সংরচনা addrinfo	*addr = রিক্ত;
#অন্যথা
    সংরচনা sockaddr_in	server;
    সংরচনা hostent	*host = রিক্ত;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == রিক্ত)
    {
	ch_error(রিক্ত, "Cannot allocate channel.");
	ঘূৰাই রিক্ত;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# যদি defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    যদি ((err = getaddrinfo(hostname, রিক্ত, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    // Count the number of addresses for timeout distribution
    পূর্ণাঙ্ক addr_গুনতি = 0;
    বাবে (addr = res; addr != রিক্ত; addr = addr->ai_next)
	addr_গুনতি++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    যদি (waittime == 0)
	waittime = 15;

    পূর্ণাঙ্ক addr_index = 0;
    বাবে (addr = res; addr != রিক্ত; addr = addr->ai_next, addr_index++)
    {
	ধ্রুৱ ৱর্ণ  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ধ্রুৱ শূন্য  *src = রিক্ত;
	ৱর্ণ	    buf[NUMBUFLEN];
# endif
	পূর্ণাঙ্ক	    try_waittime;
	পূর্ণাঙ্ক	    before_waittime;
	পূর্ণাঙ্ক	    consumed;
	পূর্ণাঙ্ক	    remaining_addrs;

	যদি (addr->ai_family == AF_INET6)
	{
	    সংরচনা sockaddr_in6 *sai = (সংরচনা sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	অন্যথা যদি (addr->ai_family == AF_INET)
	{
	    সংরচনা sockaddr_in *sai = (সংরচনা sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	যদি (src != রিক্ত)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, পরিমাণ(buf));
	    যদি (dst == রিক্ত)
		dst = hostname;
	    অন্যথা যদি (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	যদি (addr->ai_next == রিক্ত)
	    try_waittime = waittime;
	অন্যথা যদি (addr_index == 0)
	{
	    যদি (waittime > 500)
		try_waittime = 250;
	    অন্যথা যদি (waittime > 30)
		try_waittime = waittime / 2;
	    অন্যথা
		try_waittime = waittime;
	}
	অন্যথা
	{
	    remaining_addrs = addr_গুনতি - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (পূর্ণাঙ্ক)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	যদি (waittime < 0)
	    waittime = 0;

	যদি (sd >= 0)
	    ভঙ্গ;

	// If we have no time left, stop trying
	যদি (waittime <= 0 && addr->ai_next != রিক্ত)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    ভঙ্গ;
	}
    }

    freeaddrinfo(res);
#অন্যথা
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    যদি ((host = gethostbyname(hostname)) == রিক্ত)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }
    {
	ৱর্ণ *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], পরিমাণ(p));
	memcpy((ৱর্ণ *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    যদি (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (সংরচনা sockaddr *)&server, পরিমাণ(server),
								   &waittime);
#endif

    যদি (sd < 0)
    {
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ৱর্ণ *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ঘূৰাই channel;
}

    স্থির শূন্য
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    যদি (callback->cb_name != রিক্ত && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    অন্যথা
	cbp->cb_name = রিক্ত;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	স্থির শূন্য
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_মান_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_মান_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    যদি (curbuf->b_ml.ml_mfp == রিক্ত)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    স্থির buf_T *
channel_find_buffer(char_u *name, পূর্ণাঙ্ক err, পূর্ণাঙ্ক বাৰ্তা)
{
    buf_T *buf = রিক্ত;
    buf_T *save_curbuf = curbuf;

    যদি (name != রিক্ত && *name != NUL)
    {
	buf = buflist_findname(name);
	যদি (buf == রিক্ত)
	    buf = buflist_findname_exp(name);
    }

    যদি (buf != রিক্ত)
	ঘূৰাই buf;

    buf = buflist_new(name == রিক্ত || *name == NUL ? রিক্ত : name,
	    রিক্ত, (linenr_T)0, BLN_LISTED | BLN_NEW);
    যদি (buf == রিক্ত)
	ঘূৰাই রিক্ত;
    prepare_buffer(buf);

    curbuf = buf;
    যদি (বাৰ্তা)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    ঘূৰাই buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    স্থির শূন্য
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    যদি (opt->jo_set & JO_MODE)
	বাবে (part = PART_SOCK; part < PART_গুনতি; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    যদি (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    যদি (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    যদি (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    যদি (opt->jo_set & JO_TIMEOUT)
	বাবে (part = PART_SOCK; part < PART_গুনতি; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    যদি (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    যদি (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    যদি (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    যদি (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    যদি (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    যদি (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    যদি (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    যদি ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	যদি (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	যদি (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    যদি (buf == রিক্ত)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক বাৰ্তা = TRUE;

	    যদি (opt->jo_set2 & JO2_OUT_বাৰ্তা)
		বাৰ্তা = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, বাৰ্তা);
	}
	যদি (buf != রিক্ত)
	{
	    যদি (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    যদি (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    অন্যথা
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (ৱর্ণ *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		যদি (buf->b_ml.ml_mfp == রিক্ত)
		    prepare_buffer(buf);
	    }
	}
    }

    যদি ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	যদি (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	যদি (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	অন্যথা যদি (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    যদি (buf == রিক্ত)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক বাৰ্তা = TRUE;

	    যদি (opt->jo_set2 & JO2_ERR_বাৰ্তা)
		বাৰ্তা = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, বাৰ্তা);
	}
	যদি (buf != রিক্ত)
	{
	    যদি (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    যদি (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    অন্যথা
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (ৱর্ণ *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		যদি (buf->b_ml.ml_mfp == রিক্ত)
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
    পূর্ণাঙ্ক
channel_parse_socketserver_address(
	char_u	*address,
	পূর্ণাঙ্ক	*port,
	char_u	**unix_path,
	bool	quiet)
{
    ৱর্ণ    *rest;
    long    val;

    যদি (*address == NUL)
	গচ্ছ fail;

    যদি (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	ঘূৰাই OK;
    }

    val = strtol((ৱর্ণ *)(address), &rest, 10);
    যদি (val < 0 || val >= 65536 || *rest != NUL)
    {
	যদি (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	ঘূৰাই FAIL;
    }
    *port = (পূর্ণাঙ্ক)val;

    ঘূৰাই OK;
fail:
    যদি (!quiet)
	semsg(_(e_invalid_argument_str), address);
    ঘূৰাই FAIL;
}

/*
 * Implements ch_open().
 */
    স্থির channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    ৱর্ণ	*rest;
    পূর্ণাঙ্ক		port = 0;
    পূর্ণাঙ্ক		is_ipv6 = FALSE;
    পূর্ণাঙ্ক		is_unix = FALSE;
    পূর্ণাঙ্ক		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই রিক্ত;

    address = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ঘূৰাই রিক্ত;

    যদি (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	ঘূৰাই রিক্ত;
    }

    যদি (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    অন্যথা যদি (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    অন্যথা যদি (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	যদি (p == রিক্ত || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    ঘূৰাই রিক্ত;
	}
    }
    অন্যথা
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	যদি (p == রিক্ত)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ঘূৰাই রিক্ত;
	}
    }

    যদি (!is_unix && !is_stdio)
    {
	port = strtol((ৱর্ণ *)(p + 1), &rest, 10);
	যদি (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ঘূৰাই রিক্ত;
	}
	যদি (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	অন্যথা
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    যদি (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	গচ্ছ theend;
    যদি (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	গচ্ছ theend;
    }

    যদি (is_stdio)
	channel = channel_open_stdio();
    অন্যথা যদি (is_unix)
	channel = channel_open_unix((ৱর্ণ *)address, রিক্ত);
    অন্যথা
	channel = channel_open((ৱর্ণ *)address, port, opt.jo_waittime, রিক্ত);
    যদি (channel != রিক্ত)
    {
	পূর্ণাঙ্ক	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	যদি (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ঘূৰাই channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
স্থির channel_T *stdio_channel = রিক্ত;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    স্থির channel_T *
channel_open_stdio(শূন্য)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    যদি (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	ঘূৰাই রিক্ত;
    }
    যদি (stdio_channel != রিক্ত)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ঘূৰাই রিক্ত;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	পূর্ণাঙ্ক	nul_fd;

	যদি (!DuplicateHandle(proc, GetStdHandle(STD_প্ৰবেশ_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ঘূৰাই রিক্ত;
	}
	যদি (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ঘূৰাই রিক্ত;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, রিক্ত, OPEN_EXISTING, 0,
		রিক্ত);
	যদি (hnul != INVALID_HANDLE_মান)
	{
	    SetStdHandle(STD_প্ৰবেশ_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	যদি (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#অন্যথা
    {
	পূর্ণাঙ্ক	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	যদি (in_fd < 0 || out_fd < 0)
	{
	    যদি (in_fd >= 0)
		close(in_fd);
	    যদি (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ঘূৰাই রিক্ত;
	}
	(শূন্য)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(শূন্য)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	যদি (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    যদি (channel == রিক্ত)
    {
	ch_error(রিক্ত, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	ঘূৰাই রিক্ত;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ঘূৰাই channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    ৱর্ণ	*rest;
    পূর্ণাঙ্ক		port;
    পূর্ণাঙ্ক		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = রিক্ত;

    যদি (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ঘূৰাই রিক্ত;

    arg = tv_get_string(&argvars[0]);
    যদি (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	ঘূৰাই রিক্ত;
    }

    যদি (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    অন্যথা
    {
	port = strtol((ৱর্ণ *)(arg), &rest, 10);
	যদি (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ঘূৰাই রিক্ত;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    যদি (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	গচ্ছ theend;
    যদি (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	গচ্ছ theend;
    }

    যদি (is_unix)
	channel = channel_listen_unix((ৱর্ণ *)arg, রিক্ত, true);
    অন্যথা
	channel = channel_listen(port, রিক্ত);
    যদি (channel != রিক্ত)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ঘূৰাই channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	পূর্ণাঙ্ক port_in,
	শূন্য (*nb_close_cb)(শূন্য))
{
    পূর্ণাঙ্ক			sd = -1;
    সংরচনা sockaddr_in	server;
#ifndef FEAT_IPV6
    সংরচনা hostent	*host;
#endif
    পূর্ণাঙ্ক			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == রিক্ত)
    {
	ch_error(রিক্ত, "Cannot allocate channel.");
	ঘূৰাই রিক্ত;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((ৱর্ণ *)&server, 0, পরিমাণ(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    যদি (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

#ifdef MSWIN
    যদি (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ধ্রুৱ ৱর্ণ *)&val, পরিমাণ(val)) < 0)
#অন্যথা
    যদি (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, পরিমাণ(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    // Bind the socket to the port
    যদি (bind(sd, (সংরচনা sockaddr *)&server, পরিমাণ(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    যদি (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    যদি (port_in == 0)
    {
	সংরচনা sockaddr_in	addr;
	socklen_t		addr_len = পরিমাণ(addr);

	যদি (getsockname(sd, (সংরচনা sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ৱর্ণ *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ঘূৰাই channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	ৱর্ণ *path,
	শূন্য (*nb_close_cb)(শূন্য),
	bool replace)
{
    পূর্ণাঙ্ক			sd = -1;
    সংরচনা sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    যদি (path == রিক্ত || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == রিক্ত ? (ৱর্ণ *)"" : path);
	ঘূৰাই রিক্ত;
    }

    path_len = STRLEN(path);
    যদি (path_len >= পরিমাণ(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ঘূৰাই রিক্ত;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == রিক্ত)
    {
	ch_error(রিক্ত, "Cannot allocate channel.");
	ঘূৰাই রিক্ত;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, পরিমাণ(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    যদি (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    যদি (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(সংরচনা sockaddr_un, sun_path) + path_len + 1;
    যদি (bind(sd, (সংরচনা sockaddr *)&server, (পূর্ণাঙ্ক)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    যদি (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ঘূৰাই রিক্ত;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ৱর্ণ *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ঘূৰাই channel;
}

     শূন্য
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    যদি (*fd == INVALID_FD)
	ঘূৰাই;

    যদি (part == PART_SOCK)
	sock_close(*fd);
    অন্যথা
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	যদি ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    যদি (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    শূন্য
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    যদি (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#যদি defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	যদি (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    যদি (out != INVALID_FD)
    {
#যদি defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#যদি defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    যদি (err != INVALID_FD)
    {
#যদি defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#যদি defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    শূন্য
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    যদি (job->jv_in_buf == রিক্ত)
	ঘূৰাই;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (ৱর্ণ *)in_part->ch_bufref.br_buf->b_ffname);
    যদি (options->jo_set & JO_IN_শীৰ্ষ)
    {
	যদি (options->jo_in_শীৰ্ষ == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_শীৰ্ষ =
		in_part->ch_bufref.br_buf->b_ml.ml_সাৰি_গুনতি + 1;
	}
	অন্যথা
	    in_part->ch_buf_শীৰ্ষ = options->jo_in_শীৰ্ষ;
    }
    অন্যথা
	in_part->ch_buf_শীৰ্ষ = 1;
    যদি (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    অন্যথা
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_সাৰি_গুনতি;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    স্থির শূন্য
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	পূর্ণাঙ্ক	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    যদি (item == রিক্ত)
	ঘূৰাই;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = রিক্ত;
    যদি (item->cq_prev == রিক্ত)
	head->cq_next = item;
    অন্যথা
	item->cq_prev->cq_next = item;
}

    স্থির শূন্য
write_buf_সাৰি(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *সাৰি = ml_get_buf(buf, lnum, FALSE);
    পূর্ণাঙ্ক	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    পূর্ণাঙ্ক	    i;

    // Need to make a copy to be able to append a NL.
    যদি ((p = alloc(len + 2)) == রিক্ত)
	ঘূৰাই;
    memcpy((ৱর্ণ *)p, (ৱর্ণ *)সাৰি, len);

    যদি (channel->ch_write_text_mode)
	p[len] = CAR;
    অন্যথা
    {
	বাবে (i = 0; i < len; ++i)
	    যদি (p[i] == NL)
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
    স্থির পূর্ণাঙ্ক
can_write_buf_সাৰি(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    যদি (in_part->ch_fd == INVALID_FD)
	ঘূৰাই FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    যদি (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    অন্যথা যদি (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# যদি defined(HAVE_SELECT)
	সংরচনা timeval	tval;
	fd_set		wfds;
	পূর্ণাঙ্ক		ret;

	FD_ZERO(&wfds);
	FD_SET((পূর্ণাঙ্ক)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	বাবে (;;)
	{
	    ret = select((পূর্ণাঙ্ক)in_part->ch_fd + 1, রিক্ত, &wfds, রিক্ত, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    যদি (ret == -1 && errno == EINTR)
		অনুৱর্ত;
#  endif
	    যদি (ret <= 0 || in_part->ch_block_write == 1)
	    {
		যদি (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		অন্যথা
		    ch_log(channel, "Input not ready for writing");
		ঘূৰাই FALSE;
	    }
	    ভঙ্গ;
	}
# অন্যথা
	সংরচনা pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	যদি (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ঘূৰাই FALSE;
	}
	যদি (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ঘূৰাই FALSE;
	}
# endif
    }
#endif
    ঘূৰাই TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    শূন্য
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    পূর্ণাঙ্ক		written = 0;

    যদি (buf == রিক্ত || in_part->ch_buf_append)
	ঘূৰাই;  // no buffer or using appending
    যদি (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == রিক্ত)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = রিক্ত;
	ঘূৰাই;
    }

    বাবে (lnum = in_part->ch_buf_শীৰ্ষ; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_সাৰি_গুনতি; ++lnum)
    {
	যদি (!can_write_buf_সাৰি(channel))
	    ভঙ্গ;
	write_buf_সাৰি(buf, lnum, channel);
	++written;
    }

    যদি (written == 1)
	ch_log(channel, "written line %d to channel", (পূর্ণাঙ্ক)lnum - 1);
    অন্যথা যদি (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_শীৰ্ষ = lnum;
    যদি (lnum > buf->b_ml.ml_সাৰি_গুনতি || lnum > in_part->ch_buf_bot)
    {
#যদি defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	যদি (channel->ch_job != রিক্ত)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = রিক্ত;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	যদি (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    অন্যথা
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_সাৰি_গুনতি - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    শূন্য
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	বাবে (part = PART_SOCK; part < PART_গুনতি; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    যদি (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = রিক্ত;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    শূন্য
channel_write_প্ৰবেশ(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    যদি (in_part->ch_writeque.wq_next != রিক্ত)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    অন্যথা যদি (in_part->ch_bufref.br_buf != রিক্ত)
    {
	যদি (in_part->ch_buf_append)
	    channel_write_new_সাৰিগুলি(in_part->ch_bufref.br_buf);
	অন্যথা
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    শূন্য
channel_write_any_সাৰিগুলি(শূন্য)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_প্ৰবেশ(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    শূন্য
channel_write_new_সাৰিগুলি(buf_T *buf)
{
    channel_T	*channel;
    পূর্ণাঙ্ক		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	পূর্ণাঙ্ক	    written = 0;

	যদি (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    যদি (in_part->ch_fd == INVALID_FD)
		অনুৱর্ত;  // pipe was closed
	    found_one = TRUE;
	    বাবে (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_সাৰি_গুনতি;
								       ++lnum)
	    {
		যদি (!can_write_buf_সাৰি(channel))
		    ভঙ্গ;
		write_buf_সাৰি(buf, lnum, channel);
		++written;
	    }

	    যদি (written == 1)
		ch_log(channel, "written line %d to channel", (পূর্ণাঙ্ক)lnum - 1);
	    অন্যথা যদি (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    যদি (lnum < buf->b_ml.ml_সাৰি_গুনতি)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_সাৰি_গুনতি - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    যদি (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    স্থির শূন্য
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    যদি (safe_to_invoke_callback == 0)
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

    ঘূৰাই head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *গ্ৰন্থি)
{
    char_u  *buffer = গ্ৰন্থি->rq_buffer;
    long_u  i;

    বাবে (i = 0; i < গ্ৰন্থি->rq_buflen; ++i)
	যদি (buffer[i] == NL)
	    ঘূৰাই buffer + i;
    ঘূৰাই রিক্ত;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, পূর্ণাঙ্ক *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *গ্ৰন্থি = head->rq_next;
    char_u *p;

    যদি (গ্ৰন্থি == রিক্ত)
	ঘূৰাই রিক্ত;
    যদি (outlen != রিক্ত)
	*outlen += গ্ৰন্থি->rq_buflen;
    // dispose of the node but keep the buffer
    p = গ্ৰন্থি->rq_buffer;
    head->rq_next = গ্ৰন্থি->rq_next;
    যদি (গ্ৰন্থি->rq_next == রিক্ত)
	head->rq_prev = রিক্ত;
    অন্যথা
	গ্ৰন্থি->rq_next->rq_prev = রিক্ত;
    vim_free(গ্ৰন্থি);
    ঘূৰাই p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    স্থির char_u *
channel_get_all(channel_T *channel, ch_part_T part, পূর্ণাঙ্ক *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *গ্ৰন্থি;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    বাবে (গ্ৰন্থি = head->rq_next; গ্ৰন্থি != রিক্ত; গ্ৰন্থি = গ্ৰন্থি->rq_next)
	len += গ্ৰন্থি->rq_buflen;
    res = alloc(len + 1);
    যদি (res == রিক্ত)
	ঘূৰাই রিক্ত;
    p = res;
    বাবে (গ্ৰন্থি = head->rq_next; গ্ৰন্থি != রিক্ত; গ্ৰন্থি = গ্ৰন্থি->rq_next)
    {
	mch_memmove(p, গ্ৰন্থি->rq_buffer, গ্ৰন্থি->rq_buflen);
	p += গ্ৰন্থি->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    কৰা
    {
	p = channel_get(channel, part, রিক্ত);
	vim_free(p);
    } যেতিয়ালৈ (p != রিক্ত);

    যদি (outlen != রিক্ত)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ঘূৰাই res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    যেতিয়ালৈ (p < res + len)
    {
	যদি (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	অন্যথা যদি (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    যদি (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		যেতিয়ালৈ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		যদি (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    ঘূৰাই res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    শূন্য
channel_consume(channel_T *channel, ch_part_T part, পূর্ণাঙ্ক len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *গ্ৰন্থি = head->rq_next;
    char_u *buf = গ্ৰন্থি->rq_buffer;

    যদি (len < 0 || (long_u)len > গ্ৰন্থি->rq_buflen)
	ঘূৰাই;
    mch_memmove(buf, buf + len, গ্ৰন্থি->rq_buflen - len);
    গ্ৰন্থি->rq_buflen -= len;
    গ্ৰন্থি->rq_buffer[গ্ৰন্থি->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    পূর্ণাঙ্ক
channel_collapse(channel_T *channel, ch_part_T part, পূর্ণাঙ্ক want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*গ্ৰন্থি = head->rq_next;
    readq_T	*last_গ্ৰন্থি;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    যদি (গ্ৰন্থি == রিক্ত || গ্ৰন্থি->rq_next == রিক্ত)
	ঘূৰাই FAIL;

    last_গ্ৰন্থি = গ্ৰন্থি->rq_next;
    len = গ্ৰন্থি->rq_buflen + last_গ্ৰন্থি->rq_buflen;
    যদি (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	যেতিয়ালৈ (last_গ্ৰন্থি->rq_next != রিক্ত
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_গ্ৰন্থি) == রিক্ত))
	{
	    last_গ্ৰন্থি = last_গ্ৰন্থি->rq_next;
	    len += last_গ্ৰন্থি->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    যদি (newbuf == রিক্ত)
	ঘূৰাই FAIL;	    // out of memory
    mch_memmove(p, গ্ৰন্থি->rq_buffer, গ্ৰন্থি->rq_buflen);
    p += গ্ৰন্থি->rq_buflen;
    vim_free(গ্ৰন্থি->rq_buffer);
    গ্ৰন্থি->rq_buffer = newbuf;
    বাবে (n = গ্ৰন্থি; n != last_গ্ৰন্থি; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    গ্ৰন্থি->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    বাবে (n = গ্ৰন্থি->rq_next; n != last_গ্ৰন্থি; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    গ্ৰন্থি->rq_next = last_গ্ৰন্থি->rq_next;
    যদি (last_গ্ৰন্থি->rq_next == রিক্ত)
	head->rq_prev = গ্ৰন্থি;
    অন্যথা
	last_গ্ৰন্থি->rq_next->rq_prev = গ্ৰন্থি;
    vim_free(last_গ্ৰন্থি);
    ঘূৰাই OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
channel_save(channel_T *channel, ch_part_T part, char_u *buf, পূর্ণাঙ্ক len,
						      পূর্ণাঙ্ক prepend, ৱর্ণ *lead)
{
    readq_T *গ্ৰন্থি;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    পূর্ণাঙ্ক	    i;

    গ্ৰন্থি = ALLOC_ONE(readq_T);
    যদি (গ্ৰন্থি == রিক্ত)
	ঘূৰাই FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    গ্ৰন্থি->rq_buffer = alloc(len + 1);
    যদি (গ্ৰন্থি->rq_buffer == রিক্ত)
    {
	vim_free(গ্ৰন্থি);
	ঘূৰাই FAIL;	    // out of memory
    }

    যদি (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = গ্ৰন্থি->rq_buffer;
	বাবে (i = 0; i < len; ++i)
	    যদি (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	গ্ৰন্থি->rq_buflen = (long_u)(p - গ্ৰন্থি->rq_buffer);
    }
    অন্যথা
    {
	mch_memmove(গ্ৰন্থি->rq_buffer, buf, len);
	গ্ৰন্থি->rq_buffer[len] = NUL;
	গ্ৰন্থি->rq_buflen = (long_u)len;
    }

    যদি (prepend)
    {
	// prepend node to the head of the queue
	গ্ৰন্থি->rq_next = head->rq_next;
	গ্ৰন্থি->rq_prev = রিক্ত;
	যদি (head->rq_next == রিক্ত)
	    head->rq_prev = গ্ৰন্থি;
	অন্যথা
	    head->rq_next->rq_prev = গ্ৰন্থি;
	head->rq_next = গ্ৰন্থি;
    }
    অন্যথা
    {
	// append node to the tail of the queue
	গ্ৰন্থি->rq_next = রিক্ত;
	গ্ৰন্থি->rq_prev = head->rq_prev;
	যদি (head->rq_prev == রিক্ত)
	    head->rq_next = গ্ৰন্থি;
	অন্যথা
	    head->rq_prev->rq_next = গ্ৰন্থি;
	head->rq_prev = গ্ৰন্থি;
    }

    যদি (ch_log_active() && lead != রিক্ত)
	ch_log_literal(lead, channel, part, buf, len);

    ঘূৰাই OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    পূর্ণাঙ্ক
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, রিক্ত);
    পূর্ণাঙ্ক		keeplen;
    পূর্ণাঙ্ক		addlen;
    char_u	*p;

    যদি (next == রিক্ত)
	ঘূৰাই FALSE;

    keeplen = reader->js_end - reader->js_buf;
    যদি (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (পূর্ণাঙ্ক)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	যদি (p == রিক্ত)
	{
	    vim_free(next);
	    ঘূৰাই FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    ঘূৰাই TRUE;
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
    স্থির পূর্ণাঙ্ক
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*সাৰি_start;
    char_u	*p;
    int_u	hdr_len;
    পূর্ণাঙ্ক		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    যেতিয়ালৈ (TRUE)
    {
	সাৰি_start = p;
	যেতিয়ালৈ (*p != NUL && *p != '\n')
	    p++;
	যদি (*p == NUL)			// partial header
	    ঘূৰাই MAYBE;
	p++;

	// process the content length field (if present)
	যদি ((p - সাৰি_start > 16)
		&& STRNICMP(সাৰি_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((ৱর্ণ *)সাৰি_start + 16, রিক্ত, 10);
	    যদি (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ঘূৰাই FAIL;
	}

	যদি ((p - সাৰি_start) == 2 && সাৰি_start[0] == '\r' &&
		সাৰি_start[1] == '\n')
	    // reached the empty line
	    ভঙ্গ;
    }

    যদি (payload_len == -1)
	// Content-Length field is not present in the header
	ঘূৰাই FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    যদি (jsbuf_len < hdr_len + payload_len)
	ঘূৰাই MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    ঘূৰাই OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    পূর্ণাঙ্ক
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    পূর্ণাঙ্ক		status = OK;
    পূর্ণাঙ্ক		ret;

    যদি (channel_peek(channel, part) == রিক্ত)
	ঘূৰাই FALSE;

    reader.js_buf = channel_get(channel, part, রিক্ত);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    যদি (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    যদি (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    যদি (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	যদি (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	অন্যথা যদি (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    যদি (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    অন্যথা
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	অন্যথা
	{
	    item = ALLOC_ONE(jsonq_T);
	    যদি (item == রিক্ত)
		clear_tv(&listtv);
	    অন্যথা
	    {
		item->jq_no_callback = FALSE;
		item->jq_মান = alloc_tv();
		যদি (item->jq_মান == রিক্ত)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		অন্যথা
		{
		    *item->jq_মান = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = রিক্ত;
		    যদি (item->jq_prev == রিক্ত)
			head->jq_next = item;
		    অন্যথা
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    যদি (status == OK)
	chanpart->ch_wait_len = 0;
    অন্যথা যদি (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	যদি (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (পূর্ণাঙ্ক)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_গুনতি() + 100L;
#অন্যথা
	    gettimeofday(&chanpart->ch_deadline, রিক্ত);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    যদি (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক timeout;
#ifdef MSWIN
	    timeout = (পূর্ণাঙ্ক)(Get_Tick_গুনতি() - chanpart->ch_deadline) > 0;
#অন্যথা
	    {
		সংরচনা timeval now_tv;

		gettimeofday(&now_tv, রিক্ত);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    যদি (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    অন্যথা
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    যদি (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    অন্যথা যদি (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(পূর্ণাঙ্ক)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, রিক্ত);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    অন্যথা
	ret = FALSE;

    vim_free(reader.js_buf);
    ঘূৰাই ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    স্থির শূন্য
remove_cb_গ্ৰন্থি(cbq_T *head, cbq_T *গ্ৰন্থি)
{
    যদি (গ্ৰন্থি->cq_prev == রিক্ত)
	head->cq_next = গ্ৰন্থি->cq_next;
    অন্যথা
	গ্ৰন্থি->cq_prev->cq_next = গ্ৰন্থি->cq_next;
    যদি (গ্ৰন্থি->cq_next == রিক্ত)
	head->cq_prev = গ্ৰন্থি->cq_prev;
    অন্যথা
	গ্ৰন্থি->cq_next->cq_prev = গ্ৰন্থি->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    শূন্য
remove_json_গ্ৰন্থি(jsonq_T *head, jsonq_T *গ্ৰন্থি)
{
    যদি (গ্ৰন্থি->jq_prev == রিক্ত)
	head->jq_next = গ্ৰন্থি->jq_next;
    অন্যথা
	গ্ৰন্থি->jq_prev->jq_next = গ্ৰন্থি->jq_next;
    যদি (গ্ৰন্থি->jq_next == রিক্ত)
	head->jq_prev = গ্ৰন্থি->jq_prev;
    অন্যথা
	গ্ৰন্থি->jq_next->jq_prev = গ্ৰন্থি->jq_prev;
    vim_free(গ্ৰন্থি);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    স্থির শূন্য
channel_add_block_id(chanpart_T *chanpart, পূর্ণাঙ্ক id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    যদি (gap->ga_growsize == 0)
	ga_init2(gap, পরিমাণ(পূর্ণাঙ্ক), 10);
    যদি (ga_grow(gap, 1) == OK)
    {
	((পূর্ণাঙ্ক *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    স্থির শূন্য
channel_remove_block_id(chanpart_T *chanpart, পূর্ণাঙ্ক id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < gap->ga_len; ++i)
	যদি (((পূর্ণাঙ্ক *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    যদি (i < gap->ga_len)
	    {
		পূর্ণাঙ্ক *p = ((পূর্ণাঙ্ক *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * পরিমাণ(পূর্ণাঙ্ক));
	    }
	    ঘূৰাই;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    স্থির পূর্ণাঙ্ক
channel_has_block_id(chanpart_T *chanpart, পূর্ণাঙ্ক id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    পূর্ণাঙ্ক		i;

    বাবে (i = 0; i < gap->ga_len; ++i)
	যদি (((পূর্ণাঙ্ক *)gap->ga_data)[i] == id)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
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
    স্থির পূর্ণাঙ্ক
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	পূর্ণাঙ্ক	    id,
	পূর্ণাঙ্ক	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    যেতিয়ালৈ (item != রিক্ত)
    {
	list_T	    *l;
	typval_T    *tv;

	যদি (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_মান->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	অন্যথা
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    যদি (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		যদি (id > 0)
		{
		    যদি (item->jq_মান->v_type != VAR_DICT)
			গচ্ছ nextitem;
		    d = item->jq_মান->vval.v_dict;
		    যদি (d == রিক্ত)
			গচ্ছ nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    যদি (dict_has_চাবি(d, "method"))
			গচ্ছ nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    যদি (di == রিক্ত)
			গচ্ছ nextitem;
		    tv = &di->di_tv;
		}
		অন্যথা
		    tv = item->jq_মান;
	    }
	    অন্যথা
	    {
		যদি (id > 0)
		{
		    যদি (item->jq_মান->v_type != VAR_DICT)
			গচ্ছ nextitem;
		    d = item->jq_মান->vval.v_dict;
		    যদি (d == রিক্ত)
			গচ্ছ nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    যদি (di == রিক্ত)
			গচ্ছ nextitem;
		    tv = &di->di_tv;
		}
		অন্যথা
		    tv = item->jq_মান;
	    }
	}

	যদি ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_সংখ্যা && tv->vval.v_সংখ্যা == id)
	      || (id <= 0 && (tv->v_type != VAR_সংখ্যা
		 || tv->vval.v_সংখ্যা == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_সংখ্যা)))))
	{
	    *rettv = item->jq_মান;
	    যদি (tv->v_type == VAR_সংখ্যা)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_সংখ্যা);
	    remove_json_গ্ৰন্থি(head, item);
	    ঘূৰাই OK;
	}
nextitem:
	item = item->jq_next;
    }
    ঘূৰাই FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    স্থির শূন্য
channel_ঠেলো_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    যদি (head->jq_prev != রিক্ত && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = রিক্ত;
    অন্যথা যেতিয়ালৈ (item != রিক্ত && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    যদি (newitem == রিক্ত)
    {
	clear_tv(rettv);
	ঘূৰাই;
    }

    newitem->jq_মান = alloc_tv();
    যদি (newitem->jq_মান == রিক্ত)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ঘূৰাই;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_মান = *rettv;
    যদি (item == রিক্ত)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = রিক্ত;
	যদি (newitem->jq_prev == রিক্ত)
	    head->jq_next = newitem;
	অন্যথা
	    newitem->jq_prev->jq_next = newitem;
    }
    অন্যথা
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	যদি (newitem->jq_next == রিক্ত)
	    head->jq_prev = newitem;
	অন্যথা
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    স্থির শূন্য
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    পূর্ণাঙ্ক	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    যদি (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	যদি (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	ঘূৰাই;
    }
    arg = argv[1].vval.v_string;
    যদি (arg == রিক্ত)
	arg = (char_u *)"";

    যদি (STRCMP(cmd, "ex") == 0)
    {
	পূর্ণাঙ্ক	called_emsg_before = called_emsg;
	char_u	*p = arg;
	পূর্ণাঙ্ক	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (ৱর্ণ *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	যদি (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	যদি (do_emsg_silent)
	    --emsg_silent;
	যদি (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (ৱর্ণ *)get_vim_var_str(VV_ERRMSG));
    }
    অন্যথা যদি (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (ৱর্ণ *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_গুনতি = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    অন্যথা যদি (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    অন্যথা যদি (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	পূর্ণাঙ্ক is_call = cmd[0] == 'c';
	পূর্ণাঙ্ক id_idx = is_call ? 3 : 2;

	যদি (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_সংখ্যা)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    যদি (p_verbose > 2)
		emsg(_(e_last_argument_for_ৰাশিমালা_call_must_be_সংখ্যা));
	}
	অন্যথা যদি (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    যদি (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	অন্যথা
	{
	    typval_T	*tv = রিক্ত;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = রিক্ত;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    যদি (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (ৱর্ণ *)arg);
		tv = eval_ৰাশিমালা(arg, রিক্ত);
	    }
	    অন্যথা
	    {
		ch_log(channel, "Calling '%s'", (ৱর্ণ *)arg);
		যদি (func_call(arg, &argv[2], রিক্ত, রিক্ত, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    যদি (argv[id_idx].v_type == VAR_সংখ্যা)
	    {
		পূর্ণাঙ্ক id = argv[id_idx].vval.v_সংখ্যা;

		যদি (tv != রিক্ত)
		    json = json_encode_nr_ৰাশিমালা(id, tv, options | JSON_NL);
		যদি (tv == রিক্ত || (json != রিক্ত && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_ৰাশিমালা(id, &err_tv, options | JSON_NL);
		}
		যদি (json != রিক্ত)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (পূর্ণাঙ্ক)STRLEN(json), (ৱর্ণ *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    যদি (tv == &res_tv)
		clear_tv(tv);
	    অন্যথা
		free_tv(tv);
	}
    }
    অন্যথা যদি (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (ৱর্ণ *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    স্থির শূন্য
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (ৱর্ণ *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_গ্ৰন্থি(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    স্থির শূন্য
append_to_buffer(
    buf_T	*buffer,
    char_u	*বাৰ্তা,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_সাৰি_গুনতি;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    পূর্ণাঙ্ক		save_p_ma = buffer->b_p_ma;
    পূর্ণাঙ্ক		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    যদি (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	যদি (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ঘূৰাই;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    যদি (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (পূর্ণাঙ্ক)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    যদি (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ঘূৰাই;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    যদি (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, বাৰ্তা, TRUE);
	lnum = 0;
    }
    অন্যথা
	ml_append(lnum, বাৰ্তা, 0, FALSE);
    appended_সাৰিগুলি_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    যদি (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    অন্যথা
	buffer->b_p_ma = save_p_ma;

    যদি (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_buffer == buffer)
	    {
		পূর্ণাঙ্ক move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		যদি (move_cursor || wp->w_topline > buffer->b_ml.ml_সাৰি_গুনতি)
		{
		    win_T *save_curwin = curwin;

		    যদি (move_cursor)
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

    যদি (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    যদি (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_সাৰি_গুনতি;
	}
    }
}

    স্থির শূন্য
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *বাৰ্তা;

    যেতিয়ালৈ ((বাৰ্তা = channel_get(channel, part, রিক্ত)) != রিক্ত)
    {
	ch_log(channel, "Dropping message '%s'", (ৱর্ণ *)বাৰ্তা);
	vim_free(বাৰ্তা);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    স্থির পূর্ণাঙ্ক
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    ঘূৰাই ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    স্থির পূর্ণাঙ্ক
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*বাৰ্তা = রিক্ত;
    blob_T	*blob = রিক্ত;
    typval_T	*listtv = রিক্ত;
    typval_T	argv[CH_JSON_MAX_ARGS];
    পূর্ণাঙ্ক		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = রিক্ত;
    buf_T	*buffer = রিক্ত;
    char_u	*p;
    পূর্ণাঙ্ক		called_otc;		// one time callbackup
    পূর্ণাঙ্ক		raw_len = 0;

    যদি (channel->ch_nb_close_cb != রিক্ত
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ঘূৰাই FALSE;

    // Use a message-specific callback, part callback or channel callback
    বাবে (cbitem = cbhead->cq_next; cbitem != রিক্ত; cbitem = cbitem->cq_next)
	যদি (cbitem->cq_seq_nr == 0)
	    ভঙ্গ;
    যদি (cbitem != রিক্ত)
	callback = &cbitem->cq_callback;
    অন্যথা যদি (ch_part->ch_callback.cb_name != রিক্ত)
	callback = &ch_part->ch_callback;
    অন্যথা যদি (channel->ch_callback.cb_name != রিক্ত)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    যদি (buffer != রিক্ত && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == রিক্ত))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = রিক্ত;
	buffer = রিক্ত;
    }

    যদি (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	পূর্ণাঙ্ক		argc = 0;

	// Get any json message in the queue.
	যদি (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    যদি (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(শূন্য)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    যদি (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		ঘূৰাই FALSE;
	}

	যদি (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    যদি (d != রিক্ত)
	    {
		যদি (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		অন্যথা
		    di = dict_find(d, (char_u *)"request_seq", -1);
		যদি (di != রিক্ত && di->di_tv.v_type == VAR_সংখ্যা)
		    seq_nr = di->di_tv.vval.v_সংখ্যা;
	    }

	    argv[1] = *listtv;
	}
	অন্যথা
	{
	    বাবে (item = listtv->vval.v_list->lv_first;
		    item != রিক্ত && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    যেতিয়ালৈ (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    যদি (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ঘূৰাই TRUE;
	    }

	    যদি (argv[0].v_type != VAR_সংখ্যা)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ঘূৰাই FALSE;
	    }
	    seq_nr = argv[0].vval.v_সংখ্যা;
	}
    }
    অন্যথা যদি (channel_peek(channel, part) == রিক্ত)
    {
	// nothing to read on RAW or NL channel
	ঘূৰাই FALSE;
    }
    অন্যথা
    {
	// If there is no callback or buffer drop the message.
	যদি (callback == রিক্ত && buffer == রিক্ত)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    যদি (channel->ch_close_cb.cb_name == রিক্ত && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ঘূৰাই FALSE;
	}

	যদি (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = রিক্ত;
	    char_u  *buf;
	    readq_T *গ্ৰন্থি;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    যেতিয়ালৈ (TRUE)
	    {
		গ্ৰন্থি = channel_peek(channel, part);
		nl = channel_first_nl(গ্ৰন্থি);
		যদি (nl != রিক্ত)
		    ভঙ্গ;
		যদি (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    যদি (ch_part->ch_fd == INVALID_FD && গ্ৰন্থি->rq_buflen > 0)
			ভঙ্গ;
		    ঘূৰাই FALSE; // incomplete message
		}
	    }
	    buf = গ্ৰন্থি->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    বাবে (p = buf; (nl == রিক্ত || p < nl)
					    && p < buf + গ্ৰন্থি->rq_buflen; ++p)
		যদি (*p == NUL)
		    *p = NL;

	    যদি (nl == রিক্ত)
	    {
		// get the whole buffer, drop the NL
		বাৰ্তা = channel_get(channel, part, রিক্ত);
	    }
	    অন্যথা যদি (nl + 1 == buf + গ্ৰন্থি->rq_buflen)
	    {
		// get the whole buffer
		বাৰ্তা = channel_get(channel, part, রিক্ত);
		*nl = NUL;
	    }
	    অন্যথা
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		বাৰ্তা = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (পূর্ণাঙ্ক)(nl - buf) + 1);
	    }
	}
	অন্যথা
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    বাৰ্তা = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : রিক্ত);
	}

	যদি (বাৰ্তা == রিক্ত)
	    ঘূৰাই FALSE; // out of memory (and avoids Coverity warning)

	যদি (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    যদি (blob == রিক্ত)
	    {
		vim_free(বাৰ্তা);
		ঘূৰাই FALSE;
	    }
	    যদি (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(বাৰ্তা);
		ঘূৰাই FALSE;
	    }
	    যদি (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, বাৰ্তা, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	অন্যথা
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = বাৰ্তা;
	}
    }

    called_otc = FALSE;
    যদি (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	পূর্ণাঙ্ক lsp_req_বাৰ্তা = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	যদি ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_চাবি(argv[1].vval.v_dict, "method"))
	    lsp_req_বাৰ্তা = TRUE;

	যদি (!lsp_req_বাৰ্তা)
	{
	    বাবে (cbitem = cbhead->cq_next; cbitem != রিক্ত;
		    cbitem = cbitem->cq_next)
	    {
		যদি (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    ভঙ্গ;
		}
	    }
	}
    }

    যদি (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	যদি (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    যদি (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ঠেলো_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_সংখ্যা;
		free_tv(listtv);
		listtv = রিক্ত;
	    }
	    অন্যথা
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    অন্যথা যদি (callback != রিক্ত || buffer != রিক্ত)
    {
	যদি (buffer != রিক্ত)
	{
	    যদি (বাৰ্তা == রিক্ত)
		// JSON or JS mode: re-encode the message.
		বাৰ্তা = json_encode(listtv, ch_mode);
	    যদি (বাৰ্তা != রিক্ত)
	    {
#ifdef FEAT_TERMINAL
		যদি (buffer->b_term != রিক্ত)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    যদি (part == PART_ERR)
		    {
			char_u *cp;
			পূর্ণাঙ্ক    extra = 0;

			বাবে (cp = বাৰ্তা; *cp != NUL; ++cp)
			    যদি (*cp == NL && (cp == বাৰ্তা || cp[-1] != CAR))
				++extra;
			যদি (extra > 0)
			{
			    char_u *crlf_বাৰ্তা = alloc(STRLEN(বাৰ্তা) + extra + 1);

			    যদি (crlf_বাৰ্তা != রিক্ত)
			    {
				char_u *q = crlf_বাৰ্তা;

				বাবে (cp = বাৰ্তা; *cp != NUL; ++cp)
				{
				    যদি (*cp == NL && (cp == বাৰ্তা || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_বাৰ্তা, channel);
				vim_free(crlf_বাৰ্তা);
			    }
			}
			অন্যথা
			    write_to_term(buffer, বাৰ্তা, channel);
		    }
		    অন্যথা
			write_to_term(buffer, বাৰ্তা, channel);
		}
		অন্যথা
#endif
		    append_to_buffer(buffer, বাৰ্তা, channel, part);
	    }
	}

	যদি (callback != রিক্ত)
	{
	    যদি (cbitem != রিক্ত)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    অন্যথা
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (ৱর্ণ *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    অন্যথা
	ch_log(channel, "Dropping message %d", seq_nr);

    যদি (listtv != রিক্ত)
	free_tv(listtv);
    যদি (blob != রিক্ত)
	blob_unref(blob);
    vim_free(বাৰ্তা);

    ঘূৰাই TRUE;
}

#যদি defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    পূর্ণাঙ্ক
channel_can_write_to(channel_T *channel)
{
    ঘূৰাই channel != রিক্ত && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    পূর্ণাঙ্ক
channel_is_open(channel_T *channel)
{
    ঘূৰাই channel != রিক্ত && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    স্থির শূন্য *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    যদি (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	যদি (head->jq_next == রিক্ত)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ঘূৰাই head->jq_next;
    }
    ঘূৰাই channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    স্থির পূর্ণাঙ্ক
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ঘূৰাই channel_readahead_pointer(channel, part) != রিক্ত;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    স্থির ৱর্ণ *
channel_status(channel_T *channel, পূর্ণাঙ্ক req_part)
{
    ch_part_T part;
    পূর্ণাঙ্ক has_readahead = FALSE;

    যদি (channel == রিক্ত)
	 ঘূৰাই "fail";
    যদি (req_part == PART_OUT)
    {
	যদি (channel->CH_OUT_FD != INVALID_FD)
	    ঘূৰাই "open";
	যদি (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    অন্যথা যদি (req_part == PART_ERR)
    {
	যদি (channel->CH_ERR_FD != INVALID_FD)
	    ঘূৰাই "open";
	যদি (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    অন্যথা
    {
	যদি (channel_is_open(channel))
	    ঘূৰাই "open";
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	    যদি (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		ভঙ্গ;
	    }
    }

    যদি (has_readahead)
	ঘূৰাই "buffered";
    ঘূৰাই "closed";
}

    স্থির শূন্য
channel_part_info(channel_T *channel, dict_T *dict, ৱর্ণ *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    ৱর্ণ	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    যদি (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    অন্যথা যদি (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    অন্যথা
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (পূর্ণাঙ্ক)s.length);

    STRCPY(namebuf + tail, "mode");
    ৱিকল্প (chanpart->ch_mode)
    {
	পক্ষ CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    ভঙ্গ;
	পক্ষ CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    ভঙ্গ;
	পক্ষ CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    ভঙ্গ;
	পক্ষ CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    ভঙ্গ;
	পক্ষ CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    ভঙ্গ;
	পক্ষ CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    ভঙ্গ;
	পক্ষ CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    ভঙ্গ;
	স্ৱভাৱ:
	    STR_LITERAL_SET(s, "");
	    ভঙ্গ;
    }
    dict_add_string_len(dict, namebuf, s.string, (পূর্ণাঙ্ক)s.length);

    STRCPY(namebuf + tail, "io");
    যদি (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    অন্যথা ৱিকল্প (chanpart->ch_io)
    {
	পক্ষ JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    ভঙ্গ;
	পক্ষ JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    ভঙ্গ;
	পক্ষ JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    ভঙ্গ;
	পক্ষ JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    ভঙ্গ;
	পক্ষ JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    ভঙ্গ;
	স্ৱভাৱ:
	    STR_LITERAL_SET(s, "");
	    ভঙ্গ;
    }
    dict_add_string_len(dict, namebuf, s.string, (পূর্ণাঙ্ক)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_সংখ্যা(dict, namebuf, chanpart->ch_timeout);
}

    স্থির শূন্য
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_সংখ্যা(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    যদি (channel->ch_hostname != রিক্ত)
    {
	যদি (channel->ch_port)
	    dict_add_সংখ্যা(dict, "port", channel->ch_port);
	অন্যথা
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    অন্যথা
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
    শূন্য
channel_close(channel_T *channel, পূর্ণাঙ্ক invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    যদি (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	যদি (channel->ch_close_cb.cb_name != রিক্ত)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	{
	    যদি (channel->ch_close_cb.cb_name != রিক্ত
			    || channel->ch_part[part].ch_bufref.br_buf != রিক্ত)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		যদি (channel->ch_close_cb.cb_name == রিক্ত)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		যেতিয়ালৈ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	যদি (channel->ch_close_cb.cb_name != রিক্ত)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (ৱর্ণ *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      যদি (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      যদি (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  বাবে (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = রিক্ত;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = রিক্ত;
    channel->ch_ss_close_cb = রিক্ত;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    স্থির শূন্য
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    স্থির শূন্য
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    যদি (wq->wq_next == রিক্ত)
	wq->wq_prev = রিক্ত;
    অন্যথা
	wq->wq_next->wq_prev = রিক্ত;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    স্থির শূন্য
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    যেতিয়ালৈ (channel_peek(channel, part) != রিক্ত)
	vim_free(channel_get(channel, part, রিক্ত));

    যেতিয়ালৈ (cb_head->cq_next != রিক্ত)
    {
	cbq_T *গ্ৰন্থি = cb_head->cq_next;

	remove_cb_গ্ৰন্থি(cb_head, গ্ৰন্থি);
	free_callback(&গ্ৰন্থি->cq_callback);
	vim_free(গ্ৰন্থি);
    }

    যেতিয়ালৈ (json_head->jq_next != রিক্ত)
    {
	free_tv(json_head->jq_next->jq_মান);
	remove_json_গ্ৰন্থি(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    যেতিয়ালৈ (ch_part->ch_writeque.wq_next != রিক্ত)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    শূন্য
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

#যদি defined(EXITFREE)
    শূন্য
channel_free_all(শূন্য)
{
    channel_T *channel;

    ch_log(রিক্ত, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_বাৰ্তা_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    স্থির পূর্ণাঙ্ক
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    যদি (in_part->ch_writeque.wq_next != রিক্ত)
	ঘূৰাই TRUE;
    যদি (buf == রিক্ত)
	ঘূৰাই FALSE;
    ঘূৰাই in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_সাৰি_গুনতি)
	    : (in_part->ch_buf_শীৰ্ষ <= in_part->ch_buf_bot
			    && in_part->ch_buf_শীৰ্ষ <= buf->b_ml.ml_সাৰি_গুনতি);
}

#যদি defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    স্থির পূর্ণাঙ্ক
channel_fill_wfds(পূর্ণাঙ্ক maxfd_arg, fd_set *wfds)
{
    পূর্ণাঙ্ক		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	যদি (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (পূর্ণাঙ্ক)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((পূর্ণাঙ্ক)in_part->ch_fd, wfds);
	    যদি ((পূর্ণাঙ্ক)in_part->ch_fd >= maxfd)
		maxfd = (পূর্ণাঙ্ক)in_part->ch_fd + 1;
	}
    }
    ঘূৰাই maxfd;
}
#অন্যথা
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    স্থির পূর্ণাঙ্ক
channel_fill_poll_write(পূর্ণাঙ্ক nfd_in, সংরচনা pollfd *fds)
{
    পূর্ণাঙ্ক		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	যদি (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	অন্যথা
	    in_part->ch_poll_idx = -1;
    }
    ঘূৰাই nfd;
}
#endif

প্রকারনির্দেশ গণনা {
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
    স্থির channel_wait_result
channel_wait(channel_T *channel, sock_T fd, পূর্ণাঙ্ক timeout)
{
    যদি (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    যদি (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	পূর্ণাঙ্ক	sleep_time;
	DWORD	deadline = Get_Tick_গুনতি() + timeout;
	পূর্ণাঙ্ক	delay = 1;

	// reading from a pipe, not a socket
	যেতিয়ালৈ (TRUE)
	{
	    পূর্ণাঙ্ক r = PeekNamedPipe((HANDLE)fd, রিক্ত, 0, রিক্ত, &nread, রিক্ত);

	    যদি (r && nread > 0)
		ঘূৰাই CW_READY;

	    যদি (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, রিক্ত);
	    }
	    অন্যথা যদি (r == 0)
		ঘূৰাই CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_সাৰিগুলি();

	    sleep_time = deadline - Get_Tick_গুনতি();
	    যদি (sleep_time <= 0)
		ভঙ্গ;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    যদি (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    যদি (delay > 10)
		delay = 10;
	}
    }
    অন্যথা
#endif
    {
#যদি defined(HAVE_SELECT)
	সংরচনা timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	পূর্ণাঙ্ক		ret;
	পূর্ণাঙ্ক		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	বাবে (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((পূর্ণাঙ্ক)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (পূর্ণাঙ্ক)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, রিক্ত, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    যদি (ret == -1 && errno == EINTR)
		অনুৱর্ত;
# endif
	    যদি (ret > 0)
	    {
		যদি (FD_ISSET(fd, &rfds))
		    ঘূৰাই CW_READY;
		channel_write_any_সাৰিগুলি();
		অনুৱর্ত;
	    }
	    ভঙ্গ;
	}
#অন্যথা
	বাবে (;;)
	{
	    সংরচনা pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    পূর্ণাঙ্ক		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    যদি (poll(fds, nfd, timeout) > 0)
	    {
		যদি (fds[0].revents & POLLIN)
		    ঘূৰাই CW_READY;
		channel_write_any_সাৰিগুলি();
		অনুৱর্ত;
	    }
	    ভঙ্গ;
	}
#endif
    }
    ঘূৰাই CW_NOT_READY;
}

    স্থির শূন্য
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, পূর্ণাঙ্ক is_err, ৱর্ণ *func)
{
    ৱর্ণ	বাৰ্তা[] = "%s(): Read %s from ch_part[%d], closing";

    যদি (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, বাৰ্তা, func, "error", part);
    অন্যথা
	ch_log(channel, বাৰ্তা, func, "EOF", part);

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
    যদি (channel->ch_nb_close_cb != রিক্ত)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_বাৰ্তা_RAW,
			      (পূর্ণাঙ্ক)STRLEN_LITERAL(DETACH_বাৰ্তা_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    যদি (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	যদি (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    স্থির শূন্য
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    যদি (channel->ch_nb_close_cb != রিক্ত)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    যদি (channel->ch_ss_close_cb != রিক্ত)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    স্থির শূন্য
channel_read(channel_T *channel, ch_part_T part, ৱর্ণ *func)
{
    স্থির char_u	*buf = রিক্ত;
    পূর্ণাঙ্ক			len = 0;
    পূর্ণাঙ্ক			readlen = 0;
    sock_T		fd;
    পূর্ণাঙ্ক			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    যদি (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	ঘূৰাই;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    যদি (buf == রিক্ত)
    {
	buf = alloc(MAXMSGSIZE);
	যদি (buf == রিক্ত)
	    ঘূৰাই;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    বাবে (;;)
    {
	যদি (channel_wait(channel, fd, 0) != CW_READY)
	    ভঙ্গ;
	যদি (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    সংরচনা sockaddr_storage	client;

	    newchannel = add_channel();
	    যদি (newchannel == রিক্ত)
	    {
		ch_error(রিক্ত, "Cannot allocate channel.");
		ঘূৰাই;
	    }
	    socklen = পরিমাণ(client);
	    newfd = accept(fd, (সংরচনা sockaddr*)&client, &socklen);
	    যদি (newfd < 0)
	    {
		ch_error(রিক্ত, "Cannot accept channel.");
		channel_free(newchannel);
		ঘূৰাই;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    যদি (channel->ch_ss_accept_cb != রিক্ত)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ঘূৰাই;
	    }
#endif

	    যদি (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		ৱর্ণ addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((সংরচনা sockaddr_in*)&client)->sin_addr,
			addr, পরিমাণ(addr));
		vim_snprintf((ৱর্ণ *)namebuf, পরিমাণ(namebuf), "%s:%d",
			addr,
			ntohs(((সংরচনা sockaddr_in*)&client)->sin_port));
#অন্যথা
		vim_snprintf((ৱর্ণ *)namebuf, পরিমাণ(namebuf), "%s:%d",
		    inet_ntoa(((সংরচনা sockaddr_in*)&client)->sin_addr),
		    ntohs(((সংরচনা sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    অন্যথা যদি (client.ss_family == AF_INET6)
	    {
		ৱর্ণ addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((সংরচনা sockaddr_in6*)&client)->sin6_addr,
			addr, পরিমাণ(addr));
		vim_snprintf((ৱর্ণ *)namebuf, পরিমাণ(namebuf), "[%s]:%d",
			addr,
			ntohs(((সংরচনা sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    অন্যথা যদি (client.ss_family == AF_UNIX)
		vim_snprintf((ৱর্ণ *)namebuf, পরিমাণ(namebuf),
							    "unix:anonymous");
	    অন্যথা
		vim_snprintf((ৱর্ণ *)namebuf, পরিমাণ(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    ঘূৰাই;
	}
	যদি (use_socket)
	    len = sock_read(fd, (ৱর্ণ *)buf, MAXMSGSIZE);
	অন্যথা
	    len = fd_read(fd, (ৱর্ণ *)buf, MAXMSGSIZE);
	যদি (len <= 0)
	    ভঙ্গ;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    যদি (readlen <= 0)
    {
	যদি (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#যদি defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    অন্যথা যদি (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    শূন্য
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
    স্থির char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, পূর্ণাঙ্ক timeout, পূর্ণাঙ্ক raw, পূর্ণাঙ্ক *outlen)
{
    char_u	*buf;
    char_u	*বাৰ্তা;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*গ্ৰন্থি;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    যেতিয়ালৈ (TRUE)
    {
	গ্ৰন্থি = channel_peek(channel, part);
	যদি (গ্ৰন্থি != রিক্ত)
	{
	    যদি (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(গ্ৰন্থি) != রিক্ত))
		// got a complete message
		ভঙ্গ;
	    যদি (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		অনুৱর্ত;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    যদি (raw || fd == INVALID_FD)
		ভঙ্গ;
	}

	// Wait for up to the channel timeout.
	যদি (fd == INVALID_FD)
	    ঘূৰাই রিক্ত;
	যদি (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ঘূৰাই রিক্ত;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    যদি (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != রিক্ত)
    {
	বাৰ্তা = channel_get_all(channel, part, outlen);
    }
    অন্যথা
    {
	char_u *p;

	buf = গ্ৰন্থি->rq_buffer;
	nl = channel_first_nl(গ্ৰন্থি);

	// Convert NUL to NL, the internal representation.
	বাবে (p = buf; (nl == রিক্ত || p < nl) && p < buf + গ্ৰন্থি->rq_buflen; ++p)
	    যদি (*p == NUL)
		*p = NL;

	যদি (nl == রিক্ত)
	{
	    // must be a closed channel with missing NL
	    বাৰ্তা = channel_get(channel, part, রিক্ত);
	}
	অন্যথা যদি (nl + 1 == buf + গ্ৰন্থি->rq_buflen)
	{
	    // get the whole buffer
	    বাৰ্তা = channel_get(channel, part, রিক্ত);
	    *nl = NUL;
	}
	অন্যথা
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    বাৰ্তা = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (পূর্ণাঙ্ক)(nl - buf) + 1);
	}
    }
    যদি (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != রিক্ত ? *outlen : (পূর্ণাঙ্ক)STRLEN(বাৰ্তা));
    ঘূৰাই বাৰ্তা;
}

স্থির পূর্ণাঙ্ক channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    পূর্ণাঙ্ক
channel_in_blocking_wait(শূন্য)
{
    ঘূৰাই channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    স্থির শূন্য
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, পূর্ণাঙ্ক timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (শূন্য)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#অন্যথা
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
    স্থির পূর্ণাঙ্ক
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	পূর্ণাঙ্ক	    timeout_arg,
	পূর্ণাঙ্ক	    id,
	typval_T    **rettv)
{
    পূর্ণাঙ্ক		more;
    sock_T	fd;
    পূর্ণাঙ্ক		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    পূর্ণাঙ্ক		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    যদি (id >= 0)
	channel_add_block_id(chanpart, id);

    বাবে (;;)
    {
	যদি (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (শূন্য)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	যদি (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    ভঙ্গ;
	}

	যদি (!more)
	{
	    শূন্য *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    শূন্য *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    যদি (channel_parse_messages())
		অনুৱর্ত;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    যদি (readahead_ptr != রিক্ত && readahead_ptr != prev_readahead_ptr)
		অনুৱর্ত;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (পূর্ণাঙ্ক)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    যদি (timeout <= 0 || fd == INVALID_FD)
	    {
		যদি (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		ভঙ্গ;
	    }
	    // If there was an incomplete message use the deadline for that.
	    যদি (chanpart->ch_wait_len > 0)
	    {
		পূর্ণাঙ্ক	বাম;
#ifdef MSWIN
		বাম = chanpart->ch_deadline - Get_Tick_গুনতি() + 1;
#অন্যথা
		{
		    সংরচনা timeval now_tv;

		    gettimeofday(&now_tv, রিক্ত);
		    বাম = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		যদি (বাম < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		অন্যথা যদি (বাম < timeout)
		    timeout = বাম;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    যদি (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    ঘূৰাই retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, পূর্ণাঙ্ক check_open, পূর্ণাঙ্ক reading, ch_part_T part)
{
    channel_T	*channel = রিক্ত;
    পূর্ণাঙ্ক		has_readahead = FALSE;

    যদি (tv->v_type == VAR_JOB)
    {
	যদি (tv->vval.v_job != রিক্ত)
	    channel = tv->vval.v_job->jv_channel;
    }
    অন্যথা যদি (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    অন্যথা
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ঘূৰাই রিক্ত;
    }
    যদি (channel != রিক্ত && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_গুনতি ? part : channel_part_read(channel));

    যদি (check_open && (channel == রিক্ত || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ঘূৰাই রিক্ত;
    }
    ঘূৰাই channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    স্থির শূন্য
common_channel_read(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক raw, পূর্ণাঙ্ক blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_গুনতি;
    jobopt_T	opt;
    পূর্ণাঙ্ক		mode;
    পূর্ণাঙ্ক		timeout;
    পূর্ণাঙ্ক		id = -1;
    typval_T	*listtv = রিক্ত;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    clear_job_options(&opt);
    যদি (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	গচ্ছ theend;

    যদি (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    যদি (channel == রিক্ত)
	গচ্ছ theend;

    যদি (part == PART_গুনতি)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    যদি (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    যদি (blob || mode == CH_MODE_BLOB)
    {
	পূর্ণাঙ্ক	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	যদি (p != রিক্ত)
	{
	    blob_T	*b = blob_alloc();

	    যদি (b != রিক্ত)
	    {
		b->bv_ga.ga_len = outlen;
		যদি (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		অন্যথা
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    অন্যথা যদি (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, রিক্ত);
    অন্যথা
    {
	যদি (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	যদি (listtv != রিক্ত)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	অন্যথা
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_সংখ্যা = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#যদি defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    শূন্য
channel_handle_events(পূর্ণাঙ্ক only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	যদি (only_keep_open && !channel->ch_keep_open)
	    অনুৱর্ত;

	// check the socket and pipes
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    যদি (fd == INVALID_FD)
		অনুৱর্ত;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    পূর্ণাঙ্ক r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    যদি (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    অন্যথা যদি (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	যদি (channel->ch_job != রিক্ত)
	{
	    job_T *job = channel->ch_job;

	    যদি (job->jv_tty_out != রিক্ত && job->jv_status == JOB_FINISHED)
		বাবে (part = PART_SOCK; part < PART_গুনতি; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#যদি defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    পূর্ণাঙ্ক
channel_any_keep_open(শূন্য)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	যদি (channel->ch_keep_open)
	    ঘূৰাই TRUE;
    ঘূৰাই FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    শূন্য
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    পূর্ণাঙ্ক		fd = ch_part->ch_fd;

    যদি (fd == INVALID_FD)
	ঘূৰাই;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#অন্যথা
    (শূন্য)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    পূর্ণাঙ্ক
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	পূর্ণাঙ্ক	  len_arg,
	ৱর্ণ	  *fun)
{
    পূর্ণাঙ্ক		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    পূর্ণাঙ্ক		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    যদি (fd == INVALID_FD)
    {
	যদি (!channel->ch_error && fun != রিক্ত)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ঘূৰাই FAIL;
    }

    যদি (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    যদি (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_বাৰ্তা = 0;
    }

    বাবে (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	পূর্ণাঙ্ক	    len;

	যদি (wq->wq_next != রিক্ত)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	অন্যথা
	{
	    যদি (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ঘূৰাই OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	যদি (part == PART_SOCK)
	    res = sock_write(fd, (ৱর্ণ *)buf, len);
	অন্যথা
	{
	    res = fd_write(fd, (ৱর্ণ *)buf, len);
#ifdef MSWIN
	    যদি (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, রিক্ত);
	    }
#endif
	}
	যদি (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	যদি (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    যদি (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    যদি (res == len)
	    {
		// Wrote all the buf[len] bytes.
		যদি (entry != রিক্ত)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    অনুৱর্ত;
		}
		যদি (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    অন্যথা
	    {
		// Wrote only buf[res] bytes, can't write more now.
		যদি (entry != রিক্ত)
		{
		    যদি (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (ৱর্ণ *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		অন্যথা
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		যদি (wq->wq_prev != রিক্ত
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    যদি (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((ৱর্ণ *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		অন্যথা
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    যদি (last != রিক্ত)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = রিক্ত;
			যদি (wq->wq_prev == রিক্ত)
			    wq->wq_next = last;
			অন্যথা
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			যদি (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	অন্যথা যদি (res != len)
	{
	    যদি (!channel->ch_error && fun != রিক্ত)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ঘূৰাই FAIL;
	}

	channel->ch_error = FALSE;
	ঘূৰাই OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    স্থির channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	পূর্ণাঙ্ক	    len,
	পূর্ণাঙ্ক	    id,
	পূর্ণাঙ্ক	    eval,
	jobopt_T    *opt,
	ৱর্ণ	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel == রিক্ত)
	ঘূৰাই রিক্ত;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    যদি (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ঘূৰাই রিক্ত;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    যদি (opt->jo_callback.cb_name != রিক্ত && *opt->jo_callback.cb_name != NUL)
    {
	যদি (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ঘূৰাই রিক্ত;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    যদি (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == রিক্ত)
	ঘূৰাই channel;
    ঘূৰাই রিক্ত;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    স্থির শূন্য
ch_ৰাশিমালা_common(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    পূর্ণাঙ্ক		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    পূর্ণাঙ্ক		timeout;
    পূর্ণাঙ্ক		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel == রিক্ত)
	ঘূৰাই;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    যদি (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ঘূৰাই;
    }

    যদি (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	ৱর্ণ		*চাবি = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	যদি (rettv_dict_alloc(rettv) == FAIL)
	    ঘূৰাই;

	যদি (check_for_dict_arg(argvars, 1) == FAIL)
	    ঘূৰাই;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)চাবি, -1);
	যদি (argvars[2].v_type == VAR_DICT
		&& dict_has_চাবি(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	যদি (di != রিক্ত && di->di_tv.v_type != VAR_সংখ্যা
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_মান_for_argument_str), চাবি);
	    ঘূৰাই;
	}

	যদি (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_বাৰ্তা_id;
	    যদি (di == রিক্ত)
		dict_add_সংখ্যা(d, "seq", id);
	    অন্যথা
		di->di_tv.vval.v_সংখ্যা = id;
	}
	অন্যথা যদি (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_বাৰ্তা_id;
	    যদি (di == রিক্ত)
		dict_add_সংখ্যা(d, "id", id);
	    অন্যথা
		di->di_tv.vval.v_সংখ্যা = id;
	}
	অন্যথা
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    যদি (di != রিক্ত && di->di_tv.v_type == VAR_সংখ্যা)
		id = di->di_tv.vval.v_সংখ্যা;
	}
	যদি (ch_mode == CH_MODE_LSP && !dict_has_চাবি(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_বাৰ্তা(&argvars[1]);
    }
    অন্যথা
    {
	id = ++channel->ch_last_বাৰ্তা_id;
	text = json_encode_nr_ৰাশিমালা(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    যদি (text == রিক্ত)
	ঘূৰাই;

    channel = send_common(argvars, text, (পূর্ণাঙ্ক)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    যদি (channel != রিক্ত && eval)
    {
	যদি (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	অন্যথা
	    timeout = channel_get_timeout(channel, part_read);
	যদি (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    যদি (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_সংখ্যা;
		free_tv(listtv);
	    }
	    অন্যথা
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_সংখ্যা;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    যদি (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	যদি (rettv->vval.v_dict != রিক্ত)
	    dict_add_সংখ্যা(rettv->vval.v_dict, "seq", id);
    }
    অন্যথা যদি (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	যদি (rettv->vval.v_dict != রিক্ত)
	    dict_add_সংখ্যা(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    স্থির শূন্য
ch_raw_common(typval_T *argvars, typval_T *rettv, পূর্ণাঙ্ক eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    পূর্ণাঙ্ক		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    পূর্ণাঙ্ক		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ঘূৰাই;

    যদি (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    অন্যথা
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (পূর্ণাঙ্ক)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    যদি (channel != রিক্ত && eval)
    {
	যদি (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	অন্যথা
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, রিক্ত);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#যদি defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    পূর্ণাঙ্ক
channel_poll_setup(পূর্ণাঙ্ক nfd_in, শূন্য *fds_in, পূর্ণাঙ্ক *towait)
{
    পূর্ণাঙ্ক		nfd = nfd_in;
    channel_T	*channel;
    সংরচনা	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    যদি (ch_part->ch_fd != INVALID_FD)
	    {
		যদি (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    যদি (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		অন্যথা
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    অন্যথা
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    ঘূৰাই nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    পূর্ণাঙ্ক
channel_poll_check(পূর্ণাঙ্ক ret_in, শূন্য *fds_in)
{
    পূর্ণাঙ্ক		ret = ret_in;
    channel_T	*channel;
    সংরচনা	pollfd *fds = fds_in;
    ch_part_T	part;
    পূর্ণাঙ্ক		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    যদি (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    অন্যথা যদি (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	যদি (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_প্ৰবেশ(channel);
	    --ret;
	}
    }

    ঘূৰাই ret;
}
#endif // UNIX && !HAVE_SELECT

#যদি !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    পূর্ণাঙ্ক
channel_select_setup(
	পূর্ণাঙ্ক maxfd_in,
	শূন্য *rfds_in,
	শূন্য *wfds_in,
	সংরচনা timeval *tv,
	সংরচনা timeval **tvp)
{
    পূর্ণাঙ্ক		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    যদি (fd != INVALID_FD)
	    {
		যদি (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    যদি (*tvp == রিক্ত || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		অন্যথা
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    যদি ((পূর্ণাঙ্ক)fd >= FD_SETSIZE)
			অনুৱর্ত;
# endif
		    FD_SET((পূর্ণাঙ্ক)fd, rfds);
		    যদি (maxfd < (পূর্ণাঙ্ক)fd)
			maxfd = (পূর্ণাঙ্ক)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ঘূৰাই maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    পূর্ণাঙ্ক
channel_select_check(পূর্ণাঙ্ক ret_in, শূন্য *rfds_in, শূন্য *wfds_in)
{
    পূর্ণাঙ্ক		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	বাবে (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    যদি (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (পূর্ণাঙ্ক)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    অন্যথা যদি (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	যদি (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (পূর্ণাঙ্ক)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_প্ৰবেশ(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	যদি (channel->ch_job != রিক্ত)
	{
	    job_T *job = channel->ch_job;

	    যদি (job->jv_tty_out != রিক্ত && job->jv_status == JOB_FINISHED)
		বাবে (part = PART_SOCK; part < PART_গুনতি; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ঘূৰাই ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    স্থির শূন্য
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    যদি (stdio_channel->CH_OUT_FD != INVALID_FD)
	(শূন্য)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(পূর্ণাঙ্ক)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#নোচেদ্যদি defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    সংরচনা timeval	tv;
    সংরচনা timeval	*tvp = &tv;
    পূর্ণাঙ্ক			maxfd;
    পূর্ণাঙ্ক			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, রিক্ত, tvp);
    যদি (ret >= 0)
	(শূন্য)channel_select_check(ret, &rfds, &wfds);
#অন্যথা
    সংরচনা pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    পূর্ণাঙ্ক			nfd;
    পূর্ণাঙ্ক			towait = (পূর্ণাঙ্ক)msec;
    পূর্ণাঙ্ক			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    যদি (ret >= 0)
	(শূন্য)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    শূন্য
channel_stdio_loop(শূন্য)
{
    যেতিয়ালৈ (stdio_channel != রিক্ত && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	যদি (due_time > 0 && due_time < msec)
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
    পূর্ণাঙ্ক
channel_parse_messages(শূন্য)
{
    channel_T	*channel = first_channel;
    পূর্ণাঙ্ক		ret = FALSE;
    পূর্ণাঙ্ক		r;
    ch_part_T	part = PART_SOCK;
    স্থির পূর্ণাঙ্ক	recursive = 0;
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
    যদি ((did_repeated_বাৰ্তা & REPEATED_বাৰ্তা_LOOKING) == 0)
    {
	ch_log(রিক্ত, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_বাৰ্তা = REPEATED_বাৰ্তা_LOOKING;
    }
    যেতিয়ালৈ (channel != রিক্ত)
    {
	যদি (recursive == 1)
	{
	    যদি (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_গুনতি);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		অনুৱর্ত;
	    }
	    যদি (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		যদি (channel->ch_job != রিক্ত)
		    channel->ch_job->jv_channel = রিক্ত;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		অনুৱর্ত;
	    }
	    যদি (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		অনুৱর্ত;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	যদি (part == PART_OUT
		&& channel->ch_job != রিক্ত
		&& channel->ch_job->jv_tty_out != রিক্ত
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    অনুৱর্ত;
	}
#endif
	যদি (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    যদি (r == OK)
		ret = TRUE;
	    যদি (channel_unref(channel) || (r == OK
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
		অনুৱর্ত;
	    }
	}
	যদি (part < PART_ERR)
	    ++part;
	অন্যথা
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    যদি (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    ঘূৰাই ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    পূর্ণাঙ্ক
channel_any_readahead(শূন্য)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    যেতিয়ালৈ (channel != রিক্ত)
    {
	যদি (channel_has_readahead(channel, part))
	    ঘূৰাই TRUE;
	যদি (part < PART_ERR)
	    ++part;
	অন্যথা
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    ঘূৰাই FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    পূর্ণাঙ্ক
set_ref_in_channel(পূর্ণাঙ্ক copyID)
{
    পূর্ণাঙ্ক		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    বাবে (channel = first_channel; !abort && channel != রিক্ত;
						   channel = channel->ch_next)
	যদি (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, রিক্ত, রিক্ত, রিক্ত);
	}
    ঘূৰাই abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    স্থির ch_part_T
channel_part_send(channel_T *channel)
{
    যদি (channel->CH_SOCK_FD == INVALID_FD)
	ঘূৰাই PART_IN;
    ঘূৰাই PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    স্থির ch_part_T
channel_part_read(channel_T *channel)
{
    যদি (channel->CH_SOCK_FD == INVALID_FD)
	ঘূৰাই PART_OUT;
    ঘূৰাই PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    স্থির ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    যদি (channel == রিক্ত)
	ঘূৰাই CH_MODE_JSON;
    ঘূৰাই channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    স্থির পূর্ণাঙ্ক
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ঘূৰাই channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    শূন্য
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_সংখ্যা = 0;
    যদি (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel != রিক্ত)
	rettv->vval.v_সংখ্যা = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    শূন্য
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    যদি (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel != রিক্ত)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    শূন্য
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    যদি (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel != রিক্ত)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    শূন্য
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_সংখ্যা = -1;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel == রিক্ত)
	ঘূৰাই;

    char_u	*what = tv_get_string(&argvars[1]);
    পূর্ণাঙ্ক		part;
    যদি (STRCMP(what, "err") == 0)
	part = PART_ERR;
    অন্যথা যদি (STRCMP(what, "out") == 0)
	part = PART_OUT;
    অন্যথা যদি (STRCMP(what, "in") == 0)
	part = PART_IN;
    অন্যথা
	part = PART_SOCK;
    যদি (channel->ch_part[part].ch_bufref.br_buf != রিক্ত)
	rettv->vval.v_সংখ্যা =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    শূন্য
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    যদি (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel == রিক্ত)
	ঘূৰাই;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    যদি (channel->ch_job != রিক্ত)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    শূন্য
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    যদি (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel != রিক্ত && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    শূন্য
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    যদি (check_restricted() || check_secure())
	ঘূৰাই;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    শূন্য
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    যদি (check_restricted() || check_secure())
	ঘূৰাই;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    শূন্য
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    শূন্য
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    শূন্য
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    শূন্য
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ৰাশিমালা_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    শূন্য
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ৰাশিমালা_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    শূন্য
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    শূন্য
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    শূন্য
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    যদি (check_restricted() || check_secure())
	ঘূৰাই;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel == রিক্ত)
	ঘূৰাই;
    clear_job_options(&opt);
    যদি (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    শূন্য
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    পূর্ণাঙ্ক		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = রিক্ত;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ঘূৰাই;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    যদি (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	যদি (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    ৱর্ণ      *status = channel_status(channel, -1);

    যদি (channel == রিক্ত)
	vim_snprintf((ৱর্ণ *)buf, NUMBUFLEN, "channel %s", status);
    অন্যথা
	vim_snprintf((ৱর্ণ *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    ঘূৰাই buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(পূর্ণাঙ্ক ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	যদি (ch->ch_id == ch_id)
	    ঘূৰাই ch;
    ঘূৰাই রিক্ত;
}

#endif // FEAT_JOB_CHANNEL
