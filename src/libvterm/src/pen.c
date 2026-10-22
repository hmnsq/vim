#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
प्रकारनिर्देश संरचना {
  uint8_t red, green, blue;
} VTermRGB;

स्थिर ध्रुव VTermRGB ansi_colors[] = {
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

स्थिर पूर्णाङ्क ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
स्थिर पूर्णाङ्क ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

स्थिर शून्य lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

स्थिर पूर्णाङ्क lookup_colour_ansi(ध्रुव VTermState *state, long index, VTermColor *col)
{
  यदि(index >= 0 && index < 16) {
    *col = state->colors[index];
    प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE;
}

स्थिर पूर्णाङ्क lookup_colour_palette(ध्रुव VTermState *state, long index, VTermColor *col)
{
  यदि(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    प्रत्यागम lookup_colour_ansi(state, index, col);
  }
  अन्यथा यदि(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    प्रत्यागम TRUE;
  }
  अन्यथा यदि(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    प्रत्यागम TRUE;
  }

  प्रत्यागम FALSE;
}

स्थिर पूर्णाङ्क lookup_colour(ध्रुव VTermState *state, पूर्णाङ्क palette, ध्रुव long args[], पूर्णाङ्क argcount, VTermColor *col)
{
  विकल्प(palette) {
  पक्ष 2: // RGB mode - 3 args contain colour values directly
    यदि(argcount < 3)
      प्रत्यागम argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    प्रत्यागम 3;

  पक्ष 5: // XTerm 256-colour mode
    यदि (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      प्रत्यागम argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    प्रत्यागम 1;

  स्वभाव:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    प्रत्यागम 0;
  }
}

// Some conveniences

स्थिर शून्य setpenattr(VTermState *state, VTermAttr attr, VTerm_मूल्य_Type type UNUSED, VTerm_मूल्य *val)
{
#ifdef DEBUG
  यदि(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    प्रत्यागम;
  }
#endif
  यदि(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

स्थिर शून्य setpenattr_bool(VTermState *state, VTermAttr attr, पूर्णाङ्क boolean)
{
  VTerm_मूल्य val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

स्थिर शून्य setpenattr_int(VTermState *state, VTermAttr attr, पूर्णाङ्क संख्या)
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

स्थिर शून्य set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
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

  प्रति(पूर्णाङ्क col = 0; col < 16; col++)
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

INTERNAL शून्य vterm_state_savepen(VTermState *state, पूर्णाङ्क save)
{
  यदि(save) {
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

पूर्णाङ्क vterm_color_is_equal(ध्रुव VTermColor *a, ध्रुव VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  यदि (a->type != b->type) {
    प्रत्यागम FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  यदि (VTERM_COLOR_IS_INDEXED(a)) {
    प्रत्यागम a->index == b->index;
  }
  अन्यथा यदि (VTERM_COLOR_IS_RGB(a)) {
    प्रत्यागम    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  प्रत्यागम 0;
}

शून्य vterm_state_get_default_colors(ध्रुव VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

शून्य vterm_state_get_palette_color(ध्रुव VTermState *state, पूर्णाङ्क index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

शून्य vterm_state_set_default_colors(VTermState *state, ध्रुव VTermColor *default_fg, ध्रुव VTermColor *default_bg)
{
  यदि(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  यदि(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

शून्य vterm_state_set_palette_color(VTermState *state, पूर्णाङ्क index, ध्रुव VTermColor *col)
{
  यदि(index >= 0 && index < 16)
    state->colors[index] = *col;
}

शून्य vterm_state_convert_color_to_rgb(ध्रुव VTermState *state, VTermColor *col)
{
  यदि (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

शून्य vterm_state_set_bold_highbright(VTermState *state, पूर्णाङ्क bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL शून्य vterm_state_setpen(VTermState *state, ध्रुव long args[], पूर्णाङ्क argcount)
{
  // SGR - ECMA-48 8.3.117

  पूर्णाङ्क argi = 0;
  पूर्णाङ्क मूल्य;

  यावत्(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    पूर्णाङ्क done = 1;

    long arg;
    विकल्प(arg = CSI_ARG(args[argi])) {
    पक्ष CSI_ARG_MISSING:
    पक्ष 0: // Reset
      vterm_state_resetpen(state);
      भङ्ग;

    पक्ष 1: { // Bold on
      ध्रुव VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      यदि(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      भङ्ग;
    }

    पक्ष 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      भङ्ग;

    पक्ष 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      यदि(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        विकल्प(CSI_ARG(args[argi])) {
          पक्ष 0:
            state->pen.underline = 0;
            भङ्ग;
          पक्ष 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            भङ्ग;
          पक्ष 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            भङ्ग;
          पक्ष 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            भङ्ग;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      भङ्ग;

    पक्ष 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      भङ्ग;

    पक्ष 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      भङ्ग;

    पक्ष 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      भङ्ग;

    पक्ष 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      भङ्ग;

    पक्ष 10: पक्ष 11: पक्ष 12: पक्ष 13: पक्ष 14:
    पक्ष 15: पक्ष 16: पक्ष 17: पक्ष 18: पक्ष 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      भङ्ग;

    पक्ष 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      भङ्ग;

    पक्ष 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      भङ्ग;

    पक्ष 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      भङ्ग;

    पक्ष 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      भङ्ग;

    पक्ष 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      भङ्ग;

    पक्ष 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      भङ्ग;

    पक्ष 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      भङ्ग;

    पक्ष 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      भङ्ग;

    पक्ष 30: पक्ष 31: पक्ष 32: पक्ष 33:
    पक्ष 34: पक्ष 35: पक्ष 36: पक्ष 37: // Foreground colour palette
      मूल्य = CSI_ARG(args[argi]) - 30;
      यदि(state->pen.bold && state->bold_is_highbright)
        मूल्य += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, मूल्य);
      भङ्ग;

    पक्ष 38: // Foreground colour alternative palette
      यदि(argcount - argi < 1)
        प्रत्यागम;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      भङ्ग;

    पक्ष 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      भङ्ग;

    पक्ष 40: पक्ष 41: पक्ष 42: पक्ष 43:
    पक्ष 44: पक्ष 45: पक्ष 46: पक्ष 47: // Background colour palette
      मूल्य = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, मूल्य);
      भङ्ग;

    पक्ष 48: // Background colour alternative palette
      यदि(argcount - argi < 1)
        प्रत्यागम;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      भङ्ग;

    पक्ष 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      भङ्ग;

    पक्ष 73: // Superscript
    पक्ष 74: // Subscript
    पक्ष 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      भङ्ग;

    पक्ष 90: पक्ष 91: पक्ष 92: पक्ष 93:
    पक्ष 94: पक्ष 95: पक्ष 96: पक्ष 97: // Foreground colour high-intensity palette
      मूल्य = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, मूल्य);
      भङ्ग;

    पक्ष 100: पक्ष 101: पक्ष 102: पक्ष 103:
    पक्ष 104: पक्ष 105: पक्ष 106: पक्ष 107: // Background colour high-intensity palette
      मूल्य = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, मूल्य);
      भङ्ग;

    स्वभाव:
      done = 0;
      भङ्ग;
    }

    यदि(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    यावत्(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

स्थिर पूर्णाङ्क vterm_state_getpen_color(ध्रुव VTermColor *col, पूर्णाङ्क argi, long args[], पूर्णाङ्क fg)
{
    /* Do nothing if the given color is the default color */
    यदि (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        प्रत्यागम argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    यदि (VTERM_COLOR_IS_INDEXED(col)) {
        ध्रुव uint8_t idx = col->index;
        यदि (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        अन्यथा यदि (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        अन्यथा {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    अन्यथा यदि (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    प्रत्यागम argi;
}

INTERNAL पूर्णाङ्क vterm_state_getpen(VTermState *state, long args[], पूर्णाङ्क argcount UNUSED)
{
  पूर्णाङ्क argi = 0;

  यदि(state->pen.bold)
    args[argi++] = 1;

  यदि(state->pen.italic)
    args[argi++] = 3;

  यदि(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  यदि(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  यदि(state->pen.blink)
    args[argi++] = 5;

  यदि(state->pen.reverse)
    args[argi++] = 7;

  यदि(state->pen.conceal)
    args[argi++] = 8;

  यदि(state->pen.strike)
    args[argi++] = 9;

  यदि(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  यदि(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  यदि(state->pen.small) {
    यदि(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    अन्यथा यदि(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  प्रत्यागम argi;
}

पूर्णाङ्क vterm_state_get_penattr(ध्रुव VTermState *state, VTermAttr attr, VTerm_मूल्य *val)
{
  विकल्प(attr) {
  पक्ष VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_UNDERLINE:
    val->संख्या = state->pen.underline;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_FONT:
    val->संख्या = state->pen.font;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    प्रत्यागम 1;

  पक्ष VTERM_ATTR_BASELINE:
    val->संख्या = state->pen.baseline;
    प्रत्यागम 1;

  पक्ष VTERM_N_ATTRS:
    प्रत्यागम 0;
  }

  प्रत्यागम 0;
}
