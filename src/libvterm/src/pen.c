#అన్తర్భావ "vterm_internal.h"

#అన్తర్భావ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
రకనిర్వచనం నిర్మాణం {
  uint8_t red, green, blue;
} VTermRGB;

స్థిర స్థిరరాశి VTermRGB ansi_colors[] = {
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

స్థిర పూర్ణాంకం ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
స్థిర పూర్ణాంకం ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

స్థిర శూన్య lookup_default_రంగు_ansi(దీర్ఘ idx, VTermColor *నిలువు)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      నిలువు,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  నిలువు->సూచీ = (uint8_t)idx;
  నిలువు->type = VTERM_COLOR_సూచీకృత;
}

స్థిర పూర్ణాంకం lookup_రంగు_ansi(స్థిరరాశి VTermState *state, దీర్ఘ సూచీ, VTermColor *నిలువు)
{
  ఒకవేళ(సూచీ >= 0 && సూచీ < 16) {
    *నిలువు = state->colors[సూచీ];
    తిరిగి TRUE;
  }

  తిరిగి FALSE;
}

స్థిర పూర్ణాంకం lookup_రంగు_palette(స్థిరరాశి VTermState *state, దీర్ఘ సూచీ, VTermColor *నిలువు)
{
  ఒకవేళ(సూచీ >= 0 && సూచీ < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    తిరిగి lookup_రంగు_ansi(state, సూచీ, నిలువు);
  }
  లేకపోతే ఒకవేళ(సూచీ >= 16 && సూచీ < 232) {
    // 216-colour cube
    సూచీ -= 16;

    vterm_color_rgb(నిలువు, ramp6[సూచీ/6/6 % 6],
                         ramp6[సూచీ/6   % 6],
                         ramp6[సూచీ     % 6]);

    తిరిగి TRUE;
  }
  లేకపోతే ఒకవేళ(సూచీ >= 232 && సూచీ < 256) {
    // 24 greyscales
    సూచీ -= 232;

    vterm_color_rgb(నిలువు, ramp24[సూచీ], ramp24[సూచీ], ramp24[సూచీ]);

    తిరిగి TRUE;
  }

  తిరిగి FALSE;
}

స్థిర పూర్ణాంకం lookup_రంగు(స్థిరరాశి VTermState *state, పూర్ణాంకం palette, స్థిరరాశి దీర్ఘ వాదనలు[], పూర్ణాంకం argcount, VTermColor *నిలువు)
{
  ఎంపిక(palette) {
  స్థితి 2: // RGB mode - 3 args contain colour values directly
    ఒకవేళ(argcount < 3)
      తిరిగి argcount;

    vterm_color_rgb(నిలువు, (uint8_t)CSI_ARG(వాదనలు[0]), (uint8_t)CSI_ARG(వాదనలు[1]), (uint8_t)CSI_ARG(వాదనలు[2]));

    తిరిగి 3;

  స్థితి 5: // XTerm 256-colour mode
    ఒకవేళ (!argcount || CSI_ARG_IS_MISSING(వాదనలు[0])) {
      తిరిగి argcount ? 1 : 0;
    }

    lookup_రంగు_palette(state, వాదనలు[0], నిలువు);
    తిరిగి 1;

  అప్రమేయ:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    తిరిగి 0;
  }
}

// Some conveniences

