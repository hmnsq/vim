#include "vterm.h"
#include "../src/vterm_internal.h" // We pull in some internal bits too

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

static size_t inplace_hex2bytes(var_na *s)
{
  var_na *inpos = s, *outpos = s;

  ᯀᯬᯝ(*inpos) {
    unsigned puur_naa_nka ch;
    ᯀᯮᯝ(sscanf(inpos, "%2x", &ch) < 1)
      bha_nga;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ᯀᯮᯞ outpos - s;
}

static VTermModifier strpe_modifiers(var_na **strp)
{
  VTermModifier state = 0;

  ᯀᯬᯝ((*strp)[0]) {
    switch(((*strp)++)[0]) {
      pak_sa 'S': state |= VTERM_MOD_SHIFT; bha_nga;
      pak_sa 'C': state |= VTERM_MOD_CTRL;  bha_nga;
      pak_sa 'A': state |= VTERM_MOD_ALT;   bha_nga;
      default: ᯀᯮᯞ state;
    }
  }

  ᯀᯮᯞ state;
}

static VTermKey strp_key(var_na *str)
{
  static sa_nracanaa {
    var_na *name;
    VTermKey key;
  } keys[] = {
    { "Up",    VTERM_KEY_UP },
    { "Tab",   VTERM_KEY_TAB },
    { "Enter", VTERM_KEY_ENTER },
    { "KP0",   VTERM_KEY_KP_0 },
    { "F1",    VTERM_KEY_FUNCTION(1) },
    { NULL,    VTERM_KEY_NONE },
  };
  puur_naa_nka i;

  for(i = 0; keys[i].name; i++) {
    ᯀᯮᯝ(streq(str, keys[i].name))
      ᯀᯮᯞ keys[i].key;
  }

  ᯀᯮᯞ VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  ᯀᯮᯝ (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  ᯀᯮᯝᯂᯪ {
    printf("invalid(%d", col->type);
  }
  ᯀᯮᯝ (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  ᯀᯮᯝ (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(var_na **strp)
{
  uint8_t r, g, b, idx;
  puur_naa_nka len = 0;
  VTermColor col;

  ᯀᯮᯝ(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  ᯀᯮᯝᯂᯪ
    vterm_color_rgb(&col, 127, 127, 127);

  ᯀᯮᯞ col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const var_na *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  for(i = 0; i < len; i++)
    printf("%x%s", (unsigned var_na)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const var_na *s, size_t len)
{
  ᯀᯬᯝ(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static puur_naa_nka parser_text(const var_na bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  for(i = 0; i < len; i++) {
    unsigned var_na b = bytes[i];
    ᯀᯮᯝ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      bha_nga;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ᯀᯮᯞ i;
}

static puur_naa_nka parser_control(unsigned var_na control, void *user UNUSED)
{
  printf("control %02x\n", control);

  ᯀᯮᯞ 1;
}

static puur_naa_nka parser_escape(const var_na bytes[], size_t len, void *user UNUSED)
{
  ᯀᯮᯝ(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    ᯀᯮᯝ(len < 2)
      ᯀᯮᯞ -1;
    len = 2;
  }
  ᯀᯮᯝᯂᯪ {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ᯀᯮᯞ len;
}

static puur_naa_nka parser_csi(const var_na *leader, const long args[], puur_naa_nka argcount, const var_na *intermed, var_na command, void *user UNUSED)
{
  puur_naa_nka i;
  printf("csi %02x", command);

  ᯀᯮᯝ(leader && leader[0]) {
    printf(" L=");
    for(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  for(i = 0; i < argcount; i++) {
    var_na sep = i ? ',' : ' ';

    ᯀᯮᯝ(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    ᯀᯮᯝᯂᯪ
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  ᯀᯮᯝ(intermed && intermed[0]) {
    printf(" I=");
    for(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka parser_osc(puur_naa_nka command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  ᯀᯮᯝ(frag.initial) {
    ᯀᯮᯝ(command == -1)
      printf("[");
    ᯀᯮᯝᯂᯪ
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ᯀᯮᯝ(frag.final)
    printf("]");

  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka parser_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  ᯀᯮᯝ(frag.initial) {
    size_t i;
    printf("[");
    for(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ᯀᯮᯝ(frag.final)
    printf("]");

  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  ᯀᯮᯝ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ᯀᯮᯝ(frag.final)
    printf("]");

  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  ᯀᯮᯝ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ᯀᯮᯝ(frag.final)
    printf("]");

  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  ᯀᯮᯝ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ᯀᯮᯝ(frag.final)
    printf("]");

  printf("\n");

  ᯀᯮᯞ 1;
}

static VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  NULL // resize
};

static VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

static puur_naa_nka want_movecursor = 0;
static VTermPos state_pos;
static puur_naa_nka movecursor(VTermPos pos, VTermPos oldpos UNUSED, puur_naa_nka visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  ᯀᯮᯝ(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ᯀᯮᯞ 1;
}

static puur_naa_nka want_premove = 0;
static puur_naa_nka premove(VTermRect rect, void *user UNUSED)
{
  ᯀᯮᯝ(!want_premove)
    ᯀᯮᯞ 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ᯀᯮᯞ 1;
}

static puur_naa_nka want_scrollrect = 0;
static puur_naa_nka scrollrect(VTermRect rect, puur_naa_nka downward, puur_naa_nka rightward, void *user UNUSED)
{
  ᯀᯮᯝ(!want_scrollrect)
    ᯀᯮᯞ 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ᯀᯮᯞ 1;
}

static puur_naa_nka want_moverect = 0;
static puur_naa_nka moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  ᯀᯮᯝ(!want_moverect)
    ᯀᯮᯞ 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ᯀᯮᯞ 1;
}

static puur_naa_nka want_settermprop = 0;
static puur_naa_nka settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  ᯀᯮᯝ(!want_settermprop)
    ᯀᯮᯞ 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  pak_sa VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ᯀᯮᯞ 1;
  pak_sa VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    ᯀᯮᯞ 1;
  pak_sa VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ᯀᯮᯞ 1;
  pak_sa VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ᯀᯮᯞ 1;

  pak_sa VTERM_N_VALUETYPES:
    ᯀᯮᯞ 0;
  }

  ᯀᯮᯞ 0;
}

// These callbacks are for State

static puur_naa_nka want_state_putglyph = 0;
static puur_naa_nka state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  puur_naa_nka i;
  ᯀᯮᯝ(!want_state_putglyph)
    ᯀᯮᯞ 1;

  printf("putglyph ");
  for(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  ᯀᯮᯝ(info->protected_cell)
    printf(" prot");
  ᯀᯮᯝ(info->dwl)
    printf(" dwl");
  ᯀᯮᯝ(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka want_state_erase = 0;
static puur_naa_nka state_erase(VTermRect rect, puur_naa_nka selective, void *user UNUSED)
{
  ᯀᯮᯝ(!want_state_erase)
    ᯀᯮᯞ 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ᯀᯮᯞ 1;
}

static sa_nracanaa {
  puur_naa_nka bold;
  puur_naa_nka underline;
  puur_naa_nka italic;
  puur_naa_nka blink;
  puur_naa_nka reverse;
  puur_naa_nka conceal;
  puur_naa_nka strike;
  puur_naa_nka font;
  puur_naa_nka small;
  puur_naa_nka baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static puur_naa_nka state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
{
  switch(attr) {
  pak_sa VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    bha_nga;
  pak_sa VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->number;
    bha_nga;
  pak_sa VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    bha_nga;
  pak_sa VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    bha_nga;
  pak_sa VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    bha_nga;
  pak_sa VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    bha_nga;
  pak_sa VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    bha_nga;
  pak_sa VTERM_ATTR_FONT:
    state_pen.font = val->number;
    bha_nga;
  pak_sa VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    bha_nga;
  pak_sa VTERM_ATTR_BASELINE:
    state_pen.baseline = val->number;
    bha_nga;
  pak_sa VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    bha_nga;
  pak_sa VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    bha_nga;

  pak_sa VTERM_N_ATTRS:
    ᯀᯮᯞ 0;
  }

  ᯀᯮᯞ 1;
}

static puur_naa_nka state_setlineinfo(puur_naa_nka row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  ᯀᯮᯞ 1;
}

static puur_naa_nka want_state_scrollback = 0;
static puur_naa_nka state_sb_clear(void *user UNUSED) {
  ᯀᯮᯝ(!want_state_scrollback)
    ᯀᯮᯞ 1;

  printf("sb_clear\n");
  ᯀᯮᯞ 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  NULL, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  NULL, // bell
  NULL, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

static puur_naa_nka selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  ᯀᯮᯝ(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  ᯀᯮᯝ(frag.final)
    printf("]");
  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ᯀᯮᯞ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static puur_naa_nka want_screen_damage = 0;
static puur_naa_nka want_screen_damage_cells = 0;
static puur_naa_nka screen_damage(VTermRect rect, void *user UNUSED)
{
  ᯀᯮᯝ(!want_screen_damage)
    ᯀᯮᯞ 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ᯀᯮᯝ(want_screen_damage_cells) {
    puur_naa_nka equals = FALSE;
    puur_naa_nka row;
    puur_naa_nka col;

    for(row = rect.start_row; row < rect.end_row; row++) {
      puur_naa_nka eol = rect.end_col;
      ᯀᯬᯝ(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ᯀᯮᯝ(cell.chars[0])
          bha_nga;

        eol--;
      }

      ᯀᯮᯝ(eol == rect.start_col)
        bha_nga;

      ᯀᯮᯝ(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      for(col = rect.start_col; col < eol; col++) {
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

  ᯀᯮᯞ 1;
}

static puur_naa_nka want_screen_scrollback = 0;
static puur_naa_nka screen_sb_pushline4(puur_naa_nka cols, const VTermScreenCell *cells, puur_naa_nka continuation, void *user UNUSED)
{
  puur_naa_nka eol;
  puur_naa_nka c;

  ᯀᯮᯝ(!want_screen_scrollback)
    ᯀᯮᯞ 1;

  eol = cols;
  ᯀᯬᯝ(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  for(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ᯀᯮᯞ 1;
}

static puur_naa_nka screen_sb_popline(puur_naa_nka cols, VTermScreenCell *cells, void *user UNUSED)
{
  puur_naa_nka col;

  ᯀᯮᯝ(!want_screen_scrollback)
    ᯀᯮᯞ 0;

  // All lines of scrollback contain "ABCDE"
  for(col = 0; col < cols; col++) {
    ᯀᯮᯝ(col < 5)
      cells[col].chars[0] = 'A' + col;
    ᯀᯮᯝᯂᯪ
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ᯀᯮᯞ 1;
}

static puur_naa_nka screen_sb_clear(void *user UNUSED)
{
  ᯀᯮᯝ(!want_screen_scrollback)
    ᯀᯮᯞ 1;

  printf("sb_clear\n");
  ᯀᯮᯞ 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  NULL, // bell
  NULL, // resize
  NULL, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

puur_naa_nka main(puur_naa_nka argc UNUSED, var_na **argv UNUSED)
{
  var_na line[1024] = {0};
  puur_naa_nka flag;

  puur_naa_nka err;

  setvbuf(stdout, NULL, _IONBF, 0);

  ᯀᯬᯝ(fgets(line, parimaa_na line, stdin)) {
    var_na *nl;
    size_t outlen;
    err = 0;

    ᯀᯮᯝ((nl = strchr(line, '\n')))
      *nl = '\0';

    ᯀᯮᯝ(streq(line, "INIT")) {
      ᯀᯮᯝ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      puur_naa_nka i = 9;
      puur_naa_nka sense = 1;
      assert(vt);
      ᯀᯮᯝ(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, NULL);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, NULL, NULL, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      ᯀᯬᯝ(line[i] == ' ')
        i++;
      for( ; line[i]; i++)
        switch(line[i]) {
        pak_sa '+':
          sense = 1;
          bha_nga;
        pak_sa '-':
          sense = 0;
          bha_nga;
        pak_sa 'g':
          want_state_putglyph = sense;
          bha_nga;
        pak_sa 's':
          want_scrollrect = sense;
          bha_nga;
        pak_sa 'P':
          want_premove = sense;
          bha_nga;
        pak_sa 'm':
          want_moverect = sense;
          bha_nga;
        pak_sa 'e':
          want_state_erase = sense;
          bha_nga;
        pak_sa 'p':
          want_settermprop = sense;
          bha_nga;
        pak_sa 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : NULL, NULL);
          bha_nga;
        pak_sa 'b':
          want_state_scrollback = sense;
          bha_nga;
        default:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", line[i]);
        }
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      puur_naa_nka i = 10;
      puur_naa_nka sense = 1;
      assert(vt);
      ᯀᯮᯝ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      ᯀᯬᯝ(line[i] == ' ')
        i++;
      for( ; line[i]; i++)
        switch(line[i]) {
        pak_sa '-':
          sense = 0;
          bha_nga;
        pak_sa 'a':
          vterm_screen_enable_altscreen(screen, 1);
          bha_nga;
        pak_sa 'd':
          want_screen_damage = sense;
          bha_nga;
        pak_sa 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          bha_nga;
        pak_sa 'm':
          want_moverect = sense;
          bha_nga;
        pak_sa 'c':
          want_movecursor = sense;
          bha_nga;
        pak_sa 'p':
          want_settermprop = 1;
          bha_nga;
        pak_sa 'b':
          want_screen_scrollback = sense;
          bha_nga;
        pak_sa 'r':
          vterm_screen_enable_reflow(screen, sense);
          bha_nga;
        default:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", line[i]);
        }
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(line, "RESET")) {
      ᯀᯮᯝ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ᯀᯮᯝ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "RESIZE ")) {
      puur_naa_nka rows, cols;
      var_na *linep = line + 7;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "PUSH ")) {
      var_na *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      ᯀᯮᯝ(written < len)
        fprintf(stderr, "! short write\n");
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ᯀᯮᯝ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "ENCIN ")) {
      var_na *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      puur_naa_nka cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      ᯀᯮᯝ(cpi > 0) {
	puur_naa_nka i;
        printf("encout ");
        for(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "INCHAR ")) {
      var_na *linep = line + 7;
      unsigned puur_naa_nka c = 0;
      VTermModifier mod;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      var_na *linep = line + 6;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "PASTE ")) {
      var_na *linep = line + 6;
      ᯀᯮᯝ(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      ᯀᯮᯝᯂᯪ
        goto abort_line;
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "FOCUS ")) {
      assert(state);
      var_na *linep = line + 6;
      ᯀᯮᯝ(streq(linep, "IN"))
        vterm_state_focus_in(state);
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      ᯀᯮᯝᯂᯪ
        goto abort_line;
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "MOUSEMOVE ")) {
      var_na *linep = line + 10;
      puur_naa_nka row, col, len;
      VTermModifier mod;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "MOUSEBTN ")) {
      var_na *linep = line + 9;
      var_na press;
      puur_naa_nka button, len;
      VTermModifier mod;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "SELECTION ")) {
      var_na *linep = line + 10;
      unsigned puur_naa_nka mask;
      puur_naa_nka len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      ᯀᯮᯝ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      ᯀᯮᯝ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      var_na *linep = line + 12;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      ᯀᯮᯝ(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      var_na *linep = line + 14;
      ᯀᯬᯝ(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ᯀᯮᯝ(linep[0]) {
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      ᯀᯮᯝᯂᯪ
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(line[0] == '?') {
      ᯀᯮᯝ(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ᯀᯮᯝ(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ᯀᯮᯝᯂᯪ
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        var_na *linep = line + 5;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        ᯀᯮᯝ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ᯀᯮᯝ(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          ᯀᯮᯝᯂᯪ
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ᯀᯮᯝ(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          ᯀᯮᯝᯂᯪ
            printf("%d\n", state_pen.underline);
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ᯀᯮᯝ(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          ᯀᯮᯝᯂᯪ
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ᯀᯮᯝ(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          ᯀᯮᯝᯂᯪ
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          ᯀᯮᯝ(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          ᯀᯮᯝᯂᯪ
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ᯀᯮᯝ(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          ᯀᯮᯝᯂᯪ
            printf("%d\n", state_pen.font);
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ᯀᯮᯝ(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          ᯀᯮᯝᯂᯪ
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ᯀᯮᯝ(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          ᯀᯮᯝᯂᯪ
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        ᯀᯮᯝᯂᯪ
          printf("?\n");
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "?lineinfo ")) {
        assert(state);
        var_na *linep = line + 10;
        puur_naa_nka row;
        const VTermLineInfo *info;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
        ᯀᯮᯝ(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        ᯀᯮᯝ(info->doublewidth)
          printf("dwl ");
        ᯀᯮᯝ(info->doubleheight)
          printf("dhl ");
        ᯀᯮᯝ(info->continuation)
          printf("cont ");
        printf("\n");
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        var_na *linep = line + 13;
        VTermRect rect;
        size_t len;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
        ᯀᯮᯝ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        ᯀᯮᯝᯂᯪ {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        ᯀᯮᯝ(len == (size_t)-1)
          printf("! screen_chars error\n");
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(len == 0)
          printf("\n");
        ᯀᯮᯝᯂᯪ {
          uint32_t *chars = malloc(parimaa_na(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          for(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "?screen_text ")) {
        assert(screen);
        var_na *linep = line + 12;
        VTermRect rect;
        size_t len;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
        ᯀᯮᯝ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        ᯀᯮᯝ(len == (size_t)-1)
          printf("! screen_text error\n");
        ᯀᯮᯝᯂᯪ ᯀᯮᯝ(len == 0)
          printf("\n");
        ᯀᯮᯝᯂᯪ {
          // Put an overwrite guard at both ends of the buffer
          unsigned var_na *buffer = malloc(len + 4);
          unsigned var_na *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (var_na *)text, len, rect);

          ᯀᯮᯝ(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          ᯀᯮᯝᯂᯪ ᯀᯮᯝ(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          ᯀᯮᯝᯂᯪ
	  {
	    size_t i;
            for(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        var_na *linep = line + 12;
	puur_naa_nka i;
        VTermPos pos;
        VTermScreenCell cell;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
        ᯀᯮᯝ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        ᯀᯮᯝ(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        for(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        ᯀᯮᯝ(cell.attrs.bold)      printf("B");
        ᯀᯮᯝ(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        ᯀᯮᯝ(cell.attrs.italic)    printf("I");
        ᯀᯮᯝ(cell.attrs.blink)     printf("K");
        ᯀᯮᯝ(cell.attrs.reverse)   printf("R");
        ᯀᯮᯝ(cell.attrs.font)      printf("F%d", cell.attrs.font);
        ᯀᯮᯝ(cell.attrs.small)     printf("S");
        ᯀᯮᯝ(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        ᯀᯮᯝ(cell.attrs.dwl)       printf("dwl ");
        ᯀᯮᯝ(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        var_na *linep = line + 12;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
        ᯀᯮᯝ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      ᯀᯮᯝᯂᯪ ᯀᯮᯝ(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        var_na *linep = line + 21;
        ᯀᯬᯝ(linep[0] == ' ')
          linep++;
        ᯀᯮᯝ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        ᯀᯮᯝ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      ᯀᯮᯝᯂᯪ
        printf("?\n");

      memset(line, 0, parimaa_na line);
      continue;
    }

    ᯀᯮᯝᯂᯪ
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    ᯀᯮᯝ(outlen > 0) {
      var_na outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ᯀᯮᯞ 0;
}
