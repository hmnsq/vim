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

#إذا defined(FEAT_کم_CHANNEL)

// TRUE when netbeans is running with a GUI.
#جے_تعریف_شدہ FEAT_GUI
# define CH_HAS_GUI (gui.وچ_use || gui.starting)
#جے_انت

// Note: when making changes here also adjust configure.ac.
#جے_تعریف_شدہ MSWIN
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
# define UNIX_رستہ_MAX 108
تعريفنوع بنية sockaddr_un {
    پتہ_FAMILY sun_family;
    حرف sun_رستہ[UNIX_رستہ_MAX];
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
#جے_انت

ساكن فراغ channel_read(channel_T *channel, ch_part_T part, حرف *func);
ساكن فراغ channel_wait_any(لما msec);
ساكن ch_منوال_T channel_get_منوال(channel_T *channel, ch_part_T part);
ساكن صحيح channel_get_timeout(channel_T *channel, ch_part_T part);
ساكن channel_T *channel_open_stdio(فراغ);
ساكن ch_part_T channel_part_send(channel_T *channel);
ساكن ch_part_T channel_part_read(channel_T *channel);

#تعریف_کرو FOR_ALL_CHANNELS(ch) \
    لكل ((ch) = پہلا_channel; (ch) != عدم; (ch) = (ch)->ch_اگلا)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ساكن صحيح محفوظ_منزل_invoke_callback = 0;

#جے_تعریف_شدہ MSWIN
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
    DWORD	nwrite, ناپ, پورا = 0;
    OVERLAPPED	ov;

    طالما (todo > 0)
    {
	إذا (todo > MAX_NAMED_PIPE_ناپ)
	    ناپ = MAX_NAMED_PIPE_ناپ;
	وإلا
	    ناپ = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_میدان(ov);
	nwrite = 0;
	إذا (!WriteFile(h, buf + پورا, ناپ, &nwrite, &ov))
	{
	    DWORD err = Get_آخری_Error();

	    إذا (err != ERROR_IO_PENDING)
		أرجع -1;
	    إذا (!Get_Overlapped_نتیجہ(h, &ov, &nwrite, FALSE))
		أرجع -1;
	    FlushFileBuffers(h);
	}
	وإلا إذا (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    اكسر;
	todo -= nwrite;
	پورا += nwrite;
    }
    أرجع (صحيح)پورا;
}

    ساكن فراغ
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#جے_انت

#جے_تعریف_شدہ MSWIN
# undef PERROR
# define PERROR(پیغام) (فراغ)semsg("%s: %s", پیغام, strerror_win32(errno))

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
    Format_سنیہا(
	FORMAT_سنیہا_ALLOCATE_بفر |
	FORMAT_سنیہا_FROM_SYSTEM |
	FORMAT_سنیہا_IGNORE_INSERTS,
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
#جے_انت

/*
 * The list of all allocated channels.
 */
ساكن channel_T *پہلا_channel = عدم;
ساكن صحيح اگلا_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
جوڑو_channel(فراغ)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    إذا (channel == عدم)
	أرجع عدم;

    channel->ch_id = اگلا_ch_id++;
    ch_log(channel, "Created channel");

    لكل (part = PART_SOCK; part < PART_گنتی; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#جے_تعریف_شدہ FEAT_GUI_X11
	channel->ch_part[part].ch_داخلہ_Handler = (Xt_داخلہ_Id)عدم;
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_GTK
	channel->ch_part[part].ch_داخلہ_Handler = 0;
#جے_انت
	channel->ch_part[part].ch_timeout = 2000;
    }

    إذا (پہلا_channel != عدم)
    {
	پہلا_channel->ch_prev = channel;
	channel->ch_اگلا = پہلا_channel;
    }
    پہلا_channel = channel;

    channel->ch_refcount = 1;
    أرجع channel;
}

    صحيح
has_any_channel(فراغ)
{
    أرجع پہلا_channel != عدم;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    صحيح
channel_still_useful(channel_T *channel)
{
    صحيح has_sock_پیغام;
    صحيح	has_باہرلا_پیغام;
    صحيح	has_err_پیغام;

    // If the job was killed the channel is not expected to work anymore.
    إذا (channel->ch_کم_killed && channel->ch_کم == عدم)
	أرجع FALSE;

    // If there is a close callback it may still need to be invoked.
    إذا (channel->ch_close_cb.cb_name != عدم)
	أرجع TRUE;

    // If reading from or a buffer it's still useful.
    إذا (channel->ch_part[PART_وچ].ch_bufref.br_buf != عدم)
	أرجع TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_پیغام = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_سر.rq_اگلا != عدم
		|| channel->ch_part[PART_SOCK].ch_json_سر.jq_اگلا != عدم;
    has_باہرلا_پیغام = channel->ch_part[PART_باہرلا].ch_fd != INVALID_FD
		  || channel->ch_part[PART_باہرلا].ch_سر.rq_اگلا != عدم
		  || channel->ch_part[PART_باہرلا].ch_json_سر.jq_اگلا != عدم;
    has_err_پیغام = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_سر.rq_اگلا != عدم
		  || channel->ch_part[PART_ERR].ch_json_سر.jq_اگلا != عدم;
    أرجع (channel->ch_callback.cb_name != عدم && (has_sock_پیغام
		|| has_باہرلا_پیغام || has_err_پیغام))
	    || ((channel->ch_part[PART_باہرلا].ch_callback.cb_name != عدم
		       || channel->ch_part[PART_باہرلا].ch_bufref.br_buf != عدم)
		    && has_باہرلا_پیغام)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != عدم
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != عدم)
		    && has_err_پیغام);
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
    إذا (channel->ch_اگلا != عدم)
	channel->ch_اگلا->ch_prev = channel->ch_prev;
    إذا (channel->ch_prev == عدم)
	پہلا_channel = channel->ch_اگلا;
    وإلا
	channel->ch_prev->ch_اگلا = channel->ch_اگلا;
    vim_free(channel);
}

    ساكن فراغ
channel_free(channel_T *channel)
{
    إذا (وچ_free_unref_اشیاء)
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
free_بن_ورتیا_channels_contents(صحيح copyID, صحيح نقاب)
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
free_بن_ورتیا_channels(صحيح copyID, صحيح نقاب)
{
    channel_T	*ch;
    channel_T	*ch_اگلا;

    لكل (ch = پہلا_channel; ch != عدم; ch = ch_اگلا)
    {
	ch_اگلا = ch->ch_اگلا;
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
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
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
سنیہا_From_Server_X11(Xt_اشارہ_گر client_انکڑے,
		  صحيح *unused1 بن_ورتیا,
		  Xt_داخلہ_Id *unused2 بن_ورتیا)
{
    channel_read_fd((صحيح)(لما)client_انکڑے);
}
# endif

# ifdef FEAT_GUI_GTK
#  إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن gboolean
سنیہا_From_Server_Gtk3(GIOChannel *unused1 بن_ورتیا,
		  GIOCondition unused2 بن_ورتیا,
		  gpointer client_انکڑے)
{
    channel_read_fd(GPOINTER_منزل_INT(client_انکڑے));
    أرجع TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  وإلا
    ساكن فراغ
سنیہا_From_Server_Gtk2(gpointer client_انکڑے,
		  gint unused1 بن_ورتیا,
		  Gdk_داخلہ_Condition unused2 بن_ورتیا)
{
    channel_read_fd((صحيح)(لما)client_انکڑے);
}
#  endif
# endif

    ساكن فراغ
channel_gui_رجسٹر_one(channel_T *channel, ch_part_T part بن_ورتیا)
{
    إذا (!CH_HAS_GUI)
	أرجع;

    // gets stuck in handling events for a not connected channel
    إذا (channel->ch_keep_open)
	أرجع;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    إذا (channel->ch_part[part].ch_داخلہ_Handler == (Xt_داخلہ_Id)عدم)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_ناں[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_داخلہ_Handler = Xt_App_جوڑو_داخلہ(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(Xt_اشارہ_گر)(Xt_داخلہ_Read_نقاب + Xt_داخلہ_Except_نقاب),
		سنیہا_From_Server_X11,
		(Xt_اشارہ_گر)(لما)channel->ch_part[part].ch_fd);
    }
# وإلا
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    إذا (channel->ch_part[part].ch_داخلہ_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_ناں[part], channel->ch_part[part].ch_fd);
#   إذا GTK_CHECK_نسخہ(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_داخلہ_Handler = g_io_جوڑو_watch(
		chnnl,
		G_IO_وچ|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		سنیہا_From_Server_Gtk3,
		GINT_منزل_اشارہ_گر(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   وإلا
	channel->ch_part[part].ch_داخلہ_Handler = gdk_داخلہ_جوڑو(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_داخلہ_Condition)
			     ((صحيح)GDK_داخلہ_READ + (صحيح)GDK_داخلہ_EXCEPTION),
		سنیہا_From_Server_Gtk2,
		(gpointer)(لما)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ساكن فراغ
channel_gui_رجسٹر(channel_T *channel)
{
    إذا (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_رجسٹر_one(channel, PART_SOCK);
    إذا (channel->CH_باہرلا_FD != INVALID_FD
	    && channel->CH_باہرلا_FD != channel->CH_SOCK_FD)
	channel_gui_رجسٹر_one(channel, PART_باہرلا);
    إذا (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_باہرلا_FD)
	channel_gui_رجسٹر_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    فراغ
channel_gui_رجسٹر_all(فراغ)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_رجسٹر(channel);
}

    ساكن فراغ
channel_gui_unregister_one(channel_T *channel بن_ورتیا, ch_part_T part بن_ورتیا)
{
# ifdef FEAT_GUI_X11
    إذا (channel->ch_part[part].ch_داخلہ_Handler != (Xt_داخلہ_Id)عدم)
    {
	ch_log(channel, "Unregistering part %s", ch_part_ناں[part]);
	Xt_Remove_داخلہ(channel->ch_part[part].ch_داخلہ_Handler);
	channel->ch_part[part].ch_داخلہ_Handler = (Xt_داخلہ_Id)عدم;
    }
# وإلا
#  ifdef FEAT_GUI_GTK
    إذا (channel->ch_part[part].ch_داخلہ_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_ناں[part]);
#   إذا GTK_CHECK_نسخہ(3,0,0)
	g_سروت_remove(channel->ch_part[part].ch_داخلہ_Handler);
#   وإلا
	gdk_داخلہ_remove(channel->ch_part[part].ch_داخلہ_Handler);
#   endif
	channel->ch_part[part].ch_داخلہ_Handler = 0;
    }
#  endif
# endif
}

    ساكن فراغ
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    لكل (part = PART_SOCK; part < PART_وچ; ++part)
	channel_gui_unregister_one(channel, part);
}

#جے_انت  // FEAT_GUI

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
#جے_تعریف_شدہ MSWIN
    u_long	val = 1;
#جے_انت

    طالما (TRUE)
    {
	لما	elapsed_msec = 0;
	صحيح	waitnow;
	صحيح	ret;

	إذا (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	إذا (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_وچ_channel_connect));
	    أرجع -1;
	}

	إذا (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    إذا (
#جے_تعریف_شدہ MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#وإلا
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#جے_انت
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
#جے_تعریف_شدہ EINPROGRESS
		&& errno != EINPROGRESS
#جے_انت
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
	    PERROR(_(e_cannot_connect_منزل_پورٹ));
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
#جے_بنا_تعریف MSWIN
	    صحيح			so_error = 0;
	    socklen_t		so_error_len = حجم(so_error);
	    بنية timeval	شروع_tv;
	    بنية timeval	سرا_tv;
#جے_انت
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#جے_بنا_تعریف MSWIN
	    gettimeofday(&شروع_tv, عدم);
#جے_انت
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, عدم, &tv);
	    إذا (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_منزل_پورٹ));
		sock_close(sd);
		أرجع -1;
	    }

#جے_تعریف_شدہ MSWIN
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
		    PERROR(_(e_cannot_connect_منزل_پورٹ));
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

	    gettimeofday(&سرا_tv, عدم);
	    elapsed_msec = (سرا_tv.tv_sec - شروع_tv.tv_sec) * 1000
				 + (سرا_tv.tv_usec - شروع_tv.tv_usec) / 1000;
#جے_انت
	}

#جے_بنا_تعریف MSWIN
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
		mch_delay((لما)waitnow, MCH_DELAY_IGNOREINPUT);
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
#جے_انت

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	أرجع -1;
    }

    إذا (*waittime >= 0)
    {
#جے_تعریف_شدہ MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#وإلا
	(فراغ)fcntl(sd, F_SETFL, 0);
#جے_انت
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
	ثابت حرف *رستہ,
	فراغ (*nb_close_cb)(فراغ))
{
    channel_T		*channel = عدم;
    صحيح			sd = -1;
    size_t		رستہ_len = STRLEN(رستہ);
    بنية sockaddr_un	server;
    size_t		server_len;
    صحيح			waittime = -1;

    إذا (*رستہ == NUL || رستہ_len >= حجم(server.sun_رستہ))
    {
	semsg(_(e_invalid_دلیل_str), رستہ);
	أرجع عدم;
    }

#جے_تعریف_شدہ MSWIN
    channel_init_winsock();
#جے_انت

    channel = جوڑو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    CLEAR_میدان(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_رستہ, رستہ, حجم(server.sun_رستہ) - 1);

    ch_log(channel, "Trying to connect to %s", رستہ);

    server_len = offsetof(بنية sockaddr_un, sun_رستہ) + رستہ_len + 1;
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
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)رستہ);
    channel->ch_پورٹ = 0;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جے_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#جے_انت

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
	صحيح پورٹ,
	صحيح waittime,
	فراغ (*nb_close_cb)(فراغ))
{
    صحيح			sd = -1;
    channel_T		*channel = عدم;
#جے_تعریف_شدہ FEAT_IPV6
    صحيح			err;
    بنية addrinfo	hints;
    بنية addrinfo	*res = عدم;
    بنية addrinfo	*addr = عدم;
#وإلا
    بنية sockaddr_وچ	server;
    بنية hostent	*host = عدم;
#جے_انت

#جے_تعریف_شدہ MSWIN
    channel_init_winsock();
#جے_انت

    channel = جوڑو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#جے_تعریف_شدہ FEAT_IPV6
    CLEAR_میدان(hints);
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
	semsg(_(e_getaddrinfo_وچ_channel_open_str), gai_strerror(err));
	channel_free(channel);
	أرجع عدم;
    }

    // Count the number of addresses for timeout distribution
    صحيح addr_گنتی = 0;
    لكل (addr = res; addr != عدم; addr = addr->ai_اگلا)
	addr_گنتی++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    إذا (waittime == 0)
	waittime = 15;

    صحيح addr_اشاریہ = 0;
    لكل (addr = res; addr != عدم; addr = addr->ai_اگلا, addr_اشاریہ++)
    {
	ثابت حرف  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ثابت فراغ  *src = عدم;
	حرف	    buf[NUMBUFLEN];
# endif
	صحيح	    try_waittime;
	صحيح	    before_waittime;
	صحيح	    consumed;
	صحيح	    بچیا_addrs;

	إذا (addr->ai_family == AF_INET6)
	{
	    بنية sockaddr_in6 *sai = (بنية sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_پورٹ = htons(پورٹ);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	وإلا إذا (addr->ai_family == AF_INET)
	{
	    بنية sockaddr_وچ *sai = (بنية sockaddr_وچ *)addr->ai_addr;

	    sai->sin_پورٹ = htons(پورٹ);
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

	ch_log(channel, "Trying to connect to %s port %d", dst, پورٹ);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	إذا (addr->ai_اگلا == عدم)
	    try_waittime = waittime;
	وإلا إذا (addr_اشاریہ == 0)
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
	    بچیا_addrs = addr_گنتی - addr_اشاریہ;
	    try_waittime = waittime / بچیا_addrs;
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
	إذا (waittime <= 0 && addr->ai_اگلا != عدم)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    اكسر;
	}
    }

    freeaddrinfo(res);
#وإلا
    CLEAR_میدان(server);
    server.sin_family = AF_INET;
    server.sin_پورٹ = htons(پورٹ);
    إذا ((host = gethostbyname(hostname)) == عدم)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_وچ_channel_open));
	channel_free(channel);
	أرجع عدم;
    }
    {
	حرف *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	حافظہ_نقل(&p, &host->h_addr_list[0], حجم(p));
	حافظہ_نقل((حرف *)&server.sin_addr, p, host->h_لمائی);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, پورٹ);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    إذا (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (بنية sockaddr *)&server, حجم(server),
								   &waittime);
