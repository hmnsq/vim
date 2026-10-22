#ضمن "vterm.h"
#ضمن "../src/vterm_internal.h" // We pull in some internal bits too

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#تعریف_کن streq(a,b) (!strcmp(a,b))
#تعریف_کن strstartswith(a,b) (!مقایسه_رشته(a,b,طول_رشته(b)))

ساكن size_t inplace_hex2bytes(حرف *s)
{
  حرف *inpos = s, *outpos = s;

  طالما(*inpos) {
    بیعلامت صحيح ch;
    إذا(از_رشته_بخوان(inpos, "%2x", &ch) < 1)
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
      حالة 'S': state |= VTERM_MOD_لغزش; اكسر;
      حالة 'C': state |= VTERM_MOD_CTRL;  اكسر;
      حالة 'A': state |= VTERM_MOD_ALT;   اكسر;
      افتراضي: أرجع state;
    }
  }

  أرجع state;
}

ساكن VTerm_کلید strp_کلید(حرف *str)
{
  ساكن بنية {
    حرف *name;
    VTerm_کلید کلید;
  } کلیدها[] = {
    { "Up",    VTERM_کلید_به_بالا },
    { "Tab",   VTERM_کلید_TAB },
    { "Enter", VTERM_کلید_ENTER },
    { "KP0",   VTERM_کلید_KP_0 },
    { "F1",    VTERM_کلید_FUNCTION(1) },
    { عدم,    VTERM_کلید_NONE },
  };
  صحيح i;

  لكل(i = 0; کلیدها[i].name; i++) {
    إذا(streq(str, کلیدها[i].name))
      أرجع کلیدها[i].کلید;
  }

  أرجع VTERM_کلید_NONE;
}

ساكن فراغ print_color(ثابت VTermColor *ستون)
{
  إذا (VTERM_COLOR_IS_RGB(ستون)) {
    چاپ_کن("rgb(%d,%d,%d", ستون->red, ستون->green, ستون->blue);
  }
  وإلا إذا (VTERM_COLOR_IS_نمایهای(ستون)) {
    چاپ_کن("idx(%d", ستون->شاخص);
  }
  وإلا {
    چاپ_کن("invalid(%d", ستون->type);
  }
  إذا (VTERM_COLOR_IS_DEFAULT_FG(ستون)) {
    چاپ_کن(",is_default_fg");
  }
  إذا (VTERM_COLOR_IS_DEFAULT_BG(ستون)) {
    چاپ_کن(",is_default_bg");
  }
  چاپ_کن(")");
}

