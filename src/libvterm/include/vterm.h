/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
બાહ્ય "C" {
#endif

#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
પ્રકારવ્યાખ્યા unsigned અક્ષર		uint8_t;
પ્રકારવ્યાખ્યા unsigned short		uint16_t;
પ્રકારવ્યાખ્યા unsigned પૂર્ણાંક		uint32_t;

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

પ્રકારવ્યાખ્યા સંરચના VTerm VTerm;
પ્રકારવ્યાખ્યા સંરચના VTermState VTermState;
પ્રકારવ્યાખ્યા સંરચના VTermScreen VTermScreen;

// Specifies a screen point.
પ્રકારવ્યાખ્યા સંરચના {
  પૂર્ણાંક row;
  પૂર્ણાંક col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
પૂર્ણાંક vterm_pos_cmp(VTermPos a, VTermPos b);

#જો defined(DEFINE_INLINES) || USE_INLINE
INLINE પૂર્ણાંક vterm_pos_cmp(VTermPos a, VTermPos b)
{
  પાછા (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
પ્રકારવ્યાખ્યા સંરચના {
  પૂર્ણાંક start_row;
  પૂર્ણાંક end_row;
  પૂર્ણાંક start_col;
  પૂર્ણાંક end_col;
} VTermRect;

/* true if the rect contains the point */
પૂર્ણાંક vterm_rect_contains(VTermRect r, VTermPos p);

#જો defined(DEFINE_INLINES) || USE_INLINE
INLINE પૂર્ણાંક vterm_rect_contains(VTermRect r, VTermPos p)
{
  પાછા p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
શૂન્ય vterm_rect_move(VTermRect *rect, પૂર્ણાંક row_delta, પૂર્ણાંક col_delta);

#જો defined(DEFINE_INLINES) || USE_INLINE
INLINE શૂન્ય vterm_rect_move(VTermRect *rect, પૂર્ણાંક row_delta, પૂર્ણાંક col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
પ્રકારવ્યાખ્યા ગણના {
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
પ્રકારવ્યાખ્યા સંરચના {
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
શૂન્ય vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
શૂન્ય vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
પૂર્ણાંક vterm_color_is_equal(અચલ VTermColor *a, અચલ VTermColor *b);

પ્રકારવ્યાખ્યા ગણના {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_કિંમત_Type;

પ્રકારવ્યાખ્યા સંરચના {
  અચલ અક્ષર *str;
  size_t      len : 30;
  unsigned પૂર્ણાંક  initial : 1;
  unsigned પૂર્ણાંક  final : 1;
} VTermStringFragment;

પ્રકારવ્યાખ્યા સંઘ {
  પૂર્ણાંક boolean;
  પૂર્ણાંક સંખ્યા;
  VTermStringFragment string;
  VTermColor color;
} VTerm_કિંમત;

પ્રકારવ્યાખ્યા ગણના {
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

પ્રકારવ્યાખ્યા ગણના {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //સંખ્યા
  VTERM_PROP_MOUSE,             //સંખ્યા
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

ગણના {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_ડાબું,

  VTERM_N_PROP_CURSORSHAPES
};

ગણના {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

પ્રકારવ્યાખ્યા ગણના {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

પ્રકારવ્યાખ્યા સંરચના {
  અચલ uint32_t *chars;
  પૂર્ણાંક             width;
  unsigned પૂર્ણાંક    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned પૂર્ણાંક    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned પૂર્ણાંક    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

પ્રકારવ્યાખ્યા સંરચના {
  unsigned પૂર્ણાંક    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned પૂર્ણાંક    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned પૂર્ણાંક    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_લીટી_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
પ્રકારવ્યાખ્યા સંરચના {
  VTermPos pos;                /* current cursor position */
  VTerm_લીટી_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

પ્રકારવ્યાખ્યા સંરચના {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  શૂન્ય *(*malloc)(size_t size, શૂન્ય *allocdata);
  શૂન્ય  (*મુક્ત)(શૂન્ય *ptr, શૂન્ય *allocdata);
} VTermAllocatorFunctions;

શૂન્ય vterm_check_version(પૂર્ણાંક major, પૂર્ણાંક minor);

સંરચના VTermBuilder {
  પૂર્ણાંક ver; /* currently unused but reserved for some sort of ABI version flag */

  પૂર્ણાંક rows, cols;

  અચલ VTermAllocatorFunctions *allocator;
  શૂન્ય *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(અચલ સંરચના VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(પૂર્ણાંક rows, પૂર્ણાંક cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(પૂર્ણાંક rows, પૂર્ણાંક cols, VTermAllocatorFunctions *funcs, શૂન્ય *allocdata);

// Free and cleanup a terminal and all its data.
શૂન્ય   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
શૂન્ય vterm_get_size(અચલ VTerm *vt, પૂર્ણાંક *rowsp, પૂર્ણાંક *colsp);

શૂન્ય vterm_set_size(VTerm *vt, પૂર્ણાંક rows, પૂર્ણાંક cols);

પૂર્ણાંક  vterm_get_utf8(અચલ VTerm *vt);
શૂન્ય vterm_set_utf8(VTerm *vt, પૂર્ણાંક is_utf8);

size_t vterm_આગત_write(VTerm *vt, અચલ અક્ષર *bytes, size_t len);

/* Setting output callback will override the buffer logic */
પ્રકારવ્યાખ્યા શૂન્ય VTermOutputCallback(અચલ અક્ષર *s, size_t len, શૂન્ય *user);
શૂન્ય vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, શૂન્ય *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(અચલ VTerm *vt);
size_t vterm_output_get_buffer_current(અચલ VTerm *vt);
size_t vterm_output_get_buffer_remaining(અચલ VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, અક્ષર *buffer, size_t len);

પૂર્ણાંક vterm_is_modify_other_ચાવીઓ(VTerm *vt);
પૂર્ણાંક vterm_is_kitty_keyboard(VTerm *vt);
શૂન્ય vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
શૂન્ય vterm_keyboard_ચાવી(VTerm *vt, VTerm_ચાવી ચાવી, VTermModifier mod);

શૂન્ય vterm_keyboard_start_paste(VTerm *vt);
શૂન્ય vterm_keyboard_end_paste(VTerm *vt);

શૂન્ય vterm_mouse_move(VTerm *vt, પૂર્ણાંક row, પૂર્ણાંક col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
શૂન્ય vterm_mouse_button(VTerm *vt, પૂર્ણાંક button, પૂર્ણાંક pressed, VTermModifier mod);

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
#define CSI_ARG_ગણતરી(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

પ્રકારવ્યાખ્યા સંરચના {
  પૂર્ણાંક (*text)(અચલ અક્ષર *bytes, size_t len, શૂન્ય *user);
  પૂર્ણાંક (*control)(unsigned અક્ષર control, શૂન્ય *user);
  પૂર્ણાંક (*escape)(અચલ અક્ષર *bytes, size_t len, શૂન્ય *user);
  પૂર્ણાંક (*csi)(અચલ અક્ષર *leader, અચલ long args[], પૂર્ણાંક argcount, અચલ અક્ષર *intermed, અક્ષર command, શૂન્ય *user);
  પૂર્ણાંક (*osc)(પૂર્ણાંક command, VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*dcs)(અચલ અક્ષર *command, size_t commandlen, VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*apc)(VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*pm)(VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*sos)(VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*resize)(પૂર્ણાંક rows, પૂર્ણાંક cols, શૂન્ય *user);
} VTermParserCallbacks;

શૂન્ય  vterm_parser_set_callbacks(VTerm *vt, અચલ VTermParserCallbacks *callbacks, શૂન્ય *user);
શૂન્ય *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
શૂન્ય vterm_parser_set_emit_nul(VTerm *vt, પૂર્ણાંક emit);

// -----------
// State layer
// -----------

પ્રકારવ્યાખ્યા સંરચના {
  પૂર્ણાંક (*putglyph)(VTermGlyphInfo *info, VTermPos pos, શૂન્ય *user);
  પૂર્ણાંક (*movecursor)(VTermPos pos, VTermPos oldpos, પૂર્ણાંક visible, શૂન્ય *user);
  પૂર્ણાંક (*scrollrect)(VTermRect rect, પૂર્ણાંક downward, પૂર્ણાંક rightward, શૂન્ય *user);
  પૂર્ણાંક (*moverect)(VTermRect dest, VTermRect src, શૂન્ય *user);
  પૂર્ણાંક (*erase)(VTermRect rect, પૂર્ણાંક selective, શૂન્ય *user);
  પૂર્ણાંક (*initpen)(શૂન્ય *user);
  પૂર્ણાંક (*setpenattr)(VTermAttr attr, VTerm_કિંમત *val, શૂન્ય *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  પૂર્ણાંક (*settermprop)(VTermProp prop, VTerm_કિંમત *val, શૂન્ય *user);
  પૂર્ણાંક (*bell)(શૂન્ય *user);
  પૂર્ણાંક (*resize)(પૂર્ણાંક rows, પૂર્ણાંક cols, VTermStateFields *fields, શૂન્ય *user);
  પૂર્ણાંક (*setlineinfo)(પૂર્ણાંક row, અચલ VTerm_લીટી_Info *newinfo, અચલ VTerm_લીટી_Info *oldinfo, શૂન્ય *user);
  પૂર્ણાંક (*sb_clear)(શૂન્ય *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  પૂર્ણાંક (*premove)(VTermRect dest, શૂન્ય *user);
} VTermStateCallbacks;

// VIM: added
પ્રકારવ્યાખ્યા સંરચના {
  VTermPos pos;
  પૂર્ણાંક	   buttons;
#define MOUSE_BUTTON_ડાબું 0x01
#define MOUSE_BUTTON_મધ્ય 0x02
#define MOUSE_BUTTON_જમણું 0x04
  પૂર્ણાંક      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

પ્રકારવ્યાખ્યા સંરચના {
  પૂર્ણાંક (*control)(unsigned અક્ષર control, શૂન્ય *user);
  પૂર્ણાંક (*csi)(અચલ અક્ષર *leader, અચલ long args[], પૂર્ણાંક argcount, અચલ અક્ષર *intermed, અક્ષર command, શૂન્ય *user);
  પૂર્ણાંક (*osc)(પૂર્ણાંક command, VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*dcs)(અચલ અક્ષર *command, size_t commandlen, VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*apc)(VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*pm)(VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*sos)(VTermStringFragment frag, શૂન્ય *user);
} VTermStateFallbacks;

પ્રકારવ્યાખ્યા સંરચના {
  પૂર્ણાંક (*set)(VTermSelectionMask mask, VTermStringFragment frag, શૂન્ય *user);
  પૂર્ણાંક (*query)(VTermSelectionMask mask, શૂન્ય *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

શૂન્ય  vterm_state_set_callbacks(VTermState *state, અચલ VTermStateCallbacks *callbacks, શૂન્ય *user);
શૂન્ય *vterm_state_get_cbdata(VTermState *state);

શૂન્ય vterm_state_callbacks_has_premove(VTermState *state);

શૂન્ય  vterm_state_set_unrecognised_fallbacks(VTermState *state, અચલ VTermStateFallbacks *fallbacks, શૂન્ય *user);
શૂન્ય *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
શૂન્ય vterm_state_reset(VTermState *state, પૂર્ણાંક hard);

શૂન્ય vterm_state_get_cursorpos(અચલ VTermState *state, VTermPos *cursorpos);
// VIM: added
શૂન્ય vterm_state_get_mousestate(અચલ VTermState *state, VTermMouseState *mousestate);
શૂન્ય vterm_state_get_default_colors(અચલ VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
શૂન્ય vterm_state_get_palette_color(અચલ VTermState *state, પૂર્ણાંક index, VTermColor *col);
શૂન્ય vterm_state_set_default_colors(VTermState *state, અચલ VTermColor *default_fg, અચલ VTermColor *default_bg);
શૂન્ય vterm_state_set_palette_color(VTermState *state, પૂર્ણાંક index, અચલ VTermColor *col);
શૂન્ય vterm_state_set_bold_highbright(VTermState *state, પૂર્ણાંક bold_is_highbright);
પૂર્ણાંક  vterm_state_get_penattr(અચલ VTermState *state, VTermAttr attr, VTerm_કિંમત *val);
પૂર્ણાંક  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_કિંમત *val);
શૂન્ય vterm_state_focus_in(VTermState *state);
શૂન્ય vterm_state_focus_out(VTermState *state);
અચલ VTerm_લીટી_Info *vterm_state_get_lineinfo(અચલ VTermState *state, પૂર્ણાંક row);

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
શૂન્ય vterm_state_convert_color_to_rgb(અચલ VTermState *state, VTermColor *col);

શૂન્ય vterm_state_set_selection_callbacks(VTermState *state, અચલ VTermSelectionCallbacks *callbacks, શૂન્ય *user,
    અક્ષર *buffer, size_t buflen);

શૂન્ય vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

પ્રકારવ્યાખ્યા સંરચના {
    unsigned પૂર્ણાંક bold      : 1;
    unsigned પૂર્ણાંક underline : 2;
    unsigned પૂર્ણાંક italic    : 1;
    unsigned પૂર્ણાંક blink     : 1;
    unsigned પૂર્ણાંક reverse   : 1;
    unsigned પૂર્ણાંક conceal   : 1;
    unsigned પૂર્ણાંક strike    : 1;
    unsigned પૂર્ણાંક font      : 4; /* 0 to 9 */
    unsigned પૂર્ણાંક dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned પૂર્ણાંક dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned પૂર્ણાંક small     : 1;
    unsigned પૂર્ણાંક baseline  : 2;
} VTermScreenCellAttrs;

ગણના {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

ગણના {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

પ્રકારવ્યાખ્યા સંરચના {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  અક્ષર     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
પ્રકારવ્યાખ્યા સંરચના {
  પૂર્ણાંક (*damage)(VTermRect rect, શૂન્ય *user);
  પૂર્ણાંક (*moverect)(VTermRect dest, VTermRect src, શૂન્ય *user);
  પૂર્ણાંક (*movecursor)(VTermPos pos, VTermPos oldpos, પૂર્ણાંક visible, શૂન્ય *user);
  પૂર્ણાંક (*settermprop)(VTermProp prop, VTerm_કિંમત *val, શૂન્ય *user);
  પૂર્ણાંક (*bell)(શૂન્ય *user);
  પૂર્ણાંક (*resize)(પૂર્ણાંક rows, પૂર્ણાંક cols, શૂન્ય *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  પૂર્ણાંક (*sb_pushline)(પૂર્ણાંક cols, અચલ VTermScreenCell *cells, શૂન્ય *user);
  પૂર્ણાંક (*sb_popline)(પૂર્ણાંક cols, VTermScreenCell *cells, શૂન્ય *user);
  પૂર્ણાંક (*sb_clear)(શૂન્ય* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  પૂર્ણાંક (*sb_pushline4)(પૂર્ણાંક cols, અચલ VTermScreenCell *cells, પૂર્ણાંક continuation, શૂન્ય *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
શૂન્ય  vterm_screen_set_callbacks(VTermScreen *screen, અચલ VTermScreenCallbacks *callbacks, શૂન્ય *user);
શૂન્ય *vterm_screen_get_cbdata(VTermScreen *screen);

શૂન્ય vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

શૂન્ય  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, અચલ VTermStateFallbacks *fallbacks, શૂન્ય *user);
શૂન્ય *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

શૂન્ય vterm_screen_enable_reflow(VTermScreen *screen, પૂર્ણાંક reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
શૂન્ય vterm_screen_enable_altscreen(VTermScreen *screen, પૂર્ણાંક altscreen);

પ્રકારવ્યાખ્યા ગણના {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
શૂન્ય vterm_screen_flush_damage(VTermScreen *screen);

શૂન્ય vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
શૂન્ય   vterm_screen_reset(VTermScreen *screen, પૂર્ણાંક hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(અચલ VTermScreen *screen, uint32_t *chars, size_t len, અચલ VTermRect rect);
size_t vterm_screen_get_text(અચલ VTermScreen *screen, અક્ષર *str, size_t len, અચલ VTermRect rect);

પ્રકારવ્યાખ્યા ગણના {
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

પૂર્ણાંક vterm_screen_get_attrs_extent(અચલ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

પૂર્ણાંક vterm_screen_get_cell(અચલ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

પૂર્ણાંક vterm_screen_is_eol(અચલ VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
શૂન્ય vterm_screen_convert_color_to_rgb(અચલ VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
શૂન્ય vterm_screen_set_default_colors(VTermScreen *screen, અચલ VTermColor *default_fg, અચલ VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_કિંમત_Type vterm_get_attr_type(VTermAttr attr);
VTerm_કિંમત_Type vterm_get_prop_type(VTermProp prop);

શૂન્ય vterm_scroll_rect(VTermRect rect,
                       પૂર્ણાંક downward,
                       પૂર્ણાંક rightward,
                       પૂર્ણાંક (*moverect)(VTermRect src, VTermRect dest, શૂન્ય *user),
                       પૂર્ણાંક (*eraserect)(VTermRect rect, પૂર્ણાંક selective, શૂન્ય *user),
                       શૂન્ય *user);

શૂન્ય vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      શૂન્ય (*copycell)(VTermPos dest, VTermPos src, શૂન્ય *user),
                      શૂન્ય *user);

#ifdef __cplusplus
}
#endif

#endif
