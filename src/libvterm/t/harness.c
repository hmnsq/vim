#ਅਨ੍ਤਰ੍ਭਾਵ "vterm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../src/vterm_internal.h" // We pull in some internal bits too

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ streq(a,b) (!strcmp(a,b))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strstartswith(a,b) (!ਲੜੀ_ਤੁਲਨਾ(a,b,ਲੜੀ_ਲੰਬਾਈ(b)))

ਸਥਿਰ size_t inplace_hex2bytes(ਅੱਖਰ *s)
{
  ਅੱਖਰ *inpos = s, *outpos = s;

  ਜਦੋਂ(*inpos) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ch;
    ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(inpos, "%2x", &ch) < 1)
      ਰੋਕੋ;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ਵਾਪਸ outpos - s;
}

ਸਥਿਰ VTermModifier strpe_modifiers(ਅੱਖਰ **strp)
{
  VTermModifier state = 0;

  ਜਦੋਂ((*strp)[0]) {
    ਚੋਣ(((*strp)++)[0]) {
      ਹਾਲਤ 'S': state |= VTERM_MOD_ਖਿਸਕਾਓ; ਰੋਕੋ;
      ਹਾਲਤ 'C': state |= VTERM_MOD_CTRL;  ਰੋਕੋ;
      ਹਾਲਤ 'A': state |= VTERM_MOD_ALT;   ਰੋਕੋ;
      ਮੂਲ: ਵਾਪਸ state;
    }
  }

  ਵਾਪਸ state;
}

ਸਥਿਰ VTerm_ਕੁੰਜੀ strp_ਕੁੰਜੀ(ਅੱਖਰ *str)
{
  ਸਥਿਰ ਬਣਤਰ {
    ਅੱਖਰ *name;
    VTerm_ਕੁੰਜੀ ਕੁੰਜੀ;
  } ਕੁੰਜੀਆਂ[] = {
    { "Up",    VTERM_ਕੁੰਜੀ_ਉੱਪਰ },
    { "Tab",   VTERM_ਕੁੰਜੀ_TAB },
    { "Enter", VTERM_ਕੁੰਜੀ_ENTER },
    { "KP0",   VTERM_ਕੁੰਜੀ_KP_0 },
    { "F1",    VTERM_ਕੁੰਜੀ_FUNCTION(1) },
    { ਨਿਰਾ,    VTERM_ਕੁੰਜੀ_NONE },
  };
  ਪੂਰਨ i;

  ਲਈ(i = 0; ਕੁੰਜੀਆਂ[i].name; i++) {
    ਜੇ(streq(str, ਕੁੰਜੀਆਂ[i].name))
      ਵਾਪਸ ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ;
  }

  ਵਾਪਸ VTERM_ਕੁੰਜੀ_NONE;
}

ਸਥਿਰ ਖਾਲੀ print_color(ਅਚਲ VTermColor *ਖਾਨਾ)
{
  ਜੇ (VTERM_COLOR_IS_RGB(ਖਾਨਾ)) {
    ਲਿਖੋ("rgb(%d,%d,%d", ਖਾਨਾ->red, ਖਾਨਾ->green, ਖਾਨਾ->blue);
  }
  ਨਹੀਂਤਾਂ ਜੇ (VTERM_COLOR_IS_ਸੂਚਕਾਂਕਿਤ(ਖਾਨਾ)) {
    ਲਿਖੋ("idx(%d", ਖਾਨਾ->ਸੂਚਕਾਂਕ);
  }
  ਨਹੀਂਤਾਂ {
    ਲਿਖੋ("invalid(%d", ਖਾਨਾ->type);
  }
  ਜੇ (VTERM_COLOR_IS_DEFAULT_FG(ਖਾਨਾ)) {
    ਲਿਖੋ(",is_default_fg");
  }
  ਜੇ (VTERM_COLOR_IS_DEFAULT_BG(ਖਾਨਾ)) {
    ਲਿਖੋ(",is_default_bg");
  }
  ਲਿਖੋ(")");
}

ਸਥਿਰ VTermColor strpe_color(ਅੱਖਰ **strp)
{
  uint8_t r, g, b, idx;
  ਪੂਰਨ len = 0;
  VTermColor ਖਾਨਾ;

  ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&ਖਾਨਾ, r, g, b);
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_ਸੂਚਕਾਂਕਿਤ(&ਖਾਨਾ, idx);
  }
  ਨਹੀਂਤਾਂ
    vterm_color_rgb(&ਖਾਨਾ, 127, 127, 127);

  ਵਾਪਸ ਖਾਨਾ;
}

ਸਥਿਰ VTerm *vt;
ਸਥਿਰ VTermState *state;
ਸਥਿਰ VTermScreen *screen;

ਸਥਿਰ VTermEncodingInstance encoding;

ਸਥਿਰ ਖਾਲੀ ਪਦ_output(ਅਚਲ ਅੱਖਰ *s, size_t len, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  size_t i;

  ਲਿਖੋ("output ");
  ਲਈ(i = 0; i < len; i++)
    ਲਿਖੋ("%x%s", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)s[i], i < len-1 ? "," : "\n");
}

ਸਥਿਰ ਖਾਲੀ printhex(ਅਚਲ ਅੱਖਰ *s, size_t len)
{
  ਜਦੋਂ(len--)
    ਲਿਖੋ("%02x", (uint8_t)(s++)[0]);
}

