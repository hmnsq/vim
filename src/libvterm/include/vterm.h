/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __VTERM_H__
#ವ್ಯಾಖ್ಯಾನಿಸು __VTERM_H__

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
ಬಾಹ್ಯ "C" {
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#ವ್ಯಾಖ್ಯಾನಿಸು TRUE 1
#ವ್ಯಾಖ್ಯಾನಿಸು FALSE 0

// VIM: from stdint.h
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ		uint8_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ		uint16_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ		uint32_t;

// VIM: define max screen cols and rows
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_MAX_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ 1000
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_MAX_ಅಡ್ಡಸಾಲುಗಳು 1000

#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_ಆವೃತ್ತಿ_MAJOR 0
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_ಆವೃತ್ತಿ_MINOR 3
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_ಆವೃತ್ತಿ_PATCH 3

#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_CHECK_ಆವೃತ್ತಿ \
        vterm_check_ಆವೃತ್ತಿ(VTERM_ಆವೃತ್ತಿ_MAJOR, VTERM_ಆವೃತ್ತಿ_MINOR)

/* Any cell can contain at most one basic printing character and 5 combining
 * characters. This number could be changed but will be ABI-incompatible if
 * you do */
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_MAX_CHARS_ಪ್ರತಿ_CELL 6

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VTerm VTerm;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VTermState VTermState;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ VTermScreen VTermScreen;

// Specifies a screen point.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
ಪೂರ್ಣಾಂಕ vterm_pos_cmp(VTermPos a, VTermPos b);

#ಆದರೆ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಪೂರ್ಣಾಂಕ vterm_pos_cmp(VTermPos a, VTermPos b)
{
  ಹಿಂತಿರುಗಿ (a.ಅಡ್ಡಸಾಲು == b.ಅಡ್ಡಸಾಲು) ? a.ಲಂಬಸಾಲು - b.ಲಂಬಸಾಲು : a.ಅಡ್ಡಸಾಲು - b.ಅಡ್ಡಸಾಲು;
}
#ಆದರೆ_ಅಂತ್ಯ

// Specifies a rectangular screen area.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಅಡ್ಡಸಾಲು;
  ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಅಡ್ಡಸಾಲು;
  ಪೂರ್ಣಾಂಕ ಆರಂಭ_ಲಂಬಸಾಲು;
  ಪೂರ್ಣಾಂಕ ಕೊನೆ_ಲಂಬಸಾಲು;
} VTermRect;

/* true if the rect contains the point */
ಪೂರ್ಣಾಂಕ vterm_rect_ಒಳಗೊಂಡಿದೆ(VTermRect r, VTermPos p);

#ಆದರೆ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಪೂರ್ಣಾಂಕ vterm_rect_ಒಳಗೊಂಡಿದೆ(VTermRect r, VTermPos p)
{
  ಹಿಂತಿರುಗಿ p.ಅಡ್ಡಸಾಲು >= r.ಆರಂಭ_ಅಡ್ಡಸಾಲು && p.ಅಡ್ಡಸಾಲು < r.ಕೊನೆ_ಅಡ್ಡಸಾಲು &&
         p.ಲಂಬಸಾಲು >= r.ಆರಂಭ_ಲಂಬಸಾಲು && p.ಲಂಬಸಾಲು < r.ಕೊನೆ_ಲಂಬಸಾಲು;
}
#ಆದರೆ_ಅಂತ್ಯ

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
ಶೂನ್ಯ vterm_rect_move(VTermRect *rect, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು_delta, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_delta);

#ಆದರೆ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಶೂನ್ಯ vterm_rect_move(VTermRect *rect, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು_delta, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_delta)
{
  rect->ಆರಂಭ_ಅಡ್ಡಸಾಲು += ಅಡ್ಡಸಾಲು_delta; rect->ಕೊನೆ_ಅಡ್ಡಸಾಲು += ಅಡ್ಡಸಾಲು_delta;
  rect->ಆರಂಭ_ಲಂಬಸಾಲು += ಲಂಬಸಾಲು_delta; rect->ಕೊನೆ_ಲಂಬಸಾಲು += ಲಂಬಸಾಲು_delta;
}
#ಆದರೆ_ಅಂತ್ಯ

