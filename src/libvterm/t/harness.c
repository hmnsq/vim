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

  동안(*inpos) {
    unsigned 정수 ch;
    만약(sscanf(inpos, "%2x", &ch) < 1)
      break;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  반환 outpos - s;
}

static VTermModifier strpe_modifiers(char **strp)
{
  VTermModifier state = 0;

  동안((*strp)[0]) {
    switch(((*strp)++)[0]) {
      case 'S': state |= VTERM_MOD_SHIFT; break;
      case 'C': state |= VTERM_MOD_CTRL;  break;
      case 'A': state |= VTERM_MOD_ALT;   break;
      default: 반환 state;
    }
  }

  반환 state;
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
  정수 i;

  위해(i = 0; keys[i].name; i++) {
    만약(streq(str, keys[i].name))
      반환 keys[i].key;
  }

  반환 VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  만약 (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  아니면 만약 (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  아니면 {
    printf("invalid(%d", col->type);
  }
  만약 (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  만약 (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(char **strp)
{
  uint8_t r, g, b, idx;
  정수 len = 0;
  VTermColor col;

  만약(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  아니면 만약(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  아니면
    vterm_color_rgb(&col, 127, 127, 127);

  반환 col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const char *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  위해(i = 0; i < len; i++)
    printf("%x%s", (unsigned char)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const char *s, size_t len)
{
  동안(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static 정수 parser_text(const char bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  위해(i = 0; i < len; i++) {
    unsigned char b = bytes[i];
    만약(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      break;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  반환 i;
}

static 정수 parser_control(unsigned char control, void *user UNUSED)
{
  printf("control %02x\n", control);

  반환 1;
}

static 정수 parser_escape(const char bytes[], size_t len, void *user UNUSED)
{
  만약(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    만약(len < 2)
      반환 -1;
    len = 2;
  }
  아니면 {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  반환 len;
}

static 정수 parser_csi(const char *leader, const long args[], 정수 argcount, const char *intermed, char command, void *user UNUSED)
{
  정수 i;
  printf("csi %02x", command);

  만약(leader && leader[0]) {
    printf(" L=");
    위해(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  위해(i = 0; i < argcount; i++) {
    char sep = i ? ',' : ' ';

    만약(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    아니면
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  만약(intermed && intermed[0]) {
    printf(" I=");
    위해(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  반환 1;
}

static 정수 parser_osc(정수 command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  만약(frag.initial) {
    만약(command == -1)
      printf("[");
    아니면
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  만약(frag.final)
    printf("]");

  printf("\n");

  반환 1;
}

static 정수 parser_dcs(const char *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  만약(frag.initial) {
    size_t i;
    printf("[");
    위해(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  만약(frag.final)
    printf("]");

  printf("\n");

  반환 1;
}

static 정수 parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  만약(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  만약(frag.final)
    printf("]");

  printf("\n");

  반환 1;
}

static 정수 parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  만약(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  만약(frag.final)
    printf("]");

  printf("\n");

  반환 1;
}

static 정수 parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  만약(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  만약(frag.final)
    printf("]");

  printf("\n");

  반환 1;
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

static 정수 want_movecursor = 0;
static VTermPos state_pos;
static 정수 movecursor(VTermPos pos, VTermPos oldpos UNUSED, 정수 visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  만약(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  반환 1;
}

static 정수 want_premove = 0;
static 정수 premove(VTermRect rect, void *user UNUSED)
{
  만약(!want_premove)
    반환 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  반환 1;
}

static 정수 want_scrollrect = 0;
static 정수 scrollrect(VTermRect rect, 정수 downward, 정수 rightward, void *user UNUSED)
{
  만약(!want_scrollrect)
    반환 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  반환 1;
}

static 정수 want_moverect = 0;
static 정수 moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  만약(!want_moverect)
    반환 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  반환 1;
}

static 정수 want_settermprop = 0;
static 정수 settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  만약(!want_settermprop)
    반환 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  case VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    반환 1;
  case VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    반환 1;
  case VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    반환 1;
  case VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    반환 1;

  case VTERM_N_VALUETYPES:
    반환 0;
  }

  반환 0;
}

// These callbacks are for State

static 정수 want_state_putglyph = 0;
static 정수 state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  정수 i;
  만약(!want_state_putglyph)
    반환 1;

  printf("putglyph ");
  위해(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  만약(info->protected_cell)
    printf(" prot");
  만약(info->dwl)
    printf(" dwl");
  만약(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  반환 1;
}

static 정수 want_state_erase = 0;
static 정수 state_erase(VTermRect rect, 정수 selective, void *user UNUSED)
{
  만약(!want_state_erase)
    반환 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  반환 1;
}

static struct {
  정수 bold;
  정수 underline;
  정수 italic;
  정수 blink;
  정수 reverse;
  정수 conceal;
  정수 strike;
  정수 font;
  정수 small;
  정수 baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static 정수 state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    반환 0;
  }

  반환 1;
}

static 정수 state_setlineinfo(정수 row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  반환 1;
}

static 정수 want_state_scrollback = 0;
static 정수 state_sb_clear(void *user UNUSED) {
  만약(!want_state_scrollback)
    반환 1;

  printf("sb_clear\n");
  반환 0;
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

static 정수 selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  만약(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  만약(frag.final)
    printf("]");
  printf("\n");

  반환 1;
}

static 정수 selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  반환 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static 정수 want_screen_damage = 0;
static 정수 want_screen_damage_cells = 0;
static 정수 screen_damage(VTermRect rect, void *user UNUSED)
{
  만약(!want_screen_damage)
    반환 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  만약(want_screen_damage_cells) {
    정수 equals = FALSE;
    정수 row;
    정수 col;

    위해(row = rect.start_row; row < rect.end_row; row++) {
      정수 eol = rect.end_col;
      동안(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        만약(cell.chars[0])
          break;

        eol--;
      }

      만약(eol == rect.start_col)
        break;

      만약(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      위해(col = rect.start_col; col < eol; col++) {
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

  반환 1;
}

static 정수 want_screen_scrollback = 0;
static 정수 screen_sb_pushline4(정수 cols, const VTermScreenCell *cells, 정수 continuation, void *user UNUSED)
{
  정수 eol;
  정수 c;

  만약(!want_screen_scrollback)
    반환 1;

  eol = cols;
  동안(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  위해(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  반환 1;
}

static 정수 screen_sb_popline(정수 cols, VTermScreenCell *cells, void *user UNUSED)
{
  정수 col;

  만약(!want_screen_scrollback)
    반환 0;

  // All lines of scrollback contain "ABCDE"
  위해(col = 0; col < cols; col++) {
    만약(col < 5)
      cells[col].chars[0] = 'A' + col;
    아니면
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  반환 1;
}

static 정수 screen_sb_clear(void *user UNUSED)
{
  만약(!want_screen_scrollback)
    반환 1;

  printf("sb_clear\n");
  반환 0;
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

정수 main(정수 argc UNUSED, char **argv UNUSED)
{
  char line[1024] = {0};
  정수 flag;

  정수 err;

  setvbuf(stdout, NULL, _IONBF, 0);

  동안(fgets(line, sizeof line, stdin)) {
    char *nl;
    size_t outlen;
    err = 0;

    만약((nl = strchr(line, '\n')))
      *nl = '\0';

    만약(streq(line, "INIT")) {
      만약(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    아니면 만약(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    아니면 만약(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      정수 i = 9;
      정수 sense = 1;
      assert(vt);
      만약(!state) {
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

      동안(line[i] == ' ')
        i++;
      위해( ; line[i]; i++)
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

    아니면 만약(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      정수 i = 10;
      정수 sense = 1;
      assert(vt);
      만약(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      동안(line[i] == ' ')
        i++;
      위해( ; line[i]; i++)
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

    아니면 만약(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    아니면 만약(streq(line, "RESET")) {
      만약(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      만약(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    아니면 만약(strstartswith(line, "RESIZE ")) {
      정수 rows, cols;
      char *linep = line + 7;
      동안(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    아니면 만약(strstartswith(line, "PUSH ")) {
      char *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      만약(written < len)
        fprintf(stderr, "! short write\n");
    }

    아니면 만약(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      만약(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    아니면 만약(strstartswith(line, "ENCIN ")) {
      char *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      정수 cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      만약(cpi > 0) {
	정수 i;
        printf("encout ");
        위해(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    아니면 만약(strstartswith(line, "INCHAR ")) {
      char *linep = line + 7;
      unsigned 정수 c = 0;
      VTermModifier mod;
      동안(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    아니면 만약(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      char *linep = line + 6;
      동안(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      동안(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    아니면 만약(strstartswith(line, "PASTE ")) {
      char *linep = line + 6;
      만약(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      아니면 만약(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      아니면
        goto abort_line;
    }

    아니면 만약(strstartswith(line, "FOCUS ")) {
      assert(state);
      char *linep = line + 6;
      만약(streq(linep, "IN"))
        vterm_state_focus_in(state);
      아니면 만약(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      아니면
        goto abort_line;
    }

    아니면 만약(strstartswith(line, "MOUSEMOVE ")) {
      char *linep = line + 10;
      정수 row, col, len;
      VTermModifier mod;
      동안(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      동안(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    아니면 만약(strstartswith(line, "MOUSEBTN ")) {
      char *linep = line + 9;
      char press;
      정수 button, len;
      VTermModifier mod;
      동안(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      동안(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    아니면 만약(strstartswith(line, "SELECTION ")) {
      char *linep = line + 10;
      unsigned 정수 mask;
      정수 len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      동안(linep[0] == ' ')
        linep++;
      만약(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        동안(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      동안(linep[0] == ' ')
        linep++;
      만약(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    아니면 만약(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      char *linep = line + 12;
      동안(linep[0] == ' ')
        linep++;
      만약(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      아니면 만약(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      아니면 만약(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      아니면 만약(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    아니면 만약(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    아니면 만약(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      char *linep = line + 14;
      동안(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      만약(linep[0]) {
        동안(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      아니면
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    아니면 만약(line[0] == '?') {
      만약(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        만약(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        아니면 만약(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        아니면
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      아니면 만약(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        char *linep = line + 5;
        동안(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        만약(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          만약(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          아니면
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        아니면 만약(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          만약(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          아니면
            printf("%d\n", state_pen.underline);
        }
        아니면 만약(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          만약(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          아니면
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        아니면 만약(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          만약(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          아니면
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        아니면 만약(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          만약(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          아니면
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        아니면 만약(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          만약(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          아니면
            printf("%d\n", state_pen.font);
        }
        아니면 만약(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          만약(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          아니면
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        아니면 만약(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          만약(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          아니면
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        아니면 만약(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        아니면 만약(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        아니면
          printf("?\n");
      }
      아니면 만약(strstartswith(line, "?lineinfo ")) {
        assert(state);
        char *linep = line + 10;
        정수 row;
        const VTermLineInfo *info;
        동안(linep[0] == ' ')
          linep++;
        만약(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        만약(info->doublewidth)
          printf("dwl ");
        만약(info->doubleheight)
          printf("dhl ");
        만약(info->continuation)
          printf("cont ");
        printf("\n");
      }
      아니면 만약(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        char *linep = line + 13;
        VTermRect rect;
        size_t len;
        동안(linep[0] == ' ')
          linep++;
        만약(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        아니면 만약(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        아니면 {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        만약(len == (size_t)-1)
          printf("! screen_chars error\n");
        아니면 만약(len == 0)
          printf("\n");
        아니면 {
          uint32_t *chars = malloc(sizeof(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          위해(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      아니면 만약(strstartswith(line, "?screen_text ")) {
        assert(screen);
        char *linep = line + 12;
        VTermRect rect;
        size_t len;
        동안(linep[0] == ' ')
          linep++;
        만약(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        만약(len == (size_t)-1)
          printf("! screen_text error\n");
        아니면 만약(len == 0)
          printf("\n");
        아니면 {
          // Put an overwrite guard at both ends of the buffer
          unsigned char *buffer = malloc(len + 4);
          unsigned char *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (char *)text, len, rect);

          만약(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          아니면 만약(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          아니면
	  {
	    size_t i;
            위해(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      아니면 만약(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        char *linep = line + 12;
	정수 i;
        VTermPos pos;
        VTermScreenCell cell;
        동안(linep[0] == ' ')
          linep++;
        만약(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        만약(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        위해(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        만약(cell.attrs.bold)      printf("B");
        만약(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        만약(cell.attrs.italic)    printf("I");
        만약(cell.attrs.blink)     printf("K");
        만약(cell.attrs.reverse)   printf("R");
        만약(cell.attrs.font)      printf("F%d", cell.attrs.font);
        만약(cell.attrs.small)     printf("S");
        만약(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        만약(cell.attrs.dwl)       printf("dwl ");
        만약(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      아니면 만약(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        char *linep = line + 12;
        동안(linep[0] == ' ')
          linep++;
        만약(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      아니면 만약(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        char *linep = line + 21;
        동안(linep[0] == ' ')
          linep++;
        만약(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        만약(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      아니면
        printf("?\n");

      memset(line, 0, sizeof line);
      continue;
    }

    아니면
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    만약(outlen > 0) {
      char outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  반환 0;
}
