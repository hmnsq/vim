#ଅନ୍ତର୍ଭାଵ "vterm.h"
#ଅନ୍ତର୍ଭାଵ "../src/vterm_internal.h" // We pull in some internal bits too

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

ସ୍ଥିର size_t inplace_hex2bytes(ଅକ୍ଷର *s)
{
  ଅକ୍ଷର *inpos = s, *outpos = s;

  ଯେତେବେଳେ(*inpos) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ch;
    ଯଦି(sscanf(inpos, "%2x", &ch) < 1)
      ଭାଙ୍ଗ;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ଫେରନ୍ତୁ outpos - s;
}

ସ୍ଥିର VTermModifier strpe_modifiers(ଅକ୍ଷର **strp)
{
  VTermModifier state = 0;

  ଯେତେବେଳେ((*strp)[0]) {
    ବାଛ(((*strp)++)[0]) {
      ସ୍ଥିତି 'S': state |= VTERM_MOD_SHIFT; ଭାଙ୍ଗ;
      ସ୍ଥିତି 'C': state |= VTERM_MOD_CTRL;  ଭାଙ୍ଗ;
      ସ୍ଥିତି 'A': state |= VTERM_MOD_ALT;   ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ state;
    }
  }

  ଫେରନ୍ତୁ state;
}

ସ୍ଥିର VTerm_ଚାବି strp_ଚାବି(ଅକ୍ଷର *str)
{
  ସ୍ଥିର ସଂରଚନା {
    ଅକ୍ଷର *name;
    VTerm_ଚାବି ଚାବି;
  } ଚାବିଗୁଡ଼ିକ[] = {
    { "Up",    VTERM_ଚାବି_UP },
    { "Tab",   VTERM_ଚାବି_TAB },
    { "Enter", VTERM_ଚାବି_ENTER },
    { "KP0",   VTERM_ଚାବି_KP_0 },
    { "F1",    VTERM_ଚାବି_FUNCTION(1) },
    { ଖାଲି,    VTERM_ଚାବି_NONE },
  };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ(i = 0; ଚାବିଗୁଡ଼ିକ[i].name; i++) {
    ଯଦି(streq(str, ଚାବିଗୁଡ଼ିକ[i].name))
      ଫେରନ୍ତୁ ଚାବିଗୁଡ଼ିକ[i].ଚାବି;
  }

  ଫେରନ୍ତୁ VTERM_ଚାବି_NONE;
}

