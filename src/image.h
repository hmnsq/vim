/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_IMAGE

#ਅਨ੍ਤਰ੍ਭਾਵ <pixman.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PLACEMENT_ID_INC 1000

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    IMAGE_BACKEND_NONE = -1,
    IMAGE_BACKEND_GUI = 0,
    IMAGE_BACKEND_KITTY = 1,
    IMAGE_BACKEND_SIXEL = 2
} image_backend_T;

/*
 * Enum value is the number of bytes a pixel takes up
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    IMAGE_FORMAT_RGB = 3,
    IMAGE_FORMAT_RGBA = 4
} image_format_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    IMAGE_STATE_ਜਨਤਕ,
    IMAGE_STATE_ਨਿੱਜੀ
} image_state_T;

/*
 * Each image placement is assigned a "use" (what its used for). Uses with
 * higher values will always be on top of other image placements with a lower
 * use that have the same zindex as the higher use image.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ
{
    // See find_next_popup() for why (global popups are always drawn first,
    // meaning they should be below local tab popups).
    IMAGE_PLACEMENT_USE_POPUP_GLOBAL = 0,   // Local tab popup windows
    IMAGE_PLACEMENT_USE_POPUP_LOCAL = 1,    // Global popup windows

    IMAGE_PLACEMENT_USE_PUM = 2 // Popup menu (pum)
} image_placement_use_T;

/*
 * Represents image data, that can be referenced by multiple image placements,
 * each with a different view.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ image_S image_T;
ਬਣਤਰ image_S
{
    int_u id;
    ਪੂਰਨ refcount;

    // If image state is public, then it can be modified using the builtin
    // functions.
    image_state_T state;

    uint8_t	    *ਅੰਕੜੇ; // If NULL, then image is invalid and should not be
			   // used.
    ਪੂਰਨ		    ਚੌੜਾਈ;  // In physical pixels
    ਪੂਰਨ		    ਉਚਾਈ; // In physical pixels
    image_format_T  fmt;

    // Incremented every time the image data changes.
    int_u	    ver;

    image_backend_T backend;
    ਖਾਲੀ	    *backend_ਅੰਕੜੇ;

    image_T *ਅਗਲਾ;
    image_T *prev;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAGEPF_DIRTY 1	    // If image positioning/geometry has been modified
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAGEPF_HIDDEN 2    // If image should not be drawn
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAGEPF_FORCE 4     // If image backend should not use its cache
			    // (if any)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAGEPF_VISIBLE_INIT 8  // If "visible" is valid
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IMAGEPF_ENABLED 16	// If image should be drawn (not reset across
				// redraws).

/*
 * Represents an image placement, which is a visible view of an image.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ image_placement_S image_placement_T;
ਬਣਤਰ image_placement_S
{
    int_u   id;
    int_u   gen;    // See image_placement_new(), note that "use" is prioritized
		    // over this.
    image_T *img;   // May be NULL, if so then only "bounding_box" is relevant
		    // (and the position + zindex).

    image_backend_T backend;
    ਖਾਲੀ	    *backend_ਅੰਕੜੇ;

    // Note that positioning uses the top left of the final cropped image
    linenr_T	ਪੰਕਤੀ;
    colnr_T	ਖਾਨਾ;
    ਪੂਰਨ		zindex;

    int_u flags;

    pixman_box32_t crop_box; // In cells

    // The bounding box represents a region that images (including their
    // bounding boxes) under it with lower zindexes will have their overlapping
    // region not be rendered at all. This is used to render text (e.g. borders)
    // ontop of images.
    pixman_box32_t bounding_box; // In cells

    // Cached region that represents the parts of the image that have been drawn
    // to the screen. Used to check if image should be redrawn at all (if
    // nothing has been changed).
    pixman_region32_t	visible;	// In cells
    pixman_region32_t	visible_abs;    // In cells, uses absolute coordinates
					// (only used for blit image backends),

    int_u img_ver; // Current image version, if it is different from the image,
		   // then must redraw the image.

    ਪੂਰਨ cell_ਚੌੜਾਈ; // Cell dimensions used to draw this placement
    ਪੂਰਨ cell_ਉਚਾਈ;

    ਪੂਰਨ ਪੰਕਤੀ_off;

    image_placement_use_T use;

    image_placement_T *ਅਗਲਾ;
    image_placement_T *prev;
};

#ਨਹੀਂਤਾਂ

// Dummy structs for .pro files when GUI Vim is compiled without image support
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ dummy;
} image_T;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ dummy;
} image_placement_T;

#ਜੇ_ਅੰਤ // FEAT_IMAGE
