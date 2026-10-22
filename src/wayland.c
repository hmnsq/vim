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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND

#ਅਨ੍ਤਰ੍ਭਾਵ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    ਪੂਰਨ
vwl_connection_flush(vwl_connection_T *self)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SELECT
    ਬਣਤਰ pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#ਨਹੀਂਤਾਂ
    fd_set	    wfds;
    ਬਣਤਰ timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#ਜੇ_ਅੰਤ

    ਜੇ (self->display.proxy == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    ਜਦੋਂ (true)
    {
	ਪੂਰਨ ret = wl_display_flush(self->display.proxy);

	ਜੇ (ret == -1 && errno == EAGAIN)
	{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SELECT
	    ਜੇ (poll(&fds, 1, p_wtm) <= 0)
#ਨਹੀਂਤਾਂ
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    ਜੇ (select(self->display.fd + 1, ਨਿਰਾ, &wfds, ਨਿਰਾ, &tv) <= 0)
		ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (ret == -1)
	    ਵਾਪਸ FAIL;
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    ਵਾਪਸ OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    ਪੂਰਨ
vwl_connection_dispatch(vwl_connection_T *self)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SELECT
    ਬਣਤਰ pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#ਨਹੀਂਤਾਂ
    fd_set	    rfds;
    ਬਣਤਰ timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#ਜੇ_ਅੰਤ

    ਜੇ (self->display.proxy == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜਦੋਂ (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	ਜੇ (wl_display_dispatch_pending(self->display.proxy) == -1)
	    ਵਾਪਸ -1;

    // Send any requests before we starting blocking to read display fd
    ਜੇ (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	ਵਾਪਸ -1;
    }

    // Poll until there is data to read from the display fd.
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_SELECT
    ਜੇ (poll(&fds, 1, p_wtm) <= 0)
#ਨਹੀਂਤਾਂ
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    ਜੇ (select(self->display.fd + 1, &rfds, ਨਿਰਾ, ਨਿਰਾ, &tv) <= 0)
#ਜੇ_ਅੰਤ
    {
	wl_display_cancel_read(self->display.proxy);
	ਵਾਪਸ -1;
    }

    // Read events into the queue
    ਜੇ (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	ਵਾਪਸ -1;

    // Dispatch those events (call the handlers associated for each event)
    ਵਾਪਸ wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    ਸਥਿਰ ਖਾਲੀ
vwl_callback_event_ਪੂਰਾ(ਖਾਲੀ *ਅੰਕੜੇ, ਬਣਤਰ wl_callback *callback,
	uint32_t callback_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    *((bool*)ਅੰਕੜੇ) = true;
    wl_callback_destroy(callback);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ wl_callback_listener vwl_callback_listener = {
    .ਪੂਰਾ = vwl_callback_event_ਪੂਰਾ
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    ਪੂਰਨ
vwl_connection_roundtrip(vwl_connection_T *self)
{
    ਬਣਤਰ wl_callback	*callback;
    ਪੂਰਨ			ret;
    bool		ਪੂਰਾ = false;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    elapsed_T		ਸ਼ੁਰੂ_tv;
#ਜੇ_ਅੰਤ

    ਜੇ (self->display.proxy == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    ਜੇ (callback == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    wl_callback_ਜੋੜੋ_listener(callback, &vwl_callback_listener, &ਪੂਰਾ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
#ਜੇ_ਅੰਤ

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    ਜਦੋਂ (true)
    {
	ret = vwl_connection_dispatch(self);

	ਜੇ (ਪੂਰਾ || ret == -1)
	    ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ELAPSED_FUNC
	ਜੇ (ELAPSED_FUNC(ਸ਼ੁਰੂ_tv) >= p_wtm)
	{
	    ret = -1;
	    ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ
    }

    ਜੇ (ret == -1)
    {
	ਜੇ (!ਪੂਰਾ)
	    wl_callback_destroy(callback);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    ਸਥਿਰ ਖਾਲੀ
vwl_log_handler(ਅਚਲ ਅੱਖਰ *fmt, va_list ਦਲੀਲਾਂ)
{
    // 512 bytes should be big enough
    ਅੱਖਰ	*buf	= alloc(512);
    ਅੱਖਰ	*ਅਗੇਤਰ = _("wayland protocol error -> ");
    size_t	len	= STRLEN(ਅਗੇਤਰ);

    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ;

    vim_strncpy((char_u*)buf, (char_u*)ਅਗੇਤਰ, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, ਦਲੀਲਾਂ);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ch_log(ਨਿਰਾ, "%s", buf);
#ਜੇ_ਅੰਤ
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    ਸਥਿਰ ਖਾਲੀ
wl_seat_listener_event_name(
	ਖਾਲੀ		*ਅੰਕੜੇ,
	ਬਣਤਰ wl_seat	*seat_proxy ਅਣਵਰਤਿਆ,
	ਅਚਲ ਅੱਖਰ	*name)
{
    vwl_seat_T *seat = ਅੰਕੜੇ;

    seat->label = (ਅੱਖਰ *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    ਸਥਿਰ ਖਾਲੀ
wl_seat_listener_event_capabilities(
	ਖਾਲੀ		*ਅੰਕੜੇ,
	ਬਣਤਰ wl_seat	*seat_proxy ਅਣਵਰਤਿਆ,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = ਅੰਕੜੇ;

    seat->capabilities = capabilities;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

ਸਥਿਰ ਖਾਲੀ vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    ਸਥਿਰ ਖਾਲੀ
wl_registry_listener_event_global(
	ਖਾਲੀ		    *ਅੰਕੜੇ,
	ਬਣਤਰ wl_registry  *registry,
	uint32_t	    name,
	ਅਚਲ ਅੱਖਰ	    *interface,
	uint32_t	    ਸੰਸਕਰਣ_2)
{
    vwl_connection_T *ct = ਅੰਕੜੇ;

    ਜੇ (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	ਬਣਤਰ wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, ਸੰਸਕਰਣ_2 > 5 ? 5 : ਸੰਸਕਰਣ_2);
	vwl_seat_T	*seat;

	ਜੇ (seat_proxy == ਨਿਰਾ)
	    ਵਾਪਸ;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	ਜੇ (seat == ਨਿਰਾ || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    ਵਾਪਸ;
	}

	seat->proxy = seat_proxy;
	wl_seat_ਜੋੜੋ_listener(seat_proxy, &wl_seat_listener, seat);

	ਜੇ (vwl_connection_roundtrip(ct) == FAIL || seat->label == ਨਿਰਾ)
	{
	    vwl_seat_destroy(seat);
	    ਵਾਪਸ;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_ਅੰਕੜੇ)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(interface, zwlr_ਅੰਕੜੇ_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_ਅੰਕੜੇ_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_ਅੰਕੜੇ_control_manager_v1_interface,
		    ਸੰਸਕਰਣ_2 > 2 ? 2 : ਸੰਸਕਰਣ_2);

    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(interface, ext_ਅੰਕੜੇ_control_manager_v1_interface.name) == 0)
	ct->gobjects.ext_ਅੰਕੜੇ_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &ext_ਅੰਕੜੇ_control_manager_v1_interface, 1);
#ਜੇ_ਅੰਤ // FEAT_WAYLAND_CLIPBOARD

}

/*
 * Called when a global object is removed, if so, then do nothing. This is to
 * avoid a global being removed while it is in the process of being used. Let
 * the user call :wlrestore in order to reset everything. Requests to that
 * global will just be ignored on the compositor side.
 */
    ਸਥਿਰ ਖਾਲੀ
wl_registry_listener_event_global_remove(
	ਖਾਲੀ		    *ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
	ਬਣਤਰ wl_registry  *registry ਅਣਵਰਤਿਆ,
	uint32_t	    name ਅਣਵਰਤਿਆ)
{
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

ਸਥਿਰ ਖਾਲੀ vwl_connection_destroy(vwl_connection_T *self);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    ਜੇ (ct->gobjects.object != ਨਿਰਾ) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = ਨਿਰਾ; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != ਨਿਰਾ)

#ਜੇ_ਅੰਤ

// Make sure to call wayland_set_display(display);
    ਸਥਿਰ vwl_connection_T *
vwl_connection_new(ਅਚਲ ਅੱਖਰ *display)
{
    vwl_connection_T	*ct;

    ਜੇ (wayland_no_connect)
	ਵਾਪਸ ਨਿਰਾ;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    ਜੇ (mch_getenv("XDG_RUNTIME_DIR") == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    ਜੇ (ct == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Must set log handler before we connect display in order to work.
    wl_log_ਸੈੱਟ_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    ਜੇ (ct->display.proxy == ਨਿਰਾ)
    {
	vim_free(ct);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    ਜੇ (ct->registry.proxy == ਨਿਰਾ)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ga_init2(&ct->gobjects.seats, ਆਕਾਰ(vwl_seat_T *), 1);

    wl_registry_ਜੋੜੋ_listener(ct->registry.proxy, &wl_registry_listener, ct);

    ਜੇ (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ ct;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    ਸਥਿਰ ਖਾਲੀ
vwl_seat_destroy(vwl_seat_T *self)
{
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ;
    ਜੇ (self->proxy != ਨਿਰਾ)
    {
	ਜੇ (wl_seat_get_ਸੰਸਕਰਣ(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	ਨਹੀਂਤਾਂ
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    ਸਥਿਰ ਖਾਲੀ
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_ਅੰਕੜੇ_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_ਅੰਕੜੇ_control_manager_v1)
# endif

    ਲਈ (ਪੂਰਨ i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_ਅੰਕੜੇ)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    ਜੇ (self->registry.proxy != ਨਿਰਾ)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = ਨਿਰਾ;
    }
    ਜੇ (self->display.proxy != ਨਿਰਾ)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = ਨਿਰਾ;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ਅਚਲ ਅੱਖਰ *label)
{
    ਜੇ ((STRCMP(label, "") == 0 || label == ਨਿਰਾ)
	    && self->gobjects.seats.ga_len > 0)
	ਵਾਪਸ ((vwl_seat_T **)self->gobjects.seats.ga_ਅੰਕੜੇ)[0];

    ਲਈ (ਪੂਰਨ i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_ਅੰਕੜੇ)[i];
	ਜੇ (STRCMP(seat->label, label) == 0)
	    ਵਾਪਸ seat;
    }
    ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅੰਤ

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    ਸਥਿਰ ਖਾਲੀ
wayland_ਸੈੱਟ_display(ਅਚਲ ਅੱਖਰ *display)
{
    ਜੇ (display == ਨਿਰਾ)
	display = (ਅੱਖਰ*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    ਨਹੀਂਤਾਂ ਜੇ (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;

    ਜੇ (display == ਨਿਰਾ)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    ਜੇ (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (ਅੱਖਰ*)vim_strsave((char_u*)display);
    }

ਬਾਹਰ_ਨਿਕਲੋ:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਸੈੱਟ_vim_var_string(VV_WAYLAND_DISPLAY, (char_u*)display, -1);
#ਜੇ_ਅੰਤ
}

/*
 * Initializes the global Wayland connection. Connects to the Wayland display
 * with given name and binds to global objects as needed. If display is NULL
 * then the $WAYLAND_DISPLAY environment variable will be used (handled by
 * libwayland). Returns FAIL on failure and OK on
 * success
 */
    ਪੂਰਨ
wayland_init_connection(ਅਚਲ ਅੱਖਰ *display)
{
    wayland_ਸੈੱਟ_display(display);

    wayland_ct = vwl_connection_new(display);

    ਜੇ (wayland_ct == ਨਿਰਾ)
	ਜਾਓ fail;

    ਵਾਪਸ OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_ਸੈੱਟ_display("");
    ਵਾਪਸ FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    ਖਾਲੀ
wayland_uninit_connection(ਖਾਲੀ)
{
    ਜੇ (wayland_ct == ਨਿਰਾ)
	ਵਾਪਸ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#ਜੇ_ਅੰਤ
    vwl_connection_destroy(wayland_ct);
    wayland_ct = ਨਿਰਾ;
    is_reading = false;
    wayland_ਸੈੱਟ_display("");
}

ਸਥਿਰ ਪੂਰਨ wayland_ct_restore_ਗਿਣਤੀ = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    ਸਥਿਰ ਖਾਲੀ
wayland_restore_connection(ਖਾਲੀ)
{
    // No point in restoring the connection if we are exiting or dying.
    ਜੇ (exiting || v_dying || wayland_ct_restore_ਗਿਣਤੀ <= 0)
	wayland_ਸੈੱਟ_display("");

    --wayland_ct_restore_ਗਿਣਤੀ;
    wayland_uninit_connection();

    ਜੇ (wayland_init_connection(wayland_display_name) == OK)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#ਜੇ_ਅੰਤ
    }
}

/*
 * Should be called before polling (select or poll) the global Wayland
 * connection display fd. Returns fd on success and -1 on failure.
 */
    ਪੂਰਨ
wayland_prepare_read(ਖਾਲੀ)
{
    ਜੇ (wayland_ct == ਨਿਰਾ)
	ਵਾਪਸ -1;

    ਜੇ (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    ਜਦੋਂ (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	ਜੇ (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    ਵਾਪਸ -1;

    ਜੇ (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	ਵਾਪਸ -1;
    }

    is_reading = true;

    ਵਾਪਸ wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    ਪੂਰਨ
wayland_update(ਖਾਲੀ)
{
    ਜੇ (wayland_ct == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ vwl_connection_roundtrip(wayland_ct);
}

#ਜੇ !defined(HAVE_SELECT)

    ਖਾਲੀ
wayland_poll_check(ਪੂਰਨ revents)
{
    ਜੇ (wayland_ct == ਨਿਰਾ)
	ਵਾਪਸ;

    is_reading = false;
    ਜੇ (revents & POLLIN)
    {
	ਜੇ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    ਵਾਪਸ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    ਨਹੀਂਤਾਂ
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	ਵਾਪਸ;
    }
    wayland_restore_connection();
}

#ਜੇ_ਅੰਤ
#ਜੇ defined(HAVE_SELECT)

    ਖਾਲੀ
wayland_select_check(bool is_ਸੈੱਟ)
{
    ਜੇ (wayland_ct == ਨਿਰਾ)
	ਵਾਪਸ;

    is_reading = false;
    ਜੇ (is_ਸੈੱਟ)
    {
	ਜੇ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	ਨਹੀਂਤਾਂ
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    ਨਹੀਂਤਾਂ
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#ਜੇ_ਅੰਤ

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    ਖਾਲੀ
ex_wlrestore(exarg_T *eap)
{
    ਅੱਖਰ *display;

    ਜੇ (eap->arg == ਨਿਰਾ || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    ਨਹੀਂਤਾਂ
	display = (ਅੱਖਰ*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    ਜੇ (!eap->forceit && wayland_ct != ਨਿਰਾ &&
	    (display == wayland_display_name ||
	     (wayland_display_name != ਨਿਰਾ &&
	      STRCMP(wayland_display_name, display) == 0)))
	ਵਾਪਸ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
    ਜੇ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	ਜੇ (clip_ਤਾਰਾ.owned)
	    clip_lose_selection(&clip_ਤਾਰਾ);
	ਜੇ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (display != ਨਿਰਾ)
	display = (ਅੱਖਰ*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_ਗਿਣਤੀ = 5;

    ਜੇ (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#ਜੇ_ਅੰਤ
    }
    ਨਹੀਂਤਾਂ
	ਸੁਨੇਹਾ(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    ਚੁਣੋ_clipmethod();
}

#ਜੇ defined(FEAT_WAYLAND_CLIPBOARD)

/*
 * Get a suitable data device manager from connection. "supported" should be
 * initialized to VWL_DATA_PROTOCOL_NONE beforehand. Returns NULL if there are
 * no data device manager available with the required selection.
 */
    vwl_ਅੰਕੜੇ_device_manager_T *
vwl_connection_get_ਅੰਕੜੇ_device_manager(
	vwl_connection_T *self,
	wayland_selection_T req_sel,
	int_u *supported)
{
    vwl_ਅੰਕੜੇ_device_manager_T *manager =
	ALLOC_CLEAR_ONE(vwl_ਅੰਕੜੇ_device_manager_T);

    // Prioritize ext-data-control-v1 over wlr-data-control-unstable-v1 because
    // it is newer.
    ਜੇ (self->gobjects.ext_ਅੰਕੜੇ_control_manager_v1 != ਨਿਰਾ)
    {
	manager->proxy = self->gobjects.ext_ਅੰਕੜੇ_control_manager_v1;
	manager->protocol = VWL_ਅੰਕੜੇ_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    ਨਹੀਂਤਾਂ ਜੇ (self->gobjects.zwlr_ਅੰਕੜੇ_control_manager_v1 != ਨਿਰਾ)
    {
	manager->proxy = self->gobjects.zwlr_ਅੰਕੜੇ_control_manager_v1;
	manager->protocol = VWL_ਅੰਕੜੇ_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	ਜੇ (zwlr_ਅੰਕੜੇ_control_manager_v1_get_ਸੰਸਕਰਣ(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    ਜੇ (!(*supported & req_sel))
    {
	vim_free(manager);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ manager;
}

    vwl_ਅੰਕੜੇ_device_T *
vwl_ਅੰਕੜੇ_device_manager_get_ਅੰਕੜੇ_device(
	vwl_ਅੰਕੜੇ_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_ਅੰਕੜੇ_device_T *device = ALLOC_CLEAR_ONE(vwl_ਅੰਕੜੇ_device_T);

    ਚੋਣ (self->protocol)
    {
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT:
	    device->proxy = ext_ਅੰਕੜੇ_control_manager_v1_get_ਅੰਕੜੇ_device(
		    self->proxy, seat->proxy);
	    ਰੋਕੋ;
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR:
	    device->proxy = zwlr_ਅੰਕੜੇ_control_manager_v1_get_ਅੰਕੜੇ_device(
		    self->proxy, seat->proxy);
	    ਰੋਕੋ;
	ਮੂਲ:
	    vim_free(device);
	    ਵਾਪਸ ਨਿਰਾ;
    }
    device->protocol = self->protocol;

    ਵਾਪਸ device;
}

    vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *
vwl_ਅੰਕੜੇ_device_manager_create_ਅੰਕੜੇ_ਸਰੋਤ(vwl_ਅੰਕੜੇ_device_manager_T *self)
{
    vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *ਸਰੋਤ = ALLOC_CLEAR_ONE(vwl_ਅੰਕੜੇ_ਸਰੋਤ_T);

    ਚੋਣ (self->protocol)
    {
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT:
	    ਸਰੋਤ->proxy = ext_ਅੰਕੜੇ_control_manager_v1_create_ਅੰਕੜੇ_ਸਰੋਤ(
		    self->proxy);
	    ਰੋਕੋ;
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR:
	    ਸਰੋਤ->proxy = zwlr_ਅੰਕੜੇ_control_manager_v1_create_ਅੰਕੜੇ_ਸਰੋਤ(
		    self->proxy);
	    ਰੋਕੋ;
	ਮੂਲ:
	    vim_free(ਸਰੋਤ);
	    ਵਾਪਸ ਨਿਰਾ;
    }
    ਸਰੋਤ->protocol = self->protocol;

    ਵਾਪਸ ਸਰੋਤ;
}

    ਸਥਿਰ vwl_ਅੰਕੜੇ_offer_T *
vwl_ਅੰਕੜੇ_device_wrap_offer_proxy(vwl_ਅੰਕੜੇ_device_T *self, ਖਾਲੀ *proxy)
{
    vwl_ਅੰਕੜੇ_offer_T *offer = ALLOC_CLEAR_ONE(vwl_ਅੰਕੜੇ_offer_T);

    ਜੇ (offer == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->ਅੰਕੜੇ = self->ਅੰਕੜੇ;
    ga_init2(&offer->mime_types, ਆਕਾਰ(ਅੱਖਰ *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    ਵਾਪਸ offer;
}

# define VWL_FUNC_ਅੰਕੜੇ_PROXY_DESTROY(type) \
	ਖਾਲੀ \
    vwl_ਅੰਕੜੇ##type##_destroy(vwl_ਅੰਕੜੇ##type##_T *self) \
    { \
	ਜੇ (self == ਨਿਰਾ) \
	    ਵਾਪਸ; \
	ਚੋਣ (self->protocol) \
	{ \
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT: \
		ext_ਅੰਕੜੇ_control##type##_v1_destroy(self->proxy); \
		ਰੋਕੋ; \
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR: \
		zwlr_ਅੰਕੜੇ_control##type##_v1_destroy(self->proxy); \
		ਰੋਕੋ; \
	    ਮੂਲ: \
		ਰੋਕੋ; \
	} \
	vim_free(self); \
    }

VWL_FUNC_ਅੰਕੜੇ_PROXY_DESTROY(device)
VWL_FUNC_ਅੰਕੜੇ_PROXY_DESTROY(ਸਰੋਤ)

    ਖਾਲੀ
vwl_ਅੰਕੜੇ_offer_destroy(vwl_ਅੰਕੜੇ_offer_T *self)
{
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ;
    ਚੋਣ (self->protocol)
    {
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT:
	    ext_ਅੰਕੜੇ_control_offer_v1_destroy(self->proxy);
	    ਰੋਕੋ;
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR:
	    zwlr_ਅੰਕੜੇ_control_offer_v1_destroy(self->proxy);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
    ga_clear_ਲੜੀਆਂ(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
ਖਾਲੀ
vwl_ਅੰਕੜੇ_device_manager_discard(vwl_ਅੰਕੜੇ_device_manager_T *self)
{
    ਜੇ (self == ਨਿਰਾ)
	ਵਾਪਸ;
    vim_free(self);
}

# define VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_ਅੰਕੜੇ_OFFER(device_type, offer_type) \
	ਸਥਿਰ ਖਾਲੀ \
    device_type##_listener_event_data_offer( \
	    ਖਾਲੀ *ਅੰਕੜੇ, \
	    ਬਣਤਰ device_type *device ਅਣਵਰਤਿਆ, \
	    ਬਣਤਰ offer_type *offer) \
    { \
	vwl_ਅੰਕੜੇ_device_T *self = ਅੰਕੜੇ; \
	self->offer = vwl_ਅੰਕੜੇ_device_wrap_offer_proxy(self, offer); \
	self->listener->ਅੰਕੜੇ_offer(self->ਅੰਕੜੇ, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	ਸਥਿਰ ਖਾਲੀ \
    device_type##_listener_event_selection( \
	    ਖਾਲੀ *ਅੰਕੜੇ, \
	    ਬਣਤਰ device_type *device ਅਣਵਰਤਿਆ, \
	    ਬਣਤਰ offer_type *offer ਅਣਵਰਤਿਆ) \
    { \
	vwl_ਅੰਕੜੇ_device_T *self = ਅੰਕੜੇ; \
	self->listener->selection(self->ਅੰਕੜੇ, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = ਨਿਰਾ; \
    } \

# define VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	ਸਥਿਰ ਖਾਲੀ \
    device_type##_listener_event_primary_selection( \
	    ਖਾਲੀ *ਅੰਕੜੇ, \
	    ਬਣਤਰ device_type *device ਅਣਵਰਤਿਆ, \
	    ਬਣਤਰ offer_type *offer ਅਣਵਰਤਿਆ) \
    { \
	vwl_ਅੰਕੜੇ_device_T *self = ਅੰਕੜੇ; \
	self->listener->selection(self->ਅੰਕੜੇ, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = ਨਿਰਾ; \
    }

# define VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_ਮੁਕੰਮਲ(device_type) \
	ਸਥਿਰ ਖਾਲੀ \
    device_type##_listener_event_finished( \
	    ਖਾਲੀ *ਅੰਕੜੇ, \
	    ਬਣਤਰ device_type *device ਅਣਵਰਤਿਆ) \
    { \
	vwl_ਅੰਕੜੇ_device_T *self = ਅੰਕੜੇ; \
	self->listener->ਮੁਕੰਮਲ(self->ਅੰਕੜੇ, self); \
    }

VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_ਅੰਕੜੇ_OFFER(
    ext_ਅੰਕੜੇ_control_device_v1, ext_ਅੰਕੜੇ_control_offer_v1)
VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_ਅੰਕੜੇ_OFFER(
    zwlr_ਅੰਕੜੇ_control_device_v1, zwlr_ਅੰਕੜੇ_control_offer_v1)

VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_SELECTION(
    ext_ਅੰਕੜੇ_control_device_v1, ext_ਅੰਕੜੇ_control_offer_v1
)
VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_SELECTION(
    zwlr_ਅੰਕੜੇ_control_device_v1, zwlr_ਅੰਕੜੇ_control_offer_v1
)
VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_PRIMARY_SELECTION(
    ext_ਅੰਕੜੇ_control_device_v1, ext_ਅੰਕੜੇ_control_offer_v1
)
VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_PRIMARY_SELECTION(
    zwlr_ਅੰਕੜੇ_control_device_v1, zwlr_ਅੰਕੜੇ_control_offer_v1
)

VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_ਮੁਕੰਮਲ(ext_ਅੰਕੜੇ_control_device_v1)
VWL_FUNC_ਅੰਕੜੇ_DEVICE_EVENT_ਮੁਕੰਮਲ(zwlr_ਅੰਕੜੇ_control_device_v1)

ਸਥਿਰ ਬਣਤਰ ext_ਅੰਕੜੇ_control_device_v1_listener
    ext_ਅੰਕੜੇ_control_device_v1_listener = {
	.ਅੰਕੜੇ_offer = ext_ਅੰਕੜੇ_control_device_v1_listener_event_ਅੰਕੜੇ_offer,
	.selection = ext_ਅੰਕੜੇ_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_ਅੰਕੜੇ_control_device_v1_listener_event_primary_selection,
	.ਮੁਕੰਮਲ = ext_ਅੰਕੜੇ_control_device_v1_listener_event_ਮੁਕੰਮਲ
};
ਸਥਿਰ ਅਚਲ ਬਣਤਰ zwlr_ਅੰਕੜੇ_control_device_v1_listener
    zwlr_ਅੰਕੜੇ_control_device_v1_listener = {
	.ਅੰਕੜੇ_offer = zwlr_ਅੰਕੜੇ_control_device_v1_listener_event_ਅੰਕੜੇ_offer,
	.selection = zwlr_ਅੰਕੜੇ_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_ਅੰਕੜੇ_control_device_v1_listener_event_primary_selection,
	.ਮੁਕੰਮਲ = zwlr_ਅੰਕੜੇ_control_device_v1_listener_event_ਮੁਕੰਮਲ
};

# define VWL_FUNC_ਅੰਕੜੇ_ਸਰੋਤ_EVENT_SEND(ਸਰੋਤ_type) \
	ਸਥਿਰ ਖਾਲੀ \
    ਸਰੋਤ_type##_listener_event_send( \
	    ਖਾਲੀ *ਅੰਕੜੇ, ਬਣਤਰ ਸਰੋਤ_type *ਸਰੋਤ ਅਣਵਰਤਿਆ, \
	    ਅਚਲ ਅੱਖਰ *mime_type, ਪੂਰਨ fd) \
    { \
	vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *self = ਅੰਕੜੇ; \
	self->listener->send(self->ਅੰਕੜੇ, self, mime_type, fd); \
    }

# define VWL_FUNC_ਅੰਕੜੇ_ਸਰੋਤ_EVENT_CANCELLED(ਸਰੋਤ_type) \
	ਸਥਿਰ ਖਾਲੀ \
    ਸਰੋਤ_type##_listener_event_cancelled( \
	    ਖਾਲੀ *ਅੰਕੜੇ, ਬਣਤਰ ਸਰੋਤ_type *ਸਰੋਤ ਅਣਵਰਤਿਆ) \
    { \
	vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *self = ਅੰਕੜੇ; \
	self->listener->cancelled(self->ਅੰਕੜੇ, self); \
    }

VWL_FUNC_ਅੰਕੜੇ_ਸਰੋਤ_EVENT_SEND(ext_ਅੰਕੜੇ_control_ਸਰੋਤ_v1)
VWL_FUNC_ਅੰਕੜੇ_ਸਰੋਤ_EVENT_SEND(zwlr_ਅੰਕੜੇ_control_ਸਰੋਤ_v1)

VWL_FUNC_ਅੰਕੜੇ_ਸਰੋਤ_EVENT_CANCELLED(ext_ਅੰਕੜੇ_control_ਸਰੋਤ_v1)
VWL_FUNC_ਅੰਕੜੇ_ਸਰੋਤ_EVENT_CANCELLED(zwlr_ਅੰਕੜੇ_control_ਸਰੋਤ_v1)

ਸਥਿਰ ਅਚਲ ਬਣਤਰ ext_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener
    ext_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener = {
	.send = ext_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener_event_send,
	.cancelled = ext_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener_event_cancelled
};
ਸਥਿਰ ਅਚਲ ਬਣਤਰ zwlr_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener
    zwlr_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener = {
	.send = zwlr_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener_event_send,
	.cancelled = zwlr_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_listener_event_cancelled
};

# define VWL_FUNC_ਅੰਕੜੇ_OFFER_EVENT_OFFER(offer_type) \
	ਸਥਿਰ ਖਾਲੀ \
    offer_type##_listener_event_offer( \
	    ਖਾਲੀ *ਅੰਕੜੇ, \
	    ਬਣਤਰ offer_type *offer ਅਣਵਰਤਿਆ, \
	    ਅਚਲ ਅੱਖਰ *mime_type) \
    { \
	vwl_ਅੰਕੜੇ_offer_T *self = ਅੰਕੜੇ; \
	ਜੇ (self->listener->offer(self->ਅੰਕੜੇ, self, mime_type)) \
	{ \
	    ਅੱਖਰ *mime = (ਅੱਖਰ *)vim_strsave((char_u *)mime_type); \
	    ਜੇ (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    ਨਹੀਂਤਾਂ \
		ਜੇ (mime != ਨਿਰਾ) \
		    ((ਅੱਖਰ **)self->mime_types.ga_ਅੰਕੜੇ) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_ਅੰਕੜੇ_OFFER_EVENT_OFFER(ext_ਅੰਕੜੇ_control_offer_v1)
VWL_FUNC_ਅੰਕੜੇ_OFFER_EVENT_OFFER(zwlr_ਅੰਕੜੇ_control_offer_v1)

ਸਥਿਰ ਅਚਲ ਬਣਤਰ ext_ਅੰਕੜੇ_control_offer_v1_listener
    ext_ਅੰਕੜੇ_control_offer_v1_listener = {
	.offer = ext_ਅੰਕੜੇ_control_offer_v1_listener_event_offer
};
ਸਥਿਰ ਅਚਲ ਬਣਤਰ zwlr_ਅੰਕੜੇ_control_offer_v1_listener
    zwlr_ਅੰਕੜੇ_control_offer_v1_listener = {
	.offer = zwlr_ਅੰਕੜੇ_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_ਅੰਕੜੇ_PROXY_ਜੋੜੋ_LISTENER(type) \
	ਖਾਲੀ \
    vwl_ਅੰਕੜੇ##type##_add_listener( \
	    vwl_ਅੰਕੜੇ##type##_T *self, \
	    ਅਚਲ vwl_ਅੰਕੜੇ##type##_listener_T *listener, \
	    ਖਾਲੀ *ਅੰਕੜੇ) \
    { \
	ਜੇ (self == ਨਿਰਾ) \
	    ਵਾਪਸ; \
	self->ਅੰਕੜੇ = ਅੰਕੜੇ; \
	self->listener = listener; \
	ਚੋਣ (self->protocol) \
	{ \
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT: \
		ext_ਅੰਕੜੇ_control##type##_v1_add_listener(self->proxy, \
			&ext_ਅੰਕੜੇ_control##type##_v1_listener, self); \
		ਰੋਕੋ; \
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR: \
		zwlr_ਅੰਕੜੇ_control##type##_v1_add_listener(self->proxy, \
			&zwlr_ਅੰਕੜੇ_control##type##_v1_listener, self); \
		ਰੋਕੋ; \
	    ਮੂਲ: \
		ਰੋਕੋ; \
	} \
    }

VWL_FUNC_ਅੰਕੜੇ_PROXY_ਜੋੜੋ_LISTENER(device)
VWL_FUNC_ਅੰਕੜੇ_PROXY_ਜੋੜੋ_LISTENER(ਸਰੋਤ)
VWL_FUNC_ਅੰਕੜੇ_PROXY_ਜੋੜੋ_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    ਖਾਲੀ
vwl_ਅੰਕੜੇ_device_ਸੈੱਟ_selection(
    vwl_ਅੰਕੜੇ_device_T *self,
    vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *ਸਰੋਤ,
    uint32_t serial ਅਣਵਰਤਿਆ,
    wayland_selection_T selection
)
{
    ਖਾਲੀ *proxy = ਸਰੋਤ == ਨਿਰਾ ? ਨਿਰਾ : ਸਰੋਤ->proxy;

    ਜੇ (selection == WAYLAND_SELECTION_REGULAR)
    {
	ਚੋਣ (self->protocol)
	{
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT:
		ext_ਅੰਕੜੇ_control_device_v1_ਸੈੱਟ_selection(self->proxy, proxy);
		ਰੋਕੋ;
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR:
		zwlr_ਅੰਕੜੇ_control_device_v1_ਸੈੱਟ_selection(self->proxy, proxy);
		ਰੋਕੋ;
	    ਮੂਲ:
		ਰੋਕੋ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (selection == WAYLAND_SELECTION_PRIMARY)
    {
	ਚੋਣ (self->protocol)
	{
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT:
		ext_ਅੰਕੜੇ_control_device_v1_ਸੈੱਟ_primary_selection(
			self->proxy, proxy
			);
		ਰੋਕੋ;
	    ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR:
		zwlr_ਅੰਕੜੇ_control_device_v1_ਸੈੱਟ_primary_selection(
			self->proxy, proxy
			);
		ਰੋਕੋ;
	    ਮੂਲ:
		ਰੋਕੋ;
	}
    }
}

    ਖਾਲੀ
vwl_ਅੰਕੜੇ_ਸਰੋਤ_offer(vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *self, ਅਚਲ ਅੱਖਰ *mime_type)
{
    ਚੋਣ (self->protocol)
    {
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT:
	    ext_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_offer(self->proxy, mime_type);
	    ਰੋਕੋ;
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR:
	    zwlr_ਅੰਕੜੇ_control_ਸਰੋਤ_v1_offer(self->proxy, mime_type);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
}

    ਖਾਲੀ
vwl_ਅੰਕੜੇ_offer_receive(
	vwl_ਅੰਕੜੇ_offer_T *self,
	ਅਚਲ ਅੱਖਰ *mime_type,
	int32_t fd)
{
    ਚੋਣ (self->protocol)
    {
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_EXT:
	    ext_ਅੰਕੜੇ_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ਰੋਕੋ;
	ਹਾਲਤ VWL_ਅੰਕੜੇ_PROTOCOL_WLR:
	    zwlr_ਅੰਕੜੇ_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਰੋਕੋ;
    }
}

#ਜੇ_ਅੰਤ // FEAT_WAYLAND_CLIPBOARD

#ਜੇ_ਅੰਤ // FEAT_WAYLAND
