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

#شامل <wayland-client.h>

#ifdef FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#endif

#ifdef FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
تعریفنوع شمارش {
    VWL_DATA_PROTOCOL_NONE,
    VWL_DATA_PROTOCOL_EXT,
    VWL_DATA_PROTOCOL_WLR,
} vwl_data_protocol_T;

#endif // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
ساختار vwl_seat_S {
    ساختار wl_seat  *proxy;
    نویسه	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
ساختار vwl_connection_S {
    ساختار {
	ساختار wl_display   *proxy;
	صحیح		    fd;	// File descriptor for display
    } display;

    ساختار {
	ساختار wl_registry *proxy;
    } registry;

    // Global objects
    ساختار {
	garray_T seats;

#ifdef FEAT_WAYLAND_CLIPBOARD
	ساختار zwlr_data_control_manager_v1 *zwlr_data_control_manager_v1;
	ساختار ext_data_control_manager_v1  *ext_data_control_manager_v1;
#endif
    } gobjects;
};

#ifdef FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

ساختار vwl_data_device_listener_S {
    تهی (*data_offer)(تهی *data,
		       vwl_data_device_T *device,
		       vwl_data_offer_T *offer);
    تهی (*selection)(تهی *data,
		      vwl_data_device_T *device,
		      vwl_data_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    تهی (*finished)(تهی *data, vwl_data_device_T *device);
};

ساختار vwl_data_source_listener_S {
    تهی (*send)(تهی *data,
		 vwl_data_source_T *source,
		 ثابت نویسه *mime_type,
		 صحیح fd);
    تهی (*cancelled)(تهی *data, vwl_data_source_T *source);
};

ساختار vwl_data_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(تهی *data, vwl_data_offer_T *offer, ثابت نویسه *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

ساختار vwl_data_offer_S {
    تهی			*proxy;
    تهی			*data;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    ثابت vwl_data_offer_listener_T *listener;
    vwl_data_protocol_T		    protocol;
};

ساختار vwl_data_source_S {
    تهی				*proxy;
    تهی				*data;
    ثابت vwl_data_source_listener_T	*listener;
    vwl_data_protocol_T	    protocol;
};

ساختار vwl_data_device_S {
    تهی				*proxy;
    تهی				*data;
    vwl_data_offer_T			*offer;
    ثابت vwl_data_device_listener_T	*listener;
    vwl_data_protocol_T			protocol;
};

ساختار vwl_data_device_manager_S {
    تهی		*proxy;
    vwl_data_protocol_T protocol;
};

#endif // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
بیرونی vwl_connection_T *wayland_ct;

#endif // FEAT_WAYLAND
