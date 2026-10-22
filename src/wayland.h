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


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND

#ਅਨ੍ਤਰ੍ਭਾਵ <wayland-client.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
    VWL_ਅੰਕੜੇ_PROTOCOL_NONE,
    VWL_ਅੰਕੜੇ_PROTOCOL_EXT,
    VWL_ਅੰਕੜੇ_PROTOCOL_WLR,
} vwl_ਅੰਕੜੇ_protocol_T;

#ਜੇ_ਅੰਤ // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
ਬਣਤਰ vwl_seat_S {
    ਬਣਤਰ wl_seat  *proxy;
    ਅੱਖਰ	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
ਬਣਤਰ vwl_connection_S {
    ਬਣਤਰ {
	ਬਣਤਰ wl_display   *proxy;
	ਪੂਰਨ		    fd;	// File descriptor for display
    } display;

    ਬਣਤਰ {
	ਬਣਤਰ wl_registry *proxy;
    } registry;

    // Global objects
    ਬਣਤਰ {
	garray_T seats;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD
	ਬਣਤਰ zwlr_ਅੰਕੜੇ_control_manager_v1 *zwlr_ਅੰਕੜੇ_control_manager_v1;
	ਬਣਤਰ ext_ਅੰਕੜੇ_control_manager_v1  *ext_ਅੰਕੜੇ_control_manager_v1;
#ਜੇ_ਅੰਤ
    } gobjects;
};

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

ਬਣਤਰ vwl_ਅੰਕੜੇ_device_listener_S {
    ਖਾਲੀ (*ਅੰਕੜੇ_offer)(ਖਾਲੀ *ਅੰਕੜੇ,
		       vwl_ਅੰਕੜੇ_device_T *device,
		       vwl_ਅੰਕੜੇ_offer_T *offer);
    ਖਾਲੀ (*selection)(ਖਾਲੀ *ਅੰਕੜੇ,
		      vwl_ਅੰਕੜੇ_device_T *device,
		      vwl_ਅੰਕੜੇ_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    ਖਾਲੀ (*ਮੁਕੰਮਲ)(ਖਾਲੀ *ਅੰਕੜੇ, vwl_ਅੰਕੜੇ_device_T *device);
};

ਬਣਤਰ vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_S {
    ਖਾਲੀ (*send)(ਖਾਲੀ *ਅੰਕੜੇ,
		 vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *ਸਰੋਤ,
		 ਅਚਲ ਅੱਖਰ *mime_type,
		 ਪੂਰਨ fd);
    ਖਾਲੀ (*cancelled)(ਖਾਲੀ *ਅੰਕੜੇ, vwl_ਅੰਕੜੇ_ਸਰੋਤ_T *ਸਰੋਤ);
};

ਬਣਤਰ vwl_ਅੰਕੜੇ_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(ਖਾਲੀ *ਅੰਕੜੇ, vwl_ਅੰਕੜੇ_offer_T *offer, ਅਚਲ ਅੱਖਰ *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

ਬਣਤਰ vwl_ਅੰਕੜੇ_offer_S {
    ਖਾਲੀ			*proxy;
    ਖਾਲੀ			*ਅੰਕੜੇ;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    ਅਚਲ vwl_ਅੰਕੜੇ_offer_listener_T *listener;
    vwl_ਅੰਕੜੇ_protocol_T		    protocol;
};

ਬਣਤਰ vwl_ਅੰਕੜੇ_ਸਰੋਤ_S {
    ਖਾਲੀ				*proxy;
    ਖਾਲੀ				*ਅੰਕੜੇ;
    ਅਚਲ vwl_ਅੰਕੜੇ_ਸਰੋਤ_listener_T	*listener;
    vwl_ਅੰਕੜੇ_protocol_T	    protocol;
};

ਬਣਤਰ vwl_ਅੰਕੜੇ_device_S {
    ਖਾਲੀ				*proxy;
    ਖਾਲੀ				*ਅੰਕੜੇ;
    vwl_ਅੰਕੜੇ_offer_T			*offer;
    ਅਚਲ vwl_ਅੰਕੜੇ_device_listener_T	*listener;
    vwl_ਅੰਕੜੇ_protocol_T			protocol;
};

ਬਣਤਰ vwl_ਅੰਕੜੇ_device_manager_S {
    ਖਾਲੀ		*proxy;
    vwl_ਅੰਕੜੇ_protocol_T protocol;
};

#ਜੇ_ਅੰਤ // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
ਬਾਹਰੀ vwl_connection_T *wayland_ct;

#ਜੇ_ਅੰਤ // FEAT_WAYLAND
