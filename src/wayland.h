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

#അന്തര്ഭാവ <wayland-client.h>

#ifdef FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#endif

#ifdef FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
തരനിർവചനം എണ്ണൽ {
    VWL_DATA_PROTOCOL_NONE,
    VWL_DATA_PROTOCOL_EXT,
    VWL_DATA_PROTOCOL_WLR,
} vwl_data_protocol_T;

#endif // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
ഘടന vwl_seat_S {
    ഘടന wl_seat  *proxy;
    അക്ഷരം	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
ഘടന vwl_connection_S {
    ഘടന {
	ഘടന wl_display   *proxy;
	പൂർണ്ണസംഖ്യ		    fd;	// File descriptor for display
    } display;

    ഘടന {
	ഘടന wl_registry *proxy;
    } registry;

    // Global objects
    ഘടന {
	garray_T seats;

#ifdef FEAT_WAYLAND_CLIPBOARD
	ഘടന zwlr_data_control_manager_v1 *zwlr_data_control_manager_v1;
	ഘടന ext_data_control_manager_v1  *ext_data_control_manager_v1;
#endif
    } gobjects;
};

#ifdef FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

ഘടന vwl_data_device_listener_S {
    ശൂന്യം (*data_offer)(ശൂന്യം *data,
		       vwl_data_device_T *device,
		       vwl_data_offer_T *offer);
    ശൂന്യം (*selection)(ശൂന്യം *data,
		      vwl_data_device_T *device,
		      vwl_data_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    ശൂന്യം (*finished)(ശൂന്യം *data, vwl_data_device_T *device);
};

ഘടന vwl_data_source_listener_S {
    ശൂന്യം (*send)(ശൂന്യം *data,
		 vwl_data_source_T *source,
		 സ്ഥിരം അക്ഷരം *mime_type,
		 പൂർണ്ണസംഖ്യ fd);
    ശൂന്യം (*cancelled)(ശൂന്യം *data, vwl_data_source_T *source);
};

ഘടന vwl_data_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(ശൂന്യം *data, vwl_data_offer_T *offer, സ്ഥിരം അക്ഷരം *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

ഘടന vwl_data_offer_S {
    ശൂന്യം			*proxy;
    ശൂന്യം			*data;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    സ്ഥിരം vwl_data_offer_listener_T *listener;
    vwl_data_protocol_T		    protocol;
};

ഘടന vwl_data_source_S {
    ശൂന്യം				*proxy;
    ശൂന്യം				*data;
    സ്ഥിരം vwl_data_source_listener_T	*listener;
    vwl_data_protocol_T	    protocol;
};

ഘടന vwl_data_device_S {
    ശൂന്യം				*proxy;
    ശൂന്യം				*data;
    vwl_data_offer_T			*offer;
    സ്ഥിരം vwl_data_device_listener_T	*listener;
    vwl_data_protocol_T			protocol;
};

ഘടന vwl_data_device_manager_S {
    ശൂന്യം		*proxy;
    vwl_data_protocol_T protocol;
};

#endif // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
ബാഹ്യ vwl_connection_T *wayland_ct;

#endif // FEAT_WAYLAND
