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

  док(*inpos) {
    unsigned цео ch;
    ако(sscanf(inpos, "%2x", &ch) < 1)
      break;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  врати outpos - s;
}

static VTermModifier strpe_modifiers(char **strp)
{
  VTermModifier state = 0;

  док((*strp)[0]) {
    switch(((*strp)++)[0]) {
      case 'S': state |= VTERM_MOD_SHIFT; break;
      case 'C': state |= VTERM_MOD_CTRL;  break;
      case 'A': state |= VTERM_MOD_ALT;   break;
      default: врати state;
    }
  }

  врати state;
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
  цео i;

  за(i = 0; keys[i].name; i++) {
    ако(streq(str, keys[i].name))
      врати keys[i].key;
  }

  врати VTERM_KEY_NONE;
}

static void print_color(const VTermColor *col)
{
  ако (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  инако ако (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  инако {
    printf("invalid(%d", col->type);
  }
  ако (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  ако (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

static VTermColor strpe_color(char **strp)
{
  uint8_t r, g, b, idx;
  цео len = 0;
  VTermColor col;

  ако(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  инако ако(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  инако
    vterm_color_rgb(&col, 127, 127, 127);

  врати col;
}

static VTerm *vt;
static VTermState *state;
static VTermScreen *screen;

static VTermEncodingInstance encoding;

static void term_output(const char *s, size_t len, void *user UNUSED)
{
  size_t i;

  printf("output ");
  за(i = 0; i < len; i++)
    printf("%x%s", (unsigned char)s[i], i < len-1 ? "," : "\n");
}

static void printhex(const char *s, size_t len)
{
  док(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

static цео parser_text(const char bytes[], size_t len, void *user UNUSED)
{
  size_t i;

  printf("text ");
  за(i = 0; i < len; i++) {
    unsigned char b = bytes[i];
    ако(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      break;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  врати i;
}

static цео parser_control(unsigned char control, void *user UNUSED)
{
  printf("control %02x\n", control);

  врати 1;
}

static цео parser_escape(const char bytes[], size_t len, void *user UNUSED)
{
  ако(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    ако(len < 2)
      врати -1;
    len = 2;
  }
  инако {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  врати len;
}

static цео parser_csi(const char *leader, const long args[], цео argcount, const char *intermed, char command, void *user UNUSED)
{
  цео i;
  printf("csi %02x", command);

  ако(leader && leader[0]) {
    printf(" L=");
    за(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  за(i = 0; i < argcount; i++) {
    char sep = i ? ',' : ' ';

    ако(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    инако
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  ако(intermed && intermed[0]) {
    printf(" I=");
    за(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  врати 1;
}

static цео parser_osc(цео command, VTermStringFragment frag, void *user UNUSED)
{

  printf("osc ");

  ако(frag.initial) {
    ако(command == -1)
      printf("[");
    инако
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ако(frag.final)
    printf("]");

  printf("\n");

  врати 1;
}

static цео parser_dcs(const char *command, size_t commandlen, VTermStringFragment frag, void *user UNUSED)
{
  printf("dcs ");

  ако(frag.initial) {
    size_t i;
    printf("[");
    за(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ако(frag.final)
    printf("]");

  printf("\n");

  врати 1;
}

static цео parser_apc(VTermStringFragment frag, void *user UNUSED)
{
  printf("apc ");

  ако(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ако(frag.final)
    printf("]");

  printf("\n");

  врати 1;
}

static цео parser_pm(VTermStringFragment frag, void *user UNUSED)
{
  printf("pm ");

  ако(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ако(frag.final)
    printf("]");

  printf("\n");

  врати 1;
}

static цео parser_sos(VTermStringFragment frag, void *user UNUSED)
{
  printf("sos ");

  ако(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  ако(frag.final)
    printf("]");

  printf("\n");

  врати 1;
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

static цео want_movecursor = 0;
static VTermPos state_pos;
static цео movecursor(VTermPos pos, VTermPos oldpos UNUSED, цео visible UNUSED, void *user UNUSED)
{
  state_pos = pos;

  ако(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  врати 1;
}

static цео want_premove = 0;
static цео premove(VTermRect rect, void *user UNUSED)
{
  ако(!want_premove)
    врати 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  врати 1;
}

static цео want_scrollrect = 0;
static цео scrollrect(VTermRect rect, цео downward, цео rightward, void *user UNUSED)
{
  ако(!want_scrollrect)
    врати 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  врати 1;
}

static цео want_moverect = 0;
static цео moverect(VTermRect dest, VTermRect src, void *user UNUSED)
{
  ако(!want_moverect)
    врати 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  врати 1;
}

static цео want_settermprop = 0;
static цео settermprop(VTermProp prop, VTermValue *val, void *user UNUSED)
{
  VTermValueType type;
  ако(!want_settermprop)
    врати 1;

  type = vterm_get_prop_type(prop);
  switch(type) {
  case VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    врати 1;
  case VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    врати 1;
  case VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    врати 1;
  case VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    врати 1;

  case VTERM_N_VALUETYPES:
    врати 0;
  }

  врати 0;
}

// These callbacks are for State

static цео want_state_putglyph = 0;
static цео state_putglyph(VTermGlyphInfo *info, VTermPos pos, void *user UNUSED)
{
  цео i;
  ако(!want_state_putglyph)
    врати 1;

  printf("putglyph ");
  за(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  ако(info->protected_cell)
    printf(" prot");
  ако(info->dwl)
    printf(" dwl");
  ако(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  врати 1;
}

static цео want_state_erase = 0;
static цео state_erase(VTermRect rect, цео selective, void *user UNUSED)
{
  ако(!want_state_erase)
    врати 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  врати 1;
}

static struct {
  цео bold;
  цео underline;
  цео italic;
  цео blink;
  цео reverse;
  цео conceal;
  цео strike;
  цео font;
  цео small;
  цео baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
static цео state_setpenattr(VTermAttr attr, VTermValue *val, void *user UNUSED)
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
    врати 0;
  }

  врати 1;
}

static цео state_setlineinfo(цео row UNUSED, const VTermLineInfo *newinfo UNUSED, const VTermLineInfo *oldinfo UNUSED, void *user UNUSED)
{
  врати 1;
}

static цео want_state_scrollback = 0;
static цео state_sb_clear(void *user UNUSED) {
  ако(!want_state_scrollback)
    врати 1;

  printf("sb_clear\n");
  врати 0;
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

static цео selection_set(VTermSelectionMask mask, VTermStringFragment frag, void *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  ако(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  ако(frag.final)
    printf("]");
  printf("\n");

  врати 1;
}

static цео selection_query(VTermSelectionMask mask, void *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  врати 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

static цео want_screen_damage = 0;
static цео want_screen_damage_cells = 0;
static цео screen_damage(VTermRect rect, void *user UNUSED)
{
  ако(!want_screen_damage)
    врати 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ако(want_screen_damage_cells) {
    цео equals = FALSE;
    цео row;
    цео col;

    за(row = rect.start_row; row < rect.end_row; row++) {
      цео eol = rect.end_col;
      док(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ако(cell.chars[0])
          break;

        eol--;
      }

      ако(eol == rect.start_col)
        break;

      ако(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      за(col = rect.start_col; col < eol; col++) {
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

  врати 1;
}

static цео want_screen_scrollback = 0;
static цео screen_sb_pushline4(цео cols, const VTermScreenCell *cells, цео continuation, void *user UNUSED)
{
  цео eol;
  цео c;

  ако(!want_screen_scrollback)
    врати 1;

  eol = cols;
  док(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  за(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  врати 1;
}

static цео screen_sb_popline(цео cols, VTermScreenCell *cells, void *user UNUSED)
{
  цео col;

  ако(!want_screen_scrollback)
    врати 0;

  // All lines of scrollback contain "ABCDE"
  за(col = 0; col < cols; col++) {
    ако(col < 5)
      cells[col].chars[0] = 'A' + col;
    инако
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  врати 1;
}

static цео screen_sb_clear(void *user UNUSED)
{
  ако(!want_screen_scrollback)
    врати 1;

  printf("sb_clear\n");
  врати 0;
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

цео main(цео argc UNUSED, char **argv UNUSED)
{
  char line[1024] = {0};
  цео flag;

  цео err;

  setvbuf(stdout, NULL, _IONBF, 0);

  док(fgets(line, sizeof line, stdin)) {
    char *nl;
    size_t outlen;
    err = 0;

    ако((nl = strchr(line, '\n')))
      *nl = '\0';

    ако(streq(line, "INIT")) {
      ако(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    инако ако(streq(line, "WANTPARSER")) {
      assert(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, NULL);
    }

    инако ако(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      цео i = 9;
      цео sense = 1;
      assert(vt);
      ако(!state) {
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

      док(line[i] == ' ')
        i++;
      за( ; line[i]; i++)
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

    инако ако(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      цео i = 10;
      цео sense = 1;
      assert(vt);
      ако(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, NULL);
      vterm_screen_callbacks_has_pushline4(screen);

      док(line[i] == ' ')
        i++;
      за( ; line[i]; i++)
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

    инако ако(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    инако ако(streq(line, "RESET")) {
      ако(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ако(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    инако ако(strstartswith(line, "RESIZE ")) {
      цео rows, cols;
      char *linep = line + 7;
      док(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    инако ако(strstartswith(line, "PUSH ")) {
      char *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      size_t written = vterm_input_write(vt, bytes, len);
      ако(written < len)
        fprintf(stderr, "! short write\n");
    }

    инако ако(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ако(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    инако ако(strstartswith(line, "ENCIN ")) {
      char *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      assert(len);

      uint32_t cp[1024];
      цео cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      ако(cpi > 0) {
	цео i;
        printf("encout ");
        за(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    инако ако(strstartswith(line, "INCHAR ")) {
      char *linep = line + 7;
      unsigned цео c = 0;
      VTermModifier mod;
      док(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    инако ако(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      char *linep = line + 6;
      док(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      док(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    инако ако(strstartswith(line, "PASTE ")) {
      char *linep = line + 6;
      ако(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      инако ако(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      инако
        goto abort_line;
    }

    инако ако(strstartswith(line, "FOCUS ")) {
      assert(state);
      char *linep = line + 6;
      ако(streq(linep, "IN"))
        vterm_state_focus_in(state);
      инако ако(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      инако
        goto abort_line;
    }

    инако ако(strstartswith(line, "MOUSEMOVE ")) {
      char *linep = line + 10;
      цео row, col, len;
      VTermModifier mod;
      док(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      док(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    инако ако(strstartswith(line, "MOUSEBTN ")) {
      char *linep = line + 9;
      char press;
      цео button, len;
      VTermModifier mod;
      док(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      док(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    инако ако(strstartswith(line, "SELECTION ")) {
      char *linep = line + 10;
      unsigned цео mask;
      цео len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      док(linep[0] == ' ')
        linep++;
      ако(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        док(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      assert(frag.len);

      linep += frag.len * 2;
      док(linep[0] == ' ')
        linep++;
      ако(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    инако ако(strstartswith(line, "DAMAGEMERGE ")) {
      assert(screen);
      char *linep = line + 12;
      док(linep[0] == ' ')
        linep++;
      ако(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      инако ако(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      инако ако(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      инако ако(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    инако ако(strstartswith(line, "DAMAGEFLUSH")) {
      assert(screen);
      vterm_screen_flush_damage(screen);
    }

    инако ако(strstartswith(line, "SETDEFAULTCOL ")) {
      assert(screen);
      char *linep = line + 14;
      док(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ако(linep[0]) {
        док(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      инако
        vterm_screen_set_default_colors(screen, &fg, NULL);
    }

    инако ако(line[0] == '?') {
      ако(streq(line, "?cursor")) {
        assert(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ако(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        инако ако(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        инако
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      инако ако(strstartswith(line, "?pen ")) {
        assert(state);
        VTermValue val;
        char *linep = line + 5;
        док(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        ако(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ако(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          инако
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        инако ако(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ако(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          инако
            printf("%d\n", state_pen.underline);
        }
        инако ако(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ако(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          инако
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        инако ако(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ако(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          инако
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        инако ако(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          ако(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          инако
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        инако ако(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ако(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          инако
            printf("%d\n", state_pen.font);
        }
        инако ако(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ако(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          инако
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        инако ако(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ако(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          инако
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        инако ако(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        инако ако(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        инако
          printf("?\n");
      }
      инако ако(strstartswith(line, "?lineinfo ")) {
        assert(state);
        char *linep = line + 10;
        цео row;
        const VTermLineInfo *info;
        док(linep[0] == ' ')
          linep++;
        ако(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          goto abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        ако(info->doublewidth)
          printf("dwl ");
        ако(info->doubleheight)
          printf("dhl ");
        ако(info->continuation)
          printf("cont ");
        printf("\n");
      }
      инако ако(strstartswith(line, "?screen_chars ")) {
        assert(screen);
        char *linep = line + 13;
        VTermRect rect;
        size_t len;
        док(linep[0] == ' ')
          linep++;
        ако(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        инако ако(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, NULL, &rect.end_col);
        }
        инако {
          printf("! screen_chars unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_chars(screen, NULL, 0, rect);
        ако(len == (size_t)-1)
          printf("! screen_chars error\n");
        инако ако(len == 0)
          printf("\n");
        инако {
          uint32_t *chars = malloc(sizeof(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          за(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          free(chars);
        }
      }
      инако ако(strstartswith(line, "?screen_text ")) {
        assert(screen);
        char *linep = line + 12;
        VTermRect rect;
        size_t len;
        док(linep[0] == ' ')
          linep++;
        ако(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          goto abort_line;
        }
        len = vterm_screen_get_text(screen, NULL, 0, rect);
        ако(len == (size_t)-1)
          printf("! screen_text error\n");
        инако ако(len == 0)
          printf("\n");
        инако {
          // Put an overwrite guard at both ends of the buffer
          unsigned char *buffer = malloc(len + 4);
          unsigned char *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (char *)text, len, rect);

          ако(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          инако ако(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          инако
	  {
	    size_t i;
            за(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          free(buffer);
        }
      }
      инако ако(strstartswith(line, "?screen_cell ")) {
        assert(screen);
        char *linep = line + 12;
	цео i;
        VTermPos pos;
        VTermScreenCell cell;
        док(linep[0] == ' ')
          linep++;
        ако(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          goto abort_line;
        }
        ако(!vterm_screen_get_cell(screen, pos, &cell))
          goto abort_line;
        printf("{");
        за(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        ако(cell.attrs.bold)      printf("B");
        ако(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        ако(cell.attrs.italic)    printf("I");
        ако(cell.attrs.blink)     printf("K");
        ако(cell.attrs.reverse)   printf("R");
        ако(cell.attrs.font)      printf("F%d", cell.attrs.font);
        ако(cell.attrs.small)     printf("S");
        ако(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        ако(cell.attrs.dwl)       printf("dwl ");
        ако(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      инако ако(strstartswith(line, "?screen_eol ")) {
        assert(screen);
        VTermPos pos;
        char *linep = line + 12;
        док(linep[0] == ' ')
          linep++;
        ако(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          goto abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      инако ако(strstartswith(line, "?screen_attrs_extent ")) {
        assert(screen);
        VTermPos pos;
        VTermRect rect;
        char *linep = line + 21;
        док(linep[0] == ' ')
          linep++;
        ако(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          goto abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        ако(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          goto abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      инако
        printf("?\n");

      memset(line, 0, sizeof line);
      continue;
    }

    инако
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    ако(outlen > 0) {
      char outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, NULL);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  врати 0;
}
