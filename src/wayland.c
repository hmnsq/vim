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

#अन्तर्भाव "vim.h"

#ifdef FEAT_WAYLAND

#अन्तर्भाव "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    पूर्णाङ्क
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    संरचना pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#अण्णहा
    fd_set	    wfds;
    संरचना timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    जइ (self->display.proxy == रिक्त)
	प्रत्यागम FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    जाव (true)
    {
	पूर्णाङ्क ret = wl_display_flush(self->display.proxy);

	जइ (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    जइ (poll(&fds, 1, p_wtm) <= 0)
#अण्णहा
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    जइ (select(self->display.fd + 1, रिक्त, &wfds, रिक्त, &tv) <= 0)
		प्रत्यागम FAIL;
#endif
	}
	अण्णहा जइ (ret == -1)
	    प्रत्यागम FAIL;
	अण्णहा
	    भङ्ग;
    }

    प्रत्यागम OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    पूर्णाङ्क
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    संरचना pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#अण्णहा
    fd_set	    rfds;
    संरचना timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    जइ (self->display.proxy == रिक्त)
	प्रत्यागम -1;

    जाव (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	जइ (wl_display_dispatch_pending(self->display.proxy) == -1)
	    प्रत्यागम -1;

    // Send any requests before we starting blocking to read display fd
    जइ (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	प्रत्यागम -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    जइ (poll(&fds, 1, p_wtm) <= 0)
#अण्णहा
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    जइ (select(self->display.fd + 1, &rfds, रिक्त, रिक्त, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	प्रत्यागम -1;
    }

    // Read events into the queue
    जइ (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	प्रत्यागम -1;

    // Dispatch those events (call the handlers associated for each event)
    प्रत्यागम wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    स्थिर शून्य
vwl_callback_event_done(शून्य *data, संरचना wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

स्थिर ध्रुव संरचना wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    पूर्णाङ्क
vwl_connection_roundtrip(vwl_connection_T *self)
{
    संरचना wl_callback	*callback;
    पूर्णाङ्क			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    जइ (self->display.proxy == रिक्त)
	प्रत्यागम FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    जइ (callback == रिक्त)
	प्रत्यागम FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    जाव (true)
    {
	ret = vwl_connection_dispatch(self);

	जइ (done || ret == -1)
	    भङ्ग;

#ifdef ELAPSED_FUNC
	जइ (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    भङ्ग;
	}
#endif
    }

    जइ (ret == -1)
    {
	जइ (!done)
	    wl_callback_destroy(callback);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    स्थिर शून्य
vwl_log_handler(ध्रुव वर्ण *fmt, va_list args)
{
    // 512 bytes should be big enough
    वर्ण	*buf	= alloc(512);
    वर्ण	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    जइ (buf == रिक्त)
	प्रत्यागम;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(रिक्त, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    स्थिर शून्य
wl_seat_listener_event_name(
	शून्य		*data,
	संरचना wl_seat	*seat_proxy UNUSED,
	ध्रुव वर्ण	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (वर्ण *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    स्थिर शून्य
wl_seat_listener_event_capabilities(
	शून्य		*data,
	संरचना wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

स्थिर ध्रुव संरचना wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

स्थिर शून्य vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    स्थिर शून्य
wl_registry_listener_event_global(
	शून्य		    *data,
	संरचना wl_registry  *registry,
	uint32_t	    name,
	ध्रुव वर्ण	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    जइ (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	संरचना wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	जइ (seat_proxy == रिक्त)
	    प्रत्यागम;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	जइ (seat == रिक्त || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    प्रत्यागम;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	जइ (vwl_connection_roundtrip(ct) == FAIL || seat->label == रिक्त)
	{
	    vwl_seat_destroy(seat);
	    प्रत्यागम;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    अण्णहा जइ (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    अण्णहा जइ (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
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
    स्थिर शून्य
wl_registry_listener_event_global_remove(
	शून्य		    *data UNUSED,
	संरचना wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

स्थिर ध्रुव संरचना wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

स्थिर शून्य vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    जइ (ct->gobjects.object != रिक्त) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = रिक्त; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != रिक्त)

#endif

// Make sure to call wayland_set_display(display);
    स्थिर vwl_connection_T *
vwl_connection_new(ध्रुव वर्ण *display)
{
    vwl_connection_T	*ct;

    जइ (wayland_no_connect)
	प्रत्यागम रिक्त;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    जइ (mch_getenv("XDG_RUNTIME_DIR") == रिक्त)
	प्रत्यागम रिक्त;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    जइ (ct == रिक्त)
	प्रत्यागम रिक्त;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    जइ (ct->display.proxy == रिक्त)
    {
	vim_free(ct);
	प्रत्यागम रिक्त;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    जइ (ct->registry.proxy == रिक्त)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	प्रत्यागम रिक्त;
    }

    ga_init2(&ct->gobjects.seats, परिमाण(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    जइ (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	प्रत्यागम रिक्त;
    }

    प्रत्यागम ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    स्थिर शून्य
vwl_seat_destroy(vwl_seat_T *self)
{
    जइ (self == रिक्त)
	प्रत्यागम;
    जइ (self->proxy != रिक्त)
    {
	जइ (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	अण्णहा
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    स्थिर शून्य
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    प्रति (पूर्णाङ्क i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    जइ (self->registry.proxy != रिक्त)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = रिक्त;
    }
    जइ (self->display.proxy != रिक्त)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = रिक्त;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, ध्रुव वर्ण *label)
{
    जइ ((STRCMP(label, "") == 0 || label == रिक्त)
	    && self->gobjects.seats.ga_len > 0)
	प्रत्यागम ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    प्रति (पूर्णाङ्क i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	जइ (STRCMP(seat->label, label) == 0)
	    प्रत्यागम seat;
    }
    प्रत्यागम रिक्त;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    स्थिर शून्य
wayland_set_display(ध्रुव वर्ण *display)
{
    जइ (display == रिक्त)
	display = (वर्ण*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    अण्णहा जइ (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	गच्छ exit;

    जइ (display == रिक्त)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    जइ (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (वर्ण*)vim_strsave((char_u*)display);
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
    पूर्णाङ्क
wayland_init_connection(ध्रुव वर्ण *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    जइ (wayland_ct == रिक्त)
	गच्छ fail;

    प्रत्यागम OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    प्रत्यागम FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    शून्य
wayland_uninit_connection(शून्य)
{
    जइ (wayland_ct == रिक्त)
	प्रत्यागम;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = रिक्त;
    is_reading = false;
    wayland_set_display("");
}

स्थिर पूर्णाङ्क wayland_ct_restore_गणनम् = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    स्थिर शून्य
wayland_restore_connection(शून्य)
{
    // No point in restoring the connection if we are exiting or dying.
    जइ (exiting || v_dying || wayland_ct_restore_गणनम् <= 0)
	wayland_set_display("");

    --wayland_ct_restore_गणनम्;
    wayland_uninit_connection();

    जइ (wayland_init_connection(wayland_display_name) == OK)
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
    पूर्णाङ्क
wayland_prepare_read(शून्य)
{
    जइ (wayland_ct == रिक्त)
	प्रत्यागम -1;

    जइ (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    जाव (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	जइ (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    प्रत्यागम -1;

    जइ (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	प्रत्यागम -1;
    }

    is_reading = true;

    प्रत्यागम wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    पूर्णाङ्क
wayland_update(शून्य)
{
    जइ (wayland_ct == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम vwl_connection_roundtrip(wayland_ct);
}

#जइ !defined(HAVE_SELECT)

    शून्य
wayland_poll_check(पूर्णाङ्क revents)
{
    जइ (wayland_ct == रिक्त)
	प्रत्यागम;

    is_reading = false;
    जइ (revents & POLLIN)
    {
	जइ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    प्रत्यागम;
	}
    }
    अण्णहा जइ (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    अण्णहा
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	प्रत्यागम;
    }
    wayland_restore_connection();
}

#endif
#जइ defined(HAVE_SELECT)

    शून्य
wayland_select_check(bool is_set)
{
    जइ (wayland_ct == रिक्त)
	प्रत्यागम;

    is_reading = false;
    जइ (is_set)
    {
	जइ (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	अण्णहा
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    अण्णहा
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    शून्य
ex_wlrestore(exarg_T *eap)
{
    वर्ण *display;

    जइ (eap->arg == रिक्त || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    अण्णहा
	display = (वर्ण*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    जइ (!eap->forceit && wayland_ct != रिक्त &&
	    (display == wayland_display_name ||
	     (wayland_display_name != रिक्त &&
	      STRCMP(wayland_display_name, display) == 0)))
	प्रत्यागम;

#ifdef FEAT_WAYLAND_CLIPBOARD
    जइ (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	जइ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	जइ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    जइ (display != रिक्त)
	display = (वर्ण*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_गणनम् = 5;

    जइ (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    अण्णहा
	सन्देशः(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#जइ defined(FEAT_WAYLAND_CLIPBOARD)

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
    जइ (self->gobjects.ext_data_control_manager_v1 != रिक्त)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    अण्णहा जइ (self->gobjects.zwlr_data_control_manager_v1 != रिक्त)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	जइ (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    जइ (!(*supported & req_sel))
    {
	vim_free(manager);
	प्रत्यागम रिक्त;
    }

    प्रत्यागम manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    विकल्प (self->protocol)
    {
	पक्ष VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    भङ्ग;
	पक्ष VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    भङ्ग;
	स्वभाव:
	    vim_free(device);
	    प्रत्यागम रिक्त;
    }
    device->protocol = self->protocol;

    प्रत्यागम device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    विकल्प (self->protocol)
    {
	पक्ष VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    भङ्ग;
	पक्ष VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    भङ्ग;
	स्वभाव:
	    vim_free(source);
	    प्रत्यागम रिक्त;
    }
    source->protocol = self->protocol;

    प्रत्यागम source;
}

    स्थिर vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, शून्य *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    जइ (offer == रिक्त)
	प्रत्यागम रिक्त;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, परिमाण(वर्ण *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    प्रत्यागम offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	शून्य \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	जइ (self == रिक्त) \
	    प्रत्यागम; \
	विकल्प (self->protocol) \
	{ \
	    पक्ष VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		भङ्ग; \
	    पक्ष VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		भङ्ग; \
	    स्वभाव: \
		भङ्ग; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    शून्य
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    जइ (self == रिक्त)
	प्रत्यागम;
    विकल्प (self->protocol)
    {
	पक्ष VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    भङ्ग;
	पक्ष VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
शून्य
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    जइ (self == रिक्त)
	प्रत्यागम;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	स्थिर शून्य \
    device_type##_listener_event_data_offer( \
	    शून्य *data, \
	    संरचना device_type *device UNUSED, \
	    संरचना offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	स्थिर शून्य \
    device_type##_listener_event_selection( \
	    शून्य *data, \
	    संरचना device_type *device UNUSED, \
	    संरचना offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = रिक्त; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	स्थिर शून्य \
    device_type##_listener_event_primary_selection( \
	    शून्य *data, \
	    संरचना device_type *device UNUSED, \
	    संरचना offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = रिक्त; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	स्थिर शून्य \
    device_type##_listener_event_finished( \
	    शून्य *data, \
	    संरचना device_type *device UNUSED) \
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

स्थिर संरचना ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
स्थिर ध्रुव संरचना zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	स्थिर शून्य \
    source_type##_listener_event_send( \
	    शून्य *data, संरचना source_type *source UNUSED, \
	    ध्रुव वर्ण *mime_type, पूर्णाङ्क fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	स्थिर शून्य \
    source_type##_listener_event_cancelled( \
	    शून्य *data, संरचना source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

स्थिर ध्रुव संरचना ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
स्थिर ध्रुव संरचना zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	स्थिर शून्य \
    offer_type##_listener_event_offer( \
	    शून्य *data, \
	    संरचना offer_type *offer UNUSED, \
	    ध्रुव वर्ण *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	जइ (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    वर्ण *mime = (वर्ण *)vim_strsave((char_u *)mime_type); \
	    जइ (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    अण्णहा \
		जइ (mime != रिक्त) \
		    ((वर्ण **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

स्थिर ध्रुव संरचना ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
स्थिर ध्रुव संरचना zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	शून्य \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    ध्रुव vwl_data_##type##_listener_T *listener, \
	    शून्य *data) \
    { \
	जइ (self == रिक्त) \
	    प्रत्यागम; \
	self->data = data; \
	self->listener = listener; \
	विकल्प (self->protocol) \
	{ \
	    पक्ष VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		भङ्ग; \
	    पक्ष VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		भङ्ग; \
	    स्वभाव: \
		भङ्ग; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    शून्य
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    शून्य *proxy = source == रिक्त ? रिक्त : source->proxy;

    जइ (selection == WAYLAND_SELECTION_REGULAR)
    {
	विकल्प (self->protocol)
	{
	    पक्ष VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		भङ्ग;
	    पक्ष VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		भङ्ग;
	    स्वभाव:
		भङ्ग;
	}
    }
    अण्णहा जइ (selection == WAYLAND_SELECTION_PRIMARY)
    {
	विकल्प (self->protocol)
	{
	    पक्ष VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		भङ्ग;
	    पक्ष VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		भङ्ग;
	    स्वभाव:
		भङ्ग;
	}
    }
}

    शून्य
vwl_data_source_offer(vwl_data_source_T *self, ध्रुव वर्ण *mime_type)
{
    विकल्प (self->protocol)
    {
	पक्ष VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    भङ्ग;
	पक्ष VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }
}

    शून्य
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	ध्रुव वर्ण *mime_type,
	int32_t fd)
{
    विकल्प (self->protocol)
    {
	पक्ष VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    भङ्ग;
	पक्ष VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