#جے_انت

    إذا (sd < 0)
    {
	channel_free(channel);
	أرجع عدم;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)hostname);
    channel->ch_پورٹ = پورٹ;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جے_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#جے_انت

    أرجع channel;
}

    ساكن فراغ
free_سیٹ_callback(callback_T *cbp, callback_T *callback)
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
#جے_تعریف_شدہ FEAT_QUICKFIX
    سیٹ_option_قدر_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    سیٹ_option_قدر_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#جے_انت
    إذا (curbuf->b_ml.ml_mfp == عدم)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ساكن buf_T *
channel_لبھو_بفر(char_u *name, صحيح err, صحيح پیغام)
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
    إذا (پیغام)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    بدلیا_بائٹاں(1, 0);
    curbuf = save_curbuf;

    أرجع buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ساكن فراغ
channel_سیٹ_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    إذا (opt->jo_سیٹ & JO_منوال)
	لكل (part = PART_SOCK; part < PART_گنتی; ++part)
	    channel->ch_part[part].ch_منوال = opt->jo_منوال;
    إذا (opt->jo_سیٹ & JO_وچ_منوال)
	channel->ch_part[PART_وچ].ch_منوال = opt->jo_وچ_منوال;
    إذا (opt->jo_سیٹ & JO_باہرلا_منوال)
	channel->ch_part[PART_باہرلا].ch_منوال = opt->jo_باہرلا_منوال;
    إذا (opt->jo_سیٹ & JO_ERR_منوال)
	channel->ch_part[PART_ERR].ch_منوال = opt->jo_err_منوال;
    channel->ch_nonblock = opt->jo_noblock;

    إذا (opt->jo_سیٹ & JO_TIMEOUT)
	لكل (part = PART_SOCK; part < PART_گنتی; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    إذا (opt->jo_سیٹ & JO_باہرلا_TIMEOUT)
	channel->ch_part[PART_باہرلا].ch_timeout = opt->jo_باہرلا_timeout;
    إذا (opt->jo_سیٹ & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    إذا (opt->jo_سیٹ & JO_بلاک_WRITE)
	channel->ch_part[PART_وچ].ch_بلاک_write = 1;

    إذا (opt->jo_سیٹ & JO_CALLBACK)
	free_سیٹ_callback(&channel->ch_callback, &opt->jo_callback);
    إذا (opt->jo_سیٹ & JO_باہرلا_CALLBACK)
	free_سیٹ_callback(&channel->ch_part[PART_باہرلا].ch_callback,
							      &opt->jo_باہرلا_cb);
    إذا (opt->jo_سیٹ & JO_ERR_CALLBACK)
	free_سیٹ_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    إذا (opt->jo_سیٹ & JO_CLOSE_CALLBACK)
	free_سیٹ_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    إذا ((opt->jo_سیٹ & JO_باہرلا_IO) && opt->jo_io[PART_باہرلا] == JIO_بفر)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	إذا (!(opt->jo_سیٹ & JO_باہرلا_منوال))
	    channel->ch_part[PART_باہرلا].ch_منوال = CH_منوال_NL;
	إذا (opt->jo_سیٹ & JO_باہرلا_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_باہرلا]);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist),
					       (لما)opt->jo_io_buf[PART_باہرلا]);
	}
	وإلا
	{
	    صحيح پیغام = TRUE;

	    إذا (opt->jo_set2 & JO2_باہرلا_پیغام)
		پیغام = opt->jo_سنیہا[PART_باہرلا];
	    buf = channel_لبھو_بفر(opt->jo_io_name[PART_باہرلا], FALSE, پیغام);
	}
	إذا (buf != عدم)
	{
	    إذا (opt->jo_سیٹ & JO_باہرلا_MODIFIABLE)
		channel->ch_part[PART_باہرلا].ch_nomodifiable =
						!opt->jo_modifiable[PART_باہرلا];

	    إذا (!buf->b_p_ma && !channel->ch_part[PART_باہرلا].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    وإلا
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (حرف *)buf->b_ffname);
		سیٹ_bufref(&channel->ch_part[PART_باہرلا].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		إذا (buf->b_ml.ml_mfp == عدم)
		    prepare_بفر(buf);
	    }
	}
    }

    إذا ((opt->jo_سیٹ & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_بفر
	 || (opt->jo_io[PART_ERR] == JIO_باہرلا && (opt->jo_سیٹ & JO_باہرلا_IO)
				       && opt->jo_io[PART_باہرلا] == JIO_بفر)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	إذا (!(opt->jo_سیٹ & JO_ERR_منوال))
	    channel->ch_part[PART_ERR].ch_منوال = CH_منوال_NL;
	إذا (opt->jo_io[PART_ERR] == JIO_باہرلا)
	    buf = channel->ch_part[PART_باہرلا].ch_bufref.br_buf;
	وإلا إذا (opt->jo_سیٹ & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    إذا (buf == عدم)
		semsg(_(e_بفر_nr_does_not_exist),
					       (لما)opt->jo_io_buf[PART_ERR]);
	}
	وإلا
	{
	    صحيح پیغام = TRUE;

	    إذا (opt->jo_set2 & JO2_ERR_پیغام)
		پیغام = opt->jo_سنیہا[PART_ERR];
	    buf = channel_لبھو_بفر(opt->jo_io_name[PART_ERR], TRUE, پیغام);
	}
	إذا (buf != عدم)
	{
	    إذا (opt->jo_سیٹ & JO_ERR_MODIFIABLE)
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
		سیٹ_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		إذا (buf->b_ml.ml_mfp == عدم)
		    prepare_بفر(buf);
	    }
	}
    }

    channel->ch_part[PART_باہرلا].ch_io = opt->jo_io[PART_باہرلا];
    channel->ch_part[PART_ERR].ch_io = opt->jo_io[PART_ERR];
    channel->ch_part[PART_وچ].ch_io = opt->jo_io[PART_وچ];
}

/*
 * Parse the address for socketserver and store port in "port", or the path in
 * "unix_path" if it is a unix domain socket. Returns OK on success and FAIL on
 * failure.
 */
    صحيح
channel_parse_socketserver_پتہ(
	char_u	*پتہ,
	صحيح	*پورٹ,
	char_u	**unix_رستہ,
	bool	quiet)
{
    حرف    *rest;
    لما    val;

    إذا (*پتہ == NUL)
	اذهب fail;

    إذا (STRNCMP(پتہ, "unix:", 5) == 0)
    {
	*unix_رستہ = vim_strsave(پتہ + 5);
	أرجع OK;
    }

    val = strtol((حرف *)(پتہ), &rest, 10);
    إذا (val < 0 || val >= 65536 || *rest != NUL)
    {
	إذا (!quiet)
	    semsg(_(e_invalid_دلیل_str), پتہ);
	أرجع FAIL;
    }
    *پورٹ = (صحيح)val;

    أرجع OK;
fail:
    إذا (!quiet)
	semsg(_(e_invalid_دلیل_str), پتہ);
    أرجع FAIL;
}

