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

  ໃນຂະນະທີ່(*inpos) {
    unsigned ຈຳນວນເຕັມ ch;
    ຖ້າ(sscanf(inpos, "%2x", &ch) < 1)
      bha_nga;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ສົ່ງຄືນ outpos - s;
}

static VTermModifier strpe_modifiers(var_na **strp)
{
  VTermModifier state = 0;

  ໃນຂະນະທີ່((*strp)[0]) {
    switch(((*strp)++)[0]) {
      pak_sa 'S': state |= VTERM_MOD_SHIFT; bha_nga;
      pak_sa 'C': state |= VTERM_MOD_CTRL;  bha_nga;
      pak_sa 'A': state |= VTERM_MOD_ALT;   bha_nga;
      default: ສົ່ງຄືນ state;
    }
  }

  ສົ່ງຄືນ state;
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
  ຈຳນວນເຕັມ i;

  ສຳລັບ(i = 0; keys[i].name; i++) {
    ຖ້າ(streq(str, keys[i].name))
      ສົ່ງຄືນ keys[i].key;
  }

  ສົ່ງຄືນ VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  ຖ້າ (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  ບໍ່ດັ່ງນັ້ນ {
    printf("invalid(%d", col->type);
  }
  ຖ້າ (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  ຖ້າ (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(var_na **strp)
{
  uint8_t r, g, b, idx;
  ຈຳນວນເຕັມ len = 0;
  VTermColor col;

  ຖ້າ(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  ບໍ່ດັ່ງນັ້ນ
    vterm_color_rgb(&col, 127, 127, 127);

  ສົ່ງຄືນ col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const var_na *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  ສຳລັບ(i = 0; i < len; i++)
    printf("%x%s", (unsigned var_na)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const var_na *s, size_t len)
{
  ໃນຂະນະທີ່(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static ຈຳນວນເຕັມ parser_text(const var_na bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  ສຳລັບ(i = 0; i < len; i++) {
    unsigned var_na b = bytes[i];
    ຖ້າ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      bha_nga;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ສົ່ງຄືນ i;
}

static ຈຳນວນເຕັມ parser_control(unsigned var_na control, void *user UNUSED)
{
  printf("control %02x\n", control);

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ parser_escape(const var_na bytes[], size_t len, void *user UNUSED)
{
  ຖ້າ(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    ຖ້າ(len < 2)
      ສົ່ງຄືນ -1;
    len = 2;
  }
  ບໍ່ດັ່ງນັ້ນ {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ສົ່ງຄືນ len;
}

static ຈຳນວນເຕັມ parser_csi(const var_na *leader, const long args[], ຈຳນວນເຕັມ argcount, const var_na *intermed, var_na command, void *user UNUSED)
{
  ຈຳນວນເຕັມ i;
  printf("csi %02x", command);

  ຖ້າ(leader && leader[0]) {
    printf(" L=");
    ສຳລັບ(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  ສຳລັບ(i = 0; i < argcount; i++) {
    var_na sep = i ? ',' : ' ';

    ຖ້າ(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    ບໍ່ດັ່ງນັ້ນ
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  ຖ້າ(intermed && intermed[0]) {
    printf(" I=");
    ສຳລັບ(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ parser_osc(ຈຳນວນເຕັມ command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  ຖ້າ(frag.initial) {
    ຖ້າ(command == -1)
      printf("[");
    ບໍ່ດັ່ງນັ້ນ
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ຖ້າ(frag.final)
    printf("]");

  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ parser_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  ຖ້າ(frag.initial) {
    size_t i;
    printf("[");
    ສຳລັບ(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ຖ້າ(frag.final)
    printf("]");

  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  ຖ້າ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ຖ້າ(frag.final)
    printf("]");

  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  ຖ້າ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ຖ້າ(frag.final)
    printf("]");

  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  ຖ້າ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ຖ້າ(frag.final)
    printf("]");

  printf("\n");

  ສົ່ງຄືນ 1;
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

static ຈຳນວນເຕັມ want_movecursor = 0;
static VTermPos state_pos;
static ຈຳນວນເຕັມ movecursor(VTermPos pos, VTermPos oldpos UNUSED, ຈຳນວນເຕັມ visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  ຖ້າ(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ want_premove = 0;
static ຈຳນວນເຕັມ premove(VTermRect rect, void *user UNUSED)
{
  ຖ້າ(!want_premove)
    ສົ່ງຄືນ 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ want_scrollrect = 0;
static ຈຳນວນເຕັມ scrollrect(VTermRect rect, ຈຳນວນເຕັມ downward, ຈຳນວນເຕັມ rightward, void *user UNUSED)
{
  ຖ້າ(!want_scrollrect)
    ສົ່ງຄືນ 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ want_moverect = 0;
static ຈຳນວນເຕັມ moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  ຖ້າ(!want_moverect)
    ສົ່ງຄືນ 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ want_settermprop = 0;
static ຈຳນວນເຕັມ settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  ຖ້າ(!want_settermprop)
    ສົ່ງຄືນ 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  pak_sa VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ສົ່ງຄືນ 1;
  pak_sa VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    ສົ່ງຄືນ 1;
  pak_sa VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ສົ່ງຄືນ 1;
  pak_sa VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ສົ່ງຄືນ 1;

  pak_sa VTERM_N_VALUETYPES:
    ສົ່ງຄືນ 0;
  }

  ສົ່ງຄືນ 0;
}

// These callbacks are for State

static ຈຳນວນເຕັມ want_state_putglyph = 0;
static ຈຳນວນເຕັມ state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  ຈຳນວນເຕັມ i;
  ຖ້າ(!want_state_putglyph)
    ສົ່ງຄືນ 1;

  printf("putglyph ");
  ສຳລັບ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  ຖ້າ(info->protected_cell)
    printf(" prot");
  ຖ້າ(info->dwl)
    printf(" dwl");
  ຖ້າ(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ want_state_erase = 0;
static ຈຳນວນເຕັມ state_erase(VTermRect rect, ຈຳນວນເຕັມ selective, void *user UNUSED)
{
  ຖ້າ(!want_state_erase)
    ສົ່ງຄືນ 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ສົ່ງຄືນ 1;
}

static sa_nracanaa {
  ຈຳນວນເຕັມ bold;
  ຈຳນວນເຕັມ underline;
  ຈຳນວນເຕັມ italic;
  ຈຳນວນເຕັມ blink;
  ຈຳນວນເຕັມ reverse;
  ຈຳນວນເຕັມ conceal;
  ຈຳນວນເຕັມ strike;
  ຈຳນວນເຕັມ font;
  ຈຳນວນເຕັມ small;
  ຈຳນວນເຕັມ baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static ຈຳນວນເຕັມ state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    ສົ່ງຄືນ 0;
  }

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ state_setlineinfo(ຈຳນວນເຕັມ row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ want_state_scrollback = 0;
static ຈຳນວນເຕັມ state_sb_clear(void *user UNUSED) {
  ຖ້າ(!want_state_scrollback)
    ສົ່ງຄືນ 1;

  printf("sb_clear\n");
  ສົ່ງຄືນ 0;
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

static ຈຳນວນເຕັມ selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  ຖ້າ(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  ຖ້າ(frag.final)
    printf("]");
  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ສົ່ງຄືນ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static ຈຳນວນເຕັມ want_screen_damage = 0;
static ຈຳນວນເຕັມ want_screen_damage_cells = 0;
static ຈຳນວນເຕັມ screen_damage(VTermRect rect, void *user UNUSED)
{
  ຖ້າ(!want_screen_damage)
    ສົ່ງຄືນ 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ຖ້າ(want_screen_damage_cells) {
    ຈຳນວນເຕັມ equals = FALSE;
    ຈຳນວນເຕັມ row;
    ຈຳນວນເຕັມ col;

    ສຳລັບ(row = rect.start_row; row < rect.end_row; row++) {
      ຈຳນວນເຕັມ eol = rect.end_col;
      ໃນຂະນະທີ່(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ຖ້າ(cell.chars[0])
          bha_nga;

        eol--;
      }

      ຖ້າ(eol == rect.start_col)
        bha_nga;

      ຖ້າ(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      ສຳລັບ(col = rect.start_col; col < eol; col++) {
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

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ want_screen_scrollback = 0;
static ຈຳນວນເຕັມ screen_sb_pushline4(ຈຳນວນເຕັມ cols, const VTermScreenCell *cells, ຈຳນວນເຕັມ continuation, void *user UNUSED)
{
  ຈຳນວນເຕັມ eol;
  ຈຳນວນເຕັມ c;

  ຖ້າ(!want_screen_scrollback)
    ສົ່ງຄືນ 1;

  eol = cols;
  ໃນຂະນະທີ່(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  ສຳລັບ(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ screen_sb_popline(ຈຳນວນເຕັມ cols, VTermScreenCell *cells, void *user UNUSED)
{
  ຈຳນວນເຕັມ col;

  ຖ້າ(!want_screen_scrollback)
    ສົ່ງຄືນ 0;

  // All lines of scrollback contain "ABCDE"
  ສຳລັບ(col = 0; col < cols; col++) {
    ຖ້າ(col < 5)
      cells[col].chars[0] = 'A' + col;
    ບໍ່ດັ່ງນັ້ນ
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ສົ່ງຄືນ 1;
}

static ຈຳນວນເຕັມ screen_sb_clear(void *user UNUSED)
{
  ຖ້າ(!want_screen_scrollback)
    ສົ່ງຄືນ 1;

  printf("sb_clear\n");
  ສົ່ງຄືນ 0;
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

ຈຳນວນເຕັມ main(ຈຳນວນເຕັມ argc UNUSED, var_na **argv UNUSED)
{
  var_na line[1024] = {0};
  ຈຳນວນເຕັມ flag;

  ຈຳນວນເຕັມ err;

  setvbuf(stdout, NULL, _IONBF, 0);

  ໃນຂະນະທີ່(fgets(line, parimaa_na line, stdin)) {
    var_na *nl;
    size_t outlen;
    err = 0;

    ຖ້າ((nl = strchr(line, '\n')))
      *nl = '\0';

    ຖ້າ(streq(line, "INIT")) {
      ຖ້າ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      ຈຳນວນເຕັມ i = 9;
      ຈຳນວນເຕັມ sense = 1;
      assert(vt);
      ຖ້າ(!state) {
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

      ໃນຂະນະທີ່(line[i] == ' ')
        i++;
      ສຳລັບ( ; line[i]; i++)
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

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      ຈຳນວນເຕັມ i = 10;
      ຈຳນວນເຕັມ sense = 1;
      assert(vt);
      ຖ້າ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      ໃນຂະນະທີ່(line[i] == ' ')
        i++;
      ສຳລັບ( ; line[i]; i++)
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

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(line, "RESET")) {
      ຖ້າ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ຖ້າ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "RESIZE ")) {
      ຈຳນວນເຕັມ rows, cols;
      var_na *linep = line + 7;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "PUSH ")) {
      var_na *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      ຖ້າ(written < len)
        fprintf(stderr, "! short write\n");
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ຖ້າ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "ENCIN ")) {
      var_na *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      ຈຳນວນເຕັມ cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      ຖ້າ(cpi > 0) {
	ຈຳນວນເຕັມ i;
        printf("encout ");
        ສຳລັບ(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "INCHAR ")) {
      var_na *linep = line + 7;
      unsigned ຈຳນວນເຕັມ c = 0;
      VTermModifier mod;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      var_na *linep = line + 6;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "PASTE ")) {
      var_na *linep = line + 6;
      ຖ້າ(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      ບໍ່ດັ່ງນັ້ນ
        goto abort_line;
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "FOCUS ")) {
      assert(state);
      var_na *linep = line + 6;
      ຖ້າ(streq(linep, "IN"))
        vterm_state_focus_in(state);
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      ບໍ່ດັ່ງນັ້ນ
        goto abort_line;
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "MOUSEMOVE ")) {
      var_na *linep = line + 10;
      ຈຳນວນເຕັມ row, col, len;
      VTermModifier mod;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "MOUSEBTN ")) {
      var_na *linep = line + 9;
      var_na press;
      ຈຳນວນເຕັມ button, len;
      VTermModifier mod;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "SELECTION ")) {
      var_na *linep = line + 10;
      unsigned ຈຳນວນເຕັມ mask;
      ຈຳນວນເຕັມ len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      ຖ້າ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      ຖ້າ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      var_na *linep = line + 12;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      ຖ້າ(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      var_na *linep = line + 14;
      ໃນຂະນະທີ່(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ຖ້າ(linep[0]) {
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      ບໍ່ດັ່ງນັ້ນ
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    ບໍ່ດັ່ງນັ້ນ ຖ້າ(line[0] == '?') {
      ຖ້າ(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ຖ້າ(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        ບໍ່ດັ່ງນັ້ນ
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        var_na *linep = line + 5;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        ຖ້າ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ຖ້າ(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          ບໍ່ດັ່ງນັ້ນ
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ຖ້າ(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          ບໍ່ດັ່ງນັ້ນ
            printf("%d\n", state_pen.underline);
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ຖ້າ(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          ບໍ່ດັ່ງນັ້ນ
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ຖ້າ(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          ບໍ່ດັ່ງນັ້ນ
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          ຖ້າ(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          ບໍ່ດັ່ງນັ້ນ
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ຖ້າ(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          ບໍ່ດັ່ງນັ້ນ
            printf("%d\n", state_pen.font);
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ຖ້າ(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          ບໍ່ດັ່ງນັ້ນ
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ຖ້າ(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          ບໍ່ດັ່ງນັ້ນ
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        ບໍ່ດັ່ງນັ້ນ
          printf("?\n");
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "?lineinfo ")) {
        assert(state);
        var_na *linep = line + 10;
        ຈຳນວນເຕັມ row;
        const VTermLineInfo *info;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
        ຖ້າ(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        ຖ້າ(info->doublewidth)
          printf("dwl ");
        ຖ້າ(info->doubleheight)
          printf("dhl ");
        ຖ້າ(info->continuation)
          printf("cont ");
        printf("\n");
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        var_na *linep = line + 13;
        VTermRect rect;
        size_t len;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
        ຖ້າ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        ບໍ່ດັ່ງນັ້ນ {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        ຖ້າ(len == (size_t)-1)
          printf("! screen_chars error\n");
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(len == 0)
          printf("\n");
        ບໍ່ດັ່ງນັ້ນ {
          uint32_t *chars = malloc(parimaa_na(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          ສຳລັບ(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "?screen_text ")) {
        assert(screen);
        var_na *linep = line + 12;
        VTermRect rect;
        size_t len;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
        ຖ້າ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        ຖ້າ(len == (size_t)-1)
          printf("! screen_text error\n");
        ບໍ່ດັ່ງນັ້ນ ຖ້າ(len == 0)
          printf("\n");
        ບໍ່ດັ່ງນັ້ນ {
          // Put an overwrite guard at both ends of the buffer
          unsigned var_na *buffer = malloc(len + 4);
          unsigned var_na *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (var_na *)text, len, rect);

          ຖ້າ(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          ບໍ່ດັ່ງນັ້ນ ຖ້າ(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          ບໍ່ດັ່ງນັ້ນ
	  {
	    size_t i;
            ສຳລັບ(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        var_na *linep = line + 12;
	ຈຳນວນເຕັມ i;
        VTermPos pos;
        VTermScreenCell cell;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
        ຖ້າ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        ຖ້າ(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        ສຳລັບ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        ຖ້າ(cell.attrs.bold)      printf("B");
        ຖ້າ(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        ຖ້າ(cell.attrs.italic)    printf("I");
        ຖ້າ(cell.attrs.blink)     printf("K");
        ຖ້າ(cell.attrs.reverse)   printf("R");
        ຖ້າ(cell.attrs.font)      printf("F%d", cell.attrs.font);
        ຖ້າ(cell.attrs.small)     printf("S");
        ຖ້າ(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        ຖ້າ(cell.attrs.dwl)       printf("dwl ");
        ຖ້າ(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        var_na *linep = line + 12;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
        ຖ້າ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        var_na *linep = line + 21;
        ໃນຂະນະທີ່(linep[0] == ' ')
          linep++;
        ຖ້າ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        ຖ້າ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      ບໍ່ດັ່ງນັ້ນ
        printf("?\n");

      memset(line, 0, parimaa_na line);
      continue;
    }

    ບໍ່ດັ່ງນັ້ນ
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    ຖ້າ(outlen > 0) {
      var_na outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ສົ່ງຄືນ 0;
}
