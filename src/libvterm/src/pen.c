#شامل "vterm_internal.h"

#شامل <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
تعریفنوع ساختار {
  uint8_t red, green, blue;
} VTermRGB;

ایستا ثابت VTermRGB ansi_colors[] = {
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

ایستا صحیح ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
ایستا صحیح ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

ایستا تهی lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

ایستا صحیح lookup_colour_ansi(ثابت VTermState *state, long index, VTermColor *col)
{
  اگر(index >= 0 && index < 16) {
    *col = state->colors[index];
    بازگشت TRUE;
  }

  بازگشت FALSE;
}

ایستا صحیح lookup_colour_palette(ثابت VTermState *state, long index, VTermColor *col)
{
  اگر(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    بازگشت lookup_colour_ansi(state, index, col);
  }
  وگرنه اگر(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    بازگشت TRUE;
  }
  وگرنه اگر(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    بازگشت TRUE;
  }

  بازگشت FALSE;
}

ایستا صحیح lookup_colour(ثابت VTermState *state, صحیح palette, ثابت long args[], صحیح argcount, VTermColor *col)
{
  انتخاب(palette) {
  حالت 2: // RGB mode - 3 args contain colour values directly
    اگر(argcount < 3)
      بازگشت argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    بازگشت 3;

  حالت 5: // XTerm 256-colour mode
    اگر (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      بازگشت argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    بازگشت 1;

  پیشفرض:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    بازگشت 0;
  }
}

// Some conveniences

ایستا تهی setpenattr(VTermState *state, VTermAttr attr, VTerm_مقدار_Type type UNUSED, VTerm_مقدار *val)
{
#ifdef DEBUG
  اگر(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    بازگشت;
  }
#endif
  اگر(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

ایستا تهی setpenattr_bool(VTermState *state, VTermAttr attr, صحیح boolean)
{
  VTerm_مقدار val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

ایستا تهی setpenattr_int(VTermState *state, VTermAttr attr, صحیح عدد)
{
  VTerm_مقدار val;
  val.عدد = عدد;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

ایستا تهی setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_مقدار val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

ایستا تهی set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL تهی vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  برای(صحیح col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL تهی vterm_state_resetpen(VTermState *state)
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

INTERNAL تهی vterm_state_savepen(VTermState *state, صحیح save)
{
  اگر(save) {
    state->saved.pen = state->pen;
  }
  وگرنه {
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

تهی vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

تهی vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

صحیح vterm_color_is_equal(ثابت VTermColor *a, ثابت VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  اگر (a->type != b->type) {
    بازگشت FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  اگر (VTERM_COLOR_IS_INDEXED(a)) {
    بازگشت a->index == b->index;
  }
  وگرنه اگر (VTERM_COLOR_IS_RGB(a)) {
    بازگشت    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  بازگشت 0;
}

تهی vterm_state_get_default_colors(ثابت VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

تهی vterm_state_get_palette_color(ثابت VTermState *state, صحیح index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

تهی vterm_state_set_default_colors(VTermState *state, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  اگر(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  اگر(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

تهی vterm_state_set_palette_color(VTermState *state, صحیح index, ثابت VTermColor *col)
{
  اگر(index >= 0 && index < 16)
    state->colors[index] = *col;
}

تهی vterm_state_convert_color_to_rgb(ثابت VTermState *state, VTermColor *col)
{
  اگر (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

تهی vterm_state_set_bold_highbright(VTermState *state, صحیح bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL تهی vterm_state_setpen(VTermState *state, ثابت long args[], صحیح argcount)
{
  // SGR - ECMA-48 8.3.117

  صحیح argi = 0;
  صحیح مقدار;

  وقتی(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    صحیح done = 1;

    long arg;
    انتخاب(arg = CSI_ARG(args[argi])) {
    حالت CSI_ARG_MISSING:
    حالت 0: // Reset
      vterm_state_resetpen(state);
      بشکن;

    حالت 1: { // Bold on
      ثابت VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      اگر(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      بشکن;
    }

    حالت 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      بشکن;

    حالت 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      اگر(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        انتخاب(CSI_ARG(args[argi])) {
          حالت 0:
            state->pen.underline = 0;
            بشکن;
          حالت 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            بشکن;
          حالت 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            بشکن;
          حالت 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            بشکن;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      بشکن;

    حالت 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      بشکن;

    حالت 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      بشکن;

    حالت 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      بشکن;

    حالت 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      بشکن;

    حالت 10: حالت 11: حالت 12: حالت 13: حالت 14:
    حالت 15: حالت 16: حالت 17: حالت 18: حالت 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      بشکن;

    حالت 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      بشکن;

    حالت 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      بشکن;

    حالت 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      بشکن;

    حالت 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      بشکن;

    حالت 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      بشکن;

    حالت 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      بشکن;

    حالت 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      بشکن;

    حالت 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      بشکن;

    حالت 30: حالت 31: حالت 32: حالت 33:
    حالت 34: حالت 35: حالت 36: حالت 37: // Foreground colour palette
      مقدار = CSI_ARG(args[argi]) - 30;
      اگر(state->pen.bold && state->bold_is_highbright)
        مقدار += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, مقدار);
      بشکن;

    حالت 38: // Foreground colour alternative palette
      اگر(argcount - argi < 1)
        بازگشت;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      بشکن;

    حالت 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      بشکن;

    حالت 40: حالت 41: حالت 42: حالت 43:
    حالت 44: حالت 45: حالت 46: حالت 47: // Background colour palette
      مقدار = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, مقدار);
      بشکن;

    حالت 48: // Background colour alternative palette
      اگر(argcount - argi < 1)
        بازگشت;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      بشکن;

    حالت 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      بشکن;

    حالت 73: // Superscript
    حالت 74: // Subscript
    حالت 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      بشکن;

    حالت 90: حالت 91: حالت 92: حالت 93:
    حالت 94: حالت 95: حالت 96: حالت 97: // Foreground colour high-intensity palette
      مقدار = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, مقدار);
      بشکن;

    حالت 100: حالت 101: حالت 102: حالت 103:
    حالت 104: حالت 105: حالت 106: حالت 107: // Background colour high-intensity palette
      مقدار = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, مقدار);
      بشکن;

    پیشفرض:
      done = 0;
      بشکن;
    }

    اگر(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    وقتی(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

ایستا صحیح vterm_state_getpen_color(ثابت VTermColor *col, صحیح argi, long args[], صحیح fg)
{
    /* Do nothing if the given color is the default color */
    اگر (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        بازگشت argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    اگر (VTERM_COLOR_IS_INDEXED(col)) {
        ثابت uint8_t idx = col->index;
        اگر (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        وگرنه اگر (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        وگرنه {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    وگرنه اگر (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    بازگشت argi;
}

INTERNAL صحیح vterm_state_getpen(VTermState *state, long args[], صحیح argcount UNUSED)
{
  صحیح argi = 0;

  اگر(state->pen.bold)
    args[argi++] = 1;

  اگر(state->pen.italic)
    args[argi++] = 3;

  اگر(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  اگر(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  اگر(state->pen.blink)
    args[argi++] = 5;

  اگر(state->pen.reverse)
    args[argi++] = 7;

  اگر(state->pen.conceal)
    args[argi++] = 8;

  اگر(state->pen.strike)
    args[argi++] = 9;

  اگر(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  اگر(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  اگر(state->pen.small) {
    اگر(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    وگرنه اگر(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  بازگشت argi;
}

صحیح vterm_state_get_penattr(ثابت VTermState *state, VTermAttr attr, VTerm_مقدار *val)
{
  انتخاب(attr) {
  حالت VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    بازگشت 1;

  حالت VTERM_ATTR_UNDERLINE:
    val->عدد = state->pen.underline;
    بازگشت 1;

  حالت VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    بازگشت 1;

  حالت VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    بازگشت 1;

  حالت VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    بازگشت 1;

  حالت VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    بازگشت 1;

  حالت VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    بازگشت 1;

  حالت VTERM_ATTR_FONT:
    val->عدد = state->pen.font;
    بازگشت 1;

  حالت VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    بازگشت 1;

  حالت VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    بازگشت 1;

  حالت VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    بازگشت 1;

  حالت VTERM_ATTR_BASELINE:
    val->عدد = state->pen.baseline;
    بازگشت 1;

  حالت VTERM_N_ATTRS:
    بازگشت 0;
  }

  بازگشت 0;
}
