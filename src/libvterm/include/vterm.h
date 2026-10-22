/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
बाह्य "C" {
#endif

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
प्रकारव्याख्या अचिन्हित अक्षर		uint8_t;
प्रकारव्याख्या अचिन्हित लघु		uint16_t;
प्रकारव्याख्या अचिन्हित पूर्णांक		uint32_t;

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

प्रकारव्याख्या संरचना VTerm VTerm;
प्रकारव्याख्या संरचना VTermState VTermState;
प्रकारव्याख्या संरचना VTermScreen VTermScreen;

// Specifies a screen point.
प्रकारव्याख्या संरचना {
  पूर्णांक row;
  पूर्णांक col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
पूर्णांक vterm_pos_cmp(VTermPos a, VTermPos b);

#जर defined(DEFINE_INLINES) || USE_INLINE
INLINE पूर्णांक vterm_pos_cmp(VTermPos a, VTermPos b)
{
  परत (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
प्रकारव्याख्या संरचना {
  पूर्णांक start_row;
  पूर्णांक end_row;
  पूर्णांक start_col;
  पूर्णांक end_col;
} VTermRect;

/* true if the rect contains the point */
पूर्णांक vterm_rect_contains(VTermRect r, VTermPos p);

#जर defined(DEFINE_INLINES) || USE_INLINE
INLINE पूर्णांक vterm_rect_contains(VTermRect r, VTermPos p)
{
  परत p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
शून्य vterm_rect_move(VTermRect *rect, पूर्णांक row_delta, पूर्णांक col_delta);

#जर defined(DEFINE_INLINES) || USE_INLINE
INLINE शून्य vterm_rect_move(VTermRect *rect, पूर्णांक row_delta, पूर्णांक col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
प्रकारव्याख्या गणना {
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
प्रकारव्याख्या संरचना {
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
शून्य vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
शून्य vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
पूर्णांक vterm_color_is_equal(स्थिरांक VTermColor *a, स्थिरांक VTermColor *b);

प्रकारव्याख्या गणना {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_मूल्य_Type;

प्रकारव्याख्या संरचना {
  स्थिरांक अक्षर *str;
  माप_प्रकार      len : 30;
  अचिन्हित पूर्णांक  initial : 1;
  अचिन्हित पूर्णांक  final : 1;
} VTermStringFragment;

प्रकारव्याख्या संघ {
  पूर्णांक boolean;
  पूर्णांक संख्या;
  VTermStringFragment string;
  VTermColor color;
} VTerm_मूल्य;

प्रकारव्याख्या गणना {
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

प्रकारव्याख्या गणना {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //संख्या
  VTERM_PROP_MOUSE,             //संख्या
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

गणना {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_डावा,

  VTERM_N_PROP_CURSORSHAPES
};

गणना {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

प्रकारव्याख्या गणना {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

प्रकारव्याख्या संरचना {
  स्थिरांक uint32_t *chars;
  पूर्णांक             width;
  अचिन्हित पूर्णांक    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  अचिन्हित पूर्णांक    dwl:1;             /* DECDWL or DECDHL double-width line */
  अचिन्हित पूर्णांक    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

प्रकारव्याख्या संरचना {
  अचिन्हित पूर्णांक    doublewidth:1;     /* DECDWL or DECDHL line */
  अचिन्हित पूर्णांक    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  अचिन्हित पूर्णांक    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_ओळ_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
प्रकारव्याख्या संरचना {
  VTermPos pos;                /* current cursor position */
  VTerm_ओळ_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

प्रकारव्याख्या संरचना {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  शून्य *(*दो_स्मृति)(माप_प्रकार size, शून्य *allocdata);
  शून्य  (*मुक्त)(शून्य *ptr, शून्य *allocdata);
} VTermAllocatorFunctions;

शून्य vterm_check_version(पूर्णांक major, पूर्णांक minor);

संरचना VTermBuilder {
  पूर्णांक ver; /* currently unused but reserved for some sort of ABI version flag */

  पूर्णांक rows, cols;

  स्थिरांक VTermAllocatorFunctions *allocator;
  शून्य *allocdata;

  /* Override default sizes for various structures */
  माप_प्रकार outbuffer_len;  /* default: 4096 */
  माप_प्रकार tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(स्थिरांक संरचना VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(पूर्णांक rows, पूर्णांक cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(पूर्णांक rows, पूर्णांक cols, VTermAllocatorFunctions *funcs, शून्य *allocdata);

// Free and cleanup a terminal and all its data.
शून्य   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
शून्य vterm_get_size(स्थिरांक VTerm *vt, पूर्णांक *rowsp, पूर्णांक *colsp);

शून्य vterm_set_size(VTerm *vt, पूर्णांक rows, पूर्णांक cols);

पूर्णांक  vterm_get_utf8(स्थिरांक VTerm *vt);
शून्य vterm_set_utf8(VTerm *vt, पूर्णांक is_utf8);

माप_प्रकार vterm_आदान_write(VTerm *vt, स्थिरांक अक्षर *bytes, माप_प्रकार len);

/* Setting output callback will override the buffer logic */
प्रकारव्याख्या शून्य VTermOutputCallback(स्थिरांक अक्षर *s, माप_प्रकार len, शून्य *user);
शून्य vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, शून्य *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
माप_प्रकार vterm_output_get_buffer_size(स्थिरांक VTerm *vt);
माप_प्रकार vterm_output_get_buffer_current(स्थिरांक VTerm *vt);
माप_प्रकार vterm_output_get_buffer_remaining(स्थिरांक VTerm *vt);

/* This too */
माप_प्रकार vterm_output_read(VTerm *vt, अक्षर *buffer, माप_प्रकार len);

पूर्णांक vterm_is_modify_other_किल्ल्या(VTerm *vt);
पूर्णांक vterm_is_kitty_keyboard(VTerm *vt);
शून्य vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
शून्य vterm_keyboard_किल्ली(VTerm *vt, VTerm_किल्ली किल्ली, VTermModifier mod);

शून्य vterm_keyboard_start_paste(VTerm *vt);
शून्य vterm_keyboard_end_paste(VTerm *vt);

शून्य vterm_mouse_move(VTerm *vt, पूर्णांक row, पूर्णांक col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
शून्य vterm_mouse_button(VTerm *vt, पूर्णांक button, पूर्णांक pressed, VTermModifier mod);

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
#define CSI_ARG_मोजणी(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

प्रकारव्याख्या संरचना {
  पूर्णांक (*text)(स्थिरांक अक्षर *bytes, माप_प्रकार len, शून्य *user);
  पूर्णांक (*control)(अचिन्हित अक्षर control, शून्य *user);
  पूर्णांक (*escape)(स्थिरांक अक्षर *bytes, माप_प्रकार len, शून्य *user);
  पूर्णांक (*csi)(स्थिरांक अक्षर *leader, स्थिरांक दीर्घ args[], पूर्णांक argcount, स्थिरांक अक्षर *intermed, अक्षर command, शून्य *user);
  पूर्णांक (*osc)(पूर्णांक command, VTermStringFragment frag, शून्य *user);
  पूर्णांक (*dcs)(स्थिरांक अक्षर *command, माप_प्रकार commandlen, VTermStringFragment frag, शून्य *user);
  पूर्णांक (*apc)(VTermStringFragment frag, शून्य *user);
  पूर्णांक (*pm)(VTermStringFragment frag, शून्य *user);
  पूर्णांक (*sos)(VTermStringFragment frag, शून्य *user);
  पूर्णांक (*resize)(पूर्णांक rows, पूर्णांक cols, शून्य *user);
} VTermParserCallbacks;

शून्य  vterm_parser_set_callbacks(VTerm *vt, स्थिरांक VTermParserCallbacks *callbacks, शून्य *user);
शून्य *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
शून्य vterm_parser_set_emit_nul(VTerm *vt, पूर्णांक emit);

// -----------
// State layer
// -----------

प्रकारव्याख्या संरचना {
  पूर्णांक (*putglyph)(VTermGlyphInfo *info, VTermPos pos, शून्य *user);
  पूर्णांक (*movecursor)(VTermPos pos, VTermPos oldpos, पूर्णांक visible, शून्य *user);
  पूर्णांक (*scrollrect)(VTermRect rect, पूर्णांक downward, पूर्णांक rightward, शून्य *user);
  पूर्णांक (*moverect)(VTermRect dest, VTermRect src, शून्य *user);
  पूर्णांक (*erase)(VTermRect rect, पूर्णांक selective, शून्य *user);
  पूर्णांक (*initpen)(शून्य *user);
  पूर्णांक (*setpenattr)(VTermAttr attr, VTerm_मूल्य *val, शून्य *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  पूर्णांक (*settermprop)(VTermProp prop, VTerm_मूल्य *val, शून्य *user);
  पूर्णांक (*bell)(शून्य *user);
  पूर्णांक (*resize)(पूर्णांक rows, पूर्णांक cols, VTermStateFields *fields, शून्य *user);
  पूर्णांक (*setlineinfo)(पूर्णांक row, स्थिरांक VTerm_ओळ_Info *newinfo, स्थिरांक VTerm_ओळ_Info *oldinfo, शून्य *user);
  पूर्णांक (*sb_clear)(शून्य *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  पूर्णांक (*premove)(VTermRect dest, शून्य *user);
} VTermStateCallbacks;

// VIM: added
प्रकारव्याख्या संरचना {
  VTermPos pos;
  पूर्णांक	   buttons;
#define MOUSE_BUTTON_डावा 0x01
#define MOUSE_BUTTON_मधला 0x02
#define MOUSE_BUTTON_उजवा 0x04
  पूर्णांक      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

प्रकारव्याख्या संरचना {
  पूर्णांक (*control)(अचिन्हित अक्षर control, शून्य *user);
  पूर्णांक (*csi)(स्थिरांक अक्षर *leader, स्थिरांक दीर्घ args[], पूर्णांक argcount, स्थिरांक अक्षर *intermed, अक्षर command, शून्य *user);
  पूर्णांक (*osc)(पूर्णांक command, VTermStringFragment frag, शून्य *user);
  पूर्णांक (*dcs)(स्थिरांक अक्षर *command, माप_प्रकार commandlen, VTermStringFragment frag, शून्य *user);
  पूर्णांक (*apc)(VTermStringFragment frag, शून्य *user);
  पूर्णांक (*pm)(VTermStringFragment frag, शून्य *user);
  पूर्णांक (*sos)(VTermStringFragment frag, शून्य *user);
} VTermStateFallbacks;

प्रकारव्याख्या संरचना {
  पूर्णांक (*set)(VTermSelectionMask mask, VTermStringFragment frag, शून्य *user);
  पूर्णांक (*query)(VTermSelectionMask mask, शून्य *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

शून्य  vterm_state_set_callbacks(VTermState *state, स्थिरांक VTermStateCallbacks *callbacks, शून्य *user);
शून्य *vterm_state_get_cbdata(VTermState *state);

शून्य vterm_state_callbacks_has_premove(VTermState *state);

शून्य  vterm_state_set_unrecognised_fallbacks(VTermState *state, स्थिरांक VTermStateFallbacks *fallbacks, शून्य *user);
शून्य *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
शून्य vterm_state_reset(VTermState *state, पूर्णांक hard);

शून्य vterm_state_get_cursorpos(स्थिरांक VTermState *state, VTermPos *cursorpos);
// VIM: added
शून्य vterm_state_get_mousestate(स्थिरांक VTermState *state, VTermMouseState *mousestate);
शून्य vterm_state_get_default_colors(स्थिरांक VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
शून्य vterm_state_get_palette_color(स्थिरांक VTermState *state, पूर्णांक index, VTermColor *col);
शून्य vterm_state_set_default_colors(VTermState *state, स्थिरांक VTermColor *default_fg, स्थिरांक VTermColor *default_bg);
शून्य vterm_state_set_palette_color(VTermState *state, पूर्णांक index, स्थिरांक VTermColor *col);
शून्य vterm_state_set_bold_highbright(VTermState *state, पूर्णांक bold_is_highbright);
पूर्णांक  vterm_state_get_penattr(स्थिरांक VTermState *state, VTermAttr attr, VTerm_मूल्य *val);
पूर्णांक  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_मूल्य *val);
शून्य vterm_state_focus_in(VTermState *state);
शून्य vterm_state_focus_out(VTermState *state);
स्थिरांक VTerm_ओळ_Info *vterm_state_get_lineinfo(स्थिरांक VTermState *state, पूर्णांक row);

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
शून्य vterm_state_convert_color_to_rgb(स्थिरांक VTermState *state, VTermColor *col);

शून्य vterm_state_set_selection_callbacks(VTermState *state, स्थिरांक VTermSelectionCallbacks *callbacks, शून्य *user,
    अक्षर *buffer, माप_प्रकार buflen);

शून्य vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

प्रकारव्याख्या संरचना {
    अचिन्हित पूर्णांक bold      : 1;
    अचिन्हित पूर्णांक underline : 2;
    अचिन्हित पूर्णांक italic    : 1;
    अचिन्हित पूर्णांक blink     : 1;
    अचिन्हित पूर्णांक reverse   : 1;
    अचिन्हित पूर्णांक conceal   : 1;
    अचिन्हित पूर्णांक strike    : 1;
    अचिन्हित पूर्णांक font      : 4; /* 0 to 9 */
    अचिन्हित पूर्णांक dwl       : 1; /* On a DECDWL or DECDHL line */
    अचिन्हित पूर्णांक dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    अचिन्हित पूर्णांक small     : 1;
    अचिन्हित पूर्णांक baseline  : 2;
} VTermScreenCellAttrs;

गणना {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

गणना {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

प्रकारव्याख्या संरचना {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  अक्षर     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
प्रकारव्याख्या संरचना {
  पूर्णांक (*damage)(VTermRect rect, शून्य *user);
  पूर्णांक (*moverect)(VTermRect dest, VTermRect src, शून्य *user);
  पूर्णांक (*movecursor)(VTermPos pos, VTermPos oldpos, पूर्णांक visible, शून्य *user);
  पूर्णांक (*settermprop)(VTermProp prop, VTerm_मूल्य *val, शून्य *user);
  पूर्णांक (*bell)(शून्य *user);
  पूर्णांक (*resize)(पूर्णांक rows, पूर्णांक cols, शून्य *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  पूर्णांक (*sb_pushline)(पूर्णांक cols, स्थिरांक VTermScreenCell *cells, शून्य *user);
  पूर्णांक (*sb_popline)(पूर्णांक cols, VTermScreenCell *cells, शून्य *user);
  पूर्णांक (*sb_clear)(शून्य* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  पूर्णांक (*sb_pushline4)(पूर्णांक cols, स्थिरांक VTermScreenCell *cells, पूर्णांक continuation, शून्य *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
शून्य  vterm_screen_set_callbacks(VTermScreen *screen, स्थिरांक VTermScreenCallbacks *callbacks, शून्य *user);
शून्य *vterm_screen_get_cbdata(VTermScreen *screen);

शून्य vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

शून्य  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, स्थिरांक VTermStateFallbacks *fallbacks, शून्य *user);
शून्य *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

शून्य vterm_screen_enable_reflow(VTermScreen *screen, पूर्णांक reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
शून्य vterm_screen_enable_altscreen(VTermScreen *screen, पूर्णांक altscreen);

प्रकारव्याख्या गणना {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
शून्य vterm_screen_flush_damage(VTermScreen *screen);

शून्य vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
शून्य   vterm_screen_reset(VTermScreen *screen, पूर्णांक hard);

/* Neither of these functions NUL-terminate the buffer */
माप_प्रकार vterm_screen_get_chars(स्थिरांक VTermScreen *screen, uint32_t *chars, माप_प्रकार len, स्थिरांक VTermRect rect);
माप_प्रकार vterm_screen_get_text(स्थिरांक VTermScreen *screen, अक्षर *str, माप_प्रकार len, स्थिरांक VTermRect rect);

प्रकारव्याख्या गणना {
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

पूर्णांक vterm_screen_get_attrs_extent(स्थिरांक VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

पूर्णांक vterm_screen_get_cell(स्थिरांक VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

पूर्णांक vterm_screen_is_eol(स्थिरांक VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
शून्य vterm_screen_convert_color_to_rgb(स्थिरांक VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
शून्य vterm_screen_set_default_colors(VTermScreen *screen, स्थिरांक VTermColor *default_fg, स्थिरांक VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_मूल्य_Type vterm_get_attr_type(VTermAttr attr);
VTerm_मूल्य_Type vterm_get_prop_type(VTermProp prop);

शून्य vterm_scroll_rect(VTermRect rect,
                       पूर्णांक downward,
                       पूर्णांक rightward,
                       पूर्णांक (*moverect)(VTermRect src, VTermRect dest, शून्य *user),
                       पूर्णांक (*eraserect)(VTermRect rect, पूर्णांक selective, शून्य *user),
                       शून्य *user);

शून्य vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      शून्य (*copycell)(VTermPos dest, VTermPos src, शून्य *user),
                      शून्य *user);

#ifdef __cplusplus
}
#endif

#endif
