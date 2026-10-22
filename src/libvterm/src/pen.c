#அந்தர்பாவ "vterm_internal.h"

#அந்தர்பாவ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
வகைவரை கட்டமைப்பு {
  uint8_t red, green, blue;
} VTermRGB;

நிலையான மாறிலி VTermRGB ansi_colors[] = {
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

நிலையான முழு ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
நிலையான முழு ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

நிலையான வெறுமை lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

நிலையான முழு lookup_colour_ansi(மாறிலி VTermState *state, long index, VTermColor *col)
{
  எனில்(index >= 0 && index < 16) {
    *col = state->colors[index];
    திரும்பு TRUE;
  }

  திரும்பு FALSE;
}

நிலையான முழு lookup_colour_palette(மாறிலி VTermState *state, long index, VTermColor *col)
{
  எனில்(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    திரும்பு lookup_colour_ansi(state, index, col);
  }
  இல்லையெனில் எனில்(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    திரும்பு TRUE;
  }

  திரும்பு FALSE;
}

நிலையான முழு lookup_colour(மாறிலி VTermState *state, முழு palette, மாறிலி long args[], முழு argcount, VTermColor *col)
{
  தேர்வு(palette) {
  நிலை 2: // RGB mode - 3 args contain colour values directly
    எனில்(argcount < 3)
      திரும்பு argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    திரும்பு 3;

  நிலை 5: // XTerm 256-colour mode
    எனில் (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      திரும்பு argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    திரும்பு 1;

  இயல்பு:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    திரும்பு 0;
  }
}

// Some conveniences

நிலையான வெறுமை setpenattr(VTermState *state, VTermAttr attr, VTerm_மதிப்பு_Type type UNUSED, VTerm_மதிப்பு *val)
{
#ifdef DEBUG
  எனில்(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    திரும்பு;
  }
#endif
  எனில்(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

நிலையான வெறுமை setpenattr_bool(VTermState *state, VTermAttr attr, முழு boolean)
{
  VTerm_மதிப்பு val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

நிலையான வெறுமை setpenattr_int(VTermState *state, VTermAttr attr, முழு எண்)
{
  VTerm_மதிப்பு val;
  val.எண் = எண்;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

நிலையான வெறுமை setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_மதிப்பு val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

நிலையான வெறுமை set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL வெறுமை vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  க்கு(முழு col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL வெறுமை vterm_state_resetpen(VTermState *state)
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

INTERNAL வெறுமை vterm_state_savepen(VTermState *state, முழு save)
{
  எனில்(save) {
    state->saved.pen = state->pen;
  }
  இல்லையெனில் {
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

வெறுமை vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

வெறுமை vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

முழு vterm_color_is_equal(மாறிலி VTermColor *a, மாறிலி VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  எனில் (a->type != b->type) {
    திரும்பு FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  எனில் (VTERM_COLOR_IS_INDEXED(a)) {
    திரும்பு a->index == b->index;
  }
  இல்லையெனில் எனில் (VTERM_COLOR_IS_RGB(a)) {
    திரும்பு    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  திரும்பு 0;
}

வெறுமை vterm_state_get_default_colors(மாறிலி VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

வெறுமை vterm_state_get_palette_color(மாறிலி VTermState *state, முழு index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

வெறுமை vterm_state_set_default_colors(VTermState *state, மாறிலி VTermColor *default_fg, மாறிலி VTermColor *default_bg)
{
  எனில்(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  எனில்(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

வெறுமை vterm_state_set_palette_color(VTermState *state, முழு index, மாறிலி VTermColor *col)
{
  எனில்(index >= 0 && index < 16)
    state->colors[index] = *col;
}

வெறுமை vterm_state_convert_color_to_rgb(மாறிலி VTermState *state, VTermColor *col)
{
  எனில் (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

வெறுமை vterm_state_set_bold_highbright(VTermState *state, முழு bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL வெறுமை vterm_state_setpen(VTermState *state, மாறிலி long args[], முழு argcount)
{
  // SGR - ECMA-48 8.3.117

  முழு argi = 0;
  முழு மதிப்பு;

  போது(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    முழு done = 1;

    long arg;
    தேர்வு(arg = CSI_ARG(args[argi])) {
    நிலை CSI_ARG_MISSING:
    நிலை 0: // Reset
      vterm_state_resetpen(state);
      நிறுத்து;

    நிலை 1: { // Bold on
      மாறிலி VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      எனில்(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      நிறுத்து;
    }

    நிலை 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      நிறுத்து;

    நிலை 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      எனில்(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        தேர்வு(CSI_ARG(args[argi])) {
          நிலை 0:
            state->pen.underline = 0;
            நிறுத்து;
          நிலை 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            நிறுத்து;
          நிலை 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            நிறுத்து;
          நிலை 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            நிறுத்து;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      நிறுத்து;

    நிலை 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      நிறுத்து;

    நிலை 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      நிறுத்து;

    நிலை 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      நிறுத்து;

    நிலை 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      நிறுத்து;

    நிலை 10: நிலை 11: நிலை 12: நிலை 13: நிலை 14:
    நிலை 15: நிலை 16: நிலை 17: நிலை 18: நிலை 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      நிறுத்து;

    நிலை 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      நிறுத்து;

    நிலை 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      நிறுத்து;

    நிலை 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      நிறுத்து;

    நிலை 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      நிறுத்து;

    நிலை 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      நிறுத்து;

    நிலை 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      நிறுத்து;

    நிலை 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      நிறுத்து;

    நிலை 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      நிறுத்து;

    நிலை 30: நிலை 31: நிலை 32: நிலை 33:
    நிலை 34: நிலை 35: நிலை 36: நிலை 37: // Foreground colour palette
      மதிப்பு = CSI_ARG(args[argi]) - 30;
      எனில்(state->pen.bold && state->bold_is_highbright)
        மதிப்பு += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, மதிப்பு);
      நிறுத்து;

    நிலை 38: // Foreground colour alternative palette
      எனில்(argcount - argi < 1)
        திரும்பு;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      நிறுத்து;

    நிலை 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      நிறுத்து;

    நிலை 40: நிலை 41: நிலை 42: நிலை 43:
    நிலை 44: நிலை 45: நிலை 46: நிலை 47: // Background colour palette
      மதிப்பு = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, மதிப்பு);
      நிறுத்து;

    நிலை 48: // Background colour alternative palette
      எனில்(argcount - argi < 1)
        திரும்பு;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      நிறுத்து;

    நிலை 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      நிறுத்து;

    நிலை 73: // Superscript
    நிலை 74: // Subscript
    நிலை 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      நிறுத்து;

    நிலை 90: நிலை 91: நிலை 92: நிலை 93:
    நிலை 94: நிலை 95: நிலை 96: நிலை 97: // Foreground colour high-intensity palette
      மதிப்பு = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, மதிப்பு);
      நிறுத்து;

    நிலை 100: நிலை 101: நிலை 102: நிலை 103:
    நிலை 104: நிலை 105: நிலை 106: நிலை 107: // Background colour high-intensity palette
      மதிப்பு = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, மதிப்பு);
      நிறுத்து;

    இயல்பு:
      done = 0;
      நிறுத்து;
    }

    எனில்(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    போது(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

நிலையான முழு vterm_state_getpen_color(மாறிலி VTermColor *col, முழு argi, long args[], முழு fg)
{
    /* Do nothing if the given color is the default color */
    எனில் (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        திரும்பு argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    எனில் (VTERM_COLOR_IS_INDEXED(col)) {
        மாறிலி uint8_t idx = col->index;
        எனில் (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        இல்லையெனில் எனில் (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        இல்லையெனில் {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    இல்லையெனில் எனில் (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    திரும்பு argi;
}

INTERNAL முழு vterm_state_getpen(VTermState *state, long args[], முழு argcount UNUSED)
{
  முழு argi = 0;

  எனில்(state->pen.bold)
    args[argi++] = 1;

  எனில்(state->pen.italic)
    args[argi++] = 3;

  எனில்(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  எனில்(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  எனில்(state->pen.blink)
    args[argi++] = 5;

  எனில்(state->pen.reverse)
    args[argi++] = 7;

  எனில்(state->pen.conceal)
    args[argi++] = 8;

  எனில்(state->pen.strike)
    args[argi++] = 9;

  எனில்(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  எனில்(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  எனில்(state->pen.small) {
    எனில்(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    இல்லையெனில் எனில்(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  திரும்பு argi;
}

முழு vterm_state_get_penattr(மாறிலி VTermState *state, VTermAttr attr, VTerm_மதிப்பு *val)
{
  தேர்வு(attr) {
  நிலை VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    திரும்பு 1;

  நிலை VTERM_ATTR_UNDERLINE:
    val->எண் = state->pen.underline;
    திரும்பு 1;

  நிலை VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    திரும்பு 1;

  நிலை VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    திரும்பு 1;

  நிலை VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    திரும்பு 1;

  நிலை VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    திரும்பு 1;

  நிலை VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    திரும்பு 1;

  நிலை VTERM_ATTR_FONT:
    val->எண் = state->pen.font;
    திரும்பு 1;

  நிலை VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    திரும்பு 1;

  நிலை VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    திரும்பு 1;

  நிலை VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    திரும்பு 1;

  நிலை VTERM_ATTR_BASELINE:
    val->எண் = state->pen.baseline;
    திரும்பு 1;

  நிலை VTERM_N_ATTRS:
    திரும்பு 0;
  }

  திரும்பு 0;
}
