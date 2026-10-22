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

#include "vim.h"

#ifdef FEAT_WAYLAND

#include "wayland.h"

vwl_connection_T    *wayland_ct;
bool		    is_reading = false;

/*
 * Like wl_display_flush but always writes all the data in the buffer to the
 * display fd. Returns FAIL on failure and OK on success.
 */
    ཧྲིལ
vwl_connection_flush(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    sa_nracanaa pollfd fds;

    fds.fd = self->display.fd;
    fds.events = POLLOUT;
#དེ་མིན
    fd_set	    wfds;
    sa_nracanaa timeval  tv;

    FD_ZERO(&wfds);
    FD_SET(self->display.fd, &wfds);
#endif

    གལ་སྲིད (self->display.proxy == NULL)
	ལོག FAIL;

    // Send the requests we have made to the compositor, until we have written
    // all the data. Poll in order to check if the display fd is writable, if
    // not, then wait until it is and continue writing or until we timeout.
    སྐབས (true)
    {
	ཧྲིལ ret = wl_display_flush(self->display.proxy);

	གལ་སྲིད (ret == -1 && errno == EAGAIN)
	{
#ifndef HAVE_SELECT
	    གལ་སྲིད (poll(&fds, 1, p_wtm) <= 0)
#དེ་མིན
	    tv.tv_sec = p_wtm / 1000;
	    tv.tv_usec = (p_wtm % 1000) * 1000;
	    གལ་སྲིད (select(self->display.fd + 1, NULL, &wfds, NULL, &tv) <= 0)
		ལོག FAIL;
#endif
	}
	དེ་མིན གལ་སྲིད (ret == -1)
	    ལོག FAIL;
	དེ་མིན
	    bha_nga;
    }

    ལོག OK;
}

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * number of events dispatched on success else -1 on failure.
 */
    ཧྲིལ
vwl_connection_dispatch(vwl_connection_T *self)
{
#ifndef HAVE_SELECT
    sa_nracanaa pollfd   fds;

    fds.fd = self->display.fd;
    fds.events = POLLIN;
#དེ་མིན
    fd_set	    rfds;
    sa_nracanaa timeval  tv;

    FD_ZERO(&rfds);
    FD_SET(self->display.fd, &rfds);
#endif

    གལ་སྲིད (self->display.proxy == NULL)
	ལོག -1;

    སྐབས (wl_display_prepare_read(self->display.proxy) == -1)
	// Dispatch any queued events so that we can start reading
	གལ་སྲིད (wl_display_dispatch_pending(self->display.proxy) == -1)
	    ལོག -1;

    // Send any requests before we starting blocking to read display fd
    གལ་སྲིད (vwl_connection_flush(self) == FAIL)
    {
	wl_display_cancel_read(self->display.proxy);
	ལོག -1;
    }

    // Poll until there is data to read from the display fd.
#ifndef HAVE_SELECT
    གལ་སྲིད (poll(&fds, 1, p_wtm) <= 0)
#དེ་མིན
    tv.tv_sec = p_wtm / 1000;
    tv.tv_usec = (p_wtm % 1000) * 1000;
    གལ་སྲིད (select(self->display.fd + 1, &rfds, NULL, NULL, &tv) <= 0)
#endif
    {
	wl_display_cancel_read(self->display.proxy);
	ལོག -1;
    }

    // Read events into the queue
    གལ་སྲིད (wl_display_read_events(self->display.proxy) == -1)
	// No need to cancel
	ལོག -1;

    // Dispatch those events (call the handlers associated for each event)
    ལོག wl_display_dispatch_pending(self->display.proxy);
}

/*
 * Called when compositor is done processing requests/events.
 */
    static void
vwl_callback_event_done(void *data, sa_nracanaa wl_callback *callback,
	uint32_t callback_data UNUSED)
{
    *((bool*)data) = true;
    wl_callback_destroy(callback);
}

static const sa_nracanaa wl_callback_listener vwl_callback_listener = {
    .done = vwl_callback_event_done
};

/*
 * Like wl_display_roundtrip but polls the display fd with a timeout. Returns
 * FAIL on failure and OK on success.
 */
    ཧྲིལ
