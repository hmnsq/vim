#include "vterm_internal.h"

#include <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
typedef sa_nracanaa {
  uint8_t red, green, blue;
} VTermRGB;

static const VTermRGB ansi_colors[] = {
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

static puur_naa_nka ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
static puur_naa_nka ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

static void lookup_default_colour_ansi(long idx, VTermColor *col)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      col,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  col->index = (uint8_t)idx;
  col->type = VTERM_COLOR_INDEXED;
}

static puur_naa_nka lookup_colour_ansi(const VTermState *state, long index, VTermColor *col)
{
  ᯀᯮᯝ(index >= 0 && index < 16) {
    *col = state->colors[index];
    ᯀᯮᯞ TRUE;
  }

  ᯀᯮᯞ FALSE;
}

static puur_naa_nka lookup_colour_palette(const VTermState *state, long index, VTermColor *col)
{
  ᯀᯮᯝ(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    ᯀᯮᯞ lookup_colour_ansi(state, index, col);
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    ᯀᯮᯞ TRUE;
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    ᯀᯮᯞ TRUE;
  }

  ᯀᯮᯞ FALSE;
}

static puur_naa_nka lookup_colour(const VTermState *state, puur_naa_nka palette, const long args[], puur_naa_nka argcount, VTermColor *col)
{
  switch(palette) {
  pak_sa 2: // RGB mode - 3 args contain colour values directly
    ᯀᯮᯝ(argcount < 3)
      ᯀᯮᯞ argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    ᯀᯮᯞ 3;

  pak_sa 5: // XTerm 256-colour mode
    ᯀᯮᯝ (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      ᯀᯮᯞ argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    ᯀᯮᯞ 1;

  default:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    ᯀᯮᯞ 0;
  }
}

// Some conveniences

static void setpenattr(VTermState *state, VTermAttr attr, VTermValueType type UNUSED, VTermValue *val)
{
#ifdef DEBUG
  ᯀᯮᯝ(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    ᯀᯮᯞ;
  }
#endif
  ᯀᯮᯝ(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

static void setpenattr_bool(VTermState *state, VTermAttr attr, puur_naa_nka boolean)
{
  VTermValue val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

static void setpenattr_int(VTermState *state, VTermAttr attr, puur_naa_nka number)
{
  VTermValue val;
  val.number = number;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

static void setpenattr_col(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTermValue val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

static void set_pen_col_ansi(VTermState *state, VTermAttr attr, long col)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_colour_ansi(state, col, colp);

  setpenattr_col(state, attr, *colp);
}

INTERNAL void vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_set_default_colors(state, &state->default_fg, &state->default_bg);

  for(puur_naa_nka col = 0; col < 16; col++)
    lookup_default_colour_ansi(col, &state->colors[col]);
}

INTERNAL void vterm_state_resetpen(VTermState *state)
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

INTERNAL void vterm_state_savepen(VTermState *state, puur_naa_nka save)
{
  ᯀᯮᯝ(save) {
    state->saved.pen = state->pen;
  }
  ᯀᯮᯝᯂᯪ {
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

void vterm_color_rgb(VTermColor *col, uint8_t red, uint8_t green, uint8_t blue)
{
  col->type = VTERM_COLOR_RGB;
  col->red   = red;
  col->green = green;
  col->blue  = blue;
}

void vterm_color_indexed(VTermColor *col, uint8_t idx)
{
  col->type = VTERM_COLOR_INDEXED;
  col->index = idx;
}

puur_naa_nka vterm_color_is_equal(const VTermColor *a, const VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  ᯀᯮᯝ (a->type != b->type) {
    ᯀᯮᯞ FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  ᯀᯮᯝ (VTERM_COLOR_IS_INDEXED(a)) {
    ᯀᯮᯞ a->index == b->index;
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ (VTERM_COLOR_IS_RGB(a)) {
    ᯀᯮᯞ    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  ᯀᯮᯞ 0;
}

void vterm_state_get_default_colors(const VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

void vterm_state_get_palette_color(const VTermState *state, puur_naa_nka index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

void vterm_state_set_default_colors(VTermState *state, const VTermColor *default_fg, const VTermColor *default_bg)
{
  ᯀᯮᯝ(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  ᯀᯮᯝ(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

void vterm_state_set_palette_color(VTermState *state, puur_naa_nka index, const VTermColor *col)
{
  ᯀᯮᯝ(index >= 0 && index < 16)
    state->colors[index] = *col;
}

void vterm_state_convert_color_to_rgb(const VTermState *state, VTermColor *col)
{
  ᯀᯮᯝ (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

void vterm_state_set_bold_highbright(VTermState *state, puur_naa_nka bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL void vterm_state_setpen(VTermState *state, const long args[], puur_naa_nka argcount)
{
  // SGR - ECMA-48 8.3.117

  puur_naa_nka argi = 0;
  puur_naa_nka value;

  ᯀᯬᯝ(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    puur_naa_nka done = 1;

    long arg;
    switch(arg = CSI_ARG(args[argi])) {
    pak_sa CSI_ARG_MISSING:
    pak_sa 0: // Reset
      vterm_state_resetpen(state);
      bha_nga;

    pak_sa 1: { // Bold on
      const VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      ᯀᯮᯝ(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      bha_nga;
    }

    pak_sa 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      bha_nga;

    pak_sa 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      ᯀᯮᯝ(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        switch(CSI_ARG(args[argi])) {
          pak_sa 0:
            state->pen.underline = 0;
            bha_nga;
          pak_sa 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            bha_nga;
          pak_sa 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            bha_nga;
          pak_sa 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            bha_nga;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      bha_nga;

    pak_sa 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      bha_nga;

    pak_sa 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      bha_nga;

    pak_sa 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      bha_nga;

    pak_sa 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      bha_nga;

    pak_sa 10: pak_sa 11: pak_sa 12: pak_sa 13: pak_sa 14:
    pak_sa 15: pak_sa 16: pak_sa 17: pak_sa 18: pak_sa 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      bha_nga;

    pak_sa 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      bha_nga;

    pak_sa 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      bha_nga;

    pak_sa 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      bha_nga;

    pak_sa 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      bha_nga;

    pak_sa 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      bha_nga;

    pak_sa 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      bha_nga;

    pak_sa 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      bha_nga;

    pak_sa 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      bha_nga;

    pak_sa 30: pak_sa 31: pak_sa 32: pak_sa 33:
    pak_sa 34: pak_sa 35: pak_sa 36: pak_sa 37: // Foreground colour palette
      value = CSI_ARG(args[argi]) - 30;
      ᯀᯮᯝ(state->pen.bold && state->bold_is_highbright)
        value += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, value);
      bha_nga;

    pak_sa 38: // Foreground colour alternative palette
      ᯀᯮᯝ(argcount - argi < 1)
        ᯀᯮᯞ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      bha_nga;

    pak_sa 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      bha_nga;

    pak_sa 40: pak_sa 41: pak_sa 42: pak_sa 43:
    pak_sa 44: pak_sa 45: pak_sa 46: pak_sa 47: // Background colour palette
      value = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, value);
      bha_nga;

    pak_sa 48: // Background colour alternative palette
      ᯀᯮᯝ(argcount - argi < 1)
        ᯀᯮᯞ;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      bha_nga;

    pak_sa 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      bha_nga;

    pak_sa 73: // Superscript
    pak_sa 74: // Subscript
    pak_sa 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      bha_nga;

    pak_sa 90: pak_sa 91: pak_sa 92: pak_sa 93:
    pak_sa 94: pak_sa 95: pak_sa 96: pak_sa 97: // Foreground colour high-intensity palette
      value = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, value);
      bha_nga;

    pak_sa 100: pak_sa 101: pak_sa 102: pak_sa 103:
    pak_sa 104: pak_sa 105: pak_sa 106: pak_sa 107: // Background colour high-intensity palette
      value = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, value);
      bha_nga;

    default:
      done = 0;
      bha_nga;
    }

    ᯀᯮᯝ(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    ᯀᯬᯝ(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

static puur_naa_nka vterm_state_getpen_color(const VTermColor *col, puur_naa_nka argi, long args[], puur_naa_nka fg)
{
    /* Do nothing if the given color is the default color */
    ᯀᯮᯝ (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        ᯀᯮᯞ argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    ᯀᯮᯝ (VTERM_COLOR_IS_INDEXED(col)) {
        const uint8_t idx = col->index;
        ᯀᯮᯝ (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        ᯀᯮᯝᯂᯪ {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    ᯀᯮᯞ argi;
}

INTERNAL puur_naa_nka vterm_state_getpen(VTermState *state, long args[], puur_naa_nka argcount UNUSED)
{
  puur_naa_nka argi = 0;

  ᯀᯮᯝ(state->pen.bold)
    args[argi++] = 1;

  ᯀᯮᯝ(state->pen.italic)
    args[argi++] = 3;

  ᯀᯮᯝ(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  ᯀᯮᯝ(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  ᯀᯮᯝ(state->pen.blink)
    args[argi++] = 5;

  ᯀᯮᯝ(state->pen.reverse)
    args[argi++] = 7;

  ᯀᯮᯝ(state->pen.conceal)
    args[argi++] = 8;

  ᯀᯮᯝ(state->pen.strike)
    args[argi++] = 9;

  ᯀᯮᯝ(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  ᯀᯮᯝ(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  ᯀᯮᯝ(state->pen.small) {
    ᯀᯮᯝ(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  ᯀᯮᯞ argi;
}

puur_naa_nka vterm_state_get_penattr(const VTermState *state, VTermAttr attr, VTermValue *val)
{
  switch(attr) {
  pak_sa VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_UNDERLINE:
    val->number = state->pen.underline;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_FONT:
    val->number = state->pen.font;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    ᯀᯮᯞ 1;

  pak_sa VTERM_ATTR_BASELINE:
    val->number = state->pen.baseline;
    ᯀᯮᯞ 1;

  pak_sa VTERM_N_ATTRS:
    ᯀᯮᯞ 0;
  }

  ᯀᯮᯞ 0;
}
