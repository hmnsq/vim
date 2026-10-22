#include "vterm_internal.h"

#include <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
typedef struct {
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

static 정수 ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
static 정수 ramp24[] = {
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

static 정수 lookup_colour_ansi(const VTermState *state, long index, VTermColor *col)
{
  만약(index >= 0 && index < 16) {
    *col = state->colors[index];
    반환 TRUE;
  }

  반환 FALSE;
}

static 정수 lookup_colour_palette(const VTermState *state, long index, VTermColor *col)
{
  만약(index >= 0 && index < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    반환 lookup_colour_ansi(state, index, col);
  }
  아니면 만약(index >= 16 && index < 232) {
    // 216-colour cube
    index -= 16;

    vterm_color_rgb(col, ramp6[index/6/6 % 6],
                         ramp6[index/6   % 6],
                         ramp6[index     % 6]);

    반환 TRUE;
  }
  아니면 만약(index >= 232 && index < 256) {
    // 24 greyscales
    index -= 232;

    vterm_color_rgb(col, ramp24[index], ramp24[index], ramp24[index]);

    반환 TRUE;
  }

  반환 FALSE;
}

static 정수 lookup_colour(const VTermState *state, 정수 palette, const long args[], 정수 argcount, VTermColor *col)
{
  switch(palette) {
  case 2: // RGB mode - 3 args contain colour values directly
    만약(argcount < 3)
      반환 argcount;

    vterm_color_rgb(col, (uint8_t)CSI_ARG(args[0]), (uint8_t)CSI_ARG(args[1]), (uint8_t)CSI_ARG(args[2]));

    반환 3;

  case 5: // XTerm 256-colour mode
    만약 (!argcount || CSI_ARG_IS_MISSING(args[0])) {
      반환 argcount ? 1 : 0;
    }

    lookup_colour_palette(state, args[0], col);
    반환 1;

  default:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    반환 0;
  }
}

// Some conveniences

static void setpenattr(VTermState *state, VTermAttr attr, VTermValueType type UNUSED, VTermValue *val)
{
#ifdef DEBUG
  만약(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    반환;
  }
#endif
  만약(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

static void setpenattr_bool(VTermState *state, VTermAttr attr, 정수 boolean)
{
  VTermValue val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

static void setpenattr_int(VTermState *state, VTermAttr attr, 정수 number)
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

  위해(정수 col = 0; col < 16; col++)
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

INTERNAL void vterm_state_savepen(VTermState *state, 정수 save)
{
  만약(save) {
    state->saved.pen = state->pen;
  }
  아니면 {
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

정수 vterm_color_is_equal(const VTermColor *a, const VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  만약 (a->type != b->type) {
    반환 FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  만약 (VTERM_COLOR_IS_INDEXED(a)) {
    반환 a->index == b->index;
  }
  아니면 만약 (VTERM_COLOR_IS_RGB(a)) {
    반환    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  반환 0;
}

void vterm_state_get_default_colors(const VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

void vterm_state_get_palette_color(const VTermState *state, 정수 index, VTermColor *col)
{
  lookup_colour_palette(state, index, col);
}

void vterm_state_set_default_colors(VTermState *state, const VTermColor *default_fg, const VTermColor *default_bg)
{
  만약(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  만약(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

void vterm_state_set_palette_color(VTermState *state, 정수 index, const VTermColor *col)
{
  만약(index >= 0 && index < 16)
    state->colors[index] = *col;
}

void vterm_state_convert_color_to_rgb(const VTermState *state, VTermColor *col)
{
  만약 (VTERM_COLOR_IS_INDEXED(col)) { /* Convert indexed colors to RGB */
    lookup_colour_palette(state, col->index, col);
  }
  col->type &= VTERM_COLOR_TYPE_MASK; /* Reset any metadata but the type */
}

void vterm_state_set_bold_highbright(VTermState *state, 정수 bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

INTERNAL void vterm_state_setpen(VTermState *state, const long args[], 정수 argcount)
{
  // SGR - ECMA-48 8.3.117

  정수 argi = 0;
  정수 value;

  동안(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    정수 done = 1;

    long arg;
    switch(arg = CSI_ARG(args[argi])) {
    case CSI_ARG_MISSING:
    case 0: // Reset
      vterm_state_resetpen(state);
      break;

    case 1: { // Bold on
      const VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      만약(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_INDEXED(fg) && fg->index < 8 && state->bold_is_highbright)
        set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, fg->index + (state->pen.bold ? 8 : 0));
      break;
    }

    case 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      break;

    case 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      만약(CSI_ARG_HAS_MORE(args[argi])) {
        argi++;
        switch(CSI_ARG(args[argi])) {
          case 0:
            state->pen.underline = 0;
            break;
          case 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            break;
          case 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            break;
          case 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            break;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      break;

    case 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      break;

    case 7: // Reverse on
      state->pen.reverse = 1;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 1);
      break;

    case 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      break;

    case 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      break;

    case 10: case 11: case 12: case 13: case 14:
    case 15: case 16: case 17: case 18: case 19: // Select font
      state->pen.font = CSI_ARG(args[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      break;

    case 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      break;

    case 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      break;

    case 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      break;

    case 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      break;

    case 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      break;

    case 27: // Reverse off
      state->pen.reverse = 0;
      setpenattr_bool(state, VTERM_ATTR_REVERSE, 0);
      break;

    case 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      break;

    case 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      break;

    case 30: case 31: case 32: case 33:
    case 34: case 35: case 36: case 37: // Foreground colour palette
      value = CSI_ARG(args[argi]) - 30;
      만약(state->pen.bold && state->bold_is_highbright)
        value += 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, value);
      break;

    case 38: // Foreground colour alternative palette
      만약(argcount - argi < 1)
        반환;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      break;

    case 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_col(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      break;

    case 40: case 41: case 42: case 43:
    case 44: case 45: case 46: case 47: // Background colour palette
      value = CSI_ARG(args[argi]) - 40;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, value);
      break;

    case 48: // Background colour alternative palette
      만약(argcount - argi < 1)
        반환;
      argi += 1 + lookup_colour(state, CSI_ARG(args[argi+1]), args+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      break;

    case 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_col(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      break;

    case 73: // Superscript
    case 74: // Subscript
    case 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_LOWER :
                      VTERM_BASELINE_NORMAL;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      break;

    case 90: case 91: case 92: case 93:
    case 94: case 95: case 96: case 97: // Foreground colour high-intensity palette
      value = CSI_ARG(args[argi]) - 90 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_FOREGROUND, value);
      break;

    case 100: case 101: case 102: case 103:
    case 104: case 105: case 106: case 107: // Background colour high-intensity palette
      value = CSI_ARG(args[argi]) - 100 + 8;
      set_pen_col_ansi(state, VTERM_ATTR_BACKGROUND, value);
      break;

    default:
      done = 0;
      break;
    }

    만약(!done)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    동안(CSI_ARG_HAS_MORE(args[argi++]))
      ;
  }
}

static 정수 vterm_state_getpen_color(const VTermColor *col, 정수 argi, long args[], 정수 fg)
{
    /* Do nothing if the given color is the default color */
    만약 (( fg && VTERM_COLOR_IS_DEFAULT_FG(col)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(col))) {
        반환 argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    만약 (VTERM_COLOR_IS_INDEXED(col)) {
        const uint8_t idx = col->index;
        만약 (idx < 8) {
            args[argi++] = (idx + (fg ? 30 : 40));
        }
        아니면 만약 (idx < 16) {
            args[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        아니면 {
            args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
            args[argi++] = CSI_ARG_FLAG_MORE | 5;
            args[argi++] = idx;
        }
    }
    아니면 만약 (VTERM_COLOR_IS_RGB(col)) {
        args[argi++] = CSI_ARG_FLAG_MORE | (fg ? 38 : 48);
        args[argi++] = CSI_ARG_FLAG_MORE | 2;
        args[argi++] = CSI_ARG_FLAG_MORE | col->red;
        args[argi++] = CSI_ARG_FLAG_MORE | col->green;
        args[argi++] = col->blue;
    }
    반환 argi;
}

INTERNAL 정수 vterm_state_getpen(VTermState *state, long args[], 정수 argcount UNUSED)
{
  정수 argi = 0;

  만약(state->pen.bold)
    args[argi++] = 1;

  만약(state->pen.italic)
    args[argi++] = 3;

  만약(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    args[argi++] = 4;
  만약(state->pen.underline == VTERM_UNDERLINE_CURLY)
    args[argi++] = 4 | CSI_ARG_FLAG_MORE, args[argi++] = 3;

  만약(state->pen.blink)
    args[argi++] = 5;

  만약(state->pen.reverse)
    args[argi++] = 7;

  만약(state->pen.conceal)
    args[argi++] = 8;

  만약(state->pen.strike)
    args[argi++] = 9;

  만약(state->pen.font)
    args[argi++] = 10 + state->pen.font;

  만약(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    args[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, args, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, args, FALSE);

  만약(state->pen.small) {
    만약(state->pen.baseline == VTERM_BASELINE_RAISE)
      args[argi++] = 73;
    아니면 만약(state->pen.baseline == VTERM_BASELINE_LOWER)
      args[argi++] = 74;
  }

  반환 argi;
}

정수 vterm_state_get_penattr(const VTermState *state, VTermAttr attr, VTermValue *val)
{
  switch(attr) {
  case VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    반환 1;

  case VTERM_ATTR_UNDERLINE:
    val->number = state->pen.underline;
    반환 1;

  case VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    반환 1;

  case VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    반환 1;

  case VTERM_ATTR_REVERSE:
    val->boolean = state->pen.reverse;
    반환 1;

  case VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    반환 1;

  case VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    반환 1;

  case VTERM_ATTR_FONT:
    val->number = state->pen.font;
    반환 1;

  case VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    반환 1;

  case VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    반환 1;

  case VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    반환 1;

  case VTERM_ATTR_BASELINE:
    val->number = state->pen.baseline;
    반환 1;

  case VTERM_N_ATTRS:
    반환 0;
  }

  반환 0;
}
