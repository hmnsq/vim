/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * wayland.c: Stuff related to Wayland. Functions that prefixed with "vwl_"
 *	      handle/provide abstractions and building blocks to create more
 *	      complex things. The "wayland_" functions handle the global
 *	      Wayland connection.
 *
 *	      At the end of this file, there are a bunch of macro definitions
 *	      that abstract away all the different protocols for the clipboard
 *	      we need to support under one single interface. This is then used
 *	      by clipboard.c to implement the Wayland clipboard functionality.
 *
 *	      The clipboard functionality monitors the Wayland display at all
 *	      times, and saves new selections/offers as events come in. When we
 *	      want retrieve the selection, the currently saved data offer is
 *	      used from the respective data device.
 */

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_WAYLAND

#ଅନ୍ତର୍ଭାଵ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ସଂରଚନା pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#ଅନ୍ୟଥା
    fd_set	    wfds;
    ସଂରଚନା timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    ଯଦି (self->display.proxy == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    ଯେତେବେଳେ (true)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = wl_display_flush(self->display.proxy);

	ଯଦି (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    ଯଦି (poll(&fds, 1, p_wtm) <= 0)
#ଅନ୍ୟଥା
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    ଯଦି (select(self->display.fd + 1, ଖାଲି, &wfds, ଖାଲି, &tv) <= 0)
		ଫେରନ୍ତୁ FAIL;
#endif
	}
	ଅନ୍ୟଥା ଯଦି (ret == -1)
	    ଫେରନ୍ତୁ FAIL;
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ସଂରଚନା pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#ଅନ୍ୟଥା
    fd_set	    rfds;
    ସଂରଚନା timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    ଯଦି (self->display.proxy == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯେତେବେଳେ (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	ଯଦି (wl_display_dispatch_pending(self->display.proxy) == -1)
	    ଫେରନ୍ତୁ -1;

    // Send any requests before we starting blocking to read display fd
    ଯଦି (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	ଫେରନ୍ତୁ -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    ଯଦି (poll(&fds, 1, p_wtm) <= 0)
#ଅନ୍ୟଥା
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    ଯଦି (select(self->display.fd + 1, &rfds, ଖାଲି, ଖାଲି, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	ଫେରନ୍ତୁ -1;
    }

    // Read events into the queue
    ଯଦି (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	ଫେରନ୍ତୁ -1;

    // Dispatch those events (call the handlers associated for each event)
    ଫେରନ୍ତୁ wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vwl_callback_event_done(ଶୂନ୍ୟ *data, ସଂରଚନା wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vwl_connection_roundtrip(vwl_connection_T *self)
{
    ସଂରଚନା wl_callback	*callback;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    ଯଦି (self->display.proxy == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    ଯଦି (callback == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    ଯେତେବେଳେ (true)
    {
	ret = vwl_connection_dispatch(self);

	ଯଦି (done || ret == -1)
	    ଭାଙ୍ଗ;

#ifdef ELAPSED_FUNC
	ଯଦି (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    ଭାଙ୍ଗ;
	}
#endif
    }

    ଯଦି (ret == -1)
    {
	ଯଦି (!done)
	    wl_callback_destroy(callback);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vwl_log_handler(ଅଚଳ ଅକ୍ଷର *fmt, va_list args)
{
    // 512 bytes should be big enough
    ଅକ୍ଷର	*buf	= alloc(512);
    ଅକ୍ଷର	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(ଖାଲି, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wl_seat_listener_event_name(
	ଶୂନ୍ୟ		*data,
	ସଂରଚନା wl_seat	*seat_proxy UNUSED,
	ଅଚଳ ଅକ୍ଷର	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (ଅକ୍ଷର *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wl_seat_listener_event_capabilities(
	ଶୂନ୍ୟ		*data,
	ସଂରଚନା wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

ସ୍ଥିର ଶୂନ୍ୟ vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wl_registry_listener_event_global(
	ଶୂନ୍ୟ		    *data,
	ସଂରଚନା wl_registry  *registry,
	uint32_t	    name,
	ଅଚଳ ଅକ୍ଷର	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    ଯଦି (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	ସଂରଚନା wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	ଯଦି (seat_proxy == ଖାଲି)
	    ଫେରନ୍ତୁ;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	ଯଦି (seat == ଖାଲି || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    ଫେରନ୍ତୁ;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	ଯଦି (vwl_connection_roundtrip(ct) == FAIL || seat->label == ଖାଲି)
	{
	    vwl_seat_destroy(seat);
	    ଫେରନ୍ତୁ;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    ଅନ୍ୟଥା ଯଦି (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    ଅନ୍ୟଥା ଯଦି (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.ext_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &ext_data_control_manager_v1_interface, 1);
#endif // FEAT_WAYLAND_CLIPBOARD

}

/*
 * Called when a global object is removed, if so, then do nothing. This is to
 * avoid a global being removed while it is in the process of being used. Let
 * the user call :wlrestore in order to reset everything. Requests to that
 * global will just be ignored on the compositor side.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wl_registry_listener_event_global_remove(
	ଶୂନ୍ୟ		    *data UNUSED,
	ସଂରଚନା wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

ସ୍ଥିର ଶୂନ୍ୟ vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    ଯଦି (ct->gobjects.object != ଖାଲି) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = ଖାଲି; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != ଖାଲି)

#endif

// Make sure to call wayland_set_display(display);
    ସ୍ଥିର vwl_connection_T *
vwl_connection_new(ଅଚଳ ଅକ୍ଷର *display)
{
    vwl_connection_T	*ct;

    ଯଦି (wayland_no_connect)
	ଫେରନ୍ତୁ ଖାଲି;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    ଯଦି (mch_getenv("XDG_RUNTIME_DIR") == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    ଯଦି (ct == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    ଯଦି (ct->display.proxy == ଖାଲି)
    {
	vim_free(ct);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    ଯଦି (ct->registry.proxy == ଖାଲି)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ga_init2(&ct->gobjects.seats, ଆକାର(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    ଯଦି (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vwl_seat_destroy(vwl_seat_T *self)
{
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (self->proxy != ଖାଲି)
    {
	ଯଦି (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	ଅନ୍ୟଥା
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    ଯଦି (self->registry.proxy != ଖାଲି)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = ଖାଲି;
    }
    ଯଦି (self->display.proxy != ଖାଲି)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = ଖାଲି;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ଅଚଳ ଅକ୍ଷର *label)
{
    ଯଦି ((STRCMP(label, "") == 0 || label == ଖାଲି)
	    && self->gobjects.seats.ga_len > 0)
	ଫେରନ୍ତୁ ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	ଯଦି (STRCMP(seat->label, label) == 0)
	    ଫେରନ୍ତୁ seat;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wayland_set_display(ଅଚଳ ଅକ୍ଷର *display)
{
    ଯଦି (display == ଖାଲି)
	display = (ଅକ୍ଷର*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    ଅନ୍ୟଥା ଯଦି (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	ଯାଅ exit;

    ଯଦି (display == ଖାଲି)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    ଯଦି (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (ଅକ୍ଷର*)vim_strsave((char_u*)display);
    }

exit:
#ifdef FEAT_EVAL
    set_vim_var_string(VV_WAYLAND_DISPLAY, (char_u*)display, -1);
#endif
}

/*
 * Initializes the global Wayland connection. Connects to the Wayland display
 * with given name and binds to global objects as needed. If display is NULL
 * then the $WAYLAND_DISPLAY environment variable will be used (handled by
 * libwayland). Returns FAIL on failure and OK on
 * success
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wayland_init_connection(ଅଚଳ ଅକ୍ଷର *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    ଯଦି (wayland_ct == ଖାଲି)
	ଯାଅ fail;

    ଫେରନ୍ତୁ OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    ଶୂନ୍ୟ
wayland_uninit_connection(ଶୂନ୍ୟ)
{
    ଯଦି (wayland_ct == ଖାଲି)
	ଫେରନ୍ତୁ;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = ଖାଲି;
    is_reading = false;
    wayland_set_display("");
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା wayland_ct_restore_ଗଣନା = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
wayland_restore_connection(ଶୂନ୍ୟ)
{
    // No point in restoring the connection if we are exiting or dying.
    ଯଦି (exiting || v_dying || wayland_ct_restore_ଗଣନା <= 0)
	wayland_set_display("");

    --wayland_ct_restore_ଗଣନା;
    wayland_uninit_connection();

    ଯଦି (wayland_init_connection(wayland_display_name) == OK)
    {
#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
}

/*
 * Should be called before polling (select or poll) the global Wayland
 * connection display fd. Returns fd on success and -1 on failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wayland_prepare_read(ଶୂନ୍ୟ)
{
    ଯଦି (wayland_ct == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    ଯଦି (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    ଯେତେବେଳେ (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	ଯଦି (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    ଫେରନ୍ତୁ -1;

    ଯଦି (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	ଫେରନ୍ତୁ -1;
    }

    is_reading = true;

    ଫେରନ୍ତୁ wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
wayland_update(ଶୂନ୍ୟ)
{
    ଯଦି (wayland_ct == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ vwl_connection_roundtrip(wayland_ct);
}

#ଯଦି !defined(HAVE_SELECT)

    ଶୂନ୍ୟ
wayland_poll_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା revents)
{
    ଯଦି (wayland_ct == ଖାଲି)
	ଫେରନ୍ତୁ;

    is_reading = false;
    ଯଦି (revents & POLLIN)
    {
	ଯଦି (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    ଫେରନ୍ତୁ;
	}
    }
    ଅନ୍ୟଥା ଯଦି (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    ଅନ୍ୟଥା
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	ଫେରନ୍ତୁ;
    }
    wayland_restore_connection();
}

#endif
#ଯଦି defined(HAVE_SELECT)

    ଶୂନ୍ୟ
wayland_select_check(bool is_set)
{
    ଯଦି (wayland_ct == ଖାଲି)
	ଫେରନ୍ତୁ;

    is_reading = false;
    ଯଦି (is_set)
    {
	ଯଦି (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	ଅନ୍ୟଥା
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    ଅନ୍ୟଥା
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    ଶୂନ୍ୟ
ex_wlrestore(exarg_T *eap)
{
    ଅକ୍ଷର *display;

    ଯଦି (eap->arg == ଖାଲି || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    ଅନ୍ୟଥା
	display = (ଅକ୍ଷର*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    ଯଦି (!eap->forceit && wayland_ct != ଖାଲି &&
	    (display == wayland_display_name ||
	     (wayland_display_name != ଖାଲି &&
	      STRCMP(wayland_display_name, display) == 0)))
	ଫେରନ୍ତୁ;

#ifdef FEAT_WAYLAND_CLIPBOARD
    ଯଦି (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	ଯଦି (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ଯଦି (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    ଯଦି (display != ଖାଲି)
	display = (ଅକ୍ଷର*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_ଗଣନା = 5;

    ଯଦି (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    ଅନ୍ୟଥା
	ସନ୍ଦେଶ(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#ଯଦି defined(FEAT_WAYLAND_CLIPBOARD)

/*
 * Get a suitable data device manager from connection. "supported" should be
 * initialized to VWL_DATA_PROTOCOL_NONE beforehand. Returns NULL if there are
 * no data device manager available with the required selection.
 */
    vwl_data_device_manager_T *
vwl_connection_get_data_device_manager(
	vwl_connection_T *self,
	wayland_selection_T req_sel,
	int_u *supported)
{
    vwl_data_device_manager_T *manager =
	ALLOC_CLEAR_ONE(vwl_data_device_manager_T);

    // Prioritize ext-data-control-v1 over wlr-data-control-unstable-v1 because
    // it is newer.
    ଯଦି (self->gobjects.ext_data_control_manager_v1 != ଖାଲି)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    ଅନ୍ୟଥା ଯଦି (self->gobjects.zwlr_data_control_manager_v1 != ଖାଲି)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	ଯଦି (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    ଯଦି (!(*supported & req_sel))
    {
	vim_free(manager);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    ବାଛ (self->protocol)
    {
	ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    vim_free(device);
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    device->protocol = self->protocol;

    ଫେରନ୍ତୁ device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    ବାଛ (self->protocol)
    {
	ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    vim_free(source);
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    source->protocol = self->protocol;

    ଫେରନ୍ତୁ source;
}

    ସ୍ଥିର vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, ଶୂନ୍ୟ *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    ଯଦି (offer == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, ଆକାର(ଅକ୍ଷର *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    ଫେରନ୍ତୁ offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	ଶୂନ୍ୟ \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	ଯଦି (self == ଖାଲି) \
	    ଫେରନ୍ତୁ; \
	ବାଛ (self->protocol) \
	{ \
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		ଭାଙ୍ଗ; \
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		ଭାଙ୍ଗ; \
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: \
		ଭାଙ୍ଗ; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    ଶୂନ୍ୟ
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ;
    ବାଛ (self->protocol)
    {
	ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
ଶୂନ୍ୟ
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    ଯଦି (self == ଖାଲି)
	ଫେରନ୍ତୁ;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	ସ୍ଥିର ଶୂନ୍ୟ \
    device_type##_listener_event_data_offer( \
	    ଶୂନ୍ୟ *data, \
	    ସଂରଚନା device_type *device UNUSED, \
	    ସଂରଚନା offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	ସ୍ଥିର ଶୂନ୍ୟ \
    device_type##_listener_event_selection( \
	    ଶୂନ୍ୟ *data, \
	    ସଂରଚନା device_type *device UNUSED, \
	    ସଂରଚନା offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = ଖାଲି; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	ସ୍ଥିର ଶୂନ୍ୟ \
    device_type##_listener_event_primary_selection( \
	    ଶୂନ୍ୟ *data, \
	    ସଂରଚନା device_type *device UNUSED, \
	    ସଂରଚନା offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = ଖାଲି; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	ସ୍ଥିର ଶୂନ୍ୟ \
    device_type##_listener_event_finished( \
	    ଶୂନ୍ୟ *data, \
	    ସଂରଚନା device_type *device UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->finished(self->data, self); \
    }

VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(
    ext_data_control_device_v1, ext_data_control_offer_v1)
VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(
    zwlr_data_control_device_v1, zwlr_data_control_offer_v1)

VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(
    ext_data_control_device_v1, ext_data_control_offer_v1
)
VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(
    zwlr_data_control_device_v1, zwlr_data_control_offer_v1
)
VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(
    ext_data_control_device_v1, ext_data_control_offer_v1
)
VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(
    zwlr_data_control_device_v1, zwlr_data_control_offer_v1
)

VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(ext_data_control_device_v1)
VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(zwlr_data_control_device_v1)

ସ୍ଥିର ସଂରଚନା ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
ସ୍ଥିର ଅଚଳ ସଂରଚନା zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	ସ୍ଥିର ଶୂନ୍ୟ \
    source_type##_listener_event_send( \
	    ଶୂନ୍ୟ *data, ସଂରଚନା source_type *source UNUSED, \
	    ଅଚଳ ଅକ୍ଷର *mime_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	ସ୍ଥିର ଶୂନ୍ୟ \
    source_type##_listener_event_cancelled( \
	    ଶୂନ୍ୟ *data, ସଂରଚନା source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

ସ୍ଥିର ଅଚଳ ସଂରଚନା ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
ସ୍ଥିର ଅଚଳ ସଂରଚନା zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	ସ୍ଥିର ଶୂନ୍ୟ \
    offer_type##_listener_event_offer( \
	    ଶୂନ୍ୟ *data, \
	    ସଂରଚନା offer_type *offer UNUSED, \
	    ଅଚଳ ଅକ୍ଷର *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	ଯଦି (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    ଅକ୍ଷର *mime = (ଅକ୍ଷର *)vim_strsave((char_u *)mime_type); \
	    ଯଦି (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    ଅନ୍ୟଥା \
		ଯଦି (mime != ଖାଲି) \
		    ((ଅକ୍ଷର **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

ସ୍ଥିର ଅଚଳ ସଂରଚନା ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
ସ୍ଥିର ଅଚଳ ସଂରଚନା zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	ଶୂନ୍ୟ \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    ଅଚଳ vwl_data_##type##_listener_T *listener, \
	    ଶୂନ୍ୟ *data) \
    { \
	ଯଦି (self == ଖାଲି) \
	    ଫେରନ୍ତୁ; \
	self->data = data; \
	self->listener = listener; \
	ବାଛ (self->protocol) \
	{ \
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		ଭାଙ୍ଗ; \
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		ଭାଙ୍ଗ; \
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: \
		ଭାଙ୍ଗ; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    ଶୂନ୍ୟ
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    ଶୂନ୍ୟ *proxy = source == ଖାଲି ? ଖାଲି : source->proxy;

    ଯଦି (selection == WAYLAND_SELECTION_REGULAR)
    {
	ବାଛ (self->protocol)
	{
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
    }
    ଅନ୍ୟଥା ଯଦି (selection == WAYLAND_SELECTION_PRIMARY)
    {
	ବାଛ (self->protocol)
	{
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		ଭାଙ୍ଗ;
	}
    }
}

    ଶୂନ୍ୟ
vwl_data_source_offer(vwl_data_source_T *self, ଅଚଳ ଅକ୍ଷର *mime_type)
{
    ବାଛ (self->protocol)
    {
	ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
}

    ଶୂନ୍ୟ
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	ଅଚଳ ଅକ୍ଷର *mime_type,
	int32_t fd)
{
    ବାଛ (self->protocol)
    {
	ସ୍ଥିତି VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
