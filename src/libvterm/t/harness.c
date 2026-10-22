#ಅನ್ತರ್ಭಾವ "vterm.h"
#ಅನ್ತರ್ಭಾವ "../src/vterm_internal.h" // We pull in some internal bits too

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ವ್ಯಾಖ್ಯಾನಿಸು streq(a,b) (!strcmp(a,b))
#ವ್ಯಾಖ್ಯಾನಿಸು strstartswith(a,b) (!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(a,b,ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(b)))

ಸ್ಥಿರ size_t inplace_hex2bytes(ಅಕ್ಷರ *s)
{
  ಅಕ್ಷರ *inpos = s, *outpos = s;

  ಆಗ(*inpos) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ch;
    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(inpos, "%2x", &ch) < 1)
      ನಿಲ್ಲಿಸು;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ಹಿಂತಿರುಗಿ outpos - s;
}

ಸ್ಥಿರ VTermModifier strpe_modifiers(ಅಕ್ಷರ **strp)
{
  VTermModifier state = 0;

  ಆಗ((*strp)[0]) {
    ಆಯ್ಕೆ(((*strp)++)[0]) {
      ಸ್ಥಿತಿ 'S': state |= VTERM_MOD_ಸರಿಸು; ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 'C': state |= VTERM_MOD_CTRL;  ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 'A': state |= VTERM_MOD_ALT;   ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ state;
    }
  }

  ಹಿಂತಿರುಗಿ state;
}

