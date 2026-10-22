#include "vterm.h"
#include "../src/vterm_internal.h" // We pull in some internal bits too

#include <assert.h>
#include <stdio.h>
#include <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

static size_t inplace_hex2bytes(char *s)
{
  char *inpos = s, *outpos = s;

  trong_khi(*inpos) {
    unsigned số_nguyên ch;
    nếu(sscanf(inpos, "%2x", &ch) < 1)
      break;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  trả_về outpos - s;
}

static VTermModifier strpe_modifiers(char **strp)
{
  VTermModifier state = 0;

  trong_khi((*strp)[0]) {
    switch(((*strp)++)[0]) {
      case 'S': state |= VTERM_MOD_SHIFT; break;
      case 'C': state |= VTERM_MOD_CTRL;  break;
      case 'A': state |= VTERM_MOD_ALT;   break;
      default: trả_về state;
    }
  }

  trả_về state;
}

static VTermKey strp_key(char *str)
{
  static struct {
    char *name;
    VTermKey key;
  } keys[] = {
    { "Up",    VTERM_KEY_UP },
    { "Tab",   VTERM_KEY_TAB },
    { "Enter", VTERM_KEY_ENTER },
    { "KP0",   VTERM_KEY_KP_0 },
    { "F1",    VTERM_KEY_FUNCTION(1) },
    { NULL,    VTERM_KEY_NONE },
  };
  số_nguyên i;

  cho(i = 0; keys[i].name; i++) {
    nếu(streq(str, keys[i].name))
      trả_về keys[i].key;
  }

  trả_về VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  nếu (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  không_thì nếu (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  không_thì {
    printf("invalid(%d", col->type);
  }
  nếu (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  nếu (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(char **strp)
{
  uint8_t r, g, b, idx;
  số_nguyên len = 0;
  VTermColor col;

  nếu(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  không_thì nếu(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  không_thì
    vterm_color_rgb(&col, 127, 127, 127);

  trả_về col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const char *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  cho(i = 0; i < len; i++)
    printf("%x%s", (unsigned char)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const char *s, size_t len)
{
  trong_khi(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static số_nguyên parser_text(const char bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  cho(i = 0; i < len; i++) {
    unsigned char b = bytes[i];
    nếu(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      break;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  trả_về i;
}

static số_nguyên parser_control(unsigned char control, void *user UNUSED)
{
  printf("control %02x\n", control);

  trả_về 1;
}

static số_nguyên parser_escape(const char bytes[], size_t len, void *user UNUSED)
{
  nếu(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    nếu(len < 2)
      trả_về -1;
    len = 2;
  }
  không_thì {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  trả_về len;
}

static số_nguyên parser_csi(const char *leader, const long args[], số_nguyên argcount, const char *intermed, char command, void *user UNUSED)
{
  số_nguyên i;
  printf("csi %02x", command);

  nếu(leader && leader[0]) {
    printf(" L=");
    cho(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  cho(i = 0; i < argcount; i++) {
    char sep = i ? ',' : ' ';

    nếu(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    không_thì
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  nếu(intermed && intermed[0]) {
    printf(" I=");
    cho(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  trả_về 1;
}

static số_nguyên parser_osc(số_nguyên command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  nếu(frag.initial) {
    nếu(command == -1)
      printf("[");
    không_thì
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  nếu(frag.final)
    printf("]");

  printf("\n");

  trả_về 1;
}

static số_nguyên parser_dcs(const char *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  nếu(frag.initial) {
    size_t i;
    printf("[");
    cho(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  nếu(frag.final)
    printf("]");

  printf("\n");

  trả_về 1;
}

static số_nguyên parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  nếu(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  nếu(frag.final)
    printf("]");

  printf("\n");

  trả_về 1;
}

static số_nguyên parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  nếu(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  nếu(frag.final)
    printf("]");

  printf("\n");

  trả_về 1;
}

static số_nguyên parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  nếu(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  nếu(frag.final)
    printf("]");

  printf("\n");

  trả_về 1;
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

static số_nguyên want_movecursor = 0;
static VTermPos state_pos;
static số_nguyên movecursor(VTermPos pos, VTermPos oldpos UNUSED, số_nguyên visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  nếu(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  trả_về 1;
}

static số_nguyên want_premove = 0;
static số_nguyên premove(VTermRect rect, void *user UNUSED)
{
  nếu(!want_premove)
    trả_về 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  trả_về 1;
}

static số_nguyên want_scrollrect = 0;
static số_nguyên scrollrect(VTermRect rect, số_nguyên downward, số_nguyên rightward, void *user UNUSED)
{
  nếu(!want_scrollrect)
    trả_về 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  trả_về 1;
}

static số_nguyên want_moverect = 0;
static số_nguyên moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  nếu(!want_moverect)
    trả_về 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  trả_về 1;
}

static số_nguyên want_settermprop = 0;
static số_nguyên settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  nếu(!want_settermprop)
    trả_về 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  case VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    trả_về 1;
  case VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    trả_về 1;
  case VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    trả_về 1;
  case VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    trả_về 1;

  case VTERM_N_VALUETYPES:
    trả_về 0;
  }

  trả_về 0;
}

// These callbacks are for State

static số_nguyên want_state_putglyph = 0;
static số_nguyên state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  số_nguyên i;
  nếu(!want_state_putglyph)
    trả_về 1;

  printf("putglyph ");
  cho(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  nếu(info->protected_cell)
    printf(" prot");
  nếu(info->dwl)
    printf(" dwl");
  nếu(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  trả_về 1;
}

static số_nguyên want_state_erase = 0;
static số_nguyên state_erase(VTermRect rect, số_nguyên selective, void *user UNUSED)
{
  nếu(!want_state_erase)
    trả_về 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  trả_về 1;
}

static struct {
  số_nguyên bold;
  số_nguyên underline;
  số_nguyên italic;
  số_nguyên blink;
  số_nguyên reverse;
  số_nguyên conceal;
  số_nguyên strike;
  số_nguyên font;
  số_nguyên small;
  số_nguyên baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static số_nguyên state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
{
  switch(attr) {
  case VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    break;
  case VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->number;
    break;
  case VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    break;
  case VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    break;
  case VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    break;
  case VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    break;
  case VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    break;
  case VTERM_ATTR_FONT:
    state_pen.font = val->number;
    break;
  case VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    break;
  case VTERM_ATTR_BASELINE:
    state_pen.baseline = val->number;
    break;
  case VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    break;
  case VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    break;

  case VTERM_N_ATTRS:
    trả_về 0;
  }

  trả_về 1;
}

static số_nguyên state_setlineinfo(số_nguyên row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  trả_về 1;
}

static số_nguyên want_state_scrollback = 0;
static số_nguyên state_sb_clear(void *user UNUSED) {
  nếu(!want_state_scrollback)
    trả_về 1;

  printf("sb_clear\n");
  trả_về 0;
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

static số_nguyên selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  nếu(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  nếu(frag.final)
    printf("]");
  printf("\n");

  trả_về 1;
}

static số_nguyên selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  trả_về 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static số_nguyên want_screen_damage = 0;
static số_nguyên want_screen_damage_cells = 0;
static số_nguyên screen_damage(VTermRect rect, void *user UNUSED)
{
  nếu(!want_screen_damage)
    trả_về 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  nếu(want_screen_damage_cells) {
    số_nguyên equals = FALSE;
    số_nguyên row;
    số_nguyên col;

    cho(row = rect.start_row; row < rect.end_row; row++) {
      số_nguyên eol = rect.end_col;
      trong_khi(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        nếu(cell.chars[0])
          break;

        eol--;
      }

      nếu(eol == rect.start_col)
        break;

      nếu(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      cho(col = rect.start_col; col < eol; col++) {
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

  trả_về 1;
}

static số_nguyên want_screen_scrollback = 0;
static số_nguyên screen_sb_pushline4(số_nguyên cols, const VTermScreenCell *cells, số_nguyên continuation, void *user UNUSED)
{
  số_nguyên eol;
  số_nguyên c;

  nếu(!want_screen_scrollback)
    trả_về 1;

  eol = cols;
  trong_khi(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  cho(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  trả_về 1;
}

static số_nguyên screen_sb_popline(số_nguyên cols, VTermScreenCell *cells, void *user UNUSED)
{
  số_nguyên col;

  nếu(!want_screen_scrollback)
    trả_về 0;

  // All lines of scrollback contain "ABCDE"
  cho(col = 0; col < cols; col++) {
    nếu(col < 5)
      cells[col].chars[0] = 'A' + col;
    không_thì
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  trả_về 1;
}

static số_nguyên screen_sb_clear(void *user UNUSED)
{
  nếu(!want_screen_scrollback)
    trả_về 1;

  printf("sb_clear\n");
  trả_về 0;
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

số_nguyên main(số_nguyên argc UNUSED, char **argv UNUSED)
{
  char line[1024] = {0};
  số_nguyên flag;

  số_nguyên err;

  setvbuf(stdout, NULL, _IONBF, 0);

  trong_khi(fgets(line, sizeof line, stdin)) {
    char *nl;
    size_t outlen;
    err = 0;

    nếu((nl = strchr(line, '\n')))
      *nl = '\0';

    nếu(streq(line, "INIT")) {
      nếu(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    không_thì nếu(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    không_thì nếu(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      số_nguyên i = 9;
      số_nguyên sense = 1;
      assert(vt);
      nếu(!state) {
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

      trong_khi(line[i] == ' ')
        i++;
      cho( ; line[i]; i++)
        switch(line[i]) {
        case '+':
          sense = 1;
          break;
        case '-':
          sense = 0;
          break;
        case 'g':
          want_state_putglyph = sense;
          break;
        case 's':
          want_scrollrect = sense;
          break;
        case 'P':
          want_premove = sense;
          break;
        case 'm':
          want_moverect = sense;
          break;
        case 'e':
          want_state_erase = sense;
          break;
        case 'p':
          want_settermprop = sense;
          break;
        case 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : NULL, NULL);
          break;
        case 'b':
          want_state_scrollback = sense;
          break;
        default:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", line[i]);
        }
    }

    không_thì nếu(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      số_nguyên i = 10;
      số_nguyên sense = 1;
      assert(vt);
      nếu(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      trong_khi(line[i] == ' ')
        i++;
      cho( ; line[i]; i++)
        switch(line[i]) {
        case '-':
          sense = 0;
          break;
        case 'a':
          vterm_screen_enable_altscreen(screen, 1);
          break;
        case 'd':
          want_screen_damage = sense;
          break;
        case 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          break;
        case 'm':
          want_moverect = sense;
          break;
        case 'c':
          want_movecursor = sense;
          break;
        case 'p':
          want_settermprop = 1;
          break;
        case 'b':
          want_screen_scrollback = sense;
          break;
        case 'r':
          vterm_screen_enable_reflow(screen, sense);
          break;
        default:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", line[i]);
        }
    }

    không_thì nếu(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    không_thì nếu(streq(line, "RESET")) {
      nếu(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      nếu(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    không_thì nếu(strstartswith(line, "RESIZE ")) {
      số_nguyên rows, cols;
      char *linep = line + 7;
      trong_khi(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    không_thì nếu(strstartswith(line, "PUSH ")) {
      char *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      nếu(written < len)
        fprintf(stderr, "! short write\n");
    }

    không_thì nếu(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      nếu(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    không_thì nếu(strstartswith(line, "ENCIN ")) {
      char *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      số_nguyên cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      nếu(cpi > 0) {
	số_nguyên i;
        printf("encout ");
        cho(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    không_thì nếu(strstartswith(line, "INCHAR ")) {
      char *linep = line + 7;
      unsigned số_nguyên c = 0;
      VTermModifier mod;
      trong_khi(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    không_thì nếu(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      char *linep = line + 6;
      trong_khi(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      trong_khi(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    không_thì nếu(strstartswith(line, "PASTE ")) {
      char *linep = line + 6;
      nếu(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      không_thì nếu(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      không_thì
        goto abort_line;
    }

    không_thì nếu(strstartswith(line, "FOCUS ")) {
      assert(state);
      char *linep = line + 6;
      nếu(streq(linep, "IN"))
        vterm_state_focus_in(state);
      không_thì nếu(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      không_thì
        goto abort_line;
    }

    không_thì nếu(strstartswith(line, "MOUSEMOVE ")) {
      char *linep = line + 10;
      số_nguyên row, col, len;
      VTermModifier mod;
      trong_khi(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      trong_khi(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    không_thì nếu(strstartswith(line, "MOUSEBTN ")) {
      char *linep = line + 9;
      char press;
      số_nguyên button, len;
      VTermModifier mod;
      trong_khi(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      trong_khi(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    không_thì nếu(strstartswith(line, "SELECTION ")) {
      char *linep = line + 10;
      unsigned số_nguyên mask;
      số_nguyên len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      trong_khi(linep[0] == ' ')
        linep++;
      nếu(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        trong_khi(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      trong_khi(linep[0] == ' ')
        linep++;
      nếu(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    không_thì nếu(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      char *linep = line + 12;
      trong_khi(linep[0] == ' ')
        linep++;
      nếu(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      không_thì nếu(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      không_thì nếu(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      không_thì nếu(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    không_thì nếu(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    không_thì nếu(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      char *linep = line + 14;
      trong_khi(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      nếu(linep[0]) {
        trong_khi(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      không_thì
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    không_thì nếu(line[0] == '?') {
      nếu(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        nếu(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        không_thì nếu(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        không_thì
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      không_thì nếu(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        char *linep = line + 5;
        trong_khi(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        nếu(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          nếu(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          không_thì
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        không_thì nếu(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          nếu(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          không_thì
            printf("%d\n", state_pen.underline);
        }
        không_thì nếu(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          nếu(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          không_thì
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        không_thì nếu(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          nếu(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          không_thì
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        không_thì nếu(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          nếu(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          không_thì
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        không_thì nếu(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          nếu(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          không_thì
            printf("%d\n", state_pen.font);
        }
        không_thì nếu(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          nếu(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          không_thì
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        không_thì nếu(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          nếu(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          không_thì
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        không_thì nếu(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        không_thì nếu(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        không_thì
          printf("?\n");
      }
      không_thì nếu(strstartswith(line, "?lineinfo ")) {
        assert(state);
        char *linep = line + 10;
        số_nguyên row;
        const VTermLineInfo *info;
        trong_khi(linep[0] == ' ')
          linep++;
        nếu(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        nếu(info->doublewidth)
          printf("dwl ");
        nếu(info->doubleheight)
          printf("dhl ");
        nếu(info->continuation)
          printf("cont ");
        printf("\n");
      }
      không_thì nếu(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        char *linep = line + 13;
        VTermRect rect;
        size_t len;
        trong_khi(linep[0] == ' ')
          linep++;
        nếu(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        không_thì nếu(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        không_thì {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        nếu(len == (size_t)-1)
          printf("! screen_chars error\n");
        không_thì nếu(len == 0)
          printf("\n");
        không_thì {
          uint32_t *chars = malloc(sizeof(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          cho(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      không_thì nếu(strstartswith(line, "?screen_text ")) {
        assert(screen);
        char *linep = line + 12;
        VTermRect rect;
        size_t len;
        trong_khi(linep[0] == ' ')
          linep++;
        nếu(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        nếu(len == (size_t)-1)
          printf("! screen_text error\n");
        không_thì nếu(len == 0)
          printf("\n");
        không_thì {
          // Put an overwrite guard at both ends of the buffer
          unsigned char *buffer = malloc(len + 4);
          unsigned char *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (char *)text, len, rect);

          nếu(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          không_thì nếu(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          không_thì
	  {
	    size_t i;
            cho(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      không_thì nếu(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        char *linep = line + 12;
	số_nguyên i;
        VTermPos pos;
        VTermScreenCell cell;
        trong_khi(linep[0] == ' ')
          linep++;
        nếu(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        nếu(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        cho(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        nếu(cell.attrs.bold)      printf("B");
        nếu(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        nếu(cell.attrs.italic)    printf("I");
        nếu(cell.attrs.blink)     printf("K");
        nếu(cell.attrs.reverse)   printf("R");
        nếu(cell.attrs.font)      printf("F%d", cell.attrs.font);
        nếu(cell.attrs.small)     printf("S");
        nếu(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        nếu(cell.attrs.dwl)       printf("dwl ");
        nếu(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      không_thì nếu(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        char *linep = line + 12;
        trong_khi(linep[0] == ' ')
          linep++;
        nếu(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      không_thì nếu(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        char *linep = line + 21;
        trong_khi(linep[0] == ' ')
          linep++;
        nếu(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        nếu(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      không_thì
        printf("?\n");

      memset(line, 0, sizeof line);
      continue;
    }

    không_thì
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    nếu(outlen > 0) {
      char outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  trả_về 0;
}
