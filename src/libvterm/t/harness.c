#ضمن "vterm.h"
#ضمن "../src/vterm_internal.h" // We pull in some internal bits too

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#تعریف_کړه streq(a,b) (!strcmp(a,b))
#تعریف_کړه strstartswith(a,b) (!مزي_پرتله(a,b,مزي_اوږدوالی(b)))

ساكن size_t inplace_hex2bytes(حرف *s)
{
  حرف *inpos = s, *outpos = s;

  طالما(*inpos) {
    بینښه صحيح ch;
    إذا(له_مزي_ولوله(inpos, "%2x", &ch) < 1)
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
      حالة 'S': state |= VTERM_MOD_ښويدنه; اكسر;
      حالة 'C': state |= VTERM_MOD_CTRL;  اكسر;
      حالة 'A': state |= VTERM_MOD_ALT;   اكسر;
      افتراضي: أرجع state;
    }
  }

  أرجع state;
}

ساكن VTerm_کلی strp_کلی(حرف *str)
{
  ساكن بنية {
    حرف *name;
    VTerm_کلی کلی;
  } کلیګانې[] = {
    { "Up",    VTERM_کلی_پورته },
    { "Tab",   VTERM_کلی_TAB },
    { "Enter", VTERM_کلی_ENTER },
    { "KP0",   VTERM_کلی_KP_0 },
    { "F1",    VTERM_کلی_FUNCTION(1) },
    { عدم,    VTERM_کلی_NONE },
  };
  صحيح i;

  لكل(i = 0; کلیګانې[i].name; i++) {
    إذا(streq(str, کلیګانې[i].name))
      أرجع کلیګانې[i].کلی;
  }

  أرجع VTERM_کلی_NONE;
}

ساكن فراغ print_color(ثابت VTermColor *ستون)
{
  إذا (VTERM_COLOR_IS_RGB(ستون)) {
    چاپ_کړه("rgb(%d,%d,%d", ستون->red, ستون->green, ستون->blue);
  }
  وإلا إذا (VTERM_COLOR_IS_شاخصي(ستون)) {
    چاپ_کړه("idx(%d", ستون->شاخص);
  }
  وإلا {
    چاپ_کړه("invalid(%d", ستون->type);
  }
  إذا (VTERM_COLOR_IS_DEFAULT_FG(ستون)) {
    چاپ_کړه(",is_default_fg");
  }
  إذا (VTERM_COLOR_IS_DEFAULT_BG(ستون)) {
    چاپ_کړه(",is_default_bg");
  }
  چاپ_کړه(")");
}

