/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __VTERM_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __VTERM_H__

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
ਬਾਹਰੀ "C" {
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRUE 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FALSE 0

// VIM: from stdint.h
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ		uint8_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ		uint16_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ		uint32_t;

// VIM: define max screen cols and rows
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_MAX_ਖਾਨੇ_ਗਿਣਤੀ 1000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_MAX_ਪੰਕਤੀਆਂ 1000

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_ਸੰਸਕਰਣ_MAJOR 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_ਸੰਸਕਰਣ_MINOR 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_ਸੰਸਕਰਣ_PATCH 3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_CHECK_ਸੰਸਕਰਣ \
        vterm_check_ਸੰਸਕਰਣ(VTERM_ਸੰਸਕਰਣ_MAJOR, VTERM_ਸੰਸਕਰਣ_MINOR)

/* Any cell can contain at most one basic printing character and 5 combining
 * characters. This number could be changed but will be ABI-incompatible if
 * you do */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL 6

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VTerm VTerm;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VTermState VTermState;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ VTermScreen VTermScreen;

// Specifies a screen point.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਪੂਰਨ ਪੰਕਤੀ;
  ਪੂਰਨ ਖਾਨਾ;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
ਪੂਰਨ vterm_pos_cmp(VTermPos a, VTermPos b);

#ਜੇ defined(DEFINE_INLINES) || USE_INLINE
INLINE ਪੂਰਨ vterm_pos_cmp(VTermPos a, VTermPos b)
{
  ਵਾਪਸ (a.ਪੰਕਤੀ == b.ਪੰਕਤੀ) ? a.ਖਾਨਾ - b.ਖਾਨਾ : a.ਪੰਕਤੀ - b.ਪੰਕਤੀ;
}
#ਜੇ_ਅੰਤ

// Specifies a rectangular screen area.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਪੂਰਨ ਸ਼ੁਰੂ_ਪੰਕਤੀ;
  ਪੂਰਨ ਸਿਰਾ_ਪੰਕਤੀ;
  ਪੂਰਨ ਸ਼ੁਰੂ_ਖਾਨਾ;
  ਪੂਰਨ ਸਿਰਾ_ਖਾਨਾ;
} VTermRect;

/* true if the rect contains the point */
ਪੂਰਨ vterm_rect_ਸ਼ਾਮਲ_ਹੈ(VTermRect r, VTermPos p);

#ਜੇ defined(DEFINE_INLINES) || USE_INLINE
INLINE ਪੂਰਨ vterm_rect_ਸ਼ਾਮਲ_ਹੈ(VTermRect r, VTermPos p)
{
  ਵਾਪਸ p.ਪੰਕਤੀ >= r.ਸ਼ੁਰੂ_ਪੰਕਤੀ && p.ਪੰਕਤੀ < r.ਸਿਰਾ_ਪੰਕਤੀ &&
         p.ਖਾਨਾ >= r.ਸ਼ੁਰੂ_ਖਾਨਾ && p.ਖਾਨਾ < r.ਸਿਰਾ_ਖਾਨਾ;
}
#ਜੇ_ਅੰਤ

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
ਖਾਲੀ vterm_rect_move(VTermRect *rect, ਪੂਰਨ ਪੰਕਤੀ_delta, ਪੂਰਨ ਖਾਨਾ_delta);

#ਜੇ defined(DEFINE_INLINES) || USE_INLINE
INLINE ਖਾਲੀ vterm_rect_move(VTermRect *rect, ਪੂਰਨ ਪੰਕਤੀ_delta, ਪੂਰਨ ਖਾਨਾ_delta)
{
  rect->ਸ਼ੁਰੂ_ਪੰਕਤੀ += ਪੰਕਤੀ_delta; rect->ਸਿਰਾ_ਪੰਕਤੀ += ਪੰਕਤੀ_delta;
  rect->ਸ਼ੁਰੂ_ਖਾਨਾ += ਖਾਨਾ_delta; rect->ਸਿਰਾ_ਖਾਨਾ += ਖਾਨਾ_delta;
}
#ਜੇ_ਅੰਤ