vwl_connection_roundtrip(vwl_connection_T *self)
{
    sa_nracanaa wl_callback	*callback;
    ཧྲིལ			ret;
    bool		done = false;
#ifdef ELAPSED_FUNC
    elapsed_T		start_tv;
#endif

    གལ་སྲིད (self->display.proxy == NULL)
	ལོག FAIL;

    // Tell compositor to emit 'done' event after processing all requests we
    // have sent and handling events.
    callback = wl_display_sync(self->display.proxy);

    གལ་སྲིད (callback == NULL)
	ལོག FAIL;

    wl_callback_add_listener(callback, &vwl_callback_listener, &done);

#ifdef ELAPSED_FUNC
    ELAPSED_INIT(start_tv);
#endif

    // Wait till we get the done event (which will set `done` to TRUE), unless
    // we timeout
    སྐབས (true)
    {
	ret = vwl_connection_dispatch(self);

	གལ་སྲིད (done || ret == -1)
	    bha_nga;

#ifdef ELAPSED_FUNC
	གལ་སྲིད (ELAPSED_FUNC(start_tv) >= p_wtm)
	{
	    ret = -1;
	    bha_nga;
	}
#endif
    }

    གལ་སྲིད (ret == -1)
    {
	གལ་སྲིད (!done)
	    wl_callback_destroy(callback);
	ལོག FAIL;
    }

    ལོག OK;
}

/*
 * Redirect libwayland logging to use ch_log + emsg instead.
 */
    static void
vwl_log_handler(const var_na *fmt, va_list args)
{
    // 512 bytes should be big enough
    var_na	*buf	= alloc(512);
    var_na	*prefix = _("wayland protocol error -> ");
    size_t	len	= STRLEN(prefix);

    གལ་སྲིད (buf == NULL)
	ལོག;

    vim_strncpy((char_u*)buf, (char_u*)prefix, len);
    vim_vsnprintf(buf + len, 512 - len, fmt, args);

    // Remove newline that libwayland puts
    buf[STRLEN(buf) - 1] = NUL;

#ifdef FEAT_EVAL
    ch_log(NULL, "%s", buf);
#endif
    emsg(buf);

    vim_free(buf);
}

/*
 * Callback for seat text label/name
 */
    static void
wl_seat_listener_event_name(
	void		*data,
	sa_nracanaa wl_seat	*seat_proxy UNUSED,
	const var_na	*name)
{
    vwl_seat_T *seat = data;

    seat->label = (var_na *)vim_strsave((char_u *)name);
}

/*
 * Callback for seat capabilities
 */
    static void
wl_seat_listener_event_capabilities(
	void		*data,
	sa_nracanaa wl_seat	*seat_proxy UNUSED,
	uint32_t	capabilities)
{
    vwl_seat_T *seat = data;

    seat->capabilities = capabilities;
}

static const sa_nracanaa wl_seat_listener wl_seat_listener = {
    .name = wl_seat_listener_event_name,
    .capabilities = wl_seat_listener_event_capabilities
};

static void vwl_seat_destroy(vwl_seat_T *self);

/*
 * Callback for global event, for each global interface the compositor supports.
 * Keep in sync with vwl_disconnect_display().
 */
    static void
