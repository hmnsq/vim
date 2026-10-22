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

#ضمن "vim.h"

#اگر_تعریف_شده FEAT_WAYLAND

#ضمن "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    صحيح
vwl_connection_flush(vwl_connection_T *self)
{
#اگر_تعریف_نشده HAVE_SELECT
    بنية pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#وإلا
    fd_set	    wfds;
    بنية timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#پایان_اگر

    إذا (self->display.proxy == عدم)
	أرجع FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    طالما (true)
    {
	صحيح ret = wl_display_flush(self->display.proxy);

	إذا (ret == -1 && errno == EAGAIN)
	{
#اگر_تعریف_نشده HAVE_SELECT
	    إذا (poll(&fds, 1, p_wtm) <= 0)
#وإلا
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    إذا (select(self->display.fd + 1, عدم, &wfds, عدم, &tv) <= 0)
		أرجع FAIL;
#پایان_اگر
	}
	وإلا إذا (ret == -1)
	    أرجع FAIL;
	وإلا
	    اكسر;
    }

    أرجع OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    صحيح
vwl_connection_dispatch(vwl_connection_T *self)
{
#اگر_تعریف_نشده HAVE_SELECT
    بنية pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#وإلا
    fd_set	    rfds;
    بنية timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#پایان_اگر

    إذا (self->display.proxy == عدم)
	أرجع -1;

    طالما (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	إذا (wl_display_dispatch_pending(self->display.proxy) == -1)
	    أرجع -1;

    // Send any requests before we starting blocking to read display fd
    إذا (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	أرجع -1;
    }

    // Poll until there is data to read from the display fd.
#اگر_تعریف_نشده HAVE_SELECT
    إذا (poll(&fds, 1, p_wtm) <= 0)
#وإلا
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    إذا (select(self->display.fd + 1, &rfds, عدم, عدم, &tv) <= 0)
#پایان_اگر
    {
	wl_display_cancel_read(self->display.proxy);
	أرجع -1;
    }

    // Read events into the queue
    إذا (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	أرجع -1;

    // Dispatch those events (call the handlers associated for each event)
    أرجع wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    ساكن فراغ
vwl_callback_event_انجامشده(فراغ *داده, بنية wl_callback *callback,
	uint32_t callback_داده استفادهنشده)
{
    *((bool*)داده) = true;
    wl_callback_destroy(callback);
}

ساكن ثابت بنية wl_callback_listener vwl_callback_listener = {
    .انجامشده = vwl_callback_event_انجامشده
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    صحيح
vwl_connection_roundtrip(vwl_connection_T *self)
{
    بنية wl_callback	*callback;
    صحيح			ret;
    bool		انجامشده = false;
#اگر_تعریف_شده ELAPSED_FUNC
    elapsed_T		شروع_tv;
#پایان_اگر

    إذا (self->display.proxy == عدم)
	أرجع FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    إذا (callback == عدم)
	أرجع FAIL;

    wl_callback_جمع_کن_listener(callback, &vwl_callback_listener, &انجامشده);

#اگر_تعریف_شده ELAPSED_FUNC
    ELAPSED_INIT(شروع_tv);
#پایان_اگر

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    طالما (true)
    {
	ret = vwl_connection_dispatch(self);

	إذا (انجامشده || ret == -1)
	    اكسر;

#اگر_تعریف_شده ELAPSED_FUNC
	إذا (ELAPSED_FUNC(شروع_tv) >= p_wtm)
	{
	    ret = -1;
	    اكسر;
	}
#پایان_اگر
    }

    إذا (ret == -1)
    {
	إذا (!انجامشده)
	    wl_callback_destroy(callback);
	أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    ساكن فراغ
vwl_log_handler(ثابت حرف *fmt, va_list آرگومانها)
{
    // 512 bytes should be big enough
    حرف	*buf	= alloc(512);
    حرف	*پیشوند = _("wayland protocol error -> ");
    size_t	len	= STRLEN(پیشوند);

    إذا (buf == عدم)
	أرجع;

    vim_strncpy((char_u*)buf, (char_u*)پیشوند, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, آرگومانها);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#اگر_تعریف_شده FEAT_EVAL
    ch_log(عدم, "%s", buf);
#پایان_اگر
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    ساكن فراغ
wl_seat_listener_event_name(
	فراغ		*داده,
	بنية wl_seat	*seat_proxy استفادهنشده,
	ثابت حرف	*name)
{
    vwl_seat_T *seat = داده;

    seat->label = (حرف *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    ساكن فراغ
wl_seat_listener_event_capabilities(
	فراغ		*داده,
	بنية wl_seat	*seat_proxy استفادهنشده,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = داده;

    seat->capabilities = capabilities;
}

ساكن ثابت بنية wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

ساكن فراغ vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    ساكن فراغ
wl_registry_listener_event_global(
	فراغ		    *داده,
	بنية wl_registry  *registry,
	uint32_t	    name,
	ثابت حرف	    *interface,
	uint32_t	    نسخه_2)
{
    vwl_connection_T *ct = داده;

    إذا (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	بنية wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, نسخه_2 > 5 ? 5 : نسخه_2);
	vwl_seat_T	*seat;

	إذا (seat_proxy == عدم)
	    أرجع;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	إذا (seat == عدم || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    أرجع;
	}

	seat->proxy = seat_proxy;
	wl_seat_جمع_کن_listener(seat_proxy, &wl_seat_listener, seat);

	إذا (vwl_connection_roundtrip(ct) == FAIL || seat->label == عدم)
	{
	    vwl_seat_destroy(seat);
	    أرجع;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_داده)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD
    وإلا إذا (STRCMP(interface, zwlr_داده_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_داده_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_داده_control_manager_v1_interface,
		    نسخه_2 > 2 ? 2 : نسخه_2);

    وإلا إذا (STRCMP(interface, ext_داده_control_manager_v1_interface.name) == 0)
	ct->gobjects.ext_داده_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &ext_داده_control_manager_v1_interface, 1);
#پایان_اگر // FEAT_WAYLAND_CLIPBOARD

}

/*
 * Called when a global object is removed, if so, then do nothing. This is to
 * avoid a global being removed while it is in the process of being used. Let
 * the user call :wlrestore in order to reset everything. Requests to that
 * global will just be ignored on the compositor side.
 */
    ساكن فراغ
wl_registry_listener_event_global_remove(
	فراغ		    *داده استفادهنشده,
	بنية wl_registry  *registry استفادهنشده,
	uint32_t	    name استفادهنشده)
{
}

ساكن ثابت بنية wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

ساكن فراغ vwl_connection_destroy(vwl_connection_T *self);

#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    إذا (ct->gobjects.object != عدم) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = عدم; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != عدم)

#پایان_اگر

// Make sure to call wayland_set_display(display);
    ساكن vwl_connection_T *
vwl_connection_new(ثابت حرف *display)
{
    vwl_connection_T	*ct;

    إذا (wayland_no_connect)
	أرجع عدم;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    إذا (mch_getenv("XDG_RUNTIME_DIR") == عدم)
	أرجع عدم;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    إذا (ct == عدم)
	أرجع عدم;

    // Must set log handler before we connect display in order to work.
    wl_log_مجموعه_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    إذا (ct->display.proxy == عدم)
    {
	vim_free(ct);
	أرجع عدم;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    إذا (ct->registry.proxy == عدم)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	أرجع عدم;
    }

    ga_init2(&ct->gobjects.seats, حجم(vwl_seat_T *), 1);

    wl_registry_جمع_کن_listener(ct->registry.proxy, &wl_registry_listener, ct);

    إذا (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	أرجع عدم;
    }

    أرجع ct;
}

#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    ساكن فراغ
vwl_seat_destroy(vwl_seat_T *self)
{
    إذا (self == عدم)
	أرجع;
    إذا (self->proxy != عدم)
    {
	إذا (wl_seat_get_نسخه(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	وإلا
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    ساكن فراغ
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_داده_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_داده_control_manager_v1)
# endif

    لكل (صحيح i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_داده)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    إذا (self->registry.proxy != عدم)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = عدم;
    }
    إذا (self->display.proxy != عدم)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = عدم;
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
    إذا ((STRCMP(label, "") == 0 || label == عدم)
	    && self->gobjects.seats.ga_len > 0)
	أرجع ((vwl_seat_T **)self->gobjects.seats.ga_داده)[0];

    لكل (صحيح i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_داده)[i];
	إذا (STRCMP(seat->label, label) == 0)
	    أرجع seat;
    }
    أرجع عدم;
}

#پایان_اگر

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    ساكن فراغ
wayland_مجموعه_display(ثابت حرف *display)
{
    إذا (display == عدم)
	display = (حرف*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    وإلا إذا (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	اذهب خروج;

    إذا (display == عدم)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    إذا (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (حرف*)vim_strsave((char_u*)display);
    }

خروج:
#اگر_تعریف_شده FEAT_EVAL
    مجموعه_vim_var_string(VV_WAYLAND_DISPLAY, (char_u*)display, -1);
#پایان_اگر
}

/*
 * Initializes the global Wayland connection. Connects to the Wayland display
 * with given name and binds to global objects as needed. If display is NULL
 * then the $WAYLAND_DISPLAY environment variable will be used (handled by
 * libwayland). Returns FAIL on failure and OK on
 * success
 */
    صحيح
wayland_init_connection(ثابت حرف *display)
{
    wayland_مجموعه_display(display);

    wayland_ct = vwl_connection_new(display);

    إذا (wayland_ct == عدم)
	اذهب fail;

    أرجع OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_مجموعه_display("");
    أرجع FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    فراغ
wayland_uninit_connection(فراغ)
{
    إذا (wayland_ct == عدم)
	أرجع;
#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#پایان_اگر
    vwl_connection_destroy(wayland_ct);
    wayland_ct = عدم;
    is_reading = false;
    wayland_مجموعه_display("");
}

ساكن صحيح wayland_ct_restore_شمار = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    ساكن فراغ
wayland_restore_connection(فراغ)
{
    // No point in restoring the connection if we are exiting or dying.
    إذا (exiting || v_dying || wayland_ct_restore_شمار <= 0)
	wayland_مجموعه_display("");

    --wayland_ct_restore_شمار;
    wayland_uninit_connection();

    إذا (wayland_init_connection(wayland_display_name) == OK)
    {
#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#پایان_اگر
    }
}

/*
 * Should be called before polling (select or poll) the global Wayland
 * connection display fd. Returns fd on success and -1 on failure.
 */
    صحيح
wayland_prepare_read(فراغ)
{
    إذا (wayland_ct == عدم)
	أرجع -1;

    إذا (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    طالما (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	إذا (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    أرجع -1;

    إذا (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	أرجع -1;
    }

    is_reading = true;

    أرجع wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    صحيح
wayland_update(فراغ)
{
    إذا (wayland_ct == عدم)
	أرجع FAIL;
    أرجع vwl_connection_roundtrip(wayland_ct);
}

#إذا !defined(HAVE_SELECT)

    فراغ
wayland_poll_check(صحيح revents)
{
    إذا (wayland_ct == عدم)
	أرجع;

    is_reading = false;
    إذا (revents & POLLIN)
    {
	إذا (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    أرجع;
	}
    }
    وإلا إذا (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    وإلا
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	أرجع;
    }
    wayland_restore_connection();
}

#پایان_اگر
#إذا defined(HAVE_SELECT)

    فراغ
wayland_select_check(bool is_مجموعه)
{
    إذا (wayland_ct == عدم)
	أرجع;

    is_reading = false;
    إذا (is_مجموعه)
    {
	إذا (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	وإلا
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    وإلا
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#پایان_اگر

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    فراغ
ex_wlrestore(exarg_T *eap)
{
    حرف *display;

    إذا (eap->arg == عدم || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    وإلا
	display = (حرف*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    إذا (!eap->forceit && wayland_ct != عدم &&
	    (display == wayland_display_name ||
	     (wayland_display_name != عدم &&
	      STRCMP(wayland_display_name, display) == 0)))
	أرجع;

#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD
    إذا (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	إذا (clip_ستاره.owned)
	    clip_lose_selection(&clip_ستاره);
	إذا (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#پایان_اگر

    إذا (display != عدم)
	display = (حرف*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_شمار = 5;

    إذا (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#اگر_تعریف_شده FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#پایان_اگر
    }
    وإلا
	پیام(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    انتخاب_clipmethod();
}

#إذا defined(FEAT_WAYLAND_CLIPBOARD)

/*
 * Get a suitable data device manager from connection. "supported" should be
 * initialized to VWL_DATA_PROTOCOL_NONE beforehand. Returns NULL if there are
 * no data device manager available with the required selection.
 */
    vwl_داده_device_manager_T *
vwl_connection_get_داده_device_manager(
	vwl_connection_T *self,
	wayland_selection_T req_sel,
	int_u *supported)
{
    vwl_داده_device_manager_T *manager =
	ALLOC_CLEAR_ONE(vwl_داده_device_manager_T);

    // Prioritize ext-data-control-v1 over wlr-data-control-unstable-v1 because
    // it is newer.
    إذا (self->gobjects.ext_داده_control_manager_v1 != عدم)
    {
	manager->proxy = self->gobjects.ext_داده_control_manager_v1;
	manager->protocol = VWL_داده_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    وإلا إذا (self->gobjects.zwlr_داده_control_manager_v1 != عدم)
    {
	manager->proxy = self->gobjects.zwlr_داده_control_manager_v1;
	manager->protocol = VWL_داده_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	إذا (zwlr_داده_control_manager_v1_get_نسخه(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    إذا (!(*supported & req_sel))
    {
	vim_free(manager);
	أرجع عدم;
    }

    أرجع manager;
}

    vwl_داده_device_T *
vwl_داده_device_manager_get_داده_device(
	vwl_داده_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_داده_device_T *device = ALLOC_CLEAR_ONE(vwl_داده_device_T);

    بدل (self->protocol)
    {
	حالة VWL_داده_PROTOCOL_EXT:
	    device->proxy = ext_داده_control_manager_v1_get_داده_device(
		    self->proxy, seat->proxy);
	    اكسر;
	حالة VWL_داده_PROTOCOL_WLR:
	    device->proxy = zwlr_داده_control_manager_v1_get_داده_device(
		    self->proxy, seat->proxy);
	    اكسر;
	افتراضي:
	    vim_free(device);
	    أرجع عدم;
    }
    device->protocol = self->protocol;

    أرجع device;
}

    vwl_داده_منبع_T *
vwl_داده_device_manager_create_داده_منبع(vwl_داده_device_manager_T *self)
{
    vwl_داده_منبع_T *منبع = ALLOC_CLEAR_ONE(vwl_داده_منبع_T);

    بدل (self->protocol)
    {
	حالة VWL_داده_PROTOCOL_EXT:
	    منبع->proxy = ext_داده_control_manager_v1_create_داده_منبع(
		    self->proxy);
	    اكسر;
	حالة VWL_داده_PROTOCOL_WLR:
	    منبع->proxy = zwlr_داده_control_manager_v1_create_داده_منبع(
		    self->proxy);
	    اكسر;
	افتراضي:
	    vim_free(منبع);
	    أرجع عدم;
    }
    منبع->protocol = self->protocol;

    أرجع منبع;
}

    ساكن vwl_داده_offer_T *
vwl_داده_device_wrap_offer_proxy(vwl_داده_device_T *self, فراغ *proxy)
{
    vwl_داده_offer_T *offer = ALLOC_CLEAR_ONE(vwl_داده_offer_T);

    إذا (offer == عدم)
	أرجع عدم;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->داده = self->داده;
    ga_init2(&offer->mime_types, حجم(حرف *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    أرجع offer;
}

# define VWL_FUNC_داده_PROXY_DESTROY(type) \
	فراغ \
    vwl_داده##type##_destroy(vwl_داده##type##_T *self) \
    { \
	إذا (self == عدم) \
	    أرجع; \
	بدل (self->protocol) \
	{ \
	    حالة VWL_داده_PROTOCOL_EXT: \
		ext_داده_control##type##_v1_destroy(self->proxy); \
		اكسر; \
	    حالة VWL_داده_PROTOCOL_WLR: \
		zwlr_داده_control##type##_v1_destroy(self->proxy); \
		اكسر; \
	    افتراضي: \
		اكسر; \
	} \
	vim_free(self); \
    }

VWL_FUNC_داده_PROXY_DESTROY(device)
VWL_FUNC_داده_PROXY_DESTROY(منبع)

    فراغ
vwl_داده_offer_destroy(vwl_داده_offer_T *self)
{
    إذا (self == عدم)
	أرجع;
    بدل (self->protocol)
    {
	حالة VWL_داده_PROTOCOL_EXT:
	    ext_داده_control_offer_v1_destroy(self->proxy);
	    اكسر;
	حالة VWL_داده_PROTOCOL_WLR:
	    zwlr_داده_control_offer_v1_destroy(self->proxy);
	    اكسر;
	افتراضي:
	    اكسر;
    }
    ga_clear_رشتهها(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
فراغ
vwl_داده_device_manager_discard(vwl_داده_device_manager_T *self)
{
    إذا (self == عدم)
	أرجع;
    vim_free(self);
}

# define VWL_FUNC_داده_DEVICE_EVENT_داده_OFFER(device_type, offer_type) \
	ساكن فراغ \
    device_type##_listener_event_data_offer( \
	    فراغ *داده, \
	    بنية device_type *device استفادهنشده, \
	    بنية offer_type *offer) \
    { \
	vwl_داده_device_T *self = داده; \
	self->offer = vwl_داده_device_wrap_offer_proxy(self, offer); \
	self->listener->داده_offer(self->داده, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_داده_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	ساكن فراغ \
    device_type##_listener_event_selection( \
	    فراغ *داده, \
	    بنية device_type *device استفادهنشده, \
	    بنية offer_type *offer استفادهنشده) \
    { \
	vwl_داده_device_T *self = داده; \
	self->listener->selection(self->داده, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = عدم; \
    } \

# define VWL_FUNC_داده_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	ساكن فراغ \
    device_type##_listener_event_primary_selection( \
	    فراغ *داده, \
	    بنية device_type *device استفادهنشده, \
	    بنية offer_type *offer استفادهنشده) \
    { \
	vwl_داده_device_T *self = داده; \
	self->listener->selection(self->داده, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = عدم; \
    }

# define VWL_FUNC_داده_DEVICE_EVENT_پایانیافته(device_type) \
	ساكن فراغ \
    device_type##_listener_event_finished( \
	    فراغ *داده, \
	    بنية device_type *device استفادهنشده) \
    { \
	vwl_داده_device_T *self = داده; \
	self->listener->پایانیافته(self->داده, self); \
    }

VWL_FUNC_داده_DEVICE_EVENT_داده_OFFER(
    ext_داده_control_device_v1, ext_داده_control_offer_v1)
VWL_FUNC_داده_DEVICE_EVENT_داده_OFFER(
    zwlr_داده_control_device_v1, zwlr_داده_control_offer_v1)

VWL_FUNC_داده_DEVICE_EVENT_SELECTION(
    ext_داده_control_device_v1, ext_داده_control_offer_v1
)
VWL_FUNC_داده_DEVICE_EVENT_SELECTION(
    zwlr_داده_control_device_v1, zwlr_داده_control_offer_v1
)
VWL_FUNC_داده_DEVICE_EVENT_PRIMARY_SELECTION(
    ext_داده_control_device_v1, ext_داده_control_offer_v1
)
VWL_FUNC_داده_DEVICE_EVENT_PRIMARY_SELECTION(
    zwlr_داده_control_device_v1, zwlr_داده_control_offer_v1
)

VWL_FUNC_داده_DEVICE_EVENT_پایانیافته(ext_داده_control_device_v1)
VWL_FUNC_داده_DEVICE_EVENT_پایانیافته(zwlr_داده_control_device_v1)

ساكن بنية ext_داده_control_device_v1_listener
    ext_داده_control_device_v1_listener = {
	.داده_offer = ext_داده_control_device_v1_listener_event_داده_offer,
	.selection = ext_داده_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_داده_control_device_v1_listener_event_primary_selection,
	.پایانیافته = ext_داده_control_device_v1_listener_event_پایانیافته
};
ساكن ثابت بنية zwlr_داده_control_device_v1_listener
    zwlr_داده_control_device_v1_listener = {
	.داده_offer = zwlr_داده_control_device_v1_listener_event_داده_offer,
	.selection = zwlr_داده_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_داده_control_device_v1_listener_event_primary_selection,
	.پایانیافته = zwlr_داده_control_device_v1_listener_event_پایانیافته
};

# define VWL_FUNC_داده_منبع_EVENT_SEND(منبع_type) \
	ساكن فراغ \
    منبع_type##_listener_event_send( \
	    فراغ *داده, بنية منبع_type *منبع استفادهنشده, \
	    ثابت حرف *mime_type, صحيح fd) \
    { \
	vwl_داده_منبع_T *self = داده; \
	self->listener->send(self->داده, self, mime_type, fd); \
    }

# define VWL_FUNC_داده_منبع_EVENT_CANCELLED(منبع_type) \
	ساكن فراغ \
    منبع_type##_listener_event_cancelled( \
	    فراغ *داده, بنية منبع_type *منبع استفادهنشده) \
    { \
	vwl_داده_منبع_T *self = داده; \
	self->listener->cancelled(self->داده, self); \
    }

VWL_FUNC_داده_منبع_EVENT_SEND(ext_داده_control_منبع_v1)
VWL_FUNC_داده_منبع_EVENT_SEND(zwlr_داده_control_منبع_v1)

VWL_FUNC_داده_منبع_EVENT_CANCELLED(ext_داده_control_منبع_v1)
VWL_FUNC_داده_منبع_EVENT_CANCELLED(zwlr_داده_control_منبع_v1)

ساكن ثابت بنية ext_داده_control_منبع_v1_listener
    ext_داده_control_منبع_v1_listener = {
	.send = ext_داده_control_منبع_v1_listener_event_send,
	.cancelled = ext_داده_control_منبع_v1_listener_event_cancelled
};
ساكن ثابت بنية zwlr_داده_control_منبع_v1_listener
    zwlr_داده_control_منبع_v1_listener = {
	.send = zwlr_داده_control_منبع_v1_listener_event_send,
	.cancelled = zwlr_داده_control_منبع_v1_listener_event_cancelled
};

# define VWL_FUNC_داده_OFFER_EVENT_OFFER(offer_type) \
	ساكن فراغ \
    offer_type##_listener_event_offer( \
	    فراغ *داده, \
	    بنية offer_type *offer استفادهنشده, \
	    ثابت حرف *mime_type) \
    { \
	vwl_داده_offer_T *self = داده; \
	إذا (self->listener->offer(self->داده, self, mime_type)) \
	{ \
	    حرف *mime = (حرف *)vim_strsave((char_u *)mime_type); \
	    إذا (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    وإلا \
		إذا (mime != عدم) \
		    ((حرف **)self->mime_types.ga_داده) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_داده_OFFER_EVENT_OFFER(ext_داده_control_offer_v1)
VWL_FUNC_داده_OFFER_EVENT_OFFER(zwlr_داده_control_offer_v1)

ساكن ثابت بنية ext_داده_control_offer_v1_listener
    ext_داده_control_offer_v1_listener = {
	.offer = ext_داده_control_offer_v1_listener_event_offer
};
ساكن ثابت بنية zwlr_داده_control_offer_v1_listener
    zwlr_داده_control_offer_v1_listener = {
	.offer = zwlr_داده_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_داده_PROXY_جمع_کن_LISTENER(type) \
	فراغ \
    vwl_داده##type##_add_listener( \
	    vwl_داده##type##_T *self, \
	    ثابت vwl_داده##type##_listener_T *listener, \
	    فراغ *داده) \
    { \
	إذا (self == عدم) \
	    أرجع; \
	self->داده = داده; \
	self->listener = listener; \
	بدل (self->protocol) \
	{ \
	    حالة VWL_داده_PROTOCOL_EXT: \
		ext_داده_control##type##_v1_add_listener(self->proxy, \
			&ext_داده_control##type##_v1_listener, self); \
		اكسر; \
	    حالة VWL_داده_PROTOCOL_WLR: \
		zwlr_داده_control##type##_v1_add_listener(self->proxy, \
			&zwlr_داده_control##type##_v1_listener, self); \
		اكسر; \
	    افتراضي: \
		اكسر; \
	} \
    }

VWL_FUNC_داده_PROXY_جمع_کن_LISTENER(device)
VWL_FUNC_داده_PROXY_جمع_کن_LISTENER(منبع)
VWL_FUNC_داده_PROXY_جمع_کن_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    فراغ
vwl_داده_device_مجموعه_selection(
    vwl_داده_device_T *self,
    vwl_داده_منبع_T *منبع,
    uint32_t serial استفادهنشده,
    wayland_selection_T selection
)
{
    فراغ *proxy = منبع == عدم ? عدم : منبع->proxy;

    إذا (selection == WAYLAND_SELECTION_REGULAR)
    {
	بدل (self->protocol)
	{
	    حالة VWL_داده_PROTOCOL_EXT:
		ext_داده_control_device_v1_مجموعه_selection(self->proxy, proxy);
		اكسر;
	    حالة VWL_داده_PROTOCOL_WLR:
		zwlr_داده_control_device_v1_مجموعه_selection(self->proxy, proxy);
		اكسر;
	    افتراضي:
		اكسر;
	}
    }
    وإلا إذا (selection == WAYLAND_SELECTION_PRIMARY)
    {
	بدل (self->protocol)
	{
	    حالة VWL_داده_PROTOCOL_EXT:
		ext_داده_control_device_v1_مجموعه_primary_selection(
			self->proxy, proxy
			);
		اكسر;
	    حالة VWL_داده_PROTOCOL_WLR:
		zwlr_داده_control_device_v1_مجموعه_primary_selection(
			self->proxy, proxy
			);
		اكسر;
	    افتراضي:
		اكسر;
	}
    }
}

    فراغ
vwl_داده_منبع_offer(vwl_داده_منبع_T *self, ثابت حرف *mime_type)
{
    بدل (self->protocol)
    {
	حالة VWL_داده_PROTOCOL_EXT:
	    ext_داده_control_منبع_v1_offer(self->proxy, mime_type);
	    اكسر;
	حالة VWL_داده_PROTOCOL_WLR:
	    zwlr_داده_control_منبع_v1_offer(self->proxy, mime_type);
	    اكسر;
	افتراضي:
	    اكسر;
    }
}

    فراغ
vwl_داده_offer_receive(
	vwl_داده_offer_T *self,
	ثابت حرف *mime_type,
	int32_t fd)
{
    بدل (self->protocol)
    {
	حالة VWL_داده_PROTOCOL_EXT:
	    ext_داده_control_offer_v1_receive(self->proxy, mime_type, fd);
	    اكسر;
	حالة VWL_داده_PROTOCOL_WLR:
	    zwlr_داده_control_offer_v1_receive(self->proxy, mime_type, fd);
	    اكسر;
	افتراضي:
	    اكسر;
    }
}

#پایان_اگر // FEAT_WAYLAND_CLIPBOARD

#پایان_اگر // FEAT_WAYLAND
