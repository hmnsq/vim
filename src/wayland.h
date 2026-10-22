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


#اگر_تعریف_شدہ FEAT_WAYLAND

#شامل <wayland-client.h>

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
قسمتعریف شمار {
    VWL_ڈیٹا_PROTOCOL_NONE,
    VWL_ڈیٹا_PROTOCOL_EXT,
    VWL_ڈیٹا_PROTOCOL_WLR,
} vwl_ڈیٹا_protocol_T;

#اگر_ختم // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
ساخت vwl_seat_S {
    ساخت wl_seat  *proxy;
    حرف	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
ساخت vwl_connection_S {
    ساخت {
	ساخت wl_display   *proxy;
	صحیح		    fd;	// File descriptor for display
    } display;

    ساخت {
	ساخت wl_registry *proxy;
    } registry;

    // Global objects
    ساخت {
	garray_T seats;

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD
	ساخت zwlr_ڈیٹا_control_manager_v1 *zwlr_ڈیٹا_control_manager_v1;
	ساخت ext_ڈیٹا_control_manager_v1  *ext_ڈیٹا_control_manager_v1;
#اگر_ختم
    } gobjects;
};

#اگر_تعریف_شدہ FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

ساخت vwl_ڈیٹا_device_listener_S {
    خالی (*ڈیٹا_offer)(خالی *ڈیٹا,
		       vwl_ڈیٹا_device_T *device,
		       vwl_ڈیٹا_offer_T *offer);
    خالی (*selection)(خالی *ڈیٹا,
		      vwl_ڈیٹا_device_T *device,
		      vwl_ڈیٹا_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    خالی (*ختم_شدہ)(خالی *ڈیٹا, vwl_ڈیٹا_device_T *device);
};

ساخت vwl_ڈیٹا_ماخذ_listener_S {
    خالی (*send)(خالی *ڈیٹا,
		 vwl_ڈیٹا_ماخذ_T *ماخذ,
		 ثابت حرف *mime_type,
		 صحیح fd);
    خالی (*cancelled)(خالی *ڈیٹا, vwl_ڈیٹا_ماخذ_T *ماخذ);
};

ساخت vwl_ڈیٹا_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(خالی *ڈیٹا, vwl_ڈیٹا_offer_T *offer, ثابت حرف *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

ساخت vwl_ڈیٹا_offer_S {
    خالی			*proxy;
    خالی			*ڈیٹا;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    ثابت vwl_ڈیٹا_offer_listener_T *listener;
    vwl_ڈیٹا_protocol_T		    protocol;
};

ساخت vwl_ڈیٹا_ماخذ_S {
    خالی				*proxy;
    خالی				*ڈیٹا;
    ثابت vwl_ڈیٹا_ماخذ_listener_T	*listener;
    vwl_ڈیٹا_protocol_T	    protocol;
};

ساخت vwl_ڈیٹا_device_S {
    خالی				*proxy;
    خالی				*ڈیٹا;
    vwl_ڈیٹا_offer_T			*offer;
    ثابت vwl_ڈیٹا_device_listener_T	*listener;
    vwl_ڈیٹا_protocol_T			protocol;
};

ساخت vwl_ڈیٹا_device_manager_S {
    خالی		*proxy;
    vwl_ڈیٹا_protocol_T protocol;
};

#اگر_ختم // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
خارجی vwl_connection_T *wayland_ct;

#اگر_ختم // FEAT_WAYLAND
