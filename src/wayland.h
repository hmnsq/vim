/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * wayland.h: Common definitions for Wayland code
 */


#ifdef FEAT_WAYLAND

#અન્તર્ભાવ <wayland-client.h>

#ifdef FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#endif

#ifdef FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
પ્રકારવ્યાખ્યા ગણના {
    VWL_DATA_PROTOCOL_NONE,
    VWL_DATA_PROTOCOL_EXT,
    VWL_DATA_PROTOCOL_WLR,
} vwl_data_protocol_T;

#endif // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
સંરચના vwl_seat_S {
    સંરચના wl_seat  *proxy;
    અક્ષર	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
સંરચના vwl_connection_S {
    સંરચના {
	સંરચના wl_display   *proxy;
	પૂર્ણાંક		    fd;	// File descriptor for display
    } display;

    સંરચના {
	સંરચના wl_registry *proxy;
    } registry;

    // Global objects
    સંરચના {
	garray_T seats;

#ifdef FEAT_WAYLAND_CLIPBOARD
	સંરચના zwlr_data_control_manager_v1 *zwlr_data_control_manager_v1;
	સંરચના ext_data_control_manager_v1  *ext_data_control_manager_v1;
#endif
    } gobjects;
};

#ifdef FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

સંરચના vwl_data_device_listener_S {
    શૂન્ય (*data_offer)(શૂન્ય *data,
		       vwl_data_device_T *device,
		       vwl_data_offer_T *offer);
    શૂન્ય (*selection)(શૂન્ય *data,
		      vwl_data_device_T *device,
		      vwl_data_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    શૂન્ય (*finished)(શૂન્ય *data, vwl_data_device_T *device);
};

સંરચના vwl_data_source_listener_S {
    શૂન્ય (*send)(શૂન્ય *data,
		 vwl_data_source_T *source,
		 અચલ અક્ષર *mime_type,
		 પૂર્ણાંક fd);
    શૂન્ય (*cancelled)(શૂન્ય *data, vwl_data_source_T *source);
};

સંરચના vwl_data_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(શૂન્ય *data, vwl_data_offer_T *offer, અચલ અક્ષર *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

સંરચના vwl_data_offer_S {
    શૂન્ય			*proxy;
    શૂન્ય			*data;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    અચલ vwl_data_offer_listener_T *listener;
    vwl_data_protocol_T		    protocol;
};

સંરચના vwl_data_source_S {
    શૂન્ય				*proxy;
    શૂન્ય				*data;
    અચલ vwl_data_source_listener_T	*listener;
    vwl_data_protocol_T	    protocol;
};

સંરચના vwl_data_device_S {
    શૂન્ય				*proxy;
    શૂન્ય				*data;
    vwl_data_offer_T			*offer;
    અચલ vwl_data_device_listener_T	*listener;
    vwl_data_protocol_T			protocol;
};

સંરચના vwl_data_device_manager_S {
    શૂન્ય		*proxy;
    vwl_data_protocol_T protocol;
};

#endif // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
બાહ્ય vwl_connection_T *wayland_ct;

#endif // FEAT_WAYLAND