/**
 * Bit-field describing the value of VTermColor.type
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  /**
   * If the lower bit of `type` is not set, the colour is 24-bit RGB.
   */
  VTERM_COLOR_RGB = 0x00,

  /**
   * The colour is an index into a palette of 256 colours.
   */
  VTERM_COLOR_ಸೂಚ್ಯಂಕಿತ = 0x01,

  /**
   * Mask that can be used to extract the RGB/Indexed bit.
   */
  VTERM_COLOR_TYPE_ಮುಖವಾಡ = 0x01,

  /**
   * If set, indicates that this colour should be the default foreground
   * color, i.e. there was no SGR request for another colour. When
   * rendering this colour it is possible to ignore "idx" and just use a
   * colour that is not in the palette.
   */
  VTERM_COLOR_DEFAULT_FG = 0x02,

  /**
   * If set, indicates that this colour should be the default background
   * color, i.e. there was no SGR request for another colour. A common
   * option when rendering this colour is to not render a background at
   * all, for example by rendering the window transparently at this spot.
   */
  VTERM_COLOR_DEFAULT_BG = 0x04,

  /**
   * Mask that can be used to extract the default foreground/background bit.
   */
  VTERM_COLOR_DEFAULT_ಮುಖವಾಡ = 0x06,

  /**
   * VIM: If set, indicates that the color is invalid.
   */
  VTERM_COLOR_INVALID = 0x08
} VTermColorType;

/**
 * Returns true if the VTERM_COLOR_RGB `type` flag is set, indicating that the
 * given VTermColor instance is an indexed colour.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(ಲಂಬಸಾಲು) \
  (((ಲಂಬಸಾಲು)->type & VTERM_COLOR_TYPE_ಮುಖವಾಡ) == VTERM_COLOR_ಸೂಚ್ಯಂಕಿತ)

/**
 * Returns true if the VTERM_COLOR_INDEXED `type` flag is set, indicating that
 * the given VTermColor instance is an rgb colour.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_COLOR_IS_RGB(ಲಂಬಸಾಲು) \
  (((ಲಂಬಸಾಲು)->type & VTERM_COLOR_TYPE_ಮುಖವಾಡ) == VTERM_COLOR_RGB)

/**
 * Returns true if the VTERM_COLOR_DEFAULT_FG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default foreground
 * color.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_COLOR_IS_DEFAULT_FG(ಲಂಬಸಾಲು) \
  (!!((ಲಂಬಸಾಲು)->type & VTERM_COLOR_DEFAULT_FG))

/**
 * Returns true if the VTERM_COLOR_DEFAULT_BG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default background
 * color.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_COLOR_IS_DEFAULT_BG(ಲಂಬಸಾಲು) \
  (!!((ಲಂಬಸಾಲು)->type & VTERM_COLOR_DEFAULT_BG))

/**
 * Returns true if the VTERM_COLOR_INVALID `type` flag is set, indicating
 * that the given VTermColor instance is an invalid color.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು VTERM_COLOR_IS_INVALID(ಲಂಬಸಾಲು) (!!((ಲಂಬಸಾಲು)->type & VTERM_COLOR_INVALID))

// VIM: this was a union, but that doesn't always work.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  /**
   * Tag indicating which member is actually valid.
   * Please use the `VTERM_COLOR_IS_*` test macros to check whether a
   * particular type flag is set.
   */
  uint8_t type;

  uint8_t red, green, blue;

  uint8_t ಸೂಚ್ಯಂಕ;
} VTermColor;

/**
 * Constructs a new VTermColor instance representing the given RGB values.
 */