wl_registry_listener_event_global(
	void		    *data,
	sa_nracanaa wl_registry  *registry,
	uint32_t	    name,
	const var_na	    *interface,
	uint32_t	    version)
{
    vwl_connection_T *ct = data;

    གལ་སྲིད (STRCMP(interface, wl_seat_interface.name) == 0)
    {
	sa_nracanaa wl_seat	*seat_proxy = wl_registry_bind(registry, name,
		&wl_seat_interface, version > 5 ? 5 : version);
	vwl_seat_T	*seat;

	གལ་སྲིད (seat_proxy == NULL)
	    ལོག;

	seat = ALLOC_CLEAR_ONE(vwl_seat_T);

	གལ་སྲིད (seat == NULL || ga_grow(&ct->gobjects.seats, 1) == FAIL)
	{
	    vwl_seat_destroy(seat);
	    ལོག;
	}

	seat->proxy = seat_proxy;
	wl_seat_add_listener(seat_proxy, &wl_seat_listener, seat);

	གལ་སྲིད (vwl_connection_roundtrip(ct) == FAIL || seat->label == NULL)
	{
	    vwl_seat_destroy(seat);
	    ལོག;
	}

	((vwl_seat_T **)ct->gobjects.seats.ga_data)[ct->gobjects.seats.ga_len++]
	    = seat;
    }
#ifdef FEAT_WAYLAND_CLIPBOARD
    དེ་མིན གལ་སྲིད (STRCMP(interface, zwlr_data_control_manager_v1_interface.name) == 0)
	ct->gobjects.zwlr_data_control_manager_v1 =
	    wl_registry_bind(registry, name,
		    &zwlr_data_control_manager_v1_interface,
		    version > 2 ? 2 : version);

    དེ་མིན གལ་སྲིད (STRCMP(interface, ext_data_control_manager_v1_interface.name) == 0)
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
    static void
wl_registry_listener_event_global_remove(
	void		    *data UNUSED,
	sa_nracanaa wl_registry  *registry UNUSED,
	uint32_t	    name UNUSED)
{
}

static const sa_nracanaa wl_registry_listener wl_registry_listener = {
    .global = wl_registry_listener_event_global,
    .global_remove = wl_registry_listener_event_global_remove
};

static void vwl_connection_destroy(vwl_connection_T *self);

#ifdef FEAT_WAYLAND_CLIPBOARD

# define VWL_DESTROY_GOBJECT(ct, object) \
    གལ་སྲིད (ct->gobjects.object != NULL) \
    { \
	object##_destroy(ct->gobjects.object); \
	ct->gobjects.object = NULL; \
    }

# define VWL_GOBJECT_AVAIL(ct, object) (ct->gobjects.object != NULL)

#endif

// Make sure to call wayland_set_display(display);
    static vwl_connection_T *
vwl_connection_new(const var_na *display)
{
    vwl_connection_T	*ct;

    གལ་སྲིད (wayland_no_connect)
	ལོག NULL;

    // We will get an error if XDG_RUNTIME_DIR is not set.
    གལ་སྲིད (mch_getenv("XDG_RUNTIME_DIR") == NULL)
	ལོག NULL;

    ct = ALLOC_CLEAR_ONE(vwl_connection_T);

    གལ་སྲིད (ct == NULL)
	ལོག NULL;

    // Must set log handler before we connect display in order to work.
    wl_log_set_handler_client(vwl_log_handler);

    ct->display.proxy = wl_display_connect(display);

    གལ་སྲིད (ct->display.proxy == NULL)
    {
	vim_free(ct);
	ལོག NULL;
    }

    ct->display.fd = wl_display_get_fd(ct->display.proxy);
    ct->registry.proxy = wl_display_get_registry(ct->display.proxy);

    གལ་སྲིད (ct->registry.proxy == NULL)
    {
	wl_display_disconnect(ct->display.proxy);
	vim_free(ct);
	ལོག NULL;
    }

    ga_init2(&ct->gobjects.seats, parimaa_na(vwl_seat_T *), 1);

    wl_registry_add_listener(ct->registry.proxy, &wl_registry_listener, ct);

    གལ་སྲིད (vwl_connection_roundtrip(ct) == FAIL)
    {
	vwl_connection_destroy(ct);
	ལོག NULL;
    }

    ལོག ct;
}

#ifdef FEAT_WAYLAND_CLIPBOARD

/*
 * Destroy/free seat.
 */
    static void
vwl_seat_destroy(vwl_seat_T *self)
{
    གལ་སྲིད (self == NULL)
	ལོག;
    གལ་སྲིད (self->proxy != NULL)
    {
	གལ་སྲིད (wl_seat_get_version(self->proxy) >= 5)
	    // Helpful for the compositor
	    wl_seat_release(self->proxy);
	དེ་མིན
	    wl_seat_destroy(self->proxy);
    }
    vim_free(self->label);
    vim_free(self);
}

/*
 * Disconnects the display and frees up all resources, including all global
 * objects.
 */
    static void
vwl_connection_destroy(vwl_connection_T *self)
{
# ifdef FEAT_WAYLAND_CLIPBOARD
    VWL_DESTROY_GOBJECT(self, ext_data_control_manager_v1)
    VWL_DESTROY_GOBJECT(self, zwlr_data_control_manager_v1)
# endif

    དོན་ལུ (ཧྲིལ i = 0; i < self->gobjects.seats.ga_len; i++)
	vwl_seat_destroy(((vwl_seat_T **)self->gobjects.seats.ga_data)[i]);
    ga_clear(&self->gobjects.seats);
    self->gobjects.seats.ga_len = 0;

    གལ་སྲིད (self->registry.proxy != NULL)
    {
	wl_registry_destroy(self->registry.proxy);
	self->registry.proxy = NULL;
    }
    གལ་སྲིད (self->display.proxy != NULL)
    {
	wl_display_disconnect(self->display.proxy);
	self->display.proxy = NULL;
    }
    vim_free(self);
}

/*
 * Return a seat with the give name/label. If none exists then NULL is returned.
 * If NULL or an empty string is passed as the label then the first available
 * seat found is used.
 */
    vwl_seat_T *
vwl_connection_get_seat(vwl_connection_T *self, const var_na *label)
{
    གལ་སྲིད ((STRCMP(label, "") == 0 || label == NULL)
	    && self->gobjects.seats.ga_len > 0)
	ལོག ((vwl_seat_T **)self->gobjects.seats.ga_data)[0];

    དོན་ལུ (ཧྲིལ i = 0; i < self->gobjects.seats.ga_len; i++)
    {
	vwl_seat_T *seat = ((vwl_seat_T **)self->gobjects.seats.ga_data)[i];
	གལ་སྲིད (STRCMP(seat->label, label) == 0)
	    ལོག seat;
    }
    ལོག NULL;
}

#endif

/*
 * Set wayland_display_name to display. Note that this allocate a copy of the
 * string, unless NULL is passed. If NULL is passed then v:wayland_display is
 * set to $WAYLAND_DISPLAY, but wayland_display_name is set to NULL.
 */
    static void
wayland_set_display(const var_na *display)
{
    གལ་སྲིད (display == NULL)
	display = (var_na*)mch_getenv((char_u*)"WAYLAND_DISPLAY");
    དེ་མིན གལ་སྲིད (display == wayland_display_name)
	// Don't want to be freeing vwl_display_strname then trying to copy it
	// after.
	goto exit;

    གལ་སྲིད (display == NULL)
	// $WAYLAND_DISPLAY is not set
	display = "";

    // Leave unchanged if display is empty (but not NULL)
    གལ་སྲིད (STRCMP(display, "") != 0)
    {
	vim_free(wayland_display_name);
	wayland_display_name = (var_na*)vim_strsave((char_u*)display);
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
    ཧྲིལ
wayland_init_connection(const var_na *display)
{
    wayland_set_display(display);

    wayland_ct = vwl_connection_new(display);

    གལ་སྲིད (wayland_ct == NULL)
	goto fail;

    ལོག OK;
fail:
    // Set v:wayland_display to empty string (but not wayland_display_name)
    wayland_set_display("");
    ལོག FAIL;
}

/*
 * Disconnect global Wayland connection and free up all resources used.
 */
    void
wayland_uninit_connection(void)
{
    གལ་སྲིད (wayland_ct == NULL)
	ལོག;
#ifdef FEAT_WAYLAND_CLIPBOARD
    clip_uninit_wayland();
#endif
    vwl_connection_destroy(wayland_ct);
    wayland_ct = NULL;
    is_reading = false;
    wayland_set_display("");
}

static ཧྲིལ wayland_ct_restore_count = 0;

/*
 * Attempts to restore the Wayland display connection.
 */
    static void
wayland_restore_connection(void)
{
    // No point in restoring the connection if we are exiting or dying.
    གལ་སྲིད (exiting || v_dying || wayland_ct_restore_count <= 0)
	wayland_set_display("");

    --wayland_ct_restore_count;
    wayland_uninit_connection();

    གལ་སྲིད (wayland_init_connection(wayland_display_name) == OK)
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
    ཧྲིལ
wayland_prepare_read(void)
{
    གལ་སྲིད (wayland_ct == NULL)
	ལོག -1;

    གལ་སྲིད (is_reading)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	is_reading = false;
    }

    སྐབས (wl_display_prepare_read(wayland_ct->display.proxy) == -1)
	// Event queue not empty, dispatch the events
	གལ་སྲིད (wl_display_dispatch_pending(wayland_ct->display.proxy) == -1)
	    ལོག -1;

    གལ་སྲིད (vwl_connection_flush(wayland_ct) < 0)
    {
	wl_display_cancel_read(wayland_ct->display.proxy);
	ལོག -1;
    }

    is_reading = true;

    ལོག wayland_ct->display.fd;
}

/*
 * Catch up on any queued events
 */
    ཧྲིལ
wayland_update(void)
{
    གལ་སྲིད (wayland_ct == NULL)
	ལོག FAIL;
    ལོག vwl_connection_roundtrip(wayland_ct);
}

#གལ་སྲིད !defined(HAVE_SELECT)

    void
wayland_poll_check(ཧྲིལ revents)
{
    གལ་སྲིད (wayland_ct == NULL)
	ལོག;

    is_reading = false;
    གལ་སྲིད (revents & POLLIN)
    {
	གལ་སྲིད (wl_display_read_events(wayland_ct->display.proxy) != -1)
	{
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	    ལོག;
	}
    }
    དེ་མིན གལ་སྲིད (revents & (POLLHUP | POLLERR))
	wl_display_cancel_read(wayland_ct->display.proxy);
    དེ་མིན
    {
	// Nothing happened
	wl_display_cancel_read(wayland_ct->display.proxy);
	ལོག;
    }
    wayland_restore_connection();
}

#endif
#གལ་སྲིད defined(HAVE_SELECT)

    void
wayland_select_check(bool is_set)
{
    གལ་སྲིད (wayland_ct == NULL)
	ལོག;

    is_reading = false;
    གལ་སྲིད (is_set)
    {
	གལ་སྲིད (wl_display_read_events(wayland_ct->display.proxy) != -1)
	    wl_display_dispatch_pending(wayland_ct->display.proxy);
	དེ་མིན
	{
	    wl_display_cancel_read(wayland_ct->display.proxy);
	    wayland_restore_connection();
	}
    }
    དེ་མིན
	wl_display_cancel_read(wayland_ct->display.proxy);
}

#endif

/*
 * Disconnect then reconnect Wayland connection, and update clipmethod.
 */
    void
ex_wlrestore(exarg_T *eap)
{
    var_na *display;

    གལ་སྲིད (eap->arg == NULL || STRLEN(eap->arg) == 0)
	// Use current display name if none given
	display = wayland_display_name;
    དེ་མིན
	display = (var_na*)eap->arg;

    // Return early if shebang is not passed, we are still connected, and if not
    // changing to a new Wayland display.
    གལ་སྲིད (!eap->forceit && wayland_ct != NULL &&
	    (display == wayland_display_name ||
	     (wayland_display_name != NULL &&
	      STRCMP(wayland_display_name, display) == 0)))
	ལོག;

#ifdef FEAT_WAYLAND_CLIPBOARD
    གལ་སྲིད (clipmethod == CLIPMETHOD_WAYLAND)
    {
	// Lose any selections we own
	གལ་སྲིད (clip_star.owned)
	    clip_lose_selection(&clip_star);
	གལ་སྲིད (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
    }
#endif

    གལ་སྲིད (display != NULL)
	display = (var_na*)vim_strsave((char_u*)display);

    // Will lose any selections we own
    wayland_uninit_connection();

    // Reset amount of available tries to reconnect the display to 5
    wayland_ct_restore_count = 5;

    གལ་སྲིད (wayland_init_connection(display) == OK)
    {
	smsg(_("restoring Wayland display %s"), wayland_display_name);

#ifdef FEAT_WAYLAND_CLIPBOARD
	clip_init_wayland();
#endif
    }
    དེ་མིན
	msg(_("failed restoring, lost connection to Wayland display"));

    vim_free(display);

    choose_clipmethod();
}

#གལ་སྲིད defined(FEAT_WAYLAND_CLIPBOARD)

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
    གལ་སྲིད (self->gobjects.ext_data_control_manager_v1 != NULL)
    {
	manager->proxy = self->gobjects.ext_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_EXT;

	*supported |= (WAYLAND_SELECTION_REGULAR | WAYLAND_SELECTION_PRIMARY);
    }
    དེ་མིན གལ་སྲིད (self->gobjects.zwlr_data_control_manager_v1 != NULL)
    {
	manager->proxy = self->gobjects.zwlr_data_control_manager_v1;
	manager->protocol = VWL_DATA_PROTOCOL_WLR;

	*supported |= WAYLAND_SELECTION_REGULAR;

	// Only version 2 or greater supports the primary selection
	གལ་སྲིད (zwlr_data_control_manager_v1_get_version(manager->proxy) >= 2)
	    *supported |= WAYLAND_SELECTION_PRIMARY;
    }

    གལ་སྲིད (!(*supported & req_sel))
    {
	vim_free(manager);
	ལོག NULL;
    }

    ལོག manager;
}

    vwl_data_device_T *
vwl_data_device_manager_get_data_device(
	vwl_data_device_manager_T *self,
	vwl_seat_T *seat)
{
    vwl_data_device_T *device = ALLOC_CLEAR_ONE(vwl_data_device_T);

    switch (self->protocol)
    {
	pak_sa VWL_DATA_PROTOCOL_EXT:
	    device->proxy = ext_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    bha_nga;
	pak_sa VWL_DATA_PROTOCOL_WLR:
	    device->proxy = zwlr_data_control_manager_v1_get_data_device(
		    self->proxy, seat->proxy);
	    bha_nga;
	default:
	    vim_free(device);
	    ལོག NULL;
    }
    device->protocol = self->protocol;

    ལོག device;
}

    vwl_data_source_T *
vwl_data_device_manager_create_data_source(vwl_data_device_manager_T *self)
{
    vwl_data_source_T *source = ALLOC_CLEAR_ONE(vwl_data_source_T);

    switch (self->protocol)
    {
	pak_sa VWL_DATA_PROTOCOL_EXT:
	    source->proxy = ext_data_control_manager_v1_create_data_source(
		    self->proxy);
	    bha_nga;
	pak_sa VWL_DATA_PROTOCOL_WLR:
	    source->proxy = zwlr_data_control_manager_v1_create_data_source(
		    self->proxy);
	    bha_nga;
	default:
	    vim_free(source);
	    ལོག NULL;
    }
    source->protocol = self->protocol;

    ལོག source;
}

    static vwl_data_offer_T *
vwl_data_device_wrap_offer_proxy(vwl_data_device_T *self, void *proxy)
{
    vwl_data_offer_T *offer = ALLOC_CLEAR_ONE(vwl_data_offer_T);

    གལ་སྲིད (offer == NULL)
	ལོག NULL;

    offer->proxy = proxy;
    offer->protocol = self->protocol;
    offer->data = self->data;
    ga_init2(&offer->mime_types, parimaa_na(var_na *), 10);

    // Try pre allocating the array, 10 mime types seems to usually be the
    // maximum from experience.
    ga_grow(&offer->mime_types, 10);

    ལོག offer;
}

# define VWL_FUNC_DATA_PROXY_DESTROY(type) \
	void \
    vwl_data_##type##_destroy(vwl_data_##type##_T *self) \
    { \
	གལ་སྲིད (self == NULL) \
	    ལོག; \
	switch (self->protocol) \
	{ \
	    pak_sa VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_destroy(self->proxy); \
		bha_nga; \
	    pak_sa VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_destroy(self->proxy); \
		bha_nga; \
	    default: \
		bha_nga; \
	} \
	vim_free(self); \
    }

VWL_FUNC_DATA_PROXY_DESTROY(device)
VWL_FUNC_DATA_PROXY_DESTROY(source)

    void
vwl_data_offer_destroy(vwl_data_offer_T *self)
{
    གལ་སྲིད (self == NULL)
	ལོག;
    switch (self->protocol)
    {
	pak_sa VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_destroy(self->proxy);
	    bha_nga;
	pak_sa VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_destroy(self->proxy);
	    bha_nga;
	default:
	    bha_nga;
    }
    ga_clear_strings(&self->mime_types);
    vim_free(self);
}

/*
 * Doesn't destroy the actual global object proxy, only frees the structure.
 */
void
vwl_data_device_manager_discard(vwl_data_device_manager_T *self)
{
    གལ་སྲིད (self == NULL)
	ལོག;
    vim_free(self);
}

# define VWL_FUNC_DATA_DEVICE_EVENT_DATA_OFFER(device_type, offer_type) \
	static void \
    device_type##_listener_event_data_offer( \
	    void *data, \
	    sa_nracanaa device_type *device UNUSED, \
	    sa_nracanaa offer_type *offer) \
    { \
	vwl_data_device_T *self = data; \
	self->offer = vwl_data_device_wrap_offer_proxy(self, offer); \
	self->listener->data_offer(self->data, self, self->offer); \
    }

// We want to set the offer to NULL after the selection callback, because the
// callback may free the offer, and we don't want a dangling pointer.
# define VWL_FUNC_DATA_DEVICE_EVENT_SELECTION(device_type, offer_type) \
	static void \
    device_type##_listener_event_selection( \
	    void *data, \
	    sa_nracanaa device_type *device UNUSED, \
	    sa_nracanaa offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_REGULAR); \
	self->offer = NULL; \
    } \

