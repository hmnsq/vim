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

#অন্তর্ভাৱ "vim.h"

#ifdef FEAT_WAYLAND

#অন্তর্ভাৱ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    পূর্ণাঙ্ক
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    সংরচনা pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#অন্যথা
    fd_set	    wfds;
    সংরচনা timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    যদি (self->display.proxy == রিক্ত)
	ঘূৰাই FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    যেতিয়ালৈ (true)
    {
	পূর্ণাঙ্ক ret = wl_display_flush(self->display.proxy);

	যদি (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    যদি (poll(&fds, 1, p_wtm) <= 0)
#অন্যথা
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    যদি (select(self->display.fd + 1, রিক্ত, &wfds, রিক্ত, &tv) <= 0)
		ঘূৰাই FAIL;
#endif
	}
	অন্যথা যদি (ret == -1)
	    ঘূৰাই FAIL;
	অন্যথা
	    ভঙ্গ;
    }

    ঘূৰাই OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    পূর্ণাঙ্ক
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    সংরচনা pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#অন্যথা
    fd_set	    rfds;
    সংরচনা timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    যদি (self->display.proxy == রিক্ত)
	ঘূৰাই -1;

    যেতিয়ালৈ (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	যদি (wl_display_dispatch_pending(self->display.proxy) == -1)
	    ঘূৰাই -1;

    // Send any requests before we starting blocking to read display fd
    যদি (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	ঘূৰাই -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    যদি (poll(&fds, 1, p_wtm) <= 0)
#অন্যথা
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    যদি (select(self->display.fd + 1, &rfds, রিক্ত, রিক্ত, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	ঘূৰাই -1;
    }

    // Read events into the queue
    যদি (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	ঘূৰাই -1;

    // Dispatch those events (call the handlers associated for each event)
    ঘূৰাই wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    স্থির শূন্য
vwl_callback_event_done(শূন্য *data, সংরচনা wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

স্থির ধ্রুৱ সংরচনা wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    পূর্ণাঙ্ক
vwl_connection_roundtrip(vwl_connection_T *self)
{
    সংরচনা wl_callback	*callback;
    পূর্ণাঙ্ক			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    যদি (self->display.proxy == রিক্ত)
	ঘূৰাই FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    যদি (callback == রিক্ত)
	ঘূৰাই FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    যেতিয়ালৈ (true)
    {
	ret = vwl_connection_dispatch(self);

	যদি (done || ret == -1)
	    ভঙ্গ;

#ifdef ELAPSED_FUNC
	যদি (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    ভঙ্গ;
	}
#endif
    }

    যদি (ret == -1)
    {
	যদি (!done)
	    wl_callback_destroy(callback);
	ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    স্থির শূন্য
vwl_log_handler(ধ্রুৱ ৱর্ণ *fmt, va_list args)
{
    // 512 bytes should be big enough
    ৱর্ণ	*buf	= alloc(512);
    ৱর্ণ	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    যদি (buf == রিক্ত)
	ঘূৰাই;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(রিক্ত, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    স্থির শূন্য
wl_seat_listener_event_name(
	শূন্য		*data,
	সংরচনা wl_seat	*seat_proxy UNUSED,
	ধ্রুৱ ৱর্ণ	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (ৱর্ণ *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    স্থির শূন্য
wl_seat_listener_event_capabilities(
	শূন্য		*data,
	সংরচনা wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

স্থির ধ্রুৱ সংরচনা wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

স্থির শূন্য vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    স্থির শূন্য
wl_registry_listener_event_global(
	শূন্য		    *data,
	সংরচনা wl_registry  *registry,
	uint32_t	    name,
	ধ্রুৱ ৱর্ণ	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    যদি (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	সংরচনা wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	যদি (seat_proxy == রিক্ত)
	    ঘূৰাই;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	যদি (seat == রিক্ত || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    ঘূৰাই;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	যদি (vwl_connection_roundtrip(ct) == FAIL || seat->label == রিক্ত)
	{
	    vwl_seat_destroy(seat);
	    ঘূৰাই;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    অন্যথা যদি (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    অন্যথা যদি (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
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
    স্থির শূন্য
wl_registry_listener_event_global_remove(
	শূন্য		    *data UNUSED,
	সংরচনা wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

স্থির ধ্রুৱ সংরচনা wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

স্থির শূন্য vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    যদি (ct->gobjects.object != রিক্ত) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = রিক্ত; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != রিক্ত)

#endif

// Make sure to call wayland_set_display(display);
    স্থির vwl_connection_T *
vwl_connection_new(ধ্রুৱ ৱর্ণ *display)
{
    vwl_connection_T	*ct;

    যদি (wayland_no_connect)
	ঘূৰাই রিক্ত;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    যদি (mch_getenv("XDG_RUNTIME_DIR") == রিক্ত)
	ঘূৰাই রিক্ত;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    যদি (ct == রিক্ত)
	ঘূৰাই রিক্ত;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    যদি (ct->display.proxy == রিক্ত)
    {
	vim_free(ct);
	ঘূৰাই রিক্ত;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    যদি (ct->registry.proxy == রিক্ত)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	ঘূৰাই রিক্ত;
    }

    ga_init2(&ct->gobjects.seats, পরিমাণ(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    যদি (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    স্থির শূন্য
vwl_seat_destroy(vwl_seat_T *self)
{
    যদি (self == রিক্ত)
	ঘূৰাই;
    যদি (self->proxy != রিক্ত)
    {
	যদি (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	অন্যথা
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    স্থির শূন্য
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    বাবে (পূর্ণাঙ্ক i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    যদি (self->registry.proxy != রিক্ত)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = রিক্ত;
    }
    যদি (self->display.proxy != রিক্ত)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = রিক্ত;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ধ্রুৱ ৱর্ণ *label)
{
    যদি ((STRCMP(label, "") == 0 || label == রিক্ত)
	    && self->gobjects.seats.ga_len > 0)
	ঘূৰাই ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    বাবে (পূর্ণাঙ্ক i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	যদি (STRCMP(seat->label, label) == 0)
	    ঘূৰাই seat;
    }
    ঘূৰাই রিক্ত;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    স্থির শূন্য
wayland_set_display(ধ্রুৱ ৱর্ণ *display)
{
    যদি (display == রিক্ত)
	display = (ৱর্ণ*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    অন্যথা যদি (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	গচ্ছ exit;

    যদি (display == রিক্ত)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    যদি (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (ৱর্ণ*)vim_strsave((char_u*)display);
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
    পূর্ণাঙ্ক
wayland_init_connection(ধ্রুৱ ৱর্ণ *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    যদি (wayland_ct == রিক্ত)
	গচ্ছ fail;

    ঘূৰাই OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    ঘূৰাই FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    শূন্য
wayland_uninit_connection(শূন্য)
{
    যদি (wayland_ct == রিক্ত)
	ঘূৰাই;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = রিক্ত;
    is_reading = false;
    wayland_set_display("");
}

স্থির পূর্ণাঙ্ক wayland_ct_restore_গুনতি = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    স্থির শূন্য
wayland_restore_connection(শূন্য)
{
    // No point in restoring the connection if we are exiting or dying.
    যদি (exiting || v_dying || wayland_ct_restore_গুনতি <= 0)
	wayland_set_display("");

    --wayland_ct_restore_গুনতি;
    wayland_uninit_connection();

    যদি (wayland_init_connection(wayland_display_name) == OK)
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
    পূর্ণাঙ্ক
wayland_prepare_read(শূন্য)
{
    যদি (wayland_ct == রিক্ত)
	ঘূৰাই -1;

    যদি (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    যেতিয়ালৈ (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	যদি (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    ঘূৰাই -1;

    যদি (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	ঘূৰাই -1;
    }

    is_reading = true;

    ঘূৰাই wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    পূর্ণাঙ্ক
wayland_update(শূন্য)
{
    যদি (wayland_ct == রিক্ত)
	ঘূৰাই FAIL;
    ঘূৰাই vwl_connection_roundtrip(wayland_ct);
}

#যদি !defined(HAVE_SELECT)

    শূন্য
wayland_poll_check(পূর্ণাঙ্ক revents)
{
    যদি (wayland_ct == রিক্ত)
	ঘূৰাই;

    is_reading = false;
    যদি (revents & POLLIN)
    {
	যদি (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    ঘূৰাই;
	}
    }
    অন্যথা যদি (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    অন্যথা
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	ঘূৰাই;
    }
    wayland_restore_connection();
}

#endif
#যদি defined(HAVE_SELECT)

    শূন্য
wayland_select_check(bool is_set)
{
    যদি (wayland_ct == রিক্ত)
	ঘূৰাই;

    is_reading = false;
    যদি (is_set)
    {
	যদি (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	অন্যথা
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    অন্যথা
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    শূন্য
ex_wlrestore(exarg_T *eap)
{
    ৱর্ণ *display;

    যদি (eap->arg == রিক্ত || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    অন্যথা
	display = (ৱর্ণ*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    যদি (!eap->forceit && wayland_ct != রিক্ত &&
	    (display == wayland_display_name ||
	     (wayland_display_name != রিক্ত &&
	      STRCMP(wayland_display_name, display) == 0)))
	ঘূৰাই;

#ifdef FEAT_WAYLAND_CLIPBOARD
    যদি (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	যদি (clip_star.owned)
	    clip_lose_selection(&clip_star);
	যদি (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    যদি (display != রিক্ত)
	display = (ৱর্ণ*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_গুনতি = 5;

    যদি (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    অন্যথা
	বাৰ্তা(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#যদি defined(FEAT_WAYLAND_CLIPBOARD)

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
    যদি (self->gobjects.ext_data_control_manager_v1 != রিক্ত)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    অন্যথা যদি (self->gobjects.zwlr_data_control_manager_v1 != রিক্ত)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	যদি (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    যদি (!(*supported & req_sel))
    {
	vim_free(manager);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    ৱিকল্প (self->protocol)
    {
	পক্ষ VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    ভঙ্গ;
	পক্ষ VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    ভঙ্গ;
	স্ৱভাৱ:
	    vim_free(device);
	    ঘূৰাই রিক্ত;
    }
    device->protocol = self->protocol;

    ঘূৰাই device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    ৱিকল্প (self->protocol)
    {
	পক্ষ VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    ভঙ্গ;
	পক্ষ VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    ভঙ্গ;
	স্ৱভাৱ:
	    vim_free(source);
	    ঘূৰাই রিক্ত;
    }
    source->protocol = self->protocol;

    ঘূৰাই source;
}

    স্থির vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, শূন্য *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    যদি (offer == রিক্ত)
	ঘূৰাই রিক্ত;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, পরিমাণ(ৱর্ণ *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    ঘূৰাই offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	শূন্য \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	যদি (self == রিক্ত) \
	    ঘূৰাই; \
	ৱিকল্প (self->protocol) \
	{ \
	    পক্ষ VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		ভঙ্গ; \
	    পক্ষ VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		ভঙ্গ; \
	    স্ৱভাৱ: \
		ভঙ্গ; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    শূন্য
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    যদি (self == রিক্ত)
	ঘূৰাই;
    ৱিকল্প (self->protocol)
    {
	পক্ষ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    ভঙ্গ;
	পক্ষ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    ভঙ্গ;
	স্ৱভাৱ:
	    ভঙ্গ;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
শূন্য
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    যদি (self == রিক্ত)
	ঘূৰাই;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	স্থির শূন্য \
    device_type##_listener_event_data_offer( \
	    শূন্য *data, \
	    সংরচনা device_type *device UNUSED, \
	    সংরচনা offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	স্থির শূন্য \
    device_type##_listener_event_selection( \
	    শূন্য *data, \
	    সংরচনা device_type *device UNUSED, \
	    সংরচনা offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = রিক্ত; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	স্থির শূন্য \
    device_type##_listener_event_primary_selection( \
	    শূন্য *data, \
	    সংরচনা device_type *device UNUSED, \
	    সংরচনা offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = রিক্ত; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	স্থির শূন্য \
    device_type##_listener_event_finished( \
	    শূন্য *data, \
	    সংরচনা device_type *device UNUSED) \
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

স্থির সংরচনা ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
স্থির ধ্রুৱ সংরচনা zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	স্থির শূন্য \
    source_type##_listener_event_send( \
	    শূন্য *data, সংরচনা source_type *source UNUSED, \
	    ধ্রুৱ ৱর্ণ *mime_type, পূর্ণাঙ্ক fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	স্থির শূন্য \
    source_type##_listener_event_cancelled( \
	    শূন্য *data, সংরচনা source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

স্থির ধ্রুৱ সংরচনা ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
স্থির ধ্রুৱ সংরচনা zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	স্থির শূন্য \
    offer_type##_listener_event_offer( \
	    শূন্য *data, \
	    সংরচনা offer_type *offer UNUSED, \
	    ধ্রুৱ ৱর্ণ *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	যদি (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    ৱর্ণ *mime = (ৱর্ণ *)vim_strsave((char_u *)mime_type); \
	    যদি (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    অন্যথা \
		যদি (mime != রিক্ত) \
		    ((ৱর্ণ **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

স্থির ধ্রুৱ সংরচনা ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
স্থির ধ্রুৱ সংরচনা zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	শূন্য \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    ধ্রুৱ vwl_data_##type##_listener_T *listener, \
	    শূন্য *data) \
    { \
	যদি (self == রিক্ত) \
	    ঘূৰাই; \
	self->data = data; \
	self->listener = listener; \
	ৱিকল্প (self->protocol) \
	{ \
	    পক্ষ VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		ভঙ্গ; \
	    পক্ষ VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		ভঙ্গ; \
	    স্ৱভাৱ: \
		ভঙ্গ; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    শূন্য
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    শূন্য *proxy = source == রিক্ত ? রিক্ত : source->proxy;

    যদি (selection == WAYLAND_SELECTION_REGULAR)
    {
	ৱিকল্প (self->protocol)
	{
	    পক্ষ VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		ভঙ্গ;
	    পক্ষ VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		ভঙ্গ;
	    স্ৱভাৱ:
		ভঙ্গ;
	}
    }
    অন্যথা যদি (selection == WAYLAND_SELECTION_PRIMARY)
    {
	ৱিকল্প (self->protocol)
	{
	    পক্ষ VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		ভঙ্গ;
	    পক্ষ VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		ভঙ্গ;
	    স্ৱভাৱ:
		ভঙ্গ;
	}
    }
}

    শূন্য
vwl_data_source_offer(vwl_data_source_T *self, ধ্রুৱ ৱর্ণ *mime_type)
{
    ৱিকল্প (self->protocol)
    {
	পক্ষ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    ভঙ্গ;
	পক্ষ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    ভঙ্গ;
	স্ৱভাৱ:
	    ভঙ্গ;
    }
}

    শূন্য
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	ধ্রুৱ ৱর্ণ *mime_type,
	int32_t fd)
{
    ৱিকল্প (self->protocol)
    {
	পক্ষ VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ভঙ্গ;
	পক্ষ VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ভঙ্গ;
	স্ৱভাৱ:
	    ভঙ্গ;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