/*
 * Implements ch_open().
 */
    ساكن channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*پتہ;
    char_u	*p;
    حرف	*rest;
    صحيح		پورٹ = 0;
    صحيح		is_ipv6 = FALSE;
    صحيح		is_unix = FALSE;
    صحيح		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = عدم;

    إذا (وچ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع عدم;

    پتہ = tv_get_string(&argvars[0]);
    إذا (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	أرجع عدم;

    إذا (*پتہ == NUL)
    {
	semsg(_(e_invalid_دلیل_str), پتہ);
	أرجع عدم;
    }

    إذا (STRCMP(پتہ, "stdio") == 0)
	is_stdio = TRUE;
    وإلا إذا (STRNCMP(پتہ, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	پتہ += 5;
    }
    وإلا إذا (*پتہ == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(پتہ + 1, ']');
	إذا (p == عدم || *++p != ':')
	{
	    semsg(_(e_invalid_دلیل_str), پتہ);
	    أرجع عدم;
	}
    }
    وإلا
    {
	// ipv4 address
	p = vim_strchr(پتہ, ':');
	إذا (p == عدم)
	{
	    semsg(_(e_invalid_دلیل_str), پتہ);
	    أرجع عدم;
	}
    }

    إذا (!is_unix && !is_stdio)
    {
	پورٹ = strtol((حرف *)(p + 1), &rest, 10);
	إذا (پورٹ <= 0 || پورٹ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_دلیل_str), پتہ);
	    أرجع عدم;
	}
	إذا (is_ipv6)
	{
	    // strip '[' and ']'
	    ++پتہ;
	    *(p - 1) = NUL;
	}
	وإلا
	    *p = NUL;
    }

    // parse options
    clear_کم_options(&opt);
    opt.jo_منوال = CH_منوال_JSON;
    opt.jo_timeout = 2000;
    إذا (get_کم_options(&argvars[1], &opt,
	    JO_منوال_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	اذهب theend;
    إذا (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_دلیل));
	اذهب theend;
    }

    إذا (is_stdio)
	channel = channel_open_stdio();
    وإلا إذا (is_unix)
	channel = channel_open_unix((حرف *)پتہ, عدم);
    وإلا
	channel = channel_open((حرف *)پتہ, پورٹ, opt.jo_waittime, عدم);
    إذا (channel != عدم)
    {
	صحيح	user_سیٹ = opt.jo_سیٹ;

	opt.jo_سیٹ = JO_ALL;
	إذا (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_سیٹ &= ~((JO_وچ_منوال | JO_باہرلا_منوال | JO_ERR_منوال
				| JO_باہرلا_TIMEOUT | JO_ERR_TIMEOUT) & ~user_سیٹ);
	channel_سیٹ_options(channel, &opt);
    }
theend:
    free_کم_options(&opt);
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
    sock_T	وچ_fd;		// our stdin, the channel reads from it
    sock_T	باہرلا_fd;		// our stdout, the channel writes to it

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
#جے_تعریف_شدہ MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	صحيح	nul_fd;

	إذا (!DuplicateHandle(proc, GetStdHandle(STD_داخلہ_HANDLE), proc, &hin,
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
	    سیٹ_Std_Handle(STD_داخلہ_HANDLE, hnul);
	    سیٹ_Std_Handle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	إذا (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	وچ_fd = (sock_T)hin;
	باہرلا_fd = (sock_T)hout;
    }
#وإلا
    {
	صحيح	nul_fd;

	وچ_fd = dup(0);
	باہرلا_fd = dup(1);
	إذا (وچ_fd < 0 || باہرلا_fd < 0)
	{
	    إذا (وچ_fd >= 0)
		close(وچ_fd);
	    إذا (باہرلا_fd >= 0)
		close(باہرلا_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    أرجع عدم;
	}
	(فراغ)fcntl(وچ_fd, F_SETFD, FD_CLOEXEC);
	(فراغ)fcntl(باہرلا_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	إذا (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#جے_انت

    channel = جوڑو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	fd_close(وچ_fd);
	fd_close(باہرلا_fd);
	أرجع عدم;
    }
    channel_سیٹ_pipes(channel, باہرلا_fd, وچ_fd, INVALID_FD);
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
    صحيح		پورٹ;
    صحيح		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = عدم;

    إذا (وچ_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	أرجع عدم;

    arg = tv_get_string(&argvars[0]);
    إذا (*arg == NUL)
    {
	semsg(_(e_invalid_دلیل_str), arg);
	أرجع عدم;
    }

    إذا (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	پورٹ = 0;
    }
    وإلا
    {
	پورٹ = strtol((حرف *)(arg), &rest, 10);
	إذا (پورٹ < 0 || پورٹ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_دلیل_str), arg);
	    أرجع عدم;
	}
	*arg = NUL;
    }

    // parse options
    clear_کم_options(&opt);
    opt.jo_منوال = CH_منوال_JSON;
    opt.jo_timeout = 2000;
    إذا (get_کم_options(&argvars[1], &opt,
	    JO_منوال_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	اذهب theend;
    إذا (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_دلیل));
	اذهب theend;
    }

    إذا (is_unix)
	channel = channel_listen_unix((حرف *)arg, عدم, true);
    وإلا
	channel = channel_listen(پورٹ, عدم);
    إذا (channel != عدم)
    {
	opt.jo_سیٹ = JO_ALL;
	channel_سیٹ_options(channel, &opt);
    }
theend:
    free_کم_options(&opt);
    أرجع channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	صحيح پورٹ_وچ,
	فراغ (*nb_close_cb)(فراغ))
{
    صحيح			sd = -1;
    بنية sockaddr_وچ	server;
#جے_بنا_تعریف FEAT_IPV6
    بنية hostent	*host;
#جے_انت
    صحيح			val = 1;
    channel_T		*channel;

#جے_تعریف_شدہ MSWIN
    channel_init_winsock();
#جے_انت

    channel = جوڑو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((حرف *)&server, 0, حجم(server));
    server.sin_family = AF_INET;
    server.sin_پورٹ = htons(پورٹ_وچ);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    إذا (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	channel_free(channel);
	أرجع عدم;
    }

#جے_تعریف_شدہ MSWIN
    إذا (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ثابت حرف *)&val, حجم(val)) < 0)
#وإلا
    إذا (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, حجم(val)) < 0)
#جے_انت
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    // Bind the socket to the port
    إذا (bind(sd, (بنية sockaddr *)&server, حجم(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    إذا (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    إذا (پورٹ_وچ == 0)
    {
	بنية sockaddr_وچ	addr;
	socklen_t		addr_len = حجم(addr);

	إذا (getsockname(sd, (بنية sockaddr *)&addr, &addr_len) == 0)
	    پورٹ_وچ = ntohs(addr.sin_پورٹ);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)"");
    channel->ch_پورٹ = پورٹ_وچ;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جے_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#جے_انت

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
	حرف *رستہ,
	فراغ (*nb_close_cb)(فراغ),
	bool replace)
{
    صحيح			sd = -1;
    بنية sockaddr_un	server;
    channel_T		*channel;
    size_t		رستہ_len;
    size_t		server_len;

    إذا (رستہ == عدم || *رستہ == NUL)
    {
	semsg(_(e_invalid_دلیل_str), رستہ == عدم ? (حرف *)"" : رستہ);
	أرجع عدم;
    }

    رستہ_len = STRLEN(رستہ);
    إذا (رستہ_len >= حجم(server.sun_رستہ))
    {
	semsg(_(e_invalid_دلیل_str), رستہ);
	أرجع عدم;
    }

#جے_تعریف_شدہ MSWIN
    channel_init_winsock();
#جے_انت

    channel = جوڑو_channel();
    إذا (channel == عدم)
    {
	ch_error(عدم, "Cannot allocate channel.");
	أرجع عدم;
    }

    CLEAR_میدان(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_رستہ, رستہ, حجم(server.sun_رستہ) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    إذا (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	channel_free(channel);
	أرجع عدم;
    }

    إذا (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_رستہ);

    // Bind the socket to the path
    server_len = offsetof(بنية sockaddr_un, sun_رستہ) + رستہ_len + 1;
    إذا (bind(sd, (بنية sockaddr *)&server, (صحيح)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    إذا (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_پورٹ));
	sock_close(sd);
	channel_free(channel);
	أرجع عدم;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (حرف *)vim_strsave((char_u *)رستہ);
    channel->ch_پورٹ = 0;
    channel->ch_منزل_be_closed |= (1U << PART_SOCK);

#جے_تعریف_شدہ FEAT_GUI
    channel_gui_رجسٹر_one(channel, PART_SOCK);
#جے_انت

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
	إذا ((part == PART_وچ || channel->CH_وچ_FD != *fd)
		&& (part == PART_باہرلا || channel->CH_باہرلا_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#جے_تعریف_شدہ MSWIN
	    إذا (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#جے_انت
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_منزل_be_closed &= ~(1U << part);
}

    فراغ
channel_سیٹ_pipes(channel_T *channel, sock_T وچ, sock_T باہرلا, sock_T err)
{
    إذا (وچ != INVALID_FD)
    {
	ch_close_part(channel, PART_وچ);
	channel->CH_وچ_FD = وچ;
#إذا defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	إذا (mch_isatty(وچ))
	    channel->ch_منزل_be_closed |= (1U << PART_وچ);
#جے_انت
    }
    إذا (باہرلا != INVALID_FD)
    {
#إذا defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_باہرلا);
#جے_انت
	ch_close_part(channel, PART_باہرلا);
	channel->CH_باہرلا_FD = باہرلا;
	channel->ch_منزل_be_closed |= (1U << PART_باہرلا);
#إذا defined(FEAT_GUI)
	channel_gui_رجسٹر_one(channel, PART_باہرلا);
#جے_انت
    }
    إذا (err != INVALID_FD)
    {
#إذا defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#جے_انت
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_منزل_be_closed |= (1U << PART_ERR);
#إذا defined(FEAT_GUI)
	channel_gui_رجسٹر_one(channel, PART_ERR);
#جے_انت
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    فراغ
channel_سیٹ_کم(channel_T *channel, کم_T *کم, jobopt_T *options)
{
    channel->ch_کم = کم;

    channel_سیٹ_options(channel, options);

    إذا (کم->jv_وچ_buf == عدم)
	أرجع;

    chanpart_T *وچ_part = &channel->ch_part[PART_وچ];

    سیٹ_bufref(&وچ_part->ch_bufref, کم->jv_وچ_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (حرف *)وچ_part->ch_bufref.br_buf->b_ffname);
    إذا (options->jo_سیٹ & JO_وچ_چوٹی)
    {
	إذا (options->jo_وچ_چوٹی == 0 && !(options->jo_سیٹ & JO_وچ_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    وچ_part->ch_bufref.br_buf->b_write_منزل_channel = true;
	    وچ_part->ch_buf_append = TRUE;
	    وچ_part->ch_buf_چوٹی =
		وچ_part->ch_bufref.br_buf->b_ml.ml_سطر_گنتی + 1;
	}
	وإلا
	    وچ_part->ch_buf_چوٹی = options->jo_وچ_چوٹی;
    }
    وإلا
	وچ_part->ch_buf_چوٹی = 1;
    إذا (options->jo_سیٹ & JO_وچ_BOT)
	وچ_part->ch_buf_bot = options->jo_وچ_bot;
    وإلا
	وچ_part->ch_buf_bot = وچ_part->ch_bufref.br_buf->b_ml.ml_سطر_گنتی;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ساكن فراغ
channel_سیٹ_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	صحيح	    id)
{
    cbq_T *سر = &channel->ch_part[part].ch_cb_سر;
    cbq_T *شے = ALLOC_ONE(cbq_T);

    إذا (شے == عدم)
	أرجع;

    copy_callback(&شے->cq_callback, callback);
    شے->cq_seq_nr = id;
    شے->cq_prev = سر->cq_prev;
    سر->cq_prev = شے;
    شے->cq_اگلا = عدم;
    إذا (شے->cq_prev == عدم)
	سر->cq_اگلا = شے;
    وإلا
	شے->cq_prev->cq_اگلا = شے;
}

    ساكن فراغ
write_buf_سطر(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *سطر = ml_get_buf(buf, lnum, FALSE);
    صحيح	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    صحيح	    i;

    // Need to make a copy to be able to append a NL.
    إذا ((p = alloc(len + 2)) == عدم)
	أرجع;
    حافظہ_نقل((حرف *)p, (حرف *)سطر, len);

    إذا (channel->ch_write_متن_منوال)
	p[len] = CAR;
    وإلا
    {
	لكل (i = 0; i < len; ++i)
	    إذا (p[i] == NL)
		p[i] = NUL;

	p[len] = NL;
    }
    p[len + 1] = NUL;
    channel_send(channel, PART_وچ, p, len + 1, "write_buf_line");
    vim_free(p);
}

/*
 * Return TRUE if "channel" can be written to.
 * Returns FALSE if the input is closed or the write would block.
 */
    ساكن صحيح
can_write_buf_سطر(channel_T *channel)
{
    chanpart_T *وچ_part = &channel->ch_part[PART_وچ];

    إذا (وچ_part->ch_fd == INVALID_FD)
	أرجع FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    إذا (وچ_part->ch_بلاک_write == 1)
	وچ_part->ch_بلاک_write = -1;
    وإلا إذا (وچ_part->ch_بلاک_write == -1)
	وچ_part->ch_بلاک_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#جے_بنا_تعریف MSWIN
    {
# إذا defined(HAVE_SELECT)
	بنية timeval	tval;
	fd_set		wfds;
	صحيح		ret;

	FD_ZERO(&wfds);
	FD_SET((صحيح)وچ_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	لكل (;;)
	{
	    ret = select((صحيح)وچ_part->ch_fd + 1, عدم, &wfds, عدم, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    إذا (ret == -1 && errno == EINTR)
		استمر;
#  endif
	    إذا (ret <= 0 || وچ_part->ch_بلاک_write == 1)
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

	fds.fd = وچ_part->ch_fd;
	fds.events = POLLOUT;
	إذا (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    أرجع FALSE;
	}
	إذا (وچ_part->ch_بلاک_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    أرجع FALSE;
	}
# endif
    }
#جے_انت
    أرجع TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    فراغ
channel_write_وچ(channel_T *channel)
{
    chanpart_T *وچ_part = &channel->ch_part[PART_وچ];
    linenr_T    lnum;
    buf_T	*buf = وچ_part->ch_bufref.br_buf;
    صحيح		written = 0;

    إذا (buf == عدم || وچ_part->ch_buf_append)
	أرجع;  // no buffer or using appending
    إذا (!bufref_valid(&وچ_part->ch_bufref) || buf->b_ml.ml_mfp == عدم)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	وچ_part->ch_bufref.br_buf = عدم;
	أرجع;
    }

    لكل (lnum = وچ_part->ch_buf_چوٹی; lnum <= وچ_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_سطر_گنتی; ++lnum)
    {
	إذا (!can_write_buf_سطر(channel))
	    اكسر;
	write_buf_سطر(buf, lnum, channel);
	++written;
    }

    إذا (written == 1)
	ch_log(channel, "written line %d to channel", (صحيح)lnum - 1);
    وإلا إذا (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    وچ_part->ch_buf_چوٹی = lnum;
    إذا (lnum > buf->b_ml.ml_سطر_گنتی || lnum > وچ_part->ch_buf_bot)
    {
#إذا defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	إذا (channel->ch_کم != عدم)
	    رکن_send_eof(channel);
#جے_انت

	// Writing is done, no longer need the buffer.
	وچ_part->ch_bufref.br_buf = عدم;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#جے_تعریف_شدہ MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	إذا (!channel->ch_anonymous_pipe)
#جے_انت
	    ch_close_part(channel, PART_وچ);
    }
    وإلا
	ch_log(channel, "Still %ld more lines to write",
				   (لما)(buf->b_ml.ml_سطر_گنتی - lnum + 1));
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
	لكل (part = PART_SOCK; part < PART_گنتی; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    إذا (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_ناں[part]);
		ch_part->ch_bufref.br_buf = عدم;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    فراغ
channel_write_داخلہ(channel_T *channel)
{
    chanpart_T	*وچ_part = &channel->ch_part[PART_وچ];

    إذا (وچ_part->ch_writeque.wq_اگلا != عدم)
	channel_send(channel, PART_وچ, (char_u *)"", 0, "channel_write_input");
    وإلا إذا (وچ_part->ch_bufref.br_buf != عدم)
    {
	إذا (وچ_part->ch_buf_append)
	    channel_write_new_سطریں(وچ_part->ch_bufref.br_buf);
	وإلا
	    channel_write_وچ(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    فراغ
channel_write_any_سطریں(فراغ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_داخلہ(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    فراغ
channel_write_new_سطریں(buf_T *buf)
{
    channel_T	*channel;
    صحيح		لبھیا_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *وچ_part = &channel->ch_part[PART_وچ];
	linenr_T    lnum;
	صحيح	    written = 0;

	إذا (وچ_part->ch_bufref.br_buf == buf && وچ_part->ch_buf_append)
	{
	    إذا (وچ_part->ch_fd == INVALID_FD)
		استمر;  // pipe was closed
	    لبھیا_one = TRUE;
	    لكل (lnum = وچ_part->ch_buf_bot; lnum < buf->b_ml.ml_سطر_گنتی;
								       ++lnum)
	    {
		إذا (!can_write_buf_سطر(channel))
		    اكسر;
		write_buf_سطر(buf, lnum, channel);
		++written;
	    }

	    إذا (written == 1)
		ch_log(channel, "written line %d to channel", (صحيح)lnum - 1);
	    وإلا إذا (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    إذا (lnum < buf->b_ml.ml_سطر_گنتی)
		ch_log(channel, "Still %ld more lines to write",
				       (لما)(buf->b_ml.ml_سطر_گنتی - lnum));

	    وچ_part->ch_buf_bot = lnum;
	}
    }
    إذا (!لبھیا_one)
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

    أرجع سر->rq_اگلا;
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

    لكل (i = 0; i < گرہ->rq_buflen; ++i)
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
    readq_T *گرہ = سر->rq_اگلا;
    char_u *p;

    إذا (گرہ == عدم)
	أرجع عدم;
    إذا (outlen != عدم)
	*outlen += گرہ->rq_buflen;
    // dispose of the node but keep the buffer
    p = گرہ->rq_بفر;
    سر->rq_اگلا = گرہ->rq_اگلا;
    إذا (گرہ->rq_اگلا == عدم)
	سر->rq_prev = عدم;
    وإلا
	گرہ->rq_اگلا->rq_prev = عدم;
    vim_free(گرہ);
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
    readq_T *گرہ;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    لكل (گرہ = سر->rq_اگلا; گرہ != عدم; گرہ = گرہ->rq_اگلا)
	len += گرہ->rq_buflen;
    res = alloc(len + 1);
    إذا (res == عدم)
	أرجع عدم;
    p = res;
    لكل (گرہ = سر->rq_اگلا; گرہ != عدم; گرہ = گرہ->rq_اگلا)
    {
	mch_memmove(p, گرہ->rq_بفر, گرہ->rq_buflen);
	p += گرہ->rq_buflen;
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
#جے_تعریف_شدہ MSWIN
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
#جے_انت
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
    readq_T *گرہ = سر->rq_اگلا;
    char_u *buf = گرہ->rq_بفر;

    إذا (len < 0 || (long_u)len > گرہ->rq_buflen)
	أرجع;
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
    صحيح
channel_collapse(channel_T *channel, ch_part_T part, صحيح want_nl)
{
    ch_منوال_T	منوال = channel->ch_part[part].ch_منوال;
    readq_T	*سر = &channel->ch_part[part].ch_سر;
    readq_T	*گرہ = سر->rq_اگلا;
    readq_T	*آخری_گرہ;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    إذا (گرہ == عدم || گرہ->rq_اگلا == عدم)
	أرجع FAIL;

    آخری_گرہ = گرہ->rq_اگلا;
    len = گرہ->rq_buflen + آخری_گرہ->rq_buflen;
    إذا (want_nl || منوال == CH_منوال_LSP || منوال == CH_منوال_DAP)
	طالما (آخری_گرہ->rq_اگلا != عدم
		&& (منوال == CH_منوال_LSP || منوال == CH_منوال_DAP
		    || channel_پہلا_nl(آخری_گرہ) == عدم))
	{
	    آخری_گرہ = آخری_گرہ->rq_اگلا;
	    len += آخری_گرہ->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    إذا (newbuf == عدم)
	أرجع FAIL;	    // out of memory
    mch_memmove(p, گرہ->rq_بفر, گرہ->rq_buflen);
    p += گرہ->rq_buflen;
    vim_free(گرہ->rq_بفر);
    گرہ->rq_بفر = newbuf;
    لكل (n = گرہ; n != آخری_گرہ; )
    {
	n = n->rq_اگلا;
	mch_memmove(p, n->rq_بفر, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_بفر);
    }
    *p = NUL;
    گرہ->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    لكل (n = گرہ->rq_اگلا; n != آخری_گرہ; )
    {
	n = n->rq_اگلا;
	vim_free(n->rq_prev);
    }
    گرہ->rq_اگلا = آخری_گرہ->rq_اگلا;
    إذا (آخری_گرہ->rq_اگلا == عدم)
	سر->rq_prev = گرہ;
    وإلا
	آخری_گرہ->rq_اگلا->rq_prev = گرہ;
    vim_free(آخری_گرہ);
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
    readq_T *گرہ;
    readq_T *سر = &channel->ch_part[part].ch_سر;
    char_u  *p;
    صحيح	    i;

    گرہ = ALLOC_ONE(readq_T);
    إذا (گرہ == عدم)
	أرجع FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    گرہ->rq_بفر = alloc(len + 1);
    إذا (گرہ->rq_بفر == عدم)
    {
	vim_free(گرہ);
	أرجع FAIL;	    // out of memory
    }

    إذا (channel->ch_part[part].ch_منوال == CH_منوال_NL)
    {
	// Drop any CR before a NL.
	p = گرہ->rq_بفر;
	لكل (i = 0; i < len; ++i)
	    إذا (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	گرہ->rq_buflen = (long_u)(p - گرہ->rq_بفر);
    }
    وإلا
    {
	mch_memmove(گرہ->rq_بفر, buf, len);
	گرہ->rq_بفر[len] = NUL;
	گرہ->rq_buflen = (long_u)len;
    }

    إذا (prepend)
    {
	// prepend node to the head of the queue
	گرہ->rq_اگلا = سر->rq_اگلا;
	گرہ->rq_prev = عدم;
	إذا (سر->rq_اگلا == عدم)
	    سر->rq_prev = گرہ;
	وإلا
	    سر->rq_اگلا->rq_prev = گرہ;
	سر->rq_اگلا = گرہ;
    }
    وإلا
    {
	// append node to the tail of the queue
	گرہ->rq_اگلا = عدم;
	گرہ->rq_prev = سر->rq_prev;
	إذا (سر->rq_prev == عدم)
	    سر->rq_اگلا = گرہ;
	وإلا
	    سر->rq_prev->rq_اگلا = گرہ;
	سر->rq_prev = گرہ;
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
    char_u	*اگلا = channel_get(channel, part, عدم);
    صحيح		keeplen;
    صحيح		addlen;
    char_u	*p;

    إذا (اگلا == عدم)
	أرجع FALSE;

    keeplen = reader->js_سرا - reader->js_buf;
    إذا (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (صحيح)STRLEN(اگلا);
	p = alloc(keeplen + addlen + 1);
	إذا (p == عدم)
	{
	    vim_free(اگلا);
	    أرجع FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, اگلا, addlen + 1);
	vim_free(اگلا);
	اگلا = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = اگلا;
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
    char_u	*سطر_شروع;
    char_u	*p;
    int_u	hdr_len;
    صحيح		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_سرا = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    طالما (TRUE)
    {
	سطر_شروع = p;
	طالما (*p != NUL && *p != '\n')
	    p++;
	إذا (*p == NUL)			// partial header
	    أرجع MAYBE;
	p++;

	// process the content length field (if present)
	إذا ((p - سطر_شروع > 16)
		&& STRNICMP(سطر_شروع, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((حرف *)سطر_شروع + 16, عدم, 10);
	    إذا (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		أرجع FAIL;
	}

	إذا ((p - سطر_شروع) == 2 && سطر_شروع[0] == '\r' &&
		سطر_شروع[1] == '\n')
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

    reader->js_ورتیا += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_سرا = reader->js_buf + hdr_len + payload_len;

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
    jsonq_T	*شے;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*سر = &chanpart->ch_json_سر;
    صحيح		حالت = OK;
    صحيح		ret;

    إذا (channel_peek(channel, part) == عدم)
	أرجع FALSE;

    reader.js_buf = channel_get(channel, part, عدم);
    reader.js_ورتیا = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    إذا (!socketserver && (chanpart->ch_منوال == CH_منوال_LSP
	    || chanpart->ch_منوال == CH_منوال_DAP))
	حالت = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    إذا (حالت == OK)
    {
	++emsg_silent;
	حالت = json_decode(&reader, &listtv,
				chanpart->ch_منوال == CH_منوال_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    إذا (حالت == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	إذا (!socketserver && (chanpart->ch_منوال == CH_منوال_LSP
		    || chanpart->ch_منوال == CH_منوال_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	وإلا إذا (!socketserver && chanpart->ch_منوال != CH_منوال_LSP
		&& chanpart->ch_منوال != CH_منوال_DAP
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
	    شے = ALLOC_ONE(jsonq_T);
	    إذا (شے == عدم)
		clear_tv(&listtv);
	    وإلا
	    {
		شے->jq_no_callback = FALSE;
		شے->jq_قدر = alloc_tv();
		إذا (شے->jq_قدر == عدم)
		{
		    vim_free(شے);
		    clear_tv(&listtv);
		}
		وإلا
		{
		    *شے->jq_قدر = listtv;
		    شے->jq_prev = سر->jq_prev;
		    سر->jq_prev = شے;
		    شے->jq_اگلا = عدم;
		    إذا (شے->jq_prev == عدم)
			سر->jq_اگلا = شے;
		    وإلا
			شے->jq_prev->jq_اگلا = شے;
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
	    reader.js_ورتیا = 0;
	    chanpart->ch_wait_len = buflen;
#جے_تعریف_شدہ MSWIN
	    chanpart->ch_deadline = Get_Tick_گنتی() + 100L;
#وإلا
	    gettimeofday(&chanpart->ch_deadline, عدم);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    إذا (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#جے_انت
	}
	وإلا
	{
	    صحيح timeout;
#جے_تعریف_شدہ MSWIN
	    timeout = (صحيح)(Get_Tick_گنتی() - chanpart->ch_deadline) > 0;
#وإلا
	    {
		بنية timeval now_tv;

		gettimeofday(&now_tv, عدم);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#جے_انت
	    إذا (timeout)
	    {
		حالت = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    وإلا
	    {
		reader.js_ورتیا = 0;
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
    وإلا إذا (reader.js_buf[reader.js_ورتیا] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_ورتیا,
			(صحيح)(reader.js_سرا - reader.js_buf) - reader.js_ورتیا,
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
remove_cb_گرہ(cbq_T *سر, cbq_T *گرہ)
{
    إذا (گرہ->cq_prev == عدم)
	سر->cq_اگلا = گرہ->cq_اگلا;
    وإلا
	گرہ->cq_prev->cq_اگلا = گرہ->cq_اگلا;
    إذا (گرہ->cq_اگلا == عدم)
	سر->cq_prev = گرہ->cq_prev;
    وإلا
	گرہ->cq_اگلا->cq_prev = گرہ->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    فراغ
remove_json_گرہ(jsonq_T *سر, jsonq_T *گرہ)
{
    إذا (گرہ->jq_prev == عدم)
	سر->jq_اگلا = گرہ->jq_اگلا;
    وإلا
	گرہ->jq_prev->jq_اگلا = گرہ->jq_اگلا;
    إذا (گرہ->jq_اگلا == عدم)
	سر->jq_prev = گرہ->jq_prev;
    وإلا
	گرہ->jq_اگلا->jq_prev = گرہ->jq_prev;
    vim_free(گرہ);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ساكن فراغ
channel_جوڑو_بلاک_id(chanpart_T *chanpart, صحيح id)
{
    garray_T *gap = &chanpart->ch_بلاک_ids;

    إذا (gap->ga_growsize == 0)
	ga_init2(gap, حجم(صحيح), 10);
    إذا (ga_grow(gap, 1) == OK)
    {
	((صحيح *)gap->ga_انکڑے)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ساكن فراغ
channel_remove_بلاک_id(chanpart_T *chanpart, صحيح id)
{
    garray_T	*gap = &chanpart->ch_بلاک_ids;
    صحيح		i;

    لكل (i = 0; i < gap->ga_len; ++i)
	إذا (((صحيح *)gap->ga_انکڑے)[i] == id)
	{
	    --gap->ga_len;
	    إذا (i < gap->ga_len)
	    {
		صحيح *p = ((صحيح *)gap->ga_انکڑے) + i;

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
channel_has_بلاک_id(chanpart_T *chanpart, صحيح id)
{
    garray_T	*gap = &chanpart->ch_بلاک_ids;
    صحيح		i;

    لكل (i = 0; i < gap->ga_len; ++i)
	إذا (((صحيح *)gap->ga_انکڑے)[i] == id)
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
    jsonq_T   *شے = سر->jq_اگلا;

    طالما (شے != عدم)
    {
	list_T	    *l;
	typval_T    *tv;

	إذا (channel->ch_part[part].ch_منوال != CH_منوال_LSP
		&& channel->ch_part[part].ch_منوال != CH_منوال_DAP)
	{
	    l = شے->jq_قدر->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_پہلا->li_tv;
	}
	وإلا
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    إذا (channel->ch_part[part].ch_منوال == CH_منوال_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		إذا (id > 0)
		{
		    إذا (شے->jq_قدر->v_type != VAR_DICT)
			اذهب nextitem;
		    d = شے->jq_قدر->vval.v_dict;
		    إذا (d == عدم)
			اذهب nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    إذا (dict_has_کلید(d, "method"))
			اذهب nextitem;
		    di = dict_لبھو(d, (char_u *)"id", -1);
		    إذا (di == عدم)
			اذهب nextitem;
		    tv = &di->di_tv;
		}
		وإلا
		    tv = شے->jq_قدر;
	    }
	    وإلا
	    {
		إذا (id > 0)
		{
		    إذا (شے->jq_قدر->v_type != VAR_DICT)
			اذهب nextitem;
		    d = شے->jq_قدر->vval.v_dict;
		    إذا (d == عدم)
			اذهب nextitem;
		    di = dict_لبھو(d, (char_u *)"request_seq", -1);
		    إذا (di == عدم)
			اذهب nextitem;
		    tv = &di->di_tv;
		}
		وإلا
		    tv = شے->jq_قدر;
	    }
	}

	إذا ((without_callback || !شے->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_نمبر && tv->vval.v_نمبر == id)
	      || (id <= 0 && (tv->v_type != VAR_نمبر
		 || tv->vval.v_نمبر == 0
		 || !channel_has_بلاک_id(
				&channel->ch_part[part], tv->vval.v_نمبر)))))
	{
	    *rettv = شے->jq_قدر;
	    إذا (tv->v_type == VAR_نمبر)
		ch_log(channel, "Getting JSON message %ld",
						      (لما)tv->vval.v_نمبر);
	    remove_json_گرہ(سر, شے);
	    أرجع OK;
	}
nextitem:
	شے = شے->jq_اگلا;
    }
    أرجع FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ساكن فراغ
channel_ڈالو_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;
    jsonq_T   *شے = سر->jq_اگلا;
    jsonq_T   *newitem;

    إذا (سر->jq_prev != عدم && سر->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	شے = عدم;
    وإلا طالما (شے != عدم && شے->jq_no_callback)
	// append after the last item that was pushed back
	شے = شے->jq_اگلا;

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
    إذا (شے == عدم)
    {
	// append to the end
	newitem->jq_prev = سر->jq_prev;
	سر->jq_prev = newitem;
	newitem->jq_اگلا = عدم;
	إذا (newitem->jq_prev == عدم)
	    سر->jq_اگلا = newitem;
	وإلا
	    newitem->jq_prev->jq_اگلا = newitem;
    }
    وإلا
    {
	// append after "item"
	newitem->jq_prev = شے;
	newitem->jq_اگلا = شے->jq_اگلا;
	شے->jq_اگلا = newitem;
	إذا (newitem->jq_اگلا == عدم)
	    سر->jq_prev = newitem;
	وإلا
	    newitem->jq_اگلا->jq_prev = newitem;
    }
}

#تعریف_کرو CH_JSON_MAX_دلیلاں 4

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
    صحيح	    options = channel->ch_part[part].ch_منوال == CH_منوال_JS
								 ? JSON_JS : 0;

    إذا (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	إذا (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_دلیل));
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
	CLEAR_میدان(ea);
	ea.arg = arg;
	ea.addr_گنتی = 0;
	ea.forceit = TRUE; // no mapping
	ex_عام(&ea);
    }
    وإلا إذا (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	باہرلا_flush_cursor(TRUE, FALSE);
    }
    وإلا إذا (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	صحيح is_call = cmd[0] == 'c';
	صحيح id_idx = is_call ? 3 : 2;

	إذا (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_نمبر)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    إذا (p_verbose > 2)
		emsg(_(e_آخری_دلیل_for_اظہاریہ_call_must_be_نمبر));
	}
	وإلا إذا (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    إذا (p_verbose > 2)
		emsg(_(e_تیجا_دلیل_for_call_must_be_list));
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
		tv = eval_اظہاریہ(arg, عدم);
	    }
	    وإلا
	    {
		ch_log(channel, "Calling '%s'", (حرف *)arg);
		إذا (func_call(arg, &argv[2], عدم, عدم, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    إذا (argv[id_idx].v_type == VAR_نمبر)
	    {
		صحيح id = argv[id_idx].vval.v_نمبر;

		إذا (tv != عدم)
		    json = json_encode_nr_اظہاریہ(id, tv, options | JSON_NL);
		إذا (tv == عدم || (json != عدم && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_اظہاریہ(id, &err_tv, options | JSON_NL);
		}
		إذا (json != عدم)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_وچ,
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
invoke_one_ویلا_callback(
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

    ساكن فراغ
append_منزل_بفر(
    buf_T	*بفر,
    char_u	*پیغام,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = بفر->b_ml.ml_سطر_گنتی;
    bool	save_write_منزل = بفر->b_write_منزل_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    صحيح		save_p_ma = بفر->b_p_ma;
    صحيح		خالی_تھاواں_2 = (بفر->b_ml.ml_flags & ML_خالی_تھاواں) ? 1 : 0;

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
				       (صحيح)lnum + 1 - خالی_تھاواں_2, بفر->b_fname);

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
    vim_ignored = u_save(lnum - خالی_تھاواں_2, lnum + 1);

    إذا (خالی_تھاواں_2)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, پیغام, TRUE);
	lnum = 0;
    }
    وإلا
	ml_append(lnum, پیغام, 0, FALSE);
    appended_سطریں_mark(lnum, 1L);

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
				&& wp->w_cursor.خانہ == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		إذا (move_cursor || wp->w_topline > بفر->b_ml.ml_سطر_گنتی)
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
	    chanpart_T  *وچ_part = &ch->ch_part[PART_وچ];

	    إذا (وچ_part->ch_bufref.br_buf == بفر)
		وچ_part->ch_buf_bot = بفر->b_ml.ml_سطر_گنتی;
	}
    }
}

    ساكن فراغ
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *پیغام;

    طالما ((پیغام = channel_get(channel, part, عدم)) != عدم)
    {
	ch_log(channel, "Dropping message '%s'", (حرف *)پیغام);
	vim_free(پیغام);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ساكن صحيح
channel_use_json_سر(channel_T *channel, ch_part_T part)
{
    ch_منوال_T	ch_منوال = channel->ch_part[part].ch_منوال;

    أرجع ch_منوال == CH_منوال_JSON || ch_منوال == CH_منوال_JS
						     || ch_منوال == CH_منوال_LSP
						     || ch_منوال == CH_منوال_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ساكن صحيح
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*پیغام = عدم;
    blob_T	*blob = عدم;
    typval_T	*listtv = عدم;
    typval_T	argv[CH_JSON_MAX_دلیلاں];
    صحيح		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_منوال_T	ch_منوال = ch_part->ch_منوال;
    cbq_T	*cbhead = &ch_part->ch_cb_سر;
    cbq_T	*cbitem;
    callback_T	*callback = عدم;
    buf_T	*بفر = عدم;
    char_u	*p;
    صحيح		called_otc;		// one time callbackup
    صحيح		raw_len = 0;

    إذا (channel->ch_nb_close_cb != عدم
#جے_تعریف_شدہ FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#جے_انت
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	أرجع FALSE;

    // Use a message-specific callback, part callback or channel callback
    لكل (cbitem = cbhead->cq_اگلا; cbitem != عدم; cbitem = cbitem->cq_اگلا)
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
	ch_log(channel, "%s buffer has been wiped out", ch_part_ناں[part]);
	ch_part->ch_bufref.br_buf = عدم;
	بفر = عدم;
    }

    إذا (channel_use_json_سر(channel, part))
    {
	listitem_T	*شے;
	صحيح		argc = 0;

	// Get any json message in the queue.
	إذا (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    إذا (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(فراغ)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    إذا (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		أرجع FALSE;
	}

	إذا (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    إذا (d != عدم)
	    {
		إذا (ch_منوال == CH_منوال_LSP)
		    di = dict_لبھو(d, (char_u *)"id", -1);
		وإلا
		    di = dict_لبھو(d, (char_u *)"request_seq", -1);
		إذا (di != عدم && di->di_tv.v_type == VAR_نمبر)
		    seq_nr = di->di_tv.vval.v_نمبر;
	    }

	    argv[1] = *listtv;
	}
	وإلا
	{
	    لكل (شے = listtv->vval.v_list->lv_پہلا;
		    شے != عدم && argc < CH_JSON_MAX_دلیلاں;
		    شے = شے->li_اگلا)
		argv[argc++] = شے->li_tv;
	    طالما (argc < CH_JSON_MAX_دلیلاں)
		argv[argc++].v_type = VAR_UNKNOWN;

	    إذا (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		أرجع TRUE;
	    }

	    إذا (argv[0].v_type != VAR_نمبر)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		أرجع FALSE;
	    }
	    seq_nr = argv[0].vval.v_نمبر;
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

	إذا (ch_منوال == CH_منوال_NL)
	{
	    char_u  *nl = عدم;
	    char_u  *buf;
	    readq_T *گرہ;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    طالما (TRUE)
	    {
		گرہ = channel_peek(channel, part);
		nl = channel_پہلا_nl(گرہ);
		إذا (nl != عدم)
		    اكسر;
		إذا (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    إذا (ch_part->ch_fd == INVALID_FD && گرہ->rq_buflen > 0)
			اكسر;
		    أرجع FALSE; // incomplete message
		}
	    }
	    buf = گرہ->rq_بفر;

	    // Convert NUL to NL, the internal representation.
	    لكل (p = buf; (nl == عدم || p < nl)
					    && p < buf + گرہ->rq_buflen; ++p)
		إذا (*p == NUL)
		    *p = NL;

	    إذا (nl == عدم)
	    {
		// get the whole buffer, drop the NL
		پیغام = channel_get(channel, part, عدم);
	    }
	    وإلا إذا (nl + 1 == buf + گرہ->rq_buflen)
	    {
		// get the whole buffer
		پیغام = channel_get(channel, part, عدم);
		*nl = NUL;
	    }
	    وإلا
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		پیغام = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (صحيح)(nl - buf) + 1);
	    }
	}
	وإلا
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    پیغام = channel_get_all(channel, part,
			ch_منوال == CH_منوال_BLOB ? &raw_len : عدم);
	}

	إذا (پیغام == عدم)
	    أرجع FALSE; // out of memory (and avoids Coverity warning)

	إذا (ch_منوال == CH_منوال_BLOB)
	{
	    blob = blob_alloc();
	    إذا (blob == عدم)
	    {
		vim_free(پیغام);
		أرجع FALSE;
	    }
	    إذا (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(پیغام);
		أرجع FALSE;
	    }
	    إذا (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_انکڑے, پیغام, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	وإلا
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = پیغام;
	}
    }

    called_otc = FALSE;
    إذا (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	صحيح lsp_req_پیغام = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	إذا ((ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_کلید(argv[1].vval.v_dict, "method"))
	    lsp_req_پیغام = TRUE;

	إذا (!lsp_req_پیغام)
	{
	    لكل (cbitem = cbhead->cq_اگلا; cbitem != عدم;
		    cbitem = cbitem->cq_اگلا)
	    {
		إذا (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_ویلا_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    اكسر;
		}
	    }
	}
    }

    إذا (seq_nr > 0 && ((ch_منوال != CH_منوال_LSP && ch_منوال != CH_منوال_DAP)
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
		channel_ڈالو_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_نمبر;
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
	    إذا (پیغام == عدم)
		// JSON or JS mode: re-encode the message.
		پیغام = json_encode(listtv, ch_منوال);
	    إذا (پیغام != عدم)
	    {
#جے_تعریف_شدہ FEAT_TERMINAL
		إذا (بفر->b_رکن != عدم)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    إذا (part == PART_ERR)
		    {
			char_u *cp;
			صحيح    extra = 0;

			لكل (cp = پیغام; *cp != NUL; ++cp)
			    إذا (*cp == NL && (cp == پیغام || cp[-1] != CAR))
				++extra;
			إذا (extra > 0)
			{
			    char_u *crlf_پیغام = alloc(STRLEN(پیغام) + extra + 1);

			    إذا (crlf_پیغام != عدم)
			    {
				char_u *q = crlf_پیغام;

				لكل (cp = پیغام; *cp != NUL; ++cp)
				{
				    إذا (*cp == NL && (cp == پیغام || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_منزل_رکن(بفر, crlf_پیغام, channel);
				vim_free(crlf_پیغام);
			    }
			}
			وإلا
			    write_منزل_رکن(بفر, پیغام, channel);
		    }
		    وإلا
			write_منزل_رکن(بفر, پیغام, channel);
		}
		وإلا
#جے_انت
		    append_منزل_بفر(بفر, پیغام, channel, part);
	    }
	}

	إذا (callback != عدم)
	{
	    إذا (cbitem != عدم)
		invoke_one_ویلا_callback(channel, cbhead, cbitem, argv);
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
    vim_free(پیغام);

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
			  || channel->CH_وچ_FD != INVALID_FD);
}
#جے_انت

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    صحيح
channel_is_open(channel_T *channel)
{
    أرجع channel != عدم && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_وچ_FD != INVALID_FD
			  || channel->CH_باہرلا_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ساكن فراغ *
channel_readahead_اشارہ_گر(channel_T *channel, ch_part_T part)
{
    إذا (channel_use_json_سر(channel, part))
    {
	jsonq_T   *سر = &channel->ch_part[part].ch_json_سر;

	إذا (سر->jq_اگلا == عدم)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	أرجع سر->jq_اگلا;
    }
    أرجع channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ساكن صحيح
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    أرجع channel_readahead_اشارہ_گر(channel, part) != عدم;
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
    إذا (req_part == PART_باہرلا)
    {
	إذا (channel->CH_باہرلا_FD != INVALID_FD)
	    أرجع "open";
	إذا (channel_has_readahead(channel, PART_باہرلا))
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
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
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
    size_t	پوچھ;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    پوچھ = STRLEN(namebuf);
    STRCPY(namebuf + پوچھ, "_");
    پوچھ += STRLEN_LITERAL("_");

    STRCPY(namebuf + پوچھ, "status");
    إذا (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_سیٹ(s, "open");
    وإلا إذا (channel_has_readahead(channel, part))
	STR_LITERAL_سیٹ(s, "buffered");
    وإلا
	STR_LITERAL_سیٹ(s, "closed");
    dict_جوڑو_string_len(dict, namebuf, s.string, (صحيح)s.لمائی);

    STRCPY(namebuf + پوچھ, "mode");
    بدل (chanpart->ch_منوال)
    {
	حالة CH_منوال_NL:
	    STR_LITERAL_سیٹ(s, "NL");
	    اكسر;
	حالة CH_منوال_RAW:
	    STR_LITERAL_سیٹ(s, "RAW");
	    اكسر;
	حالة CH_منوال_BLOB:
	    STR_LITERAL_سیٹ(s, "BLOB");
	    اكسر;
	حالة CH_منوال_JSON:
	    STR_LITERAL_سیٹ(s, "JSON");
	    اكسر;
	حالة CH_منوال_JS:
	    STR_LITERAL_سیٹ(s, "JS");
	    اكسر;
	حالة CH_منوال_LSP:
	    STR_LITERAL_سیٹ(s, "LSP");
	    اكسر;
	حالة CH_منوال_DAP:
	    STR_LITERAL_سیٹ(s, "DAP");
	    اكسر;
	افتراضي:
	    STR_LITERAL_سیٹ(s, "");
	    اكسر;
    }
    dict_جوڑو_string_len(dict, namebuf, s.string, (صحيح)s.لمائی);

    STRCPY(namebuf + پوچھ, "io");
    إذا (part == PART_SOCK)
	STR_LITERAL_سیٹ(s, "socket");
    وإلا بدل (chanpart->ch_io)
    {
	حالة JIO_NULL:
	    STR_LITERAL_سیٹ(s, "null");
	    اكسر;
	حالة JIO_PIPE:
	    STR_LITERAL_سیٹ(s, "pipe");
	    اكسر;
	حالة JIO_FILE:
	    STR_LITERAL_سیٹ(s, "file");
	    اكسر;
	حالة JIO_بفر:
	    STR_LITERAL_سیٹ(s, "buffer");
	    اكسر;
	حالة JIO_باہرلا:
	    STR_LITERAL_سیٹ(s, "out");
	    اكسر;
	افتراضي:
	    STR_LITERAL_سیٹ(s, "");
	    اكسر;
    }
    dict_جوڑو_string_len(dict, namebuf, s.string, (صحيح)s.لمائی);

    STRCPY(namebuf + پوچھ, "timeout");
    dict_جوڑو_نمبر(dict, namebuf, chanpart->ch_timeout);
}

    ساكن فراغ
channel_info(channel_T *channel, dict_T *dict)
{
    dict_جوڑو_نمبر(dict, "id", channel->ch_id);
    dict_جوڑو_string(dict, "status", (char_u *)channel_حالت(channel, -1));

    إذا (channel->ch_hostname != عدم)
    {
	إذا (channel->ch_پورٹ)
	    dict_جوڑو_نمبر(dict, "port", channel->ch_پورٹ);
	وإلا
	    // Unix-domain socket.
	    dict_جوڑو_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    وإلا
    {
	channel_part_info(channel, dict, "out", PART_باہرلا);
	channel_part_info(channel, dict, "err", PART_ERR);
	channel_part_info(channel, dict, "in", PART_وچ);
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

#جے_تعریف_شدہ FEAT_GUI
    channel_gui_unregister(channel);
#جے_انت

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_وچ);
    ch_close_part(channel, PART_باہرلا);
    ch_close_part(channel, PART_ERR);

    إذا (invoke_close_cb)
    {
	ch_part_T	part;

#جے_تعریف_شدہ FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	رکن_channel_closing(channel);
#جے_انت
	// Invoke callbacks and flush buffers before the close callback.
	إذا (channel->ch_close_cb.cb_name != عدم)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
	{
	    إذا (channel->ch_close_cb.cb_name != عدم
			    || channel->ch_part[part].ch_bufref.br_buf != عدم)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		إذا (channel->ch_close_cb.cb_name == عدم)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_ناں[part]);
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
		  لكل (part = PART_SOCK; part < PART_وچ; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = عدم;
#جے_تعریف_شدہ FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = عدم;
    channel->ch_ss_close_cb = عدم;
#جے_انت

#جے_تعریف_شدہ FEAT_TERMINAL
    رکن_channel_closed(channel);
#جے_انت
}

/*
 * Close the "in" part channel "channel".
 */
    ساكن فراغ
channel_close_وچ(channel_T *channel)
{
    ch_close_part(channel, PART_وچ);
}

    ساكن فراغ
remove_from_writeque(writeq_T *wq, writeq_T *اندراج)
{
    ga_clear(&اندراج->wq_ga);
    wq->wq_اگلا = اندراج->wq_اگلا;
    إذا (wq->wq_اگلا == عدم)
	wq->wq_prev = عدم;
    وإلا
	wq->wq_اگلا->wq_prev = عدم;
    vim_free(اندراج);
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

    طالما (cb_سر->cq_اگلا != عدم)
    {
	cbq_T *گرہ = cb_سر->cq_اگلا;

	remove_cb_گرہ(cb_سر, گرہ);
	free_callback(&گرہ->cq_callback);
	vim_free(گرہ);
    }

    طالما (json_سر->jq_اگلا != عدم)
    {
	free_tv(json_سر->jq_اگلا->jq_قدر);
	remove_json_گرہ(json_سر, json_سر->jq_اگلا);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_بلاک_ids);

    طالما (ch_part->ch_writeque.wq_اگلا != عدم)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_اگلا);
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
    channel_clear_one(channel, PART_باہرلا);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_وچ);
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
#جے_انت


// Sent when the netbeans channel is found closed when reading.
#تعریف_کرو DETACH_پیغام_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#تعریف_کرو MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ساكن صحيح
is_channel_write_بچیا(chanpart_T *وچ_part)
{
    buf_T *buf = وچ_part->ch_bufref.br_buf;

    إذا (وچ_part->ch_writeque.wq_اگلا != عدم)
	أرجع TRUE;
    إذا (buf == عدم)
	أرجع FALSE;
    أرجع وچ_part->ch_buf_append
	    ? (وچ_part->ch_buf_bot < buf->b_ml.ml_سطر_گنتی)
	    : (وچ_part->ch_buf_چوٹی <= وچ_part->ch_buf_bot
			    && وچ_part->ch_buf_چوٹی <= buf->b_ml.ml_سطر_گنتی);
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
	chanpart_T  *وچ_part = &ch->ch_part[PART_وچ];

	إذا (وچ_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (صحيح)وچ_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_بچیا(وچ_part))
	{
	    FD_SET((صحيح)وچ_part->ch_fd, wfds);
	    إذا ((صحيح)وچ_part->ch_fd >= maxfd)
		maxfd = (صحيح)وچ_part->ch_fd + 1;
	}
    }
    أرجع maxfd;
}
#وإلا
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ساكن صحيح
channel_fill_poll_write(صحيح nfd_وچ, بنية pollfd *fds)
{
    صحيح		nfd = nfd_وچ;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *وچ_part = &ch->ch_part[PART_وچ];

	إذا (وچ_part->ch_fd != INVALID_FD
		&& is_channel_write_بچیا(وچ_part))
	{
	    وچ_part->ch_poll_idx = nfd;
	    fds[nfd].fd = وچ_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	وإلا
	    وچ_part->ch_poll_idx = -1;
    }
    أرجع nfd;
}
#جے_انت

تعريفنوع تعداد {
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
    ساكن channel_wait_نتیجہ
channel_wait(channel_T *channel, sock_T fd, صحيح timeout)
{
    إذا (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#جے_تعریف_شدہ MSWIN
    إذا (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	صحيح	sleep_ویلا;
	DWORD	deadline = Get_Tick_گنتی() + timeout;
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
	    channel_write_any_سطریں();

	    sleep_ویلا = deadline - Get_Tick_گنتی();
	    إذا (sleep_ویلا <= 0)
		اكسر;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    إذا (sleep_ویلا > delay)
		sleep_ویلا = delay;
	    Sleep(sleep_ویلا);
	    delay = delay * 2;
	    إذا (delay > 10)
		delay = 10;
	}
    }
    وإلا
#جے_انت
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
		channel_write_any_سطریں();
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
		channel_write_any_سطریں();
		استمر;
	    }
	    اكسر;
	}
#جے_انت
    }
    أرجع CW_NOT_READY;
}

    ساكن فراغ
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, صحيح is_err, حرف *func)
{
    حرف	پیغام[] = "%s(): Read %s from ch_part[%d], closing";

    إذا (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, پیغام, func, "error", part);
    وإلا
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
    إذا (channel->ch_nb_close_cb != عدم)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_پیغام_RAW,
			      (صحيح)STRLEN_LITERAL(DETACH_پیغام_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    إذا (part == PART_باہرلا || part == PART_ERR)
    {
	ch_part_T other = part == PART_باہرلا ? PART_ERR : PART_باہرلا;

	إذا (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#جے_تعریف_شدہ FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#جے_انت
}

    ساكن فراغ
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    إذا (channel->ch_nb_close_cb != عدم)
	(*channel->ch_nb_close_cb)();
#جے_تعریف_شدہ FEAT_SOCKETSERVER
    إذا (channel->ch_ss_close_cb != عدم)
	channel->ch_ss_close_cb(channel);
#جے_انت
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
							  ch_part_ناں[part]);
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

	    newchannel = جوڑو_channel();
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

#جے_تعریف_شدہ FEAT_GUI
	    channel_gui_رجسٹر_one(newchannel, PART_SOCK);
#جے_انت

#جے_تعریف_شدہ FEAT_SOCKETSERVER
	    إذا (channel->ch_ss_accept_cb != عدم)
	    {
		channel->ch_ss_accept_cb(newchannel);
		أرجع;
	    }
#جے_انت

	    إذا (client.ss_family == AF_INET)
	    {
#جے_تعریف_شدہ HAVE_INET_NTOP
		حرف addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((بنية sockaddr_وچ*)&client)->sin_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
			addr,
			ntohs(((بنية sockaddr_وچ*)&client)->sin_پورٹ));
#وإلا
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "%s:%d",
		    inet_ntoa(((بنية sockaddr_وچ*)&client)->sin_addr),
		    ntohs(((بنية sockaddr_وچ*)&client)->sin_پورٹ));
#جے_انت
	    }
#جے_تعریف_شدہ HAVE_INET_NTOP
	    وإلا إذا (client.ss_family == AF_INET6)
	    {
		حرف addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((بنية sockaddr_in6*)&client)->sin6_addr,
			addr, حجم(addr));
		vim_snprintf((حرف *)namebuf, حجم(namebuf), "[%s]:%d",
			addr,
			ntohs(((بنية sockaddr_in6*)&client)->sin6_پورٹ));
	    }
#جے_انت
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
#جے_انت
}

#جے_تعریف_شدہ FEAT_SOCKETSERVER

    فراغ
channel_check(channel_T *channel, ch_part_T part)
{
    channel_read(channel, part, "channel_check");
}

#جے_انت

/*
 * Read from RAW or NL "channel"/"part".  Blocks until there is something to
 * read or the timeout expires.
 * When "raw" is TRUE don't block waiting on a NL.
 * Does not trigger timers or handle messages.
 * Returns what was read in allocated memory.
 * Returns NULL in case of error or timeout.
 */
    ساكن char_u *
channel_read_بلاک(
	channel_T *channel, ch_part_T part, صحيح timeout, صحيح raw, صحيح *outlen)
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

    طالما (TRUE)
    {
	گرہ = channel_peek(channel, part);
	إذا (گرہ != عدم)
	{
	    إذا (منوال == CH_منوال_RAW || منوال == CH_منوال_BLOB
		    || (منوال == CH_منوال_NL
					   && channel_پہلا_nl(گرہ) != عدم))
		// got a complete message
		اكسر;
	    إذا (channel_collapse(channel, part, منوال == CH_منوال_NL) == OK)
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
    إذا (منوال == CH_منوال_RAW || منوال == CH_منوال_BLOB || outlen != عدم)
    {
	پیغام = channel_get_all(channel, part, outlen);
    }
    وإلا
    {
	char_u *p;

	buf = گرہ->rq_بفر;
	nl = channel_پہلا_nl(گرہ);

	// Convert NUL to NL, the internal representation.
	لكل (p = buf; (nl == عدم || p < nl) && p < buf + گرہ->rq_buflen; ++p)
	    إذا (*p == NUL)
		*p = NL;

	إذا (nl == عدم)
	{
	    // must be a closed channel with missing NL
	    پیغام = channel_get(channel, part, عدم);
	}
	وإلا إذا (nl + 1 == buf + گرہ->rq_buflen)
	{
	    // get the whole buffer
	    پیغام = channel_get(channel, part, عدم);
	    *nl = NUL;
	}
	وإلا
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    پیغام = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (صحيح)(nl - buf) + 1);
	}
    }
    إذا (ch_log_سرگرم())
	ch_log(channel, "Returning %d bytes",
		outlen != عدم ? *outlen : (صحيح)STRLEN(پیغام));
    أرجع پیغام;
}

ساكن صحيح channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    صحيح
channel_وچ_blocking_wait(فراغ)
{
    أرجع channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ساكن فراغ
channel_wait_reading(channel_T *channel بن_ورتیا, sock_T fd بن_ورتیا, صحيح timeout)
{
#جے_تعریف_شدہ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (فراغ)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#وإلا
    channel_wait_any(timeout);
#جے_انت
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
channel_read_json_بلاک(
	channel_T   *channel,
	ch_part_T   part,
	صحيح	    timeout_arg,
	صحيح	    id,
	typval_T    **rettv)
{
    صحيح		ہور;
    sock_T	fd;
    صحيح		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_منوال_T	منوال = channel->ch_part[part].ch_منوال;
    صحيح		retval = FAIL;
#جے_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	شروع_tv;

    ELAPSED_INIT(شروع_tv);
#جے_انت

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    إذا (id >= 0)
	channel_جوڑو_بلاک_id(chanpart, id);

    لكل (;;)
    {
	إذا (منوال == CH_منوال_LSP || منوال == CH_منوال_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (فراغ)channel_collapse(channel, part, FALSE);

	ہور = channel_parse_json(channel, part, false);

	// search for message "id"
	إذا (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    اكسر;
	}

	إذا (!ہور)
	{
	    فراغ *prev_readahead_ptr = channel_readahead_اشارہ_گر(channel, part);
	    فراغ *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    إذا (channel_parse_messages())
		استمر;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_اشارہ_گر(channel, part);
	    إذا (readahead_ptr != عدم && readahead_ptr != prev_readahead_ptr)
		استمر;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#جے_تعریف_شدہ ELAPSED_FUNC
	    timeout -= (صحيح)ELAPSED_FUNC(شروع_tv);
#جے_انت
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
		صحيح	بایاں;
#جے_تعریف_شدہ MSWIN
		بایاں = chanpart->ch_deadline - Get_Tick_گنتی() + 1;
#وإلا
		{
		    بنية timeval now_tv;

		    gettimeofday(&now_tv, عدم);
		    بایاں = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#جے_انت
		إذا (بایاں < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		وإلا إذا (بایاں < timeout)
		    timeout = بایاں;
	    }
	    channel_wait_reading(channel, fd, timeout);
#جے_بنا_تعریف ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#جے_انت
	}
    }
    إذا (id >= 0)
	channel_remove_بلاک_id(chanpart, id);
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

    إذا (tv->v_type == VAR_کم)
    {
	إذا (tv->vval.v_کم != عدم)
	    channel = tv->vval.v_کم->jv_channel;
    }
    وإلا إذا (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    وإلا
    {
	semsg(_(e_invalid_دلیل_str), tv_get_string(tv));
	أرجع عدم;
    }
    إذا (channel != عدم && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_گنتی ? part : channel_part_read(channel));

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
    ch_part_T	part = PART_گنتی;
    jobopt_T	opt;
    صحيح		منوال;
    صحيح		timeout;
    صحيح		id = -1;
    typval_T	*listtv = عدم;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (وچ_vim9script()
	    && (check_for_chan_or_کم_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    clear_کم_options(&opt);
    إذا (get_کم_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	اذهب theend;

    إذا (opt.jo_سیٹ & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    إذا (channel == عدم)
	اذهب theend;

    إذا (part == PART_گنتی)
	part = channel_part_read(channel);
    منوال = channel_get_منوال(channel, part);
    timeout = channel_get_timeout(channel, part);
    إذا (opt.jo_سیٹ & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    إذا (blob || منوال == CH_منوال_BLOB)
    {
	صحيح	    outlen = 0;
	char_u  *p = channel_read_بلاک(channel, part,
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
		    حافظہ_نقل(b->bv_ga.ga_انکڑے, p, outlen);
		    rettv_blob_سیٹ(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    وإلا إذا (raw || منوال == CH_منوال_RAW || منوال == CH_منوال_NL)
	rettv->vval.v_string = channel_read_بلاک(channel, part,
		timeout, raw, عدم);
    وإلا
    {
	إذا (opt.jo_سیٹ & JO_ID)
	    id = opt.jo_id;
	channel_read_json_بلاک(channel, part, timeout, id, &listtv);
	إذا (listtv != عدم)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	وإلا
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_نمبر = VVAL_NONE;
	}
    }

theend:
    free_کم_options(&opt);
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
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
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
			(part == PART_باہرلا || part == PART_ERR)) ? 1 : 0);

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
	إذا (channel->ch_کم != عدم)
	{
	    کم_T *کم = channel->ch_کم;

	    إذا (کم->jv_tty_باہرلا != عدم && کم->jv_حالت == کم_مکمل)
		لكل (part = PART_SOCK; part < PART_گنتی; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#جے_انت

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
#جے_انت

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    فراغ
channel_سیٹ_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    صحيح		fd = ch_part->ch_fd;

    إذا (fd == INVALID_FD)
	أرجع;

#جے_تعریف_شدہ MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#وإلا
    (فراغ)fcntl(fd, F_SETFL, O_NONBLOCK);
#جے_انت
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
	channel_سیٹ_nonblock(channel, part);

    إذا (ch_log_سرگرم())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_پیغام = 0;
    }

    لكل (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	صحيح	    len;

	إذا (wq->wq_اگلا != عدم)
	{
	    // first write what was queued
	    buf = wq->wq_اگلا->wq_ga.ga_انکڑے;
	    len = wq->wq_اگلا->wq_ga.ga_len;
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
#جے_تعریف_شدہ MSWIN
	    إذا (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, عدم);
	    }
#جے_انت
	}
	إذا (res < 0 && (errno == EWOULDBLOCK
#جے_تعریف_شدہ EAGAIN
			|| errno == EAGAIN
#جے_انت
		    ))
	    res = 0; // nothing got written

	إذا (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *اندراج = wq->wq_اگلا;

	    إذا (did_use_قطار)
		ch_log(channel, "Sent %d bytes now", res);
	    إذا (res == len)
	    {
		// Wrote all the buf[len] bytes.
		إذا (اندراج != عدم)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, اندراج);
		    استمر;
		}
		إذا (did_use_قطار)
		    ch_log(channel, "Write queue empty");
	    }
	    وإلا
	    {
		// Wrote only buf[res] bytes, can't write more now.
		إذا (اندراج != عدم)
		{
		    إذا (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(اندراج->wq_ga.ga_انکڑے,
				    (حرف *)اندراج->wq_ga.ga_انکڑے + res,
				    len - res);
			اندراج->wq_ga.ga_len -= res;
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
		    writeq_T *آخری = wq->wq_prev;

		    // append to the last entry
		    إذا (len > 0 && ga_grow(&آخری->wq_ga, len) == OK)
		    {
			mch_memmove((حرف *)آخری->wq_ga.ga_انکڑے
							  + آخری->wq_ga.ga_len,
				    buf, len);
			آخری->wq_ga.ga_len += len;
		    }
		}
		وإلا
		{
		    writeq_T *آخری = ALLOC_ONE(writeq_T);

		    إذا (آخری != عدم)
		    {
			آخری->wq_prev = wq->wq_prev;
			آخری->wq_اگلا = عدم;
			إذا (wq->wq_prev == عدم)
			    wq->wq_اگلا = آخری;
			وإلا
			    wq->wq_prev->wq_اگلا = آخری;
			wq->wq_prev = آخری;
			ga_init2(&آخری->wq_ga, 1, 1000);
			إذا (len > 0 && ga_grow(&آخری->wq_ga, len) == OK)
			{
			    mch_memmove(آخری->wq_ga.ga_انکڑے, buf, len);
			    آخری->wq_ga.ga_len = len;
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

    clear_کم_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    إذا (channel == عدم)
	أرجع عدم;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    إذا (get_کم_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
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
	channel_سیٹ_req_callback(channel, *part_read, &opt->jo_callback, id);
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
ch_اظہاریہ_common(typval_T *argvars, typval_T *rettv, صحيح eval)
{
    char_u	*متن;
    typval_T	*listtv;
    channel_T	*channel;
    صحيح		id;
    ch_منوال_T	ch_منوال;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    صحيح		timeout;
    صحيح		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = عدم;

    إذا (وچ_vim9script()
	    && (check_for_chan_or_کم_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    إذا (channel == عدم)
	أرجع;
    part_send = channel_part_send(channel);

    ch_منوال = channel_get_منوال(channel, part_send);
    إذا (ch_منوال == CH_منوال_RAW || ch_منوال == CH_منوال_BLOB
	    || ch_منوال == CH_منوال_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	أرجع;
    }

    إذا (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	حرف		*کلید = ch_منوال == CH_منوال_LSP ? "id" : "seq";

	// return an empty dict by default
	إذا (rettv_dict_alloc(rettv) == FAIL)
	    أرجع;

	إذا (check_for_dict_arg(argvars, 1) == FAIL)
	    أرجع;

	d = argvars[1].vval.v_dict;
	di = dict_لبھو(d, (char_u *)کلید, -1);
	إذا (argvars[2].v_type == VAR_DICT
		&& dict_has_کلید(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	إذا (di != عدم && di->di_tv.v_type != VAR_نمبر
		&& (ch_منوال == CH_منوال_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_قدر_for_دلیل_str), کلید);
	    أرجع;
	}

	إذا (ch_منوال == CH_منوال_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_آخری_پیغام_id;
	    إذا (di == عدم)
		dict_جوڑو_نمبر(d, "seq", id);
	    وإلا
		di->di_tv.vval.v_نمبر = id;
	}
	وإلا إذا (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_آخری_پیغام_id;
	    إذا (di == عدم)
		dict_جوڑو_نمبر(d, "id", id);
	    وإلا
		di->di_tv.vval.v_نمبر = id;
	}
	وإلا
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    إذا (di != عدم && di->di_tv.v_type == VAR_نمبر)
		id = di->di_tv.vval.v_نمبر;
	}
	إذا (ch_منوال == CH_منوال_LSP && !dict_has_کلید(d, "jsonrpc"))
	    dict_جوڑو_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	متن = json_encode_lsp_پیغام(&argvars[1]);
    }
    وإلا
    {
	id = ++channel->ch_آخری_پیغام_id;
	متن = json_encode_nr_اظہاریہ(id, &argvars[1],
			      (ch_منوال == CH_منوال_JS ? JSON_JS : 0) | JSON_NL);
    }
    إذا (متن == عدم)
	أرجع;

    channel = send_common(argvars, متن, (صحيح)STRLEN(متن), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(متن);
    إذا (channel != عدم && eval)
    {
	إذا (opt.jo_سیٹ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	وإلا
	    timeout = channel_get_timeout(channel, part_read);
	إذا (channel_read_json_بلاک(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    إذا (ch_منوال == CH_منوال_LSP || ch_منوال == CH_منوال_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_نمبر;
		free_tv(listtv);
	    }
	    وإلا
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
    free_کم_options(&opt);
    إذا (ch_منوال == CH_منوال_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	إذا (rettv->vval.v_dict != عدم)
	    dict_جوڑو_نمبر(rettv->vval.v_dict, "seq", id);
    }
    وإلا إذا (ch_منوال == CH_منوال_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	إذا (rettv->vval.v_dict != عدم)
	    dict_جوڑو_نمبر(rettv->vval.v_dict, "id", id);
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

    إذا (وچ_vim9script()
	    && (check_for_chan_or_کم_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	أرجع;

    إذا (argvars[1].v_type == VAR_BLOB)
    {
	متن = argvars[1].vval.v_blob->bv_ga.ga_انکڑے;
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
	إذا (opt.jo_سیٹ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	وإلا
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_بلاک(channel, part_read,
							timeout, TRUE, عدم);
    }
    free_کم_options(&opt);
}

#تعریف_کرو KEEP_OPEN_ویلا 20  // msec

#إذا defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    صحيح
channel_poll_تیاری(صحيح nfd_وچ, فراغ *fds_وچ, صحيح *towait)
{
    صحيح		nfd = nfd_وچ;
    channel_T	*channel;
    بنية	pollfd *fds = fds_وچ;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    إذا (ch_part->ch_fd != INVALID_FD)
	    {
		إذا (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    إذا (*towait < 0 || *towait > KEEP_OPEN_ویلا)
			*towait = KEEP_OPEN_ویلا;
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
channel_poll_check(صحيح ret_وچ, فراغ *fds_وچ)
{
    صحيح		ret = ret_وچ;
    channel_T	*channel;
    بنية	pollfd *fds = fds_وچ;
    ch_part_T	part;
    صحيح		idx;
    chanpart_T	*وچ_part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
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

	وچ_part = &channel->ch_part[PART_وچ];
	idx = وچ_part->ch_poll_idx;
	إذا (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_داخلہ(channel);
	    --ret;
	}
    }

    أرجع ret;
}
#جے_انت // UNIX && !HAVE_SELECT

#إذا !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    صحيح
channel_select_تیاری(
	صحيح maxfd_وچ,
	فراغ *rfds_وچ,
	فراغ *wfds_وچ,
	بنية timeval *tv,
	بنية timeval **tvp)
{
    صحيح		maxfd = maxfd_وچ;
    channel_T	*channel;
    fd_set	*rfds = rfds_وچ;
    fd_set	*wfds = wfds_وچ;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
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
					|| tv->tv_usec > KEEP_OPEN_ویلا * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_ویلا * 1000;
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
channel_select_check(صحيح ret_وچ, فراغ *rfds_وچ, فراغ *wfds_وچ)
{
    صحيح		ret = ret_وچ;
    channel_T	*channel;
    fd_set	*rfds = rfds_وچ;
    fd_set	*wfds = wfds_وچ;
    ch_part_T	part;
    chanpart_T	*وچ_part;

    FOR_ALL_CHANNELS(channel)
    {
	لكل (part = PART_SOCK; part < PART_وچ; ++part)
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

	وچ_part = &channel->ch_part[PART_وچ];
	إذا (ret > 0 && وچ_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (صحيح)وچ_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(وچ_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(وچ_part->ch_fd, wfds);
	    channel_write_داخلہ(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	إذا (channel->ch_کم != عدم)
	{
	    کم_T *کم = channel->ch_کم;

	    إذا (کم->jv_tty_باہرلا != عدم && کم->jv_حالت == کم_مکمل)
		لكل (part = PART_SOCK; part < PART_گنتی; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    أرجع ret;
}
#جے_انت // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ساكن فراغ
channel_wait_any(لما msec)
{
#جے_تعریف_شدہ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    إذا (stdio_channel->CH_باہرلا_FD != INVALID_FD)
	(فراغ)channel_wait(stdio_channel, stdio_channel->CH_باہرلا_FD,
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
    maxfd = channel_select_تیاری(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, عدم, tvp);
    إذا (ret >= 0)
	(فراغ)channel_select_check(ret, &rfds, &wfds);
#وإلا
    بنية pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    صحيح			nfd;
    صحيح			towait = (صحيح)msec;
    صحيح			ret;

    nfd = channel_poll_تیاری(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    إذا (ret >= 0)
	(فراغ)channel_poll_check(ret, fds);
#جے_انت
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
	لما	msec = 1000;
#جے_تعریف_شدہ FEAT_TIMERS
	لما	due_ویلا = check_due_timer();

	إذا (due_ویلا > 0 && due_ویلا < msec)
	    msec = due_ویلا;
#جے_انت
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
    channel_T	*channel = پہلا_channel;
    صحيح		ret = FALSE;
    صحيح		r;
    ch_part_T	part = PART_SOCK;
    ساكن صحيح	recursive = 0;
#جے_تعریف_شدہ ELAPSED_FUNC
    elapsed_T	شروع_tv;
#جے_انت

    // The code below may invoke callbacks, which might call us back.
    // In a recursive call channels will not be closed.
    ++recursive;
    ++محفوظ_منزل_invoke_callback;

#جے_تعریف_شدہ ELAPSED_FUNC
    ELAPSED_INIT(شروع_tv);
#جے_انت

    // Only do this message when another message was given, otherwise we get
    // lots of them.
    إذا ((did_repeated_پیغام & REPEATED_پیغام_LOOKING) == 0)
    {
	ch_log(عدم, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_پیغام = REPEATED_پیغام_LOOKING;
    }
    طالما (channel != عدم)
    {
	إذا (recursive == 1)
	{
	    إذا (channel_can_close(channel))
	    {
		channel->ch_منزل_be_closed = (1U << PART_گنتی);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = پہلا_channel;
		استمر;
	    }
	    إذا (channel->ch_منزل_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		إذا (channel->ch_کم != عدم)
		    channel->ch_کم->jv_channel = عدم;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = پہلا_channel;
		استمر;
	    }
	    إذا (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = پہلا_channel;
		part = PART_SOCK;
		استمر;
	    }
	}

#جے_تعریف_شدہ FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	إذا (part == PART_باہرلا
		&& channel->ch_کم != عدم
		&& channel->ch_کم->jv_tty_باہرلا != عدم
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    استمر;
	}
#جے_انت
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
#جے_تعریف_شدہ ELAPSED_FUNC
			// Limit the time we loop here to 100 msec, otherwise
			// Vim becomes unresponsive when the callback takes
			// more than a bit of time.
			&& ELAPSED_FUNC(شروع_tv) < 100L
#جے_انت
			))
	    {
		// channel was freed or something was done, start over
		channel = پہلا_channel;
		part = PART_SOCK;
		استمر;
	    }
	}
	إذا (part < PART_ERR)
	    ++part;
	وإلا
	{
	    channel = channel->ch_اگلا;
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
    channel_T	*channel = پہلا_channel;
    ch_part_T	part = PART_SOCK;

    طالما (channel != عدم)
    {
	إذا (channel_has_readahead(channel, part))
	    أرجع TRUE;
	إذا (part < PART_ERR)
	    ++part;
	وإلا
	{
	    channel = channel->ch_اگلا;
	    part = PART_SOCK;
	}
    }
    أرجع FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    صحيح
سیٹ_ref_وچ_channel(صحيح copyID)
{
    صحيح		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    لكل (channel = پہلا_channel; !abort && channel != عدم;
						   channel = channel->ch_اگلا)
	إذا (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || سیٹ_ref_وچ_شے(&tv, copyID, عدم, عدم, عدم);
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
	أرجع PART_وچ;
    أرجع PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ساكن ch_part_T
channel_part_read(channel_T *channel)
{
    إذا (channel->CH_SOCK_FD == INVALID_FD)
	أرجع PART_باہرلا;
    أرجع PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ساكن ch_منوال_T
channel_get_منوال(channel_T *channel, ch_part_T part)
{
    إذا (channel == عدم)
	أرجع CH_منوال_JSON;
    أرجع channel->ch_part[part].ch_منوال;
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

    rettv->vval.v_نمبر = 0;
    إذا (وچ_vim9script() && check_for_chan_or_کم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel != عدم)
	rettv->vval.v_نمبر = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_باہرلا)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    فراغ
f_ch_close(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    channel_T *channel;

    إذا (وچ_vim9script() && check_for_chan_or_کم_arg(argvars, 0) == FAIL)
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
f_ch_close_وچ(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    channel_T *channel;

    إذا (وچ_vim9script() && check_for_chan_or_کم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    إذا (channel != عدم)
	channel_close_وچ(channel);
}

/*
 * "ch_getbufnr()" function
 */
    فراغ
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_نمبر = -1;

    إذا (وچ_vim9script()
	    && (check_for_chan_or_کم_arg(argvars, 0) == FAIL
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
	part = PART_باہرلا;
    وإلا إذا (STRCMP(what, "in") == 0)
	part = PART_وچ;
    وإلا
	part = PART_SOCK;
    إذا (channel->ch_part[part].ch_bufref.br_buf != عدم)
	rettv->vval.v_نمبر =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    فراغ
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    إذا (وچ_vim9script() && check_for_chan_or_کم_arg(argvars, 0) == FAIL)
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel == عدم)
	أرجع;

    rettv->v_type = VAR_کم;
    rettv->vval.v_کم = channel->ch_کم;
    إذا (channel->ch_کم != عدم)
	++channel->ch_کم->jv_refcount;
}

/*
 * "ch_info()" function
 */
    فراغ
f_ch_info(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    channel_T *channel;

    إذا (وچ_vim9script() && check_for_chan_or_کم_arg(argvars, 0) == FAIL)
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
    ch_اظہاریہ_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    فراغ
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_اظہاریہ_common(argvars, rettv, FALSE);
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
f_ch_setoptions(typval_T *argvars, typval_T *rettv بن_ورتیا)
{
    channel_T	*channel;
    jobopt_T	opt;

    إذا (check_restricted() || check_secure())
	أرجع;

    إذا (وچ_vim9script()
	    && (check_for_chan_or_کم_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    إذا (channel == عدم)
	أرجع;
    clear_کم_options(&opt);
    إذا (get_کم_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_منوال_ALL, 0) == OK)
	channel_سیٹ_options(channel, &opt);
    free_کم_options(&opt);
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

    إذا (وچ_vim9script()
	    && (check_for_chan_or_کم_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	أرجع;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    إذا (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_کم_options(&opt);
	إذا (get_کم_options(&argvars[1], &opt, JO_PART, 0) == OK
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
channel_لبھو(صحيح ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	إذا (ch->ch_id == ch_id)
	    أرجع ch;
    أرجع عدم;
}

#جے_انت // FEAT_JOB_CHANNEL
