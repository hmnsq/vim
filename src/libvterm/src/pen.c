#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  uint8_t red, green, blue;
} VTermRGB;

ಸ್ಥಿರ ಧ್ರುವ VTermRGB ansi_colors[] = {
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

ಸ್ಥಿರ ಶೂನ್ಯ lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lookup_colour_ansi(ಧ್ರುವ VTermState *state, long index, VTermColor *col)
{
  ಯದಿ(index >= 0 && index < 16) {
    *col = state->colors[index];
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lookup_colour_palette(ಧ್ರುವ VTermState *state, long index, VTermColor *col)
{
  ಯದಿ(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    ಪ್ರತ್ಯಾಗಮ lookup_colour_ansi(state, index, col);
  }
  ಅನ್ಯಥಾ ಯದಿ(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lookup_colour(ಧ್ರುವ VTermState *state, ಪೂರ್ಣಾಙ್ಕ palette, ಧ್ರುವ long args[], ಪೂರ್ಣಾಙ್ಕ argcount, VTermColor *col)
{
  ವಿಕಲ್ಪ(palette) {
  ಪಕ್ಷ 2: // RGB mode - 3 args contain colour values directly
    ಯದಿ(argcount < 3)
      ಪ್ರತ್ಯಾಗಮ argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    ಪ್ರತ್ಯಾಗಮ 3;

  ಪಕ್ಷ 5: // XTerm 256-colour mode
    ಯದಿ (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      ಪ್ರತ್ಯಾಗಮ argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    ಪ್ರತ್ಯಾಗಮ 1;

  ಸ್ವಭಾವ:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    ಪ್ರತ್ಯಾಗಮ 0;
  }
}

// Some conveniences

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr(VTermState *state, VTermAttr attr, VTerm_ಮೌಲ್ಯ_Type type UNUSED, VTerm_ಮೌಲ್ಯ *val)
{
#ifdef DEBUG
  ಯದಿ(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    ಪ್ರತ್ಯಾಗಮ;
  }
#endif
  ಯದಿ(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr_bool(VTermState *state, VTermAttr attr, ಪೂರ್ಣಾಙ್ಕ boolean)
{
  VTerm_ಮೌಲ್ಯ val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr_int(VTermState *state, VTermAttr attr, ಪೂರ್ಣಾಙ್ಕ ಸಂಖ್ಯೆ)
{
  VTerm_ಮೌಲ್ಯ val;
  val.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_ಮೌಲ್ಯ val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL ಶೂನ್ಯ vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL ಶೂನ್ಯ vterm_state_resetpen(VTermState *state)
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

INTERNAL ಶೂನ್ಯ vterm_state_savepen(VTermState *state, ಪೂರ್ಣಾಙ್ಕ save)
{
  ಯದಿ(save) {
    state->saved.pen = state->pen;
  }
  ಅನ್ಯಥಾ {
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

ಶೂನ್ಯ vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

ಶೂನ್ಯ vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

ಪೂರ್ಣಾಙ್ಕ vterm_color_is_equal(ಧ್ರುವ VTermColor *a, ಧ್ರುವ VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  ಯದಿ (a->type != b->type) {
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  ಯದಿ (VTERM_COLOR_IS_INDEXED(a)) {
    ಪ್ರತ್ಯಾಗಮ a->index == b->index;
  }
  ಅನ್ಯಥಾ ಯದಿ (VTERM_COLOR_IS_RGB(a)) {
    ಪ್ರತ್ಯಾಗಮ    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vterm_state_get_default_colors(ಧ್ರುವ VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

ಶೂನ್ಯ vterm_state_get_palette_color(ಧ್ರುವ VTermState *state, ಪೂರ್ಣಾಙ್ಕ index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

ಶೂನ್ಯ vterm_state_set_default_colors(VTermState *state, ಧ್ರುವ VTermColor *default_fg, ಧ್ರುವ VTermColor *default_bg)
{
  ಯದಿ(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  ಯದಿ(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

ಶೂನ್ಯ vterm_state_set_palette_color(VTermState *state, ಪೂರ್ಣಾಙ್ಕ index, ಧ್ರುವ VTermColor *col)
{
  ಯದಿ(index >= 0 && index < 16)
    state->colors[index] = *col;
}

ಶೂನ್ಯ vterm_state_convert_color_to_rgb(ಧ್ರುವ VTermState *state, VTermColor *col)
{
  ಯದಿ (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

ಶೂನ್ಯ vterm_state_set_bold_highbright(VTermState *state, ಪೂರ್ಣಾಙ್ಕ bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL ಶೂನ್ಯ vterm_state_setpen(VTermState *state, ಧ್ರುವ long args[], ಪೂರ್ಣಾಙ್ಕ argcount)
{
  // SGR - ECMA-48 8.3.117

  ಪೂರ್ಣಾಙ್ಕ argi = 0;
  ಪೂರ್ಣಾಙ್ಕ ಮೌಲ್ಯ;

  ಯಾವತ್(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    ಪೂರ್ಣಾಙ್ಕ done = 1;

    long arg;
    ವಿಕಲ್ಪ(arg = CSI_ARG(args[argi])) {
    ಪಕ್ಷ CSI_ARG_MISSING:
    ಪಕ್ಷ 0: // Reset
      vterm_state_resetpen(state);
      ಭಙ್ಗ;

    ಪಕ್ಷ 1: { // Bold on
      ಧ್ರುವ VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      ಯದಿ(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      ಭಙ್ಗ;
    }

    ಪಕ್ಷ 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      ಭಙ್ಗ;

    ಪಕ್ಷ 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      ಯದಿ(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        ವಿಕಲ್ಪ(CSI_ARG(args[argi])) {
          ಪಕ್ಷ 0:
            state->pen.underline = 0;
            ಭಙ್ಗ;
          ಪಕ್ಷ 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            ಭಙ್ಗ;
          ಪಕ್ಷ 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            ಭಙ್ಗ;
          ಪಕ್ಷ 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            ಭಙ್ಗ;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ಭಙ್ಗ;

    ಪಕ್ಷ 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      ಭಙ್ಗ;

    ಪಕ್ಷ 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      ಭಙ್ಗ;

    ಪಕ್ಷ 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      ಭಙ್ಗ;

    ಪಕ್ಷ 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      ಭಙ್ಗ;

    ಪಕ್ಷ 10: ಪಕ್ಷ 11: ಪಕ್ಷ 12: ಪಕ್ಷ 13: ಪಕ್ಷ 14:
    ಪಕ್ಷ 15: ಪಕ್ಷ 16: ಪಕ್ಷ 17: ಪಕ್ಷ 18: ಪಕ್ಷ 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      ಭಙ್ಗ;

    ಪಕ್ಷ 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ಭಙ್ಗ;

    ಪಕ್ಷ 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      ಭಙ್ಗ;

    ಪಕ್ಷ 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      ಭಙ್ಗ;

    ಪಕ್ಷ 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      ಭಙ್ಗ;

    ಪಕ್ಷ 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      ಭಙ್ಗ;

    ಪಕ್ಷ 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      ಭಙ್ಗ;

    ಪಕ್ಷ 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      ಭಙ್ಗ;

    ಪಕ್ಷ 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      ಭಙ್ಗ;

    ಪಕ್ಷ 30: ಪಕ್ಷ 31: ಪಕ್ಷ 32: ಪಕ್ಷ 33:
    ಪಕ್ಷ 34: ಪಕ್ಷ 35: ಪಕ್ಷ 36: ಪಕ್ಷ 37: // Foreground colour palette
      ಮೌಲ್ಯ = CSI_ARG(args[argi]) - 30;
      ಯದಿ(state->pen.bold && state->bold_is_highbright)
        ಮೌಲ್ಯ += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, ಮೌಲ್ಯ);
      ಭಙ್ಗ;

    ಪಕ್ಷ 38: // Foreground colour alternative palette
      ಯದಿ(argcount - argi < 1)
        ಪ್ರತ್ಯಾಗಮ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ಭಙ್ಗ;

    ಪಕ್ಷ 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ಭಙ್ಗ;

    ಪಕ್ಷ 40: ಪಕ್ಷ 41: ಪಕ್ಷ 42: ಪಕ್ಷ 43:
    ಪಕ್ಷ 44: ಪಕ್ಷ 45: ಪಕ್ಷ 46: ಪಕ್ಷ 47: // Background colour palette
      ಮೌಲ್ಯ = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, ಮೌಲ್ಯ);
      ಭಙ್ಗ;

    ಪಕ್ಷ 48: // Background colour alternative palette
      ಯದಿ(argcount - argi < 1)
        ಪ್ರತ್ಯಾಗಮ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ಭಙ್ಗ;

    ಪಕ್ಷ 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ಭಙ್ಗ;

    ಪಕ್ಷ 73: // Superscript
    ಪಕ್ಷ 74: // Subscript
    ಪಕ್ಷ 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      ಭಙ್ಗ;

    ಪಕ್ಷ 90: ಪಕ್ಷ 91: ಪಕ್ಷ 92: ಪಕ್ಷ 93:
    ಪಕ್ಷ 94: ಪಕ್ಷ 95: ಪಕ್ಷ 96: ಪಕ್ಷ 97: // Foreground colour high-intensity palette
      ಮೌಲ್ಯ = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, ಮೌಲ್ಯ);
      ಭಙ್ಗ;

    ಪಕ್ಷ 100: ಪಕ್ಷ 101: ಪಕ್ಷ 102: ಪಕ್ಷ 103:
    ಪಕ್ಷ 104: ಪಕ್ಷ 105: ಪಕ್ಷ 106: ಪಕ್ಷ 107: // Background colour high-intensity palette
      ಮೌಲ್ಯ = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, ಮೌಲ್ಯ);
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      done = 0;
      ಭಙ್ಗ;
    }

    ಯದಿ(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    ಯಾವತ್(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vterm_state_getpen_color(ಧ್ರುವ VTermColor *col, ಪೂರ್ಣಾಙ್ಕ argi, long args[], ಪೂರ್ಣಾಙ್ಕ fg)
{
    /* Do nothing if the given color is the default color */
    ಯದಿ (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        ಪ್ರತ್ಯಾಗಮ argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    ಯದಿ (VTERM_COLOR_IS_INDEXED(col)) {
        ಧ್ರುವ uint8_t idx = col->index;
        ಯದಿ (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        ಅನ್ಯಥಾ ಯದಿ (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        ಅನ್ಯಥಾ {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    ಅನ್ಯಥಾ ಯದಿ (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    ಪ್ರತ್ಯಾಗಮ argi;
}

INTERNAL ಪೂರ್ಣಾಙ್ಕ vterm_state_getpen(VTermState *state, long args[], ಪೂರ್ಣಾಙ್ಕ argcount UNUSED)
{
  ಪೂರ್ಣಾಙ್ಕ argi = 0;

  ಯದಿ(state->pen.bold)
    args[argi++] = 1;

  ಯದಿ(state->pen.italic)
    args[argi++] = 3;

  ಯದಿ(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  ಯದಿ(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  ಯದಿ(state->pen.blink)
    args[argi++] = 5;

  ಯದಿ(state->pen.reverse)
    args[argi++] = 7;

  ಯದಿ(state->pen.conceal)
    args[argi++] = 8;

  ಯದಿ(state->pen.strike)
    args[argi++] = 9;

  ಯದಿ(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  ಯದಿ(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  ಯದಿ(state->pen.small) {
    ಯದಿ(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    ಅನ್ಯಥಾ ಯದಿ(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  ಪ್ರತ್ಯಾಗಮ argi;
}

ಪೂರ್ಣಾಙ್ಕ vterm_state_get_penattr(ಧ್ರುವ VTermState *state, VTermAttr attr, VTerm_ಮೌಲ್ಯ *val)
{
  ವಿಕಲ್ಪ(attr) {
  ಪಕ್ಷ VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_UNDERLINE:
    val->ಸಂಖ್ಯೆ = state->pen.underline;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_FONT:
    val->ಸಂಖ್ಯೆ = state->pen.font;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_ATTR_BASELINE:
    val->ಸಂಖ್ಯೆ = state->pen.baseline;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_N_ATTRS:
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}
