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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)

// TRUE when netbeans is running with a GUI.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# define CH_HAS_GUI (gui.ਵਿੱਚ_use || gui.starting)
#ਜੇ_ਅੰਤ

// Note: when making changes here also adjust configure.ac.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
// WinSock API is separated from C API, thus we can't use read(), write(),
// errno...
# define SOCK_ERRNO errno = WSAGet_ਆਖਰੀ_Error()
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
# define UNIX_ਰਸਤਾ_MAX 108
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ sockaddr_un {
    ਪਤਾ_FAMILY sun_family;
    ਅੱਖਰ sun_ਰਸਤਾ[UNIX_ਰਸਤਾ_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ਨਹੀਂਤਾਂ
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) ਲਿਖ(sd, buf, len)
# define sock_read(sd, buf, len) ਪਠ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) ਪਠ(fd, buf, len)
# define fd_write(sd, buf, len) ਲਿਖ(sd, buf, len)
# define fd_close(sd) close(sd)
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ channel_read(channel_T *channel, ch_part_T part, ਅੱਖਰ *func);
ਸਥਿਰ ਖਾਲੀ channel_wait_any(ਲੰਮਾ msec);
ਸਥਿਰ ch_ਬਹੁਲਕ_T channel_get_ਬਹੁਲਕ(channel_T *channel, ch_part_T part);
ਸਥਿਰ ਪੂਰਨ channel_get_timeout(channel_T *channel, ch_part_T part);
ਸਥਿਰ channel_T *channel_open_stdio(ਖਾਲੀ);
ਸਥਿਰ ch_part_T channel_part_send(channel_T *channel);
ਸਥਿਰ ch_part_T channel_part_read(channel_T *channel);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_CHANNELS(ch) \
    ਲਈ ((ch) = ਪਹਿਲਾ_channel; (ch) != ਨਿਰਾ; (ch) = (ch)->ch_ਅਗਲਾ)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ਸਥਿਰ ਪੂਰਨ ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਸਥਿਰ ਪੂਰਨ
fd_read(sock_T fd, ਅੱਖਰ *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ਜੇ (!ReadFile(h, buf, (DWORD)len, &nread, ਨਿਰਾ))
	ਵਾਪਸ -1;
    ਵਾਪਸ (ਪੂਰਨ)nread;
}

    ਸਥਿਰ ਪੂਰਨ
fd_write(sock_T fd, ਅੱਖਰ *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, ਮਾਪ, ਪੂਰਾ = 0;
    OVERLAPPED	ov;

    ਜਦੋਂ (todo > 0)
    {
	ਜੇ (todo > MAX_NAMED_PIPE_ਮਾਪ)
	    ਮਾਪ = MAX_NAMED_PIPE_ਮਾਪ;
	ਨਹੀਂਤਾਂ
	    ਮਾਪ = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_ਖੇਤਰ(ov);
	nwrite = 0;
	ਜੇ (!WriteFile(h, buf + ਪੂਰਾ, ਮਾਪ, &nwrite, &ov))
	{
	    DWORD err = Get_ਆਖਰੀ_Error();

	    ਜੇ (err != ERROR_IO_PENDING)
		ਵਾਪਸ -1;
	    ਜੇ (!Get_Overlapped_ਨਤੀਜਾ(h, &ov, &nwrite, FALSE))
		ਵਾਪਸ -1;
	    FlushFileBuffers(h);
	}
	ਨਹੀਂਤਾਂ ਜੇ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    ਰੋਕੋ;
	todo -= nwrite;
	ਪੂਰਾ += nwrite;
    }
    ਵਾਪਸ (ਪੂਰਨ)ਪੂਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
# undef PERROR
# define PERROR(ਸੁਨੇਹਾ) (ਖਾਲੀ)semsg("%s: %s", ਸੁਨੇਹਾ, strerror_win32(errno))

    ਸਥਿਰ ਅੱਖਰ *
strerror_win32(ਪੂਰਨ eno)
{
    ਸਥਿਰ LPVOID msgbuf = ਨਿਰਾ;
    char_u *ptr;

    ਜੇ (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ਨਿਰਾ;
    }
    Format_ਸੁਨੇਹਾ(
	FORMAT_ਸੁਨੇਹਾ_ALLOCATE_ਬਫਰ |
	FORMAT_ਸੁਨੇਹਾ_FROM_SYSTEM |
	FORMAT_ਸੁਨੇਹਾ_IGNORE_INSERTS,
	ਨਿਰਾ,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ਨਿਰਾ);
    ਜੇ (msgbuf != ਨਿਰਾ)
	// chomp \r or \n
	ਲਈ (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    ਚੋਣ (*ptr)
	    {
		ਹਾਲਤ '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    ਰੋਕੋ;
		ਹਾਲਤ '\n':
		    ਜੇ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    ਨਹੀਂਤਾਂ
			*ptr = ' ';
		    ਰੋਕੋ;
	    }
    ਵਾਪਸ msgbuf;
}
#ਜੇ_ਅੰਤ

/*
 * The list of all allocated channels.
 */
ਸਥਿਰ channel_T *ਪਹਿਲਾ_channel = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ ਅਗਲਾ_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
ਜੋੜੋ_channel(ਖਾਲੀ)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    channel->ch_id = ਅਗਲਾ_ch_id++;
    ch_log(channel, "Created channel");

    ਲਈ (part = PART_SOCK; part < PART_ਗਿਣਤੀ; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_X11
	channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler = (Xt_ਨਿਵੇਸ਼_Id)ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler = 0;
#ਜੇ_ਅੰਤ
	channel->ch_part[part].ch_timeout = 2000;
    }

    ਜੇ (ਪਹਿਲਾ_channel != ਨਿਰਾ)
    {
	ਪਹਿਲਾ_channel->ch_prev = channel;
	channel->ch_ਅਗਲਾ = ਪਹਿਲਾ_channel;
    }
    ਪਹਿਲਾ_channel = channel;

    channel->ch_refcount = 1;
    ਵਾਪਸ channel;
}

    ਪੂਰਨ
has_any_channel(ਖਾਲੀ)
{
    ਵਾਪਸ ਪਹਿਲਾ_channel != ਨਿਰਾ;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    ਪੂਰਨ
channel_still_useful(channel_T *channel)
{
    ਪੂਰਨ has_sock_ਸੁਨੇਹਾ;
    ਪੂਰਨ	has_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ;
    ਪੂਰਨ	has_err_ਸੁਨੇਹਾ;

    // If the job was killed the channel is not expected to work anymore.
    ਜੇ (channel->ch_ਕੰਮ_killed && channel->ch_ਕੰਮ == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    // If there is a close callback it may still need to be invoked.
    ਜੇ (channel->ch_close_cb.cb_name != ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    // If reading from or a buffer it's still useful.
    ਜੇ (channel->ch_part[PART_ਵਿੱਚ].ch_bufref.br_buf != ਨਿਰਾ)
	ਵਾਪਸ TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_ਸੁਨੇਹਾ = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_ਸਿਰ.rq_ਅਗਲਾ != ਨਿਰਾ
		|| channel->ch_part[PART_SOCK].ch_json_ਸਿਰ.jq_ਅਗਲਾ != ਨਿਰਾ;
    has_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ = channel->ch_part[PART_ਬਾਹਰਲਾ].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ਬਾਹਰਲਾ].ch_ਸਿਰ.rq_ਅਗਲਾ != ਨਿਰਾ
		  || channel->ch_part[PART_ਬਾਹਰਲਾ].ch_json_ਸਿਰ.jq_ਅਗਲਾ != ਨਿਰਾ;
    has_err_ਸੁਨੇਹਾ = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_ਸਿਰ.rq_ਅਗਲਾ != ਨਿਰਾ
		  || channel->ch_part[PART_ERR].ch_json_ਸਿਰ.jq_ਅਗਲਾ != ਨਿਰਾ;
    ਵਾਪਸ (channel->ch_callback.cb_name != ਨਿਰਾ && (has_sock_ਸੁਨੇਹਾ
		|| has_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ || has_err_ਸੁਨੇਹਾ))
	    || ((channel->ch_part[PART_ਬਾਹਰਲਾ].ch_callback.cb_name != ਨਿਰਾ
		       || channel->ch_part[PART_ਬਾਹਰਲਾ].ch_bufref.br_buf != ਨਿਰਾ)
		    && has_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ਨਿਰਾ
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ਨਿਰਾ)
		    && has_err_ਸੁਨੇਹਾ);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    ਪੂਰਨ
channel_can_close(channel_T *channel)
{
    ਵਾਪਸ channel->ch_ਮੰਜ਼ਿਲ_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_free_channel(channel_T *channel)
{
    ਜੇ (channel->ch_ਅਗਲਾ != ਨਿਰਾ)
	channel->ch_ਅਗਲਾ->ch_prev = channel->ch_prev;
    ਜੇ (channel->ch_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_channel = channel->ch_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	channel->ch_prev->ch_ਅਗਲਾ = channel->ch_ਅਗਲਾ;
    vim_free(channel);
}

    ਸਥਿਰ ਖਾਲੀ
channel_free(channel_T *channel)
{
    ਜੇ (ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ)
	ਵਾਪਸ;

    ਜੇ (ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback == 0)
	channel->ch_ਮੰਜ਼ਿਲ_be_freed = TRUE;
    ਨਹੀਂਤਾਂ
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
    ਸਥਿਰ ਪੂਰਨ
channel_may_free(channel_T *channel)
{
    ਜੇ (!channel_still_useful(channel))
    {
	channel_free(channel);
	ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    ਪੂਰਨ
channel_unref(channel_T *channel)
{
    ਜੇ (channel != ਨਿਰਾ && --channel->ch_refcount <= 0)
	ਵਾਪਸ channel_may_free(channel);
    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
free_ਅਣਵਰਤਿਆ_channels_contents(ਪੂਰਨ copyID, ਪੂਰਨ ਨਕਾਬ)
{
    ਪੂਰਨ		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ਜੇ (!channel_still_useful(ch)
				 && (ch->ch_copyID & ਨਕਾਬ) != (copyID & ਨਕਾਬ))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback;
    ਵਾਪਸ did_free;
}

    ਖਾਲੀ
free_ਅਣਵਰਤਿਆ_channels(ਪੂਰਨ copyID, ਪੂਰਨ ਨਕਾਬ)
{
    channel_T	*ch;
    channel_T	*ch_ਅਗਲਾ;

    ਲਈ (ch = ਪਹਿਲਾ_channel; ch != ਨਿਰਾ; ch = ch_ਅਗਲਾ)
    {
	ch_ਅਗਲਾ = ch->ch_ਅਗਲਾ;
	ਜੇ (!channel_still_useful(ch)
				 && (ch->ch_copyID & ਨਕਾਬ) != (copyID & ਨਕਾਬ))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ਜੇ defined(FEAT_GUI)

# ਜੇ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    ਸਥਿਰ channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ਜੇ (fd == INVALID_FD)
	ਵਾਪਸ ਨਿਰਾ;

    FOR_ALL_CHANNELS(channel)
    {
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	    ਜੇ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ਵਾਪਸ channel;
	    }
    }
    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ
channel_read_fd(ਪੂਰਨ fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ਜੇ (channel == ਨਿਰਾ)
	ch_error(ਨਿਰਾ, "Channel for fd %d not found", fd);
    ਨਹੀਂਤਾਂ
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_From_Server_X11(Xt_ਸੂਚਕ client_ਅੰਕੜੇ,
		  ਪੂਰਨ *unused1 ਅਣਵਰਤਿਆ,
		  Xt_ਨਿਵੇਸ਼_Id *unused2 ਅਣਵਰਤਿਆ)
{
    channel_read_fd((ਪੂਰਨ)(ਲੰਮਾ)client_ਅੰਕੜੇ);
}
# endif

# ifdef FEAT_GUI_GTK
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ gboolean
ਸੁਨੇਹਾ_From_Server_Gtk3(GIOChannel *unused1 ਅਣਵਰਤਿਆ,
		  GIOCondition unused2 ਅਣਵਰਤਿਆ,
		  gpointer client_ਅੰਕੜੇ)
{
    channel_read_fd(GPOINTER_ਮੰਜ਼ਿਲ_INT(client_ਅੰਕੜੇ));
    ਵਾਪਸ TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਖਾਲੀ
ਸੁਨੇਹਾ_From_Server_Gtk2(gpointer client_ਅੰਕੜੇ,
		  gint unused1 ਅਣਵਰਤਿਆ,
		  Gdk_ਨਿਵੇਸ਼_Condition unused2 ਅਣਵਰਤਿਆ)
{
    channel_read_fd((ਪੂਰਨ)(ਲੰਮਾ)client_ਅੰਕੜੇ);
}
#  endif
# endif

    ਸਥਿਰ ਖਾਲੀ
channel_gui_ਰਜਿਸਟਰ_one(channel_T *channel, ch_part_T part ਅਣਵਰਤਿਆ)
{
    ਜੇ (!CH_HAS_GUI)
	ਵਾਪਸ;

    // gets stuck in handling events for a not connected channel
    ਜੇ (channel->ch_keep_open)
	ਵਾਪਸ;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ਜੇ (channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler == (Xt_ਨਿਵੇਸ਼_Id)ਨਿਰਾ)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_ਨਾਂ[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler = Xt_App_ਜੋੜੋ_ਨਿਵੇਸ਼(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(Xt_ਸੂਚਕ)(Xt_ਨਿਵੇਸ਼_Read_ਨਕਾਬ + Xt_ਨਿਵੇਸ਼_Except_ਨਕਾਬ),
		ਸੁਨੇਹਾ_From_Server_X11,
		(Xt_ਸੂਚਕ)(ਲੰਮਾ)channel->ch_part[part].ch_fd);
    }
# ਨਹੀਂਤਾਂ
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ਜੇ (channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_ਨਾਂ[part], channel->ch_part[part].ch_fd);
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler = g_io_ਜੋੜੋ_watch(
		chnnl,
		G_IO_ਵਿੱਚ|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		ਸੁਨੇਹਾ_From_Server_Gtk3,
		GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   ਨਹੀਂਤਾਂ
	channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler = gdk_ਨਿਵੇਸ਼_ਜੋੜੋ(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_ਨਿਵੇਸ਼_Condition)
			     ((ਪੂਰਨ)GDK_ਨਿਵੇਸ਼_READ + (ਪੂਰਨ)GDK_ਨਿਵੇਸ਼_EXCEPTION),
		ਸੁਨੇਹਾ_From_Server_Gtk2,
		(gpointer)(ਲੰਮਾ)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ਸਥਿਰ ਖਾਲੀ
channel_gui_ਰਜਿਸਟਰ(channel_T *channel)
{
    ਜੇ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_ਰਜਿਸਟਰ_one(channel, PART_SOCK);
    ਜੇ (channel->CH_ਬਾਹਰਲਾ_FD != INVALID_FD
	    && channel->CH_ਬਾਹਰਲਾ_FD != channel->CH_SOCK_FD)
	channel_gui_ਰਜਿਸਟਰ_one(channel, PART_ਬਾਹਰਲਾ);
    ਜੇ (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_ਬਾਹਰਲਾ_FD)
	channel_gui_ਰਜਿਸਟਰ_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    ਖਾਲੀ
channel_gui_ਰਜਿਸਟਰ_all(ਖਾਲੀ)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_ਰਜਿਸਟਰ(channel);
}

    ਸਥਿਰ ਖਾਲੀ
channel_gui_unregister_one(channel_T *channel ਅਣਵਰਤਿਆ, ch_part_T part ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_GUI_X11
    ਜੇ (channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler != (Xt_ਨਿਵੇਸ਼_Id)ਨਿਰਾ)
    {
	ch_log(channel, "Unregistering part %s", ch_part_ਨਾਂ[part]);
	Xt_Remove_ਨਿਵੇਸ਼(channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler);
	channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler = (Xt_ਨਿਵੇਸ਼_Id)ਨਿਰਾ;
    }
# ਨਹੀਂਤਾਂ
#  ifdef FEAT_GUI_GTK
    ਜੇ (channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_ਨਾਂ[part]);
#   ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	g_ਸਰੋਤ_remove(channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler);
#   ਨਹੀਂਤਾਂ
	gdk_ਨਿਵੇਸ਼_remove(channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler);
#   endif
	channel->ch_part[part].ch_ਨਿਵੇਸ਼_Handler = 0;
    }
#  endif
# endif
}

    ਸਥਿਰ ਖਾਲੀ
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	channel_gui_unregister_one(channel, part);
}

#ਜੇ_ਅੰਤ  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_connect(
	channel_T *channel,
	ਅਚਲ ਬਣਤਰ sockaddr *server_addr,
	ਪੂਰਨ server_addrlen,
	ਪੂਰਨ *waittime)
{
    ਪੂਰਨ		sd = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    u_long	val = 1;
#ਜੇ_ਅੰਤ

    ਜਦੋਂ (TRUE)
    {
	ਲੰਮਾ	elapsed_msec = 0;
	ਪੂਰਨ	waitnow;
	ਪੂਰਨ	ret;

	ਜੇ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ਜੇ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_ਵਿੱਚ_channel_connect));
	    ਵਾਪਸ -1;
	}

	ਜੇ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#ਨਹੀਂਤਾਂ
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#ਜੇ_ਅੰਤ
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		ਵਾਪਸ -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ਜੇ (ret == 0)
	    // The connection could be established.
	    ਰੋਕੋ;

	SOCK_ERRNO;
	ਜੇ (*waittime < 0 || (errno != EWOULDBLOCK
		&& errno != ECONNREFUSED
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EINPROGRESS
		&& errno != EINPROGRESS
#ਜੇ_ਅੰਤ
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
	    PERROR(_(e_cannot_connect_ਮੰਜ਼ਿਲ_ਪੋਰਟ));
	    sock_close(sd);
	    ਵਾਪਸ -1;
	}
	ਨਹੀਂਤਾਂ ਜੇ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ਵਾਪਸ -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    ਬਣਤਰ timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਪੂਰਨ			so_error = 0;
	    socklen_t		so_error_len = ਆਕਾਰ(so_error);
	    ਬਣਤਰ timeval	ਸ਼ੁਰੂ_tv;
	    ਬਣਤਰ timeval	ਸਿਰਾ_tv;
#ਜੇ_ਅੰਤ
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
	    gettimeofday(&ਸ਼ੁਰੂ_tv, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ਨਿਰਾ, &tv);
	    ਜੇ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_ਮੰਜ਼ਿਲ_ਪੋਰਟ));
		sock_close(sd);
		ਵਾਪਸ -1;
	    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ਜੇ (FD_ISSET(sd, &wfds))
		ਰੋਕੋ;
	    elapsed_msec = waitnow;
	    ਜੇ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		ਜਾਰੀ;
	    }
#ਨਹੀਂਤਾਂ
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ਜੇ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ਜੇ (ret < 0 || (so_error != 0
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
		    PERROR(_(e_cannot_connect_ਮੰਜ਼ਿਲ_ਪੋਰਟ));
		    sock_close(sd);
		    ਵਾਪਸ -1;
		}
		ਨਹੀਂਤਾਂ ਜੇ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ਵਾਪਸ -1;
		}
	    }

	    ਜੇ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		ਰੋਕੋ;

	    gettimeofday(&ਸਿਰਾ_tv, ਨਿਰਾ);
	    elapsed_msec = (ਸਿਰਾ_tv.tv_sec - ਸ਼ੁਰੂ_tv.tv_sec) * 1000
				 + (ਸਿਰਾ_tv.tv_usec - ਸ਼ੁਰੂ_tv.tv_usec) / 1000;
#ਜੇ_ਅੰਤ
	}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਜੇ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ਜੇ (waitnow > 0)
	    {
		mch_delay((ਲੰਮਾ)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ਜੇ (!got_int)
	    {
		ਜੇ (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		ਜਾਰੀ;
	    }
	    // we were interrupted, behave as if timed out
	}
#ਜੇ_ਅੰਤ

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	ਵਾਪਸ -1;
    }

    ਜੇ (*waittime >= 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)fcntl(sd, F_SETFL, 0);
#ਜੇ_ਅੰਤ
    }

    ਵਾਪਸ sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
	ਖਾਲੀ (*nb_close_cb)(ਖਾਲੀ))
{
    channel_T		*channel = ਨਿਰਾ;
    ਪੂਰਨ			sd = -1;
    size_t		ਰਸਤਾ_len = STRLEN(ਰਸਤਾ);
    ਬਣਤਰ sockaddr_un	server;
    size_t		server_len;
    ਪੂਰਨ			waittime = -1;

    ਜੇ (*ਰਸਤਾ == NUL || ਰਸਤਾ_len >= ਆਕਾਰ(server.sun_ਰਸਤਾ))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), ਰਸਤਾ);
	ਵਾਪਸ ਨਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    channel_init_winsock();
#ਜੇ_ਅੰਤ

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
    {
	ch_error(ਨਿਰਾ, "Cannot allocate channel.");
	ਵਾਪਸ ਨਿਰਾ;
    }

    CLEAR_ਖੇਤਰ(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_ਰਸਤਾ, ਰਸਤਾ, ਆਕਾਰ(server.sun_ਰਸਤਾ) - 1);

    ch_log(channel, "Trying to connect to %s", ਰਸਤਾ);

    server_len = offsetof(ਬਣਤਰ sockaddr_un, sun_ਰਸਤਾ) + ਰਸਤਾ_len + 1;
    sd = channel_connect(channel, (ਬਣਤਰ sockaddr *)&server, (ਪੂਰਨ)server_len,
								   &waittime);

    ਜੇ (sd < 0)
    {
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ਅੱਖਰ *)vim_strsave((char_u *)ਰਸਤਾ);
    channel->ch_ਪੋਰਟ = 0;
    channel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_SOCK);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    channel_gui_ਰਜਿਸਟਰ_one(channel, PART_SOCK);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ channel;
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
	ਅਚਲ ਅੱਖਰ *hostname,
	ਪੂਰਨ ਪੋਰਟ,
	ਪੂਰਨ waittime,
	ਖਾਲੀ (*nb_close_cb)(ਖਾਲੀ))
{
    ਪੂਰਨ			sd = -1;
    channel_T		*channel = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IPV6
    ਪੂਰਨ			err;
    ਬਣਤਰ addrinfo	hints;
    ਬਣਤਰ addrinfo	*res = ਨਿਰਾ;
    ਬਣਤਰ addrinfo	*addr = ਨਿਰਾ;
#ਨਹੀਂਤਾਂ
    ਬਣਤਰ sockaddr_ਵਿੱਚ	server;
    ਬਣਤਰ hostent	*host = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    channel_init_winsock();
#ਜੇ_ਅੰਤ

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
    {
	ch_error(ਨਿਰਾ, "Cannot allocate channel.");
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IPV6
    CLEAR_ਖੇਤਰ(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ਜੇ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ਜੇ ((err = getaddrinfo(hostname, ਨਿਰਾ, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_ਵਿੱਚ_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Count the number of addresses for timeout distribution
    ਪੂਰਨ addr_ਗਿਣਤੀ = 0;
    ਲਈ (addr = res; addr != ਨਿਰਾ; addr = addr->ai_ਅਗਲਾ)
	addr_ਗਿਣਤੀ++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ਜੇ (waittime == 0)
	waittime = 15;

    ਪੂਰਨ addr_ਸੂਚਕਾਂਕ = 0;
    ਲਈ (addr = res; addr != ਨਿਰਾ; addr = addr->ai_ਅਗਲਾ, addr_ਸੂਚਕਾਂਕ++)
    {
	ਅਚਲ ਅੱਖਰ  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ਅਚਲ ਖਾਲੀ  *src = ਨਿਰਾ;
	ਅੱਖਰ	    buf[NUMBUFLEN];
# endif
	ਪੂਰਨ	    try_waittime;
	ਪੂਰਨ	    before_waittime;
	ਪੂਰਨ	    consumed;
	ਪੂਰਨ	    ਬਚਿਆ_addrs;

	ਜੇ (addr->ai_family == AF_INET6)
	{
	    ਬਣਤਰ sockaddr_in6 *sai = (ਬਣਤਰ sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_ਪੋਰਟ = htons(ਪੋਰਟ);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	ਨਹੀਂਤਾਂ ਜੇ (addr->ai_family == AF_INET)
	{
	    ਬਣਤਰ sockaddr_ਵਿੱਚ *sai = (ਬਣਤਰ sockaddr_ਵਿੱਚ *)addr->ai_addr;

	    sai->sin_ਪੋਰਟ = htons(ਪੋਰਟ);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ਜੇ (src != ਨਿਰਾ)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, ਆਕਾਰ(buf));
	    ਜੇ (dst == ਨਿਰਾ)
		dst = hostname;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, ਪੋਰਟ);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ਜੇ (addr->ai_ਅਗਲਾ == ਨਿਰਾ)
	    try_waittime = waittime;
	ਨਹੀਂਤਾਂ ਜੇ (addr_ਸੂਚਕਾਂਕ == 0)
	{
	    ਜੇ (waittime > 500)
		try_waittime = 250;
	    ਨਹੀਂਤਾਂ ਜੇ (waittime > 30)
		try_waittime = waittime / 2;
	    ਨਹੀਂਤਾਂ
		try_waittime = waittime;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਬਚਿਆ_addrs = addr_ਗਿਣਤੀ - addr_ਸੂਚਕਾਂਕ;
	    try_waittime = waittime / ਬਚਿਆ_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (ਪੂਰਨ)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ਜੇ (waittime < 0)
	    waittime = 0;

	ਜੇ (sd >= 0)
	    ਰੋਕੋ;

	// If we have no time left, stop trying
	ਜੇ (waittime <= 0 && addr->ai_ਅਗਲਾ != ਨਿਰਾ)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    ਰੋਕੋ;
	}
    }

    freeaddrinfo(res);
#ਨਹੀਂਤਾਂ
    CLEAR_ਖੇਤਰ(server);
    server.sin_family = AF_INET;
    server.sin_ਪੋਰਟ = htons(ਪੋਰਟ);
    ਜੇ ((host = gethostbyname(hostname)) == ਨਿਰਾ)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_ਵਿੱਚ_channel_open));
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }
    {
	ਅੱਖਰ *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(&p, &host->h_addr_list[0], ਆਕਾਰ(p));
	ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)&server.sin_addr, p, host->h_ਲੰਮਾਈ);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, ਪੋਰਟ);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    ਜੇ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (ਬਣਤਰ sockaddr *)&server, ਆਕਾਰ(server),
								   &waittime);
#ਜੇ_ਅੰਤ

    ਜੇ (sd < 0)
    {
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ਅੱਖਰ *)vim_strsave((char_u *)hostname);
    channel->ch_ਪੋਰਟ = ਪੋਰਟ;
    channel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_SOCK);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    channel_gui_ਰਜਿਸਟਰ_one(channel, PART_SOCK);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ channel;
}

    ਸਥਿਰ ਖਾਲੀ
free_ਸੈੱਟ_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ਜੇ (callback->cb_name != ਨਿਰਾ && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    ਨਹੀਂਤਾਂ
	cbp->cb_name = ਨਿਰਾ;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ਸਥਿਰ ਖਾਲੀ
prepare_ਬਫਰ(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_QUICKFIX
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#ਜੇ_ਅੰਤ
    ਜੇ (curbuf->b_ml.ml_mfp == ਨਿਰਾ)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ਸਥਿਰ buf_T *
channel_ਲੱਭੋ_ਬਫਰ(char_u *name, ਪੂਰਨ err, ਪੂਰਨ ਸੁਨੇਹਾ)
{
    buf_T *buf = ਨਿਰਾ;
    buf_T *save_curbuf = curbuf;

    ਜੇ (name != ਨਿਰਾ && *name != NUL)
    {
	buf = buflist_findname(name);
	ਜੇ (buf == ਨਿਰਾ)
	    buf = buflist_findname_exp(name);
    }

    ਜੇ (buf != ਨਿਰਾ)
	ਵਾਪਸ buf;

    buf = buflist_new(name == ਨਿਰਾ || *name == NUL ? ਨਿਰਾ : name,
	    ਨਿਰਾ, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    prepare_ਬਫਰ(buf);

    curbuf = buf;
    ਜੇ (ਸੁਨੇਹਾ)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    ਬਦਲਿਆ_ਬਾਈਟਾਂ(1, 0);
    curbuf = save_curbuf;

    ਵਾਪਸ buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_ਸੈੱਟ_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਬਹੁਲਕ)
	ਲਈ (part = PART_SOCK; part < PART_ਗਿਣਤੀ; ++part)
	    channel->ch_part[part].ch_ਬਹੁਲਕ = opt->jo_ਬਹੁਲਕ;
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਵਿੱਚ_ਬਹੁਲਕ)
	channel->ch_part[PART_ਵਿੱਚ].ch_ਬਹੁਲਕ = opt->jo_ਵਿੱਚ_ਬਹੁਲਕ;
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ)
	channel->ch_part[PART_ਬਾਹਰਲਾ].ch_ਬਹੁਲਕ = opt->jo_ਬਾਹਰਲਾ_ਬਹੁਲਕ;
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ERR_ਬਹੁਲਕ)
	channel->ch_part[PART_ERR].ch_ਬਹੁਲਕ = opt->jo_err_ਬਹੁਲਕ;
    channel->ch_nonblock = opt->jo_noblock;

    ਜੇ (opt->jo_ਸੈੱਟ & JO_TIMEOUT)
	ਲਈ (part = PART_SOCK; part < PART_ਗਿਣਤੀ; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_TIMEOUT)
	channel->ch_part[PART_ਬਾਹਰਲਾ].ch_timeout = opt->jo_ਬਾਹਰਲਾ_timeout;
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਖੰਡ_WRITE)
	channel->ch_part[PART_ਵਿੱਚ].ch_ਖੰਡ_write = 1;

    ਜੇ (opt->jo_ਸੈੱਟ & JO_CALLBACK)
	free_ਸੈੱਟ_callback(&channel->ch_callback, &opt->jo_callback);
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_CALLBACK)
	free_ਸੈੱਟ_callback(&channel->ch_part[PART_ਬਾਹਰਲਾ].ch_callback,
							      &opt->jo_ਬਾਹਰਲਾ_cb);
    ਜੇ (opt->jo_ਸੈੱਟ & JO_ERR_CALLBACK)
	free_ਸੈੱਟ_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ਜੇ (opt->jo_ਸੈੱਟ & JO_CLOSE_CALLBACK)
	free_ਸੈੱਟ_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ਜੇ ((opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_IO) && opt->jo_io[PART_ਬਾਹਰਲਾ] == JIO_ਬਫਰ)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ਜੇ (!(opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ))
	    channel->ch_part[PART_ਬਾਹਰਲਾ].ch_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_NL;
	ਜੇ (opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ਬਾਹਰਲਾ]);
	    ਜੇ (buf == ਨਿਰਾ)
		semsg(_(e_ਬਫਰ_nr_does_not_exist),
					       (ਲੰਮਾ)opt->jo_io_buf[PART_ਬਾਹਰਲਾ]);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ ਸੁਨੇਹਾ = TRUE;

	    ਜੇ (opt->jo_set2 & JO2_ਬਾਹਰਲਾ_ਸੁਨੇਹਾ)
		ਸੁਨੇਹਾ = opt->jo_ਸੁਨੇਹਾ[PART_ਬਾਹਰਲਾ];
	    buf = channel_ਲੱਭੋ_ਬਫਰ(opt->jo_io_name[PART_ਬਾਹਰਲਾ], FALSE, ਸੁਨੇਹਾ);
	}
	ਜੇ (buf != ਨਿਰਾ)
	{
	    ਜੇ (opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_MODIFIABLE)
		channel->ch_part[PART_ਬਾਹਰਲਾ].ch_nomodifiable =
						!opt->jo_modifiable[PART_ਬਾਹਰਲਾ];

	    ਜੇ (!buf->b_p_ma && !channel->ch_part[PART_ਬਾਹਰਲਾ].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (ਅੱਖਰ *)buf->b_ffname);
		ਸੈੱਟ_bufref(&channel->ch_part[PART_ਬਾਹਰਲਾ].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
		    prepare_ਬਫਰ(buf);
	    }
	}
    }

    ਜੇ ((opt->jo_ਸੈੱਟ & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_ਬਫਰ
	 || (opt->jo_io[PART_ERR] == JIO_ਬਾਹਰਲਾ && (opt->jo_ਸੈੱਟ & JO_ਬਾਹਰਲਾ_IO)
				       && opt->jo_io[PART_ਬਾਹਰਲਾ] == JIO_ਬਫਰ)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ਜੇ (!(opt->jo_ਸੈੱਟ & JO_ERR_ਬਹੁਲਕ))
	    channel->ch_part[PART_ERR].ch_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_NL;
	ਜੇ (opt->jo_io[PART_ERR] == JIO_ਬਾਹਰਲਾ)
	    buf = channel->ch_part[PART_ਬਾਹਰਲਾ].ch_bufref.br_buf;
	ਨਹੀਂਤਾਂ ਜੇ (opt->jo_ਸੈੱਟ & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ਜੇ (buf == ਨਿਰਾ)
		semsg(_(e_ਬਫਰ_nr_does_not_exist),
					       (ਲੰਮਾ)opt->jo_io_buf[PART_ERR]);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ ਸੁਨੇਹਾ = TRUE;

	    ਜੇ (opt->jo_set2 & JO2_ERR_ਸੁਨੇਹਾ)
		ਸੁਨੇਹਾ = opt->jo_ਸੁਨੇਹਾ[PART_ERR];
	    buf = channel_ਲੱਭੋ_ਬਫਰ(opt->jo_io_name[PART_ERR], TRUE, ਸੁਨੇਹਾ);
	}
	ਜੇ (buf != ਨਿਰਾ)
	{
	    ਜੇ (opt->jo_ਸੈੱਟ & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ਜੇ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (ਅੱਖਰ *)buf->b_ffname);
		ਸੈੱਟ_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ਜੇ (buf->b_ml.ml_mfp == ਨਿਰਾ)
		    prepare_ਬਫਰ(buf);
	    }
	}
    }

    channel->ch_part[PART_ਬਾਹਰਲਾ].ch_io = opt->jo_io[PART_ਬਾਹਰਲਾ];
    channel->ch_part[PART_ERR].ch_io = opt->jo_io[PART_ERR];
    channel->ch_part[PART_ਵਿੱਚ].ch_io = opt->jo_io[PART_ਵਿੱਚ];
}