స్థిర శూన్య setpenattr(VTermState *state, VTermAttr attr, VTerm_విలువ_Type type వాడని, VTerm_విలువ *val)
{
#నిర్వచితమైతే DEBUG
  ఒకవేళ(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    తిరిగి;
  }
#అయితే_ముగింపు
  ఒకవేళ(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

స్థిర శూన్య setpenattr_bool(VTermState *state, VTermAttr attr, పూర్ణాంకం boolean)
{
  VTerm_విలువ val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

స్థిర శూన్య setpenattr_int(VTermState *state, VTermAttr attr, పూర్ణాంకం సంఖ్య)
{
  VTerm_విలువ val;
  val.సంఖ్య = సంఖ్య;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

స్థిర శూన్య setpenattr_నిలువు(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_విలువ val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

స్థిర శూన్య సమితి_pen_నిలువు_ansi(VTermState *state, VTermAttr attr, దీర్ఘ నిలువు)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_రంగు_ansi(state, నిలువు, colp);

  setpenattr_నిలువు(state, attr, *colp);
}

అంతర్గత శూన్య vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_సమితి_default_colors(state, &state->default_fg, &state->default_bg);

  కొరకు(పూర్ణాంకం నిలువు = 0; నిలువు < 16; నిలువు++)
    lookup_default_రంగు_ansi(నిలువు, &state->colors[నిలువు]);
}

అంతర్గత శూన్య vterm_state_resetpen(VTermState *state)
{
  state->pen.bold = 0;      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
  state->pen.underline = 0; setpenattr_int (state, VTERM_ATTR_UNDERLINE, 0);
  state->pen.italic = 0;    setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
  state->pen.blink = 0;     setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
  state->pen.తిరగేయి = 0;   setpenattr_bool(state, VTERM_ATTR_తిరగేయి, 0);
  state->pen.conceal = 0;   setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
  state->pen.strike = 0;    setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
  state->pen.font = 0;      setpenattr_int (state, VTERM_ATTR_FONT, 0);
  state->pen.small = 0;     setpenattr_bool(state, VTERM_ATTR_SMALL, 0);
  state->pen.baseline = 0;  setpenattr_int (state, VTERM_ATTR_BASELINE, 0);

  state->pen.fg = state->default_fg;  setpenattr_నిలువు(state, VTERM_ATTR_FOREGROUND, state->default_fg);
  state->pen.bg = state->default_bg;  setpenattr_నిలువు(state, VTERM_ATTR_BACKGROUND, state->default_bg);
}

అంతర్గత శూన్య vterm_state_savepen(VTermState *state, పూర్ణాంకం save)
{
  ఒకవేళ(save) {
    state->saved.pen = state->pen;
  }
  లేకపోతే {
    state->pen = state->saved.pen;

    setpenattr_bool(state, VTERM_ATTR_BOLD,      state->pen.bold);
    setpenattr_int (state, VTERM_ATTR_UNDERLINE, state->pen.underline);
    setpenattr_bool(state, VTERM_ATTR_ITALIC,    state->pen.italic);
    setpenattr_bool(state, VTERM_ATTR_BLINK,     state->pen.blink);
    setpenattr_bool(state, VTERM_ATTR_తిరగేయి,   state->pen.తిరగేయి);
    setpenattr_bool(state, VTERM_ATTR_CONCEAL,   state->pen.conceal);
    setpenattr_bool(state, VTERM_ATTR_STRIKE,    state->pen.strike);
    setpenattr_int (state, VTERM_ATTR_FONT,      state->pen.font);
    setpenattr_bool(state, VTERM_ATTR_SMALL,     state->pen.small);
    setpenattr_int (state, VTERM_ATTR_BASELINE,  state->pen.baseline);

    setpenattr_నిలువు( state, VTERM_ATTR_FOREGROUND, state->pen.fg);
    setpenattr_నిలువు( state, VTERM_ATTR_BACKGROUND, state->pen.bg);
  }
}

శూన్య vterm_color_rgb(VTermColor *నిలువు, uint8_t red, uint8_t green, uint8_t blue)
{
  నిలువు->type = VTERM_COLOR_RGB;
  నిలువు->red   = red;
  నిలువు->green = green;
  నిలువు->blue  = blue;
}

శూన్య vterm_color_సూచీకృత(VTermColor *నిలువు, uint8_t idx)
{
  నిలువు->type = VTERM_COLOR_సూచీకృత;
  నిలువు->సూచీ = idx;
}

పూర్ణాంకం vterm_color_is_equal(స్థిరరాశి VTermColor *a, స్థిరరాశి VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  ఒకవేళ (a->type != b->type) {
    తిరిగి FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  ఒకవేళ (VTERM_COLOR_IS_సూచీకృత(a)) {
    తిరిగి a->సూచీ == b->సూచీ;
  }
  లేకపోతే ఒకవేళ (VTERM_COLOR_IS_RGB(a)) {
    తిరిగి    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  తిరిగి 0;
}

శూన్య vterm_state_get_default_colors(స్థిరరాశి VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

శూన్య vterm_state_get_palette_color(స్థిరరాశి VTermState *state, పూర్ణాంకం సూచీ, VTermColor *నిలువు)
{
  lookup_రంగు_palette(state, సూచీ, నిలువు);
}

శూన్య vterm_state_సమితి_default_colors(VTermState *state, స్థిరరాశి VTermColor *default_fg, స్థిరరాశి VTermColor *default_bg)
{
  ఒకవేళ(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_ముసుగు)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  ఒకవేళ(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_ముసుగు)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

శూన్య vterm_state_సమితి_palette_color(VTermState *state, పూర్ణాంకం సూచీ, స్థిరరాశి VTermColor *నిలువు)
{
  ఒకవేళ(సూచీ >= 0 && సూచీ < 16)
    state->colors[సూచీ] = *నిలువు;
}

శూన్య vterm_state_convert_color_గమ్యం_rgb(స్థిరరాశి VTermState *state, VTermColor *నిలువు)
{
  ఒకవేళ (VTERM_COLOR_IS_సూచీకృత(నిలువు)) { /* Convert indexed colors to RGB */
    lookup_రంగు_palette(state, నిలువు->సూచీ, నిలువు);
  }
  నిలువు->type &= VTERM_COLOR_TYPE_ముసుగు; /* Reset any metadata but the type */
}

శూన్య vterm_state_సమితి_bold_highbright(VTermState *state, పూర్ణాంకం bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

అంతర్గత శూన్య vterm_state_setpen(VTermState *state, స్థిరరాశి దీర్ఘ వాదనలు[], పూర్ణాంకం argcount)
{
  // SGR - ECMA-48 8.3.117

  పూర్ణాంకం argi = 0;
  పూర్ణాంకం విలువ;

  అయితే(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    పూర్ణాంకం పూర్తి = 1;

    దీర్ఘ arg;
    ఎంపిక(arg = CSI_ARG(వాదనలు[argi])) {
    స్థితి CSI_ARG_MISSING:
    స్థితి 0: // Reset
      vterm_state_resetpen(state);
      ఆపు;

    స్థితి 1: { // Bold on
      స్థిరరాశి VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      ఒకవేళ(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_సూచీకృత(fg) && fg->సూచీ < 8 && state->bold_is_highbright)
        సమితి_pen_నిలువు_ansi(state, VTERM_ATTR_FOREGROUND, fg->సూచీ + (state->pen.bold ? 8 : 0));
      ఆపు;
    }

    స్థితి 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      ఆపు;

    స్థితి 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      ఒకవేళ(CSI_ARG_HAS_మరిన్ని(వాదనలు[argi])) {
        argi++;
        ఎంపిక(CSI_ARG(వాదనలు[argi])) {
          స్థితి 0:
            state->pen.underline = 0;
            ఆపు;
          స్థితి 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            ఆపు;
          స్థితి 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            ఆపు;
          స్థితి 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            ఆపు;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ఆపు;

    స్థితి 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      ఆపు;

    స్థితి 7: // Reverse on
      state->pen.తిరగేయి = 1;
      setpenattr_bool(state, VTERM_ATTR_తిరగేయి, 1);
      ఆపు;

    స్థితి 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      ఆపు;

    స్థితి 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      ఆపు;

    స్థితి 10: స్థితి 11: స్థితి 12: స్థితి 13: స్థితి 14:
    స్థితి 15: స్థితి 16: స్థితి 17: స్థితి 18: స్థితి 19: // Select font
      state->pen.font = CSI_ARG(వాదనలు[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      ఆపు;

    స్థితి 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ఆపు;

    స్థితి 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      ఆపు;

    స్థితి 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      ఆపు;

    స్థితి 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      ఆపు;

    స్థితి 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      ఆపు;

    స్థితి 27: // Reverse off
      state->pen.తిరగేయి = 0;
      setpenattr_bool(state, VTERM_ATTR_తిరగేయి, 0);
      ఆపు;

    స్థితి 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      ఆపు;

    స్థితి 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      ఆపు;

    స్థితి 30: స్థితి 31: స్థితి 32: స్థితి 33:
    స్థితి 34: స్థితి 35: స్థితి 36: స్థితి 37: // Foreground colour palette
      విలువ = CSI_ARG(వాదనలు[argi]) - 30;
      ఒకవేళ(state->pen.bold && state->bold_is_highbright)
        విలువ += 8;
      సమితి_pen_నిలువు_ansi(state, VTERM_ATTR_FOREGROUND, విలువ);
      ఆపు;

    స్థితి 38: // Foreground colour alternative palette
      ఒకవేళ(argcount - argi < 1)
        తిరిగి;
      argi += 1 + lookup_రంగు(state, CSI_ARG(వాదనలు[argi+1]), వాదనలు+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_నిలువు(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ఆపు;

    స్థితి 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_నిలువు(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ఆపు;

    స్థితి 40: స్థితి 41: స్థితి 42: స్థితి 43:
    స్థితి 44: స్థితి 45: స్థితి 46: స్థితి 47: // Background colour palette
      విలువ = CSI_ARG(వాదనలు[argi]) - 40;
      సమితి_pen_నిలువు_ansi(state, VTERM_ATTR_BACKGROUND, విలువ);
      ఆపు;

    స్థితి 48: // Background colour alternative palette
      ఒకవేళ(argcount - argi < 1)
        తిరిగి;
      argi += 1 + lookup_రంగు(state, CSI_ARG(వాదనలు[argi+1]), వాదనలు+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_నిలువు(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ఆపు;

    స్థితి 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_నిలువు(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ఆపు;

    స్థితి 73: // Superscript
    స్థితి 74: // Subscript
    స్థితి 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_క్రింది :
                      VTERM_BASELINE_సాధారణ;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      ఆపు;

    స్థితి 90: స్థితి 91: స్థితి 92: స్థితి 93:
    స్థితి 94: స్థితి 95: స్థితి 96: స్థితి 97: // Foreground colour high-intensity palette
      విలువ = CSI_ARG(వాదనలు[argi]) - 90 + 8;
      సమితి_pen_నిలువు_ansi(state, VTERM_ATTR_FOREGROUND, విలువ);
      ఆపు;

    స్థితి 100: స్థితి 101: స్థితి 102: స్థితి 103:
    స్థితి 104: స్థితి 105: స్థితి 106: స్థితి 107: // Background colour high-intensity palette
      విలువ = CSI_ARG(వాదనలు[argi]) - 100 + 8;
      సమితి_pen_నిలువు_ansi(state, VTERM_ATTR_BACKGROUND, విలువ);
      ఆపు;

    అప్రమేయ:
      పూర్తి = 0;
      ఆపు;
    }

    ఒకవేళ(!పూర్తి)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    అయితే(CSI_ARG_HAS_మరిన్ని(వాదనలు[argi++]))
      ;
  }
}

స్థిర పూర్ణాంకం vterm_state_getpen_color(స్థిరరాశి VTermColor *నిలువు, పూర్ణాంకం argi, దీర్ఘ వాదనలు[], పూర్ణాంకం fg)
{
    /* Do nothing if the given color is the default color */
    ఒకవేళ (( fg && VTERM_COLOR_IS_DEFAULT_FG(నిలువు)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(నిలువు))) {
        తిరిగి argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    ఒకవేళ (VTERM_COLOR_IS_సూచీకృత(నిలువు)) {
        స్థిరరాశి uint8_t idx = నిలువు->సూచీ;
        ఒకవేళ (idx < 8) {
            వాదనలు[argi++] = (idx + (fg ? 30 : 40));
        }
        లేకపోతే ఒకవేళ (idx < 16) {
            వాదనలు[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        లేకపోతే {
            వాదనలు[argi++] = CSI_ARG_FLAG_మరిన్ని | (fg ? 38 : 48);
            వాదనలు[argi++] = CSI_ARG_FLAG_మరిన్ని | 5;
            వాదనలు[argi++] = idx;
        }
    }
    లేకపోతే ఒకవేళ (VTERM_COLOR_IS_RGB(నిలువు)) {
        వాదనలు[argi++] = CSI_ARG_FLAG_మరిన్ని | (fg ? 38 : 48);
        వాదనలు[argi++] = CSI_ARG_FLAG_మరిన్ని | 2;
        వాదనలు[argi++] = CSI_ARG_FLAG_మరిన్ని | నిలువు->red;
        వాదనలు[argi++] = CSI_ARG_FLAG_మరిన్ని | నిలువు->green;
        వాదనలు[argi++] = నిలువు->blue;
    }
    తిరిగి argi;
}

అంతర్గత పూర్ణాంకం vterm_state_getpen(VTermState *state, దీర్ఘ వాదనలు[], పూర్ణాంకం argcount వాడని)
{
  పూర్ణాంకం argi = 0;

  ఒకవేళ(state->pen.bold)
    వాదనలు[argi++] = 1;

  ఒకవేళ(state->pen.italic)
    వాదనలు[argi++] = 3;

  ఒకవేళ(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    వాదనలు[argi++] = 4;
  ఒకవేళ(state->pen.underline == VTERM_UNDERLINE_CURLY)
    వాదనలు[argi++] = 4 | CSI_ARG_FLAG_మరిన్ని, వాదనలు[argi++] = 3;

  ఒకవేళ(state->pen.blink)
    వాదనలు[argi++] = 5;

  ఒకవేళ(state->pen.తిరగేయి)
    వాదనలు[argi++] = 7;

  ఒకవేళ(state->pen.conceal)
    వాదనలు[argi++] = 8;

  ఒకవేళ(state->pen.strike)
    వాదనలు[argi++] = 9;

  ఒకవేళ(state->pen.font)
    వాదనలు[argi++] = 10 + state->pen.font;

  ఒకవేళ(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    వాదనలు[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, వాదనలు, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, వాదనలు, FALSE);

  ఒకవేళ(state->pen.small) {
    ఒకవేళ(state->pen.baseline == VTERM_BASELINE_RAISE)
      వాదనలు[argi++] = 73;
    లేకపోతే ఒకవేళ(state->pen.baseline == VTERM_BASELINE_క్రింది)
      వాదనలు[argi++] = 74;
  }

  తిరిగి argi;
}

పూర్ణాంకం vterm_state_get_penattr(స్థిరరాశి VTermState *state, VTermAttr attr, VTerm_విలువ *val)
{
  ఎంపిక(attr) {
  స్థితి VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    తిరిగి 1;

  స్థితి VTERM_ATTR_UNDERLINE:
    val->సంఖ్య = state->pen.underline;
    తిరిగి 1;

  స్థితి VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    తిరిగి 1;

  స్థితి VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    తిరిగి 1;

  స్థితి VTERM_ATTR_తిరగేయి:
    val->boolean = state->pen.తిరగేయి;
    తిరిగి 1;

  స్థితి VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    తిరిగి 1;

  స్థితి VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    తిరిగి 1;

  స్థితి VTERM_ATTR_FONT:
    val->సంఖ్య = state->pen.font;
    తిరిగి 1;

  స్థితి VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    తిరిగి 1;

  స్థితి VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    తిరిగి 1;

  స్థితి VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    తిరిగి 1;

  స్థితి VTERM_ATTR_BASELINE:
    val->సంఖ్య = state->pen.baseline;
    తిరిగి 1;

  స్థితి VTERM_N_ATTRS:
    తిరిగి 0;
  }

  తిరిగి 0;
}
