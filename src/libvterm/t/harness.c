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

  སྐབས(*inpos) {
    unsigned ཧྲིལ ch;
    གལ་ཏེ(sscanf(inpos, "%2x", &ch) < 1)
      bha_nga;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ཕྱིར outpos - s;
}

static VTermModifier strpe_modifiers(var_na **strp)
{
  VTermModifier state = 0;

  སྐབས((*strp)[0]) {
    switch(((*strp)++)[0]) {
      pak_sa 'S': state |= VTERM_MOD_SHIFT; bha_nga;
      pak_sa 'C': state |= VTERM_MOD_CTRL;  bha_nga;
      pak_sa 'A': state |= VTERM_MOD_ALT;   bha_nga;
      default: ཕྱིར state;
    }
  }

  ཕྱིར state;
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
  ཧྲིལ i;

  ཆེད་དུ(i = 0; keys[i].name; i++) {
    གལ་ཏེ(streq(str, keys[i].name))
      ཕྱིར keys[i].key;
  }

  ཕྱིར VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  གལ་ཏེ (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  དེ་མིན གལ་ཏེ (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  དེ་མིན {
    printf("invalid(%d", col->type);
  }
  གལ་ཏེ (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  གལ་ཏེ (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(var_na **strp)
{
  uint8_t r, g, b, idx;
  ཧྲིལ len = 0;
  VTermColor col;

  གལ་ཏེ(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  དེ་མིན གལ་ཏེ(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  དེ་མིན
    vterm_color_rgb(&col, 127, 127, 127);

  ཕྱིར col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const var_na *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  ཆེད་དུ(i = 0; i < len; i++)
    printf("%x%s", (unsigned var_na)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const var_na *s, size_t len)
{
  སྐབས(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static ཧྲིལ parser_text(const var_na bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  ཆེད་དུ(i = 0; i < len; i++) {
    unsigned var_na b = bytes[i];
    གལ་ཏེ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      bha_nga;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ཕྱིར i;
}

static ཧྲིལ parser_control(unsigned var_na control, void *user UNUSED)
{
  printf("control %02x\n", control);

  ཕྱིར 1;
}

static ཧྲིལ parser_escape(const var_na bytes[], size_t len, void *user UNUSED)
{
  གལ་ཏེ(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    གལ་ཏེ(len < 2)
      ཕྱིར -1;
    len = 2;
  }
  དེ་མིན {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ཕྱིར len;
}

static ཧྲིལ parser_csi(const var_na *leader, const long args[], ཧྲིལ argcount, const var_na *intermed, var_na command, void *user UNUSED)
{
  ཧྲིལ i;
  printf("csi %02x", command);

  གལ་ཏེ(leader && leader[0]) {
    printf(" L=");
    ཆེད་དུ(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  ཆེད་དུ(i = 0; i < argcount; i++) {
    var_na sep = i ? ',' : ' ';

    གལ་ཏེ(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    དེ་མིན
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  གལ་ཏེ(intermed && intermed[0]) {
    printf(" I=");
    ཆེད་དུ(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ parser_osc(ཧྲིལ command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  གལ་ཏེ(frag.initial) {
    གལ་ཏེ(command == -1)
      printf("[");
    དེ་མིན
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  གལ་ཏེ(frag.final)
    printf("]");

  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ parser_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  གལ་ཏེ(frag.initial) {
    size_t i;
    printf("[");
    ཆེད་དུ(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  གལ་ཏེ(frag.final)
    printf("]");

  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  གལ་ཏེ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  གལ་ཏེ(frag.final)
    printf("]");

  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  གལ་ཏེ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  གལ་ཏེ(frag.final)
    printf("]");

  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  གལ་ཏེ(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  གལ་ཏེ(frag.final)
    printf("]");

  printf("\n");

  ཕྱིར 1;
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

static ཧྲིལ want_movecursor = 0;
static VTermPos state_pos;
static ཧྲིལ movecursor(VTermPos pos, VTermPos oldpos UNUSED, ཧྲིལ visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  གལ་ཏེ(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ཕྱིར 1;
}

static ཧྲིལ want_premove = 0;
static ཧྲིལ premove(VTermRect rect, void *user UNUSED)
{
  གལ་ཏེ(!want_premove)
    ཕྱིར 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ཕྱིར 1;
}

static ཧྲིལ want_scrollrect = 0;
static ཧྲིལ scrollrect(VTermRect rect, ཧྲིལ downward, ཧྲིལ rightward, void *user UNUSED)
{
  གལ་ཏེ(!want_scrollrect)
    ཕྱིར 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ཕྱིར 1;
}

static ཧྲིལ want_moverect = 0;
static ཧྲིལ moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  གལ་ཏེ(!want_moverect)
    ཕྱིར 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ཕྱིར 1;
}

static ཧྲིལ want_settermprop = 0;
static ཧྲིལ settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  གལ་ཏེ(!want_settermprop)
    ཕྱིར 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  pak_sa VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ཕྱིར 1;
  pak_sa VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    ཕྱིར 1;
  pak_sa VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ཕྱིར 1;
  pak_sa VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ཕྱིར 1;

  pak_sa VTERM_N_VALUETYPES:
    ཕྱིར 0;
  }

  ཕྱིར 0;
}

// These callbacks are for State

static ཧྲིལ want_state_putglyph = 0;
static ཧྲིལ state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  ཧྲིལ i;
  གལ་ཏེ(!want_state_putglyph)
    ཕྱིར 1;

  printf("putglyph ");
  ཆེད་དུ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  གལ་ཏེ(info->protected_cell)
    printf(" prot");
  གལ་ཏེ(info->dwl)
    printf(" dwl");
  གལ་ཏེ(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ want_state_erase = 0;
static ཧྲིལ state_erase(VTermRect rect, ཧྲིལ selective, void *user UNUSED)
{
  གལ་ཏེ(!want_state_erase)
    ཕྱིར 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ཕྱིར 1;
}

static sa_nracanaa {
  ཧྲིལ bold;
  ཧྲིལ underline;
  ཧྲིལ italic;
  ཧྲིལ blink;
  ཧྲིལ reverse;
  ཧྲིལ conceal;
  ཧྲིལ strike;
  ཧྲིལ font;
  ཧྲིལ small;
  ཧྲིལ baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static ཧྲིལ state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    ཕྱིར 0;
  }

  ཕྱིར 1;
}

static ཧྲིལ state_setlineinfo(ཧྲིལ row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  ཕྱིར 1;
}

static ཧྲིལ want_state_scrollback = 0;
static ཧྲིལ state_sb_clear(void *user UNUSED) {
  གལ་ཏེ(!want_state_scrollback)
    ཕྱིར 1;

  printf("sb_clear\n");
  ཕྱིར 0;
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

static ཧྲིལ selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  གལ་ཏེ(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  གལ་ཏེ(frag.final)
    printf("]");
  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ཕྱིར 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static ཧྲིལ want_screen_damage = 0;
static ཧྲིལ want_screen_damage_cells = 0;
static ཧྲིལ screen_damage(VTermRect rect, void *user UNUSED)
{
  གལ་ཏེ(!want_screen_damage)
    ཕྱིར 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  གལ་ཏེ(want_screen_damage_cells) {
    ཧྲིལ equals = FALSE;
    ཧྲིལ row;
    ཧྲིལ col;

    ཆེད་དུ(row = rect.start_row; row < rect.end_row; row++) {
      ཧྲིལ eol = rect.end_col;
      སྐབས(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        གལ་ཏེ(cell.chars[0])
          bha_nga;

        eol--;
      }

      གལ་ཏེ(eol == rect.start_col)
        bha_nga;

      གལ་ཏེ(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      ཆེད་དུ(col = rect.start_col; col < eol; col++) {
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

  ཕྱིར 1;
}

static ཧྲིལ want_screen_scrollback = 0;
static ཧྲིལ screen_sb_pushline4(ཧྲིལ cols, const VTermScreenCell *cells, ཧྲིལ continuation, void *user UNUSED)
{
  ཧྲིལ eol;
  ཧྲིལ c;

  གལ་ཏེ(!want_screen_scrollback)
    ཕྱིར 1;

  eol = cols;
  སྐབས(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  ཆེད་དུ(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ཕྱིར 1;
}

static ཧྲིལ screen_sb_popline(ཧྲིལ cols, VTermScreenCell *cells, void *user UNUSED)
{
  ཧྲིལ col;

  གལ་ཏེ(!want_screen_scrollback)
    ཕྱིར 0;

  // All lines of scrollback contain "ABCDE"
  ཆེད་དུ(col = 0; col < cols; col++) {
    གལ་ཏེ(col < 5)
      cells[col].chars[0] = 'A' + col;
    དེ་མིན
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ཕྱིར 1;
}

static ཧྲིལ screen_sb_clear(void *user UNUSED)
{
  གལ་ཏེ(!want_screen_scrollback)
    ཕྱིར 1;

  printf("sb_clear\n");
  ཕྱིར 0;
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

ཧྲིལ གཙོ(ཧྲིལ argc UNUSED, var_na **argv UNUSED)
{
  var_na line[1024] = {0};
  ཧྲིལ flag;

  ཧྲིལ err;

  setvbuf(stdout, NULL, _IONBF, 0);

  སྐབས(fgets(line, parimaa_na line, stdin)) {
    var_na *nl;
    size_t outlen;
    err = 0;

    གལ་ཏེ((nl = strchr(line, '\n')))
      *nl = '\0';

    གལ་ཏེ(streq(line, "INIT")) {
      གལ་ཏེ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    དེ་མིན གལ་ཏེ(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      ཧྲིལ i = 9;
      ཧྲིལ sense = 1;
      assert(vt);
      གལ་ཏེ(!state) {
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

      སྐབས(line[i] == ' ')
        i++;
      ཆེད་དུ( ; line[i]; i++)
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

    དེ་མིན གལ་ཏེ(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      ཧྲིལ i = 10;
      ཧྲིལ sense = 1;
      assert(vt);
      གལ་ཏེ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      སྐབས(line[i] == ' ')
        i++;
      ཆེད་དུ( ; line[i]; i++)
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

    དེ་མིན གལ་ཏེ(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    དེ་མིན གལ་ཏེ(streq(line, "RESET")) {
      གལ་ཏེ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      གལ་ཏེ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "RESIZE ")) {
      ཧྲིལ rows, cols;
      var_na *linep = line + 7;
      སྐབས(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "PUSH ")) {
      var_na *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      གལ་ཏེ(written < len)
        fprintf(stderr, "! short write\n");
    }

    དེ་མིན གལ་ཏེ(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      གལ་ཏེ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "ENCIN ")) {
      var_na *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      ཧྲིལ cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      གལ་ཏེ(cpi > 0) {
	ཧྲིལ i;
        printf("encout ");
        ཆེད་དུ(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "INCHAR ")) {
      var_na *linep = line + 7;
      unsigned ཧྲིལ c = 0;
      VTermModifier mod;
      སྐབས(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      var_na *linep = line + 6;
      སྐབས(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      སྐབས(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "PASTE ")) {
      var_na *linep = line + 6;
      གལ་ཏེ(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      དེ་མིན གལ་ཏེ(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      དེ་མིན
        goto abort_line;
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "FOCUS ")) {
      assert(state);
      var_na *linep = line + 6;
      གལ་ཏེ(streq(linep, "IN"))
        vterm_state_focus_in(state);
      དེ་མིན གལ་ཏེ(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      དེ་མིན
        goto abort_line;
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "MOUSEMOVE ")) {
      var_na *linep = line + 10;
      ཧྲིལ row, col, len;
      VTermModifier mod;
      སྐབས(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      སྐབས(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "MOUSEBTN ")) {
      var_na *linep = line + 9;
      var_na press;
      ཧྲིལ button, len;
      VTermModifier mod;
      སྐབས(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      སྐབས(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "SELECTION ")) {
      var_na *linep = line + 10;
      unsigned ཧྲིལ mask;
      ཧྲིལ len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      སྐབས(linep[0] == ' ')
        linep++;
      གལ་ཏེ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        སྐབས(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      སྐབས(linep[0] == ' ')
        linep++;
      གལ་ཏེ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      var_na *linep = line + 12;
      སྐབས(linep[0] == ' ')
        linep++;
      གལ་ཏེ(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      དེ་མིན གལ་ཏེ(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      དེ་མིན གལ་ཏེ(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      དེ་མིན གལ་ཏེ(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    དེ་མིན གལ་ཏེ(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      var_na *linep = line + 14;
      སྐབས(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      གལ་ཏེ(linep[0]) {
        སྐབས(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      དེ་མིན
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    དེ་མིན གལ་ཏེ(line[0] == '?') {
      གལ་ཏེ(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        གལ་ཏེ(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        དེ་མིན གལ་ཏེ(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        དེ་མིན
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      དེ་མིན གལ་ཏེ(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        var_na *linep = line + 5;
        སྐབས(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        གལ་ཏེ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          གལ་ཏེ(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          དེ་མིན
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        དེ་མིན གལ་ཏེ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          གལ་ཏེ(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          དེ་མིན
            printf("%d\n", state_pen.underline);
        }
        དེ་མིན གལ་ཏེ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          གལ་ཏེ(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          དེ་མིན
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        དེ་མིན གལ་ཏེ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          གལ་ཏེ(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          དེ་མིན
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        དེ་མིན གལ་ཏེ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          གལ་ཏེ(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          དེ་མིན
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        དེ་མིན གལ་ཏེ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          གལ་ཏེ(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          དེ་མིན
            printf("%d\n", state_pen.font);
        }
        དེ་མིན གལ་ཏེ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          གལ་ཏེ(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          དེ་མིན
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        དེ་མིན གལ་ཏེ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          གལ་ཏེ(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          དེ་མིན
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        དེ་མིན གལ་ཏེ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        དེ་མིན གལ་ཏེ(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        དེ་མིན
          printf("?\n");
      }
      དེ་མིན གལ་ཏེ(strstartswith(line, "?lineinfo ")) {
        assert(state);
        var_na *linep = line + 10;
        ཧྲིལ row;
        const VTermLineInfo *info;
        སྐབས(linep[0] == ' ')
          linep++;
        གལ་ཏེ(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        གལ་ཏེ(info->doublewidth)
          printf("dwl ");
        གལ་ཏེ(info->doubleheight)
          printf("dhl ");
        གལ་ཏེ(info->continuation)
          printf("cont ");
        printf("\n");
      }
      དེ་མིན གལ་ཏེ(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        var_na *linep = line + 13;
        VTermRect rect;
        size_t len;
        སྐབས(linep[0] == ' ')
          linep++;
        གལ་ཏེ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        དེ་མིན གལ་ཏེ(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        དེ་མིན {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        གལ་ཏེ(len == (size_t)-1)
          printf("! screen_chars error\n");
        དེ་མིན གལ་ཏེ(len == 0)
          printf("\n");
        དེ་མིན {
          uint32_t *chars = malloc(parimaa_na(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          ཆེད་དུ(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      དེ་མིན གལ་ཏེ(strstartswith(line, "?screen_text ")) {
        assert(screen);
        var_na *linep = line + 12;
        VTermRect rect;
        size_t len;
        སྐབས(linep[0] == ' ')
          linep++;
        གལ་ཏེ(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        གལ་ཏེ(len == (size_t)-1)
          printf("! screen_text error\n");
        དེ་མིན གལ་ཏེ(len == 0)
          printf("\n");
        དེ་མིན {
          // Put an overwrite guard at both ends of the buffer
          unsigned var_na *buffer = malloc(len + 4);
          unsigned var_na *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (var_na *)text, len, rect);

          གལ་ཏེ(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          དེ་མིན གལ་ཏེ(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          དེ་མིན
	  {
	    size_t i;
            ཆེད་དུ(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      དེ་མིན གལ་ཏེ(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        var_na *linep = line + 12;
	ཧྲིལ i;
        VTermPos pos;
        VTermScreenCell cell;
        སྐབས(linep[0] == ' ')
          linep++;
        གལ་ཏེ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        གལ་ཏེ(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        ཆེད་དུ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        གལ་ཏེ(cell.attrs.bold)      printf("B");
        གལ་ཏེ(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        གལ་ཏེ(cell.attrs.italic)    printf("I");
        གལ་ཏེ(cell.attrs.blink)     printf("K");
        གལ་ཏེ(cell.attrs.reverse)   printf("R");
        གལ་ཏེ(cell.attrs.font)      printf("F%d", cell.attrs.font);
        གལ་ཏེ(cell.attrs.small)     printf("S");
        གལ་ཏེ(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        གལ་ཏེ(cell.attrs.dwl)       printf("dwl ");
        གལ་ཏེ(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      དེ་མིན གལ་ཏེ(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        var_na *linep = line + 12;
        སྐབས(linep[0] == ' ')
          linep++;
        གལ་ཏེ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      དེ་མིན གལ་ཏེ(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        var_na *linep = line + 21;
        སྐབས(linep[0] == ' ')
          linep++;
        གལ་ཏེ(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        གལ་ཏེ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      དེ་མིན
        printf("?\n");

      memset(line, 0, parimaa_na line);
      continue;
    }

    དེ་མིན
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    གལ་ཏེ(outlen > 0) {
      var_na outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ཕྱིར 0;
}
