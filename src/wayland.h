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

#ଅନ୍ତର୍ଭାଵ <wayland-client.h>

#ifdef FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#endif

#ifdef FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
    VWL_DATA_PROTOCOL_NONE,
    VWL_DATA_PROTOCOL_EXT,
    VWL_DATA_PROTOCOL_WLR,
} vwl_data_protocol_T;

#endif // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
ସଂରଚନା vwl_seat_S {
    ସଂରଚନା wl_seat  *proxy;
    ଅକ୍ଷର	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
ସଂରଚନା vwl_connection_S {
    ସଂରଚନା {
	ସଂରଚନା wl_display   *proxy;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    fd;	// File descriptor for display
    } display;

    ସଂରଚନା {
	ସଂରଚନା wl_registry *proxy;
    } registry;

    // Global objects
    ସଂରଚନା {
	garray_T seats;

#ifdef FEAT_WAYLAND_CLIPBOARD
	ସଂରଚନା zwlr_data_control_manager_v1 *zwlr_data_control_manager_v1;
	ସଂରଚନା ext_data_control_manager_v1  *ext_data_control_manager_v1;
#endif
    } gobjects;
};

#ifdef FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

ସଂରଚନା vwl_data_device_listener_S {
    ଶୂନ୍ୟ (*data_offer)(ଶୂନ୍ୟ *data,
		       vwl_data_device_T *device,
		       vwl_data_offer_T *offer);
    ଶୂନ୍ୟ (*selection)(ଶୂନ୍ୟ *data,
		      vwl_data_device_T *device,
		      vwl_data_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    ଶୂନ୍ୟ (*finished)(ଶୂନ୍ୟ *data, vwl_data_device_T *device);
};

ସଂରଚନା vwl_data_source_listener_S {
    ଶୂନ୍ୟ (*send)(ଶୂନ୍ୟ *data,
		 vwl_data_source_T *source,
		 ଅଚଳ ଅକ୍ଷର *mime_type,
		 ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
    ଶୂନ୍ୟ (*cancelled)(ଶୂନ୍ୟ *data, vwl_data_source_T *source);
};

ସଂରଚନା vwl_data_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(ଶୂନ୍ୟ *data, vwl_data_offer_T *offer, ଅଚଳ ଅକ୍ଷର *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

ସଂରଚନା vwl_data_offer_S {
    ଶୂନ୍ୟ			*proxy;
    ଶୂନ୍ୟ			*data;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    ଅଚଳ vwl_data_offer_listener_T *listener;
    vwl_data_protocol_T		    protocol;
};

ସଂରଚନା vwl_data_source_S {
    ଶୂନ୍ୟ				*proxy;
    ଶୂନ୍ୟ				*data;
    ଅଚଳ vwl_data_source_listener_T	*listener;
    vwl_data_protocol_T	    protocol;
};

ସଂରଚନା vwl_data_device_S {
    ଶୂନ୍ୟ				*proxy;
    ଶୂନ୍ୟ				*data;
    vwl_data_offer_T			*offer;
    ଅଚଳ vwl_data_device_listener_T	*listener;
    vwl_data_protocol_T			protocol;
};

ସଂରଚନା vwl_data_device_manager_S {
    ଶୂନ୍ୟ		*proxy;
    vwl_data_protocol_T protocol;
};

#endif // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
ବାହ୍ୟ vwl_connection_T *wayland_ct;

#endif // FEAT_WAYLAND
