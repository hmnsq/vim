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

#அந்தர்பாவ "vim.h"

#ifdef FEAT_WAYLAND

#அந்தர்பாவ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    முழு
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    கட்டமைப்பு pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#இல்லையெனில்
    fd_set	    wfds;
    கட்டமைப்பு timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    எனில் (self->display.proxy == வெற்று)
	திரும்பு FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    போது (true)
    {
	முழு ret = wl_display_flush(self->display.proxy);

	எனில் (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    எனில் (poll(&fds, 1, p_wtm) <= 0)
#இல்லையெனில்
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    எனில் (select(self->display.fd + 1, வெற்று, &wfds, வெற்று, &tv) <= 0)
		திரும்பு FAIL;
#endif
	}
	இல்லையெனில் எனில் (ret == -1)
	    திரும்பு FAIL;
	இல்லையெனில்
	    நிறுத்து;
    }

    திரும்பு OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    முழு
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    கட்டமைப்பு pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#இல்லையெனில்
    fd_set	    rfds;
    கட்டமைப்பு timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    எனில் (self->display.proxy == வெற்று)
	திரும்பு -1;

    போது (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	எனில் (wl_display_dispatch_pending(self->display.proxy) == -1)
	    திரும்பு -1;

    // Send any requests before we starting blocking to read display fd
    எனில் (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	திரும்பு -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    எனில் (poll(&fds, 1, p_wtm) <= 0)
#இல்லையெனில்
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    எனில் (select(self->display.fd + 1, &rfds, வெற்று, வெற்று, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	திரும்பு -1;
    }

    // Read events into the queue
    எனில் (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	திரும்பு -1;

    // Dispatch those events (call the handlers associated for each event)
    திரும்பு wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    நிலையான வெறுமை
vwl_callback_event_done(வெறுமை *data, கட்டமைப்பு wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

நிலையான மாறிலி கட்டமைப்பு wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    முழு
vwl_connection_roundtrip(vwl_connection_T *self)
{
    கட்டமைப்பு wl_callback	*callback;
    முழு			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    எனில் (self->display.proxy == வெற்று)
	திரும்பு FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    எனில் (callback == வெற்று)
	திரும்பு FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    போது (true)
    {
	ret = vwl_connection_dispatch(self);

	எனில் (done || ret == -1)
	    நிறுத்து;

#ifdef ELAPSED_FUNC
	எனில் (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    நிறுத்து;
	}
#endif
    }

    எனில் (ret == -1)
    {
	எனில் (!done)
	    wl_callback_destroy(callback);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    நிலையான வெறுமை
vwl_log_handler(மாறிலி எழுத்து *fmt, va_list args)
{
    // 512 bytes should be big enough
    எழுத்து	*buf	= alloc(512);
    எழுத்து	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    எனில் (buf == வெற்று)
	திரும்பு;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(வெற்று, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    நிலையான வெறுமை
wl_seat_listener_event_name(
	வெறுமை		*data,
	கட்டமைப்பு wl_seat	*seat_proxy UNUSED,
	மாறிலி எழுத்து	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (எழுத்து *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    நிலையான வெறுமை
wl_seat_listener_event_capabilities(
	வெறுமை		*data,
	கட்டமைப்பு wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

நிலையான மாறிலி கட்டமைப்பு wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

நிலையான வெறுமை vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    நிலையான வெறுமை
wl_registry_listener_event_global(
	வெறுமை		    *data,
	கட்டமைப்பு wl_registry  *registry,
	uint32_t	    name,
	மாறிலி எழுத்து	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    எனில் (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	கட்டமைப்பு wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	எனில் (seat_proxy == வெற்று)
	    திரும்பு;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	எனில் (seat == வெற்று || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    திரும்பு;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	எனில் (vwl_connection_roundtrip(ct) == FAIL || seat->label == வெற்று)
	{
	    vwl_seat_destroy(seat);
	    திரும்பு;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    இல்லையெனில் எனில் (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    இல்லையெனில் எனில் (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
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
    நிலையான வெறுமை
wl_registry_listener_event_global_remove(
	வெறுமை		    *data UNUSED,
	கட்டமைப்பு wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

நிலையான மாறிலி கட்டமைப்பு wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

நிலையான வெறுமை vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    எனில் (ct->gobjects.object != வெற்று) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = வெற்று; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != வெற்று)

#endif

// Make sure to call wayland_set_display(display);
    நிலையான vwl_connection_T *
vwl_connection_new(மாறிலி எழுத்து *display)
{
    vwl_connection_T	*ct;

    எனில் (wayland_no_connect)
	திரும்பு வெற்று;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    எனில் (mch_getenv("XDG_RUNTIME_DIR") == வெற்று)
	திரும்பு வெற்று;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    எனில் (ct == வெற்று)
	திரும்பு வெற்று;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    எனில் (ct->display.proxy == வெற்று)
    {
	vim_free(ct);
	திரும்பு வெற்று;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    எனில் (ct->registry.proxy == வெற்று)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	திரும்பு வெற்று;
    }

    ga_init2(&ct->gobjects.seats, அளவு(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    எனில் (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	திரும்பு வெற்று;
    }

    திரும்பு ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    நிலையான வெறுமை
vwl_seat_destroy(vwl_seat_T *self)
{
    எனில் (self == வெற்று)
	திரும்பு;
    எனில் (self->proxy != வெற்று)
    {
	எனில் (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	இல்லையெனில்
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    நிலையான வெறுமை
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    க்கு (முழு i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    எனில் (self->registry.proxy != வெற்று)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = வெற்று;
    }
    எனில் (self->display.proxy != வெற்று)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = வெற்று;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, மாறிலி எழுத்து *label)
{
    எனில் ((STRCMP(label, "") == 0 || label == வெற்று)
	    && self->gobjects.seats.ga_len > 0)
	திரும்பு ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    க்கு (முழு i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	எனில் (STRCMP(seat->label, label) == 0)
	    திரும்பு seat;
    }
    திரும்பு வெற்று;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    நிலையான வெறுமை
wayland_set_display(மாறிலி எழுத்து *display)
{
    எனில் (display == வெற்று)
	display = (எழுத்து*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    இல்லையெனில் எனில் (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	செல் exit;

    எனில் (display == வெற்று)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    எனில் (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (எழுத்து*)vim_strsave((char_u*)display);
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
    முழு
wayland_init_connection(மாறிலி எழுத்து *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    எனில் (wayland_ct == வெற்று)
	செல் fail;

    திரும்பு OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    திரும்பு FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    வெறுமை
wayland_uninit_connection(வெறுமை)
{
    எனில் (wayland_ct == வெற்று)
	திரும்பு;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = வெற்று;
    is_reading = false;
    wayland_set_display("");
}

நிலையான முழு wayland_ct_restore_மொத்தம் = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    நிலையான வெறுமை
wayland_restore_connection(வெறுமை)
{
    // No point in restoring the connection if we are exiting or dying.
    எனில் (exiting || v_dying || wayland_ct_restore_மொத்தம் <= 0)
	wayland_set_display("");

    --wayland_ct_restore_மொத்தம்;
    wayland_uninit_connection();

    எனில் (wayland_init_connection(wayland_display_name) == OK)
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
    முழு
wayland_prepare_read(வெறுமை)
{
    எனில் (wayland_ct == வெற்று)
	திரும்பு -1;

    எனில் (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    போது (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	எனில் (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    திரும்பு -1;

    எனில் (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	திரும்பு -1;
    }

    is_reading = true;

    திரும்பு wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    முழு
wayland_update(வெறுமை)
{
    எனில் (wayland_ct == வெற்று)
	திரும்பு FAIL;
    திரும்பு vwl_connection_roundtrip(wayland_ct);
}

#எனில் !defined(HAVE_SELECT)

    வெறுமை
wayland_poll_check(முழு revents)
{
    எனில் (wayland_ct == வெற்று)
	திரும்பு;

    is_reading = false;
    எனில் (revents & POLLIN)
    {
	எனில் (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    திரும்பு;
	}
    }
    இல்லையெனில் எனில் (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    இல்லையெனில்
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	திரும்பு;
    }
    wayland_restore_connection();
}

#endif
#எனில் defined(HAVE_SELECT)

    வெறுமை
wayland_select_check(bool is_set)
{
    எனில் (wayland_ct == வெற்று)
	திரும்பு;

    is_reading = false;
    எனில் (is_set)
    {
	எனில் (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	இல்லையெனில்
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    இல்லையெனில்
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    வெறுமை
ex_wlrestore(exarg_T *eap)
{
    எழுத்து *display;

    எனில் (eap->arg == வெற்று || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    இல்லையெனில்
	display = (எழுத்து*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    எனில் (!eap->forceit && wayland_ct != வெற்று &&
	    (display == wayland_display_name ||
	     (wayland_display_name != வெற்று &&
	      STRCMP(wayland_display_name, display) == 0)))
	திரும்பு;

#ifdef FEAT_WAYLAND_CLIPBOARD
    எனில் (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	எனில் (clip_star.owned)
	    clip_lose_selection(&clip_star);
	எனில் (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    எனில் (display != வெற்று)
	display = (எழுத்து*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_மொத்தம் = 5;

    எனில் (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    இல்லையெனில்
	செய்தி(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#எனில் defined(FEAT_WAYLAND_CLIPBOARD)

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
    எனில் (self->gobjects.ext_data_control_manager_v1 != வெற்று)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    இல்லையெனில் எனில் (self->gobjects.zwlr_data_control_manager_v1 != வெற்று)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	எனில் (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    எனில் (!(*supported & req_sel))
    {
	vim_free(manager);
	திரும்பு வெற்று;
    }

    திரும்பு manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    தேர்வு (self->protocol)
    {
	நிலை VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    நிறுத்து;
	நிலை VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    நிறுத்து;
	இயல்பு:
	    vim_free(device);
	    திரும்பு வெற்று;
    }
    device->protocol = self->protocol;

    திரும்பு device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    தேர்வு (self->protocol)
    {
	நிலை VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    நிறுத்து;
	நிலை VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    நிறுத்து;
	இயல்பு:
	    vim_free(source);
	    திரும்பு வெற்று;
    }
    source->protocol = self->protocol;

    திரும்பு source;
}

    நிலையான vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, வெறுமை *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    எனில் (offer == வெற்று)
	திரும்பு வெற்று;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, அளவு(எழுத்து *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    திரும்பு offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	வெறுமை \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	எனில் (self == வெற்று) \
	    திரும்பு; \
	தேர்வு (self->protocol) \
	{ \
	    நிலை VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		நிறுத்து; \
	    நிலை VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		நிறுத்து; \
	    இயல்பு: \
		நிறுத்து; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    வெறுமை
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    எனில் (self == வெற்று)
	திரும்பு;
    தேர்வு (self->protocol)
    {
	நிலை VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    நிறுத்து;
	நிலை VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
வெறுமை
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    எனில் (self == வெற்று)
	திரும்பு;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	நிலையான வெறுமை \
    device_type##_listener_event_data_offer( \
	    வெறுமை *data, \
	    கட்டமைப்பு device_type *device UNUSED, \
	    கட்டமைப்பு offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	நிலையான வெறுமை \
    device_type##_listener_event_selection( \
	    வெறுமை *data, \
	    கட்டமைப்பு device_type *device UNUSED, \
	    கட்டமைப்பு offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = வெற்று; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	நிலையான வெறுமை \
    device_type##_listener_event_primary_selection( \
	    வெறுமை *data, \
	    கட்டமைப்பு device_type *device UNUSED, \
	    கட்டமைப்பு offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = வெற்று; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	நிலையான வெறுமை \
    device_type##_listener_event_finished( \
	    வெறுமை *data, \
	    கட்டமைப்பு device_type *device UNUSED) \
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

நிலையான கட்டமைப்பு ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
நிலையான மாறிலி கட்டமைப்பு zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	நிலையான வெறுமை \
    source_type##_listener_event_send( \
	    வெறுமை *data, கட்டமைப்பு source_type *source UNUSED, \
	    மாறிலி எழுத்து *mime_type, முழு fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	நிலையான வெறுமை \
    source_type##_listener_event_cancelled( \
	    வெறுமை *data, கட்டமைப்பு source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

நிலையான மாறிலி கட்டமைப்பு ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
நிலையான மாறிலி கட்டமைப்பு zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	நிலையான வெறுமை \
    offer_type##_listener_event_offer( \
	    வெறுமை *data, \
	    கட்டமைப்பு offer_type *offer UNUSED, \
	    மாறிலி எழுத்து *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	எனில் (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    எழுத்து *mime = (எழுத்து *)vim_strsave((char_u *)mime_type); \
	    எனில் (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    இல்லையெனில் \
		எனில் (mime != வெற்று) \
		    ((எழுத்து **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

நிலையான மாறிலி கட்டமைப்பு ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
நிலையான மாறிலி கட்டமைப்பு zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	வெறுமை \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    மாறிலி vwl_data_##type##_listener_T *listener, \
	    வெறுமை *data) \
    { \
	எனில் (self == வெற்று) \
	    திரும்பு; \
	self->data = data; \
	self->listener = listener; \
	தேர்வு (self->protocol) \
	{ \
	    நிலை VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		நிறுத்து; \
	    நிலை VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		நிறுத்து; \
	    இயல்பு: \
		நிறுத்து; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    வெறுமை
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    வெறுமை *proxy = source == வெற்று ? வெற்று : source->proxy;

    எனில் (selection == WAYLAND_SELECTION_REGULAR)
    {
	தேர்வு (self->protocol)
	{
	    நிலை VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		நிறுத்து;
	    நிலை VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		நிறுத்து;
	    இயல்பு:
		நிறுத்து;
	}
    }
    இல்லையெனில் எனில் (selection == WAYLAND_SELECTION_PRIMARY)
    {
	தேர்வு (self->protocol)
	{
	    நிலை VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		நிறுத்து;
	    நிலை VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		நிறுத்து;
	    இயல்பு:
		நிறுத்து;
	}
    }
}

    வெறுமை
vwl_data_source_offer(vwl_data_source_T *self, மாறிலி எழுத்து *mime_type)
{
    தேர்வு (self->protocol)
    {
	நிலை VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    நிறுத்து;
	நிலை VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }
}

    வெறுமை
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	மாறிலி எழுத்து *mime_type,
	int32_t fd)
{
    தேர்வு (self->protocol)
    {
	நிலை VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    நிறுத்து;
	நிலை VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