/**
 * Bit-field describing the value of VTermColor.type
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  /**
   * If the lower bit of `type` is not set, the colour is 24-bit RGB.
   */
  VTERM_COLOR_RGB = 0x00,

  /**
   * The colour is an index into a palette of 256 colours.
   */
  VTERM_COLOR_ਸੂਚਕਾਂਕਿਤ = 0x01,

  /**
   * Mask that can be used to extract the RGB/Indexed bit.
   */
  VTERM_COLOR_TYPE_ਨਕਾਬ = 0x01,

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
  VTERM_COLOR_DEFAULT_ਨਕਾਬ = 0x06,

  /**
   * VIM: If set, indicates that the color is invalid.
   */
  VTERM_COLOR_INVALID = 0x08
} VTermColorType;

/**
 * Returns true if the VTERM_COLOR_RGB `type` flag is set, indicating that the
 * given VTermColor instance is an indexed colour.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(ਖਾਨਾ) \
  (((ਖਾਨਾ)->type & VTERM_COLOR_TYPE_ਨਕਾਬ) == VTERM_COLOR_ਸੂਚਕਾਂਕਿਤ)

/**
 * Returns true if the VTERM_COLOR_INDEXED `type` flag is set, indicating that
 * the given VTermColor instance is an rgb colour.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_COLOR_IS_RGB(ਖਾਨਾ) \
  (((ਖਾਨਾ)->type & VTERM_COLOR_TYPE_ਨਕਾਬ) == VTERM_COLOR_RGB)

/**
 * Returns true if the VTERM_COLOR_DEFAULT_FG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default foreground
 * color.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_COLOR_IS_DEFAULT_FG(ਖਾਨਾ) \
  (!!((ਖਾਨਾ)->type & VTERM_COLOR_DEFAULT_FG))

/**
 * Returns true if the VTERM_COLOR_DEFAULT_BG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default background
 * color.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_COLOR_IS_DEFAULT_BG(ਖਾਨਾ) \
  (!!((ਖਾਨਾ)->type & VTERM_COLOR_DEFAULT_BG))

/**
 * Returns true if the VTERM_COLOR_INVALID `type` flag is set, indicating
 * that the given VTermColor instance is an invalid color.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_COLOR_IS_INVALID(ਖਾਨਾ) (!!((ਖਾਨਾ)->type & VTERM_COLOR_INVALID))

// VIM: this was a union, but that doesn't always work.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  /**
   * Tag indicating which member is actually valid.
   * Please use the `VTERM_COLOR_IS_*` test macros to check whether a
   * particular type flag is set.
   */
  uint8_t type;

  uint8_t red, green, blue;

  uint8_t ਸੂਚਕਾਂਕ;
} VTermColor;

/**
 * Constructs a new VTermColor instance representing the given RGB values.
 */
