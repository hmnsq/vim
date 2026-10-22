#അന്തര്ഭാവ "vterm.h"
#അന്തര്ഭാവ "../src/vterm_internal.h" // We pull in some internal bits too

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

സ്ഥിതം size_t inplace_hex2bytes(അക്ഷരം *s)
{
  അക്ഷരം *inpos = s, *outpos = s;

  ആകുമ്പോൾ(*inpos) {
    unsigned പൂർണ്ണസംഖ്യ ch;
    എങ്കിൽ(sscanf(inpos, "%2x", &ch) < 1)
      നിർത്തുക;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  തിരികെ outpos - s;
}

സ്ഥിതം VTermModifier strpe_modifiers(അക്ഷരം **strp)
{
  VTermModifier state = 0;

  ആകുമ്പോൾ((*strp)[0]) {
    തിരഞ്ഞെടുപ്പ്(((*strp)++)[0]) {
      അവസ്ഥ 'S': state |= VTERM_MOD_SHIFT; നിർത്തുക;
      അവസ്ഥ 'C': state |= VTERM_MOD_CTRL;  നിർത്തുക;
      അവസ്ഥ 'A': state |= VTERM_MOD_ALT;   നിർത്തുക;
      സ്ഥിരസ്ഥിതി: തിരികെ state;
    }
  }

  തിരികെ state;
}

സ്ഥിതം VTerm_താക്കോൽ strp_താക്കോൽ(അക്ഷരം *str)
{
  സ്ഥിതം ഘടന {
    അക്ഷരം *name;
    VTerm_താക്കോൽ താക്കോൽ;
  } താക്കോലുകൾ[] = {
    { "Up",    VTERM_താക്കോൽ_UP },
    { "Tab",   VTERM_താക്കോൽ_TAB },
    { "Enter", VTERM_താക്കോൽ_ENTER },
    { "KP0",   VTERM_താക്കോൽ_KP_0 },
    { "F1",    VTERM_താക്കോൽ_FUNCTION(1) },
    { ശൂന്യ,    VTERM_താക്കോൽ_NONE },
  };
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി(i = 0; താക്കോലുകൾ[i].name; i++) {
    എങ്കിൽ(streq(str, താക്കോലുകൾ[i].name))
      തിരികെ താക്കോലുകൾ[i].താക്കോൽ;
  }

  തിരികെ VTERM_താക്കോൽ_NONE;
}

സ്ഥിതം ശൂന്യം print_color(സ്ഥിരം VTermColor *col)
{
  എങ്കിൽ (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  അല്ലെങ്കിൽ എങ്കിൽ (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  അല്ലെങ്കിൽ {
    printf("invalid(%d", col->type);
  }
  എങ്കിൽ (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  എങ്കിൽ (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

സ്ഥിതം VTermColor strpe_color(അക്ഷരം **strp)
{
  uint8_t r, g, b, idx;
  പൂർണ്ണസംഖ്യ len = 0;
  VTermColor col;

  എങ്കിൽ(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  അല്ലെങ്കിൽ
    vterm_color_rgb(&col, 127, 127, 127);

  തിരികെ col;
}

സ്ഥിതം VTerm *vt;
സ്ഥിതം VTermState *state;
സ്ഥിതം VTermScreen *screen;

സ്ഥിതം VTermEncodingInstance encoding;

സ്ഥിതം ശൂന്യം term_output(സ്ഥിരം അക്ഷരം *s, size_t len, ശൂന്യം *user UNUSED)
{
  size_t i;

  printf("output ");
  വേണ്ടി(i = 0; i < len; i++)
    printf("%x%s", (unsigned അക്ഷരം)s[i], i < len-1 ? "," : "\n");
}

സ്ഥിതം ശൂന്യം printhex(സ്ഥിരം അക്ഷരം *s, size_t len)
{
  ആകുമ്പോൾ(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_text(സ്ഥിരം അക്ഷരം bytes[], size_t len, ശൂന്യം *user UNUSED)
{
  size_t i;

  printf("text ");
  വേണ്ടി(i = 0; i < len; i++) {
    unsigned അക്ഷരം b = bytes[i];
    എങ്കിൽ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      നിർത്തുക;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  തിരികെ i;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_control(unsigned അക്ഷരം control, ശൂന്യം *user UNUSED)
{
  printf("control %02x\n", control);

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_escape(സ്ഥിരം അക്ഷരം bytes[], size_t len, ശൂന്യം *user UNUSED)
{
  എങ്കിൽ(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    എങ്കിൽ(len < 2)
      തിരികെ -1;
    len = 2;
  }
  അല്ലെങ്കിൽ {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  തിരികെ len;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_csi(സ്ഥിരം അക്ഷരം *leader, സ്ഥിരം long args[], പൂർണ്ണസംഖ്യ argcount, സ്ഥിരം അക്ഷരം *intermed, അക്ഷരം command, ശൂന്യം *user UNUSED)
{
  പൂർണ്ണസംഖ്യ i;
  printf("csi %02x", command);

  എങ്കിൽ(leader && leader[0]) {
    printf(" L=");
    വേണ്ടി(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  വേണ്ടി(i = 0; i < argcount; i++) {
    അക്ഷരം sep = i ? ',' : ' ';

    എങ്കിൽ(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    അല്ലെങ്കിൽ
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  എങ്കിൽ(intermed && intermed[0]) {
    printf(" I=");
    വേണ്ടി(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_osc(പൂർണ്ണസംഖ്യ command, VTermStringFragment frag, ശൂന്യം *user UNUSED)
{

  printf("osc ");

  എങ്കിൽ(frag.initial) {
    എങ്കിൽ(command == -1)
      printf("[");
    അല്ലെങ്കിൽ
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  എങ്കിൽ(frag.final)
    printf("]");

  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_dcs(സ്ഥിരം അക്ഷരം *command, size_t commandlen, VTermStringFragment frag, ശൂന്യം *user UNUSED)
{
  printf("dcs ");

  എങ്കിൽ(frag.initial) {
    size_t i;
    printf("[");
    വേണ്ടി(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  എങ്കിൽ(frag.final)
    printf("]");

  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_apc(VTermStringFragment frag, ശൂന്യം *user UNUSED)
{
  printf("apc ");

  എങ്കിൽ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  എങ്കിൽ(frag.final)
    printf("]");

  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_pm(VTermStringFragment frag, ശൂന്യം *user UNUSED)
{
  printf("pm ");

  എങ്കിൽ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  എങ്കിൽ(frag.final)
    printf("]");

  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parser_sos(VTermStringFragment frag, ശൂന്യം *user UNUSED)
{
  printf("sos ");

  എങ്കിൽ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  എങ്കിൽ(frag.final)
    printf("]");

  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ശൂന്യ // resize
};

സ്ഥിതം VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

സ്ഥിതം പൂർണ്ണസംഖ്യ want_movecursor = 0;
സ്ഥിതം VTermPos state_pos;
സ്ഥിതം പൂർണ്ണസംഖ്യ movecursor(VTermPos pos, VTermPos oldpos UNUSED, പൂർണ്ണസംഖ്യ visible UNUSED, ശൂന്യം *user UNUSED)
{
  state_pos = pos;

  എങ്കിൽ(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ want_premove = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ premove(VTermRect rect, ശൂന്യം *user UNUSED)
{
  എങ്കിൽ(!want_premove)
    തിരികെ 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ want_scrollrect = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ scrollrect(VTermRect rect, പൂർണ്ണസംഖ്യ downward, പൂർണ്ണസംഖ്യ rightward, ശൂന്യം *user UNUSED)
{
  എങ്കിൽ(!want_scrollrect)
    തിരികെ 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ want_moverect = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ moverect(VTermRect dest, VTermRect src, ശൂന്യം *user UNUSED)
{
  എങ്കിൽ(!want_moverect)
    തിരികെ 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ want_settermprop = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ settermprop(VTermProp prop, VTerm_മൂല്യം *val, ശൂന്യം *user UNUSED)
{
  VTerm_മൂല്യം_Type type;
  എങ്കിൽ(!want_settermprop)
    തിരികെ 1;

  type = vterm_get_prop_type(prop);
  തിരഞ്ഞെടുപ്പ്(type) {
  അവസ്ഥ VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    തിരികെ 1;
  അവസ്ഥ VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->സംഖ്യ);
    തിരികെ 1;
  അവസ്ഥ VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    തിരികെ 1;
  അവസ്ഥ VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    തിരികെ 1;

  അവസ്ഥ VTERM_N_VALUETYPES:
    തിരികെ 0;
  }

  തിരികെ 0;
}

// These callbacks are for State

സ്ഥിതം പൂർണ്ണസംഖ്യ want_state_putglyph = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ state_putglyph(VTermGlyphInfo *info, VTermPos pos, ശൂന്യം *user UNUSED)
{
  പൂർണ്ണസംഖ്യ i;
  എങ്കിൽ(!want_state_putglyph)
    തിരികെ 1;

  printf("putglyph ");
  വേണ്ടി(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  എങ്കിൽ(info->protected_cell)
    printf(" prot");
  എങ്കിൽ(info->dwl)
    printf(" dwl");
  എങ്കിൽ(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ want_state_erase = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ state_erase(VTermRect rect, പൂർണ്ണസംഖ്യ selective, ശൂന്യം *user UNUSED)
{
  എങ്കിൽ(!want_state_erase)
    തിരികെ 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  തിരികെ 1;
}

സ്ഥിതം ഘടന {
  പൂർണ്ണസംഖ്യ bold;
  പൂർണ്ണസംഖ്യ underline;
  പൂർണ്ണസംഖ്യ italic;
  പൂർണ്ണസംഖ്യ blink;
  പൂർണ്ണസംഖ്യ reverse;
  പൂർണ്ണസംഖ്യ conceal;
  പൂർണ്ണസംഖ്യ strike;
  പൂർണ്ണസംഖ്യ font;
  പൂർണ്ണസംഖ്യ small;
  പൂർണ്ണസംഖ്യ baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
സ്ഥിതം പൂർണ്ണസംഖ്യ state_setpenattr(VTermAttr attr, VTerm_മൂല്യം *val, ശൂന്യം *user UNUSED)
{
  തിരഞ്ഞെടുപ്പ്(attr) {
  അവസ്ഥ VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->സംഖ്യ;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_FONT:
    state_pen.font = val->സംഖ്യ;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_BASELINE:
    state_pen.baseline = val->സംഖ്യ;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    നിർത്തുക;
  അവസ്ഥ VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    നിർത്തുക;

  അവസ്ഥ VTERM_N_ATTRS:
    തിരികെ 0;
  }

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ state_setlineinfo(പൂർണ്ണസംഖ്യ row UNUSED, സ്ഥിരം VTerm_വരി_Info *newinfo UNUSED, സ്ഥിരം VTerm_വരി_Info *oldinfo UNUSED, ശൂന്യം *user UNUSED)
{
  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ want_state_scrollback = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ state_sb_clear(ശൂന്യം *user UNUSED) {
  എങ്കിൽ(!want_state_scrollback)
    തിരികെ 1;

  printf("sb_clear\n");
  തിരികെ 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ശൂന്യ, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ശൂന്യ, // bell
  ശൂന്യ, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

സ്ഥിതം പൂർണ്ണസംഖ്യ selection_set(VTermSelectionMask mask, VTermStringFragment frag, ശൂന്യം *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  എങ്കിൽ(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  എങ്കിൽ(frag.final)
    printf("]");
  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ selection_query(VTermSelectionMask mask, ശൂന്യം *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  തിരികെ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

സ്ഥിതം പൂർണ്ണസംഖ്യ want_screen_damage = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ want_screen_damage_cells = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ screen_damage(VTermRect rect, ശൂന്യം *user UNUSED)
{
  എങ്കിൽ(!want_screen_damage)
    തിരികെ 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  എങ്കിൽ(want_screen_damage_cells) {
    പൂർണ്ണസംഖ്യ equals = FALSE;
    പൂർണ്ണസംഖ്യ row;
    പൂർണ്ണസംഖ്യ col;

    വേണ്ടി(row = rect.start_row; row < rect.end_row; row++) {
      പൂർണ്ണസംഖ്യ eol = rect.end_col;
      ആകുമ്പോൾ(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        എങ്കിൽ(cell.chars[0])
          നിർത്തുക;

        eol--;
      }

      എങ്കിൽ(eol == rect.start_col)
        നിർത്തുക;

      എങ്കിൽ(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      വേണ്ടി(col = rect.start_col; col < eol; col++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = col;
        vterm_screen_get_cell(screen, pos, &cell);
        printf(col == rect.start_col ? "%02X" : " %02X", cell.chars[0]);
      }
      printf(">");
    }
  }

  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ want_screen_scrollback = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ screen_sb_pushline4(പൂർണ്ണസംഖ്യ cols, സ്ഥിരം VTermScreenCell *cells, പൂർണ്ണസംഖ്യ continuation, ശൂന്യം *user UNUSED)
{
  പൂർണ്ണസംഖ്യ eol;
  പൂർണ്ണസംഖ്യ c;

  എങ്കിൽ(!want_screen_scrollback)
    തിരികെ 1;

  eol = cols;
  ആകുമ്പോൾ(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  വേണ്ടി(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ screen_sb_popline(പൂർണ്ണസംഖ്യ cols, VTermScreenCell *cells, ശൂന്യം *user UNUSED)
{
  പൂർണ്ണസംഖ്യ col;

  എങ്കിൽ(!want_screen_scrollback)
    തിരികെ 0;

  // All lines of scrollback contain "ABCDE"
  വേണ്ടി(col = 0; col < cols; col++) {
    എങ്കിൽ(col < 5)
      cells[col].chars[0] = 'A' + col;
    അല്ലെങ്കിൽ
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ screen_sb_clear(ശൂന്യം *user UNUSED)
{
  എങ്കിൽ(!want_screen_scrollback)
    തിരികെ 1;

  printf("sb_clear\n");
  തിരികെ 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ശൂന്യ, // bell
  ശൂന്യ, // resize
  ശൂന്യ, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

പൂർണ്ണസംഖ്യ പ്രധാന(പൂർണ്ണസംഖ്യ argc UNUSED, അക്ഷരം **argv UNUSED)
{
  അക്ഷരം വരി[1024] = {0};
  പൂർണ്ണസംഖ്യ flag;

  പൂർണ്ണസംഖ്യ err;

  setvbuf(stdout, ശൂന്യ, _IONBF, 0);

  ആകുമ്പോൾ(fgets(വരി, വലുപ്പം വരി, stdin)) {
    അക്ഷരം *nl;
    size_t outlen;
    err = 0;

    എങ്കിൽ((nl = strchr(വരി, '\n')))
      *nl = '\0';

    എങ്കിൽ(streq(വരി, "INIT")) {
      എങ്കിൽ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(streq(വരി, "WANTPARSER")) {
      പ്രതിജ്ഞാ(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, ശൂന്യ);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "WANTSTATE") && (വരി[9] == '\0' || വരി[9] == ' ')) {
      പൂർണ്ണസംഖ്യ i = 9;
      പൂർണ്ണസംഖ്യ sense = 1;
      പ്രതിജ്ഞാ(vt);
      എങ്കിൽ(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, ശൂന്യ);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, ശൂന്യ, ശൂന്യ, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      ആകുമ്പോൾ(വരി[i] == ' ')
        i++;
      വേണ്ടി( ; വരി[i]; i++)
        തിരഞ്ഞെടുപ്പ്(വരി[i]) {
        അവസ്ഥ '+':
          sense = 1;
          നിർത്തുക;
        അവസ്ഥ '-':
          sense = 0;
          നിർത്തുക;
        അവസ്ഥ 'g':
          want_state_putglyph = sense;
          നിർത്തുക;
        അവസ്ഥ 's':
          want_scrollrect = sense;
          നിർത്തുക;
        അവസ്ഥ 'P':
          want_premove = sense;
          നിർത്തുക;
        അവസ്ഥ 'm':
          want_moverect = sense;
          നിർത്തുക;
        അവസ്ഥ 'e':
          want_state_erase = sense;
          നിർത്തുക;
        അവസ്ഥ 'p':
          want_settermprop = sense;
          നിർത്തുക;
        അവസ്ഥ 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : ശൂന്യ, ശൂന്യ);
          നിർത്തുക;
        അവസ്ഥ 'b':
          want_state_scrollback = sense;
          നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", വരി[i]);
        }
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "WANTSCREEN") && (വരി[10] == '\0' || വരി[10] == ' ')) {
      പൂർണ്ണസംഖ്യ i = 10;
      പൂർണ്ണസംഖ്യ sense = 1;
      പ്രതിജ്ഞാ(vt);
      എങ്കിൽ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, ശൂന്യ);
      vterm_screen_callbacks_has_pushline4(screen);

      ആകുമ്പോൾ(വരി[i] == ' ')
        i++;
      വേണ്ടി( ; വരി[i]; i++)
        തിരഞ്ഞെടുപ്പ്(വരി[i]) {
        അവസ്ഥ '-':
          sense = 0;
          നിർത്തുക;
        അവസ്ഥ 'a':
          vterm_screen_enable_altscreen(screen, 1);
          നിർത്തുക;
        അവസ്ഥ 'd':
          want_screen_damage = sense;
          നിർത്തുക;
        അവസ്ഥ 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          നിർത്തുക;
        അവസ്ഥ 'm':
          want_moverect = sense;
          നിർത്തുക;
        അവസ്ഥ 'c':
          want_movecursor = sense;
          നിർത്തുക;
        അവസ്ഥ 'p':
          want_settermprop = 1;
          നിർത്തുക;
        അവസ്ഥ 'b':
          want_screen_scrollback = sense;
          നിർത്തുക;
        അവസ്ഥ 'r':
          vterm_screen_enable_reflow(screen, sense);
          നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", വരി[i]);
        }
    }

    അല്ലെങ്കിൽ എങ്കിൽ(sscanf(വരി, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(streq(വരി, "RESET")) {
      എങ്കിൽ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      എങ്കിൽ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "RESIZE ")) {
      പൂർണ്ണസംഖ്യ rows, cols;
      അക്ഷരം *linep = വരി + 7;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "PUSH ")) {
      അക്ഷരം *bytes = വരി + 5;
      size_t len = inplace_hex2bytes(bytes);
      പ്രതിജ്ഞാ(len);

      size_t written = vterm_നിവേശം_write(vt, bytes, len);
      എങ്കിൽ(written < len)
        fprintf(stderr, "! short write\n");
    }

    അല്ലെങ്കിൽ എങ്കിൽ(streq(വരി, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      എങ്കിൽ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "ENCIN ")) {
      അക്ഷരം *bytes = വരി + 6;
      size_t len = inplace_hex2bytes(bytes);
      പ്രതിജ്ഞാ(len);

      uint32_t cp[1024];
      പൂർണ്ണസംഖ്യ cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      എങ്കിൽ(cpi > 0) {
	പൂർണ്ണസംഖ്യ i;
        printf("encout ");
        വേണ്ടി(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "INCHAR ")) {
      അക്ഷരം *linep = വരി + 7;
      unsigned പൂർണ്ണസംഖ്യ c = 0;
      VTermModifier mod;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "INKEY ")) {
      VTermModifier mod;
      VTerm_താക്കോൽ താക്കോൽ;
      അക്ഷരം *linep = വരി + 6;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      താക്കോൽ = strp_താക്കോൽ(linep);

      vterm_keyboard_താക്കോൽ(vt, താക്കോൽ, mod);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "PASTE ")) {
      അക്ഷരം *linep = വരി + 6;
      എങ്കിൽ(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      അല്ലെങ്കിൽ
        പോകുക abort_വരി;
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "FOCUS ")) {
      പ്രതിജ്ഞാ(state);
      അക്ഷരം *linep = വരി + 6;
      എങ്കിൽ(streq(linep, "IN"))
        vterm_state_focus_in(state);
      അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      അല്ലെങ്കിൽ
        പോകുക abort_വരി;
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "MOUSEMOVE ")) {
      അക്ഷരം *linep = വരി + 10;
      പൂർണ്ണസംഖ്യ row, col, len;
      VTermModifier mod;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "MOUSEBTN ")) {
      അക്ഷരം *linep = വരി + 9;
      അക്ഷരം press;
      പൂർണ്ണസംഖ്യ button, len;
      VTermModifier mod;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "SELECTION ")) {
      അക്ഷരം *linep = വരി + 10;
      unsigned പൂർണ്ണസംഖ്യ mask;
      പൂർണ്ണസംഖ്യ len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      എങ്കിൽ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      പ്രതിജ്ഞാ(frag.len);

      linep += frag.len * 2;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      എങ്കിൽ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "DAMAGEMERGE ")) {
      പ്രതിജ്ഞാ(screen);
      അക്ഷരം *linep = വരി + 12;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      എങ്കിൽ(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "DAMAGEFLUSH")) {
      പ്രതിജ്ഞാ(screen);
      vterm_screen_flush_damage(screen);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "SETDEFAULTCOL ")) {
      പ്രതിജ്ഞാ(screen);
      അക്ഷരം *linep = വരി + 14;
      ആകുമ്പോൾ(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      എങ്കിൽ(linep[0]) {
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      അല്ലെങ്കിൽ
        vterm_screen_set_default_colors(screen, &fg, ശൂന്യ);
    }

    അല്ലെങ്കിൽ എങ്കിൽ(വരി[0] == '?') {
      എങ്കിൽ(streq(വരി, "?cursor")) {
        പ്രതിജ്ഞാ(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        എങ്കിൽ(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        അല്ലെങ്കിൽ എങ്കിൽ(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        അല്ലെങ്കിൽ
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "?pen ")) {
        പ്രതിജ്ഞാ(state);
        VTerm_മൂല്യം val;
        അക്ഷരം *linep = വരി + 5;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        എങ്കിൽ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          എങ്കിൽ(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          അല്ലെങ്കിൽ
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          എങ്കിൽ(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          അല്ലെങ്കിൽ
            printf("%d\n", state_pen.underline);
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          എങ്കിൽ(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          അല്ലെങ്കിൽ
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          എങ്കിൽ(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          അല്ലെങ്കിൽ
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          എങ്കിൽ(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          അല്ലെങ്കിൽ
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          എങ്കിൽ(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          അല്ലെങ്കിൽ
            printf("%d\n", state_pen.font);
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          എങ്കിൽ(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          അല്ലെങ്കിൽ
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          എങ്കിൽ(val.സംഖ്യ != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.സംഖ്യ, state_pen.baseline);
          അല്ലെങ്കിൽ
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        അല്ലെങ്കിൽ എങ്കിൽ(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        അല്ലെങ്കിൽ
          printf("?\n");
      }
      അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "?lineinfo ")) {
        പ്രതിജ്ഞാ(state);
        അക്ഷരം *linep = വരി + 10;
        പൂർണ്ണസംഖ്യ row;
        സ്ഥിരം VTerm_വരി_Info *info;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
        എങ്കിൽ(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          പോകുക abort_വരി;
        }
        info = vterm_state_get_lineinfo(state, row);
        എങ്കിൽ(info->doublewidth)
          printf("dwl ");
        എങ്കിൽ(info->doubleheight)
          printf("dhl ");
        എങ്കിൽ(info->continuation)
          printf("cont ");
        printf("\n");
      }
      അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "?screen_chars ")) {
        പ്രതിജ്ഞാ(screen);
        അക്ഷരം *linep = വരി + 13;
        VTermRect rect;
        size_t len;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
        എങ്കിൽ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        അല്ലെങ്കിൽ എങ്കിൽ(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, ശൂന്യ, &rect.end_col);
        }
        അല്ലെങ്കിൽ {
          printf("! screen_chars unrecognised input\n");
          പോകുക abort_വരി;
        }
        len = vterm_screen_get_chars(screen, ശൂന്യ, 0, rect);
        എങ്കിൽ(len == (size_t)-1)
          printf("! screen_chars error\n");
        അല്ലെങ്കിൽ എങ്കിൽ(len == 0)
          printf("\n");
        അല്ലെങ്കിൽ {
          uint32_t *chars = malloc(വലുപ്പം(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          വേണ്ടി(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          സ്വതന്ത്രം(chars);
        }
      }
      അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "?screen_text ")) {
        പ്രതിജ്ഞാ(screen);
        അക്ഷരം *linep = വരി + 12;
        VTermRect rect;
        size_t len;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
        എങ്കിൽ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          പോകുക abort_വരി;
        }
        len = vterm_screen_get_text(screen, ശൂന്യ, 0, rect);
        എങ്കിൽ(len == (size_t)-1)
          printf("! screen_text error\n");
        അല്ലെങ്കിൽ എങ്കിൽ(len == 0)
          printf("\n");
        അല്ലെങ്കിൽ {
          // Put an overwrite guard at both ends of the buffer
          unsigned അക്ഷരം *buffer = malloc(len + 4);
          unsigned അക്ഷരം *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (അക്ഷരം *)text, len, rect);

          എങ്കിൽ(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          അല്ലെങ്കിൽ എങ്കിൽ(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          അല്ലെങ്കിൽ
	  {
	    size_t i;
            വേണ്ടി(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          സ്വതന്ത്രം(buffer);
        }
      }
      അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "?screen_cell ")) {
        പ്രതിജ്ഞാ(screen);
        അക്ഷരം *linep = വരി + 12;
	പൂർണ്ണസംഖ്യ i;
        VTermPos pos;
        VTermScreenCell cell;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
        എങ്കിൽ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          പോകുക abort_വരി;
        }
        എങ്കിൽ(!vterm_screen_get_cell(screen, pos, &cell))
          പോകുക abort_വരി;
        printf("{");
        വേണ്ടി(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        എങ്കിൽ(cell.attrs.bold)      printf("B");
        എങ്കിൽ(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        എങ്കിൽ(cell.attrs.italic)    printf("I");
        എങ്കിൽ(cell.attrs.blink)     printf("K");
        എങ്കിൽ(cell.attrs.reverse)   printf("R");
        എങ്കിൽ(cell.attrs.font)      printf("F%d", cell.attrs.font);
        എങ്കിൽ(cell.attrs.small)     printf("S");
        എങ്കിൽ(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        എങ്കിൽ(cell.attrs.dwl)       printf("dwl ");
        എങ്കിൽ(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "?screen_eol ")) {
        പ്രതിജ്ഞാ(screen);
        VTermPos pos;
        അക്ഷരം *linep = വരി + 12;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
        എങ്കിൽ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          പോകുക abort_വരി;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      അല്ലെങ്കിൽ എങ്കിൽ(strstartswith(വരി, "?screen_attrs_extent ")) {
        പ്രതിജ്ഞാ(screen);
        VTermPos pos;
        VTermRect rect;
        അക്ഷരം *linep = വരി + 21;
        ആകുമ്പോൾ(linep[0] == ' ')
          linep++;
        എങ്കിൽ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          പോകുക abort_വരി;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        എങ്കിൽ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          പോകുക abort_വരി;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      അല്ലെങ്കിൽ
        printf("?\n");

      memset(വരി, 0, വലുപ്പം വരി);
      തുടരുക;
    }

    അല്ലെങ്കിൽ
      abort_വരി: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    എങ്കിൽ(outlen > 0) {
      അക്ഷരം outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, ശൂന്യ);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  തിരികെ 0;
}
