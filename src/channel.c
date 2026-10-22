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

#اگر defined(FEAT_کام_CHANNEL)

// TRUE when netbeans is running with a GUI.
#اگر_تعریف_شدہ FEAT_GUI
# define CH_HAS_GUI (gui.اندر_use || gui.starting)
#اگر_ختم

// Note: when making changes here also adjust configure.ac.
#اگر_تعریف_شدہ MSWIN
// WinSock API is separated from C API, thus we can't use read(), write(),
// errno...
# define SOCK_ERRNO errno = WSAGet_آخری_Error()
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
# define UNIX_راستہ_MAX 108
قسمتعریف ساخت sockaddr_un {
    پتہ_FAMILY sun_family;
    حرف sun_راستہ[UNIX_راستہ_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ورنہ
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
#اگر_ختم

ساکن خالی channel_read(channel_T *channel, ch_part_T part, حرف *func);
ساکن خالی channel_wait_any(طویل msec);
ساکن ch_منوال_T channel_get_منوال(channel_T *channel, ch_part_T part);
ساکن صحیح channel_get_timeout(channel_T *channel, ch_part_T part);
ساکن channel_T *channel_open_stdio(خالی);
ساکن ch_part_T channel_part_send(channel_T *channel);
ساکن ch_part_T channel_part_read(channel_T *channel);

#تعریف_کرو FOR_ALL_CHANNELS(ch) \
    برائے ((ch) = پہلا_channel; (ch) != تہی; (ch) = (ch)->ch_اگلا)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ساکن صحیح محفوظ_مقصد_invoke_callback = 0;

#اگر_تعریف_شدہ MSWIN
    ساکن صحیح
fd_read(sock_T fd, حرف *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    اگر (!ReadFile(h, buf, (DWORD)len, &nread, تہی))
	واپس -1;
    واپس (صحیح)nread;
}

    ساکن صحیح
fd_write(sock_T fd, حرف *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, سائز, مکمل = 0;
    OVERLAPPED	ov;

    جب (todo > 0)
    {
	اگر (todo > MAX_NAMED_PIPE_سائز)
	    سائز = MAX_NAMED_PIPE_سائز;
	ورنہ
	    سائز = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_میدان(ov);
	nwrite = 0;
	اگر (!WriteFile(h, buf + مکمل, سائز, &nwrite, &ov))
	{
	    DWORD err = Get_آخری_Error();

	    اگر (err != ERROR_IO_PENDING)
		واپس -1;
	    اگر (!Get_Overlapped_نتیجہ(h, &ov, &nwrite, FALSE))
		واپس -1;
	    FlushFileBuffers(h);
	}
	ورنہ اگر (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    توڑو;
	todo -= nwrite;
	مکمل += nwrite;
    }
    واپس (صحیح)مکمل;
}

    ساکن خالی
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
# undef PERROR
# define PERROR(پیغام) (خالی)semsg("%s: %s", پیغام, strerror_win32(errno))

    ساکن حرف *
strerror_win32(صحیح eno)
{
    ساکن LPVOID msgbuf = تہی;
    char_u *ptr;

    اگر (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = تہی;
    }
    Format_پیغام(
	FORMAT_پیغام_ALLOCATE_بفر |
	FORMAT_پیغام_FROM_SYSTEM |
	FORMAT_پیغام_IGNORE_INSERTS,
	تہی,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	تہی);
    اگر (msgbuf != تہی)
	// chomp \r or \n
	برائے (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    انتخاب (*ptr)
	    {
		حالت '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    توڑو;
		حالت '\n':
		    اگر (*(ptr + 1) == '\0')
			*ptr = '\0';
		    ورنہ
			*ptr = ' ';
		    توڑو;
	    }
    واپس msgbuf;
}
#اگر_ختم

/*
 * The list of all allocated channels.
 */
ساکن channel_T *پہلا_channel = تہی;
ساکن صحیح اگلا_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
جوڑو_channel(خالی)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    اگر (channel == تہی)
	واپس تہی;

    channel->ch_id = اگلا_ch_id++;
    ch_log(channel, "Created channel");

    برائے (part = PART_SOCK; part < PART_گنتی; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#اگر_تعریف_شدہ FEAT_GUI_X11
	channel->ch_part[part].ch_داخلہ_Handler = (Xt_داخلہ_Id)تہی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_GTK
	channel->ch_part[part].ch_داخلہ_Handler = 0;
#اگر_ختم
	channel->ch_part[part].ch_timeout = 2000;
    }

    اگر (پہلا_channel != تہی)
    {
	پہلا_channel->ch_پچھلا = channel;
	channel->ch_اگلا = پہلا_channel;
    }
    پہلا_channel = channel;

    channel->ch_refcount = 1;
    واپس channel;
}

    صحیح
has_any_channel(خالی)
{
    واپس پہلا_channel != تہی;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    صحیح
channel_still_useful(channel_T *channel)
{
    صحیح has_sock_پیغام;
    صحیح	has_برآمد_پیغام;
    صحیح	has_err_پیغام;

    // If the job was killed the channel is not expected to work anymore.
    اگر (channel->ch_کام_killed && channel->ch_کام == تہی)
	واپس FALSE;

    // If there is a close callback it may still need to be invoked.
    اگر (channel->ch_close_cb.cb_name != تہی)
	واپس TRUE;

    // If reading from or a buffer it's still useful.
    اگر (channel->ch_part[PART_اندر].ch_bufref.br_buf != تہی)
	واپس TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_پیغام = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_سر.rq_اگلا != تہی
		|| channel->ch_part[PART_SOCK].ch_json_سر.jq_اگلا != تہی;
    has_برآمد_پیغام = channel->ch_part[PART_برآمد].ch_fd != INVALID_FD
		  || channel->ch_part[PART_برآمد].ch_سر.rq_اگلا != تہی
		  || channel->ch_part[PART_برآمد].ch_json_سر.jq_اگلا != تہی;
    has_err_پیغام = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_سر.rq_اگلا != تہی
		  || channel->ch_part[PART_ERR].ch_json_سر.jq_اگلا != تہی;
    واپس (channel->ch_callback.cb_name != تہی && (has_sock_پیغام
		|| has_برآمد_پیغام || has_err_پیغام))
	    || ((channel->ch_part[PART_برآمد].ch_callback.cb_name != تہی
		       || channel->ch_part[PART_برآمد].ch_bufref.br_buf != تہی)
		    && has_برآمد_پیغام)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != تہی
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != تہی)
		    && has_err_پیغام);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    صحیح
channel_can_close(channel_T *channel)
{
    واپس channel->ch_مقصد_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    ساکن خالی
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    ساکن خالی
channel_free_channel(channel_T *channel)
{
    اگر (channel->ch_اگلا != تہی)
	channel->ch_اگلا->ch_پچھلا = channel->ch_پچھلا;
    اگر (channel->ch_پچھلا == تہی)
	پہلا_channel = channel->ch_اگلا;
    ورنہ
	channel->ch_پچھلا->ch_اگلا = channel->ch_اگلا;
    vim_free(channel);
}

    ساکن خالی
channel_free(channel_T *channel)
{
    اگر (اندر_free_unref_اشیاء)
	واپس;

    اگر (محفوظ_مقصد_invoke_callback == 0)
	channel->ch_مقصد_be_freed = TRUE;
    ورنہ
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
    ساکن صحیح
channel_may_free(channel_T *channel)
{
    اگر (!channel_still_useful(channel))
    {
	channel_free(channel);
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    صحیح
channel_unref(channel_T *channel)
{
    اگر (channel != تہی && --channel->ch_refcount <= 0)
	واپس channel_may_free(channel);
    واپس FALSE;
}

    صحیح
free_غیر_مستعمل_channels_contents(صحیح copyID, صحیح نقاب)
{
    صحیح		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++محفوظ_مقصد_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	اگر (!channel_still_useful(ch)
				 && (ch->ch_copyID & نقاب) != (copyID & نقاب))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --محفوظ_مقصد_invoke_callback;
    واپس did_free;
}

    خالی
free_غیر_مستعمل_channels(صحیح copyID, صحیح نقاب)
{
    channel_T	*ch;
    channel_T	*ch_اگلا;

    برائے (ch = پہلا_channel; ch != تہی; ch = ch_اگلا)
    {
	ch_اگلا = ch->ch_اگلا;
	اگر (!channel_still_useful(ch)
				 && (ch->ch_copyID & نقاب) != (copyID & نقاب))
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
    ساکن channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    اگر (fd == INVALID_FD)
	واپس تہی;

    FOR_ALL_CHANNELS(channel)
    {
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
	    اگر (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		واپس channel;
	    }
    }
    واپس تہی;
}

    ساکن خالی
channel_read_fd(صحیح fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    اگر (channel == تہی)
	ch_error(تہی, "Channel for fd %d not found", fd);
    ورنہ
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ساکن خالی
پیغام_From_Server_X11(Xt_اشارہ_گر client_ڈیٹا,
		  صحیح *unused1 غیر_مستعمل,
		  Xt_داخلہ_Id *unused2 غیر_مستعمل)
{
    channel_read_fd((صحیح)(طویل)client_ڈیٹا);
}
# endif

# ifdef FEAT_GUI_GTK
#  اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن gboolean
پیغام_From_Server_Gtk3(GIOChannel *unused1 غیر_مستعمل,
		  GIOCondition unused2 غیر_مستعمل,
		  gpointer client_ڈیٹا)
{
    channel_read_fd(GPOINTER_مقصد_INT(client_ڈیٹا));
    واپس TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  ورنہ
    ساکن خالی
پیغام_From_Server_Gtk2(gpointer client_ڈیٹا,
		  gint unused1 غیر_مستعمل,
		  Gdk_داخلہ_Condition unused2 غیر_مستعمل)
{
    channel_read_fd((صحیح)(طویل)client_ڈیٹا);
}
#  endif
# endif

    ساکن خالی
channel_gui_رجسٹر_one(channel_T *channel, ch_part_T part غیر_مستعمل)
{
    اگر (!CH_HAS_GUI)
	واپس;

    // gets stuck in handling events for a not connected channel
    اگر (channel->ch_keep_open)
	واپس;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    اگر (channel->ch_part[part].ch_داخلہ_Handler == (Xt_داخلہ_Id)تہی)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_نام[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_داخلہ_Handler = Xt_App_جوڑو_داخلہ(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(Xt_اشارہ_گر)(Xt_داخلہ_Read_نقاب + Xt_داخلہ_Except_نقاب),
		پیغام_From_Server_X11,
		(Xt_اشارہ_گر)(طویل)channel->ch_part[part].ch_fd);
    }
# ورنہ
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    اگر (channel->ch_part[part].ch_داخلہ_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_نام[part], channel->ch_part[part].ch_fd);
#   اگر GTK_CHECK_نسخہ(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_داخلہ_Handler = g_io_جوڑو_watch(
		chnnl,
		G_IO_اندر|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		پیغام_From_Server_Gtk3,
		GINT_مقصد_اشارہ_گر(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   ورنہ
	channel->ch_part[part].ch_داخلہ_Handler = gdk_داخلہ_جوڑو(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_داخلہ_Condition)
			     ((صحیح)GDK_داخلہ_READ + (صحیح)GDK_داخلہ_EXCEPTION),
		پیغام_From_Server_Gtk2,
		(gpointer)(طویل)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ساکن خالی
channel_gui_رجسٹر(channel_T *channel)
{
    اگر (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_رجسٹر_one(channel, PART_SOCK);
    اگر (channel->CH_برآمد_FD != INVALID_FD
	    && channel->CH_برآمد_FD != channel->CH_SOCK_FD)
	channel_gui_رجسٹر_one(channel, PART_برآمد);
    اگر (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_برآمد_FD)
	channel_gui_رجسٹر_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    خالی
channel_gui_رجسٹر_all(خالی)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_رجسٹر(channel);
}

    ساکن خالی
channel_gui_unregister_one(channel_T *channel غیر_مستعمل, ch_part_T part غیر_مستعمل)
{
# ifdef FEAT_GUI_X11
    اگر (channel->ch_part[part].ch_داخلہ_Handler != (Xt_داخلہ_Id)تہی)
    {
	ch_log(channel, "Unregistering part %s", ch_part_نام[part]);
	Xt_Remove_داخلہ(channel->ch_part[part].ch_داخلہ_Handler);
	channel->ch_part[part].ch_داخلہ_Handler = (Xt_داخلہ_Id)تہی;
    }
# ورنہ
#  ifdef FEAT_GUI_GTK
    اگر (channel->ch_part[part].ch_داخلہ_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_نام[part]);
#   اگر GTK_CHECK_نسخہ(3,0,0)
	g_ماخذ_remove(channel->ch_part[part].ch_داخلہ_Handler);
#   ورنہ
	gdk_داخلہ_remove(channel->ch_part[part].ch_داخلہ_Handler);
#   endif
	channel->ch_part[part].ch_داخلہ_Handler = 0;
    }
#  endif
# endif
}

    ساکن خالی
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    برائے (part = PART_SOCK; part < PART_اندر; ++part)
	channel_gui_unregister_one(channel, part);
}

#اگر_ختم  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    ساکن صحیح
channel_connect(
	channel_T *channel,
	ثابت ساخت sockaddr *server_addr,
	صحیح server_addrlen,
	صحیح *waittime)
{
    صحیح		sd = -1;
#اگر_تعریف_شدہ MSWIN
    u_long	val = 1;
#اگر_ختم

    جب (TRUE)
    {
	طویل	elapsed_msec = 0;
	صحیح	waitnow;
	صحیح	ret;

	اگر (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	اگر (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_اندر_channel_connect));
	    واپس -1;
	}

	اگر (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    اگر (
#اگر_تعریف_شدہ MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#ورنہ
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#اگر_ختم
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		واپس -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	اگر (ret == 0)
	    // The connection could be established.
	    توڑو;

	SOCK_ERRNO;
	اگر (*waittime < 0 || (errno != EWOULDBLOCK
		&& errno != ECONNREFUSED
#اگر_تعریف_شدہ EINPROGRESS
		&& errno != EINPROGRESS
#اگر_ختم
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
	    PERROR(_(e_cannot_connect_مقصد_پورٹ));
	    sock_close(sd);
	    واپس -1;
	}
	ورنہ اگر (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    واپس -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    ساخت timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#اگر_غیر_تعریف_شدہ MSWIN
	    صحیح			so_error = 0;
	    socklen_t		so_error_len = حجم(so_error);
	    ساخت timeval	آغاز_tv;
	    ساخت timeval	انتہا_tv;
#اگر_ختم
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#اگر_غیر_تعریف_شدہ MSWIN
	    gettimeofday(&آغاز_tv, تہی);
#اگر_ختم
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, تہی, &tv);
	    اگر (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_مقصد_پورٹ));
		sock_close(sd);
		واپس -1;
	    }

#اگر_تعریف_شدہ MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    اگر (FD_ISSET(sd, &wfds))
		توڑو;
	    elapsed_msec = waitnow;
	    اگر (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		جاری;
	    }
#ورنہ
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
		    PERROR(_(e_cannot_connect_مقصد_پورٹ));
		    sock_close(sd);
		    واپس -1;
		}
		ورنہ اگر (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    واپس -1;
		}
	    }

	    اگر (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		توڑو;

	    gettimeofday(&انتہا_tv, تہی);
	    elapsed_msec = (انتہا_tv.tv_sec - آغاز_tv.tv_sec) * 1000
				 + (انتہا_tv.tv_usec - آغاز_tv.tv_usec) / 1000;
#اگر_ختم
	}

#اگر_غیر_تعریف_شدہ MSWIN
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
		mch_delay((طویل)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    اگر (!got_int)
	    {
		اگر (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		جاری;
	    }
	    // we were interrupted, behave as if timed out
	}
#اگر_ختم

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	واپس -1;
    }

    اگر (*waittime >= 0)
    {
#اگر_تعریف_شدہ MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#ورنہ
	(خالی)fcntl(sd, F_SETFL, 0);
#اگر_ختم
    }

    واپس sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ثابت حرف *راستہ,
	خالی (*nb_close_cb)(خالی))
{
    channel_T		*channel = تہی;
    صحیح			sd = -1;
    size_t		راستہ_len = STRLEN(راستہ);
    ساخت sockaddr_un	server;
    size_t		server_len;
    صحیح			waittime = -1;

    اگر (*راستہ == NUL || راستہ_len >= حجم(server.sun_راستہ))
    {
	semsg(_(e_invalid_دلیل_str), راستہ);
	واپس تہی;
    }

#اگر_تعریف_شدہ MSWIN
    channel_init_winsock();
#اگر_ختم

    channel = جوڑو_channel();
    اگر (channel == تہی)
    {
	ch_error(تہی, "Cannot allocate channel.");
	واپس تہی;
    }

    CLEAR_میدان(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_راستہ, راستہ, حجم(server.sun_راستہ) - 1);

    ch_log(channel, "Trying to connect to %s", راستہ);

    server_len = offsetof(ساخت sockaddr_un, sun_راستہ) + راستہ_len + 1;
    sd = channel_connect(channel, (ساخت sockaddr *)&server, (صحیح)server_len,
								   &waittime);

    اگر (sd < 0)
    {
	channel_free(channel);
	واپس تہی;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)راستہ);
    channel->ch_پورٹ = 0;
    channel->ch_مقصد_be_closed |= (1U << PART_SOCK);

#اگر_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#اگر_ختم

    واپس channel;
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
	صحیح پورٹ,
	صحیح waittime,
	خالی (*nb_close_cb)(خالی))
{
    صحیح			sd = -1;
    channel_T		*channel = تہی;
#اگر_تعریف_شدہ FEAT_IPV6
    صحیح			err;
    ساخت addrinfo	hints;
    ساخت addrinfo	*res = تہی;
    ساخت addrinfo	*addr = تہی;
#ورنہ
    ساخت sockaddr_اندر	server;
    ساخت hostent	*host = تہی;
#اگر_ختم

#اگر_تعریف_شدہ MSWIN
    channel_init_winsock();
#اگر_ختم

    channel = جوڑو_channel();
    اگر (channel == تہی)
    {
	ch_error(تہی, "Cannot allocate channel.");
	واپس تہی;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#اگر_تعریف_شدہ FEAT_IPV6
    CLEAR_میدان(hints);
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
    اگر ((err = getaddrinfo(hostname, تہی, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_اندر_channel_open_str), gai_strerror(err));
	channel_free(channel);
	واپس تہی;
    }

    // Count the number of addresses for timeout distribution
    صحیح addr_گنتی = 0;
    برائے (addr = res; addr != تہی; addr = addr->ai_اگلا)
	addr_گنتی++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    اگر (waittime == 0)
	waittime = 15;

    صحیح addr_اشاریہ = 0;
    برائے (addr = res; addr != تہی; addr = addr->ai_اگلا, addr_اشاریہ++)
    {
	ثابت حرف  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ثابت خالی  *src = تہی;
	حرف	    buf[NUMBUFLEN];
# endif
	صحیح	    try_waittime;
	صحیح	    before_waittime;
	صحیح	    consumed;
	صحیح	    بقایا_addrs;

	اگر (addr->ai_family == AF_INET6)
	{
	    ساخت sockaddr_in6 *sai = (ساخت sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_پورٹ = htons(پورٹ);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	ورنہ اگر (addr->ai_family == AF_INET)
	{
	    ساخت sockaddr_اندر *sai = (ساخت sockaddr_اندر *)addr->ai_addr;

	    sai->sin_پورٹ = htons(پورٹ);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	اگر (src != تہی)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, حجم(buf));
	    اگر (dst == تہی)
		dst = hostname;
	    ورنہ اگر (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, پورٹ);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	اگر (addr->ai_اگلا == تہی)
	    try_waittime = waittime;
	ورنہ اگر (addr_اشاریہ == 0)
	{
	    اگر (waittime > 500)
		try_waittime = 250;
	    ورنہ اگر (waittime > 30)
		try_waittime = waittime / 2;
	    ورنہ
		try_waittime = waittime;
	}
	ورنہ
	{
	    بقایا_addrs = addr_گنتی - addr_اشاریہ;
	    try_waittime = waittime / بقایا_addrs;
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
	    توڑو;

	// If we have no time left, stop trying
	اگر (waittime <= 0 && addr->ai_اگلا != تہی)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    توڑو;
	}
    }

    freeaddrinfo(res);
#ورنہ
    CLEAR_میدان(server);
    server.sin_family = AF_INET;
    server.sin_پورٹ = htons(پورٹ);
    اگر ((host = gethostbyname(hostname)) == تہی)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_اندر_channel_open));
	channel_free(channel);
	واپس تہی;
    }
    {
	حرف *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	حافظہ_نقل(&p, &host->h_addr_list[0], حجم(p));
	حافظہ_نقل((حرف *)&server.sin_addr, p, host->h_لمبائی);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, پورٹ);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    اگر (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (ساخت sockaddr *)&server, حجم(server),
								   &waittime);
#اگر_ختم

    اگر (sd < 0)
    {
	channel_free(channel);
	واپس تہی;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)hostname);
    channel->ch_پورٹ = پورٹ;
    channel->ch_مقصد_be_closed |= (1U << PART_SOCK);

#اگر_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#اگر_ختم

    واپس channel;
}

    ساکن خالی
free_سیٹ_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    اگر (callback->cb_name != تہی && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    ورنہ
	cbp->cb_name = تہی;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ساکن خالی
prepare_بفر(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#اگر_تعریف_شدہ FEAT_QUICKFIX
    سیٹ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    سیٹ_option_قدر_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#اگر_ختم
    اگر (curbuf->b_ml.ml_mfp == تہی)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ساکن buf_T *
channel_ڈھونڈو_بفر(char_u *name, صحیح err, صحیح پیغام)
{
    buf_T *buf = تہی;
    buf_T *save_curbuf = curbuf;

    اگر (name != تہی && *name != NUL)
    {
	buf = buflist_findname(name);
	اگر (buf == تہی)
	    buf = buflist_findname_exp(name);
    }

    اگر (buf != تہی)
	واپس buf;

    buf = buflist_new(name == تہی || *name == NUL ? تہی : name,
	    تہی, (linenr_T)0, BLN_LISTED | BLN_NEW);
    اگر (buf == تہی)
	واپس تہی;
    prepare_بفر(buf);

    curbuf = buf;
    اگر (پیغام)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    بدلا_بائٹیں(1, 0);
    curbuf = save_curbuf;

    واپس buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ساکن خالی
channel_سیٹ_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    اگر (opt->jo_سیٹ & JO_منوال)
	برائے (part = PART_SOCK; part < PART_گنتی; ++part)
	    channel->ch_part[part].ch_منوال = opt->jo_منوال;
    اگر (opt->jo_سیٹ & JO_اندر_منوال)
	channel->ch_part[PART_اندر].ch_منوال = opt->jo_اندر_منوال;
    اگر (opt->jo_سیٹ & JO_برآمد_منوال)
	channel->ch_part[PART_برآمد].ch_منوال = opt->jo_برآمد_منوال;
    اگر (opt->jo_سیٹ & JO_ERR_منوال)
	channel->ch_part[PART_ERR].ch_منوال = opt->jo_err_منوال;
    channel->ch_nonblock = opt->jo_noblock;

    اگر (opt->jo_سیٹ & JO_TIMEOUT)
	برائے (part = PART_SOCK; part < PART_گنتی; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    اگر (opt->jo_سیٹ & JO_برآمد_TIMEOUT)
	channel->ch_part[PART_برآمد].ch_timeout = opt->jo_برآمد_timeout;
    اگر (opt->jo_سیٹ & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    اگر (opt->jo_سیٹ & JO_بلاک_WRITE)
	channel->ch_part[PART_اندر].ch_بلاک_write = 1;

    اگر (opt->jo_سیٹ & JO_CALLBACK)
	free_سیٹ_callback(&channel->ch_callback, &opt->jo_callback);
    اگر (opt->jo_سیٹ & JO_برآمد_CALLBACK)
	free_سیٹ_callback(&channel->ch_part[PART_برآمد].ch_callback,
							      &opt->jo_برآمد_cb);
    اگر (opt->jo_سیٹ & JO_ERR_CALLBACK)
	free_سیٹ_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    اگر (opt->jo_سیٹ & JO_CLOSE_CALLBACK)
	free_سیٹ_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    اگر ((opt->jo_سیٹ & JO_برآمد_IO) && opt->jo_io[PART_برآمد] == JIO_بفر)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	اگر (!(opt->jo_سیٹ & JO_برآمد_منوال))
	    channel->ch_part[PART_برآمد].ch_منوال = CH_منوال_NL;
	اگر (opt->jo_سیٹ & JO_برآمد_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_برآمد]);
	    اگر (buf == تہی)
		semsg(_(e_بفر_nr_does_not_exist),
					       (طویل)opt->jo_io_buf[PART_برآمد]);
	}
	ورنہ
	{
	    صحیح پیغام = TRUE;

	    اگر (opt->jo_set2 & JO2_برآمد_پیغام)
		پیغام = opt->jo_پیغام[PART_برآمد];
	    buf = channel_ڈھونڈو_بفر(opt->jo_io_name[PART_برآمد], FALSE, پیغام);
	}
	اگر (buf != تہی)
	{
	    اگر (opt->jo_سیٹ & JO_برآمد_MODIFIABLE)
		channel->ch_part[PART_برآمد].ch_nomodifiable =
						!opt->jo_modifiable[PART_برآمد];

	    اگر (!buf->b_p_ma && !channel->ch_part[PART_برآمد].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ورنہ
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (حرف *)buf->b_ffname);
		سیٹ_bufref(&channel->ch_part[PART_برآمد].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		اگر (buf->b_ml.ml_mfp == تہی)
		    prepare_بفر(buf);
	    }
	}
    }

    اگر ((opt->jo_سیٹ & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_بفر
	 || (opt->jo_io[PART_ERR] == JIO_برآمد && (opt->jo_سیٹ & JO_برآمد_IO)
				       && opt->jo_io[PART_برآمد] == JIO_بفر)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	اگر (!(opt->jo_سیٹ & JO_ERR_منوال))
	    channel->ch_part[PART_ERR].ch_منوال = CH_منوال_NL;
	اگر (opt->jo_io[PART_ERR] == JIO_برآمد)
	    buf = channel->ch_part[PART_برآمد].ch_bufref.br_buf;
	ورنہ اگر (opt->jo_سیٹ & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    اگر (buf == تہی)
		semsg(_(e_بفر_nr_does_not_exist),
					       (طویل)opt->jo_io_buf[PART_ERR]);
	}
	ورنہ
	{
	    صحیح پیغام = TRUE;

	    اگر (opt->jo_set2 & JO2_ERR_پیغام)
		پیغام = opt->jo_پیغام[PART_ERR];
	    buf = channel_ڈھونڈو_بفر(opt->jo_io_name[PART_ERR], TRUE, پیغام);
	}
	اگر (buf != تہی)
	{
	    اگر (opt->jo_سیٹ & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    اگر (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ورنہ
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (حرف *)buf->b_ffname);
		سیٹ_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		اگر (buf->b_ml.ml_mfp == تہی)
		    prepare_بفر(buf);
	    }
	}
    }

    channel->ch_part[PART_برآمد].ch_io = opt->jo_io[PART_برآمد];
    channel->ch_part[PART_ERR].ch_io = opt->jo_io[PART_ERR];
    channel->ch_part[PART_اندر].ch_io = opt->jo_io[PART_اندر];
}

/*
 * Parse the address for socketserver and store port in "port", or the path in
 * "unix_path" if it is a unix domain socket. Returns OK on success and FAIL on
 * failure.
 */
    صحیح
channel_parse_socketserver_پتہ(
	char_u	*پتہ,
	صحیح	*پورٹ,
	char_u	**unix_راستہ,
	bool	quiet)
{
    حرف    *rest;
    طویل    val;

    اگر (*پتہ == NUL)
	جاؤ fail;

    اگر (STRNCMP(پتہ, "unix:", 5) == 0)
    {
	*unix_راستہ = vim_strsave(پتہ + 5);
	واپس OK;
    }

    val = strtol((حرف *)(پتہ), &rest, 10);
    اگر (val < 0 || val >= 65536 || *rest != NUL)
    {
	اگر (!quiet)
	    semsg(_(e_invalid_دلیل_str), پتہ);
	واپس FAIL;
    }
    *پورٹ = (صحیح)val;

    واپس OK;
fail:
    اگر (!quiet)
	semsg(_(e_invalid_دلیل_str), پتہ);
    واپس FAIL;
}

/*
 * Implements ch_open().
 */
    ساکن channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*پتہ;
    char_u	*p;
    حرف	*rest;
    صحیح		پورٹ = 0;
    صحیح		is_ipv6 = FALSE;
    صحیح		is_unix = FALSE;
    صحیح		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = تہی;

    اگر (اندر_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس تہی;

    پتہ = tv_get_string(&argvars[0]);
    اگر (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	واپس تہی;

    اگر (*پتہ == NUL)
    {
	semsg(_(e_invalid_دلیل_str), پتہ);
	واپس تہی;
    }

    اگر (STRCMP(پتہ, "stdio") == 0)
	is_stdio = TRUE;
    ورنہ اگر (STRNCMP(پتہ, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	پتہ += 5;
    }
    ورنہ اگر (*پتہ == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(پتہ + 1, ']');
	اگر (p == تہی || *++p != ':')
	{
	    semsg(_(e_invalid_دلیل_str), پتہ);
	    واپس تہی;
	}
    }
    ورنہ
    {
	// ipv4 address
	p = vim_strchr(پتہ, ':');
	اگر (p == تہی)
	{
	    semsg(_(e_invalid_دلیل_str), پتہ);
	    واپس تہی;
	}
    }

    اگر (!is_unix && !is_stdio)
    {
	پورٹ = strtol((حرف *)(p + 1), &rest, 10);
	اگر (پورٹ <= 0 || پورٹ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_دلیل_str), پتہ);
	    واپس تہی;
	}
	اگر (is_ipv6)
	{
	    // strip '[' and ']'
	    ++پتہ;
	    *(p - 1) = NUL;
	}
	ورنہ
	    *p = NUL;
    }

    // parse options
    clear_کام_options(&opt);
    opt.jo_منوال = CH_منوال_JSON;
    opt.jo_timeout = 2000;
    اگر (get_کام_options(&argvars[1], &opt,
	    JO_منوال_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	جاؤ theend;
    اگر (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_دلیل));
	جاؤ theend;
    }

    اگر (is_stdio)
	channel = channel_open_stdio();
    ورنہ اگر (is_unix)
	channel = channel_open_unix((حرف *)پتہ, تہی);
    ورنہ
	channel = channel_open((حرف *)پتہ, پورٹ, opt.jo_waittime, تہی);
    اگر (channel != تہی)
    {
	صحیح	user_سیٹ = opt.jo_سیٹ;

	opt.jo_سیٹ = JO_ALL;
	اگر (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_سیٹ &= ~((JO_اندر_منوال | JO_برآمد_منوال | JO_ERR_منوال
				| JO_برآمد_TIMEOUT | JO_ERR_TIMEOUT) & ~user_سیٹ);
	channel_سیٹ_options(channel, &opt);
    }
theend:
    free_کام_options(&opt);
    واپس channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
ساکن channel_T *stdio_channel = تہی;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    ساکن channel_T *
channel_open_stdio(خالی)
{
    channel_T	*channel;
    sock_T	اندر_fd;		// our stdin, the channel reads from it
    sock_T	برآمد_fd;		// our stdout, the channel writes to it

    اگر (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	واپس تہی;
    }
    اگر (stdio_channel != تہی)
    {
	emsg(_(e_cannot_open_stdio_channel));
	واپس تہی;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#اگر_تعریف_شدہ MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	صحیح	nul_fd;

	اگر (!DuplicateHandle(proc, GetStdHandle(STD_داخلہ_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    واپس تہی;
	}
	اگر (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    واپس تہی;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, تہی, OPEN_EXISTING, 0,
		تہی);
	اگر (hnul != INVALID_HANDLE_قدر)
	{
	    سیٹ_Std_Handle(STD_داخلہ_HANDLE, hnul);
	    سیٹ_Std_Handle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	اگر (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	اندر_fd = (sock_T)hin;
	برآمد_fd = (sock_T)hout;
    }
#ورنہ
    {
	صحیح	nul_fd;

	اندر_fd = dup(0);
	برآمد_fd = dup(1);
	اگر (اندر_fd < 0 || برآمد_fd < 0)
	{
	    اگر (اندر_fd >= 0)
		close(اندر_fd);
	    اگر (برآمد_fd >= 0)
		close(برآمد_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    واپس تہی;
	}
	(خالی)fcntl(اندر_fd, F_SETFD, FD_CLOEXEC);
	(خالی)fcntl(برآمد_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	اگر (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#اگر_ختم

    channel = جوڑو_channel();
    اگر (channel == تہی)
    {
	ch_error(تہی, "Cannot allocate channel.");
	fd_close(اندر_fd);
	fd_close(برآمد_fd);
	واپس تہی;
    }
    channel_سیٹ_pipes(channel, برآمد_fd, اندر_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    واپس channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    حرف	*rest;
    صحیح		پورٹ;
    صحیح		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = تہی;

    اگر (اندر_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	واپس تہی;

    arg = tv_get_string(&argvars[0]);
    اگر (*arg == NUL)
    {
	semsg(_(e_invalid_دلیل_str), arg);
	واپس تہی;
    }

    اگر (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	پورٹ = 0;
    }
    ورنہ
    {
	پورٹ = strtol((حرف *)(arg), &rest, 10);
	اگر (پورٹ < 0 || پورٹ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_دلیل_str), arg);
	    واپس تہی;
	}
	*arg = NUL;
    }

    // parse options
    clear_کام_options(&opt);
    opt.jo_منوال = CH_منوال_JSON;
    opt.jo_timeout = 2000;
    اگر (get_کام_options(&argvars[1], &opt,
	    JO_منوال_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	جاؤ theend;
    اگر (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_دلیل));
	جاؤ theend;
    }

    اگر (is_unix)
	channel = channel_listen_unix((حرف *)arg, تہی, true);
    ورنہ
	channel = channel_listen(پورٹ, تہی);
    اگر (channel != تہی)
    {
	opt.jo_سیٹ = JO_ALL;
	channel_سیٹ_options(channel, &opt);
    }
theend:
    free_کام_options(&opt);
    واپس channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	صحیح پورٹ_اندر,
	خالی (*nb_close_cb)(خالی))
{
    صحیح			sd = -1;
    ساخت sockaddr_اندر	server;
#اگر_غیر_تعریف_شدہ FEAT_IPV6
    ساخت hostent	*host;
#اگر_ختم
    صحیح			val = 1;
    channel_T		*channel;

#اگر_تعریف_شدہ MSWIN
    channel_init_winsock();
#اگر_ختم

    channel = جوڑو_channel();
    اگر (channel == تہی)
    {
	ch_error(تہی, "Cannot allocate channel.");
	واپس تہی;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((حرف *)&server, 0, حجم(server));
    server.sin_family = AF_INET;
    server.sin_پورٹ = htons(پورٹ_اندر);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    اگر (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	channel_free(channel);
	واپس تہی;
    }

#اگر_تعریف_شدہ MSWIN
    اگر (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ثابت حرف *)&val, حجم(val)) < 0)
#ورنہ
    اگر (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, حجم(val)) < 0)
#اگر_ختم
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	واپس تہی;
    }

    // Bind the socket to the port
    اگر (bind(sd, (ساخت sockaddr *)&server, حجم(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	واپس تہی;
    }

    اگر (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	واپس تہی;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    اگر (پورٹ_اندر == 0)
    {
	ساخت sockaddr_اندر	addr;
	socklen_t		addr_len = حجم(addr);

	اگر (getsockname(sd, (ساخت sockaddr *)&addr, &addr_len) == 0)
	    پورٹ_اندر = ntohs(addr.sin_پورٹ);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)"");
    channel->ch_پورٹ = پورٹ_اندر;
    channel->ch_مقصد_be_closed |= (1U << PART_SOCK);

#اگر_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#اگر_ختم

    واپس channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	حرف *راستہ,
	خالی (*nb_close_cb)(خالی),
	bool replace)
{
    صحیح			sd = -1;
    ساخت sockaddr_un	server;
    channel_T		*channel;
    size_t		راستہ_len;
    size_t		server_len;

    اگر (راستہ == تہی || *راستہ == NUL)
    {
	semsg(_(e_invalid_دلیل_str), راستہ == تہی ? (حرف *)"" : راستہ);
	واپس تہی;
    }

    راستہ_len = STRLEN(راستہ);
    اگر (راستہ_len >= حجم(server.sun_راستہ))
    {
	semsg(_(e_invalid_دلیل_str), راستہ);
	واپس تہی;
    }

#اگر_تعریف_شدہ MSWIN
    channel_init_winsock();
#اگر_ختم

    channel = جوڑو_channel();
    اگر (channel == تہی)
    {
	ch_error(تہی, "Cannot allocate channel.");
	واپس تہی;
    }

    CLEAR_میدان(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_راستہ, راستہ, حجم(server.sun_راستہ) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    اگر (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	channel_free(channel);
	واپس تہی;
    }

    اگر (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_راستہ);

    // Bind the socket to the path
    server_len = offsetof(ساخت sockaddr_un, sun_راستہ) + راستہ_len + 1;
    اگر (bind(sd, (ساخت sockaddr *)&server, (صحیح)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	واپس تہی;
    }

    اگر (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	واپس تہی;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)راستہ);
    channel->ch_پورٹ = 0;
    channel->ch_مقصد_be_closed |= (1U << PART_SOCK);

#اگر_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#اگر_ختم

    واپس channel;
}

     خالی
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    اگر (*fd == INVALID_FD)
	واپس;

    اگر (part == PART_SOCK)
	sock_close(*fd);
    ورنہ
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	اگر ((part == PART_اندر || channel->CH_اندر_FD != *fd)
		&& (part == PART_برآمد || channel->CH_برآمد_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#اگر_تعریف_شدہ MSWIN
	    اگر (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#اگر_ختم
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_مقصد_be_closed &= ~(1U << part);
}

    خالی
channel_سیٹ_pipes(channel_T *channel, sock_T اندر, sock_T برآمد, sock_T err)
{
    اگر (اندر != INVALID_FD)
    {
	ch_close_part(channel, PART_اندر);
	channel->CH_اندر_FD = اندر;
#اگر defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	اگر (mch_isatty(اندر))
	    channel->ch_مقصد_be_closed |= (1U << PART_اندر);
#اگر_ختم
    }
    اگر (برآمد != INVALID_FD)
    {
#اگر defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_برآمد);
#اگر_ختم
	ch_close_part(channel, PART_برآمد);
	channel->CH_برآمد_FD = برآمد;
	channel->ch_مقصد_be_closed |= (1U << PART_برآمد);
#اگر defined(FEAT_GUI)
	channel_gui_رجسٹر_one(channel, PART_برآمد);
#اگر_ختم
    }
    اگر (err != INVALID_FD)
    {
#اگر defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#اگر_ختم
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_مقصد_be_closed |= (1U << PART_ERR);
#اگر defined(FEAT_GUI)
	channel_gui_رجسٹر_one(channel, PART_ERR);
#اگر_ختم
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    خالی
channel_سیٹ_کام(channel_T *channel, کام_T *کام, jobopt_T *options)
{
    channel->ch_کام = کام;

    channel_سیٹ_options(channel, options);

    اگر (کام->jv_اندر_buf == تہی)
	واپس;

    chanpart_T *اندر_part = &channel->ch_part[PART_اندر];

    سیٹ_bufref(&اندر_part->ch_bufref, کام->jv_اندر_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (حرف *)اندر_part->ch_bufref.br_buf->b_ffname);
    اگر (options->jo_سیٹ & JO_اندر_چوٹی)
    {
	اگر (options->jo_اندر_چوٹی == 0 && !(options->jo_سیٹ & JO_اندر_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    اندر_part->ch_bufref.br_buf->b_write_مقصد_channel = true;
	    اندر_part->ch_buf_append = TRUE;
	    اندر_part->ch_buf_چوٹی =
		اندر_part->ch_bufref.br_buf->b_ml.ml_سطر_گنتی + 1;
	}
	ورنہ
	    اندر_part->ch_buf_چوٹی = options->jo_اندر_چوٹی;
    }
    ورنہ
	اندر_part->ch_buf_چوٹی = 1;
    اگر (options->jo_سیٹ & JO_اندر_BOT)
	اندر_part->ch_buf_bot = options->jo_اندر_bot;
    ورنہ
	اندر_part->ch_buf_bot = اندر_part->ch_bufref.br_buf->b_ml.ml_سطر_گنتی;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ساکن خالی
channel_سیٹ_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	صحیح	    id)
{
    cbq_T *سر = &channel->ch_part[part].ch_cb_سر;
    cbq_T *شے = ALLOC_ONE(cbq_T);

    اگر (شے == تہی)
	واپس;

    copy_callback(&شے->cq_callback, callback);
    شے->cq_seq_nr = id;
    شے->cq_پچھلا = سر->cq_پچھلا;
    سر->cq_پچھلا = شے;
    شے->cq_اگلا = تہی;
    اگر (شے->cq_پچھلا == تہی)
	سر->cq_اگلا = شے;
    ورنہ
	شے->cq_پچھلا->cq_اگلا = شے;
}

    ساکن خالی
write_buf_سطر(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *سطر = ml_get_buf(buf, lnum, FALSE);
    صحیح	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    صحیح	    i;

    // Need to make a copy to be able to append a NL.
    اگر ((p = alloc(len + 2)) == تہی)
	واپس;
    حافظہ_نقل((حرف *)p, (حرف *)سطر, len);

    اگر (channel->ch_write_متن_منوال)
	p[len] = CAR;
    ورنہ
    {
	برائے (i = 0; i < len; ++i)
	    اگر (p[i] == NL)
		p[i] = NUL;

	p[len] = NL;
    }
    p[len + 1] = NUL;
    channel_send(channel, PART_اندر, p, len + 1, "write_buf_line");
    vim_free(p);
}

/*
 * Return TRUE if "channel" can be written to.
 * Returns FALSE if the input is closed or the write would block.
 */
    ساکن صحیح
can_write_buf_سطر(channel_T *channel)
{
    chanpart_T *اندر_part = &channel->ch_part[PART_اندر];

    اگر (اندر_part->ch_fd == INVALID_FD)
	واپس FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    اگر (اندر_part->ch_بلاک_write == 1)
	اندر_part->ch_بلاک_write = -1;
    ورنہ اگر (اندر_part->ch_بلاک_write == -1)
	اندر_part->ch_بلاک_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#اگر_غیر_تعریف_شدہ MSWIN
    {
# اگر defined(HAVE_SELECT)
	ساخت timeval	tval;
	fd_set		wfds;
	صحیح		ret;

	FD_ZERO(&wfds);
	FD_SET((صحیح)اندر_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	برائے (;;)
	{
	    ret = select((صحیح)اندر_part->ch_fd + 1, تہی, &wfds, تہی, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    اگر (ret == -1 && errno == EINTR)
		جاری;
#  endif
	    اگر (ret <= 0 || اندر_part->ch_بلاک_write == 1)
	    {
		اگر (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		ورنہ
		    ch_log(channel, "Input not ready for writing");
		واپس FALSE;
	    }
	    توڑو;
	}
# ورنہ
	ساخت pollfd	fds;

	fds.fd = اندر_part->ch_fd;
	fds.events = POLLOUT;
	اگر (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    واپس FALSE;
	}
	اگر (اندر_part->ch_بلاک_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    واپس FALSE;
	}
# endif
    }
#اگر_ختم
    واپس TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    خالی
channel_write_اندر(channel_T *channel)
{
    chanpart_T *اندر_part = &channel->ch_part[PART_اندر];
    linenr_T    lnum;
    buf_T	*buf = اندر_part->ch_bufref.br_buf;
    صحیح		written = 0;

    اگر (buf == تہی || اندر_part->ch_buf_append)
	واپس;  // no buffer or using appending
    اگر (!bufref_valid(&اندر_part->ch_bufref) || buf->b_ml.ml_mfp == تہی)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	اندر_part->ch_bufref.br_buf = تہی;
	واپس;
    }

    برائے (lnum = اندر_part->ch_buf_چوٹی; lnum <= اندر_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_سطر_گنتی; ++lnum)
    {
	اگر (!can_write_buf_سطر(channel))
	    توڑو;
	write_buf_سطر(buf, lnum, channel);
	++written;
    }

    اگر (written == 1)
	ch_log(channel, "written line %d to channel", (صحیح)lnum - 1);
    ورنہ اگر (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    اندر_part->ch_buf_چوٹی = lnum;
    اگر (lnum > buf->b_ml.ml_سطر_گنتی || lnum > اندر_part->ch_buf_bot)
    {
#اگر defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	اگر (channel->ch_کام != تہی)
	    رکن_send_eof(channel);
#اگر_ختم

	// Writing is done, no longer need the buffer.
	اندر_part->ch_bufref.br_buf = تہی;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#اگر_تعریف_شدہ MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	اگر (!channel->ch_anonymous_pipe)
#اگر_ختم
	    ch_close_part(channel, PART_اندر);
    }
    ورنہ
	ch_log(channel, "Still %ld more lines to write",
				   (طویل)(buf->b_ml.ml_سطر_گنتی - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    خالی
channel_بفر_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	برائے (part = PART_SOCK; part < PART_گنتی; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    اگر (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_نام[part]);
		ch_part->ch_bufref.br_buf = تہی;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    خالی
channel_write_داخلہ(channel_T *channel)
{
    chanpart_T	*اندر_part = &channel->ch_part[PART_اندر];

    اگر (اندر_part->ch_writeque.wq_اگلا != تہی)
	channel_send(channel, PART_اندر, (char_u *)"", 0, "channel_write_input");
    ورنہ اگر (اندر_part->ch_bufref.br_buf != تہی)
    {
	اگر (اندر_part->ch_buf_append)
	    channel_write_new_سطریں(اندر_part->ch_bufref.br_buf);
	ورنہ
	    channel_write_اندر(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    خالی
channel_write_any_سطریں(خالی)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_داخلہ(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    خالی
channel_write_new_سطریں(buf_T *buf)
{
    channel_T	*channel;
    صحیح		ملا_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *اندر_part = &channel->ch_part[PART_اندر];
	linenr_T    lnum;
	صحیح	    written = 0;

	اگر (اندر_part->ch_bufref.br_buf == buf && اندر_part->ch_buf_append)
	{
	    اگر (اندر_part->ch_fd == INVALID_FD)
		جاری;  // pipe was closed
	    ملا_one = TRUE;
	    برائے (lnum = اندر_part->ch_buf_bot; lnum < buf->b_ml.ml_سطر_گنتی;
								       ++lnum)
	    {
		اگر (!can_write_buf_سطر(channel))
		    توڑو;
		write_buf_سطر(buf, lnum, channel);
		++written;
	    }

	    اگر (written == 1)
		ch_log(channel, "written line %d to channel", (صحیح)lnum - 1);
	    ورنہ اگر (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    اگر (lnum < buf->b_ml.ml_سطر_گنتی)
		ch_log(channel, "Still %ld more lines to write",
				       (طویل)(buf->b_ml.ml_سطر_گنتی - lnum));

	    اندر_part->ch_buf_bot = lnum;
	}
    }
    اگر (!ملا_one)
	buf->b_write_مقصد_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ساکن خالی
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    اگر (محفوظ_مقصد_invoke_callback == 0)
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

    واپس سر->rq_اگلا;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_پہلا_nl(readq_T *گرہ)
{
    char_u  *بفر = گرہ->rq_بفر;
    long_u  i;

    برائے (i = 0; i < گرہ->rq_buflen; ++i)
	اگر (بفر[i] == NL)
	    واپس بفر + i;
    واپس تہی;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, صحیح *outlen)
{
    readq_T *سر = &channel->ch_part[part].ch_سر;
    readq_T *گرہ = سر->rq_اگلا;
    char_u *p;

    اگر (گرہ == تہی)
	واپس تہی;
    اگر (outlen != تہی)
	*outlen += گرہ->rq_buflen;
    // dispose of the node but keep the buffer
    p = گرہ->rq_بفر;
    سر->rq_اگلا = گرہ->rq_اگلا;
    اگر (گرہ->rq_اگلا == تہی)
	سر->rq_پچھلا = تہی;
    ورنہ
	گرہ->rq_اگلا->rq_پچھلا = تہی;
    vim_free(گرہ);
    واپس p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ساکن char_u *
channel_get_all(channel_T *channel, ch_part_T part, صحیح *outlen)
{
    readq_T *سر = &channel->ch_part[part].ch_سر;
    readq_T *گرہ;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    برائے (گرہ = سر->rq_اگلا; گرہ != تہی; گرہ = گرہ->rq_اگلا)
	len += گرہ->rq_buflen;
    res = alloc(len + 1);
    اگر (res == تہی)
	واپس تہی;
    p = res;
    برائے (گرہ = سر->rq_اگلا; گرہ != تہی; گرہ = گرہ->rq_اگلا)
    {
	mch_memmove(p, گرہ->rq_بفر, گرہ->rq_buflen);
	p += گرہ->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    کرو
    {
	p = channel_get(channel, part, تہی);
	vim_free(p);
    } جب (p != تہی);

    اگر (outlen != تہی)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	واپس res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    جب (p < res + len)
    {
	اگر (*p == NUL)
	    *p = NL;
#اگر_تعریف_شدہ MSWIN
	ورنہ اگر (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    اگر (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		جب (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		اگر (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#اگر_ختم
	++p;
    }

    واپس res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    خالی
channel_consume(channel_T *channel, ch_part_T part, صحیح len)
{
    readq_T *سر = &channel->ch_part[part].ch_سر;
    readq_T *گرہ = سر->rq_اگلا;
    char_u *buf = گرہ->rq_بفر;

    اگر (len < 0 || (long_u)len > گرہ->rq_buflen)
	واپس;
    mch_memmove(buf, buf + len, گرہ->rq_buflen - len);
    گرہ->rq_buflen -= len;
    گرہ->rq_بفر[گرہ->rq_buflen] = NUL;
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
    ch_منوال_T	منوال = channel->ch_part[part].ch_منوال;
    readq_T	*سر = &channel->ch_part[part].ch_سر;
    readq_T	*گرہ = سر->rq_اگلا;
    readq_T	*آخری_گرہ;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    اگر (گرہ == تہی || گرہ->rq_اگلا == تہی)
	واپس FAIL;

    آخری_گرہ = گرہ->rq_اگلا;
    len = گرہ->rq_buflen + آخری_گرہ->rq_buflen;
    اگر (want_nl || منوال == CH_منوال_LSP || منوال == CH_منوال_DAP)
	جب (آخری_گرہ->rq_اگلا != تہی
		&& (منوال == CH_منوال_LSP || منوال == CH_منوال_DAP
		    || channel_پہلا_nl(آخری_گرہ) == تہی))
	{
	    آخری_گرہ = آخری_گرہ->rq_اگلا;
	    len += آخری_گرہ->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    اگر (newbuf == تہی)
	واپس FAIL;	    // out of memory
    mch_memmove(p, گرہ->rq_بفر, گرہ->rq_buflen);
    p += گرہ->rq_buflen;
    vim_free(گرہ->rq_بفر);
    گرہ->rq_بفر = newbuf;
    برائے (n = گرہ; n != آخری_گرہ; )
    {
	n = n->rq_اگلا;
	mch_memmove(p, n->rq_بفر, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_بفر);
    }
    *p = NUL;
    گرہ->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    برائے (n = گرہ->rq_اگلا; n != آخری_گرہ; )
    {
	n = n->rq_اگلا;
	vim_free(n->rq_پچھلا);
    }
    گرہ->rq_اگلا = آخری_گرہ->rq_اگلا;
    اگر (آخری_گرہ->rq_اگلا == تہی)
	سر->rq_پچھلا = گرہ;
    ورنہ
	آخری_گرہ->rq_اگلا->rq_پچھلا = گرہ;
    vim_free(آخری_گرہ);
    واپس OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    ساکن صحیح
channel_save(channel_T *channel, ch_part_T part, char_u *buf, صحیح len,
						      صحیح prepend, حرف *lead)
{
    readq_T *گرہ;
    readq_T *سر = &channel->ch_part[part].ch_سر;
    char_u  *p;
    صحیح	    i;

    گرہ = ALLOC_ONE(readq_T);
    اگر (گرہ == تہی)
	واپس FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    گرہ->rq_بفر = alloc(len + 1);
    اگر (گرہ->rq_بفر == تہی)
    {
	vim_free(گرہ);
	واپس FAIL;	    // out of memory
    }

    اگر (channel->ch_part[part].ch_منوال == CH_منوال_NL)
    {
	// Drop any CR before a NL.
	p = گرہ->rq_بفر;
	برائے (i = 0; i < len; ++i)
	    اگر (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	گرہ->rq_buflen = (long_u)(p - گرہ->rq_بفر);
    }
    ورنہ
    {
	mch_memmove(گرہ->rq_بفر, buf, len);
	گرہ->rq_بفر[len] = NUL;
	گرہ->rq_buflen = (long_u)len;
    }

    اگر (prepend)
    {
	// prepend node to the head of the queue
	گرہ->rq_اگلا = سر->rq_اگلا;
	گرہ->rq_پچھلا = تہی;
	اگر (سر->rq_اگلا == تہی)
	    سر->rq_پچھلا = گرہ;
	ورنہ
	    سر->rq_اگلا->rq_پچھلا = گرہ;
	سر->rq_اگلا = گرہ;
    }
    ورنہ
    {
	// append node to the tail of the queue
	گرہ->rq_اگلا = تہی;
	گرہ->rq_پچھلا = سر->rq_پچھلا;
	اگر (سر->rq_پچھلا == تہی)
	    سر->rq_اگلا = گرہ;
	ورنہ
	    سر->rq_پچھلا->rq_اگلا = گرہ;
	سر->rq_پچھلا = گرہ;
    }

    اگر (ch_log_فعال() && lead != تہی)
	ch_log_literal(lead, channel, part, buf, len);

    واپس OK;
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
    char_u	*اگلا = channel_get(channel, part, تہی);
    صحیح		keeplen;
    صحیح		addlen;
    char_u	*p;

    اگر (اگلا == تہی)
	واپس FALSE;

    keeplen = reader->js_انتہا - reader->js_buf;
    اگر (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (صحیح)STRLEN(اگلا);
	p = alloc(keeplen + addlen + 1);
	اگر (p == تہی)
	{
	    vim_free(اگلا);
	    واپس FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, اگلا, addlen + 1);
	vim_free(اگلا);
	اگلا = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = اگلا;
    واپس TRUE;
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
    ساکن صحیح
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*سطر_آغاز;
    char_u	*p;
    int_u	hdr_len;
    صحیح		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_انتہا = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    جب (TRUE)
    {
	سطر_آغاز = p;
	جب (*p != NUL && *p != '\n')
	    p++;
	اگر (*p == NUL)			// partial header
	    واپس MAYBE;
	p++;

	// process the content length field (if present)
	اگر ((p - سطر_آغاز > 16)
		&& STRNICMP(سطر_آغاز, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((حرف *)سطر_آغاز + 16, تہی, 10);
	    اگر (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		واپس FAIL;
	}

	اگر ((p - سطر_آغاز) == 2 && سطر_آغاز[0] == '\r' &&
		سطر_آغاز[1] == '\n')
	    // reached the empty line
	    توڑو;
    }

    اگر (payload_len == -1)
	// Content-Length field is not present in the header
	واپس FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    اگر (jsbuf_len < hdr_len + payload_len)
	واپس MAYBE;

    reader->js_مستعمل += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_انتہا = reader->js_buf + hdr_len + payload_len;

    واپس OK;
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
    jsonq_T	*شے;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*سر = &chanpart->ch_json_سر;
    صحیح		حالت_2 = OK;
    صحیح		ret;

    اگر (channel_peek(channel, part) == تہی)
	واپس FALSE;

    reader.js_buf = channel_get(channel, part, تہی);
    reader.js_مستعمل = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    اگر (!socketserver && (chanpart->ch_منوال == CH_منوال_LSP
	    || chanpart->ch_منوال == CH_منوال_DAP))
	حالت_2 = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    اگر (حالت_2 == OK)
    {
	++emsg_silent;
	حالت_2 = json_decode(&reader, &listtv,
				chanpart->ch_منوال == CH_منوال_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    اگر (حالت_2 == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	اگر (!socketserver && (chanpart->ch_منوال == CH_منوال_LSP
		    || chanpart->ch_منوال == CH_منوال_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	ورنہ اگر (!socketserver && chanpart->ch_منوال != CH_منوال_LSP
		&& chanpart->ch_منوال != CH_منوال_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    اگر (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    ورنہ
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	ورنہ
	{
	    شے = ALLOC_ONE(jsonq_T);
	    اگر (شے == تہی)
		clear_tv(&listtv);
	    ورنہ
	    {
		شے->jq_no_callback = FALSE;
		شے->jq_قدر = alloc_tv();
		اگر (شے->jq_قدر == تہی)
		{
		    vim_free(شے);
		    clear_tv(&listtv);
		}
		ورنہ
		{
		    *شے->jq_قدر = listtv;
		    شے->jq_پچھلا = سر->jq_پچھلا;
		    سر->jq_پچھلا = شے;
		    شے->jq_اگلا = تہی;
		    اگر (شے->jq_پچھلا == تہی)
			سر->jq_اگلا = شے;
		    ورنہ
			شے->jq_پچھلا->jq_اگلا = شے;
		}
	    }
	}
    }

    اگر (حالت_2 == OK)
	chanpart->ch_wait_len = 0;
    ورنہ اگر (حالت_2 == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	اگر (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (صحیح)buflen);
	    reader.js_مستعمل = 0;
	    chanpart->ch_wait_len = buflen;
#اگر_تعریف_شدہ MSWIN
	    chanpart->ch_deadline = Get_Tick_گنتی() + 100L;
#ورنہ
	    gettimeofday(&chanpart->ch_deadline, تہی);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    اگر (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#اگر_ختم
	}
	ورنہ
	{
	    صحیح timeout;
#اگر_تعریف_شدہ MSWIN
	    timeout = (صحیح)(Get_Tick_گنتی() - chanpart->ch_deadline) > 0;
#ورنہ
	    {
		ساخت timeval now_tv;

		gettimeofday(&now_tv, تہی);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#اگر_ختم
	    اگر (timeout)
	    {
		حالت_2 = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    ورنہ
	    {
		reader.js_مستعمل = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    اگر (حالت_2 == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    ورنہ اگر (reader.js_buf[reader.js_مستعمل] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_مستعمل,
			(صحیح)(reader.js_انتہا - reader.js_buf) - reader.js_مستعمل,
								  TRUE, تہی);
	ret = حالت_2 == MAYBE ? FALSE: TRUE;
    }
    ورنہ
	ret = FALSE;

    vim_free(reader.js_buf);
    واپس ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ساکن خالی
remove_cb_گرہ(cbq_T *سر, cbq_T *گرہ)
{
    اگر (گرہ->cq_پچھلا == تہی)
	سر->cq_اگلا = گرہ->cq_اگلا;
    ورنہ
	گرہ->cq_پچھلا->cq_اگلا = گرہ->cq_اگلا;
    اگر (گرہ->cq_اگلا == تہی)
	سر->cq_پچھلا = گرہ->cq_پچھلا;
    ورنہ
	گرہ->cq_اگلا->cq_پچھلا = گرہ->cq_پچھلا;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    خالی
remove_json_گرہ(jsonq_T *سر, jsonq_T *گرہ)
{
    اگر (گرہ->jq_پچھلا == تہی)
	سر->jq_اگلا = گرہ->jq_اگلا;
    ورنہ
	گرہ->jq_پچھلا->jq_اگلا = گرہ->jq_اگلا;
    اگر (گرہ->jq_اگلا == تہی)
	سر->jq_پچھلا = گرہ->jq_پچھلا;
    ورنہ
	گرہ->jq_اگلا->jq_پچھلا = گرہ->jq_پچھلا;
    vim_free(گرہ);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ساکن خالی
channel_جوڑو_بلاک_id(chanpart_T *chanpart, صحیح id)
{
    garray_T *gap = &chanpart->ch_بلاک_ids;

    اگر (gap->ga_growsize == 0)
	ga_init2(gap, حجم(صحیح), 10);
    اگر (ga_grow(gap, 1) == OK)
    {
	((صحیح *)gap->ga_ڈیٹا)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ساکن خالی
channel_remove_بلاک_id(chanpart_T *chanpart, صحیح id)
{
    garray_T	*gap = &chanpart->ch_بلاک_ids;
    صحیح		i;

    برائے (i = 0; i < gap->ga_len; ++i)
	اگر (((صحیح *)gap->ga_ڈیٹا)[i] == id)
	{
	    --gap->ga_len;
	    اگر (i < gap->ga_len)
	    {
		صحیح *p = ((صحیح *)gap->ga_ڈیٹا) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * حجم(صحیح));
	    }
	    واپس;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ساکن صحیح
channel_has_بلاک_id(chanpart_T *chanpart, صحیح id)
{
    garray_T	*gap = &chanpart->ch_بلاک_ids;
    صحیح		i;

    برائے (i = 0; i < gap->ga_len; ++i)
	اگر (((صحیح *)gap->ga_ڈیٹا)[i] == id)
	    واپس TRUE;
    واپس FALSE;
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
    ساکن صحیح
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	صحیح	    id,
	صحیح	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;
    jsonq_T   *شے = سر->jq_اگلا;

    جب (شے != تہی)
    {
	list_T	    *l;
	typval_T    *tv;

	اگر (channel->ch_part[part].ch_منوال != CH_منوال_LSP
		&& channel->ch_part[part].ch_منوال != CH_منوال_DAP)
	{
	    l = شے->jq_قدر->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_پہلا->li_tv;
	}
	ورنہ
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    اگر (channel->ch_part[part].ch_منوال == CH_منوال_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		اگر (id > 0)
		{
		    اگر (شے->jq_قدر->v_type != VAR_DICT)
			جاؤ nextitem;
		    d = شے->jq_قدر->vval.v_dict;
		    اگر (d == تہی)
			جاؤ nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    اگر (dict_has_کلید(d, "method"))
			جاؤ nextitem;
		    di = dict_ڈھونڈو(d, (char_u *)"id", -1);
		    اگر (di == تہی)
			جاؤ nextitem;
		    tv = &di->di_tv;
		}
		ورنہ
		    tv = شے->jq_قدر;
	    }
	    ورنہ
	    {
		اگر (id > 0)
		{
		    اگر (شے->jq_قدر->v_type != VAR_DICT)
			جاؤ nextitem;
		    d = شے->jq_قدر->vval.v_dict;
		    اگر (d == تہی)
			جاؤ nextitem;
		    di = dict_ڈھونڈو(d, (char_u *)"request_seq", -1);
		    اگر (di == تہی)
			جاؤ nextitem;
		    tv = &di->di_tv;
		}
		ورنہ
		    tv = شے->jq_قدر;
	    }
	}

	اگر ((without_callback || !شے->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_نمبر && tv->vval.v_نمبر == id)
	      || (id <= 0 && (tv->v_type != VAR_نمبر
		 || tv->vval.v_نمبر == 0
		 || !channel_has_بلاک_id(
				&channel->ch_part[part], tv->vval.v_نمبر)))))
	{
	    *rettv = شے->jq_قدر;
	    اگر (tv->v_type == VAR_نمبر)
		ch_log(channel, "Getting JSON message %ld",
						      (طویل)tv->vval.v_نمبر);
	    remove_json_گرہ(سر, شے);
	    واپس OK;
	}
nextitem:
	شے = شے->jq_اگلا;
    }
    واپس FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ساکن خالی
channel_ڈالو_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;
    jsonq_T   *شے = سر->jq_اگلا;
    jsonq_T   *newitem;

    اگر (سر->jq_پچھلا != تہی && سر->jq_پچھلا->jq_no_callback)
	// last item was pushed back, append to the end
	شے = تہی;
    ورنہ جب (شے != تہی && شے->jq_no_callback)
	// append after the last item that was pushed back
	شے = شے->jq_اگلا;

    newitem = ALLOC_ONE(jsonq_T);
    اگر (newitem == تہی)
    {
	clear_tv(rettv);
	واپس;
    }

    newitem->jq_قدر = alloc_tv();
    اگر (newitem->jq_قدر == تہی)
    {
	vim_free(newitem);
	clear_tv(rettv);
	واپس;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_قدر = *rettv;
    اگر (شے == تہی)
    {
	// append to the end
	newitem->jq_پچھلا = سر->jq_پچھلا;
	سر->jq_پچھلا = newitem;
	newitem->jq_اگلا = تہی;
	اگر (newitem->jq_پچھلا == تہی)
	    سر->jq_اگلا = newitem;
	ورنہ
	    newitem->jq_پچھلا->jq_اگلا = newitem;
    }
    ورنہ
    {
	// append after "item"
	newitem->jq_پچھلا = شے;
	newitem->jq_اگلا = شے->jq_اگلا;
	شے->jq_اگلا = newitem;
	اگر (newitem->jq_اگلا == تہی)
	    سر->jq_پچھلا = newitem;
	ورنہ
	    newitem->jq_اگلا->jq_پچھلا = newitem;
    }
}

#تعریف_کرو CH_JSON_MAX_دلائل 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    ساکن خالی
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    صحیح	    options = channel->ch_part[part].ch_منوال == CH_منوال_JS
								 ? JSON_JS : 0;

    اگر (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	اگر (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_دلیل));
	واپس;
    }
    arg = argv[1].vval.v_string;
    اگر (arg == تہی)
	arg = (char_u *)"";

    اگر (STRCMP(cmd, "ex") == 0)
    {
	صحیح	called_emsg_before = called_emsg;
	char_u	*p = arg;
	صحیح	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (حرف *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	اگر (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	اگر (do_emsg_silent)
	    --emsg_silent;
	اگر (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (حرف *)get_vim_var_str(VV_ERRMSG));
    }
    ورنہ اگر (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (حرف *)arg);
	CLEAR_میدان(ea);
	ea.arg = arg;
	ea.addr_گنتی = 0;
	ea.forceit = TRUE; // no mapping
	ex_معمول(&ea);
    }
    ورنہ اگر (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	برآمد_flush_cursor(TRUE, FALSE);
    }
    ورنہ اگر (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	صحیح is_call = cmd[0] == 'c';
	صحیح id_idx = is_call ? 3 : 2;

	اگر (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_نمبر)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    اگر (p_verbose > 2)
		emsg(_(e_آخری_دلیل_for_اظہاریہ_call_must_be_نمبر));
	}
	ورنہ اگر (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    اگر (p_verbose > 2)
		emsg(_(e_تیسرا_دلیل_for_call_must_be_list));
	}
	ورنہ
	{
	    typval_T	*tv = تہی;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = تہی;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    اگر (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (حرف *)arg);
		tv = eval_اظہاریہ(arg, تہی);
	    }
	    ورنہ
	    {
		ch_log(channel, "Calling '%s'", (حرف *)arg);
		اگر (func_call(arg, &argv[2], تہی, تہی, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    اگر (argv[id_idx].v_type == VAR_نمبر)
	    {
		صحیح id = argv[id_idx].vval.v_نمبر;

		اگر (tv != تہی)
		    json = json_encode_nr_اظہاریہ(id, tv, options | JSON_NL);
		اگر (tv == تہی || (json != تہی && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_اظہاریہ(id, &err_tv, options | JSON_NL);
		}
		اگر (json != تہی)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_اندر,
				 json, (صحیح)STRLEN(json), (حرف *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    اگر (tv == &res_tv)
		clear_tv(tv);
	    ورنہ
		free_tv(tv);
	}
    }
    ورنہ اگر (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (حرف *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    ساکن خالی
invoke_one_وقت_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *شے,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (حرف *)شے->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_گرہ(cbhead, شے);
    invoke_callback(channel, &شے->cq_callback, argv);
    free_callback(&شے->cq_callback);
    vim_free(شے);
}

    ساکن خالی
append_مقصد_بفر(
    buf_T	*بفر,
    char_u	*پیغام,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = بفر->b_ml.ml_سطر_گنتی;
    bool	save_write_مقصد = بفر->b_write_مقصد_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    صحیح		save_p_ma = بفر->b_p_ma;
    صحیح		خالی_خانے_2 = (بفر->b_ml.ml_flags & ML_خالی_خانے) ? 1 : 0;

    اگر (!بفر->b_p_ma && !ch_part->ch_nomodifiable)
    {
	اگر (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	واپس;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    اگر (save_write_مقصد)
    {
	--lnum;
	بفر->b_write_مقصد_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (صحیح)lnum + 1 - خالی_خانے_2, بفر->b_fname);

    بفر->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, بفر);
    اگر (curbuf != بفر)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	واپس;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - خالی_خانے_2, lnum + 1);

    اگر (خالی_خانے_2)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, پیغام, TRUE);
	lnum = 0;
    }
    ورنہ
	ml_append(lnum, پیغام, 0, FALSE);
    appended_سطریں_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    اگر (ch_part->ch_nomodifiable)
	بفر->b_p_ma = FALSE;
    ورنہ
	بفر->b_p_ma = save_p_ma;

    اگر (بفر->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    اگر (wp->w_بفر == بفر)
	    {
		صحیح move_cursor = save_write_مقصد
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.ستون == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		اگر (move_cursor || wp->w_topline > بفر->b_ml.ml_سطر_گنتی)
		{
		    win_T *save_curwin = curwin;

		    اگر (move_cursor)
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

    اگر (save_write_مقصد)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	بفر->b_write_مقصد_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *اندر_part = &ch->ch_part[PART_اندر];

	    اگر (اندر_part->ch_bufref.br_buf == بفر)
		اندر_part->ch_buf_bot = بفر->b_ml.ml_سطر_گنتی;
	}
    }
}

    ساکن خالی
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *پیغام;

    جب ((پیغام = channel_get(channel, part, تہی)) != تہی)
    {
	ch_log(channel, "Dropping message '%s'", (حرف *)پیغام);
	vim_free(پیغام);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ساکن صحیح
channel_use_json_سر(channel_T *channel, ch_part_T part)
{
    ch_منوال_T	ch_منوال = channel->ch_part[part].ch_منوال;

    واپس ch_منوال == CH_منوال_JSON || ch_منوال == CH_منوال_JS
						     || ch_منوال == CH_منوال_LSP
						     || ch_منوال == CH_منوال_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ساکن صحیح
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*پیغام = تہی;
    blob_T	*blob = تہی;
    typval_T	*listtv = تہی;
    typval_T	argv[CH_JSON_MAX_دلائل];
    صحیح		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_منوال_T	ch_منوال = ch_part->ch_منوال;
    cbq_T	*cbhead = &ch_part->ch_cb_سر;
    cbq_T	*cbitem;
    callback_T	*callback = تہی;
    buf_T	*بفر = تہی;
    char_u	*p;
    صحیح		called_otc;		// one time callbackup
    صحیح		raw_len = 0;

    اگر (channel->ch_nb_close_cb != تہی
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#اگر_ختم
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	واپس FALSE;

    // Use a message-specific callback, part callback or channel callback
    برائے (cbitem = cbhead->cq_اگلا; cbitem != تہی; cbitem = cbitem->cq_اگلا)
	اگر (cbitem->cq_seq_nr == 0)
	    توڑو;
    اگر (cbitem != تہی)
	callback = &cbitem->cq_callback;
    ورنہ اگر (ch_part->ch_callback.cb_name != تہی)
	callback = &ch_part->ch_callback;
    ورنہ اگر (channel->ch_callback.cb_name != تہی)
	callback = &channel->ch_callback;

    بفر = ch_part->ch_bufref.br_buf;
    اگر (بفر != تہی && (!bufref_valid(&ch_part->ch_bufref)
					       || بفر->b_ml.ml_mfp == تہی))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_نام[part]);
	ch_part->ch_bufref.br_buf = تہی;
	بفر = تہی;
    }

    اگر (channel_use_json_سر(channel, part))
    {
	listitem_T	*شے;
	صحیح		argc = 0;

	// Get any json message in the queue.
	اگر (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    اگر (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(خالی)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    اگر (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		واپس FALSE;
	}

	اگر (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    اگر (d != تہی)
	    {
		اگر (ch_منوال == CH_منوال_LSP)
		    di = dict_ڈھونڈو(d, (char_u *)"id", -1);
		ورنہ
		    di = dict_ڈھونڈو(d, (char_u *)"request_seq", -1);
		اگر (di != تہی && di->di_tv.v_type == VAR_نمبر)
		    seq_nr = di->di_tv.vval.v_نمبر;
	    }

	    argv[1] = *listtv;
	}
	ورنہ
	{
	    برائے (شے = listtv->vval.v_list->lv_پہلا;
		    شے != تہی && argc < CH_JSON_MAX_دلائل;
		    شے = شے->li_اگلا)
		argv[argc++] = شے->li_tv;
	    جب (argc < CH_JSON_MAX_دلائل)
		argv[argc++].v_type = VAR_UNKNOWN;

	    اگر (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		واپس TRUE;
	    }

	    اگر (argv[0].v_type != VAR_نمبر)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		واپس FALSE;
	    }
	    seq_nr = argv[0].vval.v_نمبر;
	}
    }
    ورنہ اگر (channel_peek(channel, part) == تہی)
    {
	// nothing to read on RAW or NL channel
	واپس FALSE;
    }
    ورنہ
    {
	// If there is no callback or buffer drop the message.
	اگر (callback == تہی && بفر == تہی)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    اگر (channel->ch_close_cb.cb_name == تہی && !channel->ch_drop_never)
		drop_messages(channel, part);
	    واپس FALSE;
	}

	اگر (ch_منوال == CH_منوال_NL)
	{
	    char_u  *nl = تہی;
	    char_u  *buf;
	    readq_T *گرہ;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    جب (TRUE)
	    {
		گرہ = channel_peek(channel, part);
		nl = channel_پہلا_nl(گرہ);
		اگر (nl != تہی)
		    توڑو;
		اگر (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    اگر (ch_part->ch_fd == INVALID_FD && گرہ->rq_buflen > 0)
			توڑو;
		    واپس FALSE; // incomplete message
		}
	    }
	    buf = گرہ->rq_بفر;

	    // Convert NUL to NL, the internal representation.
	    برائے (p = buf; (nl == تہی || p < nl)
					    && p < buf + گرہ->rq_buflen; ++p)
		اگر (*p == NUL)
		    *p = NL;

	    اگر (nl == تہی)
	    {
		// get the whole buffer, drop the NL
		پیغام = channel_get(channel, part, تہی);
	    }
	    ورنہ اگر (nl + 1 == buf + گرہ->rq_buflen)
	    {
		// get the whole buffer
		پیغام = channel_get(channel, part, تہی);
		*nl = NUL;
	    }
	    ورنہ
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		پیغام = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (صحیح)(nl - buf) + 1);
	    }
	}
	ورنہ
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    پیغام = channel_get_all(channel, part,
			ch_منوال == CH_منوال_BLOB ? &raw_len : تہی);
	}

	اگر (پیغام == تہی)
	    واپس FALSE; // out of memory (and avoids Coverity warning)

	اگر (ch_منوال == CH_منوال_BLOB)
	{
	    blob = blob_alloc();
	    اگر (blob == تہی)
	    {
		vim_free(پیغام);
		واپس FALSE;
	    }
	    اگر (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(پیغام);
		واپس FALSE;
	    }
	    اگر (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_ڈیٹا, پیغام, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	ورنہ
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = پیغام;
	}
    }

    called_otc = FALSE;
    اگر (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	صحیح lsp_req_پیغام = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	اگر ((ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_کلید(argv[1].vval.v_dict, "method"))
	    lsp_req_پیغام = TRUE;

	اگر (!lsp_req_پیغام)
	{
	    برائے (cbitem = cbhead->cq_اگلا; cbitem != تہی;
		    cbitem = cbitem->cq_اگلا)
	    {
		اگر (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_وقت_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    توڑو;
		}
	    }
	}
    }

    اگر (seq_nr > 0 && ((ch_منوال != CH_منوال_LSP && ch_منوال != CH_منوال_DAP)
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
		channel_ڈالو_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_نمبر;
		free_tv(listtv);
		listtv = تہی;
	    }
	    ورنہ
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    ورنہ اگر (callback != تہی || بفر != تہی)
    {
	اگر (بفر != تہی)
	{
	    اگر (پیغام == تہی)
		// JSON or JS mode: re-encode the message.
		پیغام = json_encode(listtv, ch_منوال);
	    اگر (پیغام != تہی)
	    {
#اگر_تعریف_شدہ FEAT_TERMINAL
		اگر (بفر->b_رکن != تہی)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    اگر (part == PART_ERR)
		    {
			char_u *cp;
			صحیح    extra = 0;

			برائے (cp = پیغام; *cp != NUL; ++cp)
			    اگر (*cp == NL && (cp == پیغام || cp[-1] != CAR))
				++extra;
			اگر (extra > 0)
			{
			    char_u *crlf_پیغام = alloc(STRLEN(پیغام) + extra + 1);

			    اگر (crlf_پیغام != تہی)
			    {
				char_u *q = crlf_پیغام;

				برائے (cp = پیغام; *cp != NUL; ++cp)
				{
				    اگر (*cp == NL && (cp == پیغام || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_مقصد_رکن(بفر, crlf_پیغام, channel);
				vim_free(crlf_پیغام);
			    }
			}
			ورنہ
			    write_مقصد_رکن(بفر, پیغام, channel);
		    }
		    ورنہ
			write_مقصد_رکن(بفر, پیغام, channel);
		}
		ورنہ
#اگر_ختم
		    append_مقصد_بفر(بفر, پیغام, channel, part);
	    }
	}

	اگر (callback != تہی)
	{
	    اگر (cbitem != تہی)
		invoke_one_وقت_callback(channel, cbhead, cbitem, argv);
	    ورنہ
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (حرف *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    ورنہ
	ch_log(channel, "Dropping message %d", seq_nr);

    اگر (listtv != تہی)
	free_tv(listtv);
    اگر (blob != تہی)
	blob_unref(blob);
    vim_free(پیغام);

    واپس TRUE;
}

#اگر defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    صحیح
channel_can_write_مقصد(channel_T *channel)
{
    واپس channel != تہی && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_اندر_FD != INVALID_FD);
}
#اگر_ختم

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    صحیح
channel_is_open(channel_T *channel)
{
    واپس channel != تہی && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_اندر_FD != INVALID_FD
			  || channel->CH_برآمد_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ساکن خالی *
channel_readahead_اشارہ_گر(channel_T *channel, ch_part_T part)
{
    اگر (channel_use_json_سر(channel, part))
    {
	jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;

	اگر (سر->jq_اگلا == تہی)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	واپس سر->jq_اگلا;
    }
    واپس channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ساکن صحیح
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    واپس channel_readahead_اشارہ_گر(channel, part) != تہی;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ساکن حرف *
channel_حالت(channel_T *channel, صحیح req_part)
{
    ch_part_T part;
    صحیح has_readahead = FALSE;

    اگر (channel == تہی)
	 واپس "fail";
    اگر (req_part == PART_برآمد)
    {
	اگر (channel->CH_برآمد_FD != INVALID_FD)
	    واپس "open";
	اگر (channel_has_readahead(channel, PART_برآمد))
	    has_readahead = TRUE;
    }
    ورنہ اگر (req_part == PART_ERR)
    {
	اگر (channel->CH_ERR_FD != INVALID_FD)
	    واپس "open";
	اگر (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    ورنہ
    {
	اگر (channel_is_open(channel))
	    واپس "open";
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
	    اگر (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		توڑو;
	    }
    }

    اگر (has_readahead)
	واپس "buffered";
    واپس "closed";
}

    ساکن خالی
channel_part_info(channel_T *channel, dict_T *dict, حرف *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    حرف	namebuf[20];  // longest is "sock_timeout"
    size_t	دم;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    دم = STRLEN(namebuf);
    STRCPY(namebuf + دم, "_");
    دم += STRLEN_LITERAL("_");

    STRCPY(namebuf + دم, "status");
    اگر (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_سیٹ(s, "open");
    ورنہ اگر (channel_has_readahead(channel, part))
	STR_LITERAL_سیٹ(s, "buffered");
    ورنہ
	STR_LITERAL_سیٹ(s, "closed");
    dict_جوڑو_string_len(dict, namebuf, s.string, (صحیح)s.لمبائی);

    STRCPY(namebuf + دم, "mode");
    انتخاب (chanpart->ch_منوال)
    {
	حالت CH_منوال_NL:
	    STR_LITERAL_سیٹ(s, "NL");
	    توڑو;
	حالت CH_منوال_RAW:
	    STR_LITERAL_سیٹ(s, "RAW");
	    توڑو;
	حالت CH_منوال_BLOB:
	    STR_LITERAL_سیٹ(s, "BLOB");
	    توڑو;
	حالت CH_منوال_JSON:
	    STR_LITERAL_سیٹ(s, "JSON");
	    توڑو;
	حالت CH_منوال_JS:
	    STR_LITERAL_سیٹ(s, "JS");
	    توڑو;
	حالت CH_منوال_LSP:
	    STR_LITERAL_سیٹ(s, "LSP");
	    توڑو;
	حالت CH_منوال_DAP:
	    STR_LITERAL_سیٹ(s, "DAP");
	    توڑو;
	طےشدہ:
	    STR_LITERAL_سیٹ(s, "");
	    توڑو;
    }
    dict_جوڑو_string_len(dict, namebuf, s.string, (صحیح)s.لمبائی);

    STRCPY(namebuf + دم, "io");
    اگر (part == PART_SOCK)
	STR_LITERAL_سیٹ(s, "socket");
    ورنہ انتخاب (chanpart->ch_io)
    {
	حالت JIO_NULL:
	    STR_LITERAL_سیٹ(s, "null");
	    توڑو;
	حالت JIO_PIPE:
	    STR_LITERAL_سیٹ(s, "pipe");
	    توڑو;
	حالت JIO_FILE:
	    STR_LITERAL_سیٹ(s, "file");
	    توڑو;
	حالت JIO_بفر:
	    STR_LITERAL_سیٹ(s, "buffer");
	    توڑو;
	حالت JIO_برآمد:
	    STR_LITERAL_سیٹ(s, "out");
	    توڑو;
	طےشدہ:
	    STR_LITERAL_سیٹ(s, "");
	    توڑو;
    }
    dict_جوڑو_string_len(dict, namebuf, s.string, (صحیح)s.لمبائی);

    STRCPY(namebuf + دم, "timeout");
    dict_جوڑو_نمبر(dict, namebuf, chanpart->ch_timeout);
}

    ساکن خالی
channel_info(channel_T *channel, dict_T *dict)
{
    dict_جوڑو_نمبر(dict, "id", channel->ch_id);
    dict_جوڑو_string(dict, "status", (char_u *)channel_حالت(channel, -1));

    اگر (channel->ch_hostname != تہی)
    {
	اگر (channel->ch_پورٹ)
	    dict_جوڑو_نمبر(dict, "port", channel->ch_پورٹ);
	ورنہ
	    // Unix-domain socket.
	    dict_جوڑو_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    ورنہ
    {
	channel_part_info(channel, dict, "out", PART_برآمد);
	channel_part_info(channel, dict, "err", PART_ERR);
	channel_part_info(channel, dict, "in", PART_اندر);
    }
}

/*
 * Close channel "channel".
 * Trigger the close callback if "invoke_close_cb" is TRUE.
 * Does not clear the buffers.
 */
    خالی
channel_close(channel_T *channel, صحیح invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#اگر_تعریف_شدہ FEAT_GUI
    channel_gui_unregister(channel);
#اگر_ختم

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_اندر);
    ch_close_part(channel, PART_برآمد);
    ch_close_part(channel, PART_ERR);

    اگر (invoke_close_cb)
    {
	ch_part_T	part;

#اگر_تعریف_شدہ FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	رکن_channel_closing(channel);
#اگر_ختم
	// Invoke callbacks and flush buffers before the close callback.
	اگر (channel->ch_close_cb.cb_name != تہی)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
	{
	    اگر (channel->ch_close_cb.cb_name != تہی
			    || channel->ch_part[part].ch_bufref.br_buf != تہی)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		اگر (channel->ch_close_cb.cb_name == تہی)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_نام[part]);
		جب (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	اگر (channel->ch_close_cb.cb_name != تہی)
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

	      اگر (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      اگر (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  برائے (part = PART_SOCK; part < PART_اندر; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = تہی;
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = تہی;
    channel->ch_ss_close_cb = تہی;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_TERMINAL
    رکن_channel_closed(channel);
#اگر_ختم
}

/*
 * Close the "in" part channel "channel".
 */
    ساکن خالی
channel_close_اندر(channel_T *channel)
{
    ch_close_part(channel, PART_اندر);
}

    ساکن خالی
remove_from_writeque(writeq_T *wq, writeq_T *اندراج_2)
{
    ga_clear(&اندراج_2->wq_ga);
    wq->wq_اگلا = اندراج_2->wq_اگلا;
    اگر (wq->wq_اگلا == تہی)
	wq->wq_پچھلا = تہی;
    ورنہ
	wq->wq_اگلا->wq_پچھلا = تہی;
    vim_free(اندراج_2);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ساکن خالی
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_سر = &ch_part->ch_json_سر;
    cbq_T   *cb_سر = &ch_part->ch_cb_سر;

    جب (channel_peek(channel, part) != تہی)
	vim_free(channel_get(channel, part, تہی));

    جب (cb_سر->cq_اگلا != تہی)
    {
	cbq_T *گرہ = cb_سر->cq_اگلا;

	remove_cb_گرہ(cb_سر, گرہ);
	free_callback(&گرہ->cq_callback);
	vim_free(گرہ);
    }

    جب (json_سر->jq_اگلا != تہی)
    {
	free_tv(json_سر->jq_اگلا->jq_قدر);
	remove_json_گرہ(json_سر, json_سر->jq_اگلا);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_بلاک_ids);

    جب (ch_part->ch_writeque.wq_اگلا != تہی)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_اگلا);
}

/*
 * Clear all the read buffers on "channel".
 */
    خالی
channel_clear(channel_T *channel)
{
    ch_log(channel, "Clearing channel");
    VIM_CLEAR(channel->ch_hostname);
    channel_clear_one(channel, PART_SOCK);
    channel_clear_one(channel, PART_برآمد);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_اندر);
    free_callback(&channel->ch_callback);
    free_callback(&channel->ch_close_cb);
}

#اگر defined(EXITFREE)
    خالی
channel_free_all(خالی)
{
    channel_T *channel;

    ch_log(تہی, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#اگر_ختم


// Sent when the netbeans channel is found closed when reading.
#تعریف_کرو DETACH_پیغام_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#تعریف_کرو MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ساکن صحیح
is_channel_write_بقایا(chanpart_T *اندر_part)
{
    buf_T *buf = اندر_part->ch_bufref.br_buf;

    اگر (اندر_part->ch_writeque.wq_اگلا != تہی)
	واپس TRUE;
    اگر (buf == تہی)
	واپس FALSE;
    واپس اندر_part->ch_buf_append
	    ? (اندر_part->ch_buf_bot < buf->b_ml.ml_سطر_گنتی)
	    : (اندر_part->ch_buf_چوٹی <= اندر_part->ch_buf_bot
			    && اندر_part->ch_buf_چوٹی <= buf->b_ml.ml_سطر_گنتی);
}

#اگر defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ساکن صحیح
channel_fill_wfds(صحیح maxfd_arg, fd_set *wfds)
{
    صحیح		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *اندر_part = &ch->ch_part[PART_اندر];

	اگر (اندر_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (صحیح)اندر_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_بقایا(اندر_part))
	{
	    FD_SET((صحیح)اندر_part->ch_fd, wfds);
	    اگر ((صحیح)اندر_part->ch_fd >= maxfd)
		maxfd = (صحیح)اندر_part->ch_fd + 1;
	}
    }
    واپس maxfd;
}
#ورنہ
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ساکن صحیح
channel_fill_poll_write(صحیح nfd_اندر, ساخت pollfd *fds)
{
    صحیح		nfd = nfd_اندر;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *اندر_part = &ch->ch_part[PART_اندر];

	اگر (اندر_part->ch_fd != INVALID_FD
		&& is_channel_write_بقایا(اندر_part))
	{
	    اندر_part->ch_poll_idx = nfd;
	    fds[nfd].fd = اندر_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	ورنہ
	    اندر_part->ch_poll_idx = -1;
    }
    واپس nfd;
}
#اگر_ختم

قسمتعریف شمار {
    CW_READY,
    CW_NOT_READY,
    CW_ERROR
} channel_wait_نتیجہ;

/*
 * Check for reading from "fd" with "timeout" msec.
 * Return CW_READY when there is something to read.
 * Return CW_NOT_READY when there is nothing to read.
 * Return CW_ERROR when there is an error.
 */
    ساکن channel_wait_نتیجہ
channel_wait(channel_T *channel, sock_T fd, صحیح timeout)
{
    اگر (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#اگر_تعریف_شدہ MSWIN
    اگر (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	صحیح	sleep_وقت;
	DWORD	deadline = Get_Tick_گنتی() + timeout;
	صحیح	delay = 1;

	// reading from a pipe, not a socket
	جب (TRUE)
	{
	    صحیح r = PeekNamedPipe((HANDLE)fd, تہی, 0, تہی, &nread, تہی);

	    اگر (r && nread > 0)
		واپس CW_READY;

	    اگر (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, تہی);
	    }
	    ورنہ اگر (r == 0)
		واپس CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_سطریں();

	    sleep_وقت = deadline - Get_Tick_گنتی();
	    اگر (sleep_وقت <= 0)
		توڑو;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    اگر (sleep_وقت > delay)
		sleep_وقت = delay;
	    Sleep(sleep_وقت);
	    delay = delay * 2;
	    اگر (delay > 10)
		delay = 10;
	}
    }
    ورنہ
#اگر_ختم
    {
#اگر defined(HAVE_SELECT)
	ساخت timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	صحیح		ret;
	صحیح		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	برائے (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((صحیح)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (صحیح)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, تہی, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    اگر (ret == -1 && errno == EINTR)
		جاری;
# endif
	    اگر (ret > 0)
	    {
		اگر (FD_ISSET(fd, &rfds))
		    واپس CW_READY;
		channel_write_any_سطریں();
		جاری;
	    }
	    توڑو;
	}
#ورنہ
	برائے (;;)
	{
	    ساخت pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    صحیح		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    اگر (poll(fds, nfd, timeout) > 0)
	    {
		اگر (fds[0].revents & POLLIN)
		    واپس CW_READY;
		channel_write_any_سطریں();
		جاری;
	    }
	    توڑو;
	}
#اگر_ختم
    }
    واپس CW_NOT_READY;
}

    ساکن خالی
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, صحیح is_err, حرف *func)
{
    حرف	پیغام[] = "%s(): Read %s from ch_part[%d], closing";

    اگر (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, پیغام, func, "error", part);
    ورنہ
	ch_log(channel, پیغام, func, "EOF", part);

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
    اگر (channel->ch_nb_close_cb != تہی)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_پیغام_RAW,
			      (صحیح)STRLEN_LITERAL(DETACH_پیغام_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    اگر (part == PART_برآمد || part == PART_ERR)
    {
	ch_part_T other = part == PART_برآمد ? PART_ERR : PART_برآمد;

	اگر (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#اگر_تعریف_شدہ FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#اگر_ختم
}

    ساکن خالی
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    اگر (channel->ch_nb_close_cb != تہی)
	(*channel->ch_nb_close_cb)();
#اگر_تعریف_شدہ FEAT_SOCKETSERVER
    اگر (channel->ch_ss_close_cb != تہی)
	channel->ch_ss_close_cb(channel);
#اگر_ختم
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    ساکن خالی
channel_read(channel_T *channel, ch_part_T part, حرف *func)
{
    ساکن char_u	*buf = تہی;
    صحیح			len = 0;
    صحیح			readlen = 0;
    sock_T		fd;
    صحیح			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    اگر (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_نام[part]);
	واپس;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    اگر (buf == تہی)
    {
	buf = alloc(MAXMSGSIZE);
	اگر (buf == تہی)
	    واپس;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    برائے (;;)
    {
	اگر (channel_wait(channel, fd, 0) != CW_READY)
	    توڑو;
	اگر (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    ساخت sockaddr_storage	client;

	    newchannel = جوڑو_channel();
	    اگر (newchannel == تہی)
	    {
		ch_error(تہی, "Cannot allocate channel.");
		واپس;
	    }
	    socklen = حجم(client);
	    newfd = accept(fd, (ساخت sockaddr*)&client, &socklen);
	    اگر (newfd < 0)
	    {
		ch_error(تہی, "Cannot accept channel.");
		channel_free(newchannel);
		واپس;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_مقصد_be_closed |= (1U << PART_SOCK);

#اگر_تعریف_شدہ FEAT_GUI
	    channel_gui_رجسٹر_one(newchannel, PART_SOCK);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SOCKETSERVER
	    اگر (channel->ch_ss_accept_cb != تہی)
	    {
		channel->ch_ss_accept_cb(newchannel);
		واپس;
	    }
#اگر_ختم

	    اگر (client.ss_family == AF_INET)
	    {
#اگر_تعریف_شدہ HAVE_INET_NTOP
		حرف addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((ساخت sockaddr_اندر*)&client)->sin_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
			addr,
			ntohs(((ساخت sockaddr_اندر*)&client)->sin_پورٹ));
#ورنہ
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
		    inet_ntoa(((ساخت sockaddr_اندر*)&client)->sin_addr),
		    ntohs(((ساخت sockaddr_اندر*)&client)->sin_پورٹ));
#اگر_ختم
	    }
#اگر_تعریف_شدہ HAVE_INET_NTOP
	    ورنہ اگر (client.ss_family == AF_INET6)
	    {
		حرف addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((ساخت sockaddr_in6*)&client)->sin6_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "[%s]:%d",
			addr,
			ntohs(((ساخت sockaddr_in6*)&client)->sin6_پورٹ));
	    }
#اگر_ختم
	    ورنہ اگر (client.ss_family == AF_UNIX)
		vim_snprintf((حرف *)namebuf, حجم(namebuf),
							    "unix:anonymous");
	    ورنہ
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "unknown");
	    ++محفوظ_مقصد_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --محفوظ_مقصد_invoke_callback;
	    clear_tv(&argv[1]);
	    واپس;
	}
	اگر (use_socket)
	    len = sock_read(fd, (حرف *)buf, MAXMSGSIZE);
	ورنہ
	    len = fd_read(fd, (حرف *)buf, MAXMSGSIZE);
	اگر (len <= 0)
	    توڑو;	// error or nothing more to read

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
    ورنہ اگر (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#اگر_ختم
}

#اگر_تعریف_شدہ FEAT_SOCKETSERVER

    خالی
channel_check(channel_T *channel, ch_part_T part)
{
    channel_read(channel, part, "channel_check");
}

#اگر_ختم

/*
 * Read from RAW or NL "channel"/"part".  Blocks until there is something to
 * read or the timeout expires.
 * When "raw" is TRUE don't block waiting on a NL.
 * Does not trigger timers or handle messages.
 * Returns what was read in allocated memory.
 * Returns NULL in case of error or timeout.
 */
    ساکن char_u *
channel_read_بلاک(
	channel_T *channel, ch_part_T part, صحیح timeout, صحیح raw, صحیح *outlen)
{
    char_u	*buf;
    char_u	*پیغام;
    ch_منوال_T	منوال = channel->ch_part[part].ch_منوال;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*گرہ;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  منوال == CH_منوال_RAW ? "RAW"
				: منوال == CH_منوال_BLOB ? "BLOB" : "NL", timeout);

    جب (TRUE)
    {
	گرہ = channel_peek(channel, part);
	اگر (گرہ != تہی)
	{
	    اگر (منوال == CH_منوال_RAW || منوال == CH_منوال_BLOB
		    || (منوال == CH_منوال_NL
					   && channel_پہلا_nl(گرہ) != تہی))
		// got a complete message
		توڑو;
	    اگر (channel_collapse(channel, part, منوال == CH_منوال_NL) == OK)
		جاری;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    اگر (raw || fd == INVALID_FD)
		توڑو;
	}

	// Wait for up to the channel timeout.
	اگر (fd == INVALID_FD)
	    واپس تہی;
	اگر (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    واپس تہی;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    اگر (منوال == CH_منوال_RAW || منوال == CH_منوال_BLOB || outlen != تہی)
    {
	پیغام = channel_get_all(channel, part, outlen);
    }
    ورنہ
    {
	char_u *p;

	buf = گرہ->rq_بفر;
	nl = channel_پہلا_nl(گرہ);

	// Convert NUL to NL, the internal representation.
	برائے (p = buf; (nl == تہی || p < nl) && p < buf + گرہ->rq_buflen; ++p)
	    اگر (*p == NUL)
		*p = NL;

	اگر (nl == تہی)
	{
	    // must be a closed channel with missing NL
	    پیغام = channel_get(channel, part, تہی);
	}
	ورنہ اگر (nl + 1 == buf + گرہ->rq_buflen)
	{
	    // get the whole buffer
	    پیغام = channel_get(channel, part, تہی);
	    *nl = NUL;
	}
	ورنہ
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    پیغام = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (صحیح)(nl - buf) + 1);
	}
    }
    اگر (ch_log_فعال())
	ch_log(channel, "Returning %d bytes",
		outlen != تہی ? *outlen : (صحیح)STRLEN(پیغام));
    واپس پیغام;
}

ساکن صحیح channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    صحیح
channel_اندر_blocking_wait(خالی)
{
    واپس channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ساکن خالی
channel_wait_reading(channel_T *channel غیر_مستعمل, sock_T fd غیر_مستعمل, صحیح timeout)
{
#اگر_تعریف_شدہ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (خالی)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#ورنہ
    channel_wait_any(timeout);
#اگر_ختم
}

/*
 * Read one JSON message with ID "id" from "channel"/"part" and store the
 * result in "rettv".
 * When "id" is -1 accept any message;
 * Blocks until the message is received or the timeout is reached.
 * In corner cases this can be called recursively, that is why ch_block_ids is
 * a list.
 */
    ساکن صحیح
channel_read_json_بلاک(
	channel_T   *channel,
	ch_part_T   part,
	صحیح	    timeout_arg,
	صحیح	    id,
	typval_T    **rettv)
{
    صحیح		مزید;
    sock_T	fd;
    صحیح		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_منوال_T	منوال = channel->ch_part[part].ch_منوال;
    صحیح		retval = FAIL;
#اگر_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	آغاز_tv;

    ELAPSED_INIT(آغاز_tv);
#اگر_ختم

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    اگر (id >= 0)
	channel_جوڑو_بلاک_id(chanpart, id);

    برائے (;;)
    {
	اگر (منوال == CH_منوال_LSP || منوال == CH_منوال_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (خالی)channel_collapse(channel, part, FALSE);

	مزید = channel_parse_json(channel, part, false);

	// search for message "id"
	اگر (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    توڑو;
	}

	اگر (!مزید)
	{
	    خالی *پچھلا_readahead_ptr = channel_readahead_اشارہ_گر(channel, part);
	    خالی *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    اگر (channel_parse_messages())
		جاری;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_اشارہ_گر(channel, part);
	    اگر (readahead_ptr != تہی && readahead_ptr != پچھلا_readahead_ptr)
		جاری;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#اگر_تعریف_شدہ ELAPSED_FUNC
	    timeout -= (صحیح)ELAPSED_FUNC(آغاز_tv);
#اگر_ختم
	    fd = chanpart->ch_fd;
	    اگر (timeout <= 0 || fd == INVALID_FD)
	    {
		اگر (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		توڑو;
	    }
	    // If there was an incomplete message use the deadline for that.
	    اگر (chanpart->ch_wait_len > 0)
	    {
		صحیح	بایاں;
#اگر_تعریف_شدہ MSWIN
		بایاں = chanpart->ch_deadline - Get_Tick_گنتی() + 1;
#ورنہ
		{
		    ساخت timeval now_tv;

		    gettimeofday(&now_tv, تہی);
		    بایاں = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#اگر_ختم
		اگر (بایاں < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		ورنہ اگر (بایاں < timeout)
		    timeout = بایاں;
	    }
	    channel_wait_reading(channel, fd, timeout);
#اگر_غیر_تعریف_شدہ ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#اگر_ختم
	}
    }
    اگر (id >= 0)
	channel_remove_بلاک_id(chanpart, id);
    --channel_blocking_wait;

    واپس retval;
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
    channel_T	*channel = تہی;
    صحیح		has_readahead = FALSE;

    اگر (tv->v_type == VAR_کام)
    {
	اگر (tv->vval.v_کام != تہی)
	    channel = tv->vval.v_کام->jv_channel;
    }
    ورنہ اگر (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    ورنہ
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(tv));
	واپس تہی;
    }
    اگر (channel != تہی && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_گنتی ? part : channel_part_read(channel));

    اگر (check_open && (channel == تہی || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	واپس تہی;
    }
    واپس channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ساکن خالی
common_channel_read(typval_T *argvars, typval_T *rettv, صحیح raw, صحیح blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_گنتی;
    jobopt_T	opt;
    صحیح		منوال;
    صحیح		timeout;
    صحیح		id = -1;
    typval_T	*listtv = تہی;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_chan_or_کام_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    clear_کام_options(&opt);
    اگر (get_کام_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	جاؤ theend;

    اگر (opt.jo_سیٹ & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    اگر (channel == تہی)
	جاؤ theend;

    اگر (part == PART_گنتی)
	part = channel_part_read(channel);
    منوال = channel_get_منوال(channel, part);
    timeout = channel_get_timeout(channel, part);
    اگر (opt.jo_سیٹ & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    اگر (blob || منوال == CH_منوال_BLOB)
    {
	صحیح	    outlen = 0;
	char_u  *p = channel_read_بلاک(channel, part,
		timeout, TRUE, &outlen);
	اگر (p != تہی)
	{
	    blob_T	*b = blob_alloc();

	    اگر (b != تہی)
	    {
		b->bv_ga.ga_len = outlen;
		اگر (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		ورنہ
		{
		    حافظہ_نقل(b->bv_ga.ga_ڈیٹا, p, outlen);
		    rettv_blob_سیٹ(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    ورنہ اگر (raw || منوال == CH_منوال_RAW || منوال == CH_منوال_NL)
	rettv->vval.v_string = channel_read_بلاک(channel, part,
		timeout, raw, تہی);
    ورنہ
    {
	اگر (opt.jo_سیٹ & JO_ID)
	    id = opt.jo_id;
	channel_read_json_بلاک(channel, part, timeout, id, &listtv);
	اگر (listtv != تہی)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	ورنہ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_نمبر = VVAL_NONE;
	}
    }

theend:
    free_کام_options(&opt);
}

#اگر defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    خالی
channel_handle_events(صحیح only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	اگر (only_keep_open && !channel->ch_keep_open)
	    جاری;

	// check the socket and pipes
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    اگر (fd == INVALID_FD)
		جاری;

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
			(part == PART_برآمد || part == PART_ERR)) ? 1 : 0);

	    اگر (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    ورنہ اگر (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	اگر (channel->ch_کام != تہی)
	{
	    کام_T *کام = channel->ch_کام;

	    اگر (کام->jv_tty_برآمد != تہی && کام->jv_حالت == کام_ختم_شدہ)
		برائے (part = PART_SOCK; part < PART_گنتی; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#اگر_ختم

#اگر defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    صحیح
channel_any_keep_open(خالی)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	اگر (channel->ch_keep_open)
	    واپس TRUE;
    واپس FALSE;
}
#اگر_ختم

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    خالی
channel_سیٹ_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    صحیح		fd = ch_part->ch_fd;

    اگر (fd == INVALID_FD)
	واپس;

#اگر_تعریف_شدہ MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#ورنہ
    (خالی)fcntl(fd, F_SETFL, O_NONBLOCK);
#اگر_ختم
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
	حرف	  *fun)
{
    صحیح		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    صحیح		did_use_قطار = FALSE;

    fd = ch_part->ch_fd;
    اگر (fd == INVALID_FD)
    {
	اگر (!channel->ch_error && fun != تہی)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	واپس FAIL;
    }

    اگر (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_سیٹ_nonblock(channel, part);

    اگر (ch_log_فعال())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_پیغام = 0;
    }

    برائے (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	صحیح	    len;

	اگر (wq->wq_اگلا != تہی)
	{
	    // first write what was queued
	    buf = wq->wq_اگلا->wq_ga.ga_ڈیٹا;
	    len = wq->wq_اگلا->wq_ga.ga_len;
	    did_use_قطار = TRUE;
	}
	ورنہ
	{
	    اگر (len_arg == 0)
		// nothing to write, called from channel_select_check()
		واپس OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	اگر (part == PART_SOCK)
	    res = sock_write(fd, (حرف *)buf, len);
	ورنہ
	{
	    res = fd_write(fd, (حرف *)buf, len);
#اگر_تعریف_شدہ MSWIN
	    اگر (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, تہی);
	    }
#اگر_ختم
	}
	اگر (res < 0 && (errno == EWOULDBLOCK
#اگر_تعریف_شدہ EAGAIN
			|| errno == EAGAIN
#اگر_ختم
		    ))
	    res = 0; // nothing got written

	اگر (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *اندراج_2 = wq->wq_اگلا;

	    اگر (did_use_قطار)
		ch_log(channel, "Sent %d bytes now", res);
	    اگر (res == len)
	    {
		// Wrote all the buf[len] bytes.
		اگر (اندراج_2 != تہی)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, اندراج_2);
		    جاری;
		}
		اگر (did_use_قطار)
		    ch_log(channel, "Write queue empty");
	    }
	    ورنہ
	    {
		// Wrote only buf[res] bytes, can't write more now.
		اگر (اندراج_2 != تہی)
		{
		    اگر (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(اندراج_2->wq_ga.ga_ڈیٹا,
				    (حرف *)اندراج_2->wq_ga.ga_ڈیٹا + res,
				    len - res);
			اندراج_2->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		ورنہ
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		اگر (wq->wq_پچھلا != تہی
			&& wq->wq_پچھلا->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *آخری = wq->wq_پچھلا;

		    // append to the last entry
		    اگر (len > 0 && ga_grow(&آخری->wq_ga, len) == OK)
		    {
			mch_memmove((حرف *)آخری->wq_ga.ga_ڈیٹا
							  + آخری->wq_ga.ga_len,
				    buf, len);
			آخری->wq_ga.ga_len += len;
		    }
		}
		ورنہ
		{
		    writeq_T *آخری = ALLOC_ONE(writeq_T);

		    اگر (آخری != تہی)
		    {
			آخری->wq_پچھلا = wq->wq_پچھلا;
			آخری->wq_اگلا = تہی;
			اگر (wq->wq_پچھلا == تہی)
			    wq->wq_اگلا = آخری;
			ورنہ
			    wq->wq_پچھلا->wq_اگلا = آخری;
			wq->wq_پچھلا = آخری;
			ga_init2(&آخری->wq_ga, 1, 1000);
			اگر (len > 0 && ga_grow(&آخری->wq_ga, len) == OK)
			{
			    mch_memmove(آخری->wq_ga.ga_ڈیٹا, buf, len);
			    آخری->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	ورنہ اگر (res != len)
	{
	    اگر (!channel->ch_error && fun != تہی)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    واپس FAIL;
	}

	channel->ch_error = FALSE;
	واپس OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    ساکن channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *متن,
	صحیح	    len,
	صحیح	    id,
	صحیح	    eval,
	jobopt_T    *opt,
	حرف	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_کام_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel == تہی)
	واپس تہی;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    اگر (get_کام_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	واپس تہی;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    اگر (opt->jo_callback.cb_name != تہی && *opt->jo_callback.cb_name != NUL)
    {
	اگر (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    واپس تہی;
	}
	channel_سیٹ_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    اگر (channel_send(channel, part_send, متن, len, fun) == OK
					   && opt->jo_callback.cb_name == تہی)
	واپس channel;
    واپس تہی;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ساکن خالی
ch_اظہاریہ_common(typval_T *argvars, typval_T *rettv, صحیح eval)
{
    char_u	*متن;
    typval_T	*listtv;
    channel_T	*channel;
    صحیح		id;
    ch_منوال_T	ch_منوال;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحیح		timeout;
    صحیح		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_chan_or_کام_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel == تہی)
	واپس;
    part_send = channel_part_send(channel);

    ch_منوال = channel_get_منوال(channel, part_send);
    اگر (ch_منوال == CH_منوال_RAW || ch_منوال == CH_منوال_BLOB
	    || ch_منوال == CH_منوال_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	واپس;
    }

    اگر (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	حرف		*کلید = ch_منوال == CH_منوال_LSP ? "id" : "seq";

	// return an empty dict by default
	اگر (rettv_dict_alloc(rettv) == FAIL)
	    واپس;

	اگر (check_for_dict_arg(argvars, 1) == FAIL)
	    واپس;

	d = argvars[1].vval.v_dict;
	di = dict_ڈھونڈو(d, (char_u *)کلید, -1);
	اگر (argvars[2].v_type == VAR_DICT
		&& dict_has_کلید(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	اگر (di != تہی && di->di_tv.v_type != VAR_نمبر
		&& (ch_منوال == CH_منوال_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), کلید);
	    واپس;
	}

	اگر (ch_منوال == CH_منوال_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_آخری_پیغام_id;
	    اگر (di == تہی)
		dict_جوڑو_نمبر(d, "seq", id);
	    ورنہ
		di->di_tv.vval.v_نمبر = id;
	}
	ورنہ اگر (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_آخری_پیغام_id;
	    اگر (di == تہی)
		dict_جوڑو_نمبر(d, "id", id);
	    ورنہ
		di->di_tv.vval.v_نمبر = id;
	}
	ورنہ
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    اگر (di != تہی && di->di_tv.v_type == VAR_نمبر)
		id = di->di_tv.vval.v_نمبر;
	}
	اگر (ch_منوال == CH_منوال_LSP && !dict_has_کلید(d, "jsonrpc"))
	    dict_جوڑو_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	متن = json_encode_lsp_پیغام(&argvars[1]);
    }
    ورنہ
    {
	id = ++channel->ch_آخری_پیغام_id;
	متن = json_encode_nr_اظہاریہ(id, &argvars[1],
			      (ch_منوال == CH_منوال_JS ? JSON_JS : 0) | JSON_NL);
    }
    اگر (متن == تہی)
	واپس;

    channel = send_common(argvars, متن, (صحیح)STRLEN(متن), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(متن);
    اگر (channel != تہی && eval)
    {
	اگر (opt.jo_سیٹ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ورنہ
	    timeout = channel_get_timeout(channel, part_read);
	اگر (channel_read_json_بلاک(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    اگر (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_نمبر;
		free_tv(listtv);
	    }
	    ورنہ
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_آخری->li_tv;
		list->lv_u.mat.lv_آخری->li_tv.v_type = VAR_نمبر;
		free_tv(listtv);
	    }
	}
    }
    free_کام_options(&opt);
    اگر (ch_منوال == CH_منوال_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	اگر (rettv->vval.v_dict != تہی)
	    dict_جوڑو_نمبر(rettv->vval.v_dict, "seq", id);
    }
    ورنہ اگر (ch_منوال == CH_منوال_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	اگر (rettv->vval.v_dict != تہی)
	    dict_جوڑو_نمبر(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ساکن خالی
ch_raw_common(typval_T *argvars, typval_T *rettv, صحیح eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*متن;
    صحیح		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحیح		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_chan_or_کام_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	واپس;

    اگر (argvars[1].v_type == VAR_BLOB)
    {
	متن = argvars[1].vval.v_blob->bv_ga.ga_ڈیٹا;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    ورنہ
    {
	متن = tv_get_string_buf(&argvars[1], buf);
	len = (صحیح)STRLEN(متن);
    }
    channel = send_common(argvars, متن, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    اگر (channel != تہی && eval)
    {
	اگر (opt.jo_سیٹ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ورنہ
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_بلاک(channel, part_read,
							timeout, TRUE, تہی);
    }
    free_کام_options(&opt);
}

#تعریف_کرو KEEP_OPEN_وقت 20  // msec

#اگر defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحیح
channel_poll_تیاری(صحیح nfd_اندر, خالی *fds_اندر, صحیح *towait)
{
    صحیح		nfd = nfd_اندر;
    channel_T	*channel;
    ساخت	pollfd *fds = fds_اندر;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    اگر (ch_part->ch_fd != INVALID_FD)
	    {
		اگر (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    اگر (*towait < 0 || *towait > KEEP_OPEN_وقت)
			*towait = KEEP_OPEN_وقت;
		}
		ورنہ
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    ورنہ
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    واپس nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحیح
channel_poll_check(صحیح ret_اندر, خالی *fds_اندر)
{
    صحیح		ret = ret_اندر;
    channel_T	*channel;
    ساخت	pollfd *fds = fds_اندر;
    ch_part_T	part;
    صحیح		idx;
    chanpart_T	*اندر_part;

    FOR_ALL_CHANNELS(channel)
    {
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    اگر (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    ورنہ اگر (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	اندر_part = &channel->ch_part[PART_اندر];
	idx = اندر_part->ch_poll_idx;
	اگر (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_داخلہ(channel);
	    --ret;
	}
    }

    واپس ret;
}
#اگر_ختم // UNIX && !HAVE_SELECT

#اگر !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحیح
channel_select_تیاری(
	صحیح maxfd_اندر,
	خالی *rfds_اندر,
	خالی *wfds_اندر,
	ساخت timeval *tv,
	ساخت timeval **tvp)
{
    صحیح		maxfd = maxfd_اندر;
    channel_T	*channel;
    fd_set	*rfds = rfds_اندر;
    fd_set	*wfds = wfds_اندر;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    اگر (fd != INVALID_FD)
	    {
		اگر (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    اگر (*tvp == تہی || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_وقت * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_وقت * 1000;
		    }
		}
		ورنہ
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    اگر ((صحیح)fd >= FD_SETSIZE)
			جاری;
# endif
		    FD_SET((صحیح)fd, rfds);
		    اگر (maxfd < (صحیح)fd)
			maxfd = (صحیح)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    واپس maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحیح
channel_select_check(صحیح ret_اندر, خالی *rfds_اندر, خالی *wfds_اندر)
{
    صحیح		ret = ret_اندر;
    channel_T	*channel;
    fd_set	*rfds = rfds_اندر;
    fd_set	*wfds = wfds_اندر;
    ch_part_T	part;
    chanpart_T	*اندر_part;

    FOR_ALL_CHANNELS(channel)
    {
	برائے (part = PART_SOCK; part < PART_اندر; ++part)
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
	    ورنہ اگر (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	اندر_part = &channel->ch_part[PART_اندر];
	اگر (ret > 0 && اندر_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحیح)اندر_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(اندر_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(اندر_part->ch_fd, wfds);
	    channel_write_داخلہ(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	اگر (channel->ch_کام != تہی)
	{
	    کام_T *کام = channel->ch_کام;

	    اگر (کام->jv_tty_برآمد != تہی && کام->jv_حالت == کام_ختم_شدہ)
		برائے (part = PART_SOCK; part < PART_گنتی; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    واپس ret;
}
#اگر_ختم // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ساکن خالی
channel_wait_any(طویل msec)
{
#اگر_تعریف_شدہ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    اگر (stdio_channel->CH_برآمد_FD != INVALID_FD)
	(خالی)channel_wait(stdio_channel, stdio_channel->CH_برآمد_FD,
						(صحیح)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#وگرنہ_اگر defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    ساخت timeval	tv;
    ساخت timeval	*tvp = &tv;
    صحیح			maxfd;
    صحیح			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_تیاری(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, تہی, tvp);
    اگر (ret >= 0)
	(خالی)channel_select_check(ret, &rfds, &wfds);
#ورنہ
    ساخت pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    صحیح			nfd;
    صحیح			towait = (صحیح)msec;
    صحیح			ret;

    nfd = channel_poll_تیاری(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    اگر (ret >= 0)
	(خالی)channel_poll_check(ret, fds);
#اگر_ختم
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    خالی
channel_stdio_loop(خالی)
{
    جب (stdio_channel != تہی && channel_is_open(stdio_channel))
    {
	طویل	msec = 1000;
#اگر_تعریف_شدہ FEAT_TIMERS
	طویل	due_وقت = check_due_timer();

	اگر (due_وقت > 0 && due_وقت < msec)
	    msec = due_وقت;
#اگر_ختم
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
channel_parse_messages(خالی)
{
    channel_T	*channel = پہلا_channel;
    صحیح		ret = FALSE;
    صحیح		r;
    ch_part_T	part = PART_SOCK;
    ساکن صحیح	recursive = 0;
#اگر_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	آغاز_tv;
#اگر_ختم

    // The code below may invoke callbacks, which might call us back.
    // In a recursive call channels will not be closed.
    ++recursive;
    ++محفوظ_مقصد_invoke_callback;

#اگر_تعریف_شدہ ELAPSED_FUNC
    ELAPSED_INIT(آغاز_tv);
#اگر_ختم

    // Only do this message when another message was given, otherwise we get
    // lots of them.
    اگر ((did_repeated_پیغام & REPEATED_پیغام_LOOKING) == 0)
    {
	ch_log(تہی, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_پیغام = REPEATED_پیغام_LOOKING;
    }
    جب (channel != تہی)
    {
	اگر (recursive == 1)
	{
	    اگر (channel_can_close(channel))
	    {
		channel->ch_مقصد_be_closed = (1U << PART_گنتی);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = پہلا_channel;
		جاری;
	    }
	    اگر (channel->ch_مقصد_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		اگر (channel->ch_کام != تہی)
		    channel->ch_کام->jv_channel = تہی;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = پہلا_channel;
		جاری;
	    }
	    اگر (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = پہلا_channel;
		part = PART_SOCK;
		جاری;
	    }
	}

#اگر_تعریف_شدہ FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	اگر (part == PART_برآمد
		&& channel->ch_کام != تہی
		&& channel->ch_کام->jv_tty_برآمد != تہی
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    جاری;
	}
#اگر_ختم
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
#اگر_تعریف_شدہ ELAPSED_FUNC
			// Limit the time we loop here to 100 msec, otherwise
			// Vim becomes unresponsive when the callback takes
			// more than a bit of time.
			&& ELAPSED_FUNC(آغاز_tv) < 100L
#اگر_ختم
			))
	    {
		// channel was freed or something was done, start over
		channel = پہلا_channel;
		part = PART_SOCK;
		جاری;
	    }
	}
	اگر (part < PART_ERR)
	    ++part;
	ورنہ
	{
	    channel = channel->ch_اگلا;
	    part = PART_SOCK;
	}
    }

    اگر (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --محفوظ_مقصد_invoke_callback;
    --recursive;

    واپس ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    صحیح
channel_any_readahead(خالی)
{
    channel_T	*channel = پہلا_channel;
    ch_part_T	part = PART_SOCK;

    جب (channel != تہی)
    {
	اگر (channel_has_readahead(channel, part))
	    واپس TRUE;
	اگر (part < PART_ERR)
	    ++part;
	ورنہ
	{
	    channel = channel->ch_اگلا;
	    part = PART_SOCK;
	}
    }
    واپس FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    صحیح
سیٹ_ref_اندر_channel(صحیح copyID)
{
    صحیح		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    برائے (channel = پہلا_channel; !abort && channel != تہی;
						   channel = channel->ch_اگلا)
	اگر (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || سیٹ_ref_اندر_شے(&tv, copyID, تہی, تہی, تہی);
	}
    واپس abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ساکن ch_part_T
channel_part_send(channel_T *channel)
{
    اگر (channel->CH_SOCK_FD == INVALID_FD)
	واپس PART_اندر;
    واپس PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ساکن ch_part_T
channel_part_read(channel_T *channel)
{
    اگر (channel->CH_SOCK_FD == INVALID_FD)
	واپس PART_برآمد;
    واپس PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ساکن ch_منوال_T
channel_get_منوال(channel_T *channel, ch_part_T part)
{
    اگر (channel == تہی)
	واپس CH_منوال_JSON;
    واپس channel->ch_part[part].ch_منوال;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ساکن صحیح
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    واپس channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    خالی
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_نمبر = 0;
    اگر (اندر_vim9script() && check_for_chan_or_کام_arg(argvars, 0) == FAIL)
	واپس;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel != تہی)
	rettv->vval.v_نمبر = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_برآمد)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    خالی
f_ch_close(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    channel_T *channel;

    اگر (اندر_vim9script() && check_for_chan_or_کام_arg(argvars, 0) == FAIL)
	واپس;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel != تہی)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    خالی
f_ch_close_اندر(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    channel_T *channel;

    اگر (اندر_vim9script() && check_for_chan_or_کام_arg(argvars, 0) == FAIL)
	واپس;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    اگر (channel != تہی)
	channel_close_اندر(channel);
}

/*
 * "ch_getbufnr()" function
 */
    خالی
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_نمبر = -1;

    اگر (اندر_vim9script()
	    && (check_for_chan_or_کام_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	واپس;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel == تہی)
	واپس;

    char_u	*what = tv_get_string(&argvars[1]);
    صحیح		part;
    اگر (STRCMP(what, "err") == 0)
	part = PART_ERR;
    ورنہ اگر (STRCMP(what, "out") == 0)
	part = PART_برآمد;
    ورنہ اگر (STRCMP(what, "in") == 0)
	part = PART_اندر;
    ورنہ
	part = PART_SOCK;
    اگر (channel->ch_part[part].ch_bufref.br_buf != تہی)
	rettv->vval.v_نمبر =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    خالی
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    اگر (اندر_vim9script() && check_for_chan_or_کام_arg(argvars, 0) == FAIL)
	واپس;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel == تہی)
	واپس;

    rettv->v_type = VAR_کام;
    rettv->vval.v_کام = channel->ch_کام;
    اگر (channel->ch_کام != تہی)
	++channel->ch_کام->jv_refcount;
}

/*
 * "ch_info()" function
 */
    خالی
f_ch_info(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    channel_T *channel;

    اگر (اندر_vim9script() && check_for_chan_or_کام_arg(argvars, 0) == FAIL)
	واپس;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel != تہی && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    خالی
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    اگر (check_restricted() || check_secure())
	واپس;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    خالی
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    اگر (check_restricted() || check_secure())
	واپس;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    خالی
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    خالی
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    خالی
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    خالی
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_اظہاریہ_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    خالی
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_اظہاریہ_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    خالی
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    خالی
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    خالی
f_ch_setoptions(typval_T *argvars, typval_T *rettv غیر_مستعمل)
{
    channel_T	*channel;
    jobopt_T	opt;

    اگر (check_restricted() || check_secure())
	واپس;

    اگر (اندر_vim9script()
	    && (check_for_chan_or_کام_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	واپس;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    اگر (channel == تہی)
	واپس;
    clear_کام_options(&opt);
    اگر (get_کام_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_منوال_ALL, 0) == OK)
	channel_سیٹ_options(channel, &opt);
    free_کام_options(&opt);
}

/*
 * "ch_status()" function
 */
    خالی
f_ch_حالت(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    صحیح		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = تہی;

    اگر (اندر_vim9script()
	    && (check_for_chan_or_کام_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	واپس;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    اگر (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_کام_options(&opt);
	اگر (get_کام_options(&argvars[1], &opt, JO_PART, 0) == OK
						     && (opt.jo_سیٹ & JO_PART))
	    part = opt.jo_part;
    }

    rettv->vval.v_string = vim_strsave((char_u *)channel_حالت(channel, part));
}

/*
 * Get a string with information about the channel in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
channel_مقصد_string_buf(typval_T *varp, char_u *buf)
{
    channel_T *channel = varp->vval.v_channel;
    حرف      *حالت_2 = channel_حالت(channel, -1);

    اگر (channel == تہی)
	vim_snprintf((حرف *)buf, NUMBUFLEN, "channel %s", حالت_2);
    ورنہ
	vim_snprintf((حرف *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, حالت_2);
    واپس buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_ڈھونڈو(صحیح ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	اگر (ch->ch_id == ch_id)
	    واپس ch;
    واپس تہی;
}

#اگر_ختم // FEAT_JOB_CHANNEL