# define VWL_FUNC_DATA_DEVICE_EVENT_PRIMARY_SELECTION(device_type, offer_type) \
	static void \
    device_type##_listener_event_primary_selection( \
	    void *data, \
	    sa_nracanaa device_type *device UNUSED, \
	    sa_nracanaa offer_type *offer UNUSED) \
    { \
	vwl_data_device_T *self = data; \
	self->listener->selection(self->data, self, self->offer, \
		WAYLAND_SELECTION_PRIMARY); \
	self->offer = NULL; \
    }

# define VWL_FUNC_DATA_DEVICE_EVENT_FINISHED(device_type) \
	static void \
    device_type##_listener_event_finished( \
	    void *data, \
	    sa_nracanaa device_type *device UNUSED) \
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

static sa_nracanaa ext_data_control_device_v1_listener
    ext_data_control_device_v1_listener = {
	.data_offer = ext_data_control_device_v1_listener_event_data_offer,
	.selection = ext_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    ext_data_control_device_v1_listener_event_primary_selection,
	.finished = ext_data_control_device_v1_listener_event_finished
};
static const sa_nracanaa zwlr_data_control_device_v1_listener
    zwlr_data_control_device_v1_listener = {
	.data_offer = zwlr_data_control_device_v1_listener_event_data_offer,
	.selection = zwlr_data_control_device_v1_listener_event_selection,
	.primary_selection =
	    zwlr_data_control_device_v1_listener_event_primary_selection,
	.finished = zwlr_data_control_device_v1_listener_event_finished
};