/*
 * Parse the address for socketserver and store port in "port", or the path in
 * "unix_path" if it is a unix domain socket. Returns OK on success and FAIL on
 * failure.
 */
    ਪੂਰਨ
channel_parse_socketserver_ਪਤਾ(
	char_u	*ਪਤਾ,
	ਪੂਰਨ	*ਪੋਰਟ,
	char_u	**unix_ਰਸਤਾ,
	bool	quiet)
{
    ਅੱਖਰ    *rest;
    ਲੰਮਾ    val;

    ਜੇ (*ਪਤਾ == NUL)
	ਜਾਓ fail;

    ਜੇ (STRNCMP(ਪਤਾ, "unix:", 5) == 0)
    {
	*unix_ਰਸਤਾ = vim_strsave(ਪਤਾ + 5);
	ਵਾਪਸ OK;
    }

    val = strtol((ਅੱਖਰ *)(ਪਤਾ), &rest, 10);
    ਜੇ (val < 0 || val >= 65536 || *rest != NUL)
    {
	ਜੇ (!quiet)
	    semsg(_(e_invalid_ਦਲੀਲ_str), ਪਤਾ);
	ਵਾਪਸ FAIL;
    }
    *ਪੋਰਟ = (ਪੂਰਨ)val;

    ਵਾਪਸ OK;
fail:
    ਜੇ (!quiet)
	semsg(_(e_invalid_ਦਲੀਲ_str), ਪਤਾ);
    ਵਾਪਸ FAIL;
}

/*
 * Implements ch_open().
 */
    ਸਥਿਰ channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*ਪਤਾ;
    char_u	*p;
    ਅੱਖਰ	*rest;
    ਪੂਰਨ		ਪੋਰਟ = 0;
    ਪੂਰਨ		is_ipv6 = FALSE;
    ਪੂਰਨ		is_unix = FALSE;
    ਪੂਰਨ		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ ਨਿਰਾ;

    ਪਤਾ = tv_get_string(&argvars[0]);
    ਜੇ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (*ਪਤਾ == NUL)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), ਪਤਾ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (STRCMP(ਪਤਾ, "stdio") == 0)
	is_stdio = TRUE;
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(ਪਤਾ, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	ਪਤਾ += 5;
    }
    ਨਹੀਂਤਾਂ ਜੇ (*ਪਤਾ == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(ਪਤਾ + 1, ']');
	ਜੇ (p == ਨਿਰਾ || *++p != ':')
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), ਪਤਾ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// ipv4 address
	p = vim_strchr(ਪਤਾ, ':');
	ਜੇ (p == ਨਿਰਾ)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), ਪਤਾ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    ਜੇ (!is_unix && !is_stdio)
    {
	ਪੋਰਟ = strtol((ਅੱਖਰ *)(p + 1), &rest, 10);
	ਜੇ (ਪੋਰਟ <= 0 || ਪੋਰਟ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), ਪਤਾ);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++ਪਤਾ;
	    *(p - 1) = NUL;
	}
	ਨਹੀਂਤਾਂ
	    *p = NUL;
    }

    // parse options
    clear_ਕੰਮ_options(&opt);
    opt.jo_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_JSON;
    opt.jo_timeout = 2000;
    ਜੇ (get_ਕੰਮ_options(&argvars[1], &opt,
	    JO_ਬਹੁਲਕ_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	ਜਾਓ theend;
    ਜੇ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਜਾਓ theend;
    }

    ਜੇ (is_stdio)
	channel = channel_open_stdio();
    ਨਹੀਂਤਾਂ ਜੇ (is_unix)
	channel = channel_open_unix((ਅੱਖਰ *)ਪਤਾ, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	channel = channel_open((ਅੱਖਰ *)ਪਤਾ, ਪੋਰਟ, opt.jo_waittime, ਨਿਰਾ);
    ਜੇ (channel != ਨਿਰਾ)
    {
	ਪੂਰਨ	user_ਸੈੱਟ = opt.jo_ਸੈੱਟ;

	opt.jo_ਸੈੱਟ = JO_ALL;
	ਜੇ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_ਸੈੱਟ &= ~((JO_ਵਿੱਚ_ਬਹੁਲਕ | JO_ਬਾਹਰਲਾ_ਬਹੁਲਕ | JO_ERR_ਬਹੁਲਕ
				| JO_ਬਾਹਰਲਾ_TIMEOUT | JO_ERR_TIMEOUT) & ~user_ਸੈੱਟ);
	channel_ਸੈੱਟ_options(channel, &opt);
    }
theend:
    free_ਕੰਮ_options(&opt);
    ਵਾਪਸ channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
ਸਥਿਰ channel_T *stdio_channel = ਨਿਰਾ;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    ਸਥਿਰ channel_T *
channel_open_stdio(ਖਾਲੀ)
{
    channel_T	*channel;
    sock_T	ਵਿੱਚ_fd;		// our stdin, the channel reads from it
    sock_T	ਬਾਹਰਲਾ_fd;		// our stdout, the channel writes to it

    ਜੇ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (stdio_channel != ਨਿਰਾ)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	ਪੂਰਨ	nul_fd;

	ਜੇ (!DuplicateHandle(proc, GetStdHandle(STD_ਨਿਵੇਸ਼_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ਵਾਪਸ ਨਿਰਾ;
	}
	ਜੇ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ਵਾਪਸ ਨਿਰਾ;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ਨਿਰਾ, OPEN_EXISTING, 0,
		ਨਿਰਾ);
	ਜੇ (hnul != INVALID_HANDLE_ਮੁੱਲ)
	{
	    ਸੈੱਟ_Std_Handle(STD_ਨਿਵੇਸ਼_HANDLE, hnul);
	    ਸੈੱਟ_Std_Handle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ਜੇ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	ਵਿੱਚ_fd = (sock_T)hin;
	ਬਾਹਰਲਾ_fd = (sock_T)hout;
    }
#ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	nul_fd;

	ਵਿੱਚ_fd = dup(0);
	ਬਾਹਰਲਾ_fd = dup(1);
	ਜੇ (ਵਿੱਚ_fd < 0 || ਬਾਹਰਲਾ_fd < 0)
	{
	    ਜੇ (ਵਿੱਚ_fd >= 0)
		close(ਵਿੱਚ_fd);
	    ਜੇ (ਬਾਹਰਲਾ_fd >= 0)
		close(ਬਾਹਰਲਾ_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ਵਾਪਸ ਨਿਰਾ;
	}
	(ਖਾਲੀ)fcntl(ਵਿੱਚ_fd, F_SETFD, FD_CLOEXEC);
	(ਖਾਲੀ)fcntl(ਬਾਹਰਲਾ_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ਜੇ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#ਜੇ_ਅੰਤ

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
    {
	ch_error(ਨਿਰਾ, "Cannot allocate channel.");
	fd_close(ਵਿੱਚ_fd);
	fd_close(ਬਾਹਰਲਾ_fd);
	ਵਾਪਸ ਨਿਰਾ;
    }
    channel_ਸੈੱਟ_pipes(channel, ਬਾਹਰਲਾ_fd, ਵਿੱਚ_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ਵਾਪਸ channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    ਅੱਖਰ	*rest;
    ਪੂਰਨ		ਪੋਰਟ;
    ਪੂਰਨ		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    arg = tv_get_string(&argvars[0]);
    ਜੇ (*arg == NUL)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), arg);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	ਪੋਰਟ = 0;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੋਰਟ = strtol((ਅੱਖਰ *)(arg), &rest, 10);
	ਜੇ (ਪੋਰਟ < 0 || ਪੋਰਟ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_ਦਲੀਲ_str), arg);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	*arg = NUL;
    }

    // parse options
    clear_ਕੰਮ_options(&opt);
    opt.jo_ਬਹੁਲਕ = CH_ਬਹੁਲਕ_JSON;
    opt.jo_timeout = 2000;
    ਜੇ (get_ਕੰਮ_options(&argvars[1], &opt,
	    JO_ਬਹੁਲਕ_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	ਜਾਓ theend;
    ਜੇ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_ਦਲੀਲ));
	ਜਾਓ theend;
    }

    ਜੇ (is_unix)
	channel = channel_listen_unix((ਅੱਖਰ *)arg, ਨਿਰਾ, true);
    ਨਹੀਂਤਾਂ
	channel = channel_listen(ਪੋਰਟ, ਨਿਰਾ);
    ਜੇ (channel != ਨਿਰਾ)
    {
	opt.jo_ਸੈੱਟ = JO_ALL;
	channel_ਸੈੱਟ_options(channel, &opt);
    }
theend:
    free_ਕੰਮ_options(&opt);
    ਵਾਪਸ channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	ਪੂਰਨ ਪੋਰਟ_ਵਿੱਚ,
	ਖਾਲੀ (*nb_close_cb)(ਖਾਲੀ))
{
    ਪੂਰਨ			sd = -1;
    ਬਣਤਰ sockaddr_ਵਿੱਚ	server;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ FEAT_IPV6
    ਬਣਤਰ hostent	*host;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ			val = 1;
    channel_T		*channel;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    channel_init_winsock();
#ਜੇ_ਅੰਤ

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
    {
	ch_error(ਨਿਰਾ, "Cannot allocate channel.");
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((ਅੱਖਰ *)&server, 0, ਆਕਾਰ(server));
    server.sin_family = AF_INET;
    server.sin_ਪੋਰਟ = htons(ਪੋਰਟ_ਵਿੱਚ);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ਜੇ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ਪੋਰਟ));
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ਅਚਲ ਅੱਖਰ *)&val, ਆਕਾਰ(val)) < 0)
#ਨਹੀਂਤਾਂ
    ਜੇ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, ਆਕਾਰ(val)) < 0)
