/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
ବାହ୍ୟ "C" {
#endif

#ଅନ୍ତର୍ଭାଵ <stdlib.h>

#ଅନ୍ତର୍ଭାଵ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned ଅକ୍ଷର		uint8_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned short		uint16_t;
ପ୍ରକାରବ୍ୟାଖ୍ୟା unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା		uint32_t;

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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VTerm VTerm;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VTermState VTermState;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା VTermScreen VTermScreen;

// Specifies a screen point.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_pos_cmp(VTermPos a, VTermPos b);

#ଯଦି defined(DEFINE_INLINES) || USE_INLINE
INLINE ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_pos_cmp(VTermPos a, VTermPos b)
{
  ଫେରନ୍ତୁ (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_row;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_row;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_col;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା end_col;
} VTermRect;

/* true if the rect contains the point */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_rect_contains(VTermRect r, VTermPos p);

#ଯଦି defined(DEFINE_INLINES) || USE_INLINE
INLINE ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_rect_contains(VTermRect r, VTermPos p)
{
  ଫେରନ୍ତୁ p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
ଶୂନ୍ୟ vterm_rect_move(VTermRect *rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row_delta, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_delta);

#ଯଦି defined(DEFINE_INLINES) || USE_INLINE
INLINE ଶୂନ୍ୟ vterm_rect_move(VTermRect *rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row_delta, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
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
ଶୂନ୍ୟ vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
ଶୂନ୍ୟ vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_color_is_equal(ଅଚଳ VTermColor *a, ଅଚଳ VTermColor *b);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_ମୂଲ୍ୟ_Type;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ଅଚଳ ଅକ୍ଷର *str;
  size_t      len : 30;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  initial : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା  final : 1;
} VTermStringFragment;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂଘ {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା boolean;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସଂଖ୍ୟା;
  VTermStringFragment string;
  VTermColor color;
} VTerm_ମୂଲ୍ୟ;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //ସଂଖ୍ୟା
  VTERM_PROP_MOUSE,             //ସଂଖ୍ୟା
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

ଗଣନା {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_ବାମ,

  VTERM_N_PROP_CURSORSHAPES
};

ଗଣନା {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ଅଚଳ uint32_t *chars;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା             width;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_ଧାଡ଼ି_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  VTermPos pos;                /* current cursor position */
  VTerm_ଧାଡ଼ି_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  ଶୂନ୍ୟ *(*malloc)(size_t size, ଶୂନ୍ୟ *allocdata);
  ଶୂନ୍ୟ  (*ମୁକ୍ତ)(ଶୂନ୍ୟ *ptr, ଶୂନ୍ୟ *allocdata);
} VTermAllocatorFunctions;

ଶୂନ୍ୟ vterm_check_version(ପୂର୍ଣ୍ଣସଂଖ୍ୟା major, ପୂର୍ଣ୍ଣସଂଖ୍ୟା minor);

ସଂରଚନା VTermBuilder {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ver; /* currently unused but reserved for some sort of ABI version flag */

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, cols;

  ଅଚଳ VTermAllocatorFunctions *allocator;
  ଶୂନ୍ୟ *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ଅଚଳ ସଂରଚନା VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, VTermAllocatorFunctions *funcs, ଶୂନ୍ୟ *allocdata);

// Free and cleanup a terminal and all its data.
ଶୂନ୍ୟ   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
ଶୂନ୍ୟ vterm_get_size(ଅଚଳ VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *rowsp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *colsp);

ଶୂନ୍ୟ vterm_set_size(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା  vterm_get_utf8(ଅଚଳ VTerm *vt);
ଶୂନ୍ୟ vterm_set_utf8(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_utf8);

size_t vterm_ନିବେଶ_write(VTerm *vt, ଅଚଳ ଅକ୍ଷର *bytes, size_t len);

/* Setting output callback will override the buffer logic */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ VTermOutputCallback(ଅଚଳ ଅକ୍ଷର *s, size_t len, ଶୂନ୍ୟ *user);
ଶୂନ୍ୟ vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, ଶୂନ୍ୟ *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(ଅଚଳ VTerm *vt);
size_t vterm_output_get_buffer_current(ଅଚଳ VTerm *vt);
size_t vterm_output_get_buffer_remaining(ଅଚଳ VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, ଅକ୍ଷର *buffer, size_t len);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_is_modify_other_ଚାବିଗୁଡ଼ିକ(VTerm *vt);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_is_kitty_keyboard(VTerm *vt);
ଶୂନ୍ୟ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
ଶୂନ୍ୟ vterm_keyboard_ଚାବି(VTerm *vt, VTerm_ଚାବି ଚାବି, VTermModifier mod);

ଶୂନ୍ୟ vterm_keyboard_start_paste(VTerm *vt);
ଶୂନ୍ୟ vterm_keyboard_end_paste(VTerm *vt);

ଶୂନ୍ୟ vterm_mouse_move(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
ଶୂନ୍ୟ vterm_mouse_button(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pressed, VTermModifier mod);

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
#define CSI_ARG_ଗଣନା(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*text)(ଅଚଳ ଅକ୍ଷର *bytes, size_t len, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*control)(unsigned ଅକ୍ଷର control, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*escape)(ଅଚଳ ଅକ୍ଷର *bytes, size_t len, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*csi)(ଅଚଳ ଅକ୍ଷର *leader, ଅଚଳ long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ଅଚଳ ଅକ୍ଷର *intermed, ଅକ୍ଷର command, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*osc)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା command, VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dcs)(ଅଚଳ ଅକ୍ଷର *command, size_t commandlen, VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*apc)(VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*pm)(VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*sos)(VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*resize)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଶୂନ୍ୟ *user);
} VTermParserCallbacks;

ଶୂନ୍ୟ  vterm_parser_set_callbacks(VTerm *vt, ଅଚଳ VTermParserCallbacks *callbacks, ଶୂନ୍ୟ *user);
ଶୂନ୍ୟ *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
ଶୂନ୍ୟ vterm_parser_set_emit_nul(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା emit);

// -----------
// State layer
// -----------

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*putglyph)(VTermGlyphInfo *info, VTermPos pos, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*movecursor)(VTermPos pos, VTermPos oldpos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା visible, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*scrollrect)(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rightward, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*moverect)(VTermRect dest, VTermRect src, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*erase)(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*initpen)(ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*setpenattr)(VTermAttr attr, VTerm_ମୂଲ୍ୟ *val, ଶୂନ୍ୟ *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*settermprop)(VTermProp prop, VTerm_ମୂଲ୍ୟ *val, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*bell)(ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*resize)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, VTermStateFields *fields, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*setlineinfo)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ଅଚଳ VTerm_ଧାଡ଼ି_Info *newinfo, ଅଚଳ VTerm_ଧାଡ଼ି_Info *oldinfo, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*sb_clear)(ଶୂନ୍ୟ *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*premove)(VTermRect dest, ଶୂନ୍ୟ *user);
} VTermStateCallbacks;