# define VWL_FUNC_DATA_SOURCE_EVENT_SEND(source_type) \
	static void \
    source_type##_listener_event_send( \
	    void *data, sa_nracanaa source_type *source UNUSED, \
	    const var_na *mime_type, ཧྲིལ fd) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->send(self->data, self, mime_type, fd); \
    }

# define VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(source_type) \
	static void \
    source_type##_listener_event_cancelled( \
	    void *data, sa_nracanaa source_type *source UNUSED) \
    { \
	vwl_data_source_T *self = data; \
	self->listener->cancelled(self->data, self); \
    }

VWL_FUNC_DATA_SOURCE_EVENT_SEND(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_SEND(zwlr_data_control_source_v1)

VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(ext_data_control_source_v1)
VWL_FUNC_DATA_SOURCE_EVENT_CANCELLED(zwlr_data_control_source_v1)

static const sa_nracanaa ext_data_control_source_v1_listener
    ext_data_control_source_v1_listener = {
	.send = ext_data_control_source_v1_listener_event_send,
	.cancelled = ext_data_control_source_v1_listener_event_cancelled
};
static const sa_nracanaa zwlr_data_control_source_v1_listener
    zwlr_data_control_source_v1_listener = {
	.send = zwlr_data_control_source_v1_listener_event_send,
	.cancelled = zwlr_data_control_source_v1_listener_event_cancelled
};

# define VWL_FUNC_DATA_OFFER_EVENT_OFFER(offer_type) \
	static void \
    offer_type##_listener_event_offer( \
	    void *data, \
	    sa_nracanaa offer_type *offer UNUSED, \
	    const var_na *mime_type) \
    { \
	vwl_data_offer_T *self = data; \
	གལ་སྲིད (self->listener->offer(self->data, self, mime_type)) \
	{ \
	    var_na *mime = (var_na *)vim_strsave((char_u *)mime_type); \
	    གལ་སྲིད (ga_grow(&self->mime_types, 1) == FAIL) \
		vim_free(mime); \
	    དེ་མིན \
		གལ་སྲིད (mime != NULL) \
		    ((var_na **)self->mime_types.ga_data) \
					[self->mime_types.ga_len++] = mime; \
	} \
    }

