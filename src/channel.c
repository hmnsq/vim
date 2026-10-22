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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)

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
തരനിർവചനം ഘടന sockaddr_un {
    ADDRESS_FAMILY sun_family;
    അക്ഷരം sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#അല്ലെങ്കിൽ
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) ലിഖ(sd, buf, len)
# define sock_read(sd, buf, len) പഠ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) പഠ(fd, buf, len)
# define fd_write(sd, buf, len) ലിഖ(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

സ്ഥിതം ശൂന്യം channel_read(channel_T *channel, ch_part_T part, അക്ഷരം *func);
സ്ഥിതം ശൂന്യം channel_wait_any(long msec);
സ്ഥിതം ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
സ്ഥിതം പൂർണ്ണസംഖ്യ channel_get_timeout(channel_T *channel, ch_part_T part);
സ്ഥിതം channel_T *channel_open_stdio(ശൂന്യം);
സ്ഥിതം ch_part_T channel_part_send(channel_T *channel);
സ്ഥിതം ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    വേണ്ടി ((ch) = first_channel; (ch) != ശൂന്യ; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
സ്ഥിതം പൂർണ്ണസംഖ്യ safe_to_invoke_callback = 0;

#ifdef MSWIN
    സ്ഥിതം പൂർണ്ണസംഖ്യ
fd_read(sock_T fd, അക്ഷരം *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    എങ്കിൽ (!ReadFile(h, buf, (DWORD)len, &nread, ശൂന്യ))
	തിരികെ -1;
    തിരികെ (പൂർണ്ണസംഖ്യ)nread;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
fd_write(sock_T fd, അക്ഷരം *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    ആകുമ്പോൾ (todo > 0)
    {
	എങ്കിൽ (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	അല്ലെങ്കിൽ
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	എങ്കിൽ (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    എങ്കിൽ (err != ERROR_IO_PENDING)
		തിരികെ -1;
	    എങ്കിൽ (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		തിരികെ -1;
	    FlushFileBuffers(h);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    നിർത്തുക;
	todo -= nwrite;
	done += nwrite;
    }
    തിരികെ (പൂർണ്ണസംഖ്യ)done;
}

    സ്ഥിതം ശൂന്യം
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(സന്ദേശം) (ശൂന്യം)semsg("%s: %s", സന്ദേശം, strerror_win32(errno))

    സ്ഥിതം അക്ഷരം *
strerror_win32(പൂർണ്ണസംഖ്യ eno)
{
    സ്ഥിതം LPVOID msgbuf = ശൂന്യ;
    char_u *ptr;

    എങ്കിൽ (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ശൂന്യ;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	ശൂന്യ,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ശൂന്യ);
    എങ്കിൽ (msgbuf != ശൂന്യ)
	// chomp \r or \n
	വേണ്ടി (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    തിരഞ്ഞെടുപ്പ് (*ptr)
	    {
		അവസ്ഥ '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    നിർത്തുക;
		അവസ്ഥ '\n':
		    എങ്കിൽ (*(ptr + 1) == '\0')
			*ptr = '\0';
		    അല്ലെങ്കിൽ
			*ptr = ' ';
		    നിർത്തുക;
	    }
    തിരികെ msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
സ്ഥിതം channel_T *first_channel = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(ശൂന്യം)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ ശൂന്യ;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    വേണ്ടി (part = PART_SOCK; part < PART_എണ്ണം; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_നിവേശം_Handler = (Xt_നിവേശം_Id)ശൂന്യ;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_നിവേശം_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    എങ്കിൽ (first_channel != ശൂന്യ)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    തിരികെ channel;
}

    പൂർണ്ണസംഖ്യ
has_any_channel(ശൂന്യം)
{
    തിരികെ first_channel != ശൂന്യ;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    പൂർണ്ണസംഖ്യ
channel_still_useful(channel_T *channel)
{
    പൂർണ്ണസംഖ്യ has_sock_സന്ദേശം;
    പൂർണ്ണസംഖ്യ	has_out_സന്ദേശം;
    പൂർണ്ണസംഖ്യ	has_err_സന്ദേശം;

    // If the job was killed the channel is not expected to work anymore.
    എങ്കിൽ (channel->ch_job_killed && channel->ch_job == ശൂന്യ)
	തിരികെ FALSE;

    // If there is a close callback it may still need to be invoked.
    എങ്കിൽ (channel->ch_close_cb.cb_name != ശൂന്യ)
	തിരികെ TRUE;

    // If reading from or a buffer it's still useful.
    എങ്കിൽ (channel->ch_part[PART_IN].ch_bufref.br_buf != ശൂന്യ)
	തിരികെ TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_സന്ദേശം = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != ശൂന്യ
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != ശൂന്യ;
    has_out_സന്ദേശം = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != ശൂന്യ
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != ശൂന്യ;
    has_err_സന്ദേശം = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != ശൂന്യ
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != ശൂന്യ;
    തിരികെ (channel->ch_callback.cb_name != ശൂന്യ && (has_sock_സന്ദേശം
		|| has_out_സന്ദേശം || has_err_സന്ദേശം))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != ശൂന്യ
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != ശൂന്യ)
		    && has_out_സന്ദേശം)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ശൂന്യ
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ശൂന്യ)
		    && has_err_സന്ദേശം);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    പൂർണ്ണസംഖ്യ
channel_can_close(channel_T *channel)
{
    തിരികെ channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    സ്ഥിതം ശൂന്യം
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    സ്ഥിതം ശൂന്യം
channel_free_channel(channel_T *channel)
{
    എങ്കിൽ (channel->ch_next != ശൂന്യ)
	channel->ch_next->ch_prev = channel->ch_prev;
    എങ്കിൽ (channel->ch_prev == ശൂന്യ)
	first_channel = channel->ch_next;
    അല്ലെങ്കിൽ
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    സ്ഥിതം ശൂന്യം
channel_free(channel_T *channel)
{
    എങ്കിൽ (in_free_unref_ഇനങ്ങൾ)
	തിരികെ;

    എങ്കിൽ (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_may_free(channel_T *channel)
{
    എങ്കിൽ (!channel_still_useful(channel))
    {
	channel_free(channel);
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    പൂർണ്ണസംഖ്യ
channel_unref(channel_T *channel)
{
    എങ്കിൽ (channel != ശൂന്യ && --channel->ch_refcount <= 0)
	തിരികെ channel_may_free(channel);
    തിരികെ FALSE;
}

    പൂർണ്ണസംഖ്യ
free_unused_channels_contents(പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ mask)
{
    പൂർണ്ണസംഖ്യ		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	എങ്കിൽ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    തിരികെ did_free;
}

    ശൂന്യം
free_unused_channels(പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    വേണ്ടി (ch = first_channel; ch != ശൂന്യ; ch = ch_next)
    {
	ch_next = ch->ch_next;
	എങ്കിൽ (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#എങ്കിൽ defined(FEAT_GUI)

# എങ്കിൽ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    സ്ഥിതം channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    എങ്കിൽ (fd == INVALID_FD)
	തിരികെ ശൂന്യ;

    FOR_ALL_CHANNELS(channel)
    {
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	    എങ്കിൽ (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		തിരികെ channel;
	    }
    }
    തിരികെ ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം
channel_read_fd(പൂർണ്ണസംഖ്യ fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    എങ്കിൽ (channel == ശൂന്യ)
	ch_error(ശൂന്യ, "Channel for fd %d not found", fd);
    അല്ലെങ്കിൽ
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    സ്ഥിതം ശൂന്യം
messageFromServerX11(XtPointer clientData,
		  പൂർണ്ണസംഖ്യ *unused1 UNUSED,
		  Xt_നിവേശം_Id *unused2 UNUSED)
{
    channel_read_fd((പൂർണ്ണസംഖ്യ)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    തിരികെ TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  അല്ലെങ്കിൽ
    സ്ഥിതം ശൂന്യം
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_നിവേശം_Condition unused2 UNUSED)
{
    channel_read_fd((പൂർണ്ണസംഖ്യ)(long)clientData);
}
#  endif
# endif

    സ്ഥിതം ശൂന്യം
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    എങ്കിൽ (!CH_HAS_GUI)
	തിരികെ;

    // gets stuck in handling events for a not connected channel
    എങ്കിൽ (channel->ch_keep_open)
	തിരികെ;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    എങ്കിൽ (channel->ch_part[part].ch_നിവേശം_Handler == (Xt_നിവേശം_Id)ശൂന്യ)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_നിവേശം_Handler = Xt_App_Add_നിവേശം(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_നിവേശം_Read_Mask + Xt_നിവേശം_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# അല്ലെങ്കിൽ
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    എങ്കിൽ (channel->ch_part[part].ch_നിവേശം_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_നിവേശം_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   അല്ലെങ്കിൽ
	channel->ch_part[part].ch_നിവേശം_Handler = gdk_നിവേശം_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_നിവേശം_Condition)
			     ((പൂർണ്ണസംഖ്യ)GDK_നിവേശം_READ + (പൂർണ്ണസംഖ്യ)GDK_നിവേശം_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    സ്ഥിതം ശൂന്യം
channel_gui_register(channel_T *channel)
{
    എങ്കിൽ (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    എങ്കിൽ (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    എങ്കിൽ (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    ശൂന്യം
channel_gui_register_all(ശൂന്യം)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    സ്ഥിതം ശൂന്യം
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    എങ്കിൽ (channel->ch_part[part].ch_നിവേശം_Handler != (Xt_നിവേശം_Id)ശൂന്യ)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_നിവേശം(channel->ch_part[part].ch_നിവേശം_Handler);
	channel->ch_part[part].ch_നിവേശം_Handler = (Xt_നിവേശം_Id)ശൂന്യ;
    }
# അല്ലെങ്കിൽ
#  ifdef FEAT_GUI_GTK
    എങ്കിൽ (channel->ch_part[part].ch_നിവേശം_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_നിവേശം_Handler);
#   അല്ലെങ്കിൽ
	gdk_നിവേശം_remove(channel->ch_part[part].ch_നിവേശം_Handler);
#   endif
	channel->ch_part[part].ch_നിവേശം_Handler = 0;
    }
#  endif
# endif
}

    സ്ഥിതം ശൂന്യം
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_connect(
	channel_T *channel,
	സ്ഥിരം ഘടന sockaddr *server_addr,
	പൂർണ്ണസംഖ്യ server_addrlen,
	പൂർണ്ണസംഖ്യ *waittime)
{
    പൂർണ്ണസംഖ്യ		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    ആകുമ്പോൾ (TRUE)
    {
	long	elapsed_msec = 0;
	പൂർണ്ണസംഖ്യ	waitnow;
	പൂർണ്ണസംഖ്യ	ret;

	എങ്കിൽ (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	എങ്കിൽ (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    തിരികെ -1;
	}

	എങ്കിൽ (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    എങ്കിൽ (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#അല്ലെങ്കിൽ
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		തിരികെ -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	എങ്കിൽ (ret == 0)
	    // The connection could be established.
	    നിർത്തുക;

	SOCK_ERRNO;
	എങ്കിൽ (*waittime < 0 || (errno != EWOULDBLOCK
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
	    തിരികെ -1;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    തിരികെ -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    ഘടന timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    പൂർണ്ണസംഖ്യ			so_error = 0;
	    socklen_t		so_error_len = വലുപ്പം(so_error);
	    ഘടന timeval	start_tv;
	    ഘടന timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, ശൂന്യ);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ശൂന്യ, &tv);
	    എങ്കിൽ (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		തിരികെ -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    എങ്കിൽ (FD_ISSET(sd, &wfds))
		നിർത്തുക;
	    elapsed_msec = waitnow;
	    എങ്കിൽ (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		തുടരുക;
	    }
#അല്ലെങ്കിൽ
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    എങ്കിൽ (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		എങ്കിൽ (ret < 0 || (so_error != 0
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
		    തിരികെ -1;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    തിരികെ -1;
		}
	    }

	    എങ്കിൽ (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		നിർത്തുക;

	    gettimeofday(&end_tv, ശൂന്യ);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	എങ്കിൽ (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    എങ്കിൽ (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    എങ്കിൽ (!got_int)
	    {
		എങ്കിൽ (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		തുടരുക;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	തിരികെ -1;
    }

    എങ്കിൽ (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#അല്ലെങ്കിൽ
	(ശൂന്യം)fcntl(sd, F_SETFL, 0);
#endif
    }

    തിരികെ sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	സ്ഥിരം അക്ഷരം *path,
	ശൂന്യം (*nb_close_cb)(ശൂന്യം))
{
    channel_T		*channel = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			sd = -1;
    size_t		path_len = STRLEN(path);
    ഘടന sockaddr_un	server;
    size_t		server_len;
    പൂർണ്ണസംഖ്യ			waittime = -1;

    എങ്കിൽ (*path == NUL || path_len >= വലുപ്പം(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	തിരികെ ശൂന്യ;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
    {
	ch_error(ശൂന്യ, "Cannot allocate channel.");
	തിരികെ ശൂന്യ;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, വലുപ്പം(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(ഘടന sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (ഘടന sockaddr *)&server, (പൂർണ്ണസംഖ്യ)server_len,
								   &waittime);

    എങ്കിൽ (sd < 0)
    {
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (അക്ഷരം *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    തിരികെ channel;
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
	സ്ഥിരം അക്ഷരം *hostname,
	പൂർണ്ണസംഖ്യ port,
	പൂർണ്ണസംഖ്യ waittime,
	ശൂന്യം (*nb_close_cb)(ശൂന്യം))
{
    പൂർണ്ണസംഖ്യ			sd = -1;
    channel_T		*channel = ശൂന്യ;
#ifdef FEAT_IPV6
    പൂർണ്ണസംഖ്യ			err;
    ഘടന addrinfo	hints;
    ഘടന addrinfo	*res = ശൂന്യ;
    ഘടന addrinfo	*addr = ശൂന്യ;
#അല്ലെങ്കിൽ
    ഘടന sockaddr_in	server;
    ഘടന hostent	*host = ശൂന്യ;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
    {
	ch_error(ശൂന്യ, "Cannot allocate channel.");
	തിരികെ ശൂന്യ;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# എങ്കിൽ defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    എങ്കിൽ ((err = getaddrinfo(hostname, ശൂന്യ, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    // Count the number of addresses for timeout distribution
    പൂർണ്ണസംഖ്യ addr_എണ്ണം = 0;
    വേണ്ടി (addr = res; addr != ശൂന്യ; addr = addr->ai_next)
	addr_എണ്ണം++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    എങ്കിൽ (waittime == 0)
	waittime = 15;

    പൂർണ്ണസംഖ്യ addr_index = 0;
    വേണ്ടി (addr = res; addr != ശൂന്യ; addr = addr->ai_next, addr_index++)
    {
	സ്ഥിരം അക്ഷരം  *dst = hostname;
# ifdef HAVE_INET_NTOP
	സ്ഥിരം ശൂന്യം  *src = ശൂന്യ;
	അക്ഷരം	    buf[NUMBUFLEN];
# endif
	പൂർണ്ണസംഖ്യ	    try_waittime;
	പൂർണ്ണസംഖ്യ	    before_waittime;
	പൂർണ്ണസംഖ്യ	    consumed;
	പൂർണ്ണസംഖ്യ	    remaining_addrs;

	എങ്കിൽ (addr->ai_family == AF_INET6)
	{
	    ഘടന sockaddr_in6 *sai = (ഘടന sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (addr->ai_family == AF_INET)
	{
	    ഘടന sockaddr_in *sai = (ഘടന sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	എങ്കിൽ (src != ശൂന്യ)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, വലുപ്പം(buf));
	    എങ്കിൽ (dst == ശൂന്യ)
		dst = hostname;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	എങ്കിൽ (addr->ai_next == ശൂന്യ)
	    try_waittime = waittime;
	അല്ലെങ്കിൽ എങ്കിൽ (addr_index == 0)
	{
	    എങ്കിൽ (waittime > 500)
		try_waittime = 250;
	    അല്ലെങ്കിൽ എങ്കിൽ (waittime > 30)
		try_waittime = waittime / 2;
	    അല്ലെങ്കിൽ
		try_waittime = waittime;
	}
	അല്ലെങ്കിൽ
	{
	    remaining_addrs = addr_എണ്ണം - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (പൂർണ്ണസംഖ്യ)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	എങ്കിൽ (waittime < 0)
	    waittime = 0;

	എങ്കിൽ (sd >= 0)
	    നിർത്തുക;

	// If we have no time left, stop trying
	എങ്കിൽ (waittime <= 0 && addr->ai_next != ശൂന്യ)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    നിർത്തുക;
	}
    }

    freeaddrinfo(res);
#അല്ലെങ്കിൽ
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    എങ്കിൽ ((host = gethostbyname(hostname)) == ശൂന്യ)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }
    {
	അക്ഷരം *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], വലുപ്പം(p));
	memcpy((അക്ഷരം *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    എങ്കിൽ (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (ഘടന sockaddr *)&server, വലുപ്പം(server),
								   &waittime);
#endif

    എങ്കിൽ (sd < 0)
    {
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (അക്ഷരം *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    തിരികെ channel;
}

    സ്ഥിതം ശൂന്യം
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    എങ്കിൽ (callback->cb_name != ശൂന്യ && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    അല്ലെങ്കിൽ
	cbp->cb_name = ശൂന്യ;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	സ്ഥിതം ശൂന്യം
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_മൂല്യം_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_മൂല്യം_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    എങ്കിൽ (curbuf->b_ml.ml_mfp == ശൂന്യ)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    സ്ഥിതം buf_T *
channel_find_buffer(char_u *name, പൂർണ്ണസംഖ്യ err, പൂർണ്ണസംഖ്യ സന്ദേശം)
{
    buf_T *buf = ശൂന്യ;
    buf_T *save_curbuf = curbuf;

    എങ്കിൽ (name != ശൂന്യ && *name != NUL)
    {
	buf = buflist_findname(name);
	എങ്കിൽ (buf == ശൂന്യ)
	    buf = buflist_findname_exp(name);
    }

    എങ്കിൽ (buf != ശൂന്യ)
	തിരികെ buf;

    buf = buflist_new(name == ശൂന്യ || *name == NUL ? ശൂന്യ : name,
	    ശൂന്യ, (linenr_T)0, BLN_LISTED | BLN_NEW);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ ശൂന്യ;
    prepare_buffer(buf);

    curbuf = buf;
    എങ്കിൽ (സന്ദേശം)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    തിരികെ buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    സ്ഥിതം ശൂന്യം
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    എങ്കിൽ (opt->jo_set & JO_MODE)
	വേണ്ടി (part = PART_SOCK; part < PART_എണ്ണം; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    എങ്കിൽ (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    എങ്കിൽ (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    എങ്കിൽ (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    എങ്കിൽ (opt->jo_set & JO_TIMEOUT)
	വേണ്ടി (part = PART_SOCK; part < PART_എണ്ണം; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    എങ്കിൽ (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    എങ്കിൽ (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    എങ്കിൽ (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    എങ്കിൽ (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    എങ്കിൽ (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    എങ്കിൽ (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    എങ്കിൽ (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    എങ്കിൽ ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	എങ്കിൽ (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	എങ്കിൽ (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    എങ്കിൽ (buf == ശൂന്യ)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ സന്ദേശം = TRUE;

	    എങ്കിൽ (opt->jo_set2 & JO2_OUT_സന്ദേശം)
		സന്ദേശം = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, സന്ദേശം);
	}
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    എങ്കിൽ (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    എങ്കിൽ (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    അല്ലെങ്കിൽ
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (അക്ഷരം *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
		    prepare_buffer(buf);
	    }
	}
    }

    എങ്കിൽ ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	എങ്കിൽ (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	എങ്കിൽ (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	അല്ലെങ്കിൽ എങ്കിൽ (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    എങ്കിൽ (buf == ശൂന്യ)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ സന്ദേശം = TRUE;

	    എങ്കിൽ (opt->jo_set2 & JO2_ERR_സന്ദേശം)
		സന്ദേശം = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, സന്ദേശം);
	}
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    എങ്കിൽ (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    എങ്കിൽ (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    അല്ലെങ്കിൽ
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (അക്ഷരം *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		എങ്കിൽ (buf->b_ml.ml_mfp == ശൂന്യ)
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
    പൂർണ്ണസംഖ്യ
channel_parse_socketserver_address(
	char_u	*address,
	പൂർണ്ണസംഖ്യ	*port,
	char_u	**unix_path,
	bool	quiet)
{
    അക്ഷരം    *rest;
    long    val;

    എങ്കിൽ (*address == NUL)
	പോകുക fail;

    എങ്കിൽ (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	തിരികെ OK;
    }

    val = strtol((അക്ഷരം *)(address), &rest, 10);
    എങ്കിൽ (val < 0 || val >= 65536 || *rest != NUL)
    {
	എങ്കിൽ (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	തിരികെ FAIL;
    }
    *port = (പൂർണ്ണസംഖ്യ)val;

    തിരികെ OK;
fail:
    എങ്കിൽ (!quiet)
	semsg(_(e_invalid_argument_str), address);
    തിരികെ FAIL;
}

/*
 * Implements ch_open().
 */
    സ്ഥിതം channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    അക്ഷരം	*rest;
    പൂർണ്ണസംഖ്യ		port = 0;
    പൂർണ്ണസംഖ്യ		is_ipv6 = FALSE;
    പൂർണ്ണസംഖ്യ		is_unix = FALSE;
    പൂർണ്ണസംഖ്യ		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ ശൂന്യ;

    address = tv_get_string(&argvars[0]);
    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	എങ്കിൽ (p == ശൂന്യ || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    തിരികെ ശൂന്യ;
	}
    }
    അല്ലെങ്കിൽ
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	എങ്കിൽ (p == ശൂന്യ)
	{
	    semsg(_(e_invalid_argument_str), address);
	    തിരികെ ശൂന്യ;
	}
    }

    എങ്കിൽ (!is_unix && !is_stdio)
    {
	port = strtol((അക്ഷരം *)(p + 1), &rest, 10);
	എങ്കിൽ (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	അല്ലെങ്കിൽ
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    എങ്കിൽ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	പോകുക theend;
    എങ്കിൽ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	പോകുക theend;
    }

    എങ്കിൽ (is_stdio)
	channel = channel_open_stdio();
    അല്ലെങ്കിൽ എങ്കിൽ (is_unix)
	channel = channel_open_unix((അക്ഷരം *)address, ശൂന്യ);
    അല്ലെങ്കിൽ
	channel = channel_open((അക്ഷരം *)address, port, opt.jo_waittime, ശൂന്യ);
    എങ്കിൽ (channel != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	എങ്കിൽ (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    തിരികെ channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
സ്ഥിതം channel_T *stdio_channel = ശൂന്യ;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    സ്ഥിതം channel_T *
channel_open_stdio(ശൂന്യം)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    എങ്കിൽ (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (stdio_channel != ശൂന്യ)
    {
	emsg(_(e_cannot_open_stdio_channel));
	തിരികെ ശൂന്യ;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	പൂർണ്ണസംഖ്യ	nul_fd;

	എങ്കിൽ (!DuplicateHandle(proc, GetStdHandle(STD_നിവേശം_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    തിരികെ ശൂന്യ;
	}
	എങ്കിൽ (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    തിരികെ ശൂന്യ;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ശൂന്യ, OPEN_EXISTING, 0,
		ശൂന്യ);
	എങ്കിൽ (hnul != INVALID_HANDLE_മൂല്യം)
	{
	    SetStdHandle(STD_നിവേശം_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	എങ്കിൽ (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	എങ്കിൽ (in_fd < 0 || out_fd < 0)
	{
	    എങ്കിൽ (in_fd >= 0)
		close(in_fd);
	    എങ്കിൽ (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    തിരികെ ശൂന്യ;
	}
	(ശൂന്യം)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(ശൂന്യം)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	എങ്കിൽ (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
    {
	ch_error(ശൂന്യ, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	തിരികെ ശൂന്യ;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    തിരികെ channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    അക്ഷരം	*rest;
    പൂർണ്ണസംഖ്യ		port;
    പൂർണ്ണസംഖ്യ		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	തിരികെ ശൂന്യ;

    arg = tv_get_string(&argvars[0]);
    എങ്കിൽ (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    അല്ലെങ്കിൽ
    {
	port = strtol((അക്ഷരം *)(arg), &rest, 10);
	എങ്കിൽ (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    തിരികെ ശൂന്യ;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    എങ്കിൽ (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	പോകുക theend;
    എങ്കിൽ (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	പോകുക theend;
    }

    എങ്കിൽ (is_unix)
	channel = channel_listen_unix((അക്ഷരം *)arg, ശൂന്യ, true);
    അല്ലെങ്കിൽ
	channel = channel_listen(port, ശൂന്യ);
    എങ്കിൽ (channel != ശൂന്യ)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    തിരികെ channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	പൂർണ്ണസംഖ്യ port_in,
	ശൂന്യം (*nb_close_cb)(ശൂന്യം))
{
    പൂർണ്ണസംഖ്യ			sd = -1;
    ഘടന sockaddr_in	server;
#ifndef FEAT_IPV6
    ഘടന hostent	*host;
#endif
    പൂർണ്ണസംഖ്യ			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
    {
	ch_error(ശൂന്യ, "Cannot allocate channel.");
	തിരികെ ശൂന്യ;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((അക്ഷരം *)&server, 0, വലുപ്പം(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    എങ്കിൽ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

#ifdef MSWIN
    എങ്കിൽ (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (സ്ഥിരം അക്ഷരം *)&val, വലുപ്പം(val)) < 0)
#അല്ലെങ്കിൽ
    എങ്കിൽ (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, വലുപ്പം(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    // Bind the socket to the port
    എങ്കിൽ (bind(sd, (ഘടന sockaddr *)&server, വലുപ്പം(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    എങ്കിൽ (port_in == 0)
    {
	ഘടന sockaddr_in	addr;
	socklen_t		addr_len = വലുപ്പം(addr);

	എങ്കിൽ (getsockname(sd, (ഘടന sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (അക്ഷരം *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    തിരികെ channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	അക്ഷരം *path,
	ശൂന്യം (*nb_close_cb)(ശൂന്യം),
	bool replace)
{
    പൂർണ്ണസംഖ്യ			sd = -1;
    ഘടന sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    എങ്കിൽ (path == ശൂന്യ || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == ശൂന്യ ? (അക്ഷരം *)"" : path);
	തിരികെ ശൂന്യ;
    }

    path_len = STRLEN(path);
    എങ്കിൽ (path_len >= വലുപ്പം(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	തിരികെ ശൂന്യ;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
    {
	ch_error(ശൂന്യ, "Cannot allocate channel.");
	തിരികെ ശൂന്യ;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, വലുപ്പം(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    എങ്കിൽ (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(ഘടന sockaddr_un, sun_path) + path_len + 1;
    എങ്കിൽ (bind(sd, (ഘടന sockaddr *)&server, (പൂർണ്ണസംഖ്യ)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	തിരികെ ശൂന്യ;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (അക്ഷരം *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    തിരികെ channel;
}

     ശൂന്യം
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    എങ്കിൽ (*fd == INVALID_FD)
	തിരികെ;

    എങ്കിൽ (part == PART_SOCK)
	sock_close(*fd);
    അല്ലെങ്കിൽ
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	എങ്കിൽ ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    എങ്കിൽ (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    ശൂന്യം
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    എങ്കിൽ (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#എങ്കിൽ defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	എങ്കിൽ (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    എങ്കിൽ (out != INVALID_FD)
    {
#എങ്കിൽ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#എങ്കിൽ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    എങ്കിൽ (err != INVALID_FD)
    {
#എങ്കിൽ defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#എങ്കിൽ defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    ശൂന്യം
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    എങ്കിൽ (job->jv_in_buf == ശൂന്യ)
	തിരികെ;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (അക്ഷരം *)in_part->ch_bufref.br_buf->b_ffname);
    എങ്കിൽ (options->jo_set & JO_IN_മുകൾ)
    {
	എങ്കിൽ (options->jo_in_മുകൾ == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_മുകൾ =
		in_part->ch_bufref.br_buf->b_ml.ml_വരി_എണ്ണം + 1;
	}
	അല്ലെങ്കിൽ
	    in_part->ch_buf_മുകൾ = options->jo_in_മുകൾ;
    }
    അല്ലെങ്കിൽ
	in_part->ch_buf_മുകൾ = 1;
    എങ്കിൽ (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    അല്ലെങ്കിൽ
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_വരി_എണ്ണം;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    സ്ഥിതം ശൂന്യം
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	പൂർണ്ണസംഖ്യ	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    എങ്കിൽ (item == ശൂന്യ)
	തിരികെ;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = ശൂന്യ;
    എങ്കിൽ (item->cq_prev == ശൂന്യ)
	head->cq_next = item;
    അല്ലെങ്കിൽ
	item->cq_prev->cq_next = item;
}

    സ്ഥിതം ശൂന്യം
write_buf_വരി(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *വരി = ml_get_buf(buf, lnum, FALSE);
    പൂർണ്ണസംഖ്യ	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    i;

    // Need to make a copy to be able to append a NL.
    എങ്കിൽ ((p = alloc(len + 2)) == ശൂന്യ)
	തിരികെ;
    memcpy((അക്ഷരം *)p, (അക്ഷരം *)വരി, len);

    എങ്കിൽ (channel->ch_write_text_mode)
	p[len] = CAR;
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (i = 0; i < len; ++i)
	    എങ്കിൽ (p[i] == NL)
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
can_write_buf_വരി(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    എങ്കിൽ (in_part->ch_fd == INVALID_FD)
	തിരികെ FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    എങ്കിൽ (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    അല്ലെങ്കിൽ എങ്കിൽ (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# എങ്കിൽ defined(HAVE_SELECT)
	ഘടന timeval	tval;
	fd_set		wfds;
	പൂർണ്ണസംഖ്യ		ret;

	FD_ZERO(&wfds);
	FD_SET((പൂർണ്ണസംഖ്യ)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	വേണ്ടി (;;)
	{
	    ret = select((പൂർണ്ണസംഖ്യ)in_part->ch_fd + 1, ശൂന്യ, &wfds, ശൂന്യ, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    എങ്കിൽ (ret == -1 && errno == EINTR)
		തുടരുക;
#  endif
	    എങ്കിൽ (ret <= 0 || in_part->ch_block_write == 1)
	    {
		എങ്കിൽ (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		അല്ലെങ്കിൽ
		    ch_log(channel, "Input not ready for writing");
		തിരികെ FALSE;
	    }
	    നിർത്തുക;
	}
# അല്ലെങ്കിൽ
	ഘടന pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	എങ്കിൽ (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    തിരികെ FALSE;
	}
	എങ്കിൽ (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    തിരികെ FALSE;
	}
# endif
    }
#endif
    തിരികെ TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    ശൂന്യം
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    പൂർണ്ണസംഖ്യ		written = 0;

    എങ്കിൽ (buf == ശൂന്യ || in_part->ch_buf_append)
	തിരികെ;  // no buffer or using appending
    എങ്കിൽ (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == ശൂന്യ)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = ശൂന്യ;
	തിരികെ;
    }

    വേണ്ടി (lnum = in_part->ch_buf_മുകൾ; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_വരി_എണ്ണം; ++lnum)
    {
	എങ്കിൽ (!can_write_buf_വരി(channel))
	    നിർത്തുക;
	write_buf_വരി(buf, lnum, channel);
	++written;
    }

    എങ്കിൽ (written == 1)
	ch_log(channel, "written line %d to channel", (പൂർണ്ണസംഖ്യ)lnum - 1);
    അല്ലെങ്കിൽ എങ്കിൽ (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_മുകൾ = lnum;
    എങ്കിൽ (lnum > buf->b_ml.ml_വരി_എണ്ണം || lnum > in_part->ch_buf_bot)
    {
#എങ്കിൽ defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	എങ്കിൽ (channel->ch_job != ശൂന്യ)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = ശൂന്യ;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	എങ്കിൽ (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    അല്ലെങ്കിൽ
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_വരി_എണ്ണം - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    ശൂന്യം
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	വേണ്ടി (part = PART_SOCK; part < PART_എണ്ണം; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    എങ്കിൽ (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = ശൂന്യ;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    ശൂന്യം
channel_write_നിവേശം(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    എങ്കിൽ (in_part->ch_writeque.wq_next != ശൂന്യ)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    അല്ലെങ്കിൽ എങ്കിൽ (in_part->ch_bufref.br_buf != ശൂന്യ)
    {
	എങ്കിൽ (in_part->ch_buf_append)
	    channel_write_new_വരികൾ(in_part->ch_bufref.br_buf);
	അല്ലെങ്കിൽ
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    ശൂന്യം
channel_write_any_വരികൾ(ശൂന്യം)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_നിവേശം(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    ശൂന്യം
channel_write_new_വരികൾ(buf_T *buf)
{
    channel_T	*channel;
    പൂർണ്ണസംഖ്യ		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	പൂർണ്ണസംഖ്യ	    written = 0;

	എങ്കിൽ (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    എങ്കിൽ (in_part->ch_fd == INVALID_FD)
		തുടരുക;  // pipe was closed
	    found_one = TRUE;
	    വേണ്ടി (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_വരി_എണ്ണം;
								       ++lnum)
	    {
		എങ്കിൽ (!can_write_buf_വരി(channel))
		    നിർത്തുക;
		write_buf_വരി(buf, lnum, channel);
		++written;
	    }

	    എങ്കിൽ (written == 1)
		ch_log(channel, "written line %d to channel", (പൂർണ്ണസംഖ്യ)lnum - 1);
	    അല്ലെങ്കിൽ എങ്കിൽ (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    എങ്കിൽ (lnum < buf->b_ml.ml_വരി_എണ്ണം)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_വരി_എണ്ണം - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    എങ്കിൽ (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    സ്ഥിതം ശൂന്യം
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    എങ്കിൽ (safe_to_invoke_callback == 0)
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

    തിരികെ head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *കണ്ണി)
{
    char_u  *buffer = കണ്ണി->rq_buffer;
    long_u  i;

    വേണ്ടി (i = 0; i < കണ്ണി->rq_buflen; ++i)
	എങ്കിൽ (buffer[i] == NL)
	    തിരികെ buffer + i;
    തിരികെ ശൂന്യ;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, പൂർണ്ണസംഖ്യ *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *കണ്ണി = head->rq_next;
    char_u *p;

    എങ്കിൽ (കണ്ണി == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (outlen != ശൂന്യ)
	*outlen += കണ്ണി->rq_buflen;
    // dispose of the node but keep the buffer
    p = കണ്ണി->rq_buffer;
    head->rq_next = കണ്ണി->rq_next;
    എങ്കിൽ (കണ്ണി->rq_next == ശൂന്യ)
	head->rq_prev = ശൂന്യ;
    അല്ലെങ്കിൽ
	കണ്ണി->rq_next->rq_prev = ശൂന്യ;
    vim_free(കണ്ണി);
    തിരികെ p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    സ്ഥിതം char_u *
channel_get_all(channel_T *channel, ch_part_T part, പൂർണ്ണസംഖ്യ *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *കണ്ണി;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    വേണ്ടി (കണ്ണി = head->rq_next; കണ്ണി != ശൂന്യ; കണ്ണി = കണ്ണി->rq_next)
	len += കണ്ണി->rq_buflen;
    res = alloc(len + 1);
    എങ്കിൽ (res == ശൂന്യ)
	തിരികെ ശൂന്യ;
    p = res;
    വേണ്ടി (കണ്ണി = head->rq_next; കണ്ണി != ശൂന്യ; കണ്ണി = കണ്ണി->rq_next)
    {
	mch_memmove(p, കണ്ണി->rq_buffer, കണ്ണി->rq_buflen);
	p += കണ്ണി->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    ചെയ്യുക
    {
	p = channel_get(channel, part, ശൂന്യ);
	vim_free(p);
    } ആകുമ്പോൾ (p != ശൂന്യ);

    എങ്കിൽ (outlen != ശൂന്യ)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	തിരികെ res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ആകുമ്പോൾ (p < res + len)
    {
	എങ്കിൽ (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	അല്ലെങ്കിൽ എങ്കിൽ (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    എങ്കിൽ (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ആകുമ്പോൾ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		എങ്കിൽ (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    തിരികെ res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    ശൂന്യം
channel_consume(channel_T *channel, ch_part_T part, പൂർണ്ണസംഖ്യ len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *കണ്ണി = head->rq_next;
    char_u *buf = കണ്ണി->rq_buffer;

    എങ്കിൽ (len < 0 || (long_u)len > കണ്ണി->rq_buflen)
	തിരികെ;
    mch_memmove(buf, buf + len, കണ്ണി->rq_buflen - len);
    കണ്ണി->rq_buflen -= len;
    കണ്ണി->rq_buffer[കണ്ണി->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    പൂർണ്ണസംഖ്യ
channel_collapse(channel_T *channel, ch_part_T part, പൂർണ്ണസംഖ്യ want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*കണ്ണി = head->rq_next;
    readq_T	*last_കണ്ണി;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    എങ്കിൽ (കണ്ണി == ശൂന്യ || കണ്ണി->rq_next == ശൂന്യ)
	തിരികെ FAIL;

    last_കണ്ണി = കണ്ണി->rq_next;
    len = കണ്ണി->rq_buflen + last_കണ്ണി->rq_buflen;
    എങ്കിൽ (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	ആകുമ്പോൾ (last_കണ്ണി->rq_next != ശൂന്യ
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_കണ്ണി) == ശൂന്യ))
	{
	    last_കണ്ണി = last_കണ്ണി->rq_next;
	    len += last_കണ്ണി->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    എങ്കിൽ (newbuf == ശൂന്യ)
	തിരികെ FAIL;	    // out of memory
    mch_memmove(p, കണ്ണി->rq_buffer, കണ്ണി->rq_buflen);
    p += കണ്ണി->rq_buflen;
    vim_free(കണ്ണി->rq_buffer);
    കണ്ണി->rq_buffer = newbuf;
    വേണ്ടി (n = കണ്ണി; n != last_കണ്ണി; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    കണ്ണി->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    വേണ്ടി (n = കണ്ണി->rq_next; n != last_കണ്ണി; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    കണ്ണി->rq_next = last_കണ്ണി->rq_next;
    എങ്കിൽ (last_കണ്ണി->rq_next == ശൂന്യ)
	head->rq_prev = കണ്ണി;
    അല്ലെങ്കിൽ
	last_കണ്ണി->rq_next->rq_prev = കണ്ണി;
    vim_free(last_കണ്ണി);
    തിരികെ OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_save(channel_T *channel, ch_part_T part, char_u *buf, പൂർണ്ണസംഖ്യ len,
						      പൂർണ്ണസംഖ്യ prepend, അക്ഷരം *lead)
{
    readq_T *കണ്ണി;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    i;

    കണ്ണി = ALLOC_ONE(readq_T);
    എങ്കിൽ (കണ്ണി == ശൂന്യ)
	തിരികെ FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    കണ്ണി->rq_buffer = alloc(len + 1);
    എങ്കിൽ (കണ്ണി->rq_buffer == ശൂന്യ)
    {
	vim_free(കണ്ണി);
	തിരികെ FAIL;	    // out of memory
    }

    എങ്കിൽ (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = കണ്ണി->rq_buffer;
	വേണ്ടി (i = 0; i < len; ++i)
	    എങ്കിൽ (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	കണ്ണി->rq_buflen = (long_u)(p - കണ്ണി->rq_buffer);
    }
    അല്ലെങ്കിൽ
    {
	mch_memmove(കണ്ണി->rq_buffer, buf, len);
	കണ്ണി->rq_buffer[len] = NUL;
	കണ്ണി->rq_buflen = (long_u)len;
    }

    എങ്കിൽ (prepend)
    {
	// prepend node to the head of the queue
	കണ്ണി->rq_next = head->rq_next;
	കണ്ണി->rq_prev = ശൂന്യ;
	എങ്കിൽ (head->rq_next == ശൂന്യ)
	    head->rq_prev = കണ്ണി;
	അല്ലെങ്കിൽ
	    head->rq_next->rq_prev = കണ്ണി;
	head->rq_next = കണ്ണി;
    }
    അല്ലെങ്കിൽ
    {
	// append node to the tail of the queue
	കണ്ണി->rq_next = ശൂന്യ;
	കണ്ണി->rq_prev = head->rq_prev;
	എങ്കിൽ (head->rq_prev == ശൂന്യ)
	    head->rq_next = കണ്ണി;
	അല്ലെങ്കിൽ
	    head->rq_prev->rq_next = കണ്ണി;
	head->rq_prev = കണ്ണി;
    }

    എങ്കിൽ (ch_log_active() && lead != ശൂന്യ)
	ch_log_literal(lead, channel, part, buf, len);

    തിരികെ OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    പൂർണ്ണസംഖ്യ
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, ശൂന്യ);
    പൂർണ്ണസംഖ്യ		keeplen;
    പൂർണ്ണസംഖ്യ		addlen;
    char_u	*p;

    എങ്കിൽ (next == ശൂന്യ)
	തിരികെ FALSE;

    keeplen = reader->js_end - reader->js_buf;
    എങ്കിൽ (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (പൂർണ്ണസംഖ്യ)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    vim_free(next);
	    തിരികെ FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    തിരികെ TRUE;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*വരി_start;
    char_u	*p;
    int_u	hdr_len;
    പൂർണ്ണസംഖ്യ		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ആകുമ്പോൾ (TRUE)
    {
	വരി_start = p;
	ആകുമ്പോൾ (*p != NUL && *p != '\n')
	    p++;
	എങ്കിൽ (*p == NUL)			// partial header
	    തിരികെ MAYBE;
	p++;

	// process the content length field (if present)
	എങ്കിൽ ((p - വരി_start > 16)
		&& STRNICMP(വരി_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((അക്ഷരം *)വരി_start + 16, ശൂന്യ, 10);
	    എങ്കിൽ (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		തിരികെ FAIL;
	}

	എങ്കിൽ ((p - വരി_start) == 2 && വരി_start[0] == '\r' &&
		വരി_start[1] == '\n')
	    // reached the empty line
	    നിർത്തുക;
    }

    എങ്കിൽ (payload_len == -1)
	// Content-Length field is not present in the header
	തിരികെ FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    എങ്കിൽ (jsbuf_len < hdr_len + payload_len)
	തിരികെ MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    തിരികെ OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    പൂർണ്ണസംഖ്യ
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    പൂർണ്ണസംഖ്യ		status = OK;
    പൂർണ്ണസംഖ്യ		ret;

    എങ്കിൽ (channel_peek(channel, part) == ശൂന്യ)
	തിരികെ FALSE;

    reader.js_buf = channel_get(channel, part, ശൂന്യ);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    എങ്കിൽ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    എങ്കിൽ (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    എങ്കിൽ (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	എങ്കിൽ (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    എങ്കിൽ (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    അല്ലെങ്കിൽ
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	അല്ലെങ്കിൽ
	{
	    item = ALLOC_ONE(jsonq_T);
	    എങ്കിൽ (item == ശൂന്യ)
		clear_tv(&listtv);
	    അല്ലെങ്കിൽ
	    {
		item->jq_no_callback = FALSE;
		item->jq_മൂല്യം = alloc_tv();
		എങ്കിൽ (item->jq_മൂല്യം == ശൂന്യ)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		അല്ലെങ്കിൽ
		{
		    *item->jq_മൂല്യം = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = ശൂന്യ;
		    എങ്കിൽ (item->jq_prev == ശൂന്യ)
			head->jq_next = item;
		    അല്ലെങ്കിൽ
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    എങ്കിൽ (status == OK)
	chanpart->ch_wait_len = 0;
    അല്ലെങ്കിൽ എങ്കിൽ (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	എങ്കിൽ (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (പൂർണ്ണസംഖ്യ)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_എണ്ണം() + 100L;
#അല്ലെങ്കിൽ
	    gettimeofday(&chanpart->ch_deadline, ശൂന്യ);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    എങ്കിൽ (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ timeout;
#ifdef MSWIN
	    timeout = (പൂർണ്ണസംഖ്യ)(Get_Tick_എണ്ണം() - chanpart->ch_deadline) > 0;
#അല്ലെങ്കിൽ
	    {
		ഘടന timeval now_tv;

		gettimeofday(&now_tv, ശൂന്യ);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    എങ്കിൽ (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    അല്ലെങ്കിൽ
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    എങ്കിൽ (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(പൂർണ്ണസംഖ്യ)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, ശൂന്യ);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    അല്ലെങ്കിൽ
	ret = FALSE;

    vim_free(reader.js_buf);
    തിരികെ ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    സ്ഥിതം ശൂന്യം
remove_cb_കണ്ണി(cbq_T *head, cbq_T *കണ്ണി)
{
    എങ്കിൽ (കണ്ണി->cq_prev == ശൂന്യ)
	head->cq_next = കണ്ണി->cq_next;
    അല്ലെങ്കിൽ
	കണ്ണി->cq_prev->cq_next = കണ്ണി->cq_next;
    എങ്കിൽ (കണ്ണി->cq_next == ശൂന്യ)
	head->cq_prev = കണ്ണി->cq_prev;
    അല്ലെങ്കിൽ
	കണ്ണി->cq_next->cq_prev = കണ്ണി->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    ശൂന്യം
remove_json_കണ്ണി(jsonq_T *head, jsonq_T *കണ്ണി)
{
    എങ്കിൽ (കണ്ണി->jq_prev == ശൂന്യ)
	head->jq_next = കണ്ണി->jq_next;
    അല്ലെങ്കിൽ
	കണ്ണി->jq_prev->jq_next = കണ്ണി->jq_next;
    എങ്കിൽ (കണ്ണി->jq_next == ശൂന്യ)
	head->jq_prev = കണ്ണി->jq_prev;
    അല്ലെങ്കിൽ
	കണ്ണി->jq_next->jq_prev = കണ്ണി->jq_prev;
    vim_free(കണ്ണി);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    സ്ഥിതം ശൂന്യം
channel_add_block_id(chanpart_T *chanpart, പൂർണ്ണസംഖ്യ id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    എങ്കിൽ (gap->ga_growsize == 0)
	ga_init2(gap, വലുപ്പം(പൂർണ്ണസംഖ്യ), 10);
    എങ്കിൽ (ga_grow(gap, 1) == OK)
    {
	((പൂർണ്ണസംഖ്യ *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    സ്ഥിതം ശൂന്യം
channel_remove_block_id(chanpart_T *chanpart, പൂർണ്ണസംഖ്യ id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	എങ്കിൽ (((പൂർണ്ണസംഖ്യ *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    എങ്കിൽ (i < gap->ga_len)
	    {
		പൂർണ്ണസംഖ്യ *p = ((പൂർണ്ണസംഖ്യ *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * വലുപ്പം(പൂർണ്ണസംഖ്യ));
	    }
	    തിരികെ;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_has_block_id(chanpart_T *chanpart, പൂർണ്ണസംഖ്യ id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    പൂർണ്ണസംഖ്യ		i;

    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	എങ്കിൽ (((പൂർണ്ണസംഖ്യ *)gap->ga_data)[i] == id)
	    തിരികെ TRUE;
    തിരികെ FALSE;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	പൂർണ്ണസംഖ്യ	    id,
	പൂർണ്ണസംഖ്യ	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    ആകുമ്പോൾ (item != ശൂന്യ)
    {
	list_T	    *l;
	typval_T    *tv;

	എങ്കിൽ (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_മൂല്യം->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	അല്ലെങ്കിൽ
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    എങ്കിൽ (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		എങ്കിൽ (id > 0)
		{
		    എങ്കിൽ (item->jq_മൂല്യം->v_type != VAR_DICT)
			പോകുക nextitem;
		    d = item->jq_മൂല്യം->vval.v_dict;
		    എങ്കിൽ (d == ശൂന്യ)
			പോകുക nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    എങ്കിൽ (dict_has_താക്കോൽ(d, "method"))
			പോകുക nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    എങ്കിൽ (di == ശൂന്യ)
			പോകുക nextitem;
		    tv = &di->di_tv;
		}
		അല്ലെങ്കിൽ
		    tv = item->jq_മൂല്യം;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (id > 0)
		{
		    എങ്കിൽ (item->jq_മൂല്യം->v_type != VAR_DICT)
			പോകുക nextitem;
		    d = item->jq_മൂല്യം->vval.v_dict;
		    എങ്കിൽ (d == ശൂന്യ)
			പോകുക nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    എങ്കിൽ (di == ശൂന്യ)
			പോകുക nextitem;
		    tv = &di->di_tv;
		}
		അല്ലെങ്കിൽ
		    tv = item->jq_മൂല്യം;
	    }
	}

	എങ്കിൽ ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_സംഖ്യ && tv->vval.v_സംഖ്യ == id)
	      || (id <= 0 && (tv->v_type != VAR_സംഖ്യ
		 || tv->vval.v_സംഖ്യ == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_സംഖ്യ)))))
	{
	    *rettv = item->jq_മൂല്യം;
	    എങ്കിൽ (tv->v_type == VAR_സംഖ്യ)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_സംഖ്യ);
	    remove_json_കണ്ണി(head, item);
	    തിരികെ OK;
	}
nextitem:
	item = item->jq_next;
    }
    തിരികെ FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    സ്ഥിതം ശൂന്യം
channel_തള്ളുക_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    എങ്കിൽ (head->jq_prev != ശൂന്യ && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = ശൂന്യ;
    അല്ലെങ്കിൽ ആകുമ്പോൾ (item != ശൂന്യ && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    എങ്കിൽ (newitem == ശൂന്യ)
    {
	clear_tv(rettv);
	തിരികെ;
    }

    newitem->jq_മൂല്യം = alloc_tv();
    എങ്കിൽ (newitem->jq_മൂല്യം == ശൂന്യ)
    {
	vim_free(newitem);
	clear_tv(rettv);
	തിരികെ;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_മൂല്യം = *rettv;
    എങ്കിൽ (item == ശൂന്യ)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = ശൂന്യ;
	എങ്കിൽ (newitem->jq_prev == ശൂന്യ)
	    head->jq_next = newitem;
	അല്ലെങ്കിൽ
	    newitem->jq_prev->jq_next = newitem;
    }
    അല്ലെങ്കിൽ
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	എങ്കിൽ (newitem->jq_next == ശൂന്യ)
	    head->jq_prev = newitem;
	അല്ലെങ്കിൽ
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    സ്ഥിതം ശൂന്യം
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    പൂർണ്ണസംഖ്യ	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    എങ്കിൽ (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	എങ്കിൽ (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	തിരികെ;
    }
    arg = argv[1].vval.v_string;
    എങ്കിൽ (arg == ശൂന്യ)
	arg = (char_u *)"";

    എങ്കിൽ (STRCMP(cmd, "ex") == 0)
    {
	പൂർണ്ണസംഖ്യ	called_emsg_before = called_emsg;
	char_u	*p = arg;
	പൂർണ്ണസംഖ്യ	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (അക്ഷരം *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	എങ്കിൽ (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	എങ്കിൽ (do_emsg_silent)
	    --emsg_silent;
	എങ്കിൽ (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (അക്ഷരം *)get_vim_var_str(VV_ERRMSG));
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (അക്ഷരം *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_എണ്ണം = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	പൂർണ്ണസംഖ്യ is_call = cmd[0] == 'c';
	പൂർണ്ണസംഖ്യ id_idx = is_call ? 3 : 2;

	എങ്കിൽ (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_സംഖ്യ)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    എങ്കിൽ (p_verbose > 2)
		emsg(_(e_last_argument_for_വ്യഞ്ജകം_call_must_be_സംഖ്യ));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    എങ്കിൽ (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	അല്ലെങ്കിൽ
	{
	    typval_T	*tv = ശൂന്യ;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ശൂന്യ;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    എങ്കിൽ (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (അക്ഷരം *)arg);
		tv = eval_വ്യഞ്ജകം(arg, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
	    {
		ch_log(channel, "Calling '%s'", (അക്ഷരം *)arg);
		എങ്കിൽ (func_call(arg, &argv[2], ശൂന്യ, ശൂന്യ, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    എങ്കിൽ (argv[id_idx].v_type == VAR_സംഖ്യ)
	    {
		പൂർണ്ണസംഖ്യ id = argv[id_idx].vval.v_സംഖ്യ;

		എങ്കിൽ (tv != ശൂന്യ)
		    json = json_encode_nr_വ്യഞ്ജകം(id, tv, options | JSON_NL);
		എങ്കിൽ (tv == ശൂന്യ || (json != ശൂന്യ && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_വ്യഞ്ജകം(id, &err_tv, options | JSON_NL);
		}
		എങ്കിൽ (json != ശൂന്യ)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (പൂർണ്ണസംഖ്യ)STRLEN(json), (അക്ഷരം *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    എങ്കിൽ (tv == &res_tv)
		clear_tv(tv);
	    അല്ലെങ്കിൽ
		free_tv(tv);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (അക്ഷരം *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    സ്ഥിതം ശൂന്യം
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (അക്ഷരം *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_കണ്ണി(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    സ്ഥിതം ശൂന്യം
append_to_buffer(
    buf_T	*buffer,
    char_u	*സന്ദേശം,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_വരി_എണ്ണം;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    പൂർണ്ണസംഖ്യ		save_p_ma = buffer->b_p_ma;
    പൂർണ്ണസംഖ്യ		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    എങ്കിൽ (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	എങ്കിൽ (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	തിരികെ;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    എങ്കിൽ (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (പൂർണ്ണസംഖ്യ)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    എങ്കിൽ (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	തിരികെ;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    എങ്കിൽ (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, സന്ദേശം, TRUE);
	lnum = 0;
    }
    അല്ലെങ്കിൽ
	ml_append(lnum, സന്ദേശം, 0, FALSE);
    appended_വരികൾ_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    എങ്കിൽ (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    അല്ലെങ്കിൽ
	buffer->b_p_ma = save_p_ma;

    എങ്കിൽ (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    എങ്കിൽ (wp->w_buffer == buffer)
	    {
		പൂർണ്ണസംഖ്യ move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		എങ്കിൽ (move_cursor || wp->w_topline > buffer->b_ml.ml_വരി_എണ്ണം)
		{
		    win_T *save_curwin = curwin;

		    എങ്കിൽ (move_cursor)
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

    എങ്കിൽ (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    എങ്കിൽ (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_വരി_എണ്ണം;
	}
    }
}

    സ്ഥിതം ശൂന്യം
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *സന്ദേശം;

    ആകുമ്പോൾ ((സന്ദേശം = channel_get(channel, part, ശൂന്യ)) != ശൂന്യ)
    {
	ch_log(channel, "Dropping message '%s'", (അക്ഷരം *)സന്ദേശം);
	vim_free(സന്ദേശം);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    തിരികെ ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*സന്ദേശം = ശൂന്യ;
    blob_T	*blob = ശൂന്യ;
    typval_T	*listtv = ശൂന്യ;
    typval_T	argv[CH_JSON_MAX_ARGS];
    പൂർണ്ണസംഖ്യ		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = ശൂന്യ;
    buf_T	*buffer = ശൂന്യ;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		called_otc;		// one time callbackup
    പൂർണ്ണസംഖ്യ		raw_len = 0;

    എങ്കിൽ (channel->ch_nb_close_cb != ശൂന്യ
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	തിരികെ FALSE;

    // Use a message-specific callback, part callback or channel callback
    വേണ്ടി (cbitem = cbhead->cq_next; cbitem != ശൂന്യ; cbitem = cbitem->cq_next)
	എങ്കിൽ (cbitem->cq_seq_nr == 0)
	    നിർത്തുക;
    എങ്കിൽ (cbitem != ശൂന്യ)
	callback = &cbitem->cq_callback;
    അല്ലെങ്കിൽ എങ്കിൽ (ch_part->ch_callback.cb_name != ശൂന്യ)
	callback = &ch_part->ch_callback;
    അല്ലെങ്കിൽ എങ്കിൽ (channel->ch_callback.cb_name != ശൂന്യ)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    എങ്കിൽ (buffer != ശൂന്യ && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == ശൂന്യ))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = ശൂന്യ;
	buffer = ശൂന്യ;
    }

    എങ്കിൽ (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	പൂർണ്ണസംഖ്യ		argc = 0;

	// Get any json message in the queue.
	എങ്കിൽ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    എങ്കിൽ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(ശൂന്യം)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    എങ്കിൽ (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		തിരികെ FALSE;
	}

	എങ്കിൽ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    എങ്കിൽ (d != ശൂന്യ)
	    {
		എങ്കിൽ (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		അല്ലെങ്കിൽ
		    di = dict_find(d, (char_u *)"request_seq", -1);
		എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_സംഖ്യ)
		    seq_nr = di->di_tv.vval.v_സംഖ്യ;
	    }

	    argv[1] = *listtv;
	}
	അല്ലെങ്കിൽ
	{
	    വേണ്ടി (item = listtv->vval.v_list->lv_first;
		    item != ശൂന്യ && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    ആകുമ്പോൾ (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    എങ്കിൽ (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		തിരികെ TRUE;
	    }

	    എങ്കിൽ (argv[0].v_type != VAR_സംഖ്യ)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		തിരികെ FALSE;
	    }
	    seq_nr = argv[0].vval.v_സംഖ്യ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (channel_peek(channel, part) == ശൂന്യ)
    {
	// nothing to read on RAW or NL channel
	തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ
    {
	// If there is no callback or buffer drop the message.
	എങ്കിൽ (callback == ശൂന്യ && buffer == ശൂന്യ)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    എങ്കിൽ (channel->ch_close_cb.cb_name == ശൂന്യ && !channel->ch_drop_never)
		drop_messages(channel, part);
	    തിരികെ FALSE;
	}

	എങ്കിൽ (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = ശൂന്യ;
	    char_u  *buf;
	    readq_T *കണ്ണി;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ആകുമ്പോൾ (TRUE)
	    {
		കണ്ണി = channel_peek(channel, part);
		nl = channel_first_nl(കണ്ണി);
		എങ്കിൽ (nl != ശൂന്യ)
		    നിർത്തുക;
		എങ്കിൽ (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    എങ്കിൽ (ch_part->ch_fd == INVALID_FD && കണ്ണി->rq_buflen > 0)
			നിർത്തുക;
		    തിരികെ FALSE; // incomplete message
		}
	    }
	    buf = കണ്ണി->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    വേണ്ടി (p = buf; (nl == ശൂന്യ || p < nl)
					    && p < buf + കണ്ണി->rq_buflen; ++p)
		എങ്കിൽ (*p == NUL)
		    *p = NL;

	    എങ്കിൽ (nl == ശൂന്യ)
	    {
		// get the whole buffer, drop the NL
		സന്ദേശം = channel_get(channel, part, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (nl + 1 == buf + കണ്ണി->rq_buflen)
	    {
		// get the whole buffer
		സന്ദേശം = channel_get(channel, part, ശൂന്യ);
		*nl = NUL;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		സന്ദേശം = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (പൂർണ്ണസംഖ്യ)(nl - buf) + 1);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    സന്ദേശം = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : ശൂന്യ);
	}

	എങ്കിൽ (സന്ദേശം == ശൂന്യ)
	    തിരികെ FALSE; // out of memory (and avoids Coverity warning)

	എങ്കിൽ (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    എങ്കിൽ (blob == ശൂന്യ)
	    {
		vim_free(സന്ദേശം);
		തിരികെ FALSE;
	    }
	    എങ്കിൽ (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(സന്ദേശം);
		തിരികെ FALSE;
	    }
	    എങ്കിൽ (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, സന്ദേശം, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	അല്ലെങ്കിൽ
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = സന്ദേശം;
	}
    }

    called_otc = FALSE;
    എങ്കിൽ (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	പൂർണ്ണസംഖ്യ lsp_req_സന്ദേശം = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	എങ്കിൽ ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_താക്കോൽ(argv[1].vval.v_dict, "method"))
	    lsp_req_സന്ദേശം = TRUE;

	എങ്കിൽ (!lsp_req_സന്ദേശം)
	{
	    വേണ്ടി (cbitem = cbhead->cq_next; cbitem != ശൂന്യ;
		    cbitem = cbitem->cq_next)
	    {
		എങ്കിൽ (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    നിർത്തുക;
		}
	    }
	}
    }

    എങ്കിൽ (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	എങ്കിൽ (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    എങ്കിൽ (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_തള്ളുക_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_സംഖ്യ;
		free_tv(listtv);
		listtv = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (callback != ശൂന്യ || buffer != ശൂന്യ)
    {
	എങ്കിൽ (buffer != ശൂന്യ)
	{
	    എങ്കിൽ (സന്ദേശം == ശൂന്യ)
		// JSON or JS mode: re-encode the message.
		സന്ദേശം = json_encode(listtv, ch_mode);
	    എങ്കിൽ (സന്ദേശം != ശൂന്യ)
	    {
#ifdef FEAT_TERMINAL
		എങ്കിൽ (buffer->b_term != ശൂന്യ)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    എങ്കിൽ (part == PART_ERR)
		    {
			char_u *cp;
			പൂർണ്ണസംഖ്യ    extra = 0;

			വേണ്ടി (cp = സന്ദേശം; *cp != NUL; ++cp)
			    എങ്കിൽ (*cp == NL && (cp == സന്ദേശം || cp[-1] != CAR))
				++extra;
			എങ്കിൽ (extra > 0)
			{
			    char_u *crlf_സന്ദേശം = alloc(STRLEN(സന്ദേശം) + extra + 1);

			    എങ്കിൽ (crlf_സന്ദേശം != ശൂന്യ)
			    {
				char_u *q = crlf_സന്ദേശം;

				വേണ്ടി (cp = സന്ദേശം; *cp != NUL; ++cp)
				{
				    എങ്കിൽ (*cp == NL && (cp == സന്ദേശം || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_സന്ദേശം, channel);
				vim_free(crlf_സന്ദേശം);
			    }
			}
			അല്ലെങ്കിൽ
			    write_to_term(buffer, സന്ദേശം, channel);
		    }
		    അല്ലെങ്കിൽ
			write_to_term(buffer, സന്ദേശം, channel);
		}
		അല്ലെങ്കിൽ
#endif
		    append_to_buffer(buffer, സന്ദേശം, channel, part);
	    }
	}

	എങ്കിൽ (callback != ശൂന്യ)
	{
	    എങ്കിൽ (cbitem != ശൂന്യ)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    അല്ലെങ്കിൽ
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (അക്ഷരം *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    അല്ലെങ്കിൽ
	ch_log(channel, "Dropping message %d", seq_nr);

    എങ്കിൽ (listtv != ശൂന്യ)
	free_tv(listtv);
    എങ്കിൽ (blob != ശൂന്യ)
	blob_unref(blob);
    vim_free(സന്ദേശം);

    തിരികെ TRUE;
}

#എങ്കിൽ defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    പൂർണ്ണസംഖ്യ
channel_can_write_to(channel_T *channel)
{
    തിരികെ channel != ശൂന്യ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    പൂർണ്ണസംഖ്യ
channel_is_open(channel_T *channel)
{
    തിരികെ channel != ശൂന്യ && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    സ്ഥിതം ശൂന്യം *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    എങ്കിൽ (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	എങ്കിൽ (head->jq_next == ശൂന്യ)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	തിരികെ head->jq_next;
    }
    തിരികെ channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    തിരികെ channel_readahead_pointer(channel, part) != ശൂന്യ;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    സ്ഥിതം അക്ഷരം *
channel_status(channel_T *channel, പൂർണ്ണസംഖ്യ req_part)
{
    ch_part_T part;
    പൂർണ്ണസംഖ്യ has_readahead = FALSE;

    എങ്കിൽ (channel == ശൂന്യ)
	 തിരികെ "fail";
    എങ്കിൽ (req_part == PART_OUT)
    {
	എങ്കിൽ (channel->CH_OUT_FD != INVALID_FD)
	    തിരികെ "open";
	എങ്കിൽ (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (req_part == PART_ERR)
    {
	എങ്കിൽ (channel->CH_ERR_FD != INVALID_FD)
	    തിരികെ "open";
	എങ്കിൽ (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (channel_is_open(channel))
	    തിരികെ "open";
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	    എങ്കിൽ (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		നിർത്തുക;
	    }
    }

    എങ്കിൽ (has_readahead)
	തിരികെ "buffered";
    തിരികെ "closed";
}

    സ്ഥിതം ശൂന്യം
channel_part_info(channel_T *channel, dict_T *dict, അക്ഷരം *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    അക്ഷരം	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    എങ്കിൽ (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    അല്ലെങ്കിൽ എങ്കിൽ (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    അല്ലെങ്കിൽ
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (പൂർണ്ണസംഖ്യ)s.length);

    STRCPY(namebuf + tail, "mode");
    തിരഞ്ഞെടുപ്പ് (chanpart->ch_mode)
    {
	അവസ്ഥ CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    നിർത്തുക;
	അവസ്ഥ CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    നിർത്തുക;
	അവസ്ഥ CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    നിർത്തുക;
	അവസ്ഥ CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    നിർത്തുക;
	അവസ്ഥ CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    നിർത്തുക;
	അവസ്ഥ CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    നിർത്തുക;
	അവസ്ഥ CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    STR_LITERAL_SET(s, "");
	    നിർത്തുക;
    }
    dict_add_string_len(dict, namebuf, s.string, (പൂർണ്ണസംഖ്യ)s.length);

    STRCPY(namebuf + tail, "io");
    എങ്കിൽ (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    അല്ലെങ്കിൽ തിരഞ്ഞെടുപ്പ് (chanpart->ch_io)
    {
	അവസ്ഥ JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    നിർത്തുക;
	അവസ്ഥ JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    നിർത്തുക;
	അവസ്ഥ JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    നിർത്തുക;
	അവസ്ഥ JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    നിർത്തുക;
	അവസ്ഥ JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    STR_LITERAL_SET(s, "");
	    നിർത്തുക;
    }
    dict_add_string_len(dict, namebuf, s.string, (പൂർണ്ണസംഖ്യ)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_സംഖ്യ(dict, namebuf, chanpart->ch_timeout);
}

    സ്ഥിതം ശൂന്യം
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_സംഖ്യ(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    എങ്കിൽ (channel->ch_hostname != ശൂന്യ)
    {
	എങ്കിൽ (channel->ch_port)
	    dict_add_സംഖ്യ(dict, "port", channel->ch_port);
	അല്ലെങ്കിൽ
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    അല്ലെങ്കിൽ
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
    ശൂന്യം
channel_close(channel_T *channel, പൂർണ്ണസംഖ്യ invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    എങ്കിൽ (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	എങ്കിൽ (channel->ch_close_cb.cb_name != ശൂന്യ)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	{
	    എങ്കിൽ (channel->ch_close_cb.cb_name != ശൂന്യ
			    || channel->ch_part[part].ch_bufref.br_buf != ശൂന്യ)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		എങ്കിൽ (channel->ch_close_cb.cb_name == ശൂന്യ)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		ആകുമ്പോൾ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	എങ്കിൽ (channel->ch_close_cb.cb_name != ശൂന്യ)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (അക്ഷരം *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      എങ്കിൽ (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      എങ്കിൽ (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ശൂന്യ;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ശൂന്യ;
    channel->ch_ss_close_cb = ശൂന്യ;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    സ്ഥിതം ശൂന്യം
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    സ്ഥിതം ശൂന്യം
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    എങ്കിൽ (wq->wq_next == ശൂന്യ)
	wq->wq_prev = ശൂന്യ;
    അല്ലെങ്കിൽ
	wq->wq_next->wq_prev = ശൂന്യ;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    സ്ഥിതം ശൂന്യം
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    ആകുമ്പോൾ (channel_peek(channel, part) != ശൂന്യ)
	vim_free(channel_get(channel, part, ശൂന്യ));

    ആകുമ്പോൾ (cb_head->cq_next != ശൂന്യ)
    {
	cbq_T *കണ്ണി = cb_head->cq_next;

	remove_cb_കണ്ണി(cb_head, കണ്ണി);
	free_callback(&കണ്ണി->cq_callback);
	vim_free(കണ്ണി);
    }

    ആകുമ്പോൾ (json_head->jq_next != ശൂന്യ)
    {
	free_tv(json_head->jq_next->jq_മൂല്യം);
	remove_json_കണ്ണി(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    ആകുമ്പോൾ (ch_part->ch_writeque.wq_next != ശൂന്യ)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    ശൂന്യം
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

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
channel_free_all(ശൂന്യം)
{
    channel_T *channel;

    ch_log(ശൂന്യ, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_സന്ദേശം_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    എങ്കിൽ (in_part->ch_writeque.wq_next != ശൂന്യ)
	തിരികെ TRUE;
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ FALSE;
    തിരികെ in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_വരി_എണ്ണം)
	    : (in_part->ch_buf_മുകൾ <= in_part->ch_buf_bot
			    && in_part->ch_buf_മുകൾ <= buf->b_ml.ml_വരി_എണ്ണം);
}

#എങ്കിൽ defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_fill_wfds(പൂർണ്ണസംഖ്യ maxfd_arg, fd_set *wfds)
{
    പൂർണ്ണസംഖ്യ		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	എങ്കിൽ (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (പൂർണ്ണസംഖ്യ)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((പൂർണ്ണസംഖ്യ)in_part->ch_fd, wfds);
	    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)in_part->ch_fd >= maxfd)
		maxfd = (പൂർണ്ണസംഖ്യ)in_part->ch_fd + 1;
	}
    }
    തിരികെ maxfd;
}
#അല്ലെങ്കിൽ
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_fill_poll_write(പൂർണ്ണസംഖ്യ nfd_in, ഘടന pollfd *fds)
{
    പൂർണ്ണസംഖ്യ		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	എങ്കിൽ (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	അല്ലെങ്കിൽ
	    in_part->ch_poll_idx = -1;
    }
    തിരികെ nfd;
}
#endif

തരനിർവചനം എണ്ണൽ {
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
    സ്ഥിതം channel_wait_result
channel_wait(channel_T *channel, sock_T fd, പൂർണ്ണസംഖ്യ timeout)
{
    എങ്കിൽ (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    എങ്കിൽ (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	പൂർണ്ണസംഖ്യ	sleep_time;
	DWORD	deadline = Get_Tick_എണ്ണം() + timeout;
	പൂർണ്ണസംഖ്യ	delay = 1;

	// reading from a pipe, not a socket
	ആകുമ്പോൾ (TRUE)
	{
	    പൂർണ്ണസംഖ്യ r = PeekNamedPipe((HANDLE)fd, ശൂന്യ, 0, ശൂന്യ, &nread, ശൂന്യ);

	    എങ്കിൽ (r && nread > 0)
		തിരികെ CW_READY;

	    എങ്കിൽ (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (r == 0)
		തിരികെ CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_വരികൾ();

	    sleep_time = deadline - Get_Tick_എണ്ണം();
	    എങ്കിൽ (sleep_time <= 0)
		നിർത്തുക;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    എങ്കിൽ (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    എങ്കിൽ (delay > 10)
		delay = 10;
	}
    }
    അല്ലെങ്കിൽ
#endif
    {
#എങ്കിൽ defined(HAVE_SELECT)
	ഘടന timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	പൂർണ്ണസംഖ്യ		ret;
	പൂർണ്ണസംഖ്യ		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	വേണ്ടി (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((പൂർണ്ണസംഖ്യ)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (പൂർണ്ണസംഖ്യ)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ശൂന്യ, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    എങ്കിൽ (ret == -1 && errno == EINTR)
		തുടരുക;
# endif
	    എങ്കിൽ (ret > 0)
	    {
		എങ്കിൽ (FD_ISSET(fd, &rfds))
		    തിരികെ CW_READY;
		channel_write_any_വരികൾ();
		തുടരുക;
	    }
	    നിർത്തുക;
	}
#അല്ലെങ്കിൽ
	വേണ്ടി (;;)
	{
	    ഘടന pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    പൂർണ്ണസംഖ്യ		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    എങ്കിൽ (poll(fds, nfd, timeout) > 0)
	    {
		എങ്കിൽ (fds[0].revents & POLLIN)
		    തിരികെ CW_READY;
		channel_write_any_വരികൾ();
		തുടരുക;
	    }
	    നിർത്തുക;
	}
#endif
    }
    തിരികെ CW_NOT_READY;
}

    സ്ഥിതം ശൂന്യം
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, പൂർണ്ണസംഖ്യ is_err, അക്ഷരം *func)
{
    അക്ഷരം	സന്ദേശം[] = "%s(): Read %s from ch_part[%d], closing";

    എങ്കിൽ (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, സന്ദേശം, func, "error", part);
    അല്ലെങ്കിൽ
	ch_log(channel, സന്ദേശം, func, "EOF", part);

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
    എങ്കിൽ (channel->ch_nb_close_cb != ശൂന്യ)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_സന്ദേശം_RAW,
			      (പൂർണ്ണസംഖ്യ)STRLEN_LITERAL(DETACH_സന്ദേശം_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    എങ്കിൽ (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	എങ്കിൽ (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    സ്ഥിതം ശൂന്യം
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    എങ്കിൽ (channel->ch_nb_close_cb != ശൂന്യ)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    എങ്കിൽ (channel->ch_ss_close_cb != ശൂന്യ)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    സ്ഥിതം ശൂന്യം
channel_read(channel_T *channel, ch_part_T part, അക്ഷരം *func)
{
    സ്ഥിതം char_u	*buf = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			len = 0;
    പൂർണ്ണസംഖ്യ			readlen = 0;
    sock_T		fd;
    പൂർണ്ണസംഖ്യ			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    എങ്കിൽ (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	തിരികെ;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    എങ്കിൽ (buf == ശൂന്യ)
    {
	buf = alloc(MAXMSGSIZE);
	എങ്കിൽ (buf == ശൂന്യ)
	    തിരികെ;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    വേണ്ടി (;;)
    {
	എങ്കിൽ (channel_wait(channel, fd, 0) != CW_READY)
	    നിർത്തുക;
	എങ്കിൽ (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    ഘടന sockaddr_storage	client;

	    newchannel = add_channel();
	    എങ്കിൽ (newchannel == ശൂന്യ)
	    {
		ch_error(ശൂന്യ, "Cannot allocate channel.");
		തിരികെ;
	    }
	    socklen = വലുപ്പം(client);
	    newfd = accept(fd, (ഘടന sockaddr*)&client, &socklen);
	    എങ്കിൽ (newfd < 0)
	    {
		ch_error(ശൂന്യ, "Cannot accept channel.");
		channel_free(newchannel);
		തിരികെ;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    എങ്കിൽ (channel->ch_ss_accept_cb != ശൂന്യ)
	    {
		channel->ch_ss_accept_cb(newchannel);
		തിരികെ;
	    }
#endif

	    എങ്കിൽ (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		അക്ഷരം addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((ഘടന sockaddr_in*)&client)->sin_addr,
			addr, വലുപ്പം(addr));
		vim_snprintf((അക്ഷരം *)namebuf, വലുപ്പം(namebuf), "%s:%d",
			addr,
			ntohs(((ഘടന sockaddr_in*)&client)->sin_port));
#അല്ലെങ്കിൽ
		vim_snprintf((അക്ഷരം *)namebuf, വലുപ്പം(namebuf), "%s:%d",
		    inet_ntoa(((ഘടന sockaddr_in*)&client)->sin_addr),
		    ntohs(((ഘടന sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    അല്ലെങ്കിൽ എങ്കിൽ (client.ss_family == AF_INET6)
	    {
		അക്ഷരം addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((ഘടന sockaddr_in6*)&client)->sin6_addr,
			addr, വലുപ്പം(addr));
		vim_snprintf((അക്ഷരം *)namebuf, വലുപ്പം(namebuf), "[%s]:%d",
			addr,
			ntohs(((ഘടന sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    അല്ലെങ്കിൽ എങ്കിൽ (client.ss_family == AF_UNIX)
		vim_snprintf((അക്ഷരം *)namebuf, വലുപ്പം(namebuf),
							    "unix:anonymous");
	    അല്ലെങ്കിൽ
		vim_snprintf((അക്ഷരം *)namebuf, വലുപ്പം(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    തിരികെ;
	}
	എങ്കിൽ (use_socket)
	    len = sock_read(fd, (അക്ഷരം *)buf, MAXMSGSIZE);
	അല്ലെങ്കിൽ
	    len = fd_read(fd, (അക്ഷരം *)buf, MAXMSGSIZE);
	എങ്കിൽ (len <= 0)
	    നിർത്തുക;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    എങ്കിൽ (readlen <= 0)
    {
	എങ്കിൽ (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#എങ്കിൽ defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    അല്ലെങ്കിൽ എങ്കിൽ (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    ശൂന്യം
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
    സ്ഥിതം char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, പൂർണ്ണസംഖ്യ timeout, പൂർണ്ണസംഖ്യ raw, പൂർണ്ണസംഖ്യ *outlen)
{
    char_u	*buf;
    char_u	*സന്ദേശം;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*കണ്ണി;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    ആകുമ്പോൾ (TRUE)
    {
	കണ്ണി = channel_peek(channel, part);
	എങ്കിൽ (കണ്ണി != ശൂന്യ)
	{
	    എങ്കിൽ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(കണ്ണി) != ശൂന്യ))
		// got a complete message
		നിർത്തുക;
	    എങ്കിൽ (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		തുടരുക;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    എങ്കിൽ (raw || fd == INVALID_FD)
		നിർത്തുക;
	}

	// Wait for up to the channel timeout.
	എങ്കിൽ (fd == INVALID_FD)
	    തിരികെ ശൂന്യ;
	എങ്കിൽ (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    തിരികെ ശൂന്യ;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    എങ്കിൽ (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != ശൂന്യ)
    {
	സന്ദേശം = channel_get_all(channel, part, outlen);
    }
    അല്ലെങ്കിൽ
    {
	char_u *p;

	buf = കണ്ണി->rq_buffer;
	nl = channel_first_nl(കണ്ണി);

	// Convert NUL to NL, the internal representation.
	വേണ്ടി (p = buf; (nl == ശൂന്യ || p < nl) && p < buf + കണ്ണി->rq_buflen; ++p)
	    എങ്കിൽ (*p == NUL)
		*p = NL;

	എങ്കിൽ (nl == ശൂന്യ)
	{
	    // must be a closed channel with missing NL
	    സന്ദേശം = channel_get(channel, part, ശൂന്യ);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (nl + 1 == buf + കണ്ണി->rq_buflen)
	{
	    // get the whole buffer
	    സന്ദേശം = channel_get(channel, part, ശൂന്യ);
	    *nl = NUL;
	}
	അല്ലെങ്കിൽ
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    സന്ദേശം = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (പൂർണ്ണസംഖ്യ)(nl - buf) + 1);
	}
    }
    എങ്കിൽ (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != ശൂന്യ ? *outlen : (പൂർണ്ണസംഖ്യ)STRLEN(സന്ദേശം));
    തിരികെ സന്ദേശം;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    പൂർണ്ണസംഖ്യ
channel_in_blocking_wait(ശൂന്യം)
{
    തിരികെ channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    സ്ഥിതം ശൂന്യം
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, പൂർണ്ണസംഖ്യ timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (ശൂന്യം)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#അല്ലെങ്കിൽ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	പൂർണ്ണസംഖ്യ	    timeout_arg,
	പൂർണ്ണസംഖ്യ	    id,
	typval_T    **rettv)
{
    പൂർണ്ണസംഖ്യ		more;
    sock_T	fd;
    പൂർണ്ണസംഖ്യ		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    എങ്കിൽ (id >= 0)
	channel_add_block_id(chanpart, id);

    വേണ്ടി (;;)
    {
	എങ്കിൽ (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (ശൂന്യം)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	എങ്കിൽ (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    നിർത്തുക;
	}

	എങ്കിൽ (!more)
	{
	    ശൂന്യം *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    ശൂന്യം *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    എങ്കിൽ (channel_parse_messages())
		തുടരുക;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    എങ്കിൽ (readahead_ptr != ശൂന്യ && readahead_ptr != prev_readahead_ptr)
		തുടരുക;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (പൂർണ്ണസംഖ്യ)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    എങ്കിൽ (timeout <= 0 || fd == INVALID_FD)
	    {
		എങ്കിൽ (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		നിർത്തുക;
	    }
	    // If there was an incomplete message use the deadline for that.
	    എങ്കിൽ (chanpart->ch_wait_len > 0)
	    {
		പൂർണ്ണസംഖ്യ	ഇടത്;
#ifdef MSWIN
		ഇടത് = chanpart->ch_deadline - Get_Tick_എണ്ണം() + 1;
#അല്ലെങ്കിൽ
		{
		    ഘടന timeval now_tv;

		    gettimeofday(&now_tv, ശൂന്യ);
		    ഇടത് = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		എങ്കിൽ (ഇടത് < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		അല്ലെങ്കിൽ എങ്കിൽ (ഇടത് < timeout)
		    timeout = ഇടത്;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    എങ്കിൽ (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    തിരികെ retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, പൂർണ്ണസംഖ്യ check_open, പൂർണ്ണസംഖ്യ reading, ch_part_T part)
{
    channel_T	*channel = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		has_readahead = FALSE;

    എങ്കിൽ (tv->v_type == VAR_JOB)
    {
	എങ്കിൽ (tv->vval.v_job != ശൂന്യ)
	    channel = tv->vval.v_job->jv_channel;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (channel != ശൂന്യ && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_എണ്ണം ? part : channel_part_read(channel));

    എങ്കിൽ (check_open && (channel == ശൂന്യ || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	തിരികെ ശൂന്യ;
    }
    തിരികെ channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    സ്ഥിതം ശൂന്യം
common_channel_read(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ raw, പൂർണ്ണസംഖ്യ blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_എണ്ണം;
    jobopt_T	opt;
    പൂർണ്ണസംഖ്യ		mode;
    പൂർണ്ണസംഖ്യ		timeout;
    പൂർണ്ണസംഖ്യ		id = -1;
    typval_T	*listtv = ശൂന്യ;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    clear_job_options(&opt);
    എങ്കിൽ (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	പോകുക theend;

    എങ്കിൽ (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    എങ്കിൽ (channel == ശൂന്യ)
	പോകുക theend;

    എങ്കിൽ (part == PART_എണ്ണം)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    എങ്കിൽ (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    എങ്കിൽ (blob || mode == CH_MODE_BLOB)
    {
	പൂർണ്ണസംഖ്യ	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	എങ്കിൽ (p != ശൂന്യ)
	{
	    blob_T	*b = blob_alloc();

	    എങ്കിൽ (b != ശൂന്യ)
	    {
		b->bv_ga.ga_len = outlen;
		എങ്കിൽ (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		അല്ലെങ്കിൽ
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, ശൂന്യ);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	എങ്കിൽ (listtv != ശൂന്യ)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	അല്ലെങ്കിൽ
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_സംഖ്യ = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#എങ്കിൽ defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    ശൂന്യം
channel_handle_events(പൂർണ്ണസംഖ്യ only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	എങ്കിൽ (only_keep_open && !channel->ch_keep_open)
	    തുടരുക;

	// check the socket and pipes
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    എങ്കിൽ (fd == INVALID_FD)
		തുടരുക;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    പൂർണ്ണസംഖ്യ r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    എങ്കിൽ (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    അല്ലെങ്കിൽ എങ്കിൽ (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	എങ്കിൽ (channel->ch_job != ശൂന്യ)
	{
	    job_T *job = channel->ch_job;

	    എങ്കിൽ (job->jv_tty_out != ശൂന്യ && job->jv_status == JOB_FINISHED)
		വേണ്ടി (part = PART_SOCK; part < PART_എണ്ണം; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#എങ്കിൽ defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    പൂർണ്ണസംഖ്യ
channel_any_keep_open(ശൂന്യം)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	എങ്കിൽ (channel->ch_keep_open)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    ശൂന്യം
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    പൂർണ്ണസംഖ്യ		fd = ch_part->ch_fd;

    എങ്കിൽ (fd == INVALID_FD)
	തിരികെ;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#അല്ലെങ്കിൽ
    (ശൂന്യം)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	പൂർണ്ണസംഖ്യ	  len_arg,
	അക്ഷരം	  *fun)
{
    പൂർണ്ണസംഖ്യ		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    പൂർണ്ണസംഖ്യ		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    എങ്കിൽ (fd == INVALID_FD)
    {
	എങ്കിൽ (!channel->ch_error && fun != ശൂന്യ)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	തിരികെ FAIL;
    }

    എങ്കിൽ (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    എങ്കിൽ (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_സന്ദേശം = 0;
    }

    വേണ്ടി (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	പൂർണ്ണസംഖ്യ	    len;

	എങ്കിൽ (wq->wq_next != ശൂന്യ)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (len_arg == 0)
		// nothing to write, called from channel_select_check()
		തിരികെ OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	എങ്കിൽ (part == PART_SOCK)
	    res = sock_write(fd, (അക്ഷരം *)buf, len);
	അല്ലെങ്കിൽ
	{
	    res = fd_write(fd, (അക്ഷരം *)buf, len);
#ifdef MSWIN
	    എങ്കിൽ (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ശൂന്യ);
	    }
#endif
	}
	എങ്കിൽ (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	എങ്കിൽ (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    എങ്കിൽ (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    എങ്കിൽ (res == len)
	    {
		// Wrote all the buf[len] bytes.
		എങ്കിൽ (entry != ശൂന്യ)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    തുടരുക;
		}
		എങ്കിൽ (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    അല്ലെങ്കിൽ
	    {
		// Wrote only buf[res] bytes, can't write more now.
		എങ്കിൽ (entry != ശൂന്യ)
		{
		    എങ്കിൽ (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (അക്ഷരം *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		അല്ലെങ്കിൽ
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		എങ്കിൽ (wq->wq_prev != ശൂന്യ
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    എങ്കിൽ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((അക്ഷരം *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    എങ്കിൽ (last != ശൂന്യ)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = ശൂന്യ;
			എങ്കിൽ (wq->wq_prev == ശൂന്യ)
			    wq->wq_next = last;
			അല്ലെങ്കിൽ
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			എങ്കിൽ (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (res != len)
	{
	    എങ്കിൽ (!channel->ch_error && fun != ശൂന്യ)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    തിരികെ FAIL;
	}

	channel->ch_error = FALSE;
	തിരികെ OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    സ്ഥിതം channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	പൂർണ്ണസംഖ്യ	    len,
	പൂർണ്ണസംഖ്യ	    id,
	പൂർണ്ണസംഖ്യ	    eval,
	jobopt_T    *opt,
	അക്ഷരം	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ ശൂന്യ;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    എങ്കിൽ (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	തിരികെ ശൂന്യ;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    എങ്കിൽ (opt->jo_callback.cb_name != ശൂന്യ && *opt->jo_callback.cb_name != NUL)
    {
	എങ്കിൽ (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    തിരികെ ശൂന്യ;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    എങ്കിൽ (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == ശൂന്യ)
	തിരികെ channel;
    തിരികെ ശൂന്യ;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    സ്ഥിതം ശൂന്യം
ch_വ്യഞ്ജകം_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    പൂർണ്ണസംഖ്യ		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    പൂർണ്ണസംഖ്യ		timeout;
    പൂർണ്ണസംഖ്യ		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    എങ്കിൽ (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	തിരികെ;
    }

    എങ്കിൽ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	അക്ഷരം		*താക്കോൽ = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	എങ്കിൽ (rettv_dict_alloc(rettv) == FAIL)
	    തിരികെ;

	എങ്കിൽ (check_for_dict_arg(argvars, 1) == FAIL)
	    തിരികെ;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)താക്കോൽ, -1);
	എങ്കിൽ (argvars[2].v_type == VAR_DICT
		&& dict_has_താക്കോൽ(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type != VAR_സംഖ്യ
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_മൂല്യം_for_argument_str), താക്കോൽ);
	    തിരികെ;
	}

	എങ്കിൽ (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_സന്ദേശം_id;
	    എങ്കിൽ (di == ശൂന്യ)
		dict_add_സംഖ്യ(d, "seq", id);
	    അല്ലെങ്കിൽ
		di->di_tv.vval.v_സംഖ്യ = id;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_സന്ദേശം_id;
	    എങ്കിൽ (di == ശൂന്യ)
		dict_add_സംഖ്യ(d, "id", id);
	    അല്ലെങ്കിൽ
		di->di_tv.vval.v_സംഖ്യ = id;
	}
	അല്ലെങ്കിൽ
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    എങ്കിൽ (di != ശൂന്യ && di->di_tv.v_type == VAR_സംഖ്യ)
		id = di->di_tv.vval.v_സംഖ്യ;
	}
	എങ്കിൽ (ch_mode == CH_MODE_LSP && !dict_has_താക്കോൽ(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_സന്ദേശം(&argvars[1]);
    }
    അല്ലെങ്കിൽ
    {
	id = ++channel->ch_last_സന്ദേശം_id;
	text = json_encode_nr_വ്യഞ്ജകം(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    എങ്കിൽ (text == ശൂന്യ)
	തിരികെ;

    channel = send_common(argvars, text, (പൂർണ്ണസംഖ്യ)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    എങ്കിൽ (channel != ശൂന്യ && eval)
    {
	എങ്കിൽ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	അല്ലെങ്കിൽ
	    timeout = channel_get_timeout(channel, part_read);
	എങ്കിൽ (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    എങ്കിൽ (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_സംഖ്യ;
		free_tv(listtv);
	    }
	    അല്ലെങ്കിൽ
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_സംഖ്യ;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    എങ്കിൽ (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	എങ്കിൽ (rettv->vval.v_dict != ശൂന്യ)
	    dict_add_സംഖ്യ(rettv->vval.v_dict, "seq", id);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	എങ്കിൽ (rettv->vval.v_dict != ശൂന്യ)
	    dict_add_സംഖ്യ(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    സ്ഥിതം ശൂന്യം
ch_raw_common(typval_T *argvars, typval_T *rettv, പൂർണ്ണസംഖ്യ eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    പൂർണ്ണസംഖ്യ		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    പൂർണ്ണസംഖ്യ		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	തിരികെ;

    എങ്കിൽ (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    അല്ലെങ്കിൽ
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (പൂർണ്ണസംഖ്യ)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    എങ്കിൽ (channel != ശൂന്യ && eval)
    {
	എങ്കിൽ (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	അല്ലെങ്കിൽ
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, ശൂന്യ);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#എങ്കിൽ defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    പൂർണ്ണസംഖ്യ
channel_poll_setup(പൂർണ്ണസംഖ്യ nfd_in, ശൂന്യം *fds_in, പൂർണ്ണസംഖ്യ *towait)
{
    പൂർണ്ണസംഖ്യ		nfd = nfd_in;
    channel_T	*channel;
    ഘടന	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    എങ്കിൽ (ch_part->ch_fd != INVALID_FD)
	    {
		എങ്കിൽ (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    എങ്കിൽ (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		അല്ലെങ്കിൽ
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    അല്ലെങ്കിൽ
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    തിരികെ nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    പൂർണ്ണസംഖ്യ
channel_poll_check(പൂർണ്ണസംഖ്യ ret_in, ശൂന്യം *fds_in)
{
    പൂർണ്ണസംഖ്യ		ret = ret_in;
    channel_T	*channel;
    ഘടന	pollfd *fds = fds_in;
    ch_part_T	part;
    പൂർണ്ണസംഖ്യ		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    എങ്കിൽ (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	എങ്കിൽ (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_നിവേശം(channel);
	    --ret;
	}
    }

    തിരികെ ret;
}
#endif // UNIX && !HAVE_SELECT

#എങ്കിൽ !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    പൂർണ്ണസംഖ്യ
channel_select_setup(
	പൂർണ്ണസംഖ്യ maxfd_in,
	ശൂന്യം *rfds_in,
	ശൂന്യം *wfds_in,
	ഘടന timeval *tv,
	ഘടന timeval **tvp)
{
    പൂർണ്ണസംഖ്യ		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    എങ്കിൽ (fd != INVALID_FD)
	    {
		എങ്കിൽ (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    എങ്കിൽ (*tvp == ശൂന്യ || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		അല്ലെങ്കിൽ
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    എങ്കിൽ ((പൂർണ്ണസംഖ്യ)fd >= FD_SETSIZE)
			തുടരുക;
# endif
		    FD_SET((പൂർണ്ണസംഖ്യ)fd, rfds);
		    എങ്കിൽ (maxfd < (പൂർണ്ണസംഖ്യ)fd)
			maxfd = (പൂർണ്ണസംഖ്യ)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    തിരികെ maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    പൂർണ്ണസംഖ്യ
channel_select_check(പൂർണ്ണസംഖ്യ ret_in, ശൂന്യം *rfds_in, ശൂന്യം *wfds_in)
{
    പൂർണ്ണസംഖ്യ		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	വേണ്ടി (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    എങ്കിൽ (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (പൂർണ്ണസംഖ്യ)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	എങ്കിൽ (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (പൂർണ്ണസംഖ്യ)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_നിവേശം(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	എങ്കിൽ (channel->ch_job != ശൂന്യ)
	{
	    job_T *job = channel->ch_job;

	    എങ്കിൽ (job->jv_tty_out != ശൂന്യ && job->jv_status == JOB_FINISHED)
		വേണ്ടി (part = PART_SOCK; part < PART_എണ്ണം; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    തിരികെ ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    സ്ഥിതം ശൂന്യം
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    എങ്കിൽ (stdio_channel->CH_OUT_FD != INVALID_FD)
	(ശൂന്യം)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(പൂർണ്ണസംഖ്യ)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    ഘടന timeval	tv;
    ഘടന timeval	*tvp = &tv;
    പൂർണ്ണസംഖ്യ			maxfd;
    പൂർണ്ണസംഖ്യ			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ശൂന്യ, tvp);
    എങ്കിൽ (ret >= 0)
	(ശൂന്യം)channel_select_check(ret, &rfds, &wfds);
#അല്ലെങ്കിൽ
    ഘടന pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    പൂർണ്ണസംഖ്യ			nfd;
    പൂർണ്ണസംഖ്യ			towait = (പൂർണ്ണസംഖ്യ)msec;
    പൂർണ്ണസംഖ്യ			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    എങ്കിൽ (ret >= 0)
	(ശൂന്യം)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    ശൂന്യം
channel_stdio_loop(ശൂന്യം)
{
    ആകുമ്പോൾ (stdio_channel != ശൂന്യ && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	എങ്കിൽ (due_time > 0 && due_time < msec)
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
    പൂർണ്ണസംഖ്യ
channel_parse_messages(ശൂന്യം)
{
    channel_T	*channel = first_channel;
    പൂർണ്ണസംഖ്യ		ret = FALSE;
    പൂർണ്ണസംഖ്യ		r;
    ch_part_T	part = PART_SOCK;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = 0;
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
    എങ്കിൽ ((did_repeated_സന്ദേശം & REPEATED_സന്ദേശം_LOOKING) == 0)
    {
	ch_log(ശൂന്യ, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_സന്ദേശം = REPEATED_സന്ദേശം_LOOKING;
    }
    ആകുമ്പോൾ (channel != ശൂന്യ)
    {
	എങ്കിൽ (recursive == 1)
	{
	    എങ്കിൽ (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_എണ്ണം);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		തുടരുക;
	    }
	    എങ്കിൽ (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		എങ്കിൽ (channel->ch_job != ശൂന്യ)
		    channel->ch_job->jv_channel = ശൂന്യ;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		തുടരുക;
	    }
	    എങ്കിൽ (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		തുടരുക;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	എങ്കിൽ (part == PART_OUT
		&& channel->ch_job != ശൂന്യ
		&& channel->ch_job->jv_tty_out != ശൂന്യ
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    തുടരുക;
	}
#endif
	എങ്കിൽ (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    എങ്കിൽ (r == OK)
		ret = TRUE;
	    എങ്കിൽ (channel_unref(channel) || (r == OK
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
		തുടരുക;
	    }
	}
	എങ്കിൽ (part < PART_ERR)
	    ++part;
	അല്ലെങ്കിൽ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    എങ്കിൽ (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    തിരികെ ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    പൂർണ്ണസംഖ്യ
channel_any_readahead(ശൂന്യം)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    ആകുമ്പോൾ (channel != ശൂന്യ)
    {
	എങ്കിൽ (channel_has_readahead(channel, part))
	    തിരികെ TRUE;
	എങ്കിൽ (part < PART_ERR)
	    ++part;
	അല്ലെങ്കിൽ
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    തിരികെ FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_channel(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    വേണ്ടി (channel = first_channel; !abort && channel != ശൂന്യ;
						   channel = channel->ch_next)
	എങ്കിൽ (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ);
	}
    തിരികെ abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    സ്ഥിതം ch_part_T
channel_part_send(channel_T *channel)
{
    എങ്കിൽ (channel->CH_SOCK_FD == INVALID_FD)
	തിരികെ PART_IN;
    തിരികെ PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    സ്ഥിതം ch_part_T
channel_part_read(channel_T *channel)
{
    എങ്കിൽ (channel->CH_SOCK_FD == INVALID_FD)
	തിരികെ PART_OUT;
    തിരികെ PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    സ്ഥിതം ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ CH_MODE_JSON;
    തിരികെ channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    തിരികെ channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    ശൂന്യം
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_സംഖ്യ = 0;
    എങ്കിൽ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    എങ്കിൽ (channel != ശൂന്യ)
	rettv->vval.v_സംഖ്യ = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    ശൂന്യം
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    എങ്കിൽ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    എങ്കിൽ (channel != ശൂന്യ)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    ശൂന്യം
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    എങ്കിൽ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    എങ്കിൽ (channel != ശൂന്യ)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    ശൂന്യം
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_സംഖ്യ = -1;

    എങ്കിൽ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	തിരികെ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ;

    char_u	*what = tv_get_string(&argvars[1]);
    പൂർണ്ണസംഖ്യ		part;
    എങ്കിൽ (STRCMP(what, "err") == 0)
	part = PART_ERR;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "out") == 0)
	part = PART_OUT;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(what, "in") == 0)
	part = PART_IN;
    അല്ലെങ്കിൽ
	part = PART_SOCK;
    എങ്കിൽ (channel->ch_part[part].ch_bufref.br_buf != ശൂന്യ)
	rettv->vval.v_സംഖ്യ =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    ശൂന്യം
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    എങ്കിൽ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    എങ്കിൽ (channel->ch_job != ശൂന്യ)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    ശൂന്യം
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    എങ്കിൽ (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	തിരികെ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    എങ്കിൽ (channel != ശൂന്യ && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    ശൂന്യം
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    ശൂന്യം
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    ശൂന്യം
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    ശൂന്യം
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    ശൂന്യം
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    ശൂന്യം
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_വ്യഞ്ജകം_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    ശൂന്യം
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_വ്യഞ്ജകം_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    ശൂന്യം
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    ശൂന്യം
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    ശൂന്യം
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    എങ്കിൽ (check_restricted() || check_secure())
	തിരികെ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    എങ്കിൽ (channel == ശൂന്യ)
	തിരികെ;
    clear_job_options(&opt);
    എങ്കിൽ (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    ശൂന്യം
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    പൂർണ്ണസംഖ്യ		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ശൂന്യ;

    എങ്കിൽ (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	തിരികെ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    എങ്കിൽ (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	എങ്കിൽ (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    അക്ഷരം      *status = channel_status(channel, -1);

    എങ്കിൽ (channel == ശൂന്യ)
	vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN, "channel %s", status);
    അല്ലെങ്കിൽ
	vim_snprintf((അക്ഷരം *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    തിരികെ buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(പൂർണ്ണസംഖ്യ ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	എങ്കിൽ (ch->ch_id == ch_id)
	    തിരികെ ch;
    തിരികെ ശൂന്യ;
}

#endif // FEAT_JOB_CHANNEL
