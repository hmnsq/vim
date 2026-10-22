#അന്തര്ഭാവ "vterm_internal.h"

#അന്തര്ഭാവ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
തരനിർവചനം ഘടന {
  uint8_t red, green, blue;
} VTermRGB;

സ്ഥിതം സ്ഥിരം VTermRGB ansi_colors[] = {
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

സ്ഥിതം പൂർണ്ണസംഖ്യ ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
സ്ഥിതം പൂർണ്ണസംഖ്യ ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

സ്ഥിതം ശൂന്യം lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ lookup_colour_ansi(സ്ഥിരം VTermState *state, long index, VTermColor *col)
{
  എങ്കിൽ(index >= 0 && index < 16) {
    *col = state->colors[index];
    തിരികെ TRUE;
  }

  തിരികെ FALSE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ lookup_colour_palette(സ്ഥിരം VTermState *state, long index, VTermColor *col)
{
  എങ്കിൽ(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    തിരികെ lookup_colour_ansi(state, index, col);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    തിരികെ TRUE;
  }

  തിരികെ FALSE;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ lookup_colour(സ്ഥിരം VTermState *state, പൂർണ്ണസംഖ്യ palette, സ്ഥിരം long args[], പൂർണ്ണസംഖ്യ argcount, VTermColor *col)
{
  തിരഞ്ഞെടുപ്പ്(palette) {
  അവസ്ഥ 2: // RGB mode - 3 args contain colour values directly
    എങ്കിൽ(argcount < 3)
      തിരികെ argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    തിരികെ 3;

  അവസ്ഥ 5: // XTerm 256-colour mode
    എങ്കിൽ (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      തിരികെ argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    തിരികെ 1;

  സ്ഥിരസ്ഥിതി:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    തിരികെ 0;
  }
}

// Some conveniences

സ്ഥിതം ശൂന്യം setpenattr(VTermState *state, VTermAttr attr, VTerm_മൂല്യം_Type type UNUSED, VTerm_മൂല്യം *val)
{
#ifdef DEBUG
  എങ്കിൽ(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    തിരികെ;
  }
#endif
  എങ്കിൽ(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

സ്ഥിതം ശൂന്യം setpenattr_bool(VTermState *state, VTermAttr attr, പൂർണ്ണസംഖ്യ boolean)
{
  VTerm_മൂല്യം val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

സ്ഥിതം ശൂന്യം setpenattr_int(VTermState *state, VTermAttr attr, പൂർണ്ണസംഖ്യ സംഖ്യ)
{
  VTerm_മൂല്യം val;
  val.സംഖ്യ = സംഖ്യ;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

സ്ഥിതം ശൂന്യം setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_മൂല്യം val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

സ്ഥിതം ശൂന്യം set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL ശൂന്യം vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  വേണ്ടി(പൂർണ്ണസംഖ്യ col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL ശൂന്യം vterm_state_resetpen(VTermState *state)
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

INTERNAL ശൂന്യം vterm_state_savepen(VTermState *state, പൂർണ്ണസംഖ്യ save)
{
  എങ്കിൽ(save) {
    state->saved.pen = state->pen;
  }
  അല്ലെങ്കിൽ {
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

ശൂന്യം vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

ശൂന്യം vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

പൂർണ്ണസംഖ്യ vterm_color_is_equal(സ്ഥിരം VTermColor *a, സ്ഥിരം VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  എങ്കിൽ (a->type != b->type) {
    തിരികെ FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  എങ്കിൽ (VTERM_COLOR_IS_INDEXED(a)) {
    തിരികെ a->index == b->index;
  }
  അല്ലെങ്കിൽ എങ്കിൽ (VTERM_COLOR_IS_RGB(a)) {
    തിരികെ    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  തിരികെ 0;
}

ശൂന്യം vterm_state_get_default_colors(സ്ഥിരം VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

ശൂന്യം vterm_state_get_palette_color(സ്ഥിരം VTermState *state, പൂർണ്ണസംഖ്യ index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

ശൂന്യം vterm_state_set_default_colors(VTermState *state, സ്ഥിരം VTermColor *default_fg, സ്ഥിരം VTermColor *default_bg)
{
  എങ്കിൽ(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  എങ്കിൽ(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

ശൂന്യം vterm_state_set_palette_color(VTermState *state, പൂർണ്ണസംഖ്യ index, സ്ഥിരം VTermColor *col)
{
  എങ്കിൽ(index >= 0 && index < 16)
    state->colors[index] = *col;
}

ശൂന്യം vterm_state_convert_color_to_rgb(സ്ഥിരം VTermState *state, VTermColor *col)
{
  എങ്കിൽ (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

ശൂന്യം vterm_state_set_bold_highbright(VTermState *state, പൂർണ്ണസംഖ്യ bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL ശൂന്യം vterm_state_setpen(VTermState *state, സ്ഥിരം long args[], പൂർണ്ണസംഖ്യ argcount)
{
  // SGR - ECMA-48 8.3.117

  പൂർണ്ണസംഖ്യ argi = 0;
  പൂർണ്ണസംഖ്യ മൂല്യം;

  ആകുമ്പോൾ(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    പൂർണ്ണസംഖ്യ done = 1;

    long arg;
    തിരഞ്ഞെടുപ്പ്(arg = CSI_ARG(args[argi])) {
    അവസ്ഥ CSI_ARG_MISSING:
    അവസ്ഥ 0: // Reset
      vterm_state_resetpen(state);
      നിർത്തുക;

    അവസ്ഥ 1: { // Bold on
      സ്ഥിരം VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      എങ്കിൽ(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      നിർത്തുക;
    }

    അവസ്ഥ 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      നിർത്തുക;

    അവസ്ഥ 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      എങ്കിൽ(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        തിരഞ്ഞെടുപ്പ്(CSI_ARG(args[argi])) {
          അവസ്ഥ 0:
            state->pen.underline = 0;
            നിർത്തുക;
          അവസ്ഥ 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            നിർത്തുക;
          അവസ്ഥ 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            നിർത്തുക;
          അവസ്ഥ 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            നിർത്തുക;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      നിർത്തുക;

    അവസ്ഥ 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      നിർത്തുക;

    അവസ്ഥ 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      നിർത്തുക;

    അവസ്ഥ 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      നിർത്തുക;

    അവസ്ഥ 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      നിർത്തുക;

    അവസ്ഥ 10: അവസ്ഥ 11: അവസ്ഥ 12: അവസ്ഥ 13: അവസ്ഥ 14:
    അവസ്ഥ 15: അവസ്ഥ 16: അവസ്ഥ 17: അവസ്ഥ 18: അവസ്ഥ 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      നിർത്തുക;

    അവസ്ഥ 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      നിർത്തുക;

    അവസ്ഥ 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      നിർത്തുക;

    അവസ്ഥ 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      നിർത്തുക;

    അവസ്ഥ 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      നിർത്തുക;

    അവസ്ഥ 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      നിർത്തുക;

    അവസ്ഥ 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      നിർത്തുക;

    അവസ്ഥ 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      നിർത്തുക;

    അവസ്ഥ 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      നിർത്തുക;

    അവസ്ഥ 30: അവസ്ഥ 31: അവസ്ഥ 32: അവസ്ഥ 33:
    അവസ്ഥ 34: അവസ്ഥ 35: അവസ്ഥ 36: അവസ്ഥ 37: // Foreground colour palette
      മൂല്യം = CSI_ARG(args[argi]) - 30;
      എങ്കിൽ(state->pen.bold && state->bold_is_highbright)
        മൂല്യം += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, മൂല്യം);
      നിർത്തുക;

    അവസ്ഥ 38: // Foreground colour alternative palette
      എങ്കിൽ(argcount - argi < 1)
        തിരികെ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      നിർത്തുക;

    അവസ്ഥ 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      നിർത്തുക;

    അവസ്ഥ 40: അവസ്ഥ 41: അവസ്ഥ 42: അവസ്ഥ 43:
    അവസ്ഥ 44: അവസ്ഥ 45: അവസ്ഥ 46: അവസ്ഥ 47: // Background colour palette
      മൂല്യം = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, മൂല്യം);
      നിർത്തുക;

    അവസ്ഥ 48: // Background colour alternative palette
      എങ്കിൽ(argcount - argi < 1)
        തിരികെ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      നിർത്തുക;

    അവസ്ഥ 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      നിർത്തുക;

    അവസ്ഥ 73: // Superscript
    അവസ്ഥ 74: // Subscript
    അവസ്ഥ 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      നിർത്തുക;

    അവസ്ഥ 90: അവസ്ഥ 91: അവസ്ഥ 92: അവസ്ഥ 93:
    അവസ്ഥ 94: അവസ്ഥ 95: അവസ്ഥ 96: അവസ്ഥ 97: // Foreground colour high-intensity palette
      മൂല്യം = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, മൂല്യം);
      നിർത്തുക;

    അവസ്ഥ 100: അവസ്ഥ 101: അവസ്ഥ 102: അവസ്ഥ 103:
    അവസ്ഥ 104: അവസ്ഥ 105: അവസ്ഥ 106: അവസ്ഥ 107: // Background colour high-intensity palette
      മൂല്യം = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, മൂല്യം);
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      done = 0;
      നിർത്തുക;
    }

    എങ്കിൽ(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    ആകുമ്പോൾ(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ vterm_state_getpen_color(സ്ഥിരം VTermColor *col, പൂർണ്ണസംഖ്യ argi, long args[], പൂർണ്ണസംഖ്യ fg)
{
    /* Do nothing if the given color is the default color */
    എങ്കിൽ (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        തിരികെ argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    എങ്കിൽ (VTERM_COLOR_IS_INDEXED(col)) {
        സ്ഥിരം uint8_t idx = col->index;
        എങ്കിൽ (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        അല്ലെങ്കിൽ എങ്കിൽ (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        അല്ലെങ്കിൽ {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    അല്ലെങ്കിൽ എങ്കിൽ (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    തിരികെ argi;
}

INTERNAL പൂർണ്ണസംഖ്യ vterm_state_getpen(VTermState *state, long args[], പൂർണ്ണസംഖ്യ argcount UNUSED)
{
  പൂർണ്ണസംഖ്യ argi = 0;

  എങ്കിൽ(state->pen.bold)
    args[argi++] = 1;

  എങ്കിൽ(state->pen.italic)
    args[argi++] = 3;

  എങ്കിൽ(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  എങ്കിൽ(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  എങ്കിൽ(state->pen.blink)
    args[argi++] = 5;

  എങ്കിൽ(state->pen.reverse)
    args[argi++] = 7;

  എങ്കിൽ(state->pen.conceal)
    args[argi++] = 8;

  എങ്കിൽ(state->pen.strike)
    args[argi++] = 9;

  എങ്കിൽ(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  എങ്കിൽ(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  എങ്കിൽ(state->pen.small) {
    എങ്കിൽ(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    അല്ലെങ്കിൽ എങ്കിൽ(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  തിരികെ argi;
}

പൂർണ്ണസംഖ്യ vterm_state_get_penattr(സ്ഥിരം VTermState *state, VTermAttr attr, VTerm_മൂല്യം *val)
{
  തിരഞ്ഞെടുപ്പ്(attr) {
  അവസ്ഥ VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_UNDERLINE:
    val->സംഖ്യ = state->pen.underline;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_FONT:
    val->സംഖ്യ = state->pen.font;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    തിരികെ 1;

  അവസ്ഥ VTERM_ATTR_BASELINE:
    val->സംഖ്യ = state->pen.baseline;
    തിരികെ 1;

  അവസ്ഥ VTERM_N_ATTRS:
    തിരികെ 0;
  }

  തിരികെ 0;
}
