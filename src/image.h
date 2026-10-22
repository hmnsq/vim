/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read a list of people who contributed.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_IMAGE

#ಅನ್ತರ್ಭಾವ <pixman.h>

#ವ್ಯಾಖ್ಯಾನಿಸು PLACEMENT_ID_INC 1000

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    IMAGE_BACKEND_NONE = -1,
    IMAGE_BACKEND_GUI = 0,
    IMAGE_BACKEND_KITTY = 1,
    IMAGE_BACKEND_SIXEL = 2
} image_backend_T;

/*
 * Enum value is the number of bytes a pixel takes up
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    IMAGE_FORMAT_RGB = 3,
    IMAGE_FORMAT_RGBA = 4
} image_format_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
{
    IMAGE_STATE_ಬಹಿರಂಗ,
    IMAGE_STATE_ಖಾಸಗಿ
} image_state_T;

/*
 * Each image placement is assigned a "use" (what its used for). Uses with
 * higher values will always be on top of other image placements with a lower
 * use that have the same zindex as the higher use image.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ image_S image_T;
ರಚನೆ image_S
{
    int_u id;
    ಪೂರ್ಣಾಂಕ refcount;

    // If image state is public, then it can be modified using the builtin
    // functions.
    image_state_T state;

    uint8_t	    *ದತ್ತಾಂಶ; // If NULL, then image is invalid and should not be
			   // used.
    ಪೂರ್ಣಾಂಕ		    ಅಗಲ;  // In physical pixels
    ಪೂರ್ಣಾಂಕ		    ಎತ್ತರ; // In physical pixels
    image_format_T  fmt;

    // Incremented every time the image data changes.
    int_u	    ver;

    image_backend_T backend;
    ಶೂನ್ಯ	    *backend_ದತ್ತಾಂಶ;

    image_T *ಮುಂದಿನ;
    image_T *prev;
};

#ವ್ಯಾಖ್ಯಾನಿಸು IMAGEPF_DIRTY 1	    // If image positioning/geometry has been modified
#ವ್ಯಾಖ್ಯಾನಿಸು IMAGEPF_HIDDEN 2    // If image should not be drawn
#ವ್ಯಾಖ್ಯಾನಿಸು IMAGEPF_FORCE 4     // If image backend should not use its cache
			    // (if any)

#ವ್ಯಾಖ್ಯಾನಿಸು IMAGEPF_VISIBLE_INIT 8  // If "visible" is valid
#ವ್ಯಾಖ್ಯಾನಿಸು IMAGEPF_ENABLED 16	// If image should be drawn (not reset across
				// redraws).

/*
 * Represents an image placement, which is a visible view of an image.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ image_placement_S image_placement_T;
ರಚನೆ image_placement_S
{
    int_u   id;
    int_u   gen;    // See image_placement_new(), note that "use" is prioritized
		    // over this.
    image_T *img;   // May be NULL, if so then only "bounding_box" is relevant
		    // (and the position + zindex).

    image_backend_T backend;
    ಶೂನ್ಯ	    *backend_ದತ್ತಾಂಶ;

    // Note that positioning uses the top left of the final cropped image
    linenr_T	ಅಡ್ಡಸಾಲು;
    colnr_T	ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ		zindex;

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

    ಪೂರ್ಣಾಂಕ cell_ಅಗಲ; // Cell dimensions used to draw this placement
    ಪೂರ್ಣಾಂಕ cell_ಎತ್ತರ;

    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು_off;

    image_placement_use_T use;

    image_placement_T *ಮುಂದಿನ;
    image_placement_T *prev;
};

#ಇಲ್ಲದಿದ್ದರೆ

// Dummy structs for .pro files when GUI Vim is compiled without image support
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ dummy;
} image_T;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಪೂರ್ಣಾಂಕ dummy;
} image_placement_T;

#ಆದರೆ_ಅಂತ್ಯ // FEAT_IMAGE
