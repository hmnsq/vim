#अन्तर्भाव "vterm.h"
#अन्तर्भाव "../src/vterm_internal.h" // We pull in some internal bits too

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

स्थिर size_t inplace_hex2bytes(वर्ण *s)
{
  वर्ण *inpos = s, *outpos = s;

  याव(*inpos) {
    unsigned पूर्णाङ्क ch;
    सचे(sscanf(inpos, "%2x", &ch) < 1)
      भङ्ग;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  पटिनिवत्त outpos - s;
}

स्थिर VTermModifier strpe_modifiers(वर्ण **strp)
{
  VTermModifier state = 0;

  याव((*strp)[0]) {
    विकल्प(((*strp)++)[0]) {
      पक्ष 'S': state |= VTERM_MOD_SHIFT; भङ्ग;
      पक्ष 'C': state |= VTERM_MOD_CTRL;  भङ्ग;
      पक्ष 'A': state |= VTERM_MOD_ALT;   भङ्ग;
      स्वभाव: पटिनिवत्त state;
    }
  }

  पटिनिवत्त state;
}

स्थिर VTerm_कुञ्चिका strp_कुञ्चिका(वर्ण *str)
{
  स्थिर संरचना {
    वर्ण *name;
    VTerm_कुञ्चिका कुञ्चिका;
  } कुञ्चिकाः[] = {
    { "Up",    VTERM_कुञ्चिका_UP },
    { "Tab",   VTERM_कुञ्चिका_TAB },
    { "Enter", VTERM_कुञ्चिका_ENTER },
    { "KP0",   VTERM_कुञ्चिका_KP_0 },
    { "F1",    VTERM_कुञ्चिका_FUNCTION(1) },
    { रिक्त,    VTERM_कुञ्चिका_NONE },
  };
  पूर्णाङ्क i;

  अत्थाय(i = 0; कुञ्चिकाः[i].name; i++) {
    सचे(streq(str, कुञ्चिकाः[i].name))
      पटिनिवत्त कुञ्चिकाः[i].कुञ्चिका;
  }

  पटिनिवत्त VTERM_कुञ्चिका_NONE;
}

स्थिर शून्य print_color(ध्रुव VTermColor *col)
{
  सचे (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  अञ्ञथा सचे (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  अञ्ञथा {
    printf("invalid(%d", col->type);
  }
  सचे (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  सचे (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

स्थिर VTermColor strpe_color(वर्ण **strp)
{
  uint8_t r, g, b, idx;
  पूर्णाङ्क len = 0;
  VTermColor col;

  सचे(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  अञ्ञथा सचे(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  अञ्ञथा
    vterm_color_rgb(&col, 127, 127, 127);

  पटिनिवत्त col;
}

स्थिर VTerm *vt;
स्थिर VTermState *state;
स्थिर VTermScreen *screen;

स्थिर VTermEncodingInstance encoding;

स्थिर शून्य term_output(ध्रुव वर्ण *s, size_t len, शून्य *user UNUSED)
{
  size_t i;

  printf("output ");
  अत्थाय(i = 0; i < len; i++)
    printf("%x%s", (unsigned वर्ण)s[i], i < len-1 ? "," : "\n");
}

स्थिर शून्य printhex(ध्रुव वर्ण *s, size_t len)
{
  याव(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

स्थिर पूर्णाङ्क parser_text(ध्रुव वर्ण bytes[], size_t len, शून्य *user UNUSED)
{
  size_t i;

  printf("text ");
  अत्थाय(i = 0; i < len; i++) {
    unsigned वर्ण b = bytes[i];
    सचे(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      भङ्ग;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  पटिनिवत्त i;
}

स्थिर पूर्णाङ्क parser_control(unsigned वर्ण control, शून्य *user UNUSED)
{
  printf("control %02x\n", control);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क parser_escape(ध्रुव वर्ण bytes[], size_t len, शून्य *user UNUSED)
{
  सचे(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    सचे(len < 2)
      पटिनिवत्त -1;
    len = 2;
  }
  अञ्ञथा {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  पटिनिवत्त len;
}

स्थिर पूर्णाङ्क parser_csi(ध्रुव वर्ण *leader, ध्रुव long args[], पूर्णाङ्क argcount, ध्रुव वर्ण *intermed, वर्ण command, शून्य *user UNUSED)
{
  पूर्णाङ्क i;
  printf("csi %02x", command);

  सचे(leader && leader[0]) {
    printf(" L=");
    अत्थाय(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  अत्थाय(i = 0; i < argcount; i++) {
    वर्ण sep = i ? ',' : ' ';

    सचे(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    अञ्ञथा
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  सचे(intermed && intermed[0]) {
    printf(" I=");
    अत्थाय(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क parser_osc(पूर्णाङ्क command, VTermStringFragment frag, शून्य *user UNUSED)
{

  printf("osc ");

  सचे(frag.initial) {
    सचे(command == -1)
      printf("[");
    अञ्ञथा
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  सचे(frag.final)
    printf("]");

  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क parser_dcs(ध्रुव वर्ण *command, size_t commandlen, VTermStringFragment frag, शून्य *user UNUSED)
{
  printf("dcs ");

  सचे(frag.initial) {
    size_t i;
    printf("[");
    अत्थाय(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  सचे(frag.final)
    printf("]");

  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क parser_apc(VTermStringFragment frag, शून्य *user UNUSED)
{
  printf("apc ");

  सचे(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  सचे(frag.final)
    printf("]");

  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क parser_pm(VTermStringFragment frag, शून्य *user UNUSED)
{
  printf("pm ");

  सचे(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  सचे(frag.final)
    printf("]");

  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क parser_sos(VTermStringFragment frag, शून्य *user UNUSED)
{
  printf("sos ");

  सचे(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  सचे(frag.final)
    printf("]");

  printf("\n");

  पटिनिवत्त 1;
}

स्थिर VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  रिक्त // resize
};

स्थिर VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

स्थिर पूर्णाङ्क want_movecursor = 0;
स्थिर VTermPos state_pos;
स्थिर पूर्णाङ्क movecursor(VTermPos pos, VTermPos oldpos UNUSED, पूर्णाङ्क visible UNUSED, शून्य *user UNUSED)
{
  state_pos = pos;

  सचे(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क want_premove = 0;
स्थिर पूर्णाङ्क premove(VTermRect rect, शून्य *user UNUSED)
{
  सचे(!want_premove)
    पटिनिवत्त 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क want_scrollrect = 0;
स्थिर पूर्णाङ्क scrollrect(VTermRect rect, पूर्णाङ्क downward, पूर्णाङ्क rightward, शून्य *user UNUSED)
{
  सचे(!want_scrollrect)
    पटिनिवत्त 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क want_moverect = 0;
स्थिर पूर्णाङ्क moverect(VTermRect dest, VTermRect src, शून्य *user UNUSED)
{
  सचे(!want_moverect)
    पटिनिवत्त 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क want_settermprop = 0;
स्थिर पूर्णाङ्क settermprop(VTermProp prop, VTerm_मानम् *val, शून्य *user UNUSED)
{
  VTerm_मानम्_Type type;
  सचे(!want_settermprop)
    पटिनिवत्त 1;

  type = vterm_get_prop_type(prop);
  विकल्प(type) {
  पक्ष VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    पटिनिवत्त 1;
  पक्ष VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->सङ्ख्या);
    पटिनिवत्त 1;
  पक्ष VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    पटिनिवत्त 1;
  पक्ष VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    पटिनिवत्त 1;

  पक्ष VTERM_N_VALUETYPES:
    पटिनिवत्त 0;
  }

  पटिनिवत्त 0;
}

// These callbacks are for State

स्थिर पूर्णाङ्क want_state_putglyph = 0;
स्थिर पूर्णाङ्क state_putglyph(VTermGlyphInfo *info, VTermPos pos, शून्य *user UNUSED)
{
  पूर्णाङ्क i;
  सचे(!want_state_putglyph)
    पटिनिवत्त 1;

  printf("putglyph ");
  अत्थाय(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  सचे(info->protected_cell)
    printf(" prot");
  सचे(info->dwl)
    printf(" dwl");
  सचे(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क want_state_erase = 0;
स्थिर पूर्णाङ्क state_erase(VTermRect rect, पूर्णाङ्क selective, शून्य *user UNUSED)
{
  सचे(!want_state_erase)
    पटिनिवत्त 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  पटिनिवत्त 1;
}

स्थिर संरचना {
  पूर्णाङ्क bold;
  पूर्णाङ्क underline;
  पूर्णाङ्क italic;
  पूर्णाङ्क blink;
  पूर्णाङ्क reverse;
  पूर्णाङ्क conceal;
  पूर्णाङ्क strike;
  पूर्णाङ्क font;
  पूर्णाङ्क small;
  पूर्णाङ्क baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
स्थिर पूर्णाङ्क state_setpenattr(VTermAttr attr, VTerm_मानम् *val, शून्य *user UNUSED)
{
  विकल्प(attr) {
  पक्ष VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->सङ्ख्या;
    भङ्ग;
  पक्ष VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_FONT:
    state_pen.font = val->सङ्ख्या;
    भङ्ग;
  पक्ष VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_BASELINE:
    state_pen.baseline = val->सङ्ख्या;
    भङ्ग;
  पक्ष VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    भङ्ग;
  पक्ष VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    भङ्ग;

  पक्ष VTERM_N_ATTRS:
    पटिनिवत्त 0;
  }

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क state_setlineinfo(पूर्णाङ्क row UNUSED, ध्रुव VTerm_पङ्क्तिः_Info *newinfo UNUSED, ध्रुव VTerm_पङ्क्तिः_Info *oldinfo UNUSED, शून्य *user UNUSED)
{
  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क want_state_scrollback = 0;
स्थिर पूर्णाङ्क state_sb_clear(शून्य *user UNUSED) {
  सचे(!want_state_scrollback)
    पटिनिवत्त 1;

  printf("sb_clear\n");
  पटिनिवत्त 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  रिक्त, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  रिक्त, // bell
  रिक्त, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

स्थिर पूर्णाङ्क selection_set(VTermSelectionMask mask, VTermStringFragment frag, शून्य *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  सचे(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  सचे(frag.final)
    printf("]");
  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क selection_query(VTermSelectionMask mask, शून्य *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  पटिनिवत्त 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

स्थिर पूर्णाङ्क want_screen_damage = 0;
स्थिर पूर्णाङ्क want_screen_damage_cells = 0;
स्थिर पूर्णाङ्क screen_damage(VTermRect rect, शून्य *user UNUSED)
{
  सचे(!want_screen_damage)
    पटिनिवत्त 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  सचे(want_screen_damage_cells) {
    पूर्णाङ्क equals = FALSE;
    पूर्णाङ्क row;
    पूर्णाङ्क col;

    अत्थाय(row = rect.start_row; row < rect.end_row; row++) {
      पूर्णाङ्क eol = rect.end_col;
      याव(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        सचे(cell.chars[0])
          भङ्ग;

        eol--;
      }

      सचे(eol == rect.start_col)
        भङ्ग;

      सचे(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      अत्थाय(col = rect.start_col; col < eol; col++) {
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

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क want_screen_scrollback = 0;
स्थिर पूर्णाङ्क screen_sb_pushline4(पूर्णाङ्क cols, ध्रुव VTermScreenCell *cells, पूर्णाङ्क continuation, शून्य *user UNUSED)
{
  पूर्णाङ्क eol;
  पूर्णाङ्क c;

  सचे(!want_screen_scrollback)
    पटिनिवत्त 1;

  eol = cols;
  याव(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  अत्थाय(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क screen_sb_popline(पूर्णाङ्क cols, VTermScreenCell *cells, शून्य *user UNUSED)
{
  पूर्णाङ्क col;

  सचे(!want_screen_scrollback)
    पटिनिवत्त 0;

  // All lines of scrollback contain "ABCDE"
  अत्थाय(col = 0; col < cols; col++) {
    सचे(col < 5)
      cells[col].chars[0] = 'A' + col;
    अञ्ञथा
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क screen_sb_clear(शून्य *user UNUSED)
{
  सचे(!want_screen_scrollback)
    पटिनिवत्त 1;

  printf("sb_clear\n");
  पटिनिवत्त 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  रिक्त, // bell
  रिक्त, // resize
  रिक्त, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

पूर्णाङ्क main(पूर्णाङ्क argc UNUSED, वर्ण **argv UNUSED)
{
  वर्ण पङ्क्तिः[1024] = {0};
  पूर्णाङ्क flag;

  पूर्णाङ्क err;

  setvbuf(stdout, रिक्त, _IONBF, 0);

  याव(fgets(पङ्क्तिः, परिमाण पङ्क्तिः, stdin)) {
    वर्ण *nl;
    size_t outlen;
    err = 0;

    सचे((nl = strchr(पङ्क्तिः, '\n')))
      *nl = '\0';

    सचे(streq(पङ्क्तिः, "INIT")) {
      सचे(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    अञ्ञथा सचे(streq(पङ्क्तिः, "WANTPARSER")) {
      प्रतिज्ञा(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, रिक्त);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "WANTSTATE") && (पङ्क्तिः[9] == '\0' || पङ्क्तिः[9] == ' ')) {
      पूर्णाङ्क i = 9;
      पूर्णाङ्क sense = 1;
      प्रतिज्ञा(vt);
      सचे(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, रिक्त);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, रिक्त, रिक्त, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      याव(पङ्क्तिः[i] == ' ')
        i++;
      अत्थाय( ; पङ्क्तिः[i]; i++)
        विकल्प(पङ्क्तिः[i]) {
        पक्ष '+':
          sense = 1;
          भङ्ग;
        पक्ष '-':
          sense = 0;
          भङ्ग;
        पक्ष 'g':
          want_state_putglyph = sense;
          भङ्ग;
        पक्ष 's':
          want_scrollrect = sense;
          भङ्ग;
        पक्ष 'P':
          want_premove = sense;
          भङ्ग;
        पक्ष 'm':
          want_moverect = sense;
          भङ्ग;
        पक्ष 'e':
          want_state_erase = sense;
          भङ्ग;
        पक्ष 'p':
          want_settermprop = sense;
          भङ्ग;
        पक्ष 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : रिक्त, रिक्त);
          भङ्ग;
        पक्ष 'b':
          want_state_scrollback = sense;
          भङ्ग;
        स्वभाव:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", पङ्क्तिः[i]);
        }
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "WANTSCREEN") && (पङ्क्तिः[10] == '\0' || पङ्क्तिः[10] == ' ')) {
      पूर्णाङ्क i = 10;
      पूर्णाङ्क sense = 1;
      प्रतिज्ञा(vt);
      सचे(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, रिक्त);
      vterm_screen_callbacks_has_pushline4(screen);

      याव(पङ्क्तिः[i] == ' ')
        i++;
      अत्थाय( ; पङ्क्तिः[i]; i++)
        विकल्प(पङ्क्तिः[i]) {
        पक्ष '-':
          sense = 0;
          भङ्ग;
        पक्ष 'a':
          vterm_screen_enable_altscreen(screen, 1);
          भङ्ग;
        पक्ष 'd':
          want_screen_damage = sense;
          भङ्ग;
        पक्ष 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          भङ्ग;
        पक्ष 'm':
          want_moverect = sense;
          भङ्ग;
        पक्ष 'c':
          want_movecursor = sense;
          भङ्ग;
        पक्ष 'p':
          want_settermprop = 1;
          भङ्ग;
        पक्ष 'b':
          want_screen_scrollback = sense;
          भङ्ग;
        पक्ष 'r':
          vterm_screen_enable_reflow(screen, sense);
          भङ्ग;
        स्वभाव:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", पङ्क्तिः[i]);
        }
    }

    अञ्ञथा सचे(sscanf(पङ्क्तिः, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    अञ्ञथा सचे(streq(पङ्क्तिः, "RESET")) {
      सचे(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      सचे(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "RESIZE ")) {
      पूर्णाङ्क rows, cols;
      वर्ण *linep = पङ्क्तिः + 7;
      याव(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "PUSH ")) {
      वर्ण *bytes = पङ्क्तिः + 5;
      size_t len = inplace_hex2bytes(bytes);
      प्रतिज्ञा(len);

      size_t written = vterm_निवेशः_write(vt, bytes, len);
      सचे(written < len)
        fprintf(stderr, "! short write\n");
    }

    अञ्ञथा सचे(streq(पङ्क्तिः, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      सचे(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "ENCIN ")) {
      वर्ण *bytes = पङ्क्तिः + 6;
      size_t len = inplace_hex2bytes(bytes);
      प्रतिज्ञा(len);

      uint32_t cp[1024];
      पूर्णाङ्क cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      सचे(cpi > 0) {
	पूर्णाङ्क i;
        printf("encout ");
        अत्थाय(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "INCHAR ")) {
      वर्ण *linep = पङ्क्तिः + 7;
      unsigned पूर्णाङ्क c = 0;
      VTermModifier mod;
      याव(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "INKEY ")) {
      VTermModifier mod;
      VTerm_कुञ्चिका कुञ्चिका;
      वर्ण *linep = पङ्क्तिः + 6;
      याव(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      याव(linep[0] == ' ')
        linep++;
      कुञ्चिका = strp_कुञ्चिका(linep);

      vterm_keyboard_कुञ्चिका(vt, कुञ्चिका, mod);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "PASTE ")) {
      वर्ण *linep = पङ्क्तिः + 6;
      सचे(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      अञ्ञथा सचे(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      अञ्ञथा
        गच्छ abort_पङ्क्तिः;
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "FOCUS ")) {
      प्रतिज्ञा(state);
      वर्ण *linep = पङ्क्तिः + 6;
      सचे(streq(linep, "IN"))
        vterm_state_focus_in(state);
      अञ्ञथा सचे(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      अञ्ञथा
        गच्छ abort_पङ्क्तिः;
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "MOUSEMOVE ")) {
      वर्ण *linep = पङ्क्तिः + 10;
      पूर्णाङ्क row, col, len;
      VTermModifier mod;
      याव(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      याव(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "MOUSEBTN ")) {
      वर्ण *linep = पङ्क्तिः + 9;
      वर्ण press;
      पूर्णाङ्क button, len;
      VTermModifier mod;
      याव(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      याव(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "SELECTION ")) {
      वर्ण *linep = पङ्क्तिः + 10;
      unsigned पूर्णाङ्क mask;
      पूर्णाङ्क len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      याव(linep[0] == ' ')
        linep++;
      सचे(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        याव(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      प्रतिज्ञा(frag.len);

      linep += frag.len * 2;
      याव(linep[0] == ' ')
        linep++;
      सचे(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "DAMAGEMERGE ")) {
      प्रतिज्ञा(screen);
      वर्ण *linep = पङ्क्तिः + 12;
      याव(linep[0] == ' ')
        linep++;
      सचे(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      अञ्ञथा सचे(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      अञ्ञथा सचे(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      अञ्ञथा सचे(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "DAMAGEFLUSH")) {
      प्रतिज्ञा(screen);
      vterm_screen_flush_damage(screen);
    }

    अञ्ञथा सचे(strstartswith(पङ्क्तिः, "SETDEFAULTCOL ")) {
      प्रतिज्ञा(screen);
      वर्ण *linep = पङ्क्तिः + 14;
      याव(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      सचे(linep[0]) {
        याव(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      अञ्ञथा
        vterm_screen_set_default_colors(screen, &fg, रिक्त);
    }

    अञ्ञथा सचे(पङ्क्तिः[0] == '?') {
      सचे(streq(पङ्क्तिः, "?cursor")) {
        प्रतिज्ञा(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        सचे(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        अञ्ञथा सचे(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        अञ्ञथा
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      अञ्ञथा सचे(strstartswith(पङ्क्तिः, "?pen ")) {
        प्रतिज्ञा(state);
        VTerm_मानम् val;
        वर्ण *linep = पङ्क्तिः + 5;
        याव(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        सचे(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          सचे(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          अञ्ञथा
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        अञ्ञथा सचे(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          सचे(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          अञ्ञथा
            printf("%d\n", state_pen.underline);
        }
        अञ्ञथा सचे(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          सचे(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          अञ्ञथा
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        अञ्ञथा सचे(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          सचे(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          अञ्ञथा
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        अञ्ञथा सचे(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          सचे(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          अञ्ञथा
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        अञ्ञथा सचे(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          सचे(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          अञ्ञथा
            printf("%d\n", state_pen.font);
        }
        अञ्ञथा सचे(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          सचे(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          अञ्ञथा
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        अञ्ञथा सचे(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          सचे(val.सङ्ख्या != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.सङ्ख्या, state_pen.baseline);
          अञ्ञथा
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        अञ्ञथा सचे(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        अञ्ञथा सचे(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        अञ्ञथा
          printf("?\n");
      }
      अञ्ञथा सचे(strstartswith(पङ्क्तिः, "?lineinfo ")) {
        प्रतिज्ञा(state);
        वर्ण *linep = पङ्क्तिः + 10;
        पूर्णाङ्क row;
        ध्रुव VTerm_पङ्क्तिः_Info *info;
        याव(linep[0] == ' ')
          linep++;
        सचे(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          गच्छ abort_पङ्क्तिः;
        }
        info = vterm_state_get_lineinfo(state, row);
        सचे(info->doublewidth)
          printf("dwl ");
        सचे(info->doubleheight)
          printf("dhl ");
        सचे(info->continuation)
          printf("cont ");
        printf("\n");
      }
      अञ्ञथा सचे(strstartswith(पङ्क्तिः, "?screen_chars ")) {
        प्रतिज्ञा(screen);
        वर्ण *linep = पङ्क्तिः + 13;
        VTermRect rect;
        size_t len;
        याव(linep[0] == ' ')
          linep++;
        सचे(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        अञ्ञथा सचे(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, रिक्त, &rect.end_col);
        }
        अञ्ञथा {
          printf("! screen_chars unrecognised input\n");
          गच्छ abort_पङ्क्तिः;
        }
        len = vterm_screen_get_chars(screen, रिक्त, 0, rect);
        सचे(len == (size_t)-1)
          printf("! screen_chars error\n");
        अञ्ञथा सचे(len == 0)
          printf("\n");
        अञ्ञथा {
          uint32_t *chars = malloc(परिमाण(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          अत्थाय(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          मुक्त(chars);
        }
      }
      अञ्ञथा सचे(strstartswith(पङ्क्तिः, "?screen_text ")) {
        प्रतिज्ञा(screen);
        वर्ण *linep = पङ्क्तिः + 12;
        VTermRect rect;
        size_t len;
        याव(linep[0] == ' ')
          linep++;
        सचे(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          गच्छ abort_पङ्क्तिः;
        }
        len = vterm_screen_get_text(screen, रिक्त, 0, rect);
        सचे(len == (size_t)-1)
          printf("! screen_text error\n");
        अञ्ञथा सचे(len == 0)
          printf("\n");
        अञ्ञथा {
          // Put an overwrite guard at both ends of the buffer
          unsigned वर्ण *buffer = malloc(len + 4);
          unsigned वर्ण *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (वर्ण *)text, len, rect);

          सचे(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          अञ्ञथा सचे(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          अञ्ञथा
	  {
	    size_t i;
            अत्थाय(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          मुक्त(buffer);
        }
      }
      अञ्ञथा सचे(strstartswith(पङ्क्तिः, "?screen_cell ")) {
        प्रतिज्ञा(screen);
        वर्ण *linep = पङ्क्तिः + 12;
	पूर्णाङ्क i;
        VTermPos pos;
        VTermScreenCell cell;
        याव(linep[0] == ' ')
          linep++;
        सचे(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          गच्छ abort_पङ्क्तिः;
        }
        सचे(!vterm_screen_get_cell(screen, pos, &cell))
          गच्छ abort_पङ्क्तिः;
        printf("{");
        अत्थाय(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        सचे(cell.attrs.bold)      printf("B");
        सचे(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        सचे(cell.attrs.italic)    printf("I");
        सचे(cell.attrs.blink)     printf("K");
        सचे(cell.attrs.reverse)   printf("R");
        सचे(cell.attrs.font)      printf("F%d", cell.attrs.font);
        सचे(cell.attrs.small)     printf("S");
        सचे(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        सचे(cell.attrs.dwl)       printf("dwl ");
        सचे(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      अञ्ञथा सचे(strstartswith(पङ्क्तिः, "?screen_eol ")) {
        प्रतिज्ञा(screen);
        VTermPos pos;
        वर्ण *linep = पङ्क्तिः + 12;
        याव(linep[0] == ' ')
          linep++;
        सचे(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          गच्छ abort_पङ्क्तिः;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      अञ्ञथा सचे(strstartswith(पङ्क्तिः, "?screen_attrs_extent ")) {
        प्रतिज्ञा(screen);
        VTermPos pos;
        VTermRect rect;
        वर्ण *linep = पङ्क्तिः + 21;
        याव(linep[0] == ' ')
          linep++;
        सचे(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          गच्छ abort_पङ्क्तिः;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        सचे(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          गच्छ abort_पङ्क्तिः;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      अञ्ञथा
        printf("?\n");

      memset(पङ्क्तिः, 0, परिमाण पङ्क्तिः);
      अनुवर्त;
    }

    अञ्ञथा
      abort_पङ्क्तिः: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    सचे(outlen > 0) {
      वर्ण outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, रिक्त);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  पटिनिवत्त 0;
}
