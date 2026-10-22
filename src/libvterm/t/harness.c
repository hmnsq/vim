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

  අතරතුර(*inpos) {
    unsigned පූර්ණ_සංඛ්‍යා ch;
    නම්(sscanf(inpos, "%2x", &ch) < 1)
      bha_nga;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ආපසු outpos - s;
}

static VTermModifier strpe_modifiers(var_na **strp)
{
  VTermModifier state = 0;

  අතරතුර((*strp)[0]) {
    switch(((*strp)++)[0]) {
      pak_sa 'S': state |= VTERM_MOD_SHIFT; bha_nga;
      pak_sa 'C': state |= VTERM_MOD_CTRL;  bha_nga;
      pak_sa 'A': state |= VTERM_MOD_ALT;   bha_nga;
      default: ආපසු state;
    }
  }

  ආපසු state;
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
  පූර්ණ_සංඛ්‍යා i;

  සඳහා(i = 0; keys[i].name; i++) {
    නම්(streq(str, keys[i].name))
      ආපසු keys[i].key;
  }

  ආපසු VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  නම් (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  නැතහොත් නම් (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  නැතහොත් {
    printf("invalid(%d", col->type);
  }
  නම් (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  නම් (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(var_na **strp)
{
  uint8_t r, g, b, idx;
  පූර්ණ_සංඛ්‍යා len = 0;
  VTermColor col;

  නම්(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  නැතහොත් නම්(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  නැතහොත්
    vterm_color_rgb(&col, 127, 127, 127);

  ආපසු col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const var_na *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  සඳහා(i = 0; i < len; i++)
    printf("%x%s", (unsigned var_na)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const var_na *s, size_t len)
{
  අතරතුර(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static පූර්ණ_සංඛ්‍යා parser_text(const var_na bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  සඳහා(i = 0; i < len; i++) {
    unsigned var_na b = bytes[i];
    නම්(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      bha_nga;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ආපසු i;
}

static පූර්ණ_සංඛ්‍යා parser_control(unsigned var_na control, void *user UNUSED)
{
  printf("control %02x\n", control);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා parser_escape(const var_na bytes[], size_t len, void *user UNUSED)
{
  නම්(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    නම්(len < 2)
      ආපසු -1;
    len = 2;
  }
  නැතහොත් {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ආපසු len;
}

static පූර්ණ_සංඛ්‍යා parser_csi(const var_na *leader, const long args[], පූර්ණ_සංඛ්‍යා argcount, const var_na *intermed, var_na command, void *user UNUSED)
{
  පූර්ණ_සංඛ්‍යා i;
  printf("csi %02x", command);

  නම්(leader && leader[0]) {
    printf(" L=");
    සඳහා(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  සඳහා(i = 0; i < argcount; i++) {
    var_na sep = i ? ',' : ' ';

    නම්(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    නැතහොත්
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  නම්(intermed && intermed[0]) {
    printf(" I=");
    සඳහා(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා parser_osc(පූර්ණ_සංඛ්‍යා command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  නම්(frag.initial) {
    නම්(command == -1)
      printf("[");
    නැතහොත්
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  නම්(frag.final)
    printf("]");

  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා parser_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  නම්(frag.initial) {
    size_t i;
    printf("[");
    සඳහා(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  නම්(frag.final)
    printf("]");

  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  නම්(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  නම්(frag.final)
    printf("]");

  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  නම්(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  නම්(frag.final)
    printf("]");

  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  නම්(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  නම්(frag.final)
    printf("]");

  printf("\n");

  ආපසු 1;
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

static පූර්ණ_සංඛ්‍යා want_movecursor = 0;
static VTermPos state_pos;
static පූර්ණ_සංඛ්‍යා movecursor(VTermPos pos, VTermPos oldpos UNUSED, පූර්ණ_සංඛ්‍යා visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  නම්(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා want_premove = 0;
static පූර්ණ_සංඛ්‍යා premove(VTermRect rect, void *user UNUSED)
{
  නම්(!want_premove)
    ආපසු 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා want_scrollrect = 0;
static පූර්ණ_සංඛ්‍යා scrollrect(VTermRect rect, පූර්ණ_සංඛ්‍යා downward, පූර්ණ_සංඛ්‍යා rightward, void *user UNUSED)
{
  නම්(!want_scrollrect)
    ආපසු 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා want_moverect = 0;
static පූර්ණ_සංඛ්‍යා moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  නම්(!want_moverect)
    ආපසු 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා want_settermprop = 0;
static පූර්ණ_සංඛ්‍යා settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  නම්(!want_settermprop)
    ආපසු 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  pak_sa VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ආපසු 1;
  pak_sa VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    ආපසු 1;
  pak_sa VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ආපසු 1;
  pak_sa VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ආපසු 1;

  pak_sa VTERM_N_VALUETYPES:
    ආපසු 0;
  }

  ආපසු 0;
}

// These callbacks are for State

static පූර්ණ_සංඛ්‍යා want_state_putglyph = 0;
static පූර්ණ_සංඛ්‍යා state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  පූර්ණ_සංඛ්‍යා i;
  නම්(!want_state_putglyph)
    ආපසු 1;

  printf("putglyph ");
  සඳහා(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  නම්(info->protected_cell)
    printf(" prot");
  නම්(info->dwl)
    printf(" dwl");
  නම්(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා want_state_erase = 0;
static පූර්ණ_සංඛ්‍යා state_erase(VTermRect rect, පූර්ණ_සංඛ්‍යා selective, void *user UNUSED)
{
  නම්(!want_state_erase)
    ආපසු 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ආපසු 1;
}

static sa_nracanaa {
  පූර්ණ_සංඛ්‍යා bold;
  පූර්ණ_සංඛ්‍යා underline;
  පූර්ණ_සංඛ්‍යා italic;
  පූර්ණ_සංඛ්‍යා blink;
  පූර්ණ_සංඛ්‍යා reverse;
  පූර්ණ_සංඛ්‍යා conceal;
  පූර්ණ_සංඛ්‍යා strike;
  පූර්ණ_සංඛ්‍යා font;
  පූර්ණ_සංඛ්‍යා small;
  පූර්ණ_සංඛ්‍යා baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static පූර්ණ_සංඛ්‍යා state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    ආපසු 0;
  }

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා state_setlineinfo(පූර්ණ_සංඛ්‍යා row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා want_state_scrollback = 0;
static පූර්ණ_සංඛ්‍යා state_sb_clear(void *user UNUSED) {
  නම්(!want_state_scrollback)
    ආපසු 1;

  printf("sb_clear\n");
  ආපසු 0;
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

static පූර්ණ_සංඛ්‍යා selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  නම්(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  නම්(frag.final)
    printf("]");
  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ආපසු 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static පූර්ණ_සංඛ්‍යා want_screen_damage = 0;
static පූර්ණ_සංඛ්‍යා want_screen_damage_cells = 0;
static පූර්ණ_සංඛ්‍යා screen_damage(VTermRect rect, void *user UNUSED)
{
  නම්(!want_screen_damage)
    ආපසු 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  නම්(want_screen_damage_cells) {
    පූර්ණ_සංඛ්‍යා equals = FALSE;
    පූර්ණ_සංඛ්‍යා row;
    පූර්ණ_සංඛ්‍යා col;

    සඳහා(row = rect.start_row; row < rect.end_row; row++) {
      පූර්ණ_සංඛ්‍යා eol = rect.end_col;
      අතරතුර(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        නම්(cell.chars[0])
          bha_nga;

        eol--;
      }

      නම්(eol == rect.start_col)
        bha_nga;

      නම්(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      සඳහා(col = rect.start_col; col < eol; col++) {
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

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා want_screen_scrollback = 0;
static පූර්ණ_සංඛ්‍යා screen_sb_pushline4(පූර්ණ_සංඛ්‍යා cols, const VTermScreenCell *cells, පූර්ණ_සංඛ්‍යා continuation, void *user UNUSED)
{
  පූර්ණ_සංඛ්‍යා eol;
  පූර්ණ_සංඛ්‍යා c;

  නම්(!want_screen_scrollback)
    ආපසු 1;

  eol = cols;
  අතරතුර(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  සඳහා(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා screen_sb_popline(පූර්ණ_සංඛ්‍යා cols, VTermScreenCell *cells, void *user UNUSED)
{
  පූර්ණ_සංඛ්‍යා col;

  නම්(!want_screen_scrollback)
    ආපසු 0;

  // All lines of scrollback contain "ABCDE"
  සඳහා(col = 0; col < cols; col++) {
    නම්(col < 5)
      cells[col].chars[0] = 'A' + col;
    නැතහොත්
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා screen_sb_clear(void *user UNUSED)
{
  නම්(!want_screen_scrollback)
    ආපසු 1;

  printf("sb_clear\n");
  ආපසු 0;
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

පූර්ණ_සංඛ්‍යා main(පූර්ණ_සංඛ්‍යා argc UNUSED, var_na **argv UNUSED)
{
  var_na line[1024] = {0};
  පූර්ණ_සංඛ්‍යා flag;

  පූර්ණ_සංඛ්‍යා err;

  setvbuf(stdout, NULL, _IONBF, 0);

  අතරතුර(fgets(line, parimaa_na line, stdin)) {
    var_na *nl;
    size_t outlen;
    err = 0;

    නම්((nl = strchr(line, '\n')))
      *nl = '\0';

    නම්(streq(line, "INIT")) {
      නම්(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    නැතහොත් නම්(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    නැතහොත් නම්(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      පූර්ණ_සංඛ්‍යා i = 9;
      පූර්ණ_සංඛ්‍යා sense = 1;
      assert(vt);
      නම්(!state) {
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

      අතරතුර(line[i] == ' ')
        i++;
      සඳහා( ; line[i]; i++)
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

    නැතහොත් නම්(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      පූර්ණ_සංඛ්‍යා i = 10;
      පූර්ණ_සංඛ්‍යා sense = 1;
      assert(vt);
      නම්(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      අතරතුර(line[i] == ' ')
        i++;
      සඳහා( ; line[i]; i++)
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

    නැතහොත් නම්(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    නැතහොත් නම්(streq(line, "RESET")) {
      නම්(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      නම්(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    නැතහොත් නම්(strstartswith(line, "RESIZE ")) {
      පූර්ණ_සංඛ්‍යා rows, cols;
      var_na *linep = line + 7;
      අතරතුර(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    නැතහොත් නම්(strstartswith(line, "PUSH ")) {
      var_na *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      නම්(written < len)
        fprintf(stderr, "! short write\n");
    }

    නැතහොත් නම්(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      නම්(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    නැතහොත් නම්(strstartswith(line, "ENCIN ")) {
      var_na *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      පූර්ණ_සංඛ්‍යා cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      නම්(cpi > 0) {
	පූර්ණ_සංඛ්‍යා i;
        printf("encout ");
        සඳහා(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    නැතහොත් නම්(strstartswith(line, "INCHAR ")) {
      var_na *linep = line + 7;
      unsigned පූර්ණ_සංඛ්‍යා c = 0;
      VTermModifier mod;
      අතරතුර(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    නැතහොත් නම්(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      var_na *linep = line + 6;
      අතරතුර(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      අතරතුර(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    නැතහොත් නම්(strstartswith(line, "PASTE ")) {
      var_na *linep = line + 6;
      නම්(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      නැතහොත් නම්(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      නැතහොත්
        goto abort_line;
    }

    නැතහොත් නම්(strstartswith(line, "FOCUS ")) {
      assert(state);
      var_na *linep = line + 6;
      නම්(streq(linep, "IN"))
        vterm_state_focus_in(state);
      නැතහොත් නම්(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      නැතහොත්
        goto abort_line;
    }

    නැතහොත් නම්(strstartswith(line, "MOUSEMOVE ")) {
      var_na *linep = line + 10;
      පූර්ණ_සංඛ්‍යා row, col, len;
      VTermModifier mod;
      අතරතුර(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      අතරතුර(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    නැතහොත් නම්(strstartswith(line, "MOUSEBTN ")) {
      var_na *linep = line + 9;
      var_na press;
      පූර්ණ_සංඛ්‍යා button, len;
      VTermModifier mod;
      අතරතුර(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      අතරතුර(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    නැතහොත් නම්(strstartswith(line, "SELECTION ")) {
      var_na *linep = line + 10;
      unsigned පූර්ණ_සංඛ්‍යා mask;
      පූර්ණ_සංඛ්‍යා len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      අතරතුර(linep[0] == ' ')
        linep++;
      නම්(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        අතරතුර(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      අතරතුර(linep[0] == ' ')
        linep++;
      නම්(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    නැතහොත් නම්(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      var_na *linep = line + 12;
      අතරතුර(linep[0] == ' ')
        linep++;
      නම්(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      නැතහොත් නම්(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      නැතහොත් නම්(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      නැතහොත් නම්(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    නැතහොත් නම්(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    නැතහොත් නම්(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      var_na *linep = line + 14;
      අතරතුර(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      නම්(linep[0]) {
        අතරතුර(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      නැතහොත්
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    නැතහොත් නම්(line[0] == '?') {
      නම්(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        නම්(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        නැතහොත් නම්(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        නැතහොත්
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      නැතහොත් නම්(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        var_na *linep = line + 5;
        අතරතුර(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        නම්(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          නම්(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          නැතහොත්
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        නැතහොත් නම්(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          නම්(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          නැතහොත්
            printf("%d\n", state_pen.underline);
        }
        නැතහොත් නම්(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          නම්(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          නැතහොත්
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        නැතහොත් නම්(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          නම්(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          නැතහොත්
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        නැතහොත් නම්(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          නම්(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          නැතහොත්
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        නැතහොත් නම්(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          නම්(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          නැතහොත්
            printf("%d\n", state_pen.font);
        }
        නැතහොත් නම්(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          නම්(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          නැතහොත්
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        නැතහොත් නම්(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          නම්(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          නැතහොත්
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        නැතහොත් නම්(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        නැතහොත් නම්(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        නැතහොත්
          printf("?\n");
      }
      නැතහොත් නම්(strstartswith(line, "?lineinfo ")) {
        assert(state);
        var_na *linep = line + 10;
        පූර්ණ_සංඛ්‍යා row;
        const VTermLineInfo *info;
        අතරතුර(linep[0] == ' ')
          linep++;
        නම්(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        නම්(info->doublewidth)
          printf("dwl ");
        නම්(info->doubleheight)
          printf("dhl ");
        නම්(info->continuation)
          printf("cont ");
        printf("\n");
      }
      නැතහොත් නම්(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        var_na *linep = line + 13;
        VTermRect rect;
        size_t len;
        අතරතුර(linep[0] == ' ')
          linep++;
        නම්(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        නැතහොත් නම්(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        නැතහොත් {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        නම්(len == (size_t)-1)
          printf("! screen_chars error\n");
        නැතහොත් නම්(len == 0)
          printf("\n");
        නැතහොත් {
          uint32_t *chars = malloc(parimaa_na(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          සඳහා(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      නැතහොත් නම්(strstartswith(line, "?screen_text ")) {
        assert(screen);
        var_na *linep = line + 12;
        VTermRect rect;
        size_t len;
        අතරතුර(linep[0] == ' ')
          linep++;
        නම්(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        නම්(len == (size_t)-1)
          printf("! screen_text error\n");
        නැතහොත් නම්(len == 0)
          printf("\n");
        නැතහොත් {
          // Put an overwrite guard at both ends of the buffer
          unsigned var_na *buffer = malloc(len + 4);
          unsigned var_na *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (var_na *)text, len, rect);

          නම්(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          නැතහොත් නම්(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          නැතහොත්
	  {
	    size_t i;
            සඳහා(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      නැතහොත් නම්(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        var_na *linep = line + 12;
	පූර්ණ_සංඛ්‍යා i;
        VTermPos pos;
        VTermScreenCell cell;
        අතරතුර(linep[0] == ' ')
          linep++;
        නම්(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        නම්(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        සඳහා(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        නම්(cell.attrs.bold)      printf("B");
        නම්(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        නම්(cell.attrs.italic)    printf("I");
        නම්(cell.attrs.blink)     printf("K");
        නම්(cell.attrs.reverse)   printf("R");
        නම්(cell.attrs.font)      printf("F%d", cell.attrs.font);
        නම්(cell.attrs.small)     printf("S");
        නම්(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        නම්(cell.attrs.dwl)       printf("dwl ");
        නම්(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      නැතහොත් නම්(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        var_na *linep = line + 12;
        අතරතුර(linep[0] == ' ')
          linep++;
        නම්(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      නැතහොත් නම්(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        var_na *linep = line + 21;
        අතරතුර(linep[0] == ' ')
          linep++;
        නම්(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        නම්(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      නැතහොත්
        printf("?\n");

      memset(line, 0, parimaa_na line);
      continue;
    }

    නැතහොත්
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    නම්(outlen > 0) {
      var_na outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ආපසු 0;
}
