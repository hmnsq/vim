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

#اگر_تعریف_شدہ FEAT_WAYLAND

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
#اگر_غیر_تعریف_شدہ HAVE_SELECT
    ساخت pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#ورنہ
    fd_set	    wfds;
    ساخت timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#اگر_ختم

    اگر (self->display.proxy == تہی)
	واپس FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    جب (true)
    {
	صحیح ret = wl_display_flush(self->display.proxy);

	اگر (ret == -1 && errno == EAGAIN)
	{
#اگر_غیر_تعریف_شدہ HAVE_SELECT
	    اگر (poll(&fds, 1, p_wtm) <= 0)
#ورنہ
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    اگر (select(self->display.fd + 1, تہی, &wfds, تہی, &tv) <= 0)
		واپس FAIL;
#اگر_ختم
	}
	ورنہ اگر (ret == -1)
	    واپس FAIL;
	ورنہ
	    توڑو;
    }

    واپس OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    صحیح
vwl_connection_dispatch(vwl_connection_T *self)
{
#اگر_غیر_تعریف_شدہ HAVE_SELECT
    ساخت pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#ورنہ
    fd_set	    rfds;
    ساخت timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#اگر_ختم

    اگر (self->display.proxy == تہی)
	واپس -1;

    جب (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	اگر (wl_display_dispatch_pending(self->display.proxy) == -1)
	    واپس -1;

    // Send any requests before we starting blocking to read display fd
    اگر (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	واپس -1;
    }

    // Poll until there is data to read from the display fd.
#اگر_غیر_تعریف_شدہ HAVE_SELECT
    اگر (poll(&fds, 1, p_wtm) <= 0)
#ورنہ
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    اگر (select(self->display.fd + 1, &rfds, تہی, تہی, &tv) <= 0)
#اگر_ختم
    {
	wl_display_cancel_read(self->display.proxy);
	واپس -1;
    }

    // Read events into the queue
    اگر (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	واپس -1;

    // Dispatch those events (call the handlers associated for each event)
    واپس wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    ساکن خالی
vwl_callback_event_مکمل(خالی *ڈیٹا, ساخت wl_callback *callback,
	uint32_t callback_ڈیٹا غیر_مستعمل)
{
    *((bool*)ڈیٹا) = true;
    wl_callback_destroy(callback);
}

ساکن ثابت ساخت wl_callback_listener vwl_callback_listener = {
    .مکمل = vwl_callback_event_مکمل
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    صحیح
vwl_connection_roundtrip(vwl_connection_T *self)
{
    ساخت wl_callback	*callback;
    صحیح			ret;
    bool		مکمل = false;
#اگر_تعریف_شدہ ELAPSED_FUNC
    elapsed_T		آغاز_tv;
#اگر_ختم

    اگر (self->display.proxy == تہی)
	واپس FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    اگر (callback == تہی)
	واپس FAIL;

    wl_callback_جوڑو_listener(callback, &vwl_callback_listener, &مکمل);

#اگر_تعریف_شدہ ELAPSED_FUNC
    ELAPSED_INIT(آغاز_tv);
#اگر_ختم

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    جب (true)
    {
	ret = vwl_connection_dispatch(self);

	اگر (مکمل || ret == -1)
	    توڑو;

#اگر_تعریف_شدہ ELAPSED_FUNC
	اگر (ELAPSED_FUNC(آغاز_tv) >= p_wtm)
	{
	    ret = -1;
	    توڑو;
	}
#اگر_ختم
    }

    اگر (ret == -1)
    {
	اگر (!مکمل)
	    wl_callback_destroy(callback);
	واپس FAIL;
    }

    واپس OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    ساکن خالی
vwl_log_handler(ثابت حرف *fmt, va_list دلائل)
{
    // 512 bytes should be big enough
    حرف	*buf	= alloc(512);
    حرف	*سابقہ = _("wayland protocol error -> ");
    size_t	len	= STRLEN(سابقہ);

    اگر (buf == تہی)
	واپس;

    vim_strncpy((char_u*)buf, (char_u*)سابقہ, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, دلائل);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#اگر_تعریف_شدہ FEAT_EVAL
    ch_log(تہی, "%s", buf);
#اگر_ختم
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    ساکن خالی
wl_seat_listener_event_name(
	خالی		*ڈیٹا,
	ساخت wl_seat	*seat_proxy غیر_مستعمل,
	ثابت حرف	*name)
{
    vwl_seat_T *seat = ڈیٹا;

    seat->label = (حرف *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    ساکن خالی
wl_seat_listener_event_capabilities(
	خالی		*ڈیٹا,
	ساخت wl_seat	*seat_proxy غیر_مستعمل,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = ڈیٹا;

    seat->capabilities = capabilities;
}

ساکن ثابت ساخت wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

ساکن خالی vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    ساکن خالی
wl_registry_listener_event_global(
	خالی		    *ڈیٹا,
	ساخت wl_registry  *registry,
	uint32_t	    name,
	ثابت حرف	    *interface,
	uint32_t	    نسخہ_2)
{
    vwl_connection_T *ct = ڈیٹا;

    اگر (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	ساخت wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, نسخہ_2 > 5 ? 5 : نسخہ_2);
	vwl_seat_T	*seat;

	اگر (seat_proxy == تہی)
	    واپس;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	اگر (seat == تہی || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    واپس;
	}

	seat->proxy = seat_proxy;
	wl_seat_جوڑو_listener(seat_proxy, &wl_seat_listener, seat);

	اگر (vwl_connection_roundtrip(ct) == FAIL || seat->label == تہی)
	{
	    vwl_seat_destroy(seat);
	    واپس;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_ڈیٹا)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
    ورنہ اگر (STRCMP(interface, zwlr_ڈیٹا_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_ڈیٹا_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_ڈیٹا_control_manager_v1_interface,
		    نسخہ_2 > 2 ? 2 : نسخہ_2);

    ورنہ اگر (STRCMP(interface, ext_ڈیٹا_control_manager_v1_interface.name) == 0)
	ct->gobjects.ext_ڈیٹا_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &ext_ڈیٹا_control_manager_v1_interface, 1);
#اگر_ختم // FEAT_WAYLAND_CLIPBOARD

}

/*
 * Called when a global object is removed, if so, then do nothing. This is to
 * avoid a global being removed while it is in the process of being used. Let
 * the user call :wlrestore in order to reset everything. Requests to that
 * global will just be ignored on the compositor side.
 */
    ساکن خالی
wl_registry_listener_event_global_remove(
	خالی		    *ڈیٹا غیر_مستعمل,
	ساخت wl_registry  *registry غیر_مستعمل,
	uint32_t	    name غیر_مستعمل)
{
}

ساکن ثابت ساخت wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

ساکن خالی vwl_connection_destroy(vwl_connection_T *self);

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    اگر (ct->gobjects.object != تہی) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = تہی; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != تہی)

#اگر_ختم

// Make sure to call wayland_set_display(display);
    ساکن vwl_connection_T *
vwl_connection_new(ثابت حرف *display)
{
    vwl_connection_T	*ct;

    اگر (wayland_no_connect)
	واپس تہی;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    اگر (mch_getenv("XDG_RUNTIME_DIR") == تہی)
	واپس تہی;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    اگر (ct == تہی)
	واپس تہی;

    // Must set log handler before we connect display in order to work.
    wl_log_سیٹ_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    اگر (ct->display.proxy == تہی)
    {
	vim_free(ct);
	واپس تہی;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    اگر (ct->registry.proxy == تہی)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	واپس تہی;
    }

    ga_init2(&ct->gobjects.seats, حجم(vwl_seat_T *), 1);

    wl_registry_جوڑو_listener(ct->registry.proxy, &wl_registry_listener, ct);

    اگر (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	واپس تہی;
    }

    واپس ct;
}

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    ساکن خالی
vwl_seat_destroy(vwl_seat_T *self)
{
    اگر (self == تہی)
	واپس;
    اگر (self->proxy != تہی)
    {
	اگر (wl_seat_get_نسخہ(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	ورنہ
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    ساکن خالی
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_ڈیٹا_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_ڈیٹا_control_manager_v1)
# endif

    برائے (صحیح i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_ڈیٹا)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    اگر (self->registry.proxy != تہی)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = تہی;
    }
    اگر (self->display.proxy != تہی)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = تہی;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ثابت حرف *label)
{
    اگر ((STRCMP(label, "") == 0 || label == تہی)
	    && self->gobjects.seats.ga_len > 0)
	واپس ((vwl_seat_T **)self->gobjects.seats.ga_ڈیٹا)[0];

    برائے (صحیح i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_ڈیٹا)[i];
	اگر (STRCMP(seat->label, label) == 0)
	    واپس seat;
    }
    واپس تہی;
}

#اگر_ختم

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    ساکن خالی
wayland_سیٹ_display(ثابت حرف *display)
{
    اگر (display == تہی)
	display = (حرف*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    ورنہ اگر (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	جاؤ باہر_نکلو;

    اگر (display == تہی)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    اگر (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (حرف*)vim_strsave((char_u*)display);
    }

باہر_نکلو:
#اگر_تعریف_شدہ FEAT_EVAL
    سیٹ_vim_var_string(VV_WAYLAND_DISPLAY, (char_u*)display, -1);
#اگر_ختم
}

/*
 * Initializes the global Wayland connection. Connects to the Wayland display
 * with given name and binds to global objects as needed. If display is NULL
 * then the $WAYLAND_DISPLAY environment variable will be used (handled by
 * libwayland). Returns FAIL on failure and OK on
 * success
 */
    صحیح
wayland_init_connection(ثابت حرف *display)
{
    wayland_سیٹ_display(display);

    wayland_ct = vwl_connection_new(display);

    اگر (wayland_ct == تہی)
	جاؤ fail;

    واپس OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_سیٹ_display("");
    واپس FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    خالی
wayland_uninit_connection(خالی)
{
    اگر (wayland_ct == تہی)
	واپس;
#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#اگر_ختم
    vwl_connection_destroy(wayland_ct);
    wayland_ct = تہی;
    is_reading = false;
    wayland_سیٹ_display("");
}

ساکن صحیح wayland_ct_restore_گنتی = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    ساکن خالی
wayland_restore_connection(خالی)
{
    // No point in restoring the connection if we are exiting or dying.
    اگر (exiting || v_dying || wayland_ct_restore_گنتی <= 0)
	wayland_سیٹ_display("");

    --wayland_ct_restore_گنتی;
    wayland_uninit_connection();

    اگر (wayland_init_connection(wayland_display_name) == OK)
    {
#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#اگر_ختم
    }
}

/*
 * Should be called before polling (select or poll) the global Wayland
 * connection display fd. Returns fd on success and -1 on failure.
 */
    صحیح
wayland_prepare_read(خالی)
{
    اگر (wayland_ct == تہی)
	واپس -1;

    اگر (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    جب (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	اگر (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    واپس -1;

    اگر (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	واپس -1;
    }

    is_reading = true;

    واپس wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    صحیح
wayland_update(خالی)
{
    اگر (wayland_ct == تہی)
	واپس FAIL;
    واپس vwl_connection_roundtrip(wayland_ct);
}

#اگر !defined(HAVE_SELECT)

    خالی
wayland_poll_check(صحیح revents)
{
    اگر (wayland_ct == تہی)
	واپس;

    is_reading = false;
    اگر (revents & POLLIN)
    {
	اگر (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    واپس;
	}
    }
    ورنہ اگر (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    ورنہ
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	واپس;
    }
    wayland_restore_connection();
}

#اگر_ختم
#اگر defined(HAVE_SELECT)

    خالی
wayland_select_check(bool is_سیٹ)
{
    اگر (wayland_ct == تہی)
	واپس;

    is_reading = false;
    اگر (is_سیٹ)
    {
	اگر (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	ورنہ
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    ورنہ
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#اگر_ختم

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    خالی
ex_wlrestore(exarg_T *eap)
{
    حرف *display;

    اگر (eap->arg == تہی || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    ورنہ
	display = (حرف*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    اگر (!eap->forceit && wayland_ct != تہی &&
	    (display == wayland_display_name ||
	     (wayland_display_name != تہی &&
	      STRCMP(wayland_display_name, display) == 0)))
	واپس;

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
    اگر (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	اگر (clip_ستارہ.owned)
	    clip_lose_selection(&clip_ستارہ);
	اگر (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#اگر_ختم

    اگر (display != تہی)
	display = (حرف*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_گنتی = 5;

    اگر (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#اگر_ختم
    }
    ورنہ
	پیغام(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    انتخاب_clipmethod();
}

#اگر defined(FEAT_WAYLAND_CLIPBOARD)

/*
 * Get a suitable data device manager from connection. "supported" should be
 * initialized to VWL_DATA_PROTOCOL_NONE beforehand. Returns NULL if there are
 * no data device manager available with the required selection.
 */
    vwl_ڈیٹا_device_manager_T *
vwl_connection_get_ڈیٹا_device_manager(
	vwl_connection_T *self,
	wayland_selection_T req_sel,
	int_u *supported)
{
    vwl_ڈیٹا_device_manager_T *manager =
	ALLOC_CLEAR_ONE(vwl_ڈیٹا_device_manager_T);

    // Prioritize ext-data-control-v1 over wlr-data-control-unstable-v1 because
    // it is newer.
    اگر (self->gobjects.ext_ڈیٹا_control_manager_v1 != تہی)
    {
	manager->proxy = self->gobjects.ext_ڈیٹا_control_manager_v1;
	manager->protocol = VWL_ڈیٹا_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    ورنہ اگر (self->gobjects.zwlr_ڈیٹا_control_manager_v1 != تہی)
    {
	manager->proxy = self->gobjects.zwlr_ڈیٹا_control_manager_v1;
	manager->protocol = VWL_ڈیٹا_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	اگر (zwlr_ڈیٹا_control_manager_v1_get_نسخہ(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    اگر (!(*supported & req_sel))
    {
	vim_free(manager);
	واپس تہی;
    }

    واپس manager;
}

    vwl_ڈیٹا_device_T *
vwl_ڈیٹا_device_manager_get_ڈیٹا_device(
	vwl_ڈیٹا_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_ڈیٹا_device_T *device = ALLOC_CLEAR_ONE(vwl_ڈیٹا_device_T);

    انتخاب (self->protocol)
    {
	حالت VWL_ڈیٹا_PROTOCOL_EXT:
	    device->proxy = ext_ڈیٹا_control_manager_v1_get_ڈیٹا_device(
		    self->proxy, seat->proxy);
	    توڑو;
	حالت VWL_ڈیٹا_PROTOCOL_WLR:
	    device->proxy = zwlr_ڈیٹا_control_manager_v1_get_ڈیٹا_device(
		    self->proxy, seat->proxy);
	    توڑو;
	طےشدہ:
	    vim_free(device);
	    واپس تہی;
    }
    device->protocol = self->protocol;

    واپس device;
}

    vwl_ڈیٹا_ماخذ_T *
vwl_ڈیٹا_device_manager_create_ڈیٹا_ماخذ(vwl_ڈیٹا_device_manager_T *self)
{
    vwl_ڈیٹا_ماخذ_T *ماخذ = ALLOC_CLEAR_ONE(vwl_ڈیٹا_ماخذ_T);

    انتخاب (self->protocol)
    {
	حالت VWL_ڈیٹا_PROTOCOL_EXT:
	    ماخذ->proxy = ext_ڈیٹا_control_manager_v1_create_ڈیٹا_ماخذ(
		    self->proxy);
	    توڑو;
	حالت VWL_ڈیٹا_PROTOCOL_WLR:
	    ماخذ->proxy = zwlr_ڈیٹا_control_manager_v1_create_ڈیٹا_ماخذ(
		    self->proxy);
	    توڑو;
	طےشدہ:
	    vim_free(ماخذ);
	    واپس تہی;
    }
    ماخذ->protocol = self->protocol;

    واپس ماخذ;
}

    ساکن vwl_ڈیٹا_offer_T *
vwl_ڈیٹا_device_wrap_offer_proxy(vwl_ڈیٹا_device_T *self, خالی *proxy)
{
    vwl_ڈیٹا_offer_T *offer = ALLOC_CLEAR_ONE(vwl_ڈیٹا_offer_T);

    اگر (offer == تہی)
	واپس تہی;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->ڈیٹا = self->ڈیٹا;
    ga_init2(&offer->mime_types, حجم(حرف *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    واپس offer;
}

# define VWL_FUNC_ڈیٹا_PROXY_DESTROY(type) \
	خالی \
    vwl_ڈیٹا##type##_destroy(vwl_ڈیٹا##type##_T *self) \
    { \
	اگر (self == تہی) \
	    واپس; \
	انتخاب (self->protocol) \
	{ \
	    حالت VWL_ڈیٹا_PROTOCOL_EXT: \
		ext_ڈیٹا_control##type##_v1_destroy(self->proxy); \
		توڑو; \
	    حالت VWL_ڈیٹا_PROTOCOL_WLR: \
		zwlr_ڈیٹا_control##type##_v1_destroy(self->proxy); \
		توڑو; \
	    طےشدہ: \
		توڑو; \
	} \
	vim_free(self); \
    }

VWL_FUNC_ڈیٹا_PROXY_DESTROY(device)
VWL_FUNC_ڈیٹا_PROXY_DESTROY(ماخذ)

    خالی
vwl_ڈیٹا_offer_destroy(vwl_ڈیٹا_offer_T *self)
{
    اگر (self == تہی)
	واپس;
    انتخاب (self->protocol)
    {
	حالت VWL_ڈیٹا_PROTOCOL_EXT:
	    ext_ڈیٹا_control_offer_v1_destroy(self->proxy);
	    توڑو;
	حالت VWL_ڈیٹا_PROTOCOL_WLR:
	    zwlr_ڈیٹا_control_offer_v1_destroy(self->proxy);
	    توڑو;
	طےشدہ:
	    توڑو;
    }
    ga_clear_لڑیاں(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
خالی
vwl_ڈیٹا_device_manager_discard(vwl_ڈیٹا_device_manager_T *self)
{
    اگر (self == تہی)
	واپس;
    vim_free(self);
}

# define VWL_FUNC_ڈیٹا_DEVICE_EVENT_ڈیٹا_OFFER(device_type, offer_type) \
	ساکن خالی \
    device_type##_listener_event_data_offer( \
	    خالی *ڈیٹا, \
	    ساخت device_type *device غیر_مستعمل, \
	    ساخت offer_type *offer) \
    { \
	vwl_ڈیٹا_device_T *self = ڈیٹا; \
	self->offer = vwl_ڈیٹا_device_wrap_offer_proxy(self, offer); \
	self->listener->ڈیٹا_offer(self->ڈیٹا, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_ڈیٹا_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	ساکن خالی \
    device_type##_listener_event_selection( \
	    خالی *ڈیٹا, \
	    ساخت device_type *device غیر_مستعمل, \
	    ساخت offer_type *offer غیر_مستعمل) \
    { \
	vwl_ڈیٹا_device_T *self = ڈیٹا; \
	self->listener->selection(self->ڈیٹا, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = تہی; \
    } \

# define VWL_FUNC_ڈیٹا_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	ساکن خالی \
    device_type##_listener_event_primary_selection( \
	    خالی *ڈیٹا, \
	    ساخت device_type *device غیر_مستعمل, \
	    ساخت offer_type *offer غیر_مستعمل) \
    { \
	vwl_ڈیٹا_device_T *self = ڈیٹا; \
	self->listener->selection(self->ڈیٹا, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = تہی; \
    }

# define VWL_FUNC_ڈیٹا_DEVICE_EVENT_ختم_شدہ(device_type) \
	ساکن خالی \
    device_type##_listener_event_finished( \
	    خالی *ڈیٹا, \
	    ساخت device_type *device غیر_مستعمل) \
    { \
	vwl_ڈیٹا_device_T *self = ڈیٹا; \
	self->listener->ختم_شدہ(self->ڈیٹا, self); \
    }

VWL_FUNC_ڈیٹا_DEVICE_EVENT_ڈیٹا_OFFER(
    ext_ڈیٹا_control_device_v1, ext_ڈیٹا_control_offer_v1)
VWL_FUNC_ڈیٹا_DEVICE_EVENT_ڈیٹا_OFFER(
    zwlr_ڈیٹا_control_device_v1, zwlr_ڈیٹا_control_offer_v1)

VWL_FUNC_ڈیٹا_DEVICE_EVENT_SELECTION(
    ext_ڈیٹا_control_device_v1, ext_ڈیٹا_control_offer_v1
)
VWL_FUNC_ڈیٹا_DEVICE_EVENT_SELECTION(
    zwlr_ڈیٹا_control_device_v1, zwlr_ڈیٹا_control_offer_v1
)
VWL_FUNC_ڈیٹا_DEVICE_EVENT_PRIMARY_SELECTION(
    ext_ڈیٹا_control_device_v1, ext_ڈیٹا_control_offer_v1
)
VWL_FUNC_ڈیٹا_DEVICE_EVENT_PRIMARY_SELECTION(
    zwlr_ڈیٹا_control_device_v1, zwlr_ڈیٹا_control_offer_v1
)

VWL_FUNC_ڈیٹا_DEVICE_EVENT_ختم_شدہ(ext_ڈیٹا_control_device_v1)
VWL_FUNC_ڈیٹا_DEVICE_EVENT_ختم_شدہ(zwlr_ڈیٹا_control_device_v1)

ساکن ساخت ext_ڈیٹا_control_device_v1_listener
    ext_ڈیٹا_control_device_v1_listener = {
	.ڈیٹا_offer = ext_ڈیٹا_control_device_v1_listener_event_ڈیٹا_offer,
	.selection = ext_ڈیٹا_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_ڈیٹا_control_device_v1_listener_event_primary_selection,
	.ختم_شدہ = ext_ڈیٹا_control_device_v1_listener_event_ختم_شدہ
};
ساکن ثابت ساخت zwlr_ڈیٹا_control_device_v1_listener
    zwlr_ڈیٹا_control_device_v1_listener = {
	.ڈیٹا_offer = zwlr_ڈیٹا_control_device_v1_listener_event_ڈیٹا_offer,
	.selection = zwlr_ڈیٹا_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_ڈیٹا_control_device_v1_listener_event_primary_selection,
	.ختم_شدہ = zwlr_ڈیٹا_control_device_v1_listener_event_ختم_شدہ
};

# define VWL_FUNC_ڈیٹا_ماخذ_EVENT_SEND(ماخذ_type) \
	ساکن خالی \
    ماخذ_type##_listener_event_send( \
	    خالی *ڈیٹا, ساخت ماخذ_type *ماخذ غیر_مستعمل, \
	    ثابت حرف *mime_type, صحیح fd) \
    { \
	vwl_ڈیٹا_ماخذ_T *self = ڈیٹا; \
	self->listener->send(self->ڈیٹا, self, mime_type, fd); \
    }

# define VWL_FUNC_ڈیٹا_ماخذ_EVENT_CANCELLED(ماخذ_type) \
	ساکن خالی \
    ماخذ_type##_listener_event_cancelled( \
	    خالی *ڈیٹا, ساخت ماخذ_type *ماخذ غیر_مستعمل) \
    { \
	vwl_ڈیٹا_ماخذ_T *self = ڈیٹا; \
	self->listener->cancelled(self->ڈیٹا, self); \
    }

VWL_FUNC_ڈیٹا_ماخذ_EVENT_SEND(ext_ڈیٹا_control_ماخذ_v1)
VWL_FUNC_ڈیٹا_ماخذ_EVENT_SEND(zwlr_ڈیٹا_control_ماخذ_v1)

VWL_FUNC_ڈیٹا_ماخذ_EVENT_CANCELLED(ext_ڈیٹا_control_ماخذ_v1)
VWL_FUNC_ڈیٹا_ماخذ_EVENT_CANCELLED(zwlr_ڈیٹا_control_ماخذ_v1)

ساکن ثابت ساخت ext_ڈیٹا_control_ماخذ_v1_listener
    ext_ڈیٹا_control_ماخذ_v1_listener = {
	.send = ext_ڈیٹا_control_ماخذ_v1_listener_event_send,
	.cancelled = ext_ڈیٹا_control_ماخذ_v1_listener_event_cancelled
};
ساکن ثابت ساخت zwlr_ڈیٹا_control_ماخذ_v1_listener
    zwlr_ڈیٹا_control_ماخذ_v1_listener = {
	.send = zwlr_ڈیٹا_control_ماخذ_v1_listener_event_send,
	.cancelled = zwlr_ڈیٹا_control_ماخذ_v1_listener_event_cancelled
};

# define VWL_FUNC_ڈیٹا_OFFER_EVENT_OFFER(offer_type) \
	ساکن خالی \
    offer_type##_listener_event_offer( \
	    خالی *ڈیٹا, \
	    ساخت offer_type *offer غیر_مستعمل, \
	    ثابت حرف *mime_type) \
    { \
	vwl_ڈیٹا_offer_T *self = ڈیٹا; \
	اگر (self->listener->offer(self->ڈیٹا, self, mime_type)) \
	{ \
	    حرف *mime = (حرف *)vim_strsave((char_u *)mime_type); \
	    اگر (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    ورنہ \
		اگر (mime != تہی) \
		    ((حرف **)self->mime_types.ga_ڈیٹا) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_ڈیٹا_OFFER_EVENT_OFFER(ext_ڈیٹا_control_offer_v1)
VWL_FUNC_ڈیٹا_OFFER_EVENT_OFFER(zwlr_ڈیٹا_control_offer_v1)

ساکن ثابت ساخت ext_ڈیٹا_control_offer_v1_listener
    ext_ڈیٹا_control_offer_v1_listener = {
	.offer = ext_ڈیٹا_control_offer_v1_listener_event_offer
};
ساکن ثابت ساخت zwlr_ڈیٹا_control_offer_v1_listener
    zwlr_ڈیٹا_control_offer_v1_listener = {
	.offer = zwlr_ڈیٹا_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_ڈیٹا_PROXY_جوڑو_LISTENER(type) \
	خالی \
    vwl_ڈیٹا##type##_add_listener( \
	    vwl_ڈیٹا##type##_T *self, \
	    ثابت vwl_ڈیٹا##type##_listener_T *listener, \
	    خالی *ڈیٹا) \
    { \
	اگر (self == تہی) \
	    واپس; \
	self->ڈیٹا = ڈیٹا; \
	self->listener = listener; \
	انتخاب (self->protocol) \
	{ \
	    حالت VWL_ڈیٹا_PROTOCOL_EXT: \
		ext_ڈیٹا_control##type##_v1_add_listener(self->proxy, \
			&ext_ڈیٹا_control##type##_v1_listener, self); \
		توڑو; \
	    حالت VWL_ڈیٹا_PROTOCOL_WLR: \
		zwlr_ڈیٹا_control##type##_v1_add_listener(self->proxy, \
			&zwlr_ڈیٹا_control##type##_v1_listener, self); \
		توڑو; \
	    طےشدہ: \
		توڑو; \
	} \
    }

VWL_FUNC_ڈیٹا_PROXY_جوڑو_LISTENER(device)
VWL_FUNC_ڈیٹا_PROXY_جوڑو_LISTENER(ماخذ)
VWL_FUNC_ڈیٹا_PROXY_جوڑو_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    خالی
vwl_ڈیٹا_device_سیٹ_selection(
    vwl_ڈیٹا_device_T *self,
    vwl_ڈیٹا_ماخذ_T *ماخذ,
    uint32_t serial غیر_مستعمل,
    wayland_selection_T selection
)
{
    خالی *proxy = ماخذ == تہی ? تہی : ماخذ->proxy;

    اگر (selection == WAYLAND_SELECTION_REGULAR)
    {
	انتخاب (self->protocol)
	{
	    حالت VWL_ڈیٹا_PROTOCOL_EXT:
		ext_ڈیٹا_control_device_v1_سیٹ_selection(self->proxy, proxy);
		توڑو;
	    حالت VWL_ڈیٹا_PROTOCOL_WLR:
		zwlr_ڈیٹا_control_device_v1_سیٹ_selection(self->proxy, proxy);
		توڑو;
	    طےشدہ:
		توڑو;
	}
    }
    ورنہ اگر (selection == WAYLAND_SELECTION_PRIMARY)
    {
	انتخاب (self->protocol)
	{
	    حالت VWL_ڈیٹا_PROTOCOL_EXT:
		ext_ڈیٹا_control_device_v1_سیٹ_primary_selection(
			self->proxy, proxy
			);
		توڑو;
	    حالت VWL_ڈیٹا_PROTOCOL_WLR:
		zwlr_ڈیٹا_control_device_v1_سیٹ_primary_selection(
			self->proxy, proxy
			);
		توڑو;
	    طےشدہ:
		توڑو;
	}
    }
}

    خالی
vwl_ڈیٹا_ماخذ_offer(vwl_ڈیٹا_ماخذ_T *self, ثابت حرف *mime_type)
{
    انتخاب (self->protocol)
    {
	حالت VWL_ڈیٹا_PROTOCOL_EXT:
	    ext_ڈیٹا_control_ماخذ_v1_offer(self->proxy, mime_type);
	    توڑو;
	حالت VWL_ڈیٹا_PROTOCOL_WLR:
	    zwlr_ڈیٹا_control_ماخذ_v1_offer(self->proxy, mime_type);
	    توڑو;
	طےشدہ:
	    توڑو;
    }
}

    خالی
vwl_ڈیٹا_offer_receive(
	vwl_ڈیٹا_offer_T *self,
	ثابت حرف *mime_type,
	int32_t fd)
{
    انتخاب (self->protocol)
    {
	حالت VWL_ڈیٹا_PROTOCOL_EXT:
	    ext_ڈیٹا_control_offer_v1_receive(self->proxy, mime_type, fd);
	    توڑو;
	حالت VWL_ڈیٹا_PROTOCOL_WLR:
	    zwlr_ڈیٹا_control_offer_v1_receive(self->proxy, mime_type, fd);
	    توڑو;
	طےشدہ:
	    توڑو;
    }
}

#اگر_ختم // FEAT_WAYLAND_CLIPBOARD

#اگر_ختم // FEAT_WAYLAND
