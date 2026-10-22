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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_పని_CHANNEL)

// TRUE when netbeans is running with a GUI.
#నిర్వచితమైతే FEAT_GUI
# define CH_HAS_GUI (gui.లోపల_use || gui.starting)
#అయితే_ముగింపు

// Note: when making changes here also adjust configure.ac.
#నిర్వచితమైతే MSWIN
// WinSock API is separated from C API, thus we can't use read(), write(),
// errno...
# define SOCK_ERRNO errno = WSAGet_చివరి_Error()
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
# define UNIX_మార్గం_MAX 108
రకనిర్వచనం నిర్మాణం sockaddr_un {
    చిరునామా_FAMILY sun_family;
    అక్షరం sun_మార్గం[UNIX_మార్గం_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#లేకపోతే
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) లిఖ(sd, buf, len)
# define sock_read(sd, buf, len) పఠ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) పఠ(fd, buf, len)
# define fd_write(sd, buf, len) లిఖ(sd, buf, len)
# define fd_close(sd) close(sd)
#అయితే_ముగింపు

స్థిర శూన్య channel_read(channel_T *channel, ch_part_T part, అక్షరం *func);
స్థిర శూన్య channel_wait_any(దీర్ఘ msec);
స్థిర ch_బహుళకం_T channel_get_బహుళకం(channel_T *channel, ch_part_T part);
స్థిర పూర్ణాంకం channel_get_timeout(channel_T *channel, ch_part_T part);
స్థిర channel_T *channel_open_stdio(శూన్య);
స్థిర ch_part_T channel_part_send(channel_T *channel);
స్థిర ch_part_T channel_part_read(channel_T *channel);

#నిర్వచించు FOR_ALL_CHANNELS(ch) \
    కొరకు ((ch) = మొదటి_channel; (ch) != ఖాళీ; (ch) = (ch)->ch_తదుపరి)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
స్థిర పూర్ణాంకం సురక్షిత_గమ్యం_invoke_callback = 0;

#నిర్వచితమైతే MSWIN
    స్థిర పూర్ణాంకం
fd_read(sock_T fd, అక్షరం *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ఒకవేళ (!ReadFile(h, buf, (DWORD)len, &nread, ఖాళీ))
	తిరిగి -1;
    తిరిగి (పూర్ణాంకం)nread;
}

    స్థిర పూర్ణాంకం
fd_write(sock_T fd, అక్షరం *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, కొలత, పూర్తి = 0;
    OVERLAPPED	ov;

    అయితే (todo > 0)
    {
	ఒకవేళ (todo > MAX_NAMED_PIPE_కొలత)
	    కొలత = MAX_NAMED_PIPE_కొలత;
	లేకపోతే
	    కొలత = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_క్షేత్రం(ov);
	nwrite = 0;
	ఒకవేళ (!WriteFile(h, buf + పూర్తి, కొలత, &nwrite, &ov))
	{
	    DWORD err = Get_చివరి_Error();

	    ఒకవేళ (err != ERROR_IO_PENDING)
		తిరిగి -1;
	    ఒకవేళ (!Get_Overlapped_ఫలితం(h, &ov, &nwrite, FALSE))
		తిరిగి -1;
	    FlushFileBuffers(h);
	}
	లేకపోతే ఒకవేళ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    ఆపు;
	todo -= nwrite;
	పూర్తి += nwrite;
    }
    తిరిగి (పూర్ణాంకం)పూర్తి;
}

    స్థిర శూన్య
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
# undef PERROR
# define PERROR(సందేశం) (శూన్య)semsg("%s: %s", సందేశం, strerror_win32(errno))

    స్థిర అక్షరం *
strerror_win32(పూర్ణాంకం eno)
{
    స్థిర LPVOID msgbuf = ఖాళీ;
    char_u *ptr;

    ఒకవేళ (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ఖాళీ;
    }
    Format_సందేశం(
	FORMAT_సందేశం_ALLOCATE_బఫరు |
	FORMAT_సందేశం_FROM_SYSTEM |
	FORMAT_సందేశం_IGNORE_INSERTS,
	ఖాళీ,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ఖాళీ);
    ఒకవేళ (msgbuf != ఖాళీ)
	// chomp \r or \n
	కొరకు (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    ఎంపిక (*ptr)
	    {
		స్థితి '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    ఆపు;
		స్థితి '\n':
		    ఒకవేళ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    లేకపోతే
			*ptr = ' ';
		    ఆపు;
	    }
    తిరిగి msgbuf;
}
#అయితే_ముగింపు

/*
 * The list of all allocated channels.
 */
స్థిర channel_T *మొదటి_channel = ఖాళీ;
స్థిర పూర్ణాంకం తదుపరి_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
కూడు_channel(శూన్య)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    ఒకవేళ (channel == ఖాళీ)
	తిరిగి ఖాళీ;

    channel->ch_id = తదుపరి_ch_id++;
    ch_log(channel, "Created channel");

    కొరకు (part = PART_SOCK; part < PART_లెక్క; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#నిర్వచితమైతే FEAT_GUI_X11
	channel->ch_part[part].ch_ఆదానం_Handler = (Xt_ఆదానం_Id)ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_GTK
	channel->ch_part[part].ch_ఆదానం_Handler = 0;
#అయితే_ముగింపు
	channel->ch_part[part].ch_timeout = 2000;
    }

    ఒకవేళ (మొదటి_channel != ఖాళీ)
    {
	మొదటి_channel->ch_prev = channel;
	channel->ch_తదుపరి = మొదటి_channel;
    }
    మొదటి_channel = channel;

    channel->ch_refcount = 1;
    తిరిగి channel;
}

    పూర్ణాంకం
has_any_channel(శూన్య)
{
    తిరిగి మొదటి_channel != ఖాళీ;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    పూర్ణాంకం
channel_still_useful(channel_T *channel)
{
    పూర్ణాంకం has_sock_సందేశం;
    పూర్ణాంకం	has_బహిర్గతం_సందేశం;
    పూర్ణాంకం	has_err_సందేశం;

    // If the job was killed the channel is not expected to work anymore.
    ఒకవేళ (channel->ch_పని_killed && channel->ch_పని == ఖాళీ)
	తిరిగి FALSE;

    // If there is a close callback it may still need to be invoked.
    ఒకవేళ (channel->ch_close_cb.cb_name != ఖాళీ)
	తిరిగి TRUE;

    // If reading from or a buffer it's still useful.
    ఒకవేళ (channel->ch_part[PART_లోపల].ch_bufref.br_buf != ఖాళీ)
	తిరిగి TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_సందేశం = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_తల.rq_తదుపరి != ఖాళీ
		|| channel->ch_part[PART_SOCK].ch_json_తల.jq_తదుపరి != ఖాళీ;
    has_బహిర్గతం_సందేశం = channel->ch_part[PART_బహిర్గతం].ch_fd != INVALID_FD
		  || channel->ch_part[PART_బహిర్గతం].ch_తల.rq_తదుపరి != ఖాళీ
		  || channel->ch_part[PART_బహిర్గతం].ch_json_తల.jq_తదుపరి != ఖాళీ;
    has_err_సందేశం = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_తల.rq_తదుపరి != ఖాళీ
		  || channel->ch_part[PART_ERR].ch_json_తల.jq_తదుపరి != ఖాళీ;
    తిరిగి (channel->ch_callback.cb_name != ఖాళీ && (has_sock_సందేశం
		|| has_బహిర్గతం_సందేశం || has_err_సందేశం))
	    || ((channel->ch_part[PART_బహిర్గతం].ch_callback.cb_name != ఖాళీ
		       || channel->ch_part[PART_బహిర్గతం].ch_bufref.br_buf != ఖాళీ)
		    && has_బహిర్గతం_సందేశం)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ఖాళీ
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ఖాళీ)
		    && has_err_సందేశం);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    పూర్ణాంకం
channel_can_close(channel_T *channel)
{
    తిరిగి channel->ch_గమ్యం_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    స్థిర శూన్య
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    స్థిర శూన్య
channel_free_channel(channel_T *channel)
{
    ఒకవేళ (channel->ch_తదుపరి != ఖాళీ)
	channel->ch_తదుపరి->ch_prev = channel->ch_prev;
    ఒకవేళ (channel->ch_prev == ఖాళీ)
	మొదటి_channel = channel->ch_తదుపరి;
    లేకపోతే
	channel->ch_prev->ch_తదుపరి = channel->ch_తదుపరి;
    vim_free(channel);
}

    స్థిర శూన్య
channel_free(channel_T *channel)
{
    ఒకవేళ (లోపల_free_unref_వస్తువులు)
	తిరిగి;

    ఒకవేళ (సురక్షిత_గమ్యం_invoke_callback == 0)
	channel->ch_గమ్యం_be_freed = TRUE;
    లేకపోతే
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
    స్థిర పూర్ణాంకం
channel_may_free(channel_T *channel)
{
    ఒకవేళ (!channel_still_useful(channel))
    {
	channel_free(channel);
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    పూర్ణాంకం
channel_unref(channel_T *channel)
{
    ఒకవేళ (channel != ఖాళీ && --channel->ch_refcount <= 0)
	తిరిగి channel_may_free(channel);
    తిరిగి FALSE;
}

    పూర్ణాంకం
free_వాడని_channels_contents(పూర్ణాంకం copyID, పూర్ణాంకం ముసుగు)
{
    పూర్ణాంకం		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++సురక్షిత_గమ్యం_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ఒకవేళ (!channel_still_useful(ch)
				 && (ch->ch_copyID & ముసుగు) != (copyID & ముసుగు))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --సురక్షిత_గమ్యం_invoke_callback;
    తిరిగి did_free;
}

    శూన్య
free_వాడని_channels(పూర్ణాంకం copyID, పూర్ణాంకం ముసుగు)
{
    channel_T	*ch;
    channel_T	*ch_తదుపరి;

    కొరకు (ch = మొదటి_channel; ch != ఖాళీ; ch = ch_తదుపరి)
    {
	ch_తదుపరి = ch->ch_తదుపరి;
	ఒకవేళ (!channel_still_useful(ch)
				 && (ch->ch_copyID & ముసుగు) != (copyID & ముసుగు))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ఒకవేళ defined(FEAT_GUI)

# ఒకవేళ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    స్థిర channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ఒకవేళ (fd == INVALID_FD)
	తిరిగి ఖాళీ;

    FOR_ALL_CHANNELS(channel)
    {
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	    ఒకవేళ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		తిరిగి channel;
	    }
    }
    తిరిగి ఖాళీ;
}

    స్థిర శూన్య
channel_read_fd(పూర్ణాంకం fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ఒకవేళ (channel == ఖాళీ)
	ch_error(ఖాళీ, "Channel for fd %d not found", fd);
    లేకపోతే
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    స్థిర శూన్య
సందేశం_From_Server_X11(Xt_సూచిక client_దత్తాంశం,
		  పూర్ణాంకం *unused1 వాడని,
		  Xt_ఆదానం_Id *unused2 వాడని)
{
    channel_read_fd((పూర్ణాంకం)(దీర్ఘ)client_దత్తాంశం);
}
# endif

# ifdef FEAT_GUI_GTK
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర gboolean
సందేశం_From_Server_Gtk3(GIOChannel *unused1 వాడని,
		  GIOCondition unused2 వాడని,
		  gpointer client_దత్తాంశం)
{
    channel_read_fd(GPOINTER_గమ్యం_INT(client_దత్తాంశం));
    తిరిగి TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  లేకపోతే
    స్థిర శూన్య
సందేశం_From_Server_Gtk2(gpointer client_దత్తాంశం,
		  gint unused1 వాడని,
		  Gdk_ఆదానం_Condition unused2 వాడని)
{
    channel_read_fd((పూర్ణాంకం)(దీర్ఘ)client_దత్తాంశం);
}
#  endif
# endif

    స్థిర శూన్య
channel_gui_రిజిస్టరు_one(channel_T *channel, ch_part_T part వాడని)
{
    ఒకవేళ (!CH_HAS_GUI)
	తిరిగి;

    // gets stuck in handling events for a not connected channel
    ఒకవేళ (channel->ch_keep_open)
	తిరిగి;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ఒకవేళ (channel->ch_part[part].ch_ఆదానం_Handler == (Xt_ఆదానం_Id)ఖాళీ)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_పేర్లు[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ఆదానం_Handler = Xt_App_కూడు_ఆదానం(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(Xt_సూచిక)(Xt_ఆదానం_Read_ముసుగు + Xt_ఆదానం_Except_ముసుగు),
		సందేశం_From_Server_X11,
		(Xt_సూచిక)(దీర్ఘ)channel->ch_part[part].ch_fd);
    }
# లేకపోతే
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ఒకవేళ (channel->ch_part[part].ch_ఆదానం_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_పేర్లు[part], channel->ch_part[part].ch_fd);
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ఆదానం_Handler = g_io_కూడు_watch(
		chnnl,
		G_IO_లోపల|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		సందేశం_From_Server_Gtk3,
		GINT_గమ్యం_సూచిక(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   లేకపోతే
	channel->ch_part[part].ch_ఆదానం_Handler = gdk_ఆదానం_కూడు(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_ఆదానం_Condition)
			     ((పూర్ణాంకం)GDK_ఆదానం_READ + (పూర్ణాంకం)GDK_ఆదానం_EXCEPTION),
		సందేశం_From_Server_Gtk2,
		(gpointer)(దీర్ఘ)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    స్థిర శూన్య
channel_gui_రిజిస్టరు(channel_T *channel)
{
    ఒకవేళ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_రిజిస్టరు_one(channel, PART_SOCK);
    ఒకవేళ (channel->CH_బహిర్గతం_FD != INVALID_FD
	    && channel->CH_బహిర్గతం_FD != channel->CH_SOCK_FD)
	channel_gui_రిజిస్టరు_one(channel, PART_బహిర్గతం);
    ఒకవేళ (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_బహిర్గతం_FD)
	channel_gui_రిజిస్టరు_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    శూన్య
channel_gui_రిజిస్టరు_all(శూన్య)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_రిజిస్టరు(channel);
}

    స్థిర శూన్య
channel_gui_unregister_one(channel_T *channel వాడని, ch_part_T part వాడని)
{
# ifdef FEAT_GUI_X11
    ఒకవేళ (channel->ch_part[part].ch_ఆదానం_Handler != (Xt_ఆదానం_Id)ఖాళీ)
    {
	ch_log(channel, "Unregistering part %s", ch_part_పేర్లు[part]);
	Xt_Remove_ఆదానం(channel->ch_part[part].ch_ఆదానం_Handler);
	channel->ch_part[part].ch_ఆదానం_Handler = (Xt_ఆదానం_Id)ఖాళీ;
    }
# లేకపోతే
#  ifdef FEAT_GUI_GTK
    ఒకవేళ (channel->ch_part[part].ch_ఆదానం_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_పేర్లు[part]);
#   ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	g_ఆధారం_remove(channel->ch_part[part].ch_ఆదానం_Handler);
#   లేకపోతే
	gdk_ఆదానం_remove(channel->ch_part[part].ch_ఆదానం_Handler);
#   endif
	channel->ch_part[part].ch_ఆదానం_Handler = 0;
    }
#  endif
# endif
}

    స్థిర శూన్య
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	channel_gui_unregister_one(channel, part);
}

#అయితే_ముగింపు  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    స్థిర పూర్ణాంకం
channel_connect(
	channel_T *channel,
	స్థిరరాశి నిర్మాణం sockaddr *server_addr,
	పూర్ణాంకం server_addrlen,
	పూర్ణాంకం *waittime)
{
    పూర్ణాంకం		sd = -1;
#నిర్వచితమైతే MSWIN
    u_long	val = 1;
#అయితే_ముగింపు

    అయితే (TRUE)
    {
	దీర్ఘ	elapsed_msec = 0;
	పూర్ణాంకం	waitnow;
	పూర్ణాంకం	ret;

	ఒకవేళ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ఒకవేళ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_లోపల_channel_connect));
	    తిరిగి -1;
	}

	ఒకవేళ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ఒకవేళ (
#నిర్వచితమైతే MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#లేకపోతే
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#అయితే_ముగింపు
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		తిరిగి -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ఒకవేళ (ret == 0)
	    // The connection could be established.
	    ఆపు;

	SOCK_ERRNO;
	ఒకవేళ (*waittime < 0 || (errno != EWOULDBLOCK
		&& errno != ECONNREFUSED
#నిర్వచితమైతే EINPROGRESS
		&& errno != EINPROGRESS
#అయితే_ముగింపు
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
	    PERROR(_(e_cannot_connect_గమ్యం_పోర్టు));
	    sock_close(sd);
	    తిరిగి -1;
	}
	లేకపోతే ఒకవేళ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    తిరిగి -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    నిర్మాణం timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#నిర్వచితం_కాకపోతే MSWIN
	    పూర్ణాంకం			so_error = 0;
	    socklen_t		so_error_len = పరిమాణం(so_error);
	    నిర్మాణం timeval	ప్రారంభం_tv;
	    నిర్మాణం timeval	చివర_tv;
#అయితే_ముగింపు
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#నిర్వచితం_కాకపోతే MSWIN
	    gettimeofday(&ప్రారంభం_tv, ఖాళీ);
#అయితే_ముగింపు
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ఖాళీ, &tv);
	    ఒకవేళ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_గమ్యం_పోర్టు));
		sock_close(sd);
		తిరిగి -1;
	    }

#నిర్వచితమైతే MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ఒకవేళ (FD_ISSET(sd, &wfds))
		ఆపు;
	    elapsed_msec = waitnow;
	    ఒకవేళ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		కొనసాగు;
	    }
#లేకపోతే
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ఒకవేళ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ఒకవేళ (ret < 0 || (so_error != 0
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
		    PERROR(_(e_cannot_connect_గమ్యం_పోర్టు));
		    sock_close(sd);
		    తిరిగి -1;
		}
		లేకపోతే ఒకవేళ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    తిరిగి -1;
		}
	    }

	    ఒకవేళ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		ఆపు;

	    gettimeofday(&చివర_tv, ఖాళీ);
	    elapsed_msec = (చివర_tv.tv_sec - ప్రారంభం_tv.tv_sec) * 1000
				 + (చివర_tv.tv_usec - ప్రారంభం_tv.tv_usec) / 1000;
#అయితే_ముగింపు
	}

#నిర్వచితం_కాకపోతే MSWIN
	ఒకవేళ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ఒకవేళ (waitnow > 0)
	    {
		mch_delay((దీర్ఘ)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ఒకవేళ (!got_int)
	    {
		ఒకవేళ (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		కొనసాగు;
	    }
	    // we were interrupted, behave as if timed out
	}
#అయితే_ముగింపు

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	తిరిగి -1;
    }

    ఒకవేళ (*waittime >= 0)
    {
#నిర్వచితమైతే MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#లేకపోతే
	(శూన్య)fcntl(sd, F_SETFL, 0);
#అయితే_ముగింపు
    }

    తిరిగి sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	స్థిరరాశి అక్షరం *మార్గం,
	శూన్య (*nb_close_cb)(శూన్య))
{
    channel_T		*channel = ఖాళీ;
    పూర్ణాంకం			sd = -1;
    size_t		మార్గం_len = STRLEN(మార్గం);
    నిర్మాణం sockaddr_un	server;
    size_t		server_len;
    పూర్ణాంకం			waittime = -1;

    ఒకవేళ (*మార్గం == NUL || మార్గం_len >= పరిమాణం(server.sun_మార్గం))
    {
	semsg(_(e_invalid_వాదం_str), మార్గం);
	తిరిగి ఖాళీ;
    }

#నిర్వచితమైతే MSWIN
    channel_init_winsock();
#అయితే_ముగింపు

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
    {
	ch_error(ఖాళీ, "Cannot allocate channel.");
	తిరిగి ఖాళీ;
    }

    CLEAR_క్షేత్రం(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_మార్గం, మార్గం, పరిమాణం(server.sun_మార్గం) - 1);

    ch_log(channel, "Trying to connect to %s", మార్గం);

    server_len = offsetof(నిర్మాణం sockaddr_un, sun_మార్గం) + మార్గం_len + 1;
    sd = channel_connect(channel, (నిర్మాణం sockaddr *)&server, (పూర్ణాంకం)server_len,
								   &waittime);

    ఒకవేళ (sd < 0)
    {
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (అక్షరం *)vim_strsave((char_u *)మార్గం);
    channel->ch_పోర్టు = 0;
    channel->ch_గమ్యం_be_closed |= (1U << PART_SOCK);

#నిర్వచితమైతే FEAT_GUI
    channel_gui_రిజిస్టరు_one(channel, PART_SOCK);
#అయితే_ముగింపు

    తిరిగి channel;
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
	స్థిరరాశి అక్షరం *hostname,
	పూర్ణాంకం పోర్టు,
	పూర్ణాంకం waittime,
	శూన్య (*nb_close_cb)(శూన్య))
{
    పూర్ణాంకం			sd = -1;
    channel_T		*channel = ఖాళీ;
#నిర్వచితమైతే FEAT_IPV6
    పూర్ణాంకం			err;
    నిర్మాణం addrinfo	hints;
    నిర్మాణం addrinfo	*res = ఖాళీ;
    నిర్మాణం addrinfo	*addr = ఖాళీ;
#లేకపోతే
    నిర్మాణం sockaddr_లోపల	server;
    నిర్మాణం hostent	*host = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితమైతే MSWIN
    channel_init_winsock();
#అయితే_ముగింపు

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
    {
	ch_error(ఖాళీ, "Cannot allocate channel.");
	తిరిగి ఖాళీ;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#నిర్వచితమైతే FEAT_IPV6
    CLEAR_క్షేత్రం(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ఒకవేళ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ఒకవేళ ((err = getaddrinfo(hostname, ఖాళీ, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_లోపల_channel_open_str), gai_strerror(err));
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    // Count the number of addresses for timeout distribution
    పూర్ణాంకం addr_లెక్క = 0;
    కొరకు (addr = res; addr != ఖాళీ; addr = addr->ai_తదుపరి)
	addr_లెక్క++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ఒకవేళ (waittime == 0)
	waittime = 15;

    పూర్ణాంకం addr_సూచీ = 0;
    కొరకు (addr = res; addr != ఖాళీ; addr = addr->ai_తదుపరి, addr_సూచీ++)
    {
	స్థిరరాశి అక్షరం  *dst = hostname;
# ifdef HAVE_INET_NTOP
	స్థిరరాశి శూన్య  *src = ఖాళీ;
	అక్షరం	    buf[NUMBUFLEN];
# endif
	పూర్ణాంకం	    try_waittime;
	పూర్ణాంకం	    before_waittime;
	పూర్ణాంకం	    consumed;
	పూర్ణాంకం	    మిగిలిన_addrs;

	ఒకవేళ (addr->ai_family == AF_INET6)
	{
	    నిర్మాణం sockaddr_in6 *sai = (నిర్మాణం sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_పోర్టు = htons(పోర్టు);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	లేకపోతే ఒకవేళ (addr->ai_family == AF_INET)
	{
	    నిర్మాణం sockaddr_లోపల *sai = (నిర్మాణం sockaddr_లోపల *)addr->ai_addr;

	    sai->sin_పోర్టు = htons(పోర్టు);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ఒకవేళ (src != ఖాళీ)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, పరిమాణం(buf));
	    ఒకవేళ (dst == ఖాళీ)
		dst = hostname;
	    లేకపోతే ఒకవేళ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, పోర్టు);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ఒకవేళ (addr->ai_తదుపరి == ఖాళీ)
	    try_waittime = waittime;
	లేకపోతే ఒకవేళ (addr_సూచీ == 0)
	{
	    ఒకవేళ (waittime > 500)
		try_waittime = 250;
	    లేకపోతే ఒకవేళ (waittime > 30)
		try_waittime = waittime / 2;
	    లేకపోతే
		try_waittime = waittime;
	}
	లేకపోతే
	{
	    మిగిలిన_addrs = addr_లెక్క - addr_సూచీ;
	    try_waittime = waittime / మిగిలిన_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (పూర్ణాంకం)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ఒకవేళ (waittime < 0)
	    waittime = 0;

	ఒకవేళ (sd >= 0)
	    ఆపు;

	// If we have no time left, stop trying
	ఒకవేళ (waittime <= 0 && addr->ai_తదుపరి != ఖాళీ)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    ఆపు;
	}
    }

    freeaddrinfo(res);
#లేకపోతే
    CLEAR_క్షేత్రం(server);
    server.sin_family = AF_INET;
    server.sin_పోర్టు = htons(పోర్టు);
    ఒకవేళ ((host = gethostbyname(hostname)) == ఖాళీ)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_లోపల_channel_open));
	channel_free(channel);
	తిరిగి ఖాళీ;
    }
    {
	అక్షరం *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	జ్ఞాపకం_నకలు(&p, &host->h_addr_list[0], పరిమాణం(p));
	జ్ఞాపకం_నకలు((అక్షరం *)&server.sin_addr, p, host->h_నిడివి);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, పోర్టు);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    ఒకవేళ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (నిర్మాణం sockaddr *)&server, పరిమాణం(server),
								   &waittime);
#అయితే_ముగింపు

    ఒకవేళ (sd < 0)
    {
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (అక్షరం *)vim_strsave((char_u *)hostname);
    channel->ch_పోర్టు = పోర్టు;
    channel->ch_గమ్యం_be_closed |= (1U << PART_SOCK);

#నిర్వచితమైతే FEAT_GUI
    channel_gui_రిజిస్టరు_one(channel, PART_SOCK);
#అయితే_ముగింపు

    తిరిగి channel;
}

    స్థిర శూన్య
free_సమితి_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ఒకవేళ (callback->cb_name != ఖాళీ && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    లేకపోతే
	cbp->cb_name = ఖాళీ;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	స్థిర శూన్య
prepare_బఫరు(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#నిర్వచితమైతే FEAT_QUICKFIX
    సమితి_option_విలువ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    సమితి_option_విలువ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#అయితే_ముగింపు
    ఒకవేళ (curbuf->b_ml.ml_mfp == ఖాళీ)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    స్థిర buf_T *
channel_కనుగొను_బఫరు(char_u *name, పూర్ణాంకం err, పూర్ణాంకం సందేశం)
{
    buf_T *buf = ఖాళీ;
    buf_T *save_curbuf = curbuf;

    ఒకవేళ (name != ఖాళీ && *name != NUL)
    {
	buf = buflist_findname(name);
	ఒకవేళ (buf == ఖాళీ)
	    buf = buflist_findname_exp(name);
    }

    ఒకవేళ (buf != ఖాళీ)
	తిరిగి buf;

    buf = buflist_new(name == ఖాళీ || *name == NUL ? ఖాళీ : name,
	    ఖాళీ, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి ఖాళీ;
    prepare_బఫరు(buf);

    curbuf = buf;
    ఒకవేళ (సందేశం)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    మారింది_బైట్లు(1, 0);
    curbuf = save_curbuf;

    తిరిగి buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    స్థిర శూన్య
channel_సమితి_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ఒకవేళ (opt->jo_సమితి & JO_బహుళకం)
	కొరకు (part = PART_SOCK; part < PART_లెక్క; ++part)
	    channel->ch_part[part].ch_బహుళకం = opt->jo_బహుళకం;
    ఒకవేళ (opt->jo_సమితి & JO_లోపల_బహుళకం)
	channel->ch_part[PART_లోపల].ch_బహుళకం = opt->jo_లోపల_బహుళకం;
    ఒకవేళ (opt->jo_సమితి & JO_బహిర్గతం_బహుళకం)
	channel->ch_part[PART_బహిర్గతం].ch_బహుళకం = opt->jo_బహిర్గతం_బహుళకం;
    ఒకవేళ (opt->jo_సమితి & JO_ERR_బహుళకం)
	channel->ch_part[PART_ERR].ch_బహుళకం = opt->jo_err_బహుళకం;
    channel->ch_nonblock = opt->jo_noblock;

    ఒకవేళ (opt->jo_సమితి & JO_TIMEOUT)
	కొరకు (part = PART_SOCK; part < PART_లెక్క; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ఒకవేళ (opt->jo_సమితి & JO_బహిర్గతం_TIMEOUT)
	channel->ch_part[PART_బహిర్గతం].ch_timeout = opt->jo_బహిర్గతం_timeout;
    ఒకవేళ (opt->jo_సమితి & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ఒకవేళ (opt->jo_సమితి & JO_ఖండం_WRITE)
	channel->ch_part[PART_లోపల].ch_ఖండం_write = 1;

    ఒకవేళ (opt->jo_సమితి & JO_CALLBACK)
	free_సమితి_callback(&channel->ch_callback, &opt->jo_callback);
    ఒకవేళ (opt->jo_సమితి & JO_బహిర్గతం_CALLBACK)
	free_సమితి_callback(&channel->ch_part[PART_బహిర్గతం].ch_callback,
							      &opt->jo_బహిర్గతం_cb);
    ఒకవేళ (opt->jo_సమితి & JO_ERR_CALLBACK)
	free_సమితి_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ఒకవేళ (opt->jo_సమితి & JO_CLOSE_CALLBACK)
	free_సమితి_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ఒకవేళ ((opt->jo_సమితి & JO_బహిర్గతం_IO) && opt->jo_io[PART_బహిర్గతం] == JIO_బఫరు)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ఒకవేళ (!(opt->jo_సమితి & JO_బహిర్గతం_బహుళకం))
	    channel->ch_part[PART_బహిర్గతం].ch_బహుళకం = CH_బహుళకం_NL;
	ఒకవేళ (opt->jo_సమితి & JO_బహిర్గతం_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_బహిర్గతం]);
	    ఒకవేళ (buf == ఖాళీ)
		semsg(_(e_బఫరు_nr_does_not_exist),
					       (దీర్ఘ)opt->jo_io_buf[PART_బహిర్గతం]);
	}
	లేకపోతే
	{
	    పూర్ణాంకం సందేశం = TRUE;

	    ఒకవేళ (opt->jo_set2 & JO2_బహిర్గతం_సందేశం)
		సందేశం = opt->jo_సందేశం[PART_బహిర్గతం];
	    buf = channel_కనుగొను_బఫరు(opt->jo_io_name[PART_బహిర్గతం], FALSE, సందేశం);
	}
	ఒకవేళ (buf != ఖాళీ)
	{
	    ఒకవేళ (opt->jo_సమితి & JO_బహిర్గతం_MODIFIABLE)
		channel->ch_part[PART_బహిర్గతం].ch_nomodifiable =
						!opt->jo_modifiable[PART_బహిర్గతం];

	    ఒకవేళ (!buf->b_p_ma && !channel->ch_part[PART_బహిర్గతం].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    లేకపోతే
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (అక్షరం *)buf->b_ffname);
		సమితి_bufref(&channel->ch_part[PART_బహిర్గతం].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
		    prepare_బఫరు(buf);
	    }
	}
    }

    ఒకవేళ ((opt->jo_సమితి & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_బఫరు
	 || (opt->jo_io[PART_ERR] == JIO_బహిర్గతం && (opt->jo_సమితి & JO_బహిర్గతం_IO)
				       && opt->jo_io[PART_బహిర్గతం] == JIO_బఫరు)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ఒకవేళ (!(opt->jo_సమితి & JO_ERR_బహుళకం))
	    channel->ch_part[PART_ERR].ch_బహుళకం = CH_బహుళకం_NL;
	ఒకవేళ (opt->jo_io[PART_ERR] == JIO_బహిర్గతం)
	    buf = channel->ch_part[PART_బహిర్గతం].ch_bufref.br_buf;
	లేకపోతే ఒకవేళ (opt->jo_సమితి & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ఒకవేళ (buf == ఖాళీ)
		semsg(_(e_బఫరు_nr_does_not_exist),
					       (దీర్ఘ)opt->jo_io_buf[PART_ERR]);
	}
	లేకపోతే
	{
	    పూర్ణాంకం సందేశం = TRUE;

	    ఒకవేళ (opt->jo_set2 & JO2_ERR_సందేశం)
		సందేశం = opt->jo_సందేశం[PART_ERR];
	    buf = channel_కనుగొను_బఫరు(opt->jo_io_name[PART_ERR], TRUE, సందేశం);
	}
	ఒకవేళ (buf != ఖాళీ)
	{
	    ఒకవేళ (opt->jo_సమితి & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ఒకవేళ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    లేకపోతే
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (అక్షరం *)buf->b_ffname);
		సమితి_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ఒకవేళ (buf->b_ml.ml_mfp == ఖాళీ)
		    prepare_బఫరు(buf);
	    }
	}
    }

    channel->ch_part[PART_బహిర్గతం].ch_io = opt->jo_io[PART_బహిర్గతం];
    channel->ch_part[PART_ERR].ch_io = opt->jo_io[PART_ERR];
    channel->ch_part[PART_లోపల].ch_io = opt->jo_io[PART_లోపల];
}

/*
 * Parse the address for socketserver and store port in "port", or the path in
 * "unix_path" if it is a unix domain socket. Returns OK on success and FAIL on
 * failure.
 */
    పూర్ణాంకం
channel_parse_socketserver_చిరునామా(
	char_u	*చిరునామా,
	పూర్ణాంకం	*పోర్టు,
	char_u	**unix_మార్గం,
	bool	quiet)
{
    అక్షరం    *rest;
    దీర్ఘ    val;

    ఒకవేళ (*చిరునామా == NUL)
	వెళ్ళు fail;

    ఒకవేళ (STRNCMP(చిరునామా, "unix:", 5) == 0)
    {
	*unix_మార్గం = vim_strsave(చిరునామా + 5);
	తిరిగి OK;
    }

    val = strtol((అక్షరం *)(చిరునామా), &rest, 10);
    ఒకవేళ (val < 0 || val >= 65536 || *rest != NUL)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_invalid_వాదం_str), చిరునామా);
	తిరిగి FAIL;
    }
    *పోర్టు = (పూర్ణాంకం)val;

    తిరిగి OK;
fail:
    ఒకవేళ (!quiet)
	semsg(_(e_invalid_వాదం_str), చిరునామా);
    తిరిగి FAIL;
}

/*
 * Implements ch_open().
 */
    స్థిర channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*చిరునామా;
    char_u	*p;
    అక్షరం	*rest;
    పూర్ణాంకం		పోర్టు = 0;
    పూర్ణాంకం		is_ipv6 = FALSE;
    పూర్ణాంకం		is_unix = FALSE;
    పూర్ణాంకం		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి ఖాళీ;

    చిరునామా = tv_get_string(&argvars[0]);
    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	తిరిగి ఖాళీ;

    ఒకవేళ (*చిరునామా == NUL)
    {
	semsg(_(e_invalid_వాదం_str), చిరునామా);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (STRCMP(చిరునామా, "stdio") == 0)
	is_stdio = TRUE;
    లేకపోతే ఒకవేళ (STRNCMP(చిరునామా, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	చిరునామా += 5;
    }
    లేకపోతే ఒకవేళ (*చిరునామా == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(చిరునామా + 1, ']');
	ఒకవేళ (p == ఖాళీ || *++p != ':')
	{
	    semsg(_(e_invalid_వాదం_str), చిరునామా);
	    తిరిగి ఖాళీ;
	}
    }
    లేకపోతే
    {
	// ipv4 address
	p = vim_strchr(చిరునామా, ':');
	ఒకవేళ (p == ఖాళీ)
	{
	    semsg(_(e_invalid_వాదం_str), చిరునామా);
	    తిరిగి ఖాళీ;
	}
    }

    ఒకవేళ (!is_unix && !is_stdio)
    {
	పోర్టు = strtol((అక్షరం *)(p + 1), &rest, 10);
	ఒకవేళ (పోర్టు <= 0 || పోర్టు >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_వాదం_str), చిరునామా);
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++చిరునామా;
	    *(p - 1) = NUL;
	}
	లేకపోతే
	    *p = NUL;
    }

    // parse options
    clear_పని_options(&opt);
    opt.jo_బహుళకం = CH_బహుళకం_JSON;
    opt.jo_timeout = 2000;
    ఒకవేళ (get_పని_options(&argvars[1], &opt,
	    JO_బహుళకం_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	వెళ్ళు theend;
    ఒకవేళ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_వాదం));
	వెళ్ళు theend;
    }

    ఒకవేళ (is_stdio)
	channel = channel_open_stdio();
    లేకపోతే ఒకవేళ (is_unix)
	channel = channel_open_unix((అక్షరం *)చిరునామా, ఖాళీ);
    లేకపోతే
	channel = channel_open((అక్షరం *)చిరునామా, పోర్టు, opt.jo_waittime, ఖాళీ);
    ఒకవేళ (channel != ఖాళీ)
    {
	పూర్ణాంకం	user_సమితి = opt.jo_సమితి;

	opt.jo_సమితి = JO_ALL;
	ఒకవేళ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_సమితి &= ~((JO_లోపల_బహుళకం | JO_బహిర్గతం_బహుళకం | JO_ERR_బహుళకం
				| JO_బహిర్గతం_TIMEOUT | JO_ERR_TIMEOUT) & ~user_సమితి);
	channel_సమితి_options(channel, &opt);
    }
theend:
    free_పని_options(&opt);
    తిరిగి channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
స్థిర channel_T *stdio_channel = ఖాళీ;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    స్థిర channel_T *
channel_open_stdio(శూన్య)
{
    channel_T	*channel;
    sock_T	లోపల_fd;		// our stdin, the channel reads from it
    sock_T	బహిర్గతం_fd;		// our stdout, the channel writes to it

    ఒకవేళ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (stdio_channel != ఖాళీ)
    {
	emsg(_(e_cannot_open_stdio_channel));
	తిరిగి ఖాళీ;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#నిర్వచితమైతే MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	పూర్ణాంకం	nul_fd;

	ఒకవేళ (!DuplicateHandle(proc, GetStdHandle(STD_ఆదానం_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    తిరిగి ఖాళీ;
	}
	ఒకవేళ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    తిరిగి ఖాళీ;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ఖాళీ, OPEN_EXISTING, 0,
		ఖాళీ);
	ఒకవేళ (hnul != INVALID_HANDLE_విలువ)
	{
	    సమితి_Std_Handle(STD_ఆదానం_HANDLE, hnul);
	    సమితి_Std_Handle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ఒకవేళ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	లోపల_fd = (sock_T)hin;
	బహిర్గతం_fd = (sock_T)hout;
    }
#లేకపోతే
    {
	పూర్ణాంకం	nul_fd;

	లోపల_fd = dup(0);
	బహిర్గతం_fd = dup(1);
	ఒకవేళ (లోపల_fd < 0 || బహిర్గతం_fd < 0)
	{
	    ఒకవేళ (లోపల_fd >= 0)
		close(లోపల_fd);
	    ఒకవేళ (బహిర్గతం_fd >= 0)
		close(బహిర్గతం_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    తిరిగి ఖాళీ;
	}
	(శూన్య)fcntl(లోపల_fd, F_SETFD, FD_CLOEXEC);
	(శూన్య)fcntl(బహిర్గతం_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ఒకవేళ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#అయితే_ముగింపు

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
    {
	ch_error(ఖాళీ, "Cannot allocate channel.");
	fd_close(లోపల_fd);
	fd_close(బహిర్గతం_fd);
	తిరిగి ఖాళీ;
    }
    channel_సమితి_pipes(channel, బహిర్గతం_fd, లోపల_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    తిరిగి channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    అక్షరం	*rest;
    పూర్ణాంకం		పోర్టు;
    పూర్ణాంకం		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	తిరిగి ఖాళీ;

    arg = tv_get_string(&argvars[0]);
    ఒకవేళ (*arg == NUL)
    {
	semsg(_(e_invalid_వాదం_str), arg);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	పోర్టు = 0;
    }
    లేకపోతే
    {
	పోర్టు = strtol((అక్షరం *)(arg), &rest, 10);
	ఒకవేళ (పోర్టు < 0 || పోర్టు >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_వాదం_str), arg);
	    తిరిగి ఖాళీ;
	}
	*arg = NUL;
    }

    // parse options
    clear_పని_options(&opt);
    opt.jo_బహుళకం = CH_బహుళకం_JSON;
    opt.jo_timeout = 2000;
    ఒకవేళ (get_పని_options(&argvars[1], &opt,
	    JO_బహుళకం_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	వెళ్ళు theend;
    ఒకవేళ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_వాదం));
	వెళ్ళు theend;
    }

    ఒకవేళ (is_unix)
	channel = channel_listen_unix((అక్షరం *)arg, ఖాళీ, true);
    లేకపోతే
	channel = channel_listen(పోర్టు, ఖాళీ);
    ఒకవేళ (channel != ఖాళీ)
    {
	opt.jo_సమితి = JO_ALL;
	channel_సమితి_options(channel, &opt);
    }
theend:
    free_పని_options(&opt);
    తిరిగి channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	పూర్ణాంకం పోర్టు_లోపల,
	శూన్య (*nb_close_cb)(శూన్య))
{
    పూర్ణాంకం			sd = -1;
    నిర్మాణం sockaddr_లోపల	server;
#నిర్వచితం_కాకపోతే FEAT_IPV6
    నిర్మాణం hostent	*host;
#అయితే_ముగింపు
    పూర్ణాంకం			val = 1;
    channel_T		*channel;

#నిర్వచితమైతే MSWIN
    channel_init_winsock();
#అయితే_ముగింపు

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
    {
	ch_error(ఖాళీ, "Cannot allocate channel.");
	తిరిగి ఖాళీ;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((అక్షరం *)&server, 0, పరిమాణం(server));
    server.sin_family = AF_INET;
    server.sin_పోర్టు = htons(పోర్టు_లోపల);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ఒకవేళ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_పోర్టు));
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

#నిర్వచితమైతే MSWIN
    ఒకవేళ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (స్థిరరాశి అక్షరం *)&val, పరిమాణం(val)) < 0)
#లేకపోతే
    ఒకవేళ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, పరిమాణం(val)) < 0)
#అయితే_ముగింపు
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_పోర్టు));
	sock_close(sd);
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    // Bind the socket to the port
    ఒకవేళ (bind(sd, (నిర్మాణం sockaddr *)&server, పరిమాణం(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_పోర్టు));
	sock_close(sd);
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_పోర్టు));
	sock_close(sd);
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ఒకవేళ (పోర్టు_లోపల == 0)
    {
	నిర్మాణం sockaddr_లోపల	addr;
	socklen_t		addr_len = పరిమాణం(addr);

	ఒకవేళ (getsockname(sd, (నిర్మాణం sockaddr *)&addr, &addr_len) == 0)
	    పోర్టు_లోపల = ntohs(addr.sin_పోర్టు);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (అక్షరం *)vim_strsave((char_u *)"");
    channel->ch_పోర్టు = పోర్టు_లోపల;
    channel->ch_గమ్యం_be_closed |= (1U << PART_SOCK);

#నిర్వచితమైతే FEAT_GUI
    channel_gui_రిజిస్టరు_one(channel, PART_SOCK);
#అయితే_ముగింపు

    తిరిగి channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	అక్షరం *మార్గం,
	శూన్య (*nb_close_cb)(శూన్య),
	bool replace)
{
    పూర్ణాంకం			sd = -1;
    నిర్మాణం sockaddr_un	server;
    channel_T		*channel;
    size_t		మార్గం_len;
    size_t		server_len;

    ఒకవేళ (మార్గం == ఖాళీ || *మార్గం == NUL)
    {
	semsg(_(e_invalid_వాదం_str), మార్గం == ఖాళీ ? (అక్షరం *)"" : మార్గం);
	తిరిగి ఖాళీ;
    }

    మార్గం_len = STRLEN(మార్గం);
    ఒకవేళ (మార్గం_len >= పరిమాణం(server.sun_మార్గం))
    {
	semsg(_(e_invalid_వాదం_str), మార్గం);
	తిరిగి ఖాళీ;
    }

#నిర్వచితమైతే MSWIN
    channel_init_winsock();
#అయితే_ముగింపు

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
    {
	ch_error(ఖాళీ, "Cannot allocate channel.");
	తిరిగి ఖాళీ;
    }

    CLEAR_క్షేత్రం(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_మార్గం, మార్గం, పరిమాణం(server.sun_మార్గం) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ఒకవేళ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_పోర్టు));
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_మార్గం);

    // Bind the socket to the path
    server_len = offsetof(నిర్మాణం sockaddr_un, sun_మార్గం) + మార్గం_len + 1;
    ఒకవేళ (bind(sd, (నిర్మాణం sockaddr *)&server, (పూర్ణాంకం)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_పోర్టు));
	sock_close(sd);
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_పోర్టు));
	sock_close(sd);
	channel_free(channel);
	తిరిగి ఖాళీ;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (అక్షరం *)vim_strsave((char_u *)మార్గం);
    channel->ch_పోర్టు = 0;
    channel->ch_గమ్యం_be_closed |= (1U << PART_SOCK);

