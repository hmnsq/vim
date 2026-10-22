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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_JOB_CHANNEL)

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
પ્રકારવ્યાખ્યા સંરચના sockaddr_un {
    ADDRESS_FAMILY sun_family;
    અક્ષર sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#નહીંતર
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) લિખ(sd, buf, len)
# define sock_read(sd, buf, len) પઠ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) પઠ(fd, buf, len)
# define fd_write(sd, buf, len) લિખ(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

સ્થિર શૂન્ય channel_read(channel_T *channel, ch_part_T part, અક્ષર *func);
સ્થિર શૂન્ય channel_wait_any(long msec);
સ્થિર ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
સ્થિર પૂર્ણાંક channel_get_timeout(channel_T *channel, ch_part_T part);
સ્થિર channel_T *channel_open_stdio(શૂન્ય);
સ્થિર ch_part_T channel_part_send(channel_T *channel);
સ્થિર ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    માટે ((ch) = first_channel; (ch) != ખાલી; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
સ્થિર પૂર્ણાંક safe_to_invoke_callback = 0;

#ifdef MSWIN
    સ્થિર પૂર્ણાંક
fd_read(sock_T fd, અક્ષર *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    જો (!ReadFile(h, buf, (DWORD)len, &nread, ખાલી))
	પાછા -1;
    પાછા (પૂર્ણાંક)nread;
}

    સ્થિર પૂર્ણાંક
fd_write(sock_T fd, અક્ષર *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    જ્યારે (todo > 0)
    {
	જો (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	નહીંતર
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	જો (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    જો (err != ERROR_IO_PENDING)
		પાછા -1;
	    જો (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		પાછા -1;
	    FlushFileBuffers(h);
	}
	નહીંતર જો (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    તોડો;
	todo -= nwrite;
	done += nwrite;
    }
    પાછા (પૂર્ણાંક)done;
}

    સ્થિર શૂન્ય
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(સંદેશ) (શૂન્ય)semsg("%s: %s", સંદેશ, strerror_win32(errno))

    સ્થિર અક્ષર *
strerror_win32(પૂર્ણાંક eno)
{
    સ્થિર LPVOID msgbuf = ખાલી;
    char_u *ptr;

    જો (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ખાલી;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	ખાલી,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ખાલી);
    જો (msgbuf != ખાલી)
	// chomp \r or \n
	માટે (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    પસંદગી (*ptr)
	    {
		સ્થિતિ '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    તોડો;
		સ્થિતિ '\n':
		    જો (*(ptr + 1) == '\0')
			*ptr = '\0';
		    નહીંતર
			*ptr = ' ';
		    તોડો;
	    }
    પાછા msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
સ્થિર channel_T *first_channel = ખાલી;
સ્થિર પૂર્ણાંક next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(શૂન્ય)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    જો (channel == ખાલી)
	પાછા ખાલી;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    માટે (part = PART_SOCK; part < PART_ગણતરી; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_આગત_Handler = (Xt_આગત_Id)ખાલી;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_આગત_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    જો (first_channel != ખાલી)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    પાછા channel;
}

    પૂર્ણાંક
has_any_channel(શૂન્ય)
{
    પાછા first_channel != ખાલી;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    પૂર્ણાંક
channel_still_useful(channel_T *channel)
{
    પૂર્ણાંક has_sock_સંદેશ;
    પૂર્ણાંક	has_out_સંદેશ;
    પૂર્ણાંક	has_err_સંદેશ;

    // If the job was killed the channel is not expected to work anymore.
    જો (channel->ch_job_killed && channel->ch_job == ખાલી)
	પાછા FALSE;

    // If there is a close callback it may still need to be invoked.
    જો (channel->ch_close_cb.cb_name != ખાલી)
	પાછા TRUE;

    // If reading from or a buffer it's still useful.
    જો (channel->ch_part[PART_IN].ch_bufref.br_buf != ખાલી)
	પાછા TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_સંદેશ = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != ખાલી
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != ખાલી;
    has_out_સંદેશ = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != ખાલી
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != ખાલી;
    has_err_સંદેશ = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != ખાલી
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != ખાલી;
    પાછા (channel->ch_callback.cb_name != ખાલી && (has_sock_સંદેશ
		|| has_out_સંદેશ || has_err_સંદેશ))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != ખાલી
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != ખાલી)
		    && has_out_સંદેશ)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ખાલી
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ખાલી)
		    && has_err_સંદેશ);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    પૂર્ણાંક
channel_can_close(channel_T *channel)
{
    પાછા channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    સ્થિર શૂન્ય
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    સ્થિર શૂન્ય
channel_free_channel(channel_T *channel)
{
    જો (channel->ch_next != ખાલી)
	channel->ch_next->ch_prev = channel->ch_prev;
    જો (channel->ch_prev == ખાલી)
	first_channel = channel->ch_next;
    નહીંતર
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    સ્થિર શૂન્ય
channel_free(channel_T *channel)
{
    જો (in_free_unref_વસ્તુઓ)
	પાછા;

    જો (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    નહીંતર
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
    સ્થિર પૂર્ણાંક
channel_may_free(channel_T *channel)
{
    જો (!channel_still_useful(channel))
    {
	channel_free(channel);
	પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    પૂર્ણાંક
channel_unref(channel_T *channel)
{
    જો (channel != ખાલી && --channel->ch_refcount <= 0)
	પાછા channel_may_free(channel);
    પાછા FALSE;
}

    પૂર્ણાંક
free_unused_channels_contents(પૂર્ણાંક copyID, પૂર્ણાંક mask)
{
    પૂર્ણાંક		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	જો (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    પાછા did_free;
}

    શૂન્ય
free_unused_channels(પૂર્ણાંક copyID, પૂર્ણાંક mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    માટે (ch = first_channel; ch != ખાલી; ch = ch_next)
    {
	ch_next = ch->ch_next;
	જો (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#જો defined(FEAT_GUI)

# જો defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    સ્થિર channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    જો (fd == INVALID_FD)
	પાછા ખાલી;

    FOR_ALL_CHANNELS(channel)
    {
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	    જો (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		પાછા channel;
	    }
    }
    પાછા ખાલી;
}

    સ્થિર શૂન્ય
channel_read_fd(પૂર્ણાંક fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    જો (channel == ખાલી)
	ch_error(ખાલી, "Channel for fd %d not found", fd);
    નહીંતર
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    સ્થિર શૂન્ય
messageFromServerX11(XtPointer clientData,
		  પૂર્ણાંક *unused1 UNUSED,
		  Xt_આગત_Id *unused2 UNUSED)
{
    channel_read_fd((પૂર્ણાંક)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    પાછા TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  નહીંતર
    સ્થિર શૂન્ય
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_આગત_Condition unused2 UNUSED)
{
    channel_read_fd((પૂર્ણાંક)(long)clientData);
}
#  endif
# endif

    સ્થિર શૂન્ય
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    જો (!CH_HAS_GUI)
	પાછા;

    // gets stuck in handling events for a not connected channel
    જો (channel->ch_keep_open)
	પાછા;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    જો (channel->ch_part[part].ch_આગત_Handler == (Xt_આગત_Id)ખાલી)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_આગત_Handler = Xt_App_Add_આગત(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_આગત_Read_Mask + Xt_આગત_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# નહીંતર
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    જો (channel->ch_part[part].ch_આગત_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   જો GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_આગત_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   નહીંતર
	channel->ch_part[part].ch_આગત_Handler = gdk_આગત_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_આગત_Condition)
			     ((પૂર્ણાંક)GDK_આગત_READ + (પૂર્ણાંક)GDK_આગત_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    સ્થિર શૂન્ય
channel_gui_register(channel_T *channel)
{
    જો (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    જો (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    જો (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    શૂન્ય
channel_gui_register_all(શૂન્ય)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    સ્થિર શૂન્ય
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    જો (channel->ch_part[part].ch_આગત_Handler != (Xt_આગત_Id)ખાલી)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_આગત(channel->ch_part[part].ch_આગત_Handler);
	channel->ch_part[part].ch_આગત_Handler = (Xt_આગત_Id)ખાલી;
    }
# નહીંતર
#  ifdef FEAT_GUI_GTK
    જો (channel->ch_part[part].ch_આગત_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   જો GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_આગત_Handler);
#   નહીંતર
	gdk_આગત_remove(channel->ch_part[part].ch_આગત_Handler);
#   endif
	channel->ch_part[part].ch_આગત_Handler = 0;
    }
#  endif
# endif
}

    સ્થિર શૂન્ય
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    માટે (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    સ્થિર પૂર્ણાંક
channel_connect(
	channel_T *channel,
	અચલ સંરચના sockaddr *server_addr,
	પૂર્ણાંક server_addrlen,
	પૂર્ણાંક *waittime)
{
    પૂર્ણાંક		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    જ્યારે (TRUE)
    {
	long	elapsed_msec = 0;
	પૂર્ણાંક	waitnow;
	પૂર્ણાંક	ret;

	જો (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	જો (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    પાછા -1;
	}

	જો (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    જો (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#નહીંતર
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		પાછા -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	જો (ret == 0)
	    // The connection could be established.
	    તોડો;

	SOCK_ERRNO;
	જો (*waittime < 0 || (errno != EWOULDBLOCK
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
	    પાછા -1;
	}
	નહીંતર જો (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    પાછા -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    સંરચના timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    પૂર્ણાંક			so_error = 0;
	    socklen_t		so_error_len = કદ(so_error);
	    સંરચના timeval	start_tv;
	    સંરચના timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, ખાલી);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ખાલી, &tv);
	    જો (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		પાછા -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    જો (FD_ISSET(sd, &wfds))
		તોડો;
	    elapsed_msec = waitnow;
	    જો (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		ચાલુ;
	    }
#નહીંતર
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    જો (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		જો (ret < 0 || (so_error != 0
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
		    પાછા -1;
		}
		નહીંતર જો (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    પાછા -1;
		}
	    }

	    જો (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		તોડો;

	    gettimeofday(&end_tv, ખાલી);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	જો (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    જો (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    જો (!got_int)
	    {
		જો (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		ચાલુ;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	પાછા -1;
    }

    જો (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#નહીંતર
	(શૂન્ય)fcntl(sd, F_SETFL, 0);
#endif
    }

    પાછા sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	અચલ અક્ષર *path,
	શૂન્ય (*nb_close_cb)(શૂન્ય))
{
    channel_T		*channel = ખાલી;
    પૂર્ણાંક			sd = -1;
    size_t		path_len = STRLEN(path);
    સંરચના sockaddr_un	server;
    size_t		server_len;
    પૂર્ણાંક			waittime = -1;

    જો (*path == NUL || path_len >= કદ(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	પાછા ખાલી;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    જો (channel == ખાલી)
    {
	ch_error(ખાલી, "Cannot allocate channel.");
	પાછા ખાલી;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, કદ(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(સંરચના sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (સંરચના sockaddr *)&server, (પૂર્ણાંક)server_len,
								   &waittime);

    જો (sd < 0)
    {
	channel_free(channel);
	પાછા ખાલી;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (અક્ષર *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    પાછા channel;
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
	અચલ અક્ષર *hostname,
	પૂર્ણાંક port,
	પૂર્ણાંક waittime,
	શૂન્ય (*nb_close_cb)(શૂન્ય))
{
    પૂર્ણાંક			sd = -1;
    channel_T		*channel = ખાલી;
#ifdef FEAT_IPV6
    પૂર્ણાંક			err;
    સંરચના addrinfo	hints;
    સંરચના addrinfo	*res = ખાલી;
    સંરચના addrinfo	*addr = ખાલી;
#નહીંતર
    સંરચના sockaddr_in	server;
    સંરચના hostent	*host = ખાલી;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    જો (channel == ખાલી)
    {
	ch_error(ખાલી, "Cannot allocate channel.");
	પાછા ખાલી;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# જો defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    જો ((err = getaddrinfo(hostname, ખાલી, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	પાછા ખાલી;
    }

    // Count the number of addresses for timeout distribution
    પૂર્ણાંક addr_ગણતરી = 0;
    માટે (addr = res; addr != ખાલી; addr = addr->ai_next)
	addr_ગણતરી++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    જો (waittime == 0)
	waittime = 15;

    પૂર્ણાંક addr_index = 0;
    માટે (addr = res; addr != ખાલી; addr = addr->ai_next, addr_index++)
    {
	અચલ અક્ષર  *dst = hostname;
# ifdef HAVE_INET_NTOP
	અચલ શૂન્ય  *src = ખાલી;
	અક્ષર	    buf[NUMBUFLEN];
# endif
	પૂર્ણાંક	    try_waittime;
	પૂર્ણાંક	    before_waittime;
	પૂર્ણાંક	    consumed;
	પૂર્ણાંક	    remaining_addrs;

	જો (addr->ai_family == AF_INET6)
	{
	    સંરચના sockaddr_in6 *sai = (સંરચના sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	નહીંતર જો (addr->ai_family == AF_INET)
	{
	    સંરચના sockaddr_in *sai = (સંરચના sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	જો (src != ખાલી)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, કદ(buf));
	    જો (dst == ખાલી)
		dst = hostname;
	    નહીંતર જો (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	જો (addr->ai_next == ખાલી)
	    try_waittime = waittime;
	નહીંતર જો (addr_index == 0)
	{
	    જો (waittime > 500)
		try_waittime = 250;
	    નહીંતર જો (waittime > 30)
		try_waittime = waittime / 2;
	    નહીંતર
		try_waittime = waittime;
	}
	નહીંતર
	{
	    remaining_addrs = addr_ગણતરી - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (પૂર્ણાંક)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	જો (waittime < 0)
	    waittime = 0;

	જો (sd >= 0)
	    તોડો;

	// If we have no time left, stop trying
	જો (waittime <= 0 && addr->ai_next != ખાલી)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    તોડો;
	}
    }

    freeaddrinfo(res);
#નહીંતર
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    જો ((host = gethostbyname(hostname)) == ખાલી)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	પાછા ખાલી;
    }
    {
	અક્ષર *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], કદ(p));
	memcpy((અક્ષર *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    જો (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (સંરચના sockaddr *)&server, કદ(server),
								   &waittime);
#endif

    જો (sd < 0)
    {
	channel_free(channel);
	પાછા ખાલી;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (અક્ષર *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    પાછા channel;
}

    સ્થિર શૂન્ય
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    જો (callback->cb_name != ખાલી && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    નહીંતર
	cbp->cb_name = ખાલી;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	સ્થિર શૂન્ય
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_કિંમત_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_કિંમત_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    જો (curbuf->b_ml.ml_mfp == ખાલી)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    સ્થિર buf_T *
channel_find_buffer(char_u *name, પૂર્ણાંક err, પૂર્ણાંક સંદેશ)
{
    buf_T *buf = ખાલી;
    buf_T *save_curbuf = curbuf;

    જો (name != ખાલી && *name != NUL)
    {
	buf = buflist_findname(name);
	જો (buf == ખાલી)
	    buf = buflist_findname_exp(name);
    }

    જો (buf != ખાલી)
	પાછા buf;

    buf = buflist_new(name == ખાલી || *name == NUL ? ખાલી : name,
	    ખાલી, (linenr_T)0, BLN_LISTED | BLN_NEW);
    જો (buf == ખાલી)
	પાછા ખાલી;
    prepare_buffer(buf);

    curbuf = buf;
    જો (સંદેશ)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    પાછા buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    સ્થિર શૂન્ય
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    જો (opt->jo_set & JO_MODE)
	માટે (part = PART_SOCK; part < PART_ગણતરી; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    જો (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    જો (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    જો (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    જો (opt->jo_set & JO_TIMEOUT)
	માટે (part = PART_SOCK; part < PART_ગણતરી; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    જો (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    જો (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    જો (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    જો (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    જો (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    જો (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    જો (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    જો ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	જો (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	જો (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    જો (buf == ખાલી)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	નહીંતર
	{
	    પૂર્ણાંક સંદેશ = TRUE;

	    જો (opt->jo_set2 & JO2_OUT_સંદેશ)
		સંદેશ = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, સંદેશ);
	}
	જો (buf != ખાલી)
	{
	    જો (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    જો (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    નહીંતર
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (અક્ષર *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		જો (buf->b_ml.ml_mfp == ખાલી)
		    prepare_buffer(buf);
	    }
	}
    }

    જો ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	જો (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	જો (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	નહીંતર જો (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    જો (buf == ખાલી)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	નહીંતર
	{
	    પૂર્ણાંક સંદેશ = TRUE;

	    જો (opt->jo_set2 & JO2_ERR_સંદેશ)
		સંદેશ = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, સંદેશ);
	}
	જો (buf != ખાલી)
	{
	    જો (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    જો (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    નહીંતર
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (અક્ષર *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		જો (buf->b_ml.ml_mfp == ખાલી)
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
    પૂર્ણાંક
channel_parse_socketserver_address(
	char_u	*address,
	પૂર્ણાંક	*port,
	char_u	**unix_path,
	bool	quiet)
{
    અક્ષર    *rest;
    long    val;

    જો (*address == NUL)
	જાઓ fail;

    જો (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	પાછા OK;
    }

    val = strtol((અક્ષર *)(address), &rest, 10);
    જો (val < 0 || val >= 65536 || *rest != NUL)
    {
	જો (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	પાછા FAIL;
    }
    *port = (પૂર્ણાંક)val;

    પાછા OK;
fail:
    જો (!quiet)
	semsg(_(e_invalid_argument_str), address);
    પાછા FAIL;
}

/*
 * Implements ch_open().
 */
    સ્થિર channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    અક્ષર	*rest;
    પૂર્ણાંક		port = 0;
    પૂર્ણાંક		is_ipv6 = FALSE;
    પૂર્ણાંક		is_unix = FALSE;
    પૂર્ણાંક		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ખાલી;

    જો (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા ખાલી;

    address = tv_get_string(&argvars[0]);
    જો (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	પાછા ખાલી;

    જો (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	પાછા ખાલી;
    }

    જો (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    નહીંતર જો (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    નહીંતર જો (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	જો (p == ખાલી || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    પાછા ખાલી;
	}
    }
    નહીંતર
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	જો (p == ખાલી)
	{
	    semsg(_(e_invalid_argument_str), address);
	    પાછા ખાલી;
	}
    }

    જો (!is_unix && !is_stdio)
    {
	port = strtol((અક્ષર *)(p + 1), &rest, 10);
	જો (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    પાછા ખાલી;
	}
	જો (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	નહીંતર
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    જો (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	જાઓ theend;
    જો (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	જાઓ theend;
    }

    જો (is_stdio)
	channel = channel_open_stdio();
    નહીંતર જો (is_unix)
	channel = channel_open_unix((અક્ષર *)address, ખાલી);
    નહીંતર
	channel = channel_open((અક્ષર *)address, port, opt.jo_waittime, ખાલી);
    જો (channel != ખાલી)
    {
	પૂર્ણાંક	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	જો (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    પાછા channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
સ્થિર channel_T *stdio_channel = ખાલી;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    સ્થિર channel_T *
channel_open_stdio(શૂન્ય)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    જો (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	પાછા ખાલી;
    }
    જો (stdio_channel != ખાલી)
    {
	emsg(_(e_cannot_open_stdio_channel));
	પાછા ખાલી;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	પૂર્ણાંક	nul_fd;

	જો (!DuplicateHandle(proc, GetStdHandle(STD_આગત_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    પાછા ખાલી;
	}
	જો (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    પાછા ખાલી;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ખાલી, OPEN_EXISTING, 0,
		ખાલી);
	જો (hnul != INVALID_HANDLE_કિંમત)
	{
	    SetStdHandle(STD_આગત_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	જો (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#નહીંતર
    {
	પૂર્ણાંક	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	જો (in_fd < 0 || out_fd < 0)
	{
	    જો (in_fd >= 0)
		close(in_fd);
	    જો (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    પાછા ખાલી;
	}
	(શૂન્ય)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(શૂન્ય)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	જો (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    જો (channel == ખાલી)
    {
	ch_error(ખાલી, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	પાછા ખાલી;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    પાછા channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    અક્ષર	*rest;
    પૂર્ણાંક		port;
    પૂર્ણાંક		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ખાલી;

    જો (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	પાછા ખાલી;

    arg = tv_get_string(&argvars[0]);
    જો (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	પાછા ખાલી;
    }

    જો (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    નહીંતર
    {
	port = strtol((અક્ષર *)(arg), &rest, 10);
	જો (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    પાછા ખાલી;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    જો (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	જાઓ theend;
    જો (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	જાઓ theend;
    }

    જો (is_unix)
	channel = channel_listen_unix((અક્ષર *)arg, ખાલી, true);
    નહીંતર
	channel = channel_listen(port, ખાલી);
    જો (channel != ખાલી)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    પાછા channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	પૂર્ણાંક port_in,
	શૂન્ય (*nb_close_cb)(શૂન્ય))
{
    પૂર્ણાંક			sd = -1;
    સંરચના sockaddr_in	server;
#ifndef FEAT_IPV6
    સંરચના hostent	*host;
#endif
    પૂર્ણાંક			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    જો (channel == ખાલી)
    {
	ch_error(ખાલી, "Cannot allocate channel.");
	પાછા ખાલી;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((અક્ષર *)&server, 0, કદ(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    જો (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	પાછા ખાલી;
    }

#ifdef MSWIN
    જો (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (અચલ અક્ષર *)&val, કદ(val)) < 0)
#નહીંતર
    જો (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, કદ(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	પાછા ખાલી;
    }

    // Bind the socket to the port
    જો (bind(sd, (સંરચના sockaddr *)&server, કદ(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	પાછા ખાલી;
    }

    જો (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	પાછા ખાલી;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    જો (port_in == 0)
    {
	સંરચના sockaddr_in	addr;
	socklen_t		addr_len = કદ(addr);

	જો (getsockname(sd, (સંરચના sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (અક્ષર *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    પાછા channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	અક્ષર *path,
	શૂન્ય (*nb_close_cb)(શૂન્ય),
	bool replace)
{
    પૂર્ણાંક			sd = -1;
    સંરચના sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    જો (path == ખાલી || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == ખાલી ? (અક્ષર *)"" : path);
	પાછા ખાલી;
    }

    path_len = STRLEN(path);
    જો (path_len >= કદ(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	પાછા ખાલી;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    જો (channel == ખાલી)
    {
	ch_error(ખાલી, "Cannot allocate channel.");
	પાછા ખાલી;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, કદ(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    જો (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	પાછા ખાલી;
    }

    જો (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(સંરચના sockaddr_un, sun_path) + path_len + 1;
    જો (bind(sd, (સંરચના sockaddr *)&server, (પૂર્ણાંક)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	પાછા ખાલી;
    }

    જો (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	પાછા ખાલી;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (અક્ષર *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    પાછા channel;
}

     શૂન્ય
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    જો (*fd == INVALID_FD)
	પાછા;

    જો (part == PART_SOCK)
	sock_close(*fd);
    નહીંતર
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	જો ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    જો (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    શૂન્ય
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    જો (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#જો defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	જો (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    જો (out != INVALID_FD)
    {
#જો defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#જો defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    જો (err != INVALID_FD)
    {
#જો defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#જો defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    શૂન્ય
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    જો (job->jv_in_buf == ખાલી)
	પાછા;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (અક્ષર *)in_part->ch_bufref.br_buf->b_ffname);
    જો (options->jo_set & JO_IN_ટોચ)
    {
	જો (options->jo_in_ટોચ == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_ટોચ =
		in_part->ch_bufref.br_buf->b_ml.ml_લીટી_ગણતરી + 1;
	}
	નહીંતર
	    in_part->ch_buf_ટોચ = options->jo_in_ટોચ;
    }
    નહીંતર
	in_part->ch_buf_ટોચ = 1;
    જો (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    નહીંતર
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_લીટી_ગણતરી;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    સ્થિર શૂન્ય
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	પૂર્ણાંક	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    જો (item == ખાલી)
	પાછા;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = ખાલી;
    જો (item->cq_prev == ખાલી)
	head->cq_next = item;
    નહીંતર
	item->cq_prev->cq_next = item;
}

    સ્થિર શૂન્ય
write_buf_લીટી(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *લીટી = ml_get_buf(buf, lnum, FALSE);
    પૂર્ણાંક	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    પૂર્ણાંક	    i;

    // Need to make a copy to be able to append a NL.
    જો ((p = alloc(len + 2)) == ખાલી)
	પાછા;
    memcpy((અક્ષર *)p, (અક્ષર *)લીટી, len);

    જો (channel->ch_write_text_mode)
	p[len] = CAR;
    નહીંતર
    {
	માટે (i = 0; i < len; ++i)
	    જો (p[i] == NL)
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
    સ્થિર પૂર્ણાંક
can_write_buf_લીટી(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    જો (in_part->ch_fd == INVALID_FD)
	પાછા FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    જો (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    નહીંતર જો (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# જો defined(HAVE_SELECT)
	સંરચના timeval	tval;
	fd_set		wfds;
	પૂર્ણાંક		ret;

	FD_ZERO(&wfds);
	FD_SET((પૂર્ણાંક)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	માટે (;;)
	{
	    ret = select((પૂર્ણાંક)in_part->ch_fd + 1, ખાલી, &wfds, ખાલી, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    જો (ret == -1 && errno == EINTR)
		ચાલુ;
#  endif
	    જો (ret <= 0 || in_part->ch_block_write == 1)
	    {
		જો (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		નહીંતર
		    ch_log(channel, "Input not ready for writing");
		પાછા FALSE;
	    }
	    તોડો;
	}
# નહીંતર
	સંરચના pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	જો (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    પાછા FALSE;
	}
	જો (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    પાછા FALSE;
	}
# endif
    }
#endif
    પાછા TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    શૂન્ય
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    પૂર્ણાંક		written = 0;

    જો (buf == ખાલી || in_part->ch_buf_append)
	પાછા;  // no buffer or using appending
    જો (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == ખાલી)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = ખાલી;
	પાછા;
    }

    માટે (lnum = in_part->ch_buf_ટોચ; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_લીટી_ગણતરી; ++lnum)
    {
	જો (!can_write_buf_લીટી(channel))
	    તોડો;
	write_buf_લીટી(buf, lnum, channel);
	++written;
    }

    જો (written == 1)
	ch_log(channel, "written line %d to channel", (પૂર્ણાંક)lnum - 1);
    નહીંતર જો (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_ટોચ = lnum;
    જો (lnum > buf->b_ml.ml_લીટી_ગણતરી || lnum > in_part->ch_buf_bot)
    {
#જો defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	જો (channel->ch_job != ખાલી)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = ખાલી;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	જો (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    નહીંતર
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_લીટી_ગણતરી - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    શૂન્ય
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	માટે (part = PART_SOCK; part < PART_ગણતરી; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    જો (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = ખાલી;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    શૂન્ય
channel_write_આગત(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    જો (in_part->ch_writeque.wq_next != ખાલી)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    નહીંતર જો (in_part->ch_bufref.br_buf != ખાલી)
    {
	જો (in_part->ch_buf_append)
	    channel_write_new_લીટીઓ(in_part->ch_bufref.br_buf);
	નહીંતર
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    શૂન્ય
channel_write_any_લીટીઓ(શૂન્ય)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_આગત(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    શૂન્ય
channel_write_new_લીટીઓ(buf_T *buf)
{
    channel_T	*channel;
    પૂર્ણાંક		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	પૂર્ણાંક	    written = 0;

	જો (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    જો (in_part->ch_fd == INVALID_FD)
		ચાલુ;  // pipe was closed
	    found_one = TRUE;
	    માટે (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_લીટી_ગણતરી;
								       ++lnum)
	    {
		જો (!can_write_buf_લીટી(channel))
		    તોડો;
		write_buf_લીટી(buf, lnum, channel);
		++written;
	    }

	    જો (written == 1)
		ch_log(channel, "written line %d to channel", (પૂર્ણાંક)lnum - 1);
	    નહીંતર જો (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    જો (lnum < buf->b_ml.ml_લીટી_ગણતરી)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_લીટી_ગણતરી - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    જો (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    સ્થિર શૂન્ય
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    જો (safe_to_invoke_callback == 0)
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

    પાછા head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *ગાંઠ)
{
    char_u  *buffer = ગાંઠ->rq_buffer;
    long_u  i;

    માટે (i = 0; i < ગાંઠ->rq_buflen; ++i)
	જો (buffer[i] == NL)
	    પાછા buffer + i;
    પાછા ખાલી;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, પૂર્ણાંક *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ગાંઠ = head->rq_next;
    char_u *p;

    જો (ગાંઠ == ખાલી)
	પાછા ખાલી;
    જો (outlen != ખાલી)
	*outlen += ગાંઠ->rq_buflen;
    // dispose of the node but keep the buffer
    p = ગાંઠ->rq_buffer;
    head->rq_next = ગાંઠ->rq_next;
    જો (ગાંઠ->rq_next == ખાલી)
	head->rq_prev = ખાલી;
    નહીંતર
	ગાંઠ->rq_next->rq_prev = ખાલી;
    vim_free(ગાંઠ);
    પાછા p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    સ્થિર char_u *
channel_get_all(channel_T *channel, ch_part_T part, પૂર્ણાંક *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ગાંઠ;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    માટે (ગાંઠ = head->rq_next; ગાંઠ != ખાલી; ગાંઠ = ગાંઠ->rq_next)
	len += ગાંઠ->rq_buflen;
    res = alloc(len + 1);
    જો (res == ખાલી)
	પાછા ખાલી;
    p = res;
    માટે (ગાંઠ = head->rq_next; ગાંઠ != ખાલી; ગાંઠ = ગાંઠ->rq_next)
    {
	mch_memmove(p, ગાંઠ->rq_buffer, ગાંઠ->rq_buflen);
	p += ગાંઠ->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    કરો
    {
	p = channel_get(channel, part, ખાલી);
	vim_free(p);
    } જ્યારે (p != ખાલી);

    જો (outlen != ખાલી)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	પાછા res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    જ્યારે (p < res + len)
    {
	જો (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	નહીંતર જો (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    જો (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		જ્યારે (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		જો (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    પાછા res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    શૂન્ય
channel_consume(channel_T *channel, ch_part_T part, પૂર્ણાંક len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ગાંઠ = head->rq_next;
    char_u *buf = ગાંઠ->rq_buffer;

    જો (len < 0 || (long_u)len > ગાંઠ->rq_buflen)
	પાછા;
    mch_memmove(buf, buf + len, ગાંઠ->rq_buflen - len);
    ગાંઠ->rq_buflen -= len;
    ગાંઠ->rq_buffer[ગાંઠ->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    પૂર્ણાંક
channel_collapse(channel_T *channel, ch_part_T part, પૂર્ણાંક want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*ગાંઠ = head->rq_next;
    readq_T	*last_ગાંઠ;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    જો (ગાંઠ == ખાલી || ગાંઠ->rq_next == ખાલી)
	પાછા FAIL;

    last_ગાંઠ = ગાંઠ->rq_next;
    len = ગાંઠ->rq_buflen + last_ગાંઠ->rq_buflen;
    જો (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	જ્યારે (last_ગાંઠ->rq_next != ખાલી
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_ગાંઠ) == ખાલી))
	{
	    last_ગાંઠ = last_ગાંઠ->rq_next;
	    len += last_ગાંઠ->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    જો (newbuf == ખાલી)
	પાછા FAIL;	    // out of memory
    mch_memmove(p, ગાંઠ->rq_buffer, ગાંઠ->rq_buflen);
    p += ગાંઠ->rq_buflen;
    vim_free(ગાંઠ->rq_buffer);
    ગાંઠ->rq_buffer = newbuf;
    માટે (n = ગાંઠ; n != last_ગાંઠ; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    ગાંઠ->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    માટે (n = ગાંઠ->rq_next; n != last_ગાંઠ; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    ગાંઠ->rq_next = last_ગાંઠ->rq_next;
    જો (last_ગાંઠ->rq_next == ખાલી)
	head->rq_prev = ગાંઠ;
    નહીંતર
	last_ગાંઠ->rq_next->rq_prev = ગાંઠ;
    vim_free(last_ગાંઠ);
    પાછા OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
channel_save(channel_T *channel, ch_part_T part, char_u *buf, પૂર્ણાંક len,
						      પૂર્ણાંક prepend, અક્ષર *lead)
{
    readq_T *ગાંઠ;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    પૂર્ણાંક	    i;

    ગાંઠ = ALLOC_ONE(readq_T);
    જો (ગાંઠ == ખાલી)
	પાછા FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    ગાંઠ->rq_buffer = alloc(len + 1);
    જો (ગાંઠ->rq_buffer == ખાલી)
    {
	vim_free(ગાંઠ);
	પાછા FAIL;	    // out of memory
    }

    જો (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = ગાંઠ->rq_buffer;
	માટે (i = 0; i < len; ++i)
	    જો (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	ગાંઠ->rq_buflen = (long_u)(p - ગાંઠ->rq_buffer);
    }
    નહીંતર
    {
	mch_memmove(ગાંઠ->rq_buffer, buf, len);
	ગાંઠ->rq_buffer[len] = NUL;
	ગાંઠ->rq_buflen = (long_u)len;
    }

    જો (prepend)
    {
	// prepend node to the head of the queue
	ગાંઠ->rq_next = head->rq_next;
	ગાંઠ->rq_prev = ખાલી;
	જો (head->rq_next == ખાલી)
	    head->rq_prev = ગાંઠ;
	નહીંતર
	    head->rq_next->rq_prev = ગાંઠ;
	head->rq_next = ગાંઠ;
    }
    નહીંતર
    {
	// append node to the tail of the queue
	ગાંઠ->rq_next = ખાલી;
	ગાંઠ->rq_prev = head->rq_prev;
	જો (head->rq_prev == ખાલી)
	    head->rq_next = ગાંઠ;
	નહીંતર
	    head->rq_prev->rq_next = ગાંઠ;
	head->rq_prev = ગાંઠ;
    }

    જો (ch_log_active() && lead != ખાલી)
	ch_log_literal(lead, channel, part, buf, len);

    પાછા OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    પૂર્ણાંક
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, ખાલી);
    પૂર્ણાંક		keeplen;
    પૂર્ણાંક		addlen;
    char_u	*p;

    જો (next == ખાલી)
	પાછા FALSE;

    keeplen = reader->js_end - reader->js_buf;
    જો (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (પૂર્ણાંક)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	જો (p == ખાલી)
	{
	    vim_free(next);
	    પાછા FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    પાછા TRUE;
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
    સ્થિર પૂર્ણાંક
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*લીટી_start;
    char_u	*p;
    int_u	hdr_len;
    પૂર્ણાંક		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    જ્યારે (TRUE)
    {
	લીટી_start = p;
	જ્યારે (*p != NUL && *p != '\n')
	    p++;
	જો (*p == NUL)			// partial header
	    પાછા MAYBE;
	p++;

	// process the content length field (if present)
	જો ((p - લીટી_start > 16)
		&& STRNICMP(લીટી_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((અક્ષર *)લીટી_start + 16, ખાલી, 10);
	    જો (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		પાછા FAIL;
	}

	જો ((p - લીટી_start) == 2 && લીટી_start[0] == '\r' &&
		લીટી_start[1] == '\n')
	    // reached the empty line
	    તોડો;
    }

    જો (payload_len == -1)
	// Content-Length field is not present in the header
	પાછા FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    જો (jsbuf_len < hdr_len + payload_len)
	પાછા MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    પાછા OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    પૂર્ણાંક
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    પૂર્ણાંક		status = OK;
    પૂર્ણાંક		ret;

    જો (channel_peek(channel, part) == ખાલી)
	પાછા FALSE;

    reader.js_buf = channel_get(channel, part, ખાલી);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    જો (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    જો (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    જો (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	જો (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	નહીંતર જો (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    જો (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    નહીંતર
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	નહીંતર
	{
	    item = ALLOC_ONE(jsonq_T);
	    જો (item == ખાલી)
		clear_tv(&listtv);
	    નહીંતર
	    {
		item->jq_no_callback = FALSE;
		item->jq_કિંમત = alloc_tv();
		જો (item->jq_કિંમત == ખાલી)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		નહીંતર
		{
		    *item->jq_કિંમત = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = ખાલી;
		    જો (item->jq_prev == ખાલી)
			head->jq_next = item;
		    નહીંતર
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    જો (status == OK)
	chanpart->ch_wait_len = 0;
    નહીંતર જો (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	જો (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (પૂર્ણાંક)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_ગણતરી() + 100L;
#નહીંતર
	    gettimeofday(&chanpart->ch_deadline, ખાલી);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    જો (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	નહીંતર
	{
	    પૂર્ણાંક timeout;
#ifdef MSWIN
	    timeout = (પૂર્ણાંક)(Get_Tick_ગણતરી() - chanpart->ch_deadline) > 0;
#નહીંતર
	    {
		સંરચના timeval now_tv;

		gettimeofday(&now_tv, ખાલી);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    જો (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    નહીંતર
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    જો (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    નહીંતર જો (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(પૂર્ણાંક)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, ખાલી);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    નહીંતર
	ret = FALSE;

    vim_free(reader.js_buf);
    પાછા ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    સ્થિર શૂન્ય
remove_cb_ગાંઠ(cbq_T *head, cbq_T *ગાંઠ)
{
    જો (ગાંઠ->cq_prev == ખાલી)
	head->cq_next = ગાંઠ->cq_next;
    નહીંતર
	ગાંઠ->cq_prev->cq_next = ગાંઠ->cq_next;
    જો (ગાંઠ->cq_next == ખાલી)
	head->cq_prev = ગાંઠ->cq_prev;
    નહીંતર
	ગાંઠ->cq_next->cq_prev = ગાંઠ->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    શૂન્ય
remove_json_ગાંઠ(jsonq_T *head, jsonq_T *ગાંઠ)
{
    જો (ગાંઠ->jq_prev == ખાલી)
	head->jq_next = ગાંઠ->jq_next;
    નહીંતર
	ગાંઠ->jq_prev->jq_next = ગાંઠ->jq_next;
    જો (ગાંઠ->jq_next == ખાલી)
	head->jq_prev = ગાંઠ->jq_prev;
    નહીંતર
	ગાંઠ->jq_next->jq_prev = ગાંઠ->jq_prev;
    vim_free(ગાંઠ);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    સ્થિર શૂન્ય
channel_add_block_id(chanpart_T *chanpart, પૂર્ણાંક id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    જો (gap->ga_growsize == 0)
	ga_init2(gap, કદ(પૂર્ણાંક), 10);
    જો (ga_grow(gap, 1) == OK)
    {
	((પૂર્ણાંક *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    સ્થિર શૂન્ય
channel_remove_block_id(chanpart_T *chanpart, પૂર્ણાંક id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    પૂર્ણાંક		i;

    માટે (i = 0; i < gap->ga_len; ++i)
	જો (((પૂર્ણાંક *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    જો (i < gap->ga_len)
	    {
		પૂર્ણાંક *p = ((પૂર્ણાંક *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * કદ(પૂર્ણાંક));
	    }
	    પાછા;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    સ્થિર પૂર્ણાંક
channel_has_block_id(chanpart_T *chanpart, પૂર્ણાંક id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    પૂર્ણાંક		i;

    માટે (i = 0; i < gap->ga_len; ++i)
	જો (((પૂર્ણાંક *)gap->ga_data)[i] == id)
	    પાછા TRUE;
    પાછા FALSE;
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
    સ્થિર પૂર્ણાંક
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	પૂર્ણાંક	    id,
	પૂર્ણાંક	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    જ્યારે (item != ખાલી)
    {
	list_T	    *l;
	typval_T    *tv;

	જો (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_કિંમત->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	નહીંતર
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    જો (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		જો (id > 0)
		{
		    જો (item->jq_કિંમત->v_type != VAR_DICT)
			જાઓ nextitem;
		    d = item->jq_કિંમત->vval.v_dict;
		    જો (d == ખાલી)
			જાઓ nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    જો (dict_has_ચાવી(d, "method"))
			જાઓ nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    જો (di == ખાલી)
			જાઓ nextitem;
		    tv = &di->di_tv;
		}
		નહીંતર
		    tv = item->jq_કિંમત;
	    }
	    નહીંતર
	    {
		જો (id > 0)
		{
		    જો (item->jq_કિંમત->v_type != VAR_DICT)
			જાઓ nextitem;
		    d = item->jq_કિંમત->vval.v_dict;
		    જો (d == ખાલી)
			જાઓ nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    જો (di == ખાલી)
			જાઓ nextitem;
		    tv = &di->di_tv;
		}
		નહીંતર
		    tv = item->jq_કિંમત;
	    }
	}

	જો ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_સંખ્યા && tv->vval.v_સંખ્યા == id)
	      || (id <= 0 && (tv->v_type != VAR_સંખ્યા
		 || tv->vval.v_સંખ્યા == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_સંખ્યા)))))
	{
	    *rettv = item->jq_કિંમત;
	    જો (tv->v_type == VAR_સંખ્યા)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_સંખ્યા);
	    remove_json_ગાંઠ(head, item);
	    પાછા OK;
	}
nextitem:
	item = item->jq_next;
    }
    પાછા FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    સ્થિર શૂન્ય
channel_ધકેલો_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    જો (head->jq_prev != ખાલી && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = ખાલી;
    નહીંતર જ્યારે (item != ખાલી && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    જો (newitem == ખાલી)
    {
	clear_tv(rettv);
	પાછા;
    }

    newitem->jq_કિંમત = alloc_tv();
    જો (newitem->jq_કિંમત == ખાલી)
    {
	vim_free(newitem);
	clear_tv(rettv);
	પાછા;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_કિંમત = *rettv;
    જો (item == ખાલી)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = ખાલી;
	જો (newitem->jq_prev == ખાલી)
	    head->jq_next = newitem;
	નહીંતર
	    newitem->jq_prev->jq_next = newitem;
    }
    નહીંતર
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	જો (newitem->jq_next == ખાલી)
	    head->jq_prev = newitem;
	નહીંતર
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    સ્થિર શૂન્ય
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    પૂર્ણાંક	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    જો (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	જો (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	પાછા;
    }
    arg = argv[1].vval.v_string;
    જો (arg == ખાલી)
	arg = (char_u *)"";

    જો (STRCMP(cmd, "ex") == 0)
    {
	પૂર્ણાંક	called_emsg_before = called_emsg;
	char_u	*p = arg;
	પૂર્ણાંક	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (અક્ષર *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	જો (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	જો (do_emsg_silent)
	    --emsg_silent;
	જો (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (અક્ષર *)get_vim_var_str(VV_ERRMSG));
    }
    નહીંતર જો (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (અક્ષર *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_ગણતરી = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    નહીંતર જો (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    નહીંતર જો (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	પૂર્ણાંક is_call = cmd[0] == 'c';
	પૂર્ણાંક id_idx = is_call ? 3 : 2;

	જો (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_સંખ્યા)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    જો (p_verbose > 2)
		emsg(_(e_last_argument_for_પદાવલિ_call_must_be_સંખ્યા));
	}
	નહીંતર જો (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    જો (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	નહીંતર
	{
	    typval_T	*tv = ખાલી;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ખાલી;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    જો (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (અક્ષર *)arg);
		tv = eval_પદાવલિ(arg, ખાલી);
	    }
	    નહીંતર
	    {
		ch_log(channel, "Calling '%s'", (અક્ષર *)arg);
		જો (func_call(arg, &argv[2], ખાલી, ખાલી, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    જો (argv[id_idx].v_type == VAR_સંખ્યા)
	    {
		પૂર્ણાંક id = argv[id_idx].vval.v_સંખ્યા;

		જો (tv != ખાલી)
		    json = json_encode_nr_પદાવલિ(id, tv, options | JSON_NL);
		જો (tv == ખાલી || (json != ખાલી && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_પદાવલિ(id, &err_tv, options | JSON_NL);
		}
		જો (json != ખાલી)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (પૂર્ણાંક)STRLEN(json), (અક્ષર *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    જો (tv == &res_tv)
		clear_tv(tv);
	    નહીંતર
		free_tv(tv);
	}
    }
    નહીંતર જો (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (અક્ષર *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    સ્થિર શૂન્ય
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (અક્ષર *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_ગાંઠ(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    સ્થિર શૂન્ય
append_to_buffer(
    buf_T	*buffer,
    char_u	*સંદેશ,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_લીટી_ગણતરી;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    પૂર્ણાંક		save_p_ma = buffer->b_p_ma;
    પૂર્ણાંક		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    જો (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	જો (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	પાછા;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    જો (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (પૂર્ણાંક)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    જો (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	પાછા;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    જો (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, સંદેશ, TRUE);
	lnum = 0;
    }
    નહીંતર
	ml_append(lnum, સંદેશ, 0, FALSE);
    appended_લીટીઓ_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    જો (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    નહીંતર
	buffer->b_p_ma = save_p_ma;

    જો (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    જો (wp->w_buffer == buffer)
	    {
		પૂર્ણાંક move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		જો (move_cursor || wp->w_topline > buffer->b_ml.ml_લીટી_ગણતરી)
		{
		    win_T *save_curwin = curwin;

		    જો (move_cursor)
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

    જો (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    જો (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_લીટી_ગણતરી;
	}
    }
}

    સ્થિર શૂન્ય
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *સંદેશ;

    જ્યારે ((સંદેશ = channel_get(channel, part, ખાલી)) != ખાલી)
    {
	ch_log(channel, "Dropping message '%s'", (અક્ષર *)સંદેશ);
	vim_free(સંદેશ);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    સ્થિર પૂર્ણાંક
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    પાછા ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    સ્થિર પૂર્ણાંક
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*સંદેશ = ખાલી;
    blob_T	*blob = ખાલી;
    typval_T	*listtv = ખાલી;
    typval_T	argv[CH_JSON_MAX_ARGS];
    પૂર્ણાંક		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = ખાલી;
    buf_T	*buffer = ખાલી;
    char_u	*p;
    પૂર્ણાંક		called_otc;		// one time callbackup
    પૂર્ણાંક		raw_len = 0;

    જો (channel->ch_nb_close_cb != ખાલી
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	પાછા FALSE;

    // Use a message-specific callback, part callback or channel callback
    માટે (cbitem = cbhead->cq_next; cbitem != ખાલી; cbitem = cbitem->cq_next)
	જો (cbitem->cq_seq_nr == 0)
	    તોડો;
    જો (cbitem != ખાલી)
	callback = &cbitem->cq_callback;
    નહીંતર જો (ch_part->ch_callback.cb_name != ખાલી)
	callback = &ch_part->ch_callback;
    નહીંતર જો (channel->ch_callback.cb_name != ખાલી)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    જો (buffer != ખાલી && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == ખાલી))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = ખાલી;
	buffer = ખાલી;
    }

    જો (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	પૂર્ણાંક		argc = 0;

	// Get any json message in the queue.
	જો (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    જો (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(શૂન્ય)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    જો (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		પાછા FALSE;
	}

	જો (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    જો (d != ખાલી)
	    {
		જો (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		નહીંતર
		    di = dict_find(d, (char_u *)"request_seq", -1);
		જો (di != ખાલી && di->di_tv.v_type == VAR_સંખ્યા)
		    seq_nr = di->di_tv.vval.v_સંખ્યા;
	    }

	    argv[1] = *listtv;
	}
	નહીંતર
	{
	    માટે (item = listtv->vval.v_list->lv_first;
		    item != ખાલી && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    જ્યારે (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    જો (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		પાછા TRUE;
	    }

	    જો (argv[0].v_type != VAR_સંખ્યા)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		પાછા FALSE;
	    }
	    seq_nr = argv[0].vval.v_સંખ્યા;
	}
    }
    નહીંતર જો (channel_peek(channel, part) == ખાલી)
    {
	// nothing to read on RAW or NL channel
	પાછા FALSE;
    }
    નહીંતર
    {
	// If there is no callback or buffer drop the message.
	જો (callback == ખાલી && buffer == ખાલી)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    જો (channel->ch_close_cb.cb_name == ખાલી && !channel->ch_drop_never)
		drop_messages(channel, part);
	    પાછા FALSE;
	}

	જો (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = ખાલી;
	    char_u  *buf;
	    readq_T *ગાંઠ;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    જ્યારે (TRUE)
	    {
		ગાંઠ = channel_peek(channel, part);
		nl = channel_first_nl(ગાંઠ);
		જો (nl != ખાલી)
		    તોડો;
		જો (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    જો (ch_part->ch_fd == INVALID_FD && ગાંઠ->rq_buflen > 0)
			તોડો;
		    પાછા FALSE; // incomplete message
		}
	    }
	    buf = ગાંઠ->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    માટે (p = buf; (nl == ખાલી || p < nl)
					    && p < buf + ગાંઠ->rq_buflen; ++p)
		જો (*p == NUL)
		    *p = NL;

	    જો (nl == ખાલી)
	    {
		// get the whole buffer, drop the NL
		સંદેશ = channel_get(channel, part, ખાલી);
	    }
	    નહીંતર જો (nl + 1 == buf + ગાંઠ->rq_buflen)
	    {
		// get the whole buffer
		સંદેશ = channel_get(channel, part, ખાલી);
		*nl = NUL;
	    }
	    નહીંતર
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		સંદેશ = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (પૂર્ણાંક)(nl - buf) + 1);
	    }
	}
	નહીંતર
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    સંદેશ = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : ખાલી);
	}

	જો (સંદેશ == ખાલી)
	    પાછા FALSE; // out of memory (and avoids Coverity warning)

	જો (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    જો (blob == ખાલી)
	    {
		vim_free(સંદેશ);
		પાછા FALSE;
	    }
	    જો (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(સંદેશ);
		પાછા FALSE;
	    }
	    જો (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, સંદેશ, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	નહીંતર
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = સંદેશ;
	}
    }

    called_otc = FALSE;
    જો (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	પૂર્ણાંક lsp_req_સંદેશ = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	જો ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_ચાવી(argv[1].vval.v_dict, "method"))
	    lsp_req_સંદેશ = TRUE;

	જો (!lsp_req_સંદેશ)
	{
	    માટે (cbitem = cbhead->cq_next; cbitem != ખાલી;
		    cbitem = cbitem->cq_next)
	    {
		જો (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    તોડો;
		}
	    }
	}
    }

    જો (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	જો (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    જો (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ધકેલો_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_સંખ્યા;
		free_tv(listtv);
		listtv = ખાલી;
	    }
	    નહીંતર
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    નહીંતર જો (callback != ખાલી || buffer != ખાલી)
    {
	જો (buffer != ખાલી)
	{
	    જો (સંદેશ == ખાલી)
		// JSON or JS mode: re-encode the message.
		સંદેશ = json_encode(listtv, ch_mode);
	    જો (સંદેશ != ખાલી)
	    {
#ifdef FEAT_TERMINAL
		જો (buffer->b_term != ખાલી)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    જો (part == PART_ERR)
		    {
			char_u *cp;
			પૂર્ણાંક    extra = 0;

			માટે (cp = સંદેશ; *cp != NUL; ++cp)
			    જો (*cp == NL && (cp == સંદેશ || cp[-1] != CAR))
				++extra;
			જો (extra > 0)
			{
			    char_u *crlf_સંદેશ = alloc(STRLEN(સંદેશ) + extra + 1);

			    જો (crlf_સંદેશ != ખાલી)
			    {
				char_u *q = crlf_સંદેશ;

				માટે (cp = સંદેશ; *cp != NUL; ++cp)
				{
				    જો (*cp == NL && (cp == સંદેશ || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_સંદેશ, channel);
				vim_free(crlf_સંદેશ);
			    }
			}
			નહીંતર
			    write_to_term(buffer, સંદેશ, channel);
		    }
		    નહીંતર
			write_to_term(buffer, સંદેશ, channel);
		}
		નહીંતર
#endif
		    append_to_buffer(buffer, સંદેશ, channel, part);
	    }
	}

	જો (callback != ખાલી)
	{
	    જો (cbitem != ખાલી)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    નહીંતર
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (અક્ષર *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    નહીંતર
	ch_log(channel, "Dropping message %d", seq_nr);

    જો (listtv != ખાલી)
	free_tv(listtv);
    જો (blob != ખાલી)
	blob_unref(blob);
    vim_free(સંદેશ);

    પાછા TRUE;
}

#જો defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    પૂર્ણાંક
channel_can_write_to(channel_T *channel)
{
    પાછા channel != ખાલી && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    પૂર્ણાંક
channel_is_open(channel_T *channel)
{
    પાછા channel != ખાલી && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    સ્થિર શૂન્ય *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    જો (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	જો (head->jq_next == ખાલી)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	પાછા head->jq_next;
    }
    પાછા channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    સ્થિર પૂર્ણાંક
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    પાછા channel_readahead_pointer(channel, part) != ખાલી;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    સ્થિર અક્ષર *
channel_status(channel_T *channel, પૂર્ણાંક req_part)
{
    ch_part_T part;
    પૂર્ણાંક has_readahead = FALSE;

    જો (channel == ખાલી)
	 પાછા "fail";
    જો (req_part == PART_OUT)
    {
	જો (channel->CH_OUT_FD != INVALID_FD)
	    પાછા "open";
	જો (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    નહીંતર જો (req_part == PART_ERR)
    {
	જો (channel->CH_ERR_FD != INVALID_FD)
	    પાછા "open";
	જો (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    નહીંતર
    {
	જો (channel_is_open(channel))
	    પાછા "open";
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	    જો (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		તોડો;
	    }
    }

    જો (has_readahead)
	પાછા "buffered";
    પાછા "closed";
}

    સ્થિર શૂન્ય
channel_part_info(channel_T *channel, dict_T *dict, અક્ષર *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    અક્ષર	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    જો (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    નહીંતર જો (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    નહીંતર
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (પૂર્ણાંક)s.length);

    STRCPY(namebuf + tail, "mode");
    પસંદગી (chanpart->ch_mode)
    {
	સ્થિતિ CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    તોડો;
	સ્થિતિ CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    તોડો;
	સ્થિતિ CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    તોડો;
	સ્થિતિ CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    તોડો;
	સ્થિતિ CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    તોડો;
	સ્થિતિ CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    તોડો;
	સ્થિતિ CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    તોડો;
	મૂળભૂત:
	    STR_LITERAL_SET(s, "");
	    તોડો;
    }
    dict_add_string_len(dict, namebuf, s.string, (પૂર્ણાંક)s.length);

    STRCPY(namebuf + tail, "io");
    જો (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    નહીંતર પસંદગી (chanpart->ch_io)
    {
	સ્થિતિ JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    તોડો;
	સ્થિતિ JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    તોડો;
	સ્થિતિ JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    તોડો;
	સ્થિતિ JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    તોડો;
	સ્થિતિ JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    તોડો;
	મૂળભૂત:
	    STR_LITERAL_SET(s, "");
	    તોડો;
    }
    dict_add_string_len(dict, namebuf, s.string, (પૂર્ણાંક)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_સંખ્યા(dict, namebuf, chanpart->ch_timeout);
}

    સ્થિર શૂન્ય
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_સંખ્યા(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    જો (channel->ch_hostname != ખાલી)
    {
	જો (channel->ch_port)
	    dict_add_સંખ્યા(dict, "port", channel->ch_port);
	નહીંતર
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    નહીંતર
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
    શૂન્ય
channel_close(channel_T *channel, પૂર્ણાંક invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    જો (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	જો (channel->ch_close_cb.cb_name != ખાલી)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	{
	    જો (channel->ch_close_cb.cb_name != ખાલી
			    || channel->ch_part[part].ch_bufref.br_buf != ખાલી)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		જો (channel->ch_close_cb.cb_name == ખાલી)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		જ્યારે (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	જો (channel->ch_close_cb.cb_name != ખાલી)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (અક્ષર *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      જો (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      જો (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  માટે (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ખાલી;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ખાલી;
    channel->ch_ss_close_cb = ખાલી;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    સ્થિર શૂન્ય
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    સ્થિર શૂન્ય
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    જો (wq->wq_next == ખાલી)
	wq->wq_prev = ખાલી;
    નહીંતર
	wq->wq_next->wq_prev = ખાલી;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    સ્થિર શૂન્ય
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    જ્યારે (channel_peek(channel, part) != ખાલી)
	vim_free(channel_get(channel, part, ખાલી));

    જ્યારે (cb_head->cq_next != ખાલી)
    {
	cbq_T *ગાંઠ = cb_head->cq_next;

	remove_cb_ગાંઠ(cb_head, ગાંઠ);
	free_callback(&ગાંઠ->cq_callback);
	vim_free(ગાંઠ);
    }

    જ્યારે (json_head->jq_next != ખાલી)
    {
	free_tv(json_head->jq_next->jq_કિંમત);
	remove_json_ગાંઠ(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    જ્યારે (ch_part->ch_writeque.wq_next != ખાલી)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    શૂન્ય
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

#જો defined(EXITFREE)
    શૂન્ય
channel_free_all(શૂન્ય)
{
    channel_T *channel;

    ch_log(ખાલી, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_સંદેશ_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    સ્થિર પૂર્ણાંક
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    જો (in_part->ch_writeque.wq_next != ખાલી)
	પાછા TRUE;
    જો (buf == ખાલી)
	પાછા FALSE;
    પાછા in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_લીટી_ગણતરી)
	    : (in_part->ch_buf_ટોચ <= in_part->ch_buf_bot
			    && in_part->ch_buf_ટોચ <= buf->b_ml.ml_લીટી_ગણતરી);
}

#જો defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    સ્થિર પૂર્ણાંક
channel_fill_wfds(પૂર્ણાંક maxfd_arg, fd_set *wfds)
{
    પૂર્ણાંક		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	જો (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (પૂર્ણાંક)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((પૂર્ણાંક)in_part->ch_fd, wfds);
	    જો ((પૂર્ણાંક)in_part->ch_fd >= maxfd)
		maxfd = (પૂર્ણાંક)in_part->ch_fd + 1;
	}
    }
    પાછા maxfd;
}
#નહીંતર
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    સ્થિર પૂર્ણાંક
channel_fill_poll_write(પૂર્ણાંક nfd_in, સંરચના pollfd *fds)
{
    પૂર્ણાંક		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	જો (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	નહીંતર
	    in_part->ch_poll_idx = -1;
    }
    પાછા nfd;
}
#endif

પ્રકારવ્યાખ્યા ગણના {
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
    સ્થિર channel_wait_result
channel_wait(channel_T *channel, sock_T fd, પૂર્ણાંક timeout)
{
    જો (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    જો (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	પૂર્ણાંક	sleep_time;
	DWORD	deadline = Get_Tick_ગણતરી() + timeout;
	પૂર્ણાંક	delay = 1;

	// reading from a pipe, not a socket
	જ્યારે (TRUE)
	{
	    પૂર્ણાંક r = PeekNamedPipe((HANDLE)fd, ખાલી, 0, ખાલી, &nread, ખાલી);

	    જો (r && nread > 0)
		પાછા CW_READY;

	    જો (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ખાલી);
	    }
	    નહીંતર જો (r == 0)
		પાછા CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_લીટીઓ();

	    sleep_time = deadline - Get_Tick_ગણતરી();
	    જો (sleep_time <= 0)
		તોડો;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    જો (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    જો (delay > 10)
		delay = 10;
	}
    }
    નહીંતર
#endif
    {
#જો defined(HAVE_SELECT)
	સંરચના timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	પૂર્ણાંક		ret;
	પૂર્ણાંક		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	માટે (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((પૂર્ણાંક)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (પૂર્ણાંક)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ખાલી, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    જો (ret == -1 && errno == EINTR)
		ચાલુ;
# endif
	    જો (ret > 0)
	    {
		જો (FD_ISSET(fd, &rfds))
		    પાછા CW_READY;
		channel_write_any_લીટીઓ();
		ચાલુ;
	    }
	    તોડો;
	}
#નહીંતર
	માટે (;;)
	{
	    સંરચના pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    પૂર્ણાંક		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    જો (poll(fds, nfd, timeout) > 0)
	    {
		જો (fds[0].revents & POLLIN)
		    પાછા CW_READY;
		channel_write_any_લીટીઓ();
		ચાલુ;
	    }
	    તોડો;
	}
#endif
    }
    પાછા CW_NOT_READY;
}

    સ્થિર શૂન્ય
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, પૂર્ણાંક is_err, અક્ષર *func)
{
    અક્ષર	સંદેશ[] = "%s(): Read %s from ch_part[%d], closing";

    જો (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, સંદેશ, func, "error", part);
    નહીંતર
	ch_log(channel, સંદેશ, func, "EOF", part);

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
    જો (channel->ch_nb_close_cb != ખાલી)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_સંદેશ_RAW,
			      (પૂર્ણાંક)STRLEN_LITERAL(DETACH_સંદેશ_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    જો (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	જો (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    સ્થિર શૂન્ય
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    જો (channel->ch_nb_close_cb != ખાલી)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    જો (channel->ch_ss_close_cb != ખાલી)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    સ્થિર શૂન્ય
channel_read(channel_T *channel, ch_part_T part, અક્ષર *func)
{
    સ્થિર char_u	*buf = ખાલી;
    પૂર્ણાંક			len = 0;
    પૂર્ણાંક			readlen = 0;
    sock_T		fd;
    પૂર્ણાંક			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    જો (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	પાછા;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    જો (buf == ખાલી)
    {
	buf = alloc(MAXMSGSIZE);
	જો (buf == ખાલી)
	    પાછા;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    માટે (;;)
    {
	જો (channel_wait(channel, fd, 0) != CW_READY)
	    તોડો;
	જો (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    સંરચના sockaddr_storage	client;

	    newchannel = add_channel();
	    જો (newchannel == ખાલી)
	    {
		ch_error(ખાલી, "Cannot allocate channel.");
		પાછા;
	    }
	    socklen = કદ(client);
	    newfd = accept(fd, (સંરચના sockaddr*)&client, &socklen);
	    જો (newfd < 0)
	    {
		ch_error(ખાલી, "Cannot accept channel.");
		channel_free(newchannel);
		પાછા;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    જો (channel->ch_ss_accept_cb != ખાલી)
	    {
		channel->ch_ss_accept_cb(newchannel);
		પાછા;
	    }
#endif

	    જો (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		અક્ષર addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((સંરચના sockaddr_in*)&client)->sin_addr,
			addr, કદ(addr));
		vim_snprintf((અક્ષર *)namebuf, કદ(namebuf), "%s:%d",
			addr,
			ntohs(((સંરચના sockaddr_in*)&client)->sin_port));
#નહીંતર
		vim_snprintf((અક્ષર *)namebuf, કદ(namebuf), "%s:%d",
		    inet_ntoa(((સંરચના sockaddr_in*)&client)->sin_addr),
		    ntohs(((સંરચના sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    નહીંતર જો (client.ss_family == AF_INET6)
	    {
		અક્ષર addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((સંરચના sockaddr_in6*)&client)->sin6_addr,
			addr, કદ(addr));
		vim_snprintf((અક્ષર *)namebuf, કદ(namebuf), "[%s]:%d",
			addr,
			ntohs(((સંરચના sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    નહીંતર જો (client.ss_family == AF_UNIX)
		vim_snprintf((અક્ષર *)namebuf, કદ(namebuf),
							    "unix:anonymous");
	    નહીંતર
		vim_snprintf((અક્ષર *)namebuf, કદ(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    પાછા;
	}
	જો (use_socket)
	    len = sock_read(fd, (અક્ષર *)buf, MAXMSGSIZE);
	નહીંતર
	    len = fd_read(fd, (અક્ષર *)buf, MAXMSGSIZE);
	જો (len <= 0)
	    તોડો;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    જો (readlen <= 0)
    {
	જો (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#જો defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    નહીંતર જો (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    શૂન્ય
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
    સ્થિર char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, પૂર્ણાંક timeout, પૂર્ણાંક raw, પૂર્ણાંક *outlen)
{
    char_u	*buf;
    char_u	*સંદેશ;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*ગાંઠ;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    જ્યારે (TRUE)
    {
	ગાંઠ = channel_peek(channel, part);
	જો (ગાંઠ != ખાલી)
	{
	    જો (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(ગાંઠ) != ખાલી))
		// got a complete message
		તોડો;
	    જો (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		ચાલુ;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    જો (raw || fd == INVALID_FD)
		તોડો;
	}

	// Wait for up to the channel timeout.
	જો (fd == INVALID_FD)
	    પાછા ખાલી;
	જો (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    પાછા ખાલી;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    જો (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != ખાલી)
    {
	સંદેશ = channel_get_all(channel, part, outlen);
    }
    નહીંતર
    {
	char_u *p;

	buf = ગાંઠ->rq_buffer;
	nl = channel_first_nl(ગાંઠ);

	// Convert NUL to NL, the internal representation.
	માટે (p = buf; (nl == ખાલી || p < nl) && p < buf + ગાંઠ->rq_buflen; ++p)
	    જો (*p == NUL)
		*p = NL;

	જો (nl == ખાલી)
	{
	    // must be a closed channel with missing NL
	    સંદેશ = channel_get(channel, part, ખાલી);
	}
	નહીંતર જો (nl + 1 == buf + ગાંઠ->rq_buflen)
	{
	    // get the whole buffer
	    સંદેશ = channel_get(channel, part, ખાલી);
	    *nl = NUL;
	}
	નહીંતર
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    સંદેશ = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (પૂર્ણાંક)(nl - buf) + 1);
	}
    }
    જો (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != ખાલી ? *outlen : (પૂર્ણાંક)STRLEN(સંદેશ));
    પાછા સંદેશ;
}

સ્થિર પૂર્ણાંક channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    પૂર્ણાંક
channel_in_blocking_wait(શૂન્ય)
{
    પાછા channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    સ્થિર શૂન્ય
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, પૂર્ણાંક timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (શૂન્ય)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#નહીંતર
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
    સ્થિર પૂર્ણાંક
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	પૂર્ણાંક	    timeout_arg,
	પૂર્ણાંક	    id,
	typval_T    **rettv)
{
    પૂર્ણાંક		more;
    sock_T	fd;
    પૂર્ણાંક		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    પૂર્ણાંક		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    જો (id >= 0)
	channel_add_block_id(chanpart, id);

    માટે (;;)
    {
	જો (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (શૂન્ય)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	જો (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    તોડો;
	}

	જો (!more)
	{
	    શૂન્ય *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    શૂન્ય *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    જો (channel_parse_messages())
		ચાલુ;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    જો (readahead_ptr != ખાલી && readahead_ptr != prev_readahead_ptr)
		ચાલુ;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (પૂર્ણાંક)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    જો (timeout <= 0 || fd == INVALID_FD)
	    {
		જો (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		તોડો;
	    }
	    // If there was an incomplete message use the deadline for that.
	    જો (chanpart->ch_wait_len > 0)
	    {
		પૂર્ણાંક	ડાબું;
#ifdef MSWIN
		ડાબું = chanpart->ch_deadline - Get_Tick_ગણતરી() + 1;
#નહીંતર
		{
		    સંરચના timeval now_tv;

		    gettimeofday(&now_tv, ખાલી);
		    ડાબું = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		જો (ડાબું < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		નહીંતર જો (ડાબું < timeout)
		    timeout = ડાબું;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    જો (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    પાછા retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, પૂર્ણાંક check_open, પૂર્ણાંક reading, ch_part_T part)
{
    channel_T	*channel = ખાલી;
    પૂર્ણાંક		has_readahead = FALSE;

    જો (tv->v_type == VAR_JOB)
    {
	જો (tv->vval.v_job != ખાલી)
	    channel = tv->vval.v_job->jv_channel;
    }
    નહીંતર જો (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    નહીંતર
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	પાછા ખાલી;
    }
    જો (channel != ખાલી && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_ગણતરી ? part : channel_part_read(channel));

    જો (check_open && (channel == ખાલી || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	પાછા ખાલી;
    }
    પાછા channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    સ્થિર શૂન્ય
common_channel_read(typval_T *argvars, typval_T *rettv, પૂર્ણાંક raw, પૂર્ણાંક blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_ગણતરી;
    jobopt_T	opt;
    પૂર્ણાંક		mode;
    પૂર્ણાંક		timeout;
    પૂર્ણાંક		id = -1;
    typval_T	*listtv = ખાલી;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    clear_job_options(&opt);
    જો (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	જાઓ theend;

    જો (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    જો (channel == ખાલી)
	જાઓ theend;

    જો (part == PART_ગણતરી)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    જો (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    જો (blob || mode == CH_MODE_BLOB)
    {
	પૂર્ણાંક	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	જો (p != ખાલી)
	{
	    blob_T	*b = blob_alloc();

	    જો (b != ખાલી)
	    {
		b->bv_ga.ga_len = outlen;
		જો (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		નહીંતર
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    નહીંતર જો (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, ખાલી);
    નહીંતર
    {
	જો (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	જો (listtv != ખાલી)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	નહીંતર
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_સંખ્યા = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#જો defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    શૂન્ય
channel_handle_events(પૂર્ણાંક only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	જો (only_keep_open && !channel->ch_keep_open)
	    ચાલુ;

	// check the socket and pipes
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    જો (fd == INVALID_FD)
		ચાલુ;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    પૂર્ણાંક r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    જો (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    નહીંતર જો (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	જો (channel->ch_job != ખાલી)
	{
	    job_T *job = channel->ch_job;

	    જો (job->jv_tty_out != ખાલી && job->jv_status == JOB_FINISHED)
		માટે (part = PART_SOCK; part < PART_ગણતરી; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#જો defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    પૂર્ણાંક
channel_any_keep_open(શૂન્ય)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	જો (channel->ch_keep_open)
	    પાછા TRUE;
    પાછા FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    શૂન્ય
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    પૂર્ણાંક		fd = ch_part->ch_fd;

    જો (fd == INVALID_FD)
	પાછા;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#નહીંતર
    (શૂન્ય)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    પૂર્ણાંક
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	પૂર્ણાંક	  len_arg,
	અક્ષર	  *fun)
{
    પૂર્ણાંક		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    પૂર્ણાંક		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    જો (fd == INVALID_FD)
    {
	જો (!channel->ch_error && fun != ખાલી)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	પાછા FAIL;
    }

    જો (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    જો (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_સંદેશ = 0;
    }

    માટે (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	પૂર્ણાંક	    len;

	જો (wq->wq_next != ખાલી)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	નહીંતર
	{
	    જો (len_arg == 0)
		// nothing to write, called from channel_select_check()
		પાછા OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	જો (part == PART_SOCK)
	    res = sock_write(fd, (અક્ષર *)buf, len);
	નહીંતર
	{
	    res = fd_write(fd, (અક્ષર *)buf, len);
#ifdef MSWIN
	    જો (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ખાલી);
	    }
#endif
	}
	જો (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	જો (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    જો (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    જો (res == len)
	    {
		// Wrote all the buf[len] bytes.
		જો (entry != ખાલી)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    ચાલુ;
		}
		જો (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    નહીંતર
	    {
		// Wrote only buf[res] bytes, can't write more now.
		જો (entry != ખાલી)
		{
		    જો (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (અક્ષર *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		નહીંતર
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		જો (wq->wq_prev != ખાલી
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    જો (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((અક્ષર *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		નહીંતર
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    જો (last != ખાલી)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = ખાલી;
			જો (wq->wq_prev == ખાલી)
			    wq->wq_next = last;
			નહીંતર
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			જો (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	નહીંતર જો (res != len)
	{
	    જો (!channel->ch_error && fun != ખાલી)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    પાછા FAIL;
	}

	channel->ch_error = FALSE;
	પાછા OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    સ્થિર channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	પૂર્ણાંક	    len,
	પૂર્ણાંક	    id,
	પૂર્ણાંક	    eval,
	jobopt_T    *opt,
	અક્ષર	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    જો (channel == ખાલી)
	પાછા ખાલી;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    જો (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	પાછા ખાલી;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    જો (opt->jo_callback.cb_name != ખાલી && *opt->jo_callback.cb_name != NUL)
    {
	જો (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    પાછા ખાલી;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    જો (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == ખાલી)
	પાછા channel;
    પાછા ખાલી;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    સ્થિર શૂન્ય
ch_પદાવલિ_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    પૂર્ણાંક		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    પૂર્ણાંક		timeout;
    પૂર્ણાંક		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    જો (channel == ખાલી)
	પાછા;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    જો (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	પાછા;
    }

    જો (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	અક્ષર		*ચાવી = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	જો (rettv_dict_alloc(rettv) == FAIL)
	    પાછા;

	જો (check_for_dict_arg(argvars, 1) == FAIL)
	    પાછા;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)ચાવી, -1);
	જો (argvars[2].v_type == VAR_DICT
		&& dict_has_ચાવી(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	જો (di != ખાલી && di->di_tv.v_type != VAR_સંખ્યા
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_કિંમત_for_argument_str), ચાવી);
	    પાછા;
	}

	જો (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_સંદેશ_id;
	    જો (di == ખાલી)
		dict_add_સંખ્યા(d, "seq", id);
	    નહીંતર
		di->di_tv.vval.v_સંખ્યા = id;
	}
	નહીંતર જો (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_સંદેશ_id;
	    જો (di == ખાલી)
		dict_add_સંખ્યા(d, "id", id);
	    નહીંતર
		di->di_tv.vval.v_સંખ્યા = id;
	}
	નહીંતર
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    જો (di != ખાલી && di->di_tv.v_type == VAR_સંખ્યા)
		id = di->di_tv.vval.v_સંખ્યા;
	}
	જો (ch_mode == CH_MODE_LSP && !dict_has_ચાવી(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_સંદેશ(&argvars[1]);
    }
    નહીંતર
    {
	id = ++channel->ch_last_સંદેશ_id;
	text = json_encode_nr_પદાવલિ(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    જો (text == ખાલી)
	પાછા;

    channel = send_common(argvars, text, (પૂર્ણાંક)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    જો (channel != ખાલી && eval)
    {
	જો (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	નહીંતર
	    timeout = channel_get_timeout(channel, part_read);
	જો (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    જો (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_સંખ્યા;
		free_tv(listtv);
	    }
	    નહીંતર
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_સંખ્યા;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    જો (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	જો (rettv->vval.v_dict != ખાલી)
	    dict_add_સંખ્યા(rettv->vval.v_dict, "seq", id);
    }
    નહીંતર જો (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	જો (rettv->vval.v_dict != ખાલી)
	    dict_add_સંખ્યા(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    સ્થિર શૂન્ય
ch_raw_common(typval_T *argvars, typval_T *rettv, પૂર્ણાંક eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    પૂર્ણાંક		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    પૂર્ણાંક		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	પાછા;

    જો (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    નહીંતર
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (પૂર્ણાંક)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    જો (channel != ખાલી && eval)
    {
	જો (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	નહીંતર
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, ખાલી);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#જો defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    પૂર્ણાંક
channel_poll_setup(પૂર્ણાંક nfd_in, શૂન્ય *fds_in, પૂર્ણાંક *towait)
{
    પૂર્ણાંક		nfd = nfd_in;
    channel_T	*channel;
    સંરચના	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    જો (ch_part->ch_fd != INVALID_FD)
	    {
		જો (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    જો (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		નહીંતર
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    નહીંતર
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    પાછા nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    પૂર્ણાંક
channel_poll_check(પૂર્ણાંક ret_in, શૂન્ય *fds_in)
{
    પૂર્ણાંક		ret = ret_in;
    channel_T	*channel;
    સંરચના	pollfd *fds = fds_in;
    ch_part_T	part;
    પૂર્ણાંક		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    જો (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    નહીંતર જો (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	જો (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_આગત(channel);
	    --ret;
	}
    }

    પાછા ret;
}
#endif // UNIX && !HAVE_SELECT

#જો !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    પૂર્ણાંક
channel_select_setup(
	પૂર્ણાંક maxfd_in,
	શૂન્ય *rfds_in,
	શૂન્ય *wfds_in,
	સંરચના timeval *tv,
	સંરચના timeval **tvp)
{
    પૂર્ણાંક		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    જો (fd != INVALID_FD)
	    {
		જો (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    જો (*tvp == ખાલી || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		નહીંતર
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    જો ((પૂર્ણાંક)fd >= FD_SETSIZE)
			ચાલુ;
# endif
		    FD_SET((પૂર્ણાંક)fd, rfds);
		    જો (maxfd < (પૂર્ણાંક)fd)
			maxfd = (પૂર્ણાંક)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    પાછા maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    પૂર્ણાંક
channel_select_check(પૂર્ણાંક ret_in, શૂન્ય *rfds_in, શૂન્ય *wfds_in)
{
    પૂર્ણાંક		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	માટે (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    જો (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (પૂર્ણાંક)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    નહીંતર જો (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	જો (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (પૂર્ણાંક)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_આગત(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	જો (channel->ch_job != ખાલી)
	{
	    job_T *job = channel->ch_job;

	    જો (job->jv_tty_out != ખાલી && job->jv_status == JOB_FINISHED)
		માટે (part = PART_SOCK; part < PART_ગણતરી; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    પાછા ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    સ્થિર શૂન્ય
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    જો (stdio_channel->CH_OUT_FD != INVALID_FD)
	(શૂન્ય)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(પૂર્ણાંક)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#અથવાજો defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    સંરચના timeval	tv;
    સંરચના timeval	*tvp = &tv;
    પૂર્ણાંક			maxfd;
    પૂર્ણાંક			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ખાલી, tvp);
    જો (ret >= 0)
	(શૂન્ય)channel_select_check(ret, &rfds, &wfds);
#નહીંતર
    સંરચના pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    પૂર્ણાંક			nfd;
    પૂર્ણાંક			towait = (પૂર્ણાંક)msec;
    પૂર્ણાંક			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    જો (ret >= 0)
	(શૂન્ય)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    શૂન્ય
channel_stdio_loop(શૂન્ય)
{
    જ્યારે (stdio_channel != ખાલી && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	જો (due_time > 0 && due_time < msec)
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
    પૂર્ણાંક
channel_parse_messages(શૂન્ય)
{
    channel_T	*channel = first_channel;
    પૂર્ણાંક		ret = FALSE;
    પૂર્ણાંક		r;
    ch_part_T	part = PART_SOCK;
    સ્થિર પૂર્ણાંક	recursive = 0;
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
    જો ((did_repeated_સંદેશ & REPEATED_સંદેશ_LOOKING) == 0)
    {
	ch_log(ખાલી, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_સંદેશ = REPEATED_સંદેશ_LOOKING;
    }
    જ્યારે (channel != ખાલી)
    {
	જો (recursive == 1)
	{
	    જો (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_ગણતરી);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		ચાલુ;
	    }
	    જો (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		જો (channel->ch_job != ખાલી)
		    channel->ch_job->jv_channel = ખાલી;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		ચાલુ;
	    }
	    જો (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		ચાલુ;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	જો (part == PART_OUT
		&& channel->ch_job != ખાલી
		&& channel->ch_job->jv_tty_out != ખાલી
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    ચાલુ;
	}
#endif
	જો (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    જો (r == OK)
		ret = TRUE;
	    જો (channel_unref(channel) || (r == OK
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
		ચાલુ;
	    }
	}
	જો (part < PART_ERR)
	    ++part;
	નહીંતર
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    જો (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    પાછા ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    પૂર્ણાંક
channel_any_readahead(શૂન્ય)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    જ્યારે (channel != ખાલી)
    {
	જો (channel_has_readahead(channel, part))
	    પાછા TRUE;
	જો (part < PART_ERR)
	    ++part;
	નહીંતર
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    પાછા FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    પૂર્ણાંક
set_ref_in_channel(પૂર્ણાંક copyID)
{
    પૂર્ણાંક		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    માટે (channel = first_channel; !abort && channel != ખાલી;
						   channel = channel->ch_next)
	જો (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, ખાલી, ખાલી, ખાલી);
	}
    પાછા abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    સ્થિર ch_part_T
channel_part_send(channel_T *channel)
{
    જો (channel->CH_SOCK_FD == INVALID_FD)
	પાછા PART_IN;
    પાછા PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    સ્થિર ch_part_T
channel_part_read(channel_T *channel)
{
    જો (channel->CH_SOCK_FD == INVALID_FD)
	પાછા PART_OUT;
    પાછા PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    સ્થિર ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    જો (channel == ખાલી)
	પાછા CH_MODE_JSON;
    પાછા channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    સ્થિર પૂર્ણાંક
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    પાછા channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    શૂન્ય
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_સંખ્યા = 0;
    જો (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	પાછા;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    જો (channel != ખાલી)
	rettv->vval.v_સંખ્યા = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    શૂન્ય
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    જો (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	પાછા;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    જો (channel != ખાલી)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    શૂન્ય
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    જો (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	પાછા;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    જો (channel != ખાલી)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    શૂન્ય
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_સંખ્યા = -1;

    જો (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	પાછા;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    જો (channel == ખાલી)
	પાછા;

    char_u	*what = tv_get_string(&argvars[1]);
    પૂર્ણાંક		part;
    જો (STRCMP(what, "err") == 0)
	part = PART_ERR;
    નહીંતર જો (STRCMP(what, "out") == 0)
	part = PART_OUT;
    નહીંતર જો (STRCMP(what, "in") == 0)
	part = PART_IN;
    નહીંતર
	part = PART_SOCK;
    જો (channel->ch_part[part].ch_bufref.br_buf != ખાલી)
	rettv->vval.v_સંખ્યા =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    શૂન્ય
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    જો (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	પાછા;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    જો (channel == ખાલી)
	પાછા;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    જો (channel->ch_job != ખાલી)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    શૂન્ય
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    જો (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	પાછા;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    જો (channel != ખાલી && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    શૂન્ય
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    જો (check_restricted() || check_secure())
	પાછા;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    શૂન્ય
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    જો (check_restricted() || check_secure())
	પાછા;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    શૂન્ય
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    શૂન્ય
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    શૂન્ય
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    શૂન્ય
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_પદાવલિ_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    શૂન્ય
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_પદાવલિ_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    શૂન્ય
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    શૂન્ય
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    શૂન્ય
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    જો (check_restricted() || check_secure())
	પાછા;

    જો (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	પાછા;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    જો (channel == ખાલી)
	પાછા;
    clear_job_options(&opt);
    જો (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    શૂન્ય
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    પૂર્ણાંક		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ખાલી;

    જો (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	પાછા;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    જો (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	જો (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    અક્ષર      *status = channel_status(channel, -1);

    જો (channel == ખાલી)
	vim_snprintf((અક્ષર *)buf, NUMBUFLEN, "channel %s", status);
    નહીંતર
	vim_snprintf((અક્ષર *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    પાછા buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(પૂર્ણાંક ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	જો (ch->ch_id == ch_id)
	    પાછા ch;
    પાછા ખાલી;
}

#endif // FEAT_JOB_CHANNEL
