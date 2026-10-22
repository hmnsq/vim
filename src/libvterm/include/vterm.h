/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
বাহ্য "C" {
#endif

#অন্তর্ভাৱ <stdlib.h>

#অন্তর্ভাৱ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
প্রকারনির্দেশ unsigned ৱর্ণ		uint8_t;
প্রকারনির্দেশ unsigned short		uint16_t;
প্রকারনির্দেশ unsigned পূর্ণাঙ্ক		uint32_t;

// VIM: define max screen cols and rows
#define VTERM_MAX_COLS 1000
#define VTERM_MAX_ROWS 1000

#define VTERM_VERSION_MAJOR 0
#define VTERM_VERSION_MINOR 3
#define VTERM_VERSION_PATCH 3

#define VTERM_CHECK_VERSION \
        vterm_check_version(VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR)

/* Any cell can contain at most one basic printing character and 5 combining
 * characters. This number could be changed but will be ABI-incompatible if
 * you do */
#define VTERM_MAX_CHARS_PER_CELL 6

প্রকারনির্দেশ সংরচনা VTerm VTerm;
প্রকারনির্দেশ সংরচনা VTermState VTermState;
প্রকারনির্দেশ সংরচনা VTermScreen VTermScreen;

// Specifies a screen point.
প্রকারনির্দেশ সংরচনা {
  পূর্ণাঙ্ক row;
  পূর্ণাঙ্ক col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
পূর্ণাঙ্ক vterm_pos_cmp(VTermPos a, VTermPos b);

#যদি defined(DEFINE_INLINES) || USE_INLINE
INLINE পূর্ণাঙ্ক vterm_pos_cmp(VTermPos a, VTermPos b)
{
  ঘূৰাই (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
প্রকারনির্দেশ সংরচনা {
  পূর্ণাঙ্ক start_row;
  পূর্ণাঙ্ক end_row;
  পূর্ণাঙ্ক start_col;
  পূর্ণাঙ্ক end_col;
} VTermRect;

/* true if the rect contains the point */
পূর্ণাঙ্ক vterm_rect_contains(VTermRect r, VTermPos p);

#যদি defined(DEFINE_INLINES) || USE_INLINE
INLINE পূর্ণাঙ্ক vterm_rect_contains(VTermRect r, VTermPos p)
{
  ঘূৰাই p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
শূন্য vterm_rect_move(VTermRect *rect, পূর্ণাঙ্ক row_delta, পূর্ণাঙ্ক col_delta);

#যদি defined(DEFINE_INLINES) || USE_INLINE
INLINE শূন্য vterm_rect_move(VTermRect *rect, পূর্ণাঙ্ক row_delta, পূর্ণাঙ্ক col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
প্রকারনির্দেশ গণনা {
  /**
   * If the lower bit of `type` is not set, the colour is 24-bit RGB.
   */
  VTERM_COLOR_RGB = 0x00,

  /**
   * The colour is an index into a palette of 256 colours.
   */
  VTERM_COLOR_INDEXED = 0x01,

  /**
   * Mask that can be used to extract the RGB/Indexed bit.
   */
  VTERM_COLOR_TYPE_MASK = 0x01,

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
  VTERM_COLOR_DEFAULT_MASK = 0x06,

  /**
   * VIM: If set, indicates that the color is invalid.
   */
  VTERM_COLOR_INVALID = 0x08
} VTermColorType;

/**
 * Returns true if the VTERM_COLOR_RGB `type` flag is set, indicating that the
 * given VTermColor instance is an indexed colour.
 */
#define VTERM_COLOR_IS_INDEXED(col) \
  (((col)->type & VTERM_COLOR_TYPE_MASK) == VTERM_COLOR_INDEXED)

/**
 * Returns true if the VTERM_COLOR_INDEXED `type` flag is set, indicating that
 * the given VTermColor instance is an rgb colour.
 */
#define VTERM_COLOR_IS_RGB(col) \
  (((col)->type & VTERM_COLOR_TYPE_MASK) == VTERM_COLOR_RGB)

/**
 * Returns true if the VTERM_COLOR_DEFAULT_FG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default foreground
 * color.
 */
#define VTERM_COLOR_IS_DEFAULT_FG(col) \
  (!!((col)->type & VTERM_COLOR_DEFAULT_FG))

/**
 * Returns true if the VTERM_COLOR_DEFAULT_BG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default background
 * color.
 */
#define VTERM_COLOR_IS_DEFAULT_BG(col) \
  (!!((col)->type & VTERM_COLOR_DEFAULT_BG))

/**
 * Returns true if the VTERM_COLOR_INVALID `type` flag is set, indicating
 * that the given VTermColor instance is an invalid color.
 */
#define VTERM_COLOR_IS_INVALID(col) (!!((col)->type & VTERM_COLOR_INVALID))

// VIM: this was a union, but that doesn't always work.
প্রকারনির্দেশ সংরচনা {
  /**
   * Tag indicating which member is actually valid.
   * Please use the `VTERM_COLOR_IS_*` test macros to check whether a
   * particular type flag is set.
   */
  uint8_t type;

  uint8_t red, green, blue;

  uint8_t index;
} VTermColor;

/**
 * Constructs a new VTermColor instance representing the given RGB values.
 */
শূন্য vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
শূন্য vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
পূর্ণাঙ্ক vterm_color_is_equal(ধ্রুৱ VTermColor *a, ধ্রুৱ VTermColor *b);

প্রকারনির্দেশ গণনা {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_মান_Type;

প্রকারনির্দেশ সংরচনা {
  ধ্রুৱ ৱর্ণ *str;
  size_t      len : 30;
  unsigned পূর্ণাঙ্ক  initial : 1;
  unsigned পূর্ণাঙ্ক  final : 1;
} VTermStringFragment;

প্রকারনির্দেশ সঙ্ঘ {
  পূর্ণাঙ্ক boolean;
  পূর্ণাঙ্ক সংখ্যা;
  VTermStringFragment string;
  VTermColor color;
} VTerm_মান;

প্রকারনির্দেশ গণনা {
  /* VTERM_ATTR_NONE = 0 */
  VTERM_ATTR_BOLD = 1,   // bool:   1, 22
  VTERM_ATTR_UNDERLINE,  // number: 4, 21, 24
  VTERM_ATTR_ITALIC,     // bool:   3, 23
  VTERM_ATTR_BLINK,      // bool:   5, 25
  VTERM_ATTR_REVERSE,    // bool:   7, 27
  VTERM_ATTR_CONCEAL,    // bool:   8, 28
  VTERM_ATTR_STRIKE,     // bool:   9, 29
  VTERM_ATTR_FONT,       // number: 10-19
  VTERM_ATTR_FOREGROUND, // color:  30-39 90-97
  VTERM_ATTR_BACKGROUND, // color:  40-49 100-107
  VTERM_ATTR_SMALL,      // bool:   73, 74, 75
  VTERM_ATTR_BASELINE,   // number: 73, 74, 75

  VTERM_N_ATTRS
} VTermAttr;

প্রকারনির্দেশ গণনা {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //সংখ্যা
  VTERM_PROP_MOUSE,             //সংখ্যা
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

গণনা {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_বাম,

  VTERM_N_PROP_CURSORSHAPES
};

গণনা {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

প্রকারনির্দেশ গণনা {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

প্রকারনির্দেশ সংরচনা {
  ধ্রুৱ uint32_t *chars;
  পূর্ণাঙ্ক             width;
  unsigned পূর্ণাঙ্ক    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned পূর্ণাঙ্ক    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned পূর্ণাঙ্ক    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

প্রকারনির্দেশ সংরচনা {
  unsigned পূর্ণাঙ্ক    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned পূর্ণাঙ্ক    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned পূর্ণাঙ্ক    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_সাৰি_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
প্রকারনির্দেশ সংরচনা {
  VTermPos pos;                /* current cursor position */
  VTerm_সাৰি_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

প্রকারনির্দেশ সংরচনা {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  শূন্য *(*malloc)(size_t size, শূন্য *allocdata);
  শূন্য  (*মুক্ত)(শূন্য *ptr, শূন্য *allocdata);
} VTermAllocatorFunctions;

শূন্য vterm_check_version(পূর্ণাঙ্ক major, পূর্ণাঙ্ক minor);

সংরচনা VTermBuilder {
  পূর্ণাঙ্ক ver; /* currently unused but reserved for some sort of ABI version flag */

  পূর্ণাঙ্ক rows, cols;

  ধ্রুৱ VTermAllocatorFunctions *allocator;
  শূন্য *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ধ্রুৱ সংরচনা VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols, VTermAllocatorFunctions *funcs, শূন্য *allocdata);

// Free and cleanup a terminal and all its data.
শূন্য   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
শূন্য vterm_get_size(ধ্রুৱ VTerm *vt, পূর্ণাঙ্ক *rowsp, পূর্ণাঙ্ক *colsp);

শূন্য vterm_set_size(VTerm *vt, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols);

পূর্ণাঙ্ক  vterm_get_utf8(ধ্রুৱ VTerm *vt);
শূন্য vterm_set_utf8(VTerm *vt, পূর্ণাঙ্ক is_utf8);

size_t vterm_প্ৰবেশ_write(VTerm *vt, ধ্রুৱ ৱর্ণ *bytes, size_t len);

/* Setting output callback will override the buffer logic */
প্রকারনির্দেশ শূন্য VTermOutputCallback(ধ্রুৱ ৱর্ণ *s, size_t len, শূন্য *user);
শূন্য vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, শূন্য *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(ধ্রুৱ VTerm *vt);
size_t vterm_output_get_buffer_current(ধ্রুৱ VTerm *vt);
size_t vterm_output_get_buffer_remaining(ধ্রুৱ VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, ৱর্ণ *buffer, size_t len);

পূর্ণাঙ্ক vterm_is_modify_other_চাবিগুলি(VTerm *vt);
পূর্ণাঙ্ক vterm_is_kitty_keyboard(VTerm *vt);
শূন্য vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
শূন্য vterm_keyboard_চাবি(VTerm *vt, VTerm_চাবি চাবি, VTermModifier mod);

শূন্য vterm_keyboard_start_paste(VTerm *vt);
শূন্য vterm_keyboard_end_paste(VTerm *vt);

শূন্য vterm_mouse_move(VTerm *vt, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
শূন্য vterm_mouse_button(VTerm *vt, পূর্ণাঙ্ক button, পূর্ণাঙ্ক pressed, VTermModifier mod);

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
#define CSI_ARG_FLAG_MORE (1U<<31)
#define CSI_ARG_MASK      (~(1U<<31))

#define CSI_ARG_HAS_MORE(a) ((a) & CSI_ARG_FLAG_MORE)
#define CSI_ARG(a)          ((a) & CSI_ARG_MASK)

/* Can't use -1 to indicate a missing argument; use this instead */
// VIM: changed 31 to 30 to avoid an overflow warning
#define CSI_ARG_MISSING ((1<<30)-1)

#define CSI_ARG_IS_MISSING(a) (CSI_ARG(a) == CSI_ARG_MISSING)
#define CSI_ARG_OR(a,def)     (CSI_ARG(a) == CSI_ARG_MISSING ? (def) : CSI_ARG(a))
#define CSI_ARG_গুনতি(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

প্রকারনির্দেশ সংরচনা {
  পূর্ণাঙ্ক (*text)(ধ্রুৱ ৱর্ণ *bytes, size_t len, শূন্য *user);
  পূর্ণাঙ্ক (*control)(unsigned ৱর্ণ control, শূন্য *user);
  পূর্ণাঙ্ক (*escape)(ধ্রুৱ ৱর্ণ *bytes, size_t len, শূন্য *user);
  পূর্ণাঙ্ক (*csi)(ধ্রুৱ ৱর্ণ *leader, ধ্রুৱ long args[], পূর্ণাঙ্ক argcount, ধ্রুৱ ৱর্ণ *intermed, ৱর্ণ command, শূন্য *user);
  পূর্ণাঙ্ক (*osc)(পূর্ণাঙ্ক command, VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*dcs)(ধ্রুৱ ৱর্ণ *command, size_t commandlen, VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*apc)(VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*pm)(VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*sos)(VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*resize)(পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols, শূন্য *user);
} VTermParserCallbacks;

শূন্য  vterm_parser_set_callbacks(VTerm *vt, ধ্রুৱ VTermParserCallbacks *callbacks, শূন্য *user);
শূন্য *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
শূন্য vterm_parser_set_emit_nul(VTerm *vt, পূর্ণাঙ্ক emit);

// -----------
// State layer
// -----------

প্রকারনির্দেশ সংরচনা {
  পূর্ণাঙ্ক (*putglyph)(VTermGlyphInfo *info, VTermPos pos, শূন্য *user);
  পূর্ণাঙ্ক (*movecursor)(VTermPos pos, VTermPos oldpos, পূর্ণাঙ্ক visible, শূন্য *user);
  পূর্ণাঙ্ক (*scrollrect)(VTermRect rect, পূর্ণাঙ্ক downward, পূর্ণাঙ্ক rightward, শূন্য *user);
  পূর্ণাঙ্ক (*moverect)(VTermRect dest, VTermRect src, শূন্য *user);
  পূর্ণাঙ্ক (*erase)(VTermRect rect, পূর্ণাঙ্ক selective, শূন্য *user);
  পূর্ণাঙ্ক (*initpen)(শূন্য *user);
  পূর্ণাঙ্ক (*setpenattr)(VTermAttr attr, VTerm_মান *val, শূন্য *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  পূর্ণাঙ্ক (*settermprop)(VTermProp prop, VTerm_মান *val, শূন্য *user);
  পূর্ণাঙ্ক (*bell)(শূন্য *user);
  পূর্ণাঙ্ক (*resize)(পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols, VTermStateFields *fields, শূন্য *user);
  পূর্ণাঙ্ক (*setlineinfo)(পূর্ণাঙ্ক row, ধ্রুৱ VTerm_সাৰি_Info *newinfo, ধ্রুৱ VTerm_সাৰি_Info *oldinfo, শূন্য *user);
  পূর্ণাঙ্ক (*sb_clear)(শূন্য *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  পূর্ণাঙ্ক (*premove)(VTermRect dest, শূন্য *user);
} VTermStateCallbacks;

// VIM: added
প্রকারনির্দেশ সংরচনা {
  VTermPos pos;
  পূর্ণাঙ্ক	   buttons;
#define MOUSE_BUTTON_বাম 0x01
#define MOUSE_BUTTON_মধ্য 0x02
#define MOUSE_BUTTON_ডান 0x04
  পূর্ণাঙ্ক      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

প্রকারনির্দেশ সংরচনা {
  পূর্ণাঙ্ক (*control)(unsigned ৱর্ণ control, শূন্য *user);
  পূর্ণাঙ্ক (*csi)(ধ্রুৱ ৱর্ণ *leader, ধ্রুৱ long args[], পূর্ণাঙ্ক argcount, ধ্রুৱ ৱর্ণ *intermed, ৱর্ণ command, শূন্য *user);
  পূর্ণাঙ্ক (*osc)(পূর্ণাঙ্ক command, VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*dcs)(ধ্রুৱ ৱর্ণ *command, size_t commandlen, VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*apc)(VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*pm)(VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*sos)(VTermStringFragment frag, শূন্য *user);
} VTermStateFallbacks;

প্রকারনির্দেশ সংরচনা {
  পূর্ণাঙ্ক (*set)(VTermSelectionMask mask, VTermStringFragment frag, শূন্য *user);
  পূর্ণাঙ্ক (*query)(VTermSelectionMask mask, শূন্য *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

শূন্য  vterm_state_set_callbacks(VTermState *state, ধ্রুৱ VTermStateCallbacks *callbacks, শূন্য *user);
শূন্য *vterm_state_get_cbdata(VTermState *state);

শূন্য vterm_state_callbacks_has_premove(VTermState *state);

শূন্য  vterm_state_set_unrecognised_fallbacks(VTermState *state, ধ্রুৱ VTermStateFallbacks *fallbacks, শূন্য *user);
শূন্য *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
শূন্য vterm_state_reset(VTermState *state, পূর্ণাঙ্ক hard);

শূন্য vterm_state_get_cursorpos(ধ্রুৱ VTermState *state, VTermPos *cursorpos);
// VIM: added
শূন্য vterm_state_get_mousestate(ধ্রুৱ VTermState *state, VTermMouseState *mousestate);
শূন্য vterm_state_get_default_colors(ধ্রুৱ VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
শূন্য vterm_state_get_palette_color(ধ্রুৱ VTermState *state, পূর্ণাঙ্ক index, VTermColor *col);
শূন্য vterm_state_set_default_colors(VTermState *state, ধ্রুৱ VTermColor *default_fg, ধ্রুৱ VTermColor *default_bg);
শূন্য vterm_state_set_palette_color(VTermState *state, পূর্ণাঙ্ক index, ধ্রুৱ VTermColor *col);
শূন্য vterm_state_set_bold_highbright(VTermState *state, পূর্ণাঙ্ক bold_is_highbright);
পূর্ণাঙ্ক  vterm_state_get_penattr(ধ্রুৱ VTermState *state, VTermAttr attr, VTerm_মান *val);
পূর্ণাঙ্ক  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_মান *val);
শূন্য vterm_state_focus_in(VTermState *state);
শূন্য vterm_state_focus_out(VTermState *state);
ধ্রুৱ VTerm_সাৰি_Info *vterm_state_get_lineinfo(ধ্রুৱ VTermState *state, পূর্ণাঙ্ক row);

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
শূন্য vterm_state_convert_color_to_rgb(ধ্রুৱ VTermState *state, VTermColor *col);

শূন্য vterm_state_set_selection_callbacks(VTermState *state, ধ্রুৱ VTermSelectionCallbacks *callbacks, শূন্য *user,
    ৱর্ণ *buffer, size_t buflen);

শূন্য vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

প্রকারনির্দেশ সংরচনা {
    unsigned পূর্ণাঙ্ক bold      : 1;
    unsigned পূর্ণাঙ্ক underline : 2;
    unsigned পূর্ণাঙ্ক italic    : 1;
    unsigned পূর্ণাঙ্ক blink     : 1;
    unsigned পূর্ণাঙ্ক reverse   : 1;
    unsigned পূর্ণাঙ্ক conceal   : 1;
    unsigned পূর্ণাঙ্ক strike    : 1;
    unsigned পূর্ণাঙ্ক font      : 4; /* 0 to 9 */
    unsigned পূর্ণাঙ্ক dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned পূর্ণাঙ্ক dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned পূর্ণাঙ্ক small     : 1;
    unsigned পূর্ণাঙ্ক baseline  : 2;
} VTermScreenCellAttrs;

গণনা {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

গণনা {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

প্রকারনির্দেশ সংরচনা {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ৱর্ণ     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
প্রকারনির্দেশ সংরচনা {
  পূর্ণাঙ্ক (*damage)(VTermRect rect, শূন্য *user);
  পূর্ণাঙ্ক (*moverect)(VTermRect dest, VTermRect src, শূন্য *user);
  পূর্ণাঙ্ক (*movecursor)(VTermPos pos, VTermPos oldpos, পূর্ণাঙ্ক visible, শূন্য *user);
  পূর্ণাঙ্ক (*settermprop)(VTermProp prop, VTerm_মান *val, শূন্য *user);
  পূর্ণাঙ্ক (*bell)(শূন্য *user);
  পূর্ণাঙ্ক (*resize)(পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols, শূন্য *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  পূর্ণাঙ্ক (*sb_pushline)(পূর্ণাঙ্ক cols, ধ্রুৱ VTermScreenCell *cells, শূন্য *user);
  পূর্ণাঙ্ক (*sb_popline)(পূর্ণাঙ্ক cols, VTermScreenCell *cells, শূন্য *user);
  পূর্ণাঙ্ক (*sb_clear)(শূন্য* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  পূর্ণাঙ্ক (*sb_pushline4)(পূর্ণাঙ্ক cols, ধ্রুৱ VTermScreenCell *cells, পূর্ণাঙ্ক continuation, শূন্য *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
শূন্য  vterm_screen_set_callbacks(VTermScreen *screen, ধ্রুৱ VTermScreenCallbacks *callbacks, শূন্য *user);
শূন্য *vterm_screen_get_cbdata(VTermScreen *screen);

শূন্য vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

শূন্য  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ধ্রুৱ VTermStateFallbacks *fallbacks, শূন্য *user);
শূন্য *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

শূন্য vterm_screen_enable_reflow(VTermScreen *screen, পূর্ণাঙ্ক reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
শূন্য vterm_screen_enable_altscreen(VTermScreen *screen, পূর্ণাঙ্ক altscreen);

প্রকারনির্দেশ গণনা {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
শূন্য vterm_screen_flush_damage(VTermScreen *screen);

শূন্য vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
শূন্য   vterm_screen_reset(VTermScreen *screen, পূর্ণাঙ্ক hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ধ্রুৱ VTermScreen *screen, uint32_t *chars, size_t len, ধ্রুৱ VTermRect rect);
size_t vterm_screen_get_text(ধ্রুৱ VTermScreen *screen, ৱর্ণ *str, size_t len, ধ্রুৱ VTermRect rect);

প্রকারনির্দেশ গণনা {
  VTERM_ATTR_BOLD_MASK       = 1 << 0,
  VTERM_ATTR_UNDERLINE_MASK  = 1 << 1,
  VTERM_ATTR_ITALIC_MASK     = 1 << 2,
  VTERM_ATTR_BLINK_MASK      = 1 << 3,
  VTERM_ATTR_REVERSE_MASK    = 1 << 4,
  VTERM_ATTR_STRIKE_MASK     = 1 << 5,
  VTERM_ATTR_FONT_MASK       = 1 << 6,
  VTERM_ATTR_FOREGROUND_MASK = 1 << 7,
  VTERM_ATTR_BACKGROUND_MASK = 1 << 8,
  VTERM_ATTR_CONCEAL_MASK    = 1 << 9,
  VTERM_ATTR_SMALL_MASK      = 1 << 10,
  VTERM_ATTR_BASELINE_MASK   = 1 << 11,

  VTERM_ALL_ATTRS_MASK = (1 << 12) - 1
} VTermAttrMask;

পূর্ণাঙ্ক vterm_screen_get_attrs_extent(ধ্রুৱ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

পূর্ণাঙ্ক vterm_screen_get_cell(ধ্রুৱ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

পূর্ণাঙ্ক vterm_screen_is_eol(ধ্রুৱ VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
শূন্য vterm_screen_convert_color_to_rgb(ধ্রুৱ VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
শূন্য vterm_screen_set_default_colors(VTermScreen *screen, ধ্রুৱ VTermColor *default_fg, ধ্রুৱ VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_মান_Type vterm_get_attr_type(VTermAttr attr);
VTerm_মান_Type vterm_get_prop_type(VTermProp prop);

শূন্য vterm_scroll_rect(VTermRect rect,
                       পূর্ণাঙ্ক downward,
                       পূর্ণাঙ্ক rightward,
                       পূর্ণাঙ্ক (*moverect)(VTermRect src, VTermRect dest, শূন্য *user),
                       পূর্ণাঙ্ক (*eraserect)(VTermRect rect, পূর্ণাঙ্ক selective, শূন্য *user),
                       শূন্য *user);

শূন্য vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      শূন্য (*copycell)(VTermPos dest, VTermPos src, শূন্য *user),
                      শূন্য *user);

#ifdef __cplusplus
}
#endif

#endif
