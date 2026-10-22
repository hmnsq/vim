#شامل "vterm.h"
#شامل "../src/vterm_internal.h" // We pull in some internal bits too

#شامل <assert.h>
#شامل <stdio.h>
#شامل <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

ایستا size_t inplace_hex2bytes(نویسه *s)
{
  نویسه *inpos = s, *outpos = s;

  وقتی(*inpos) {
    unsigned صحیح ch;
    اگر(sscanf(inpos, "%2x", &ch) < 1)
      بشکن;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  بازگشت outpos - s;
}

ایستا VTermModifier strpe_modifiers(نویسه **strp)
{
  VTermModifier state = 0;

  وقتی((*strp)[0]) {
    انتخاب(((*strp)++)[0]) {
      حالت 'S': state |= VTERM_MOD_SHIFT; بشکن;
      حالت 'C': state |= VTERM_MOD_CTRL;  بشکن;
      حالت 'A': state |= VTERM_MOD_ALT;   بشکن;
      پیشفرض: بازگشت state;
    }
  }

  بازگشت state;
}

ایستا VTerm_کلید strp_کلید(نویسه *str)
{
  ایستا ساختار {
    نویسه *name;
    VTerm_کلید کلید;
  } کلیدها[] = {
    { "Up",    VTERM_کلید_UP },
    { "Tab",   VTERM_کلید_TAB },
    { "Enter", VTERM_کلید_ENTER },
    { "KP0",   VTERM_کلید_KP_0 },
    { "F1",    VTERM_کلید_FUNCTION(1) },
    { پوچ,    VTERM_کلید_NONE },
  };
  صحیح i;

  برای(i = 0; کلیدها[i].name; i++) {
    اگر(streq(str, کلیدها[i].name))
      بازگشت کلیدها[i].کلید;
  }

  بازگشت VTERM_کلید_NONE;
}

ایستا تهی print_color(ثابت VTermColor *col)
{
  اگر (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  وگرنه اگر (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  وگرنه {
    printf("invalid(%d", col->type);
  }
  اگر (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  اگر (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

ایستا VTermColor strpe_color(نویسه **strp)
{
  uint8_t r, g, b, idx;
  صحیح len = 0;
  VTermColor col;

  اگر(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  وگرنه اگر(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  وگرنه
    vterm_color_rgb(&col, 127, 127, 127);

  بازگشت col;
}

ایستا VTerm *vt;
ایستا VTermState *state;
ایستا VTermScreen *screen;

ایستا VTermEncodingInstance encoding;

ایستا تهی term_output(ثابت نویسه *s, size_t len, تهی *user UNUSED)
{
  size_t i;

  printf("output ");
  برای(i = 0; i < len; i++)
    printf("%x%s", (unsigned نویسه)s[i], i < len-1 ? "," : "\n");
}

ایستا تهی printhex(ثابت نویسه *s, size_t len)
{
  وقتی(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

ایستا صحیح parser_text(ثابت نویسه bytes[], size_t len, تهی *user UNUSED)
{
  size_t i;

  printf("text ");
  برای(i = 0; i < len; i++) {
    unsigned نویسه b = bytes[i];
    اگر(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      بشکن;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  بازگشت i;
}

ایستا صحیح parser_control(unsigned نویسه control, تهی *user UNUSED)
{
  printf("control %02x\n", control);

  بازگشت 1;
}

ایستا صحیح parser_escape(ثابت نویسه bytes[], size_t len, تهی *user UNUSED)
{
  اگر(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    اگر(len < 2)
      بازگشت -1;
    len = 2;
  }
  وگرنه {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  بازگشت len;
}

ایستا صحیح parser_csi(ثابت نویسه *leader, ثابت long args[], صحیح argcount, ثابت نویسه *intermed, نویسه command, تهی *user UNUSED)
{
  صحیح i;
  printf("csi %02x", command);

  اگر(leader && leader[0]) {
    printf(" L=");
    برای(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  برای(i = 0; i < argcount; i++) {
    نویسه sep = i ? ',' : ' ';

    اگر(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    وگرنه
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  اگر(intermed && intermed[0]) {
    printf(" I=");
    برای(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  بازگشت 1;
}

ایستا صحیح parser_osc(صحیح command, VTermStringFragment frag, تهی *user UNUSED)
{

  printf("osc ");

  اگر(frag.initial) {
    اگر(command == -1)
      printf("[");
    وگرنه
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  اگر(frag.final)
    printf("]");

  printf("\n");

  بازگشت 1;
}

ایستا صحیح parser_dcs(ثابت نویسه *command, size_t commandlen, VTermStringFragment frag, تهی *user UNUSED)
{
  printf("dcs ");

  اگر(frag.initial) {
    size_t i;
    printf("[");
    برای(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  اگر(frag.final)
    printf("]");

  printf("\n");

  بازگشت 1;
}

ایستا صحیح parser_apc(VTermStringFragment frag, تهی *user UNUSED)
{
  printf("apc ");

  اگر(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  اگر(frag.final)
    printf("]");

  printf("\n");

  بازگشت 1;
}

ایستا صحیح parser_pm(VTermStringFragment frag, تهی *user UNUSED)
{
  printf("pm ");

  اگر(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  اگر(frag.final)
    printf("]");

  printf("\n");

  بازگشت 1;
}

ایستا صحیح parser_sos(VTermStringFragment frag, تهی *user UNUSED)
{
  printf("sos ");

  اگر(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  اگر(frag.final)
    printf("]");

  printf("\n");

  بازگشت 1;
}

ایستا VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  پوچ // resize
};

ایستا VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

ایستا صحیح want_movecursor = 0;
ایستا VTermPos state_pos;
ایستا صحیح movecursor(VTermPos pos, VTermPos oldpos UNUSED, صحیح visible UNUSED, تهی *user UNUSED)
{
  state_pos = pos;

  اگر(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  بازگشت 1;
}

ایستا صحیح want_premove = 0;
ایستا صحیح premove(VTermRect rect, تهی *user UNUSED)
{
  اگر(!want_premove)
    بازگشت 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  بازگشت 1;
}

ایستا صحیح want_scrollrect = 0;
ایستا صحیح scrollrect(VTermRect rect, صحیح downward, صحیح rightward, تهی *user UNUSED)
{
  اگر(!want_scrollrect)
    بازگشت 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  بازگشت 1;
}

ایستا صحیح want_moverect = 0;
ایستا صحیح moverect(VTermRect dest, VTermRect src, تهی *user UNUSED)
{
  اگر(!want_moverect)
    بازگشت 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  بازگشت 1;
}

ایستا صحیح want_settermprop = 0;
ایستا صحیح settermprop(VTermProp prop, VTerm_مقدار *val, تهی *user UNUSED)
{
  VTerm_مقدار_Type type;
  اگر(!want_settermprop)
    بازگشت 1;

  type = vterm_get_prop_type(prop);
  انتخاب(type) {
  حالت VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    بازگشت 1;
  حالت VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->عدد);
    بازگشت 1;
  حالت VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    بازگشت 1;
  حالت VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    بازگشت 1;

  حالت VTERM_N_VALUETYPES:
    بازگشت 0;
  }

  بازگشت 0;
}

// These callbacks are for State

ایستا صحیح want_state_putglyph = 0;
ایستا صحیح state_putglyph(VTermGlyphInfo *info, VTermPos pos, تهی *user UNUSED)
{
  صحیح i;
  اگر(!want_state_putglyph)
    بازگشت 1;

  printf("putglyph ");
  برای(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  اگر(info->protected_cell)
    printf(" prot");
  اگر(info->dwl)
    printf(" dwl");
  اگر(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  بازگشت 1;
}

ایستا صحیح want_state_erase = 0;
ایستا صحیح state_erase(VTermRect rect, صحیح selective, تهی *user UNUSED)
{
  اگر(!want_state_erase)
    بازگشت 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  بازگشت 1;
}

ایستا ساختار {
  صحیح bold;
  صحیح underline;
  صحیح italic;
  صحیح blink;
  صحیح reverse;
  صحیح conceal;
  صحیح strike;
  صحیح font;
  صحیح small;
  صحیح baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ایستا صحیح state_setpenattr(VTermAttr attr, VTerm_مقدار *val, تهی *user UNUSED)
{
  انتخاب(attr) {
  حالت VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    بشکن;
  حالت VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->عدد;
    بشکن;
  حالت VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    بشکن;
  حالت VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    بشکن;
  حالت VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    بشکن;
  حالت VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    بشکن;
  حالت VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    بشکن;
  حالت VTERM_ATTR_FONT:
    state_pen.font = val->عدد;
    بشکن;
  حالت VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    بشکن;
  حالت VTERM_ATTR_BASELINE:
    state_pen.baseline = val->عدد;
    بشکن;
  حالت VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    بشکن;
  حالت VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    بشکن;

  حالت VTERM_N_ATTRS:
    بازگشت 0;
  }

  بازگشت 1;
}

ایستا صحیح state_setlineinfo(صحیح row UNUSED, ثابت VTerm_سطر_Info *newinfo UNUSED, ثابت VTerm_سطر_Info *oldinfo UNUSED, تهی *user UNUSED)
{
  بازگشت 1;
}

ایستا صحیح want_state_scrollback = 0;
ایستا صحیح state_sb_clear(تهی *user UNUSED) {
  اگر(!want_state_scrollback)
    بازگشت 1;

  printf("sb_clear\n");
  بازگشت 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  پوچ, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  پوچ, // bell
  پوچ, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

ایستا صحیح selection_set(VTermSelectionMask mask, VTermStringFragment frag, تهی *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  اگر(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  اگر(frag.final)
    printf("]");
  printf("\n");

  بازگشت 1;
}

ایستا صحیح selection_query(VTermSelectionMask mask, تهی *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  بازگشت 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

ایستا صحیح want_screen_damage = 0;
ایستا صحیح want_screen_damage_cells = 0;
ایستا صحیح screen_damage(VTermRect rect, تهی *user UNUSED)
{
  اگر(!want_screen_damage)
    بازگشت 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  اگر(want_screen_damage_cells) {
    صحیح equals = FALSE;
    صحیح row;
    صحیح col;

    برای(row = rect.start_row; row < rect.end_row; row++) {
      صحیح eol = rect.end_col;
      وقتی(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        اگر(cell.chars[0])
          بشکن;

        eol--;
      }

      اگر(eol == rect.start_col)
        بشکن;

      اگر(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      برای(col = rect.start_col; col < eol; col++) {
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

  بازگشت 1;
}

ایستا صحیح want_screen_scrollback = 0;
ایستا صحیح screen_sb_pushline4(صحیح cols, ثابت VTermScreenCell *cells, صحیح continuation, تهی *user UNUSED)
{
  صحیح eol;
  صحیح c;

  اگر(!want_screen_scrollback)
    بازگشت 1;

  eol = cols;
  وقتی(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  برای(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  بازگشت 1;
}

ایستا صحیح screen_sb_popline(صحیح cols, VTermScreenCell *cells, تهی *user UNUSED)
{
  صحیح col;

  اگر(!want_screen_scrollback)
    بازگشت 0;

  // All lines of scrollback contain "ABCDE"
  برای(col = 0; col < cols; col++) {
    اگر(col < 5)
      cells[col].chars[0] = 'A' + col;
    وگرنه
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  بازگشت 1;
}

ایستا صحیح screen_sb_clear(تهی *user UNUSED)
{
  اگر(!want_screen_scrollback)
    بازگشت 1;

  printf("sb_clear\n");
  بازگشت 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  پوچ, // bell
  پوچ, // resize
  پوچ, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

صحیح اصلی(صحیح argc UNUSED, نویسه **argv UNUSED)
{
  نویسه سطر[1024] = {0};
  صحیح flag;

  صحیح err;

  setvbuf(stdout, پوچ, _IONBF, 0);

  وقتی(fgets(سطر, اندازه سطر, stdin)) {
    نویسه *nl;
    size_t outlen;
    err = 0;

    اگر((nl = strchr(سطر, '\n')))
      *nl = '\0';

    اگر(streq(سطر, "INIT")) {
      اگر(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    وگرنه اگر(streq(سطر, "WANTPARSER")) {
      أكد(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, پوچ);
    }

    وگرنه اگر(strstartswith(سطر, "WANTSTATE") && (سطر[9] == '\0' || سطر[9] == ' ')) {
      صحیح i = 9;
      صحیح sense = 1;
      أكد(vt);
      اگر(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, پوچ);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, پوچ, پوچ, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      وقتی(سطر[i] == ' ')
        i++;
      برای( ; سطر[i]; i++)
        انتخاب(سطر[i]) {
        حالت '+':
          sense = 1;
          بشکن;
        حالت '-':
          sense = 0;
          بشکن;
        حالت 'g':
          want_state_putglyph = sense;
          بشکن;
        حالت 's':
          want_scrollrect = sense;
          بشکن;
        حالت 'P':
          want_premove = sense;
          بشکن;
        حالت 'm':
          want_moverect = sense;
          بشکن;
        حالت 'e':
          want_state_erase = sense;
          بشکن;
        حالت 'p':
          want_settermprop = sense;
          بشکن;
        حالت 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : پوچ, پوچ);
          بشکن;
        حالت 'b':
          want_state_scrollback = sense;
          بشکن;
        پیشفرض:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", سطر[i]);
        }
    }

    وگرنه اگر(strstartswith(سطر, "WANTSCREEN") && (سطر[10] == '\0' || سطر[10] == ' ')) {
      صحیح i = 10;
      صحیح sense = 1;
      أكد(vt);
      اگر(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, پوچ);
      vterm_screen_callbacks_has_pushline4(screen);

      وقتی(سطر[i] == ' ')
        i++;
      برای( ; سطر[i]; i++)
        انتخاب(سطر[i]) {
        حالت '-':
          sense = 0;
          بشکن;
        حالت 'a':
          vterm_screen_enable_altscreen(screen, 1);
          بشکن;
        حالت 'd':
          want_screen_damage = sense;
          بشکن;
        حالت 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          بشکن;
        حالت 'm':
          want_moverect = sense;
          بشکن;
        حالت 'c':
          want_movecursor = sense;
          بشکن;
        حالت 'p':
          want_settermprop = 1;
          بشکن;
        حالت 'b':
          want_screen_scrollback = sense;
          بشکن;
        حالت 'r':
          vterm_screen_enable_reflow(screen, sense);
          بشکن;
        پیشفرض:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", سطر[i]);
        }
    }

    وگرنه اگر(sscanf(سطر, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    وگرنه اگر(streq(سطر, "RESET")) {
      اگر(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      اگر(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    وگرنه اگر(strstartswith(سطر, "RESIZE ")) {
      صحیح rows, cols;
      نویسه *linep = سطر + 7;
      وقتی(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    وگرنه اگر(strstartswith(سطر, "PUSH ")) {
      نویسه *bytes = سطر + 5;
      size_t len = inplace_hex2bytes(bytes);
      أكد(len);

      size_t written = vterm_درون_داد_write(vt, bytes, len);
      اگر(written < len)
        fprintf(stderr, "! short write\n");
    }

    وگرنه اگر(streq(سطر, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      اگر(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    وگرنه اگر(strstartswith(سطر, "ENCIN ")) {
      نویسه *bytes = سطر + 6;
      size_t len = inplace_hex2bytes(bytes);
      أكد(len);

      uint32_t cp[1024];
      صحیح cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      اگر(cpi > 0) {
	صحیح i;
        printf("encout ");
        برای(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    وگرنه اگر(strstartswith(سطر, "INCHAR ")) {
      نویسه *linep = سطر + 7;
      unsigned صحیح c = 0;
      VTermModifier mod;
      وقتی(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    وگرنه اگر(strstartswith(سطر, "INKEY ")) {
      VTermModifier mod;
      VTerm_کلید کلید;
      نویسه *linep = سطر + 6;
      وقتی(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      وقتی(linep[0] == ' ')
        linep++;
      کلید = strp_کلید(linep);

      vterm_keyboard_کلید(vt, کلید, mod);
    }

    وگرنه اگر(strstartswith(سطر, "PASTE ")) {
      نویسه *linep = سطر + 6;
      اگر(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      وگرنه اگر(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      وگرنه
        برو abort_سطر;
    }

    وگرنه اگر(strstartswith(سطر, "FOCUS ")) {
      أكد(state);
      نویسه *linep = سطر + 6;
      اگر(streq(linep, "IN"))
        vterm_state_focus_in(state);
      وگرنه اگر(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      وگرنه
        برو abort_سطر;
    }

    وگرنه اگر(strstartswith(سطر, "MOUSEMOVE ")) {
      نویسه *linep = سطر + 10;
      صحیح row, col, len;
      VTermModifier mod;
      وقتی(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      وقتی(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    وگرنه اگر(strstartswith(سطر, "MOUSEBTN ")) {
      نویسه *linep = سطر + 9;
      نویسه press;
      صحیح button, len;
      VTermModifier mod;
      وقتی(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      وقتی(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    وگرنه اگر(strstartswith(سطر, "SELECTION ")) {
      نویسه *linep = سطر + 10;
      unsigned صحیح mask;
      صحیح len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      وقتی(linep[0] == ' ')
        linep++;
      اگر(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        وقتی(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      أكد(frag.len);

      linep += frag.len * 2;
      وقتی(linep[0] == ' ')
        linep++;
      اگر(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    وگرنه اگر(strstartswith(سطر, "DAMAGEMERGE ")) {
      أكد(screen);
      نویسه *linep = سطر + 12;
      وقتی(linep[0] == ' ')
        linep++;
      اگر(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      وگرنه اگر(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      وگرنه اگر(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      وگرنه اگر(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    وگرنه اگر(strstartswith(سطر, "DAMAGEFLUSH")) {
      أكد(screen);
      vterm_screen_flush_damage(screen);
    }

    وگرنه اگر(strstartswith(سطر, "SETDEFAULTCOL ")) {
      أكد(screen);
      نویسه *linep = سطر + 14;
      وقتی(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      اگر(linep[0]) {
        وقتی(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      وگرنه
        vterm_screen_set_default_colors(screen, &fg, پوچ);
    }

    وگرنه اگر(سطر[0] == '?') {
      اگر(streq(سطر, "?cursor")) {
        أكد(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        اگر(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        وگرنه اگر(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        وگرنه
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      وگرنه اگر(strstartswith(سطر, "?pen ")) {
        أكد(state);
        VTerm_مقدار val;
        نویسه *linep = سطر + 5;
        وقتی(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        اگر(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          اگر(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          وگرنه
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        وگرنه اگر(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          اگر(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          وگرنه
            printf("%d\n", state_pen.underline);
        }
        وگرنه اگر(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          اگر(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          وگرنه
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        وگرنه اگر(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          اگر(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          وگرنه
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        وگرنه اگر(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          اگر(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          وگرنه
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        وگرنه اگر(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          اگر(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          وگرنه
            printf("%d\n", state_pen.font);
        }
        وگرنه اگر(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          اگر(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          وگرنه
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        وگرنه اگر(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          اگر(val.عدد != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.عدد, state_pen.baseline);
          وگرنه
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        وگرنه اگر(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        وگرنه اگر(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        وگرنه
          printf("?\n");
      }
      وگرنه اگر(strstartswith(سطر, "?lineinfo ")) {
        أكد(state);
        نویسه *linep = سطر + 10;
        صحیح row;
        ثابت VTerm_سطر_Info *info;
        وقتی(linep[0] == ' ')
          linep++;
        اگر(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          برو abort_سطر;
        }
        info = vterm_state_get_lineinfo(state, row);
        اگر(info->doublewidth)
          printf("dwl ");
        اگر(info->doubleheight)
          printf("dhl ");
        اگر(info->continuation)
          printf("cont ");
        printf("\n");
      }
      وگرنه اگر(strstartswith(سطر, "?screen_chars ")) {
        أكد(screen);
        نویسه *linep = سطر + 13;
        VTermRect rect;
        size_t len;
        وقتی(linep[0] == ' ')
          linep++;
        اگر(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        وگرنه اگر(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, پوچ, &rect.end_col);
        }
        وگرنه {
          printf("! screen_chars unrecognised input\n");
          برو abort_سطر;
        }
        len = vterm_screen_get_chars(screen, پوچ, 0, rect);
        اگر(len == (size_t)-1)
          printf("! screen_chars error\n");
        وگرنه اگر(len == 0)
          printf("\n");
        وگرنه {
          uint32_t *chars = malloc(اندازه(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          برای(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          آزاد(chars);
        }
      }
      وگرنه اگر(strstartswith(سطر, "?screen_text ")) {
        أكد(screen);
        نویسه *linep = سطر + 12;
        VTermRect rect;
        size_t len;
        وقتی(linep[0] == ' ')
          linep++;
        اگر(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          برو abort_سطر;
        }
        len = vterm_screen_get_text(screen, پوچ, 0, rect);
        اگر(len == (size_t)-1)
          printf("! screen_text error\n");
        وگرنه اگر(len == 0)
          printf("\n");
        وگرنه {
          // Put an overwrite guard at both ends of the buffer
          unsigned نویسه *buffer = malloc(len + 4);
          unsigned نویسه *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (نویسه *)text, len, rect);

          اگر(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          وگرنه اگر(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          وگرنه
	  {
	    size_t i;
            برای(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          آزاد(buffer);
        }
      }
      وگرنه اگر(strstartswith(سطر, "?screen_cell ")) {
        أكد(screen);
        نویسه *linep = سطر + 12;
	صحیح i;
        VTermPos pos;
        VTermScreenCell cell;
        وقتی(linep[0] == ' ')
          linep++;
        اگر(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          برو abort_سطر;
        }
        اگر(!vterm_screen_get_cell(screen, pos, &cell))
          برو abort_سطر;
        printf("{");
        برای(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        اگر(cell.attrs.bold)      printf("B");
        اگر(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        اگر(cell.attrs.italic)    printf("I");
        اگر(cell.attrs.blink)     printf("K");
        اگر(cell.attrs.reverse)   printf("R");
        اگر(cell.attrs.font)      printf("F%d", cell.attrs.font);
        اگر(cell.attrs.small)     printf("S");
        اگر(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        اگر(cell.attrs.dwl)       printf("dwl ");
        اگر(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      وگرنه اگر(strstartswith(سطر, "?screen_eol ")) {
        أكد(screen);
        VTermPos pos;
        نویسه *linep = سطر + 12;
        وقتی(linep[0] == ' ')
          linep++;
        اگر(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          برو abort_سطر;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      وگرنه اگر(strstartswith(سطر, "?screen_attrs_extent ")) {
        أكد(screen);
        VTermPos pos;
        VTermRect rect;
        نویسه *linep = سطر + 21;
        وقتی(linep[0] == ' ')
          linep++;
        اگر(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          برو abort_سطر;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        اگر(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          برو abort_سطر;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      وگرنه
        printf("?\n");

      memset(سطر, 0, اندازه سطر);
      ادامه;
    }

    وگرنه
      abort_سطر: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    اگر(outlen > 0) {
      نویسه outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, پوچ);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  بازگشت 0;
}
