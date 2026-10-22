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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_JOB_CHANNEL)

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
வகைவரை கட்டமைப்பு sockaddr_un {
    ADDRESS_FAMILY sun_family;
    எழுத்து sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#இல்லையெனில்
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) லிக(sd, buf, len)
# define sock_read(sd, buf, len) பட(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) பட(fd, buf, len)
# define fd_write(sd, buf, len) லிக(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

நிலையான வெறுமை channel_read(channel_T *channel, ch_part_T part, எழுத்து *func);
நிலையான வெறுமை channel_wait_any(long msec);
நிலையான ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
நிலையான முழு channel_get_timeout(channel_T *channel, ch_part_T part);
நிலையான channel_T *channel_open_stdio(வெறுமை);
நிலையான ch_part_T channel_part_send(channel_T *channel);
நிலையான ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    க்கு ((ch) = first_channel; (ch) != வெற்று; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
நிலையான முழு safe_to_invoke_callback = 0;

#ifdef MSWIN
    நிலையான முழு
fd_read(sock_T fd, எழுத்து *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    எனில் (!ReadFile(h, buf, (DWORD)len, &nread, வெற்று))
	திரும்பு -1;
    திரும்பு (முழு)nread;
}

    நிலையான முழு
fd_write(sock_T fd, எழுத்து *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    போது (todo > 0)
    {
	எனில் (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	இல்லையெனில்
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	எனில் (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    எனில் (err != ERROR_IO_PENDING)
		திரும்பு -1;
	    எனில் (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		திரும்பு -1;
	    FlushFileBuffers(h);
	}
	இல்லையெனில் எனில் (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    நிறுத்து;
	todo -= nwrite;
	done += nwrite;
    }
    திரும்பு (முழு)done;
}

    நிலையான வெறுமை
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(செய்தி) (வெறுமை)semsg("%s: %s", செய்தி, strerror_win32(errno))

    நிலையான எழுத்து *
strerror_win32(முழு eno)
{
    நிலையான LPVOID msgbuf = வெற்று;
    char_u *ptr;

    எனில் (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = வெற்று;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	வெற்று,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	வெற்று);
    எனில் (msgbuf != வெற்று)
	// chomp \r or \n
	க்கு (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    தேர்வு (*ptr)
	    {
		நிலை '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    நிறுத்து;
		நிலை '\n':
		    எனில் (*(ptr + 1) == '\0')
			*ptr = '\0';
		    இல்லையெனில்
			*ptr = ' ';
		    நிறுத்து;
	    }
    திரும்பு msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
நிலையான channel_T *first_channel = வெற்று;
நிலையான முழு next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(வெறுமை)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    எனில் (channel == வெற்று)
	திரும்பு வெற்று;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    க்கு (part = PART_SOCK; part < PART_மொத்தம்; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_உள்ளீடு_Handler = (Xt_உள்ளீடு_Id)வெற்று;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_உள்ளீடு_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    எனில் (first_channel != வெற்று)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    திரும்பு channel;
}

    முழு
has_any_channel(வெறுமை)
{
    திரும்பு first_channel != வெற்று;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    முழு
channel_still_useful(channel_T *channel)
{
    முழு has_sock_செய்தி;
    முழு	has_out_செய்தி;
    முழு	has_err_செய்தி;

    // If the job was killed the channel is not expected to work anymore.
    எனில் (channel->ch_job_killed && channel->ch_job == வெற்று)
	திரும்பு FALSE;

    // If there is a close callback it may still need to be invoked.
    எனில் (channel->ch_close_cb.cb_name != வெற்று)
	திரும்பு TRUE;

    // If reading from or a buffer it's still useful.
    எனில் (channel->ch_part[PART_IN].ch_bufref.br_buf != வெற்று)
	திரும்பு TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_செய்தி = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != வெற்று
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != வெற்று;
    has_out_செய்தி = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != வெற்று
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != வெற்று;
    has_err_செய்தி = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != வெற்று
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != வெற்று;
    திரும்பு (channel->ch_callback.cb_name != வெற்று && (has_sock_செய்தி
		|| has_out_செய்தி || has_err_செய்தி))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != வெற்று
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != வெற்று)
		    && has_out_செய்தி)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != வெற்று
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != வெற்று)
		    && has_err_செய்தி);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    முழு
channel_can_close(channel_T *channel)
{
    திரும்பு channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    நிலையான வெறுமை
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    நிலையான வெறுமை
channel_free_channel(channel_T *channel)
{
    எனில் (channel->ch_next != வெற்று)
	channel->ch_next->ch_prev = channel->ch_prev;
    எனில் (channel->ch_prev == வெற்று)
	first_channel = channel->ch_next;
    இல்லையெனில்
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    நிலையான வெறுமை
channel_free(channel_T *channel)
{
    எனில் (in_free_unref_பொருட்கள்)
	திரும்பு;

    எனில் (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    இல்லையெனில்
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
    நிலையான முழு
channel_may_free(channel_T *channel)
{
    எனில் (!channel_still_useful(channel))
    {
	channel_free(channel);
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    முழு
channel_unref(channel_T *channel)
{
    எனில் (channel != வெற்று && --channel->ch_refcount <= 0)
	திரும்பு channel_may_free(channel);
    திரும்பு FALSE;
}

    முழு
free_unused_channels_contents(முழு copyID, முழு mask)
{
    முழு		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	எனில் (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    திரும்பு did_free;
}

    வெறுமை
free_unused_channels(முழு copyID, முழு mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    க்கு (ch = first_channel; ch != வெற்று; ch = ch_next)
    {
	ch_next = ch->ch_next;
	எனில் (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#எனில் defined(FEAT_GUI)

# எனில் defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    நிலையான channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    எனில் (fd == INVALID_FD)
	திரும்பு வெற்று;

    FOR_ALL_CHANNELS(channel)
    {
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	    எனில் (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		திரும்பு channel;
	    }
    }
    திரும்பு வெற்று;
}

    நிலையான வெறுமை
channel_read_fd(முழு fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    எனில் (channel == வெற்று)
	ch_error(வெற்று, "Channel for fd %d not found", fd);
    இல்லையெனில்
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    நிலையான வெறுமை
messageFromServerX11(XtPointer clientData,
		  முழு *unused1 UNUSED,
		  Xt_உள்ளீடு_Id *unused2 UNUSED)
{
    channel_read_fd((முழு)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  எனில் GTK_CHECK_VERSION(3,0,0)
    நிலையான gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    திரும்பு TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  இல்லையெனில்
    நிலையான வெறுமை
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_உள்ளீடு_Condition unused2 UNUSED)
{
    channel_read_fd((முழு)(long)clientData);
}
#  endif
# endif

    நிலையான வெறுமை
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    எனில் (!CH_HAS_GUI)
	திரும்பு;

    // gets stuck in handling events for a not connected channel
    எனில் (channel->ch_keep_open)
	திரும்பு;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    எனில் (channel->ch_part[part].ch_உள்ளீடு_Handler == (Xt_உள்ளீடு_Id)வெற்று)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_உள்ளீடு_Handler = Xt_App_Add_உள்ளீடு(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_உள்ளீடு_Read_Mask + Xt_உள்ளீடு_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# இல்லையெனில்
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    எனில் (channel->ch_part[part].ch_உள்ளீடு_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   எனில் GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_உள்ளீடு_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   இல்லையெனில்
	channel->ch_part[part].ch_உள்ளீடு_Handler = gdk_உள்ளீடு_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_உள்ளீடு_Condition)
			     ((முழு)GDK_உள்ளீடு_READ + (முழு)GDK_உள்ளீடு_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    நிலையான வெறுமை
channel_gui_register(channel_T *channel)
{
    எனில் (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    எனில் (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    எனில் (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    வெறுமை
channel_gui_register_all(வெறுமை)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    நிலையான வெறுமை
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    எனில் (channel->ch_part[part].ch_உள்ளீடு_Handler != (Xt_உள்ளீடு_Id)வெற்று)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_உள்ளீடு(channel->ch_part[part].ch_உள்ளீடு_Handler);
	channel->ch_part[part].ch_உள்ளீடு_Handler = (Xt_உள்ளீடு_Id)வெற்று;
    }
# இல்லையெனில்
#  ifdef FEAT_GUI_GTK
    எனில் (channel->ch_part[part].ch_உள்ளீடு_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   எனில் GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_உள்ளீடு_Handler);
#   இல்லையெனில்
	gdk_உள்ளீடு_remove(channel->ch_part[part].ch_உள்ளீடு_Handler);
#   endif
	channel->ch_part[part].ch_உள்ளீடு_Handler = 0;
    }
#  endif
# endif
}

    நிலையான வெறுமை
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    க்கு (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    நிலையான முழு
channel_connect(
	channel_T *channel,
	மாறிலி கட்டமைப்பு sockaddr *server_addr,
	முழு server_addrlen,
	முழு *waittime)
{
    முழு		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    போது (TRUE)
    {
	long	elapsed_msec = 0;
	முழு	waitnow;
	முழு	ret;

	எனில் (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	எனில் (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    திரும்பு -1;
	}

	எனில் (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    எனில் (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#இல்லையெனில்
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		திரும்பு -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	எனில் (ret == 0)
	    // The connection could be established.
	    நிறுத்து;

	SOCK_ERRNO;
	எனில் (*waittime < 0 || (errno != EWOULDBLOCK
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
	    திரும்பு -1;
	}
	இல்லையெனில் எனில் (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    திரும்பு -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    கட்டமைப்பு timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    முழு			so_error = 0;
	    socklen_t		so_error_len = அளவு(so_error);
	    கட்டமைப்பு timeval	start_tv;
	    கட்டமைப்பு timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, வெற்று);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, வெற்று, &tv);
	    எனில் (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		திரும்பு -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    எனில் (FD_ISSET(sd, &wfds))
		நிறுத்து;
	    elapsed_msec = waitnow;
	    எனில் (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		தொடர்;
	    }
#இல்லையெனில்
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    எனில் (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		எனில் (ret < 0 || (so_error != 0
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
		    திரும்பு -1;
		}
		இல்லையெனில் எனில் (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    திரும்பு -1;
		}
	    }

	    எனில் (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		நிறுத்து;

	    gettimeofday(&end_tv, வெற்று);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	எனில் (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    எனில் (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    எனில் (!got_int)
	    {
		எனில் (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		தொடர்;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	திரும்பு -1;
    }

    எனில் (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#இல்லையெனில்
	(வெறுமை)fcntl(sd, F_SETFL, 0);
#endif
    }

    திரும்பு sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	மாறிலி எழுத்து *path,
	வெறுமை (*nb_close_cb)(வெறுமை))
{
    channel_T		*channel = வெற்று;
    முழு			sd = -1;
    size_t		path_len = STRLEN(path);
    கட்டமைப்பு sockaddr_un	server;
    size_t		server_len;
    முழு			waittime = -1;

    எனில் (*path == NUL || path_len >= அளவு(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	திரும்பு வெற்று;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    எனில் (channel == வெற்று)
    {
	ch_error(வெற்று, "Cannot allocate channel.");
	திரும்பு வெற்று;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, அளவு(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(கட்டமைப்பு sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (கட்டமைப்பு sockaddr *)&server, (முழு)server_len,
								   &waittime);

    எனில் (sd < 0)
    {
	channel_free(channel);
	திரும்பு வெற்று;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (எழுத்து *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    திரும்பு channel;
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
	மாறிலி எழுத்து *hostname,
	முழு port,
	முழு waittime,
	வெறுமை (*nb_close_cb)(வெறுமை))
{
    முழு			sd = -1;
    channel_T		*channel = வெற்று;
#ifdef FEAT_IPV6
    முழு			err;
    கட்டமைப்பு addrinfo	hints;
    கட்டமைப்பு addrinfo	*res = வெற்று;
    கட்டமைப்பு addrinfo	*addr = வெற்று;
#இல்லையெனில்
    கட்டமைப்பு sockaddr_in	server;
    கட்டமைப்பு hostent	*host = வெற்று;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    எனில் (channel == வெற்று)
    {
	ch_error(வெற்று, "Cannot allocate channel.");
	திரும்பு வெற்று;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# எனில் defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    எனில் ((err = getaddrinfo(hostname, வெற்று, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	திரும்பு வெற்று;
    }

    // Count the number of addresses for timeout distribution
    முழு addr_மொத்தம் = 0;
    க்கு (addr = res; addr != வெற்று; addr = addr->ai_next)
	addr_மொத்தம்++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    எனில் (waittime == 0)
	waittime = 15;

    முழு addr_index = 0;
    க்கு (addr = res; addr != வெற்று; addr = addr->ai_next, addr_index++)
    {
	மாறிலி எழுத்து  *dst = hostname;
# ifdef HAVE_INET_NTOP
	மாறிலி வெறுமை  *src = வெற்று;
	எழுத்து	    buf[NUMBUFLEN];
# endif
	முழு	    try_waittime;
	முழு	    before_waittime;
	முழு	    consumed;
	முழு	    remaining_addrs;

	எனில் (addr->ai_family == AF_INET6)
	{
	    கட்டமைப்பு sockaddr_in6 *sai = (கட்டமைப்பு sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	இல்லையெனில் எனில் (addr->ai_family == AF_INET)
	{
	    கட்டமைப்பு sockaddr_in *sai = (கட்டமைப்பு sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	எனில் (src != வெற்று)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, அளவு(buf));
	    எனில் (dst == வெற்று)
		dst = hostname;
	    இல்லையெனில் எனில் (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	எனில் (addr->ai_next == வெற்று)
	    try_waittime = waittime;
	இல்லையெனில் எனில் (addr_index == 0)
	{
	    எனில் (waittime > 500)
		try_waittime = 250;
	    இல்லையெனில் எனில் (waittime > 30)
		try_waittime = waittime / 2;
	    இல்லையெனில்
		try_waittime = waittime;
	}
	இல்லையெனில்
	{
	    remaining_addrs = addr_மொத்தம் - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (முழு)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	எனில் (waittime < 0)
	    waittime = 0;

	எனில் (sd >= 0)
	    நிறுத்து;

	// If we have no time left, stop trying
	எனில் (waittime <= 0 && addr->ai_next != வெற்று)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    நிறுத்து;
	}
    }

    freeaddrinfo(res);
#இல்லையெனில்
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    எனில் ((host = gethostbyname(hostname)) == வெற்று)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	திரும்பு வெற்று;
    }
    {
	எழுத்து *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], அளவு(p));
	memcpy((எழுத்து *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    எனில் (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (கட்டமைப்பு sockaddr *)&server, அளவு(server),
								   &waittime);
#endif

    எனில் (sd < 0)
    {
	channel_free(channel);
	திரும்பு வெற்று;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (எழுத்து *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    திரும்பு channel;
}

    நிலையான வெறுமை
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    எனில் (callback->cb_name != வெற்று && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    இல்லையெனில்
	cbp->cb_name = வெற்று;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	நிலையான வெறுமை
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_மதிப்பு_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_மதிப்பு_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    எனில் (curbuf->b_ml.ml_mfp == வெற்று)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    நிலையான buf_T *
channel_find_buffer(char_u *name, முழு err, முழு செய்தி)
{
    buf_T *buf = வெற்று;
    buf_T *save_curbuf = curbuf;

    எனில் (name != வெற்று && *name != NUL)
    {
	buf = buflist_findname(name);
	எனில் (buf == வெற்று)
	    buf = buflist_findname_exp(name);
    }

    எனில் (buf != வெற்று)
	திரும்பு buf;

    buf = buflist_new(name == வெற்று || *name == NUL ? வெற்று : name,
	    வெற்று, (linenr_T)0, BLN_LISTED | BLN_NEW);
    எனில் (buf == வெற்று)
	திரும்பு வெற்று;
    prepare_buffer(buf);

    curbuf = buf;
    எனில் (செய்தி)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    திரும்பு buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    நிலையான வெறுமை
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    எனில் (opt->jo_set & JO_MODE)
	க்கு (part = PART_SOCK; part < PART_மொத்தம்; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    எனில் (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    எனில் (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    எனில் (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    எனில் (opt->jo_set & JO_TIMEOUT)
	க்கு (part = PART_SOCK; part < PART_மொத்தம்; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    எனில் (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    எனில் (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    எனில் (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    எனில் (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    எனில் (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    எனில் (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    எனில் (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    எனில் ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	எனில் (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	எனில் (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    எனில் (buf == வெற்று)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	இல்லையெனில்
	{
	    முழு செய்தி = TRUE;

	    எனில் (opt->jo_set2 & JO2_OUT_செய்தி)
		செய்தி = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, செய்தி);
	}
	எனில் (buf != வெற்று)
	{
	    எனில் (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    எனில் (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    இல்லையெனில்
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (எழுத்து *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		எனில் (buf->b_ml.ml_mfp == வெற்று)
		    prepare_buffer(buf);
	    }
	}
    }

    எனில் ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	எனில் (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	எனில் (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	இல்லையெனில் எனில் (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    எனில் (buf == வெற்று)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	இல்லையெனில்
	{
	    முழு செய்தி = TRUE;

	    எனில் (opt->jo_set2 & JO2_ERR_செய்தி)
		செய்தி = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, செய்தி);
	}
	எனில் (buf != வெற்று)
	{
	    எனில் (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    எனில் (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    இல்லையெனில்
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (எழுத்து *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		எனில் (buf->b_ml.ml_mfp == வெற்று)
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
    முழு
channel_parse_socketserver_address(
	char_u	*address,
	முழு	*port,
	char_u	**unix_path,
	bool	quiet)
{
    எழுத்து    *rest;
    long    val;

    எனில் (*address == NUL)
	செல் fail;

    எனில் (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	திரும்பு OK;
    }

    val = strtol((எழுத்து *)(address), &rest, 10);
    எனில் (val < 0 || val >= 65536 || *rest != NUL)
    {
	எனில் (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	திரும்பு FAIL;
    }
    *port = (முழு)val;

    திரும்பு OK;
fail:
    எனில் (!quiet)
	semsg(_(e_invalid_argument_str), address);
    திரும்பு FAIL;
}

/*
 * Implements ch_open().
 */
    நிலையான channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    எழுத்து	*rest;
    முழு		port = 0;
    முழு		is_ipv6 = FALSE;
    முழு		is_unix = FALSE;
    முழு		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு வெற்று;

    address = tv_get_string(&argvars[0]);
    எனில் (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	திரும்பு வெற்று;

    எனில் (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	திரும்பு வெற்று;
    }

    எனில் (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    இல்லையெனில் எனில் (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    இல்லையெனில் எனில் (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	எனில் (p == வெற்று || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    திரும்பு வெற்று;
	}
    }
    இல்லையெனில்
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	எனில் (p == வெற்று)
	{
	    semsg(_(e_invalid_argument_str), address);
	    திரும்பு வெற்று;
	}
    }

    எனில் (!is_unix && !is_stdio)
    {
	port = strtol((எழுத்து *)(p + 1), &rest, 10);
	எனில் (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    திரும்பு வெற்று;
	}
	எனில் (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	இல்லையெனில்
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    எனில் (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	செல் theend;
    எனில் (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	செல் theend;
    }

    எனில் (is_stdio)
	channel = channel_open_stdio();
    இல்லையெனில் எனில் (is_unix)
	channel = channel_open_unix((எழுத்து *)address, வெற்று);
    இல்லையெனில்
	channel = channel_open((எழுத்து *)address, port, opt.jo_waittime, வெற்று);
    எனில் (channel != வெற்று)
    {
	முழு	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	எனில் (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    திரும்பு channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
நிலையான channel_T *stdio_channel = வெற்று;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    நிலையான channel_T *
channel_open_stdio(வெறுமை)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    எனில் (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	திரும்பு வெற்று;
    }
    எனில் (stdio_channel != வெற்று)
    {
	emsg(_(e_cannot_open_stdio_channel));
	திரும்பு வெற்று;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	முழு	nul_fd;

	எனில் (!DuplicateHandle(proc, GetStdHandle(STD_உள்ளீடு_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    திரும்பு வெற்று;
	}
	எனில் (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    திரும்பு வெற்று;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, வெற்று, OPEN_EXISTING, 0,
		வெற்று);
	எனில் (hnul != INVALID_HANDLE_மதிப்பு)
	{
	    SetStdHandle(STD_உள்ளீடு_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	எனில் (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#இல்லையெனில்
    {
	முழு	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	எனில் (in_fd < 0 || out_fd < 0)
	{
	    எனில் (in_fd >= 0)
		close(in_fd);
	    எனில் (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    திரும்பு வெற்று;
	}
	(வெறுமை)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(வெறுமை)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	எனில் (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    எனில் (channel == வெற்று)
    {
	ch_error(வெற்று, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	திரும்பு வெற்று;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    திரும்பு channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    எழுத்து	*rest;
    முழு		port;
    முழு		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = வெற்று;

    எனில் (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	திரும்பு வெற்று;

    arg = tv_get_string(&argvars[0]);
    எனில் (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	திரும்பு வெற்று;
    }

    எனில் (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    இல்லையெனில்
    {
	port = strtol((எழுத்து *)(arg), &rest, 10);
	எனில் (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    திரும்பு வெற்று;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    எனில் (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	செல் theend;
    எனில் (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	செல் theend;
    }

    எனில் (is_unix)
	channel = channel_listen_unix((எழுத்து *)arg, வெற்று, true);
    இல்லையெனில்
	channel = channel_listen(port, வெற்று);
    எனில் (channel != வெற்று)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    திரும்பு channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	முழு port_in,
	வெறுமை (*nb_close_cb)(வெறுமை))
{
    முழு			sd = -1;
    கட்டமைப்பு sockaddr_in	server;
#ifndef FEAT_IPV6
    கட்டமைப்பு hostent	*host;
#endif
    முழு			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    எனில் (channel == வெற்று)
    {
	ch_error(வெற்று, "Cannot allocate channel.");
	திரும்பு வெற்று;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((எழுத்து *)&server, 0, அளவு(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    எனில் (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	திரும்பு வெற்று;
    }

#ifdef MSWIN
    எனில் (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (மாறிலி எழுத்து *)&val, அளவு(val)) < 0)
#இல்லையெனில்
    எனில் (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, அளவு(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	திரும்பு வெற்று;
    }

    // Bind the socket to the port
    எனில் (bind(sd, (கட்டமைப்பு sockaddr *)&server, அளவு(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	திரும்பு வெற்று;
    }

    எனில் (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	திரும்பு வெற்று;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    எனில் (port_in == 0)
    {
	கட்டமைப்பு sockaddr_in	addr;
	socklen_t		addr_len = அளவு(addr);

	எனில் (getsockname(sd, (கட்டமைப்பு sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (எழுத்து *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    திரும்பு channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	எழுத்து *path,
	வெறுமை (*nb_close_cb)(வெறுமை),
	bool replace)
{
    முழு			sd = -1;
    கட்டமைப்பு sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    எனில் (path == வெற்று || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == வெற்று ? (எழுத்து *)"" : path);
	திரும்பு வெற்று;
    }

    path_len = STRLEN(path);
    எனில் (path_len >= அளவு(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	திரும்பு வெற்று;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    எனில் (channel == வெற்று)
    {
	ch_error(வெற்று, "Cannot allocate channel.");
	திரும்பு வெற்று;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, அளவு(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    எனில் (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	திரும்பு வெற்று;
    }

    எனில் (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(கட்டமைப்பு sockaddr_un, sun_path) + path_len + 1;
    எனில் (bind(sd, (கட்டமைப்பு sockaddr *)&server, (முழு)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	திரும்பு வெற்று;
    }

    எனில் (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	திரும்பு வெற்று;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (எழுத்து *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    திரும்பு channel;
}

     வெறுமை
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    எனில் (*fd == INVALID_FD)
	திரும்பு;

    எனில் (part == PART_SOCK)
	sock_close(*fd);
    இல்லையெனில்
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	எனில் ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    எனில் (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    வெறுமை
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    எனில் (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#எனில் defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	எனில் (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    எனில் (out != INVALID_FD)
    {
#எனில் defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#எனில் defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    எனில் (err != INVALID_FD)
    {
#எனில் defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#எனில் defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    வெறுமை
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    எனில் (job->jv_in_buf == வெற்று)
	திரும்பு;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (எழுத்து *)in_part->ch_bufref.br_buf->b_ffname);
    எனில் (options->jo_set & JO_IN_உச்சி)
    {
	எனில் (options->jo_in_உச்சி == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_உச்சி =
		in_part->ch_bufref.br_buf->b_ml.ml_வரி_மொத்தம் + 1;
	}
	இல்லையெனில்
	    in_part->ch_buf_உச்சி = options->jo_in_உச்சி;
    }
    இல்லையெனில்
	in_part->ch_buf_உச்சி = 1;
    எனில் (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    இல்லையெனில்
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_வரி_மொத்தம்;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    நிலையான வெறுமை
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	முழு	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    எனில் (item == வெற்று)
	திரும்பு;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = வெற்று;
    எனில் (item->cq_prev == வெற்று)
	head->cq_next = item;
    இல்லையெனில்
	item->cq_prev->cq_next = item;
}

    நிலையான வெறுமை
write_buf_வரி(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *வரி = ml_get_buf(buf, lnum, FALSE);
    முழு	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    முழு	    i;

    // Need to make a copy to be able to append a NL.
    எனில் ((p = alloc(len + 2)) == வெற்று)
	திரும்பு;
    memcpy((எழுத்து *)p, (எழுத்து *)வரி, len);

    எனில் (channel->ch_write_text_mode)
	p[len] = CAR;
    இல்லையெனில்
    {
	க்கு (i = 0; i < len; ++i)
	    எனில் (p[i] == NL)
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
    நிலையான முழு
can_write_buf_வரி(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    எனில் (in_part->ch_fd == INVALID_FD)
	திரும்பு FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    எனில் (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    இல்லையெனில் எனில் (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# எனில் defined(HAVE_SELECT)
	கட்டமைப்பு timeval	tval;
	fd_set		wfds;
	முழு		ret;

	FD_ZERO(&wfds);
	FD_SET((முழு)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	க்கு (;;)
	{
	    ret = select((முழு)in_part->ch_fd + 1, வெற்று, &wfds, வெற்று, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    எனில் (ret == -1 && errno == EINTR)
		தொடர்;
#  endif
	    எனில் (ret <= 0 || in_part->ch_block_write == 1)
	    {
		எனில் (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		இல்லையெனில்
		    ch_log(channel, "Input not ready for writing");
		திரும்பு FALSE;
	    }
	    நிறுத்து;
	}
# இல்லையெனில்
	கட்டமைப்பு pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	எனில் (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    திரும்பு FALSE;
	}
	எனில் (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    திரும்பு FALSE;
	}
# endif
    }
#endif
    திரும்பு TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    வெறுமை
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    முழு		written = 0;

    எனில் (buf == வெற்று || in_part->ch_buf_append)
	திரும்பு;  // no buffer or using appending
    எனில் (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == வெற்று)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = வெற்று;
	திரும்பு;
    }

    க்கு (lnum = in_part->ch_buf_உச்சி; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_வரி_மொத்தம்; ++lnum)
    {
	எனில் (!can_write_buf_வரி(channel))
	    நிறுத்து;
	write_buf_வரி(buf, lnum, channel);
	++written;
    }

    எனில் (written == 1)
	ch_log(channel, "written line %d to channel", (முழு)lnum - 1);
    இல்லையெனில் எனில் (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_உச்சி = lnum;
    எனில் (lnum > buf->b_ml.ml_வரி_மொத்தம் || lnum > in_part->ch_buf_bot)
    {
#எனில் defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	எனில் (channel->ch_job != வெற்று)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = வெற்று;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	எனில் (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    இல்லையெனில்
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_வரி_மொத்தம் - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    வெறுமை
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	க்கு (part = PART_SOCK; part < PART_மொத்தம்; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    எனில் (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = வெற்று;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    வெறுமை
channel_write_உள்ளீடு(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    எனில் (in_part->ch_writeque.wq_next != வெற்று)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    இல்லையெனில் எனில் (in_part->ch_bufref.br_buf != வெற்று)
    {
	எனில் (in_part->ch_buf_append)
	    channel_write_new_வரிகள்(in_part->ch_bufref.br_buf);
	இல்லையெனில்
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    வெறுமை
channel_write_any_வரிகள்(வெறுமை)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_உள்ளீடு(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    வெறுமை
channel_write_new_வரிகள்(buf_T *buf)
{
    channel_T	*channel;
    முழு		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	முழு	    written = 0;

	எனில் (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    எனில் (in_part->ch_fd == INVALID_FD)
		தொடர்;  // pipe was closed
	    found_one = TRUE;
	    க்கு (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_வரி_மொத்தம்;
								       ++lnum)
	    {
		எனில் (!can_write_buf_வரி(channel))
		    நிறுத்து;
		write_buf_வரி(buf, lnum, channel);
		++written;
	    }

	    எனில் (written == 1)
		ch_log(channel, "written line %d to channel", (முழு)lnum - 1);
	    இல்லையெனில் எனில் (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    எனில் (lnum < buf->b_ml.ml_வரி_மொத்தம்)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_வரி_மொத்தம் - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    எனில் (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    நிலையான வெறுமை
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    எனில் (safe_to_invoke_callback == 0)
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

    திரும்பு head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *முனை)
{
    char_u  *buffer = முனை->rq_buffer;
    long_u  i;

    க்கு (i = 0; i < முனை->rq_buflen; ++i)
	எனில் (buffer[i] == NL)
	    திரும்பு buffer + i;
    திரும்பு வெற்று;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, முழு *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *முனை = head->rq_next;
    char_u *p;

    எனில் (முனை == வெற்று)
	திரும்பு வெற்று;
    எனில் (outlen != வெற்று)
	*outlen += முனை->rq_buflen;
    // dispose of the node but keep the buffer
    p = முனை->rq_buffer;
    head->rq_next = முனை->rq_next;
    எனில் (முனை->rq_next == வெற்று)
	head->rq_prev = வெற்று;
    இல்லையெனில்
	முனை->rq_next->rq_prev = வெற்று;
    vim_free(முனை);
    திரும்பு p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    நிலையான char_u *
channel_get_all(channel_T *channel, ch_part_T part, முழு *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *முனை;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    க்கு (முனை = head->rq_next; முனை != வெற்று; முனை = முனை->rq_next)
	len += முனை->rq_buflen;
    res = alloc(len + 1);
    எனில் (res == வெற்று)
	திரும்பு வெற்று;
    p = res;
    க்கு (முனை = head->rq_next; முனை != வெற்று; முனை = முனை->rq_next)
    {
	mch_memmove(p, முனை->rq_buffer, முனை->rq_buflen);
	p += முனை->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    செய்
    {
	p = channel_get(channel, part, வெற்று);
	vim_free(p);
    } போது (p != வெற்று);

    எனில் (outlen != வெற்று)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	திரும்பு res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    போது (p < res + len)
    {
	எனில் (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	இல்லையெனில் எனில் (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    எனில் (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		போது (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		எனில் (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    திரும்பு res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    வெறுமை
channel_consume(channel_T *channel, ch_part_T part, முழு len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *முனை = head->rq_next;
    char_u *buf = முனை->rq_buffer;

    எனில் (len < 0 || (long_u)len > முனை->rq_buflen)
	திரும்பு;
    mch_memmove(buf, buf + len, முனை->rq_buflen - len);
    முனை->rq_buflen -= len;
    முனை->rq_buffer[முனை->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    முழு
channel_collapse(channel_T *channel, ch_part_T part, முழு want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*முனை = head->rq_next;
    readq_T	*last_முனை;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    எனில் (முனை == வெற்று || முனை->rq_next == வெற்று)
	திரும்பு FAIL;

    last_முனை = முனை->rq_next;
    len = முனை->rq_buflen + last_முனை->rq_buflen;
    எனில் (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	போது (last_முனை->rq_next != வெற்று
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_முனை) == வெற்று))
	{
	    last_முனை = last_முனை->rq_next;
	    len += last_முனை->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    எனில் (newbuf == வெற்று)
	திரும்பு FAIL;	    // out of memory
    mch_memmove(p, முனை->rq_buffer, முனை->rq_buflen);
    p += முனை->rq_buflen;
    vim_free(முனை->rq_buffer);
    முனை->rq_buffer = newbuf;
    க்கு (n = முனை; n != last_முனை; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    முனை->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    க்கு (n = முனை->rq_next; n != last_முனை; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    முனை->rq_next = last_முனை->rq_next;
    எனில் (last_முனை->rq_next == வெற்று)
	head->rq_prev = முனை;
    இல்லையெனில்
	last_முனை->rq_next->rq_prev = முனை;
    vim_free(last_முனை);
    திரும்பு OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    நிலையான முழு
channel_save(channel_T *channel, ch_part_T part, char_u *buf, முழு len,
						      முழு prepend, எழுத்து *lead)
{
    readq_T *முனை;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    முழு	    i;

    முனை = ALLOC_ONE(readq_T);
    எனில் (முனை == வெற்று)
	திரும்பு FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    முனை->rq_buffer = alloc(len + 1);
    எனில் (முனை->rq_buffer == வெற்று)
    {
	vim_free(முனை);
	திரும்பு FAIL;	    // out of memory
    }

    எனில் (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = முனை->rq_buffer;
	க்கு (i = 0; i < len; ++i)
	    எனில் (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	முனை->rq_buflen = (long_u)(p - முனை->rq_buffer);
    }
    இல்லையெனில்
    {
	mch_memmove(முனை->rq_buffer, buf, len);
	முனை->rq_buffer[len] = NUL;
	முனை->rq_buflen = (long_u)len;
    }

    எனில் (prepend)
    {
	// prepend node to the head of the queue
	முனை->rq_next = head->rq_next;
	முனை->rq_prev = வெற்று;
	எனில் (head->rq_next == வெற்று)
	    head->rq_prev = முனை;
	இல்லையெனில்
	    head->rq_next->rq_prev = முனை;
	head->rq_next = முனை;
    }
    இல்லையெனில்
    {
	// append node to the tail of the queue
	முனை->rq_next = வெற்று;
	முனை->rq_prev = head->rq_prev;
	எனில் (head->rq_prev == வெற்று)
	    head->rq_next = முனை;
	இல்லையெனில்
	    head->rq_prev->rq_next = முனை;
	head->rq_prev = முனை;
    }

    எனில் (ch_log_active() && lead != வெற்று)
	ch_log_literal(lead, channel, part, buf, len);

    திரும்பு OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    முழு
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, வெற்று);
    முழு		keeplen;
    முழு		addlen;
    char_u	*p;

    எனில் (next == வெற்று)
	திரும்பு FALSE;

    keeplen = reader->js_end - reader->js_buf;
    எனில் (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (முழு)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	எனில் (p == வெற்று)
	{
	    vim_free(next);
	    திரும்பு FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    திரும்பு TRUE;
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
    நிலையான முழு
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*வரி_start;
    char_u	*p;
    int_u	hdr_len;
    முழு		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    போது (TRUE)
    {
	வரி_start = p;
	போது (*p != NUL && *p != '\n')
	    p++;
	எனில் (*p == NUL)			// partial header
	    திரும்பு MAYBE;
	p++;

	// process the content length field (if present)
	எனில் ((p - வரி_start > 16)
		&& STRNICMP(வரி_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((எழுத்து *)வரி_start + 16, வெற்று, 10);
	    எனில் (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		திரும்பு FAIL;
	}

	எனில் ((p - வரி_start) == 2 && வரி_start[0] == '\r' &&
		வரி_start[1] == '\n')
	    // reached the empty line
	    நிறுத்து;
    }

    எனில் (payload_len == -1)
	// Content-Length field is not present in the header
	திரும்பு FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    எனில் (jsbuf_len < hdr_len + payload_len)
	திரும்பு MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    திரும்பு OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    முழு
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    முழு		status = OK;
    முழு		ret;

    எனில் (channel_peek(channel, part) == வெற்று)
	திரும்பு FALSE;

    reader.js_buf = channel_get(channel, part, வெற்று);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    எனில் (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    எனில் (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    எனில் (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	எனில் (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	இல்லையெனில் எனில் (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    எனில் (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    இல்லையெனில்
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	இல்லையெனில்
	{
	    item = ALLOC_ONE(jsonq_T);
	    எனில் (item == வெற்று)
		clear_tv(&listtv);
	    இல்லையெனில்
	    {
		item->jq_no_callback = FALSE;
		item->jq_மதிப்பு = alloc_tv();
		எனில் (item->jq_மதிப்பு == வெற்று)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		இல்லையெனில்
		{
		    *item->jq_மதிப்பு = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = வெற்று;
		    எனில் (item->jq_prev == வெற்று)
			head->jq_next = item;
		    இல்லையெனில்
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    எனில் (status == OK)
	chanpart->ch_wait_len = 0;
    இல்லையெனில் எனில் (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	எனில் (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (முழு)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_மொத்தம்() + 100L;
#இல்லையெனில்
	    gettimeofday(&chanpart->ch_deadline, வெற்று);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    எனில் (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	இல்லையெனில்
	{
	    முழு timeout;
#ifdef MSWIN
	    timeout = (முழு)(Get_Tick_மொத்தம்() - chanpart->ch_deadline) > 0;
#இல்லையெனில்
	    {
		கட்டமைப்பு timeval now_tv;

		gettimeofday(&now_tv, வெற்று);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    எனில் (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    இல்லையெனில்
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    எனில் (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    இல்லையெனில் எனில் (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(முழு)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, வெற்று);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    இல்லையெனில்
	ret = FALSE;

    vim_free(reader.js_buf);
    திரும்பு ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    நிலையான வெறுமை
remove_cb_முனை(cbq_T *head, cbq_T *முனை)
{
    எனில் (முனை->cq_prev == வெற்று)
	head->cq_next = முனை->cq_next;
    இல்லையெனில்
	முனை->cq_prev->cq_next = முனை->cq_next;
    எனில் (முனை->cq_next == வெற்று)
	head->cq_prev = முனை->cq_prev;
    இல்லையெனில்
	முனை->cq_next->cq_prev = முனை->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    வெறுமை
remove_json_முனை(jsonq_T *head, jsonq_T *முனை)
{
    எனில் (முனை->jq_prev == வெற்று)
	head->jq_next = முனை->jq_next;
    இல்லையெனில்
	முனை->jq_prev->jq_next = முனை->jq_next;
    எனில் (முனை->jq_next == வெற்று)
	head->jq_prev = முனை->jq_prev;
    இல்லையெனில்
	முனை->jq_next->jq_prev = முனை->jq_prev;
    vim_free(முனை);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    நிலையான வெறுமை
channel_add_block_id(chanpart_T *chanpart, முழு id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    எனில் (gap->ga_growsize == 0)
	ga_init2(gap, அளவு(முழு), 10);
    எனில் (ga_grow(gap, 1) == OK)
    {
	((முழு *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    நிலையான வெறுமை
channel_remove_block_id(chanpart_T *chanpart, முழு id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    முழு		i;

    க்கு (i = 0; i < gap->ga_len; ++i)
	எனில் (((முழு *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    எனில் (i < gap->ga_len)
	    {
		முழு *p = ((முழு *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * அளவு(முழு));
	    }
	    திரும்பு;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    நிலையான முழு
channel_has_block_id(chanpart_T *chanpart, முழு id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    முழு		i;

    க்கு (i = 0; i < gap->ga_len; ++i)
	எனில் (((முழு *)gap->ga_data)[i] == id)
	    திரும்பு TRUE;
    திரும்பு FALSE;
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
    நிலையான முழு
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	முழு	    id,
	முழு	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    போது (item != வெற்று)
    {
	list_T	    *l;
	typval_T    *tv;

	எனில் (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_மதிப்பு->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	இல்லையெனில்
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    எனில் (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		எனில் (id > 0)
		{
		    எனில் (item->jq_மதிப்பு->v_type != VAR_DICT)
			செல் nextitem;
		    d = item->jq_மதிப்பு->vval.v_dict;
		    எனில் (d == வெற்று)
			செல் nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    எனில் (dict_has_சாவி(d, "method"))
			செல் nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    எனில் (di == வெற்று)
			செல் nextitem;
		    tv = &di->di_tv;
		}
		இல்லையெனில்
		    tv = item->jq_மதிப்பு;
	    }
	    இல்லையெனில்
	    {
		எனில் (id > 0)
		{
		    எனில் (item->jq_மதிப்பு->v_type != VAR_DICT)
			செல் nextitem;
		    d = item->jq_மதிப்பு->vval.v_dict;
		    எனில் (d == வெற்று)
			செல் nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    எனில் (di == வெற்று)
			செல் nextitem;
		    tv = &di->di_tv;
		}
		இல்லையெனில்
		    tv = item->jq_மதிப்பு;
	    }
	}

	எனில் ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_எண் && tv->vval.v_எண் == id)
	      || (id <= 0 && (tv->v_type != VAR_எண்
		 || tv->vval.v_எண் == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_எண்)))))
	{
	    *rettv = item->jq_மதிப்பு;
	    எனில் (tv->v_type == VAR_எண்)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_எண்);
	    remove_json_முனை(head, item);
	    திரும்பு OK;
	}
nextitem:
	item = item->jq_next;
    }
    திரும்பு FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    நிலையான வெறுமை
channel_தள்ளு_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    எனில் (head->jq_prev != வெற்று && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = வெற்று;
    இல்லையெனில் போது (item != வெற்று && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    எனில் (newitem == வெற்று)
    {
	clear_tv(rettv);
	திரும்பு;
    }

    newitem->jq_மதிப்பு = alloc_tv();
    எனில் (newitem->jq_மதிப்பு == வெற்று)
    {
	vim_free(newitem);
	clear_tv(rettv);
	திரும்பு;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_மதிப்பு = *rettv;
    எனில் (item == வெற்று)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = வெற்று;
	எனில் (newitem->jq_prev == வெற்று)
	    head->jq_next = newitem;
	இல்லையெனில்
	    newitem->jq_prev->jq_next = newitem;
    }
    இல்லையெனில்
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	எனில் (newitem->jq_next == வெற்று)
	    head->jq_prev = newitem;
	இல்லையெனில்
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    நிலையான வெறுமை
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    முழு	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    எனில் (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	எனில் (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	திரும்பு;
    }
    arg = argv[1].vval.v_string;
    எனில் (arg == வெற்று)
	arg = (char_u *)"";

    எனில் (STRCMP(cmd, "ex") == 0)
    {
	முழு	called_emsg_before = called_emsg;
	char_u	*p = arg;
	முழு	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (எழுத்து *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	எனில் (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	எனில் (do_emsg_silent)
	    --emsg_silent;
	எனில் (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (எழுத்து *)get_vim_var_str(VV_ERRMSG));
    }
    இல்லையெனில் எனில் (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (எழுத்து *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_மொத்தம் = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    இல்லையெனில் எனில் (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    இல்லையெனில் எனில் (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	முழு is_call = cmd[0] == 'c';
	முழு id_idx = is_call ? 3 : 2;

	எனில் (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_எண்)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    எனில் (p_verbose > 2)
		emsg(_(e_last_argument_for_கோவை_call_must_be_எண்));
	}
	இல்லையெனில் எனில் (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    எனில் (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	இல்லையெனில்
	{
	    typval_T	*tv = வெற்று;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = வெற்று;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    எனில் (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (எழுத்து *)arg);
		tv = eval_கோவை(arg, வெற்று);
	    }
	    இல்லையெனில்
	    {
		ch_log(channel, "Calling '%s'", (எழுத்து *)arg);
		எனில் (func_call(arg, &argv[2], வெற்று, வெற்று, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    எனில் (argv[id_idx].v_type == VAR_எண்)
	    {
		முழு id = argv[id_idx].vval.v_எண்;

		எனில் (tv != வெற்று)
		    json = json_encode_nr_கோவை(id, tv, options | JSON_NL);
		எனில் (tv == வெற்று || (json != வெற்று && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_கோவை(id, &err_tv, options | JSON_NL);
		}
		எனில் (json != வெற்று)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (முழு)STRLEN(json), (எழுத்து *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    எனில் (tv == &res_tv)
		clear_tv(tv);
	    இல்லையெனில்
		free_tv(tv);
	}
    }
    இல்லையெனில் எனில் (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (எழுத்து *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    நிலையான வெறுமை
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (எழுத்து *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_முனை(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    நிலையான வெறுமை
append_to_buffer(
    buf_T	*buffer,
    char_u	*செய்தி,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_வரி_மொத்தம்;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    முழு		save_p_ma = buffer->b_p_ma;
    முழு		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    எனில் (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	எனில் (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	திரும்பு;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    எனில் (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (முழு)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    எனில் (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	திரும்பு;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    எனில் (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, செய்தி, TRUE);
	lnum = 0;
    }
    இல்லையெனில்
	ml_append(lnum, செய்தி, 0, FALSE);
    appended_வரிகள்_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    எனில் (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    இல்லையெனில்
	buffer->b_p_ma = save_p_ma;

    எனில் (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    எனில் (wp->w_buffer == buffer)
	    {
		முழு move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		எனில் (move_cursor || wp->w_topline > buffer->b_ml.ml_வரி_மொத்தம்)
		{
		    win_T *save_curwin = curwin;

		    எனில் (move_cursor)
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

    எனில் (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    எனில் (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_வரி_மொத்தம்;
	}
    }
}

    நிலையான வெறுமை
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *செய்தி;

    போது ((செய்தி = channel_get(channel, part, வெற்று)) != வெற்று)
    {
	ch_log(channel, "Dropping message '%s'", (எழுத்து *)செய்தி);
	vim_free(செய்தி);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    நிலையான முழு
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    திரும்பு ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    நிலையான முழு
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*செய்தி = வெற்று;
    blob_T	*blob = வெற்று;
    typval_T	*listtv = வெற்று;
    typval_T	argv[CH_JSON_MAX_ARGS];
    முழு		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = வெற்று;
    buf_T	*buffer = வெற்று;
    char_u	*p;
    முழு		called_otc;		// one time callbackup
    முழு		raw_len = 0;

    எனில் (channel->ch_nb_close_cb != வெற்று
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	திரும்பு FALSE;

    // Use a message-specific callback, part callback or channel callback
    க்கு (cbitem = cbhead->cq_next; cbitem != வெற்று; cbitem = cbitem->cq_next)
	எனில் (cbitem->cq_seq_nr == 0)
	    நிறுத்து;
    எனில் (cbitem != வெற்று)
	callback = &cbitem->cq_callback;
    இல்லையெனில் எனில் (ch_part->ch_callback.cb_name != வெற்று)
	callback = &ch_part->ch_callback;
    இல்லையெனில் எனில் (channel->ch_callback.cb_name != வெற்று)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    எனில் (buffer != வெற்று && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == வெற்று))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = வெற்று;
	buffer = வெற்று;
    }

    எனில் (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	முழு		argc = 0;

	// Get any json message in the queue.
	எனில் (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    எனில் (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(வெறுமை)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    எனில் (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		திரும்பு FALSE;
	}

	எனில் (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    எனில் (d != வெற்று)
	    {
		எனில் (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		இல்லையெனில்
		    di = dict_find(d, (char_u *)"request_seq", -1);
		எனில் (di != வெற்று && di->di_tv.v_type == VAR_எண்)
		    seq_nr = di->di_tv.vval.v_எண்;
	    }

	    argv[1] = *listtv;
	}
	இல்லையெனில்
	{
	    க்கு (item = listtv->vval.v_list->lv_first;
		    item != வெற்று && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    போது (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    எனில் (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		திரும்பு TRUE;
	    }

	    எனில் (argv[0].v_type != VAR_எண்)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		திரும்பு FALSE;
	    }
	    seq_nr = argv[0].vval.v_எண்;
	}
    }
    இல்லையெனில் எனில் (channel_peek(channel, part) == வெற்று)
    {
	// nothing to read on RAW or NL channel
	திரும்பு FALSE;
    }
    இல்லையெனில்
    {
	// If there is no callback or buffer drop the message.
	எனில் (callback == வெற்று && buffer == வெற்று)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    எனில் (channel->ch_close_cb.cb_name == வெற்று && !channel->ch_drop_never)
		drop_messages(channel, part);
	    திரும்பு FALSE;
	}

	எனில் (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = வெற்று;
	    char_u  *buf;
	    readq_T *முனை;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    போது (TRUE)
	    {
		முனை = channel_peek(channel, part);
		nl = channel_first_nl(முனை);
		எனில் (nl != வெற்று)
		    நிறுத்து;
		எனில் (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    எனில் (ch_part->ch_fd == INVALID_FD && முனை->rq_buflen > 0)
			நிறுத்து;
		    திரும்பு FALSE; // incomplete message
		}
	    }
	    buf = முனை->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    க்கு (p = buf; (nl == வெற்று || p < nl)
					    && p < buf + முனை->rq_buflen; ++p)
		எனில் (*p == NUL)
		    *p = NL;

	    எனில் (nl == வெற்று)
	    {
		// get the whole buffer, drop the NL
		செய்தி = channel_get(channel, part, வெற்று);
	    }
	    இல்லையெனில் எனில் (nl + 1 == buf + முனை->rq_buflen)
	    {
		// get the whole buffer
		செய்தி = channel_get(channel, part, வெற்று);
		*nl = NUL;
	    }
	    இல்லையெனில்
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		செய்தி = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (முழு)(nl - buf) + 1);
	    }
	}
	இல்லையெனில்
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    செய்தி = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : வெற்று);
	}

	எனில் (செய்தி == வெற்று)
	    திரும்பு FALSE; // out of memory (and avoids Coverity warning)

	எனில் (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    எனில் (blob == வெற்று)
	    {
		vim_free(செய்தி);
		திரும்பு FALSE;
	    }
	    எனில் (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(செய்தி);
		திரும்பு FALSE;
	    }
	    எனில் (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, செய்தி, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	இல்லையெனில்
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = செய்தி;
	}
    }

    called_otc = FALSE;
    எனில் (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	முழு lsp_req_செய்தி = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	எனில் ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_சாவி(argv[1].vval.v_dict, "method"))
	    lsp_req_செய்தி = TRUE;

	எனில் (!lsp_req_செய்தி)
	{
	    க்கு (cbitem = cbhead->cq_next; cbitem != வெற்று;
		    cbitem = cbitem->cq_next)
	    {
		எனில் (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    நிறுத்து;
		}
	    }
	}
    }

    எனில் (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	எனில் (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    எனில் (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_தள்ளு_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_எண்;
		free_tv(listtv);
		listtv = வெற்று;
	    }
	    இல்லையெனில்
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    இல்லையெனில் எனில் (callback != வெற்று || buffer != வெற்று)
    {
	எனில் (buffer != வெற்று)
	{
	    எனில் (செய்தி == வெற்று)
		// JSON or JS mode: re-encode the message.
		செய்தி = json_encode(listtv, ch_mode);
	    எனில் (செய்தி != வெற்று)
	    {
#ifdef FEAT_TERMINAL
		எனில் (buffer->b_term != வெற்று)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    எனில் (part == PART_ERR)
		    {
			char_u *cp;
			முழு    extra = 0;

			க்கு (cp = செய்தி; *cp != NUL; ++cp)
			    எனில் (*cp == NL && (cp == செய்தி || cp[-1] != CAR))
				++extra;
			எனில் (extra > 0)
			{
			    char_u *crlf_செய்தி = alloc(STRLEN(செய்தி) + extra + 1);

			    எனில் (crlf_செய்தி != வெற்று)
			    {
				char_u *q = crlf_செய்தி;

				க்கு (cp = செய்தி; *cp != NUL; ++cp)
				{
				    எனில் (*cp == NL && (cp == செய்தி || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_செய்தி, channel);
				vim_free(crlf_செய்தி);
			    }
			}
			இல்லையெனில்
			    write_to_term(buffer, செய்தி, channel);
		    }
		    இல்லையெனில்
			write_to_term(buffer, செய்தி, channel);
		}
		இல்லையெனில்
#endif
		    append_to_buffer(buffer, செய்தி, channel, part);
	    }
	}

	எனில் (callback != வெற்று)
	{
	    எனில் (cbitem != வெற்று)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    இல்லையெனில்
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (எழுத்து *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    இல்லையெனில்
	ch_log(channel, "Dropping message %d", seq_nr);

    எனில் (listtv != வெற்று)
	free_tv(listtv);
    எனில் (blob != வெற்று)
	blob_unref(blob);
    vim_free(செய்தி);

    திரும்பு TRUE;
}

#எனில் defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    முழு
channel_can_write_to(channel_T *channel)
{
    திரும்பு channel != வெற்று && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    முழு
channel_is_open(channel_T *channel)
{
    திரும்பு channel != வெற்று && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    நிலையான வெறுமை *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    எனில் (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	எனில் (head->jq_next == வெற்று)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	திரும்பு head->jq_next;
    }
    திரும்பு channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    நிலையான முழு
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    திரும்பு channel_readahead_pointer(channel, part) != வெற்று;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    நிலையான எழுத்து *
channel_status(channel_T *channel, முழு req_part)
{
    ch_part_T part;
    முழு has_readahead = FALSE;

    எனில் (channel == வெற்று)
	 திரும்பு "fail";
    எனில் (req_part == PART_OUT)
    {
	எனில் (channel->CH_OUT_FD != INVALID_FD)
	    திரும்பு "open";
	எனில் (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    இல்லையெனில் எனில் (req_part == PART_ERR)
    {
	எனில் (channel->CH_ERR_FD != INVALID_FD)
	    திரும்பு "open";
	எனில் (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    இல்லையெனில்
    {
	எனில் (channel_is_open(channel))
	    திரும்பு "open";
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	    எனில் (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		நிறுத்து;
	    }
    }

    எனில் (has_readahead)
	திரும்பு "buffered";
    திரும்பு "closed";
}

    நிலையான வெறுமை
channel_part_info(channel_T *channel, dict_T *dict, எழுத்து *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    எழுத்து	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    எனில் (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    இல்லையெனில் எனில் (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    இல்லையெனில்
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (முழு)s.length);

    STRCPY(namebuf + tail, "mode");
    தேர்வு (chanpart->ch_mode)
    {
	நிலை CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    நிறுத்து;
	நிலை CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    நிறுத்து;
	நிலை CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    நிறுத்து;
	நிலை CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    நிறுத்து;
	நிலை CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    நிறுத்து;
	நிலை CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    நிறுத்து;
	நிலை CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    நிறுத்து;
	இயல்பு:
	    STR_LITERAL_SET(s, "");
	    நிறுத்து;
    }
    dict_add_string_len(dict, namebuf, s.string, (முழு)s.length);

    STRCPY(namebuf + tail, "io");
    எனில் (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    இல்லையெனில் தேர்வு (chanpart->ch_io)
    {
	நிலை JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    நிறுத்து;
	நிலை JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    நிறுத்து;
	நிலை JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    நிறுத்து;
	நிலை JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    நிறுத்து;
	நிலை JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    நிறுத்து;
	இயல்பு:
	    STR_LITERAL_SET(s, "");
	    நிறுத்து;
    }
    dict_add_string_len(dict, namebuf, s.string, (முழு)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_எண்(dict, namebuf, chanpart->ch_timeout);
}

    நிலையான வெறுமை
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_எண்(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    எனில் (channel->ch_hostname != வெற்று)
    {
	எனில் (channel->ch_port)
	    dict_add_எண்(dict, "port", channel->ch_port);
	இல்லையெனில்
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    இல்லையெனில்
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
    வெறுமை
channel_close(channel_T *channel, முழு invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    எனில் (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	எனில் (channel->ch_close_cb.cb_name != வெற்று)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	{
	    எனில் (channel->ch_close_cb.cb_name != வெற்று
			    || channel->ch_part[part].ch_bufref.br_buf != வெற்று)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		எனில் (channel->ch_close_cb.cb_name == வெற்று)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		போது (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	எனில் (channel->ch_close_cb.cb_name != வெற்று)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (எழுத்து *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      எனில் (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      எனில் (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  க்கு (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = வெற்று;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = வெற்று;
    channel->ch_ss_close_cb = வெற்று;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    நிலையான வெறுமை
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    நிலையான வெறுமை
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    எனில் (wq->wq_next == வெற்று)
	wq->wq_prev = வெற்று;
    இல்லையெனில்
	wq->wq_next->wq_prev = வெற்று;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    நிலையான வெறுமை
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    போது (channel_peek(channel, part) != வெற்று)
	vim_free(channel_get(channel, part, வெற்று));

    போது (cb_head->cq_next != வெற்று)
    {
	cbq_T *முனை = cb_head->cq_next;

	remove_cb_முனை(cb_head, முனை);
	free_callback(&முனை->cq_callback);
	vim_free(முனை);
    }

    போது (json_head->jq_next != வெற்று)
    {
	free_tv(json_head->jq_next->jq_மதிப்பு);
	remove_json_முனை(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    போது (ch_part->ch_writeque.wq_next != வெற்று)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    வெறுமை
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

#எனில் defined(EXITFREE)
    வெறுமை
channel_free_all(வெறுமை)
{
    channel_T *channel;

    ch_log(வெற்று, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_செய்தி_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    நிலையான முழு
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    எனில் (in_part->ch_writeque.wq_next != வெற்று)
	திரும்பு TRUE;
    எனில் (buf == வெற்று)
	திரும்பு FALSE;
    திரும்பு in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_வரி_மொத்தம்)
	    : (in_part->ch_buf_உச்சி <= in_part->ch_buf_bot
			    && in_part->ch_buf_உச்சி <= buf->b_ml.ml_வரி_மொத்தம்);
}

#எனில் defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    நிலையான முழு
channel_fill_wfds(முழு maxfd_arg, fd_set *wfds)
{
    முழு		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	எனில் (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (முழு)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((முழு)in_part->ch_fd, wfds);
	    எனில் ((முழு)in_part->ch_fd >= maxfd)
		maxfd = (முழு)in_part->ch_fd + 1;
	}
    }
    திரும்பு maxfd;
}
#இல்லையெனில்
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    நிலையான முழு
channel_fill_poll_write(முழு nfd_in, கட்டமைப்பு pollfd *fds)
{
    முழு		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	எனில் (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	இல்லையெனில்
	    in_part->ch_poll_idx = -1;
    }
    திரும்பு nfd;
}
#endif

வகைவரை எண்ணிக்கை {
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
    நிலையான channel_wait_result
channel_wait(channel_T *channel, sock_T fd, முழு timeout)
{
    எனில் (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    எனில் (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	முழு	sleep_time;
	DWORD	deadline = Get_Tick_மொத்தம்() + timeout;
	முழு	delay = 1;

	// reading from a pipe, not a socket
	போது (TRUE)
	{
	    முழு r = PeekNamedPipe((HANDLE)fd, வெற்று, 0, வெற்று, &nread, வெற்று);

	    எனில் (r && nread > 0)
		திரும்பு CW_READY;

	    எனில் (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, வெற்று);
	    }
	    இல்லையெனில் எனில் (r == 0)
		திரும்பு CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_வரிகள்();

	    sleep_time = deadline - Get_Tick_மொத்தம்();
	    எனில் (sleep_time <= 0)
		நிறுத்து;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    எனில் (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    எனில் (delay > 10)
		delay = 10;
	}
    }
    இல்லையெனில்
#endif
    {
#எனில் defined(HAVE_SELECT)
	கட்டமைப்பு timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	முழு		ret;
	முழு		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	க்கு (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((முழு)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (முழு)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, வெற்று, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    எனில் (ret == -1 && errno == EINTR)
		தொடர்;
# endif
	    எனில் (ret > 0)
	    {
		எனில் (FD_ISSET(fd, &rfds))
		    திரும்பு CW_READY;
		channel_write_any_வரிகள்();
		தொடர்;
	    }
	    நிறுத்து;
	}
#இல்லையெனில்
	க்கு (;;)
	{
	    கட்டமைப்பு pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    முழு		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    எனில் (poll(fds, nfd, timeout) > 0)
	    {
		எனில் (fds[0].revents & POLLIN)
		    திரும்பு CW_READY;
		channel_write_any_வரிகள்();
		தொடர்;
	    }
	    நிறுத்து;
	}
#endif
    }
    திரும்பு CW_NOT_READY;
}

    நிலையான வெறுமை
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, முழு is_err, எழுத்து *func)
{
    எழுத்து	செய்தி[] = "%s(): Read %s from ch_part[%d], closing";

    எனில் (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, செய்தி, func, "error", part);
    இல்லையெனில்
	ch_log(channel, செய்தி, func, "EOF", part);

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
    எனில் (channel->ch_nb_close_cb != வெற்று)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_செய்தி_RAW,
			      (முழு)STRLEN_LITERAL(DETACH_செய்தி_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    எனில் (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	எனில் (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    நிலையான வெறுமை
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    எனில் (channel->ch_nb_close_cb != வெற்று)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    எனில் (channel->ch_ss_close_cb != வெற்று)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    நிலையான வெறுமை
channel_read(channel_T *channel, ch_part_T part, எழுத்து *func)
{
    நிலையான char_u	*buf = வெற்று;
    முழு			len = 0;
    முழு			readlen = 0;
    sock_T		fd;
    முழு			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    எனில் (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	திரும்பு;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    எனில் (buf == வெற்று)
    {
	buf = alloc(MAXMSGSIZE);
	எனில் (buf == வெற்று)
	    திரும்பு;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    க்கு (;;)
    {
	எனில் (channel_wait(channel, fd, 0) != CW_READY)
	    நிறுத்து;
	எனில் (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    கட்டமைப்பு sockaddr_storage	client;

	    newchannel = add_channel();
	    எனில் (newchannel == வெற்று)
	    {
		ch_error(வெற்று, "Cannot allocate channel.");
		திரும்பு;
	    }
	    socklen = அளவு(client);
	    newfd = accept(fd, (கட்டமைப்பு sockaddr*)&client, &socklen);
	    எனில் (newfd < 0)
	    {
		ch_error(வெற்று, "Cannot accept channel.");
		channel_free(newchannel);
		திரும்பு;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    எனில் (channel->ch_ss_accept_cb != வெற்று)
	    {
		channel->ch_ss_accept_cb(newchannel);
		திரும்பு;
	    }
#endif

	    எனில் (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		எழுத்து addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((கட்டமைப்பு sockaddr_in*)&client)->sin_addr,
			addr, அளவு(addr));
		vim_snprintf((எழுத்து *)namebuf, அளவு(namebuf), "%s:%d",
			addr,
			ntohs(((கட்டமைப்பு sockaddr_in*)&client)->sin_port));
#இல்லையெனில்
		vim_snprintf((எழுத்து *)namebuf, அளவு(namebuf), "%s:%d",
		    inet_ntoa(((கட்டமைப்பு sockaddr_in*)&client)->sin_addr),
		    ntohs(((கட்டமைப்பு sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    இல்லையெனில் எனில் (client.ss_family == AF_INET6)
	    {
		எழுத்து addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((கட்டமைப்பு sockaddr_in6*)&client)->sin6_addr,
			addr, அளவு(addr));
		vim_snprintf((எழுத்து *)namebuf, அளவு(namebuf), "[%s]:%d",
			addr,
			ntohs(((கட்டமைப்பு sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    இல்லையெனில் எனில் (client.ss_family == AF_UNIX)
		vim_snprintf((எழுத்து *)namebuf, அளவு(namebuf),
							    "unix:anonymous");
	    இல்லையெனில்
		vim_snprintf((எழுத்து *)namebuf, அளவு(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    திரும்பு;
	}
	எனில் (use_socket)
	    len = sock_read(fd, (எழுத்து *)buf, MAXMSGSIZE);
	இல்லையெனில்
	    len = fd_read(fd, (எழுத்து *)buf, MAXMSGSIZE);
	எனில் (len <= 0)
	    நிறுத்து;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    எனில் (readlen <= 0)
    {
	எனில் (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#எனில் defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    இல்லையெனில் எனில் (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    வெறுமை
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
    நிலையான char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, முழு timeout, முழு raw, முழு *outlen)
{
    char_u	*buf;
    char_u	*செய்தி;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*முனை;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    போது (TRUE)
    {
	முனை = channel_peek(channel, part);
	எனில் (முனை != வெற்று)
	{
	    எனில் (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(முனை) != வெற்று))
		// got a complete message
		நிறுத்து;
	    எனில் (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		தொடர்;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    எனில் (raw || fd == INVALID_FD)
		நிறுத்து;
	}

	// Wait for up to the channel timeout.
	எனில் (fd == INVALID_FD)
	    திரும்பு வெற்று;
	எனில் (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    திரும்பு வெற்று;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    எனில் (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != வெற்று)
    {
	செய்தி = channel_get_all(channel, part, outlen);
    }
    இல்லையெனில்
    {
	char_u *p;

	buf = முனை->rq_buffer;
	nl = channel_first_nl(முனை);

	// Convert NUL to NL, the internal representation.
	க்கு (p = buf; (nl == வெற்று || p < nl) && p < buf + முனை->rq_buflen; ++p)
	    எனில் (*p == NUL)
		*p = NL;

	எனில் (nl == வெற்று)
	{
	    // must be a closed channel with missing NL
	    செய்தி = channel_get(channel, part, வெற்று);
	}
	இல்லையெனில் எனில் (nl + 1 == buf + முனை->rq_buflen)
	{
	    // get the whole buffer
	    செய்தி = channel_get(channel, part, வெற்று);
	    *nl = NUL;
	}
	இல்லையெனில்
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    செய்தி = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (முழு)(nl - buf) + 1);
	}
    }
    எனில் (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != வெற்று ? *outlen : (முழு)STRLEN(செய்தி));
    திரும்பு செய்தி;
}

நிலையான முழு channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    முழு
channel_in_blocking_wait(வெறுமை)
{
    திரும்பு channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    நிலையான வெறுமை
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, முழு timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (வெறுமை)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#இல்லையெனில்
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
    நிலையான முழு
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	முழு	    timeout_arg,
	முழு	    id,
	typval_T    **rettv)
{
    முழு		more;
    sock_T	fd;
    முழு		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    முழு		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    எனில் (id >= 0)
	channel_add_block_id(chanpart, id);

    க்கு (;;)
    {
	எனில் (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (வெறுமை)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	எனில் (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    நிறுத்து;
	}

	எனில் (!more)
	{
	    வெறுமை *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    வெறுமை *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    எனில் (channel_parse_messages())
		தொடர்;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    எனில் (readahead_ptr != வெற்று && readahead_ptr != prev_readahead_ptr)
		தொடர்;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (முழு)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    எனில் (timeout <= 0 || fd == INVALID_FD)
	    {
		எனில் (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		நிறுத்து;
	    }
	    // If there was an incomplete message use the deadline for that.
	    எனில் (chanpart->ch_wait_len > 0)
	    {
		முழு	இடது;
#ifdef MSWIN
		இடது = chanpart->ch_deadline - Get_Tick_மொத்தம்() + 1;
#இல்லையெனில்
		{
		    கட்டமைப்பு timeval now_tv;

		    gettimeofday(&now_tv, வெற்று);
		    இடது = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		எனில் (இடது < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		இல்லையெனில் எனில் (இடது < timeout)
		    timeout = இடது;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    எனில் (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    திரும்பு retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, முழு check_open, முழு reading, ch_part_T part)
{
    channel_T	*channel = வெற்று;
    முழு		has_readahead = FALSE;

    எனில் (tv->v_type == VAR_JOB)
    {
	எனில் (tv->vval.v_job != வெற்று)
	    channel = tv->vval.v_job->jv_channel;
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    இல்லையெனில்
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	திரும்பு வெற்று;
    }
    எனில் (channel != வெற்று && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_மொத்தம் ? part : channel_part_read(channel));

    எனில் (check_open && (channel == வெற்று || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	திரும்பு வெற்று;
    }
    திரும்பு channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    நிலையான வெறுமை
common_channel_read(typval_T *argvars, typval_T *rettv, முழு raw, முழு blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_மொத்தம்;
    jobopt_T	opt;
    முழு		mode;
    முழு		timeout;
    முழு		id = -1;
    typval_T	*listtv = வெற்று;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    clear_job_options(&opt);
    எனில் (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	செல் theend;

    எனில் (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    எனில் (channel == வெற்று)
	செல் theend;

    எனில் (part == PART_மொத்தம்)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    எனில் (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    எனில் (blob || mode == CH_MODE_BLOB)
    {
	முழு	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	எனில் (p != வெற்று)
	{
	    blob_T	*b = blob_alloc();

	    எனில் (b != வெற்று)
	    {
		b->bv_ga.ga_len = outlen;
		எனில் (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		இல்லையெனில்
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    இல்லையெனில் எனில் (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, வெற்று);
    இல்லையெனில்
    {
	எனில் (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	எனில் (listtv != வெற்று)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	இல்லையெனில்
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_எண் = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#எனில் defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    வெறுமை
channel_handle_events(முழு only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	எனில் (only_keep_open && !channel->ch_keep_open)
	    தொடர்;

	// check the socket and pipes
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    எனில் (fd == INVALID_FD)
		தொடர்;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    முழு r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    எனில் (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    இல்லையெனில் எனில் (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	எனில் (channel->ch_job != வெற்று)
	{
	    job_T *job = channel->ch_job;

	    எனில் (job->jv_tty_out != வெற்று && job->jv_status == JOB_FINISHED)
		க்கு (part = PART_SOCK; part < PART_மொத்தம்; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#எனில் defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    முழு
channel_any_keep_open(வெறுமை)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	எனில் (channel->ch_keep_open)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    வெறுமை
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    முழு		fd = ch_part->ch_fd;

    எனில் (fd == INVALID_FD)
	திரும்பு;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#இல்லையெனில்
    (வெறுமை)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    முழு
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	முழு	  len_arg,
	எழுத்து	  *fun)
{
    முழு		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    முழு		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    எனில் (fd == INVALID_FD)
    {
	எனில் (!channel->ch_error && fun != வெற்று)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	திரும்பு FAIL;
    }

    எனில் (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    எனில் (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_செய்தி = 0;
    }

    க்கு (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	முழு	    len;

	எனில் (wq->wq_next != வெற்று)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	இல்லையெனில்
	{
	    எனில் (len_arg == 0)
		// nothing to write, called from channel_select_check()
		திரும்பு OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	எனில் (part == PART_SOCK)
	    res = sock_write(fd, (எழுத்து *)buf, len);
	இல்லையெனில்
	{
	    res = fd_write(fd, (எழுத்து *)buf, len);
#ifdef MSWIN
	    எனில் (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, வெற்று);
	    }
#endif
	}
	எனில் (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	எனில் (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    எனில் (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    எனில் (res == len)
	    {
		// Wrote all the buf[len] bytes.
		எனில் (entry != வெற்று)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    தொடர்;
		}
		எனில் (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    இல்லையெனில்
	    {
		// Wrote only buf[res] bytes, can't write more now.
		எனில் (entry != வெற்று)
		{
		    எனில் (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (எழுத்து *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		இல்லையெனில்
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		எனில் (wq->wq_prev != வெற்று
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    எனில் (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((எழுத்து *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		இல்லையெனில்
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    எனில் (last != வெற்று)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = வெற்று;
			எனில் (wq->wq_prev == வெற்று)
			    wq->wq_next = last;
			இல்லையெனில்
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			எனில் (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	இல்லையெனில் எனில் (res != len)
	{
	    எனில் (!channel->ch_error && fun != வெற்று)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    திரும்பு FAIL;
	}

	channel->ch_error = FALSE;
	திரும்பு OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    நிலையான channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	முழு	    len,
	முழு	    id,
	முழு	    eval,
	jobopt_T    *opt,
	எழுத்து	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    எனில் (channel == வெற்று)
	திரும்பு வெற்று;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    எனில் (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	திரும்பு வெற்று;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    எனில் (opt->jo_callback.cb_name != வெற்று && *opt->jo_callback.cb_name != NUL)
    {
	எனில் (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    திரும்பு வெற்று;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    எனில் (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == வெற்று)
	திரும்பு channel;
    திரும்பு வெற்று;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    நிலையான வெறுமை
ch_கோவை_common(typval_T *argvars, typval_T *rettv, முழு eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    முழு		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    முழு		timeout;
    முழு		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    எனில் (channel == வெற்று)
	திரும்பு;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    எனில் (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	திரும்பு;
    }

    எனில் (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	எழுத்து		*சாவி = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	எனில் (rettv_dict_alloc(rettv) == FAIL)
	    திரும்பு;

	எனில் (check_for_dict_arg(argvars, 1) == FAIL)
	    திரும்பு;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)சாவி, -1);
	எனில் (argvars[2].v_type == VAR_DICT
		&& dict_has_சாவி(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	எனில் (di != வெற்று && di->di_tv.v_type != VAR_எண்
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_மதிப்பு_for_argument_str), சாவி);
	    திரும்பு;
	}

	எனில் (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_செய்தி_id;
	    எனில் (di == வெற்று)
		dict_add_எண்(d, "seq", id);
	    இல்லையெனில்
		di->di_tv.vval.v_எண் = id;
	}
	இல்லையெனில் எனில் (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_செய்தி_id;
	    எனில் (di == வெற்று)
		dict_add_எண்(d, "id", id);
	    இல்லையெனில்
		di->di_tv.vval.v_எண் = id;
	}
	இல்லையெனில்
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    எனில் (di != வெற்று && di->di_tv.v_type == VAR_எண்)
		id = di->di_tv.vval.v_எண்;
	}
	எனில் (ch_mode == CH_MODE_LSP && !dict_has_சாவி(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_செய்தி(&argvars[1]);
    }
    இல்லையெனில்
    {
	id = ++channel->ch_last_செய்தி_id;
	text = json_encode_nr_கோவை(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    எனில் (text == வெற்று)
	திரும்பு;

    channel = send_common(argvars, text, (முழு)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    எனில் (channel != வெற்று && eval)
    {
	எனில் (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	இல்லையெனில்
	    timeout = channel_get_timeout(channel, part_read);
	எனில் (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    எனில் (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_எண்;
		free_tv(listtv);
	    }
	    இல்லையெனில்
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_எண்;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    எனில் (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	எனில் (rettv->vval.v_dict != வெற்று)
	    dict_add_எண்(rettv->vval.v_dict, "seq", id);
    }
    இல்லையெனில் எனில் (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	எனில் (rettv->vval.v_dict != வெற்று)
	    dict_add_எண்(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    நிலையான வெறுமை
ch_raw_common(typval_T *argvars, typval_T *rettv, முழு eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    முழு		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    முழு		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	திரும்பு;

    எனில் (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    இல்லையெனில்
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (முழு)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    எனில் (channel != வெற்று && eval)
    {
	எனில் (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	இல்லையெனில்
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, வெற்று);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#எனில் defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    முழு
channel_poll_setup(முழு nfd_in, வெறுமை *fds_in, முழு *towait)
{
    முழு		nfd = nfd_in;
    channel_T	*channel;
    கட்டமைப்பு	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    எனில் (ch_part->ch_fd != INVALID_FD)
	    {
		எனில் (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    எனில் (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		இல்லையெனில்
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    இல்லையெனில்
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    திரும்பு nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    முழு
channel_poll_check(முழு ret_in, வெறுமை *fds_in)
{
    முழு		ret = ret_in;
    channel_T	*channel;
    கட்டமைப்பு	pollfd *fds = fds_in;
    ch_part_T	part;
    முழு		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    எனில் (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    இல்லையெனில் எனில் (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	எனில் (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_உள்ளீடு(channel);
	    --ret;
	}
    }

    திரும்பு ret;
}
#endif // UNIX && !HAVE_SELECT

#எனில் !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    முழு
channel_select_setup(
	முழு maxfd_in,
	வெறுமை *rfds_in,
	வெறுமை *wfds_in,
	கட்டமைப்பு timeval *tv,
	கட்டமைப்பு timeval **tvp)
{
    முழு		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    எனில் (fd != INVALID_FD)
	    {
		எனில் (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    எனில் (*tvp == வெற்று || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		இல்லையெனில்
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    எனில் ((முழு)fd >= FD_SETSIZE)
			தொடர்;
# endif
		    FD_SET((முழு)fd, rfds);
		    எனில் (maxfd < (முழு)fd)
			maxfd = (முழு)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    திரும்பு maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    முழு
channel_select_check(முழு ret_in, வெறுமை *rfds_in, வெறுமை *wfds_in)
{
    முழு		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	க்கு (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    எனில் (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (முழு)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    இல்லையெனில் எனில் (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	எனில் (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (முழு)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_உள்ளீடு(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	எனில் (channel->ch_job != வெற்று)
	{
	    job_T *job = channel->ch_job;

	    எனில் (job->jv_tty_out != வெற்று && job->jv_status == JOB_FINISHED)
		க்கு (part = PART_SOCK; part < PART_மொத்தம்; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    திரும்பு ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    நிலையான வெறுமை
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    எனில் (stdio_channel->CH_OUT_FD != INVALID_FD)
	(வெறுமை)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(முழு)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#அல்லதெனில் defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    கட்டமைப்பு timeval	tv;
    கட்டமைப்பு timeval	*tvp = &tv;
    முழு			maxfd;
    முழு			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, வெற்று, tvp);
    எனில் (ret >= 0)
	(வெறுமை)channel_select_check(ret, &rfds, &wfds);
#இல்லையெனில்
    கட்டமைப்பு pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    முழு			nfd;
    முழு			towait = (முழு)msec;
    முழு			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    எனில் (ret >= 0)
	(வெறுமை)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    வெறுமை
channel_stdio_loop(வெறுமை)
{
    போது (stdio_channel != வெற்று && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	எனில் (due_time > 0 && due_time < msec)
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
    முழு
channel_parse_messages(வெறுமை)
{
    channel_T	*channel = first_channel;
    முழு		ret = FALSE;
    முழு		r;
    ch_part_T	part = PART_SOCK;
    நிலையான முழு	recursive = 0;
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
    எனில் ((did_repeated_செய்தி & REPEATED_செய்தி_LOOKING) == 0)
    {
	ch_log(வெற்று, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_செய்தி = REPEATED_செய்தி_LOOKING;
    }
    போது (channel != வெற்று)
    {
	எனில் (recursive == 1)
	{
	    எனில் (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_மொத்தம்);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		தொடர்;
	    }
	    எனில் (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		எனில் (channel->ch_job != வெற்று)
		    channel->ch_job->jv_channel = வெற்று;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		தொடர்;
	    }
	    எனில் (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		தொடர்;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	எனில் (part == PART_OUT
		&& channel->ch_job != வெற்று
		&& channel->ch_job->jv_tty_out != வெற்று
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    தொடர்;
	}
#endif
	எனில் (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    எனில் (r == OK)
		ret = TRUE;
	    எனில் (channel_unref(channel) || (r == OK
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
		தொடர்;
	    }
	}
	எனில் (part < PART_ERR)
	    ++part;
	இல்லையெனில்
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    எனில் (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    திரும்பு ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    முழு
channel_any_readahead(வெறுமை)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    போது (channel != வெற்று)
    {
	எனில் (channel_has_readahead(channel, part))
	    திரும்பு TRUE;
	எனில் (part < PART_ERR)
	    ++part;
	இல்லையெனில்
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    திரும்பு FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    முழு
set_ref_in_channel(முழு copyID)
{
    முழு		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    க்கு (channel = first_channel; !abort && channel != வெற்று;
						   channel = channel->ch_next)
	எனில் (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, வெற்று, வெற்று, வெற்று);
	}
    திரும்பு abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    நிலையான ch_part_T
channel_part_send(channel_T *channel)
{
    எனில் (channel->CH_SOCK_FD == INVALID_FD)
	திரும்பு PART_IN;
    திரும்பு PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    நிலையான ch_part_T
channel_part_read(channel_T *channel)
{
    எனில் (channel->CH_SOCK_FD == INVALID_FD)
	திரும்பு PART_OUT;
    திரும்பு PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    நிலையான ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    எனில் (channel == வெற்று)
	திரும்பு CH_MODE_JSON;
    திரும்பு channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    நிலையான முழு
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    திரும்பு channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    வெறுமை
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_எண் = 0;
    எனில் (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    எனில் (channel != வெற்று)
	rettv->vval.v_எண் = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    வெறுமை
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    எனில் (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    எனில் (channel != வெற்று)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    வெறுமை
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    எனில் (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    எனில் (channel != வெற்று)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    வெறுமை
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_எண் = -1;

    எனில் (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	திரும்பு;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    எனில் (channel == வெற்று)
	திரும்பு;

    char_u	*what = tv_get_string(&argvars[1]);
    முழு		part;
    எனில் (STRCMP(what, "err") == 0)
	part = PART_ERR;
    இல்லையெனில் எனில் (STRCMP(what, "out") == 0)
	part = PART_OUT;
    இல்லையெனில் எனில் (STRCMP(what, "in") == 0)
	part = PART_IN;
    இல்லையெனில்
	part = PART_SOCK;
    எனில் (channel->ch_part[part].ch_bufref.br_buf != வெற்று)
	rettv->vval.v_எண் =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    வெறுமை
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    எனில் (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    எனில் (channel == வெற்று)
	திரும்பு;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    எனில் (channel->ch_job != வெற்று)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    வெறுமை
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    எனில் (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	திரும்பு;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    எனில் (channel != வெற்று && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    வெறுமை
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    எனில் (check_restricted() || check_secure())
	திரும்பு;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    வெறுமை
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    எனில் (check_restricted() || check_secure())
	திரும்பு;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    வெறுமை
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    வெறுமை
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    வெறுமை
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    வெறுமை
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_கோவை_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    வெறுமை
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_கோவை_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    வெறுமை
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    வெறுமை
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    வெறுமை
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    எனில் (check_restricted() || check_secure())
	திரும்பு;

    எனில் (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    எனில் (channel == வெற்று)
	திரும்பு;
    clear_job_options(&opt);
    எனில் (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    வெறுமை
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    முழு		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = வெற்று;

    எனில் (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	திரும்பு;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    எனில் (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	எனில் (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    எழுத்து      *status = channel_status(channel, -1);

    எனில் (channel == வெற்று)
	vim_snprintf((எழுத்து *)buf, NUMBUFLEN, "channel %s", status);
    இல்லையெனில்
	vim_snprintf((எழுத்து *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    திரும்பு buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(முழு ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	எனில் (ch->ch_id == ch_id)
	    திரும்பு ch;
    திரும்பு வெற்று;
}

#endif // FEAT_JOB_CHANNEL
