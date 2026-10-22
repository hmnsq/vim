/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
வெளி "C" {
#endif

#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
வகைவரை unsigned எழுத்து		uint8_t;
வகைவரை unsigned short		uint16_t;
வகைவரை unsigned முழு		uint32_t;

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

வகைவரை கட்டமைப்பு VTerm VTerm;
வகைவரை கட்டமைப்பு VTermState VTermState;
வகைவரை கட்டமைப்பு VTermScreen VTermScreen;

// Specifies a screen point.
வகைவரை கட்டமைப்பு {
  முழு row;
  முழு col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
முழு vterm_pos_cmp(VTermPos a, VTermPos b);

#எனில் defined(DEFINE_INLINES) || USE_INLINE
INLINE முழு vterm_pos_cmp(VTermPos a, VTermPos b)
{
  திரும்பு (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
வகைவரை கட்டமைப்பு {
  முழு start_row;
  முழு end_row;
  முழு start_col;
  முழு end_col;
} VTermRect;

/* true if the rect contains the point */
முழு vterm_rect_contains(VTermRect r, VTermPos p);

#எனில் defined(DEFINE_INLINES) || USE_INLINE
INLINE முழு vterm_rect_contains(VTermRect r, VTermPos p)
{
  திரும்பு p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
வெறுமை vterm_rect_move(VTermRect *rect, முழு row_delta, முழு col_delta);

#எனில் defined(DEFINE_INLINES) || USE_INLINE
INLINE வெறுமை vterm_rect_move(VTermRect *rect, முழு row_delta, முழு col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
வகைவரை எண்ணிக்கை {
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
வகைவரை கட்டமைப்பு {
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
வெறுமை vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
வெறுமை vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
முழு vterm_color_is_equal(மாறிலி VTermColor *a, மாறிலி VTermColor *b);

வகைவரை எண்ணிக்கை {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_மதிப்பு_Type;

வகைவரை கட்டமைப்பு {
  மாறிலி எழுத்து *str;
  size_t      len : 30;
  unsigned முழு  initial : 1;
  unsigned முழு  final : 1;
} VTermStringFragment;

வகைவரை ஒன்றிப்பு {
  முழு boolean;
  முழு எண்;
  VTermStringFragment string;
  VTermColor color;
} VTerm_மதிப்பு;

வகைவரை எண்ணிக்கை {
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

வகைவரை எண்ணிக்கை {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //எண்
  VTERM_PROP_MOUSE,             //எண்
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

எண்ணிக்கை {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_இடது,

  VTERM_N_PROP_CURSORSHAPES
};

எண்ணிக்கை {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

வகைவரை எண்ணிக்கை {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

வகைவரை கட்டமைப்பு {
  மாறிலி uint32_t *chars;
  முழு             width;
  unsigned முழு    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned முழு    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned முழு    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

வகைவரை கட்டமைப்பு {
  unsigned முழு    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned முழு    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned முழு    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_வரி_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
வகைவரை கட்டமைப்பு {
  VTermPos pos;                /* current cursor position */
  VTerm_வரி_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

வகைவரை கட்டமைப்பு {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  வெறுமை *(*malloc)(size_t size, வெறுமை *allocdata);
  வெறுமை  (*விடுவி)(வெறுமை *ptr, வெறுமை *allocdata);
} VTermAllocatorFunctions;

வெறுமை vterm_check_version(முழு major, முழு minor);

கட்டமைப்பு VTermBuilder {
  முழு ver; /* currently unused but reserved for some sort of ABI version flag */

  முழு rows, cols;

  மாறிலி VTermAllocatorFunctions *allocator;
  வெறுமை *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(மாறிலி கட்டமைப்பு VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(முழு rows, முழு cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(முழு rows, முழு cols, VTermAllocatorFunctions *funcs, வெறுமை *allocdata);

// Free and cleanup a terminal and all its data.
வெறுமை   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
வெறுமை vterm_get_size(மாறிலி VTerm *vt, முழு *rowsp, முழு *colsp);

வெறுமை vterm_set_size(VTerm *vt, முழு rows, முழு cols);

முழு  vterm_get_utf8(மாறிலி VTerm *vt);
வெறுமை vterm_set_utf8(VTerm *vt, முழு is_utf8);

size_t vterm_உள்ளீடு_write(VTerm *vt, மாறிலி எழுத்து *bytes, size_t len);

/* Setting output callback will override the buffer logic */
வகைவரை வெறுமை VTermOutputCallback(மாறிலி எழுத்து *s, size_t len, வெறுமை *user);
வெறுமை vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, வெறுமை *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(மாறிலி VTerm *vt);
size_t vterm_output_get_buffer_current(மாறிலி VTerm *vt);
size_t vterm_output_get_buffer_remaining(மாறிலி VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, எழுத்து *buffer, size_t len);

முழு vterm_is_modify_other_சாவிகள்(VTerm *vt);
முழு vterm_is_kitty_keyboard(VTerm *vt);
வெறுமை vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
வெறுமை vterm_keyboard_சாவி(VTerm *vt, VTerm_சாவி சாவி, VTermModifier mod);

வெறுமை vterm_keyboard_start_paste(VTerm *vt);
வெறுமை vterm_keyboard_end_paste(VTerm *vt);

வெறுமை vterm_mouse_move(VTerm *vt, முழு row, முழு col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
வெறுமை vterm_mouse_button(VTerm *vt, முழு button, முழு pressed, VTermModifier mod);

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
#define CSI_ARG_மொத்தம்(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

வகைவரை கட்டமைப்பு {
  முழு (*text)(மாறிலி எழுத்து *bytes, size_t len, வெறுமை *user);
  முழு (*control)(unsigned எழுத்து control, வெறுமை *user);
  முழு (*escape)(மாறிலி எழுத்து *bytes, size_t len, வெறுமை *user);
  முழு (*csi)(மாறிலி எழுத்து *leader, மாறிலி long args[], முழு argcount, மாறிலி எழுத்து *intermed, எழுத்து command, வெறுமை *user);
  முழு (*osc)(முழு command, VTermStringFragment frag, வெறுமை *user);
  முழு (*dcs)(மாறிலி எழுத்து *command, size_t commandlen, VTermStringFragment frag, வெறுமை *user);
  முழு (*apc)(VTermStringFragment frag, வெறுமை *user);
  முழு (*pm)(VTermStringFragment frag, வெறுமை *user);
  முழு (*sos)(VTermStringFragment frag, வெறுமை *user);
  முழு (*resize)(முழு rows, முழு cols, வெறுமை *user);
} VTermParserCallbacks;

வெறுமை  vterm_parser_set_callbacks(VTerm *vt, மாறிலி VTermParserCallbacks *callbacks, வெறுமை *user);
வெறுமை *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
வெறுமை vterm_parser_set_emit_nul(VTerm *vt, முழு emit);

// -----------
// State layer
// -----------

வகைவரை கட்டமைப்பு {
  முழு (*putglyph)(VTermGlyphInfo *info, VTermPos pos, வெறுமை *user);
  முழு (*movecursor)(VTermPos pos, VTermPos oldpos, முழு visible, வெறுமை *user);
  முழு (*scrollrect)(VTermRect rect, முழு downward, முழு rightward, வெறுமை *user);
  முழு (*moverect)(VTermRect dest, VTermRect src, வெறுமை *user);
  முழு (*erase)(VTermRect rect, முழு selective, வெறுமை *user);
  முழு (*initpen)(வெறுமை *user);
  முழு (*setpenattr)(VTermAttr attr, VTerm_மதிப்பு *val, வெறுமை *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  முழு (*settermprop)(VTermProp prop, VTerm_மதிப்பு *val, வெறுமை *user);
  முழு (*bell)(வெறுமை *user);
  முழு (*resize)(முழு rows, முழு cols, VTermStateFields *fields, வெறுமை *user);
  முழு (*setlineinfo)(முழு row, மாறிலி VTerm_வரி_Info *newinfo, மாறிலி VTerm_வரி_Info *oldinfo, வெறுமை *user);
  முழு (*sb_clear)(வெறுமை *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  முழு (*premove)(VTermRect dest, வெறுமை *user);
} VTermStateCallbacks;

// VIM: added
வகைவரை கட்டமைப்பு {
  VTermPos pos;
  முழு	   buttons;
#define MOUSE_BUTTON_இடது 0x01
#define MOUSE_BUTTON_நடு 0x02
#define MOUSE_BUTTON_வலது 0x04
  முழு      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

வகைவரை கட்டமைப்பு {
  முழு (*control)(unsigned எழுத்து control, வெறுமை *user);
  முழு (*csi)(மாறிலி எழுத்து *leader, மாறிலி long args[], முழு argcount, மாறிலி எழுத்து *intermed, எழுத்து command, வெறுமை *user);
  முழு (*osc)(முழு command, VTermStringFragment frag, வெறுமை *user);
  முழு (*dcs)(மாறிலி எழுத்து *command, size_t commandlen, VTermStringFragment frag, வெறுமை *user);
  முழு (*apc)(VTermStringFragment frag, வெறுமை *user);
  முழு (*pm)(VTermStringFragment frag, வெறுமை *user);
  முழு (*sos)(VTermStringFragment frag, வெறுமை *user);
} VTermStateFallbacks;

வகைவரை கட்டமைப்பு {
  முழு (*set)(VTermSelectionMask mask, VTermStringFragment frag, வெறுமை *user);
  முழு (*query)(VTermSelectionMask mask, வெறுமை *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

வெறுமை  vterm_state_set_callbacks(VTermState *state, மாறிலி VTermStateCallbacks *callbacks, வெறுமை *user);
வெறுமை *vterm_state_get_cbdata(VTermState *state);

வெறுமை vterm_state_callbacks_has_premove(VTermState *state);

வெறுமை  vterm_state_set_unrecognised_fallbacks(VTermState *state, மாறிலி VTermStateFallbacks *fallbacks, வெறுமை *user);
வெறுமை *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
வெறுமை vterm_state_reset(VTermState *state, முழு hard);

வெறுமை vterm_state_get_cursorpos(மாறிலி VTermState *state, VTermPos *cursorpos);
// VIM: added
வெறுமை vterm_state_get_mousestate(மாறிலி VTermState *state, VTermMouseState *mousestate);
வெறுமை vterm_state_get_default_colors(மாறிலி VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
வெறுமை vterm_state_get_palette_color(மாறிலி VTermState *state, முழு index, VTermColor *col);
வெறுமை vterm_state_set_default_colors(VTermState *state, மாறிலி VTermColor *default_fg, மாறிலி VTermColor *default_bg);
வெறுமை vterm_state_set_palette_color(VTermState *state, முழு index, மாறிலி VTermColor *col);
வெறுமை vterm_state_set_bold_highbright(VTermState *state, முழு bold_is_highbright);
முழு  vterm_state_get_penattr(மாறிலி VTermState *state, VTermAttr attr, VTerm_மதிப்பு *val);
முழு  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_மதிப்பு *val);
வெறுமை vterm_state_focus_in(VTermState *state);
வெறுமை vterm_state_focus_out(VTermState *state);
மாறிலி VTerm_வரி_Info *vterm_state_get_lineinfo(மாறிலி VTermState *state, முழு row);

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
வெறுமை vterm_state_convert_color_to_rgb(மாறிலி VTermState *state, VTermColor *col);

வெறுமை vterm_state_set_selection_callbacks(VTermState *state, மாறிலி VTermSelectionCallbacks *callbacks, வெறுமை *user,
    எழுத்து *buffer, size_t buflen);

வெறுமை vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

வகைவரை கட்டமைப்பு {
    unsigned முழு bold      : 1;
    unsigned முழு underline : 2;
    unsigned முழு italic    : 1;
    unsigned முழு blink     : 1;
    unsigned முழு reverse   : 1;
    unsigned முழு conceal   : 1;
    unsigned முழு strike    : 1;
    unsigned முழு font      : 4; /* 0 to 9 */
    unsigned முழு dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned முழு dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned முழு small     : 1;
    unsigned முழு baseline  : 2;
} VTermScreenCellAttrs;

எண்ணிக்கை {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

எண்ணிக்கை {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

வகைவரை கட்டமைப்பு {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  எழுத்து     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
வகைவரை கட்டமைப்பு {
  முழு (*damage)(VTermRect rect, வெறுமை *user);
  முழு (*moverect)(VTermRect dest, VTermRect src, வெறுமை *user);
  முழு (*movecursor)(VTermPos pos, VTermPos oldpos, முழு visible, வெறுமை *user);
  முழு (*settermprop)(VTermProp prop, VTerm_மதிப்பு *val, வெறுமை *user);
  முழு (*bell)(வெறுமை *user);
  முழு (*resize)(முழு rows, முழு cols, வெறுமை *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  முழு (*sb_pushline)(முழு cols, மாறிலி VTermScreenCell *cells, வெறுமை *user);
  முழு (*sb_popline)(முழு cols, VTermScreenCell *cells, வெறுமை *user);
  முழு (*sb_clear)(வெறுமை* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  முழு (*sb_pushline4)(முழு cols, மாறிலி VTermScreenCell *cells, முழு continuation, வெறுமை *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
வெறுமை  vterm_screen_set_callbacks(VTermScreen *screen, மாறிலி VTermScreenCallbacks *callbacks, வெறுமை *user);
வெறுமை *vterm_screen_get_cbdata(VTermScreen *screen);

வெறுமை vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

வெறுமை  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, மாறிலி VTermStateFallbacks *fallbacks, வெறுமை *user);
வெறுமை *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

வெறுமை vterm_screen_enable_reflow(VTermScreen *screen, முழு reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
வெறுமை vterm_screen_enable_altscreen(VTermScreen *screen, முழு altscreen);

வகைவரை எண்ணிக்கை {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
வெறுமை vterm_screen_flush_damage(VTermScreen *screen);

வெறுமை vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
வெறுமை   vterm_screen_reset(VTermScreen *screen, முழு hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(மாறிலி VTermScreen *screen, uint32_t *chars, size_t len, மாறிலி VTermRect rect);
size_t vterm_screen_get_text(மாறிலி VTermScreen *screen, எழுத்து *str, size_t len, மாறிலி VTermRect rect);

வகைவரை எண்ணிக்கை {
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

முழு vterm_screen_get_attrs_extent(மாறிலி VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

முழு vterm_screen_get_cell(மாறிலி VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

முழு vterm_screen_is_eol(மாறிலி VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
வெறுமை vterm_screen_convert_color_to_rgb(மாறிலி VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
வெறுமை vterm_screen_set_default_colors(VTermScreen *screen, மாறிலி VTermColor *default_fg, மாறிலி VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_மதிப்பு_Type vterm_get_attr_type(VTermAttr attr);
VTerm_மதிப்பு_Type vterm_get_prop_type(VTermProp prop);

வெறுமை vterm_scroll_rect(VTermRect rect,
                       முழு downward,
                       முழு rightward,
                       முழு (*moverect)(VTermRect src, VTermRect dest, வெறுமை *user),
                       முழு (*eraserect)(VTermRect rect, முழு selective, வெறுமை *user),
                       வெறுமை *user);

வெறுமை vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      வெறுமை (*copycell)(VTermPos dest, VTermPos src, வெறுமை *user),
                      வெறுமை *user);

#ifdef __cplusplus
}
#endif

#endif
