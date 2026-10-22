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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_JOB_CHANNEL)

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
प्रकारनिर्देश संरचना sockaddr_un {
    ADDRESS_FAMILY sun_family;
    वर्ण sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#अण्णहा
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) लिख(sd, buf, len)
# define sock_read(sd, buf, len) पठ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) पठ(fd, buf, len)
# define fd_write(sd, buf, len) लिख(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

स्थिर शून्य channel_read(channel_T *channel, ch_part_T part, वर्ण *func);
स्थिर शून्य channel_wait_any(long msec);
स्थिर ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
स्थिर पूर्णाङ्क channel_get_timeout(channel_T *channel, ch_part_T part);
स्थिर channel_T *channel_open_stdio(शून्य);
स्थिर ch_part_T channel_part_send(channel_T *channel);
स्थिर ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    प्रति ((ch) = first_channel; (ch) != रिक्त; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
स्थिर पूर्णाङ्क safe_to_invoke_callback = 0;

#ifdef MSWIN
    स्थिर पूर्णाङ्क
fd_read(sock_T fd, वर्ण *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    जइ (!ReadFile(h, buf, (DWORD)len, &nread, रिक्त))
	प्रत्यागम -1;
    प्रत्यागम (पूर्णाङ्क)nread;
}

    स्थिर पूर्णाङ्क
fd_write(sock_T fd, वर्ण *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    जाव (todo > 0)
    {
	जइ (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	अण्णहा
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	जइ (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    जइ (err != ERROR_IO_PENDING)
		प्रत्यागम -1;
	    जइ (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		प्रत्यागम -1;
	    FlushFileBuffers(h);
	}
	अण्णहा जइ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    भङ्ग;
	todo -= nwrite;
	done += nwrite;
    }
    प्रत्यागम (पूर्णाङ्क)done;
}

    स्थिर शून्य
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(सन्देशः) (शून्य)semsg("%s: %s", सन्देशः, strerror_win32(errno))

    स्थिर वर्ण *
strerror_win32(पूर्णाङ्क eno)
{
    स्थिर LPVOID msgbuf = रिक्त;
    char_u *ptr;

    जइ (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = रिक्त;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	रिक्त,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	रिक्त);
    जइ (msgbuf != रिक्त)
	// chomp \r or \n
	प्रति (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    विकल्प (*ptr)
	    {
		पक्ष '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    भङ्ग;
		पक्ष '\n':
		    जइ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    अण्णहा
			*ptr = ' ';
		    भङ्ग;
	    }
    प्रत्यागम msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
स्थिर channel_T *first_channel = रिक्त;
स्थिर पूर्णाङ्क next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(शून्य)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    जइ (channel == रिक्त)
	प्रत्यागम रिक्त;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    प्रति (part = PART_SOCK; part < PART_गणनम्; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_निवेशः_Handler = (Xt_निवेशः_Id)रिक्त;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_निवेशः_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    जइ (first_channel != रिक्त)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    प्रत्यागम channel;
}

    पूर्णाङ्क
has_any_channel(शून्य)
{
    प्रत्यागम first_channel != रिक्त;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    पूर्णाङ्क
channel_still_useful(channel_T *channel)
{
    पूर्णाङ्क has_sock_सन्देशः;
    पूर्णाङ्क	has_out_सन्देशः;
    पूर्णाङ्क	has_err_सन्देशः;

    // If the job was killed the channel is not expected to work anymore.
    जइ (channel->ch_job_killed && channel->ch_job == रिक्त)
	प्रत्यागम FALSE;

    // If there is a close callback it may still need to be invoked.
    जइ (channel->ch_close_cb.cb_name != रिक्त)
	प्रत्यागम TRUE;

    // If reading from or a buffer it's still useful.
    जइ (channel->ch_part[PART_IN].ch_bufref.br_buf != रिक्त)
	प्रत्यागम TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_सन्देशः = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != रिक्त
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != रिक्त;
    has_out_सन्देशः = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != रिक्त
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != रिक्त;
    has_err_सन्देशः = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != रिक्त
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != रिक्त;
    प्रत्यागम (channel->ch_callback.cb_name != रिक्त && (has_sock_सन्देशः
		|| has_out_सन्देशः || has_err_सन्देशः))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != रिक्त
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != रिक्त)
		    && has_out_सन्देशः)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != रिक्त
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != रिक्त)
		    && has_err_सन्देशः);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    पूर्णाङ्क
channel_can_close(channel_T *channel)
{
    प्रत्यागम channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    स्थिर शून्य
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    स्थिर शून्य
channel_free_channel(channel_T *channel)
{
    जइ (channel->ch_next != रिक्त)
	channel->ch_next->ch_prev = channel->ch_prev;
    जइ (channel->ch_prev == रिक्त)
	first_channel = channel->ch_next;
    अण्णहा
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    स्थिर शून्य
channel_free(channel_T *channel)
{
    जइ (in_free_unref_वस्तूनि)
	प्रत्यागम;

    जइ (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    अण्णहा
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
    स्थिर पूर्णाङ्क
channel_may_free(channel_T *channel)
{
    जइ (!channel_still_useful(channel))
    {
	channel_free(channel);
	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    पूर्णाङ्क
channel_unref(channel_T *channel)
{
    जइ (channel != रिक्त && --channel->ch_refcount <= 0)
	प्रत्यागम channel_may_free(channel);
    प्रत्यागम FALSE;
}

    पूर्णाङ्क
free_unused_channels_contents(पूर्णाङ्क copyID, पूर्णाङ्क mask)
{
    पूर्णाङ्क		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	जइ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    प्रत्यागम did_free;
}

    शून्य
free_unused_channels(पूर्णाङ्क copyID, पूर्णाङ्क mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    प्रति (ch = first_channel; ch != रिक्त; ch = ch_next)
    {
	ch_next = ch->ch_next;
	जइ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#जइ defined(FEAT_GUI)

# जइ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    स्थिर channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    जइ (fd == INVALID_FD)
	प्रत्यागम रिक्त;

    FOR_ALL_CHANNELS(channel)
    {
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	    जइ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		प्रत्यागम channel;
	    }
    }
    प्रत्यागम रिक्त;
}

    स्थिर शून्य
channel_read_fd(पूर्णाङ्क fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    जइ (channel == रिक्त)
	ch_error(रिक्त, "Channel for fd %d not found", fd);
    अण्णहा
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    स्थिर शून्य
messageFromServerX11(XtPointer clientData,
		  पूर्णाङ्क *unused1 UNUSED,
		  Xt_निवेशः_Id *unused2 UNUSED)
{
    channel_read_fd((पूर्णाङ्क)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  जइ GTK_CHECK_VERSION(3,0,0)
    स्थिर gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    प्रत्यागम TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  अण्णहा
    स्थिर शून्य
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_निवेशः_Condition unused2 UNUSED)
{
    channel_read_fd((पूर्णाङ्क)(long)clientData);
}
#  endif
# endif

    स्थिर शून्य
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    जइ (!CH_HAS_GUI)
	प्रत्यागम;

    // gets stuck in handling events for a not connected channel
    जइ (channel->ch_keep_open)
	प्रत्यागम;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    जइ (channel->ch_part[part].ch_निवेशः_Handler == (Xt_निवेशः_Id)रिक्त)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_निवेशः_Handler = Xt_App_Add_निवेशः(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_निवेशः_Read_Mask + Xt_निवेशः_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# अण्णहा
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    जइ (channel->ch_part[part].ch_निवेशः_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   जइ GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_निवेशः_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   अण्णहा
	channel->ch_part[part].ch_निवेशः_Handler = gdk_निवेशः_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_निवेशः_Condition)
			     ((पूर्णाङ्क)GDK_निवेशः_READ + (पूर्णाङ्क)GDK_निवेशः_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    स्थिर शून्य
channel_gui_register(channel_T *channel)
{
    जइ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    जइ (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    जइ (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    शून्य
channel_gui_register_all(शून्य)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    स्थिर शून्य
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    जइ (channel->ch_part[part].ch_निवेशः_Handler != (Xt_निवेशः_Id)रिक्त)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_निवेशः(channel->ch_part[part].ch_निवेशः_Handler);
	channel->ch_part[part].ch_निवेशः_Handler = (Xt_निवेशः_Id)रिक्त;
    }
# अण्णहा
#  ifdef FEAT_GUI_GTK
    जइ (channel->ch_part[part].ch_निवेशः_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   जइ GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_निवेशः_Handler);
#   अण्णहा
	gdk_निवेशः_remove(channel->ch_part[part].ch_निवेशः_Handler);
#   endif
	channel->ch_part[part].ch_निवेशः_Handler = 0;
    }
#  endif
# endif
}

    स्थिर शून्य
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    प्रति (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    स्थिर पूर्णाङ्क
channel_connect(
	channel_T *channel,
	ध्रुव संरचना sockaddr *server_addr,
	पूर्णाङ्क server_addrlen,
	पूर्णाङ्क *waittime)
{
    पूर्णाङ्क		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    जाव (TRUE)
    {
	long	elapsed_msec = 0;
	पूर्णाङ्क	waitnow;
	पूर्णाङ्क	ret;

	जइ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	जइ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    प्रत्यागम -1;
	}

	जइ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    जइ (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#अण्णहा
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		प्रत्यागम -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	जइ (ret == 0)
	    // The connection could be established.
	    भङ्ग;

	SOCK_ERRNO;
	जइ (*waittime < 0 || (errno != EWOULDBLOCK
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
	    प्रत्यागम -1;
	}
	अण्णहा जइ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    प्रत्यागम -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    संरचना timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    पूर्णाङ्क			so_error = 0;
	    socklen_t		so_error_len = परिमाण(so_error);
	    संरचना timeval	start_tv;
	    संरचना timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, रिक्त);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, रिक्त, &tv);
	    जइ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		प्रत्यागम -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    जइ (FD_ISSET(sd, &wfds))
		भङ्ग;
	    elapsed_msec = waitnow;
	    जइ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		अनुवर्त;
	    }
#अण्णहा
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    जइ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		जइ (ret < 0 || (so_error != 0
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
		    प्रत्यागम -1;
		}
		अण्णहा जइ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    प्रत्यागम -1;
		}
	    }

	    जइ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		भङ्ग;

	    gettimeofday(&end_tv, रिक्त);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	जइ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    जइ (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    जइ (!got_int)
	    {
		जइ (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		अनुवर्त;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	प्रत्यागम -1;
    }

    जइ (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#अण्णहा
	(शून्य)fcntl(sd, F_SETFL, 0);
#endif
    }

    प्रत्यागम sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ध्रुव वर्ण *path,
	शून्य (*nb_close_cb)(शून्य))
{
    channel_T		*channel = रिक्त;
    पूर्णाङ्क			sd = -1;
    size_t		path_len = STRLEN(path);
    संरचना sockaddr_un	server;
    size_t		server_len;
    पूर्णाङ्क			waittime = -1;

    जइ (*path == NUL || path_len >= परिमाण(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	प्रत्यागम रिक्त;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जइ (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	प्रत्यागम रिक्त;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, परिमाण(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(संरचना sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (संरचना sockaddr *)&server, (पूर्णाङ्क)server_len,
								   &waittime);

    जइ (sd < 0)
    {
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (वर्ण *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    प्रत्यागम channel;
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
	ध्रुव वर्ण *hostname,
	पूर्णाङ्क port,
	पूर्णाङ्क waittime,
	शून्य (*nb_close_cb)(शून्य))
{
    पूर्णाङ्क			sd = -1;
    channel_T		*channel = रिक्त;
#ifdef FEAT_IPV6
    पूर्णाङ्क			err;
    संरचना addrinfo	hints;
    संरचना addrinfo	*res = रिक्त;
    संरचना addrinfo	*addr = रिक्त;
#अण्णहा
    संरचना sockaddr_in	server;
    संरचना hostent	*host = रिक्त;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जइ (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	प्रत्यागम रिक्त;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# जइ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    जइ ((err = getaddrinfo(hostname, रिक्त, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    // Count the number of addresses for timeout distribution
    पूर्णाङ्क addr_गणनम् = 0;
    प्रति (addr = res; addr != रिक्त; addr = addr->ai_next)
	addr_गणनम्++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    जइ (waittime == 0)
	waittime = 15;

    पूर्णाङ्क addr_index = 0;
    प्रति (addr = res; addr != रिक्त; addr = addr->ai_next, addr_index++)
    {
	ध्रुव वर्ण  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ध्रुव शून्य  *src = रिक्त;
	वर्ण	    buf[NUMBUFLEN];
# endif
	पूर्णाङ्क	    try_waittime;
	पूर्णाङ्क	    before_waittime;
	पूर्णाङ्क	    consumed;
	पूर्णाङ्क	    remaining_addrs;

	जइ (addr->ai_family == AF_INET6)
	{
	    संरचना sockaddr_in6 *sai = (संरचना sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	अण्णहा जइ (addr->ai_family == AF_INET)
	{
	    संरचना sockaddr_in *sai = (संरचना sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	जइ (src != रिक्त)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, परिमाण(buf));
	    जइ (dst == रिक्त)
		dst = hostname;
	    अण्णहा जइ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	जइ (addr->ai_next == रिक्त)
	    try_waittime = waittime;
	अण्णहा जइ (addr_index == 0)
	{
	    जइ (waittime > 500)
		try_waittime = 250;
	    अण्णहा जइ (waittime > 30)
		try_waittime = waittime / 2;
	    अण्णहा
		try_waittime = waittime;
	}
	अण्णहा
	{
	    remaining_addrs = addr_गणनम् - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (पूर्णाङ्क)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	जइ (waittime < 0)
	    waittime = 0;

	जइ (sd >= 0)
	    भङ्ग;

	// If we have no time left, stop trying
	जइ (waittime <= 0 && addr->ai_next != रिक्त)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    भङ्ग;
	}
    }

    freeaddrinfo(res);
#अण्णहा
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    जइ ((host = gethostbyname(hostname)) == रिक्त)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	प्रत्यागम रिक्त;
    }
    {
	वर्ण *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], परिमाण(p));
	memcpy((वर्ण *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    जइ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (संरचना sockaddr *)&server, परिमाण(server),
								   &waittime);
#endif

    जइ (sd < 0)
    {
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (वर्ण *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    प्रत्यागम channel;
}

    स्थिर शून्य
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    जइ (callback->cb_name != रिक्त && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    अण्णहा
	cbp->cb_name = रिक्त;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	स्थिर शून्य
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_मानम्_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_मानम्_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    जइ (curbuf->b_ml.ml_mfp == रिक्त)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    स्थिर buf_T *
channel_find_buffer(char_u *name, पूर्णाङ्क err, पूर्णाङ्क सन्देशः)
{
    buf_T *buf = रिक्त;
    buf_T *save_curbuf = curbuf;

    जइ (name != रिक्त && *name != NUL)
    {
	buf = buflist_findname(name);
	जइ (buf == रिक्त)
	    buf = buflist_findname_exp(name);
    }

    जइ (buf != रिक्त)
	प्रत्यागम buf;

    buf = buflist_new(name == रिक्त || *name == NUL ? रिक्त : name,
	    रिक्त, (linenr_T)0, BLN_LISTED | BLN_NEW);
    जइ (buf == रिक्त)
	प्रत्यागम रिक्त;
    prepare_buffer(buf);

    curbuf = buf;
    जइ (सन्देशः)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    प्रत्यागम buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    स्थिर शून्य
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    जइ (opt->jo_set & JO_MODE)
	प्रति (part = PART_SOCK; part < PART_गणनम्; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    जइ (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    जइ (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    जइ (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    जइ (opt->jo_set & JO_TIMEOUT)
	प्रति (part = PART_SOCK; part < PART_गणनम्; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    जइ (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    जइ (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    जइ (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    जइ (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    जइ (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    जइ (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    जइ (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    जइ ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	जइ (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	जइ (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    जइ (buf == रिक्त)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	अण्णहा
	{
	    पूर्णाङ्क सन्देशः = TRUE;

	    जइ (opt->jo_set2 & JO2_OUT_सन्देशः)
		सन्देशः = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, सन्देशः);
	}
	जइ (buf != रिक्त)
	{
	    जइ (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    जइ (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    अण्णहा
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (वर्ण *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		जइ (buf->b_ml.ml_mfp == रिक्त)
		    prepare_buffer(buf);
	    }
	}
    }

    जइ ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	जइ (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	जइ (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	अण्णहा जइ (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    जइ (buf == रिक्त)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	अण्णहा
	{
	    पूर्णाङ्क सन्देशः = TRUE;

	    जइ (opt->jo_set2 & JO2_ERR_सन्देशः)
		सन्देशः = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, सन्देशः);
	}
	जइ (buf != रिक्त)
	{
	    जइ (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    जइ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    अण्णहा
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (वर्ण *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		जइ (buf->b_ml.ml_mfp == रिक्त)
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
    पूर्णाङ्क
channel_parse_socketserver_address(
	char_u	*address,
	पूर्णाङ्क	*port,
	char_u	**unix_path,
	bool	quiet)
{
    वर्ण    *rest;
    long    val;

    जइ (*address == NUL)
	गच्छ fail;

    जइ (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	प्रत्यागम OK;
    }

    val = strtol((वर्ण *)(address), &rest, 10);
    जइ (val < 0 || val >= 65536 || *rest != NUL)
    {
	जइ (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	प्रत्यागम FAIL;
    }
    *port = (पूर्णाङ्क)val;

    प्रत्यागम OK;
fail:
    जइ (!quiet)
	semsg(_(e_invalid_argument_str), address);
    प्रत्यागम FAIL;
}

/*
 * Implements ch_open().
 */
    स्थिर channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    वर्ण	*rest;
    पूर्णाङ्क		port = 0;
    पूर्णाङ्क		is_ipv6 = FALSE;
    पूर्णाङ्क		is_unix = FALSE;
    पूर्णाङ्क		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = रिक्त;

    जइ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम रिक्त;

    address = tv_get_string(&argvars[0]);
    जइ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	प्रत्यागम रिक्त;

    जइ (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	प्रत्यागम रिक्त;
    }

    जइ (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    अण्णहा जइ (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    अण्णहा जइ (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	जइ (p == रिक्त || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    प्रत्यागम रिक्त;
	}
    }
    अण्णहा
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	जइ (p == रिक्त)
	{
	    semsg(_(e_invalid_argument_str), address);
	    प्रत्यागम रिक्त;
	}
    }

    जइ (!is_unix && !is_stdio)
    {
	port = strtol((वर्ण *)(p + 1), &rest, 10);
	जइ (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    प्रत्यागम रिक्त;
	}
	जइ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	अण्णहा
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    जइ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	गच्छ theend;
    जइ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	गच्छ theend;
    }

    जइ (is_stdio)
	channel = channel_open_stdio();
    अण्णहा जइ (is_unix)
	channel = channel_open_unix((वर्ण *)address, रिक्त);
    अण्णहा
	channel = channel_open((वर्ण *)address, port, opt.jo_waittime, रिक्त);
    जइ (channel != रिक्त)
    {
	पूर्णाङ्क	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	जइ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    प्रत्यागम channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
स्थिर channel_T *stdio_channel = रिक्त;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    स्थिर channel_T *
channel_open_stdio(शून्य)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    जइ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	प्रत्यागम रिक्त;
    }
    जइ (stdio_channel != रिक्त)
    {
	emsg(_(e_cannot_open_stdio_channel));
	प्रत्यागम रिक्त;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	पूर्णाङ्क	nul_fd;

	जइ (!DuplicateHandle(proc, GetStdHandle(STD_निवेशः_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    प्रत्यागम रिक्त;
	}
	जइ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    प्रत्यागम रिक्त;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, रिक्त, OPEN_EXISTING, 0,
		रिक्त);
	जइ (hnul != INVALID_HANDLE_मानम्)
	{
	    SetStdHandle(STD_निवेशः_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	जइ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#अण्णहा
    {
	पूर्णाङ्क	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	जइ (in_fd < 0 || out_fd < 0)
	{
	    जइ (in_fd >= 0)
		close(in_fd);
	    जइ (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    प्रत्यागम रिक्त;
	}
	(शून्य)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(शून्य)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	जइ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    जइ (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	प्रत्यागम रिक्त;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    प्रत्यागम channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    वर्ण	*rest;
    पूर्णाङ्क		port;
    पूर्णाङ्क		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = रिक्त;

    जइ (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	प्रत्यागम रिक्त;

    arg = tv_get_string(&argvars[0]);
    जइ (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	प्रत्यागम रिक्त;
    }

    जइ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    अण्णहा
    {
	port = strtol((वर्ण *)(arg), &rest, 10);
	जइ (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    प्रत्यागम रिक्त;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    जइ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	गच्छ theend;
    जइ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	गच्छ theend;
    }

    जइ (is_unix)
	channel = channel_listen_unix((वर्ण *)arg, रिक्त, true);
    अण्णहा
	channel = channel_listen(port, रिक्त);
    जइ (channel != रिक्त)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    प्रत्यागम channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	पूर्णाङ्क port_in,
	शून्य (*nb_close_cb)(शून्य))
{
    पूर्णाङ्क			sd = -1;
    संरचना sockaddr_in	server;
#ifndef FEAT_IPV6
    संरचना hostent	*host;
#endif
    पूर्णाङ्क			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जइ (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	प्रत्यागम रिक्त;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((वर्ण *)&server, 0, परिमाण(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    जइ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

#ifdef MSWIN
    जइ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ध्रुव वर्ण *)&val, परिमाण(val)) < 0)
#अण्णहा
    जइ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, परिमाण(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    // Bind the socket to the port
    जइ (bind(sd, (संरचना sockaddr *)&server, परिमाण(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    जइ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    जइ (port_in == 0)
    {
	संरचना sockaddr_in	addr;
	socklen_t		addr_len = परिमाण(addr);

	जइ (getsockname(sd, (संरचना sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (वर्ण *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    प्रत्यागम channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	वर्ण *path,
	शून्य (*nb_close_cb)(शून्य),
	bool replace)
{
    पूर्णाङ्क			sd = -1;
    संरचना sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    जइ (path == रिक्त || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == रिक्त ? (वर्ण *)"" : path);
	प्रत्यागम रिक्त;
    }

    path_len = STRLEN(path);
    जइ (path_len >= परिमाण(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	प्रत्यागम रिक्त;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जइ (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	प्रत्यागम रिक्त;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, परिमाण(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    जइ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    जइ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(संरचना sockaddr_un, sun_path) + path_len + 1;
    जइ (bind(sd, (संरचना sockaddr *)&server, (पूर्णाङ्क)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    जइ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	प्रत्यागम रिक्त;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (वर्ण *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    प्रत्यागम channel;
}

     शून्य
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    जइ (*fd == INVALID_FD)
	प्रत्यागम;

    जइ (part == PART_SOCK)
	sock_close(*fd);
    अण्णहा
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	जइ ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    जइ (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    शून्य
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    जइ (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#जइ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	जइ (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    जइ (out != INVALID_FD)
    {
#जइ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#जइ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    जइ (err != INVALID_FD)
    {
#जइ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#जइ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    शून्य
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    जइ (job->jv_in_buf == रिक्त)
	प्रत्यागम;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (वर्ण *)in_part->ch_bufref.br_buf->b_ffname);
    जइ (options->jo_set & JO_IN_शिखरम्)
    {
	जइ (options->jo_in_शिखरम् == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_शिखरम् =
		in_part->ch_bufref.br_buf->b_ml.ml_पङ्क्तिः_गणनम् + 1;
	}
	अण्णहा
	    in_part->ch_buf_शिखरम् = options->jo_in_शिखरम्;
    }
    अण्णहा
	in_part->ch_buf_शिखरम् = 1;
    जइ (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    अण्णहा
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_पङ्क्तिः_गणनम्;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    स्थिर शून्य
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	पूर्णाङ्क	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    जइ (item == रिक्त)
	प्रत्यागम;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = रिक्त;
    जइ (item->cq_prev == रिक्त)
	head->cq_next = item;
    अण्णहा
	item->cq_prev->cq_next = item;
}

    स्थिर शून्य
write_buf_पङ्क्तिः(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *पङ्क्तिः = ml_get_buf(buf, lnum, FALSE);
    पूर्णाङ्क	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    पूर्णाङ्क	    i;

    // Need to make a copy to be able to append a NL.
    जइ ((p = alloc(len + 2)) == रिक्त)
	प्रत्यागम;
    memcpy((वर्ण *)p, (वर्ण *)पङ्क्तिः, len);

    जइ (channel->ch_write_text_mode)
	p[len] = CAR;
    अण्णहा
    {
	प्रति (i = 0; i < len; ++i)
	    जइ (p[i] == NL)
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
    स्थिर पूर्णाङ्क
can_write_buf_पङ्क्तिः(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    जइ (in_part->ch_fd == INVALID_FD)
	प्रत्यागम FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    जइ (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    अण्णहा जइ (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# जइ defined(HAVE_SELECT)
	संरचना timeval	tval;
	fd_set		wfds;
	पूर्णाङ्क		ret;

	FD_ZERO(&wfds);
	FD_SET((पूर्णाङ्क)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	प्रति (;;)
	{
	    ret = select((पूर्णाङ्क)in_part->ch_fd + 1, रिक्त, &wfds, रिक्त, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    जइ (ret == -1 && errno == EINTR)
		अनुवर्त;
#  endif
	    जइ (ret <= 0 || in_part->ch_block_write == 1)
	    {
		जइ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		अण्णहा
		    ch_log(channel, "Input not ready for writing");
		प्रत्यागम FALSE;
	    }
	    भङ्ग;
	}
# अण्णहा
	संरचना pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	जइ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    प्रत्यागम FALSE;
	}
	जइ (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    प्रत्यागम FALSE;
	}
# endif
    }
#endif
    प्रत्यागम TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    शून्य
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    पूर्णाङ्क		written = 0;

    जइ (buf == रिक्त || in_part->ch_buf_append)
	प्रत्यागम;  // no buffer or using appending
    जइ (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == रिक्त)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = रिक्त;
	प्रत्यागम;
    }

    प्रति (lnum = in_part->ch_buf_शिखरम्; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_पङ्क्तिः_गणनम्; ++lnum)
    {
	जइ (!can_write_buf_पङ्क्तिः(channel))
	    भङ्ग;
	write_buf_पङ्क्तिः(buf, lnum, channel);
	++written;
    }

    जइ (written == 1)
	ch_log(channel, "written line %d to channel", (पूर्णाङ्क)lnum - 1);
    अण्णहा जइ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_शिखरम् = lnum;
    जइ (lnum > buf->b_ml.ml_पङ्क्तिः_गणनम् || lnum > in_part->ch_buf_bot)
    {
#जइ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	जइ (channel->ch_job != रिक्त)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = रिक्त;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	जइ (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    अण्णहा
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_पङ्क्तिः_गणनम् - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    शून्य
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	प्रति (part = PART_SOCK; part < PART_गणनम्; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    जइ (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = रिक्त;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    शून्य
channel_write_निवेशः(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    जइ (in_part->ch_writeque.wq_next != रिक्त)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    अण्णहा जइ (in_part->ch_bufref.br_buf != रिक्त)
    {
	जइ (in_part->ch_buf_append)
	    channel_write_new_पङ्क्तयः(in_part->ch_bufref.br_buf);
	अण्णहा
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    शून्य
channel_write_any_पङ्क्तयः(शून्य)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_निवेशः(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    शून्य
channel_write_new_पङ्क्तयः(buf_T *buf)
{
    channel_T	*channel;
    पूर्णाङ्क		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	पूर्णाङ्क	    written = 0;

	जइ (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    जइ (in_part->ch_fd == INVALID_FD)
		अनुवर्त;  // pipe was closed
	    found_one = TRUE;
	    प्रति (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_पङ्क्तिः_गणनम्;
								       ++lnum)
	    {
		जइ (!can_write_buf_पङ्क्तिः(channel))
		    भङ्ग;
		write_buf_पङ्क्तिः(buf, lnum, channel);
		++written;
	    }

	    जइ (written == 1)
		ch_log(channel, "written line %d to channel", (पूर्णाङ्क)lnum - 1);
	    अण्णहा जइ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    जइ (lnum < buf->b_ml.ml_पङ्क्तिः_गणनम्)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_पङ्क्तिः_गणनम् - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    जइ (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    स्थिर शून्य
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    जइ (safe_to_invoke_callback == 0)
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

    प्रत्यागम head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *ग्रन्थिः)
{
    char_u  *buffer = ग्रन्थिः->rq_buffer;
    long_u  i;

    प्रति (i = 0; i < ग्रन्थिः->rq_buflen; ++i)
	जइ (buffer[i] == NL)
	    प्रत्यागम buffer + i;
    प्रत्यागम रिक्त;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, पूर्णाङ्क *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ग्रन्थिः = head->rq_next;
    char_u *p;

    जइ (ग्रन्थिः == रिक्त)
	प्रत्यागम रिक्त;
    जइ (outlen != रिक्त)
	*outlen += ग्रन्थिः->rq_buflen;
    // dispose of the node but keep the buffer
    p = ग्रन्थिः->rq_buffer;
    head->rq_next = ग्रन्थिः->rq_next;
    जइ (ग्रन्थिः->rq_next == रिक्त)
	head->rq_prev = रिक्त;
    अण्णहा
	ग्रन्थिः->rq_next->rq_prev = रिक्त;
    vim_free(ग्रन्थिः);
    प्रत्यागम p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    स्थिर char_u *
channel_get_all(channel_T *channel, ch_part_T part, पूर्णाङ्क *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ग्रन्थिः;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    प्रति (ग्रन्थिः = head->rq_next; ग्रन्थिः != रिक्त; ग्रन्थिः = ग्रन्थिः->rq_next)
	len += ग्रन्थिः->rq_buflen;
    res = alloc(len + 1);
    जइ (res == रिक्त)
	प्रत्यागम रिक्त;
    p = res;
    प्रति (ग्रन्थिः = head->rq_next; ग्रन्थिः != रिक्त; ग्रन्थिः = ग्रन्थिः->rq_next)
    {
	mch_memmove(p, ग्रन्थिः->rq_buffer, ग्रन्थिः->rq_buflen);
	p += ग्रन्थिः->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    करेहि
    {
	p = channel_get(channel, part, रिक्त);
	vim_free(p);
    } जाव (p != रिक्त);

    जइ (outlen != रिक्त)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	प्रत्यागम res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    जाव (p < res + len)
    {
	जइ (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	अण्णहा जइ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    जइ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		जाव (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		जइ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    प्रत्यागम res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    शून्य
channel_consume(channel_T *channel, ch_part_T part, पूर्णाङ्क len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ग्रन्थिः = head->rq_next;
    char_u *buf = ग्रन्थिः->rq_buffer;

    जइ (len < 0 || (long_u)len > ग्रन्थिः->rq_buflen)
	प्रत्यागम;
    mch_memmove(buf, buf + len, ग्रन्थिः->rq_buflen - len);
    ग्रन्थिः->rq_buflen -= len;
    ग्रन्थिः->rq_buffer[ग्रन्थिः->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    पूर्णाङ्क
channel_collapse(channel_T *channel, ch_part_T part, पूर्णाङ्क want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*ग्रन्थिः = head->rq_next;
    readq_T	*last_ग्रन्थिः;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    जइ (ग्रन्थिः == रिक्त || ग्रन्थिः->rq_next == रिक्त)
	प्रत्यागम FAIL;

    last_ग्रन्थिः = ग्रन्थिः->rq_next;
    len = ग्रन्थिः->rq_buflen + last_ग्रन्थिः->rq_buflen;
    जइ (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	जाव (last_ग्रन्थिः->rq_next != रिक्त
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_ग्रन्थिः) == रिक्त))
	{
	    last_ग्रन्थिः = last_ग्रन्थिः->rq_next;
	    len += last_ग्रन्थिः->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    जइ (newbuf == रिक्त)
	प्रत्यागम FAIL;	    // out of memory
    mch_memmove(p, ग्रन्थिः->rq_buffer, ग्रन्थिः->rq_buflen);
    p += ग्रन्थिः->rq_buflen;
    vim_free(ग्रन्थिः->rq_buffer);
    ग्रन्थिः->rq_buffer = newbuf;
    प्रति (n = ग्रन्थिः; n != last_ग्रन्थिः; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    ग्रन्थिः->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    प्रति (n = ग्रन्थिः->rq_next; n != last_ग्रन्थिः; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    ग्रन्थिः->rq_next = last_ग्रन्थिः->rq_next;
    जइ (last_ग्रन्थिः->rq_next == रिक्त)
	head->rq_prev = ग्रन्थिः;
    अण्णहा
	last_ग्रन्थिः->rq_next->rq_prev = ग्रन्थिः;
    vim_free(last_ग्रन्थिः);
    प्रत्यागम OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
channel_save(channel_T *channel, ch_part_T part, char_u *buf, पूर्णाङ्क len,
						      पूर्णाङ्क prepend, वर्ण *lead)
{
    readq_T *ग्रन्थिः;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    पूर्णाङ्क	    i;

    ग्रन्थिः = ALLOC_ONE(readq_T);
    जइ (ग्रन्थिः == रिक्त)
	प्रत्यागम FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    ग्रन्थिः->rq_buffer = alloc(len + 1);
    जइ (ग्रन्थिः->rq_buffer == रिक्त)
    {
	vim_free(ग्रन्थिः);
	प्रत्यागम FAIL;	    // out of memory
    }

    जइ (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = ग्रन्थिः->rq_buffer;
	प्रति (i = 0; i < len; ++i)
	    जइ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	ग्रन्थिः->rq_buflen = (long_u)(p - ग्रन्थिः->rq_buffer);
    }
    अण्णहा
    {
	mch_memmove(ग्रन्थिः->rq_buffer, buf, len);
	ग्रन्थिः->rq_buffer[len] = NUL;
	ग्रन्थिः->rq_buflen = (long_u)len;
    }

    जइ (prepend)
    {
	// prepend node to the head of the queue
	ग्रन्थिः->rq_next = head->rq_next;
	ग्रन्थिः->rq_prev = रिक्त;
	जइ (head->rq_next == रिक्त)
	    head->rq_prev = ग्रन्थिः;
	अण्णहा
	    head->rq_next->rq_prev = ग्रन्थिः;
	head->rq_next = ग्रन्थिः;
    }
    अण्णहा
    {
	// append node to the tail of the queue
	ग्रन्थिः->rq_next = रिक्त;
	ग्रन्थिः->rq_prev = head->rq_prev;
	जइ (head->rq_prev == रिक्त)
	    head->rq_next = ग्रन्थिः;
	अण्णहा
	    head->rq_prev->rq_next = ग्रन्थिः;
	head->rq_prev = ग्रन्थिः;
    }

    जइ (ch_log_active() && lead != रिक्त)
	ch_log_literal(lead, channel, part, buf, len);

    प्रत्यागम OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    पूर्णाङ्क
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, रिक्त);
    पूर्णाङ्क		keeplen;
    पूर्णाङ्क		addlen;
    char_u	*p;

    जइ (next == रिक्त)
	प्रत्यागम FALSE;

    keeplen = reader->js_end - reader->js_buf;
    जइ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (पूर्णाङ्क)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	जइ (p == रिक्त)
	{
	    vim_free(next);
	    प्रत्यागम FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    प्रत्यागम TRUE;
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
    स्थिर पूर्णाङ्क
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*पङ्क्तिः_start;
    char_u	*p;
    int_u	hdr_len;
    पूर्णाङ्क		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    जाव (TRUE)
    {
	पङ्क्तिः_start = p;
	जाव (*p != NUL && *p != '\n')
	    p++;
	जइ (*p == NUL)			// partial header
	    प्रत्यागम MAYBE;
	p++;

	// process the content length field (if present)
	जइ ((p - पङ्क्तिः_start > 16)
		&& STRNICMP(पङ्क्तिः_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((वर्ण *)पङ्क्तिः_start + 16, रिक्त, 10);
	    जइ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		प्रत्यागम FAIL;
	}

	जइ ((p - पङ्क्तिः_start) == 2 && पङ्क्तिः_start[0] == '\r' &&
		पङ्क्तिः_start[1] == '\n')
	    // reached the empty line
	    भङ्ग;
    }

    जइ (payload_len == -1)
	// Content-Length field is not present in the header
	प्रत्यागम FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    जइ (jsbuf_len < hdr_len + payload_len)
	प्रत्यागम MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    प्रत्यागम OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    पूर्णाङ्क
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    पूर्णाङ्क		status = OK;
    पूर्णाङ्क		ret;

    जइ (channel_peek(channel, part) == रिक्त)
	प्रत्यागम FALSE;

    reader.js_buf = channel_get(channel, part, रिक्त);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    जइ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    जइ (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    जइ (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	जइ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	अण्णहा जइ (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    जइ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    अण्णहा
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	अण्णहा
	{
	    item = ALLOC_ONE(jsonq_T);
	    जइ (item == रिक्त)
		clear_tv(&listtv);
	    अण्णहा
	    {
		item->jq_no_callback = FALSE;
		item->jq_मानम् = alloc_tv();
		जइ (item->jq_मानम् == रिक्त)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		अण्णहा
		{
		    *item->jq_मानम् = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = रिक्त;
		    जइ (item->jq_prev == रिक्त)
			head->jq_next = item;
		    अण्णहा
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    जइ (status == OK)
	chanpart->ch_wait_len = 0;
    अण्णहा जइ (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	जइ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (पूर्णाङ्क)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_गणनम्() + 100L;
#अण्णहा
	    gettimeofday(&chanpart->ch_deadline, रिक्त);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    जइ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	अण्णहा
	{
	    पूर्णाङ्क timeout;
#ifdef MSWIN
	    timeout = (पूर्णाङ्क)(Get_Tick_गणनम्() - chanpart->ch_deadline) > 0;
#अण्णहा
	    {
		संरचना timeval now_tv;

		gettimeofday(&now_tv, रिक्त);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    जइ (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    अण्णहा
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    जइ (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    अण्णहा जइ (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(पूर्णाङ्क)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, रिक्त);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    अण्णहा
	ret = FALSE;

    vim_free(reader.js_buf);
    प्रत्यागम ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    स्थिर शून्य
remove_cb_ग्रन्थिः(cbq_T *head, cbq_T *ग्रन्थिः)
{
    जइ (ग्रन्थिः->cq_prev == रिक्त)
	head->cq_next = ग्रन्थिः->cq_next;
    अण्णहा
	ग्रन्थिः->cq_prev->cq_next = ग्रन्थिः->cq_next;
    जइ (ग्रन्थिः->cq_next == रिक्त)
	head->cq_prev = ग्रन्थिः->cq_prev;
    अण्णहा
	ग्रन्थिः->cq_next->cq_prev = ग्रन्थिः->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    शून्य
remove_json_ग्रन्थिः(jsonq_T *head, jsonq_T *ग्रन्थिः)
{
    जइ (ग्रन्थिः->jq_prev == रिक्त)
	head->jq_next = ग्रन्थिः->jq_next;
    अण्णहा
	ग्रन्थिः->jq_prev->jq_next = ग्रन्थिः->jq_next;
    जइ (ग्रन्थिः->jq_next == रिक्त)
	head->jq_prev = ग्रन्थिः->jq_prev;
    अण्णहा
	ग्रन्थिः->jq_next->jq_prev = ग्रन्थिः->jq_prev;
    vim_free(ग्रन्थिः);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    स्थिर शून्य
channel_add_block_id(chanpart_T *chanpart, पूर्णाङ्क id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    जइ (gap->ga_growsize == 0)
	ga_init2(gap, परिमाण(पूर्णाङ्क), 10);
    जइ (ga_grow(gap, 1) == OK)
    {
	((पूर्णाङ्क *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    स्थिर शून्य
channel_remove_block_id(chanpart_T *chanpart, पूर्णाङ्क id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < gap->ga_len; ++i)
	जइ (((पूर्णाङ्क *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    जइ (i < gap->ga_len)
	    {
		पूर्णाङ्क *p = ((पूर्णाङ्क *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * परिमाण(पूर्णाङ्क));
	    }
	    प्रत्यागम;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    स्थिर पूर्णाङ्क
channel_has_block_id(chanpart_T *chanpart, पूर्णाङ्क id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    पूर्णाङ्क		i;

    प्रति (i = 0; i < gap->ga_len; ++i)
	जइ (((पूर्णाङ्क *)gap->ga_data)[i] == id)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
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
    स्थिर पूर्णाङ्क
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	पूर्णाङ्क	    id,
	पूर्णाङ्क	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    जाव (item != रिक्त)
    {
	list_T	    *l;
	typval_T    *tv;

	जइ (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_मानम्->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	अण्णहा
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    जइ (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		जइ (id > 0)
		{
		    जइ (item->jq_मानम्->v_type != VAR_DICT)
			गच्छ nextitem;
		    d = item->jq_मानम्->vval.v_dict;
		    जइ (d == रिक्त)
			गच्छ nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    जइ (dict_has_कुञ्चिका(d, "method"))
			गच्छ nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    जइ (di == रिक्त)
			गच्छ nextitem;
		    tv = &di->di_tv;
		}
		अण्णहा
		    tv = item->jq_मानम्;
	    }
	    अण्णहा
	    {
		जइ (id > 0)
		{
		    जइ (item->jq_मानम्->v_type != VAR_DICT)
			गच्छ nextitem;
		    d = item->jq_मानम्->vval.v_dict;
		    जइ (d == रिक्त)
			गच्छ nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    जइ (di == रिक्त)
			गच्छ nextitem;
		    tv = &di->di_tv;
		}
		अण्णहा
		    tv = item->jq_मानम्;
	    }
	}

	जइ ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_सङ्ख्या && tv->vval.v_सङ्ख्या == id)
	      || (id <= 0 && (tv->v_type != VAR_सङ्ख्या
		 || tv->vval.v_सङ्ख्या == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_सङ्ख्या)))))
	{
	    *rettv = item->jq_मानम्;
	    जइ (tv->v_type == VAR_सङ्ख्या)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_सङ्ख्या);
	    remove_json_ग्रन्थिः(head, item);
	    प्रत्यागम OK;
	}
nextitem:
	item = item->jq_next;
    }
    प्रत्यागम FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    स्थिर शून्य
channel_प्रक्षिप_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    जइ (head->jq_prev != रिक्त && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = रिक्त;
    अण्णहा जाव (item != रिक्त && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    जइ (newitem == रिक्त)
    {
	clear_tv(rettv);
	प्रत्यागम;
    }

    newitem->jq_मानम् = alloc_tv();
    जइ (newitem->jq_मानम् == रिक्त)
    {
	vim_free(newitem);
	clear_tv(rettv);
	प्रत्यागम;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_मानम् = *rettv;
    जइ (item == रिक्त)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = रिक्त;
	जइ (newitem->jq_prev == रिक्त)
	    head->jq_next = newitem;
	अण्णहा
	    newitem->jq_prev->jq_next = newitem;
    }
    अण्णहा
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	जइ (newitem->jq_next == रिक्त)
	    head->jq_prev = newitem;
	अण्णहा
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    स्थिर शून्य
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    पूर्णाङ्क	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    जइ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	जइ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	प्रत्यागम;
    }
    arg = argv[1].vval.v_string;
    जइ (arg == रिक्त)
	arg = (char_u *)"";

    जइ (STRCMP(cmd, "ex") == 0)
    {
	पूर्णाङ्क	called_emsg_before = called_emsg;
	char_u	*p = arg;
	पूर्णाङ्क	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (वर्ण *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	जइ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	जइ (do_emsg_silent)
	    --emsg_silent;
	जइ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (वर्ण *)get_vim_var_str(VV_ERRMSG));
    }
    अण्णहा जइ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (वर्ण *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_गणनम् = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    अण्णहा जइ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    अण्णहा जइ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	पूर्णाङ्क is_call = cmd[0] == 'c';
	पूर्णाङ्क id_idx = is_call ? 3 : 2;

	जइ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_सङ्ख्या)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    जइ (p_verbose > 2)
		emsg(_(e_last_argument_for_व्यञ्जकम्_call_must_be_सङ्ख्या));
	}
	अण्णहा जइ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    जइ (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	अण्णहा
	{
	    typval_T	*tv = रिक्त;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = रिक्त;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    जइ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (वर्ण *)arg);
		tv = eval_व्यञ्जकम्(arg, रिक्त);
	    }
	    अण्णहा
	    {
		ch_log(channel, "Calling '%s'", (वर्ण *)arg);
		जइ (func_call(arg, &argv[2], रिक्त, रिक्त, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    जइ (argv[id_idx].v_type == VAR_सङ्ख्या)
	    {
		पूर्णाङ्क id = argv[id_idx].vval.v_सङ्ख्या;

		जइ (tv != रिक्त)
		    json = json_encode_nr_व्यञ्जकम्(id, tv, options | JSON_NL);
		जइ (tv == रिक्त || (json != रिक्त && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_व्यञ्जकम्(id, &err_tv, options | JSON_NL);
		}
		जइ (json != रिक्त)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (पूर्णाङ्क)STRLEN(json), (वर्ण *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    जइ (tv == &res_tv)
		clear_tv(tv);
	    अण्णहा
		free_tv(tv);
	}
    }
    अण्णहा जइ (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (वर्ण *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    स्थिर शून्य
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (वर्ण *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_ग्रन्थिः(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    स्थिर शून्य
append_to_buffer(
    buf_T	*buffer,
    char_u	*सन्देशः,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_पङ्क्तिः_गणनम्;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    पूर्णाङ्क		save_p_ma = buffer->b_p_ma;
    पूर्णाङ्क		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    जइ (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	जइ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	प्रत्यागम;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    जइ (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (पूर्णाङ्क)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    जइ (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	प्रत्यागम;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    जइ (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, सन्देशः, TRUE);
	lnum = 0;
    }
    अण्णहा
	ml_append(lnum, सन्देशः, 0, FALSE);
    appended_पङ्क्तयः_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    जइ (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    अण्णहा
	buffer->b_p_ma = save_p_ma;

    जइ (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    जइ (wp->w_buffer == buffer)
	    {
		पूर्णाङ्क move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		जइ (move_cursor || wp->w_topline > buffer->b_ml.ml_पङ्क्तिः_गणनम्)
		{
		    win_T *save_curwin = curwin;

		    जइ (move_cursor)
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

    जइ (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    जइ (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_पङ्क्तिः_गणनम्;
	}
    }
}

    स्थिर शून्य
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *सन्देशः;

    जाव ((सन्देशः = channel_get(channel, part, रिक्त)) != रिक्त)
    {
	ch_log(channel, "Dropping message '%s'", (वर्ण *)सन्देशः);
	vim_free(सन्देशः);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    स्थिर पूर्णाङ्क
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    प्रत्यागम ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    स्थिर पूर्णाङ्क
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*सन्देशः = रिक्त;
    blob_T	*blob = रिक्त;
    typval_T	*listtv = रिक्त;
    typval_T	argv[CH_JSON_MAX_ARGS];
    पूर्णाङ्क		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = रिक्त;
    buf_T	*buffer = रिक्त;
    char_u	*p;
    पूर्णाङ्क		called_otc;		// one time callbackup
    पूर्णाङ्क		raw_len = 0;

    जइ (channel->ch_nb_close_cb != रिक्त
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	प्रत्यागम FALSE;

    // Use a message-specific callback, part callback or channel callback
    प्रति (cbitem = cbhead->cq_next; cbitem != रिक्त; cbitem = cbitem->cq_next)
	जइ (cbitem->cq_seq_nr == 0)
	    भङ्ग;
    जइ (cbitem != रिक्त)
	callback = &cbitem->cq_callback;
    अण्णहा जइ (ch_part->ch_callback.cb_name != रिक्त)
	callback = &ch_part->ch_callback;
    अण्णहा जइ (channel->ch_callback.cb_name != रिक्त)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    जइ (buffer != रिक्त && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == रिक्त))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = रिक्त;
	buffer = रिक्त;
    }

    जइ (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	पूर्णाङ्क		argc = 0;

	// Get any json message in the queue.
	जइ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    जइ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(शून्य)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    जइ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		प्रत्यागम FALSE;
	}

	जइ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    जइ (d != रिक्त)
	    {
		जइ (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		अण्णहा
		    di = dict_find(d, (char_u *)"request_seq", -1);
		जइ (di != रिक्त && di->di_tv.v_type == VAR_सङ्ख्या)
		    seq_nr = di->di_tv.vval.v_सङ्ख्या;
	    }

	    argv[1] = *listtv;
	}
	अण्णहा
	{
	    प्रति (item = listtv->vval.v_list->lv_first;
		    item != रिक्त && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    जाव (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    जइ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		प्रत्यागम TRUE;
	    }

	    जइ (argv[0].v_type != VAR_सङ्ख्या)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		प्रत्यागम FALSE;
	    }
	    seq_nr = argv[0].vval.v_सङ्ख्या;
	}
    }
    अण्णहा जइ (channel_peek(channel, part) == रिक्त)
    {
	// nothing to read on RAW or NL channel
	प्रत्यागम FALSE;
    }
    अण्णहा
    {
	// If there is no callback or buffer drop the message.
	जइ (callback == रिक्त && buffer == रिक्त)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    जइ (channel->ch_close_cb.cb_name == रिक्त && !channel->ch_drop_never)
		drop_messages(channel, part);
	    प्रत्यागम FALSE;
	}

	जइ (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = रिक्त;
	    char_u  *buf;
	    readq_T *ग्रन्थिः;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    जाव (TRUE)
	    {
		ग्रन्थिः = channel_peek(channel, part);
		nl = channel_first_nl(ग्रन्थिः);
		जइ (nl != रिक्त)
		    भङ्ग;
		जइ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    जइ (ch_part->ch_fd == INVALID_FD && ग्रन्थिः->rq_buflen > 0)
			भङ्ग;
		    प्रत्यागम FALSE; // incomplete message
		}
	    }
	    buf = ग्रन्थिः->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    प्रति (p = buf; (nl == रिक्त || p < nl)
					    && p < buf + ग्रन्थिः->rq_buflen; ++p)
		जइ (*p == NUL)
		    *p = NL;

	    जइ (nl == रिक्त)
	    {
		// get the whole buffer, drop the NL
		सन्देशः = channel_get(channel, part, रिक्त);
	    }
	    अण्णहा जइ (nl + 1 == buf + ग्रन्थिः->rq_buflen)
	    {
		// get the whole buffer
		सन्देशः = channel_get(channel, part, रिक्त);
		*nl = NUL;
	    }
	    अण्णहा
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		सन्देशः = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (पूर्णाङ्क)(nl - buf) + 1);
	    }
	}
	अण्णहा
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    सन्देशः = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : रिक्त);
	}

	जइ (सन्देशः == रिक्त)
	    प्रत्यागम FALSE; // out of memory (and avoids Coverity warning)

	जइ (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    जइ (blob == रिक्त)
	    {
		vim_free(सन्देशः);
		प्रत्यागम FALSE;
	    }
	    जइ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(सन्देशः);
		प्रत्यागम FALSE;
	    }
	    जइ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, सन्देशः, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	अण्णहा
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = सन्देशः;
	}
    }

    called_otc = FALSE;
    जइ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	पूर्णाङ्क lsp_req_सन्देशः = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	जइ ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_कुञ्चिका(argv[1].vval.v_dict, "method"))
	    lsp_req_सन्देशः = TRUE;

	जइ (!lsp_req_सन्देशः)
	{
	    प्रति (cbitem = cbhead->cq_next; cbitem != रिक्त;
		    cbitem = cbitem->cq_next)
	    {
		जइ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    भङ्ग;
		}
	    }
	}
    }

    जइ (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	जइ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    जइ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_प्रक्षिप_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_सङ्ख्या;
		free_tv(listtv);
		listtv = रिक्त;
	    }
	    अण्णहा
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    अण्णहा जइ (callback != रिक्त || buffer != रिक्त)
    {
	जइ (buffer != रिक्त)
	{
	    जइ (सन्देशः == रिक्त)
		// JSON or JS mode: re-encode the message.
		सन्देशः = json_encode(listtv, ch_mode);
	    जइ (सन्देशः != रिक्त)
	    {
#ifdef FEAT_TERMINAL
		जइ (buffer->b_term != रिक्त)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    जइ (part == PART_ERR)
		    {
			char_u *cp;
			पूर्णाङ्क    extra = 0;

			प्रति (cp = सन्देशः; *cp != NUL; ++cp)
			    जइ (*cp == NL && (cp == सन्देशः || cp[-1] != CAR))
				++extra;
			जइ (extra > 0)
			{
			    char_u *crlf_सन्देशः = alloc(STRLEN(सन्देशः) + extra + 1);

			    जइ (crlf_सन्देशः != रिक्त)
			    {
				char_u *q = crlf_सन्देशः;

				प्रति (cp = सन्देशः; *cp != NUL; ++cp)
				{
				    जइ (*cp == NL && (cp == सन्देशः || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_सन्देशः, channel);
				vim_free(crlf_सन्देशः);
			    }
			}
			अण्णहा
			    write_to_term(buffer, सन्देशः, channel);
		    }
		    अण्णहा
			write_to_term(buffer, सन्देशः, channel);
		}
		अण्णहा
#endif
		    append_to_buffer(buffer, सन्देशः, channel, part);
	    }
	}

	जइ (callback != रिक्त)
	{
	    जइ (cbitem != रिक्त)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    अण्णहा
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (वर्ण *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    अण्णहा
	ch_log(channel, "Dropping message %d", seq_nr);

    जइ (listtv != रिक्त)
	free_tv(listtv);
    जइ (blob != रिक्त)
	blob_unref(blob);
    vim_free(सन्देशः);

    प्रत्यागम TRUE;
}

#जइ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    पूर्णाङ्क
channel_can_write_to(channel_T *channel)
{
    प्रत्यागम channel != रिक्त && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    पूर्णाङ्क
channel_is_open(channel_T *channel)
{
    प्रत्यागम channel != रिक्त && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    स्थिर शून्य *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    जइ (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	जइ (head->jq_next == रिक्त)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	प्रत्यागम head->jq_next;
    }
    प्रत्यागम channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    स्थिर पूर्णाङ्क
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    प्रत्यागम channel_readahead_pointer(channel, part) != रिक्त;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    स्थिर वर्ण *
channel_status(channel_T *channel, पूर्णाङ्क req_part)
{
    ch_part_T part;
    पूर्णाङ्क has_readahead = FALSE;

    जइ (channel == रिक्त)
	 प्रत्यागम "fail";
    जइ (req_part == PART_OUT)
    {
	जइ (channel->CH_OUT_FD != INVALID_FD)
	    प्रत्यागम "open";
	जइ (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    अण्णहा जइ (req_part == PART_ERR)
    {
	जइ (channel->CH_ERR_FD != INVALID_FD)
	    प्रत्यागम "open";
	जइ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    अण्णहा
    {
	जइ (channel_is_open(channel))
	    प्रत्यागम "open";
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	    जइ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		भङ्ग;
	    }
    }

    जइ (has_readahead)
	प्रत्यागम "buffered";
    प्रत्यागम "closed";
}

    स्थिर शून्य
channel_part_info(channel_T *channel, dict_T *dict, वर्ण *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    वर्ण	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    जइ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    अण्णहा जइ (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    अण्णहा
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (पूर्णाङ्क)s.length);

    STRCPY(namebuf + tail, "mode");
    विकल्प (chanpart->ch_mode)
    {
	पक्ष CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    भङ्ग;
	पक्ष CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    भङ्ग;
	पक्ष CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    भङ्ग;
	पक्ष CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    भङ्ग;
	पक्ष CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    भङ्ग;
	पक्ष CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    भङ्ग;
	पक्ष CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    भङ्ग;
	स्वभाव:
	    STR_LITERAL_SET(s, "");
	    भङ्ग;
    }
    dict_add_string_len(dict, namebuf, s.string, (पूर्णाङ्क)s.length);

    STRCPY(namebuf + tail, "io");
    जइ (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    अण्णहा विकल्प (chanpart->ch_io)
    {
	पक्ष JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    भङ्ग;
	पक्ष JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    भङ्ग;
	पक्ष JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    भङ्ग;
	पक्ष JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    भङ्ग;
	पक्ष JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    भङ्ग;
	स्वभाव:
	    STR_LITERAL_SET(s, "");
	    भङ्ग;
    }
    dict_add_string_len(dict, namebuf, s.string, (पूर्णाङ्क)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_सङ्ख्या(dict, namebuf, chanpart->ch_timeout);
}

    स्थिर शून्य
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_सङ्ख्या(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    जइ (channel->ch_hostname != रिक्त)
    {
	जइ (channel->ch_port)
	    dict_add_सङ्ख्या(dict, "port", channel->ch_port);
	अण्णहा
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    अण्णहा
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
    शून्य
channel_close(channel_T *channel, पूर्णाङ्क invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    जइ (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	जइ (channel->ch_close_cb.cb_name != रिक्त)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	{
	    जइ (channel->ch_close_cb.cb_name != रिक्त
			    || channel->ch_part[part].ch_bufref.br_buf != रिक्त)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		जइ (channel->ch_close_cb.cb_name == रिक्त)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		जाव (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	जइ (channel->ch_close_cb.cb_name != रिक्त)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (वर्ण *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      जइ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      जइ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  प्रति (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = रिक्त;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = रिक्त;
    channel->ch_ss_close_cb = रिक्त;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    स्थिर शून्य
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    स्थिर शून्य
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    जइ (wq->wq_next == रिक्त)
	wq->wq_prev = रिक्त;
    अण्णहा
	wq->wq_next->wq_prev = रिक्त;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    स्थिर शून्य
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    जाव (channel_peek(channel, part) != रिक्त)
	vim_free(channel_get(channel, part, रिक्त));

    जाव (cb_head->cq_next != रिक्त)
    {
	cbq_T *ग्रन्थिः = cb_head->cq_next;

	remove_cb_ग्रन्थिः(cb_head, ग्रन्थिः);
	free_callback(&ग्रन्थिः->cq_callback);
	vim_free(ग्रन्थिः);
    }

    जाव (json_head->jq_next != रिक्त)
    {
	free_tv(json_head->jq_next->jq_मानम्);
	remove_json_ग्रन्थिः(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    जाव (ch_part->ch_writeque.wq_next != रिक्त)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    शून्य
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

#जइ defined(EXITFREE)
    शून्य
channel_free_all(शून्य)
{
    channel_T *channel;

    ch_log(रिक्त, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_सन्देशः_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    स्थिर पूर्णाङ्क
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    जइ (in_part->ch_writeque.wq_next != रिक्त)
	प्रत्यागम TRUE;
    जइ (buf == रिक्त)
	प्रत्यागम FALSE;
    प्रत्यागम in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_पङ्क्तिः_गणनम्)
	    : (in_part->ch_buf_शिखरम् <= in_part->ch_buf_bot
			    && in_part->ch_buf_शिखरम् <= buf->b_ml.ml_पङ्क्तिः_गणनम्);
}

#जइ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    स्थिर पूर्णाङ्क
channel_fill_wfds(पूर्णाङ्क maxfd_arg, fd_set *wfds)
{
    पूर्णाङ्क		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	जइ (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (पूर्णाङ्क)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((पूर्णाङ्क)in_part->ch_fd, wfds);
	    जइ ((पूर्णाङ्क)in_part->ch_fd >= maxfd)
		maxfd = (पूर्णाङ्क)in_part->ch_fd + 1;
	}
    }
    प्रत्यागम maxfd;
}
#अण्णहा
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    स्थिर पूर्णाङ्क
channel_fill_poll_write(पूर्णाङ्क nfd_in, संरचना pollfd *fds)
{
    पूर्णाङ्क		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	जइ (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	अण्णहा
	    in_part->ch_poll_idx = -1;
    }
    प्रत्यागम nfd;
}
#endif

प्रकारनिर्देश गणना {
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
    स्थिर channel_wait_result
channel_wait(channel_T *channel, sock_T fd, पूर्णाङ्क timeout)
{
    जइ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    जइ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	पूर्णाङ्क	sleep_time;
	DWORD	deadline = Get_Tick_गणनम्() + timeout;
	पूर्णाङ्क	delay = 1;

	// reading from a pipe, not a socket
	जाव (TRUE)
	{
	    पूर्णाङ्क r = PeekNamedPipe((HANDLE)fd, रिक्त, 0, रिक्त, &nread, रिक्त);

	    जइ (r && nread > 0)
		प्रत्यागम CW_READY;

	    जइ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, रिक्त);
	    }
	    अण्णहा जइ (r == 0)
		प्रत्यागम CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_पङ्क्तयः();

	    sleep_time = deadline - Get_Tick_गणनम्();
	    जइ (sleep_time <= 0)
		भङ्ग;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    जइ (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    जइ (delay > 10)
		delay = 10;
	}
    }
    अण्णहा
#endif
    {
#जइ defined(HAVE_SELECT)
	संरचना timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	पूर्णाङ्क		ret;
	पूर्णाङ्क		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	प्रति (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((पूर्णाङ्क)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (पूर्णाङ्क)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, रिक्त, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    जइ (ret == -1 && errno == EINTR)
		अनुवर्त;
# endif
	    जइ (ret > 0)
	    {
		जइ (FD_ISSET(fd, &rfds))
		    प्रत्यागम CW_READY;
		channel_write_any_पङ्क्तयः();
		अनुवर्त;
	    }
	    भङ्ग;
	}
#अण्णहा
	प्रति (;;)
	{
	    संरचना pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    पूर्णाङ्क		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    जइ (poll(fds, nfd, timeout) > 0)
	    {
		जइ (fds[0].revents & POLLIN)
		    प्रत्यागम CW_READY;
		channel_write_any_पङ्क्तयः();
		अनुवर्त;
	    }
	    भङ्ग;
	}
#endif
    }
    प्रत्यागम CW_NOT_READY;
}

    स्थिर शून्य
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, पूर्णाङ्क is_err, वर्ण *func)
{
    वर्ण	सन्देशः[] = "%s(): Read %s from ch_part[%d], closing";

    जइ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, सन्देशः, func, "error", part);
    अण्णहा
	ch_log(channel, सन्देशः, func, "EOF", part);

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
    जइ (channel->ch_nb_close_cb != रिक्त)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_सन्देशः_RAW,
			      (पूर्णाङ्क)STRLEN_LITERAL(DETACH_सन्देशः_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    जइ (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	जइ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    स्थिर शून्य
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    जइ (channel->ch_nb_close_cb != रिक्त)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    जइ (channel->ch_ss_close_cb != रिक्त)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    स्थिर शून्य
channel_read(channel_T *channel, ch_part_T part, वर्ण *func)
{
    स्थिर char_u	*buf = रिक्त;
    पूर्णाङ्क			len = 0;
    पूर्णाङ्क			readlen = 0;
    sock_T		fd;
    पूर्णाङ्क			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    जइ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	प्रत्यागम;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    जइ (buf == रिक्त)
    {
	buf = alloc(MAXMSGSIZE);
	जइ (buf == रिक्त)
	    प्रत्यागम;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    प्रति (;;)
    {
	जइ (channel_wait(channel, fd, 0) != CW_READY)
	    भङ्ग;
	जइ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    संरचना sockaddr_storage	client;

	    newchannel = add_channel();
	    जइ (newchannel == रिक्त)
	    {
		ch_error(रिक्त, "Cannot allocate channel.");
		प्रत्यागम;
	    }
	    socklen = परिमाण(client);
	    newfd = accept(fd, (संरचना sockaddr*)&client, &socklen);
	    जइ (newfd < 0)
	    {
		ch_error(रिक्त, "Cannot accept channel.");
		channel_free(newchannel);
		प्रत्यागम;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    जइ (channel->ch_ss_accept_cb != रिक्त)
	    {
		channel->ch_ss_accept_cb(newchannel);
		प्रत्यागम;
	    }
#endif

	    जइ (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		वर्ण addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((संरचना sockaddr_in*)&client)->sin_addr,
			addr, परिमाण(addr));
		vim_snprintf((वर्ण *)namebuf, परिमाण(namebuf), "%s:%d",
			addr,
			ntohs(((संरचना sockaddr_in*)&client)->sin_port));
#अण्णहा
		vim_snprintf((वर्ण *)namebuf, परिमाण(namebuf), "%s:%d",
		    inet_ntoa(((संरचना sockaddr_in*)&client)->sin_addr),
		    ntohs(((संरचना sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    अण्णहा जइ (client.ss_family == AF_INET6)
	    {
		वर्ण addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((संरचना sockaddr_in6*)&client)->sin6_addr,
			addr, परिमाण(addr));
		vim_snprintf((वर्ण *)namebuf, परिमाण(namebuf), "[%s]:%d",
			addr,
			ntohs(((संरचना sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    अण्णहा जइ (client.ss_family == AF_UNIX)
		vim_snprintf((वर्ण *)namebuf, परिमाण(namebuf),
							    "unix:anonymous");
	    अण्णहा
		vim_snprintf((वर्ण *)namebuf, परिमाण(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    प्रत्यागम;
	}
	जइ (use_socket)
	    len = sock_read(fd, (वर्ण *)buf, MAXMSGSIZE);
	अण्णहा
	    len = fd_read(fd, (वर्ण *)buf, MAXMSGSIZE);
	जइ (len <= 0)
	    भङ्ग;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    जइ (readlen <= 0)
    {
	जइ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#जइ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    अण्णहा जइ (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    शून्य
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
    स्थिर char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, पूर्णाङ्क timeout, पूर्णाङ्क raw, पूर्णाङ्क *outlen)
{
    char_u	*buf;
    char_u	*सन्देशः;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*ग्रन्थिः;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    जाव (TRUE)
    {
	ग्रन्थिः = channel_peek(channel, part);
	जइ (ग्रन्थिः != रिक्त)
	{
	    जइ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(ग्रन्थिः) != रिक्त))
		// got a complete message
		भङ्ग;
	    जइ (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		अनुवर्त;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    जइ (raw || fd == INVALID_FD)
		भङ्ग;
	}

	// Wait for up to the channel timeout.
	जइ (fd == INVALID_FD)
	    प्रत्यागम रिक्त;
	जइ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    प्रत्यागम रिक्त;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    जइ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != रिक्त)
    {
	सन्देशः = channel_get_all(channel, part, outlen);
    }
    अण्णहा
    {
	char_u *p;

	buf = ग्रन्थिः->rq_buffer;
	nl = channel_first_nl(ग्रन्थिः);

	// Convert NUL to NL, the internal representation.
	प्रति (p = buf; (nl == रिक्त || p < nl) && p < buf + ग्रन्थिः->rq_buflen; ++p)
	    जइ (*p == NUL)
		*p = NL;

	जइ (nl == रिक्त)
	{
	    // must be a closed channel with missing NL
	    सन्देशः = channel_get(channel, part, रिक्त);
	}
	अण्णहा जइ (nl + 1 == buf + ग्रन्थिः->rq_buflen)
	{
	    // get the whole buffer
	    सन्देशः = channel_get(channel, part, रिक्त);
	    *nl = NUL;
	}
	अण्णहा
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    सन्देशः = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (पूर्णाङ्क)(nl - buf) + 1);
	}
    }
    जइ (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != रिक्त ? *outlen : (पूर्णाङ्क)STRLEN(सन्देशः));
    प्रत्यागम सन्देशः;
}

स्थिर पूर्णाङ्क channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    पूर्णाङ्क
channel_in_blocking_wait(शून्य)
{
    प्रत्यागम channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    स्थिर शून्य
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, पूर्णाङ्क timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (शून्य)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#अण्णहा
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
    स्थिर पूर्णाङ्क
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	पूर्णाङ्क	    timeout_arg,
	पूर्णाङ्क	    id,
	typval_T    **rettv)
{
    पूर्णाङ्क		more;
    sock_T	fd;
    पूर्णाङ्क		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    पूर्णाङ्क		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    जइ (id >= 0)
	channel_add_block_id(chanpart, id);

    प्रति (;;)
    {
	जइ (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (शून्य)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	जइ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    भङ्ग;
	}

	जइ (!more)
	{
	    शून्य *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    शून्य *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    जइ (channel_parse_messages())
		अनुवर्त;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    जइ (readahead_ptr != रिक्त && readahead_ptr != prev_readahead_ptr)
		अनुवर्त;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (पूर्णाङ्क)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    जइ (timeout <= 0 || fd == INVALID_FD)
	    {
		जइ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		भङ्ग;
	    }
	    // If there was an incomplete message use the deadline for that.
	    जइ (chanpart->ch_wait_len > 0)
	    {
		पूर्णाङ्क	वामः;
#ifdef MSWIN
		वामः = chanpart->ch_deadline - Get_Tick_गणनम्() + 1;
#अण्णहा
		{
		    संरचना timeval now_tv;

		    gettimeofday(&now_tv, रिक्त);
		    वामः = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		जइ (वामः < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		अण्णहा जइ (वामः < timeout)
		    timeout = वामः;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    जइ (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    प्रत्यागम retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, पूर्णाङ्क check_open, पूर्णाङ्क reading, ch_part_T part)
{
    channel_T	*channel = रिक्त;
    पूर्णाङ्क		has_readahead = FALSE;

    जइ (tv->v_type == VAR_JOB)
    {
	जइ (tv->vval.v_job != रिक्त)
	    channel = tv->vval.v_job->jv_channel;
    }
    अण्णहा जइ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    अण्णहा
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	प्रत्यागम रिक्त;
    }
    जइ (channel != रिक्त && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_गणनम् ? part : channel_part_read(channel));

    जइ (check_open && (channel == रिक्त || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	प्रत्यागम रिक्त;
    }
    प्रत्यागम channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    स्थिर शून्य
common_channel_read(typval_T *argvars, typval_T *rettv, पूर्णाङ्क raw, पूर्णाङ्क blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_गणनम्;
    jobopt_T	opt;
    पूर्णाङ्क		mode;
    पूर्णाङ्क		timeout;
    पूर्णाङ्क		id = -1;
    typval_T	*listtv = रिक्त;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जइ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    clear_job_options(&opt);
    जइ (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	गच्छ theend;

    जइ (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    जइ (channel == रिक्त)
	गच्छ theend;

    जइ (part == PART_गणनम्)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    जइ (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    जइ (blob || mode == CH_MODE_BLOB)
    {
	पूर्णाङ्क	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	जइ (p != रिक्त)
	{
	    blob_T	*b = blob_alloc();

	    जइ (b != रिक्त)
	    {
		b->bv_ga.ga_len = outlen;
		जइ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		अण्णहा
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    अण्णहा जइ (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, रिक्त);
    अण्णहा
    {
	जइ (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	जइ (listtv != रिक्त)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	अण्णहा
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_सङ्ख्या = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#जइ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    शून्य
channel_handle_events(पूर्णाङ्क only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	जइ (only_keep_open && !channel->ch_keep_open)
	    अनुवर्त;

	// check the socket and pipes
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    जइ (fd == INVALID_FD)
		अनुवर्त;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    पूर्णाङ्क r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    जइ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    अण्णहा जइ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	जइ (channel->ch_job != रिक्त)
	{
	    job_T *job = channel->ch_job;

	    जइ (job->jv_tty_out != रिक्त && job->jv_status == JOB_FINISHED)
		प्रति (part = PART_SOCK; part < PART_गणनम्; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#जइ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    पूर्णाङ्क
channel_any_keep_open(शून्य)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	जइ (channel->ch_keep_open)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    शून्य
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    पूर्णाङ्क		fd = ch_part->ch_fd;

    जइ (fd == INVALID_FD)
	प्रत्यागम;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#अण्णहा
    (शून्य)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    पूर्णाङ्क
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	पूर्णाङ्क	  len_arg,
	वर्ण	  *fun)
{
    पूर्णाङ्क		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    पूर्णाङ्क		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    जइ (fd == INVALID_FD)
    {
	जइ (!channel->ch_error && fun != रिक्त)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	प्रत्यागम FAIL;
    }

    जइ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    जइ (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_सन्देशः = 0;
    }

    प्रति (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	पूर्णाङ्क	    len;

	जइ (wq->wq_next != रिक्त)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	अण्णहा
	{
	    जइ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		प्रत्यागम OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	जइ (part == PART_SOCK)
	    res = sock_write(fd, (वर्ण *)buf, len);
	अण्णहा
	{
	    res = fd_write(fd, (वर्ण *)buf, len);
#ifdef MSWIN
	    जइ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, रिक्त);
	    }
#endif
	}
	जइ (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	जइ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    जइ (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    जइ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		जइ (entry != रिक्त)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    अनुवर्त;
		}
		जइ (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    अण्णहा
	    {
		// Wrote only buf[res] bytes, can't write more now.
		जइ (entry != रिक्त)
		{
		    जइ (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (वर्ण *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		अण्णहा
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		जइ (wq->wq_prev != रिक्त
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    जइ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((वर्ण *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		अण्णहा
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    जइ (last != रिक्त)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = रिक्त;
			जइ (wq->wq_prev == रिक्त)
			    wq->wq_next = last;
			अण्णहा
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			जइ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	अण्णहा जइ (res != len)
	{
	    जइ (!channel->ch_error && fun != रिक्त)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    प्रत्यागम FAIL;
	}

	channel->ch_error = FALSE;
	प्रत्यागम OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    स्थिर channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	पूर्णाङ्क	    len,
	पूर्णाङ्क	    id,
	पूर्णाङ्क	    eval,
	jobopt_T    *opt,
	वर्ण	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जइ (channel == रिक्त)
	प्रत्यागम रिक्त;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    जइ (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	प्रत्यागम रिक्त;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    जइ (opt->jo_callback.cb_name != रिक्त && *opt->jo_callback.cb_name != NUL)
    {
	जइ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    प्रत्यागम रिक्त;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    जइ (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == रिक्त)
	प्रत्यागम channel;
    प्रत्यागम रिक्त;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    स्थिर शून्य
ch_व्यञ्जकम्_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    पूर्णाङ्क		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    पूर्णाङ्क		timeout;
    पूर्णाङ्क		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जइ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जइ (channel == रिक्त)
	प्रत्यागम;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    जइ (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	प्रत्यागम;
    }

    जइ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	वर्ण		*कुञ्चिका = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	जइ (rettv_dict_alloc(rettv) == FAIL)
	    प्रत्यागम;

	जइ (check_for_dict_arg(argvars, 1) == FAIL)
	    प्रत्यागम;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)कुञ्चिका, -1);
	जइ (argvars[2].v_type == VAR_DICT
		&& dict_has_कुञ्चिका(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	जइ (di != रिक्त && di->di_tv.v_type != VAR_सङ्ख्या
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_मानम्_for_argument_str), कुञ्चिका);
	    प्रत्यागम;
	}

	जइ (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_सन्देशः_id;
	    जइ (di == रिक्त)
		dict_add_सङ्ख्या(d, "seq", id);
	    अण्णहा
		di->di_tv.vval.v_सङ्ख्या = id;
	}
	अण्णहा जइ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_सन्देशः_id;
	    जइ (di == रिक्त)
		dict_add_सङ्ख्या(d, "id", id);
	    अण्णहा
		di->di_tv.vval.v_सङ्ख्या = id;
	}
	अण्णहा
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    जइ (di != रिक्त && di->di_tv.v_type == VAR_सङ्ख्या)
		id = di->di_tv.vval.v_सङ्ख्या;
	}
	जइ (ch_mode == CH_MODE_LSP && !dict_has_कुञ्चिका(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_सन्देशः(&argvars[1]);
    }
    अण्णहा
    {
	id = ++channel->ch_last_सन्देशः_id;
	text = json_encode_nr_व्यञ्जकम्(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    जइ (text == रिक्त)
	प्रत्यागम;

    channel = send_common(argvars, text, (पूर्णाङ्क)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    जइ (channel != रिक्त && eval)
    {
	जइ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	अण्णहा
	    timeout = channel_get_timeout(channel, part_read);
	जइ (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    जइ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_सङ्ख्या;
		free_tv(listtv);
	    }
	    अण्णहा
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_सङ्ख्या;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    जइ (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	जइ (rettv->vval.v_dict != रिक्त)
	    dict_add_सङ्ख्या(rettv->vval.v_dict, "seq", id);
    }
    अण्णहा जइ (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	जइ (rettv->vval.v_dict != रिक्त)
	    dict_add_सङ्ख्या(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    स्थिर शून्य
ch_raw_common(typval_T *argvars, typval_T *rettv, पूर्णाङ्क eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    पूर्णाङ्क		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    पूर्णाङ्क		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जइ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	प्रत्यागम;

    जइ (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    अण्णहा
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (पूर्णाङ्क)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    जइ (channel != रिक्त && eval)
    {
	जइ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	अण्णहा
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, रिक्त);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#जइ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    पूर्णाङ्क
channel_poll_setup(पूर्णाङ्क nfd_in, शून्य *fds_in, पूर्णाङ्क *towait)
{
    पूर्णाङ्क		nfd = nfd_in;
    channel_T	*channel;
    संरचना	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    जइ (ch_part->ch_fd != INVALID_FD)
	    {
		जइ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    जइ (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		अण्णहा
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    अण्णहा
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    प्रत्यागम nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    पूर्णाङ्क
channel_poll_check(पूर्णाङ्क ret_in, शून्य *fds_in)
{
    पूर्णाङ्क		ret = ret_in;
    channel_T	*channel;
    संरचना	pollfd *fds = fds_in;
    ch_part_T	part;
    पूर्णाङ्क		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    जइ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    अण्णहा जइ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	जइ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_निवेशः(channel);
	    --ret;
	}
    }

    प्रत्यागम ret;
}
#endif // UNIX && !HAVE_SELECT

#जइ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    पूर्णाङ्क
channel_select_setup(
	पूर्णाङ्क maxfd_in,
	शून्य *rfds_in,
	शून्य *wfds_in,
	संरचना timeval *tv,
	संरचना timeval **tvp)
{
    पूर्णाङ्क		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    जइ (fd != INVALID_FD)
	    {
		जइ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    जइ (*tvp == रिक्त || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		अण्णहा
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    जइ ((पूर्णाङ्क)fd >= FD_SETSIZE)
			अनुवर्त;
# endif
		    FD_SET((पूर्णाङ्क)fd, rfds);
		    जइ (maxfd < (पूर्णाङ्क)fd)
			maxfd = (पूर्णाङ्क)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    प्रत्यागम maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    पूर्णाङ्क
channel_select_check(पूर्णाङ्क ret_in, शून्य *rfds_in, शून्य *wfds_in)
{
    पूर्णाङ्क		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	प्रति (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    जइ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (पूर्णाङ्क)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    अण्णहा जइ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	जइ (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (पूर्णाङ्क)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_निवेशः(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	जइ (channel->ch_job != रिक्त)
	{
	    job_T *job = channel->ch_job;

	    जइ (job->jv_tty_out != रिक्त && job->jv_status == JOB_FINISHED)
		प्रति (part = PART_SOCK; part < PART_गणनम्; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    प्रत्यागम ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    स्थिर शून्य
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    जइ (stdio_channel->CH_OUT_FD != INVALID_FD)
	(शून्य)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(पूर्णाङ्क)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#नोचेद्यदि defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    संरचना timeval	tv;
    संरचना timeval	*tvp = &tv;
    पूर्णाङ्क			maxfd;
    पूर्णाङ्क			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, रिक्त, tvp);
    जइ (ret >= 0)
	(शून्य)channel_select_check(ret, &rfds, &wfds);
#अण्णहा
    संरचना pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    पूर्णाङ्क			nfd;
    पूर्णाङ्क			towait = (पूर्णाङ्क)msec;
    पूर्णाङ्क			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    जइ (ret >= 0)
	(शून्य)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    शून्य
channel_stdio_loop(शून्य)
{
    जाव (stdio_channel != रिक्त && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	जइ (due_time > 0 && due_time < msec)
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
    पूर्णाङ्क
channel_parse_messages(शून्य)
{
    channel_T	*channel = first_channel;
    पूर्णाङ्क		ret = FALSE;
    पूर्णाङ्क		r;
    ch_part_T	part = PART_SOCK;
    स्थिर पूर्णाङ्क	recursive = 0;
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
    जइ ((did_repeated_सन्देशः & REPEATED_सन्देशः_LOOKING) == 0)
    {
	ch_log(रिक्त, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_सन्देशः = REPEATED_सन्देशः_LOOKING;
    }
    जाव (channel != रिक्त)
    {
	जइ (recursive == 1)
	{
	    जइ (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_गणनम्);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		अनुवर्त;
	    }
	    जइ (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		जइ (channel->ch_job != रिक्त)
		    channel->ch_job->jv_channel = रिक्त;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		अनुवर्त;
	    }
	    जइ (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		अनुवर्त;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	जइ (part == PART_OUT
		&& channel->ch_job != रिक्त
		&& channel->ch_job->jv_tty_out != रिक्त
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    अनुवर्त;
	}
#endif
	जइ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    जइ (r == OK)
		ret = TRUE;
	    जइ (channel_unref(channel) || (r == OK
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
		अनुवर्त;
	    }
	}
	जइ (part < PART_ERR)
	    ++part;
	अण्णहा
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    जइ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    प्रत्यागम ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    पूर्णाङ्क
channel_any_readahead(शून्य)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    जाव (channel != रिक्त)
    {
	जइ (channel_has_readahead(channel, part))
	    प्रत्यागम TRUE;
	जइ (part < PART_ERR)
	    ++part;
	अण्णहा
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    प्रत्यागम FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    पूर्णाङ्क
set_ref_in_channel(पूर्णाङ्क copyID)
{
    पूर्णाङ्क		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    प्रति (channel = first_channel; !abort && channel != रिक्त;
						   channel = channel->ch_next)
	जइ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
	}
    प्रत्यागम abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    स्थिर ch_part_T
channel_part_send(channel_T *channel)
{
    जइ (channel->CH_SOCK_FD == INVALID_FD)
	प्रत्यागम PART_IN;
    प्रत्यागम PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    स्थिर ch_part_T
channel_part_read(channel_T *channel)
{
    जइ (channel->CH_SOCK_FD == INVALID_FD)
	प्रत्यागम PART_OUT;
    प्रत्यागम PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    स्थिर ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    जइ (channel == रिक्त)
	प्रत्यागम CH_MODE_JSON;
    प्रत्यागम channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    स्थिर पूर्णाङ्क
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    प्रत्यागम channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    शून्य
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_सङ्ख्या = 0;
    जइ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जइ (channel != रिक्त)
	rettv->vval.v_सङ्ख्या = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    शून्य
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    जइ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जइ (channel != रिक्त)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    शून्य
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    जइ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जइ (channel != रिक्त)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    शून्य
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_सङ्ख्या = -1;

    जइ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जइ (channel == रिक्त)
	प्रत्यागम;

    char_u	*what = tv_get_string(&argvars[1]);
    पूर्णाङ्क		part;
    जइ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    अण्णहा जइ (STRCMP(what, "out") == 0)
	part = PART_OUT;
    अण्णहा जइ (STRCMP(what, "in") == 0)
	part = PART_IN;
    अण्णहा
	part = PART_SOCK;
    जइ (channel->ch_part[part].ch_bufref.br_buf != रिक्त)
	rettv->vval.v_सङ्ख्या =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    शून्य
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    जइ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जइ (channel == रिक्त)
	प्रत्यागम;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    जइ (channel->ch_job != रिक्त)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    शून्य
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    जइ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जइ (channel != रिक्त && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    शून्य
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    जइ (check_restricted() || check_secure())
	प्रत्यागम;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    शून्य
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    जइ (check_restricted() || check_secure())
	प्रत्यागम;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    शून्य
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    शून्य
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    शून्य
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    शून्य
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_व्यञ्जकम्_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    शून्य
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_व्यञ्जकम्_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    शून्य
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    शून्य
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    शून्य
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    जइ (check_restricted() || check_secure())
	प्रत्यागम;

    जइ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जइ (channel == रिक्त)
	प्रत्यागम;
    clear_job_options(&opt);
    जइ (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    शून्य
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    पूर्णाङ्क		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जइ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	प्रत्यागम;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    जइ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	जइ (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    वर्ण      *status = channel_status(channel, -1);

    जइ (channel == रिक्त)
	vim_snprintf((वर्ण *)buf, NUMBUFLEN, "channel %s", status);
    अण्णहा
	vim_snprintf((वर्ण *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    प्रत्यागम buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(पूर्णाङ्क ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	जइ (ch->ch_id == ch_id)
	    प्रत्यागम ch;
    प्रत्यागम रिक्त;
}

#endif // FEAT_JOB_CHANNEL
