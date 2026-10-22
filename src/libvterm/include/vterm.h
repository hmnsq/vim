/*
 * NOTE: This is a MODIFIED version of libvterm, see the README file.
 */
#నిర్వచితం_కాకపోతే __VTERM_H__
#నిర్వచించు __VTERM_H__

#నిర్వచితమైతే __cplusplus
బాహ్య "C" {
#అయితే_ముగింపు

#అన్తర్భావ <stdlib.h>

#అన్తర్భావ "vterm_keycodes.h"

// VIM: use TRUE and FALSE instead of true and false
#నిర్వచించు TRUE 1
#నిర్వచించు FALSE 0

// VIM: from stdint.h
రకనిర్వచనం గుర్తులేని అక్షరం		uint8_t;
రకనిర్వచనం గుర్తులేని పొట్టి		uint16_t;
రకనిర్వచనం గుర్తులేని పూర్ణాంకం		uint32_t;

// VIM: define max screen cols and rows
#నిర్వచించు VTERM_MAX_నిలువుల_సంఖ్య 1000
#నిర్వచించు VTERM_MAX_అడ్డువరుసలు 1000

#నిర్వచించు VTERM_సంస్కరణ_MAJOR 0
#నిర్వచించు VTERM_సంస్కరణ_MINOR 3
#నిర్వచించు VTERM_సంస్కరణ_PATCH 3

#నిర్వచించు VTERM_CHECK_సంస్కరణ \
        vterm_check_సంస్కరణ(VTERM_సంస్కరణ_MAJOR, VTERM_సంస్కరణ_MINOR)

/* Any cell can contain at most one basic printing character and 5 combining
 * characters. This number could be changed but will be ABI-incompatible if
 * you do */
#నిర్వచించు VTERM_MAX_CHARS_చొప్పున_CELL 6

రకనిర్వచనం నిర్మాణం VTerm VTerm;
రకనిర్వచనం నిర్మాణం VTermState VTermState;
రకనిర్వచనం నిర్మాణం VTermScreen VTermScreen;

// Specifies a screen point.
రకనిర్వచనం నిర్మాణం {
  పూర్ణాంకం అడ్డువరుస;
  పూర్ణాంకం నిలువు;
} VTermPos;

/* some small utility functions; we can just keep these static here */

/*
 * Order points by on-screen flow order:
 * Return < 0 if "a" is before "b"
 * Return  0  if "a" and "b" are equal
 * Return > 0 if "a" is after "b".
 */
పూర్ణాంకం vterm_pos_cmp(VTermPos a, VTermPos b);

#ఒకవేళ defined(DEFINE_INLINES) || USE_INLINE
INLINE పూర్ణాంకం vterm_pos_cmp(VTermPos a, VTermPos b)
{
  తిరిగి (a.అడ్డువరుస == b.అడ్డువరుస) ? a.నిలువు - b.నిలువు : a.అడ్డువరుస - b.అడ్డువరుస;
}
#అయితే_ముగింపు

// Specifies a rectangular screen area.
రకనిర్వచనం నిర్మాణం {
  పూర్ణాంకం ప్రారంభం_అడ్డువరుస;
  పూర్ణాంకం చివర_అడ్డువరుస;
  పూర్ణాంకం ప్రారంభం_నిలువు;
  పూర్ణాంకం చివర_నిలువు;
} VTermRect;

/* true if the rect contains the point */
పూర్ణాంకం vterm_rect_కలిగియుంది(VTermRect r, VTermPos p);

#ఒకవేళ defined(DEFINE_INLINES) || USE_INLINE
INLINE పూర్ణాంకం vterm_rect_కలిగియుంది(VTermRect r, VTermPos p)
{
  తిరిగి p.అడ్డువరుస >= r.ప్రారంభం_అడ్డువరుస && p.అడ్డువరుస < r.చివర_అడ్డువరుస &&
         p.నిలువు >= r.ప్రారంభం_నిలువు && p.నిలువు < r.చివర_నిలువు;
}
#అయితే_ముగింపు

/* move a rect */
// Move "rect" "row_delta" down and "col_delta" right.
// Does not check boundaries.
శూన్య vterm_rect_move(VTermRect *rect, పూర్ణాంకం అడ్డువరుస_delta, పూర్ణాంకం నిలువు_delta);

#ఒకవేళ defined(DEFINE_INLINES) || USE_INLINE
INLINE శూన్య vterm_rect_move(VTermRect *rect, పూర్ణాంకం అడ్డువరుస_delta, పూర్ణాంకం నిలువు_delta)
{
  rect->ప్రారంభం_అడ్డువరుస += అడ్డువరుస_delta; rect->చివర_అడ్డువరుస += అడ్డువరుస_delta;
  rect->ప్రారంభం_నిలువు += నిలువు_delta; rect->చివర_నిలువు += నిలువు_delta;
}
#అయితే_ముగింపు

/**
 * Bit-field describing the value of VTermColor.type
 */
రకనిర్వచనం గణన {
  /**
   * If the lower bit of `type` is not set, the colour is 24-bit RGB.
   */
  VTERM_COLOR_RGB = 0x00,

  /**
   * The colour is an index into a palette of 256 colours.
   */
  VTERM_COLOR_సూచీకృత = 0x01,

  /**
   * Mask that can be used to extract the RGB/Indexed bit.
   */
  VTERM_COLOR_TYPE_ముసుగు = 0x01,

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
  VTERM_COLOR_DEFAULT_ముసుగు = 0x06,

  /**
   * VIM: If set, indicates that the color is invalid.
   */
  VTERM_COLOR_INVALID = 0x08
} VTermColorType;

/**
 * Returns true if the VTERM_COLOR_RGB `type` flag is set, indicating that the
 * given VTermColor instance is an indexed colour.
 */
#నిర్వచించు VTERM_COLOR_IS_సూచీకృత(నిలువు) \
  (((నిలువు)->type & VTERM_COLOR_TYPE_ముసుగు) == VTERM_COLOR_సూచీకృత)

/**
 * Returns true if the VTERM_COLOR_INDEXED `type` flag is set, indicating that
 * the given VTermColor instance is an rgb colour.
 */
#నిర్వచించు VTERM_COLOR_IS_RGB(నిలువు) \
  (((నిలువు)->type & VTERM_COLOR_TYPE_ముసుగు) == VTERM_COLOR_RGB)

/**
 * Returns true if the VTERM_COLOR_DEFAULT_FG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default foreground
 * color.
 */
#నిర్వచించు VTERM_COLOR_IS_DEFAULT_FG(నిలువు) \
  (!!((నిలువు)->type & VTERM_COLOR_DEFAULT_FG))

/**
 * Returns true if the VTERM_COLOR_DEFAULT_BG `type` flag is set, indicating
 * that the given VTermColor instance corresponds to the default background
 * color.
 */
#నిర్వచించు VTERM_COLOR_IS_DEFAULT_BG(నిలువు) \
  (!!((నిలువు)->type & VTERM_COLOR_DEFAULT_BG))

/**
 * Returns true if the VTERM_COLOR_INVALID `type` flag is set, indicating
 * that the given VTermColor instance is an invalid color.
 */
#నిర్వచించు VTERM_COLOR_IS_INVALID(నిలువు) (!!((నిలువు)->type & VTERM_COLOR_INVALID))

// VIM: this was a union, but that doesn't always work.
రకనిర్వచనం నిర్మాణం {
  /**
   * Tag indicating which member is actually valid.
   * Please use the `VTERM_COLOR_IS_*` test macros to check whether a
   * particular type flag is set.
   */
  uint8_t type;

  uint8_t red, green, blue;

  uint8_t సూచీ;
} VTermColor;

/**
 * Constructs a new VTermColor instance representing the given RGB values.
 */
శూన్య vterm_color_rgb(VTermColor *నిలువు, uint8_t red, uint8_t green, uint8_t blue);

/**
 * Construct a new VTermColor instance representing an indexed color with the
 * given index.
 */
శూన్య vterm_color_సూచీకృత(VTermColor *నిలువు, uint8_t idx);

/**
 * Compares two colours. Returns true if the colors are equal, false otherwise.
 */
పూర్ణాంకం vterm_color_is_equal(స్థిరరాశి VTermColor *a, స్థిరరాశి VTermColor *b);

రకనిర్వచనం గణన {
  /* VTERM_VALUETYPE_NONE = 0 */
  VTERM_VALUETYPE_BOOL = 1,
  VTERM_VALUETYPE_INT,
  VTERM_VALUETYPE_STRING,
  VTERM_VALUETYPE_COLOR,

  VTERM_N_VALUETYPES
} VTerm_విలువ_Type;

రకనిర్వచనం నిర్మాణం {
  స్థిరరాశి అక్షరం *str;
  size_t      len : 30;
  గుర్తులేని పూర్ణాంకం  initial : 1;
  గుర్తులేని పూర్ణాంకం  final : 1;
} VTermStringFragment;

రకనిర్వచనం సంఘం {
  పూర్ణాంకం boolean;
  పూర్ణాంకం సంఖ్య;
  VTermStringFragment string;
  VTermColor color;
} VTerm_విలువ;

రకనిర్వచనం గణన {
  /* VTERM_ATTR_NONE = 0 */
  VTERM_ATTR_BOLD = 1,   // bool:   1, 22
  VTERM_ATTR_UNDERLINE,  // number: 4, 21, 24
  VTERM_ATTR_ITALIC,     // bool:   3, 23
  VTERM_ATTR_BLINK,      // bool:   5, 25
  VTERM_ATTR_తిరగేయి,    // bool:   7, 27
  VTERM_ATTR_CONCEAL,    // bool:   8, 28
  VTERM_ATTR_STRIKE,     // bool:   9, 29
  VTERM_ATTR_FONT,       // number: 10-19
  VTERM_ATTR_FOREGROUND, // color:  30-39 90-97
  VTERM_ATTR_BACKGROUND, // color:  40-49 100-107
  VTERM_ATTR_SMALL,      // bool:   73, 74, 75
  VTERM_ATTR_BASELINE,   // number: 73, 74, 75

  VTERM_N_ATTRS
} VTermAttr;

రకనిర్వచనం గణన {
  /* VTERM_PROP_NONE = 0 */
  VTERM_PROP_CURSORVISIBLE = 1, // bool
  VTERM_PROP_CURSORBLINK,       // bool
  VTERM_PROP_ALTSCREEN,         // bool
  VTERM_PROP_TITLE,             // string
  VTERM_PROP_ICONNAME,          // string
  VTERM_PROP_తిరగేయి,           // bool
  VTERM_PROP_CURSORSHAPE,       //సంఖ్య
  VTERM_PROP_MOUSE,             //సంఖ్య
  VTERM_PROP_FOCUSREPORT,       // bool
  VTERM_PROP_CURSORCOLOR,       // VIM - string

  VTERM_N_PROPS
} VTermProp;

గణన {
  VTERM_PROP_CURSORSHAPE_ఖండం = 1,
  VTERM_PROP_CURSORSHAPE_UNDERLINE,
  VTERM_PROP_CURSORSHAPE_BAR_ఎడమ,

  VTERM_N_PROP_CURSORSHAPES
};

గణన {
  VTERM_PROP_MOUSE_NONE = 0,
  VTERM_PROP_MOUSE_CLICK,
  VTERM_PROP_MOUSE_DRAG,
  VTERM_PROP_MOUSE_MOVE,

  VTERM_N_PROP_MOUSES
};

రకనిర్వచనం గణన {
  VTERM_SELECTION_CLIPBOARD = (1<<0),
  VTERM_SELECTION_PRIMARY   = (1<<1),
  VTERM_SELECTION_SECONDARY = (1<<2),
  VTERM_SELECTION_SELECT    = (1<<3),
  VTERM_SELECTION_CUT0      = (1<<4), /* also CUT1 .. CUT7 by bitshifting */
} VTerm_Selection_ముసుగు;

రకనిర్వచనం నిర్మాణం {
  స్థిరరాశి uint32_t *chars;
  పూర్ణాంకం             వెడల్పు;
  గుర్తులేని పూర్ణాంకం    protected_cell:1;  /* DECSCA-protected against DECSEL/DECSED */
  గుర్తులేని పూర్ణాంకం    dwl:1;             /* DECDWL or DECDHL double-width line */
  గుర్తులేని పూర్ణాంకం    dhl:2;             /* DECDHL double-height line (1=top 2=bottom) */
} VTermGlyphInfo;

రకనిర్వచనం నిర్మాణం {
  గుర్తులేని పూర్ణాంకం    doublewidth:1;     /* DECDWL or DECDHL line */
  గుర్తులేని పూర్ణాంకం    doubleheight:2;    /* DECDHL line (1=top 2=bottom) */
  గుర్తులేని పూర్ణాంకం    continuation:1;    /* Line is a flow continuation of the previous */
} VTerm_పంక్తి_Info;

/* Copies of VTermState fields that the 'resize' callback might have reason to
 * edit. 'resize' callback gets total control of these fields and may
 * free-and-reallocate them if required. They will be copied back from the
 * struct after the callback has returned.
 */
రకనిర్వచనం నిర్మాణం {
  VTermPos pos;                /* current cursor position */
  VTerm_పంక్తి_Info *lineinfos[2]; /* [1] may be NULL */
} VTermStateFields;

రకనిర్వచనం నిర్మాణం {
  /* libvterm relies on this memory to be zeroed out before it is returned
   * by the allocator. */
  శూన్య *(*జ్ఞాపకం_ఇవ్వు)(size_t కొలత, శూన్య *allocdata);
  శూన్య  (*విడుదల)(శూన్య *ptr, శూన్య *allocdata);
} VTermAllocatorFunctions;

శూన్య vterm_check_సంస్కరణ(పూర్ణాంకం major, పూర్ణాంకం minor);

నిర్మాణం VTermBuilder {
  పూర్ణాంకం ver; /* currently unused but reserved for some sort of ABI version flag */

  పూర్ణాంకం అడ్డువరుసలు_2, నిలువుల_సంఖ్య;

  స్థిరరాశి VTermAllocatorFunctions *allocator;
  శూన్య *allocdata;

  /* Override default sizes for various structures */
  size_t outbuffer_len;  /* default: 4096 */
  size_t tmpbuffer_len;  /* default: 4096 */
};

VTerm *vterm_build(స్థిరరాశి నిర్మాణం VTermBuilder *builder);

/* A convenient shortcut for default cases */
// Allocate and initialize a new terminal with default allocators.
VTerm *vterm_new(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య);
/* These shortcuts are generally discouraged in favour of just using vterm_build() */

// Allocate and initialize a new terminal with specified allocators.
VTerm *vterm_new_with_allocator(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, VTermAllocatorFunctions *funcs, శూన్య *allocdata);

// Free and cleanup a terminal and all its data.
శూన్య   vterm_free(VTerm* vt);

// Get the current size of the terminal and store in "rowsp" and "colsp".
శూన్య vterm_get_కొలత(స్థిరరాశి VTerm *vt, పూర్ణాంకం *rowsp, పూర్ణాంకం *colsp);

శూన్య vterm_సమితి_కొలత(VTerm *vt, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య);

పూర్ణాంకం  vterm_get_utf8(స్థిరరాశి VTerm *vt);
శూన్య vterm_సమితి_utf8(VTerm *vt, పూర్ణాంకం is_utf8);

size_t vterm_ఆదానం_write(VTerm *vt, స్థిరరాశి అక్షరం *బైట్లు, size_t len);

/* Setting output callback will override the buffer logic */
రకనిర్వచనం శూన్య VTermOutputCallback(స్థిరరాశి అక్షరం *s, size_t len, శూన్య *user);
శూన్య vterm_output_సమితి_callback(VTerm *vt, VTermOutputCallback *func, శూన్య *user);

/* These buffer functions only work if output callback is NOT set
 * These are deprecated and will be removed in a later version */
size_t vterm_output_get_బఫరు_కొలత(స్థిరరాశి VTerm *vt);
size_t vterm_output_get_బఫరు_current(స్థిరరాశి VTerm *vt);
size_t vterm_output_get_బఫరు_మిగిలిన(స్థిరరాశి VTerm *vt);

/* This too */
size_t vterm_output_read(VTerm *vt, అక్షరం *బఫరు, size_t len);

పూర్ణాంకం vterm_is_modify_other_కీలకాలు(VTerm *vt);
పూర్ణాంకం vterm_is_kitty_keyboard(VTerm *vt);
శూన్య vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod);
శూన్య vterm_keyboard_కీలకం(VTerm *vt, VTerm_కీలకం కీలకం, VTermModifier mod);

