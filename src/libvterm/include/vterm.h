/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
بیرونی "C" {
#endif

#شامل <stdlib.h>

#شامل "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
تعریفنوع unsigned نویسه		uint8_t;
تعریفنوع unsigned short		uint16_t;
تعریفنوع unsigned صحیح		uint32_t;

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

تعریفنوع ساختار VTerm VTerm;
تعریفنوع ساختار VTermState VTermState;
تعریفنوع ساختار VTermScreen VTermScreen;

// Specifies a screen point.
تعریفنوع ساختار {
  صحیح row;
  صحیح col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
صحیح vterm_pos_cmp(VTermPos a, VTermPos b);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE صحیح vterm_pos_cmp(VTermPos a, VTermPos b)
{
  بازگشت (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
تعریفنوع ساختار {
  صحیح start_row;
  صحیح end_row;
  صحیح start_col;
  صحیح end_col;
} VTermRect;

/* true if the rect contains the point */
صحیح vterm_rect_contains(VTermRect r, VTermPos p);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE صحیح vterm_rect_contains(VTermRect r, VTermPos p)
{
  بازگشت p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
تهی vterm_rect_move(VTermRect *rect, صحیح row_delta, صحیح col_delta);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE تهی vterm_rect_move(VTermRect *rect, صحیح row_delta, صحیح col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
تعریفنوع شمارش {
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
تعریفنوع ساختار {
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
تهی vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
تهی vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
صحیح vterm_color_is_equal(ثابت VTermColor *a, ثابت VTermColor *b);

تعریفنوع شمارش {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_مقدار_Type;

تعریفنوع ساختار {
  ثابت نویسه *str;
  size_t      len : 30;
  unsigned صحیح  initial : 1;
  unsigned صحیح  final : 1;
} VTermStringFragment;

تعریفنوع اجتماع {
  صحیح boolean;
  صحیح عدد;
  VTermStringFragment string;
  VTermColor color;
} VTerm_مقدار;

تعریفنوع شمارش {
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

تعریفنوع شمارش {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //عدد
  VTERM_PROP_MOUSE,             //عدد
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

شمارش {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_چپ,

  VTERM_N_PROP_CURSORSHAPES
};

شمارش {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

تعریفنوع شمارش {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

تعریفنوع ساختار {
  ثابت uint32_t *chars;
  صحیح             width;
  unsigned صحیح    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned صحیح    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned صحیح    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

تعریفنوع ساختار {
  unsigned صحیح    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned صحیح    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned صحیح    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_سطر_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
تعریفنوع ساختار {
  VTermPos pos;                /* current cursor position */
  VTerm_سطر_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

تعریفنوع ساختار {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  تهی *(*malloc)(size_t size, تهی *allocdata);
  تهی  (*آزاد)(تهی *ptr, تهی *allocdata);
} VTermAllocatorFunctions;

تهی vterm_check_version(صحیح major, صحیح minor);

ساختار VTermBuilder {
  صحیح ver; /* currently unused but reserved for some sort of ABI version flag */

  صحیح rows, cols;

  ثابت VTermAllocatorFunctions *allocator;
  تهی *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ثابت ساختار VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(صحیح rows, صحیح cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(صحیح rows, صحیح cols, VTermAllocatorFunctions *funcs, تهی *allocdata);

// Free and cleanup a terminal and all its data.
تهی   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
تهی vterm_get_size(ثابت VTerm *vt, صحیح *rowsp, صحیح *colsp);

تهی vterm_set_size(VTerm *vt, صحیح rows, صحیح cols);

صحیح  vterm_get_utf8(ثابت VTerm *vt);
تهی vterm_set_utf8(VTerm *vt, صحیح is_utf8);

size_t vterm_درون_داد_write(VTerm *vt, ثابت نویسه *bytes, size_t len);

/* Setting output callback will override the buffer logic */
تعریفنوع تهی VTermOutputCallback(ثابت نویسه *s, size_t len, تهی *user);
تهی vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, تهی *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(ثابت VTerm *vt);
size_t vterm_output_get_buffer_current(ثابت VTerm *vt);
size_t vterm_output_get_buffer_remaining(ثابت VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, نویسه *buffer, size_t len);

صحیح vterm_is_modify_other_کلیدها(VTerm *vt);
صحیح vterm_is_kitty_keyboard(VTerm *vt);
تهی vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
تهی vterm_keyboard_کلید(VTerm *vt, VTerm_کلید کلید, VTermModifier mod);

تهی vterm_keyboard_start_paste(VTerm *vt);
تهی vterm_keyboard_end_paste(VTerm *vt);

تهی vterm_mouse_move(VTerm *vt, صحیح row, صحیح col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
تهی vterm_mouse_button(VTerm *vt, صحیح button, صحیح pressed, VTermModifier mod);

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
#define CSI_ARG_شمار(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

تعریفنوع ساختار {
  صحیح (*text)(ثابت نویسه *bytes, size_t len, تهی *user);
  صحیح (*control)(unsigned نویسه control, تهی *user);
  صحیح (*escape)(ثابت نویسه *bytes, size_t len, تهی *user);
  صحیح (*csi)(ثابت نویسه *leader, ثابت long args[], صحیح argcount, ثابت نویسه *intermed, نویسه command, تهی *user);
  صحیح (*osc)(صحیح command, VTermStringFragment frag, تهی *user);
  صحیح (*dcs)(ثابت نویسه *command, size_t commandlen, VTermStringFragment frag, تهی *user);
  صحیح (*apc)(VTermStringFragment frag, تهی *user);
  صحیح (*pm)(VTermStringFragment frag, تهی *user);
  صحیح (*sos)(VTermStringFragment frag, تهی *user);
  صحیح (*resize)(صحیح rows, صحیح cols, تهی *user);
} VTermParserCallbacks;

تهی  vterm_parser_set_callbacks(VTerm *vt, ثابت VTermParserCallbacks *callbacks, تهی *user);
تهی *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
تهی vterm_parser_set_emit_nul(VTerm *vt, صحیح emit);

// -----------
// State layer
// -----------

تعریفنوع ساختار {
  صحیح (*putglyph)(VTermGlyphInfo *info, VTermPos pos, تهی *user);
  صحیح (*movecursor)(VTermPos pos, VTermPos oldpos, صحیح visible, تهی *user);
  صحیح (*scrollrect)(VTermRect rect, صحیح downward, صحیح rightward, تهی *user);
  صحیح (*moverect)(VTermRect dest, VTermRect src, تهی *user);
  صحیح (*erase)(VTermRect rect, صحیح selective, تهی *user);
  صحیح (*initpen)(تهی *user);
  صحیح (*setpenattr)(VTermAttr attr, VTerm_مقدار *val, تهی *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  صحیح (*settermprop)(VTermProp prop, VTerm_مقدار *val, تهی *user);
  صحیح (*bell)(تهی *user);
  صحیح (*resize)(صحیح rows, صحیح cols, VTermStateFields *fields, تهی *user);
  صحیح (*setlineinfo)(صحیح row, ثابت VTerm_سطر_Info *newinfo, ثابت VTerm_سطر_Info *oldinfo, تهی *user);
  صحیح (*sb_clear)(تهی *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  صحیح (*premove)(VTermRect dest, تهی *user);
} VTermStateCallbacks;

// VIM: added
تعریفنوع ساختار {
  VTermPos pos;
  صحیح	   buttons;
#define MOUSE_BUTTON_چپ 0x01
#define MOUSE_BUTTON_میانه 0x02
#define MOUSE_BUTTON_راست 0x04
  صحیح      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

تعریفنوع ساختار {
  صحیح (*control)(unsigned نویسه control, تهی *user);
  صحیح (*csi)(ثابت نویسه *leader, ثابت long args[], صحیح argcount, ثابت نویسه *intermed, نویسه command, تهی *user);
  صحیح (*osc)(صحیح command, VTermStringFragment frag, تهی *user);
  صحیح (*dcs)(ثابت نویسه *command, size_t commandlen, VTermStringFragment frag, تهی *user);
  صحیح (*apc)(VTermStringFragment frag, تهی *user);
  صحیح (*pm)(VTermStringFragment frag, تهی *user);
  صحیح (*sos)(VTermStringFragment frag, تهی *user);
} VTermStateFallbacks;

تعریفنوع ساختار {
  صحیح (*set)(VTermSelectionMask mask, VTermStringFragment frag, تهی *user);
  صحیح (*query)(VTermSelectionMask mask, تهی *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

تهی  vterm_state_set_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, تهی *user);
تهی *vterm_state_get_cbdata(VTermState *state);

تهی vterm_state_callbacks_has_premove(VTermState *state);

تهی  vterm_state_set_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, تهی *user);
تهی *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
تهی vterm_state_reset(VTermState *state, صحیح hard);

تهی vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos);
// VIM: added
تهی vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate);
تهی vterm_state_get_default_colors(ثابت VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
تهی vterm_state_get_palette_color(ثابت VTermState *state, صحیح index, VTermColor *col);
تهی vterm_state_set_default_colors(VTermState *state, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg);
تهی vterm_state_set_palette_color(VTermState *state, صحیح index, ثابت VTermColor *col);
تهی vterm_state_set_bold_highbright(VTermState *state, صحیح bold_is_highbright);
صحیح  vterm_state_get_penattr(ثابت VTermState *state, VTermAttr attr, VTerm_مقدار *val);
صحیح  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_مقدار *val);
تهی vterm_state_focus_in(VTermState *state);
تهی vterm_state_focus_out(VTermState *state);
ثابت VTerm_سطر_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحیح row);

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
تهی vterm_state_convert_color_to_rgb(ثابت VTermState *state, VTermColor *col);

تهی vterm_state_set_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, تهی *user,
    نویسه *buffer, size_t buflen);

تهی vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

تعریفنوع ساختار {
    unsigned صحیح bold      : 1;
    unsigned صحیح underline : 2;
    unsigned صحیح italic    : 1;
    unsigned صحیح blink     : 1;
    unsigned صحیح reverse   : 1;
    unsigned صحیح conceal   : 1;
    unsigned صحیح strike    : 1;
    unsigned صحیح font      : 4; /* 0 to 9 */
    unsigned صحیح dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned صحیح dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned صحیح small     : 1;
    unsigned صحیح baseline  : 2;
} VTermScreenCellAttrs;

شمارش {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

شمارش {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

تعریفنوع ساختار {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  نویسه     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
تعریفنوع ساختار {
  صحیح (*damage)(VTermRect rect, تهی *user);
  صحیح (*moverect)(VTermRect dest, VTermRect src, تهی *user);
  صحیح (*movecursor)(VTermPos pos, VTermPos oldpos, صحیح visible, تهی *user);
  صحیح (*settermprop)(VTermProp prop, VTerm_مقدار *val, تهی *user);
  صحیح (*bell)(تهی *user);
  صحیح (*resize)(صحیح rows, صحیح cols, تهی *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  صحیح (*sb_pushline)(صحیح cols, ثابت VTermScreenCell *cells, تهی *user);
  صحیح (*sb_popline)(صحیح cols, VTermScreenCell *cells, تهی *user);
  صحیح (*sb_clear)(تهی* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  صحیح (*sb_pushline4)(صحیح cols, ثابت VTermScreenCell *cells, صحیح continuation, تهی *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
تهی  vterm_screen_set_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, تهی *user);
تهی *vterm_screen_get_cbdata(VTermScreen *screen);

تهی vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

تهی  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, تهی *user);
تهی *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

تهی vterm_screen_enable_reflow(VTermScreen *screen, صحیح reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
تهی vterm_screen_enable_altscreen(VTermScreen *screen, صحیح altscreen);

تعریفنوع شمارش {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
تهی vterm_screen_flush_damage(VTermScreen *screen);

تهی vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
تهی   vterm_screen_reset(VTermScreen *screen, صحیح hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect);
size_t vterm_screen_get_text(ثابت VTermScreen *screen, نویسه *str, size_t len, ثابت VTermRect rect);

تعریفنوع شمارش {
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

صحیح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

صحیح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

صحیح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
تهی vterm_screen_convert_color_to_rgb(ثابت VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
تهی vterm_screen_set_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_مقدار_Type vterm_get_attr_type(VTermAttr attr);
VTerm_مقدار_Type vterm_get_prop_type(VTermProp prop);

تهی vterm_scroll_rect(VTermRect rect,
                       صحیح downward,
                       صحیح rightward,
                       صحیح (*moverect)(VTermRect src, VTermRect dest, تهی *user),
                       صحیح (*eraserect)(VTermRect rect, صحیح selective, تهی *user),
                       تهی *user);

تهی vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      تهی (*copycell)(VTermPos dest, VTermPos src, تهی *user),
                      تهی *user);

#ifdef __cplusplus
}
#endif

#endif
