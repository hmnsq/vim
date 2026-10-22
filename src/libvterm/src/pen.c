#অন্তর্ভাৱ "vterm_internal.h"

#অন্তর্ভাৱ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
প্রকারনির্দেশ সংরচনা {
  uint8_t red, green, blue;
} VTermRGB;

স্থির ধ্রুৱ VTermRGB ansi_colors[] = {
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

স্থির পূর্ণাঙ্ক ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
স্থির পূর্ণাঙ্ক ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

স্থির শূন্য lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

স্থির পূর্ণাঙ্ক lookup_colour_ansi(ধ্রুৱ VTermState *state, long index, VTermColor *col)
{
  যদি(index >= 0 && index < 16) {
    *col = state->colors[index];
    ঘূৰাই TRUE;
  }

  ঘূৰাই FALSE;
}

স্থির পূর্ণাঙ্ক lookup_colour_palette(ধ্রুৱ VTermState *state, long index, VTermColor *col)
{
  যদি(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    ঘূৰাই lookup_colour_ansi(state, index, col);
  }
  অন্যথা যদি(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    ঘূৰাই TRUE;
  }
  অন্যথা যদি(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    ঘূৰাই TRUE;
  }

  ঘূৰাই FALSE;
}

স্থির পূর্ণাঙ্ক lookup_colour(ধ্রুৱ VTermState *state, পূর্ণাঙ্ক palette, ধ্রুৱ long args[], পূর্ণাঙ্ক argcount, VTermColor *col)
{
  ৱিকল্প(palette) {
  পক্ষ 2: // RGB mode - 3 args contain colour values directly
    যদি(argcount < 3)
      ঘূৰাই argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    ঘূৰাই 3;

  পক্ষ 5: // XTerm 256-colour mode
    যদি (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      ঘূৰাই argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    ঘূৰাই 1;

  স্ৱভাৱ:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    ঘূৰাই 0;
  }
}

// Some conveniences

স্থির শূন্য setpenattr(VTermState *state, VTermAttr attr, VTerm_মান_Type type UNUSED, VTerm_মান *val)
{
#ifdef DEBUG
  যদি(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    ঘূৰাই;
  }
#endif
  যদি(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

স্থির শূন্য setpenattr_bool(VTermState *state, VTermAttr attr, পূর্ণাঙ্ক boolean)
{
  VTerm_মান val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

স্থির শূন্য setpenattr_int(VTermState *state, VTermAttr attr, পূর্ণাঙ্ক সংখ্যা)
{
  VTerm_মান val;
  val.সংখ্যা = সংখ্যা;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

স্থির শূন্য setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_মান val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

স্থির শূন্য set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL শূন্য vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  বাবে(পূর্ণাঙ্ক col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL শূন্য vterm_state_resetpen(VTermState *state)
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

INTERNAL শূন্য vterm_state_savepen(VTermState *state, পূর্ণাঙ্ক save)
{
  যদি(save) {
    state->saved.pen = state->pen;
  }
  অন্যথা {
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

শূন্য vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

শূন্য vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

পূর্ণাঙ্ক vterm_color_is_equal(ধ্রুৱ VTermColor *a, ধ্রুৱ VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  যদি (a->type != b->type) {
    ঘূৰাই FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  যদি (VTERM_COLOR_IS_INDEXED(a)) {
    ঘূৰাই a->index == b->index;
  }
  অন্যথা যদি (VTERM_COLOR_IS_RGB(a)) {
    ঘূৰাই    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  ঘূৰাই 0;
}

শূন্য vterm_state_get_default_colors(ধ্রুৱ VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

শূন্য vterm_state_get_palette_color(ধ্রুৱ VTermState *state, পূর্ণাঙ্ক index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

শূন্য vterm_state_set_default_colors(VTermState *state, ধ্রুৱ VTermColor *default_fg, ধ্রুৱ VTermColor *default_bg)
{
  যদি(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  যদি(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

শূন্য vterm_state_set_palette_color(VTermState *state, পূর্ণাঙ্ক index, ধ্রুৱ VTermColor *col)
{
  যদি(index >= 0 && index < 16)
    state->colors[index] = *col;
}

শূন্য vterm_state_convert_color_to_rgb(ধ্রুৱ VTermState *state, VTermColor *col)
{
  যদি (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

শূন্য vterm_state_set_bold_highbright(VTermState *state, পূর্ণাঙ্ক bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL শূন্য vterm_state_setpen(VTermState *state, ধ্রুৱ long args[], পূর্ণাঙ্ক argcount)
{
  // SGR - ECMA-48 8.3.117

  পূর্ণাঙ্ক argi = 0;
  পূর্ণাঙ্ক মান;

  যেতিয়ালৈ(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    পূর্ণাঙ্ক done = 1;

    long arg;
    ৱিকল্প(arg = CSI_ARG(args[argi])) {
    পক্ষ CSI_ARG_MISSING:
    পক্ষ 0: // Reset
      vterm_state_resetpen(state);
      ভঙ্গ;

    পক্ষ 1: { // Bold on
      ধ্রুৱ VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      যদি(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      ভঙ্গ;
    }

    পক্ষ 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      ভঙ্গ;

    পক্ষ 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      যদি(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        ৱিকল্প(CSI_ARG(args[argi])) {
          পক্ষ 0:
            state->pen.underline = 0;
            ভঙ্গ;
          পক্ষ 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            ভঙ্গ;
          পক্ষ 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            ভঙ্গ;
          পক্ষ 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            ভঙ্গ;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ভঙ্গ;

    পক্ষ 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      ভঙ্গ;

    পক্ষ 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      ভঙ্গ;

    পক্ষ 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      ভঙ্গ;

    পক্ষ 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      ভঙ্গ;

    পক্ষ 10: পক্ষ 11: পক্ষ 12: পক্ষ 13: পক্ষ 14:
    পক্ষ 15: পক্ষ 16: পক্ষ 17: পক্ষ 18: পক্ষ 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      ভঙ্গ;

    পক্ষ 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ভঙ্গ;

    পক্ষ 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      ভঙ্গ;

    পক্ষ 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      ভঙ্গ;

    পক্ষ 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      ভঙ্গ;

    পক্ষ 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      ভঙ্গ;

    পক্ষ 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      ভঙ্গ;

    পক্ষ 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      ভঙ্গ;

    পক্ষ 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      ভঙ্গ;

    পক্ষ 30: পক্ষ 31: পক্ষ 32: পক্ষ 33:
    পক্ষ 34: পক্ষ 35: পক্ষ 36: পক্ষ 37: // Foreground colour palette
      মান = CSI_ARG(args[argi]) - 30;
      যদি(state->pen.bold && state->bold_is_highbright)
        মান += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, মান);
      ভঙ্গ;

    পক্ষ 38: // Foreground colour alternative palette
      যদি(argcount - argi < 1)
        ঘূৰাই;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ভঙ্গ;

    পক্ষ 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ভঙ্গ;

    পক্ষ 40: পক্ষ 41: পক্ষ 42: পক্ষ 43:
    পক্ষ 44: পক্ষ 45: পক্ষ 46: পক্ষ 47: // Background colour palette
      মান = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, মান);
      ভঙ্গ;

    পক্ষ 48: // Background colour alternative palette
      যদি(argcount - argi < 1)
        ঘূৰাই;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ভঙ্গ;

    পক্ষ 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ভঙ্গ;

    পক্ষ 73: // Superscript
    পক্ষ 74: // Subscript
    পক্ষ 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      ভঙ্গ;

    পক্ষ 90: পক্ষ 91: পক্ষ 92: পক্ষ 93:
    পক্ষ 94: পক্ষ 95: পক্ষ 96: পক্ষ 97: // Foreground colour high-intensity palette
      মান = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, মান);
      ভঙ্গ;

    পক্ষ 100: পক্ষ 101: পক্ষ 102: পক্ষ 103:
    পক্ষ 104: পক্ষ 105: পক্ষ 106: পক্ষ 107: // Background colour high-intensity palette
      মান = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, মান);
      ভঙ্গ;

    স্ৱভাৱ:
      done = 0;
      ভঙ্গ;
    }

    যদি(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    যেতিয়ালৈ(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

স্থির পূর্ণাঙ্ক vterm_state_getpen_color(ধ্রুৱ VTermColor *col, পূর্ণাঙ্ক argi, long args[], পূর্ণাঙ্ক fg)
{
    /* Do nothing if the given color is the default color */
    যদি (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        ঘূৰাই argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    যদি (VTERM_COLOR_IS_INDEXED(col)) {
        ধ্রুৱ uint8_t idx = col->index;
        যদি (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        অন্যথা যদি (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        অন্যথা {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    অন্যথা যদি (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    ঘূৰাই argi;
}

INTERNAL পূর্ণাঙ্ক vterm_state_getpen(VTermState *state, long args[], পূর্ণাঙ্ক argcount UNUSED)
{
  পূর্ণাঙ্ক argi = 0;

  যদি(state->pen.bold)
    args[argi++] = 1;

  যদি(state->pen.italic)
    args[argi++] = 3;

  যদি(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  যদি(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  যদি(state->pen.blink)
    args[argi++] = 5;

  যদি(state->pen.reverse)
    args[argi++] = 7;

  যদি(state->pen.conceal)
    args[argi++] = 8;

  যদি(state->pen.strike)
    args[argi++] = 9;

  যদি(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  যদি(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  যদি(state->pen.small) {
    যদি(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    অন্যথা যদি(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  ঘূৰাই argi;
}

পূর্ণাঙ্ক vterm_state_get_penattr(ধ্রুৱ VTermState *state, VTermAttr attr, VTerm_মান *val)
{
  ৱিকল্প(attr) {
  পক্ষ VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_UNDERLINE:
    val->সংখ্যা = state->pen.underline;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_FONT:
    val->সংখ্যা = state->pen.font;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    ঘূৰাই 1;

  পক্ষ VTERM_ATTR_BASELINE:
    val->সংখ্যা = state->pen.baseline;
    ঘূৰাই 1;

  পক্ষ VTERM_N_ATTRS:
    ঘূৰাই 0;
  }

  ঘূৰাই 0;
}