ಶೂನ್ಯ vterm_color_rgb(VTermColor *ಲಂಬಸಾಲು, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
ಶೂನ್ಯ vterm_color_ಸೂಚ್ಯಂಕಿತ(VTermColor *ಲಂಬಸಾಲು, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
ಪೂರ್ಣಾಂಕ vterm_color_is_equal(ಸ್ಥಿರಾಂಕ VTermColor *a, ಸ್ಥಿರಾಂಕ VTermColor *b);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_ಮೌಲ್ಯ_Type;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str;
  size_t      len : 30;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  initial : 1;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ  final : 1;
} VTermStringFragment;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಒಕ್ಕೂಟ {
  ಪೂರ್ಣಾಂಕ boolean;
  ಪೂರ್ಣಾಂಕ ಸಂಖ್ಯೆ;
  VTermStringFragment string;
  VTermColor color;
} VTerm_ಮೌಲ್ಯ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  /* VTERM_ATTR_NONE = 0 */
  VTERM_ATTR_BOLD = 1,   // bool:   1, 22
  VTERM_ATTR_UNDERLINE,  // number: 4, 21, 24
  VTERM_ATTR_ITALIC,     // bool:   3, 23
  VTERM_ATTR_BLINK,      // bool:   5, 25
  VTERM_ATTR_ಹಿಮ್ಮುಖ,    // bool:   7, 27
  VTERM_ATTR_CONCEAL,    // bool:   8, 28
  VTERM_ATTR_STRIKE,     // bool:   9, 29
  VTERM_ATTR_FONT,       // number: 10-19
  VTERM_ATTR_FOREGROUND, // color:  30-39 90-97
  VTERM_ATTR_BACKGROUND, // color:  40-49 100-107
  VTERM_ATTR_SMALL,      // bool:   73, 74, 75
  VTERM_ATTR_BASELINE,   // number: 73, 74, 75

  VTERM_N_ATTRS
} VTermAttr;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_ಹಿಮ್ಮುಖ,           // bool
  VTERM_PROP_CURSORSHAPE,       //ಸಂಖ್ಯೆ
  VTERM_PROP_MOUSE,             //ಸಂಖ್ಯೆ
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

ಎಣಿಕೆ {
  VTERM_PROP_CURSORSHAPE_ಖಂಡ = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_ಎಡ,

  VTERM_N_PROP_CURSORSHAPES
};

ಎಣಿಕೆ {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTerm_Selection_ಮುಖವಾಡ;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಸ್ಥಿರಾಂಕ uint32_t *chars;
  ಪೂರ್ಣಾಂಕ             ಅಗಲ;
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    dwl:1;             /* DECDWL or DECDHL double-width line */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    doublewidth:1;     /* DECDWL or DECDHL line */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_ಸಾಲು_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  VTermPos pos;                /* current cursor position */
  VTerm_ಸಾಲು_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  ಶೂನ್ಯ *(*ಸ್ಮರಣೆ_ಕೊಡು)(size_t ಗಾತ್ರ_2, ಶೂನ್ಯ *allocdata);
  ಶೂನ್ಯ  (*ಬಿಡುಗಡೆ)(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *allocdata);
} VTermAllocatorFunctions;

ಶೂನ್ಯ vterm_check_ಆವೃತ್ತಿ(ಪೂರ್ಣಾಂಕ major, ಪೂರ್ಣಾಂಕ minor);

ರಚನೆ VTermBuilder {
  ಪೂರ್ಣಾಂಕ ver; /* currently unused but reserved for some sort of ABI version flag */

  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಸ್ಥಿರಾಂಕ VTermAllocatorFunctions *allocator;
  ಶೂನ್ಯ *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ಸ್ಥಿರಾಂಕ ರಚನೆ VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, VTermAllocatorFunctions *funcs, ಶೂನ್ಯ *allocdata);

// Free and cleanup a terminal and all its data.
ಶೂನ್ಯ   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
ಶೂನ್ಯ vterm_get_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ VTerm *vt, ಪೂರ್ಣಾಂಕ *rowsp, ಪೂರ್ಣಾಂಕ *colsp);

