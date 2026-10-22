/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ifndef __VTERM_H__
#define __VTERM_H__

#ifdef __cplusplus
ബാഹ്യ "C" {
#endif

#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#define TRUE 1
#define FALSE 0

// VIM: from stdint.h
തരനിർവചനം unsigned അക്ഷരം		uint8_t;
തരനിർവചനം unsigned short		uint16_t;
തരനിർവചനം unsigned പൂർണ്ണസംഖ്യ		uint32_t;

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

തരനിർവചനം ഘടന VTerm VTerm;
തരനിർവചനം ഘടന VTermState VTermState;
തരനിർവചനം ഘടന VTermScreen VTermScreen;

// Specifies a screen point.
തരനിർവചനം ഘടന {
  പൂർണ്ണസംഖ്യ row;
  പൂർണ്ണസംഖ്യ col;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
പൂർണ്ണസംഖ്യ vterm_pos_cmp(VTermPos a, VTermPos b);

#എങ്കിൽ defined(DEFINE_INLINES) || USE_INLINE
INLINE പൂർണ്ണസംഖ്യ vterm_pos_cmp(VTermPos a, VTermPos b)
{
  തിരികെ (a.row == b.row) ? a.col - b.col : a.row - b.row;
}
#endif

// Specifies a rectangular screen area.
തരനിർവചനം ഘടന {
  പൂർണ്ണസംഖ്യ start_row;
  പൂർണ്ണസംഖ്യ end_row;
  പൂർണ്ണസംഖ്യ start_col;
  പൂർണ്ണസംഖ്യ end_col;
} VTermRect;

/* true if the rect contains the point */
പൂർണ്ണസംഖ്യ vterm_rect_contains(VTermRect r, VTermPos p);

#എങ്കിൽ defined(DEFINE_INLINES) || USE_INLINE
INLINE പൂർണ്ണസംഖ്യ vterm_rect_contains(VTermRect r, VTermPos p)
{
  തിരികെ p.row >= r.start_row && p.row < r.end_row &&
         p.col >= r.start_col && p.col < r.end_col;
}
#endif

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
ശൂന്യം vterm_rect_move(VTermRect *rect, പൂർണ്ണസംഖ്യ row_delta, പൂർണ്ണസംഖ്യ col_delta);

#എങ്കിൽ defined(DEFINE_INLINES) || USE_INLINE
INLINE ശൂന്യം vterm_rect_move(VTermRect *rect, പൂർണ്ണസംഖ്യ row_delta, പൂർണ്ണസംഖ്യ col_delta)
{
  rect->start_row += row_delta; rect->end_row += row_delta;
  rect->start_col += col_delta; rect->end_col += col_delta;
}
#endif

/**
 * Bit-field describing the value of VTermColor.type
 */
തരനിർവചനം എണ്ണൽ {
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
തരനിർവചനം ഘടന {
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
ശൂന്യം vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
ശൂന്യം vterm_color_indexed(VTermColor *col, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
പൂർണ്ണസംഖ്യ vterm_color_is_equal(സ്ഥിരം VTermColor *a, സ്ഥിരം VTermColor *b);

തരനിർവചനം എണ്ണൽ {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_മൂല്യം_Type;

തരനിർവചനം ഘടന {
  സ്ഥിരം അക്ഷരം *str;
  size_t      len : 30;
  unsigned പൂർണ്ണസംഖ്യ  initial : 1;
  unsigned പൂർണ്ണസംഖ്യ  final : 1;
} VTermStringFragment;

തരനിർവചനം സംഘം {
  പൂർണ്ണസംഖ്യ boolean;
  പൂർണ്ണസംഖ്യ സംഖ്യ;
  VTermStringFragment string;
  VTermColor color;
} VTerm_മൂല്യം;

തരനിർവചനം എണ്ണൽ {
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

തരനിർവചനം എണ്ണൽ {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_REVERSE,           // bool
  VTERM_PROP_CURSORSHAPE,       //സംഖ്യ
  VTERM_PROP_MOUSE,             //സംഖ്യ
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

എണ്ണൽ {
  VTERM_PROP_CURSORSHAPE_BLOCK = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_ഇടത്,

  VTERM_N_PROP_CURSORSHAPES
};

എണ്ണൽ {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

തരനിർവചനം എണ്ണൽ {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTermSelectionMask;

തരനിർവചനം ഘടന {
  സ്ഥിരം uint32_t *chars;
  പൂർണ്ണസംഖ്യ             width;
  unsigned പൂർണ്ണസംഖ്യ    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  unsigned പൂർണ്ണസംഖ്യ    dwl:1;             /* DECDWL or DECDHL double-width line */
  unsigned പൂർണ്ണസംഖ്യ    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

തരനിർവചനം ഘടന {
  unsigned പൂർണ്ണസംഖ്യ    doublewidth:1;     /* DECDWL or DECDHL line */
  unsigned പൂർണ്ണസംഖ്യ    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  unsigned പൂർണ്ണസംഖ്യ    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_വരി_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
തരനിർവചനം ഘടന {
  VTermPos pos;                /* current cursor position */
  VTerm_വരി_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

തരനിർവചനം ഘടന {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  ശൂന്യം *(*malloc)(size_t size, ശൂന്യം *allocdata);
  ശൂന്യം  (*സ്വതന്ത്രം)(ശൂന്യം *ptr, ശൂന്യം *allocdata);
} VTermAllocatorFunctions;

ശൂന്യം vterm_check_version(പൂർണ്ണസംഖ്യ major, പൂർണ്ണസംഖ്യ minor);

ഘടന VTermBuilder {
  പൂർണ്ണസംഖ്യ ver; /* currently unused but reserved for some sort of ABI version flag */

  പൂർണ്ണസംഖ്യ rows, cols;

  സ്ഥിരം VTermAllocatorFunctions *allocator;
  ശൂന്യം *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(സ്ഥിരം ഘടന VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, VTermAllocatorFunctions *funcs, ശൂന്യം *allocdata);

// Free and cleanup a terminal and all its data.
ശൂന്യം   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
ശൂന്യം vterm_get_size(സ്ഥിരം VTerm *vt, പൂർണ്ണസംഖ്യ *rowsp, പൂർണ്ണസംഖ്യ *colsp);

ശൂന്യം vterm_set_size(VTerm *vt, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols);

പൂർണ്ണസംഖ്യ  vterm_get_utf8(സ്ഥിരം VTerm *vt);
ശൂന്യം vterm_set_utf8(VTerm *vt, പൂർണ്ണസംഖ്യ is_utf8);

size_t vterm_നിവേശം_write(VTerm *vt, സ്ഥിരം അക്ഷരം *bytes, size_t len);

/* Setting output callback will override the buffer logic */
തരനിർവചനം ശൂന്യം VTermOutputCallback(സ്ഥിരം അക്ഷരം *s, size_t len, ശൂന്യം *user);
ശൂന്യം vterm_output_set_callback(VTerm *vt, VTermOutputCallback *func, ശൂന്യം *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_buffer_size(സ്ഥിരം VTerm *vt);
size_t vterm_output_get_buffer_current(സ്ഥിരം VTerm *vt);
size_t vterm_output_get_buffer_remaining(സ്ഥിരം VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, അക്ഷരം *buffer, size_t len);

പൂർണ്ണസംഖ്യ vterm_is_modify_other_താക്കോലുകൾ(VTerm *vt);
പൂർണ്ണസംഖ്യ vterm_is_kitty_keyboard(VTerm *vt);
ശൂന്യം vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
ശൂന്യം vterm_keyboard_താക്കോൽ(VTerm *vt, VTerm_താക്കോൽ താക്കോൽ, VTermModifier mod);

ശൂന്യം vterm_keyboard_start_paste(VTerm *vt);
ശൂന്യം vterm_keyboard_end_paste(VTerm *vt);

ശൂന്യം vterm_mouse_move(VTerm *vt, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
ശൂന്യം vterm_mouse_button(VTerm *vt, പൂർണ്ണസംഖ്യ button, പൂർണ്ണസംഖ്യ pressed, VTermModifier mod);

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
#define CSI_ARG_എണ്ണം(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

തരനിർവചനം ഘടന {
  പൂർണ്ണസംഖ്യ (*text)(സ്ഥിരം അക്ഷരം *bytes, size_t len, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*control)(unsigned അക്ഷരം control, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*escape)(സ്ഥിരം അക്ഷരം *bytes, size_t len, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*csi)(സ്ഥിരം അക്ഷരം *leader, സ്ഥിരം long args[], പൂർണ്ണസംഖ്യ argcount, സ്ഥിരം അക്ഷരം *intermed, അക്ഷരം command, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*osc)(പൂർണ്ണസംഖ്യ command, VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*dcs)(സ്ഥിരം അക്ഷരം *command, size_t commandlen, VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*apc)(VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*pm)(VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*sos)(VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*resize)(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, ശൂന്യം *user);
} VTermParserCallbacks;

ശൂന്യം  vterm_parser_set_callbacks(VTerm *vt, സ്ഥിരം VTermParserCallbacks *callbacks, ശൂന്യം *user);
ശൂന്യം *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
ശൂന്യം vterm_parser_set_emit_nul(VTerm *vt, പൂർണ്ണസംഖ്യ emit);

// -----------
// State layer
// -----------

തരനിർവചനം ഘടന {
  പൂർണ്ണസംഖ്യ (*putglyph)(VTermGlyphInfo *info, VTermPos pos, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*movecursor)(VTermPos pos, VTermPos oldpos, പൂർണ്ണസംഖ്യ visible, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*scrollrect)(VTermRect rect, പൂർണ്ണസംഖ്യ downward, പൂർണ്ണസംഖ്യ rightward, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*moverect)(VTermRect dest, VTermRect src, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*erase)(VTermRect rect, പൂർണ്ണസംഖ്യ selective, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*initpen)(ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*setpenattr)(VTermAttr attr, VTerm_മൂല്യം *val, ശൂന്യം *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  പൂർണ്ണസംഖ്യ (*settermprop)(VTermProp prop, VTerm_മൂല്യം *val, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*bell)(ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*resize)(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, VTermStateFields *fields, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*setlineinfo)(പൂർണ്ണസംഖ്യ row, സ്ഥിരം VTerm_വരി_Info *newinfo, സ്ഥിരം VTerm_വരി_Info *oldinfo, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*sb_clear)(ശൂന്യം *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  പൂർണ്ണസംഖ്യ (*premove)(VTermRect dest, ശൂന്യം *user);
} VTermStateCallbacks;

// VIM: added
തരനിർവചനം ഘടന {
  VTermPos pos;
  പൂർണ്ണസംഖ്യ	   buttons;
#define MOUSE_BUTTON_ഇടത് 0x01
#define MOUSE_BUTTON_നടുവ് 0x02
#define MOUSE_BUTTON_വലത് 0x04
  പൂർണ്ണസംഖ്യ      flags;
#define MOUSE_WANT_CLICK 0x01
#define MOUSE_WANT_DRAG  0x02
#define MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

തരനിർവചനം ഘടന {
  പൂർണ്ണസംഖ്യ (*control)(unsigned അക്ഷരം control, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*csi)(സ്ഥിരം അക്ഷരം *leader, സ്ഥിരം long args[], പൂർണ്ണസംഖ്യ argcount, സ്ഥിരം അക്ഷരം *intermed, അക്ഷരം command, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*osc)(പൂർണ്ണസംഖ്യ command, VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*dcs)(സ്ഥിരം അക്ഷരം *command, size_t commandlen, VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*apc)(VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*pm)(VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*sos)(VTermStringFragment frag, ശൂന്യം *user);
} VTermStateFallbacks;

തരനിർവചനം ഘടന {
  പൂർണ്ണസംഖ്യ (*set)(VTermSelectionMask mask, VTermStringFragment frag, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*query)(VTermSelectionMask mask, ശൂന്യം *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

ശൂന്യം  vterm_state_set_callbacks(VTermState *state, സ്ഥിരം VTermStateCallbacks *callbacks, ശൂന്യം *user);
ശൂന്യം *vterm_state_get_cbdata(VTermState *state);

ശൂന്യം vterm_state_callbacks_has_premove(VTermState *state);

ശൂന്യം  vterm_state_set_unrecognised_fallbacks(VTermState *state, സ്ഥിരം VTermStateFallbacks *fallbacks, ശൂന്യം *user);
ശൂന്യം *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
ശൂന്യം vterm_state_reset(VTermState *state, പൂർണ്ണസംഖ്യ hard);

ശൂന്യം vterm_state_get_cursorpos(സ്ഥിരം VTermState *state, VTermPos *cursorpos);
// VIM: added
ശൂന്യം vterm_state_get_mousestate(സ്ഥിരം VTermState *state, VTermMouseState *mousestate);
ശൂന്യം vterm_state_get_default_colors(സ്ഥിരം VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
ശൂന്യം vterm_state_get_palette_color(സ്ഥിരം VTermState *state, പൂർണ്ണസംഖ്യ index, VTermColor *col);
ശൂന്യം vterm_state_set_default_colors(VTermState *state, സ്ഥിരം VTermColor *default_fg, സ്ഥിരം VTermColor *default_bg);
ശൂന്യം vterm_state_set_palette_color(VTermState *state, പൂർണ്ണസംഖ്യ index, സ്ഥിരം VTermColor *col);
ശൂന്യം vterm_state_set_bold_highbright(VTermState *state, പൂർണ്ണസംഖ്യ bold_is_highbright);
പൂർണ്ണസംഖ്യ  vterm_state_get_penattr(സ്ഥിരം VTermState *state, VTermAttr attr, VTerm_മൂല്യം *val);
പൂർണ്ണസംഖ്യ  vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_മൂല്യം *val);
ശൂന്യം vterm_state_focus_in(VTermState *state);
ശൂന്യം vterm_state_focus_out(VTermState *state);
സ്ഥിരം VTerm_വരി_Info *vterm_state_get_lineinfo(സ്ഥിരം VTermState *state, പൂർണ്ണസംഖ്യ row);

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
ശൂന്യം vterm_state_convert_color_to_rgb(സ്ഥിരം VTermState *state, VTermColor *col);

ശൂന്യം vterm_state_set_selection_callbacks(VTermState *state, സ്ഥിരം VTermSelectionCallbacks *callbacks, ശൂന്യം *user,
    അക്ഷരം *buffer, size_t buflen);

ശൂന്യം vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

തരനിർവചനം ഘടന {
    unsigned പൂർണ്ണസംഖ്യ bold      : 1;
    unsigned പൂർണ്ണസംഖ്യ underline : 2;
    unsigned പൂർണ്ണസംഖ്യ italic    : 1;
    unsigned പൂർണ്ണസംഖ്യ blink     : 1;
    unsigned പൂർണ്ണസംഖ്യ reverse   : 1;
    unsigned പൂർണ്ണസംഖ്യ conceal   : 1;
    unsigned പൂർണ്ണസംഖ്യ strike    : 1;
    unsigned പൂർണ്ണസംഖ്യ font      : 4; /* 0 to 9 */
    unsigned പൂർണ്ണസംഖ്യ dwl       : 1; /* On a DECDWL or DECDHL line */
    unsigned പൂർണ്ണസംഖ്യ dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    unsigned പൂർണ്ണസംഖ്യ small     : 1;
    unsigned പൂർണ്ണസംഖ്യ baseline  : 2;
} VTermScreenCellAttrs;

എണ്ണൽ {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

എണ്ണൽ {
  VTERM_BASELINE_NORMAL,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_LOWER,
};

തരനിർവചനം ഘടന {
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  അക്ഷരം     width;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
തരനിർവചനം ഘടന {
  പൂർണ്ണസംഖ്യ (*damage)(VTermRect rect, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*moverect)(VTermRect dest, VTermRect src, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*movecursor)(VTermPos pos, VTermPos oldpos, പൂർണ്ണസംഖ്യ visible, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*settermprop)(VTermProp prop, VTerm_മൂല്യം *val, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*bell)(ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*resize)(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, ശൂന്യം *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  പൂർണ്ണസംഖ്യ (*sb_pushline)(പൂർണ്ണസംഖ്യ cols, സ്ഥിരം VTermScreenCell *cells, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*sb_popline)(പൂർണ്ണസംഖ്യ cols, VTermScreenCell *cells, ശൂന്യം *user);
  പൂർണ്ണസംഖ്യ (*sb_clear)(ശൂന്യം* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  പൂർണ്ണസംഖ്യ (*sb_pushline4)(പൂർണ്ണസംഖ്യ cols, സ്ഥിരം VTermScreenCell *cells, പൂർണ്ണസംഖ്യ continuation, ശൂന്യം *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
ശൂന്യം  vterm_screen_set_callbacks(VTermScreen *screen, സ്ഥിരം VTermScreenCallbacks *callbacks, ശൂന്യം *user);
ശൂന്യം *vterm_screen_get_cbdata(VTermScreen *screen);

ശൂന്യം vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

ശൂന്യം  vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, സ്ഥിരം VTermStateFallbacks *fallbacks, ശൂന്യം *user);
ശൂന്യം *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

ശൂന്യം vterm_screen_enable_reflow(VTermScreen *screen, പൂർണ്ണസംഖ്യ reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#define vterm_screen_set_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
ശൂന്യം vterm_screen_enable_altscreen(VTermScreen *screen, പൂർണ്ണസംഖ്യ altscreen);

തരനിർവചനം എണ്ണൽ {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ROW,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTermDamageSize;

// Invoke the relevant callbacks to update the screen.
ശൂന്യം vterm_screen_flush_damage(VTermScreen *screen);

ശൂന്യം vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
ശൂന്യം   vterm_screen_reset(VTermScreen *screen, പൂർണ്ണസംഖ്യ hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(സ്ഥിരം VTermScreen *screen, uint32_t *chars, size_t len, സ്ഥിരം VTermRect rect);
size_t vterm_screen_get_text(സ്ഥിരം VTermScreen *screen, അക്ഷരം *str, size_t len, സ്ഥിരം VTermRect rect);

തരനിർവചനം എണ്ണൽ {
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

പൂർണ്ണസംഖ്യ vterm_screen_get_attrs_extent(സ്ഥിരം VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs);

പൂർണ്ണസംഖ്യ vterm_screen_get_cell(സ്ഥിരം VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

പൂർണ്ണസംഖ്യ vterm_screen_is_eol(സ്ഥിരം VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
ശൂന്യം vterm_screen_convert_color_to_rgb(സ്ഥിരം VTermScreen *screen, VTermColor *col);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
ശൂന്യം vterm_screen_set_default_colors(VTermScreen *screen, സ്ഥിരം VTermColor *default_fg, സ്ഥിരം VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_മൂല്യം_Type vterm_get_attr_type(VTermAttr attr);
VTerm_മൂല്യം_Type vterm_get_prop_type(VTermProp prop);

ശൂന്യം vterm_scroll_rect(VTermRect rect,
                       പൂർണ്ണസംഖ്യ downward,
                       പൂർണ്ണസംഖ്യ rightward,
                       പൂർണ്ണസംഖ്യ (*moverect)(VTermRect src, VTermRect dest, ശൂന്യം *user),
                       പൂർണ്ണസംഖ്യ (*eraserect)(VTermRect rect, പൂർണ്ണസംഖ്യ selective, ശൂന്യം *user),
                       ശൂന്യം *user);

ശൂന്യം vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      ശൂന്യം (*copycell)(VTermPos dest, VTermPos src, ശൂന്യം *user),
                      ശൂന്യം *user);

#ifdef __cplusplus
}
#endif

#endif