ਸਥਿਰ ਪੂਰਨ parser_ਪਾਠ(ਅਚਲ ਅੱਖਰ ਬਾਈਟਾਂ[], size_t len, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  size_t i;

  ਲਿਖੋ("text ");
  ਲਈ(i = 0; i < len; i++) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ b = ਬਾਈਟਾਂ[i];
    ਜੇ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      ਰੋਕੋ;
    ਲਿਖੋ(i ? ",%x" : "%x", b);
  }
  ਲਿਖੋ("\n");

  ਵਾਪਸ i;
}

ਸਥਿਰ ਪੂਰਨ parser_control(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ control, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਲਿਖੋ("control %02x\n", control);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ parser_escape(ਅਚਲ ਅੱਖਰ ਬਾਈਟਾਂ[], size_t len, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਜੇ(ਬਾਈਟਾਂ[0] >= 0x20 && ਬਾਈਟਾਂ[0] < 0x30) {
    ਜੇ(len < 2)
      ਵਾਪਸ -1;
    len = 2;
  }
  ਨਹੀਂਤਾਂ {
    len = 1;
  }

  ਲਿਖੋ("escape ");
  printhex(ਬਾਈਟਾਂ, len);
  ਲਿਖੋ("\n");

  ਵਾਪਸ len;
}

ਸਥਿਰ ਪੂਰਨ parser_csi(ਅਚਲ ਅੱਖਰ *leader, ਅਚਲ ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount, ਅਚਲ ਅੱਖਰ *intermed, ਅੱਖਰ command, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਪੂਰਨ i;
  ਲਿਖੋ("csi %02x", command);

  ਜੇ(leader && leader[0]) {
    ਲਿਖੋ(" L=");
    ਲਈ(i = 0; leader[i]; i++)
      ਲਿਖੋ("%02x", leader[i]);
  }

  ਲਈ(i = 0; i < argcount; i++) {
    ਅੱਖਰ sep = i ? ',' : ' ';

    ਜੇ(ਦਲੀਲਾਂ[i] == CSI_ARG_MISSING)
      ਲਿਖੋ("%c*", sep);
    ਨਹੀਂਤਾਂ
      ਲਿਖੋ("%c%ld%s", sep, CSI_ARG(ਦਲੀਲਾਂ[i]), CSI_ARG_HAS_ਹੋਰ(ਦਲੀਲਾਂ[i]) ? "+" : "");
  }

  ਜੇ(intermed && intermed[0]) {
    ਲਿਖੋ(" I=");
    ਲਈ(i = 0; intermed[i]; i++)
      ਲਿਖੋ("%02x", intermed[i]);
  }

  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ parser_osc(ਪੂਰਨ command, VTermStringFragment frag, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{

  ਲਿਖੋ("osc ");

  ਜੇ(frag.initial) {
    ਜੇ(command == -1)
      ਲਿਖੋ("[");
    ਨਹੀਂਤਾਂ
      ਲਿਖੋ("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ਜੇ(frag.final)
    ਲਿਖੋ("]");

  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ parser_dcs(ਅਚਲ ਅੱਖਰ *command, size_t commandlen, VTermStringFragment frag, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਲਿਖੋ("dcs ");

  ਜੇ(frag.initial) {
    size_t i;
    ਲਿਖੋ("[");
    ਲਈ(i = 0; i < commandlen; i++)
      ਲਿਖੋ("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ਜੇ(frag.final)
    ਲਿਖੋ("]");

  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ parser_apc(VTermStringFragment frag, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਲਿਖੋ("apc ");

  ਜੇ(frag.initial)
    ਲਿਖੋ("[");

  printhex(frag.str, frag.len);

  ਜੇ(frag.final)
    ਲਿਖੋ("]");

  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ parser_pm(VTermStringFragment frag, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਲਿਖੋ("pm ");

  ਜੇ(frag.initial)
    ਲਿਖੋ("[");

  printhex(frag.str, frag.len);

  ਜੇ(frag.final)
    ਲਿਖੋ("]");

  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ parser_sos(VTermStringFragment frag, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਲਿਖੋ("sos ");

  ਜੇ(frag.initial)
    ਲਿਖੋ("[");

  printhex(frag.str, frag.len);

  ਜੇ(frag.final)
    ਲਿਖੋ("]");

  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ VTermParserCallbacks parser_cbs = {
  parser_ਪਾਠ, //ਪਾਠ
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ਨਿਰਾ // resize
};

ਸਥਿਰ VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

ਸਥਿਰ ਪੂਰਨ want_movecursor = 0;
ਸਥਿਰ VTermPos state_pos;
ਸਥਿਰ ਪੂਰਨ movecursor(VTermPos pos, VTermPos oldpos ਅਣਵਰਤਿਆ, ਪੂਰਨ visible ਅਣਵਰਤਿਆ, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  state_pos = pos;

  ਜੇ(want_movecursor)
    ਲਿਖੋ("movecursor %d,%d\n", pos.ਪੰਕਤੀ, pos.ਖਾਨਾ);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ want_premove = 0;
ਸਥਿਰ ਪੂਰਨ premove(VTermRect rect, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਜੇ(!want_premove)
    ਵਾਪਸ 0;

  ਲਿਖੋ("premove %d..%d,%d..%d\n",
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, rect.ਸਿਰਾ_ਪੰਕਤੀ, rect.ਸ਼ੁਰੂ_ਖਾਨਾ, rect.ਸਿਰਾ_ਖਾਨਾ);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ want_scrollrect = 0;
ਸਥਿਰ ਪੂਰਨ scrollrect(VTermRect rect, ਪੂਰਨ downward, ਪੂਰਨ rightward, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਜੇ(!want_scrollrect)
    ਵਾਪਸ 0;

  ਲਿਖੋ("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, rect.ਸਿਰਾ_ਪੰਕਤੀ, rect.ਸ਼ੁਰੂ_ਖਾਨਾ, rect.ਸਿਰਾ_ਖਾਨਾ,
      downward, rightward);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ want_moverect = 0;
ਸਥਿਰ ਪੂਰਨ moverect(VTermRect dest, VTermRect src, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਜੇ(!want_moverect)
    ਵਾਪਸ 0;

  ਲਿਖੋ("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.ਸ਼ੁਰੂ_ਪੰਕਤੀ,  src.ਸਿਰਾ_ਪੰਕਤੀ,  src.ਸ਼ੁਰੂ_ਖਾਨਾ,  src.ਸਿਰਾ_ਖਾਨਾ,
      dest.ਸ਼ੁਰੂ_ਪੰਕਤੀ, dest.ਸਿਰਾ_ਪੰਕਤੀ, dest.ਸ਼ੁਰੂ_ਖਾਨਾ, dest.ਸਿਰਾ_ਖਾਨਾ);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ want_settermprop = 0;
ਸਥਿਰ ਪੂਰਨ settermprop(VTermProp prop, VTerm_ਮੁੱਲ *val, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  VTerm_ਮੁੱਲ_Type type;
  ਜੇ(!want_settermprop)
    ਵਾਪਸ 1;

  type = vterm_get_prop_type(prop);
  ਚੋਣ(type) {
  ਹਾਲਤ VTERM_VALUETYPE_BOOL:
    ਲਿਖੋ("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_VALUETYPE_INT:
    ਲਿਖੋ("settermprop %d %d\n", prop, val->ਸੰਖਿਆ);
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_VALUETYPE_STRING:
    ਲਿਖੋ("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_VALUETYPE_COLOR:
    ਲਿਖੋ("settermprop %d ", prop);
    print_color(&val->color);
    ਲਿਖੋ("\n");
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_N_VALUETYPES:
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ 0;
}

// These callbacks are for State

ਸਥਿਰ ਪੂਰਨ want_state_putglyph = 0;
ਸਥਿਰ ਪੂਰਨ state_putglyph(VTermGlyphInfo *info, VTermPos pos, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਪੂਰਨ i;
  ਜੇ(!want_state_putglyph)
    ਵਾਪਸ 1;

  ਲਿਖੋ("putglyph ");
  ਲਈ(i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL && info->chars[i]; i++)
    ਲਿਖੋ(i ? ",%x" : "%x", info->chars[i]);
  ਲਿਖੋ(" %d %d,%d", info->ਚੌੜਾਈ, pos.ਪੰਕਤੀ, pos.ਖਾਨਾ);
  ਜੇ(info->protected_cell)
    ਲਿਖੋ(" prot");
  ਜੇ(info->dwl)
    ਲਿਖੋ(" dwl");
  ਜੇ(info->dhl)
    ਲਿਖੋ(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ want_state_erase = 0;
ਸਥਿਰ ਪੂਰਨ state_erase(VTermRect rect, ਪੂਰਨ selective, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਜੇ(!want_state_erase)
    ਵਾਪਸ 1;

  ਲਿਖੋ("erase %d..%d,%d..%d%s\n",
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, rect.ਸਿਰਾ_ਪੰਕਤੀ, rect.ਸ਼ੁਰੂ_ਖਾਨਾ, rect.ਸਿਰਾ_ਖਾਨਾ,
      selective ? " selective" : "");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਬਣਤਰ {
  ਪੂਰਨ bold;
  ਪੂਰਨ underline;
  ਪੂਰਨ italic;
  ਪੂਰਨ blink;
  ਪੂਰਨ ਉਲਟ;
  ਪੂਰਨ conceal;
  ਪੂਰਨ strike;
  ਪੂਰਨ font;
  ਪੂਰਨ small;
  ਪੂਰਨ baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ਸਥਿਰ ਪੂਰਨ state_setpenattr(VTermAttr attr, VTerm_ਮੁੱਲ *val, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਚੋਣ(attr) {
  ਹਾਲਤ VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->ਸੰਖਿਆ;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_ਉਲਟ:
    state_pen.ਉਲਟ = val->boolean;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_FONT:
    state_pen.font = val->ਸੰਖਿਆ;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_BASELINE:
    state_pen.baseline = val->ਸੰਖਿਆ;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    ਰੋਕੋ;
  ਹਾਲਤ VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    ਰੋਕੋ;

  ਹਾਲਤ VTERM_N_ATTRS:
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ state_setlineinfo(ਪੂਰਨ ਪੰਕਤੀ ਅਣਵਰਤਿਆ, ਅਚਲ VTerm_ਸਤਰ_Info *newinfo ਅਣਵਰਤਿਆ, ਅਚਲ VTerm_ਸਤਰ_Info *oldinfo ਅਣਵਰਤਿਆ, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ want_state_scrollback = 0;
ਸਥਿਰ ਪੂਰਨ state_sb_clear(ਖਾਲੀ *user ਅਣਵਰਤਿਆ) {
  ਜੇ(!want_state_scrollback)
    ਵਾਪਸ 1;

  ਲਿਖੋ("sb_clear\n");
  ਵਾਪਸ 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ਨਿਰਾ, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ਨਿਰਾ, // bell
  ਨਿਰਾ, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

ਸਥਿਰ ਪੂਰਨ selection_ਸੈੱਟ(VTerm_Selection_ਨਕਾਬ ਨਕਾਬ, VTermStringFragment frag, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਲਿਖੋ("selection-set mask=%04X ", ਨਕਾਬ);
  ਜੇ(frag.initial)
    ਲਿਖੋ("[");
  printhex(frag.str, frag.len);
  ਜੇ(frag.final)
    ਲਿਖੋ("]");
  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ selection_query(VTerm_Selection_ਨਕਾਬ ਨਕਾਬ, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਲਿਖੋ("selection-query mask=%04X\n", ਨਕਾਬ);

  ਵਾਪਸ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .ਸੈੱਟ_2   = selection_ਸੈੱਟ,
  .query = selection_query,
};

ਸਥਿਰ ਪੂਰਨ want_screen_damage = 0;
ਸਥਿਰ ਪੂਰਨ want_screen_damage_cells = 0;
ਸਥਿਰ ਪੂਰਨ screen_damage(VTermRect rect, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਜੇ(!want_screen_damage)
    ਵਾਪਸ 1;

  ਲਿਖੋ("damage %d..%d,%d..%d",
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, rect.ਸਿਰਾ_ਪੰਕਤੀ, rect.ਸ਼ੁਰੂ_ਖਾਨਾ, rect.ਸਿਰਾ_ਖਾਨਾ);

  ਜੇ(want_screen_damage_cells) {
    ਪੂਰਨ equals = FALSE;
    ਪੂਰਨ ਪੰਕਤੀ;
    ਪੂਰਨ ਖਾਨਾ;

    ਲਈ(ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++) {
      ਪੂਰਨ eol = rect.ਸਿਰਾ_ਖਾਨਾ;
      ਜਦੋਂ(eol > rect.ਸ਼ੁਰੂ_ਖਾਨਾ) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.ਪੰਕਤੀ = ਪੰਕਤੀ;
	pos.ਖਾਨਾ = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ਜੇ(cell.chars[0])
          ਰੋਕੋ;

        eol--;
      }

      ਜੇ(eol == rect.ਸ਼ੁਰੂ_ਖਾਨਾ)
        ਰੋਕੋ;

      ਜੇ(!equals)
        ਲਿਖੋ(" ="), equals = TRUE;

      ਲਿਖੋ(" %d<", ਪੰਕਤੀ);
      ਲਈ(ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ; ਖਾਨਾ < eol; ਖਾਨਾ++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.ਪੰਕਤੀ = ਪੰਕਤੀ;
	pos.ਖਾਨਾ = ਖਾਨਾ;
        vterm_screen_get_cell(screen, pos, &cell);
        ਲਿਖੋ(ਖਾਨਾ == rect.ਸ਼ੁਰੂ_ਖਾਨਾ ? "%02X" : " %02X", cell.chars[0]);
      }
      ਲਿਖੋ(">");
    }
  }

  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ want_screen_scrollback = 0;
ਸਥਿਰ ਪੂਰਨ screen_sb_pushline4(ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਅਚਲ VTermScreenCell *cells, ਪੂਰਨ continuation, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਪੂਰਨ eol;
  ਪੂਰਨ c;

  ਜੇ(!want_screen_scrollback)
    ਵਾਪਸ 1;

  eol = ਖਾਨੇ_ਗਿਣਤੀ;
  ਜਦੋਂ(eol && !cells[eol-1].chars[0])
    eol--;

  ਲਿਖੋ("sb_pushline %d%s =", ਖਾਨੇ_ਗਿਣਤੀ, continuation ? " cont" : "");
  ਲਈ(c = 0; c < eol; c++)
    ਲਿਖੋ(" %02X", cells[c].chars[0]);
  ਲਿਖੋ("\n");

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ screen_sb_popline(ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, VTermScreenCell *cells, ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਪੂਰਨ ਖਾਨਾ;

  ਜੇ(!want_screen_scrollback)
    ਵਾਪਸ 0;

  // All lines of scrollback contain "ABCDE"
  ਲਈ(ਖਾਨਾ = 0; ਖਾਨਾ < ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++) {
    ਜੇ(ਖਾਨਾ < 5)
      cells[ਖਾਨਾ].chars[0] = 'A' + ਖਾਨਾ;
    ਨਹੀਂਤਾਂ
      cells[ਖਾਨਾ].chars[0] = 0;

    cells[ਖਾਨਾ].ਚੌੜਾਈ = 1;
  }

  ਲਿਖੋ("sb_popline %d\n", ਖਾਨੇ_ਗਿਣਤੀ);
  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ screen_sb_clear(ਖਾਲੀ *user ਅਣਵਰਤਿਆ)
{
  ਜੇ(!want_screen_scrollback)
    ਵਾਪਸ 1;

  ਲਿਖੋ("sb_clear\n");
  ਵਾਪਸ 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ਨਿਰਾ, // bell
  ਨਿਰਾ, // resize
  ਨਿਰਾ, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

ਪੂਰਨ ਮੁੱਖ(ਪੂਰਨ argc ਅਣਵਰਤਿਆ, ਅੱਖਰ **argv ਅਣਵਰਤਿਆ)
{
  ਅੱਖਰ ਸਤਰ[1024] = {0};
  ਪੂਰਨ flag;

  ਪੂਰਨ err;

  setvbuf(stdout, ਨਿਰਾ, _IONBF, 0);

  ਜਦੋਂ(fgets(ਸਤਰ, ਆਕਾਰ ਸਤਰ, stdin)) {
    ਅੱਖਰ *nl;
    size_t outlen;
    err = 0;

    ਜੇ((nl = strchr(ਸਤਰ, '\n')))
      *nl = '\0';

    ਜੇ(streq(ਸਤਰ, "INIT")) {
      ਜੇ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    ਨਹੀਂਤਾਂ ਜੇ(streq(ਸਤਰ, "WANTPARSER")) {
      ਪ੍ਰਤਿਜ੍ਞਾ(vt);
      vterm_parser_ਸੈੱਟ_callbacks(vt, &parser_cbs, ਨਿਰਾ);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "WANTSTATE") && (ਸਤਰ[9] == '\0' || ਸਤਰ[9] == ' ')) {
      ਪੂਰਨ i = 9;
      ਪੂਰਨ sense = 1;
      ਪ੍ਰਤਿਜ੍ਞਾ(vt);
      ਜੇ(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_ਸੈੱਟ_callbacks(state, &state_cbs, ਨਿਰਾ);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_ਸੈੱਟ_selection_callbacks(state, &selection_cbs, ਨਿਰਾ, ਨਿਰਾ, 16);
        vterm_state_ਸੈੱਟ_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      ਜਦੋਂ(ਸਤਰ[i] == ' ')
        i++;
      ਲਈ( ; ਸਤਰ[i]; i++)
        ਚੋਣ(ਸਤਰ[i]) {
        ਹਾਲਤ '+':
          sense = 1;
          ਰੋਕੋ;
        ਹਾਲਤ '-':
          sense = 0;
          ਰੋਕੋ;
        ਹਾਲਤ 'g':
          want_state_putglyph = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 's':
          want_scrollrect = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'P':
          want_premove = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'm':
          want_moverect = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'e':
          want_state_erase = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'p':
          want_settermprop = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'f':
          vterm_state_ਸੈੱਟ_unrecognised_fallbacks(state, sense ? &fallbacks : ਨਿਰਾ, ਨਿਰਾ);
          ਰੋਕੋ;
        ਹਾਲਤ 'b':
          want_state_scrollback = sense;
          ਰੋਕੋ;
        ਮੂਲ:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", ਸਤਰ[i]);
        }
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "WANTSCREEN") && (ਸਤਰ[10] == '\0' || ਸਤਰ[10] == ' ')) {
      ਪੂਰਨ i = 10;
      ਪੂਰਨ sense = 1;
      ਪ੍ਰਤਿਜ੍ਞਾ(vt);
      ਜੇ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_ਸੈੱਟ_callbacks(screen, &screen_cbs, ਨਿਰਾ);
      vterm_screen_callbacks_has_pushline4(screen);

      ਜਦੋਂ(ਸਤਰ[i] == ' ')
        i++;
      ਲਈ( ; ਸਤਰ[i]; i++)
        ਚੋਣ(ਸਤਰ[i]) {
        ਹਾਲਤ '-':
          sense = 0;
          ਰੋਕੋ;
        ਹਾਲਤ 'a':
          vterm_screen_enable_altscreen(screen, 1);
          ਰੋਕੋ;
        ਹਾਲਤ 'd':
          want_screen_damage = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'm':
          want_moverect = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'c':
          want_movecursor = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'p':
          want_settermprop = 1;
          ਰੋਕੋ;
        ਹਾਲਤ 'b':
          want_screen_scrollback = sense;
          ਰੋਕੋ;
        ਹਾਲਤ 'r':
          vterm_screen_enable_reflow(screen, sense);
          ਰੋਕੋ;
        ਮੂਲ:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", ਸਤਰ[i]);
        }
    }

    ਨਹੀਂਤਾਂ ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(ਸਤਰ, "UTF8 %d", &flag)) {
      vterm_ਸੈੱਟ_utf8(vt, flag);
    }

    ਨਹੀਂਤਾਂ ਜੇ(streq(ਸਤਰ, "RESET")) {
      ਜੇ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ਜੇ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "RESIZE ")) {
      ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ;
      ਅੱਖਰ *linep = ਸਤਰ + 7;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d, %d", &ਪੰਕਤੀਆਂ_2, &ਖਾਨੇ_ਗਿਣਤੀ);
      vterm_ਸੈੱਟ_ਮਾਪ(vt, ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "PUSH ")) {
      ਅੱਖਰ *ਬਾਈਟਾਂ = ਸਤਰ + 5;
      size_t len = inplace_hex2bytes(ਬਾਈਟਾਂ);
      ਪ੍ਰਤਿਜ੍ਞਾ(len);

      size_t written = vterm_ਨਿਵੇਸ਼_write(vt, ਬਾਈਟਾਂ, len);
      ਜੇ(written < len)
        fprintf(stderr, "! short write\n");
    }

    ਨਹੀਂਤਾਂ ਜੇ(streq(ਸਤਰ, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ਜੇ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.ਅੰਕੜੇ);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "ENCIN ")) {
      ਅੱਖਰ *ਬਾਈਟਾਂ = ਸਤਰ + 6;
      size_t len = inplace_hex2bytes(ਬਾਈਟਾਂ);
      ਪ੍ਰਤਿਜ੍ਞਾ(len);

      uint32_t cp[1024];
      ਪੂਰਨ cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.ਅੰਕੜੇ,
          cp, &cpi, len, ਬਾਈਟਾਂ, &pos, len);

      ਜੇ(cpi > 0) {
	ਪੂਰਨ i;
        ਲਿਖੋ("encout ");
        ਲਈ(i = 0; i < cpi; i++) {
          ਲਿਖੋ(i ? ",%x" : "%x", cp[i]);
        }
        ਲਿਖੋ("\n");
      }
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "INCHAR ")) {
      ਅੱਖਰ *linep = ਸਤਰ + 7;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ c = 0;
      VTermModifier mod;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "INKEY ")) {
      VTermModifier mod;
      VTerm_ਕੁੰਜੀ ਕੁੰਜੀ;
      ਅੱਖਰ *linep = ਸਤਰ + 6;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      ਕੁੰਜੀ = strp_ਕੁੰਜੀ(linep);

      vterm_keyboard_ਕੁੰਜੀ(vt, ਕੁੰਜੀ, mod);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "PASTE ")) {
      ਅੱਖਰ *linep = ਸਤਰ + 6;
      ਜੇ(streq(linep, "START"))
        vterm_keyboard_ਸ਼ੁਰੂ_paste(vt);
      ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "END"))
        vterm_keyboard_ਸਿਰਾ_paste(vt);
      ਨਹੀਂਤਾਂ
        ਜਾਓ abort_ਸਤਰ;
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "FOCUS ")) {
      ਪ੍ਰਤਿਜ੍ਞਾ(state);
      ਅੱਖਰ *linep = ਸਤਰ + 6;
      ਜੇ(streq(linep, "IN"))
        vterm_state_focus_ਵਿੱਚ(state);
      ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "OUT"))
        vterm_state_focus_ਬਾਹਰਲਾ(state);
      ਨਹੀਂਤਾਂ
        ਜਾਓ abort_ਸਤਰ;
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "MOUSEMOVE ")) {
      ਅੱਖਰ *linep = ਸਤਰ + 10;
      ਪੂਰਨ ਪੰਕਤੀ, ਖਾਨਾ, len;
      VTermModifier mod;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d,%d%n", &ਪੰਕਤੀ, &ਖਾਨਾ, &len);
      linep += len;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, ਪੰਕਤੀ, ਖਾਨਾ, mod);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "MOUSEBTN ")) {
      ਅੱਖਰ *linep = ਸਤਰ + 9;
      ਅੱਖਰ press;
      ਪੂਰਨ button, len;
      VTermModifier mod;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "SELECTION ")) {
      ਅੱਖਰ *linep = ਸਤਰ + 10;
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਨਕਾਬ;
      ਪੂਰਨ len;
      VTermStringFragment frag = { 0 };
      ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%x%n", &ਨਕਾਬ, &len);
      linep += len;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      ਜੇ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      ਪ੍ਰਤਿਜ੍ਞਾ(frag.len);

      linep += frag.len * 2;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      ਜੇ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, ਨਕਾਬ, frag);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "DAMAGEMERGE ")) {
      ਪ੍ਰਤਿਜ੍ਞਾ(screen);
      ਅੱਖਰ *linep = ਸਤਰ + 12;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      ਜੇ(streq(linep, "CELL"))
        vterm_screen_ਸੈੱਟ_damage_ਮਿਲਾਓ(screen, VTERM_DAMAGE_CELL);
      ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "ROW"))
        vterm_screen_ਸੈੱਟ_damage_ਮਿਲਾਓ(screen, VTERM_DAMAGE_ਪੰਕਤੀ);
      ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "SCREEN"))
        vterm_screen_ਸੈੱਟ_damage_ਮਿਲਾਓ(screen, VTERM_DAMAGE_SCREEN);
      ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "SCROLL"))
        vterm_screen_ਸੈੱਟ_damage_ਮਿਲਾਓ(screen, VTERM_DAMAGE_SCROLL);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "DAMAGEFLUSH")) {
      ਪ੍ਰਤਿਜ੍ਞਾ(screen);
      vterm_screen_flush_damage(screen);
    }

    ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "SETDEFAULTCOL ")) {
      ਪ੍ਰਤਿਜ੍ਞਾ(screen);
      ਅੱਖਰ *linep = ਸਤਰ + 14;
      ਜਦੋਂ(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ਜੇ(linep[0]) {
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_ਸੈੱਟ_default_colors(screen, &fg, &bg);
      }
      ਨਹੀਂਤਾਂ
        vterm_screen_ਸੈੱਟ_default_colors(screen, &fg, ਨਿਰਾ);
    }

    ਨਹੀਂਤਾਂ ਜੇ(ਸਤਰ[0] == '?') {
      ਜੇ(streq(ਸਤਰ, "?cursor")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ਜੇ(pos.ਪੰਕਤੀ != state_pos.ਪੰਕਤੀ)
          ਲਿਖੋ("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.ਪੰਕਤੀ, pos.ਖਾਨਾ, state_pos.ਪੰਕਤੀ, state_pos.ਖਾਨਾ);
        ਨਹੀਂਤਾਂ ਜੇ(pos.ਖਾਨਾ != state_pos.ਖਾਨਾ)
          ਲਿਖੋ("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.ਪੰਕਤੀ, pos.ਖਾਨਾ, state_pos.ਪੰਕਤੀ, state_pos.ਖਾਨਾ);
        ਨਹੀਂਤਾਂ
          ਲਿਖੋ("%d,%d\n", state_pos.ਪੰਕਤੀ, state_pos.ਖਾਨਾ);
      }
      ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "?pen ")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(state);
        VTerm_ਮੁੱਲ val;
        ਅੱਖਰ *linep = ਸਤਰ + 5;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BOOLSTR(v) ((v) ? "on" : "off")

        ਜੇ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ਜੇ(val.boolean != state_pen.bold)
            ਲਿਖੋ("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%s\n", BOOLSTR(state_pen.bold));
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ਜੇ(val.boolean != state_pen.underline)
            ਲਿਖੋ("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%d\n", state_pen.underline);
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ਜੇ(val.boolean != state_pen.italic)
            ਲਿਖੋ("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%s\n", BOOLSTR(state_pen.italic));
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ਜੇ(val.boolean != state_pen.blink)
            ਲਿਖੋ("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%s\n", BOOLSTR(state_pen.blink));
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ਉਲਟ, &val);
          ਜੇ(val.boolean != state_pen.ਉਲਟ)
            ਲਿਖੋ("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.ਉਲਟ));
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%s\n", BOOLSTR(state_pen.ਉਲਟ));
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ਜੇ(val.boolean != state_pen.font)
            ਲਿਖੋ("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%d\n", state_pen.font);
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ਜੇ(val.boolean != state_pen.small)
            ਲਿਖੋ("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%s\n", BOOLSTR(state_pen.small));
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ਜੇ(val.ਸੰਖਿਆ != state_pen.baseline)
            ਲਿਖੋ("! pen baseline mismatch: state=%d, event=%d\n",
                val.ਸੰਖਿਆ, state_pen.baseline);
          ਨਹੀਂਤਾਂ
            ਲਿਖੋ("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_ਹੇਠਲਾ ? "lower"
                         : "normal");
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          ਲਿਖੋ("\n");
        }
        ਨਹੀਂਤਾਂ ਜੇ(streq(linep, "background")) {
          print_color(&state_pen.background);
          ਲਿਖੋ("\n");
        }
        ਨਹੀਂਤਾਂ
          ਲਿਖੋ("?\n");
      }
      ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "?lineinfo ")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(state);
        ਅੱਖਰ *linep = ਸਤਰ + 10;
        ਪੂਰਨ ਪੰਕਤੀ;
        ਅਚਲ VTerm_ਸਤਰ_Info *info;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
        ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d", &ਪੰਕਤੀ) < 1) {
          ਲਿਖੋ("! lineinfo unrecognised input\n");
          ਜਾਓ abort_ਸਤਰ;
        }
        info = vterm_state_get_lineinfo(state, ਪੰਕਤੀ);
        ਜੇ(info->doublewidth)
          ਲਿਖੋ("dwl ");
        ਜੇ(info->doubleheight)
          ਲਿਖੋ("dhl ");
        ਜੇ(info->continuation)
          ਲਿਖੋ("cont ");
        ਲਿਖੋ("\n");
      }
      ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "?screen_chars ")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(screen);
        ਅੱਖਰ *linep = ਸਤਰ + 13;
        VTermRect rect;
        size_t len;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
        ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d,%d,%d,%d", &rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, &rect.ਸ਼ੁਰੂ_ਖਾਨਾ, &rect.ਸਿਰਾ_ਪੰਕਤੀ, &rect.ਸਿਰਾ_ਖਾਨਾ) == 4)
          ; // fine
        ਨਹੀਂਤਾਂ ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d", &rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ) == 1) {
          rect.ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ + 1;
          rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
          vterm_get_ਮਾਪ(vt, ਨਿਰਾ, &rect.ਸਿਰਾ_ਖਾਨਾ);
        }
        ਨਹੀਂਤਾਂ {
          ਲਿਖੋ("! screen_chars unrecognised input\n");
          ਜਾਓ abort_ਸਤਰ;
        }
        len = vterm_screen_get_chars(screen, ਨਿਰਾ, 0, rect);
        ਜੇ(len == (size_t)-1)
          ਲਿਖੋ("! screen_chars error\n");
        ਨਹੀਂਤਾਂ ਜੇ(len == 0)
          ਲਿਖੋ("\n");
        ਨਹੀਂਤਾਂ {
          uint32_t *chars = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          ਲਈ(i = 0; i < len; i++) {
            ਲਿਖੋ("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          ਮੁਕਤ(chars);
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "?screen_text ")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(screen);
        ਅੱਖਰ *linep = ਸਤਰ + 12;
        VTermRect rect;
        size_t len;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
        ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d,%d,%d,%d", &rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, &rect.ਸ਼ੁਰੂ_ਖਾਨਾ, &rect.ਸਿਰਾ_ਪੰਕਤੀ, &rect.ਸਿਰਾ_ਖਾਨਾ) < 4) {
          ਲਿਖੋ("! screen_text unrecognised input\n");
          ਜਾਓ abort_ਸਤਰ;
        }
        len = vterm_screen_get_ਪਾਠ(screen, ਨਿਰਾ, 0, rect);
        ਜੇ(len == (size_t)-1)
          ਲਿਖੋ("! screen_text error\n");
        ਨਹੀਂਤਾਂ ਜੇ(len == 0)
          ਲਿਖੋ("\n");
        ਨਹੀਂਤਾਂ {
          // Put an overwrite guard at both ends of the buffer
          ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਬਫਰ = ਸਮ੍ਰਿਤੀ_ਦਿਓ(len + 4);
          ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਪਾਠ = ਬਫਰ + 2;
          ਪਾਠ[-2] = 0x55; ਪਾਠ[-1] = 0xAA;
          ਪਾਠ[len] = 0x55; ਪਾਠ[len+1] = 0xAA;

          vterm_screen_get_ਪਾਠ(screen, (ਅੱਖਰ *)ਪਾਠ, len, rect);

          ਜੇ(ਪਾਠ[-2] != 0x55 || ਪਾਠ[-1] != 0xAA)
            ਲਿਖੋ("! screen_get_text buffer overrun left [%02x,%02x]\n", ਪਾਠ[-2], ਪਾਠ[-1]);
          ਨਹੀਂਤਾਂ ਜੇ(ਪਾਠ[len] != 0x55 || ਪਾਠ[len+1] != 0xAA)
            ਲਿਖੋ("! screen_get_text buffer overrun right [%02x,%02x]\n", ਪਾਠ[len], ਪਾਠ[len+1]);
          ਨਹੀਂਤਾਂ
	  {
	    size_t i;
            ਲਈ(i = 0; i < len; i++) {
              ਲਿਖੋ("0x%02x%s", ਪਾਠ[i], i < len-1 ? "," : "\n");
            }
	  }

          ਮੁਕਤ(ਬਫਰ);
        }
      }
      ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "?screen_cell ")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(screen);
        ਅੱਖਰ *linep = ਸਤਰ + 12;
	ਪੂਰਨ i;
        VTermPos pos;
        VTermScreenCell cell;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
        ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d,%d\n", &pos.ਪੰਕਤੀ, &pos.ਖਾਨਾ) < 2) {
          ਲਿਖੋ("! screen_cell unrecognised input\n");
          ਜਾਓ abort_ਸਤਰ;
        }
        ਜੇ(!vterm_screen_get_cell(screen, pos, &cell))
          ਜਾਓ abort_ਸਤਰ;
        ਲਿਖੋ("{");
        ਲਈ(i = 0; i < VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL && cell.chars[i]; i++) {
          ਲਿਖੋ("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        ਲਿਖੋ("} width=%d attrs={", cell.ਚੌੜਾਈ);
        ਜੇ(cell.attrs.bold)      ਲਿਖੋ("B");
        ਜੇ(cell.attrs.underline) ਲਿਖੋ("U%d", cell.attrs.underline);
        ਜੇ(cell.attrs.italic)    ਲਿਖੋ("I");
        ਜੇ(cell.attrs.blink)     ਲਿਖੋ("K");
        ਜੇ(cell.attrs.ਉਲਟ)   ਲਿਖੋ("R");
        ਜੇ(cell.attrs.font)      ਲਿਖੋ("F%d", cell.attrs.font);
        ਜੇ(cell.attrs.small)     ਲਿਖੋ("S");
        ਜੇ(cell.attrs.baseline)  ਲਿਖੋ(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        ਲਿਖੋ("} ");
        ਜੇ(cell.attrs.dwl)       ਲਿਖੋ("dwl ");
        ਜੇ(cell.attrs.dhl)       ਲਿਖੋ("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        ਲਿਖੋ("fg=");
        vterm_screen_convert_color_ਮੰਜ਼ਿਲ_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        ਲਿਖੋ(" bg=");
        vterm_screen_convert_color_ਮੰਜ਼ਿਲ_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        ਲਿਖੋ("\n");
      }
      ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "?screen_eol ")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(screen);
        VTermPos pos;
        ਅੱਖਰ *linep = ਸਤਰ + 12;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
        ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d,%d\n", &pos.ਪੰਕਤੀ, &pos.ਖਾਨਾ) < 2) {
          ਲਿਖੋ("! screen_eol unrecognised input\n");
          ਜਾਓ abort_ਸਤਰ;
        }
        ਲਿਖੋ("%d\n", vterm_screen_is_eol(screen, pos));
      }
      ਨਹੀਂਤਾਂ ਜੇ(strstartswith(ਸਤਰ, "?screen_attrs_extent ")) {
        ਪ੍ਰਤਿਜ੍ਞਾ(screen);
        VTermPos pos;
        VTermRect rect;
        ਅੱਖਰ *linep = ਸਤਰ + 21;
        ਜਦੋਂ(linep[0] == ' ')
          linep++;
        ਜੇ(ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(linep, "%d,%d\n", &pos.ਪੰਕਤੀ, &pos.ਖਾਨਾ) < 2) {
          ਲਿਖੋ("! screen_attrs_extent unrecognised input\n");
          ਜਾਓ abort_ਸਤਰ;
        }
	rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
	rect.ਸਿਰਾ_ਖਾਨਾ   = -1;
        ਜੇ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          ਲਿਖੋ("! screen_attrs_extent failed\n");
          ਜਾਓ abort_ਸਤਰ;
        }
        ਲਿਖੋ("%d,%d-%d,%d\n", rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, rect.ਸ਼ੁਰੂ_ਖਾਨਾ, rect.ਸਿਰਾ_ਪੰਕਤੀ, rect.ਸਿਰਾ_ਖਾਨਾ);
      }
      ਨਹੀਂਤਾਂ
        ਲਿਖੋ("?\n");

      memset(ਸਤਰ, 0, ਆਕਾਰ ਸਤਰ);
      ਜਾਰੀ;
    }

    ਨਹੀਂਤਾਂ
      abort_ਸਤਰ: err = 1;

    outlen = vterm_output_get_ਬਫਰ_current(vt);
    ਜੇ(outlen > 0) {
      ਅੱਖਰ outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      ਪਦ_output(outbuff, outlen, ਨਿਰਾ);
    }

    ਲਿਖੋ(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ਵਾਪਸ 0;
}
