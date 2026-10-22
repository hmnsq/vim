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

#ಯದಿ defined(FEAT_JOB_CHANNEL)

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ sockaddr_un {
    ADDRESS_FAMILY sun_family;
    ವರ್ಣ sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ಅನ್ಯಥಾ
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
#endif

ಸ್ಥಿರ ಶೂನ್ಯ channel_read(channel_T *channel, ch_part_T part, ವರ್ಣ *func);
ಸ್ಥಿರ ಶೂನ್ಯ channel_wait_any(long msec);
ಸ್ಥಿರ ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ channel_get_timeout(channel_T *channel, ch_part_T part);
ಸ್ಥಿರ channel_T *channel_open_stdio(ಶೂನ್ಯ);
ಸ್ಥಿರ ch_part_T channel_part_send(channel_T *channel);
ಸ್ಥಿರ ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    ಪ್ರತಿ ((ch) = first_channel; (ch) != ರಿಕ್ತ; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ safe_to_invoke_callback = 0;

#ifdef MSWIN
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fd_read(sock_T fd, ವರ್ಣ *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ಯದಿ (!ReadFile(h, buf, (DWORD)len, &nread, ರಿಕ್ತ))
	ಪ್ರತ್ಯಾಗಮ -1;
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)nread;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
fd_write(sock_T fd, ವರ್ಣ *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    ಯಾವತ್ (todo > 0)
    {
	ಯದಿ (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	ಅನ್ಯಥಾ
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	ಯದಿ (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    ಯದಿ (err != ERROR_IO_PENDING)
		ಪ್ರತ್ಯಾಗಮ -1;
	    ಯದಿ (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		ಪ್ರತ್ಯಾಗಮ -1;
	    FlushFileBuffers(h);
	}
	ಅನ್ಯಥಾ ಯದಿ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    ಭಙ್ಗ;
	todo -= nwrite;
	done += nwrite;
    }
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)done;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(ಸಂದೇಶ) (ಶೂನ್ಯ)semsg("%s: %s", ಸಂದೇಶ, strerror_win32(errno))

    ಸ್ಥಿರ ವರ್ಣ *
strerror_win32(ಪೂರ್ಣಾಙ್ಕ eno)
{
    ಸ್ಥಿರ LPVOID msgbuf = ರಿಕ್ತ;
    char_u *ptr;

    ಯದಿ (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ರಿಕ್ತ;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	ರಿಕ್ತ,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ರಿಕ್ತ);
    ಯದಿ (msgbuf != ರಿಕ್ತ)
	// chomp \r or \n
	ಪ್ರತಿ (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    ವಿಕಲ್ಪ (*ptr)
	    {
		ಪಕ್ಷ '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    ಭಙ್ಗ;
		ಪಕ್ಷ '\n':
		    ಯದಿ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    ಅನ್ಯಥಾ
			*ptr = ' ';
		    ಭಙ್ಗ;
	    }
    ಪ್ರತ್ಯಾಗಮ msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
ಸ್ಥಿರ channel_T *first_channel = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(ಶೂನ್ಯ)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    ಪ್ರತಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = (Xt_ಒಳಹರಿವು_Id)ರಿಕ್ತ;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    ಯದಿ (first_channel != ರಿಕ್ತ)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    ಪ್ರತ್ಯಾಗಮ channel;
}

    ಪೂರ್ಣಾಙ್ಕ
has_any_channel(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ first_channel != ರಿಕ್ತ;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_still_useful(channel_T *channel)
{
    ಪೂರ್ಣಾಙ್ಕ has_sock_ಸಂದೇಶ;
    ಪೂರ್ಣಾಙ್ಕ	has_out_ಸಂದೇಶ;
    ಪೂರ್ಣಾಙ್ಕ	has_err_ಸಂದೇಶ;

    // If the job was killed the channel is not expected to work anymore.
    ಯದಿ (channel->ch_job_killed && channel->ch_job == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // If there is a close callback it may still need to be invoked.
    ಯದಿ (channel->ch_close_cb.cb_name != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // If reading from or a buffer it's still useful.
    ಯದಿ (channel->ch_part[PART_IN].ch_bufref.br_buf != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_ಸಂದೇಶ = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != ರಿಕ್ತ
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != ರಿಕ್ತ;
    has_out_ಸಂದೇಶ = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != ರಿಕ್ತ
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != ರಿಕ್ತ;
    has_err_ಸಂದೇಶ = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != ರಿಕ್ತ
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ (channel->ch_callback.cb_name != ರಿಕ್ತ && (has_sock_ಸಂದೇಶ
		|| has_out_ಸಂದೇಶ || has_err_ಸಂದೇಶ))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != ರಿಕ್ತ
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != ರಿಕ್ತ)
		    && has_out_ಸಂದೇಶ)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ರಿಕ್ತ
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ರಿಕ್ತ)
		    && has_err_ಸಂದೇಶ);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_can_close(channel_T *channel)
{
    ಪ್ರತ್ಯಾಗಮ channel->ch_to_be_closed == 0;
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
    ಯದಿ (channel->ch_next != ರಿಕ್ತ)
	channel->ch_next->ch_prev = channel->ch_prev;
    ಯದಿ (channel->ch_prev == ರಿಕ್ತ)
	first_channel = channel->ch_next;
    ಅನ್ಯಥಾ
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_free(channel_T *channel)
{
    ಯದಿ (in_free_unref_ವಸ್ತುಗಳು)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_may_free(channel_T *channel)
{
    ಯದಿ (!channel_still_useful(channel))
    {
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_unref(channel_T *channel)
{
    ಯದಿ (channel != ರಿಕ್ತ && --channel->ch_refcount <= 0)
	ಪ್ರತ್ಯಾಗಮ channel_may_free(channel);
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
free_unused_channels_contents(ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ mask)
{
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ಯದಿ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    ಪ್ರತ್ಯಾಗಮ did_free;
}

    ಶೂನ್ಯ
free_unused_channels(ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    ಪ್ರತಿ (ch = first_channel; ch != ರಿಕ್ತ; ch = ch_next)
    {
	ch_next = ch->ch_next;
	ಯದಿ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ಯದಿ defined(FEAT_GUI)

# ಯದಿ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    ಸ್ಥಿರ channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ಯದಿ (fd == INVALID_FD)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    FOR_ALL_CHANNELS(channel)
    {
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	    ಯದಿ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ಪ್ರತ್ಯಾಗಮ channel;
	    }
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_read_fd(ಪೂರ್ಣಾಙ್ಕ fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ಯದಿ (channel == ರಿಕ್ತ)
	ch_error(ರಿಕ್ತ, "Channel for fd %d not found", fd);
    ಅನ್ಯಥಾ
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ಸ್ಥಿರ ಶೂನ್ಯ
messageFromServerX11(XtPointer clientData,
		  ಪೂರ್ಣಾಙ್ಕ *unused1 UNUSED,
		  Xt_ಒಳಹರಿವು_Id *unused2 UNUSED)
{
    channel_read_fd((ಪೂರ್ಣಾಙ್ಕ)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  ಯದಿ GTK_CHECK_VERSION(3,0,0)
    ಸ್ಥಿರ gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    ಪ್ರತ್ಯಾಗಮ TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  ಅನ್ಯಥಾ
    ಸ್ಥಿರ ಶೂನ್ಯ
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_ಒಳಹರಿವು_Condition unused2 UNUSED)
{
    channel_read_fd((ಪೂರ್ಣಾಙ್ಕ)(long)clientData);
}
#  endif
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    ಯದಿ (!CH_HAS_GUI)
	ಪ್ರತ್ಯಾಗಮ;

    // gets stuck in handling events for a not connected channel
    ಯದಿ (channel->ch_keep_open)
	ಪ್ರತ್ಯಾಗಮ;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ಯದಿ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler == (Xt_ಒಳಹರಿವು_Id)ರಿಕ್ತ)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = Xt_App_Add_ಒಳಹರಿವು(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_ಒಳಹರಿವು_Read_Mask + Xt_ಒಳಹರಿವು_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# ಅನ್ಯಥಾ
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ಯದಿ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   ಅನ್ಯಥಾ
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = gdk_ಒಳಹರಿವು_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_ಒಳಹರಿವು_Condition)
			     ((ಪೂರ್ಣಾಙ್ಕ)GDK_ಒಳಹರಿವು_READ + (ಪೂರ್ಣಾಙ್ಕ)GDK_ಒಳಹರಿವು_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_gui_register(channel_T *channel)
{
    ಯದಿ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    ಯದಿ (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    ಯದಿ (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    ಶೂನ್ಯ
channel_gui_register_all(ಶೂನ್ಯ)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    ಯದಿ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler != (Xt_ಒಳಹರಿವು_Id)ರಿಕ್ತ)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_ಒಳಹರಿವು(channel->ch_part[part].ch_ಒಳಹರಿವು_Handler);
	channel->ch_part[part].ch_ಒಳಹರಿವು_Handler = (Xt_ಒಳಹರಿವು_Id)ರಿಕ್ತ;
    }
# ಅನ್ಯಥಾ
#  ifdef FEAT_GUI_GTK
    ಯದಿ (channel->ch_part[part].ch_ಒಳಹರಿವು_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   ಯದಿ GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_ಒಳಹರಿವು_Handler);
#   ಅನ್ಯಥಾ
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

    ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_connect(
	channel_T *channel,
	ಧ್ರುವ ಸಂರಚನಾ sockaddr *server_addr,
	ಪೂರ್ಣಾಙ್ಕ server_addrlen,
	ಪೂರ್ಣಾಙ್ಕ *waittime)
{
    ಪೂರ್ಣಾಙ್ಕ		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    ಯಾವತ್ (TRUE)
    {
	long	elapsed_msec = 0;
	ಪೂರ್ಣಾಙ್ಕ	waitnow;
	ಪೂರ್ಣಾಙ್ಕ	ret;

	ಯದಿ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ಯದಿ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	ಯದಿ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ಯದಿ (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#ಅನ್ಯಥಾ
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		ಪ್ರತ್ಯಾಗಮ -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ಯದಿ (ret == 0)
	    // The connection could be established.
	    ಭಙ್ಗ;

	SOCK_ERRNO;
	ಯದಿ (*waittime < 0 || (errno != EWOULDBLOCK
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
	    ಪ್ರತ್ಯಾಗಮ -1;
	}
	ಅನ್ಯಥಾ ಯದಿ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ಪ್ರತ್ಯಾಗಮ -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    ಸಂರಚನಾ timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    ಪೂರ್ಣಾಙ್ಕ			so_error = 0;
	    socklen_t		so_error_len = ಪರಿಮಾಣ(so_error);
	    ಸಂರಚನಾ timeval	start_tv;
	    ಸಂರಚನಾ timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, ರಿಕ್ತ);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ರಿಕ್ತ, &tv);
	    ಯದಿ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		ಪ್ರತ್ಯಾಗಮ -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ಯದಿ (FD_ISSET(sd, &wfds))
		ಭಙ್ಗ;
	    elapsed_msec = waitnow;
	    ಯದಿ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		ಅನುವರ್ತ;
	    }
#ಅನ್ಯಥಾ
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ಯದಿ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ಯದಿ (ret < 0 || (so_error != 0
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
		    ಪ್ರತ್ಯಾಗಮ -1;
		}
		ಅನ್ಯಥಾ ಯದಿ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ಪ್ರತ್ಯಾಗಮ -1;
		}
	    }

	    ಯದಿ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		ಭಙ್ಗ;

	    gettimeofday(&end_tv, ರಿಕ್ತ);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	ಯದಿ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ಯದಿ (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ಯದಿ (!got_int)
	    {
		ಯದಿ (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		ಅನುವರ್ತ;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)fcntl(sd, F_SETFL, 0);
#endif
    }

    ಪ್ರತ್ಯಾಗಮ sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ಧ್ರುವ ವರ್ಣ *path,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ))
{
    channel_T		*channel = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			sd = -1;
    size_t		path_len = STRLEN(path);
    ಸಂರಚನಾ sockaddr_un	server;
    size_t		server_len;
    ಪೂರ್ಣಾಙ್ಕ			waittime = -1;

    ಯದಿ (*path == NUL || path_len >= ಪರಿಮಾಣ(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
    {
	ch_error(ರಿಕ್ತ, "Cannot allocate channel.");
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, ಪರಿಮಾಣ(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (ಸಂರಚನಾ sockaddr *)&server, (ಪೂರ್ಣಾಙ್ಕ)server_len,
								   &waittime);

    ಯದಿ (sd < 0)
    {
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ವರ್ಣ *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ಪ್ರತ್ಯಾಗಮ channel;
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
	ಧ್ರುವ ವರ್ಣ *hostname,
	ಪೂರ್ಣಾಙ್ಕ port,
	ಪೂರ್ಣಾಙ್ಕ waittime,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ))
{
    ಪೂರ್ಣಾಙ್ಕ			sd = -1;
    channel_T		*channel = ರಿಕ್ತ;
#ifdef FEAT_IPV6
    ಪೂರ್ಣಾಙ್ಕ			err;
    ಸಂರಚನಾ addrinfo	hints;
    ಸಂರಚನಾ addrinfo	*res = ರಿಕ್ತ;
    ಸಂರಚನಾ addrinfo	*addr = ರಿಕ್ತ;
#ಅನ್ಯಥಾ
    ಸಂರಚನಾ sockaddr_in	server;
    ಸಂರಚನಾ hostent	*host = ರಿಕ್ತ;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
    {
	ch_error(ರಿಕ್ತ, "Cannot allocate channel.");
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ಯದಿ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ಯದಿ ((err = getaddrinfo(hostname, ರಿಕ್ತ, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Count the number of addresses for timeout distribution
    ಪೂರ್ಣಾಙ್ಕ addr_ಎಣಿಕೆ = 0;
    ಪ್ರತಿ (addr = res; addr != ರಿಕ್ತ; addr = addr->ai_next)
	addr_ಎಣಿಕೆ++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ಯದಿ (waittime == 0)
	waittime = 15;

    ಪೂರ್ಣಾಙ್ಕ addr_index = 0;
    ಪ್ರತಿ (addr = res; addr != ರಿಕ್ತ; addr = addr->ai_next, addr_index++)
    {
	ಧ್ರುವ ವರ್ಣ  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ಧ್ರುವ ಶೂನ್ಯ  *src = ರಿಕ್ತ;
	ವರ್ಣ	    buf[NUMBUFLEN];
# endif
	ಪೂರ್ಣಾಙ್ಕ	    try_waittime;
	ಪೂರ್ಣಾಙ್ಕ	    before_waittime;
	ಪೂರ್ಣಾಙ್ಕ	    consumed;
	ಪೂರ್ಣಾಙ್ಕ	    remaining_addrs;

	ಯದಿ (addr->ai_family == AF_INET6)
	{
	    ಸಂರಚನಾ sockaddr_in6 *sai = (ಸಂರಚನಾ sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	ಅನ್ಯಥಾ ಯದಿ (addr->ai_family == AF_INET)
	{
	    ಸಂರಚನಾ sockaddr_in *sai = (ಸಂರಚನಾ sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ಯದಿ (src != ರಿಕ್ತ)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, ಪರಿಮಾಣ(buf));
	    ಯದಿ (dst == ರಿಕ್ತ)
		dst = hostname;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ಯದಿ (addr->ai_next == ರಿಕ್ತ)
	    try_waittime = waittime;
	ಅನ್ಯಥಾ ಯದಿ (addr_index == 0)
	{
	    ಯದಿ (waittime > 500)
		try_waittime = 250;
	    ಅನ್ಯಥಾ ಯದಿ (waittime > 30)
		try_waittime = waittime / 2;
	    ಅನ್ಯಥಾ
		try_waittime = waittime;
	}
	ಅನ್ಯಥಾ
	{
	    remaining_addrs = addr_ಎಣಿಕೆ - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (ಪೂರ್ಣಾಙ್ಕ)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ಯದಿ (waittime < 0)
	    waittime = 0;

	ಯದಿ (sd >= 0)
	    ಭಙ್ಗ;

	// If we have no time left, stop trying
	ಯದಿ (waittime <= 0 && addr->ai_next != ರಿಕ್ತ)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    ಭಙ್ಗ;
	}
    }

    freeaddrinfo(res);
#ಅನ್ಯಥಾ
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    ಯದಿ ((host = gethostbyname(hostname)) == ರಿಕ್ತ)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    {
	ವರ್ಣ *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], ಪರಿಮಾಣ(p));
	memcpy((ವರ್ಣ *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    ಯದಿ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (ಸಂರಚನಾ sockaddr *)&server, ಪರಿಮಾಣ(server),
								   &waittime);
#endif

    ಯದಿ (sd < 0)
    {
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ವರ್ಣ *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ಪ್ರತ್ಯಾಗಮ channel;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ಯದಿ (callback->cb_name != ರಿಕ್ತ && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    ಅನ್ಯಥಾ
	cbp->cb_name = ರಿಕ್ತ;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ಸ್ಥಿರ ಶೂನ್ಯ
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_ಮೌಲ್ಯ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_ಮೌಲ್ಯ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    ಯದಿ (curbuf->b_ml.ml_mfp == ರಿಕ್ತ)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ಸ್ಥಿರ buf_T *
channel_find_buffer(char_u *name, ಪೂರ್ಣಾಙ್ಕ err, ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ)
{
    buf_T *buf = ರಿಕ್ತ;
    buf_T *save_curbuf = curbuf;

    ಯದಿ (name != ರಿಕ್ತ && *name != NUL)
    {
	buf = buflist_findname(name);
	ಯದಿ (buf == ರಿಕ್ತ)
	    buf = buflist_findname_exp(name);
    }

    ಯದಿ (buf != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ buf;

    buf = buflist_new(name == ರಿಕ್ತ || *name == NUL ? ರಿಕ್ತ : name,
	    ರಿಕ್ತ, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    prepare_buffer(buf);

    curbuf = buf;
    ಯದಿ (ಸಂದೇಶ)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ಯದಿ (opt->jo_set & JO_MODE)
	ಪ್ರತಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    ಯದಿ (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    ಯದಿ (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    ಯದಿ (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    ಯದಿ (opt->jo_set & JO_TIMEOUT)
	ಪ್ರತಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ಯದಿ (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    ಯದಿ (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ಯದಿ (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    ಯದಿ (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    ಯದಿ (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    ಯದಿ (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ಯದಿ (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ಯದಿ ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ಯದಿ (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	ಯದಿ (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    ಯದಿ (buf == ರಿಕ್ತ)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ = TRUE;

	    ಯದಿ (opt->jo_set2 & JO2_OUT_ಸಂದೇಶ)
		ಸಂದೇಶ = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, ಸಂದೇಶ);
	}
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    ಯದಿ (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    ಯದಿ (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ಅನ್ಯಥಾ
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (ವರ್ಣ *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
		    prepare_buffer(buf);
	    }
	}
    }

    ಯದಿ ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ಯದಿ (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	ಯದಿ (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	ಅನ್ಯಥಾ ಯದಿ (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ಯದಿ (buf == ರಿಕ್ತ)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ ಸಂದೇಶ = TRUE;

	    ಯದಿ (opt->jo_set2 & JO2_ERR_ಸಂದೇಶ)
		ಸಂದೇಶ = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, ಸಂದೇಶ);
	}
	ಯದಿ (buf != ರಿಕ್ತ)
	{
	    ಯದಿ (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ಯದಿ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ಅನ್ಯಥಾ
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (ವರ್ಣ *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ಯದಿ (buf->b_ml.ml_mfp == ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ
channel_parse_socketserver_address(
	char_u	*address,
	ಪೂರ್ಣಾಙ್ಕ	*port,
	char_u	**unix_path,
	bool	quiet)
{
    ವರ್ಣ    *rest;
    long    val;

    ಯದಿ (*address == NUL)
	ಗಚ್ಛ fail;

    ಯದಿ (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    val = strtol((ವರ್ಣ *)(address), &rest, 10);
    ಯದಿ (val < 0 || val >= 65536 || *rest != NUL)
    {
	ಯದಿ (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    *port = (ಪೂರ್ಣಾಙ್ಕ)val;

    ಪ್ರತ್ಯಾಗಮ OK;
fail:
    ಯದಿ (!quiet)
	semsg(_(e_invalid_argument_str), address);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Implements ch_open().
 */
    ಸ್ಥಿರ channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    ವರ್ಣ	*rest;
    ಪೂರ್ಣಾಙ್ಕ		port = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_ipv6 = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		is_unix = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    address = tv_get_string(&argvars[0]);
    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    ಅನ್ಯಥಾ ಯದಿ (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	ಯದಿ (p == ರಿಕ್ತ || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ಅನ್ಯಥಾ
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    ಯದಿ (!is_unix && !is_stdio)
    {
	port = strtol((ವರ್ಣ *)(p + 1), &rest, 10);
	ಯದಿ (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	ಅನ್ಯಥಾ
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ಯದಿ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	ಗಚ್ಛ theend;
    ಯದಿ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	ಗಚ್ಛ theend;
    }

    ಯದಿ (is_stdio)
	channel = channel_open_stdio();
    ಅನ್ಯಥಾ ಯದಿ (is_unix)
	channel = channel_open_unix((ವರ್ಣ *)address, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	channel = channel_open((ವರ್ಣ *)address, port, opt.jo_waittime, ರಿಕ್ತ);
    ಯದಿ (channel != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	ಯದಿ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ಪ್ರತ್ಯಾಗಮ channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
ಸ್ಥಿರ channel_T *stdio_channel = ರಿಕ್ತ;

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
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    ಯದಿ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (stdio_channel != ರಿಕ್ತ)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	ಪೂರ್ಣಾಙ್ಕ	nul_fd;

	ಯದಿ (!DuplicateHandle(proc, GetStdHandle(STD_ಒಳಹರಿವು_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	ಯದಿ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ರಿಕ್ತ, OPEN_EXISTING, 0,
		ರಿಕ್ತ);
	ಯದಿ (hnul != INVALID_HANDLE_ಮೌಲ್ಯ)
	{
	    SetStdHandle(STD_ಒಳಹರಿವು_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ಯದಿ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	ಯದಿ (in_fd < 0 || out_fd < 0)
	{
	    ಯದಿ (in_fd >= 0)
		close(in_fd);
	    ಯದಿ (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	(ಶೂನ್ಯ)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(ಶೂನ್ಯ)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ಯದಿ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
    {
	ch_error(ರಿಕ್ತ, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ಪ್ರತ್ಯಾಗಮ channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    ವರ್ಣ	*rest;
    ಪೂರ್ಣಾಙ್ಕ		port;
    ಪೂರ್ಣಾಙ್ಕ		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    arg = tv_get_string(&argvars[0]);
    ಯದಿ (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    ಅನ್ಯಥಾ
    {
	port = strtol((ವರ್ಣ *)(arg), &rest, 10);
	ಯದಿ (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ಯದಿ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	ಗಚ್ಛ theend;
    ಯದಿ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	ಗಚ್ಛ theend;
    }

    ಯದಿ (is_unix)
	channel = channel_listen_unix((ವರ್ಣ *)arg, ರಿಕ್ತ, true);
    ಅನ್ಯಥಾ
	channel = channel_listen(port, ರಿಕ್ತ);
    ಯದಿ (channel != ರಿಕ್ತ)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ಪ್ರತ್ಯಾಗಮ channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	ಪೂರ್ಣಾಙ್ಕ port_in,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ))
{
    ಪೂರ್ಣಾಙ್ಕ			sd = -1;
    ಸಂರಚನಾ sockaddr_in	server;
#ifndef FEAT_IPV6
    ಸಂರಚನಾ hostent	*host;
#endif
    ಪೂರ್ಣಾಙ್ಕ			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
    {
	ch_error(ರಿಕ್ತ, "Cannot allocate channel.");
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((ವರ್ಣ *)&server, 0, ಪರಿಮಾಣ(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ಯದಿ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

#ifdef MSWIN
    ಯದಿ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ಧ್ರುವ ವರ್ಣ *)&val, ಪರಿಮಾಣ(val)) < 0)
#ಅನ್ಯಥಾ
    ಯದಿ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, ಪರಿಮಾಣ(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Bind the socket to the port
    ಯದಿ (bind(sd, (ಸಂರಚನಾ sockaddr *)&server, ಪರಿಮಾಣ(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ಯದಿ (port_in == 0)
    {
	ಸಂರಚನಾ sockaddr_in	addr;
	socklen_t		addr_len = ಪರಿಮಾಣ(addr);

	ಯದಿ (getsockname(sd, (ಸಂರಚನಾ sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ವರ್ಣ *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ಪ್ರತ್ಯಾಗಮ channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	ವರ್ಣ *path,
	ಶೂನ್ಯ (*nb_close_cb)(ಶೂನ್ಯ),
	bool replace)
{
    ಪೂರ್ಣಾಙ್ಕ			sd = -1;
    ಸಂರಚನಾ sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    ಯದಿ (path == ರಿಕ್ತ || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == ರಿಕ್ತ ? (ವರ್ಣ *)"" : path);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    path_len = STRLEN(path);
    ಯದಿ (path_len >= ಪರಿಮಾಣ(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
    {
	ch_error(ರಿಕ್ತ, "Cannot allocate channel.");
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, ಪರಿಮಾಣ(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ಯದಿ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(ಸಂರಚನಾ sockaddr_un, sun_path) + path_len + 1;
    ಯದಿ (bind(sd, (ಸಂರಚನಾ sockaddr *)&server, (ಪೂರ್ಣಾಙ್ಕ)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ವರ್ಣ *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ಪ್ರತ್ಯಾಗಮ channel;
}

     ಶೂನ್ಯ
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ಯದಿ (*fd == INVALID_FD)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (part == PART_SOCK)
	sock_close(*fd);
    ಅನ್ಯಥಾ
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ಯದಿ ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    ಯದಿ (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    ಶೂನ್ಯ
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    ಯದಿ (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#ಯದಿ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ಯದಿ (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    ಯದಿ (out != INVALID_FD)
    {
#ಯದಿ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#ಯದಿ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    ಯದಿ (err != INVALID_FD)
    {
#ಯದಿ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#ಯದಿ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    ಶೂನ್ಯ
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    ಯದಿ (job->jv_in_buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (ವರ್ಣ *)in_part->ch_bufref.br_buf->b_ffname);
    ಯದಿ (options->jo_set & JO_IN_ತುದಿ)
    {
	ಯದಿ (options->jo_in_ತುದಿ == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_ತುದಿ =
		in_part->ch_bufref.br_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ + 1;
	}
	ಅನ್ಯಥಾ
	    in_part->ch_buf_ತುದಿ = options->jo_in_ತುದಿ;
    }
    ಅನ್ಯಥಾ
	in_part->ch_buf_ತುದಿ = 1;
    ಯದಿ (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    ಅನ್ಯಥಾ
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	ಪೂರ್ಣಾಙ್ಕ	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    ಯದಿ (item == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = ರಿಕ್ತ;
    ಯದಿ (item->cq_prev == ರಿಕ್ತ)
	head->cq_next = item;
    ಅನ್ಯಥಾ
	item->cq_prev->cq_next = item;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
write_buf_ಸಾಲು(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *ಸಾಲು = ml_get_buf(buf, lnum, FALSE);
    ಪೂರ್ಣಾಙ್ಕ	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    // Need to make a copy to be able to append a NL.
    ಯದಿ ((p = alloc(len + 2)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    memcpy((ವರ್ಣ *)p, (ವರ್ಣ *)ಸಾಲು, len);

    ಯದಿ (channel->ch_write_text_mode)
	p[len] = CAR;
    ಅನ್ಯಥಾ
    {
	ಪ್ರತಿ (i = 0; i < len; ++i)
	    ಯದಿ (p[i] == NL)
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
can_write_buf_ಸಾಲು(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    ಯದಿ (in_part->ch_fd == INVALID_FD)
	ಪ್ರತ್ಯಾಗಮ FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ಯದಿ (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    ಅನ್ಯಥಾ ಯದಿ (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# ಯದಿ defined(HAVE_SELECT)
	ಸಂರಚನಾ timeval	tval;
	fd_set		wfds;
	ಪೂರ್ಣಾಙ್ಕ		ret;

	FD_ZERO(&wfds);
	FD_SET((ಪೂರ್ಣಾಙ್ಕ)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	ಪ್ರತಿ (;;)
	{
	    ret = select((ಪೂರ್ಣಾಙ್ಕ)in_part->ch_fd + 1, ರಿಕ್ತ, &wfds, ರಿಕ್ತ, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ಯದಿ (ret == -1 && errno == EINTR)
		ಅನುವರ್ತ;
#  endif
	    ಯದಿ (ret <= 0 || in_part->ch_block_write == 1)
	    {
		ಯದಿ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		ಅನ್ಯಥಾ
		    ch_log(channel, "Input not ready for writing");
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    ಭಙ್ಗ;
	}
# ಅನ್ಯಥಾ
	ಸಂರಚನಾ pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	ಯದಿ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	ಯದಿ (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
# endif
    }
#endif
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    ಶೂನ್ಯ
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    ಪೂರ್ಣಾಙ್ಕ		written = 0;

    ಯದಿ (buf == ರಿಕ್ತ || in_part->ch_buf_append)
	ಪ್ರತ್ಯಾಗಮ;  // no buffer or using appending
    ಯದಿ (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == ರಿಕ್ತ)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿ (lnum = in_part->ch_buf_ತುದಿ; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ; ++lnum)
    {
	ಯದಿ (!can_write_buf_ಸಾಲು(channel))
	    ಭಙ್ಗ;
	write_buf_ಸಾಲು(buf, lnum, channel);
	++written;
    }

    ಯದಿ (written == 1)
	ch_log(channel, "written line %d to channel", (ಪೂರ್ಣಾಙ್ಕ)lnum - 1);
    ಅನ್ಯಥಾ ಯದಿ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_ತುದಿ = lnum;
    ಯದಿ (lnum > buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ || lnum > in_part->ch_buf_bot)
    {
#ಯದಿ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ಯದಿ (channel->ch_job != ರಿಕ್ತ)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = ರಿಕ್ತ;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	ಯದಿ (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    ಅನ್ಯಥಾ
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    ಶೂನ್ಯ
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	ಪ್ರತಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ಯದಿ (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = ರಿಕ್ತ;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    ಶೂನ್ಯ
channel_write_ಒಳಹರಿವು(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    ಯದಿ (in_part->ch_writeque.wq_next != ರಿಕ್ತ)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    ಅನ್ಯಥಾ ಯದಿ (in_part->ch_bufref.br_buf != ರಿಕ್ತ)
    {
	ಯದಿ (in_part->ch_buf_append)
	    channel_write_new_ಸಾಲುಗಳು(in_part->ch_bufref.br_buf);
	ಅನ್ಯಥಾ
	    channel_write_in(channel);
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
    ಪೂರ್ಣಾಙ್ಕ		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	ಪೂರ್ಣಾಙ್ಕ	    written = 0;

	ಯದಿ (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    ಯದಿ (in_part->ch_fd == INVALID_FD)
		ಅನುವರ್ತ;  // pipe was closed
	    found_one = TRUE;
	    ಪ್ರತಿ (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
								       ++lnum)
	    {
		ಯದಿ (!can_write_buf_ಸಾಲು(channel))
		    ಭಙ್ಗ;
		write_buf_ಸಾಲು(buf, lnum, channel);
		++written;
	    }

	    ಯದಿ (written == 1)
		ch_log(channel, "written line %d to channel", (ಪೂರ್ಣಾಙ್ಕ)lnum - 1);
	    ಅನ್ಯಥಾ ಯದಿ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ಯದಿ (lnum < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    ಯದಿ (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    ಯದಿ (safe_to_invoke_callback == 0)
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

    ಪ್ರತ್ಯಾಗಮ head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *ಗಂಟು)
{
    char_u  *buffer = ಗಂಟು->rq_buffer;
    long_u  i;

    ಪ್ರತಿ (i = 0; i < ಗಂಟು->rq_buflen; ++i)
	ಯದಿ (buffer[i] == NL)
	    ಪ್ರತ್ಯಾಗಮ buffer + i;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಙ್ಕ *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ಗಂಟು = head->rq_next;
    char_u *p;

    ಯದಿ (ಗಂಟು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (outlen != ರಿಕ್ತ)
	*outlen += ಗಂಟು->rq_buflen;
    // dispose of the node but keep the buffer
    p = ಗಂಟು->rq_buffer;
    head->rq_next = ಗಂಟು->rq_next;
    ಯದಿ (ಗಂಟು->rq_next == ರಿಕ್ತ)
	head->rq_prev = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	ಗಂಟು->rq_next->rq_prev = ರಿಕ್ತ;
    vim_free(ಗಂಟು);
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ಸ್ಥಿರ char_u *
channel_get_all(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಙ್ಕ *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ಗಂಟು;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    ಪ್ರತಿ (ಗಂಟು = head->rq_next; ಗಂಟು != ರಿಕ್ತ; ಗಂಟು = ಗಂಟು->rq_next)
	len += ಗಂಟು->rq_buflen;
    res = alloc(len + 1);
    ಯದಿ (res == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    p = res;
    ಪ್ರತಿ (ಗಂಟು = head->rq_next; ಗಂಟು != ರಿಕ್ತ; ಗಂಟು = ಗಂಟು->rq_next)
    {
	mch_memmove(p, ಗಂಟು->rq_buffer, ಗಂಟು->rq_buflen);
	p += ಗಂಟು->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    ಕುರು
    {
	p = channel_get(channel, part, ರಿಕ್ತ);
	vim_free(p);
    } ಯಾವತ್ (p != ರಿಕ್ತ);

    ಯದಿ (outlen != ರಿಕ್ತ)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ಪ್ರತ್ಯಾಗಮ res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ಯಾವತ್ (p < res + len)
    {
	ಯದಿ (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	ಅನ್ಯಥಾ ಯದಿ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ಯದಿ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ಯಾವತ್ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ಯದಿ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    ಶೂನ್ಯ
channel_consume(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಙ್ಕ len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ಗಂಟು = head->rq_next;
    char_u *buf = ಗಂಟು->rq_buffer;

    ಯದಿ (len < 0 || (long_u)len > ಗಂಟು->rq_buflen)
	ಪ್ರತ್ಯಾಗಮ;
    mch_memmove(buf, buf + len, ಗಂಟು->rq_buflen - len);
    ಗಂಟು->rq_buflen -= len;
    ಗಂಟು->rq_buffer[ಗಂಟು->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_collapse(channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಙ್ಕ want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*ಗಂಟು = head->rq_next;
    readq_T	*last_ಗಂಟು;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ಯದಿ (ಗಂಟು == ರಿಕ್ತ || ಗಂಟು->rq_next == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    last_ಗಂಟು = ಗಂಟು->rq_next;
    len = ಗಂಟು->rq_buflen + last_ಗಂಟು->rq_buflen;
    ಯದಿ (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	ಯಾವತ್ (last_ಗಂಟು->rq_next != ರಿಕ್ತ
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_ಗಂಟು) == ರಿಕ್ತ))
	{
	    last_ಗಂಟು = last_ಗಂಟು->rq_next;
	    len += last_ಗಂಟು->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ಯದಿ (newbuf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;	    // out of memory
    mch_memmove(p, ಗಂಟು->rq_buffer, ಗಂಟು->rq_buflen);
    p += ಗಂಟು->rq_buflen;
    vim_free(ಗಂಟು->rq_buffer);
    ಗಂಟು->rq_buffer = newbuf;
    ಪ್ರತಿ (n = ಗಂಟು; n != last_ಗಂಟು; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    ಗಂಟು->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    ಪ್ರತಿ (n = ಗಂಟು->rq_next; n != last_ಗಂಟು; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    ಗಂಟು->rq_next = last_ಗಂಟು->rq_next;
    ಯದಿ (last_ಗಂಟು->rq_next == ರಿಕ್ತ)
	head->rq_prev = ಗಂಟು;
    ಅನ್ಯಥಾ
	last_ಗಂಟು->rq_next->rq_prev = ಗಂಟು;
    vim_free(last_ಗಂಟು);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_save(channel_T *channel, ch_part_T part, char_u *buf, ಪೂರ್ಣಾಙ್ಕ len,
						      ಪೂರ್ಣಾಙ್ಕ prepend, ವರ್ಣ *lead)
{
    readq_T *ಗಂಟು;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಗಂಟು = ALLOC_ONE(readq_T);
    ಯದಿ (ಗಂಟು == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    ಗಂಟು->rq_buffer = alloc(len + 1);
    ಯದಿ (ಗಂಟು->rq_buffer == ರಿಕ್ತ)
    {
	vim_free(ಗಂಟು);
	ಪ್ರತ್ಯಾಗಮ FAIL;	    // out of memory
    }

    ಯದಿ (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = ಗಂಟು->rq_buffer;
	ಪ್ರತಿ (i = 0; i < len; ++i)
	    ಯದಿ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	ಗಂಟು->rq_buflen = (long_u)(p - ಗಂಟು->rq_buffer);
    }
    ಅನ್ಯಥಾ
    {
	mch_memmove(ಗಂಟು->rq_buffer, buf, len);
	ಗಂಟು->rq_buffer[len] = NUL;
	ಗಂಟು->rq_buflen = (long_u)len;
    }

    ಯದಿ (prepend)
    {
	// prepend node to the head of the queue
	ಗಂಟು->rq_next = head->rq_next;
	ಗಂಟು->rq_prev = ರಿಕ್ತ;
	ಯದಿ (head->rq_next == ರಿಕ್ತ)
	    head->rq_prev = ಗಂಟು;
	ಅನ್ಯಥಾ
	    head->rq_next->rq_prev = ಗಂಟು;
	head->rq_next = ಗಂಟು;
    }
    ಅನ್ಯಥಾ
    {
	// append node to the tail of the queue
	ಗಂಟು->rq_next = ರಿಕ್ತ;
	ಗಂಟು->rq_prev = head->rq_prev;
	ಯದಿ (head->rq_prev == ರಿಕ್ತ)
	    head->rq_next = ಗಂಟು;
	ಅನ್ಯಥಾ
	    head->rq_prev->rq_next = ಗಂಟು;
	head->rq_prev = ಗಂಟು;
    }

    ಯದಿ (ch_log_active() && lead != ರಿಕ್ತ)
	ch_log_literal(lead, channel, part, buf, len);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, ರಿಕ್ತ);
    ಪೂರ್ಣಾಙ್ಕ		keeplen;
    ಪೂರ್ಣಾಙ್ಕ		addlen;
    char_u	*p;

    ಯದಿ (next == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    keeplen = reader->js_end - reader->js_buf;
    ಯದಿ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    vim_free(next);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    ಪ್ರತ್ಯಾಗಮ TRUE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*ಸಾಲು_start;
    char_u	*p;
    int_u	hdr_len;
    ಪೂರ್ಣಾಙ್ಕ		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ಯಾವತ್ (TRUE)
    {
	ಸಾಲು_start = p;
	ಯಾವತ್ (*p != NUL && *p != '\n')
	    p++;
	ಯದಿ (*p == NUL)			// partial header
	    ಪ್ರತ್ಯಾಗಮ MAYBE;
	p++;

	// process the content length field (if present)
	ಯದಿ ((p - ಸಾಲು_start > 16)
		&& STRNICMP(ಸಾಲು_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((ವರ್ಣ *)ಸಾಲು_start + 16, ರಿಕ್ತ, 10);
	    ಯದಿ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ ((p - ಸಾಲು_start) == 2 && ಸಾಲು_start[0] == '\r' &&
		ಸಾಲು_start[1] == '\n')
	    // reached the empty line
	    ಭಙ್ಗ;
    }

    ಯದಿ (payload_len == -1)
	// Content-Length field is not present in the header
	ಪ್ರತ್ಯಾಗಮ FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ಯದಿ (jsbuf_len < hdr_len + payload_len)
	ಪ್ರತ್ಯಾಗಮ MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    ಪೂರ್ಣಾಙ್ಕ		status = OK;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ಯದಿ (channel_peek(channel, part) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    reader.js_buf = channel_get(channel, part, ರಿಕ್ತ);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ಯದಿ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ಯದಿ (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ಯದಿ (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ಯದಿ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	ಅನ್ಯಥಾ ಯದಿ (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ಯದಿ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    ಅನ್ಯಥಾ
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	ಅನ್ಯಥಾ
	{
	    item = ALLOC_ONE(jsonq_T);
	    ಯದಿ (item == ರಿಕ್ತ)
		clear_tv(&listtv);
	    ಅನ್ಯಥಾ
	    {
		item->jq_no_callback = FALSE;
		item->jq_ಮೌಲ್ಯ = alloc_tv();
		ಯದಿ (item->jq_ಮೌಲ್ಯ == ರಿಕ್ತ)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		ಅನ್ಯಥಾ
		{
		    *item->jq_ಮೌಲ್ಯ = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = ರಿಕ್ತ;
		    ಯದಿ (item->jq_prev == ರಿಕ್ತ)
			head->jq_next = item;
		    ಅನ್ಯಥಾ
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    ಯದಿ (status == OK)
	chanpart->ch_wait_len = 0;
    ಅನ್ಯಥಾ ಯದಿ (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ಯದಿ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (ಪೂರ್ಣಾಙ್ಕ)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_ಎಣಿಕೆ() + 100L;
#ಅನ್ಯಥಾ
	    gettimeofday(&chanpart->ch_deadline, ರಿಕ್ತ);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ಯದಿ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ timeout;
#ifdef MSWIN
	    timeout = (ಪೂರ್ಣಾಙ್ಕ)(Get_Tick_ಎಣಿಕೆ() - chanpart->ch_deadline) > 0;
#ಅನ್ಯಥಾ
	    {
		ಸಂರಚನಾ timeval now_tv;

		gettimeofday(&now_tv, ರಿಕ್ತ);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    ಯದಿ (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    ಅನ್ಯಥಾ
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ಯದಿ (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    ಅನ್ಯಥಾ ಯದಿ (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(ಪೂರ್ಣಾಙ್ಕ)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, ರಿಕ್ತ);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    ಅನ್ಯಥಾ
	ret = FALSE;

    vim_free(reader.js_buf);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
remove_cb_ಗಂಟು(cbq_T *head, cbq_T *ಗಂಟು)
{
    ಯದಿ (ಗಂಟು->cq_prev == ರಿಕ್ತ)
	head->cq_next = ಗಂಟು->cq_next;
    ಅನ್ಯಥಾ
	ಗಂಟು->cq_prev->cq_next = ಗಂಟು->cq_next;
    ಯದಿ (ಗಂಟು->cq_next == ರಿಕ್ತ)
	head->cq_prev = ಗಂಟು->cq_prev;
    ಅನ್ಯಥಾ
	ಗಂಟು->cq_next->cq_prev = ಗಂಟು->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    ಶೂನ್ಯ
remove_json_ಗಂಟು(jsonq_T *head, jsonq_T *ಗಂಟು)
{
    ಯದಿ (ಗಂಟು->jq_prev == ರಿಕ್ತ)
	head->jq_next = ಗಂಟು->jq_next;
    ಅನ್ಯಥಾ
	ಗಂಟು->jq_prev->jq_next = ಗಂಟು->jq_next;
    ಯದಿ (ಗಂಟು->jq_next == ರಿಕ್ತ)
	head->jq_prev = ಗಂಟು->jq_prev;
    ಅನ್ಯಥಾ
	ಗಂಟು->jq_next->jq_prev = ಗಂಟು->jq_prev;
    vim_free(ಗಂಟು);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_add_block_id(chanpart_T *chanpart, ಪೂರ್ಣಾಙ್ಕ id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    ಯದಿ (gap->ga_growsize == 0)
	ga_init2(gap, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), 10);
    ಯದಿ (ga_grow(gap, 1) == OK)
    {
	((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_remove_block_id(chanpart_T *chanpart, ಪೂರ್ಣಾಙ್ಕ id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	ಯದಿ (((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    ಯದಿ (i < gap->ga_len)
	    {
		ಪೂರ್ಣಾಙ್ಕ *p = ((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ));
	    }
	    ಪ್ರತ್ಯಾಗಮ;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_has_block_id(chanpart_T *chanpart, ಪೂರ್ಣಾಙ್ಕ id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	ಯದಿ (((ಪೂರ್ಣಾಙ್ಕ *)gap->ga_data)[i] == id)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	ಪೂರ್ಣಾಙ್ಕ	    id,
	ಪೂರ್ಣಾಙ್ಕ	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    ಯಾವತ್ (item != ರಿಕ್ತ)
    {
	list_T	    *l;
	typval_T    *tv;

	ಯದಿ (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_ಮೌಲ್ಯ->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	ಅನ್ಯಥಾ
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ಯದಿ (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ಯದಿ (id > 0)
		{
		    ಯದಿ (item->jq_ಮೌಲ್ಯ->v_type != VAR_DICT)
			ಗಚ್ಛ nextitem;
		    d = item->jq_ಮೌಲ್ಯ->vval.v_dict;
		    ಯದಿ (d == ರಿಕ್ತ)
			ಗಚ್ಛ nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ಯದಿ (dict_has_ಕೀಲಿ(d, "method"))
			ಗಚ್ಛ nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    ಯದಿ (di == ರಿಕ್ತ)
			ಗಚ್ಛ nextitem;
		    tv = &di->di_tv;
		}
		ಅನ್ಯಥಾ
		    tv = item->jq_ಮೌಲ್ಯ;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (id > 0)
		{
		    ಯದಿ (item->jq_ಮೌಲ್ಯ->v_type != VAR_DICT)
			ಗಚ್ಛ nextitem;
		    d = item->jq_ಮೌಲ್ಯ->vval.v_dict;
		    ಯದಿ (d == ರಿಕ್ತ)
			ಗಚ್ಛ nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    ಯದಿ (di == ರಿಕ್ತ)
			ಗಚ್ಛ nextitem;
		    tv = &di->di_tv;
		}
		ಅನ್ಯಥಾ
		    tv = item->jq_ಮೌಲ್ಯ;
	    }
	}

	ಯದಿ ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_ಸಂಖ್ಯೆ && tv->vval.v_ಸಂಖ್ಯೆ == id)
	      || (id <= 0 && (tv->v_type != VAR_ಸಂಖ್ಯೆ
		 || tv->vval.v_ಸಂಖ್ಯೆ == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_ಸಂಖ್ಯೆ)))))
	{
	    *rettv = item->jq_ಮೌಲ್ಯ;
	    ಯದಿ (tv->v_type == VAR_ಸಂಖ್ಯೆ)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_ಸಂಖ್ಯೆ);
	    remove_json_ಗಂಟು(head, item);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
nextitem:
	item = item->jq_next;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_ತಳ್ಳು_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    ಯದಿ (head->jq_prev != ರಿಕ್ತ && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯಾವತ್ (item != ರಿಕ್ತ && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    ಯದಿ (newitem == ರಿಕ್ತ)
    {
	clear_tv(rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }

    newitem->jq_ಮೌಲ್ಯ = alloc_tv();
    ಯದಿ (newitem->jq_ಮೌಲ್ಯ == ರಿಕ್ತ)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ಪ್ರತ್ಯಾಗಮ;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_ಮೌಲ್ಯ = *rettv;
    ಯದಿ (item == ರಿಕ್ತ)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = ರಿಕ್ತ;
	ಯದಿ (newitem->jq_prev == ರಿಕ್ತ)
	    head->jq_next = newitem;
	ಅನ್ಯಥಾ
	    newitem->jq_prev->jq_next = newitem;
    }
    ಅನ್ಯಥಾ
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	ಯದಿ (newitem->jq_next == ರಿಕ್ತ)
	    head->jq_prev = newitem;
	ಅನ್ಯಥಾ
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

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
    ಪೂರ್ಣಾಙ್ಕ	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    ಯದಿ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ಯದಿ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	ಪ್ರತ್ಯಾಗಮ;
    }
    arg = argv[1].vval.v_string;
    ಯದಿ (arg == ರಿಕ್ತ)
	arg = (char_u *)"";

    ಯದಿ (STRCMP(cmd, "ex") == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ	called_emsg_before = called_emsg;
	char_u	*p = arg;
	ಪೂರ್ಣಾಙ್ಕ	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (ವರ್ಣ *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ಯದಿ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ಯದಿ (do_emsg_silent)
	    --emsg_silent;
	ಯದಿ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (ವರ್ಣ *)get_vim_var_str(VV_ERRMSG));
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (ವರ್ಣ *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_ಎಣಿಕೆ = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ is_call = cmd[0] == 'c';
	ಪೂರ್ಣಾಙ್ಕ id_idx = is_call ? 3 : 2;

	ಯದಿ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ಯದಿ (p_verbose > 2)
		emsg(_(e_last_argument_for_ಅಭಿವ್ಯಕ್ತಿ_call_must_be_ಸಂಖ್ಯೆ));
	}
	ಅನ್ಯಥಾ ಯದಿ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ಯದಿ (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	ಅನ್ಯಥಾ
	{
	    typval_T	*tv = ರಿಕ್ತ;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ರಿಕ್ತ;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ಯದಿ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (ವರ್ಣ *)arg);
		tv = eval_ಅಭಿವ್ಯಕ್ತಿ(arg, ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ
	    {
		ch_log(channel, "Calling '%s'", (ವರ್ಣ *)arg);
		ಯದಿ (func_call(arg, &argv[2], ರಿಕ್ತ, ರಿಕ್ತ, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ಯದಿ (argv[id_idx].v_type == VAR_ಸಂಖ್ಯೆ)
	    {
		ಪೂರ್ಣಾಙ್ಕ id = argv[id_idx].vval.v_ಸಂಖ್ಯೆ;

		ಯದಿ (tv != ರಿಕ್ತ)
		    json = json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(id, tv, options | JSON_NL);
		ಯದಿ (tv == ರಿಕ್ತ || (json != ರಿಕ್ತ && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(id, &err_tv, options | JSON_NL);
		}
		ಯದಿ (json != ರಿಕ್ತ)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(json), (ವರ್ಣ *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ಯದಿ (tv == &res_tv)
		clear_tv(tv);
	    ಅನ್ಯಥಾ
		free_tv(tv);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (ವರ್ಣ *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (ವರ್ಣ *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_ಗಂಟು(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
append_to_buffer(
    buf_T	*buffer,
    char_u	*ಸಂದೇಶ,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    ಪೂರ್ಣಾಙ್ಕ		save_p_ma = buffer->b_p_ma;
    ಪೂರ್ಣಾಙ್ಕ		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    ಯದಿ (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ಯದಿ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ಯದಿ (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (ಪೂರ್ಣಾಙ್ಕ)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    ಯದಿ (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ಪ್ರತ್ಯಾಗಮ;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    ಯದಿ (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, ಸಂದೇಶ, TRUE);
	lnum = 0;
    }
    ಅನ್ಯಥಾ
	ml_append(lnum, ಸಂದೇಶ, 0, FALSE);
    appended_ಸಾಲುಗಳು_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ಯದಿ (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    ಅನ್ಯಥಾ
	buffer->b_p_ma = save_p_ma;

    ಯದಿ (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ಯದಿ (wp->w_buffer == buffer)
	    {
		ಪೂರ್ಣಾಙ್ಕ move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ಯದಿ (move_cursor || wp->w_topline > buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
		{
		    win_T *save_curwin = curwin;

		    ಯದಿ (move_cursor)
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

    ಯದಿ (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    ಯದಿ (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
	}
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *ಸಂದೇಶ;

    ಯಾವತ್ ((ಸಂದೇಶ = channel_get(channel, part, ರಿಕ್ತ)) != ರಿಕ್ತ)
    {
	ch_log(channel, "Dropping message '%s'", (ವರ್ಣ *)ಸಂದೇಶ);
	vim_free(ಸಂದೇಶ);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    ಪ್ರತ್ಯಾಗಮ ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*ಸಂದೇಶ = ರಿಕ್ತ;
    blob_T	*blob = ರಿಕ್ತ;
    typval_T	*listtv = ರಿಕ್ತ;
    typval_T	argv[CH_JSON_MAX_ARGS];
    ಪೂರ್ಣಾಙ್ಕ		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = ರಿಕ್ತ;
    buf_T	*buffer = ರಿಕ್ತ;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		called_otc;		// one time callbackup
    ಪೂರ್ಣಾಙ್ಕ		raw_len = 0;

    ಯದಿ (channel->ch_nb_close_cb != ರಿಕ್ತ
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Use a message-specific callback, part callback or channel callback
    ಪ್ರತಿ (cbitem = cbhead->cq_next; cbitem != ರಿಕ್ತ; cbitem = cbitem->cq_next)
	ಯದಿ (cbitem->cq_seq_nr == 0)
	    ಭಙ್ಗ;
    ಯದಿ (cbitem != ರಿಕ್ತ)
	callback = &cbitem->cq_callback;
    ಅನ್ಯಥಾ ಯದಿ (ch_part->ch_callback.cb_name != ರಿಕ್ತ)
	callback = &ch_part->ch_callback;
    ಅನ್ಯಥಾ ಯದಿ (channel->ch_callback.cb_name != ರಿಕ್ತ)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    ಯದಿ (buffer != ರಿಕ್ತ && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == ರಿಕ್ತ))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = ರಿಕ್ತ;
	buffer = ರಿಕ್ತ;
    }

    ಯದಿ (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	ಪೂರ್ಣಾಙ್ಕ		argc = 0;

	// Get any json message in the queue.
	ಯದಿ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ಯದಿ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(ಶೂನ್ಯ)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ಯದಿ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	ಯದಿ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ಯದಿ (d != ರಿಕ್ತ)
	    {
		ಯದಿ (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		ಅನ್ಯಥಾ
		    di = dict_find(d, (char_u *)"request_seq", -1);
		ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
		    seq_nr = di->di_tv.vval.v_ಸಂಖ್ಯೆ;
	    }

	    argv[1] = *listtv;
	}
	ಅನ್ಯಥಾ
	{
	    ಪ್ರತಿ (item = listtv->vval.v_list->lv_first;
		    item != ರಿಕ್ತ && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    ಯಾವತ್ (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ಯದಿ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ಪ್ರತ್ಯಾಗಮ TRUE;
	    }

	    ಯದಿ (argv[0].v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    seq_nr = argv[0].vval.v_ಸಂಖ್ಯೆ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (channel_peek(channel, part) == ರಿಕ್ತ)
    {
	// nothing to read on RAW or NL channel
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಅನ್ಯಥಾ
    {
	// If there is no callback or buffer drop the message.
	ಯದಿ (callback == ರಿಕ್ತ && buffer == ರಿಕ್ತ)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ಯದಿ (channel->ch_close_cb.cb_name == ರಿಕ್ತ && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	ಯದಿ (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = ರಿಕ್ತ;
	    char_u  *buf;
	    readq_T *ಗಂಟು;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ಯಾವತ್ (TRUE)
	    {
		ಗಂಟು = channel_peek(channel, part);
		nl = channel_first_nl(ಗಂಟು);
		ಯದಿ (nl != ರಿಕ್ತ)
		    ಭಙ್ಗ;
		ಯದಿ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ಯದಿ (ch_part->ch_fd == INVALID_FD && ಗಂಟು->rq_buflen > 0)
			ಭಙ್ಗ;
		    ಪ್ರತ್ಯಾಗಮ FALSE; // incomplete message
		}
	    }
	    buf = ಗಂಟು->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    ಪ್ರತಿ (p = buf; (nl == ರಿಕ್ತ || p < nl)
					    && p < buf + ಗಂಟು->rq_buflen; ++p)
		ಯದಿ (*p == NUL)
		    *p = NL;

	    ಯದಿ (nl == ರಿಕ್ತ)
	    {
		// get the whole buffer, drop the NL
		ಸಂದೇಶ = channel_get(channel, part, ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (nl + 1 == buf + ಗಂಟು->rq_buflen)
	    {
		// get the whole buffer
		ಸಂದೇಶ = channel_get(channel, part, ರಿಕ್ತ);
		*nl = NUL;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		ಸಂದೇಶ = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (ಪೂರ್ಣಾಙ್ಕ)(nl - buf) + 1);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    ಸಂದೇಶ = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : ರಿಕ್ತ);
	}

	ಯದಿ (ಸಂದೇಶ == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FALSE; // out of memory (and avoids Coverity warning)

	ಯದಿ (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    ಯದಿ (blob == ರಿಕ್ತ)
	    {
		vim_free(ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    ಯದಿ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(ಸಂದೇಶ);
		ಪ್ರತ್ಯಾಗಮ FALSE;
	    }
	    ಯದಿ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, ಸಂದೇಶ, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	ಅನ್ಯಥಾ
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = ಸಂದೇಶ;
	}
    }

    called_otc = FALSE;
    ಯದಿ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	ಪೂರ್ಣಾಙ್ಕ lsp_req_ಸಂದೇಶ = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ಯದಿ ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_ಕೀಲಿ(argv[1].vval.v_dict, "method"))
	    lsp_req_ಸಂದೇಶ = TRUE;

	ಯದಿ (!lsp_req_ಸಂದೇಶ)
	{
	    ಪ್ರತಿ (cbitem = cbhead->cq_next; cbitem != ರಿಕ್ತ;
		    cbitem = cbitem->cq_next)
	    {
		ಯದಿ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    ಭಙ್ಗ;
		}
	    }
	}
    }

    ಯದಿ (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	ಯದಿ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ಯದಿ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ತಳ್ಳು_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ಸಂಖ್ಯೆ;
		free_tv(listtv);
		listtv = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (callback != ರಿಕ್ತ || buffer != ರಿಕ್ತ)
    {
	ಯದಿ (buffer != ರಿಕ್ತ)
	{
	    ಯದಿ (ಸಂದೇಶ == ರಿಕ್ತ)
		// JSON or JS mode: re-encode the message.
		ಸಂದೇಶ = json_encode(listtv, ch_mode);
	    ಯದಿ (ಸಂದೇಶ != ರಿಕ್ತ)
	    {
#ifdef FEAT_TERMINAL
		ಯದಿ (buffer->b_term != ರಿಕ್ತ)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ಯದಿ (part == PART_ERR)
		    {
			char_u *cp;
			ಪೂರ್ಣಾಙ್ಕ    extra = 0;

			ಪ್ರತಿ (cp = ಸಂದೇಶ; *cp != NUL; ++cp)
			    ಯದಿ (*cp == NL && (cp == ಸಂದೇಶ || cp[-1] != CAR))
				++extra;
			ಯದಿ (extra > 0)
			{
			    char_u *crlf_ಸಂದೇಶ = alloc(STRLEN(ಸಂದೇಶ) + extra + 1);

			    ಯದಿ (crlf_ಸಂದೇಶ != ರಿಕ್ತ)
			    {
				char_u *q = crlf_ಸಂದೇಶ;

				ಪ್ರತಿ (cp = ಸಂದೇಶ; *cp != NUL; ++cp)
				{
				    ಯದಿ (*cp == NL && (cp == ಸಂದೇಶ || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_ಸಂದೇಶ, channel);
				vim_free(crlf_ಸಂದೇಶ);
			    }
			}
			ಅನ್ಯಥಾ
			    write_to_term(buffer, ಸಂದೇಶ, channel);
		    }
		    ಅನ್ಯಥಾ
			write_to_term(buffer, ಸಂದೇಶ, channel);
		}
		ಅನ್ಯಥಾ
#endif
		    append_to_buffer(buffer, ಸಂದೇಶ, channel, part);
	    }
	}

	ಯದಿ (callback != ರಿಕ್ತ)
	{
	    ಯದಿ (cbitem != ರಿಕ್ತ)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    ಅನ್ಯಥಾ
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (ವರ್ಣ *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    ಅನ್ಯಥಾ
	ch_log(channel, "Dropping message %d", seq_nr);

    ಯದಿ (listtv != ರಿಕ್ತ)
	free_tv(listtv);
    ಯದಿ (blob != ರಿಕ್ತ)
	blob_unref(blob);
    vim_free(ಸಂದೇಶ);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ಯದಿ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_can_write_to(channel_T *channel)
{
    ಪ್ರತ್ಯಾಗಮ channel != ರಿಕ್ತ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_is_open(channel_T *channel)
{
    ಪ್ರತ್ಯಾಗಮ channel != ರಿಕ್ತ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    ಯದಿ (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	ಯದಿ (head->jq_next == ರಿಕ್ತ)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ಪ್ರತ್ಯಾಗಮ head->jq_next;
    }
    ಪ್ರತ್ಯಾಗಮ channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ಪ್ರತ್ಯಾಗಮ channel_readahead_pointer(channel, part) != ರಿಕ್ತ;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ಸ್ಥಿರ ವರ್ಣ *
channel_status(channel_T *channel, ಪೂರ್ಣಾಙ್ಕ req_part)
{
    ch_part_T part;
    ಪೂರ್ಣಾಙ್ಕ has_readahead = FALSE;

    ಯದಿ (channel == ರಿಕ್ತ)
	 ಪ್ರತ್ಯಾಗಮ "fail";
    ಯದಿ (req_part == PART_OUT)
    {
	ಯದಿ (channel->CH_OUT_FD != INVALID_FD)
	    ಪ್ರತ್ಯಾಗಮ "open";
	ಯದಿ (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (req_part == PART_ERR)
    {
	ಯದಿ (channel->CH_ERR_FD != INVALID_FD)
	    ಪ್ರತ್ಯಾಗಮ "open";
	ಯದಿ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (channel_is_open(channel))
	    ಪ್ರತ್ಯಾಗಮ "open";
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	    ಯದಿ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		ಭಙ್ಗ;
	    }
    }

    ಯದಿ (has_readahead)
	ಪ್ರತ್ಯಾಗಮ "buffered";
    ಪ್ರತ್ಯಾಗಮ "closed";
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_part_info(channel_T *channel, dict_T *dict, ವರ್ಣ *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    ವರ್ಣ	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    ಯದಿ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    ಅನ್ಯಥಾ ಯದಿ (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    ಅನ್ಯಥಾ
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (ಪೂರ್ಣಾಙ್ಕ)s.length);

    STRCPY(namebuf + tail, "mode");
    ವಿಕಲ್ಪ (chanpart->ch_mode)
    {
	ಪಕ್ಷ CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    ಭಙ್ಗ;
	ಪಕ್ಷ CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    ಭಙ್ಗ;
	ಪಕ್ಷ CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    ಭಙ್ಗ;
	ಪಕ್ಷ CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    ಭಙ್ಗ;
	ಪಕ್ಷ CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    ಭಙ್ಗ;
	ಪಕ್ಷ CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    ಭಙ್ಗ;
	ಪಕ್ಷ CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    STR_LITERAL_SET(s, "");
	    ಭಙ್ಗ;
    }
    dict_add_string_len(dict, namebuf, s.string, (ಪೂರ್ಣಾಙ್ಕ)s.length);

    STRCPY(namebuf + tail, "io");
    ಯದಿ (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    ಅನ್ಯಥಾ ವಿಕಲ್ಪ (chanpart->ch_io)
    {
	ಪಕ್ಷ JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    ಭಙ್ಗ;
	ಪಕ್ಷ JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    ಭಙ್ಗ;
	ಪಕ್ಷ JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    ಭಙ್ಗ;
	ಪಕ್ಷ JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    ಭಙ್ಗ;
	ಪಕ್ಷ JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    STR_LITERAL_SET(s, "");
	    ಭಙ್ಗ;
    }
    dict_add_string_len(dict, namebuf, s.string, (ಪೂರ್ಣಾಙ್ಕ)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_ಸಂಖ್ಯೆ(dict, namebuf, chanpart->ch_timeout);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_ಸಂಖ್ಯೆ(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    ಯದಿ (channel->ch_hostname != ರಿಕ್ತ)
    {
	ಯದಿ (channel->ch_port)
	    dict_add_ಸಂಖ್ಯೆ(dict, "port", channel->ch_port);
	ಅನ್ಯಥಾ
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    ಅನ್ಯಥಾ
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
    ಶೂನ್ಯ
channel_close(channel_T *channel, ಪೂರ್ಣಾಙ್ಕ invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    ಯದಿ (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	ಯದಿ (channel->ch_close_cb.cb_name != ರಿಕ್ತ)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    ಯದಿ (channel->ch_close_cb.cb_name != ರಿಕ್ತ
			    || channel->ch_part[part].ch_bufref.br_buf != ರಿಕ್ತ)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ಯದಿ (channel->ch_close_cb.cb_name == ರಿಕ್ತ)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		ಯಾವತ್ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ಯದಿ (channel->ch_close_cb.cb_name != ರಿಕ್ತ)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (ವರ್ಣ *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      ಯದಿ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ಯದಿ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ರಿಕ್ತ;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ರಿಕ್ತ;
    channel->ch_ss_close_cb = ರಿಕ್ತ;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    ಯದಿ (wq->wq_next == ರಿಕ್ತ)
	wq->wq_prev = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	wq->wq_next->wq_prev = ರಿಕ್ತ;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    ಯಾವತ್ (channel_peek(channel, part) != ರಿಕ್ತ)
	vim_free(channel_get(channel, part, ರಿಕ್ತ));

    ಯಾವತ್ (cb_head->cq_next != ರಿಕ್ತ)
    {
	cbq_T *ಗಂಟು = cb_head->cq_next;

	remove_cb_ಗಂಟು(cb_head, ಗಂಟು);
	free_callback(&ಗಂಟು->cq_callback);
	vim_free(ಗಂಟು);
    }

    ಯಾವತ್ (json_head->jq_next != ರಿಕ್ತ)
    {
	free_tv(json_head->jq_next->jq_ಮೌಲ್ಯ);
	remove_json_ಗಂಟು(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    ಯಾವತ್ (ch_part->ch_writeque.wq_next != ರಿಕ್ತ)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
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
    channel_clear_one(channel, PART_OUT);
    channel_clear_one(channel, PART_ERR);
    channel_clear_one(channel, PART_IN);
    free_callback(&channel->ch_callback);
    free_callback(&channel->ch_close_cb);
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
channel_free_all(ಶೂನ್ಯ)
{
    channel_T *channel;

    ch_log(ರಿಕ್ತ, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_ಸಂದೇಶ_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    ಯದಿ (in_part->ch_writeque.wq_next != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    : (in_part->ch_buf_ತುದಿ <= in_part->ch_buf_bot
			    && in_part->ch_buf_ತುದಿ <= buf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
}

#ಯದಿ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_fill_wfds(ಪೂರ್ಣಾಙ್ಕ maxfd_arg, fd_set *wfds)
{
    ಪೂರ್ಣಾಙ್ಕ		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ಯದಿ (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (ಪೂರ್ಣಾಙ್ಕ)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((ಪೂರ್ಣಾಙ್ಕ)in_part->ch_fd, wfds);
	    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)in_part->ch_fd >= maxfd)
		maxfd = (ಪೂರ್ಣಾಙ್ಕ)in_part->ch_fd + 1;
	}
    }
    ಪ್ರತ್ಯಾಗಮ maxfd;
}
#ಅನ್ಯಥಾ
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_fill_poll_write(ಪೂರ್ಣಾಙ್ಕ nfd_in, ಸಂರಚನಾ pollfd *fds)
{
    ಪೂರ್ಣಾಙ್ಕ		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ಯದಿ (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	ಅನ್ಯಥಾ
	    in_part->ch_poll_idx = -1;
    }
    ಪ್ರತ್ಯಾಗಮ nfd;
}
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
    ಸ್ಥಿರ channel_wait_result
channel_wait(channel_T *channel, sock_T fd, ಪೂರ್ಣಾಙ್ಕ timeout)
{
    ಯದಿ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    ಯದಿ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	ಪೂರ್ಣಾಙ್ಕ	sleep_time;
	DWORD	deadline = Get_Tick_ಎಣಿಕೆ() + timeout;
	ಪೂರ್ಣಾಙ್ಕ	delay = 1;

	// reading from a pipe, not a socket
	ಯಾವತ್ (TRUE)
	{
	    ಪೂರ್ಣಾಙ್ಕ r = PeekNamedPipe((HANDLE)fd, ರಿಕ್ತ, 0, ರಿಕ್ತ, &nread, ರಿಕ್ತ);

	    ಯದಿ (r && nread > 0)
		ಪ್ರತ್ಯಾಗಮ CW_READY;

	    ಯದಿ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ರಿಕ್ತ);
	    }
	    ಅನ್ಯಥಾ ಯದಿ (r == 0)
		ಪ್ರತ್ಯಾಗಮ CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_ಸಾಲುಗಳು();

	    sleep_time = deadline - Get_Tick_ಎಣಿಕೆ();
	    ಯದಿ (sleep_time <= 0)
		ಭಙ್ಗ;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ಯದಿ (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    ಯದಿ (delay > 10)
		delay = 10;
	}
    }
    ಅನ್ಯಥಾ
#endif
    {
#ಯದಿ defined(HAVE_SELECT)
	ಸಂರಚನಾ timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	ಪೂರ್ಣಾಙ್ಕ		ret;
	ಪೂರ್ಣಾಙ್ಕ		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	ಪ್ರತಿ (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((ಪೂರ್ಣಾಙ್ಕ)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (ಪೂರ್ಣಾಙ್ಕ)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ರಿಕ್ತ, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ಯದಿ (ret == -1 && errno == EINTR)
		ಅನುವರ್ತ;
# endif
	    ಯದಿ (ret > 0)
	    {
		ಯದಿ (FD_ISSET(fd, &rfds))
		    ಪ್ರತ್ಯಾಗಮ CW_READY;
		channel_write_any_ಸಾಲುಗಳು();
		ಅನುವರ್ತ;
	    }
	    ಭಙ್ಗ;
	}
#ಅನ್ಯಥಾ
	ಪ್ರತಿ (;;)
	{
	    ಸಂರಚನಾ pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    ಪೂರ್ಣಾಙ್ಕ		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ಯದಿ (poll(fds, nfd, timeout) > 0)
	    {
		ಯದಿ (fds[0].revents & POLLIN)
		    ಪ್ರತ್ಯಾಗಮ CW_READY;
		channel_write_any_ಸಾಲುಗಳು();
		ಅನುವರ್ತ;
	    }
	    ಭಙ್ಗ;
	}
#endif
    }
    ಪ್ರತ್ಯಾಗಮ CW_NOT_READY;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಙ್ಕ is_err, ವರ್ಣ *func)
{
    ವರ್ಣ	ಸಂದೇಶ[] = "%s(): Read %s from ch_part[%d], closing";

    ಯದಿ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, ಸಂದೇಶ, func, "error", part);
    ಅನ್ಯಥಾ
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
    ಯದಿ (channel->ch_nb_close_cb != ರಿಕ್ತ)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_ಸಂದೇಶ_RAW,
			      (ಪೂರ್ಣಾಙ್ಕ)STRLEN_LITERAL(DETACH_ಸಂದೇಶ_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ಯದಿ (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	ಯದಿ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    ಯದಿ (channel->ch_nb_close_cb != ರಿಕ್ತ)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    ಯದಿ (channel->ch_ss_close_cb != ರಿಕ್ತ)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_read(channel_T *channel, ch_part_T part, ವರ್ಣ *func)
{
    ಸ್ಥಿರ char_u	*buf = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			len = 0;
    ಪೂರ್ಣಾಙ್ಕ			readlen = 0;
    sock_T		fd;
    ಪೂರ್ಣಾಙ್ಕ			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ಯದಿ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	ಪ್ರತ್ಯಾಗಮ;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ಯದಿ (buf == ರಿಕ್ತ)
    {
	buf = alloc(MAXMSGSIZE);
	ಯದಿ (buf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    ಪ್ರತಿ (;;)
    {
	ಯದಿ (channel_wait(channel, fd, 0) != CW_READY)
	    ಭಙ್ಗ;
	ಯದಿ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    ಸಂರಚನಾ sockaddr_storage	client;

	    newchannel = add_channel();
	    ಯದಿ (newchannel == ರಿಕ್ತ)
	    {
		ch_error(ರಿಕ್ತ, "Cannot allocate channel.");
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    socklen = ಪರಿಮಾಣ(client);
	    newfd = accept(fd, (ಸಂರಚನಾ sockaddr*)&client, &socklen);
	    ಯದಿ (newfd < 0)
	    {
		ch_error(ರಿಕ್ತ, "Cannot accept channel.");
		channel_free(newchannel);
		ಪ್ರತ್ಯಾಗಮ;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    ಯದಿ (channel->ch_ss_accept_cb != ರಿಕ್ತ)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ಪ್ರತ್ಯಾಗಮ;
	    }
#endif

	    ಯದಿ (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		ವರ್ಣ addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((ಸಂರಚನಾ sockaddr_in*)&client)->sin_addr,
			addr, ಪರಿಮಾಣ(addr));
		vim_snprintf((ವರ್ಣ *)namebuf, ಪರಿಮಾಣ(namebuf), "%s:%d",
			addr,
			ntohs(((ಸಂರಚನಾ sockaddr_in*)&client)->sin_port));
#ಅನ್ಯಥಾ
		vim_snprintf((ವರ್ಣ *)namebuf, ಪರಿಮಾಣ(namebuf), "%s:%d",
		    inet_ntoa(((ಸಂರಚನಾ sockaddr_in*)&client)->sin_addr),
		    ntohs(((ಸಂರಚನಾ sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    ಅನ್ಯಥಾ ಯದಿ (client.ss_family == AF_INET6)
	    {
		ವರ್ಣ addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((ಸಂರಚನಾ sockaddr_in6*)&client)->sin6_addr,
			addr, ಪರಿಮಾಣ(addr));
		vim_snprintf((ವರ್ಣ *)namebuf, ಪರಿಮಾಣ(namebuf), "[%s]:%d",
			addr,
			ntohs(((ಸಂರಚನಾ sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    ಅನ್ಯಥಾ ಯದಿ (client.ss_family == AF_UNIX)
		vim_snprintf((ವರ್ಣ *)namebuf, ಪರಿಮಾಣ(namebuf),
							    "unix:anonymous");
	    ಅನ್ಯಥಾ
		vim_snprintf((ವರ್ಣ *)namebuf, ಪರಿಮಾಣ(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	ಯದಿ (use_socket)
	    len = sock_read(fd, (ವರ್ಣ *)buf, MAXMSGSIZE);
	ಅನ್ಯಥಾ
	    len = fd_read(fd, (ವರ್ಣ *)buf, MAXMSGSIZE);
	ಯದಿ (len <= 0)
	    ಭಙ್ಗ;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ಯದಿ (readlen <= 0)
    {
	ಯದಿ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ಯದಿ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    ಅನ್ಯಥಾ ಯದಿ (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    ಶೂನ್ಯ
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
    ಸ್ಥಿರ char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, ಪೂರ್ಣಾಙ್ಕ timeout, ಪೂರ್ಣಾಙ್ಕ raw, ಪೂರ್ಣಾಙ್ಕ *outlen)
{
    char_u	*buf;
    char_u	*ಸಂದೇಶ;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*ಗಂಟು;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    ಯಾವತ್ (TRUE)
    {
	ಗಂಟು = channel_peek(channel, part);
	ಯದಿ (ಗಂಟು != ರಿಕ್ತ)
	{
	    ಯದಿ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(ಗಂಟು) != ರಿಕ್ತ))
		// got a complete message
		ಭಙ್ಗ;
	    ಯದಿ (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		ಅನುವರ್ತ;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ಯದಿ (raw || fd == INVALID_FD)
		ಭಙ್ಗ;
	}

	// Wait for up to the channel timeout.
	ಯದಿ (fd == INVALID_FD)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	ಯದಿ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ಯದಿ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != ರಿಕ್ತ)
    {
	ಸಂದೇಶ = channel_get_all(channel, part, outlen);
    }
    ಅನ್ಯಥಾ
    {
	char_u *p;

	buf = ಗಂಟು->rq_buffer;
	nl = channel_first_nl(ಗಂಟು);

	// Convert NUL to NL, the internal representation.
	ಪ್ರತಿ (p = buf; (nl == ರಿಕ್ತ || p < nl) && p < buf + ಗಂಟು->rq_buflen; ++p)
	    ಯದಿ (*p == NUL)
		*p = NL;

	ಯದಿ (nl == ರಿಕ್ತ)
	{
	    // must be a closed channel with missing NL
	    ಸಂದೇಶ = channel_get(channel, part, ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ ಯದಿ (nl + 1 == buf + ಗಂಟು->rq_buflen)
	{
	    // get the whole buffer
	    ಸಂದೇಶ = channel_get(channel, part, ರಿಕ್ತ);
	    *nl = NUL;
	}
	ಅನ್ಯಥಾ
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    ಸಂದೇಶ = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (ಪೂರ್ಣಾಙ್ಕ)(nl - buf) + 1);
	}
    }
    ಯದಿ (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != ರಿಕ್ತ ? *outlen : (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಸಂದೇಶ));
    ಪ್ರತ್ಯಾಗಮ ಸಂದೇಶ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_in_blocking_wait(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, ಪೂರ್ಣಾಙ್ಕ timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (ಶೂನ್ಯ)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	ಪೂರ್ಣಾಙ್ಕ	    timeout_arg,
	ಪೂರ್ಣಾಙ್ಕ	    id,
	typval_T    **rettv)
{
    ಪೂರ್ಣಾಙ್ಕ		more;
    sock_T	fd;
    ಪೂರ್ಣಾಙ್ಕ		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ಯದಿ (id >= 0)
	channel_add_block_id(chanpart, id);

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (ಶೂನ್ಯ)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	ಯದಿ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    ಭಙ್ಗ;
	}

	ಯದಿ (!more)
	{
	    ಶೂನ್ಯ *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    ಶೂನ್ಯ *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ಯದಿ (channel_parse_messages())
		ಅನುವರ್ತ;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    ಯದಿ (readahead_ptr != ರಿಕ್ತ && readahead_ptr != prev_readahead_ptr)
		ಅನುವರ್ತ;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (ಪೂರ್ಣಾಙ್ಕ)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    ಯದಿ (timeout <= 0 || fd == INVALID_FD)
	    {
		ಯದಿ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		ಭಙ್ಗ;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ಯದಿ (chanpart->ch_wait_len > 0)
	    {
		ಪೂರ್ಣಾಙ್ಕ	ಎಡ;
#ifdef MSWIN
		ಎಡ = chanpart->ch_deadline - Get_Tick_ಎಣಿಕೆ() + 1;
#ಅನ್ಯಥಾ
		{
		    ಸಂರಚನಾ timeval now_tv;

		    gettimeofday(&now_tv, ರಿಕ್ತ);
		    ಎಡ = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		ಯದಿ (ಎಡ < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		ಅನ್ಯಥಾ ಯದಿ (ಎಡ < timeout)
		    timeout = ಎಡ;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    ಯದಿ (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ check_open, ಪೂರ್ಣಾಙ್ಕ reading, ch_part_T part)
{
    channel_T	*channel = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		has_readahead = FALSE;

    ಯದಿ (tv->v_type == VAR_JOB)
    {
	ಯದಿ (tv->vval.v_job != ರಿಕ್ತ)
	    channel = tv->vval.v_job->jv_channel;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (channel != ರಿಕ್ತ && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_ಎಣಿಕೆ ? part : channel_part_read(channel));

    ಯದಿ (check_open && (channel == ರಿಕ್ತ || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
common_channel_read(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ raw, ಪೂರ್ಣಾಙ್ಕ blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_ಎಣಿಕೆ;
    jobopt_T	opt;
    ಪೂರ್ಣಾಙ್ಕ		mode;
    ಪೂರ್ಣಾಙ್ಕ		timeout;
    ಪೂರ್ಣಾಙ್ಕ		id = -1;
    typval_T	*listtv = ರಿಕ್ತ;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    clear_job_options(&opt);
    ಯದಿ (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	ಗಚ್ಛ theend;

    ಯದಿ (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    ಯದಿ (part == PART_ಎಣಿಕೆ)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    ಯದಿ (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ಯದಿ (blob || mode == CH_MODE_BLOB)
    {
	ಪೂರ್ಣಾಙ್ಕ	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    blob_T	*b = blob_alloc();

	    ಯದಿ (b != ರಿಕ್ತ)
	    {
		b->bv_ga.ga_len = outlen;
		ಯದಿ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		ಅನ್ಯಥಾ
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	ಯದಿ (listtv != ರಿಕ್ತ)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	ಅನ್ಯಥಾ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#ಯದಿ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    ಶೂನ್ಯ
channel_handle_events(ಪೂರ್ಣಾಙ್ಕ only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ಯದಿ (only_keep_open && !channel->ch_keep_open)
	    ಅನುವರ್ತ;

	// check the socket and pipes
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ಯದಿ (fd == INVALID_FD)
		ಅನುವರ್ತ;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    ಪೂರ್ಣಾಙ್ಕ r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    ಯದಿ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    ಅನ್ಯಥಾ ಯದಿ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ಯದಿ (channel->ch_job != ರಿಕ್ತ)
	{
	    job_T *job = channel->ch_job;

	    ಯದಿ (job->jv_tty_out != ರಿಕ್ತ && job->jv_status == JOB_FINISHED)
		ಪ್ರತಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#ಯದಿ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_any_keep_open(ಶೂನ್ಯ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ಯದಿ (channel->ch_keep_open)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    ಶೂನ್ಯ
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    ಪೂರ್ಣಾಙ್ಕ		fd = ch_part->ch_fd;

    ಯದಿ (fd == INVALID_FD)
	ಪ್ರತ್ಯಾಗಮ;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#ಅನ್ಯಥಾ
    (ಶೂನ್ಯ)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	ಪೂರ್ಣಾಙ್ಕ	  len_arg,
	ವರ್ಣ	  *fun)
{
    ಪೂರ್ಣಾಙ್ಕ		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ಪೂರ್ಣಾಙ್ಕ		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    ಯದಿ (fd == INVALID_FD)
    {
	ಯದಿ (!channel->ch_error && fun != ರಿಕ್ತ)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    ಯದಿ (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_ಸಂದೇಶ = 0;
    }

    ಪ್ರತಿ (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	ಪೂರ್ಣಾಙ್ಕ	    len;

	ಯದಿ (wq->wq_next != ರಿಕ್ತ)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ಪ್ರತ್ಯಾಗಮ OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ಯದಿ (part == PART_SOCK)
	    res = sock_write(fd, (ವರ್ಣ *)buf, len);
	ಅನ್ಯಥಾ
	{
	    res = fd_write(fd, (ವರ್ಣ *)buf, len);
#ifdef MSWIN
	    ಯದಿ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ರಿಕ್ತ);
	    }
#endif
	}
	ಯದಿ (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	ಯದಿ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    ಯದಿ (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    ಯದಿ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ಯದಿ (entry != ರಿಕ್ತ)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    ಅನುವರ್ತ;
		}
		ಯದಿ (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    ಅನ್ಯಥಾ
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ಯದಿ (entry != ರಿಕ್ತ)
		{
		    ಯದಿ (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (ವರ್ಣ *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		ಅನ್ಯಥಾ
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ಯದಿ (wq->wq_prev != ರಿಕ್ತ
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    ಯದಿ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((ವರ್ಣ *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    ಯದಿ (last != ರಿಕ್ತ)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = ರಿಕ್ತ;
			ಯದಿ (wq->wq_prev == ರಿಕ್ತ)
			    wq->wq_next = last;
			ಅನ್ಯಥಾ
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			ಯದಿ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (res != len)
	{
	    ಯದಿ (!channel->ch_error && fun != ರಿಕ್ತ)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	channel->ch_error = FALSE;
	ಪ್ರತ್ಯಾಗಮ OK;
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
	char_u	    *text,
	ಪೂರ್ಣಾಙ್ಕ	    len,
	ಪೂರ್ಣಾಙ್ಕ	    id,
	ಪೂರ್ಣಾಙ್ಕ	    eval,
	jobopt_T    *opt,
	ವರ್ಣ	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ಯದಿ (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ಯದಿ (opt->jo_callback.cb_name != ರಿಕ್ತ && *opt->jo_callback.cb_name != NUL)
    {
	ಯದಿ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ಯದಿ (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ channel;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ch_ಅಭಿವ್ಯಕ್ತಿ_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    ಪೂರ್ಣಾಙ್ಕ		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    ಪೂರ್ಣಾಙ್ಕ		timeout;
    ಪೂರ್ಣಾಙ್ಕ		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    ಯದಿ (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	ವರ್ಣ		*ಕೀಲಿ = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	ಯದಿ (rettv_dict_alloc(rettv) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	ಯದಿ (check_for_dict_arg(argvars, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)ಕೀಲಿ, -1);
	ಯದಿ (argvars[2].v_type == VAR_DICT
		&& dict_has_ಕೀಲಿ(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type != VAR_ಸಂಖ್ಯೆ
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_ಮೌಲ್ಯ_for_argument_str), ಕೀಲಿ);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	ಯದಿ (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_ಸಂದೇಶ_id;
	    ಯದಿ (di == ರಿಕ್ತ)
		dict_add_ಸಂಖ್ಯೆ(d, "seq", id);
	    ಅನ್ಯಥಾ
		di->di_tv.vval.v_ಸಂಖ್ಯೆ = id;
	}
	ಅನ್ಯಥಾ ಯದಿ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_ಸಂದೇಶ_id;
	    ಯದಿ (di == ರಿಕ್ತ)
		dict_add_ಸಂಖ್ಯೆ(d, "id", id);
	    ಅನ್ಯಥಾ
		di->di_tv.vval.v_ಸಂಖ್ಯೆ = id;
	}
	ಅನ್ಯಥಾ
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ಯದಿ (di != ರಿಕ್ತ && di->di_tv.v_type == VAR_ಸಂಖ್ಯೆ)
		id = di->di_tv.vval.v_ಸಂಖ್ಯೆ;
	}
	ಯದಿ (ch_mode == CH_MODE_LSP && !dict_has_ಕೀಲಿ(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_ಸಂದೇಶ(&argvars[1]);
    }
    ಅನ್ಯಥಾ
    {
	id = ++channel->ch_last_ಸಂದೇಶ_id;
	text = json_encode_nr_ಅಭಿವ್ಯಕ್ತಿ(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    ಯದಿ (text == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    channel = send_common(argvars, text, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    ಯದಿ (channel != ರಿಕ್ತ && eval)
    {
	ಯದಿ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ಅನ್ಯಥಾ
	    timeout = channel_get_timeout(channel, part_read);
	ಯದಿ (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ಯದಿ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ಸಂಖ್ಯೆ;
		free_tv(listtv);
	    }
	    ಅನ್ಯಥಾ
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_ಸಂಖ್ಯೆ;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    ಯದಿ (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ಯದಿ (rettv->vval.v_dict != ರಿಕ್ತ)
	    dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "seq", id);
    }
    ಅನ್ಯಥಾ ಯದಿ (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ಯದಿ (rettv->vval.v_dict != ರಿಕ್ತ)
	    dict_add_ಸಂಖ್ಯೆ(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ch_raw_common(typval_T *argvars, typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    ಪೂರ್ಣಾಙ್ಕ		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    ಪೂರ್ಣಾಙ್ಕ		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    ಅನ್ಯಥಾ
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ಯದಿ (channel != ರಿಕ್ತ && eval)
    {
	ಯದಿ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ಅನ್ಯಥಾ
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, ರಿಕ್ತ);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#ಯದಿ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_poll_setup(ಪೂರ್ಣಾಙ್ಕ nfd_in, ಶೂನ್ಯ *fds_in, ಪೂರ್ಣಾಙ್ಕ *towait)
{
    ಪೂರ್ಣಾಙ್ಕ		nfd = nfd_in;
    channel_T	*channel;
    ಸಂರಚನಾ	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ಯದಿ (ch_part->ch_fd != INVALID_FD)
	    {
		ಯದಿ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ಯದಿ (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		ಅನ್ಯಥಾ
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    ಅನ್ಯಥಾ
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    ಪ್ರತ್ಯಾಗಮ nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_poll_check(ಪೂರ್ಣಾಙ್ಕ ret_in, ಶೂನ್ಯ *fds_in)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = ret_in;
    channel_T	*channel;
    ಸಂರಚನಾ	pollfd *fds = fds_in;
    ch_part_T	part;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ಯದಿ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	ಯದಿ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_ಒಳಹರಿವು(channel);
	    --ret;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}
#endif // UNIX && !HAVE_SELECT

#ಯದಿ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_select_setup(
	ಪೂರ್ಣಾಙ್ಕ maxfd_in,
	ಶೂನ್ಯ *rfds_in,
	ಶೂನ್ಯ *wfds_in,
	ಸಂರಚನಾ timeval *tv,
	ಸಂರಚನಾ timeval **tvp)
{
    ಪೂರ್ಣಾಙ್ಕ		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ಯದಿ (fd != INVALID_FD)
	    {
		ಯದಿ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ಯದಿ (*tvp == ರಿಕ್ತ || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		ಅನ್ಯಥಾ
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ಯದಿ ((ಪೂರ್ಣಾಙ್ಕ)fd >= FD_SETSIZE)
			ಅನುವರ್ತ;
# endif
		    FD_SET((ಪೂರ್ಣಾಙ್ಕ)fd, rfds);
		    ಯದಿ (maxfd < (ಪೂರ್ಣಾಙ್ಕ)fd)
			maxfd = (ಪೂರ್ಣಾಙ್ಕ)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ಪ್ರತ್ಯಾಗಮ maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_select_check(ಪೂರ್ಣಾಙ್ಕ ret_in, ಶೂನ್ಯ *rfds_in, ಶೂನ್ಯ *wfds_in)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	ಪ್ರತಿ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ಯದಿ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ಪೂರ್ಣಾಙ್ಕ)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	ಯದಿ (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ಪೂರ್ಣಾಙ್ಕ)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_ಒಳಹರಿವು(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ಯದಿ (channel->ch_job != ರಿಕ್ತ)
	{
	    job_T *job = channel->ch_job;

	    ಯದಿ (job->jv_tty_out != ರಿಕ್ತ && job->jv_status == JOB_FINISHED)
		ಪ್ರತಿ (part = PART_SOCK; part < PART_ಎಣಿಕೆ; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ಪ್ರತ್ಯಾಗಮ ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    ಯದಿ (stdio_channel->CH_OUT_FD != INVALID_FD)
	(ಶೂನ್ಯ)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(ಪೂರ್ಣಾಙ್ಕ)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#ನೋಚೇದ್ಯದಿ defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    ಸಂರಚನಾ timeval	tv;
    ಸಂರಚನಾ timeval	*tvp = &tv;
    ಪೂರ್ಣಾಙ್ಕ			maxfd;
    ಪೂರ್ಣಾಙ್ಕ			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ರಿಕ್ತ, tvp);
    ಯದಿ (ret >= 0)
	(ಶೂನ್ಯ)channel_select_check(ret, &rfds, &wfds);
#ಅನ್ಯಥಾ
    ಸಂರಚನಾ pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    ಪೂರ್ಣಾಙ್ಕ			nfd;
    ಪೂರ್ಣಾಙ್ಕ			towait = (ಪೂರ್ಣಾಙ್ಕ)msec;
    ಪೂರ್ಣಾಙ್ಕ			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ಯದಿ (ret >= 0)
	(ಶೂನ್ಯ)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    ಶೂನ್ಯ
channel_stdio_loop(ಶೂನ್ಯ)
{
    ಯಾವತ್ (stdio_channel != ರಿಕ್ತ && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	ಯದಿ (due_time > 0 && due_time < msec)
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
    ಪೂರ್ಣಾಙ್ಕ
channel_parse_messages(ಶೂನ್ಯ)
{
    channel_T	*channel = first_channel;
    ಪೂರ್ಣಾಙ್ಕ		ret = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		r;
    ch_part_T	part = PART_SOCK;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = 0;
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
    ಯದಿ ((did_repeated_ಸಂದೇಶ & REPEATED_ಸಂದೇಶ_LOOKING) == 0)
    {
	ch_log(ರಿಕ್ತ, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_ಸಂದೇಶ = REPEATED_ಸಂದೇಶ_LOOKING;
    }
    ಯಾವತ್ (channel != ರಿಕ್ತ)
    {
	ಯದಿ (recursive == 1)
	{
	    ಯದಿ (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_ಎಣಿಕೆ);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ಯದಿ (channel->ch_job != ರಿಕ್ತ)
		    channel->ch_job->jv_channel = ರಿಕ್ತ;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		ಅನುವರ್ತ;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	ಯದಿ (part == PART_OUT
		&& channel->ch_job != ರಿಕ್ತ
		&& channel->ch_job->jv_tty_out != ರಿಕ್ತ
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    ಅನುವರ್ತ;
	}
#endif
	ಯದಿ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ಯದಿ (r == OK)
		ret = TRUE;
	    ಯದಿ (channel_unref(channel) || (r == OK
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
		ಅನುವರ್ತ;
	    }
	}
	ಯದಿ (part < PART_ERR)
	    ++part;
	ಅನ್ಯಥಾ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    ಯದಿ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    ಪೂರ್ಣಾಙ್ಕ
channel_any_readahead(ಶೂನ್ಯ)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    ಯಾವತ್ (channel != ರಿಕ್ತ)
    {
	ಯದಿ (channel_has_readahead(channel, part))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	ಯದಿ (part < PART_ERR)
	    ++part;
	ಅನ್ಯಥಾ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_channel(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    ಪ್ರತಿ (channel = first_channel; !abort && channel != ರಿಕ್ತ;
						   channel = channel->ch_next)
	ಯದಿ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
	}
    ಪ್ರತ್ಯಾಗಮ abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ಸ್ಥಿರ ch_part_T
channel_part_send(channel_T *channel)
{
    ಯದಿ (channel->CH_SOCK_FD == INVALID_FD)
	ಪ್ರತ್ಯಾಗಮ PART_IN;
    ಪ್ರತ್ಯಾಗಮ PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ಸ್ಥಿರ ch_part_T
channel_part_read(channel_T *channel)
{
    ಯದಿ (channel->CH_SOCK_FD == INVALID_FD)
	ಪ್ರತ್ಯಾಗಮ PART_OUT;
    ಪ್ರತ್ಯಾಗಮ PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ಸ್ಥಿರ ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ CH_MODE_JSON;
    ಪ್ರತ್ಯಾಗಮ channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ಪ್ರತ್ಯಾಗಮ channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    ಶೂನ್ಯ
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_ಸಂಖ್ಯೆ = 0;
    ಯದಿ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಯದಿ (channel != ರಿಕ್ತ)
	rettv->vval.v_ಸಂಖ್ಯೆ = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    ಶೂನ್ಯ
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ಯದಿ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಯದಿ (channel != ರಿಕ್ತ)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    ಶೂನ್ಯ
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ಯದಿ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ಯದಿ (channel != ರಿಕ್ತ)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    ಶೂನ್ಯ
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_ಸಂಖ್ಯೆ = -1;

    ಯದಿ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    char_u	*what = tv_get_string(&argvars[1]);
    ಪೂರ್ಣಾಙ್ಕ		part;
    ಯದಿ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "out") == 0)
	part = PART_OUT;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(what, "in") == 0)
	part = PART_IN;
    ಅನ್ಯಥಾ
	part = PART_SOCK;
    ಯದಿ (channel->ch_part[part].ch_bufref.br_buf != ರಿಕ್ತ)
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

    ಯದಿ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    ಯದಿ (channel->ch_job != ರಿಕ್ತ)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    ಶೂನ್ಯ
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ಯದಿ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಯದಿ (channel != ರಿಕ್ತ && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    ಶೂನ್ಯ
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    ಶೂನ್ಯ
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;
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
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    ಯದಿ (check_restricted() || check_secure())
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ಯದಿ (channel == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    clear_job_options(&opt);
    ಯದಿ (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    ಶೂನ್ಯ
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    ಪೂರ್ಣಾಙ್ಕ		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ರಿಕ್ತ;

    ಯದಿ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ಪ್ರತ್ಯಾಗಮ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ಯದಿ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	ಯದಿ (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    ವರ್ಣ      *status = channel_status(channel, -1);

    ಯದಿ (channel == ರಿಕ್ತ)
	vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN, "channel %s", status);
    ಅನ್ಯಥಾ
	vim_snprintf((ವರ್ಣ *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(ಪೂರ್ಣಾಙ್ಕ ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ಯದಿ (ch->ch_id == ch_id)
	    ಪ್ರತ್ಯಾಗಮ ch;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#endif // FEAT_JOB_CHANNEL
