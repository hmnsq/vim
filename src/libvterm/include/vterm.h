/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#اگر_غیر_تعریف_شدہ __VTERM_H__
#تعریف_کرو __VTERM_H__

#اگر_تعریف_شدہ __cplusplus
خارجی "C" {
#اگر_ختم

#شامل <stdlib.h>

#شامل "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#تعریف_کرو TRUE 1
#تعریف_کرو FALSE 0

// VIM: from stdint.h
قسمتعریف غیرعلامتی حرف		uint8_t;
قسمتعریف غیرعلامتی مختصر		uint16_t;
قسمتعریف غیرعلامتی صحیح		uint32_t;

// VIM: define max screen cols and rows
#تعریف_کرو VTERM_MAX_ستون_تعداد 1000
#تعریف_کرو VTERM_MAX_صفیں 1000

#تعریف_کرو VTERM_نسخہ_MAJOR 0
#تعریف_کرو VTERM_نسخہ_MINOR 3
#تعریف_کرو VTERM_نسخہ_PATCH 3

#تعریف_کرو VTERM_CHECK_نسخہ \
        vterm_check_نسخہ(VTERM_نسخہ_MAJOR, VTERM_نسخہ_MINOR)

/* Any cell can contain at most one basic printing character and 5 combining
 * characters. This number could be changed but will be ABI-incompatible if
 * you do */
#تعریف_کرو VTERM_MAX_CHARS_فی_CELL 6

قسمتعریف ساخت VTerm VTerm;
قسمتعریف ساخت VTermState VTermState;
قسمتعریف ساخت VTermScreen VTermScreen;

// Specifies a screen point.
قسمتعریف ساخت {
  صحیح صف;
  صحیح ستون;
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
  واپس (a.صف == b.صف) ? a.ستون - b.ستون : a.صف - b.صف;
}
#اگر_ختم

// Specifies a rectangular screen area.
قسمتعریف ساخت {
  صحیح آغاز_صف;
  صحیح انتہا_صف;
  صحیح آغاز_ستون;
  صحیح انتہا_ستون;
} VTermRect;

/* true if the rect contains the point */
صحیح vterm_rect_شامل_ہے(VTermRect r, VTermPos p);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE صحیح vterm_rect_شامل_ہے(VTermRect r, VTermPos p)
{
  واپس p.صف >= r.آغاز_صف && p.صف < r.انتہا_صف &&
         p.ستون >= r.آغاز_ستون && p.ستون < r.انتہا_ستون;
}
#اگر_ختم

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
خالی vterm_rect_move(VTermRect *rect, صحیح صف_delta, صحیح ستون_delta);

#اگر defined(DEFINE_INLINES) || USE_INLINE
INLINE خالی vterm_rect_move(VTermRect *rect, صحیح صف_delta, صحیح ستون_delta)
{
  rect->آغاز_صف += صف_delta; rect->انتہا_صف += صف_delta;
  rect->آغاز_ستون += ستون_delta; rect->انتہا_ستون += ستون_delta;
}
#اگر_ختم

/**
 * Bit-field describing the value of VTermColor.type
 */
قسمتعریف شمار {
  /**
   * If the lower bit of `type` is not set, the colour is 24-bit RGB.
   */
  VTERM_COLOR_RGB = 0x00,

  /**
   * The colour is an index into a palette of 256 colours.
   */
  VTERM_COLOR_اشاریہ_دار = 0x01,

  /**
   * Mask that can be used to extract the RGB/Indexed bit.
   */
  VTERM_COLOR_TYPE_نقاب = 0x01,

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
  VTERM_COLOR_DEFAULT_نقاب = 0x06,

  /**
   * VIM: If set, indicates that the color is invalid.
   */
  VTERM_COLOR_INVALID = 0x08
} VTermColorType;

/**
 * Returns true if the VTERM_COLOR_RGB `type` flag is set, indicating that the
 * given VTermColor instance is an indexed colour.
 */
#تعریف_کرو VTERM_COLOR_IS_اشاریہ_دار(ستون) \
  (((ستون)->type & VTERM_COLOR_TYPE_نقاب) == VTERM_COLOR_اشاریہ_دار)

/**
 * Returns true if the VTERM_COLOR_INDEXED `type` flag is set, indicating that
 * the given VTermColor instance is an rgb colour.
 */
#تعریف_کرو VTERM_COLOR_IS_RGB(ستون) \
  (((ستون)->type & VTERM_COLOR_TYPE_نقاب) == VTERM_COLOR_RGB)

/**
 * Returns true if the VTERM_COLOR_DEFAULT_FG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default foreground
 * color.
 */
#تعریف_کرو VTERM_COLOR_IS_DEFAULT_FG(ستون) \
  (!!((ستون)->type & VTERM_COLOR_DEFAULT_FG))

/**
 * Returns true if the VTERM_COLOR_DEFAULT_BG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default background
 * color.
 */
#تعریف_کرو VTERM_COLOR_IS_DEFAULT_BG(ستون) \
  (!!((ستون)->type & VTERM_COLOR_DEFAULT_BG))

/**
 * Returns true if the VTERM_COLOR_INVALID `type` flag is set, indicating
 * that the given VTermColor instance is an invalid color.
 */
#تعریف_کرو VTERM_COLOR_IS_INVALID(ستون) (!!((ستون)->type & VTERM_COLOR_INVALID))

// VIM: this was a union, but that doesn't always work.
قسمتعریف ساخت {
  /**
   * Tag indicating which member is actually valid.
   * Please use the `VTERM_COLOR_IS_*` test macros to check whether a
   * particular type flag is set.
   */
  uint8_t type;

  uint8_t red, green, blue;

  uint8_t اشاریہ;
} VTermColor;

/**
 * Constructs a new VTermColor instance representing the given RGB values.
 */
خالی vterm_color_rgb(VTermColor *ستون, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
خالی vterm_color_اشاریہ_دار(VTermColor *ستون, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
صحیح vterm_color_is_equal(ثابت VTermColor *a, ثابت VTermColor *b);

قسمتعریف شمار {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_قدر_Type;

قسمتعریف ساخت {
  ثابت حرف *str;
  size_t      len : 30;
  غیرعلامتی صحیح  initial : 1;
  غیرعلامتی صحیح  final : 1;
} VTermStringFragment;

قسمتعریف اتحاد {
  صحیح boolean;
  صحیح نمبر;
  VTermStringFragment string;
  VTermColor color;
} VTerm_قدر;

قسمتعریف شمار {
  /* VTERM_ATTR_NONE = 0 */
  VTERM_ATTR_BOLD = 1,   // bool:   1, 22
  VTERM_ATTR_UNDERLINE,  // number: 4, 21, 24
  VTERM_ATTR_ITALIC,     // bool:   3, 23
  VTERM_ATTR_BLINK,      // bool:   5, 25
  VTERM_ATTR_الٹ,    // bool:   7, 27
  VTERM_ATTR_CONCEAL,    // bool:   8, 28
  VTERM_ATTR_STRIKE,     // bool:   9, 29
  VTERM_ATTR_FONT,       // number: 10-19
  VTERM_ATTR_FOREGROUND, // color:  30-39 90-97
  VTERM_ATTR_BACKGROUND, // color:  40-49 100-107
  VTERM_ATTR_SMALL,      // bool:   73, 74, 75
  VTERM_ATTR_BASELINE,   // number: 73, 74, 75

  VTERM_N_ATTRS
} VTermAttr;

قسمتعریف شمار {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_الٹ,           // bool
  VTERM_PROP_CURSORSHAPE,       //نمبر
  VTERM_PROP_MOUSE,             //نمبر
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

شمار {
  VTERM_PROP_CURSORSHAPE_بلاک = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_بایاں,

  VTERM_N_PROP_CURSORSHAPES
};

شمار {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

قسمتعریف شمار {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTerm_Selection_نقاب;

قسمتعریف ساخت {
  ثابت uint32_t *chars;
  صحیح             چوڑائی;
  غیرعلامتی صحیح    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  غیرعلامتی صحیح    dwl:1;             /* DECDWL or DECDHL double-width line */
  غیرعلامتی صحیح    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

قسمتعریف ساخت {
  غیرعلامتی صحیح    doublewidth:1;     /* DECDWL or DECDHL line */
  غیرعلامتی صحیح    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  غیرعلامتی صحیح    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_سطر_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
قسمتعریف ساخت {
  VTermPos pos;                /* current cursor position */
  VTerm_سطر_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

قسمتعریف ساخت {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  خالی *(*حافظہ_دو)(size_t سائز, خالی *allocdata);
  خالی  (*آزاد)(خالی *ptr, خالی *allocdata);
} VTermAllocatorFunctions;

خالی vterm_check_نسخہ(صحیح major, صحیح minor);

ساخت VTermBuilder {
  صحیح ver; /* currently unused but reserved for some sort of ABI version flag */

  صحیح صفیں_2, ستون_تعداد;

  ثابت VTermAllocatorFunctions *allocator;
  خالی *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ثابت ساخت VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(صحیح صفیں_2, صحیح ستون_تعداد);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(صحیح صفیں_2, صحیح ستون_تعداد, VTermAllocatorFunctions *funcs, خالی *allocdata);

// Free and cleanup a terminal and all its data.
خالی   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
خالی vterm_get_سائز(ثابت VTerm *vt, صحیح *rowsp, صحیح *colsp);

خالی vterm_سیٹ_سائز(VTerm *vt, صحیح صفیں_2, صحیح ستون_تعداد);

صحیح  vterm_get_utf8(ثابت VTerm *vt);
خالی vterm_سیٹ_utf8(VTerm *vt, صحیح is_utf8);

size_t vterm_داخلہ_write(VTerm *vt, ثابت حرف *بائٹیں, size_t len);

/* Setting output callback will override the buffer logic */
قسمتعریف خالی VTermOutputCallback(ثابت حرف *s, size_t len, خالی *user);
خالی vterm_output_سیٹ_callback(VTerm *vt, VTermOutputCallback *func, خالی *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_بفر_سائز(ثابت VTerm *vt);
size_t vterm_output_get_بفر_current(ثابت VTerm *vt);
size_t vterm_output_get_بفر_بقایا(ثابت VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, حرف *بفر, size_t len);

صحیح vterm_is_modify_other_کلیدیں(VTerm *vt);
صحیح vterm_is_kitty_keyboard(VTerm *vt);
خالی vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
خالی vterm_keyboard_کلید(VTerm *vt, VTerm_کلید کلید, VTermModifier mod);

خالی vterm_keyboard_آغاز_paste(VTerm *vt);
خالی vterm_keyboard_انتہا_paste(VTerm *vt);

خالی vterm_mouse_move(VTerm *vt, صحیح صف, صحیح ستون, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
خالی vterm_mouse_button(VTerm *vt, صحیح button, صحیح pressed, VTermModifier mod);

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
#تعریف_کرو CSI_ARG_FLAG_مزید (1U<<31)
#تعریف_کرو CSI_ARG_نقاب      (~(1U<<31))

#تعریف_کرو CSI_ARG_HAS_مزید(a) ((a) & CSI_ARG_FLAG_مزید)
#تعریف_کرو CSI_ARG(a)          ((a) & CSI_ARG_نقاب)

/* Can't use -1 to indicate a missing argument; use this instead */
// VIM: changed 31 to 30 to avoid an overflow warning
#تعریف_کرو CSI_ARG_MISSING ((1<<30)-1)

#تعریف_کرو CSI_ARG_IS_MISSING(a) (CSI_ARG(a) == CSI_ARG_MISSING)
#تعریف_کرو CSI_ARG_OR(a,def)     (CSI_ARG(a) == CSI_ARG_MISSING ? (def) : CSI_ARG(a))
#تعریف_کرو CSI_ARG_گنتی(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

قسمتعریف ساخت {
  صحیح (*متن)(ثابت حرف *بائٹیں, size_t len, خالی *user);
  صحیح (*control)(غیرعلامتی حرف control, خالی *user);
  صحیح (*escape)(ثابت حرف *بائٹیں, size_t len, خالی *user);
  صحیح (*csi)(ثابت حرف *leader, ثابت طویل دلائل[], صحیح argcount, ثابت حرف *intermed, حرف command, خالی *user);
  صحیح (*osc)(صحیح command, VTermStringFragment frag, خالی *user);
  صحیح (*dcs)(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, خالی *user);
  صحیح (*apc)(VTermStringFragment frag, خالی *user);
  صحیح (*pm)(VTermStringFragment frag, خالی *user);
  صحیح (*sos)(VTermStringFragment frag, خالی *user);
  صحیح (*resize)(صحیح صفیں_2, صحیح ستون_تعداد, خالی *user);
} VTermParserCallbacks;

خالی  vterm_parser_سیٹ_callbacks(VTerm *vt, ثابت VTermParserCallbacks *callbacks, خالی *user);
خالی *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
خالی vterm_parser_سیٹ_emit_nul(VTerm *vt, صحیح emit);

// -----------
// State layer
// -----------

قسمتعریف ساخت {
  صحیح (*putglyph)(VTermGlyphInfo *info, VTermPos pos, خالی *user);
  صحیح (*movecursor)(VTermPos pos, VTermPos oldpos, صحیح visible, خالی *user);
  صحیح (*scrollrect)(VTermRect rect, صحیح downward, صحیح rightward, خالی *user);
  صحیح (*moverect)(VTermRect dest, VTermRect src, خالی *user);
  صحیح (*erase)(VTermRect rect, صحیح selective, خالی *user);
  صحیح (*initpen)(خالی *user);
  صحیح (*setpenattr)(VTermAttr attr, VTerm_قدر *val, خالی *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  صحیح (*settermprop)(VTermProp prop, VTerm_قدر *val, خالی *user);
  صحیح (*bell)(خالی *user);
  صحیح (*resize)(صحیح صفیں_2, صحیح ستون_تعداد, VTermStateFields *fields, خالی *user);
  صحیح (*setlineinfo)(صحیح صف, ثابت VTerm_سطر_Info *newinfo, ثابت VTerm_سطر_Info *oldinfo, خالی *user);
  صحیح (*sb_clear)(خالی *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  صحیح (*premove)(VTermRect dest, خالی *user);
} VTermStateCallbacks;

// VIM: added
قسمتعریف ساخت {
  VTermPos pos;
  صحیح	   buttons;
#تعریف_کرو MOUSE_BUTTON_بایاں 0x01
#تعریف_کرو MOUSE_BUTTON_درمیان 0x02
#تعریف_کرو MOUSE_BUTTON_دایاں 0x04
  صحیح      flags;
#تعریف_کرو MOUSE_WANT_CLICK 0x01
#تعریف_کرو MOUSE_WANT_DRAG  0x02
#تعریف_کرو MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

قسمتعریف ساخت {
  صحیح (*control)(غیرعلامتی حرف control, خالی *user);
  صحیح (*csi)(ثابت حرف *leader, ثابت طویل دلائل[], صحیح argcount, ثابت حرف *intermed, حرف command, خالی *user);
  صحیح (*osc)(صحیح command, VTermStringFragment frag, خالی *user);
  صحیح (*dcs)(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, خالی *user);
  صحیح (*apc)(VTermStringFragment frag, خالی *user);
  صحیح (*pm)(VTermStringFragment frag, خالی *user);
  صحیح (*sos)(VTermStringFragment frag, خالی *user);
} VTermStateFallbacks;

قسمتعریف ساخت {
  صحیح (*سیٹ_2)(VTerm_Selection_نقاب نقاب, VTermStringFragment frag, خالی *user);
  صحیح (*query)(VTerm_Selection_نقاب نقاب, خالی *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

خالی  vterm_state_سیٹ_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, خالی *user);
خالی *vterm_state_get_cbdata(VTermState *state);

خالی vterm_state_callbacks_has_premove(VTermState *state);

خالی  vterm_state_سیٹ_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, خالی *user);
خالی *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
خالی vterm_state_reset(VTermState *state, صحیح hard);

خالی vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos);
// VIM: added
خالی vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate);
خالی vterm_state_get_default_colors(ثابت VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
خالی vterm_state_get_palette_color(ثابت VTermState *state, صحیح اشاریہ, VTermColor *ستون);
خالی vterm_state_سیٹ_default_colors(VTermState *state, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg);
خالی vterm_state_سیٹ_palette_color(VTermState *state, صحیح اشاریہ, ثابت VTermColor *ستون);
خالی vterm_state_سیٹ_bold_highbright(VTermState *state, صحیح bold_is_highbright);
صحیح  vterm_state_get_penattr(ثابت VTermState *state, VTermAttr attr, VTerm_قدر *val);
صحیح  vterm_state_سیٹ_termprop(VTermState *state, VTermProp prop, VTerm_قدر *val);
خالی vterm_state_focus_اندر(VTermState *state);
خالی vterm_state_focus_برآمد(VTermState *state);
ثابت VTerm_سطر_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحیح صف);

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
خالی vterm_state_convert_color_مقصد_rgb(ثابت VTermState *state, VTermColor *ستون);

خالی vterm_state_سیٹ_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, خالی *user,
    حرف *بفر, size_t buflen);

خالی vterm_state_send_selection(VTermState *state, VTerm_Selection_نقاب نقاب, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

قسمتعریف ساخت {
    غیرعلامتی صحیح bold      : 1;
    غیرعلامتی صحیح underline : 2;
    غیرعلامتی صحیح italic    : 1;
    غیرعلامتی صحیح blink     : 1;
    غیرعلامتی صحیح الٹ   : 1;
    غیرعلامتی صحیح conceal   : 1;
    غیرعلامتی صحیح strike    : 1;
    غیرعلامتی صحیح font      : 4; /* 0 to 9 */
    غیرعلامتی صحیح dwl       : 1; /* On a DECDWL or DECDHL line */
    غیرعلامتی صحیح dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    غیرعلامتی صحیح small     : 1;
    غیرعلامتی صحیح baseline  : 2;
} VTermScreenCellAttrs;

شمار {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

شمار {
  VTERM_BASELINE_معمول,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_زیریں,
};

قسمتعریف ساخت {
  uint32_t chars[VTERM_MAX_CHARS_فی_CELL];
  حرف     چوڑائی;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
قسمتعریف ساخت {
  صحیح (*damage)(VTermRect rect, خالی *user);
  صحیح (*moverect)(VTermRect dest, VTermRect src, خالی *user);
  صحیح (*movecursor)(VTermPos pos, VTermPos oldpos, صحیح visible, خالی *user);
  صحیح (*settermprop)(VTermProp prop, VTerm_قدر *val, خالی *user);
  صحیح (*bell)(خالی *user);
  صحیح (*resize)(صحیح صفیں_2, صحیح ستون_تعداد, خالی *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  صحیح (*sb_pushline)(صحیح ستون_تعداد, ثابت VTermScreenCell *cells, خالی *user);
  صحیح (*sb_popline)(صحیح ستون_تعداد, VTermScreenCell *cells, خالی *user);
  صحیح (*sb_clear)(خالی* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  صحیح (*sb_pushline4)(صحیح ستون_تعداد, ثابت VTermScreenCell *cells, صحیح continuation, خالی *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
خالی  vterm_screen_سیٹ_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, خالی *user);
خالی *vterm_screen_get_cbdata(VTermScreen *screen);

خالی vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

خالی  vterm_screen_سیٹ_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, خالی *user);
خالی *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

خالی vterm_screen_enable_reflow(VTermScreen *screen, صحیح reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#تعریف_کرو vterm_screen_سیٹ_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
خالی vterm_screen_enable_altscreen(VTermScreen *screen, صحیح altscreen);

قسمتعریف شمار {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_صف,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTerm_Damage_سائز;

// Invoke the relevant callbacks to update the screen.
خالی vterm_screen_flush_damage(VTermScreen *screen);

خالی vterm_screen_سیٹ_damage_ملاؤ(VTermScreen *screen, VTerm_Damage_سائز سائز);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
خالی   vterm_screen_reset(VTermScreen *screen, صحیح hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect);
size_t vterm_screen_get_متن(ثابت VTermScreen *screen, حرف *str, size_t len, ثابت VTermRect rect);

قسمتعریف شمار {
  VTERM_ATTR_BOLD_نقاب       = 1 << 0,
  VTERM_ATTR_UNDERLINE_نقاب  = 1 << 1,
  VTERM_ATTR_ITALIC_نقاب     = 1 << 2,
  VTERM_ATTR_BLINK_نقاب      = 1 << 3,
  VTERM_ATTR_الٹ_نقاب    = 1 << 4,
  VTERM_ATTR_STRIKE_نقاب     = 1 << 5,
  VTERM_ATTR_FONT_نقاب       = 1 << 6,
  VTERM_ATTR_FOREGROUND_نقاب = 1 << 7,
  VTERM_ATTR_BACKGROUND_نقاب = 1 << 8,
  VTERM_ATTR_CONCEAL_نقاب    = 1 << 9,
  VTERM_ATTR_SMALL_نقاب      = 1 << 10,
  VTERM_ATTR_BASELINE_نقاب   = 1 << 11,

  VTERM_ALL_ATTRS_نقاب = (1 << 12) - 1
} VTerm_Attr_نقاب;

صحیح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_نقاب attrs);

صحیح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

صحیح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
خالی vterm_screen_convert_color_مقصد_rgb(ثابت VTermScreen *screen, VTermColor *ستون);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
خالی vterm_screen_سیٹ_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_قدر_Type vterm_get_attr_type(VTermAttr attr);
VTerm_قدر_Type vterm_get_prop_type(VTermProp prop);

خالی vterm_scroll_rect(VTermRect rect,
                       صحیح downward,
                       صحیح rightward,
                       صحیح (*moverect)(VTermRect src, VTermRect dest, خالی *user),
                       صحیح (*eraserect)(VTermRect rect, صحیح selective, خالی *user),
                       خالی *user);

خالی vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      خالی (*copycell)(VTermPos dest, VTermPos src, خالی *user),
                      خالی *user);

#اگر_تعریف_شدہ __cplusplus
}
#اگر_ختم

#اگر_ختم
