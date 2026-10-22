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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND

#ಅನ್ತರ್ಭಾವ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    ಪೂರ್ಣಾಂಕ
vwl_connection_flush(vwl_connection_T *self)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SELECT
    ರಚನೆ pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#ಇಲ್ಲದಿದ್ದರೆ
    fd_set	    wfds;
    ರಚನೆ timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (self->display.proxy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    ಆಗ (true)
    {
	ಪೂರ್ಣಾಂಕ ret = wl_display_flush(self->display.proxy);

	ಆದರೆ (ret == -1 && errno == EAGAIN)
	{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SELECT
	    ಆದರೆ (poll(&fds, 1, p_wtm) <= 0)
#ಇಲ್ಲದಿದ್ದರೆ
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    ಆದರೆ (select(self->display.fd + 1, ಖಾಲಿ, &wfds, ಖಾಲಿ, &tv) <= 0)
		ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == -1)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    ಪೂರ್ಣಾಂಕ
vwl_connection_dispatch(vwl_connection_T *self)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SELECT
    ರಚನೆ pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#ಇಲ್ಲದಿದ್ದರೆ
    fd_set	    rfds;
    ರಚನೆ timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (self->display.proxy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆಗ (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	ಆದರೆ (wl_display_dispatch_pending(self->display.proxy) == -1)
	    ಹಿಂತಿರುಗಿ -1;

    // Send any requests before we starting blocking to read display fd
    ಆದರೆ (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	ಹಿಂತಿರುಗಿ -1;
    }

    // Poll until there is data to read from the display fd.
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_SELECT
    ಆದರೆ (poll(&fds, 1, p_wtm) <= 0)
#ಇಲ್ಲದಿದ್ದರೆ
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    ಆದರೆ (select(self->display.fd + 1, &rfds, ಖಾಲಿ, ಖಾಲಿ, &tv) <= 0)
#ಆದರೆ_ಅಂತ್ಯ
    {
	wl_display_cancel_read(self->display.proxy);
	ಹಿಂತಿರುಗಿ -1;
    }

    // Read events into the queue
    ಆದರೆ (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	ಹಿಂತಿರುಗಿ -1;

    // Dispatch those events (call the handlers associated for each event)
    ಹಿಂತಿರುಗಿ wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_callback_event_ಮುಗಿದ(ಶೂನ್ಯ *ದತ್ತಾಂಶ, ರಚನೆ wl_callback *callback,
	uint32_t callback_ದತ್ತಾಂಶ ಬಳಸದ)
{
    *((bool*)ದತ್ತಾಂಶ) = true;
    wl_callback_destroy(callback);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ wl_callback_listener vwl_callback_listener = {
    .ಮುಗಿದ = vwl_callback_event_ಮುಗಿದ
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    ಪೂರ್ಣಾಂಕ
vwl_connection_roundtrip(vwl_connection_T *self)
{
    ರಚನೆ wl_callback	*callback;
    ಪೂರ್ಣಾಂಕ			ret;
    bool		ಮುಗಿದ = false;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    elapsed_T		ಆರಂಭ_tv;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (self->display.proxy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    ಆದರೆ (callback == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    wl_callback_ಕೂಡಿಸು_listener(callback, &vwl_callback_listener, &ಮುಗಿದ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
    ELAPSED_INIT(ಆರಂಭ_tv);
#ಆದರೆ_ಅಂತ್ಯ

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    ಆಗ (true)
    {
	ret = vwl_connection_dispatch(self);

	ಆದರೆ (ಮುಗಿದ || ret == -1)
	    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ELAPSED_FUNC
	ಆದರೆ (ELAPSED_FUNC(ಆರಂಭ_tv) >= p_wtm)
	{
	    ret = -1;
	    ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (ret == -1)
    {
	ಆದರೆ (!ಮುಗಿದ)
	    wl_callback_destroy(callback);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_log_handler(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ವಾದಗಳು)
{
    // 512 bytes should be big enough
    ಅಕ್ಷರ	*buf	= alloc(512);
    ಅಕ್ಷರ	*ಪೂರ್ವಪ್ರತ್ಯಯ = _("wayland protocol error -> ");
    size_t	len	= STRLEN(ಪೂರ್ವಪ್ರತ್ಯಯ);

    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    vim_strncpy((char_u*)buf, (char_u*)ಪೂರ್ವಪ್ರತ್ಯಯ, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, ವಾದಗಳು);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ch_log(ಖಾಲಿ, "%s", buf);
#ಆದರೆ_ಅಂತ್ಯ
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wl_seat_listener_event_name(
	ಶೂನ್ಯ		*ದತ್ತಾಂಶ,
	ರಚನೆ wl_seat	*seat_proxy ಬಳಸದ,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*name)
{
    vwl_seat_T *seat = ದತ್ತಾಂಶ;

    seat->label = (ಅಕ್ಷರ *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wl_seat_listener_event_capabilities(
	ಶೂನ್ಯ		*ದತ್ತಾಂಶ,
	ರಚನೆ wl_seat	*seat_proxy ಬಳಸದ,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = ದತ್ತಾಂಶ;

    seat->capabilities = capabilities;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ wl_seat_listener wl_seat_listener = {
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
	ಶೂನ್ಯ		    *ದತ್ತಾಂಶ,
	ರಚನೆ wl_registry  *registry,
	uint32_t	    name,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	    *interface,
	uint32_t	    ಆವೃತ್ತಿ_2)
{
    vwl_connection_T *ct = ದತ್ತಾಂಶ;

    ಆದರೆ (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	ರಚನೆ wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, ಆವೃತ್ತಿ_2 > 5 ? 5 : ಆವೃತ್ತಿ_2);
	vwl_seat_T	*seat;

	ಆದರೆ (seat_proxy == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	ಆದರೆ (seat == ಖಾಲಿ || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    ಹಿಂತಿರುಗಿ;
	}

	seat->proxy = seat_proxy;
	wl_seat_ಕೂಡಿಸು_listener(seat_proxy, &wl_seat_listener, seat);

	ಆದರೆ (vwl_connection_roundtrip(ct) == FAIL || seat->label == ಖಾಲಿ)
	{
	    vwl_seat_destroy(seat);
	    ಹಿಂತಿರುಗಿ;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_ದತ್ತಾಂಶ)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(interface, zwlr_ದತ್ತಾಂಶ_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_ದತ್ತಾಂಶ_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_ದತ್ತಾಂಶ_control_manager_v1_interface,
		    ಆವೃತ್ತಿ_2 > 2 ? 2 : ಆವೃತ್ತಿ_2);

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(interface, ext_ದತ್ತಾಂಶ_control_manager_v1_interface.name) == 0)
	ct->gobjects.ext_ದತ್ತಾಂಶ_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &ext_ದತ್ತಾಂಶ_control_manager_v1_interface, 1);
#ಆದರೆ_ಅಂತ್ಯ // FEAT_WAYLAND_CLIPBOARD

}

/*
 * Called when a global object is removed, if so, then do nothing. This is to
 * avoid a global being removed while it is in the process of being used. Let
 * the user call :wlrestore in order to reset everything. Requests to that
 * global will just be ignored on the compositor side.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wl_registry_listener_event_global_remove(
	ಶೂನ್ಯ		    *ದತ್ತಾಂಶ ಬಳಸದ,
	ರಚನೆ wl_registry  *registry ಬಳಸದ,
	uint32_t	    name ಬಳಸದ)
{
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

ಸ್ಥಿರ ಶೂನ್ಯ vwl_connection_destroy(vwl_connection_T *self);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    ಆದರೆ (ct->gobjects.object != ಖಾಲಿ) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = ಖಾಲಿ; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != ಖಾಲಿ)

#ಆದರೆ_ಅಂತ್ಯ

// Make sure to call wayland_set_display(display);
    ಸ್ಥಿರ vwl_connection_T *
vwl_connection_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *display)
{
    vwl_connection_T	*ct;

    ಆದರೆ (wayland_no_connect)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    ಆದರೆ (mch_getenv("XDG_RUNTIME_DIR") == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    ಆದರೆ (ct == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Must set log handler before we connect display in order to work.
    wl_log_ಗಣ_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    ಆದರೆ (ct->display.proxy == ಖಾಲಿ)
    {
	vim_free(ct);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    ಆದರೆ (ct->registry.proxy == ಖಾಲಿ)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ga_init2(&ct->gobjects.seats, ಗಾತ್ರ(vwl_seat_T *), 1);

    wl_registry_ಕೂಡಿಸು_listener(ct->registry.proxy, &wl_registry_listener, ct);

    ಆದರೆ (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ ct;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
vwl_seat_destroy(vwl_seat_T *self)
{
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (self->proxy != ಖಾಲಿ)
    {
	ಆದರೆ (wl_seat_get_ಆವೃತ್ತಿ(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	ಇಲ್ಲದಿದ್ದರೆ
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
    VWL_DESTROY_GOBJECT(self, ext_ದತ್ತಾಂಶ_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_ದತ್ತಾಂಶ_control_manager_v1)
# endif

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_ದತ್ತಾಂಶ)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    ಆದರೆ (self->registry.proxy != ಖಾಲಿ)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = ಖಾಲಿ;
    }
    ಆದರೆ (self->display.proxy != ಖಾಲಿ)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = ಖಾಲಿ;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *label)
{
    ಆದರೆ ((STRCMP(label, "") == 0 || label == ಖಾಲಿ)
	    && self->gobjects.seats.ga_len > 0)
	ಹಿಂತಿರುಗಿ ((vwl_seat_T **)self->gobjects.seats.ga_ದತ್ತಾಂಶ)[0];

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_ದತ್ತಾಂಶ)[i];
	ಆದರೆ (STRCMP(seat->label, label) == 0)
	    ಹಿಂತಿರುಗಿ seat;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wayland_ಗಣ_display(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *display)
{
    ಆದರೆ (display == ಖಾಲಿ)
	display = (ಅಕ್ಷರ*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	ಹೋಗು ನಿರ್ಗಮಿಸು;

    ಆದರೆ (display == ಖಾಲಿ)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    ಆದರೆ (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (ಅಕ್ಷರ*)vim_strsave((char_u*)display);
    }

ನಿರ್ಗಮಿಸು:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ಗಣ_vim_var_string(VV_WAYLAND_DISPLAY, (char_u*)display, -1);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Initializes the global Wayland connection. Connects to the Wayland display
 * with given name and binds to global objects as needed. If display is NULL
 * then the $WAYLAND_DISPLAY environment variable will be used (handled by
 * libwayland). Returns FAIL on failure and OK on
 * success
 */
    ಪೂರ್ಣಾಂಕ
wayland_init_connection(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *display)
{
    wayland_ಗಣ_display(display);

    wayland_ct = vwl_connection_new(display);

    ಆದರೆ (wayland_ct == ಖಾಲಿ)
	ಹೋಗು fail;

    ಹಿಂತಿರುಗಿ OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_ಗಣ_display("");
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    ಶೂನ್ಯ
wayland_uninit_connection(ಶೂನ್ಯ)
{
    ಆದರೆ (wayland_ct == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#ಆದರೆ_ಅಂತ್ಯ
    vwl_connection_destroy(wayland_ct);
    wayland_ct = ಖಾಲಿ;
    is_reading = false;
    wayland_ಗಣ_display("");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ wayland_ct_restore_ಎಣಿಕೆ = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
wayland_restore_connection(ಶೂನ್ಯ)
{
    // No point in restoring the connection if we are exiting or dying.
    ಆದರೆ (exiting || v_dying || wayland_ct_restore_ಎಣಿಕೆ <= 0)
	wayland_ಗಣ_display("");

    --wayland_ct_restore_ಎಣಿಕೆ;
    wayland_uninit_connection();

    ಆದರೆ (wayland_init_connection(wayland_display_name) == OK)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#ಆದರೆ_ಅಂತ್ಯ
    }
}

/*
 * Should be called before polling (select or poll) the global Wayland
 * connection display fd. Returns fd on success and -1 on failure.
 */
    ಪೂರ್ಣಾಂಕ
wayland_prepare_read(ಶೂನ್ಯ)
{
    ಆದರೆ (wayland_ct == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    ಆಗ (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	ಆದರೆ (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    ಹಿಂತಿರುಗಿ -1;

    ಆದರೆ (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	ಹಿಂತಿರುಗಿ -1;
    }

    is_reading = true;

    ಹಿಂತಿರುಗಿ wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    ಪೂರ್ಣಾಂಕ
wayland_update(ಶೂನ್ಯ)
{
    ಆದರೆ (wayland_ct == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ vwl_connection_roundtrip(wayland_ct);
}

#ಆದರೆ !defined(HAVE_SELECT)

    ಶೂನ್ಯ
wayland_poll_check(ಪೂರ್ಣಾಂಕ revents)
{
    ಆದರೆ (wayland_ct == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    is_reading = false;
    ಆದರೆ (revents & POLLIN)
    {
	ಆದರೆ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    ಹಿಂತಿರುಗಿ;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	ಹಿಂತಿರುಗಿ;
    }
    wayland_restore_connection();
}

#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_SELECT)

    ಶೂನ್ಯ
wayland_select_check(bool is_ಗಣ)
{
    ಆದರೆ (wayland_ct == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    is_reading = false;
    ಆದರೆ (is_ಗಣ)
    {
	ಆದರೆ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#ಆದರೆ_ಅಂತ್ಯ

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    ಶೂನ್ಯ
ex_wlrestore(exarg_T *eap)
{
    ಅಕ್ಷರ *display;

    ಆದರೆ (eap->arg == ಖಾಲಿ || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    ಇಲ್ಲದಿದ್ದರೆ
	display = (ಅಕ್ಷರ*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    ಆದರೆ (!eap->forceit && wayland_ct != ಖಾಲಿ &&
	    (display == wayland_display_name ||
	     (wayland_display_name != ಖಾಲಿ &&
	      STRCMP(wayland_display_name, display) == 0)))
	ಹಿಂತಿರುಗಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
    ಆದರೆ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned)
	    clip_lose_selection(&clip_ನಕ್ಷತ್ರ);
	ಆದರೆ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (display != ಖಾಲಿ)
	display = (ಅಕ್ಷರ*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_ಎಣಿಕೆ = 5;

    ಆದರೆ (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಂದೇಶ(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    ಆರಿಸು_clipmethod();
}

#ಆದರೆ defined(FEAT_WAYLAND_CLIPBOARD)

/*
 * Get a suitable data device manager from connection. "supported" should be
 * initialized to VWL_DATA_PROTOCOL_NONE beforehand. Returns NULL if there are
 * no data device manager available with the required selection.
 */
    vwl_ದತ್ತಾಂಶ_device_manager_T *
vwl_connection_get_ದತ್ತಾಂಶ_device_manager(
	vwl_connection_T *self,
	wayland_selection_T req_sel,
	int_u *supported)
{
    vwl_ದತ್ತಾಂಶ_device_manager_T *manager =
	ALLOC_CLEAR_ONE(vwl_ದತ್ತಾಂಶ_device_manager_T);

    // Prioritize ext-data-control-v1 over wlr-data-control-unstable-v1 because
    // it is newer.
    ಆದರೆ (self->gobjects.ext_ದತ್ತಾಂಶ_control_manager_v1 != ಖಾಲಿ)
    {
	manager->proxy = self->gobjects.ext_ದತ್ತಾಂಶ_control_manager_v1;
	manager->protocol = VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (self->gobjects.zwlr_ದತ್ತಾಂಶ_control_manager_v1 != ಖಾಲಿ)
    {
	manager->proxy = self->gobjects.zwlr_ದತ್ತಾಂಶ_control_manager_v1;
	manager->protocol = VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	ಆದರೆ (zwlr_ದತ್ತಾಂಶ_control_manager_v1_get_ಆವೃತ್ತಿ(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    ಆದರೆ (!(*supported & req_sel))
    {
	vim_free(manager);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ manager;
}

    vwl_ದತ್ತಾಂಶ_device_T *
vwl_ದತ್ತಾಂಶ_device_manager_get_ದತ್ತಾಂಶ_device(
	vwl_ದತ್ತಾಂಶ_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_ದತ್ತಾಂಶ_device_T *device = ALLOC_CLEAR_ONE(vwl_ದತ್ತಾಂಶ_device_T);

    ಆಯ್ಕೆ (self->protocol)
    {
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT:
	    device->proxy = ext_ದತ್ತಾಂಶ_control_manager_v1_get_ದತ್ತಾಂಶ_device(
		    self->proxy, seat->proxy);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR:
	    device->proxy = zwlr_ದತ್ತಾಂಶ_control_manager_v1_get_ದತ್ತಾಂಶ_device(
		    self->proxy, seat->proxy);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    vim_free(device);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    device->protocol = self->protocol;

    ಹಿಂತಿರುಗಿ device;
}

    vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *
vwl_ದತ್ತಾಂಶ_device_manager_create_ದತ್ತಾಂಶ_ಮೂಲ(vwl_ದತ್ತಾಂಶ_device_manager_T *self)
{
    vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *ಮೂಲ = ALLOC_CLEAR_ONE(vwl_ದತ್ತಾಂಶ_ಮೂಲ_T);

    ಆಯ್ಕೆ (self->protocol)
    {
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT:
	    ಮೂಲ->proxy = ext_ದತ್ತಾಂಶ_control_manager_v1_create_ದತ್ತಾಂಶ_ಮೂಲ(
		    self->proxy);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR:
	    ಮೂಲ->proxy = zwlr_ದತ್ತಾಂಶ_control_manager_v1_create_ದತ್ತಾಂಶ_ಮೂಲ(
		    self->proxy);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    vim_free(ಮೂಲ);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಮೂಲ->protocol = self->protocol;

    ಹಿಂತಿರುಗಿ ಮೂಲ;
}

    ಸ್ಥಿರ vwl_ದತ್ತಾಂಶ_offer_T *
vwl_ದತ್ತಾಂಶ_device_wrap_offer_proxy(vwl_ದತ್ತಾಂಶ_device_T *self, ಶೂನ್ಯ *proxy)
{
    vwl_ದತ್ತಾಂಶ_offer_T *offer = ALLOC_CLEAR_ONE(vwl_ದತ್ತಾಂಶ_offer_T);

    ಆದರೆ (offer == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->ದತ್ತಾಂಶ = self->ದತ್ತಾಂಶ;
    ga_init2(&offer->mime_types, ಗಾತ್ರ(ಅಕ್ಷರ *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    ಹಿಂತಿರುಗಿ offer;
}

# define VWL_FUNC_ದತ್ತಾಂಶ_PROXY_DESTROY(type) \
	ಶೂನ್ಯ \
    vwl_ದತ್ತಾಂಶ##type##_destroy(vwl_ದತ್ತಾಂಶ##type##_T *self) \
    { \
	ಆದರೆ (self == ಖಾಲಿ) \
	    ಹಿಂತಿರುಗಿ; \
	ಆಯ್ಕೆ (self->protocol) \
	{ \
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT: \
		ext_ದತ್ತಾಂಶ_control##type##_v1_destroy(self->proxy); \
		ನಿಲ್ಲಿಸು; \
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR: \
		zwlr_ದತ್ತಾಂಶ_control##type##_v1_destroy(self->proxy); \
		ನಿಲ್ಲಿಸು; \
	    ಪೂರ್ವನಿಯೋಜಿತ: \
		ನಿಲ್ಲಿಸು; \
	} \
	vim_free(self); \
    }

VWL_FUNC_ದತ್ತಾಂಶ_PROXY_DESTROY(device)
VWL_FUNC_ದತ್ತಾಂಶ_PROXY_DESTROY(ಮೂಲ)

    ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_offer_destroy(vwl_ದತ್ತಾಂಶ_offer_T *self)
{
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆಯ್ಕೆ (self->protocol)
    {
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT:
	    ext_ದತ್ತಾಂಶ_control_offer_v1_destroy(self->proxy);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR:
	    zwlr_ದತ್ತಾಂಶ_control_offer_v1_destroy(self->proxy);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_device_manager_discard(vwl_ದತ್ತಾಂಶ_device_manager_T *self)
{
    ಆದರೆ (self == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    vim_free(self);
}

# define VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_ದತ್ತಾಂಶ_OFFER(device_type, offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_data_offer( \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ, \
	    ರಚನೆ device_type *device ಬಳಸದ, \
	    ರಚನೆ offer_type *offer) \
    { \
	vwl_ದತ್ತಾಂಶ_device_T *self = ದತ್ತಾಂಶ; \
	self->offer = vwl_ದತ್ತಾಂಶ_device_wrap_offer_proxy(self, offer); \
	self->listener->ದತ್ತಾಂಶ_offer(self->ದತ್ತಾಂಶ, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_selection( \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ, \
	    ರಚನೆ device_type *device ಬಳಸದ, \
	    ರಚನೆ offer_type *offer ಬಳಸದ) \
    { \
	vwl_ದತ್ತಾಂಶ_device_T *self = ದತ್ತಾಂಶ; \
	self->listener->selection(self->ದತ್ತಾಂಶ, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = ಖಾಲಿ; \
    } \

# define VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_primary_selection( \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ, \
	    ರಚನೆ device_type *device ಬಳಸದ, \
	    ರಚನೆ offer_type *offer ಬಳಸದ) \
    { \
	vwl_ದತ್ತಾಂಶ_device_T *self = ದತ್ತಾಂಶ; \
	self->listener->selection(self->ದತ್ತಾಂಶ, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = ಖಾಲಿ; \
    }

# define VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_ಪೂರ್ಣಗೊಂಡ(device_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    device_type##_listener_event_finished( \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ, \
	    ರಚನೆ device_type *device ಬಳಸದ) \
    { \
	vwl_ದತ್ತಾಂಶ_device_T *self = ದತ್ತಾಂಶ; \
	self->listener->ಪೂರ್ಣಗೊಂಡ(self->ದತ್ತಾಂಶ, self); \
    }

VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_ದತ್ತಾಂಶ_OFFER(
    ext_ದತ್ತಾಂಶ_control_device_v1, ext_ದತ್ತಾಂಶ_control_offer_v1)
VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_ದತ್ತಾಂಶ_OFFER(
    zwlr_ದತ್ತಾಂಶ_control_device_v1, zwlr_ದತ್ತಾಂಶ_control_offer_v1)

VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_SELECTION(
    ext_ದತ್ತಾಂಶ_control_device_v1, ext_ದತ್ತಾಂಶ_control_offer_v1
)
VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_SELECTION(
    zwlr_ದತ್ತಾಂಶ_control_device_v1, zwlr_ದತ್ತಾಂಶ_control_offer_v1
)
VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_PRIMARY_SELECTION(
    ext_ದತ್ತಾಂಶ_control_device_v1, ext_ದತ್ತಾಂಶ_control_offer_v1
)
VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_PRIMARY_SELECTION(
    zwlr_ದತ್ತಾಂಶ_control_device_v1, zwlr_ದತ್ತಾಂಶ_control_offer_v1
)

VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_ಪೂರ್ಣಗೊಂಡ(ext_ದತ್ತಾಂಶ_control_device_v1)
VWL_FUNC_ದತ್ತಾಂಶ_DEVICE_EVENT_ಪೂರ್ಣಗೊಂಡ(zwlr_ದತ್ತಾಂಶ_control_device_v1)

ಸ್ಥಿರ ರಚನೆ ext_ದತ್ತಾಂಶ_control_device_v1_listener
    ext_ದತ್ತಾಂಶ_control_device_v1_listener = {
	.ದತ್ತಾಂಶ_offer = ext_ದತ್ತಾಂಶ_control_device_v1_listener_event_ದತ್ತಾಂಶ_offer,
	.selection = ext_ದತ್ತಾಂಶ_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_ದತ್ತಾಂಶ_control_device_v1_listener_event_primary_selection,
	.ಪೂರ್ಣಗೊಂಡ = ext_ದತ್ತಾಂಶ_control_device_v1_listener_event_ಪೂರ್ಣಗೊಂಡ
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ zwlr_ದತ್ತಾಂಶ_control_device_v1_listener
    zwlr_ದತ್ತಾಂಶ_control_device_v1_listener = {
	.ದತ್ತಾಂಶ_offer = zwlr_ದತ್ತಾಂಶ_control_device_v1_listener_event_ದತ್ತಾಂಶ_offer,
	.selection = zwlr_ದತ್ತಾಂಶ_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_ದತ್ತಾಂಶ_control_device_v1_listener_event_primary_selection,
	.ಪೂರ್ಣಗೊಂಡ = zwlr_ದತ್ತಾಂಶ_control_device_v1_listener_event_ಪೂರ್ಣಗೊಂಡ
};

# define VWL_FUNC_ದತ್ತಾಂಶ_ಮೂಲ_EVENT_SEND(ಮೂಲ_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    ಮೂಲ_type##_listener_event_send( \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ, ರಚನೆ ಮೂಲ_type *ಮೂಲ ಬಳಸದ, \
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type, ಪೂರ್ಣಾಂಕ fd) \
    { \
	vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *self = ದತ್ತಾಂಶ; \
	self->listener->send(self->ದತ್ತಾಂಶ, self, mime_type, fd); \
    }

# define VWL_FUNC_ದತ್ತಾಂಶ_ಮೂಲ_EVENT_CANCELLED(ಮೂಲ_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    ಮೂಲ_type##_listener_event_cancelled( \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ, ರಚನೆ ಮೂಲ_type *ಮೂಲ ಬಳಸದ) \
    { \
	vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *self = ದತ್ತಾಂಶ; \
	self->listener->cancelled(self->ದತ್ತಾಂಶ, self); \
    }

VWL_FUNC_ದತ್ತಾಂಶ_ಮೂಲ_EVENT_SEND(ext_ದತ್ತಾಂಶ_control_ಮೂಲ_v1)
VWL_FUNC_ದತ್ತಾಂಶ_ಮೂಲ_EVENT_SEND(zwlr_ದತ್ತಾಂಶ_control_ಮೂಲ_v1)

VWL_FUNC_ದತ್ತಾಂಶ_ಮೂಲ_EVENT_CANCELLED(ext_ದತ್ತಾಂಶ_control_ಮೂಲ_v1)
VWL_FUNC_ದತ್ತಾಂಶ_ಮೂಲ_EVENT_CANCELLED(zwlr_ದತ್ತಾಂಶ_control_ಮೂಲ_v1)

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ ext_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener
    ext_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener = {
	.send = ext_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener_event_send,
	.cancelled = ext_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener_event_cancelled
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ zwlr_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener
    zwlr_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener = {
	.send = zwlr_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener_event_send,
	.cancelled = zwlr_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_listener_event_cancelled
};

# define VWL_FUNC_ದತ್ತಾಂಶ_OFFER_EVENT_OFFER(offer_type) \
	ಸ್ಥಿರ ಶೂನ್ಯ \
    offer_type##_listener_event_offer( \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ, \
	    ರಚನೆ offer_type *offer ಬಳಸದ, \
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type) \
    { \
	vwl_ದತ್ತಾಂಶ_offer_T *self = ದತ್ತಾಂಶ; \
	ಆದರೆ (self->listener->offer(self->ದತ್ತಾಂಶ, self, mime_type)) \
	{ \
	    ಅಕ್ಷರ *mime = (ಅಕ್ಷರ *)vim_strsave((char_u *)mime_type); \
	    ಆದರೆ (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    ಇಲ್ಲದಿದ್ದರೆ \
		ಆದರೆ (mime != ಖಾಲಿ) \
		    ((ಅಕ್ಷರ **)self->mime_types.ga_ದತ್ತಾಂಶ) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_ದತ್ತಾಂಶ_OFFER_EVENT_OFFER(ext_ದತ್ತಾಂಶ_control_offer_v1)
VWL_FUNC_ದತ್ತಾಂಶ_OFFER_EVENT_OFFER(zwlr_ದತ್ತಾಂಶ_control_offer_v1)

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ ext_ದತ್ತಾಂಶ_control_offer_v1_listener
    ext_ದತ್ತಾಂಶ_control_offer_v1_listener = {
	.offer = ext_ದತ್ತಾಂಶ_control_offer_v1_listener_event_offer
};
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ zwlr_ದತ್ತಾಂಶ_control_offer_v1_listener
    zwlr_ದತ್ತಾಂಶ_control_offer_v1_listener = {
	.offer = zwlr_ದತ್ತಾಂಶ_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_ದತ್ತಾಂಶ_PROXY_ಕೂಡಿಸು_LISTENER(type) \
	ಶೂನ್ಯ \
    vwl_ದತ್ತಾಂಶ##type##_add_listener( \
	    vwl_ದತ್ತಾಂಶ##type##_T *self, \
	    ಸ್ಥಿರಾಂಕ vwl_ದತ್ತಾಂಶ##type##_listener_T *listener, \
	    ಶೂನ್ಯ *ದತ್ತಾಂಶ) \
    { \
	ಆದರೆ (self == ಖಾಲಿ) \
	    ಹಿಂತಿರುಗಿ; \
	self->ದತ್ತಾಂಶ = ದತ್ತಾಂಶ; \
	self->listener = listener; \
	ಆಯ್ಕೆ (self->protocol) \
	{ \
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT: \
		ext_ದತ್ತಾಂಶ_control##type##_v1_add_listener(self->proxy, \
			&ext_ದತ್ತಾಂಶ_control##type##_v1_listener, self); \
		ನಿಲ್ಲಿಸು; \
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR: \
		zwlr_ದತ್ತಾಂಶ_control##type##_v1_add_listener(self->proxy, \
			&zwlr_ದತ್ತಾಂಶ_control##type##_v1_listener, self); \
		ನಿಲ್ಲಿಸು; \
	    ಪೂರ್ವನಿಯೋಜಿತ: \
		ನಿಲ್ಲಿಸು; \
	} \
    }

VWL_FUNC_ದತ್ತಾಂಶ_PROXY_ಕೂಡಿಸು_LISTENER(device)
VWL_FUNC_ದತ್ತಾಂಶ_PROXY_ಕೂಡಿಸು_LISTENER(ಮೂಲ)
VWL_FUNC_ದತ್ತಾಂಶ_PROXY_ಕೂಡಿಸು_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_device_ಗಣ_selection(
    vwl_ದತ್ತಾಂಶ_device_T *self,
    vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *ಮೂಲ,
    uint32_t serial ಬಳಸದ,
    wayland_selection_T selection
)
{
    ಶೂನ್ಯ *proxy = ಮೂಲ == ಖಾಲಿ ? ಖಾಲಿ : ಮೂಲ->proxy;

    ಆದರೆ (selection == WAYLAND_SELECTION_REGULAR)
    {
	ಆಯ್ಕೆ (self->protocol)
	{
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT:
		ext_ದತ್ತಾಂಶ_control_device_v1_ಗಣ_selection(self->proxy, proxy);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR:
		zwlr_ದತ್ತಾಂಶ_control_device_v1_ಗಣ_selection(self->proxy, proxy);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (selection == WAYLAND_SELECTION_PRIMARY)
    {
	ಆಯ್ಕೆ (self->protocol)
	{
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT:
		ext_ದತ್ತಾಂಶ_control_device_v1_ಗಣ_primary_selection(
			self->proxy, proxy
			);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR:
		zwlr_ದತ್ತಾಂಶ_control_device_v1_ಗಣ_primary_selection(
			self->proxy, proxy
			);
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		ನಿಲ್ಲಿಸು;
	}
    }
}

    ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_ಮೂಲ_offer(vwl_ದತ್ತಾಂಶ_ಮೂಲ_T *self, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type)
{
    ಆಯ್ಕೆ (self->protocol)
    {
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT:
	    ext_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_offer(self->proxy, mime_type);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR:
	    zwlr_ದತ್ತಾಂಶ_control_ಮೂಲ_v1_offer(self->proxy, mime_type);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
}

    ಶೂನ್ಯ
vwl_ದತ್ತಾಂಶ_offer_receive(
	vwl_ದತ್ತಾಂಶ_offer_T *self,
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *mime_type,
	int32_t fd)
{
    ಆಯ್ಕೆ (self->protocol)
    {
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_EXT:
	    ext_ದತ್ತಾಂಶ_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VWL_ದತ್ತಾಂಶ_PROTOCOL_WLR:
	    zwlr_ದತ್ತಾಂಶ_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_WAYLAND_CLIPBOARD

#ಆದರೆ_ಅಂತ್ಯ // FEAT_WAYLAND
