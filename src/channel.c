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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)

// TRUE when netbeans is running with a GUI.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# define CH_HAS_GUI (gui.ಒಳ_use || gui.starting)
#ಆದರೆ_ಅಂತ್ಯ

// Note: when making changes here also adjust configure.ac.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
// WinSock API is separated from C API, thus we can't use read(), write(),
// errno...
# define SOCK_ERRNO errno = WSAGet_ಕೊನೆಯ_Error()
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
# define UNIX_ಮಾರ್ಗ_MAX 108
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ sockaddr_un {
    ವಿಳಾಸ_FAMILY sun_family;
    ಅಕ್ಷರ sun_ಮಾರ್ಗ[UNIX_ಮಾರ್ಗ_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ಇಲ್ಲದಿದ್ದರೆ
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) ಲಿಖ(sd, buf, len)
# define sock_read(sd, buf, len) ಪಠ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) ಪಠ(fd, buf, len)
# define fd_write(sd, buf, len) ಲಿಖ(sd, buf, len)
# define fd_close(sd) close(sd)
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ channel_read(channel_T *channel, ch_part_T part, ಅಕ್ಷರ *func);
ಸ್ಥಿರ ಶೂನ್ಯ channel_wait_any(ದೀರ್ಘ msec);
ಸ್ಥಿರ ch_ಬಹುಲಕ_T channel_get_ಬಹುಲಕ(channel_T *channel, ch_part_T part);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ channel_get_timeout(channel_T *channel, ch_part_T part);
ಸ್ಥಿರ channel_T *channel_open_stdio(ಶೂನ್ಯ);
ಸ್ಥಿರ ch_part_T channel_part_send(channel_T *channel);
ಸ್ಥಿರ ch_part_T channel_part_read(channel_T *channel);

#ವ್ಯಾಖ್ಯಾನಿಸು FOR_ALL_CHANNELS(ch) \
    ಗಾಗಿ ((ch) = ಮೊದಲ_channel; (ch) != ಖಾಲಿ; (ch) = (ch)->ch_ಮುಂದಿನ)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fd_read(sock_T fd, ಅಕ್ಷರ *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ಆದರೆ (!ReadFile(h, buf, (DWORD)len, &nread, ಖಾಲಿ))
	ಹಿಂತಿರುಗಿ -1;
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)nread;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
fd_write(sock_T fd, ಅಕ್ಷರ *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, ಗಾತ್ರ_2, ಮುಗಿದ = 0;
    OVERLAPPED	ov;

    ಆಗ (todo > 0)
    {
	ಆದರೆ (todo > MAX_NAMED_PIPE_ಗಾತ್ರ)
	    ಗಾತ್ರ_2 = MAX_NAMED_PIPE_ಗಾತ್ರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಾತ್ರ_2 = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_ಕ್ಷೇತ್ರ(ov);
	nwrite = 0;
	ಆದರೆ (!WriteFile(h, buf + ಮುಗಿದ, ಗಾತ್ರ_2, &nwrite, &ov))
	{
	    DWORD err = Get_ಕೊನೆಯ_Error();

	    ಆದರೆ (err != ERROR_IO_PENDING)
		ಹಿಂತಿರುಗಿ -1;
	    ಆದರೆ (!Get_Overlapped_ಫಲಿತಾಂಶ(h, &ov, &nwrite, FALSE))
		ಹಿಂತಿರುಗಿ -1;
	    FlushFileBuffers(h);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    ನಿಲ್ಲಿಸು;
	todo -= nwrite;
	ಮುಗಿದ += nwrite;
    }
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)ಮುಗಿದ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
# undef PERROR
# define PERROR(ಸಂದೇಶ) (ಶೂನ್ಯ)semsg("%s: %s", ಸಂದೇಶ, strerror_win32(errno))

    ಸ್ಥಿರ ಅಕ್ಷರ *
strerror_win32(ಪೂರ್ಣಾಂಕ eno)
{
    ಸ್ಥಿರ LPVOID msgbuf = ಖಾಲಿ;
    char_u *ptr;

    ಆದರೆ (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ಖಾಲಿ;
    }
    Format_ಸಂದೇಶ(
	FORMAT_ಸಂದೇಶ_ALLOCATE_ಬಫರ್ |
	FORMAT_ಸಂದೇಶ_FROM_SYSTEM |
	FORMAT_ಸಂದೇಶ_IGNORE_INSERTS,
	ಖಾಲಿ,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ಖಾಲಿ);
    ಆದರೆ (msgbuf != ಖಾಲಿ)
	// chomp \r or \n
	ಗಾಗಿ (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    ಆಯ್ಕೆ (*ptr)
	    {
		ಸ್ಥಿತಿ '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '\n':
		    ಆದರೆ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    ಇಲ್ಲದಿದ್ದರೆ
			*ptr = ' ';
		    ನಿಲ್ಲಿಸು;
	    }
    ಹಿಂತಿರುಗಿ msgbuf;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The list of all allocated channels.
 */
ಸ್ಥಿರ channel_T *ಮೊದಲ_channel = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮುಂದಿನ_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
ಕೂಡಿಸು_channel(ಶೂನ್ಯ)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    channel->ch_id = ಮುಂದಿನ_ch_id++;
    ch_log(channel, "Created channel");

    ಗಾಗಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_X11
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = (Xt_ಒಳಹರಿವು_Id)ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = 0;
#ಆದರೆ_ಅಂತ್ಯ
	channel->ch_part[part].ch_timeout = 2000;
    }

    ಆದರೆ (ಮೊದಲ_channel != ಖಾಲಿ)
    {
	ಮೊದಲ_channel->ch_prev = channel;
	channel->ch_ಮುಂದಿನ = ಮೊದಲ_channel;
    }
    ಮೊದಲ_channel = channel;

    channel->ch_refcount = 1;
    ಹಿಂತಿರುಗಿ channel;
}

    ಪೂರ್ಣಾಂಕ
has_any_channel(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ ಮೊದಲ_channel != ಖಾಲಿ;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    ಪೂರ್ಣಾಂಕ
channel_still_useful(channel_T *channel)
{
    ಪೂರ್ಣಾಂಕ has_sock_ಸಂದೇಶ;
    ಪೂರ್ಣಾಂಕ	has_ಹೊರಹೋಗು_ಸಂದೇಶ;
    ಪೂರ್ಣಾಂಕ	has_err_ಸಂದೇಶ;

    // If the job was killed the channel is not expected to work anymore.
    ಆದರೆ (channel->ch_ಕೆಲಸ_killed && channel->ch_ಕೆಲಸ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    // If there is a close callback it may still need to be invoked.
    ಆದರೆ (channel->ch_close_cb.cb_name != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    // If reading from or a buffer it's still useful.
    ಆದರೆ (channel->ch_part[PART_ಒಳ].ch_bufref.br_buf != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_ಸಂದೇಶ = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_ತಲೆ.rq_ಮುಂದಿನ != ಖಾಲಿ
		|| channel->ch_part[PART_SOCK].ch_json_ತಲೆ.jq_ಮುಂದಿನ != ಖಾಲಿ;
    has_ಹೊರಹೋಗು_ಸಂದೇಶ = channel->ch_part[PART_ಹೊರಹೋಗು].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ಹೊರಹೋಗು].ch_ತಲೆ.rq_ಮುಂದಿನ != ಖಾಲಿ
		  || channel->ch_part[PART_ಹೊರಹೋಗು].ch_json_ತಲೆ.jq_ಮುಂದಿನ != ಖಾಲಿ;
    has_err_ಸಂದೇಶ = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_ತಲೆ.rq_ಮುಂದಿನ != ಖಾಲಿ
		  || channel->ch_part[PART_ERR].ch_json_ತಲೆ.jq_ಮುಂದಿನ != ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ (channel->ch_callback.cb_name != ಖಾಲಿ && (has_sock_ಸಂದೇಶ
		|| has_ಹೊರಹೋಗು_ಸಂದೇಶ || has_err_ಸಂದೇಶ))
	    || ((channel->ch_part[PART_ಹೊರಹೋಗು].ch_callback.cb_name != ಖಾಲಿ
		       || channel->ch_part[PART_ಹೊರಹೋಗು].ch_bufref.br_buf != ಖಾಲಿ)
		    && has_ಹೊರಹೋಗು_ಸಂದೇಶ)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ಖಾಲಿ
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ಖಾಲಿ)
		    && has_err_ಸಂದೇಶ);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    ಪೂರ್ಣಾಂಕ
channel_can_close(channel_T *channel)
{
    ಹಿಂತಿರುಗಿ channel->ch_ಗಮ್ಯ_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_free_channel(channel_T *channel)
{
    ಆದರೆ (channel->ch_ಮುಂದಿನ != ಖಾಲಿ)
	channel->ch_ಮುಂದಿನ->ch_prev = channel->ch_prev;
    ಆದರೆ (channel->ch_prev == ಖಾಲಿ)
	ಮೊದಲ_channel = channel->ch_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	channel->ch_prev->ch_ಮುಂದಿನ = channel->ch_ಮುಂದಿನ;
    vim_free(channel);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_free(channel_T *channel)
{
    ಆದರೆ (ಒಳ_free_unref_ವಸ್ತುಗಳು)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback == 0)
	channel->ch_ಗಮ್ಯ_be_freed = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_may_free(channel_T *channel)
{
    ಆದರೆ (!channel_still_useful(channel))
    {
	channel_free(channel);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    ಪೂರ್ಣಾಂಕ
channel_unref(channel_T *channel)
{
    ಆದರೆ (channel != ಖಾಲಿ && --channel->ch_refcount <= 0)
	ಹಿಂತಿರುಗಿ channel_may_free(channel);
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
free_ಬಳಸದ_channels_contents(ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ)
{
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ಆದರೆ (!channel_still_useful(ch)
				 && (ch->ch_copyID & ಮುಖವಾಡ) != (copyID & ಮುಖವಾಡ))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback;
    ಹಿಂತಿರುಗಿ did_free;
}

    ಶೂನ್ಯ
free_ಬಳಸದ_channels(ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ)
{
    channel_T	*ch;
    channel_T	*ch_ಮುಂದಿನ;

    ಗಾಗಿ (ch = ಮೊದಲ_channel; ch != ಖಾಲಿ; ch = ch_ಮುಂದಿನ)
    {
	ch_ಮುಂದಿನ = ch->ch_ಮುಂದಿನ;
	ಆದರೆ (!channel_still_useful(ch)
				 && (ch->ch_copyID & ಮುಖವಾಡ) != (copyID & ಮುಖವಾಡ))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ಆದರೆ defined(FEAT_GUI)

# ಆದರೆ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    ಸ್ಥಿರ channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ಆದರೆ (fd == INVALID_FD)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    FOR_ALL_CHANNELS(channel)
    {
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	    ಆದರೆ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ಹಿಂತಿರುಗಿ channel;
	    }
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_read_fd(ಪೂರ್ಣಾಂಕ fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ಆದರೆ (channel == ಖಾಲಿ)
	ch_error(ಖಾಲಿ, "Channel for fd %d not found", fd);
    ಇಲ್ಲದಿದ್ದರೆ
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_From_Server_X11(Xt_ಸೂಚಕ client_ದತ್ತಾಂಶ,
		  ಪೂರ್ಣಾಂಕ *unused1 ಬಳಸದ,
		  Xt_ಒಳಹರಿವು_Id *unused2 ಬಳಸದ)
{
    channel_read_fd((ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)client_ದತ್ತಾಂಶ);
}
# endif

# ifdef FEAT_GUI_GTK
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ gboolean
ಸಂದೇಶ_From_Server_Gtk3(GIOChannel *unused1 ಬಳಸದ,
		  GIOCondition unused2 ಬಳಸದ,
		  gpointer client_ದತ್ತಾಂಶ)
{
    channel_read_fd(GPOINTER_ಗಮ್ಯ_INT(client_ದತ್ತಾಂಶ));
    ಹಿಂತಿರುಗಿ TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಂದೇಶ_From_Server_Gtk2(gpointer client_ದತ್ತಾಂಶ,
		  gint unused1 ಬಳಸದ,
		  Gdk_ಒಳಹರಿವು_Condition unused2 ಬಳಸದ)
{
    channel_read_fd((ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)client_ದತ್ತಾಂಶ);
}
#  endif
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_gui_ರಿಜಿಸ್ಟರ್_one(channel_T *channel, ch_part_T part ಬಳಸದ)
{
    ಆದರೆ (!CH_HAS_GUI)
	ಹಿಂತಿರುಗಿ;

    // gets stuck in handling events for a not connected channel
    ಆದರೆ (channel->ch_keep_open)
	ಹಿಂತಿರುಗಿ;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ಆದರೆ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler == (Xt_ಒಳಹರಿವು_Id)ಖಾಲಿ)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_ಹೆಸರುಗಳು[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = Xt_App_ಕೂಡಿಸು_ಒಳಹರಿವು(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(Xt_ಸೂಚಕ)(Xt_ಒಳಹರಿವು_Read_ಮುಖವಾಡ + Xt_ಒಳಹರಿವು_Except_ಮುಖವಾಡ),
		ಸಂದೇಶ_From_Server_X11,
		(Xt_ಸೂಚಕ)(ದೀರ್ಘ)channel->ch_part[part].ch_fd);
    }
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ಆದರೆ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_ಹೆಸರುಗಳು[part], channel->ch_part[part].ch_fd);
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = g_io_ಕೂಡಿಸು_watch(
		chnnl,
		G_IO_ಒಳ|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		ಸಂದೇಶ_From_Server_Gtk3,
		GINT_ಗಮ್ಯ_ಸೂಚಕ(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   ಇಲ್ಲದಿದ್ದರೆ
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = gdk_ಒಳಹರಿವು_ಕೂಡಿಸು(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_ಒಳಹರಿವು_Condition)
			     ((ಪೂರ್ಣಾಂಕ)GDK_ಒಳಹರಿವು_READ + (ಪೂರ್ಣಾಂಕ)GDK_ಒಳಹರಿವು_EXCEPTION),
		ಸಂದೇಶ_From_Server_Gtk2,
		(gpointer)(ದೀರ್ಘ)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_gui_ರಿಜಿಸ್ಟರ್(channel_T *channel)
{
    ಆದರೆ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_SOCK);
    ಆದರೆ (channel->CH_ಹೊರಹೋಗು_FD != INVALID_FD
	    && channel->CH_ಹೊರಹೋಗು_FD != channel->CH_SOCK_FD)
	channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_ಹೊರಹೋಗು);
    ಆದರೆ (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_ಹೊರಹೋಗು_FD)
	channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    ಶೂನ್ಯ
channel_gui_ರಿಜಿಸ್ಟರ್_all(ಶೂನ್ಯ)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_ರಿಜಿಸ್ಟರ್(channel);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_gui_unregister_one(channel_T *channel ಬಳಸದ, ch_part_T part ಬಳಸದ)
{
# ifdef FEAT_GUI_X11
    ಆದರೆ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler != (Xt_ಒಳಹರಿವು_Id)ಖಾಲಿ)
    {
	ch_log(channel, "Unregistering part %s", ch_part_ಹೆಸರುಗಳು[part]);
	Xt_Remove_ಒಳಹರಿವು(channel->ch_part[part].ch_ಒಳಹರಿವು_Handler);
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = (Xt_ಒಳಹರಿವು_Id)ಖಾಲಿ;
    }
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef FEAT_GUI_GTK
    ಆದರೆ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_ಹೆಸರುಗಳು[part]);
#   ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	g_ಮೂಲ_remove(channel->ch_part[part].ch_ಒಳಹರಿವು_Handler);
#   ಇಲ್ಲದಿದ್ದರೆ
	gdk_ಒಳಹರಿವು_remove(channel->ch_part[part].ch_ಒಳಹರಿವು_Handler);
#   endif
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = 0;
    }
#  endif
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	channel_gui_unregister_one(channel, part);
}

#ಆದರೆ_ಅಂತ್ಯ  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_connect(
	channel_T *channel,
	ಸ್ಥಿರಾಂಕ ರಚನೆ sockaddr *server_addr,
	ಪೂರ್ಣಾಂಕ server_addrlen,
	ಪೂರ್ಣಾಂಕ *waittime)
{
    ಪೂರ್ಣಾಂಕ		sd = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    u_long	val = 1;
#ಆದರೆ_ಅಂತ್ಯ

    ಆಗ (TRUE)
    {
	ದೀರ್ಘ	elapsed_msec = 0;
	ಪೂರ್ಣಾಂಕ	waitnow;
	ಪೂರ್ಣಾಂಕ	ret;

	ಆದರೆ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ಆದರೆ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_ಒಳ_channel_connect));
	    ಹಿಂತಿರುಗಿ -1;
	}

	ಆದರೆ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#ಇಲ್ಲದಿದ್ದರೆ
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#ಆದರೆ_ಅಂತ್ಯ
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		ಹಿಂತಿರುಗಿ -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ಆದರೆ (ret == 0)
	    // The connection could be established.
	    ನಿಲ್ಲಿಸು;

	SOCK_ERRNO;
	ಆದರೆ (*waittime < 0 || (errno != EWOULDBLOCK
		&& errno != ECONNREFUSED
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EINPROGRESS
		&& errno != EINPROGRESS
#ಆದರೆ_ಅಂತ್ಯ
		))
	{
	    ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
	    PERROR(_(e_cannot_connect_ಗಮ್ಯ_ಪೋರ್ಟ್));
	    sock_close(sd);
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ಹಿಂತಿರುಗಿ -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    ರಚನೆ timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
	    ಪೂರ್ಣಾಂಕ			so_error = 0;
	    socklen_t		so_error_len = ಗಾತ್ರ(so_error);
	    ರಚನೆ timeval	ಆರಂಭ_tv;
	    ರಚನೆ timeval	ಕೊನೆ_tv;
#ಆದರೆ_ಅಂತ್ಯ
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
	    gettimeofday(&ಆರಂಭ_tv, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ಖಾಲಿ, &tv);
	    ಆದರೆ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_ಗಮ್ಯ_ಪೋರ್ಟ್));
		sock_close(sd);
		ಹಿಂತಿರುಗಿ -1;
	    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ಆದರೆ (FD_ISSET(sd, &wfds))
		ನಿಲ್ಲಿಸು;
	    elapsed_msec = waitnow;
	    ಆದರೆ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		ಮುಂದುವರಿ;
	    }
#ಇಲ್ಲದಿದ್ದರೆ
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ಆದರೆ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ಆದರೆ (ret < 0 || (so_error != 0
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
		    PERROR(_(e_cannot_connect_ಗಮ್ಯ_ಪೋರ್ಟ್));
		    sock_close(sd);
		    ಹಿಂತಿರುಗಿ -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ಹಿಂತಿರುಗಿ -1;
		}
	    }

	    ಆದರೆ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		ನಿಲ್ಲಿಸು;

	    gettimeofday(&ಕೊನೆ_tv, ಖಾಲಿ);
	    elapsed_msec = (ಕೊನೆ_tv.tv_sec - ಆರಂಭ_tv.tv_sec) * 1000
				 + (ಕೊನೆ_tv.tv_usec - ಆರಂಭ_tv.tv_usec) / 1000;
#ಆದರೆ_ಅಂತ್ಯ
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
	ಆದರೆ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ಆದರೆ (waitnow > 0)
	    {
		mch_delay((ದೀರ್ಘ)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ಆದರೆ (!got_int)
	    {
		ಆದರೆ (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		ಮುಂದುವರಿ;
	    }
	    // we were interrupted, behave as if timed out
	}
#ಆದರೆ_ಅಂತ್ಯ

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (*waittime >= 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)fcntl(sd, F_SETFL, 0);
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಹಿಂತಿರುಗಿ sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ))
{
    channel_T		*channel = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			sd = -1;
    size_t		ಮಾರ್ಗ_len = STRLEN(ಮಾರ್ಗ);
    ರಚನೆ sockaddr_un	server;
    size_t		server_len;
    ಪೂರ್ಣಾಂಕ			waittime = -1;

    ಆದರೆ (*ಮಾರ್ಗ == NUL || ಮಾರ್ಗ_len >= ಗಾತ್ರ(server.sun_ಮಾರ್ಗ))
    {
	semsg(_(e_invalid_ವಾದ_str), ಮಾರ್ಗ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    channel_init_winsock();
#ಆದರೆ_ಅಂತ್ಯ

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
    {
	ch_error(ಖಾಲಿ, "Cannot allocate channel.");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    CLEAR_ಕ್ಷೇತ್ರ(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_ಮಾರ್ಗ, ಮಾರ್ಗ, ಗಾತ್ರ(server.sun_ಮಾರ್ಗ) - 1);

    ch_log(channel, "Trying to connect to %s", ಮಾರ್ಗ);

    server_len = offsetof(ರಚನೆ sockaddr_un, sun_ಮಾರ್ಗ) + ಮಾರ್ಗ_len + 1;
    sd = channel_connect(channel, (ರಚನೆ sockaddr *)&server, (ಪೂರ್ಣಾಂಕ)server_len,
								   &waittime);

    ಆದರೆ (sd < 0)
    {
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ಅಕ್ಷರ *)vim_strsave((char_u *)ಮಾರ್ಗ);
    channel->ch_ಪೋರ್ಟ್ = 0;
    channel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_SOCK);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_SOCK);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ channel;
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
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
	ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್,
	ಪೂರ್ಣಾಂಕ waittime,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ))
{
    ಪೂರ್ಣಾಂಕ			sd = -1;
    channel_T		*channel = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IPV6
    ಪೂರ್ಣಾಂಕ			err;
    ರಚನೆ addrinfo	hints;
    ರಚನೆ addrinfo	*res = ಖಾಲಿ;
    ರಚನೆ addrinfo	*addr = ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
    ರಚನೆ sockaddr_ಒಳ	server;
    ರಚನೆ hostent	*host = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    channel_init_winsock();
#ಆದರೆ_ಅಂತ್ಯ

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
    {
	ch_error(ಖಾಲಿ, "Cannot allocate channel.");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IPV6
    CLEAR_ಕ್ಷೇತ್ರ(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ಆದರೆ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ಆದರೆ ((err = getaddrinfo(hostname, ಖಾಲಿ, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_ಒಳ_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Count the number of addresses for timeout distribution
    ಪೂರ್ಣಾಂಕ addr_ಎಣಿಕೆ = 0;
    ಗಾಗಿ (addr = res; addr != ಖಾಲಿ; addr = addr->ai_ಮುಂದಿನ)
	addr_ಎಣಿಕೆ++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ಆದರೆ (waittime == 0)
	waittime = 15;

    ಪೂರ್ಣಾಂಕ addr_ಸೂಚ್ಯಂಕ = 0;
    ಗಾಗಿ (addr = res; addr != ಖಾಲಿ; addr = addr->ai_ಮುಂದಿನ, addr_ಸೂಚ್ಯಂಕ++)
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ  *src = ಖಾಲಿ;
	ಅಕ್ಷರ	    buf[NUMBUFLEN];
# endif
	ಪೂರ್ಣಾಂಕ	    try_waittime;
	ಪೂರ್ಣಾಂಕ	    before_waittime;
	ಪೂರ್ಣಾಂಕ	    consumed;
	ಪೂರ್ಣಾಂಕ	    ಉಳಿದ_addrs;

	ಆದರೆ (addr->ai_family == AF_INET6)
	{
	    ರಚನೆ sockaddr_in6 *sai = (ರಚನೆ sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_ಪೋರ್ಟ್ = htons(ಪೋರ್ಟ್);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (addr->ai_family == AF_INET)
	{
	    ರಚನೆ sockaddr_ಒಳ *sai = (ರಚನೆ sockaddr_ಒಳ *)addr->ai_addr;

	    sai->sin_ಪೋರ್ಟ್ = htons(ಪೋರ್ಟ್);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ಆದರೆ (src != ಖಾಲಿ)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, ಗಾತ್ರ(buf));
	    ಆದರೆ (dst == ಖಾಲಿ)
		dst = hostname;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, ಪೋರ್ಟ್);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ಆದರೆ (addr->ai_ಮುಂದಿನ == ಖಾಲಿ)
	    try_waittime = waittime;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (addr_ಸೂಚ್ಯಂಕ == 0)
	{
	    ಆದರೆ (waittime > 500)
		try_waittime = 250;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (waittime > 30)
		try_waittime = waittime / 2;
	    ಇಲ್ಲದಿದ್ದರೆ
		try_waittime = waittime;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಉಳಿದ_addrs = addr_ಎಣಿಕೆ - addr_ಸೂಚ್ಯಂಕ;
	    try_waittime = waittime / ಉಳಿದ_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (ಪೂರ್ಣಾಂಕ)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ಆದರೆ (waittime < 0)
	    waittime = 0;

	ಆದರೆ (sd >= 0)
	    ನಿಲ್ಲಿಸು;

	// If we have no time left, stop trying
	ಆದರೆ (waittime <= 0 && addr->ai_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    ನಿಲ್ಲಿಸು;
	}
    }

    freeaddrinfo(res);
#ಇಲ್ಲದಿದ್ದರೆ
    CLEAR_ಕ್ಷೇತ್ರ(server);
    server.sin_family = AF_INET;
    server.sin_ಪೋರ್ಟ್ = htons(ಪೋರ್ಟ್);
    ಆದರೆ ((host = gethostbyname(hostname)) == ಖಾಲಿ)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_ಒಳ_channel_open));
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    {
	ಅಕ್ಷರ *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	ಸ್ಮರಣೆ_ನಕಲು(&p, &host->h_addr_list[0], ಗಾತ್ರ(p));
	ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)&server.sin_addr, p, host->h_ಉದ್ದಳತೆ);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, ಪೋರ್ಟ್);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    ಆದರೆ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (ರಚನೆ sockaddr *)&server, ಗಾತ್ರ(server),
								   &waittime);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (sd < 0)
    {
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ಅಕ್ಷರ *)vim_strsave((char_u *)hostname);
    channel->ch_ಪೋರ್ಟ್ = ಪೋರ್ಟ್;
    channel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_SOCK);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_SOCK);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ channel;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_ಗಣ_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ಆದರೆ (callback->cb_name != ಖಾಲಿ && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    ಇಲ್ಲದಿದ್ದರೆ
	cbp->cb_name = ಖಾಲಿ;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ಸ್ಥಿರ ಶೂನ್ಯ
prepare_ಬಫರ್(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (curbuf->b_ml.ml_mfp == ಖಾಲಿ)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ಸ್ಥಿರ buf_T *
channel_ಕಂಡುಹಿಡಿ_ಬಫರ್(char_u *name, ಪೂರ್ಣಾಂಕ err, ಪೂರ್ಣಾಂಕ ಸಂದೇಶ)
{
    buf_T *buf = ಖಾಲಿ;
    buf_T *save_curbuf = curbuf;

    ಆದರೆ (name != ಖಾಲಿ && *name != NUL)
    {
	buf = buflist_findname(name);
	ಆದರೆ (buf == ಖಾಲಿ)
	    buf = buflist_findname_exp(name);
    }

    ಆದರೆ (buf != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ buf;

    buf = buflist_new(name == ಖಾಲಿ || *name == NUL ? ಖಾಲಿ : name,
	    ಖಾಲಿ, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    prepare_ಬಫರ್(buf);

    curbuf = buf;
    ಆದರೆ (ಸಂದೇಶ)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(1, 0);
    curbuf = save_curbuf;

    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_ಗಣ_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ಆದರೆ (opt->jo_ಗಣ & JO_ಬಹುಲಕ)
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
	    channel->ch_part[part].ch_ಬಹುಲಕ = opt->jo_ಬಹುಲಕ;
    ಆದರೆ (opt->jo_ಗಣ & JO_ಒಳ_ಬಹುಲಕ)
	channel->ch_part[PART_ಒಳ].ch_ಬಹುಲಕ = opt->jo_ಒಳ_ಬಹುಲಕ;
    ಆದರೆ (opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_ಬಹುಲಕ)
	channel->ch_part[PART_ಹೊರಹೋಗು].ch_ಬಹುಲಕ = opt->jo_ಹೊರಹೋಗು_ಬಹುಲಕ;
    ಆದರೆ (opt->jo_ಗಣ & JO_ERR_ಬಹುಲಕ)
	channel->ch_part[PART_ERR].ch_ಬಹುಲಕ = opt->jo_err_ಬಹುಲಕ;
    channel->ch_nonblock = opt->jo_noblock;

    ಆದರೆ (opt->jo_ಗಣ & JO_TIMEOUT)
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ಆದರೆ (opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_TIMEOUT)
	channel->ch_part[PART_ಹೊರಹೋಗು].ch_timeout = opt->jo_ಹೊರಹೋಗು_timeout;
    ಆದರೆ (opt->jo_ಗಣ & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ಆದರೆ (opt->jo_ಗಣ & JO_ಖಂಡ_WRITE)
	channel->ch_part[PART_ಒಳ].ch_ಖಂಡ_write = 1;

    ಆದರೆ (opt->jo_ಗಣ & JO_CALLBACK)
	free_ಗಣ_callback(&channel->ch_callback, &opt->jo_callback);
    ಆದರೆ (opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_CALLBACK)
	free_ಗಣ_callback(&channel->ch_part[PART_ಹೊರಹೋಗು].ch_callback,
							      &opt->jo_ಹೊರಹೋಗು_cb);
    ಆದರೆ (opt->jo_ಗಣ & JO_ERR_CALLBACK)
	free_ಗಣ_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ಆದರೆ (opt->jo_ಗಣ & JO_CLOSE_CALLBACK)
	free_ಗಣ_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ಆದರೆ ((opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_IO) && opt->jo_io[PART_ಹೊರಹೋಗು] == JIO_ಬಫರ್)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ಆದರೆ (!(opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_ಬಹುಲಕ))
	    channel->ch_part[PART_ಹೊರಹೋಗು].ch_ಬಹುಲಕ = CH_ಬಹುಲಕ_NL;
	ಆದರೆ (opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ಹೊರಹೋಗು]);
	    ಆದರೆ (buf == ಖಾಲಿ)
		semsg(_(e_ಬಫರ್_nr_does_not_exist),
					       (ದೀರ್ಘ)opt->jo_io_buf[PART_ಹೊರಹೋಗು]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ ಸಂದೇಶ = TRUE;

	    ಆದರೆ (opt->jo_set2 & JO2_ಹೊರಹೋಗು_ಸಂದೇಶ)
		ಸಂದೇಶ = opt->jo_ಸಂದೇಶ[PART_ಹೊರಹೋಗು];
	    buf = channel_ಕಂಡುಹಿಡಿ_ಬಫರ್(opt->jo_io_name[PART_ಹೊರಹೋಗು], FALSE, ಸಂದೇಶ);
	}
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    ಆದರೆ (opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_MODIFIABLE)
		channel->ch_part[PART_ಹೊರಹೋಗು].ch_nomodifiable =
						!opt->jo_modifiable[PART_ಹೊರಹೋಗು];

	    ಆದರೆ (!buf->b_p_ma && !channel->ch_part[PART_ಹೊರಹೋಗು].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (ಅಕ್ಷರ *)buf->b_ffname);
		ಗಣ_bufref(&channel->ch_part[PART_ಹೊರಹೋಗು].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
		    prepare_ಬಫರ್(buf);
	    }
	}
    }

    ಆದರೆ ((opt->jo_ಗಣ & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_ಬಫರ್
	 || (opt->jo_io[PART_ERR] == JIO_ಹೊರಹೋಗು && (opt->jo_ಗಣ & JO_ಹೊರಹೋಗು_IO)
				       && opt->jo_io[PART_ಹೊರಹೋಗು] == JIO_ಬಫರ್)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ಆದರೆ (!(opt->jo_ಗಣ & JO_ERR_ಬಹುಲಕ))
	    channel->ch_part[PART_ERR].ch_ಬಹುಲಕ = CH_ಬಹುಲಕ_NL;
	ಆದರೆ (opt->jo_io[PART_ERR] == JIO_ಹೊರಹೋಗು)
	    buf = channel->ch_part[PART_ಹೊರಹೋಗು].ch_bufref.br_buf;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (opt->jo_ಗಣ & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ಆದರೆ (buf == ಖಾಲಿ)
		semsg(_(e_ಬಫರ್_nr_does_not_exist),
					       (ದೀರ್ಘ)opt->jo_io_buf[PART_ERR]);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ ಸಂದೇಶ = TRUE;

	    ಆದರೆ (opt->jo_set2 & JO2_ERR_ಸಂದೇಶ)
		ಸಂದೇಶ = opt->jo_ಸಂದೇಶ[PART_ERR];
	    buf = channel_ಕಂಡುಹಿಡಿ_ಬಫರ್(opt->jo_io_name[PART_ERR], TRUE, ಸಂದೇಶ);
	}
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    ಆದರೆ (opt->jo_ಗಣ & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ಆದರೆ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (ಅಕ್ಷರ *)buf->b_ffname);
		ಗಣ_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ಆದರೆ (buf->b_ml.ml_mfp == ಖಾಲಿ)
		    prepare_ಬಫರ್(buf);
	    }
	}
    }

    channel->ch_part[PART_ಹೊರಹೋಗು].ch_io = opt->jo_io[PART_ಹೊರಹೋಗು];
    channel->ch_part[PART_ERR].ch_io = opt->jo_io[PART_ERR];
    channel->ch_part[PART_ಒಳ].ch_io = opt->jo_io[PART_ಒಳ];
}

/*
 * Parse the address for socketserver and store port in "port", or the path in
 * "unix_path" if it is a unix domain socket. Returns OK on success and FAIL on
 * failure.
 */
    ಪೂರ್ಣಾಂಕ
channel_parse_socketserver_ವಿಳಾಸ(
	char_u	*ವಿಳಾಸ,
	ಪೂರ್ಣಾಂಕ	*ಪೋರ್ಟ್,
	char_u	**unix_ಮಾರ್ಗ,
	bool	quiet)
{
    ಅಕ್ಷರ    *rest;
    ದೀರ್ಘ    val;

    ಆದರೆ (*ವಿಳಾಸ == NUL)
	ಹೋಗು fail;

    ಆದರೆ (STRNCMP(ವಿಳಾಸ, "unix:", 5) == 0)
    {
	*unix_ಮಾರ್ಗ = vim_strsave(ವಿಳಾಸ + 5);
	ಹಿಂತಿರುಗಿ OK;
    }

    val = strtol((ಅಕ್ಷರ *)(ವಿಳಾಸ), &rest, 10);
    ಆದರೆ (val < 0 || val >= 65536 || *rest != NUL)
    {
	ಆದರೆ (!quiet)
	    semsg(_(e_invalid_ವಾದ_str), ವಿಳಾಸ);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    *ಪೋರ್ಟ್ = (ಪೂರ್ಣಾಂಕ)val;

    ಹಿಂತಿರುಗಿ OK;
fail:
    ಆದರೆ (!quiet)
	semsg(_(e_invalid_ವಾದ_str), ವಿಳಾಸ);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Implements ch_open().
 */
    ಸ್ಥಿರ channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*ವಿಳಾಸ;
    char_u	*p;
    ಅಕ್ಷರ	*rest;
    ಪೂರ್ಣಾಂಕ		ಪೋರ್ಟ್ = 0;
    ಪೂರ್ಣಾಂಕ		is_ipv6 = FALSE;
    ಪೂರ್ಣಾಂಕ		is_unix = FALSE;
    ಪೂರ್ಣಾಂಕ		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ವಿಳಾಸ = tv_get_string(&argvars[0]);
    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (*ವಿಳಾಸ == NUL)
    {
	semsg(_(e_invalid_ವಾದ_str), ವಿಳಾಸ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (STRCMP(ವಿಳಾಸ, "stdio") == 0)
	is_stdio = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(ವಿಳಾಸ, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	ವಿಳಾಸ += 5;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*ವಿಳಾಸ == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(ವಿಳಾಸ + 1, ']');
	ಆದರೆ (p == ಖಾಲಿ || *++p != ':')
	{
	    semsg(_(e_invalid_ವಾದ_str), ವಿಳಾಸ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// ipv4 address
	p = vim_strchr(ವಿಳಾಸ, ':');
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    semsg(_(e_invalid_ವಾದ_str), ವಿಳಾಸ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    ಆದರೆ (!is_unix && !is_stdio)
    {
	ಪೋರ್ಟ್ = strtol((ಅಕ್ಷರ *)(p + 1), &rest, 10);
	ಆದರೆ (ಪೋರ್ಟ್ <= 0 || ಪೋರ್ಟ್ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_ವಾದ_str), ವಿಳಾಸ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++ವಿಳಾಸ;
	    *(p - 1) = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *p = NUL;
    }

    // parse options
    clear_ಕೆಲಸ_options(&opt);
    opt.jo_ಬಹುಲಕ = CH_ಬಹುಲಕ_JSON;
    opt.jo_timeout = 2000;
    ಆದರೆ (get_ಕೆಲಸ_options(&argvars[1], &opt,
	    JO_ಬಹುಲಕ_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	ಹೋಗು theend;
    ಆದರೆ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹೋಗು theend;
    }

    ಆದರೆ (is_stdio)
	channel = channel_open_stdio();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_unix)
	channel = channel_open_unix((ಅಕ್ಷರ *)ವಿಳಾಸ, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	channel = channel_open((ಅಕ್ಷರ *)ವಿಳಾಸ, ಪೋರ್ಟ್, opt.jo_waittime, ಖಾಲಿ);
    ಆದರೆ (channel != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ	user_ಗಣ = opt.jo_ಗಣ;

	opt.jo_ಗಣ = JO_ALL;
	ಆದರೆ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_ಗಣ &= ~((JO_ಒಳ_ಬಹುಲಕ | JO_ಹೊರಹೋಗು_ಬಹುಲಕ | JO_ERR_ಬಹುಲಕ
				| JO_ಹೊರಹೋಗು_TIMEOUT | JO_ERR_TIMEOUT) & ~user_ಗಣ);
	channel_ಗಣ_options(channel, &opt);
    }
theend:
    free_ಕೆಲಸ_options(&opt);
    ಹಿಂತಿರುಗಿ channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
ಸ್ಥಿರ channel_T *stdio_channel = ಖಾಲಿ;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    ಸ್ಥಿರ channel_T *
channel_open_stdio(ಶೂನ್ಯ)
{
    channel_T	*channel;
    sock_T	ಒಳ_fd;		// our stdin, the channel reads from it
    sock_T	ಹೊರಹೋಗು_fd;		// our stdout, the channel writes to it

    ಆದರೆ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (stdio_channel != ಖಾಲಿ)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	ಪೂರ್ಣಾಂಕ	nul_fd;

	ಆದರೆ (!DuplicateHandle(proc, GetStdHandle(STD_ಒಳಹರಿವು_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	ಆದರೆ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ಖಾಲಿ, OPEN_EXISTING, 0,
		ಖಾಲಿ);
	ಆದರೆ (hnul != INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    ಗಣ_Std_Handle(STD_ಒಳಹರಿವು_HANDLE, hnul);
	    ಗಣ_Std_Handle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ಆದರೆ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	ಒಳ_fd = (sock_T)hin;
	ಹೊರಹೋಗು_fd = (sock_T)hout;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	nul_fd;

	ಒಳ_fd = dup(0);
	ಹೊರಹೋಗು_fd = dup(1);
	ಆದರೆ (ಒಳ_fd < 0 || ಹೊರಹೋಗು_fd < 0)
	{
	    ಆದರೆ (ಒಳ_fd >= 0)
		close(ಒಳ_fd);
	    ಆದರೆ (ಹೊರಹೋಗು_fd >= 0)
		close(ಹೊರಹೋಗು_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	(ಶೂನ್ಯ)fcntl(ಒಳ_fd, F_SETFD, FD_CLOEXEC);
	(ಶೂನ್ಯ)fcntl(ಹೊರಹೋಗು_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ಆದರೆ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
    {
	ch_error(ಖಾಲಿ, "Cannot allocate channel.");
	fd_close(ಒಳ_fd);
	fd_close(ಹೊರಹೋಗು_fd);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    channel_ಗಣ_pipes(channel, ಹೊರಹೋಗು_fd, ಒಳ_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ಹಿಂತಿರುಗಿ channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    ಅಕ್ಷರ	*rest;
    ಪೂರ್ಣಾಂಕ		ಪೋರ್ಟ್;
    ಪೂರ್ಣಾಂಕ		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    arg = tv_get_string(&argvars[0]);
    ಆದರೆ (*arg == NUL)
    {
	semsg(_(e_invalid_ವಾದ_str), arg);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	ಪೋರ್ಟ್ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೋರ್ಟ್ = strtol((ಅಕ್ಷರ *)(arg), &rest, 10);
	ಆದರೆ (ಪೋರ್ಟ್ < 0 || ಪೋರ್ಟ್ >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_ವಾದ_str), arg);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	*arg = NUL;
    }

    // parse options
    clear_ಕೆಲಸ_options(&opt);
    opt.jo_ಬಹುಲಕ = CH_ಬಹುಲಕ_JSON;
    opt.jo_timeout = 2000;
    ಆದರೆ (get_ಕೆಲಸ_options(&argvars[1], &opt,
	    JO_ಬಹುಲಕ_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	ಹೋಗು theend;
    ಆದರೆ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_ವಾದ));
	ಹೋಗು theend;
    }

    ಆದರೆ (is_unix)
	channel = channel_listen_unix((ಅಕ್ಷರ *)arg, ಖಾಲಿ, true);
    ಇಲ್ಲದಿದ್ದರೆ
	channel = channel_listen(ಪೋರ್ಟ್, ಖಾಲಿ);
    ಆದರೆ (channel != ಖಾಲಿ)
    {
	opt.jo_ಗಣ = JO_ALL;
	channel_ಗಣ_options(channel, &opt);
    }
theend:
    free_ಕೆಲಸ_options(&opt);
    ಹಿಂತಿರುಗಿ channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	ಪೂರ್ಣಾಂಕ ಪೋರ್ಟ್_ಒಳ,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ))
{
    ಪೂರ್ಣಾಂಕ			sd = -1;
    ರಚನೆ sockaddr_ಒಳ	server;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_IPV6
    ರಚನೆ hostent	*host;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ			val = 1;
    channel_T		*channel;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    channel_init_winsock();
#ಆದರೆ_ಅಂತ್ಯ

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
    {
	ch_error(ಖಾಲಿ, "Cannot allocate channel.");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((ಅಕ್ಷರ *)&server, 0, ಗಾತ್ರ(server));
    server.sin_family = AF_INET;
    server.sin_ಪೋರ್ಟ್ = htons(ಪೋರ್ಟ್_ಒಳ);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ಆದರೆ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ಪೋರ್ಟ್));
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&val, ಗಾತ್ರ(val)) < 0)
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, ಗಾತ್ರ(val)) < 0)
#ಆದರೆ_ಅಂತ್ಯ
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ಪೋರ್ಟ್));
	sock_close(sd);
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // Bind the socket to the port
    ಆದರೆ (bind(sd, (ರಚನೆ sockaddr *)&server, ಗಾತ್ರ(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ಪೋರ್ಟ್));
	sock_close(sd);
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_ಪೋರ್ಟ್));
	sock_close(sd);
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ಆದರೆ (ಪೋರ್ಟ್_ಒಳ == 0)
    {
	ರಚನೆ sockaddr_ಒಳ	addr;
	socklen_t		addr_len = ಗಾತ್ರ(addr);

	ಆದರೆ (getsockname(sd, (ರಚನೆ sockaddr *)&addr, &addr_len) == 0)
	    ಪೋರ್ಟ್_ಒಳ = ntohs(addr.sin_ಪೋರ್ಟ್);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ಅಕ್ಷರ *)vim_strsave((char_u *)"");
    channel->ch_ಪೋರ್ಟ್ = ಪೋರ್ಟ್_ಒಳ;
    channel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_SOCK);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_SOCK);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	ಅಕ್ಷರ *ಮಾರ್ಗ,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ),
	bool replace)
{
    ಪೂರ್ಣಾಂಕ			sd = -1;
    ರಚನೆ sockaddr_un	server;
    channel_T		*channel;
    size_t		ಮಾರ್ಗ_len;
    size_t		server_len;

    ಆದರೆ (ಮಾರ್ಗ == ಖಾಲಿ || *ಮಾರ್ಗ == NUL)
    {
	semsg(_(e_invalid_ವಾದ_str), ಮಾರ್ಗ == ಖಾಲಿ ? (ಅಕ್ಷರ *)"" : ಮಾರ್ಗ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಮಾರ್ಗ_len = STRLEN(ಮಾರ್ಗ);
    ಆದರೆ (ಮಾರ್ಗ_len >= ಗಾತ್ರ(server.sun_ಮಾರ್ಗ))
    {
	semsg(_(e_invalid_ವಾದ_str), ಮಾರ್ಗ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    channel_init_winsock();
#ಆದರೆ_ಅಂತ್ಯ

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
    {
	ch_error(ಖಾಲಿ, "Cannot allocate channel.");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    CLEAR_ಕ್ಷೇತ್ರ(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_ಮಾರ್ಗ, ಮಾರ್ಗ, ಗಾತ್ರ(server.sun_ಮಾರ್ಗ) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ಆದರೆ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_ಪೋರ್ಟ್));
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_ಮಾರ್ಗ);

    // Bind the socket to the path
    server_len = offsetof(ರಚನೆ sockaddr_un, sun_ಮಾರ್ಗ) + ಮಾರ್ಗ_len + 1;
    ಆದರೆ (bind(sd, (ರಚನೆ sockaddr *)&server, (ಪೂರ್ಣಾಂಕ)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_ಪೋರ್ಟ್));
	sock_close(sd);
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_ಪೋರ್ಟ್));
	sock_close(sd);
	channel_free(channel);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ಅಕ್ಷರ *)vim_strsave((char_u *)ಮಾರ್ಗ);
    channel->ch_ಪೋರ್ಟ್ = 0;
    channel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_SOCK);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_SOCK);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ channel;
}

     ಶೂನ್ಯ
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ಆದರೆ (*fd == INVALID_FD)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (part == PART_SOCK)
	sock_close(*fd);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ಆದರೆ ((part == PART_ಒಳ || channel->CH_ಒಳ_FD != *fd)
		&& (part == PART_ಹೊರಹೋಗು || channel->CH_ಹೊರಹೋಗು_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಆದರೆ (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#ಆದರೆ_ಅಂತ್ಯ
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_ಗಮ್ಯ_be_closed &= ~(1U << part);
}

    ಶೂನ್ಯ
channel_ಗಣ_pipes(channel_T *channel, sock_T ಒಳ, sock_T ಹೊರಹೋಗು, sock_T err)
{
    ಆದರೆ (ಒಳ != INVALID_FD)
    {
	ch_close_part(channel, PART_ಒಳ);
	channel->CH_ಒಳ_FD = ಒಳ;
#ಆದರೆ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ಆದರೆ (mch_isatty(ಒಳ))
	    channel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_ಒಳ);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (ಹೊರಹೋಗು != INVALID_FD)
    {
#ಆದರೆ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ಹೊರಹೋಗು);
#ಆದರೆ_ಅಂತ್ಯ
	ch_close_part(channel, PART_ಹೊರಹೋಗು);
	channel->CH_ಹೊರಹೋಗು_FD = ಹೊರಹೋಗು;
	channel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_ಹೊರಹೋಗು);
#ಆದರೆ defined(FEAT_GUI)
	channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_ಹೊರಹೋಗು);
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (err != INVALID_FD)
    {
#ಆದರೆ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#ಆದರೆ_ಅಂತ್ಯ
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_ERR);
#ಆದರೆ defined(FEAT_GUI)
	channel_gui_ರಿಜಿಸ್ಟರ್_one(channel, PART_ERR);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    ಶೂನ್ಯ
channel_ಗಣ_ಕೆಲಸ(channel_T *channel, ಕೆಲಸ_T *ಕೆಲಸ, jobopt_T *options)
{
    channel->ch_ಕೆಲಸ = ಕೆಲಸ;

    channel_ಗಣ_options(channel, options);

    ಆದರೆ (ಕೆಲಸ->jv_ಒಳ_buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    chanpart_T *ಒಳ_part = &channel->ch_part[PART_ಒಳ];

    ಗಣ_bufref(&ಒಳ_part->ch_bufref, ಕೆಲಸ->jv_ಒಳ_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (ಅಕ್ಷರ *)ಒಳ_part->ch_bufref.br_buf->b_ffname);
    ಆದರೆ (options->jo_ಗಣ & JO_ಒಳ_ತುದಿ)
    {
	ಆದರೆ (options->jo_ಒಳ_ತುದಿ == 0 && !(options->jo_ಗಣ & JO_ಒಳ_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    ಒಳ_part->ch_bufref.br_buf->b_write_ಗಮ್ಯ_channel = true;
	    ಒಳ_part->ch_buf_append = TRUE;
	    ಒಳ_part->ch_buf_ತುದಿ =
		ಒಳ_part->ch_bufref.br_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಒಳ_part->ch_buf_ತುದಿ = options->jo_ಒಳ_ತುದಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಒಳ_part->ch_buf_ತುದಿ = 1;
    ಆದರೆ (options->jo_ಗಣ & JO_ಒಳ_BOT)
	ಒಳ_part->ch_buf_bot = options->jo_ಒಳ_bot;
    ಇಲ್ಲದಿದ್ದರೆ
	ಒಳ_part->ch_buf_bot = ಒಳ_part->ch_bufref.br_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_ಗಣ_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	ಪೂರ್ಣಾಂಕ	    id)
{
    cbq_T *ತಲೆ = &channel->ch_part[part].ch_cb_ತಲೆ;
    cbq_T *ಅಂಶ = ALLOC_ONE(cbq_T);

    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    copy_callback(&ಅಂಶ->cq_callback, callback);
    ಅಂಶ->cq_seq_nr = id;
    ಅಂಶ->cq_prev = ತಲೆ->cq_prev;
    ತಲೆ->cq_prev = ಅಂಶ;
    ಅಂಶ->cq_ಮುಂದಿನ = ಖಾಲಿ;
    ಆದರೆ (ಅಂಶ->cq_prev == ಖಾಲಿ)
	ತಲೆ->cq_ಮುಂದಿನ = ಅಂಶ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಅಂಶ->cq_prev->cq_ಮುಂದಿನ = ಅಂಶ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_buf_ಸಾಲು(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *ಸಾಲು = ml_get_buf(buf, lnum, FALSE);
    ಪೂರ್ಣಾಂಕ	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    i;

    // Need to make a copy to be able to append a NL.
    ಆದರೆ ((p = alloc(len + 2)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಸ್ಮರಣೆ_ನಕಲು((ಅಕ್ಷರ *)p, (ಅಕ್ಷರ *)ಸಾಲು, len);

    ಆದರೆ (channel->ch_write_ಪಠ್ಯ_ಬಹುಲಕ)
	p[len] = CAR;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (i = 0; i < len; ++i)
	    ಆದರೆ (p[i] == NL)
		p[i] = NUL;

	p[len] = NL;
    }
    p[len + 1] = NUL;
    channel_send(channel, PART_ಒಳ, p, len + 1, "write_buf_line");
    vim_free(p);
}

/*
 * Return TRUE if "channel" can be written to.
 * Returns FALSE if the input is closed or the write would block.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
can_write_buf_ಸಾಲು(channel_T *channel)
{
    chanpart_T *ಒಳ_part = &channel->ch_part[PART_ಒಳ];

    ಆದರೆ (ಒಳ_part->ch_fd == INVALID_FD)
	ಹಿಂತಿರುಗಿ FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ಆದರೆ (ಒಳ_part->ch_ಖಂಡ_write == 1)
	ಒಳ_part->ch_ಖಂಡ_write = -1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_part->ch_ಖಂಡ_write == -1)
	ಒಳ_part->ch_ಖಂಡ_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ MSWIN
    {
# ಆದರೆ defined(HAVE_SELECT)
	ರಚನೆ timeval	tval;
	fd_set		wfds;
	ಪೂರ್ಣಾಂಕ		ret;

	FD_ZERO(&wfds);
	FD_SET((ಪೂರ್ಣಾಂಕ)ಒಳ_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	ಗಾಗಿ (;;)
	{
	    ret = select((ಪೂರ್ಣಾಂಕ)ಒಳ_part->ch_fd + 1, ಖಾಲಿ, &wfds, ಖಾಲಿ, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ಆದರೆ (ret == -1 && errno == EINTR)
		ಮುಂದುವರಿ;
#  endif
	    ಆದರೆ (ret <= 0 || ಒಳ_part->ch_ಖಂಡ_write == 1)
	    {
		ಆದರೆ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		ಇಲ್ಲದಿದ್ದರೆ
		    ch_log(channel, "Input not ready for writing");
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ನಿಲ್ಲಿಸು;
	}
# ಇಲ್ಲದಿದ್ದರೆ
	ರಚನೆ pollfd	fds;

	fds.fd = ಒಳ_part->ch_fd;
	fds.events = POLLOUT;
	ಆದರೆ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	ಆದರೆ (ಒಳ_part->ch_ಖಂಡ_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ಹಿಂತಿರುಗಿ FALSE;
	}
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    ಶೂನ್ಯ
channel_write_ಒಳ(channel_T *channel)
{
    chanpart_T *ಒಳ_part = &channel->ch_part[PART_ಒಳ];
    linenr_T    lnum;
    buf_T	*buf = ಒಳ_part->ch_bufref.br_buf;
    ಪೂರ್ಣಾಂಕ		written = 0;

    ಆದರೆ (buf == ಖಾಲಿ || ಒಳ_part->ch_buf_append)
	ಹಿಂತಿರುಗಿ;  // no buffer or using appending
    ಆದರೆ (!bufref_valid(&ಒಳ_part->ch_bufref) || buf->b_ml.ml_mfp == ಖಾಲಿ)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	ಒಳ_part->ch_bufref.br_buf = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ;
    }

    ಗಾಗಿ (lnum = ಒಳ_part->ch_buf_ತುದಿ; lnum <= ಒಳ_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
    {
	ಆದರೆ (!can_write_buf_ಸಾಲು(channel))
	    ನಿಲ್ಲಿಸು;
	write_buf_ಸಾಲು(buf, lnum, channel);
	++written;
    }

    ಆದರೆ (written == 1)
	ch_log(channel, "written line %d to channel", (ಪೂರ್ಣಾಂಕ)lnum - 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    ಒಳ_part->ch_buf_ತುದಿ = lnum;
    ಆದರೆ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || lnum > ಒಳ_part->ch_buf_bot)
    {
#ಆದರೆ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ಆದರೆ (channel->ch_ಕೆಲಸ != ಖಾಲಿ)
	    ಪದ_send_eof(channel);
#ಆದರೆ_ಅಂತ್ಯ

	// Writing is done, no longer need the buffer.
	ಒಳ_part->ch_bufref.br_buf = ಖಾಲಿ;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	ಆದರೆ (!channel->ch_anonymous_pipe)
#ಆದರೆ_ಅಂತ್ಯ
	    ch_close_part(channel, PART_ಒಳ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ch_log(channel, "Still %ld more lines to write",
				   (ದೀರ್ಘ)(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    ಶೂನ್ಯ
channel_ಬಫರ್_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ಆದರೆ (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_ಹೆಸರುಗಳು[part]);
		ch_part->ch_bufref.br_buf = ಖಾಲಿ;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    ಶೂನ್ಯ
channel_write_ಒಳಹರಿವು(channel_T *channel)
{
    chanpart_T	*ಒಳ_part = &channel->ch_part[PART_ಒಳ];

    ಆದರೆ (ಒಳ_part->ch_writeque.wq_ಮುಂದಿನ != ಖಾಲಿ)
	channel_send(channel, PART_ಒಳ, (char_u *)"", 0, "channel_write_input");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_part->ch_bufref.br_buf != ಖಾಲಿ)
    {
	ಆದರೆ (ಒಳ_part->ch_buf_append)
	    channel_write_new_ಸಾಲುಗಳು(ಒಳ_part->ch_bufref.br_buf);
	ಇಲ್ಲದಿದ್ದರೆ
	    channel_write_ಒಳ(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    ಶೂನ್ಯ
channel_write_any_ಸಾಲುಗಳು(ಶೂನ್ಯ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_ಒಳಹರಿವು(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    ಶೂನ್ಯ
channel_write_new_ಸಾಲುಗಳು(buf_T *buf)
{
    channel_T	*channel;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *ಒಳ_part = &channel->ch_part[PART_ಒಳ];
	linenr_T    lnum;
	ಪೂರ್ಣಾಂಕ	    written = 0;

	ಆದರೆ (ಒಳ_part->ch_bufref.br_buf == buf && ಒಳ_part->ch_buf_append)
	{
	    ಆದರೆ (ಒಳ_part->ch_fd == INVALID_FD)
		ಮುಂದುವರಿ;  // pipe was closed
	    ಸಿಕ್ಕಿತು_one = TRUE;
	    ಗಾಗಿ (lnum = ಒಳ_part->ch_buf_bot; lnum < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
								       ++lnum)
	    {
		ಆದರೆ (!can_write_buf_ಸಾಲು(channel))
		    ನಿಲ್ಲಿಸು;
		write_buf_ಸಾಲು(buf, lnum, channel);
		++written;
	    }

	    ಆದರೆ (written == 1)
		ch_log(channel, "written line %d to channel", (ಪೂರ್ಣಾಂಕ)lnum - 1);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ಆದರೆ (lnum < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ch_log(channel, "Still %ld more lines to write",
				       (ದೀರ್ಘ)(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum));

	    ಒಳ_part->ch_buf_bot = lnum;
	}
    }
    ಆದರೆ (!ಸಿಕ್ಕಿತು_one)
	buf->b_write_ಗಮ್ಯ_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    ಆದರೆ (ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback == 0)
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
    readq_T *ತಲೆ = &channel->ch_part[part].ch_ತಲೆ;

    ಹಿಂತಿರುಗಿ ತಲೆ->rq_ಮುಂದಿನ;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_ಮೊದಲ_nl(readq_T *ಗಂಟು)
{
    char_u  *ಬಫರ್ = ಗಂಟು->rq_ಬಫರ್;
    long_u  i;

    ಗಾಗಿ (i = 0; i < ಗಂಟು->rq_buflen; ++i)
	ಆದರೆ (ಬಫರ್[i] == NL)
	    ಹಿಂತಿರುಗಿ ಬಫರ್ + i;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಂಕ *outlen)
{
    readq_T *ತಲೆ = &channel->ch_part[part].ch_ತಲೆ;
    readq_T *ಗಂಟು = ತಲೆ->rq_ಮುಂದಿನ;
    char_u *p;

    ಆದರೆ (ಗಂಟು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (outlen != ಖಾಲಿ)
	*outlen += ಗಂಟು->rq_buflen;
    // dispose of the node but keep the buffer
    p = ಗಂಟು->rq_ಬಫರ್;
    ತಲೆ->rq_ಮುಂದಿನ = ಗಂಟು->rq_ಮುಂದಿನ;
    ಆದರೆ (ಗಂಟು->rq_ಮುಂದಿನ == ಖಾಲಿ)
	ತಲೆ->rq_prev = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಂಟು->rq_ಮುಂದಿನ->rq_prev = ಖಾಲಿ;
    vim_free(ಗಂಟು);
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ಸ್ಥಿರ char_u *
channel_get_all(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಂಕ *outlen)
{
    readq_T *ತಲೆ = &channel->ch_part[part].ch_ತಲೆ;
    readq_T *ಗಂಟು;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    ಗಾಗಿ (ಗಂಟು = ತಲೆ->rq_ಮುಂದಿನ; ಗಂಟು != ಖಾಲಿ; ಗಂಟು = ಗಂಟು->rq_ಮುಂದಿನ)
	len += ಗಂಟು->rq_buflen;
    res = alloc(len + 1);
    ಆದರೆ (res == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    p = res;
    ಗಾಗಿ (ಗಂಟು = ತಲೆ->rq_ಮುಂದಿನ; ಗಂಟು != ಖಾಲಿ; ಗಂಟು = ಗಂಟು->rq_ಮುಂದಿನ)
    {
	mch_memmove(p, ಗಂಟು->rq_ಬಫರ್, ಗಂಟು->rq_buflen);
	p += ಗಂಟು->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    ಮಾಡು
    {
	p = channel_get(channel, part, ಖಾಲಿ);
	vim_free(p);
    } ಆಗ (p != ಖಾಲಿ);

    ಆದರೆ (outlen != ಖಾಲಿ)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ಹಿಂತಿರುಗಿ res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ಆಗ (p < res + len)
    {
	ಆದರೆ (*p == NUL)
	    *p = NL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ಆದರೆ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ಆಗ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ಆದರೆ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#ಆದರೆ_ಅಂತ್ಯ
	++p;
    }

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    ಶೂನ್ಯ
channel_consume(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಂಕ len)
{
    readq_T *ತಲೆ = &channel->ch_part[part].ch_ತಲೆ;
    readq_T *ಗಂಟು = ತಲೆ->rq_ಮುಂದಿನ;
    char_u *buf = ಗಂಟು->rq_ಬಫರ್;

    ಆದರೆ (len < 0 || (long_u)len > ಗಂಟು->rq_buflen)
	ಹಿಂತಿರುಗಿ;
    mch_memmove(buf, buf + len, ಗಂಟು->rq_buflen - len);
    ಗಂಟು->rq_buflen -= len;
    ಗಂಟು->rq_ಬಫರ್[ಗಂಟು->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    ಪೂರ್ಣಾಂಕ
channel_collapse(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಂಕ want_nl)
{
    ch_ಬಹುಲಕ_T	ಬಹುಲಕ = channel->ch_part[part].ch_ಬಹುಲಕ;
    readq_T	*ತಲೆ = &channel->ch_part[part].ch_ತಲೆ;
    readq_T	*ಗಂಟು = ತಲೆ->rq_ಮುಂದಿನ;
    readq_T	*ಕೊನೆಯ_ಗಂಟು;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ಆದರೆ (ಗಂಟು == ಖಾಲಿ || ಗಂಟು->rq_ಮುಂದಿನ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಕೊನೆಯ_ಗಂಟು = ಗಂಟು->rq_ಮುಂದಿನ;
    len = ಗಂಟು->rq_buflen + ಕೊನೆಯ_ಗಂಟು->rq_buflen;
    ಆದರೆ (want_nl || ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
	ಆಗ (ಕೊನೆಯ_ಗಂಟು->rq_ಮುಂದಿನ != ಖಾಲಿ
		&& (ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP
		    || channel_ಮೊದಲ_nl(ಕೊನೆಯ_ಗಂಟು) == ಖಾಲಿ))
	{
	    ಕೊನೆಯ_ಗಂಟು = ಕೊನೆಯ_ಗಂಟು->rq_ಮುಂದಿನ;
	    len += ಕೊನೆಯ_ಗಂಟು->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ಆದರೆ (newbuf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;	    // out of memory
    mch_memmove(p, ಗಂಟು->rq_ಬಫರ್, ಗಂಟು->rq_buflen);
    p += ಗಂಟು->rq_buflen;
    vim_free(ಗಂಟು->rq_ಬಫರ್);
    ಗಂಟು->rq_ಬಫರ್ = newbuf;
    ಗಾಗಿ (n = ಗಂಟು; n != ಕೊನೆಯ_ಗಂಟು; )
    {
	n = n->rq_ಮುಂದಿನ;
	mch_memmove(p, n->rq_ಬಫರ್, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_ಬಫರ್);
    }
    *p = NUL;
    ಗಂಟು->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    ಗಾಗಿ (n = ಗಂಟು->rq_ಮುಂದಿನ; n != ಕೊನೆಯ_ಗಂಟು; )
    {
	n = n->rq_ಮುಂದಿನ;
	vim_free(n->rq_prev);
    }
    ಗಂಟು->rq_ಮುಂದಿನ = ಕೊನೆಯ_ಗಂಟು->rq_ಮುಂದಿನ;
    ಆದರೆ (ಕೊನೆಯ_ಗಂಟು->rq_ಮುಂದಿನ == ಖಾಲಿ)
	ತಲೆ->rq_prev = ಗಂಟು;
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೊನೆಯ_ಗಂಟು->rq_ಮುಂದಿನ->rq_prev = ಗಂಟು;
    vim_free(ಕೊನೆಯ_ಗಂಟು);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_save(channel_T *channel, ch_part_T part, char_u *buf, ಪೂರ್ಣಾಂಕ len,
						      ಪೂರ್ಣಾಂಕ prepend, ಅಕ್ಷರ *lead)
{
    readq_T *ಗಂಟು;
    readq_T *ತಲೆ = &channel->ch_part[part].ch_ತಲೆ;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಂಟು = ALLOC_ONE(readq_T);
    ಆದರೆ (ಗಂಟು == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    ಗಂಟು->rq_ಬಫರ್ = alloc(len + 1);
    ಆದರೆ (ಗಂಟು->rq_ಬಫರ್ == ಖಾಲಿ)
    {
	vim_free(ಗಂಟು);
	ಹಿಂತಿರುಗಿ FAIL;	    // out of memory
    }

    ಆದರೆ (channel->ch_part[part].ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_NL)
    {
	// Drop any CR before a NL.
	p = ಗಂಟು->rq_ಬಫರ್;
	ಗಾಗಿ (i = 0; i < len; ++i)
	    ಆದರೆ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	ಗಂಟು->rq_buflen = (long_u)(p - ಗಂಟು->rq_ಬಫರ್);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	mch_memmove(ಗಂಟು->rq_ಬಫರ್, buf, len);
	ಗಂಟು->rq_ಬಫರ್[len] = NUL;
	ಗಂಟು->rq_buflen = (long_u)len;
    }

    ಆದರೆ (prepend)
    {
	// prepend node to the head of the queue
	ಗಂಟು->rq_ಮುಂದಿನ = ತಲೆ->rq_ಮುಂದಿನ;
	ಗಂಟು->rq_prev = ಖಾಲಿ;
	ಆದರೆ (ತಲೆ->rq_ಮುಂದಿನ == ಖಾಲಿ)
	    ತಲೆ->rq_prev = ಗಂಟು;
	ಇಲ್ಲದಿದ್ದರೆ
	    ತಲೆ->rq_ಮುಂದಿನ->rq_prev = ಗಂಟು;
	ತಲೆ->rq_ಮುಂದಿನ = ಗಂಟು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// append node to the tail of the queue
	ಗಂಟು->rq_ಮುಂದಿನ = ಖಾಲಿ;
	ಗಂಟು->rq_prev = ತಲೆ->rq_prev;
	ಆದರೆ (ತಲೆ->rq_prev == ಖಾಲಿ)
	    ತಲೆ->rq_ಮುಂದಿನ = ಗಂಟು;
	ಇಲ್ಲದಿದ್ದರೆ
	    ತಲೆ->rq_prev->rq_ಮುಂದಿನ = ಗಂಟು;
	ತಲೆ->rq_prev = ಗಂಟು;
    }

    ಆದರೆ (ch_log_ಸಕ್ರಿಯ() && lead != ಖಾಲಿ)
	ch_log_literal(lead, channel, part, buf, len);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    ಪೂರ್ಣಾಂಕ
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*ಮುಂದಿನ = channel_get(channel, part, ಖಾಲಿ);
    ಪೂರ್ಣಾಂಕ		keeplen;
    ಪೂರ್ಣಾಂಕ		addlen;
    char_u	*p;

    ಆದರೆ (ಮುಂದಿನ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    keeplen = reader->js_ಕೊನೆ - reader->js_buf;
    ಆದರೆ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (ಪೂರ್ಣಾಂಕ)STRLEN(ಮುಂದಿನ);
	p = alloc(keeplen + addlen + 1);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    vim_free(ಮುಂದಿನ);
	    ಹಿಂತಿರುಗಿ FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, ಮುಂದಿನ, addlen + 1);
	vim_free(ಮುಂದಿನ);
	ಮುಂದಿನ = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = ಮುಂದಿನ;
    ಹಿಂತಿರುಗಿ TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*ಸಾಲು_ಆರಂಭ;
    char_u	*p;
    int_u	hdr_len;
    ಪೂರ್ಣಾಂಕ		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_ಕೊನೆ = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ಆಗ (TRUE)
    {
	ಸಾಲು_ಆರಂಭ = p;
	ಆಗ (*p != NUL && *p != '\n')
	    p++;
	ಆದರೆ (*p == NUL)			// partial header
	    ಹಿಂತಿರುಗಿ MAYBE;
	p++;

	// process the content length field (if present)
	ಆದರೆ ((p - ಸಾಲು_ಆರಂಭ > 16)
		&& STRNICMP(ಸಾಲು_ಆರಂಭ, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((ಅಕ್ಷರ *)ಸಾಲು_ಆರಂಭ + 16, ಖಾಲಿ, 10);
	    ಆದರೆ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ ((p - ಸಾಲು_ಆರಂಭ) == 2 && ಸಾಲು_ಆರಂಭ[0] == '\r' &&
		ಸಾಲು_ಆರಂಭ[1] == '\n')
	    // reached the empty line
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (payload_len == -1)
	// Content-Length field is not present in the header
	ಹಿಂತಿರುಗಿ FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ಆದರೆ (jsbuf_len < hdr_len + payload_len)
	ಹಿಂತಿರುಗಿ MAYBE;

    reader->js_ಬಳಸಿದ += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_ಕೊನೆ = reader->js_buf + hdr_len + payload_len;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    ಪೂರ್ಣಾಂಕ
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*ಅಂಶ;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*ತಲೆ = &chanpart->ch_json_ತಲೆ;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;
    ಪೂರ್ಣಾಂಕ		ret;

    ಆದರೆ (channel_peek(channel, part) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    reader.js_buf = channel_get(channel, part, ಖಾಲಿ);
    reader.js_ಬಳಸಿದ = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ಆದರೆ (!socketserver && (chanpart->ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP
	    || chanpart->ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP))
	ಸ್ಥಿತಿ_2 = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
    {
	++emsg_silent;
	ಸ್ಥಿತಿ_2 = json_decode(&reader, &listtv,
				chanpart->ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ಆದರೆ (!socketserver && (chanpart->ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP
		    || chanpart->ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!socketserver && chanpart->ch_ಬಹುಲಕ != CH_ಬಹುಲಕ_LSP
		&& chanpart->ch_ಬಹುಲಕ != CH_ಬಹುಲಕ_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ಆದರೆ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    ಇಲ್ಲದಿದ್ದರೆ
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಂಶ = ALLOC_ONE(jsonq_T);
	    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
		clear_tv(&listtv);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಅಂಶ->jq_no_callback = FALSE;
		ಅಂಶ->jq_ಮೌಲ್ಯ = alloc_tv();
		ಆದರೆ (ಅಂಶ->jq_ಮೌಲ್ಯ == ಖಾಲಿ)
		{
		    vim_free(ಅಂಶ);
		    clear_tv(&listtv);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *ಅಂಶ->jq_ಮೌಲ್ಯ = listtv;
		    ಅಂಶ->jq_prev = ತಲೆ->jq_prev;
		    ತಲೆ->jq_prev = ಅಂಶ;
		    ಅಂಶ->jq_ಮುಂದಿನ = ಖಾಲಿ;
		    ಆದರೆ (ಅಂಶ->jq_prev == ಖಾಲಿ)
			ತಲೆ->jq_ಮುಂದಿನ = ಅಂಶ;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಅಂಶ->jq_prev->jq_ಮುಂದಿನ = ಅಂಶ;
		}
	    }
	}
    }

    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
	chanpart->ch_wait_len = 0;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸ್ಥಿತಿ_2 == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ಆದರೆ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (ಪೂರ್ಣಾಂಕ)buflen);
	    reader.js_ಬಳಸಿದ = 0;
	    chanpart->ch_wait_len = buflen;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    chanpart->ch_deadline = Get_Tick_ಎಣಿಕೆ() + 100L;
#ಇಲ್ಲದಿದ್ದರೆ
	    gettimeofday(&chanpart->ch_deadline, ಖಾಲಿ);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ಆದರೆ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ timeout;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    timeout = (ಪೂರ್ಣಾಂಕ)(Get_Tick_ಎಣಿಕೆ() - chanpart->ch_deadline) > 0;
#ಇಲ್ಲದಿದ್ದರೆ
	    {
		ರಚನೆ timeval now_tv;

		gettimeofday(&now_tv, ಖಾಲಿ);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (timeout)
	    {
		ಸ್ಥಿತಿ_2 = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		reader.js_ಬಳಸಿದ = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reader.js_buf[reader.js_ಬಳಸಿದ] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_ಬಳಸಿದ,
			(ಪೂರ್ಣಾಂಕ)(reader.js_ಕೊನೆ - reader.js_buf) - reader.js_ಬಳಸಿದ,
								  TRUE, ಖಾಲಿ);
	ret = ಸ್ಥಿತಿ_2 == MAYBE ? FALSE: TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ret = FALSE;

    vim_free(reader.js_buf);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_cb_ಗಂಟು(cbq_T *ತಲೆ, cbq_T *ಗಂಟು)
{
    ಆದರೆ (ಗಂಟು->cq_prev == ಖಾಲಿ)
	ತಲೆ->cq_ಮುಂದಿನ = ಗಂಟು->cq_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಂಟು->cq_prev->cq_ಮುಂದಿನ = ಗಂಟು->cq_ಮುಂದಿನ;
    ಆದರೆ (ಗಂಟು->cq_ಮುಂದಿನ == ಖಾಲಿ)
	ತಲೆ->cq_prev = ಗಂಟು->cq_prev;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಂಟು->cq_ಮುಂದಿನ->cq_prev = ಗಂಟು->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    ಶೂನ್ಯ
remove_json_ಗಂಟು(jsonq_T *ತಲೆ, jsonq_T *ಗಂಟು)
{
    ಆದರೆ (ಗಂಟು->jq_prev == ಖಾಲಿ)
	ತಲೆ->jq_ಮುಂದಿನ = ಗಂಟು->jq_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಂಟು->jq_prev->jq_ಮುಂದಿನ = ಗಂಟು->jq_ಮುಂದಿನ;
    ಆದರೆ (ಗಂಟು->jq_ಮುಂದಿನ == ಖಾಲಿ)
	ತಲೆ->jq_prev = ಗಂಟು->jq_prev;
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಂಟು->jq_ಮುಂದಿನ->jq_prev = ಗಂಟು->jq_prev;
    vim_free(ಗಂಟು);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_ಕೂಡಿಸು_ಖಂಡ_id(chanpart_T *chanpart, ಪೂರ್ಣಾಂಕ id)
{
    garray_T *gap = &chanpart->ch_ಖಂಡ_ids;

    ಆದರೆ (gap->ga_growsize == 0)
	ga_init2(gap, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), 10);
    ಆದರೆ (ga_grow(gap, 1) == OK)
    {
	((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_remove_ಖಂಡ_id(chanpart_T *chanpart, ಪೂರ್ಣಾಂಕ id)
{
    garray_T	*gap = &chanpart->ch_ಖಂಡ_ids;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	ಆದರೆ (((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[i] == id)
	{
	    --gap->ga_len;
	    ಆದರೆ (i < gap->ga_len)
	    {
		ಪೂರ್ಣಾಂಕ *p = ((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ));
	    }
	    ಹಿಂತಿರುಗಿ;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_has_ಖಂಡ_id(chanpart_T *chanpart, ಪೂರ್ಣಾಂಕ id)
{
    garray_T	*gap = &chanpart->ch_ಖಂಡ_ids;
    ಪೂರ್ಣಾಂಕ		i;

    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	ಆದರೆ (((ಪೂರ್ಣಾಂಕ *)gap->ga_ದತ್ತಾಂಶ)[i] == id)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	ಪೂರ್ಣಾಂಕ	    id,
	ಪೂರ್ಣಾಂಕ	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *ತಲೆ = &channel->ch_part[part].ch_json_ತಲೆ;
    jsonq_T   *ಅಂಶ = ತಲೆ->jq_ಮುಂದಿನ;

    ಆಗ (ಅಂಶ != ಖಾಲಿ)
    {
	list_T	    *l;
	typval_T    *tv;

	ಆದರೆ (channel->ch_part[part].ch_ಬಹುಲಕ != CH_ಬಹುಲಕ_LSP
		&& channel->ch_part[part].ch_ಬಹುಲಕ != CH_ಬಹುಲಕ_DAP)
	{
	    l = ಅಂಶ->jq_ಮೌಲ್ಯ->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_ಮೊದಲ->li_tv;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ಆದರೆ (channel->ch_part[part].ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ಆದರೆ (id > 0)
		{
		    ಆದರೆ (ಅಂಶ->jq_ಮೌಲ್ಯ->v_type != VAR_DICT)
			ಹೋಗು nextitem;
		    d = ಅಂಶ->jq_ಮೌಲ್ಯ->vval.v_dict;
		    ಆದರೆ (d == ಖಾಲಿ)
			ಹೋಗು nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ಆದರೆ (dict_has_ಕೀಲಿ(d, "method"))
			ಹೋಗು nextitem;
		    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"id", -1);
		    ಆದರೆ (di == ಖಾಲಿ)
			ಹೋಗು nextitem;
		    tv = &di->di_tv;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    tv = ಅಂಶ->jq_ಮೌಲ್ಯ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (id > 0)
		{
		    ಆದರೆ (ಅಂಶ->jq_ಮೌಲ್ಯ->v_type != VAR_DICT)
			ಹೋಗು nextitem;
		    d = ಅಂಶ->jq_ಮೌಲ್ಯ->vval.v_dict;
		    ಆದರೆ (d == ಖಾಲಿ)
			ಹೋಗು nextitem;
		    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"request_seq", -1);
		    ಆದರೆ (di == ಖಾಲಿ)
			ಹೋಗು nextitem;
		    tv = &di->di_tv;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    tv = ಅಂಶ->jq_ಮೌಲ್ಯ;
	    }
	}

	ಆದರೆ ((without_callback || !ಅಂಶ->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_ಸಂಖ್ಯೆ && tv->vval.v_ಸಂಖ್ಯೆ == id)
	      || (id <= 0 && (tv->v_type != VAR_ಸಂಖ್ಯೆ
		 || tv->vval.v_ಸಂಖ್ಯೆ == 0
		 || !channel_has_ಖಂಡ_id(
				&channel->ch_part[part], tv->vval.v_ಸಂಖ್ಯೆ)))))
	{
	    *rettv = ಅಂಶ->jq_ಮೌಲ್ಯ;
	    ಆದರೆ (tv->v_type == VAR_ಸಂಖ್ಯೆ)
		ch_log(channel, "Getting JSON message %ld",
						      (ದೀರ್ಘ)tv->vval.v_ಸಂಖ್ಯೆ);
	    remove_json_ಗಂಟು(ತಲೆ, ಅಂಶ);
	    ಹಿಂತಿರುಗಿ OK;
	}
nextitem:
	ಅಂಶ = ಅಂಶ->jq_ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_ತಳ್ಳು_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *ತಲೆ = &channel->ch_part[part].ch_json_ತಲೆ;
    jsonq_T   *ಅಂಶ = ತಲೆ->jq_ಮುಂದಿನ;
    jsonq_T   *newitem;

    ಆದರೆ (ತಲೆ->jq_prev != ಖಾಲಿ && ತಲೆ->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	ಅಂಶ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆಗ (ಅಂಶ != ಖಾಲಿ && ಅಂಶ->jq_no_callback)
	// append after the last item that was pushed back
	ಅಂಶ = ಅಂಶ->jq_ಮುಂದಿನ;

    newitem = ALLOC_ONE(jsonq_T);
    ಆದರೆ (newitem == ಖಾಲಿ)
    {
	clear_tv(rettv);
	ಹಿಂತಿರುಗಿ;
    }

    newitem->jq_ಮೌಲ್ಯ = alloc_tv();
    ಆದರೆ (newitem->jq_ಮೌಲ್ಯ == ಖಾಲಿ)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ಹಿಂತಿರುಗಿ;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_ಮೌಲ್ಯ = *rettv;
    ಆದರೆ (ಅಂಶ == ಖಾಲಿ)
    {
	// append to the end
	newitem->jq_prev = ತಲೆ->jq_prev;
	ತಲೆ->jq_prev = newitem;
	newitem->jq_ಮುಂದಿನ = ಖಾಲಿ;
	ಆದರೆ (newitem->jq_prev == ಖಾಲಿ)
	    ತಲೆ->jq_ಮುಂದಿನ = newitem;
	ಇಲ್ಲದಿದ್ದರೆ
	    newitem->jq_prev->jq_ಮುಂದಿನ = newitem;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// append after "item"
	newitem->jq_prev = ಅಂಶ;
	newitem->jq_ಮುಂದಿನ = ಅಂಶ->jq_ಮುಂದಿನ;
	ಅಂಶ->jq_ಮುಂದಿನ = newitem;
	ಆದರೆ (newitem->jq_ಮುಂದಿನ == ಖಾಲಿ)
	    ತಲೆ->jq_prev = newitem;
	ಇಲ್ಲದಿದ್ದರೆ
	    newitem->jq_ಮುಂದಿನ->jq_prev = newitem;
    }
}

#ವ್ಯಾಖ್ಯಾನಿಸು CH_JSON_MAX_ವಾದಗಳು 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    ಪೂರ್ಣಾಂಕ	    options = channel->ch_part[part].ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_JS
								 ? JSON_JS : 0;

    ಆದರೆ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ಆದರೆ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_ವಾದ));
	ಹಿಂತಿರುಗಿ;
    }
    arg = argv[1].vval.v_string;
    ಆದರೆ (arg == ಖಾಲಿ)
	arg = (char_u *)"";

    ಆದರೆ (STRCMP(cmd, "ex") == 0)
    {
	ಪೂರ್ಣಾಂಕ	called_emsg_before = called_emsg;
	char_u	*p = arg;
	ಪೂರ್ಣಾಂಕ	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (ಅಕ್ಷರ *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ಆದರೆ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ಆದರೆ (do_emsg_silent)
	    --emsg_silent;
	ಆದರೆ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (ಅಕ್ಷರ *)get_vim_var_str(VV_ERRMSG));
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (ಅಕ್ಷರ *)arg);
	CLEAR_ಕ್ಷೇತ್ರ(ea);
	ea.arg = arg;
	ea.addr_ಎಣಿಕೆ = 0;
	ea.forceit = TRUE; // no mapping
	ex_ಸಾಮಾನ್ಯ(&ea);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	ಹೊರಹೋಗು_flush_cursor(TRUE, FALSE);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	ಪೂರ್ಣಾಂಕ is_call = cmd[0] == 'c';
	ಪೂರ್ಣಾಂಕ id_idx = is_call ? 3 : 2;

	ಆದರೆ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ಆದರೆ (p_verbose > 2)
		emsg(_(e_ಕೊನೆಯ_ವಾದ_for_ಅಭಿವ್ಯಕ್ತಿ_call_must_be_ಸಂಖ್ಯೆ));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ಆದರೆ (p_verbose > 2)
		emsg(_(e_ಮೂರನೇ_ವಾದ_for_call_must_be_list));
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    typval_T	*tv = ಖಾಲಿ;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ಖಾಲಿ;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ಆದರೆ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (ಅಕ್ಷರ *)arg);
		tv = eval_ಅಭಿವ್ಯಕ್ತಿ(arg, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ch_log(channel, "Calling '%s'", (ಅಕ್ಷರ *)arg);
		ಆದರೆ (func_call(arg, &argv[2], ಖಾಲಿ, ಖಾಲಿ, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ಆದರೆ (argv[id_idx].v_type == VAR_ಸಂಖ್ಯೆ)
	    {
		ಪೂರ್ಣಾಂಕ id = argv[id_idx].vval.v_ಸಂಖ್ಯೆ;

		ಆದರೆ (tv != ಖಾಲಿ)
		    json = json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(id, tv, options | JSON_NL);
		ಆದರೆ (tv == ಖಾಲಿ || (json != ಖಾಲಿ && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(id, &err_tv, options | JSON_NL);
		}
		ಆದರೆ (json != ಖಾಲಿ)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_ಒಳ,
				 json, (ಪೂರ್ಣಾಂಕ)STRLEN(json), (ಅಕ್ಷರ *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ಆದರೆ (tv == &res_tv)
		clear_tv(tv);
	    ಇಲ್ಲದಿದ್ದರೆ
		free_tv(tv);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (ಅಕ್ಷರ *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_one_ಸಮಯ_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *ಅಂಶ,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (ಅಕ್ಷರ *)ಅಂಶ->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_ಗಂಟು(cbhead, ಅಂಶ);
    invoke_callback(channel, &ಅಂಶ->cq_callback, argv);
    free_callback(&ಅಂಶ->cq_callback);
    vim_free(ಅಂಶ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
append_ಗಮ್ಯ_ಬಫರ್(
    buf_T	*ಬಫರ್,
    char_u	*ಸಂದೇಶ,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    bool	save_write_ಗಮ್ಯ = ಬಫರ್->b_write_ಗಮ್ಯ_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    ಪೂರ್ಣಾಂಕ		save_p_ma = ಬಫರ್->b_p_ma;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_2 = (ಬಫರ್->b_ml.ml_flags & ML_ಖಾಲಿಗಳು) ? 1 : 0;

    ಆದರೆ (!ಬಫರ್->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ಆದರೆ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ಹಿಂತಿರುಗಿ;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ಆದರೆ (save_write_ಗಮ್ಯ)
    {
	--lnum;
	ಬಫರ್->b_write_ಗಮ್ಯ_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (ಪೂರ್ಣಾಂಕ)lnum + 1 - ಖಾಲಿಗಳು_2, ಬಫರ್->b_fname);

    ಬಫರ್->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, ಬಫರ್);
    ಆದರೆ (curbuf != ಬಫರ್)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ಹಿಂತಿರುಗಿ;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - ಖಾಲಿಗಳು_2, lnum + 1);

    ಆದರೆ (ಖಾಲಿಗಳು_2)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, ಸಂದೇಶ, TRUE);
	lnum = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ml_append(lnum, ಸಂದೇಶ, 0, FALSE);
    appended_ಸಾಲುಗಳು_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ಆದರೆ (ch_part->ch_nomodifiable)
	ಬಫರ್->b_p_ma = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	ಬಫರ್->b_p_ma = save_p_ma;

    ಆದರೆ (ಬಫರ್->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ಆದರೆ (wp->w_ಬಫರ್ == ಬಫರ್)
	    {
		ಪೂರ್ಣಾಂಕ move_cursor = save_write_ಗಮ್ಯ
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.ಲಂಬಸಾಲು == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ಆದರೆ (move_cursor || wp->w_topline > ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		{
		    win_T *save_curwin = curwin;

		    ಆದರೆ (move_cursor)
			++wp->w_cursor.lnum;
		    curwin = wp;
		    curbuf = curwin->w_ಬಫರ್;
		    scroll_cursor_bot(0, FALSE);
		    curwin = save_curwin;
		    curbuf = curwin->w_ಬಫರ್;
		}
	    }
	}
	redraw_buf_and_ಸ್ಥಿತಿ_later(ಬಫರ್, UPD_VALID);
	channel_need_redraw = TRUE;
    }

    ಆದರೆ (save_write_ಗಮ್ಯ)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	ಬಫರ್->b_write_ಗಮ್ಯ_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *ಒಳ_part = &ch->ch_part[PART_ಒಳ];

	    ಆದರೆ (ಒಳ_part->ch_bufref.br_buf == ಬಫರ್)
		ಒಳ_part->ch_buf_bot = ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *ಸಂದೇಶ;

    ಆಗ ((ಸಂದೇಶ = channel_get(channel, part, ಖಾಲಿ)) != ಖಾಲಿ)
    {
	ch_log(channel, "Dropping message '%s'", (ಅಕ್ಷರ *)ಸಂದೇಶ);
	vim_free(ಸಂದೇಶ);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_use_json_ತಲೆ(channel_T *channel, ch_part_T part)
{
    ch_ಬಹುಲಕ_T	ch_ಬಹುಲಕ = channel->ch_part[part].ch_ಬಹುಲಕ;

    ಹಿಂತಿರುಗಿ ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_JSON || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_JS
						     || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP
						     || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*ಸಂದೇಶ = ಖಾಲಿ;
    blob_T	*blob = ಖಾಲಿ;
    typval_T	*listtv = ಖಾಲಿ;
    typval_T	argv[CH_JSON_MAX_ವಾದಗಳು];
    ಪೂರ್ಣಾಂಕ		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_ಬಹುಲಕ_T	ch_ಬಹುಲಕ = ch_part->ch_ಬಹುಲಕ;
    cbq_T	*cbhead = &ch_part->ch_cb_ತಲೆ;
    cbq_T	*cbitem;
    callback_T	*callback = ಖಾಲಿ;
    buf_T	*ಬಫರ್ = ಖಾಲಿ;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		called_otc;		// one time callbackup
    ಪೂರ್ಣಾಂಕ		raw_len = 0;

    ಆದರೆ (channel->ch_nb_close_cb != ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#ಆದರೆ_ಅಂತ್ಯ
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ಹಿಂತಿರುಗಿ FALSE;

    // Use a message-specific callback, part callback or channel callback
    ಗಾಗಿ (cbitem = cbhead->cq_ಮುಂದಿನ; cbitem != ಖಾಲಿ; cbitem = cbitem->cq_ಮುಂದಿನ)
	ಆದರೆ (cbitem->cq_seq_nr == 0)
	    ನಿಲ್ಲಿಸು;
    ಆದರೆ (cbitem != ಖಾಲಿ)
	callback = &cbitem->cq_callback;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch_part->ch_callback.cb_name != ಖಾಲಿ)
	callback = &ch_part->ch_callback;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (channel->ch_callback.cb_name != ಖಾಲಿ)
	callback = &channel->ch_callback;

    ಬಫರ್ = ch_part->ch_bufref.br_buf;
    ಆದರೆ (ಬಫರ್ != ಖಾಲಿ && (!bufref_valid(&ch_part->ch_bufref)
					       || ಬಫರ್->b_ml.ml_mfp == ಖಾಲಿ))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_ಹೆಸರುಗಳು[part]);
	ch_part->ch_bufref.br_buf = ಖಾಲಿ;
	ಬಫರ್ = ಖಾಲಿ;
    }

    ಆದರೆ (channel_use_json_ತಲೆ(channel, part))
    {
	listitem_T	*ಅಂಶ;
	ಪೂರ್ಣಾಂಕ		argc = 0;

	// Get any json message in the queue.
	ಆದರೆ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(ಶೂನ್ಯ)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ಆದರೆ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		ಹಿಂತಿರುಗಿ FALSE;
	}

	ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ಆದರೆ (d != ಖಾಲಿ)
	    {
		ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP)
		    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"id", -1);
		ಇಲ್ಲದಿದ್ದರೆ
		    di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)"request_seq", -1);
		ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
		    seq_nr = di->di_tv.vval.v_ಸಂಖ್ಯೆ;
	    }

	    argv[1] = *listtv;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಗಾಗಿ (ಅಂಶ = listtv->vval.v_list->lv_ಮೊದಲ;
		    ಅಂಶ != ಖಾಲಿ && argc < CH_JSON_MAX_ವಾದಗಳು;
		    ಅಂಶ = ಅಂಶ->li_ಮುಂದಿನ)
		argv[argc++] = ಅಂಶ->li_tv;
	    ಆಗ (argc < CH_JSON_MAX_ವಾದಗಳು)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ಆದರೆ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ಹಿಂತಿರುಗಿ TRUE;
	    }

	    ಆದರೆ (argv[0].v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    seq_nr = argv[0].vval.v_ಸಂಖ್ಯೆ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (channel_peek(channel, part) == ಖಾಲಿ)
    {
	// nothing to read on RAW or NL channel
	ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// If there is no callback or buffer drop the message.
	ಆದರೆ (callback == ಖಾಲಿ && ಬಫರ್ == ಖಾಲಿ)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ಆದರೆ (channel->ch_close_cb.cb_name == ಖಾಲಿ && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_NL)
	{
	    char_u  *nl = ಖಾಲಿ;
	    char_u  *buf;
	    readq_T *ಗಂಟು;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ಆಗ (TRUE)
	    {
		ಗಂಟು = channel_peek(channel, part);
		nl = channel_ಮೊದಲ_nl(ಗಂಟು);
		ಆದರೆ (nl != ಖಾಲಿ)
		    ನಿಲ್ಲಿಸು;
		ಆದರೆ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ಆದರೆ (ch_part->ch_fd == INVALID_FD && ಗಂಟು->rq_buflen > 0)
			ನಿಲ್ಲಿಸು;
		    ಹಿಂತಿರುಗಿ FALSE; // incomplete message
		}
	    }
	    buf = ಗಂಟು->rq_ಬಫರ್;

	    // Convert NUL to NL, the internal representation.
	    ಗಾಗಿ (p = buf; (nl == ಖಾಲಿ || p < nl)
					    && p < buf + ಗಂಟು->rq_buflen; ++p)
		ಆದರೆ (*p == NUL)
		    *p = NL;

	    ಆದರೆ (nl == ಖಾಲಿ)
	    {
		// get the whole buffer, drop the NL
		ಸಂದೇಶ = channel_get(channel, part, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nl + 1 == buf + ಗಂಟು->rq_buflen)
	    {
		// get the whole buffer
		ಸಂದೇಶ = channel_get(channel, part, ಖಾಲಿ);
		*nl = NUL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		ಸಂದೇಶ = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (ಪೂರ್ಣಾಂಕ)(nl - buf) + 1);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    ಸಂದೇಶ = channel_get_all(channel, part,
			ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_BLOB ? &raw_len : ಖಾಲಿ);
	}

	ಆದರೆ (ಸಂದೇಶ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FALSE; // out of memory (and avoids Coverity warning)

	ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_BLOB)
	{
	    blob = blob_alloc();
	    ಆದರೆ (blob == ಖಾಲಿ)
	    {
		vim_free(ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ಆದರೆ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(ಸಂದೇಶ);
		ಹಿಂತಿರುಗಿ FALSE;
	    }
	    ಆದರೆ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_ದತ್ತಾಂಶ, ಸಂದೇಶ, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = ಸಂದೇಶ;
	}
    }

    called_otc = FALSE;
    ಆದರೆ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	ಪೂರ್ಣಾಂಕ lsp_req_ಸಂದೇಶ = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ಆದರೆ ((ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_ಕೀಲಿ(argv[1].vval.v_dict, "method"))
	    lsp_req_ಸಂದೇಶ = TRUE;

	ಆದರೆ (!lsp_req_ಸಂದೇಶ)
	{
	    ಗಾಗಿ (cbitem = cbhead->cq_ಮುಂದಿನ; cbitem != ಖಾಲಿ;
		    cbitem = cbitem->cq_ಮುಂದಿನ)
	    {
		ಆದರೆ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_ಸಮಯ_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    ನಿಲ್ಲಿಸು;
		}
	    }
	}
    }

    ಆದರೆ (seq_nr > 0 && ((ch_ಬಹುಲಕ != CH_ಬಹುಲಕ_LSP && ch_ಬಹುಲಕ != CH_ಬಹುಲಕ_DAP)
		|| called_otc))
    {
	ಆದರೆ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ಆದರೆ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ತಳ್ಳು_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ಸಂಖ್ಯೆ;
		free_tv(listtv);
		listtv = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (callback != ಖಾಲಿ || ಬಫರ್ != ಖಾಲಿ)
    {
	ಆದರೆ (ಬಫರ್ != ಖಾಲಿ)
	{
	    ಆದರೆ (ಸಂದೇಶ == ಖಾಲಿ)
		// JSON or JS mode: re-encode the message.
		ಸಂದೇಶ = json_encode(listtv, ch_ಬಹುಲಕ);
	    ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
		ಆದರೆ (ಬಫರ್->b_ಪದ != ಖಾಲಿ)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ಆದರೆ (part == PART_ERR)
		    {
			char_u *cp;
			ಪೂರ್ಣಾಂಕ    extra = 0;

			ಗಾಗಿ (cp = ಸಂದೇಶ; *cp != NUL; ++cp)
			    ಆದರೆ (*cp == NL && (cp == ಸಂದೇಶ || cp[-1] != CAR))
				++extra;
			ಆದರೆ (extra > 0)
			{
			    char_u *crlf_ಸಂದೇಶ = alloc(STRLEN(ಸಂದೇಶ) + extra + 1);

			    ಆದರೆ (crlf_ಸಂದೇಶ != ಖಾಲಿ)
			    {
				char_u *q = crlf_ಸಂದೇಶ;

				ಗಾಗಿ (cp = ಸಂದೇಶ; *cp != NUL; ++cp)
				{
				    ಆದರೆ (*cp == NL && (cp == ಸಂದೇಶ || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_ಗಮ್ಯ_ಪದ(ಬಫರ್, crlf_ಸಂದೇಶ, channel);
				vim_free(crlf_ಸಂದೇಶ);
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    write_ಗಮ್ಯ_ಪದ(ಬಫರ್, ಸಂದೇಶ, channel);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			write_ಗಮ್ಯ_ಪದ(ಬಫರ್, ಸಂದೇಶ, channel);
		}
		ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		    append_ಗಮ್ಯ_ಬಫರ್(ಬಫರ್, ಸಂದೇಶ, channel, part);
	    }
	}

	ಆದರೆ (callback != ಖಾಲಿ)
	{
	    ಆದರೆ (cbitem != ಖಾಲಿ)
		invoke_one_ಸಮಯ_callback(channel, cbhead, cbitem, argv);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (ಅಕ್ಷರ *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ch_log(channel, "Dropping message %d", seq_nr);

    ಆದರೆ (listtv != ಖಾಲಿ)
	free_tv(listtv);
    ಆದರೆ (blob != ಖಾಲಿ)
	blob_unref(blob);
    vim_free(ಸಂದೇಶ);

    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    ಪೂರ್ಣಾಂಕ
channel_can_write_ಗಮ್ಯ(channel_T *channel)
{
    ಹಿಂತಿರುಗಿ channel != ಖಾಲಿ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_ಒಳ_FD != INVALID_FD);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    ಪೂರ್ಣಾಂಕ
channel_is_open(channel_T *channel)
{
    ಹಿಂತಿರುಗಿ channel != ಖಾಲಿ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_ಒಳ_FD != INVALID_FD
			  || channel->CH_ಹೊರಹೋಗು_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
channel_readahead_ಸೂಚಕ(channel_T *channel, ch_part_T part)
{
    ಆದರೆ (channel_use_json_ತಲೆ(channel, part))
    {
	jsonq_T   *ತಲೆ = &channel->ch_part[part].ch_json_ತಲೆ;

	ಆದರೆ (ತಲೆ->jq_ಮುಂದಿನ == ಖಾಲಿ)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ಹಿಂತಿರುಗಿ ತಲೆ->jq_ಮುಂದಿನ;
    }
    ಹಿಂತಿರುಗಿ channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ಹಿಂತಿರುಗಿ channel_readahead_ಸೂಚಕ(channel, part) != ಖಾಲಿ;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ಸ್ಥಿರ ಅಕ್ಷರ *
channel_ಸ್ಥಿತಿ(channel_T *channel, ಪೂರ್ಣಾಂಕ req_part)
{
    ch_part_T part;
    ಪೂರ್ಣಾಂಕ has_readahead = FALSE;

    ಆದರೆ (channel == ಖಾಲಿ)
	 ಹಿಂತಿರುಗಿ "fail";
    ಆದರೆ (req_part == PART_ಹೊರಹೋಗು)
    {
	ಆದರೆ (channel->CH_ಹೊರಹೋಗು_FD != INVALID_FD)
	    ಹಿಂತಿರುಗಿ "open";
	ಆದರೆ (channel_has_readahead(channel, PART_ಹೊರಹೋಗು))
	    has_readahead = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (req_part == PART_ERR)
    {
	ಆದರೆ (channel->CH_ERR_FD != INVALID_FD)
	    ಹಿಂತಿರುಗಿ "open";
	ಆದರೆ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (channel_is_open(channel))
	    ಹಿಂತಿರುಗಿ "open";
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	    ಆದರೆ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
    }

    ಆದರೆ (has_readahead)
	ಹಿಂತಿರುಗಿ "buffered";
    ಹಿಂತಿರುಗಿ "closed";
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_part_info(channel_T *channel, dict_T *dict, ಅಕ್ಷರ *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    ಅಕ್ಷರ	namebuf[20];  // longest is "sock_timeout"
    size_t	ಬಾಲ;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    ಬಾಲ = STRLEN(namebuf);
    STRCPY(namebuf + ಬಾಲ, "_");
    ಬಾಲ += STRLEN_LITERAL("_");

    STRCPY(namebuf + ಬಾಲ, "status");
    ಆದರೆ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_ಗಣ(s, "open");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (channel_has_readahead(channel, part))
	STR_LITERAL_ಗಣ(s, "buffered");
    ಇಲ್ಲದಿದ್ದರೆ
	STR_LITERAL_ಗಣ(s, "closed");
    dict_ಕೂಡಿಸು_string_len(dict, namebuf, s.string, (ಪೂರ್ಣಾಂಕ)s.ಉದ್ದಳತೆ);

    STRCPY(namebuf + ಬಾಲ, "mode");
    ಆಯ್ಕೆ (chanpart->ch_ಬಹುಲಕ)
    {
	ಸ್ಥಿತಿ CH_ಬಹುಲಕ_NL:
	    STR_LITERAL_ಗಣ(s, "NL");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CH_ಬಹುಲಕ_RAW:
	    STR_LITERAL_ಗಣ(s, "RAW");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CH_ಬಹುಲಕ_BLOB:
	    STR_LITERAL_ಗಣ(s, "BLOB");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CH_ಬಹುಲಕ_JSON:
	    STR_LITERAL_ಗಣ(s, "JSON");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CH_ಬಹುಲಕ_JS:
	    STR_LITERAL_ಗಣ(s, "JS");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CH_ಬಹುಲಕ_LSP:
	    STR_LITERAL_ಗಣ(s, "LSP");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CH_ಬಹುಲಕ_DAP:
	    STR_LITERAL_ಗಣ(s, "DAP");
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    STR_LITERAL_ಗಣ(s, "");
	    ನಿಲ್ಲಿಸು;
    }
    dict_ಕೂಡಿಸು_string_len(dict, namebuf, s.string, (ಪೂರ್ಣಾಂಕ)s.ಉದ್ದಳತೆ);

    STRCPY(namebuf + ಬಾಲ, "io");
    ಆದರೆ (part == PART_SOCK)
	STR_LITERAL_ಗಣ(s, "socket");
    ಇಲ್ಲದಿದ್ದರೆ ಆಯ್ಕೆ (chanpart->ch_io)
    {
	ಸ್ಥಿತಿ JIO_NULL:
	    STR_LITERAL_ಗಣ(s, "null");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ JIO_PIPE:
	    STR_LITERAL_ಗಣ(s, "pipe");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ JIO_FILE:
	    STR_LITERAL_ಗಣ(s, "file");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ JIO_ಬಫರ್:
	    STR_LITERAL_ಗಣ(s, "buffer");
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ JIO_ಹೊರಹೋಗು:
	    STR_LITERAL_ಗಣ(s, "out");
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    STR_LITERAL_ಗಣ(s, "");
	    ನಿಲ್ಲಿಸು;
    }
    dict_ಕೂಡಿಸು_string_len(dict, namebuf, s.string, (ಪೂರ್ಣಾಂಕ)s.ಉದ್ದಳತೆ);

    STRCPY(namebuf + ಬಾಲ, "timeout");
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, namebuf, chanpart->ch_timeout);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_info(channel_T *channel, dict_T *dict)
{
    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "id", channel->ch_id);
    dict_ಕೂಡಿಸು_string(dict, "status", (char_u *)channel_ಸ್ಥಿತಿ(channel, -1));

    ಆದರೆ (channel->ch_hostname != ಖಾಲಿ)
    {
	ಆದರೆ (channel->ch_ಪೋರ್ಟ್)
	    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(dict, "port", channel->ch_ಪೋರ್ಟ್);
	ಇಲ್ಲದಿದ್ದರೆ
	    // Unix-domain socket.
	    dict_ಕೂಡಿಸು_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	channel_part_info(channel, dict, "out", PART_ಹೊರಹೋಗು);
	channel_part_info(channel, dict, "err", PART_ERR);
	channel_part_info(channel, dict, "in", PART_ಒಳ);
    }
}

/*
 * Close channel "channel".
 * Trigger the close callback if "invoke_close_cb" is TRUE.
 * Does not clear the buffers.
 */
    ಶೂನ್ಯ
channel_close(channel_T *channel, ಪೂರ್ಣಾಂಕ invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    channel_gui_unregister(channel);
#ಆದರೆ_ಅಂತ್ಯ

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_ಒಳ);
    ch_close_part(channel, PART_ಹೊರಹೋಗು);
    ch_close_part(channel, PART_ERR);

    ಆದರೆ (invoke_close_cb)
    {
	ch_part_T	part;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	ಪದ_channel_closing(channel);
#ಆದರೆ_ಅಂತ್ಯ
	// Invoke callbacks and flush buffers before the close callback.
	ಆದರೆ (channel->ch_close_cb.cb_name != ಖಾಲಿ)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	{
	    ಆದರೆ (channel->ch_close_cb.cb_name != ಖಾಲಿ
			    || channel->ch_part[part].ch_bufref.br_buf != ಖಾಲಿ)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ಆದರೆ (channel->ch_close_cb.cb_name == ಖಾಲಿ)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_ಹೆಸರುಗಳು[part]);
		ಆಗ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ಆದರೆ (channel->ch_close_cb.cb_name != ಖಾಲಿ)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (ಅಕ್ಷರ *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      ಆದರೆ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ಆದರೆ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ಖಾಲಿ;
    channel->ch_ss_close_cb = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
    ಪದ_channel_closed(channel);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Close the "in" part channel "channel".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_close_ಒಳ(channel_T *channel)
{
    ch_close_part(channel, PART_ಒಳ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_from_writeque(writeq_T *wq, writeq_T *ನಮೂದು)
{
    ga_clear(&ನಮೂದು->wq_ga);
    wq->wq_ಮುಂದಿನ = ನಮೂದು->wq_ಮುಂದಿನ;
    ಆದರೆ (wq->wq_ಮುಂದಿನ == ಖಾಲಿ)
	wq->wq_prev = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	wq->wq_ಮುಂದಿನ->wq_prev = ಖಾಲಿ;
    vim_free(ನಮೂದು);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_ತಲೆ = &ch_part->ch_json_ತಲೆ;
    cbq_T   *cb_ತಲೆ = &ch_part->ch_cb_ತಲೆ;

    ಆಗ (channel_peek(channel, part) != ಖಾಲಿ)
	vim_free(channel_get(channel, part, ಖಾಲಿ));

    ಆಗ (cb_ತಲೆ->cq_ಮುಂದಿನ != ಖಾಲಿ)
    {
	cbq_T *ಗಂಟು = cb_ತಲೆ->cq_ಮುಂದಿನ;

	remove_cb_ಗಂಟು(cb_ತಲೆ, ಗಂಟು);
	free_callback(&ಗಂಟು->cq_callback);
	vim_free(ಗಂಟು);
    }

    ಆಗ (json_ತಲೆ->jq_ಮುಂದಿನ != ಖಾಲಿ)
    {
	free_tv(json_ತಲೆ->jq_ಮುಂದಿನ->jq_ಮೌಲ್ಯ);
	remove_json_ಗಂಟು(json_ತಲೆ, json_ತಲೆ->jq_ಮುಂದಿನ);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_ಖಂಡ_ids);

    ಆಗ (ch_part->ch_writeque.wq_ಮುಂದಿನ != ಖಾಲಿ)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_ಮುಂದಿನ);
}

/*
 * Clear all the read buffers on "channel".
 */
    ಶೂನ್ಯ
channel_clear(channel_T *channel)
{
    ch_log(channel, "Clearing channel");
    VIM_CLEAR(channel->ch_hostname);
    channel_clear_one(channel, PART_SOCK);
    channel_clear_one(channel, PART_ಹೊರಹೋಗು);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_ಒಳ);
    free_callback(&channel->ch_callback);
    free_callback(&channel->ch_close_cb);
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
channel_free_all(ಶೂನ್ಯ)
{
    channel_T *channel;

    ch_log(ಖಾಲಿ, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#ಆದರೆ_ಅಂತ್ಯ


// Sent when the netbeans channel is found closed when reading.
#ವ್ಯಾಖ್ಯಾನಿಸು DETACH_ಸಂದೇಶ_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#ವ್ಯಾಖ್ಯಾನಿಸು MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_channel_write_ಉಳಿದ(chanpart_T *ಒಳ_part)
{
    buf_T *buf = ಒಳ_part->ch_bufref.br_buf;

    ಆದರೆ (ಒಳ_part->ch_writeque.wq_ಮುಂದಿನ != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ ಒಳ_part->ch_buf_append
	    ? (ಒಳ_part->ch_buf_bot < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    : (ಒಳ_part->ch_buf_ತುದಿ <= ಒಳ_part->ch_buf_bot
			    && ಒಳ_part->ch_buf_ತುದಿ <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

#ಆದರೆ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_fill_wfds(ಪೂರ್ಣಾಂಕ maxfd_arg, fd_set *wfds)
{
    ಪೂರ್ಣಾಂಕ		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *ಒಳ_part = &ch->ch_part[PART_ಒಳ];

	ಆದರೆ (ಒಳ_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (ಪೂರ್ಣಾಂಕ)ಒಳ_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_ಉಳಿದ(ಒಳ_part))
	{
	    FD_SET((ಪೂರ್ಣಾಂಕ)ಒಳ_part->ch_fd, wfds);
	    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)ಒಳ_part->ch_fd >= maxfd)
		maxfd = (ಪೂರ್ಣಾಂಕ)ಒಳ_part->ch_fd + 1;
	}
    }
    ಹಿಂತಿರುಗಿ maxfd;
}
#ಇಲ್ಲದಿದ್ದರೆ
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_fill_poll_write(ಪೂರ್ಣಾಂಕ nfd_ಒಳ, ರಚನೆ pollfd *fds)
{
    ಪೂರ್ಣಾಂಕ		nfd = nfd_ಒಳ;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *ಒಳ_part = &ch->ch_part[PART_ಒಳ];

	ಆದರೆ (ಒಳ_part->ch_fd != INVALID_FD
		&& is_channel_write_ಉಳಿದ(ಒಳ_part))
	{
	    ಒಳ_part->ch_poll_idx = nfd;
	    fds[nfd].fd = ಒಳ_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಒಳ_part->ch_poll_idx = -1;
    }
    ಹಿಂತಿರುಗಿ nfd;
}
#ಆದರೆ_ಅಂತ್ಯ

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
    CW_READY,
    CW_NOT_READY,
    CW_ERROR
} channel_wait_ಫಲಿತಾಂಶ;

/*
 * Check for reading from "fd" with "timeout" msec.
 * Return CW_READY when there is something to read.
 * Return CW_NOT_READY when there is nothing to read.
 * Return CW_ERROR when there is an error.
 */
    ಸ್ಥಿರ channel_wait_ಫಲಿತಾಂಶ
channel_wait(channel_T *channel, sock_T fd, ಪೂರ್ಣಾಂಕ timeout)
{
    ಆದರೆ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    ಆದರೆ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	ಪೂರ್ಣಾಂಕ	sleep_ಸಮಯ;
	DWORD	deadline = Get_Tick_ಎಣಿಕೆ() + timeout;
	ಪೂರ್ಣಾಂಕ	delay = 1;

	// reading from a pipe, not a socket
	ಆಗ (TRUE)
	{
	    ಪೂರ್ಣಾಂಕ r = PeekNamedPipe((HANDLE)fd, ಖಾಲಿ, 0, ಖಾಲಿ, &nread, ಖಾಲಿ);

	    ಆದರೆ (r && nread > 0)
		ಹಿಂತಿರುಗಿ CW_READY;

	    ಆದರೆ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == 0)
		ಹಿಂತಿರುಗಿ CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_ಸಾಲುಗಳು();

	    sleep_ಸಮಯ = deadline - Get_Tick_ಎಣಿಕೆ();
	    ಆದರೆ (sleep_ಸಮಯ <= 0)
		ನಿಲ್ಲಿಸು;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ಆದರೆ (sleep_ಸಮಯ > delay)
		sleep_ಸಮಯ = delay;
	    Sleep(sleep_ಸಮಯ);
	    delay = delay * 2;
	    ಆದರೆ (delay > 10)
		delay = 10;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
#ಆದರೆ defined(HAVE_SELECT)
	ರಚನೆ timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	ಪೂರ್ಣಾಂಕ		ret;
	ಪೂರ್ಣಾಂಕ		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	ಗಾಗಿ (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((ಪೂರ್ಣಾಂಕ)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (ಪೂರ್ಣಾಂಕ)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ಖಾಲಿ, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ಆದರೆ (ret == -1 && errno == EINTR)
		ಮುಂದುವರಿ;
# endif
	    ಆದರೆ (ret > 0)
	    {
		ಆದರೆ (FD_ISSET(fd, &rfds))
		    ಹಿಂತಿರುಗಿ CW_READY;
		channel_write_any_ಸಾಲುಗಳು();
		ಮುಂದುವರಿ;
	    }
	    ನಿಲ್ಲಿಸು;
	}
#ಇಲ್ಲದಿದ್ದರೆ
	ಗಾಗಿ (;;)
	{
	    ರಚನೆ pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    ಪೂರ್ಣಾಂಕ		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ಆದರೆ (poll(fds, nfd, timeout) > 0)
	    {
		ಆದರೆ (fds[0].revents & POLLIN)
		    ಹಿಂತಿರುಗಿ CW_READY;
		channel_write_any_ಸಾಲುಗಳು();
		ಮುಂದುವರಿ;
	    }
	    ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ CW_NOT_READY;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಂಕ is_err, ಅಕ್ಷರ *func)
{
    ಅಕ್ಷರ	ಸಂದೇಶ[] = "%s(): Read %s from ch_part[%d], closing";

    ಆದರೆ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, ಸಂದೇಶ, func, "error", part);
    ಇಲ್ಲದಿದ್ದರೆ
	ch_log(channel, ಸಂದೇಶ, func, "EOF", part);

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
    ಆದರೆ (channel->ch_nb_close_cb != ಖಾಲಿ)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_ಸಂದೇಶ_RAW,
			      (ಪೂರ್ಣಾಂಕ)STRLEN_LITERAL(DETACH_ಸಂದೇಶ_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ಆದರೆ (part == PART_ಹೊರಹೋಗು || part == PART_ERR)
    {
	ch_part_T other = part == PART_ಹೊರಹೋಗು ? PART_ERR : PART_ಹೊರಹೋಗು;

	ಆದರೆ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    ಆದರೆ (channel->ch_nb_close_cb != ಖಾಲಿ)
	(*channel->ch_nb_close_cb)();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
    ಆದರೆ (channel->ch_ss_close_cb != ಖಾಲಿ)
	channel->ch_ss_close_cb(channel);
#ಆದರೆ_ಅಂತ್ಯ
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_read(channel_T *channel, ch_part_T part, ಅಕ್ಷರ *func)
{
    ಸ್ಥಿರ char_u	*buf = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			len = 0;
    ಪೂರ್ಣಾಂಕ			readlen = 0;
    sock_T		fd;
    ಪೂರ್ಣಾಂಕ			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ಆದರೆ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_ಹೆಸರುಗಳು[part]);
	ಹಿಂತಿರುಗಿ;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ಆದರೆ (buf == ಖಾಲಿ)
    {
	buf = alloc(MAXMSGSIZE);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    ಗಾಗಿ (;;)
    {
	ಆದರೆ (channel_wait(channel, fd, 0) != CW_READY)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    ರಚನೆ sockaddr_storage	client;

	    newchannel = ಕೂಡಿಸು_channel();
	    ಆದರೆ (newchannel == ಖಾಲಿ)
	    {
		ch_error(ಖಾಲಿ, "Cannot allocate channel.");
		ಹಿಂತಿರುಗಿ;
	    }
	    socklen = ಗಾತ್ರ(client);
	    newfd = accept(fd, (ರಚನೆ sockaddr*)&client, &socklen);
	    ಆದರೆ (newfd < 0)
	    {
		ch_error(ಖಾಲಿ, "Cannot accept channel.");
		channel_free(newchannel);
		ಹಿಂತಿರುಗಿ;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_ಗಮ್ಯ_be_closed |= (1U << PART_SOCK);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    channel_gui_ರಿಜಿಸ್ಟರ್_one(newchannel, PART_SOCK);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER
	    ಆದರೆ (channel->ch_ss_accept_cb != ಖಾಲಿ)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ಹಿಂತಿರುಗಿ;
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    ಆದರೆ (client.ss_family == AF_INET)
	    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_INET_NTOP
		ಅಕ್ಷರ addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((ರಚನೆ sockaddr_ಒಳ*)&client)->sin_addr,
			addr, ಗಾತ್ರ(addr));
		vim_snprintf((ಅಕ್ಷರ *)namebuf, ಗಾತ್ರ(namebuf), "%s:%d",
			addr,
			ntohs(((ರಚನೆ sockaddr_ಒಳ*)&client)->sin_ಪೋರ್ಟ್));
#ಇಲ್ಲದಿದ್ದರೆ
		vim_snprintf((ಅಕ್ಷರ *)namebuf, ಗಾತ್ರ(namebuf), "%s:%d",
		    inet_ntoa(((ರಚನೆ sockaddr_ಒಳ*)&client)->sin_addr),
		    ntohs(((ರಚನೆ sockaddr_ಒಳ*)&client)->sin_ಪೋರ್ಟ್));
#ಆದರೆ_ಅಂತ್ಯ
	    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_INET_NTOP
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (client.ss_family == AF_INET6)
	    {
		ಅಕ್ಷರ addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((ರಚನೆ sockaddr_in6*)&client)->sin6_addr,
			addr, ಗಾತ್ರ(addr));
		vim_snprintf((ಅಕ್ಷರ *)namebuf, ಗಾತ್ರ(namebuf), "[%s]:%d",
			addr,
			ntohs(((ರಚನೆ sockaddr_in6*)&client)->sin6_ಪೋರ್ಟ್));
	    }
#ಆದರೆ_ಅಂತ್ಯ
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (client.ss_family == AF_UNIX)
		vim_snprintf((ಅಕ್ಷರ *)namebuf, ಗಾತ್ರ(namebuf),
							    "unix:anonymous");
	    ಇಲ್ಲದಿದ್ದರೆ
		vim_snprintf((ಅಕ್ಷರ *)namebuf, ಗಾತ್ರ(namebuf), "unknown");
	    ++ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback;
	    clear_tv(&argv[1]);
	    ಹಿಂತಿರುಗಿ;
	}
	ಆದರೆ (use_socket)
	    len = sock_read(fd, (ಅಕ್ಷರ *)buf, MAXMSGSIZE);
	ಇಲ್ಲದಿದ್ದರೆ
	    len = fd_read(fd, (ಅಕ್ಷರ *)buf, MAXMSGSIZE);
	ಆದರೆ (len <= 0)
	    ನಿಲ್ಲಿಸು;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ಆದರೆ (readlen <= 0)
    {
	ಆದರೆ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ಆದರೆ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SOCKETSERVER

    ಶೂನ್ಯ
channel_check(channel_T *channel, ch_part_T part)
{
    channel_read(channel, part, "channel_check");
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Read from RAW or NL "channel"/"part".  Blocks until there is something to
 * read or the timeout expires.
 * When "raw" is TRUE don't block waiting on a NL.
 * Does not trigger timers or handle messages.
 * Returns what was read in allocated memory.
 * Returns NULL in case of error or timeout.
 */
    ಸ್ಥಿರ char_u *
channel_read_ಖಂಡ(
	channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಂಕ timeout, ಪೂರ್ಣಾಂಕ raw, ಪೂರ್ಣಾಂಕ *outlen)
{
    char_u	*buf;
    char_u	*ಸಂದೇಶ;
    ch_ಬಹುಲಕ_T	ಬಹುಲಕ = channel->ch_part[part].ch_ಬಹುಲಕ;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*ಗಂಟು;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  ಬಹುಲಕ == CH_ಬಹುಲಕ_RAW ? "RAW"
				: ಬಹುಲಕ == CH_ಬಹುಲಕ_BLOB ? "BLOB" : "NL", timeout);

    ಆಗ (TRUE)
    {
	ಗಂಟು = channel_peek(channel, part);
	ಆದರೆ (ಗಂಟು != ಖಾಲಿ)
	{
	    ಆದರೆ (ಬಹುಲಕ == CH_ಬಹುಲಕ_RAW || ಬಹುಲಕ == CH_ಬಹುಲಕ_BLOB
		    || (ಬಹುಲಕ == CH_ಬಹುಲಕ_NL
					   && channel_ಮೊದಲ_nl(ಗಂಟು) != ಖಾಲಿ))
		// got a complete message
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (channel_collapse(channel, part, ಬಹುಲಕ == CH_ಬಹುಲಕ_NL) == OK)
		ಮುಂದುವರಿ;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ಆದರೆ (raw || fd == INVALID_FD)
		ನಿಲ್ಲಿಸು;
	}

	// Wait for up to the channel timeout.
	ಆದರೆ (fd == INVALID_FD)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಆದರೆ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ಆದರೆ (ಬಹುಲಕ == CH_ಬಹುಲಕ_RAW || ಬಹುಲಕ == CH_ಬಹುಲಕ_BLOB || outlen != ಖಾಲಿ)
    {
	ಸಂದೇಶ = channel_get_all(channel, part, outlen);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	char_u *p;

	buf = ಗಂಟು->rq_ಬಫರ್;
	nl = channel_ಮೊದಲ_nl(ಗಂಟು);

	// Convert NUL to NL, the internal representation.
	ಗಾಗಿ (p = buf; (nl == ಖಾಲಿ || p < nl) && p < buf + ಗಂಟು->rq_buflen; ++p)
	    ಆದರೆ (*p == NUL)
		*p = NL;

	ಆದರೆ (nl == ಖಾಲಿ)
	{
	    // must be a closed channel with missing NL
	    ಸಂದೇಶ = channel_get(channel, part, ಖಾಲಿ);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (nl + 1 == buf + ಗಂಟು->rq_buflen)
	{
	    // get the whole buffer
	    ಸಂದೇಶ = channel_get(channel, part, ಖಾಲಿ);
	    *nl = NUL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    ಸಂದೇಶ = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (ಪೂರ್ಣಾಂಕ)(nl - buf) + 1);
	}
    }
    ಆದರೆ (ch_log_ಸಕ್ರಿಯ())
	ch_log(channel, "Returning %d bytes",
		outlen != ಖಾಲಿ ? *outlen : (ಪೂರ್ಣಾಂಕ)STRLEN(ಸಂದೇಶ));
    ಹಿಂತಿರುಗಿ ಸಂದೇಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    ಪೂರ್ಣಾಂಕ
channel_ಒಳ_blocking_wait(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_wait_reading(channel_T *channel ಬಳಸದ, sock_T fd ಬಳಸದ, ಪೂರ್ಣಾಂಕ timeout)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (ಶೂನ್ಯ)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
    channel_wait_any(timeout);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Read one JSON message with ID "id" from "channel"/"part" and store the
 * result in "rettv".
 * When "id" is -1 accept any message;
 * Blocks until the message is received or the timeout is reached.
 * In corner cases this can be called recursively, that is why ch_block_ids is
 * a list.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_read_json_ಖಂಡ(
	channel_T   *channel,
	ch_part_T   part,
	ಪೂರ್ಣಾಂಕ	    timeout_arg,
	ಪೂರ್ಣಾಂಕ	    id,
	typval_T    **rettv)
{
    ಪೂರ್ಣಾಂಕ		ಇನ್ನಷ್ಟು;
    sock_T	fd;
    ಪೂರ್ಣಾಂಕ		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_ಬಹುಲಕ_T	ಬಹುಲಕ = channel->ch_part[part].ch_ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    elapsed_T	ಆರಂಭ_tv;

    ELAPSED_INIT(ಆರಂಭ_tv);
#ಆದರೆ_ಅಂತ್ಯ

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ಆದರೆ (id >= 0)
	channel_ಕೂಡಿಸು_ಖಂಡ_id(chanpart, id);

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (ಶೂನ್ಯ)channel_collapse(channel, part, FALSE);

	ಇನ್ನಷ್ಟು = channel_parse_json(channel, part, false);

	// search for message "id"
	ಆದರೆ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (!ಇನ್ನಷ್ಟು)
	{
	    ಶೂನ್ಯ *prev_readahead_ptr = channel_readahead_ಸೂಚಕ(channel, part);
	    ಶೂನ್ಯ *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ಆದರೆ (channel_parse_messages())
		ಮುಂದುವರಿ;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_ಸೂಚಕ(channel, part);
	    ಆದರೆ (readahead_ptr != ಖಾಲಿ && readahead_ptr != prev_readahead_ptr)
		ಮುಂದುವರಿ;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
	    timeout -= (ಪೂರ್ಣಾಂಕ)ELAPSED_FUNC(ಆರಂಭ_tv);
#ಆದರೆ_ಅಂತ್ಯ
	    fd = chanpart->ch_fd;
	    ಆದರೆ (timeout <= 0 || fd == INVALID_FD)
	    {
		ಆದರೆ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		ನಿಲ್ಲಿಸು;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ಆದರೆ (chanpart->ch_wait_len > 0)
	    {
		ಪೂರ್ಣಾಂಕ	ಎಡ_2;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
		ಎಡ_2 = chanpart->ch_deadline - Get_Tick_ಎಣಿಕೆ() + 1;
#ಇಲ್ಲದಿದ್ದರೆ
		{
		    ರಚನೆ timeval now_tv;

		    gettimeofday(&now_tv, ಖಾಲಿ);
		    ಎಡ_2 = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ಆದರೆ (ಎಡ_2 < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಎಡ_2 < timeout)
		    timeout = ಎಡ_2;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
    ಆದರೆ (id >= 0)
	channel_remove_ಖಂಡ_id(chanpart, id);
    --channel_blocking_wait;

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, ಪೂರ್ಣಾಂಕ check_open, ಪೂರ್ಣಾಂಕ reading, ch_part_T part)
{
    channel_T	*channel = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		has_readahead = FALSE;

    ಆದರೆ (tv->v_type == VAR_ಕೆಲಸ)
    {
	ಆದರೆ (tv->vval.v_ಕೆಲಸ != ಖಾಲಿ)
	    channel = tv->vval.v_ಕೆಲಸ->jv_channel;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_invalid_ವಾದ_str), tv_get_string(tv));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (channel != ಖಾಲಿ && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_ಎಣಿಕೆ ? part : channel_part_read(channel));

    ಆದರೆ (check_open && (channel == ಖಾಲಿ || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
common_channel_read(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ raw, ಪೂರ್ಣಾಂಕ blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_ಎಣಿಕೆ;
    jobopt_T	opt;
    ಪೂರ್ಣಾಂಕ		ಬಹುಲಕ;
    ಪೂರ್ಣಾಂಕ		timeout;
    ಪೂರ್ಣಾಂಕ		id = -1;
    typval_T	*listtv = ಖಾಲಿ;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    clear_ಕೆಲಸ_options(&opt);
    ಆದರೆ (get_ಕೆಲಸ_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	ಹೋಗು theend;

    ಆದರೆ (opt.jo_ಗಣ & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹೋಗು theend;

    ಆದರೆ (part == PART_ಎಣಿಕೆ)
	part = channel_part_read(channel);
    ಬಹುಲಕ = channel_get_ಬಹುಲಕ(channel, part);
    timeout = channel_get_timeout(channel, part);
    ಆದರೆ (opt.jo_ಗಣ & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ಆದರೆ (blob || ಬಹುಲಕ == CH_ಬಹುಲಕ_BLOB)
    {
	ಪೂರ್ಣಾಂಕ	    outlen = 0;
	char_u  *p = channel_read_ಖಂಡ(channel, part,
		timeout, TRUE, &outlen);
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    blob_T	*b = blob_alloc();

	    ಆದರೆ (b != ಖಾಲಿ)
	    {
		b->bv_ga.ga_len = outlen;
		ಆದರೆ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಸ್ಮರಣೆ_ನಕಲು(b->bv_ga.ga_ದತ್ತಾಂಶ, p, outlen);
		    rettv_blob_ಗಣ(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (raw || ಬಹುಲಕ == CH_ಬಹುಲಕ_RAW || ಬಹುಲಕ == CH_ಬಹುಲಕ_NL)
	rettv->vval.v_string = channel_read_ಖಂಡ(channel, part,
		timeout, raw, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (opt.jo_ಗಣ & JO_ID)
	    id = opt.jo_id;
	channel_read_json_ಖಂಡ(channel, part, timeout, id, &listtv);
	ಆದರೆ (listtv != ಖಾಲಿ)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
	}
    }

theend:
    free_ಕೆಲಸ_options(&opt);
}

#ಆದರೆ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    ಶೂನ್ಯ
channel_handle_events(ಪೂರ್ಣಾಂಕ only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ಆದರೆ (only_keep_open && !channel->ch_keep_open)
	    ಮುಂದುವರಿ;

	// check the socket and pipes
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ಆದರೆ (fd == INVALID_FD)
		ಮುಂದುವರಿ;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    ಪೂರ್ಣಾಂಕ r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_ಹೊರಹೋಗು || part == PART_ERR)) ? 1 : 0);

	    ಆದರೆ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ಆದರೆ (channel->ch_ಕೆಲಸ != ಖಾಲಿ)
	{
	    ಕೆಲಸ_T *ಕೆಲಸ = channel->ch_ಕೆಲಸ;

	    ಆದರೆ (ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು != ಖಾಲಿ && ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ)
		ಗಾಗಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    ಪೂರ್ಣಾಂಕ
channel_any_keep_open(ಶೂನ್ಯ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ಆದರೆ (channel->ch_keep_open)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    ಶೂನ್ಯ
channel_ಗಣ_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    ಪೂರ್ಣಾಂಕ		fd = ch_part->ch_fd;

    ಆದರೆ (fd == INVALID_FD)
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#ಇಲ್ಲದಿದ್ದರೆ
    (ಶೂನ್ಯ)fcntl(fd, F_SETFL, O_NONBLOCK);
#ಆದರೆ_ಅಂತ್ಯ
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	ಪೂರ್ಣಾಂಕ	  len_arg,
	ಅಕ್ಷರ	  *fun)
{
    ಪೂರ್ಣಾಂಕ		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ಪೂರ್ಣಾಂಕ		did_use_ಸರತಿ = FALSE;

    fd = ch_part->ch_fd;
    ಆದರೆ (fd == INVALID_FD)
    {
	ಆದರೆ (!channel->ch_error && fun != ಖಾಲಿ)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_ಗಣ_nonblock(channel, part);

    ಆದರೆ (ch_log_ಸಕ್ರಿಯ())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_ಸಂದೇಶ = 0;
    }

    ಗಾಗಿ (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	ಪೂರ್ಣಾಂಕ	    len;

	ಆದರೆ (wq->wq_ಮುಂದಿನ != ಖಾಲಿ)
	{
	    // first write what was queued
	    buf = wq->wq_ಮುಂದಿನ->wq_ga.ga_ದತ್ತಾಂಶ;
	    len = wq->wq_ಮುಂದಿನ->wq_ga.ga_len;
	    did_use_ಸರತಿ = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ಹಿಂತಿರುಗಿ OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ಆದರೆ (part == PART_SOCK)
	    res = sock_write(fd, (ಅಕ್ಷರ *)buf, len);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    res = fd_write(fd, (ಅಕ್ಷರ *)buf, len);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
	    ಆದರೆ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ಖಾಲಿ);
	    }
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಆದರೆ (res < 0 && (errno == EWOULDBLOCK
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EAGAIN
			|| errno == EAGAIN
#ಆದರೆ_ಅಂತ್ಯ
		    ))
	    res = 0; // nothing got written

	ಆದರೆ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *ನಮೂದು = wq->wq_ಮುಂದಿನ;

	    ಆದರೆ (did_use_ಸರತಿ)
		ch_log(channel, "Sent %d bytes now", res);
	    ಆದರೆ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, ನಮೂದು);
		    ಮುಂದುವರಿ;
		}
		ಆದರೆ (did_use_ಸರತಿ)
		    ch_log(channel, "Write queue empty");
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
		{
		    ಆದರೆ (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(ನಮೂದು->wq_ga.ga_ದತ್ತಾಂಶ,
				    (ಅಕ್ಷರ *)ನಮೂದು->wq_ga.ga_ದತ್ತಾಂಶ + res,
				    len - res);
			ನಮೂದು->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ಆದರೆ (wq->wq_prev != ಖಾಲಿ
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *ಕೊನೆಯ = wq->wq_prev;

		    // append to the last entry
		    ಆದರೆ (len > 0 && ga_grow(&ಕೊನೆಯ->wq_ga, len) == OK)
		    {
			mch_memmove((ಅಕ್ಷರ *)ಕೊನೆಯ->wq_ga.ga_ದತ್ತಾಂಶ
							  + ಕೊನೆಯ->wq_ga.ga_len,
				    buf, len);
			ಕೊನೆಯ->wq_ga.ga_len += len;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    writeq_T *ಕೊನೆಯ = ALLOC_ONE(writeq_T);

		    ಆದರೆ (ಕೊನೆಯ != ಖಾಲಿ)
		    {
			ಕೊನೆಯ->wq_prev = wq->wq_prev;
			ಕೊನೆಯ->wq_ಮುಂದಿನ = ಖಾಲಿ;
			ಆದರೆ (wq->wq_prev == ಖಾಲಿ)
			    wq->wq_ಮುಂದಿನ = ಕೊನೆಯ;
			ಇಲ್ಲದಿದ್ದರೆ
			    wq->wq_prev->wq_ಮುಂದಿನ = ಕೊನೆಯ;
			wq->wq_prev = ಕೊನೆಯ;
			ga_init2(&ಕೊನೆಯ->wq_ga, 1, 1000);
			ಆದರೆ (len > 0 && ga_grow(&ಕೊನೆಯ->wq_ga, len) == OK)
			{
			    mch_memmove(ಕೊನೆಯ->wq_ga.ga_ದತ್ತಾಂಶ, buf, len);
			    ಕೊನೆಯ->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (res != len)
	{
	    ಆದರೆ (!channel->ch_error && fun != ಖಾಲಿ)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	channel->ch_error = FALSE;
	ಹಿಂತಿರುಗಿ OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    ಸ್ಥಿರ channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *ಪಠ್ಯ,
	ಪೂರ್ಣಾಂಕ	    len,
	ಪೂರ್ಣಾಂಕ	    id,
	ಪೂರ್ಣಾಂಕ	    eval,
	jobopt_T    *opt,
	ಅಕ್ಷರ	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_ಕೆಲಸ_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ಆದರೆ (get_ಕೆಲಸ_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ಆದರೆ (opt->jo_callback.cb_name != ಖಾಲಿ && *opt->jo_callback.cb_name != NUL)
    {
	ಆದರೆ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
	channel_ಗಣ_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ಆದರೆ (channel_send(channel, part_send, ಪಠ್ಯ, len, fun) == OK
					   && opt->jo_callback.cb_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ channel;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ch_ಅಭಿವ್ಯಕ್ತಿ_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ eval)
{
    char_u	*ಪಠ್ಯ;
    typval_T	*listtv;
    channel_T	*channel;
    ಪೂರ್ಣಾಂಕ		id;
    ch_ಬಹುಲಕ_T	ch_ಬಹುಲಕ;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    ಪೂರ್ಣಾಂಕ		timeout;
    ಪೂರ್ಣಾಂಕ		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    part_send = channel_part_send(channel);

    ch_ಬಹುಲಕ = channel_get_ಬಹುಲಕ(channel, part_send);
    ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_RAW || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_BLOB
	    || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	ಅಕ್ಷರ		*ಕೀಲಿ = ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP ? "id" : "seq";

	// return an empty dict by default
	ಆದರೆ (rettv_dict_alloc(rettv) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	ಆದರೆ (check_for_dict_arg(argvars, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ;

	d = argvars[1].vval.v_dict;
	di = dict_ಕಂಡುಹಿಡಿ(d, (char_u *)ಕೀಲಿ, -1);
	ಆದರೆ (argvars[2].v_type == VAR_DICT
		&& dict_has_ಕೀಲಿ(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ
		&& (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_ವಾದ_str), ಕೀಲಿ);
	    ಹಿಂತಿರುಗಿ;
	}

	ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_ಕೊನೆಯ_ಸಂದೇಶ_id;
	    ಆದರೆ (di == ಖಾಲಿ)
		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "seq", id);
	    ಇಲ್ಲದಿದ್ದರೆ
		di->di_tv.vval.v_ಸಂಖ್ಯೆ = id;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_ಕೊನೆಯ_ಸಂದೇಶ_id;
	    ಆದರೆ (di == ಖಾಲಿ)
		dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(d, "id", id);
	    ಇಲ್ಲದಿದ್ದರೆ
		di->di_tv.vval.v_ಸಂಖ್ಯೆ = id;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ಆದರೆ (di != ಖಾಲಿ && di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
		id = di->di_tv.vval.v_ಸಂಖ್ಯೆ;
	}
	ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP && !dict_has_ಕೀಲಿ(d, "jsonrpc"))
	    dict_ಕೂಡಿಸು_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	ಪಠ್ಯ = json_encode_lsp_ಸಂದೇಶ(&argvars[1]);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	id = ++channel->ch_ಕೊನೆಯ_ಸಂದೇಶ_id;
	ಪಠ್ಯ = json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(id, &argvars[1],
			      (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_JS ? JSON_JS : 0) | JSON_NL);
    }
    ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    channel = send_common(argvars, ಪಠ್ಯ, (ಪೂರ್ಣಾಂಕ)STRLEN(ಪಠ್ಯ), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(ಪಠ್ಯ);
    ಆದರೆ (channel != ಖಾಲಿ && eval)
    {
	ಆದರೆ (opt.jo_ಗಣ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ಇಲ್ಲದಿದ್ದರೆ
	    timeout = channel_get_timeout(channel, part_read);
	ಆದರೆ (channel_read_json_ಖಂಡ(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP || ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ಸಂಖ್ಯೆ;
		free_tv(listtv);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_ಕೊನೆಯ->li_tv;
		list->lv_u.mat.lv_ಕೊನೆಯ->li_tv.v_type = VAR_ಸಂಖ್ಯೆ;
		free_tv(listtv);
	    }
	}
    }
    free_ಕೆಲಸ_options(&opt);
    ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ಆದರೆ (rettv->vval.v_dict != ಖಾಲಿ)
	    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "seq", id);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ch_ಬಹುಲಕ == CH_ಬಹುಲಕ_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ಆದರೆ (rettv->vval.v_dict != ಖಾಲಿ)
	    dict_ಕೂಡಿಸು_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ch_raw_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಂಕ eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    ಪೂರ್ಣಾಂಕ		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (argvars[1].v_type == VAR_BLOB)
    {
	ಪಠ್ಯ = argvars[1].vval.v_blob->bv_ga.ga_ದತ್ತಾಂಶ;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪಠ್ಯ = tv_get_string_buf(&argvars[1], buf);
	len = (ಪೂರ್ಣಾಂಕ)STRLEN(ಪಠ್ಯ);
    }
    channel = send_common(argvars, ಪಠ್ಯ, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ಆದರೆ (channel != ಖಾಲಿ && eval)
    {
	ಆದರೆ (opt.jo_ಗಣ & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ಇಲ್ಲದಿದ್ದರೆ
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_ಖಂಡ(channel, part_read,
							timeout, TRUE, ಖಾಲಿ);
    }
    free_ಕೆಲಸ_options(&opt);
}

#ವ್ಯಾಖ್ಯಾನಿಸು KEEP_OPEN_ಸಮಯ 20  // msec

#ಆದರೆ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಂಕ
channel_poll_ಸಿದ್ಧತೆ(ಪೂರ್ಣಾಂಕ nfd_ಒಳ, ಶೂನ್ಯ *fds_ಒಳ, ಪೂರ್ಣಾಂಕ *towait)
{
    ಪೂರ್ಣಾಂಕ		nfd = nfd_ಒಳ;
    channel_T	*channel;
    ರಚನೆ	pollfd *fds = fds_ಒಳ;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ಆದರೆ (ch_part->ch_fd != INVALID_FD)
	    {
		ಆದರೆ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ಆದರೆ (*towait < 0 || *towait > KEEP_OPEN_ಸಮಯ)
			*towait = KEEP_OPEN_ಸಮಯ;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    ಹಿಂತಿರುಗಿ nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಂಕ
channel_poll_check(ಪೂರ್ಣಾಂಕ ret_ಒಳ, ಶೂನ್ಯ *fds_ಒಳ)
{
    ಪೂರ್ಣಾಂಕ		ret = ret_ಒಳ;
    channel_T	*channel;
    ರಚನೆ	pollfd *fds = fds_ಒಳ;
    ch_part_T	part;
    ಪೂರ್ಣಾಂಕ		idx;
    chanpart_T	*ಒಳ_part;

    FOR_ALL_CHANNELS(channel)
    {
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ಆದರೆ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	ಒಳ_part = &channel->ch_part[PART_ಒಳ];
	idx = ಒಳ_part->ch_poll_idx;
	ಆದರೆ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_ಒಳಹರಿವು(channel);
	    --ret;
	}
    }

    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ // UNIX && !HAVE_SELECT

#ಆದರೆ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಂಕ
channel_select_ಸಿದ್ಧತೆ(
	ಪೂರ್ಣಾಂಕ maxfd_ಒಳ,
	ಶೂನ್ಯ *rfds_ಒಳ,
	ಶೂನ್ಯ *wfds_ಒಳ,
	ರಚನೆ timeval *tv,
	ರಚನೆ timeval **tvp)
{
    ಪೂರ್ಣಾಂಕ		maxfd = maxfd_ಒಳ;
    channel_T	*channel;
    fd_set	*rfds = rfds_ಒಳ;
    fd_set	*wfds = wfds_ಒಳ;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ಆದರೆ (fd != INVALID_FD)
	    {
		ಆದರೆ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ಆದರೆ (*tvp == ಖಾಲಿ || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_ಸಮಯ * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_ಸಮಯ * 1000;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ಆದರೆ ((ಪೂರ್ಣಾಂಕ)fd >= FD_SETSIZE)
			ಮುಂದುವರಿ;
# endif
		    FD_SET((ಪೂರ್ಣಾಂಕ)fd, rfds);
		    ಆದರೆ (maxfd < (ಪೂರ್ಣಾಂಕ)fd)
			maxfd = (ಪೂರ್ಣಾಂಕ)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ಹಿಂತಿರುಗಿ maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಂಕ
channel_select_check(ಪೂರ್ಣಾಂಕ ret_ಒಳ, ಶೂನ್ಯ *rfds_ಒಳ, ಶೂನ್ಯ *wfds_ಒಳ)
{
    ಪೂರ್ಣಾಂಕ		ret = ret_ಒಳ;
    channel_T	*channel;
    fd_set	*rfds = rfds_ಒಳ;
    fd_set	*wfds = wfds_ಒಳ;
    ch_part_T	part;
    chanpart_T	*ಒಳ_part;

    FOR_ALL_CHANNELS(channel)
    {
	ಗಾಗಿ (part = PART_SOCK; part < PART_ಒಳ; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ಆದರೆ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ಪೂರ್ಣಾಂಕ)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	ಒಳ_part = &channel->ch_part[PART_ಒಳ];
	ಆದರೆ (ret > 0 && ಒಳ_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ಪೂರ್ಣಾಂಕ)ಒಳ_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(ಒಳ_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(ಒಳ_part->ch_fd, wfds);
	    channel_write_ಒಳಹರಿವು(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ಆದರೆ (channel->ch_ಕೆಲಸ != ಖಾಲಿ)
	{
	    ಕೆಲಸ_T *ಕೆಲಸ = channel->ch_ಕೆಲಸ;

	    ಆದರೆ (ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು != ಖಾಲಿ && ಕೆಲಸ->jv_ಸ್ಥಿತಿ == ಕೆಲಸ_ಪೂರ್ಣಗೊಂಡ)
		ಗಾಗಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ಹಿಂತಿರುಗಿ ret;
}
#ಆದರೆ_ಅಂತ್ಯ // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_wait_any(ದೀರ್ಘ msec)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    ಆದರೆ (stdio_channel->CH_ಹೊರಹೋಗು_FD != INVALID_FD)
	(ಶೂನ್ಯ)channel_wait(stdio_channel, stdio_channel->CH_ಹೊರಹೋಗು_FD,
						(ಪೂರ್ಣಾಂಕ)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#ಅಥವಾಆದರೆ defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    ರಚನೆ timeval	tv;
    ರಚನೆ timeval	*tvp = &tv;
    ಪೂರ್ಣಾಂಕ			maxfd;
    ಪೂರ್ಣಾಂಕ			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_ಸಿದ್ಧತೆ(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ಖಾಲಿ, tvp);
    ಆದರೆ (ret >= 0)
	(ಶೂನ್ಯ)channel_select_check(ret, &rfds, &wfds);
#ಇಲ್ಲದಿದ್ದರೆ
    ರಚನೆ pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    ಪೂರ್ಣಾಂಕ			nfd;
    ಪೂರ್ಣಾಂಕ			towait = (ಪೂರ್ಣಾಂಕ)msec;
    ಪೂರ್ಣಾಂಕ			ret;

    nfd = channel_poll_ಸಿದ್ಧತೆ(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ಆದರೆ (ret >= 0)
	(ಶೂನ್ಯ)channel_poll_check(ret, fds);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    ಶೂನ್ಯ
channel_stdio_loop(ಶೂನ್ಯ)
{
    ಆಗ (stdio_channel != ಖಾಲಿ && channel_is_open(stdio_channel))
    {
	ದೀರ್ಘ	msec = 1000;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
	ದೀರ್ಘ	due_ಸಮಯ = check_due_timer();

	ಆದರೆ (due_ಸಮಯ > 0 && due_ಸಮಯ < msec)
	    msec = due_ಸಮಯ;
#ಆದರೆ_ಅಂತ್ಯ
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
    ಪೂರ್ಣಾಂಕ
channel_parse_messages(ಶೂನ್ಯ)
{
    channel_T	*channel = ಮೊದಲ_channel;
    ಪೂರ್ಣಾಂಕ		ret = FALSE;
    ಪೂರ್ಣಾಂಕ		r;
    ch_part_T	part = PART_SOCK;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    elapsed_T	ಆರಂಭ_tv;
#ಆದರೆ_ಅಂತ್ಯ

    // The code below may invoke callbacks, which might call us back.
    // In a recursive call channels will not be closed.
    ++recursive;
    ++ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    ELAPSED_INIT(ಆರಂಭ_tv);
#ಆದರೆ_ಅಂತ್ಯ

    // Only do this message when another message was given, otherwise we get
    // lots of them.
    ಆದರೆ ((did_repeated_ಸಂದೇಶ & REPEATED_ಸಂದೇಶ_LOOKING) == 0)
    {
	ch_log(ಖಾಲಿ, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_ಸಂದೇಶ = REPEATED_ಸಂದೇಶ_LOOKING;
    }
    ಆಗ (channel != ಖಾಲಿ)
    {
	ಆದರೆ (recursive == 1)
	{
	    ಆದರೆ (channel_can_close(channel))
	    {
		channel->ch_ಗಮ್ಯ_be_closed = (1U << PART_ಎಣಿಕೆ);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = ಮೊದಲ_channel;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (channel->ch_ಗಮ್ಯ_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ಆದರೆ (channel->ch_ಕೆಲಸ != ಖಾಲಿ)
		    channel->ch_ಕೆಲಸ->jv_channel = ಖಾಲಿ;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = ಮೊದಲ_channel;
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = ಮೊದಲ_channel;
		part = PART_SOCK;
		ಮುಂದುವರಿ;
	    }
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	ಆದರೆ (part == PART_ಹೊರಹೋಗು
		&& channel->ch_ಕೆಲಸ != ಖಾಲಿ
		&& channel->ch_ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು != ಖಾಲಿ
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    ಮುಂದುವರಿ;
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ಆದರೆ (r == OK)
		ret = TRUE;
	    ಆದರೆ (channel_unref(channel) || (r == OK
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
			// Limit the time we loop here to 100 msec, otherwise
			// Vim becomes unresponsive when the callback takes
			// more than a bit of time.
			&& ELAPSED_FUNC(ಆರಂಭ_tv) < 100L
#ಆದರೆ_ಅಂತ್ಯ
			))
	    {
		// channel was freed or something was done, start over
		channel = ಮೊದಲ_channel;
		part = PART_SOCK;
		ಮುಂದುವರಿ;
	    }
	}
	ಆದರೆ (part < PART_ERR)
	    ++part;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    channel = channel->ch_ಮುಂದಿನ;
	    part = PART_SOCK;
	}
    }

    ಆದರೆ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --ಸುರಕ್ಷಿತ_ಗಮ್ಯ_invoke_callback;
    --recursive;

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    ಪೂರ್ಣಾಂಕ
channel_any_readahead(ಶೂನ್ಯ)
{
    channel_T	*channel = ಮೊದಲ_channel;
    ch_part_T	part = PART_SOCK;

    ಆಗ (channel != ಖಾಲಿ)
    {
	ಆದರೆ (channel_has_readahead(channel, part))
	    ಹಿಂತಿರುಗಿ TRUE;
	ಆದರೆ (part < PART_ERR)
	    ++part;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    channel = channel->ch_ಮುಂದಿನ;
	    part = PART_SOCK;
	}
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_channel(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    ಗಾಗಿ (channel = ಮೊದಲ_channel; !abort && channel != ಖಾಲಿ;
						   channel = channel->ch_ಮುಂದಿನ)
	ಆದರೆ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || ಗಣ_ref_ಒಳ_ಅಂಶ(&tv, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	}
    ಹಿಂತಿರುಗಿ abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ಸ್ಥಿರ ch_part_T
channel_part_send(channel_T *channel)
{
    ಆದರೆ (channel->CH_SOCK_FD == INVALID_FD)
	ಹಿಂತಿರುಗಿ PART_ಒಳ;
    ಹಿಂತಿರುಗಿ PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ಸ್ಥಿರ ch_part_T
channel_part_read(channel_T *channel)
{
    ಆದರೆ (channel->CH_SOCK_FD == INVALID_FD)
	ಹಿಂತಿರುಗಿ PART_ಹೊರಹೋಗು;
    ಹಿಂತಿರುಗಿ PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ಸ್ಥಿರ ch_ಬಹುಲಕ_T
channel_get_ಬಹುಲಕ(channel_T *channel, ch_part_T part)
{
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ CH_ಬಹುಲಕ_JSON;
    ಹಿಂತಿರುಗಿ channel->ch_part[part].ch_ಬಹುಲಕ;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ಹಿಂತಿರುಗಿ channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    ಶೂನ್ಯ
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    ಆದರೆ (ಒಳ_vim9script() && check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಆದರೆ (channel != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_ಹೊರಹೋಗು)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    ಶೂನ್ಯ
f_ch_close(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    channel_T *channel;

    ಆದರೆ (ಒಳ_vim9script() && check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಆದರೆ (channel != ಖಾಲಿ)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    ಶೂನ್ಯ
f_ch_close_ಒಳ(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    channel_T *channel;

    ಆದರೆ (ಒಳ_vim9script() && check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಆದರೆ (channel != ಖಾಲಿ)
	channel_close_ಒಳ(channel);
}

/*
 * "ch_getbufnr()" function
 */
    ಶೂನ್ಯ
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    char_u	*what = tv_get_string(&argvars[1]);
    ಪೂರ್ಣಾಂಕ		part;
    ಆದರೆ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "out") == 0)
	part = PART_ಹೊರಹೋಗು;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(what, "in") == 0)
	part = PART_ಒಳ;
    ಇಲ್ಲದಿದ್ದರೆ
	part = PART_SOCK;
    ಆದರೆ (channel->ch_part[part].ch_bufref.br_buf != ಖಾಲಿ)
	rettv->vval.v_ಸಂಖ್ಯೆ =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    ಶೂನ್ಯ
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    ಆದರೆ (ಒಳ_vim9script() && check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rettv->v_type = VAR_ಕೆಲಸ;
    rettv->vval.v_ಕೆಲಸ = channel->ch_ಕೆಲಸ;
    ಆದರೆ (channel->ch_ಕೆಲಸ != ಖಾಲಿ)
	++channel->ch_ಕೆಲಸ->jv_refcount;
}

/*
 * "ch_info()" function
 */
    ಶೂನ್ಯ
f_ch_info(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    channel_T *channel;

    ಆದರೆ (ಒಳ_vim9script() && check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL)
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಆದರೆ (channel != ಖಾಲಿ && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    ಶೂನ್ಯ
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    ಶೂನ್ಯ
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    ಶೂನ್ಯ
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    ಶೂನ್ಯ
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    ಶೂನ್ಯ
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    ಶೂನ್ಯ
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ಅಭಿವ್ಯಕ್ತಿ_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    ಶೂನ್ಯ
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ಅಭಿವ್ಯಕ್ತಿ_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    ಶೂನ್ಯ
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    ಶೂನ್ಯ
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    ಶೂನ್ಯ
f_ch_setoptions(typval_T *argvars, typval_T *rettv ಬಳಸದ)
{
    channel_T	*channel;
    jobopt_T	opt;

    ಆದರೆ (check_restricted() || check_secure())
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಆದರೆ (channel == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    clear_ಕೆಲಸ_options(&opt);
    ಆದರೆ (get_ಕೆಲಸ_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_ಬಹುಲಕ_ALL, 0) == OK)
	channel_ಗಣ_options(channel, &opt);
    free_ಕೆಲಸ_options(&opt);
}

/*
 * "ch_status()" function
 */
    ಶೂನ್ಯ
f_ch_ಸ್ಥಿತಿ(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    ಪೂರ್ಣಾಂಕ		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ಖಾಲಿ;

    ಆದರೆ (ಒಳ_vim9script()
	    && (check_for_chan_or_ಕೆಲಸ_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಹಿಂತಿರುಗಿ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ಆದರೆ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_ಕೆಲಸ_options(&opt);
	ಆದರೆ (get_ಕೆಲಸ_options(&argvars[1], &opt, JO_PART, 0) == OK
						     && (opt.jo_ಗಣ & JO_PART))
	    part = opt.jo_part;
    }

    rettv->vval.v_string = vim_strsave((char_u *)channel_ಸ್ಥಿತಿ(channel, part));
}

/*
 * Get a string with information about the channel in "varp" in "buf".
 * "buf" must be at least NUMBUFLEN long.
 */
    char_u *
channel_ಗಮ್ಯ_string_buf(typval_T *varp, char_u *buf)
{
    channel_T *channel = varp->vval.v_channel;
    ಅಕ್ಷರ      *ಸ್ಥಿತಿ_2 = channel_ಸ್ಥಿತಿ(channel, -1);

    ಆದರೆ (channel == ಖಾಲಿ)
	vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN, "channel %s", ಸ್ಥಿತಿ_2);
    ಇಲ್ಲದಿದ್ದರೆ
	vim_snprintf((ಅಕ್ಷರ *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, ಸ್ಥಿತಿ_2);
    ಹಿಂತಿರುಗಿ buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_ಕಂಡುಹಿಡಿ(ಪೂರ್ಣಾಂಕ ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ಆದರೆ (ch->ch_id == ch_id)
	    ಹಿಂತಿರುಗಿ ch;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_JOB_CHANNEL
