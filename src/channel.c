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

#जर defined(FEAT_JOB_CHANNEL)

// TRUE when netbeans is running with a GUI.
#ifdef FEAT_GUI
# define CH_HAS_GUI (gui.in_use || gui.starting)
#endif

// Note: when making changes here also adjust configure.ac.
#ifdef MSWIN
// WinSock API is separated from C API, thus we can't use read(), write(),
// errno...
# define SOCK_ERRNO त्रुटि_सं = WSAGetLastError()
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
प्रकारव्याख्या संरचना sockaddr_un {
    ADDRESS_FAMILY sun_family;
    अक्षर sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#अन्यथा
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
# define sock_close(sd) बंद(sd)
# define fd_read(fd, buf, len) पठ(fd, buf, len)
# define fd_write(sd, buf, len) लिख(sd, buf, len)
# define fd_close(sd) बंद(sd)
#endif

स्थिर शून्य channel_read(channel_T *channel, ch_part_T part, अक्षर *func);
स्थिर शून्य channel_wait_any(दीर्घ msec);
स्थिर ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
स्थिर पूर्णांक channel_get_timeout(channel_T *channel, ch_part_T part);
स्थिर channel_T *channel_open_stdio(शून्य);
स्थिर ch_part_T channel_part_send(channel_T *channel);
स्थिर ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    साठी ((ch) = first_channel; (ch) != रिक्त; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
स्थिर पूर्णांक safe_to_invoke_callback = 0;

#ifdef MSWIN
    स्थिर पूर्णांक
fd_read(sock_T fd, अक्षर *buf, माप_प्रकार len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    जर (!ReadFile(h, buf, (DWORD)len, &nread, रिक्त))
	परत -1;
    परत (पूर्णांक)nread;
}

    स्थिर पूर्णांक
fd_write(sock_T fd, अक्षर *buf, माप_प्रकार len)
{
    माप_प्रकार	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    पर्यंत (todo > 0)
    {
	जर (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	अन्यथा
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	जर (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    जर (err != ERROR_IO_PENDING)
		परत -1;
	    जर (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		परत -1;
	    FlushFileBuffers(h);
	}
	अन्यथा जर (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    थांबा;
	todo -= nwrite;
	done += nwrite;
    }
    परत (पूर्णांक)done;
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
# define PERROR(संदेश) (शून्य)semsg("%s: %s", संदेश, strerror_win32(त्रुटि_सं))

    स्थिर अक्षर *
strerror_win32(पूर्णांक eno)
{
    स्थिर LPVOID msgbuf = रिक्त;
    char_u *ptr;

    जर (msgbuf)
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
    जर (msgbuf != रिक्त)
	// chomp \r or \n
	साठी (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    निवड (*ptr)
	    {
		स्थिती '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    थांबा;
		स्थिती '\n':
		    जर (*(ptr + 1) == '\0')
			*ptr = '\0';
		    अन्यथा
			*ptr = ' ';
		    थांबा;
	    }
    परत msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
स्थिर channel_T *first_channel = रिक्त;
स्थिर पूर्णांक next_ch_id = 0;

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

    जर (channel == रिक्त)
	परत रिक्त;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    साठी (part = PART_SOCK; part < PART_मोजणी; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_आदान_Handler = (Xt_आदान_Id)रिक्त;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_आदान_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    जर (first_channel != रिक्त)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    परत channel;
}

    पूर्णांक
has_any_channel(शून्य)
{
    परत first_channel != रिक्त;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    पूर्णांक
channel_still_useful(channel_T *channel)
{
    पूर्णांक has_sock_संदेश;
    पूर्णांक	has_out_संदेश;
    पूर्णांक	has_err_संदेश;

    // If the job was killed the channel is not expected to work anymore.
    जर (channel->ch_job_killed && channel->ch_job == रिक्त)
	परत FALSE;

    // If there is a close callback it may still need to be invoked.
    जर (channel->ch_close_cb.cb_name != रिक्त)
	परत TRUE;

    // If reading from or a buffer it's still useful.
    जर (channel->ch_part[PART_IN].ch_bufref.br_buf != रिक्त)
	परत TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_संदेश = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != रिक्त
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != रिक्त;
    has_out_संदेश = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != रिक्त
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != रिक्त;
    has_err_संदेश = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != रिक्त
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != रिक्त;
    परत (channel->ch_callback.cb_name != रिक्त && (has_sock_संदेश
		|| has_out_संदेश || has_err_संदेश))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != रिक्त
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != रिक्त)
		    && has_out_संदेश)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != रिक्त
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != रिक्त)
		    && has_err_संदेश);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    पूर्णांक
channel_can_close(channel_T *channel)
{
    परत channel->ch_to_be_closed == 0;
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
    जर (channel->ch_next != रिक्त)
	channel->ch_next->ch_prev = channel->ch_prev;
    जर (channel->ch_prev == रिक्त)
	first_channel = channel->ch_next;
    अन्यथा
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    स्थिर शून्य
channel_free(channel_T *channel)
{
    जर (in_free_unref_वस्तुसंच)
	परत;

    जर (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    अन्यथा
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
    स्थिर पूर्णांक
channel_may_free(channel_T *channel)
{
    जर (!channel_still_useful(channel))
    {
	channel_free(channel);
	परत TRUE;
    }
    परत FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    पूर्णांक
channel_unref(channel_T *channel)
{
    जर (channel != रिक्त && --channel->ch_refcount <= 0)
	परत channel_may_free(channel);
    परत FALSE;
}

    पूर्णांक
free_unused_channels_contents(पूर्णांक copyID, पूर्णांक mask)
{
    पूर्णांक		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	जर (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    परत did_free;
}

    शून्य
free_unused_channels(पूर्णांक copyID, पूर्णांक mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    साठी (ch = first_channel; ch != रिक्त; ch = ch_next)
    {
	ch_next = ch->ch_next;
	जर (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#जर defined(FEAT_GUI)

# जर defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    स्थिर channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    जर (fd == INVALID_FD)
	परत रिक्त;

    FOR_ALL_CHANNELS(channel)
    {
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	    जर (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		परत channel;
	    }
    }
    परत रिक्त;
}

    स्थिर शून्य
channel_read_fd(पूर्णांक fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    जर (channel == रिक्त)
	ch_error(रिक्त, "Channel for fd %d not found", fd);
    अन्यथा
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    स्थिर शून्य
messageFromServerX11(XtPointer clientData,
		  पूर्णांक *unused1 UNUSED,
		  Xt_आदान_Id *unused2 UNUSED)
{
    channel_read_fd((पूर्णांक)(दीर्घ)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  जर GTK_CHECK_VERSION(3,0,0)
    स्थिर gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    परत TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  अन्यथा
    स्थिर शून्य
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_आदान_Condition unused2 UNUSED)
{
    channel_read_fd((पूर्णांक)(दीर्घ)clientData);
}
#  endif
# endif

    स्थिर शून्य
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    जर (!CH_HAS_GUI)
	परत;

    // gets stuck in handling events for a not connected channel
    जर (channel->ch_keep_open)
	परत;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    जर (channel->ch_part[part].ch_आदान_Handler == (Xt_आदान_Id)रिक्त)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_आदान_Handler = Xt_App_Add_आदान(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_आदान_Read_Mask + Xt_आदान_Except_Mask),
		messageFromServerX11,
		(XtPointer)(दीर्घ)channel->ch_part[part].ch_fd);
    }
# अन्यथा
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    जर (channel->ch_part[part].ch_आदान_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   जर GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_आदान_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   अन्यथा
	channel->ch_part[part].ch_आदान_Handler = gdk_आदान_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_आदान_Condition)
			     ((पूर्णांक)GDK_आदान_READ + (पूर्णांक)GDK_आदान_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(दीर्घ)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    स्थिर शून्य
channel_gui_register(channel_T *channel)
{
    जर (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    जर (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    जर (channel->CH_ERR_FD != INVALID_FD
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
    जर (channel->ch_part[part].ch_आदान_Handler != (Xt_आदान_Id)रिक्त)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_आदान(channel->ch_part[part].ch_आदान_Handler);
	channel->ch_part[part].ch_आदान_Handler = (Xt_आदान_Id)रिक्त;
    }
# अन्यथा
#  ifdef FEAT_GUI_GTK
    जर (channel->ch_part[part].ch_आदान_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   जर GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_आदान_Handler);
#   अन्यथा
	gdk_आदान_remove(channel->ch_part[part].ch_आदान_Handler);
#   endif
	channel->ch_part[part].ch_आदान_Handler = 0;
    }
#  endif
# endif
}

    स्थिर शून्य
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    साठी (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    स्थिर पूर्णांक
channel_connect(
	channel_T *channel,
	स्थिरांक संरचना sockaddr *server_addr,
	पूर्णांक server_addrlen,
	पूर्णांक *waittime)
{
    पूर्णांक		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    पर्यंत (TRUE)
    {
	दीर्घ	elapsed_msec = 0;
	पूर्णांक	waitnow;
	पूर्णांक	ret;

	जर (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	जर (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    परत -1;
	}

	जर (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    जर (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#अन्यथा
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", त्रुटि_सं);
		sock_close(sd);
		परत -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	जर (ret == 0)
	    // The connection could be established.
	    थांबा;

	SOCK_ERRNO;
	जर (*waittime < 0 || (त्रुटि_सं != EWOULDBLOCK
		&& त्रुटि_सं != ECONNREFUSED
#ifdef EINPROGRESS
		&& त्रुटि_सं != EINPROGRESS
#endif
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", त्रुटि_सं);
	    PERROR(_(e_cannot_connect_to_port));
	    sock_close(sd);
	    परत -1;
	}
	अन्यथा जर (त्रुटि_सं == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    परत -1;
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
	    पूर्णांक			so_error = 0;
	    socklen_t		so_error_len = आकार(so_error);
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
	    जर (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", त्रुटि_सं);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		परत -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    जर (FD_ISSET(sd, &wfds))
		थांबा;
	    elapsed_msec = waitnow;
	    जर (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		सुरूठेवा;
	    }
#अन्यथा
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    जर (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		जर (ret < 0 || (so_error != 0
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
		    परत -1;
		}
		अन्यथा जर (त्रुटि_सं == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    परत -1;
		}
	    }

	    जर (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		थांबा;

	    gettimeofday(&end_tv, रिक्त);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	जर (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    जर (waitnow > 0)
	    {
		mch_delay((दीर्घ)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    जर (!got_int)
	    {
		जर (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		सुरूठेवा;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	परत -1;
    }

    जर (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#अन्यथा
	(शून्य)fcntl(sd, F_SETFL, 0);
#endif
    }

    परत sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	स्थिरांक अक्षर *path,
	शून्य (*nb_close_cb)(शून्य))
{
    channel_T		*channel = रिक्त;
    पूर्णांक			sd = -1;
    माप_प्रकार		path_len = STRLEN(path);
    संरचना sockaddr_un	server;
    माप_प्रकार		server_len;
    पूर्णांक			waittime = -1;

    जर (*path == NUL || path_len >= आकार(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	परत रिक्त;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जर (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	परत रिक्त;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, आकार(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = दुरत्व(संरचना sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (संरचना sockaddr *)&server, (पूर्णांक)server_len,
								   &waittime);

    जर (sd < 0)
    {
	channel_free(channel);
	परत रिक्त;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (अक्षर *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    परत channel;
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
	स्थिरांक अक्षर *hostname,
	पूर्णांक port,
	पूर्णांक waittime,
	शून्य (*nb_close_cb)(शून्य))
{
    पूर्णांक			sd = -1;
    channel_T		*channel = रिक्त;
#ifdef FEAT_IPV6
    पूर्णांक			err;
    संरचना addrinfo	hints;
    संरचना addrinfo	*res = रिक्त;
    संरचना addrinfo	*addr = रिक्त;
#अन्यथा
    संरचना sockaddr_in	server;
    संरचना hostent	*host = रिक्त;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जर (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	परत रिक्त;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# जर defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    जर ((err = getaddrinfo(hostname, रिक्त, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	परत रिक्त;
    }

    // Count the number of addresses for timeout distribution
    पूर्णांक addr_मोजणी = 0;
    साठी (addr = res; addr != रिक्त; addr = addr->ai_next)
	addr_मोजणी++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    जर (waittime == 0)
	waittime = 15;

    पूर्णांक addr_index = 0;
    साठी (addr = res; addr != रिक्त; addr = addr->ai_next, addr_index++)
    {
	स्थिरांक अक्षर  *dst = hostname;
# ifdef HAVE_INET_NTOP
	स्थिरांक शून्य  *src = रिक्त;
	अक्षर	    buf[NUMBUFLEN];
# endif
	पूर्णांक	    try_waittime;
	पूर्णांक	    before_waittime;
	पूर्णांक	    consumed;
	पूर्णांक	    remaining_addrs;

	जर (addr->ai_family == AF_INET6)
	{
	    संरचना sockaddr_in6 *sai = (संरचना sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	अन्यथा जर (addr->ai_family == AF_INET)
	{
	    संरचना sockaddr_in *sai = (संरचना sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	जर (src != रिक्त)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, आकार(buf));
	    जर (dst == रिक्त)
		dst = hostname;
	    अन्यथा जर (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	जर (addr->ai_next == रिक्त)
	    try_waittime = waittime;
	अन्यथा जर (addr_index == 0)
	{
	    जर (waittime > 500)
		try_waittime = 250;
	    अन्यथा जर (waittime > 30)
		try_waittime = waittime / 2;
	    अन्यथा
		try_waittime = waittime;
	}
	अन्यथा
	{
	    remaining_addrs = addr_मोजणी - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (पूर्णांक)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	जर (waittime < 0)
	    waittime = 0;

	जर (sd >= 0)
	    थांबा;

	// If we have no time left, stop trying
	जर (waittime <= 0 && addr->ai_next != रिक्त)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    थांबा;
	}
    }

    freeaddrinfo(res);
#अन्यथा
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    जर ((host = gethostbyname(hostname)) == रिक्त)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	परत रिक्त;
    }
    {
	अक्षर *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	स_नकल(&p, &host->h_addr_list[0], आकार(p));
	स_नकल((अक्षर *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    जर (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (संरचना sockaddr *)&server, आकार(server),
								   &waittime);
#endif

    जर (sd < 0)
    {
	channel_free(channel);
	परत रिक्त;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (अक्षर *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    परत channel;
}

    स्थिर शून्य
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    जर (callback->cb_name != रिक्त && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    अन्यथा
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
    set_option_मूल्य_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_मूल्य_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    जर (curbuf->b_ml.ml_mfp == रिक्त)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    स्थिर buf_T *
channel_find_buffer(char_u *name, पूर्णांक err, पूर्णांक संदेश)
{
    buf_T *buf = रिक्त;
    buf_T *save_curbuf = curbuf;

    जर (name != रिक्त && *name != NUL)
    {
	buf = buflist_findname(name);
	जर (buf == रिक्त)
	    buf = buflist_findname_exp(name);
    }

    जर (buf != रिक्त)
	परत buf;

    buf = buflist_new(name == रिक्त || *name == NUL ? रिक्त : name,
	    रिक्त, (linenr_T)0, BLN_LISTED | BLN_NEW);
    जर (buf == रिक्त)
	परत रिक्त;
    prepare_buffer(buf);

    curbuf = buf;
    जर (संदेश)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    परत buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    स्थिर शून्य
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    जर (opt->jo_set & JO_MODE)
	साठी (part = PART_SOCK; part < PART_मोजणी; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    जर (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    जर (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    जर (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    जर (opt->jo_set & JO_TIMEOUT)
	साठी (part = PART_SOCK; part < PART_मोजणी; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    जर (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    जर (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    जर (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    जर (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    जर (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    जर (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    जर (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    जर ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	जर (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	जर (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    जर (buf == रिक्त)
		semsg(_(e_buffer_nr_does_not_exist),
					       (दीर्घ)opt->jo_io_buf[PART_OUT]);
	}
	अन्यथा
	{
	    पूर्णांक संदेश = TRUE;

	    जर (opt->jo_set2 & JO2_OUT_संदेश)
		संदेश = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, संदेश);
	}
	जर (buf != रिक्त)
	{
	    जर (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    जर (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    अन्यथा
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (अक्षर *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		जर (buf->b_ml.ml_mfp == रिक्त)
		    prepare_buffer(buf);
	    }
	}
    }

    जर ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	जर (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	जर (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	अन्यथा जर (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    जर (buf == रिक्त)
		semsg(_(e_buffer_nr_does_not_exist),
					       (दीर्घ)opt->jo_io_buf[PART_ERR]);
	}
	अन्यथा
	{
	    पूर्णांक संदेश = TRUE;

	    जर (opt->jo_set2 & JO2_ERR_संदेश)
		संदेश = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, संदेश);
	}
	जर (buf != रिक्त)
	{
	    जर (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    जर (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    अन्यथा
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (अक्षर *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		जर (buf->b_ml.ml_mfp == रिक्त)
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
    पूर्णांक
channel_parse_socketserver_address(
	char_u	*address,
	पूर्णांक	*port,
	char_u	**unix_path,
	bool	quiet)
{
    अक्षर    *rest;
    दीर्घ    val;

    जर (*address == NUL)
	जा fail;

    जर (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	परत OK;
    }

    val = म_से_दीर्घ((अक्षर *)(address), &rest, 10);
    जर (val < 0 || val >= 65536 || *rest != NUL)
    {
	जर (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	परत FAIL;
    }
    *port = (पूर्णांक)val;

    परत OK;
fail:
    जर (!quiet)
	semsg(_(e_invalid_argument_str), address);
    परत FAIL;
}

/*
 * Implements ch_open().
 */
    स्थिर channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    अक्षर	*rest;
    पूर्णांक		port = 0;
    पूर्णांक		is_ipv6 = FALSE;
    पूर्णांक		is_unix = FALSE;
    पूर्णांक		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = रिक्त;

    जर (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत रिक्त;

    address = tv_get_string(&argvars[0]);
    जर (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	परत रिक्त;

    जर (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	परत रिक्त;
    }

    जर (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    अन्यथा जर (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    अन्यथा जर (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	जर (p == रिक्त || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    परत रिक्त;
	}
    }
    अन्यथा
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	जर (p == रिक्त)
	{
	    semsg(_(e_invalid_argument_str), address);
	    परत रिक्त;
	}
    }

    जर (!is_unix && !is_stdio)
    {
	port = म_से_दीर्घ((अक्षर *)(p + 1), &rest, 10);
	जर (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    परत रिक्त;
	}
	जर (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	अन्यथा
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    जर (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	जा theend;
    जर (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	जा theend;
    }

    जर (is_stdio)
	channel = channel_open_stdio();
    अन्यथा जर (is_unix)
	channel = channel_open_unix((अक्षर *)address, रिक्त);
    अन्यथा
	channel = channel_open((अक्षर *)address, port, opt.jo_waittime, रिक्त);
    जर (channel != रिक्त)
    {
	पूर्णांक	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	जर (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    परत channel;
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

    जर (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	परत रिक्त;
    }
    जर (stdio_channel != रिक्त)
    {
	emsg(_(e_cannot_open_stdio_channel));
	परत रिक्त;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	पूर्णांक	nul_fd;

	जर (!DuplicateHandle(proc, GetStdHandle(STD_आदान_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    परत रिक्त;
	}
	जर (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    परत रिक्त;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, रिक्त, OPEN_EXISTING, 0,
		रिक्त);
	जर (hnul != INVALID_HANDLE_मूल्य)
	{
	    SetStdHandle(STD_आदान_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	जर (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#अन्यथा
    {
	पूर्णांक	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	जर (in_fd < 0 || out_fd < 0)
	{
	    जर (in_fd >= 0)
		बंद(in_fd);
	    जर (out_fd >= 0)
		बंद(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    परत रिक्त;
	}
	(शून्य)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(शून्य)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = खोलो("/dev/null", O_RDWR);
	जर (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    बंद(nul_fd);
	}
    }
#endif

    channel = add_channel();
    जर (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	परत रिक्त;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    परत channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    अक्षर	*rest;
    पूर्णांक		port;
    पूर्णांक		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = रिक्त;

    जर (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	परत रिक्त;

    arg = tv_get_string(&argvars[0]);
    जर (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	परत रिक्त;
    }

    जर (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    अन्यथा
    {
	port = म_से_दीर्घ((अक्षर *)(arg), &rest, 10);
	जर (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    परत रिक्त;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    जर (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	जा theend;
    जर (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	जा theend;
    }

    जर (is_unix)
	channel = channel_listen_unix((अक्षर *)arg, रिक्त, true);
    अन्यथा
	channel = channel_listen(port, रिक्त);
    जर (channel != रिक्त)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    परत channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	पूर्णांक port_in,
	शून्य (*nb_close_cb)(शून्य))
{
    पूर्णांक			sd = -1;
    संरचना sockaddr_in	server;
#ifndef FEAT_IPV6
    संरचना hostent	*host;
#endif
    पूर्णांक			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जर (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	परत रिक्त;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((अक्षर *)&server, 0, आकार(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    जर (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	परत रिक्त;
    }

#ifdef MSWIN
    जर (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (स्थिरांक अक्षर *)&val, आकार(val)) < 0)
#अन्यथा
    जर (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, आकार(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	परत रिक्त;
    }

    // Bind the socket to the port
    जर (bind(sd, (संरचना sockaddr *)&server, आकार(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	परत रिक्त;
    }

    जर (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	परत रिक्त;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    जर (port_in == 0)
    {
	संरचना sockaddr_in	addr;
	socklen_t		addr_len = आकार(addr);

	जर (getsockname(sd, (संरचना sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (अक्षर *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    परत channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	अक्षर *path,
	शून्य (*nb_close_cb)(शून्य),
	bool replace)
{
    पूर्णांक			sd = -1;
    संरचना sockaddr_un	server;
    channel_T		*channel;
    माप_प्रकार		path_len;
    माप_प्रकार		server_len;

    जर (path == रिक्त || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == रिक्त ? (अक्षर *)"" : path);
	परत रिक्त;
    }

    path_len = STRLEN(path);
    जर (path_len >= आकार(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	परत रिक्त;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    जर (channel == रिक्त)
    {
	ch_error(रिक्त, "Cannot allocate channel.");
	परत रिक्त;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, आकार(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    जर (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	परत रिक्त;
    }

    जर (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = दुरत्व(संरचना sockaddr_un, sun_path) + path_len + 1;
    जर (bind(sd, (संरचना sockaddr *)&server, (पूर्णांक)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	परत रिक्त;
    }

    जर (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	परत रिक्त;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (अक्षर *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    परत channel;
}

     शून्य
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    जर (*fd == INVALID_FD)
	परत;

    जर (part == PART_SOCK)
	sock_close(*fd);
    अन्यथा
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	जर ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    जर (channel->ch_named_pipe)
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
    जर (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#जर defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	जर (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    जर (out != INVALID_FD)
    {
#जर defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#जर defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    जर (err != INVALID_FD)
    {
#जर defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#जर defined(FEAT_GUI)
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

    जर (job->jv_in_buf == रिक्त)
	परत;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (अक्षर *)in_part->ch_bufref.br_buf->b_ffname);
    जर (options->jo_set & JO_IN_शीर्ष)
    {
	जर (options->jo_in_शीर्ष == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_शीर्ष =
		in_part->ch_bufref.br_buf->b_ml.ml_ओळ_मोजणी + 1;
	}
	अन्यथा
	    in_part->ch_buf_शीर्ष = options->jo_in_शीर्ष;
    }
    अन्यथा
	in_part->ch_buf_शीर्ष = 1;
    जर (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    अन्यथा
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_ओळ_मोजणी;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    स्थिर शून्य
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	पूर्णांक	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    जर (item == रिक्त)
	परत;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = रिक्त;
    जर (item->cq_prev == रिक्त)
	head->cq_next = item;
    अन्यथा
	item->cq_prev->cq_next = item;
}

    स्थिर शून्य
write_buf_ओळ(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *ओळ = ml_get_buf(buf, lnum, FALSE);
    पूर्णांक	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    पूर्णांक	    i;

    // Need to make a copy to be able to append a NL.
    जर ((p = alloc(len + 2)) == रिक्त)
	परत;
    स_नकल((अक्षर *)p, (अक्षर *)ओळ, len);

    जर (channel->ch_write_text_mode)
	p[len] = CAR;
    अन्यथा
    {
	साठी (i = 0; i < len; ++i)
	    जर (p[i] == NL)
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
    स्थिर पूर्णांक
can_write_buf_ओळ(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    जर (in_part->ch_fd == INVALID_FD)
	परत FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    जर (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    अन्यथा जर (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# जर defined(HAVE_SELECT)
	संरचना timeval	tval;
	fd_set		wfds;
	पूर्णांक		ret;

	FD_ZERO(&wfds);
	FD_SET((पूर्णांक)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	साठी (;;)
	{
	    ret = select((पूर्णांक)in_part->ch_fd + 1, रिक्त, &wfds, रिक्त, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    जर (ret == -1 && त्रुटि_सं == EINTR)
		सुरूठेवा;
#  endif
	    जर (ret <= 0 || in_part->ch_block_write == 1)
	    {
		जर (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		अन्यथा
		    ch_log(channel, "Input not ready for writing");
		परत FALSE;
	    }
	    थांबा;
	}
# अन्यथा
	संरचना pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	जर (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    परत FALSE;
	}
	जर (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    परत FALSE;
	}
# endif
    }
#endif
    परत TRUE;
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
    पूर्णांक		written = 0;

    जर (buf == रिक्त || in_part->ch_buf_append)
	परत;  // no buffer or using appending
    जर (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == रिक्त)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = रिक्त;
	परत;
    }

    साठी (lnum = in_part->ch_buf_शीर्ष; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_ओळ_मोजणी; ++lnum)
    {
	जर (!can_write_buf_ओळ(channel))
	    थांबा;
	write_buf_ओळ(buf, lnum, channel);
	++written;
    }

    जर (written == 1)
	ch_log(channel, "written line %d to channel", (पूर्णांक)lnum - 1);
    अन्यथा जर (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_शीर्ष = lnum;
    जर (lnum > buf->b_ml.ml_ओळ_मोजणी || lnum > in_part->ch_buf_bot)
    {
#जर defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	जर (channel->ch_job != रिक्त)
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
	जर (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    अन्यथा
	ch_log(channel, "Still %ld more lines to write",
				   (दीर्घ)(buf->b_ml.ml_ओळ_मोजणी - lnum + 1));
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
	साठी (part = PART_SOCK; part < PART_मोजणी; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    जर (ch_part->ch_bufref.br_buf == buf)
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
channel_write_आदान(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    जर (in_part->ch_writeque.wq_next != रिक्त)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    अन्यथा जर (in_part->ch_bufref.br_buf != रिक्त)
    {
	जर (in_part->ch_buf_append)
	    channel_write_new_ओळी(in_part->ch_bufref.br_buf);
	अन्यथा
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    शून्य
channel_write_any_ओळी(शून्य)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_आदान(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    शून्य
channel_write_new_ओळी(buf_T *buf)
{
    channel_T	*channel;
    पूर्णांक		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	पूर्णांक	    written = 0;

	जर (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    जर (in_part->ch_fd == INVALID_FD)
		सुरूठेवा;  // pipe was closed
	    found_one = TRUE;
	    साठी (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_ओळ_मोजणी;
								       ++lnum)
	    {
		जर (!can_write_buf_ओळ(channel))
		    थांबा;
		write_buf_ओळ(buf, lnum, channel);
		++written;
	    }

	    जर (written == 1)
		ch_log(channel, "written line %d to channel", (पूर्णांक)lnum - 1);
	    अन्यथा जर (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    जर (lnum < buf->b_ml.ml_ओळ_मोजणी)
		ch_log(channel, "Still %ld more lines to write",
				       (दीर्घ)(buf->b_ml.ml_ओळ_मोजणी - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    जर (!found_one)
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

    जर (safe_to_invoke_callback == 0)
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

    परत head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *गाठ)
{
    char_u  *buffer = गाठ->rq_buffer;
    long_u  i;

    साठी (i = 0; i < गाठ->rq_buflen; ++i)
	जर (buffer[i] == NL)
	    परत buffer + i;
    परत रिक्त;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, पूर्णांक *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *गाठ = head->rq_next;
    char_u *p;

    जर (गाठ == रिक्त)
	परत रिक्त;
    जर (outlen != रिक्त)
	*outlen += गाठ->rq_buflen;
    // dispose of the node but keep the buffer
    p = गाठ->rq_buffer;
    head->rq_next = गाठ->rq_next;
    जर (गाठ->rq_next == रिक्त)
	head->rq_prev = रिक्त;
    अन्यथा
	गाठ->rq_next->rq_prev = रिक्त;
    vim_free(गाठ);
    परत p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    स्थिर char_u *
channel_get_all(channel_T *channel, ch_part_T part, पूर्णांक *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *गाठ;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    साठी (गाठ = head->rq_next; गाठ != रिक्त; गाठ = गाठ->rq_next)
	len += गाठ->rq_buflen;
    res = alloc(len + 1);
    जर (res == रिक्त)
	परत रिक्त;
    p = res;
    साठी (गाठ = head->rq_next; गाठ != रिक्त; गाठ = गाठ->rq_next)
    {
	mch_memmove(p, गाठ->rq_buffer, गाठ->rq_buflen);
	p += गाठ->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    करा
    {
	p = channel_get(channel, part, रिक्त);
	vim_free(p);
    } पर्यंत (p != रिक्त);

    जर (outlen != रिक्त)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	परत res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    पर्यंत (p < res + len)
    {
	जर (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	अन्यथा जर (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    जर (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		पर्यंत (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		जर (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    परत res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    शून्य
channel_consume(channel_T *channel, ch_part_T part, पूर्णांक len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *गाठ = head->rq_next;
    char_u *buf = गाठ->rq_buffer;

    जर (len < 0 || (long_u)len > गाठ->rq_buflen)
	परत;
    mch_memmove(buf, buf + len, गाठ->rq_buflen - len);
    गाठ->rq_buflen -= len;
    गाठ->rq_buffer[गाठ->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    पूर्णांक
channel_collapse(channel_T *channel, ch_part_T part, पूर्णांक want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*गाठ = head->rq_next;
    readq_T	*last_गाठ;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    जर (गाठ == रिक्त || गाठ->rq_next == रिक्त)
	परत FAIL;

    last_गाठ = गाठ->rq_next;
    len = गाठ->rq_buflen + last_गाठ->rq_buflen;
    जर (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	पर्यंत (last_गाठ->rq_next != रिक्त
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_गाठ) == रिक्त))
	{
	    last_गाठ = last_गाठ->rq_next;
	    len += last_गाठ->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    जर (newbuf == रिक्त)
	परत FAIL;	    // out of memory
    mch_memmove(p, गाठ->rq_buffer, गाठ->rq_buflen);
    p += गाठ->rq_buflen;
    vim_free(गाठ->rq_buffer);
    गाठ->rq_buffer = newbuf;
    साठी (n = गाठ; n != last_गाठ; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    गाठ->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    साठी (n = गाठ->rq_next; n != last_गाठ; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    गाठ->rq_next = last_गाठ->rq_next;
    जर (last_गाठ->rq_next == रिक्त)
	head->rq_prev = गाठ;
    अन्यथा
	last_गाठ->rq_next->rq_prev = गाठ;
    vim_free(last_गाठ);
    परत OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णांक
channel_save(channel_T *channel, ch_part_T part, char_u *buf, पूर्णांक len,
						      पूर्णांक prepend, अक्षर *lead)
{
    readq_T *गाठ;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    पूर्णांक	    i;

    गाठ = ALLOC_ONE(readq_T);
    जर (गाठ == रिक्त)
	परत FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    गाठ->rq_buffer = alloc(len + 1);
    जर (गाठ->rq_buffer == रिक्त)
    {
	vim_free(गाठ);
	परत FAIL;	    // out of memory
    }

    जर (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = गाठ->rq_buffer;
	साठी (i = 0; i < len; ++i)
	    जर (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	गाठ->rq_buflen = (long_u)(p - गाठ->rq_buffer);
    }
    अन्यथा
    {
	mch_memmove(गाठ->rq_buffer, buf, len);
	गाठ->rq_buffer[len] = NUL;
	गाठ->rq_buflen = (long_u)len;
    }

    जर (prepend)
    {
	// prepend node to the head of the queue
	गाठ->rq_next = head->rq_next;
	गाठ->rq_prev = रिक्त;
	जर (head->rq_next == रिक्त)
	    head->rq_prev = गाठ;
	अन्यथा
	    head->rq_next->rq_prev = गाठ;
	head->rq_next = गाठ;
    }
    अन्यथा
    {
	// append node to the tail of the queue
	गाठ->rq_next = रिक्त;
	गाठ->rq_prev = head->rq_prev;
	जर (head->rq_prev == रिक्त)
	    head->rq_next = गाठ;
	अन्यथा
	    head->rq_prev->rq_next = गाठ;
	head->rq_prev = गाठ;
    }

    जर (ch_log_active() && lead != रिक्त)
	ch_log_literal(lead, channel, part, buf, len);

    परत OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    पूर्णांक
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, रिक्त);
    पूर्णांक		keeplen;
    पूर्णांक		addlen;
    char_u	*p;

    जर (next == रिक्त)
	परत FALSE;

    keeplen = reader->js_end - reader->js_buf;
    जर (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (पूर्णांक)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	जर (p == रिक्त)
	{
	    vim_free(next);
	    परत FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    परत TRUE;
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
    स्थिर पूर्णांक
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*ओळ_start;
    char_u	*p;
    int_u	hdr_len;
    पूर्णांक		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    पर्यंत (TRUE)
    {
	ओळ_start = p;
	पर्यंत (*p != NUL && *p != '\n')
	    p++;
	जर (*p == NUL)			// partial header
	    परत MAYBE;
	p++;

	// process the content length field (if present)
	जर ((p - ओळ_start > 16)
		&& STRNICMP(ओळ_start, "Content-Length: ", 16) == 0)
	{
	    त्रुटि_सं = 0;
	    payload_len = म_से_दीर्घ((अक्षर *)ओळ_start + 16, रिक्त, 10);
	    जर (त्रुटि_सं == दुस्फल || payload_len < 0)
		// invalid length, discard the payload
		परत FAIL;
	}

	जर ((p - ओळ_start) == 2 && ओळ_start[0] == '\r' &&
		ओळ_start[1] == '\n')
	    // reached the empty line
	    थांबा;
    }

    जर (payload_len == -1)
	// Content-Length field is not present in the header
	परत FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    जर (jsbuf_len < hdr_len + payload_len)
	परत MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    परत OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    पूर्णांक
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    पूर्णांक		status = OK;
    पूर्णांक		ret;

    जर (channel_peek(channel, part) == रिक्त)
	परत FALSE;

    reader.js_buf = channel_get(channel, part, रिक्त);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    जर (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    जर (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    जर (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	जर (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	अन्यथा जर (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    जर (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    अन्यथा
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	अन्यथा
	{
	    item = ALLOC_ONE(jsonq_T);
	    जर (item == रिक्त)
		clear_tv(&listtv);
	    अन्यथा
	    {
		item->jq_no_callback = FALSE;
		item->jq_मूल्य = alloc_tv();
		जर (item->jq_मूल्य == रिक्त)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		अन्यथा
		{
		    *item->jq_मूल्य = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = रिक्त;
		    जर (item->jq_prev == रिक्त)
			head->jq_next = item;
		    अन्यथा
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    जर (status == OK)
	chanpart->ch_wait_len = 0;
    अन्यथा जर (status == MAYBE)
    {
	माप_प्रकार buflen = STRLEN(reader.js_buf);

	जर (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (पूर्णांक)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_मोजणी() + 100L;
#अन्यथा
	    gettimeofday(&chanpart->ch_deadline, रिक्त);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    जर (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	अन्यथा
	{
	    पूर्णांक timeout;
#ifdef MSWIN
	    timeout = (पूर्णांक)(Get_Tick_मोजणी() - chanpart->ch_deadline) > 0;
#अन्यथा
	    {
		संरचना timeval now_tv;

		gettimeofday(&now_tv, रिक्त);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    जर (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    अन्यथा
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    जर (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    अन्यथा जर (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(पूर्णांक)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, रिक्त);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    अन्यथा
	ret = FALSE;

    vim_free(reader.js_buf);
    परत ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    स्थिर शून्य
remove_cb_गाठ(cbq_T *head, cbq_T *गाठ)
{
    जर (गाठ->cq_prev == रिक्त)
	head->cq_next = गाठ->cq_next;
    अन्यथा
	गाठ->cq_prev->cq_next = गाठ->cq_next;
    जर (गाठ->cq_next == रिक्त)
	head->cq_prev = गाठ->cq_prev;
    अन्यथा
	गाठ->cq_next->cq_prev = गाठ->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    शून्य
remove_json_गाठ(jsonq_T *head, jsonq_T *गाठ)
{
    जर (गाठ->jq_prev == रिक्त)
	head->jq_next = गाठ->jq_next;
    अन्यथा
	गाठ->jq_prev->jq_next = गाठ->jq_next;
    जर (गाठ->jq_next == रिक्त)
	head->jq_prev = गाठ->jq_prev;
    अन्यथा
	गाठ->jq_next->jq_prev = गाठ->jq_prev;
    vim_free(गाठ);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    स्थिर शून्य
channel_add_block_id(chanpart_T *chanpart, पूर्णांक id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    जर (gap->ga_growsize == 0)
	ga_init2(gap, आकार(पूर्णांक), 10);
    जर (ga_grow(gap, 1) == OK)
    {
	((पूर्णांक *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    स्थिर शून्य
channel_remove_block_id(chanpart_T *chanpart, पूर्णांक id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    पूर्णांक		i;

    साठी (i = 0; i < gap->ga_len; ++i)
	जर (((पूर्णांक *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    जर (i < gap->ga_len)
	    {
		पूर्णांक *p = ((पूर्णांक *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * आकार(पूर्णांक));
	    }
	    परत;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    स्थिर पूर्णांक
channel_has_block_id(chanpart_T *chanpart, पूर्णांक id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    पूर्णांक		i;

    साठी (i = 0; i < gap->ga_len; ++i)
	जर (((पूर्णांक *)gap->ga_data)[i] == id)
	    परत TRUE;
    परत FALSE;
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
    स्थिर पूर्णांक
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	पूर्णांक	    id,
	पूर्णांक	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    पर्यंत (item != रिक्त)
    {
	list_T	    *l;
	typval_T    *tv;

	जर (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_मूल्य->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	अन्यथा
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    जर (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		जर (id > 0)
		{
		    जर (item->jq_मूल्य->v_type != VAR_DICT)
			जा nextitem;
		    d = item->jq_मूल्य->vval.v_dict;
		    जर (d == रिक्त)
			जा nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    जर (dict_has_किल्ली(d, "method"))
			जा nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    जर (di == रिक्त)
			जा nextitem;
		    tv = &di->di_tv;
		}
		अन्यथा
		    tv = item->jq_मूल्य;
	    }
	    अन्यथा
	    {
		जर (id > 0)
		{
		    जर (item->jq_मूल्य->v_type != VAR_DICT)
			जा nextitem;
		    d = item->jq_मूल्य->vval.v_dict;
		    जर (d == रिक्त)
			जा nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    जर (di == रिक्त)
			जा nextitem;
		    tv = &di->di_tv;
		}
		अन्यथा
		    tv = item->jq_मूल्य;
	    }
	}

	जर ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_संख्या && tv->vval.v_संख्या == id)
	      || (id <= 0 && (tv->v_type != VAR_संख्या
		 || tv->vval.v_संख्या == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_संख्या)))))
	{
	    *rettv = item->jq_मूल्य;
	    जर (tv->v_type == VAR_संख्या)
		ch_log(channel, "Getting JSON message %ld",
						      (दीर्घ)tv->vval.v_संख्या);
	    remove_json_गाठ(head, item);
	    परत OK;
	}
nextitem:
	item = item->jq_next;
    }
    परत FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    स्थिर शून्य
channel_ढकला_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    जर (head->jq_prev != रिक्त && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = रिक्त;
    अन्यथा पर्यंत (item != रिक्त && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    जर (newitem == रिक्त)
    {
	clear_tv(rettv);
	परत;
    }

    newitem->jq_मूल्य = alloc_tv();
    जर (newitem->jq_मूल्य == रिक्त)
    {
	vim_free(newitem);
	clear_tv(rettv);
	परत;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_मूल्य = *rettv;
    जर (item == रिक्त)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = रिक्त;
	जर (newitem->jq_prev == रिक्त)
	    head->jq_next = newitem;
	अन्यथा
	    newitem->jq_prev->jq_next = newitem;
    }
    अन्यथा
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	जर (newitem->jq_next == रिक्त)
	    head->jq_prev = newitem;
	अन्यथा
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
    पूर्णांक	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    जर (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	जर (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	परत;
    }
    arg = argv[1].vval.v_string;
    जर (arg == रिक्त)
	arg = (char_u *)"";

    जर (STRCMP(cmd, "ex") == 0)
    {
	पूर्णांक	called_emsg_before = called_emsg;
	char_u	*p = arg;
	पूर्णांक	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (अक्षर *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	जर (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	जर (do_emsg_silent)
	    --emsg_silent;
	जर (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (अक्षर *)get_vim_var_str(VV_ERRMSG));
    }
    अन्यथा जर (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (अक्षर *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_मोजणी = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    अन्यथा जर (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    अन्यथा जर (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	पूर्णांक is_call = cmd[0] == 'c';
	पूर्णांक id_idx = is_call ? 3 : 2;

	जर (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_संख्या)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    जर (p_verbose > 2)
		emsg(_(e_last_argument_for_अभिव्यक्ती_call_must_be_संख्या));
	}
	अन्यथा जर (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    जर (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	अन्यथा
	{
	    typval_T	*tv = रिक्त;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = रिक्त;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    जर (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (अक्षर *)arg);
		tv = eval_अभिव्यक्ती(arg, रिक्त);
	    }
	    अन्यथा
	    {
		ch_log(channel, "Calling '%s'", (अक्षर *)arg);
		जर (func_call(arg, &argv[2], रिक्त, रिक्त, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    जर (argv[id_idx].v_type == VAR_संख्या)
	    {
		पूर्णांक id = argv[id_idx].vval.v_संख्या;

		जर (tv != रिक्त)
		    json = json_encode_nr_अभिव्यक्ती(id, tv, options | JSON_NL);
		जर (tv == रिक्त || (json != रिक्त && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_अभिव्यक्ती(id, &err_tv, options | JSON_NL);
		}
		जर (json != रिक्त)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (पूर्णांक)STRLEN(json), (अक्षर *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    जर (tv == &res_tv)
		clear_tv(tv);
	    अन्यथा
		free_tv(tv);
	}
    }
    अन्यथा जर (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (अक्षर *)cmd);
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
					    (अक्षर *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_गाठ(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    स्थिर शून्य
append_to_buffer(
    buf_T	*buffer,
    char_u	*संदेश,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_ओळ_मोजणी;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    पूर्णांक		save_p_ma = buffer->b_p_ma;
    पूर्णांक		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    जर (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	जर (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	परत;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    जर (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (पूर्णांक)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    जर (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	परत;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    जर (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, संदेश, TRUE);
	lnum = 0;
    }
    अन्यथा
	ml_append(lnum, संदेश, 0, FALSE);
    appended_ओळी_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    जर (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    अन्यथा
	buffer->b_p_ma = save_p_ma;

    जर (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    जर (wp->w_buffer == buffer)
	    {
		पूर्णांक move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		जर (move_cursor || wp->w_topline > buffer->b_ml.ml_ओळ_मोजणी)
		{
		    win_T *save_curwin = curwin;

		    जर (move_cursor)
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

    जर (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    जर (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_ओळ_मोजणी;
	}
    }
}

    स्थिर शून्य
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *संदेश;

    पर्यंत ((संदेश = channel_get(channel, part, रिक्त)) != रिक्त)
    {
	ch_log(channel, "Dropping message '%s'", (अक्षर *)संदेश);
	vim_free(संदेश);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    स्थिर पूर्णांक
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    परत ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    स्थिर पूर्णांक
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*संदेश = रिक्त;
    blob_T	*blob = रिक्त;
    typval_T	*listtv = रिक्त;
    typval_T	argv[CH_JSON_MAX_ARGS];
    पूर्णांक		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = रिक्त;
    buf_T	*buffer = रिक्त;
    char_u	*p;
    पूर्णांक		called_otc;		// one time callbackup
    पूर्णांक		raw_len = 0;

    जर (channel->ch_nb_close_cb != रिक्त
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	परत FALSE;

    // Use a message-specific callback, part callback or channel callback
    साठी (cbitem = cbhead->cq_next; cbitem != रिक्त; cbitem = cbitem->cq_next)
	जर (cbitem->cq_seq_nr == 0)
	    थांबा;
    जर (cbitem != रिक्त)
	callback = &cbitem->cq_callback;
    अन्यथा जर (ch_part->ch_callback.cb_name != रिक्त)
	callback = &ch_part->ch_callback;
    अन्यथा जर (channel->ch_callback.cb_name != रिक्त)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    जर (buffer != रिक्त && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == रिक्त))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = रिक्त;
	buffer = रिक्त;
    }

    जर (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	पूर्णांक		argc = 0;

	// Get any json message in the queue.
	जर (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    जर (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(शून्य)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    जर (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		परत FALSE;
	}

	जर (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    जर (d != रिक्त)
	    {
		जर (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		अन्यथा
		    di = dict_find(d, (char_u *)"request_seq", -1);
		जर (di != रिक्त && di->di_tv.v_type == VAR_संख्या)
		    seq_nr = di->di_tv.vval.v_संख्या;
	    }

	    argv[1] = *listtv;
	}
	अन्यथा
	{
	    साठी (item = listtv->vval.v_list->lv_first;
		    item != रिक्त && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    पर्यंत (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    जर (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		परत TRUE;
	    }

	    जर (argv[0].v_type != VAR_संख्या)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		परत FALSE;
	    }
	    seq_nr = argv[0].vval.v_संख्या;
	}
    }
    अन्यथा जर (channel_peek(channel, part) == रिक्त)
    {
	// nothing to read on RAW or NL channel
	परत FALSE;
    }
    अन्यथा
    {
	// If there is no callback or buffer drop the message.
	जर (callback == रिक्त && buffer == रिक्त)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    जर (channel->ch_close_cb.cb_name == रिक्त && !channel->ch_drop_never)
		drop_messages(channel, part);
	    परत FALSE;
	}

	जर (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = रिक्त;
	    char_u  *buf;
	    readq_T *गाठ;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    पर्यंत (TRUE)
	    {
		गाठ = channel_peek(channel, part);
		nl = channel_first_nl(गाठ);
		जर (nl != रिक्त)
		    थांबा;
		जर (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    जर (ch_part->ch_fd == INVALID_FD && गाठ->rq_buflen > 0)
			थांबा;
		    परत FALSE; // incomplete message
		}
	    }
	    buf = गाठ->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    साठी (p = buf; (nl == रिक्त || p < nl)
					    && p < buf + गाठ->rq_buflen; ++p)
		जर (*p == NUL)
		    *p = NL;

	    जर (nl == रिक्त)
	    {
		// get the whole buffer, drop the NL
		संदेश = channel_get(channel, part, रिक्त);
	    }
	    अन्यथा जर (nl + 1 == buf + गाठ->rq_buflen)
	    {
		// get the whole buffer
		संदेश = channel_get(channel, part, रिक्त);
		*nl = NUL;
	    }
	    अन्यथा
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		संदेश = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (पूर्णांक)(nl - buf) + 1);
	    }
	}
	अन्यथा
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    संदेश = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : रिक्त);
	}

	जर (संदेश == रिक्त)
	    परत FALSE; // out of memory (and avoids Coverity warning)

	जर (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    जर (blob == रिक्त)
	    {
		vim_free(संदेश);
		परत FALSE;
	    }
	    जर (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(संदेश);
		परत FALSE;
	    }
	    जर (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, संदेश, (माप_प्रकार)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	अन्यथा
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = संदेश;
	}
    }

    called_otc = FALSE;
    जर (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	पूर्णांक lsp_req_संदेश = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	जर ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_किल्ली(argv[1].vval.v_dict, "method"))
	    lsp_req_संदेश = TRUE;

	जर (!lsp_req_संदेश)
	{
	    साठी (cbitem = cbhead->cq_next; cbitem != रिक्त;
		    cbitem = cbitem->cq_next)
	    {
		जर (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    थांबा;
		}
	    }
	}
    }

    जर (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	जर (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    जर (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ढकला_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_संख्या;
		free_tv(listtv);
		listtv = रिक्त;
	    }
	    अन्यथा
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    अन्यथा जर (callback != रिक्त || buffer != रिक्त)
    {
	जर (buffer != रिक्त)
	{
	    जर (संदेश == रिक्त)
		// JSON or JS mode: re-encode the message.
		संदेश = json_encode(listtv, ch_mode);
	    जर (संदेश != रिक्त)
	    {
#ifdef FEAT_TERMINAL
		जर (buffer->b_term != रिक्त)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    जर (part == PART_ERR)
		    {
			char_u *cp;
			पूर्णांक    extra = 0;

			साठी (cp = संदेश; *cp != NUL; ++cp)
			    जर (*cp == NL && (cp == संदेश || cp[-1] != CAR))
				++extra;
			जर (extra > 0)
			{
			    char_u *crlf_संदेश = alloc(STRLEN(संदेश) + extra + 1);

			    जर (crlf_संदेश != रिक्त)
			    {
				char_u *q = crlf_संदेश;

				साठी (cp = संदेश; *cp != NUL; ++cp)
				{
				    जर (*cp == NL && (cp == संदेश || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_संदेश, channel);
				vim_free(crlf_संदेश);
			    }
			}
			अन्यथा
			    write_to_term(buffer, संदेश, channel);
		    }
		    अन्यथा
			write_to_term(buffer, संदेश, channel);
		}
		अन्यथा
#endif
		    append_to_buffer(buffer, संदेश, channel, part);
	    }
	}

	जर (callback != रिक्त)
	{
	    जर (cbitem != रिक्त)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    अन्यथा
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (अक्षर *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    अन्यथा
	ch_log(channel, "Dropping message %d", seq_nr);

    जर (listtv != रिक्त)
	free_tv(listtv);
    जर (blob != रिक्त)
	blob_unref(blob);
    vim_free(संदेश);

    परत TRUE;
}

#जर defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    पूर्णांक
channel_can_write_to(channel_T *channel)
{
    परत channel != रिक्त && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    पूर्णांक
channel_is_open(channel_T *channel)
{
    परत channel != रिक्त && (channel->CH_SOCK_FD != INVALID_FD
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
    जर (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	जर (head->jq_next == रिक्त)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	परत head->jq_next;
    }
    परत channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    स्थिर पूर्णांक
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    परत channel_readahead_pointer(channel, part) != रिक्त;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    स्थिर अक्षर *
channel_status(channel_T *channel, पूर्णांक req_part)
{
    ch_part_T part;
    पूर्णांक has_readahead = FALSE;

    जर (channel == रिक्त)
	 परत "fail";
    जर (req_part == PART_OUT)
    {
	जर (channel->CH_OUT_FD != INVALID_FD)
	    परत "open";
	जर (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    अन्यथा जर (req_part == PART_ERR)
    {
	जर (channel->CH_ERR_FD != INVALID_FD)
	    परत "open";
	जर (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    अन्यथा
    {
	जर (channel_is_open(channel))
	    परत "open";
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	    जर (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		थांबा;
	    }
    }

    जर (has_readahead)
	परत "buffered";
    परत "closed";
}

    स्थिर शून्य
channel_part_info(channel_T *channel, dict_T *dict, अक्षर *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    अक्षर	namebuf[20];  // longest is "sock_timeout"
    माप_प्रकार	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    जर (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    अन्यथा जर (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    अन्यथा
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (पूर्णांक)s.length);

    STRCPY(namebuf + tail, "mode");
    निवड (chanpart->ch_mode)
    {
	स्थिती CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    थांबा;
	स्थिती CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    थांबा;
	स्थिती CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    थांबा;
	स्थिती CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    थांबा;
	स्थिती CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    थांबा;
	स्थिती CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    थांबा;
	स्थिती CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    थांबा;
	पूर्वनिर्धारित:
	    STR_LITERAL_SET(s, "");
	    थांबा;
    }
    dict_add_string_len(dict, namebuf, s.string, (पूर्णांक)s.length);

    STRCPY(namebuf + tail, "io");
    जर (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    अन्यथा निवड (chanpart->ch_io)
    {
	स्थिती JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    थांबा;
	स्थिती JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    थांबा;
	स्थिती JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    थांबा;
	स्थिती JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    थांबा;
	स्थिती JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    थांबा;
	पूर्वनिर्धारित:
	    STR_LITERAL_SET(s, "");
	    थांबा;
    }
    dict_add_string_len(dict, namebuf, s.string, (पूर्णांक)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_संख्या(dict, namebuf, chanpart->ch_timeout);
}

    स्थिर शून्य
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_संख्या(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    जर (channel->ch_hostname != रिक्त)
    {
	जर (channel->ch_port)
	    dict_add_संख्या(dict, "port", channel->ch_port);
	अन्यथा
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    अन्यथा
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
channel_close(channel_T *channel, पूर्णांक invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    जर (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	जर (channel->ch_close_cb.cb_name != रिक्त)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	{
	    जर (channel->ch_close_cb.cb_name != रिक्त
			    || channel->ch_part[part].ch_bufref.br_buf != रिक्त)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		जर (channel->ch_close_cb.cb_name == रिक्त)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		पर्यंत (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	जर (channel->ch_close_cb.cb_name != रिक्त)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (अक्षर *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      जर (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      जर (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  साठी (part = PART_SOCK; part < PART_IN; ++part)
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
    जर (wq->wq_next == रिक्त)
	wq->wq_prev = रिक्त;
    अन्यथा
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

    पर्यंत (channel_peek(channel, part) != रिक्त)
	vim_free(channel_get(channel, part, रिक्त));

    पर्यंत (cb_head->cq_next != रिक्त)
    {
	cbq_T *गाठ = cb_head->cq_next;

	remove_cb_गाठ(cb_head, गाठ);
	free_callback(&गाठ->cq_callback);
	vim_free(गाठ);
    }

    पर्यंत (json_head->jq_next != रिक्त)
    {
	free_tv(json_head->jq_next->jq_मूल्य);
	remove_json_गाठ(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    पर्यंत (ch_part->ch_writeque.wq_next != रिक्त)
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

#जर defined(EXITFREE)
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
#define DETACH_संदेश_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    स्थिर पूर्णांक
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    जर (in_part->ch_writeque.wq_next != रिक्त)
	परत TRUE;
    जर (buf == रिक्त)
	परत FALSE;
    परत in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_ओळ_मोजणी)
	    : (in_part->ch_buf_शीर्ष <= in_part->ch_buf_bot
			    && in_part->ch_buf_शीर्ष <= buf->b_ml.ml_ओळ_मोजणी);
}

#जर defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    स्थिर पूर्णांक
channel_fill_wfds(पूर्णांक maxfd_arg, fd_set *wfds)
{
    पूर्णांक		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	जर (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (पूर्णांक)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((पूर्णांक)in_part->ch_fd, wfds);
	    जर ((पूर्णांक)in_part->ch_fd >= maxfd)
		maxfd = (पूर्णांक)in_part->ch_fd + 1;
	}
    }
    परत maxfd;
}
#अन्यथा
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    स्थिर पूर्णांक
channel_fill_poll_write(पूर्णांक nfd_in, संरचना pollfd *fds)
{
    पूर्णांक		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	जर (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	अन्यथा
	    in_part->ch_poll_idx = -1;
    }
    परत nfd;
}
#endif

प्रकारव्याख्या गणना {
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
channel_wait(channel_T *channel, sock_T fd, पूर्णांक timeout)
{
    जर (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    जर (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	पूर्णांक	sleep_time;
	DWORD	deadline = Get_Tick_मोजणी() + timeout;
	पूर्णांक	delay = 1;

	// reading from a pipe, not a socket
	पर्यंत (TRUE)
	{
	    पूर्णांक r = PeekNamedPipe((HANDLE)fd, रिक्त, 0, रिक्त, &nread, रिक्त);

	    जर (r && nread > 0)
		परत CW_READY;

	    जर (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, रिक्त);
	    }
	    अन्यथा जर (r == 0)
		परत CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_ओळी();

	    sleep_time = deadline - Get_Tick_मोजणी();
	    जर (sleep_time <= 0)
		थांबा;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    जर (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    जर (delay > 10)
		delay = 10;
	}
    }
    अन्यथा
#endif
    {
#जर defined(HAVE_SELECT)
	संरचना timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	पूर्णांक		ret;
	पूर्णांक		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	साठी (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((पूर्णांक)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (पूर्णांक)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, रिक्त, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    जर (ret == -1 && त्रुटि_सं == EINTR)
		सुरूठेवा;
# endif
	    जर (ret > 0)
	    {
		जर (FD_ISSET(fd, &rfds))
		    परत CW_READY;
		channel_write_any_ओळी();
		सुरूठेवा;
	    }
	    थांबा;
	}
#अन्यथा
	साठी (;;)
	{
	    संरचना pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    पूर्णांक		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    जर (poll(fds, nfd, timeout) > 0)
	    {
		जर (fds[0].revents & POLLIN)
		    परत CW_READY;
		channel_write_any_ओळी();
		सुरूठेवा;
	    }
	    थांबा;
	}
#endif
    }
    परत CW_NOT_READY;
}

    स्थिर शून्य
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, पूर्णांक is_err, अक्षर *func)
{
    अक्षर	संदेश[] = "%s(): Read %s from ch_part[%d], closing";

    जर (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, संदेश, func, "error", part);
    अन्यथा
	ch_log(channel, संदेश, func, "EOF", part);

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
    जर (channel->ch_nb_close_cb != रिक्त)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_संदेश_RAW,
			      (पूर्णांक)STRLEN_LITERAL(DETACH_संदेश_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    जर (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	जर (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
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
    जर (channel->ch_nb_close_cb != रिक्त)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    जर (channel->ch_ss_close_cb != रिक्त)
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
channel_read(channel_T *channel, ch_part_T part, अक्षर *func)
{
    स्थिर char_u	*buf = रिक्त;
    पूर्णांक			len = 0;
    पूर्णांक			readlen = 0;
    sock_T		fd;
    पूर्णांक			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    जर (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	परत;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    जर (buf == रिक्त)
    {
	buf = alloc(MAXMSGSIZE);
	जर (buf == रिक्त)
	    परत;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    साठी (;;)
    {
	जर (channel_wait(channel, fd, 0) != CW_READY)
	    थांबा;
	जर (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    संरचना sockaddr_storage	client;

	    newchannel = add_channel();
	    जर (newchannel == रिक्त)
	    {
		ch_error(रिक्त, "Cannot allocate channel.");
		परत;
	    }
	    socklen = आकार(client);
	    newfd = accept(fd, (संरचना sockaddr*)&client, &socklen);
	    जर (newfd < 0)
	    {
		ch_error(रिक्त, "Cannot accept channel.");
		channel_free(newchannel);
		परत;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    जर (channel->ch_ss_accept_cb != रिक्त)
	    {
		channel->ch_ss_accept_cb(newchannel);
		परत;
	    }
#endif

	    जर (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		अक्षर addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((संरचना sockaddr_in*)&client)->sin_addr,
			addr, आकार(addr));
		vim_snprintf((अक्षर *)namebuf, आकार(namebuf), "%s:%d",
			addr,
			ntohs(((संरचना sockaddr_in*)&client)->sin_port));
#अन्यथा
		vim_snprintf((अक्षर *)namebuf, आकार(namebuf), "%s:%d",
		    inet_ntoa(((संरचना sockaddr_in*)&client)->sin_addr),
		    ntohs(((संरचना sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    अन्यथा जर (client.ss_family == AF_INET6)
	    {
		अक्षर addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((संरचना sockaddr_in6*)&client)->sin6_addr,
			addr, आकार(addr));
		vim_snprintf((अक्षर *)namebuf, आकार(namebuf), "[%s]:%d",
			addr,
			ntohs(((संरचना sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    अन्यथा जर (client.ss_family == AF_UNIX)
		vim_snprintf((अक्षर *)namebuf, आकार(namebuf),
							    "unix:anonymous");
	    अन्यथा
		vim_snprintf((अक्षर *)namebuf, आकार(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    परत;
	}
	जर (use_socket)
	    len = sock_read(fd, (अक्षर *)buf, MAXMSGSIZE);
	अन्यथा
	    len = fd_read(fd, (अक्षर *)buf, MAXMSGSIZE);
	जर (len <= 0)
	    थांबा;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    जर (readlen <= 0)
    {
	जर (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#जर defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    अन्यथा जर (CH_HAS_GUI && gtk_main_level() > 0)
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
	channel_T *channel, ch_part_T part, पूर्णांक timeout, पूर्णांक raw, पूर्णांक *outlen)
{
    char_u	*buf;
    char_u	*संदेश;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*गाठ;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    पर्यंत (TRUE)
    {
	गाठ = channel_peek(channel, part);
	जर (गाठ != रिक्त)
	{
	    जर (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(गाठ) != रिक्त))
		// got a complete message
		थांबा;
	    जर (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		सुरूठेवा;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    जर (raw || fd == INVALID_FD)
		थांबा;
	}

	// Wait for up to the channel timeout.
	जर (fd == INVALID_FD)
	    परत रिक्त;
	जर (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    परत रिक्त;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    जर (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != रिक्त)
    {
	संदेश = channel_get_all(channel, part, outlen);
    }
    अन्यथा
    {
	char_u *p;

	buf = गाठ->rq_buffer;
	nl = channel_first_nl(गाठ);

	// Convert NUL to NL, the internal representation.
	साठी (p = buf; (nl == रिक्त || p < nl) && p < buf + गाठ->rq_buflen; ++p)
	    जर (*p == NUL)
		*p = NL;

	जर (nl == रिक्त)
	{
	    // must be a closed channel with missing NL
	    संदेश = channel_get(channel, part, रिक्त);
	}
	अन्यथा जर (nl + 1 == buf + गाठ->rq_buflen)
	{
	    // get the whole buffer
	    संदेश = channel_get(channel, part, रिक्त);
	    *nl = NUL;
	}
	अन्यथा
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    संदेश = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (पूर्णांक)(nl - buf) + 1);
	}
    }
    जर (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != रिक्त ? *outlen : (पूर्णांक)STRLEN(संदेश));
    परत संदेश;
}

स्थिर पूर्णांक channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    पूर्णांक
channel_in_blocking_wait(शून्य)
{
    परत channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    स्थिर शून्य
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, पूर्णांक timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (शून्य)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#अन्यथा
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
    स्थिर पूर्णांक
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	पूर्णांक	    timeout_arg,
	पूर्णांक	    id,
	typval_T    **rettv)
{
    पूर्णांक		more;
    sock_T	fd;
    पूर्णांक		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    पूर्णांक		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    जर (id >= 0)
	channel_add_block_id(chanpart, id);

    साठी (;;)
    {
	जर (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (शून्य)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	जर (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    थांबा;
	}

	जर (!more)
	{
	    शून्य *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    शून्य *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    जर (channel_parse_messages())
		सुरूठेवा;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    जर (readahead_ptr != रिक्त && readahead_ptr != prev_readahead_ptr)
		सुरूठेवा;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (पूर्णांक)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    जर (timeout <= 0 || fd == INVALID_FD)
	    {
		जर (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		थांबा;
	    }
	    // If there was an incomplete message use the deadline for that.
	    जर (chanpart->ch_wait_len > 0)
	    {
		पूर्णांक	डावा;
#ifdef MSWIN
		डावा = chanpart->ch_deadline - Get_Tick_मोजणी() + 1;
#अन्यथा
		{
		    संरचना timeval now_tv;

		    gettimeofday(&now_tv, रिक्त);
		    डावा = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		जर (डावा < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		अन्यथा जर (डावा < timeout)
		    timeout = डावा;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    जर (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    परत retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, पूर्णांक check_open, पूर्णांक reading, ch_part_T part)
{
    channel_T	*channel = रिक्त;
    पूर्णांक		has_readahead = FALSE;

    जर (tv->v_type == VAR_JOB)
    {
	जर (tv->vval.v_job != रिक्त)
	    channel = tv->vval.v_job->jv_channel;
    }
    अन्यथा जर (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    अन्यथा
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	परत रिक्त;
    }
    जर (channel != रिक्त && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_मोजणी ? part : channel_part_read(channel));

    जर (check_open && (channel == रिक्त || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	परत रिक्त;
    }
    परत channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    स्थिर शून्य
common_channel_read(typval_T *argvars, typval_T *rettv, पूर्णांक raw, पूर्णांक blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_मोजणी;
    jobopt_T	opt;
    पूर्णांक		mode;
    पूर्णांक		timeout;
    पूर्णांक		id = -1;
    typval_T	*listtv = रिक्त;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    clear_job_options(&opt);
    जर (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	जा theend;

    जर (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    जर (channel == रिक्त)
	जा theend;

    जर (part == PART_मोजणी)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    जर (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    जर (blob || mode == CH_MODE_BLOB)
    {
	पूर्णांक	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	जर (p != रिक्त)
	{
	    blob_T	*b = blob_alloc();

	    जर (b != रिक्त)
	    {
		b->bv_ga.ga_len = outlen;
		जर (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		अन्यथा
		{
		    स_नकल(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    अन्यथा जर (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, रिक्त);
    अन्यथा
    {
	जर (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	जर (listtv != रिक्त)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	अन्यथा
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_संख्या = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#जर defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    शून्य
channel_handle_events(पूर्णांक only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	जर (only_keep_open && !channel->ch_keep_open)
	    सुरूठेवा;

	// check the socket and pipes
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    जर (fd == INVALID_FD)
		सुरूठेवा;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    पूर्णांक r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    जर (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    अन्यथा जर (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	जर (channel->ch_job != रिक्त)
	{
	    job_T *job = channel->ch_job;

	    जर (job->jv_tty_out != रिक्त && job->jv_status == JOB_FINISHED)
		साठी (part = PART_SOCK; part < PART_मोजणी; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#जर defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    पूर्णांक
channel_any_keep_open(शून्य)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	जर (channel->ch_keep_open)
	    परत TRUE;
    परत FALSE;
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
    पूर्णांक		fd = ch_part->ch_fd;

    जर (fd == INVALID_FD)
	परत;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#अन्यथा
    (शून्य)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    पूर्णांक
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	पूर्णांक	  len_arg,
	अक्षर	  *fun)
{
    पूर्णांक		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    पूर्णांक		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    जर (fd == INVALID_FD)
    {
	जर (!channel->ch_error && fun != रिक्त)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	परत FAIL;
    }

    जर (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    जर (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_संदेश = 0;
    }

    साठी (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	पूर्णांक	    len;

	जर (wq->wq_next != रिक्त)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	अन्यथा
	{
	    जर (len_arg == 0)
		// nothing to write, called from channel_select_check()
		परत OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	जर (part == PART_SOCK)
	    res = sock_write(fd, (अक्षर *)buf, len);
	अन्यथा
	{
	    res = fd_write(fd, (अक्षर *)buf, len);
#ifdef MSWIN
	    जर (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, रिक्त);
	    }
#endif
	}
	जर (res < 0 && (त्रुटि_सं == EWOULDBLOCK
#ifdef EAGAIN
			|| त्रुटि_सं == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	जर (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    जर (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    जर (res == len)
	    {
		// Wrote all the buf[len] bytes.
		जर (entry != रिक्त)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    सुरूठेवा;
		}
		जर (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    अन्यथा
	    {
		// Wrote only buf[res] bytes, can't write more now.
		जर (entry != रिक्त)
		{
		    जर (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (अक्षर *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		अन्यथा
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		जर (wq->wq_prev != रिक्त
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    जर (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((अक्षर *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		अन्यथा
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    जर (last != रिक्त)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = रिक्त;
			जर (wq->wq_prev == रिक्त)
			    wq->wq_next = last;
			अन्यथा
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			जर (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	अन्यथा जर (res != len)
	{
	    जर (!channel->ch_error && fun != रिक्त)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    परत FAIL;
	}

	channel->ch_error = FALSE;
	परत OK;
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
	पूर्णांक	    len,
	पूर्णांक	    id,
	पूर्णांक	    eval,
	jobopt_T    *opt,
	अक्षर	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जर (channel == रिक्त)
	परत रिक्त;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    जर (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	परत रिक्त;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    जर (opt->jo_callback.cb_name != रिक्त && *opt->jo_callback.cb_name != NUL)
    {
	जर (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    परत रिक्त;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    जर (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == रिक्त)
	परत channel;
    परत रिक्त;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    स्थिर शून्य
ch_अभिव्यक्ती_common(typval_T *argvars, typval_T *rettv, पूर्णांक eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    पूर्णांक		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    पूर्णांक		timeout;
    पूर्णांक		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	परत;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जर (channel == रिक्त)
	परत;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    जर (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	परत;
    }

    जर (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	अक्षर		*किल्ली = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	जर (rettv_dict_alloc(rettv) == FAIL)
	    परत;

	जर (check_for_dict_arg(argvars, 1) == FAIL)
	    परत;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)किल्ली, -1);
	जर (argvars[2].v_type == VAR_DICT
		&& dict_has_किल्ली(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	जर (di != रिक्त && di->di_tv.v_type != VAR_संख्या
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_मूल्य_for_argument_str), किल्ली);
	    परत;
	}

	जर (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_संदेश_id;
	    जर (di == रिक्त)
		dict_add_संख्या(d, "seq", id);
	    अन्यथा
		di->di_tv.vval.v_संख्या = id;
	}
	अन्यथा जर (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_संदेश_id;
	    जर (di == रिक्त)
		dict_add_संख्या(d, "id", id);
	    अन्यथा
		di->di_tv.vval.v_संख्या = id;
	}
	अन्यथा
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    जर (di != रिक्त && di->di_tv.v_type == VAR_संख्या)
		id = di->di_tv.vval.v_संख्या;
	}
	जर (ch_mode == CH_MODE_LSP && !dict_has_किल्ली(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_संदेश(&argvars[1]);
    }
    अन्यथा
    {
	id = ++channel->ch_last_संदेश_id;
	text = json_encode_nr_अभिव्यक्ती(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    जर (text == रिक्त)
	परत;

    channel = send_common(argvars, text, (पूर्णांक)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    जर (channel != रिक्त && eval)
    {
	जर (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	अन्यथा
	    timeout = channel_get_timeout(channel, part_read);
	जर (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    जर (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_संख्या;
		free_tv(listtv);
	    }
	    अन्यथा
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_संख्या;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    जर (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	जर (rettv->vval.v_dict != रिक्त)
	    dict_add_संख्या(rettv->vval.v_dict, "seq", id);
    }
    अन्यथा जर (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	जर (rettv->vval.v_dict != रिक्त)
	    dict_add_संख्या(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    स्थिर शून्य
ch_raw_common(typval_T *argvars, typval_T *rettv, पूर्णांक eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    पूर्णांक		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    पूर्णांक		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	परत;

    जर (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    अन्यथा
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (पूर्णांक)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    जर (channel != रिक्त && eval)
    {
	जर (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	अन्यथा
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, रिक्त);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#जर defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    पूर्णांक
channel_poll_setup(पूर्णांक nfd_in, शून्य *fds_in, पूर्णांक *towait)
{
    पूर्णांक		nfd = nfd_in;
    channel_T	*channel;
    संरचना	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    जर (ch_part->ch_fd != INVALID_FD)
	    {
		जर (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    जर (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		अन्यथा
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    अन्यथा
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    परत nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    पूर्णांक
channel_poll_check(पूर्णांक ret_in, शून्य *fds_in)
{
    पूर्णांक		ret = ret_in;
    channel_T	*channel;
    संरचना	pollfd *fds = fds_in;
    ch_part_T	part;
    पूर्णांक		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    जर (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    अन्यथा जर (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	जर (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_आदान(channel);
	    --ret;
	}
    }

    परत ret;
}
#endif // UNIX && !HAVE_SELECT

#जर !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    पूर्णांक
channel_select_setup(
	पूर्णांक maxfd_in,
	शून्य *rfds_in,
	शून्य *wfds_in,
	संरचना timeval *tv,
	संरचना timeval **tvp)
{
    पूर्णांक		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    जर (fd != INVALID_FD)
	    {
		जर (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    जर (*tvp == रिक्त || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		अन्यथा
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    जर ((पूर्णांक)fd >= FD_SETSIZE)
			सुरूठेवा;
# endif
		    FD_SET((पूर्णांक)fd, rfds);
		    जर (maxfd < (पूर्णांक)fd)
			maxfd = (पूर्णांक)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    परत maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    पूर्णांक
channel_select_check(पूर्णांक ret_in, शून्य *rfds_in, शून्य *wfds_in)
{
    पूर्णांक		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	साठी (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    जर (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (पूर्णांक)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    अन्यथा जर (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	जर (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (पूर्णांक)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_आदान(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	जर (channel->ch_job != रिक्त)
	{
	    job_T *job = channel->ch_job;

	    जर (job->jv_tty_out != रिक्त && job->jv_status == JOB_FINISHED)
		साठी (part = PART_SOCK; part < PART_मोजणी; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    परत ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    स्थिर शून्य
channel_wait_any(दीर्घ msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    जर (stdio_channel->CH_OUT_FD != INVALID_FD)
	(शून्य)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(पूर्णांक)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#किंवाजर defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    संरचना timeval	tv;
    संरचना timeval	*tvp = &tv;
    पूर्णांक			maxfd;
    पूर्णांक			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, रिक्त, tvp);
    जर (ret >= 0)
	(शून्य)channel_select_check(ret, &rfds, &wfds);
#अन्यथा
    संरचना pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    पूर्णांक			nfd;
    पूर्णांक			towait = (पूर्णांक)msec;
    पूर्णांक			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    जर (ret >= 0)
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
    पर्यंत (stdio_channel != रिक्त && channel_is_open(stdio_channel))
    {
	दीर्घ	msec = 1000;
#ifdef FEAT_TIMERS
	दीर्घ	due_time = check_due_timer();

	जर (due_time > 0 && due_time < msec)
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
    पूर्णांक
channel_parse_messages(शून्य)
{
    channel_T	*channel = first_channel;
    पूर्णांक		ret = FALSE;
    पूर्णांक		r;
    ch_part_T	part = PART_SOCK;
    स्थिर पूर्णांक	recursive = 0;
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
    जर ((did_repeated_संदेश & REPEATED_संदेश_LOOKING) == 0)
    {
	ch_log(रिक्त, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_संदेश = REPEATED_संदेश_LOOKING;
    }
    पर्यंत (channel != रिक्त)
    {
	जर (recursive == 1)
	{
	    जर (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_मोजणी);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		सुरूठेवा;
	    }
	    जर (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		जर (channel->ch_job != रिक्त)
		    channel->ch_job->jv_channel = रिक्त;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		सुरूठेवा;
	    }
	    जर (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		सुरूठेवा;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	जर (part == PART_OUT
		&& channel->ch_job != रिक्त
		&& channel->ch_job->jv_tty_out != रिक्त
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    सुरूठेवा;
	}
#endif
	जर (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    जर (r == OK)
		ret = TRUE;
	    जर (channel_unref(channel) || (r == OK
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
		सुरूठेवा;
	    }
	}
	जर (part < PART_ERR)
	    ++part;
	अन्यथा
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    जर (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    परत ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    पूर्णांक
channel_any_readahead(शून्य)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    पर्यंत (channel != रिक्त)
    {
	जर (channel_has_readahead(channel, part))
	    परत TRUE;
	जर (part < PART_ERR)
	    ++part;
	अन्यथा
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    परत FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    पूर्णांक
set_ref_in_channel(पूर्णांक copyID)
{
    पूर्णांक		पात = FALSE;
    channel_T	*channel;
    typval_T	tv;

    साठी (channel = first_channel; !पात && channel != रिक्त;
						   channel = channel->ch_next)
	जर (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    पात = पात || set_ref_in_item(&tv, copyID, रिक्त, रिक्त, रिक्त);
	}
    परत पात;
}

/*
 * Return the "part" to write to for "channel".
 */
    स्थिर ch_part_T
channel_part_send(channel_T *channel)
{
    जर (channel->CH_SOCK_FD == INVALID_FD)
	परत PART_IN;
    परत PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    स्थिर ch_part_T
channel_part_read(channel_T *channel)
{
    जर (channel->CH_SOCK_FD == INVALID_FD)
	परत PART_OUT;
    परत PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    स्थिर ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    जर (channel == रिक्त)
	परत CH_MODE_JSON;
    परत channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    स्थिर पूर्णांक
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    परत channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    शून्य
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_संख्या = 0;
    जर (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	परत;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जर (channel != रिक्त)
	rettv->vval.v_संख्या = channel_has_readahead(channel, PART_SOCK)
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

    जर (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	परत;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जर (channel != रिक्त)
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

    जर (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	परत;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    जर (channel != रिक्त)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    शून्य
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_संख्या = -1;

    जर (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	परत;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जर (channel == रिक्त)
	परत;

    char_u	*what = tv_get_string(&argvars[1]);
    पूर्णांक		part;
    जर (STRCMP(what, "err") == 0)
	part = PART_ERR;
    अन्यथा जर (STRCMP(what, "out") == 0)
	part = PART_OUT;
    अन्यथा जर (STRCMP(what, "in") == 0)
	part = PART_IN;
    अन्यथा
	part = PART_SOCK;
    जर (channel->ch_part[part].ch_bufref.br_buf != रिक्त)
	rettv->vval.v_संख्या =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    शून्य
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    जर (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	परत;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जर (channel == रिक्त)
	परत;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    जर (channel->ch_job != रिक्त)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    शून्य
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    जर (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	परत;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जर (channel != रिक्त && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    शून्य
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    जर (check_restricted() || check_secure())
	परत;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    शून्य
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    जर (check_restricted() || check_secure())
	परत;
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
    ch_अभिव्यक्ती_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    शून्य
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_अभिव्यक्ती_common(argvars, rettv, FALSE);
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

    जर (check_restricted() || check_secure())
	परत;

    जर (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	परत;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    जर (channel == रिक्त)
	परत;
    clear_job_options(&opt);
    जर (get_job_options(&argvars[1], &opt,
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
    पूर्णांक		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = रिक्त;

    जर (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	परत;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    जर (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	जर (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    अक्षर      *status = channel_status(channel, -1);

    जर (channel == रिक्त)
	vim_snprintf((अक्षर *)buf, NUMBUFLEN, "channel %s", status);
    अन्यथा
	vim_snprintf((अक्षर *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    परत buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(पूर्णांक ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	जर (ch->ch_id == ch_id)
	    परत ch;
    परत रिक्त;
}

#endif // FEAT_JOB_CHANNEL
