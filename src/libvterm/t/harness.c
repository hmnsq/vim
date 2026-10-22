#ضمن "vterm.h"
#ضمن "../src/vterm_internal.h" // We pull in some internal bits too

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#وصف_ڪريو streq(a,b) (!strcmp(a,b))
#وصف_ڪريو strstartswith(a,b) (!لڙي_ڀيٽ(a,b,لڙي_ڊيگهه(b)))

ساكن size_t inplace_hex2bytes(حرف *s)
{
  حرف *inpos = s, *outpos = s;

  طالما(*inpos) {
    بنانشان صحيح ch;
    إذا(لڙي_مان_پڙهو(inpos, "%2x", &ch) < 1)
      اكسر;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  أرجع outpos - s;
}

ساكن VTermModifier strpe_modifiers(حرف **strp)
{
  VTermModifier state = 0;

  طالما((*strp)[0]) {
    بدل(((*strp)++)[0]) {
      حالة 'S': state |= VTERM_MOD_سرڪايو; اكسر;
      حالة 'C': state |= VTERM_MOD_CTRL;  اكسر;
      حالة 'A': state |= VTERM_MOD_ALT;   اكسر;
      افتراضي: أرجع state;
    }
  }

  أرجع state;
}

ساكن VTerm_ڪنجي strp_ڪنجي(حرف *str)
{
  ساكن بنية {
    حرف *name;
    VTerm_ڪنجي ڪنجي;
  } ڪنجيون[] = {
    { "Up",    VTERM_ڪنجي_مٿي },
    { "Tab",   VTERM_ڪنجي_TAB },
    { "Enter", VTERM_ڪنجي_ENTER },
    { "KP0",   VTERM_ڪنجي_KP_0 },
    { "F1",    VTERM_ڪنجي_FUNCTION(1) },
    { عدم,    VTERM_ڪنجي_NONE },
  };
  صحيح i;

  لكل(i = 0; ڪنجيون[i].name; i++) {
    إذا(streq(str, ڪنجيون[i].name))
      أرجع ڪنجيون[i].ڪنجي;
  }

  أرجع VTERM_ڪنجي_NONE;
}

ساكن فراغ print_color(ثابت VTermColor *کانو)
{
  إذا (VTERM_COLOR_IS_RGB(کانو)) {
    لکو("rgb(%d,%d,%d", کانو->red, کانو->green, کانو->blue);
  }
  وإلا إذا (VTERM_COLOR_IS_اشاريي_وارو(کانو)) {
    لکو("idx(%d", کانو->اشاريو);
  }
  وإلا {
    لکو("invalid(%d", کانو->type);
  }
  إذا (VTERM_COLOR_IS_DEFAULT_FG(کانو)) {
    لکو(",is_default_fg");
  }
  إذا (VTERM_COLOR_IS_DEFAULT_BG(کانو)) {
    لکو(",is_default_bg");
  }
  لکو(")");
}

ساكن VTermColor strpe_color(حرف **strp)
{
  uint8_t r, g, b, idx;
  صحيح len = 0;
  VTermColor کانو;

  إذا(لڙي_مان_پڙهو(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&کانو, r, g, b);
  }
  وإلا إذا(لڙي_مان_پڙهو(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_اشاريي_وارو(&کانو, idx);
  }
  وإلا
    vterm_color_rgb(&کانو, 127, 127, 127);

  أرجع کانو;
}

ساكن VTerm *vt;
ساكن VTermState *state;
ساكن VTermScreen *screen;

ساكن VTermEncodingInstance encoding;

ساكن فراغ رڪن_output(ثابت حرف *s, size_t len, فراغ *user اڻ_استعمال_ٿيل)
{
  size_t i;

  لکو("output ");
  لكل(i = 0; i < len; i++)
    لکو("%x%s", (بنانشان حرف)s[i], i < len-1 ? "," : "\n");
}

ساكن فراغ printhex(ثابت حرف *s, size_t len)
{
  طالما(len--)
    لکو("%02x", (uint8_t)(s++)[0]);
}

ساكن صحيح parser_متن(ثابت حرف بائيٽون[], size_t len, فراغ *user اڻ_استعمال_ٿيل)
{
  size_t i;

  لکو("text ");
  لكل(i = 0; i < len; i++) {
    بنانشان حرف b = بائيٽون[i];
    إذا(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      اكسر;
    لکو(i ? ",%x" : "%x", b);
  }
  لکو("\n");

  أرجع i;
}

ساكن صحيح parser_control(بنانشان حرف control, فراغ *user اڻ_استعمال_ٿيل)
{
  لکو("control %02x\n", control);

  أرجع 1;
}

ساكن صحيح parser_escape(ثابت حرف بائيٽون[], size_t len, فراغ *user اڻ_استعمال_ٿيل)
{
  إذا(بائيٽون[0] >= 0x20 && بائيٽون[0] < 0x30) {
    إذا(len < 2)
      أرجع -1;
    len = 2;
  }
  وإلا {
    len = 1;
  }

  لکو("escape ");
  printhex(بائيٽون, len);
  لکو("\n");

  أرجع len;
}

ساكن صحيح parser_csi(ثابت حرف *leader, ثابت ڊگهو دليلون[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user اڻ_استعمال_ٿيل)
{
  صحيح i;
  لکو("csi %02x", command);

  إذا(leader && leader[0]) {
    لکو(" L=");
    لكل(i = 0; leader[i]; i++)
      لکو("%02x", leader[i]);
  }

  لكل(i = 0; i < argcount; i++) {
    حرف sep = i ? ',' : ' ';

    إذا(دليلون[i] == CSI_ARG_MISSING)
      لکو("%c*", sep);
    وإلا
      لکو("%c%ld%s", sep, CSI_ARG(دليلون[i]), CSI_ARG_HAS_وڌيڪ(دليلون[i]) ? "+" : "");
  }

  إذا(intermed && intermed[0]) {
    لکو(" I=");
    لكل(i = 0; intermed[i]; i++)
      لکو("%02x", intermed[i]);
  }

  لکو("\n");

  أرجع 1;
}

ساكن صحيح parser_osc(صحيح command, VTermStringFragment frag, فراغ *user اڻ_استعمال_ٿيل)
{

  لکو("osc ");

  إذا(frag.initial) {
    إذا(command == -1)
      لکو("[");
    وإلا
      لکو("[%d;", command);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکو("]");

  لکو("\n");

  أرجع 1;
}

ساكن صحيح parser_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user اڻ_استعمال_ٿيل)
{
  لکو("dcs ");

  إذا(frag.initial) {
    size_t i;
    لکو("[");
    لكل(i = 0; i < commandlen; i++)
      لکو("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکو("]");

  لکو("\n");

  أرجع 1;
}

ساكن صحيح parser_apc(VTermStringFragment frag, فراغ *user اڻ_استعمال_ٿيل)
{
  لکو("apc ");

  إذا(frag.initial)
    لکو("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکو("]");

  لکو("\n");

  أرجع 1;
}

ساكن صحيح parser_pm(VTermStringFragment frag, فراغ *user اڻ_استعمال_ٿيل)
{
  لکو("pm ");

  إذا(frag.initial)
    لکو("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکو("]");

  لکو("\n");

  أرجع 1;
}

ساكن صحيح parser_sos(VTermStringFragment frag, فراغ *user اڻ_استعمال_ٿيل)
{
  لکو("sos ");

  إذا(frag.initial)
    لکو("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکو("]");

  لکو("\n");

  أرجع 1;
}

ساكن VTermParserCallbacks parser_cbs = {
  parser_متن, //متن
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
ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos اڻ_استعمال_ٿيل, صحيح visible اڻ_استعمال_ٿيل, فراغ *user اڻ_استعمال_ٿيل)
{
  state_pos = pos;

  إذا(want_movecursor)
    لکو("movecursor %d,%d\n", pos.صف, pos.کانو);

  أرجع 1;
}

ساكن صحيح want_premove = 0;
ساكن صحيح premove(VTermRect rect, فراغ *user اڻ_استعمال_ٿيل)
{
  إذا(!want_premove)
    أرجع 0;

  لکو("premove %d..%d,%d..%d\n",
      rect.شروعات_صف, rect.پڇاڙي_صف, rect.شروعات_کانو, rect.پڇاڙي_کانو);

  أرجع 1;
}

ساكن صحيح want_scrollrect = 0;
ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user اڻ_استعمال_ٿيل)
{
  إذا(!want_scrollrect)
    أرجع 0;

  لکو("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.شروعات_صف, rect.پڇاڙي_صف, rect.شروعات_کانو, rect.پڇاڙي_کانو,
      downward, rightward);

  أرجع 1;
}

ساكن صحيح want_moverect = 0;
ساكن صحيح moverect(VTermRect dest, VTermRect src, فراغ *user اڻ_استعمال_ٿيل)
{
  إذا(!want_moverect)
    أرجع 0;

  لکو("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.شروعات_صف,  src.پڇاڙي_صف,  src.شروعات_کانو,  src.پڇاڙي_کانو,
      dest.شروعات_صف, dest.پڇاڙي_صف, dest.شروعات_کانو, dest.پڇاڙي_کانو);

  أرجع 1;
}

ساكن صحيح want_settermprop = 0;
ساكن صحيح settermprop(VTermProp prop, VTerm_قدر *val, فراغ *user اڻ_استعمال_ٿيل)
{
  VTerm_قدر_Type type;
  إذا(!want_settermprop)
    أرجع 1;

  type = vterm_get_prop_type(prop);
  بدل(type) {
  حالة VTERM_VALUETYPE_BOOL:
    لکو("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    أرجع 1;
  حالة VTERM_VALUETYPE_INT:
    لکو("settermprop %d %d\n", prop, val->عدد);
    أرجع 1;
  حالة VTERM_VALUETYPE_STRING:
    لکو("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    أرجع 1;
  حالة VTERM_VALUETYPE_COLOR:
    لکو("settermprop %d ", prop);
    print_color(&val->color);
    لکو("\n");
    أرجع 1;

  حالة VTERM_N_VALUETYPES:
    أرجع 0;
  }

  أرجع 0;
}

// These callbacks are for State

ساكن صحيح want_state_putglyph = 0;
ساكن صحيح state_putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user اڻ_استعمال_ٿيل)
{
  صحيح i;
  إذا(!want_state_putglyph)
    أرجع 1;

  لکو("putglyph ");
  لكل(i = 0; i < VTERM_MAX_CHARS_في_CELL && info->chars[i]; i++)
    لکو(i ? ",%x" : "%x", info->chars[i]);
  لکو(" %d %d,%d", info->ويڪر, pos.صف, pos.کانو);
  إذا(info->protected_cell)
    لکو(" prot");
  إذا(info->dwl)
    لکو(" dwl");
  إذا(info->dhl)
    لکو(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  لکو("\n");

  أرجع 1;
}

ساكن صحيح want_state_erase = 0;
ساكن صحيح state_erase(VTermRect rect, صحيح selective, فراغ *user اڻ_استعمال_ٿيل)
{
  إذا(!want_state_erase)
    أرجع 1;

  لکو("erase %d..%d,%d..%d%s\n",
      rect.شروعات_صف, rect.پڇاڙي_صف, rect.شروعات_کانو, rect.پڇاڙي_کانو,
      selective ? " selective" : "");

  أرجع 1;
}

ساكن بنية {
  صحيح bold;
  صحيح underline;
  صحيح italic;
  صحيح blink;
  صحيح ابتو;
  صحيح conceal;
  صحيح strike;
  صحيح font;
  صحيح small;
  صحيح baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ساكن صحيح state_setpenattr(VTermAttr attr, VTerm_قدر *val, فراغ *user اڻ_استعمال_ٿيل)
{
  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    اكسر;
  حالة VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->عدد;
    اكسر;
  حالة VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    اكسر;
  حالة VTERM_ATTR_ابتو:
    state_pen.ابتو = val->boolean;
    اكسر;
  حالة VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    اكسر;
  حالة VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    اكسر;
  حالة VTERM_ATTR_FONT:
    state_pen.font = val->عدد;
    اكسر;
  حالة VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BASELINE:
    state_pen.baseline = val->عدد;
    اكسر;
  حالة VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    اكسر;
  حالة VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    اكسر;

  حالة VTERM_N_ATTRS:
    أرجع 0;
  }

  أرجع 1;
}

ساكن صحيح state_setlineinfo(صحيح صف اڻ_استعمال_ٿيل, ثابت VTerm_سٽ_Info *newinfo اڻ_استعمال_ٿيل, ثابت VTerm_سٽ_Info *oldinfo اڻ_استعمال_ٿيل, فراغ *user اڻ_استعمال_ٿيل)
{
  أرجع 1;
}

ساكن صحيح want_state_scrollback = 0;
ساكن صحيح state_sb_clear(فراغ *user اڻ_استعمال_ٿيل) {
  إذا(!want_state_scrollback)
    أرجع 1;

  لکو("sb_clear\n");
  أرجع 0;
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

ساكن صحيح selection_سيٽ(VTerm_Selection_نقاب نقاب, VTermStringFragment frag, فراغ *user اڻ_استعمال_ٿيل)
{
  لکو("selection-set mask=%04X ", نقاب);
  إذا(frag.initial)
    لکو("[");
  printhex(frag.str, frag.len);
  إذا(frag.final)
    لکو("]");
  لکو("\n");

  أرجع 1;
}

ساكن صحيح selection_query(VTerm_Selection_نقاب نقاب, فراغ *user اڻ_استعمال_ٿيل)
{
  لکو("selection-query mask=%04X\n", نقاب);

  أرجع 1;
}

VTermSelectionCallbacks selection_cbs = {
  .سيٽ_2   = selection_سيٽ,
  .query = selection_query,
};

ساكن صحيح want_screen_damage = 0;
ساكن صحيح want_screen_damage_cells = 0;
ساكن صحيح screen_damage(VTermRect rect, فراغ *user اڻ_استعمال_ٿيل)
{
  إذا(!want_screen_damage)
    أرجع 1;

  لکو("damage %d..%d,%d..%d",
      rect.شروعات_صف, rect.پڇاڙي_صف, rect.شروعات_کانو, rect.پڇاڙي_کانو);

  إذا(want_screen_damage_cells) {
    صحيح equals = FALSE;
    صحيح صف;
    صحيح کانو;

    لكل(صف = rect.شروعات_صف; صف < rect.پڇاڙي_صف; صف++) {
      صحيح eol = rect.پڇاڙي_کانو;
      طالما(eol > rect.شروعات_کانو) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.صف = صف;
	pos.کانو = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        إذا(cell.chars[0])
          اكسر;

        eol--;
      }

      إذا(eol == rect.شروعات_کانو)
        اكسر;

      إذا(!equals)
        لکو(" ="), equals = TRUE;

      لکو(" %d<", صف);
      لكل(کانو = rect.شروعات_کانو; کانو < eol; کانو++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.صف = صف;
	pos.کانو = کانو;
        vterm_screen_get_cell(screen, pos, &cell);
        لکو(کانو == rect.شروعات_کانو ? "%02X" : " %02X", cell.chars[0]);
      }
      لکو(">");
    }
  }

  لکو("\n");

  أرجع 1;
}

ساكن صحيح want_screen_scrollback = 0;
ساكن صحيح screen_sb_pushline4(صحيح کانن_ڳڻپ, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user اڻ_استعمال_ٿيل)
{
  صحيح eol;
  صحيح c;

  إذا(!want_screen_scrollback)
    أرجع 1;

  eol = کانن_ڳڻپ;
  طالما(eol && !cells[eol-1].chars[0])
    eol--;

  لکو("sb_pushline %d%s =", کانن_ڳڻپ, continuation ? " cont" : "");
  لكل(c = 0; c < eol; c++)
    لکو(" %02X", cells[c].chars[0]);
  لکو("\n");

  أرجع 1;
}

ساكن صحيح screen_sb_popline(صحيح کانن_ڳڻپ, VTermScreenCell *cells, فراغ *user اڻ_استعمال_ٿيل)
{
  صحيح کانو;

  إذا(!want_screen_scrollback)
    أرجع 0;

  // All lines of scrollback contain "ABCDE"
  لكل(کانو = 0; کانو < کانن_ڳڻپ; کانو++) {
    إذا(کانو < 5)
      cells[کانو].chars[0] = 'A' + کانو;
    وإلا
      cells[کانو].chars[0] = 0;

    cells[کانو].ويڪر = 1;
  }

  لکو("sb_popline %d\n", کانن_ڳڻپ);
  أرجع 1;
}

ساكن صحيح screen_sb_clear(فراغ *user اڻ_استعمال_ٿيل)
{
  إذا(!want_screen_scrollback)
    أرجع 1;

  لکو("sb_clear\n");
  أرجع 0;
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

صحيح مکيه(صحيح argc اڻ_استعمال_ٿيل, حرف **argv اڻ_استعمال_ٿيل)
{
  حرف سٽ[1024] = {0};
  صحيح flag;

  صحيح err;

  setvbuf(stdout, عدم, _IONBF, 0);

  طالما(fgets(سٽ, حجم سٽ, stdin)) {
    حرف *nl;
    size_t outlen;
    err = 0;

    إذا((nl = strchr(سٽ, '\n')))
      *nl = '\0';

    إذا(streq(سٽ, "INIT")) {
      إذا(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    وإلا إذا(streq(سٽ, "WANTPARSER")) {
      أكد(vt);
      vterm_parser_سيٽ_callbacks(vt, &parser_cbs, عدم);
    }

    وإلا إذا(strstartswith(سٽ, "WANTSTATE") && (سٽ[9] == '\0' || سٽ[9] == ' ')) {
      صحيح i = 9;
      صحيح sense = 1;
      أكد(vt);
      إذا(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_سيٽ_callbacks(state, &state_cbs, عدم);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_سيٽ_selection_callbacks(state, &selection_cbs, عدم, عدم, 16);
        vterm_state_سيٽ_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      طالما(سٽ[i] == ' ')
        i++;
      لكل( ; سٽ[i]; i++)
        بدل(سٽ[i]) {
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
          vterm_state_سيٽ_unrecognised_fallbacks(state, sense ? &fallbacks : عدم, عدم);
          اكسر;
        حالة 'b':
          want_state_scrollback = sense;
          اكسر;
        افتراضي:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", سٽ[i]);
        }
    }

    وإلا إذا(strstartswith(سٽ, "WANTSCREEN") && (سٽ[10] == '\0' || سٽ[10] == ' ')) {
      صحيح i = 10;
      صحيح sense = 1;
      أكد(vt);
      إذا(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_سيٽ_callbacks(screen, &screen_cbs, عدم);
      vterm_screen_callbacks_has_pushline4(screen);

      طالما(سٽ[i] == ' ')
        i++;
      لكل( ; سٽ[i]; i++)
        بدل(سٽ[i]) {
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
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", سٽ[i]);
        }
    }

    وإلا إذا(لڙي_مان_پڙهو(سٽ, "UTF8 %d", &flag)) {
      vterm_سيٽ_utf8(vt, flag);
    }

    وإلا إذا(streq(سٽ, "RESET")) {
      إذا(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      إذا(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    وإلا إذا(strstartswith(سٽ, "RESIZE ")) {
      صحيح صفون_2, کانن_ڳڻپ;
      حرف *linep = سٽ + 7;
      طالما(linep[0] == ' ')
        linep++;
      لڙي_مان_پڙهو(linep, "%d, %d", &صفون_2, &کانن_ڳڻپ);
      vterm_سيٽ_سائيز(vt, صفون_2, کانن_ڳڻپ);
    }

    وإلا إذا(strstartswith(سٽ, "PUSH ")) {
      حرف *بائيٽون = سٽ + 5;
      size_t len = inplace_hex2bytes(بائيٽون);
      أكد(len);

      size_t written = vterm_داخلا_write(vt, بائيٽون, len);
      إذا(written < len)
        fprintf(stderr, "! short write\n");
    }

    وإلا إذا(streq(سٽ, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      إذا(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.معلومات);
    }

    وإلا إذا(strstartswith(سٽ, "ENCIN ")) {
      حرف *بائيٽون = سٽ + 6;
      size_t len = inplace_hex2bytes(بائيٽون);
      أكد(len);

      uint32_t cp[1024];
      صحيح cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.معلومات,
          cp, &cpi, len, بائيٽون, &pos, len);

      إذا(cpi > 0) {
	صحيح i;
        لکو("encout ");
        لكل(i = 0; i < cpi; i++) {
          لکو(i ? ",%x" : "%x", cp[i]);
        }
        لکو("\n");
      }
    }

    وإلا إذا(strstartswith(سٽ, "INCHAR ")) {
      حرف *linep = سٽ + 7;
      بنانشان صحيح c = 0;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      لڙي_مان_پڙهو(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    وإلا إذا(strstartswith(سٽ, "INKEY ")) {
      VTermModifier mod;
      VTerm_ڪنجي ڪنجي;
      حرف *linep = سٽ + 6;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      طالما(linep[0] == ' ')
        linep++;
      ڪنجي = strp_ڪنجي(linep);

      vterm_keyboard_ڪنجي(vt, ڪنجي, mod);
    }

    وإلا إذا(strstartswith(سٽ, "PASTE ")) {
      حرف *linep = سٽ + 6;
      إذا(streq(linep, "START"))
        vterm_keyboard_شروعات_paste(vt);
      وإلا إذا(streq(linep, "END"))
        vterm_keyboard_پڇاڙي_paste(vt);
      وإلا
        اذهب abort_سٽ;
    }

    وإلا إذا(strstartswith(سٽ, "FOCUS ")) {
      أكد(state);
      حرف *linep = سٽ + 6;
      إذا(streq(linep, "IN"))
        vterm_state_focus_اندريون(state);
      وإلا إذا(streq(linep, "OUT"))
        vterm_state_focus_ٻاهريون(state);
      وإلا
        اذهب abort_سٽ;
    }

    وإلا إذا(strstartswith(سٽ, "MOUSEMOVE ")) {
      حرف *linep = سٽ + 10;
      صحيح صف, کانو, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      لڙي_مان_پڙهو(linep, "%d,%d%n", &صف, &کانو, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, صف, کانو, mod);
    }

    وإلا إذا(strstartswith(سٽ, "MOUSEBTN ")) {
      حرف *linep = سٽ + 9;
      حرف press;
      صحيح button, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      لڙي_مان_پڙهو(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    وإلا إذا(strstartswith(سٽ, "SELECTION ")) {
      حرف *linep = سٽ + 10;
      بنانشان صحيح نقاب;
      صحيح len;
      VTermStringFragment frag = { 0 };
      لڙي_مان_پڙهو(linep, "%x%n", &نقاب, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      إذا(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        طالما(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      أكد(frag.len);

      linep += frag.len * 2;
      طالما(linep[0] == ' ')
        linep++;
      إذا(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, نقاب, frag);
    }

    وإلا إذا(strstartswith(سٽ, "DAMAGEMERGE ")) {
      أكد(screen);
      حرف *linep = سٽ + 12;
      طالما(linep[0] == ' ')
        linep++;
      إذا(streq(linep, "CELL"))
        vterm_screen_سيٽ_damage_ملايو(screen, VTERM_DAMAGE_CELL);
      وإلا إذا(streq(linep, "ROW"))
        vterm_screen_سيٽ_damage_ملايو(screen, VTERM_DAMAGE_صف);
      وإلا إذا(streq(linep, "SCREEN"))
        vterm_screen_سيٽ_damage_ملايو(screen, VTERM_DAMAGE_SCREEN);
      وإلا إذا(streq(linep, "SCROLL"))
        vterm_screen_سيٽ_damage_ملايو(screen, VTERM_DAMAGE_SCROLL);
    }

    وإلا إذا(strstartswith(سٽ, "DAMAGEFLUSH")) {
      أكد(screen);
      vterm_screen_flush_damage(screen);
    }

    وإلا إذا(strstartswith(سٽ, "SETDEFAULTCOL ")) {
      أكد(screen);
      حرف *linep = سٽ + 14;
      طالما(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      إذا(linep[0]) {
        طالما(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_سيٽ_default_colors(screen, &fg, &bg);
      }
      وإلا
        vterm_screen_سيٽ_default_colors(screen, &fg, عدم);
    }

    وإلا إذا(سٽ[0] == '?') {
      إذا(streq(سٽ, "?cursor")) {
        أكد(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        إذا(pos.صف != state_pos.صف)
          لکو("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.صف, pos.کانو, state_pos.صف, state_pos.کانو);
        وإلا إذا(pos.کانو != state_pos.کانو)
          لکو("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.صف, pos.کانو, state_pos.صف, state_pos.کانو);
        وإلا
          لکو("%d,%d\n", state_pos.صف, state_pos.کانو);
      }
      وإلا إذا(strstartswith(سٽ, "?pen ")) {
        أكد(state);
        VTerm_قدر val;
        حرف *linep = سٽ + 5;
        طالما(linep[0] == ' ')
          linep++;

#وصف_ڪريو BOOLSTR(v) ((v) ? "on" : "off")

        إذا(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          إذا(val.boolean != state_pen.bold)
            لکو("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          وإلا
            لکو("%s\n", BOOLSTR(state_pen.bold));
        }
        وإلا إذا(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          إذا(val.boolean != state_pen.underline)
            لکو("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          وإلا
            لکو("%d\n", state_pen.underline);
        }
        وإلا إذا(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          إذا(val.boolean != state_pen.italic)
            لکو("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          وإلا
            لکو("%s\n", BOOLSTR(state_pen.italic));
        }
        وإلا إذا(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          إذا(val.boolean != state_pen.blink)
            لکو("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          وإلا
            لکو("%s\n", BOOLSTR(state_pen.blink));
        }
        وإلا إذا(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ابتو, &val);
          إذا(val.boolean != state_pen.ابتو)
            لکو("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.ابتو));
          وإلا
            لکو("%s\n", BOOLSTR(state_pen.ابتو));
        }
        وإلا إذا(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          إذا(val.boolean != state_pen.font)
            لکو("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          وإلا
            لکو("%d\n", state_pen.font);
        }
        وإلا إذا(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          إذا(val.boolean != state_pen.small)
            لکو("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          وإلا
            لکو("%s\n", BOOLSTR(state_pen.small));
        }
        وإلا إذا(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          إذا(val.عدد != state_pen.baseline)
            لکو("! pen baseline mismatch: state=%d, event=%d\n",
                val.عدد, state_pen.baseline);
          وإلا
            لکو("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_هيٺاهون ? "lower"
                         : "normal");
        }
        وإلا إذا(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          لکو("\n");
        }
        وإلا إذا(streq(linep, "background")) {
          print_color(&state_pen.background);
          لکو("\n");
        }
        وإلا
          لکو("?\n");
      }
      وإلا إذا(strstartswith(سٽ, "?lineinfo ")) {
        أكد(state);
        حرف *linep = سٽ + 10;
        صحيح صف;
        ثابت VTerm_سٽ_Info *info;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڙي_مان_پڙهو(linep, "%d", &صف) < 1) {
          لکو("! lineinfo unrecognised input\n");
          اذهب abort_سٽ;
        }
        info = vterm_state_get_lineinfo(state, صف);
        إذا(info->doublewidth)
          لکو("dwl ");
        إذا(info->doubleheight)
          لکو("dhl ");
        إذا(info->continuation)
          لکو("cont ");
        لکو("\n");
      }
      وإلا إذا(strstartswith(سٽ, "?screen_chars ")) {
        أكد(screen);
        حرف *linep = سٽ + 13;
        VTermRect rect;
        size_t len;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڙي_مان_پڙهو(linep, "%d,%d,%d,%d", &rect.شروعات_صف, &rect.شروعات_کانو, &rect.پڇاڙي_صف, &rect.پڇاڙي_کانو) == 4)
          ; // fine
        وإلا إذا(لڙي_مان_پڙهو(linep, "%d", &rect.شروعات_صف) == 1) {
          rect.پڇاڙي_صف = rect.شروعات_صف + 1;
          rect.شروعات_کانو = 0;
          vterm_get_سائيز(vt, عدم, &rect.پڇاڙي_کانو);
        }
        وإلا {
          لکو("! screen_chars unrecognised input\n");
          اذهب abort_سٽ;
        }
        len = vterm_screen_get_chars(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          لکو("! screen_chars error\n");
        وإلا إذا(len == 0)
          لکو("\n");
        وإلا {
          uint32_t *chars = يادگيري_ڏيو(حجم(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          لكل(i = 0; i < len; i++) {
            لکو("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          حرر(chars);
        }
      }
      وإلا إذا(strstartswith(سٽ, "?screen_text ")) {
        أكد(screen);
        حرف *linep = سٽ + 12;
        VTermRect rect;
        size_t len;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڙي_مان_پڙهو(linep, "%d,%d,%d,%d", &rect.شروعات_صف, &rect.شروعات_کانو, &rect.پڇاڙي_صف, &rect.پڇاڙي_کانو) < 4) {
          لکو("! screen_text unrecognised input\n");
          اذهب abort_سٽ;
        }
        len = vterm_screen_get_متن(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          لکو("! screen_text error\n");
        وإلا إذا(len == 0)
          لکو("\n");
        وإلا {
          // Put an overwrite guard at both ends of the buffer
          بنانشان حرف *بفر = يادگيري_ڏيو(len + 4);
          بنانشان حرف *متن = بفر + 2;
          متن[-2] = 0x55; متن[-1] = 0xAA;
          متن[len] = 0x55; متن[len+1] = 0xAA;

          vterm_screen_get_متن(screen, (حرف *)متن, len, rect);

          إذا(متن[-2] != 0x55 || متن[-1] != 0xAA)
            لکو("! screen_get_text buffer overrun left [%02x,%02x]\n", متن[-2], متن[-1]);
          وإلا إذا(متن[len] != 0x55 || متن[len+1] != 0xAA)
            لکو("! screen_get_text buffer overrun right [%02x,%02x]\n", متن[len], متن[len+1]);
          وإلا
	  {
	    size_t i;
            لكل(i = 0; i < len; i++) {
              لکو("0x%02x%s", متن[i], i < len-1 ? "," : "\n");
            }
	  }

          حرر(بفر);
        }
      }
      وإلا إذا(strstartswith(سٽ, "?screen_cell ")) {
        أكد(screen);
        حرف *linep = سٽ + 12;
	صحيح i;
        VTermPos pos;
        VTermScreenCell cell;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڙي_مان_پڙهو(linep, "%d,%d\n", &pos.صف, &pos.کانو) < 2) {
          لکو("! screen_cell unrecognised input\n");
          اذهب abort_سٽ;
        }
        إذا(!vterm_screen_get_cell(screen, pos, &cell))
          اذهب abort_سٽ;
        لکو("{");
        لكل(i = 0; i < VTERM_MAX_CHARS_في_CELL && cell.chars[i]; i++) {
          لکو("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        لکو("} width=%d attrs={", cell.ويڪر);
        إذا(cell.attrs.bold)      لکو("B");
        إذا(cell.attrs.underline) لکو("U%d", cell.attrs.underline);
        إذا(cell.attrs.italic)    لکو("I");
        إذا(cell.attrs.blink)     لکو("K");
        إذا(cell.attrs.ابتو)   لکو("R");
        إذا(cell.attrs.font)      لکو("F%d", cell.attrs.font);
        إذا(cell.attrs.small)     لکو("S");
        إذا(cell.attrs.baseline)  لکو(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        لکو("} ");
        إذا(cell.attrs.dwl)       لکو("dwl ");
        إذا(cell.attrs.dhl)       لکو("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        لکو("fg=");
        vterm_screen_convert_color_منزل_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        لکو(" bg=");
        vterm_screen_convert_color_منزل_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        لکو("\n");
      }
      وإلا إذا(strstartswith(سٽ, "?screen_eol ")) {
        أكد(screen);
        VTermPos pos;
        حرف *linep = سٽ + 12;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڙي_مان_پڙهو(linep, "%d,%d\n", &pos.صف, &pos.کانو) < 2) {
          لکو("! screen_eol unrecognised input\n");
          اذهب abort_سٽ;
        }
        لکو("%d\n", vterm_screen_is_eol(screen, pos));
      }
      وإلا إذا(strstartswith(سٽ, "?screen_attrs_extent ")) {
        أكد(screen);
        VTermPos pos;
        VTermRect rect;
        حرف *linep = سٽ + 21;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڙي_مان_پڙهو(linep, "%d,%d\n", &pos.صف, &pos.کانو) < 2) {
          لکو("! screen_attrs_extent unrecognised input\n");
          اذهب abort_سٽ;
        }
	rect.شروعات_کانو = 0;
	rect.پڇاڙي_کانو   = -1;
        إذا(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          لکو("! screen_attrs_extent failed\n");
          اذهب abort_سٽ;
        }
        لکو("%d,%d-%d,%d\n", rect.شروعات_صف, rect.شروعات_کانو, rect.پڇاڙي_صف, rect.پڇاڙي_کانو);
      }
      وإلا
        لکو("?\n");

      memset(سٽ, 0, حجم سٽ);
      استمر;
    }

    وإلا
      abort_سٽ: err = 1;

    outlen = vterm_output_get_بفر_current(vt);
    إذا(outlen > 0) {
      حرف outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      رڪن_output(outbuff, outlen, عدم);
    }

    لکو(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  أرجع 0;
}
