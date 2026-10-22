/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
ಬಾಹ್ಯ "C" {
#endif

#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ವರ್ಣ		uint8_t;
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned short		uint16_t;
ಪ್ರಕಾರನಿರ್ದೇಶ unsigned ಪೂರ್ಣಾಙ್ಕ		uint32_t;

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VTerm VTerm;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VTermState VTermState;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ VTermScreen VTermScreen;

// Specifies a screen point.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ row;
  ಪೂರ್ಣಾಙ್ಕ col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
ಪೂರ್ಣಾಙ್ಕ vterm_pos_cmp(VTermPos a, VTermPos b);

#ಯದಿ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಪೂರ್ಣಾಙ್ಕ vterm_pos_cmp(VTermPos a, VTermPos b)
{
  ಪ್ರತ್ಯಾಗಮ (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ start_row;
  ಪೂರ್ಣಾಙ್ಕ end_row;
  ಪೂರ್ಣಾಙ್ಕ start_col;
  ಪೂರ್ಣಾಙ್ಕ end_col;
} VTermRect;

/* true if the rect contains the point */
ಪೂರ್ಣಾಙ್ಕ vterm_rect_contains(VTermRect r, VTermPos p);

#ಯದಿ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಪೂರ್ಣಾಙ್ಕ vterm_rect_contains(VTermRect r, VTermPos p)
{
  ಪ್ರತ್ಯಾಗಮ p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
ಶೂನ್ಯ vterm_rect_move(VTermRect *rect, ಪೂರ್ಣಾಙ್ಕ row_delta, ಪೂರ್ಣಾಙ್ಕ col_delta);

#ಯದಿ defined(DEFINE_INLINES) || USE_INLINE
INLINE ಶೂನ್ಯ vterm_rect_move(VTermRect *rect, ಪೂರ್ಣಾಙ್ಕ row_delta, ಪೂರ್ಣಾಙ್ಕ col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
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
ಶೂನ್ಯ vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
ಶೂನ್ಯ vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
ಪೂರ್ಣಾಙ್ಕ vterm_color_is_equal(ಧ್ರುವ VTermColor *a, ಧ್ರುವ VTermColor *b);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_ಮೌಲ್ಯ_Type;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಧ್ರುವ ವರ್ಣ *str;
  size_t      len : 30;
  unsigned ಪೂರ್ಣಾಙ್ಕ  initial : 1;
  unsigned ಪೂರ್ಣಾಙ್ಕ  final : 1;
} VTermStringFragment;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಙ್ಘ {
  ಪೂರ್ಣಾಙ್ಕ boolean;
  ಪೂರ್ಣಾಙ್ಕ ಸಂಖ್ಯೆ;
  VTermStringFragment string;
  VTermColor color;
} VTerm_ಮೌಲ್ಯ;

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //ಸಂಖ್ಯೆ
  VTERM_PROP_MOUSE,             //ಸಂಖ್ಯೆ
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

ಗಣನಾ {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_ಎಡ,

  VTERM_N_PROP_CURSORSHAPES
};

ಗಣನಾ {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಧ್ರುವ uint32_t *chars;
  ಪೂರ್ಣಾಙ್ಕ             width;
  unsigned ಪೂರ್ಣಾಙ್ಕ    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned ಪೂರ್ಣಾಙ್ಕ    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned ಪೂರ್ಣಾಙ್ಕ    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  unsigned ಪೂರ್ಣಾಙ್ಕ    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned ಪೂರ್ಣಾಙ್ಕ    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned ಪೂರ್ಣಾಙ್ಕ    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_ಸಾಲು_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  VTermPos pos;                /* current cursor position */
  VTerm_ಸಾಲು_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  ಶೂನ್ಯ *(*malloc)(size_t size, ಶೂನ್ಯ *allocdata);
  ಶೂನ್ಯ  (*ಮುಕ್ತ)(ಶೂನ್ಯ *ptr, ಶೂನ್ಯ *allocdata);
} VTermAllocatorFunctions;

ಶೂನ್ಯ vterm_check_version(ಪೂರ್ಣಾಙ್ಕ major, ಪೂರ್ಣಾಙ್ಕ minor);

ಸಂರಚನಾ VTermBuilder {
  ಪೂರ್ಣಾಙ್ಕ ver; /* currently unused but reserved for some sort of ABI version flag */

  ಪೂರ್ಣಾಙ್ಕ rows, cols;

  ಧ್ರುವ VTermAllocatorFunctions *allocator;
  ಶೂನ್ಯ *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ಧ್ರುವ ಸಂರಚನಾ VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, VTermAllocatorFunctions *funcs, ಶೂನ್ಯ *allocdata);

// Free and cleanup a terminal and all its data.
ಶೂನ್ಯ   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
ಶೂನ್ಯ vterm_get_size(ಧ್ರುವ VTerm *vt, ಪೂರ್ಣಾಙ್ಕ *rowsp, ಪೂರ್ಣಾಙ್ಕ *colsp);

ಶೂನ್ಯ vterm_set_size(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols);

ಪೂರ್ಣಾಙ್ಕ  vterm_get_utf8(ಧ್ರುವ VTerm *vt);
ಶೂನ್ಯ vterm_set_utf8(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ is_utf8);

size_t vterm_ಒಳಹರಿವು_write(VTerm *vt, ಧ್ರುವ ವರ್ಣ *bytes, size_t len);

/* Setting output callback will override the buffer logic */
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ VTermOutputCallback(ಧ್ರುವ ವರ್ಣ *s, size_t len, ಶೂನ್ಯ *user);
ಶೂನ್ಯ vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, ಶೂನ್ಯ *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(ಧ್ರುವ VTerm *vt);
size_t vterm_output_get_buffer_current(ಧ್ರುವ VTerm *vt);
size_t vterm_output_get_buffer_remaining(ಧ್ರುವ VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, ವರ್ಣ *buffer, size_t len);

ಪೂರ್ಣಾಙ್ಕ vterm_is_modify_other_ಕೀಲಿಗಳು(VTerm *vt);
ಪೂರ್ಣಾಙ್ಕ vterm_is_kitty_keyboard(VTerm *vt);
ಶೂನ್ಯ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
ಶೂನ್ಯ vterm_keyboard_ಕೀಲಿ(VTerm *vt, VTerm_ಕೀಲಿ ಕೀಲಿ, VTermModifier mod);

ಶೂನ್ಯ vterm_keyboard_start_paste(VTerm *vt);
ಶೂನ್ಯ vterm_keyboard_end_paste(VTerm *vt);

ಶೂನ್ಯ vterm_mouse_move(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
ಶೂನ್ಯ vterm_mouse_button(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ button, ಪೂರ್ಣಾಙ್ಕ pressed, VTermModifier mod);

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
#define CSI_ARG_ಎಣಿಕೆ(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ (*text)(ಧ್ರುವ ವರ್ಣ *bytes, size_t len, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*control)(unsigned ವರ್ಣ control, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*escape)(ಧ್ರುವ ವರ್ಣ *bytes, size_t len, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*csi)(ಧ್ರುವ ವರ್ಣ *leader, ಧ್ರುವ long args[], ಪೂರ್ಣಾಙ್ಕ argcount, ಧ್ರುವ ವರ್ಣ *intermed, ವರ್ಣ command, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*osc)(ಪೂರ್ಣಾಙ್ಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*dcs)(ಧ್ರುವ ವರ್ಣ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*apc)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*pm)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*sos)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*resize)(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, ಶೂನ್ಯ *user);
} VTermParserCallbacks;

ಶೂನ್ಯ  vterm_parser_set_callbacks(VTerm *vt, ಧ್ರುವ VTermParserCallbacks *callbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
ಶೂನ್ಯ vterm_parser_set_emit_nul(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ emit);

// -----------
// State layer
// -----------

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ (*putglyph)(VTermGlyphInfo *info, VTermPos pos, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*movecursor)(VTermPos pos, VTermPos oldpos, ಪೂರ್ಣಾಙ್ಕ visible, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*scrollrect)(VTermRect rect, ಪೂರ್ಣಾಙ್ಕ downward, ಪೂರ್ಣಾಙ್ಕ rightward, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*moverect)(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*erase)(VTermRect rect, ಪೂರ್ಣಾಙ್ಕ selective, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*initpen)(ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*setpenattr)(VTermAttr attr, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  ಪೂರ್ಣಾಙ್ಕ (*settermprop)(VTermProp prop, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*bell)(ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*resize)(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, VTermStateFields *fields, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*setlineinfo)(ಪೂರ್ಣಾಙ್ಕ row, ಧ್ರುವ VTerm_ಸಾಲು_Info *newinfo, ಧ್ರುವ VTerm_ಸಾಲು_Info *oldinfo, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*sb_clear)(ಶೂನ್ಯ *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  ಪೂರ್ಣಾಙ್ಕ (*premove)(VTermRect dest, ಶೂನ್ಯ *user);
} VTermStateCallbacks;

// VIM: added
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  VTermPos pos;
  ಪೂರ್ಣಾಙ್ಕ	   buttons;
#define MOUSE_BUTTON_ಎಡ 0x01
#define MOUSE_BUTTON_ಮಧ್ಯ 0x02
#define MOUSE_BUTTON_ಬಲ 0x04
  ಪೂರ್ಣಾಙ್ಕ      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ (*control)(unsigned ವರ್ಣ control, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*csi)(ಧ್ರುವ ವರ್ಣ *leader, ಧ್ರುವ long args[], ಪೂರ್ಣಾಙ್ಕ argcount, ಧ್ರುವ ವರ್ಣ *intermed, ವರ್ಣ command, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*osc)(ಪೂರ್ಣಾಙ್ಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*dcs)(ಧ್ರುವ ವರ್ಣ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*apc)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*pm)(VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*sos)(VTermStringFragment frag, ಶೂನ್ಯ *user);
} VTermStateFallbacks;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ (*set)(VTermSelectionMask mask, VTermStringFragment frag, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*query)(VTermSelectionMask mask, ಶೂನ್ಯ *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

ಶೂನ್ಯ  vterm_state_set_callbacks(VTermState *state, ಧ್ರುವ VTermStateCallbacks *callbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_state_get_cbdata(VTermState *state);

ಶೂನ್ಯ vterm_state_callbacks_has_premove(VTermState *state);

ಶೂನ್ಯ  vterm_state_set_unrecognised_fallbacks(VTermState *state, ಧ್ರುವ VTermStateFallbacks *fallbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
ಶೂನ್ಯ vterm_state_reset(VTermState *state, ಪೂರ್ಣಾಙ್ಕ hard);

ಶೂನ್ಯ vterm_state_get_cursorpos(ಧ್ರುವ VTermState *state, VTermPos *cursorpos);
// VIM: added
ಶೂನ್ಯ vterm_state_get_mousestate(ಧ್ರುವ VTermState *state, VTermMouseState *mousestate);
ಶೂನ್ಯ vterm_state_get_default_colors(ಧ್ರುವ VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
ಶೂನ್ಯ vterm_state_get_palette_color(ಧ್ರುವ VTermState *state, ಪೂರ್ಣಾಙ್ಕ index, VTermColor *col);
ಶೂನ್ಯ vterm_state_set_default_colors(VTermState *state, ಧ್ರುವ VTermColor *default_fg, ಧ್ರುವ VTermColor *default_bg);
ಶೂನ್ಯ vterm_state_set_palette_color(VTermState *state, ಪೂರ್ಣಾಙ್ಕ index, ಧ್ರುವ VTermColor *col);
ಶೂನ್ಯ vterm_state_set_bold_highbright(VTermState *state, ಪೂರ್ಣಾಙ್ಕ bold_is_highbright);
ಪೂರ್ಣಾಙ್ಕ  vterm_state_get_penattr(ಧ್ರುವ VTermState *state, VTermAttr attr, VTerm_ಮೌಲ್ಯ *val);
ಪೂರ್ಣಾಙ್ಕ  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_ಮೌಲ್ಯ *val);
ಶೂನ್ಯ vterm_state_focus_in(VTermState *state);
ಶೂನ್ಯ vterm_state_focus_out(VTermState *state);
ಧ್ರುವ VTerm_ಸಾಲು_Info *vterm_state_get_lineinfo(ಧ್ರುವ VTermState *state, ಪೂರ್ಣಾಙ್ಕ row);

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
ಶೂನ್ಯ vterm_state_convert_color_to_rgb(ಧ್ರುವ VTermState *state, VTermColor *col);

ಶೂನ್ಯ vterm_state_set_selection_callbacks(VTermState *state, ಧ್ರುವ VTermSelectionCallbacks *callbacks, ಶೂನ್ಯ *user,
    ವರ್ಣ *buffer, size_t buflen);

ಶೂನ್ಯ vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    unsigned ಪೂರ್ಣಾಙ್ಕ bold      : 1;
    unsigned ಪೂರ್ಣಾಙ್ಕ underline : 2;
    unsigned ಪೂರ್ಣಾಙ್ಕ italic    : 1;
    unsigned ಪೂರ್ಣಾಙ್ಕ blink     : 1;
    unsigned ಪೂರ್ಣಾಙ್ಕ reverse   : 1;
    unsigned ಪೂರ್ಣಾಙ್ಕ conceal   : 1;
    unsigned ಪೂರ್ಣಾಙ್ಕ strike    : 1;
    unsigned ಪೂರ್ಣಾಙ್ಕ font      : 4; /* 0 to 9 */
    unsigned ಪೂರ್ಣಾಙ್ಕ dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned ಪೂರ್ಣಾಙ್ಕ dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned ಪೂರ್ಣಾಙ್ಕ small     : 1;
    unsigned ಪೂರ್ಣಾಙ್ಕ baseline  : 2;
} VTermScreenCellAttrs;

ಗಣನಾ {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

ಗಣನಾ {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ವರ್ಣ     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ (*damage)(VTermRect rect, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*moverect)(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*movecursor)(VTermPos pos, VTermPos oldpos, ಪೂರ್ಣಾಙ್ಕ visible, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*settermprop)(VTermProp prop, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*bell)(ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*resize)(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, ಶೂನ್ಯ *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  ಪೂರ್ಣಾಙ್ಕ (*sb_pushline)(ಪೂರ್ಣಾಙ್ಕ cols, ಧ್ರುವ VTermScreenCell *cells, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*sb_popline)(ಪೂರ್ಣಾಙ್ಕ cols, VTermScreenCell *cells, ಶೂನ್ಯ *user);
  ಪೂರ್ಣಾಙ್ಕ (*sb_clear)(ಶೂನ್ಯ* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  ಪೂರ್ಣಾಙ್ಕ (*sb_pushline4)(ಪೂರ್ಣಾಙ್ಕ cols, ಧ್ರುವ VTermScreenCell *cells, ಪೂರ್ಣಾಙ್ಕ continuation, ಶೂನ್ಯ *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
ಶೂನ್ಯ  vterm_screen_set_callbacks(VTermScreen *screen, ಧ್ರುವ VTermScreenCallbacks *callbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_screen_get_cbdata(VTermScreen *screen);

ಶೂನ್ಯ vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

ಶೂನ್ಯ  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ಧ್ರುವ VTermStateFallbacks *fallbacks, ಶೂನ್ಯ *user);
ಶೂನ್ಯ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

ಶೂನ್ಯ vterm_screen_enable_reflow(VTermScreen *screen, ಪೂರ್ಣಾಙ್ಕ reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
ಶೂನ್ಯ vterm_screen_enable_altscreen(VTermScreen *screen, ಪೂರ್ಣಾಙ್ಕ altscreen);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
ಶೂನ್ಯ vterm_screen_flush_damage(VTermScreen *screen);

ಶೂನ್ಯ vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
ಶೂನ್ಯ   vterm_screen_reset(VTermScreen *screen, ಪೂರ್ಣಾಙ್ಕ hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ಧ್ರುವ VTermScreen *screen, uint32_t *chars, size_t len, ಧ್ರುವ VTermRect rect);
size_t vterm_screen_get_text(ಧ್ರುವ VTermScreen *screen, ವರ್ಣ *str, size_t len, ಧ್ರುವ VTermRect rect);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಪೂರ್ಣಾಙ್ಕ vterm_screen_get_attrs_extent(ಧ್ರುವ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

ಪೂರ್ಣಾಙ್ಕ vterm_screen_get_cell(ಧ್ರುವ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

ಪೂರ್ಣಾಙ್ಕ vterm_screen_is_eol(ಧ್ರುವ VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
ಶೂನ್ಯ vterm_screen_convert_color_to_rgb(ಧ್ರುವ VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
ಶೂನ್ಯ vterm_screen_set_default_colors(VTermScreen *screen, ಧ್ರುವ VTermColor *default_fg, ಧ್ರುವ VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_ಮೌಲ್ಯ_Type vterm_get_attr_type(VTermAttr attr);
VTerm_ಮೌಲ್ಯ_Type vterm_get_prop_type(VTermProp prop);

ಶೂನ್ಯ vterm_scroll_rect(VTermRect rect,
                       ಪೂರ್ಣಾಙ್ಕ downward,
                       ಪೂರ್ಣಾಙ್ಕ rightward,
                       ಪೂರ್ಣಾಙ್ಕ (*moverect)(VTermRect src, VTermRect dest, ಶೂನ್ಯ *user),
                       ಪೂರ್ಣಾಙ್ಕ (*eraserect)(VTermRect rect, ಪೂರ್ಣಾಙ್ಕ selective, ಶೂನ್ಯ *user),
                       ಶೂನ್ಯ *user);

ಶೂನ್ಯ vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      ಶೂನ್ಯ (*copycell)(VTermPos dest, VTermPos src, ಶೂನ್ಯ *user),
                      ಶೂನ್ಯ *user);

#ifdef __cplusplus
}
#endif

#endif