శూన్య vterm_keyboard_ప్రారంభం_paste(VTerm *vt);
శూన్య vterm_keyboard_చివర_paste(VTerm *vt);

శూన్య vterm_mouse_move(VTerm *vt, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, VTermModifier mod);
// "button" is 1 for left, 2 for middle, 3 for right.
// Button 4 is scroll wheel down, button 5 is scroll wheel up.
శూన్య vterm_mouse_button(VTerm *vt, పూర్ణాంకం button, పూర్ణాంకం pressed, VTermModifier mod);

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
#నిర్వచించు CSI_ARG_FLAG_మరిన్ని (1U<<31)
#నిర్వచించు CSI_ARG_ముసుగు      (~(1U<<31))

#నిర్వచించు CSI_ARG_HAS_మరిన్ని(a) ((a) & CSI_ARG_FLAG_మరిన్ని)
#నిర్వచించు CSI_ARG(a)          ((a) & CSI_ARG_ముసుగు)

/* Can't use -1 to indicate a missing argument; use this instead */
// VIM: changed 31 to 30 to avoid an overflow warning
#నిర్వచించు CSI_ARG_MISSING ((1<<30)-1)

#నిర్వచించు CSI_ARG_IS_MISSING(a) (CSI_ARG(a) == CSI_ARG_MISSING)
#నిర్వచించు CSI_ARG_OR(a,def)     (CSI_ARG(a) == CSI_ARG_MISSING ? (def) : CSI_ARG(a))
#నిర్వచించు CSI_ARG_లెక్క(a)      (CSI_ARG(a) == CSI_ARG_MISSING || CSI_ARG(a) == 0 ? 1 : CSI_ARG(a))

