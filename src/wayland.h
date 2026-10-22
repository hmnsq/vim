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

#அந்தர்பாவ <wayland-client.h>

#ifdef FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#endif

#ifdef FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
வகைவரை எண்ணிக்கை {
    VWL_DATA_PROTOCOL_NONE,
    VWL_DATA_PROTOCOL_EXT,
    VWL_DATA_PROTOCOL_WLR,
} vwl_data_protocol_T;

#endif // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
கட்டமைப்பு vwl_seat_S {
    கட்டமைப்பு wl_seat  *proxy;
    எழுத்து	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
கட்டமைப்பு vwl_connection_S {
    கட்டமைப்பு {
	கட்டமைப்பு wl_display   *proxy;
	முழு		    fd;	// File descriptor for display
    } display;

    கட்டமைப்பு {
	கட்டமைப்பு wl_registry *proxy;
    } registry;

    // Global objects
    கட்டமைப்பு {
	garray_T seats;

#ifdef FEAT_WAYLAND_CLIPBOARD
	கட்டமைப்பு zwlr_data_control_manager_v1 *zwlr_data_control_manager_v1;
	கட்டமைப்பு ext_data_control_manager_v1  *ext_data_control_manager_v1;
#endif
    } gobjects;
};

#ifdef FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

கட்டமைப்பு vwl_data_device_listener_S {
    வெறுமை (*data_offer)(வெறுமை *data,
		       vwl_data_device_T *device,
		       vwl_data_offer_T *offer);
    வெறுமை (*selection)(வெறுமை *data,
		      vwl_data_device_T *device,
		      vwl_data_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    வெறுமை (*finished)(வெறுமை *data, vwl_data_device_T *device);
};

கட்டமைப்பு vwl_data_source_listener_S {
    வெறுமை (*send)(வெறுமை *data,
		 vwl_data_source_T *source,
		 மாறிலி எழுத்து *mime_type,
		 முழு fd);
    வெறுமை (*cancelled)(வெறுமை *data, vwl_data_source_T *source);
};

கட்டமைப்பு vwl_data_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(வெறுமை *data, vwl_data_offer_T *offer, மாறிலி எழுத்து *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

கட்டமைப்பு vwl_data_offer_S {
    வெறுமை			*proxy;
    வெறுமை			*data;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    மாறிலி vwl_data_offer_listener_T *listener;
    vwl_data_protocol_T		    protocol;
};

கட்டமைப்பு vwl_data_source_S {
    வெறுமை				*proxy;
    வெறுமை				*data;
    மாறிலி vwl_data_source_listener_T	*listener;
    vwl_data_protocol_T	    protocol;
};

கட்டமைப்பு vwl_data_device_S {
    வெறுமை				*proxy;
    வெறுமை				*data;
    vwl_data_offer_T			*offer;
    மாறிலி vwl_data_device_listener_T	*listener;
    vwl_data_protocol_T			protocol;
};

கட்டமைப்பு vwl_data_device_manager_S {
    வெறுமை		*proxy;
    vwl_data_protocol_T protocol;
};

#endif // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
வெளி vwl_connection_T *wayland_ct;

#endif // FEAT_WAYLAND
