#شامل "vterm_internal.h"

#شامل <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
قسمتعریف ساخت {
  uint8_t red, green, blue;
} VTermRGB;

ساکن ثابت VTermRGB ansi_colors[] = {
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

ساکن صحیح ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
ساکن صحیح ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

ساکن خالی lookup_default_رنگ_ansi(طویل idx, VTermColor *ستون)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      ستون,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  ستون->اشاریہ = (uint8_t)idx;
  ستون->type = VTERM_COLOR_اشاریہ_دار;
}

ساکن صحیح lookup_رنگ_ansi(ثابت VTermState *state, طویل اشاریہ, VTermColor *ستون)
{
  اگر(اشاریہ >= 0 && اشاریہ < 16) {
    *ستون = state->colors[اشاریہ];
    واپس TRUE;
  }

  واپس FALSE;
}

ساکن صحیح lookup_رنگ_palette(ثابت VTermState *state, طویل اشاریہ, VTermColor *ستون)
{
  اگر(اشاریہ >= 0 && اشاریہ < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    واپس lookup_رنگ_ansi(state, اشاریہ, ستون);
  }
  ورنہ اگر(اشاریہ >= 16 && اشاریہ < 232) {
    // 216-colour cube
    اشاریہ -= 16;

    vterm_color_rgb(ستون, ramp6[اشاریہ/6/6 % 6],
                         ramp6[اشاریہ/6   % 6],
                         ramp6[اشاریہ     % 6]);

    واپس TRUE;
  }
  ورنہ اگر(اشاریہ >= 232 && اشاریہ < 256) {
    // 24 greyscales
    اشاریہ -= 232;

    vterm_color_rgb(ستون, ramp24[اشاریہ], ramp24[اشاریہ], ramp24[اشاریہ]);

    واپس TRUE;
  }

  واپس FALSE;
}

ساکن صحیح lookup_رنگ(ثابت VTermState *state, صحیح palette, ثابت طویل دلائل[], صحیح argcount, VTermColor *ستون)
{
  انتخاب(palette) {
  حالت 2: // RGB mode - 3 args contain colour values directly
    اگر(argcount < 3)
      واپس argcount;

    vterm_color_rgb(ستون, (uint8_t)CSI_ARG(دلائل[0]), (uint8_t)CSI_ARG(دلائل[1]), (uint8_t)CSI_ARG(دلائل[2]));

    واپس 3;

  حالت 5: // XTerm 256-colour mode
    اگر (!argcount || CSI_ARG_IS_MISSING(دلائل[0])) {
      واپس argcount ? 1 : 0;
    }

    lookup_رنگ_palette(state, دلائل[0], ستون);
    واپس 1;

  طےشدہ:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    واپس 0;
  }
}

// Some conveniences

