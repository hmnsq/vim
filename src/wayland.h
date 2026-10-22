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

#ضمن <wayland-client.h>

#ifdef FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#endif

#ifdef FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
تعريفنوع تعداد {
    VWL_DATA_PROTOCOL_NONE,
    VWL_DATA_PROTOCOL_EXT,
    VWL_DATA_PROTOCOL_WLR,
} vwl_data_protocol_T;

#endif // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
بنية vwl_seat_S {
    بنية wl_seat  *proxy;
    حرف	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
بنية vwl_connection_S {
    بنية {
	بنية wl_display   *proxy;
	صحيح		    fd;	// File descriptor for display
    } display;

    بنية {
	بنية wl_registry *proxy;
    } registry;

    // Global objects
    بنية {
	garray_T seats;

#ifdef FEAT_WAYLAND_CLIPBOARD
	بنية zwlr_data_control_manager_v1 *zwlr_data_control_manager_v1;
	بنية ext_data_control_manager_v1  *ext_data_control_manager_v1;
#endif
    } gobjects;
};

#ifdef FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

بنية vwl_data_device_listener_S {
    فراغ (*data_offer)(فراغ *data,
		       vwl_data_device_T *device,
		       vwl_data_offer_T *offer);
    فراغ (*selection)(فراغ *data,
		      vwl_data_device_T *device,
		      vwl_data_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    فراغ (*finished)(فراغ *data, vwl_data_device_T *device);
};

بنية vwl_data_source_listener_S {
    فراغ (*send)(فراغ *data,
		 vwl_data_source_T *source,
		 ثابت حرف *mime_type,
		 صحيح fd);
    فراغ (*cancelled)(فراغ *data, vwl_data_source_T *source);
};

بنية vwl_data_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(فراغ *data, vwl_data_offer_T *offer, ثابت حرف *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

بنية vwl_data_offer_S {
    فراغ			*proxy;
    فراغ			*data;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    ثابت vwl_data_offer_listener_T *listener;
    vwl_data_protocol_T		    protocol;
};

بنية vwl_data_source_S {
    فراغ				*proxy;
    فراغ				*data;
    ثابت vwl_data_source_listener_T	*listener;
    vwl_data_protocol_T	    protocol;
};

بنية vwl_data_device_S {
    فراغ				*proxy;
    فراغ				*data;
    vwl_data_offer_T			*offer;
    ثابت vwl_data_device_listener_T	*listener;
    vwl_data_protocol_T			protocol;
};

بنية vwl_data_device_manager_S {
    فراغ		*proxy;
    vwl_data_protocol_T protocol;
};

#endif // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
خارجي vwl_connection_T *wayland_ct;

#endif // FEAT_WAYLAND
