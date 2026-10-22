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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_JOB_CHANNEL)

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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା sockaddr_un {
    ADDRESS_FAMILY sun_family;
    ଅକ୍ଷର sun_path[UNIX_PATH_MAX];
} SOCKADDR_UN, *PSOCKADDR_UN;
#ଅନ୍ୟଥା
# include <netdb.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <sys/socket.h>
# include <sys/un.h>
# ifdef HAVE_LIBGEN_H
#  include <libgen.h>
# endif
# define SOCK_ERRNO
# define sock_write(sd, buf, len) ଲିଖ(sd, buf, len)
# define sock_read(sd, buf, len) ପଠ(sd, buf, len)
# define sock_close(sd) close(sd)
# define fd_read(fd, buf, len) ପଠ(fd, buf, len)
# define fd_write(sd, buf, len) ଲିଖ(sd, buf, len)
# define fd_close(sd) close(sd)
#endif

ସ୍ଥିର ଶୂନ୍ୟ channel_read(channel_T *channel, ch_part_T part, ଅକ୍ଷର *func);
ସ୍ଥିର ଶୂନ୍ୟ channel_wait_any(long msec);
ସ୍ଥିର ch_mode_T channel_get_mode(channel_T *channel, ch_part_T part);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା channel_get_timeout(channel_T *channel, ch_part_T part);
ସ୍ଥିର channel_T *channel_open_stdio(ଶୂନ୍ୟ);
ସ୍ଥିର ch_part_T channel_part_send(channel_T *channel);
ସ୍ଥିର ch_part_T channel_part_read(channel_T *channel);

#define FOR_ALL_CHANNELS(ch) \
    ପାଇଁ ((ch) = first_channel; (ch) != ଖାଲି; (ch) = (ch)->ch_next)

// Whether we are inside channel_parse_messages() or another situation where it
// is safe to invoke callbacks.
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା safe_to_invoke_callback = 0;

#ifdef MSWIN
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fd_read(sock_T fd, ଅକ୍ଷର *buf, size_t len)
{
    HANDLE h = (HANDLE)fd;
    DWORD nread;

    ଯଦି (!ReadFile(h, buf, (DWORD)len, &nread, ଖାଲି))
	ଫେରନ୍ତୁ -1;
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)nread;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fd_write(sock_T fd, ଅକ୍ଷର *buf, size_t len)
{
    size_t	todo = len;
    HANDLE	h = (HANDLE)fd;
    DWORD	nwrite, size, done = 0;
    OVERLAPPED	ov;

    ଯେତେବେଳେ (todo > 0)
    {
	ଯଦି (todo > MAX_NAMED_PIPE_SIZE)
	    size = MAX_NAMED_PIPE_SIZE;
	ଅନ୍ୟଥା
	    size = (DWORD)todo;
	// If the pipe overflows while the job does not read the data,
	// WriteFile() will block forever. This abandons the write.
	CLEAR_FIELD(ov);
	nwrite = 0;
	ଯଦି (!WriteFile(h, buf + done, size, &nwrite, &ov))
	{
	    DWORD err = GetLastError();

	    ଯଦି (err != ERROR_IO_PENDING)
		ଫେରନ୍ତୁ -1;
	    ଯଦି (!GetOverlappedResult(h, &ov, &nwrite, FALSE))
		ଫେରନ୍ତୁ -1;
	    FlushFileBuffers(h);
	}
	ଅନ୍ୟଥା ଯଦି (nwrite == 0)
	    // WriteFile() returns TRUE but did not write anything. This causes
	    // a hang, so bail out.
	    ଭାଙ୍ଗ;
	todo -= nwrite;
	done += nwrite;
    }
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)done;
}

    ସ୍ଥିର ଶୂନ୍ୟ
fd_close(sock_T fd)
{
    HANDLE h = (HANDLE)fd;

    CloseHandle(h);
}
#endif

#ifdef MSWIN
# undef PERROR
# define PERROR(ସନ୍ଦେଶ) (ଶୂନ୍ୟ)semsg("%s: %s", ସନ୍ଦେଶ, strerror_win32(errno))

    ସ୍ଥିର ଅକ୍ଷର *
strerror_win32(ପୂର୍ଣ୍ଣସଂଖ୍ୟା eno)
{
    ସ୍ଥିର LPVOID msgbuf = ଖାଲି;
    char_u *ptr;

    ଯଦି (msgbuf)
    {
	LocalFree(msgbuf);
	msgbuf = ଖାଲି;
    }
    FormatMessage(
	FORMAT_MESSAGE_ALLOCATE_BUFFER |
	FORMAT_MESSAGE_FROM_SYSTEM |
	FORMAT_MESSAGE_IGNORE_INSERTS,
	ଖାଲି,
	eno,
	MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
	(LPTSTR) &msgbuf,
	0,
	ଖାଲି);
    ଯଦି (msgbuf != ଖାଲି)
	// chomp \r or \n
	ପାଇଁ (ptr = (char_u *)msgbuf; *ptr; ptr++)
	    ବାଛ (*ptr)
	    {
		ସ୍ଥିତି '\r':
		    STRMOVE(ptr, ptr + 1);
		    ptr--;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି '\n':
		    ଯଦି (*(ptr + 1) == '\0')
			*ptr = '\0';
		    ଅନ୍ୟଥା
			*ptr = ' ';
		    ଭାଙ୍ଗ;
	    }
    ଫେରନ୍ତୁ msgbuf;
}
#endif

/*
 * The list of all allocated channels.
 */
ସ୍ଥିର channel_T *first_channel = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା next_ch_id = 0;

/*
 * Allocate a new channel.  The refcount is set to 1.
 * The channel isn't actually used until it is opened.
 * Returns NULL if out of memory.
 */
    channel_T *