ਖਾਲੀ vterm_color_rgb(VTermColor *ਖਾਨਾ, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
ਖਾਲੀ vterm_color_ਸੂਚਕਾਂਕਿਤ(VTermColor *ਖਾਨਾ, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
ਪੂਰਨ vterm_color_is_equal(ਅਚਲ VTermColor *a, ਅਚਲ VTermColor *b);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_ਮੁੱਲ_Type;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਅਚਲ ਅੱਖਰ *str;
  size_t      len : 30;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  initial : 1;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ  final : 1;
} VTermStringFragment;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਸੰਘ {
  ਪੂਰਨ boolean;
  ਪੂਰਨ ਸੰਖਿਆ;
  VTermStringFragment string;
  VTermColor color;
} VTerm_ਮੁੱਲ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  /* VTERM_ATTR_NONE = 0 */
  VTERM_ATTR_BOLD = 1,   // bool:   1, 22
  VTERM_ATTR_UNDERLINE,  // number: 4, 21, 24
  VTERM_ATTR_ITALIC,     // bool:   3, 23
  VTERM_ATTR_BLINK,      // bool:   5, 25
  VTERM_ATTR_ਉਲਟ,    // bool:   7, 27
  VTERM_ATTR_CONCEAL,    // bool:   8, 28
  VTERM_ATTR_STRIKE,     // bool:   9, 29
  VTERM_ATTR_FONT,       // number: 10-19
  VTERM_ATTR_FOREGROUND, // color:  30-39 90-97
  VTERM_ATTR_BACKGROUND, // color:  40-49 100-107
  VTERM_ATTR_SMALL,      // bool:   73, 74, 75
  VTERM_ATTR_BASELINE,   // number: 73, 74, 75

  VTERM_N_ATTRS
} VTermAttr;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_ਉਲਟ,           // bool
  VTERM_PROP_CURSORSHAPE,       //ਸੰਖਿਆ
  VTERM_PROP_MOUSE,             //ਸੰਖਿਆ
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

ਗਿਣਤੀ {
  VTERM_PROP_CURSORSHAPE_ਖੰਡ = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_ਖੱਬਾ,

  VTERM_N_PROP_CURSORSHAPES
};

ਗਿਣਤੀ {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTerm_Selection_ਨਕਾਬ;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਅਚਲ uint32_t *chars;
  ਪੂਰਨ             ਚੌੜਾਈ;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    dwl:1;             /* DECDWL or DECDHL double-width line */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    doublewidth:1;     /* DECDWL or DECDHL line */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_ਸਤਰ_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  VTermPos pos;                /* current cursor position */
  VTerm_ਸਤਰ_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  ਖਾਲੀ *(*ਸਮ੍ਰਿਤੀ_ਦਿਓ)(size_t ਮਾਪ, ਖਾਲੀ *allocdata);
  ਖਾਲੀ  (*ਮੁਕਤ)(ਖਾਲੀ *ptr, ਖਾਲੀ *allocdata);
} VTermAllocatorFunctions;

ਖਾਲੀ vterm_check_ਸੰਸਕਰਣ(ਪੂਰਨ major, ਪੂਰਨ minor);

ਬਣਤਰ VTermBuilder {
  ਪੂਰਨ ver; /* currently unused but reserved for some sort of ABI version flag */

  ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ;

  ਅਚਲ VTermAllocatorFunctions *allocator;
  ਖਾਲੀ *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ਅਚਲ ਬਣਤਰ VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, VTermAllocatorFunctions *funcs, ਖਾਲੀ *allocdata);

// Free and cleanup a terminal and all its data.
ਖਾਲੀ   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
ਖਾਲੀ vterm_get_ਮਾਪ(ਅਚਲ VTerm *vt, ਪੂਰਨ *rowsp, ਪੂਰਨ *colsp);

ਖਾਲੀ vterm_ਸੈੱਟ_ਮਾਪ(VTerm *vt, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ);

ਪੂਰਨ  vterm_get_utf8(ਅਚਲ VTerm *vt);
ਖਾਲੀ vterm_ਸੈੱਟ_utf8(VTerm *vt, ਪੂਰਨ is_utf8);

size_t vterm_ਨਿਵੇਸ਼_write(VTerm *vt, ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len);