#నిర్వచితమైతే FEAT_GUI
    channel_gui_రిజిస్టరు_one(channel, PART_SOCK);
#అయితే_ముగింపు

    తిరిగి channel;
}

     శూన్య
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ఒకవేళ (*fd == INVALID_FD)
	తిరిగి;

    ఒకవేళ (part == PART_SOCK)
	sock_close(*fd);
    లేకపోతే
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ఒకవేళ ((part == PART_లోపల || channel->CH_లోపల_FD != *fd)
		&& (part == PART_బహిర్గతం || channel->CH_బహిర్గతం_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#నిర్వచితమైతే MSWIN
	    ఒకవేళ (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#అయితే_ముగింపు
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_గమ్యం_be_closed &= ~(1U << part);
}

    శూన్య
channel_సమితి_pipes(channel_T *channel, sock_T లోపల, sock_T బహిర్గతం, sock_T err)
{
    ఒకవేళ (లోపల != INVALID_FD)
    {
	ch_close_part(channel, PART_లోపల);
	channel->CH_లోపల_FD = లోపల;
#ఒకవేళ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ఒకవేళ (mch_isatty(లోపల))
	    channel->ch_గమ్యం_be_closed |= (1U << PART_లోపల);
#అయితే_ముగింపు
    }
    ఒకవేళ (బహిర్గతం != INVALID_FD)
    {
#ఒకవేళ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_బహిర్గతం);
#అయితే_ముగింపు
	ch_close_part(channel, PART_బహిర్గతం);
	channel->CH_బహిర్గతం_FD = బహిర్గతం;
	channel->ch_గమ్యం_be_closed |= (1U << PART_బహిర్గతం);
#ఒకవేళ defined(FEAT_GUI)
	channel_gui_రిజిస్టరు_one(channel, PART_బహిర్గతం);
#అయితే_ముగింపు
    }
    ఒకవేళ (err != INVALID_FD)
    {
#ఒకవేళ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#అయితే_ముగింపు
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_గమ్యం_be_closed |= (1U << PART_ERR);
#ఒకవేళ defined(FEAT_GUI)
	channel_gui_రిజిస్టరు_one(channel, PART_ERR);
#అయితే_ముగింపు
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    శూన్య
channel_సమితి_పని(channel_T *channel, పని_T *పని, jobopt_T *options)
{
    channel->ch_పని = పని;

    channel_సమితి_options(channel, options);

    ఒకవేళ (పని->jv_లోపల_buf == ఖాళీ)
	తిరిగి;

    chanpart_T *లోపల_part = &channel->ch_part[PART_లోపల];

    సమితి_bufref(&లోపల_part->ch_bufref, పని->jv_లోపల_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (అక్షరం *)లోపల_part->ch_bufref.br_buf->b_ffname);
    ఒకవేళ (options->jo_సమితి & JO_లోపల_శిఖరం)
    {
	ఒకవేళ (options->jo_లోపల_శిఖరం == 0 && !(options->jo_సమితి & JO_లోపల_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    లోపల_part->ch_bufref.br_buf->b_write_గమ్యం_channel = true;
	    లోపల_part->ch_buf_append = TRUE;
	    లోపల_part->ch_buf_శిఖరం =
		లోపల_part->ch_bufref.br_buf->b_ml.ml_పంక్తి_లెక్క + 1;
	}
	లేకపోతే
	    లోపల_part->ch_buf_శిఖరం = options->jo_లోపల_శిఖరం;
    }
    లేకపోతే
	లోపల_part->ch_buf_శిఖరం = 1;
    ఒకవేళ (options->jo_సమితి & JO_లోపల_BOT)
	లోపల_part->ch_buf_bot = options->jo_లోపల_bot;
    లేకపోతే
	లోపల_part->ch_buf_bot = లోపల_part->ch_bufref.br_buf->b_ml.ml_పంక్తి_లెక్క;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    స్థిర శూన్య
channel_సమితి_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	పూర్ణాంకం	    id)
{
    cbq_T *తల = &channel->ch_part[part].ch_cb_తల;
    cbq_T *అంశం = ALLOC_ONE(cbq_T);

    ఒకవేళ (అంశం == ఖాళీ)
	తిరిగి;

    copy_callback(&అంశం->cq_callback, callback);
    అంశం->cq_seq_nr = id;
    అంశం->cq_prev = తల->cq_prev;
    తల->cq_prev = అంశం;
    అంశం->cq_తదుపరి = ఖాళీ;
    ఒకవేళ (అంశం->cq_prev == ఖాళీ)
	తల->cq_తదుపరి = అంశం;
    లేకపోతే
	అంశం->cq_prev->cq_తదుపరి = అంశం;
}

    స్థిర శూన్య
write_buf_పంక్తి(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *పంక్తి = ml_get_buf(buf, lnum, FALSE);
    పూర్ణాంకం	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    పూర్ణాంకం	    i;

    // Need to make a copy to be able to append a NL.
    ఒకవేళ ((p = alloc(len + 2)) == ఖాళీ)
	తిరిగి;
    జ్ఞాపకం_నకలు((అక్షరం *)p, (అక్షరం *)పంక్తి, len);

    ఒకవేళ (channel->ch_write_పాఠ్యం_బహుళకం)
	p[len] = CAR;
    లేకపోతే
    {
	కొరకు (i = 0; i < len; ++i)
	    ఒకవేళ (p[i] == NL)
		p[i] = NUL;

	p[len] = NL;
    }
    p[len + 1] = NUL;
    channel_send(channel, PART_లోపల, p, len + 1, "write_buf_line");
    vim_free(p);
}

/*
 * Return TRUE if "channel" can be written to.
 * Returns FALSE if the input is closed or the write would block.
 */
    స్థిర పూర్ణాంకం
can_write_buf_పంక్తి(channel_T *channel)
{
    chanpart_T *లోపల_part = &channel->ch_part[PART_లోపల];

    ఒకవేళ (లోపల_part->ch_fd == INVALID_FD)
	తిరిగి FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ఒకవేళ (లోపల_part->ch_ఖండం_write == 1)
	లోపల_part->ch_ఖండం_write = -1;
    లేకపోతే ఒకవేళ (లోపల_part->ch_ఖండం_write == -1)
	లోపల_part->ch_ఖండం_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#నిర్వచితం_కాకపోతే MSWIN
    {
# ఒకవేళ defined(HAVE_SELECT)
	నిర్మాణం timeval	tval;
	fd_set		wfds;
	పూర్ణాంకం		ret;

	FD_ZERO(&wfds);
	FD_SET((పూర్ణాంకం)లోపల_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	కొరకు (;;)
	{
	    ret = select((పూర్ణాంకం)లోపల_part->ch_fd + 1, ఖాళీ, &wfds, ఖాళీ, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ఒకవేళ (ret == -1 && errno == EINTR)
		కొనసాగు;
#  endif
	    ఒకవేళ (ret <= 0 || లోపల_part->ch_ఖండం_write == 1)
	    {
		ఒకవేళ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		లేకపోతే
		    ch_log(channel, "Input not ready for writing");
		తిరిగి FALSE;
	    }
	    ఆపు;
	}
# లేకపోతే
	నిర్మాణం pollfd	fds;

	fds.fd = లోపల_part->ch_fd;
	fds.events = POLLOUT;
	ఒకవేళ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    తిరిగి FALSE;
	}
	ఒకవేళ (లోపల_part->ch_ఖండం_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    తిరిగి FALSE;
	}
# endif
    }
#అయితే_ముగింపు
    తిరిగి TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    శూన్య
channel_write_లోపల(channel_T *channel)
{
    chanpart_T *లోపల_part = &channel->ch_part[PART_లోపల];
    linenr_T    lnum;
    buf_T	*buf = లోపల_part->ch_bufref.br_buf;
    పూర్ణాంకం		written = 0;

    ఒకవేళ (buf == ఖాళీ || లోపల_part->ch_buf_append)
	తిరిగి;  // no buffer or using appending
    ఒకవేళ (!bufref_valid(&లోపల_part->ch_bufref) || buf->b_ml.ml_mfp == ఖాళీ)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	లోపల_part->ch_bufref.br_buf = ఖాళీ;
	తిరిగి;
    }

    కొరకు (lnum = లోపల_part->ch_buf_శిఖరం; lnum <= లోపల_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_పంక్తి_లెక్క; ++lnum)
    {
	ఒకవేళ (!can_write_buf_పంక్తి(channel))
	    ఆపు;
	write_buf_పంక్తి(buf, lnum, channel);
	++written;
    }

    ఒకవేళ (written == 1)
	ch_log(channel, "written line %d to channel", (పూర్ణాంకం)lnum - 1);
    లేకపోతే ఒకవేళ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    లోపల_part->ch_buf_శిఖరం = lnum;
    ఒకవేళ (lnum > buf->b_ml.ml_పంక్తి_లెక్క || lnum > లోపల_part->ch_buf_bot)
    {
#ఒకవేళ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ఒకవేళ (channel->ch_పని != ఖాళీ)
	    పదం_send_eof(channel);
#అయితే_ముగింపు

	// Writing is done, no longer need the buffer.
	లోపల_part->ch_bufref.br_buf = ఖాళీ;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#నిర్వచితమైతే MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	ఒకవేళ (!channel->ch_anonymous_pipe)
#అయితే_ముగింపు
	    ch_close_part(channel, PART_లోపల);
    }
    లేకపోతే
	ch_log(channel, "Still %ld more lines to write",
				   (దీర్ఘ)(buf->b_ml.ml_పంక్తి_లెక్క - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    శూన్య
channel_బఫరు_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	కొరకు (part = PART_SOCK; part < PART_లెక్క; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ఒకవేళ (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_పేర్లు[part]);
		ch_part->ch_bufref.br_buf = ఖాళీ;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    శూన్య
channel_write_ఆదానం(channel_T *channel)
{
    chanpart_T	*లోపల_part = &channel->ch_part[PART_లోపల];

    ఒకవేళ (లోపల_part->ch_writeque.wq_తదుపరి != ఖాళీ)
	channel_send(channel, PART_లోపల, (char_u *)"", 0, "channel_write_input");
    లేకపోతే ఒకవేళ (లోపల_part->ch_bufref.br_buf != ఖాళీ)
    {
	ఒకవేళ (లోపల_part->ch_buf_append)
	    channel_write_new_పంక్తులు(లోపల_part->ch_bufref.br_buf);
	లేకపోతే
	    channel_write_లోపల(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    శూన్య
channel_write_any_పంక్తులు(శూన్య)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_ఆదానం(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    శూన్య
channel_write_new_పంక్తులు(buf_T *buf)
{
    channel_T	*channel;
    పూర్ణాంకం		దొరికింది_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *లోపల_part = &channel->ch_part[PART_లోపల];
	linenr_T    lnum;
	పూర్ణాంకం	    written = 0;

	ఒకవేళ (లోపల_part->ch_bufref.br_buf == buf && లోపల_part->ch_buf_append)
	{
	    ఒకవేళ (లోపల_part->ch_fd == INVALID_FD)
		కొనసాగు;  // pipe was closed
	    దొరికింది_one = TRUE;
	    కొరకు (lnum = లోపల_part->ch_buf_bot; lnum < buf->b_ml.ml_పంక్తి_లెక్క;
								       ++lnum)
	    {
		ఒకవేళ (!can_write_buf_పంక్తి(channel))
		    ఆపు;
		write_buf_పంక్తి(buf, lnum, channel);
		++written;
	    }

	    ఒకవేళ (written == 1)
		ch_log(channel, "written line %d to channel", (పూర్ణాంకం)lnum - 1);
	    లేకపోతే ఒకవేళ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ఒకవేళ (lnum < buf->b_ml.ml_పంక్తి_లెక్క)
		ch_log(channel, "Still %ld more lines to write",
				       (దీర్ఘ)(buf->b_ml.ml_పంక్తి_లెక్క - lnum));

	    లోపల_part->ch_buf_bot = lnum;
	}
    }
    ఒకవేళ (!దొరికింది_one)
	buf->b_write_గమ్యం_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    స్థిర శూన్య
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    ఒకవేళ (సురక్షిత_గమ్యం_invoke_callback == 0)
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
    readq_T *తల = &channel->ch_part[part].ch_తల;

    తిరిగి తల->rq_తదుపరి;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_మొదటి_nl(readq_T *కణుపు)
{
    char_u  *బఫరు = కణుపు->rq_బఫరు;
    long_u  i;

    కొరకు (i = 0; i < కణుపు->rq_buflen; ++i)
	ఒకవేళ (బఫరు[i] == NL)
	    తిరిగి బఫరు + i;
    తిరిగి ఖాళీ;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, పూర్ణాంకం *outlen)
{
    readq_T *తల = &channel->ch_part[part].ch_తల;
    readq_T *కణుపు = తల->rq_తదుపరి;
    char_u *p;

    ఒకవేళ (కణుపు == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (outlen != ఖాళీ)
	*outlen += కణుపు->rq_buflen;
    // dispose of the node but keep the buffer
    p = కణుపు->rq_బఫరు;
    తల->rq_తదుపరి = కణుపు->rq_తదుపరి;
    ఒకవేళ (కణుపు->rq_తదుపరి == ఖాళీ)
	తల->rq_prev = ఖాళీ;
    లేకపోతే
	కణుపు->rq_తదుపరి->rq_prev = ఖాళీ;
    vim_free(కణుపు);
    తిరిగి p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    స్థిర char_u *
channel_get_all(channel_T *channel, ch_part_T part, పూర్ణాంకం *outlen)
{
    readq_T *తల = &channel->ch_part[part].ch_తల;
    readq_T *కణుపు;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    కొరకు (కణుపు = తల->rq_తదుపరి; కణుపు != ఖాళీ; కణుపు = కణుపు->rq_తదుపరి)
	len += కణుపు->rq_buflen;
    res = alloc(len + 1);
    ఒకవేళ (res == ఖాళీ)
	తిరిగి ఖాళీ;
    p = res;
    కొరకు (కణుపు = తల->rq_తదుపరి; కణుపు != ఖాళీ; కణుపు = కణుపు->rq_తదుపరి)
    {
	mch_memmove(p, కణుపు->rq_బఫరు, కణుపు->rq_buflen);
	p += కణుపు->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    చేయి
    {
	p = channel_get(channel, part, ఖాళీ);
	vim_free(p);
    } అయితే (p != ఖాళీ);

    ఒకవేళ (outlen != ఖాళీ)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	తిరిగి res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    అయితే (p < res + len)
    {
	ఒకవేళ (*p == NUL)
	    *p = NL;
#నిర్వచితమైతే MSWIN
	లేకపోతే ఒకవేళ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ఒకవేళ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		అయితే (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ఒకవేళ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#అయితే_ముగింపు
	++p;
    }

    తిరిగి res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    శూన్య
channel_consume(channel_T *channel, ch_part_T part, పూర్ణాంకం len)
{
    readq_T *తల = &channel->ch_part[part].ch_తల;
    readq_T *కణుపు = తల->rq_తదుపరి;
    char_u *buf = కణుపు->rq_బఫరు;

    ఒకవేళ (len < 0 || (long_u)len > కణుపు->rq_buflen)
	తిరిగి;
    mch_memmove(buf, buf + len, కణుపు->rq_buflen - len);
    కణుపు->rq_buflen -= len;
    కణుపు->rq_బఫరు[కణుపు->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    పూర్ణాంకం
channel_collapse(channel_T *channel, ch_part_T part, పూర్ణాంకం want_nl)
{
    ch_బహుళకం_T	బహుళకం = channel->ch_part[part].ch_బహుళకం;
    readq_T	*తల = &channel->ch_part[part].ch_తల;
    readq_T	*కణుపు = తల->rq_తదుపరి;
    readq_T	*చివరి_కణుపు;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ఒకవేళ (కణుపు == ఖాళీ || కణుపు->rq_తదుపరి == ఖాళీ)
	తిరిగి FAIL;

    చివరి_కణుపు = కణుపు->rq_తదుపరి;
    len = కణుపు->rq_buflen + చివరి_కణుపు->rq_buflen;
    ఒకవేళ (want_nl || బహుళకం == CH_బహుళకం_LSP || బహుళకం == CH_బహుళకం_DAP)
	అయితే (చివరి_కణుపు->rq_తదుపరి != ఖాళీ
		&& (బహుళకం == CH_బహుళకం_LSP || బహుళకం == CH_బహుళకం_DAP
		    || channel_మొదటి_nl(చివరి_కణుపు) == ఖాళీ))
	{
	    చివరి_కణుపు = చివరి_కణుపు->rq_తదుపరి;
	    len += చివరి_కణుపు->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ఒకవేళ (newbuf == ఖాళీ)
	తిరిగి FAIL;	    // out of memory
    mch_memmove(p, కణుపు->rq_బఫరు, కణుపు->rq_buflen);
    p += కణుపు->rq_buflen;
    vim_free(కణుపు->rq_బఫరు);
    కణుపు->rq_బఫరు = newbuf;
    కొరకు (n = కణుపు; n != చివరి_కణుపు; )
    {
	n = n->rq_తదుపరి;
	mch_memmove(p, n->rq_బఫరు, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_బఫరు);
    }
    *p = NUL;
    కణుపు->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    కొరకు (n = కణుపు->rq_తదుపరి; n != చివరి_కణుపు; )
    {
	n = n->rq_తదుపరి;
	vim_free(n->rq_prev);
    }
    కణుపు->rq_తదుపరి = చివరి_కణుపు->rq_తదుపరి;
    ఒకవేళ (చివరి_కణుపు->rq_తదుపరి == ఖాళీ)
	తల->rq_prev = కణుపు;
    లేకపోతే
	చివరి_కణుపు->rq_తదుపరి->rq_prev = కణుపు;
    vim_free(చివరి_కణుపు);
    తిరిగి OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
channel_save(channel_T *channel, ch_part_T part, char_u *buf, పూర్ణాంకం len,
						      పూర్ణాంకం prepend, అక్షరం *lead)
{
    readq_T *కణుపు;
    readq_T *తల = &channel->ch_part[part].ch_తల;
    char_u  *p;
    పూర్ణాంకం	    i;

    కణుపు = ALLOC_ONE(readq_T);
    ఒకవేళ (కణుపు == ఖాళీ)
	తిరిగి FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    కణుపు->rq_బఫరు = alloc(len + 1);
    ఒకవేళ (కణుపు->rq_బఫరు == ఖాళీ)
    {
	vim_free(కణుపు);
	తిరిగి FAIL;	    // out of memory
    }

    ఒకవేళ (channel->ch_part[part].ch_బహుళకం == CH_బహుళకం_NL)
    {
	// Drop any CR before a NL.
	p = కణుపు->rq_బఫరు;
	కొరకు (i = 0; i < len; ++i)
	    ఒకవేళ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	కణుపు->rq_buflen = (long_u)(p - కణుపు->rq_బఫరు);
    }
    లేకపోతే
    {
	mch_memmove(కణుపు->rq_బఫరు, buf, len);
	కణుపు->rq_బఫరు[len] = NUL;
	కణుపు->rq_buflen = (long_u)len;
    }

    ఒకవేళ (prepend)
    {
	// prepend node to the head of the queue
	కణుపు->rq_తదుపరి = తల->rq_తదుపరి;
	కణుపు->rq_prev = ఖాళీ;
	ఒకవేళ (తల->rq_తదుపరి == ఖాళీ)
	    తల->rq_prev = కణుపు;
	లేకపోతే
	    తల->rq_తదుపరి->rq_prev = కణుపు;
	తల->rq_తదుపరి = కణుపు;
    }
    లేకపోతే
    {
	// append node to the tail of the queue
	కణుపు->rq_తదుపరి = ఖాళీ;
	కణుపు->rq_prev = తల->rq_prev;
	ఒకవేళ (తల->rq_prev == ఖాళీ)
	    తల->rq_తదుపరి = కణుపు;
	లేకపోతే
	    తల->rq_prev->rq_తదుపరి = కణుపు;
	తల->rq_prev = కణుపు;
    }

    ఒకవేళ (ch_log_చురుకైన() && lead != ఖాళీ)
	ch_log_literal(lead, channel, part, buf, len);

    తిరిగి OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    పూర్ణాంకం
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*తదుపరి = channel_get(channel, part, ఖాళీ);
    పూర్ణాంకం		keeplen;
    పూర్ణాంకం		addlen;
    char_u	*p;

    ఒకవేళ (తదుపరి == ఖాళీ)
	తిరిగి FALSE;

    keeplen = reader->js_చివర - reader->js_buf;
    ఒకవేళ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (పూర్ణాంకం)STRLEN(తదుపరి);
	p = alloc(keeplen + addlen + 1);
	ఒకవేళ (p == ఖాళీ)
	{
	    vim_free(తదుపరి);
	    తిరిగి FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, తదుపరి, addlen + 1);
	vim_free(తదుపరి);
	తదుపరి = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = తదుపరి;
    తిరిగి TRUE;
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
    స్థిర పూర్ణాంకం
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*పంక్తి_ప్రారంభం;
    char_u	*p;
    int_u	hdr_len;
    పూర్ణాంకం		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_చివర = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    అయితే (TRUE)
    {
	పంక్తి_ప్రారంభం = p;
	అయితే (*p != NUL && *p != '\n')
	    p++;
	ఒకవేళ (*p == NUL)			// partial header
	    తిరిగి MAYBE;
	p++;

	// process the content length field (if present)
	ఒకవేళ ((p - పంక్తి_ప్రారంభం > 16)
		&& STRNICMP(పంక్తి_ప్రారంభం, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((అక్షరం *)పంక్తి_ప్రారంభం + 16, ఖాళీ, 10);
	    ఒకవేళ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		తిరిగి FAIL;
	}

	ఒకవేళ ((p - పంక్తి_ప్రారంభం) == 2 && పంక్తి_ప్రారంభం[0] == '\r' &&
		పంక్తి_ప్రారంభం[1] == '\n')
	    // reached the empty line
	    ఆపు;
    }

    ఒకవేళ (payload_len == -1)
	// Content-Length field is not present in the header
	తిరిగి FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ఒకవేళ (jsbuf_len < hdr_len + payload_len)
	తిరిగి MAYBE;

    reader->js_వాడిన += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_చివర = reader->js_buf + hdr_len + payload_len;

    తిరిగి OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    పూర్ణాంకం
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*అంశం;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*తల = &chanpart->ch_json_తల;
    పూర్ణాంకం		స్థితి_2 = OK;
    పూర్ణాంకం		ret;

    ఒకవేళ (channel_peek(channel, part) == ఖాళీ)
	తిరిగి FALSE;

    reader.js_buf = channel_get(channel, part, ఖాళీ);
    reader.js_వాడిన = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ఒకవేళ (!socketserver && (chanpart->ch_బహుళకం == CH_బహుళకం_LSP
	    || chanpart->ch_బహుళకం == CH_బహుళకం_DAP))
	స్థితి_2 = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ఒకవేళ (స్థితి_2 == OK)
    {
	++emsg_silent;
	స్థితి_2 = json_decode(&reader, &listtv,
				chanpart->ch_బహుళకం == CH_బహుళకం_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ఒకవేళ (స్థితి_2 == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ఒకవేళ (!socketserver && (chanpart->ch_బహుళకం == CH_బహుళకం_LSP
		    || chanpart->ch_బహుళకం == CH_బహుళకం_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	లేకపోతే ఒకవేళ (!socketserver && chanpart->ch_బహుళకం != CH_బహుళకం_LSP
		&& chanpart->ch_బహుళకం != CH_బహుళకం_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ఒకవేళ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    లేకపోతే
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	లేకపోతే
	{
	    అంశం = ALLOC_ONE(jsonq_T);
	    ఒకవేళ (అంశం == ఖాళీ)
		clear_tv(&listtv);
	    లేకపోతే
	    {
		అంశం->jq_no_callback = FALSE;
		అంశం->jq_విలువ = alloc_tv();
		ఒకవేళ (అంశం->jq_విలువ == ఖాళీ)
		{
		    vim_free(అంశం);
		    clear_tv(&listtv);
		}
		లేకపోతే
		{
		    *అంశం->jq_విలువ = listtv;
		    అంశం->jq_prev = తల->jq_prev;
		    తల->jq_prev = అంశం;
		    అంశం->jq_తదుపరి = ఖాళీ;
		    ఒకవేళ (అంశం->jq_prev == ఖాళీ)
			తల->jq_తదుపరి = అంశం;
		    లేకపోతే
			అంశం->jq_prev->jq_తదుపరి = అంశం;
		}
	    }
	}
    }

    ఒకవేళ (స్థితి_2 == OK)
	chanpart->ch_wait_len = 0;
    లేకపోతే ఒకవేళ (స్థితి_2 == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ఒకవేళ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (పూర్ణాంకం)buflen);
	    reader.js_వాడిన = 0;
	    chanpart->ch_wait_len = buflen;
#నిర్వచితమైతే MSWIN
	    chanpart->ch_deadline = Get_Tick_లెక్క() + 100L;
#లేకపోతే
	    gettimeofday(&chanpart->ch_deadline, ఖాళీ);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ఒకవేళ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#అయితే_ముగింపు
	}
	లేకపోతే
	{
	    పూర్ణాంకం timeout;
#నిర్వచితమైతే MSWIN
	    timeout = (పూర్ణాంకం)(Get_Tick_లెక్క() - chanpart->ch_deadline) > 0;
#లేకపోతే
	    {
		నిర్మాణం timeval now_tv;

		gettimeofday(&now_tv, ఖాళీ);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#అయితే_ముగింపు
	    ఒకవేళ (timeout)
	    {
		స్థితి_2 = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    లేకపోతే
	    {
		reader.js_వాడిన = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ఒకవేళ (స్థితి_2 == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    లేకపోతే ఒకవేళ (reader.js_buf[reader.js_వాడిన] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_వాడిన,
			(పూర్ణాంకం)(reader.js_చివర - reader.js_buf) - reader.js_వాడిన,
								  TRUE, ఖాళీ);
	ret = స్థితి_2 == MAYBE ? FALSE: TRUE;
    }
    లేకపోతే
	ret = FALSE;

    vim_free(reader.js_buf);
    తిరిగి ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    స్థిర శూన్య
remove_cb_కణుపు(cbq_T *తల, cbq_T *కణుపు)
{
    ఒకవేళ (కణుపు->cq_prev == ఖాళీ)
	తల->cq_తదుపరి = కణుపు->cq_తదుపరి;
    లేకపోతే
	కణుపు->cq_prev->cq_తదుపరి = కణుపు->cq_తదుపరి;
    ఒకవేళ (కణుపు->cq_తదుపరి == ఖాళీ)
	తల->cq_prev = కణుపు->cq_prev;
    లేకపోతే
	కణుపు->cq_తదుపరి->cq_prev = కణుపు->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    శూన్య
remove_json_కణుపు(jsonq_T *తల, jsonq_T *కణుపు)
{
    ఒకవేళ (కణుపు->jq_prev == ఖాళీ)
	తల->jq_తదుపరి = కణుపు->jq_తదుపరి;
    లేకపోతే
	కణుపు->jq_prev->jq_తదుపరి = కణుపు->jq_తదుపరి;
    ఒకవేళ (కణుపు->jq_తదుపరి == ఖాళీ)
	తల->jq_prev = కణుపు->jq_prev;
    లేకపోతే
	కణుపు->jq_తదుపరి->jq_prev = కణుపు->jq_prev;
    vim_free(కణుపు);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    స్థిర శూన్య
channel_కూడు_ఖండం_id(chanpart_T *chanpart, పూర్ణాంకం id)
{
    garray_T *gap = &chanpart->ch_ఖండం_ids;

    ఒకవేళ (gap->ga_growsize == 0)
	ga_init2(gap, పరిమాణం(పూర్ణాంకం), 10);
    ఒకవేళ (ga_grow(gap, 1) == OK)
    {
	((పూర్ణాంకం *)gap->ga_దత్తాంశం)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    స్థిర శూన్య
channel_remove_ఖండం_id(chanpart_T *chanpart, పూర్ణాంకం id)
{
    garray_T	*gap = &chanpart->ch_ఖండం_ids;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < gap->ga_len; ++i)
	ఒకవేళ (((పూర్ణాంకం *)gap->ga_దత్తాంశం)[i] == id)
	{
	    --gap->ga_len;
	    ఒకవేళ (i < gap->ga_len)
	    {
		పూర్ణాంకం *p = ((పూర్ణాంకం *)gap->ga_దత్తాంశం) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * పరిమాణం(పూర్ణాంకం));
	    }
	    తిరిగి;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    స్థిర పూర్ణాంకం
channel_has_ఖండం_id(chanpart_T *chanpart, పూర్ణాంకం id)
{
    garray_T	*gap = &chanpart->ch_ఖండం_ids;
    పూర్ణాంకం		i;

    కొరకు (i = 0; i < gap->ga_len; ++i)
	ఒకవేళ (((పూర్ణాంకం *)gap->ga_దత్తాంశం)[i] == id)
	    తిరిగి TRUE;
    తిరిగి FALSE;
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
    స్థిర పూర్ణాంకం
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	పూర్ణాంకం	    id,
	పూర్ణాంకం	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *తల = &channel->ch_part[part].ch_json_తల;
    jsonq_T   *అంశం = తల->jq_తదుపరి;

    అయితే (అంశం != ఖాళీ)
    {
	list_T	    *l;
	typval_T    *tv;

	ఒకవేళ (channel->ch_part[part].ch_బహుళకం != CH_బహుళకం_LSP
		&& channel->ch_part[part].ch_బహుళకం != CH_బహుళకం_DAP)
	{
	    l = అంశం->jq_విలువ->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_మొదటి->li_tv;
	}
	లేకపోతే
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ఒకవేళ (channel->ch_part[part].ch_బహుళకం == CH_బహుళకం_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ఒకవేళ (id > 0)
		{
		    ఒకవేళ (అంశం->jq_విలువ->v_type != VAR_DICT)
			వెళ్ళు nextitem;
		    d = అంశం->jq_విలువ->vval.v_dict;
		    ఒకవేళ (d == ఖాళీ)
			వెళ్ళు nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ఒకవేళ (dict_has_కీలకం(d, "method"))
			వెళ్ళు nextitem;
		    di = dict_కనుగొను(d, (char_u *)"id", -1);
		    ఒకవేళ (di == ఖాళీ)
			వెళ్ళు nextitem;
		    tv = &di->di_tv;
		}
		లేకపోతే
		    tv = అంశం->jq_విలువ;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (id > 0)
		{
		    ఒకవేళ (అంశం->jq_విలువ->v_type != VAR_DICT)
			వెళ్ళు nextitem;
		    d = అంశం->jq_విలువ->vval.v_dict;
		    ఒకవేళ (d == ఖాళీ)
			వెళ్ళు nextitem;
		    di = dict_కనుగొను(d, (char_u *)"request_seq", -1);
		    ఒకవేళ (di == ఖాళీ)
			వెళ్ళు nextitem;
		    tv = &di->di_tv;
		}
		లేకపోతే
		    tv = అంశం->jq_విలువ;
	    }
	}

	ఒకవేళ ((without_callback || !అంశం->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_సంఖ్య && tv->vval.v_సంఖ్య == id)
	      || (id <= 0 && (tv->v_type != VAR_సంఖ్య
		 || tv->vval.v_సంఖ్య == 0
		 || !channel_has_ఖండం_id(
				&channel->ch_part[part], tv->vval.v_సంఖ్య)))))
	{
	    *rettv = అంశం->jq_విలువ;
	    ఒకవేళ (tv->v_type == VAR_సంఖ్య)
		ch_log(channel, "Getting JSON message %ld",
						      (దీర్ఘ)tv->vval.v_సంఖ్య);
	    remove_json_కణుపు(తల, అంశం);
	    తిరిగి OK;
	}
nextitem:
	అంశం = అంశం->jq_తదుపరి;
    }
    తిరిగి FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    స్థిర శూన్య
channel_నెట్టు_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *తల = &channel->ch_part[part].ch_json_తల;
    jsonq_T   *అంశం = తల->jq_తదుపరి;
    jsonq_T   *newitem;

    ఒకవేళ (తల->jq_prev != ఖాళీ && తల->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	అంశం = ఖాళీ;
    లేకపోతే అయితే (అంశం != ఖాళీ && అంశం->jq_no_callback)
	// append after the last item that was pushed back
	అంశం = అంశం->jq_తదుపరి;

    newitem = ALLOC_ONE(jsonq_T);
    ఒకవేళ (newitem == ఖాళీ)
    {
	clear_tv(rettv);
	తిరిగి;
    }

    newitem->jq_విలువ = alloc_tv();
    ఒకవేళ (newitem->jq_విలువ == ఖాళీ)
    {
	vim_free(newitem);
	clear_tv(rettv);
	తిరిగి;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_విలువ = *rettv;
    ఒకవేళ (అంశం == ఖాళీ)
    {
	// append to the end
	newitem->jq_prev = తల->jq_prev;
	తల->jq_prev = newitem;
	newitem->jq_తదుపరి = ఖాళీ;
	ఒకవేళ (newitem->jq_prev == ఖాళీ)
	    తల->jq_తదుపరి = newitem;
	లేకపోతే
	    newitem->jq_prev->jq_తదుపరి = newitem;
    }
    లేకపోతే
    {
	// append after "item"
	newitem->jq_prev = అంశం;
	newitem->jq_తదుపరి = అంశం->jq_తదుపరి;
	అంశం->jq_తదుపరి = newitem;
	ఒకవేళ (newitem->jq_తదుపరి == ఖాళీ)
	    తల->jq_prev = newitem;
	లేకపోతే
	    newitem->jq_తదుపరి->jq_prev = newitem;
    }
}

#నిర్వచించు CH_JSON_MAX_వాదనలు 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    స్థిర శూన్య
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    పూర్ణాంకం	    options = channel->ch_part[part].ch_బహుళకం == CH_బహుళకం_JS
								 ? JSON_JS : 0;

    ఒకవేళ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ఒకవేళ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_వాదం));
	తిరిగి;
    }
    arg = argv[1].vval.v_string;
    ఒకవేళ (arg == ఖాళీ)
	arg = (char_u *)"";

    ఒకవేళ (STRCMP(cmd, "ex") == 0)
    {
	పూర్ణాంకం	called_emsg_before = called_emsg;
	char_u	*p = arg;
	పూర్ణాంకం	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (అక్షరం *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ఒకవేళ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ఒకవేళ (do_emsg_silent)
	    --emsg_silent;
	ఒకవేళ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (అక్షరం *)get_vim_var_str(VV_ERRMSG));
    }
    లేకపోతే ఒకవేళ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (అక్షరం *)arg);
	CLEAR_క్షేత్రం(ea);
	ea.arg = arg;
	ea.addr_లెక్క = 0;
	ea.forceit = TRUE; // no mapping
	ex_సాధారణ(&ea);
    }
    లేకపోతే ఒకవేళ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	బహిర్గతం_flush_cursor(TRUE, FALSE);
    }
    లేకపోతే ఒకవేళ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	పూర్ణాంకం is_call = cmd[0] == 'c';
	పూర్ణాంకం id_idx = is_call ? 3 : 2;

	ఒకవేళ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_సంఖ్య)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ఒకవేళ (p_verbose > 2)
		emsg(_(e_చివరి_వాదం_for_సమాసం_call_must_be_సంఖ్య));
	}
	లేకపోతే ఒకవేళ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ఒకవేళ (p_verbose > 2)
		emsg(_(e_మూడవ_వాదం_for_call_must_be_list));
	}
	లేకపోతే
	{
	    typval_T	*tv = ఖాళీ;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ఖాళీ;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ఒకవేళ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (అక్షరం *)arg);
		tv = eval_సమాసం(arg, ఖాళీ);
	    }
	    లేకపోతే
	    {
		ch_log(channel, "Calling '%s'", (అక్షరం *)arg);
		ఒకవేళ (func_call(arg, &argv[2], ఖాళీ, ఖాళీ, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ఒకవేళ (argv[id_idx].v_type == VAR_సంఖ్య)
	    {
		పూర్ణాంకం id = argv[id_idx].vval.v_సంఖ్య;

		ఒకవేళ (tv != ఖాళీ)
		    json = json_encode_nr_సమాసం(id, tv, options | JSON_NL);
		ఒకవేళ (tv == ఖాళీ || (json != ఖాళీ && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_సమాసం(id, &err_tv, options | JSON_NL);
		}
		ఒకవేళ (json != ఖాళీ)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_లోపల,
				 json, (పూర్ణాంకం)STRLEN(json), (అక్షరం *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ఒకవేళ (tv == &res_tv)
		clear_tv(tv);
	    లేకపోతే
		free_tv(tv);
	}
    }
    లేకపోతే ఒకవేళ (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (అక్షరం *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    స్థిర శూన్య
invoke_one_సమయం_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *అంశం,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (అక్షరం *)అంశం->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_కణుపు(cbhead, అంశం);
    invoke_callback(channel, &అంశం->cq_callback, argv);
    free_callback(&అంశం->cq_callback);
    vim_free(అంశం);
}

    స్థిర శూన్య
append_గమ్యం_బఫరు(
    buf_T	*బఫరు,
    char_u	*సందేశం,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = బఫరు->b_ml.ml_పంక్తి_లెక్క;
    bool	save_write_గమ్యం = బఫరు->b_write_గమ్యం_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    పూర్ణాంకం		save_p_ma = బఫరు->b_p_ma;
    పూర్ణాంకం		ఖాళీలు_2 = (బఫరు->b_ml.ml_flags & ML_ఖాళీలు) ? 1 : 0;

    ఒకవేళ (!బఫరు->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ఒకవేళ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	తిరిగి;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ఒకవేళ (save_write_గమ్యం)
    {
	--lnum;
	బఫరు->b_write_గమ్యం_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (పూర్ణాంకం)lnum + 1 - ఖాళీలు_2, బఫరు->b_fname);

    బఫరు->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, బఫరు);
    ఒకవేళ (curbuf != బఫరు)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	తిరిగి;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - ఖాళీలు_2, lnum + 1);

    ఒకవేళ (ఖాళీలు_2)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, సందేశం, TRUE);
	lnum = 0;
    }
    లేకపోతే
	ml_append(lnum, సందేశం, 0, FALSE);
    appended_పంక్తులు_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ఒకవేళ (ch_part->ch_nomodifiable)
	బఫరు->b_p_ma = FALSE;
    లేకపోతే
	బఫరు->b_p_ma = save_p_ma;

    ఒకవేళ (బఫరు->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ఒకవేళ (wp->w_బఫరు == బఫరు)
	    {
		పూర్ణాంకం move_cursor = save_write_గమ్యం
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.నిలువు == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ఒకవేళ (move_cursor || wp->w_topline > బఫరు->b_ml.ml_పంక్తి_లెక్క)
		{
		    win_T *save_curwin = curwin;

		    ఒకవేళ (move_cursor)
			++wp->w_cursor.lnum;
		    curwin = wp;
		    curbuf = curwin->w_బఫరు;
		    scroll_cursor_bot(0, FALSE);
		    curwin = save_curwin;
		    curbuf = curwin->w_బఫరు;
		}
	    }
	}
	redraw_buf_and_స్థితి_later(బఫరు, UPD_VALID);
	channel_need_redraw = TRUE;
    }

    ఒకవేళ (save_write_గమ్యం)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	బఫరు->b_write_గమ్యం_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *లోపల_part = &ch->ch_part[PART_లోపల];

	    ఒకవేళ (లోపల_part->ch_bufref.br_buf == బఫరు)
		లోపల_part->ch_buf_bot = బఫరు->b_ml.ml_పంక్తి_లెక్క;
	}
    }
}

    స్థిర శూన్య
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *సందేశం;

    అయితే ((సందేశం = channel_get(channel, part, ఖాళీ)) != ఖాళీ)
    {
	ch_log(channel, "Dropping message '%s'", (అక్షరం *)సందేశం);
	vim_free(సందేశం);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    స్థిర పూర్ణాంకం
channel_use_json_తల(channel_T *channel, ch_part_T part)
{
    ch_బహుళకం_T	ch_బహుళకం = channel->ch_part[part].ch_బహుళకం;

    తిరిగి ch_బహుళకం == CH_బహుళకం_JSON || ch_బహుళకం == CH_బహుళకం_JS
						     || ch_బహుళకం == CH_బహుళకం_LSP
						     || ch_బహుళకం == CH_బహుళకం_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    స్థిర పూర్ణాంకం
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*సందేశం = ఖాళీ;
    blob_T	*blob = ఖాళీ;
    typval_T	*listtv = ఖాళీ;
    typval_T	argv[CH_JSON_MAX_వాదనలు];
    పూర్ణాంకం		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_బహుళకం_T	ch_బహుళకం = ch_part->ch_బహుళకం;
    cbq_T	*cbhead = &ch_part->ch_cb_తల;
    cbq_T	*cbitem;
    callback_T	*callback = ఖాళీ;
    buf_T	*బఫరు = ఖాళీ;
    char_u	*p;
    పూర్ణాంకం		called_otc;		// one time callbackup
    పూర్ణాంకం		raw_len = 0;

    ఒకవేళ (channel->ch_nb_close_cb != ఖాళీ
#నిర్వచితమైతే FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#అయితే_ముగింపు
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	తిరిగి FALSE;

    // Use a message-specific callback, part callback or channel callback
    కొరకు (cbitem = cbhead->cq_తదుపరి; cbitem != ఖాళీ; cbitem = cbitem->cq_తదుపరి)
	ఒకవేళ (cbitem->cq_seq_nr == 0)
	    ఆపు;
    ఒకవేళ (cbitem != ఖాళీ)
	callback = &cbitem->cq_callback;
    లేకపోతే ఒకవేళ (ch_part->ch_callback.cb_name != ఖాళీ)
	callback = &ch_part->ch_callback;
    లేకపోతే ఒకవేళ (channel->ch_callback.cb_name != ఖాళీ)
	callback = &channel->ch_callback;

    బఫరు = ch_part->ch_bufref.br_buf;
    ఒకవేళ (బఫరు != ఖాళీ && (!bufref_valid(&ch_part->ch_bufref)
					       || బఫరు->b_ml.ml_mfp == ఖాళీ))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_పేర్లు[part]);
	ch_part->ch_bufref.br_buf = ఖాళీ;
	బఫరు = ఖాళీ;
    }

    ఒకవేళ (channel_use_json_తల(channel, part))
    {
	listitem_T	*అంశం;
	పూర్ణాంకం		argc = 0;

	// Get any json message in the queue.
	ఒకవేళ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ఒకవేళ (ch_బహుళకం == CH_బహుళకం_LSP || ch_బహుళకం == CH_బహుళకం_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(శూన్య)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ఒకవేళ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		తిరిగి FALSE;
	}

	ఒకవేళ (ch_బహుళకం == CH_బహుళకం_LSP || ch_బహుళకం == CH_బహుళకం_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ఒకవేళ (d != ఖాళీ)
	    {
		ఒకవేళ (ch_బహుళకం == CH_బహుళకం_LSP)
		    di = dict_కనుగొను(d, (char_u *)"id", -1);
		లేకపోతే
		    di = dict_కనుగొను(d, (char_u *)"request_seq", -1);
		ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_సంఖ్య)
		    seq_nr = di->di_tv.vval.v_సంఖ్య;
	    }

	    argv[1] = *listtv;
	}
	లేకపోతే
	{
	    కొరకు (అంశం = listtv->vval.v_list->lv_మొదటి;
		    అంశం != ఖాళీ && argc < CH_JSON_MAX_వాదనలు;
		    అంశం = అంశం->li_తదుపరి)
		argv[argc++] = అంశం->li_tv;
	    అయితే (argc < CH_JSON_MAX_వాదనలు)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ఒకవేళ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		తిరిగి TRUE;
	    }

	    ఒకవేళ (argv[0].v_type != VAR_సంఖ్య)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		తిరిగి FALSE;
	    }
	    seq_nr = argv[0].vval.v_సంఖ్య;
	}
    }
    లేకపోతే ఒకవేళ (channel_peek(channel, part) == ఖాళీ)
    {
	// nothing to read on RAW or NL channel
	తిరిగి FALSE;
    }
    లేకపోతే
    {
	// If there is no callback or buffer drop the message.
	ఒకవేళ (callback == ఖాళీ && బఫరు == ఖాళీ)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ఒకవేళ (channel->ch_close_cb.cb_name == ఖాళీ && !channel->ch_drop_never)
		drop_messages(channel, part);
	    తిరిగి FALSE;
	}

	ఒకవేళ (ch_బహుళకం == CH_బహుళకం_NL)
	{
	    char_u  *nl = ఖాళీ;
	    char_u  *buf;
	    readq_T *కణుపు;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    అయితే (TRUE)
	    {
		కణుపు = channel_peek(channel, part);
		nl = channel_మొదటి_nl(కణుపు);
		ఒకవేళ (nl != ఖాళీ)
		    ఆపు;
		ఒకవేళ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ఒకవేళ (ch_part->ch_fd == INVALID_FD && కణుపు->rq_buflen > 0)
			ఆపు;
		    తిరిగి FALSE; // incomplete message
		}
	    }
	    buf = కణుపు->rq_బఫరు;

	    // Convert NUL to NL, the internal representation.
	    కొరకు (p = buf; (nl == ఖాళీ || p < nl)
					    && p < buf + కణుపు->rq_buflen; ++p)
		ఒకవేళ (*p == NUL)
		    *p = NL;

	    ఒకవేళ (nl == ఖాళీ)
	    {
		// get the whole buffer, drop the NL
		సందేశం = channel_get(channel, part, ఖాళీ);
	    }
	    లేకపోతే ఒకవేళ (nl + 1 == buf + కణుపు->rq_buflen)
	    {
		// get the whole buffer
		సందేశం = channel_get(channel, part, ఖాళీ);
		*nl = NUL;
	    }
	    లేకపోతే
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		సందేశం = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (పూర్ణాంకం)(nl - buf) + 1);
	    }
	}
	లేకపోతే
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    సందేశం = channel_get_all(channel, part,
			ch_బహుళకం == CH_బహుళకం_BLOB ? &raw_len : ఖాళీ);
	}

	ఒకవేళ (సందేశం == ఖాళీ)
	    తిరిగి FALSE; // out of memory (and avoids Coverity warning)

	ఒకవేళ (ch_బహుళకం == CH_బహుళకం_BLOB)
	{
	    blob = blob_alloc();
	    ఒకవేళ (blob == ఖాళీ)
	    {
		vim_free(సందేశం);
		తిరిగి FALSE;
	    }
	    ఒకవేళ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(సందేశం);
		తిరిగి FALSE;
	    }
	    ఒకవేళ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_దత్తాంశం, సందేశం, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	లేకపోతే
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = సందేశం;
	}
    }

    called_otc = FALSE;
    ఒకవేళ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	పూర్ణాంకం lsp_req_సందేశం = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ఒకవేళ ((ch_బహుళకం == CH_బహుళకం_LSP || ch_బహుళకం == CH_బహుళకం_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_కీలకం(argv[1].vval.v_dict, "method"))
	    lsp_req_సందేశం = TRUE;

	ఒకవేళ (!lsp_req_సందేశం)
	{
	    కొరకు (cbitem = cbhead->cq_తదుపరి; cbitem != ఖాళీ;
		    cbitem = cbitem->cq_తదుపరి)
	    {
		ఒకవేళ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_సమయం_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    ఆపు;
		}
	    }
	}
    }

    ఒకవేళ (seq_nr > 0 && ((ch_బహుళకం != CH_బహుళకం_LSP && ch_బహుళకం != CH_బహుళకం_DAP)
		|| called_otc))
    {
	ఒకవేళ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ఒకవేళ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_నెట్టు_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_సంఖ్య;
		free_tv(listtv);
		listtv = ఖాళీ;
	    }
	    లేకపోతే
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    లేకపోతే ఒకవేళ (callback != ఖాళీ || బఫరు != ఖాళీ)
    {
	ఒకవేళ (బఫరు != ఖాళీ)
	{
	    ఒకవేళ (సందేశం == ఖాళీ)
		// JSON or JS mode: re-encode the message.
		సందేశం = json_encode(listtv, ch_బహుళకం);
	    ఒకవేళ (సందేశం != ఖాళీ)
	    {
#నిర్వచితమైతే FEAT_TERMINAL
		ఒకవేళ (బఫరు->b_పదం != ఖాళీ)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ఒకవేళ (part == PART_ERR)
		    {
			char_u *cp;
			పూర్ణాంకం    extra = 0;

			కొరకు (cp = సందేశం; *cp != NUL; ++cp)
			    ఒకవేళ (*cp == NL && (cp == సందేశం || cp[-1] != CAR))
				++extra;
			ఒకవేళ (extra > 0)
			{
			    char_u *crlf_సందేశం = alloc(STRLEN(సందేశం) + extra + 1);

			    ఒకవేళ (crlf_సందేశం != ఖాళీ)
			    {
				char_u *q = crlf_సందేశం;

				కొరకు (cp = సందేశం; *cp != NUL; ++cp)
				{
				    ఒకవేళ (*cp == NL && (cp == సందేశం || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_గమ్యం_పదం(బఫరు, crlf_సందేశం, channel);
				vim_free(crlf_సందేశం);
			    }
			}
			లేకపోతే
			    write_గమ్యం_పదం(బఫరు, సందేశం, channel);
		    }
		    లేకపోతే
			write_గమ్యం_పదం(బఫరు, సందేశం, channel);
		}
		లేకపోతే
#అయితే_ముగింపు
		    append_గమ్యం_బఫరు(బఫరు, సందేశం, channel, part);
	    }
	}

	ఒకవేళ (callback != ఖాళీ)
	{
	    ఒకవేళ (cbitem != ఖాళీ)
		invoke_one_సమయం_callback(channel, cbhead, cbitem, argv);
	    లేకపోతే
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (అక్షరం *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    లేకపోతే
	ch_log(channel, "Dropping message %d", seq_nr);

    ఒకవేళ (listtv != ఖాళీ)
	free_tv(listtv);
    ఒకవేళ (blob != ఖాళీ)
	blob_unref(blob);
    vim_free(సందేశం);

    తిరిగి TRUE;
}

#ఒకవేళ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    పూర్ణాంకం
channel_can_write_గమ్యం(channel_T *channel)
{
    తిరిగి channel != ఖాళీ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_లోపల_FD != INVALID_FD);
}
#అయితే_ముగింపు

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    పూర్ణాంకం
channel_is_open(channel_T *channel)
{
    తిరిగి channel != ఖాళీ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_లోపల_FD != INVALID_FD
			  || channel->CH_బహిర్గతం_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    స్థిర శూన్య *
channel_readahead_సూచిక(channel_T *channel, ch_part_T part)
{
    ఒకవేళ (channel_use_json_తల(channel, part))
    {
	jsonq_T   *తల = &channel->ch_part[part].ch_json_తల;

	ఒకవేళ (తల->jq_తదుపరి == ఖాళీ)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	తిరిగి తల->jq_తదుపరి;
    }
    తిరిగి channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    స్థిర పూర్ణాంకం
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    తిరిగి channel_readahead_సూచిక(channel, part) != ఖాళీ;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    స్థిర అక్షరం *
channel_స్థితి(channel_T *channel, పూర్ణాంకం req_part)
{
    ch_part_T part;
    పూర్ణాంకం has_readahead = FALSE;

    ఒకవేళ (channel == ఖాళీ)
	 తిరిగి "fail";
    ఒకవేళ (req_part == PART_బహిర్గతం)
    {
	ఒకవేళ (channel->CH_బహిర్గతం_FD != INVALID_FD)
	    తిరిగి "open";
	ఒకవేళ (channel_has_readahead(channel, PART_బహిర్గతం))
	    has_readahead = TRUE;
    }
    లేకపోతే ఒకవేళ (req_part == PART_ERR)
    {
	ఒకవేళ (channel->CH_ERR_FD != INVALID_FD)
	    తిరిగి "open";
	ఒకవేళ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    లేకపోతే
    {
	ఒకవేళ (channel_is_open(channel))
	    తిరిగి "open";
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	    ఒకవేళ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		ఆపు;
	    }
    }

    ఒకవేళ (has_readahead)
	తిరిగి "buffered";
    తిరిగి "closed";
}

    స్థిర శూన్య
channel_part_info(channel_T *channel, dict_T *dict, అక్షరం *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    అక్షరం	namebuf[20];  // longest is "sock_timeout"
    size_t	తోక;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    తోక = STRLEN(namebuf);
    STRCPY(namebuf + తోక, "_");
    తోక += STRLEN_LITERAL("_");

    STRCPY(namebuf + తోక, "status");
    ఒకవేళ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_సమితి(s, "open");
    లేకపోతే ఒకవేళ (channel_has_readahead(channel, part))
	STR_LITERAL_సమితి(s, "buffered");
    లేకపోతే
	STR_LITERAL_సమితి(s, "closed");
    dict_కూడు_string_len(dict, namebuf, s.string, (పూర్ణాంకం)s.నిడివి);

    STRCPY(namebuf + తోక, "mode");
    ఎంపిక (chanpart->ch_బహుళకం)
    {
	స్థితి CH_బహుళకం_NL:
	    STR_LITERAL_సమితి(s, "NL");
	    ఆపు;
	స్థితి CH_బహుళకం_RAW:
	    STR_LITERAL_సమితి(s, "RAW");
	    ఆపు;
	స్థితి CH_బహుళకం_BLOB:
	    STR_LITERAL_సమితి(s, "BLOB");
	    ఆపు;
	స్థితి CH_బహుళకం_JSON:
	    STR_LITERAL_సమితి(s, "JSON");
	    ఆపు;
	స్థితి CH_బహుళకం_JS:
	    STR_LITERAL_సమితి(s, "JS");
	    ఆపు;
	స్థితి CH_బహుళకం_LSP:
	    STR_LITERAL_సమితి(s, "LSP");
	    ఆపు;
	స్థితి CH_బహుళకం_DAP:
	    STR_LITERAL_సమితి(s, "DAP");
	    ఆపు;
	అప్రమేయ:
	    STR_LITERAL_సమితి(s, "");
	    ఆపు;
    }
    dict_కూడు_string_len(dict, namebuf, s.string, (పూర్ణాంకం)s.నిడివి);

    STRCPY(namebuf + తోక, "io");
    ఒకవేళ (part == PART_SOCK)
	STR_LITERAL_సమితి(s, "socket");
    లేకపోతే ఎంపిక (chanpart->ch_io)
    {
	స్థితి JIO_NULL:
	    STR_LITERAL_సమితి(s, "null");
	    ఆపు;
	స్థితి JIO_PIPE:
	    STR_LITERAL_సమితి(s, "pipe");
	    ఆపు;
	స్థితి JIO_FILE:
	    STR_LITERAL_సమితి(s, "file");
	    ఆపు;
	స్థితి JIO_బఫరు:
	    STR_LITERAL_సమితి(s, "buffer");
	    ఆపు;
	స్థితి JIO_బహిర్గతం:
	    STR_LITERAL_సమితి(s, "out");
	    ఆపు;
	అప్రమేయ:
	    STR_LITERAL_సమితి(s, "");
	    ఆపు;
    }
    dict_కూడు_string_len(dict, namebuf, s.string, (పూర్ణాంకం)s.నిడివి);

    STRCPY(namebuf + తోక, "timeout");
    dict_కూడు_సంఖ్య(dict, namebuf, chanpart->ch_timeout);
}

    స్థిర శూన్య
channel_info(channel_T *channel, dict_T *dict)
{
    dict_కూడు_సంఖ్య(dict, "id", channel->ch_id);
    dict_కూడు_string(dict, "status", (char_u *)channel_స్థితి(channel, -1));

    ఒకవేళ (channel->ch_hostname != ఖాళీ)
    {
	ఒకవేళ (channel->ch_పోర్టు)
	    dict_కూడు_సంఖ్య(dict, "port", channel->ch_పోర్టు);
	లేకపోతే
	    // Unix-domain socket.
	    dict_కూడు_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    లేకపోతే
    {
	channel_part_info(channel, dict, "out", PART_బహిర్గతం);
	channel_part_info(channel, dict, "err", PART_ERR);
	channel_part_info(channel, dict, "in", PART_లోపల);
    }
}

/*
 * Close channel "channel".
 * Trigger the close callback if "invoke_close_cb" is TRUE.
 * Does not clear the buffers.
 */
    శూన్య
channel_close(channel_T *channel, పూర్ణాంకం invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#నిర్వచితమైతే FEAT_GUI
    channel_gui_unregister(channel);
#అయితే_ముగింపు

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_లోపల);
    ch_close_part(channel, PART_బహిర్గతం);
    ch_close_part(channel, PART_ERR);

    ఒకవేళ (invoke_close_cb)
    {
	ch_part_T	part;

#నిర్వచితమైతే FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	పదం_channel_closing(channel);
#అయితే_ముగింపు
	// Invoke callbacks and flush buffers before the close callback.
	ఒకవేళ (channel->ch_close_cb.cb_name != ఖాళీ)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	{
	    ఒకవేళ (channel->ch_close_cb.cb_name != ఖాళీ
			    || channel->ch_part[part].ch_bufref.br_buf != ఖాళీ)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ఒకవేళ (channel->ch_close_cb.cb_name == ఖాళీ)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_పేర్లు[part]);
		అయితే (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ఒకవేళ (channel->ch_close_cb.cb_name != ఖాళీ)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (అక్షరం *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      ఒకవేళ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ఒకవేళ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ఖాళీ;
#నిర్వచితమైతే FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ఖాళీ;
    channel->ch_ss_close_cb = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TERMINAL
    పదం_channel_closed(channel);
#అయితే_ముగింపు
}

/*
 * Close the "in" part channel "channel".
 */
    స్థిర శూన్య
channel_close_లోపల(channel_T *channel)
{
    ch_close_part(channel, PART_లోపల);
}

    స్థిర శూన్య
remove_from_writeque(writeq_T *wq, writeq_T *నమోదు_2)
{
    ga_clear(&నమోదు_2->wq_ga);
    wq->wq_తదుపరి = నమోదు_2->wq_తదుపరి;
    ఒకవేళ (wq->wq_తదుపరి == ఖాళీ)
	wq->wq_prev = ఖాళీ;
    లేకపోతే
	wq->wq_తదుపరి->wq_prev = ఖాళీ;
    vim_free(నమోదు_2);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    స్థిర శూన్య
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_తల = &ch_part->ch_json_తల;
    cbq_T   *cb_తల = &ch_part->ch_cb_తల;

    అయితే (channel_peek(channel, part) != ఖాళీ)
	vim_free(channel_get(channel, part, ఖాళీ));

    అయితే (cb_తల->cq_తదుపరి != ఖాళీ)
    {
	cbq_T *కణుపు = cb_తల->cq_తదుపరి;

	remove_cb_కణుపు(cb_తల, కణుపు);
	free_callback(&కణుపు->cq_callback);
	vim_free(కణుపు);
    }

    అయితే (json_తల->jq_తదుపరి != ఖాళీ)
    {
	free_tv(json_తల->jq_తదుపరి->jq_విలువ);
	remove_json_కణుపు(json_తల, json_తల->jq_తదుపరి);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_ఖండం_ids);

    అయితే (ch_part->ch_writeque.wq_తదుపరి != ఖాళీ)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_తదుపరి);
}

/*
 * Clear all the read buffers on "channel".
 */
    శూన్య
channel_clear(channel_T *channel)
{
    ch_log(channel, "Clearing channel");
    VIM_CLEAR(channel->ch_hostname);
    channel_clear_one(channel, PART_SOCK);
    channel_clear_one(channel, PART_బహిర్గతం);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_లోపల);
    free_callback(&channel->ch_callback);
    free_callback(&channel->ch_close_cb);
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
channel_free_all(శూన్య)
{
    channel_T *channel;

    ch_log(ఖాళీ, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#అయితే_ముగింపు


// Sent when the netbeans channel is found closed when reading.
#నిర్వచించు DETACH_సందేశం_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#నిర్వచించు MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    స్థిర పూర్ణాంకం
is_channel_write_మిగిలిన(chanpart_T *లోపల_part)
{
    buf_T *buf = లోపల_part->ch_bufref.br_buf;

    ఒకవేళ (లోపల_part->ch_writeque.wq_తదుపరి != ఖాళీ)
	తిరిగి TRUE;
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి FALSE;
    తిరిగి లోపల_part->ch_buf_append
	    ? (లోపల_part->ch_buf_bot < buf->b_ml.ml_పంక్తి_లెక్క)
	    : (లోపల_part->ch_buf_శిఖరం <= లోపల_part->ch_buf_bot
			    && లోపల_part->ch_buf_శిఖరం <= buf->b_ml.ml_పంక్తి_లెక్క);
}

#ఒకవేళ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    స్థిర పూర్ణాంకం
channel_fill_wfds(పూర్ణాంకం maxfd_arg, fd_set *wfds)
{
    పూర్ణాంకం		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *లోపల_part = &ch->ch_part[PART_లోపల];

	ఒకవేళ (లోపల_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (పూర్ణాంకం)లోపల_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_మిగిలిన(లోపల_part))
	{
	    FD_SET((పూర్ణాంకం)లోపల_part->ch_fd, wfds);
	    ఒకవేళ ((పూర్ణాంకం)లోపల_part->ch_fd >= maxfd)
		maxfd = (పూర్ణాంకం)లోపల_part->ch_fd + 1;
	}
    }
    తిరిగి maxfd;
}
#లేకపోతే
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    స్థిర పూర్ణాంకం
channel_fill_poll_write(పూర్ణాంకం nfd_లోపల, నిర్మాణం pollfd *fds)
{
    పూర్ణాంకం		nfd = nfd_లోపల;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *లోపల_part = &ch->ch_part[PART_లోపల];

	ఒకవేళ (లోపల_part->ch_fd != INVALID_FD
		&& is_channel_write_మిగిలిన(లోపల_part))
	{
	    లోపల_part->ch_poll_idx = nfd;
	    fds[nfd].fd = లోపల_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	లేకపోతే
	    లోపల_part->ch_poll_idx = -1;
    }
    తిరిగి nfd;
}
#అయితే_ముగింపు

రకనిర్వచనం గణన {
    CW_READY,
    CW_NOT_READY,
    CW_ERROR
} channel_wait_ఫలితం;

/*
 * Check for reading from "fd" with "timeout" msec.
 * Return CW_READY when there is something to read.
 * Return CW_NOT_READY when there is nothing to read.
 * Return CW_ERROR when there is an error.
 */
    స్థిర channel_wait_ఫలితం
channel_wait(channel_T *channel, sock_T fd, పూర్ణాంకం timeout)
{
    ఒకవేళ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#నిర్వచితమైతే MSWIN
    ఒకవేళ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	పూర్ణాంకం	sleep_సమయం;
	DWORD	deadline = Get_Tick_లెక్క() + timeout;
	పూర్ణాంకం	delay = 1;

	// reading from a pipe, not a socket
	అయితే (TRUE)
	{
	    పూర్ణాంకం r = PeekNamedPipe((HANDLE)fd, ఖాళీ, 0, ఖాళీ, &nread, ఖాళీ);

	    ఒకవేళ (r && nread > 0)
		తిరిగి CW_READY;

	    ఒకవేళ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ఖాళీ);
	    }
	    లేకపోతే ఒకవేళ (r == 0)
		తిరిగి CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_పంక్తులు();

	    sleep_సమయం = deadline - Get_Tick_లెక్క();
	    ఒకవేళ (sleep_సమయం <= 0)
		ఆపు;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ఒకవేళ (sleep_సమయం > delay)
		sleep_సమయం = delay;
	    Sleep(sleep_సమయం);
	    delay = delay * 2;
	    ఒకవేళ (delay > 10)
		delay = 10;
	}
    }
    లేకపోతే
#అయితే_ముగింపు
    {
#ఒకవేళ defined(HAVE_SELECT)
	నిర్మాణం timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	పూర్ణాంకం		ret;
	పూర్ణాంకం		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	కొరకు (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((పూర్ణాంకం)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (పూర్ణాంకం)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ఖాళీ, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ఒకవేళ (ret == -1 && errno == EINTR)
		కొనసాగు;
# endif
	    ఒకవేళ (ret > 0)
	    {
		ఒకవేళ (FD_ISSET(fd, &rfds))
		    తిరిగి CW_READY;
		channel_write_any_పంక్తులు();
		కొనసాగు;
	    }
	    ఆపు;
	}
#లేకపోతే
	కొరకు (;;)
	{
	    నిర్మాణం pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    పూర్ణాంకం		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ఒకవేళ (poll(fds, nfd, timeout) > 0)
	    {
		ఒకవేళ (fds[0].revents & POLLIN)
		    తిరిగి CW_READY;
		channel_write_any_పంక్తులు();
		కొనసాగు;
	    }
	    ఆపు;
	}
#అయితే_ముగింపు
    }
    తిరిగి CW_NOT_READY;
}

    స్థిర శూన్య
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, పూర్ణాంకం is_err, అక్షరం *func)
{
    అక్షరం	సందేశం[] = "%s(): Read %s from ch_part[%d], closing";

    ఒకవేళ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, సందేశం, func, "error", part);
    లేకపోతే
	ch_log(channel, సందేశం, func, "EOF", part);

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
    ఒకవేళ (channel->ch_nb_close_cb != ఖాళీ)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_సందేశం_RAW,
			      (పూర్ణాంకం)STRLEN_LITERAL(DETACH_సందేశం_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ఒకవేళ (part == PART_బహిర్గతం || part == PART_ERR)
    {
	ch_part_T other = part == PART_బహిర్గతం ? PART_ERR : PART_బహిర్గతం;

	ఒకవేళ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#నిర్వచితమైతే FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#అయితే_ముగింపు
}

    స్థిర శూన్య
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    ఒకవేళ (channel->ch_nb_close_cb != ఖాళీ)
	(*channel->ch_nb_close_cb)();
#నిర్వచితమైతే FEAT_SOCKETSERVER
    ఒకవేళ (channel->ch_ss_close_cb != ఖాళీ)
	channel->ch_ss_close_cb(channel);
#అయితే_ముగింపు
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    స్థిర శూన్య
channel_read(channel_T *channel, ch_part_T part, అక్షరం *func)
{
    స్థిర char_u	*buf = ఖాళీ;
    పూర్ణాంకం			len = 0;
    పూర్ణాంకం			readlen = 0;
    sock_T		fd;
    పూర్ణాంకం			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ఒకవేళ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_పేర్లు[part]);
	తిరిగి;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ఒకవేళ (buf == ఖాళీ)
    {
	buf = alloc(MAXMSGSIZE);
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    కొరకు (;;)
    {
	ఒకవేళ (channel_wait(channel, fd, 0) != CW_READY)
	    ఆపు;
	ఒకవేళ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    నిర్మాణం sockaddr_storage	client;

	    newchannel = కూడు_channel();
	    ఒకవేళ (newchannel == ఖాళీ)
	    {
		ch_error(ఖాళీ, "Cannot allocate channel.");
		తిరిగి;
	    }
	    socklen = పరిమాణం(client);
	    newfd = accept(fd, (నిర్మాణం sockaddr*)&client, &socklen);
	    ఒకవేళ (newfd < 0)
	    {
		ch_error(ఖాళీ, "Cannot accept channel.");
		channel_free(newchannel);
		తిరిగి;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_గమ్యం_be_closed |= (1U << PART_SOCK);

#నిర్వచితమైతే FEAT_GUI
	    channel_gui_రిజిస్టరు_one(newchannel, PART_SOCK);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SOCKETSERVER
	    ఒకవేళ (channel->ch_ss_accept_cb != ఖాళీ)
	    {
		channel->ch_ss_accept_cb(newchannel);
		తిరిగి;
	    }
#అయితే_ముగింపు

	    ఒకవేళ (client.ss_family == AF_INET)
	    {
#నిర్వచితమైతే HAVE_INET_NTOP
		అక్షరం addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((నిర్మాణం sockaddr_లోపల*)&client)->sin_addr,
			addr, పరిమాణం(addr));
		vim_snprintf((అక్షరం *)namebuf, పరిమాణం(namebuf), "%s:%d",
			addr,
			ntohs(((నిర్మాణం sockaddr_లోపల*)&client)->sin_పోర్టు));
#లేకపోతే
		vim_snprintf((అక్షరం *)namebuf, పరిమాణం(namebuf), "%s:%d",
		    inet_ntoa(((నిర్మాణం sockaddr_లోపల*)&client)->sin_addr),
		    ntohs(((నిర్మాణం sockaddr_లోపల*)&client)->sin_పోర్టు));
#అయితే_ముగింపు
	    }
#నిర్వచితమైతే HAVE_INET_NTOP
	    లేకపోతే ఒకవేళ (client.ss_family == AF_INET6)
	    {
		అక్షరం addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((నిర్మాణం sockaddr_in6*)&client)->sin6_addr,
			addr, పరిమాణం(addr));
		vim_snprintf((అక్షరం *)namebuf, పరిమాణం(namebuf), "[%s]:%d",
			addr,
			ntohs(((నిర్మాణం sockaddr_in6*)&client)->sin6_పోర్టు));
	    }
#అయితే_ముగింపు
	    లేకపోతే ఒకవేళ (client.ss_family == AF_UNIX)
		vim_snprintf((అక్షరం *)namebuf, పరిమాణం(namebuf),
							    "unix:anonymous");
	    లేకపోతే
		vim_snprintf((అక్షరం *)namebuf, పరిమాణం(namebuf), "unknown");
	    ++సురక్షిత_గమ్యం_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --సురక్షిత_గమ్యం_invoke_callback;
	    clear_tv(&argv[1]);
	    తిరిగి;
	}
	ఒకవేళ (use_socket)
	    len = sock_read(fd, (అక్షరం *)buf, MAXMSGSIZE);
	లేకపోతే
	    len = fd_read(fd, (అక్షరం *)buf, MAXMSGSIZE);
	ఒకవేళ (len <= 0)
	    ఆపు;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ఒకవేళ (readlen <= 0)
    {
	ఒకవేళ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ఒకవేళ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    లేకపోతే ఒకవేళ (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#అయితే_ముగింపు
}

#నిర్వచితమైతే FEAT_SOCKETSERVER

    శూన్య
channel_check(channel_T *channel, ch_part_T part)
{
    channel_read(channel, part, "channel_check");
}

#అయితే_ముగింపు

/*
 * Read from RAW or NL "channel"/"part".  Blocks until there is something to
 * read or the timeout expires.
 * When "raw" is TRUE don't block waiting on a NL.
 * Does not trigger timers or handle messages.
 * Returns what was read in allocated memory.
 * Returns NULL in case of error or timeout.
 */
    స్థిర char_u *
channel_read_ఖండం(
	channel_T *channel, ch_part_T part, పూర్ణాంకం timeout, పూర్ణాంకం raw, పూర్ణాంకం *outlen)
{
    char_u	*buf;
    char_u	*సందేశం;
    ch_బహుళకం_T	బహుళకం = channel->ch_part[part].ch_బహుళకం;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*కణుపు;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  బహుళకం == CH_బహుళకం_RAW ? "RAW"
				: బహుళకం == CH_బహుళకం_BLOB ? "BLOB" : "NL", timeout);

    అయితే (TRUE)
    {
	కణుపు = channel_peek(channel, part);
	ఒకవేళ (కణుపు != ఖాళీ)
	{
	    ఒకవేళ (బహుళకం == CH_బహుళకం_RAW || బహుళకం == CH_బహుళకం_BLOB
		    || (బహుళకం == CH_బహుళకం_NL
					   && channel_మొదటి_nl(కణుపు) != ఖాళీ))
		// got a complete message
		ఆపు;
	    ఒకవేళ (channel_collapse(channel, part, బహుళకం == CH_బహుళకం_NL) == OK)
		కొనసాగు;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ఒకవేళ (raw || fd == INVALID_FD)
		ఆపు;
	}

	// Wait for up to the channel timeout.
	ఒకవేళ (fd == INVALID_FD)
	    తిరిగి ఖాళీ;
	ఒకవేళ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    తిరిగి ఖాళీ;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ఒకవేళ (బహుళకం == CH_బహుళకం_RAW || బహుళకం == CH_బహుళకం_BLOB || outlen != ఖాళీ)
    {
	సందేశం = channel_get_all(channel, part, outlen);
    }
    లేకపోతే
    {
	char_u *p;

	buf = కణుపు->rq_బఫరు;
	nl = channel_మొదటి_nl(కణుపు);

	// Convert NUL to NL, the internal representation.
	కొరకు (p = buf; (nl == ఖాళీ || p < nl) && p < buf + కణుపు->rq_buflen; ++p)
	    ఒకవేళ (*p == NUL)
		*p = NL;

	ఒకవేళ (nl == ఖాళీ)
	{
	    // must be a closed channel with missing NL
	    సందేశం = channel_get(channel, part, ఖాళీ);
	}
	లేకపోతే ఒకవేళ (nl + 1 == buf + కణుపు->rq_buflen)
	{
	    // get the whole buffer
	    సందేశం = channel_get(channel, part, ఖాళీ);
	    *nl = NUL;
	}
	లేకపోతే
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    సందేశం = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (పూర్ణాంకం)(nl - buf) + 1);
	}
    }
    ఒకవేళ (ch_log_చురుకైన())
	ch_log(channel, "Returning %d bytes",
		outlen != ఖాళీ ? *outlen : (పూర్ణాంకం)STRLEN(సందేశం));
    తిరిగి సందేశం;
}

స్థిర పూర్ణాంకం channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    పూర్ణాంకం
channel_లోపల_blocking_wait(శూన్య)
{
    తిరిగి channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    స్థిర శూన్య
channel_wait_reading(channel_T *channel వాడని, sock_T fd వాడని, పూర్ణాంకం timeout)
{
#నిర్వచితమైతే MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (శూన్య)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#లేకపోతే
    channel_wait_any(timeout);
#అయితే_ముగింపు
}

/*
 * Read one JSON message with ID "id" from "channel"/"part" and store the
 * result in "rettv".
 * When "id" is -1 accept any message;
 * Blocks until the message is received or the timeout is reached.
 * In corner cases this can be called recursively, that is why ch_block_ids is
 * a list.
 */
    స్థిర పూర్ణాంకం
channel_read_json_ఖండం(
	channel_T   *channel,
	ch_part_T   part,
	పూర్ణాంకం	    timeout_arg,
	పూర్ణాంకం	    id,
	typval_T    **rettv)
{
    పూర్ణాంకం		మరిన్ని;
    sock_T	fd;
    పూర్ణాంకం		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_బహుళకం_T	బహుళకం = channel->ch_part[part].ch_బహుళకం;
    పూర్ణాంకం		retval = FAIL;
#నిర్వచితమైతే ELAPSED_FUNC
    elapsed_T	ప్రారంభం_tv;

    ELAPSED_INIT(ప్రారంభం_tv);
#అయితే_ముగింపు

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ఒకవేళ (id >= 0)
	channel_కూడు_ఖండం_id(chanpart, id);

    కొరకు (;;)
    {
	ఒకవేళ (బహుళకం == CH_బహుళకం_LSP || బహుళకం == CH_బహుళకం_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (శూన్య)channel_collapse(channel, part, FALSE);

	మరిన్ని = channel_parse_json(channel, part, false);

	// search for message "id"
	ఒకవేళ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    ఆపు;
	}

	ఒకవేళ (!మరిన్ని)
	{
	    శూన్య *prev_readahead_ptr = channel_readahead_సూచిక(channel, part);
	    శూన్య *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ఒకవేళ (channel_parse_messages())
		కొనసాగు;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_సూచిక(channel, part);
	    ఒకవేళ (readahead_ptr != ఖాళీ && readahead_ptr != prev_readahead_ptr)
		కొనసాగు;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#నిర్వచితమైతే ELAPSED_FUNC
	    timeout -= (పూర్ణాంకం)ELAPSED_FUNC(ప్రారంభం_tv);
#అయితే_ముగింపు
	    fd = chanpart->ch_fd;
	    ఒకవేళ (timeout <= 0 || fd == INVALID_FD)
	    {
		ఒకవేళ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		ఆపు;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ఒకవేళ (chanpart->ch_wait_len > 0)
	    {
		పూర్ణాంకం	ఎడమ_2;
#నిర్వచితమైతే MSWIN
		ఎడమ_2 = chanpart->ch_deadline - Get_Tick_లెక్క() + 1;
#లేకపోతే
		{
		    నిర్మాణం timeval now_tv;

		    gettimeofday(&now_tv, ఖాళీ);
		    ఎడమ_2 = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#అయితే_ముగింపు
		ఒకవేళ (ఎడమ_2 < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		లేకపోతే ఒకవేళ (ఎడమ_2 < timeout)
		    timeout = ఎడమ_2;
	    }
	    channel_wait_reading(channel, fd, timeout);
#నిర్వచితం_కాకపోతే ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#అయితే_ముగింపు
	}
    }
    ఒకవేళ (id >= 0)
	channel_remove_ఖండం_id(chanpart, id);
    --channel_blocking_wait;

    తిరిగి retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, పూర్ణాంకం check_open, పూర్ణాంకం reading, ch_part_T part)
{
    channel_T	*channel = ఖాళీ;
    పూర్ణాంకం		has_readahead = FALSE;

    ఒకవేళ (tv->v_type == VAR_పని)
    {
	ఒకవేళ (tv->vval.v_పని != ఖాళీ)
	    channel = tv->vval.v_పని->jv_channel;
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    లేకపోతే
    {
	semsg(_(e_invalid_వాదం_str), tv_get_string(tv));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (channel != ఖాళీ && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_లెక్క ? part : channel_part_read(channel));

    ఒకవేళ (check_open && (channel == ఖాళీ || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	తిరిగి ఖాళీ;
    }
    తిరిగి channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    స్థిర శూన్య
common_channel_read(typval_T *argvars, typval_T *rettv, పూర్ణాంకం raw, పూర్ణాంకం blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_లెక్క;
    jobopt_T	opt;
    పూర్ణాంకం		బహుళకం;
    పూర్ణాంకం		timeout;
    పూర్ణాంకం		id = -1;
    typval_T	*listtv = ఖాళీ;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_chan_or_పని_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    clear_పని_options(&opt);
    ఒకవేళ (get_పని_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	వెళ్ళు theend;

    ఒకవేళ (opt.jo_సమితి & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ఒకవేళ (channel == ఖాళీ)
	వెళ్ళు theend;

    ఒకవేళ (part == PART_లెక్క)
	part = channel_part_read(channel);
    బహుళకం = channel_get_బహుళకం(channel, part);
    timeout = channel_get_timeout(channel, part);
    ఒకవేళ (opt.jo_సమితి & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ఒకవేళ (blob || బహుళకం == CH_బహుళకం_BLOB)
    {
	పూర్ణాంకం	    outlen = 0;
	char_u  *p = channel_read_ఖండం(channel, part,
		timeout, TRUE, &outlen);
	ఒకవేళ (p != ఖాళీ)
	{
	    blob_T	*b = blob_alloc();

	    ఒకవేళ (b != ఖాళీ)
	    {
		b->bv_ga.ga_len = outlen;
		ఒకవేళ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		లేకపోతే
		{
		    జ్ఞాపకం_నకలు(b->bv_ga.ga_దత్తాంశం, p, outlen);
		    rettv_blob_సమితి(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    లేకపోతే ఒకవేళ (raw || బహుళకం == CH_బహుళకం_RAW || బహుళకం == CH_బహుళకం_NL)
	rettv->vval.v_string = channel_read_ఖండం(channel, part,
		timeout, raw, ఖాళీ);
    లేకపోతే
    {
	ఒకవేళ (opt.jo_సమితి & JO_ID)
	    id = opt.jo_id;
	channel_read_json_ఖండం(channel, part, timeout, id, &listtv);
	ఒకవేళ (listtv != ఖాళీ)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	లేకపోతే
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_సంఖ్య = VVAL_NONE;
	}
    }

theend:
    free_పని_options(&opt);
}

#ఒకవేళ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    శూన్య
channel_handle_events(పూర్ణాంకం only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ఒకవేళ (only_keep_open && !channel->ch_keep_open)
	    కొనసాగు;

	// check the socket and pipes
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ఒకవేళ (fd == INVALID_FD)
		కొనసాగు;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    పూర్ణాంకం r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_బహిర్గతం || part == PART_ERR)) ? 1 : 0);

	    ఒకవేళ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    లేకపోతే ఒకవేళ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ఒకవేళ (channel->ch_పని != ఖాళీ)
	{
	    పని_T *పని = channel->ch_పని;

	    ఒకవేళ (పని->jv_tty_బహిర్గతం != ఖాళీ && పని->jv_స్థితి == పని_ముగిసిన)
		కొరకు (part = PART_SOCK; part < PART_లెక్క; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    పూర్ణాంకం
channel_any_keep_open(శూన్య)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ఒకవేళ (channel->ch_keep_open)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}
#అయితే_ముగింపు

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    శూన్య
channel_సమితి_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    పూర్ణాంకం		fd = ch_part->ch_fd;

    ఒకవేళ (fd == INVALID_FD)
	తిరిగి;

#నిర్వచితమైతే MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#లేకపోతే
    (శూన్య)fcntl(fd, F_SETFL, O_NONBLOCK);
#అయితే_ముగింపు
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    పూర్ణాంకం
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	పూర్ణాంకం	  len_arg,
	అక్షరం	  *fun)
{
    పూర్ణాంకం		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    పూర్ణాంకం		did_use_వరుస = FALSE;

    fd = ch_part->ch_fd;
    ఒకవేళ (fd == INVALID_FD)
    {
	ఒకవేళ (!channel->ch_error && fun != ఖాళీ)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	తిరిగి FAIL;
    }

    ఒకవేళ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_సమితి_nonblock(channel, part);

    ఒకవేళ (ch_log_చురుకైన())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_సందేశం = 0;
    }

    కొరకు (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	పూర్ణాంకం	    len;

	ఒకవేళ (wq->wq_తదుపరి != ఖాళీ)
	{
	    // first write what was queued
	    buf = wq->wq_తదుపరి->wq_ga.ga_దత్తాంశం;
	    len = wq->wq_తదుపరి->wq_ga.ga_len;
	    did_use_వరుస = TRUE;
	}
	లేకపోతే
	{
	    ఒకవేళ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		తిరిగి OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ఒకవేళ (part == PART_SOCK)
	    res = sock_write(fd, (అక్షరం *)buf, len);
	లేకపోతే
	{
	    res = fd_write(fd, (అక్షరం *)buf, len);
#నిర్వచితమైతే MSWIN
	    ఒకవేళ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ఖాళీ);
	    }
#అయితే_ముగింపు
	}
	ఒకవేళ (res < 0 && (errno == EWOULDBLOCK
#నిర్వచితమైతే EAGAIN
			|| errno == EAGAIN
#అయితే_ముగింపు
		    ))
	    res = 0; // nothing got written

	ఒకవేళ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *నమోదు_2 = wq->wq_తదుపరి;

	    ఒకవేళ (did_use_వరుస)
		ch_log(channel, "Sent %d bytes now", res);
	    ఒకవేళ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ఒకవేళ (నమోదు_2 != ఖాళీ)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, నమోదు_2);
		    కొనసాగు;
		}
		ఒకవేళ (did_use_వరుస)
		    ch_log(channel, "Write queue empty");
	    }
	    లేకపోతే
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ఒకవేళ (నమోదు_2 != ఖాళీ)
		{
		    ఒకవేళ (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(నమోదు_2->wq_ga.ga_దత్తాంశం,
				    (అక్షరం *)నమోదు_2->wq_ga.ga_దత్తాంశం + res,
				    len - res);
			నమోదు_2->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		లేకపోతే
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ఒకవేళ (wq->wq_prev != ఖాళీ
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *చివరి = wq->wq_prev;

		    // append to the last entry
		    ఒకవేళ (len > 0 && ga_grow(&చివరి->wq_ga, len) == OK)
		    {
			mch_memmove((అక్షరం *)చివరి->wq_ga.ga_దత్తాంశం
							  + చివరి->wq_ga.ga_len,
				    buf, len);
			చివరి->wq_ga.ga_len += len;
		    }
		}
		లేకపోతే
		{
		    writeq_T *చివరి = ALLOC_ONE(writeq_T);

		    ఒకవేళ (చివరి != ఖాళీ)
		    {
			చివరి->wq_prev = wq->wq_prev;
			చివరి->wq_తదుపరి = ఖాళీ;
			ఒకవేళ (wq->wq_prev == ఖాళీ)
			    wq->wq_తదుపరి = చివరి;
			లేకపోతే
			    wq->wq_prev->wq_తదుపరి = చివరి;
			wq->wq_prev = చివరి;
			ga_init2(&చివరి->wq_ga, 1, 1000);
			ఒకవేళ (len > 0 && ga_grow(&చివరి->wq_ga, len) == OK)
			{
			    mch_memmove(చివరి->wq_ga.ga_దత్తాంశం, buf, len);
			    చివరి->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	లేకపోతే ఒకవేళ (res != len)
	{
	    ఒకవేళ (!channel->ch_error && fun != ఖాళీ)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    తిరిగి FAIL;
	}

	channel->ch_error = FALSE;
	తిరిగి OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    స్థిర channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *పాఠ్యం,
	పూర్ణాంకం	    len,
	పూర్ణాంకం	    id,
	పూర్ణాంకం	    eval,
	jobopt_T    *opt,
	అక్షరం	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_పని_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి ఖాళీ;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ఒకవేళ (get_పని_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	తిరిగి ఖాళీ;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ఒకవేళ (opt->jo_callback.cb_name != ఖాళీ && *opt->jo_callback.cb_name != NUL)
    {
	ఒకవేళ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    తిరిగి ఖాళీ;
	}
	channel_సమితి_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ఒకవేళ (channel_send(channel, part_send, పాఠ్యం, len, fun) == OK
					   && opt->jo_callback.cb_name == ఖాళీ)
	తిరిగి channel;
    తిరిగి ఖాళీ;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    స్థిర శూన్య
ch_సమాసం_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం eval)
{
    char_u	*పాఠ్యం;
    typval_T	*listtv;
    channel_T	*channel;
    పూర్ణాంకం		id;
    ch_బహుళకం_T	ch_బహుళకం;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    పూర్ణాంకం		timeout;
    పూర్ణాంకం		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_chan_or_పని_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి;
    part_send = channel_part_send(channel);

    ch_బహుళకం = channel_get_బహుళకం(channel, part_send);
    ఒకవేళ (ch_బహుళకం == CH_బహుళకం_RAW || ch_బహుళకం == CH_బహుళకం_BLOB
	    || ch_బహుళకం == CH_బహుళకం_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	తిరిగి;
    }

    ఒకవేళ (ch_బహుళకం == CH_బహుళకం_LSP || ch_బహుళకం == CH_బహుళకం_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	అక్షరం		*కీలకం = ch_బహుళకం == CH_బహుళకం_LSP ? "id" : "seq";

	// return an empty dict by default
	ఒకవేళ (rettv_dict_alloc(rettv) == FAIL)
	    తిరిగి;

	ఒకవేళ (check_for_dict_arg(argvars, 1) == FAIL)
	    తిరిగి;

	d = argvars[1].vval.v_dict;
	di = dict_కనుగొను(d, (char_u *)కీలకం, -1);
	ఒకవేళ (argvars[2].v_type == VAR_DICT
		&& dict_has_కీలకం(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ఒకవేళ (di != ఖాళీ && di->di_tv.v_type != VAR_సంఖ్య
		&& (ch_బహుళకం == CH_బహుళకం_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_విలువ_for_వాదం_str), కీలకం);
	    తిరిగి;
	}

	ఒకవేళ (ch_బహుళకం == CH_బహుళకం_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_చివరి_సందేశం_id;
	    ఒకవేళ (di == ఖాళీ)
		dict_కూడు_సంఖ్య(d, "seq", id);
	    లేకపోతే
		di->di_tv.vval.v_సంఖ్య = id;
	}
	లేకపోతే ఒకవేళ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_చివరి_సందేశం_id;
	    ఒకవేళ (di == ఖాళీ)
		dict_కూడు_సంఖ్య(d, "id", id);
	    లేకపోతే
		di->di_tv.vval.v_సంఖ్య = id;
	}
	లేకపోతే
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ఒకవేళ (di != ఖాళీ && di->di_tv.v_type == VAR_సంఖ్య)
		id = di->di_tv.vval.v_సంఖ్య;
	}
	ఒకవేళ (ch_బహుళకం == CH_బహుళకం_LSP && !dict_has_కీలకం(d, "jsonrpc"))
	    dict_కూడు_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	పాఠ్యం = json_encode_lsp_సందేశం(&argvars[1]);
    }
    లేకపోతే
    {
	id = ++channel->ch_చివరి_సందేశం_id;
	పాఠ్యం = json_encode_nr_సమాసం(id, &argvars[1],
			      (ch_బహుళకం == CH_బహుళకం_JS ? JSON_JS : 0) | JSON_NL);
    }
    ఒకవేళ (పాఠ్యం == ఖాళీ)
	తిరిగి;

    channel = send_common(argvars, పాఠ్యం, (పూర్ణాంకం)STRLEN(పాఠ్యం), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(పాఠ్యం);
    ఒకవేళ (channel != ఖాళీ && eval)
    {
	ఒకవేళ (opt.jo_సమితి & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	లేకపోతే
	    timeout = channel_get_timeout(channel, part_read);
	ఒకవేళ (channel_read_json_ఖండం(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ఒకవేళ (ch_బహుళకం == CH_బహుళకం_LSP || ch_బహుళకం == CH_బహుళకం_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_సంఖ్య;
		free_tv(listtv);
	    }
	    లేకపోతే
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_చివరి->li_tv;
		list->lv_u.mat.lv_చివరి->li_tv.v_type = VAR_సంఖ్య;
		free_tv(listtv);
	    }
	}
    }
    free_పని_options(&opt);
    ఒకవేళ (ch_బహుళకం == CH_బహుళకం_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ఒకవేళ (rettv->vval.v_dict != ఖాళీ)
	    dict_కూడు_సంఖ్య(rettv->vval.v_dict, "seq", id);
    }
    లేకపోతే ఒకవేళ (ch_బహుళకం == CH_బహుళకం_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ఒకవేళ (rettv->vval.v_dict != ఖాళీ)
	    dict_కూడు_సంఖ్య(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    స్థిర శూన్య
ch_raw_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*పాఠ్యం;
    పూర్ణాంకం		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    పూర్ణాంకం		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_chan_or_పని_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type == VAR_BLOB)
    {
	పాఠ్యం = argvars[1].vval.v_blob->bv_ga.ga_దత్తాంశం;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    లేకపోతే
    {
	పాఠ్యం = tv_get_string_buf(&argvars[1], buf);
	len = (పూర్ణాంకం)STRLEN(పాఠ్యం);
    }
    channel = send_common(argvars, పాఠ్యం, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ఒకవేళ (channel != ఖాళీ && eval)
    {
	ఒకవేళ (opt.jo_సమితి & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	లేకపోతే
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_ఖండం(channel, part_read,
							timeout, TRUE, ఖాళీ);
    }
    free_పని_options(&opt);
}

#నిర్వచించు KEEP_OPEN_సమయం 20  // msec

#ఒకవేళ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    పూర్ణాంకం
channel_poll_అమరిక(పూర్ణాంకం nfd_లోపల, శూన్య *fds_లోపల, పూర్ణాంకం *towait)
{
    పూర్ణాంకం		nfd = nfd_లోపల;
    channel_T	*channel;
    నిర్మాణం	pollfd *fds = fds_లోపల;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ఒకవేళ (ch_part->ch_fd != INVALID_FD)
	    {
		ఒకవేళ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ఒకవేళ (*towait < 0 || *towait > KEEP_OPEN_సమయం)
			*towait = KEEP_OPEN_సమయం;
		}
		లేకపోతే
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    లేకపోతే
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    తిరిగి nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    పూర్ణాంకం
channel_poll_check(పూర్ణాంకం ret_లోపల, శూన్య *fds_లోపల)
{
    పూర్ణాంకం		ret = ret_లోపల;
    channel_T	*channel;
    నిర్మాణం	pollfd *fds = fds_లోపల;
    ch_part_T	part;
    పూర్ణాంకం		idx;
    chanpart_T	*లోపల_part;

    FOR_ALL_CHANNELS(channel)
    {
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ఒకవేళ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    లేకపోతే ఒకవేళ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	లోపల_part = &channel->ch_part[PART_లోపల];
	idx = లోపల_part->ch_poll_idx;
	ఒకవేళ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_ఆదానం(channel);
	    --ret;
	}
    }

    తిరిగి ret;
}
#అయితే_ముగింపు // UNIX && !HAVE_SELECT

#ఒకవేళ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    పూర్ణాంకం
channel_select_అమరిక(
	పూర్ణాంకం maxfd_లోపల,
	శూన్య *rfds_లోపల,
	శూన్య *wfds_లోపల,
	నిర్మాణం timeval *tv,
	నిర్మాణం timeval **tvp)
{
    పూర్ణాంకం		maxfd = maxfd_లోపల;
    channel_T	*channel;
    fd_set	*rfds = rfds_లోపల;
    fd_set	*wfds = wfds_లోపల;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ఒకవేళ (fd != INVALID_FD)
	    {
		ఒకవేళ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ఒకవేళ (*tvp == ఖాళీ || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_సమయం * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_సమయం * 1000;
		    }
		}
		లేకపోతే
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ఒకవేళ ((పూర్ణాంకం)fd >= FD_SETSIZE)
			కొనసాగు;
# endif
		    FD_SET((పూర్ణాంకం)fd, rfds);
		    ఒకవేళ (maxfd < (పూర్ణాంకం)fd)
			maxfd = (పూర్ణాంకం)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    తిరిగి maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    పూర్ణాంకం
channel_select_check(పూర్ణాంకం ret_లోపల, శూన్య *rfds_లోపల, శూన్య *wfds_లోపల)
{
    పూర్ణాంకం		ret = ret_లోపల;
    channel_T	*channel;
    fd_set	*rfds = rfds_లోపల;
    fd_set	*wfds = wfds_లోపల;
    ch_part_T	part;
    chanpart_T	*లోపల_part;

    FOR_ALL_CHANNELS(channel)
    {
	కొరకు (part = PART_SOCK; part < PART_లోపల; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ఒకవేళ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (పూర్ణాంకం)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    లేకపోతే ఒకవేళ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	లోపల_part = &channel->ch_part[PART_లోపల];
	ఒకవేళ (ret > 0 && లోపల_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (పూర్ణాంకం)లోపల_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(లోపల_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(లోపల_part->ch_fd, wfds);
	    channel_write_ఆదానం(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ఒకవేళ (channel->ch_పని != ఖాళీ)
	{
	    పని_T *పని = channel->ch_పని;

	    ఒకవేళ (పని->jv_tty_బహిర్గతం != ఖాళీ && పని->jv_స్థితి == పని_ముగిసిన)
		కొరకు (part = PART_SOCK; part < PART_లెక్క; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    తిరిగి ret;
}
#అయితే_ముగింపు // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    స్థిర శూన్య
channel_wait_any(దీర్ఘ msec)
{
#నిర్వచితమైతే MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    ఒకవేళ (stdio_channel->CH_బహిర్గతం_FD != INVALID_FD)
	(శూన్య)channel_wait(stdio_channel, stdio_channel->CH_బహిర్గతం_FD,
						(పూర్ణాంకం)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#లేదావేళ defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    నిర్మాణం timeval	tv;
    నిర్మాణం timeval	*tvp = &tv;
    పూర్ణాంకం			maxfd;
    పూర్ణాంకం			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_అమరిక(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ఖాళీ, tvp);
    ఒకవేళ (ret >= 0)
	(శూన్య)channel_select_check(ret, &rfds, &wfds);
#లేకపోతే
    నిర్మాణం pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    పూర్ణాంకం			nfd;
    పూర్ణాంకం			towait = (పూర్ణాంకం)msec;
    పూర్ణాంకం			ret;

    nfd = channel_poll_అమరిక(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ఒకవేళ (ret >= 0)
	(శూన్య)channel_poll_check(ret, fds);
#అయితే_ముగింపు
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    శూన్య
channel_stdio_loop(శూన్య)
{
    అయితే (stdio_channel != ఖాళీ && channel_is_open(stdio_channel))
    {
	దీర్ఘ	msec = 1000;
#నిర్వచితమైతే FEAT_TIMERS
	దీర్ఘ	due_సమయం = check_due_timer();

	ఒకవేళ (due_సమయం > 0 && due_సమయం < msec)
	    msec = due_సమయం;
#అయితే_ముగింపు
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
    పూర్ణాంకం
channel_parse_messages(శూన్య)
{
    channel_T	*channel = మొదటి_channel;
    పూర్ణాంకం		ret = FALSE;
    పూర్ణాంకం		r;
    ch_part_T	part = PART_SOCK;
    స్థిర పూర్ణాంకం	recursive = 0;
#నిర్వచితమైతే ELAPSED_FUNC
    elapsed_T	ప్రారంభం_tv;
#అయితే_ముగింపు

    // The code below may invoke callbacks, which might call us back.
    // In a recursive call channels will not be closed.
    ++recursive;
    ++సురక్షిత_గమ్యం_invoke_callback;

#నిర్వచితమైతే ELAPSED_FUNC
    ELAPSED_INIT(ప్రారంభం_tv);
#అయితే_ముగింపు

    // Only do this message when another message was given, otherwise we get
    // lots of them.
    ఒకవేళ ((did_repeated_సందేశం & REPEATED_సందేశం_LOOKING) == 0)
    {
	ch_log(ఖాళీ, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_సందేశం = REPEATED_సందేశం_LOOKING;
    }
    అయితే (channel != ఖాళీ)
    {
	ఒకవేళ (recursive == 1)
	{
	    ఒకవేళ (channel_can_close(channel))
	    {
		channel->ch_గమ్యం_be_closed = (1U << PART_లెక్క);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = మొదటి_channel;
		కొనసాగు;
	    }
	    ఒకవేళ (channel->ch_గమ్యం_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ఒకవేళ (channel->ch_పని != ఖాళీ)
		    channel->ch_పని->jv_channel = ఖాళీ;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = మొదటి_channel;
		కొనసాగు;
	    }
	    ఒకవేళ (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = మొదటి_channel;
		part = PART_SOCK;
		కొనసాగు;
	    }
	}

#నిర్వచితమైతే FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	ఒకవేళ (part == PART_బహిర్గతం
		&& channel->ch_పని != ఖాళీ
		&& channel->ch_పని->jv_tty_బహిర్గతం != ఖాళీ
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    కొనసాగు;
	}
#అయితే_ముగింపు
	ఒకవేళ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ఒకవేళ (r == OK)
		ret = TRUE;
	    ఒకవేళ (channel_unref(channel) || (r == OK
#నిర్వచితమైతే ELAPSED_FUNC
			// Limit the time we loop here to 100 msec, otherwise
			// Vim becomes unresponsive when the callback takes
			// more than a bit of time.
			&& ELAPSED_FUNC(ప్రారంభం_tv) < 100L
#అయితే_ముగింపు
			))
	    {
		// channel was freed or something was done, start over
		channel = మొదటి_channel;
		part = PART_SOCK;
		కొనసాగు;
	    }
	}
	ఒకవేళ (part < PART_ERR)
	    ++part;
	లేకపోతే
	{
	    channel = channel->ch_తదుపరి;
	    part = PART_SOCK;
	}
    }

    ఒకవేళ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --సురక్షిత_గమ్యం_invoke_callback;
    --recursive;

    తిరిగి ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    పూర్ణాంకం
channel_any_readahead(శూన్య)
{
    channel_T	*channel = మొదటి_channel;
    ch_part_T	part = PART_SOCK;

    అయితే (channel != ఖాళీ)
    {
	ఒకవేళ (channel_has_readahead(channel, part))
	    తిరిగి TRUE;
	ఒకవేళ (part < PART_ERR)
	    ++part;
	లేకపోతే
	{
	    channel = channel->ch_తదుపరి;
	    part = PART_SOCK;
	}
    }
    తిరిగి FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_channel(పూర్ణాంకం copyID)
{
    పూర్ణాంకం		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    కొరకు (channel = మొదటి_channel; !abort && channel != ఖాళీ;
						   channel = channel->ch_తదుపరి)
	ఒకవేళ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || సమితి_ref_లోపల_అంశం(&tv, copyID, ఖాళీ, ఖాళీ, ఖాళీ);
	}
    తిరిగి abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    స్థిర ch_part_T
channel_part_send(channel_T *channel)
{
    ఒకవేళ (channel->CH_SOCK_FD == INVALID_FD)
	తిరిగి PART_లోపల;
    తిరిగి PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    స్థిర ch_part_T
channel_part_read(channel_T *channel)
{
    ఒకవేళ (channel->CH_SOCK_FD == INVALID_FD)
	తిరిగి PART_బహిర్గతం;
    తిరిగి PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    స్థిర ch_బహుళకం_T
channel_get_బహుళకం(channel_T *channel, ch_part_T part)
{
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి CH_బహుళకం_JSON;
    తిరిగి channel->ch_part[part].ch_బహుళకం;
}

/*
 * Return the timeout of "channel"/"part"
 */
    స్థిర పూర్ణాంకం
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    తిరిగి channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    శూన్య
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_సంఖ్య = 0;
    ఒకవేళ (లోపల_vim9script() && check_for_chan_or_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ఒకవేళ (channel != ఖాళీ)
	rettv->vval.v_సంఖ్య = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_బహిర్గతం)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    శూన్య
f_ch_close(typval_T *argvars, typval_T *rettv వాడని)
{
    channel_T *channel;

    ఒకవేళ (లోపల_vim9script() && check_for_chan_or_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ఒకవేళ (channel != ఖాళీ)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    శూన్య
f_ch_close_లోపల(typval_T *argvars, typval_T *rettv వాడని)
{
    channel_T *channel;

    ఒకవేళ (లోపల_vim9script() && check_for_chan_or_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ఒకవేళ (channel != ఖాళీ)
	channel_close_లోపల(channel);
}

/*
 * "ch_getbufnr()" function
 */
    శూన్య
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_సంఖ్య = -1;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_chan_or_పని_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	తిరిగి;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి;

    char_u	*what = tv_get_string(&argvars[1]);
    పూర్ణాంకం		part;
    ఒకవేళ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    లేకపోతే ఒకవేళ (STRCMP(what, "out") == 0)
	part = PART_బహిర్గతం;
    లేకపోతే ఒకవేళ (STRCMP(what, "in") == 0)
	part = PART_లోపల;
    లేకపోతే
	part = PART_SOCK;
    ఒకవేళ (channel->ch_part[part].ch_bufref.br_buf != ఖాళీ)
	rettv->vval.v_సంఖ్య =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    శూన్య
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    ఒకవేళ (లోపల_vim9script() && check_for_chan_or_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి;

    rettv->v_type = VAR_పని;
    rettv->vval.v_పని = channel->ch_పని;
    ఒకవేళ (channel->ch_పని != ఖాళీ)
	++channel->ch_పని->jv_refcount;
}

/*
 * "ch_info()" function
 */
    శూన్య
f_ch_info(typval_T *argvars, typval_T *rettv వాడని)
{
    channel_T *channel;

    ఒకవేళ (లోపల_vim9script() && check_for_chan_or_పని_arg(argvars, 0) == FAIL)
	తిరిగి;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ఒకవేళ (channel != ఖాళీ && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    శూన్య
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    శూన్య
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    శూన్య
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    శూన్య
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    శూన్య
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    శూన్య
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_సమాసం_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    శూన్య
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_సమాసం_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    శూన్య
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    శూన్య
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    శూన్య
f_ch_setoptions(typval_T *argvars, typval_T *rettv వాడని)
{
    channel_T	*channel;
    jobopt_T	opt;

    ఒకవేళ (check_restricted() || check_secure())
	తిరిగి;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_chan_or_పని_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ఒకవేళ (channel == ఖాళీ)
	తిరిగి;
    clear_పని_options(&opt);
    ఒకవేళ (get_పని_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_బహుళకం_ALL, 0) == OK)
	channel_సమితి_options(channel, &opt);
    free_పని_options(&opt);
}

/*
 * "ch_status()" function
 */
    శూన్య
f_ch_స్థితి(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    పూర్ణాంకం		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_chan_or_పని_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	తిరిగి;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_పని_options(&opt);
	ఒకవేళ (get_పని_options(&argvars[1], &opt, JO_PART, 0) == OK
						     && (opt.jo_సమితి & JO_PART))
	    part = opt.jo_part;
    }

    rettv->vval.v_string = vim_strsave((char_u *)channel_స్థితి(channel, part));
}

/*
 * Get a string with information about the channel in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
channel_గమ్యం_string_buf(typval_T *varp, char_u *buf)
{
    channel_T *channel = varp->vval.v_channel;
    అక్షరం      *స్థితి_2 = channel_స్థితి(channel, -1);

    ఒకవేళ (channel == ఖాళీ)
	vim_snprintf((అక్షరం *)buf, NUMBUFLEN, "channel %s", స్థితి_2);
    లేకపోతే
	vim_snprintf((అక్షరం *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, స్థితి_2);
    తిరిగి buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_కనుగొను(పూర్ణాంకం ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ఒకవేళ (ch->ch_id == ch_id)
	    తిరిగి ch;
    తిరిగి ఖాళీ;
}

#అయితే_ముగింపు // FEAT_JOB_CHANNEL