రకనిర్వచనం నిర్మాణం {
  పూర్ణాంకం (*పాఠ్యం)(స్థిరరాశి అక్షరం *బైట్లు, size_t len, శూన్య *user);
  పూర్ణాంకం (*control)(గుర్తులేని అక్షరం control, శూన్య *user);
  పూర్ణాంకం (*escape)(స్థిరరాశి అక్షరం *బైట్లు, size_t len, శూన్య *user);
  పూర్ణాంకం (*csi)(స్థిరరాశి అక్షరం *leader, స్థిరరాశి దీర్ఘ వాదనలు[], పూర్ణాంకం argcount, స్థిరరాశి అక్షరం *intermed, అక్షరం command, శూన్య *user);
  పూర్ణాంకం (*osc)(పూర్ణాంకం command, VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*dcs)(స్థిరరాశి అక్షరం *command, size_t commandlen, VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*apc)(VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*pm)(VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*sos)(VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*resize)(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, శూన్య *user);
} VTermParserCallbacks;

శూన్య  vterm_parser_సమితి_callbacks(VTerm *vt, స్థిరరాశి VTermParserCallbacks *callbacks, శూన్య *user);
శూన్య *vterm_parser_get_cbdata(VTerm *vt);

/* Normally NUL, CAN, SUB and DEL are ignored. Setting this true causes them
 * to be emitted by the 'control' callback
 */