/* Setting output callback will override the buffer logic */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ VTermOutputCallback(ਅਚਲ ਅੱਖਰ *s, size_t len, ਖਾਲੀ *user);
ਖਾਲੀ vterm_output_ਸੈੱਟ_callback(VTerm *vt, VTermOutputCallback *func, ਖਾਲੀ *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_ਬਫਰ_ਮਾਪ(ਅਚਲ VTerm *vt);
size_t vterm_output_get_ਬਫਰ_current(ਅਚਲ VTerm *vt);
size_t vterm_output_get_ਬਫਰ_ਬਚਿਆ(ਅਚਲ VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, ਅੱਖਰ *ਬਫਰ, size_t len);

ਪੂਰਨ vterm_is_modify_other_ਕੁੰਜੀਆਂ(VTerm *vt);
ਪੂਰਨ vterm_is_kitty_keyboard(VTerm *vt);
ਖਾਲੀ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
ਖਾਲੀ vterm_keyboard_ਕੁੰਜੀ(VTerm *vt, VTerm_ਕੁੰਜੀ ਕੁੰਜੀ, VTermModifier mod);

ਖਾਲੀ vterm_keyboard_ਸ਼ੁਰੂ_paste(VTerm *vt);
ਖਾਲੀ vterm_keyboard_ਸਿਰਾ_paste(VTerm *vt);

ਖਾਲੀ vterm_mouse_move(VTerm *vt, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
ਖਾਲੀ vterm_mouse_button(VTerm *vt, ਪੂਰਨ button, ਪੂਰਨ pressed, VTermModifier mod);

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG_FLAG_ਹੋਰ (1U<<31)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG_ਨਕਾਬ      (~(1U<<31))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG_HAS_ਹੋਰ(a) ((a) & CSI_ARG_FLAG_ਹੋਰ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG(a)          ((a) & CSI_ARG_ਨਕਾਬ)

/* Can't use -1 to indicate a missing argument; use this instead */
// VIM: changed 31 to 30 to avoid an overflow warning
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG_MISSING ((1<<30)-1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG_IS_MISSING(a) (CSI_ARG(a) == CSI_ARG_MISSING)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG_OR(a,def)     (CSI_ARG(a) == CSI_ARG_MISSING ? (def) : CSI_ARG(a))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI_ARG_ਗਿਣਤੀ(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਪੂਰਨ (*ਪਾਠ)(ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len, ਖਾਲੀ *user);
  ਪੂਰਨ (*control)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ control, ਖਾਲੀ *user);
  ਪੂਰਨ (*escape)(ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len, ਖਾਲੀ *user);
  ਪੂਰਨ (*csi)(ਅਚਲ ਅੱਖਰ *leader, ਅਚਲ ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount, ਅਚਲ ਅੱਖਰ *intermed, ਅੱਖਰ command, ਖਾਲੀ *user);
  ਪੂਰਨ (*osc)(ਪੂਰਨ command, VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*dcs)(ਅਚਲ ਅੱਖਰ *command, size_t commandlen, VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*apc)(VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*pm)(VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*sos)(VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*resize)(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਖਾਲੀ *user);
} VTermParserCallbacks;

ਖਾਲੀ  vterm_parser_ਸੈੱਟ_callbacks(VTerm *vt, ਅਚਲ VTermParserCallbacks *callbacks, ਖਾਲੀ *user);
ਖਾਲੀ *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
ਖਾਲੀ vterm_parser_ਸੈੱਟ_emit_nul(VTerm *vt, ਪੂਰਨ emit);

// -----------
// State layer
// -----------

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਪੂਰਨ (*putglyph)(VTermGlyphInfo *info, VTermPos pos, ਖਾਲੀ *user);
  ਪੂਰਨ (*movecursor)(VTermPos pos, VTermPos oldpos, ਪੂਰਨ visible, ਖਾਲੀ *user);
  ਪੂਰਨ (*scrollrect)(VTermRect rect, ਪੂਰਨ downward, ਪੂਰਨ rightward, ਖਾਲੀ *user);
  ਪੂਰਨ (*moverect)(VTermRect dest, VTermRect src, ਖਾਲੀ *user);
  ਪੂਰਨ (*erase)(VTermRect rect, ਪੂਰਨ selective, ਖਾਲੀ *user);
  ਪੂਰਨ (*initpen)(ਖਾਲੀ *user);
  ਪੂਰਨ (*setpenattr)(VTermAttr attr, VTerm_ਮੁੱਲ *val, ਖਾਲੀ *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  ਪੂਰਨ (*settermprop)(VTermProp prop, VTerm_ਮੁੱਲ *val, ਖਾਲੀ *user);
  ਪੂਰਨ (*bell)(ਖਾਲੀ *user);
  ਪੂਰਨ (*resize)(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, VTermStateFields *fields, ਖਾਲੀ *user);
  ਪੂਰਨ (*setlineinfo)(ਪੂਰਨ ਪੰਕਤੀ, ਅਚਲ VTerm_ਸਤਰ_Info *newinfo, ਅਚਲ VTerm_ਸਤਰ_Info *oldinfo, ਖਾਲੀ *user);
  ਪੂਰਨ (*sb_clear)(ਖਾਲੀ *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  ਪੂਰਨ (*premove)(VTermRect dest, ਖਾਲੀ *user);
} VTermStateCallbacks;

// VIM: added
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  VTermPos pos;
  ਪੂਰਨ	   buttons;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_BUTTON_ਖੱਬਾ 0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_BUTTON_ਵਿਚਕਾਰ 0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_BUTTON_ਸੱਜਾ 0x04
  ਪੂਰਨ      flags;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_WANT_CLICK 0x01
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_WANT_DRAG  0x02
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਪੂਰਨ (*control)(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ control, ਖਾਲੀ *user);
  ਪੂਰਨ (*csi)(ਅਚਲ ਅੱਖਰ *leader, ਅਚਲ ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount, ਅਚਲ ਅੱਖਰ *intermed, ਅੱਖਰ command, ਖਾਲੀ *user);
  ਪੂਰਨ (*osc)(ਪੂਰਨ command, VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*dcs)(ਅਚਲ ਅੱਖਰ *command, size_t commandlen, VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*apc)(VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*pm)(VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*sos)(VTermStringFragment frag, ਖਾਲੀ *user);
} VTermStateFallbacks;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਪੂਰਨ (*ਸੈੱਟ_2)(VTerm_Selection_ਨਕਾਬ ਨਕਾਬ, VTermStringFragment frag, ਖਾਲੀ *user);
  ਪੂਰਨ (*query)(VTerm_Selection_ਨਕਾਬ ਨਕਾਬ, ਖਾਲੀ *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

ਖਾਲੀ  vterm_state_ਸੈੱਟ_callbacks(VTermState *state, ਅਚਲ VTermStateCallbacks *callbacks, ਖਾਲੀ *user);
ਖਾਲੀ *vterm_state_get_cbdata(VTermState *state);

ਖਾਲੀ vterm_state_callbacks_has_premove(VTermState *state);

ਖਾਲੀ  vterm_state_ਸੈੱਟ_unrecognised_fallbacks(VTermState *state, ਅਚਲ VTermStateFallbacks *fallbacks, ਖਾਲੀ *user);
ਖਾਲੀ *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
ਖਾਲੀ vterm_state_reset(VTermState *state, ਪੂਰਨ hard);

ਖਾਲੀ vterm_state_get_cursorpos(ਅਚਲ VTermState *state, VTermPos *cursorpos);
// VIM: added
ਖਾਲੀ vterm_state_get_mousestate(ਅਚਲ VTermState *state, VTermMouseState *mousestate);
ਖਾਲੀ vterm_state_get_default_colors(ਅਚਲ VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
ਖਾਲੀ vterm_state_get_palette_color(ਅਚਲ VTermState *state, ਪੂਰਨ ਸੂਚਕਾਂਕ, VTermColor *ਖਾਨਾ);
ਖਾਲੀ vterm_state_ਸੈੱਟ_default_colors(VTermState *state, ਅਚਲ VTermColor *default_fg, ਅਚਲ VTermColor *default_bg);
ਖਾਲੀ vterm_state_ਸੈੱਟ_palette_color(VTermState *state, ਪੂਰਨ ਸੂਚਕਾਂਕ, ਅਚਲ VTermColor *ਖਾਨਾ);
ਖਾਲੀ vterm_state_ਸੈੱਟ_bold_highbright(VTermState *state, ਪੂਰਨ bold_is_highbright);
ਪੂਰਨ  vterm_state_get_penattr(ਅਚਲ VTermState *state, VTermAttr attr, VTerm_ਮੁੱਲ *val);
ਪੂਰਨ  vterm_state_ਸੈੱਟ_termprop(VTermState *state, VTermProp prop, VTerm_ਮੁੱਲ *val);
ਖਾਲੀ vterm_state_focus_ਵਿੱਚ(VTermState *state);
ਖਾਲੀ vterm_state_focus_ਬਾਹਰਲਾ(VTermState *state);
ਅਚਲ VTerm_ਸਤਰ_Info *vterm_state_get_lineinfo(ਅਚਲ VTermState *state, ਪੂਰਨ ਪੰਕਤੀ);

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
ਖਾਲੀ vterm_state_convert_color_ਮੰਜ਼ਿਲ_rgb(ਅਚਲ VTermState *state, VTermColor *ਖਾਨਾ);

ਖਾਲੀ vterm_state_ਸੈੱਟ_selection_callbacks(VTermState *state, ਅਚਲ VTermSelectionCallbacks *callbacks, ਖਾਲੀ *user,
    ਅੱਖਰ *ਬਫਰ, size_t buflen);

ਖਾਲੀ vterm_state_send_selection(VTermState *state, VTerm_Selection_ਨਕਾਬ ਨਕਾਬ, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ bold      : 1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ underline : 2;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ italic    : 1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ blink     : 1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਉਲਟ   : 1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ conceal   : 1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ strike    : 1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ font      : 4; /* 0 to 9 */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ dwl       : 1; /* On a DECDWL or DECDHL line */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ small     : 1;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ baseline  : 2;
} VTermScreenCellAttrs;

ਗਿਣਤੀ {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

ਗਿਣਤੀ {
  VTERM_BASELINE_ਸਾਧਾਰਨ,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_ਹੇਠਲਾ,
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  uint32_t chars[VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL];
  ਅੱਖਰ     ਚੌੜਾਈ;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਪੂਰਨ (*damage)(VTermRect rect, ਖਾਲੀ *user);
  ਪੂਰਨ (*moverect)(VTermRect dest, VTermRect src, ਖਾਲੀ *user);
  ਪੂਰਨ (*movecursor)(VTermPos pos, VTermPos oldpos, ਪੂਰਨ visible, ਖਾਲੀ *user);
  ਪੂਰਨ (*settermprop)(VTermProp prop, VTerm_ਮੁੱਲ *val, ਖਾਲੀ *user);
  ਪੂਰਨ (*bell)(ਖਾਲੀ *user);
  ਪੂਰਨ (*resize)(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਖਾਲੀ *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  ਪੂਰਨ (*sb_pushline)(ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਅਚਲ VTermScreenCell *cells, ਖਾਲੀ *user);
  ਪੂਰਨ (*sb_popline)(ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, VTermScreenCell *cells, ਖਾਲੀ *user);
  ਪੂਰਨ (*sb_clear)(ਖਾਲੀ* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  ਪੂਰਨ (*sb_pushline4)(ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਅਚਲ VTermScreenCell *cells, ਪੂਰਨ continuation, ਖਾਲੀ *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
ਖਾਲੀ  vterm_screen_ਸੈੱਟ_callbacks(VTermScreen *screen, ਅਚਲ VTermScreenCallbacks *callbacks, ਖਾਲੀ *user);
ਖਾਲੀ *vterm_screen_get_cbdata(VTermScreen *screen);

ਖਾਲੀ vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

ਖਾਲੀ  vterm_screen_ਸੈੱਟ_unrecognised_fallbacks(VTermScreen *screen, ਅਚਲ VTermStateFallbacks *fallbacks, ਖਾਲੀ *user);
ਖਾਲੀ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

ਖਾਲੀ vterm_screen_enable_reflow(VTermScreen *screen, ਪੂਰਨ reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vterm_screen_ਸੈੱਟ_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
ਖਾਲੀ vterm_screen_enable_altscreen(VTermScreen *screen, ਪੂਰਨ altscreen);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ਪੰਕਤੀ,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTerm_Damage_ਮਾਪ;

// Invoke the relevant callbacks to update the screen.
ਖਾਲੀ vterm_screen_flush_damage(VTermScreen *screen);

ਖਾਲੀ vterm_screen_ਸੈੱਟ_damage_ਮਿਲਾਓ(VTermScreen *screen, VTerm_Damage_ਮਾਪ ਮਾਪ);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
ਖਾਲੀ   vterm_screen_reset(VTermScreen *screen, ਪੂਰਨ hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ਅਚਲ VTermScreen *screen, uint32_t *chars, size_t len, ਅਚਲ VTermRect rect);
size_t vterm_screen_get_ਪਾਠ(ਅਚਲ VTermScreen *screen, ਅੱਖਰ *str, size_t len, ਅਚਲ VTermRect rect);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  VTERM_ATTR_BOLD_ਨਕਾਬ       = 1 << 0,
  VTERM_ATTR_UNDERLINE_ਨਕਾਬ  = 1 << 1,
  VTERM_ATTR_ITALIC_ਨਕਾਬ     = 1 << 2,
  VTERM_ATTR_BLINK_ਨਕਾਬ      = 1 << 3,
  VTERM_ATTR_ਉਲਟ_ਨਕਾਬ    = 1 << 4,
  VTERM_ATTR_STRIKE_ਨਕਾਬ     = 1 << 5,
  VTERM_ATTR_FONT_ਨਕਾਬ       = 1 << 6,
  VTERM_ATTR_FOREGROUND_ਨਕਾਬ = 1 << 7,
  VTERM_ATTR_BACKGROUND_ਨਕਾਬ = 1 << 8,
  VTERM_ATTR_CONCEAL_ਨਕਾਬ    = 1 << 9,
  VTERM_ATTR_SMALL_ਨਕਾਬ      = 1 << 10,
  VTERM_ATTR_BASELINE_ਨਕਾਬ   = 1 << 11,

  VTERM_ALL_ATTRS_ਨਕਾਬ = (1 << 12) - 1
} VTerm_Attr_ਨਕਾਬ;

ਪੂਰਨ vterm_screen_get_attrs_extent(ਅਚਲ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ਨਕਾਬ attrs);

ਪੂਰਨ vterm_screen_get_cell(ਅਚਲ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

ਪੂਰਨ vterm_screen_is_eol(ਅਚਲ VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
ਖਾਲੀ vterm_screen_convert_color_ਮੰਜ਼ਿਲ_rgb(ਅਚਲ VTermScreen *screen, VTermColor *ਖਾਨਾ);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
ਖਾਲੀ vterm_screen_ਸੈੱਟ_default_colors(VTermScreen *screen, ਅਚਲ VTermColor *default_fg, ਅਚਲ VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_ਮੁੱਲ_Type vterm_get_attr_type(VTermAttr attr);
VTerm_ਮੁੱਲ_Type vterm_get_prop_type(VTermProp prop);

ਖਾਲੀ vterm_scroll_rect(VTermRect rect,
                       ਪੂਰਨ downward,
                       ਪੂਰਨ rightward,
                       ਪੂਰਨ (*moverect)(VTermRect src, VTermRect dest, ਖਾਲੀ *user),
                       ਪੂਰਨ (*eraserect)(VTermRect rect, ਪੂਰਨ selective, ਖਾਲੀ *user),
                       ਖਾਲੀ *user);

ਖਾਲੀ vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      ਖਾਲੀ (*copycell)(VTermPos dest, VTermPos src, ਖਾਲੀ *user),
                      ਖਾਲੀ *user);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __cplusplus
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ
