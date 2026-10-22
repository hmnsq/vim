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

#شامل "vim.h"

#ifdef FEAT_WAYLAND

#شامل "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    صحیح
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ساختار pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#وگرنه
    fd_set	    wfds;
    ساختار timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    اگر (self->display.proxy == پوچ)
	بازگشت FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    وقتی (true)
    {
	صحیح ret = wl_display_flush(self->display.proxy);

	اگر (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    اگر (poll(&fds, 1, p_wtm) <= 0)
#وگرنه
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    اگر (select(self->display.fd + 1, پوچ, &wfds, پوچ, &tv) <= 0)
		بازگشت FAIL;
#endif
	}
	وگرنه اگر (ret == -1)
	    بازگشت FAIL;
	وگرنه
	    بشکن;
    }

    بازگشت OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    صحیح
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    ساختار pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#وگرنه
    fd_set	    rfds;
    ساختار timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    اگر (self->display.proxy == پوچ)
	بازگشت -1;

    وقتی (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	اگر (wl_display_dispatch_pending(self->display.proxy) == -1)
	    بازگشت -1;

    // Send any requests before we starting blocking to read display fd
    اگر (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	بازگشت -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    اگر (poll(&fds, 1, p_wtm) <= 0)
#وگرنه
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    اگر (select(self->display.fd + 1, &rfds, پوچ, پوچ, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	بازگشت -1;
    }

    // Read events into the queue
    اگر (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	بازگشت -1;

    // Dispatch those events (call the handlers associated for each event)
    بازگشت wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    ایستا تهی
vwl_callback_event_done(تهی *data, ساختار wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

ایستا ثابت ساختار wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    صحیح
vwl_connection_roundtrip(vwl_connection_T *self)
{
    ساختار wl_callback	*callback;
    صحیح			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    اگر (self->display.proxy == پوچ)
	بازگشت FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    اگر (callback == پوچ)
	بازگشت FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    وقتی (true)
    {
	ret = vwl_connection_dispatch(self);

	اگر (done || ret == -1)
	    بشکن;

#ifdef ELAPSED_FUNC
	اگر (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    بشکن;
	}
#endif
    }

    اگر (ret == -1)
    {
	اگر (!done)
	    wl_callback_destroy(callback);
	بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    ایستا تهی
vwl_log_handler(ثابت نویسه *fmt, va_list args)
{
    // 512 bytes should be big enough
    نویسه	*buf	= alloc(512);
    نویسه	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    اگر (buf == پوچ)
	بازگشت;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(پوچ, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    ایستا تهی
wl_seat_listener_event_name(
	تهی		*data,
	ساختار wl_seat	*seat_proxy UNUSED,
	ثابت نویسه	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (نویسه *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    ایستا تهی
wl_seat_listener_event_capabilities(
	تهی		*data,
	ساختار wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

ایستا ثابت ساختار wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

ایستا تهی vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    ایستا تهی
wl_registry_listener_event_global(
	تهی		    *data,
	ساختار wl_registry  *registry,
	uint32_t	    name,
	ثابت نویسه	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    اگر (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	ساختار wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	اگر (seat_proxy == پوچ)
	    بازگشت;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	اگر (seat == پوچ || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    بازگشت;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	اگر (vwl_connection_roundtrip(ct) == FAIL || seat->label == پوچ)
	{
	    vwl_seat_destroy(seat);
	    بازگشت;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    وگرنه اگر (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    وگرنه اگر (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
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
    ایستا تهی
wl_registry_listener_event_global_remove(
	تهی		    *data UNUSED,
	ساختار wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

ایستا ثابت ساختار wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

ایستا تهی vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    اگر (ct->gobjects.object != پوچ) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = پوچ; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != پوچ)

#endif

// Make sure to call wayland_set_display(display);
    ایستا vwl_connection_T *
vwl_connection_new(ثابت نویسه *display)
{
    vwl_connection_T	*ct;

    اگر (wayland_no_connect)
	بازگشت پوچ;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    اگر (mch_getenv("XDG_RUNTIME_DIR") == پوچ)
	بازگشت پوچ;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    اگر (ct == پوچ)
	بازگشت پوچ;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    اگر (ct->display.proxy == پوچ)
    {
	vim_free(ct);
	بازگشت پوچ;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    اگر (ct->registry.proxy == پوچ)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	بازگشت پوچ;
    }

    ga_init2(&ct->gobjects.seats, اندازه(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    اگر (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	بازگشت پوچ;
    }

    بازگشت ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    ایستا تهی
vwl_seat_destroy(vwl_seat_T *self)
{
    اگر (self == پوچ)
	بازگشت;
    اگر (self->proxy != پوچ)
    {
	اگر (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	وگرنه
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    ایستا تهی
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    برای (صحیح i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    اگر (self->registry.proxy != پوچ)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = پوچ;
    }
    اگر (self->display.proxy != پوچ)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = پوچ;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ثابت نویسه *label)
{
    اگر ((STRCMP(label, "") == 0 || label == پوچ)
	    && self->gobjects.seats.ga_len > 0)
	بازگشت ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    برای (صحیح i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	اگر (STRCMP(seat->label, label) == 0)
	    بازگشت seat;
    }
    بازگشت پوچ;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    ایستا تهی
wayland_set_display(ثابت نویسه *display)
{
    اگر (display == پوچ)
	display = (نویسه*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    وگرنه اگر (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	برو exit;

    اگر (display == پوچ)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    اگر (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (نویسه*)vim_strsave((char_u*)display);
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
    صحیح
wayland_init_connection(ثابت نویسه *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    اگر (wayland_ct == پوچ)
	برو fail;

    بازگشت OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    بازگشت FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    تهی
wayland_uninit_connection(تهی)
{
    اگر (wayland_ct == پوچ)
	بازگشت;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = پوچ;
    is_reading = false;
    wayland_set_display("");
}

ایستا صحیح wayland_ct_restore_شمار = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    ایستا تهی
wayland_restore_connection(تهی)
{
    // No point in restoring the connection if we are exiting or dying.
    اگر (exiting || v_dying || wayland_ct_restore_شمار <= 0)
	wayland_set_display("");

    --wayland_ct_restore_شمار;
    wayland_uninit_connection();

    اگر (wayland_init_connection(wayland_display_name) == OK)
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
    صحیح
wayland_prepare_read(تهی)
{
    اگر (wayland_ct == پوچ)
	بازگشت -1;

    اگر (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    وقتی (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	اگر (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    بازگشت -1;

    اگر (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	بازگشت -1;
    }

    is_reading = true;

    بازگشت wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    صحیح
wayland_update(تهی)
{
    اگر (wayland_ct == پوچ)
	بازگشت FAIL;
    بازگشت vwl_connection_roundtrip(wayland_ct);
}

#اگر !defined(HAVE_SELECT)

    تهی
wayland_poll_check(صحیح revents)
{
    اگر (wayland_ct == پوچ)
	بازگشت;

    is_reading = false;
    اگر (revents & POLLIN)
    {
	اگر (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    بازگشت;
	}
    }
    وگرنه اگر (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    وگرنه
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	بازگشت;
    }
    wayland_restore_connection();
}

#endif
#اگر defined(HAVE_SELECT)

    تهی
wayland_select_check(bool is_set)
{
    اگر (wayland_ct == پوچ)
	بازگشت;

    is_reading = false;
    اگر (is_set)
    {
	اگر (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	وگرنه
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    وگرنه
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    تهی
ex_wlrestore(exarg_T *eap)
{
    نویسه *display;

    اگر (eap->arg == پوچ || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    وگرنه
	display = (نویسه*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    اگر (!eap->forceit && wayland_ct != پوچ &&
	    (display == wayland_display_name ||
	     (wayland_display_name != پوچ &&
	      STRCMP(wayland_display_name, display) == 0)))
	بازگشت;

#ifdef FEAT_WAYLAND_CLIPBOARD
    اگر (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	اگر (clip_star.owned)
	    clip_lose_selection(&clip_star);
	اگر (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    اگر (display != پوچ)
	display = (نویسه*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_شمار = 5;

    اگر (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    وگرنه
	پیام(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#اگر defined(FEAT_WAYLAND_CLIPBOARD)

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
    اگر (self->gobjects.ext_data_control_manager_v1 != پوچ)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    وگرنه اگر (self->gobjects.zwlr_data_control_manager_v1 != پوچ)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	اگر (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    اگر (!(*supported & req_sel))
    {
	vim_free(manager);
	بازگشت پوچ;
    }

    بازگشت manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    انتخاب (self->protocol)
    {
	حالت VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    بشکن;
	حالت VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    بشکن;
	پیشفرض:
	    vim_free(device);
	    بازگشت پوچ;
    }
    device->protocol = self->protocol;

    بازگشت device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    انتخاب (self->protocol)
    {
	حالت VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    بشکن;
	حالت VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    بشکن;
	پیشفرض:
	    vim_free(source);
	    بازگشت پوچ;
    }
    source->protocol = self->protocol;

    بازگشت source;
}

    ایستا vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, تهی *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    اگر (offer == پوچ)
	بازگشت پوچ;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, اندازه(نویسه *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    بازگشت offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	تهی \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	اگر (self == پوچ) \
	    بازگشت; \
	انتخاب (self->protocol) \
	{ \
	    حالت VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		بشکن; \
	    حالت VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		بشکن; \
	    پیشفرض: \
		بشکن; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    تهی
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    اگر (self == پوچ)
	بازگشت;
    انتخاب (self->protocol)
    {
	حالت VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    بشکن;
	حالت VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    بشکن;
	پیشفرض:
	    بشکن;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
تهی
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    اگر (self == پوچ)
	بازگشت;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	ایستا تهی \
    device_type##_listener_event_data_offer( \
	    تهی *data, \
	    ساختار device_type *device UNUSED, \
	    ساختار offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	ایستا تهی \
    device_type##_listener_event_selection( \
	    تهی *data, \
	    ساختار device_type *device UNUSED, \
	    ساختار offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = پوچ; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	ایستا تهی \
    device_type##_listener_event_primary_selection( \
	    تهی *data, \
	    ساختار device_type *device UNUSED, \
	    ساختار offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = پوچ; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	ایستا تهی \
    device_type##_listener_event_finished( \
	    تهی *data, \
	    ساختار device_type *device UNUSED) \
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

ایستا ساختار ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
ایستا ثابت ساختار zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	ایستا تهی \
    source_type##_listener_event_send( \
	    تهی *data, ساختار source_type *source UNUSED, \
	    ثابت نویسه *mime_type, صحیح fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	ایستا تهی \
    source_type##_listener_event_cancelled( \
	    تهی *data, ساختار source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

ایستا ثابت ساختار ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
ایستا ثابت ساختار zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	ایستا تهی \
    offer_type##_listener_event_offer( \
	    تهی *data, \
	    ساختار offer_type *offer UNUSED, \
	    ثابت نویسه *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	اگر (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    نویسه *mime = (نویسه *)vim_strsave((char_u *)mime_type); \
	    اگر (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    وگرنه \
		اگر (mime != پوچ) \
		    ((نویسه **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

ایستا ثابت ساختار ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
ایستا ثابت ساختار zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	تهی \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    ثابت vwl_data_##type##_listener_T *listener, \
	    تهی *data) \
    { \
	اگر (self == پوچ) \
	    بازگشت; \
	self->data = data; \
	self->listener = listener; \
	انتخاب (self->protocol) \
	{ \
	    حالت VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		بشکن; \
	    حالت VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		بشکن; \
	    پیشفرض: \
		بشکن; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    تهی
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    تهی *proxy = source == پوچ ? پوچ : source->proxy;

    اگر (selection == WAYLAND_SELECTION_REGULAR)
    {
	انتخاب (self->protocol)
	{
	    حالت VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		بشکن;
	    حالت VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		بشکن;
	    پیشفرض:
		بشکن;
	}
    }
    وگرنه اگر (selection == WAYLAND_SELECTION_PRIMARY)
    {
	انتخاب (self->protocol)
	{
	    حالت VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		بشکن;
	    حالت VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		بشکن;
	    پیشفرض:
		بشکن;
	}
    }
}

    تهی
vwl_data_source_offer(vwl_data_source_T *self, ثابت نویسه *mime_type)
{
    انتخاب (self->protocol)
    {
	حالت VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    بشکن;
	حالت VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    بشکن;
	پیشفرض:
	    بشکن;
    }
}

    تهی
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	ثابت نویسه *mime_type,
	int32_t fd)
{
    انتخاب (self->protocol)
    {
	حالت VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    بشکن;
	حالت VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    بشکن;
	پیشفرض:
	    بشکن;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