// VIM: added
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  VTermPos pos;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା	   buttons;
#define MOUSE_BUTTON_ବାମ 0x01
#define MOUSE_BUTTON_ମଧ୍ୟ 0x02
#define MOUSE_BUTTON_ଡାହାଣ 0x04
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*control)(unsigned ଅକ୍ଷର control, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*csi)(ଅଚଳ ଅକ୍ଷର *leader, ଅଚଳ long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ଅଚଳ ଅକ୍ଷର *intermed, ଅକ୍ଷର command, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*osc)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା command, VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dcs)(ଅଚଳ ଅକ୍ଷର *command, size_t commandlen, VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*apc)(VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*pm)(VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*sos)(VTermStringFragment frag, ଶୂନ୍ୟ *user);
} VTermStateFallbacks;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*set)(VTermSelectionMask mask, VTermStringFragment frag, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*query)(VTermSelectionMask mask, ଶୂନ୍ୟ *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

ଶୂନ୍ୟ  vterm_state_set_callbacks(VTermState *state, ଅଚଳ VTermStateCallbacks *callbacks, ଶୂନ୍ୟ *user);
ଶୂନ୍ୟ *vterm_state_get_cbdata(VTermState *state);

ଶୂନ୍ୟ vterm_state_callbacks_has_premove(VTermState *state);

ଶୂନ୍ୟ  vterm_state_set_unrecognised_fallbacks(VTermState *state, ଅଚଳ VTermStateFallbacks *fallbacks, ଶୂନ୍ୟ *user);
ଶୂନ୍ୟ *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
ଶୂନ୍ୟ vterm_state_reset(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hard);

ଶୂନ୍ୟ vterm_state_get_cursorpos(ଅଚଳ VTermState *state, VTermPos *cursorpos);
// VIM: added
ଶୂନ୍ୟ vterm_state_get_mousestate(ଅଚଳ VTermState *state, VTermMouseState *mousestate);
ଶୂନ୍ୟ vterm_state_get_default_colors(ଅଚଳ VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
ଶୂନ୍ୟ vterm_state_get_palette_color(ଅଚଳ VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, VTermColor *col);
ଶୂନ୍ୟ vterm_state_set_default_colors(VTermState *state, ଅଚଳ VTermColor *default_fg, ଅଚଳ VTermColor *default_bg);
ଶୂନ୍ୟ vterm_state_set_palette_color(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ଅଚଳ VTermColor *col);
ଶୂନ୍ୟ vterm_state_set_bold_highbright(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold_is_highbright);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା  vterm_state_get_penattr(ଅଚଳ VTermState *state, VTermAttr attr, VTerm_ମୂଲ୍ୟ *val);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_ମୂଲ୍ୟ *val);
ଶୂନ୍ୟ vterm_state_focus_in(VTermState *state);
ଶୂନ୍ୟ vterm_state_focus_out(VTermState *state);
ଅଚଳ VTerm_ଧାଡ଼ି_Info *vterm_state_get_lineinfo(ଅଚଳ VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row);

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
ଶୂନ୍ୟ vterm_state_convert_color_to_rgb(ଅଚଳ VTermState *state, VTermColor *col);

ଶୂନ୍ୟ vterm_state_set_selection_callbacks(VTermState *state, ଅଚଳ VTermSelectionCallbacks *callbacks, ଶୂନ୍ୟ *user,
    ଅକ୍ଷର *buffer, size_t buflen);

ଶୂନ୍ୟ vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold      : 1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା underline : 2;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା italic    : 1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା blink     : 1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reverse   : 1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା conceal   : 1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା strike    : 1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା font      : 4; /* 0 to 9 */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା small     : 1;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା baseline  : 2;
} VTermScreenCellAttrs;