శూన్య vterm_parser_సమితి_emit_nul(VTerm *vt, పూర్ణాంకం emit);

// -----------
// State layer
// -----------

రకనిర్వచనం నిర్మాణం {
  పూర్ణాంకం (*putglyph)(VTermGlyphInfo *info, VTermPos pos, శూన్య *user);
  పూర్ణాంకం (*movecursor)(VTermPos pos, VTermPos oldpos, పూర్ణాంకం visible, శూన్య *user);
  పూర్ణాంకం (*scrollrect)(VTermRect rect, పూర్ణాంకం downward, పూర్ణాంకం rightward, శూన్య *user);
  పూర్ణాంకం (*moverect)(VTermRect dest, VTermRect src, శూన్య *user);
  పూర్ణాంకం (*erase)(VTermRect rect, పూర్ణాంకం selective, శూన్య *user);
  పూర్ణాంకం (*initpen)(శూన్య *user);
  పూర్ణాంకం (*setpenattr)(VTermAttr attr, VTerm_విలువ *val, శూన్య *user);
  // Callback for setting various properties.  Must return 1 if the property
  // was accepted, 0 otherwise.
  పూర్ణాంకం (*settermprop)(VTermProp prop, VTerm_విలువ *val, శూన్య *user);
  పూర్ణాంకం (*bell)(శూన్య *user);
  పూర్ణాంకం (*resize)(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, VTermStateFields *fields, శూన్య *user);
  పూర్ణాంకం (*setlineinfo)(పూర్ణాంకం అడ్డువరుస, స్థిరరాశి VTerm_పంక్తి_Info *newinfo, స్థిరరాశి VTerm_పంక్తి_Info *oldinfo, శూన్య *user);
  పూర్ణాంకం (*sb_clear)(శూన్య *user);
  // ABI-compat only enabled if vterm_state_callbacks_has_premove() is invoked
  పూర్ణాంకం (*premove)(VTermRect dest, శూన్య *user);
} VTermStateCallbacks;

