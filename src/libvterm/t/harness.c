#ಅನ್ತರ್ಭಾವ "vterm.h"
#ಅನ್ತರ್ಭಾವ "../src/vterm_internal.h" // We pull in some internal bits too

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

ಸ್ಥಿರ size_t inplace_hex2bytes(ವರ್ಣ *s)
{
  ವರ್ಣ *inpos = s, *outpos = s;

  ಯಾವತ್(*inpos) {
    unsigned ಪೂರ್ಣಾಙ್ಕ ch;
    ಯದಿ(sscanf(inpos, "%2x", &ch) < 1)
      ಭಙ್ಗ;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ಪ್ರತ್ಯಾಗಮ outpos - s;
}

ಸ್ಥಿರ VTermModifier strpe_modifiers(ವರ್ಣ **strp)
{
  VTermModifier state = 0;

  ಯಾವತ್((*strp)[0]) {
    ವಿಕಲ್ಪ(((*strp)++)[0]) {
      ಪಕ್ಷ 'S': state |= VTERM_MOD_SHIFT; ಭಙ್ಗ;
      ಪಕ್ಷ 'C': state |= VTERM_MOD_CTRL;  ಭಙ್ಗ;
      ಪಕ್ಷ 'A': state |= VTERM_MOD_ALT;   ಭಙ್ಗ;
      ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ state;
    }
  }

  ಪ್ರತ್ಯಾಗಮ state;
}

ಸ್ಥಿರ VTerm_ಕೀಲಿ strp_ಕೀಲಿ(ವರ್ಣ *str)
{
  ಸ್ಥಿರ ಸಂರಚನಾ {
    ವರ್ಣ *name;
    VTerm_ಕೀಲಿ ಕೀಲಿ;
  } ಕೀಲಿಗಳು[] = {
    { "Up",    VTERM_ಕೀಲಿ_UP },
    { "Tab",   VTERM_ಕೀಲಿ_TAB },
    { "Enter", VTERM_ಕೀಲಿ_ENTER },
    { "KP0",   VTERM_ಕೀಲಿ_KP_0 },
    { "F1",    VTERM_ಕೀಲಿ_FUNCTION(1) },
    { ರಿಕ್ತ,    VTERM_ಕೀಲಿ_NONE },
  };
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ(i = 0; ಕೀಲಿಗಳು[i].name; i++) {
    ಯದಿ(streq(str, ಕೀಲಿಗಳು[i].name))
      ಪ್ರತ್ಯಾಗಮ ಕೀಲಿಗಳು[i].ಕೀಲಿ;
  }

  ಪ್ರತ್ಯಾಗಮ VTERM_ಕೀಲಿ_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ print_color(ಧ್ರುವ VTermColor *col)
{
  ಯದಿ (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  ಅನ್ಯಥಾ ಯದಿ (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  ಅನ್ಯಥಾ {
    printf("invalid(%d", col->type);
  }
  ಯದಿ (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  ಯದಿ (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

ಸ್ಥಿರ VTermColor strpe_color(ವರ್ಣ **strp)
{
  uint8_t r, g, b, idx;
  ಪೂರ್ಣಾಙ್ಕ len = 0;
  VTermColor col;

  ಯದಿ(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  ಅನ್ಯಥಾ ಯದಿ(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  ಅನ್ಯಥಾ
    vterm_color_rgb(&col, 127, 127, 127);

  ಪ್ರತ್ಯಾಗಮ col;
}

ಸ್ಥಿರ VTerm *vt;
ಸ್ಥಿರ VTermState *state;
ಸ್ಥಿರ VTermScreen *screen;

ಸ್ಥಿರ VTermEncodingInstance encoding;

ಸ್ಥಿರ ಶೂನ್ಯ term_output(ಧ್ರುವ ವರ್ಣ *s, size_t len, ಶೂನ್ಯ *user UNUSED)
{
  size_t i;

  printf("output ");
  ಪ್ರತಿ(i = 0; i < len; i++)
    printf("%x%s", (unsigned ವರ್ಣ)s[i], i < len-1 ? "," : "\n");
}

ಸ್ಥಿರ ಶೂನ್ಯ printhex(ಧ್ರುವ ವರ್ಣ *s, size_t len)
{
  ಯಾವತ್(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_text(ಧ್ರುವ ವರ್ಣ bytes[], size_t len, ಶೂನ್ಯ *user UNUSED)
{
  size_t i;

  printf("text ");
  ಪ್ರತಿ(i = 0; i < len; i++) {
    unsigned ವರ್ಣ b = bytes[i];
    ಯದಿ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      ಭಙ್ಗ;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ಪ್ರತ್ಯಾಗಮ i;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_control(unsigned ವರ್ಣ control, ಶೂನ್ಯ *user UNUSED)
{
  printf("control %02x\n", control);

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_escape(ಧ್ರುವ ವರ್ಣ bytes[], size_t len, ಶೂನ್ಯ *user UNUSED)
{
  ಯದಿ(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    ಯದಿ(len < 2)
      ಪ್ರತ್ಯಾಗಮ -1;
    len = 2;
  }
  ಅನ್ಯಥಾ {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ಪ್ರತ್ಯಾಗಮ len;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_csi(ಧ್ರುವ ವರ್ಣ *leader, ಧ್ರುವ long args[], ಪೂರ್ಣಾಙ್ಕ argcount, ಧ್ರುವ ವರ್ಣ *intermed, ವರ್ಣ command, ಶೂನ್ಯ *user UNUSED)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  printf("csi %02x", command);

  ಯದಿ(leader && leader[0]) {
    printf(" L=");
    ಪ್ರತಿ(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  ಪ್ರತಿ(i = 0; i < argcount; i++) {
    ವರ್ಣ sep = i ? ',' : ' ';

    ಯದಿ(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    ಅನ್ಯಥಾ
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  ಯದಿ(intermed && intermed[0]) {
    printf(" I=");
    ಪ್ರತಿ(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_osc(ಪೂರ್ಣಾಙ್ಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user UNUSED)
{

  printf("osc ");

  ಯದಿ(frag.initial) {
    ಯದಿ(command == -1)
      printf("[");
    ಅನ್ಯಥಾ
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ಯದಿ(frag.final)
    printf("]");

  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_dcs(ಧ್ರುವ ವರ್ಣ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user UNUSED)
{
  printf("dcs ");

  ಯದಿ(frag.initial) {
    size_t i;
    printf("[");
    ಪ್ರತಿ(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ಯದಿ(frag.final)
    printf("]");

  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_apc(VTermStringFragment frag, ಶೂನ್ಯ *user UNUSED)
{
  printf("apc ");

  ಯದಿ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ಯದಿ(frag.final)
    printf("]");

  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_pm(VTermStringFragment frag, ಶೂನ್ಯ *user UNUSED)
{
  printf("pm ");

  ಯದಿ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ಯದಿ(frag.final)
    printf("]");

  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parser_sos(VTermStringFragment frag, ಶೂನ್ಯ *user UNUSED)
{
  printf("sos ");

  ಯದಿ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ಯದಿ(frag.final)
    printf("]");

  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ರಿಕ್ತ // resize
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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_movecursor = 0;
ಸ್ಥಿರ VTermPos state_pos;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ movecursor(VTermPos pos, VTermPos oldpos UNUSED, ಪೂರ್ಣಾಙ್ಕ visible UNUSED, ಶೂನ್ಯ *user UNUSED)
{
  state_pos = pos;

  ಯದಿ(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_premove = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ premove(VTermRect rect, ಶೂನ್ಯ *user UNUSED)
{
  ಯದಿ(!want_premove)
    ಪ್ರತ್ಯಾಗಮ 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_scrollrect = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scrollrect(VTermRect rect, ಪೂರ್ಣಾಙ್ಕ downward, ಪೂರ್ಣಾಙ್ಕ rightward, ಶೂನ್ಯ *user UNUSED)
{
  ಯದಿ(!want_scrollrect)
    ಪ್ರತ್ಯಾಗಮ 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_moverect = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ moverect(VTermRect dest, VTermRect src, ಶೂನ್ಯ *user UNUSED)
{
  ಯದಿ(!want_moverect)
    ಪ್ರತ್ಯಾಗಮ 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_settermprop = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ settermprop(VTermProp prop, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user UNUSED)
{
  VTerm_ಮೌಲ್ಯ_Type type;
  ಯದಿ(!want_settermprop)
    ಪ್ರತ್ಯಾಗಮ 1;

  type = vterm_get_prop_type(prop);
  ವಿಕಲ್ಪ(type) {
  ಪಕ್ಷ VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->ಸಂಖ್ಯೆ);
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_N_VALUETYPES:
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

// These callbacks are for State

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_state_putglyph = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ state_putglyph(VTermGlyphInfo *info, VTermPos pos, ಶೂನ್ಯ *user UNUSED)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಯದಿ(!want_state_putglyph)
    ಪ್ರತ್ಯಾಗಮ 1;

  printf("putglyph ");
  ಪ್ರತಿ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  ಯದಿ(info->protected_cell)
    printf(" prot");
  ಯದಿ(info->dwl)
    printf(" dwl");
  ಯದಿ(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_state_erase = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ state_erase(VTermRect rect, ಪೂರ್ಣಾಙ್ಕ selective, ಶೂನ್ಯ *user UNUSED)
{
  ಯದಿ(!want_state_erase)
    ಪ್ರತ್ಯಾಗಮ 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಸಂರಚನಾ {
  ಪೂರ್ಣಾಙ್ಕ bold;
  ಪೂರ್ಣಾಙ್ಕ underline;
  ಪೂರ್ಣಾಙ್ಕ italic;
  ಪೂರ್ಣಾಙ್ಕ blink;
  ಪೂರ್ಣಾಙ್ಕ reverse;
  ಪೂರ್ಣಾಙ್ಕ conceal;
  ಪೂರ್ಣಾಙ್ಕ strike;
  ಪೂರ್ಣಾಙ್ಕ font;
  ಪೂರ್ಣಾಙ್ಕ small;
  ಪೂರ್ಣಾಙ್ಕ baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ state_setpenattr(VTermAttr attr, VTerm_ಮೌಲ್ಯ *val, ಶೂನ್ಯ *user UNUSED)
{
  ವಿಕಲ್ಪ(attr) {
  ಪಕ್ಷ VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->ಸಂಖ್ಯೆ;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_FONT:
    state_pen.font = val->ಸಂಖ್ಯೆ;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_BASELINE:
    state_pen.baseline = val->ಸಂಖ್ಯೆ;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    ಭಙ್ಗ;
  ಪಕ್ಷ VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    ಭಙ್ಗ;

  ಪಕ್ಷ VTERM_N_ATTRS:
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ state_setlineinfo(ಪೂರ್ಣಾಙ್ಕ row UNUSED, ಧ್ರುವ VTerm_ಸಾಲು_Info *newinfo UNUSED, ಧ್ರುವ VTerm_ಸಾಲು_Info *oldinfo UNUSED, ಶೂನ್ಯ *user UNUSED)
{
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_state_scrollback = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ state_sb_clear(ಶೂನ್ಯ *user UNUSED) {
  ಯದಿ(!want_state_scrollback)
    ಪ್ರತ್ಯಾಗಮ 1;

  printf("sb_clear\n");
  ಪ್ರತ್ಯಾಗಮ 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ರಿಕ್ತ, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ರಿಕ್ತ, // bell
  ರಿಕ್ತ, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ selection_set(VTermSelectionMask mask, VTermStringFragment frag, ಶೂನ್ಯ *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  ಯದಿ(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  ಯದಿ(frag.final)
    printf("]");
  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ selection_query(VTermSelectionMask mask, ಶೂನ್ಯ *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ಪ್ರತ್ಯಾಗಮ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_screen_damage = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_screen_damage_cells = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ screen_damage(VTermRect rect, ಶೂನ್ಯ *user UNUSED)
{
  ಯದಿ(!want_screen_damage)
    ಪ್ರತ್ಯಾಗಮ 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ಯದಿ(want_screen_damage_cells) {
    ಪೂರ್ಣಾಙ್ಕ equals = FALSE;
    ಪೂರ್ಣಾಙ್ಕ row;
    ಪೂರ್ಣಾಙ್ಕ col;

    ಪ್ರತಿ(row = rect.start_row; row < rect.end_row; row++) {
      ಪೂರ್ಣಾಙ್ಕ eol = rect.end_col;
      ಯಾವತ್(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ಯದಿ(cell.chars[0])
          ಭಙ್ಗ;

        eol--;
      }

      ಯದಿ(eol == rect.start_col)
        ಭಙ್ಗ;

      ಯದಿ(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      ಪ್ರತಿ(col = rect.start_col; col < eol; col++) {
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

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ want_screen_scrollback = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ screen_sb_pushline4(ಪೂರ್ಣಾಙ್ಕ cols, ಧ್ರುವ VTermScreenCell *cells, ಪೂರ್ಣಾಙ್ಕ continuation, ಶೂನ್ಯ *user UNUSED)
{
  ಪೂರ್ಣಾಙ್ಕ eol;
  ಪೂರ್ಣಾಙ್ಕ c;

  ಯದಿ(!want_screen_scrollback)
    ಪ್ರತ್ಯಾಗಮ 1;

  eol = cols;
  ಯಾವತ್(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  ಪ್ರತಿ(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ screen_sb_popline(ಪೂರ್ಣಾಙ್ಕ cols, VTermScreenCell *cells, ಶೂನ್ಯ *user UNUSED)
{
  ಪೂರ್ಣಾಙ್ಕ col;

  ಯದಿ(!want_screen_scrollback)
    ಪ್ರತ್ಯಾಗಮ 0;

  // All lines of scrollback contain "ABCDE"
  ಪ್ರತಿ(col = 0; col < cols; col++) {
    ಯದಿ(col < 5)
      cells[col].chars[0] = 'A' + col;
    ಅನ್ಯಥಾ
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ screen_sb_clear(ಶೂನ್ಯ *user UNUSED)
{
  ಯದಿ(!want_screen_scrollback)
    ಪ್ರತ್ಯಾಗಮ 1;

  printf("sb_clear\n");
  ಪ್ರತ್ಯಾಗಮ 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ರಿಕ್ತ, // bell
  ರಿಕ್ತ, // resize
  ರಿಕ್ತ, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

ಪೂರ್ಣಾಙ್ಕ main(ಪೂರ್ಣಾಙ್ಕ argc UNUSED, ವರ್ಣ **argv UNUSED)
{
  ವರ್ಣ ಸಾಲು[1024] = {0};
  ಪೂರ್ಣಾಙ್ಕ flag;

  ಪೂರ್ಣಾಙ್ಕ err;

  setvbuf(stdout, ರಿಕ್ತ, _IONBF, 0);

  ಯಾವತ್(fgets(ಸಾಲು, ಪರಿಮಾಣ ಸಾಲು, stdin)) {
    ವರ್ಣ *nl;
    size_t outlen;
    err = 0;

    ಯದಿ((nl = strchr(ಸಾಲು, '\n')))
      *nl = '\0';

    ಯದಿ(streq(ಸಾಲು, "INIT")) {
      ಯದಿ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    ಅನ್ಯಥಾ ಯದಿ(streq(ಸಾಲು, "WANTPARSER")) {
      ಪ್ರತಿಜ್ಞಾ(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, ರಿಕ್ತ);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "WANTSTATE") && (ಸಾಲು[9] == '\0' || ಸಾಲು[9] == ' ')) {
      ಪೂರ್ಣಾಙ್ಕ i = 9;
      ಪೂರ್ಣಾಙ್ಕ sense = 1;
      ಪ್ರತಿಜ್ಞಾ(vt);
      ಯದಿ(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, ರಿಕ್ತ);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, ರಿಕ್ತ, ರಿಕ್ತ, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      ಯಾವತ್(ಸಾಲು[i] == ' ')
        i++;
      ಪ್ರತಿ( ; ಸಾಲು[i]; i++)
        ವಿಕಲ್ಪ(ಸಾಲು[i]) {
        ಪಕ್ಷ '+':
          sense = 1;
          ಭಙ್ಗ;
        ಪಕ್ಷ '-':
          sense = 0;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'g':
          want_state_putglyph = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 's':
          want_scrollrect = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'P':
          want_premove = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'm':
          want_moverect = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'e':
          want_state_erase = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'p':
          want_settermprop = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : ರಿಕ್ತ, ರಿಕ್ತ);
          ಭಙ್ಗ;
        ಪಕ್ಷ 'b':
          want_state_scrollback = sense;
          ಭಙ್ಗ;
        ಸ್ವಭಾವ:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", ಸಾಲು[i]);
        }
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "WANTSCREEN") && (ಸಾಲು[10] == '\0' || ಸಾಲು[10] == ' ')) {
      ಪೂರ್ಣಾಙ್ಕ i = 10;
      ಪೂರ್ಣಾಙ್ಕ sense = 1;
      ಪ್ರತಿಜ್ಞಾ(vt);
      ಯದಿ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, ರಿಕ್ತ);
      vterm_screen_callbacks_has_pushline4(screen);

      ಯಾವತ್(ಸಾಲು[i] == ' ')
        i++;
      ಪ್ರತಿ( ; ಸಾಲು[i]; i++)
        ವಿಕಲ್ಪ(ಸಾಲು[i]) {
        ಪಕ್ಷ '-':
          sense = 0;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'a':
          vterm_screen_enable_altscreen(screen, 1);
          ಭಙ್ಗ;
        ಪಕ್ಷ 'd':
          want_screen_damage = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'm':
          want_moverect = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'c':
          want_movecursor = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'p':
          want_settermprop = 1;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'b':
          want_screen_scrollback = sense;
          ಭಙ್ಗ;
        ಪಕ್ಷ 'r':
          vterm_screen_enable_reflow(screen, sense);
          ಭಙ್ಗ;
        ಸ್ವಭಾವ:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", ಸಾಲು[i]);
        }
    }

    ಅನ್ಯಥಾ ಯದಿ(sscanf(ಸಾಲು, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    ಅನ್ಯಥಾ ಯದಿ(streq(ಸಾಲು, "RESET")) {
      ಯದಿ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ಯದಿ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "RESIZE ")) {
      ಪೂರ್ಣಾಙ್ಕ rows, cols;
      ವರ್ಣ *linep = ಸಾಲು + 7;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "PUSH ")) {
      ವರ್ಣ *bytes = ಸಾಲು + 5;
      size_t len = inplace_hex2bytes(bytes);
      ಪ್ರತಿಜ್ಞಾ(len);

      size_t written = vterm_ಒಳಹರಿವು_write(vt, bytes, len);
      ಯದಿ(written < len)
        fprintf(stderr, "! short write\n");
    }

    ಅನ್ಯಥಾ ಯದಿ(streq(ಸಾಲು, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ಯದಿ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "ENCIN ")) {
      ವರ್ಣ *bytes = ಸಾಲು + 6;
      size_t len = inplace_hex2bytes(bytes);
      ಪ್ರತಿಜ್ಞಾ(len);

      uint32_t cp[1024];
      ಪೂರ್ಣಾಙ್ಕ cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      ಯದಿ(cpi > 0) {
	ಪೂರ್ಣಾಙ್ಕ i;
        printf("encout ");
        ಪ್ರತಿ(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "INCHAR ")) {
      ವರ್ಣ *linep = ಸಾಲು + 7;
      unsigned ಪೂರ್ಣಾಙ್ಕ c = 0;
      VTermModifier mod;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "INKEY ")) {
      VTermModifier mod;
      VTerm_ಕೀಲಿ ಕೀಲಿ;
      ವರ್ಣ *linep = ಸಾಲು + 6;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      ಕೀಲಿ = strp_ಕೀಲಿ(linep);

      vterm_keyboard_ಕೀಲಿ(vt, ಕೀಲಿ, mod);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "PASTE ")) {
      ವರ್ಣ *linep = ಸಾಲು + 6;
      ಯದಿ(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      ಅನ್ಯಥಾ ಯದಿ(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      ಅನ್ಯಥಾ
        ಗಚ್ಛ abort_ಸಾಲು;
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "FOCUS ")) {
      ಪ್ರತಿಜ್ಞಾ(state);
      ವರ್ಣ *linep = ಸಾಲು + 6;
      ಯದಿ(streq(linep, "IN"))
        vterm_state_focus_in(state);
      ಅನ್ಯಥಾ ಯದಿ(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      ಅನ್ಯಥಾ
        ಗಚ್ಛ abort_ಸಾಲು;
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "MOUSEMOVE ")) {
      ವರ್ಣ *linep = ಸಾಲು + 10;
      ಪೂರ್ಣಾಙ್ಕ row, col, len;
      VTermModifier mod;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "MOUSEBTN ")) {
      ವರ್ಣ *linep = ಸಾಲು + 9;
      ವರ್ಣ press;
      ಪೂರ್ಣಾಙ್ಕ button, len;
      VTermModifier mod;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "SELECTION ")) {
      ವರ್ಣ *linep = ಸಾಲು + 10;
      unsigned ಪೂರ್ಣಾಙ್ಕ mask;
      ಪೂರ್ಣಾಙ್ಕ len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      ಯದಿ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ಯಾವತ್(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      ಪ್ರತಿಜ್ಞಾ(frag.len);

      linep += frag.len * 2;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      ಯದಿ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "DAMAGEMERGE ")) {
      ಪ್ರತಿಜ್ಞಾ(screen);
      ವರ್ಣ *linep = ಸಾಲು + 12;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      ಯದಿ(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      ಅನ್ಯಥಾ ಯದಿ(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      ಅನ್ಯಥಾ ಯದಿ(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      ಅನ್ಯಥಾ ಯದಿ(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "DAMAGEFLUSH")) {
      ಪ್ರತಿಜ್ಞಾ(screen);
      vterm_screen_flush_damage(screen);
    }

    ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "SETDEFAULTCOL ")) {
      ಪ್ರತಿಜ್ಞಾ(screen);
      ವರ್ಣ *linep = ಸಾಲು + 14;
      ಯಾವತ್(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ಯದಿ(linep[0]) {
        ಯಾವತ್(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      ಅನ್ಯಥಾ
        vterm_screen_set_default_colors(screen, &fg, ರಿಕ್ತ);
    }

    ಅನ್ಯಥಾ ಯದಿ(ಸಾಲು[0] == '?') {
      ಯದಿ(streq(ಸಾಲು, "?cursor")) {
        ಪ್ರತಿಜ್ಞಾ(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ಯದಿ(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ಅನ್ಯಥಾ ಯದಿ(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ಅನ್ಯಥಾ
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "?pen ")) {
        ಪ್ರತಿಜ್ಞಾ(state);
        VTerm_ಮೌಲ್ಯ val;
        ವರ್ಣ *linep = ಸಾಲು + 5;
        ಯಾವತ್(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        ಯದಿ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ಯದಿ(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          ಅನ್ಯಥಾ
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ಯದಿ(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          ಅನ್ಯಥಾ
            printf("%d\n", state_pen.underline);
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ಯದಿ(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          ಅನ್ಯಥಾ
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ಯದಿ(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          ಅನ್ಯಥಾ
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          ಯದಿ(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          ಅನ್ಯಥಾ
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ಯದಿ(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          ಅನ್ಯಥಾ
            printf("%d\n", state_pen.font);
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ಯದಿ(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          ಅನ್ಯಥಾ
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ಯದಿ(val.ಸಂಖ್ಯೆ != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.ಸಂಖ್ಯೆ, state_pen.baseline);
          ಅನ್ಯಥಾ
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        ಅನ್ಯಥಾ ಯದಿ(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        ಅನ್ಯಥಾ
          printf("?\n");
      }
      ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "?lineinfo ")) {
        ಪ್ರತಿಜ್ಞಾ(state);
        ವರ್ಣ *linep = ಸಾಲು + 10;
        ಪೂರ್ಣಾಙ್ಕ row;
        ಧ್ರುವ VTerm_ಸಾಲು_Info *info;
        ಯಾವತ್(linep[0] == ' ')
          linep++;
        ಯದಿ(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          ಗಚ್ಛ abort_ಸಾಲು;
        }
        info = vterm_state_get_lineinfo(state, row);
        ಯದಿ(info->doublewidth)
          printf("dwl ");
        ಯದಿ(info->doubleheight)
          printf("dhl ");
        ಯದಿ(info->continuation)
          printf("cont ");
        printf("\n");
      }
      ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "?screen_chars ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        ವರ್ಣ *linep = ಸಾಲು + 13;
        VTermRect rect;
        size_t len;
        ಯಾವತ್(linep[0] == ' ')
          linep++;
        ಯದಿ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        ಅನ್ಯಥಾ ಯದಿ(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, ರಿಕ್ತ, &rect.end_col);
        }
        ಅನ್ಯಥಾ {
          printf("! screen_chars unrecognised input\n");
          ಗಚ್ಛ abort_ಸಾಲು;
        }
        len = vterm_screen_get_chars(screen, ರಿಕ್ತ, 0, rect);
        ಯದಿ(len == (size_t)-1)
          printf("! screen_chars error\n");
        ಅನ್ಯಥಾ ಯದಿ(len == 0)
          printf("\n");
        ಅನ್ಯಥಾ {
          uint32_t *chars = malloc(ಪರಿಮಾಣ(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          ಪ್ರತಿ(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          ಮುಕ್ತ(chars);
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "?screen_text ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        ವರ್ಣ *linep = ಸಾಲು + 12;
        VTermRect rect;
        size_t len;
        ಯಾವತ್(linep[0] == ' ')
          linep++;
        ಯದಿ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          ಗಚ್ಛ abort_ಸಾಲು;
        }
        len = vterm_screen_get_text(screen, ರಿಕ್ತ, 0, rect);
        ಯದಿ(len == (size_t)-1)
          printf("! screen_text error\n");
        ಅನ್ಯಥಾ ಯದಿ(len == 0)
          printf("\n");
        ಅನ್ಯಥಾ {
          // Put an overwrite guard at both ends of the buffer
          unsigned ವರ್ಣ *buffer = malloc(len + 4);
          unsigned ವರ್ಣ *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (ವರ್ಣ *)text, len, rect);

          ಯದಿ(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          ಅನ್ಯಥಾ ಯದಿ(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          ಅನ್ಯಥಾ
	  {
	    size_t i;
            ಪ್ರತಿ(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          ಮುಕ್ತ(buffer);
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "?screen_cell ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        ವರ್ಣ *linep = ಸಾಲು + 12;
	ಪೂರ್ಣಾಙ್ಕ i;
        VTermPos pos;
        VTermScreenCell cell;
        ಯಾವತ್(linep[0] == ' ')
          linep++;
        ಯದಿ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          ಗಚ್ಛ abort_ಸಾಲು;
        }
        ಯದಿ(!vterm_screen_get_cell(screen, pos, &cell))
          ಗಚ್ಛ abort_ಸಾಲು;
        printf("{");
        ಪ್ರತಿ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        ಯದಿ(cell.attrs.bold)      printf("B");
        ಯದಿ(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        ಯದಿ(cell.attrs.italic)    printf("I");
        ಯದಿ(cell.attrs.blink)     printf("K");
        ಯದಿ(cell.attrs.reverse)   printf("R");
        ಯದಿ(cell.attrs.font)      printf("F%d", cell.attrs.font);
        ಯದಿ(cell.attrs.small)     printf("S");
        ಯದಿ(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        ಯದಿ(cell.attrs.dwl)       printf("dwl ");
        ಯದಿ(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "?screen_eol ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        VTermPos pos;
        ವರ್ಣ *linep = ಸಾಲು + 12;
        ಯಾವತ್(linep[0] == ' ')
          linep++;
        ಯದಿ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          ಗಚ್ಛ abort_ಸಾಲು;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      ಅನ್ಯಥಾ ಯದಿ(strstartswith(ಸಾಲು, "?screen_attrs_extent ")) {
        ಪ್ರತಿಜ್ಞಾ(screen);
        VTermPos pos;
        VTermRect rect;
        ವರ್ಣ *linep = ಸಾಲು + 21;
        ಯಾವತ್(linep[0] == ' ')
          linep++;
        ಯದಿ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          ಗಚ್ಛ abort_ಸಾಲು;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        ಯದಿ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          ಗಚ್ಛ abort_ಸಾಲು;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      ಅನ್ಯಥಾ
        printf("?\n");

      memset(ಸಾಲು, 0, ಪರಿಮಾಣ ಸಾಲು);
      ಅನುವರ್ತ;
    }

    ಅನ್ಯಥಾ
      abort_ಸಾಲು: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    ಯದಿ(outlen > 0) {
      ವರ್ಣ outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, ರಿಕ್ತ);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ಪ್ರತ್ಯಾಗಮ 0;
}
