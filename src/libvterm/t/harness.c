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

  ขณะ(*inpos) {
    unsigned จำนวนเต็ม ch;
    ถ้า(sscanf(inpos, "%2x", &ch) < 1)
      bha_nga;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  คืน outpos - s;
}

static VTermModifier strpe_modifiers(var_na **strp)
{
  VTermModifier state = 0;

  ขณะ((*strp)[0]) {
    switch(((*strp)++)[0]) {
      pak_sa 'S': state |= VTERM_MOD_SHIFT; bha_nga;
      pak_sa 'C': state |= VTERM_MOD_CTRL;  bha_nga;
      pak_sa 'A': state |= VTERM_MOD_ALT;   bha_nga;
      default: คืน state;
    }
  }

  คืน state;
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
  จำนวนเต็ม i;

  สำหรับ(i = 0; keys[i].name; i++) {
    ถ้า(streq(str, keys[i].name))
      คืน keys[i].key;
  }

  คืน VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  ถ้า (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  มิฉะนั้น ถ้า (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  มิฉะนั้น {
    printf("invalid(%d", col->type);
  }
  ถ้า (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  ถ้า (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(var_na **strp)
{
  uint8_t r, g, b, idx;
  จำนวนเต็ม len = 0;
  VTermColor col;

  ถ้า(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  มิฉะนั้น ถ้า(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  มิฉะนั้น
    vterm_color_rgb(&col, 127, 127, 127);

  คืน col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const var_na *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  สำหรับ(i = 0; i < len; i++)
    printf("%x%s", (unsigned var_na)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const var_na *s, size_t len)
{
  ขณะ(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static จำนวนเต็ม parser_text(const var_na bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  สำหรับ(i = 0; i < len; i++) {
    unsigned var_na b = bytes[i];
    ถ้า(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      bha_nga;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  คืน i;
}

static จำนวนเต็ม parser_control(unsigned var_na control, void *user UNUSED)
{
  printf("control %02x\n", control);

  คืน 1;
}

static จำนวนเต็ม parser_escape(const var_na bytes[], size_t len, void *user UNUSED)
{
  ถ้า(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    ถ้า(len < 2)
      คืน -1;
    len = 2;
  }
  มิฉะนั้น {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  คืน len;
}

static จำนวนเต็ม parser_csi(const var_na *leader, const long args[], จำนวนเต็ม argcount, const var_na *intermed, var_na command, void *user UNUSED)
{
  จำนวนเต็ม i;
  printf("csi %02x", command);

  ถ้า(leader && leader[0]) {
    printf(" L=");
    สำหรับ(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  สำหรับ(i = 0; i < argcount; i++) {
    var_na sep = i ? ',' : ' ';

    ถ้า(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    มิฉะนั้น
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  ถ้า(intermed && intermed[0]) {
    printf(" I=");
    สำหรับ(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  คืน 1;
}

static จำนวนเต็ม parser_osc(จำนวนเต็ม command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  ถ้า(frag.initial) {
    ถ้า(command == -1)
      printf("[");
    มิฉะนั้น
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ถ้า(frag.final)
    printf("]");

  printf("\n");

  คืน 1;
}

static จำนวนเต็ม parser_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  ถ้า(frag.initial) {
    size_t i;
    printf("[");
    สำหรับ(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ถ้า(frag.final)
    printf("]");

  printf("\n");

  คืน 1;
}

static จำนวนเต็ม parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  ถ้า(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ถ้า(frag.final)
    printf("]");

  printf("\n");

  คืน 1;
}

static จำนวนเต็ม parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  ถ้า(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ถ้า(frag.final)
    printf("]");

  printf("\n");

  คืน 1;
}

static จำนวนเต็ม parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  ถ้า(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ถ้า(frag.final)
    printf("]");

  printf("\n");

  คืน 1;
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

static จำนวนเต็ม want_movecursor = 0;
static VTermPos state_pos;
static จำนวนเต็ม movecursor(VTermPos pos, VTermPos oldpos UNUSED, จำนวนเต็ม visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  ถ้า(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  คืน 1;
}

static จำนวนเต็ม want_premove = 0;
static จำนวนเต็ม premove(VTermRect rect, void *user UNUSED)
{
  ถ้า(!want_premove)
    คืน 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  คืน 1;
}

static จำนวนเต็ม want_scrollrect = 0;
static จำนวนเต็ม scrollrect(VTermRect rect, จำนวนเต็ม downward, จำนวนเต็ม rightward, void *user UNUSED)
{
  ถ้า(!want_scrollrect)
    คืน 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  คืน 1;
}

static จำนวนเต็ม want_moverect = 0;
static จำนวนเต็ม moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  ถ้า(!want_moverect)
    คืน 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  คืน 1;
}

static จำนวนเต็ม want_settermprop = 0;
static จำนวนเต็ม settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  ถ้า(!want_settermprop)
    คืน 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  pak_sa VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    คืน 1;
  pak_sa VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    คืน 1;
  pak_sa VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    คืน 1;
  pak_sa VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    คืน 1;

  pak_sa VTERM_N_VALUETYPES:
    คืน 0;
  }

  คืน 0;
}

// These callbacks are for State

static จำนวนเต็ม want_state_putglyph = 0;
static จำนวนเต็ม state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  จำนวนเต็ม i;
  ถ้า(!want_state_putglyph)
    คืน 1;

  printf("putglyph ");
  สำหรับ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  ถ้า(info->protected_cell)
    printf(" prot");
  ถ้า(info->dwl)
    printf(" dwl");
  ถ้า(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  คืน 1;
}

static จำนวนเต็ม want_state_erase = 0;
static จำนวนเต็ม state_erase(VTermRect rect, จำนวนเต็ม selective, void *user UNUSED)
{
  ถ้า(!want_state_erase)
    คืน 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  คืน 1;
}

static sa_nracanaa {
  จำนวนเต็ม bold;
  จำนวนเต็ม underline;
  จำนวนเต็ม italic;
  จำนวนเต็ม blink;
  จำนวนเต็ม reverse;
  จำนวนเต็ม conceal;
  จำนวนเต็ม strike;
  จำนวนเต็ม font;
  จำนวนเต็ม small;
  จำนวนเต็ม baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static จำนวนเต็ม state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    คืน 0;
  }

  คืน 1;
}

static จำนวนเต็ม state_setlineinfo(จำนวนเต็ม row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  คืน 1;
}

static จำนวนเต็ม want_state_scrollback = 0;
static จำนวนเต็ม state_sb_clear(void *user UNUSED) {
  ถ้า(!want_state_scrollback)
    คืน 1;

  printf("sb_clear\n");
  คืน 0;
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

static จำนวนเต็ม selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  ถ้า(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  ถ้า(frag.final)
    printf("]");
  printf("\n");

  คืน 1;
}

static จำนวนเต็ม selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  คืน 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static จำนวนเต็ม want_screen_damage = 0;
static จำนวนเต็ม want_screen_damage_cells = 0;
static จำนวนเต็ม screen_damage(VTermRect rect, void *user UNUSED)
{
  ถ้า(!want_screen_damage)
    คืน 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ถ้า(want_screen_damage_cells) {
    จำนวนเต็ม equals = FALSE;
    จำนวนเต็ม row;
    จำนวนเต็ม col;

    สำหรับ(row = rect.start_row; row < rect.end_row; row++) {
      จำนวนเต็ม eol = rect.end_col;
      ขณะ(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ถ้า(cell.chars[0])
          bha_nga;

        eol--;
      }

      ถ้า(eol == rect.start_col)
        bha_nga;

      ถ้า(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      สำหรับ(col = rect.start_col; col < eol; col++) {
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

  คืน 1;
}

static จำนวนเต็ม want_screen_scrollback = 0;
static จำนวนเต็ม screen_sb_pushline4(จำนวนเต็ม cols, const VTermScreenCell *cells, จำนวนเต็ม continuation, void *user UNUSED)
{
  จำนวนเต็ม eol;
  จำนวนเต็ม c;

  ถ้า(!want_screen_scrollback)
    คืน 1;

  eol = cols;
  ขณะ(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  สำหรับ(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  คืน 1;
}

static จำนวนเต็ม screen_sb_popline(จำนวนเต็ม cols, VTermScreenCell *cells, void *user UNUSED)
{
  จำนวนเต็ม col;

  ถ้า(!want_screen_scrollback)
    คืน 0;

  // All lines of scrollback contain "ABCDE"
  สำหรับ(col = 0; col < cols; col++) {
    ถ้า(col < 5)
      cells[col].chars[0] = 'A' + col;
    มิฉะนั้น
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  คืน 1;
}

static จำนวนเต็ม screen_sb_clear(void *user UNUSED)
{
  ถ้า(!want_screen_scrollback)
    คืน 1;

  printf("sb_clear\n");
  คืน 0;
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

จำนวนเต็ม หลัก(จำนวนเต็ม argc UNUSED, var_na **argv UNUSED)
{
  var_na line[1024] = {0};
  จำนวนเต็ม flag;

  จำนวนเต็ม err;

  setvbuf(stdout, NULL, _IONBF, 0);

  ขณะ(fgets(line, parimaa_na line, stdin)) {
    var_na *nl;
    size_t outlen;
    err = 0;

    ถ้า((nl = strchr(line, '\n')))
      *nl = '\0';

    ถ้า(streq(line, "INIT")) {
      ถ้า(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    มิฉะนั้น ถ้า(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      จำนวนเต็ม i = 9;
      จำนวนเต็ม sense = 1;
      assert(vt);
      ถ้า(!state) {
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

      ขณะ(line[i] == ' ')
        i++;
      สำหรับ( ; line[i]; i++)
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

    มิฉะนั้น ถ้า(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      จำนวนเต็ม i = 10;
      จำนวนเต็ม sense = 1;
      assert(vt);
      ถ้า(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      ขณะ(line[i] == ' ')
        i++;
      สำหรับ( ; line[i]; i++)
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

    มิฉะนั้น ถ้า(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    มิฉะนั้น ถ้า(streq(line, "RESET")) {
      ถ้า(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ถ้า(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    มิฉะนั้น ถ้า(strstartswith(line, "RESIZE ")) {
      จำนวนเต็ม rows, cols;
      var_na *linep = line + 7;
      ขณะ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "PUSH ")) {
      var_na *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      ถ้า(written < len)
        fprintf(stderr, "! short write\n");
    }

    มิฉะนั้น ถ้า(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ถ้า(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "ENCIN ")) {
      var_na *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      จำนวนเต็ม cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      ถ้า(cpi > 0) {
	จำนวนเต็ม i;
        printf("encout ");
        สำหรับ(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    มิฉะนั้น ถ้า(strstartswith(line, "INCHAR ")) {
      var_na *linep = line + 7;
      unsigned จำนวนเต็ม c = 0;
      VTermModifier mod;
      ขณะ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      var_na *linep = line + 6;
      ขณะ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      ขณะ(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "PASTE ")) {
      var_na *linep = line + 6;
      ถ้า(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      มิฉะนั้น ถ้า(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      มิฉะนั้น
        goto abort_line;
    }

    มิฉะนั้น ถ้า(strstartswith(line, "FOCUS ")) {
      assert(state);
      var_na *linep = line + 6;
      ถ้า(streq(linep, "IN"))
        vterm_state_focus_in(state);
      มิฉะนั้น ถ้า(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      มิฉะนั้น
        goto abort_line;
    }

    มิฉะนั้น ถ้า(strstartswith(line, "MOUSEMOVE ")) {
      var_na *linep = line + 10;
      จำนวนเต็ม row, col, len;
      VTermModifier mod;
      ขณะ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      ขณะ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "MOUSEBTN ")) {
      var_na *linep = line + 9;
      var_na press;
      จำนวนเต็ม button, len;
      VTermModifier mod;
      ขณะ(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      ขณะ(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "SELECTION ")) {
      var_na *linep = line + 10;
      unsigned จำนวนเต็ม mask;
      จำนวนเต็ม len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      ขณะ(linep[0] == ' ')
        linep++;
      ถ้า(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        ขณะ(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      ขณะ(linep[0] == ' ')
        linep++;
      ถ้า(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      var_na *linep = line + 12;
      ขณะ(linep[0] == ' ')
        linep++;
      ถ้า(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      มิฉะนั้น ถ้า(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      มิฉะนั้น ถ้า(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      มิฉะนั้น ถ้า(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    มิฉะนั้น ถ้า(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      var_na *linep = line + 14;
      ขณะ(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ถ้า(linep[0]) {
        ขณะ(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      มิฉะนั้น
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    มิฉะนั้น ถ้า(line[0] == '?') {
      ถ้า(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ถ้า(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        มิฉะนั้น ถ้า(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        มิฉะนั้น
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      มิฉะนั้น ถ้า(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        var_na *linep = line + 5;
        ขณะ(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        ถ้า(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ถ้า(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          มิฉะนั้น
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        มิฉะนั้น ถ้า(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ถ้า(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          มิฉะนั้น
            printf("%d\n", state_pen.underline);
        }
        มิฉะนั้น ถ้า(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ถ้า(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          มิฉะนั้น
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        มิฉะนั้น ถ้า(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ถ้า(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          มิฉะนั้น
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        มิฉะนั้น ถ้า(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          ถ้า(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          มิฉะนั้น
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        มิฉะนั้น ถ้า(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ถ้า(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          มิฉะนั้น
            printf("%d\n", state_pen.font);
        }
        มิฉะนั้น ถ้า(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ถ้า(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          มิฉะนั้น
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        มิฉะนั้น ถ้า(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ถ้า(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          มิฉะนั้น
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        มิฉะนั้น ถ้า(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        มิฉะนั้น ถ้า(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        มิฉะนั้น
          printf("?\n");
      }
      มิฉะนั้น ถ้า(strstartswith(line, "?lineinfo ")) {
        assert(state);
        var_na *linep = line + 10;
        จำนวนเต็ม row;
        const VTermLineInfo *info;
        ขณะ(linep[0] == ' ')
          linep++;
        ถ้า(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        ถ้า(info->doublewidth)
          printf("dwl ");
        ถ้า(info->doubleheight)
          printf("dhl ");
        ถ้า(info->continuation)
          printf("cont ");
        printf("\n");
      }
      มิฉะนั้น ถ้า(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        var_na *linep = line + 13;
        VTermRect rect;
        size_t len;
        ขณะ(linep[0] == ' ')
          linep++;
        ถ้า(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        มิฉะนั้น ถ้า(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        มิฉะนั้น {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        ถ้า(len == (size_t)-1)
          printf("! screen_chars error\n");
        มิฉะนั้น ถ้า(len == 0)
          printf("\n");
        มิฉะนั้น {
          uint32_t *chars = malloc(parimaa_na(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          สำหรับ(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      มิฉะนั้น ถ้า(strstartswith(line, "?screen_text ")) {
        assert(screen);
        var_na *linep = line + 12;
        VTermRect rect;
        size_t len;
        ขณะ(linep[0] == ' ')
          linep++;
        ถ้า(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        ถ้า(len == (size_t)-1)
          printf("! screen_text error\n");
        มิฉะนั้น ถ้า(len == 0)
          printf("\n");
        มิฉะนั้น {
          // Put an overwrite guard at both ends of the buffer
          unsigned var_na *buffer = malloc(len + 4);
          unsigned var_na *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (var_na *)text, len, rect);

          ถ้า(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          มิฉะนั้น ถ้า(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          มิฉะนั้น
	  {
	    size_t i;
            สำหรับ(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      มิฉะนั้น ถ้า(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        var_na *linep = line + 12;
	จำนวนเต็ม i;
        VTermPos pos;
        VTermScreenCell cell;
        ขณะ(linep[0] == ' ')
          linep++;
        ถ้า(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        ถ้า(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        สำหรับ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        ถ้า(cell.attrs.bold)      printf("B");
        ถ้า(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        ถ้า(cell.attrs.italic)    printf("I");
        ถ้า(cell.attrs.blink)     printf("K");
        ถ้า(cell.attrs.reverse)   printf("R");
        ถ้า(cell.attrs.font)      printf("F%d", cell.attrs.font);
        ถ้า(cell.attrs.small)     printf("S");
        ถ้า(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        ถ้า(cell.attrs.dwl)       printf("dwl ");
        ถ้า(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      มิฉะนั้น ถ้า(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        var_na *linep = line + 12;
        ขณะ(linep[0] == ' ')
          linep++;
        ถ้า(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      มิฉะนั้น ถ้า(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        var_na *linep = line + 21;
        ขณะ(linep[0] == ' ')
          linep++;
        ถ้า(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        ถ้า(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      มิฉะนั้น
        printf("?\n");

      memset(line, 0, parimaa_na line);
      continue;
    }

    มิฉะนั้น
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    ถ้า(outlen > 0) {
      var_na outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  คืน 0;
}
