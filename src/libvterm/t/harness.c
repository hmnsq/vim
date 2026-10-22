#ضمن "vterm.h"
#ضمن "../src/vterm_internal.h" // We pull in some internal bits too

#ضمن <assert.h>
#ضمن <stdio.h>
#ضمن <string.h>

#تعریف_کرو streq(a,b) (!strcmp(a,b))
#تعریف_کرو strstartswith(a,b) (!لڑی_مقابلہ(a,b,لڑی_لمبائی(b)))

ساكن size_t inplace_hex2bytes(حرف *s)
{
  حرف *inpos = s, *outpos = s;

  طالما(*inpos) {
    بنانشانی صحيح ch;
    إذا(لڑی_توں_پڑھو(inpos, "%2x", &ch) < 1)
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
      حالة 'S': state |= VTERM_MOD_کھسکاؤ; اكسر;
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
  } کلیدیں[] = {
    { "Up",    VTERM_کلید_اتے },
    { "Tab",   VTERM_کلید_TAB },
    { "Enter", VTERM_کلید_ENTER },
    { "KP0",   VTERM_کلید_KP_0 },
    { "F1",    VTERM_کلید_FUNCTION(1) },
    { عدم,    VTERM_کلید_NONE },
  };
  صحيح i;

  لكل(i = 0; کلیدیں[i].name; i++) {
    إذا(streq(str, کلیدیں[i].name))
      أرجع کلیدیں[i].کلید;
  }

  أرجع VTERM_کلید_NONE;
}

ساكن فراغ print_color(ثابت VTermColor *خانہ)
{
  إذا (VTERM_COLOR_IS_RGB(خانہ)) {
    لکھو("rgb(%d,%d,%d", خانہ->red, خانہ->green, خانہ->blue);
  }
  وإلا إذا (VTERM_COLOR_IS_اشاریہ_دار(خانہ)) {
    لکھو("idx(%d", خانہ->اشاریہ);
  }
  وإلا {
    لکھو("invalid(%d", خانہ->type);
  }
  إذا (VTERM_COLOR_IS_DEFAULT_FG(خانہ)) {
    لکھو(",is_default_fg");
  }
  إذا (VTERM_COLOR_IS_DEFAULT_BG(خانہ)) {
    لکھو(",is_default_bg");
  }
  لکھو(")");
}