#ਜੇ_ਅੰਤ
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ਪੋਰਟ));
	sock_close(sd);
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // Bind the socket to the port
    ਜੇ (bind(sd, (ਬਣਤਰ sockaddr *)&server, ਆਕਾਰ(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ਪੋਰਟ));
	sock_close(sd);
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ਪੋਰਟ));
	sock_close(sd);
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ਜੇ (ਪੋਰਟ_ਵਿੱਚ == 0)
    {
	ਬਣਤਰ sockaddr_ਵਿੱਚ	addr;
	socklen_t		addr_len = ਆਕਾਰ(addr);

	ਜੇ (getsockname(sd, (ਬਣਤਰ sockaddr *)&addr, &addr_len) == 0)
	    ਪੋਰਟ_ਵਿੱਚ = ntohs(addr.sin_ਪੋਰਟ);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ਅੱਖਰ *)vim_strsave((char_u *)"");
    channel->ch_ਪੋਰਟ = ਪੋਰਟ_ਵਿੱਚ;
    channel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_SOCK);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    channel_gui_ਰਜਿਸਟਰ_one(channel, PART_SOCK);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	ਅੱਖਰ *ਰਸਤਾ,
	ਖਾਲੀ (*nb_close_cb)(ਖਾਲੀ),
	bool replace)
{
    ਪੂਰਨ			sd = -1;
    ਬਣਤਰ sockaddr_un	server;
    channel_T		*channel;
    size_t		ਰਸਤਾ_len;
    size_t		server_len;

    ਜੇ (ਰਸਤਾ == ਨਿਰਾ || *ਰਸਤਾ == NUL)
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), ਰਸਤਾ == ਨਿਰਾ ? (ਅੱਖਰ *)"" : ਰਸਤਾ);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਰਸਤਾ_len = STRLEN(ਰਸਤਾ);
    ਜੇ (ਰਸਤਾ_len >= ਆਕਾਰ(server.sun_ਰਸਤਾ))
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), ਰਸਤਾ);
	ਵਾਪਸ ਨਿਰਾ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    channel_init_winsock();
#ਜੇ_ਅੰਤ

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
    {
	ch_error(ਨਿਰਾ, "Cannot allocate channel.");
	ਵਾਪਸ ਨਿਰਾ;
    }

    CLEAR_ਖੇਤਰ(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_ਰਸਤਾ, ਰਸਤਾ, ਆਕਾਰ(server.sun_ਰਸਤਾ) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ਜੇ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_ਪੋਰਟ));
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_ਰਸਤਾ);

    // Bind the socket to the path
    server_len = offsetof(ਬਣਤਰ sockaddr_un, sun_ਰਸਤਾ) + ਰਸਤਾ_len + 1;
    ਜੇ (bind(sd, (ਬਣਤਰ sockaddr *)&server, (ਪੂਰਨ)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_ਪੋਰਟ));
	sock_close(sd);
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_ਪੋਰਟ));
	sock_close(sd);
	channel_free(channel);
	ਵਾਪਸ ਨਿਰਾ;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ਅੱਖਰ *)vim_strsave((char_u *)ਰਸਤਾ);
    channel->ch_ਪੋਰਟ = 0;
    channel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_SOCK);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    channel_gui_ਰਜਿਸਟਰ_one(channel, PART_SOCK);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ channel;
}

     ਖਾਲੀ
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ਜੇ (*fd == INVALID_FD)
	ਵਾਪਸ;

    ਜੇ (part == PART_SOCK)
	sock_close(*fd);
    ਨਹੀਂਤਾਂ
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ਜੇ ((part == PART_ਵਿੱਚ || channel->CH_ਵਿੱਚ_FD != *fd)
		&& (part == PART_ਬਾਹਰਲਾ || channel->CH_ਬਾਹਰਲਾ_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਜੇ (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#ਜੇ_ਅੰਤ
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_ਮੰਜ਼ਿਲ_be_closed &= ~(1U << part);
}

    ਖਾਲੀ
channel_ਸੈੱਟ_pipes(channel_T *channel, sock_T ਵਿੱਚ, sock_T ਬਾਹਰਲਾ, sock_T err)
{
    ਜੇ (ਵਿੱਚ != INVALID_FD)
    {
	ch_close_part(channel, PART_ਵਿੱਚ);
	channel->CH_ਵਿੱਚ_FD = ਵਿੱਚ;
#ਜੇ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ਜੇ (mch_isatty(ਵਿੱਚ))
	    channel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_ਵਿੱਚ);
#ਜੇ_ਅੰਤ
    }
    ਜੇ (ਬਾਹਰਲਾ != INVALID_FD)
    {
#ਜੇ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ਬਾਹਰਲਾ);
#ਜੇ_ਅੰਤ
	ch_close_part(channel, PART_ਬਾਹਰਲਾ);
	channel->CH_ਬਾਹਰਲਾ_FD = ਬਾਹਰਲਾ;
	channel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_ਬਾਹਰਲਾ);
#ਜੇ defined(FEAT_GUI)
	channel_gui_ਰਜਿਸਟਰ_one(channel, PART_ਬਾਹਰਲਾ);
#ਜੇ_ਅੰਤ
    }
    ਜੇ (err != INVALID_FD)
    {
#ਜੇ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#ਜੇ_ਅੰਤ
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_ERR);
#ਜੇ defined(FEAT_GUI)
	channel_gui_ਰਜਿਸਟਰ_one(channel, PART_ERR);
#ਜੇ_ਅੰਤ
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    ਖਾਲੀ
channel_ਸੈੱਟ_ਕੰਮ(channel_T *channel, ਕੰਮ_T *ਕੰਮ, jobopt_T *options)
{
    channel->ch_ਕੰਮ = ਕੰਮ;

    channel_ਸੈੱਟ_options(channel, options);

    ਜੇ (ਕੰਮ->jv_ਵਿੱਚ_buf == ਨਿਰਾ)
	ਵਾਪਸ;

    chanpart_T *ਵਿੱਚ_part = &channel->ch_part[PART_ਵਿੱਚ];

    ਸੈੱਟ_bufref(&ਵਿੱਚ_part->ch_bufref, ਕੰਮ->jv_ਵਿੱਚ_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (ਅੱਖਰ *)ਵਿੱਚ_part->ch_bufref.br_buf->b_ffname);
    ਜੇ (options->jo_ਸੈੱਟ & JO_ਵਿੱਚ_ਸਿਖਰ)
    {
	ਜੇ (options->jo_ਵਿੱਚ_ਸਿਖਰ == 0 && !(options->jo_ਸੈੱਟ & JO_ਵਿੱਚ_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    ਵਿੱਚ_part->ch_bufref.br_buf->b_write_ਮੰਜ਼ਿਲ_channel = true;
	    ਵਿੱਚ_part->ch_buf_append = TRUE;
	    ਵਿੱਚ_part->ch_buf_ਸਿਖਰ =
		ਵਿੱਚ_part->ch_bufref.br_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ + 1;
	}
	ਨਹੀਂਤਾਂ
	    ਵਿੱਚ_part->ch_buf_ਸਿਖਰ = options->jo_ਵਿੱਚ_ਸਿਖਰ;
    }
    ਨਹੀਂਤਾਂ
	ਵਿੱਚ_part->ch_buf_ਸਿਖਰ = 1;
    ਜੇ (options->jo_ਸੈੱਟ & JO_ਵਿੱਚ_BOT)
	ਵਿੱਚ_part->ch_buf_bot = options->jo_ਵਿੱਚ_bot;
    ਨਹੀਂਤਾਂ
	ਵਿੱਚ_part->ch_buf_bot = ਵਿੱਚ_part->ch_bufref.br_buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ਸਥਿਰ ਖਾਲੀ
channel_ਸੈੱਟ_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	ਪੂਰਨ	    id)
{
    cbq_T *ਸਿਰ = &channel->ch_part[part].ch_cb_ਸਿਰ;
    cbq_T *ਵਸਤੂ = ALLOC_ONE(cbq_T);

    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	ਵਾਪਸ;

    copy_callback(&ਵਸਤੂ->cq_callback, callback);
    ਵਸਤੂ->cq_seq_nr = id;
    ਵਸਤੂ->cq_prev = ਸਿਰ->cq_prev;
    ਸਿਰ->cq_prev = ਵਸਤੂ;
    ਵਸਤੂ->cq_ਅਗਲਾ = ਨਿਰਾ;
    ਜੇ (ਵਸਤੂ->cq_prev == ਨਿਰਾ)
	ਸਿਰ->cq_ਅਗਲਾ = ਵਸਤੂ;
    ਨਹੀਂਤਾਂ
	ਵਸਤੂ->cq_prev->cq_ਅਗਲਾ = ਵਸਤੂ;
}

    ਸਥਿਰ ਖਾਲੀ
write_buf_ਸਤਰ(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *ਸਤਰ = ml_get_buf(buf, lnum, FALSE);
    ਪੂਰਨ	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    ਪੂਰਨ	    i;

    // Need to make a copy to be able to append a NL.
    ਜੇ ((p = alloc(len + 2)) == ਨਿਰਾ)
	ਵਾਪਸ;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਅੱਖਰ *)p, (ਅੱਖਰ *)ਸਤਰ, len);

    ਜੇ (channel->ch_write_ਪਾਠ_ਬਹੁਲਕ)
	p[len] = CAR;
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (i = 0; i < len; ++i)
	    ਜੇ (p[i] == NL)
		p[i] = NUL;

	p[len] = NL;
    }
    p[len + 1] = NUL;
    channel_send(channel, PART_ਵਿੱਚ, p, len + 1, "write_buf_line");
    vim_free(p);
}

/*
 * Return TRUE if "channel" can be written to.
 * Returns FALSE if the input is closed or the write would block.
 */
    ਸਥਿਰ ਪੂਰਨ