// VIM: added
రకనిర్వచనం నిర్మాణం {
  VTermPos pos;
  పూర్ణాంకం	   buttons;
#నిర్వచించు MOUSE_BUTTON_ఎడమ 0x01
#నిర్వచించు MOUSE_BUTTON_మధ్య 0x02
#నిర్వచించు MOUSE_BUTTON_కుడి 0x04
  పూర్ణాంకం      flags;
#నిర్వచించు MOUSE_WANT_CLICK 0x01
#నిర్వచించు MOUSE_WANT_DRAG  0x02
#నిర్వచించు MOUSE_WANT_MOVE  0x04
  // useful to add protocol?
} VTermMouseState;

రకనిర్వచనం నిర్మాణం {
  పూర్ణాంకం (*control)(గుర్తులేని అక్షరం control, శూన్య *user);
  పూర్ణాంకం (*csi)(స్థిరరాశి అక్షరం *leader, స్థిరరాశి దీర్ఘ వాదనలు[], పూర్ణాంకం argcount, స్థిరరాశి అక్షరం *intermed, అక్షరం command, శూన్య *user);
  పూర్ణాంకం (*osc)(పూర్ణాంకం command, VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*dcs)(స్థిరరాశి అక్షరం *command, size_t commandlen, VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*apc)(VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*pm)(VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*sos)(VTermStringFragment frag, శూన్య *user);
} VTermStateFallbacks;