add_channel(ଶୂନ୍ୟ)
{
    ch_part_T	part;
    channel_T	*channel = ALLOC_CLEAR_ONE(channel_T);

    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    channel->ch_id = next_ch_id++;
    ch_log(channel, "Created channel");

    ପାଇଁ (part = PART_SOCK; part < PART_ଗଣନା; ++part)
    {
	channel->ch_part[part].ch_fd = INVALID_FD;
#ifdef FEAT_GUI_X11
	channel->ch_part[part].ch_ନିବେଶ_Handler = (Xt_ନିବେଶ_Id)ଖାଲି;
#endif
#ifdef FEAT_GUI_GTK
	channel->ch_part[part].ch_ନିବେଶ_Handler = 0;
#endif
	channel->ch_part[part].ch_timeout = 2000;
    }

    ଯଦି (first_channel != ଖାଲି)
    {
	first_channel->ch_prev = channel;
	channel->ch_next = first_channel;
    }
    first_channel = channel;

    channel->ch_refcount = 1;
    ଫେରନ୍ତୁ channel;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
has_any_channel(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ first_channel != ଖାଲି;
}

/*
 * Called when the refcount of a channel is zero.
 * Return TRUE if "channel" has a callback and the associated job wasn't
 * killed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_still_useful(channel_T *channel)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_sock_ସନ୍ଦେଶ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_out_ସନ୍ଦେଶ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_err_ସନ୍ଦେଶ;

    // If the job was killed the channel is not expected to work anymore.
    ଯଦି (channel->ch_job_killed && channel->ch_job == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    // If there is a close callback it may still need to be invoked.
    ଯଦି (channel->ch_close_cb.cb_name != ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    // If reading from or a buffer it's still useful.
    ଯଦି (channel->ch_part[PART_IN].ch_bufref.br_buf != ଖାଲି)
	ଫେରନ୍ତୁ TRUE;

    // If there is no callback then nobody can get readahead.  If the fd is
    // closed and there is no readahead then the callback won't be called.
    has_sock_ସନ୍ଦେଶ = channel->ch_part[PART_SOCK].ch_fd != INVALID_FD
		|| channel->ch_part[PART_SOCK].ch_head.rq_next != ଖାଲି
		|| channel->ch_part[PART_SOCK].ch_json_head.jq_next != ଖାଲି;
    has_out_ସନ୍ଦେଶ = channel->ch_part[PART_OUT].ch_fd != INVALID_FD
		  || channel->ch_part[PART_OUT].ch_head.rq_next != ଖାଲି
		  || channel->ch_part[PART_OUT].ch_json_head.jq_next != ଖାଲି;
    has_err_ସନ୍ଦେଶ = channel->ch_part[PART_ERR].ch_fd != INVALID_FD
		  || channel->ch_part[PART_ERR].ch_head.rq_next != ଖାଲି
		  || channel->ch_part[PART_ERR].ch_json_head.jq_next != ଖାଲି;
    ଫେରନ୍ତୁ (channel->ch_callback.cb_name != ଖାଲି && (has_sock_ସନ୍ଦେଶ
		|| has_out_ସନ୍ଦେଶ || has_err_ସନ୍ଦେଶ))
	    || ((channel->ch_part[PART_OUT].ch_callback.cb_name != ଖାଲି
		       || channel->ch_part[PART_OUT].ch_bufref.br_buf != ଖାଲି)
		    && has_out_ସନ୍ଦେଶ)
	    || ((channel->ch_part[PART_ERR].ch_callback.cb_name != ଖାଲି
		       || channel->ch_part[PART_ERR].ch_bufref.br_buf != ଖାଲି)
		    && has_err_ସନ୍ଦେଶ);
}

/*
 * Return TRUE if "channel" is closeable (i.e. all readable fds are closed).
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_can_close(channel_T *channel)
{
    ଫେରନ୍ତୁ channel->ch_to_be_closed == 0;
}

/*
 * Close a channel and free all its resources.
 * The "channel" pointer remains valid.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_free_contents(channel_T *channel)
{
    channel_close(channel, TRUE);
    channel_clear(channel);
    ch_log(channel, "Freeing channel");
}

/*
 * Unlink "channel" from the list of channels and free it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_free_channel(channel_T *channel)
{
    ଯଦି (channel->ch_next != ଖାଲି)
	channel->ch_next->ch_prev = channel->ch_prev;
    ଯଦି (channel->ch_prev == ଖାଲି)
	first_channel = channel->ch_next;
    ଅନ୍ୟଥା
	channel->ch_prev->ch_next = channel->ch_next;
    vim_free(channel);
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_free(channel_T *channel)
{
    ଯଦି (in_free_unref_ବସ୍ତୁଗୁଡ଼ିକ)
	ଫେରନ୍ତୁ;

    ଯଦି (safe_to_invoke_callback == 0)
	channel->ch_to_be_freed = TRUE;
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_may_free(channel_T *channel)
{
    ଯଦି (!channel_still_useful(channel))
    {
	channel_free(channel);
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Decrement the reference count on "channel" and maybe free it when it goes
 * down to zero.  Don't free it if there is a pending action.
 * Returns TRUE when the channel is no longer referenced.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_unref(channel_T *channel)
{
    ଯଦି (channel != ଖାଲି && --channel->ch_refcount <= 0)
	ଫେରନ୍ତୁ channel_may_free(channel);
    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
free_unused_channels_contents(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;
    channel_T	*ch;

    // This is invoked from the garbage collector, which only runs at a safe
    // point.
    ++safe_to_invoke_callback;

    FOR_ALL_CHANNELS(ch)
	ଯଦି (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	{
	    // Free the channel and ordinary items it contains, but don't
	    // recurse into Lists, Dictionaries etc.
	    channel_free_contents(ch);
	    did_free = TRUE;
	}

    --safe_to_invoke_callback;
    ଫେରନ୍ତୁ did_free;
}

    ଶୂନ୍ୟ
free_unused_channels(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask)
{
    channel_T	*ch;
    channel_T	*ch_next;

    ପାଇଁ (ch = first_channel; ch != ଖାଲି; ch = ch_next)
    {
	ch_next = ch->ch_next;
	ଯଦି (!channel_still_useful(ch)
				 && (ch->ch_copyID & mask) != (copyID & mask))
	    // Free the channel struct itself.
	    channel_free_channel(ch);
    }
}

#ଯଦି defined(FEAT_GUI)

# ଯଦି defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
/*
 * Lookup the channel from the socket.  Set "partp" to the fd index.
 * Returns NULL when the socket isn't found.
 */
    ସ୍ଥିର channel_T *
channel_fd2channel(sock_T fd, ch_part_T *partp)
{
    channel_T	*channel;
    ch_part_T	part;

    ଯଦି (fd == INVALID_FD)
	ଫେରନ୍ତୁ ଖାଲି;

    FOR_ALL_CHANNELS(channel)
    {
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	    ଯଦି (channel->ch_part[part].ch_fd == fd)
	    {
		*partp = part;
		ଫେରନ୍ତୁ channel;
	    }
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_read_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
    channel_T	*channel;
    ch_part_T	part;

    channel = channel_fd2channel(fd, &part);
    ଯଦି (channel == ଖାଲି)
	ch_error(ଖାଲି, "Channel for fd %d not found", fd);
    ଅନ୍ୟଥା
	channel_read(channel, part, "channel_read_fd");
}
# endif

/*
 * Read a command from netbeans.
 */
# ifdef FEAT_GUI_X11
    ସ୍ଥିର ଶୂନ୍ୟ
messageFromServerX11(XtPointer clientData,
		  ପୂର୍ଣ୍ଣସଂଖ୍ୟା *unused1 UNUSED,
		  Xt_ନିବେଶ_Id *unused2 UNUSED)
{
    channel_read_fd((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)clientData);
}
# endif

# ifdef FEAT_GUI_GTK
#  ଯଦି GTK_CHECK_VERSION(3,0,0)
    ସ୍ଥିର gboolean
messageFromServerGtk3(GIOChannel *unused1 UNUSED,
		  GIOCondition unused2 UNUSED,
		  gpointer clientData)
{
    channel_read_fd(GPOINTER_TO_INT(clientData));
    ଫେରନ୍ତୁ TRUE; // Return FALSE instead in case the event source is to
		 // be removed after this function returns.
}
#  ଅନ୍ୟଥା
    ସ୍ଥିର ଶୂନ୍ୟ
messageFromServerGtk2(gpointer clientData,
		  gint unused1 UNUSED,
		  Gdk_ନିବେଶ_Condition unused2 UNUSED)
{
    channel_read_fd((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(long)clientData);
}
#  endif
# endif

    ସ୍ଥିର ଶୂନ୍ୟ
channel_gui_register_one(channel_T *channel, ch_part_T part UNUSED)
{
    ଯଦି (!CH_HAS_GUI)
	ଫେରନ୍ତୁ;

    // gets stuck in handling events for a not connected channel
    ଯଦି (channel->ch_keep_open)
	ଫେରନ୍ତୁ;

# ifdef FEAT_GUI_X11
    // Tell notifier we are interested in being called when there is input on
    // the editor connection socket.
    ଯଦି (channel->ch_part[part].ch_ନିବେଶ_Handler == (Xt_ନିବେଶ_Id)ଖାଲି)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ନିବେଶ_Handler = Xt_App_Add_ନିବେଶ(
		(XtAppContext)app_context,
		channel->ch_part[part].ch_fd,
		(XtPointer)(Xt_ନିବେଶ_Read_Mask + Xt_ନିବେଶ_Except_Mask),
		messageFromServerX11,
		(XtPointer)(long)channel->ch_part[part].ch_fd);
    }
# ଅନ୍ୟଥା
#  ifdef FEAT_GUI_GTK
    // Tell gdk we are interested in being called when there is input on the
    // editor connection socket.
    ଯଦି (channel->ch_part[part].ch_ନିବେଶ_Handler == 0)
    {
	ch_log(channel, "Registering part %s with fd %d",
		ch_part_names[part], channel->ch_part[part].ch_fd);
#   ଯଦି GTK_CHECK_VERSION(3,0,0)
	GIOChannel *chnnl = g_io_channel_unix_new(
		(gint)channel->ch_part[part].ch_fd);

	channel->ch_part[part].ch_ନିବେଶ_Handler = g_io_add_watch(
		chnnl,
		G_IO_IN|G_IO_HUP|G_IO_ERR|G_IO_PRI,
		messageFromServerGtk3,
		GINT_TO_POINTER(channel->ch_part[part].ch_fd));

	g_io_channel_unref(chnnl);
#   ଅନ୍ୟଥା
	channel->ch_part[part].ch_ନିବେଶ_Handler = gdk_ନିବେଶ_add(
		(gint)channel->ch_part[part].ch_fd,
		(Gdk_ନିବେଶ_Condition)
			     ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_ନିବେଶ_READ + (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)GDK_ନିବେଶ_EXCEPTION),
		messageFromServerGtk2,
		(gpointer)(long)channel->ch_part[part].ch_fd);
#   endif
    }
#  endif
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_gui_register(channel_T *channel)
{
    ଯଦି (channel->CH_SOCK_FD != INVALID_FD)
	channel_gui_register_one(channel, PART_SOCK);
    ଯଦି (channel->CH_OUT_FD != INVALID_FD
	    && channel->CH_OUT_FD != channel->CH_SOCK_FD)
	channel_gui_register_one(channel, PART_OUT);
    ଯଦି (channel->CH_ERR_FD != INVALID_FD
	    && channel->CH_ERR_FD != channel->CH_SOCK_FD
	    && channel->CH_ERR_FD != channel->CH_OUT_FD)
	channel_gui_register_one(channel, PART_ERR);
}

/*
 * Register any of our file descriptors with the GUI event handling system.
 * Called when the GUI has started.
 */
    ଶୂନ୍ୟ
channel_gui_register_all(ଶୂନ୍ୟ)
{
    channel_T *channel;

    FOR_ALL_CHANNELS(channel)
	channel_gui_register(channel);
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_gui_unregister_one(channel_T *channel UNUSED, ch_part_T part UNUSED)
{
# ifdef FEAT_GUI_X11
    ଯଦି (channel->ch_part[part].ch_ନିବେଶ_Handler != (Xt_ନିବେଶ_Id)ଖାଲି)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
	Xt_Remove_ନିବେଶ(channel->ch_part[part].ch_ନିବେଶ_Handler);
	channel->ch_part[part].ch_ନିବେଶ_Handler = (Xt_ନିବେଶ_Id)ଖାଲି;
    }
# ଅନ୍ୟଥା
#  ifdef FEAT_GUI_GTK
    ଯଦି (channel->ch_part[part].ch_ନିବେଶ_Handler != 0)
    {
	ch_log(channel, "Unregistering part %s", ch_part_names[part]);
#   ଯଦି GTK_CHECK_VERSION(3,0,0)
	g_source_remove(channel->ch_part[part].ch_ନିବେଶ_Handler);
#   ଅନ୍ୟଥା
	gdk_ନିବେଶ_remove(channel->ch_part[part].ch_ନିବେଶ_Handler);
#   endif
	channel->ch_part[part].ch_ନିବେଶ_Handler = 0;
    }
#  endif
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_gui_unregister(channel_T *channel)
{
    ch_part_T	part;

    ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	channel_gui_unregister_one(channel, part);
}

#endif  // FEAT_GUI

/*
 * For Unix we need to call connect() again after connect() failed.
 * On Win32 one time is sufficient.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_connect(
	channel_T *channel,
	ଅଚଳ ସଂରଚନା sockaddr *server_addr,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା server_addrlen,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା *waittime)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sd = -1;
#ifdef MSWIN
    u_long	val = 1;
#endif

    ଯେତେବେଳେ (TRUE)
    {
	long	elapsed_msec = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	waitnow;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ret;

	ଯଦି (sd >= 0)
	    sock_close(sd);
	sd = socket(server_addr->sa_family, SOCK_STREAM, 0);
	ଯଦି (sd == -1)
	{
	    ch_error(channel, "in socket() in channel_connect().");
	    PERROR(_(e_socket_in_channel_connect));
	    ଫେରନ୍ତୁ -1;
	}

	ଯଦି (*waittime >= 0)
	{
	    // Make connect() non-blocking.
	    ଯଦି (
#ifdef MSWIN
		ioctlsocket(sd, FIONBIO, &val) < 0
#ଅନ୍ୟଥା
		fcntl(sd, F_SETFL, O_NONBLOCK) < 0
#endif
	       )
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		sock_close(sd);
		ଫେରନ୍ତୁ -1;
	    }
	}

	// Try connecting to the server.
	ch_log(channel, "Connecting...");

	ret = connect(sd, server_addr, server_addrlen);
	ଯଦି (ret == 0)
	    // The connection could be established.
	    ଭାଙ୍ଗ;

	SOCK_ERRNO;
	ଯଦି (*waittime < 0 || (errno != EWOULDBLOCK
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
	    ଫେରନ୍ତୁ -1;
	}
	ଅନ୍ୟଥା ଯଦି (errno == ECONNREFUSED)
	{
	    ch_error(channel, "channel_connect: Connection refused");
	    sock_close(sd);
	    ଫେରନ୍ତୁ -1;
	}

	// Limit the waittime to 50 msec.  If it doesn't work within this
	// time we close the socket and try creating it again.
	waitnow = *waittime > 50 ? 50 : *waittime;

	// If connect() didn't finish then try using select() to wait for the
	// connection to be made. For Win32 always use select() to wait.
	{
	    ସଂରଚନା timeval	tv;
	    fd_set		rfds;
	    fd_set		wfds;
#ifndef MSWIN
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			so_error = 0;
	    socklen_t		so_error_len = ଆକାର(so_error);
	    ସଂରଚନା timeval	start_tv;
	    ସଂରଚନା timeval	end_tv;
#endif
	    FD_ZERO(&rfds);
	    FD_SET(sd, &rfds);
	    FD_ZERO(&wfds);
	    FD_SET(sd, &wfds);

	    tv.tv_sec = waitnow / 1000;
	    tv.tv_usec = (waitnow % 1000) * 1000;
#ifndef MSWIN
	    gettimeofday(&start_tv, ଖାଲି);
#endif
	    ch_log(channel,
		      "Waiting for connection (waiting %d msec)...", waitnow);

	    ret = select(sd + 1, &rfds, &wfds, ଖାଲି, &tv);
	    ଯଦି (ret < 0)
	    {
		SOCK_ERRNO;
		ch_error(channel,
		      "channel_connect: Connect failed with errno %d", errno);
		PERROR(_(e_cannot_connect_to_port));
		sock_close(sd);
		ଫେରନ୍ତୁ -1;
	    }

#ifdef MSWIN
	    // On Win32: select() is expected to work and wait for up to
	    // "waitnow" msec for the socket to be open.
	    ଯଦି (FD_ISSET(sd, &wfds))
		ଭାଙ୍ଗ;
	    elapsed_msec = waitnow;
	    ଯଦି (*waittime > 1 && elapsed_msec < *waittime)
	    {
		*waittime -= elapsed_msec;
		ଜାରି;
	    }
#ଅନ୍ୟଥା
	    // On Linux-like systems: See socket(7) for the behavior
	    // After putting the socket in non-blocking mode, connect() will
	    // return EINPROGRESS, select() will not wait (as if writing is
	    // possible), need to use getsockopt() to check if the socket is
	    // actually able to connect.
	    // We detect a failure to connect when either read and write fds
	    // are set.  Use getsockopt() to find out what kind of failure.
	    ଯଦି (FD_ISSET(sd, &rfds) || FD_ISSET(sd, &wfds))
	    {
		ret = getsockopt(sd,
			      SOL_SOCKET, SO_ERROR, &so_error, &so_error_len);
		ଯଦି (ret < 0 || (so_error != 0
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
		    ଫେରନ୍ତୁ -1;
		}
		ଅନ୍ୟଥା ଯଦି (errno == ECONNREFUSED)
		{
		    ch_error(channel, "channel_connect: Connection refused");
		    sock_close(sd);
		    ଫେରନ୍ତୁ -1;
		}
	    }

	    ଯଦି (FD_ISSET(sd, &wfds) && so_error == 0)
		// Did not detect an error, connection is established.
		ଭାଙ୍ଗ;

	    gettimeofday(&end_tv, ଖାଲି);
	    elapsed_msec = (end_tv.tv_sec - start_tv.tv_sec) * 1000
				 + (end_tv.tv_usec - start_tv.tv_usec) / 1000;
#endif
	}

#ifndef MSWIN
	ଯଦି (*waittime > 1 && elapsed_msec < *waittime)
	{
	    // The port isn't ready but we also didn't get an error.
	    // This happens when the server didn't open the socket
	    // yet.  Select() may return early, wait until the remaining
	    // "waitnow"  and try again.
	    waitnow -= elapsed_msec;
	    *waittime -= elapsed_msec;
	    ଯଦି (waitnow > 0)
	    {
		mch_delay((long)waitnow, MCH_DELAY_IGNOREINPUT);
		ui_breakcheck();
		*waittime -= waitnow;
	    }
	    ଯଦି (!got_int)
	    {
		ଯଦି (*waittime <= 0)
		    // give it one more try
		    *waittime = 1;
		ଜାରି;
	    }
	    // we were interrupted, behave as if timed out
	}
#endif

	// We timed out.
	ch_error(channel, "Connection timed out");
	sock_close(sd);
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (*waittime >= 0)
    {
#ifdef MSWIN
	val = 0;
	ioctlsocket(sd, FIONBIO, &val);
#ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)fcntl(sd, F_SETFL, 0);
#endif
    }

    ଫେରନ୍ତୁ sd;
}

/*
 * Open a socket channel to the UNIX socket at "path".
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_open_unix(
	ଅଚଳ ଅକ୍ଷର *path,
	ଶୂନ୍ୟ (*nb_close_cb)(ଶୂନ୍ୟ))
{
    channel_T		*channel = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			sd = -1;
    size_t		path_len = STRLEN(path);
    ସଂରଚନା sockaddr_un	server;
    size_t		server_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			waittime = -1;

    ଯଦି (*path == NUL || path_len >= ଆକାର(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ଫେରନ୍ତୁ ଖାଲି;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
    {
	ch_error(ଖାଲି, "Cannot allocate channel.");
	ଫେରନ୍ତୁ ଖାଲି;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, ଆକାର(server.sun_path) - 1);

    ch_log(channel, "Trying to connect to %s", path);

    server_len = offsetof(ସଂରଚନା sockaddr_un, sun_path) + path_len + 1;
    sd = channel_connect(channel, (ସଂରଚନା sockaddr *)&server, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)server_len,
								   &waittime);

    ଯଦି (sd < 0)
    {
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ଅକ୍ଷର *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ଫେରନ୍ତୁ channel;
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
	ଅଚଳ ଅକ୍ଷର *hostname,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା port,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା waittime,
	ଶୂନ୍ୟ (*nb_close_cb)(ଶୂନ୍ୟ))
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			sd = -1;
    channel_T		*channel = ଖାଲି;
#ifdef FEAT_IPV6
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			err;
    ସଂରଚନା addrinfo	hints;
    ସଂରଚନା addrinfo	*res = ଖାଲି;
    ସଂରଚନା addrinfo	*addr = ଖାଲି;
#ଅନ୍ୟଥା
    ସଂରଚନା sockaddr_in	server;
    ସଂରଚନା hostent	*host = ଖାଲି;
#endif

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
    {
	ch_error(ଖାଲି, "Cannot allocate channel.");
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Get the server internet address and put into addr structure fill in the
    // socket address structure and connect to server.
#ifdef FEAT_IPV6
    CLEAR_FIELD(hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
# ଯଦି defined(__ANDROID__)
    hints.ai_flags = AI_ADDRCONFIG;
# elif defined(AI_ADDRCONFIG) && defined(AI_V4MAPPED)
    hints.ai_flags = AI_ADDRCONFIG | AI_V4MAPPED;
# endif
    // Set port number manually in order to prevent name resolution services
    // from being invoked in the environment where AI_NUMERICSERV is not
    // defined.
    ଯଦି ((err = getaddrinfo(hostname, ଖାଲି, &hints, &res)) != 0)
    {
	ch_error(channel, "in getaddrinfo() in channel_open()");
	semsg(_(e_getaddrinfo_in_channel_open_str), gai_strerror(err));
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Count the number of addresses for timeout distribution
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା addr_ଗଣନା = 0;
    ପାଇଁ (addr = res; addr != ଖାଲି; addr = addr->ai_next)
	addr_ଗଣନା++;

    // On Mac and Solaris a zero timeout almost never works.  Waiting for
    // one millisecond already helps a lot.  Later Mac systems (using IPv6)
    // need more time, 15 milliseconds appears to work well.
    // Let's do it for all systems, because we don't know why this is
    // needed.
    ଯଦି (waittime == 0)
	waittime = 15;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା addr_index = 0;
    ପାଇଁ (addr = res; addr != ଖାଲି; addr = addr->ai_next, addr_index++)
    {
	ଅଚଳ ଅକ୍ଷର  *dst = hostname;
# ifdef HAVE_INET_NTOP
	ଅଚଳ ଶୂନ୍ୟ  *src = ଖାଲି;
	ଅକ୍ଷର	    buf[NUMBUFLEN];
# endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    try_waittime;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    before_waittime;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    consumed;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    remaining_addrs;

	ଯଦି (addr->ai_family == AF_INET6)
	{
	    ସଂରଚନା sockaddr_in6 *sai = (ସଂରଚନା sockaddr_in6 *)addr->ai_addr;

	    sai->sin6_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin6_addr;
# endif
	}
	ଅନ୍ୟଥା ଯଦି (addr->ai_family == AF_INET)
	{
	    ସଂରଚନା sockaddr_in *sai = (ସଂରଚନା sockaddr_in *)addr->ai_addr;

	    sai->sin_port = htons(port);
# ifdef HAVE_INET_NTOP
	    src = &sai->sin_addr;
# endif
	}
# ifdef HAVE_INET_NTOP
	ଯଦି (src != ଖାଲି)
	{
	    dst = inet_ntop(addr->ai_family, src, buf, ଆକାର(buf));
	    ଯଦି (dst == ଖାଲି)
		dst = hostname;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(hostname, dst) != 0)
		ch_log(channel, "Resolved %s to %s", hostname, dst);
	}
# endif

	ch_log(channel, "Trying to connect to %s port %d", dst, port);

	// Distribute the timeout across addresses for better fallback behavior.
	// This implements a simplified version of Happy Eyeballs (RFC 8305).
	ଯଦି (addr->ai_next == ଖାଲି)
	    try_waittime = waittime;
	ଅନ୍ୟଥା ଯଦି (addr_index == 0)
	{
	    ଯଦି (waittime > 500)
		try_waittime = 250;
	    ଅନ୍ୟଥା ଯଦି (waittime > 30)
		try_waittime = waittime / 2;
	    ଅନ୍ୟଥା
		try_waittime = waittime;
	}
	ଅନ୍ୟଥା
	{
	    remaining_addrs = addr_ଗଣନା - addr_index;
	    try_waittime = waittime / remaining_addrs;
	}

	before_waittime = try_waittime;
	sd = channel_connect(channel, addr->ai_addr, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)addr->ai_addrlen,
							   &try_waittime);

	// Update the overall waittime based on consumed time
	consumed = before_waittime - try_waittime;
	waittime -= consumed;
	ଯଦି (waittime < 0)
	    waittime = 0;

	ଯଦି (sd >= 0)
	    ଭାଙ୍ଗ;

	// If we have no time left, stop trying
	ଯଦି (waittime <= 0 && addr->ai_next != ଖାଲି)
	{
	    ch_log(channel, "Out of time, stopping connection attempts");
	    ଭାଙ୍ଗ;
	}
    }

    freeaddrinfo(res);
#ଅନ୍ୟଥା
    CLEAR_FIELD(server);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    ଯଦି ((host = gethostbyname(hostname)) == ଖାଲି)
    {
	ch_error(channel, "in gethostbyname() in channel_open()");
	PERROR(_(e_gethostbyname_in_channel_open));
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    {
	ଅକ୍ଷର *p;

	// When using host->h_addr_list[0] directly ubsan warns for it to not
	// be aligned.  First copy the pointer to avoid that.
	memcpy(&p, &host->h_addr_list[0], ଆକାର(p));
	memcpy((ଅକ୍ଷର *)&server.sin_addr, p, host->h_length);
    }

    ch_log(channel, "Trying to connect to %s port %d", hostname, port);

    // On Mac and Solaris a zero timeout almost never works.  At least wait one
    // millisecond.  Let's do it for all systems, because we don't know why
    // this is needed.
    ଯଦି (waittime == 0)
	waittime = 1;

    sd = channel_connect(channel, (ସଂରଚନା sockaddr *)&server, ଆକାର(server),
								   &waittime);
#endif

    ଯଦି (sd < 0)
    {
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ch_log(channel, "Connection made");

    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ଅକ୍ଷର *)vim_strsave((char_u *)hostname);
    channel->ch_port = port;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ଫେରନ୍ତୁ channel;
}

    ସ୍ଥିର ଶୂନ୍ୟ
free_set_callback(callback_T *cbp, callback_T *callback)
{
    free_callback(cbp);

    ଯଦି (callback->cb_name != ଖାଲି && *callback->cb_name != NUL)
	copy_callback(cbp, callback);
    ଅନ୍ୟଥା
	cbp->cb_name = ଖାଲି;
}

/*
 * Prepare buffer "buf" for writing channel output to.
 */
	ସ୍ଥିର ଶୂନ୍ୟ
prepare_buffer(buf_T *buf)
{
    buf_T *save_curbuf = curbuf;

    buf_copy_options(buf, BCO_ENTER);
    curbuf = buf;
#ifdef FEAT_QUICKFIX
    set_option_ମୂଲ୍ୟ_give_err((char_u *)"bt",
					    0L, (char_u *)"nofile", OPT_LOCAL);
    set_option_ମୂଲ୍ୟ_give_err((char_u *)"bh", 0L, (char_u *)"hide", OPT_LOCAL);
#endif
    ଯଦି (curbuf->b_ml.ml_mfp == ଖାଲି)
	ml_open(curbuf);
    curbuf = save_curbuf;
}

/*
 * Find a buffer matching "name" or create a new one.
 * Returns NULL if there is something very wrong (error already reported).
 */
    ସ୍ଥିର buf_T *
channel_find_buffer(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା err, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ)
{
    buf_T *buf = ଖାଲି;
    buf_T *save_curbuf = curbuf;

    ଯଦି (name != ଖାଲି && *name != NUL)
    {
	buf = buflist_findname(name);
	ଯଦି (buf == ଖାଲି)
	    buf = buflist_findname_exp(name);
    }

    ଯଦି (buf != ଖାଲି)
	ଫେରନ୍ତୁ buf;

    buf = buflist_new(name == ଖାଲି || *name == NUL ? ଖାଲି : name,
	    ଖାଲି, (linenr_T)0, BLN_LISTED | BLN_NEW);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    prepare_buffer(buf);

    curbuf = buf;
    ଯଦି (ସନ୍ଦେଶ)
	ml_replace(1, (char_u *)(err ? "Reading from channel error..."
		    : "Reading from channel output..."), TRUE);
    changed_bytes(1, 0);
    curbuf = save_curbuf;

    ଫେରନ୍ତୁ buf;
}

/*
 * Set various properties from an "opt" argument.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_set_options(channel_T *channel, jobopt_T *opt)
{
    ch_part_T	part;

    ଯଦି (opt->jo_set & JO_MODE)
	ପାଇଁ (part = PART_SOCK; part < PART_ଗଣନା; ++part)
	    channel->ch_part[part].ch_mode = opt->jo_mode;
    ଯଦି (opt->jo_set & JO_IN_MODE)
	channel->ch_part[PART_IN].ch_mode = opt->jo_in_mode;
    ଯଦି (opt->jo_set & JO_OUT_MODE)
	channel->ch_part[PART_OUT].ch_mode = opt->jo_out_mode;
    ଯଦି (opt->jo_set & JO_ERR_MODE)
	channel->ch_part[PART_ERR].ch_mode = opt->jo_err_mode;
    channel->ch_nonblock = opt->jo_noblock;

    ଯଦି (opt->jo_set & JO_TIMEOUT)
	ପାଇଁ (part = PART_SOCK; part < PART_ଗଣନା; ++part)
	    channel->ch_part[part].ch_timeout = opt->jo_timeout;
    ଯଦି (opt->jo_set & JO_OUT_TIMEOUT)
	channel->ch_part[PART_OUT].ch_timeout = opt->jo_out_timeout;
    ଯଦି (opt->jo_set & JO_ERR_TIMEOUT)
	channel->ch_part[PART_ERR].ch_timeout = opt->jo_err_timeout;
    ଯଦି (opt->jo_set & JO_BLOCK_WRITE)
	channel->ch_part[PART_IN].ch_block_write = 1;

    ଯଦି (opt->jo_set & JO_CALLBACK)
	free_set_callback(&channel->ch_callback, &opt->jo_callback);
    ଯଦି (opt->jo_set & JO_OUT_CALLBACK)
	free_set_callback(&channel->ch_part[PART_OUT].ch_callback,
							      &opt->jo_out_cb);
    ଯଦି (opt->jo_set & JO_ERR_CALLBACK)
	free_set_callback(&channel->ch_part[PART_ERR].ch_callback,
							      &opt->jo_err_cb);
    ଯଦି (opt->jo_set & JO_CLOSE_CALLBACK)
	free_set_callback(&channel->ch_close_cb, &opt->jo_close_cb);
    channel->ch_drop_never = opt->jo_drop_never;

    ଯଦି ((opt->jo_set & JO_OUT_IO) && opt->jo_io[PART_OUT] == JIO_BUFFER)
    {
	buf_T *buf;

	// writing output to a buffer. Default mode is NL.
	ଯଦି (!(opt->jo_set & JO_OUT_MODE))
	    channel->ch_part[PART_OUT].ch_mode = CH_MODE_NL;
	ଯଦି (opt->jo_set & JO_OUT_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_OUT]);
	    ଯଦି (buf == ଖାଲି)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_OUT]);
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ = TRUE;

	    ଯଦି (opt->jo_set2 & JO2_OUT_ସନ୍ଦେଶ)
		ସନ୍ଦେଶ = opt->jo_message[PART_OUT];
	    buf = channel_find_buffer(opt->jo_io_name[PART_OUT], FALSE, ସନ୍ଦେଶ);
	}
	ଯଦି (buf != ଖାଲି)
	{
	    ଯଦି (opt->jo_set & JO_OUT_MODIFIABLE)
		channel->ch_part[PART_OUT].ch_nomodifiable =
						!opt->jo_modifiable[PART_OUT];

	    ଯଦି (!buf->b_p_ma && !channel->ch_part[PART_OUT].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ଅନ୍ୟଥା
	    {
		ch_log(channel, "writing out to buffer '%s'",
						       (ଅକ୍ଷର *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_OUT].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
		    prepare_buffer(buf);
	    }
	}
    }

    ଯଦି ((opt->jo_set & JO_ERR_IO) && (opt->jo_io[PART_ERR] == JIO_BUFFER
	 || (opt->jo_io[PART_ERR] == JIO_OUT && (opt->jo_set & JO_OUT_IO)
				       && opt->jo_io[PART_OUT] == JIO_BUFFER)))
    {
	buf_T *buf;

	// writing err to a buffer. Default mode is NL.
	ଯଦି (!(opt->jo_set & JO_ERR_MODE))
	    channel->ch_part[PART_ERR].ch_mode = CH_MODE_NL;
	ଯଦି (opt->jo_io[PART_ERR] == JIO_OUT)
	    buf = channel->ch_part[PART_OUT].ch_bufref.br_buf;
	ଅନ୍ୟଥା ଯଦି (opt->jo_set & JO_ERR_BUF)
	{
	    buf = buflist_findnr(opt->jo_io_buf[PART_ERR]);
	    ଯଦି (buf == ଖାଲି)
		semsg(_(e_buffer_nr_does_not_exist),
					       (long)opt->jo_io_buf[PART_ERR]);
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସନ୍ଦେଶ = TRUE;

	    ଯଦି (opt->jo_set2 & JO2_ERR_ସନ୍ଦେଶ)
		ସନ୍ଦେଶ = opt->jo_message[PART_ERR];
	    buf = channel_find_buffer(opt->jo_io_name[PART_ERR], TRUE, ସନ୍ଦେଶ);
	}
	ଯଦି (buf != ଖାଲି)
	{
	    ଯଦି (opt->jo_set & JO_ERR_MODIFIABLE)
		channel->ch_part[PART_ERR].ch_nomodifiable =
						!opt->jo_modifiable[PART_ERR];
	    ଯଦି (!buf->b_p_ma && !channel->ch_part[PART_ERR].ch_nomodifiable)
	    {
		emsg(_(e_cannot_make_changes_modifiable_is_off));
	    }
	    ଅନ୍ୟଥା
	    {
		ch_log(channel, "writing err to buffer '%s'",
						       (ଅକ୍ଷର *)buf->b_ffname);
		set_bufref(&channel->ch_part[PART_ERR].ch_bufref, buf);
		// if the buffer was deleted or unloaded resurrect it
		ଯଦି (buf->b_ml.ml_mfp == ଖାଲି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_parse_socketserver_address(
	char_u	*address,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	*port,
	char_u	**unix_path,
	bool	quiet)
{
    ଅକ୍ଷର    *rest;
    long    val;

    ଯଦି (*address == NUL)
	ଯାଅ fail;

    ଯଦି (STRNCMP(address, "unix:", 5) == 0)
    {
	*unix_path = vim_strsave(address + 5);
	ଫେରନ୍ତୁ OK;
    }

    val = strtol((ଅକ୍ଷର *)(address), &rest, 10);
    ଯଦି (val < 0 || val >= 65536 || *rest != NUL)
    {
	ଯଦି (!quiet)
	    semsg(_(e_invalid_argument_str), address);
	ଫେରନ୍ତୁ FAIL;
    }
    *port = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val;

    ଫେରନ୍ତୁ OK;
fail:
    ଯଦି (!quiet)
	semsg(_(e_invalid_argument_str), address);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Implements ch_open().
 */
    ସ୍ଥିର channel_T *
channel_open_func(typval_T *argvars)
{
    char_u	*address;
    char_u	*p;
    ଅକ୍ଷର	*rest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		port = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_ipv6 = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_unix = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_stdio = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_string_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ ଖାଲି;

    address = tv_get_string(&argvars[0]);
    ଯଦି (argvars[1].v_type != VAR_UNKNOWN
	    && check_for_nonnull_dict_arg(argvars, 1) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (*address == NUL)
    {
	semsg(_(e_invalid_argument_str), address);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (STRCMP(address, "stdio") == 0)
	is_stdio = TRUE;
    ଅନ୍ୟଥା ଯଦି (STRNCMP(address, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	address += 5;
    }
    ଅନ୍ୟଥା ଯଦି (*address == '[')
    {
	// ipv6 address
	is_ipv6 = TRUE;
	p = vim_strchr(address + 1, ']');
	ଯଦି (p == ଖାଲି || *++p != ':')
	{
	    semsg(_(e_invalid_argument_str), address);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ଅନ୍ୟଥା
    {
	// ipv4 address
	p = vim_strchr(address, ':');
	ଯଦି (p == ଖାଲି)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    ଯଦି (!is_unix && !is_stdio)
    {
	port = strtol((ଅକ୍ଷର *)(p + 1), &rest, 10);
	ଯଦି (port <= 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), address);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (is_ipv6)
	{
	    // strip '[' and ']'
	    ++address;
	    *(p - 1) = NUL;
	}
	ଅନ୍ୟଥା
	    *p = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ଯଦି (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL
		+ (is_unix || is_stdio ? 0 : JO_WAITTIME), 0) == FAIL)
	ଯାଅ theend;
    ଯଦି (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	ଯାଅ theend;
    }

    ଯଦି (is_stdio)
	channel = channel_open_stdio();
    ଅନ୍ୟଥା ଯଦି (is_unix)
	channel = channel_open_unix((ଅକ୍ଷର *)address, ଖାଲି);
    ଅନ୍ୟଥା
	channel = channel_open((ଅକ୍ଷର *)address, port, opt.jo_waittime, ଖାଲି);
    ଯଦି (channel != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	user_set = opt.jo_set;

	opt.jo_set = JO_ALL;
	ଯଦି (is_stdio)
	    // "mode" and "timeout" apply to both reading and writing, unless
	    // given for a part specifically.
	    opt.jo_set &= ~((JO_IN_MODE | JO_OUT_MODE | JO_ERR_MODE
				| JO_OUT_TIMEOUT | JO_ERR_TIMEOUT) & ~user_set);
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ଫେରନ୍ତୁ channel;
}

// The channel on the stdin and stdout of Vim, see --stdio-channel.
ସ୍ଥିର channel_T *stdio_channel = ଖାଲି;

/*
 * Open a channel that reads from the stdin of Vim and writes to its stdout.
 * Only when started with --stdio-channel, and only once.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    ସ୍ଥିର channel_T *
channel_open_stdio(ଶୂନ୍ୟ)
{
    channel_T	*channel;
    sock_T	in_fd;		// our stdin, the channel reads from it
    sock_T	out_fd;		// our stdout, the channel writes to it

    ଯଦି (!use_stdio_channel)
    {
	emsg(_(e_not_started_with_stdio_channel));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (stdio_channel != ଖାଲି)
    {
	emsg(_(e_cannot_open_stdio_channel));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Point the original stdin and stdout at the null device, so that neither
    // Vim nor the commands it starts can use the channel.
#ifdef MSWIN
    {
	HANDLE	proc = GetCurrentProcess();
	HANDLE	hin;
	HANDLE	hout;
	HANDLE	hnul;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nul_fd;

	ଯଦି (!DuplicateHandle(proc, GetStdHandle(STD_ନିବେଶ_HANDLE), proc, &hin,
					      0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    emsg(_(e_cannot_open_stdio_channel));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	ଯଦି (!DuplicateHandle(proc, GetStdHandle(STD_OUTPUT_HANDLE), proc,
				       &hout, 0, FALSE, DUPLICATE_SAME_ACCESS))
	{
	    CloseHandle(hin);
	    emsg(_(e_cannot_open_stdio_channel));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	hnul = CreateFile("NUL", GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, ଖାଲି, OPEN_EXISTING, 0,
		ଖାଲି);
	ଯଦି (hnul != INVALID_HANDLE_ମୂଲ୍ୟ)
	{
	    SetStdHandle(STD_ନିବେଶ_HANDLE, hnul);
	    SetStdHandle(STD_OUTPUT_HANDLE, hnul);
	}
	// The C runtime keeps its own stdout, used for messages.
	nul_fd = _open("NUL", _O_RDWR);
	ଯଦି (nul_fd >= 0)
	{
	    _dup2(nul_fd, 1);
	    _close(nul_fd);
	}
	in_fd = (sock_T)hin;
	out_fd = (sock_T)hout;
    }
#ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nul_fd;

	in_fd = dup(0);
	out_fd = dup(1);
	ଯଦି (in_fd < 0 || out_fd < 0)
	{
	    ଯଦି (in_fd >= 0)
		close(in_fd);
	    ଯଦି (out_fd >= 0)
		close(out_fd);
	    emsg(_(e_cannot_open_stdio_channel));
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	(ଶୂନ୍ୟ)fcntl(in_fd, F_SETFD, FD_CLOEXEC);
	(ଶୂନ୍ୟ)fcntl(out_fd, F_SETFD, FD_CLOEXEC);
	nul_fd = open("/dev/null", O_RDWR);
	ଯଦି (nul_fd >= 0)
	{
	    dup2(nul_fd, 0);
	    dup2(nul_fd, 1);
	    close(nul_fd);
	}
    }
#endif

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
    {
	ch_error(ଖାଲି, "Cannot allocate channel.");
	fd_close(in_fd);
	fd_close(out_fd);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    channel_set_pipes(channel, out_fd, in_fd, INVALID_FD);
    ch_log(channel, "Using stdin and stdout as a channel");

    // Keep a reference for channel_stdio_loop().
    ++channel->ch_refcount;
    stdio_channel = channel;
    ଫେରନ୍ତୁ channel;
}

/*
 * Implements ch_listen().
 */
    channel_T *
channel_listen_func(typval_T *argvars)
{
    char_u	*arg;
    ଅକ୍ଷର	*rest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		port;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_unix = FALSE;
    jobopt_T    opt;
    channel_T	*channel = ଖାଲି;

    ଯଦି (in_vim9script()
	    && check_for_string_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    arg = tv_get_string(&argvars[0]);
    ଯଦି (*arg == NUL)
    {
	semsg(_(e_invalid_argument_str), arg);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (STRNCMP(arg, "unix:", 5) == 0)
    {
	is_unix = TRUE;
	arg += 5;
	port = 0;
    }
    ଅନ୍ୟଥା
    {
	port = strtol((ଅକ୍ଷର *)(arg), &rest, 10);
	ଯଦି (port < 0 || port >= 65536 || *rest != NUL)
	{
	    semsg(_(e_invalid_argument_str), arg);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	*arg = NUL;
    }

    // parse options
    clear_job_options(&opt);
    opt.jo_mode = CH_MODE_JSON;
    opt.jo_timeout = 2000;
    ଯଦି (get_job_options(&argvars[1], &opt,
	    JO_MODE_ALL + JO_CB_ALL + JO_TIMEOUT_ALL, 0) == FAIL)
	ଯାଅ theend;
    ଯଦି (opt.jo_timeout < 0)
    {
	emsg(_(e_invalid_argument));
	ଯାଅ theend;
    }

    ଯଦି (is_unix)
	channel = channel_listen_unix((ଅକ୍ଷର *)arg, ଖାଲି, true);
    ଅନ୍ୟଥା
	channel = channel_listen(port, ଖାଲି);
    ଯଦି (channel != ଖାଲି)
    {
	opt.jo_set = JO_ALL;
	channel_set_options(channel, &opt);
    }
theend:
    free_job_options(&opt);
    ଫେରନ୍ତୁ channel;
}

/*
 * Listen to a socket for connections.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା port_in,
	ଶୂନ୍ୟ (*nb_close_cb)(ଶୂନ୍ୟ))
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			sd = -1;
    ସଂରଚନା sockaddr_in	server;
#ifndef FEAT_IPV6
    ସଂରଚନା hostent	*host;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			val = 1;
    channel_T		*channel;

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
    {
	ch_error(ଖାଲି, "Cannot allocate channel.");
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Get the server internet address and put into addr structure
    // fill in the socket address structure and bind to port
    vim_memset((ଅକ୍ଷର *)&server, 0, ଆକାର(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port_in);
    server.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    sd = socket(AF_INET, SOCK_STREAM, 0);
    ଯଦି (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

#ifdef MSWIN
    ଯଦି (setsockopt(sd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE,
				    (ଅଚଳ ଅକ୍ଷର *)&val, ଆକାର(val)) < 0)
#ଅନ୍ୟଥା
    ଯଦି (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR,
				    &val, ଆକାର(val)) < 0)
#endif
    {
	SOCK_ERRNO;
	ch_error(channel, "in setsockopt() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Bind the socket to the port
    ଯଦି (bind(sd, (ସଂରଚନା sockaddr *)&server, ଆକାର(server)) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // When port 0 was specified, retrieve the actual port assigned by the OS.
    ଯଦି (port_in == 0)
    {
	ସଂରଚନା sockaddr_in	addr;
	socklen_t		addr_len = ଆକାର(addr);

	ଯଦି (getsockname(sd, (ସଂରଚନା sockaddr *)&addr, &addr_len) == 0)
	    port_in = ntohs(addr.sin_port);
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ଅକ୍ଷର *)vim_strsave((char_u *)"");
    channel->ch_port = port_in;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ଫେରନ୍ତୁ channel;
}

/*
 * Listen to a Unix domain socket channel.
 * If "replace" is true, then unlink the path first beforehand.
 * Returns the channel for success.
 * Returns NULL for failure.
 */
    channel_T *
channel_listen_unix(
	ଅକ୍ଷର *path,
	ଶୂନ୍ୟ (*nb_close_cb)(ଶୂନ୍ୟ),
	bool replace)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			sd = -1;
    ସଂରଚନା sockaddr_un	server;
    channel_T		*channel;
    size_t		path_len;
    size_t		server_len;

    ଯଦି (path == ଖାଲି || *path == NUL)
    {
	semsg(_(e_invalid_argument_str), path == ଖାଲି ? (ଅକ୍ଷର *)"" : path);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    path_len = STRLEN(path);
    ଯଦି (path_len >= ଆକାର(server.sun_path))
    {
	semsg(_(e_invalid_argument_str), path);
	ଫେରନ୍ତୁ ଖାଲି;
    }

#ifdef MSWIN
    channel_init_winsock();
#endif

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
    {
	ch_error(ଖାଲି, "Cannot allocate channel.");
	ଫେରନ୍ତୁ ଖାଲି;
    }

    CLEAR_FIELD(server);
    server.sun_family = AF_UNIX;
    STRNCPY(server.sun_path, path, ଆକାର(server.sun_path) - 1);

    sd = socket(AF_UNIX, SOCK_STREAM, 0);
    ଯଦି (sd == -1)
    {
	SOCK_ERRNO;
	ch_error(channel, "in socket() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (replace)
	// Unlink the socket in case it already exists
	unlink(server.sun_path);

    // Bind the socket to the path
    server_len = offsetof(ସଂରଚନା sockaddr_un, sun_path) + path_len + 1;
    ଯଦି (bind(sd, (ସଂରଚନା sockaddr *)&server, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)server_len) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in bind() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (listen(sd, 5) < 0)
    {
	SOCK_ERRNO;
	ch_error(channel, "in listen() in channel_listen_unix().");
	PERROR(_(e_cannot_listen_on_port));
	sock_close(sd);
	channel_free(channel);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    channel->ch_listen = TRUE;
    channel->CH_SOCK_FD = (sock_T)sd;
    channel->ch_nb_close_cb = nb_close_cb;
    channel->ch_hostname = (ଅକ୍ଷର *)vim_strsave((char_u *)path);
    channel->ch_port = 0;
    channel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
    channel_gui_register_one(channel, PART_SOCK);
#endif

    ଫେରନ୍ତୁ channel;
}

     ଶୂନ୍ୟ
ch_close_part(channel_T *channel, ch_part_T part)
{
    sock_T *fd = &channel->ch_part[part].ch_fd;

    ଯଦି (*fd == INVALID_FD)
	ଫେରନ୍ତୁ;

    ଯଦି (part == PART_SOCK)
	sock_close(*fd);
    ଅନ୍ୟଥା
    {
	// When using a pty the same FD is set on multiple parts, only
	// close it when the last reference is closed.
	ଯଦି ((part == PART_IN || channel->CH_IN_FD != *fd)
		&& (part == PART_OUT || channel->CH_OUT_FD != *fd)
		&& (part == PART_ERR || channel->CH_ERR_FD != *fd))
	{
#ifdef MSWIN
	    ଯଦି (channel->ch_named_pipe)
		DisconnectNamedPipe((HANDLE)fd);
#endif
	    fd_close(*fd);
	}
    }
    *fd = INVALID_FD;

    // channel is closed, may want to end the job if it was the last
    channel->ch_to_be_closed &= ~(1U << part);
}

    ଶୂନ୍ୟ
channel_set_pipes(channel_T *channel, sock_T in, sock_T out, sock_T err)
{
    ଯଦି (in != INVALID_FD)
    {
	ch_close_part(channel, PART_IN);
	channel->CH_IN_FD = in;
#ଯଦି defined(UNIX)
	// Do not end the job when all output channels are closed, wait until
	// the job ended.
	ଯଦି (mch_isatty(in))
	    channel->ch_to_be_closed |= (1U << PART_IN);
#endif
    }
    ଯଦି (out != INVALID_FD)
    {
#ଯଦି defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_OUT);
#endif
	ch_close_part(channel, PART_OUT);
	channel->CH_OUT_FD = out;
	channel->ch_to_be_closed |= (1U << PART_OUT);
#ଯଦି defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_OUT);
#endif
    }
    ଯଦି (err != INVALID_FD)
    {
#ଯଦି defined(FEAT_GUI)
	channel_gui_unregister_one(channel, PART_ERR);
#endif
	ch_close_part(channel, PART_ERR);
	channel->CH_ERR_FD = err;
	channel->ch_to_be_closed |= (1U << PART_ERR);
#ଯଦି defined(FEAT_GUI)
	channel_gui_register_one(channel, PART_ERR);
#endif
    }
}

/*
 * Sets the job the channel is associated with and associated options.
 * This does not keep a refcount, when the job is freed ch_job is cleared.
 */
    ଶୂନ୍ୟ
channel_set_job(channel_T *channel, job_T *job, jobopt_T *options)
{
    channel->ch_job = job;

    channel_set_options(channel, options);

    ଯଦି (job->jv_in_buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    chanpart_T *in_part = &channel->ch_part[PART_IN];

    set_bufref(&in_part->ch_bufref, job->jv_in_buf);
    ch_log(channel, "reading from buffer '%s'",
	    (ଅକ୍ଷର *)in_part->ch_bufref.br_buf->b_ffname);
    ଯଦି (options->jo_set & JO_IN_ଶୀର୍ଷ)
    {
	ଯଦି (options->jo_in_ଶୀର୍ଷ == 0 && !(options->jo_set & JO_IN_BOT))
	{
	    // Special mode: send last-but-one line when appending a line
	    // to the buffer.
	    in_part->ch_bufref.br_buf->b_write_to_channel = true;
	    in_part->ch_buf_append = TRUE;
	    in_part->ch_buf_ଶୀର୍ଷ =
		in_part->ch_bufref.br_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା + 1;
	}
	ଅନ୍ୟଥା
	    in_part->ch_buf_ଶୀର୍ଷ = options->jo_in_ଶୀର୍ଷ;
    }
    ଅନ୍ୟଥା
	in_part->ch_buf_ଶୀର୍ଷ = 1;
    ଯଦି (options->jo_set & JO_IN_BOT)
	in_part->ch_buf_bot = options->jo_in_bot;
    ଅନ୍ୟଥା
	in_part->ch_buf_bot = in_part->ch_bufref.br_buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
}

/*
 * Set the callback for "channel"/"part" for the response with "id".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_set_req_callback(
	channel_T   *channel,
	ch_part_T   part,
	callback_T  *callback,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id)
{
    cbq_T *head = &channel->ch_part[part].ch_cb_head;
    cbq_T *item = ALLOC_ONE(cbq_T);

    ଯଦି (item == ଖାଲି)
	ଫେରନ୍ତୁ;

    copy_callback(&item->cq_callback, callback);
    item->cq_seq_nr = id;
    item->cq_prev = head->cq_prev;
    head->cq_prev = item;
    item->cq_next = ଖାଲି;
    ଯଦି (item->cq_prev == ଖାଲି)
	head->cq_next = item;
    ଅନ୍ୟଥା
	item->cq_prev->cq_next = item;
}

    ସ୍ଥିର ଶୂନ୍ୟ
write_buf_ଧାଡ଼ି(buf_T *buf, linenr_T lnum, channel_T *channel)
{
    char_u  *ଧାଡ଼ି = ml_get_buf(buf, lnum, FALSE);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = ml_get_buf_len(buf, lnum);
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    // Need to make a copy to be able to append a NL.
    ଯଦି ((p = alloc(len + 2)) == ଖାଲି)
	ଫେରନ୍ତୁ;
    memcpy((ଅକ୍ଷର *)p, (ଅକ୍ଷର *)ଧାଡ଼ି, len);

    ଯଦି (channel->ch_write_text_mode)
	p[len] = CAR;
    ଅନ୍ୟଥା
    {
	ପାଇଁ (i = 0; i < len; ++i)
	    ଯଦି (p[i] == NL)
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
can_write_buf_ଧାଡ଼ି(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];

    ଯଦି (in_part->ch_fd == INVALID_FD)
	ଫେରନ୍ତୁ FALSE;  // pipe was closed

    // for testing: block every other attempt to write
    ଯଦି (in_part->ch_block_write == 1)
	in_part->ch_block_write = -1;
    ଅନ୍ୟଥା ଯଦି (in_part->ch_block_write == -1)
	in_part->ch_block_write = 1;

    // TODO: Win32 implementation, probably using WaitForMultipleObjects()
#ifndef MSWIN
    {
# ଯଦି defined(HAVE_SELECT)
	ସଂରଚନା timeval	tval;
	fd_set		wfds;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

	FD_ZERO(&wfds);
	FD_SET((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)in_part->ch_fd, &wfds);
	tval.tv_sec = 0;
	tval.tv_usec = 0;
	ପାଇଁ (;;)
	{
	    ret = select((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)in_part->ch_fd + 1, ଖାଲି, &wfds, ଖାଲି, &tval);
#  ifdef EINTR
	    SOCK_ERRNO;
	    ଯଦି (ret == -1 && errno == EINTR)
		ଜାରି;
#  endif
	    ଯଦି (ret <= 0 || in_part->ch_block_write == 1)
	    {
		ଯଦି (ret > 0)
		    ch_log(channel, "FAKED Input not ready for writing");
		ଅନ୍ୟଥା
		    ch_log(channel, "Input not ready for writing");
		ଫେରନ୍ତୁ FALSE;
	    }
	    ଭାଙ୍ଗ;
	}
# ଅନ୍ୟଥା
	ସଂରଚନା pollfd	fds;

	fds.fd = in_part->ch_fd;
	fds.events = POLLOUT;
	ଯଦି (poll(&fds, 1, 0) <= 0)
	{
	    ch_log(channel, "Input not ready for writing");
	    ଫେରନ୍ତୁ FALSE;
	}
	ଯଦି (in_part->ch_block_write == 1)
	{
	    ch_log(channel, "FAKED Input not ready for writing");
	    ଫେରନ୍ତୁ FALSE;
	}
# endif
    }
#endif
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Write any buffer lines to the input channel.
 */
    ଶୂନ୍ୟ
channel_write_in(channel_T *channel)
{
    chanpart_T *in_part = &channel->ch_part[PART_IN];
    linenr_T    lnum;
    buf_T	*buf = in_part->ch_bufref.br_buf;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		written = 0;

    ଯଦି (buf == ଖାଲି || in_part->ch_buf_append)
	ଫେରନ୍ତୁ;  // no buffer or using appending
    ଯଦି (!bufref_valid(&in_part->ch_bufref) || buf->b_ml.ml_mfp == ଖାଲି)
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "input buffer has been wiped out");
	in_part->ch_bufref.br_buf = ଖାଲି;
	ଫେରନ୍ତୁ;
    }

    ପାଇଁ (lnum = in_part->ch_buf_ଶୀର୍ଷ; lnum <= in_part->ch_buf_bot
				   && lnum <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା; ++lnum)
    {
	ଯଦି (!can_write_buf_ଧାଡ଼ି(channel))
	    ଭାଙ୍ଗ;
	write_buf_ଧାଡ଼ି(buf, lnum, channel);
	++written;
    }

    ଯଦି (written == 1)
	ch_log(channel, "written line %d to channel", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)lnum - 1);
    ଅନ୍ୟଥା ଯଦି (written > 1)
	ch_log(channel, "written %d lines to channel", written);

    in_part->ch_buf_ଶୀର୍ଷ = lnum;
    ଯଦି (lnum > buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା || lnum > in_part->ch_buf_bot)
    {
#ଯଦି defined(FEAT_TERMINAL)
	// Send CTRL-D or "eof_chars" to close stdin on MS-Windows.
	ଯଦି (channel->ch_job != ଖାଲି)
	    term_send_eof(channel);
#endif

	// Writing is done, no longer need the buffer.
	in_part->ch_bufref.br_buf = ଖାଲି;
	ch_log(channel, "Finished writing all lines to channel");

	// Close the pipe/socket, so that the other side gets EOF.
#ifdef MSWIN
	// At this point, the input part of the conpty channel must not be
	// closed.  If it is closed, the pipe will be destroyed, the console
	// that was using it will be destroyed, and the process running within
	// it will be forcibly terminated, so this needs to be prevented.
	ଯଦି (!channel->ch_anonymous_pipe)
#endif
	    ch_close_part(channel, PART_IN);
    }
    ଅନ୍ୟଥା
	ch_log(channel, "Still %ld more lines to write",
				   (long)(buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - lnum + 1));
}

/*
 * Handle buffer "buf" being freed, remove it from any channels.
 */
    ଶୂନ୍ୟ
channel_buffer_free(buf_T *buf)
{
    channel_T	*channel;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
	ପାଇଁ (part = PART_SOCK; part < PART_ଗଣନା; ++part)
	{
	    chanpart_T  *ch_part = &channel->ch_part[part];

	    ଯଦି (ch_part->ch_bufref.br_buf == buf)
	    {
		ch_log(channel, "%s buffer has been wiped out",
							  ch_part_names[part]);
		ch_part->ch_bufref.br_buf = ଖାଲି;
	    }
	}
}

/*
 * Write any lines waiting to be written to "channel".
 */
    ଶୂନ୍ୟ
channel_write_ନିବେଶ(channel_T *channel)
{
    chanpart_T	*in_part = &channel->ch_part[PART_IN];

    ଯଦି (in_part->ch_writeque.wq_next != ଖାଲି)
	channel_send(channel, PART_IN, (char_u *)"", 0, "channel_write_input");
    ଅନ୍ୟଥା ଯଦି (in_part->ch_bufref.br_buf != ଖାଲି)
    {
	ଯଦି (in_part->ch_buf_append)
	    channel_write_new_ଧାଡ଼ିଗୁଡ଼ିକ(in_part->ch_bufref.br_buf);
	ଅନ୍ୟଥା
	    channel_write_in(channel);
    }
}

/*
 * Write any lines waiting to be written to a channel.
 */
    ଶୂନ୍ୟ
channel_write_any_ଧାଡ଼ିଗୁଡ଼ିକ(ଶୂନ୍ୟ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	channel_write_ନିବେଶ(channel);
}

/*
 * Write appended lines above the last one in "buf" to the channel.
 */
    ଶୂନ୍ୟ
channel_write_new_ଧାଡ଼ିଗୁଡ଼ିକ(buf_T *buf)
{
    channel_T	*channel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found_one = FALSE;

    // There could be more than one channel for the buffer, loop over all of
    // them.
    FOR_ALL_CHANNELS(channel)
    {
	chanpart_T  *in_part = &channel->ch_part[PART_IN];
	linenr_T    lnum;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    written = 0;

	ଯଦି (in_part->ch_bufref.br_buf == buf && in_part->ch_buf_append)
	{
	    ଯଦି (in_part->ch_fd == INVALID_FD)
		ଜାରି;  // pipe was closed
	    found_one = TRUE;
	    ପାଇଁ (lnum = in_part->ch_buf_bot; lnum < buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
								       ++lnum)
	    {
		ଯଦି (!can_write_buf_ଧାଡ଼ି(channel))
		    ଭାଙ୍ଗ;
		write_buf_ଧାଡ଼ି(buf, lnum, channel);
		++written;
	    }

	    ଯଦି (written == 1)
		ch_log(channel, "written line %d to channel", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)lnum - 1);
	    ଅନ୍ୟଥା ଯଦି (written > 1)
		ch_log(channel, "written %d lines to channel", written);
	    ଯଦି (lnum < buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		ch_log(channel, "Still %ld more lines to write",
				       (long)(buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା - lnum));

	    in_part->ch_buf_bot = lnum;
	}
    }
    ଯଦି (!found_one)
	buf->b_write_to_channel = false;
}

/*
 * Invoke the "callback" on channel "channel".
 * This does not redraw but sets channel_need_redraw;
 */
    ସ୍ଥିର ଶୂନ୍ୟ
invoke_callback(channel_T *channel, callback_T *callback, typval_T *argv)
{
    typval_T	rettv;

    ଯଦି (safe_to_invoke_callback == 0)
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

    ଫେରନ୍ତୁ head->rq_next;
}

/*
 * Return a pointer to the first NL in "node".
 * Skips over NUL characters.
 * Returns NULL if there is no NL.
 */
    char_u *
channel_first_nl(readq_T *ଗଣ୍ଠି)
{
    char_u  *buffer = ଗଣ୍ଠି->rq_buffer;
    long_u  i;

    ପାଇଁ (i = 0; i < ଗଣ୍ଠି->rq_buflen; ++i)
	ଯଦି (buffer[i] == NL)
	    ଫେରନ୍ତୁ buffer + i;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return the first buffer from channel "channel"/"part" and remove it.
 * The caller must free it.
 * Returns NULL if there is nothing.
 */
    char_u *
channel_get(channel_T *channel, ch_part_T part, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ଗଣ୍ଠି = head->rq_next;
    char_u *p;

    ଯଦି (ଗଣ୍ଠି == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (outlen != ଖାଲି)
	*outlen += ଗଣ୍ଠି->rq_buflen;
    // dispose of the node but keep the buffer
    p = ଗଣ୍ଠି->rq_buffer;
    head->rq_next = ଗଣ୍ଠି->rq_next;
    ଯଦି (ଗଣ୍ଠି->rq_next == ଖାଲି)
	head->rq_prev = ଖାଲି;
    ଅନ୍ୟଥା
	ଗଣ୍ଠି->rq_next->rq_prev = ଖାଲି;
    vim_free(ଗଣ୍ଠି);
    ଫେରନ୍ତୁ p;
}

/*
 * Returns the whole buffer contents concatenated for "channel"/"part".
 * Replaces NUL bytes with NL.
 */
    ସ୍ଥିର char_u *
channel_get_all(channel_T *channel, ch_part_T part, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *outlen)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ଗଣ୍ଠି;
    long_u  len = 0;
    char_u  *res;
    char_u  *p;

    // Concatenate everything into one buffer.
    ପାଇଁ (ଗଣ୍ଠି = head->rq_next; ଗଣ୍ଠି != ଖାଲି; ଗଣ୍ଠି = ଗଣ୍ଠି->rq_next)
	len += ଗଣ୍ଠି->rq_buflen;
    res = alloc(len + 1);
    ଯଦି (res == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    p = res;
    ପାଇଁ (ଗଣ୍ଠି = head->rq_next; ଗଣ୍ଠି != ଖାଲି; ଗଣ୍ଠି = ଗଣ୍ଠି->rq_next)
    {
	mch_memmove(p, ଗଣ୍ଠି->rq_buffer, ଗଣ୍ଠି->rq_buflen);
	p += ଗଣ୍ଠି->rq_buflen;
    }
    *p = NUL;

    // Free all buffers
    କର
    {
	p = channel_get(channel, part, ଖାଲି);
	vim_free(p);
    } ଯେତେବେଳେ (p != ଖାଲି);

    ଯଦି (outlen != ଖାଲି)
    {
	// Returning the length, keep NUL characters.
	*outlen += len;
	ଫେରନ୍ତୁ res;
    }

    // Turn all NUL into NL, so that the result can be used as a string.
    p = res;
    ଯେତେବେଳେ (p < res + len)
    {
	ଯଦି (*p == NUL)
	    *p = NL;
#ifdef MSWIN
	ଅନ୍ୟଥା ଯଦି (*p == 0x1b)
	{
	    // crush the escape sequence OSC 0/1/2: ESC ]0;
	    ଯଦି (p + 3 < res + len
		    && p[1] == ']'
		    && (p[2] == '0' || p[2] == '1' || p[2] == '2')
		    && p[3] == ';')
	    {
		// '\a' becomes a NL
		ଯେତେବେଳେ (p < res + (len - 1) && *p != '\a')
		    ++p;
		// BEL is zero width characters, suppress display mistake
		// ConPTY (after 10.0.18317) requires advance checking
		ଯଦି (p[-1] == NUL)
		    p[-1] = 0x07;
	    }
	}
#endif
	++p;
    }

    ଫେରନ୍ତୁ res;
}

/*
 * Consume "len" bytes from the head of "node".
 * Caller must check these bytes are available.
 */
    ଶୂନ୍ୟ
channel_consume(channel_T *channel, ch_part_T part, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    readq_T *head = &channel->ch_part[part].ch_head;
    readq_T *ଗଣ୍ଠି = head->rq_next;
    char_u *buf = ଗଣ୍ଠି->rq_buffer;

    ଯଦି (len < 0 || (long_u)len > ଗଣ୍ଠି->rq_buflen)
	ଫେରନ୍ତୁ;
    mch_memmove(buf, buf + len, ଗଣ୍ଠି->rq_buflen - len);
    ଗଣ୍ଠି->rq_buflen -= len;
    ଗଣ୍ଠି->rq_buffer[ଗଣ୍ଠି->rq_buflen] = NUL;
}

/*
 * Collapses the first and second buffer for "channel"/"part".
 * Returns FAIL if nothing was done.
 * When "want_nl" is TRUE collapse more buffers until a NL is found.
 * When the channel part mode is "lsp", collapse all the buffers as the http
 * header and the JSON content can be present in multiple buffers.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_collapse(channel_T *channel, ch_part_T part, ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_nl)
{
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    readq_T	*head = &channel->ch_part[part].ch_head;
    readq_T	*ଗଣ୍ଠି = head->rq_next;
    readq_T	*last_ଗଣ୍ଠି;
    readq_T	*n;
    char_u	*newbuf;
    char_u	*p;
    long_u	len;

    ଯଦି (ଗଣ୍ଠି == ଖାଲି || ଗଣ୍ଠି->rq_next == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    last_ଗଣ୍ଠି = ଗଣ୍ଠି->rq_next;
    len = ଗଣ୍ଠି->rq_buflen + last_ଗଣ୍ଠି->rq_buflen;
    ଯଦି (want_nl || mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	ଯେତେବେଳେ (last_ଗଣ୍ଠି->rq_next != ଖାଲି
		&& (mode == CH_MODE_LSP || mode == CH_MODE_DAP
		    || channel_first_nl(last_ଗଣ୍ଠି) == ଖାଲି))
	{
	    last_ଗଣ୍ଠି = last_ଗଣ୍ଠି->rq_next;
	    len += last_ଗଣ୍ଠି->rq_buflen;
	}

    p = newbuf = alloc(len + 1);
    ଯଦି (newbuf == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;	    // out of memory
    mch_memmove(p, ଗଣ୍ଠି->rq_buffer, ଗଣ୍ଠି->rq_buflen);
    p += ଗଣ୍ଠି->rq_buflen;
    vim_free(ଗଣ୍ଠି->rq_buffer);
    ଗଣ୍ଠି->rq_buffer = newbuf;
    ପାଇଁ (n = ଗଣ୍ଠି; n != last_ଗଣ୍ଠି; )
    {
	n = n->rq_next;
	mch_memmove(p, n->rq_buffer, n->rq_buflen);
	p += n->rq_buflen;
	vim_free(n->rq_buffer);
    }
    *p = NUL;
    ଗଣ୍ଠି->rq_buflen = (long_u)(p - newbuf);

    // dispose of the collapsed nodes and their buffers
    ପାଇଁ (n = ଗଣ୍ଠି->rq_next; n != last_ଗଣ୍ଠି; )
    {
	n = n->rq_next;
	vim_free(n->rq_prev);
    }
    ଗଣ୍ଠି->rq_next = last_ଗଣ୍ଠି->rq_next;
    ଯଦି (last_ଗଣ୍ଠି->rq_next == ଖାଲି)
	head->rq_prev = ଗଣ୍ଠି;
    ଅନ୍ୟଥା
	last_ଗଣ୍ଠି->rq_next->rq_prev = ଗଣ୍ଠି;
    vim_free(last_ଗଣ୍ଠି);
    ଫେରନ୍ତୁ OK;
}

/*
 * Store "buf[len]" on "channel"/"part".
 * When "prepend" is TRUE put in front, otherwise append at the end.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_save(channel_T *channel, ch_part_T part, char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len,
						      ପୂର୍ଣ୍ଣସଂଖ୍ୟା prepend, ଅକ୍ଷର *lead)
{
    readq_T *ଗଣ୍ଠି;
    readq_T *head = &channel->ch_part[part].ch_head;
    char_u  *p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ଗଣ୍ଠି = ALLOC_ONE(readq_T);
    ଯଦି (ଗଣ୍ଠି == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;	    // out of memory
    // A NUL is added at the end, because netbeans code expects that.
    // Otherwise a NUL may appear inside the text.
    ଗଣ୍ଠି->rq_buffer = alloc(len + 1);
    ଯଦି (ଗଣ୍ଠି->rq_buffer == ଖାଲି)
    {
	vim_free(ଗଣ୍ଠି);
	ଫେରନ୍ତୁ FAIL;	    // out of memory
    }

    ଯଦି (channel->ch_part[part].ch_mode == CH_MODE_NL)
    {
	// Drop any CR before a NL.
	p = ଗଣ୍ଠି->rq_buffer;
	ପାଇଁ (i = 0; i < len; ++i)
	    ଯଦି (buf[i] != CAR || i + 1 >= len || buf[i + 1] != NL)
		*p++ = buf[i];
	*p = NUL;
	ଗଣ୍ଠି->rq_buflen = (long_u)(p - ଗଣ୍ଠି->rq_buffer);
    }
    ଅନ୍ୟଥା
    {
	mch_memmove(ଗଣ୍ଠି->rq_buffer, buf, len);
	ଗଣ୍ଠି->rq_buffer[len] = NUL;
	ଗଣ୍ଠି->rq_buflen = (long_u)len;
    }

    ଯଦି (prepend)
    {
	// prepend node to the head of the queue
	ଗଣ୍ଠି->rq_next = head->rq_next;
	ଗଣ୍ଠି->rq_prev = ଖାଲି;
	ଯଦି (head->rq_next == ଖାଲି)
	    head->rq_prev = ଗଣ୍ଠି;
	ଅନ୍ୟଥା
	    head->rq_next->rq_prev = ଗଣ୍ଠି;
	head->rq_next = ଗଣ୍ଠି;
    }
    ଅନ୍ୟଥା
    {
	// append node to the tail of the queue
	ଗଣ୍ଠି->rq_next = ଖାଲି;
	ଗଣ୍ଠି->rq_prev = head->rq_prev;
	ଯଦି (head->rq_prev == ଖାଲି)
	    head->rq_next = ଗଣ୍ଠି;
	ଅନ୍ୟଥା
	    head->rq_prev->rq_next = ଗଣ୍ଠି;
	head->rq_prev = ଗଣ୍ଠି;
    }

    ଯଦି (ch_log_active() && lead != ଖାଲି)
	ch_log_literal(lead, channel, part, buf, len);

    ଫେରନ୍ତୁ OK;
}

/*
 * Try to fill the buffer of "reader".
 * Returns FALSE when nothing was added.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_fill(js_read_T *reader)
{
    channel_T	*channel = (channel_T *)reader->js_cookie;
    ch_part_T	part = reader->js_cookie_arg;
    char_u	*next = channel_get(channel, part, ଖାଲି);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keeplen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		addlen;
    char_u	*p;

    ଯଦି (next == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    keeplen = reader->js_end - reader->js_buf;
    ଯଦି (keeplen > 0)
    {
	// Prepend unused text.
	addlen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(next);
	p = alloc(keeplen + addlen + 1);
	ଯଦି (p == ଖାଲି)
	{
	    vim_free(next);
	    ଫେରନ୍ତୁ FALSE;
	}
	mch_memmove(p, reader->js_buf, keeplen);
	mch_memmove(p + keeplen, next, addlen + 1);
	vim_free(next);
	next = p;
    }

    vim_free(reader->js_buf);
    reader->js_buf = next;
    ଫେରନ୍ତୁ TRUE;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_process_lspdap_http_hdr(js_read_T *reader)
{
    char_u	*ଧାଡ଼ି_start;
    char_u	*p;
    int_u	hdr_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		payload_len = -1;
    int_u	jsbuf_len;

    // We find the end once, to avoid calling strlen() many times.
    jsbuf_len = (int_u)STRLEN(reader->js_buf);
    reader->js_end = reader->js_buf + jsbuf_len;

    p = reader->js_buf;

    // Process each line in the header till an empty line is read (header
    // separator).
    ଯେତେବେଳେ (TRUE)
    {
	ଧାଡ଼ି_start = p;
	ଯେତେବେଳେ (*p != NUL && *p != '\n')
	    p++;
	ଯଦି (*p == NUL)			// partial header
	    ଫେରନ୍ତୁ MAYBE;
	p++;

	// process the content length field (if present)
	ଯଦି ((p - ଧାଡ଼ି_start > 16)
		&& STRNICMP(ଧାଡ଼ି_start, "Content-Length: ", 16) == 0)
	{
	    errno = 0;
	    payload_len = strtol((ଅକ୍ଷର *)ଧାଡ଼ି_start + 16, ଖାଲି, 10);
	    ଯଦି (errno == ERANGE || payload_len < 0)
		// invalid length, discard the payload
		ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି ((p - ଧାଡ଼ି_start) == 2 && ଧାଡ଼ି_start[0] == '\r' &&
		ଧାଡ଼ି_start[1] == '\n')
	    // reached the empty line
	    ଭାଙ୍ଗ;
    }

    ଯଦି (payload_len == -1)
	// Content-Length field is not present in the header
	ଫେରନ୍ତୁ FAIL;

    hdr_len = p - reader->js_buf;

    // if the entire payload is not received, wait for more data to arrive
    ଯଦି (jsbuf_len < hdr_len + payload_len)
	ଫେରନ୍ତୁ MAYBE;

    reader->js_used += hdr_len;
    // recalculate the end based on the length read from the header.
    reader->js_end = reader->js_buf + hdr_len + payload_len;

    ଫେରନ୍ତୁ OK;
}

/*
 * Use the read buffer of "channel"/"part" and parse a JSON message that is
 * complete.  The messages are added to the queue. If "socketserver" is true,
 * then ignore the channel mode.
 * Return TRUE if there is more to read.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_parse_json(channel_T *channel, ch_part_T part, bool socketserver)
{
    js_read_T	reader;
    typval_T	listtv;
    jsonq_T	*item;
    chanpart_T	*chanpart = &channel->ch_part[part];
    jsonq_T	*head = &chanpart->ch_json_head;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ଯଦି (channel_peek(channel, part) == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    reader.js_buf = channel_get(channel, part, ଖାଲି);
    reader.js_used = 0;
    reader.js_fill = channel_fill;
    reader.js_cookie = channel;
    reader.js_cookie_arg = part;

    ଯଦି (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
	    || chanpart->ch_mode == CH_MODE_DAP))
	status = channel_process_lspdap_http_hdr(&reader);

    // When a message is incomplete we wait for a short while for more to
    // arrive.  After the delay drop the input, otherwise a truncated string
    // or list will make us hang.
    // Do not generate error messages, they will be written in a channel log.
    ଯଦି (status == OK)
    {
	++emsg_silent;
	status = json_decode(&reader, &listtv,
				chanpart->ch_mode == CH_MODE_JS ? JSON_JS : 0);
	--emsg_silent;
    }
    ଯଦି (status == OK)
    {
	// Only accept the response when it is a list with at least two
	// items.
	ଯଦି (!socketserver && (chanpart->ch_mode == CH_MODE_LSP
		    || chanpart->ch_mode == CH_MODE_DAP)
		&& listtv.v_type != VAR_DICT)
	{
	    ch_error(channel, "Did not receive a LSP dict, discarding");
	    clear_tv(&listtv);
	}
	ଅନ୍ୟଥା ଯଦି (!socketserver && chanpart->ch_mode != CH_MODE_LSP
		&& chanpart->ch_mode != CH_MODE_DAP
		&& (listtv.v_type != VAR_LIST || listtv.vval.v_list->lv_len < 2))
	{
	    ଯଦି (listtv.v_type != VAR_LIST)
		ch_error(channel, "Did not receive a list, discarding");
	    ଅନ୍ୟଥା
		ch_error(channel, "Expected list with two items, got %d",
						  listtv.vval.v_list->lv_len);
	    clear_tv(&listtv);
	}
	ଅନ୍ୟଥା
	{
	    item = ALLOC_ONE(jsonq_T);
	    ଯଦି (item == ଖାଲି)
		clear_tv(&listtv);
	    ଅନ୍ୟଥା
	    {
		item->jq_no_callback = FALSE;
		item->jq_ମୂଲ୍ୟ = alloc_tv();
		ଯଦି (item->jq_ମୂଲ୍ୟ == ଖାଲି)
		{
		    vim_free(item);
		    clear_tv(&listtv);
		}
		ଅନ୍ୟଥା
		{
		    *item->jq_ମୂଲ୍ୟ = listtv;
		    item->jq_prev = head->jq_prev;
		    head->jq_prev = item;
		    item->jq_next = ଖାଲି;
		    ଯଦି (item->jq_prev == ଖାଲି)
			head->jq_next = item;
		    ଅନ୍ୟଥା
			item->jq_prev->jq_next = item;
		}
	    }
	}
    }

    ଯଦି (status == OK)
	chanpart->ch_wait_len = 0;
    ଅନ୍ୟଥା ଯଦି (status == MAYBE)
    {
	size_t buflen = STRLEN(reader.js_buf);

	ଯଦି (chanpart->ch_wait_len < buflen)
	{
	    // First time encountering incomplete message or after receiving
	    // more (but still incomplete): set a deadline of 100 msec.
	    ch_log(channel,
		    "Incomplete message (%d bytes) - wait 100 msec for more",
		    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)buflen);
	    reader.js_used = 0;
	    chanpart->ch_wait_len = buflen;
#ifdef MSWIN
	    chanpart->ch_deadline = Get_Tick_ଗଣନା() + 100L;
#ଅନ୍ୟଥା
	    gettimeofday(&chanpart->ch_deadline, ଖାଲି);
	    chanpart->ch_deadline.tv_usec += 100 * 1000;
	    ଯଦି (chanpart->ch_deadline.tv_usec > 1000 * 1000)
	    {
		chanpart->ch_deadline.tv_usec -= 1000 * 1000;
		++chanpart->ch_deadline.tv_sec;
	    }
#endif
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout;
#ifdef MSWIN
	    timeout = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(Get_Tick_ଗଣନା() - chanpart->ch_deadline) > 0;
#ଅନ୍ୟଥା
	    {
		ସଂରଚନା timeval now_tv;

		gettimeofday(&now_tv, ଖାଲି);
		timeout = now_tv.tv_sec > chanpart->ch_deadline.tv_sec
		      || (now_tv.tv_sec == chanpart->ch_deadline.tv_sec
			   && now_tv.tv_usec > chanpart->ch_deadline.tv_usec);
	    }
#endif
	    ଯଦି (timeout)
	    {
		status = FAIL;
		chanpart->ch_wait_len = 0;
		ch_log(channel, "timed out");
	    }
	    ଅନ୍ୟଥା
	    {
		reader.js_used = 0;
		ch_log(channel, "still waiting on incomplete message");
	    }
	}
    }

    ଯଦି (status == FAIL)
    {
	ch_error(channel, "Decoding failed - discarding input");
	ret = FALSE;
	chanpart->ch_wait_len = 0;
    }
    ଅନ୍ୟଥା ଯଦି (reader.js_buf[reader.js_used] != NUL)
    {
	// Put the unread part back into the channel.
	channel_save(channel, part, reader.js_buf + reader.js_used,
			(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(reader.js_end - reader.js_buf) - reader.js_used,
								  TRUE, ଖାଲି);
	ret = status == MAYBE ? FALSE: TRUE;
    }
    ଅନ୍ୟଥା
	ret = FALSE;

    vim_free(reader.js_buf);
    ଫେରନ୍ତୁ ret;
}

/*
 * Remove "node" from the queue that it is in.  Does not free it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
remove_cb_ଗଣ୍ଠି(cbq_T *head, cbq_T *ଗଣ୍ଠି)
{
    ଯଦି (ଗଣ୍ଠି->cq_prev == ଖାଲି)
	head->cq_next = ଗଣ୍ଠି->cq_next;
    ଅନ୍ୟଥା
	ଗଣ୍ଠି->cq_prev->cq_next = ଗଣ୍ଠି->cq_next;
    ଯଦି (ଗଣ୍ଠି->cq_next == ଖାଲି)
	head->cq_prev = ଗଣ୍ଠି->cq_prev;
    ଅନ୍ୟଥା
	ଗଣ୍ଠି->cq_next->cq_prev = ଗଣ୍ଠି->cq_prev;
}

/*
 * Remove "node" from the queue that it is in and free it.
 * Caller should have freed or used node->jq_value.
 */
    ଶୂନ୍ୟ
remove_json_ଗଣ୍ଠି(jsonq_T *head, jsonq_T *ଗଣ୍ଠି)
{
    ଯଦି (ଗଣ୍ଠି->jq_prev == ଖାଲି)
	head->jq_next = ଗଣ୍ଠି->jq_next;
    ଅନ୍ୟଥା
	ଗଣ୍ଠି->jq_prev->jq_next = ଗଣ୍ଠି->jq_next;
    ଯଦି (ଗଣ୍ଠି->jq_next == ଖାଲି)
	head->jq_prev = ଗଣ୍ଠି->jq_prev;
    ଅନ୍ୟଥା
	ଗଣ୍ଠି->jq_next->jq_prev = ଗଣ୍ଠି->jq_prev;
    vim_free(ଗଣ୍ଠି);
}

/*
 * Add "id" to the list of JSON message IDs we are waiting on.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_add_block_id(chanpart_T *chanpart, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    garray_T *gap = &chanpart->ch_block_ids;

    ଯଦି (gap->ga_growsize == 0)
	ga_init2(gap, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 10);
    ଯଦି (ga_grow(gap, 1) == OK)
    {
	((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[gap->ga_len] = id;
	++gap->ga_len;
    }
}

/*
 * Remove "id" from the list of JSON message IDs we are waiting on.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_remove_block_id(chanpart_T *chanpart, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	ଯଦି (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[i] == id)
	{
	    --gap->ga_len;
	    ଯଦି (i < gap->ga_len)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା *p = ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data) + i;

		mch_memmove(p, p + 1, (gap->ga_len - i) * ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
	    }
	    ଫେରନ୍ତୁ;
	}
    siemsg("channel_remove_block_id(): cannot find id %d", id);
}

/*
 * Return TRUE if "id" is in the list of JSON message IDs we are waiting on.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_has_block_id(chanpart_T *chanpart, ପୂର୍ଣ୍ଣସଂଖ୍ୟା id)
{
    garray_T	*gap = &chanpart->ch_block_ids;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	ଯଦି (((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)gap->ga_data)[i] == id)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_get_json(
	channel_T   *channel,
	ch_part_T   part,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    without_callback,
	typval_T    **rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;

    ଯେତେବେଳେ (item != ଖାଲି)
    {
	list_T	    *l;
	typval_T    *tv;

	ଯଦି (channel->ch_part[part].ch_mode != CH_MODE_LSP
		&& channel->ch_part[part].ch_mode != CH_MODE_DAP)
	{
	    l = item->jq_ମୂଲ୍ୟ->vval.v_list;
	    CHECK_LIST_MATERIALIZE(l);
	    tv = &l->lv_first->li_tv;
	}
	ଅନ୍ୟଥା
	{
	    dict_T	*d;
	    dictitem_T	*di;

	    ଯଦି (channel->ch_part[part].ch_mode == CH_MODE_LSP)
	    {
		// LSP message payload is a JSON-RPC dict. For RPC requests and
		// responses, the 'id' item will be present. For notifications,
		// it will not be present.
		ଯଦି (id > 0)
		{
		    ଯଦି (item->jq_ମୂଲ୍ୟ->v_type != VAR_DICT)
			ଯାଅ nextitem;
		    d = item->jq_ମୂଲ୍ୟ->vval.v_dict;
		    ଯଦି (d == ଖାଲି)
			ଯାଅ nextitem;
		    // When looking for a response message from the LSP server,
		    // ignore new LSP request and notification messages.  LSP
		    // request and notification messages have the "method" field
		    // in the header and the response messages do not have this
		    // field.
		    ଯଦି (dict_has_ଚାବି(d, "method"))
			ଯାଅ nextitem;
		    di = dict_find(d, (char_u *)"id", -1);
		    ଯଦି (di == ଖାଲି)
			ଯାଅ nextitem;
		    tv = &di->di_tv;
		}
		ଅନ୍ୟଥା
		    tv = item->jq_ମୂଲ୍ୟ;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (id > 0)
		{
		    ଯଦି (item->jq_ମୂଲ୍ୟ->v_type != VAR_DICT)
			ଯାଅ nextitem;
		    d = item->jq_ମୂଲ୍ୟ->vval.v_dict;
		    ଯଦି (d == ଖାଲି)
			ଯାଅ nextitem;
		    di = dict_find(d, (char_u *)"request_seq", -1);
		    ଯଦି (di == ଖାଲି)
			ଯାଅ nextitem;
		    tv = &di->di_tv;
		}
		ଅନ୍ୟଥା
		    tv = item->jq_ମୂଲ୍ୟ;
	    }
	}

	ଯଦି ((without_callback || !item->jq_no_callback)
	    && ((id > 0 && tv->v_type == VAR_ସଂଖ୍ୟା && tv->vval.v_ସଂଖ୍ୟା == id)
	      || (id <= 0 && (tv->v_type != VAR_ସଂଖ୍ୟା
		 || tv->vval.v_ସଂଖ୍ୟା == 0
		 || !channel_has_block_id(
				&channel->ch_part[part], tv->vval.v_ସଂଖ୍ୟା)))))
	{
	    *rettv = item->jq_ମୂଲ୍ୟ;
	    ଯଦି (tv->v_type == VAR_ସଂଖ୍ୟା)
		ch_log(channel, "Getting JSON message %ld",
						      (long)tv->vval.v_ସଂଖ୍ୟା);
	    remove_json_ଗଣ୍ଠି(head, item);
	    ଫେରନ୍ତୁ OK;
	}
nextitem:
	item = item->jq_next;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Put back "rettv" into the JSON queue, there was no callback for it.
 * Takes over the values in "rettv".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_ଠେଲ_json(channel_T *channel, ch_part_T part, typval_T *rettv)
{
    jsonq_T   *head = &channel->ch_part[part].ch_json_head;
    jsonq_T   *item = head->jq_next;
    jsonq_T   *newitem;

    ଯଦି (head->jq_prev != ଖାଲି && head->jq_prev->jq_no_callback)
	// last item was pushed back, append to the end
	item = ଖାଲି;
    ଅନ୍ୟଥା ଯେତେବେଳେ (item != ଖାଲି && item->jq_no_callback)
	// append after the last item that was pushed back
	item = item->jq_next;

    newitem = ALLOC_ONE(jsonq_T);
    ଯଦି (newitem == ଖାଲି)
    {
	clear_tv(rettv);
	ଫେରନ୍ତୁ;
    }

    newitem->jq_ମୂଲ୍ୟ = alloc_tv();
    ଯଦି (newitem->jq_ମୂଲ୍ୟ == ଖାଲି)
    {
	vim_free(newitem);
	clear_tv(rettv);
	ଫେରନ୍ତୁ;
    }

    newitem->jq_no_callback = FALSE;
    *newitem->jq_ମୂଲ୍ୟ = *rettv;
    ଯଦି (item == ଖାଲି)
    {
	// append to the end
	newitem->jq_prev = head->jq_prev;
	head->jq_prev = newitem;
	newitem->jq_next = ଖାଲି;
	ଯଦି (newitem->jq_prev == ଖାଲି)
	    head->jq_next = newitem;
	ଅନ୍ୟଥା
	    newitem->jq_prev->jq_next = newitem;
    }
    ଅନ୍ୟଥା
    {
	// append after "item"
	newitem->jq_prev = item;
	newitem->jq_next = item->jq_next;
	item->jq_next = newitem;
	ଯଦି (newitem->jq_next == ଖାଲି)
	    head->jq_prev = newitem;
	ଅନ୍ୟଥା
	    newitem->jq_next->jq_prev = newitem;
    }
}

#define CH_JSON_MAX_ARGS 4

/*
 * Execute a command received over "channel"/"part"
 * "argv[0]" is the command string.
 * "argv[1]" etc. have further arguments, type is VAR_UNKNOWN if missing.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_exe_cmd(channel_T *channel, ch_part_T part, typval_T *argv)
{
    char_u  *cmd = argv[0].vval.v_string;
    char_u  *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    options = channel->ch_part[part].ch_mode == CH_MODE_JS
								 ? JSON_JS : 0;

    ଯଦି (argv[1].v_type != VAR_STRING)
    {
	ch_error(channel, "received command with non-string argument");
	ଯଦି (p_verbose > 2)
	    emsg(_(e_received_command_with_non_string_argument));
	ଫେରନ୍ତୁ;
    }
    arg = argv[1].vval.v_string;
    ଯଦି (arg == ଖାଲି)
	arg = (char_u *)"";

    ଯଦି (STRCMP(cmd, "ex") == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	called_emsg_before = called_emsg;
	char_u	*p = arg;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_emsg_silent;

	ch_log(channel, "Executing ex command '%s'", (ଅକ୍ଷର *)arg);
	do_emsg_silent = !checkforcmd(&p, "echoerr", 5);
	ଯଦି (do_emsg_silent)
	    ++emsg_silent;
	do_cmdline_cmd(arg);
	ଯଦି (do_emsg_silent)
	    --emsg_silent;
	ଯଦି (called_emsg > called_emsg_before)
	    ch_log(channel, "Ex command error: '%s'",
					  (ଅକ୍ଷର *)get_vim_var_str(VV_ERRMSG));
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(cmd, "normal") == 0)
    {
	exarg_T ea;

	ch_log(channel, "Executing normal command '%s'", (ଅକ୍ଷର *)arg);
	CLEAR_FIELD(ea);
	ea.arg = arg;
	ea.addr_ଗଣନା = 0;
	ea.forceit = TRUE; // no mapping
	ex_normal(&ea);
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(cmd, "redraw") == 0)
    {
	ch_log(channel, "redraw");
	redraw_cmd(*arg != NUL);
	showruler(FALSE);
	setcursor();
	out_flush_cursor(TRUE, FALSE);
    }
    ଅନ୍ୟଥା ଯଦି (STRCMP(cmd, "expr") == 0 || STRCMP(cmd, "call") == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_call = cmd[0] == 'c';
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା id_idx = is_call ? 3 : 2;

	ଯଦି (argv[id_idx].v_type != VAR_UNKNOWN
					 && argv[id_idx].v_type != VAR_ସଂଖ୍ୟା)
	{
	    ch_error(channel, "last argument for expr/call must be a number");
	    ଯଦି (p_verbose > 2)
		emsg(_(e_last_argument_for_ପଦସମଷ୍ଟି_call_must_be_ସଂଖ୍ୟା));
	}
	ଅନ୍ୟଥା ଯଦି (is_call && argv[2].v_type != VAR_LIST)
	{
	    ch_error(channel, "third argument for call must be a list");
	    ଯଦି (p_verbose > 2)
		emsg(_(e_third_argument_for_call_must_be_list));
	}
	ଅନ୍ୟଥା
	{
	    typval_T	*tv = ଖାଲି;
	    typval_T	res_tv;
	    typval_T	err_tv;
	    char_u	*json = ଖାଲି;

	    // Don't pollute the display with errors.
	    // Do generate the errors so that try/catch works.
	    ++emsg_silent;
	    ଯଦି (!is_call)
	    {
		ch_log(channel, "Evaluating expression '%s'", (ଅକ୍ଷର *)arg);
		tv = eval_ପଦସମଷ୍ଟି(arg, ଖାଲି);
	    }
	    ଅନ୍ୟଥା
	    {
		ch_log(channel, "Calling '%s'", (ଅକ୍ଷର *)arg);
		ଯଦି (func_call(arg, &argv[2], ଖାଲି, ଖାଲି, &res_tv) == OK)
		    tv = &res_tv;
	    }

	    ଯଦି (argv[id_idx].v_type == VAR_ସଂଖ୍ୟା)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା id = argv[id_idx].vval.v_ସଂଖ୍ୟା;

		ଯଦି (tv != ଖାଲି)
		    json = json_encode_nr_ପଦସମଷ୍ଟି(id, tv, options | JSON_NL);
		ଯଦି (tv == ଖାଲି || (json != ଖାଲି && *json == NUL))
		{
		    // If evaluation failed or the result can't be encoded
		    // then return the string "ERROR".
		    vim_free(json);
		    err_tv.v_type = VAR_STRING;
		    err_tv.vval.v_string = (char_u *)"ERROR";
		    json = json_encode_nr_ପଦସମଷ୍ଟି(id, &err_tv, options | JSON_NL);
		}
		ଯଦି (json != ଖାଲି)
		{
		    channel_send(channel,
				 part == PART_SOCK ? PART_SOCK : PART_IN,
				 json, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(json), (ଅକ୍ଷର *)cmd);
		    vim_free(json);
		}
	    }
	    --emsg_silent;
	    ଯଦି (tv == &res_tv)
		clear_tv(tv);
	    ଅନ୍ୟଥା
		free_tv(tv);
	}
    }
    ଅନ୍ୟଥା ଯଦି (p_verbose > 2)
    {
	ch_error(channel, "Received unknown command: %s", (ଅକ୍ଷର *)cmd);
	semsg(_(e_received_unknown_command_str), cmd);
    }
}

/*
 * Invoke the callback at "cbhead".
 * Does not redraw but sets channel_need_redraw.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
invoke_one_time_callback(
	channel_T   *channel,
	cbq_T	    *cbhead,
	cbq_T	    *item,
	typval_T    *argv)
{
    ch_log(channel, "Invoking one-time callback %s",
					    (ଅକ୍ଷର *)item->cq_callback.cb_name);
    // Remove the item from the list first, if the callback
    // invokes ch_close() the list will be cleared.
    remove_cb_ଗଣ୍ଠି(cbhead, item);
    invoke_callback(channel, &item->cq_callback, argv);
    free_callback(&item->cq_callback);
    vim_free(item);
}

    ସ୍ଥିର ଶୂନ୍ୟ
append_to_buffer(
    buf_T	*buffer,
    char_u	*ସନ୍ଦେଶ,
    channel_T	*channel,
    ch_part_T	part)
{
    aco_save_T	aco;
    linenr_T    lnum = buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    bool	save_write_to = buffer->b_write_to_channel;
    chanpart_T  *ch_part = &channel->ch_part[part];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_p_ma = buffer->b_p_ma;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		empty = (buffer->b_ml.ml_flags & ML_EMPTY) ? 1 : 0;

    ଯଦି (!buffer->b_p_ma && !ch_part->ch_nomodifiable)
    {
	ଯଦି (!ch_part->ch_nomod_error)
	{
	    ch_error(channel, "Buffer is not modifiable, cannot append");
	    ch_part->ch_nomod_error = TRUE;
	}
	ଫେରନ୍ତୁ;
    }

    // If the buffer is also used as input insert above the last
    // line. Don't write these lines.
    ଯଦି (save_write_to)
    {
	--lnum;
	buffer->b_write_to_channel = false;
    }

    // Append to the buffer
    ch_log(channel, "appending line %d to buffer %s",
				       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)lnum + 1 - empty, buffer->b_fname);

    buffer->b_p_ma = TRUE;

    // Set curbuf to "buffer", temporarily.
    aucmd_prepbuf(&aco, buffer);
    ଯଦି (curbuf != buffer)
    {
	// Could not find a window for this buffer, the following might cause
	// trouble, better bail out.
	ଫେରନ୍ତୁ;
    }

    u_sync(TRUE);
    // ignore undo failure, undo is not very useful here
    vim_ignored = u_save(lnum - empty, lnum + 1);

    ଯଦି (empty)
    {
	// The buffer is empty, replace the first (dummy) line.
	ml_replace(lnum, ସନ୍ଦେଶ, TRUE);
	lnum = 0;
    }
    ଅନ୍ୟଥା
	ml_append(lnum, ସନ୍ଦେଶ, 0, FALSE);
    appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(lnum, 1L);

    // reset notion of buffer
    aucmd_restbuf(&aco);

    ଯଦି (ch_part->ch_nomodifiable)
	buffer->b_p_ma = FALSE;
    ଅନ୍ୟଥା
	buffer->b_p_ma = save_p_ma;

    ଯଦି (buffer->b_nwindows > 0)
    {
	win_T	*wp;

	FOR_ALL_WINDOWS(wp)
	{
	    ଯଦି (wp->w_buffer == buffer)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା move_cursor = save_write_to
			    ? wp->w_cursor.lnum == lnum + 1
			    : (wp->w_cursor.lnum == lnum
				&& wp->w_cursor.col == 0);

		// If the cursor is at or above the new line, move it one line
		// down.  If the topline is outdated update it now.
		ଯଦି (move_cursor || wp->w_topline > buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
		{
		    win_T *save_curwin = curwin;

		    ଯଦି (move_cursor)
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

    ଯଦି (save_write_to)
    {
	channel_T *ch;

	// Find channels reading from this buffer and adjust their
	// next-to-read line number.
	buffer->b_write_to_channel = true;
	FOR_ALL_CHANNELS(ch)
	{
	    chanpart_T  *in_part = &ch->ch_part[PART_IN];

	    ଯଦି (in_part->ch_bufref.br_buf == buffer)
		in_part->ch_buf_bot = buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
	}
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
drop_messages(channel_T *channel, ch_part_T part)
{
    char_u *ସନ୍ଦେଶ;

    ଯେତେବେଳେ ((ସନ୍ଦେଶ = channel_get(channel, part, ଖାଲି)) != ଖାଲି)
    {
	ch_log(channel, "Dropping message '%s'", (ଅକ୍ଷର *)ସନ୍ଦେଶ);
	vim_free(ସନ୍ଦେଶ);
    }
}

/*
 * Return TRUE if for "channel" / "part" ch_json_head should be used.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_use_json_head(channel_T *channel, ch_part_T part)
{
    ch_mode_T	ch_mode = channel->ch_part[part].ch_mode;

    ଫେରନ୍ତୁ ch_mode == CH_MODE_JSON || ch_mode == CH_MODE_JS
						     || ch_mode == CH_MODE_LSP
						     || ch_mode == CH_MODE_DAP;
}

/*
 * Invoke a callback for "channel"/"part" if needed.
 * This does not redraw but sets channel_need_redraw when redraw is needed.
 * Return TRUE when a message was handled, there might be another one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_invoke_callback(channel_T *channel, ch_part_T part)
{
    char_u	*ସନ୍ଦେଶ = ଖାଲି;
    blob_T	*blob = ଖାଲି;
    typval_T	*listtv = ଖାଲି;
    typval_T	argv[CH_JSON_MAX_ARGS];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		seq_nr = -1;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ch_mode_T	ch_mode = ch_part->ch_mode;
    cbq_T	*cbhead = &ch_part->ch_cb_head;
    cbq_T	*cbitem;
    callback_T	*callback = ଖାଲି;
    buf_T	*buffer = ଖାଲି;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_otc;		// one time callbackup
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		raw_len = 0;

    ଯଦି (channel->ch_nb_close_cb != ଖାଲି
#ifdef FEAT_SOCKETSERVER
	    || channel->ch_socketserver
#endif
	    )
	// this channel is handled elsewhere (netbeans or socketserver)
	ଫେରନ୍ତୁ FALSE;

    // Use a message-specific callback, part callback or channel callback
    ପାଇଁ (cbitem = cbhead->cq_next; cbitem != ଖାଲି; cbitem = cbitem->cq_next)
	ଯଦି (cbitem->cq_seq_nr == 0)
	    ଭାଙ୍ଗ;
    ଯଦି (cbitem != ଖାଲି)
	callback = &cbitem->cq_callback;
    ଅନ୍ୟଥା ଯଦି (ch_part->ch_callback.cb_name != ଖାଲି)
	callback = &ch_part->ch_callback;
    ଅନ୍ୟଥା ଯଦି (channel->ch_callback.cb_name != ଖାଲି)
	callback = &channel->ch_callback;

    buffer = ch_part->ch_bufref.br_buf;
    ଯଦି (buffer != ଖାଲି && (!bufref_valid(&ch_part->ch_bufref)
					       || buffer->b_ml.ml_mfp == ଖାଲି))
    {
	// buffer was wiped out or unloaded
	ch_log(channel, "%s buffer has been wiped out", ch_part_names[part]);
	ch_part->ch_bufref.br_buf = ଖାଲି;
	buffer = ଖାଲି;
    }

    ଯଦି (channel_use_json_head(channel, part))
    {
	listitem_T	*item;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = 0;

	// Get any json message in the queue.
	ଯଦି (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
	{
	    ଯଦି (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		// In the "lsp" or "dap" mode, the http header and the json
		// payload may be received in multiple messages. So concatenate
		// all the received messages.
		(ଶୂନ୍ୟ)channel_collapse(channel, part, FALSE);

	    // Parse readahead, return when there is still no message.
	    channel_parse_json(channel, part, false);
	    ଯଦି (channel_get_json(channel, part, -1, FALSE, &listtv) == FAIL)
		ଫେରନ୍ତୁ FALSE;
	}

	ଯଦି (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	{
	    dict_T	*d = listtv->vval.v_dict;
	    dictitem_T	*di;

	    seq_nr = 0;
	    ଯଦି (d != ଖାଲି)
	    {
		ଯଦି (ch_mode == CH_MODE_LSP)
		    di = dict_find(d, (char_u *)"id", -1);
		ଅନ୍ୟଥା
		    di = dict_find(d, (char_u *)"request_seq", -1);
		ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_ସଂଖ୍ୟା)
		    seq_nr = di->di_tv.vval.v_ସଂଖ୍ୟା;
	    }

	    argv[1] = *listtv;
	}
	ଅନ୍ୟଥା
	{
	    ପାଇଁ (item = listtv->vval.v_list->lv_first;
		    item != ଖାଲି && argc < CH_JSON_MAX_ARGS;
		    item = item->li_next)
		argv[argc++] = item->li_tv;
	    ଯେତେବେଳେ (argc < CH_JSON_MAX_ARGS)
		argv[argc++].v_type = VAR_UNKNOWN;

	    ଯଦି (argv[0].v_type == VAR_STRING)
	    {
		// ["cmd", arg] or ["cmd", arg, arg] or ["cmd", arg, arg, arg]
		channel_exe_cmd(channel, part, argv);
		free_tv(listtv);
		ଫେରନ୍ତୁ TRUE;
	    }

	    ଯଦି (argv[0].v_type != VAR_ସଂଖ୍ୟା)
	    {
		ch_error(channel,
			"Dropping message with invalid sequence number type");
		free_tv(listtv);
		ଫେରନ୍ତୁ FALSE;
	    }
	    seq_nr = argv[0].vval.v_ସଂଖ୍ୟା;
	}
    }
    ଅନ୍ୟଥା ଯଦି (channel_peek(channel, part) == ଖାଲି)
    {
	// nothing to read on RAW or NL channel
	ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା
    {
	// If there is no callback or buffer drop the message.
	ଯଦି (callback == ଖାଲି && buffer == ଖାଲି)
	{
	    // If there is a close callback it may use ch_read() to get the
	    // messages.
	    ଯଦି (channel->ch_close_cb.cb_name == ଖାଲି && !channel->ch_drop_never)
		drop_messages(channel, part);
	    ଫେରନ୍ତୁ FALSE;
	}

	ଯଦି (ch_mode == CH_MODE_NL)
	{
	    char_u  *nl = ଖାଲି;
	    char_u  *buf;
	    readq_T *ଗଣ୍ଠି;

	    // See if we have a message ending in NL in the first buffer.  If
	    // not try to concatenate the first and the second buffer.
	    ଯେତେବେଳେ (TRUE)
	    {
		ଗଣ୍ଠି = channel_peek(channel, part);
		nl = channel_first_nl(ଗଣ୍ଠି);
		ଯଦି (nl != ଖାଲି)
		    ଭାଙ୍ଗ;
		ଯଦି (channel_collapse(channel, part, TRUE) == FAIL)
		{
		    ଯଦି (ch_part->ch_fd == INVALID_FD && ଗଣ୍ଠି->rq_buflen > 0)
			ଭାଙ୍ଗ;
		    ଫେରନ୍ତୁ FALSE; // incomplete message
		}
	    }
	    buf = ଗଣ୍ଠି->rq_buffer;

	    // Convert NUL to NL, the internal representation.
	    ପାଇଁ (p = buf; (nl == ଖାଲି || p < nl)
					    && p < buf + ଗଣ୍ଠି->rq_buflen; ++p)
		ଯଦି (*p == NUL)
		    *p = NL;

	    ଯଦି (nl == ଖାଲି)
	    {
		// get the whole buffer, drop the NL
		ସନ୍ଦେଶ = channel_get(channel, part, ଖାଲି);
	    }
	    ଅନ୍ୟଥା ଯଦି (nl + 1 == buf + ଗଣ୍ଠି->rq_buflen)
	    {
		// get the whole buffer
		ସନ୍ଦେଶ = channel_get(channel, part, ଖାଲି);
		*nl = NUL;
	    }
	    ଅନ୍ୟଥା
	    {
		// Copy the message into allocated memory (excluding the NL)
		// and remove it from the buffer (including the NL).
		ସନ୍ଦେଶ = vim_strnsave(buf, nl - buf);
		channel_consume(channel, part, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(nl - buf) + 1);
	    }
	}
	ଅନ୍ୟଥା
	{
	    // For a raw channel we don't know where the message ends, just
	    // get everything we have.
	    raw_len = 0;
	    ସନ୍ଦେଶ = channel_get_all(channel, part,
			ch_mode == CH_MODE_BLOB ? &raw_len : ଖାଲି);
	}

	ଯଦି (ସନ୍ଦେଶ == ଖାଲି)
	    ଫେରନ୍ତୁ FALSE; // out of memory (and avoids Coverity warning)

	ଯଦି (ch_mode == CH_MODE_BLOB)
	{
	    blob = blob_alloc();
	    ଯଦି (blob == ଖାଲି)
	    {
		vim_free(ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ FALSE;
	    }
	    ଯଦି (ga_grow(&blob->bv_ga, raw_len) == FAIL)
	    {
		blob_free(blob);
		vim_free(ସନ୍ଦେଶ);
		ଫେରନ୍ତୁ FALSE;
	    }
	    ଯଦି (raw_len > 0)
	    {
		mch_memmove(blob->bv_ga.ga_data, ସନ୍ଦେଶ, (size_t)raw_len);
		blob->bv_ga.ga_len = raw_len;
	    }
	    argv[1].v_type = VAR_BLOB;
	    argv[1].vval.v_blob = blob;
	    ++blob->bv_refcount;
	}
	ଅନ୍ୟଥା
	{
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = ସନ୍ଦେଶ;
	}
    }

    called_otc = FALSE;
    ଯଦି (seq_nr > 0)
    {
	// JSON or JS or LSP or DAP mode: invoke the one-time callback with the
	// matching nr
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା lsp_req_ସନ୍ଦେଶ = FALSE;

	// Don't use a LSP/DAP server request message with the same sequence
	// number as the client request message as the response message.
	ଯଦି ((ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
		&& argv[1].v_type == VAR_DICT
		&& dict_has_ଚାବି(argv[1].vval.v_dict, "method"))
	    lsp_req_ସନ୍ଦେଶ = TRUE;

	ଯଦି (!lsp_req_ସନ୍ଦେଶ)
	{
	    ପାଇଁ (cbitem = cbhead->cq_next; cbitem != ଖାଲି;
		    cbitem = cbitem->cq_next)
	    {
		ଯଦି (cbitem->cq_seq_nr == seq_nr)
		{
		    invoke_one_time_callback(channel, cbhead, cbitem, argv);
		    called_otc = TRUE;
		    ଭାଙ୍ଗ;
		}
	    }
	}
    }

    ଯଦି (seq_nr > 0 && ((ch_mode != CH_MODE_LSP && ch_mode != CH_MODE_DAP)
		|| called_otc))
    {
	ଯଦି (!called_otc)
	{
	    // If the 'drop' channel attribute is set to 'never' or if
	    // ch_evalexpr() is waiting for this response message, then don't
	    // drop this message.
	    ଯଦି (channel->ch_drop_never)
	    {
		// message must be read with ch_read()
		channel_ଠେଲ_json(channel, part, listtv);

		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ସଂଖ୍ୟା;
		free_tv(listtv);
		listtv = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
		ch_log(channel, "Dropping message %d without callback",
								       seq_nr);
	}
    }
    ଅନ୍ୟଥା ଯଦି (callback != ଖାଲି || buffer != ଖାଲି)
    {
	ଯଦି (buffer != ଖାଲି)
	{
	    ଯଦି (ସନ୍ଦେଶ == ଖାଲି)
		// JSON or JS mode: re-encode the message.
		ସନ୍ଦେଶ = json_encode(listtv, ch_mode);
	    ଯଦି (ସନ୍ଦେଶ != ଖାଲି)
	    {
#ifdef FEAT_TERMINAL
		ଯଦି (buffer->b_term != ଖାଲି)
		{
		    // PART_ERR data arrives from a pipe without PTY ONLCR
		    // conversion, so lone NL must be converted to CR+NL to
		    // prevent unexpected indentation in the terminal display.
		    ଯଦି (part == PART_ERR)
		    {
			char_u *cp;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା    extra = 0;

			ପାଇଁ (cp = ସନ୍ଦେଶ; *cp != NUL; ++cp)
			    ଯଦି (*cp == NL && (cp == ସନ୍ଦେଶ || cp[-1] != CAR))
				++extra;
			ଯଦି (extra > 0)
			{
			    char_u *crlf_ସନ୍ଦେଶ = alloc(STRLEN(ସନ୍ଦେଶ) + extra + 1);

			    ଯଦି (crlf_ସନ୍ଦେଶ != ଖାଲି)
			    {
				char_u *q = crlf_ସନ୍ଦେଶ;

				ପାଇଁ (cp = ସନ୍ଦେଶ; *cp != NUL; ++cp)
				{
				    ଯଦି (*cp == NL && (cp == ସନ୍ଦେଶ || cp[-1] != CAR))
					*q++ = CAR;
				    *q++ = *cp;
				}
				*q = NUL;
				write_to_term(buffer, crlf_ସନ୍ଦେଶ, channel);
				vim_free(crlf_ସନ୍ଦେଶ);
			    }
			}
			ଅନ୍ୟଥା
			    write_to_term(buffer, ସନ୍ଦେଶ, channel);
		    }
		    ଅନ୍ୟଥା
			write_to_term(buffer, ସନ୍ଦେଶ, channel);
		}
		ଅନ୍ୟଥା
#endif
		    append_to_buffer(buffer, ସନ୍ଦେଶ, channel, part);
	    }
	}

	ଯଦି (callback != ଖାଲି)
	{
	    ଯଦି (cbitem != ଖାଲି)
		invoke_one_time_callback(channel, cbhead, cbitem, argv);
	    ଅନ୍ୟଥା
	    {
		// invoke the channel callback
		ch_log(channel, "Invoking channel callback %s",
						    (ଅକ୍ଷର *)callback->cb_name);
		invoke_callback(channel, callback, argv);
	    }
	}
    }
    ଅନ୍ୟଥା
	ch_log(channel, "Dropping message %d", seq_nr);

    ଯଦି (listtv != ଖାଲି)
	free_tv(listtv);
    ଯଦି (blob != ଖାଲି)
	blob_unref(blob);
    vim_free(ସନ୍ଦେଶ);

    ଫେରନ୍ତୁ TRUE;
}

#ଯଦି defined(FEAT_NETBEANS_INTG)
/*
 * Return TRUE when channel "channel" is open for writing to.
 * Also returns FALSE or invalid "channel".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_can_write_to(channel_T *channel)
{
    ଫେରନ୍ତୁ channel != ଖାଲି && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD);
}
#endif

/*
 * Return TRUE when channel "channel" is open for reading or writing.
 * Also returns FALSE for invalid "channel".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_is_open(channel_T *channel)
{
    ଫେରନ୍ତୁ channel != ଖାଲି && (channel->CH_SOCK_FD != INVALID_FD
			  || channel->CH_IN_FD != INVALID_FD
			  || channel->CH_OUT_FD != INVALID_FD
			  || channel->CH_ERR_FD != INVALID_FD);
}

/*
 * Return a pointer indicating the readahead.  Can only be compared between
 * calls.  Returns NULL if there is no readahead.
 */
    ସ୍ଥିର ଶୂନ୍ୟ *
channel_readahead_pointer(channel_T *channel, ch_part_T part)
{
    ଯଦି (channel_use_json_head(channel, part))
    {
	jsonq_T   *head = &channel->ch_part[part].ch_json_head;

	ଯଦି (head->jq_next == ଖାଲି)
	    // Parse json from readahead, there might be a complete message to
	    // process.
	    channel_parse_json(channel, part, false);

	ଫେରନ୍ତୁ head->jq_next;
    }
    ଫେରନ୍ତୁ channel_peek(channel, part);
}

/*
 * Return TRUE if "channel" has JSON or other typeahead.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_has_readahead(channel_T *channel, ch_part_T part)
{
    ଫେରନ୍ତୁ channel_readahead_pointer(channel, part) != ଖାଲି;
}

/*
 * Return a string indicating the status of the channel.
 * If "req_part" is not negative check that part.
 */
    ସ୍ଥିର ଅକ୍ଷର *
channel_status(channel_T *channel, ପୂର୍ଣ୍ଣସଂଖ୍ୟା req_part)
{
    ch_part_T part;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା has_readahead = FALSE;

    ଯଦି (channel == ଖାଲି)
	 ଫେରନ୍ତୁ "fail";
    ଯଦି (req_part == PART_OUT)
    {
	ଯଦି (channel->CH_OUT_FD != INVALID_FD)
	    ଫେରନ୍ତୁ "open";
	ଯଦି (channel_has_readahead(channel, PART_OUT))
	    has_readahead = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (req_part == PART_ERR)
    {
	ଯଦି (channel->CH_ERR_FD != INVALID_FD)
	    ଫେରନ୍ତୁ "open";
	ଯଦି (channel_has_readahead(channel, PART_ERR))
	    has_readahead = TRUE;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (channel_is_open(channel))
	    ଫେରନ୍ତୁ "open";
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	    ଯଦି (channel_has_readahead(channel, part))
	    {
		has_readahead = TRUE;
		ଭାଙ୍ଗ;
	    }
    }

    ଯଦି (has_readahead)
	ଫେରନ୍ତୁ "buffered";
    ଫେରନ୍ତୁ "closed";
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_part_info(channel_T *channel, dict_T *dict, ଅକ୍ଷର *name, ch_part_T part)
{
    chanpart_T *chanpart = &channel->ch_part[part];
    ଅକ୍ଷର	namebuf[20];  // longest is "sock_timeout"
    size_t	tail;
    string_T	s;

    vim_strncpy((char_u *)namebuf, (char_u *)name, 4);
    tail = STRLEN(namebuf);
    STRCPY(namebuf + tail, "_");
    tail += STRLEN_LITERAL("_");

    STRCPY(namebuf + tail, "status");
    ଯଦି (chanpart->ch_fd != INVALID_FD)
	STR_LITERAL_SET(s, "open");
    ଅନ୍ୟଥା ଯଦି (channel_has_readahead(channel, part))
	STR_LITERAL_SET(s, "buffered");
    ଅନ୍ୟଥା
	STR_LITERAL_SET(s, "closed");
    dict_add_string_len(dict, namebuf, s.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s.length);

    STRCPY(namebuf + tail, "mode");
    ବାଛ (chanpart->ch_mode)
    {
	ସ୍ଥିତି CH_MODE_NL:
	    STR_LITERAL_SET(s, "NL");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CH_MODE_RAW:
	    STR_LITERAL_SET(s, "RAW");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CH_MODE_BLOB:
	    STR_LITERAL_SET(s, "BLOB");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CH_MODE_JSON:
	    STR_LITERAL_SET(s, "JSON");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CH_MODE_JS:
	    STR_LITERAL_SET(s, "JS");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CH_MODE_LSP:
	    STR_LITERAL_SET(s, "LSP");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CH_MODE_DAP:
	    STR_LITERAL_SET(s, "DAP");
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    STR_LITERAL_SET(s, "");
	    ଭାଙ୍ଗ;
    }
    dict_add_string_len(dict, namebuf, s.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s.length);

    STRCPY(namebuf + tail, "io");
    ଯଦି (part == PART_SOCK)
	STR_LITERAL_SET(s, "socket");
    ଅନ୍ୟଥା ବାଛ (chanpart->ch_io)
    {
	ସ୍ଥିତି JIO_NULL:
	    STR_LITERAL_SET(s, "null");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି JIO_PIPE:
	    STR_LITERAL_SET(s, "pipe");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି JIO_FILE:
	    STR_LITERAL_SET(s, "file");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି JIO_BUFFER:
	    STR_LITERAL_SET(s, "buffer");
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି JIO_OUT:
	    STR_LITERAL_SET(s, "out");
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    STR_LITERAL_SET(s, "");
	    ଭାଙ୍ଗ;
    }
    dict_add_string_len(dict, namebuf, s.string, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s.length);

    STRCPY(namebuf + tail, "timeout");
    dict_add_ସଂଖ୍ୟା(dict, namebuf, chanpart->ch_timeout);
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_info(channel_T *channel, dict_T *dict)
{
    dict_add_ସଂଖ୍ୟା(dict, "id", channel->ch_id);
    dict_add_string(dict, "status", (char_u *)channel_status(channel, -1));

    ଯଦି (channel->ch_hostname != ଖାଲି)
    {
	ଯଦି (channel->ch_port)
	    dict_add_ସଂଖ୍ୟା(dict, "port", channel->ch_port);
	ଅନ୍ୟଥା
	    // Unix-domain socket.
	    dict_add_string(dict, "path", (char_u *)channel->ch_hostname);
	channel_part_info(channel, dict, "sock", PART_SOCK);
    }
    ଅନ୍ୟଥା
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
    ଶୂନ୍ୟ
channel_close(channel_T *channel, ପୂର୍ଣ୍ଣସଂଖ୍ୟା invoke_close_cb)
{
    ch_log(channel, "Closing channel");

#ifdef FEAT_GUI
    channel_gui_unregister(channel);
#endif

    ch_close_part(channel, PART_SOCK);
    ch_close_part(channel, PART_IN);
    ch_close_part(channel, PART_OUT);
    ch_close_part(channel, PART_ERR);

    ଯଦି (invoke_close_cb)
    {
	ch_part_T	part;

#ifdef FEAT_TERMINAL
	// let the terminal know it is closing to avoid getting stuck
	term_channel_closing(channel);
#endif
	// Invoke callbacks and flush buffers before the close callback.
	ଯଦି (channel->ch_close_cb.cb_name != ଖାଲି)
	    ch_log(channel,
		     "Invoking callbacks and flushing buffers before closing");
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    ଯଦି (channel->ch_close_cb.cb_name != ଖାଲି
			    || channel->ch_part[part].ch_bufref.br_buf != ଖାଲି)
	    {
		// Increment the refcount to avoid the channel being freed
		// halfway.
		++channel->ch_refcount;
		ଯଦି (channel->ch_close_cb.cb_name == ଖାଲି)
		    ch_log(channel, "flushing %s buffers before closing",
							  ch_part_names[part]);
		ଯେତେବେଳେ (may_invoke_callback(channel, part))
		    ;
		--channel->ch_refcount;
	    }
	}

	ଯଦି (channel->ch_close_cb.cb_name != ଖାଲି)
	{
	      typval_T	argv[1];
	      typval_T	rettv;

	      // Increment the refcount to avoid the channel being freed
	      // halfway.
	      ++channel->ch_refcount;
	      ch_log(channel, "Invoking close callback %s",
					 (ଅକ୍ଷର *)channel->ch_close_cb.cb_name);
	      argv[0].v_type = VAR_CHANNEL;
	      argv[0].vval.v_channel = channel;
	      call_callback(&channel->ch_close_cb, -1, &rettv, 1, argv);
	      clear_tv(&rettv);
	      channel_need_redraw = TRUE;

	      // the callback is only called once
	      free_callback(&channel->ch_close_cb);

	      ଯଦି (channel_need_redraw)
	      {
		  channel_need_redraw = FALSE;
		  redraw_after_callback(TRUE, FALSE);
	      }

	      ଯଦି (!channel->ch_drop_never)
		  // any remaining messages are useless now
		  ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
		      drop_messages(channel, part);

	      --channel->ch_refcount;
	}
    }

    channel->ch_nb_close_cb = ଖାଲି;
#ifdef FEAT_SOCKETSERVER
    channel->ch_socketserver = false;
    channel->ch_ss_accept_cb = ଖାଲି;
    channel->ch_ss_close_cb = ଖାଲି;
#endif

#ifdef FEAT_TERMINAL
    term_channel_closed(channel);
#endif
}

/*
 * Close the "in" part channel "channel".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_close_in(channel_T *channel)
{
    ch_close_part(channel, PART_IN);
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_from_writeque(writeq_T *wq, writeq_T *entry)
{
    ga_clear(&entry->wq_ga);
    wq->wq_next = entry->wq_next;
    ଯଦି (wq->wq_next == ଖାଲି)
	wq->wq_prev = ଖାଲି;
    ଅନ୍ୟଥା
	wq->wq_next->wq_prev = ଖାଲି;
    vim_free(entry);
}

/*
 * Clear the read buffer on "channel"/"part".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_clear_one(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    jsonq_T *json_head = &ch_part->ch_json_head;
    cbq_T   *cb_head = &ch_part->ch_cb_head;

    ଯେତେବେଳେ (channel_peek(channel, part) != ଖାଲି)
	vim_free(channel_get(channel, part, ଖାଲି));

    ଯେତେବେଳେ (cb_head->cq_next != ଖାଲି)
    {
	cbq_T *ଗଣ୍ଠି = cb_head->cq_next;

	remove_cb_ଗଣ୍ଠି(cb_head, ଗଣ୍ଠି);
	free_callback(&ଗଣ୍ଠି->cq_callback);
	vim_free(ଗଣ୍ଠି);
    }

    ଯେତେବେଳେ (json_head->jq_next != ଖାଲି)
    {
	free_tv(json_head->jq_next->jq_ମୂଲ୍ୟ);
	remove_json_ଗଣ୍ଠି(json_head, json_head->jq_next);
    }

    free_callback(&ch_part->ch_callback);
    ga_clear(&ch_part->ch_block_ids);

    ଯେତେବେଳେ (ch_part->ch_writeque.wq_next != ଖାଲି)
	remove_from_writeque(&ch_part->ch_writeque,
						 ch_part->ch_writeque.wq_next);
}

/*
 * Clear all the read buffers on "channel".
 */
    ଶୂନ୍ୟ
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

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
channel_free_all(ଶୂନ୍ୟ)
{
    channel_T *channel;

    ch_log(ଖାଲି, "channel_free_all()");
    FOR_ALL_CHANNELS(channel)
	channel_clear(channel);
}
#endif


// Sent when the netbeans channel is found closed when reading.
#define DETACH_ସନ୍ଦେଶ_RAW "DETACH\n"

// Buffer size for reading incoming messages.
#define MAXMSGSIZE 4096

/*
 * Check if there are remaining data that should be written for "in_part".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
is_channel_write_remaining(chanpart_T *in_part)
{
    buf_T *buf = in_part->ch_bufref.br_buf;

    ଯଦି (in_part->ch_writeque.wq_next != ଖାଲି)
	ଫେରନ୍ତୁ TRUE;
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ in_part->ch_buf_append
	    ? (in_part->ch_buf_bot < buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    : (in_part->ch_buf_ଶୀର୍ଷ <= in_part->ch_buf_bot
			    && in_part->ch_buf_ଶୀର୍ଷ <= buf->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
}

#ଯଦି defined(HAVE_SELECT)
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_fill_wfds(ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxfd_arg, fd_set *wfds)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxfd = maxfd_arg;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ଯଦି (in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		&& (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)in_part->ch_fd < FD_SETSIZE
# endif
		&& is_channel_write_remaining(in_part))
	{
	    FD_SET((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)in_part->ch_fd, wfds);
	    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)in_part->ch_fd >= maxfd)
		maxfd = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)in_part->ch_fd + 1;
	}
    }
    ଫେରନ୍ତୁ maxfd;
}
#ଅନ୍ୟଥା
/*
 * Add write fds where we are waiting for writing to be possible.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_fill_poll_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfd_in, ସଂରଚନା pollfd *fds)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nfd = nfd_in;
    channel_T	*ch;

    FOR_ALL_CHANNELS(ch)
    {
	chanpart_T  *in_part = &ch->ch_part[PART_IN];

	ଯଦି (in_part->ch_fd != INVALID_FD
		&& is_channel_write_remaining(in_part))
	{
	    in_part->ch_poll_idx = nfd;
	    fds[nfd].fd = in_part->ch_fd;
	    fds[nfd].events = POLLOUT;
	    ++nfd;
	}
	ଅନ୍ୟଥା
	    in_part->ch_poll_idx = -1;
    }
    ଫେରନ୍ତୁ nfd;
}
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
    ସ୍ଥିର channel_wait_result
channel_wait(channel_T *channel, sock_T fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout)
{
    ଯଦି (timeout > 0)
	ch_log(channel, "Waiting for up to %d msec", timeout);

#ifdef MSWIN
    ଯଦି (fd != channel->CH_SOCK_FD)
    {
	DWORD	nread;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sleep_time;
	DWORD	deadline = Get_Tick_ଗଣନା() + timeout;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	delay = 1;

	// reading from a pipe, not a socket
	ଯେତେବେଳେ (TRUE)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = PeekNamedPipe((HANDLE)fd, ଖାଲି, 0, ଖାଲି, &nread, ଖାଲି);

	    ଯଦି (r && nread > 0)
		ଫେରନ୍ତୁ CW_READY;

	    ଯଦି (channel->ch_named_pipe)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ଖାଲି);
	    }
	    ଅନ୍ୟଥା ଯଦି (r == 0)
		ଫେରନ୍ତୁ CW_ERROR;

	    // perhaps write some buffer lines
	    channel_write_any_ଧାଡ଼ିଗୁଡ଼ିକ();

	    sleep_time = deadline - Get_Tick_ଗଣନା();
	    ଯଦି (sleep_time <= 0)
		ଭାଙ୍ଗ;
	    // Wait for a little while.  Very short at first, up to 10 msec
	    // after looping a few times.
	    ଯଦି (sleep_time > delay)
		sleep_time = delay;
	    Sleep(sleep_time);
	    delay = delay * 2;
	    ଯଦି (delay > 10)
		delay = 10;
	}
    }
    ଅନ୍ୟଥା
#endif
    {
#ଯଦି defined(HAVE_SELECT)
	ସଂରଚନା timeval	tval;
	fd_set		rfds;
	fd_set		wfds;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxfd;

	tval.tv_sec = timeout / 1000;
	tval.tv_usec = (timeout % 1000) * 1000;
	ପାଇଁ (;;)
	{
	    FD_ZERO(&rfds);
	    FD_SET((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fd, &rfds);

	    // Write lines to a pipe when a pipe can be written to.  Need to
	    // set this every time, some buffers may be done.
	    maxfd = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fd + 1;
	    FD_ZERO(&wfds);
	    maxfd = channel_fill_wfds(maxfd, &wfds);

	    ret = select(maxfd, &rfds, &wfds, ଖାଲି, &tval);
# ifdef EINTR
	    SOCK_ERRNO;
	    ଯଦି (ret == -1 && errno == EINTR)
		ଜାରି;
# endif
	    ଯଦି (ret > 0)
	    {
		ଯଦି (FD_ISSET(fd, &rfds))
		    ଫେରନ୍ତୁ CW_READY;
		channel_write_any_ଧାଡ଼ିଗୁଡ଼ିକ();
		ଜାରି;
	    }
	    ଭାଙ୍ଗ;
	}
#ଅନ୍ୟଥା
	ପାଇଁ (;;)
	{
	    ସଂରଚନା pollfd   fds[MAX_OPEN_CHANNELS + 1 + MAX_CLIENT_CHANNELS];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    nfd = 1;

	    fds[0].fd = fd;
	    fds[0].events = POLLIN;
	    nfd = channel_fill_poll_write(nfd, fds);
	    ଯଦି (poll(fds, nfd, timeout) > 0)
	    {
		ଯଦି (fds[0].revents & POLLIN)
		    ଫେରନ୍ତୁ CW_READY;
		channel_write_any_ଧାଡ଼ିଗୁଡ଼ିକ();
		ଜାରି;
	    }
	    ଭାଙ୍ଗ;
	}
#endif
    }
    ଫେରନ୍ତୁ CW_NOT_READY;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ch_close_part_on_error(
	channel_T *channel, ch_part_T part, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_err, ଅକ୍ଷର *func)
{
    ଅକ୍ଷର	ସନ୍ଦେଶ[] = "%s(): Read %s from ch_part[%d], closing";

    ଯଦି (is_err)
	// Do not call emsg(), most likely the other end just exited.
	ch_error(channel, ସନ୍ଦେଶ, func, "error", part);
    ଅନ୍ୟଥା
	ch_log(channel, ସନ୍ଦେଶ, func, "EOF", part);

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
    ଯଦି (channel->ch_nb_close_cb != ଖାଲି)
	channel_save(channel, PART_SOCK, (char_u *)DETACH_ସନ୍ଦେଶ_RAW,
			      (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN_LITERAL(DETACH_ସନ୍ଦେଶ_RAW), FALSE, "PUT ");

    // When reading is not possible close this part of the channel.  Don't
    // close the channel yet, there may be something to read on another part.
    // When stdout and stderr use the same FD we get the error only on one of
    // them, also close the other.
    ଯଦି (part == PART_OUT || part == PART_ERR)
    {
	ch_part_T other = part == PART_OUT ? PART_ERR : PART_OUT;

	ଯଦି (channel->ch_part[part].ch_fd == channel->ch_part[other].ch_fd)
	    ch_close_part(channel, other);
    }
    ch_close_part(channel, part);

#ifdef FEAT_GUI
    // Stop listening to GUI events right away.
    channel_gui_unregister_one(channel, part);
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
channel_close_now(channel_T *channel)
{
    ch_log(channel, "Closing channel because all readable fds are closed");
    ଯଦି (channel->ch_nb_close_cb != ଖାଲି)
	(*channel->ch_nb_close_cb)();
#ifdef FEAT_SOCKETSERVER
    ଯଦି (channel->ch_ss_close_cb != ଖାଲି)
	channel->ch_ss_close_cb(channel);
#endif
    channel_close(channel, TRUE);
}

/*
 * Read from channel "channel" for as long as there is something to read.
 * "part" is PART_SOCK, PART_OUT or PART_ERR.
 * The data is put in the read queue.  No callbacks are invoked here.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_read(channel_T *channel, ch_part_T part, ଅକ୍ଷର *func)
{
    ସ୍ଥିର char_u	*buf = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			len = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			readlen = 0;
    sock_T		fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			use_socket = FALSE;

    fd = channel->ch_part[part].ch_fd;
    ଯଦି (fd == INVALID_FD)
    {
	ch_error(channel, "channel_read() called while %s part is closed",
							  ch_part_names[part]);
	ଫେରନ୍ତୁ;
    }
    use_socket = fd == channel->CH_SOCK_FD;

    // Allocate a buffer to read into.
    ଯଦି (buf == ଖାଲି)
    {
	buf = alloc(MAXMSGSIZE);
	ଯଦି (buf == ଖାଲି)
	    ଫେରନ୍ତୁ;	// out of memory!
    }

    // Keep on reading for as long as there is something to read.
    // Use select() or poll() to avoid blocking on a message that is exactly
    // MAXMSGSIZE long.
    ପାଇଁ (;;)
    {
	ଯଦି (channel_wait(channel, fd, 0) != CW_READY)
	    ଭାଙ୍ଗ;
	ଯଦି (channel->ch_listen)
	{
	    sock_T		newfd;
	    socklen_t		socklen;
	    channel_T		*newchannel;
	    typval_T		argv[2];
	    char_u		namebuf[256];
	    ସଂରଚନା sockaddr_storage	client;

	    newchannel = add_channel();
	    ଯଦି (newchannel == ଖାଲି)
	    {
		ch_error(ଖାଲି, "Cannot allocate channel.");
		ଫେରନ୍ତୁ;
	    }
	    socklen = ଆକାର(client);
	    newfd = accept(fd, (ସଂରଚନା sockaddr*)&client, &socklen);
	    ଯଦି (newfd < 0)
	    {
		ch_error(ଖାଲି, "Cannot accept channel.");
		channel_free(newchannel);
		ଫେରନ୍ତୁ;
	    }
	    newchannel->CH_SOCK_FD = (sock_T)newfd;
	    newchannel->ch_to_be_closed |= (1U << PART_SOCK);

#ifdef FEAT_GUI
	    channel_gui_register_one(newchannel, PART_SOCK);
#endif

#ifdef FEAT_SOCKETSERVER
	    ଯଦି (channel->ch_ss_accept_cb != ଖାଲି)
	    {
		channel->ch_ss_accept_cb(newchannel);
		ଫେରନ୍ତୁ;
	    }
#endif

	    ଯଦି (client.ss_family == AF_INET)
	    {
#ifdef HAVE_INET_NTOP
		ଅକ୍ଷର addr[INET_ADDRSTRLEN];

		inet_ntop(AF_INET,
			&((ସଂରଚନା sockaddr_in*)&client)->sin_addr,
			addr, ଆକାର(addr));
		vim_snprintf((ଅକ୍ଷର *)namebuf, ଆକାର(namebuf), "%s:%d",
			addr,
			ntohs(((ସଂରଚନା sockaddr_in*)&client)->sin_port));
#ଅନ୍ୟଥା
		vim_snprintf((ଅକ୍ଷର *)namebuf, ଆକାର(namebuf), "%s:%d",
		    inet_ntoa(((ସଂରଚନା sockaddr_in*)&client)->sin_addr),
		    ntohs(((ସଂରଚନା sockaddr_in*)&client)->sin_port));
#endif
	    }
#ifdef HAVE_INET_NTOP
	    ଅନ୍ୟଥା ଯଦି (client.ss_family == AF_INET6)
	    {
		ଅକ୍ଷର addr[INET6_ADDRSTRLEN];

		inet_ntop(AF_INET6,
			&((ସଂରଚନା sockaddr_in6*)&client)->sin6_addr,
			addr, ଆକାର(addr));
		vim_snprintf((ଅକ୍ଷର *)namebuf, ଆକାର(namebuf), "[%s]:%d",
			addr,
			ntohs(((ସଂରଚନା sockaddr_in6*)&client)->sin6_port));
	    }
#endif
	    ଅନ୍ୟଥା ଯଦି (client.ss_family == AF_UNIX)
		vim_snprintf((ଅକ୍ଷର *)namebuf, ଆକାର(namebuf),
							    "unix:anonymous");
	    ଅନ୍ୟଥା
		vim_snprintf((ଅକ୍ଷର *)namebuf, ଆକାର(namebuf), "unknown");
	    ++safe_to_invoke_callback;
	    ++newchannel->ch_refcount;
	    argv[0].v_type = VAR_CHANNEL;
	    argv[0].vval.v_channel = newchannel;
	    argv[1].v_type = VAR_STRING;
	    argv[1].vval.v_string = vim_strsave(namebuf);
	    invoke_callback(newchannel, &channel->ch_callback, argv);
	    --safe_to_invoke_callback;
	    clear_tv(&argv[1]);
	    ଫେରନ୍ତୁ;
	}
	ଯଦି (use_socket)
	    len = sock_read(fd, (ଅକ୍ଷର *)buf, MAXMSGSIZE);
	ଅନ୍ୟଥା
	    len = fd_read(fd, (ଅକ୍ଷର *)buf, MAXMSGSIZE);
	ଯଦି (len <= 0)
	    ଭାଙ୍ଗ;	// error or nothing more to read

	// Store the read message in the queue.
	channel_save(channel, part, buf, len, FALSE, "RECV ");
	readlen += len;
    }

    // Reading a disconnection (readlen == 0), or an error.
    ଯଦି (readlen <= 0)
    {
	ଯଦି (!channel->ch_keep_open)
	    ch_close_part_on_error(channel, part, (len < 0), func);
    }
#ଯଦି defined(CH_HAS_GUI) && defined(FEAT_GUI_GTK)
    ଅନ୍ୟଥା ଯଦି (CH_HAS_GUI && gtk_main_level() > 0)
	// signal the main loop that there is something to read
	gtk_main_quit();
#endif
}

#ifdef FEAT_SOCKETSERVER

    ଶୂନ୍ୟ
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
    ସ୍ଥିର char_u *
channel_read_block(
	channel_T *channel, ch_part_T part, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout, ପୂର୍ଣ୍ଣସଂଖ୍ୟା raw, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *outlen)
{
    char_u	*buf;
    char_u	*ସନ୍ଦେଶ;
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    sock_T	fd = channel->ch_part[part].ch_fd;
    char_u	*nl;
    readq_T	*ଗଣ୍ଠି;

    ch_log(channel, "Blocking %s read, timeout: %d msec",
				  mode == CH_MODE_RAW ? "RAW"
				: mode == CH_MODE_BLOB ? "BLOB" : "NL", timeout);

    ଯେତେବେଳେ (TRUE)
    {
	ଗଣ୍ଠି = channel_peek(channel, part);
	ଯଦି (ଗଣ୍ଠି != ଖାଲି)
	{
	    ଯଦି (mode == CH_MODE_RAW || mode == CH_MODE_BLOB
		    || (mode == CH_MODE_NL
					   && channel_first_nl(ଗଣ୍ଠି) != ଖାଲି))
		// got a complete message
		ଭାଙ୍ଗ;
	    ଯଦି (channel_collapse(channel, part, mode == CH_MODE_NL) == OK)
		ଜାରି;
	    // If not blocking or nothing more is coming then return what we
	    // have.
	    ଯଦି (raw || fd == INVALID_FD)
		ଭାଙ୍ଗ;
	}

	// Wait for up to the channel timeout.
	ଯଦି (fd == INVALID_FD)
	    ଫେରନ୍ତୁ ଖାଲି;
	ଯଦି (channel_wait(channel, fd, timeout) != CW_READY)
	{
	    ch_log(channel, "Timed out");
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	channel_read(channel, part, "channel_read_block");
    }

    // We have a complete message now.
    ଯଦି (mode == CH_MODE_RAW || mode == CH_MODE_BLOB || outlen != ଖାଲି)
    {
	ସନ୍ଦେଶ = channel_get_all(channel, part, outlen);
    }
    ଅନ୍ୟଥା
    {
	char_u *p;

	buf = ଗଣ୍ଠି->rq_buffer;
	nl = channel_first_nl(ଗଣ୍ଠି);

	// Convert NUL to NL, the internal representation.
	ପାଇଁ (p = buf; (nl == ଖାଲି || p < nl) && p < buf + ଗଣ୍ଠି->rq_buflen; ++p)
	    ଯଦି (*p == NUL)
		*p = NL;

	ଯଦି (nl == ଖାଲି)
	{
	    // must be a closed channel with missing NL
	    ସନ୍ଦେଶ = channel_get(channel, part, ଖାଲି);
	}
	ଅନ୍ୟଥା ଯଦି (nl + 1 == buf + ଗଣ୍ଠି->rq_buflen)
	{
	    // get the whole buffer
	    ସନ୍ଦେଶ = channel_get(channel, part, ଖାଲି);
	    *nl = NUL;
	}
	ଅନ୍ୟଥା
	{
	    // Copy the message into allocated memory and remove it from the
	    // buffer.
	    ସନ୍ଦେଶ = vim_strnsave(buf, nl - buf);
	    channel_consume(channel, part, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(nl - buf) + 1);
	}
    }
    ଯଦି (ch_log_active())
	ch_log(channel, "Returning %d bytes",
		outlen != ଖାଲି ? *outlen : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ସନ୍ଦେଶ));
    ଫେରନ୍ତୁ ସନ୍ଦେଶ;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା channel_blocking_wait = 0;

/*
 * Return TRUE if in a blocking wait that might trigger callbacks.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_in_blocking_wait(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ channel_blocking_wait > 0;
}

/*
 * Wait up to "timeout" msec for something to read on "fd" of "channel", and
 * read what arrives on any channel meanwhile.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_wait_reading(channel_T *channel UNUSED, sock_T fd UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it; the other
    // channels are looked at every 10 msec.
    (ଶୂନ୍ୟ)channel_wait(channel, fd, timeout > 10 ? 10 : timeout);
    channel_handle_events(FALSE);
#ଅନ୍ୟଥା
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_read_json_block(
	channel_T   *channel,
	ch_part_T   part,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    timeout_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id,
	typval_T    **rettv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		more;
    sock_T	fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timeout;
    chanpart_T	*chanpart = &channel->ch_part[part];
    ch_mode_T	mode = channel->ch_part[part].ch_mode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
#ifdef ELAPSED_FUNC
    elapsed_T	start_tv;

    ELAPSED_INIT(start_tv);
#endif

    ch_log(channel, "Blocking read JSON for id %d", id);
    ++channel_blocking_wait;

    ଯଦି (id >= 0)
	channel_add_block_id(chanpart, id);

    ପାଇଁ (;;)
    {
	ଯଦି (mode == CH_MODE_LSP || mode == CH_MODE_DAP)
	    // In the "lsp" or "dap" mode, the http header and the json payload
	    // may be received in multiple messages. So concatenate all the
	    // received messages.
	    (ଶୂନ୍ୟ)channel_collapse(channel, part, FALSE);

	more = channel_parse_json(channel, part, false);

	// search for message "id"
	ଯଦି (channel_get_json(channel, part, id, TRUE, rettv) == OK)
	{
	    ch_log(channel, "Received JSON for id %d", id);
	    retval = OK;
	    ଭାଙ୍ଗ;
	}

	ଯଦି (!more)
	{
	    ଶୂନ୍ୟ *prev_readahead_ptr = channel_readahead_pointer(channel, part);
	    ଶୂନ୍ୟ *readahead_ptr;

	    // Handle any other messages in the queue.  If done some more
	    // messages may have arrived.
	    ଯଦି (channel_parse_messages())
		ଜାରି;

	    // channel_parse_messages() may fill the queue with new data to
	    // process.  Only loop when the readahead changed, otherwise we
	    // would busy-loop.
	    readahead_ptr = channel_readahead_pointer(channel, part);
	    ଯଦି (readahead_ptr != ଖାଲି && readahead_ptr != prev_readahead_ptr)
		ଜାରି;

	    // Wait for up to what is left of the timeout.
	    timeout = timeout_arg;
#ifdef ELAPSED_FUNC
	    timeout -= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ELAPSED_FUNC(start_tv);
#endif
	    fd = chanpart->ch_fd;
	    ଯଦି (timeout <= 0 || fd == INVALID_FD)
	    {
		ଯଦି (fd != INVALID_FD)
		    ch_log(channel, "Timed out on id %d", id);
		ଭାଙ୍ଗ;
	    }
	    // If there was an incomplete message use the deadline for that.
	    ଯଦି (chanpart->ch_wait_len > 0)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ବାମ;
#ifdef MSWIN
		ବାମ = chanpart->ch_deadline - Get_Tick_ଗଣନା() + 1;
#ଅନ୍ୟଥା
		{
		    ସଂରଚନା timeval now_tv;

		    gettimeofday(&now_tv, ଖାଲି);
		    ବାମ = (chanpart->ch_deadline.tv_sec - now_tv.tv_sec) * 1000
			+ (chanpart->ch_deadline.tv_usec - now_tv.tv_usec)
								    / 1000 + 1;
		}
#endif
		ଯଦି (ବାମ < 0)
		    // Something went wrong, channel_parse_json() didn't
		    // discard message.  Cancel waiting.
		    chanpart->ch_wait_len = 0;
		ଅନ୍ୟଥା ଯଦି (ବାମ < timeout)
		    timeout = ବାମ;
	    }
	    channel_wait_reading(channel, fd, timeout);
#ifndef ELAPSED_FUNC
	    // Without a clock the wait counts as the whole timeout.
	    timeout_arg = 0;
#endif
	}
    }
    ଯଦି (id >= 0)
	channel_remove_block_id(chanpart, id);
    --channel_blocking_wait;

    ଫେରନ୍ତୁ retval;
}

/*
 * Get the channel from the argument.
 * Returns NULL if the handle is invalid.
 * When "check_open" is TRUE check that the channel can be used.
 * When "reading" is TRUE "check_open" considers typeahead useful.
 * "part" is used to check typeahead, when PART_COUNT use the default part.
 */
    channel_T *
get_channel_arg(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_open, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reading, ch_part_T part)
{
    channel_T	*channel = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_readahead = FALSE;

    ଯଦି (tv->v_type == VAR_JOB)
    {
	ଯଦି (tv->vval.v_job != ଖାଲି)
	    channel = tv->vval.v_job->jv_channel;
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_CHANNEL)
    {
	channel = tv->vval.v_channel;
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_invalid_argument_str), tv_get_string(tv));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (channel != ଖାଲି && reading)
	has_readahead = channel_has_readahead(channel,
		       part != PART_ଗଣନା ? part : channel_part_read(channel));

    ଯଦି (check_open && (channel == ଖାଲି || (!channel_is_open(channel)
					     && !(reading && has_readahead))))
    {
	emsg(_(e_not_an_open_channel));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ channel;
}

/*
 * Common for ch_read() and ch_readraw().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
common_channel_read(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା raw, ପୂର୍ଣ୍ଣସଂଖ୍ୟା blob)
{
    channel_T	*channel;
    ch_part_T	part = PART_ଗଣନା;
    jobopt_T	opt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timeout;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id = -1;
    typval_T	*listtv = ଖାଲି;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    clear_job_options(&opt);
    ଯଦି (get_job_options(&argvars[1], &opt, JO_TIMEOUT + JO_PART + JO_ID, 0)
								      == FAIL)
	ଯାଅ theend;

    ଯଦି (opt.jo_set & JO_PART)
	part = opt.jo_part;
    channel = get_channel_arg(&argvars[0], TRUE, TRUE, part);
    ଯଦି (channel == ଖାଲି)
	ଯାଅ theend;

    ଯଦି (part == PART_ଗଣନା)
	part = channel_part_read(channel);
    mode = channel_get_mode(channel, part);
    timeout = channel_get_timeout(channel, part);
    ଯଦି (opt.jo_set & JO_TIMEOUT)
	timeout = opt.jo_timeout;

    ଯଦି (blob || mode == CH_MODE_BLOB)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    outlen = 0;
	char_u  *p = channel_read_block(channel, part,
		timeout, TRUE, &outlen);
	ଯଦି (p != ଖାଲି)
	{
	    blob_T	*b = blob_alloc();

	    ଯଦି (b != ଖାଲି)
	    {
		b->bv_ga.ga_len = outlen;
		ଯଦି (ga_grow(&b->bv_ga, outlen) == FAIL)
		    blob_free(b);
		ଅନ୍ୟଥା
		{
		    memcpy(b->bv_ga.ga_data, p, outlen);
		    rettv_blob_set(rettv, b);
		}
	    }
	    vim_free(p);
	}
    }
    ଅନ୍ୟଥା ଯଦି (raw || mode == CH_MODE_RAW || mode == CH_MODE_NL)
	rettv->vval.v_string = channel_read_block(channel, part,
		timeout, raw, ଖାଲି);
    ଅନ୍ୟଥା
    {
	ଯଦି (opt.jo_set & JO_ID)
	    id = opt.jo_id;
	channel_read_json_block(channel, part, timeout, id, &listtv);
	ଯଦି (listtv != ଖାଲି)
	{
	    *rettv = *listtv;
	    vim_free(listtv);
	}
	ଅନ୍ୟଥା
	{
	    rettv->v_type = VAR_SPECIAL;
	    rettv->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
	}
    }

theend:
    free_job_options(&opt);
}

#ଯଦି defined(MSWIN) || defined(__HAIKU__) || defined(FEAT_GUI)
/*
 * Check the channels for anything that is ready to be read.
 * The data is put in the read queue.
 * if "only_keep_open" is TRUE only check channels where ch_keep_open is set.
 */
    ଶୂନ୍ୟ
channel_handle_events(ପୂର୍ଣ୍ଣସଂଖ୍ୟା only_keep_open)
{
    channel_T	*channel;
    ch_part_T	part;
    sock_T	fd;

    FOR_ALL_CHANNELS(channel)
    {
	ଯଦି (only_keep_open && !channel->ch_keep_open)
	    ଜାରି;

	// check the socket and pipes
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    fd = channel->ch_part[part].ch_fd;
	    ଯଦି (fd == INVALID_FD)
		ଜାରି;

	    // In normal cases, a timeout of 0 is sufficient.
	    //
	    // But, in Windows conpty terminals, the final output of a
	    // terminated process may be missed.  In this case, in order for
	    // Vim to read the final output, it is necessary to set the timeout
	    // to 1 msec or more.  It seems that the final output can be
	    // received by calling Sleep() once within channel_wait().  Note
	    // that ch_killing can only be TRUE in conpty terminals, so it has
	    // no side effects in environments other than conpty.
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = channel_wait(channel, fd, (channel->ch_killing &&
			(part == PART_OUT || part == PART_ERR)) ? 1 : 0);

	    ଯଦି (r == CW_READY)
		channel_read(channel, part, "channel_handle_events");
	    ଅନ୍ୟଥା ଯଦି (r == CW_ERROR)
		ch_close_part_on_error(channel, part, TRUE,
			"channel_handle_events");
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ଯଦି (channel->ch_job != ଖାଲି)
	{
	    job_T *job = channel->ch_job;

	    ଯଦି (job->jv_tty_out != ଖାଲି && job->jv_status == JOB_FINISHED)
		ପାଇଁ (part = PART_SOCK; part < PART_ଗଣନା; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }
}
#endif

#ଯଦି defined(FEAT_GUI)
/*
 * Return TRUE when there is any channel with a keep_open flag.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_any_keep_open(ଶୂନ୍ୟ)
{
    channel_T	*channel;

    FOR_ALL_CHANNELS(channel)
	ଯଦି (channel->ch_keep_open)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}
#endif

/*
 * Set "channel"/"part" to non-blocking.
 * Only works for sockets and pipes.
 */
    ଶୂନ୍ୟ
channel_set_nonblock(channel_T *channel, ch_part_T part)
{
    chanpart_T *ch_part = &channel->ch_part[part];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd = ch_part->ch_fd;

    ଯଦି (fd == INVALID_FD)
	ଫେରନ୍ତୁ;

#ifdef MSWIN
    u_long	val = 1;

    ioctlsocket(fd, FIONBIO, &val);
#ଅନ୍ୟଥା
    (ଶୂନ୍ୟ)fcntl(fd, F_SETFL, O_NONBLOCK);
#endif
    ch_part->ch_nonblocking = TRUE;
}

/*
 * Write "buf" (NUL terminated string) to "channel"/"part".
 * When "fun" is not NULL an error message might be given.
 * Return FAIL or OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_send(
	channel_T *channel,
	ch_part_T part,
	char_u	  *buf_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	  len_arg,
	ଅକ୍ଷର	  *fun)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;
    sock_T	fd;
    chanpart_T	*ch_part = &channel->ch_part[part];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_use_queue = FALSE;

    fd = ch_part->ch_fd;
    ଯଦି (fd == INVALID_FD)
    {
	ଯଦି (!channel->ch_error && fun != ଖାଲି)
	{
	    ch_error(channel, "%s(): write while not connected", fun);
	    semsg(_(e_str_write_while_not_connected), fun);
	}
	channel->ch_error = TRUE;
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (channel->ch_nonblock && !ch_part->ch_nonblocking)
	channel_set_nonblock(channel, part);

    ଯଦି (ch_log_active())
    {
	ch_log_literal("SEND ", channel, part, buf_arg, len_arg);
	did_repeated_ସନ୍ଦେଶ = 0;
    }

    ପାଇଁ (;;)
    {
	writeq_T    *wq = &ch_part->ch_writeque;
	char_u	    *buf;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

	ଯଦି (wq->wq_next != ଖାଲି)
	{
	    // first write what was queued
	    buf = wq->wq_next->wq_ga.ga_data;
	    len = wq->wq_next->wq_ga.ga_len;
	    did_use_queue = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (len_arg == 0)
		// nothing to write, called from channel_select_check()
		ଫେରନ୍ତୁ OK;
	    buf = buf_arg;
	    len = len_arg;
	}

	ଯଦି (part == PART_SOCK)
	    res = sock_write(fd, (ଅକ୍ଷର *)buf, len);
	ଅନ୍ୟଥା
	{
	    res = fd_write(fd, (ଅକ୍ଷର *)buf, len);
#ifdef MSWIN
	    ଯଦି (channel->ch_named_pipe && res < 0)
	    {
		DisconnectNamedPipe((HANDLE)fd);
		ConnectNamedPipe((HANDLE)fd, ଖାଲି);
	    }
#endif
	}
	ଯଦି (res < 0 && (errno == EWOULDBLOCK
#ifdef EAGAIN
			|| errno == EAGAIN
#endif
		    ))
	    res = 0; // nothing got written

	ଯଦି (res >= 0 && ch_part->ch_nonblocking)
	{
	    writeq_T *entry = wq->wq_next;

	    ଯଦି (did_use_queue)
		ch_log(channel, "Sent %d bytes now", res);
	    ଯଦି (res == len)
	    {
		// Wrote all the buf[len] bytes.
		ଯଦି (entry != ଖାଲି)
		{
		    // Remove the entry from the write queue.
		    remove_from_writeque(wq, entry);
		    ଜାରି;
		}
		ଯଦି (did_use_queue)
		    ch_log(channel, "Write queue empty");
	    }
	    ଅନ୍ୟଥା
	    {
		// Wrote only buf[res] bytes, can't write more now.
		ଯଦି (entry != ଖାଲି)
		{
		    ଯଦି (res > 0)
		    {
			// Remove the bytes that were written.
			mch_memmove(entry->wq_ga.ga_data,
				    (ଅକ୍ଷର *)entry->wq_ga.ga_data + res,
				    len - res);
			entry->wq_ga.ga_len -= res;
		    }
		    buf = buf_arg;
		    len = len_arg;
		}
		ଅନ୍ୟଥା
		{
		    buf += res;
		    len -= res;
		}
		ch_log(channel, "Adding %d bytes to the write queue", len);

		// Append the not written bytes of the argument to the write
		// buffer.  Limit entries to 4000 bytes.
		ଯଦି (wq->wq_prev != ଖାଲି
			&& wq->wq_prev->wq_ga.ga_len + len < 4000)
		{
		    writeq_T *last = wq->wq_prev;

		    // append to the last entry
		    ଯଦି (len > 0 && ga_grow(&last->wq_ga, len) == OK)
		    {
			mch_memmove((ଅକ୍ଷର *)last->wq_ga.ga_data
							  + last->wq_ga.ga_len,
				    buf, len);
			last->wq_ga.ga_len += len;
		    }
		}
		ଅନ୍ୟଥା
		{
		    writeq_T *last = ALLOC_ONE(writeq_T);

		    ଯଦି (last != ଖାଲି)
		    {
			last->wq_prev = wq->wq_prev;
			last->wq_next = ଖାଲି;
			ଯଦି (wq->wq_prev == ଖାଲି)
			    wq->wq_next = last;
			ଅନ୍ୟଥା
			    wq->wq_prev->wq_next = last;
			wq->wq_prev = last;
			ga_init2(&last->wq_ga, 1, 1000);
			ଯଦି (len > 0 && ga_grow(&last->wq_ga, len) == OK)
			{
			    mch_memmove(last->wq_ga.ga_data, buf, len);
			    last->wq_ga.ga_len = len;
			}
		    }
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (res != len)
	{
	    ଯଦି (!channel->ch_error && fun != ଖାଲି)
	    {
		ch_error(channel, "%s(): write failed", fun);
		semsg(_(e_str_write_failed), fun);
	    }
	    channel->ch_error = TRUE;
	    ଫେରନ୍ତୁ FAIL;
	}

	channel->ch_error = FALSE;
	ଫେରନ୍ତୁ OK;
    }
}

/*
 * Common for "ch_sendexpr()" and "ch_sendraw()".
 * Returns the channel if the caller should read the response.
 * Sets "part_read" to the read fd.
 * Otherwise returns NULL.
 */
    ସ୍ଥିର channel_T *
send_common(
	typval_T    *argvars,
	char_u	    *text,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    id,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    eval,
	jobopt_T    *opt,
	ଅକ୍ଷର	    *fun,
	ch_part_T   *part_read)
{
    channel_T	*channel;
    ch_part_T	part_send;

    clear_job_options(opt);
    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    part_send = channel_part_send(channel);
    *part_read = channel_part_read(channel);

    ଯଦି (get_job_options(&argvars[2], opt, JO_CALLBACK + JO_TIMEOUT, 0) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    // Set the callback. An empty callback means no callback and not reading
    // the response. With "ch_evalexpr()" and "ch_evalraw()" a callback is not
    // allowed.
    ଯଦି (opt->jo_callback.cb_name != ଖାଲି && *opt->jo_callback.cb_name != NUL)
    {
	ଯଦି (eval)
	{
	    semsg(_(e_cannot_use_callback_with_str), fun);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
	channel_set_req_callback(channel, *part_read, &opt->jo_callback, id);
    }

    ଯଦି (channel_send(channel, part_send, text, len, fun) == OK
					   && opt->jo_callback.cb_name == ଖାଲି)
	ଫେରନ୍ତୁ channel;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * common for "ch_evalexpr()" and "ch_sendexpr()"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ch_ପଦସମଷ୍ଟି_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval)
{
    char_u	*text;
    typval_T	*listtv;
    channel_T	*channel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		id;
    ch_mode_T	ch_mode;
    ch_part_T	part_send;
    ch_part_T	part_read;
    jobopt_T    opt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timeout;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		callback_present = FALSE;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ;
    part_send = channel_part_send(channel);

    ch_mode = channel_get_mode(channel, part_send);
    ଯଦି (ch_mode == CH_MODE_RAW || ch_mode == CH_MODE_BLOB
	    || ch_mode == CH_MODE_NL)
    {
	emsg(_(e_cannot_use_evalexpr_sendexpr_with_raw_nl_or_blob_channel));
	ଫେରନ୍ତୁ;
    }

    ଯଦି (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
    {
	dict_T		*d;
	dictitem_T	*di;
	ଅକ୍ଷର		*ଚାବି = ch_mode == CH_MODE_LSP ? "id" : "seq";

	// return an empty dict by default
	ଯଦି (rettv_dict_alloc(rettv) == FAIL)
	    ଫେରନ୍ତୁ;

	ଯଦି (check_for_dict_arg(argvars, 1) == FAIL)
	    ଫେରନ୍ତୁ;

	d = argvars[1].vval.v_dict;
	di = dict_find(d, (char_u *)ଚାବି, -1);
	ଯଦି (argvars[2].v_type == VAR_DICT
		&& dict_has_ଚାବି(argvars[2].vval.v_dict, "callback"))
	    callback_present = TRUE;

	// The id is what a reply is matched by, so it must be a number when
	// one is waited for.
	ଯଦି (di != ଖାଲି && di->di_tv.v_type != VAR_ସଂଖ୍ୟା
		&& (ch_mode == CH_MODE_DAP || eval || callback_present))
	{
	    semsg(_(e_invalid_ମୂଲ୍ୟ_for_argument_str), ଚାବି);
	    ଫେରନ୍ତୁ;
	}

	ଯଦି (ch_mode == CH_MODE_DAP)
	{
	    // DAP message always has a sequence number (id)
	    id = ++channel->ch_last_ସନ୍ଦେଶ_id;
	    ଯଦି (di == ଖାଲି)
		dict_add_ସଂଖ୍ୟା(d, "seq", id);
	    ଅନ୍ୟଥା
		di->di_tv.vval.v_ସଂଖ୍ୟା = id;
	}
	ଅନ୍ୟଥା ଯଦି (eval || callback_present)
	{
	    // When evaluating an expression or sending an expression with a
	    // callback, always assign a generated ID
	    id = ++channel->ch_last_ସନ୍ଦେଶ_id;
	    ଯଦି (di == ଖାଲି)
		dict_add_ସଂଖ୍ୟା(d, "id", id);
	    ଅନ୍ୟଥା
		di->di_tv.vval.v_ସଂଖ୍ୟା = id;
	}
	ଅନ୍ୟଥା
	{
	    // When sending an expression, if the message has an 'id' item,
	    // then use it.
	    id = 0;
	    ଯଦି (di != ଖାଲି && di->di_tv.v_type == VAR_ସଂଖ୍ୟା)
		id = di->di_tv.vval.v_ସଂଖ୍ୟା;
	}
	ଯଦି (ch_mode == CH_MODE_LSP && !dict_has_ଚାବି(d, "jsonrpc"))
	    dict_add_string_len(d, "jsonrpc", (char_u *)"2.0", STRLEN_LITERAL("2.0"));
	text = json_encode_lsp_ସନ୍ଦେଶ(&argvars[1]);
    }
    ଅନ୍ୟଥା
    {
	id = ++channel->ch_last_ସନ୍ଦେଶ_id;
	text = json_encode_nr_ପଦସମଷ୍ଟି(id, &argvars[1],
			      (ch_mode == CH_MODE_JS ? JSON_JS : 0) | JSON_NL);
    }
    ଯଦି (text == ଖାଲି)
	ଫେରନ୍ତୁ;

    channel = send_common(argvars, text, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(text), id, eval, &opt,
			    eval ? "ch_evalexpr" : "ch_sendexpr", &part_read);
    vim_free(text);
    ଯଦି (channel != ଖାଲି && eval)
    {
	ଯଦି (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ଅନ୍ୟଥା
	    timeout = channel_get_timeout(channel, part_read);
	ଯଦି (channel_read_json_block(channel, part_read, timeout, id, &listtv)
									== OK)
	{
	    ଯଦି (ch_mode == CH_MODE_LSP || ch_mode == CH_MODE_DAP)
	    {
		*rettv = *listtv;
		// Change the type to avoid the value being freed.
		listtv->v_type = VAR_ସଂଖ୍ୟା;
		free_tv(listtv);
	    }
	    ଅନ୍ୟଥା
	    {
		list_T *list = listtv->vval.v_list;

		// Move the item from the list and then change the type to
		// avoid the value being freed.
		*rettv = list->lv_u.mat.lv_last->li_tv;
		list->lv_u.mat.lv_last->li_tv.v_type = VAR_ସଂଖ୍ୟା;
		free_tv(listtv);
	    }
	}
    }
    free_job_options(&opt);
    ଯଦି (ch_mode == CH_MODE_DAP && !eval)
    {
	// A DAP message always has a sequence number.
	ଯଦି (rettv->vval.v_dict != ଖାଲି)
	    dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "seq", id);
    }
    ଅନ୍ୟଥା ଯଦି (ch_mode == CH_MODE_LSP && !eval && callback_present)
    {
	// if ch_sendexpr() is used to send a LSP message and a callback
	// function is specified, then return the generated identifier for the
	// message.  The user can use this to cancel the request (if needed).
	ଯଦି (rettv->vval.v_dict != ଖାଲି)
	    dict_add_ସଂଖ୍ୟା(rettv->vval.v_dict, "id", id);
    }
}

/*
 * common for "ch_evalraw()" and "ch_sendraw()"
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ch_raw_common(typval_T *argvars, typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା eval)
{
    char_u	buf[NUMBUFLEN];
    char_u	*text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    channel_T	*channel;
    ch_part_T	part_read;
    jobopt_T    opt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		timeout;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_or_blob_arg(argvars, 1) == FAIL
		|| check_for_opt_dict_arg(argvars, 2) == FAIL))
	ଫେରନ୍ତୁ;

    ଯଦି (argvars[1].v_type == VAR_BLOB)
    {
	text = argvars[1].vval.v_blob->bv_ga.ga_data;
	len = argvars[1].vval.v_blob->bv_ga.ga_len;
    }
    ଅନ୍ୟଥା
    {
	text = tv_get_string_buf(&argvars[1], buf);
	len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(text);
    }
    channel = send_common(argvars, text, len, 0, eval, &opt,
			      eval ? "ch_evalraw" : "ch_sendraw", &part_read);
    ଯଦି (channel != ଖାଲି && eval)
    {
	ଯଦି (opt.jo_set & JO_TIMEOUT)
	    timeout = opt.jo_timeout;
	ଅନ୍ୟଥା
	    timeout = channel_get_timeout(channel, part_read);
	rettv->vval.v_string = channel_read_block(channel, part_read,
							timeout, TRUE, ଖାଲି);
    }
    free_job_options(&opt);
}

#define KEEP_OPEN_TIME 20  // msec

#ଯଦି defined(UNIX) && !defined(HAVE_SELECT)
/*
 * Add open channels to the poll struct.
 * Return the adjusted struct index.
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_poll_setup(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfd_in, ଶୂନ୍ୟ *fds_in, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *towait)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nfd = nfd_in;
    channel_T	*channel;
    ସଂରଚନା	pollfd *fds = fds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    chanpart_T	*ch_part = &channel->ch_part[part];

	    ଯଦି (ch_part->ch_fd != INVALID_FD)
	    {
		ଯଦି (channel->ch_keep_open)
		{
		    // For unknown reason poll() returns immediately for a
		    // keep-open channel.  Instead of adding it to the fds add
		    // a short timeout and check, like polling.
		    ଯଦି (*towait < 0 || *towait > KEEP_OPEN_TIME)
			*towait = KEEP_OPEN_TIME;
		}
		ଅନ୍ୟଥା
		{
		    ch_part->ch_poll_idx = nfd;
		    fds[nfd].fd = ch_part->ch_fd;
		    fds[nfd].events = POLLIN;
		    nfd++;
		}
	    }
	    ଅନ୍ୟଥା
		channel->ch_part[part].ch_poll_idx = -1;
	}
    }

    nfd = channel_fill_poll_write(nfd, fds);

    ଫେରନ୍ତୁ nfd;
}

/*
 * The type of "fds" is hidden to avoid problems with the function proto.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_poll_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_in, ଶୂନ୍ୟ *fds_in)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = ret_in;
    channel_T	*channel;
    ସଂରଚନା	pollfd *fds = fds_in;
    ch_part_T	part;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    idx = channel->ch_part[part].ch_poll_idx;

	    ଯଦି (ret > 0 && idx != -1 && (fds[idx].revents & POLLIN))
	    {
		channel_read(channel, part, "channel_poll_check");
		--ret;
	    }
	    ଅନ୍ୟଥା ଯଦି (channel->ch_part[part].ch_fd != INVALID_FD
						      && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_poll_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	idx = in_part->ch_poll_idx;
	ଯଦି (ret > 0 && idx != -1 && (fds[idx].revents & POLLOUT))
	{
	    channel_write_ନିବେଶ(channel);
	    --ret;
	}
    }

    ଫେରନ୍ତୁ ret;
}
#endif // UNIX && !HAVE_SELECT

#ଯଦି !defined(MSWIN) && defined(HAVE_SELECT)

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_select_setup(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା maxfd_in,
	ଶୂନ୍ୟ *rfds_in,
	ଶୂନ୍ୟ *wfds_in,
	ସଂରଚନା timeval *tv,
	ସଂରଚନା timeval **tvp)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxfd = maxfd_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;

    FOR_ALL_CHANNELS(channel)
    {
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ଯଦି (fd != INVALID_FD)
	    {
		ଯଦି (channel->ch_keep_open)
		{
		    // For unknown reason select() returns immediately for a
		    // keep-open channel.  Instead of adding it to the rfds add
		    // a short timeout and check, like polling.
		    ଯଦି (*tvp == ଖାଲି || tv->tv_sec > 0
					|| tv->tv_usec > KEEP_OPEN_TIME * 1000)
		    {
			*tvp = tv;
			tv->tv_sec = 0;
			tv->tv_usec = KEEP_OPEN_TIME * 1000;
		    }
		}
		ଅନ୍ୟଥା
		{
# ifdef FD_SETSIZE
		    // An fd that does not fit in the fd_set cannot be watched
		    // with select(); skip it rather than overflow the set.
		    ଯଦି ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fd >= FD_SETSIZE)
			ଜାରି;
# endif
		    FD_SET((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fd, rfds);
		    ଯଦି (maxfd < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fd)
			maxfd = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fd;
		}
	    }
	}
    }

    maxfd = channel_fill_wfds(maxfd, wfds);

    ଫେରନ୍ତୁ maxfd;
}

/*
 * The "fd_set" type is hidden to avoid problems with the function proto.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_select_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret_in, ଶୂନ୍ୟ *rfds_in, ଶୂନ୍ୟ *wfds_in)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = ret_in;
    channel_T	*channel;
    fd_set	*rfds = rfds_in;
    fd_set	*wfds = wfds_in;
    ch_part_T	part;
    chanpart_T	*in_part;

    FOR_ALL_CHANNELS(channel)
    {
	ପାଇଁ (part = PART_SOCK; part < PART_IN; ++part)
	{
	    sock_T fd = channel->ch_part[part].ch_fd;

	    ଯଦି (ret > 0 && fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)fd < FD_SETSIZE
# endif
		    && FD_ISSET(fd, rfds))
	    {
		channel_read(channel, part, "channel_select_check");
		FD_CLR(fd, rfds);
		--ret;
	    }
	    ଅନ୍ୟଥା ଯଦି (fd != INVALID_FD && channel->ch_keep_open)
	    {
		// polling a keep-open channel
		channel_read(channel, part, "channel_select_check_keep_open");
	    }
	}

	in_part = &channel->ch_part[PART_IN];
	ଯଦି (ret > 0 && in_part->ch_fd != INVALID_FD
# ifdef FD_SETSIZE
		    && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)in_part->ch_fd < FD_SETSIZE
# endif
					    && FD_ISSET(in_part->ch_fd, wfds))
	{
	    // Clear the flag first, ch_fd may change in channel_write_input().
	    FD_CLR(in_part->ch_fd, wfds);
	    channel_write_ନିବେଶ(channel);
	    --ret;
	}

# ifdef __HAIKU__
	// Workaround for Haiku: Since select/poll cannot detect EOF from tty,
	// should close fds when the job has finished if 'channel' connects to
	// the pty.
	ଯଦି (channel->ch_job != ଖାଲି)
	{
	    job_T *job = channel->ch_job;

	    ଯଦି (job->jv_tty_out != ଖାଲି && job->jv_status == JOB_FINISHED)
		ପାଇଁ (part = PART_SOCK; part < PART_ଗଣନା; ++part)
		    ch_close_part(channel, part);
	}
# endif
    }

    ଫେରନ୍ତୁ ret;
}
#endif // !MSWIN && HAVE_SELECT

/*
 * Wait up to "msec" for something to read on any channel and read it.
 * For channel_stdio_loop(), which has no terminal to wait for.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
channel_wait_any(long msec)
{
#ifdef MSWIN
    // A pipe cannot be waited for, channel_wait() polls it.
    ଯଦି (stdio_channel->CH_OUT_FD != INVALID_FD)
	(ଶୂନ୍ୟ)channel_wait(stdio_channel, stdio_channel->CH_OUT_FD,
						(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(msec > 20 ? 20 : msec));
    channel_handle_events(FALSE);
#ନଚେତଯଦି defined(HAVE_SELECT)
    fd_set		rfds;
    fd_set		wfds;
    ସଂରଚନା timeval	tv;
    ସଂରଚନା timeval	*tvp = &tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			maxfd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ret;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    maxfd = channel_select_setup(-1, &rfds, &wfds, &tv, &tvp);
    ret = select(maxfd + 1, &rfds, &wfds, ଖାଲି, tvp);
    ଯଦି (ret >= 0)
	(ଶୂନ୍ୟ)channel_select_check(ret, &rfds, &wfds);
#ଅନ୍ୟଥା
    ସଂରଚନା pollfd	fds[4 * MAX_OPEN_CHANNELS + MAX_CLIENT_CHANNELS];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			nfd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			towait = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)msec;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ret;

    nfd = channel_poll_setup(0, fds, &towait);
    ret = poll(fds, nfd, towait);
    ଯଦି (ret >= 0)
	(ଶୂନ୍ୟ)channel_poll_check(ret, fds);
#endif
}

/*
 * Wait for messages on the stdio channel and handle them, until the channel
 * is closed.  Takes the place of the main loop with --stdio-channel.
 */
    ଶୂନ୍ୟ
channel_stdio_loop(ଶୂନ୍ୟ)
{
    ଯେତେବେଳେ (stdio_channel != ଖାଲି && channel_is_open(stdio_channel))
    {
	long	msec = 1000;
#ifdef FEAT_TIMERS
	long	due_time = check_due_timer();

	ଯଦି (due_time > 0 && due_time < msec)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_parse_messages(ଶୂନ୍ୟ)
{
    channel_T	*channel = first_channel;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ch_part_T	part = PART_SOCK;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = 0;
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
    ଯଦି ((did_repeated_ସନ୍ଦେଶ & REPEATED_ସନ୍ଦେଶ_LOOKING) == 0)
    {
	ch_log(ଖାଲି, "looking for messages on channels");
	// now we should also give the message for SafeState
	did_repeated_ସନ୍ଦେଶ = REPEATED_ସନ୍ଦେଶ_LOOKING;
    }
    ଯେତେବେଳେ (channel != ଖାଲି)
    {
	ଯଦି (recursive == 1)
	{
	    ଯଦି (channel_can_close(channel))
	    {
		channel->ch_to_be_closed = (1U << PART_ଗଣନା);
		channel_close_now(channel);
		// channel may have been freed, start over
		channel = first_channel;
		ଜାରି;
	    }
	    ଯଦି (channel->ch_to_be_freed || channel->ch_killing)
	    {
		channel_free_contents(channel);
		ଯଦି (channel->ch_job != ଖାଲି)
		    channel->ch_job->jv_channel = ଖାଲି;

		// free the channel and then start over
		channel_free_channel(channel);
		channel = first_channel;
		ଜାରି;
	    }
	    ଯଦି (channel->ch_refcount == 0 && !channel_still_useful(channel))
	    {
		// channel is no longer useful, free it
		channel_free(channel);
		channel = first_channel;
		part = PART_SOCK;
		ଜାରି;
	    }
	}

#ifdef FEAT_TERMINAL
	// For a terminal job with a separate stderr pipe, defer processing
	// PART_OUT until PART_ERR is drained.  Both are filled by
	// channel_select_check() in the same select() pass, so when both
	// have readahead the child's write order (stderr before stdout) is
	// preserved by handling PART_ERR first.
	ଯଦି (part == PART_OUT
		&& channel->ch_job != ଖାଲି
		&& channel->ch_job->jv_tty_out != ଖାଲି
		&& channel_has_readahead(channel, PART_ERR))
	{
	    part = PART_ERR;
	    ଜାରି;
	}
#endif
	ଯଦି (channel->ch_part[part].ch_fd != INVALID_FD
				      || channel_has_readahead(channel, part))
	{
	    // Increase the refcount, in case the handler causes the channel
	    // to be unreferenced or closed.
	    ++channel->ch_refcount;
	    r = may_invoke_callback(channel, part);
	    ଯଦି (r == OK)
		ret = TRUE;
	    ଯଦି (channel_unref(channel) || (r == OK
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
		ଜାରି;
	    }
	}
	ଯଦି (part < PART_ERR)
	    ++part;
	ଅନ୍ୟଥା
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }

    ଯଦି (channel_need_redraw)
    {
	channel_need_redraw = FALSE;
	redraw_after_callback(TRUE, FALSE);
    }

    --safe_to_invoke_callback;
    --recursive;

    ଫେରନ୍ତୁ ret;
}

/*
 * Return TRUE if any channel has readahead.  That means we should not block on
 * waiting for input.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_any_readahead(ଶୂନ୍ୟ)
{
    channel_T	*channel = first_channel;
    ch_part_T	part = PART_SOCK;

    ଯେତେବେଳେ (channel != ଖାଲି)
    {
	ଯଦି (channel_has_readahead(channel, part))
	    ଫେରନ୍ତୁ TRUE;
	ଯଦି (part < PART_ERR)
	    ++part;
	ଅନ୍ୟଥା
	{
	    channel = channel->ch_next;
	    part = PART_SOCK;
	}
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Mark references to lists used in channels.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_channel(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		abort = FALSE;
    channel_T	*channel;
    typval_T	tv;

    ପାଇଁ (channel = first_channel; !abort && channel != ଖାଲି;
						   channel = channel->ch_next)
	ଯଦି (channel_still_useful(channel))
	{
	    tv.v_type = VAR_CHANNEL;
	    tv.vval.v_channel = channel;
	    abort = abort || set_ref_in_item(&tv, copyID, ଖାଲି, ଖାଲି, ଖାଲି);
	}
    ଫେରନ୍ତୁ abort;
}

/*
 * Return the "part" to write to for "channel".
 */
    ସ୍ଥିର ch_part_T
channel_part_send(channel_T *channel)
{
    ଯଦି (channel->CH_SOCK_FD == INVALID_FD)
	ଫେରନ୍ତୁ PART_IN;
    ଫେରନ୍ତୁ PART_SOCK;
}

/*
 * Return the default "part" to read from for "channel".
 */
    ସ୍ଥିର ch_part_T
channel_part_read(channel_T *channel)
{
    ଯଦି (channel->CH_SOCK_FD == INVALID_FD)
	ଫେରନ୍ତୁ PART_OUT;
    ଫେରନ୍ତୁ PART_SOCK;
}

/*
 * Return the mode of "channel"/"part"
 * If "channel" is invalid returns CH_MODE_JSON.
 */
    ସ୍ଥିର ch_mode_T
channel_get_mode(channel_T *channel, ch_part_T part)
{
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ CH_MODE_JSON;
    ଫେରନ୍ତୁ channel->ch_part[part].ch_mode;
}

/*
 * Return the timeout of "channel"/"part"
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
channel_get_timeout(channel_T *channel, ch_part_T part)
{
    ଫେରନ୍ତୁ channel->ch_part[part].ch_timeout;
}

/*
 * "ch_canread()" function
 */
    ଶୂନ୍ୟ
f_ch_canread(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    rettv->vval.v_ସଂଖ୍ୟା = 0;
    ଯଦି (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ଯଦି (channel != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା = channel_has_readahead(channel, PART_SOCK)
			    || channel_has_readahead(channel, PART_OUT)
			    || channel_has_readahead(channel, PART_ERR);
}

/*
 * "ch_close()" function
 */
    ଶୂନ୍ୟ
f_ch_close(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ଯଦି (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ଯଦି (channel != ଖାଲି)
    {
	channel_close(channel, FALSE);
	channel_clear(channel);
    }
}

/*
 * "ch_close()" function
 */
    ଶୂନ୍ୟ
f_ch_close_in(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ଯଦି (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], TRUE, FALSE, 0);
    ଯଦି (channel != ଖାଲି)
	channel_close_in(channel);
}

/*
 * "ch_getbufnr()" function
 */
    ଶୂନ୍ୟ
f_ch_getbufnr(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;

    rettv->vval.v_ସଂଖ୍ୟା = -1;

    ଯଦି (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_string_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ;

    char_u	*what = tv_get_string(&argvars[1]);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		part;
    ଯଦି (STRCMP(what, "err") == 0)
	part = PART_ERR;
    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "out") == 0)
	part = PART_OUT;
    ଅନ୍ୟଥା ଯଦି (STRCMP(what, "in") == 0)
	part = PART_IN;
    ଅନ୍ୟଥା
	part = PART_SOCK;
    ଯଦି (channel->ch_part[part].ch_bufref.br_buf != ଖାଲି)
	rettv->vval.v_ସଂଖ୍ୟା =
	    channel->ch_part[part].ch_bufref.br_buf->b_fnum;
}

/*
 * "ch_getjob()" function
 */
    ଶୂନ୍ୟ
f_ch_getjob(typval_T *argvars, typval_T *rettv)
{
    channel_T *channel;

    ଯଦି (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ;

    rettv->v_type = VAR_JOB;
    rettv->vval.v_job = channel->ch_job;
    ଯଦି (channel->ch_job != ଖାଲି)
	++channel->ch_job->jv_refcount;
}

/*
 * "ch_info()" function
 */
    ଶୂନ୍ୟ
f_ch_info(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T *channel;

    ଯଦି (in_vim9script() && check_for_chan_or_job_arg(argvars, 0) == FAIL)
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ଯଦି (channel != ଖାଲି && rettv_dict_alloc(rettv) == OK)
	channel_info(channel, rettv->vval.v_dict);
}

/*
 * "ch_listen()" function
 */
    ଶୂନ୍ୟ
f_ch_listen(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;
    rettv->vval.v_channel = channel_listen_func(argvars);
}

/*
 * "ch_open()" function
 */
    ଶୂନ୍ୟ
f_ch_open(typval_T *argvars, typval_T *rettv)
{
    rettv->v_type = VAR_CHANNEL;
    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;
    rettv->vval.v_channel = channel_open_func(argvars);
}

/*
 * "ch_read()" function
 */
    ଶୂନ୍ୟ
f_ch_read(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, FALSE, FALSE);
}

/*
 * "ch_readblob()" function
 */
    ଶୂନ୍ୟ
f_ch_readblob(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, TRUE);
}

/*
 * "ch_readraw()" function
 */
    ଶୂନ୍ୟ
f_ch_readraw(typval_T *argvars, typval_T *rettv)
{
    common_channel_read(argvars, rettv, TRUE, FALSE);
}

/*
 * "ch_evalexpr()" function
 */
    ଶୂନ୍ୟ
f_ch_evalexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ପଦସମଷ୍ଟି_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendexpr()" function
 */
    ଶୂନ୍ୟ
f_ch_sendexpr(typval_T *argvars, typval_T *rettv)
{
    ch_ପଦସମଷ୍ଟି_common(argvars, rettv, FALSE);
}

/*
 * "ch_evalraw()" function
 */
    ଶୂନ୍ୟ
f_ch_evalraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, TRUE);
}

/*
 * "ch_sendraw()" function
 */
    ଶୂନ୍ୟ
f_ch_sendraw(typval_T *argvars, typval_T *rettv)
{
    ch_raw_common(argvars, rettv, FALSE);
}

/*
 * "ch_setoptions()" function
 */
    ଶୂନ୍ୟ
f_ch_setoptions(typval_T *argvars, typval_T *rettv UNUSED)
{
    channel_T	*channel;
    jobopt_T	opt;

    ଯଦି (check_restricted() || check_secure())
	ଫେରନ୍ତୁ;

    ଯଦି (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);
    ଯଦି (channel == ଖାଲି)
	ଫେରନ୍ତୁ;
    clear_job_options(&opt);
    ଯଦି (get_job_options(&argvars[1], &opt,
			    JO_CB_ALL + JO_TIMEOUT_ALL + JO_MODE_ALL, 0) == OK)
	channel_set_options(channel, &opt);
    free_job_options(&opt);
}

/*
 * "ch_status()" function
 */
    ଶୂନ୍ୟ
f_ch_status(typval_T *argvars, typval_T *rettv)
{
    channel_T	*channel;
    jobopt_T	opt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		part = -1;

    // return an empty string by default
    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ଖାଲି;

    ଯଦି (in_vim9script()
	    && (check_for_chan_or_job_arg(argvars, 0) == FAIL
		|| check_for_opt_dict_arg(argvars, 1) == FAIL))
	ଫେରନ୍ତୁ;

    channel = get_channel_arg(&argvars[0], FALSE, FALSE, 0);

    ଯଦି (argvars[1].v_type != VAR_UNKNOWN)
    {
	clear_job_options(&opt);
	ଯଦି (get_job_options(&argvars[1], &opt, JO_PART, 0) == OK
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
    ଅକ୍ଷର      *status = channel_status(channel, -1);

    ଯଦି (channel == ଖାଲି)
	vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN, "channel %s", status);
    ଅନ୍ୟଥା
	vim_snprintf((ଅକ୍ଷର *)buf, NUMBUFLEN,
				      "channel %d %s", channel->ch_id, status);
    ଫେରନ୍ତୁ buf;
}

/*
 * Return the channel with the given ID. Returns NULL if not found.
 */
    channel_T *
channel_find(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch_id)
{
    channel_T *ch;
    FOR_ALL_CHANNELS(ch)
	ଯଦି (ch->ch_id == ch_id)
	    ଫେରନ୍ତୁ ch;
    ଫେରନ୍ତୁ ଖାଲି;
}

#endif // FEAT_JOB_CHANNEL
