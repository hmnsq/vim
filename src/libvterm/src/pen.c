#ضمن "vterm_internal.h"

#ضمن <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
تعريفنوع بنية {
  uint8_t red, green, blue;
} VTermRGB;

ساكن ثابت VTermRGB ansi_colors[] = {
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

ساكن صحيح ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
ساكن صحيح ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

ساكن فراغ lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

ساكن صحيح lookup_colour_ansi(ثابت VTermState *state, long index, VTermColor *col)
{
  إذا(index >= 0 && index < 16) {
    *col = state->colors[index];
    أرجع TRUE;
  }

  أرجع FALSE;
}

ساكن صحيح lookup_colour_palette(ثابت VTermState *state, long index, VTermColor *col)
{
  إذا(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    أرجع lookup_colour_ansi(state, index, col);
  }
  وإلا إذا(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    أرجع TRUE;
  }
  وإلا إذا(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    أرجع TRUE;
  }

  أرجع FALSE;
}

ساكن صحيح lookup_colour(ثابت VTermState *state, صحيح palette, ثابت long args[], صحيح argcount, VTermColor *col)
{
  بدل(palette) {
  حالة 2: // RGB mode - 3 args contain colour values directly
    إذا(argcount < 3)
      أرجع argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    أرجع 3;

  حالة 5: // XTerm 256-colour mode
    إذا (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      أرجع argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    أرجع 1;

  افتراضي:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    أرجع 0;
  }
}

// Some conveniences

ساكن فراغ setpenattr(VTermState *state, VTermAttr attr, VTerm_قىممەت_Type type UNUSED, VTerm_قىممەت *val)
{
#ifdef DEBUG
  إذا(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    أرجع;
  }
#endif
  إذا(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

ساكن فراغ setpenattr_bool(VTermState *state, VTermAttr attr, صحيح boolean)
{
  VTerm_قىممەت val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

ساكن فراغ setpenattr_int(VTermState *state, VTermAttr attr, صحيح سان)
{
  VTerm_قىممەت val;
  val.سان = سان;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

ساكن فراغ setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_قىممەت val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

ساكن فراغ set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL فراغ vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  لكل(صحيح col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL فراغ vterm_state_resetpen(VTermState *state)
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

INTERNAL فراغ vterm_state_savepen(VTermState *state, صحيح save)
{
  إذا(save) {
    state->saved.pen = state->pen;
  }
  وإلا {
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

فراغ vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

فراغ vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

صحيح vterm_color_is_equal(ثابت VTermColor *a, ثابت VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  إذا (a->type != b->type) {
    أرجع FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  إذا (VTERM_COLOR_IS_INDEXED(a)) {
    أرجع a->index == b->index;
  }
  وإلا إذا (VTERM_COLOR_IS_RGB(a)) {
    أرجع    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  أرجع 0;
}

فراغ vterm_state_get_default_colors(ثابت VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

فراغ vterm_state_get_palette_color(ثابت VTermState *state, صحيح index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

فراغ vterm_state_set_default_colors(VTermState *state, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  إذا(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  إذا(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

فراغ vterm_state_set_palette_color(VTermState *state, صحيح index, ثابت VTermColor *col)
{
  إذا(index >= 0 && index < 16)
    state->colors[index] = *col;
}

فراغ vterm_state_convert_color_to_rgb(ثابت VTermState *state, VTermColor *col)
{
  إذا (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

فراغ vterm_state_set_bold_highbright(VTermState *state, صحيح bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL فراغ vterm_state_setpen(VTermState *state, ثابت long args[], صحيح argcount)
{
  // SGR - ECMA-48 8.3.117

  صحيح argi = 0;
  صحيح قىممەت;

  طالما(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    صحيح done = 1;

    long arg;
    بدل(arg = CSI_ARG(args[argi])) {
    حالة CSI_ARG_MISSING:
    حالة 0: // Reset
      vterm_state_resetpen(state);
      اكسر;

    حالة 1: { // Bold on
      ثابت VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      إذا(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      اكسر;
    }

    حالة 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      اكسر;

    حالة 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      إذا(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        بدل(CSI_ARG(args[argi])) {
          حالة 0:
            state->pen.underline = 0;
            اكسر;
          حالة 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            اكسر;
          حالة 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            اكسر;
          حالة 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            اكسر;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      اكسر;

    حالة 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      اكسر;

    حالة 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      اكسر;

    حالة 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      اكسر;

    حالة 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      اكسر;

    حالة 10: حالة 11: حالة 12: حالة 13: حالة 14:
    حالة 15: حالة 16: حالة 17: حالة 18: حالة 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      اكسر;

    حالة 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      اكسر;

    حالة 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      اكسر;

    حالة 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      اكسر;

    حالة 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      اكسر;

    حالة 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      اكسر;

    حالة 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      اكسر;

    حالة 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      اكسر;

    حالة 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      اكسر;

    حالة 30: حالة 31: حالة 32: حالة 33:
    حالة 34: حالة 35: حالة 36: حالة 37: // Foreground colour palette
      قىممەت = CSI_ARG(args[argi]) - 30;
      إذا(state->pen.bold && state->bold_is_highbright)
        قىممەت += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, قىممەت);
      اكسر;

    حالة 38: // Foreground colour alternative palette
      إذا(argcount - argi < 1)
        أرجع;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      اكسر;

    حالة 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      اكسر;

    حالة 40: حالة 41: حالة 42: حالة 43:
    حالة 44: حالة 45: حالة 46: حالة 47: // Background colour palette
      قىممەت = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, قىممەت);
      اكسر;

    حالة 48: // Background colour alternative palette
      إذا(argcount - argi < 1)
        أرجع;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      اكسر;

    حالة 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      اكسر;

    حالة 73: // Superscript
    حالة 74: // Subscript
    حالة 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      اكسر;

    حالة 90: حالة 91: حالة 92: حالة 93:
    حالة 94: حالة 95: حالة 96: حالة 97: // Foreground colour high-intensity palette
      قىممەت = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, قىممەت);
      اكسر;

    حالة 100: حالة 101: حالة 102: حالة 103:
    حالة 104: حالة 105: حالة 106: حالة 107: // Background colour high-intensity palette
      قىممەت = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, قىممەت);
      اكسر;

    افتراضي:
      done = 0;
      اكسر;
    }

    إذا(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    طالما(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

ساكن صحيح vterm_state_getpen_color(ثابت VTermColor *col, صحيح argi, long args[], صحيح fg)
{
    /* Do nothing if the given color is the default color */
    إذا (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        أرجع argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    إذا (VTERM_COLOR_IS_INDEXED(col)) {
        ثابت uint8_t idx = col->index;
        إذا (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        وإلا إذا (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        وإلا {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    وإلا إذا (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    أرجع argi;
}

INTERNAL صحيح vterm_state_getpen(VTermState *state, long args[], صحيح argcount UNUSED)
{
  صحيح argi = 0;

  إذا(state->pen.bold)
    args[argi++] = 1;

  إذا(state->pen.italic)
    args[argi++] = 3;

  إذا(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  إذا(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  إذا(state->pen.blink)
    args[argi++] = 5;

  إذا(state->pen.reverse)
    args[argi++] = 7;

  إذا(state->pen.conceal)
    args[argi++] = 8;

  إذا(state->pen.strike)
    args[argi++] = 9;

  إذا(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  إذا(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  إذا(state->pen.small) {
    إذا(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    وإلا إذا(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  أرجع argi;
}

صحيح vterm_state_get_penattr(ثابت VTermState *state, VTermAttr attr, VTerm_قىممەت *val)
{
  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    أرجع 1;

  حالة VTERM_ATTR_UNDERLINE:
    val->سان = state->pen.underline;
    أرجع 1;

  حالة VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    أرجع 1;

  حالة VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    أرجع 1;

  حالة VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    أرجع 1;

  حالة VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    أرجع 1;

  حالة VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    أرجع 1;

  حالة VTERM_ATTR_FONT:
    val->سان = state->pen.font;
    أرجع 1;

  حالة VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    أرجع 1;

  حالة VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    أرجع 1;

  حالة VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    أرجع 1;

  حالة VTERM_ATTR_BASELINE:
    val->سان = state->pen.baseline;
    أرجع 1;

  حالة VTERM_N_ATTRS:
    أرجع 0;
  }

  أرجع 0;
}