ساكن VTermColor strpe_color(حرف **strp)
{
  uint8_t r, g, b, idx;
  صحيح len = 0;
  VTermColor خانہ;

  إذا(لڑی_توں_پڑھو(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&خانہ, r, g, b);
  }
  وإلا إذا(لڑی_توں_پڑھو(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_اشاریہ_دار(&خانہ, idx);
  }
  وإلا
    vterm_color_rgb(&خانہ, 127, 127, 127);

  أرجع خانہ;
}

ساكن VTerm *vt;
ساكن VTermState *state;
ساكن VTermScreen *screen;

ساكن VTermEncodingInstance encoding;

ساكن فراغ رکن_output(ثابت حرف *s, size_t len, فراغ *user بن_ورتیا)
{
  size_t i;

  لکھو("output ");
  لكل(i = 0; i < len; i++)
    لکھو("%x%s", (بنانشانی حرف)s[i], i < len-1 ? "," : "\n");
}

ساكن فراغ printhex(ثابت حرف *s, size_t len)
{
  طالما(len--)
    لکھو("%02x", (uint8_t)(s++)[0]);
}

ساكن صحيح parser_متن(ثابت حرف بائٹاں[], size_t len, فراغ *user بن_ورتیا)
{
  size_t i;

  لکھو("text ");
  لكل(i = 0; i < len; i++) {
    بنانشانی حرف b = بائٹاں[i];
    إذا(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      اكسر;
    لکھو(i ? ",%x" : "%x", b);
  }
  لکھو("\n");

  أرجع i;
}

ساكن صحيح parser_control(بنانشانی حرف control, فراغ *user بن_ورتیا)
{
  لکھو("control %02x\n", control);

  أرجع 1;
}

ساكن صحيح parser_escape(ثابت حرف بائٹاں[], size_t len, فراغ *user بن_ورتیا)
{
  إذا(بائٹاں[0] >= 0x20 && بائٹاں[0] < 0x30) {
    إذا(len < 2)
      أرجع -1;
    len = 2;
  }
  وإلا {
    len = 1;
  }

  لکھو("escape ");
  printhex(بائٹاں, len);
  لکھو("\n");

  أرجع len;
}

ساكن صحيح parser_csi(ثابت حرف *leader, ثابت لما دلیلاں[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user بن_ورتیا)
{
  صحيح i;
  لکھو("csi %02x", command);

  إذا(leader && leader[0]) {
    لکھو(" L=");
    لكل(i = 0; leader[i]; i++)
      لکھو("%02x", leader[i]);
  }

  لكل(i = 0; i < argcount; i++) {
    حرف sep = i ? ',' : ' ';

    إذا(دلیلاں[i] == CSI_ARG_MISSING)
      لکھو("%c*", sep);
    وإلا
      لکھو("%c%ld%s", sep, CSI_ARG(دلیلاں[i]), CSI_ARG_HAS_ہور(دلیلاں[i]) ? "+" : "");
  }

  إذا(intermed && intermed[0]) {
    لکھو(" I=");
    لكل(i = 0; intermed[i]; i++)
      لکھو("%02x", intermed[i]);
  }

  لکھو("\n");

  أرجع 1;
}

ساكن صحيح parser_osc(صحيح command, VTermStringFragment frag, فراغ *user بن_ورتیا)
{

  لکھو("osc ");

  إذا(frag.initial) {
    إذا(command == -1)
      لکھو("[");
    وإلا
      لکھو("[%d;", command);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکھو("]");

  لکھو("\n");

  أرجع 1;
}

ساكن صحيح parser_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user بن_ورتیا)
{
  لکھو("dcs ");

  إذا(frag.initial) {
    size_t i;
    لکھو("[");
    لكل(i = 0; i < commandlen; i++)
      لکھو("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکھو("]");

  لکھو("\n");

  أرجع 1;
}

ساكن صحيح parser_apc(VTermStringFragment frag, فراغ *user بن_ورتیا)
{
  لکھو("apc ");

  إذا(frag.initial)
    لکھو("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکھو("]");

  لکھو("\n");

  أرجع 1;
}

ساكن صحيح parser_pm(VTermStringFragment frag, فراغ *user بن_ورتیا)
{
  لکھو("pm ");

  إذا(frag.initial)
    لکھو("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکھو("]");

  لکھو("\n");

  أرجع 1;
}

ساكن صحيح parser_sos(VTermStringFragment frag, فراغ *user بن_ورتیا)
{
  لکھو("sos ");

  إذا(frag.initial)
    لکھو("[");

  printhex(frag.str, frag.len);

  إذا(frag.final)
    لکھو("]");

  لکھو("\n");

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
ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos بن_ورتیا, صحيح visible بن_ورتیا, فراغ *user بن_ورتیا)
{
  state_pos = pos;

  إذا(want_movecursor)
    لکھو("movecursor %d,%d\n", pos.صف, pos.خانہ);

  أرجع 1;
}

ساكن صحيح want_premove = 0;
ساكن صحيح premove(VTermRect rect, فراغ *user بن_ورتیا)
{
  إذا(!want_premove)
    أرجع 0;

  لکھو("premove %d..%d,%d..%d\n",
      rect.شروع_صف, rect.سرا_صف, rect.شروع_خانہ, rect.سرا_خانہ);

  أرجع 1;
}

ساكن صحيح want_scrollrect = 0;
ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user بن_ورتیا)
{
  إذا(!want_scrollrect)
    أرجع 0;

  لکھو("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.شروع_صف, rect.سرا_صف, rect.شروع_خانہ, rect.سرا_خانہ,
      downward, rightward);

  أرجع 1;
}

ساكن صحيح want_moverect = 0;
ساكن صحيح moverect(VTermRect dest, VTermRect src, فراغ *user بن_ورتیا)
{
  إذا(!want_moverect)
    أرجع 0;

  لکھو("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.شروع_صف,  src.سرا_صف,  src.شروع_خانہ,  src.سرا_خانہ,
      dest.شروع_صف, dest.سرا_صف, dest.شروع_خانہ, dest.سرا_خانہ);

  أرجع 1;
}

ساكن صحيح want_settermprop = 0;
ساكن صحيح settermprop(VTermProp prop, VTerm_قدر *val, فراغ *user بن_ورتیا)
{
  VTerm_قدر_Type type;
  إذا(!want_settermprop)
    أرجع 1;

  type = vterm_get_prop_type(prop);
  بدل(type) {
  حالة VTERM_VALUETYPE_BOOL:
    لکھو("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    أرجع 1;
  حالة VTERM_VALUETYPE_INT:
    لکھو("settermprop %d %d\n", prop, val->نمبر);
    أرجع 1;
  حالة VTERM_VALUETYPE_STRING:
    لکھو("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    أرجع 1;
  حالة VTERM_VALUETYPE_COLOR:
    لکھو("settermprop %d ", prop);
    print_color(&val->color);
    لکھو("\n");
    أرجع 1;

  حالة VTERM_N_VALUETYPES:
    أرجع 0;
  }

  أرجع 0;
}

// These callbacks are for State

ساكن صحيح want_state_putglyph = 0;
ساكن صحيح state_putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user بن_ورتیا)
{
  صحيح i;
  إذا(!want_state_putglyph)
    أرجع 1;

  لکھو("putglyph ");
  لكل(i = 0; i < VTERM_MAX_CHARS_فی_CELL && info->chars[i]; i++)
    لکھو(i ? ",%x" : "%x", info->chars[i]);
  لکھو(" %d %d,%d", info->چوڑائی, pos.صف, pos.خانہ);
  إذا(info->protected_cell)
    لکھو(" prot");
  إذا(info->dwl)
    لکھو(" dwl");
  إذا(info->dhl)
    لکھو(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  لکھو("\n");

  أرجع 1;
}

ساكن صحيح want_state_erase = 0;
ساكن صحيح state_erase(VTermRect rect, صحيح selective, فراغ *user بن_ورتیا)
{
  إذا(!want_state_erase)
    أرجع 1;

  لکھو("erase %d..%d,%d..%d%s\n",
      rect.شروع_صف, rect.سرا_صف, rect.شروع_خانہ, rect.سرا_خانہ,
      selective ? " selective" : "");

  أرجع 1;
}

ساكن بنية {
  صحيح bold;
  صحيح underline;
  صحيح italic;
  صحيح blink;
  صحيح الٹ;
  صحيح conceal;
  صحيح strike;
  صحيح font;
  صحيح small;
  صحيح baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
ساكن صحيح state_setpenattr(VTermAttr attr, VTerm_قدر *val, فراغ *user بن_ورتیا)
{
  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    اكسر;
  حالة VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->نمبر;
    اكسر;
  حالة VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    اكسر;
  حالة VTERM_ATTR_الٹ:
    state_pen.الٹ = val->boolean;
    اكسر;
  حالة VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    اكسر;
  حالة VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    اكسر;
  حالة VTERM_ATTR_FONT:
    state_pen.font = val->نمبر;
    اكسر;
  حالة VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    اكسر;
  حالة VTERM_ATTR_BASELINE:
    state_pen.baseline = val->نمبر;
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

ساكن صحيح state_setlineinfo(صحيح صف بن_ورتیا, ثابت VTerm_سطر_Info *newinfo بن_ورتیا, ثابت VTerm_سطر_Info *oldinfo بن_ورتیا, فراغ *user بن_ورتیا)
{
  أرجع 1;
}

ساكن صحيح want_state_scrollback = 0;
ساكن صحيح state_sb_clear(فراغ *user بن_ورتیا) {
  إذا(!want_state_scrollback)
    أرجع 1;

  لکھو("sb_clear\n");
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

ساكن صحيح selection_سیٹ(VTerm_Selection_نقاب نقاب, VTermStringFragment frag, فراغ *user بن_ورتیا)
{
  لکھو("selection-set mask=%04X ", نقاب);
  إذا(frag.initial)
    لکھو("[");
  printhex(frag.str, frag.len);
  إذا(frag.final)
    لکھو("]");
  لکھو("\n");

  أرجع 1;
}

ساكن صحيح selection_query(VTerm_Selection_نقاب نقاب, فراغ *user بن_ورتیا)
{
  لکھو("selection-query mask=%04X\n", نقاب);

  أرجع 1;
}

VTermSelectionCallbacks selection_cbs = {
  .سیٹ_2   = selection_سیٹ,
  .query = selection_query,
};

ساكن صحيح want_screen_damage = 0;
ساكن صحيح want_screen_damage_cells = 0;
ساكن صحيح screen_damage(VTermRect rect, فراغ *user بن_ورتیا)
{
  إذا(!want_screen_damage)
    أرجع 1;

  لکھو("damage %d..%d,%d..%d",
      rect.شروع_صف, rect.سرا_صف, rect.شروع_خانہ, rect.سرا_خانہ);

  إذا(want_screen_damage_cells) {
    صحيح equals = FALSE;
    صحيح صف;
    صحيح خانہ;

    لكل(صف = rect.شروع_صف; صف < rect.سرا_صف; صف++) {
      صحيح eol = rect.سرا_خانہ;
      طالما(eol > rect.شروع_خانہ) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.صف = صف;
	pos.خانہ = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        إذا(cell.chars[0])
          اكسر;

        eol--;
      }

      إذا(eol == rect.شروع_خانہ)
        اكسر;

      إذا(!equals)
        لکھو(" ="), equals = TRUE;

      لکھو(" %d<", صف);
      لكل(خانہ = rect.شروع_خانہ; خانہ < eol; خانہ++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.صف = صف;
	pos.خانہ = خانہ;
        vterm_screen_get_cell(screen, pos, &cell);
        لکھو(خانہ == rect.شروع_خانہ ? "%02X" : " %02X", cell.chars[0]);
      }
      لکھو(">");
    }
  }

  لکھو("\n");

  أرجع 1;
}

ساكن صحيح want_screen_scrollback = 0;
ساكن صحيح screen_sb_pushline4(صحيح خانے_گنتی, ثابت VTermScreenCell *cells, صحيح continuation, فراغ *user بن_ورتیا)
{
  صحيح eol;
  صحيح c;

  إذا(!want_screen_scrollback)
    أرجع 1;

  eol = خانے_گنتی;
  طالما(eol && !cells[eol-1].chars[0])
    eol--;

  لکھو("sb_pushline %d%s =", خانے_گنتی, continuation ? " cont" : "");
  لكل(c = 0; c < eol; c++)
    لکھو(" %02X", cells[c].chars[0]);
  لکھو("\n");

  أرجع 1;
}

ساكن صحيح screen_sb_popline(صحيح خانے_گنتی, VTermScreenCell *cells, فراغ *user بن_ورتیا)
{
  صحيح خانہ;

  إذا(!want_screen_scrollback)
    أرجع 0;

  // All lines of scrollback contain "ABCDE"
  لكل(خانہ = 0; خانہ < خانے_گنتی; خانہ++) {
    إذا(خانہ < 5)
      cells[خانہ].chars[0] = 'A' + خانہ;
    وإلا
      cells[خانہ].chars[0] = 0;

    cells[خانہ].چوڑائی = 1;
  }

  لکھو("sb_popline %d\n", خانے_گنتی);
  أرجع 1;
}

ساكن صحيح screen_sb_clear(فراغ *user بن_ورتیا)
{
  إذا(!want_screen_scrollback)
    أرجع 1;

  لکھو("sb_clear\n");
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

صحيح مکھ(صحيح argc بن_ورتیا, حرف **argv بن_ورتیا)
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
      vterm_parser_سیٹ_callbacks(vt, &parser_cbs, عدم);
    }

    وإلا إذا(strstartswith(سطر, "WANTSTATE") && (سطر[9] == '\0' || سطر[9] == ' ')) {
      صحيح i = 9;
      صحيح sense = 1;
      أكد(vt);
      إذا(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_سیٹ_callbacks(state, &state_cbs, عدم);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_سیٹ_selection_callbacks(state, &selection_cbs, عدم, عدم, 16);
        vterm_state_سیٹ_bold_highbright(state, 1);
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
          vterm_state_سیٹ_unrecognised_fallbacks(state, sense ? &fallbacks : عدم, عدم);
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
      vterm_screen_سیٹ_callbacks(screen, &screen_cbs, عدم);
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

    وإلا إذا(لڑی_توں_پڑھو(سطر, "UTF8 %d", &flag)) {
      vterm_سیٹ_utf8(vt, flag);
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
      صحيح صفاں_2, خانے_گنتی;
      حرف *linep = سطر + 7;
      طالما(linep[0] == ' ')
        linep++;
      لڑی_توں_پڑھو(linep, "%d, %d", &صفاں_2, &خانے_گنتی);
      vterm_سیٹ_ناپ(vt, صفاں_2, خانے_گنتی);
    }

    وإلا إذا(strstartswith(سطر, "PUSH ")) {
      حرف *بائٹاں = سطر + 5;
      size_t len = inplace_hex2bytes(بائٹاں);
      أكد(len);

      size_t written = vterm_داخلہ_write(vt, بائٹاں, len);
      إذا(written < len)
        fprintf(stderr, "! short write\n");
    }

    وإلا إذا(streq(سطر, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      إذا(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.انکڑے);
    }

    وإلا إذا(strstartswith(سطر, "ENCIN ")) {
      حرف *بائٹاں = سطر + 6;
      size_t len = inplace_hex2bytes(بائٹاں);
      أكد(len);

      uint32_t cp[1024];
      صحيح cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.انکڑے,
          cp, &cpi, len, بائٹاں, &pos, len);

      إذا(cpi > 0) {
	صحيح i;
        لکھو("encout ");
        لكل(i = 0; i < cpi; i++) {
          لکھو(i ? ",%x" : "%x", cp[i]);
        }
        لکھو("\n");
      }
    }

    وإلا إذا(strstartswith(سطر, "INCHAR ")) {
      حرف *linep = سطر + 7;
      بنانشانی صحيح c = 0;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      لڑی_توں_پڑھو(linep, " %x", &c);

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
        vterm_keyboard_سرا_paste(vt);
      وإلا
        اذهب abort_سطر;
    }

    وإلا إذا(strstartswith(سطر, "FOCUS ")) {
      أكد(state);
      حرف *linep = سطر + 6;
      إذا(streq(linep, "IN"))
        vterm_state_focus_وچ(state);
      وإلا إذا(streq(linep, "OUT"))
        vterm_state_focus_باہرلا(state);
      وإلا
        اذهب abort_سطر;
    }

    وإلا إذا(strstartswith(سطر, "MOUSEMOVE ")) {
      حرف *linep = سطر + 10;
      صحيح صف, خانہ, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      لڑی_توں_پڑھو(linep, "%d,%d%n", &صف, &خانہ, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, صف, خانہ, mod);
    }

    وإلا إذا(strstartswith(سطر, "MOUSEBTN ")) {
      حرف *linep = سطر + 9;
      حرف press;
      صحيح button, len;
      VTermModifier mod;
      طالما(linep[0] == ' ')
        linep++;
      لڑی_توں_پڑھو(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      طالما(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    وإلا إذا(strstartswith(سطر, "SELECTION ")) {
      حرف *linep = سطر + 10;
      بنانشانی صحيح نقاب;
      صحيح len;
      VTermStringFragment frag = { 0 };
      لڑی_توں_پڑھو(linep, "%x%n", &نقاب, &len);
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

    وإلا إذا(strstartswith(سطر, "DAMAGEMERGE ")) {
      أكد(screen);
      حرف *linep = سطر + 12;
      طالما(linep[0] == ' ')
        linep++;
      إذا(streq(linep, "CELL"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_CELL);
      وإلا إذا(streq(linep, "ROW"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_صف);
      وإلا إذا(streq(linep, "SCREEN"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_SCREEN);
      وإلا إذا(streq(linep, "SCROLL"))
        vterm_screen_سیٹ_damage_ملاؤ(screen, VTERM_DAMAGE_SCROLL);
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

        vterm_screen_سیٹ_default_colors(screen, &fg, &bg);
      }
      وإلا
        vterm_screen_سیٹ_default_colors(screen, &fg, عدم);
    }

    وإلا إذا(سطر[0] == '?') {
      إذا(streq(سطر, "?cursor")) {
        أكد(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        إذا(pos.صف != state_pos.صف)
          لکھو("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.صف, pos.خانہ, state_pos.صف, state_pos.خانہ);
        وإلا إذا(pos.خانہ != state_pos.خانہ)
          لکھو("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.صف, pos.خانہ, state_pos.صف, state_pos.خانہ);
        وإلا
          لکھو("%d,%d\n", state_pos.صف, state_pos.خانہ);
      }
      وإلا إذا(strstartswith(سطر, "?pen ")) {
        أكد(state);
        VTerm_قدر val;
        حرف *linep = سطر + 5;
        طالما(linep[0] == ' ')
          linep++;

#تعریف_کرو BOOLSTR(v) ((v) ? "on" : "off")

        إذا(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          إذا(val.boolean != state_pen.bold)
            لکھو("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          وإلا
            لکھو("%s\n", BOOLSTR(state_pen.bold));
        }
        وإلا إذا(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          إذا(val.boolean != state_pen.underline)
            لکھو("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          وإلا
            لکھو("%d\n", state_pen.underline);
        }
        وإلا إذا(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          إذا(val.boolean != state_pen.italic)
            لکھو("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          وإلا
            لکھو("%s\n", BOOLSTR(state_pen.italic));
        }
        وإلا إذا(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          إذا(val.boolean != state_pen.blink)
            لکھو("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          وإلا
            لکھو("%s\n", BOOLSTR(state_pen.blink));
        }
        وإلا إذا(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_الٹ, &val);
          إذا(val.boolean != state_pen.الٹ)
            لکھو("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.الٹ));
          وإلا
            لکھو("%s\n", BOOLSTR(state_pen.الٹ));
        }
        وإلا إذا(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          إذا(val.boolean != state_pen.font)
            لکھو("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          وإلا
            لکھو("%d\n", state_pen.font);
        }
        وإلا إذا(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          إذا(val.boolean != state_pen.small)
            لکھو("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          وإلا
            لکھو("%s\n", BOOLSTR(state_pen.small));
        }
        وإلا إذا(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          إذا(val.نمبر != state_pen.baseline)
            لکھو("! pen baseline mismatch: state=%d, event=%d\n",
                val.نمبر, state_pen.baseline);
          وإلا
            لکھو("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_ہیٹھلا ? "lower"
                         : "normal");
        }
        وإلا إذا(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          لکھو("\n");
        }
        وإلا إذا(streq(linep, "background")) {
          print_color(&state_pen.background);
          لکھو("\n");
        }
        وإلا
          لکھو("?\n");
      }
      وإلا إذا(strstartswith(سطر, "?lineinfo ")) {
        أكد(state);
        حرف *linep = سطر + 10;
        صحيح صف;
        ثابت VTerm_سطر_Info *info;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڑی_توں_پڑھو(linep, "%d", &صف) < 1) {
          لکھو("! lineinfo unrecognised input\n");
          اذهب abort_سطر;
        }
        info = vterm_state_get_lineinfo(state, صف);
        إذا(info->doublewidth)
          لکھو("dwl ");
        إذا(info->doubleheight)
          لکھو("dhl ");
        إذا(info->continuation)
          لکھو("cont ");
        لکھو("\n");
      }
      وإلا إذا(strstartswith(سطر, "?screen_chars ")) {
        أكد(screen);
        حرف *linep = سطر + 13;
        VTermRect rect;
        size_t len;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڑی_توں_پڑھو(linep, "%d,%d,%d,%d", &rect.شروع_صف, &rect.شروع_خانہ, &rect.سرا_صف, &rect.سرا_خانہ) == 4)
          ; // fine
        وإلا إذا(لڑی_توں_پڑھو(linep, "%d", &rect.شروع_صف) == 1) {
          rect.سرا_صف = rect.شروع_صف + 1;
          rect.شروع_خانہ = 0;
          vterm_get_ناپ(vt, عدم, &rect.سرا_خانہ);
        }
        وإلا {
          لکھو("! screen_chars unrecognised input\n");
          اذهب abort_سطر;
        }
        len = vterm_screen_get_chars(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          لکھو("! screen_chars error\n");
        وإلا إذا(len == 0)
          لکھو("\n");
        وإلا {
          uint32_t *chars = حافظہ_دیو(حجم(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          لكل(i = 0; i < len; i++) {
            لکھو("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
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
        إذا(لڑی_توں_پڑھو(linep, "%d,%d,%d,%d", &rect.شروع_صف, &rect.شروع_خانہ, &rect.سرا_صف, &rect.سرا_خانہ) < 4) {
          لکھو("! screen_text unrecognised input\n");
          اذهب abort_سطر;
        }
        len = vterm_screen_get_متن(screen, عدم, 0, rect);
        إذا(len == (size_t)-1)
          لکھو("! screen_text error\n");
        وإلا إذا(len == 0)
          لکھو("\n");
        وإلا {
          // Put an overwrite guard at both ends of the buffer
          بنانشانی حرف *بفر = حافظہ_دیو(len + 4);
          بنانشانی حرف *متن = بفر + 2;
          متن[-2] = 0x55; متن[-1] = 0xAA;
          متن[len] = 0x55; متن[len+1] = 0xAA;

          vterm_screen_get_متن(screen, (حرف *)متن, len, rect);

          إذا(متن[-2] != 0x55 || متن[-1] != 0xAA)
            لکھو("! screen_get_text buffer overrun left [%02x,%02x]\n", متن[-2], متن[-1]);
          وإلا إذا(متن[len] != 0x55 || متن[len+1] != 0xAA)
            لکھو("! screen_get_text buffer overrun right [%02x,%02x]\n", متن[len], متن[len+1]);
          وإلا
	  {
	    size_t i;
            لكل(i = 0; i < len; i++) {
              لکھو("0x%02x%s", متن[i], i < len-1 ? "," : "\n");
            }
	  }

          حرر(بفر);
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
        إذا(لڑی_توں_پڑھو(linep, "%d,%d\n", &pos.صف, &pos.خانہ) < 2) {
          لکھو("! screen_cell unrecognised input\n");
          اذهب abort_سطر;
        }
        إذا(!vterm_screen_get_cell(screen, pos, &cell))
          اذهب abort_سطر;
        لکھو("{");
        لكل(i = 0; i < VTERM_MAX_CHARS_فی_CELL && cell.chars[i]; i++) {
          لکھو("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        لکھو("} width=%d attrs={", cell.چوڑائی);
        إذا(cell.attrs.bold)      لکھو("B");
        إذا(cell.attrs.underline) لکھو("U%d", cell.attrs.underline);
        إذا(cell.attrs.italic)    لکھو("I");
        إذا(cell.attrs.blink)     لکھو("K");
        إذا(cell.attrs.الٹ)   لکھو("R");
        إذا(cell.attrs.font)      لکھو("F%d", cell.attrs.font);
        إذا(cell.attrs.small)     لکھو("S");
        إذا(cell.attrs.baseline)  لکھو(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        لکھو("} ");
        إذا(cell.attrs.dwl)       لکھو("dwl ");
        إذا(cell.attrs.dhl)       لکھو("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        لکھو("fg=");
        vterm_screen_convert_color_منزل_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        لکھو(" bg=");
        vterm_screen_convert_color_منزل_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        لکھو("\n");
      }
      وإلا إذا(strstartswith(سطر, "?screen_eol ")) {
        أكد(screen);
        VTermPos pos;
        حرف *linep = سطر + 12;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڑی_توں_پڑھو(linep, "%d,%d\n", &pos.صف, &pos.خانہ) < 2) {
          لکھو("! screen_eol unrecognised input\n");
          اذهب abort_سطر;
        }
        لکھو("%d\n", vterm_screen_is_eol(screen, pos));
      }
      وإلا إذا(strstartswith(سطر, "?screen_attrs_extent ")) {
        أكد(screen);
        VTermPos pos;
        VTermRect rect;
        حرف *linep = سطر + 21;
        طالما(linep[0] == ' ')
          linep++;
        إذا(لڑی_توں_پڑھو(linep, "%d,%d\n", &pos.صف, &pos.خانہ) < 2) {
          لکھو("! screen_attrs_extent unrecognised input\n");
          اذهب abort_سطر;
        }
	rect.شروع_خانہ = 0;
	rect.سرا_خانہ   = -1;
        إذا(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          لکھو("! screen_attrs_extent failed\n");
          اذهب abort_سطر;
        }
        لکھو("%d,%d-%d,%d\n", rect.شروع_صف, rect.شروع_خانہ, rect.سرا_صف, rect.سرا_خانہ);
      }
      وإلا
        لکھو("?\n");

      memset(سطر, 0, حجم سطر);
      استمر;
    }

    وإلا
      abort_سطر: err = 1;

    outlen = vterm_output_get_بفر_current(vt);
    إذا(outlen > 0) {
      حرف outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      رکن_output(outbuff, outlen, عدم);
    }

    لکھو(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  أرجع 0;
}
