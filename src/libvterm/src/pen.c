#અન્તર્ભાવ "vterm_internal.h"

#અન્તર્ભાવ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
પ્રકારવ્યાખ્યા સંરચના {
  uint8_t red, green, blue;
} VTermRGB;

સ્થિર અચલ VTermRGB ansi_colors[] = {
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

સ્થિર પૂર્ણાંક ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
સ્થિર પૂર્ણાંક ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

સ્થિર શૂન્ય lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

સ્થિર પૂર્ણાંક lookup_colour_ansi(અચલ VTermState *state, long index, VTermColor *col)
{
  જો(index >= 0 && index < 16) {
    *col = state->colors[index];
    પાછા TRUE;
  }

  પાછા FALSE;
}

સ્થિર પૂર્ણાંક lookup_colour_palette(અચલ VTermState *state, long index, VTermColor *col)
{
  જો(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    પાછા lookup_colour_ansi(state, index, col);
  }
  નહીંતર જો(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    પાછા TRUE;
  }
  નહીંતર જો(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    પાછા TRUE;
  }

  પાછા FALSE;
}

સ્થિર પૂર્ણાંક lookup_colour(અચલ VTermState *state, પૂર્ણાંક palette, અચલ long args[], પૂર્ણાંક argcount, VTermColor *col)
{
  પસંદગી(palette) {
  સ્થિતિ 2: // RGB mode - 3 args contain colour values directly
    જો(argcount < 3)
      પાછા argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    પાછા 3;

  સ્થિતિ 5: // XTerm 256-colour mode
    જો (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      પાછા argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    પાછા 1;

  મૂળભૂત:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    પાછા 0;
  }
}

// Some conveniences

સ્થિર શૂન્ય setpenattr(VTermState *state, VTermAttr attr, VTerm_કિંમત_Type type UNUSED, VTerm_કિંમત *val)
{
#ifdef DEBUG
  જો(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    પાછા;
  }
#endif
  જો(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

સ્થિર શૂન્ય setpenattr_bool(VTermState *state, VTermAttr attr, પૂર્ણાંક boolean)
{
  VTerm_કિંમત val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

સ્થિર શૂન્ય setpenattr_int(VTermState *state, VTermAttr attr, પૂર્ણાંક સંખ્યા)
{
  VTerm_કિંમત val;
  val.સંખ્યા = સંખ્યા;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

સ્થિર શૂન્ય setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_કિંમત val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

સ્થિર શૂન્ય set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL શૂન્ય vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  માટે(પૂર્ણાંક col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL શૂન્ય vterm_state_resetpen(VTermState *state)
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

INTERNAL શૂન્ય vterm_state_savepen(VTermState *state, પૂર્ણાંક save)
{
  જો(save) {
    state->saved.pen = state->pen;
  }
  નહીંતર {
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

શૂન્ય vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

શૂન્ય vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

પૂર્ણાંક vterm_color_is_equal(અચલ VTermColor *a, અચલ VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  જો (a->type != b->type) {
    પાછા FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  જો (VTERM_COLOR_IS_INDEXED(a)) {
    પાછા a->index == b->index;
  }
  નહીંતર જો (VTERM_COLOR_IS_RGB(a)) {
    પાછા    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  પાછા 0;
}

શૂન્ય vterm_state_get_default_colors(અચલ VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

શૂન્ય vterm_state_get_palette_color(અચલ VTermState *state, પૂર્ણાંક index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

શૂન્ય vterm_state_set_default_colors(VTermState *state, અચલ VTermColor *default_fg, અચલ VTermColor *default_bg)
{
  જો(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  જો(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

શૂન્ય vterm_state_set_palette_color(VTermState *state, પૂર્ણાંક index, અચલ VTermColor *col)
{
  જો(index >= 0 && index < 16)
    state->colors[index] = *col;
}

શૂન્ય vterm_state_convert_color_to_rgb(અચલ VTermState *state, VTermColor *col)
{
  જો (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

શૂન્ય vterm_state_set_bold_highbright(VTermState *state, પૂર્ણાંક bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL શૂન્ય vterm_state_setpen(VTermState *state, અચલ long args[], પૂર્ણાંક argcount)
{
  // SGR - ECMA-48 8.3.117

  પૂર્ણાંક argi = 0;
  પૂર્ણાંક કિંમત;

  જ્યારે(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    પૂર્ણાંક done = 1;

    long arg;
    પસંદગી(arg = CSI_ARG(args[argi])) {
    સ્થિતિ CSI_ARG_MISSING:
    સ્થિતિ 0: // Reset
      vterm_state_resetpen(state);
      તોડો;

    સ્થિતિ 1: { // Bold on
      અચલ VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      જો(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      તોડો;
    }

    સ્થિતિ 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      તોડો;

    સ્થિતિ 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      જો(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        પસંદગી(CSI_ARG(args[argi])) {
          સ્થિતિ 0:
            state->pen.underline = 0;
            તોડો;
          સ્થિતિ 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            તોડો;
          સ્થિતિ 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            તોડો;
          સ્થિતિ 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            તોડો;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      તોડો;

    સ્થિતિ 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      તોડો;

    સ્થિતિ 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      તોડો;

    સ્થિતિ 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      તોડો;

    સ્થિતિ 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      તોડો;

    સ્થિતિ 10: સ્થિતિ 11: સ્થિતિ 12: સ્થિતિ 13: સ્થિતિ 14:
    સ્થિતિ 15: સ્થિતિ 16: સ્થિતિ 17: સ્થિતિ 18: સ્થિતિ 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      તોડો;

    સ્થિતિ 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      તોડો;

    સ્થિતિ 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      તોડો;

    સ્થિતિ 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      તોડો;

    સ્થિતિ 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      તોડો;

    સ્થિતિ 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      તોડો;

    સ્થિતિ 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      તોડો;

    સ્થિતિ 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      તોડો;

    સ્થિતિ 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      તોડો;

    સ્થિતિ 30: સ્થિતિ 31: સ્થિતિ 32: સ્થિતિ 33:
    સ્થિતિ 34: સ્થિતિ 35: સ્થિતિ 36: સ્થિતિ 37: // Foreground colour palette
      કિંમત = CSI_ARG(args[argi]) - 30;
      જો(state->pen.bold && state->bold_is_highbright)
        કિંમત += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, કિંમત);
      તોડો;

    સ્થિતિ 38: // Foreground colour alternative palette
      જો(argcount - argi < 1)
        પાછા;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      તોડો;

    સ્થિતિ 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      તોડો;

    સ્થિતિ 40: સ્થિતિ 41: સ્થિતિ 42: સ્થિતિ 43:
    સ્થિતિ 44: સ્થિતિ 45: સ્થિતિ 46: સ્થિતિ 47: // Background colour palette
      કિંમત = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, કિંમત);
      તોડો;

    સ્થિતિ 48: // Background colour alternative palette
      જો(argcount - argi < 1)
        પાછા;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      તોડો;

    સ્થિતિ 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      તોડો;

    સ્થિતિ 73: // Superscript
    સ્થિતિ 74: // Subscript
    સ્થિતિ 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      તોડો;

    સ્થિતિ 90: સ્થિતિ 91: સ્થિતિ 92: સ્થિતિ 93:
    સ્થિતિ 94: સ્થિતિ 95: સ્થિતિ 96: સ્થિતિ 97: // Foreground colour high-intensity palette
      કિંમત = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, કિંમત);
      તોડો;

    સ્થિતિ 100: સ્થિતિ 101: સ્થિતિ 102: સ્થિતિ 103:
    સ્થિતિ 104: સ્થિતિ 105: સ્થિતિ 106: સ્થિતિ 107: // Background colour high-intensity palette
      કિંમત = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, કિંમત);
      તોડો;

    મૂળભૂત:
      done = 0;
      તોડો;
    }

    જો(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    જ્યારે(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

સ્થિર પૂર્ણાંક vterm_state_getpen_color(અચલ VTermColor *col, પૂર્ણાંક argi, long args[], પૂર્ણાંક fg)
{
    /* Do nothing if the given color is the default color */
    જો (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        પાછા argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    જો (VTERM_COLOR_IS_INDEXED(col)) {
        અચલ uint8_t idx = col->index;
        જો (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        નહીંતર જો (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        નહીંતર {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    નહીંતર જો (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    પાછા argi;
}

INTERNAL પૂર્ણાંક vterm_state_getpen(VTermState *state, long args[], પૂર્ણાંક argcount UNUSED)
{
  પૂર્ણાંક argi = 0;

  જો(state->pen.bold)
    args[argi++] = 1;

  જો(state->pen.italic)
    args[argi++] = 3;

  જો(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  જો(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  જો(state->pen.blink)
    args[argi++] = 5;

  જો(state->pen.reverse)
    args[argi++] = 7;

  જો(state->pen.conceal)
    args[argi++] = 8;

  જો(state->pen.strike)
    args[argi++] = 9;

  જો(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  જો(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  જો(state->pen.small) {
    જો(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    નહીંતર જો(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  પાછા argi;
}

પૂર્ણાંક vterm_state_get_penattr(અચલ VTermState *state, VTermAttr attr, VTerm_કિંમત *val)
{
  પસંદગી(attr) {
  સ્થિતિ VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_UNDERLINE:
    val->સંખ્યા = state->pen.underline;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_FONT:
    val->સંખ્યા = state->pen.font;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    પાછા 1;

  સ્થિતિ VTERM_ATTR_BASELINE:
    val->સંખ્યા = state->pen.baseline;
    પાછા 1;

  સ્થિતિ VTERM_N_ATTRS:
    પાછા 0;
  }

  પાછા 0;
}