ଗଣନା {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

ଗଣନା {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ଅକ୍ଷର     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*damage)(VTermRect rect, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*moverect)(VTermRect dest, VTermRect src, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*movecursor)(VTermPos pos, VTermPos oldpos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା visible, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*settermprop)(VTermProp prop, VTerm_ମୂଲ୍ୟ *val, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*bell)(ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*resize)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଶୂନ୍ୟ *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*sb_pushline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଅଚଳ VTermScreenCell *cells, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*sb_popline)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, VTermScreenCell *cells, ଶୂନ୍ୟ *user);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*sb_clear)(ଶୂନ୍ୟ* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*sb_pushline4)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଅଚଳ VTermScreenCell *cells, ପୂର୍ଣ୍ଣସଂଖ୍ୟା continuation, ଶୂନ୍ୟ *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
ଶୂନ୍ୟ  vterm_screen_set_callbacks(VTermScreen *screen, ଅଚଳ VTermScreenCallbacks *callbacks, ଶୂନ୍ୟ *user);
ଶୂନ୍ୟ *vterm_screen_get_cbdata(VTermScreen *screen);

ଶୂନ୍ୟ vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

ଶୂନ୍ୟ  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ଅଚଳ VTermStateFallbacks *fallbacks, ଶୂନ୍ୟ *user);
ଶୂନ୍ୟ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

ଶୂନ୍ୟ vterm_screen_enable_reflow(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
ଶୂନ୍ୟ vterm_screen_enable_altscreen(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା altscreen);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
ଶୂନ୍ୟ vterm_screen_flush_damage(VTermScreen *screen);

ଶୂନ୍ୟ vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
ଶୂନ୍ୟ   vterm_screen_reset(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ଅଚଳ VTermScreen *screen, uint32_t *chars, size_t len, ଅଚଳ VTermRect rect);
size_t vterm_screen_get_text(ଅଚଳ VTermScreen *screen, ଅକ୍ଷର *str, size_t len, ଅଚଳ VTermRect rect);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_screen_get_attrs_extent(ଅଚଳ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_screen_get_cell(ଅଚଳ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_screen_is_eol(ଅଚଳ VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
ଶୂନ୍ୟ vterm_screen_convert_color_to_rgb(ଅଚଳ VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
ଶୂନ୍ୟ vterm_screen_set_default_colors(VTermScreen *screen, ଅଚଳ VTermColor *default_fg, ଅଚଳ VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_ମୂଲ୍ୟ_Type vterm_get_attr_type(VTermAttr attr);
VTerm_ମୂଲ୍ୟ_Type vterm_get_prop_type(VTermProp prop);

ଶୂନ୍ୟ vterm_scroll_rect(VTermRect rect,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା rightward,
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*moverect)(VTermRect src, VTermRect dest, ଶୂନ୍ୟ *user),
                       ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*eraserect)(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective, ଶୂନ୍ୟ *user),
                       ଶୂନ୍ୟ *user);

ଶୂନ୍ୟ vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      ଶୂନ୍ୟ (*copycell)(VTermPos dest, VTermPos src, ଶୂନ୍ୟ *user),
                      ଶୂନ୍ୟ *user);

#ifdef __cplusplus
}
#endif

#endif