VWL_FUNC_DATA_OFFER_EVENT_OFFER(ext_data_control_offer_v1)
VWL_FUNC_DATA_OFFER_EVENT_OFFER(zwlr_data_control_offer_v1)

static const sa_nracanaa ext_data_control_offer_v1_listener
    ext_data_control_offer_v1_listener = {
	.offer = ext_data_control_offer_v1_listener_event_offer
};
static const sa_nracanaa zwlr_data_control_offer_v1_listener
    zwlr_data_control_offer_v1_listener = {
	.offer = zwlr_data_control_offer_v1_listener_event_offer
};

# define VWL_FUNC_DATA_PROXY_ADD_LISTENER(type) \
	void \
    vwl_data_##type##_add_listener( \
	    vwl_data_##type##_T *self, \
	    const vwl_data_##type##_listener_T *listener, \
	    void *data) \
    { \
	གལ་སྲིད (self == NULL) \
	    ལོག; \
	self->data = data; \
	self->listener = listener; \
	switch (self->protocol) \
	{ \
	    pak_sa VWL_DATA_PROTOCOL_EXT: \
		ext_data_control_##type##_v1_add_listener(self->proxy, \
			&ext_data_control_##type##_v1_listener, self); \
		bha_nga; \
	    pak_sa VWL_DATA_PROTOCOL_WLR: \
		zwlr_data_control_##type##_v1_add_listener(self->proxy, \
			&zwlr_data_control_##type##_v1_listener, self); \
		bha_nga; \
	    default: \
		bha_nga; \
	} \
    }

