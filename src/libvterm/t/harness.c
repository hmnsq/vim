#شامل "vterm.h"
#شامل "../src/vterm_internal.h" // We pull in some internal bits too

#شامل <assert.h>
#شامل <stdio.h>
#شامل <string.h>

#تعریف_کرو streq(a,b) (!strcmp(a,b))
#تعریف_کرو strstartswith(a,b) (!لڑی_موازنہ(a,b,لڑی_لمبائی(b)))

ساکن size_t inplace_hex2bytes(حرف *s)
{
  حرف *inpos = s, *outpos = s;

  جب(*inpos) {
    غیرعلامتی صحیح ch;
    اگر(لڑی_سے_پڑھو(inpos, "%2x", &ch) < 1)
      توڑو;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  واپس outpos - s;
}

ساکن VTermModifier strpe_modifiers(حرف **strp)
{
  VTermModifier state = 0;

  جب((*strp)[0]) {
    انتخاب(((*strp)++)[0]) {
      حالت 'S': state |= VTERM_MOD_سرکاؤ; توڑو;
      حالت 'C': state |= VTERM_MOD_CTRL;  توڑو;
      حالت 'A': state |= VTERM_MOD_ALT;   توڑو;
      طےشدہ: واپس state;
    }
  }

  واپس state;
}

ساکن VTerm_کلید strp_کلید(حرف *str)
{
  ساکن ساخت {
    حرف *name;
    VTerm_کلید کلید;
  } کلیدیں[] = {
    { "Up",    VTERM_کلید_اوپر },
    { "Tab",   VTERM_کلید_TAB },
    { "Enter", VTERM_کلید_ENTER },
    { "KP0",   VTERM_کلید_KP_0 },
    { "F1",    VTERM_کلید_FUNCTION(1) },
    { تہی,    VTERM_کلید_NONE },
  };
  صحیح i;

  برائے(i = 0; کلیدیں[i].name; i++) {
    اگر(streq(str, کلیدیں[i].name))
      واپس کلیدیں[i].کلید;
  }

  واپس VTERM_کلید_NONE;
}

ساکن خالی print_color(ثابت VTermColor *ستون)
{
  اگر (VTERM_COLOR_IS_RGB(ستون)) {
    لکھو("rgb(%d,%d,%d", ستون->red, ستون->green, ستون->blue);
  }
  ورنہ اگر (VTERM_COLOR_IS_اشاریہ_دار(ستون)) {
    لکھو("idx(%d", ستون->اشاریہ);
  }
  ورنہ {
    لکھو("invalid(%d", ستون->type);
  }
  اگر (VTERM_COLOR_IS_DEFAULT_FG(ستون)) {
    لکھو(",is_default_fg");
  }
  اگر (VTERM_COLOR_IS_DEFAULT_BG(ستون)) {
    لکھو(",is_default_bg");
  }
  لکھو(")");
}

ساکن VTermColor strpe_color(حرف **strp)
{
  uint8_t r, g, b, idx;
  صحیح len = 0;
  VTermColor ستون;

  اگر(لڑی_سے_پڑھو(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&ستون, r, g, b);
  }
  ورنہ اگر(لڑی_سے_پڑھو(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_اشاریہ_دار(&ستون, idx);
  }
  ورنہ
    vterm_color_rgb(&ستون, 127, 127, 127);

  واپس ستون;
}

ساکن VTerm *vt;
ساکن VTermState *state;
ساکن VTermScreen *screen;

ساکن VTermEncodingInstance encoding;

ساکن خالی رکن_output(ثابت حرف *s, size_t len, خالی *user غیر_مستعمل)
{
  size_t i;

  لکھو("output ");
  برائے(i = 0; i < len; i++)
    لکھو("%x%s", (غیرعلامتی حرف)s[i], i < len-1 ? "," : "\n");
}

ساکن خالی printhex(ثابت حرف *s, size_t len)
{
  جب(len--)
    لکھو("%02x", (uint8_t)(s++)[0]);
}

ساکن صحیح parser_متن(ثابت حرف بائٹیں[], size_t len, خالی *user غیر_مستعمل)
{
  size_t i;

  لکھو("text ");
  برائے(i = 0; i < len; i++) {
    غیرعلامتی حرف b = بائٹیں[i];
    اگر(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      توڑو;
    لکھو(i ? ",%x" : "%x", b);
  }
  لکھو("\n");

  واپس i;
}

ساکن صحیح parser_control(غیرعلامتی حرف control, خالی *user غیر_مستعمل)
{
  لکھو("control %02x\n", control);

  واپس 1;
}

ساکن صحیح parser_escape(ثابت حرف بائٹیں[], size_t len, خالی *user غیر_مستعمل)
{
  اگر(بائٹیں[0] >= 0x20 && بائٹیں[0] < 0x30) {
    اگر(len < 2)
      واپس -1;
    len = 2;
  }
  ورنہ {
    len = 1;
  }

  لکھو("escape ");
  printhex(بائٹیں, len);
  لکھو("\n");

  واپس len;
}

ساکن صحیح parser_csi(ثابت حرف *leader, ثابت طویل دلائل[], صحیح argcount, ثابت حرف *intermed, حرف command, خالی *user غیر_مستعمل)
{
  صحیح i;
  لکھو("csi %02x", command);

  اگر(leader && leader[0]) {
    لکھو(" L=");
    برائے(i = 0; leader[i]; i++)
      لکھو("%02x", leader[i]);
  }

  برائے(i = 0; i < argcount; i++) {
    حرف sep = i ? ',' : ' ';

    اگر(دلائل[i] == CSI_ARG_MISSING)
      لکھو("%c*", sep);
    ورنہ
      لکھو("%c%ld%s", sep, CSI_ARG(دلائل[i]), CSI_ARG_HAS_مزید(دلائل[i]) ? "+" : "");
  }

  اگر(intermed && intermed[0]) {
    لکھو(" I=");
    برائے(i = 0; intermed[i]; i++)
      لکھو("%02x", intermed[i]);
  }

  لکھو("\n");

  واپس 1;
}

ساکن صحیح parser_osc(صحیح command, VTermStringFragment frag, خالی *user غیر_مستعمل)
{

  لکھو("osc ");

  اگر(frag.initial) {
    اگر(command == -1)
      لکھو("[");
    ورنہ
      لکھو("[%d;", command);
  }

  printhex(frag.str, frag.len);

  اگر(frag.final)
    لکھو("]");

  لکھو("\n");

  واپس 1;
}

ساکن صحیح parser_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, خالی *user غیر_مستعمل)
{
  لکھو("dcs ");

  اگر(frag.initial) {
    size_t i;
    لکھو("[");
    برائے(i = 0; i < commandlen; i++)
      لکھو("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  اگر(frag.final)
    لکھو("]");

  لکھو("\n");

  واپس 1;
}

ساکن صحیح parser_apc(VTermStringFragment frag, خالی *user غیر_مستعمل)
{
  لکھو("apc ");

  اگر(frag.initial)
    لکھو("[");

  printhex(frag.str, frag.len);

  اگر(frag.final)
    لکھو("]");

  لکھو("\n");

  واپس 1;
}

ساکن صحیح parser_pm(VTermStringFragment frag, خالی *user غیر_مستعمل)
{
  لکھو("pm ");

  اگر(frag.initial)
    لکھو("[");

  printhex(frag.str, frag.len);

  اگر(frag.final)
    لکھو("]");

  لکھو("\n");

  واپس 1;
}

ساکن صحیح parser_sos(VTermStringFragment frag, خالی *user غیر_مستعمل)
{
  لکھو("sos ");

  اگر(frag.initial)
    لکھو("[");

  printhex(frag.str, frag.len);

  اگر(frag.final)
    لکھو("]");

  لکھو("\n");

  واپس 1;
}

ساکن VTermParserCallbacks parser_cbs = {
  parser_متن, //متن
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  تہی // resize
};

ساکن VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

ساکن صحیح want_movecursor = 0;
ساکن VTermPos state_pos;
ساکن صحیح movecursor(VTermPos pos, VTermPos oldpos غیر_مستعمل, صحیح visible غیر_مستعمل, خالی *user غیر_مستعمل)
{
  state_pos = pos;

  اگر(want_movecursor)
    لکھو("movecursor %d,%d\n", pos.صف, pos.ستون);

  واپس 1;
}

ساکن صحیح want_premove = 0;
ساکن صحیح premove(VTermRect rect, خالی *user غیر_مستعمل)
{
  اگر(!want_premove)
    واپس 0;

  لکھو("premove %d..%d,%d..%d\n",
      rect.آغاز_صف, rect.انتہا_صف, rect.آغاز_ستون, rect.انتہا_ستون);

  واپس 1;
}

ساکن صحیح want_scrollrect = 0;
ساکن صحیح scrollrect(VTermRect rect, صحیح downward, صحیح rightward, خالی *user غیر_مستعمل)
{
  اگر(!want_scrollrect)
    واپس 0;

  لکھو("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.آغاز_صف, rect.انتہا_صف, rect.آغاز_ستون, rect.انتہا_ستون,
      downward, rightward);

  واپس 1;
}

ساکن صحیح want_moverect = 0;
ساکن صحیح moverect(VTermRect dest, VTermRect src, خالی *user غیر_مستعمل)
{
  اگر(!want_moverect)
    واپس 0;

  لکھو("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.آغاز_صف,  src.انتہا_صف,  src.آغاز_ستون,  src.انتہا_ستون,
      dest.آغاز_صف, dest.انتہا_صف, dest.آغاز_ستون, dest.انتہا_ستون);

  واپس 1;
}

ساکن صحیح want_settermprop = 0;
ساکن صحیح settermprop(VTermProp prop, VTerm_قدر *val, خالی *user غیر_مستعمل)
{
  VTerm_قدر_Type type;
  اگر(!want_settermprop)
    واپس 1;

  type = vterm_get_prop_type(prop);
  انتخاب(type) {
  حالت VTERM_VALUETYPE_BOOL:
    لکھو("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    واپس 1;
  حالت VTERM_VALUETYPE_INT:
    لکھو("settermprop %d %d\n", prop, val->نمبر);
    واپس 1;
  حالت VTERM_VALUETYPE_STRING:
    لکھو("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    واپس 1;
  حالت VTERM_VALUETYPE_COLOR:
    لکھو("settermprop %d ", prop);
    print_color(&val->color);
    لکھو("\n");
    واپس 1;

  حالت VTERM_N_VALUETYPES:
    واپس 0;
  }

  واپس 0;
}

// These callbacks are for State

ساکن صحیح want_state_putglyph = 0;
ساکن صحیح state_putglyph(VTermGlyphInfo *info, VTermPos pos, خالی *user غیر_مستعمل)
{
  صحیح i;
  اگر(!want_state_putglyph)
    واپس 1;

  لکھو("putglyph ");
  برائے(i = 0; i < VTERM_MAX_CHARS_فی_CELL && info->chars[i]; i++)
    لکھو(i ? ",%x" : "%x", info->chars[i]);
  لکھو(" %d %d,%d", info->چوڑائی, pos.صف, pos.ستون);
  اگر(info->protected_cell)
    لکھو(" prot");
  اگر(info->dwl)
    لکھو(" dwl");
  اگر(info->dhl)
    لکھو(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  لکھو("\n");

  واپس 1;
}

ساکن صحیح want_state_erase = 0;
ساکن صحیح state_erase(VTermRect rect, صحیح selective, خالی *user غیر_مستعمل)
{
  اگر(!want_state_erase)
    واپس 1;

  لکھو("erase %d..%d,%d..%d%s\n",
      rect.آغاز_صف, rect.انتہا_صف, rect.آغاز_ستون, rect.انتہا_ستون,
      selective ? " selective" : "");

  واپس 1;
}

ساکن ساخت {
  صحیح bold;
  صحیح underline;
  صحیح italic;
  صحیح blink;
  صحیح الٹ;
  صحیح conceal;
  صحیح strike;
  صحیح font;
  صحیح small;
  صحیح baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ساکن صحیح state_setpenattr(VTermAttr attr, VTerm_قدر *val, خالی *user غیر_مستعمل)
{
  انتخاب(attr) {
  حالت VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    توڑو;
  حالت VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->نمبر;
    توڑو;
  حالت VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    توڑو;
  حالت VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    توڑو;
  حالت VTERM_ATTR_الٹ:
    state_pen.الٹ = val->boolean;
    توڑو;
  حالت VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    توڑو;
  حالت VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    توڑو;
  حالت VTERM_ATTR_FONT:
    state_pen.font = val->نمبر;
    توڑو;
  حالت VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    توڑو;
  حالت VTERM_ATTR_BASELINE:
    state_pen.baseline = val->نمبر;
    توڑو;
  حالت VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    توڑو;
  حالت VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    توڑو;

  حالت VTERM_N_ATTRS:
    واپس 0;
  }

  واپس 1;
}

ساکن صحیح state_setlineinfo(صحیح صف غیر_مستعمل, ثابت VTerm_سطر_Info *newinfo غیر_مستعمل, ثابت VTerm_سطر_Info *oldinfo غیر_مستعمل, خالی *user غیر_مستعمل)
{
  واپس 1;
}

ساکن صحیح want_state_scrollback = 0;
ساکن صحیح state_sb_clear(خالی *user غیر_مستعمل) {
  اگر(!want_state_scrollback)
    واپس 1;

  لکھو("sb_clear\n");
  واپس 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  تہی, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  تہی, // bell
  تہی, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

ساکن صحیح selection_سیٹ(VTerm_Selection_نقاب نقاب, VTermStringFragment frag, خالی *user غیر_مستعمل)
{
  لکھو("selection-set mask=%04X ", نقاب);
  اگر(frag.initial)
    لکھو("[");
  printhex(frag.str, frag.len);
  اگر(frag.final)
    لکھو("]");
  لکھو("\n");

  واپس 1;
}

ساکن صحیح selection_query(VTerm_Selection_نقاب نقاب, خالی *user غیر_مستعمل)
{
  لکھو("selection-query mask=%04X\n", نقاب);

  واپس 1;
}

VTermSelectionCallbacks selection_cbs = {
  .سیٹ_2   = selection_سیٹ,
  .query = selection_query,
};

ساکن صحیح want_screen_damage = 0;
ساکن صحیح want_screen_damage_cells = 0;
ساکن صحیح screen_damage(VTermRect rect, خالی *user غیر_مستعمل)
{
  اگر(!want_screen_damage)
    واپس 1;

  لکھو("damage %d..%d,%d..%d",
      rect.آغاز_صف, rect.انتہا_صف, rect.آغاز_ستون, rect.انتہا_ستون);

  اگر(want_screen_damage_cells) {
    صحیح equals = FALSE;
    صحیح صف;
    صحیح ستون;

    برائے(صف = rect.آغاز_صف; صف < rect.انتہا_صف; صف++) {
      صحیح eol = rect.انتہا_ستون;
      جب(eol > rect.آغاز_ستون) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.صف = صف;
	pos.ستون = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        اگر(cell.chars[0])
          توڑو;

        eol--;
      }

      اگر(eol == rect.آغاز_ستون)
        توڑو;

      اگر(!equals)
        لکھو(" ="), equals = TRUE;

      لکھو(" %d<", صف);
      برائے(ستون = rect.آغاز_ستون; ستون < eol; ستون++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.صف = صف;
	pos.ستون = ستون;
        vterm_screen_get_cell(screen, pos, &cell);
        لکھو(ستون == rect.آغاز_ستون ? "%02X" : " %02X", cell.chars[0]);
      }
      لکھو(">");
    }
  }

  لکھو("\n");

  واپس 1;
}

ساکن صحیح want_screen_scrollback = 0;
ساکن صحیح screen_sb_pushline4(صحیح ستون_تعداد, ثابت VTermScreenCell *cells, صحیح continuation, خالی *user غیر_مستعمل)
{
  صحیح eol;
  صحیح c;

  اگر(!want_screen_scrollback)
    واپس 1;

  eol = ستون_تعداد;
  جب(eol && !cells[eol-1].chars[0])
    eol--;

  لکھو("sb_pushline %d%s =", ستون_تعداد, continuation ? " cont" : "");
  برائے(c = 0; c < eol; c++)
    لکھو(" %02X", cells[c].chars[0]);
  لکھو("\n");

  واپس 1;
}

ساکن صحیح screen_sb_popline(صحیح ستون_تعداد, VTermScreenCell *cells, خالی *user غیر_مستعمل)
{
  صحیح ستون;

  اگر(!want_screen_scrollback)
    واپس 0;

  // All lines of scrollback contain "ABCDE"
  برائے(ستون = 0; ستون < ستون_تعداد; ستون++) {
    اگر(ستون < 5)
      cells[ستون].chars[0] = 'A' + ستون;
    ورنہ
      cells[ستون].chars[0] = 0;

    cells[ستون].چوڑائی = 1;
  }

  لکھو("sb_popline %d\n", ستون_تعداد);
  واپس 1;
}

ساکن صحیح screen_sb_clear(خالی *user غیر_مستعمل)
{
  اگر(!want_screen_scrollback)
    واپس 1;

  لکھو("sb_clear\n");
  واپس 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  تہی, // bell
  تہی, // resize
  تہی, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

صحیح مین(صحیح argc غیر_مستعمل, حرف **argv غیر_مستعمل)
{
  حرف سطر[1024] = {0};
  صحیح flag;

  صحیح err;

  setvbuf(stdout, تہی, _IONBF, 0);

  جب(fgets(سطر, حجم سطر, stdin)) {
    حرف *nl;
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

    ورنہ اگر(streq(سطر, "WANTPARSER")) {
      أكد(vt);
      vterm_parser_سیٹ_callbacks(vt, &parser_cbs, تہی);
    }

    ورنہ اگر(strstartswith(سطر, "WANTSTATE") && (سطر[9] == '\0' || سطر[9] == ' ')) {
      صحیح i = 9;
      صحیح sense = 1;
      أكد(vt);
      اگر(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_سیٹ_callbacks(state, &state_cbs, تہی);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_سیٹ_selection_callbacks(state, &selection_cbs, تہی, تہی, 16);
        vterm_state_سیٹ_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      جب(سطر[i] == ' ')
        i++;
      برائے( ; سطر[i]; i++)
        انتخاب(سطر[i]) {
        حالت '+':
          sense = 1;
          توڑو;
        حالت '-':
          sense = 0;
          توڑو;
        حالت 'g':
          want_state_putglyph = sense;
          توڑو;
        حالت 's':
          want_scrollrect = sense;
          توڑو;
        حالت 'P':
          want_premove = sense;
          توڑو;
        حالت 'm':
          want_moverect = sense;
          توڑو;
        حالت 'e':
          want_state_erase = sense;
          توڑو;
        حالت 'p':
          want_settermprop = sense;
          توڑو;
        حالت 'f':
          vterm_state_سیٹ_unrecognised_fallbacks(state, sense ? &fallbacks : تہی, تہی);
          توڑو;
        حالت 'b':
          want_state_scrollback = sense;
          توڑو;
        طےشدہ:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", سطر[i]);
        }
    }

    ورنہ اگر(strstartswith(سطر, "WANTSCREEN") && (سطر[10] == '\0' || سطر[10] == ' ')) {
      صحیح i = 10;
      صحیح sense = 1;
      أكد(vt);
      اگر(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_سیٹ_callbacks(screen, &screen_cbs, تہی);
      vterm_screen_callbacks_has_pushline4(screen);

      جب(سطر[i] == ' ')
        i++;
      برائے( ; سطر[i]; i++)
        انتخاب(سطر[i]) {
        حالت '-':
          sense = 0;
          توڑو;
        حالت 'a':
          vterm_screen_enable_altscreen(screen, 1);
          توڑو;
        حالت 'd':
          want_screen_damage = sense;
          توڑو;
        حالت 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          توڑو;
        حالت 'm':
          want_moverect = sense;
          توڑو;
        حالت 'c':
          want_movecursor = sense;
          توڑو;
        حالت 'p':
          want_settermprop = 1;
          توڑو;
        حالت 'b':
          want_screen_scrollback = sense;
          توڑو;
        حالت 'r':
          vterm_screen_enable_reflow(screen, sense);
          توڑو;
        طےشدہ:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", سطر[i]);
        }
    }

    ورنہ اگر(لڑی_سے_پڑھو(سطر, "UTF8 %d", &flag)) {
      vterm_سیٹ_utf8(vt, flag);
    }

    ورنہ اگر(streq(سطر, "RESET")) {
      اگر(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      اگر(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    ورنہ اگر(strstartswith(سطر, "RESIZE ")) {
      صحیح صفیں_2, ستون_تعداد;
      حرف *linep = سطر + 7;
      جب(linep[0] == ' ')
        linep++;
      لڑی_سے_پڑھو(linep, "%d, %d", &صفیں_2, &ستون_تعداد);
      vterm_سیٹ_سائز(vt, صفیں_2, ستون_تعداد);
    }

    ورنہ اگر(strstartswith(سطر, "PUSH ")) {
      حرف *بائٹیں = سطر + 5;
      size_t len = inplace_hex2bytes(بائٹیں);
      أكد(len);

      size_t written = vterm_داخلہ_write(vt, بائٹیں, len);
      اگر(written < len)
        fprintf(stderr, "! short write\n");
    }

    ورنہ اگر(streq(سطر, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      اگر(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.ڈیٹا);
    }

    ورنہ اگر(strstartswith(سطر, "ENCIN ")) {
      حرف *بائٹیں = سطر + 6;
      size_t len = inplace_hex2bytes(بائٹیں);
      أكد(len);

      uint32_t cp[1024];
      صحیح cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.ڈیٹا,
          cp, &cpi, len, بائٹیں, &pos, len);

      اگر(cpi > 0) {
	صحیح i;
        لکھو("encout ");
        برائے(i = 0; i < cpi; i++) {
          لکھو(i ? ",%x" : "%x", cp[i]);
        }
        لکھو("\n");
      }
    }

    ورنہ اگر(strstartswith(سطر, "INCHAR ")) {
      حرف *linep = سطر + 7;
      غیرعلامتی صحیح c = 0;
      VTermModifier mod;
      جب(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      لڑی_سے_پڑھو(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    ورنہ اگر(strstartswith(سطر, "INKEY ")) {
      VTermModifier mod;
      VTerm_کلید کلید;
      حرف *linep = سطر + 6;
      جب(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      جب(linep[0] == ' ')
        linep++;
      کلید = strp_کلید(linep);

      vterm_keyboard_کلید(vt, کلید, mod);
    }

    ورنہ اگر(strstartswith(سطر, "PASTE ")) {
      حرف *linep = سطر + 6;
      اگر(streq(linep, "START"))
        vterm_keyboard_آغاز_paste(vt);
      ورنہ اگر(streq(linep, "END"))
        vterm_keyboard_انتہا_paste(vt);
      ورنہ
        جاؤ abort_سطر;
    }

    ورنہ اگر(strstartswith(سطر, "FOCUS ")) {
      أكد(state);
      حرف *linep = سطر + 6;
      اگر(streq(linep, "IN"))
        vterm_state_focus_اندر(state);
      ورنہ اگر(streq(linep, "OUT"))
        vterm_state_focus_برآمد(state);
      ورنہ
        جاؤ abort_سطر;
    }

    ورنہ اگر(strstartswith(سطر, "MOUSEMOVE ")) {
      حرف *linep = سطر + 10;
      صحیح صف, ستون, len;
      VTermModifier mod;
      جب(linep[0] == ' ')
        linep++;
      لڑی_سے_پڑھو(linep, "%d,%d%n", &صف, &ستون, &len);
      linep += len;
      جب(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, صف, ستون, mod);
    }

    ورنہ اگر(strstartswith(سطر, "MOUSEBTN ")) {
      حرف *linep = سطر + 9;
      حرف press;
      صحیح button, len;
      VTermModifier mod;
      جب(linep[0] == ' ')
        linep++;
      لڑی_سے_پڑھو(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      جب(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    ورنہ اگر(strstartswith(سطر, "SELECTION ")) {
      حرف *linep = سطر + 10;
      غیرعلامتی صحیح نقاب;
      صحیح len;
      VTermStringFragment frag = { 0 };
      لڑی_سے_پڑھو(linep, "%x%n", &نقاب, &len);
      linep += len;
      جب(linep[0] == ' ')
        linep++;
      اگر(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        جب(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      أكد(frag.len);

      linep += frag.len * 2;
      جب(linep[0] == ' ')
        linep++;
      اگر(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, نقاب, frag);
    }

    ورنہ اگر(strstartswith(سطر, "DAMAGEMERGE ")) {
      أكد(screen);
      حرف *linep = سطر + 12;
      جب(linep[0] == ' ')
        linep++;
      اگر(streq(linep, "CELL"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_CELL);
      ورنہ اگر(streq(linep, "ROW"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_صف);
      ورنہ اگر(streq(linep, "SCREEN"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_SCREEN);
      ورنہ اگر(streq(linep, "SCROLL"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_SCROLL);
    }

    ورنہ اگر(strstartswith(سطر, "DAMAGEFLUSH")) {
      أكد(screen);
      vterm_screen_flush_damage(screen);
    }

    ورنہ اگر(strstartswith(سطر, "SETDEFAULTCOL ")) {
      أكد(screen);
      حرف *linep = سطر + 14;
      جب(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      اگر(linep[0]) {
        جب(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_سیٹ_default_colors(screen, &fg, &bg);
      }
      ورنہ
        vterm_screen_سیٹ_default_colors(screen, &fg, تہی);
    }

    ورنہ اگر(سطر[0] == '?') {
      اگر(streq(سطر, "?cursor")) {
        أكد(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        اگر(pos.صف != state_pos.صف)
          لکھو("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.صف, pos.ستون, state_pos.صف, state_pos.ستون);
        ورنہ اگر(pos.ستون != state_pos.ستون)
          لکھو("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.صف, pos.ستون, state_pos.صف, state_pos.ستون);
        ورنہ
          لکھو("%d,%d\n", state_pos.صف, state_pos.ستون);
      }
      ورنہ اگر(strstartswith(سطر, "?pen ")) {
        أكد(state);
        VTerm_قدر val;
        حرف *linep = سطر + 5;
        جب(linep[0] == ' ')
          linep++;

#تعریف_کرو BOOLSTR(v) ((v) ? "on" : "off")

        اگر(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          اگر(val.boolean != state_pen.bold)
            لکھو("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          ورنہ
            لکھو("%s\n", BOOLSTR(state_pen.bold));
        }
        ورنہ اگر(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          اگر(val.boolean != state_pen.underline)
            لکھو("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          ورنہ
            لکھو("%d\n", state_pen.underline);
        }
        ورنہ اگر(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          اگر(val.boolean != state_pen.italic)
            لکھو("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          ورنہ
            لکھو("%s\n", BOOLSTR(state_pen.italic));
        }
        ورنہ اگر(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          اگر(val.boolean != state_pen.blink)
            لکھو("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          ورنہ
            لکھو("%s\n", BOOLSTR(state_pen.blink));
        }
        ورنہ اگر(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_الٹ, &val);
          اگر(val.boolean != state_pen.الٹ)
            لکھو("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.الٹ));
          ورنہ
            لکھو("%s\n", BOOLSTR(state_pen.الٹ));
        }
        ورنہ اگر(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          اگر(val.boolean != state_pen.font)
            لکھو("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          ورنہ
            لکھو("%d\n", state_pen.font);
        }
        ورنہ اگر(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          اگر(val.boolean != state_pen.small)
            لکھو("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          ورنہ
            لکھو("%s\n", BOOLSTR(state_pen.small));
        }
        ورنہ اگر(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          اگر(val.نمبر != state_pen.baseline)
            لکھو("! pen baseline mismatch: state=%d, event=%d\n",
                val.نمبر, state_pen.baseline);
          ورنہ
            لکھو("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_زیریں ? "lower"
                         : "normal");
        }
        ورنہ اگر(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          لکھو("\n");
        }
        ورنہ اگر(streq(linep, "background")) {
          print_color(&state_pen.background);
          لکھو("\n");
        }
        ورنہ
          لکھو("?\n");
      }
      ورنہ اگر(strstartswith(سطر, "?lineinfo ")) {
        أكد(state);
        حرف *linep = سطر + 10;
        صحیح صف;
        ثابت VTerm_سطر_Info *info;
        جب(linep[0] == ' ')
          linep++;
        اگر(لڑی_سے_پڑھو(linep, "%d", &صف) < 1) {
          لکھو("! lineinfo unrecognised input\n");
          جاؤ abort_سطر;
        }
        info = vterm_state_get_lineinfo(state, صف);
        اگر(info->doublewidth)
          لکھو("dwl ");
        اگر(info->doubleheight)
          لکھو("dhl ");
        اگر(info->continuation)
          لکھو("cont ");
        لکھو("\n");
      }
      ورنہ اگر(strstartswith(سطر, "?screen_chars ")) {
        أكد(screen);
        حرف *linep = سطر + 13;
        VTermRect rect;
        size_t len;
        جب(linep[0] == ' ')
          linep++;
        اگر(لڑی_سے_پڑھو(linep, "%d,%d,%d,%d", &rect.آغاز_صف, &rect.آغاز_ستون, &rect.انتہا_صف, &rect.انتہا_ستون) == 4)
          ; // fine
        ورنہ اگر(لڑی_سے_پڑھو(linep, "%d", &rect.آغاز_صف) == 1) {
          rect.انتہا_صف = rect.آغاز_صف + 1;
          rect.آغاز_ستون = 0;
          vterm_get_سائز(vt, تہی, &rect.انتہا_ستون);
        }
        ورنہ {
          لکھو("! screen_chars unrecognised input\n");
          جاؤ abort_سطر;
        }
        len = vterm_screen_get_chars(screen, تہی, 0, rect);
        اگر(len == (size_t)-1)
          لکھو("! screen_chars error\n");
        ورنہ اگر(len == 0)
          لکھو("\n");
        ورنہ {
          uint32_t *chars = حافظہ_دو(حجم(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          برائے(i = 0; i < len; i++) {
            لکھو("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          آزاد(chars);
        }
      }
      ورنہ اگر(strstartswith(سطر, "?screen_text ")) {
        أكد(screen);
        حرف *linep = سطر + 12;
        VTermRect rect;
        size_t len;
        جب(linep[0] == ' ')
          linep++;
        اگر(لڑی_سے_پڑھو(linep, "%d,%d,%d,%d", &rect.آغاز_صف, &rect.آغاز_ستون, &rect.انتہا_صف, &rect.انتہا_ستون) < 4) {
          لکھو("! screen_text unrecognised input\n");
          جاؤ abort_سطر;
        }
        len = vterm_screen_get_متن(screen, تہی, 0, rect);
        اگر(len == (size_t)-1)
          لکھو("! screen_text error\n");
        ورنہ اگر(len == 0)
          لکھو("\n");
        ورنہ {
          // Put an overwrite guard at both ends of the buffer
          غیرعلامتی حرف *بفر = حافظہ_دو(len + 4);
          غیرعلامتی حرف *متن = بفر + 2;
          متن[-2] = 0x55; متن[-1] = 0xAA;
          متن[len] = 0x55; متن[len+1] = 0xAA;

          vterm_screen_get_متن(screen, (حرف *)متن, len, rect);

          اگر(متن[-2] != 0x55 || متن[-1] != 0xAA)
            لکھو("! screen_get_text buffer overrun left [%02x,%02x]\n", متن[-2], متن[-1]);
          ورنہ اگر(متن[len] != 0x55 || متن[len+1] != 0xAA)
            لکھو("! screen_get_text buffer overrun right [%02x,%02x]\n", متن[len], متن[len+1]);
          ورنہ
	  {
	    size_t i;
            برائے(i = 0; i < len; i++) {
              لکھو("0x%02x%s", متن[i], i < len-1 ? "," : "\n");
            }
	  }

          آزاد(بفر);
        }
      }
      ورنہ اگر(strstartswith(سطر, "?screen_cell ")) {
        أكد(screen);
        حرف *linep = سطر + 12;
	صحیح i;
        VTermPos pos;
        VTermScreenCell cell;
        جب(linep[0] == ' ')
          linep++;
        اگر(لڑی_سے_پڑھو(linep, "%d,%d\n", &pos.صف, &pos.ستون) < 2) {
          لکھو("! screen_cell unrecognised input\n");
          جاؤ abort_سطر;
        }
        اگر(!vterm_screen_get_cell(screen, pos, &cell))
          جاؤ abort_سطر;
        لکھو("{");
        برائے(i = 0; i < VTERM_MAX_CHARS_فی_CELL && cell.chars[i]; i++) {
          لکھو("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        لکھو("} width=%d attrs={", cell.چوڑائی);
        اگر(cell.attrs.bold)      لکھو("B");
        اگر(cell.attrs.underline) لکھو("U%d", cell.attrs.underline);
        اگر(cell.attrs.italic)    لکھو("I");
        اگر(cell.attrs.blink)     لکھو("K");
        اگر(cell.attrs.الٹ)   لکھو("R");
        اگر(cell.attrs.font)      لکھو("F%d", cell.attrs.font);
        اگر(cell.attrs.small)     لکھو("S");
        اگر(cell.attrs.baseline)  لکھو(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        لکھو("} ");
        اگر(cell.attrs.dwl)       لکھو("dwl ");
        اگر(cell.attrs.dhl)       لکھو("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        لکھو("fg=");
        vterm_screen_convert_color_مقصد_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        لکھو(" bg=");
        vterm_screen_convert_color_مقصد_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        لکھو("\n");
      }
      ورنہ اگر(strstartswith(سطر, "?screen_eol ")) {
        أكد(screen);
        VTermPos pos;
        حرف *linep = سطر + 12;
        جب(linep[0] == ' ')
          linep++;
        اگر(لڑی_سے_پڑھو(linep, "%d,%d\n", &pos.صف, &pos.ستون) < 2) {
          لکھو("! screen_eol unrecognised input\n");
          جاؤ abort_سطر;
        }
        لکھو("%d\n", vterm_screen_is_eol(screen, pos));
      }
      ورنہ اگر(strstartswith(سطر, "?screen_attrs_extent ")) {
        أكد(screen);
        VTermPos pos;
        VTermRect rect;
        حرف *linep = سطر + 21;
        جب(linep[0] == ' ')
          linep++;
        اگر(لڑی_سے_پڑھو(linep, "%d,%d\n", &pos.صف, &pos.ستون) < 2) {
          لکھو("! screen_attrs_extent unrecognised input\n");
          جاؤ abort_سطر;
        }
	rect.آغاز_ستون = 0;
	rect.انتہا_ستون   = -1;
        اگر(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          لکھو("! screen_attrs_extent failed\n");
          جاؤ abort_سطر;
        }
        لکھو("%d,%d-%d,%d\n", rect.آغاز_صف, rect.آغاز_ستون, rect.انتہا_صف, rect.انتہا_ستون);
      }
      ورنہ
        لکھو("?\n");

      memset(سطر, 0, حجم سطر);
      جاری;
    }

    ورنہ
      abort_سطر: err = 1;

    outlen = vterm_output_get_بفر_current(vt);
    اگر(outlen > 0) {
      حرف outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      رکن_output(outbuff, outlen, تہی);
    }

    لکھو(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  واپس 0;
}
