#அந்தர்பாவ "vterm.h"
#அந்தர்பாவ "../src/vterm_internal.h" // We pull in some internal bits too

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#define streq(a,b) (!strcmp(a,b))
#define strstartswith(a,b) (!strncmp(a,b,strlen(b)))

நிலையான size_t inplace_hex2bytes(எழுத்து *s)
{
  எழுத்து *inpos = s, *outpos = s;

  போது(*inpos) {
    unsigned முழு ch;
    எனில்(sscanf(inpos, "%2x", &ch) < 1)
      நிறுத்து;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  திரும்பு outpos - s;
}

நிலையான VTermModifier strpe_modifiers(எழுத்து **strp)
{
  VTermModifier state = 0;

  போது((*strp)[0]) {
    தேர்வு(((*strp)++)[0]) {
      நிலை 'S': state |= VTERM_MOD_SHIFT; நிறுத்து;
      நிலை 'C': state |= VTERM_MOD_CTRL;  நிறுத்து;
      நிலை 'A': state |= VTERM_MOD_ALT;   நிறுத்து;
      இயல்பு: திரும்பு state;
    }
  }

  திரும்பு state;
}

நிலையான VTerm_சாவி strp_சாவி(எழுத்து *str)
{
  நிலையான கட்டமைப்பு {
    எழுத்து *name;
    VTerm_சாவி சாவி;
  } சாவிகள்[] = {
    { "Up",    VTERM_சாவி_UP },
    { "Tab",   VTERM_சாவி_TAB },
    { "Enter", VTERM_சாவி_ENTER },
    { "KP0",   VTERM_சாவி_KP_0 },
    { "F1",    VTERM_சாவி_FUNCTION(1) },
    { வெற்று,    VTERM_சாவி_NONE },
  };
  முழு i;

  க்கு(i = 0; சாவிகள்[i].name; i++) {
    எனில்(streq(str, சாவிகள்[i].name))
      திரும்பு சாவிகள்[i].சாவி;
  }

  திரும்பு VTERM_சாவி_NONE;
}

நிலையான வெறுமை print_color(மாறிலி VTermColor *col)
{
  எனில் (VTERM_COLOR_IS_RGB(col)) {
    printf("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  இல்லையெனில் எனில் (VTERM_COLOR_IS_INDEXED(col)) {
    printf("idx(%d", col->index);
  }
  இல்லையெனில் {
    printf("invalid(%d", col->type);
  }
  எனில் (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    printf(",is_default_fg");
  }
  எனில் (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    printf(",is_default_bg");
  }
  printf(")");
}

நிலையான VTermColor strpe_color(எழுத்து **strp)
{
  uint8_t r, g, b, idx;
  முழு len = 0;
  VTermColor col;

  எனில்(sscanf(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  இல்லையெனில் எனில்(sscanf(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  இல்லையெனில்
    vterm_color_rgb(&col, 127, 127, 127);

  திரும்பு col;
}

நிலையான VTerm *vt;
நிலையான VTermState *state;
நிலையான VTermScreen *screen;

நிலையான VTermEncodingInstance encoding;

நிலையான வெறுமை term_output(மாறிலி எழுத்து *s, size_t len, வெறுமை *user UNUSED)
{
  size_t i;

  printf("output ");
  க்கு(i = 0; i < len; i++)
    printf("%x%s", (unsigned எழுத்து)s[i], i < len-1 ? "," : "\n");
}

நிலையான வெறுமை printhex(மாறிலி எழுத்து *s, size_t len)
{
  போது(len--)
    printf("%02x", (uint8_t)(s++)[0]);
}

நிலையான முழு parser_text(மாறிலி எழுத்து bytes[], size_t len, வெறுமை *user UNUSED)
{
  size_t i;

  printf("text ");
  க்கு(i = 0; i < len; i++) {
    unsigned எழுத்து b = bytes[i];
    எனில்(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      நிறுத்து;
    printf(i ? ",%x" : "%x", b);
  }
  printf("\n");

  திரும்பு i;
}

நிலையான முழு parser_control(unsigned எழுத்து control, வெறுமை *user UNUSED)
{
  printf("control %02x\n", control);

  திரும்பு 1;
}

நிலையான முழு parser_escape(மாறிலி எழுத்து bytes[], size_t len, வெறுமை *user UNUSED)
{
  எனில்(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    எனில்(len < 2)
      திரும்பு -1;
    len = 2;
  }
  இல்லையெனில் {
    len = 1;
  }

  printf("escape ");
  printhex(bytes, len);
  printf("\n");

  திரும்பு len;
}

நிலையான முழு parser_csi(மாறிலி எழுத்து *leader, மாறிலி long args[], முழு argcount, மாறிலி எழுத்து *intermed, எழுத்து command, வெறுமை *user UNUSED)
{
  முழு i;
  printf("csi %02x", command);

  எனில்(leader && leader[0]) {
    printf(" L=");
    க்கு(i = 0; leader[i]; i++)
      printf("%02x", leader[i]);
  }

  க்கு(i = 0; i < argcount; i++) {
    எழுத்து sep = i ? ',' : ' ';

    எனில்(args[i] == CSI_ARG_MISSING)
      printf("%c*", sep);
    இல்லையெனில்
      printf("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  எனில்(intermed && intermed[0]) {
    printf(" I=");
    க்கு(i = 0; intermed[i]; i++)
      printf("%02x", intermed[i]);
  }

  printf("\n");

  திரும்பு 1;
}

நிலையான முழு parser_osc(முழு command, VTermStringFragment frag, வெறுமை *user UNUSED)
{

  printf("osc ");

  எனில்(frag.initial) {
    எனில்(command == -1)
      printf("[");
    இல்லையெனில்
      printf("[%d;", command);
  }

  printhex(frag.str, frag.len);

  எனில்(frag.final)
    printf("]");

  printf("\n");

  திரும்பு 1;
}

நிலையான முழு parser_dcs(மாறிலி எழுத்து *command, size_t commandlen, VTermStringFragment frag, வெறுமை *user UNUSED)
{
  printf("dcs ");

  எனில்(frag.initial) {
    size_t i;
    printf("[");
    க்கு(i = 0; i < commandlen; i++)
      printf("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  எனில்(frag.final)
    printf("]");

  printf("\n");

  திரும்பு 1;
}

நிலையான முழு parser_apc(VTermStringFragment frag, வெறுமை *user UNUSED)
{
  printf("apc ");

  எனில்(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  எனில்(frag.final)
    printf("]");

  printf("\n");

  திரும்பு 1;
}

நிலையான முழு parser_pm(VTermStringFragment frag, வெறுமை *user UNUSED)
{
  printf("pm ");

  எனில்(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  எனில்(frag.final)
    printf("]");

  printf("\n");

  திரும்பு 1;
}

நிலையான முழு parser_sos(VTermStringFragment frag, வெறுமை *user UNUSED)
{
  printf("sos ");

  எனில்(frag.initial)
    printf("[");

  printhex(frag.str, frag.len);

  எனில்(frag.final)
    printf("]");

  printf("\n");

  திரும்பு 1;
}

நிலையான VTermParserCallbacks parser_cbs = {
  parser_text, // text
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  வெற்று // resize
};

நிலையான VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

நிலையான முழு want_movecursor = 0;
நிலையான VTermPos state_pos;
நிலையான முழு movecursor(VTermPos pos, VTermPos oldpos UNUSED, முழு visible UNUSED, வெறுமை *user UNUSED)
{
  state_pos = pos;

  எனில்(want_movecursor)
    printf("movecursor %d,%d\n", pos.row, pos.col);

  திரும்பு 1;
}

நிலையான முழு want_premove = 0;
நிலையான முழு premove(VTermRect rect, வெறுமை *user UNUSED)
{
  எனில்(!want_premove)
    திரும்பு 0;

  printf("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  திரும்பு 1;
}

நிலையான முழு want_scrollrect = 0;
நிலையான முழு scrollrect(VTermRect rect, முழு downward, முழு rightward, வெறுமை *user UNUSED)
{
  எனில்(!want_scrollrect)
    திரும்பு 0;

  printf("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  திரும்பு 1;
}

நிலையான முழு want_moverect = 0;
நிலையான முழு moverect(VTermRect dest, VTermRect src, வெறுமை *user UNUSED)
{
  எனில்(!want_moverect)
    திரும்பு 0;

  printf("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  திரும்பு 1;
}

நிலையான முழு want_settermprop = 0;
நிலையான முழு settermprop(VTermProp prop, VTerm_மதிப்பு *val, வெறுமை *user UNUSED)
{
  VTerm_மதிப்பு_Type type;
  எனில்(!want_settermprop)
    திரும்பு 1;

  type = vterm_get_prop_type(prop);
  தேர்வு(type) {
  நிலை VTERM_VALUETYPE_BOOL:
    printf("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    திரும்பு 1;
  நிலை VTERM_VALUETYPE_INT:
    printf("settermprop %d %d\n", prop, val->எண்);
    திரும்பு 1;
  நிலை VTERM_VALUETYPE_STRING:
    printf("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    திரும்பு 1;
  நிலை VTERM_VALUETYPE_COLOR:
    printf("settermprop %d ", prop);
    print_color(&val->color);
    printf("\n");
    திரும்பு 1;

  நிலை VTERM_N_VALUETYPES:
    திரும்பு 0;
  }

  திரும்பு 0;
}

// These callbacks are for State

நிலையான முழு want_state_putglyph = 0;
நிலையான முழு state_putglyph(VTermGlyphInfo *info, VTermPos pos, வெறுமை *user UNUSED)
{
  முழு i;
  எனில்(!want_state_putglyph)
    திரும்பு 1;

  printf("putglyph ");
  க்கு(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    printf(i ? ",%x" : "%x", info->chars[i]);
  printf(" %d %d,%d", info->width, pos.row, pos.col);
  எனில்(info->protected_cell)
    printf(" prot");
  எனில்(info->dwl)
    printf(" dwl");
  எனில்(info->dhl)
    printf(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  printf("\n");

  திரும்பு 1;
}

நிலையான முழு want_state_erase = 0;
நிலையான முழு state_erase(VTermRect rect, முழு selective, வெறுமை *user UNUSED)
{
  எனில்(!want_state_erase)
    திரும்பு 1;

  printf("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  திரும்பு 1;
}

நிலையான கட்டமைப்பு {
  முழு bold;
  முழு underline;
  முழு italic;
  முழு blink;
  முழு reverse;
  முழு conceal;
  முழு strike;
  முழு font;
  முழு small;
  முழு baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
நிலையான முழு state_setpenattr(VTermAttr attr, VTerm_மதிப்பு *val, வெறுமை *user UNUSED)
{
  தேர்வு(attr) {
  நிலை VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    நிறுத்து;
  நிலை VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->எண்;
    நிறுத்து;
  நிலை VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    நிறுத்து;
  நிலை VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    நிறுத்து;
  நிலை VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    நிறுத்து;
  நிலை VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    நிறுத்து;
  நிலை VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    நிறுத்து;
  நிலை VTERM_ATTR_FONT:
    state_pen.font = val->எண்;
    நிறுத்து;
  நிலை VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    நிறுத்து;
  நிலை VTERM_ATTR_BASELINE:
    state_pen.baseline = val->எண்;
    நிறுத்து;
  நிலை VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    நிறுத்து;
  நிலை VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    நிறுத்து;

  நிலை VTERM_N_ATTRS:
    திரும்பு 0;
  }

  திரும்பு 1;
}

நிலையான முழு state_setlineinfo(முழு row UNUSED, மாறிலி VTerm_வரி_Info *newinfo UNUSED, மாறிலி VTerm_வரி_Info *oldinfo UNUSED, வெறுமை *user UNUSED)
{
  திரும்பு 1;
}

நிலையான முழு want_state_scrollback = 0;
நிலையான முழு state_sb_clear(வெறுமை *user UNUSED) {
  எனில்(!want_state_scrollback)
    திரும்பு 1;

  printf("sb_clear\n");
  திரும்பு 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  வெற்று, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  வெற்று, // bell
  வெற்று, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

நிலையான முழு selection_set(VTermSelectionMask mask, VTermStringFragment frag, வெறுமை *user UNUSED)
{
  printf("selection-set mask=%04X ", mask);
  எனில்(frag.initial)
    printf("[");
  printhex(frag.str, frag.len);
  எனில்(frag.final)
    printf("]");
  printf("\n");

  திரும்பு 1;
}

நிலையான முழு selection_query(VTermSelectionMask mask, வெறுமை *user UNUSED)
{
  printf("selection-query mask=%04X\n", mask);

  திரும்பு 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

நிலையான முழு want_screen_damage = 0;
நிலையான முழு want_screen_damage_cells = 0;
நிலையான முழு screen_damage(VTermRect rect, வெறுமை *user UNUSED)
{
  எனில்(!want_screen_damage)
    திரும்பு 1;

  printf("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  எனில்(want_screen_damage_cells) {
    முழு equals = FALSE;
    முழு row;
    முழு col;

    க்கு(row = rect.start_row; row < rect.end_row; row++) {
      முழு eol = rect.end_col;
      போது(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        எனில்(cell.chars[0])
          நிறுத்து;

        eol--;
      }

      எனில்(eol == rect.start_col)
        நிறுத்து;

      எனில்(!equals)
        printf(" ="), equals = TRUE;

      printf(" %d<", row);
      க்கு(col = rect.start_col; col < eol; col++) {
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

  திரும்பு 1;
}

நிலையான முழு want_screen_scrollback = 0;
நிலையான முழு screen_sb_pushline4(முழு cols, மாறிலி VTermScreenCell *cells, முழு continuation, வெறுமை *user UNUSED)
{
  முழு eol;
  முழு c;

  எனில்(!want_screen_scrollback)
    திரும்பு 1;

  eol = cols;
  போது(eol && !cells[eol-1].chars[0])
    eol--;

  printf("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  க்கு(c = 0; c < eol; c++)
    printf(" %02X", cells[c].chars[0]);
  printf("\n");

  திரும்பு 1;
}

நிலையான முழு screen_sb_popline(முழு cols, VTermScreenCell *cells, வெறுமை *user UNUSED)
{
  முழு col;

  எனில்(!want_screen_scrollback)
    திரும்பு 0;

  // All lines of scrollback contain "ABCDE"
  க்கு(col = 0; col < cols; col++) {
    எனில்(col < 5)
      cells[col].chars[0] = 'A' + col;
    இல்லையெனில்
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  printf("sb_popline %d\n", cols);
  திரும்பு 1;
}

நிலையான முழு screen_sb_clear(வெறுமை *user UNUSED)
{
  எனில்(!want_screen_scrollback)
    திரும்பு 1;

  printf("sb_clear\n");
  திரும்பு 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  வெற்று, // bell
  வெற்று, // resize
  வெற்று, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

முழு முதன்மை(முழு argc UNUSED, எழுத்து **argv UNUSED)
{
  எழுத்து வரி[1024] = {0};
  முழு flag;

  முழு err;

  setvbuf(stdout, வெற்று, _IONBF, 0);

  போது(fgets(வரி, அளவு வரி, stdin)) {
    எழுத்து *nl;
    size_t outlen;
    err = 0;

    எனில்((nl = strchr(வரி, '\n')))
      *nl = '\0';

    எனில்(streq(வரி, "INIT")) {
      எனில்(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    இல்லையெனில் எனில்(streq(வரி, "WANTPARSER")) {
      ப்ரதிஜ்ஞா(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, வெற்று);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "WANTSTATE") && (வரி[9] == '\0' || வரி[9] == ' ')) {
      முழு i = 9;
      முழு sense = 1;
      ப்ரதிஜ்ஞா(vt);
      எனில்(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_set_callbacks(state, &state_cbs, வெற்று);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_set_selection_callbacks(state, &selection_cbs, வெற்று, வெற்று, 16);
        vterm_state_set_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      போது(வரி[i] == ' ')
        i++;
      க்கு( ; வரி[i]; i++)
        தேர்வு(வரி[i]) {
        நிலை '+':
          sense = 1;
          நிறுத்து;
        நிலை '-':
          sense = 0;
          நிறுத்து;
        நிலை 'g':
          want_state_putglyph = sense;
          நிறுத்து;
        நிலை 's':
          want_scrollrect = sense;
          நிறுத்து;
        நிலை 'P':
          want_premove = sense;
          நிறுத்து;
        நிலை 'm':
          want_moverect = sense;
          நிறுத்து;
        நிலை 'e':
          want_state_erase = sense;
          நிறுத்து;
        நிலை 'p':
          want_settermprop = sense;
          நிறுத்து;
        நிலை 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : வெற்று, வெற்று);
          நிறுத்து;
        நிலை 'b':
          want_state_scrollback = sense;
          நிறுத்து;
        இயல்பு:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", வரி[i]);
        }
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "WANTSCREEN") && (வரி[10] == '\0' || வரி[10] == ' ')) {
      முழு i = 10;
      முழு sense = 1;
      ப்ரதிஜ்ஞா(vt);
      எனில்(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, வெற்று);
      vterm_screen_callbacks_has_pushline4(screen);

      போது(வரி[i] == ' ')
        i++;
      க்கு( ; வரி[i]; i++)
        தேர்வு(வரி[i]) {
        நிலை '-':
          sense = 0;
          நிறுத்து;
        நிலை 'a':
          vterm_screen_enable_altscreen(screen, 1);
          நிறுத்து;
        நிலை 'd':
          want_screen_damage = sense;
          நிறுத்து;
        நிலை 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          நிறுத்து;
        நிலை 'm':
          want_moverect = sense;
          நிறுத்து;
        நிலை 'c':
          want_movecursor = sense;
          நிறுத்து;
        நிலை 'p':
          want_settermprop = 1;
          நிறுத்து;
        நிலை 'b':
          want_screen_scrollback = sense;
          நிறுத்து;
        நிலை 'r':
          vterm_screen_enable_reflow(screen, sense);
          நிறுத்து;
        இயல்பு:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", வரி[i]);
        }
    }

    இல்லையெனில் எனில்(sscanf(வரி, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    இல்லையெனில் எனில்(streq(வரி, "RESET")) {
      எனில்(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      எனில்(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "RESIZE ")) {
      முழு rows, cols;
      எழுத்து *linep = வரி + 7;
      போது(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "PUSH ")) {
      எழுத்து *bytes = வரி + 5;
      size_t len = inplace_hex2bytes(bytes);
      ப்ரதிஜ்ஞா(len);

      size_t written = vterm_உள்ளீடு_write(vt, bytes, len);
      எனில்(written < len)
        fprintf(stderr, "! short write\n");
    }

    இல்லையெனில் எனில்(streq(வரி, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      எனில்(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "ENCIN ")) {
      எழுத்து *bytes = வரி + 6;
      size_t len = inplace_hex2bytes(bytes);
      ப்ரதிஜ்ஞா(len);

      uint32_t cp[1024];
      முழு cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      எனில்(cpi > 0) {
	முழு i;
        printf("encout ");
        க்கு(i = 0; i < cpi; i++) {
          printf(i ? ",%x" : "%x", cp[i]);
        }
        printf("\n");
      }
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "INCHAR ")) {
      எழுத்து *linep = வரி + 7;
      unsigned முழு c = 0;
      VTermModifier mod;
      போது(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      sscanf(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "INKEY ")) {
      VTermModifier mod;
      VTerm_சாவி சாவி;
      எழுத்து *linep = வரி + 6;
      போது(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      போது(linep[0] == ' ')
        linep++;
      சாவி = strp_சாவி(linep);

      vterm_keyboard_சாவி(vt, சாவி, mod);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "PASTE ")) {
      எழுத்து *linep = வரி + 6;
      எனில்(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      இல்லையெனில் எனில்(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      இல்லையெனில்
        செல் abort_வரி;
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "FOCUS ")) {
      ப்ரதிஜ்ஞா(state);
      எழுத்து *linep = வரி + 6;
      எனில்(streq(linep, "IN"))
        vterm_state_focus_in(state);
      இல்லையெனில் எனில்(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      இல்லையெனில்
        செல் abort_வரி;
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "MOUSEMOVE ")) {
      எழுத்து *linep = வரி + 10;
      முழு row, col, len;
      VTermModifier mod;
      போது(linep[0] == ' ')
        linep++;
      sscanf(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      போது(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "MOUSEBTN ")) {
      எழுத்து *linep = வரி + 9;
      எழுத்து press;
      முழு button, len;
      VTermModifier mod;
      போது(linep[0] == ' ')
        linep++;
      sscanf(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      போது(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "SELECTION ")) {
      எழுத்து *linep = வரி + 10;
      unsigned முழு mask;
      முழு len;
      VTermStringFragment frag = { 0 };
      sscanf(linep, "%x%n", &mask, &len);
      linep += len;
      போது(linep[0] == ' ')
        linep++;
      எனில்(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        போது(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      ப்ரதிஜ்ஞா(frag.len);

      linep += frag.len * 2;
      போது(linep[0] == ' ')
        linep++;
      எனில்(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "DAMAGEMERGE ")) {
      ப்ரதிஜ்ஞா(screen);
      எழுத்து *linep = வரி + 12;
      போது(linep[0] == ' ')
        linep++;
      எனில்(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      இல்லையெனில் எனில்(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      இல்லையெனில் எனில்(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      இல்லையெனில் எனில்(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "DAMAGEFLUSH")) {
      ப்ரதிஜ்ஞா(screen);
      vterm_screen_flush_damage(screen);
    }

    இல்லையெனில் எனில்(strstartswith(வரி, "SETDEFAULTCOL ")) {
      ப்ரதிஜ்ஞா(screen);
      எழுத்து *linep = வரி + 14;
      போது(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      எனில்(linep[0]) {
        போது(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      இல்லையெனில்
        vterm_screen_set_default_colors(screen, &fg, வெற்று);
    }

    இல்லையெனில் எனில்(வரி[0] == '?') {
      எனில்(streq(வரி, "?cursor")) {
        ப்ரதிஜ்ஞா(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        எனில்(pos.row != state_pos.row)
          printf("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        இல்லையெனில் எனில்(pos.col != state_pos.col)
          printf("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        இல்லையெனில்
          printf("%d,%d\n", state_pos.row, state_pos.col);
      }
      இல்லையெனில் எனில்(strstartswith(வரி, "?pen ")) {
        ப்ரதிஜ்ஞா(state);
        VTerm_மதிப்பு val;
        எழுத்து *linep = வரி + 5;
        போது(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        எனில்(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          எனில்(val.boolean != state_pen.bold)
            printf("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          இல்லையெனில்
            printf("%s\n", BOOLSTR(state_pen.bold));
        }
        இல்லையெனில் எனில்(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          எனில்(val.boolean != state_pen.underline)
            printf("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          இல்லையெனில்
            printf("%d\n", state_pen.underline);
        }
        இல்லையெனில் எனில்(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          எனில்(val.boolean != state_pen.italic)
            printf("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          இல்லையெனில்
            printf("%s\n", BOOLSTR(state_pen.italic));
        }
        இல்லையெனில் எனில்(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          எனில்(val.boolean != state_pen.blink)
            printf("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          இல்லையெனில்
            printf("%s\n", BOOLSTR(state_pen.blink));
        }
        இல்லையெனில் எனில்(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          எனில்(val.boolean != state_pen.reverse)
            printf("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          இல்லையெனில்
            printf("%s\n", BOOLSTR(state_pen.reverse));
        }
        இல்லையெனில் எனில்(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          எனில்(val.boolean != state_pen.font)
            printf("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          இல்லையெனில்
            printf("%d\n", state_pen.font);
        }
        இல்லையெனில் எனில்(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          எனில்(val.boolean != state_pen.small)
            printf("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          இல்லையெனில்
            printf("%s\n", BOOLSTR(state_pen.small));
        }
        இல்லையெனில் எனில்(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          எனில்(val.எண் != state_pen.baseline)
            printf("! pen baseline mismatch: state=%d, event=%d\n",
                val.எண், state_pen.baseline);
          இல்லையெனில்
            printf("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        இல்லையெனில் எனில்(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          printf("\n");
        }
        இல்லையெனில் எனில்(streq(linep, "background")) {
          print_color(&state_pen.background);
          printf("\n");
        }
        இல்லையெனில்
          printf("?\n");
      }
      இல்லையெனில் எனில்(strstartswith(வரி, "?lineinfo ")) {
        ப்ரதிஜ்ஞா(state);
        எழுத்து *linep = வரி + 10;
        முழு row;
        மாறிலி VTerm_வரி_Info *info;
        போது(linep[0] == ' ')
          linep++;
        எனில்(sscanf(linep, "%d", &row) < 1) {
          printf("! lineinfo unrecognised input\n");
          செல் abort_வரி;
        }
        info = vterm_state_get_lineinfo(state, row);
        எனில்(info->doublewidth)
          printf("dwl ");
        எனில்(info->doubleheight)
          printf("dhl ");
        எனில்(info->continuation)
          printf("cont ");
        printf("\n");
      }
      இல்லையெனில் எனில்(strstartswith(வரி, "?screen_chars ")) {
        ப்ரதிஜ்ஞா(screen);
        எழுத்து *linep = வரி + 13;
        VTermRect rect;
        size_t len;
        போது(linep[0] == ' ')
          linep++;
        எனில்(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        இல்லையெனில் எனில்(sscanf(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, வெற்று, &rect.end_col);
        }
        இல்லையெனில் {
          printf("! screen_chars unrecognised input\n");
          செல் abort_வரி;
        }
        len = vterm_screen_get_chars(screen, வெற்று, 0, rect);
        எனில்(len == (size_t)-1)
          printf("! screen_chars error\n");
        இல்லையெனில் எனில்(len == 0)
          printf("\n");
        இல்லையெனில் {
          uint32_t *chars = malloc(அளவு(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          க்கு(i = 0; i < len; i++) {
            printf("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          விடுவி(chars);
        }
      }
      இல்லையெனில் எனில்(strstartswith(வரி, "?screen_text ")) {
        ப்ரதிஜ்ஞா(screen);
        எழுத்து *linep = வரி + 12;
        VTermRect rect;
        size_t len;
        போது(linep[0] == ' ')
          linep++;
        எனில்(sscanf(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          printf("! screen_text unrecognised input\n");
          செல் abort_வரி;
        }
        len = vterm_screen_get_text(screen, வெற்று, 0, rect);
        எனில்(len == (size_t)-1)
          printf("! screen_text error\n");
        இல்லையெனில் எனில்(len == 0)
          printf("\n");
        இல்லையெனில் {
          // Put an overwrite guard at both ends of the buffer
          unsigned எழுத்து *buffer = malloc(len + 4);
          unsigned எழுத்து *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (எழுத்து *)text, len, rect);

          எனில்(text[-2] != 0x55 || text[-1] != 0xAA)
            printf("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          இல்லையெனில் எனில்(text[len] != 0x55 || text[len+1] != 0xAA)
            printf("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          இல்லையெனில்
	  {
	    size_t i;
            க்கு(i = 0; i < len; i++) {
              printf("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          விடுவி(buffer);
        }
      }
      இல்லையெனில் எனில்(strstartswith(வரி, "?screen_cell ")) {
        ப்ரதிஜ்ஞா(screen);
        எழுத்து *linep = வரி + 12;
	முழு i;
        VTermPos pos;
        VTermScreenCell cell;
        போது(linep[0] == ' ')
          linep++;
        எனில்(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_cell unrecognised input\n");
          செல் abort_வரி;
        }
        எனில்(!vterm_screen_get_cell(screen, pos, &cell))
          செல் abort_வரி;
        printf("{");
        க்கு(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          printf("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        printf("} width=%d attrs={", cell.width);
        எனில்(cell.attrs.bold)      printf("B");
        எனில்(cell.attrs.underline) printf("U%d", cell.attrs.underline);
        எனில்(cell.attrs.italic)    printf("I");
        எனில்(cell.attrs.blink)     printf("K");
        எனில்(cell.attrs.reverse)   printf("R");
        எனில்(cell.attrs.font)      printf("F%d", cell.attrs.font);
        எனில்(cell.attrs.small)     printf("S");
        எனில்(cell.attrs.baseline)  printf(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        printf("} ");
        எனில்(cell.attrs.dwl)       printf("dwl ");
        எனில்(cell.attrs.dhl)       printf("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        printf("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        printf(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        printf("\n");
      }
      இல்லையெனில் எனில்(strstartswith(வரி, "?screen_eol ")) {
        ப்ரதிஜ்ஞா(screen);
        VTermPos pos;
        எழுத்து *linep = வரி + 12;
        போது(linep[0] == ' ')
          linep++;
        எனில்(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_eol unrecognised input\n");
          செல் abort_வரி;
        }
        printf("%d\n", vterm_screen_is_eol(screen, pos));
      }
      இல்லையெனில் எனில்(strstartswith(வரி, "?screen_attrs_extent ")) {
        ப்ரதிஜ்ஞா(screen);
        VTermPos pos;
        VTermRect rect;
        எழுத்து *linep = வரி + 21;
        போது(linep[0] == ' ')
          linep++;
        எனில்(sscanf(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          printf("! screen_attrs_extent unrecognised input\n");
          செல் abort_வரி;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        எனில்(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          printf("! screen_attrs_extent failed\n");
          செல் abort_வரி;
        }
        printf("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      இல்லையெனில்
        printf("?\n");

      memset(வரி, 0, அளவு வரி);
      தொடர்;
    }

    இல்லையெனில்
      abort_வரி: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    எனில்(outlen > 0) {
      எழுத்து outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, வெற்று);
    }

    printf(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  திரும்பு 0;
}
