#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
प्रकारव्याख्या संरचना {
  uint8_t red, green, blue;
} VTermRGB;

स्थिर स्थिरांक VTermRGB ansi_colors[] = {
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

स्थिर पूर्णांक ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
स्थिर पूर्णांक ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

स्थिर शून्य lookup_default_colour_ansi(दीर्घ idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

स्थिर पूर्णांक lookup_colour_ansi(स्थिरांक VTermState *state, दीर्घ index, VTermColor *col)
{
  जर(index >= 0 && index < 16) {
    *col = state->colors[index];
    परत TRUE;
  }

  परत FALSE;
}

स्थिर पूर्णांक lookup_colour_palette(स्थिरांक VTermState *state, दीर्घ index, VTermColor *col)
{
  जर(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    परत lookup_colour_ansi(state, index, col);
  }
  अन्यथा जर(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    परत TRUE;
  }
  अन्यथा जर(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    परत TRUE;
  }

  परत FALSE;
}

स्थिर पूर्णांक lookup_colour(स्थिरांक VTermState *state, पूर्णांक palette, स्थिरांक दीर्घ args[], पूर्णांक argcount, VTermColor *col)
{
  निवड(palette) {
  स्थिती 2: // RGB mode - 3 args contain colour values directly
    जर(argcount < 3)
      परत argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    परत 3;

  स्थिती 5: // XTerm 256-colour mode
    जर (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      परत argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    परत 1;

  पूर्वनिर्धारित:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    परत 0;
  }
}

// Some conveniences

स्थिर शून्य setpenattr(VTermState *state, VTermAttr attr, VTerm_मूल्य_Type type UNUSED, VTerm_मूल्य *val)
{
#ifdef DEBUG
  जर(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    परत;
  }
#endif
  जर(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

स्थिर शून्य setpenattr_bool(VTermState *state, VTermAttr attr, पूर्णांक boolean)
{
  VTerm_मूल्य val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

स्थिर शून्य setpenattr_int(VTermState *state, VTermAttr attr, पूर्णांक संख्या)
{
  VTerm_मूल्य val;
  val.संख्या = संख्या;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

स्थिर शून्य setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_मूल्य val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

स्थिर शून्य set_pen_col_ansi(VTermState *state, VTermAttr attr, दीर्घ col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL शून्य vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  साठी(पूर्णांक col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL शून्य vterm_state_resetpen(VTermState *state)
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

INTERNAL शून्य vterm_state_savepen(VTermState *state, पूर्णांक save)
{
  जर(save) {
    state->saved.pen = state->pen;
  }
  अन्यथा {
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

शून्य vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

शून्य vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

पूर्णांक vterm_color_is_equal(स्थिरांक VTermColor *a, स्थिरांक VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  जर (a->type != b->type) {
    परत FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  जर (VTERM_COLOR_IS_INDEXED(a)) {
    परत a->index == b->index;
  }
  अन्यथा जर (VTERM_COLOR_IS_RGB(a)) {
    परत    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  परत 0;
}

शून्य vterm_state_get_default_colors(स्थिरांक VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

शून्य vterm_state_get_palette_color(स्थिरांक VTermState *state, पूर्णांक index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

शून्य vterm_state_set_default_colors(VTermState *state, स्थिरांक VTermColor *default_fg, स्थिरांक VTermColor *default_bg)
{
  जर(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  जर(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

शून्य vterm_state_set_palette_color(VTermState *state, पूर्णांक index, स्थिरांक VTermColor *col)
{
  जर(index >= 0 && index < 16)
    state->colors[index] = *col;
}

शून्य vterm_state_convert_color_to_rgb(स्थिरांक VTermState *state, VTermColor *col)
{
  जर (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

शून्य vterm_state_set_bold_highbright(VTermState *state, पूर्णांक bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL शून्य vterm_state_setpen(VTermState *state, स्थिरांक दीर्घ args[], पूर्णांक argcount)
{
  // SGR - ECMA-48 8.3.117

  पूर्णांक argi = 0;
  पूर्णांक मूल्य;

  पर्यंत(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    पूर्णांक done = 1;

    दीर्घ arg;
    निवड(arg = CSI_ARG(args[argi])) {
    स्थिती CSI_ARG_MISSING:
    स्थिती 0: // Reset
      vterm_state_resetpen(state);
      थांबा;

    स्थिती 1: { // Bold on
      स्थिरांक VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      जर(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      थांबा;
    }

    स्थिती 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      थांबा;

    स्थिती 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      जर(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        निवड(CSI_ARG(args[argi])) {
          स्थिती 0:
            state->pen.underline = 0;
            थांबा;
          स्थिती 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            थांबा;
          स्थिती 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            थांबा;
          स्थिती 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            थांबा;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      थांबा;

    स्थिती 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      थांबा;

    स्थिती 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      थांबा;

    स्थिती 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      थांबा;

    स्थिती 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      थांबा;

    स्थिती 10: स्थिती 11: स्थिती 12: स्थिती 13: स्थिती 14:
    स्थिती 15: स्थिती 16: स्थिती 17: स्थिती 18: स्थिती 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      थांबा;

    स्थिती 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      थांबा;

    स्थिती 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      थांबा;

    स्थिती 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      थांबा;

    स्थिती 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      थांबा;

    स्थिती 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      थांबा;

    स्थिती 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      थांबा;

    स्थिती 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      थांबा;

    स्थिती 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      थांबा;

    स्थिती 30: स्थिती 31: स्थिती 32: स्थिती 33:
    स्थिती 34: स्थिती 35: स्थिती 36: स्थिती 37: // Foreground colour palette
      मूल्य = CSI_ARG(args[argi]) - 30;
      जर(state->pen.bold && state->bold_is_highbright)
        मूल्य += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, मूल्य);
      थांबा;

    स्थिती 38: // Foreground colour alternative palette
      जर(argcount - argi < 1)
        परत;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      थांबा;

    स्थिती 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      थांबा;

    स्थिती 40: स्थिती 41: स्थिती 42: स्थिती 43:
    स्थिती 44: स्थिती 45: स्थिती 46: स्थिती 47: // Background colour palette
      मूल्य = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, मूल्य);
      थांबा;

    स्थिती 48: // Background colour alternative palette
      जर(argcount - argi < 1)
        परत;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      थांबा;

    स्थिती 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      थांबा;

    स्थिती 73: // Superscript
    स्थिती 74: // Subscript
    स्थिती 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      थांबा;

    स्थिती 90: स्थिती 91: स्थिती 92: स्थिती 93:
    स्थिती 94: स्थिती 95: स्थिती 96: स्थिती 97: // Foreground colour high-intensity palette
      मूल्य = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, मूल्य);
      थांबा;

    स्थिती 100: स्थिती 101: स्थिती 102: स्थिती 103:
    स्थिती 104: स्थिती 105: स्थिती 106: स्थिती 107: // Background colour high-intensity palette
      मूल्य = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, मूल्य);
      थांबा;

    पूर्वनिर्धारित:
      done = 0;
      थांबा;
    }

    जर(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    पर्यंत(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

स्थिर पूर्णांक vterm_state_getpen_color(स्थिरांक VTermColor *col, पूर्णांक argi, दीर्घ args[], पूर्णांक fg)
{
    /* Do nothing if the given color is the default color */
    जर (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        परत argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    जर (VTERM_COLOR_IS_INDEXED(col)) {
        स्थिरांक uint8_t idx = col->index;
        जर (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        अन्यथा जर (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        अन्यथा {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    अन्यथा जर (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    परत argi;
}

INTERNAL पूर्णांक vterm_state_getpen(VTermState *state, दीर्घ args[], पूर्णांक argcount UNUSED)
{
  पूर्णांक argi = 0;

  जर(state->pen.bold)
    args[argi++] = 1;

  जर(state->pen.italic)
    args[argi++] = 3;

  जर(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  जर(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  जर(state->pen.blink)
    args[argi++] = 5;

  जर(state->pen.reverse)
    args[argi++] = 7;

  जर(state->pen.conceal)
    args[argi++] = 8;

  जर(state->pen.strike)
    args[argi++] = 9;

  जर(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  जर(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  जर(state->pen.small) {
    जर(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    अन्यथा जर(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  परत argi;
}

पूर्णांक vterm_state_get_penattr(स्थिरांक VTermState *state, VTermAttr attr, VTerm_मूल्य *val)
{
  निवड(attr) {
  स्थिती VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    परत 1;

  स्थिती VTERM_ATTR_UNDERLINE:
    val->संख्या = state->pen.underline;
    परत 1;

  स्थिती VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    परत 1;

  स्थिती VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    परत 1;

  स्थिती VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    परत 1;

  स्थिती VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    परत 1;

  स्थिती VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    परत 1;

  स्थिती VTERM_ATTR_FONT:
    val->संख्या = state->pen.font;
    परत 1;

  स्थिती VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    परत 1;

  स्थिती VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    परत 1;

  स्थिती VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    परत 1;

  स्थिती VTERM_ATTR_BASELINE:
    val->संख्या = state->pen.baseline;
    परत 1;

  स्थिती VTERM_N_ATTRS:
    परत 0;
  }

  परत 0;
}