can_write_buf_ਸਤਰ(channel_T *channel)
{
    chanpart_T *ਵਿੱਚ_part = &channel->ch_part[PART_ਵਿੱਚ];

    ਜੇ (ਵਿੱਚ_part->ch_fd == INVALID_FD)
	ਵਾਪਸ FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ਜੇ (ਵਿੱਚ_part->ch_ਖੰਡ_write == 1)
	ਵਿੱਚ_part->ch_ਖੰਡ_write = -1;
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_part->ch_ਖੰਡ_write == -1)
	ਵਿੱਚ_part->ch_ਖੰਡ_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ MSWIN
    {
# ਜੇ defined(HAVE_SELECT)
	ਬਣਤਰ timeval	tval;
	fd_set		wfds;
	ਪੂਰਨ		ret;

	FD_ZERO(&wfds);
	FD_SET((ਪੂਰਨ)ਵਿੱਚ_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	ਲਈ (;;)
	{
	    ret = select((ਪੂਰਨ)ਵਿੱਚ_part->ch_fd + 1, ਨਿਰਾ, &wfds, ਨਿਰਾ, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ਜੇ (ret == -1 && errno == EINTR)
		ਜਾਰੀ;
#  endif
	    ਜੇ (ret <= 0 || ਵਿੱਚ_part->ch_ਖੰਡ_write == 1)
	    {
		ਜੇ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		ਨਹੀਂਤਾਂ
		    ch_log(channel, "Input not ready for writing");
		ਵਾਪਸ FALSE;
	    }
	    ਰੋਕੋ;
	}
# ਨਹੀਂਤਾਂ
	ਬਣਤਰ pollfd	fds;

	fds.fd = ਵਿੱਚ_part->ch_fd;
	fds.events = POLLOUT;
	ਜੇ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ਵਾਪਸ FALSE;
	}
	ਜੇ (ਵਿੱਚ_part->ch_ਖੰਡ_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ਵਾਪਸ FALSE;
	}
# endif
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    ਖਾਲੀ
channel_write_ਵਿੱਚ(channel_T *channel)
{
    chanpart_T *ਵਿੱਚ_part = &channel->ch_part[PART_ਵਿੱਚ];
    linenr_T    lnum;
    buf_T	*buf = ਵਿੱਚ_part->ch_bufref.br_buf;
    ਪੂਰਨ		written = 0;

    ਜੇ (buf == ਨਿਰਾ || ਵਿੱਚ_part->ch_buf_append)
	ਵਾਪਸ;  // no buffer or using appending
    ਜੇ (!bufref_valid(&ਵਿੱਚ_part->ch_bufref) || buf->b_ml.ml_mfp == ਨਿਰਾ)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	ਵਿੱਚ_part->ch_bufref.br_buf = ਨਿਰਾ;
	ਵਾਪਸ;
    }

    ਲਈ (lnum = ਵਿੱਚ_part->ch_buf_ਸਿਖਰ; lnum <= ਵਿੱਚ_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ; ++lnum)
    {
	ਜੇ (!can_write_buf_ਸਤਰ(channel))
	    ਰੋਕੋ;
	write_buf_ਸਤਰ(buf, lnum, channel);
	++written;
    }

    ਜੇ (written == 1)
	ch_log(channel, "written line %d to channel", (ਪੂਰਨ)lnum - 1);
    ਨਹੀਂਤਾਂ ਜੇ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    ਵਿੱਚ_part->ch_buf_ਸਿਖਰ = lnum;
    ਜੇ (lnum > buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ || lnum > ਵਿੱਚ_part->ch_buf_bot)
    {
#ਜੇ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ਜੇ (channel->ch_ਕੰਮ != ਨਿਰਾ)
	    ਪਦ_send_eof(channel);
#ਜੇ_ਅੰਤ

	// Writing is done, no longer need the buffer.
	ਵਿੱਚ_part->ch_bufref.br_buf = ਨਿਰਾ;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	ਜੇ (!channel->ch_anonymous_pipe)
#ਜੇ_ਅੰਤ
	    ch_close_part(channel, PART_ਵਿੱਚ);
    }
    ਨਹੀਂਤਾਂ
	ch_log(channel, "Still %ld more lines to write",
				   (ਲੰਮਾ)(buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    ਖਾਲੀ
channel_ਬਫਰ_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	ਲਈ (part = PART_SOCK; part < PART_ਗਿਣਤੀ; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ਜੇ (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_ਨਾਂ[part]);
		ch_part->ch_bufref.br_buf = ਨਿਰਾ;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    ਖਾਲੀ
channel_write_ਨਿਵੇਸ਼(channel_T *channel)
{
    chanpart_T	*ਵਿੱਚ_part = &channel->ch_part[PART_ਵਿੱਚ];

    ਜੇ (ਵਿੱਚ_part->ch_writeque.wq_ਅਗਲਾ != ਨਿਰਾ)
	channel_send(channel, PART_ਵਿੱਚ, (char_u *)"", 0, "channel_write_input");
    ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_part->ch_bufref.br_buf != ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_part->ch_buf_append)
	    channel_write_new_ਸਤਰਾਂ(ਵਿੱਚ_part->ch_bufref.br_buf);
	ਨਹੀਂਤਾਂ
	    channel_write_ਵਿੱਚ(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    ਖਾਲੀ
channel_write_any_ਸਤਰਾਂ(ਖਾਲੀ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_ਨਿਵੇਸ਼(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    ਖਾਲੀ
channel_write_new_ਸਤਰਾਂ(buf_T *buf)
{
    channel_T	*channel;
    ਪੂਰਨ		ਮਿਲਿਆ_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *ਵਿੱਚ_part = &channel->ch_part[PART_ਵਿੱਚ];
	linenr_T    lnum;
	ਪੂਰਨ	    written = 0;

	ਜੇ (ਵਿੱਚ_part->ch_bufref.br_buf == buf && ਵਿੱਚ_part->ch_buf_append)
	{
	    ਜੇ (ਵਿੱਚ_part->ch_fd == INVALID_FD)
		ਜਾਰੀ;  // pipe was closed
	    ਮਿਲਿਆ_one = TRUE;
	    ਲਈ (lnum = ਵਿੱਚ_part->ch_buf_bot; lnum < buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
								       ++lnum)
	    {
		ਜੇ (!can_write_buf_ਸਤਰ(channel))
		    ਰੋਕੋ;
		write_buf_ਸਤਰ(buf, lnum, channel);
		++written;
	    }

	    ਜੇ (written == 1)
		ch_log(channel, "written line %d to channel", (ਪੂਰਨ)lnum - 1);
	    ਨਹੀਂਤਾਂ ਜੇ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ਜੇ (lnum < buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		ch_log(channel, "Still %ld more lines to write",
				       (ਲੰਮਾ)(buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ - lnum));

	    ਵਿੱਚ_part->ch_buf_bot = lnum;
	}
    }
    ਜੇ (!ਮਿਲਿਆ_one)
	buf->b_write_ਮੰਜ਼ਿਲ_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ਸਥਿਰ ਖਾਲੀ
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    ਜੇ (ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback == 0)
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
    readq_T *ਸਿਰ = &channel->ch_part[part].ch_ਸਿਰ;

    ਵਾਪਸ ਸਿਰ->rq_ਅਗਲਾ;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_ਪਹਿਲਾ_nl(readq_T *ਗੰਢ)
{
    char_u  *ਬਫਰ = ਗੰਢ->rq_ਬਫਰ;
    long_u  i;

    ਲਈ (i = 0; i < ਗੰਢ->rq_buflen; ++i)
	ਜੇ (ਬਫਰ[i] == NL)
	    ਵਾਪਸ ਬਫਰ + i;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, ਪੂਰਨ *outlen)
{
    readq_T *ਸਿਰ = &channel->ch_part[part].ch_ਸਿਰ;
    readq_T *ਗੰਢ = ਸਿਰ->rq_ਅਗਲਾ;
    char_u *p;

    ਜੇ (ਗੰਢ == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (outlen != ਨਿਰਾ)
	*outlen += ਗੰਢ->rq_buflen;
    // dispose of the node but keep the buffer
    p = ਗੰਢ->rq_ਬਫਰ;
    ਸਿਰ->rq_ਅਗਲਾ = ਗੰਢ->rq_ਅਗਲਾ;
    ਜੇ (ਗੰਢ->rq_ਅਗਲਾ == ਨਿਰਾ)
	ਸਿਰ->rq_prev = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	ਗੰਢ->rq_ਅਗਲਾ->rq_prev = ਨਿਰਾ;
    vim_free(ਗੰਢ);
    ਵਾਪਸ p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ਸਥਿਰ char_u *
channel_get_all(channel_T *channel, ch_part_T part, ਪੂਰਨ *outlen)
{
    readq_T *ਸਿਰ = &channel->ch_part[part].ch_ਸਿਰ;
    readq_T *ਗੰਢ;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    ਲਈ (ਗੰਢ = ਸਿਰ->rq_ਅਗਲਾ; ਗੰਢ != ਨਿਰਾ; ਗੰਢ = ਗੰਢ->rq_ਅਗਲਾ)
	len += ਗੰਢ->rq_buflen;
    res = alloc(len + 1);
    ਜੇ (res == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    p = res;
    ਲਈ (ਗੰਢ = ਸਿਰ->rq_ਅਗਲਾ; ਗੰਢ != ਨਿਰਾ; ਗੰਢ = ਗੰਢ->rq_ਅਗਲਾ)
    {
	mch_memmove(p, ਗੰਢ->rq_ਬਫਰ, ਗੰਢ->rq_buflen);
	p += ਗੰਢ->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    ਕਰੋ
    {
	p = channel_get(channel, part, ਨਿਰਾ);
	vim_free(p);
    } ਜਦੋਂ (p != ਨਿਰਾ);

    ਜੇ (outlen != ਨਿਰਾ)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ਵਾਪਸ res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ਜਦੋਂ (p < res + len)
    {
	ਜੇ (*p == NUL)
	    *p = NL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	ਨਹੀਂਤਾਂ ਜੇ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ਜੇ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ਜਦੋਂ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ਜੇ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#ਜੇ_ਅੰਤ
	++p;
    }

    ਵਾਪਸ res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    ਖਾਲੀ
channel_consume(channel_T *channel, ch_part_T part, ਪੂਰਨ len)
{
    readq_T *ਸਿਰ = &channel->ch_part[part].ch_ਸਿਰ;
    readq_T *ਗੰਢ = ਸਿਰ->rq_ਅਗਲਾ;
    char_u *buf = ਗੰਢ->rq_ਬਫਰ;

    ਜੇ (len < 0 || (long_u)len > ਗੰਢ->rq_buflen)
	ਵਾਪਸ;
    mch_memmove(buf, buf + len, ਗੰਢ->rq_buflen - len);
    ਗੰਢ->rq_buflen -= len;
    ਗੰਢ->rq_ਬਫਰ[ਗੰਢ->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    ਪੂਰਨ
channel_collapse(channel_T *channel, ch_part_T part, ਪੂਰਨ want_nl)
{
    ch_ਬਹੁਲਕ_T	ਬਹੁਲਕ = channel->ch_part[part].ch_ਬਹੁਲਕ;
    readq_T	*ਸਿਰ = &channel->ch_part[part].ch_ਸਿਰ;
    readq_T	*ਗੰਢ = ਸਿਰ->rq_ਅਗਲਾ;
    readq_T	*ਆਖਰੀ_ਗੰਢ;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ਜੇ (ਗੰਢ == ਨਿਰਾ || ਗੰਢ->rq_ਅਗਲਾ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਆਖਰੀ_ਗੰਢ = ਗੰਢ->rq_ਅਗਲਾ;
    len = ਗੰਢ->rq_buflen + ਆਖਰੀ_ਗੰਢ->rq_buflen;
    ਜੇ (want_nl || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
	ਜਦੋਂ (ਆਖਰੀ_ਗੰਢ->rq_ਅਗਲਾ != ਨਿਰਾ
		&& (ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP
		    || channel_ਪਹਿਲਾ_nl(ਆਖਰੀ_ਗੰਢ) == ਨਿਰਾ))
	{
	    ਆਖਰੀ_ਗੰਢ = ਆਖਰੀ_ਗੰਢ->rq_ਅਗਲਾ;
	    len += ਆਖਰੀ_ਗੰਢ->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ਜੇ (newbuf == ਨਿਰਾ)
	ਵਾਪਸ FAIL;	    // out of memory
    mch_memmove(p, ਗੰਢ->rq_ਬਫਰ, ਗੰਢ->rq_buflen);
    p += ਗੰਢ->rq_buflen;
    vim_free(ਗੰਢ->rq_ਬਫਰ);
    ਗੰਢ->rq_ਬਫਰ = newbuf;
    ਲਈ (n = ਗੰਢ; n != ਆਖਰੀ_ਗੰਢ; )
    {
	n = n->rq_ਅਗਲਾ;
	mch_memmove(p, n->rq_ਬਫਰ, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_ਬਫਰ);
    }
    *p = NUL;
    ਗੰਢ->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    ਲਈ (n = ਗੰਢ->rq_ਅਗਲਾ; n != ਆਖਰੀ_ਗੰਢ; )
    {
	n = n->rq_ਅਗਲਾ;
	vim_free(n->rq_prev);
    }
    ਗੰਢ->rq_ਅਗਲਾ = ਆਖਰੀ_ਗੰਢ->rq_ਅਗਲਾ;
    ਜੇ (ਆਖਰੀ_ਗੰਢ->rq_ਅਗਲਾ == ਨਿਰਾ)
	ਸਿਰ->rq_prev = ਗੰਢ;
    ਨਹੀਂਤਾਂ
	ਆਖਰੀ_ਗੰਢ->rq_ਅਗਲਾ->rq_prev = ਗੰਢ;
    vim_free(ਆਖਰੀ_ਗੰਢ);
    ਵਾਪਸ OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_save(channel_T *channel, ch_part_T part, char_u *buf, ਪੂਰਨ len,
						      ਪੂਰਨ prepend, ਅੱਖਰ *lead)
{
    readq_T *ਗੰਢ;
    readq_T *ਸਿਰ = &channel->ch_part[part].ch_ਸਿਰ;
    char_u  *p;
    ਪੂਰਨ	    i;

    ਗੰਢ = ALLOC_ONE(readq_T);
    ਜੇ (ਗੰਢ == ਨਿਰਾ)
	ਵਾਪਸ FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    ਗੰਢ->rq_ਬਫਰ = alloc(len + 1);
    ਜੇ (ਗੰਢ->rq_ਬਫਰ == ਨਿਰਾ)
    {
	vim_free(ਗੰਢ);
	ਵਾਪਸ FAIL;	    // out of memory
    }

    ਜੇ (channel->ch_part[part].ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_NL)
    {
	// Drop any CR before a NL.
	p = ਗੰਢ->rq_ਬਫਰ;
	ਲਈ (i = 0; i < len; ++i)
	    ਜੇ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	ਗੰਢ->rq_buflen = (long_u)(p - ਗੰਢ->rq_ਬਫਰ);
    }
    ਨਹੀਂਤਾਂ
    {
	mch_memmove(ਗੰਢ->rq_ਬਫਰ, buf, len);
	ਗੰਢ->rq_ਬਫਰ[len] = NUL;
	ਗੰਢ->rq_buflen = (long_u)len;
    }

    ਜੇ (prepend)
    {
	// prepend node to the head of the queue
	ਗੰਢ->rq_ਅਗਲਾ = ਸਿਰ->rq_ਅਗਲਾ;
	ਗੰਢ->rq_prev = ਨਿਰਾ;
	ਜੇ (ਸਿਰ->rq_ਅਗਲਾ == ਨਿਰਾ)
	    ਸਿਰ->rq_prev = ਗੰਢ;
	ਨਹੀਂਤਾਂ
	    ਸਿਰ->rq_ਅਗਲਾ->rq_prev = ਗੰਢ;
	ਸਿਰ->rq_ਅਗਲਾ = ਗੰਢ;
    }
    ਨਹੀਂਤਾਂ
    {
	// append node to the tail of the queue
	ਗੰਢ->rq_ਅਗਲਾ = ਨਿਰਾ;
	ਗੰਢ->rq_prev = ਸਿਰ->rq_prev;
	ਜੇ (ਸਿਰ->rq_prev == ਨਿਰਾ)
	    ਸਿਰ->rq_ਅਗਲਾ = ਗੰਢ;
	ਨਹੀਂਤਾਂ
	    ਸਿਰ->rq_prev->rq_ਅਗਲਾ = ਗੰਢ;
	ਸਿਰ->rq_prev = ਗੰਢ;
    }

    ਜੇ (ch_log_ਸਰਗਰਮ() && lead != ਨਿਰਾ)
	ch_log_literal(lead, channel, part, buf, len);

    ਵਾਪਸ OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    ਪੂਰਨ
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*ਅਗਲਾ = channel_get(channel, part, ਨਿਰਾ);
    ਪੂਰਨ		keeplen;
    ਪੂਰਨ		addlen;
    char_u	*p;

    ਜੇ (ਅਗਲਾ == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    keeplen = reader->js_ਸਿਰਾ - reader->js_buf;
    ਜੇ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (ਪੂਰਨ)STRLEN(ਅਗਲਾ);
	p = alloc(keeplen + addlen + 1);
	ਜੇ (p == ਨਿਰਾ)
	{
	    vim_free(ਅਗਲਾ);
	    ਵਾਪਸ FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, ਅਗਲਾ, addlen + 1);
	vim_free(ਅਗਲਾ);
	ਅਗਲਾ = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = ਅਗਲਾ;
    ਵਾਪਸ TRUE;
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
    ਸਥਿਰ ਪੂਰਨ
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*ਸਤਰ_ਸ਼ੁਰੂ;
    char_u	*p;
    int_u	hdr_len;
    ਪੂਰਨ		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_ਸਿਰਾ = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ਜਦੋਂ (TRUE)
    {
	ਸਤਰ_ਸ਼ੁਰੂ = p;
	ਜਦੋਂ (*p != NUL && *p != '\n')
	    p++;
	ਜੇ (*p == NUL)			// partial header
	    ਵਾਪਸ MAYBE;
	p++;

	// process the content length field (if present)
	ਜੇ ((p - ਸਤਰ_ਸ਼ੁਰੂ > 16)
		&& STRNICMP(ਸਤਰ_ਸ਼ੁਰੂ, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((ਅੱਖਰ *)ਸਤਰ_ਸ਼ੁਰੂ + 16, ਨਿਰਾ, 10);
	    ਜੇ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ਵਾਪਸ FAIL;
	}

	ਜੇ ((p - ਸਤਰ_ਸ਼ੁਰੂ) == 2 && ਸਤਰ_ਸ਼ੁਰੂ[0] == '\r' &&
		ਸਤਰ_ਸ਼ੁਰੂ[1] == '\n')
	    // reached the empty line
	    ਰੋਕੋ;
    }

    ਜੇ (payload_len == -1)
	// Content-Length field is not present in the header
	ਵਾਪਸ FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ਜੇ (jsbuf_len < hdr_len + payload_len)
	ਵਾਪਸ MAYBE;

    reader->js_ਵਰਤਿਆ += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_ਸਿਰਾ = reader->js_buf + hdr_len + payload_len;

    ਵਾਪਸ OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    ਪੂਰਨ
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*ਵਸਤੂ;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*ਸਿਰ = &chanpart->ch_json_ਸਿਰ;
    ਪੂਰਨ		ਸਥਿਤੀ = OK;
    ਪੂਰਨ		ret;

    ਜੇ (channel_peek(channel, part) == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    reader.js_buf = channel_get(channel, part, ਨਿਰਾ);
    reader.js_ਵਰਤਿਆ = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ਜੇ (!socketserver && (chanpart->ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP
	    || chanpart->ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP))
	ਸਥਿਤੀ = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ਜੇ (ਸਥਿਤੀ == OK)
    {
	++emsg_silent;
	ਸਥਿਤੀ = json_decode(&reader, &listtv,
				chanpart->ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ਜੇ (ਸਥਿਤੀ == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ਜੇ (!socketserver && (chanpart->ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP
		    || chanpart->ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	ਨਹੀਂਤਾਂ ਜੇ (!socketserver && chanpart->ch_ਬਹੁਲਕ != CH_ਬਹੁਲਕ_LSP
		&& chanpart->ch_ਬਹੁਲਕ != CH_ਬਹੁਲਕ_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ਜੇ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    ਨਹੀਂਤਾਂ
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਵਸਤੂ = ALLOC_ONE(jsonq_T);
	    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
		clear_tv(&listtv);
	    ਨਹੀਂਤਾਂ
	    {
		ਵਸਤੂ->jq_no_callback = FALSE;
		ਵਸਤੂ->jq_ਮੁੱਲ = alloc_tv();
		ਜੇ (ਵਸਤੂ->jq_ਮੁੱਲ == ਨਿਰਾ)
		{
		    vim_free(ਵਸਤੂ);
		    clear_tv(&listtv);
		}
		ਨਹੀਂਤਾਂ
		{
		    *ਵਸਤੂ->jq_ਮੁੱਲ = listtv;
		    ਵਸਤੂ->jq_prev = ਸਿਰ->jq_prev;
		    ਸਿਰ->jq_prev = ਵਸਤੂ;
		    ਵਸਤੂ->jq_ਅਗਲਾ = ਨਿਰਾ;
		    ਜੇ (ਵਸਤੂ->jq_prev == ਨਿਰਾ)
			ਸਿਰ->jq_ਅਗਲਾ = ਵਸਤੂ;
		    ਨਹੀਂਤਾਂ
			ਵਸਤੂ->jq_prev->jq_ਅਗਲਾ = ਵਸਤੂ;
		}
	    }
	}
    }

    ਜੇ (ਸਥਿਤੀ == OK)
	chanpart->ch_wait_len = 0;
    ਨਹੀਂਤਾਂ ਜੇ (ਸਥਿਤੀ == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ਜੇ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (ਪੂਰਨ)buflen);
	    reader.js_ਵਰਤਿਆ = 0;
	    chanpart->ch_wait_len = buflen;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    chanpart->ch_deadline = Get_Tick_ਗਿਣਤੀ() + 100L;
#ਨਹੀਂਤਾਂ
	    gettimeofday(&chanpart->ch_deadline, ਨਿਰਾ);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ਜੇ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ timeout;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    timeout = (ਪੂਰਨ)(Get_Tick_ਗਿਣਤੀ() - chanpart->ch_deadline) > 0;
#ਨਹੀਂਤਾਂ
	    {
		ਬਣਤਰ timeval now_tv;

		gettimeofday(&now_tv, ਨਿਰਾ);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#ਜੇ_ਅੰਤ
	    ਜੇ (timeout)
	    {
		ਸਥਿਤੀ = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    ਨਹੀਂਤਾਂ
	    {
		reader.js_ਵਰਤਿਆ = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ਜੇ (ਸਥਿਤੀ == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    ਨਹੀਂਤਾਂ ਜੇ (reader.js_buf[reader.js_ਵਰਤਿਆ] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_ਵਰਤਿਆ,
			(ਪੂਰਨ)(reader.js_ਸਿਰਾ - reader.js_buf) - reader.js_ਵਰਤਿਆ,
								  TRUE, ਨਿਰਾ);
	ret = ਸਥਿਤੀ == MAYBE ? FALSE: TRUE;
    }
    ਨਹੀਂਤਾਂ
	ret = FALSE;

    vim_free(reader.js_buf);
    ਵਾਪਸ ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ਸਥਿਰ ਖਾਲੀ
remove_cb_ਗੰਢ(cbq_T *ਸਿਰ, cbq_T *ਗੰਢ)
{
    ਜੇ (ਗੰਢ->cq_prev == ਨਿਰਾ)
	ਸਿਰ->cq_ਅਗਲਾ = ਗੰਢ->cq_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	ਗੰਢ->cq_prev->cq_ਅਗਲਾ = ਗੰਢ->cq_ਅਗਲਾ;
    ਜੇ (ਗੰਢ->cq_ਅਗਲਾ == ਨਿਰਾ)
	ਸਿਰ->cq_prev = ਗੰਢ->cq_prev;
    ਨਹੀਂਤਾਂ
	ਗੰਢ->cq_ਅਗਲਾ->cq_prev = ਗੰਢ->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    ਖਾਲੀ
remove_json_ਗੰਢ(jsonq_T *ਸਿਰ, jsonq_T *ਗੰਢ)
{
    ਜੇ (ਗੰਢ->jq_prev == ਨਿਰਾ)
	ਸਿਰ->jq_ਅਗਲਾ = ਗੰਢ->jq_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	ਗੰਢ->jq_prev->jq_ਅਗਲਾ = ਗੰਢ->jq_ਅਗਲਾ;
    ਜੇ (ਗੰਢ->jq_ਅਗਲਾ == ਨਿਰਾ)
	ਸਿਰ->jq_prev = ਗੰਢ->jq_prev;
    ਨਹੀਂਤਾਂ
	ਗੰਢ->jq_ਅਗਲਾ->jq_prev = ਗੰਢ->jq_prev;
    vim_free(ਗੰਢ);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_ਜੋੜੋ_ਖੰਡ_id(chanpart_T *chanpart, ਪੂਰਨ id)
{
    garray_T *gap = &chanpart->ch_ਖੰਡ_ids;

    ਜੇ (gap->ga_growsize == 0)
	ga_init2(gap, ਆਕਾਰ(ਪੂਰਨ), 10);
    ਜੇ (ga_grow(gap, 1) == OK)
    {
	((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_remove_ਖੰਡ_id(chanpart_T *chanpart, ਪੂਰਨ id)
{
    garray_T	*gap = &chanpart->ch_ਖੰਡ_ids;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < gap->ga_len; ++i)
	ਜੇ (((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[i] == id)
	{
	    --gap->ga_len;
	    ਜੇ (i < gap->ga_len)
	    {
		ਪੂਰਨ *p = ((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * ਆਕਾਰ(ਪੂਰਨ));
	    }
	    ਵਾਪਸ;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_has_ਖੰਡ_id(chanpart_T *chanpart, ਪੂਰਨ id)
{
    garray_T	*gap = &chanpart->ch_ਖੰਡ_ids;
    ਪੂਰਨ		i;

    ਲਈ (i = 0; i < gap->ga_len; ++i)
	ਜੇ (((ਪੂਰਨ *)gap->ga_ਅੰਕੜੇ)[i] == id)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
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
    ਸਥਿਰ ਪੂਰਨ
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	ਪੂਰਨ	    id,
	ਪੂਰਨ	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *ਸਿਰ = &channel->ch_part[part].ch_json_ਸਿਰ;
    jsonq_T   *ਵਸਤੂ = ਸਿਰ->jq_ਅਗਲਾ;

    ਜਦੋਂ (ਵਸਤੂ != ਨਿਰਾ)
    {
	list_T	    *l;
	typval_T    *tv;

	ਜੇ (channel->ch_part[part].ch_ਬਹੁਲਕ != CH_ਬਹੁਲਕ_LSP
		&& channel->ch_part[part].ch_ਬਹੁਲਕ != CH_ਬਹੁਲਕ_DAP)
	{
	    l = ਵਸਤੂ->jq_ਮੁੱਲ->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_ਪਹਿਲਾ->li_tv;
	}
	ਨਹੀਂਤਾਂ
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ਜੇ (channel->ch_part[part].ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ਜੇ (id > 0)
		{
		    ਜੇ (ਵਸਤੂ->jq_ਮੁੱਲ->v_type != VAR_DICT)
			ਜਾਓ nextitem;
		    d = ਵਸਤੂ->jq_ਮੁੱਲ->vval.v_dict;
		    ਜੇ (d == ਨਿਰਾ)
			ਜਾਓ nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ਜੇ (dict_has_ਕੁੰਜੀ(d, "method"))
			ਜਾਓ nextitem;
		    di = dict_ਲੱਭੋ(d, (char_u *)"id", -1);
		    ਜੇ (di == ਨਿਰਾ)
			ਜਾਓ nextitem;
		    tv = &di->di_tv;
		}
		ਨਹੀਂਤਾਂ
		    tv = ਵਸਤੂ->jq_ਮੁੱਲ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (id > 0)
		{
		    ਜੇ (ਵਸਤੂ->jq_ਮੁੱਲ->v_type != VAR_DICT)
			ਜਾਓ nextitem;
		    d = ਵਸਤੂ->jq_ਮੁੱਲ->vval.v_dict;
		    ਜੇ (d == ਨਿਰਾ)
			ਜਾਓ nextitem;
		    di = dict_ਲੱਭੋ(d, (char_u *)"request_seq", -1);
		    ਜੇ (di == ਨਿਰਾ)
			ਜਾਓ nextitem;
		    tv = &di->di_tv;
		}
		ਨਹੀਂਤਾਂ
		    tv = ਵਸਤੂ->jq_ਮੁੱਲ;
	    }
	}

	ਜੇ ((without_callback || !ਵਸਤੂ->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_ਸੰਖਿਆ && tv->vval.v_ਸੰਖਿਆ == id)
	      || (id <= 0 && (tv->v_type != VAR_ਸੰਖਿਆ
		 || tv->vval.v_ਸੰਖਿਆ == 0
		 || !channel_has_ਖੰਡ_id(
				&channel->ch_part[part], tv->vval.v_ਸੰਖਿਆ)))))
	{
	    *rettv = ਵਸਤੂ->jq_ਮੁੱਲ;
	    ਜੇ (tv->v_type == VAR_ਸੰਖਿਆ)
		ch_log(channel, "Getting JSON message %ld",
						      (ਲੰਮਾ)tv->vval.v_ਸੰਖਿਆ);
	    remove_json_ਗੰਢ(ਸਿਰ, ਵਸਤੂ);
	    ਵਾਪਸ OK;
	}
nextitem:
	ਵਸਤੂ = ਵਸਤੂ->jq_ਅਗਲਾ;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ਸਥਿਰ ਖਾਲੀ
channel_ਧੱਕੋ_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *ਸਿਰ = &channel->ch_part[part].ch_json_ਸਿਰ;
    jsonq_T   *ਵਸਤੂ = ਸਿਰ->jq_ਅਗਲਾ;
    jsonq_T   *newitem;

    ਜੇ (ਸਿਰ->jq_prev != ਨਿਰਾ && ਸਿਰ->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	ਵਸਤੂ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜਦੋਂ (ਵਸਤੂ != ਨਿਰਾ && ਵਸਤੂ->jq_no_callback)
	// append after the last item that was pushed back
	ਵਸਤੂ = ਵਸਤੂ->jq_ਅਗਲਾ;

    newitem = ALLOC_ONE(jsonq_T);
    ਜੇ (newitem == ਨਿਰਾ)
    {
	clear_tv(rettv);
	ਵਾਪਸ;
    }

    newitem->jq_ਮੁੱਲ = alloc_tv();
    ਜੇ (newitem->jq_ਮੁੱਲ == ਨਿਰਾ)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ਵਾਪਸ;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_ਮੁੱਲ = *rettv;
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
    {
	// append to the end
	newitem->jq_prev = ਸਿਰ->jq_prev;
	ਸਿਰ->jq_prev = newitem;
	newitem->jq_ਅਗਲਾ = ਨਿਰਾ;
	ਜੇ (newitem->jq_prev == ਨਿਰਾ)
	    ਸਿਰ->jq_ਅਗਲਾ = newitem;
	ਨਹੀਂਤਾਂ
	    newitem->jq_prev->jq_ਅਗਲਾ = newitem;
    }
    ਨਹੀਂਤਾਂ
    {
	// append after "item"
	newitem->jq_prev = ਵਸਤੂ;
	newitem->jq_ਅਗਲਾ = ਵਸਤੂ->jq_ਅਗਲਾ;
	ਵਸਤੂ->jq_ਅਗਲਾ = newitem;
	ਜੇ (newitem->jq_ਅਗਲਾ == ਨਿਰਾ)
	    ਸਿਰ->jq_prev = newitem;
	ਨਹੀਂਤਾਂ
	    newitem->jq_ਅਗਲਾ->jq_prev = newitem;
    }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CH_JSON_MAX_ਦਲੀਲਾਂ 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    ਪੂਰਨ	    options = channel->ch_part[part].ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_JS
								 ? JSON_JS : 0;

    ਜੇ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ਜੇ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_ਦਲੀਲ));
	ਵਾਪਸ;
    }
    arg = argv[1].vval.v_string;
    ਜੇ (arg == ਨਿਰਾ)
	arg = (char_u *)"";

    ਜੇ (STRCMP(cmd, "ex") == 0)
    {
	ਪੂਰਨ	called_emsg_before = called_emsg;
	char_u	*p = arg;
	ਪੂਰਨ	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (ਅੱਖਰ *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ਜੇ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ਜੇ (do_emsg_silent)
	    --emsg_silent;
	ਜੇ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (ਅੱਖਰ *)get_vim_var_str(VV_ERRMSG));
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (ਅੱਖਰ *)arg);
	CLEAR_ਖੇਤਰ(ea);
	ea.arg = arg;
	ea.addr_ਗਿਣਤੀ = 0;
	ea.forceit = TRUE; // no mapping
	ex_ਸਾਧਾਰਨ(&ea);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	ਬਾਹਰਲਾ_flush_cursor(TRUE, FALSE);
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	ਪੂਰਨ is_call = cmd[0] == 'c';
	ਪੂਰਨ id_idx = is_call ? 3 : 2;

	ਜੇ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_ਸੰਖਿਆ)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ਜੇ (p_verbose > 2)
		emsg(_(e_ਆਖਰੀ_ਦਲੀਲ_for_ਵਿਅੰਜਕ_call_must_be_ਸੰਖਿਆ));
	}
	ਨਹੀਂਤਾਂ ਜੇ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ਜੇ (p_verbose > 2)
		emsg(_(e_ਤੀਜਾ_ਦਲੀਲ_for_call_must_be_list));
	}
	ਨਹੀਂਤਾਂ
	{
	    typval_T	*tv = ਨਿਰਾ;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ਨਿਰਾ;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ਜੇ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (ਅੱਖਰ *)arg);
		tv = eval_ਵਿਅੰਜਕ(arg, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ch_log(channel, "Calling '%s'", (ਅੱਖਰ *)arg);
		ਜੇ (func_call(arg, &argv[2], ਨਿਰਾ, ਨਿਰਾ, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ਜੇ (argv[id_idx].v_type == VAR_ਸੰਖਿਆ)
	    {
		ਪੂਰਨ id = argv[id_idx].vval.v_ਸੰਖਿਆ;

		ਜੇ (tv != ਨਿਰਾ)
		    json = json_encode_nr_ਵਿਅੰਜਕ(id, tv, options | JSON_NL);
		ਜੇ (tv == ਨਿਰਾ || (json != ਨਿਰਾ && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_ਵਿਅੰਜਕ(id, &err_tv, options | JSON_NL);
		}
		ਜੇ (json != ਨਿਰਾ)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_ਵਿੱਚ,
				 json, (ਪੂਰਨ)STRLEN(json), (ਅੱਖਰ *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ਜੇ (tv == &res_tv)
		clear_tv(tv);
	    ਨਹੀਂਤਾਂ
		free_tv(tv);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (ਅੱਖਰ *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    ਸਥਿਰ ਖਾਲੀ
invoke_one_ਸਮਾਂ_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *ਵਸਤੂ,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (ਅੱਖਰ *)ਵਸਤੂ->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_ਗੰਢ(cbhead, ਵਸਤੂ);
    invoke_callback(channel, &ਵਸਤੂ->cq_callback, argv);
    free_callback(&ਵਸਤੂ->cq_callback);
    vim_free(ਵਸਤੂ);
}

    ਸਥਿਰ ਖਾਲੀ
append_ਮੰਜ਼ਿਲ_ਬਫਰ(
    buf_T	*ਬਫਰ,
    char_u	*ਸੁਨੇਹਾ,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    bool	save_write_ਮੰਜ਼ਿਲ = ਬਫਰ->b_write_ਮੰਜ਼ਿਲ_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    ਪੂਰਨ		save_p_ma = ਬਫਰ->b_p_ma;
    ਪੂਰਨ		ਖਾਲੀ_ਥਾਵਾਂ_2 = (ਬਫਰ->b_ml.ml_flags & ML_ਖਾਲੀ_ਥਾਵਾਂ) ? 1 : 0;

    ਜੇ (!ਬਫਰ->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ਜੇ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ਵਾਪਸ;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ਜੇ (save_write_ਮੰਜ਼ਿਲ)
    {
	--lnum;
	ਬਫਰ->b_write_ਮੰਜ਼ਿਲ_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (ਪੂਰਨ)lnum + 1 - ਖਾਲੀ_ਥਾਵਾਂ_2, ਬਫਰ->b_fname);

    ਬਫਰ->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, ਬਫਰ);
    ਜੇ (curbuf != ਬਫਰ)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ਵਾਪਸ;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - ਖਾਲੀ_ਥਾਵਾਂ_2, lnum + 1);

    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_2)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, ਸੁਨੇਹਾ, TRUE);
	lnum = 0;
    }
    ਨਹੀਂਤਾਂ
	ml_append(lnum, ਸੁਨੇਹਾ, 0, FALSE);
    appended_ਸਤਰਾਂ_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ਜੇ (ch_part->ch_nomodifiable)
	ਬਫਰ->b_p_ma = FALSE;
    ਨਹੀਂਤਾਂ
	ਬਫਰ->b_p_ma = save_p_ma;

    ਜੇ (ਬਫਰ->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ਜੇ (wp->w_ਬਫਰ == ਬਫਰ)
	    {
		ਪੂਰਨ move_cursor = save_write_ਮੰਜ਼ਿਲ
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.ਖਾਨਾ == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ਜੇ (move_cursor || wp->w_topline > ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
		{
		    win_T *save_curwin = curwin;

		    ਜੇ (move_cursor)
			++wp->w_cursor.lnum;
		    curwin = wp;
		    curbuf = curwin->w_ਬਫਰ;
		    scroll_cursor_bot(0, FALSE);
		    curwin = save_curwin;
		    curbuf = curwin->w_ਬਫਰ;
		}
	    }
	}
	redraw_buf_and_ਸਥਿਤੀ_later(ਬਫਰ, UPD_VALID);
	channel_need_redraw = TRUE;
    }

    ਜੇ (save_write_ਮੰਜ਼ਿਲ)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	ਬਫਰ->b_write_ਮੰਜ਼ਿਲ_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *ਵਿੱਚ_part = &ch->ch_part[PART_ਵਿੱਚ];

	    ਜੇ (ਵਿੱਚ_part->ch_bufref.br_buf == ਬਫਰ)
		ਵਿੱਚ_part->ch_buf_bot = ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
	}
    }
}

    ਸਥਿਰ ਖਾਲੀ
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *ਸੁਨੇਹਾ;

    ਜਦੋਂ ((ਸੁਨੇਹਾ = channel_get(channel, part, ਨਿਰਾ)) != ਨਿਰਾ)
    {
	ch_log(channel, "Dropping message '%s'", (ਅੱਖਰ *)ਸੁਨੇਹਾ);
	vim_free(ਸੁਨੇਹਾ);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_use_json_ਸਿਰ(channel_T *channel, ch_part_T part)
{
    ch_ਬਹੁਲਕ_T	ch_ਬਹੁਲਕ = channel->ch_part[part].ch_ਬਹੁਲਕ;

    ਵਾਪਸ ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_JSON || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_JS
						     || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP
						     || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ਸਥਿਰ ਪੂਰਨ
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*ਸੁਨੇਹਾ = ਨਿਰਾ;
    blob_T	*blob = ਨਿਰਾ;
    typval_T	*listtv = ਨਿਰਾ;
    typval_T	argv[CH_JSON_MAX_ਦਲੀਲਾਂ];
    ਪੂਰਨ		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_ਬਹੁਲਕ_T	ch_ਬਹੁਲਕ = ch_part->ch_ਬਹੁਲਕ;
    cbq_T	*cbhead = &ch_part->ch_cb_ਸਿਰ;
    cbq_T	*cbitem;
    callback_T	*callback = ਨਿਰਾ;
    buf_T	*ਬਫਰ = ਨਿਰਾ;
    char_u	*p;
    ਪੂਰਨ		called_otc;		// one time callbackup
    ਪੂਰਨ		raw_len = 0;

    ਜੇ (channel->ch_nb_close_cb != ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#ਜੇ_ਅੰਤ
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ਵਾਪਸ FALSE;

    // Use a message-specific callback, part callback or channel callback
    ਲਈ (cbitem = cbhead->cq_ਅਗਲਾ; cbitem != ਨਿਰਾ; cbitem = cbitem->cq_ਅਗਲਾ)
	ਜੇ (cbitem->cq_seq_nr == 0)
	    ਰੋਕੋ;
    ਜੇ (cbitem != ਨਿਰਾ)
	callback = &cbitem->cq_callback;
    ਨਹੀਂਤਾਂ ਜੇ (ch_part->ch_callback.cb_name != ਨਿਰਾ)
	callback = &ch_part->ch_callback;
    ਨਹੀਂਤਾਂ ਜੇ (channel->ch_callback.cb_name != ਨਿਰਾ)
	callback = &channel->ch_callback;

    ਬਫਰ = ch_part->ch_bufref.br_buf;
    ਜੇ (ਬਫਰ != ਨਿਰਾ && (!bufref_valid(&ch_part->ch_bufref)
					       || ਬਫਰ->b_ml.ml_mfp == ਨਿਰਾ))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_ਨਾਂ[part]);
	ch_part->ch_bufref.br_buf = ਨਿਰਾ;
	ਬਫਰ = ਨਿਰਾ;
    }

    ਜੇ (channel_use_json_ਸਿਰ(channel, part))
    {
	listitem_T	*ਵਸਤੂ;
	ਪੂਰਨ		argc = 0;

	// Get any json message in the queue.
	ਜੇ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(ਖਾਲੀ)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ਜੇ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		ਵਾਪਸ FALSE;
	}

	ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ਜੇ (d != ਨਿਰਾ)
	    {
		ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP)
		    di = dict_ਲੱਭੋ(d, (char_u *)"id", -1);
		ਨਹੀਂਤਾਂ
		    di = dict_ਲੱਭੋ(d, (char_u *)"request_seq", -1);
		ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_ਸੰਖਿਆ)
		    seq_nr = di->di_tv.vval.v_ਸੰਖਿਆ;
	    }

	    argv[1] = *listtv;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਲਈ (ਵਸਤੂ = listtv->vval.v_list->lv_ਪਹਿਲਾ;
		    ਵਸਤੂ != ਨਿਰਾ && argc < CH_JSON_MAX_ਦਲੀਲਾਂ;
		    ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ)
		argv[argc++] = ਵਸਤੂ->li_tv;
	    ਜਦੋਂ (argc < CH_JSON_MAX_ਦਲੀਲਾਂ)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ਜੇ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ਵਾਪਸ TRUE;
	    }

	    ਜੇ (argv[0].v_type != VAR_ਸੰਖਿਆ)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ਵਾਪਸ FALSE;
	    }
	    seq_nr = argv[0].vval.v_ਸੰਖਿਆ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (channel_peek(channel, part) == ਨਿਰਾ)
    {
	// nothing to read on RAW or NL channel
	ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ
    {
	// If there is no callback or buffer drop the message.
	ਜੇ (callback == ਨਿਰਾ && ਬਫਰ == ਨਿਰਾ)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ਜੇ (channel->ch_close_cb.cb_name == ਨਿਰਾ && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ਵਾਪਸ FALSE;
	}

	ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_NL)
	{
	    char_u  *nl = ਨਿਰਾ;
	    char_u  *buf;
	    readq_T *ਗੰਢ;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ਜਦੋਂ (TRUE)
	    {
		ਗੰਢ = channel_peek(channel, part);
		nl = channel_ਪਹਿਲਾ_nl(ਗੰਢ);
		ਜੇ (nl != ਨਿਰਾ)
		    ਰੋਕੋ;
		ਜੇ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ਜੇ (ch_part->ch_fd == INVALID_FD && ਗੰਢ->rq_buflen > 0)
			ਰੋਕੋ;
		    ਵਾਪਸ FALSE; // incomplete message
		}
	    }
	    buf = ਗੰਢ->rq_ਬਫਰ;

	    // Convert NUL to NL, the internal representation.
	    ਲਈ (p = buf; (nl == ਨਿਰਾ || p < nl)
					    && p < buf + ਗੰਢ->rq_buflen; ++p)
		ਜੇ (*p == NUL)
		    *p = NL;

	    ਜੇ (nl == ਨਿਰਾ)
	    {
		// get the whole buffer, drop the NL
		ਸੁਨੇਹਾ = channel_get(channel, part, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (nl + 1 == buf + ਗੰਢ->rq_buflen)
	    {
		// get the whole buffer
		ਸੁਨੇਹਾ = channel_get(channel, part, ਨਿਰਾ);
		*nl = NUL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		ਸੁਨੇਹਾ = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (ਪੂਰਨ)(nl - buf) + 1);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    ਸੁਨੇਹਾ = channel_get_all(channel, part,
			ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_BLOB ? &raw_len : ਨਿਰਾ);
	}

	ਜੇ (ਸੁਨੇਹਾ == ਨਿਰਾ)
	    ਵਾਪਸ FALSE; // out of memory (and avoids Coverity warning)

	ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_BLOB)
	{
	    blob = blob_alloc();
	    ਜੇ (blob == ਨਿਰਾ)
	    {
		vim_free(ਸੁਨੇਹਾ);
		ਵਾਪਸ FALSE;
	    }
	    ਜੇ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(ਸੁਨੇਹਾ);
		ਵਾਪਸ FALSE;
	    }
	    ਜੇ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_ਅੰਕੜੇ, ਸੁਨੇਹਾ, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	ਨਹੀਂਤਾਂ
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = ਸੁਨੇਹਾ;
	}
    }

    called_otc = FALSE;
    ਜੇ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	ਪੂਰਨ lsp_req_ਸੁਨੇਹਾ = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ਜੇ ((ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_ਕੁੰਜੀ(argv[1].vval.v_dict, "method"))
	    lsp_req_ਸੁਨੇਹਾ = TRUE;

	ਜੇ (!lsp_req_ਸੁਨੇਹਾ)
	{
	    ਲਈ (cbitem = cbhead->cq_ਅਗਲਾ; cbitem != ਨਿਰਾ;
		    cbitem = cbitem->cq_ਅਗਲਾ)
	    {
		ਜੇ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_ਸਮਾਂ_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    ਰੋਕੋ;
		}
	    }
	}
    }

    ਜੇ (seq_nr > 0 && ((ch_ਬਹੁਲਕ != CH_ਬਹੁਲਕ_LSP && ch_ਬਹੁਲਕ != CH_ਬਹੁਲਕ_DAP)
		|| called_otc))
    {
	ਜੇ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ਜੇ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ਧੱਕੋ_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ਸੰਖਿਆ;
		free_tv(listtv);
		listtv = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (callback != ਨਿਰਾ || ਬਫਰ != ਨਿਰਾ)
    {
	ਜੇ (ਬਫਰ != ਨਿਰਾ)
	{
	    ਜੇ (ਸੁਨੇਹਾ == ਨਿਰਾ)
		// JSON or JS mode: re-encode the message.
		ਸੁਨੇਹਾ = json_encode(listtv, ch_ਬਹੁਲਕ);
	    ਜੇ (ਸੁਨੇਹਾ != ਨਿਰਾ)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
		ਜੇ (ਬਫਰ->b_ਪਦ != ਨਿਰਾ)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ਜੇ (part == PART_ERR)
		    {
			char_u *cp;
			ਪੂਰਨ    extra = 0;

			ਲਈ (cp = ਸੁਨੇਹਾ; *cp != NUL; ++cp)
			    ਜੇ (*cp == NL && (cp == ਸੁਨੇਹਾ || cp[-1] != CAR))
				++extra;
			ਜੇ (extra > 0)
			{
			    char_u *crlf_ਸੁਨੇਹਾ = alloc(STRLEN(ਸੁਨੇਹਾ) + extra + 1);

			    ਜੇ (crlf_ਸੁਨੇਹਾ != ਨਿਰਾ)
			    {
				char_u *q = crlf_ਸੁਨੇਹਾ;

				ਲਈ (cp = ਸੁਨੇਹਾ; *cp != NUL; ++cp)
				{
				    ਜੇ (*cp == NL && (cp == ਸੁਨੇਹਾ || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_ਮੰਜ਼ਿਲ_ਪਦ(ਬਫਰ, crlf_ਸੁਨੇਹਾ, channel);
				vim_free(crlf_ਸੁਨੇਹਾ);
			    }
			}
			ਨਹੀਂਤਾਂ
			    write_ਮੰਜ਼ਿਲ_ਪਦ(ਬਫਰ, ਸੁਨੇਹਾ, channel);
		    }
		    ਨਹੀਂਤਾਂ
			write_ਮੰਜ਼ਿਲ_ਪਦ(ਬਫਰ, ਸੁਨੇਹਾ, channel);
		}
		ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		    append_ਮੰਜ਼ਿਲ_ਬਫਰ(ਬਫਰ, ਸੁਨੇਹਾ, channel, part);
	    }
	}

	ਜੇ (callback != ਨਿਰਾ)
	{
	    ਜੇ (cbitem != ਨਿਰਾ)
		invoke_one_ਸਮਾਂ_callback(channel, cbhead, cbitem, argv);
	    ਨਹੀਂਤਾਂ
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (ਅੱਖਰ *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	ch_log(channel, "Dropping message %d", seq_nr);

    ਜੇ (listtv != ਨਿਰਾ)
	free_tv(listtv);
    ਜੇ (blob != ਨਿਰਾ)
	blob_unref(blob);
    vim_free(ਸੁਨੇਹਾ);

    ਵਾਪਸ TRUE;
}

#ਜੇ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    ਪੂਰਨ
channel_can_write_ਮੰਜ਼ਿਲ(channel_T *channel)
{
    ਵਾਪਸ channel != ਨਿਰਾ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_ਵਿੱਚ_FD != INVALID_FD);
}
#ਜੇ_ਅੰਤ

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    ਪੂਰਨ
channel_is_open(channel_T *channel)
{
    ਵਾਪਸ channel != ਨਿਰਾ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_ਵਿੱਚ_FD != INVALID_FD
			  || channel->CH_ਬਾਹਰਲਾ_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ਸਥਿਰ ਖਾਲੀ *
channel_readahead_ਸੂਚਕ(channel_T *channel, ch_part_T part)
{
    ਜੇ (channel_use_json_ਸਿਰ(channel, part))
    {
	jsonq_T   *ਸਿਰ = &channel->ch_part[part].ch_json_ਸਿਰ;

	ਜੇ (ਸਿਰ->jq_ਅਗਲਾ == ਨਿਰਾ)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ਵਾਪਸ ਸਿਰ->jq_ਅਗਲਾ;
    }
    ਵਾਪਸ channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ਵਾਪਸ channel_readahead_ਸੂਚਕ(channel, part) != ਨਿਰਾ;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ਸਥਿਰ ਅੱਖਰ *
channel_ਸਥਿਤੀ(channel_T *channel, ਪੂਰਨ req_part)
{
    ch_part_T part;
    ਪੂਰਨ has_readahead = FALSE;

    ਜੇ (channel == ਨਿਰਾ)
	 ਵਾਪਸ "fail";
    ਜੇ (req_part == PART_ਬਾਹਰਲਾ)
    {
	ਜੇ (channel->CH_ਬਾਹਰਲਾ_FD != INVALID_FD)
	    ਵਾਪਸ "open";
	ਜੇ (channel_has_readahead(channel, PART_ਬਾਹਰਲਾ))
	    has_readahead = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (req_part == PART_ERR)
    {
	ਜੇ (channel->CH_ERR_FD != INVALID_FD)
	    ਵਾਪਸ "open";
	ਜੇ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (channel_is_open(channel))
	    ਵਾਪਸ "open";
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	    ਜੇ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		ਰੋਕੋ;
	    }
    }

    ਜੇ (has_readahead)
	ਵਾਪਸ "buffered";
    ਵਾਪਸ "closed";
}

    ਸਥਿਰ ਖਾਲੀ
channel_part_info(channel_T *channel, dict_T *dict, ਅੱਖਰ *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    ਅੱਖਰ	namebuf[20];  // longest is "sock_timeout"
    size_t	ਪੂਛ;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    ਪੂਛ = STRLEN(namebuf);
    STRCPY(namebuf + ਪੂਛ, "_");
    ਪੂਛ += STRLEN_LITERAL("_");

    STRCPY(namebuf + ਪੂਛ, "status");
    ਜੇ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_ਸੈੱਟ(s, "open");
    ਨਹੀਂਤਾਂ ਜੇ (channel_has_readahead(channel, part))
	STR_LITERAL_ਸੈੱਟ(s, "buffered");
    ਨਹੀਂਤਾਂ
	STR_LITERAL_ਸੈੱਟ(s, "closed");
    dict_ਜੋੜੋ_string_len(dict, namebuf, s.string, (ਪੂਰਨ)s.ਲੰਮਾਈ);

    STRCPY(namebuf + ਪੂਛ, "mode");
    ਚੋਣ (chanpart->ch_ਬਹੁਲਕ)
    {
	ਹਾਲਤ CH_ਬਹੁਲਕ_NL:
	    STR_LITERAL_ਸੈੱਟ(s, "NL");
	    ਰੋਕੋ;
	ਹਾਲਤ CH_ਬਹੁਲਕ_RAW:
	    STR_LITERAL_ਸੈੱਟ(s, "RAW");
	    ਰੋਕੋ;
	ਹਾਲਤ CH_ਬਹੁਲਕ_BLOB:
	    STR_LITERAL_ਸੈੱਟ(s, "BLOB");
	    ਰੋਕੋ;
	ਹਾਲਤ CH_ਬਹੁਲਕ_JSON:
	    STR_LITERAL_ਸੈੱਟ(s, "JSON");
	    ਰੋਕੋ;
	ਹਾਲਤ CH_ਬਹੁਲਕ_JS:
	    STR_LITERAL_ਸੈੱਟ(s, "JS");
	    ਰੋਕੋ;
	ਹਾਲਤ CH_ਬਹੁਲਕ_LSP:
	    STR_LITERAL_ਸੈੱਟ(s, "LSP");
	    ਰੋਕੋ;
	ਹਾਲਤ CH_ਬਹੁਲਕ_DAP:
	    STR_LITERAL_ਸੈੱਟ(s, "DAP");
	    ਰੋਕੋ;
	ਮੂਲ:
	    STR_LITERAL_ਸੈੱਟ(s, "");
	    ਰੋਕੋ;
    }
    dict_ਜੋੜੋ_string_len(dict, namebuf, s.string, (ਪੂਰਨ)s.ਲੰਮਾਈ);

    STRCPY(namebuf + ਪੂਛ, "io");
    ਜੇ (part == PART_SOCK)
	STR_LITERAL_ਸੈੱਟ(s, "socket");
    ਨਹੀਂਤਾਂ ਚੋਣ (chanpart->ch_io)
    {
	ਹਾਲਤ JIO_NULL:
	    STR_LITERAL_ਸੈੱਟ(s, "null");
	    ਰੋਕੋ;
	ਹਾਲਤ JIO_PIPE:
	    STR_LITERAL_ਸੈੱਟ(s, "pipe");
	    ਰੋਕੋ;
	ਹਾਲਤ JIO_FILE:
	    STR_LITERAL_ਸੈੱਟ(s, "file");
	    ਰੋਕੋ;
	ਹਾਲਤ JIO_ਬਫਰ:
	    STR_LITERAL_ਸੈੱਟ(s, "buffer");
	    ਰੋਕੋ;
	ਹਾਲਤ JIO_ਬਾਹਰਲਾ:
	    STR_LITERAL_ਸੈੱਟ(s, "out");
	    ਰੋਕੋ;
	ਮੂਲ:
	    STR_LITERAL_ਸੈੱਟ(s, "");
	    ਰੋਕੋ;
    }
    dict_ਜੋੜੋ_string_len(dict, namebuf, s.string, (ਪੂਰਨ)s.ਲੰਮਾਈ);

    STRCPY(namebuf + ਪੂਛ, "timeout");
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, namebuf, chanpart->ch_timeout);
}

    ਸਥਿਰ ਖਾਲੀ
channel_info(channel_T *channel, dict_T *dict)
{
    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "id", channel->ch_id);
    dict_ਜੋੜੋ_string(dict, "status", (char_u *)channel_ਸਥਿਤੀ(channel, -1));

    ਜੇ (channel->ch_hostname != ਨਿਰਾ)
    {
	ਜੇ (channel->ch_ਪੋਰਟ)
	    dict_ਜੋੜੋ_ਸੰਖਿਆ(dict, "port", channel->ch_ਪੋਰਟ);
	ਨਹੀਂਤਾਂ
	    // Unix-domain socket.
	    dict_ਜੋੜੋ_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    ਨਹੀਂਤਾਂ
    {
	channel_part_info(channel, dict, "out", PART_ਬਾਹਰਲਾ);
	channel_part_info(channel, dict, "err", PART_ERR);
	channel_part_info(channel, dict, "in", PART_ਵਿੱਚ);
    }
}

/*
 * Close channel "channel".
 * Trigger the close callback if "invoke_close_cb" is TRUE.
 * Does not clear the buffers.
 */
    ਖਾਲੀ
channel_close(channel_T *channel, ਪੂਰਨ invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    channel_gui_unregister(channel);
#ਜੇ_ਅੰਤ

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_ਵਿੱਚ);
    ch_close_part(channel, PART_ਬਾਹਰਲਾ);
    ch_close_part(channel, PART_ERR);

    ਜੇ (invoke_close_cb)
    {
	ch_part_T	part;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	ਪਦ_channel_closing(channel);
#ਜੇ_ਅੰਤ
	// Invoke callbacks and flush buffers before the close callback.
	ਜੇ (channel->ch_close_cb.cb_name != ਨਿਰਾ)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	{
	    ਜੇ (channel->ch_close_cb.cb_name != ਨਿਰਾ
			    || channel->ch_part[part].ch_bufref.br_buf != ਨਿਰਾ)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ਜੇ (channel->ch_close_cb.cb_name == ਨਿਰਾ)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_ਨਾਂ[part]);
		ਜਦੋਂ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ਜੇ (channel->ch_close_cb.cb_name != ਨਿਰਾ)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (ਅੱਖਰ *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      ਜੇ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ਜੇ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ਨਿਰਾ;
    channel->ch_ss_close_cb = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
    ਪਦ_channel_closed(channel);
#ਜੇ_ਅੰਤ
}

/*
 * Close the "in" part channel "channel".
 */
    ਸਥਿਰ ਖਾਲੀ
channel_close_ਵਿੱਚ(channel_T *channel)
{
    ch_close_part(channel, PART_ਵਿੱਚ);
}

    ਸਥਿਰ ਖਾਲੀ
remove_from_writeque(writeq_T *wq, writeq_T *ਇੰਦਰਾਜ)
{
    ga_clear(&ਇੰਦਰਾਜ->wq_ga);
    wq->wq_ਅਗਲਾ = ਇੰਦਰਾਜ->wq_ਅਗਲਾ;
    ਜੇ (wq->wq_ਅਗਲਾ == ਨਿਰਾ)
	wq->wq_prev = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	wq->wq_ਅਗਲਾ->wq_prev = ਨਿਰਾ;
    vim_free(ਇੰਦਰਾਜ);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ਸਥਿਰ ਖਾਲੀ
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_ਸਿਰ = &ch_part->ch_json_ਸਿਰ;
    cbq_T   *cb_ਸਿਰ = &ch_part->ch_cb_ਸਿਰ;

    ਜਦੋਂ (channel_peek(channel, part) != ਨਿਰਾ)
	vim_free(channel_get(channel, part, ਨਿਰਾ));

    ਜਦੋਂ (cb_ਸਿਰ->cq_ਅਗਲਾ != ਨਿਰਾ)
    {
	cbq_T *ਗੰਢ = cb_ਸਿਰ->cq_ਅਗਲਾ;

	remove_cb_ਗੰਢ(cb_ਸਿਰ, ਗੰਢ);
	free_callback(&ਗੰਢ->cq_callback);
	vim_free(ਗੰਢ);
    }

    ਜਦੋਂ (json_ਸਿਰ->jq_ਅਗਲਾ != ਨਿਰਾ)
    {
	free_tv(json_ਸਿਰ->jq_ਅਗਲਾ->jq_ਮੁੱਲ);
	remove_json_ਗੰਢ(json_ਸਿਰ, json_ਸਿਰ->jq_ਅਗਲਾ);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_ਖੰਡ_ids);

    ਜਦੋਂ (ch_part->ch_writeque.wq_ਅਗਲਾ != ਨਿਰਾ)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_ਅਗਲਾ);
}

/*
 * Clear all the read buffers on "channel".
 */
    ਖਾਲੀ
channel_clear(channel_T *channel)
{
    ch_log(channel, "Clearing channel");
    VIM_CLEAR(channel->ch_hostname);
    channel_clear_one(channel, PART_SOCK);
    channel_clear_one(channel, PART_ਬਾਹਰਲਾ);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_ਵਿੱਚ);
    free_callback(&channel->ch_callback);
    free_callback(&channel->ch_close_cb);
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
channel_free_all(ਖਾਲੀ)
{
    channel_T *channel;

    ch_log(ਨਿਰਾ, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#ਜੇ_ਅੰਤ


// Sent when the netbeans channel is found closed when reading.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DETACH_ਸੁਨੇਹਾ_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ਸਥਿਰ ਪੂਰਨ
is_channel_write_ਬਚਿਆ(chanpart_T *ਵਿੱਚ_part)
{
    buf_T *buf = ਵਿੱਚ_part->ch_bufref.br_buf;

    ਜੇ (ਵਿੱਚ_part->ch_writeque.wq_ਅਗਲਾ != ਨਿਰਾ)
	ਵਾਪਸ TRUE;
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ ਵਿੱਚ_part->ch_buf_append
	    ? (ਵਿੱਚ_part->ch_buf_bot < buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    : (ਵਿੱਚ_part->ch_buf_ਸਿਖਰ <= ਵਿੱਚ_part->ch_buf_bot
			    && ਵਿੱਚ_part->ch_buf_ਸਿਖਰ <= buf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
}

#ਜੇ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_fill_wfds(ਪੂਰਨ maxfd_arg, fd_set *wfds)
{
    ਪੂਰਨ		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *ਵਿੱਚ_part = &ch->ch_part[PART_ਵਿੱਚ];

	ਜੇ (ਵਿੱਚ_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (ਪੂਰਨ)ਵਿੱਚ_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_ਬਚਿਆ(ਵਿੱਚ_part))
	{
	    FD_SET((ਪੂਰਨ)ਵਿੱਚ_part->ch_fd, wfds);
	    ਜੇ ((ਪੂਰਨ)ਵਿੱਚ_part->ch_fd >= maxfd)
		maxfd = (ਪੂਰਨ)ਵਿੱਚ_part->ch_fd + 1;
	}
    }
    ਵਾਪਸ maxfd;
}
#ਨਹੀਂਤਾਂ
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_fill_poll_write(ਪੂਰਨ nfd_ਵਿੱਚ, ਬਣਤਰ pollfd *fds)
{
    ਪੂਰਨ		nfd = nfd_ਵਿੱਚ;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *ਵਿੱਚ_part = &ch->ch_part[PART_ਵਿੱਚ];

	ਜੇ (ਵਿੱਚ_part->ch_fd != INVALID_FD
		&& is_channel_write_ਬਚਿਆ(ਵਿੱਚ_part))
	{
	    ਵਿੱਚ_part->ch_poll_idx = nfd;
	    fds[nfd].fd = ਵਿੱਚ_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	ਨਹੀਂਤਾਂ
	    ਵਿੱਚ_part->ch_poll_idx = -1;
    }
    ਵਾਪਸ nfd;
}
#ਜੇ_ਅੰਤ

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    CW_READY,
    CW_NOT_READY,
    CW_ERROR
} channel_wait_ਨਤੀਜਾ;

/*
 * Check for reading from "fd" with "timeout" msec.
 * Return CW_READY when there is something to read.
 * Return CW_NOT_READY when there is nothing to read.
 * Return CW_ERROR when there is an error.
 */
    ਸਥਿਰ channel_wait_ਨਤੀਜਾ
channel_wait(channel_T *channel, sock_T fd, ਪੂਰਨ timeout)
{
    ਜੇ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    ਜੇ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	ਪੂਰਨ	sleep_ਸਮਾਂ;
	DWORD	deadline = Get_Tick_ਗਿਣਤੀ() + timeout;
	ਪੂਰਨ	delay = 1;

	// reading from a pipe, not a socket
	ਜਦੋਂ (TRUE)
	{
	    ਪੂਰਨ r = PeekNamedPipe((HANDLE)fd, ਨਿਰਾ, 0, ਨਿਰਾ, &nread, ਨਿਰਾ);

	    ਜੇ (r && nread > 0)
		ਵਾਪਸ CW_READY;

	    ਜੇ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (r == 0)
		ਵਾਪਸ CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_ਸਤਰਾਂ();

	    sleep_ਸਮਾਂ = deadline - Get_Tick_ਗਿਣਤੀ();
	    ਜੇ (sleep_ਸਮਾਂ <= 0)
		ਰੋਕੋ;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ਜੇ (sleep_ਸਮਾਂ > delay)
		sleep_ਸਮਾਂ = delay;
	    Sleep(sleep_ਸਮਾਂ);
	    delay = delay * 2;
	    ਜੇ (delay > 10)
		delay = 10;
	}
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
#ਜੇ defined(HAVE_SELECT)
	ਬਣਤਰ timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	ਪੂਰਨ		ret;
	ਪੂਰਨ		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	ਲਈ (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((ਪੂਰਨ)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (ਪੂਰਨ)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ਨਿਰਾ, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ਜੇ (ret == -1 && errno == EINTR)
		ਜਾਰੀ;
# endif
	    ਜੇ (ret > 0)
	    {
		ਜੇ (FD_ISSET(fd, &rfds))
		    ਵਾਪਸ CW_READY;
		channel_write_any_ਸਤਰਾਂ();
		ਜਾਰੀ;
	    }
	    ਰੋਕੋ;
	}
#ਨਹੀਂਤਾਂ
	ਲਈ (;;)
	{
	    ਬਣਤਰ pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    ਪੂਰਨ		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ਜੇ (poll(fds, nfd, timeout) > 0)
	    {
		ਜੇ (fds[0].revents & POLLIN)
		    ਵਾਪਸ CW_READY;
		channel_write_any_ਸਤਰਾਂ();
		ਜਾਰੀ;
	    }
	    ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ CW_NOT_READY;
}

    ਸਥਿਰ ਖਾਲੀ
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, ਪੂਰਨ is_err, ਅੱਖਰ *func)
{
    ਅੱਖਰ	ਸੁਨੇਹਾ[] = "%s(): Read %s from ch_part[%d], closing";

    ਜੇ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, ਸੁਨੇਹਾ, func, "error", part);
    ਨਹੀਂਤਾਂ
	ch_log(channel, ਸੁਨੇਹਾ, func, "EOF", part);

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
    ਜੇ (channel->ch_nb_close_cb != ਨਿਰਾ)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_ਸੁਨੇਹਾ_RAW,
			      (ਪੂਰਨ)STRLEN_LITERAL(DETACH_ਸੁਨੇਹਾ_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ਜੇ (part == PART_ਬਾਹਰਲਾ || part == PART_ERR)
    {
	ch_part_T other = part == PART_ਬਾਹਰਲਾ ? PART_ERR : PART_ਬਾਹਰਲਾ;

	ਜੇ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    ਜੇ (channel->ch_nb_close_cb != ਨਿਰਾ)
	(*channel->ch_nb_close_cb)();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
    ਜੇ (channel->ch_ss_close_cb != ਨਿਰਾ)
	channel->ch_ss_close_cb(channel);
#ਜੇ_ਅੰਤ
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_read(channel_T *channel, ch_part_T part, ਅੱਖਰ *func)
{
    ਸਥਿਰ char_u	*buf = ਨਿਰਾ;
    ਪੂਰਨ			len = 0;
    ਪੂਰਨ			readlen = 0;
    sock_T		fd;
    ਪੂਰਨ			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ਜੇ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_ਨਾਂ[part]);
	ਵਾਪਸ;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ਜੇ (buf == ਨਿਰਾ)
    {
	buf = alloc(MAXMSGSIZE);
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    ਲਈ (;;)
    {
	ਜੇ (channel_wait(channel, fd, 0) != CW_READY)
	    ਰੋਕੋ;
	ਜੇ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    ਬਣਤਰ sockaddr_storage	client;

	    newchannel = ਜੋੜੋ_channel();
	    ਜੇ (newchannel == ਨਿਰਾ)
	    {
		ch_error(ਨਿਰਾ, "Cannot allocate channel.");
		ਵਾਪਸ;
	    }
	    socklen = ਆਕਾਰ(client);
	    newfd = accept(fd, (ਬਣਤਰ sockaddr*)&client, &socklen);
	    ਜੇ (newfd < 0)
	    {
		ch_error(ਨਿਰਾ, "Cannot accept channel.");
		channel_free(newchannel);
		ਵਾਪਸ;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_ਮੰਜ਼ਿਲ_be_closed |= (1U << PART_SOCK);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    channel_gui_ਰਜਿਸਟਰ_one(newchannel, PART_SOCK);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER
	    ਜੇ (channel->ch_ss_accept_cb != ਨਿਰਾ)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ਵਾਪਸ;
	    }
#ਜੇ_ਅੰਤ

	    ਜੇ (client.ss_family == AF_INET)
	    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_INET_NTOP
		ਅੱਖਰ addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((ਬਣਤਰ sockaddr_ਵਿੱਚ*)&client)->sin_addr,
			addr, ਆਕਾਰ(addr));
		vim_snprintf((ਅੱਖਰ *)namebuf, ਆਕਾਰ(namebuf), "%s:%d",
			addr,
			ntohs(((ਬਣਤਰ sockaddr_ਵਿੱਚ*)&client)->sin_ਪੋਰਟ));
#ਨਹੀਂਤਾਂ
		vim_snprintf((ਅੱਖਰ *)namebuf, ਆਕਾਰ(namebuf), "%s:%d",
		    inet_ntoa(((ਬਣਤਰ sockaddr_ਵਿੱਚ*)&client)->sin_addr),
		    ntohs(((ਬਣਤਰ sockaddr_ਵਿੱਚ*)&client)->sin_ਪੋਰਟ));
#ਜੇ_ਅੰਤ
	    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_INET_NTOP
	    ਨਹੀਂਤਾਂ ਜੇ (client.ss_family == AF_INET6)
	    {
		ਅੱਖਰ addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((ਬਣਤਰ sockaddr_in6*)&client)->sin6_addr,
			addr, ਆਕਾਰ(addr));
		vim_snprintf((ਅੱਖਰ *)namebuf, ਆਕਾਰ(namebuf), "[%s]:%d",
			addr,
			ntohs(((ਬਣਤਰ sockaddr_in6*)&client)->sin6_ਪੋਰਟ));
	    }
#ਜੇ_ਅੰਤ
	    ਨਹੀਂਤਾਂ ਜੇ (client.ss_family == AF_UNIX)
		vim_snprintf((ਅੱਖਰ *)namebuf, ਆਕਾਰ(namebuf),
							    "unix:anonymous");
	    ਨਹੀਂਤਾਂ
		vim_snprintf((ਅੱਖਰ *)namebuf, ਆਕਾਰ(namebuf), "unknown");
	    ++ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback;
	    clear_tv(&argv[1]);
	    ਵਾਪਸ;
	}
	ਜੇ (use_socket)
	    len = sock_read(fd, (ਅੱਖਰ *)buf, MAXMSGSIZE);
	ਨਹੀਂਤਾਂ
	    len = fd_read(fd, (ਅੱਖਰ *)buf, MAXMSGSIZE);
	ਜੇ (len <= 0)
	    ਰੋਕੋ;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ਜੇ (readlen <= 0)
    {
	ਜੇ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ਜੇ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    ਨਹੀਂਤਾਂ ਜੇ (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SOCKETSERVER

    ਖਾਲੀ
channel_check(channel_T *channel, ch_part_T part)
{
    channel_read(channel, part, "channel_check");
}

#ਜੇ_ਅੰਤ

/*
 * Read from RAW or NL "channel"/"part".  Blocks until there is something to
 * read or the timeout expires.
 * When "raw" is TRUE don't block waiting on a NL.
 * Does not trigger timers or handle messages.
 * Returns what was read in allocated memory.
 * Returns NULL in case of error or timeout.
 */
    ਸਥਿਰ char_u *
channel_read_ਖੰਡ(
	channel_T *channel, ch_part_T part, ਪੂਰਨ timeout, ਪੂਰਨ raw, ਪੂਰਨ *outlen)
{
    char_u	*buf;
    char_u	*ਸੁਨੇਹਾ;
    ch_ਬਹੁਲਕ_T	ਬਹੁਲਕ = channel->ch_part[part].ch_ਬਹੁਲਕ;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*ਗੰਢ;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  ਬਹੁਲਕ == CH_ਬਹੁਲਕ_RAW ? "RAW"
				: ਬਹੁਲਕ == CH_ਬਹੁਲਕ_BLOB ? "BLOB" : "NL", timeout);

    ਜਦੋਂ (TRUE)
    {
	ਗੰਢ = channel_peek(channel, part);
	ਜੇ (ਗੰਢ != ਨਿਰਾ)
	{
	    ਜੇ (ਬਹੁਲਕ == CH_ਬਹੁਲਕ_RAW || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_BLOB
		    || (ਬਹੁਲਕ == CH_ਬਹੁਲਕ_NL
					   && channel_ਪਹਿਲਾ_nl(ਗੰਢ) != ਨਿਰਾ))
		// got a complete message
		ਰੋਕੋ;
	    ਜੇ (channel_collapse(channel, part, ਬਹੁਲਕ == CH_ਬਹੁਲਕ_NL) == OK)
		ਜਾਰੀ;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ਜੇ (raw || fd == INVALID_FD)
		ਰੋਕੋ;
	}

	// Wait for up to the channel timeout.
	ਜੇ (fd == INVALID_FD)
	    ਵਾਪਸ ਨਿਰਾ;
	ਜੇ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ਵਾਪਸ ਨਿਰਾ;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ਜੇ (ਬਹੁਲਕ == CH_ਬਹੁਲਕ_RAW || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_BLOB || outlen != ਨਿਰਾ)
    {
	ਸੁਨੇਹਾ = channel_get_all(channel, part, outlen);
    }
    ਨਹੀਂਤਾਂ
    {
	char_u *p;

	buf = ਗੰਢ->rq_ਬਫਰ;
	nl = channel_ਪਹਿਲਾ_nl(ਗੰਢ);

	// Convert NUL to NL, the internal representation.
	ਲਈ (p = buf; (nl == ਨਿਰਾ || p < nl) && p < buf + ਗੰਢ->rq_buflen; ++p)
	    ਜੇ (*p == NUL)
		*p = NL;

	ਜੇ (nl == ਨਿਰਾ)
	{
	    // must be a closed channel with missing NL
	    ਸੁਨੇਹਾ = channel_get(channel, part, ਨਿਰਾ);
	}
	ਨਹੀਂਤਾਂ ਜੇ (nl + 1 == buf + ਗੰਢ->rq_buflen)
	{
	    // get the whole buffer
	    ਸੁਨੇਹਾ = channel_get(channel, part, ਨਿਰਾ);
	    *nl = NUL;
	}
	ਨਹੀਂਤਾਂ
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    ਸੁਨੇਹਾ = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (ਪੂਰਨ)(nl - buf) + 1);
	}
    }
    ਜੇ (ch_log_ਸਰਗਰਮ())
	ch_log(channel, "Returning %d bytes",
		outlen != ਨਿਰਾ ? *outlen : (ਪੂਰਨ)STRLEN(ਸੁਨੇਹਾ));
    ਵਾਪਸ ਸੁਨੇਹਾ;
}

ਸਥਿਰ ਪੂਰਨ channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    ਪੂਰਨ
channel_ਵਿੱਚ_blocking_wait(ਖਾਲੀ)
{
    ਵਾਪਸ channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_wait_reading(channel_T *channel ਅਣਵਰਤਿਆ, sock_T fd ਅਣਵਰਤਿਆ, ਪੂਰਨ timeout)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (ਖਾਲੀ)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#ਨਹੀਂਤਾਂ
    channel_wait_any(timeout);
#ਜੇ_ਅੰਤ
}

/*
 * Read one JSON message with ID "id" from "channel"/"part" and store the
 * result in "rettv".
 * When "id" is -1 accept any message;
 * Blocks until the message is received or the timeout is reached.
 * In corner cases this can be called recursively, that is why ch_block_ids is
 * a list.
 */
    ਸਥਿਰ ਪੂਰਨ
channel_read_json_ਖੰਡ(
	channel_T   *channel,
	ch_part_T   part,
	ਪੂਰਨ	    timeout_arg,
	ਪੂਰਨ	    id,
	typval_T    **rettv)
{
    ਪੂਰਨ		ਹੋਰ;
    sock_T	fd;
    ਪੂਰਨ		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_ਬਹੁਲਕ_T	ਬਹੁਲਕ = channel->ch_part[part].ch_ਬਹੁਲਕ;
    ਪੂਰਨ		retval = FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    elapsed_T	ਸ਼ੁਰੂ_tv;

    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
#ਜੇ_ਅੰਤ

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ਜੇ (id >= 0)
	channel_ਜੋੜੋ_ਖੰਡ_id(chanpart, id);

    ਲਈ (;;)
    {
	ਜੇ (ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (ਖਾਲੀ)channel_collapse(channel, part, FALSE);

	ਹੋਰ = channel_parse_json(channel, part, false);

	// search for message "id"
	ਜੇ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    ਰੋਕੋ;
	}

	ਜੇ (!ਹੋਰ)
	{
	    ਖਾਲੀ *prev_readahead_ptr = channel_readahead_ਸੂਚਕ(channel, part);
	    ਖਾਲੀ *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ਜੇ (channel_parse_messages())
		ਜਾਰੀ;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_ਸੂਚਕ(channel, part);
	    ਜੇ (readahead_ptr != ਨਿਰਾ && readahead_ptr != prev_readahead_ptr)
		ਜਾਰੀ;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
	    timeout -= (ਪੂਰਨ)ELAPSED_FUNC(ਸ਼ੁਰੂ_tv);
#ਜੇ_ਅੰਤ
	    fd = chanpart->ch_fd;
	    ਜੇ (timeout <= 0 || fd == INVALID_FD)
	    {
		ਜੇ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		ਰੋਕੋ;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ਜੇ (chanpart->ch_wait_len > 0)
	    {
		ਪੂਰਨ	ਖੱਬਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
		ਖੱਬਾ = chanpart->ch_deadline - Get_Tick_ਗਿਣਤੀ() + 1;
#ਨਹੀਂਤਾਂ
		{
		    ਬਣਤਰ timeval now_tv;

		    gettimeofday(&now_tv, ਨਿਰਾ);
		    ਖੱਬਾ = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#ਜੇ_ਅੰਤ
		ਜੇ (ਖੱਬਾ < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		ਨਹੀਂਤਾਂ ਜੇ (ਖੱਬਾ < timeout)
		    timeout = ਖੱਬਾ;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#ਜੇ_ਅੰਤ
	}
    }
    ਜੇ (id >= 0)
	channel_remove_ਖੰਡ_id(chanpart, id);
    --channel_blocking_wait;

    ਵਾਪਸ retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, ਪੂਰਨ check_open, ਪੂਰਨ reading, ch_part_T part)
{
    channel_T	*channel = ਨਿਰਾ;
    ਪੂਰਨ		has_readahead = FALSE;

    ਜੇ (tv->v_type == VAR_ਕੰਮ)
    {
	ਜੇ (tv->vval.v_ਕੰਮ != ਨਿਰਾ)
	    channel = tv->vval.v_ਕੰਮ->jv_channel;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_invalid_ਦਲੀਲ_str), tv_get_string(tv));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (channel != ਨਿਰਾ && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_ਗਿਣਤੀ ? part : channel_part_read(channel));

    ਜੇ (check_open && (channel == ਨਿਰਾ || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ਸਥਿਰ ਖਾਲੀ
common_channel_read(typval_T *argvars, typval_T *rettv, ਪੂਰਨ raw, ਪੂਰਨ blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_ਗਿਣਤੀ;
    jobopt_T	opt;
    ਪੂਰਨ		ਬਹੁਲਕ;
    ਪੂਰਨ		timeout;
    ਪੂਰਨ		id = -1;
    typval_T	*listtv = ਨਿਰਾ;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    clear_ਕੰਮ_options(&opt);
    ਜੇ (get_ਕੰਮ_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	ਜਾਓ theend;

    ਜੇ (opt.jo_ਸੈੱਟ & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ਜੇ (channel == ਨਿਰਾ)
	ਜਾਓ theend;

    ਜੇ (part == PART_ਗਿਣਤੀ)
	part = channel_part_read(channel);
    ਬਹੁਲਕ = channel_get_ਬਹੁਲਕ(channel, part);
    timeout = channel_get_timeout(channel, part);
    ਜੇ (opt.jo_ਸੈੱਟ & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ਜੇ (blob || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_BLOB)
    {
	ਪੂਰਨ	    outlen = 0;
	char_u  *p = channel_read_ਖੰਡ(channel, part,
		timeout, TRUE, &outlen);
	ਜੇ (p != ਨਿਰਾ)
	{
	    blob_T	*b = blob_alloc();

	    ਜੇ (b != ਨਿਰਾ)
	    {
		b->bv_ga.ga_len = outlen;
		ਜੇ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		ਨਹੀਂਤਾਂ
		{
		    ਸਮ੍ਰਿਤੀ_ਨਕਲ(b->bv_ga.ga_ਅੰਕੜੇ, p, outlen);
		    rettv_blob_ਸੈੱਟ(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (raw || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_RAW || ਬਹੁਲਕ == CH_ਬਹੁਲਕ_NL)
	rettv->vval.v_string = channel_read_ਖੰਡ(channel, part,
		timeout, raw, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (opt.jo_ਸੈੱਟ & JO_ID)
	    id = opt.jo_id;
	channel_read_json_ਖੰਡ(channel, part, timeout, id, &listtv);
	ਜੇ (listtv != ਨਿਰਾ)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	ਨਹੀਂਤਾਂ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ਸੰਖਿਆ = VVAL_NONE;
	}
    }

theend:
    free_ਕੰਮ_options(&opt);
}

#ਜੇ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    ਖਾਲੀ
channel_handle_events(ਪੂਰਨ only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ਜੇ (only_keep_open && !channel->ch_keep_open)
	    ਜਾਰੀ;

	// check the socket and pipes
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ਜੇ (fd == INVALID_FD)
		ਜਾਰੀ;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    ਪੂਰਨ r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_ਬਾਹਰਲਾ || part == PART_ERR)) ? 1 : 0);

	    ਜੇ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    ਨਹੀਂਤਾਂ ਜੇ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ਜੇ (channel->ch_ਕੰਮ != ਨਿਰਾ)
	{
	    ਕੰਮ_T *ਕੰਮ = channel->ch_ਕੰਮ;

	    ਜੇ (ਕੰਮ->jv_tty_ਬਾਹਰਲਾ != ਨਿਰਾ && ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_ਮੁਕੰਮਲ)
		ਲਈ (part = PART_SOCK; part < PART_ਗਿਣਤੀ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    ਪੂਰਨ
channel_any_keep_open(ਖਾਲੀ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ਜੇ (channel->ch_keep_open)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    ਖਾਲੀ
channel_ਸੈੱਟ_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    ਪੂਰਨ		fd = ch_part->ch_fd;

    ਜੇ (fd == INVALID_FD)
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#ਨਹੀਂਤਾਂ
    (ਖਾਲੀ)fcntl(fd, F_SETFL, O_NONBLOCK);
#ਜੇ_ਅੰਤ
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    ਪੂਰਨ
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	ਪੂਰਨ	  len_arg,
	ਅੱਖਰ	  *fun)
{
    ਪੂਰਨ		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ਪੂਰਨ		did_use_ਕਤਾਰ = FALSE;

    fd = ch_part->ch_fd;
    ਜੇ (fd == INVALID_FD)
    {
	ਜੇ (!channel->ch_error && fun != ਨਿਰਾ)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ਵਾਪਸ FAIL;
    }

    ਜੇ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_ਸੈੱਟ_nonblock(channel, part);

    ਜੇ (ch_log_ਸਰਗਰਮ())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_ਸੁਨੇਹਾ = 0;
    }

    ਲਈ (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	ਪੂਰਨ	    len;

	ਜੇ (wq->wq_ਅਗਲਾ != ਨਿਰਾ)
	{
	    // first write what was queued
	    buf = wq->wq_ਅਗਲਾ->wq_ga.ga_ਅੰਕੜੇ;
	    len = wq->wq_ਅਗਲਾ->wq_ga.ga_len;
	    did_use_ਕਤਾਰ = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ਵਾਪਸ OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ਜੇ (part == PART_SOCK)
	    res = sock_write(fd, (ਅੱਖਰ *)buf, len);
	ਨਹੀਂਤਾਂ
	{
	    res = fd_write(fd, (ਅੱਖਰ *)buf, len);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
	    ਜੇ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ਨਿਰਾ);
	    }
#ਜੇ_ਅੰਤ
	}
	ਜੇ (res < 0 && (errno == EWOULDBLOCK
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EAGAIN
			|| errno == EAGAIN
#ਜੇ_ਅੰਤ
		    ))
	    res = 0; // nothing got written

	ਜੇ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *ਇੰਦਰਾਜ = wq->wq_ਅਗਲਾ;

	    ਜੇ (did_use_ਕਤਾਰ)
		ch_log(channel, "Sent %d bytes now", res);
	    ਜੇ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, ਇੰਦਰਾਜ);
		    ਜਾਰੀ;
		}
		ਜੇ (did_use_ਕਤਾਰ)
		    ch_log(channel, "Write queue empty");
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ਜੇ (ਇੰਦਰਾਜ != ਨਿਰਾ)
		{
		    ਜੇ (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(ਇੰਦਰਾਜ->wq_ga.ga_ਅੰਕੜੇ,
				    (ਅੱਖਰ *)ਇੰਦਰਾਜ->wq_ga.ga_ਅੰਕੜੇ + res,
				    len - res);
			ਇੰਦਰਾਜ->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		ਨਹੀਂਤਾਂ
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ਜੇ (wq->wq_prev != ਨਿਰਾ
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *ਆਖਰੀ = wq->wq_prev;

		    // append to the last entry
		    ਜੇ (len > 0 && ga_grow(&ਆਖਰੀ->wq_ga, len) == OK)
		    {
			mch_memmove((ਅੱਖਰ *)ਆਖਰੀ->wq_ga.ga_ਅੰਕੜੇ
							  + ਆਖਰੀ->wq_ga.ga_len,
				    buf, len);
			ਆਖਰੀ->wq_ga.ga_len += len;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    writeq_T *ਆਖਰੀ = ALLOC_ONE(writeq_T);

		    ਜੇ (ਆਖਰੀ != ਨਿਰਾ)
		    {
			ਆਖਰੀ->wq_prev = wq->wq_prev;
			ਆਖਰੀ->wq_ਅਗਲਾ = ਨਿਰਾ;
			ਜੇ (wq->wq_prev == ਨਿਰਾ)
			    wq->wq_ਅਗਲਾ = ਆਖਰੀ;
			ਨਹੀਂਤਾਂ
			    wq->wq_prev->wq_ਅਗਲਾ = ਆਖਰੀ;
			wq->wq_prev = ਆਖਰੀ;
			ga_init2(&ਆਖਰੀ->wq_ga, 1, 1000);
			ਜੇ (len > 0 && ga_grow(&ਆਖਰੀ->wq_ga, len) == OK)
			{
			    mch_memmove(ਆਖਰੀ->wq_ga.ga_ਅੰਕੜੇ, buf, len);
			    ਆਖਰੀ->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (res != len)
	{
	    ਜੇ (!channel->ch_error && fun != ਨਿਰਾ)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ਵਾਪਸ FAIL;
	}

	channel->ch_error = FALSE;
	ਵਾਪਸ OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    ਸਥਿਰ channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *ਪਾਠ,
	ਪੂਰਨ	    len,
	ਪੂਰਨ	    id,
	ਪੂਰਨ	    eval,
	jobopt_T    *opt,
	ਅੱਖਰ	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_ਕੰਮ_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ਜੇ (get_ਕੰਮ_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ਜੇ (opt->jo_callback.cb_name != ਨਿਰਾ && *opt->jo_callback.cb_name != NUL)
    {
	ਜੇ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ਵਾਪਸ ਨਿਰਾ;
	}
	channel_ਸੈੱਟ_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ਜੇ (channel_send(channel, part_send, ਪਾਠ, len, fun) == OK
					   && opt->jo_callback.cb_name == ਨਿਰਾ)
	ਵਾਪਸ channel;
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ਸਥਿਰ ਖਾਲੀ
ch_ਵਿਅੰਜਕ_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ eval)
{
    char_u	*ਪਾਠ;
    typval_T	*listtv;
    channel_T	*channel;
    ਪੂਰਨ		id;
    ch_ਬਹੁਲਕ_T	ch_ਬਹੁਲਕ;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    ਪੂਰਨ		timeout;
    ਪੂਰਨ		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ;
    part_send = channel_part_send(channel);

    ch_ਬਹੁਲਕ = channel_get_ਬਹੁਲਕ(channel, part_send);
    ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_RAW || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_BLOB
	    || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ਵਾਪਸ;
    }

    ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	ਅੱਖਰ		*ਕੁੰਜੀ = ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP ? "id" : "seq";

	// return an empty dict by default
	ਜੇ (rettv_dict_alloc(rettv) == FAIL)
	    ਵਾਪਸ;

	ਜੇ (check_for_dict_arg(argvars, 1) == FAIL)
	    ਵਾਪਸ;

	d = argvars[1].vval.v_dict;
	di = dict_ਲੱਭੋ(d, (char_u *)ਕੁੰਜੀ, -1);
	ਜੇ (argvars[2].v_type == VAR_DICT
		&& dict_has_ਕੁੰਜੀ(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type != VAR_ਸੰਖਿਆ
		&& (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_ਮੁੱਲ_for_ਦਲੀਲ_str), ਕੁੰਜੀ);
	    ਵਾਪਸ;
	}

	ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_ਆਖਰੀ_ਸੁਨੇਹਾ_id;
	    ਜੇ (di == ਨਿਰਾ)
		dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "seq", id);
	    ਨਹੀਂਤਾਂ
		di->di_tv.vval.v_ਸੰਖਿਆ = id;
	}
	ਨਹੀਂਤਾਂ ਜੇ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_ਆਖਰੀ_ਸੁਨੇਹਾ_id;
	    ਜੇ (di == ਨਿਰਾ)
		dict_ਜੋੜੋ_ਸੰਖਿਆ(d, "id", id);
	    ਨਹੀਂਤਾਂ
		di->di_tv.vval.v_ਸੰਖਿਆ = id;
	}
	ਨਹੀਂਤਾਂ
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ਜੇ (di != ਨਿਰਾ && di->di_tv.v_type == VAR_ਸੰਖਿਆ)
		id = di->di_tv.vval.v_ਸੰਖਿਆ;
	}
	ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP && !dict_has_ਕੁੰਜੀ(d, "jsonrpc"))
	    dict_ਜੋੜੋ_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	ਪਾਠ = json_encode_lsp_ਸੁਨੇਹਾ(&argvars[1]);
    }
    ਨਹੀਂਤਾਂ
    {
	id = ++channel->ch_ਆਖਰੀ_ਸੁਨੇਹਾ_id;
	ਪਾਠ = json_encode_nr_ਵਿਅੰਜਕ(id, &argvars[1],
			      (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_JS ? JSON_JS : 0) | JSON_NL);
    }
    ਜੇ (ਪਾਠ == ਨਿਰਾ)
	ਵਾਪਸ;

    channel = send_common(argvars, ਪਾਠ, (ਪੂਰਨ)STRLEN(ਪਾਠ), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(ਪਾਠ);
    ਜੇ (channel != ਨਿਰਾ && eval)
    {
	ਜੇ (opt.jo_ਸੈੱਟ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ਨਹੀਂਤਾਂ
	    timeout = channel_get_timeout(channel, part_read);
	ਜੇ (channel_read_json_ਖੰਡ(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP || ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ਸੰਖਿਆ;
		free_tv(listtv);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_ਆਖਰੀ->li_tv;
		list->lv_u.mat.lv_ਆਖਰੀ->li_tv.v_type = VAR_ਸੰਖਿਆ;
		free_tv(listtv);
	    }
	}
    }
    free_ਕੰਮ_options(&opt);
    ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ਜੇ (rettv->vval.v_dict != ਨਿਰਾ)
	    dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "seq", id);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ch_ਬਹੁਲਕ == CH_ਬਹੁਲਕ_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ਜੇ (rettv->vval.v_dict != ਨਿਰਾ)
	    dict_ਜੋੜੋ_ਸੰਖਿਆ(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ਸਥਿਰ ਖਾਲੀ
ch_raw_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*ਪਾਠ;
    ਪੂਰਨ		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    ਪੂਰਨ		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type == VAR_BLOB)
    {
	ਪਾਠ = argvars[1].vval.v_blob->bv_ga.ga_ਅੰਕੜੇ;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    ਨਹੀਂਤਾਂ
    {
	ਪਾਠ = tv_get_string_buf(&argvars[1], buf);
	len = (ਪੂਰਨ)STRLEN(ਪਾਠ);
    }
    channel = send_common(argvars, ਪਾਠ, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ਜੇ (channel != ਨਿਰਾ && eval)
    {
	ਜੇ (opt.jo_ਸੈੱਟ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ਨਹੀਂਤਾਂ
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_ਖੰਡ(channel, part_read,
							timeout, TRUE, ਨਿਰਾ);
    }
    free_ਕੰਮ_options(&opt);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ KEEP_OPEN_ਸਮਾਂ 20  // msec

#ਜੇ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ਪੂਰਨ
channel_poll_ਤਿਆਰੀ(ਪੂਰਨ nfd_ਵਿੱਚ, ਖਾਲੀ *fds_ਵਿੱਚ, ਪੂਰਨ *towait)
{
    ਪੂਰਨ		nfd = nfd_ਵਿੱਚ;
    channel_T	*channel;
    ਬਣਤਰ	pollfd *fds = fds_ਵਿੱਚ;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ਜੇ (ch_part->ch_fd != INVALID_FD)
	    {
		ਜੇ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ਜੇ (*towait < 0 || *towait > KEEP_OPEN_ਸਮਾਂ)
			*towait = KEEP_OPEN_ਸਮਾਂ;
		}
		ਨਹੀਂਤਾਂ
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    ਨਹੀਂਤਾਂ
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    ਵਾਪਸ nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ਪੂਰਨ
channel_poll_check(ਪੂਰਨ ret_ਵਿੱਚ, ਖਾਲੀ *fds_ਵਿੱਚ)
{
    ਪੂਰਨ		ret = ret_ਵਿੱਚ;
    channel_T	*channel;
    ਬਣਤਰ	pollfd *fds = fds_ਵਿੱਚ;
    ch_part_T	part;
    ਪੂਰਨ		idx;
    chanpart_T	*ਵਿੱਚ_part;

    FOR_ALL_CHANNELS(channel)
    {
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ਜੇ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	ਵਿੱਚ_part = &channel->ch_part[PART_ਵਿੱਚ];
	idx = ਵਿੱਚ_part->ch_poll_idx;
	ਜੇ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_ਨਿਵੇਸ਼(channel);
	    --ret;
	}
    }

    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ // UNIX && !HAVE_SELECT

#ਜੇ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ਪੂਰਨ
channel_select_ਤਿਆਰੀ(
	ਪੂਰਨ maxfd_ਵਿੱਚ,
	ਖਾਲੀ *rfds_ਵਿੱਚ,
	ਖਾਲੀ *wfds_ਵਿੱਚ,
	ਬਣਤਰ timeval *tv,
	ਬਣਤਰ timeval **tvp)
{
    ਪੂਰਨ		maxfd = maxfd_ਵਿੱਚ;
    channel_T	*channel;
    fd_set	*rfds = rfds_ਵਿੱਚ;
    fd_set	*wfds = wfds_ਵਿੱਚ;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ਜੇ (fd != INVALID_FD)
	    {
		ਜੇ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ਜੇ (*tvp == ਨਿਰਾ || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_ਸਮਾਂ * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_ਸਮਾਂ * 1000;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ਜੇ ((ਪੂਰਨ)fd >= FD_SETSIZE)
			ਜਾਰੀ;
# endif
		    FD_SET((ਪੂਰਨ)fd, rfds);
		    ਜੇ (maxfd < (ਪੂਰਨ)fd)
			maxfd = (ਪੂਰਨ)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ਵਾਪਸ maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ਪੂਰਨ
channel_select_check(ਪੂਰਨ ret_ਵਿੱਚ, ਖਾਲੀ *rfds_ਵਿੱਚ, ਖਾਲੀ *wfds_ਵਿੱਚ)
{
    ਪੂਰਨ		ret = ret_ਵਿੱਚ;
    channel_T	*channel;
    fd_set	*rfds = rfds_ਵਿੱਚ;
    fd_set	*wfds = wfds_ਵਿੱਚ;
    ch_part_T	part;
    chanpart_T	*ਵਿੱਚ_part;

    FOR_ALL_CHANNELS(channel)
    {
	ਲਈ (part = PART_SOCK; part < PART_ਵਿੱਚ; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ਜੇ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ਪੂਰਨ)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	ਵਿੱਚ_part = &channel->ch_part[PART_ਵਿੱਚ];
	ਜੇ (ret > 0 && ਵਿੱਚ_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ਪੂਰਨ)ਵਿੱਚ_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(ਵਿੱਚ_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(ਵਿੱਚ_part->ch_fd, wfds);
	    channel_write_ਨਿਵੇਸ਼(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ਜੇ (channel->ch_ਕੰਮ != ਨਿਰਾ)
	{
	    ਕੰਮ_T *ਕੰਮ = channel->ch_ਕੰਮ;

	    ਜੇ (ਕੰਮ->jv_tty_ਬਾਹਰਲਾ != ਨਿਰਾ && ਕੰਮ->jv_ਸਥਿਤੀ == ਕੰਮ_ਮੁਕੰਮਲ)
		ਲਈ (part = PART_SOCK; part < PART_ਗਿਣਤੀ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ਵਾਪਸ ret;
}
#ਜੇ_ਅੰਤ // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ਸਥਿਰ ਖਾਲੀ
channel_wait_any(ਲੰਮਾ msec)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    ਜੇ (stdio_channel->CH_ਬਾਹਰਲਾ_FD != INVALID_FD)
	(ਖਾਲੀ)channel_wait(stdio_channel, stdio_channel->CH_ਬਾਹਰਲਾ_FD,
						(ਪੂਰਨ)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#ਜਾਂਜੇ defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    ਬਣਤਰ timeval	tv;
    ਬਣਤਰ timeval	*tvp = &tv;
    ਪੂਰਨ			maxfd;
    ਪੂਰਨ			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_ਤਿਆਰੀ(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ਨਿਰਾ, tvp);
    ਜੇ (ret >= 0)
	(ਖਾਲੀ)channel_select_check(ret, &rfds, &wfds);
#ਨਹੀਂਤਾਂ
    ਬਣਤਰ pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    ਪੂਰਨ			nfd;
    ਪੂਰਨ			towait = (ਪੂਰਨ)msec;
    ਪੂਰਨ			ret;

    nfd = channel_poll_ਤਿਆਰੀ(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ਜੇ (ret >= 0)
	(ਖਾਲੀ)channel_poll_check(ret, fds);
#ਜੇ_ਅੰਤ
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    ਖਾਲੀ
channel_stdio_loop(ਖਾਲੀ)
{
    ਜਦੋਂ (stdio_channel != ਨਿਰਾ && channel_is_open(stdio_channel))
    {
	ਲੰਮਾ	msec = 1000;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
	ਲੰਮਾ	due_ਸਮਾਂ = check_due_timer();

	ਜੇ (due_ਸਮਾਂ > 0 && due_ਸਮਾਂ < msec)
	    msec = due_ਸਮਾਂ;
#ਜੇ_ਅੰਤ
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
    ਪੂਰਨ
channel_parse_messages(ਖਾਲੀ)
{
    channel_T	*channel = ਪਹਿਲਾ_channel;
    ਪੂਰਨ		ret = FALSE;
    ਪੂਰਨ		r;
    ch_part_T	part = PART_SOCK;
    ਸਥਿਰ ਪੂਰਨ	recursive = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    elapsed_T	ਸ਼ੁਰੂ_tv;
#ਜੇ_ਅੰਤ

    // The code below may invoke callbacks, which might call us back.
    // In a recursive call channels will not be closed.
    ++recursive;
    ++ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
#ਜੇ_ਅੰਤ

    // Only do this message when another message was given, otherwise we get
    // lots of them.
    ਜੇ ((did_repeated_ਸੁਨੇਹਾ & REPEATED_ਸੁਨੇਹਾ_LOOKING) == 0)
    {
	ch_log(ਨਿਰਾ, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_ਸੁਨੇਹਾ = REPEATED_ਸੁਨੇਹਾ_LOOKING;
    }
    ਜਦੋਂ (channel != ਨਿਰਾ)
    {
	ਜੇ (recursive == 1)
	{
	    ਜੇ (channel_can_close(channel))
	    {
		channel->ch_ਮੰਜ਼ਿਲ_be_closed = (1U << PART_ਗਿਣਤੀ);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = ਪਹਿਲਾ_channel;
		ਜਾਰੀ;
	    }
	    ਜੇ (channel->ch_ਮੰਜ਼ਿਲ_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ਜੇ (channel->ch_ਕੰਮ != ਨਿਰਾ)
		    channel->ch_ਕੰਮ->jv_channel = ਨਿਰਾ;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = ਪਹਿਲਾ_channel;
		ਜਾਰੀ;
	    }
	    ਜੇ (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = ਪਹਿਲਾ_channel;
		part = PART_SOCK;
		ਜਾਰੀ;
	    }
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	ਜੇ (part == PART_ਬਾਹਰਲਾ
		&& channel->ch_ਕੰਮ != ਨਿਰਾ
		&& channel->ch_ਕੰਮ->jv_tty_ਬਾਹਰਲਾ != ਨਿਰਾ
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    ਜਾਰੀ;
	}
#ਜੇ_ਅੰਤ
	ਜੇ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ਜੇ (r == OK)
		ret = TRUE;
	    ਜੇ (channel_unref(channel) || (r == OK
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
			// Limit the time we loop here to 100 msec, otherwise
			// Vim becomes unresponsive when the callback takes
			// more than a bit of time.
			&& ELAPSED_FUNC(ਸ਼ੁਰੂ_tv) < 100L
#ਜੇ_ਅੰਤ
			))
	    {
		// channel was freed or something was done, start over
		channel = ਪਹਿਲਾ_channel;
		part = PART_SOCK;
		ਜਾਰੀ;
	    }
	}
	ਜੇ (part < PART_ERR)
	    ++part;
	ਨਹੀਂਤਾਂ
	{
	    channel = channel->ch_ਅਗਲਾ;
	    part = PART_SOCK;
	}
    }

    ਜੇ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --ਸੁਰੱਖਿਅਤ_ਮੰਜ਼ਿਲ_invoke_callback;
    --recursive;

    ਵਾਪਸ ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    ਪੂਰਨ
channel_any_readahead(ਖਾਲੀ)
{
    channel_T	*channel = ਪਹਿਲਾ_channel;
    ch_part_T	part = PART_SOCK;

    ਜਦੋਂ (channel != ਨਿਰਾ)
    {
	ਜੇ (channel_has_readahead(channel, part))
	    ਵਾਪਸ TRUE;
	ਜੇ (part < PART_ERR)
	    ++part;
	ਨਹੀਂਤਾਂ
	{
	    channel = channel->ch_ਅਗਲਾ;
	    part = PART_SOCK;
	}
    }
    ਵਾਪਸ FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_channel(ਪੂਰਨ copyID)
{
    ਪੂਰਨ		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    ਲਈ (channel = ਪਹਿਲਾ_channel; !abort && channel != ਨਿਰਾ;
						   channel = channel->ch_ਅਗਲਾ)
	ਜੇ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || ਸੈੱਟ_ref_ਵਿੱਚ_ਵਸਤੂ(&tv, copyID, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
	}
    ਵਾਪਸ abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ਸਥਿਰ ch_part_T
channel_part_send(channel_T *channel)
{
    ਜੇ (channel->CH_SOCK_FD == INVALID_FD)
	ਵਾਪਸ PART_ਵਿੱਚ;
    ਵਾਪਸ PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ਸਥਿਰ ch_part_T
channel_part_read(channel_T *channel)
{
    ਜੇ (channel->CH_SOCK_FD == INVALID_FD)
	ਵਾਪਸ PART_ਬਾਹਰਲਾ;
    ਵਾਪਸ PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ਸਥਿਰ ch_ਬਹੁਲਕ_T
channel_get_ਬਹੁਲਕ(channel_T *channel, ch_part_T part)
{
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ CH_ਬਹੁਲਕ_JSON;
    ਵਾਪਸ channel->ch_part[part].ch_ਬਹੁਲਕ;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ਸਥਿਰ ਪੂਰਨ
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ਵਾਪਸ channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    ਖਾਲੀ
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_ਸੰਖਿਆ = 0;
    ਜੇ (ਵਿੱਚ_vim9script() && check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ਜੇ (channel != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_ਬਾਹਰਲਾ)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    ਖਾਲੀ
f_ch_close(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    channel_T *channel;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ਜੇ (channel != ਨਿਰਾ)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    ਖਾਲੀ
f_ch_close_ਵਿੱਚ(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    channel_T *channel;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ਜੇ (channel != ਨਿਰਾ)
	channel_close_ਵਿੱਚ(channel);
}

/*
 * "ch_getbufnr()" function
 */
    ਖਾਲੀ
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_ਸੰਖਿਆ = -1;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ;

    char_u	*what = tv_get_string(&argvars[1]);
    ਪੂਰਨ		part;
    ਜੇ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "out") == 0)
	part = PART_ਬਾਹਰਲਾ;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(what, "in") == 0)
	part = PART_ਵਿੱਚ;
    ਨਹੀਂਤਾਂ
	part = PART_SOCK;
    ਜੇ (channel->ch_part[part].ch_bufref.br_buf != ਨਿਰਾ)
	rettv->vval.v_ਸੰਖਿਆ =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    ਖਾਲੀ
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ;

    rettv->v_type = VAR_ਕੰਮ;
    rettv->vval.v_ਕੰਮ = channel->ch_ਕੰਮ;
    ਜੇ (channel->ch_ਕੰਮ != ਨਿਰਾ)
	++channel->ch_ਕੰਮ->jv_refcount;
}

/*
 * "ch_info()" function
 */
    ਖਾਲੀ
f_ch_info(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    channel_T *channel;

    ਜੇ (ਵਿੱਚ_vim9script() && check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ਜੇ (channel != ਨਿਰਾ && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    ਖਾਲੀ
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    ਖਾਲੀ
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    ਖਾਲੀ
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    ਖਾਲੀ
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    ਖਾਲੀ
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    ਖਾਲੀ
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ਵਿਅੰਜਕ_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    ਖਾਲੀ
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ਵਿਅੰਜਕ_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    ਖਾਲੀ
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    ਖਾਲੀ
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    ਖਾਲੀ
f_ch_setoptions(typval_T *argvars, typval_T *rettv ਅਣਵਰਤਿਆ)
{
    channel_T	*channel;
    jobopt_T	opt;

    ਜੇ (check_restricted() || check_secure())
	ਵਾਪਸ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ਜੇ (channel == ਨਿਰਾ)
	ਵਾਪਸ;
    clear_ਕੰਮ_options(&opt);
    ਜੇ (get_ਕੰਮ_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_ਬਹੁਲਕ_ALL, 0) == OK)
	channel_ਸੈੱਟ_options(channel, &opt);
    free_ਕੰਮ_options(&opt);
}

/*
 * "ch_status()" function
 */
    ਖਾਲੀ
f_ch_ਸਥਿਤੀ(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    ਪੂਰਨ		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_chan_or_ਕੰਮ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_ਕੰਮ_options(&opt);
	ਜੇ (get_ਕੰਮ_options(&argvars[1], &opt, JO_PART, 0) == OK
						     && (opt.jo_ਸੈੱਟ & JO_PART))
	    part = opt.jo_part;
    }

    rettv->vval.v_string = vim_strsave((char_u *)channel_ਸਥਿਤੀ(channel, part));
}

/*
 * Get a string with information about the channel in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
channel_ਮੰਜ਼ਿਲ_string_buf(typval_T *varp, char_u *buf)
{
    channel_T *channel = varp->vval.v_channel;
    ਅੱਖਰ      *ਸਥਿਤੀ = channel_ਸਥਿਤੀ(channel, -1);

    ਜੇ (channel == ਨਿਰਾ)
	vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN, "channel %s", ਸਥਿਤੀ);
    ਨਹੀਂਤਾਂ
	vim_snprintf((ਅੱਖਰ *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, ਸਥਿਤੀ);
    ਵਾਪਸ buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_ਲੱਭੋ(ਪੂਰਨ ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ਜੇ (ch->ch_id == ch_id)
	    ਵਾਪਸ ch;
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ // FEAT_JOB_CHANNEL
