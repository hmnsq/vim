#અન્તર્ભાવ "vterm.h"
#અન્તર્ભાવ "../src/vterm_internal.h" // We pull in some internal bits too

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

સ્થિર size_t inplace_hex2bytes(અક્ષર *s)
{
  અક્ષર *inpos = s, *outpos = s;

  જ્યારે(*inpos) {
    unsigned પૂર્ણાંક ch;
    જો(sscanf(inpos, "%2x", &ch) < 1)
      તોડો;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  પાછા outpos - s;
}

સ્થિર VTermModifier strpe_modifiers(અક્ષર **strp)
{
  VTermModifier state = 0;

  જ્યારે((*strp)[0]) {
    પસંદગી(((*strp)++)[0]) {
      સ્થિતિ 'S': state |= VTERM_MOD_SHIFT; તોડો;
      સ્થિતિ 'C': state |= VTERM_MOD_CTRL;  તોડો;
      સ્થિતિ 'A': state |= VTERM_MOD_ALT;   તોડો;
      મૂળભૂત: પાછા state;
    }
  }

  પાછા state;
}

સ્થિર VTerm_ચાવી strp_ચાવી(અક્ષર *str)
{
  સ્થિર સંરચના {
    અક્ષર *name;
    VTerm_ચાવી ચાવી;
  } ચાવીઓ[] = {
    { "Up",    VTERM_ચાવી_UP },
    { "Tab",   VTERM_ચાવી_TAB },
    { "Enter", VTERM_ચાવી_ENTER },
    { "KP0",   VTERM_ચાવી_KP_0 },
    { "F1",    VTERM_ચાવી_FUNCTION(1) },
    { ખાલી,    VTERM_ચાવી_NONE },
  };
  પૂર્ણાંક i;

  માટે(i = 0; ચાવીઓ[i].name; i++) {
    જો(streq(str, ચાવીઓ[i].name))
      પાછા ચાવીઓ[i].ચાવી;
  }

  પાછા VTERM_ચાવી_NONE;
}

સ્થિર શૂન્ય print_color(અચલ VTermColor *col)
{
  જો (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  નહીંતર જો (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  નહીંતર {
    printf("invalid(%d", col->type);
  }
  જો (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  જો (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

સ્થિર VTermColor strpe_color(અક્ષર **strp)
{
  uint8_t r, g, b, idx;
  પૂર્ણાંક len = 0;
  VTermColor col;

  જો(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  નહીંતર જો(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  નહીંતર
    vterm_color_rgb(&col, 127, 127, 127);

  પાછા col;
}

સ્થિર VTerm *vt;
સ્થિર VTermState *state;
સ્થિર VTermScreen *screen;

સ્થિર VTermEncodingInstance encoding;

સ્થિર શૂન્ય term_output(અચલ અક્ષર *s, size_t len, શૂન્ય *user UNUSED)
{
  size_t i;

  printf("output ");
  માટે(i = 0; i < len; i++)
    printf("%x%s", (unsigned અક્ષર)s[i], i < len-1 ? "," : "\n");
}

સ્થિર શૂન્ય printhex(અચલ અક્ષર *s, size_t len)
{
  જ્યારે(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

સ્થિર પૂર્ણાંક parser_text(અચલ અક્ષર bytes[], size_t len, શૂન્ય *user UNUSED)
{
  size_t i;

  printf("text ");
  માટે(i = 0; i < len; i++) {
    unsigned અક્ષર b = bytes[i];
    જો(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      તોડો;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  પાછા i;
}

સ્થિર પૂર્ણાંક parser_control(unsigned અક્ષર control, શૂન્ય *user UNUSED)
{
  printf("control %02x\n", control);

  પાછા 1;
}

સ્થિર પૂર્ણાંક parser_escape(અચલ અક્ષર bytes[], size_t len, શૂન્ય *user UNUSED)
{
  જો(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    જો(len < 2)
      પાછા -1;
    len = 2;
  }
  નહીંતર {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  પાછા len;
}

સ્થિર પૂર્ણાંક parser_csi(અચલ અક્ષર *leader, અચલ long args[], પૂર્ણાંક argcount, અચલ અક્ષર *intermed, અક્ષર command, શૂન્ય *user UNUSED)
{
  પૂર્ણાંક i;
  printf("csi %02x", command);

  જો(leader && leader[0]) {
    printf(" L=");
    માટે(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  માટે(i = 0; i < argcount; i++) {
    અક્ષર sep = i ? ',' : ' ';

    જો(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    નહીંતર
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  જો(intermed && intermed[0]) {
    printf(" I=");
    માટે(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક parser_osc(પૂર્ણાંક command, VTermStringFragment frag, શૂન્ય *user UNUSED)
{

  printf("osc ");

  જો(frag.initial) {
    જો(command == -1)
      printf("[");
    નહીંતર
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  જો(frag.final)
    printf("]");

  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક parser_dcs(અચલ અક્ષર *command, size_t commandlen, VTermStringFragment frag, શૂન્ય *user UNUSED)
{
  printf("dcs ");

  જો(frag.initial) {
    size_t i;
    printf("[");
    માટે(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  જો(frag.final)
    printf("]");

  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક parser_apc(VTermStringFragment frag, શૂન્ય *user UNUSED)
{
  printf("apc ");

  જો(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  જો(frag.final)
    printf("]");

  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક parser_pm(VTermStringFragment frag, શૂન્ય *user UNUSED)
{
  printf("pm ");

  જો(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  જો(frag.final)
    printf("]");

  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક parser_sos(VTermStringFragment frag, શૂન્ય *user UNUSED)
{
  printf("sos ");

  જો(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  જો(frag.final)
    printf("]");

  printf("\n");

  પાછા 1;
}

સ્થિર VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ખાલી // resize
};

સ્થિર VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

સ્થિર પૂર્ણાંક want_movecursor = 0;
સ્થિર VTermPos state_pos;
સ્થિર પૂર્ણાંક movecursor(VTermPos pos, VTermPos oldpos UNUSED, પૂર્ણાંક visible UNUSED, શૂન્ય *user UNUSED)
{
  state_pos = pos;

  જો(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  પાછા 1;
}

સ્થિર પૂર્ણાંક want_premove = 0;
સ્થિર પૂર્ણાંક premove(VTermRect rect, શૂન્ય *user UNUSED)
{
  જો(!want_premove)
    પાછા 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  પાછા 1;
}

સ્થિર પૂર્ણાંક want_scrollrect = 0;
સ્થિર પૂર્ણાંક scrollrect(VTermRect rect, પૂર્ણાંક downward, પૂર્ણાંક rightward, શૂન્ય *user UNUSED)
{
  જો(!want_scrollrect)
    પાછા 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  પાછા 1;
}

સ્થિર પૂર્ણાંક want_moverect = 0;
સ્થિર પૂર્ણાંક moverect(VTermRect dest, VTermRect src, શૂન્ય *user UNUSED)
{
  જો(!want_moverect)
    પાછા 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  પાછા 1;
}

સ્થિર પૂર્ણાંક want_settermprop = 0;
સ્થિર પૂર્ણાંક settermprop(VTermProp prop, VTerm_કિંમત *val, શૂન્ય *user UNUSED)
{
  VTerm_કિંમત_Type type;
  જો(!want_settermprop)
    પાછા 1;

  type = vterm_get_prop_type(prop);
  પસંદગી(type) {
  સ્થિતિ VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    પાછા 1;
  સ્થિતિ VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->સંખ્યા);
    પાછા 1;
  સ્થિતિ VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    પાછા 1;
  સ્થિતિ VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    પાછા 1;

  સ્થિતિ VTERM_N_VALUETYPES:
    પાછા 0;
  }

  પાછા 0;
}

// These callbacks are for State

સ્થિર પૂર્ણાંક want_state_putglyph = 0;
સ્થિર પૂર્ણાંક state_putglyph(VTermGlyphInfo *info, VTermPos pos, શૂન્ય *user UNUSED)
{
  પૂર્ણાંક i;
  જો(!want_state_putglyph)
    પાછા 1;

  printf("putglyph ");
  માટે(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  જો(info->protected_cell)
    printf(" prot");
  જો(info->dwl)
    printf(" dwl");
  જો(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક want_state_erase = 0;
સ્થિર પૂર્ણાંક state_erase(VTermRect rect, પૂર્ણાંક selective, શૂન્ય *user UNUSED)
{
  જો(!want_state_erase)
    પાછા 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  પાછા 1;
}

સ્થિર સંરચના {
  પૂર્ણાંક bold;
  પૂર્ણાંક underline;
  પૂર્ણાંક italic;
  પૂર્ણાંક blink;
  પૂર્ણાંક reverse;
  પૂર્ણાંક conceal;
  પૂર્ણાંક strike;
  પૂર્ણાંક font;
  પૂર્ણાંક small;
  પૂર્ણાંક baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
સ્થિર પૂર્ણાંક state_setpenattr(VTermAttr attr, VTerm_કિંમત *val, શૂન્ય *user UNUSED)
{
  પસંદગી(attr) {
  સ્થિતિ VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    તોડો;
  સ્થિતિ VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->સંખ્યા;
    તોડો;
  સ્થિતિ VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    તોડો;
  સ્થિતિ VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    તોડો;
  સ્થિતિ VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    તોડો;
  સ્થિતિ VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    તોડો;
  સ્થિતિ VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    તોડો;
  સ્થિતિ VTERM_ATTR_FONT:
    state_pen.font = val->સંખ્યા;
    તોડો;
  સ્થિતિ VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    તોડો;
  સ્થિતિ VTERM_ATTR_BASELINE:
    state_pen.baseline = val->સંખ્યા;
    તોડો;
  સ્થિતિ VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    તોડો;
  સ્થિતિ VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    તોડો;

  સ્થિતિ VTERM_N_ATTRS:
    પાછા 0;
  }

  પાછા 1;
}

સ્થિર પૂર્ણાંક state_setlineinfo(પૂર્ણાંક row UNUSED, અચલ VTerm_લીટી_Info *newinfo UNUSED, અચલ VTerm_લીટી_Info *oldinfo UNUSED, શૂન્ય *user UNUSED)
{
  પાછા 1;
}

સ્થિર પૂર્ણાંક want_state_scrollback = 0;
સ્થિર પૂર્ણાંક state_sb_clear(શૂન્ય *user UNUSED) {
  જો(!want_state_scrollback)
    પાછા 1;

  printf("sb_clear\n");
  પાછા 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ખાલી, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ખાલી, // bell
  ખાલી, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

સ્થિર પૂર્ણાંક selection_set(VTermSelectionMask mask, VTermStringFragment frag, શૂન્ય *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  જો(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  જો(frag.final)
    printf("]");
  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક selection_query(VTermSelectionMask mask, શૂન્ય *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  પાછા 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

સ્થિર પૂર્ણાંક want_screen_damage = 0;
સ્થિર પૂર્ણાંક want_screen_damage_cells = 0;
સ્થિર પૂર્ણાંક screen_damage(VTermRect rect, શૂન્ય *user UNUSED)
{
  જો(!want_screen_damage)
    પાછા 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  જો(want_screen_damage_cells) {
    પૂર્ણાંક equals = FALSE;
    પૂર્ણાંક row;
    પૂર્ણાંક col;

    માટે(row = rect.start_row; row < rect.end_row; row++) {
      પૂર્ણાંક eol = rect.end_col;
      જ્યારે(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        જો(cell.chars[0])
          તોડો;

        eol--;
      }

      જો(eol == rect.start_col)
        તોડો;

      જો(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      માટે(col = rect.start_col; col < eol; col++) {
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

  પાછા 1;
}

સ્થિર પૂર્ણાંક want_screen_scrollback = 0;
સ્થિર પૂર્ણાંક screen_sb_pushline4(પૂર્ણાંક cols, અચલ VTermScreenCell *cells, પૂર્ણાંક continuation, શૂન્ય *user UNUSED)
{
  પૂર્ણાંક eol;
  પૂર્ણાંક c;

  જો(!want_screen_scrollback)
    પાછા 1;

  eol = cols;
  જ્યારે(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  માટે(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  પાછા 1;
}

સ્થિર પૂર્ણાંક screen_sb_popline(પૂર્ણાંક cols, VTermScreenCell *cells, શૂન્ય *user UNUSED)
{
  પૂર્ણાંક col;

  જો(!want_screen_scrollback)
    પાછા 0;

  // All lines of scrollback contain "ABCDE"
  માટે(col = 0; col < cols; col++) {
    જો(col < 5)
      cells[col].chars[0] = 'A' + col;
    નહીંતર
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  પાછા 1;
}

સ્થિર પૂર્ણાંક screen_sb_clear(શૂન્ય *user UNUSED)
{
  જો(!want_screen_scrollback)
    પાછા 1;

  printf("sb_clear\n");
  પાછા 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ખાલી, // bell
  ખાલી, // resize
  ખાલી, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

પૂર્ણાંક મુખ્ય(પૂર્ણાંક argc UNUSED, અક્ષર **argv UNUSED)
{
  અક્ષર લીટી[1024] = {0};
  પૂર્ણાંક flag;

  પૂર્ણાંક err;

  setvbuf(stdout, ખાલી, _IONBF, 0);

  જ્યારે(fgets(લીટી, કદ લીટી, stdin)) {
    અક્ષર *nl;
    size_t outlen;
    err = 0;

    જો((nl = strchr(લીટી, '\n')))
      *nl = '\0';

    જો(streq(લીટી, "INIT")) {
      જો(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    નહીંતર જો(streq(લીટી, "WANTPARSER")) {
      પ્રતિજ્ઞા(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, ખાલી);
    }

    નહીંતર જો(strstartswith(લીટી, "WANTSTATE") && (લીટી[9] == '\0' || લીટી[9] == ' ')) {
      પૂર્ણાંક i = 9;
      પૂર્ણાંક sense = 1;
      પ્રતિજ્ઞા(vt);
      જો(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, ખાલી);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, ખાલી, ખાલી, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      જ્યારે(લીટી[i] == ' ')
        i++;
      માટે( ; લીટી[i]; i++)
        પસંદગી(લીટી[i]) {
        સ્થિતિ '+':
          sense = 1;
          તોડો;
        સ્થિતિ '-':
          sense = 0;
          તોડો;
        સ્થિતિ 'g':
          want_state_putglyph = sense;
          તોડો;
        સ્થિતિ 's':
          want_scrollrect = sense;
          તોડો;
        સ્થિતિ 'P':
          want_premove = sense;
          તોડો;
        સ્થિતિ 'm':
          want_moverect = sense;
          તોડો;
        સ્થિતિ 'e':
          want_state_erase = sense;
          તોડો;
        સ્થિતિ 'p':
          want_settermprop = sense;
          તોડો;
        સ્થિતિ 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : ખાલી, ખાલી);
          તોડો;
        સ્થિતિ 'b':
          want_state_scrollback = sense;
          તોડો;
        મૂળભૂત:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", લીટી[i]);
        }
    }

    નહીંતર જો(strstartswith(લીટી, "WANTSCREEN") && (લીટી[10] == '\0' || લીટી[10] == ' ')) {
      પૂર્ણાંક i = 10;
      પૂર્ણાંક sense = 1;
      પ્રતિજ્ઞા(vt);
      જો(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, ખાલી);
      vterm_screen_callbacks_has_pushline4(screen);

      જ્યારે(લીટી[i] == ' ')
        i++;
      માટે( ; લીટી[i]; i++)
        પસંદગી(લીટી[i]) {
        સ્થિતિ '-':
          sense = 0;
          તોડો;
        સ્થિતિ 'a':
          vterm_screen_enable_altscreen(screen, 1);
          તોડો;
        સ્થિતિ 'd':
          want_screen_damage = sense;
          તોડો;
        સ્થિતિ 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          તોડો;
        સ્થિતિ 'm':
          want_moverect = sense;
          તોડો;
        સ્થિતિ 'c':
          want_movecursor = sense;
          તોડો;
        સ્થિતિ 'p':
          want_settermprop = 1;
          તોડો;
        સ્થિતિ 'b':
          want_screen_scrollback = sense;
          તોડો;
        સ્થિતિ 'r':
          vterm_screen_enable_reflow(screen, sense);
          તોડો;
        મૂળભૂત:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", લીટી[i]);
        }
    }

    નહીંતર જો(sscanf(લીટી, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    નહીંતર જો(streq(લીટી, "RESET")) {
      જો(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      જો(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    નહીંતર જો(strstartswith(લીટી, "RESIZE ")) {
      પૂર્ણાંક rows, cols;
      અક્ષર *linep = લીટી + 7;
      જ્યારે(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    નહીંતર જો(strstartswith(લીટી, "PUSH ")) {
      અક્ષર *bytes = લીટી + 5;
      size_t len = inplace_hex2bytes(bytes);
      પ્રતિજ્ઞા(len);

      size_t written = vterm_આગત_write(vt, bytes, len);
      જો(written < len)
        fprintf(stderr, "! short write\n");
    }

    નહીંતર જો(streq(લીટી, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      જો(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    નહીંતર જો(strstartswith(લીટી, "ENCIN ")) {
      અક્ષર *bytes = લીટી + 6;
      size_t len = inplace_hex2bytes(bytes);
      પ્રતિજ્ઞા(len);

      uint32_t cp[1024];
      પૂર્ણાંક cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      જો(cpi > 0) {
	પૂર્ણાંક i;
        printf("encout ");
        માટે(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    નહીંતર જો(strstartswith(લીટી, "INCHAR ")) {
      અક્ષર *linep = લીટી + 7;
      unsigned પૂર્ણાંક c = 0;
      VTermModifier mod;
      જ્યારે(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    નહીંતર જો(strstartswith(લીટી, "INKEY ")) {
      VTermModifier mod;
      VTerm_ચાવી ચાવી;
      અક્ષર *linep = લીટી + 6;
      જ્યારે(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      જ્યારે(linep[0] == ' ')
        linep++;
      ચાવી = strp_ચાવી(linep);

      vterm_keyboard_ચાવી(vt, ચાવી, mod);
    }

    નહીંતર જો(strstartswith(લીટી, "PASTE ")) {
      અક્ષર *linep = લીટી + 6;
      જો(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      નહીંતર જો(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      નહીંતર
        જાઓ abort_લીટી;
    }

    નહીંતર જો(strstartswith(લીટી, "FOCUS ")) {
      પ્રતિજ્ઞા(state);
      અક્ષર *linep = લીટી + 6;
      જો(streq(linep, "IN"))
        vterm_state_focus_in(state);
      નહીંતર જો(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      નહીંતર
        જાઓ abort_લીટી;
    }

    નહીંતર જો(strstartswith(લીટી, "MOUSEMOVE ")) {
      અક્ષર *linep = લીટી + 10;
      પૂર્ણાંક row, col, len;
      VTermModifier mod;
      જ્યારે(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      જ્યારે(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    નહીંતર જો(strstartswith(લીટી, "MOUSEBTN ")) {
      અક્ષર *linep = લીટી + 9;
      અક્ષર press;
      પૂર્ણાંક button, len;
      VTermModifier mod;
      જ્યારે(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      જ્યારે(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    નહીંતર જો(strstartswith(લીટી, "SELECTION ")) {
      અક્ષર *linep = લીટી + 10;
      unsigned પૂર્ણાંક mask;
      પૂર્ણાંક len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      જ્યારે(linep[0] == ' ')
        linep++;
      જો(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        જ્યારે(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      પ્રતિજ્ઞા(frag.len);

      linep += frag.len * 2;
      જ્યારે(linep[0] == ' ')
        linep++;
      જો(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    નહીંતર જો(strstartswith(લીટી, "DAMAGEMERGE ")) {
      પ્રતિજ્ઞા(screen);
      અક્ષર *linep = લીટી + 12;
      જ્યારે(linep[0] == ' ')
        linep++;
      જો(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      નહીંતર જો(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      નહીંતર જો(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      નહીંતર જો(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    નહીંતર જો(strstartswith(લીટી, "DAMAGEFLUSH")) {
      પ્રતિજ્ઞા(screen);
      vterm_screen_flush_damage(screen);
    }

    નહીંતર જો(strstartswith(લીટી, "SETDEFAULTCOL ")) {
      પ્રતિજ્ઞા(screen);
      અક્ષર *linep = લીટી + 14;
      જ્યારે(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      જો(linep[0]) {
        જ્યારે(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      નહીંતર
        vterm_screen_set_default_colors(screen, &fg, ખાલી);
    }

    નહીંતર જો(લીટી[0] == '?') {
      જો(streq(લીટી, "?cursor")) {
        પ્રતિજ્ઞા(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        જો(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        નહીંતર જો(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        નહીંતર
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      નહીંતર જો(strstartswith(લીટી, "?pen ")) {
        પ્રતિજ્ઞા(state);
        VTerm_કિંમત val;
        અક્ષર *linep = લીટી + 5;
        જ્યારે(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        જો(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          જો(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          નહીંતર
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        નહીંતર જો(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          જો(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          નહીંતર
            printf("%d\n", state_pen.underline);
        }
        નહીંતર જો(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          જો(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          નહીંતર
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        નહીંતર જો(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          જો(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          નહીંતર
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        નહીંતર જો(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          જો(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          નહીંતર
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        નહીંતર જો(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          જો(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          નહીંતર
            printf("%d\n", state_pen.font);
        }
        નહીંતર જો(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          જો(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          નહીંતર
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        નહીંતર જો(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          જો(val.સંખ્યા != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.સંખ્યા, state_pen.baseline);
          નહીંતર
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        નહીંતર જો(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        નહીંતર જો(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        નહીંતર
          printf("?\n");
      }
      નહીંતર જો(strstartswith(લીટી, "?lineinfo ")) {
        પ્રતિજ્ઞા(state);
        અક્ષર *linep = લીટી + 10;
        પૂર્ણાંક row;
        અચલ VTerm_લીટી_Info *info;
        જ્યારે(linep[0] == ' ')
          linep++;
        જો(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          જાઓ abort_લીટી;
        }
        info = vterm_state_get_lineinfo(state, row);
        જો(info->doublewidth)
          printf("dwl ");
        જો(info->doubleheight)
          printf("dhl ");
        જો(info->continuation)
          printf("cont ");
        printf("\n");
      }
      નહીંતર જો(strstartswith(લીટી, "?screen_chars ")) {
        પ્રતિજ્ઞા(screen);
        અક્ષર *linep = લીટી + 13;
        VTermRect rect;
        size_t len;
        જ્યારે(linep[0] == ' ')
          linep++;
        જો(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        નહીંતર જો(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, ખાલી, &rect.end_col);
        }
        નહીંતર {
          printf("! screen_chars unrecognised input\n");
          જાઓ abort_લીટી;
        }
        len = vterm_screen_get_chars(screen, ખાલી, 0, rect);
        જો(len == (size_t)-1)
          printf("! screen_chars error\n");
        નહીંતર જો(len == 0)
          printf("\n");
        નહીંતર {
          uint32_t *chars = malloc(કદ(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          માટે(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          મુક્ત(chars);
        }
      }
      નહીંતર જો(strstartswith(લીટી, "?screen_text ")) {
        પ્રતિજ્ઞા(screen);
        અક્ષર *linep = લીટી + 12;
        VTermRect rect;
        size_t len;
        જ્યારે(linep[0] == ' ')
          linep++;
        જો(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          જાઓ abort_લીટી;
        }
        len = vterm_screen_get_text(screen, ખાલી, 0, rect);
        જો(len == (size_t)-1)
          printf("! screen_text error\n");
        નહીંતર જો(len == 0)
          printf("\n");
        નહીંતર {
          // Put an overwrite guard at both ends of the buffer
          unsigned અક્ષર *buffer = malloc(len + 4);
          unsigned અક્ષર *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (અક્ષર *)text, len, rect);

          જો(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          નહીંતર જો(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          નહીંતર
	  {
	    size_t i;
            માટે(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          મુક્ત(buffer);
        }
      }
      નહીંતર જો(strstartswith(લીટી, "?screen_cell ")) {
        પ્રતિજ્ઞા(screen);
        અક્ષર *linep = લીટી + 12;
	પૂર્ણાંક i;
        VTermPos pos;
        VTermScreenCell cell;
        જ્યારે(linep[0] == ' ')
          linep++;
        જો(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          જાઓ abort_લીટી;
        }
        જો(!vterm_screen_get_cell(screen, pos, &cell))
          જાઓ abort_લીટી;
        printf("{");
        માટે(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        જો(cell.attrs.bold)      printf("B");
        જો(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        જો(cell.attrs.italic)    printf("I");
        જો(cell.attrs.blink)     printf("K");
        જો(cell.attrs.reverse)   printf("R");
        જો(cell.attrs.font)      printf("F%d", cell.attrs.font);
        જો(cell.attrs.small)     printf("S");
        જો(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        જો(cell.attrs.dwl)       printf("dwl ");
        જો(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      નહીંતર જો(strstartswith(લીટી, "?screen_eol ")) {
        પ્રતિજ્ઞા(screen);
        VTermPos pos;
        અક્ષર *linep = લીટી + 12;
        જ્યારે(linep[0] == ' ')
          linep++;
        જો(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          જાઓ abort_લીટી;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      નહીંતર જો(strstartswith(લીટી, "?screen_attrs_extent ")) {
        પ્રતિજ્ઞા(screen);
        VTermPos pos;
        VTermRect rect;
        અક્ષર *linep = લીટી + 21;
        જ્યારે(linep[0] == ' ')
          linep++;
        જો(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          જાઓ abort_લીટી;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        જો(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          જાઓ abort_લીટી;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      નહીંતર
        printf("?\n");

      memset(લીટી, 0, કદ લીટી);
      ચાલુ;
    }

    નહીંતર
      abort_લીટી: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    જો(outlen > 0) {
      અક્ષર outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, ખાલી);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  પાછા 0;
}
