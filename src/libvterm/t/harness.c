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

  ខណៈពេល(*inpos) {
    unsigned ចំនួនគត់ ch;
    ប្រសិនបើ(sscanf(inpos, "%2x", &ch) < 1)
      bha_nga;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ត្រឡប់ outpos - s;
}

static VTermModifier strpe_modifiers(var_na **strp)
{
  VTermModifier state = 0;

  ខណៈពេល((*strp)[0]) {
    switch(((*strp)++)[0]) {
      pak_sa 'S': state |= VTERM_MOD_SHIFT; bha_nga;
      pak_sa 'C': state |= VTERM_MOD_CTRL;  bha_nga;
      pak_sa 'A': state |= VTERM_MOD_ALT;   bha_nga;
      default: ត្រឡប់ state;
    }
  }

  ត្រឡប់ state;
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
  ចំនួនគត់ i;

  សម្រាប់(i = 0; keys[i].name; i++) {
    ប្រសិនបើ(streq(str, keys[i].name))
      ត្រឡប់ keys[i].key;
  }

  ត្រឡប់ VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  ប្រសិនបើ (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  បើមិនដូច្នេះ ប្រសិនបើ (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  បើមិនដូច្នេះ {
    printf("invalid(%d", col->type);
  }
  ប្រសិនបើ (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  ប្រសិនបើ (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(var_na **strp)
{
  uint8_t r, g, b, idx;
  ចំនួនគត់ len = 0;
  VTermColor col;

  ប្រសិនបើ(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  បើមិនដូច្នេះ ប្រសិនបើ(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  បើមិនដូច្នេះ
    vterm_color_rgb(&col, 127, 127, 127);

  ត្រឡប់ col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const var_na *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  សម្រាប់(i = 0; i < len; i++)
    printf("%x%s", (unsigned var_na)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const var_na *s, size_t len)
{
  ខណៈពេល(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static ចំនួនគត់ parser_text(const var_na bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  សម្រាប់(i = 0; i < len; i++) {
    unsigned var_na b = bytes[i];
    ប្រសិនបើ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      bha_nga;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ត្រឡប់ i;
}

static ចំនួនគត់ parser_control(unsigned var_na control, void *user UNUSED)
{
  printf("control %02x\n", control);

  ត្រឡប់ 1;
}

static ចំនួនគត់ parser_escape(const var_na bytes[], size_t len, void *user UNUSED)
{
  ប្រសិនបើ(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    ប្រសិនបើ(len < 2)
      ត្រឡប់ -1;
    len = 2;
  }
  បើមិនដូច្នេះ {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ត្រឡប់ len;
}

static ចំនួនគត់ parser_csi(const var_na *leader, const long args[], ចំនួនគត់ argcount, const var_na *intermed, var_na command, void *user UNUSED)
{
  ចំនួនគត់ i;
  printf("csi %02x", command);

  ប្រសិនបើ(leader && leader[0]) {
    printf(" L=");
    សម្រាប់(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  សម្រាប់(i = 0; i < argcount; i++) {
    var_na sep = i ? ',' : ' ';

    ប្រសិនបើ(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    បើមិនដូច្នេះ
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  ប្រសិនបើ(intermed && intermed[0]) {
    printf(" I=");
    សម្រាប់(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ parser_osc(ចំនួនគត់ command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  ប្រសិនបើ(frag.initial) {
    ប្រសិនបើ(command == -1)
      printf("[");
    បើមិនដូច្នេះ
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ប្រសិនបើ(frag.final)
    printf("]");

  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ parser_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  ប្រសិនបើ(frag.initial) {
    size_t i;
    printf("[");
    សម្រាប់(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ប្រសិនបើ(frag.final)
    printf("]");

  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  ប្រសិនបើ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ប្រសិនបើ(frag.final)
    printf("]");

  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  ប្រសិនបើ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ប្រសិនបើ(frag.final)
    printf("]");

  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  ប្រសិនបើ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ប្រសិនបើ(frag.final)
    printf("]");

  printf("\n");

  ត្រឡប់ 1;
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

static ចំនួនគត់ want_movecursor = 0;
static VTermPos state_pos;
static ចំនួនគត់ movecursor(VTermPos pos, VTermPos oldpos UNUSED, ចំនួនគត់ visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  ប្រសិនបើ(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ត្រឡប់ 1;
}

static ចំនួនគត់ want_premove = 0;
static ចំនួនគត់ premove(VTermRect rect, void *user UNUSED)
{
  ប្រសិនបើ(!want_premove)
    ត្រឡប់ 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ត្រឡប់ 1;
}

static ចំនួនគត់ want_scrollrect = 0;
static ចំនួនគត់ scrollrect(VTermRect rect, ចំនួនគត់ downward, ចំនួនគត់ rightward, void *user UNUSED)
{
  ប្រសិនបើ(!want_scrollrect)
    ត្រឡប់ 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ត្រឡប់ 1;
}

static ចំនួនគត់ want_moverect = 0;
static ចំនួនគត់ moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  ប្រសិនបើ(!want_moverect)
    ត្រឡប់ 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ត្រឡប់ 1;
}

static ចំនួនគត់ want_settermprop = 0;
static ចំនួនគត់ settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  ប្រសិនបើ(!want_settermprop)
    ត្រឡប់ 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  pak_sa VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ត្រឡប់ 1;
  pak_sa VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    ត្រឡប់ 1;
  pak_sa VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ត្រឡប់ 1;
  pak_sa VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ត្រឡប់ 1;

  pak_sa VTERM_N_VALUETYPES:
    ត្រឡប់ 0;
  }

  ត្រឡប់ 0;
}

// These callbacks are for State

static ចំនួនគត់ want_state_putglyph = 0;
static ចំនួនគត់ state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  ចំនួនគត់ i;
  ប្រសិនបើ(!want_state_putglyph)
    ត្រឡប់ 1;

  printf("putglyph ");
  សម្រាប់(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  ប្រសិនបើ(info->protected_cell)
    printf(" prot");
  ប្រសិនបើ(info->dwl)
    printf(" dwl");
  ប្រសិនបើ(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ want_state_erase = 0;
static ចំនួនគត់ state_erase(VTermRect rect, ចំនួនគត់ selective, void *user UNUSED)
{
  ប្រសិនបើ(!want_state_erase)
    ត្រឡប់ 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ត្រឡប់ 1;
}

static sa_nracanaa {
  ចំនួនគត់ bold;
  ចំនួនគត់ underline;
  ចំនួនគត់ italic;
  ចំនួនគត់ blink;
  ចំនួនគត់ reverse;
  ចំនួនគត់ conceal;
  ចំនួនគត់ strike;
  ចំនួនគត់ font;
  ចំនួនគត់ small;
  ចំនួនគត់ baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static ចំនួនគត់ state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    ត្រឡប់ 0;
  }

  ត្រឡប់ 1;
}

static ចំនួនគត់ state_setlineinfo(ចំនួនគត់ row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  ត្រឡប់ 1;
}

static ចំនួនគត់ want_state_scrollback = 0;
static ចំនួនគត់ state_sb_clear(void *user UNUSED) {
  ប្រសិនបើ(!want_state_scrollback)
    ត្រឡប់ 1;

  printf("sb_clear\n");
  ត្រឡប់ 0;
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

static ចំនួនគត់ selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  ប្រសិនបើ(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  ប្រសិនបើ(frag.final)
    printf("]");
  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ត្រឡប់ 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static ចំនួនគត់ want_screen_damage = 0;
static ចំនួនគត់ want_screen_damage_cells = 0;
static ចំនួនគត់ screen_damage(VTermRect rect, void *user UNUSED)
{
  ប្រសិនបើ(!want_screen_damage)
    ត្រឡប់ 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ប្រសិនបើ(want_screen_damage_cells) {
    ចំនួនគត់ equals = FALSE;
    ចំនួនគត់ row;
    ចំនួនគត់ col;

    សម្រាប់(row = rect.start_row; row < rect.end_row; row++) {
      ចំនួនគត់ eol = rect.end_col;
      ខណៈពេល(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ប្រសិនបើ(cell.chars[0])
          bha_nga;

        eol--;
      }

      ប្រសិនបើ(eol == rect.start_col)
        bha_nga;

      ប្រសិនបើ(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      សម្រាប់(col = rect.start_col; col < eol; col++) {
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

  ត្រឡប់ 1;
}

static ចំនួនគត់ want_screen_scrollback = 0;
static ចំនួនគត់ screen_sb_pushline4(ចំនួនគត់ cols, const VTermScreenCell *cells, ចំនួនគត់ continuation, void *user UNUSED)
{
  ចំនួនគត់ eol;
  ចំនួនគត់ c;

  ប្រសិនបើ(!want_screen_scrollback)
    ត្រឡប់ 1;

  eol = cols;
  ខណៈពេល(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  សម្រាប់(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ត្រឡប់ 1;
}

static ចំនួនគត់ screen_sb_popline(ចំនួនគត់ cols, VTermScreenCell *cells, void *user UNUSED)
{
  ចំនួនគត់ col;

  ប្រសិនបើ(!want_screen_scrollback)
    ត្រឡប់ 0;

  // All lines of scrollback contain "ABCDE"
  សម្រាប់(col = 0; col < cols; col++) {
    ប្រសិនបើ(col < 5)
      cells[col].chars[0] = 'A' + col;
    បើមិនដូច្នេះ
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ត្រឡប់ 1;
}

static ចំនួនគត់ screen_sb_clear(void *user UNUSED)
{
  ប្រសិនបើ(!want_screen_scrollback)
    ត្រឡប់ 1;

  printf("sb_clear\n");
  ត្រឡប់ 0;
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

ចំនួនគត់ main(ចំនួនគត់ argc UNUSED, var_na **argv UNUSED)
{
  var_na line[1024] = {0};
  ចំនួនគត់ flag;

  ចំនួនគត់ err;

  setvbuf(stdout, NULL, _IONBF, 0);

  ខណៈពេល(fgets(line, parimaa_na line, stdin)) {
    var_na *nl;
    size_t outlen;
    err = 0;

    ប្រសិនបើ((nl = strchr(line, '\n')))
      *nl = '\0';

    ប្រសិនបើ(streq(line, "INIT")) {
      ប្រសិនបើ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      ចំនួនគត់ i = 9;
      ចំនួនគត់ sense = 1;
      assert(vt);
      ប្រសិនបើ(!state) {
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

      ខណៈពេល(line[i] == ' ')
        i++;
      សម្រាប់( ; line[i]; i++)
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

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      ចំនួនគត់ i = 10;
      ចំនួនគត់ sense = 1;
      assert(vt);
      ប្រសិនបើ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      ខណៈពេល(line[i] == ' ')
        i++;
      សម្រាប់( ; line[i]; i++)
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

    បើមិនដូច្នេះ ប្រសិនបើ(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(streq(line, "RESET")) {
      ប្រសិនបើ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ប្រសិនបើ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "RESIZE ")) {
      ចំនួនគត់ rows, cols;
      var_na *linep = line + 7;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "PUSH ")) {
      var_na *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      ប្រសិនបើ(written < len)
        fprintf(stderr, "! short write\n");
    }

    បើមិនដូច្នេះ ប្រសិនបើ(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ប្រសិនបើ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "ENCIN ")) {
      var_na *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      ចំនួនគត់ cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      ប្រសិនបើ(cpi > 0) {
	ចំនួនគត់ i;
        printf("encout ");
        សម្រាប់(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "INCHAR ")) {
      var_na *linep = line + 7;
      unsigned ចំនួនគត់ c = 0;
      VTermModifier mod;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      var_na *linep = line + 6;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ខណៈពេល(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "PASTE ")) {
      var_na *linep = line + 6;
      ប្រសិនបើ(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      បើមិនដូច្នេះ
        goto abort_line;
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "FOCUS ")) {
      assert(state);
      var_na *linep = line + 6;
      ប្រសិនបើ(streq(linep, "IN"))
        vterm_state_focus_in(state);
      បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      បើមិនដូច្នេះ
        goto abort_line;
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "MOUSEMOVE ")) {
      var_na *linep = line + 10;
      ចំនួនគត់ row, col, len;
      VTermModifier mod;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "MOUSEBTN ")) {
      var_na *linep = line + 9;
      var_na press;
      ចំនួនគត់ button, len;
      VTermModifier mod;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "SELECTION ")) {
      var_na *linep = line + 10;
      unsigned ចំនួនគត់ mask;
      ចំនួនគត់ len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      ប្រសិនបើ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ខណៈពេល(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      ប្រសិនបើ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      var_na *linep = line + 12;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      ប្រសិនបើ(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      var_na *linep = line + 14;
      ខណៈពេល(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ប្រសិនបើ(linep[0]) {
        ខណៈពេល(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      បើមិនដូច្នេះ
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    បើមិនដូច្នេះ ប្រសិនបើ(line[0] == '?') {
      ប្រសិនបើ(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ប្រសិនបើ(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        បើមិនដូច្នេះ ប្រសិនបើ(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        បើមិនដូច្នេះ
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        var_na *linep = line + 5;
        ខណៈពេល(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        ប្រសិនបើ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ប្រសិនបើ(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          បើមិនដូច្នេះ
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ប្រសិនបើ(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          បើមិនដូច្នេះ
            printf("%d\n", state_pen.underline);
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ប្រសិនបើ(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          បើមិនដូច្នេះ
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ប្រសិនបើ(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          បើមិនដូច្នេះ
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          ប្រសិនបើ(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          បើមិនដូច្នេះ
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ប្រសិនបើ(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          បើមិនដូច្នេះ
            printf("%d\n", state_pen.font);
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ប្រសិនបើ(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          បើមិនដូច្នេះ
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ប្រសិនបើ(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          បើមិនដូច្នេះ
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        បើមិនដូច្នេះ ប្រសិនបើ(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        បើមិនដូច្នេះ
          printf("?\n");
      }
      បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "?lineinfo ")) {
        assert(state);
        var_na *linep = line + 10;
        ចំនួនគត់ row;
        const VTermLineInfo *info;
        ខណៈពេល(linep[0] == ' ')
          linep++;
        ប្រសិនបើ(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        ប្រសិនបើ(info->doublewidth)
          printf("dwl ");
        ប្រសិនបើ(info->doubleheight)
          printf("dhl ");
        ប្រសិនបើ(info->continuation)
          printf("cont ");
        printf("\n");
      }
      បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        var_na *linep = line + 13;
        VTermRect rect;
        size_t len;
        ខណៈពេល(linep[0] == ' ')
          linep++;
        ប្រសិនបើ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        បើមិនដូច្នេះ ប្រសិនបើ(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        បើមិនដូច្នេះ {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        ប្រសិនបើ(len == (size_t)-1)
          printf("! screen_chars error\n");
        បើមិនដូច្នេះ ប្រសិនបើ(len == 0)
          printf("\n");
        បើមិនដូច្នេះ {
          uint32_t *chars = malloc(parimaa_na(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          សម្រាប់(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "?screen_text ")) {
        assert(screen);
        var_na *linep = line + 12;
        VTermRect rect;
        size_t len;
        ខណៈពេល(linep[0] == ' ')
          linep++;
        ប្រសិនបើ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        ប្រសិនបើ(len == (size_t)-1)
          printf("! screen_text error\n");
        បើមិនដូច្នេះ ប្រសិនបើ(len == 0)
          printf("\n");
        បើមិនដូច្នេះ {
          // Put an overwrite guard at both ends of the buffer
          unsigned var_na *buffer = malloc(len + 4);
          unsigned var_na *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (var_na *)text, len, rect);

          ប្រសិនបើ(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          បើមិនដូច្នេះ ប្រសិនបើ(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          បើមិនដូច្នេះ
	  {
	    size_t i;
            សម្រាប់(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        var_na *linep = line + 12;
	ចំនួនគត់ i;
        VTermPos pos;
        VTermScreenCell cell;
        ខណៈពេល(linep[0] == ' ')
          linep++;
        ប្រសិនបើ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        ប្រសិនបើ(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        សម្រាប់(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        ប្រសិនបើ(cell.attrs.bold)      printf("B");
        ប្រសិនបើ(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        ប្រសិនបើ(cell.attrs.italic)    printf("I");
        ប្រសិនបើ(cell.attrs.blink)     printf("K");
        ប្រសិនបើ(cell.attrs.reverse)   printf("R");
        ប្រសិនបើ(cell.attrs.font)      printf("F%d", cell.attrs.font);
        ប្រសិនបើ(cell.attrs.small)     printf("S");
        ប្រសិនបើ(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        ប្រសិនបើ(cell.attrs.dwl)       printf("dwl ");
        ប្រសិនបើ(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        var_na *linep = line + 12;
        ខណៈពេល(linep[0] == ' ')
          linep++;
        ប្រសិនបើ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      បើមិនដូច្នេះ ប្រសិនបើ(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        var_na *linep = line + 21;
        ខណៈពេល(linep[0] == ' ')
          linep++;
        ប្រសិនបើ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        ប្រសិនបើ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      បើមិនដូច្នេះ
        printf("?\n");

      memset(line, 0, parimaa_na line);
      continue;
    }

    បើមិនដូច្នេះ
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    ប្រសិនបើ(outlen > 0) {
      var_na outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ត្រឡប់ 0;
}