ସ୍ଥିର ଶୂନ୍ୟ print_color(ଅଚଳ VTermColor *col)
{
  ଯଦି (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  ଅନ୍ୟଥା ଯଦି (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  ଅନ୍ୟଥା {
    printf("invalid(%d", col->type);
  }
  ଯଦି (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  ଯଦି (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

ସ୍ଥିର VTermColor strpe_color(ଅକ୍ଷର **strp)
{
  uint8_t r, g, b, idx;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = 0;
  VTermColor col;

  ଯଦି(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  ଅନ୍ୟଥା ଯଦି(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  ଅନ୍ୟଥା
    vterm_color_rgb(&col, 127, 127, 127);

  ଫେରନ୍ତୁ col;
}

ସ୍ଥିର VTerm *vt;
ସ୍ଥିର VTermState *state;
ସ୍ଥିର VTermScreen *screen;

ସ୍ଥିର VTermEncodingInstance encoding;

ସ୍ଥିର ଶୂନ୍ୟ term_output(ଅଚଳ ଅକ୍ଷର *s, size_t len, ଶୂନ୍ୟ *user UNUSED)
{
  size_t i;

  printf("output ");
  ପାଇଁ(i = 0; i < len; i++)
    printf("%x%s", (unsigned ଅକ୍ଷର)s[i], i < len-1 ? "," : "\n");
}

ସ୍ଥିର ଶୂନ୍ୟ printhex(ଅଚଳ ଅକ୍ଷର *s, size_t len)
{
  ଯେତେବେଳେ(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_text(ଅଚଳ ଅକ୍ଷର bytes[], size_t len, ଶୂନ୍ୟ *user UNUSED)
{
  size_t i;

  printf("text ");
  ପାଇଁ(i = 0; i < len; i++) {
    unsigned ଅକ୍ଷର b = bytes[i];
    ଯଦି(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      ଭାଙ୍ଗ;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ଫେରନ୍ତୁ i;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_control(unsigned ଅକ୍ଷର control, ଶୂନ୍ୟ *user UNUSED)
{
  printf("control %02x\n", control);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_escape(ଅଚଳ ଅକ୍ଷର bytes[], size_t len, ଶୂନ୍ୟ *user UNUSED)
{
  ଯଦି(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    ଯଦି(len < 2)
      ଫେରନ୍ତୁ -1;
    len = 2;
  }
  ଅନ୍ୟଥା {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ଫେରନ୍ତୁ len;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_csi(ଅଚଳ ଅକ୍ଷର *leader, ଅଚଳ long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ଅଚଳ ଅକ୍ଷର *intermed, ଅକ୍ଷର command, ଶୂନ୍ୟ *user UNUSED)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  printf("csi %02x", command);

  ଯଦି(leader && leader[0]) {
    printf(" L=");
    ପାଇଁ(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  ପାଇଁ(i = 0; i < argcount; i++) {
    ଅକ୍ଷର sep = i ? ',' : ' ';

    ଯଦି(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    ଅନ୍ୟଥା
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  ଯଦି(intermed && intermed[0]) {
    printf(" I=");
    ପାଇଁ(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_osc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା command, VTermStringFragment frag, ଶୂନ୍ୟ *user UNUSED)
{

  printf("osc ");

  ଯଦି(frag.initial) {
    ଯଦି(command == -1)
      printf("[");
    ଅନ୍ୟଥା
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ଯଦି(frag.final)
    printf("]");

  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_dcs(ଅଚଳ ଅକ୍ଷର *command, size_t commandlen, VTermStringFragment frag, ଶୂନ୍ୟ *user UNUSED)
{
  printf("dcs ");

  ଯଦି(frag.initial) {
    size_t i;
    printf("[");
    ପାଇଁ(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ଯଦି(frag.final)
    printf("]");

  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_apc(VTermStringFragment frag, ଶୂନ୍ୟ *user UNUSED)
{
  printf("apc ");

  ଯଦି(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ଯଦି(frag.final)
    printf("]");

  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_pm(VTermStringFragment frag, ଶୂନ୍ୟ *user UNUSED)
{
  printf("pm ");

  ଯଦି(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ଯଦି(frag.final)
    printf("]");

  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parser_sos(VTermStringFragment frag, ଶୂନ୍ୟ *user UNUSED)
{
  printf("sos ");

  ଯଦି(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ଯଦି(frag.final)
    printf("]");

  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ଖାଲି // resize
};

ସ୍ଥିର VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_movecursor = 0;
ସ୍ଥିର VTermPos state_pos;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା movecursor(VTermPos pos, VTermPos oldpos UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା visible UNUSED, ଶୂନ୍ୟ *user UNUSED)
{
  state_pos = pos;

  ଯଦି(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_premove = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା premove(VTermRect rect, ଶୂନ୍ୟ *user UNUSED)
{
  ଯଦି(!want_premove)
    ଫେରନ୍ତୁ 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_scrollrect = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scrollrect(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rightward, ଶୂନ୍ୟ *user UNUSED)
{
  ଯଦି(!want_scrollrect)
    ଫେରନ୍ତୁ 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_moverect = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା moverect(VTermRect dest, VTermRect src, ଶୂନ୍ୟ *user UNUSED)
{
  ଯଦି(!want_moverect)
    ଫେରନ୍ତୁ 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_settermprop = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା settermprop(VTermProp prop, VTerm_ମୂଲ୍ୟ *val, ଶୂନ୍ୟ *user UNUSED)
{
  VTerm_ମୂଲ୍ୟ_Type type;
  ଯଦି(!want_settermprop)
    ଫେରନ୍ତୁ 1;

  type = vterm_get_prop_type(prop);
  ବାଛ(type) {
  ସ୍ଥିତି VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->ସଂଖ୍ୟା);
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_N_VALUETYPES:
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ 0;
}

// These callbacks are for State

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_state_putglyph = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା state_putglyph(VTermGlyphInfo *info, VTermPos pos, ଶୂନ୍ୟ *user UNUSED)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ଯଦି(!want_state_putglyph)
    ଫେରନ୍ତୁ 1;

  printf("putglyph ");
  ପାଇଁ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  ଯଦି(info->protected_cell)
    printf(" prot");
  ଯଦି(info->dwl)
    printf(" dwl");
  ଯଦି(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_state_erase = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା state_erase(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective, ଶୂନ୍ୟ *user UNUSED)
{
  ଯଦି(!want_state_erase)
    ଫେରନ୍ତୁ 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ସଂରଚନା {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା underline;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା italic;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା blink;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା reverse;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା conceal;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା strike;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା font;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା small;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା state_setpenattr(VTermAttr attr, VTerm_ମୂଲ୍ୟ *val, ଶୂନ୍ୟ *user UNUSED)
{
  ବାଛ(attr) {
  ସ୍ଥିତି VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->ସଂଖ୍ୟା;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_FONT:
    state_pen.font = val->ସଂଖ୍ୟା;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_BASELINE:
    state_pen.baseline = val->ସଂଖ୍ୟା;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି VTERM_N_ATTRS:
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା state_setlineinfo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row UNUSED, ଅଚଳ VTerm_ଧାଡ଼ି_Info *newinfo UNUSED, ଅଚଳ VTerm_ଧାଡ଼ି_Info *oldinfo UNUSED, ଶୂନ୍ୟ *user UNUSED)
{
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_state_scrollback = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା state_sb_clear(ଶୂନ୍ୟ *user UNUSED) {
  ଯଦି(!want_state_scrollback)
    ଫେରନ୍ତୁ 1;

  printf("sb_clear\n");
  ଫେରନ୍ତୁ 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ଖାଲି, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ଖାଲି, // bell
  ଖାଲି, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା selection_set(VTermSelectionMask mask, VTermStringFragment frag, ଶୂନ୍ୟ *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  ଯଦି(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  ଯଦି(frag.final)
    printf("]");
  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା selection_query(VTermSelectionMask mask, ଶୂନ୍ୟ *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ଫେରନ୍ତୁ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_screen_damage = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_screen_damage_cells = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_damage(VTermRect rect, ଶୂନ୍ୟ *user UNUSED)
{
  ଯଦି(!want_screen_damage)
    ଫେରନ୍ତୁ 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ଯଦି(want_screen_damage_cells) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା equals = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;

    ପାଇଁ(row = rect.start_row; row < rect.end_row; row++) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା eol = rect.end_col;
      ଯେତେବେଳେ(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ଯଦି(cell.chars[0])
          ଭାଙ୍ଗ;

        eol--;
      }

      ଯଦି(eol == rect.start_col)
        ଭାଙ୍ଗ;

      ଯଦି(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      ପାଇଁ(col = rect.start_col; col < eol; col++) {
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

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା want_screen_scrollback = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_sb_pushline4(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଅଚଳ VTermScreenCell *cells, ପୂର୍ଣ୍ଣସଂଖ୍ୟା continuation, ଶୂନ୍ୟ *user UNUSED)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା eol;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା c;

  ଯଦି(!want_screen_scrollback)
    ଫେରନ୍ତୁ 1;

  eol = cols;
  ଯେତେବେଳେ(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  ପାଇଁ(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_sb_popline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, VTermScreenCell *cells, ଶୂନ୍ୟ *user UNUSED)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;

  ଯଦି(!want_screen_scrollback)
    ଫେରନ୍ତୁ 0;

  // All lines of scrollback contain "ABCDE"
  ପାଇଁ(col = 0; col < cols; col++) {
    ଯଦି(col < 5)
      cells[col].chars[0] = 'A' + col;
    ଅନ୍ୟଥା
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା screen_sb_clear(ଶୂନ୍ୟ *user UNUSED)
{
  ଯଦି(!want_screen_scrollback)
    ଫେରନ୍ତୁ 1;

  printf("sb_clear\n");
  ଫେରନ୍ତୁ 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ଖାଲି, // bell
  ଖାଲି, // resize
  ଖାଲି, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୁଖ୍ୟ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, ଅକ୍ଷର **argv UNUSED)
{
  ଅକ୍ଷର ଧାଡ଼ି[1024] = {0};
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା err;

  setvbuf(stdout, ଖାଲି, _IONBF, 0);

  ଯେତେବେଳେ(fgets(ଧାଡ଼ି, ଆକାର ଧାଡ଼ି, stdin)) {
    ଅକ୍ଷର *nl;
    size_t outlen;
    err = 0;

    ଯଦି((nl = strchr(ଧାଡ଼ି, '\n')))
      *nl = '\0';

    ଯଦି(streq(ଧାଡ଼ି, "INIT")) {
      ଯଦି(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    ଅନ୍ୟଥା ଯଦି(streq(ଧାଡ଼ି, "WANTPARSER")) {
      ପ୍ରତିଜ୍ଞା(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, ଖାଲି);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "WANTSTATE") && (ଧାଡ଼ି[9] == '\0' || ଧାଡ଼ି[9] == ' ')) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 9;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sense = 1;
      ପ୍ରତିଜ୍ଞା(vt);
      ଯଦି(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, ଖାଲି);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, ଖାଲି, ଖାଲି, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      ଯେତେବେଳେ(ଧାଡ଼ି[i] == ' ')
        i++;
      ପାଇଁ( ; ଧାଡ଼ି[i]; i++)
        ବାଛ(ଧାଡ଼ି[i]) {
        ସ୍ଥିତି '+':
          sense = 1;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି '-':
          sense = 0;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'g':
          want_state_putglyph = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 's':
          want_scrollrect = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'P':
          want_premove = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'm':
          want_moverect = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'e':
          want_state_erase = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'p':
          want_settermprop = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : ଖାଲି, ଖାଲି);
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'b':
          want_state_scrollback = sense;
          ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", ଧାଡ଼ି[i]);
        }
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "WANTSCREEN") && (ଧାଡ଼ି[10] == '\0' || ଧାଡ଼ି[10] == ' ')) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 10;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା sense = 1;
      ପ୍ରତିଜ୍ଞା(vt);
      ଯଦି(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, ଖାଲି);
      vterm_screen_callbacks_has_pushline4(screen);

      ଯେତେବେଳେ(ଧାଡ଼ି[i] == ' ')
        i++;
      ପାଇଁ( ; ଧାଡ଼ି[i]; i++)
        ବାଛ(ଧାଡ଼ି[i]) {
        ସ୍ଥିତି '-':
          sense = 0;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'a':
          vterm_screen_enable_altscreen(screen, 1);
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'd':
          want_screen_damage = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'm':
          want_moverect = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'c':
          want_movecursor = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'p':
          want_settermprop = 1;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'b':
          want_screen_scrollback = sense;
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 'r':
          vterm_screen_enable_reflow(screen, sense);
          ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", ଧାଡ଼ି[i]);
        }
    }

    ଅନ୍ୟଥା ଯଦି(sscanf(ଧାଡ଼ି, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    ଅନ୍ୟଥା ଯଦି(streq(ଧାଡ଼ି, "RESET")) {
      ଯଦି(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ଯଦି(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "RESIZE ")) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, cols;
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 7;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "PUSH ")) {
      ଅକ୍ଷର *bytes = ଧାଡ଼ି + 5;
      size_t len = inplace_hex2bytes(bytes);
      ପ୍ରତିଜ୍ଞା(len);

      size_t written = vterm_ନିବେଶ_write(vt, bytes, len);
      ଯଦି(written < len)
        fprintf(stderr, "! short write\n");
    }

    ଅନ୍ୟଥା ଯଦି(streq(ଧାଡ଼ି, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ଯଦି(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "ENCIN ")) {
      ଅକ୍ଷର *bytes = ଧାଡ଼ି + 6;
      size_t len = inplace_hex2bytes(bytes);
      ପ୍ରତିଜ୍ଞା(len);

      uint32_t cp[1024];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      ଯଦି(cpi > 0) {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
        printf("encout ");
        ପାଇଁ(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "INCHAR ")) {
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 7;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = 0;
      VTermModifier mod;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "INKEY ")) {
      VTermModifier mod;
      VTerm_ଚାବି ଚାବି;
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 6;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      ଚାବି = strp_ଚାବି(linep);

      vterm_keyboard_ଚାବି(vt, ଚାବି, mod);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "PASTE ")) {
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 6;
      ଯଦି(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      ଅନ୍ୟଥା ଯଦି(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      ଅନ୍ୟଥା
        ଯାଅ abort_ଧାଡ଼ି;
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "FOCUS ")) {
      ପ୍ରତିଜ୍ଞା(state);
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 6;
      ଯଦି(streq(linep, "IN"))
        vterm_state_focus_in(state);
      ଅନ୍ୟଥା ଯଦି(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      ଅନ୍ୟଥା
        ଯାଅ abort_ଧାଡ଼ି;
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "MOUSEMOVE ")) {
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 10;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, col, len;
      VTermModifier mod;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "MOUSEBTN ")) {
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 9;
      ଅକ୍ଷର press;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, len;
      VTermModifier mod;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "SELECTION ")) {
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 10;
      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      ଯଦି(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      ପ୍ରତିଜ୍ଞା(frag.len);

      linep += frag.len * 2;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      ଯଦି(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "DAMAGEMERGE ")) {
      ପ୍ରତିଜ୍ଞା(screen);
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 12;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      ଯଦି(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      ଅନ୍ୟଥା ଯଦି(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      ଅନ୍ୟଥା ଯଦି(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      ଅନ୍ୟଥା ଯଦି(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "DAMAGEFLUSH")) {
      ପ୍ରତିଜ୍ଞା(screen);
      vterm_screen_flush_damage(screen);
    }

    ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "SETDEFAULTCOL ")) {
      ପ୍ରତିଜ୍ଞା(screen);
      ଅକ୍ଷର *linep = ଧାଡ଼ି + 14;
      ଯେତେବେଳେ(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ଯଦି(linep[0]) {
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      ଅନ୍ୟଥା
        vterm_screen_set_default_colors(screen, &fg, ଖାଲି);
    }

    ଅନ୍ୟଥା ଯଦି(ଧାଡ଼ି[0] == '?') {
      ଯଦି(streq(ଧାଡ଼ି, "?cursor")) {
        ପ୍ରତିଜ୍ଞା(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ଯଦି(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ଅନ୍ୟଥା ଯଦି(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ଅନ୍ୟଥା
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "?pen ")) {
        ପ୍ରତିଜ୍ଞା(state);
        VTerm_ମୂଲ୍ୟ val;
        ଅକ୍ଷର *linep = ଧାଡ଼ି + 5;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        ଯଦି(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ଯଦି(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          ଅନ୍ୟଥା
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ଯଦି(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          ଅନ୍ୟଥା
            printf("%d\n", state_pen.underline);
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ଯଦି(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          ଅନ୍ୟଥା
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ଯଦି(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          ଅନ୍ୟଥା
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          ଯଦି(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          ଅନ୍ୟଥା
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ଯଦି(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          ଅନ୍ୟଥା
            printf("%d\n", state_pen.font);
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ଯଦି(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          ଅନ୍ୟଥା
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ଯଦି(val.ସଂଖ୍ୟା != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.ସଂଖ୍ୟା, state_pen.baseline);
          ଅନ୍ୟଥା
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        ଅନ୍ୟଥା ଯଦି(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        ଅନ୍ୟଥା
          printf("?\n");
      }
      ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "?lineinfo ")) {
        ପ୍ରତିଜ୍ଞା(state);
        ଅକ୍ଷର *linep = ଧାଡ଼ି + 10;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;
        ଅଚଳ VTerm_ଧାଡ଼ି_Info *info;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
        ଯଦି(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          ଯାଅ abort_ଧାଡ଼ି;
        }
        info = vterm_state_get_lineinfo(state, row);
        ଯଦି(info->doublewidth)
          printf("dwl ");
        ଯଦି(info->doubleheight)
          printf("dhl ");
        ଯଦି(info->continuation)
          printf("cont ");
        printf("\n");
      }
      ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "?screen_chars ")) {
        ପ୍ରତିଜ୍ଞା(screen);
        ଅକ୍ଷର *linep = ଧାଡ଼ି + 13;
        VTermRect rect;
        size_t len;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
        ଯଦି(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        ଅନ୍ୟଥା ଯଦି(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, ଖାଲି, &rect.end_col);
        }
        ଅନ୍ୟଥା {
          printf("! screen_chars unrecognised input\n");
          ଯାଅ abort_ଧାଡ଼ି;
        }
        len = vterm_screen_get_chars(screen, ଖାଲି, 0, rect);
        ଯଦି(len == (size_t)-1)
          printf("! screen_chars error\n");
        ଅନ୍ୟଥା ଯଦି(len == 0)
          printf("\n");
        ଅନ୍ୟଥା {
          uint32_t *chars = malloc(ଆକାର(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          ପାଇଁ(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          ମୁକ୍ତ(chars);
        }
      }
      ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "?screen_text ")) {
        ପ୍ରତିଜ୍ଞା(screen);
        ଅକ୍ଷର *linep = ଧାଡ଼ି + 12;
        VTermRect rect;
        size_t len;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
        ଯଦି(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          ଯାଅ abort_ଧାଡ଼ି;
        }
        len = vterm_screen_get_text(screen, ଖାଲି, 0, rect);
        ଯଦି(len == (size_t)-1)
          printf("! screen_text error\n");
        ଅନ୍ୟଥା ଯଦି(len == 0)
          printf("\n");
        ଅନ୍ୟଥା {
          // Put an overwrite guard at both ends of the buffer
          unsigned ଅକ୍ଷର *buffer = malloc(len + 4);
          unsigned ଅକ୍ଷର *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (ଅକ୍ଷର *)text, len, rect);

          ଯଦି(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          ଅନ୍ୟଥା ଯଦି(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          ଅନ୍ୟଥା
	  {
	    size_t i;
            ପାଇଁ(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          ମୁକ୍ତ(buffer);
        }
      }
      ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "?screen_cell ")) {
        ପ୍ରତିଜ୍ଞା(screen);
        ଅକ୍ଷର *linep = ଧାଡ଼ି + 12;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
        VTermPos pos;
        VTermScreenCell cell;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
        ଯଦି(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          ଯାଅ abort_ଧାଡ଼ି;
        }
        ଯଦି(!vterm_screen_get_cell(screen, pos, &cell))
          ଯାଅ abort_ଧାଡ଼ି;
        printf("{");
        ପାଇଁ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        ଯଦି(cell.attrs.bold)      printf("B");
        ଯଦି(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        ଯଦି(cell.attrs.italic)    printf("I");
        ଯଦି(cell.attrs.blink)     printf("K");
        ଯଦି(cell.attrs.reverse)   printf("R");
        ଯଦି(cell.attrs.font)      printf("F%d", cell.attrs.font);
        ଯଦି(cell.attrs.small)     printf("S");
        ଯଦି(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        ଯଦି(cell.attrs.dwl)       printf("dwl ");
        ଯଦି(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "?screen_eol ")) {
        ପ୍ରତିଜ୍ଞା(screen);
        VTermPos pos;
        ଅକ୍ଷର *linep = ଧାଡ଼ି + 12;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
        ଯଦି(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          ଯାଅ abort_ଧାଡ଼ି;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      ଅନ୍ୟଥା ଯଦି(strstartswith(ଧାଡ଼ି, "?screen_attrs_extent ")) {
        ପ୍ରତିଜ୍ଞା(screen);
        VTermPos pos;
        VTermRect rect;
        ଅକ୍ଷର *linep = ଧାଡ଼ି + 21;
        ଯେତେବେଳେ(linep[0] == ' ')
          linep++;
        ଯଦି(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          ଯାଅ abort_ଧାଡ଼ି;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        ଯଦି(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          ଯାଅ abort_ଧାଡ଼ି;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      ଅନ୍ୟଥା
        printf("?\n");

      memset(ଧାଡ଼ି, 0, ଆକାର ଧାଡ଼ି);
      ଜାରି;
    }

    ଅନ୍ୟଥା
      abort_ଧାଡ଼ି: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    ଯଦି(outlen > 0) {
      ଅକ୍ଷର outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, ଖାଲି);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ଫେରନ୍ତୁ 0;
}