ساکن خالی setpenattr(VTermState *state, VTermAttr attr, VTerm_قدر_Type type غیر_مستعمل, VTerm_قدر *val)
{
#اگر_تعریف_شدہ DEBUG
  اگر(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    واپس;
  }
#اگر_ختم
  اگر(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

ساکن خالی setpenattr_bool(VTermState *state, VTermAttr attr, صحیح boolean)
{
  VTerm_قدر val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

ساکن خالی setpenattr_int(VTermState *state, VTermAttr attr, صحیح نمبر)
{
  VTerm_قدر val;
  val.نمبر = نمبر;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

ساکن خالی setpenattr_ستون(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_قدر val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

ساکن خالی سیٹ_pen_ستون_ansi(VTermState *state, VTermAttr attr, طویل ستون)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_رنگ_ansi(state, ستون, colp);

  setpenattr_ستون(state, attr, *colp);
}

اندرونی_2 خالی vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_سیٹ_default_colors(state, &state->default_fg, &state->default_bg);

  برائے(صحیح ستون = 0; ستون < 16; ستون++)
    lookup_default_رنگ_ansi(ستون, &state->colors[ستون]);
}

اندرونی_2 خالی vterm_state_resetpen(VTermState *state)
{
  state->pen.bold = 0;      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
  state->pen.underline = 0; setpenattr_int (state, VTERM_ATTR_UNDERLINE, 0);
  state->pen.italic = 0;    setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
  state->pen.blink = 0;     setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
  state->pen.الٹ = 0;   setpenattr_bool(state, VTERM_ATTR_الٹ, 0);
  state->pen.conceal = 0;   setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
  state->pen.strike = 0;    setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
  state->pen.font = 0;      setpenattr_int (state, VTERM_ATTR_FONT, 0);
  state->pen.small = 0;     setpenattr_bool(state, VTERM_ATTR_SMALL, 0);
  state->pen.baseline = 0;  setpenattr_int (state, VTERM_ATTR_BASELINE, 0);

  state->pen.fg = state->default_fg;  setpenattr_ستون(state, VTERM_ATTR_FOREGROUND, state->default_fg);
  state->pen.bg = state->default_bg;  setpenattr_ستون(state, VTERM_ATTR_BACKGROUND, state->default_bg);
}

اندرونی_2 خالی vterm_state_savepen(VTermState *state, صحیح save)
{
  اگر(save) {
    state->saved.pen = state->pen;
  }
  ورنہ {
    state->pen = state->saved.pen;

    setpenattr_bool(state, VTERM_ATTR_BOLD,      state->pen.bold);
    setpenattr_int (state, VTERM_ATTR_UNDERLINE, state->pen.underline);
    setpenattr_bool(state, VTERM_ATTR_ITALIC,    state->pen.italic);
    setpenattr_bool(state, VTERM_ATTR_BLINK,     state->pen.blink);
    setpenattr_bool(state, VTERM_ATTR_الٹ,   state->pen.الٹ);
    setpenattr_bool(state, VTERM_ATTR_CONCEAL,   state->pen.conceal);
    setpenattr_bool(state, VTERM_ATTR_STRIKE,    state->pen.strike);
    setpenattr_int (state, VTERM_ATTR_FONT,      state->pen.font);
    setpenattr_bool(state, VTERM_ATTR_SMALL,     state->pen.small);
    setpenattr_int (state, VTERM_ATTR_BASELINE,  state->pen.baseline);

    setpenattr_ستون( state, VTERM_ATTR_FOREGROUND, state->pen.fg);
    setpenattr_ستون( state, VTERM_ATTR_BACKGROUND, state->pen.bg);
  }
}

خالی vterm_color_rgb(VTermColor *ستون, uint8_t red, uint8_t green, uint8_t blue)
{
  ستون->type = VTERM_COLOR_RGB;
  ستون->red   = red;
  ستون->green = green;
  ستون->blue  = blue;
}

خالی vterm_color_اشاریہ_دار(VTermColor *ستون, uint8_t idx)
{
  ستون->type = VTERM_COLOR_اشاریہ_دار;
  ستون->اشاریہ = idx;
}

صحیح vterm_color_is_equal(ثابت VTermColor *a, ثابت VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  اگر (a->type != b->type) {
    واپس FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  اگر (VTERM_COLOR_IS_اشاریہ_دار(a)) {
    واپس a->اشاریہ == b->اشاریہ;
  }
  ورنہ اگر (VTERM_COLOR_IS_RGB(a)) {
    واپس    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  واپس 0;
}

خالی vterm_state_get_default_colors(ثابت VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

خالی vterm_state_get_palette_color(ثابت VTermState *state, صحیح اشاریہ, VTermColor *ستون)
{
  lookup_رنگ_palette(state, اشاریہ, ستون);
}

خالی vterm_state_سیٹ_default_colors(VTermState *state, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  اگر(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_نقاب)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  اگر(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_نقاب)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

خالی vterm_state_سیٹ_palette_color(VTermState *state, صحیح اشاریہ, ثابت VTermColor *ستون)
{
  اگر(اشاریہ >= 0 && اشاریہ < 16)
    state->colors[اشاریہ] = *ستون;
}

خالی vterm_state_convert_color_مقصد_rgb(ثابت VTermState *state, VTermColor *ستون)
{
  اگر (VTERM_COLOR_IS_اشاریہ_دار(ستون)) { /* Convert indexed colors to RGB */
    lookup_رنگ_palette(state, ستون->اشاریہ, ستون);
  }
  ستون->type &= VTERM_COLOR_TYPE_نقاب; /* Reset any metadata but the type */
}

خالی vterm_state_سیٹ_bold_highbright(VTermState *state, صحیح bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

اندرونی_2 خالی vterm_state_setpen(VTermState *state, ثابت طویل دلائل[], صحیح argcount)
{
  // SGR - ECMA-48 8.3.117

  صحیح argi = 0;
  صحیح قدر;

  جب(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    صحیح مکمل = 1;

    طویل arg;
    انتخاب(arg = CSI_ARG(دلائل[argi])) {
    حالت CSI_ARG_MISSING:
    حالت 0: // Reset
      vterm_state_resetpen(state);
      توڑو;

    حالت 1: { // Bold on
      ثابت VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      اگر(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_اشاریہ_دار(fg) && fg->اشاریہ < 8 && state->bold_is_highbright)
        سیٹ_pen_ستون_ansi(state, VTERM_ATTR_FOREGROUND, fg->اشاریہ + (state->pen.bold ? 8 : 0));
      توڑو;
    }

    حالت 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      توڑو;

    حالت 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      اگر(CSI_ARG_HAS_مزید(دلائل[argi])) {
        argi++;
        انتخاب(CSI_ARG(دلائل[argi])) {
          حالت 0:
            state->pen.underline = 0;
            توڑو;
          حالت 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            توڑو;
          حالت 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            توڑو;
          حالت 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            توڑو;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      توڑو;

    حالت 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      توڑو;

    حالت 7: // Reverse on
      state->pen.الٹ = 1;
      setpenattr_bool(state, VTERM_ATTR_الٹ, 1);
      توڑو;

    حالت 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      توڑو;

    حالت 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      توڑو;

    حالت 10: حالت 11: حالت 12: حالت 13: حالت 14:
    حالت 15: حالت 16: حالت 17: حالت 18: حالت 19: // Select font
      state->pen.font = CSI_ARG(دلائل[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      توڑو;

    حالت 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      توڑو;

    حالت 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      توڑو;

    حالت 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      توڑو;

    حالت 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      توڑو;

    حالت 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      توڑو;

    حالت 27: // Reverse off
      state->pen.الٹ = 0;
      setpenattr_bool(state, VTERM_ATTR_الٹ, 0);
      توڑو;

    حالت 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      توڑو;

    حالت 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      توڑو;

    حالت 30: حالت 31: حالت 32: حالت 33:
    حالت 34: حالت 35: حالت 36: حالت 37: // Foreground colour palette
      قدر = CSI_ARG(دلائل[argi]) - 30;
      اگر(state->pen.bold && state->bold_is_highbright)
        قدر += 8;
      سیٹ_pen_ستون_ansi(state, VTERM_ATTR_FOREGROUND, قدر);
      توڑو;

    حالت 38: // Foreground colour alternative palette
      اگر(argcount - argi < 1)
        واپس;
      argi += 1 + lookup_رنگ(state, CSI_ARG(دلائل[argi+1]), دلائل+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_ستون(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      توڑو;

    حالت 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_ستون(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      توڑو;

    حالت 40: حالت 41: حالت 42: حالت 43:
    حالت 44: حالت 45: حالت 46: حالت 47: // Background colour palette
      قدر = CSI_ARG(دلائل[argi]) - 40;
      سیٹ_pen_ستون_ansi(state, VTERM_ATTR_BACKGROUND, قدر);
      توڑو;

    حالت 48: // Background colour alternative palette
      اگر(argcount - argi < 1)
        واپس;
      argi += 1 + lookup_رنگ(state, CSI_ARG(دلائل[argi+1]), دلائل+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_ستون(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      توڑو;

    حالت 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_ستون(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      توڑو;

    حالت 73: // Superscript
    حالت 74: // Subscript
    حالت 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_زیریں :
                      VTERM_BASELINE_معمول;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      توڑو;

    حالت 90: حالت 91: حالت 92: حالت 93:
    حالت 94: حالت 95: حالت 96: حالت 97: // Foreground colour high-intensity palette
      قدر = CSI_ARG(دلائل[argi]) - 90 + 8;
      سیٹ_pen_ستون_ansi(state, VTERM_ATTR_FOREGROUND, قدر);
      توڑو;

    حالت 100: حالت 101: حالت 102: حالت 103:
    حالت 104: حالت 105: حالت 106: حالت 107: // Background colour high-intensity palette
      قدر = CSI_ARG(دلائل[argi]) - 100 + 8;
      سیٹ_pen_ستون_ansi(state, VTERM_ATTR_BACKGROUND, قدر);
      توڑو;

    طےشدہ:
      مکمل = 0;
      توڑو;
    }

    اگر(!مکمل)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    جب(CSI_ARG_HAS_مزید(دلائل[argi++]))
      ;
  }
}

ساکن صحیح vterm_state_getpen_color(ثابت VTermColor *ستون, صحیح argi, طویل دلائل[], صحیح fg)
{
    /* Do nothing if the given color is the default color */
    اگر (( fg && VTERM_COLOR_IS_DEFAULT_FG(ستون)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(ستون))) {
        واپس argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    اگر (VTERM_COLOR_IS_اشاریہ_دار(ستون)) {
        ثابت uint8_t idx = ستون->اشاریہ;
        اگر (idx < 8) {
            دلائل[argi++] = (idx + (fg ? 30 : 40));
        }
        ورنہ اگر (idx < 16) {
            دلائل[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        ورنہ {
            دلائل[argi++] = CSI_ARG_FLAG_مزید | (fg ? 38 : 48);
            دلائل[argi++] = CSI_ARG_FLAG_مزید | 5;
            دلائل[argi++] = idx;
        }
    }
    ورنہ اگر (VTERM_COLOR_IS_RGB(ستون)) {
        دلائل[argi++] = CSI_ARG_FLAG_مزید | (fg ? 38 : 48);
        دلائل[argi++] = CSI_ARG_FLAG_مزید | 2;
        دلائل[argi++] = CSI_ARG_FLAG_مزید | ستون->red;
        دلائل[argi++] = CSI_ARG_FLAG_مزید | ستون->green;
        دلائل[argi++] = ستون->blue;
    }
    واپس argi;
}

اندرونی_2 صحیح vterm_state_getpen(VTermState *state, طویل دلائل[], صحیح argcount غیر_مستعمل)
{
  صحیح argi = 0;

  اگر(state->pen.bold)
    دلائل[argi++] = 1;

  اگر(state->pen.italic)
    دلائل[argi++] = 3;

  اگر(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    دلائل[argi++] = 4;
  اگر(state->pen.underline == VTERM_UNDERLINE_CURLY)
    دلائل[argi++] = 4 | CSI_ARG_FLAG_مزید, دلائل[argi++] = 3;

  اگر(state->pen.blink)
    دلائل[argi++] = 5;

  اگر(state->pen.الٹ)
    دلائل[argi++] = 7;

  اگر(state->pen.conceal)
    دلائل[argi++] = 8;

  اگر(state->pen.strike)
    دلائل[argi++] = 9;

  اگر(state->pen.font)
    دلائل[argi++] = 10 + state->pen.font;

  اگر(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    دلائل[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, دلائل, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, دلائل, FALSE);

  اگر(state->pen.small) {
    اگر(state->pen.baseline == VTERM_BASELINE_RAISE)
      دلائل[argi++] = 73;
    ورنہ اگر(state->pen.baseline == VTERM_BASELINE_زیریں)
      دلائل[argi++] = 74;
  }

  واپس argi;
}

صحیح vterm_state_get_penattr(ثابت VTermState *state, VTermAttr attr, VTerm_قدر *val)
{
  انتخاب(attr) {
  حالت VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    واپس 1;

  حالت VTERM_ATTR_UNDERLINE:
    val->نمبر = state->pen.underline;
    واپس 1;

  حالت VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    واپس 1;

  حالت VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    واپس 1;

  حالت VTERM_ATTR_الٹ:
    val->boolean = state->pen.الٹ;
    واپس 1;

  حالت VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    واپس 1;

  حالت VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    واپس 1;

  حالت VTERM_ATTR_FONT:
    val->نمبر = state->pen.font;
    واپس 1;

  حالت VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    واپس 1;

  حالت VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    واپس 1;

  حالت VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    واپس 1;

  حالت VTERM_ATTR_BASELINE:
    val->نمبر = state->pen.baseline;
    واپس 1;

  حالت VTERM_N_ATTRS:
    واپس 0;
  }

  واپس 0;
}
