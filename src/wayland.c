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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_WAYLAND

#అన్తర్భావ "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    పూర్ణాంకం
vwl_connection_flush(vwl_connection_T *self)
{
#నిర్వచితం_కాకపోతే HAVE_SELECT
    నిర్మాణం pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#లేకపోతే
    fd_set	    wfds;
    నిర్మాణం timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#అయితే_ముగింపు

    ఒకవేళ (self->display.proxy == ఖాళీ)
	తిరిగి FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    అయితే (true)
    {
	పూర్ణాంకం ret = wl_display_flush(self->display.proxy);

	ఒకవేళ (ret == -1 && errno == EAGAIN)
	{
#నిర్వచితం_కాకపోతే HAVE_SELECT
	    ఒకవేళ (poll(&fds, 1, p_wtm) <= 0)
#లేకపోతే
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    ఒకవేళ (select(self->display.fd + 1, ఖాళీ, &wfds, ఖాళీ, &tv) <= 0)
		తిరిగి FAIL;
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (ret == -1)
	    తిరిగి FAIL;
	లేకపోతే
	    ఆపు;
    }

    తిరిగి OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    పూర్ణాంకం
vwl_connection_dispatch(vwl_connection_T *self)
{
#నిర్వచితం_కాకపోతే HAVE_SELECT
    నిర్మాణం pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#లేకపోతే
    fd_set	    rfds;
    నిర్మాణం timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#అయితే_ముగింపు

    ఒకవేళ (self->display.proxy == ఖాళీ)
	తిరిగి -1;

    అయితే (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	ఒకవేళ (wl_display_dispatch_pending(self->display.proxy) == -1)
	    తిరిగి -1;

    // Send any requests before we starting blocking to read display fd
    ఒకవేళ (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	తిరిగి -1;
    }

    // Poll until there is data to read from the display fd.
#నిర్వచితం_కాకపోతే HAVE_SELECT
    ఒకవేళ (poll(&fds, 1, p_wtm) <= 0)
#లేకపోతే
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    ఒకవేళ (select(self->display.fd + 1, &rfds, ఖాళీ, ఖాళీ, &tv) <= 0)
#అయితే_ముగింపు
    {
	wl_display_cancel_read(self->display.proxy);
	తిరిగి -1;
    }

    // Read events into the queue
    ఒకవేళ (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	తిరిగి -1;

    // Dispatch those events (call the handlers associated for each event)
    తిరిగి wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    స్థిర శూన్య
vwl_callback_event_పూర్తి(శూన్య *దత్తాంశం, నిర్మాణం wl_callback *callback,
	uint32_t callback_దత్తాంశం వాడని)
{
    *((bool*)దత్తాంశం) = true;
    wl_callback_destroy(callback);
}

స్థిర స్థిరరాశి నిర్మాణం wl_callback_listener vwl_callback_listener = {
    .పూర్తి = vwl_callback_event_పూర్తి
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    పూర్ణాంకం
vwl_connection_roundtrip(vwl_connection_T *self)
{
    నిర్మాణం wl_callback	*callback;
    పూర్ణాంకం			ret;
    bool		పూర్తి = false;
#నిర్వచితమైతే ELAPSED_FUNC
    elapsed_T		ప్రారంభం_tv;
#అయితే_ముగింపు

    ఒకవేళ (self->display.proxy == ఖాళీ)
	తిరిగి FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    ఒకవేళ (callback == ఖాళీ)
	తిరిగి FAIL;

    wl_callback_కూడు_listener(callback, &vwl_callback_listener, &పూర్తి);

#నిర్వచితమైతే ELAPSED_FUNC
    ELAPSED_INIT(ప్రారంభం_tv);
#అయితే_ముగింపు

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    అయితే (true)
    {
	ret = vwl_connection_dispatch(self);

	ఒకవేళ (పూర్తి || ret == -1)
	    ఆపు;

#నిర్వచితమైతే ELAPSED_FUNC
	ఒకవేళ (ELAPSED_FUNC(ప్రారంభం_tv) >= p_wtm)
	{
	    ret = -1;
	    ఆపు;
	}
#అయితే_ముగింపు
    }

    ఒకవేళ (ret == -1)
    {
	ఒకవేళ (!పూర్తి)
	    wl_callback_destroy(callback);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    స్థిర శూన్య
vwl_log_handler(స్థిరరాశి అక్షరం *fmt, va_list వాదనలు)
{
    // 512 bytes should be big enough
    అక్షరం	*buf	= alloc(512);
    అక్షరం	*ఉపసర్గ = _("wayland protocol error -> ");
    size_t	len	= STRLEN(ఉపసర్గ);

    ఒకవేళ (buf == ఖాళీ)
	తిరిగి;

    vim_strncpy((char_u*)buf, (char_u*)ఉపసర్గ, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, వాదనలు);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#నిర్వచితమైతే FEAT_EVAL
    ch_log(ఖాళీ, "%s", buf);
#అయితే_ముగింపు
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    స్థిర శూన్య
wl_seat_listener_event_name(
	శూన్య		*దత్తాంశం,
	నిర్మాణం wl_seat	*seat_proxy వాడని,
	స్థిరరాశి అక్షరం	*name)
{
    vwl_seat_T *seat = దత్తాంశం;

    seat->label = (అక్షరం *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    స్థిర శూన్య
wl_seat_listener_event_capabilities(
	శూన్య		*దత్తాంశం,
	నిర్మాణం wl_seat	*seat_proxy వాడని,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = దత్తాంశం;

    seat->capabilities = capabilities;
}

స్థిర స్థిరరాశి నిర్మాణం wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

స్థిర శూన్య vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    స్థిర శూన్య
wl_registry_listener_event_global(
	శూన్య		    *దత్తాంశం,
	నిర్మాణం wl_registry  *registry,
	uint32_t	    name,
	స్థిరరాశి అక్షరం	    *interface,
	uint32_t	    సంస్కరణ_2)
{
    vwl_connection_T *ct = దత్తాంశం;

    ఒకవేళ (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	నిర్మాణం wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, సంస్కరణ_2 > 5 ? 5 : సంస్కరణ_2);
	vwl_seat_T	*seat;

	ఒకవేళ (seat_proxy == ఖాళీ)
	    తిరిగి;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	ఒకవేళ (seat == ఖాళీ || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    తిరిగి;
	}

	seat->proxy = seat_proxy;
	wl_seat_కూడు_listener(seat_proxy, &wl_seat_listener, seat);

	ఒకవేళ (vwl_connection_roundtrip(ct) == FAIL || seat->label == ఖాళీ)
	{
	    vwl_seat_destroy(seat);
	    తిరిగి;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_దత్తాంశం)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
    లేకపోతే ఒకవేళ (STRCMP(interface, zwlr_దత్తాంశం_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_దత్తాంశం_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_దత్తాంశం_control_manager_v1_interface,
		    సంస్కరణ_2 > 2 ? 2 : సంస్కరణ_2);

    లేకపోతే ఒకవేళ (STRCMP(interface, ext_దత్తాంశం_control_manager_v1_interface.name) == 0)
	ct->gobjects.ext_దత్తాంశం_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &ext_దత్తాంశం_control_manager_v1_interface, 1);
#అయితే_ముగింపు // FEAT_WAYLAND_CLIPBOARD

}

/*
 * Called when a global object is removed, if so, then do nothing. This is to
 * avoid a global being removed while it is in the process of being used. Let
 * the user call :wlrestore in order to reset everything. Requests to that
 * global will just be ignored on the compositor side.
 */
    స్థిర శూన్య
wl_registry_listener_event_global_remove(
	శూన్య		    *దత్తాంశం వాడని,
	నిర్మాణం wl_registry  *registry వాడని,
	uint32_t	    name వాడని)
{
}

స్థిర స్థిరరాశి నిర్మాణం wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

స్థిర శూన్య vwl_connection_destroy(vwl_connection_T *self);

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    ఒకవేళ (ct->gobjects.object != ఖాళీ) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = ఖాళీ; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != ఖాళీ)

#అయితే_ముగింపు

// Make sure to call wayland_set_display(display);
    స్థిర vwl_connection_T *
vwl_connection_new(స్థిరరాశి అక్షరం *display)
{
    vwl_connection_T	*ct;

    ఒకవేళ (wayland_no_connect)
	తిరిగి ఖాళీ;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    ఒకవేళ (mch_getenv("XDG_RUNTIME_DIR") == ఖాళీ)
	తిరిగి ఖాళీ;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    ఒకవేళ (ct == ఖాళీ)
	తిరిగి ఖాళీ;

    // Must set log handler before we connect display in order to work.
    wl_log_సమితి_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    ఒకవేళ (ct->display.proxy == ఖాళీ)
    {
	vim_free(ct);
	తిరిగి ఖాళీ;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    ఒకవేళ (ct->registry.proxy == ఖాళీ)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	తిరిగి ఖాళీ;
    }

    ga_init2(&ct->gobjects.seats, పరిమాణం(vwl_seat_T *), 1);

    wl_registry_కూడు_listener(ct->registry.proxy, &wl_registry_listener, ct);

    ఒకవేళ (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	తిరిగి ఖాళీ;
    }

    తిరిగి ct;
}

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    స్థిర శూన్య
vwl_seat_destroy(vwl_seat_T *self)
{
    ఒకవేళ (self == ఖాళీ)
	తిరిగి;
    ఒకవేళ (self->proxy != ఖాళీ)
    {
	ఒకవేళ (wl_seat_get_సంస్కరణ(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	లేకపోతే
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    స్థిర శూన్య
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_దత్తాంశం_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_దత్తాంశం_control_manager_v1)
# endif

    కొరకు (పూర్ణాంకం i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_దత్తాంశం)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    ఒకవేళ (self->registry.proxy != ఖాళీ)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = ఖాళీ;
    }
    ఒకవేళ (self->display.proxy != ఖాళీ)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = ఖాళీ;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, స్థిరరాశి అక్షరం *label)
{
    ఒకవేళ ((STRCMP(label, "") == 0 || label == ఖాళీ)
	    && self->gobjects.seats.ga_len > 0)
	తిరిగి ((vwl_seat_T **)self->gobjects.seats.ga_దత్తాంశం)[0];

    కొరకు (పూర్ణాంకం i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_దత్తాంశం)[i];
	ఒకవేళ (STRCMP(seat->label, label) == 0)
	    తిరిగి seat;
    }
    తిరిగి ఖాళీ;
}

#అయితే_ముగింపు

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    స్థిర శూన్య
wayland_సమితి_display(స్థిరరాశి అక్షరం *display)
{
    ఒకవేళ (display == ఖాళీ)
	display = (అక్షరం*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    లేకపోతే ఒకవేళ (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	వెళ్ళు నిష్క్రమించు;

    ఒకవేళ (display == ఖాళీ)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    ఒకవేళ (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (అక్షరం*)vim_strsave((char_u*)display);
    }

నిష్క్రమించు:
#నిర్వచితమైతే FEAT_EVAL
    సమితి_vim_var_string(VV_WAYLAND_DISPLAY, (char_u*)display, -1);
#అయితే_ముగింపు
}

/*
 * Initializes the global Wayland connection. Connects to the Wayland display
 * with given name and binds to global objects as needed. If display is NULL
 * then the $WAYLAND_DISPLAY environment variable will be used (handled by
 * libwayland). Returns FAIL on failure and OK on
 * success
 */
    పూర్ణాంకం
wayland_init_connection(స్థిరరాశి అక్షరం *display)
{
    wayland_సమితి_display(display);

    wayland_ct = vwl_connection_new(display);

    ఒకవేళ (wayland_ct == ఖాళీ)
	వెళ్ళు fail;

    తిరిగి OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_సమితి_display("");
    తిరిగి FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    శూన్య
wayland_uninit_connection(శూన్య)
{
    ఒకవేళ (wayland_ct == ఖాళీ)
	తిరిగి;
#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#అయితే_ముగింపు
    vwl_connection_destroy(wayland_ct);
    wayland_ct = ఖాళీ;
    is_reading = false;
    wayland_సమితి_display("");
}

స్థిర పూర్ణాంకం wayland_ct_restore_లెక్క = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    స్థిర శూన్య
wayland_restore_connection(శూన్య)
{
    // No point in restoring the connection if we are exiting or dying.
    ఒకవేళ (exiting || v_dying || wayland_ct_restore_లెక్క <= 0)
	wayland_సమితి_display("");

    --wayland_ct_restore_లెక్క;
    wayland_uninit_connection();

    ఒకవేళ (wayland_init_connection(wayland_display_name) == OK)
    {
#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#అయితే_ముగింపు
    }
}

/*
 * Should be called before polling (select or poll) the global Wayland
 * connection display fd. Returns fd on success and -1 on failure.
 */
    పూర్ణాంకం
wayland_prepare_read(శూన్య)
{
    ఒకవేళ (wayland_ct == ఖాళీ)
	తిరిగి -1;

    ఒకవేళ (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    అయితే (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	ఒకవేళ (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    తిరిగి -1;

    ఒకవేళ (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	తిరిగి -1;
    }

    is_reading = true;

    తిరిగి wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    పూర్ణాంకం
wayland_update(శూన్య)
{
    ఒకవేళ (wayland_ct == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి vwl_connection_roundtrip(wayland_ct);
}

#ఒకవేళ !defined(HAVE_SELECT)

    శూన్య
wayland_poll_check(పూర్ణాంకం revents)
{
    ఒకవేళ (wayland_ct == ఖాళీ)
	తిరిగి;

    is_reading = false;
    ఒకవేళ (revents & POLLIN)
    {
	ఒకవేళ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    తిరిగి;
	}
    }
    లేకపోతే ఒకవేళ (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    లేకపోతే
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	తిరిగి;
    }
    wayland_restore_connection();
}

#అయితే_ముగింపు
#ఒకవేళ defined(HAVE_SELECT)

    శూన్య
wayland_select_check(bool is_సమితి)
{
    ఒకవేళ (wayland_ct == ఖాళీ)
	తిరిగి;

    is_reading = false;
    ఒకవేళ (is_సమితి)
    {
	ఒకవేళ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	లేకపోతే
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    లేకపోతే
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#అయితే_ముగింపు

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    శూన్య
ex_wlrestore(exarg_T *eap)
{
    అక్షరం *display;

    ఒకవేళ (eap->arg == ఖాళీ || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    లేకపోతే
	display = (అక్షరం*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    ఒకవేళ (!eap->forceit && wayland_ct != ఖాళీ &&
	    (display == wayland_display_name ||
	     (wayland_display_name != ఖాళీ &&
	      STRCMP(wayland_display_name, display) == 0)))
	తిరిగి;

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
    ఒకవేళ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	ఒకవేళ (clip_నక్షత్రం.owned)
	    clip_lose_selection(&clip_నక్షత్రం);
	ఒకవేళ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#అయితే_ముగింపు

    ఒకవేళ (display != ఖాళీ)
	display = (అక్షరం*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_లెక్క = 5;

    ఒకవేళ (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#అయితే_ముగింపు
    }
    లేకపోతే
	సందేశం(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    ఎంచుకో_clipmethod();
}

#ఒకవేళ defined(FEAT_WAYLAND_CLIPBOARD)

/*
 * Get a suitable data device manager from connection. "supported" should be
 * initialized to VWL_DATA_PROTOCOL_NONE beforehand. Returns NULL if there are
 * no data device manager available with the required selection.
 */
    vwl_దత్తాంశం_device_manager_T *
vwl_connection_get_దత్తాంశం_device_manager(
	vwl_connection_T *self,
	wayland_selection_T req_sel,
	int_u *supported)
{
    vwl_దత్తాంశం_device_manager_T *manager =
	ALLOC_CLEAR_ONE(vwl_దత్తాంశం_device_manager_T);

    // Prioritize ext-data-control-v1 over wlr-data-control-unstable-v1 because
    // it is newer.
    ఒకవేళ (self->gobjects.ext_దత్తాంశం_control_manager_v1 != ఖాళీ)
    {
	manager->proxy = self->gobjects.ext_దత్తాంశం_control_manager_v1;
	manager->protocol = VWL_దత్తాంశం_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    లేకపోతే ఒకవేళ (self->gobjects.zwlr_దత్తాంశం_control_manager_v1 != ఖాళీ)
    {
	manager->proxy = self->gobjects.zwlr_దత్తాంశం_control_manager_v1;
	manager->protocol = VWL_దత్తాంశం_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	ఒకవేళ (zwlr_దత్తాంశం_control_manager_v1_get_సంస్కరణ(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    ఒకవేళ (!(*supported & req_sel))
    {
	vim_free(manager);
	తిరిగి ఖాళీ;
    }

    తిరిగి manager;
}

    vwl_దత్తాంశం_device_T *
vwl_దత్తాంశం_device_manager_get_దత్తాంశం_device(
	vwl_దత్తాంశం_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_దత్తాంశం_device_T *device = ALLOC_CLEAR_ONE(vwl_దత్తాంశం_device_T);

    ఎంపిక (self->protocol)
    {
	స్థితి VWL_దత్తాంశం_PROTOCOL_EXT:
	    device->proxy = ext_దత్తాంశం_control_manager_v1_get_దత్తాంశం_device(
		    self->proxy, seat->proxy);
	    ఆపు;
	స్థితి VWL_దత్తాంశం_PROTOCOL_WLR:
	    device->proxy = zwlr_దత్తాంశం_control_manager_v1_get_దత్తాంశం_device(
		    self->proxy, seat->proxy);
	    ఆపు;
	అప్రమేయ:
	    vim_free(device);
	    తిరిగి ఖాళీ;
    }
    device->protocol = self->protocol;

    తిరిగి device;
}

    vwl_దత్తాంశం_ఆధారం_T *
vwl_దత్తాంశం_device_manager_create_దత్తాంశం_ఆధారం(vwl_దత్తాంశం_device_manager_T *self)
{
    vwl_దత్తాంశం_ఆధారం_T *ఆధారం = ALLOC_CLEAR_ONE(vwl_దత్తాంశం_ఆధారం_T);

    ఎంపిక (self->protocol)
    {
	స్థితి VWL_దత్తాంశం_PROTOCOL_EXT:
	    ఆధారం->proxy = ext_దత్తాంశం_control_manager_v1_create_దత్తాంశం_ఆధారం(
		    self->proxy);
	    ఆపు;
	స్థితి VWL_దత్తాంశం_PROTOCOL_WLR:
	    ఆధారం->proxy = zwlr_దత్తాంశం_control_manager_v1_create_దత్తాంశం_ఆధారం(
		    self->proxy);
	    ఆపు;
	అప్రమేయ:
	    vim_free(ఆధారం);
	    తిరిగి ఖాళీ;
    }
    ఆధారం->protocol = self->protocol;

    తిరిగి ఆధారం;
}

    స్థిర vwl_దత్తాంశం_offer_T *
vwl_దత్తాంశం_device_wrap_offer_proxy(vwl_దత్తాంశం_device_T *self, శూన్య *proxy)
{
    vwl_దత్తాంశం_offer_T *offer = ALLOC_CLEAR_ONE(vwl_దత్తాంశం_offer_T);

    ఒకవేళ (offer == ఖాళీ)
	తిరిగి ఖాళీ;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->దత్తాంశం = self->దత్తాంశం;
    ga_init2(&offer->mime_types, పరిమాణం(అక్షరం *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    తిరిగి offer;
}

# define VWL_FUNC_దత్తాంశం_PROXY_DESTROY(type) \
	శూన్య \
    vwl_దత్తాంశం##type##_destroy(vwl_దత్తాంశం##type##_T *self) \
    { \
	ఒకవేళ (self == ఖాళీ) \
	    తిరిగి; \
	ఎంపిక (self->protocol) \
	{ \
	    స్థితి VWL_దత్తాంశం_PROTOCOL_EXT: \
		ext_దత్తాంశం_control##type##_v1_destroy(self->proxy); \
		ఆపు; \
	    స్థితి VWL_దత్తాంశం_PROTOCOL_WLR: \
		zwlr_దత్తాంశం_control##type##_v1_destroy(self->proxy); \
		ఆపు; \
	    అప్రమేయ: \
		ఆపు; \
	} \
	vim_free(self); \
    }

VWL_FUNC_దత్తాంశం_PROXY_DESTROY(device)
VWL_FUNC_దత్తాంశం_PROXY_DESTROY(ఆధారం)

    శూన్య
vwl_దత్తాంశం_offer_destroy(vwl_దత్తాంశం_offer_T *self)
{
    ఒకవేళ (self == ఖాళీ)
	తిరిగి;
    ఎంపిక (self->protocol)
    {
	స్థితి VWL_దత్తాంశం_PROTOCOL_EXT:
	    ext_దత్తాంశం_control_offer_v1_destroy(self->proxy);
	    ఆపు;
	స్థితి VWL_దత్తాంశం_PROTOCOL_WLR:
	    zwlr_దత్తాంశం_control_offer_v1_destroy(self->proxy);
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }
    ga_clear_అక్షరశ్రేణులు(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
శూన్య
vwl_దత్తాంశం_device_manager_discard(vwl_దత్తాంశం_device_manager_T *self)
{
    ఒకవేళ (self == ఖాళీ)
	తిరిగి;
    vim_free(self);
}

# define VWL_FUNC_దత్తాంశం_DEVICE_EVENT_దత్తాంశం_OFFER(device_type, offer_type) \
	స్థిర శూన్య \
    device_type##_listener_event_data_offer( \
	    శూన్య *దత్తాంశం, \
	    నిర్మాణం device_type *device వాడని, \
	    నిర్మాణం offer_type *offer) \
    { \
	vwl_దత్తాంశం_device_T *self = దత్తాంశం; \
	self->offer = vwl_దత్తాంశం_device_wrap_offer_proxy(self, offer); \
	self->listener->దత్తాంశం_offer(self->దత్తాంశం, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_దత్తాంశం_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	స్థిర శూన్య \
    device_type##_listener_event_selection( \
	    శూన్య *దత్తాంశం, \
	    నిర్మాణం device_type *device వాడని, \
	    నిర్మాణం offer_type *offer వాడని) \
    { \
	vwl_దత్తాంశం_device_T *self = దత్తాంశం; \
	self->listener->selection(self->దత్తాంశం, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = ఖాళీ; \
    } \

# define VWL_FUNC_దత్తాంశం_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	స్థిర శూన్య \
    device_type##_listener_event_primary_selection( \
	    శూన్య *దత్తాంశం, \
	    నిర్మాణం device_type *device వాడని, \
	    నిర్మాణం offer_type *offer వాడని) \
    { \
	vwl_దత్తాంశం_device_T *self = దత్తాంశం; \
	self->listener->selection(self->దత్తాంశం, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = ఖాళీ; \
    }

# define VWL_FUNC_దత్తాంశం_DEVICE_EVENT_ముగిసిన(device_type) \
	స్థిర శూన్య \
    device_type##_listener_event_finished( \
	    శూన్య *దత్తాంశం, \
	    నిర్మాణం device_type *device వాడని) \
    { \
	vwl_దత్తాంశం_device_T *self = దత్తాంశం; \
	self->listener->ముగిసిన(self->దత్తాంశం, self); \
    }

VWL_FUNC_దత్తాంశం_DEVICE_EVENT_దత్తాంశం_OFFER(
    ext_దత్తాంశం_control_device_v1, ext_దత్తాంశం_control_offer_v1)
VWL_FUNC_దత్తాంశం_DEVICE_EVENT_దత్తాంశం_OFFER(
    zwlr_దత్తాంశం_control_device_v1, zwlr_దత్తాంశం_control_offer_v1)

VWL_FUNC_దత్తాంశం_DEVICE_EVENT_SELECTION(
    ext_దత్తాంశం_control_device_v1, ext_దత్తాంశం_control_offer_v1
)
VWL_FUNC_దత్తాంశం_DEVICE_EVENT_SELECTION(
    zwlr_దత్తాంశం_control_device_v1, zwlr_దత్తాంశం_control_offer_v1
)
VWL_FUNC_దత్తాంశం_DEVICE_EVENT_PRIMARY_SELECTION(
    ext_దత్తాంశం_control_device_v1, ext_దత్తాంశం_control_offer_v1
)
VWL_FUNC_దత్తాంశం_DEVICE_EVENT_PRIMARY_SELECTION(
    zwlr_దత్తాంశం_control_device_v1, zwlr_దత్తాంశం_control_offer_v1
)

VWL_FUNC_దత్తాంశం_DEVICE_EVENT_ముగిసిన(ext_దత్తాంశం_control_device_v1)
VWL_FUNC_దత్తాంశం_DEVICE_EVENT_ముగిసిన(zwlr_దత్తాంశం_control_device_v1)

స్థిర నిర్మాణం ext_దత్తాంశం_control_device_v1_listener
    ext_దత్తాంశం_control_device_v1_listener = {
	.దత్తాంశం_offer = ext_దత్తాంశం_control_device_v1_listener_event_దత్తాంశం_offer,
	.selection = ext_దత్తాంశం_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_దత్తాంశం_control_device_v1_listener_event_primary_selection,
	.ముగిసిన = ext_దత్తాంశం_control_device_v1_listener_event_ముగిసిన
};
స్థిర స్థిరరాశి నిర్మాణం zwlr_దత్తాంశం_control_device_v1_listener
    zwlr_దత్తాంశం_control_device_v1_listener = {
	.దత్తాంశం_offer = zwlr_దత్తాంశం_control_device_v1_listener_event_దత్తాంశం_offer,
	.selection = zwlr_దత్తాంశం_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_దత్తాంశం_control_device_v1_listener_event_primary_selection,
	.ముగిసిన = zwlr_దత్తాంశం_control_device_v1_listener_event_ముగిసిన
};

# define VWL_FUNC_దత్తాంశం_ఆధారం_EVENT_SEND(ఆధారం_type) \
	స్థిర శూన్య \
    ఆధారం_type##_listener_event_send( \
	    శూన్య *దత్తాంశం, నిర్మాణం ఆధారం_type *ఆధారం వాడని, \
	    స్థిరరాశి అక్షరం *mime_type, పూర్ణాంకం fd) \
    { \
	vwl_దత్తాంశం_ఆధారం_T *self = దత్తాంశం; \
	self->listener->send(self->దత్తాంశం, self, mime_type, fd); \
    }

# define VWL_FUNC_దత్తాంశం_ఆధారం_EVENT_CANCELLED(ఆధారం_type) \
	స్థిర శూన్య \
    ఆధారం_type##_listener_event_cancelled( \
	    శూన్య *దత్తాంశం, నిర్మాణం ఆధారం_type *ఆధారం వాడని) \
    { \
	vwl_దత్తాంశం_ఆధారం_T *self = దత్తాంశం; \
	self->listener->cancelled(self->దత్తాంశం, self); \
    }

VWL_FUNC_దత్తాంశం_ఆధారం_EVENT_SEND(ext_దత్తాంశం_control_ఆధారం_v1)
VWL_FUNC_దత్తాంశం_ఆధారం_EVENT_SEND(zwlr_దత్తాంశం_control_ఆధారం_v1)

VWL_FUNC_దత్తాంశం_ఆధారం_EVENT_CANCELLED(ext_దత్తాంశం_control_ఆధారం_v1)
VWL_FUNC_దత్తాంశం_ఆధారం_EVENT_CANCELLED(zwlr_దత్తాంశం_control_ఆధారం_v1)

స్థిర స్థిరరాశి నిర్మాణం ext_దత్తాంశం_control_ఆధారం_v1_listener
    ext_దత్తాంశం_control_ఆధారం_v1_listener = {
	.send = ext_దత్తాంశం_control_ఆధారం_v1_listener_event_send,
	.cancelled = ext_దత్తాంశం_control_ఆధారం_v1_listener_event_cancelled
};
స్థిర స్థిరరాశి నిర్మాణం zwlr_దత్తాంశం_control_ఆధారం_v1_listener
    zwlr_దత్తాంశం_control_ఆధారం_v1_listener = {
	.send = zwlr_దత్తాంశం_control_ఆధారం_v1_listener_event_send,
	.cancelled = zwlr_దత్తాంశం_control_ఆధారం_v1_listener_event_cancelled
};

# define VWL_FUNC_దత్తాంశం_OFFER_EVENT_OFFER(offer_type) \
	స్థిర శూన్య \
    offer_type##_listener_event_offer( \
	    శూన్య *దత్తాంశం, \
	    నిర్మాణం offer_type *offer వాడని, \
	    స్థిరరాశి అక్షరం *mime_type) \
    { \
	vwl_దత్తాంశం_offer_T *self = దత్తాంశం; \
	ఒకవేళ (self->listener->offer(self->దత్తాంశం, self, mime_type)) \
	{ \
	    అక్షరం *mime = (అక్షరం *)vim_strsave((char_u *)mime_type); \
	    ఒకవేళ (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    లేకపోతే \
		ఒకవేళ (mime != ఖాళీ) \
		    ((అక్షరం **)self->mime_types.ga_దత్తాంశం) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_దత్తాంశం_OFFER_EVENT_OFFER(ext_దత్తాంశం_control_offer_v1)
VWL_FUNC_దత్తాంశం_OFFER_EVENT_OFFER(zwlr_దత్తాంశం_control_offer_v1)

స్థిర స్థిరరాశి నిర్మాణం ext_దత్తాంశం_control_offer_v1_listener
    ext_దత్తాంశం_control_offer_v1_listener = {
	.offer = ext_దత్తాంశం_control_offer_v1_listener_event_offer
};
స్థిర స్థిరరాశి నిర్మాణం zwlr_దత్తాంశం_control_offer_v1_listener
    zwlr_దత్తాంశం_control_offer_v1_listener = {
	.offer = zwlr_దత్తాంశం_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_దత్తాంశం_PROXY_కూడు_LISTENER(type) \
	శూన్య \
    vwl_దత్తాంశం##type##_add_listener( \
	    vwl_దత్తాంశం##type##_T *self, \
	    స్థిరరాశి vwl_దత్తాంశం##type##_listener_T *listener, \
	    శూన్య *దత్తాంశం) \
    { \
	ఒకవేళ (self == ఖాళీ) \
	    తిరిగి; \
	self->దత్తాంశం = దత్తాంశం; \
	self->listener = listener; \
	ఎంపిక (self->protocol) \
	{ \
	    స్థితి VWL_దత్తాంశం_PROTOCOL_EXT: \
		ext_దత్తాంశం_control##type##_v1_add_listener(self->proxy, \
			&ext_దత్తాంశం_control##type##_v1_listener, self); \
		ఆపు; \
	    స్థితి VWL_దత్తాంశం_PROTOCOL_WLR: \
		zwlr_దత్తాంశం_control##type##_v1_add_listener(self->proxy, \
			&zwlr_దత్తాంశం_control##type##_v1_listener, self); \
		ఆపు; \
	    అప్రమేయ: \
		ఆపు; \
	} \
    }

VWL_FUNC_దత్తాంశం_PROXY_కూడు_LISTENER(device)
VWL_FUNC_దత్తాంశం_PROXY_కూడు_LISTENER(ఆధారం)
VWL_FUNC_దత్తాంశం_PROXY_కూడు_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    శూన్య
vwl_దత్తాంశం_device_సమితి_selection(
    vwl_దత్తాంశం_device_T *self,
    vwl_దత్తాంశం_ఆధారం_T *ఆధారం,
    uint32_t serial వాడని,
    wayland_selection_T selection
)
{
    శూన్య *proxy = ఆధారం == ఖాళీ ? ఖాళీ : ఆధారం->proxy;

    ఒకవేళ (selection == WAYLAND_SELECTION_REGULAR)
    {
	ఎంపిక (self->protocol)
	{
	    స్థితి VWL_దత్తాంశం_PROTOCOL_EXT:
		ext_దత్తాంశం_control_device_v1_సమితి_selection(self->proxy, proxy);
		ఆపు;
	    స్థితి VWL_దత్తాంశం_PROTOCOL_WLR:
		zwlr_దత్తాంశం_control_device_v1_సమితి_selection(self->proxy, proxy);
		ఆపు;
	    అప్రమేయ:
		ఆపు;
	}
    }
    లేకపోతే ఒకవేళ (selection == WAYLAND_SELECTION_PRIMARY)
    {
	ఎంపిక (self->protocol)
	{
	    స్థితి VWL_దత్తాంశం_PROTOCOL_EXT:
		ext_దత్తాంశం_control_device_v1_సమితి_primary_selection(
			self->proxy, proxy
			);
		ఆపు;
	    స్థితి VWL_దత్తాంశం_PROTOCOL_WLR:
		zwlr_దత్తాంశం_control_device_v1_సమితి_primary_selection(
			self->proxy, proxy
			);
		ఆపు;
	    అప్రమేయ:
		ఆపు;
	}
    }
}

    శూన్య
vwl_దత్తాంశం_ఆధారం_offer(vwl_దత్తాంశం_ఆధారం_T *self, స్థిరరాశి అక్షరం *mime_type)
{
    ఎంపిక (self->protocol)
    {
	స్థితి VWL_దత్తాంశం_PROTOCOL_EXT:
	    ext_దత్తాంశం_control_ఆధారం_v1_offer(self->proxy, mime_type);
	    ఆపు;
	స్థితి VWL_దత్తాంశం_PROTOCOL_WLR:
	    zwlr_దత్తాంశం_control_ఆధారం_v1_offer(self->proxy, mime_type);
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }
}

    శూన్య
vwl_దత్తాంశం_offer_receive(
	vwl_దత్తాంశం_offer_T *self,
	స్థిరరాశి అక్షరం *mime_type,
	int32_t fd)
{
    ఎంపిక (self->protocol)
    {
	స్థితి VWL_దత్తాంశం_PROTOCOL_EXT:
	    ext_దత్తాంశం_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ఆపు;
	స్థితి VWL_దత్తాంశం_PROTOCOL_WLR:
	    zwlr_దత్తాంశం_control_offer_v1_receive(self->proxy, mime_type, fd);
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }
}

#అయితే_ముగింపు // FEAT_WAYLAND_CLIPBOARD

#అయితే_ముగింపు // FEAT_WAYLAND