రకనిర్వచనం నిర్మాణం {
  పూర్ణాంకం (*సమితి_2)(VTerm_Selection_ముసుగు ముసుగు, VTermStringFragment frag, శూన్య *user);
  పూర్ణాంకం (*query)(VTerm_Selection_ముసుగు ముసుగు, శూన్య *user);
} VTermSelectionCallbacks;

VTermState *vterm_obtain_state(VTerm *vt);

శూన్య  vterm_state_సమితి_callbacks(VTermState *state, స్థిరరాశి VTermStateCallbacks *callbacks, శూన్య *user);
శూన్య *vterm_state_get_cbdata(VTermState *state);

శూన్య vterm_state_callbacks_has_premove(VTermState *state);

శూన్య  vterm_state_సమితి_unrecognised_fallbacks(VTermState *state, స్థిరరాశి VTermStateFallbacks *fallbacks, శూన్య *user);
శూన్య *vterm_state_get_unrecognised_fbdata(VTermState *state);

// Initialize the state.
శూన్య vterm_state_reset(VTermState *state, పూర్ణాంకం hard);

శూన్య vterm_state_get_cursorpos(స్థిరరాశి VTermState *state, VTermPos *cursorpos);
// VIM: added
శూన్య vterm_state_get_mousestate(స్థిరరాశి VTermState *state, VTermMouseState *mousestate);
శూన్య vterm_state_get_default_colors(స్థిరరాశి VTermState *state, VTermColor *default_fg, VTermColor *default_bg);
శూన్య vterm_state_get_palette_color(స్థిరరాశి VTermState *state, పూర్ణాంకం సూచీ, VTermColor *నిలువు);
శూన్య vterm_state_సమితి_default_colors(VTermState *state, స్థిరరాశి VTermColor *default_fg, స్థిరరాశి VTermColor *default_bg);
శూన్య vterm_state_సమితి_palette_color(VTermState *state, పూర్ణాంకం సూచీ, స్థిరరాశి VTermColor *నిలువు);
శూన్య vterm_state_సమితి_bold_highbright(VTermState *state, పూర్ణాంకం bold_is_highbright);
పూర్ణాంకం  vterm_state_get_penattr(స్థిరరాశి VTermState *state, VTermAttr attr, VTerm_విలువ *val);
పూర్ణాంకం  vterm_state_సమితి_termprop(VTermState *state, VTermProp prop, VTerm_విలువ *val);
శూన్య vterm_state_focus_లోపల(VTermState *state);
శూన్య vterm_state_focus_బహిర్గతం(VTermState *state);
స్థిరరాశి VTerm_పంక్తి_Info *vterm_state_get_lineinfo(స్థిరరాశి VTermState *state, పూర్ణాంకం అడ్డువరుస);

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
శూన్య vterm_state_convert_color_గమ్యం_rgb(స్థిరరాశి VTermState *state, VTermColor *నిలువు);

శూన్య vterm_state_సమితి_selection_callbacks(VTermState *state, స్థిరరాశి VTermSelectionCallbacks *callbacks, శూన్య *user,
    అక్షరం *బఫరు, size_t buflen);

శూన్య vterm_state_send_selection(VTermState *state, VTerm_Selection_ముసుగు ముసుగు, VTermStringFragment frag);

// ------------
// Screen layer
// ------------

