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

#إذا defined(FEAT_ڪم_CHANNEL)

// TRUE when netbeans is running with a GUI.
#جيڪڏهن_وصف_ٿيل FEAT_GUI
# define CH_HAS_GUI (gui.اندريون_use || gui.starting)
#جيڪڏهن_پورو

// Note: when making changes here also adjust configure.ac.
#جيڪڏهن_وصف_ٿيل MSWIN
// WinSock API is separated from C API, thus we can't use read(), write(),
// errno...
# define SOCK_ERRNO errno = WSAGet_آخري_Error()
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
# define UNIX_رستو_MAX 108
تعريفنوع بنية sockaddr_un {
    پتو_FAMILY sun_family;
    حرف sun_رستو[UNIX_رستو_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#وإلا
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
#جيڪڏهن_پورو

ساكن فراغ channel_read(channel_T *channel, ch_part_T part, حرف *func);
ساكن فراغ channel_wait_any(ڊگهو msec);
ساكن ch_گهڻائي_T channel_get_گهڻائي(channel_T *channel, ch_part_T part);
ساكن صحيح channel_get_timeout(channel_T *channel, ch_part_T part);
ساكن channel_T *channel_open_stdio(فراغ);
ساكن ch_part_T channel_part_send(channel_T *channel);
ساكن ch_part_T channel_part_read(channel_T *channel);

#وصف_ڪريو FOR_ALL_CHANNELS(ch) \
    لكل ((ch) = پهريون_channel; (ch) != عدم; (ch) = (ch)->ch_ايندڙ)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ساكن صحيح محفوظ_منزل_invoke_callback = 0;

#جيڪڏهن_وصف_ٿيل MSWIN
    ساكن صحيح
fd_read(sock_T fd, حرف *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    إذا (!ReadFile(h, buf, (DWORD)len, &nread, عدم))
	أرجع -1;
    أرجع (صحيح)nread;
}

    ساكن صحيح
fd_write(sock_T fd, حرف *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, سائيز, پورو = 0;
    OVERLAPPED	ov;

    طالما (todo > 0)
    {
	إذا (todo > MAX_NAMED_PIPE_سائيز)
	    سائيز = MAX_NAMED_PIPE_سائيز;
	وإلا
	    سائيز = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_ميدان(ov);
	nwrite = 0;
	إذا (!WriteFile(h, buf + پورو, سائيز, &nwrite, &ov))
	{
	    DWORD err = Get_آخري_Error();

	    إذا (err != ERROR_IO_PENDING)
		أرجع -1;
	    إذا (!Get_Overlapped_نتيجو(h, &ov, &nwrite, FALSE))
		أرجع -1;
	    FlushFileBuffers(h);
	}
	وإلا إذا (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    اكسر;
	todo -= nwrite;
	پورو += nwrite;
    }
    أرجع (صحيح)پورو;
}

    ساكن فراغ
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MSWIN
# undef PERROR
# define PERROR(نياپو) (فراغ)semsg("%s: %s", نياپو, strerror_win32(errno))

    ساكن حرف *
strerror_win32(صحيح eno)
{
    ساكن LPVOID msgbuf = عدم;
    char_u *ptr;

    إذا (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = عدم;
    }
    Format_پيغام(
	FORMAT_پيغام_ALLOCATE_بفر |
	FORMAT_پيغام_FROM_SYSTEM |
	FORMAT_پيغام_IGNORE_INSERTS,
	عدم,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	عدم);
    إذا (msgbuf != عدم)
	// chomp \r or \n
	لكل (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    بدل (*ptr)
	    {
		حالة '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    اكسر;
		حالة '\n':
		    إذا (*(ptr + 1) == '\0')
			*ptr = '\0';
		    وإلا
			*ptr = ' ';
		    اكسر;
	    }
    أرجع msgbuf;
}
#جيڪڏهن_پورو

/*
 * The list of all allocated channels.
 */
ساكن channel_T *پهريون_channel = عدم;
ساكن صحيح ايندڙ_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
جوڙيو_channel(فراغ)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    إذا (channel == عدم)
	أرجع عدم;

    channel->ch_id = ايندڙ_ch_id++;
    ch_log(channel, "Created channel");

    لكل (part = PART_SOCK; part < PART_ڳڻپ; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#جيڪڏهن_وصف_ٿيل FEAT_GUI_X11
	channel->ch_part[part].ch_داخلا_Handler = (Xt_داخلا_Id)عدم;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_GTK
	channel->ch_part[part].ch_داخلا_Handler = 0;
#جيڪڏهن_پورو
	channel->ch_part[part].ch_timeout = 2000;
    }

    إذا (پهريون_channel != عدم)
    {
	پهريون_channel->ch_prev = channel;
	channel->ch_ايندڙ = پهريون_channel;
    }
    پهريون_channel = channel;

    channel->ch_refcount = 1;
    أرجع channel;
}

    صحيح
has_any_channel(فراغ)
{
    أرجع پهريون_channel != عدم;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    صحيح
channel_still_useful(channel_T *channel)
{
    صحيح has_sock_نياپو;
    صحيح	has_ٻاهريون_نياپو;
    صحيح	has_err_نياپو;

    // If the job was killed the channel is not expected to work anymore.
    إذا (channel->ch_ڪم_killed && channel->ch_ڪم == عدم)
	أرجع FALSE;

    // If there is a close callback it may still need to be invoked.
    إذا (channel->ch_close_cb.cb_name != عدم)
	أرجع TRUE;

    // If reading from or a buffer it's still useful.
    إذا (channel->ch_part[PART_اندريون].ch_bufref.br_buf != عدم)
	أرجع TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_نياپو = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_سر.rq_ايندڙ != عدم
		|| channel->ch_part[PART_SOCK].ch_json_سر.jq_ايندڙ != عدم;
    has_ٻاهريون_نياپو = channel->ch_part[PART_ٻاهريون].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ٻاهريون].ch_سر.rq_ايندڙ != عدم
		  || channel->ch_part[PART_ٻاهريون].ch_json_سر.jq_ايندڙ != عدم;
    has_err_نياپو = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_سر.rq_ايندڙ != عدم
		  || channel->ch_part[PART_ERR].ch_json_سر.jq_ايندڙ != عدم;
    أرجع (channel->ch_callback.cb_name != عدم && (has_sock_نياپو
		|| has_ٻاهريون_نياپو || has_err_نياپو))
	    || ((channel->ch_part[PART_ٻاهريون].ch_callback.cb_name != عدم
		       || channel->ch_part[PART_ٻاهريون].ch_bufref.br_buf != عدم)
		    && has_ٻاهريون_نياپو)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != عدم
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != عدم)
		    && has_err_نياپو);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    صحيح
channel_can_close(channel_T *channel)
{
    أرجع channel->ch_منزل_be_closed == 0;
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
    إذا (channel->ch_ايندڙ != عدم)
	channel->ch_ايندڙ->ch_prev = channel->ch_prev;
    إذا (channel->ch_prev == عدم)
	پهريون_channel = channel->ch_ايندڙ;
    وإلا
	channel->ch_prev->ch_ايندڙ = channel->ch_ايندڙ;
    vim_free(channel);
}

    ساكن فراغ
channel_free(channel_T *channel)
{
    إذا (اندريون_free_unref_شيون)
	أرجع;

    إذا (محفوظ_منزل_invoke_callback == 0)
	channel->ch_منزل_be_freed = TRUE;
    وإلا
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
    إذا (!channel_still_useful(channel))
    {
	channel_free(channel);
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    صحيح
channel_unref(channel_T *channel)
{
    إذا (channel != عدم && --channel->ch_refcount <= 0)
	أرجع channel_may_free(channel);
    أرجع FALSE;
}

    صحيح
free_اڻ_استعمال_ٿيل_channels_contents(صحيح copyID, صحيح نقاب)
{
    صحيح		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++محفوظ_منزل_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	إذا (!channel_still_useful(ch)
				 && (ch->ch_copyID & نقاب) != (copyID & نقاب))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --محفوظ_منزل_invoke_callback;
    أرجع did_free;
}

    فراغ
free_اڻ_استعمال_ٿيل_channels(صحيح copyID, صحيح نقاب)
{
    channel_T	*ch;
    channel_T	*ch_ايندڙ;

    لكل (ch = پهريون_channel; ch != عدم; ch = ch_ايندڙ)
    {
	ch_ايندڙ = ch->ch_ايندڙ;
	إذا (!channel_still_useful(ch)
				 && (ch->ch_copyID & نقاب) != (copyID & نقاب))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#إذا defined(FEAT_GUI)

# إذا defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    ساكن channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    إذا (fd == INVALID_FD)
	أرجع عدم;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	    إذا (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		أرجع channel;
	    }
    }
    أرجع عدم;
}

    ساكن فراغ
channel_read_fd(صحيح fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    إذا (channel == عدم)
	ch_error(عدم, "Channel for fd %d not found", fd);
    وإلا
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ساكن فراغ
پيغام_From_Server_X11(Xt_اشارو client_معلومات,
		  صحيح *unused1 اڻ_استعمال_ٿيل,
		  Xt_داخلا_Id *unused2 اڻ_استعمال_ٿيل)
{
    channel_read_fd((صحيح)(ڊگهو)client_معلومات);
}
# endif

# ifdef FEAT_GUI_GTK
#  إذا GTK_CHECK_نسخو(3,0,0)
    ساكن gboolean
پيغام_From_Server_Gtk3(GIOChannel *unused1 اڻ_استعمال_ٿيل,
		  GIOCondition unused2 اڻ_استعمال_ٿيل,
		  gpointer client_معلومات)
{
    channel_read_fd(GPOINTER_منزل_INT(client_معلومات));
    أرجع TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  وإلا
    ساكن فراغ
پيغام_From_Server_Gtk2(gpointer client_معلومات,
		  gint unused1 اڻ_استعمال_ٿيل,
		  Gdk_داخلا_Condition unused2 اڻ_استعمال_ٿيل)
{
    channel_read_fd((صحيح)(ڊگهو)client_معلومات);
}
#  endif
# endif

    ساكن فراغ
channel_gui_رجسٽر_one(channel_T *channel, ch_part_T part اڻ_استعمال_ٿيل)
{
    إذا (!CH_HAS_GUI)
	أرجع;

    // gets stuck in handling events for a not connected channel
    إذا (channel->ch_keep_open)
	أرجع;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    إذا (channel->ch_part[part].ch_داخلا_Handler == (Xt_داخلا_Id)عدم)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_نالا[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_داخلا_Handler = Xt_App_جوڙيو_داخلا(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(Xt_اشارو)(Xt_داخلا_Read_نقاب + Xt_داخلا_Except_نقاب),
		پيغام_From_Server_X11,
		(Xt_اشارو)(ڊگهو)channel->ch_part[part].ch_fd);
    }
# وإلا
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    إذا (channel->ch_part[part].ch_داخلا_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_نالا[part], channel->ch_part[part].ch_fd);
#   إذا GTK_CHECK_نسخو(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_داخلا_Handler = g_io_جوڙيو_watch(
		chnnl,
		G_IO_اندريون|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		پيغام_From_Server_Gtk3,
		GINT_منزل_اشارو(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   وإلا
	channel->ch_part[part].ch_داخلا_Handler = gdk_داخلا_جوڙيو(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_داخلا_Condition)
			     ((صحيح)GDK_داخلا_READ + (صحيح)GDK_داخلا_EXCEPTION),
		پيغام_From_Server_Gtk2,
		(gpointer)(ڊگهو)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ساكن فراغ
channel_gui_رجسٽر(channel_T *channel)
{
    إذا (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_رجسٽر_one(channel, PART_SOCK);
    إذا (channel->CH_ٻاهريون_FD != INVALID_FD
	    && channel->CH_ٻاهريون_FD != channel->CH_SOCK_FD)
	channel_gui_رجسٽر_one(channel, PART_ٻاهريون);
    إذا (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_ٻاهريون_FD)
	channel_gui_رجسٽر_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    فراغ
channel_gui_رجسٽر_all(فراغ)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_رجسٽر(channel);
}

    ساكن فراغ
channel_gui_unregister_one(channel_T *channel اڻ_استعمال_ٿيل, ch_part_T part اڻ_استعمال_ٿيل)
{
# ifdef FEAT_GUI_X11
    إذا (channel->ch_part[part].ch_داخلا_Handler != (Xt_داخلا_Id)عدم)
    {
	ch_log(channel, "Unregistering part %s", ch_part_نالا[part]);
	Xt_Remove_داخلا(channel->ch_part[part].ch_داخلا_Handler);
	channel->ch_part[part].ch_داخلا_Handler = (Xt_داخلا_Id)عدم;
    }
# وإلا
#  ifdef FEAT_GUI_GTK
    إذا (channel->ch_part[part].ch_داخلا_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_نالا[part]);
#   إذا GTK_CHECK_نسخو(3,0,0)
	g_ذريعو_remove(channel->ch_part[part].ch_داخلا_Handler);
#   وإلا
	gdk_داخلا_remove(channel->ch_part[part].ch_داخلا_Handler);
#   endif
	channel->ch_part[part].ch_داخلا_Handler = 0;
    }
#  endif
# endif
}

    ساكن فراغ
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	channel_gui_unregister_one(channel, part);
}

#جيڪڏهن_پورو  // FEAT_GUI

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
#جيڪڏهن_وصف_ٿيل MSWIN
    u_long	val = 1;
#جيڪڏهن_پورو

    طالما (TRUE)
    {
	ڊگهو	elapsed_msec = 0;
	صحيح	waitnow;
	صحيح	ret;

	إذا (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	إذا (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_اندريون_channel_connect));
	    أرجع -1;
	}

	إذا (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    إذا (
#جيڪڏهن_وصف_ٿيل MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#وإلا
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#جيڪڏهن_پورو
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		أرجع -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	إذا (ret == 0)
	    // The connection could be established.
	    اكسر;

	SOCK_ERRNO;
	إذا (*waittime < 0 || (errno != EWOULDBLOCK
		&& errno != ECONNREFUSED
#جيڪڏهن_وصف_ٿيل EINPROGRESS
		&& errno != EINPROGRESS
#جيڪڏهن_پورو
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
	    PERROR(_(e_cannot_connect_منزل_پورٽ));
	    sock_close(sd);
	    أرجع -1;
	}
	وإلا إذا (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    أرجع -1;
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
#جيڪڏهن_وصف_نه_ٿيل MSWIN
	    صحيح			so_error = 0;
	    socklen_t		so_error_len = حجم(so_error);
	    بنية timeval	شروعات_tv;
	    بنية timeval	پڇاڙي_tv;
#جيڪڏهن_پورو
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#جيڪڏهن_وصف_نه_ٿيل MSWIN
	    gettimeofday(&شروعات_tv, عدم);
#جيڪڏهن_پورو
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, عدم, &tv);
	    إذا (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_منزل_پورٽ));
		sock_close(sd);
		أرجع -1;
	    }

#جيڪڏهن_وصف_ٿيل MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    إذا (FD_ISSET(sd, &wfds))
		اكسر;
	    elapsed_msec = waitnow;
	    إذا (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		استمر;
	    }
#وإلا
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    إذا (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		إذا (ret < 0 || (so_error != 0
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
		    PERROR(_(e_cannot_connect_منزل_پورٽ));
		    sock_close(sd);
		    أرجع -1;
		}
		وإلا إذا (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    أرجع -1;
		}
	    }

	    إذا (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		اكسر;

	    gettimeofday(&پڇاڙي_tv, عدم);
	    elapsed_msec = (پڇاڙي_tv.tv_sec - شروعات_tv.tv_sec) * 1000
				 + (پڇاڙي_tv.tv_usec - شروعات_tv.tv_usec) / 1000;
#جيڪڏهن_پورو
	}

#جيڪڏهن_وصف_نه_ٿيل MSWIN
	إذا (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    إذا (waitnow > 0)
	    {
		mch_delay((ڊگهو)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    إذا (!got_int)
	    {
		إذا (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		استمر;
	    }
	    // we were interrupted, behave as if timed out
	}
#جيڪڏهن_پورو

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	أرجع -1;
    }

    إذا (*waittime >= 0)
    {
#جيڪڏهن_وصف_ٿيل MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#وإلا
	(فراغ)fcntl(sd, F_SETFL, 0);
#جيڪڏهن_پورو
    }

    أرجع sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ثابت حرف *رستو,
	فراغ (*nb_close_cb)(فراغ))
{
    channel_T		*channel = عدم;
    صحيح			sd = -1;
    size_t		رستو_len = STRLEN(رستو);
    بنية sockaddr_un	server;
    size_t		server_len;
    صحيح			waittime = -1;

    إذا (*رستو == NUL || رستو_len >= حجم(server.sun_رستو))
    {
	semsg(_(e_invalid_دليل_str), رستو);
	أرجع عدم;
    }

#جيڪڏهن_وصف_ٿيل MSWIN
    channel_init_winsock();
#جيڪڏهن_پورو

    channel = جوڙيو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    CLEAR_ميدان(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_رستو, رستو, حجم(server.sun_رستو) - 1);

    ch_log(channel, "Trying to connect to %s", رستو);

    server_len = offsetof(بنية sockaddr_un, sun_رستو) + رستو_len + 1;
    sd = channel_connect(channel, (بنية sockaddr *)&server, (صحيح)server_len,
								   &waittime);

    إذا (sd < 0)
    {
	channel_free(channel);
	أرجع عدم;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)رستو);
    channel->ch_پورٽ = 0;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    channel_gui_رجسٽر_one(channel, PART_SOCK);
#جيڪڏهن_پورو

    أرجع channel;
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
	صحيح پورٽ,
	صحيح waittime,
	فراغ (*nb_close_cb)(فراغ))
{
    صحيح			sd = -1;
    channel_T		*channel = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_IPV6
    صحيح			err;
    بنية addrinfo	hints;
    بنية addrinfo	*res = عدم;
    بنية addrinfo	*addr = عدم;
#وإلا
    بنية sockaddr_اندريون	server;
    بنية hostent	*host = عدم;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MSWIN
    channel_init_winsock();
#جيڪڏهن_پورو

    channel = جوڙيو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#جيڪڏهن_وصف_ٿيل FEAT_IPV6
    CLEAR_ميدان(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# إذا defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    إذا ((err = getaddrinfo(hostname, عدم, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_اندريون_channel_open_str), gai_strerror(err));
	channel_free(channel);
	أرجع عدم;
    }

    // Count the number of addresses for timeout distribution
    صحيح addr_ڳڻپ = 0;
    لكل (addr = res; addr != عدم; addr = addr->ai_ايندڙ)
	addr_ڳڻپ++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    إذا (waittime == 0)
	waittime = 15;

    صحيح addr_اشاريو = 0;
    لكل (addr = res; addr != عدم; addr = addr->ai_ايندڙ, addr_اشاريو++)
    {
	ثابت حرف  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ثابت فراغ  *src = عدم;
	حرف	    buf[NUMBUFLEN];
# endif
	صحيح	    try_waittime;
	صحيح	    before_waittime;
	صحيح	    consumed;
	صحيح	    بچيل_addrs;

	إذا (addr->ai_family == AF_INET6)
	{
	    بنية sockaddr_in6 *sai = (بنية sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_پورٽ = htons(پورٽ);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	وإلا إذا (addr->ai_family == AF_INET)
	{
	    بنية sockaddr_اندريون *sai = (بنية sockaddr_اندريون *)addr->ai_addr;

	    sai->sin_پورٽ = htons(پورٽ);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	إذا (src != عدم)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, حجم(buf));
	    إذا (dst == عدم)
		dst = hostname;
	    وإلا إذا (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, پورٽ);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	إذا (addr->ai_ايندڙ == عدم)
	    try_waittime = waittime;
	وإلا إذا (addr_اشاريو == 0)
	{
	    إذا (waittime > 500)
		try_waittime = 250;
	    وإلا إذا (waittime > 30)
		try_waittime = waittime / 2;
	    وإلا
		try_waittime = waittime;
	}
	وإلا
	{
	    بچيل_addrs = addr_ڳڻپ - addr_اشاريو;
	    try_waittime = waittime / بچيل_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (صحيح)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	إذا (waittime < 0)
	    waittime = 0;

	إذا (sd >= 0)
	    اكسر;

	// If we have no time left, stop trying
	إذا (waittime <= 0 && addr->ai_ايندڙ != عدم)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    اكسر;
	}
    }

    freeaddrinfo(res);
#وإلا
    CLEAR_ميدان(server);
    server.sin_family = AF_INET;
    server.sin_پورٽ = htons(پورٽ);
    إذا ((host = gethostbyname(hostname)) == عدم)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_اندريون_channel_open));
	channel_free(channel);
	أرجع عدم;
    }
    {
	حرف *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	يادگيري_نقل(&p, &host->h_addr_list[0], حجم(p));
	يادگيري_نقل((حرف *)&server.sin_addr, p, host->h_ڊيگهائي);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, پورٽ);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    إذا (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (بنية sockaddr *)&server, حجم(server),
								   &waittime);
#جيڪڏهن_پورو

    إذا (sd < 0)
    {
	channel_free(channel);
	أرجع عدم;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)hostname);
    channel->ch_پورٽ = پورٽ;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    channel_gui_رجسٽر_one(channel, PART_SOCK);
#جيڪڏهن_پورو

    أرجع channel;
}

    ساكن فراغ
free_سيٽ_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    إذا (callback->cb_name != عدم && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    وإلا
	cbp->cb_name = عدم;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ساكن فراغ
prepare_بفر(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#جيڪڏهن_وصف_ٿيل FEAT_QUICKFIX
    سيٽ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    سيٽ_option_قدر_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#جيڪڏهن_پورو
    إذا (curbuf->b_ml.ml_mfp == عدم)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ساكن buf_T *
channel_ڳوليو_بفر(char_u *name, صحيح err, صحيح نياپو)
{
    buf_T *buf = عدم;
    buf_T *save_curbuf = curbuf;

    إذا (name != عدم && *name != NUL)
    {
	buf = buflist_findname(name);
	إذا (buf == عدم)
	    buf = buflist_findname_exp(name);
    }

    إذا (buf != عدم)
	أرجع buf;

    buf = buflist_new(name == عدم || *name == NUL ? عدم : name,
	    عدم, (linenr_T)0, BLN_LISTED | BLN_NEW);
    إذا (buf == عدم)
	أرجع عدم;
    prepare_بفر(buf);

    curbuf = buf;
    إذا (نياپو)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    بدليو_بائيٽون(1, 0);
    curbuf = save_curbuf;

    أرجع buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ساكن فراغ
channel_سيٽ_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    إذا (opt->jo_سيٽ & JO_گهڻائي)
	لكل (part = PART_SOCK; part < PART_ڳڻپ; ++part)
	    channel->ch_part[part].ch_گهڻائي = opt->jo_گهڻائي;
    إذا (opt->jo_سيٽ & JO_اندريون_گهڻائي)
	channel->ch_part[PART_اندريون].ch_گهڻائي = opt->jo_اندريون_گهڻائي;
    إذا (opt->jo_سيٽ & JO_ٻاهريون_گهڻائي)
	channel->ch_part[PART_ٻاهريون].ch_گهڻائي = opt->jo_ٻاهريون_گهڻائي;
    إذا (opt->jo_سيٽ & JO_ERR_گهڻائي)
	channel->ch_part[PART_ERR].ch_گهڻائي = opt->jo_err_گهڻائي;
    channel->ch_nonblock = opt->jo_noblock;

    إذا (opt->jo_سيٽ & JO_TIMEOUT)
	لكل (part = PART_SOCK; part < PART_ڳڻپ; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    إذا (opt->jo_سيٽ & JO_ٻاهريون_TIMEOUT)
	channel->ch_part[PART_ٻاهريون].ch_timeout = opt->jo_ٻاهريون_timeout;
    إذا (opt->jo_سيٽ & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    إذا (opt->jo_سيٽ & JO_بلاڪ_WRITE)
	channel->ch_part[PART_اندريون].ch_بلاڪ_write = 1;

    إذا (opt->jo_سيٽ & JO_CALLBACK)
	free_سيٽ_callback(&channel->ch_callback, &opt->jo_callback);
    إذا (opt->jo_سيٽ & JO_ٻاهريون_CALLBACK)
	free_سيٽ_callback(&channel->ch_part[PART_ٻاهريون].ch_callback,
							      &opt->jo_ٻاهريون_cb);
    إذا (opt->jo_سيٽ & JO_ERR_CALLBACK)
	free_سيٽ_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    إذا (opt->jo_سيٽ & JO_CLOSE_CALLBACK)
	free_سيٽ_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    إذا ((opt->jo_سيٽ & JO_ٻاهريون_IO) && opt->jo_io[PART_ٻاهريون] == JIO_بفر)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	إذا (!(opt->jo_سيٽ & JO_ٻاهريون_گهڻائي))
	    channel->ch_part[PART_ٻاهريون].ch_گهڻائي = CH_گهڻائي_NL;
	إذا (opt->jo_سيٽ & JO_ٻاهريون_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ٻاهريون]);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist),
					       (ڊگهو)opt->jo_io_buf[PART_ٻاهريون]);
	}
	وإلا
	{
	    صحيح نياپو = TRUE;

	    إذا (opt->jo_set2 & JO2_ٻاهريون_نياپو)
		نياپو = opt->jo_پيغام[PART_ٻاهريون];
	    buf = channel_ڳوليو_بفر(opt->jo_io_name[PART_ٻاهريون], FALSE, نياپو);
	}
	إذا (buf != عدم)
	{
	    إذا (opt->jo_سيٽ & JO_ٻاهريون_MODIFIABLE)
		channel->ch_part[PART_ٻاهريون].ch_nomodifiable =
						!opt->jo_modifiable[PART_ٻاهريون];

	    إذا (!buf->b_p_ma && !channel->ch_part[PART_ٻاهريون].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    وإلا
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (حرف *)buf->b_ffname);
		سيٽ_bufref(&channel->ch_part[PART_ٻاهريون].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		إذا (buf->b_ml.ml_mfp == عدم)
		    prepare_بفر(buf);
	    }
	}
    }

    إذا ((opt->jo_سيٽ & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_بفر
	 || (opt->jo_io[PART_ERR] == JIO_ٻاهريون && (opt->jo_سيٽ & JO_ٻاهريون_IO)
				       && opt->jo_io[PART_ٻاهريون] == JIO_بفر)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	إذا (!(opt->jo_سيٽ & JO_ERR_گهڻائي))
	    channel->ch_part[PART_ERR].ch_گهڻائي = CH_گهڻائي_NL;
	إذا (opt->jo_io[PART_ERR] == JIO_ٻاهريون)
	    buf = channel->ch_part[PART_ٻاهريون].ch_bufref.br_buf;
	وإلا إذا (opt->jo_سيٽ & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist),
					       (ڊگهو)opt->jo_io_buf[PART_ERR]);
	}
	وإلا
	{
	    صحيح نياپو = TRUE;

	    إذا (opt->jo_set2 & JO2_ERR_نياپو)
		نياپو = opt->jo_پيغام[PART_ERR];
	    buf = channel_ڳوليو_بفر(opt->jo_io_name[PART_ERR], TRUE, نياپو);
	}
	إذا (buf != عدم)
	{
	    إذا (opt->jo_سيٽ & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    إذا (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    وإلا
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (حرف *)buf->b_ffname);
		سيٽ_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		إذا (buf->b_ml.ml_mfp == عدم)
		    prepare_بفر(buf);
	    }
	}
    }

    channel->ch_part[PART_ٻاهريون].ch_io = opt->jo_io[PART_ٻاهريون];
    channel->ch_part[PART_ERR].ch_io = opt->jo_io[PART_ERR];
    channel->ch_part[PART_اندريون].ch_io = opt->jo_io[PART_اندريون];
}

/*
 * Parse the address for socketserver and store port in "port", or the path in
 * "unix_path" if it is a unix domain socket. Returns OK on success and FAIL on
 * failure.
 */
    صحيح
channel_parse_socketserver_پتو(
	char_u	*پتو,
	صحيح	*پورٽ,
	char_u	**unix_رستو,
	bool	quiet)
{
    حرف    *rest;
    ڊگهو    val;

    إذا (*پتو == NUL)
	اذهب fail;

    إذا (STRNCMP(پتو, "unix:", 5) == 0)
    {
	*unix_رستو = vim_strsave(پتو + 5);
	أرجع OK;
    }

    val = strtol((حرف *)(پتو), &rest, 10);
    إذا (val < 0 || val >= 65536 || *rest != NUL)
    {
	إذا (!quiet)
	    semsg(_(e_invalid_دليل_str), پتو);
	أرجع FAIL;
    }
    *پورٽ = (صحيح)val;

    أرجع OK;
fail:
    إذا (!quiet)
	semsg(_(e_invalid_دليل_str), پتو);
    أرجع FAIL;
}

/*
 * Implements ch_open().
 */
    ساكن channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*پتو;
    char_u	*p;
    حرف	*rest;
    صحيح		پورٽ = 0;
    صحيح		is_ipv6 = FALSE;
    صحيح		is_unix = FALSE;
    صحيح		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = عدم;

    إذا (اندريون_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع عدم;

    پتو = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	أرجع عدم;

    إذا (*پتو == NUL)
    {
	semsg(_(e_invalid_دليل_str), پتو);
	أرجع عدم;
    }

    إذا (STRCMP(پتو, "stdio") == 0)
	is_stdio = TRUE;
    وإلا إذا (STRNCMP(پتو, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	پتو += 5;
    }
    وإلا إذا (*پتو == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(پتو + 1, ']');
	إذا (p == عدم || *++p != ':')
	{
	    semsg(_(e_invalid_دليل_str), پتو);
	    أرجع عدم;
	}
    }
    وإلا
    {
	// ipv4 address
	p = vim_strchr(پتو, ':');
	إذا (p == عدم)
	{
	    semsg(_(e_invalid_دليل_str), پتو);
	    أرجع عدم;
	}
    }

    إذا (!is_unix && !is_stdio)
    {
	پورٽ = strtol((حرف *)(p + 1), &rest, 10);
	إذا (پورٽ <= 0 || پورٽ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_دليل_str), پتو);
	    أرجع عدم;
	}
	إذا (is_ipv6)
	{
	    // strip '[' and ']'
	    ++پتو;
	    *(p - 1) = NUL;
	}
	وإلا
	    *p = NUL;
    }

    // parse options
    clear_ڪم_options(&opt);
    opt.jo_گهڻائي = CH_گهڻائي_JSON;
    opt.jo_timeout = 2000;
    إذا (get_ڪم_options(&argvars[1], &opt,
	    JO_گهڻائي_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	اذهب theend;
    إذا (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_دليل));
	اذهب theend;
    }

    إذا (is_stdio)
	channel = channel_open_stdio();
    وإلا إذا (is_unix)
	channel = channel_open_unix((حرف *)پتو, عدم);
    وإلا
	channel = channel_open((حرف *)پتو, پورٽ, opt.jo_waittime, عدم);
    إذا (channel != عدم)
    {
	صحيح	user_سيٽ = opt.jo_سيٽ;

	opt.jo_سيٽ = JO_ALL;
	إذا (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_سيٽ &= ~((JO_اندريون_گهڻائي | JO_ٻاهريون_گهڻائي | JO_ERR_گهڻائي
				| JO_ٻاهريون_TIMEOUT | JO_ERR_TIMEOUT) & ~user_سيٽ);
	channel_سيٽ_options(channel, &opt);
    }
theend:
    free_ڪم_options(&opt);
    أرجع channel;
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
    sock_T	اندريون_fd;		// our stdin, the channel reads from it
    sock_T	ٻاهريون_fd;		// our stdout, the channel writes to it

    إذا (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	أرجع عدم;
    }
    إذا (stdio_channel != عدم)
    {
	emsg(_(e_cannot_open_stdio_channel));
	أرجع عدم;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#جيڪڏهن_وصف_ٿيل MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	صحيح	nul_fd;

	إذا (!DuplicateHandle(proc, GetStdHandle(STD_داخلا_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    أرجع عدم;
	}
	إذا (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    أرجع عدم;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, عدم, OPEN_EXISTING, 0,
		عدم);
	إذا (hnul != INVALID_HANDLE_قدر)
	{
	    سيٽ_Std_Handle(STD_داخلا_HANDLE, hnul);
	    سيٽ_Std_Handle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	إذا (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	اندريون_fd = (sock_T)hin;
	ٻاهريون_fd = (sock_T)hout;
    }
#وإلا
    {
	صحيح	nul_fd;

	اندريون_fd = dup(0);
	ٻاهريون_fd = dup(1);
	إذا (اندريون_fd < 0 || ٻاهريون_fd < 0)
	{
	    إذا (اندريون_fd >= 0)
		close(اندريون_fd);
	    إذا (ٻاهريون_fd >= 0)
		close(ٻاهريون_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    أرجع عدم;
	}
	(فراغ)fcntl(اندريون_fd, F_SETFD, FD_CLOEXEC);
	(فراغ)fcntl(ٻاهريون_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	إذا (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#جيڪڏهن_پورو

    channel = جوڙيو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	fd_close(اندريون_fd);
	fd_close(ٻاهريون_fd);
	أرجع عدم;
    }
    channel_سيٽ_pipes(channel, ٻاهريون_fd, اندريون_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    أرجع channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    حرف	*rest;
    صحيح		پورٽ;
    صحيح		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = عدم;

    إذا (اندريون_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	أرجع عدم;

    arg = tv_get_string(&argvars[0]);
    إذا (*arg == NUL)
    {
	semsg(_(e_invalid_دليل_str), arg);
	أرجع عدم;
    }

    إذا (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	پورٽ = 0;
    }
    وإلا
    {
	پورٽ = strtol((حرف *)(arg), &rest, 10);
	إذا (پورٽ < 0 || پورٽ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_دليل_str), arg);
	    أرجع عدم;
	}
	*arg = NUL;
    }

    // parse options
    clear_ڪم_options(&opt);
    opt.jo_گهڻائي = CH_گهڻائي_JSON;
    opt.jo_timeout = 2000;
    إذا (get_ڪم_options(&argvars[1], &opt,
	    JO_گهڻائي_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	اذهب theend;
    إذا (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_دليل));
	اذهب theend;
    }

    إذا (is_unix)
	channel = channel_listen_unix((حرف *)arg, عدم, true);
    وإلا
	channel = channel_listen(پورٽ, عدم);
    إذا (channel != عدم)
    {
	opt.jo_سيٽ = JO_ALL;
	channel_سيٽ_options(channel, &opt);
    }
theend:
    free_ڪم_options(&opt);
    أرجع channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	صحيح پورٽ_اندريون,
	فراغ (*nb_close_cb)(فراغ))
{
    صحيح			sd = -1;
    بنية sockaddr_اندريون	server;
#جيڪڏهن_وصف_نه_ٿيل FEAT_IPV6
    بنية hostent	*host;
#جيڪڏهن_پورو
    صحيح			val = 1;
    channel_T		*channel;

#جيڪڏهن_وصف_ٿيل MSWIN
    channel_init_winsock();
#جيڪڏهن_پورو

    channel = جوڙيو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((حرف *)&server, 0, حجم(server));
    server.sin_family = AF_INET;
    server.sin_پورٽ = htons(پورٽ_اندريون);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    إذا (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٽ));
	channel_free(channel);
	أرجع عدم;
    }

#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ثابت حرف *)&val, حجم(val)) < 0)
#وإلا
    إذا (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, حجم(val)) < 0)
#جيڪڏهن_پورو
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٽ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    // Bind the socket to the port
    إذا (bind(sd, (بنية sockaddr *)&server, حجم(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٽ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    إذا (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٽ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    إذا (پورٽ_اندريون == 0)
    {
	بنية sockaddr_اندريون	addr;
	socklen_t		addr_len = حجم(addr);

	إذا (getsockname(sd, (بنية sockaddr *)&addr, &addr_len) == 0)
	    پورٽ_اندريون = ntohs(addr.sin_پورٽ);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)"");
    channel->ch_پورٽ = پورٽ_اندريون;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    channel_gui_رجسٽر_one(channel, PART_SOCK);
#جيڪڏهن_پورو

    أرجع channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	حرف *رستو,
	فراغ (*nb_close_cb)(فراغ),
	bool replace)
{
    صحيح			sd = -1;
    بنية sockaddr_un	server;
    channel_T		*channel;
    size_t		رستو_len;
    size_t		server_len;

    إذا (رستو == عدم || *رستو == NUL)
    {
	semsg(_(e_invalid_دليل_str), رستو == عدم ? (حرف *)"" : رستو);
	أرجع عدم;
    }

    رستو_len = STRLEN(رستو);
    إذا (رستو_len >= حجم(server.sun_رستو))
    {
	semsg(_(e_invalid_دليل_str), رستو);
	أرجع عدم;
    }

#جيڪڏهن_وصف_ٿيل MSWIN
    channel_init_winsock();
#جيڪڏهن_پورو

    channel = جوڙيو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    CLEAR_ميدان(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_رستو, رستو, حجم(server.sun_رستو) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    إذا (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٽ));
	channel_free(channel);
	أرجع عدم;
    }

    إذا (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_رستو);

    // Bind the socket to the path
    server_len = offsetof(بنية sockaddr_un, sun_رستو) + رستو_len + 1;
    إذا (bind(sd, (بنية sockaddr *)&server, (صحيح)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٽ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    إذا (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٽ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)رستو);
    channel->ch_پورٽ = 0;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    channel_gui_رجسٽر_one(channel, PART_SOCK);
#جيڪڏهن_پورو

    أرجع channel;
}

     فراغ
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    إذا (*fd == INVALID_FD)
	أرجع;

    إذا (part == PART_SOCK)
	sock_close(*fd);
    وإلا
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	إذا ((part == PART_اندريون || channel->CH_اندريون_FD != *fd)
		&& (part == PART_ٻاهريون || channel->CH_ٻاهريون_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#جيڪڏهن_وصف_ٿيل MSWIN
	    إذا (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#جيڪڏهن_پورو
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_منزل_be_closed &= ~(1U << part);
}

    فراغ
channel_سيٽ_pipes(channel_T *channel, sock_T اندريون, sock_T ٻاهريون, sock_T err)
{
    إذا (اندريون != INVALID_FD)
    {
	ch_close_part(channel, PART_اندريون);
	channel->CH_اندريون_FD = اندريون;
#إذا defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	إذا (mch_isatty(اندريون))
	    channel->ch_منزل_be_closed |= (1U << PART_اندريون);
#جيڪڏهن_پورو
    }
    إذا (ٻاهريون != INVALID_FD)
    {
#إذا defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ٻاهريون);
#جيڪڏهن_پورو
	ch_close_part(channel, PART_ٻاهريون);
	channel->CH_ٻاهريون_FD = ٻاهريون;
	channel->ch_منزل_be_closed |= (1U << PART_ٻاهريون);
#إذا defined(FEAT_GUI)
	channel_gui_رجسٽر_one(channel, PART_ٻاهريون);
#جيڪڏهن_پورو
    }
    إذا (err != INVALID_FD)
    {
#إذا defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#جيڪڏهن_پورو
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_منزل_be_closed |= (1U << PART_ERR);
#إذا defined(FEAT_GUI)
	channel_gui_رجسٽر_one(channel, PART_ERR);
#جيڪڏهن_پورو
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    فراغ
channel_سيٽ_ڪم(channel_T *channel, ڪم_T *ڪم, jobopt_T *options)
{
    channel->ch_ڪم = ڪم;

    channel_سيٽ_options(channel, options);

    إذا (ڪم->jv_اندريون_buf == عدم)
	أرجع;

    chanpart_T *اندريون_part = &channel->ch_part[PART_اندريون];

    سيٽ_bufref(&اندريون_part->ch_bufref, ڪم->jv_اندريون_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (حرف *)اندريون_part->ch_bufref.br_buf->b_ffname);
    إذا (options->jo_سيٽ & JO_اندريون_چوٽي)
    {
	إذا (options->jo_اندريون_چوٽي == 0 && !(options->jo_سيٽ & JO_اندريون_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    اندريون_part->ch_bufref.br_buf->b_write_منزل_channel = true;
	    اندريون_part->ch_buf_append = TRUE;
	    اندريون_part->ch_buf_چوٽي =
		اندريون_part->ch_bufref.br_buf->b_ml.ml_سٽ_ڳڻپ + 1;
	}
	وإلا
	    اندريون_part->ch_buf_چوٽي = options->jo_اندريون_چوٽي;
    }
    وإلا
	اندريون_part->ch_buf_چوٽي = 1;
    إذا (options->jo_سيٽ & JO_اندريون_BOT)
	اندريون_part->ch_buf_bot = options->jo_اندريون_bot;
    وإلا
	اندريون_part->ch_buf_bot = اندريون_part->ch_bufref.br_buf->b_ml.ml_سٽ_ڳڻپ;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ساكن فراغ
channel_سيٽ_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	صحيح	    id)
{
    cbq_T *سر = &channel->ch_part[part].ch_cb_سر;
    cbq_T *شي = ALLOC_ONE(cbq_T);

    إذا (شي == عدم)
	أرجع;

    copy_callback(&شي->cq_callback, callback);
    شي->cq_seq_nr = id;
    شي->cq_prev = سر->cq_prev;
    سر->cq_prev = شي;
    شي->cq_ايندڙ = عدم;
    إذا (شي->cq_prev == عدم)
	سر->cq_ايندڙ = شي;
    وإلا
	شي->cq_prev->cq_ايندڙ = شي;
}

    ساكن فراغ
write_buf_سٽ(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *سٽ = ml_get_buf(buf, lnum, FALSE);
    صحيح	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    صحيح	    i;

    // Need to make a copy to be able to append a NL.
    إذا ((p = alloc(len + 2)) == عدم)
	أرجع;
    يادگيري_نقل((حرف *)p, (حرف *)سٽ, len);

    إذا (channel->ch_write_متن_گهڻائي)
	p[len] = CAR;
    وإلا
    {
	لكل (i = 0; i < len; ++i)
	    إذا (p[i] == NL)
		p[i] = NUL;

	p[len] = NL;
    }
    p[len + 1] = NUL;
    channel_send(channel, PART_اندريون, p, len + 1, "write_buf_line");
    vim_free(p);
}

/*
 * Return TRUE if "channel" can be written to.
 * Returns FALSE if the input is closed or the write would block.
 */
    ساكن صحيح
can_write_buf_سٽ(channel_T *channel)
{
    chanpart_T *اندريون_part = &channel->ch_part[PART_اندريون];

    إذا (اندريون_part->ch_fd == INVALID_FD)
	أرجع FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    إذا (اندريون_part->ch_بلاڪ_write == 1)
	اندريون_part->ch_بلاڪ_write = -1;
    وإلا إذا (اندريون_part->ch_بلاڪ_write == -1)
	اندريون_part->ch_بلاڪ_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#جيڪڏهن_وصف_نه_ٿيل MSWIN
    {
# إذا defined(HAVE_SELECT)
	بنية timeval	tval;
	fd_set		wfds;
	صحيح		ret;

	FD_ZERO(&wfds);
	FD_SET((صحيح)اندريون_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	لكل (;;)
	{
	    ret = select((صحيح)اندريون_part->ch_fd + 1, عدم, &wfds, عدم, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    إذا (ret == -1 && errno == EINTR)
		استمر;
#  endif
	    إذا (ret <= 0 || اندريون_part->ch_بلاڪ_write == 1)
	    {
		إذا (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		وإلا
		    ch_log(channel, "Input not ready for writing");
		أرجع FALSE;
	    }
	    اكسر;
	}
# وإلا
	بنية pollfd	fds;

	fds.fd = اندريون_part->ch_fd;
	fds.events = POLLOUT;
	إذا (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    أرجع FALSE;
	}
	إذا (اندريون_part->ch_بلاڪ_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    أرجع FALSE;
	}
# endif
    }
#جيڪڏهن_پورو
    أرجع TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    فراغ
channel_write_اندريون(channel_T *channel)
{
    chanpart_T *اندريون_part = &channel->ch_part[PART_اندريون];
    linenr_T    lnum;
    buf_T	*buf = اندريون_part->ch_bufref.br_buf;
    صحيح		written = 0;

    إذا (buf == عدم || اندريون_part->ch_buf_append)
	أرجع;  // no buffer or using appending
    إذا (!bufref_valid(&اندريون_part->ch_bufref) || buf->b_ml.ml_mfp == عدم)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	اندريون_part->ch_bufref.br_buf = عدم;
	أرجع;
    }

    لكل (lnum = اندريون_part->ch_buf_چوٽي; lnum <= اندريون_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_سٽ_ڳڻپ; ++lnum)
    {
	إذا (!can_write_buf_سٽ(channel))
	    اكسر;
	write_buf_سٽ(buf, lnum, channel);
	++written;
    }

    إذا (written == 1)
	ch_log(channel, "written line %d to channel", (صحيح)lnum - 1);
    وإلا إذا (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    اندريون_part->ch_buf_چوٽي = lnum;
    إذا (lnum > buf->b_ml.ml_سٽ_ڳڻپ || lnum > اندريون_part->ch_buf_bot)
    {
#إذا defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	إذا (channel->ch_ڪم != عدم)
	    رڪن_send_eof(channel);
#جيڪڏهن_پورو

	// Writing is done, no longer need the buffer.
	اندريون_part->ch_bufref.br_buf = عدم;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#جيڪڏهن_وصف_ٿيل MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	إذا (!channel->ch_anonymous_pipe)
#جيڪڏهن_پورو
	    ch_close_part(channel, PART_اندريون);
    }
    وإلا
	ch_log(channel, "Still %ld more lines to write",
				   (ڊگهو)(buf->b_ml.ml_سٽ_ڳڻپ - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    فراغ
channel_بفر_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	لكل (part = PART_SOCK; part < PART_ڳڻپ; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    إذا (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_نالا[part]);
		ch_part->ch_bufref.br_buf = عدم;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    فراغ
channel_write_داخلا(channel_T *channel)
{
    chanpart_T	*اندريون_part = &channel->ch_part[PART_اندريون];

    إذا (اندريون_part->ch_writeque.wq_ايندڙ != عدم)
	channel_send(channel, PART_اندريون, (char_u *)"", 0, "channel_write_input");
    وإلا إذا (اندريون_part->ch_bufref.br_buf != عدم)
    {
	إذا (اندريون_part->ch_buf_append)
	    channel_write_new_سٽون(اندريون_part->ch_bufref.br_buf);
	وإلا
	    channel_write_اندريون(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    فراغ
channel_write_any_سٽون(فراغ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_داخلا(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    فراغ
channel_write_new_سٽون(buf_T *buf)
{
    channel_T	*channel;
    صحيح		مليو_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *اندريون_part = &channel->ch_part[PART_اندريون];
	linenr_T    lnum;
	صحيح	    written = 0;

	إذا (اندريون_part->ch_bufref.br_buf == buf && اندريون_part->ch_buf_append)
	{
	    إذا (اندريون_part->ch_fd == INVALID_FD)
		استمر;  // pipe was closed
	    مليو_one = TRUE;
	    لكل (lnum = اندريون_part->ch_buf_bot; lnum < buf->b_ml.ml_سٽ_ڳڻپ;
								       ++lnum)
	    {
		إذا (!can_write_buf_سٽ(channel))
		    اكسر;
		write_buf_سٽ(buf, lnum, channel);
		++written;
	    }

	    إذا (written == 1)
		ch_log(channel, "written line %d to channel", (صحيح)lnum - 1);
	    وإلا إذا (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    إذا (lnum < buf->b_ml.ml_سٽ_ڳڻپ)
		ch_log(channel, "Still %ld more lines to write",
				       (ڊگهو)(buf->b_ml.ml_سٽ_ڳڻپ - lnum));

	    اندريون_part->ch_buf_bot = lnum;
	}
    }
    إذا (!مليو_one)
	buf->b_write_منزل_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ساكن فراغ
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    إذا (محفوظ_منزل_invoke_callback == 0)
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
    readq_T *سر = &channel->ch_part[part].ch_سر;

    أرجع سر->rq_ايندڙ;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_پهريون_nl(readq_T *ڳنڍ)
{
    char_u  *بفر = ڳنڍ->rq_بفر;
    long_u  i;

    لكل (i = 0; i < ڳنڍ->rq_buflen; ++i)
	إذا (بفر[i] == NL)
	    أرجع بفر + i;
    أرجع عدم;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, صحيح *outlen)
{
    readq_T *سر = &channel->ch_part[part].ch_سر;
    readq_T *ڳنڍ = سر->rq_ايندڙ;
    char_u *p;

    إذا (ڳنڍ == عدم)
	أرجع عدم;
    إذا (outlen != عدم)
	*outlen += ڳنڍ->rq_buflen;
    // dispose of the node but keep the buffer
    p = ڳنڍ->rq_بفر;
    سر->rq_ايندڙ = ڳنڍ->rq_ايندڙ;
    إذا (ڳنڍ->rq_ايندڙ == عدم)
	سر->rq_prev = عدم;
    وإلا
	ڳنڍ->rq_ايندڙ->rq_prev = عدم;
    vim_free(ڳنڍ);
    أرجع p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ساكن char_u *
channel_get_all(channel_T *channel, ch_part_T part, صحيح *outlen)
{
    readq_T *سر = &channel->ch_part[part].ch_سر;
    readq_T *ڳنڍ;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    لكل (ڳنڍ = سر->rq_ايندڙ; ڳنڍ != عدم; ڳنڍ = ڳنڍ->rq_ايندڙ)
	len += ڳنڍ->rq_buflen;
    res = alloc(len + 1);
    إذا (res == عدم)
	أرجع عدم;
    p = res;
    لكل (ڳنڍ = سر->rq_ايندڙ; ڳنڍ != عدم; ڳنڍ = ڳنڍ->rq_ايندڙ)
    {
	mch_memmove(p, ڳنڍ->rq_بفر, ڳنڍ->rq_buflen);
	p += ڳنڍ->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    افعل
    {
	p = channel_get(channel, part, عدم);
	vim_free(p);
    } طالما (p != عدم);

    إذا (outlen != عدم)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	أرجع res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    طالما (p < res + len)
    {
	إذا (*p == NUL)
	    *p = NL;
#جيڪڏهن_وصف_ٿيل MSWIN
	وإلا إذا (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    إذا (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		طالما (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		إذا (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#جيڪڏهن_پورو
	++p;
    }

    أرجع res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    فراغ
channel_consume(channel_T *channel, ch_part_T part, صحيح len)
{
    readq_T *سر = &channel->ch_part[part].ch_سر;
    readq_T *ڳنڍ = سر->rq_ايندڙ;
    char_u *buf = ڳنڍ->rq_بفر;

    إذا (len < 0 || (long_u)len > ڳنڍ->rq_buflen)
	أرجع;
    mch_memmove(buf, buf + len, ڳنڍ->rq_buflen - len);
    ڳنڍ->rq_buflen -= len;
    ڳنڍ->rq_بفر[ڳنڍ->rq_buflen] = NUL;
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
    ch_گهڻائي_T	گهڻائي = channel->ch_part[part].ch_گهڻائي;
    readq_T	*سر = &channel->ch_part[part].ch_سر;
    readq_T	*ڳنڍ = سر->rq_ايندڙ;
    readq_T	*آخري_ڳنڍ;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    إذا (ڳنڍ == عدم || ڳنڍ->rq_ايندڙ == عدم)
	أرجع FAIL;

    آخري_ڳنڍ = ڳنڍ->rq_ايندڙ;
    len = ڳنڍ->rq_buflen + آخري_ڳنڍ->rq_buflen;
    إذا (want_nl || گهڻائي == CH_گهڻائي_LSP || گهڻائي == CH_گهڻائي_DAP)
	طالما (آخري_ڳنڍ->rq_ايندڙ != عدم
		&& (گهڻائي == CH_گهڻائي_LSP || گهڻائي == CH_گهڻائي_DAP
		    || channel_پهريون_nl(آخري_ڳنڍ) == عدم))
	{
	    آخري_ڳنڍ = آخري_ڳنڍ->rq_ايندڙ;
	    len += آخري_ڳنڍ->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    إذا (newbuf == عدم)
	أرجع FAIL;	    // out of memory
    mch_memmove(p, ڳنڍ->rq_بفر, ڳنڍ->rq_buflen);
    p += ڳنڍ->rq_buflen;
    vim_free(ڳنڍ->rq_بفر);
    ڳنڍ->rq_بفر = newbuf;
    لكل (n = ڳنڍ; n != آخري_ڳنڍ; )
    {
	n = n->rq_ايندڙ;
	mch_memmove(p, n->rq_بفر, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_بفر);
    }
    *p = NUL;
    ڳنڍ->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    لكل (n = ڳنڍ->rq_ايندڙ; n != آخري_ڳنڍ; )
    {
	n = n->rq_ايندڙ;
	vim_free(n->rq_prev);
    }
    ڳنڍ->rq_ايندڙ = آخري_ڳنڍ->rq_ايندڙ;
    إذا (آخري_ڳنڍ->rq_ايندڙ == عدم)
	سر->rq_prev = ڳنڍ;
    وإلا
	آخري_ڳنڍ->rq_ايندڙ->rq_prev = ڳنڍ;
    vim_free(آخري_ڳنڍ);
    أرجع OK;
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
    readq_T *ڳنڍ;
    readq_T *سر = &channel->ch_part[part].ch_سر;
    char_u  *p;
    صحيح	    i;

    ڳنڍ = ALLOC_ONE(readq_T);
    إذا (ڳنڍ == عدم)
	أرجع FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    ڳنڍ->rq_بفر = alloc(len + 1);
    إذا (ڳنڍ->rq_بفر == عدم)
    {
	vim_free(ڳنڍ);
	أرجع FAIL;	    // out of memory
    }

    إذا (channel->ch_part[part].ch_گهڻائي == CH_گهڻائي_NL)
    {
	// Drop any CR before a NL.
	p = ڳنڍ->rq_بفر;
	لكل (i = 0; i < len; ++i)
	    إذا (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	ڳنڍ->rq_buflen = (long_u)(p - ڳنڍ->rq_بفر);
    }
    وإلا
    {
	mch_memmove(ڳنڍ->rq_بفر, buf, len);
	ڳنڍ->rq_بفر[len] = NUL;
	ڳنڍ->rq_buflen = (long_u)len;
    }

    إذا (prepend)
    {
	// prepend node to the head of the queue
	ڳنڍ->rq_ايندڙ = سر->rq_ايندڙ;
	ڳنڍ->rq_prev = عدم;
	إذا (سر->rq_ايندڙ == عدم)
	    سر->rq_prev = ڳنڍ;
	وإلا
	    سر->rq_ايندڙ->rq_prev = ڳنڍ;
	سر->rq_ايندڙ = ڳنڍ;
    }
    وإلا
    {
	// append node to the tail of the queue
	ڳنڍ->rq_ايندڙ = عدم;
	ڳنڍ->rq_prev = سر->rq_prev;
	إذا (سر->rq_prev == عدم)
	    سر->rq_ايندڙ = ڳنڍ;
	وإلا
	    سر->rq_prev->rq_ايندڙ = ڳنڍ;
	سر->rq_prev = ڳنڍ;
    }

    إذا (ch_log_سرگرم() && lead != عدم)
	ch_log_literal(lead, channel, part, buf, len);

    أرجع OK;
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
    char_u	*ايندڙ = channel_get(channel, part, عدم);
    صحيح		keeplen;
    صحيح		addlen;
    char_u	*p;

    إذا (ايندڙ == عدم)
	أرجع FALSE;

    keeplen = reader->js_پڇاڙي - reader->js_buf;
    إذا (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (صحيح)STRLEN(ايندڙ);
	p = alloc(keeplen + addlen + 1);
	إذا (p == عدم)
	{
	    vim_free(ايندڙ);
	    أرجع FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, ايندڙ, addlen + 1);
	vim_free(ايندڙ);
	ايندڙ = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = ايندڙ;
    أرجع TRUE;
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
    char_u	*سٽ_شروعات;
    char_u	*p;
    int_u	hdr_len;
    صحيح		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_پڇاڙي = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    طالما (TRUE)
    {
	سٽ_شروعات = p;
	طالما (*p != NUL && *p != '\n')
	    p++;
	إذا (*p == NUL)			// partial header
	    أرجع MAYBE;
	p++;

	// process the content length field (if present)
	إذا ((p - سٽ_شروعات > 16)
		&& STRNICMP(سٽ_شروعات, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((حرف *)سٽ_شروعات + 16, عدم, 10);
	    إذا (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		أرجع FAIL;
	}

	إذا ((p - سٽ_شروعات) == 2 && سٽ_شروعات[0] == '\r' &&
		سٽ_شروعات[1] == '\n')
	    // reached the empty line
	    اكسر;
    }

    إذا (payload_len == -1)
	// Content-Length field is not present in the header
	أرجع FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    إذا (jsbuf_len < hdr_len + payload_len)
	أرجع MAYBE;

    reader->js_استعمال_ٿيل += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_پڇاڙي = reader->js_buf + hdr_len + payload_len;

    أرجع OK;
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
    jsonq_T	*شي;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*سر = &chanpart->ch_json_سر;
    صحيح		حالت = OK;
    صحيح		ret;

    إذا (channel_peek(channel, part) == عدم)
	أرجع FALSE;

    reader.js_buf = channel_get(channel, part, عدم);
    reader.js_استعمال_ٿيل = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    إذا (!socketserver && (chanpart->ch_گهڻائي == CH_گهڻائي_LSP
	    || chanpart->ch_گهڻائي == CH_گهڻائي_DAP))
	حالت = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    إذا (حالت == OK)
    {
	++emsg_silent;
	حالت = json_decode(&reader, &listtv,
				chanpart->ch_گهڻائي == CH_گهڻائي_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    إذا (حالت == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	إذا (!socketserver && (chanpart->ch_گهڻائي == CH_گهڻائي_LSP
		    || chanpart->ch_گهڻائي == CH_گهڻائي_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	وإلا إذا (!socketserver && chanpart->ch_گهڻائي != CH_گهڻائي_LSP
		&& chanpart->ch_گهڻائي != CH_گهڻائي_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    إذا (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    وإلا
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	وإلا
	{
	    شي = ALLOC_ONE(jsonq_T);
	    إذا (شي == عدم)
		clear_tv(&listtv);
	    وإلا
	    {
		شي->jq_no_callback = FALSE;
		شي->jq_قدر = alloc_tv();
		إذا (شي->jq_قدر == عدم)
		{
		    vim_free(شي);
		    clear_tv(&listtv);
		}
		وإلا
		{
		    *شي->jq_قدر = listtv;
		    شي->jq_prev = سر->jq_prev;
		    سر->jq_prev = شي;
		    شي->jq_ايندڙ = عدم;
		    إذا (شي->jq_prev == عدم)
			سر->jq_ايندڙ = شي;
		    وإلا
			شي->jq_prev->jq_ايندڙ = شي;
		}
	    }
	}
    }

    إذا (حالت == OK)
	chanpart->ch_wait_len = 0;
    وإلا إذا (حالت == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	إذا (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (صحيح)buflen);
	    reader.js_استعمال_ٿيل = 0;
	    chanpart->ch_wait_len = buflen;
#جيڪڏهن_وصف_ٿيل MSWIN
	    chanpart->ch_deadline = Get_Tick_ڳڻپ() + 100L;
#وإلا
	    gettimeofday(&chanpart->ch_deadline, عدم);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    إذا (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#جيڪڏهن_پورو
	}
	وإلا
	{
	    صحيح timeout;
#جيڪڏهن_وصف_ٿيل MSWIN
	    timeout = (صحيح)(Get_Tick_ڳڻپ() - chanpart->ch_deadline) > 0;
#وإلا
	    {
		بنية timeval now_tv;

		gettimeofday(&now_tv, عدم);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#جيڪڏهن_پورو
	    إذا (timeout)
	    {
		حالت = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    وإلا
	    {
		reader.js_استعمال_ٿيل = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    إذا (حالت == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    وإلا إذا (reader.js_buf[reader.js_استعمال_ٿيل] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_استعمال_ٿيل,
			(صحيح)(reader.js_پڇاڙي - reader.js_buf) - reader.js_استعمال_ٿيل,
								  TRUE, عدم);
	ret = حالت == MAYBE ? FALSE: TRUE;
    }
    وإلا
	ret = FALSE;

    vim_free(reader.js_buf);
    أرجع ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ساكن فراغ
remove_cb_ڳنڍ(cbq_T *سر, cbq_T *ڳنڍ)
{
    إذا (ڳنڍ->cq_prev == عدم)
	سر->cq_ايندڙ = ڳنڍ->cq_ايندڙ;
    وإلا
	ڳنڍ->cq_prev->cq_ايندڙ = ڳنڍ->cq_ايندڙ;
    إذا (ڳنڍ->cq_ايندڙ == عدم)
	سر->cq_prev = ڳنڍ->cq_prev;
    وإلا
	ڳنڍ->cq_ايندڙ->cq_prev = ڳنڍ->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    فراغ
remove_json_ڳنڍ(jsonq_T *سر, jsonq_T *ڳنڍ)
{
    إذا (ڳنڍ->jq_prev == عدم)
	سر->jq_ايندڙ = ڳنڍ->jq_ايندڙ;
    وإلا
	ڳنڍ->jq_prev->jq_ايندڙ = ڳنڍ->jq_ايندڙ;
    إذا (ڳنڍ->jq_ايندڙ == عدم)
	سر->jq_prev = ڳنڍ->jq_prev;
    وإلا
	ڳنڍ->jq_ايندڙ->jq_prev = ڳنڍ->jq_prev;
    vim_free(ڳنڍ);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ساكن فراغ
channel_جوڙيو_بلاڪ_id(chanpart_T *chanpart, صحيح id)
{
    garray_T *gap = &chanpart->ch_بلاڪ_ids;

    إذا (gap->ga_growsize == 0)
	ga_init2(gap, حجم(صحيح), 10);
    إذا (ga_grow(gap, 1) == OK)
    {
	((صحيح *)gap->ga_معلومات)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ساكن فراغ
channel_remove_بلاڪ_id(chanpart_T *chanpart, صحيح id)
{
    garray_T	*gap = &chanpart->ch_بلاڪ_ids;
    صحيح		i;

    لكل (i = 0; i < gap->ga_len; ++i)
	إذا (((صحيح *)gap->ga_معلومات)[i] == id)
	{
	    --gap->ga_len;
	    إذا (i < gap->ga_len)
	    {
		صحيح *p = ((صحيح *)gap->ga_معلومات) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * حجم(صحيح));
	    }
	    أرجع;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ساكن صحيح
channel_has_بلاڪ_id(chanpart_T *chanpart, صحيح id)
{
    garray_T	*gap = &chanpart->ch_بلاڪ_ids;
    صحيح		i;

    لكل (i = 0; i < gap->ga_len; ++i)
	إذا (((صحيح *)gap->ga_معلومات)[i] == id)
	    أرجع TRUE;
    أرجع FALSE;
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
    jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;
    jsonq_T   *شي = سر->jq_ايندڙ;

    طالما (شي != عدم)
    {
	list_T	    *l;
	typval_T    *tv;

	إذا (channel->ch_part[part].ch_گهڻائي != CH_گهڻائي_LSP
		&& channel->ch_part[part].ch_گهڻائي != CH_گهڻائي_DAP)
	{
	    l = شي->jq_قدر->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_پهريون->li_tv;
	}
	وإلا
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    إذا (channel->ch_part[part].ch_گهڻائي == CH_گهڻائي_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		إذا (id > 0)
		{
		    إذا (شي->jq_قدر->v_type != VAR_DICT)
			اذهب nextitem;
		    d = شي->jq_قدر->vval.v_dict;
		    إذا (d == عدم)
			اذهب nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    إذا (dict_has_ڪنجي(d, "method"))
			اذهب nextitem;
		    di = dict_ڳوليو(d, (char_u *)"id", -1);
		    إذا (di == عدم)
			اذهب nextitem;
		    tv = &di->di_tv;
		}
		وإلا
		    tv = شي->jq_قدر;
	    }
	    وإلا
	    {
		إذا (id > 0)
		{
		    إذا (شي->jq_قدر->v_type != VAR_DICT)
			اذهب nextitem;
		    d = شي->jq_قدر->vval.v_dict;
		    إذا (d == عدم)
			اذهب nextitem;
		    di = dict_ڳوليو(d, (char_u *)"request_seq", -1);
		    إذا (di == عدم)
			اذهب nextitem;
		    tv = &di->di_tv;
		}
		وإلا
		    tv = شي->jq_قدر;
	    }
	}

	إذا ((without_callback || !شي->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_عدد && tv->vval.v_عدد == id)
	      || (id <= 0 && (tv->v_type != VAR_عدد
		 || tv->vval.v_عدد == 0
		 || !channel_has_بلاڪ_id(
				&channel->ch_part[part], tv->vval.v_عدد)))))
	{
	    *rettv = شي->jq_قدر;
	    إذا (tv->v_type == VAR_عدد)
		ch_log(channel, "Getting JSON message %ld",
						      (ڊگهو)tv->vval.v_عدد);
	    remove_json_ڳنڍ(سر, شي);
	    أرجع OK;
	}
nextitem:
	شي = شي->jq_ايندڙ;
    }
    أرجع FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ساكن فراغ
channel_ڌڪو_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;
    jsonq_T   *شي = سر->jq_ايندڙ;
    jsonq_T   *newitem;

    إذا (سر->jq_prev != عدم && سر->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	شي = عدم;
    وإلا طالما (شي != عدم && شي->jq_no_callback)
	// append after the last item that was pushed back
	شي = شي->jq_ايندڙ;

    newitem = ALLOC_ONE(jsonq_T);
    إذا (newitem == عدم)
    {
	clear_tv(rettv);
	أرجع;
    }

    newitem->jq_قدر = alloc_tv();
    إذا (newitem->jq_قدر == عدم)
    {
	vim_free(newitem);
	clear_tv(rettv);
	أرجع;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_قدر = *rettv;
    إذا (شي == عدم)
    {
	// append to the end
	newitem->jq_prev = سر->jq_prev;
	سر->jq_prev = newitem;
	newitem->jq_ايندڙ = عدم;
	إذا (newitem->jq_prev == عدم)
	    سر->jq_ايندڙ = newitem;
	وإلا
	    newitem->jq_prev->jq_ايندڙ = newitem;
    }
    وإلا
    {
	// append after "item"
	newitem->jq_prev = شي;
	newitem->jq_ايندڙ = شي->jq_ايندڙ;
	شي->jq_ايندڙ = newitem;
	إذا (newitem->jq_ايندڙ == عدم)
	    سر->jq_prev = newitem;
	وإلا
	    newitem->jq_ايندڙ->jq_prev = newitem;
    }
}

#وصف_ڪريو CH_JSON_MAX_دليلون 4

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
    صحيح	    options = channel->ch_part[part].ch_گهڻائي == CH_گهڻائي_JS
								 ? JSON_JS : 0;

    إذا (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	إذا (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_دليل));
	أرجع;
    }
    arg = argv[1].vval.v_string;
    إذا (arg == عدم)
	arg = (char_u *)"";

    إذا (STRCMP(cmd, "ex") == 0)
    {
	صحيح	called_emsg_before = called_emsg;
	char_u	*p = arg;
	صحيح	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (حرف *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	إذا (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	إذا (do_emsg_silent)
	    --emsg_silent;
	إذا (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (حرف *)get_vim_var_str(VV_ERRMSG));
    }
    وإلا إذا (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (حرف *)arg);
	CLEAR_ميدان(ea);
	ea.arg = arg;
	ea.addr_ڳڻپ = 0;
	ea.forceit = TRUE; // no mapping
	ex_عام(&ea);
    }
    وإلا إذا (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	ٻاهريون_flush_cursor(TRUE, FALSE);
    }
    وإلا إذا (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	صحيح is_call = cmd[0] == 'c';
	صحيح id_idx = is_call ? 3 : 2;

	إذا (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_عدد)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    إذا (p_verbose > 2)
		emsg(_(e_آخري_دليل_for_اظهار_call_must_be_عدد));
	}
	وإلا إذا (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    إذا (p_verbose > 2)
		emsg(_(e_ٽيون_دليل_for_call_must_be_list));
	}
	وإلا
	{
	    typval_T	*tv = عدم;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = عدم;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    إذا (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (حرف *)arg);
		tv = eval_اظهار(arg, عدم);
	    }
	    وإلا
	    {
		ch_log(channel, "Calling '%s'", (حرف *)arg);
		إذا (func_call(arg, &argv[2], عدم, عدم, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    إذا (argv[id_idx].v_type == VAR_عدد)
	    {
		صحيح id = argv[id_idx].vval.v_عدد;

		إذا (tv != عدم)
		    json = json_encode_nr_اظهار(id, tv, options | JSON_NL);
		إذا (tv == عدم || (json != عدم && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_اظهار(id, &err_tv, options | JSON_NL);
		}
		إذا (json != عدم)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_اندريون,
				 json, (صحيح)STRLEN(json), (حرف *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    إذا (tv == &res_tv)
		clear_tv(tv);
	    وإلا
		free_tv(tv);
	}
    }
    وإلا إذا (p_verbose > 2)
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
invoke_one_وقت_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *شي,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (حرف *)شي->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_ڳنڍ(cbhead, شي);
    invoke_callback(channel, &شي->cq_callback, argv);
    free_callback(&شي->cq_callback);
    vim_free(شي);
}

    ساكن فراغ
append_منزل_بفر(
    buf_T	*بفر,
    char_u	*نياپو,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = بفر->b_ml.ml_سٽ_ڳڻپ;
    bool	save_write_منزل = بفر->b_write_منزل_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    صحيح		save_p_ma = بفر->b_p_ma;
    صحيح		خالي_جايون_2 = (بفر->b_ml.ml_flags & ML_خالي_جايون) ? 1 : 0;

    إذا (!بفر->b_p_ma && !ch_part->ch_nomodifiable)
    {
	إذا (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	أرجع;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    إذا (save_write_منزل)
    {
	--lnum;
	بفر->b_write_منزل_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (صحيح)lnum + 1 - خالي_جايون_2, بفر->b_fname);

    بفر->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, بفر);
    إذا (curbuf != بفر)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	أرجع;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - خالي_جايون_2, lnum + 1);

    إذا (خالي_جايون_2)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, نياپو, TRUE);
	lnum = 0;
    }
    وإلا
	ml_append(lnum, نياپو, 0, FALSE);
    appended_سٽون_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    إذا (ch_part->ch_nomodifiable)
	بفر->b_p_ma = FALSE;
    وإلا
	بفر->b_p_ma = save_p_ma;

    إذا (بفر->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    إذا (wp->w_بفر == بفر)
	    {
		صحيح move_cursor = save_write_منزل
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.کانو == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		إذا (move_cursor || wp->w_topline > بفر->b_ml.ml_سٽ_ڳڻپ)
		{
		    win_T *save_curwin = curwin;

		    إذا (move_cursor)
			++wp->w_cursor.lnum;
		    curwin = wp;
		    curbuf = curwin->w_بفر;
		    scroll_cursor_bot(0, FALSE);
		    curwin = save_curwin;
		    curbuf = curwin->w_بفر;
		}
	    }
	}
	redraw_buf_and_حالت_later(بفر, UPD_VALID);
	channel_need_redraw = TRUE;
    }

    إذا (save_write_منزل)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	بفر->b_write_منزل_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *اندريون_part = &ch->ch_part[PART_اندريون];

	    إذا (اندريون_part->ch_bufref.br_buf == بفر)
		اندريون_part->ch_buf_bot = بفر->b_ml.ml_سٽ_ڳڻپ;
	}
    }
}

    ساكن فراغ
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *نياپو;

    طالما ((نياپو = channel_get(channel, part, عدم)) != عدم)
    {
	ch_log(channel, "Dropping message '%s'", (حرف *)نياپو);
	vim_free(نياپو);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ساكن صحيح
channel_use_json_سر(channel_T *channel, ch_part_T part)
{
    ch_گهڻائي_T	ch_گهڻائي = channel->ch_part[part].ch_گهڻائي;

    أرجع ch_گهڻائي == CH_گهڻائي_JSON || ch_گهڻائي == CH_گهڻائي_JS
						     || ch_گهڻائي == CH_گهڻائي_LSP
						     || ch_گهڻائي == CH_گهڻائي_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ساكن صحيح
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*نياپو = عدم;
    blob_T	*blob = عدم;
    typval_T	*listtv = عدم;
    typval_T	argv[CH_JSON_MAX_دليلون];
    صحيح		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_گهڻائي_T	ch_گهڻائي = ch_part->ch_گهڻائي;
    cbq_T	*cbhead = &ch_part->ch_cb_سر;
    cbq_T	*cbitem;
    callback_T	*callback = عدم;
    buf_T	*بفر = عدم;
    char_u	*p;
    صحيح		called_otc;		// one time callbackup
    صحيح		raw_len = 0;

    إذا (channel->ch_nb_close_cb != عدم
#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#جيڪڏهن_پورو
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	أرجع FALSE;

    // Use a message-specific callback, part callback or channel callback
    لكل (cbitem = cbhead->cq_ايندڙ; cbitem != عدم; cbitem = cbitem->cq_ايندڙ)
	إذا (cbitem->cq_seq_nr == 0)
	    اكسر;
    إذا (cbitem != عدم)
	callback = &cbitem->cq_callback;
    وإلا إذا (ch_part->ch_callback.cb_name != عدم)
	callback = &ch_part->ch_callback;
    وإلا إذا (channel->ch_callback.cb_name != عدم)
	callback = &channel->ch_callback;

    بفر = ch_part->ch_bufref.br_buf;
    إذا (بفر != عدم && (!bufref_valid(&ch_part->ch_bufref)
					       || بفر->b_ml.ml_mfp == عدم))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_نالا[part]);
	ch_part->ch_bufref.br_buf = عدم;
	بفر = عدم;
    }

    إذا (channel_use_json_سر(channel, part))
    {
	listitem_T	*شي;
	صحيح		argc = 0;

	// Get any json message in the queue.
	إذا (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    إذا (ch_گهڻائي == CH_گهڻائي_LSP || ch_گهڻائي == CH_گهڻائي_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(فراغ)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    إذا (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		أرجع FALSE;
	}

	إذا (ch_گهڻائي == CH_گهڻائي_LSP || ch_گهڻائي == CH_گهڻائي_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    إذا (d != عدم)
	    {
		إذا (ch_گهڻائي == CH_گهڻائي_LSP)
		    di = dict_ڳوليو(d, (char_u *)"id", -1);
		وإلا
		    di = dict_ڳوليو(d, (char_u *)"request_seq", -1);
		إذا (di != عدم && di->di_tv.v_type == VAR_عدد)
		    seq_nr = di->di_tv.vval.v_عدد;
	    }

	    argv[1] = *listtv;
	}
	وإلا
	{
	    لكل (شي = listtv->vval.v_list->lv_پهريون;
		    شي != عدم && argc < CH_JSON_MAX_دليلون;
		    شي = شي->li_ايندڙ)
		argv[argc++] = شي->li_tv;
	    طالما (argc < CH_JSON_MAX_دليلون)
		argv[argc++].v_type = VAR_UNKNOWN;

	    إذا (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		أرجع TRUE;
	    }

	    إذا (argv[0].v_type != VAR_عدد)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		أرجع FALSE;
	    }
	    seq_nr = argv[0].vval.v_عدد;
	}
    }
    وإلا إذا (channel_peek(channel, part) == عدم)
    {
	// nothing to read on RAW or NL channel
	أرجع FALSE;
    }
    وإلا
    {
	// If there is no callback or buffer drop the message.
	إذا (callback == عدم && بفر == عدم)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    إذا (channel->ch_close_cb.cb_name == عدم && !channel->ch_drop_never)
		drop_messages(channel, part);
	    أرجع FALSE;
	}

	إذا (ch_گهڻائي == CH_گهڻائي_NL)
	{
	    char_u  *nl = عدم;
	    char_u  *buf;
	    readq_T *ڳنڍ;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    طالما (TRUE)
	    {
		ڳنڍ = channel_peek(channel, part);
		nl = channel_پهريون_nl(ڳنڍ);
		إذا (nl != عدم)
		    اكسر;
		إذا (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    إذا (ch_part->ch_fd == INVALID_FD && ڳنڍ->rq_buflen > 0)
			اكسر;
		    أرجع FALSE; // incomplete message
		}
	    }
	    buf = ڳنڍ->rq_بفر;

	    // Convert NUL to NL, the internal representation.
	    لكل (p = buf; (nl == عدم || p < nl)
					    && p < buf + ڳنڍ->rq_buflen; ++p)
		إذا (*p == NUL)
		    *p = NL;

	    إذا (nl == عدم)
	    {
		// get the whole buffer, drop the NL
		نياپو = channel_get(channel, part, عدم);
	    }
	    وإلا إذا (nl + 1 == buf + ڳنڍ->rq_buflen)
	    {
		// get the whole buffer
		نياپو = channel_get(channel, part, عدم);
		*nl = NUL;
	    }
	    وإلا
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		نياپو = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (صحيح)(nl - buf) + 1);
	    }
	}
	وإلا
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    نياپو = channel_get_all(channel, part,
			ch_گهڻائي == CH_گهڻائي_BLOB ? &raw_len : عدم);
	}

	إذا (نياپو == عدم)
	    أرجع FALSE; // out of memory (and avoids Coverity warning)

	إذا (ch_گهڻائي == CH_گهڻائي_BLOB)
	{
	    blob = blob_alloc();
	    إذا (blob == عدم)
	    {
		vim_free(نياپو);
		أرجع FALSE;
	    }
	    إذا (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(نياپو);
		أرجع FALSE;
	    }
	    إذا (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_معلومات, نياپو, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	وإلا
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = نياپو;
	}
    }

    called_otc = FALSE;
    إذا (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	صحيح lsp_req_نياپو = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	إذا ((ch_گهڻائي == CH_گهڻائي_LSP || ch_گهڻائي == CH_گهڻائي_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_ڪنجي(argv[1].vval.v_dict, "method"))
	    lsp_req_نياپو = TRUE;

	إذا (!lsp_req_نياپو)
	{
	    لكل (cbitem = cbhead->cq_ايندڙ; cbitem != عدم;
		    cbitem = cbitem->cq_ايندڙ)
	    {
		إذا (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_وقت_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    اكسر;
		}
	    }
	}
    }

    إذا (seq_nr > 0 && ((ch_گهڻائي != CH_گهڻائي_LSP && ch_گهڻائي != CH_گهڻائي_DAP)
		|| called_otc))
    {
	إذا (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    إذا (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ڌڪو_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_عدد;
		free_tv(listtv);
		listtv = عدم;
	    }
	    وإلا
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    وإلا إذا (callback != عدم || بفر != عدم)
    {
	إذا (بفر != عدم)
	{
	    إذا (نياپو == عدم)
		// JSON or JS mode: re-encode the message.
		نياپو = json_encode(listtv, ch_گهڻائي);
	    إذا (نياپو != عدم)
	    {
#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
		إذا (بفر->b_رڪن != عدم)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    إذا (part == PART_ERR)
		    {
			char_u *cp;
			صحيح    extra = 0;

			لكل (cp = نياپو; *cp != NUL; ++cp)
			    إذا (*cp == NL && (cp == نياپو || cp[-1] != CAR))
				++extra;
			إذا (extra > 0)
			{
			    char_u *crlf_نياپو = alloc(STRLEN(نياپو) + extra + 1);

			    إذا (crlf_نياپو != عدم)
			    {
				char_u *q = crlf_نياپو;

				لكل (cp = نياپو; *cp != NUL; ++cp)
				{
				    إذا (*cp == NL && (cp == نياپو || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_منزل_رڪن(بفر, crlf_نياپو, channel);
				vim_free(crlf_نياپو);
			    }
			}
			وإلا
			    write_منزل_رڪن(بفر, نياپو, channel);
		    }
		    وإلا
			write_منزل_رڪن(بفر, نياپو, channel);
		}
		وإلا
#جيڪڏهن_پورو
		    append_منزل_بفر(بفر, نياپو, channel, part);
	    }
	}

	إذا (callback != عدم)
	{
	    إذا (cbitem != عدم)
		invoke_one_وقت_callback(channel, cbhead, cbitem, argv);
	    وإلا
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (حرف *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    وإلا
	ch_log(channel, "Dropping message %d", seq_nr);

    إذا (listtv != عدم)
	free_tv(listtv);
    إذا (blob != عدم)
	blob_unref(blob);
    vim_free(نياپو);

    أرجع TRUE;
}

#إذا defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    صحيح
channel_can_write_منزل(channel_T *channel)
{
    أرجع channel != عدم && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_اندريون_FD != INVALID_FD);
}
#جيڪڏهن_پورو

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    صحيح
channel_is_open(channel_T *channel)
{
    أرجع channel != عدم && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_اندريون_FD != INVALID_FD
			  || channel->CH_ٻاهريون_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ساكن فراغ *
channel_readahead_اشارو(channel_T *channel, ch_part_T part)
{
    إذا (channel_use_json_سر(channel, part))
    {
	jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;

	إذا (سر->jq_ايندڙ == عدم)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	أرجع سر->jq_ايندڙ;
    }
    أرجع channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ساكن صحيح
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    أرجع channel_readahead_اشارو(channel, part) != عدم;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ساكن حرف *
channel_حالت(channel_T *channel, صحيح req_part)
{
    ch_part_T part;
    صحيح has_readahead = FALSE;

    إذا (channel == عدم)
	 أرجع "fail";
    إذا (req_part == PART_ٻاهريون)
    {
	إذا (channel->CH_ٻاهريون_FD != INVALID_FD)
	    أرجع "open";
	إذا (channel_has_readahead(channel, PART_ٻاهريون))
	    has_readahead = TRUE;
    }
    وإلا إذا (req_part == PART_ERR)
    {
	إذا (channel->CH_ERR_FD != INVALID_FD)
	    أرجع "open";
	إذا (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    وإلا
    {
	إذا (channel_is_open(channel))
	    أرجع "open";
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	    إذا (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		اكسر;
	    }
    }

    إذا (has_readahead)
	أرجع "buffered";
    أرجع "closed";
}

    ساكن فراغ
channel_part_info(channel_T *channel, dict_T *dict, حرف *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    حرف	namebuf[20];  // longest is "sock_timeout"
    size_t	پڇ;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    پڇ = STRLEN(namebuf);
    STRCPY(namebuf + پڇ, "_");
    پڇ += STRLEN_LITERAL("_");

    STRCPY(namebuf + پڇ, "status");
    إذا (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_سيٽ(s, "open");
    وإلا إذا (channel_has_readahead(channel, part))
	STR_LITERAL_سيٽ(s, "buffered");
    وإلا
	STR_LITERAL_سيٽ(s, "closed");
    dict_جوڙيو_string_len(dict, namebuf, s.string, (صحيح)s.ڊيگهائي);

    STRCPY(namebuf + پڇ, "mode");
    بدل (chanpart->ch_گهڻائي)
    {
	حالة CH_گهڻائي_NL:
	    STR_LITERAL_سيٽ(s, "NL");
	    اكسر;
	حالة CH_گهڻائي_RAW:
	    STR_LITERAL_سيٽ(s, "RAW");
	    اكسر;
	حالة CH_گهڻائي_BLOB:
	    STR_LITERAL_سيٽ(s, "BLOB");
	    اكسر;
	حالة CH_گهڻائي_JSON:
	    STR_LITERAL_سيٽ(s, "JSON");
	    اكسر;
	حالة CH_گهڻائي_JS:
	    STR_LITERAL_سيٽ(s, "JS");
	    اكسر;
	حالة CH_گهڻائي_LSP:
	    STR_LITERAL_سيٽ(s, "LSP");
	    اكسر;
	حالة CH_گهڻائي_DAP:
	    STR_LITERAL_سيٽ(s, "DAP");
	    اكسر;
	افتراضي:
	    STR_LITERAL_سيٽ(s, "");
	    اكسر;
    }
    dict_جوڙيو_string_len(dict, namebuf, s.string, (صحيح)s.ڊيگهائي);

    STRCPY(namebuf + پڇ, "io");
    إذا (part == PART_SOCK)
	STR_LITERAL_سيٽ(s, "socket");
    وإلا بدل (chanpart->ch_io)
    {
	حالة JIO_NULL:
	    STR_LITERAL_سيٽ(s, "null");
	    اكسر;
	حالة JIO_PIPE:
	    STR_LITERAL_سيٽ(s, "pipe");
	    اكسر;
	حالة JIO_FILE:
	    STR_LITERAL_سيٽ(s, "file");
	    اكسر;
	حالة JIO_بفر:
	    STR_LITERAL_سيٽ(s, "buffer");
	    اكسر;
	حالة JIO_ٻاهريون:
	    STR_LITERAL_سيٽ(s, "out");
	    اكسر;
	افتراضي:
	    STR_LITERAL_سيٽ(s, "");
	    اكسر;
    }
    dict_جوڙيو_string_len(dict, namebuf, s.string, (صحيح)s.ڊيگهائي);

    STRCPY(namebuf + پڇ, "timeout");
    dict_جوڙيو_عدد(dict, namebuf, chanpart->ch_timeout);
}

    ساكن فراغ
channel_info(channel_T *channel, dict_T *dict)
{
    dict_جوڙيو_عدد(dict, "id", channel->ch_id);
    dict_جوڙيو_string(dict, "status", (char_u *)channel_حالت(channel, -1));

    إذا (channel->ch_hostname != عدم)
    {
	إذا (channel->ch_پورٽ)
	    dict_جوڙيو_عدد(dict, "port", channel->ch_پورٽ);
	وإلا
	    // Unix-domain socket.
	    dict_جوڙيو_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    وإلا
    {
	channel_part_info(channel, dict, "out", PART_ٻاهريون);
	channel_part_info(channel, dict, "err", PART_ERR);
	channel_part_info(channel, dict, "in", PART_اندريون);
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

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    channel_gui_unregister(channel);
#جيڪڏهن_پورو

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_اندريون);
    ch_close_part(channel, PART_ٻاهريون);
    ch_close_part(channel, PART_ERR);

    إذا (invoke_close_cb)
    {
	ch_part_T	part;

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	رڪن_channel_closing(channel);
#جيڪڏهن_پورو
	// Invoke callbacks and flush buffers before the close callback.
	إذا (channel->ch_close_cb.cb_name != عدم)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	{
	    إذا (channel->ch_close_cb.cb_name != عدم
			    || channel->ch_part[part].ch_bufref.br_buf != عدم)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		إذا (channel->ch_close_cb.cb_name == عدم)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_نالا[part]);
		طالما (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	إذا (channel->ch_close_cb.cb_name != عدم)
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

	      إذا (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      إذا (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  لكل (part = PART_SOCK; part < PART_اندريون; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = عدم;
#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = عدم;
    channel->ch_ss_close_cb = عدم;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
    رڪن_channel_closed(channel);
#جيڪڏهن_پورو
}

/*
 * Close the "in" part channel "channel".
 */
    ساكن فراغ
channel_close_اندريون(channel_T *channel)
{
    ch_close_part(channel, PART_اندريون);
}

    ساكن فراغ
remove_from_writeque(writeq_T *wq, writeq_T *داخلا)
{
    ga_clear(&داخلا->wq_ga);
    wq->wq_ايندڙ = داخلا->wq_ايندڙ;
    إذا (wq->wq_ايندڙ == عدم)
	wq->wq_prev = عدم;
    وإلا
	wq->wq_ايندڙ->wq_prev = عدم;
    vim_free(داخلا);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ساكن فراغ
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_سر = &ch_part->ch_json_سر;
    cbq_T   *cb_سر = &ch_part->ch_cb_سر;

    طالما (channel_peek(channel, part) != عدم)
	vim_free(channel_get(channel, part, عدم));

    طالما (cb_سر->cq_ايندڙ != عدم)
    {
	cbq_T *ڳنڍ = cb_سر->cq_ايندڙ;

	remove_cb_ڳنڍ(cb_سر, ڳنڍ);
	free_callback(&ڳنڍ->cq_callback);
	vim_free(ڳنڍ);
    }

    طالما (json_سر->jq_ايندڙ != عدم)
    {
	free_tv(json_سر->jq_ايندڙ->jq_قدر);
	remove_json_ڳنڍ(json_سر, json_سر->jq_ايندڙ);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_بلاڪ_ids);

    طالما (ch_part->ch_writeque.wq_ايندڙ != عدم)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_ايندڙ);
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
    channel_clear_one(channel, PART_ٻاهريون);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_اندريون);
    free_callback(&channel->ch_callback);
    free_callback(&channel->ch_close_cb);
}

#إذا defined(EXITFREE)
    فراغ
channel_free_all(فراغ)
{
    channel_T *channel;

    ch_log(عدم, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#جيڪڏهن_پورو


// Sent when the netbeans channel is found closed when reading.
#وصف_ڪريو DETACH_نياپو_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#وصف_ڪريو MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ساكن صحيح
is_channel_write_بچيل(chanpart_T *اندريون_part)
{
    buf_T *buf = اندريون_part->ch_bufref.br_buf;

    إذا (اندريون_part->ch_writeque.wq_ايندڙ != عدم)
	أرجع TRUE;
    إذا (buf == عدم)
	أرجع FALSE;
    أرجع اندريون_part->ch_buf_append
	    ? (اندريون_part->ch_buf_bot < buf->b_ml.ml_سٽ_ڳڻپ)
	    : (اندريون_part->ch_buf_چوٽي <= اندريون_part->ch_buf_bot
			    && اندريون_part->ch_buf_چوٽي <= buf->b_ml.ml_سٽ_ڳڻپ);
}

#إذا defined(HAVE_SELECT)
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
	chanpart_T  *اندريون_part = &ch->ch_part[PART_اندريون];

	إذا (اندريون_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (صحيح)اندريون_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_بچيل(اندريون_part))
	{
	    FD_SET((صحيح)اندريون_part->ch_fd, wfds);
	    إذا ((صحيح)اندريون_part->ch_fd >= maxfd)
		maxfd = (صحيح)اندريون_part->ch_fd + 1;
	}
    }
    أرجع maxfd;
}
#وإلا
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ساكن صحيح
channel_fill_poll_write(صحيح nfd_اندريون, بنية pollfd *fds)
{
    صحيح		nfd = nfd_اندريون;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *اندريون_part = &ch->ch_part[PART_اندريون];

	إذا (اندريون_part->ch_fd != INVALID_FD
		&& is_channel_write_بچيل(اندريون_part))
	{
	    اندريون_part->ch_poll_idx = nfd;
	    fds[nfd].fd = اندريون_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	وإلا
	    اندريون_part->ch_poll_idx = -1;
    }
    أرجع nfd;
}
#جيڪڏهن_پورو

تعريفنوع تعداد {
    CW_READY,
    CW_NOT_READY,
    CW_ERROR
} channel_wait_نتيجو;

/*
 * Check for reading from "fd" with "timeout" msec.
 * Return CW_READY when there is something to read.
 * Return CW_NOT_READY when there is nothing to read.
 * Return CW_ERROR when there is an error.
 */
    ساكن channel_wait_نتيجو
channel_wait(channel_T *channel, sock_T fd, صحيح timeout)
{
    إذا (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#جيڪڏهن_وصف_ٿيل MSWIN
    إذا (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	صحيح	sleep_وقت;
	DWORD	deadline = Get_Tick_ڳڻپ() + timeout;
	صحيح	delay = 1;

	// reading from a pipe, not a socket
	طالما (TRUE)
	{
	    صحيح r = PeekNamedPipe((HANDLE)fd, عدم, 0, عدم, &nread, عدم);

	    إذا (r && nread > 0)
		أرجع CW_READY;

	    إذا (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, عدم);
	    }
	    وإلا إذا (r == 0)
		أرجع CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_سٽون();

	    sleep_وقت = deadline - Get_Tick_ڳڻپ();
	    إذا (sleep_وقت <= 0)
		اكسر;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    إذا (sleep_وقت > delay)
		sleep_وقت = delay;
	    Sleep(sleep_وقت);
	    delay = delay * 2;
	    إذا (delay > 10)
		delay = 10;
	}
    }
    وإلا
#جيڪڏهن_پورو
    {
#إذا defined(HAVE_SELECT)
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
	    إذا (ret == -1 && errno == EINTR)
		استمر;
# endif
	    إذا (ret > 0)
	    {
		إذا (FD_ISSET(fd, &rfds))
		    أرجع CW_READY;
		channel_write_any_سٽون();
		استمر;
	    }
	    اكسر;
	}
#وإلا
	لكل (;;)
	{
	    بنية pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    صحيح		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    إذا (poll(fds, nfd, timeout) > 0)
	    {
		إذا (fds[0].revents & POLLIN)
		    أرجع CW_READY;
		channel_write_any_سٽون();
		استمر;
	    }
	    اكسر;
	}
#جيڪڏهن_پورو
    }
    أرجع CW_NOT_READY;
}

    ساكن فراغ
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, صحيح is_err, حرف *func)
{
    حرف	نياپو[] = "%s(): Read %s from ch_part[%d], closing";

    إذا (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, نياپو, func, "error", part);
    وإلا
	ch_log(channel, نياپو, func, "EOF", part);

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
    إذا (channel->ch_nb_close_cb != عدم)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_نياپو_RAW,
			      (صحيح)STRLEN_LITERAL(DETACH_نياپو_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    إذا (part == PART_ٻاهريون || part == PART_ERR)
    {
	ch_part_T other = part == PART_ٻاهريون ? PART_ERR : PART_ٻاهريون;

	إذا (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#جيڪڏهن_پورو
}

    ساكن فراغ
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    إذا (channel->ch_nb_close_cb != عدم)
	(*channel->ch_nb_close_cb)();
#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER
    إذا (channel->ch_ss_close_cb != عدم)
	channel->ch_ss_close_cb(channel);
#جيڪڏهن_پورو
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
    إذا (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_نالا[part]);
	أرجع;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    إذا (buf == عدم)
    {
	buf = alloc(MAXMSGSIZE);
	إذا (buf == عدم)
	    أرجع;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    لكل (;;)
    {
	إذا (channel_wait(channel, fd, 0) != CW_READY)
	    اكسر;
	إذا (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    بنية sockaddr_storage	client;

	    newchannel = جوڙيو_channel();
	    إذا (newchannel == عدم)
	    {
		ch_error(عدم, "Cannot allocate channel.");
		أرجع;
	    }
	    socklen = حجم(client);
	    newfd = accept(fd, (بنية sockaddr*)&client, &socklen);
	    إذا (newfd < 0)
	    {
		ch_error(عدم, "Cannot accept channel.");
		channel_free(newchannel);
		أرجع;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    channel_gui_رجسٽر_one(newchannel, PART_SOCK);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER
	    إذا (channel->ch_ss_accept_cb != عدم)
	    {
		channel->ch_ss_accept_cb(newchannel);
		أرجع;
	    }
#جيڪڏهن_پورو

	    إذا (client.ss_family == AF_INET)
	    {
#جيڪڏهن_وصف_ٿيل HAVE_INET_NTOP
		حرف addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((بنية sockaddr_اندريون*)&client)->sin_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
			addr,
			ntohs(((بنية sockaddr_اندريون*)&client)->sin_پورٽ));
#وإلا
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
		    inet_ntoa(((بنية sockaddr_اندريون*)&client)->sin_addr),
		    ntohs(((بنية sockaddr_اندريون*)&client)->sin_پورٽ));
#جيڪڏهن_پورو
	    }
#جيڪڏهن_وصف_ٿيل HAVE_INET_NTOP
	    وإلا إذا (client.ss_family == AF_INET6)
	    {
		حرف addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((بنية sockaddr_in6*)&client)->sin6_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "[%s]:%d",
			addr,
			ntohs(((بنية sockaddr_in6*)&client)->sin6_پورٽ));
	    }
#جيڪڏهن_پورو
	    وإلا إذا (client.ss_family == AF_UNIX)
		vim_snprintf((حرف *)namebuf, حجم(namebuf),
							    "unix:anonymous");
	    وإلا
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "unknown");
	    ++محفوظ_منزل_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --محفوظ_منزل_invoke_callback;
	    clear_tv(&argv[1]);
	    أرجع;
	}
	إذا (use_socket)
	    len = sock_read(fd, (حرف *)buf, MAXMSGSIZE);
	وإلا
	    len = fd_read(fd, (حرف *)buf, MAXMSGSIZE);
	إذا (len <= 0)
	    اكسر;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    إذا (readlen <= 0)
    {
	إذا (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#إذا defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    وإلا إذا (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل FEAT_SOCKETSERVER

    فراغ
channel_check(channel_T *channel, ch_part_T part)
{
    channel_read(channel, part, "channel_check");
}

#جيڪڏهن_پورو

/*
 * Read from RAW or NL "channel"/"part".  Blocks until there is something to
 * read or the timeout expires.
 * When "raw" is TRUE don't block waiting on a NL.
 * Does not trigger timers or handle messages.
 * Returns what was read in allocated memory.
 * Returns NULL in case of error or timeout.
 */
    ساكن char_u *
channel_read_بلاڪ(
	channel_T *channel, ch_part_T part, صحيح timeout, صحيح raw, صحيح *outlen)
{
    char_u	*buf;
    char_u	*نياپو;
    ch_گهڻائي_T	گهڻائي = channel->ch_part[part].ch_گهڻائي;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*ڳنڍ;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  گهڻائي == CH_گهڻائي_RAW ? "RAW"
				: گهڻائي == CH_گهڻائي_BLOB ? "BLOB" : "NL", timeout);

    طالما (TRUE)
    {
	ڳنڍ = channel_peek(channel, part);
	إذا (ڳنڍ != عدم)
	{
	    إذا (گهڻائي == CH_گهڻائي_RAW || گهڻائي == CH_گهڻائي_BLOB
		    || (گهڻائي == CH_گهڻائي_NL
					   && channel_پهريون_nl(ڳنڍ) != عدم))
		// got a complete message
		اكسر;
	    إذا (channel_collapse(channel, part, گهڻائي == CH_گهڻائي_NL) == OK)
		استمر;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    إذا (raw || fd == INVALID_FD)
		اكسر;
	}

	// Wait for up to the channel timeout.
	إذا (fd == INVALID_FD)
	    أرجع عدم;
	إذا (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    أرجع عدم;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    إذا (گهڻائي == CH_گهڻائي_RAW || گهڻائي == CH_گهڻائي_BLOB || outlen != عدم)
    {
	نياپو = channel_get_all(channel, part, outlen);
    }
    وإلا
    {
	char_u *p;

	buf = ڳنڍ->rq_بفر;
	nl = channel_پهريون_nl(ڳنڍ);

	// Convert NUL to NL, the internal representation.
	لكل (p = buf; (nl == عدم || p < nl) && p < buf + ڳنڍ->rq_buflen; ++p)
	    إذا (*p == NUL)
		*p = NL;

	إذا (nl == عدم)
	{
	    // must be a closed channel with missing NL
	    نياپو = channel_get(channel, part, عدم);
	}
	وإلا إذا (nl + 1 == buf + ڳنڍ->rq_buflen)
	{
	    // get the whole buffer
	    نياپو = channel_get(channel, part, عدم);
	    *nl = NUL;
	}
	وإلا
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    نياپو = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (صحيح)(nl - buf) + 1);
	}
    }
    إذا (ch_log_سرگرم())
	ch_log(channel, "Returning %d bytes",
		outlen != عدم ? *outlen : (صحيح)STRLEN(نياپو));
    أرجع نياپو;
}

ساكن صحيح channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    صحيح
channel_اندريون_blocking_wait(فراغ)
{
    أرجع channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ساكن فراغ
channel_wait_reading(channel_T *channel اڻ_استعمال_ٿيل, sock_T fd اڻ_استعمال_ٿيل, صحيح timeout)
{
#جيڪڏهن_وصف_ٿيل MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (فراغ)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#وإلا
    channel_wait_any(timeout);
#جيڪڏهن_پورو
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
channel_read_json_بلاڪ(
	channel_T   *channel,
	ch_part_T   part,
	صحيح	    timeout_arg,
	صحيح	    id,
	typval_T    **rettv)
{
    صحيح		وڌيڪ;
    sock_T	fd;
    صحيح		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_گهڻائي_T	گهڻائي = channel->ch_part[part].ch_گهڻائي;
    صحيح		retval = FAIL;
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    elapsed_T	شروعات_tv;

    ELAPSED_INIT(شروعات_tv);
#جيڪڏهن_پورو

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    إذا (id >= 0)
	channel_جوڙيو_بلاڪ_id(chanpart, id);

    لكل (;;)
    {
	إذا (گهڻائي == CH_گهڻائي_LSP || گهڻائي == CH_گهڻائي_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (فراغ)channel_collapse(channel, part, FALSE);

	وڌيڪ = channel_parse_json(channel, part, false);

	// search for message "id"
	إذا (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    اكسر;
	}

	إذا (!وڌيڪ)
	{
	    فراغ *prev_readahead_ptr = channel_readahead_اشارو(channel, part);
	    فراغ *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    إذا (channel_parse_messages())
		استمر;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_اشارو(channel, part);
	    إذا (readahead_ptr != عدم && readahead_ptr != prev_readahead_ptr)
		استمر;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
	    timeout -= (صحيح)ELAPSED_FUNC(شروعات_tv);
#جيڪڏهن_پورو
	    fd = chanpart->ch_fd;
	    إذا (timeout <= 0 || fd == INVALID_FD)
	    {
		إذا (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		اكسر;
	    }
	    // If there was an incomplete message use the deadline for that.
	    إذا (chanpart->ch_wait_len > 0)
	    {
		صحيح	کاٻو;
#جيڪڏهن_وصف_ٿيل MSWIN
		کاٻو = chanpart->ch_deadline - Get_Tick_ڳڻپ() + 1;
#وإلا
		{
		    بنية timeval now_tv;

		    gettimeofday(&now_tv, عدم);
		    کاٻو = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#جيڪڏهن_پورو
		إذا (کاٻو < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		وإلا إذا (کاٻو < timeout)
		    timeout = کاٻو;
	    }
	    channel_wait_reading(channel, fd, timeout);
#جيڪڏهن_وصف_نه_ٿيل ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#جيڪڏهن_پورو
	}
    }
    إذا (id >= 0)
	channel_remove_بلاڪ_id(chanpart, id);
    --channel_blocking_wait;

    أرجع retval;
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

    إذا (tv->v_type == VAR_ڪم)
    {
	إذا (tv->vval.v_ڪم != عدم)
	    channel = tv->vval.v_ڪم->jv_channel;
    }
    وإلا إذا (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    وإلا
    {
	semsg(_(e_invalid_دليل_str), tv_get_string(tv));
	أرجع عدم;
    }
    إذا (channel != عدم && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_ڳڻپ ? part : channel_part_read(channel));

    إذا (check_open && (channel == عدم || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	أرجع عدم;
    }
    أرجع channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ساكن فراغ
common_channel_read(typval_T *argvars, typval_T *rettv, صحيح raw, صحيح blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_ڳڻپ;
    jobopt_T	opt;
    صحيح		گهڻائي;
    صحيح		timeout;
    صحيح		id = -1;
    typval_T	*listtv = عدم;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (اندريون_vim9script()
	    && (check_for_chan_or_ڪم_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    clear_ڪم_options(&opt);
    إذا (get_ڪم_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	اذهب theend;

    إذا (opt.jo_سيٽ & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    إذا (channel == عدم)
	اذهب theend;

    إذا (part == PART_ڳڻپ)
	part = channel_part_read(channel);
    گهڻائي = channel_get_گهڻائي(channel, part);
    timeout = channel_get_timeout(channel, part);
    إذا (opt.jo_سيٽ & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    إذا (blob || گهڻائي == CH_گهڻائي_BLOB)
    {
	صحيح	    outlen = 0;
	char_u  *p = channel_read_بلاڪ(channel, part,
		timeout, TRUE, &outlen);
	إذا (p != عدم)
	{
	    blob_T	*b = blob_alloc();

	    إذا (b != عدم)
	    {
		b->bv_ga.ga_len = outlen;
		إذا (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		وإلا
		{
		    يادگيري_نقل(b->bv_ga.ga_معلومات, p, outlen);
		    rettv_blob_سيٽ(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    وإلا إذا (raw || گهڻائي == CH_گهڻائي_RAW || گهڻائي == CH_گهڻائي_NL)
	rettv->vval.v_string = channel_read_بلاڪ(channel, part,
		timeout, raw, عدم);
    وإلا
    {
	إذا (opt.jo_سيٽ & JO_ID)
	    id = opt.jo_id;
	channel_read_json_بلاڪ(channel, part, timeout, id, &listtv);
	إذا (listtv != عدم)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	وإلا
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_عدد = VVAL_NONE;
	}
    }

theend:
    free_ڪم_options(&opt);
}

#إذا defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
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
	إذا (only_keep_open && !channel->ch_keep_open)
	    استمر;

	// check the socket and pipes
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    إذا (fd == INVALID_FD)
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
			(part == PART_ٻاهريون || part == PART_ERR)) ? 1 : 0);

	    إذا (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    وإلا إذا (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	إذا (channel->ch_ڪم != عدم)
	{
	    ڪم_T *ڪم = channel->ch_ڪم;

	    إذا (ڪم->jv_tty_ٻاهريون != عدم && ڪم->jv_حالت == ڪم_ختم)
		لكل (part = PART_SOCK; part < PART_ڳڻپ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    صحيح
channel_any_keep_open(فراغ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	إذا (channel->ch_keep_open)
	    أرجع TRUE;
    أرجع FALSE;
}
#جيڪڏهن_پورو

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    فراغ
channel_سيٽ_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    صحيح		fd = ch_part->ch_fd;

    إذا (fd == INVALID_FD)
	أرجع;

#جيڪڏهن_وصف_ٿيل MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#وإلا
    (فراغ)fcntl(fd, F_SETFL, O_NONBLOCK);
#جيڪڏهن_پورو
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
    صحيح		did_use_قطار = FALSE;

    fd = ch_part->ch_fd;
    إذا (fd == INVALID_FD)
    {
	إذا (!channel->ch_error && fun != عدم)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	أرجع FAIL;
    }

    إذا (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_سيٽ_nonblock(channel, part);

    إذا (ch_log_سرگرم())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_نياپو = 0;
    }

    لكل (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	صحيح	    len;

	إذا (wq->wq_ايندڙ != عدم)
	{
	    // first write what was queued
	    buf = wq->wq_ايندڙ->wq_ga.ga_معلومات;
	    len = wq->wq_ايندڙ->wq_ga.ga_len;
	    did_use_قطار = TRUE;
	}
	وإلا
	{
	    إذا (len_arg == 0)
		// nothing to write, called from channel_select_check()
		أرجع OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	إذا (part == PART_SOCK)
	    res = sock_write(fd, (حرف *)buf, len);
	وإلا
	{
	    res = fd_write(fd, (حرف *)buf, len);
#جيڪڏهن_وصف_ٿيل MSWIN
	    إذا (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, عدم);
	    }
#جيڪڏهن_پورو
	}
	إذا (res < 0 && (errno == EWOULDBLOCK
#جيڪڏهن_وصف_ٿيل EAGAIN
			|| errno == EAGAIN
#جيڪڏهن_پورو
		    ))
	    res = 0; // nothing got written

	إذا (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *داخلا = wq->wq_ايندڙ;

	    إذا (did_use_قطار)
		ch_log(channel, "Sent %d bytes now", res);
	    إذا (res == len)
	    {
		// Wrote all the buf[len] bytes.
		إذا (داخلا != عدم)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, داخلا);
		    استمر;
		}
		إذا (did_use_قطار)
		    ch_log(channel, "Write queue empty");
	    }
	    وإلا
	    {
		// Wrote only buf[res] bytes, can't write more now.
		إذا (داخلا != عدم)
		{
		    إذا (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(داخلا->wq_ga.ga_معلومات,
				    (حرف *)داخلا->wq_ga.ga_معلومات + res,
				    len - res);
			داخلا->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		وإلا
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		إذا (wq->wq_prev != عدم
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *آخري = wq->wq_prev;

		    // append to the last entry
		    إذا (len > 0 && ga_grow(&آخري->wq_ga, len) == OK)
		    {
			mch_memmove((حرف *)آخري->wq_ga.ga_معلومات
							  + آخري->wq_ga.ga_len,
				    buf, len);
			آخري->wq_ga.ga_len += len;
		    }
		}
		وإلا
		{
		    writeq_T *آخري = ALLOC_ONE(writeq_T);

		    إذا (آخري != عدم)
		    {
			آخري->wq_prev = wq->wq_prev;
			آخري->wq_ايندڙ = عدم;
			إذا (wq->wq_prev == عدم)
			    wq->wq_ايندڙ = آخري;
			وإلا
			    wq->wq_prev->wq_ايندڙ = آخري;
			wq->wq_prev = آخري;
			ga_init2(&آخري->wq_ga, 1, 1000);
			إذا (len > 0 && ga_grow(&آخري->wq_ga, len) == OK)
			{
			    mch_memmove(آخري->wq_ga.ga_معلومات, buf, len);
			    آخري->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	وإلا إذا (res != len)
	{
	    إذا (!channel->ch_error && fun != عدم)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    أرجع FAIL;
	}

	channel->ch_error = FALSE;
	أرجع OK;
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
	char_u	    *متن,
	صحيح	    len,
	صحيح	    id,
	صحيح	    eval,
	jobopt_T    *opt,
	حرف	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_ڪم_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    إذا (channel == عدم)
	أرجع عدم;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    إذا (get_ڪم_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	أرجع عدم;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    إذا (opt->jo_callback.cb_name != عدم && *opt->jo_callback.cb_name != NUL)
    {
	إذا (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    أرجع عدم;
	}
	channel_سيٽ_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    إذا (channel_send(channel, part_send, متن, len, fun) == OK
					   && opt->jo_callback.cb_name == عدم)
	أرجع channel;
    أرجع عدم;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ساكن فراغ
ch_اظهار_common(typval_T *argvars, typval_T *rettv, صحيح eval)
{
    char_u	*متن;
    typval_T	*listtv;
    channel_T	*channel;
    صحيح		id;
    ch_گهڻائي_T	ch_گهڻائي;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحيح		timeout;
    صحيح		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (اندريون_vim9script()
	    && (check_for_chan_or_ڪم_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    إذا (channel == عدم)
	أرجع;
    part_send = channel_part_send(channel);

    ch_گهڻائي = channel_get_گهڻائي(channel, part_send);
    إذا (ch_گهڻائي == CH_گهڻائي_RAW || ch_گهڻائي == CH_گهڻائي_BLOB
	    || ch_گهڻائي == CH_گهڻائي_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	أرجع;
    }

    إذا (ch_گهڻائي == CH_گهڻائي_LSP || ch_گهڻائي == CH_گهڻائي_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	حرف		*ڪنجي = ch_گهڻائي == CH_گهڻائي_LSP ? "id" : "seq";

	// return an empty dict by default
	إذا (rettv_dict_alloc(rettv) == FAIL)
	    أرجع;

	إذا (check_for_dict_arg(argvars, 1) == FAIL)
	    أرجع;

	d = argvars[1].vval.v_dict;
	di = dict_ڳوليو(d, (char_u *)ڪنجي, -1);
	إذا (argvars[2].v_type == VAR_DICT
		&& dict_has_ڪنجي(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	إذا (di != عدم && di->di_tv.v_type != VAR_عدد
		&& (ch_گهڻائي == CH_گهڻائي_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_قدر_for_دليل_str), ڪنجي);
	    أرجع;
	}

	إذا (ch_گهڻائي == CH_گهڻائي_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_آخري_نياپو_id;
	    إذا (di == عدم)
		dict_جوڙيو_عدد(d, "seq", id);
	    وإلا
		di->di_tv.vval.v_عدد = id;
	}
	وإلا إذا (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_آخري_نياپو_id;
	    إذا (di == عدم)
		dict_جوڙيو_عدد(d, "id", id);
	    وإلا
		di->di_tv.vval.v_عدد = id;
	}
	وإلا
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    إذا (di != عدم && di->di_tv.v_type == VAR_عدد)
		id = di->di_tv.vval.v_عدد;
	}
	إذا (ch_گهڻائي == CH_گهڻائي_LSP && !dict_has_ڪنجي(d, "jsonrpc"))
	    dict_جوڙيو_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	متن = json_encode_lsp_نياپو(&argvars[1]);
    }
    وإلا
    {
	id = ++channel->ch_آخري_نياپو_id;
	متن = json_encode_nr_اظهار(id, &argvars[1],
			      (ch_گهڻائي == CH_گهڻائي_JS ? JSON_JS : 0) | JSON_NL);
    }
    إذا (متن == عدم)
	أرجع;

    channel = send_common(argvars, متن, (صحيح)STRLEN(متن), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(متن);
    إذا (channel != عدم && eval)
    {
	إذا (opt.jo_سيٽ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	وإلا
	    timeout = channel_get_timeout(channel, part_read);
	إذا (channel_read_json_بلاڪ(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    إذا (ch_گهڻائي == CH_گهڻائي_LSP || ch_گهڻائي == CH_گهڻائي_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_عدد;
		free_tv(listtv);
	    }
	    وإلا
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_آخري->li_tv;
		list->lv_u.mat.lv_آخري->li_tv.v_type = VAR_عدد;
		free_tv(listtv);
	    }
	}
    }
    free_ڪم_options(&opt);
    إذا (ch_گهڻائي == CH_گهڻائي_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	إذا (rettv->vval.v_dict != عدم)
	    dict_جوڙيو_عدد(rettv->vval.v_dict, "seq", id);
    }
    وإلا إذا (ch_گهڻائي == CH_گهڻائي_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	إذا (rettv->vval.v_dict != عدم)
	    dict_جوڙيو_عدد(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ساكن فراغ
ch_raw_common(typval_T *argvars, typval_T *rettv, صحيح eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*متن;
    صحيح		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحيح		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (اندريون_vim9script()
	    && (check_for_chan_or_ڪم_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    إذا (argvars[1].v_type == VAR_BLOB)
    {
	متن = argvars[1].vval.v_blob->bv_ga.ga_معلومات;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    وإلا
    {
	متن = tv_get_string_buf(&argvars[1], buf);
	len = (صحيح)STRLEN(متن);
    }
    channel = send_common(argvars, متن, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    إذا (channel != عدم && eval)
    {
	إذا (opt.jo_سيٽ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	وإلا
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_بلاڪ(channel, part_read,
							timeout, TRUE, عدم);
    }
    free_ڪم_options(&opt);
}

#وصف_ڪريو KEEP_OPEN_وقت 20  // msec

#إذا defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحيح
channel_poll_تياري(صحيح nfd_اندريون, فراغ *fds_اندريون, صحيح *towait)
{
    صحيح		nfd = nfd_اندريون;
    channel_T	*channel;
    بنية	pollfd *fds = fds_اندريون;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    إذا (ch_part->ch_fd != INVALID_FD)
	    {
		إذا (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    إذا (*towait < 0 || *towait > KEEP_OPEN_وقت)
			*towait = KEEP_OPEN_وقت;
		}
		وإلا
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    وإلا
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    أرجع nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحيح
channel_poll_check(صحيح ret_اندريون, فراغ *fds_اندريون)
{
    صحيح		ret = ret_اندريون;
    channel_T	*channel;
    بنية	pollfd *fds = fds_اندريون;
    ch_part_T	part;
    صحيح		idx;
    chanpart_T	*اندريون_part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    إذا (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    وإلا إذا (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	اندريون_part = &channel->ch_part[PART_اندريون];
	idx = اندريون_part->ch_poll_idx;
	إذا (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_داخلا(channel);
	    --ret;
	}
    }

    أرجع ret;
}
#جيڪڏهن_پورو // UNIX && !HAVE_SELECT

#إذا !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحيح
channel_select_تياري(
	صحيح maxfd_اندريون,
	فراغ *rfds_اندريون,
	فراغ *wfds_اندريون,
	بنية timeval *tv,
	بنية timeval **tvp)
{
    صحيح		maxfd = maxfd_اندريون;
    channel_T	*channel;
    fd_set	*rfds = rfds_اندريون;
    fd_set	*wfds = wfds_اندريون;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    إذا (fd != INVALID_FD)
	    {
		إذا (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    إذا (*tvp == عدم || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_وقت * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_وقت * 1000;
		    }
		}
		وإلا
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    إذا ((صحيح)fd >= FD_SETSIZE)
			استمر;
# endif
		    FD_SET((صحيح)fd, rfds);
		    إذا (maxfd < (صحيح)fd)
			maxfd = (صحيح)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    أرجع maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحيح
channel_select_check(صحيح ret_اندريون, فراغ *rfds_اندريون, فراغ *wfds_اندريون)
{
    صحيح		ret = ret_اندريون;
    channel_T	*channel;
    fd_set	*rfds = rfds_اندريون;
    fd_set	*wfds = wfds_اندريون;
    ch_part_T	part;
    chanpart_T	*اندريون_part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_اندريون; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    إذا (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحيح)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    وإلا إذا (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	اندريون_part = &channel->ch_part[PART_اندريون];
	إذا (ret > 0 && اندريون_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحيح)اندريون_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(اندريون_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(اندريون_part->ch_fd, wfds);
	    channel_write_داخلا(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	إذا (channel->ch_ڪم != عدم)
	{
	    ڪم_T *ڪم = channel->ch_ڪم;

	    إذا (ڪم->jv_tty_ٻاهريون != عدم && ڪم->jv_حالت == ڪم_ختم)
		لكل (part = PART_SOCK; part < PART_ڳڻپ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    أرجع ret;
}
#جيڪڏهن_پورو // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ساكن فراغ
channel_wait_any(ڊگهو msec)
{
#جيڪڏهن_وصف_ٿيل MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    إذا (stdio_channel->CH_ٻاهريون_FD != INVALID_FD)
	(فراغ)channel_wait(stdio_channel, stdio_channel->CH_ٻاهريون_FD,
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
    maxfd = channel_select_تياري(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, عدم, tvp);
    إذا (ret >= 0)
	(فراغ)channel_select_check(ret, &rfds, &wfds);
#وإلا
    بنية pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    صحيح			nfd;
    صحيح			towait = (صحيح)msec;
    صحيح			ret;

    nfd = channel_poll_تياري(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    إذا (ret >= 0)
	(فراغ)channel_poll_check(ret, fds);
#جيڪڏهن_پورو
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    فراغ
channel_stdio_loop(فراغ)
{
    طالما (stdio_channel != عدم && channel_is_open(stdio_channel))
    {
	ڊگهو	msec = 1000;
#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
	ڊگهو	due_وقت = check_due_timer();

	إذا (due_وقت > 0 && due_وقت < msec)
	    msec = due_وقت;
#جيڪڏهن_پورو
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
    channel_T	*channel = پهريون_channel;
    صحيح		ret = FALSE;
    صحيح		r;
    ch_part_T	part = PART_SOCK;
    ساكن صحيح	recursive = 0;
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    elapsed_T	شروعات_tv;
#جيڪڏهن_پورو

    // The code below may invoke callbacks, which might call us back.
    // In a recursive call channels will not be closed.
    ++recursive;
    ++محفوظ_منزل_invoke_callback;

#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
    ELAPSED_INIT(شروعات_tv);
#جيڪڏهن_پورو

    // Only do this message when another message was given, otherwise we get
    // lots of them.
    إذا ((did_repeated_نياپو & REPEATED_نياپو_LOOKING) == 0)
    {
	ch_log(عدم, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_نياپو = REPEATED_نياپو_LOOKING;
    }
    طالما (channel != عدم)
    {
	إذا (recursive == 1)
	{
	    إذا (channel_can_close(channel))
	    {
		channel->ch_منزل_be_closed = (1U << PART_ڳڻپ);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = پهريون_channel;
		استمر;
	    }
	    إذا (channel->ch_منزل_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		إذا (channel->ch_ڪم != عدم)
		    channel->ch_ڪم->jv_channel = عدم;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = پهريون_channel;
		استمر;
	    }
	    إذا (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = پهريون_channel;
		part = PART_SOCK;
		استمر;
	    }
	}

#جيڪڏهن_وصف_ٿيل FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	إذا (part == PART_ٻاهريون
		&& channel->ch_ڪم != عدم
		&& channel->ch_ڪم->jv_tty_ٻاهريون != عدم
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    استمر;
	}
#جيڪڏهن_پورو
	إذا (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    إذا (r == OK)
		ret = TRUE;
	    إذا (channel_unref(channel) || (r == OK
#جيڪڏهن_وصف_ٿيل ELAPSED_FUNC
			// Limit the time we loop here to 100 msec, otherwise
			// Vim becomes unresponsive when the callback takes
			// more than a bit of time.
			&& ELAPSED_FUNC(شروعات_tv) < 100L
#جيڪڏهن_پورو
			))
	    {
		// channel was freed or something was done, start over
		channel = پهريون_channel;
		part = PART_SOCK;
		استمر;
	    }
	}
	إذا (part < PART_ERR)
	    ++part;
	وإلا
	{
	    channel = channel->ch_ايندڙ;
	    part = PART_SOCK;
	}
    }

    إذا (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --محفوظ_منزل_invoke_callback;
    --recursive;

    أرجع ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    صحيح
channel_any_readahead(فراغ)
{
    channel_T	*channel = پهريون_channel;
    ch_part_T	part = PART_SOCK;

    طالما (channel != عدم)
    {
	إذا (channel_has_readahead(channel, part))
	    أرجع TRUE;
	إذا (part < PART_ERR)
	    ++part;
	وإلا
	{
	    channel = channel->ch_ايندڙ;
	    part = PART_SOCK;
	}
    }
    أرجع FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    صحيح
سيٽ_ref_اندريون_channel(صحيح copyID)
{
    صحيح		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    لكل (channel = پهريون_channel; !abort && channel != عدم;
						   channel = channel->ch_ايندڙ)
	إذا (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || سيٽ_ref_اندريون_شي(&tv, copyID, عدم, عدم, عدم);
	}
    أرجع abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ساكن ch_part_T
channel_part_send(channel_T *channel)
{
    إذا (channel->CH_SOCK_FD == INVALID_FD)
	أرجع PART_اندريون;
    أرجع PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ساكن ch_part_T
channel_part_read(channel_T *channel)
{
    إذا (channel->CH_SOCK_FD == INVALID_FD)
	أرجع PART_ٻاهريون;
    أرجع PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ساكن ch_گهڻائي_T
channel_get_گهڻائي(channel_T *channel, ch_part_T part)
{
    إذا (channel == عدم)
	أرجع CH_گهڻائي_JSON;
    أرجع channel->ch_part[part].ch_گهڻائي;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ساكن صحيح
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    أرجع channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    فراغ
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_عدد = 0;
    إذا (اندريون_vim9script() && check_for_chan_or_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel != عدم)
	rettv->vval.v_عدد = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_ٻاهريون)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    فراغ
f_ch_close(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    channel_T *channel;

    إذا (اندريون_vim9script() && check_for_chan_or_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    إذا (channel != عدم)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    فراغ
f_ch_close_اندريون(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    channel_T *channel;

    إذا (اندريون_vim9script() && check_for_chan_or_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    إذا (channel != عدم)
	channel_close_اندريون(channel);
}

/*
 * "ch_getbufnr()" function
 */
    فراغ
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_عدد = -1;

    إذا (اندريون_vim9script()
	    && (check_for_chan_or_ڪم_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel == عدم)
	أرجع;

    char_u	*what = tv_get_string(&argvars[1]);
    صحيح		part;
    إذا (STRCMP(what, "err") == 0)
	part = PART_ERR;
    وإلا إذا (STRCMP(what, "out") == 0)
	part = PART_ٻاهريون;
    وإلا إذا (STRCMP(what, "in") == 0)
	part = PART_اندريون;
    وإلا
	part = PART_SOCK;
    إذا (channel->ch_part[part].ch_bufref.br_buf != عدم)
	rettv->vval.v_عدد =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    فراغ
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    إذا (اندريون_vim9script() && check_for_chan_or_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel == عدم)
	أرجع;

    rettv->v_type = VAR_ڪم;
    rettv->vval.v_ڪم = channel->ch_ڪم;
    إذا (channel->ch_ڪم != عدم)
	++channel->ch_ڪم->jv_refcount;
}

/*
 * "ch_info()" function
 */
    فراغ
f_ch_info(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    channel_T *channel;

    إذا (اندريون_vim9script() && check_for_chan_or_ڪم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel != عدم && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    فراغ
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    إذا (check_restricted() || check_secure())
	أرجع;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    فراغ
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    إذا (check_restricted() || check_secure())
	أرجع;
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
    ch_اظهار_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    فراغ
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_اظهار_common(argvars, rettv, FALSE);
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
f_ch_setoptions(typval_T *argvars, typval_T *rettv اڻ_استعمال_ٿيل)
{
    channel_T	*channel;
    jobopt_T	opt;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (اندريون_vim9script()
	    && (check_for_chan_or_ڪم_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel == عدم)
	أرجع;
    clear_ڪم_options(&opt);
    إذا (get_ڪم_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_گهڻائي_ALL, 0) == OK)
	channel_سيٽ_options(channel, &opt);
    free_ڪم_options(&opt);
}

/*
 * "ch_status()" function
 */
    فراغ
f_ch_حالت(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    صحيح		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (اندريون_vim9script()
	    && (check_for_chan_or_ڪم_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_ڪم_options(&opt);
	إذا (get_ڪم_options(&argvars[1], &opt, JO_PART, 0) == OK
						     && (opt.jo_سيٽ & JO_PART))
	    part = opt.jo_part;
    }

    rettv->vval.v_string = vim_strsave((char_u *)channel_حالت(channel, part));
}

/*
 * Get a string with information about the channel in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
channel_منزل_string_buf(typval_T *varp, char_u *buf)
{
    channel_T *channel = varp->vval.v_channel;
    حرف      *حالت = channel_حالت(channel, -1);

    إذا (channel == عدم)
	vim_snprintf((حرف *)buf, NUMBUFLEN, "channel %s", حالت);
    وإلا
	vim_snprintf((حرف *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, حالت);
    أرجع buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_ڳوليو(صحيح ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	إذا (ch->ch_id == ch_id)
	    أرجع ch;
    أرجع عدم;
}

#جيڪڏهن_پورو // FEAT_JOB_CHANNEL