ಶೂನ್ಯ vterm_ಗಣ_ಗಾತ್ರ(VTerm *vt, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

ಪೂರ್ಣಾಂಕ  vterm_get_utf8(ಸ್ಥಿರಾಂಕ VTerm *vt);
ಶೂನ್ಯ vterm_ಗಣ_utf8(VTerm *vt, ಪೂರ್ಣಾಂಕ is_utf8);

size_t vterm_ಒಳಹರಿವು_write(VTerm *vt, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len);

/* Setting output callback will override the buffer logic */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ VTermOutputCallback(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, ಶೂನ್ಯ *user);
ಶೂನ್ಯ vterm_output_ಗಣ_callback(VTerm *vt, VTermOutputCallback *func, ಶೂನ್ಯ *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_ಬಫರ್_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ VTerm *vt);
size_t vterm_output_get_ಬಫರ್_current(ಸ್ಥಿರಾಂಕ VTerm *vt);
size_t vterm_output_get_ಬಫರ್_ಉಳಿದ(ಸ್ಥಿರಾಂಕ VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, ಅಕ್ಷರ *ಬಫರ್, size_t len);

ಪೂರ್ಣಾಂಕ vterm_is_modify_other_ಕೀಲಿಗಳು(VTerm *vt);
ಪೂರ್ಣಾಂಕ vterm_is_kitty_keyboard(VTerm *vt);
ಶೂನ್ಯ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
ಶೂನ್ಯ vterm_keyboard_ಕೀಲಿ(VTerm *vt, VTerm_ಕೀಲಿ ಕೀಲಿ, VTermModifier mod);

ಶೂನ್ಯ vterm_keyboard_ಆರಂಭ_paste(VTerm *vt);
ಶೂನ್ಯ vterm_keyboard_ಕೊನೆ_paste(VTerm *vt);

ಶೂನ್ಯ vterm_mouse_move(VTerm *vt, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
ಶೂನ್ಯ vterm_mouse_button(VTerm *vt, ಪೂರ್ಣಾಂಕ button, ಪೂರ್ಣಾಂಕ pressed, VTermModifier mod);

// ------------
// Parser layer
// ------------

/* Flag to indicate non-final subparameters in a single CSI parameter.
 * Consider
 *   CSI 1;2:3:4;5a
 * 1 4 and 5 are final.
 * 2 and 3 are non-final and will have this bit set
 *
 * Don't confuse this with the final byte of the CSI escape; 'a' in this case.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG_FLAG_ಇನ್ನಷ್ಟು (1U<<31)
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG_ಮುಖವಾಡ      (~(1U<<31))

#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG_HAS_ಇನ್ನಷ್ಟು(a) ((a) & CSI_ARG_FLAG_ಇನ್ನಷ್ಟು)
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG(a)          ((a) & CSI_ARG_ಮುಖವಾಡ)

/* Can't use -1 to indicate a missing argument; use this instead */
// VIM: changed 31 to 30 to avoid an overflow warning
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG_MISSING ((1<<30)-1)

#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG_IS_MISSING(a) (CSI_ARG(a) == CSI_ARG_MISSING)
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG_OR(a,def)     (CSI_ARG(a) == CSI_ARG_MISSING ? (def) : CSI_ARG(a))
#ವ್ಯಾಖ್ಯಾನಿಸು CSI_ARG_ಎಣಿಕೆ(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ (*ಪಠ್ಯ)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*control)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ control, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*escape)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*csi)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *leader, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *intermed, ಅಕ್ಷರ command, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*osc)(ಪೂರ್ಣಾಂಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*dcs)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*apc)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*pm)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*sos)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*resize)(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಶೂನ್ಯ *user);
} VTermParserCallbacks;

ಶೂನ್ಯ  vterm_parser_ಗಣ_callbacks(VTerm *vt, ಸ್ಥಿರಾಂಕ VTermParserCallbacks *callbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
ಶೂನ್ಯ vterm_parser_ಗಣ_emit_nul(VTerm *vt, ಪೂರ್ಣಾಂಕ emit);

// -----------
// State layer
// -----------

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ (*putglyph)(VTermGlyphInfo *info, VTermPos pos, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*movecursor)(VTermPos pos, VTermPos oldpos, ಪೂರ್ಣಾಂಕ visible, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*scrollrect)(VTermRect rect, ಪೂರ್ಣಾಂಕ downward, ಪೂರ್ಣಾಂಕ rightward, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*moverect)(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*erase)(VTermRect rect, ಪೂರ್ಣಾಂಕ selective, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*initpen)(ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*setpenattr)(VTermAttr attr, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  ಪೂರ್ಣಾಂಕ (*settermprop)(VTermProp prop, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*bell)(ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*resize)(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, VTermStateFields *fields, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*setlineinfo)(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *newinfo, ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *oldinfo, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*sb_clear)(ಶೂನ್ಯ *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  ಪೂರ್ಣಾಂಕ (*premove)(VTermRect dest, ಶೂನ್ಯ *user);
} VTermStateCallbacks;

// VIM: added
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  VTermPos pos;
  ಪೂರ್ಣಾಂಕ	   buttons;
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_BUTTON_ಎಡ 0x01
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_BUTTON_ಮಧ್ಯ 0x02
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_BUTTON_ಬಲ 0x04
  ಪೂರ್ಣಾಂಕ      flags;
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_WANT_CLICK 0x01
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_WANT_DRAG  0x02
#ವ್ಯಾಖ್ಯಾನಿಸು MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ (*control)(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ control, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*csi)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *leader, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *intermed, ಅಕ್ಷರ command, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*osc)(ಪೂರ್ಣಾಂಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*dcs)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*apc)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*pm)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*sos)(VTermStringFragment frag, ಶೂನ್ಯ *user);
} VTermStateFallbacks;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ (*ಗಣ_2)(VTerm_Selection_ಮುಖವಾಡ ಮುಖವಾಡ, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*query)(VTerm_Selection_ಮುಖವಾಡ ಮುಖವಾಡ, ಶೂನ್ಯ *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

ಶೂನ್ಯ  vterm_state_ಗಣ_callbacks(VTermState *state, ಸ್ಥಿರಾಂಕ VTermStateCallbacks *callbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_state_get_cbdata(VTermState *state);

ಶೂನ್ಯ vterm_state_callbacks_has_premove(VTermState *state);

ಶೂನ್ಯ  vterm_state_ಗಣ_unrecognised_fallbacks(VTermState *state, ಸ್ಥಿರಾಂಕ VTermStateFallbacks *fallbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
ಶೂನ್ಯ vterm_state_reset(VTermState *state, ಪೂರ್ಣಾಂಕ hard);

ಶೂನ್ಯ vterm_state_get_cursorpos(ಸ್ಥಿರಾಂಕ VTermState *state, VTermPos *cursorpos);
// VIM: added
ಶೂನ್ಯ vterm_state_get_mousestate(ಸ್ಥಿರಾಂಕ VTermState *state, VTermMouseState *mousestate);
ಶೂನ್ಯ vterm_state_get_default_colors(ಸ್ಥಿರಾಂಕ VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
ಶೂನ್ಯ vterm_state_get_palette_color(ಸ್ಥಿರಾಂಕ VTermState *state, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, VTermColor *ಲಂಬಸಾಲು);
ಶೂನ್ಯ vterm_state_ಗಣ_default_colors(VTermState *state, ಸ್ಥಿರಾಂಕ VTermColor *default_fg, ಸ್ಥಿರಾಂಕ VTermColor *default_bg);
ಶೂನ್ಯ vterm_state_ಗಣ_palette_color(VTermState *state, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ VTermColor *ಲಂಬಸಾಲು);
ಶೂನ್ಯ vterm_state_ಗಣ_bold_highbright(VTermState *state, ಪೂರ್ಣಾಂಕ bold_is_highbright);
ಪೂರ್ಣಾಂಕ  vterm_state_get_penattr(ಸ್ಥಿರಾಂಕ VTermState *state, VTermAttr attr, VTerm_ಮೌಲ್ಯ *val);
ಪೂರ್ಣಾಂಕ  vterm_state_ಗಣ_termprop(VTermState *state, VTermProp prop, VTerm_ಮೌಲ್ಯ *val);
ಶೂನ್ಯ vterm_state_focus_ಒಳ(VTermState *state);
ಶೂನ್ಯ vterm_state_focus_ಹೊರಹೋಗು(VTermState *state);
ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *vterm_state_get_lineinfo(ಸ್ಥಿರಾಂಕ VTermState *state, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು);

/**
 * Makes sure that the given color `col` is indeed an RGB colour. After this
 * function returns, VTERM_COLOR_IS_RGB(col) will return true, while all other
 * flags stored in `col->type` will have been reset.
 *
 * @param state is the VTermState instance from which the colour palette should
 * be extracted.
 * @param col is a pointer at the VTermColor instance that should be converted
 * to an RGB colour.
 */
ಶೂನ್ಯ vterm_state_convert_color_ಗಮ್ಯ_rgb(ಸ್ಥಿರಾಂಕ VTermState *state, VTermColor *ಲಂಬಸಾಲು);

ಶೂನ್ಯ vterm_state_ಗಣ_selection_callbacks(VTermState *state, ಸ್ಥಿರಾಂಕ VTermSelectionCallbacks *callbacks, ಶೂನ್ಯ *user,
    ಅಕ್ಷರ *ಬಫರ್, size_t buflen);

ಶೂನ್ಯ vterm_state_send_selection(VTermState *state, VTerm_Selection_ಮುಖವಾಡ ಮುಖವಾಡ, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ bold      : 1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ underline : 2;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ italic    : 1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ blink     : 1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಹಿಮ್ಮುಖ   : 1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ conceal   : 1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ strike    : 1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ font      : 4; /* 0 to 9 */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ dwl       : 1; /* On a DECDWL or DECDHL line */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ small     : 1;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ baseline  : 2;
} VTermScreenCellAttrs;

ಎಣಿಕೆ {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

ಎಣಿಕೆ {
  VTERM_BASELINE_ಸಾಮಾನ್ಯ,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_ಅಧೋ,
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  uint32_t chars[VTERM_MAX_CHARS_ಪ್ರತಿ_CELL];
  ಅಕ್ಷರ     ಅಗಲ;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ (*damage)(VTermRect rect, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*moverect)(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*movecursor)(VTermPos pos, VTermPos oldpos, ಪೂರ್ಣಾಂಕ visible, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*settermprop)(VTermProp prop, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*bell)(ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*resize)(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಶೂನ್ಯ *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  ಪೂರ್ಣಾಂಕ (*sb_pushline)(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಸ್ಥಿರಾಂಕ VTermScreenCell *cells, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*sb_popline)(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, VTermScreenCell *cells, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಂಕ (*sb_clear)(ಶೂನ್ಯ* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  ಪೂರ್ಣಾಂಕ (*sb_pushline4)(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಸ್ಥಿರಾಂಕ VTermScreenCell *cells, ಪೂರ್ಣಾಂಕ continuation, ಶೂನ್ಯ *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
ಶೂನ್ಯ  vterm_screen_ಗಣ_callbacks(VTermScreen *screen, ಸ್ಥಿರಾಂಕ VTermScreenCallbacks *callbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_screen_get_cbdata(VTermScreen *screen);

ಶೂನ್ಯ vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

ಶೂನ್ಯ  vterm_screen_ಗಣ_unrecognised_fallbacks(VTermScreen *screen, ಸ್ಥಿರಾಂಕ VTermStateFallbacks *fallbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

ಶೂನ್ಯ vterm_screen_enable_reflow(VTermScreen *screen, ಪೂರ್ಣಾಂಕ reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#ವ್ಯಾಖ್ಯಾನಿಸು vterm_screen_ಗಣ_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
ಶೂನ್ಯ vterm_screen_enable_altscreen(VTermScreen *screen, ಪೂರ್ಣಾಂಕ altscreen);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ಅಡ್ಡಸಾಲು,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTerm_Damage_ಗಾತ್ರ;

// Invoke the relevant callbacks to update the screen.
ಶೂನ್ಯ vterm_screen_flush_damage(VTermScreen *screen);

ಶೂನ್ಯ vterm_screen_ಗಣ_damage_ವಿಲೀನ(VTermScreen *screen, VTerm_Damage_ಗಾತ್ರ ಗಾತ್ರ_2);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
ಶೂನ್ಯ   vterm_screen_reset(VTermScreen *screen, ಪೂರ್ಣಾಂಕ hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ಸ್ಥಿರಾಂಕ VTermScreen *screen, uint32_t *chars, size_t len, ಸ್ಥಿರಾಂಕ VTermRect rect);
size_t vterm_screen_get_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ VTermScreen *screen, ಅಕ್ಷರ *str, size_t len, ಸ್ಥಿರಾಂಕ VTermRect rect);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  VTERM_ATTR_BOLD_ಮುಖವಾಡ       = 1 << 0,
  VTERM_ATTR_UNDERLINE_ಮುಖವಾಡ  = 1 << 1,
  VTERM_ATTR_ITALIC_ಮುಖವಾಡ     = 1 << 2,
  VTERM_ATTR_BLINK_ಮುಖವಾಡ      = 1 << 3,
  VTERM_ATTR_ಹಿಮ್ಮುಖ_ಮುಖವಾಡ    = 1 << 4,
  VTERM_ATTR_STRIKE_ಮುಖವಾಡ     = 1 << 5,
  VTERM_ATTR_FONT_ಮುಖವಾಡ       = 1 << 6,
  VTERM_ATTR_FOREGROUND_ಮುಖವಾಡ = 1 << 7,
  VTERM_ATTR_BACKGROUND_ಮುಖವಾಡ = 1 << 8,
  VTERM_ATTR_CONCEAL_ಮುಖವಾಡ    = 1 << 9,
  VTERM_ATTR_SMALL_ಮುಖವಾಡ      = 1 << 10,
  VTERM_ATTR_BASELINE_ಮುಖವಾಡ   = 1 << 11,

  VTERM_ALL_ATTRS_ಮುಖವಾಡ = (1 << 12) - 1
} VTerm_Attr_ಮುಖವಾಡ;

ಪೂರ್ಣಾಂಕ vterm_screen_get_attrs_extent(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ಮುಖವಾಡ attrs);

ಪೂರ್ಣಾಂಕ vterm_screen_get_cell(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

ಪೂರ್ಣಾಂಕ vterm_screen_is_eol(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
ಶೂನ್ಯ vterm_screen_convert_color_ಗಮ್ಯ_rgb(ಸ್ಥಿರಾಂಕ VTermScreen *screen, VTermColor *ಲಂಬಸಾಲು);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
ಶೂನ್ಯ vterm_screen_ಗಣ_default_colors(VTermScreen *screen, ಸ್ಥಿರಾಂಕ VTermColor *default_fg, ಸ್ಥಿರಾಂಕ VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_ಮೌಲ್ಯ_Type vterm_get_attr_type(VTermAttr attr);
VTerm_ಮೌಲ್ಯ_Type vterm_get_prop_type(VTermProp prop);

ಶೂನ್ಯ vterm_scroll_rect(VTermRect rect,
                       ಪೂರ್ಣಾಂಕ downward,
                       ಪೂರ್ಣಾಂಕ rightward,
                       ಪೂರ್ಣಾಂಕ (*moverect)(VTermRect src, VTermRect dest, ಶೂನ್ಯ *user),
                       ಪೂರ್ಣಾಂಕ (*eraserect)(VTermRect rect, ಪೂರ್ಣಾಂಕ selective, ಶೂನ್ಯ *user),
                       ಶೂನ್ಯ *user);

ಶೂನ್ಯ vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      ಶೂನ್ಯ (*copycell)(VTermPos dest, VTermPos src, ಶೂನ್ಯ *user),
                      ಶೂನ್ಯ *user);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __cplusplus
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ
