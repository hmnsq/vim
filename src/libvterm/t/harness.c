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

  နေစဉ်(*inpos) {
    unsigned ကိန်းပြည့် ch;
    အကယ်၍(sscanf(inpos, "%2x", &ch) < 1)
      bha_nga;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ပြန်ပေး outpos - s;
}

static VTermModifier strpe_modifiers(var_na **strp)
{
  VTermModifier state = 0;

  နေစဉ်((*strp)[0]) {
    switch(((*strp)++)[0]) {
      pak_sa 'S': state |= VTERM_MOD_SHIFT; bha_nga;
      pak_sa 'C': state |= VTERM_MOD_CTRL;  bha_nga;
      pak_sa 'A': state |= VTERM_MOD_ALT;   bha_nga;
      default: ပြန်ပေး state;
    }
  }

  ပြန်ပေး state;
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
  ကိန်းပြည့် i;

  အတွက်(i = 0; keys[i].name; i++) {
    အကယ်၍(streq(str, keys[i].name))
      ပြန်ပေး keys[i].key;
  }

  ပြန်ပေး VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  အကယ်၍ (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  မဟုတ်ပါက အကယ်၍ (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  မဟုတ်ပါက {
    printf("invalid(%d", col->type);
  }
  အကယ်၍ (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  အကယ်၍ (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(var_na **strp)
{
  uint8_t r, g, b, idx;
  ကိန်းပြည့် len = 0;
  VTermColor col;

  အကယ်၍(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  မဟုတ်ပါက အကယ်၍(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  မဟုတ်ပါက
    vterm_color_rgb(&col, 127, 127, 127);

  ပြန်ပေး col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const var_na *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  အတွက်(i = 0; i < len; i++)
    printf("%x%s", (unsigned var_na)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const var_na *s, size_t len)
{
  နေစဉ်(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static ကိန်းပြည့် parser_text(const var_na bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  အတွက်(i = 0; i < len; i++) {
    unsigned var_na b = bytes[i];
    အကယ်၍(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      bha_nga;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ပြန်ပေး i;
}

static ကိန်းပြည့် parser_control(unsigned var_na control, void *user UNUSED)
{
  printf("control %02x\n", control);

  ပြန်ပေး 1;
}

static ကိန်းပြည့် parser_escape(const var_na bytes[], size_t len, void *user UNUSED)
{
  အကယ်၍(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    အကယ်၍(len < 2)
      ပြန်ပေး -1;
    len = 2;
  }
  မဟုတ်ပါက {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ပြန်ပေး len;
}

static ကိန်းပြည့် parser_csi(const var_na *leader, const long args[], ကိန်းပြည့် argcount, const var_na *intermed, var_na command, void *user UNUSED)
{
  ကိန်းပြည့် i;
  printf("csi %02x", command);

  အကယ်၍(leader && leader[0]) {
    printf(" L=");
    အတွက်(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  အတွက်(i = 0; i < argcount; i++) {
    var_na sep = i ? ',' : ' ';

    အကယ်၍(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    မဟုတ်ပါက
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  အကယ်၍(intermed && intermed[0]) {
    printf(" I=");
    အတွက်(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် parser_osc(ကိန်းပြည့် command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  အကယ်၍(frag.initial) {
    အကယ်၍(command == -1)
      printf("[");
    မဟုတ်ပါက
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  အကယ်၍(frag.final)
    printf("]");

  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် parser_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  အကယ်၍(frag.initial) {
    size_t i;
    printf("[");
    အတွက်(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  အကယ်၍(frag.final)
    printf("]");

  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  အကယ်၍(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  အကယ်၍(frag.final)
    printf("]");

  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  အကယ်၍(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  အကယ်၍(frag.final)
    printf("]");

  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  အကယ်၍(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  အကယ်၍(frag.final)
    printf("]");

  printf("\n");

  ပြန်ပေး 1;
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

static ကိန်းပြည့် want_movecursor = 0;
static VTermPos state_pos;
static ကိန်းပြည့် movecursor(VTermPos pos, VTermPos oldpos UNUSED, ကိန်းပြည့် visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  အကယ်၍(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ပြန်ပေး 1;
}

static ကိန်းပြည့် want_premove = 0;
static ကိန်းပြည့် premove(VTermRect rect, void *user UNUSED)
{
  အကယ်၍(!want_premove)
    ပြန်ပေး 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ပြန်ပေး 1;
}

static ကိန်းပြည့် want_scrollrect = 0;
static ကိန်းပြည့် scrollrect(VTermRect rect, ကိန်းပြည့် downward, ကိန်းပြည့် rightward, void *user UNUSED)
{
  အကယ်၍(!want_scrollrect)
    ပြန်ပေး 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ပြန်ပေး 1;
}

static ကိန်းပြည့် want_moverect = 0;
static ကိန်းပြည့် moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  အကယ်၍(!want_moverect)
    ပြန်ပေး 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ပြန်ပေး 1;
}

static ကိန်းပြည့် want_settermprop = 0;
static ကိန်းပြည့် settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  အကယ်၍(!want_settermprop)
    ပြန်ပေး 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  pak_sa VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ပြန်ပေး 1;
  pak_sa VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    ပြန်ပေး 1;
  pak_sa VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ပြန်ပေး 1;
  pak_sa VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ပြန်ပေး 1;

  pak_sa VTERM_N_VALUETYPES:
    ပြန်ပေး 0;
  }

  ပြန်ပေး 0;
}

// These callbacks are for State

static ကိန်းပြည့် want_state_putglyph = 0;
static ကိန်းပြည့် state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  ကိန်းပြည့် i;
  အကယ်၍(!want_state_putglyph)
    ပြန်ပေး 1;

  printf("putglyph ");
  အတွက်(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  အကယ်၍(info->protected_cell)
    printf(" prot");
  အကယ်၍(info->dwl)
    printf(" dwl");
  အကယ်၍(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် want_state_erase = 0;
static ကိန်းပြည့် state_erase(VTermRect rect, ကိန်းပြည့် selective, void *user UNUSED)
{
  အကယ်၍(!want_state_erase)
    ပြန်ပေး 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ပြန်ပေး 1;
}

static sa_nracanaa {
  ကိန်းပြည့် bold;
  ကိန်းပြည့် underline;
  ကိန်းပြည့် italic;
  ကိန်းပြည့် blink;
  ကိန်းပြည့် reverse;
  ကိန်းပြည့် conceal;
  ကိန်းပြည့် strike;
  ကိန်းပြည့် font;
  ကိန်းပြည့် small;
  ကိန်းပြည့် baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static ကိန်းပြည့် state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    ပြန်ပေး 0;
  }

  ပြန်ပေး 1;
}

static ကိန်းပြည့် state_setlineinfo(ကိန်းပြည့် row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  ပြန်ပေး 1;
}

static ကိန်းပြည့် want_state_scrollback = 0;
static ကိန်းပြည့် state_sb_clear(void *user UNUSED) {
  အကယ်၍(!want_state_scrollback)
    ပြန်ပေး 1;

  printf("sb_clear\n");
  ပြန်ပေး 0;
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

static ကိန်းပြည့် selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  အကယ်၍(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  အကယ်၍(frag.final)
    printf("]");
  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ပြန်ပေး 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static ကိန်းပြည့် want_screen_damage = 0;
static ကိန်းပြည့် want_screen_damage_cells = 0;
static ကိန်းပြည့် screen_damage(VTermRect rect, void *user UNUSED)
{
  အကယ်၍(!want_screen_damage)
    ပြန်ပေး 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  အကယ်၍(want_screen_damage_cells) {
    ကိန်းပြည့် equals = FALSE;
    ကိန်းပြည့် row;
    ကိန်းပြည့် col;

    အတွက်(row = rect.start_row; row < rect.end_row; row++) {
      ကိန်းပြည့် eol = rect.end_col;
      နေစဉ်(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        အကယ်၍(cell.chars[0])
          bha_nga;

        eol--;
      }

      အကယ်၍(eol == rect.start_col)
        bha_nga;

      အကယ်၍(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      အတွက်(col = rect.start_col; col < eol; col++) {
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

  ပြန်ပေး 1;
}

static ကိန်းပြည့် want_screen_scrollback = 0;
static ကိန်းပြည့် screen_sb_pushline4(ကိန်းပြည့် cols, const VTermScreenCell *cells, ကိန်းပြည့် continuation, void *user UNUSED)
{
  ကိန်းပြည့် eol;
  ကိန်းပြည့် c;

  အကယ်၍(!want_screen_scrollback)
    ပြန်ပေး 1;

  eol = cols;
  နေစဉ်(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  အတွက်(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ပြန်ပေး 1;
}

static ကိန်းပြည့် screen_sb_popline(ကိန်းပြည့် cols, VTermScreenCell *cells, void *user UNUSED)
{
  ကိန်းပြည့် col;

  အကယ်၍(!want_screen_scrollback)
    ပြန်ပေး 0;

  // All lines of scrollback contain "ABCDE"
  အတွက်(col = 0; col < cols; col++) {
    အကယ်၍(col < 5)
      cells[col].chars[0] = 'A' + col;
    မဟုတ်ပါက
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ပြန်ပေး 1;
}

static ကိန်းပြည့် screen_sb_clear(void *user UNUSED)
{
  အကယ်၍(!want_screen_scrollback)
    ပြန်ပေး 1;

  printf("sb_clear\n");
  ပြန်ပေး 0;
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

ကိန်းပြည့် main(ကိန်းပြည့် argc UNUSED, var_na **argv UNUSED)
{
  var_na line[1024] = {0};
  ကိန်းပြည့် flag;

  ကိန်းပြည့် err;

  setvbuf(stdout, NULL, _IONBF, 0);

  နေစဉ်(fgets(line, parimaa_na line, stdin)) {
    var_na *nl;
    size_t outlen;
    err = 0;

    အကယ်၍((nl = strchr(line, '\n')))
      *nl = '\0';

    အကယ်၍(streq(line, "INIT")) {
      အကယ်၍(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    မဟုတ်ပါက အကယ်၍(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      ကိန်းပြည့် i = 9;
      ကိန်းပြည့် sense = 1;
      assert(vt);
      အကယ်၍(!state) {
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

      နေစဉ်(line[i] == ' ')
        i++;
      အတွက်( ; line[i]; i++)
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

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      ကိန်းပြည့် i = 10;
      ကိန်းပြည့် sense = 1;
      assert(vt);
      အကယ်၍(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      နေစဉ်(line[i] == ' ')
        i++;
      အတွက်( ; line[i]; i++)
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

    မဟုတ်ပါက အကယ်၍(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    မဟုတ်ပါက အကယ်၍(streq(line, "RESET")) {
      အကယ်၍(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      အကယ်၍(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "RESIZE ")) {
      ကိန်းပြည့် rows, cols;
      var_na *linep = line + 7;
      နေစဉ်(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "PUSH ")) {
      var_na *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      အကယ်၍(written < len)
        fprintf(stderr, "! short write\n");
    }

    မဟုတ်ပါက အကယ်၍(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      အကယ်၍(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "ENCIN ")) {
      var_na *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      ကိန်းပြည့် cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      အကယ်၍(cpi > 0) {
	ကိန်းပြည့် i;
        printf("encout ");
        အတွက်(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "INCHAR ")) {
      var_na *linep = line + 7;
      unsigned ကိန်းပြည့် c = 0;
      VTermModifier mod;
      နေစဉ်(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      var_na *linep = line + 6;
      နေစဉ်(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      နေစဉ်(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "PASTE ")) {
      var_na *linep = line + 6;
      အကယ်၍(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      မဟုတ်ပါက အကယ်၍(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      မဟုတ်ပါက
        goto abort_line;
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "FOCUS ")) {
      assert(state);
      var_na *linep = line + 6;
      အကယ်၍(streq(linep, "IN"))
        vterm_state_focus_in(state);
      မဟုတ်ပါက အကယ်၍(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      မဟုတ်ပါက
        goto abort_line;
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "MOUSEMOVE ")) {
      var_na *linep = line + 10;
      ကိန်းပြည့် row, col, len;
      VTermModifier mod;
      နေစဉ်(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      နေစဉ်(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "MOUSEBTN ")) {
      var_na *linep = line + 9;
      var_na press;
      ကိန်းပြည့် button, len;
      VTermModifier mod;
      နေစဉ်(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      နေစဉ်(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "SELECTION ")) {
      var_na *linep = line + 10;
      unsigned ကိန်းပြည့် mask;
      ကိန်းပြည့် len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      နေစဉ်(linep[0] == ' ')
        linep++;
      အကယ်၍(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        နေစဉ်(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      နေစဉ်(linep[0] == ' ')
        linep++;
      အကယ်၍(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      var_na *linep = line + 12;
      နေစဉ်(linep[0] == ' ')
        linep++;
      အကယ်၍(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      မဟုတ်ပါက အကယ်၍(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      မဟုတ်ပါက အကယ်၍(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      မဟုတ်ပါက အကယ်၍(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    မဟုတ်ပါက အကယ်၍(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      var_na *linep = line + 14;
      နေစဉ်(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      အကယ်၍(linep[0]) {
        နေစဉ်(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      မဟုတ်ပါက
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    မဟုတ်ပါက အကယ်၍(line[0] == '?') {
      အကယ်၍(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        အကယ်၍(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        မဟုတ်ပါက အကယ်၍(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        မဟုတ်ပါက
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      မဟုတ်ပါက အကယ်၍(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        var_na *linep = line + 5;
        နေစဉ်(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        အကယ်၍(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          အကယ်၍(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          မဟုတ်ပါက
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          အကယ်၍(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          မဟုတ်ပါက
            printf("%d\n", state_pen.underline);
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          အကယ်၍(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          မဟုတ်ပါက
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          အကယ်၍(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          မဟုတ်ပါက
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          အကယ်၍(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          မဟုတ်ပါက
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          အကယ်၍(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          မဟုတ်ပါက
            printf("%d\n", state_pen.font);
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          အကယ်၍(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          မဟုတ်ပါက
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          အကယ်၍(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          မဟုတ်ပါက
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        မဟုတ်ပါက အကယ်၍(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        မဟုတ်ပါက
          printf("?\n");
      }
      မဟုတ်ပါက အကယ်၍(strstartswith(line, "?lineinfo ")) {
        assert(state);
        var_na *linep = line + 10;
        ကိန်းပြည့် row;
        const VTermLineInfo *info;
        နေစဉ်(linep[0] == ' ')
          linep++;
        အကယ်၍(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        အကယ်၍(info->doublewidth)
          printf("dwl ");
        အကယ်၍(info->doubleheight)
          printf("dhl ");
        အကယ်၍(info->continuation)
          printf("cont ");
        printf("\n");
      }
      မဟုတ်ပါက အကယ်၍(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        var_na *linep = line + 13;
        VTermRect rect;
        size_t len;
        နေစဉ်(linep[0] == ' ')
          linep++;
        အကယ်၍(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        မဟုတ်ပါက အကယ်၍(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        မဟုတ်ပါက {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        အကယ်၍(len == (size_t)-1)
          printf("! screen_chars error\n");
        မဟုတ်ပါက အကယ်၍(len == 0)
          printf("\n");
        မဟုတ်ပါက {
          uint32_t *chars = malloc(parimaa_na(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          အတွက်(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      မဟုတ်ပါက အကယ်၍(strstartswith(line, "?screen_text ")) {
        assert(screen);
        var_na *linep = line + 12;
        VTermRect rect;
        size_t len;
        နေစဉ်(linep[0] == ' ')
          linep++;
        အကယ်၍(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        အကယ်၍(len == (size_t)-1)
          printf("! screen_text error\n");
        မဟုတ်ပါက အကယ်၍(len == 0)
          printf("\n");
        မဟုတ်ပါက {
          // Put an overwrite guard at both ends of the buffer
          unsigned var_na *buffer = malloc(len + 4);
          unsigned var_na *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (var_na *)text, len, rect);

          အကယ်၍(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          မဟုတ်ပါက အကယ်၍(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          မဟုတ်ပါက
	  {
	    size_t i;
            အတွက်(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      မဟုတ်ပါက အကယ်၍(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        var_na *linep = line + 12;
	ကိန်းပြည့် i;
        VTermPos pos;
        VTermScreenCell cell;
        နေစဉ်(linep[0] == ' ')
          linep++;
        အကယ်၍(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        အကယ်၍(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        အတွက်(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        အကယ်၍(cell.attrs.bold)      printf("B");
        အကယ်၍(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        အကယ်၍(cell.attrs.italic)    printf("I");
        အကယ်၍(cell.attrs.blink)     printf("K");
        အကယ်၍(cell.attrs.reverse)   printf("R");
        အကယ်၍(cell.attrs.font)      printf("F%d", cell.attrs.font);
        အကယ်၍(cell.attrs.small)     printf("S");
        အကယ်၍(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        အကယ်၍(cell.attrs.dwl)       printf("dwl ");
        အကယ်၍(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      မဟုတ်ပါက အကယ်၍(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        var_na *linep = line + 12;
        နေစဉ်(linep[0] == ' ')
          linep++;
        အကယ်၍(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      မဟုတ်ပါက အကယ်၍(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        var_na *linep = line + 21;
        နေစဉ်(linep[0] == ' ')
          linep++;
        အကယ်၍(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        အကယ်၍(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      မဟုတ်ပါက
        printf("?\n");

      memset(line, 0, parimaa_na line);
      continue;
    }

    မဟုတ်ပါက
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    အကယ်၍(outlen > 0) {
      var_na outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ပြန်ပေး 0;
}
