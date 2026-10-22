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

  当(*inpos) {
    unsigned 整数 ch;
    如果(sscanf(inpos, "%2x", &ch) < 1)
      break;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  返回 outpos - s;
}

static VTermModifier strpe_modifiers(char **strp)
{
  VTermModifier state = 0;

  当((*strp)[0]) {
    switch(((*strp)++)[0]) {
      case 'S': state |= VTERM_MOD_SHIFT; break;
      case 'C': state |= VTERM_MOD_CTRL;  break;
      case 'A': state |= VTERM_MOD_ALT;   break;
      default: 返回 state;
    }
  }

  返回 state;
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
  整数 i;

  为(i = 0; keys[i].name; i++) {
    如果(streq(str, keys[i].name))
      返回 keys[i].key;
  }

  返回 VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  如果 (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  否则 如果 (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  否则 {
    printf("invalid(%d", col->type);
  }
  如果 (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  如果 (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(char **strp)
{
  uint8_t r, g, b, idx;
  整数 len = 0;
  VTermColor col;

  如果(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  否则 如果(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  否则
    vterm_color_rgb(&col, 127, 127, 127);

  返回 col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const char *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  为(i = 0; i < len; i++)
    printf("%x%s", (unsigned char)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const char *s, size_t len)
{
  当(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static 整数 parser_text(const char bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  为(i = 0; i < len; i++) {
    unsigned char b = bytes[i];
    如果(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      break;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  返回 i;
}

static 整数 parser_control(unsigned char control, void *user UNUSED)
{
  printf("control %02x\n", control);

  返回 1;
}

static 整数 parser_escape(const char bytes[], size_t len, void *user UNUSED)
{
  如果(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    如果(len < 2)
      返回 -1;
    len = 2;
  }
  否则 {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  返回 len;
}

static 整数 parser_csi(const char *leader, const long args[], 整数 argcount, const char *intermed, char command, void *user UNUSED)
{
  整数 i;
  printf("csi %02x", command);

  如果(leader && leader[0]) {
    printf(" L=");
    为(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  为(i = 0; i < argcount; i++) {
    char sep = i ? ',' : ' ';

    如果(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    否则
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  如果(intermed && intermed[0]) {
    printf(" I=");
    为(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  返回 1;
}

static 整数 parser_osc(整数 command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  如果(frag.initial) {
    如果(command == -1)
      printf("[");
    否则
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  如果(frag.final)
    printf("]");

  printf("\n");

  返回 1;
}

static 整数 parser_dcs(const char *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  如果(frag.initial) {
    size_t i;
    printf("[");
    为(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  如果(frag.final)
    printf("]");

  printf("\n");

  返回 1;
}

static 整数 parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  如果(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  如果(frag.final)
    printf("]");

  printf("\n");

  返回 1;
}

static 整数 parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  如果(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  如果(frag.final)
    printf("]");

  printf("\n");

  返回 1;
}

static 整数 parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  如果(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  如果(frag.final)
    printf("]");

  printf("\n");

  返回 1;
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

static 整数 want_movecursor = 0;
static VTermPos state_pos;
static 整数 movecursor(VTermPos pos, VTermPos oldpos UNUSED, 整数 visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  如果(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  返回 1;
}

static 整数 want_premove = 0;
static 整数 premove(VTermRect rect, void *user UNUSED)
{
  如果(!want_premove)
    返回 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  返回 1;
}

static 整数 want_scrollrect = 0;
static 整数 scrollrect(VTermRect rect, 整数 downward, 整数 rightward, void *user UNUSED)
{
  如果(!want_scrollrect)
    返回 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  返回 1;
}

static 整数 want_moverect = 0;
static 整数 moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  如果(!want_moverect)
    返回 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  返回 1;
}

static 整数 want_settermprop = 0;
static 整数 settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  如果(!want_settermprop)
    返回 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  case VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    返回 1;
  case VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    返回 1;
  case VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    返回 1;
  case VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    返回 1;

  case VTERM_N_VALUETYPES:
    返回 0;
  }

  返回 0;
}

// These callbacks are for State

static 整数 want_state_putglyph = 0;
static 整数 state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  整数 i;
  如果(!want_state_putglyph)
    返回 1;

  printf("putglyph ");
  为(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  如果(info->protected_cell)
    printf(" prot");
  如果(info->dwl)
    printf(" dwl");
  如果(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  返回 1;
}

static 整数 want_state_erase = 0;
static 整数 state_erase(VTermRect rect, 整数 selective, void *user UNUSED)
{
  如果(!want_state_erase)
    返回 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  返回 1;
}

static struct {
  整数 bold;
  整数 underline;
  整数 italic;
  整数 blink;
  整数 reverse;
  整数 conceal;
  整数 strike;
  整数 font;
  整数 small;
  整数 baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static 整数 state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    返回 0;
  }

  返回 1;
}

static 整数 state_setlineinfo(整数 row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  返回 1;
}

static 整数 want_state_scrollback = 0;
static 整数 state_sb_clear(void *user UNUSED) {
  如果(!want_state_scrollback)
    返回 1;

  printf("sb_clear\n");
  返回 0;
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

static 整数 selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  如果(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  如果(frag.final)
    printf("]");
  printf("\n");

  返回 1;
}

static 整数 selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  返回 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static 整数 want_screen_damage = 0;
static 整数 want_screen_damage_cells = 0;
static 整数 screen_damage(VTermRect rect, void *user UNUSED)
{
  如果(!want_screen_damage)
    返回 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  如果(want_screen_damage_cells) {
    整数 equals = FALSE;
    整数 row;
    整数 col;

    为(row = rect.start_row; row < rect.end_row; row++) {
      整数 eol = rect.end_col;
      当(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        如果(cell.chars[0])
          break;

        eol--;
      }

      如果(eol == rect.start_col)
        break;

      如果(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      为(col = rect.start_col; col < eol; col++) {
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

  返回 1;
}

static 整数 want_screen_scrollback = 0;
static 整数 screen_sb_pushline4(整数 cols, const VTermScreenCell *cells, 整数 continuation, void *user UNUSED)
{
  整数 eol;
  整数 c;

  如果(!want_screen_scrollback)
    返回 1;

  eol = cols;
  当(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  为(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  返回 1;
}

static 整数 screen_sb_popline(整数 cols, VTermScreenCell *cells, void *user UNUSED)
{
  整数 col;

  如果(!want_screen_scrollback)
    返回 0;

  // All lines of scrollback contain "ABCDE"
  为(col = 0; col < cols; col++) {
    如果(col < 5)
      cells[col].chars[0] = 'A' + col;
    否则
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  返回 1;
}

static 整数 screen_sb_clear(void *user UNUSED)
{
  如果(!want_screen_scrollback)
    返回 1;

  printf("sb_clear\n");
  返回 0;
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

整数 主要(整数 argc UNUSED, char **argv UNUSED)
{
  char line[1024] = {0};
  整数 flag;

  整数 err;

  setvbuf(stdout, NULL, _IONBF, 0);

  当(fgets(line, sizeof line, stdin)) {
    char *nl;
    size_t outlen;
    err = 0;

    如果((nl = strchr(line, '\n')))
      *nl = '\0';

    如果(streq(line, "INIT")) {
      如果(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    否则 如果(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    否则 如果(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      整数 i = 9;
      整数 sense = 1;
      assert(vt);
      如果(!state) {
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

      当(line[i] == ' ')
        i++;
      为( ; line[i]; i++)
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

    否则 如果(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      整数 i = 10;
      整数 sense = 1;
      assert(vt);
      如果(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      当(line[i] == ' ')
        i++;
      为( ; line[i]; i++)
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

    否则 如果(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    否则 如果(streq(line, "RESET")) {
      如果(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      如果(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    否则 如果(strstartswith(line, "RESIZE ")) {
      整数 rows, cols;
      char *linep = line + 7;
      当(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    否则 如果(strstartswith(line, "PUSH ")) {
      char *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      如果(written < len)
        fprintf(stderr, "! short write\n");
    }

    否则 如果(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      如果(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    否则 如果(strstartswith(line, "ENCIN ")) {
      char *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      整数 cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      如果(cpi > 0) {
	整数 i;
        printf("encout ");
        为(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    否则 如果(strstartswith(line, "INCHAR ")) {
      char *linep = line + 7;
      unsigned 整数 c = 0;
      VTermModifier mod;
      当(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    否则 如果(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      char *linep = line + 6;
      当(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      当(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    否则 如果(strstartswith(line, "PASTE ")) {
      char *linep = line + 6;
      如果(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      否则 如果(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      否则
        goto abort_line;
    }

    否则 如果(strstartswith(line, "FOCUS ")) {
      assert(state);
      char *linep = line + 6;
      如果(streq(linep, "IN"))
        vterm_state_focus_in(state);
      否则 如果(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      否则
        goto abort_line;
    }

    否则 如果(strstartswith(line, "MOUSEMOVE ")) {
      char *linep = line + 10;
      整数 row, col, len;
      VTermModifier mod;
      当(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      当(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    否则 如果(strstartswith(line, "MOUSEBTN ")) {
      char *linep = line + 9;
      char press;
      整数 button, len;
      VTermModifier mod;
      当(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      当(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    否则 如果(strstartswith(line, "SELECTION ")) {
      char *linep = line + 10;
      unsigned 整数 mask;
      整数 len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      当(linep[0] == ' ')
        linep++;
      如果(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        当(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      当(linep[0] == ' ')
        linep++;
      如果(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    否则 如果(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      char *linep = line + 12;
      当(linep[0] == ' ')
        linep++;
      如果(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      否则 如果(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      否则 如果(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      否则 如果(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    否则 如果(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    否则 如果(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      char *linep = line + 14;
      当(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      如果(linep[0]) {
        当(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      否则
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    否则 如果(line[0] == '?') {
      如果(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        如果(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        否则 如果(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        否则
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      否则 如果(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        char *linep = line + 5;
        当(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        如果(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          如果(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          否则
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        否则 如果(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          如果(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          否则
            printf("%d\n", state_pen.underline);
        }
        否则 如果(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          如果(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          否则
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        否则 如果(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          如果(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          否则
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        否则 如果(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          如果(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          否则
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        否则 如果(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          如果(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          否则
            printf("%d\n", state_pen.font);
        }
        否则 如果(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          如果(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          否则
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        否则 如果(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          如果(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          否则
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        否则 如果(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        否则 如果(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        否则
          printf("?\n");
      }
      否则 如果(strstartswith(line, "?lineinfo ")) {
        assert(state);
        char *linep = line + 10;
        整数 row;
        const VTermLineInfo *info;
        当(linep[0] == ' ')
          linep++;
        如果(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        如果(info->doublewidth)
          printf("dwl ");
        如果(info->doubleheight)
          printf("dhl ");
        如果(info->continuation)
          printf("cont ");
        printf("\n");
      }
      否则 如果(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        char *linep = line + 13;
        VTermRect rect;
        size_t len;
        当(linep[0] == ' ')
          linep++;
        如果(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        否则 如果(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        否则 {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        如果(len == (size_t)-1)
          printf("! screen_chars error\n");
        否则 如果(len == 0)
          printf("\n");
        否则 {
          uint32_t *chars = malloc(sizeof(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          为(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      否则 如果(strstartswith(line, "?screen_text ")) {
        assert(screen);
        char *linep = line + 12;
        VTermRect rect;
        size_t len;
        当(linep[0] == ' ')
          linep++;
        如果(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        如果(len == (size_t)-1)
          printf("! screen_text error\n");
        否则 如果(len == 0)
          printf("\n");
        否则 {
          // Put an overwrite guard at both ends of the buffer
          unsigned char *buffer = malloc(len + 4);
          unsigned char *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (char *)text, len, rect);

          如果(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          否则 如果(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          否则
	  {
	    size_t i;
            为(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      否则 如果(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        char *linep = line + 12;
	整数 i;
        VTermPos pos;
        VTermScreenCell cell;
        当(linep[0] == ' ')
          linep++;
        如果(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        如果(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        为(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        如果(cell.attrs.bold)      printf("B");
        如果(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        如果(cell.attrs.italic)    printf("I");
        如果(cell.attrs.blink)     printf("K");
        如果(cell.attrs.reverse)   printf("R");
        如果(cell.attrs.font)      printf("F%d", cell.attrs.font);
        如果(cell.attrs.small)     printf("S");
        如果(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        如果(cell.attrs.dwl)       printf("dwl ");
        如果(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      否则 如果(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        char *linep = line + 12;
        当(linep[0] == ' ')
          linep++;
        如果(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      否则 如果(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        char *linep = line + 21;
        当(linep[0] == ' ')
          linep++;
        如果(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        如果(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      否则
        printf("?\n");

      memset(line, 0, sizeof line);
      continue;
    }

    否则
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    如果(outlen > 0) {
      char outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  返回 0;
}
