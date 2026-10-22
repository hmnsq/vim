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


#నిర్వచితమైతే FEAT_WAYLAND

#అన్తర్భావ <wayland-client.h>

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
# include "auto/wayland/wlr-data-control-unstable-v1.h"
# include "auto/wayland/ext-data-control-v1.h"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD

// Wayland protocols for accessing the selection
రకనిర్వచనం గణన {
    VWL_దత్తాంశం_PROTOCOL_NONE,
    VWL_దత్తాంశం_PROTOCOL_EXT,
    VWL_దత్తాంశం_PROTOCOL_WLR,
} vwl_దత్తాంశం_protocol_T;

#అయితే_ముగింపు // FEAT_WAYLAND_CLIPBOARD

// Struct that represents a seat. (Should be accessed via
// vwl_get_seat()).
నిర్మాణం vwl_seat_S {
    నిర్మాణం wl_seat  *proxy;
    అక్షరం	    *label;	    // Name of seat as text (e.g. seat0,
				    // seat1...).
    uint32_t	    capabilities;   // Bitmask of the capabilities of the seat
				    // (pointer, keyboard, touch).
};

// Struct wrapper for a Wayland connection
నిర్మాణం vwl_connection_S {
    నిర్మాణం {
	నిర్మాణం wl_display   *proxy;
	పూర్ణాంకం		    fd;	// File descriptor for display
    } display;

    నిర్మాణం {
	నిర్మాణం wl_registry *proxy;
    } registry;

    // Global objects
    నిర్మాణం {
	garray_T seats;

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD
	నిర్మాణం zwlr_దత్తాంశం_control_manager_v1 *zwlr_దత్తాంశం_control_manager_v1;
	నిర్మాణం ext_దత్తాంశం_control_manager_v1  *ext_దత్తాంశం_control_manager_v1;
#అయితే_ముగింపు
    } gobjects;
};

#నిర్వచితమైతే FEAT_WAYLAND_CLIPBOARD

// LISTENER WRAPPERS

నిర్మాణం vwl_దత్తాంశం_device_listener_S {
    శూన్య (*దత్తాంశం_offer)(శూన్య *దత్తాంశం,
		       vwl_దత్తాంశం_device_T *device,
		       vwl_దత్తాంశం_offer_T *offer);
    శూన్య (*selection)(శూన్య *దత్తాంశం,
		      vwl_దత్తాంశం_device_T *device,
		      vwl_దత్తాంశం_offer_T *offer,
		      wayland_selection_T selection);

    // This event is only relevant for data control protocols
    శూన్య (*ముగిసిన)(శూన్య *దత్తాంశం, vwl_దత్తాంశం_device_T *device);
};

నిర్మాణం vwl_దత్తాంశం_ఆధారం_listener_S {
    శూన్య (*send)(శూన్య *దత్తాంశం,
		 vwl_దత్తాంశం_ఆధారం_T *ఆధారం,
		 స్థిరరాశి అక్షరం *mime_type,
		 పూర్ణాంకం fd);
    శూన్య (*cancelled)(శూన్య *దత్తాంశం, vwl_దత్తాంశం_ఆధారం_T *ఆధారం);
};

నిర్మాణం vwl_దత్తాంశం_offer_listener_S {
    // Return TRUE to add mime type to internal array in data offer. Note that
    // this is not called for the special Vim mime type
    // (wayland_vim_special_mime), but offer->from_vim is set to true.
    // Additionally when the special mime type is received, any offer events
    // after are ignored.
    bool (*offer)(శూన్య *దత్తాంశం, vwl_దత్తాంశం_offer_T *offer, స్థిరరాశి అక్షరం *mime_type);
};

// DATA RELATED OBJECT WRAPPERS
// These wrap around a proxy and act as a generic container.
// The `data` member is used to pass other needed stuff around such as a
// vwl_clipboard_selection_T pointer.

నిర్మాణం vwl_దత్తాంశం_offer_S {
    శూన్య			*proxy;
    శూన్య			*దత్తాంశం;	    // Should be same as parent data
					    // device.
    garray_T			mime_types;

    స్థిరరాశి vwl_దత్తాంశం_offer_listener_T *listener;
    vwl_దత్తాంశం_protocol_T		    protocol;
};

నిర్మాణం vwl_దత్తాంశం_ఆధారం_S {
    శూన్య				*proxy;
    శూన్య				*దత్తాంశం;
    స్థిరరాశి vwl_దత్తాంశం_ఆధారం_listener_T	*listener;
    vwl_దత్తాంశం_protocol_T	    protocol;
};

నిర్మాణం vwl_దత్తాంశం_device_S {
    శూన్య				*proxy;
    శూన్య				*దత్తాంశం;
    vwl_దత్తాంశం_offer_T			*offer;
    స్థిరరాశి vwl_దత్తాంశం_device_listener_T	*listener;
    vwl_దత్తాంశం_protocol_T			protocol;
};

నిర్మాణం vwl_దత్తాంశం_device_manager_S {
    శూన్య		*proxy;
    vwl_దత్తాంశం_protocol_T protocol;
};

#అయితే_ముగింపు // FEAT_WAYLAND_CLIPBOARD

// Global Wayland connection. Is also set to NULL when the connection is lost.
బాహ్య vwl_connection_T *wayland_ct;

#అయితే_ముగింపు // FEAT_WAYLAND
