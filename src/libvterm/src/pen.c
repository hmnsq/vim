#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>

/**
 * Structure used to store RGB triples without the additional metadata stored in
 * VTermColor.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  uint8_t red, green, blue;
} VTermRGB;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ VTermRGB ansi_colors[] = {
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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ramp6[] = {
  0x00, 0x5F, 0x87, 0xAF, 0xD7, 0xFF,
};

// Use 0x81 instead of 0x80 to be able to distinguish from ansi black
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ramp24[] = {
  0x08, 0x12, 0x1C, 0x26, 0x30, 0x3A, 0x44, 0x4E, 0x58, 0x62, 0x6C, 0x76,
  0x81, 0x8A, 0x94, 0x9E, 0xA8, 0xB2, 0xBC, 0xC6, 0xD0, 0xDA, 0xE4, 0xEE,
};

ಸ್ಥಿರ ಶೂನ್ಯ lookup_default_ಬಣ್ಣ_ansi(ದೀರ್ಘ idx, VTermColor *ಲಂಬಸಾಲು)
{
  // VIM: store both RGB color and index
  vterm_color_rgb(
      ಲಂಬಸಾಲು,
      ansi_colors[idx].red, ansi_colors[idx].green, ansi_colors[idx].blue);
  ಲಂಬಸಾಲು->ಸೂಚ್ಯಂಕ = (uint8_t)idx;
  ಲಂಬಸಾಲು->type = VTERM_COLOR_ಸೂಚ್ಯಂಕಿತ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lookup_ಬಣ್ಣ_ansi(ಸ್ಥಿರಾಂಕ VTermState *state, ದೀರ್ಘ ಸೂಚ್ಯಂಕ, VTermColor *ಲಂಬಸಾಲು)
{
  ಆದರೆ(ಸೂಚ್ಯಂಕ >= 0 && ಸೂಚ್ಯಂಕ < 16) {
    *ಲಂಬಸಾಲು = state->colors[ಸೂಚ್ಯಂಕ];
    ಹಿಂತಿರುಗಿ TRUE;
  }

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lookup_ಬಣ್ಣ_palette(ಸ್ಥಿರಾಂಕ VTermState *state, ದೀರ್ಘ ಸೂಚ್ಯಂಕ, VTermColor *ಲಂಬಸಾಲು)
{
  ಆದರೆ(ಸೂಚ್ಯಂಕ >= 0 && ಸೂಚ್ಯಂಕ < 16) {
    // Normal 8 colours or high intensity - parse as palette 0
    ಹಿಂತಿರುಗಿ lookup_ಬಣ್ಣ_ansi(state, ಸೂಚ್ಯಂಕ, ಲಂಬಸಾಲು);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ >= 16 && ಸೂಚ್ಯಂಕ < 232) {
    // 216-colour cube
    ಸೂಚ್ಯಂಕ -= 16;

    vterm_color_rgb(ಲಂಬಸಾಲು, ramp6[ಸೂಚ್ಯಂಕ/6/6 % 6],
                         ramp6[ಸೂಚ್ಯಂಕ/6   % 6],
                         ramp6[ಸೂಚ್ಯಂಕ     % 6]);

    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸೂಚ್ಯಂಕ >= 232 && ಸೂಚ್ಯಂಕ < 256) {
    // 24 greyscales
    ಸೂಚ್ಯಂಕ -= 232;

    vterm_color_rgb(ಲಂಬಸಾಲು, ramp24[ಸೂಚ್ಯಂಕ], ramp24[ಸೂಚ್ಯಂಕ], ramp24[ಸೂಚ್ಯಂಕ]);

    ಹಿಂತಿರುಗಿ TRUE;
  }

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lookup_ಬಣ್ಣ(ಸ್ಥಿರಾಂಕ VTermState *state, ಪೂರ್ಣಾಂಕ palette, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount, VTermColor *ಲಂಬಸಾಲು)
{
  ಆಯ್ಕೆ(palette) {
  ಸ್ಥಿತಿ 2: // RGB mode - 3 args contain colour values directly
    ಆದರೆ(argcount < 3)
      ಹಿಂತಿರುಗಿ argcount;

    vterm_color_rgb(ಲಂಬಸಾಲು, (uint8_t)CSI_ARG(ವಾದಗಳು[0]), (uint8_t)CSI_ARG(ವಾದಗಳು[1]), (uint8_t)CSI_ARG(ವಾದಗಳು[2]));

    ಹಿಂತಿರುಗಿ 3;

  ಸ್ಥಿತಿ 5: // XTerm 256-colour mode
    ಆದರೆ (!argcount || CSI_ARG_IS_MISSING(ವಾದಗಳು[0])) {
      ಹಿಂತಿರುಗಿ argcount ? 1 : 0;
    }

    lookup_ಬಣ್ಣ_palette(state, ವಾದಗಳು[0], ಲಂಬಸಾಲು);
    ಹಿಂತಿರುಗಿ 1;

  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUG_LOG1("Unrecognised colour palette %d\n", palette);
    ಹಿಂತಿರುಗಿ 0;
  }
}

// Some conveniences

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr(VTermState *state, VTermAttr attr, VTerm_ಮೌಲ್ಯ_Type type ಬಳಸದ, VTerm_ಮೌಲ್ಯ *val)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
  ಆದರೆ(type != vterm_get_attr_type(attr)) {
    DEBUG_LOG3("Cannot set attr %d as it has type %d, not type %d\n",
        attr, vterm_get_attr_type(attr), type);
    ಹಿಂತಿರುಗಿ;
  }
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(state->callbacks && state->callbacks->setpenattr)
    (*state->callbacks->setpenattr)(attr, val, state->cbdata);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr_bool(VTermState *state, VTermAttr attr, ಪೂರ್ಣಾಂಕ boolean)
{
  VTerm_ಮೌಲ್ಯ val;
  val.boolean = boolean;
  setpenattr(state, attr, VTERM_VALUETYPE_BOOL, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr_int(VTermState *state, VTermAttr attr, ಪೂರ್ಣಾಂಕ ಸಂಖ್ಯೆ)
{
  VTerm_ಮೌಲ್ಯ val;
  val.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;
  setpenattr(state, attr, VTERM_VALUETYPE_INT, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ setpenattr_ಲಂಬಸಾಲು(VTermState *state, VTermAttr attr, VTermColor color)
{
  VTerm_ಮೌಲ್ಯ val;
  val.color = color;
  setpenattr(state, attr, VTERM_VALUETYPE_COLOR, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_pen_ಲಂಬಸಾಲು_ansi(VTermState *state, VTermAttr attr, ದೀರ್ಘ ಲಂಬಸಾಲು)
{
  VTermColor *colp = (attr == VTERM_ATTR_BACKGROUND) ? &state->pen.bg : &state->pen.fg;

  lookup_ಬಣ್ಣ_ansi(state, ಲಂಬಸಾಲು, colp);

  setpenattr_ಲಂಬಸಾಲು(state, attr, *colp);
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_state_newpen(VTermState *state)
{
  // 90% grey so that pure white is brighter
  vterm_color_rgb(&state->default_fg, 240, 240, 240);
  vterm_color_rgb(&state->default_bg, 0, 0, 0);
  vterm_state_ಗಣ_default_colors(state, &state->default_fg, &state->default_bg);

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < 16; ಲಂಬಸಾಲು++)
    lookup_default_ಬಣ್ಣ_ansi(ಲಂಬಸಾಲು, &state->colors[ಲಂಬಸಾಲು]);
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_state_resetpen(VTermState *state)
{
  state->pen.bold = 0;      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
  state->pen.underline = 0; setpenattr_int (state, VTERM_ATTR_UNDERLINE, 0);
  state->pen.italic = 0;    setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
  state->pen.blink = 0;     setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
  state->pen.ಹಿಮ್ಮುಖ = 0;   setpenattr_bool(state, VTERM_ATTR_ಹಿಮ್ಮುಖ, 0);
  state->pen.conceal = 0;   setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
  state->pen.strike = 0;    setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
  state->pen.font = 0;      setpenattr_int (state, VTERM_ATTR_FONT, 0);
  state->pen.small = 0;     setpenattr_bool(state, VTERM_ATTR_SMALL, 0);
  state->pen.baseline = 0;  setpenattr_int (state, VTERM_ATTR_BASELINE, 0);

  state->pen.fg = state->default_fg;  setpenattr_ಲಂಬಸಾಲು(state, VTERM_ATTR_FOREGROUND, state->default_fg);
  state->pen.bg = state->default_bg;  setpenattr_ಲಂಬಸಾಲು(state, VTERM_ATTR_BACKGROUND, state->default_bg);
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_state_savepen(VTermState *state, ಪೂರ್ಣಾಂಕ save)
{
  ಆದರೆ(save) {
    state->saved.pen = state->pen;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    state->pen = state->saved.pen;

    setpenattr_bool(state, VTERM_ATTR_BOLD,      state->pen.bold);
    setpenattr_int (state, VTERM_ATTR_UNDERLINE, state->pen.underline);
    setpenattr_bool(state, VTERM_ATTR_ITALIC,    state->pen.italic);
    setpenattr_bool(state, VTERM_ATTR_BLINK,     state->pen.blink);
    setpenattr_bool(state, VTERM_ATTR_ಹಿಮ್ಮುಖ,   state->pen.ಹಿಮ್ಮುಖ);
    setpenattr_bool(state, VTERM_ATTR_CONCEAL,   state->pen.conceal);
    setpenattr_bool(state, VTERM_ATTR_STRIKE,    state->pen.strike);
    setpenattr_int (state, VTERM_ATTR_FONT,      state->pen.font);
    setpenattr_bool(state, VTERM_ATTR_SMALL,     state->pen.small);
    setpenattr_int (state, VTERM_ATTR_BASELINE,  state->pen.baseline);

    setpenattr_ಲಂಬಸಾಲು( state, VTERM_ATTR_FOREGROUND, state->pen.fg);
    setpenattr_ಲಂಬಸಾಲು( state, VTERM_ATTR_BACKGROUND, state->pen.bg);
  }
}

ಶೂನ್ಯ vterm_color_rgb(VTermColor *ಲಂಬಸಾಲು, uint8_t red, uint8_t green, uint8_t blue)
{
  ಲಂಬಸಾಲು->type = VTERM_COLOR_RGB;
  ಲಂಬಸಾಲು->red   = red;
  ಲಂಬಸಾಲು->green = green;
  ಲಂಬಸಾಲು->blue  = blue;
}

ಶೂನ್ಯ vterm_color_ಸೂಚ್ಯಂಕಿತ(VTermColor *ಲಂಬಸಾಲು, uint8_t idx)
{
  ಲಂಬಸಾಲು->type = VTERM_COLOR_ಸೂಚ್ಯಂಕಿತ;
  ಲಂಬಸಾಲು->ಸೂಚ್ಯಂಕ = idx;
}

ಪೂರ್ಣಾಂಕ vterm_color_is_equal(ಸ್ಥಿರಾಂಕ VTermColor *a, ಸ್ಥಿರಾಂಕ VTermColor *b)
{
  /* First make sure that the two colours are of the same type (RGB/Indexed) */
  ಆದರೆ (a->type != b->type) {
    ಹಿಂತಿರುಗಿ FALSE;
  }

  /* Depending on the type inspect the corresponding members */
  ಆದರೆ (VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(a)) {
    ಹಿಂತಿರುಗಿ a->ಸೂಚ್ಯಂಕ == b->ಸೂಚ್ಯಂಕ;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VTERM_COLOR_IS_RGB(a)) {
    ಹಿಂತಿರುಗಿ    (a->red   == b->red)
           && (a->green == b->green)
           && (a->blue  == b->blue);
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vterm_state_get_default_colors(ಸ್ಥಿರಾಂಕ VTermState *state, VTermColor *default_fg, VTermColor *default_bg)
{
  *default_fg = state->default_fg;
  *default_bg = state->default_bg;
}

ಶೂನ್ಯ vterm_state_get_palette_color(ಸ್ಥಿರಾಂಕ VTermState *state, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, VTermColor *ಲಂಬಸಾಲು)
{
  lookup_ಬಣ್ಣ_palette(state, ಸೂಚ್ಯಂಕ, ಲಂಬಸಾಲು);
}

ಶೂನ್ಯ vterm_state_ಗಣ_default_colors(VTermState *state, ಸ್ಥಿರಾಂಕ VTermColor *default_fg, ಸ್ಥಿರಾಂಕ VTermColor *default_bg)
{
  ಆದರೆ(default_fg) {
    state->default_fg = *default_fg;
    state->default_fg.type = (state->default_fg.type & ~VTERM_COLOR_DEFAULT_ಮುಖವಾಡ)
                           | VTERM_COLOR_DEFAULT_FG;
  }

  ಆದರೆ(default_bg) {
    state->default_bg = *default_bg;
    state->default_bg.type = (state->default_bg.type & ~VTERM_COLOR_DEFAULT_ಮುಖವಾಡ)
                           | VTERM_COLOR_DEFAULT_BG;
  }
}

ಶೂನ್ಯ vterm_state_ಗಣ_palette_color(VTermState *state, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಸ್ಥಿರಾಂಕ VTermColor *ಲಂಬಸಾಲು)
{
  ಆದರೆ(ಸೂಚ್ಯಂಕ >= 0 && ಸೂಚ್ಯಂಕ < 16)
    state->colors[ಸೂಚ್ಯಂಕ] = *ಲಂಬಸಾಲು;
}

ಶೂನ್ಯ vterm_state_convert_color_ಗಮ್ಯ_rgb(ಸ್ಥಿರಾಂಕ VTermState *state, VTermColor *ಲಂಬಸಾಲು)
{
  ಆದರೆ (VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(ಲಂಬಸಾಲು)) { /* Convert indexed colors to RGB */
    lookup_ಬಣ್ಣ_palette(state, ಲಂಬಸಾಲು->ಸೂಚ್ಯಂಕ, ಲಂಬಸಾಲು);
  }
  ಲಂಬಸಾಲು->type &= VTERM_COLOR_TYPE_ಮುಖವಾಡ; /* Reset any metadata but the type */
}

ಶೂನ್ಯ vterm_state_ಗಣ_bold_highbright(VTermState *state, ಪೂರ್ಣಾಂಕ bold_is_highbright)
{
  state->bold_is_highbright = bold_is_highbright;
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_state_setpen(VTermState *state, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount)
{
  // SGR - ECMA-48 8.3.117

  ಪೂರ್ಣಾಂಕ argi = 0;
  ಪೂರ್ಣಾಂಕ ಮೌಲ್ಯ;

  ಆಗ(argi < argcount) {
    // This logic is easier to do 'done' backwards; set it true, and make it
    // false again in the 'default' case
    ಪೂರ್ಣಾಂಕ ಮುಗಿದ = 1;

    ದೀರ್ಘ arg;
    ಆಯ್ಕೆ(arg = CSI_ARG(ವಾದಗಳು[argi])) {
    ಸ್ಥಿತಿ CSI_ARG_MISSING:
    ಸ್ಥಿತಿ 0: // Reset
      vterm_state_resetpen(state);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1: { // Bold on
      ಸ್ಥಿರಾಂಕ VTermColor *fg = &state->pen.fg;
      state->pen.bold = 1;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 1);
      ಆದರೆ(!VTERM_COLOR_IS_DEFAULT_FG(fg) && VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(fg) && fg->ಸೂಚ್ಯಂಕ < 8 && state->bold_is_highbright)
        ಗಣ_pen_ಲಂಬಸಾಲು_ansi(state, VTERM_ATTR_FOREGROUND, fg->ಸೂಚ್ಯಂಕ + (state->pen.bold ? 8 : 0));
      ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ 3: // Italic on
      state->pen.italic = 1;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 1);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 4: // Underline
      state->pen.underline = VTERM_UNDERLINE_SINGLE;
      ಆದರೆ(CSI_ARG_HAS_ಇನ್ನಷ್ಟು(ವಾದಗಳು[argi])) {
        argi++;
        ಆಯ್ಕೆ(CSI_ARG(ವಾದಗಳು[argi])) {
          ಸ್ಥಿತಿ 0:
            state->pen.underline = 0;
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 1:
            state->pen.underline = VTERM_UNDERLINE_SINGLE;
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 2:
            state->pen.underline = VTERM_UNDERLINE_DOUBLE;
            ನಿಲ್ಲಿಸು;
          ಸ್ಥಿತಿ 3:
            state->pen.underline = VTERM_UNDERLINE_CURLY;
            ನಿಲ್ಲಿಸು;
        }
      }
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 5: // Blink
      state->pen.blink = 1;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 1);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 7: // Reverse on
      state->pen.ಹಿಮ್ಮುಖ = 1;
      setpenattr_bool(state, VTERM_ATTR_ಹಿಮ್ಮುಖ, 1);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 8: // Conceal on
      state->pen.conceal = 1;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 1);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 9: // Strikethrough on
      state->pen.strike = 1;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 1);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 10: ಸ್ಥಿತಿ 11: ಸ್ಥಿತಿ 12: ಸ್ಥಿತಿ 13: ಸ್ಥಿತಿ 14:
    ಸ್ಥಿತಿ 15: ಸ್ಥಿತಿ 16: ಸ್ಥಿತಿ 17: ಸ್ಥಿತಿ 18: ಸ್ಥಿತಿ 19: // Select font
      state->pen.font = CSI_ARG(ವಾದಗಳು[argi]) - 10;
      setpenattr_int(state, VTERM_ATTR_FONT, state->pen.font);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 21: // Underline double
      state->pen.underline = VTERM_UNDERLINE_DOUBLE;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, state->pen.underline);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 22: // Bold off
      state->pen.bold = 0;
      setpenattr_bool(state, VTERM_ATTR_BOLD, 0);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 23: // Italic and Gothic (currently unsupported) off
      state->pen.italic = 0;
      setpenattr_bool(state, VTERM_ATTR_ITALIC, 0);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 24: // Underline off
      state->pen.underline = 0;
      setpenattr_int(state, VTERM_ATTR_UNDERLINE, 0);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 25: // Blink off
      state->pen.blink = 0;
      setpenattr_bool(state, VTERM_ATTR_BLINK, 0);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 27: // Reverse off
      state->pen.ಹಿಮ್ಮುಖ = 0;
      setpenattr_bool(state, VTERM_ATTR_ಹಿಮ್ಮುಖ, 0);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 28: // Conceal off (Reveal)
      state->pen.conceal = 0;
      setpenattr_bool(state, VTERM_ATTR_CONCEAL, 0);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 29: // Strikethrough off
      state->pen.strike = 0;
      setpenattr_bool(state, VTERM_ATTR_STRIKE, 0);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 30: ಸ್ಥಿತಿ 31: ಸ್ಥಿತಿ 32: ಸ್ಥಿತಿ 33:
    ಸ್ಥಿತಿ 34: ಸ್ಥಿತಿ 35: ಸ್ಥಿತಿ 36: ಸ್ಥಿತಿ 37: // Foreground colour palette
      ಮೌಲ್ಯ = CSI_ARG(ವಾದಗಳು[argi]) - 30;
      ಆದರೆ(state->pen.bold && state->bold_is_highbright)
        ಮೌಲ್ಯ += 8;
      ಗಣ_pen_ಲಂಬಸಾಲು_ansi(state, VTERM_ATTR_FOREGROUND, ಮೌಲ್ಯ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 38: // Foreground colour alternative palette
      ಆದರೆ(argcount - argi < 1)
        ಹಿಂತಿರುಗಿ;
      argi += 1 + lookup_ಬಣ್ಣ(state, CSI_ARG(ವಾದಗಳು[argi+1]), ವಾದಗಳು+argi+2, argcount-argi-2, &state->pen.fg);
      setpenattr_ಲಂಬಸಾಲು(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 39: // Foreground colour default
      state->pen.fg = state->default_fg;
      setpenattr_ಲಂಬಸಾಲು(state, VTERM_ATTR_FOREGROUND, state->pen.fg);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 40: ಸ್ಥಿತಿ 41: ಸ್ಥಿತಿ 42: ಸ್ಥಿತಿ 43:
    ಸ್ಥಿತಿ 44: ಸ್ಥಿತಿ 45: ಸ್ಥಿತಿ 46: ಸ್ಥಿತಿ 47: // Background colour palette
      ಮೌಲ್ಯ = CSI_ARG(ವಾದಗಳು[argi]) - 40;
      ಗಣ_pen_ಲಂಬಸಾಲು_ansi(state, VTERM_ATTR_BACKGROUND, ಮೌಲ್ಯ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 48: // Background colour alternative palette
      ಆದರೆ(argcount - argi < 1)
        ಹಿಂತಿರುಗಿ;
      argi += 1 + lookup_ಬಣ್ಣ(state, CSI_ARG(ವಾದಗಳು[argi+1]), ವಾದಗಳು+argi+2, argcount-argi-2, &state->pen.bg);
      setpenattr_ಲಂಬಸಾಲು(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 49: // Default background
      state->pen.bg = state->default_bg;
      setpenattr_ಲಂಬಸಾಲು(state, VTERM_ATTR_BACKGROUND, state->pen.bg);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 73: // Superscript
    ಸ್ಥಿತಿ 74: // Subscript
    ಸ್ಥಿತಿ 75: // Superscript/subscript off
      state->pen.small = (arg != 75);
      state->pen.baseline =
        (arg == 73) ? VTERM_BASELINE_RAISE :
        (arg == 74) ? VTERM_BASELINE_ಅಧೋ :
                      VTERM_BASELINE_ಸಾಮಾನ್ಯ;
      setpenattr_bool(state, VTERM_ATTR_SMALL,    state->pen.small);
      setpenattr_int (state, VTERM_ATTR_BASELINE, state->pen.baseline);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 90: ಸ್ಥಿತಿ 91: ಸ್ಥಿತಿ 92: ಸ್ಥಿತಿ 93:
    ಸ್ಥಿತಿ 94: ಸ್ಥಿತಿ 95: ಸ್ಥಿತಿ 96: ಸ್ಥಿತಿ 97: // Foreground colour high-intensity palette
      ಮೌಲ್ಯ = CSI_ARG(ವಾದಗಳು[argi]) - 90 + 8;
      ಗಣ_pen_ಲಂಬಸಾಲು_ansi(state, VTERM_ATTR_FOREGROUND, ಮೌಲ್ಯ);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 100: ಸ್ಥಿತಿ 101: ಸ್ಥಿತಿ 102: ಸ್ಥಿತಿ 103:
    ಸ್ಥಿತಿ 104: ಸ್ಥಿತಿ 105: ಸ್ಥಿತಿ 106: ಸ್ಥಿತಿ 107: // Background colour high-intensity palette
      ಮೌಲ್ಯ = CSI_ARG(ವಾದಗಳು[argi]) - 100 + 8;
      ಗಣ_pen_ಲಂಬಸಾಲು_ansi(state, VTERM_ATTR_BACKGROUND, ಮೌಲ್ಯ);
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      ಮುಗಿದ = 0;
      ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ(!ಮುಗಿದ)
    {
      DEBUG_LOG1("libvterm: Unhandled CSI SGR %ld\n", arg);
    }

    ಆಗ(CSI_ARG_HAS_ಇನ್ನಷ್ಟು(ವಾದಗಳು[argi++]))
      ;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vterm_state_getpen_color(ಸ್ಥಿರಾಂಕ VTermColor *ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ argi, ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ fg)
{
    /* Do nothing if the given color is the default color */
    ಆದರೆ (( fg && VTERM_COLOR_IS_DEFAULT_FG(ಲಂಬಸಾಲು)) ||
        (!fg && VTERM_COLOR_IS_DEFAULT_BG(ಲಂಬಸಾಲು))) {
        ಹಿಂತಿರುಗಿ argi;
    }

    /* Decide whether to send an indexed color or an RGB color */
    ಆದರೆ (VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(ಲಂಬಸಾಲು)) {
        ಸ್ಥಿರಾಂಕ uint8_t idx = ಲಂಬಸಾಲು->ಸೂಚ್ಯಂಕ;
        ಆದರೆ (idx < 8) {
            ವಾದಗಳು[argi++] = (idx + (fg ? 30 : 40));
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (idx < 16) {
            ವಾದಗಳು[argi++] = (idx - 8 + (fg ? 90 : 100));
        }
        ಇಲ್ಲದಿದ್ದರೆ {
            ವಾದಗಳು[argi++] = CSI_ARG_FLAG_ಇನ್ನಷ್ಟು | (fg ? 38 : 48);
            ವಾದಗಳು[argi++] = CSI_ARG_FLAG_ಇನ್ನಷ್ಟು | 5;
            ವಾದಗಳು[argi++] = idx;
        }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VTERM_COLOR_IS_RGB(ಲಂಬಸಾಲು)) {
        ವಾದಗಳು[argi++] = CSI_ARG_FLAG_ಇನ್ನಷ್ಟು | (fg ? 38 : 48);
        ವಾದಗಳು[argi++] = CSI_ARG_FLAG_ಇನ್ನಷ್ಟು | 2;
        ವಾದಗಳು[argi++] = CSI_ARG_FLAG_ಇನ್ನಷ್ಟು | ಲಂಬಸಾಲು->red;
        ವಾದಗಳು[argi++] = CSI_ARG_FLAG_ಇನ್ನಷ್ಟು | ಲಂಬಸಾಲು->green;
        ವಾದಗಳು[argi++] = ಲಂಬಸಾಲು->blue;
    }
    ಹಿಂತಿರುಗಿ argi;
}

ಆಂತರಿಕ ಪೂರ್ಣಾಂಕ vterm_state_getpen(VTermState *state, ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount ಬಳಸದ)
{
  ಪೂರ್ಣಾಂಕ argi = 0;

  ಆದರೆ(state->pen.bold)
    ವಾದಗಳು[argi++] = 1;

  ಆದರೆ(state->pen.italic)
    ವಾದಗಳು[argi++] = 3;

  ಆದರೆ(state->pen.underline == VTERM_UNDERLINE_SINGLE)
    ವಾದಗಳು[argi++] = 4;
  ಆದರೆ(state->pen.underline == VTERM_UNDERLINE_CURLY)
    ವಾದಗಳು[argi++] = 4 | CSI_ARG_FLAG_ಇನ್ನಷ್ಟು, ವಾದಗಳು[argi++] = 3;

  ಆದರೆ(state->pen.blink)
    ವಾದಗಳು[argi++] = 5;

  ಆದರೆ(state->pen.ಹಿಮ್ಮುಖ)
    ವಾದಗಳು[argi++] = 7;

  ಆದರೆ(state->pen.conceal)
    ವಾದಗಳು[argi++] = 8;

  ಆದರೆ(state->pen.strike)
    ವಾದಗಳು[argi++] = 9;

  ಆದರೆ(state->pen.font)
    ವಾದಗಳು[argi++] = 10 + state->pen.font;

  ಆದರೆ(state->pen.underline == VTERM_UNDERLINE_DOUBLE)
    ವಾದಗಳು[argi++] = 21;

  argi = vterm_state_getpen_color(&state->pen.fg, argi, ವಾದಗಳು, TRUE);

  argi = vterm_state_getpen_color(&state->pen.bg, argi, ವಾದಗಳು, FALSE);

  ಆದರೆ(state->pen.small) {
    ಆದರೆ(state->pen.baseline == VTERM_BASELINE_RAISE)
      ವಾದಗಳು[argi++] = 73;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state->pen.baseline == VTERM_BASELINE_ಅಧೋ)
      ವಾದಗಳು[argi++] = 74;
  }

  ಹಿಂತಿರುಗಿ argi;
}

ಪೂರ್ಣಾಂಕ vterm_state_get_penattr(ಸ್ಥಿರಾಂಕ VTermState *state, VTermAttr attr, VTerm_ಮೌಲ್ಯ *val)
{
  ಆಯ್ಕೆ(attr) {
  ಸ್ಥಿತಿ VTERM_ATTR_BOLD:
    val->boolean = state->pen.bold;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_UNDERLINE:
    val->ಸಂಖ್ಯೆ = state->pen.underline;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_ITALIC:
    val->boolean = state->pen.italic;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_BLINK:
    val->boolean = state->pen.blink;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_ಹಿಮ್ಮುಖ:
    val->boolean = state->pen.ಹಿಮ್ಮುಖ;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_CONCEAL:
    val->boolean = state->pen.conceal;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_STRIKE:
    val->boolean = state->pen.strike;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_FONT:
    val->ಸಂಖ್ಯೆ = state->pen.font;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_FOREGROUND:
    val->color = state->pen.fg;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_BACKGROUND:
    val->color = state->pen.bg;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_SMALL:
    val->boolean = state->pen.small;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_ATTR_BASELINE:
    val->ಸಂಖ್ಯೆ = state->pen.baseline;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_N_ATTRS:
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ 0;
}