రకనిర్వచనం నిర్మాణం {
    గుర్తులేని పూర్ణాంకం bold      : 1;
    గుర్తులేని పూర్ణాంకం underline : 2;
    గుర్తులేని పూర్ణాంకం italic    : 1;
    గుర్తులేని పూర్ణాంకం blink     : 1;
    గుర్తులేని పూర్ణాంకం తిరగేయి   : 1;
    గుర్తులేని పూర్ణాంకం conceal   : 1;
    గుర్తులేని పూర్ణాంకం strike    : 1;
    గుర్తులేని పూర్ణాంకం font      : 4; /* 0 to 9 */
    గుర్తులేని పూర్ణాంకం dwl       : 1; /* On a DECDWL or DECDHL line */
    గుర్తులేని పూర్ణాంకం dhl       : 2; /* On a DECDHL line (1=top 2=bottom) */
    గుర్తులేని పూర్ణాంకం small     : 1;
    గుర్తులేని పూర్ణాంకం baseline  : 2;
} VTermScreenCellAttrs;

గణన {
  VTERM_UNDERLINE_OFF,
  VTERM_UNDERLINE_SINGLE,
  VTERM_UNDERLINE_DOUBLE,
  VTERM_UNDERLINE_CURLY,
};

గణన {
  VTERM_BASELINE_సాధారణ,
  VTERM_BASELINE_RAISE,
  VTERM_BASELINE_క్రింది,
};

రకనిర్వచనం నిర్మాణం {
  uint32_t chars[VTERM_MAX_CHARS_చొప్పున_CELL];
  అక్షరం     వెడల్పు;
  VTermScreenCellAttrs attrs;
  VTermColor fg, bg;
} VTermScreenCell;

// All fields are optional, NULL when not used.
రకనిర్వచనం నిర్మాణం {
  పూర్ణాంకం (*damage)(VTermRect rect, శూన్య *user);
  పూర్ణాంకం (*moverect)(VTermRect dest, VTermRect src, శూన్య *user);
  పూర్ణాంకం (*movecursor)(VTermPos pos, VTermPos oldpos, పూర్ణాంకం visible, శూన్య *user);
  పూర్ణాంకం (*settermprop)(VTermProp prop, VTerm_విలువ *val, శూన్య *user);
  పూర్ణాంకం (*bell)(శూన్య *user);
  పూర్ణాంకం (*resize)(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, శూన్య *user);
  // A line was pushed off the top of the window.
  // "cells[cols]" contains the cells of that line.
  // Return value is unused.
  పూర్ణాంకం (*sb_pushline)(పూర్ణాంకం నిలువుల_సంఖ్య, స్థిరరాశి VTermScreenCell *cells, శూన్య *user);
  పూర్ణాంకం (*sb_popline)(పూర్ణాంకం నిలువుల_సంఖ్య, VTermScreenCell *cells, శూన్య *user);
  పూర్ణాంకం (*sb_clear)(శూన్య* user);
  /* ABI-compat this is only used if vterm_screen_callbacks_has_pushline4() is called */
  పూర్ణాంకం (*sb_pushline4)(పూర్ణాంకం నిలువుల_సంఖ్య, స్థిరరాశి VTermScreenCell *cells, పూర్ణాంకం continuation, శూన్య *user);
} VTermScreenCallbacks;

// Return the screen of the vterm.
VTermScreen *vterm_obtain_screen(VTerm *vt);

/*
 * Install screen callbacks.  These are invoked when the screen contents is
 * changed.  "user" is passed into to the callback.
 */
శూన్య  vterm_screen_సమితి_callbacks(VTermScreen *screen, స్థిరరాశి VTermScreenCallbacks *callbacks, శూన్య *user);
శూన్య *vterm_screen_get_cbdata(VTermScreen *screen);

శూన్య vterm_screen_callbacks_has_pushline4(VTermScreen *screen);

శూన్య  vterm_screen_సమితి_unrecognised_fallbacks(VTermScreen *screen, స్థిరరాశి VTermStateFallbacks *fallbacks, శూన్య *user);
శూన్య *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen);

శూన్య vterm_screen_enable_reflow(VTermScreen *screen, పూర్ణాంకం reflow);

// Back-compat alias for the brief time it was in 0.3-RC1
#నిర్వచించు vterm_screen_సమితి_reflow  vterm_screen_enable_reflow

// Enable support for using the alternate screen if "altscreen" is non-zero.
// Before that switching to the alternate screen won't work.
// Calling with "altscreen" zero has no effect.
శూన్య vterm_screen_enable_altscreen(VTermScreen *screen, పూర్ణాంకం altscreen);

