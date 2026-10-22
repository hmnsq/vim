/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>

#include "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
typedef unsigned var_na		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned පූර්ණ_සංඛ්‍යා		uint32_t;

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

typedef sa_nracanaa VTerm VTerm;
typedef sa_nracanaa VTermState VTermState;
typedef sa_nracanaa VTermScreen VTermScreen;

// Specifies a screen point.
typedef sa_nracanaa {
  පූර්ණ_සංඛ්‍යා row;
  පූර්ණ_සංඛ්‍යා col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
පූර්ණ_සංඛ්‍යා vterm_pos_cmp(VTermPos a, VTermPos b);

#නම් defined(DEFINE_INLINES) || USE_INLINE
INLINE පූර්ණ_සංඛ්‍යා vterm_pos_cmp(VTermPos a, VTermPos b)
{
  ආපසු (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
typedef sa_nracanaa {
  පූර්ණ_සංඛ්‍යා start_row;
  පූර්ණ_සංඛ්‍යා end_row;
  පූර්ණ_සංඛ්‍යා start_col;
  පූර්ණ_සංඛ්‍යා end_col;
} VTermRect;

/* true if the rect contains the point */
පූර්ණ_සංඛ්‍යා vterm_rect_contains(VTermRect r, VTermPos p);

#නම් defined(DEFINE_INLINES) || USE_INLINE
INLINE පූර්ණ_සංඛ්‍යා vterm_rect_contains(VTermRect r, VTermPos p)
{
  ආපසු p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
void vterm_rect_move(VTermRect *rect, පූර්ණ_සංඛ්‍යා row_delta, පූර්ණ_සංඛ්‍යා col_delta);

#නම් defined(DEFINE_INLINES) || USE_INLINE
INLINE void vterm_rect_move(VTermRect *rect, පූර්ණ_සංඛ්‍යා row_delta, පූර්ණ_සංඛ්‍යා col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
typedef ga_nanaa {
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
typedef sa_nracanaa {
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
void vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
void vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
පූර්ණ_සංඛ්‍යා vterm_color_is_equal(const VTermColor *a, const VTermColor *b);

typedef ga_nanaa {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTermValueType;

typedef sa_nracanaa {
  const var_na *str;
  size_t      len : 30;
  unsigned පූර්ණ_සංඛ්‍යා  initial : 1;
  unsigned පූර්ණ_සංඛ්‍යා  final : 1;
} VTermStringFragment;

typedef sa_ngha {
  පූර්ණ_සංඛ්‍යා boolean;
  පූර්ණ_සංඛ්‍යා number;
  VTermStringFragment string;
  VTermColor color;
} VTermValue;

typedef ga_nanaa {
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

typedef ga_nanaa {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       // number
  VTERM_PROP_MOUSE,             // number
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

ga_nanaa {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_LEFT,

  VTERM_N_PROP_CURSORSHAPES
};

ga_nanaa {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

typedef ga_nanaa {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

typedef sa_nracanaa {
  const uint32_t *chars;
  පූර්ණ_සංඛ්‍යා             width;
  unsigned පූර්ණ_සංඛ්‍යා    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned පූර්ණ_සංඛ්‍යා    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned පූර්ණ_සංඛ්‍යා    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

typedef sa_nracanaa {
  unsigned පූර්ණ_සංඛ්‍යා    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned පූර්ණ_සංඛ්‍යා    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned පූර්ණ_සංඛ්‍යා    continuation:1;    /* Line is a flow continuation of the previous */
} VTermLineInfo;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
typedef sa_nracanaa {
  VTermPos pos;                /* current cursor position */
  VTermLineInfo *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

typedef sa_nracanaa {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  void *(*malloc)(size_t size, void *allocdata);
  void  (*free)(void *ptr, void *allocdata);
} VTermAllocatorFunctions;

void vterm_check_version(පූර්ණ_සංඛ්‍යා major, පූර්ණ_සංඛ්‍යා minor);

sa_nracanaa VTermBuilder {
  පූර්ණ_සංඛ්‍යා ver; /* currently unused but reserved for some sort of ABI version flag */

  පූර්ණ_සංඛ්‍යා rows, cols;

  const VTermAllocatorFunctions *allocator;
  void *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(const sa_nracanaa VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols, VTermAllocatorFunctions *funcs, void *allocdata);

// Free and cleanup a terminal and all its data.
void   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
void vterm_get_size(const VTerm *vt, පූර්ණ_සංඛ්‍යා *rowsp, පූර්ණ_සංඛ්‍යා *colsp);

void vterm_set_size(VTerm *vt, පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols);

පූර්ණ_සංඛ්‍යා  vterm_get_utf8(const VTerm *vt);
void vterm_set_utf8(VTerm *vt, පූර්ණ_සංඛ්‍යා is_utf8);

size_t vterm_input_write(VTerm *vt, const var_na *bytes, size_t len);

/* Setting output callback will override the buffer logic */
typedef void VTermOutputCallback(const var_na *s, size_t len, void *user);
void vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, void *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(const VTerm *vt);
size_t vterm_output_get_buffer_current(const VTerm *vt);
size_t vterm_output_get_buffer_remaining(const VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, var_na *buffer, size_t len);

පූර්ණ_සංඛ්‍යා vterm_is_modify_other_keys(VTerm *vt);
පූර්ණ_සංඛ්‍යා vterm_is_kitty_keyboard(VTerm *vt);
void vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
void vterm_keyboard_key(VTerm *vt, VTermKey key, VTermModifier mod);

void vterm_keyboard_start_paste(VTerm *vt);
void vterm_keyboard_end_paste(VTerm *vt);

void vterm_mouse_move(VTerm *vt, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
void vterm_mouse_button(VTerm *vt, පූර්ණ_සංඛ්‍යා button, පූර්ණ_සංඛ්‍යා pressed, VTermModifier mod);

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
#define CSI_ARG_COUNT(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

typedef sa_nracanaa {
  පූර්ණ_සංඛ්‍යා (*text)(const var_na *bytes, size_t len, void *user);
  පූර්ණ_සංඛ්‍යා (*control)(unsigned var_na control, void *user);
  පූර්ණ_සංඛ්‍යා (*escape)(const var_na *bytes, size_t len, void *user);
  පූර්ණ_සංඛ්‍යා (*csi)(const var_na *leader, const long args[], පූර්ණ_සංඛ්‍යා argcount, const var_na *intermed, var_na command, void *user);
  පූර්ණ_සංඛ්‍යා (*osc)(පූර්ණ_සංඛ්‍යා command, VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*dcs)(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*apc)(VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*pm)(VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*sos)(VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*resize)(පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols, void *user);
} VTermParserCallbacks;

void  vterm_parser_set_callbacks(VTerm *vt, const VTermParserCallbacks *callbacks, void *user);
void *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
void vterm_parser_set_emit_nul(VTerm *vt, පූර්ණ_සංඛ්‍යා emit);

// -----------
// State layer
// -----------

typedef sa_nracanaa {
  පූර්ණ_සංඛ්‍යා (*putglyph)(VTermGlyphInfo *info, VTermPos pos, void *user);
  පූර්ණ_සංඛ්‍යා (*movecursor)(VTermPos pos, VTermPos oldpos, පූර්ණ_සංඛ්‍යා visible, void *user);
  පූර්ණ_සංඛ්‍යා (*scrollrect)(VTermRect rect, පූර්ණ_සංඛ්‍යා downward, පූර්ණ_සංඛ්‍යා rightward, void *user);
  පූර්ණ_සංඛ්‍යා (*moverect)(VTermRect dest, VTermRect src, void *user);
  පූර්ණ_සංඛ්‍යා (*erase)(VTermRect rect, පූර්ණ_සංඛ්‍යා selective, void *user);
  පූර්ණ_සංඛ්‍යා (*initpen)(void *user);
  පූර්ණ_සංඛ්‍යා (*setpenattr)(VTermAttr attr, VTermValue *val, void *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  පූර්ණ_සංඛ්‍යා (*settermprop)(VTermProp prop, VTermValue *val, void *user);
  පූර්ණ_සංඛ්‍යා (*bell)(void *user);
  පූර්ණ_සංඛ්‍යා (*resize)(පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols, VTermStateFields *fields, void *user);
  පූර්ණ_සංඛ්‍යා (*setlineinfo)(පූර්ණ_සංඛ්‍යා row, const VTermLineInfo *newinfo, const VTermLineInfo *oldinfo, void *user);
  පූර්ණ_සංඛ්‍යා (*sb_clear)(void *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  පූර්ණ_සංඛ්‍යා (*premove)(VTermRect dest, void *user);
} VTermStateCallbacks;

// VIM: added
typedef sa_nracanaa {
  VTermPos pos;
  පූර්ණ_සංඛ්‍යා	   buttons;
#define MOUSE_BUTTON_LEFT 0x01
#define MOUSE_BUTTON_MIDDLE 0x02
#define MOUSE_BUTTON_RIGHT 0x04
  පූර්ණ_සංඛ්‍යා      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

typedef sa_nracanaa {
  පූර්ණ_සංඛ්‍යා (*control)(unsigned var_na control, void *user);
  පූර්ණ_සංඛ්‍යා (*csi)(const var_na *leader, const long args[], පූර්ණ_සංඛ්‍යා argcount, const var_na *intermed, var_na command, void *user);
  පූර්ණ_සංඛ්‍යා (*osc)(පූර්ණ_සංඛ්‍යා command, VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*dcs)(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*apc)(VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*pm)(VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*sos)(VTermStringFragment frag, void *user);
} VTermStateFallbacks;

typedef sa_nracanaa {
  පූර්ණ_සංඛ්‍යා (*set)(VTermSelectionMask mask, VTermStringFragment frag, void *user);
  පූර්ණ_සංඛ්‍යා (*query)(VTermSelectionMask mask, void *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

void  vterm_state_set_callbacks(VTermState *state, const VTermStateCallbacks *callbacks, void *user);
void *vterm_state_get_cbdata(VTermState *state);

void vterm_state_callbacks_has_premove(VTermState *state);

void  vterm_state_set_unrecognised_fallbacks(VTermState *state, const VTermStateFallbacks *fallbacks, void *user);
void *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
void vterm_state_reset(VTermState *state, පූර්ණ_සංඛ්‍යා hard);

void vterm_state_get_cursorpos(const VTermState *state, VTermPos *cursorpos);
// VIM: added
void vterm_state_get_mousestate(const VTermState *state, VTermMouseState *mousestate);
void vterm_state_get_default_colors(const VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
void vterm_state_get_palette_color(const VTermState *state, පූර්ණ_සංඛ්‍යා index, VTermColor *col);
void vterm_state_set_default_colors(VTermState *state, const VTermColor *default_fg, const VTermColor *default_bg);
void vterm_state_set_palette_color(VTermState *state, පූර්ණ_සංඛ්‍යා index, const VTermColor *col);
void vterm_state_set_bold_highbright(VTermState *state, පූර්ණ_සංඛ්‍යා bold_is_highbright);
පූර්ණ_සංඛ්‍යා  vterm_state_get_penattr(const VTermState *state, VTermAttr attr, VTermValue *val);
පූර්ණ_සංඛ්‍යා  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTermValue *val);
void vterm_state_focus_in(VTermState *state);
void vterm_state_focus_out(VTermState *state);
const VTermLineInfo *vterm_state_get_lineinfo(const VTermState *state, පූර්ණ_සංඛ්‍යා row);

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
void vterm_state_convert_color_to_rgb(const VTermState *state, VTermColor *col);

void vterm_state_set_selection_callbacks(VTermState *state, const VTermSelectionCallbacks *callbacks, void *user,
    var_na *buffer, size_t buflen);

void vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

typedef sa_nracanaa {
    unsigned පූර්ණ_සංඛ්‍යා bold      : 1;
    unsigned පූර්ණ_සංඛ්‍යා underline : 2;
    unsigned පූර්ණ_සංඛ්‍යා italic    : 1;
    unsigned පූර්ණ_සංඛ්‍යා blink     : 1;
    unsigned පූර්ණ_සංඛ්‍යා reverse   : 1;
    unsigned පූර්ණ_සංඛ්‍යා conceal   : 1;
    unsigned පූර්ණ_සංඛ්‍යා strike    : 1;
    unsigned පූර්ණ_සංඛ්‍යා font      : 4; /* 0 to 9 */
    unsigned පූර්ණ_සංඛ්‍යා dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned පූර්ණ_සංඛ්‍යා dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned පූර්ණ_සංඛ්‍යා small     : 1;
    unsigned පූර්ණ_සංඛ්‍යා baseline  : 2;
} VTermScreenCellAttrs;

ga_nanaa {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

ga_nanaa {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

typedef sa_nracanaa {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  var_na     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
typedef sa_nracanaa {
  පූර්ණ_සංඛ්‍යා (*damage)(VTermRect rect, void *user);
  පූර්ණ_සංඛ්‍යා (*moverect)(VTermRect dest, VTermRect src, void *user);
  පූර්ණ_සංඛ්‍යා (*movecursor)(VTermPos pos, VTermPos oldpos, පූර්ණ_සංඛ්‍යා visible, void *user);
  පූර්ණ_සංඛ්‍යා (*settermprop)(VTermProp prop, VTermValue *val, void *user);
  පූර්ණ_සංඛ්‍යා (*bell)(void *user);
  පූර්ණ_සංඛ්‍යා (*resize)(පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols, void *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  පූර්ණ_සංඛ්‍යා (*sb_pushline)(පූර්ණ_සංඛ්‍යා cols, const VTermScreenCell *cells, void *user);
  පූර්ණ_සංඛ්‍යා (*sb_popline)(පූර්ණ_සංඛ්‍යා cols, VTermScreenCell *cells, void *user);
  පූර්ණ_සංඛ්‍යා (*sb_clear)(void* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  පූර්ණ_සංඛ්‍යා (*sb_pushline4)(පූර්ණ_සංඛ්‍යා cols, const VTermScreenCell *cells, පූර්ණ_සංඛ්‍යා continuation, void *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
void  vterm_screen_set_callbacks(VTermScreen *screen, const VTermScreenCallbacks *callbacks, void *user);
void *vterm_screen_get_cbdata(VTermScreen *screen);

void vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

void  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, const VTermStateFallbacks *fallbacks, void *user);
void *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

void vterm_screen_enable_reflow(VTermScreen *screen, පූර්ණ_සංඛ්‍යා reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
void vterm_screen_enable_altscreen(VTermScreen *screen, පූර්ණ_සංඛ්‍යා altscreen);

typedef ga_nanaa {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
void vterm_screen_flush_damage(VTermScreen *screen);

void vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
void   vterm_screen_reset(VTermScreen *screen, පූර්ණ_සංඛ්‍යා hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(const VTermScreen *screen, uint32_t *chars, size_t len, const VTermRect rect);
size_t vterm_screen_get_text(const VTermScreen *screen, var_na *str, size_t len, const VTermRect rect);

typedef ga_nanaa {
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

පූර්ණ_සංඛ්‍යා vterm_screen_get_attrs_extent(const VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

පූර්ණ_සංඛ්‍යා vterm_screen_get_cell(const VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

පූර්ණ_සංඛ්‍යා vterm_screen_is_eol(const VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
void vterm_screen_convert_color_to_rgb(const VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
void vterm_screen_set_default_colors(VTermScreen *screen, const VTermColor *default_fg, const VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTermValueType vterm_get_attr_type(VTermAttr attr);
VTermValueType vterm_get_prop_type(VTermProp prop);

void vterm_scroll_rect(VTermRect rect,
                       පූර්ණ_සංඛ්‍යා downward,
                       පූර්ණ_සංඛ්‍යා rightward,
                       පූර්ණ_සංඛ්‍යා (*moverect)(VTermRect src, VTermRect dest, void *user),
                       පූර්ණ_සංඛ්‍යා (*eraserect)(VTermRect rect, පූර්ණ_සංඛ්‍යා selective, void *user),
                       void *user);

void vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      void (*copycell)(VTermPos dest, VTermPos src, void *user),
                      void *user);

#ifdef __cplusplus
}
#endif

#endif
