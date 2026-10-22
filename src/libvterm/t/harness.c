#অন্তর্ভাব "vterm.h"
#অন্তর্ভাব "../src/vterm_internal.h" // We pull in some internal bits too

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

স্থির size_t inplace_hex2bytes(অক্ষর *s)
{
  অক্ষর *inpos = s, *outpos = s;

  যখন(*inpos) {
    unsigned পূর্ণসংখ্যা ch;
    যদি(sscanf(inpos, "%2x", &ch) < 1)
      বিরতি;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  ফেরত outpos - s;
}

স্থির VTermModifier strpe_modifiers(অক্ষর **strp)
{
  VTermModifier state = 0;

  যখন((*strp)[0]) {
    নির্বাচন(((*strp)++)[0]) {
      ক্ষেত্র 'S': state |= VTERM_MOD_SHIFT; বিরতি;
      ক্ষেত্র 'C': state |= VTERM_MOD_CTRL;  বিরতি;
      ক্ষেত্র 'A': state |= VTERM_MOD_ALT;   বিরতি;
      পূর্বনির্ধারিত: ফেরত state;
    }
  }

  ফেরত state;
}

স্থির VTerm_চাবি strp_চাবি(অক্ষর *str)
{
  স্থির কাঠামো {
    অক্ষর *name;
    VTerm_চাবি চাবি;
  } চাবিগুলি[] = {
    { "Up",    VTERM_চাবি_UP },
    { "Tab",   VTERM_চাবি_TAB },
    { "Enter", VTERM_চাবি_ENTER },
    { "KP0",   VTERM_চাবি_KP_0 },
    { "F1",    VTERM_চাবি_FUNCTION(1) },
    { ফাঁকা,    VTERM_চাবি_NONE },
  };
  পূর্ণসংখ্যা i;

  জন্য(i = 0; চাবিগুলি[i].name; i++) {
    যদি(streq(str, চাবিগুলি[i].name))
      ফেরত চাবিগুলি[i].চাবি;
  }

  ফেরত VTERM_চাবি_NONE;
}

স্থির শূন্য print_color(ধ্রুবক VTermColor *col)
{
  যদি (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  অন্যথা যদি (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->সূচক);
  }
  অন্যথা {
    printf("invalid(%d", col->type);
  }
  যদি (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  যদি (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

স্থির VTermColor strpe_color(অক্ষর **strp)
{
  uint8_t r, g, b, idx;
  পূর্ণসংখ্যা len = 0;
  VTermColor col;

  যদি(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  অন্যথা যদি(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  অন্যথা
    vterm_color_rgb(&col, 127, 127, 127);

  ফেরত col;
}

স্থির VTerm *vt;
স্থির VTermState *state;
স্থির VTermScreen *screen;

স্থির VTermEncodingInstance encoding;

স্থির শূন্য term_output(ধ্রুবক অক্ষর *s, size_t len, শূন্য *user UNUSED)
{
  size_t i;

  printf("output ");
  জন্য(i = 0; i < len; i++)
    printf("%x%s", (unsigned অক্ষর)s[i], i < len-1 ? "," : "\n");
}

স্থির শূন্য printhex(ধ্রুবক অক্ষর *s, size_t len)
{
  যখন(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

স্থির পূর্ণসংখ্যা parser_text(ধ্রুবক অক্ষর bytes[], size_t len, শূন্য *user UNUSED)
{
  size_t i;

  printf("text ");
  জন্য(i = 0; i < len; i++) {
    unsigned অক্ষর b = bytes[i];
    যদি(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      বিরতি;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  ফেরত i;
}

স্থির পূর্ণসংখ্যা parser_control(unsigned অক্ষর control, শূন্য *user UNUSED)
{
  printf("control %02x\n", control);

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা parser_escape(ধ্রুবক অক্ষর bytes[], size_t len, শূন্য *user UNUSED)
{
  যদি(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    যদি(len < 2)
      ফেরত -1;
    len = 2;
  }
  অন্যথা {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  ফেরত len;
}

স্থির পূর্ণসংখ্যা parser_csi(ধ্রুবক অক্ষর *leader, ধ্রুবক long args[], পূর্ণসংখ্যা argcount, ধ্রুবক অক্ষর *intermed, অক্ষর command, শূন্য *user UNUSED)
{
  পূর্ণসংখ্যা i;
  printf("csi %02x", command);

  যদি(leader && leader[0]) {
    printf(" L=");
    জন্য(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  জন্য(i = 0; i < argcount; i++) {
    অক্ষর sep = i ? ',' : ' ';

    যদি(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    অন্যথা
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  যদি(intermed && intermed[0]) {
    printf(" I=");
    জন্য(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা parser_osc(পূর্ণসংখ্যা command, VTermStringFragment frag, শূন্য *user UNUSED)
{

  printf("osc ");

  যদি(frag.initial) {
    যদি(command == -1)
      printf("[");
    অন্যথা
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  যদি(frag.final)
    printf("]");

  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা parser_dcs(ধ্রুবক অক্ষর *command, size_t commandlen, VTermStringFragment frag, শূন্য *user UNUSED)
{
  printf("dcs ");

  যদি(frag.initial) {
    size_t i;
    printf("[");
    জন্য(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  যদি(frag.final)
    printf("]");

  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা parser_apc(VTermStringFragment frag, শূন্য *user UNUSED)
{
  printf("apc ");

  যদি(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  যদি(frag.final)
    printf("]");

  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা parser_pm(VTermStringFragment frag, শূন্য *user UNUSED)
{
  printf("pm ");

  যদি(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  যদি(frag.final)
    printf("]");

  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা parser_sos(VTermStringFragment frag, শূন্য *user UNUSED)
{
  printf("sos ");

  যদি(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  যদি(frag.final)
    printf("]");

  printf("\n");

  ফেরত 1;
}

স্থির VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ফাঁকা // resize
};

স্থির VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

স্থির পূর্ণসংখ্যা want_movecursor = 0;
স্থির VTermPos state_pos;
স্থির পূর্ণসংখ্যা movecursor(VTermPos pos, VTermPos oldpos UNUSED, পূর্ণসংখ্যা visible UNUSED, শূন্য *user UNUSED)
{
  state_pos = pos;

  যদি(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা want_premove = 0;
স্থির পূর্ণসংখ্যা premove(VTermRect rect, শূন্য *user UNUSED)
{
  যদি(!want_premove)
    ফেরত 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা want_scrollrect = 0;
স্থির পূর্ণসংখ্যা scrollrect(VTermRect rect, পূর্ণসংখ্যা downward, পূর্ণসংখ্যা rightward, শূন্য *user UNUSED)
{
  যদি(!want_scrollrect)
    ফেরত 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা want_moverect = 0;
স্থির পূর্ণসংখ্যা moverect(VTermRect dest, VTermRect src, শূন্য *user UNUSED)
{
  যদি(!want_moverect)
    ফেরত 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা want_settermprop = 0;
স্থির পূর্ণসংখ্যা settermprop(VTermProp prop, VTerm_মান *val, শূন্য *user UNUSED)
{
  VTerm_মান_Type type;
  যদি(!want_settermprop)
    ফেরত 1;

  type = vterm_get_prop_type(prop);
  নির্বাচন(type) {
  ক্ষেত্র VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    ফেরত 1;
  ক্ষেত্র VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->সংখ্যা);
    ফেরত 1;
  ক্ষেত্র VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    ফেরত 1;
  ক্ষেত্র VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    ফেরত 1;

  ক্ষেত্র VTERM_N_VALUETYPES:
    ফেরত 0;
  }

  ফেরত 0;
}

// These callbacks are for State

স্থির পূর্ণসংখ্যা want_state_putglyph = 0;
স্থির পূর্ণসংখ্যা state_putglyph(VTermGlyphInfo *info, VTermPos pos, শূন্য *user UNUSED)
{
  পূর্ণসংখ্যা i;
  যদি(!want_state_putglyph)
    ফেরত 1;

  printf("putglyph ");
  জন্য(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  যদি(info->protected_cell)
    printf(" prot");
  যদি(info->dwl)
    printf(" dwl");
  যদি(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা want_state_erase = 0;
স্থির পূর্ণসংখ্যা state_erase(VTermRect rect, পূর্ণসংখ্যা selective, শূন্য *user UNUSED)
{
  যদি(!want_state_erase)
    ফেরত 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  ফেরত 1;
}

স্থির কাঠামো {
  পূর্ণসংখ্যা bold;
  পূর্ণসংখ্যা underline;
  পূর্ণসংখ্যা italic;
  পূর্ণসংখ্যা blink;
  পূর্ণসংখ্যা reverse;
  পূর্ণসংখ্যা conceal;
  পূর্ণসংখ্যা strike;
  পূর্ণসংখ্যা font;
  পূর্ণসংখ্যা small;
  পূর্ণসংখ্যা baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
স্থির পূর্ণসংখ্যা state_setpenattr(VTermAttr attr, VTerm_মান *val, শূন্য *user UNUSED)
{
  নির্বাচন(attr) {
  ক্ষেত্র VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->সংখ্যা;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_FONT:
    state_pen.font = val->সংখ্যা;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_BASELINE:
    state_pen.baseline = val->সংখ্যা;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    বিরতি;
  ক্ষেত্র VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    বিরতি;

  ক্ষেত্র VTERM_N_ATTRS:
    ফেরত 0;
  }

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা state_setlineinfo(পূর্ণসংখ্যা row UNUSED, ধ্রুবক VTerm_সারি_Info *newinfo UNUSED, ধ্রুবক VTerm_সারি_Info *oldinfo UNUSED, শূন্য *user UNUSED)
{
  ফেরত 1;
}

স্থির পূর্ণসংখ্যা want_state_scrollback = 0;
স্থির পূর্ণসংখ্যা state_sb_clear(শূন্য *user UNUSED) {
  যদি(!want_state_scrollback)
    ফেরত 1;

  printf("sb_clear\n");
  ফেরত 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ফাঁকা, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ফাঁকা, // bell
  ফাঁকা, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

স্থির পূর্ণসংখ্যা selection_set(VTermSelectionMask mask, VTermStringFragment frag, শূন্য *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  যদি(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  যদি(frag.final)
    printf("]");
  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা selection_query(VTermSelectionMask mask, শূন্য *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  ফেরত 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

স্থির পূর্ণসংখ্যা want_screen_damage = 0;
স্থির পূর্ণসংখ্যা want_screen_damage_cells = 0;
স্থির পূর্ণসংখ্যা screen_damage(VTermRect rect, শূন্য *user UNUSED)
{
  যদি(!want_screen_damage)
    ফেরত 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  যদি(want_screen_damage_cells) {
    পূর্ণসংখ্যা equals = FALSE;
    পূর্ণসংখ্যা row;
    পূর্ণসংখ্যা col;

    জন্য(row = rect.start_row; row < rect.end_row; row++) {
      পূর্ণসংখ্যা eol = rect.end_col;
      যখন(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        যদি(cell.chars[0])
          বিরতি;

        eol--;
      }

      যদি(eol == rect.start_col)
        বিরতি;

      যদি(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      জন্য(col = rect.start_col; col < eol; col++) {
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

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা want_screen_scrollback = 0;
স্থির পূর্ণসংখ্যা screen_sb_pushline4(পূর্ণসংখ্যা cols, ধ্রুবক VTermScreenCell *cells, পূর্ণসংখ্যা continuation, শূন্য *user UNUSED)
{
  পূর্ণসংখ্যা eol;
  পূর্ণসংখ্যা c;

  যদি(!want_screen_scrollback)
    ফেরত 1;

  eol = cols;
  যখন(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  জন্য(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা screen_sb_popline(পূর্ণসংখ্যা cols, VTermScreenCell *cells, শূন্য *user UNUSED)
{
  পূর্ণসংখ্যা col;

  যদি(!want_screen_scrollback)
    ফেরত 0;

  // All lines of scrollback contain "ABCDE"
  জন্য(col = 0; col < cols; col++) {
    যদি(col < 5)
      cells[col].chars[0] = 'A' + col;
    অন্যথা
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  ফেরত 1;
}

স্থির পূর্ণসংখ্যা screen_sb_clear(শূন্য *user UNUSED)
{
  যদি(!want_screen_scrollback)
    ফেরত 1;

  printf("sb_clear\n");
  ফেরত 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ফাঁকা, // bell
  ফাঁকা, // resize
  ফাঁকা, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

পূর্ণসংখ্যা প্রধান(পূর্ণসংখ্যা argc UNUSED, অক্ষর **argv UNUSED)
{
  অক্ষর সারি[1024] = {0};
  পূর্ণসংখ্যা flag;

  পূর্ণসংখ্যা err;

  setvbuf(stdout, ফাঁকা, _IONBF, 0);

  যখন(fgets(সারি, আকার সারি, stdin)) {
    অক্ষর *nl;
    size_t outlen;
    err = 0;

    যদি((nl = strchr(সারি, '\n')))
      *nl = '\0';

    যদি(streq(সারি, "INIT")) {
      যদি(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    অন্যথা যদি(streq(সারি, "WANTPARSER")) {
      প্রতিজ্ঞা(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, ফাঁকা);
    }

    অন্যথা যদি(strstartswith(সারি, "WANTSTATE") && (সারি[9] == '\0' || সারি[9] == ' ')) {
      পূর্ণসংখ্যা i = 9;
      পূর্ণসংখ্যা sense = 1;
      প্রতিজ্ঞা(vt);
      যদি(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, ফাঁকা);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, ফাঁকা, ফাঁকা, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      যখন(সারি[i] == ' ')
        i++;
      জন্য( ; সারি[i]; i++)
        নির্বাচন(সারি[i]) {
        ক্ষেত্র '+':
          sense = 1;
          বিরতি;
        ক্ষেত্র '-':
          sense = 0;
          বিরতি;
        ক্ষেত্র 'g':
          want_state_putglyph = sense;
          বিরতি;
        ক্ষেত্র 's':
          want_scrollrect = sense;
          বিরতি;
        ক্ষেত্র 'P':
          want_premove = sense;
          বিরতি;
        ক্ষেত্র 'm':
          want_moverect = sense;
          বিরতি;
        ক্ষেত্র 'e':
          want_state_erase = sense;
          বিরতি;
        ক্ষেত্র 'p':
          want_settermprop = sense;
          বিরতি;
        ক্ষেত্র 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : ফাঁকা, ফাঁকা);
          বিরতি;
        ক্ষেত্র 'b':
          want_state_scrollback = sense;
          বিরতি;
        পূর্বনির্ধারিত:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", সারি[i]);
        }
    }

    অন্যথা যদি(strstartswith(সারি, "WANTSCREEN") && (সারি[10] == '\0' || সারি[10] == ' ')) {
      পূর্ণসংখ্যা i = 10;
      পূর্ণসংখ্যা sense = 1;
      প্রতিজ্ঞা(vt);
      যদি(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, ফাঁকা);
      vterm_screen_callbacks_has_pushline4(screen);

      যখন(সারি[i] == ' ')
        i++;
      জন্য( ; সারি[i]; i++)
        নির্বাচন(সারি[i]) {
        ক্ষেত্র '-':
          sense = 0;
          বিরতি;
        ক্ষেত্র 'a':
          vterm_screen_enable_altscreen(screen, 1);
          বিরতি;
        ক্ষেত্র 'd':
          want_screen_damage = sense;
          বিরতি;
        ক্ষেত্র 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          বিরতি;
        ক্ষেত্র 'm':
          want_moverect = sense;
          বিরতি;
        ক্ষেত্র 'c':
          want_movecursor = sense;
          বিরতি;
        ক্ষেত্র 'p':
          want_settermprop = 1;
          বিরতি;
        ক্ষেত্র 'b':
          want_screen_scrollback = sense;
          বিরতি;
        ক্ষেত্র 'r':
          vterm_screen_enable_reflow(screen, sense);
          বিরতি;
        পূর্বনির্ধারিত:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", সারি[i]);
        }
    }

    অন্যথা যদি(sscanf(সারি, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    অন্যথা যদি(streq(সারি, "RESET")) {
      যদি(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      যদি(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    অন্যথা যদি(strstartswith(সারি, "RESIZE ")) {
      পূর্ণসংখ্যা rows, cols;
      অক্ষর *linep = সারি + 7;
      যখন(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_আকার(vt, rows, cols);
    }

    অন্যথা যদি(strstartswith(সারি, "PUSH ")) {
      অক্ষর *bytes = সারি + 5;
      size_t len = inplace_hex2bytes(bytes);
      প্রতিজ্ঞা(len);

      size_t written = vterm_প্রবেশ_write(vt, bytes, len);
      যদি(written < len)
        fprintf(stderr, "! short write\n");
    }

    অন্যথা যদি(streq(সারি, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      যদি(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.তথ্য);
    }

    অন্যথা যদি(strstartswith(সারি, "ENCIN ")) {
      অক্ষর *bytes = সারি + 6;
      size_t len = inplace_hex2bytes(bytes);
      প্রতিজ্ঞা(len);

      uint32_t cp[1024];
      পূর্ণসংখ্যা cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.তথ্য,
          cp, &cpi, len, bytes, &pos, len);

      যদি(cpi > 0) {
	পূর্ণসংখ্যা i;
        printf("encout ");
        জন্য(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    অন্যথা যদি(strstartswith(সারি, "INCHAR ")) {
      অক্ষর *linep = সারি + 7;
      unsigned পূর্ণসংখ্যা c = 0;
      VTermModifier mod;
      যখন(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    অন্যথা যদি(strstartswith(সারি, "INKEY ")) {
      VTermModifier mod;
      VTerm_চাবি চাবি;
      অক্ষর *linep = সারি + 6;
      যখন(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      যখন(linep[0] == ' ')
        linep++;
      চাবি = strp_চাবি(linep);

      vterm_keyboard_চাবি(vt, চাবি, mod);
    }

    অন্যথা যদি(strstartswith(সারি, "PASTE ")) {
      অক্ষর *linep = সারি + 6;
      যদি(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      অন্যথা যদি(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      অন্যথা
        যাও abort_সারি;
    }

    অন্যথা যদি(strstartswith(সারি, "FOCUS ")) {
      প্রতিজ্ঞা(state);
      অক্ষর *linep = সারি + 6;
      যদি(streq(linep, "IN"))
        vterm_state_focus_in(state);
      অন্যথা যদি(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      অন্যথা
        যাও abort_সারি;
    }

    অন্যথা যদি(strstartswith(সারি, "MOUSEMOVE ")) {
      অক্ষর *linep = সারি + 10;
      পূর্ণসংখ্যা row, col, len;
      VTermModifier mod;
      যখন(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      যখন(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    অন্যথা যদি(strstartswith(সারি, "MOUSEBTN ")) {
      অক্ষর *linep = সারি + 9;
      অক্ষর press;
      পূর্ণসংখ্যা button, len;
      VTermModifier mod;
      যখন(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      যখন(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    অন্যথা যদি(strstartswith(সারি, "SELECTION ")) {
      অক্ষর *linep = সারি + 10;
      unsigned পূর্ণসংখ্যা mask;
      পূর্ণসংখ্যা len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      যখন(linep[0] == ' ')
        linep++;
      যদি(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        যখন(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      প্রতিজ্ঞা(frag.len);

      linep += frag.len * 2;
      যখন(linep[0] == ' ')
        linep++;
      যদি(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    অন্যথা যদি(strstartswith(সারি, "DAMAGEMERGE ")) {
      প্রতিজ্ঞা(screen);
      অক্ষর *linep = সারি + 12;
      যখন(linep[0] == ' ')
        linep++;
      যদি(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      অন্যথা যদি(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      অন্যথা যদি(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      অন্যথা যদি(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    অন্যথা যদি(strstartswith(সারি, "DAMAGEFLUSH")) {
      প্রতিজ্ঞা(screen);
      vterm_screen_flush_damage(screen);
    }

    অন্যথা যদি(strstartswith(সারি, "SETDEFAULTCOL ")) {
      প্রতিজ্ঞা(screen);
      অক্ষর *linep = সারি + 14;
      যখন(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      যদি(linep[0]) {
        যখন(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      অন্যথা
        vterm_screen_set_default_colors(screen, &fg, ফাঁকা);
    }

    অন্যথা যদি(সারি[0] == '?') {
      যদি(streq(সারি, "?cursor")) {
        প্রতিজ্ঞা(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        যদি(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        অন্যথা যদি(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        অন্যথা
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      অন্যথা যদি(strstartswith(সারি, "?pen ")) {
        প্রতিজ্ঞা(state);
        VTerm_মান val;
        অক্ষর *linep = সারি + 5;
        যখন(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        যদি(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          যদি(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          অন্যথা
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        অন্যথা যদি(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          যদি(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          অন্যথা
            printf("%d\n", state_pen.underline);
        }
        অন্যথা যদি(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          যদি(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          অন্যথা
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        অন্যথা যদি(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          যদি(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          অন্যথা
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        অন্যথা যদি(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          যদি(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          অন্যথা
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        অন্যথা যদি(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          যদি(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          অন্যথা
            printf("%d\n", state_pen.font);
        }
        অন্যথা যদি(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          যদি(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          অন্যথা
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        অন্যথা যদি(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          যদি(val.সংখ্যা != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.সংখ্যা, state_pen.baseline);
          অন্যথা
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        অন্যথা যদি(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        অন্যথা যদি(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        অন্যথা
          printf("?\n");
      }
      অন্যথা যদি(strstartswith(সারি, "?lineinfo ")) {
        প্রতিজ্ঞা(state);
        অক্ষর *linep = সারি + 10;
        পূর্ণসংখ্যা row;
        ধ্রুবক VTerm_সারি_Info *info;
        যখন(linep[0] == ' ')
          linep++;
        যদি(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          যাও abort_সারি;
        }
        info = vterm_state_get_lineinfo(state, row);
        যদি(info->doublewidth)
          printf("dwl ");
        যদি(info->doubleheight)
          printf("dhl ");
        যদি(info->continuation)
          printf("cont ");
        printf("\n");
      }
      অন্যথা যদি(strstartswith(সারি, "?screen_chars ")) {
        প্রতিজ্ঞা(screen);
        অক্ষর *linep = সারি + 13;
        VTermRect rect;
        size_t len;
        যখন(linep[0] == ' ')
          linep++;
        যদি(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        অন্যথা যদি(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_আকার(vt, ফাঁকা, &rect.end_col);
        }
        অন্যথা {
          printf("! screen_chars unrecognised input\n");
          যাও abort_সারি;
        }
        len = vterm_screen_get_chars(screen, ফাঁকা, 0, rect);
        যদি(len == (size_t)-1)
          printf("! screen_chars error\n");
        অন্যথা যদি(len == 0)
          printf("\n");
        অন্যথা {
          uint32_t *chars = malloc(আকার(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          জন্য(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          মুক্ত(chars);
        }
      }
      অন্যথা যদি(strstartswith(সারি, "?screen_text ")) {
        প্রতিজ্ঞা(screen);
        অক্ষর *linep = সারি + 12;
        VTermRect rect;
        size_t len;
        যখন(linep[0] == ' ')
          linep++;
        যদি(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          যাও abort_সারি;
        }
        len = vterm_screen_get_text(screen, ফাঁকা, 0, rect);
        যদি(len == (size_t)-1)
          printf("! screen_text error\n");
        অন্যথা যদি(len == 0)
          printf("\n");
        অন্যথা {
          // Put an overwrite guard at both ends of the buffer
          unsigned অক্ষর *বাফার = malloc(len + 4);
          unsigned অক্ষর *text = বাফার + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (অক্ষর *)text, len, rect);

          যদি(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          অন্যথা যদি(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          অন্যথা
	  {
	    size_t i;
            জন্য(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          মুক্ত(বাফার);
        }
      }
      অন্যথা যদি(strstartswith(সারি, "?screen_cell ")) {
        প্রতিজ্ঞা(screen);
        অক্ষর *linep = সারি + 12;
	পূর্ণসংখ্যা i;
        VTermPos pos;
        VTermScreenCell cell;
        যখন(linep[0] == ' ')
          linep++;
        যদি(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          যাও abort_সারি;
        }
        যদি(!vterm_screen_get_cell(screen, pos, &cell))
          যাও abort_সারি;
        printf("{");
        জন্য(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        যদি(cell.attrs.bold)      printf("B");
        যদি(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        যদি(cell.attrs.italic)    printf("I");
        যদি(cell.attrs.blink)     printf("K");
        যদি(cell.attrs.reverse)   printf("R");
        যদি(cell.attrs.font)      printf("F%d", cell.attrs.font);
        যদি(cell.attrs.small)     printf("S");
        যদি(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        যদি(cell.attrs.dwl)       printf("dwl ");
        যদি(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      অন্যথা যদি(strstartswith(সারি, "?screen_eol ")) {
        প্রতিজ্ঞা(screen);
        VTermPos pos;
        অক্ষর *linep = সারি + 12;
        যখন(linep[0] == ' ')
          linep++;
        যদি(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          যাও abort_সারি;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      অন্যথা যদি(strstartswith(সারি, "?screen_attrs_extent ")) {
        প্রতিজ্ঞা(screen);
        VTermPos pos;
        VTermRect rect;
        অক্ষর *linep = সারি + 21;
        যখন(linep[0] == ' ')
          linep++;
        যদি(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          যাও abort_সারি;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        যদি(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          যাও abort_সারি;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      অন্যথা
        printf("?\n");

      memset(সারি, 0, আকার সারি);
      চালু;
    }

    অন্যথা
      abort_সারি: err = 1;

    outlen = vterm_output_get_বাফার_current(vt);
    যদি(outlen > 0) {
      অক্ষর outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, ফাঁকা);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  ফেরত 0;
}