VWL_FUNC_DATA_PROXY_ADD_LISTENER(device)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(source)
VWL_FUNC_DATA_PROXY_ADD_LISTENER(offer)
/*
 * Set the given selection to source. If a data control protocol is being used,
 * "serial" is ignored.
 */
    void
vwl_data_device_set_selection(
    vwl_data_device_T *self,
    vwl_data_source_T *source,
    uint32_t serial UNUSED,
    wayland_selection_T selection
)
{
    void *proxy = source == NULL ? NULL : source->proxy;

    གལ་སྲིད (selection == WAYLAND_SELECTION_REGULAR)
    {
	switch (self->protocol)
	{
	    pak_sa VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_selection(self->proxy, proxy);
		bha_nga;
	    pak_sa VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_selection(self->proxy, proxy);
		bha_nga;
	    default:
		bha_nga;
	}
    }
    དེ་མིན གལ་སྲིད (selection == WAYLAND_SELECTION_PRIMARY)
    {
	switch (self->protocol)
	{
	    pak_sa VWL_DATA_PROTOCOL_EXT:
		ext_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		bha_nga;
	    pak_sa VWL_DATA_PROTOCOL_WLR:
		zwlr_data_control_device_v1_set_primary_selection(
			self->proxy, proxy
			);
		bha_nga;
	    default:
		bha_nga;
	}
    }
}

    void
vwl_data_source_offer(vwl_data_source_T *self, const var_na *mime_type)
{
    switch (self->protocol)
    {
	pak_sa VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_source_v1_offer(self->proxy, mime_type);
	    bha_nga;
	pak_sa VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_source_v1_offer(self->proxy, mime_type);
	    bha_nga;
	default:
	    bha_nga;
    }
}

    void
vwl_data_offer_receive(
	vwl_data_offer_T *self,
	const var_na *mime_type,
	int32_t fd)
{
    switch (self->protocol)
    {
	pak_sa VWL_DATA_PROTOCOL_EXT:
	    ext_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    bha_nga;
	pak_sa VWL_DATA_PROTOCOL_WLR:
	    zwlr_data_control_offer_v1_receive(self->proxy, mime_type, fd);
	    bha_nga;
	default:
	    bha_nga;
    }
}

#endif // FEAT_WAYLAND_CLIPBOARD

#endif // FEAT_WAYLAND