ساكن VTermColor strpe_color(حرف **strp)
{
  uint8_t r, g, b, idx;
  صحيح len = 0;
  VTermColor ستون;

  إذا(له_مزي_ولوله(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&ستون, r, g, b);
  }
  وإلا إذا(له_مزي_ولوله(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_شاخصي(&ستون, idx);
  }
  وإلا
    vterm_color_rgb(&ستون, 127, 127, 127);

  أرجع ستون;
}

ساكن VTerm *vt;
ساكن VTermState *state;
ساكن VTermScreen *screen;

ساكن VTermEncodingInstance encoding;

ساكن فراغ غړی_output(ثابت حرف *s, size_t len, فراغ *user ناکارول_شوی)
{
  size_t i;

  چاپ_کړه("output ");
  لكل(i = 0; i < len; i++)
    چاپ_کړه("%x%s", (بینښه حرف)s[i], i < len-1 ? "," : "\n");
}

ساكن فراغ printhex(ثابت حرف *s, size_t len)
{
  طالما(len--)
    چاپ_کړه("%02x", (uint8_t)(s++)[0]);
}

ساكن صحيح parser_متن(ثابت حرف بایټونه[], size_t len, فراغ *user ناکارول_شوی)
{
  size_t i;

  چاپ_کړه("text ");
  لكل(i = 0; i < len; i++) {
    بینښه حرف b = بایټونه[i];
    إذا(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      اكسر;
    چاپ_کړه(i ? ",%x" : "%x", b);
  }
  چاپ_کړه("\n");

  أرجع i;
}

ساكن صحيح parser_control(بینښه حرف control, فراغ *user ناکارول_شوی)
{
  چاپ_کړه("control %02x\n", control);

  أرجع 1;
}

ساكن صحيح parser_escape(ثابت حرف بایټونه[], size_t len, فراغ *user ناکارول_شوی)
{
  إذا(بایټونه[0] >= 0x20 && بایټونه[0] < 0x30) {
    إذا(len < 2)
      أرجع -1;
    len = 2;
  }
  وإلا {
    len = 1;
  }

  چاپ_کړه("escape ");
  printhex(بایټونه, len);
  چاپ_کړه("\n");

  أرجع len;
}

ساكن صحيح parser_csi(ثابت حرف *leader, ثابت اوږد دلیلونه[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user ناکارول_شوی)
{
  صحيح i;
  چاپ_کړه("csi %02x", command);

  إذا(leader && leader[0]) {
    چاپ_کړه(" L=");
    لكل(i = 0; leader[i]; i++)
      چاپ_کړه("%02x", leader[i]);
  }

  لكل(i = 0; i < argcount; i++) {
    حرف sep = i ? ',' : ' ';

    إذا(دلیلونه[i] == CSI_ARG_MISSING)
      چاپ_کړه("%c*", sep);
    وإلا
      چاپ_کړه("%c%ld%s", sep, CSI_ARG(دلیلونه[i]), CSI_ARG_HAS_نور(دلیلونه[i]) ? "+" : "");
  }

  إذا(intermed && intermed[0]) {
    چاپ_کړه(" I=");
    لكل(i = 0; intermed[i]; i++)
      چاپ_کړه("%02x", intermed[i]);
  }

  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح parser_osc(صحيح command, VTermStringFragment frag, فراغ *user ناکارول_شوی)
{

  چاپ_کړه("osc ");

  إذا(frag.initial) {
    إذا(command == -1)
      چاپ_کړه("[");
    وإلا
      چاپ_کړه("[%d;", command);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کړه("]");

  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح parser_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user ناکارول_شوی)
{
  چاپ_کړه("dcs ");

  إذا(frag.initial) {
    size_t i;
    چاپ_کړه("[");
    لكل(i = 0; i < commandlen; i++)
      چاپ_کړه("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کړه("]");

  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح parser_apc(VTermStringFragment frag, فراغ *user ناکارول_شوی)
{
  چاپ_کړه("apc ");

  إذا(frag.initial)
    چاپ_کړه("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کړه("]");

  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح parser_pm(VTermStringFragment frag, فراغ *user ناکارول_شوی)
{
  چاپ_کړه("pm ");

  إذا(frag.initial)
    چاپ_کړه("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کړه("]");

  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح parser_sos(VTermStringFragment frag, فراغ *user ناکارول_شوی)
{
  چاپ_کړه("sos ");

  إذا(frag.initial)
    چاپ_کړه("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    چاپ_کړه("]");

  چاپ_کړه("\n");

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
ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos ناکارول_شوی, صحيح visible ناکارول_شوی, فراغ *user ناکارول_شوی)
{
  state_pos = pos;

  إذا(want_movecursor)
    چاپ_کړه("movecursor %d,%d\n", pos.لیکه, pos.ستون);

  أرجع 1;
}

ساكن صحيح want_premove = 0;
ساكن صحيح premove(VTermRect rect, فراغ *user ناکارول_شوی)
{
  إذا(!want_premove)
    أرجع 0;

  چاپ_کړه("premove %d..%d,%d..%d\n",
      rect.پیل_لیکه, rect.پای_ټکی_لیکه, rect.پیل_ستون, rect.پای_ټکی_ستون);

  أرجع 1;
}

ساكن صحيح want_scrollrect = 0;
ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user ناکارول_شوی)
{
  إذا(!want_scrollrect)
    أرجع 0;

  چاپ_کړه("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.پیل_لیکه, rect.پای_ټکی_لیکه, rect.پیل_ستون, rect.پای_ټکی_ستون,
      downward, rightward);

  أرجع 1;
}

ساكن صحيح want_moverect = 0;
ساكن صحيح moverect(VTermRect dest, VTermRect src, فراغ *user ناکارول_شوی)
{
  إذا(!want_moverect)
    أرجع 0;

  چاپ_کړه("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.پیل_لیکه,  src.پای_ټکی_لیکه,  src.پیل_ستون,  src.پای_ټکی_ستون,
      dest.پیل_لیکه, dest.پای_ټکی_لیکه, dest.پیل_ستون, dest.پای_ټکی_ستون);

  أرجع 1;
}

ساكن صحيح want_settermprop = 0;
ساكن صحيح settermprop(VTermProp prop, VTerm_ارزښت *val, فراغ *user ناکارول_شوی)
{
  VTerm_ارزښت_Type type;
  إذا(!want_settermprop)
    أرجع 1;

  type = vterm_get_prop_type(prop);
  بدل(type) {
  حالة VTERM_VALUETYPE_BOOL:
    چاپ_کړه("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    أرجع 1;
  حالة VTERM_VALUETYPE_INT:
    چاپ_کړه("settermprop %d %d\n", prop, val->شمېره);
    أرجع 1;
  حالة VTERM_VALUETYPE_STRING:
    چاپ_کړه("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    أرجع 1;
  حالة VTERM_VALUETYPE_COLOR:
    چاپ_کړه("settermprop %d ", prop);
    print_color(&val->color);
    چاپ_کړه("\n");
    أرجع 1;

  حالة VTERM_N_VALUETYPES:
    أرجع 0;
  }

  أرجع 0;
}

// These callbacks are for State

ساكن صحيح want_state_putglyph = 0;
ساكن صحيح state_putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user ناکارول_شوی)
{
  صحيح i;
  إذا(!want_state_putglyph)
    أرجع 1;

  چاپ_کړه("putglyph ");
  لكل(i = 0; i < VTERM_MAX_CHARS_فی_CELL && info->chars[i]; i++)
    چاپ_کړه(i ? ",%x" : "%x", info->chars[i]);
  چاپ_کړه(" %d %d,%d", info->پلنوالی, pos.لیکه, pos.ستون);
  إذا(info->protected_cell)
    چاپ_کړه(" prot");
  إذا(info->dwl)
    چاپ_کړه(" dwl");
  إذا(info->dhl)
    چاپ_کړه(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح want_state_erase = 0;
ساكن صحيح state_erase(VTermRect rect, صحيح selective, فراغ *user ناکارول_شوی)
{
  إذا(!want_state_erase)
    أرجع 1;

  چاپ_کړه("erase %d..%d,%d..%d%s\n",
      rect.پیل_لیکه, rect.پای_ټکی_لیکه, rect.پیل_ستون, rect.پای_ټکی_ستون,
      selective ? " selective" : "");

  أرجع 1;
}

ساكن بنية {
  صحيح bold;
  صحيح underline;
  صحيح italic;
  صحيح blink;
  صحيح سرچپه;
  صحيح conceal;
  صحيح strike;
  صحيح font;
  صحيح small;
  صحيح baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ساكن صحيح state_setpenattr(VTermAttr attr, VTerm_ارزښت *val, فراغ *user ناکارول_شوی)
{
  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    اكسر;
  حالة VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->شمېره;
    اكسر;
  حالة VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    اكسر;
  حالة VTERM_ATTR_سرچپه:
    state_pen.سرچپه = val->boolean;
    اكسر;
  حالة VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    اكسر;
  حالة VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    اكسر;
  حالة VTERM_ATTR_FONT:
    state_pen.font = val->شمېره;
    اكسر;
  حالة VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BASELINE:
    state_pen.baseline = val->شمېره;
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

ساكن صحيح state_setlineinfo(صحيح لیکه ناکارول_شوی, ثابت VTerm_کرښه_Info *newinfo ناکارول_شوی, ثابت VTerm_کرښه_Info *oldinfo ناکارول_شوی, فراغ *user ناکارول_شوی)
{
  أرجع 1;
}

ساكن صحيح want_state_scrollback = 0;
ساكن صحيح state_sb_clear(فراغ *user ناکارول_شوی) {
  إذا(!want_state_scrollback)
    أرجع 1;

  چاپ_کړه("sb_clear\n");
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

ساكن صحيح selection_سېټ(VTerm_Selection_ماسک ماسک, VTermStringFragment frag, فراغ *user ناکارول_شوی)
{
  چاپ_کړه("selection-set mask=%04X ", ماسک);
  إذا(frag.initial)
    چاپ_کړه("[");
  printhex(frag.str, frag.len);
  إذا(frag.final)
    چاپ_کړه("]");
  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح selection_query(VTerm_Selection_ماسک ماسک, فراغ *user ناکارول_شوی)
{
  چاپ_کړه("selection-query mask=%04X\n", ماسک);

  أرجع 1;
}

VTermSelectionCallbacks selection_cbs = {
  .سېټ_2   = selection_سېټ,
  .query = selection_query,
};

ساكن صحيح want_screen_damage = 0;
ساكن صحيح want_screen_damage_cells = 0;
ساكن صحيح screen_damage(VTermRect rect, فراغ *user ناکارول_شوی)
{
  إذا(!want_screen_damage)
    أرجع 1;

  چاپ_کړه("damage %d..%d,%d..%d",
      rect.پیل_لیکه, rect.پای_ټکی_لیکه, rect.پیل_ستون, rect.پای_ټکی_ستون);

  إذا(want_screen_damage_cells) {
    صحيح equals = FALSE;
    صحيح لیکه;
    صحيح ستون;

    لكل(لیکه = rect.پیل_لیکه; لیکه < rect.پای_ټکی_لیکه; لیکه++) {
      صحيح eol = rect.پای_ټکی_ستون;
      طالما(eol > rect.پیل_ستون) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.لیکه = لیکه;
	pos.ستون = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        إذا(cell.chars[0])
          اكسر;

        eol--;
      }

      إذا(eol == rect.پیل_ستون)
        اكسر;

      إذا(!equals)
        چاپ_کړه(" ="), equals = TRUE;

      چاپ_کړه(" %d<", لیکه);
      لكل(ستون = rect.پیل_ستون; ستون < eol; ستون++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.لیکه = لیکه;
	pos.ستون = ستون;
        vterm_screen_get_cell(screen, pos, &cell);
        چاپ_کړه(ستون == rect.پیل_ستون ? "%02X" : " %02X", cell.chars[0]);
      }
      چاپ_کړه(">");
    }
  }

  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح want_screen_scrollback = 0;
ساكن صحيح screen_sb_pushline4(صحيح ستون_شمېر, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user ناکارول_شوی)
{
  صحيح eol;
  صحيح c;

  إذا(!want_screen_scrollback)
    أرجع 1;

  eol = ستون_شمېر;
  طالما(eol && !cells[eol-1].chars[0])
    eol--;

  چاپ_کړه("sb_pushline %d%s =", ستون_شمېر, continuation ? " cont" : "");
  لكل(c = 0; c < eol; c++)
    چاپ_کړه(" %02X", cells[c].chars[0]);
  چاپ_کړه("\n");

  أرجع 1;
}

ساكن صحيح screen_sb_popline(صحيح ستون_شمېر, VTermScreenCell *cells, فراغ *user ناکارول_شوی)
{
  صحيح ستون;

  إذا(!want_screen_scrollback)
    أرجع 0;

  // All lines of scrollback contain "ABCDE"
  لكل(ستون = 0; ستون < ستون_شمېر; ستون++) {
    إذا(ستون < 5)
      cells[ستون].chars[0] = 'A' + ستون;
    وإلا
      cells[ستون].chars[0] = 0;

    cells[ستون].پلنوالی = 1;
  }

  چاپ_کړه("sb_popline %d\n", ستون_شمېر);
  أرجع 1;
}

ساكن صحيح screen_sb_clear(فراغ *user ناکارول_شوی)
{
  إذا(!want_screen_scrollback)
    أرجع 1;

  چاپ_کړه("sb_clear\n");
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

صحيح اصلي(صحيح argc ناکارول_شوی, حرف **argv ناکارول_شوی)
{
  حرف کرښه[1024] = {0};
  صحيح flag;

  صحيح err;

  setvbuf(stdout, عدم, _IONBF, 0);

  طالما(fgets(کرښه, حجم کرښه, stdin)) {
    حرف *nl;
    size_t outlen;
    err = 0;

    إذا((nl = strchr(کرښه, '\n')))
      *nl = '\0';

    إذا(streq(کرښه, "INIT")) {
      إذا(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    وإلا إذا(streq(کرښه, "WANTPARSER")) {
      أكد(vt);
      vterm_parser_سېټ_callbacks(vt, &parser_cbs, عدم);
    }

    وإلا إذا(strstartswith(کرښه, "WANTSTATE") && (کرښه[9] == '\0' || کرښه[9] == ' ')) {
      صحيح i = 9;
      صحيح sense = 1;
      أكد(vt);
      إذا(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_سېټ_callbacks(state, &state_cbs, عدم);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_سېټ_selection_callbacks(state, &selection_cbs, عدم, عدم, 16);
        vterm_state_سېټ_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      طالما(کرښه[i] == ' ')
        i++;
      لكل( ; کرښه[i]; i++)
        بدل(کرښه[i]) {
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
          vterm_state_سېټ_unrecognised_fallbacks(state, sense ? &fallbacks : عدم, عدم);
          اكسر;
        حالة 'b':
          want_state_scrollback = sense;
          اكسر;
        افتراضي:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", کرښه[i]);
        }
    }

    وإلا إذا(strstartswith(کرښه, "WANTSCREEN") && (کرښه[10] == '\0' || کرښه[10] == ' ')) {
      صحيح i = 10;
      صحيح sense = 1;
      أكد(vt);
      إذا(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_سېټ_callbacks(screen, &screen_cbs, عدم);
      vterm_screen_callbacks_has_pushline4(screen);

      طالما(کرښه[i] == ' ')
        i++;
      لكل( ; کرښه[i]; i++)
        بدل(کرښه[i]) {
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
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", کرښه[i]);
        }
    }

    وإلا إذا(له_مزي_ولوله(کرښه, "UTF8 %d", &flag)) {
      vterm_سېټ_utf8(vt, flag);
    }

    وإلا إذا(streq(کرښه, "RESET")) {
      إذا(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      إذا(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    وإلا إذا(strstartswith(کرښه, "RESIZE ")) {
      صحيح لیکې_2, ستون_شمېر;
      حرف *linep = کرښه + 7;
      طالما(linep[0] == ' ')
        linep++;
      له_مزي_ولوله(linep, "%d, %d", &لیکې_2, &ستون_شمېر);
      vterm_سېټ_اندازه(vt, لیکې_2, ستون_شمېر);
    }

    وإلا إذا(strstartswith(کرښه, "PUSH ")) {
      حرف *بایټونه = کرښه + 5;
      size_t len = inplace_hex2bytes(بایټونه);
      أكد(len);

      size_t written = vterm_ننوت_write(vt, بایټونه, len);
      إذا(written < len)
        fprintf(stderr, "! short write\n");
    }

    وإلا إذا(streq(کرښه, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      إذا(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.معلومات);
    }

    وإلا إذا(strstartswith(کرښه, "ENCIN ")) {
      حرف *بایټونه = کرښه + 6;
      size_t len = inplace_hex2bytes(بایټونه);
      أكد(len);

      uint32_t cp[1024];
      صحيح cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.معلومات,
          cp, &cpi, len, بایټونه, &pos, len);

      إذا(cpi > 0) {
	صحيح i;
        چاپ_کړه("encout ");
        لكل(i = 0; i < cpi; i++) {
          چاپ_کړه(i ? ",%x" : "%x", cp[i]);
        }
        چاپ_کړه("\n");
      }
    }

    وإلا إذا(strstartswith(کرښه, "INCHAR ")) {
      حرف *linep = کرښه + 7;
      بینښه صحيح c = 0;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      له_مزي_ولوله(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    وإلا إذا(strstartswith(کرښه, "INKEY ")) {
      VTermModifier mod;
      VTerm_کلی کلی;
      حرف *linep = کرښه + 6;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      طالما(linep[0] == ' ')
        linep++;
      کلی = strp_کلی(linep);

      vterm_keyboard_کلی(vt, کلی, mod);
    }

    وإلا إذا(strstartswith(کرښه, "PASTE ")) {
      حرف *linep = کرښه + 6;
      إذا(streq(linep, "START"))
        vterm_keyboard_پیل_paste(vt);
      وإلا إذا(streq(linep, "END"))
        vterm_keyboard_پای_ټکی_paste(vt);
      وإلا
        اذهب abort_کرښه;
    }

    وإلا إذا(strstartswith(کرښه, "FOCUS ")) {
      أكد(state);
      حرف *linep = کرښه + 6;
      إذا(streq(linep, "IN"))
        vterm_state_focus_دننه(state);
      وإلا إذا(streq(linep, "OUT"))
        vterm_state_focus_بهر(state);
      وإلا
        اذهب abort_کرښه;
    }

    وإلا إذا(strstartswith(کرښه, "MOUSEMOVE ")) {
      حرف *linep = کرښه + 10;
      صحيح لیکه, ستون, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      له_مزي_ولوله(linep, "%d,%d%n", &لیکه, &ستون, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, لیکه, ستون, mod);
    }

    وإلا إذا(strstartswith(کرښه, "MOUSEBTN ")) {
      حرف *linep = کرښه + 9;
      حرف press;
      صحيح button, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      له_مزي_ولوله(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    وإلا إذا(strstartswith(کرښه, "SELECTION ")) {
      حرف *linep = کرښه + 10;
      بینښه صحيح ماسک;
      صحيح len;
      VTermStringFragment frag = { 0 };
      له_مزي_ولوله(linep, "%x%n", &ماسک, &len);
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

    وإلا إذا(strstartswith(کرښه, "DAMAGEMERGE ")) {
      أكد(screen);
      حرف *linep = کرښه + 12;
      طالما(linep[0] == ' ')
        linep++;
      إذا(streq(linep, "CELL"))
        vterm_screen_سېټ_damage_یوځای(screen, VTERM_DAMAGE_CELL);
      وإلا إذا(streq(linep, "ROW"))
        vterm_screen_سېټ_damage_یوځای(screen, VTERM_DAMAGE_لیکه);
      وإلا إذا(streq(linep, "SCREEN"))
        vterm_screen_سېټ_damage_یوځای(screen, VTERM_DAMAGE_SCREEN);
      وإلا إذا(streq(linep, "SCROLL"))
        vterm_screen_سېټ_damage_یوځای(screen, VTERM_DAMAGE_SCROLL);
    }

    وإلا إذا(strstartswith(کرښه, "DAMAGEFLUSH")) {
      أكد(screen);
      vterm_screen_flush_damage(screen);
    }

    وإلا إذا(strstartswith(کرښه, "SETDEFAULTCOL ")) {
      أكد(screen);
      حرف *linep = کرښه + 14;
      طالما(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      إذا(linep[0]) {
        طالما(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_سېټ_default_colors(screen, &fg, &bg);
      }
      وإلا
        vterm_screen_سېټ_default_colors(screen, &fg, عدم);
    }

    وإلا إذا(کرښه[0] == '?') {
      إذا(streq(کرښه, "?cursor")) {
        أكد(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        إذا(pos.لیکه != state_pos.لیکه)
          چاپ_کړه("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.لیکه, pos.ستون, state_pos.لیکه, state_pos.ستون);
        وإلا إذا(pos.ستون != state_pos.ستون)
          چاپ_کړه("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.لیکه, pos.ستون, state_pos.لیکه, state_pos.ستون);
        وإلا
          چاپ_کړه("%d,%d\n", state_pos.لیکه, state_pos.ستون);
      }
      وإلا إذا(strstartswith(کرښه, "?pen ")) {
        أكد(state);
        VTerm_ارزښت val;
        حرف *linep = کرښه + 5;
        طالما(linep[0] == ' ')
          linep++;

#تعریف_کړه BOOLSTR(v) ((v) ? "on" : "off")

        إذا(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          إذا(val.boolean != state_pen.bold)
            چاپ_کړه("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          وإلا
            چاپ_کړه("%s\n", BOOLSTR(state_pen.bold));
        }
        وإلا إذا(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          إذا(val.boolean != state_pen.underline)
            چاپ_کړه("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          وإلا
            چاپ_کړه("%d\n", state_pen.underline);
        }
        وإلا إذا(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          إذا(val.boolean != state_pen.italic)
            چاپ_کړه("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          وإلا
            چاپ_کړه("%s\n", BOOLSTR(state_pen.italic));
        }
        وإلا إذا(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          إذا(val.boolean != state_pen.blink)
            چاپ_کړه("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          وإلا
            چاپ_کړه("%s\n", BOOLSTR(state_pen.blink));
        }
        وإلا إذا(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_سرچپه, &val);
          إذا(val.boolean != state_pen.سرچپه)
            چاپ_کړه("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.سرچپه));
          وإلا
            چاپ_کړه("%s\n", BOOLSTR(state_pen.سرچپه));
        }
        وإلا إذا(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          إذا(val.boolean != state_pen.font)
            چاپ_کړه("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          وإلا
            چاپ_کړه("%d\n", state_pen.font);
        }
        وإلا إذا(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          إذا(val.boolean != state_pen.small)
            چاپ_کړه("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          وإلا
            چاپ_کړه("%s\n", BOOLSTR(state_pen.small));
        }
        وإلا إذا(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          إذا(val.شمېره != state_pen.baseline)
            چاپ_کړه("! pen baseline mismatch: state=%d, event=%d\n",
                val.شمېره, state_pen.baseline);
          وإلا
            چاپ_کړه("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_ښکتنی ? "lower"
                         : "normal");
        }
        وإلا إذا(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          چاپ_کړه("\n");
        }
        وإلا إذا(streq(linep, "background")) {
          print_color(&state_pen.background);
          چاپ_کړه("\n");
        }
        وإلا
          چاپ_کړه("?\n");
      }
      وإلا إذا(strstartswith(کرښه, "?lineinfo ")) {
        أكد(state);
        حرف *linep = کرښه + 10;
        صحيح لیکه;
        ثابت VTerm_کرښه_Info *info;
        طالما(linep[0] == ' ')
          linep++;
        إذا(له_مزي_ولوله(linep, "%d", &لیکه) < 1) {
          چاپ_کړه("! lineinfo unrecognised input\n");
          اذهب abort_کرښه;
        }
        info = vterm_state_get_lineinfo(state, لیکه);
        إذا(info->doublewidth)
          چاپ_کړه("dwl ");
        إذا(info->doubleheight)
          چاپ_کړه("dhl ");
        إذا(info->continuation)
          چاپ_کړه("cont ");
        چاپ_کړه("\n");
      }
      وإلا إذا(strstartswith(کرښه, "?screen_chars ")) {
        أكد(screen);
        حرف *linep = کرښه + 13;
        VTermRect rect;
        size_t len;
        طالما(linep[0] == ' ')
          linep++;
        إذا(له_مزي_ولوله(linep, "%d,%d,%d,%d", &rect.پیل_لیکه, &rect.پیل_ستون, &rect.پای_ټکی_لیکه, &rect.پای_ټکی_ستون) == 4)
          ; // fine
        وإلا إذا(له_مزي_ولوله(linep, "%d", &rect.پیل_لیکه) == 1) {
          rect.پای_ټکی_لیکه = rect.پیل_لیکه + 1;
          rect.پیل_ستون = 0;
          vterm_get_اندازه(vt, عدم, &rect.پای_ټکی_ستون);
        }
        وإلا {
          چاپ_کړه("! screen_chars unrecognised input\n");
          اذهب abort_کرښه;
        }
        len = vterm_screen_get_chars(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          چاپ_کړه("! screen_chars error\n");
        وإلا إذا(len == 0)
          چاپ_کړه("\n");
        وإلا {
          uint32_t *chars = حافظه_ورکړه(حجم(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          لكل(i = 0; i < len; i++) {
            چاپ_کړه("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          حرر(chars);
        }
      }
      وإلا إذا(strstartswith(کرښه, "?screen_text ")) {
        أكد(screen);
        حرف *linep = کرښه + 12;
        VTermRect rect;
        size_t len;
        طالما(linep[0] == ' ')
          linep++;
        إذا(له_مزي_ولوله(linep, "%d,%d,%d,%d", &rect.پیل_لیکه, &rect.پیل_ستون, &rect.پای_ټکی_لیکه, &rect.پای_ټکی_ستون) < 4) {
          چاپ_کړه("! screen_text unrecognised input\n");
          اذهب abort_کرښه;
        }
        len = vterm_screen_get_متن(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          چاپ_کړه("! screen_text error\n");
        وإلا إذا(len == 0)
          چاپ_کړه("\n");
        وإلا {
          // Put an overwrite guard at both ends of the buffer
          بینښه حرف *بفر = حافظه_ورکړه(len + 4);
          بینښه حرف *متن = بفر + 2;
          متن[-2] = 0x55; متن[-1] = 0xAA;
          متن[len] = 0x55; متن[len+1] = 0xAA;

          vterm_screen_get_متن(screen, (حرف *)متن, len, rect);

          إذا(متن[-2] != 0x55 || متن[-1] != 0xAA)
            چاپ_کړه("! screen_get_text buffer overrun left [%02x,%02x]\n", متن[-2], متن[-1]);
          وإلا إذا(متن[len] != 0x55 || متن[len+1] != 0xAA)
            چاپ_کړه("! screen_get_text buffer overrun right [%02x,%02x]\n", متن[len], متن[len+1]);
          وإلا
	  {
	    size_t i;
            لكل(i = 0; i < len; i++) {
              چاپ_کړه("0x%02x%s", متن[i], i < len-1 ? "," : "\n");
            }
	  }

          حرر(بفر);
        }
      }
      وإلا إذا(strstartswith(کرښه, "?screen_cell ")) {
        أكد(screen);
        حرف *linep = کرښه + 12;
	صحيح i;
        VTermPos pos;
        VTermScreenCell cell;
        طالما(linep[0] == ' ')
          linep++;
        إذا(له_مزي_ولوله(linep, "%d,%d\n", &pos.لیکه, &pos.ستون) < 2) {
          چاپ_کړه("! screen_cell unrecognised input\n");
          اذهب abort_کرښه;
        }
        إذا(!vterm_screen_get_cell(screen, pos, &cell))
          اذهب abort_کرښه;
        چاپ_کړه("{");
        لكل(i = 0; i < VTERM_MAX_CHARS_فی_CELL && cell.chars[i]; i++) {
          چاپ_کړه("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        چاپ_کړه("} width=%d attrs={", cell.پلنوالی);
        إذا(cell.attrs.bold)      چاپ_کړه("B");
        إذا(cell.attrs.underline) چاپ_کړه("U%d", cell.attrs.underline);
        إذا(cell.attrs.italic)    چاپ_کړه("I");
        إذا(cell.attrs.blink)     چاپ_کړه("K");
        إذا(cell.attrs.سرچپه)   چاپ_کړه("R");
        إذا(cell.attrs.font)      چاپ_کړه("F%d", cell.attrs.font);
        إذا(cell.attrs.small)     چاپ_کړه("S");
        إذا(cell.attrs.baseline)  چاپ_کړه(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        چاپ_کړه("} ");
        إذا(cell.attrs.dwl)       چاپ_کړه("dwl ");
        إذا(cell.attrs.dhl)       چاپ_کړه("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        چاپ_کړه("fg=");
        vterm_screen_convert_color_منزل_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        چاپ_کړه(" bg=");
        vterm_screen_convert_color_منزل_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        چاپ_کړه("\n");
      }
      وإلا إذا(strstartswith(کرښه, "?screen_eol ")) {
        أكد(screen);
        VTermPos pos;
        حرف *linep = کرښه + 12;
        طالما(linep[0] == ' ')
          linep++;
        إذا(له_مزي_ولوله(linep, "%d,%d\n", &pos.لیکه, &pos.ستون) < 2) {
          چاپ_کړه("! screen_eol unrecognised input\n");
          اذهب abort_کرښه;
        }
        چاپ_کړه("%d\n", vterm_screen_is_eol(screen, pos));
      }
      وإلا إذا(strstartswith(کرښه, "?screen_attrs_extent ")) {
        أكد(screen);
        VTermPos pos;
        VTermRect rect;
        حرف *linep = کرښه + 21;
        طالما(linep[0] == ' ')
          linep++;
        إذا(له_مزي_ولوله(linep, "%d,%d\n", &pos.لیکه, &pos.ستون) < 2) {
          چاپ_کړه("! screen_attrs_extent unrecognised input\n");
          اذهب abort_کرښه;
        }
	rect.پیل_ستون = 0;
	rect.پای_ټکی_ستون   = -1;
        إذا(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          چاپ_کړه("! screen_attrs_extent failed\n");
          اذهب abort_کرښه;
        }
        چاپ_کړه("%d,%d-%d,%d\n", rect.پیل_لیکه, rect.پیل_ستون, rect.پای_ټکی_لیکه, rect.پای_ټکی_ستون);
      }
      وإلا
        چاپ_کړه("?\n");

      memset(کرښه, 0, حجم کرښه);
      استمر;
    }

    وإلا
      abort_کرښه: err = 1;

    outlen = vterm_output_get_بفر_current(vt);
    إذا(outlen > 0) {
      حرف outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      غړی_output(outbuff, outlen, عدم);
    }

    چاپ_کړه(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  أرجع 0;
}