ಸ್ಥಿರ VTerm_ಕೀಲಿ strp_ಕೀಲಿ(ಅಕ್ಷರ *str)
{
  ಸ್ಥಿರ ರಚನೆ {
    ಅಕ್ಷರ *name;
    VTerm_ಕೀಲಿ ಕೀಲಿ;
  } ಕೀಲಿಗಳು[] = {
    { "Up",    VTERM_ಕೀಲಿ_ಮೇಲೆ },
    { "Tab",   VTERM_ಕೀಲಿ_TAB },
    { "Enter", VTERM_ಕೀಲಿ_ENTER },
    { "KP0",   VTERM_ಕೀಲಿ_KP_0 },
    { "F1",    VTERM_ಕೀಲಿ_FUNCTION(1) },
    { ಖಾಲಿ,    VTERM_ಕೀಲಿ_NONE },
  };
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(i = 0; ಕೀಲಿಗಳು[i].name; i++) {
    ಆದರೆ(streq(str, ಕೀಲಿಗಳು[i].name))
      ಹಿಂತಿರುಗಿ ಕೀಲಿಗಳು[i].ಕೀಲಿ;
  }

  ಹಿಂತಿರುಗಿ VTERM_ಕೀಲಿ_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ print_color(ಸ್ಥಿರಾಂಕ VTermColor *ಲಂಬಸಾಲು)
{
  ಆದರೆ (VTERM_COLOR_IS_RGB(ಲಂಬಸಾಲು)) {
    ಬರೆ("rgb(%d,%d,%d", ಲಂಬಸಾಲು->red, ಲಂಬಸಾಲು->green, ಲಂಬಸಾಲು->blue);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (VTERM_COLOR_IS_ಸೂಚ್ಯಂಕಿತ(ಲಂಬಸಾಲು)) {
    ಬರೆ("idx(%d", ಲಂಬಸಾಲು->ಸೂಚ್ಯಂಕ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಬರೆ("invalid(%d", ಲಂಬಸಾಲು->type);
  }
  ಆದರೆ (VTERM_COLOR_IS_DEFAULT_FG(ಲಂಬಸಾಲು)) {
    ಬರೆ(",is_default_fg");
  }
  ಆದರೆ (VTERM_COLOR_IS_DEFAULT_BG(ಲಂಬಸಾಲು)) {
    ಬರೆ(",is_default_bg");
  }
  ಬರೆ(")");
}

ಸ್ಥಿರ VTermColor strpe_color(ಅಕ್ಷರ **strp)
{
  uint8_t r, g, b, idx;
  ಪೂರ್ಣಾಂಕ len = 0;
  VTermColor ಲಂಬಸಾಲು;

  ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&ಲಂಬಸಾಲು, r, g, b);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_ಸೂಚ್ಯಂಕಿತ(&ಲಂಬಸಾಲು, idx);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    vterm_color_rgb(&ಲಂಬಸಾಲು, 127, 127, 127);

  ಹಿಂತಿರುಗಿ ಲಂಬಸಾಲು;
}

ಸ್ಥಿರ VTerm *vt;
ಸ್ಥಿರ VTermState *state;
ಸ್ಥಿರ VTermScreen *screen;

ಸ್ಥಿರ VTermEncodingInstance encoding;

ಸ್ಥಿರ ಶೂನ್ಯ ಪದ_output(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len, ಶೂನ್ಯ *user ಬಳಸದ)
{
  size_t i;

  ಬರೆ("output ");
  ಗಾಗಿ(i = 0; i < len; i++)
    ಬರೆ("%x%s", (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)s[i], i < len-1 ? "," : "\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ printhex(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t len)
{
  ಆಗ(len--)
    ಬರೆ("%02x", (uint8_t)(s++)[0]);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬೈಟುಗಳು[], size_t len, ಶೂನ್ಯ *user ಬಳಸದ)
{
  size_t i;

  ಬರೆ("text ");
  ಗಾಗಿ(i = 0; i < len; i++) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ b = ಬೈಟುಗಳು[i];
    ಆದರೆ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      ನಿಲ್ಲಿಸು;
    ಬರೆ(i ? ",%x" : "%x", b);
  }
  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ i;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_control(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ control, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಬರೆ("control %02x\n", control);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬೈಟುಗಳು[], size_t len, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆದರೆ(ಬೈಟುಗಳು[0] >= 0x20 && ಬೈಟುಗಳು[0] < 0x30) {
    ಆದರೆ(len < 2)
      ಹಿಂತಿರುಗಿ -1;
    len = 2;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    len = 1;
  }

  ಬರೆ("escape ");
  printhex(ಬೈಟುಗಳು, len);
  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ len;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_csi(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *leader, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *intermed, ಅಕ್ಷರ command, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಪೂರ್ಣಾಂಕ i;
  ಬರೆ("csi %02x", command);

  ಆದರೆ(leader && leader[0]) {
    ಬರೆ(" L=");
    ಗಾಗಿ(i = 0; leader[i]; i++)
      ಬರೆ("%02x", leader[i]);
  }

  ಗಾಗಿ(i = 0; i < argcount; i++) {
    ಅಕ್ಷರ sep = i ? ',' : ' ';

    ಆದರೆ(ವಾದಗಳು[i] == CSI_ARG_MISSING)
      ಬರೆ("%c*", sep);
    ಇಲ್ಲದಿದ್ದರೆ
      ಬರೆ("%c%ld%s", sep, CSI_ARG(ವಾದಗಳು[i]), CSI_ARG_HAS_ಇನ್ನಷ್ಟು(ವಾದಗಳು[i]) ? "+" : "");
  }

  ಆದರೆ(intermed && intermed[0]) {
    ಬರೆ(" I=");
    ಗಾಗಿ(i = 0; intermed[i]; i++)
      ಬರೆ("%02x", intermed[i]);
  }

  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_osc(ಪೂರ್ಣಾಂಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user ಬಳಸದ)
{

  ಬರೆ("osc ");

  ಆದರೆ(frag.initial) {
    ಆದರೆ(command == -1)
      ಬರೆ("[");
    ಇಲ್ಲದಿದ್ದರೆ
      ಬರೆ("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ಆದರೆ(frag.final)
    ಬರೆ("]");

  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_dcs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಬರೆ("dcs ");

  ಆದರೆ(frag.initial) {
    size_t i;
    ಬರೆ("[");
    ಗಾಗಿ(i = 0; i < commandlen; i++)
      ಬರೆ("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ಆದರೆ(frag.final)
    ಬರೆ("]");

  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_apc(VTermStringFragment frag, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಬರೆ("apc ");

  ಆದರೆ(frag.initial)
    ಬರೆ("[");

  printhex(frag.str, frag.len);

  ಆದರೆ(frag.final)
    ಬರೆ("]");

  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_pm(VTermStringFragment frag, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಬರೆ("pm ");

  ಆದರೆ(frag.initial)
    ಬರೆ("[");

  printhex(frag.str, frag.len);

  ಆದರೆ(frag.final)
    ಬರೆ("]");

  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parser_sos(VTermStringFragment frag, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಬರೆ("sos ");

  ಆದರೆ(frag.initial)
    ಬರೆ("[");

  printhex(frag.str, frag.len);

  ಆದರೆ(frag.final)
    ಬರೆ("]");

  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ VTermParserCallbacks parser_cbs = {
  parser_ಪಠ್ಯ, //ಪಠ್ಯ
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ಖಾಲಿ // resize
};

ಸ್ಥಿರ VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_movecursor = 0;
ಸ್ಥಿರ VTermPos state_pos;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ movecursor(VTermPos pos, VTermPos oldpos ಬಳಸದ, ಪೂರ್ಣಾಂಕ visible ಬಳಸದ, ಶೂನ್ಯ *user ಬಳಸದ)
{
  state_pos = pos;

  ಆದರೆ(want_movecursor)
    ಬರೆ("movecursor %d,%d\n", pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_premove = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ premove(VTermRect rect, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆದರೆ(!want_premove)
    ಹಿಂತಿರುಗಿ 0;

  ಬರೆ("premove %d..%d,%d..%d\n",
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು, rect.ಆರಂಭ_ಲಂಬಸಾಲು, rect.ಕೊನೆ_ಲಂಬಸಾಲು);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_scrollrect = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ scrollrect(VTermRect rect, ಪೂರ್ಣಾಂಕ downward, ಪೂರ್ಣಾಂಕ rightward, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆದರೆ(!want_scrollrect)
    ಹಿಂತಿರುಗಿ 0;

  ಬರೆ("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು, rect.ಆರಂಭ_ಲಂಬಸಾಲು, rect.ಕೊನೆ_ಲಂಬಸಾಲು,
      downward, rightward);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_moverect = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ moverect(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆದರೆ(!want_moverect)
    ಹಿಂತಿರುಗಿ 0;

  ಬರೆ("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.ಆರಂಭ_ಅಡ್ಡಸಾಲು,  src.ಕೊನೆ_ಅಡ್ಡಸಾಲು,  src.ಆರಂಭ_ಲಂಬಸಾಲು,  src.ಕೊನೆ_ಲಂಬಸಾಲು,
      dest.ಆರಂಭ_ಅಡ್ಡಸಾಲು, dest.ಕೊನೆ_ಅಡ್ಡಸಾಲು, dest.ಆರಂಭ_ಲಂಬಸಾಲು, dest.ಕೊನೆ_ಲಂಬಸಾಲು);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_settermprop = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ settermprop(VTermProp prop, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user ಬಳಸದ)
{
  VTerm_ಮೌಲ್ಯ_Type type;
  ಆದರೆ(!want_settermprop)
    ಹಿಂತಿರುಗಿ 1;

  type = vterm_get_prop_type(prop);
  ಆಯ್ಕೆ(type) {
  ಸ್ಥಿತಿ VTERM_VALUETYPE_BOOL:
    ಬರೆ("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_VALUETYPE_INT:
    ಬರೆ("settermprop %d %d\n", prop, val->ಸಂಖ್ಯೆ);
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_VALUETYPE_STRING:
    ಬರೆ("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_VALUETYPE_COLOR:
    ಬರೆ("settermprop %d ", prop);
    print_color(&val->color);
    ಬರೆ("\n");
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_N_VALUETYPES:
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ 0;
}

// These callbacks are for State

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_state_putglyph = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ state_putglyph(VTermGlyphInfo *info, VTermPos pos, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಪೂರ್ಣಾಂಕ i;
  ಆದರೆ(!want_state_putglyph)
    ಹಿಂತಿರುಗಿ 1;

  ಬರೆ("putglyph ");
  ಗಾಗಿ(i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL && info->chars[i]; i++)
    ಬರೆ(i ? ",%x" : "%x", info->chars[i]);
  ಬರೆ(" %d %d,%d", info->ಅಗಲ, pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು);
  ಆದರೆ(info->protected_cell)
    ಬರೆ(" prot");
  ಆದರೆ(info->dwl)
    ಬರೆ(" dwl");
  ಆದರೆ(info->dhl)
    ಬರೆ(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_state_erase = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ state_erase(VTermRect rect, ಪೂರ್ಣಾಂಕ selective, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆದರೆ(!want_state_erase)
    ಹಿಂತಿರುಗಿ 1;

  ಬರೆ("erase %d..%d,%d..%d%s\n",
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು, rect.ಆರಂಭ_ಲಂಬಸಾಲು, rect.ಕೊನೆ_ಲಂಬಸಾಲು,
      selective ? " selective" : "");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ರಚನೆ {
  ಪೂರ್ಣಾಂಕ bold;
  ಪೂರ್ಣಾಂಕ underline;
  ಪೂರ್ಣಾಂಕ italic;
  ಪೂರ್ಣಾಂಕ blink;
  ಪೂರ್ಣಾಂಕ ಹಿಮ್ಮುಖ;
  ಪೂರ್ಣಾಂಕ conceal;
  ಪೂರ್ಣಾಂಕ strike;
  ಪೂರ್ಣಾಂಕ font;
  ಪೂರ್ಣಾಂಕ small;
  ಪೂರ್ಣಾಂಕ baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ state_setpenattr(VTermAttr attr, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆಯ್ಕೆ(attr) {
  ಸ್ಥಿತಿ VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->ಸಂಖ್ಯೆ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_ಹಿಮ್ಮುಖ:
    state_pen.ಹಿಮ್ಮುಖ = val->boolean;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_FONT:
    state_pen.font = val->ಸಂಖ್ಯೆ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_BASELINE:
    state_pen.baseline = val->ಸಂಖ್ಯೆ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ VTERM_N_ATTRS:
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ state_setlineinfo(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು ಬಳಸದ, ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *newinfo ಬಳಸದ, ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *oldinfo ಬಳಸದ, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_state_scrollback = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ state_sb_clear(ಶೂನ್ಯ *user ಬಳಸದ) {
  ಆದರೆ(!want_state_scrollback)
    ಹಿಂತಿರುಗಿ 1;

  ಬರೆ("sb_clear\n");
  ಹಿಂತಿರುಗಿ 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ಖಾಲಿ, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ಖಾಲಿ, // bell
  ಖಾಲಿ, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ selection_ಗಣ(VTerm_Selection_ಮುಖವಾಡ ಮುಖವಾಡ, VTermStringFragment frag, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಬರೆ("selection-set mask=%04X ", ಮುಖವಾಡ);
  ಆದರೆ(frag.initial)
    ಬರೆ("[");
  printhex(frag.str, frag.len);
  ಆದರೆ(frag.final)
    ಬರೆ("]");
  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ selection_query(VTerm_Selection_ಮುಖವಾಡ ಮುಖವಾಡ, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಬರೆ("selection-query mask=%04X\n", ಮುಖವಾಡ);

  ಹಿಂತಿರುಗಿ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .ಗಣ_2   = selection_ಗಣ,
  .query = selection_query,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_screen_damage = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_screen_damage_cells = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ screen_damage(VTermRect rect, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆದರೆ(!want_screen_damage)
    ಹಿಂತಿರುಗಿ 1;

  ಬರೆ("damage %d..%d,%d..%d",
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು, rect.ಆರಂಭ_ಲಂಬಸಾಲು, rect.ಕೊನೆ_ಲಂಬಸಾಲು);

  ಆದರೆ(want_screen_damage_cells) {
    ಪೂರ್ಣಾಂಕ equals = FALSE;
    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;

    ಗಾಗಿ(ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++) {
      ಪೂರ್ಣಾಂಕ eol = rect.ಕೊನೆ_ಲಂಬಸಾಲು;
      ಆಗ(eol > rect.ಆರಂಭ_ಲಂಬಸಾಲು) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	pos.ಲಂಬಸಾಲು = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ಆದರೆ(cell.chars[0])
          ನಿಲ್ಲಿಸು;

        eol--;
      }

      ಆದರೆ(eol == rect.ಆರಂಭ_ಲಂಬಸಾಲು)
        ನಿಲ್ಲಿಸು;

      ಆದರೆ(!equals)
        ಬರೆ(" ="), equals = TRUE;

      ಬರೆ(" %d<", ಅಡ್ಡಸಾಲು);
      ಗಾಗಿ(ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು; ಲಂಬಸಾಲು < eol; ಲಂಬಸಾಲು++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
	pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
        vterm_screen_get_cell(screen, pos, &cell);
        ಬರೆ(ಲಂಬಸಾಲು == rect.ಆರಂಭ_ಲಂಬಸಾಲು ? "%02X" : " %02X", cell.chars[0]);
      }
      ಬರೆ(">");
    }
  }

  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ want_screen_scrollback = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ screen_sb_pushline4(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಸ್ಥಿರಾಂಕ VTermScreenCell *cells, ಪೂರ್ಣಾಂಕ continuation, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಪೂರ್ಣಾಂಕ eol;
  ಪೂರ್ಣಾಂಕ c;

  ಆದರೆ(!want_screen_scrollback)
    ಹಿಂತಿರುಗಿ 1;

  eol = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
  ಆಗ(eol && !cells[eol-1].chars[0])
    eol--;

  ಬರೆ("sb_pushline %d%s =", ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, continuation ? " cont" : "");
  ಗಾಗಿ(c = 0; c < eol; c++)
    ಬರೆ(" %02X", cells[c].chars[0]);
  ಬರೆ("\n");

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ screen_sb_popline(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, VTermScreenCell *cells, ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;

  ಆದರೆ(!want_screen_scrollback)
    ಹಿಂತಿರುಗಿ 0;

  // All lines of scrollback contain "ABCDE"
  ಗಾಗಿ(ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++) {
    ಆದರೆ(ಲಂಬಸಾಲು < 5)
      cells[ಲಂಬಸಾಲು].chars[0] = 'A' + ಲಂಬಸಾಲು;
    ಇಲ್ಲದಿದ್ದರೆ
      cells[ಲಂಬಸಾಲು].chars[0] = 0;

    cells[ಲಂಬಸಾಲು].ಅಗಲ = 1;
  }

  ಬರೆ("sb_popline %d\n", ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ screen_sb_clear(ಶೂನ್ಯ *user ಬಳಸದ)
{
  ಆದರೆ(!want_screen_scrollback)
    ಹಿಂತಿರುಗಿ 1;

  ಬರೆ("sb_clear\n");
  ಹಿಂತಿರುಗಿ 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ಖಾಲಿ, // bell
  ಖಾಲಿ, // resize
  ಖಾಲಿ, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

ಪೂರ್ಣಾಂಕ ಮುಖ್ಯ(ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, ಅಕ್ಷರ **argv ಬಳಸದ)
{
  ಅಕ್ಷರ ಸಾಲು[1024] = {0};
  ಪೂರ್ಣಾಂಕ flag;

  ಪೂರ್ಣಾಂಕ err;

  setvbuf(stdout, ಖಾಲಿ, _IONBF, 0);

  ಆಗ(fgets(ಸಾಲು, ಗಾತ್ರ ಸಾಲು, stdin)) {
    ಅಕ್ಷರ *nl;
    size_t outlen;
    err = 0;

    ಆದರೆ((nl = strchr(ಸಾಲು, '\n')))
      *nl = '\0';

    ಆದರೆ(streq(ಸಾಲು, "INIT")) {
      ಆದರೆ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(ಸಾಲು, "WANTPARSER")) {
      ಪ್ರತಿಜ್ಞಾ(vt);
      vterm_parser_ಗಣ_callbacks(vt, &parser_cbs, ಖಾಲಿ);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "WANTSTATE") && (ಸಾಲು[9] == '\0' || ಸಾಲು[9] == ' ')) {
      ಪೂರ್ಣಾಂಕ i = 9;
      ಪೂರ್ಣಾಂಕ sense = 1;
      ಪ್ರತಿಜ್ಞಾ(vt);
      ಆದರೆ(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_ಗಣ_callbacks(state, &state_cbs, ಖಾಲಿ);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_ಗಣ_selection_callbacks(state, &selection_cbs, ಖಾಲಿ, ಖಾಲಿ, 16);
        vterm_state_ಗಣ_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      ಆಗ(ಸಾಲು[i] == ' ')
        i++;
      ಗಾಗಿ( ; ಸಾಲು[i]; i++)
        ಆಯ್ಕೆ(ಸಾಲು[i]) {
        ಸ್ಥಿತಿ '+':
          sense = 1;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ '-':
          sense = 0;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'g':
          want_state_putglyph = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 's':
          want_scrollrect = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'P':
          want_premove = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'm':
          want_moverect = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'e':
          want_state_erase = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'p':
          want_settermprop = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'f':
          vterm_state_ಗಣ_unrecognised_fallbacks(state, sense ? &fallbacks : ಖಾಲಿ, ಖಾಲಿ);
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'b':
          want_state_scrollback = sense;
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", ಸಾಲು[i]);
        }
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "WANTSCREEN") && (ಸಾಲು[10] == '\0' || ಸಾಲು[10] == ' ')) {
      ಪೂರ್ಣಾಂಕ i = 10;
      ಪೂರ್ಣಾಂಕ sense = 1;
      ಪ್ರತಿಜ್ಞಾ(vt);
      ಆದರೆ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_ಗಣ_callbacks(screen, &screen_cbs, ಖಾಲಿ);
      vterm_screen_callbacks_has_pushline4(screen);

      ಆಗ(ಸಾಲು[i] == ' ')
        i++;
      ಗಾಗಿ( ; ಸಾಲು[i]; i++)
        ಆಯ್ಕೆ(ಸಾಲು[i]) {
        ಸ್ಥಿತಿ '-':
          sense = 0;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'a':
          vterm_screen_enable_altscreen(screen, 1);
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'd':
          want_screen_damage = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'm':
          want_moverect = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'c':
          want_movecursor = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'p':
          want_settermprop = 1;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'b':
          want_screen_scrollback = sense;
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 'r':
          vterm_screen_enable_reflow(screen, sense);
          ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", ಸಾಲು[i]);
        }
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(ಸಾಲು, "UTF8 %d", &flag)) {
      vterm_ಗಣ_utf8(vt, flag);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(ಸಾಲು, "RESET")) {
      ಆದರೆ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ಆದರೆ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "RESIZE ")) {
      ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
      ಅಕ್ಷರ *linep = ಸಾಲು + 7;
      ಆಗ(linep[0] == ' ')
        linep++;
      ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d, %d", &ಅಡ್ಡಸಾಲುಗಳು_2, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
      vterm_ಗಣ_ಗಾತ್ರ(vt, ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "PUSH ")) {
      ಅಕ್ಷರ *ಬೈಟುಗಳು = ಸಾಲು + 5;
      size_t len = inplace_hex2bytes(ಬೈಟುಗಳು);
      ಪ್ರತಿಜ್ಞಾ(len);

      size_t written = vterm_ಒಳಹರಿವು_write(vt, ಬೈಟುಗಳು, len);
      ಆದರೆ(written < len)
        fprintf(stderr, "! short write\n");
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(ಸಾಲು, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ಆದರೆ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.ದತ್ತಾಂಶ);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "ENCIN ")) {
      ಅಕ್ಷರ *ಬೈಟುಗಳು = ಸಾಲು + 6;
      size_t len = inplace_hex2bytes(ಬೈಟುಗಳು);
      ಪ್ರತಿಜ್ಞಾ(len);

      uint32_t cp[1024];
      ಪೂರ್ಣಾಂಕ cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.ದತ್ತಾಂಶ,
          cp, &cpi, len, ಬೈಟುಗಳು, &pos, len);

      ಆದರೆ(cpi > 0) {
	ಪೂರ್ಣಾಂಕ i;
        ಬರೆ("encout ");
        ಗಾಗಿ(i = 0; i < cpi; i++) {
          ಬರೆ(i ? ",%x" : "%x", cp[i]);
        }
        ಬರೆ("\n");
      }
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "INCHAR ")) {
      ಅಕ್ಷರ *linep = ಸಾಲು + 7;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ c = 0;
      VTermModifier mod;
      ಆಗ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ಅಕ್ಷರಸರಣಿ_ಓದು(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "INKEY ")) {
      VTermModifier mod;
      VTerm_ಕೀಲಿ ಕೀಲಿ;
      ಅಕ್ಷರ *linep = ಸಾಲು + 6;
      ಆಗ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ಆಗ(linep[0] == ' ')
        linep++;
      ಕೀಲಿ = strp_ಕೀಲಿ(linep);

      vterm_keyboard_ಕೀಲಿ(vt, ಕೀಲಿ, mod);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "PASTE ")) {
      ಅಕ್ಷರ *linep = ಸಾಲು + 6;
      ಆದರೆ(streq(linep, "START"))
        vterm_keyboard_ಆರಂಭ_paste(vt);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "END"))
        vterm_keyboard_ಕೊನೆ_paste(vt);
      ಇಲ್ಲದಿದ್ದರೆ
        ಹೋಗು abort_ಸಾಲು;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "FOCUS ")) {
      ಪ್ರತಿಜ್ಞಾ(state);
      ಅಕ್ಷರ *linep = ಸಾಲು + 6;
      ಆದರೆ(streq(linep, "IN"))
        vterm_state_focus_ಒಳ(state);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "OUT"))
        vterm_state_focus_ಹೊರಹೋಗು(state);
      ಇಲ್ಲದಿದ್ದರೆ
        ಹೋಗು abort_ಸಾಲು;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "MOUSEMOVE ")) {
      ಅಕ್ಷರ *linep = ಸಾಲು + 10;
      ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, len;
      VTermModifier mod;
      ಆಗ(linep[0] == ' ')
        linep++;
      ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d,%d%n", &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &len);
      linep += len;
      ಆಗ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, mod);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "MOUSEBTN ")) {
      ಅಕ್ಷರ *linep = ಸಾಲು + 9;
      ಅಕ್ಷರ press;
      ಪೂರ್ಣಾಂಕ button, len;
      VTermModifier mod;
      ಆಗ(linep[0] == ' ')
        linep++;
      ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ಆಗ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "SELECTION ")) {
      ಅಕ್ಷರ *linep = ಸಾಲು + 10;
      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಮುಖವಾಡ;
      ಪೂರ್ಣಾಂಕ len;
      VTermStringFragment frag = { 0 };
      ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%x%n", &ಮುಖವಾಡ, &len);
      linep += len;
      ಆಗ(linep[0] == ' ')
        linep++;
      ಆದರೆ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ಆಗ(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      ಪ್ರತಿಜ್ಞಾ(frag.len);

      linep += frag.len * 2;
      ಆಗ(linep[0] == ' ')
        linep++;
      ಆದರೆ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, ಮುಖವಾಡ, frag);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "DAMAGEMERGE ")) {
      ಪ್ರತಿಜ್ಞಾ(screen);
      ಅಕ್ಷರ *linep = ಸಾಲು + 12;
      ಆಗ(linep[0] == ' ')
        linep++;
      ಆದರೆ(streq(linep, "CELL"))
        vterm_screen_ಗಣ_damage_ವಿಲೀನ(screen, VTERM_DAMAGE_CELL);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "ROW"))
        vterm_screen_ಗಣ_damage_ವಿಲೀನ(screen, VTERM_DAMAGE_ಅಡ್ಡಸಾಲು);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "SCREEN"))
        vterm_screen_ಗಣ_damage_ವಿಲೀನ(screen, VTERM_DAMAGE_SCREEN);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "SCROLL"))
        vterm_screen_ಗಣ_damage_ವಿಲೀನ(screen, VTERM_DAMAGE_SCROLL);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "DAMAGEFLUSH")) {
      ಪ್ರತಿಜ್ಞಾ(screen);
      vterm_screen_flush_damage(screen);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "SETDEFAULTCOL ")) {
      ಪ್ರತಿಜ್ಞಾ(screen);
      ಅಕ್ಷರ *linep = ಸಾಲು + 14;
      ಆಗ(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ಆದರೆ(linep[0]) {
        ಆಗ(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_ಗಣ_default_colors(screen, &fg, &bg);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        vterm_screen_ಗಣ_default_colors(screen, &fg, ಖಾಲಿ);
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಸಾಲು[0] == '?') {
      ಆದರೆ(streq(ಸಾಲು, "?cursor")) {
        ಪ್ರತಿಜ್ಞಾ(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ಆದರೆ(pos.ಅಡ್ಡಸಾಲು != state_pos.ಅಡ್ಡಸಾಲು)
          ಬರೆ("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು, state_pos.ಅಡ್ಡಸಾಲು, state_pos.ಲಂಬಸಾಲು);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(pos.ಲಂಬಸಾಲು != state_pos.ಲಂಬಸಾಲು)
          ಬರೆ("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.ಅಡ್ಡಸಾಲು, pos.ಲಂಬಸಾಲು, state_pos.ಅಡ್ಡಸಾಲು, state_pos.ಲಂಬಸಾಲು);
        ಇಲ್ಲದಿದ್ದರೆ
          ಬರೆ("%d,%d\n", state_pos.ಅಡ್ಡಸಾಲು, state_pos.ಲಂಬಸಾಲು);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "?pen ")) {
        ಪ್ರತಿಜ್ಞಾ(state);
        VTerm_ಮೌಲ್ಯ val;
        ಅಕ್ಷರ *linep = ಸಾಲು + 5;
        ಆಗ(linep[0] == ' ')
          linep++;

#ವ್ಯಾಖ್ಯಾನಿಸು BOOLSTR(v) ((v) ? "on" : "off")

        ಆದರೆ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ಆದರೆ(val.boolean != state_pen.bold)
            ಬರೆ("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%s\n", BOOLSTR(state_pen.bold));
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ಆದರೆ(val.boolean != state_pen.underline)
            ಬರೆ("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%d\n", state_pen.underline);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ಆದರೆ(val.boolean != state_pen.italic)
            ಬರೆ("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%s\n", BOOLSTR(state_pen.italic));
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ಆದರೆ(val.boolean != state_pen.blink)
            ಬರೆ("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%s\n", BOOLSTR(state_pen.blink));
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ಹಿಮ್ಮುಖ, &val);
          ಆದರೆ(val.boolean != state_pen.ಹಿಮ್ಮುಖ)
            ಬರೆ("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.ಹಿಮ್ಮುಖ));
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%s\n", BOOLSTR(state_pen.ಹಿಮ್ಮುಖ));
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ಆದರೆ(val.boolean != state_pen.font)
            ಬರೆ("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%d\n", state_pen.font);
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ಆದರೆ(val.boolean != state_pen.small)
            ಬರೆ("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%s\n", BOOLSTR(state_pen.small));
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ಆದರೆ(val.ಸಂಖ್ಯೆ != state_pen.baseline)
            ಬರೆ("! pen baseline mismatch: state=%d, event=%d\n",
                val.ಸಂಖ್ಯೆ, state_pen.baseline);
          ಇಲ್ಲದಿದ್ದರೆ
            ಬರೆ("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_ಅಧೋ ? "lower"
                         : "normal");
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          ಬರೆ("\n");
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(streq(linep, "background")) {
          print_color(&state_pen.background);
          ಬರೆ("\n");
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಬರೆ("?\n");
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "?lineinfo ")) {
        ಪ್ರತಿಜ್ಞಾ(state);
        ಅಕ್ಷರ *linep = ಸಾಲು + 10;
        ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;
        ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *info;
        ಆಗ(linep[0] == ' ')
          linep++;
        ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d", &ಅಡ್ಡಸಾಲು) < 1) {
          ಬರೆ("! lineinfo unrecognised input\n");
          ಹೋಗು abort_ಸಾಲು;
        }
        info = vterm_state_get_lineinfo(state, ಅಡ್ಡಸಾಲು);
        ಆದರೆ(info->doublewidth)
          ಬರೆ("dwl ");
        ಆದರೆ(info->doubleheight)
          ಬರೆ("dhl ");
        ಆದರೆ(info->continuation)
          ಬರೆ("cont ");
        ಬರೆ("\n");
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "?screen_chars ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        ಅಕ್ಷರ *linep = ಸಾಲು + 13;
        VTermRect rect;
        size_t len;
        ಆಗ(linep[0] == ' ')
          linep++;
        ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d,%d,%d,%d", &rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, &rect.ಆರಂಭ_ಲಂಬಸಾಲು, &rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು, &rect.ಕೊನೆ_ಲಂಬಸಾಲು) == 4)
          ; // fine
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d", &rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು) == 1) {
          rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು + 1;
          rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0;
          vterm_get_ಗಾತ್ರ(vt, ಖಾಲಿ, &rect.ಕೊನೆ_ಲಂಬಸಾಲು);
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          ಬರೆ("! screen_chars unrecognised input\n");
          ಹೋಗು abort_ಸಾಲು;
        }
        len = vterm_screen_get_chars(screen, ಖಾಲಿ, 0, rect);
        ಆದರೆ(len == (size_t)-1)
          ಬರೆ("! screen_chars error\n");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len == 0)
          ಬರೆ("\n");
        ಇಲ್ಲದಿದ್ದರೆ {
          uint32_t *chars = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          ಗಾಗಿ(i = 0; i < len; i++) {
            ಬರೆ("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          ಬಿಡುಗಡೆ(chars);
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "?screen_text ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        ಅಕ್ಷರ *linep = ಸಾಲು + 12;
        VTermRect rect;
        size_t len;
        ಆಗ(linep[0] == ' ')
          linep++;
        ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d,%d,%d,%d", &rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, &rect.ಆರಂಭ_ಲಂಬಸಾಲು, &rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು, &rect.ಕೊನೆ_ಲಂಬಸಾಲು) < 4) {
          ಬರೆ("! screen_text unrecognised input\n");
          ಹೋಗು abort_ಸಾಲು;
        }
        len = vterm_screen_get_ಪಠ್ಯ(screen, ಖಾಲಿ, 0, rect);
        ಆದರೆ(len == (size_t)-1)
          ಬರೆ("! screen_text error\n");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(len == 0)
          ಬರೆ("\n");
        ಇಲ್ಲದಿದ್ದರೆ {
          // Put an overwrite guard at both ends of the buffer
          ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಬಫರ್ = ಸ್ಮರಣೆ_ಕೊಡು(len + 4);
          ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಪಠ್ಯ = ಬಫರ್ + 2;
          ಪಠ್ಯ[-2] = 0x55; ಪಠ್ಯ[-1] = 0xAA;
          ಪಠ್ಯ[len] = 0x55; ಪಠ್ಯ[len+1] = 0xAA;

          vterm_screen_get_ಪಠ್ಯ(screen, (ಅಕ್ಷರ *)ಪಠ್ಯ, len, rect);

          ಆದರೆ(ಪಠ್ಯ[-2] != 0x55 || ಪಠ್ಯ[-1] != 0xAA)
            ಬರೆ("! screen_get_text buffer overrun left [%02x,%02x]\n", ಪಠ್ಯ[-2], ಪಠ್ಯ[-1]);
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಪಠ್ಯ[len] != 0x55 || ಪಠ್ಯ[len+1] != 0xAA)
            ಬರೆ("! screen_get_text buffer overrun right [%02x,%02x]\n", ಪಠ್ಯ[len], ಪಠ್ಯ[len+1]);
          ಇಲ್ಲದಿದ್ದರೆ
	  {
	    size_t i;
            ಗಾಗಿ(i = 0; i < len; i++) {
              ಬರೆ("0x%02x%s", ಪಠ್ಯ[i], i < len-1 ? "," : "\n");
            }
	  }

          ಬಿಡುಗಡೆ(ಬಫರ್);
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "?screen_cell ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        ಅಕ್ಷರ *linep = ಸಾಲು + 12;
	ಪೂರ್ಣಾಂಕ i;
        VTermPos pos;
        VTermScreenCell cell;
        ಆಗ(linep[0] == ' ')
          linep++;
        ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d,%d\n", &pos.ಅಡ್ಡಸಾಲು, &pos.ಲಂಬಸಾಲು) < 2) {
          ಬರೆ("! screen_cell unrecognised input\n");
          ಹೋಗು abort_ಸಾಲು;
        }
        ಆದರೆ(!vterm_screen_get_cell(screen, pos, &cell))
          ಹೋಗು abort_ಸಾಲು;
        ಬರೆ("{");
        ಗಾಗಿ(i = 0; i < VTERM_MAX_CHARS_ಪ್ರತಿ_CELL && cell.chars[i]; i++) {
          ಬರೆ("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        ಬರೆ("} width=%d attrs={", cell.ಅಗಲ);
        ಆದರೆ(cell.attrs.bold)      ಬರೆ("B");
        ಆದರೆ(cell.attrs.underline) ಬರೆ("U%d", cell.attrs.underline);
        ಆದರೆ(cell.attrs.italic)    ಬರೆ("I");
        ಆದರೆ(cell.attrs.blink)     ಬರೆ("K");
        ಆದರೆ(cell.attrs.ಹಿಮ್ಮುಖ)   ಬರೆ("R");
        ಆದರೆ(cell.attrs.font)      ಬರೆ("F%d", cell.attrs.font);
        ಆದರೆ(cell.attrs.small)     ಬರೆ("S");
        ಆದರೆ(cell.attrs.baseline)  ಬರೆ(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        ಬರೆ("} ");
        ಆದರೆ(cell.attrs.dwl)       ಬರೆ("dwl ");
        ಆದರೆ(cell.attrs.dhl)       ಬರೆ("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        ಬರೆ("fg=");
        vterm_screen_convert_color_ಗಮ್ಯ_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        ಬರೆ(" bg=");
        vterm_screen_convert_color_ಗಮ್ಯ_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        ಬರೆ("\n");
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "?screen_eol ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        VTermPos pos;
        ಅಕ್ಷರ *linep = ಸಾಲು + 12;
        ಆಗ(linep[0] == ' ')
          linep++;
        ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d,%d\n", &pos.ಅಡ್ಡಸಾಲು, &pos.ಲಂಬಸಾಲು) < 2) {
          ಬರೆ("! screen_eol unrecognised input\n");
          ಹೋಗು abort_ಸಾಲು;
        }
        ಬರೆ("%d\n", vterm_screen_is_eol(screen, pos));
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(strstartswith(ಸಾಲು, "?screen_attrs_extent ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        VTermPos pos;
        VTermRect rect;
        ಅಕ್ಷರ *linep = ಸಾಲು + 21;
        ಆಗ(linep[0] == ' ')
          linep++;
        ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಓದು(linep, "%d,%d\n", &pos.ಅಡ್ಡಸಾಲು, &pos.ಲಂಬಸಾಲು) < 2) {
          ಬರೆ("! screen_attrs_extent unrecognised input\n");
          ಹೋಗು abort_ಸಾಲು;
        }
	rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0;
	rect.ಕೊನೆ_ಲಂಬಸಾಲು   = -1;
        ಆದರೆ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          ಬರೆ("! screen_attrs_extent failed\n");
          ಹೋಗು abort_ಸಾಲು;
        }
        ಬರೆ("%d,%d-%d,%d\n", rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, rect.ಆರಂಭ_ಲಂಬಸಾಲು, rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು, rect.ಕೊನೆ_ಲಂಬಸಾಲು);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಬರೆ("?\n");

      memset(ಸಾಲು, 0, ಗಾತ್ರ ಸಾಲು);
      ಮುಂದುವರಿ;
    }

    ಇಲ್ಲದಿದ್ದರೆ
      abort_ಸಾಲು: err = 1;

    outlen = vterm_output_get_ಬಫರ್_current(vt);
    ಆದರೆ(outlen > 0) {
      ಅಕ್ಷರ outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      ಪದ_output(outbuff, outlen, ಖಾಲಿ);
    }

    ಬರೆ(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ಹಿಂತಿರುಗಿ 0;
}
