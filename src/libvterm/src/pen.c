#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  uint8_t red, green, blue;
} VTermRGB;

ਸਥਿਰ ਅਚਲ VTermRGB ansi_colors[] = {
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

ਸਥਿਰ ਪੂਰਨ ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
ਸਥਿਰ ਪੂਰਨ ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

ਸਥਿਰ ਖਾਲੀ lookup_default_ਰੰਗ_ansi(ਲੰਮਾ idx, VTermColor *ਖਾਨਾ)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      ਖਾਨਾ,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  ਖਾਨਾ->ਸੂਚਕਾਂਕ = (uint8_t)idx;
  ਖਾਨਾ->type = VTERM_COLOR_ਸੂਚਕਾਂਕਿਤ;
}

ਸਥਿਰ ਪੂਰਨ lookup_ਰੰਗ_ansi(ਅਚਲ VTermState *state, ਲੰਮਾ ਸੂਚਕਾਂਕ, VTermColor *ਖਾਨਾ)
{
  ਜੇ(ਸੂਚਕਾਂਕ >= 0 && ਸੂਚਕਾਂਕ < 16) {
    *ਖਾਨਾ = state->colors[ਸੂਚਕਾਂਕ];
    ਵਾਪਸ TRUE;
  }

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਪੂਰਨ lookup_ਰੰਗ_palette(ਅਚਲ VTermState *state, ਲੰਮਾ ਸੂਚਕਾਂਕ, VTermColor *ਖਾਨਾ)
{
  ਜੇ(ਸੂਚਕਾਂਕ >= 0 && ਸੂਚਕਾਂਕ < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    ਵਾਪਸ lookup_ਰੰਗ_ansi(state, ਸੂਚਕਾਂਕ, ਖਾਨਾ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ >= 16 && ਸੂਚਕਾਂਕ < 232) {
    // 216-colour cube
    ਸੂਚਕਾਂਕ -= 16;

    vterm_color_rgb(ਖਾਨਾ, ramp6[ਸੂਚਕਾਂਕ/6/6 % 6],
                         ramp6[ਸੂਚਕਾਂਕ/6   % 6],
                         ramp6[ਸੂਚਕਾਂਕ     % 6]);

    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਸੂਚਕਾਂਕ >= 232 && ਸੂਚਕਾਂਕ < 256) {
    // 24 greyscales
    ਸੂਚਕਾਂਕ -= 232;

    vterm_color_rgb(ਖਾਨਾ, ramp24[ਸੂਚਕਾਂਕ], ramp24[ਸੂਚਕਾਂਕ], ramp24[ਸੂਚਕਾਂਕ]);

    ਵਾਪਸ TRUE;
  }

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ ਪੂਰਨ lookup_ਰੰਗ(ਅਚਲ VTermState *state, ਪੂਰਨ palette, ਅਚਲ ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount, VTermColor *ਖਾਨਾ)
{
  ਚੋਣ(palette) {
  ਹਾਲਤ 2: // RGB mode - 3 args contain colour values directly
    ਜੇ(argcount < 3)
      ਵਾਪਸ argcount;

    vterm_color_rgb(ਖਾਨਾ, (uint8_t)CSI_ARG(ਦਲੀਲਾਂ[0]), (uint8_t)CSI_ARG(ਦਲੀਲਾਂ[1]), (uint8_t)CSI_ARG(ਦਲੀਲਾਂ[2]));

    ਵਾਪਸ 3;

  ਹਾਲਤ 5: // XTerm 256-colour mode
    ਜੇ (!argcount || CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[0])) {
      ਵਾਪਸ argcount ? 1 : 0;
    }

    lookup_ਰੰਗ_palette(state, ਦਲੀਲਾਂ[0], ਖਾਨਾ);
    ਵਾਪਸ 1;

  ਮੂਲ:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    ਵਾਪਸ 0;
  }
}

// Some conveniences

ਸਥਿਰ ਖਾਲੀ setpenattr(VTermState *state, VTermAttr attr, VTerm_ਮੁੱਲ_Type type ਅਣਵਰਤਿਆ, VTerm_ਮੁੱਲ *val)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
  ਜੇ(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    ਵਾਪਸ;
  }
#ਜੇ_ਅੰਤ
  ਜੇ(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

ਸਥਿਰ ਖਾਲੀ setpenattr_bool(VTermState *state, VTermAttr attr, ਪੂਰਨ boolean)
{
  VTerm_ਮੁੱਲ val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

ਸਥਿਰ ਖਾਲੀ setpenattr_int(VTermState *state, VTermAttr attr, ਪੂਰਨ ਸੰਖਿਆ)
{
  VTerm_ਮੁੱਲ val;
  val.ਸੰਖਿਆ = ਸੰਖਿਆ;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

ਸਥਿਰ ਖਾਲੀ setpenattr_ਖਾਨਾ(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_ਮੁੱਲ val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_pen_ਖਾਨਾ_ansi(VTermState *state, VTermAttr attr, ਲੰਮਾ ਖਾਨਾ)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_ਰੰਗ_ansi(state, ਖਾਨਾ, colp);

  setpenattr_ਖਾਨਾ(state, attr, *colp);
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_ਸੈੱਟ_default_colors(state, &state->default_fg, &state->default_bg);

  ਲਈ(ਪੂਰਨ ਖਾਨਾ = 0; ਖਾਨਾ < 16; ਖਾਨਾ++)
    lookup_default_ਰੰਗ_ansi(ਖਾਨਾ, &state->colors[ਖਾਨਾ]);
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_state_resetpen(VTermState *state)
{
  state->pen.bold = 0;      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
  state->pen.underline = 0; setpenattr_int (state, VTERM_ATTR_UNDERLINE, 0);
  state->pen.italic = 0;    setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
  state->pen.blink = 0;     setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
  state->pen.ਉਲਟ = 0;   setpenattr_bool(state, VTERM_ATTR_ਉਲਟ, 0);
  state->pen.conceal = 0;   setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
  state->pen.strike = 0;    setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
  state->pen.font = 0;      setpenattr_int (state, VTERM_ATTR_FONT, 0);
  state->pen.small = 0;     setpenattr_bool(state, VTERM_ATTR_SMALL, 0);
  state->pen.baseline = 0;  setpenattr_int (state, VTERM_ATTR_BASELINE, 0);

  state->pen.fg = state->default_fg;  setpenattr_ਖਾਨਾ(state, VTERM_ATTR_FOREGROUND, state->default_fg);
  state->pen.bg = state->default_bg;  setpenattr_ਖਾਨਾ(state, VTERM_ATTR_BACKGROUND, state->default_bg);
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_state_savepen(VTermState *state, ਪੂਰਨ save)
{
  ਜੇ(save) {
    state->saved.pen = state->pen;
  }
  ਨਹੀਂਤਾਂ {
    state->pen = state->saved.pen;

    setpenattr_bool(state, VTERM_ATTR_BOLD,      state->pen.bold);
    setpenattr_int (state, VTERM_ATTR_UNDERLINE, state->pen.underline);
    setpenattr_bool(state, VTERM_ATTR_ITALIC,    state->pen.italic);
    setpenattr_bool(state, VTERM_ATTR_BLINK,     state->pen.blink);
    setpenattr_bool(state, VTERM_ATTR_ਉਲਟ,   state->pen.ਉਲਟ);
    setpenattr_bool(state, VTERM_ATTR_CONCEAL,   state->pen.conceal);
    setpenattr_bool(state, VTERM_ATTR_STRIKE,    state->pen.strike);
    setpenattr_int (state, VTERM_ATTR_FONT,      state->pen.font);
    setpenattr_bool(state, VTERM_ATTR_SMALL,     state->pen.small);
    setpenattr_int (state, VTERM_ATTR_BASELINE,  state->pen.baseline);

    setpenattr_ਖਾਨਾ( state, VTERM_ATTR_FOREGROUND, state->pen.fg);
    setpenattr_ਖਾਨਾ( state, VTERM_ATTR_BACKGROUND, state->pen.bg);
  }
}

ਖਾਲੀ vterm_color_rgb(VTermColor *ਖਾਨਾ, uint8_t red, uint8_t green, uint8_t blue)
{
  ਖਾਨਾ->type = VTERM_COLOR_RGB;
  ਖਾਨਾ->red   = red;
  ਖਾਨਾ->green = green;
  ਖਾਨਾ->blue  = blue;
}

ਖਾਲੀ vterm_color_ਸੂਚਕਾਂਕਿਤ(VTermColor *ਖਾਨਾ, uint8_t idx)
{
  ਖਾਨਾ->type = VTERM_COLOR_ਸੂਚਕਾਂਕਿਤ;
  ਖਾਨਾ->ਸੂਚਕਾਂਕ = idx;
}

ਪੂਰਨ vterm_color_is_equal(ਅਚਲ VTermColor *a, ਅਚਲ VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  ਜੇ (a->type != b->type) {
    ਵਾਪਸ FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  ਜੇ (VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(a)) {
    ਵਾਪਸ a->ਸੂਚਕਾਂਕ == b->ਸੂਚਕਾਂਕ;
  }
  ਨਹੀਂਤਾਂ ਜੇ (VTERM_COLOR_IS_RGB(a)) {
    ਵਾਪਸ    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  ਵਾਪਸ 0;
}

ਖਾਲੀ vterm_state_get_default_colors(ਅਚਲ VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

ਖਾਲੀ vterm_state_get_palette_color(ਅਚਲ VTermState *state, ਪੂਰਨ ਸੂਚਕਾਂਕ, VTermColor *ਖਾਨਾ)
{
  lookup_ਰੰਗ_palette(state, ਸੂਚਕਾਂਕ, ਖਾਨਾ);
}

ਖਾਲੀ vterm_state_ਸੈੱਟ_default_colors(VTermState *state, ਅਚਲ VTermColor *default_fg, ਅਚਲ VTermColor *default_bg)
{
  ਜੇ(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_ਨਕਾਬ)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  ਜੇ(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_ਨਕਾਬ)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

ਖਾਲੀ vterm_state_ਸੈੱਟ_palette_color(VTermState *state, ਪੂਰਨ ਸੂਚਕਾਂਕ, ਅਚਲ VTermColor *ਖਾਨਾ)
{
  ਜੇ(ਸੂਚਕਾਂਕ >= 0 && ਸੂਚਕਾਂਕ < 16)
    state->colors[ਸੂਚਕਾਂਕ] = *ਖਾਨਾ;
}

ਖਾਲੀ vterm_state_convert_color_ਮੰਜ਼ਿਲ_rgb(ਅਚਲ VTermState *state, VTermColor *ਖਾਨਾ)
{
  ਜੇ (VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(ਖਾਨਾ)) { /* Convert indexed colors to RGB */
    lookup_ਰੰਗ_palette(state, ਖਾਨਾ->ਸੂਚਕਾਂਕ, ਖਾਨਾ);
  }
  ਖਾਨਾ->type &= VTERM_COLOR_TYPE_ਨਕਾਬ; /* Reset any metadata but the type */
}

ਖਾਲੀ vterm_state_ਸੈੱਟ_bold_highbright(VTermState *state, ਪੂਰਨ bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_state_setpen(VTermState *state, ਅਚਲ ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount)
{
  // SGR - ECMA-48 8.3.117

  ਪੂਰਨ argi = 0;
  ਪੂਰਨ ਮੁੱਲ;

  ਜਦੋਂ(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    ਪੂਰਨ ਪੂਰਾ = 1;

    ਲੰਮਾ arg;
    ਚੋਣ(arg = CSI_ARG(ਦਲੀਲਾਂ[argi])) {
    ਹਾਲਤ CSI_ARG_MISSING:
    ਹਾਲਤ 0: // Reset
      vterm_state_resetpen(state);
      ਰੋਕੋ;

    ਹਾਲਤ 1: { // Bold on
      ਅਚਲ VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      ਜੇ(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(fg) && fg->ਸੂਚਕਾਂਕ < 8 && state->bold_is_highbright)
        ਸੈੱਟ_pen_ਖਾਨਾ_ansi(state, VTERM_ATTR_FOREGROUND, fg->ਸੂਚਕਾਂਕ + (state->pen.bold ? 8 : 0));
      ਰੋਕੋ;
    }

    ਹਾਲਤ 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      ਰੋਕੋ;

    ਹਾਲਤ 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      ਜੇ(CSI_ARG_HAS_ਹੋਰ(ਦਲੀਲਾਂ[argi])) {
        argi++;
        ਚੋਣ(CSI_ARG(ਦਲੀਲਾਂ[argi])) {
          ਹਾਲਤ 0:
            state->pen.underline = 0;
            ਰੋਕੋ;
          ਹਾਲਤ 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            ਰੋਕੋ;
          ਹਾਲਤ 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            ਰੋਕੋ;
          ਹਾਲਤ 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            ਰੋਕੋ;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ਰੋਕੋ;

    ਹਾਲਤ 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      ਰੋਕੋ;

    ਹਾਲਤ 7: // Reverse on
      state->pen.ਉਲਟ = 1;
      setpenattr_bool(state, VTERM_ATTR_ਉਲਟ, 1);
      ਰੋਕੋ;

    ਹਾਲਤ 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      ਰੋਕੋ;

    ਹਾਲਤ 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      ਰੋਕੋ;

    ਹਾਲਤ 10: ਹਾਲਤ 11: ਹਾਲਤ 12: ਹਾਲਤ 13: ਹਾਲਤ 14:
    ਹਾਲਤ 15: ਹਾਲਤ 16: ਹਾਲਤ 17: ਹਾਲਤ 18: ਹਾਲਤ 19: // Select font
      state->pen.font = CSI_ARG(ਦਲੀਲਾਂ[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      ਰੋਕੋ;

    ਹਾਲਤ 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ਰੋਕੋ;

    ਹਾਲਤ 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      ਰੋਕੋ;

    ਹਾਲਤ 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      ਰੋਕੋ;

    ਹਾਲਤ 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      ਰੋਕੋ;

    ਹਾਲਤ 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      ਰੋਕੋ;

    ਹਾਲਤ 27: // Reverse off
      state->pen.ਉਲਟ = 0;
      setpenattr_bool(state, VTERM_ATTR_ਉਲਟ, 0);
      ਰੋਕੋ;

    ਹਾਲਤ 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      ਰੋਕੋ;

    ਹਾਲਤ 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      ਰੋਕੋ;

    ਹਾਲਤ 30: ਹਾਲਤ 31: ਹਾਲਤ 32: ਹਾਲਤ 33:
    ਹਾਲਤ 34: ਹਾਲਤ 35: ਹਾਲਤ 36: ਹਾਲਤ 37: // Foreground colour palette
      ਮੁੱਲ = CSI_ARG(ਦਲੀਲਾਂ[argi]) - 30;
      ਜੇ(state->pen.bold && state->bold_is_highbright)
        ਮੁੱਲ += 8;
      ਸੈੱਟ_pen_ਖਾਨਾ_ansi(state, VTERM_ATTR_FOREGROUND, ਮੁੱਲ);
      ਰੋਕੋ;

    ਹਾਲਤ 38: // Foreground colour alternative palette
      ਜੇ(argcount - argi < 1)
        ਵਾਪਸ;
      argi += 1 + lookup_ਰੰਗ(state, CSI_ARG(ਦਲੀਲਾਂ[argi+1]), ਦਲੀਲਾਂ+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_ਖਾਨਾ(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ਰੋਕੋ;

    ਹਾਲਤ 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_ਖਾਨਾ(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ਰੋਕੋ;

    ਹਾਲਤ 40: ਹਾਲਤ 41: ਹਾਲਤ 42: ਹਾਲਤ 43:
    ਹਾਲਤ 44: ਹਾਲਤ 45: ਹਾਲਤ 46: ਹਾਲਤ 47: // Background colour palette
      ਮੁੱਲ = CSI_ARG(ਦਲੀਲਾਂ[argi]) - 40;
      ਸੈੱਟ_pen_ਖਾਨਾ_ansi(state, VTERM_ATTR_BACKGROUND, ਮੁੱਲ);
      ਰੋਕੋ;

    ਹਾਲਤ 48: // Background colour alternative palette
      ਜੇ(argcount - argi < 1)
        ਵਾਪਸ;
      argi += 1 + lookup_ਰੰਗ(state, CSI_ARG(ਦਲੀਲਾਂ[argi+1]), ਦਲੀਲਾਂ+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_ਖਾਨਾ(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ਰੋਕੋ;

    ਹਾਲਤ 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_ਖਾਨਾ(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ਰੋਕੋ;

    ਹਾਲਤ 73: // Superscript
    ਹਾਲਤ 74: // Subscript
    ਹਾਲਤ 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_ਹੇਠਲਾ :
                      VTERM_BASELINE_ਸਾਧਾਰਨ;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      ਰੋਕੋ;

    ਹਾਲਤ 90: ਹਾਲਤ 91: ਹਾਲਤ 92: ਹਾਲਤ 93:
    ਹਾਲਤ 94: ਹਾਲਤ 95: ਹਾਲਤ 96: ਹਾਲਤ 97: // Foreground colour high-intensity palette
      ਮੁੱਲ = CSI_ARG(ਦਲੀਲਾਂ[argi]) - 90 + 8;
      ਸੈੱਟ_pen_ਖਾਨਾ_ansi(state, VTERM_ATTR_FOREGROUND, ਮੁੱਲ);
      ਰੋਕੋ;

    ਹਾਲਤ 100: ਹਾਲਤ 101: ਹਾਲਤ 102: ਹਾਲਤ 103:
    ਹਾਲਤ 104: ਹਾਲਤ 105: ਹਾਲਤ 106: ਹਾਲਤ 107: // Background colour high-intensity palette
      ਮੁੱਲ = CSI_ARG(ਦਲੀਲਾਂ[argi]) - 100 + 8;
      ਸੈੱਟ_pen_ਖਾਨਾ_ansi(state, VTERM_ATTR_BACKGROUND, ਮੁੱਲ);
      ਰੋਕੋ;

    ਮੂਲ:
      ਪੂਰਾ = 0;
      ਰੋਕੋ;
    }

    ਜੇ(!ਪੂਰਾ)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    ਜਦੋਂ(CSI_ARG_HAS_ਹੋਰ(ਦਲੀਲਾਂ[argi++]))
      ;
  }
}

ਸਥਿਰ ਪੂਰਨ vterm_state_getpen_color(ਅਚਲ VTermColor *ਖਾਨਾ, ਪੂਰਨ argi, ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ fg)
{
    /* Do nothing if the given color is the default color */
    ਜੇ (( fg && VTERM_COLOR_IS_DEFAULT_FG(ਖਾਨਾ)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(ਖਾਨਾ))) {
        ਵਾਪਸ argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    ਜੇ (VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(ਖਾਨਾ)) {
        ਅਚਲ uint8_t idx = ਖਾਨਾ->ਸੂਚਕਾਂਕ;
        ਜੇ (idx < 8) {
            ਦਲੀਲਾਂ[argi++] = (idx + (fg ? 30 : 40));
        }
        ਨਹੀਂਤਾਂ ਜੇ (idx < 16) {
            ਦਲੀਲਾਂ[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        ਨਹੀਂਤਾਂ {
            ਦਲੀਲਾਂ[argi++] = CSI_ARG_FLAG_ਹੋਰ | (fg ? 38 : 48);
            ਦਲੀਲਾਂ[argi++] = CSI_ARG_FLAG_ਹੋਰ | 5;
            ਦਲੀਲਾਂ[argi++] = idx;
        }
    }
    ਨਹੀਂਤਾਂ ਜੇ (VTERM_COLOR_IS_RGB(ਖਾਨਾ)) {
        ਦਲੀਲਾਂ[argi++] = CSI_ARG_FLAG_ਹੋਰ | (fg ? 38 : 48);
        ਦਲੀਲਾਂ[argi++] = CSI_ARG_FLAG_ਹੋਰ | 2;
        ਦਲੀਲਾਂ[argi++] = CSI_ARG_FLAG_ਹੋਰ | ਖਾਨਾ->red;
        ਦਲੀਲਾਂ[argi++] = CSI_ARG_FLAG_ਹੋਰ | ਖਾਨਾ->green;
        ਦਲੀਲਾਂ[argi++] = ਖਾਨਾ->blue;
    }
    ਵਾਪਸ argi;
}

ਅੰਦਰੂਨੀ ਪੂਰਨ vterm_state_getpen(VTermState *state, ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount ਅਣਵਰਤਿਆ)
{
  ਪੂਰਨ argi = 0;

  ਜੇ(state->pen.bold)
    ਦਲੀਲਾਂ[argi++] = 1;

  ਜੇ(state->pen.italic)
    ਦਲੀਲਾਂ[argi++] = 3;

  ਜੇ(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    ਦਲੀਲਾਂ[argi++] = 4;
  ਜੇ(state->pen.underline == VTERM_UNDERLINE_CURLY)
    ਦਲੀਲਾਂ[argi++] = 4 | CSI_ARG_FLAG_ਹੋਰ, ਦਲੀਲਾਂ[argi++] = 3;

  ਜੇ(state->pen.blink)
    ਦਲੀਲਾਂ[argi++] = 5;

  ਜੇ(state->pen.ਉਲਟ)
    ਦਲੀਲਾਂ[argi++] = 7;

  ਜੇ(state->pen.conceal)
    ਦਲੀਲਾਂ[argi++] = 8;

  ਜੇ(state->pen.strike)
    ਦਲੀਲਾਂ[argi++] = 9;

  ਜੇ(state->pen.font)
    ਦਲੀਲਾਂ[argi++] = 10 + state->pen.font;

  ਜੇ(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    ਦਲੀਲਾਂ[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, ਦਲੀਲਾਂ, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, ਦਲੀਲਾਂ, FALSE);

  ਜੇ(state->pen.small) {
    ਜੇ(state->pen.baseline == VTERM_BASELINE_RAISE)
      ਦਲੀਲਾਂ[argi++] = 73;
    ਨਹੀਂਤਾਂ ਜੇ(state->pen.baseline == VTERM_BASELINE_ਹੇਠਲਾ)
      ਦਲੀਲਾਂ[argi++] = 74;
  }

  ਵਾਪਸ argi;
}

ਪੂਰਨ vterm_state_get_penattr(ਅਚਲ VTermState *state, VTermAttr attr, VTerm_ਮੁੱਲ *val)
{
  ਚੋਣ(attr) {
  ਹਾਲਤ VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_UNDERLINE:
    val->ਸੰਖਿਆ = state->pen.underline;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_ਉਲਟ:
    val->boolean = state->pen.ਉਲਟ;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_FONT:
    val->ਸੰਖਿਆ = state->pen.font;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_ATTR_BASELINE:
    val->ਸੰਖਿਆ = state->pen.baseline;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_N_ATTRS:
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ 0;
}
