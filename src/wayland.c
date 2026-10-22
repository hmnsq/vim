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

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_WAYLAND

#അന്തര്ഭാവ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    പൂർണ്ണസംഖ്യ
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ഘടന pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#അല്ലെങ്കിൽ
    fd_set	    wfds;
    ഘടന timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    എങ്കിൽ (self->display.proxy == ശൂന്യ)
	തിരികെ FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    ആകുമ്പോൾ (true)
    {
	പൂർണ്ണസംഖ്യ ret = wl_display_flush(self->display.proxy);

	എങ്കിൽ (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    എങ്കിൽ (poll(&fds, 1, p_wtm) <= 0)
#അല്ലെങ്കിൽ
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    എങ്കിൽ (select(self->display.fd + 1, ശൂന്യ, &wfds, ശൂന്യ, &tv) <= 0)
		തിരികെ FAIL;
#endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ret == -1)
	    തിരികെ FAIL;
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    തിരികെ OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    പൂർണ്ണസംഖ്യ
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ഘടന pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#അല്ലെങ്കിൽ
    fd_set	    rfds;
    ഘടന timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    എങ്കിൽ (self->display.proxy == ശൂന്യ)
	തിരികെ -1;

    ആകുമ്പോൾ (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	എങ്കിൽ (wl_display_dispatch_pending(self->display.proxy) == -1)
	    തിരികെ -1;

    // Send any requests before we starting blocking to read display fd
    എങ്കിൽ (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	തിരികെ -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    എങ്കിൽ (poll(&fds, 1, p_wtm) <= 0)
#അല്ലെങ്കിൽ
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    എങ്കിൽ (select(self->display.fd + 1, &rfds, ശൂന്യ, ശൂന്യ, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	തിരികെ -1;
    }

    // Read events into the queue
    എങ്കിൽ (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	തിരികെ -1;

    // Dispatch those events (call the handlers associated for each event)
    തിരികെ wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    സ്ഥിതം ശൂന്യം
vwl_callback_event_done(ശൂന്യം *data, ഘടന wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

സ്ഥിതം സ്ഥിരം ഘടന wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    പൂർണ്ണസംഖ്യ
vwl_connection_roundtrip(vwl_connection_T *self)
{
    ഘടന wl_callback	*callback;
    പൂർണ്ണസംഖ്യ			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    എങ്കിൽ (self->display.proxy == ശൂന്യ)
	തിരികെ FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    എങ്കിൽ (callback == ശൂന്യ)
	തിരികെ FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    ആകുമ്പോൾ (true)
    {
	ret = vwl_connection_dispatch(self);

	എങ്കിൽ (done || ret == -1)
	    നിർത്തുക;

#ifdef ELAPSED_FUNC
	എങ്കിൽ (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    നിർത്തുക;
	}
#endif
    }

    എങ്കിൽ (ret == -1)
    {
	എങ്കിൽ (!done)
	    wl_callback_destroy(callback);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    സ്ഥിതം ശൂന്യം
vwl_log_handler(സ്ഥിരം അക്ഷരം *fmt, va_list args)
{
    // 512 bytes should be big enough
    അക്ഷരം	*buf	= alloc(512);
    അക്ഷരം	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(ശൂന്യ, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    സ്ഥിതം ശൂന്യം
wl_seat_listener_event_name(
	ശൂന്യം		*data,
	ഘടന wl_seat	*seat_proxy UNUSED,
	സ്ഥിരം അക്ഷരം	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (അക്ഷരം *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    സ്ഥിതം ശൂന്യം
wl_seat_listener_event_capabilities(
	ശൂന്യം		*data,
	ഘടന wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

സ്ഥിതം സ്ഥിരം ഘടന wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

സ്ഥിതം ശൂന്യം vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    സ്ഥിതം ശൂന്യം
wl_registry_listener_event_global(
	ശൂന്യം		    *data,
	ഘടന wl_registry  *registry,
	uint32_t	    name,
	സ്ഥിരം അക്ഷരം	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    എങ്കിൽ (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	ഘടന wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	എങ്കിൽ (seat_proxy == ശൂന്യ)
	    തിരികെ;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	എങ്കിൽ (seat == ശൂന്യ || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    തിരികെ;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	എങ്കിൽ (vwl_connection_roundtrip(ct) == FAIL || seat->label == ശൂന്യ)
	{
	    vwl_seat_destroy(seat);
	    തിരികെ;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
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
    സ്ഥിതം ശൂന്യം
wl_registry_listener_event_global_remove(
	ശൂന്യം		    *data UNUSED,
	ഘടന wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

സ്ഥിതം സ്ഥിരം ഘടന wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

സ്ഥിതം ശൂന്യം vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    എങ്കിൽ (ct->gobjects.object != ശൂന്യ) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = ശൂന്യ; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != ശൂന്യ)

#endif

// Make sure to call wayland_set_display(display);
    സ്ഥിതം vwl_connection_T *
vwl_connection_new(സ്ഥിരം അക്ഷരം *display)
{
    vwl_connection_T	*ct;

    എങ്കിൽ (wayland_no_connect)
	തിരികെ ശൂന്യ;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    എങ്കിൽ (mch_getenv("XDG_RUNTIME_DIR") == ശൂന്യ)
	തിരികെ ശൂന്യ;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    എങ്കിൽ (ct == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    എങ്കിൽ (ct->display.proxy == ശൂന്യ)
    {
	vim_free(ct);
	തിരികെ ശൂന്യ;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    എങ്കിൽ (ct->registry.proxy == ശൂന്യ)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	തിരികെ ശൂന്യ;
    }

    ga_init2(&ct->gobjects.seats, വലുപ്പം(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    എങ്കിൽ (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	തിരികെ ശൂന്യ;
    }

    തിരികെ ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    സ്ഥിതം ശൂന്യം
vwl_seat_destroy(vwl_seat_T *self)
{
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (self->proxy != ശൂന്യ)
    {
	എങ്കിൽ (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	അല്ലെങ്കിൽ
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    സ്ഥിതം ശൂന്യം
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    എങ്കിൽ (self->registry.proxy != ശൂന്യ)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = ശൂന്യ;
    }
    എങ്കിൽ (self->display.proxy != ശൂന്യ)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = ശൂന്യ;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, സ്ഥിരം അക്ഷരം *label)
{
    എങ്കിൽ ((STRCMP(label, "") == 0 || label == ശൂന്യ)
	    && self->gobjects.seats.ga_len > 0)
	തിരികെ ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	എങ്കിൽ (STRCMP(seat->label, label) == 0)
	    തിരികെ seat;
    }
    തിരികെ ശൂന്യ;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    സ്ഥിതം ശൂന്യം
wayland_set_display(സ്ഥിരം അക്ഷരം *display)
{
    എങ്കിൽ (display == ശൂന്യ)
	display = (അക്ഷരം*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    അല്ലെങ്കിൽ എങ്കിൽ (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	പോകുക exit;

    എങ്കിൽ (display == ശൂന്യ)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    എങ്കിൽ (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (അക്ഷരം*)vim_strsave((char_u*)display);
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
    പൂർണ്ണസംഖ്യ
wayland_init_connection(സ്ഥിരം അക്ഷരം *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    എങ്കിൽ (wayland_ct == ശൂന്യ)
	പോകുക fail;

    തിരികെ OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    തിരികെ FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    ശൂന്യം
wayland_uninit_connection(ശൂന്യം)
{
    എങ്കിൽ (wayland_ct == ശൂന്യ)
	തിരികെ;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = ശൂന്യ;
    is_reading = false;
    wayland_set_display("");
}

സ്ഥിതം പൂർണ്ണസംഖ്യ wayland_ct_restore_എണ്ണം = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    സ്ഥിതം ശൂന്യം
wayland_restore_connection(ശൂന്യം)
{
    // No point in restoring the connection if we are exiting or dying.
    എങ്കിൽ (exiting || v_dying || wayland_ct_restore_എണ്ണം <= 0)
	wayland_set_display("");

    --wayland_ct_restore_എണ്ണം;
    wayland_uninit_connection();

    എങ്കിൽ (wayland_init_connection(wayland_display_name) == OK)
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
    പൂർണ്ണസംഖ്യ
wayland_prepare_read(ശൂന്യം)
{
    എങ്കിൽ (wayland_ct == ശൂന്യ)
	തിരികെ -1;

    എങ്കിൽ (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    ആകുമ്പോൾ (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	എങ്കിൽ (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    തിരികെ -1;

    എങ്കിൽ (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	തിരികെ -1;
    }

    is_reading = true;

    തിരികെ wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    പൂർണ്ണസംഖ്യ
wayland_update(ശൂന്യം)
{
    എങ്കിൽ (wayland_ct == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ vwl_connection_roundtrip(wayland_ct);
}

#എങ്കിൽ !defined(HAVE_SELECT)

    ശൂന്യം
wayland_poll_check(പൂർണ്ണസംഖ്യ revents)
{
    എങ്കിൽ (wayland_ct == ശൂന്യ)
	തിരികെ;

    is_reading = false;
    എങ്കിൽ (revents & POLLIN)
    {
	എങ്കിൽ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    തിരികെ;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    അല്ലെങ്കിൽ
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	തിരികെ;
    }
    wayland_restore_connection();
}

#endif
#എങ്കിൽ defined(HAVE_SELECT)

    ശൂന്യം
wayland_select_check(bool is_set)
{
    എങ്കിൽ (wayland_ct == ശൂന്യ)
	തിരികെ;

    is_reading = false;
    എങ്കിൽ (is_set)
    {
	എങ്കിൽ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	അല്ലെങ്കിൽ
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    അല്ലെങ്കിൽ
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    ശൂന്യം
ex_wlrestore(exarg_T *eap)
{
    അക്ഷരം *display;

    എങ്കിൽ (eap->arg == ശൂന്യ || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    അല്ലെങ്കിൽ
	display = (അക്ഷരം*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    എങ്കിൽ (!eap->forceit && wayland_ct != ശൂന്യ &&
	    (display == wayland_display_name ||
	     (wayland_display_name != ശൂന്യ &&
	      STRCMP(wayland_display_name, display) == 0)))
	തിരികെ;

#ifdef FEAT_WAYLAND_CLIPBOARD
    എങ്കിൽ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	എങ്കിൽ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	എങ്കിൽ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    എങ്കിൽ (display != ശൂന്യ)
	display = (അക്ഷരം*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_എണ്ണം = 5;

    എങ്കിൽ (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    അല്ലെങ്കിൽ
	സന്ദേശം(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#എങ്കിൽ defined(FEAT_WAYLAND_CLIPBOARD)

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
    എങ്കിൽ (self->gobjects.ext_data_control_manager_v1 != ശൂന്യ)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (self->gobjects.zwlr_data_control_manager_v1 != ശൂന്യ)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	എങ്കിൽ (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    എങ്കിൽ (!(*supported & req_sel))
    {
	vim_free(manager);
	തിരികെ ശൂന്യ;
    }

    തിരികെ manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    തിരഞ്ഞെടുപ്പ് (self->protocol)
    {
	അവസ്ഥ VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    നിർത്തുക;
	അവസ്ഥ VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    vim_free(device);
	    തിരികെ ശൂന്യ;
    }
    device->protocol = self->protocol;

    തിരികെ device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    തിരഞ്ഞെടുപ്പ് (self->protocol)
    {
	അവസ്ഥ VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    നിർത്തുക;
	അവസ്ഥ VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    vim_free(source);
	    തിരികെ ശൂന്യ;
    }
    source->protocol = self->protocol;

    തിരികെ source;
}

    സ്ഥിതം vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, ശൂന്യം *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    എങ്കിൽ (offer == ശൂന്യ)
	തിരികെ ശൂന്യ;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, വലുപ്പം(അക്ഷരം *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    തിരികെ offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	ശൂന്യം \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	എങ്കിൽ (self == ശൂന്യ) \
	    തിരികെ; \
	തിരഞ്ഞെടുപ്പ് (self->protocol) \
	{ \
	    അവസ്ഥ VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		നിർത്തുക; \
	    അവസ്ഥ VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		നിർത്തുക; \
	    സ്ഥിരസ്ഥിതി: \
		നിർത്തുക; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    ശൂന്യം
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ;
    തിരഞ്ഞെടുപ്പ് (self->protocol)
    {
	അവസ്ഥ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    നിർത്തുക;
	അവസ്ഥ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
ശൂന്യം
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    എങ്കിൽ (self == ശൂന്യ)
	തിരികെ;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	സ്ഥിതം ശൂന്യം \
    device_type##_listener_event_data_offer( \
	    ശൂന്യം *data, \
	    ഘടന device_type *device UNUSED, \
	    ഘടന offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	സ്ഥിതം ശൂന്യം \
    device_type##_listener_event_selection( \
	    ശൂന്യം *data, \
	    ഘടന device_type *device UNUSED, \
	    ഘടന offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = ശൂന്യ; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	സ്ഥിതം ശൂന്യം \
    device_type##_listener_event_primary_selection( \
	    ശൂന്യം *data, \
	    ഘടന device_type *device UNUSED, \
	    ഘടന offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = ശൂന്യ; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	സ്ഥിതം ശൂന്യം \
    device_type##_listener_event_finished( \
	    ശൂന്യം *data, \
	    ഘടന device_type *device UNUSED) \
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

സ്ഥിതം ഘടന ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
സ്ഥിതം സ്ഥിരം ഘടന zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	സ്ഥിതം ശൂന്യം \
    source_type##_listener_event_send( \
	    ശൂന്യം *data, ഘടന source_type *source UNUSED, \
	    സ്ഥിരം അക്ഷരം *mime_type, പൂർണ്ണസംഖ്യ fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	സ്ഥിതം ശൂന്യം \
    source_type##_listener_event_cancelled( \
	    ശൂന്യം *data, ഘടന source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

സ്ഥിതം സ്ഥിരം ഘടന ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
സ്ഥിതം സ്ഥിരം ഘടന zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	സ്ഥിതം ശൂന്യം \
    offer_type##_listener_event_offer( \
	    ശൂന്യം *data, \
	    ഘടന offer_type *offer UNUSED, \
	    സ്ഥിരം അക്ഷരം *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	എങ്കിൽ (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    അക്ഷരം *mime = (അക്ഷരം *)vim_strsave((char_u *)mime_type); \
	    എങ്കിൽ (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    അല്ലെങ്കിൽ \
		എങ്കിൽ (mime != ശൂന്യ) \
		    ((അക്ഷരം **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

സ്ഥിതം സ്ഥിരം ഘടന ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
സ്ഥിതം സ്ഥിരം ഘടന zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	ശൂന്യം \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    സ്ഥിരം vwl_data_##type##_listener_T *listener, \
	    ശൂന്യം *data) \
    { \
	എങ്കിൽ (self == ശൂന്യ) \
	    തിരികെ; \
	self->data = data; \
	self->listener = listener; \
	തിരഞ്ഞെടുപ്പ് (self->protocol) \
	{ \
	    അവസ്ഥ VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		നിർത്തുക; \
	    അവസ്ഥ VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		നിർത്തുക; \
	    സ്ഥിരസ്ഥിതി: \
		നിർത്തുക; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    ശൂന്യം
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    ശൂന്യം *proxy = source == ശൂന്യ ? ശൂന്യ : source->proxy;

    എങ്കിൽ (selection == WAYLAND_SELECTION_REGULAR)
    {
	തിരഞ്ഞെടുപ്പ് (self->protocol)
	{
	    അവസ്ഥ VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		നിർത്തുക;
	    അവസ്ഥ VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (selection == WAYLAND_SELECTION_PRIMARY)
    {
	തിരഞ്ഞെടുപ്പ് (self->protocol)
	{
	    അവസ്ഥ VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		നിർത്തുക;
	    അവസ്ഥ VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		നിർത്തുക;
	}
    }
}

    ശൂന്യം
vwl_data_source_offer(vwl_data_source_T *self, സ്ഥിരം അക്ഷരം *mime_type)
{
    തിരഞ്ഞെടുപ്പ് (self->protocol)
    {
	അവസ്ഥ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    നിർത്തുക;
	അവസ്ഥ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
}

    ശൂന്യം
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	സ്ഥിരം അക്ഷരം *mime_type,
	int32_t fd)
{
    തിരഞ്ഞെടുപ്പ് (self->protocol)
    {
	അവസ്ഥ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    നിർത്തുക;
	അവസ്ഥ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
