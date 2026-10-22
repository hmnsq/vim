#ضمن "vterm.h"
#ضمن "../src/vterm_internal.h" // We pull in some internal bits too

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

ساكن size_t inplace_hex2bytes(حرف *s)
{
  حرف *inpos = s, *outpos = s;

  כלעוד(*inpos) {
    unsigned صحيح ch;
    אם(sscanf(inpos, "%2x", &ch) < 1)
      اكسر;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  החזר outpos - s;
}

ساكن VTermModifier strpe_modifiers(حرف **strp)
{
  VTermModifier state = 0;

  כלעוד((*strp)[0]) {
    بدل(((*strp)++)[0]) {
      حالة 'S': state |= VTERM_MOD_SHIFT; اكسر;
      حالة 'C': state |= VTERM_MOD_CTRL;  اكسر;
      حالة 'A': state |= VTERM_MOD_ALT;   اكسر;
      افتراضي: החזר state;
    }
  }

  החזר state;
}

ساكن VTermKey strp_key(حرف *str)
{
  ساكن بنية {
    حرف *name;
    VTermKey key;
  } keys[] = {
    { "Up",    VTERM_KEY_UP },
    { "Tab",   VTERM_KEY_TAB },
    { "Enter", VTERM_KEY_ENTER },
    { "KP0",   VTERM_KEY_KP_0 },
    { "F1",    VTERM_KEY_FUNCTION(1) },
    { عدم,    VTERM_KEY_NONE },
  };
  صحيح i;

  עבור(i = 0; keys[i].name; i++) {
    אם(streq(str, keys[i].name))
      החזר keys[i].key;
  }

  החזר VTERM_KEY_NONE;
}

ساكن فراغ print_color(ثابت VTermColor *col)
{
  אם (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  אחרת אם (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  אחרת {
    printf("invalid(%d", col->type);
  }
  אם (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  אם (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

ساكن VTermColor strpe_color(حرف **strp)
{
  uint8_t r, g, b, idx;
  صحيح len = 0;
  VTermColor col;

  אם(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  אחרת אם(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  אחרת
    vterm_color_rgb(&col, 127, 127, 127);

  החזר col;
}

ساكن VTerm *vt;
ساكن VTermState *state;
ساكن VTermScreen *screen;

ساكن VTermEncodingInstance encoding;

ساكن فراغ term_output(ثابت حرف *s, size_t len, فراغ *user UNUSED)
{
  size_t i;

  printf("output ");
  עבור(i = 0; i < len; i++)
    printf("%x%s", (unsigned حرف)s[i], i < len-1 ? "," : "\n");
}

ساكن فراغ printhex(ثابت حرف *s, size_t len)
{
  כלעוד(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

ساكن صحيح parser_text(ثابت حرف bytes[], size_t len, فراغ *user UNUSED)
{
  size_t i;

  printf("text ");
  עבור(i = 0; i < len; i++) {
    unsigned حرف b = bytes[i];
    אם(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      اكسر;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  החזר i;
}

ساكن صحيح parser_control(unsigned حرف control, فراغ *user UNUSED)
{
  printf("control %02x\n", control);

  החזר 1;
}

ساكن صحيح parser_escape(ثابت حرف bytes[], size_t len, فراغ *user UNUSED)
{
  אם(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    אם(len < 2)
      החזר -1;
    len = 2;
  }
  אחרת {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  החזר len;
}

ساكن صحيح parser_csi(ثابت حرف *leader, ثابت long args[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user UNUSED)
{
  صحيح i;
  printf("csi %02x", command);

  אם(leader && leader[0]) {
    printf(" L=");
    עבור(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  עבור(i = 0; i < argcount; i++) {
    حرف sep = i ? ',' : ' ';

    אם(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    אחרת
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  אם(intermed && intermed[0]) {
    printf(" I=");
    עבור(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  החזר 1;
}

ساكن صحيح parser_osc(صحيح command, VTermStringFragment frag, فراغ *user UNUSED)
{

  printf("osc ");

  אם(frag.initial) {
    אם(command == -1)
      printf("[");
    אחרת
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  אם(frag.final)
    printf("]");

  printf("\n");

  החזר 1;
}

ساكن صحيح parser_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user UNUSED)
{
  printf("dcs ");

  אם(frag.initial) {
    size_t i;
    printf("[");
    עבור(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  אם(frag.final)
    printf("]");

  printf("\n");

  החזר 1;
}

ساكن صحيح parser_apc(VTermStringFragment frag, فراغ *user UNUSED)
{
  printf("apc ");

  אם(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  אם(frag.final)
    printf("]");

  printf("\n");

  החזר 1;
}

ساكن صحيح parser_pm(VTermStringFragment frag, فراغ *user UNUSED)
{
  printf("pm ");

  אם(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  אם(frag.final)
    printf("]");

  printf("\n");

  החזר 1;
}

ساكن صحيح parser_sos(VTermStringFragment frag, فراغ *user UNUSED)
{
  printf("sos ");

  אם(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  אם(frag.final)
    printf("]");

  printf("\n");

  החזר 1;
}

ساكن VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  عدم // resize
};

ساكن VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

ساكن صحيح want_movecursor = 0;
ساكن VTermPos state_pos;
ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos UNUSED, صحيح visible UNUSED, فراغ *user UNUSED)
{
  state_pos = pos;

  אם(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  החזר 1;
}

ساكن صحيح want_premove = 0;
ساكن صحيح premove(VTermRect rect, فراغ *user UNUSED)
{
  אם(!want_premove)
    החזר 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  החזר 1;
}

ساكن صحيح want_scrollrect = 0;
ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user UNUSED)
{
  אם(!want_scrollrect)
    החזר 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  החזר 1;
}

ساكن صحيح want_moverect = 0;
ساكن صحيح moverect(VTermRect dest, VTermRect src, فراغ *user UNUSED)
{
  אם(!want_moverect)
    החזר 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  החזר 1;
}

ساكن صحيح want_settermprop = 0;
ساكن صحيح settermprop(VTermProp prop, VTermValue *val, فراغ *user UNUSED)
{
  VTermValueType type;
  אם(!want_settermprop)
    החזר 1;

  type = vterm_get_prop_type(prop);
  بدل(type) {
  حالة VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    החזר 1;
  حالة VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->number);
    החזר 1;
  حالة VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    החזר 1;
  حالة VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    החזר 1;

  حالة VTERM_N_VALUETYPES:
    החזר 0;
  }

  החזר 0;
}

// These callbacks are for State

ساكن صحيح want_state_putglyph = 0;
ساكن صحيح state_putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user UNUSED)
{
  صحيح i;
  אם(!want_state_putglyph)
    החזר 1;

  printf("putglyph ");
  עבור(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  אם(info->protected_cell)
    printf(" prot");
  אם(info->dwl)
    printf(" dwl");
  אם(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  החזר 1;
}

ساكن صحيح want_state_erase = 0;
ساكن صحيح state_erase(VTermRect rect, صحيح selective, فراغ *user UNUSED)
{
  אם(!want_state_erase)
    החזר 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  החזר 1;
}

ساكن بنية {
  صحيح bold;
  صحيح underline;
  صحيح italic;
  صحيح blink;
  صحيح reverse;
  صحيح conceal;
  صحيح strike;
  صحيح font;
  صحيح small;
  صحيح baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ساكن صحيح state_setpenattr(VTermAttr attr, VTermValue *val, فراغ *user UNUSED)
{
  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    اكسر;
  حالة VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->number;
    اكسر;
  حالة VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    اكسر;
  حالة VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    اكسر;
  حالة VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    اكسر;
  حالة VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    اكسر;
  حالة VTERM_ATTR_FONT:
    state_pen.font = val->number;
    اكسر;
  حالة VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BASELINE:
    state_pen.baseline = val->number;
    اكسر;
  حالة VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    اكسر;
  حالة VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    اكسر;

  حالة VTERM_N_ATTRS:
    החזר 0;
  }

  החזר 1;
}

ساكن صحيح state_setlineinfo(صحيح row UNUSED, ثابت VTermLineInfo *newinfo UNUSED, ثابت VTermLineInfo *oldinfo UNUSED, فراغ *user UNUSED)
{
  החזר 1;
}

ساكن صحيح want_state_scrollback = 0;
ساكن صحيح state_sb_clear(فراغ *user UNUSED) {
  אם(!want_state_scrollback)
    החזר 1;

  printf("sb_clear\n");
  החזר 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  عدم, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  عدم, // bell
  عدم, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

ساكن صحيح selection_set(VTermSelectionMask mask, VTermStringFragment frag, فراغ *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  אם(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  אם(frag.final)
    printf("]");
  printf("\n");

  החזר 1;
}

ساكن صحيح selection_query(VTermSelectionMask mask, فراغ *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  החזר 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

ساكن صحيح want_screen_damage = 0;
ساكن صحيح want_screen_damage_cells = 0;
ساكن صحيح screen_damage(VTermRect rect, فراغ *user UNUSED)
{
  אם(!want_screen_damage)
    החזר 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  אם(want_screen_damage_cells) {
    صحيح equals = FALSE;
    صحيح row;
    صحيح col;

    עבור(row = rect.start_row; row < rect.end_row; row++) {
      صحيح eol = rect.end_col;
      כלעוד(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        אם(cell.chars[0])
          اكسر;

        eol--;
      }

      אם(eol == rect.start_col)
        اكسر;

      אם(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      עבור(col = rect.start_col; col < eol; col++) {
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

  החזר 1;
}

ساكن صحيح want_screen_scrollback = 0;
ساكن صحيح screen_sb_pushline4(صحيح cols, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user UNUSED)
{
  صحيح eol;
  صحيح c;

  אם(!want_screen_scrollback)
    החזר 1;

  eol = cols;
  כלעוד(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  עבור(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  החזר 1;
}

ساكن صحيح screen_sb_popline(صحيح cols, VTermScreenCell *cells, فراغ *user UNUSED)
{
  صحيح col;

  אם(!want_screen_scrollback)
    החזר 0;

  // All lines of scrollback contain "ABCDE"
  עבור(col = 0; col < cols; col++) {
    אם(col < 5)
      cells[col].chars[0] = 'A' + col;
    אחרת
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  החזר 1;
}

ساكن صحيح screen_sb_clear(فراغ *user UNUSED)
{
  אם(!want_screen_scrollback)
    החזר 1;

  printf("sb_clear\n");
  החזר 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  عدم, // bell
  عدم, // resize
  عدم, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

صحيح main(صحيح argc UNUSED, حرف **argv UNUSED)
{
  حرف line[1024] = {0};
  صحيح flag;

  صحيح err;

  setvbuf(stdout, عدم, _IONBF, 0);

  כלעוד(fgets(line, حجم line, stdin)) {
    حرف *nl;
    size_t outlen;
    err = 0;

    אם((nl = strchr(line, '\n')))
      *nl = '\0';

    אם(streq(line, "INIT")) {
      אם(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    אחרת אם(streq(line, "WANTPARSER")) {
      أكد(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, عدم);
    }

    אחרת אם(strstartswith(line, "WANTSTATE") && (line[9] == '\0' || line[9] == ' ')) {
      صحيح i = 9;
      صحيح sense = 1;
      أكد(vt);
      אם(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, عدم);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, عدم, عدم, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      כלעוד(line[i] == ' ')
        i++;
      עבור( ; line[i]; i++)
        بدل(line[i]) {
        حالة '+':
          sense = 1;
          اكسر;
        حالة '-':
          sense = 0;
          اكسر;
        حالة 'g':
          want_state_putglyph = sense;
          اكسر;
        حالة 's':
          want_scrollrect = sense;
          اكسر;
        حالة 'P':
          want_premove = sense;
          اكسر;
        حالة 'm':
          want_moverect = sense;
          اكسر;
        حالة 'e':
          want_state_erase = sense;
          اكسر;
        حالة 'p':
          want_settermprop = sense;
          اكسر;
        حالة 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : عدم, عدم);
          اكسر;
        حالة 'b':
          want_state_scrollback = sense;
          اكسر;
        افتراضي:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", line[i]);
        }
    }

    אחרת אם(strstartswith(line, "WANTSCREEN") && (line[10] == '\0' || line[10] == ' ')) {
      صحيح i = 10;
      صحيح sense = 1;
      أكد(vt);
      אם(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, عدم);
      vterm_screen_callbacks_has_pushline4(screen);

      כלעוד(line[i] == ' ')
        i++;
      עבור( ; line[i]; i++)
        بدل(line[i]) {
        حالة '-':
          sense = 0;
          اكسر;
        حالة 'a':
          vterm_screen_enable_altscreen(screen, 1);
          اكسر;
        حالة 'd':
          want_screen_damage = sense;
          اكسر;
        حالة 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          اكسر;
        حالة 'm':
          want_moverect = sense;
          اكسر;
        حالة 'c':
          want_movecursor = sense;
          اكسر;
        حالة 'p':
          want_settermprop = 1;
          اكسر;
        حالة 'b':
          want_screen_scrollback = sense;
          اكسر;
        حالة 'r':
          vterm_screen_enable_reflow(screen, sense);
          اكسر;
        افتراضي:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", line[i]);
        }
    }

    אחרת אם(sscanf(line, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    אחרת אם(streq(line, "RESET")) {
      אם(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      אם(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    אחרת אם(strstartswith(line, "RESIZE ")) {
      صحيح rows, cols;
      حرف *linep = line + 7;
      כלעוד(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    אחרת אם(strstartswith(line, "PUSH ")) {
      حرف *bytes = line + 5;
      size_t len = inplace_hex2bytes(bytes);
      أكد(len);

      size_t written = vterm_input_write(vt, bytes, len);
      אם(written < len)
        fprintf(stderr, "! short write\n");
    }

    אחרת אם(streq(line, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      אם(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    אחרת אם(strstartswith(line, "ENCIN ")) {
      حرف *bytes = line + 6;
      size_t len = inplace_hex2bytes(bytes);
      أكد(len);

      uint32_t cp[1024];
      صحيح cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      אם(cpi > 0) {
	صحيح i;
        printf("encout ");
        עבור(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    אחרת אם(strstartswith(line, "INCHAR ")) {
      حرف *linep = line + 7;
      unsigned صحيح c = 0;
      VTermModifier mod;
      כלעוד(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    אחרת אם(strstartswith(line, "INKEY ")) {
      VTermModifier mod;
      VTermKey key;
      حرف *linep = line + 6;
      כלעוד(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      כלעוד(linep[0] == ' ')
        linep++;
      key = strp_key(linep);

      vterm_keyboard_key(vt, key, mod);
    }

    אחרת אם(strstartswith(line, "PASTE ")) {
      حرف *linep = line + 6;
      אם(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      אחרת אם(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      אחרת
        اذهب abort_line;
    }

    אחרת אם(strstartswith(line, "FOCUS ")) {
      أكد(state);
      حرف *linep = line + 6;
      אם(streq(linep, "IN"))
        vterm_state_focus_in(state);
      אחרת אם(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      אחרת
        اذهب abort_line;
    }

    אחרת אם(strstartswith(line, "MOUSEMOVE ")) {
      حرف *linep = line + 10;
      صحيح row, col, len;
      VTermModifier mod;
      כלעוד(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      כלעוד(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    אחרת אם(strstartswith(line, "MOUSEBTN ")) {
      حرف *linep = line + 9;
      حرف press;
      صحيح button, len;
      VTermModifier mod;
      כלעוד(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      כלעוד(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    אחרת אם(strstartswith(line, "SELECTION ")) {
      حرف *linep = line + 10;
      unsigned صحيح mask;
      صحيح len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      כלעוד(linep[0] == ' ')
        linep++;
      אם(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        כלעוד(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      أكد(frag.len);

      linep += frag.len * 2;
      כלעוד(linep[0] == ' ')
        linep++;
      אם(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    אחרת אם(strstartswith(line, "DAMAGEMERGE ")) {
      أكد(screen);
      حرف *linep = line + 12;
      כלעוד(linep[0] == ' ')
        linep++;
      אם(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      אחרת אם(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      אחרת אם(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      אחרת אם(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    אחרת אם(strstartswith(line, "DAMAGEFLUSH")) {
      أكد(screen);
      vterm_screen_flush_damage(screen);
    }

    אחרת אם(strstartswith(line, "SETDEFAULTCOL ")) {
      أكد(screen);
      حرف *linep = line + 14;
      כלעוד(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      אם(linep[0]) {
        כלעוד(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      אחרת
        vterm_screen_set_default_colors(screen, &fg, عدم);
    }

    אחרת אם(line[0] == '?') {
      אם(streq(line, "?cursor")) {
        أكد(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        אם(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        אחרת אם(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        אחרת
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      אחרת אם(strstartswith(line, "?pen ")) {
        أكد(state);
        VTermValue val;
        حرف *linep = line + 5;
        כלעוד(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        אם(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          אם(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          אחרת
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        אחרת אם(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          אם(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          אחרת
            printf("%d\n", state_pen.underline);
        }
        אחרת אם(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          אם(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          אחרת
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        אחרת אם(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          אם(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          אחרת
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        אחרת אם(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          אם(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          אחרת
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        אחרת אם(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          אם(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          אחרת
            printf("%d\n", state_pen.font);
        }
        אחרת אם(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          אם(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          אחרת
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        אחרת אם(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          אם(val.number != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.number, state_pen.baseline);
          אחרת
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        אחרת אם(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        אחרת אם(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        אחרת
          printf("?\n");
      }
      אחרת אם(strstartswith(line, "?lineinfo ")) {
        أكد(state);
        حرف *linep = line + 10;
        صحيح row;
        ثابت VTermLineInfo *info;
        כלעוד(linep[0] == ' ')
          linep++;
        אם(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          اذهب abort_line;
        }
        info = vterm_state_get_lineinfo(state, row);
        אם(info->doublewidth)
          printf("dwl ");
        אם(info->doubleheight)
          printf("dhl ");
        אם(info->continuation)
          printf("cont ");
        printf("\n");
      }
      אחרת אם(strstartswith(line, "?screen_chars ")) {
        أكد(screen);
        حرف *linep = line + 13;
        VTermRect rect;
        size_t len;
        כלעוד(linep[0] == ' ')
          linep++;
        אם(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        אחרת אם(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, عدم, &rect.end_col);
        }
        אחרת {
          printf("! screen_chars unrecognised input\n");
          اذهب abort_line;
        }
        len = vterm_screen_get_chars(screen, عدم, 0, rect);
        אם(len == (size_t)-1)
          printf("! screen_chars error\n");
        אחרת אם(len == 0)
          printf("\n");
        אחרת {
          uint32_t *chars = malloc(حجم(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          עבור(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          حرر(chars);
        }
      }
      אחרת אם(strstartswith(line, "?screen_text ")) {
        أكد(screen);
        حرف *linep = line + 12;
        VTermRect rect;
        size_t len;
        כלעוד(linep[0] == ' ')
          linep++;
        אם(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          اذهب abort_line;
        }
        len = vterm_screen_get_text(screen, عدم, 0, rect);
        אם(len == (size_t)-1)
          printf("! screen_text error\n");
        אחרת אם(len == 0)
          printf("\n");
        אחרת {
          // Put an overwrite guard at both ends of the buffer
          unsigned حرف *buffer = malloc(len + 4);
          unsigned حرف *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (حرف *)text, len, rect);

          אם(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          אחרת אם(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          אחרת
	  {
	    size_t i;
            עבור(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          حرر(buffer);
        }
      }
      אחרת אם(strstartswith(line, "?screen_cell ")) {
        أكد(screen);
        حرف *linep = line + 12;
	صحيح i;
        VTermPos pos;
        VTermScreenCell cell;
        כלעוד(linep[0] == ' ')
          linep++;
        אם(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          اذهب abort_line;
        }
        אם(!vterm_screen_get_cell(screen, pos, &cell))
          اذهب abort_line;
        printf("{");
        עבור(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        אם(cell.attrs.bold)      printf("B");
        אם(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        אם(cell.attrs.italic)    printf("I");
        אם(cell.attrs.blink)     printf("K");
        אם(cell.attrs.reverse)   printf("R");
        אם(cell.attrs.font)      printf("F%d", cell.attrs.font);
        אם(cell.attrs.small)     printf("S");
        אם(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        אם(cell.attrs.dwl)       printf("dwl ");
        אם(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      אחרת אם(strstartswith(line, "?screen_eol ")) {
        أكد(screen);
        VTermPos pos;
        حرف *linep = line + 12;
        כלעוד(linep[0] == ' ')
          linep++;
        אם(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          اذهب abort_line;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      אחרת אם(strstartswith(line, "?screen_attrs_extent ")) {
        أكد(screen);
        VTermPos pos;
        VTermRect rect;
        حرف *linep = line + 21;
        כלעוד(linep[0] == ' ')
          linep++;
        אם(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          اذهب abort_line;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        אם(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          اذهب abort_line;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      אחרת
        printf("?\n");

      memset(line, 0, حجم line);
      استمر;
    }

    אחרת
      abort_line: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    אם(outlen > 0) {
      حرف outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, عدم);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  החזר 0;
}
