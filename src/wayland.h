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

#অন্তর্ভাব <wayland-client.h>

#ifdef FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#endif

#ifdef FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
প্রকারলেখ গণনা {
    VWL_তথ্য_PROTOCOL_NONE,
    VWL_তথ্য_PROTOCOL_EXT,
    VWL_তথ্য_PROTOCOL_WLR,
} vwl_তথ্য_protocol_T;

#endif // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
কাঠামো vwl_seat_S {
    কাঠামো wl_seat  *proxy;
    অক্ষর	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
কাঠামো vwl_connection_S {
    কাঠামো {
	কাঠামো wl_display   *proxy;
	পূর্ণসংখ্যা		    fd;	// File descriptor for display
    } display;

    কাঠামো {
	কাঠামো wl_registry *proxy;
    } registry;

    // Global objects
    কাঠামো {
	garray_T seats;

#ifdef FEAT_WAYLAND_CLIPBOARD
	কাঠামো zwlr_তথ্য_control_manager_v1 *zwlr_তথ্য_control_manager_v1;
	কাঠামো ext_তথ্য_control_manager_v1  *ext_তথ্য_control_manager_v1;
#endif
    } gobjects;
};

#ifdef FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

কাঠামো vwl_তথ্য_device_listener_S {
    শূন্য (*তথ্য_offer)(শূন্য *তথ্য,
		       vwl_তথ্য_device_T *device,
		       vwl_তথ্য_offer_T *offer);
    শূন্য (*selection)(শূন্য *তথ্য,
		      vwl_তথ্য_device_T *device,
		      vwl_তথ্য_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    শূন্য (*finished)(শূন্য *তথ্য, vwl_তথ্য_device_T *device);
};

কাঠামো vwl_তথ্য_source_listener_S {
    শূন্য (*send)(শূন্য *তথ্য,
		 vwl_তথ্য_source_T *source,
		 ধ্রুবক অক্ষর *mime_type,
		 পূর্ণসংখ্যা fd);
    শূন্য (*cancelled)(শূন্য *তথ্য, vwl_তথ্য_source_T *source);
};

কাঠামো vwl_তথ্য_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(শূন্য *তথ্য, vwl_তথ্য_offer_T *offer, ধ্রুবক অক্ষর *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

কাঠামো vwl_তথ্য_offer_S {
    শূন্য			*proxy;
    শূন্য			*তথ্য;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    ধ্রুবক vwl_তথ্য_offer_listener_T *listener;
    vwl_তথ্য_protocol_T		    protocol;
};

কাঠামো vwl_তথ্য_source_S {
    শূন্য				*proxy;
    শূন্য				*তথ্য;
    ধ্রুবক vwl_তথ্য_source_listener_T	*listener;
    vwl_তথ্য_protocol_T	    protocol;
};

কাঠামো vwl_তথ্য_device_S {
    শূন্য				*proxy;
    শূন্য				*তথ্য;
    vwl_তথ্য_offer_T			*offer;
    ধ্রুবক vwl_তথ্য_device_listener_T	*listener;
    vwl_তথ্য_protocol_T			protocol;
};

কাঠামো vwl_তথ্য_device_manager_S {
    শূন্য		*proxy;
    vwl_তথ্য_protocol_T protocol;
};

#endif // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
বাহ্যিক vwl_connection_T *wayland_ct;

#endif // FEAT_WAYLAND
