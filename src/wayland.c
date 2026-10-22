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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_WAYLAND

#ಅನ್ತರ್ಭಾವ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    ಪೂರ್ಣಾಙ್ಕ
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ಸಂರಚನಾ pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#ಅನ್ಯಥಾ
    fd_set	    wfds;
    ಸಂರಚನಾ timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    ಯದಿ (self->display.proxy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    ಯಾವತ್ (true)
    {
	ಪೂರ್ಣಾಙ್ಕ ret = wl_display_flush(self->display.proxy);

	ಯದಿ (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    ಯದಿ (poll(&fds, 1, p_wtm) <= 0)
#ಅನ್ಯಥಾ
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    ಯದಿ (select(self->display.fd + 1, ರಿಕ್ತ, &wfds, ರಿಕ್ತ, &tv) <= 0)
		ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
	}
	ಅನ್ಯಥಾ ಯದಿ (ret == -1)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ಸಂರಚನಾ pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#ಅನ್ಯಥಾ
    fd_set	    rfds;
    ಸಂರಚನಾ timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    ಯದಿ (self->display.proxy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯಾವತ್ (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	ಯದಿ (wl_display_dispatch_pending(self->display.proxy) == -1)
	    ಪ್ರತ್ಯಾಗಮ -1;

    // Send any requests before we starting blocking to read display fd
    ಯದಿ (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    ಯದಿ (poll(&fds, 1, p_wtm) <= 0)
#ಅನ್ಯಥಾ
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    ಯದಿ (select(self->display.fd + 1, &rfds, ರಿಕ್ತ, ರಿಕ್ತ, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    // Read events into the queue
    ಯದಿ (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	ಪ್ರತ್ಯಾಗಮ -1;

    // Dispatch those events (call the handlers associated for each event)
    ಪ್ರತ್ಯಾಗಮ wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_callback_event_done(ಶೂನ್ಯ *data, ಸಂರಚನಾ wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    ಪೂರ್ಣಾಙ್ಕ
vwl_connection_roundtrip(vwl_connection_T *self)
{
    ಸಂರಚನಾ wl_callback	*callback;
    ಪೂರ್ಣಾಙ್ಕ			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    ಯದಿ (self->display.proxy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    ಯದಿ (callback == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    ಯಾವತ್ (true)
    {
	ret = vwl_connection_dispatch(self);

	ಯದಿ (done || ret == -1)
	    ಭಙ್ಗ;

#ifdef ELAPSED_FUNC
	ಯದಿ (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    ಭಙ್ಗ;
	}
#endif
    }

    ಯದಿ (ret == -1)
    {
	ಯದಿ (!done)
	    wl_callback_destroy(callback);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_log_handler(ಧ್ರುವ ವರ್ಣ *fmt, va_list args)
{
    // 512 bytes should be big enough
    ವರ್ಣ	*buf	= alloc(512);
    ವರ್ಣ	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(ರಿಕ್ತ, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wl_seat_listener_event_name(
	ಶೂನ್ಯ		*data,
	ಸಂರಚನಾ wl_seat	*seat_proxy UNUSED,
	ಧ್ರುವ ವರ್ಣ	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (ವರ್ಣ *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wl_seat_listener_event_capabilities(
	ಶೂನ್ಯ		*data,
	ಸಂರಚನಾ wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

ಸ್ಥಿರ ಶೂನ್ಯ vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wl_registry_listener_event_global(
	ಶೂನ್ಯ		    *data,
	ಸಂರಚನಾ wl_registry  *registry,
	uint32_t	    name,
	ಧ್ರುವ ವರ್ಣ	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    ಯದಿ (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	ಸಂರಚನಾ wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	ಯದಿ (seat_proxy == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	ಯದಿ (seat == ರಿಕ್ತ || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	ಯದಿ (vwl_connection_roundtrip(ct) == FAIL || seat->label == ರಿಕ್ತ)
	{
	    vwl_seat_destroy(seat);
	    ಪ್ರತ್ಯಾಗಮ;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    ಅನ್ಯಥಾ ಯದಿ (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
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
    ಸ್ಥಿರ ಶೂನ್ಯ
wl_registry_listener_event_global_remove(
	ಶೂನ್ಯ		    *data UNUSED,
	ಸಂರಚನಾ wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

ಸ್ಥಿರ ಶೂನ್ಯ vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    ಯದಿ (ct->gobjects.object != ರಿಕ್ತ) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = ರಿಕ್ತ; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != ರಿಕ್ತ)

#endif

// Make sure to call wayland_set_display(display);
    ಸ್ಥಿರ vwl_connection_T *
vwl_connection_new(ಧ್ರುವ ವರ್ಣ *display)
{
    vwl_connection_T	*ct;

    ಯದಿ (wayland_no_connect)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    ಯದಿ (mch_getenv("XDG_RUNTIME_DIR") == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    ಯದಿ (ct == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    ಯದಿ (ct->display.proxy == ರಿಕ್ತ)
    {
	vim_free(ct);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    ಯದಿ (ct->registry.proxy == ರಿಕ್ತ)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ga_init2(&ct->gobjects.seats, ಪರಿಮಾಣ(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    ಯದಿ (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_seat_destroy(vwl_seat_T *self)
{
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (self->proxy != ರಿಕ್ತ)
    {
	ಯದಿ (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	ಅನ್ಯಥಾ
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    ಯದಿ (self->registry.proxy != ರಿಕ್ತ)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = ರಿಕ್ತ;
    }
    ಯದಿ (self->display.proxy != ರಿಕ್ತ)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = ರಿಕ್ತ;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ಧ್ರುವ ವರ್ಣ *label)
{
    ಯದಿ ((STRCMP(label, "") == 0 || label == ರಿಕ್ತ)
	    && self->gobjects.seats.ga_len > 0)
	ಪ್ರತ್ಯಾಗಮ ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	ಯದಿ (STRCMP(seat->label, label) == 0)
	    ಪ್ರತ್ಯಾಗಮ seat;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wayland_set_display(ಧ್ರುವ ವರ್ಣ *display)
{
    ಯದಿ (display == ರಿಕ್ತ)
	display = (ವರ್ಣ*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    ಅನ್ಯಥಾ ಯದಿ (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	ಗಚ್ಛ exit;

    ಯದಿ (display == ರಿಕ್ತ)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    ಯದಿ (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (ವರ್ಣ*)vim_strsave((char_u*)display);
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
    ಪೂರ್ಣಾಙ್ಕ
wayland_init_connection(ಧ್ರುವ ವರ್ಣ *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    ಯದಿ (wayland_ct == ರಿಕ್ತ)
	ಗಚ್ಛ fail;

    ಪ್ರತ್ಯಾಗಮ OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    ಶೂನ್ಯ
wayland_uninit_connection(ಶೂನ್ಯ)
{
    ಯದಿ (wayland_ct == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = ರಿಕ್ತ;
    is_reading = false;
    wayland_set_display("");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ wayland_ct_restore_ಎಣಿಕೆ = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wayland_restore_connection(ಶೂನ್ಯ)
{
    // No point in restoring the connection if we are exiting or dying.
    ಯದಿ (exiting || v_dying || wayland_ct_restore_ಎಣಿಕೆ <= 0)
	wayland_set_display("");

    --wayland_ct_restore_ಎಣಿಕೆ;
    wayland_uninit_connection();

    ಯದಿ (wayland_init_connection(wayland_display_name) == OK)
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
    ಪೂರ್ಣಾಙ್ಕ
wayland_prepare_read(ಶೂನ್ಯ)
{
    ಯದಿ (wayland_ct == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    ಯಾವತ್ (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	ಯದಿ (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    ಪ್ರತ್ಯಾಗಮ -1;

    ಯದಿ (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    is_reading = true;

    ಪ್ರತ್ಯಾಗಮ wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    ಪೂರ್ಣಾಙ್ಕ
wayland_update(ಶೂನ್ಯ)
{
    ಯದಿ (wayland_ct == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ vwl_connection_roundtrip(wayland_ct);
}

#ಯದಿ !defined(HAVE_SELECT)

    ಶೂನ್ಯ
wayland_poll_check(ಪೂರ್ಣಾಙ್ಕ revents)
{
    ಯದಿ (wayland_ct == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    is_reading = false;
    ಯದಿ (revents & POLLIN)
    {
	ಯದಿ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    ಅನ್ಯಥಾ
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	ಪ್ರತ್ಯಾಗಮ;
    }
    wayland_restore_connection();
}

#endif
#ಯದಿ defined(HAVE_SELECT)

    ಶೂನ್ಯ
wayland_select_check(bool is_set)
{
    ಯದಿ (wayland_ct == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    is_reading = false;
    ಯದಿ (is_set)
    {
	ಯದಿ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	ಅನ್ಯಥಾ
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    ಅನ್ಯಥಾ
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    ಶೂನ್ಯ
ex_wlrestore(exarg_T *eap)
{
    ವರ್ಣ *display;

    ಯದಿ (eap->arg == ರಿಕ್ತ || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    ಅನ್ಯಥಾ
	display = (ವರ್ಣ*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    ಯದಿ (!eap->forceit && wayland_ct != ರಿಕ್ತ &&
	    (display == wayland_display_name ||
	     (wayland_display_name != ರಿಕ್ತ &&
	      STRCMP(wayland_display_name, display) == 0)))
	ಪ್ರತ್ಯಾಗಮ;

#ifdef FEAT_WAYLAND_CLIPBOARD
    ಯದಿ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	ಯದಿ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ಯದಿ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    ಯದಿ (display != ರಿಕ್ತ)
	display = (ವರ್ಣ*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_ಎಣಿಕೆ = 5;

    ಯದಿ (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    ಅನ್ಯಥಾ
	ಸಂದೇಶ(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#ಯದಿ defined(FEAT_WAYLAND_CLIPBOARD)

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
    ಯದಿ (self->gobjects.ext_data_control_manager_v1 != ರಿಕ್ತ)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    ಅನ್ಯಥಾ ಯದಿ (self->gobjects.zwlr_data_control_manager_v1 != ರಿಕ್ತ)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	ಯದಿ (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    ಯದಿ (!(*supported & req_sel))
    {
	vim_free(manager);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    ವಿಕಲ್ಪ (self->protocol)
    {
	ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    vim_free(device);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    device->protocol = self->protocol;

    ಪ್ರತ್ಯಾಗಮ device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    ವಿಕಲ್ಪ (self->protocol)
    {
	ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    vim_free(source);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    source->protocol = self->protocol;

    ಪ್ರತ್ಯಾಗಮ source;
}

    ಸ್ಥಿರ vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, ಶೂನ್ಯ *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    ಯದಿ (offer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, ಪರಿಮಾಣ(ವರ್ಣ *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    ಪ್ರತ್ಯಾಗಮ offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	ಶೂನ್ಯ \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	ಯದಿ (self == ರಿಕ್ತ) \
	    ಪ್ರತ್ಯಾಗಮ; \
	ವಿಕಲ್ಪ (self->protocol) \
	{ \
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		ಭಙ್ಗ; \
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		ಭಙ್ಗ; \
	    ಸ್ವಭಾವ: \
		ಭಙ್ಗ; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    ಶೂನ್ಯ
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ವಿಕಲ್ಪ (self->protocol)
    {
	ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
ಶೂನ್ಯ
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    ಯದಿ (self == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_data_offer( \
	    ಶೂನ್ಯ *data, \
	    ಸಂರಚನಾ device_type *device UNUSED, \
	    ಸಂರಚನಾ offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_selection( \
	    ಶೂನ್ಯ *data, \
	    ಸಂರಚನಾ device_type *device UNUSED, \
	    ಸಂರಚನಾ offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = ರಿಕ್ತ; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_primary_selection( \
	    ಶೂನ್ಯ *data, \
	    ಸಂರಚನಾ device_type *device UNUSED, \
	    ಸಂರಚನಾ offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = ರಿಕ್ತ; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_finished( \
	    ಶೂನ್ಯ *data, \
	    ಸಂರಚನಾ device_type *device UNUSED) \
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

ಸ್ಥಿರ ಸಂರಚನಾ ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    source_type##_listener_event_send( \
	    ಶೂನ್ಯ *data, ಸಂರಚನಾ source_type *source UNUSED, \
	    ಧ್ರುವ ವರ್ಣ *mime_type, ಪೂರ್ಣಾಙ್ಕ fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    source_type##_listener_event_cancelled( \
	    ಶೂನ್ಯ *data, ಸಂರಚನಾ source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    offer_type##_listener_event_offer( \
	    ಶೂನ್ಯ *data, \
	    ಸಂರಚನಾ offer_type *offer UNUSED, \
	    ಧ್ರುವ ವರ್ಣ *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	ಯದಿ (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    ವರ್ಣ *mime = (ವರ್ಣ *)vim_strsave((char_u *)mime_type); \
	    ಯದಿ (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    ಅನ್ಯಥಾ \
		ಯದಿ (mime != ರಿಕ್ತ) \
		    ((ವರ್ಣ **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	ಶೂನ್ಯ \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    ಧ್ರುವ vwl_data_##type##_listener_T *listener, \
	    ಶೂನ್ಯ *data) \
    { \
	ಯದಿ (self == ರಿಕ್ತ) \
	    ಪ್ರತ್ಯಾಗಮ; \
	self->data = data; \
	self->listener = listener; \
	ವಿಕಲ್ಪ (self->protocol) \
	{ \
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		ಭಙ್ಗ; \
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		ಭಙ್ಗ; \
	    ಸ್ವಭಾವ: \
		ಭಙ್ಗ; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    ಶೂನ್ಯ
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    ಶೂನ್ಯ *proxy = source == ರಿಕ್ತ ? ರಿಕ್ತ : source->proxy;

    ಯದಿ (selection == WAYLAND_SELECTION_REGULAR)
    {
	ವಿಕಲ್ಪ (self->protocol)
	{
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		ಭಙ್ಗ;
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (selection == WAYLAND_SELECTION_PRIMARY)
    {
	ವಿಕಲ್ಪ (self->protocol)
	{
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		ಭಙ್ಗ;
	    ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		ಭಙ್ಗ;
	}
    }
}

    ಶೂನ್ಯ
vwl_data_source_offer(vwl_data_source_T *self, ಧ್ರುವ ವರ್ಣ *mime_type)
{
    ವಿಕಲ್ಪ (self->protocol)
    {
	ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
}

    ಶೂನ್ಯ
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	ಧ್ರುವ ವರ್ಣ *mime_type,
	int32_t fd)
{
    ವಿಕಲ್ಪ (self->protocol)
    {
	ಪಕ್ಷ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
