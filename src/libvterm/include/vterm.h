/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#اگر_تعریف_نشده __VTERM_H__
#تعریف_کن __VTERM_H__

#اگر_تعریف_شده __cplusplus
خارجي "C" {
#پایان_اگر

#ضمن <stdlib.h>

#ضمن "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#تعریف_کن TRUE 1
#تعریف_کن FALSE 0

// VIM: from stdint.h
تعريفنوع بیعلامت حرف		uint8_t;
تعريفنوع بیعلامت کوتاه		uint16_t;
تعريفنوع بیعلامت صحيح		uint32_t;

// VIM: define max screen cols and rows
#تعریف_کن VTERM_MAX_تعداد_ستون 1000
#تعریف_کن VTERM_MAX_ردیفها 1000

#تعریف_کن VTERM_نسخه_MAJOR 0
#تعریف_کن VTERM_نسخه_MINOR 3
#تعریف_کن VTERM_نسخه_PATCH 3

#تعریف_کن VTERM_CHECK_نسخه \
        vterm_check_نسخه(VTERM_نسخه_MAJOR, VTERM_نسخه_MINOR)

/* Any cell can contain at most one basic printing character and 5 combining
 * characters. This number could be changed but will be ABI-incompatible if
 * you do */
#تعریف_کن VTERM_MAX_CHARS_در_هر_CELL 6

تعريفنوع بنية VTerm VTerm;
تعريفنوع بنية VTermState VTermState;
تعريفنوع بنية VTermScreen VTermScreen;

// Specifies a screen point.
تعريفنوع بنية {
  صحيح ردیف;
  صحيح ستون;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
صحيح vterm_pos_cmp(VTermPos a, VTermPos b);

#إذا defined(DEFINE_INLINES) || USE_INLINE
INLINE صحيح vterm_pos_cmp(VTermPos a, VTermPos b)
{
  أرجع (a.ردیف == b.ردیف) ? a.ستون - b.ستون : a.ردیف - b.ردیف;
}
#پایان_اگر

// Specifies a rectangular screen area.
تعريفنوع بنية {
  صحيح شروع_ردیف;
  صحيح انتها_ردیف;
  صحيح شروع_ستون;
  صحيح انتها_ستون;
} VTermRect;

/* true if the rect contains the point */
صحيح vterm_rect_شامل_است(VTermRect r, VTermPos p);

#إذا defined(DEFINE_INLINES) || USE_INLINE
INLINE صحيح vterm_rect_شامل_است(VTermRect r, VTermPos p)
{
  أرجع p.ردیف >= r.شروع_ردیف && p.ردیف < r.انتها_ردیف &&
         p.ستون >= r.شروع_ستون && p.ستون < r.انتها_ستون;
}
#پایان_اگر

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
فراغ vterm_rect_move(VTermRect *rect, صحيح ردیف_delta, صحيح ستون_delta);

#إذا defined(DEFINE_INLINES) || USE_INLINE
INLINE فراغ vterm_rect_move(VTermRect *rect, صحيح ردیف_delta, صحيح ستون_delta)
{
  rect->شروع_ردیف += ردیف_delta; rect->انتها_ردیف += ردیف_delta;
  rect->شروع_ستون += ستون_delta; rect->انتها_ستون += ستون_delta;
}
#پایان_اگر

/**
 * Bit-field describing the value of VTermColor.type
 */
تعريفنوع تعداد {
  /**
   * If the lower bit of `type` is not set, the colour is 24-bit RGB.
   */
  VTERM_COLOR_RGB = 0x00,

  /**
   * The colour is an index into a palette of 256 colours.
   */
  VTERM_COLOR_نمایهای = 0x01,

  /**
   * Mask that can be used to extract the RGB/Indexed bit.
   */
  VTERM_COLOR_TYPE_ماسک = 0x01,

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
  VTERM_COLOR_DEFAULT_ماسک = 0x06,

  /**
   * VIM: If set, indicates that the color is invalid.
   */
  VTERM_COLOR_INVALID = 0x08
} VTermColorType;

/**
 * Returns true if the VTERM_COLOR_RGB `type` flag is set, indicating that the
 * given VTermColor instance is an indexed colour.
 */
#تعریف_کن VTERM_COLOR_IS_نمایهای(ستون) \
  (((ستون)->type & VTERM_COLOR_TYPE_ماسک) == VTERM_COLOR_نمایهای)

/**
 * Returns true if the VTERM_COLOR_INDEXED `type` flag is set, indicating that
 * the given VTermColor instance is an rgb colour.
 */
#تعریف_کن VTERM_COLOR_IS_RGB(ستون) \
  (((ستون)->type & VTERM_COLOR_TYPE_ماسک) == VTERM_COLOR_RGB)

/**
 * Returns true if the VTERM_COLOR_DEFAULT_FG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default foreground
 * color.
 */
#تعریف_کن VTERM_COLOR_IS_DEFAULT_FG(ستون) \
  (!!((ستون)->type & VTERM_COLOR_DEFAULT_FG))

/**
 * Returns true if the VTERM_COLOR_DEFAULT_BG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default background
 * color.
 */
#تعریف_کن VTERM_COLOR_IS_DEFAULT_BG(ستون) \
  (!!((ستون)->type & VTERM_COLOR_DEFAULT_BG))

/**
 * Returns true if the VTERM_COLOR_INVALID `type` flag is set, indicating
 * that the given VTermColor instance is an invalid color.
 */
#تعریف_کن VTERM_COLOR_IS_INVALID(ستون) (!!((ستون)->type & VTERM_COLOR_INVALID))

// VIM: this was a union, but that doesn't always work.
تعريفنوع بنية {
  /**
   * Tag indicating which member is actually valid.
   * Please use the `VTERM_COLOR_IS_*` test macros to check whether a
   * particular type flag is set.
   */
  uint8_t type;

  uint8_t red, green, blue;

  uint8_t شاخص;
} VTermColor;

/**
 * Constructs a new VTermColor instance representing the given RGB values.
 */
فراغ vterm_color_rgb(VTermColor *ستون, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
فراغ vterm_color_نمایهای(VTermColor *ستون, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
صحيح vterm_color_is_equal(ثابت VTermColor *a, ثابت VTermColor *b);

تعريفنوع تعداد {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_مقدار_Type;

تعريفنوع بنية {
  ثابت حرف *str;
  size_t      len : 30;
  بیعلامت صحيح  initial : 1;
  بیعلامت صحيح  final : 1;
} VTermStringFragment;

تعريفنوع اتحاد {
  صحيح boolean;
  صحيح عدد;
  VTermStringFragment string;
  VTermColor color;
} VTerm_مقدار;

تعريفنوع تعداد {
  /* VTERM_ATTR_NONE = 0 */
  VTERM_ATTR_BOLD = 1,   // bool:   1, 22
  VTERM_ATTR_UNDERLINE,  // number: 4, 21, 24
  VTERM_ATTR_ITALIC,     // bool:   3, 23
  VTERM_ATTR_BLINK,      // bool:   5, 25
  VTERM_ATTR_معکوس,    // bool:   7, 27
  VTERM_ATTR_CONCEAL,    // bool:   8, 28
  VTERM_ATTR_STRIKE,     // bool:   9, 29
  VTERM_ATTR_FONT,       // number: 10-19
  VTERM_ATTR_FOREGROUND, // color:  30-39 90-97
  VTERM_ATTR_BACKGROUND, // color:  40-49 100-107
  VTERM_ATTR_SMALL,      // bool:   73, 74, 75
  VTERM_ATTR_BASELINE,   // number: 73, 74, 75

  VTERM_N_ATTRS
} VTermAttr;

تعريفنوع تعداد {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_معکوس,           // bool
  VTERM_PROP_CURSORSHAPE,       //عدد
  VTERM_PROP_MOUSE,             //عدد
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

تعداد {
  VTERM_PROP_CURSORSHAPE_بلوک = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_چپ,

  VTERM_N_PROP_CURSORSHAPES
};

تعداد {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

تعريفنوع تعداد {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTerm_Selection_ماسک;

تعريفنوع بنية {
  ثابت uint32_t *chars;
  صحيح             عرض;
  بیعلامت صحيح    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  بیعلامت صحيح    dwl:1;             /* DECDWL or DECDHL double-width line */
  بیعلامت صحيح    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

تعريفنوع بنية {
  بیعلامت صحيح    doublewidth:1;     /* DECDWL or DECDHL line */
  بیعلامت صحيح    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  بیعلامت صحيح    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_سطر_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
تعريفنوع بنية {
  VTermPos pos;                /* current cursor position */
  VTerm_سطر_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

تعريفنوع بنية {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  فراغ *(*حافظه_بده)(size_t اندازه, فراغ *allocdata);
  فراغ  (*حرر)(فراغ *ptr, فراغ *allocdata);
} VTermAllocatorFunctions;

فراغ vterm_check_نسخه(صحيح major, صحيح minor);

بنية VTermBuilder {
  صحيح ver; /* currently unused but reserved for some sort of ABI version flag */

  صحيح ردیفها_2, تعداد_ستون;

  ثابت VTermAllocatorFunctions *allocator;
  فراغ *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(ثابت بنية VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(صحيح ردیفها_2, صحيح تعداد_ستون);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(صحيح ردیفها_2, صحيح تعداد_ستون, VTermAllocatorFunctions *funcs, فراغ *allocdata);

// Free and cleanup a terminal and all its data.
فراغ   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
فراغ vterm_get_اندازه(ثابت VTerm *vt, صحيح *rowsp, صحيح *colsp);

فراغ vterm_مجموعه_اندازه(VTerm *vt, صحيح ردیفها_2, صحيح تعداد_ستون);

صحيح  vterm_get_utf8(ثابت VTerm *vt);
فراغ vterm_مجموعه_utf8(VTerm *vt, صحيح is_utf8);

size_t vterm_درون_داد_write(VTerm *vt, ثابت حرف *بایتها, size_t len);

/* Setting output callback will override the buffer logic */
تعريفنوع فراغ VTermOutputCallback(ثابت حرف *s, size_t len, فراغ *user);
فراغ vterm_output_مجموعه_callback(VTerm *vt, VTermOutputCallback *func, فراغ *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_میانگیر_اندازه(ثابت VTerm *vt);
size_t vterm_output_get_میانگیر_current(ثابت VTerm *vt);
size_t vterm_output_get_میانگیر_مانده(ثابت VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, حرف *میانگیر, size_t len);

صحيح vterm_is_modify_other_کلیدها(VTerm *vt);
صحيح vterm_is_kitty_keyboard(VTerm *vt);
فراغ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
فراغ vterm_keyboard_کلید(VTerm *vt, VTerm_کلید کلید, VTermModifier mod);

فراغ vterm_keyboard_شروع_paste(VTerm *vt);
فراغ vterm_keyboard_انتها_paste(VTerm *vt);

فراغ vterm_mouse_move(VTerm *vt, صحيح ردیف, صحيح ستون, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
فراغ vterm_mouse_button(VTerm *vt, صحيح button, صحيح pressed, VTermModifier mod);

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
#تعریف_کن CSI_ARG_FLAG_بیشتر (1U<<31)
#تعریف_کن CSI_ARG_ماسک      (~(1U<<31))

#تعریف_کن CSI_ARG_HAS_بیشتر(a) ((a) & CSI_ARG_FLAG_بیشتر)
#تعریف_کن CSI_ARG(a)          ((a) & CSI_ARG_ماسک)

/* Can't use -1 to indicate a missing argument; use this instead */
// VIM: changed 31 to 30 to avoid an overflow warning
#تعریف_کن CSI_ARG_MISSING ((1<<30)-1)

#تعریف_کن CSI_ARG_IS_MISSING(a) (CSI_ARG(a) == CSI_ARG_MISSING)
#تعریف_کن CSI_ARG_OR(a,def)     (CSI_ARG(a) == CSI_ARG_MISSING ? (def) : CSI_ARG(a))
#تعریف_کن CSI_ARG_شمار(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

تعريفنوع بنية {
  صحيح (*متن)(ثابت حرف *بایتها, size_t len, فراغ *user);
  صحيح (*control)(بیعلامت حرف control, فراغ *user);
  صحيح (*escape)(ثابت حرف *بایتها, size_t len, فراغ *user);
  صحيح (*csi)(ثابت حرف *leader, ثابت دراز آرگومانها[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user);
  صحيح (*osc)(صحيح command, VTermStringFragment frag, فراغ *user);
  صحيح (*dcs)(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user);
  صحيح (*apc)(VTermStringFragment frag, فراغ *user);
  صحيح (*pm)(VTermStringFragment frag, فراغ *user);
  صحيح (*sos)(VTermStringFragment frag, فراغ *user);
  صحيح (*resize)(صحيح ردیفها_2, صحيح تعداد_ستون, فراغ *user);
} VTermParserCallbacks;

فراغ  vterm_parser_مجموعه_callbacks(VTerm *vt, ثابت VTermParserCallbacks *callbacks, فراغ *user);
فراغ *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
فراغ vterm_parser_مجموعه_emit_nul(VTerm *vt, صحيح emit);

// -----------
// State layer
// -----------

تعريفنوع بنية {
  صحيح (*putglyph)(VTermGlyphInfo *info, VTermPos pos, فراغ *user);
  صحيح (*movecursor)(VTermPos pos, VTermPos oldpos, صحيح visible, فراغ *user);
  صحيح (*scrollrect)(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user);
  صحيح (*moverect)(VTermRect dest, VTermRect src, فراغ *user);
  صحيح (*erase)(VTermRect rect, صحيح selective, فراغ *user);
  صحيح (*initpen)(فراغ *user);
  صحيح (*setpenattr)(VTermAttr attr, VTerm_مقدار *val, فراغ *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  صحيح (*settermprop)(VTermProp prop, VTerm_مقدار *val, فراغ *user);
  صحيح (*bell)(فراغ *user);
  صحيح (*resize)(صحيح ردیفها_2, صحيح تعداد_ستون, VTermStateFields *fields, فراغ *user);
  صحيح (*setlineinfo)(صحيح ردیف, ثابت VTerm_سطر_Info *newinfo, ثابت VTerm_سطر_Info *oldinfo, فراغ *user);
  صحيح (*sb_clear)(فراغ *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  صحيح (*premove)(VTermRect dest, فراغ *user);
} VTermStateCallbacks;

// VIM: added
تعريفنوع بنية {
  VTermPos pos;
  صحيح	   buttons;
#تعریف_کن MOUSE_BUTTON_چپ 0x01
#تعریف_کن MOUSE_BUTTON_میانه 0x02
#تعریف_کن MOUSE_BUTTON_راست 0x04
  صحيح      flags;
#تعریف_کن MOUSE_WANT_CLICK 0x01
#تعریف_کن MOUSE_WANT_DRAG  0x02
#تعریف_کن MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

تعريفنوع بنية {
  صحيح (*control)(بیعلامت حرف control, فراغ *user);
  صحيح (*csi)(ثابت حرف *leader, ثابت دراز آرگومانها[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user);
  صحيح (*osc)(صحيح command, VTermStringFragment frag, فراغ *user);
  صحيح (*dcs)(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user);
  صحيح (*apc)(VTermStringFragment frag, فراغ *user);
  صحيح (*pm)(VTermStringFragment frag, فراغ *user);
  صحيح (*sos)(VTermStringFragment frag, فراغ *user);
} VTermStateFallbacks;

تعريفنوع بنية {
  صحيح (*مجموعه_2)(VTerm_Selection_ماسک ماسک, VTermStringFragment frag, فراغ *user);
  صحيح (*query)(VTerm_Selection_ماسک ماسک, فراغ *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

فراغ  vterm_state_مجموعه_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, فراغ *user);
فراغ *vterm_state_get_cbdata(VTermState *state);

فراغ vterm_state_callbacks_has_premove(VTermState *state);

فراغ  vterm_state_مجموعه_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, فراغ *user);
فراغ *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
فراغ vterm_state_reset(VTermState *state, صحيح hard);

فراغ vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos);
// VIM: added
فراغ vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate);
فراغ vterm_state_get_default_colors(ثابت VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
فراغ vterm_state_get_palette_color(ثابت VTermState *state, صحيح شاخص, VTermColor *ستون);
فراغ vterm_state_مجموعه_default_colors(VTermState *state, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg);
فراغ vterm_state_مجموعه_palette_color(VTermState *state, صحيح شاخص, ثابت VTermColor *ستون);
فراغ vterm_state_مجموعه_bold_highbright(VTermState *state, صحيح bold_is_highbright);
صحيح  vterm_state_get_penattr(ثابت VTermState *state, VTermAttr attr, VTerm_مقدار *val);
صحيح  vterm_state_مجموعه_termprop(VTermState *state, VTermProp prop, VTerm_مقدار *val);
فراغ vterm_state_focus_درون(VTermState *state);
فراغ vterm_state_focus_بیرون(VTermState *state);
ثابت VTerm_سطر_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحيح ردیف);

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
فراغ vterm_state_convert_color_مقصد_rgb(ثابت VTermState *state, VTermColor *ستون);

فراغ vterm_state_مجموعه_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, فراغ *user,
    حرف *میانگیر, size_t buflen);

فراغ vterm_state_send_selection(VTermState *state, VTerm_Selection_ماسک ماسک, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

تعريفنوع بنية {
    بیعلامت صحيح bold      : 1;
    بیعلامت صحيح underline : 2;
    بیعلامت صحيح italic    : 1;
    بیعلامت صحيح blink     : 1;
    بیعلامت صحيح معکوس   : 1;
    بیعلامت صحيح conceal   : 1;
    بیعلامت صحيح strike    : 1;
    بیعلامت صحيح font      : 4; /* 0 to 9 */
    بیعلامت صحيح dwl       : 1; /* On a DECDWL or DECDHL line */
    بیعلامت صحيح dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    بیعلامت صحيح small     : 1;
    بیعلامت صحيح baseline  : 2;
} VTermScreenCellAttrs;

تعداد {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

تعداد {
  VTERM_BASELINE_نرمال,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_پایینی,
};

تعريفنوع بنية {
  uint32_t chars[VTERM_MAX_CHARS_در_هر_CELL];
  حرف     عرض;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
تعريفنوع بنية {
  صحيح (*damage)(VTermRect rect, فراغ *user);
  صحيح (*moverect)(VTermRect dest, VTermRect src, فراغ *user);
  صحيح (*movecursor)(VTermPos pos, VTermPos oldpos, صحيح visible, فراغ *user);
  صحيح (*settermprop)(VTermProp prop, VTerm_مقدار *val, فراغ *user);
  صحيح (*bell)(فراغ *user);
  صحيح (*resize)(صحيح ردیفها_2, صحيح تعداد_ستون, فراغ *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  صحيح (*sb_pushline)(صحيح تعداد_ستون, ثابت VTermScreenCell *cells, فراغ *user);
  صحيح (*sb_popline)(صحيح تعداد_ستون, VTermScreenCell *cells, فراغ *user);
  صحيح (*sb_clear)(فراغ* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  صحيح (*sb_pushline4)(صحيح تعداد_ستون, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
فراغ  vterm_screen_مجموعه_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, فراغ *user);
فراغ *vterm_screen_get_cbdata(VTermScreen *screen);

فراغ vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

فراغ  vterm_screen_مجموعه_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, فراغ *user);
فراغ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

فراغ vterm_screen_enable_reflow(VTermScreen *screen, صحيح reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#تعریف_کن vterm_screen_مجموعه_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
فراغ vterm_screen_enable_altscreen(VTermScreen *screen, صحيح altscreen);

تعريفنوع تعداد {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_ردیف,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTerm_Damage_اندازه;

// Invoke the relevant callbacks to update the screen.
فراغ vterm_screen_flush_damage(VTermScreen *screen);

فراغ vterm_screen_مجموعه_damage_ادغام(VTermScreen *screen, VTerm_Damage_اندازه اندازه);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
فراغ   vterm_screen_reset(VTermScreen *screen, صحيح hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect);
size_t vterm_screen_get_متن(ثابت VTermScreen *screen, حرف *str, size_t len, ثابت VTermRect rect);

تعريفنوع تعداد {
  VTERM_ATTR_BOLD_ماسک       = 1 << 0,
  VTERM_ATTR_UNDERLINE_ماسک  = 1 << 1,
  VTERM_ATTR_ITALIC_ماسک     = 1 << 2,
  VTERM_ATTR_BLINK_ماسک      = 1 << 3,
  VTERM_ATTR_معکوس_ماسک    = 1 << 4,
  VTERM_ATTR_STRIKE_ماسک     = 1 << 5,
  VTERM_ATTR_FONT_ماسک       = 1 << 6,
  VTERM_ATTR_FOREGROUND_ماسک = 1 << 7,
  VTERM_ATTR_BACKGROUND_ماسک = 1 << 8,
  VTERM_ATTR_CONCEAL_ماسک    = 1 << 9,
  VTERM_ATTR_SMALL_ماسک      = 1 << 10,
  VTERM_ATTR_BASELINE_ماسک   = 1 << 11,

  VTERM_ALL_ATTRS_ماسک = (1 << 12) - 1
} VTerm_Attr_ماسک;

صحيح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ماسک attrs);

صحيح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

صحيح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
فراغ vterm_screen_convert_color_مقصد_rgb(ثابت VTermScreen *screen, VTermColor *ستون);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
فراغ vterm_screen_مجموعه_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_مقدار_Type vterm_get_attr_type(VTermAttr attr);
VTerm_مقدار_Type vterm_get_prop_type(VTermProp prop);

فراغ vterm_scroll_rect(VTermRect rect,
                       صحيح downward,
                       صحيح rightward,
                       صحيح (*moverect)(VTermRect src, VTermRect dest, فراغ *user),
                       صحيح (*eraserect)(VTermRect rect, صحيح selective, فراغ *user),
                       فراغ *user);

فراغ vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      فراغ (*copycell)(VTermPos dest, VTermPos src, فراغ *user),
                      فراغ *user);

#اگر_تعریف_شده __cplusplus
}
#پایان_اگر

#پایان_اگر