ساكن VTermColor strpe_color(حرف **strp)
{
  uint8_t r, g, b, idx;
  صحيح len = 0;
  VTermColor ستون;

  إذا(از_رشته_بخوان(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&ستون, r, g, b);
  }
  وإلا إذا(از_رشته_بخوان(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_نمایهای(&ستون, idx);
  }
  وإلا
    vterm_color_rgb(&ستون, 127, 127, 127);

  أرجع ستون;
}

ساكن VTerm *vt;
ساكن VTermState *state;
ساكن VTermScreen *screen;

ساكن VTermEncodingInstance encoding;

ساكن فراغ جمله_output(ثابت حرف *s, size_t len, فراغ *user استفادهنشده)
{
  size_t i;

  چاپ_کن("output ");
  لكل(i = 0; i < len; i++)
    چاپ_کن("%x%s", (بیعلامت حرف)s[i], i < len-1 ? "," : "\n");
}

ساكن فراغ printhex(ثابت حرف *s, size_t len)
{
  طالما(len--)
    چاپ_کن("%02x", (uint8_t)(s++)[0]);
}

ساكن صحيح parser_متن(ثابت حرف بایتها[], size_t len, فراغ *user استفادهنشده)
{
  size_t i;

  چاپ_کن("text ");
  لكل(i = 0; i < len; i++) {
    بیعلامت حرف b = بایتها[i];
    إذا(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      اكسر;
    چاپ_کن(i ? ",%x" : "%x", b);
  }
  چاپ_کن("\n");

  أرجع i;
}

ساكن صحيح parser_control(بیعلامت حرف control, فراغ *user استفادهنشده)
{
  چاپ_کن("control %02x\n", control);

  أرجع 1;
}

ساكن صحيح parser_escape(ثابت حرف بایتها[], size_t len, فراغ *user استفادهنشده)
{
  إذا(بایتها[0] >= 0x20 && بایتها[0] < 0x30) {
    إذا(len < 2)
      أرجع -1;
    len = 2;
  }
  وإلا {
    len = 1;
  }

  چاپ_کن("escape ");
  printhex(بایتها, len);
  چاپ_کن("\n");

  أرجع len;
}

ساكن صحيح parser_csi(ثابت حرف *leader, ثابت دراز آرگومانها[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user استفادهنشده)
{
  صحيح i;
  چاپ_کن("csi %02x", command);

  إذا(leader && leader[0]) {
    چاپ_کن(" L=");
    لكل(i = 0; leader[i]; i++)
      چاپ_کن("%02x", leader[i]);
  }

  لكل(i = 0; i < argcount; i++) {
    حرف sep = i ? ',' : ' ';

    إذا(آرگومانها[i] == CSI_ARG_MISSING)
      چاپ_کن("%c*", sep);
    وإلا
      چاپ_کن("%c%ld%s", sep, CSI_ARG(آرگومانها[i]), CSI_ARG_HAS_بیشتر(آرگومانها[i]) ? "+" : "");
  }

  إذا(intermed && intermed[0]) {
    چاپ_کن(" I=");
    لكل(i = 0; intermed[i]; i++)
      چاپ_کن("%02x", intermed[i]);
  }

  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح parser_osc(صحيح command, VTermStringFragment frag, فراغ *user استفادهنشده)
{

  چاپ_کن("osc ");

  إذا(frag.initial) {
    إذا(command == -1)
      چاپ_کن("[");
    وإلا
      چاپ_کن("[%d;", command);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کن("]");

  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح parser_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user استفادهنشده)
{
  چاپ_کن("dcs ");

  إذا(frag.initial) {
    size_t i;
    چاپ_کن("[");
    لكل(i = 0; i < commandlen; i++)
      چاپ_کن("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کن("]");

  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح parser_apc(VTermStringFragment frag, فراغ *user استفادهنشده)
{
  چاپ_کن("apc ");

  إذا(frag.initial)
    چاپ_کن("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کن("]");

  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح parser_pm(VTermStringFragment frag, فراغ *user استفادهنشده)
{
  چاپ_کن("pm ");

  إذا(frag.initial)
    چاپ_کن("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کن("]");

  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح parser_sos(VTermStringFragment frag, فراغ *user استفادهنشده)
{
  چاپ_کن("sos ");

  إذا(frag.initial)
    چاپ_کن("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کن("]");

  چاپ_کن("\n");

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
ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos استفادهنشده, صحيح visible استفادهنشده, فراغ *user استفادهنشده)
{
  state_pos = pos;

  إذا(want_movecursor)
    چاپ_کن("movecursor %d,%d\n", pos.ردیف, pos.ستون);

  أرجع 1;
}

ساكن صحيح want_premove = 0;
ساكن صحيح premove(VTermRect rect, فراغ *user استفادهنشده)
{
  إذا(!want_premove)
    أرجع 0;

  چاپ_کن("premove %d..%d,%d..%d\n",
      rect.شروع_ردیف, rect.انتها_ردیف, rect.شروع_ستون, rect.انتها_ستون);

  أرجع 1;
}

ساكن صحيح want_scrollrect = 0;
ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user استفادهنشده)
{
  إذا(!want_scrollrect)
    أرجع 0;

  چاپ_کن("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.شروع_ردیف, rect.انتها_ردیف, rect.شروع_ستون, rect.انتها_ستون,
      downward, rightward);

  أرجع 1;
}

ساكن صحيح want_moverect = 0;
ساكن صحيح moverect(VTermRect dest, VTermRect src, فراغ *user استفادهنشده)
{
  إذا(!want_moverect)
    أرجع 0;

  چاپ_کن("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.شروع_ردیف,  src.انتها_ردیف,  src.شروع_ستون,  src.انتها_ستون,
      dest.شروع_ردیف, dest.انتها_ردیف, dest.شروع_ستون, dest.انتها_ستون);

  أرجع 1;
}

ساكن صحيح want_settermprop = 0;
ساكن صحيح settermprop(VTermProp prop, VTerm_مقدار *val, فراغ *user استفادهنشده)
{
  VTerm_مقدار_Type type;
  إذا(!want_settermprop)
    أرجع 1;

  type = vterm_get_prop_type(prop);
  بدل(type) {
  حالة VTERM_VALUETYPE_BOOL:
    چاپ_کن("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    أرجع 1;
  حالة VTERM_VALUETYPE_INT:
    چاپ_کن("settermprop %d %d\n", prop, val->عدد);
    أرجع 1;
  حالة VTERM_VALUETYPE_STRING:
    چاپ_کن("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    أرجع 1;
  حالة VTERM_VALUETYPE_COLOR:
    چاپ_کن("settermprop %d ", prop);
    print_color(&val->color);
    چاپ_کن("\n");
    أرجع 1;

  حالة VTERM_N_VALUETYPES:
    أرجع 0;
  }

  أرجع 0;
}

// These callbacks are for State

ساكن صحيح want_state_putglyph = 0;
ساكن صحيح state_putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user استفادهنشده)
{
  صحيح i;
  إذا(!want_state_putglyph)
    أرجع 1;

  چاپ_کن("putglyph ");
  لكل(i = 0; i < VTERM_MAX_CHARS_در_هر_CELL && info->chars[i]; i++)
    چاپ_کن(i ? ",%x" : "%x", info->chars[i]);
  چاپ_کن(" %d %d,%d", info->عرض, pos.ردیف, pos.ستون);
  إذا(info->protected_cell)
    چاپ_کن(" prot");
  إذا(info->dwl)
    چاپ_کن(" dwl");
  إذا(info->dhl)
    چاپ_کن(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح want_state_erase = 0;
ساكن صحيح state_erase(VTermRect rect, صحيح selective, فراغ *user استفادهنشده)
{
  إذا(!want_state_erase)
    أرجع 1;

  چاپ_کن("erase %d..%d,%d..%d%s\n",
      rect.شروع_ردیف, rect.انتها_ردیف, rect.شروع_ستون, rect.انتها_ستون,
      selective ? " selective" : "");

  أرجع 1;
}

ساكن بنية {
  صحيح bold;
  صحيح underline;
  صحيح italic;
  صحيح blink;
  صحيح معکوس;
  صحيح conceal;
  صحيح strike;
  صحيح font;
  صحيح small;
  صحيح baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ساكن صحيح state_setpenattr(VTermAttr attr, VTerm_مقدار *val, فراغ *user استفادهنشده)
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
  حالة VTERM_ATTR_معکوس:
    state_pen.معکوس = val->boolean;
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

ساكن صحيح state_setlineinfo(صحيح ردیف استفادهنشده, ثابت VTerm_سطر_Info *newinfo استفادهنشده, ثابت VTerm_سطر_Info *oldinfo استفادهنشده, فراغ *user استفادهنشده)
{
  أرجع 1;
}

ساكن صحيح want_state_scrollback = 0;
ساكن صحيح state_sb_clear(فراغ *user استفادهنشده) {
  إذا(!want_state_scrollback)
    أرجع 1;

  چاپ_کن("sb_clear\n");
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

ساكن صحيح selection_مجموعه(VTerm_Selection_ماسک ماسک, VTermStringFragment frag, فراغ *user استفادهنشده)
{
  چاپ_کن("selection-set mask=%04X ", ماسک);
  إذا(frag.initial)
    چاپ_کن("[");
  printhex(frag.str, frag.len);
  إذا(frag.final)
    چاپ_کن("]");
  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح selection_query(VTerm_Selection_ماسک ماسک, فراغ *user استفادهنشده)
{
  چاپ_کن("selection-query mask=%04X\n", ماسک);

  أرجع 1;
}

VTermSelectionCallbacks selection_cbs = {
  .مجموعه_2   = selection_مجموعه,
  .query = selection_query,
};

ساكن صحيح want_screen_damage = 0;
ساكن صحيح want_screen_damage_cells = 0;
ساكن صحيح screen_damage(VTermRect rect, فراغ *user استفادهنشده)
{
  إذا(!want_screen_damage)
    أرجع 1;

  چاپ_کن("damage %d..%d,%d..%d",
      rect.شروع_ردیف, rect.انتها_ردیف, rect.شروع_ستون, rect.انتها_ستون);

  إذا(want_screen_damage_cells) {
    صحيح equals = FALSE;
    صحيح ردیف;
    صحيح ستون;

    لكل(ردیف = rect.شروع_ردیف; ردیف < rect.انتها_ردیف; ردیف++) {
      صحيح eol = rect.انتها_ستون;
      طالما(eol > rect.شروع_ستون) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.ردیف = ردیف;
	pos.ستون = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        إذا(cell.chars[0])
          اكسر;

        eol--;
      }

      إذا(eol == rect.شروع_ستون)
        اكسر;

      إذا(!equals)
        چاپ_کن(" ="), equals = TRUE;

      چاپ_کن(" %d<", ردیف);
      لكل(ستون = rect.شروع_ستون; ستون < eol; ستون++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.ردیف = ردیف;
	pos.ستون = ستون;
        vterm_screen_get_cell(screen, pos, &cell);
        چاپ_کن(ستون == rect.شروع_ستون ? "%02X" : " %02X", cell.chars[0]);
      }
      چاپ_کن(">");
    }
  }

  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح want_screen_scrollback = 0;
ساكن صحيح screen_sb_pushline4(صحيح تعداد_ستون, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user استفادهنشده)
{
  صحيح eol;
  صحيح c;

  إذا(!want_screen_scrollback)
    أرجع 1;

  eol = تعداد_ستون;
  طالما(eol && !cells[eol-1].chars[0])
    eol--;

  چاپ_کن("sb_pushline %d%s =", تعداد_ستون, continuation ? " cont" : "");
  لكل(c = 0; c < eol; c++)
    چاپ_کن(" %02X", cells[c].chars[0]);
  چاپ_کن("\n");

  أرجع 1;
}

ساكن صحيح screen_sb_popline(صحيح تعداد_ستون, VTermScreenCell *cells, فراغ *user استفادهنشده)
{
  صحيح ستون;

  إذا(!want_screen_scrollback)
    أرجع 0;

  // All lines of scrollback contain "ABCDE"
  لكل(ستون = 0; ستون < تعداد_ستون; ستون++) {
    إذا(ستون < 5)
      cells[ستون].chars[0] = 'A' + ستون;
    وإلا
      cells[ستون].chars[0] = 0;

    cells[ستون].عرض = 1;
  }

  چاپ_کن("sb_popline %d\n", تعداد_ستون);
  أرجع 1;
}

ساكن صحيح screen_sb_clear(فراغ *user استفادهنشده)
{
  إذا(!want_screen_scrollback)
    أرجع 1;

  چاپ_کن("sb_clear\n");
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

صحيح اصلی(صحيح argc استفادهنشده, حرف **argv استفادهنشده)
{
  حرف سطر[1024] = {0};
  صحيح flag;

  صحيح err;

  setvbuf(stdout, عدم, _IONBF, 0);

  طالما(fgets(سطر, حجم سطر, stdin)) {
    حرف *nl;
    size_t outlen;
    err = 0;

    إذا((nl = strchr(سطر, '\n')))
      *nl = '\0';

    إذا(streq(سطر, "INIT")) {
      إذا(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    وإلا إذا(streq(سطر, "WANTPARSER")) {
      أكد(vt);
      vterm_parser_مجموعه_callbacks(vt, &parser_cbs, عدم);
    }

    وإلا إذا(strstartswith(سطر, "WANTSTATE") && (سطر[9] == '\0' || سطر[9] == ' ')) {
      صحيح i = 9;
      صحيح sense = 1;
      أكد(vt);
      إذا(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_مجموعه_callbacks(state, &state_cbs, عدم);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_مجموعه_selection_callbacks(state, &selection_cbs, عدم, عدم, 16);
        vterm_state_مجموعه_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      طالما(سطر[i] == ' ')
        i++;
      لكل( ; سطر[i]; i++)
        بدل(سطر[i]) {
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
          vterm_state_مجموعه_unrecognised_fallbacks(state, sense ? &fallbacks : عدم, عدم);
          اكسر;
        حالة 'b':
          want_state_scrollback = sense;
          اكسر;
        افتراضي:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", سطر[i]);
        }
    }

    وإلا إذا(strstartswith(سطر, "WANTSCREEN") && (سطر[10] == '\0' || سطر[10] == ' ')) {
      صحيح i = 10;
      صحيح sense = 1;
      أكد(vt);
      إذا(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_مجموعه_callbacks(screen, &screen_cbs, عدم);
      vterm_screen_callbacks_has_pushline4(screen);

      طالما(سطر[i] == ' ')
        i++;
      لكل( ; سطر[i]; i++)
        بدل(سطر[i]) {
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
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", سطر[i]);
        }
    }

    وإلا إذا(از_رشته_بخوان(سطر, "UTF8 %d", &flag)) {
      vterm_مجموعه_utf8(vt, flag);
    }

    وإلا إذا(streq(سطر, "RESET")) {
      إذا(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      إذا(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    وإلا إذا(strstartswith(سطر, "RESIZE ")) {
      صحيح ردیفها_2, تعداد_ستون;
      حرف *linep = سطر + 7;
      طالما(linep[0] == ' ')
        linep++;
      از_رشته_بخوان(linep, "%d, %d", &ردیفها_2, &تعداد_ستون);
      vterm_مجموعه_اندازه(vt, ردیفها_2, تعداد_ستون);
    }

    وإلا إذا(strstartswith(سطر, "PUSH ")) {
      حرف *بایتها = سطر + 5;
      size_t len = inplace_hex2bytes(بایتها);
      أكد(len);

      size_t written = vterm_درون_داد_write(vt, بایتها, len);
      إذا(written < len)
        fprintf(stderr, "! short write\n");
    }

    وإلا إذا(streq(سطر, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      إذا(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.داده);
    }

    وإلا إذا(strstartswith(سطر, "ENCIN ")) {
      حرف *بایتها = سطر + 6;
      size_t len = inplace_hex2bytes(بایتها);
      أكد(len);

      uint32_t cp[1024];
      صحيح cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.داده,
          cp, &cpi, len, بایتها, &pos, len);

      إذا(cpi > 0) {
	صحيح i;
        چاپ_کن("encout ");
        لكل(i = 0; i < cpi; i++) {
          چاپ_کن(i ? ",%x" : "%x", cp[i]);
        }
        چاپ_کن("\n");
      }
    }

    وإلا إذا(strstartswith(سطر, "INCHAR ")) {
      حرف *linep = سطر + 7;
      بیعلامت صحيح c = 0;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      از_رشته_بخوان(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    وإلا إذا(strstartswith(سطر, "INKEY ")) {
      VTermModifier mod;
      VTerm_کلید کلید;
      حرف *linep = سطر + 6;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      طالما(linep[0] == ' ')
        linep++;
      کلید = strp_کلید(linep);

      vterm_keyboard_کلید(vt, کلید, mod);
    }

    وإلا إذا(strstartswith(سطر, "PASTE ")) {
      حرف *linep = سطر + 6;
      إذا(streq(linep, "START"))
        vterm_keyboard_شروع_paste(vt);
      وإلا إذا(streq(linep, "END"))
        vterm_keyboard_انتها_paste(vt);
      وإلا
        اذهب abort_سطر;
    }

    وإلا إذا(strstartswith(سطر, "FOCUS ")) {
      أكد(state);
      حرف *linep = سطر + 6;
      إذا(streq(linep, "IN"))
        vterm_state_focus_درون(state);
      وإلا إذا(streq(linep, "OUT"))
        vterm_state_focus_بیرون(state);
      وإلا
        اذهب abort_سطر;
    }

    وإلا إذا(strstartswith(سطر, "MOUSEMOVE ")) {
      حرف *linep = سطر + 10;
      صحيح ردیف, ستون, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      از_رشته_بخوان(linep, "%d,%d%n", &ردیف, &ستون, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, ردیف, ستون, mod);
    }

    وإلا إذا(strstartswith(سطر, "MOUSEBTN ")) {
      حرف *linep = سطر + 9;
      حرف press;
      صحيح button, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      از_رشته_بخوان(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    وإلا إذا(strstartswith(سطر, "SELECTION ")) {
      حرف *linep = سطر + 10;
      بیعلامت صحيح ماسک;
      صحيح len;
      VTermStringFragment frag = { 0 };
      از_رشته_بخوان(linep, "%x%n", &ماسک, &len);
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
      vterm_state_send_selection(state, ماسک, frag);
    }

    وإلا إذا(strstartswith(سطر, "DAMAGEMERGE ")) {
      أكد(screen);
      حرف *linep = سطر + 12;
      طالما(linep[0] == ' ')
        linep++;
      إذا(streq(linep, "CELL"))
        vterm_screen_مجموعه_damage_ادغام(screen, VTERM_DAMAGE_CELL);
      وإلا إذا(streq(linep, "ROW"))
        vterm_screen_مجموعه_damage_ادغام(screen, VTERM_DAMAGE_ردیف);
      وإلا إذا(streq(linep, "SCREEN"))
        vterm_screen_مجموعه_damage_ادغام(screen, VTERM_DAMAGE_SCREEN);
      وإلا إذا(streq(linep, "SCROLL"))
        vterm_screen_مجموعه_damage_ادغام(screen, VTERM_DAMAGE_SCROLL);
    }

    وإلا إذا(strstartswith(سطر, "DAMAGEFLUSH")) {
      أكد(screen);
      vterm_screen_flush_damage(screen);
    }

    وإلا إذا(strstartswith(سطر, "SETDEFAULTCOL ")) {
      أكد(screen);
      حرف *linep = سطر + 14;
      طالما(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      إذا(linep[0]) {
        طالما(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_مجموعه_default_colors(screen, &fg, &bg);
      }
      وإلا
        vterm_screen_مجموعه_default_colors(screen, &fg, عدم);
    }

    وإلا إذا(سطر[0] == '?') {
      إذا(streq(سطر, "?cursor")) {
        أكد(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        إذا(pos.ردیف != state_pos.ردیف)
          چاپ_کن("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.ردیف, pos.ستون, state_pos.ردیف, state_pos.ستون);
        وإلا إذا(pos.ستون != state_pos.ستون)
          چاپ_کن("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.ردیف, pos.ستون, state_pos.ردیف, state_pos.ستون);
        وإلا
          چاپ_کن("%d,%d\n", state_pos.ردیف, state_pos.ستون);
      }
      وإلا إذا(strstartswith(سطر, "?pen ")) {
        أكد(state);
        VTerm_مقدار val;
        حرف *linep = سطر + 5;
        طالما(linep[0] == ' ')
          linep++;

#تعریف_کن BOOLSTR(v) ((v) ? "on" : "off")

        إذا(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          إذا(val.boolean != state_pen.bold)
            چاپ_کن("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          وإلا
            چاپ_کن("%s\n", BOOLSTR(state_pen.bold));
        }
        وإلا إذا(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          إذا(val.boolean != state_pen.underline)
            چاپ_کن("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          وإلا
            چاپ_کن("%d\n", state_pen.underline);
        }
        وإلا إذا(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          إذا(val.boolean != state_pen.italic)
            چاپ_کن("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          وإلا
            چاپ_کن("%s\n", BOOLSTR(state_pen.italic));
        }
        وإلا إذا(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          إذا(val.boolean != state_pen.blink)
            چاپ_کن("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          وإلا
            چاپ_کن("%s\n", BOOLSTR(state_pen.blink));
        }
        وإلا إذا(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_معکوس, &val);
          إذا(val.boolean != state_pen.معکوس)
            چاپ_کن("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.معکوس));
          وإلا
            چاپ_کن("%s\n", BOOLSTR(state_pen.معکوس));
        }
        وإلا إذا(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          إذا(val.boolean != state_pen.font)
            چاپ_کن("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          وإلا
            چاپ_کن("%d\n", state_pen.font);
        }
        وإلا إذا(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          إذا(val.boolean != state_pen.small)
            چاپ_کن("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          وإلا
            چاپ_کن("%s\n", BOOLSTR(state_pen.small));
        }
        وإلا إذا(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          إذا(val.عدد != state_pen.baseline)
            چاپ_کن("! pen baseline mismatch: state=%d, event=%d\n",
                val.عدد, state_pen.baseline);
          وإلا
            چاپ_کن("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_پایینی ? "lower"
                         : "normal");
        }
        وإلا إذا(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          چاپ_کن("\n");
        }
        وإلا إذا(streq(linep, "background")) {
          print_color(&state_pen.background);
          چاپ_کن("\n");
        }
        وإلا
          چاپ_کن("?\n");
      }
      وإلا إذا(strstartswith(سطر, "?lineinfo ")) {
        أكد(state);
        حرف *linep = سطر + 10;
        صحيح ردیف;
        ثابت VTerm_سطر_Info *info;
        طالما(linep[0] == ' ')
          linep++;
        إذا(از_رشته_بخوان(linep, "%d", &ردیف) < 1) {
          چاپ_کن("! lineinfo unrecognised input\n");
          اذهب abort_سطر;
        }
        info = vterm_state_get_lineinfo(state, ردیف);
        إذا(info->doublewidth)
          چاپ_کن("dwl ");
        إذا(info->doubleheight)
          چاپ_کن("dhl ");
        إذا(info->continuation)
          چاپ_کن("cont ");
        چاپ_کن("\n");
      }
      وإلا إذا(strstartswith(سطر, "?screen_chars ")) {
        أكد(screen);
        حرف *linep = سطر + 13;
        VTermRect rect;
        size_t len;
        طالما(linep[0] == ' ')
          linep++;
        إذا(از_رشته_بخوان(linep, "%d,%d,%d,%d", &rect.شروع_ردیف, &rect.شروع_ستون, &rect.انتها_ردیف, &rect.انتها_ستون) == 4)
          ; // fine
        وإلا إذا(از_رشته_بخوان(linep, "%d", &rect.شروع_ردیف) == 1) {
          rect.انتها_ردیف = rect.شروع_ردیف + 1;
          rect.شروع_ستون = 0;
          vterm_get_اندازه(vt, عدم, &rect.انتها_ستون);
        }
        وإلا {
          چاپ_کن("! screen_chars unrecognised input\n");
          اذهب abort_سطر;
        }
        len = vterm_screen_get_chars(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          چاپ_کن("! screen_chars error\n");
        وإلا إذا(len == 0)
          چاپ_کن("\n");
        وإلا {
          uint32_t *chars = حافظه_بده(حجم(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          لكل(i = 0; i < len; i++) {
            چاپ_کن("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          حرر(chars);
        }
      }
      وإلا إذا(strstartswith(سطر, "?screen_text ")) {
        أكد(screen);
        حرف *linep = سطر + 12;
        VTermRect rect;
        size_t len;
        طالما(linep[0] == ' ')
          linep++;
        إذا(از_رشته_بخوان(linep, "%d,%d,%d,%d", &rect.شروع_ردیف, &rect.شروع_ستون, &rect.انتها_ردیف, &rect.انتها_ستون) < 4) {
          چاپ_کن("! screen_text unrecognised input\n");
          اذهب abort_سطر;
        }
        len = vterm_screen_get_متن(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          چاپ_کن("! screen_text error\n");
        وإلا إذا(len == 0)
          چاپ_کن("\n");
        وإلا {
          // Put an overwrite guard at both ends of the buffer
          بیعلامت حرف *میانگیر = حافظه_بده(len + 4);
          بیعلامت حرف *متن = میانگیر + 2;
          متن[-2] = 0x55; متن[-1] = 0xAA;
          متن[len] = 0x55; متن[len+1] = 0xAA;

          vterm_screen_get_متن(screen, (حرف *)متن, len, rect);

          إذا(متن[-2] != 0x55 || متن[-1] != 0xAA)
            چاپ_کن("! screen_get_text buffer overrun left [%02x,%02x]\n", متن[-2], متن[-1]);
          وإلا إذا(متن[len] != 0x55 || متن[len+1] != 0xAA)
            چاپ_کن("! screen_get_text buffer overrun right [%02x,%02x]\n", متن[len], متن[len+1]);
          وإلا
	  {
	    size_t i;
            لكل(i = 0; i < len; i++) {
              چاپ_کن("0x%02x%s", متن[i], i < len-1 ? "," : "\n");
            }
	  }

          حرر(میانگیر);
        }
      }
      وإلا إذا(strstartswith(سطر, "?screen_cell ")) {
        أكد(screen);
        حرف *linep = سطر + 12;
	صحيح i;
        VTermPos pos;
        VTermScreenCell cell;
        طالما(linep[0] == ' ')
          linep++;
        إذا(از_رشته_بخوان(linep, "%d,%d\n", &pos.ردیف, &pos.ستون) < 2) {
          چاپ_کن("! screen_cell unrecognised input\n");
          اذهب abort_سطر;
        }
        إذا(!vterm_screen_get_cell(screen, pos, &cell))
          اذهب abort_سطر;
        چاپ_کن("{");
        لكل(i = 0; i < VTERM_MAX_CHARS_در_هر_CELL && cell.chars[i]; i++) {
          چاپ_کن("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        چاپ_کن("} width=%d attrs={", cell.عرض);
        إذا(cell.attrs.bold)      چاپ_کن("B");
        إذا(cell.attrs.underline) چاپ_کن("U%d", cell.attrs.underline);
        إذا(cell.attrs.italic)    چاپ_کن("I");
        إذا(cell.attrs.blink)     چاپ_کن("K");
        إذا(cell.attrs.معکوس)   چاپ_کن("R");
        إذا(cell.attrs.font)      چاپ_کن("F%d", cell.attrs.font);
        إذا(cell.attrs.small)     چاپ_کن("S");
        إذا(cell.attrs.baseline)  چاپ_کن(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        چاپ_کن("} ");
        إذا(cell.attrs.dwl)       چاپ_کن("dwl ");
        إذا(cell.attrs.dhl)       چاپ_کن("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        چاپ_کن("fg=");
        vterm_screen_convert_color_مقصد_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        چاپ_کن(" bg=");
        vterm_screen_convert_color_مقصد_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        چاپ_کن("\n");
      }
      وإلا إذا(strstartswith(سطر, "?screen_eol ")) {
        أكد(screen);
        VTermPos pos;
        حرف *linep = سطر + 12;
        طالما(linep[0] == ' ')
          linep++;
        إذا(از_رشته_بخوان(linep, "%d,%d\n", &pos.ردیف, &pos.ستون) < 2) {
          چاپ_کن("! screen_eol unrecognised input\n");
          اذهب abort_سطر;
        }
        چاپ_کن("%d\n", vterm_screen_is_eol(screen, pos));
      }
      وإلا إذا(strstartswith(سطر, "?screen_attrs_extent ")) {
        أكد(screen);
        VTermPos pos;
        VTermRect rect;
        حرف *linep = سطر + 21;
        طالما(linep[0] == ' ')
          linep++;
        إذا(از_رشته_بخوان(linep, "%d,%d\n", &pos.ردیف, &pos.ستون) < 2) {
          چاپ_کن("! screen_attrs_extent unrecognised input\n");
          اذهب abort_سطر;
        }
	rect.شروع_ستون = 0;
	rect.انتها_ستون   = -1;
        إذا(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          چاپ_کن("! screen_attrs_extent failed\n");
          اذهب abort_سطر;
        }
        چاپ_کن("%d,%d-%d,%d\n", rect.شروع_ردیف, rect.شروع_ستون, rect.انتها_ردیف, rect.انتها_ستون);
      }
      وإلا
        چاپ_کن("?\n");

      memset(سطر, 0, حجم سطر);
      استمر;
    }

    وإلا
      abort_سطر: err = 1;

    outlen = vterm_output_get_میانگیر_current(vt);
    إذا(outlen > 0) {
      حرف outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      جمله_output(outbuff, outlen, عدم);
    }

    چاپ_کن(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  أرجع 0;
}