రకనిర్వచనం గణన {
  VTERM_DAMAGE_CELL,    /* every cell */
  VTERM_DAMAGE_అడ్డువరుస,     /* entire rows */
  VTERM_DAMAGE_SCREEN,  /* entire screen */
  VTERM_DAMAGE_SCROLL,  /* entire screen + scrollrect */

  VTERM_N_DAMAGES
} VTerm_Damage_కొలత;

// Invoke the relevant callbacks to update the screen.
శూన్య vterm_screen_flush_damage(VTermScreen *screen);

శూన్య vterm_screen_సమితి_damage_విలీనం(VTermScreen *screen, VTerm_Damage_కొలత కొలత);

/*
 * Reset the screen.  Also invokes vterm_state_reset().
 * Must be called before the terminal can be used.
 */
శూన్య   vterm_screen_reset(VTermScreen *screen, పూర్ణాంకం hard);

/* Neither of these functions NUL-terminate the buffer */
size_t vterm_screen_get_chars(స్థిరరాశి VTermScreen *screen, uint32_t *chars, size_t len, స్థిరరాశి VTermRect rect);
size_t vterm_screen_get_పాఠ్యం(స్థిరరాశి VTermScreen *screen, అక్షరం *str, size_t len, స్థిరరాశి VTermRect rect);

రకనిర్వచనం గణన {
  VTERM_ATTR_BOLD_ముసుగు       = 1 << 0,
  VTERM_ATTR_UNDERLINE_ముసుగు  = 1 << 1,
  VTERM_ATTR_ITALIC_ముసుగు     = 1 << 2,
  VTERM_ATTR_BLINK_ముసుగు      = 1 << 3,
  VTERM_ATTR_తిరగేయి_ముసుగు    = 1 << 4,
  VTERM_ATTR_STRIKE_ముసుగు     = 1 << 5,
  VTERM_ATTR_FONT_ముసుగు       = 1 << 6,
  VTERM_ATTR_FOREGROUND_ముసుగు = 1 << 7,
  VTERM_ATTR_BACKGROUND_ముసుగు = 1 << 8,
  VTERM_ATTR_CONCEAL_ముసుగు    = 1 << 9,
  VTERM_ATTR_SMALL_ముసుగు      = 1 << 10,
  VTERM_ATTR_BASELINE_ముసుగు   = 1 << 11,

  VTERM_ALL_ATTRS_ముసుగు = (1 << 12) - 1
} VTerm_Attr_ముసుగు;

పూర్ణాంకం vterm_screen_get_attrs_extent(స్థిరరాశి VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_ముసుగు attrs);

పూర్ణాంకం vterm_screen_get_cell(స్థిరరాశి VTermScreen *screen, VTermPos pos, VTermScreenCell *cell);

పూర్ణాంకం vterm_screen_is_eol(స్థిరరాశి VTermScreen *screen, VTermPos pos);

/**
 * Same as vterm_state_convert_color_to_rgb(), but takes a `screen` instead of a `state`
 * instance.
 */
శూన్య vterm_screen_convert_color_గమ్యం_rgb(స్థిరరాశి VTermScreen *screen, VTermColor *నిలువు);

/**
 * Similar to vterm_state_set_default_colors(), but also resets colours in the
 * screen buffer(s)
 */
శూన్య vterm_screen_సమితి_default_colors(VTermScreen *screen, స్థిరరాశి VTermColor *default_fg, స్థిరరాశి VTermColor *default_bg);

// ---------
// Utilities
// ---------

VTerm_విలువ_Type vterm_get_attr_type(VTermAttr attr);
VTerm_విలువ_Type vterm_get_prop_type(VTermProp prop);

శూన్య vterm_scroll_rect(VTermRect rect,
                       పూర్ణాంకం downward,
                       పూర్ణాంకం rightward,
                       పూర్ణాంకం (*moverect)(VTermRect src, VTermRect dest, శూన్య *user),
                       పూర్ణాంకం (*eraserect)(VTermRect rect, పూర్ణాంకం selective, శూన్య *user),
                       శూన్య *user);

శూన్య vterm_copy_cells(VTermRect dest,
                      VTermRect src,
                      శూన్య (*copycell)(VTermPos dest, VTermPos src, శూన్య *user),
                      శూన్య *user);

#నిర్వచితమైతే __cplusplus
}
#అయితే_ముగింపు

#అయితే_ముగింపు
