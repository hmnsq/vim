#ଅନ୍ତର୍ଭାଵ "vterm_internal.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  uint8_t red, green, blue;
} VTermRGB;

ସ୍ଥିର ଅଚଳ VTermRGB ansi_colors[] = {
  /* R    G    B */
  {   0,   0,   0 }, // black
  { 224,   0,   0 }, // red
  {   0, 224,   0 }, // green
  { 224, 224,   0 }, // yellow
  {   0,   0, 224 }, // blue
  { 224,   0, 224 }, // magenta
  {   0, 224, 224 }, // cyan
  { 224, 224, 224 }, // white == light grey

  // high intensity
  { 128, 128, 128 }, // black
  { 255,  64,  64 }, // red
  {  64, 255,  64 }, // green
  { 255, 255,  64 }, // yellow
  {  64,  64, 255 }, // blue
  { 255,  64, 255 }, // magenta
  {  64, 255, 255 }, // cyan
  { 255, 255, 255 }, // white for real
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

ସ୍ଥିର ଶୂନ୍ୟ lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup_colour_ansi(ଅଚଳ VTermState *state, long index, VTermColor *col)
{
  ଯଦି(index >= 0 && index < 16) {
    *col = state->colors[index];
    ଫେରନ୍ତୁ TRUE;
  }

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup_colour_palette(ଅଚଳ VTermState *state, long index, VTermColor *col)
{
  ଯଦି(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    ଫେରନ୍ତୁ lookup_colour_ansi(state, index, col);
  }
  ଅନ୍ୟଥା ଯଦି(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    ଫେରନ୍ତୁ TRUE;
  }

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lookup_colour(ଅଚଳ VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା palette, ଅଚଳ long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, VTermColor *col)
{
  ବାଛ(palette) {
  ସ୍ଥିତି 2: // RGB mode - 3 args contain colour values directly
    ଯଦି(argcount < 3)
      ଫେରନ୍ତୁ argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    ଫେରନ୍ତୁ 3;

  ସ୍ଥିତି 5: // XTerm 256-colour mode
    ଯଦି (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      ଫେରନ୍ତୁ argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    ଫେରନ୍ତୁ 1;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    ଫେରନ୍ତୁ 0;
  }
}

// Some conveniences

ସ୍ଥିର ଶୂନ୍ୟ setpenattr(VTermState *state, VTermAttr attr, VTerm_ମୂଲ୍ୟ_Type type UNUSED, VTerm_ମୂଲ୍ୟ *val)
{
#ifdef DEBUG
  ଯଦି(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    ଫେରନ୍ତୁ;
  }
#endif
  ଯଦି(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

ସ୍ଥିର ଶୂନ୍ୟ setpenattr_bool(VTermState *state, VTermAttr attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା boolean)
{
  VTerm_ମୂଲ୍ୟ val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

ସ୍ଥିର ଶୂନ୍ୟ setpenattr_int(VTermState *state, VTermAttr attr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ସଂଖ୍ୟା)
{
  VTerm_ମୂଲ୍ୟ val;
  val.ସଂଖ୍ୟା = ସଂଖ୍ୟା;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

ସ୍ଥିର ଶୂନ୍ୟ setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_ମୂଲ୍ୟ val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

ସ୍ଥିର ଶୂନ୍ୟ set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL ଶୂନ୍ୟ vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL ଶୂନ୍ୟ vterm_state_resetpen(VTermState *state)
{
  state->pen.bold = 0;      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
  state->pen.underline = 0; setpenattr_int (state, VTERM_ATTR_UNDERLINE, 0);
  state->pen.italic = 0;    setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
  state->pen.blink = 0;     setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
  state->pen.reverse = 0;   setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
  state->pen.conceal = 0;   setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
  state->pen.strike = 0;    setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
  state->pen.font = 0;      setpenattr_int (state, VTERM_ATTR_FONT, 0);
  state->pen.small = 0;     setpenattr_bool(state, VTERM_ATTR_SMALL, 0);
  state->pen.baseline = 0;  setpenattr_int (state, VTERM_ATTR_BASELINE, 0);

  state->pen.fg = state->default_fg;  setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->default_fg);
  state->pen.bg = state->default_bg;  setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->default_bg);
}

INTERNAL ଶୂନ୍ୟ vterm_state_savepen(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା save)
{
  ଯଦି(save) {
    state->saved.pen = state->pen;
  }
  ଅନ୍ୟଥା {
    state->pen = state->saved.pen;

    setpenattr_bool(state, VTERM_ATTR_BOLD,      state->pen.bold);
    setpenattr_int (state, VTERM_ATTR_UNDERLINE, state->pen.underline);
    setpenattr_bool(state, VTERM_ATTR_ITALIC,    state->pen.italic);
    setpenattr_bool(state, VTERM_ATTR_BLINK,     state->pen.blink);
    setpenattr_bool(state, VTERM_ATTR_REVERSE,   state->pen.reverse);
    setpenattr_bool(state, VTERM_ATTR_CONCEAL,   state->pen.conceal);
    setpenattr_bool(state, VTERM_ATTR_STRIKE,    state->pen.strike);
    setpenattr_int (state, VTERM_ATTR_FONT,      state->pen.font);
    setpenattr_bool(state, VTERM_ATTR_SMALL,     state->pen.small);
    setpenattr_int (state, VTERM_ATTR_BASELINE,  state->pen.baseline);

    setpenattr_col( state, VTERM_ATTR_FOREGROUND, state->pen.fg);
    setpenattr_col( state, VTERM_ATTR_BACKGROUND, state->pen.bg);
  }
}

ଶୂନ୍ୟ vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

ଶୂନ୍ୟ vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_color_is_equal(ଅଚଳ VTermColor *a, ଅଚଳ VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  ଯଦି (a->type != b->type) {
    ଫେରନ୍ତୁ FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  ଯଦି (VTERM_COLOR_IS_INDEXED(a)) {
    ଫେରନ୍ତୁ a->index == b->index;
  }
  ଅନ୍ୟଥା ଯଦି (VTERM_COLOR_IS_RGB(a)) {
    ଫେରନ୍ତୁ    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vterm_state_get_default_colors(ଅଚଳ VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

ଶୂନ୍ୟ vterm_state_get_palette_color(ଅଚଳ VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

ଶୂନ୍ୟ vterm_state_set_default_colors(VTermState *state, ଅଚଳ VTermColor *default_fg, ଅଚଳ VTermColor *default_bg)
{
  ଯଦି(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  ଯଦି(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

ଶୂନ୍ୟ vterm_state_set_palette_color(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ଅଚଳ VTermColor *col)
{
  ଯଦି(index >= 0 && index < 16)
    state->colors[index] = *col;
}

ଶୂନ୍ୟ vterm_state_convert_color_to_rgb(ଅଚଳ VTermState *state, VTermColor *col)
{
  ଯଦି (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

ଶୂନ୍ୟ vterm_state_set_bold_highbright(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL ଶୂନ୍ୟ vterm_state_setpen(VTermState *state, ଅଚଳ long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount)
{
  // SGR - ECMA-48 8.3.117

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା argi = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଲ୍ୟ;

  ଯେତେବେଳେ(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = 1;

    long arg;
    ବାଛ(arg = CSI_ARG(args[argi])) {
    ସ୍ଥିତି CSI_ARG_MISSING:
    ସ୍ଥିତି 0: // Reset
      vterm_state_resetpen(state);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1: { // Bold on
      ଅଚଳ VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      ଯଦି(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      ଯଦି(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        ବାଛ(CSI_ARG(args[argi])) {
          ସ୍ଥିତି 0:
            state->pen.underline = 0;
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            ଭାଙ୍ଗ;
          ସ୍ଥିତି 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            ଭାଙ୍ଗ;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 10: ସ୍ଥିତି 11: ସ୍ଥିତି 12: ସ୍ଥିତି 13: ସ୍ଥିତି 14:
    ସ୍ଥିତି 15: ସ୍ଥିତି 16: ସ୍ଥିତି 17: ସ୍ଥିତି 18: ସ୍ଥିତି 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 30: ସ୍ଥିତି 31: ସ୍ଥିତି 32: ସ୍ଥିତି 33:
    ସ୍ଥିତି 34: ସ୍ଥିତି 35: ସ୍ଥିତି 36: ସ୍ଥିତି 37: // Foreground colour palette
      ମୂଲ୍ୟ = CSI_ARG(args[argi]) - 30;
      ଯଦି(state->pen.bold && state->bold_is_highbright)
        ମୂଲ୍ୟ += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, ମୂଲ୍ୟ);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 38: // Foreground colour alternative palette
      ଯଦି(argcount - argi < 1)
        ଫେରନ୍ତୁ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 40: ସ୍ଥିତି 41: ସ୍ଥିତି 42: ସ୍ଥିତି 43:
    ସ୍ଥିତି 44: ସ୍ଥିତି 45: ସ୍ଥିତି 46: ସ୍ଥିତି 47: // Background colour palette
      ମୂଲ୍ୟ = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, ମୂଲ୍ୟ);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 48: // Background colour alternative palette
      ଯଦି(argcount - argi < 1)
        ଫେରନ୍ତୁ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 73: // Superscript
    ସ୍ଥିତି 74: // Subscript
    ସ୍ଥିତି 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 90: ସ୍ଥିତି 91: ସ୍ଥିତି 92: ସ୍ଥିତି 93:
    ସ୍ଥିତି 94: ସ୍ଥିତି 95: ସ୍ଥିତି 96: ସ୍ଥିତି 97: // Foreground colour high-intensity palette
      ମୂଲ୍ୟ = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, ମୂଲ୍ୟ);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 100: ସ୍ଥିତି 101: ସ୍ଥିତି 102: ସ୍ଥିତି 103:
    ସ୍ଥିତି 104: ସ୍ଥିତି 105: ସ୍ଥିତି 106: ସ୍ଥିତି 107: // Background colour high-intensity palette
      ମୂଲ୍ୟ = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, ମୂଲ୍ୟ);
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      done = 0;
      ଭାଙ୍ଗ;
    }

    ଯଦି(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    ଯେତେବେଳେ(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_state_getpen_color(ଅଚଳ VTermColor *col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argi, long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା fg)
{
    /* Do nothing if the given color is the default color */
    ଯଦି (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        ଫେରନ୍ତୁ argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    ଯଦି (VTERM_COLOR_IS_INDEXED(col)) {
        ଅଚଳ uint8_t idx = col->index;
        ଯଦି (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        ଅନ୍ୟଥା ଯଦି (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        ଅନ୍ୟଥା {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    ଅନ୍ୟଥା ଯଦି (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    ଫେରନ୍ତୁ argi;
}

INTERNAL ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_state_getpen(VTermState *state, long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount UNUSED)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା argi = 0;

  ଯଦି(state->pen.bold)
    args[argi++] = 1;

  ଯଦି(state->pen.italic)
    args[argi++] = 3;

  ଯଦି(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  ଯଦି(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  ଯଦି(state->pen.blink)
    args[argi++] = 5;

  ଯଦି(state->pen.reverse)
    args[argi++] = 7;

  ଯଦି(state->pen.conceal)
    args[argi++] = 8;

  ଯଦି(state->pen.strike)
    args[argi++] = 9;

  ଯଦି(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  ଯଦି(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  ଯଦି(state->pen.small) {
    ଯଦି(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    ଅନ୍ୟଥା ଯଦି(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  ଫେରନ୍ତୁ argi;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_state_get_penattr(ଅଚଳ VTermState *state, VTermAttr attr, VTerm_ମୂଲ୍ୟ *val)
{
  ବାଛ(attr) {
  ସ୍ଥିତି VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_UNDERLINE:
    val->ସଂଖ୍ୟା = state->pen.underline;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_FONT:
    val->ସଂଖ୍ୟା = state->pen.font;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_ATTR_BASELINE:
    val->ସଂଖ୍ୟା = state->pen.baseline;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_N_ATTRS:
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ 0;
}
