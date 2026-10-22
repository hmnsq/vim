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

  поки(*inpos) {
    unsigned ціле ch;
    якщо(sscanf(inpos, "%2x", &ch) < 1)
      break;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  повернути outpos - s;
}

static VTermModifier strpe_modifiers(char **strp)
{
  VTermModifier state = 0;

  поки((*strp)[0]) {
    switch(((*strp)++)[0]) {
      case 'S': state |= VTERM_MOD_SHIFT; break;
      case 'C': state |= VTERM_MOD_CTRL;  break;
      case 'A': state |= VTERM_MOD_ALT;   break;
      default: повернути state;
    }
  }

  повернути state;
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
  ціле i;

  для(i = 0; keys[i].name; i++) {
    якщо(streq(str, keys[i].name))
      повернути keys[i].key;
  }

  повернути VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  якщо (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  інакше якщо (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  інакше {
    printf("invalid(%d", col->type);
  }
  якщо (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  якщо (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(char **strp)
{
  uint8_t r, g, b, idx;
  ціле len = 0;
  VTermColor col;

  якщо(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  інакше якщо(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  інакше
    vterm_color_rgb(&col, 127, 127, 127);

  повернути col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const char *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  для(i = 0; i < len; i++)
    printf("%x%s", (unsigned char)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const char *s, size_t len)
{
  поки(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static ціле parser_text(const char bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  для(i = 0; i < len; i++) {
    unsigned char b = bytes[i];
    якщо(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      break;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  повернути i;
}

static ціле parser_control(unsigned char control, void *user UNUSED)
{
  printf("control %02x\n", control);

  повернути 1;
}

static ціле parser_escape(const char bytes[], size_t len, void *user UNUSED)
{
  якщо(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    якщо(len < 2)
      повернути -1;
    len = 2;
  }
  інакше {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  повернути len;
}

static ціле parser_csi(const char *leader, const long args[], ціле argcount, const char *intermed, char command, void *user UNUSED)
{
  ціле i;
  printf("csi %02x", command);

  якщо(leader && leader[0]) {
    printf(" L=");
    для(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  для(i = 0; i < argcount; i++) {
    char sep = i ? ',' : ' ';

    якщо(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    інакше
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  якщо(intermed && intermed[0]) {
    printf(" I=");
    для(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  повернути 1;
}

static ціле parser_osc(ціле command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  якщо(frag.initial) {
    якщо(command == -1)
      printf("[");
    інакше
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  якщо(frag.final)
    printf("]");

  printf("\n");

  повернути 1;
}

static ціле parser_dcs(const char *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  якщо(frag.initial) {
    size_t i;
    printf("[");
    для(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  якщо(frag.final)
    printf("]");

  printf("\n");

  повернути 1;
}

static ціле parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  якщо(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  якщо(frag.final)
    printf("]");

  printf("\n");

  повернути 1;
}

static ціле parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  якщо(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  якщо(frag.final)
    printf("]");

  printf("\n");

  повернути 1;
}

static ціле parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  якщо(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  якщо(frag.final)
    printf("]");

  printf("\n");

  повернути 1;
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

static ціле want_movecursor = 0;
static VTermPos state_pos;
static ціле movecursor(VTermPos pos, VTermPos oldpos UNUSED, ціле visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  якщо(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  повернути 1;
}

static ціле want_premove = 0;
static ціле premove(VTermRect rect, void *user UNUSED)
{
  якщо(!want_premove)
    повернути 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  повернути 1;
}

static ціле want_scrollrect = 0;
static ціле scrollrect(VTermRect rect, ціле downward, ціле rightward, void *user UNUSED)
{
  якщо(!want_scrollrect)
    повернути 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  повернути 1;
}

static ціле want_moverect = 0;
static ціле moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  якщо(!want_moverect)
    повернути 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  повернути 1;
}

static ціле want_settermprop = 0;
static ціле settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  якщо(!want_settermprop)
    повернути 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  case VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    повернути 1;
  case VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    повернути 1;
  case VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    повернути 1;
  case VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    повернути 1;

  case VTERM_N_VALUETYPES:
    повернути 0;
  }

  повернути 0;
}

// These callbacks are for State

static ціле want_state_putglyph = 0;
static ціле state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  ціле i;
  якщо(!want_state_putglyph)
    повернути 1;

  printf("putglyph ");
  для(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  якщо(info->protected_cell)
    printf(" prot");
  якщо(info->dwl)
    printf(" dwl");
  якщо(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  повернути 1;
}

static ціле want_state_erase = 0;
static ціле state_erase(VTermRect rect, ціле selective, void *user UNUSED)
{
  якщо(!want_state_erase)
    повернути 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  повернути 1;
}

static struct {
  ціле bold;
  ціле underline;
  ціле italic;
  ціле blink;
  ціле reverse;
  ціле conceal;
  ціле strike;
  ціле font;
  ціле small;
  ціле baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static ціле state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    повернути 0;
  }

  повернути 1;
}

static ціле state_setlineinfo(ціле row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  повернути 1;
}

static ціле want_state_scrollback = 0;
static ціле state_sb_clear(void *user UNUSED) {
  якщо(!want_state_scrollback)
    повернути 1;

  printf("sb_clear\n");
  повернути 0;
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

static ціле selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  якщо(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  якщо(frag.final)
    printf("]");
  printf("\n");

  повернути 1;
}

static ціле selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  повернути 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static ціле want_screen_damage = 0;
static ціле want_screen_damage_cells = 0;
static ціле screen_damage(VTermRect rect, void *user UNUSED)
{
  якщо(!want_screen_damage)
    повернути 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  якщо(want_screen_damage_cells) {
    ціле equals = FALSE;
    ціле row;
    ціле col;

    для(row = rect.start_row; row < rect.end_row; row++) {
      ціле eol = rect.end_col;
      поки(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        якщо(cell.chars[0])
          break;

        eol--;
      }

      якщо(eol == rect.start_col)
        break;

      якщо(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      для(col = rect.start_col; col < eol; col++) {
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

  повернути 1;
}

static ціле want_screen_scrollback = 0;
static ціле screen_sb_pushline4(ціле cols, const VTermScreenCell *cells, ціле continuation, void *user UNUSED)
{
  ціле eol;
  ціле c;

  якщо(!want_screen_scrollback)
    повернути 1;

  eol = cols;
  поки(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  для(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  повернути 1;
}

static ціле screen_sb_popline(ціле cols, VTermScreenCell *cells, void *user UNUSED)
{
  ціле col;

  якщо(!want_screen_scrollback)
    повернути 0;

  // All lines of scrollback contain "ABCDE"
  для(col = 0; col < cols; col++) {
    якщо(col < 5)
      cells[col].chars[0] = 'A' + col;
    інакше
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  повернути 1;
}

static ціле screen_sb_clear(void *user UNUSED)
{
  якщо(!want_screen_scrollback)
    повернути 1;

  printf("sb_clear\n");
  повернути 0;
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

ціле main(ціле argc UNUSED, char **argv UNUSED)
{
  char line[1024] = {0};
  ціле flag;

  ціле err;

  setvbuf(stdout, NULL, _IONBF, 0);

  поки(fgets(line, sizeof line, stdin)) {
    char *nl;
    size_t outlen;
    err = 0;

    якщо((nl = strchr(line, '\n')))
      *nl = '\0';

    якщо(streq(line, "INIT")) {
      якщо(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    інакше якщо(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    інакше якщо(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      ціле i = 9;
      ціле sense = 1;
      assert(vt);
      якщо(!state) {
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

      поки(line[i] == ' ')
        i++;
      для( ; line[i]; i++)
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

    інакше якщо(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      ціле i = 10;
      ціле sense = 1;
      assert(vt);
      якщо(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      поки(line[i] == ' ')
        i++;
      для( ; line[i]; i++)
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

    інакше якщо(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    інакше якщо(streq(line, "RESET")) {
      якщо(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      якщо(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    інакше якщо(strstartswith(line, "RESIZE ")) {
      ціле rows, cols;
      char *linep = line + 7;
      поки(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    інакше якщо(strstartswith(line, "PUSH ")) {
      char *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      якщо(written < len)
        fprintf(stderr, "! short write\n");
    }

    інакше якщо(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      якщо(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    інакше якщо(strstartswith(line, "ENCIN ")) {
      char *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      ціле cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      якщо(cpi > 0) {
	ціле i;
        printf("encout ");
        для(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    інакше якщо(strstartswith(line, "INCHAR ")) {
      char *linep = line + 7;
      unsigned ціле c = 0;
      VTermModifier mod;
      поки(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    інакше якщо(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      char *linep = line + 6;
      поки(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      поки(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    інакше якщо(strstartswith(line, "PASTE ")) {
      char *linep = line + 6;
      якщо(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      інакше якщо(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      інакше
        goto abort_line;
    }

    інакше якщо(strstartswith(line, "FOCUS ")) {
      assert(state);
      char *linep = line + 6;
      якщо(streq(linep, "IN"))
        vterm_state_focus_in(state);
      інакше якщо(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      інакше
        goto abort_line;
    }

    інакше якщо(strstartswith(line, "MOUSEMOVE ")) {
      char *linep = line + 10;
      ціле row, col, len;
      VTermModifier mod;
      поки(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      поки(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    інакше якщо(strstartswith(line, "MOUSEBTN ")) {
      char *linep = line + 9;
      char press;
      ціле button, len;
      VTermModifier mod;
      поки(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      поки(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    інакше якщо(strstartswith(line, "SELECTION ")) {
      char *linep = line + 10;
      unsigned ціле mask;
      ціле len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      поки(linep[0] == ' ')
        linep++;
      якщо(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        поки(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      поки(linep[0] == ' ')
        linep++;
      якщо(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    інакше якщо(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      char *linep = line + 12;
      поки(linep[0] == ' ')
        linep++;
      якщо(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      інакше якщо(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      інакше якщо(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      інакше якщо(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    інакше якщо(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    інакше якщо(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      char *linep = line + 14;
      поки(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      якщо(linep[0]) {
        поки(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      інакше
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    інакше якщо(line[0] == '?') {
      якщо(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        якщо(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        інакше якщо(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        інакше
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      інакше якщо(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        char *linep = line + 5;
        поки(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        якщо(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          якщо(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          інакше
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        інакше якщо(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          якщо(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          інакше
            printf("%d\n", state_pen.underline);
        }
        інакше якщо(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          якщо(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          інакше
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        інакше якщо(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          якщо(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          інакше
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        інакше якщо(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          якщо(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          інакше
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        інакше якщо(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          якщо(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          інакше
            printf("%d\n", state_pen.font);
        }
        інакше якщо(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          якщо(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          інакше
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        інакше якщо(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          якщо(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          інакше
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        інакше якщо(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        інакше якщо(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        інакше
          printf("?\n");
      }
      інакше якщо(strstartswith(line, "?lineinfo ")) {
        assert(state);
        char *linep = line + 10;
        ціле row;
        const VTermLineInfo *info;
        поки(linep[0] == ' ')
          linep++;
        якщо(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        якщо(info->doublewidth)
          printf("dwl ");
        якщо(info->doubleheight)
          printf("dhl ");
        якщо(info->continuation)
          printf("cont ");
        printf("\n");
      }
      інакше якщо(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        char *linep = line + 13;
        VTermRect rect;
        size_t len;
        поки(linep[0] == ' ')
          linep++;
        якщо(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        інакше якщо(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        інакше {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        якщо(len == (size_t)-1)
          printf("! screen_chars error\n");
        інакше якщо(len == 0)
          printf("\n");
        інакше {
          uint32_t *chars = malloc(sizeof(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          для(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      інакше якщо(strstartswith(line, "?screen_text ")) {
        assert(screen);
        char *linep = line + 12;
        VTermRect rect;
        size_t len;
        поки(linep[0] == ' ')
          linep++;
        якщо(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        якщо(len == (size_t)-1)
          printf("! screen_text error\n");
        інакше якщо(len == 0)
          printf("\n");
        інакше {
          // Put an overwrite guard at both ends of the buffer
          unsigned char *buffer = malloc(len + 4);
          unsigned char *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (char *)text, len, rect);

          якщо(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          інакше якщо(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          інакше
	  {
	    size_t i;
            для(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      інакше якщо(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        char *linep = line + 12;
	ціле i;
        VTermPos pos;
        VTermScreenCell cell;
        поки(linep[0] == ' ')
          linep++;
        якщо(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        якщо(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        для(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        якщо(cell.attrs.bold)      printf("B");
        якщо(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        якщо(cell.attrs.italic)    printf("I");
        якщо(cell.attrs.blink)     printf("K");
        якщо(cell.attrs.reverse)   printf("R");
        якщо(cell.attrs.font)      printf("F%d", cell.attrs.font);
        якщо(cell.attrs.small)     printf("S");
        якщо(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        якщо(cell.attrs.dwl)       printf("dwl ");
        якщо(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      інакше якщо(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        char *linep = line + 12;
        поки(linep[0] == ' ')
          linep++;
        якщо(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      інакше якщо(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        char *linep = line + 21;
        поки(linep[0] == ' ')
          linep++;
        якщо(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        якщо(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      інакше
        printf("?\n");

      memset(line, 0, sizeof line);
      continue;
    }

    інакше
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    якщо(outlen > 0) {
      char outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  повернути 0;
}
