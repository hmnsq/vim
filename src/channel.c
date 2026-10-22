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

#অন্তর্ভাব "vim.h"

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
প্রকারলেখ কাঠামো sockaddr_un {
    ADDRESS_FAMILY sun_family;
    অক্ষর sun_path[UNIX_PATH_MAX];
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

স্থির শূন্য channel_read(channel_T *channel, ch_part_T part, অক্ষর *func);
স্থির শূন্য channel_wait_any(long msec);
স্থির ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
স্থির পূর্ণসংখ্যা channel_get_timeout(channel_T *channel, ch_part_T part);
স্থির channel_T *channel_open_stdio(শূন্য);
স্থির ch_part_T channel_part_send(channel_T *channel);
স্থির ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    জন্য ((ch) = first_channel; (ch) != ফাঁকা; (ch) = (ch)->ch_পরবর্তী)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
স্থির পূর্ণসংখ্যা safe_to_invoke_callback = 0;

#ifdef MSWIN
    স্থির পূর্ণসংখ্যা
fd_read(sock_T fd, অক্ষর *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    যদি (!ReadFile(h, buf, (DWORD)len, &nread, ফাঁকা))
	ফেরত -1;
    ফেরত (পূর্ণসংখ্যা)nread;
}

    স্থির পূর্ণসংখ্যা
fd_write(sock_T fd, অক্ষর *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, আকার_2, done = 0;
    OVERLAPPED	ov;

    যখন (todo > 0)
    {
	যদি (todo > MAX_NAMED_PIPE_আকার)
	    আকার_2 = MAX_NAMED_PIPE_আকার;
	অন্যথা
	    আকার_2 = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	যদি (!WriteFile(h, buf + done, আকার_2, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    যদি (err != ERROR_IO_PENDING)
		ফেরত -1;
	    যদি (!Get_Overlapped_ফলাফল(h, &ov, &nwrite, FALSE))
		ফেরত -1;
	    FlushFileBuffers(h);
	}
	অন্যথা যদি (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    বিরতি;
	todo -= nwrite;
	done += nwrite;
    }
    ফেরত (পূর্ণসংখ্যা)done;
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
# define PERROR(বার্তা) (শূন্য)semsg("%s: %s", বার্তা, strerror_win32(errno))

    স্থির অক্ষর *
strerror_win32(পূর্ণসংখ্যা eno)
{
    স্থির LPVOID msgbuf = ফাঁকা;
    char_u *ptr;

    যদি (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ফাঁকা;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_বাফার |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	ফাঁকা,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ফাঁকা);
    যদি (msgbuf != ফাঁকা)
	// chomp \r or \n
	জন্য (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    নির্বাচন (*ptr)
	    {
		ক্ষেত্র '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    বিরতি;
		ক্ষেত্র '\n':
		    যদি (*(ptr + 1) == '\0')
			*ptr = '\0';
		    অন্যথা
			*ptr = ' ';
		    বিরতি;
	    }
    ফেরত msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
স্থির channel_T *first_channel = ফাঁকা;
স্থির পূর্ণসংখ্যা পরবর্তী_ch_id = 0;

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

    যদি (channel == ফাঁকা)
	ফেরত ফাঁকা;

    channel->ch_id = পরবর্তী_ch_id++;
    ch_log(channel, "Created channel");

    জন্য (part = PART_SOCK; part < PART_গুনতি; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_প্রবেশ_Handler = (Xt_প্রবেশ_Id)ফাঁকা;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_প্রবেশ_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    যদি (first_channel != ফাঁকা)
    {
	first_channel->ch_পূর্ব = channel;
	channel->ch_পরবর্তী = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    ফেরত channel;
}

    পূর্ণসংখ্যা
has_any_channel(শূন্য)
{
    ফেরত first_channel != ফাঁকা;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    পূর্ণসংখ্যা
channel_still_useful(channel_T *channel)
{
    পূর্ণসংখ্যা has_sock_বার্তা;
    পূর্ণসংখ্যা	has_out_বার্তা;
    পূর্ণসংখ্যা	has_err_বার্তা;

    // If the job was killed the channel is not expected to work anymore.
    যদি (channel->ch_job_killed && channel->ch_job == ফাঁকা)
	ফেরত FALSE;

    // If there is a close callback it may still need to be invoked.
    যদি (channel->ch_close_cb.cb_name != ফাঁকা)
	ফেরত TRUE;

    // If reading from or a buffer it's still useful.
    যদি (channel->ch_part[PART_IN].ch_bufref.br_buf != ফাঁকা)
	ফেরত TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_বার্তা = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_শীর্ষ.rq_পরবর্তী != ফাঁকা
		|| channel->ch_part[PART_SOCK].ch_json_শীর্ষ.jq_পরবর্তী != ফাঁকা;
    has_out_বার্তা = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_শীর্ষ.rq_পরবর্তী != ফাঁকা
		  || channel->ch_part[PART_OUT].ch_json_শীর্ষ.jq_পরবর্তী != ফাঁকা;
    has_err_বার্তা = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_শীর্ষ.rq_পরবর্তী != ফাঁকা
		  || channel->ch_part[PART_ERR].ch_json_শীর্ষ.jq_পরবর্তী != ফাঁকা;
    ফেরত (channel->ch_callback.cb_name != ফাঁকা && (has_sock_বার্তা
		|| has_out_বার্তা || has_err_বার্তা))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != ফাঁকা
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != ফাঁকা)
		    && has_out_বার্তা)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ফাঁকা
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ফাঁকা)
		    && has_err_বার্তা);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    পূর্ণসংখ্যা
channel_can_close(channel_T *channel)
{
    ফেরত channel->ch_to_be_closed == 0;
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
    যদি (channel->ch_পরবর্তী != ফাঁকা)
	channel->ch_পরবর্তী->ch_পূর্ব = channel->ch_পূর্ব;
    যদি (channel->ch_পূর্ব == ফাঁকা)
	first_channel = channel->ch_পরবর্তী;
    অন্যথা
	channel->ch_পূর্ব->ch_পরবর্তী = channel->ch_পরবর্তী;
    vim_free(channel);
}

    স্থির শূন্য
channel_free(channel_T *channel)
{
    যদি (in_free_unref_বস্তুসমূহ)
	ফেরত;

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
    স্থির পূর্ণসংখ্যা
channel_may_free(channel_T *channel)
{
    যদি (!channel_still_useful(channel))
    {
	channel_free(channel);
	ফেরত TRUE;
    }
    ফেরত FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    পূর্ণসংখ্যা
channel_unref(channel_T *channel)
{
    যদি (channel != ফাঁকা && --channel->ch_refcount <= 0)
	ফেরত channel_may_free(channel);
    ফেরত FALSE;
}

    পূর্ণসংখ্যা
free_unused_channels_contents(পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা mask)
{
    পূর্ণসংখ্যা		did_free = FALSE;
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
    ফেরত did_free;
}

    শূন্য
free_unused_channels(পূর্ণসংখ্যা copyID, পূর্ণসংখ্যা mask)
{
    channel_T	*ch;
    channel_T	*ch_পরবর্তী;

    জন্য (ch = first_channel; ch != ফাঁকা; ch = ch_পরবর্তী)
    {
	ch_পরবর্তী = ch->ch_পরবর্তী;
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
	ফেরত ফাঁকা;

    FOR_ALL_CHANNELS(channel)
    {
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
	    যদি (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ফেরত channel;
	    }
    }
    ফেরত ফাঁকা;
}

    স্থির শূন্য
channel_read_fd(পূর্ণসংখ্যা fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    যদি (channel == ফাঁকা)
	ch_error(ফাঁকা, "Channel for fd %d not found", fd);
    অন্যথা
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    স্থির শূন্য
messageFromServerX11(XtPointer client_তথ্য,
		  পূর্ণসংখ্যা *unused1 UNUSED,
		  Xt_প্রবেশ_Id *unused2 UNUSED)
{
    channel_read_fd((পূর্ণসংখ্যা)(long)client_তথ্য);
}
# endif

# ifdef FEAT_GUI_GTK
#  যদি GTK_CHECK_VERSION(3,0,0)
    স্থির gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer client_তথ্য)
{
    channel_read_fd(GPOINTER_TO_INT(client_তথ্য));
    ফেরত TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  অন্যথা
    স্থির শূন্য
messageFromServerGtk2(gpointer client_তথ্য,
		  gint unused1 UNUSED,
		  Gdk_প্রবেশ_Condition unused2 UNUSED)
{
    channel_read_fd((পূর্ণসংখ্যা)(long)client_তথ্য);
}
#  endif
# endif

    স্থির শূন্য
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    যদি (!CH_HAS_GUI)
	ফেরত;

    // gets stuck in handling events for a not connected channel
    যদি (channel->ch_keep_open)
	ফেরত;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    যদি (channel->ch_part[part].ch_প্রবেশ_Handler == (Xt_প্রবেশ_Id)ফাঁকা)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_প্রবেশ_Handler = Xt_App_Add_প্রবেশ(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_প্রবেশ_Read_Mask + Xt_প্রবেশ_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# অন্যথা
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    যদি (channel->ch_part[part].ch_প্রবেশ_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   যদি GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_প্রবেশ_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   অন্যথা
	channel->ch_part[part].ch_প্রবেশ_Handler = gdk_প্রবেশ_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_প্রবেশ_Condition)
			     ((পূর্ণসংখ্যা)GDK_প্রবেশ_READ + (পূর্ণসংখ্যা)GDK_প্রবেশ_EXCEPTION),
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
    যদি (channel->ch_part[part].ch_প্রবেশ_Handler != (Xt_প্রবেশ_Id)ফাঁকা)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_প্রবেশ(channel->ch_part[part].ch_প্রবেশ_Handler);
	channel->ch_part[part].ch_প্রবেশ_Handler = (Xt_প্রবেশ_Id)ফাঁকা;
    }
# অন্যথা
#  ifdef FEAT_GUI_GTK
    যদি (channel->ch_part[part].ch_প্রবেশ_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   যদি GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_প্রবেশ_Handler);
#   অন্যথা
	gdk_প্রবেশ_remove(channel->ch_part[part].ch_প্রবেশ_Handler);
#   endif
	channel->ch_part[part].ch_প্রবেশ_Handler = 0;
    }
#  endif
# endif
}

    স্থির শূন্য
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    জন্য (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    স্থির পূর্ণসংখ্যা
channel_connect(
	channel_T *channel,
	ধ্রুবক কাঠামো sockaddr *server_addr,
	পূর্ণসংখ্যা server_addrlen,
	পূর্ণসংখ্যা *waittime)
{
    পূর্ণসংখ্যা		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    যখন (TRUE)
    {
	long	elapsed_msec = 0;
	পূর্ণসংখ্যা	waitnow;
	পূর্ণসংখ্যা	ret;

	যদি (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	যদি (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    ফেরত -1;
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
		ফেরত -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	যদি (ret == 0)
	    // The connection could be established.
	    বিরতি;

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
	    ফেরত -1;
	}
	অন্যথা যদি (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ফেরত -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    কাঠামো timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    পূর্ণসংখ্যা			so_error = 0;
	    socklen_t		so_error_len = আকার(so_error);
	    কাঠামো timeval	start_tv;
	    কাঠামো timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, ফাঁকা);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ফাঁকা, &tv);
	    যদি (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		ফেরত -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    যদি (FD_ISSET(sd, &wfds))
		বিরতি;
	    elapsed_msec = waitnow;
	    যদি (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		চালু;
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
		    ফেরত -1;
		}
		অন্যথা যদি (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ফেরত -1;
		}
	    }

	    যদি (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		বিরতি;

	    gettimeofday(&end_tv, ফাঁকা);
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
		চালু;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	ফেরত -1;
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

    ফেরত sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ধ্রুবক অক্ষর *path,
	শূন্য (*nb_close_cb)(শূন্য))
{
    channel_T		*channel = ফাঁকা;
    পূর্ণসংখ্যা			sd = -1;
    size_t		path_len = STRLEN(path);
    কাঠামো sockaddr_un	server;
    size_t		server_len;
    পূর্ণসংখ্যা			waittime = -1;

    যদি (*path == NUL || path_len >= আকার(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ফেরত ফাঁকা;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == ফাঁকা)
    {
	ch_error(ফাঁকা, "Cannot allocate channel.");
	ফেরত ফাঁকা;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, আকার(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(কাঠামো sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (কাঠামো sockaddr *)&server, (পূর্ণসংখ্যা)server_len,
								   &waittime);

    যদি (sd < 0)
    {
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (অক্ষর *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ফেরত channel;
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
	ধ্রুবক অক্ষর *hostname,
	পূর্ণসংখ্যা port,
	পূর্ণসংখ্যা waittime,
	শূন্য (*nb_close_cb)(শূন্য))
{
    পূর্ণসংখ্যা			sd = -1;
    channel_T		*channel = ফাঁকা;
#ifdef FEAT_IPV6
    পূর্ণসংখ্যা			err;
    কাঠামো addrinfo	hints;
    কাঠামো addrinfo	*res = ফাঁকা;
    কাঠামো addrinfo	*addr = ফাঁকা;
#অন্যথা
    কাঠামো sockaddr_in	server;
    কাঠামো hostent	*host = ফাঁকা;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == ফাঁকা)
    {
	ch_error(ফাঁকা, "Cannot allocate channel.");
	ফেরত ফাঁকা;
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
    যদি ((err = getaddrinfo(hostname, ফাঁকা, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    // Count the number of addresses for timeout distribution
    পূর্ণসংখ্যা addr_গুনতি = 0;
    জন্য (addr = res; addr != ফাঁকা; addr = addr->ai_পরবর্তী)
	addr_গুনতি++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    যদি (waittime == 0)
	waittime = 15;

    পূর্ণসংখ্যা addr_সূচক = 0;
    জন্য (addr = res; addr != ফাঁকা; addr = addr->ai_পরবর্তী, addr_সূচক++)
    {
	ধ্রুবক অক্ষর  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ধ্রুবক শূন্য  *src = ফাঁকা;
	অক্ষর	    buf[NUMBUFLEN];
# endif
	পূর্ণসংখ্যা	    try_waittime;
	পূর্ণসংখ্যা	    before_waittime;
	পূর্ণসংখ্যা	    consumed;
	পূর্ণসংখ্যা	    remaining_addrs;

	যদি (addr->ai_family == AF_INET6)
	{
	    কাঠামো sockaddr_in6 *sai = (কাঠামো sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	অন্যথা যদি (addr->ai_family == AF_INET)
	{
	    কাঠামো sockaddr_in *sai = (কাঠামো sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	যদি (src != ফাঁকা)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, আকার(buf));
	    যদি (dst == ফাঁকা)
		dst = hostname;
	    অন্যথা যদি (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	যদি (addr->ai_পরবর্তী == ফাঁকা)
	    try_waittime = waittime;
	অন্যথা যদি (addr_সূচক == 0)
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
	    remaining_addrs = addr_গুনতি - addr_সূচক;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (পূর্ণসংখ্যা)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	যদি (waittime < 0)
	    waittime = 0;

	যদি (sd >= 0)
	    বিরতি;

	// If we have no time left, stop trying
	যদি (waittime <= 0 && addr->ai_পরবর্তী != ফাঁকা)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    বিরতি;
	}
    }

    freeaddrinfo(res);
#অন্যথা
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    যদি ((host = gethostbyname(hostname)) == ফাঁকা)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	ফেরত ফাঁকা;
    }
    {
	অক্ষর *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], আকার(p));
	memcpy((অক্ষর *)&server.sin_addr, p, host->h_দৈর্ঘ্য);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    যদি (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (কাঠামো sockaddr *)&server, আকার(server),
								   &waittime);
#endif

    যদি (sd < 0)
    {
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (অক্ষর *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ফেরত channel;
}

    স্থির শূন্য
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    যদি (callback->cb_name != ফাঁকা && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    অন্যথা
	cbp->cb_name = ফাঁকা;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	স্থির শূন্য
prepare_বাফার(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_মান_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_মান_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    যদি (curbuf->b_ml.ml_mfp == ফাঁকা)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    স্থির buf_T *
channel_find_বাফার(char_u *name, পূর্ণসংখ্যা err, পূর্ণসংখ্যা বার্তা)
{
    buf_T *buf = ফাঁকা;
    buf_T *save_curbuf = curbuf;

    যদি (name != ফাঁকা && *name != NUL)
    {
	buf = buflist_findname(name);
	যদি (buf == ফাঁকা)
	    buf = buflist_findname_exp(name);
    }

    যদি (buf != ফাঁকা)
	ফেরত buf;

    buf = buflist_new(name == ফাঁকা || *name == NUL ? ফাঁকা : name,
	    ফাঁকা, (linenr_T)0, BLN_LISTED | BLN_NEW);
    যদি (buf == ফাঁকা)
	ফেরত ফাঁকা;
    prepare_বাফার(buf);

    curbuf = buf;
    যদি (বার্তা)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    ফেরত buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    স্থির শূন্য
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    যদি (opt->jo_set & JO_MODE)
	জন্য (part = PART_SOCK; part < PART_গুনতি; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    যদি (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    যদি (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    যদি (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    যদি (opt->jo_set & JO_TIMEOUT)
	জন্য (part = PART_SOCK; part < PART_গুনতি; ++part)
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

    যদি ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_বাফার)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	যদি (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	যদি (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    যদি (buf == ফাঁকা)
		semsg(_(e_বাফার_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা বার্তা = TRUE;

	    যদি (opt->jo_set2 & JO2_OUT_বার্তা)
		বার্তা = opt->jo_message[PART_OUT];
	    buf = channel_find_বাফার(opt->jo_io_name[PART_OUT], FALSE, বার্তা);
	}
	যদি (buf != ফাঁকা)
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
						       (অক্ষর *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		যদি (buf->b_ml.ml_mfp == ফাঁকা)
		    prepare_বাফার(buf);
	    }
	}
    }

    যদি ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_বাফার
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_বাফার)))
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
	    যদি (buf == ফাঁকা)
		semsg(_(e_বাফার_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা বার্তা = TRUE;

	    যদি (opt->jo_set2 & JO2_ERR_বার্তা)
		বার্তা = opt->jo_message[PART_ERR];
	    buf = channel_find_বাফার(opt->jo_io_name[PART_ERR], TRUE, বার্তা);
	}
	যদি (buf != ফাঁকা)
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
						       (অক্ষর *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		যদি (buf->b_ml.ml_mfp == ফাঁকা)
		    prepare_বাফার(buf);
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
    পূর্ণসংখ্যা
channel_parse_socketserver_address(
	char_u	*address,
	পূর্ণসংখ্যা	*port,
	char_u	**unix_path,
	bool	quiet)
{
    অক্ষর    *rest;
    long    val;

    যদি (*address == NUL)
	যাও fail;

    যদি (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	ফেরত OK;
    }

    val = strtol((অক্ষর *)(address), &rest, 10);
    যদি (val < 0 || val >= 65536 || *rest != NUL)
    {
	যদি (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	ফেরত FAIL;
    }
    *port = (পূর্ণসংখ্যা)val;

    ফেরত OK;
fail:
    যদি (!quiet)
	semsg(_(e_invalid_argument_str), address);
    ফেরত FAIL;
}

/*
 * Implements ch_open().
 */
    স্থির channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    অক্ষর	*rest;
    পূর্ণসংখ্যা		port = 0;
    পূর্ণসংখ্যা		is_ipv6 = FALSE;
    পূর্ণসংখ্যা		is_unix = FALSE;
    পূর্ণসংখ্যা		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত ফাঁকা;

    address = tv_get_string(&argvars[0]);
    যদি (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ফেরত ফাঁকা;

    যদি (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	ফেরত ফাঁকা;
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
	যদি (p == ফাঁকা || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    ফেরত ফাঁকা;
	}
    }
    অন্যথা
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	যদি (p == ফাঁকা)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ফেরত ফাঁকা;
	}
    }

    যদি (!is_unix && !is_stdio)
    {
	port = strtol((অক্ষর *)(p + 1), &rest, 10);
	যদি (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ফেরত ফাঁকা;
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
	যাও theend;
    যদি (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	যাও theend;
    }

    যদি (is_stdio)
	channel = channel_open_stdio();
    অন্যথা যদি (is_unix)
	channel = channel_open_unix((অক্ষর *)address, ফাঁকা);
    অন্যথা
	channel = channel_open((অক্ষর *)address, port, opt.jo_waittime, ফাঁকা);
    যদি (channel != ফাঁকা)
    {
	পূর্ণসংখ্যা	user_set = opt.jo_set;

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
    ফেরত channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
স্থির channel_T *stdio_channel = ফাঁকা;

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
	ফেরত ফাঁকা;
    }
    যদি (stdio_channel != ফাঁকা)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ফেরত ফাঁকা;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	পূর্ণসংখ্যা	nul_fd;

	যদি (!DuplicateHandle(proc, GetStdHandle(STD_প্রবেশ_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ফেরত ফাঁকা;
	}
	যদি (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ফেরত ফাঁকা;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ফাঁকা, OPEN_EXISTING, 0,
		ফাঁকা);
	যদি (hnul != INVALID_HANDLE_মান)
	{
	    SetStdHandle(STD_প্রবেশ_HANDLE, hnul);
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
	পূর্ণসংখ্যা	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	যদি (in_fd < 0 || out_fd < 0)
	{
	    যদি (in_fd >= 0)
		close(in_fd);
	    যদি (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ফেরত ফাঁকা;
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
    যদি (channel == ফাঁকা)
    {
	ch_error(ফাঁকা, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	ফেরত ফাঁকা;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ফেরত channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    অক্ষর	*rest;
    পূর্ণসংখ্যা		port;
    পূর্ণসংখ্যা		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ফাঁকা;

    যদি (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ফেরত ফাঁকা;

    arg = tv_get_string(&argvars[0]);
    যদি (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	ফেরত ফাঁকা;
    }

    যদি (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    অন্যথা
    {
	port = strtol((অক্ষর *)(arg), &rest, 10);
	যদি (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ফেরত ফাঁকা;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    যদি (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	যাও theend;
    যদি (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	যাও theend;
    }

    যদি (is_unix)
	channel = channel_listen_unix((অক্ষর *)arg, ফাঁকা, true);
    অন্যথা
	channel = channel_listen(port, ফাঁকা);
    যদি (channel != ফাঁকা)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ফেরত channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	পূর্ণসংখ্যা port_in,
	শূন্য (*nb_close_cb)(শূন্য))
{
    পূর্ণসংখ্যা			sd = -1;
    কাঠামো sockaddr_in	server;
#ifndef FEAT_IPV6
    কাঠামো hostent	*host;
#endif
    পূর্ণসংখ্যা			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == ফাঁকা)
    {
	ch_error(ফাঁকা, "Cannot allocate channel.");
	ফেরত ফাঁকা;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((অক্ষর *)&server, 0, আকার(server));
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
	ফেরত ফাঁকা;
    }

#ifdef MSWIN
    যদি (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ধ্রুবক অক্ষর *)&val, আকার(val)) < 0)
#অন্যথা
    যদি (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, আকার(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    // Bind the socket to the port
    যদি (bind(sd, (কাঠামো sockaddr *)&server, আকার(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    যদি (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    যদি (port_in == 0)
    {
	কাঠামো sockaddr_in	addr;
	socklen_t		addr_len = আকার(addr);

	যদি (getsockname(sd, (কাঠামো sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (অক্ষর *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ফেরত channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	অক্ষর *path,
	শূন্য (*nb_close_cb)(শূন্য),
	bool replace)
{
    পূর্ণসংখ্যা			sd = -1;
    কাঠামো sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    যদি (path == ফাঁকা || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == ফাঁকা ? (অক্ষর *)"" : path);
	ফেরত ফাঁকা;
    }

    path_len = STRLEN(path);
    যদি (path_len >= আকার(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ফেরত ফাঁকা;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    যদি (channel == ফাঁকা)
    {
	ch_error(ফাঁকা, "Cannot allocate channel.");
	ফেরত ফাঁকা;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, আকার(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    যদি (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    যদি (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(কাঠামো sockaddr_un, sun_path) + path_len + 1;
    যদি (bind(sd, (কাঠামো sockaddr *)&server, (পূর্ণসংখ্যা)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    যদি (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ফেরত ফাঁকা;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (অক্ষর *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ফেরত channel;
}

     শূন্য
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    যদি (*fd == INVALID_FD)
	ফেরত;

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

    যদি (job->jv_in_buf == ফাঁকা)
	ফেরত;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (অক্ষর *)in_part->ch_bufref.br_buf->b_ffname);
    যদি (options->jo_set & JO_IN_শীর্ষ)
    {
	যদি (options->jo_in_শীর্ষ == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_শীর্ষ =
		in_part->ch_bufref.br_buf->b_ml.ml_সারি_গুনতি + 1;
	}
	অন্যথা
	    in_part->ch_buf_শীর্ষ = options->jo_in_শীর্ষ;
    }
    অন্যথা
	in_part->ch_buf_শীর্ষ = 1;
    যদি (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    অন্যথা
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_সারি_গুনতি;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    স্থির শূন্য
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	পূর্ণসংখ্যা	    id)
{
    cbq_T *শীর্ষ = &channel->ch_part[part].ch_cb_শীর্ষ;
    cbq_T *item = ALLOC_ONE(cbq_T);

    যদি (item == ফাঁকা)
	ফেরত;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_পূর্ব = শীর্ষ->cq_পূর্ব;
    শীর্ষ->cq_পূর্ব = item;
    item->cq_পরবর্তী = ফাঁকা;
    যদি (item->cq_পূর্ব == ফাঁকা)
	শীর্ষ->cq_পরবর্তী = item;
    অন্যথা
	item->cq_পূর্ব->cq_পরবর্তী = item;
}

    স্থির শূন্য
write_buf_সারি(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *সারি = ml_get_buf(buf, lnum, FALSE);
    পূর্ণসংখ্যা	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    পূর্ণসংখ্যা	    i;

    // Need to make a copy to be able to append a NL.
    যদি ((p = alloc(len + 2)) == ফাঁকা)
	ফেরত;
    memcpy((অক্ষর *)p, (অক্ষর *)সারি, len);

    যদি (channel->ch_write_text_mode)
	p[len] = CAR;
    অন্যথা
    {
	জন্য (i = 0; i < len; ++i)
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
    স্থির পূর্ণসংখ্যা
can_write_buf_সারি(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    যদি (in_part->ch_fd == INVALID_FD)
	ফেরত FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    যদি (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    অন্যথা যদি (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# যদি defined(HAVE_SELECT)
	কাঠামো timeval	tval;
	fd_set		wfds;
	পূর্ণসংখ্যা		ret;

	FD_ZERO(&wfds);
	FD_SET((পূর্ণসংখ্যা)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	জন্য (;;)
	{
	    ret = select((পূর্ণসংখ্যা)in_part->ch_fd + 1, ফাঁকা, &wfds, ফাঁকা, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    যদি (ret == -1 && errno == EINTR)
		চালু;
#  endif
	    যদি (ret <= 0 || in_part->ch_block_write == 1)
	    {
		যদি (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		অন্যথা
		    ch_log(channel, "Input not ready for writing");
		ফেরত FALSE;
	    }
	    বিরতি;
	}
# অন্যথা
	কাঠামো pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	যদি (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ফেরত FALSE;
	}
	যদি (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ফেরত FALSE;
	}
# endif
    }
#endif
    ফেরত TRUE;
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
    পূর্ণসংখ্যা		written = 0;

    যদি (buf == ফাঁকা || in_part->ch_buf_append)
	ফেরত;  // no buffer or using appending
    যদি (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == ফাঁকা)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = ফাঁকা;
	ফেরত;
    }

    জন্য (lnum = in_part->ch_buf_শীর্ষ; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_সারি_গুনতি; ++lnum)
    {
	যদি (!can_write_buf_সারি(channel))
	    বিরতি;
	write_buf_সারি(buf, lnum, channel);
	++written;
    }

    যদি (written == 1)
	ch_log(channel, "written line %d to channel", (পূর্ণসংখ্যা)lnum - 1);
    অন্যথা যদি (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_শীর্ষ = lnum;
    যদি (lnum > buf->b_ml.ml_সারি_গুনতি || lnum > in_part->ch_buf_bot)
    {
#যদি defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	যদি (channel->ch_job != ফাঁকা)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = ফাঁকা;
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
				   (long)(buf->b_ml.ml_সারি_গুনতি - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    শূন্য
channel_বাফার_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	জন্য (part = PART_SOCK; part < PART_গুনতি; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    যদি (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = ফাঁকা;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    শূন্য
channel_write_প্রবেশ(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    যদি (in_part->ch_writeque.wq_পরবর্তী != ফাঁকা)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    অন্যথা যদি (in_part->ch_bufref.br_buf != ফাঁকা)
    {
	যদি (in_part->ch_buf_append)
	    channel_write_new_সারিগুলি(in_part->ch_bufref.br_buf);
	অন্যথা
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    শূন্য
channel_write_any_সারিগুলি(শূন্য)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_প্রবেশ(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    শূন্য
channel_write_new_সারিগুলি(buf_T *buf)
{
    channel_T	*channel;
    পূর্ণসংখ্যা		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	পূর্ণসংখ্যা	    written = 0;

	যদি (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    যদি (in_part->ch_fd == INVALID_FD)
		চালু;  // pipe was closed
	    found_one = TRUE;
	    জন্য (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_সারি_গুনতি;
								       ++lnum)
	    {
		যদি (!can_write_buf_সারি(channel))
		    বিরতি;
		write_buf_সারি(buf, lnum, channel);
		++written;
	    }

	    যদি (written == 1)
		ch_log(channel, "written line %d to channel", (পূর্ণসংখ্যা)lnum - 1);
	    অন্যথা যদি (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    যদি (lnum < buf->b_ml.ml_সারি_গুনতি)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_সারি_গুনতি - lnum));

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
    readq_T *শীর্ষ = &channel->ch_part[part].ch_শীর্ষ;

    ফেরত শীর্ষ->rq_পরবর্তী;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *গ্রন্থি)
{
    char_u  *বাফার = গ্রন্থি->rq_বাফার;
    long_u  i;

    জন্য (i = 0; i < গ্রন্থি->rq_buflen; ++i)
	যদি (বাফার[i] == NL)
	    ফেরত বাফার + i;
    ফেরত ফাঁকা;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, পূর্ণসংখ্যা *outlen)
{
    readq_T *শীর্ষ = &channel->ch_part[part].ch_শীর্ষ;
    readq_T *গ্রন্থি = শীর্ষ->rq_পরবর্তী;
    char_u *p;

    যদি (গ্রন্থি == ফাঁকা)
	ফেরত ফাঁকা;
    যদি (outlen != ফাঁকা)
	*outlen += গ্রন্থি->rq_buflen;
    // dispose of the node but keep the buffer
    p = গ্রন্থি->rq_বাফার;
    শীর্ষ->rq_পরবর্তী = গ্রন্থি->rq_পরবর্তী;
    যদি (গ্রন্থি->rq_পরবর্তী == ফাঁকা)
	শীর্ষ->rq_পূর্ব = ফাঁকা;
    অন্যথা
	গ্রন্থি->rq_পরবর্তী->rq_পূর্ব = ফাঁকা;
    vim_free(গ্রন্থি);
    ফেরত p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    স্থির char_u *
channel_get_all(channel_T *channel, ch_part_T part, পূর্ণসংখ্যা *outlen)
{
    readq_T *শীর্ষ = &channel->ch_part[part].ch_শীর্ষ;
    readq_T *গ্রন্থি;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    জন্য (গ্রন্থি = শীর্ষ->rq_পরবর্তী; গ্রন্থি != ফাঁকা; গ্রন্থি = গ্রন্থি->rq_পরবর্তী)
	len += গ্রন্থি->rq_buflen;
    res = alloc(len + 1);
    যদি (res == ফাঁকা)
	ফেরত ফাঁকা;
    p = res;
    জন্য (গ্রন্থি = শীর্ষ->rq_পরবর্তী; গ্রন্থি != ফাঁকা; গ্রন্থি = গ্রন্থি->rq_পরবর্তী)
    {
	mch_memmove(p, গ্রন্থি->rq_বাফার, গ্রন্থি->rq_buflen);
	p += গ্রন্থি->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    করো
    {
	p = channel_get(channel, part, ফাঁকা);
	vim_free(p);
    } যখন (p != ফাঁকা);

    যদি (outlen != ফাঁকা)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ফেরত res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    যখন (p < res + len)
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
		যখন (p < res + (len - 1) && *p != '\a')
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

    ফেরত res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    শূন্য
channel_consume(channel_T *channel, ch_part_T part, পূর্ণসংখ্যা len)
{
    readq_T *শীর্ষ = &channel->ch_part[part].ch_শীর্ষ;
    readq_T *গ্রন্থি = শীর্ষ->rq_পরবর্তী;
    char_u *buf = গ্রন্থি->rq_বাফার;

    যদি (len < 0 || (long_u)len > গ্রন্থি->rq_buflen)
	ফেরত;
    mch_memmove(buf, buf + len, গ্রন্থি->rq_buflen - len);
    গ্রন্থি->rq_buflen -= len;
    গ্রন্থি->rq_বাফার[গ্রন্থি->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    পূর্ণসংখ্যা
channel_collapse(channel_T *channel, ch_part_T part, পূর্ণসংখ্যা want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*শীর্ষ = &channel->ch_part[part].ch_শীর্ষ;
    readq_T	*গ্রন্থি = শীর্ষ->rq_পরবর্তী;
    readq_T	*last_গ্রন্থি;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    যদি (গ্রন্থি == ফাঁকা || গ্রন্থি->rq_পরবর্তী == ফাঁকা)
	ফেরত FAIL;

    last_গ্রন্থি = গ্রন্থি->rq_পরবর্তী;
    len = গ্রন্থি->rq_buflen + last_গ্রন্থি->rq_buflen;
    যদি (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	যখন (last_গ্রন্থি->rq_পরবর্তী != ফাঁকা
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_গ্রন্থি) == ফাঁকা))
	{
	    last_গ্রন্থি = last_গ্রন্থি->rq_পরবর্তী;
	    len += last_গ্রন্থি->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    যদি (newbuf == ফাঁকা)
	ফেরত FAIL;	    // out of memory
    mch_memmove(p, গ্রন্থি->rq_বাফার, গ্রন্থি->rq_buflen);
    p += গ্রন্থি->rq_buflen;
    vim_free(গ্রন্থি->rq_বাফার);
    গ্রন্থি->rq_বাফার = newbuf;
    জন্য (n = গ্রন্থি; n != last_গ্রন্থি; )
    {
	n = n->rq_পরবর্তী;
	mch_memmove(p, n->rq_বাফার, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_বাফার);
    }
    *p = NUL;
    গ্রন্থি->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    জন্য (n = গ্রন্থি->rq_পরবর্তী; n != last_গ্রন্থি; )
    {
	n = n->rq_পরবর্তী;
	vim_free(n->rq_পূর্ব);
    }
    গ্রন্থি->rq_পরবর্তী = last_গ্রন্থি->rq_পরবর্তী;
    যদি (last_গ্রন্থি->rq_পরবর্তী == ফাঁকা)
	শীর্ষ->rq_পূর্ব = গ্রন্থি;
    অন্যথা
	last_গ্রন্থি->rq_পরবর্তী->rq_পূর্ব = গ্রন্থি;
    vim_free(last_গ্রন্থি);
    ফেরত OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
channel_save(channel_T *channel, ch_part_T part, char_u *buf, পূর্ণসংখ্যা len,
						      পূর্ণসংখ্যা prepend, অক্ষর *lead)
{
    readq_T *গ্রন্থি;
    readq_T *শীর্ষ = &channel->ch_part[part].ch_শীর্ষ;
    char_u  *p;
    পূর্ণসংখ্যা	    i;

    গ্রন্থি = ALLOC_ONE(readq_T);
    যদি (গ্রন্থি == ফাঁকা)
	ফেরত FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    গ্রন্থি->rq_বাফার = alloc(len + 1);
    যদি (গ্রন্থি->rq_বাফার == ফাঁকা)
    {
	vim_free(গ্রন্থি);
	ফেরত FAIL;	    // out of memory
    }

    যদি (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = গ্রন্থি->rq_বাফার;
	জন্য (i = 0; i < len; ++i)
	    যদি (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	গ্রন্থি->rq_buflen = (long_u)(p - গ্রন্থি->rq_বাফার);
    }
    অন্যথা
    {
	mch_memmove(গ্রন্থি->rq_বাফার, buf, len);
	গ্রন্থি->rq_বাফার[len] = NUL;
	গ্রন্থি->rq_buflen = (long_u)len;
    }

    যদি (prepend)
    {
	// prepend node to the head of the queue
	গ্রন্থি->rq_পরবর্তী = শীর্ষ->rq_পরবর্তী;
	গ্রন্থি->rq_পূর্ব = ফাঁকা;
	যদি (শীর্ষ->rq_পরবর্তী == ফাঁকা)
	    শীর্ষ->rq_পূর্ব = গ্রন্থি;
	অন্যথা
	    শীর্ষ->rq_পরবর্তী->rq_পূর্ব = গ্রন্থি;
	শীর্ষ->rq_পরবর্তী = গ্রন্থি;
    }
    অন্যথা
    {
	// append node to the tail of the queue
	গ্রন্থি->rq_পরবর্তী = ফাঁকা;
	গ্রন্থি->rq_পূর্ব = শীর্ষ->rq_পূর্ব;
	যদি (শীর্ষ->rq_পূর্ব == ফাঁকা)
	    শীর্ষ->rq_পরবর্তী = গ্রন্থি;
	অন্যথা
	    শীর্ষ->rq_পূর্ব->rq_পরবর্তী = গ্রন্থি;
	শীর্ষ->rq_পূর্ব = গ্রন্থি;
    }

    যদি (ch_log_active() && lead != ফাঁকা)
	ch_log_literal(lead, channel, part, buf, len);

    ফেরত OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    পূর্ণসংখ্যা
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*পরবর্তী = channel_get(channel, part, ফাঁকা);
    পূর্ণসংখ্যা		keeplen;
    পূর্ণসংখ্যা		addlen;
    char_u	*p;

    যদি (পরবর্তী == ফাঁকা)
	ফেরত FALSE;

    keeplen = reader->js_end - reader->js_buf;
    যদি (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (পূর্ণসংখ্যা)STRLEN(পরবর্তী);
	p = alloc(keeplen + addlen + 1);
	যদি (p == ফাঁকা)
	{
	    vim_free(পরবর্তী);
	    ফেরত FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, পরবর্তী, addlen + 1);
	vim_free(পরবর্তী);
	পরবর্তী = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = পরবর্তী;
    ফেরত TRUE;
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
    স্থির পূর্ণসংখ্যা
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*সারি_start;
    char_u	*p;
    int_u	hdr_len;
    পূর্ণসংখ্যা		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    যখন (TRUE)
    {
	সারি_start = p;
	যখন (*p != NUL && *p != '\n')
	    p++;
	যদি (*p == NUL)			// partial header
	    ফেরত MAYBE;
	p++;

	// process the content length field (if present)
	যদি ((p - সারি_start > 16)
		&& STRNICMP(সারি_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((অক্ষর *)সারি_start + 16, ফাঁকা, 10);
	    যদি (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ফেরত FAIL;
	}

	যদি ((p - সারি_start) == 2 && সারি_start[0] == '\r' &&
		সারি_start[1] == '\n')
	    // reached the empty line
	    বিরতি;
    }

    যদি (payload_len == -1)
	// Content-Length field is not present in the header
	ফেরত FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    যদি (jsbuf_len < hdr_len + payload_len)
	ফেরত MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    ফেরত OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    পূর্ণসংখ্যা
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*শীর্ষ = &chanpart->ch_json_শীর্ষ;
    পূর্ণসংখ্যা		status = OK;
    পূর্ণসংখ্যা		ret;

    যদি (channel_peek(channel, part) == ফাঁকা)
	ফেরত FALSE;

    reader.js_buf = channel_get(channel, part, ফাঁকা);
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
	    যদি (item == ফাঁকা)
		clear_tv(&listtv);
	    অন্যথা
	    {
		item->jq_no_callback = FALSE;
		item->jq_মান = alloc_tv();
		যদি (item->jq_মান == ফাঁকা)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		অন্যথা
		{
		    *item->jq_মান = listtv;
		    item->jq_পূর্ব = শীর্ষ->jq_পূর্ব;
		    শীর্ষ->jq_পূর্ব = item;
		    item->jq_পরবর্তী = ফাঁকা;
		    যদি (item->jq_পূর্ব == ফাঁকা)
			শীর্ষ->jq_পরবর্তী = item;
		    অন্যথা
			item->jq_পূর্ব->jq_পরবর্তী = item;
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
		    (পূর্ণসংখ্যা)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_গুনতি() + 100L;
#অন্যথা
	    gettimeofday(&chanpart->ch_deadline, ফাঁকা);
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
	    পূর্ণসংখ্যা timeout;
#ifdef MSWIN
	    timeout = (পূর্ণসংখ্যা)(Get_Tick_গুনতি() - chanpart->ch_deadline) > 0;
#অন্যথা
	    {
		কাঠামো timeval now_tv;

		gettimeofday(&now_tv, ফাঁকা);
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
			(পূর্ণসংখ্যা)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, ফাঁকা);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    অন্যথা
	ret = FALSE;

    vim_free(reader.js_buf);
    ফেরত ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    স্থির শূন্য
remove_cb_গ্রন্থি(cbq_T *শীর্ষ, cbq_T *গ্রন্থি)
{
    যদি (গ্রন্থি->cq_পূর্ব == ফাঁকা)
	শীর্ষ->cq_পরবর্তী = গ্রন্থি->cq_পরবর্তী;
    অন্যথা
	গ্রন্থি->cq_পূর্ব->cq_পরবর্তী = গ্রন্থি->cq_পরবর্তী;
    যদি (গ্রন্থি->cq_পরবর্তী == ফাঁকা)
	শীর্ষ->cq_পূর্ব = গ্রন্থি->cq_পূর্ব;
    অন্যথা
	গ্রন্থি->cq_পরবর্তী->cq_পূর্ব = গ্রন্থি->cq_পূর্ব;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    শূন্য
remove_json_গ্রন্থি(jsonq_T *শীর্ষ, jsonq_T *গ্রন্থি)
{
    যদি (গ্রন্থি->jq_পূর্ব == ফাঁকা)
	শীর্ষ->jq_পরবর্তী = গ্রন্থি->jq_পরবর্তী;
    অন্যথা
	গ্রন্থি->jq_পূর্ব->jq_পরবর্তী = গ্রন্থি->jq_পরবর্তী;
    যদি (গ্রন্থি->jq_পরবর্তী == ফাঁকা)
	শীর্ষ->jq_পূর্ব = গ্রন্থি->jq_পূর্ব;
    অন্যথা
	গ্রন্থি->jq_পরবর্তী->jq_পূর্ব = গ্রন্থি->jq_পূর্ব;
    vim_free(গ্রন্থি);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    স্থির শূন্য
channel_add_block_id(chanpart_T *chanpart, পূর্ণসংখ্যা id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    যদি (gap->ga_growsize == 0)
	ga_init2(gap, আকার(পূর্ণসংখ্যা), 10);
    যদি (ga_grow(gap, 1) == OK)
    {
	((পূর্ণসংখ্যা *)gap->ga_তথ্য)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    স্থির শূন্য
channel_remove_block_id(chanpart_T *chanpart, পূর্ণসংখ্যা id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < gap->ga_len; ++i)
	যদি (((পূর্ণসংখ্যা *)gap->ga_তথ্য)[i] == id)
	{
	    --gap->ga_len;
	    যদি (i < gap->ga_len)
	    {
		পূর্ণসংখ্যা *p = ((পূর্ণসংখ্যা *)gap->ga_তথ্য) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * আকার(পূর্ণসংখ্যা));
	    }
	    ফেরত;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    স্থির পূর্ণসংখ্যা
channel_has_block_id(chanpart_T *chanpart, পূর্ণসংখ্যা id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    পূর্ণসংখ্যা		i;

    জন্য (i = 0; i < gap->ga_len; ++i)
	যদি (((পূর্ণসংখ্যা *)gap->ga_তথ্য)[i] == id)
	    ফেরত TRUE;
    ফেরত FALSE;
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
    স্থির পূর্ণসংখ্যা
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	পূর্ণসংখ্যা	    id,
	পূর্ণসংখ্যা	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *শীর্ষ = &channel->ch_part[part].ch_json_শীর্ষ;
    jsonq_T   *item = শীর্ষ->jq_পরবর্তী;

    যখন (item != ফাঁকা)
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
			যাও nextitem;
		    d = item->jq_মান->vval.v_dict;
		    যদি (d == ফাঁকা)
			যাও nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    যদি (dict_has_চাবি(d, "method"))
			যাও nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    যদি (di == ফাঁকা)
			যাও nextitem;
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
			যাও nextitem;
		    d = item->jq_মান->vval.v_dict;
		    যদি (d == ফাঁকা)
			যাও nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    যদি (di == ফাঁকা)
			যাও nextitem;
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
	    remove_json_গ্রন্থি(শীর্ষ, item);
	    ফেরত OK;
	}
nextitem:
	item = item->jq_পরবর্তী;
    }
    ফেরত FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    স্থির শূন্য
channel_ঠেলো_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *শীর্ষ = &channel->ch_part[part].ch_json_শীর্ষ;
    jsonq_T   *item = শীর্ষ->jq_পরবর্তী;
    jsonq_T   *newitem;

    যদি (শীর্ষ->jq_পূর্ব != ফাঁকা && শীর্ষ->jq_পূর্ব->jq_no_callback)
	// last item was pushed back, append to the end
	item = ফাঁকা;
    অন্যথা যখন (item != ফাঁকা && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_পরবর্তী;

    newitem = ALLOC_ONE(jsonq_T);
    যদি (newitem == ফাঁকা)
    {
	clear_tv(rettv);
	ফেরত;
    }

    newitem->jq_মান = alloc_tv();
    যদি (newitem->jq_মান == ফাঁকা)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ফেরত;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_মান = *rettv;
    যদি (item == ফাঁকা)
    {
	// append to the end
	newitem->jq_পূর্ব = শীর্ষ->jq_পূর্ব;
	শীর্ষ->jq_পূর্ব = newitem;
	newitem->jq_পরবর্তী = ফাঁকা;
	যদি (newitem->jq_পূর্ব == ফাঁকা)
	    শীর্ষ->jq_পরবর্তী = newitem;
	অন্যথা
	    newitem->jq_পূর্ব->jq_পরবর্তী = newitem;
    }
    অন্যথা
    {
	// append after "item"
	newitem->jq_পূর্ব = item;
	newitem->jq_পরবর্তী = item->jq_পরবর্তী;
	item->jq_পরবর্তী = newitem;
	যদি (newitem->jq_পরবর্তী == ফাঁকা)
	    শীর্ষ->jq_পূর্ব = newitem;
	অন্যথা
	    newitem->jq_পরবর্তী->jq_পূর্ব = newitem;
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
    পূর্ণসংখ্যা	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    যদি (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	যদি (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	ফেরত;
    }
    arg = argv[1].vval.v_string;
    যদি (arg == ফাঁকা)
	arg = (char_u *)"";

    যদি (STRCMP(cmd, "ex") == 0)
    {
	পূর্ণসংখ্যা	called_emsg_before = called_emsg;
	char_u	*p = arg;
	পূর্ণসংখ্যা	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (অক্ষর *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	যদি (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	যদি (do_emsg_silent)
	    --emsg_silent;
	যদি (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (অক্ষর *)get_vim_var_str(VV_ERRMSG));
    }
    অন্যথা যদি (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (অক্ষর *)arg);
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
	পূর্ণসংখ্যা is_call = cmd[0] == 'c';
	পূর্ণসংখ্যা id_idx = is_call ? 3 : 2;

	যদি (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_সংখ্যা)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    যদি (p_verbose > 2)
		emsg(_(e_last_argument_for_রাশিমালা_call_must_be_সংখ্যা));
	}
	অন্যথা যদি (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    যদি (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	অন্যথা
	{
	    typval_T	*tv = ফাঁকা;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ফাঁকা;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    যদি (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (অক্ষর *)arg);
		tv = eval_রাশিমালা(arg, ফাঁকা);
	    }
	    অন্যথা
	    {
		ch_log(channel, "Calling '%s'", (অক্ষর *)arg);
		যদি (func_call(arg, &argv[2], ফাঁকা, ফাঁকা, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    যদি (argv[id_idx].v_type == VAR_সংখ্যা)
	    {
		পূর্ণসংখ্যা id = argv[id_idx].vval.v_সংখ্যা;

		যদি (tv != ফাঁকা)
		    json = json_encode_nr_রাশিমালা(id, tv, options | JSON_NL);
		যদি (tv == ফাঁকা || (json != ফাঁকা && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_রাশিমালা(id, &err_tv, options | JSON_NL);
		}
		যদি (json != ফাঁকা)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (পূর্ণসংখ্যা)STRLEN(json), (অক্ষর *)cmd);
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
	ch_error(channel, "Received unknown command: %s", (অক্ষর *)cmd);
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
					    (অক্ষর *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_গ্রন্থি(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    স্থির শূন্য
append_to_বাফার(
    buf_T	*বাফার,
    char_u	*বার্তা,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = বাফার->b_ml.ml_সারি_গুনতি;
    bool	save_write_to = বাফার->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    পূর্ণসংখ্যা		save_p_ma = বাফার->b_p_ma;
    পূর্ণসংখ্যা		empty = (বাফার->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    যদি (!বাফার->b_p_ma && !ch_part->ch_nomodifiable)
    {
	যদি (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ফেরত;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    যদি (save_write_to)
    {
	--lnum;
	বাফার->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (পূর্ণসংখ্যা)lnum + 1 - empty, বাফার->b_fname);

    বাফার->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, বাফার);
    যদি (curbuf != বাফার)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ফেরত;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    যদি (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, বার্তা, TRUE);
	lnum = 0;
    }
    অন্যথা
	ml_append(lnum, বার্তা, 0, FALSE);
    appended_সারিগুলি_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    যদি (ch_part->ch_nomodifiable)
	বাফার->b_p_ma = FALSE;
    অন্যথা
	বাফার->b_p_ma = save_p_ma;

    যদি (বাফার->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    যদি (wp->w_বাফার == বাফার)
	    {
		পূর্ণসংখ্যা move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		যদি (move_cursor || wp->w_topline > বাফার->b_ml.ml_সারি_গুনতি)
		{
		    win_T *save_curwin = curwin;

		    যদি (move_cursor)
			++wp->w_cursor.lnum;
		    curwin = wp;
		    curbuf = curwin->w_বাফার;
		    scroll_cursor_bot(0, FALSE);
		    curwin = save_curwin;
		    curbuf = curwin->w_বাফার;
		}
	    }
	}
	redraw_buf_and_status_later(বাফার, UPD_VALID);
	channel_need_redraw = TRUE;
    }

    যদি (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	বাফার->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    যদি (in_part->ch_bufref.br_buf == বাফার)
		in_part->ch_buf_bot = বাফার->b_ml.ml_সারি_গুনতি;
	}
    }
}

    স্থির শূন্য
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *বার্তা;

    যখন ((বার্তা = channel_get(channel, part, ফাঁকা)) != ফাঁকা)
    {
	ch_log(channel, "Dropping message '%s'", (অক্ষর *)বার্তা);
	vim_free(বার্তা);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    স্থির পূর্ণসংখ্যা
channel_use_json_শীর্ষ(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    ফেরত ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    স্থির পূর্ণসংখ্যা
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*বার্তা = ফাঁকা;
    blob_T	*blob = ফাঁকা;
    typval_T	*listtv = ফাঁকা;
    typval_T	argv[CH_JSON_MAX_ARGS];
    পূর্ণসংখ্যা		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_শীর্ষ;
    cbq_T	*cbitem;
    callback_T	*callback = ফাঁকা;
    buf_T	*বাফার = ফাঁকা;
    char_u	*p;
    পূর্ণসংখ্যা		called_otc;		// one time callbackup
    পূর্ণসংখ্যা		raw_len = 0;

    যদি (channel->ch_nb_close_cb != ফাঁকা
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ফেরত FALSE;

    // Use a message-specific callback, part callback or channel callback
    জন্য (cbitem = cbhead->cq_পরবর্তী; cbitem != ফাঁকা; cbitem = cbitem->cq_পরবর্তী)
	যদি (cbitem->cq_seq_nr == 0)
	    বিরতি;
    যদি (cbitem != ফাঁকা)
	callback = &cbitem->cq_callback;
    অন্যথা যদি (ch_part->ch_callback.cb_name != ফাঁকা)
	callback = &ch_part->ch_callback;
    অন্যথা যদি (channel->ch_callback.cb_name != ফাঁকা)
	callback = &channel->ch_callback;

    বাফার = ch_part->ch_bufref.br_buf;
    যদি (বাফার != ফাঁকা && (!bufref_valid(&ch_part->ch_bufref)
					       || বাফার->b_ml.ml_mfp == ফাঁকা))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = ফাঁকা;
	বাফার = ফাঁকা;
    }

    যদি (channel_use_json_শীর্ষ(channel, part))
    {
	listitem_T	*item;
	পূর্ণসংখ্যা		argc = 0;

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
		ফেরত FALSE;
	}

	যদি (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    যদি (d != ফাঁকা)
	    {
		যদি (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		অন্যথা
		    di = dict_find(d, (char_u *)"request_seq", -1);
		যদি (di != ফাঁকা && di->di_tv.v_type == VAR_সংখ্যা)
		    seq_nr = di->di_tv.vval.v_সংখ্যা;
	    }

	    argv[1] = *listtv;
	}
	অন্যথা
	{
	    জন্য (item = listtv->vval.v_list->lv_first;
		    item != ফাঁকা && argc < CH_JSON_MAX_ARGS;
		    item = item->li_পরবর্তী)
		argv[argc++] = item->li_tv;
	    যখন (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    যদি (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ফেরত TRUE;
	    }

	    যদি (argv[0].v_type != VAR_সংখ্যা)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ফেরত FALSE;
	    }
	    seq_nr = argv[0].vval.v_সংখ্যা;
	}
    }
    অন্যথা যদি (channel_peek(channel, part) == ফাঁকা)
    {
	// nothing to read on RAW or NL channel
	ফেরত FALSE;
    }
    অন্যথা
    {
	// If there is no callback or buffer drop the message.
	যদি (callback == ফাঁকা && বাফার == ফাঁকা)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    যদি (channel->ch_close_cb.cb_name == ফাঁকা && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ফেরত FALSE;
	}

	যদি (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = ফাঁকা;
	    char_u  *buf;
	    readq_T *গ্রন্থি;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    যখন (TRUE)
	    {
		গ্রন্থি = channel_peek(channel, part);
		nl = channel_first_nl(গ্রন্থি);
		যদি (nl != ফাঁকা)
		    বিরতি;
		যদি (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    যদি (ch_part->ch_fd == INVALID_FD && গ্রন্থি->rq_buflen > 0)
			বিরতি;
		    ফেরত FALSE; // incomplete message
		}
	    }
	    buf = গ্রন্থি->rq_বাফার;

	    // Convert NUL to NL, the internal representation.
	    জন্য (p = buf; (nl == ফাঁকা || p < nl)
					    && p < buf + গ্রন্থি->rq_buflen; ++p)
		যদি (*p == NUL)
		    *p = NL;

	    যদি (nl == ফাঁকা)
	    {
		// get the whole buffer, drop the NL
		বার্তা = channel_get(channel, part, ফাঁকা);
	    }
	    অন্যথা যদি (nl + 1 == buf + গ্রন্থি->rq_buflen)
	    {
		// get the whole buffer
		বার্তা = channel_get(channel, part, ফাঁকা);
		*nl = NUL;
	    }
	    অন্যথা
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		বার্তা = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (পূর্ণসংখ্যা)(nl - buf) + 1);
	    }
	}
	অন্যথা
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    বার্তা = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : ফাঁকা);
	}

	যদি (বার্তা == ফাঁকা)
	    ফেরত FALSE; // out of memory (and avoids Coverity warning)

	যদি (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    যদি (blob == ফাঁকা)
	    {
		vim_free(বার্তা);
		ফেরত FALSE;
	    }
	    যদি (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(বার্তা);
		ফেরত FALSE;
	    }
	    যদি (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_তথ্য, বার্তা, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	অন্যথা
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = বার্তা;
	}
    }

    called_otc = FALSE;
    যদি (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	পূর্ণসংখ্যা lsp_req_বার্তা = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	যদি ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_চাবি(argv[1].vval.v_dict, "method"))
	    lsp_req_বার্তা = TRUE;

	যদি (!lsp_req_বার্তা)
	{
	    জন্য (cbitem = cbhead->cq_পরবর্তী; cbitem != ফাঁকা;
		    cbitem = cbitem->cq_পরবর্তী)
	    {
		যদি (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    বিরতি;
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
		listtv = ফাঁকা;
	    }
	    অন্যথা
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    অন্যথা যদি (callback != ফাঁকা || বাফার != ফাঁকা)
    {
	যদি (বাফার != ফাঁকা)
	{
	    যদি (বার্তা == ফাঁকা)
		// JSON or JS mode: re-encode the message.
		বার্তা = json_encode(listtv, ch_mode);
	    যদি (বার্তা != ফাঁকা)
	    {
#ifdef FEAT_TERMINAL
		যদি (বাফার->b_term != ফাঁকা)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    যদি (part == PART_ERR)
		    {
			char_u *cp;
			পূর্ণসংখ্যা    extra = 0;

			জন্য (cp = বার্তা; *cp != NUL; ++cp)
			    যদি (*cp == NL && (cp == বার্তা || cp[-1] != CAR))
				++extra;
			যদি (extra > 0)
			{
			    char_u *crlf_বার্তা = alloc(STRLEN(বার্তা) + extra + 1);

			    যদি (crlf_বার্তা != ফাঁকা)
			    {
				char_u *q = crlf_বার্তা;

				জন্য (cp = বার্তা; *cp != NUL; ++cp)
				{
				    যদি (*cp == NL && (cp == বার্তা || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(বাফার, crlf_বার্তা, channel);
				vim_free(crlf_বার্তা);
			    }
			}
			অন্যথা
			    write_to_term(বাফার, বার্তা, channel);
		    }
		    অন্যথা
			write_to_term(বাফার, বার্তা, channel);
		}
		অন্যথা
#endif
		    append_to_বাফার(বাফার, বার্তা, channel, part);
	    }
	}

	যদি (callback != ফাঁকা)
	{
	    যদি (cbitem != ফাঁকা)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    অন্যথা
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (অক্ষর *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    অন্যথা
	ch_log(channel, "Dropping message %d", seq_nr);

    যদি (listtv != ফাঁকা)
	free_tv(listtv);
    যদি (blob != ফাঁকা)
	blob_unref(blob);
    vim_free(বার্তা);

    ফেরত TRUE;
}

#যদি defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    পূর্ণসংখ্যা
channel_can_write_to(channel_T *channel)
{
    ফেরত channel != ফাঁকা && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    পূর্ণসংখ্যা
channel_is_open(channel_T *channel)
{
    ফেরত channel != ফাঁকা && (channel->CH_SOCK_FD != INVALID_FD
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
    যদি (channel_use_json_শীর্ষ(channel, part))
    {
	jsonq_T   *শীর্ষ = &channel->ch_part[part].ch_json_শীর্ষ;

	যদি (শীর্ষ->jq_পরবর্তী == ফাঁকা)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ফেরত শীর্ষ->jq_পরবর্তী;
    }
    ফেরত channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    স্থির পূর্ণসংখ্যা
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ফেরত channel_readahead_pointer(channel, part) != ফাঁকা;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    স্থির অক্ষর *
channel_status(channel_T *channel, পূর্ণসংখ্যা req_part)
{
    ch_part_T part;
    পূর্ণসংখ্যা has_readahead = FALSE;

    যদি (channel == ফাঁকা)
	 ফেরত "fail";
    যদি (req_part == PART_OUT)
    {
	যদি (channel->CH_OUT_FD != INVALID_FD)
	    ফেরত "open";
	যদি (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    অন্যথা যদি (req_part == PART_ERR)
    {
	যদি (channel->CH_ERR_FD != INVALID_FD)
	    ফেরত "open";
	যদি (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    অন্যথা
    {
	যদি (channel_is_open(channel))
	    ফেরত "open";
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
	    যদি (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		বিরতি;
	    }
    }

    যদি (has_readahead)
	ফেরত "buffered";
    ফেরত "closed";
}

    স্থির শূন্য
channel_part_info(channel_T *channel, dict_T *dict, অক্ষর *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    অক্ষর	namebuf[20];  // longest is "sock_timeout"
    size_t	লেজ;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    লেজ = STRLEN(namebuf);
    STRCPY(namebuf + লেজ, "_");
    লেজ += STRLEN_LITERAL("_");

    STRCPY(namebuf + লেজ, "status");
    যদি (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    অন্যথা যদি (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    অন্যথা
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (পূর্ণসংখ্যা)s.দৈর্ঘ্য);

    STRCPY(namebuf + লেজ, "mode");
    নির্বাচন (chanpart->ch_mode)
    {
	ক্ষেত্র CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    বিরতি;
	ক্ষেত্র CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    বিরতি;
	ক্ষেত্র CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    বিরতি;
	ক্ষেত্র CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    বিরতি;
	ক্ষেত্র CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    বিরতি;
	ক্ষেত্র CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    বিরতি;
	ক্ষেত্র CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    বিরতি;
	পূর্বনির্ধারিত:
	    STR_LITERAL_SET(s, "");
	    বিরতি;
    }
    dict_add_string_len(dict, namebuf, s.string, (পূর্ণসংখ্যা)s.দৈর্ঘ্য);

    STRCPY(namebuf + লেজ, "io");
    যদি (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    অন্যথা নির্বাচন (chanpart->ch_io)
    {
	ক্ষেত্র JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    বিরতি;
	ক্ষেত্র JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    বিরতি;
	ক্ষেত্র JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    বিরতি;
	ক্ষেত্র JIO_বাফার:
	    STR_LITERAL_SET(s, "buffer");
	    বিরতি;
	ক্ষেত্র JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    বিরতি;
	পূর্বনির্ধারিত:
	    STR_LITERAL_SET(s, "");
	    বিরতি;
    }
    dict_add_string_len(dict, namebuf, s.string, (পূর্ণসংখ্যা)s.দৈর্ঘ্য);

    STRCPY(namebuf + লেজ, "timeout");
    dict_add_সংখ্যা(dict, namebuf, chanpart->ch_timeout);
}

    স্থির শূন্য
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_সংখ্যা(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    যদি (channel->ch_hostname != ফাঁকা)
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
channel_close(channel_T *channel, পূর্ণসংখ্যা invoke_close_cb)
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
	যদি (channel->ch_close_cb.cb_name != ফাঁকা)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
	{
	    যদি (channel->ch_close_cb.cb_name != ফাঁকা
			    || channel->ch_part[part].ch_bufref.br_buf != ফাঁকা)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		যদি (channel->ch_close_cb.cb_name == ফাঁকা)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		যখন (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	যদি (channel->ch_close_cb.cb_name != ফাঁকা)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (অক্ষর *)channel->ch_close_cb.cb_name);
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
		  জন্য (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ফাঁকা;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ফাঁকা;
    channel->ch_ss_close_cb = ফাঁকা;
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
    wq->wq_পরবর্তী = entry->wq_পরবর্তী;
    যদি (wq->wq_পরবর্তী == ফাঁকা)
	wq->wq_পূর্ব = ফাঁকা;
    অন্যথা
	wq->wq_পরবর্তী->wq_পূর্ব = ফাঁকা;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    স্থির শূন্য
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_শীর্ষ = &ch_part->ch_json_শীর্ষ;
    cbq_T   *cb_শীর্ষ = &ch_part->ch_cb_শীর্ষ;

    যখন (channel_peek(channel, part) != ফাঁকা)
	vim_free(channel_get(channel, part, ফাঁকা));

    যখন (cb_শীর্ষ->cq_পরবর্তী != ফাঁকা)
    {
	cbq_T *গ্রন্থি = cb_শীর্ষ->cq_পরবর্তী;

	remove_cb_গ্রন্থি(cb_শীর্ষ, গ্রন্থি);
	free_callback(&গ্রন্থি->cq_callback);
	vim_free(গ্রন্থি);
    }

    যখন (json_শীর্ষ->jq_পরবর্তী != ফাঁকা)
    {
	free_tv(json_শীর্ষ->jq_পরবর্তী->jq_মান);
	remove_json_গ্রন্থি(json_শীর্ষ, json_শীর্ষ->jq_পরবর্তী);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    যখন (ch_part->ch_writeque.wq_পরবর্তী != ফাঁকা)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_পরবর্তী);
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

    ch_log(ফাঁকা, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_বার্তা_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    স্থির পূর্ণসংখ্যা
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    যদি (in_part->ch_writeque.wq_পরবর্তী != ফাঁকা)
	ফেরত TRUE;
    যদি (buf == ফাঁকা)
	ফেরত FALSE;
    ফেরত in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_সারি_গুনতি)
	    : (in_part->ch_buf_শীর্ষ <= in_part->ch_buf_bot
			    && in_part->ch_buf_শীর্ষ <= buf->b_ml.ml_সারি_গুনতি);
}

#যদি defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    স্থির পূর্ণসংখ্যা
channel_fill_wfds(পূর্ণসংখ্যা maxfd_arg, fd_set *wfds)
{
    পূর্ণসংখ্যা		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	যদি (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (পূর্ণসংখ্যা)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((পূর্ণসংখ্যা)in_part->ch_fd, wfds);
	    যদি ((পূর্ণসংখ্যা)in_part->ch_fd >= maxfd)
		maxfd = (পূর্ণসংখ্যা)in_part->ch_fd + 1;
	}
    }
    ফেরত maxfd;
}
#অন্যথা
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    স্থির পূর্ণসংখ্যা
channel_fill_poll_write(পূর্ণসংখ্যা nfd_in, কাঠামো pollfd *fds)
{
    পূর্ণসংখ্যা		nfd = nfd_in;
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
    ফেরত nfd;
}
#endif

প্রকারলেখ গণনা {
    CW_READY,
    CW_NOT_READY,
    CW_ERROR
} channel_wait_ফলাফল;

/*
 * Check for reading from "fd" with "timeout" msec.
 * Return CW_READY when there is something to read.
 * Return CW_NOT_READY when there is nothing to read.
 * Return CW_ERROR when there is an error.
 */
    স্থির channel_wait_ফলাফল
channel_wait(channel_T *channel, sock_T fd, পূর্ণসংখ্যা timeout)
{
    যদি (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    যদি (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	পূর্ণসংখ্যা	sleep_time;
	DWORD	deadline = Get_Tick_গুনতি() + timeout;
	পূর্ণসংখ্যা	delay = 1;

	// reading from a pipe, not a socket
	যখন (TRUE)
	{
	    পূর্ণসংখ্যা r = PeekNamedPipe((HANDLE)fd, ফাঁকা, 0, ফাঁকা, &nread, ফাঁকা);

	    যদি (r && nread > 0)
		ফেরত CW_READY;

	    যদি (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ফাঁকা);
	    }
	    অন্যথা যদি (r == 0)
		ফেরত CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_সারিগুলি();

	    sleep_time = deadline - Get_Tick_গুনতি();
	    যদি (sleep_time <= 0)
		বিরতি;
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
	কাঠামো timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	পূর্ণসংখ্যা		ret;
	পূর্ণসংখ্যা		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	জন্য (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((পূর্ণসংখ্যা)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (পূর্ণসংখ্যা)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ফাঁকা, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    যদি (ret == -1 && errno == EINTR)
		চালু;
# endif
	    যদি (ret > 0)
	    {
		যদি (FD_ISSET(fd, &rfds))
		    ফেরত CW_READY;
		channel_write_any_সারিগুলি();
		চালু;
	    }
	    বিরতি;
	}
#অন্যথা
	জন্য (;;)
	{
	    কাঠামো pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    পূর্ণসংখ্যা		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    যদি (poll(fds, nfd, timeout) > 0)
	    {
		যদি (fds[0].revents & POLLIN)
		    ফেরত CW_READY;
		channel_write_any_সারিগুলি();
		চালু;
	    }
	    বিরতি;
	}
#endif
    }
    ফেরত CW_NOT_READY;
}

    স্থির শূন্য
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, পূর্ণসংখ্যা is_err, অক্ষর *func)
{
    অক্ষর	বার্তা[] = "%s(): Read %s from ch_part[%d], closing";

    যদি (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, বার্তা, func, "error", part);
    অন্যথা
	ch_log(channel, বার্তা, func, "EOF", part);

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
    যদি (channel->ch_nb_close_cb != ফাঁকা)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_বার্তা_RAW,
			      (পূর্ণসংখ্যা)STRLEN_LITERAL(DETACH_বার্তা_RAW), FALSE, "PUT ");

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
    যদি (channel->ch_nb_close_cb != ফাঁকা)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    যদি (channel->ch_ss_close_cb != ফাঁকা)
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
channel_read(channel_T *channel, ch_part_T part, অক্ষর *func)
{
    স্থির char_u	*buf = ফাঁকা;
    পূর্ণসংখ্যা			len = 0;
    পূর্ণসংখ্যা			readlen = 0;
    sock_T		fd;
    পূর্ণসংখ্যা			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    যদি (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	ফেরত;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    যদি (buf == ফাঁকা)
    {
	buf = alloc(MAXMSGSIZE);
	যদি (buf == ফাঁকা)
	    ফেরত;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    জন্য (;;)
    {
	যদি (channel_wait(channel, fd, 0) != CW_READY)
	    বিরতি;
	যদি (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    কাঠামো sockaddr_storage	client;

	    newchannel = add_channel();
	    যদি (newchannel == ফাঁকা)
	    {
		ch_error(ফাঁকা, "Cannot allocate channel.");
		ফেরত;
	    }
	    socklen = আকার(client);
	    newfd = accept(fd, (কাঠামো sockaddr*)&client, &socklen);
	    যদি (newfd < 0)
	    {
		ch_error(ফাঁকা, "Cannot accept channel.");
		channel_free(newchannel);
		ফেরত;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    যদি (channel->ch_ss_accept_cb != ফাঁকা)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ফেরত;
	    }
#endif

	    যদি (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		অক্ষর addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((কাঠামো sockaddr_in*)&client)->sin_addr,
			addr, আকার(addr));
		vim_snprintf((অক্ষর *)namebuf, আকার(namebuf), "%s:%d",
			addr,
			ntohs(((কাঠামো sockaddr_in*)&client)->sin_port));
#অন্যথা
		vim_snprintf((অক্ষর *)namebuf, আকার(namebuf), "%s:%d",
		    inet_ntoa(((কাঠামো sockaddr_in*)&client)->sin_addr),
		    ntohs(((কাঠামো sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    অন্যথা যদি (client.ss_family == AF_INET6)
	    {
		অক্ষর addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((কাঠামো sockaddr_in6*)&client)->sin6_addr,
			addr, আকার(addr));
		vim_snprintf((অক্ষর *)namebuf, আকার(namebuf), "[%s]:%d",
			addr,
			ntohs(((কাঠামো sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    অন্যথা যদি (client.ss_family == AF_UNIX)
		vim_snprintf((অক্ষর *)namebuf, আকার(namebuf),
							    "unix:anonymous");
	    অন্যথা
		vim_snprintf((অক্ষর *)namebuf, আকার(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    ফেরত;
	}
	যদি (use_socket)
	    len = sock_read(fd, (অক্ষর *)buf, MAXMSGSIZE);
	অন্যথা
	    len = fd_read(fd, (অক্ষর *)buf, MAXMSGSIZE);
	যদি (len <= 0)
	    বিরতি;	// error or nothing more to read

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
	channel_T *channel, ch_part_T part, পূর্ণসংখ্যা timeout, পূর্ণসংখ্যা raw, পূর্ণসংখ্যা *outlen)
{
    char_u	*buf;
    char_u	*বার্তা;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*গ্রন্থি;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    যখন (TRUE)
    {
	গ্রন্থি = channel_peek(channel, part);
	যদি (গ্রন্থি != ফাঁকা)
	{
	    যদি (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(গ্রন্থি) != ফাঁকা))
		// got a complete message
		বিরতি;
	    যদি (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		চালু;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    যদি (raw || fd == INVALID_FD)
		বিরতি;
	}

	// Wait for up to the channel timeout.
	যদি (fd == INVALID_FD)
	    ফেরত ফাঁকা;
	যদি (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ফেরত ফাঁকা;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    যদি (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != ফাঁকা)
    {
	বার্তা = channel_get_all(channel, part, outlen);
    }
    অন্যথা
    {
	char_u *p;

	buf = গ্রন্থি->rq_বাফার;
	nl = channel_first_nl(গ্রন্থি);

	// Convert NUL to NL, the internal representation.
	জন্য (p = buf; (nl == ফাঁকা || p < nl) && p < buf + গ্রন্থি->rq_buflen; ++p)
	    যদি (*p == NUL)
		*p = NL;

	যদি (nl == ফাঁকা)
	{
	    // must be a closed channel with missing NL
	    বার্তা = channel_get(channel, part, ফাঁকা);
	}
	অন্যথা যদি (nl + 1 == buf + গ্রন্থি->rq_buflen)
	{
	    // get the whole buffer
	    বার্তা = channel_get(channel, part, ফাঁকা);
	    *nl = NUL;
	}
	অন্যথা
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    বার্তা = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (পূর্ণসংখ্যা)(nl - buf) + 1);
	}
    }
    যদি (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != ফাঁকা ? *outlen : (পূর্ণসংখ্যা)STRLEN(বার্তা));
    ফেরত বার্তা;
}

স্থির পূর্ণসংখ্যা channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    পূর্ণসংখ্যা
channel_in_blocking_wait(শূন্য)
{
    ফেরত channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    স্থির শূন্য
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, পূর্ণসংখ্যা timeout)
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
    স্থির পূর্ণসংখ্যা
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	পূর্ণসংখ্যা	    timeout_arg,
	পূর্ণসংখ্যা	    id,
	typval_T    **rettv)
{
    পূর্ণসংখ্যা		more;
    sock_T	fd;
    পূর্ণসংখ্যা		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    পূর্ণসংখ্যা		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    যদি (id >= 0)
	channel_add_block_id(chanpart, id);

    জন্য (;;)
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
	    বিরতি;
	}

	যদি (!more)
	{
	    শূন্য *পূর্ব_readahead_ptr = channel_readahead_pointer(channel, part);
	    শূন্য *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    যদি (channel_parse_messages())
		চালু;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    যদি (readahead_ptr != ফাঁকা && readahead_ptr != পূর্ব_readahead_ptr)
		চালু;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (পূর্ণসংখ্যা)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    যদি (timeout <= 0 || fd == INVALID_FD)
	    {
		যদি (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		বিরতি;
	    }
	    // If there was an incomplete message use the deadline for that.
	    যদি (chanpart->ch_wait_len > 0)
	    {
		পূর্ণসংখ্যা	বাম;
#ifdef MSWIN
		বাম = chanpart->ch_deadline - Get_Tick_গুনতি() + 1;
#অন্যথা
		{
		    কাঠামো timeval now_tv;

		    gettimeofday(&now_tv, ফাঁকা);
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

    ফেরত retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, পূর্ণসংখ্যা check_open, পূর্ণসংখ্যা reading, ch_part_T part)
{
    channel_T	*channel = ফাঁকা;
    পূর্ণসংখ্যা		has_readahead = FALSE;

    যদি (tv->v_type == VAR_JOB)
    {
	যদি (tv->vval.v_job != ফাঁকা)
	    channel = tv->vval.v_job->jv_channel;
    }
    অন্যথা যদি (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    অন্যথা
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ফেরত ফাঁকা;
    }
    যদি (channel != ফাঁকা && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_গুনতি ? part : channel_part_read(channel));

    যদি (check_open && (channel == ফাঁকা || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ফেরত ফাঁকা;
    }
    ফেরত channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    স্থির শূন্য
common_channel_read(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা raw, পূর্ণসংখ্যা blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_গুনতি;
    jobopt_T	opt;
    পূর্ণসংখ্যা		mode;
    পূর্ণসংখ্যা		timeout;
    পূর্ণসংখ্যা		id = -1;
    typval_T	*listtv = ফাঁকা;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    clear_job_options(&opt);
    যদি (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	যাও theend;

    যদি (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    যদি (channel == ফাঁকা)
	যাও theend;

    যদি (part == PART_গুনতি)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    যদি (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    যদি (blob || mode == CH_MODE_BLOB)
    {
	পূর্ণসংখ্যা	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	যদি (p != ফাঁকা)
	{
	    blob_T	*b = blob_alloc();

	    যদি (b != ফাঁকা)
	    {
		b->bv_ga.ga_len = outlen;
		যদি (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		অন্যথা
		{
		    memcpy(b->bv_ga.ga_তথ্য, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    অন্যথা যদি (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, ফাঁকা);
    অন্যথা
    {
	যদি (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	যদি (listtv != ফাঁকা)
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
channel_handle_events(পূর্ণসংখ্যা only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	যদি (only_keep_open && !channel->ch_keep_open)
	    চালু;

	// check the socket and pipes
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    যদি (fd == INVALID_FD)
		চালু;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    পূর্ণসংখ্যা r = channel_wait(channel, fd, (channel->ch_killing &&
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
	যদি (channel->ch_job != ফাঁকা)
	{
	    job_T *job = channel->ch_job;

	    যদি (job->jv_tty_out != ফাঁকা && job->jv_status == JOB_FINISHED)
		জন্য (part = PART_SOCK; part < PART_গুনতি; ++part)
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
    পূর্ণসংখ্যা
channel_any_keep_open(শূন্য)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	যদি (channel->ch_keep_open)
	    ফেরত TRUE;
    ফেরত FALSE;
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
    পূর্ণসংখ্যা		fd = ch_part->ch_fd;

    যদি (fd == INVALID_FD)
	ফেরত;

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
    পূর্ণসংখ্যা
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	পূর্ণসংখ্যা	  len_arg,
	অক্ষর	  *fun)
{
    পূর্ণসংখ্যা		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    পূর্ণসংখ্যা		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    যদি (fd == INVALID_FD)
    {
	যদি (!channel->ch_error && fun != ফাঁকা)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ফেরত FAIL;
    }

    যদি (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    যদি (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_বার্তা = 0;
    }

    জন্য (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	পূর্ণসংখ্যা	    len;

	যদি (wq->wq_পরবর্তী != ফাঁকা)
	{
	    // first write what was queued
	    buf = wq->wq_পরবর্তী->wq_ga.ga_তথ্য;
	    len = wq->wq_পরবর্তী->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	অন্যথা
	{
	    যদি (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ফেরত OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	যদি (part == PART_SOCK)
	    res = sock_write(fd, (অক্ষর *)buf, len);
	অন্যথা
	{
	    res = fd_write(fd, (অক্ষর *)buf, len);
#ifdef MSWIN
	    যদি (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ফাঁকা);
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
	    writeq_T *entry = wq->wq_পরবর্তী;

	    যদি (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    যদি (res == len)
	    {
		// Wrote all the buf[len] bytes.
		যদি (entry != ফাঁকা)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    চালু;
		}
		যদি (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    অন্যথা
	    {
		// Wrote only buf[res] bytes, can't write more now.
		যদি (entry != ফাঁকা)
		{
		    যদি (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_তথ্য,
				    (অক্ষর *)entry->wq_ga.ga_তথ্য + res,
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
		যদি (wq->wq_পূর্ব != ফাঁকা
			&& wq->wq_পূর্ব->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_পূর্ব;

		    // append to the last entry
		    যদি (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((অক্ষর *)last->wq_ga.ga_তথ্য
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		অন্যথা
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    যদি (last != ফাঁকা)
		    {
			last->wq_পূর্ব = wq->wq_পূর্ব;
			last->wq_পরবর্তী = ফাঁকা;
			যদি (wq->wq_পূর্ব == ফাঁকা)
			    wq->wq_পরবর্তী = last;
			অন্যথা
			    wq->wq_পূর্ব->wq_পরবর্তী = last;
			wq->wq_পূর্ব = last;
			ga_init2(&last->wq_ga, 1, 1000);
			যদি (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_তথ্য, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	অন্যথা যদি (res != len)
	{
	    যদি (!channel->ch_error && fun != ফাঁকা)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ফেরত FAIL;
	}

	channel->ch_error = FALSE;
	ফেরত OK;
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
	পূর্ণসংখ্যা	    len,
	পূর্ণসংখ্যা	    id,
	পূর্ণসংখ্যা	    eval,
	jobopt_T    *opt,
	অক্ষর	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel == ফাঁকা)
	ফেরত ফাঁকা;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    যদি (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ফেরত ফাঁকা;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    যদি (opt->jo_callback.cb_name != ফাঁকা && *opt->jo_callback.cb_name != NUL)
    {
	যদি (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ফেরত ফাঁকা;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    যদি (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == ফাঁকা)
	ফেরত channel;
    ফেরত ফাঁকা;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    স্থির শূন্য
ch_রাশিমালা_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    পূর্ণসংখ্যা		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    পূর্ণসংখ্যা		timeout;
    পূর্ণসংখ্যা		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ফেরত;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel == ফাঁকা)
	ফেরত;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    যদি (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ফেরত;
    }

    যদি (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	অক্ষর		*চাবি = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	যদি (rettv_dict_alloc(rettv) == FAIL)
	    ফেরত;

	যদি (check_for_dict_arg(argvars, 1) == FAIL)
	    ফেরত;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)চাবি, -1);
	যদি (argvars[2].v_type == VAR_DICT
		&& dict_has_চাবি(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	যদি (di != ফাঁকা && di->di_tv.v_type != VAR_সংখ্যা
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_মান_for_argument_str), চাবি);
	    ফেরত;
	}

	যদি (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_বার্তা_id;
	    যদি (di == ফাঁকা)
		dict_add_সংখ্যা(d, "seq", id);
	    অন্যথা
		di->di_tv.vval.v_সংখ্যা = id;
	}
	অন্যথা যদি (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_বার্তা_id;
	    যদি (di == ফাঁকা)
		dict_add_সংখ্যা(d, "id", id);
	    অন্যথা
		di->di_tv.vval.v_সংখ্যা = id;
	}
	অন্যথা
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    যদি (di != ফাঁকা && di->di_tv.v_type == VAR_সংখ্যা)
		id = di->di_tv.vval.v_সংখ্যা;
	}
	যদি (ch_mode == CH_MODE_LSP && !dict_has_চাবি(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_বার্তা(&argvars[1]);
    }
    অন্যথা
    {
	id = ++channel->ch_last_বার্তা_id;
	text = json_encode_nr_রাশিমালা(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    যদি (text == ফাঁকা)
	ফেরত;

    channel = send_common(argvars, text, (পূর্ণসংখ্যা)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    যদি (channel != ফাঁকা && eval)
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
	যদি (rettv->vval.v_dict != ফাঁকা)
	    dict_add_সংখ্যা(rettv->vval.v_dict, "seq", id);
    }
    অন্যথা যদি (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	যদি (rettv->vval.v_dict != ফাঁকা)
	    dict_add_সংখ্যা(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    স্থির শূন্য
ch_raw_common(typval_T *argvars, typval_T *rettv, পূর্ণসংখ্যা eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    পূর্ণসংখ্যা		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    পূর্ণসংখ্যা		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ফেরত;

    যদি (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_তথ্য;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    অন্যথা
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (পূর্ণসংখ্যা)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    যদি (channel != ফাঁকা && eval)
    {
	যদি (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	অন্যথা
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, ফাঁকা);
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
    পূর্ণসংখ্যা
channel_poll_setup(পূর্ণসংখ্যা nfd_in, শূন্য *fds_in, পূর্ণসংখ্যা *towait)
{
    পূর্ণসংখ্যা		nfd = nfd_in;
    channel_T	*channel;
    কাঠামো	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
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

    ফেরত nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    পূর্ণসংখ্যা
channel_poll_check(পূর্ণসংখ্যা ret_in, শূন্য *fds_in)
{
    পূর্ণসংখ্যা		ret = ret_in;
    channel_T	*channel;
    কাঠামো	pollfd *fds = fds_in;
    ch_part_T	part;
    পূর্ণসংখ্যা		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
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
	    channel_write_প্রবেশ(channel);
	    --ret;
	}
    }

    ফেরত ret;
}
#endif // UNIX && !HAVE_SELECT

#যদি !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    পূর্ণসংখ্যা
channel_select_setup(
	পূর্ণসংখ্যা maxfd_in,
	শূন্য *rfds_in,
	শূন্য *wfds_in,
	কাঠামো timeval *tv,
	কাঠামো timeval **tvp)
{
    পূর্ণসংখ্যা		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    যদি (fd != INVALID_FD)
	    {
		যদি (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    যদি (*tvp == ফাঁকা || tv->tv_sec > 0
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
		    যদি ((পূর্ণসংখ্যা)fd >= FD_SETSIZE)
			চালু;
# endif
		    FD_SET((পূর্ণসংখ্যা)fd, rfds);
		    যদি (maxfd < (পূর্ণসংখ্যা)fd)
			maxfd = (পূর্ণসংখ্যা)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ফেরত maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    পূর্ণসংখ্যা
channel_select_check(পূর্ণসংখ্যা ret_in, শূন্য *rfds_in, শূন্য *wfds_in)
{
    পূর্ণসংখ্যা		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	জন্য (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    যদি (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (পূর্ণসংখ্যা)fd < FD_SETSIZE
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
		    && (পূর্ণসংখ্যা)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_প্রবেশ(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	যদি (channel->ch_job != ফাঁকা)
	{
	    job_T *job = channel->ch_job;

	    যদি (job->jv_tty_out != ফাঁকা && job->jv_status == JOB_FINISHED)
		জন্য (part = PART_SOCK; part < PART_গুনতি; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ফেরত ret;
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
						(পূর্ণসংখ্যা)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#নতুবা_যদি defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    কাঠামো timeval	tv;
    কাঠামো timeval	*tvp = &tv;
    পূর্ণসংখ্যা			maxfd;
    পূর্ণসংখ্যা			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ফাঁকা, tvp);
    যদি (ret >= 0)
	(শূন্য)channel_select_check(ret, &rfds, &wfds);
#অন্যথা
    কাঠামো pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    পূর্ণসংখ্যা			nfd;
    পূর্ণসংখ্যা			towait = (পূর্ণসংখ্যা)msec;
    পূর্ণসংখ্যা			ret;

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
    যখন (stdio_channel != ফাঁকা && channel_is_open(stdio_channel))
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
    পূর্ণসংখ্যা
channel_parse_messages(শূন্য)
{
    channel_T	*channel = first_channel;
    পূর্ণসংখ্যা		ret = FALSE;
    পূর্ণসংখ্যা		r;
    ch_part_T	part = PART_SOCK;
    স্থির পূর্ণসংখ্যা	recursive = 0;
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
    যদি ((did_repeated_বার্তা & REPEATED_বার্তা_LOOKING) == 0)
    {
	ch_log(ফাঁকা, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_বার্তা = REPEATED_বার্তা_LOOKING;
    }
    যখন (channel != ফাঁকা)
    {
	যদি (recursive == 1)
	{
	    যদি (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_গুনতি);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		চালু;
	    }
	    যদি (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		যদি (channel->ch_job != ফাঁকা)
		    channel->ch_job->jv_channel = ফাঁকা;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		চালু;
	    }
	    যদি (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		চালু;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	যদি (part == PART_OUT
		&& channel->ch_job != ফাঁকা
		&& channel->ch_job->jv_tty_out != ফাঁকা
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    চালু;
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
		চালু;
	    }
	}
	যদি (part < PART_ERR)
	    ++part;
	অন্যথা
	{
	    channel = channel->ch_পরবর্তী;
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

    ফেরত ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    পূর্ণসংখ্যা
channel_any_readahead(শূন্য)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    যখন (channel != ফাঁকা)
    {
	যদি (channel_has_readahead(channel, part))
	    ফেরত TRUE;
	যদি (part < PART_ERR)
	    ++part;
	অন্যথা
	{
	    channel = channel->ch_পরবর্তী;
	    part = PART_SOCK;
	}
    }
    ফেরত FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    পূর্ণসংখ্যা
set_ref_in_channel(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    জন্য (channel = first_channel; !abort && channel != ফাঁকা;
						   channel = channel->ch_পরবর্তী)
	যদি (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, ফাঁকা, ফাঁকা, ফাঁকা);
	}
    ফেরত abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    স্থির ch_part_T
channel_part_send(channel_T *channel)
{
    যদি (channel->CH_SOCK_FD == INVALID_FD)
	ফেরত PART_IN;
    ফেরত PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    স্থির ch_part_T
channel_part_read(channel_T *channel)
{
    যদি (channel->CH_SOCK_FD == INVALID_FD)
	ফেরত PART_OUT;
    ফেরত PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    স্থির ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    যদি (channel == ফাঁকা)
	ফেরত CH_MODE_JSON;
    ফেরত channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    স্থির পূর্ণসংখ্যা
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ফেরত channel->ch_part[part].ch_timeout;
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
	ফেরত;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel != ফাঁকা)
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
	ফেরত;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel != ফাঁকা)
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
	ফেরত;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    যদি (channel != ফাঁকা)
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
	ফেরত;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel == ফাঁকা)
	ফেরত;

    char_u	*what = tv_get_string(&argvars[1]);
    পূর্ণসংখ্যা		part;
    যদি (STRCMP(what, "err") == 0)
	part = PART_ERR;
    অন্যথা যদি (STRCMP(what, "out") == 0)
	part = PART_OUT;
    অন্যথা যদি (STRCMP(what, "in") == 0)
	part = PART_IN;
    অন্যথা
	part = PART_SOCK;
    যদি (channel->ch_part[part].ch_bufref.br_buf != ফাঁকা)
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
	ফেরত;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel == ফাঁকা)
	ফেরত;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    যদি (channel->ch_job != ফাঁকা)
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
	ফেরত;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel != ফাঁকা && rettv_dict_alloc(rettv) == OK)
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
	ফেরত;
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
	ফেরত;
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
    ch_রাশিমালা_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    শূন্য
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_রাশিমালা_common(argvars, rettv, FALSE);
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
	ফেরত;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ফেরত;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    যদি (channel == ফাঁকা)
	ফেরত;
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
    পূর্ণসংখ্যা		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ফাঁকা;

    যদি (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ফেরত;

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
    অক্ষর      *status = channel_status(channel, -1);

    যদি (channel == ফাঁকা)
	vim_snprintf((অক্ষর *)buf, NUMBUFLEN, "channel %s", status);
    অন্যথা
	vim_snprintf((অক্ষর *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    ফেরত buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(পূর্ণসংখ্যা ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	যদি (ch->ch_id == ch_id)
	    ফেরত ch;
    ফেরত ফাঁকা;
}

#endif // FEAT_JOB_CHANNEL
