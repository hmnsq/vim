#అన్తర్భావ "vterm.h"
#అన్తర్భావ "../src/vterm_internal.h" // We pull in some internal bits too

#అన్తర్భావ <assert.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#నిర్వచించు streq(a,b) (!strcmp(a,b))
#నిర్వచించు strstartswith(a,b) (!అక్షరశ్రేణి_పోలిక(a,b,అక్షరశ్రేణి_పొడవు(b)))

స్థిర size_t inplace_hex2bytes(అక్షరం *s)
{
  అక్షరం *inpos = s, *outpos = s;

  అయితే(*inpos) {
    గుర్తులేని పూర్ణాంకం ch;
    ఒకవేళ(అక్షరశ్రేణి_చదువు(inpos, "%2x", &ch) < 1)
      ఆపు;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  తిరిగి outpos - s;
}

స్థిర VTermModifier strpe_modifiers(అక్షరం **strp)
{
  VTermModifier state = 0;

  అయితే((*strp)[0]) {
    ఎంపిక(((*strp)++)[0]) {
      స్థితి 'S': state |= VTERM_MOD_జరుపు; ఆపు;
      స్థితి 'C': state |= VTERM_MOD_CTRL;  ఆపు;
      స్థితి 'A': state |= VTERM_MOD_ALT;   ఆపు;
      అప్రమేయ: తిరిగి state;
    }
  }

  తిరిగి state;
}

స్థిర VTerm_కీలకం strp_కీలకం(అక్షరం *str)
{
  స్థిర నిర్మాణం {
    అక్షరం *name;
    VTerm_కీలకం కీలకం;
  } కీలకాలు[] = {
    { "Up",    VTERM_కీలకం_పైకి },
    { "Tab",   VTERM_కీలకం_TAB },
    { "Enter", VTERM_కీలకం_ENTER },
    { "KP0",   VTERM_కీలకం_KP_0 },
    { "F1",    VTERM_కీలకం_FUNCTION(1) },
    { ఖాళీ,    VTERM_కీలకం_NONE },
  };
  పూర్ణాంకం i;

  కొరకు(i = 0; కీలకాలు[i].name; i++) {
    ఒకవేళ(streq(str, కీలకాలు[i].name))
      తిరిగి కీలకాలు[i].కీలకం;
  }

  తిరిగి VTERM_కీలకం_NONE;
}

స్థిర శూన్య print_color(స్థిరరాశి VTermColor *నిలువు)
{
  ఒకవేళ (VTERM_COLOR_IS_RGB(నిలువు)) {
    రాయి("rgb(%d,%d,%d", నిలువు->red, నిలువు->green, నిలువు->blue);
  }
  లేకపోతే ఒకవేళ (VTERM_COLOR_IS_సూచీకృత(నిలువు)) {
    రాయి("idx(%d", నిలువు->సూచీ);
  }
  లేకపోతే {
    రాయి("invalid(%d", నిలువు->type);
  }
  ఒకవేళ (VTERM_COLOR_IS_DEFAULT_FG(నిలువు)) {
    రాయి(",is_default_fg");
  }
  ఒకవేళ (VTERM_COLOR_IS_DEFAULT_BG(నిలువు)) {
    రాయి(",is_default_bg");
  }
  రాయి(")");
}

స్థిర VTermColor strpe_color(అక్షరం **strp)
{
  uint8_t r, g, b, idx;
  పూర్ణాంకం len = 0;
  VTermColor నిలువు;

  ఒకవేళ(అక్షరశ్రేణి_చదువు(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&నిలువు, r, g, b);
  }
  లేకపోతే ఒకవేళ(అక్షరశ్రేణి_చదువు(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_సూచీకృత(&నిలువు, idx);
  }
  లేకపోతే
    vterm_color_rgb(&నిలువు, 127, 127, 127);

  తిరిగి నిలువు;
}

స్థిర VTerm *vt;
స్థిర VTermState *state;
స్థిర VTermScreen *screen;

స్థిర VTermEncodingInstance encoding;

స్థిర శూన్య పదం_output(స్థిరరాశి అక్షరం *s, size_t len, శూన్య *user వాడని)
{
  size_t i;

  రాయి("output ");
  కొరకు(i = 0; i < len; i++)
    రాయి("%x%s", (గుర్తులేని అక్షరం)s[i], i < len-1 ? "," : "\n");
}

స్థిర శూన్య printhex(స్థిరరాశి అక్షరం *s, size_t len)
{
  అయితే(len--)
    రాయి("%02x", (uint8_t)(s++)[0]);
}

స్థిర పూర్ణాంకం parser_పాఠ్యం(స్థిరరాశి అక్షరం బైట్లు[], size_t len, శూన్య *user వాడని)
{
  size_t i;

  రాయి("text ");
  కొరకు(i = 0; i < len; i++) {
    గుర్తులేని అక్షరం b = బైట్లు[i];
    ఒకవేళ(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      ఆపు;
    రాయి(i ? ",%x" : "%x", b);
  }
  రాయి("\n");

  తిరిగి i;
}

స్థిర పూర్ణాంకం parser_control(గుర్తులేని అక్షరం control, శూన్య *user వాడని)
{
  రాయి("control %02x\n", control);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం parser_escape(స్థిరరాశి అక్షరం బైట్లు[], size_t len, శూన్య *user వాడని)
{
  ఒకవేళ(బైట్లు[0] >= 0x20 && బైట్లు[0] < 0x30) {
    ఒకవేళ(len < 2)
      తిరిగి -1;
    len = 2;
  }
  లేకపోతే {
    len = 1;
  }

  రాయి("escape ");
  printhex(బైట్లు, len);
  రాయి("\n");

  తిరిగి len;
}

స్థిర పూర్ణాంకం parser_csi(స్థిరరాశి అక్షరం *leader, స్థిరరాశి దీర్ఘ వాదనలు[], పూర్ణాంకం argcount, స్థిరరాశి అక్షరం *intermed, అక్షరం command, శూన్య *user వాడని)
{
  పూర్ణాంకం i;
  రాయి("csi %02x", command);

  ఒకవేళ(leader && leader[0]) {
    రాయి(" L=");
    కొరకు(i = 0; leader[i]; i++)
      రాయి("%02x", leader[i]);
  }

  కొరకు(i = 0; i < argcount; i++) {
    అక్షరం sep = i ? ',' : ' ';

    ఒకవేళ(వాదనలు[i] == CSI_ARG_MISSING)
      రాయి("%c*", sep);
    లేకపోతే
      రాయి("%c%ld%s", sep, CSI_ARG(వాదనలు[i]), CSI_ARG_HAS_మరిన్ని(వాదనలు[i]) ? "+" : "");
  }

  ఒకవేళ(intermed && intermed[0]) {
    రాయి(" I=");
    కొరకు(i = 0; intermed[i]; i++)
      రాయి("%02x", intermed[i]);
  }

  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం parser_osc(పూర్ణాంకం command, VTermStringFragment frag, శూన్య *user వాడని)
{

  రాయి("osc ");

  ఒకవేళ(frag.initial) {
    ఒకవేళ(command == -1)
      రాయి("[");
    లేకపోతే
      రాయి("[%d;", command);
  }

  printhex(frag.str, frag.len);

  ఒకవేళ(frag.final)
    రాయి("]");

  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం parser_dcs(స్థిరరాశి అక్షరం *command, size_t commandlen, VTermStringFragment frag, శూన్య *user వాడని)
{
  రాయి("dcs ");

  ఒకవేళ(frag.initial) {
    size_t i;
    రాయి("[");
    కొరకు(i = 0; i < commandlen; i++)
      రాయి("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  ఒకవేళ(frag.final)
    రాయి("]");

  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం parser_apc(VTermStringFragment frag, శూన్య *user వాడని)
{
  రాయి("apc ");

  ఒకవేళ(frag.initial)
    రాయి("[");

  printhex(frag.str, frag.len);

  ఒకవేళ(frag.final)
    రాయి("]");

  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం parser_pm(VTermStringFragment frag, శూన్య *user వాడని)
{
  రాయి("pm ");

  ఒకవేళ(frag.initial)
    రాయి("[");

  printhex(frag.str, frag.len);

  ఒకవేళ(frag.final)
    రాయి("]");

  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం parser_sos(VTermStringFragment frag, శూన్య *user వాడని)
{
  రాయి("sos ");

  ఒకవేళ(frag.initial)
    రాయి("[");

  printhex(frag.str, frag.len);

  ఒకవేళ(frag.final)
    రాయి("]");

  రాయి("\n");

  తిరిగి 1;
}

స్థిర VTermParserCallbacks parser_cbs = {
  parser_పాఠ్యం, //పాఠ్యం
  parser_control, // control
  parser_escape, // escape
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // apc
  parser_pm, // pm
  parser_sos, // sos
  ఖాళీ // resize
};

స్థిర VTermStateFallbacks fallbacks = {
  parser_control, // control
  parser_csi, // csi
  parser_osc, // osc
  parser_dcs, // dcs
  parser_apc, // dcs
  parser_pm, // pm
  parser_sos // sos
};

/* These callbacks are shared by State and Screen */

స్థిర పూర్ణాంకం want_movecursor = 0;
స్థిర VTermPos state_pos;
స్థిర పూర్ణాంకం movecursor(VTermPos pos, VTermPos oldpos వాడని, పూర్ణాంకం visible వాడని, శూన్య *user వాడని)
{
  state_pos = pos;

  ఒకవేళ(want_movecursor)
    రాయి("movecursor %d,%d\n", pos.అడ్డువరుస, pos.నిలువు);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం want_premove = 0;
స్థిర పూర్ణాంకం premove(VTermRect rect, శూన్య *user వాడని)
{
  ఒకవేళ(!want_premove)
    తిరిగి 0;

  రాయి("premove %d..%d,%d..%d\n",
      rect.ప్రారంభం_అడ్డువరుస, rect.చివర_అడ్డువరుస, rect.ప్రారంభం_నిలువు, rect.చివర_నిలువు);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం want_scrollrect = 0;
స్థిర పూర్ణాంకం scrollrect(VTermRect rect, పూర్ణాంకం downward, పూర్ణాంకం rightward, శూన్య *user వాడని)
{
  ఒకవేళ(!want_scrollrect)
    తిరిగి 0;

  రాయి("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.ప్రారంభం_అడ్డువరుస, rect.చివర_అడ్డువరుస, rect.ప్రారంభం_నిలువు, rect.చివర_నిలువు,
      downward, rightward);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం want_moverect = 0;
స్థిర పూర్ణాంకం moverect(VTermRect dest, VTermRect src, శూన్య *user వాడని)
{
  ఒకవేళ(!want_moverect)
    తిరిగి 0;

  రాయి("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.ప్రారంభం_అడ్డువరుస,  src.చివర_అడ్డువరుస,  src.ప్రారంభం_నిలువు,  src.చివర_నిలువు,
      dest.ప్రారంభం_అడ్డువరుస, dest.చివర_అడ్డువరుస, dest.ప్రారంభం_నిలువు, dest.చివర_నిలువు);

  తిరిగి 1;
}

స్థిర పూర్ణాంకం want_settermprop = 0;
స్థిర పూర్ణాంకం settermprop(VTermProp prop, VTerm_విలువ *val, శూన్య *user వాడని)
{
  VTerm_విలువ_Type type;
  ఒకవేళ(!want_settermprop)
    తిరిగి 1;

  type = vterm_get_prop_type(prop);
  ఎంపిక(type) {
  స్థితి VTERM_VALUETYPE_BOOL:
    రాయి("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    తిరిగి 1;
  స్థితి VTERM_VALUETYPE_INT:
    రాయి("settermprop %d %d\n", prop, val->సంఖ్య);
    తిరిగి 1;
  స్థితి VTERM_VALUETYPE_STRING:
    రాయి("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    తిరిగి 1;
  స్థితి VTERM_VALUETYPE_COLOR:
    రాయి("settermprop %d ", prop);
    print_color(&val->color);
    రాయి("\n");
    తిరిగి 1;

  స్థితి VTERM_N_VALUETYPES:
    తిరిగి 0;
  }

  తిరిగి 0;
}

// These callbacks are for State

స్థిర పూర్ణాంకం want_state_putglyph = 0;
స్థిర పూర్ణాంకం state_putglyph(VTermGlyphInfo *info, VTermPos pos, శూన్య *user వాడని)
{
  పూర్ణాంకం i;
  ఒకవేళ(!want_state_putglyph)
    తిరిగి 1;

  రాయి("putglyph ");
  కొరకు(i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL && info->chars[i]; i++)
    రాయి(i ? ",%x" : "%x", info->chars[i]);
  రాయి(" %d %d,%d", info->వెడల్పు, pos.అడ్డువరుస, pos.నిలువు);
  ఒకవేళ(info->protected_cell)
    రాయి(" prot");
  ఒకవేళ(info->dwl)
    రాయి(" dwl");
  ఒకవేళ(info->dhl)
    రాయి(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం want_state_erase = 0;
స్థిర పూర్ణాంకం state_erase(VTermRect rect, పూర్ణాంకం selective, శూన్య *user వాడని)
{
  ఒకవేళ(!want_state_erase)
    తిరిగి 1;

  రాయి("erase %d..%d,%d..%d%s\n",
      rect.ప్రారంభం_అడ్డువరుస, rect.చివర_అడ్డువరుస, rect.ప్రారంభం_నిలువు, rect.చివర_నిలువు,
      selective ? " selective" : "");

  తిరిగి 1;
}

స్థిర నిర్మాణం {
  పూర్ణాంకం bold;
  పూర్ణాంకం underline;
  పూర్ణాంకం italic;
  పూర్ణాంకం blink;
  పూర్ణాంకం తిరగేయి;
  పూర్ణాంకం conceal;
  పూర్ణాంకం strike;
  పూర్ణాంకం font;
  పూర్ణాంకం small;
  పూర్ణాంకం baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
స్థిర పూర్ణాంకం state_setpenattr(VTermAttr attr, VTerm_విలువ *val, శూన్య *user వాడని)
{
  ఎంపిక(attr) {
  స్థితి VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    ఆపు;
  స్థితి VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->సంఖ్య;
    ఆపు;
  స్థితి VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    ఆపు;
  స్థితి VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    ఆపు;
  స్థితి VTERM_ATTR_తిరగేయి:
    state_pen.తిరగేయి = val->boolean;
    ఆపు;
  స్థితి VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    ఆపు;
  స్థితి VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    ఆపు;
  స్థితి VTERM_ATTR_FONT:
    state_pen.font = val->సంఖ్య;
    ఆపు;
  స్థితి VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    ఆపు;
  స్థితి VTERM_ATTR_BASELINE:
    state_pen.baseline = val->సంఖ్య;
    ఆపు;
  స్థితి VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    ఆపు;
  స్థితి VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    ఆపు;

  స్థితి VTERM_N_ATTRS:
    తిరిగి 0;
  }

  తిరిగి 1;
}

స్థిర పూర్ణాంకం state_setlineinfo(పూర్ణాంకం అడ్డువరుస వాడని, స్థిరరాశి VTerm_పంక్తి_Info *newinfo వాడని, స్థిరరాశి VTerm_పంక్తి_Info *oldinfo వాడని, శూన్య *user వాడని)
{
  తిరిగి 1;
}

స్థిర పూర్ణాంకం want_state_scrollback = 0;
స్థిర పూర్ణాంకం state_sb_clear(శూన్య *user వాడని) {
  ఒకవేళ(!want_state_scrollback)
    తిరిగి 1;

  రాయి("sb_clear\n");
  తిరిగి 0;
}

VTermStateCallbacks state_cbs = {
  state_putglyph, // putglyph
  movecursor, // movecursor
  scrollrect, // scrollrect
  moverect, // moverect
  state_erase, // erase
  ఖాళీ, // initpen
  state_setpenattr, // setpenattr
  settermprop, // settermprop
  ఖాళీ, // bell
  ఖాళీ, // resize
  state_setlineinfo, // setlineinfo
  state_sb_clear, // sb_clear
  premove, // premove
};

స్థిర పూర్ణాంకం selection_సమితి(VTerm_Selection_ముసుగు ముసుగు, VTermStringFragment frag, శూన్య *user వాడని)
{
  రాయి("selection-set mask=%04X ", ముసుగు);
  ఒకవేళ(frag.initial)
    రాయి("[");
  printhex(frag.str, frag.len);
  ఒకవేళ(frag.final)
    రాయి("]");
  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం selection_query(VTerm_Selection_ముసుగు ముసుగు, శూన్య *user వాడని)
{
  రాయి("selection-query mask=%04X\n", ముసుగు);

  తిరిగి 1;
}

VTermSelectionCallbacks selection_cbs = {
  .సమితి_2   = selection_సమితి,
  .query = selection_query,
};

స్థిర పూర్ణాంకం want_screen_damage = 0;
స్థిర పూర్ణాంకం want_screen_damage_cells = 0;
స్థిర పూర్ణాంకం screen_damage(VTermRect rect, శూన్య *user వాడని)
{
  ఒకవేళ(!want_screen_damage)
    తిరిగి 1;

  రాయి("damage %d..%d,%d..%d",
      rect.ప్రారంభం_అడ్డువరుస, rect.చివర_అడ్డువరుస, rect.ప్రారంభం_నిలువు, rect.చివర_నిలువు);

  ఒకవేళ(want_screen_damage_cells) {
    పూర్ణాంకం equals = FALSE;
    పూర్ణాంకం అడ్డువరుస;
    పూర్ణాంకం నిలువు;

    కొరకు(అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస; అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++) {
      పూర్ణాంకం eol = rect.చివర_నిలువు;
      అయితే(eol > rect.ప్రారంభం_నిలువు) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.అడ్డువరుస = అడ్డువరుస;
	pos.నిలువు = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        ఒకవేళ(cell.chars[0])
          ఆపు;

        eol--;
      }

      ఒకవేళ(eol == rect.ప్రారంభం_నిలువు)
        ఆపు;

      ఒకవేళ(!equals)
        రాయి(" ="), equals = TRUE;

      రాయి(" %d<", అడ్డువరుస);
      కొరకు(నిలువు = rect.ప్రారంభం_నిలువు; నిలువు < eol; నిలువు++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.అడ్డువరుస = అడ్డువరుస;
	pos.నిలువు = నిలువు;
        vterm_screen_get_cell(screen, pos, &cell);
        రాయి(నిలువు == rect.ప్రారంభం_నిలువు ? "%02X" : " %02X", cell.chars[0]);
      }
      రాయి(">");
    }
  }

  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం want_screen_scrollback = 0;
స్థిర పూర్ణాంకం screen_sb_pushline4(పూర్ణాంకం నిలువుల_సంఖ్య, స్థిరరాశి VTermScreenCell *cells, పూర్ణాంకం continuation, శూన్య *user వాడని)
{
  పూర్ణాంకం eol;
  పూర్ణాంకం c;

  ఒకవేళ(!want_screen_scrollback)
    తిరిగి 1;

  eol = నిలువుల_సంఖ్య;
  అయితే(eol && !cells[eol-1].chars[0])
    eol--;

  రాయి("sb_pushline %d%s =", నిలువుల_సంఖ్య, continuation ? " cont" : "");
  కొరకు(c = 0; c < eol; c++)
    రాయి(" %02X", cells[c].chars[0]);
  రాయి("\n");

  తిరిగి 1;
}

స్థిర పూర్ణాంకం screen_sb_popline(పూర్ణాంకం నిలువుల_సంఖ్య, VTermScreenCell *cells, శూన్య *user వాడని)
{
  పూర్ణాంకం నిలువు;

  ఒకవేళ(!want_screen_scrollback)
    తిరిగి 0;

  // All lines of scrollback contain "ABCDE"
  కొరకు(నిలువు = 0; నిలువు < నిలువుల_సంఖ్య; నిలువు++) {
    ఒకవేళ(నిలువు < 5)
      cells[నిలువు].chars[0] = 'A' + నిలువు;
    లేకపోతే
      cells[నిలువు].chars[0] = 0;

    cells[నిలువు].వెడల్పు = 1;
  }

  రాయి("sb_popline %d\n", నిలువుల_సంఖ్య);
  తిరిగి 1;
}

స్థిర పూర్ణాంకం screen_sb_clear(శూన్య *user వాడని)
{
  ఒకవేళ(!want_screen_scrollback)
    తిరిగి 1;

  రాయి("sb_clear\n");
  తిరిగి 0;
}

VTermScreenCallbacks screen_cbs = {
  screen_damage, // damage
  moverect, // moverect
  movecursor, // movecursor
  settermprop, // settermprop
  ఖాళీ, // bell
  ఖాళీ, // resize
  ఖాళీ, // sb_pushline
  screen_sb_popline, // sb_popline
  screen_sb_clear, // sb_clear
  screen_sb_pushline4, // sb_pushline4
};

పూర్ణాంకం ప్రధాన(పూర్ణాంకం argc వాడని, అక్షరం **argv వాడని)
{
  అక్షరం పంక్తి[1024] = {0};
  పూర్ణాంకం flag;

  పూర్ణాంకం err;

  setvbuf(stdout, ఖాళీ, _IONBF, 0);

  అయితే(fgets(పంక్తి, పరిమాణం పంక్తి, stdin)) {
    అక్షరం *nl;
    size_t outlen;
    err = 0;

    ఒకవేళ((nl = strchr(పంక్తి, '\n')))
      *nl = '\0';

    ఒకవేళ(streq(పంక్తి, "INIT")) {
      ఒకవేళ(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    లేకపోతే ఒకవేళ(streq(పంక్తి, "WANTPARSER")) {
      ప్రతిజ్ఞా(vt);
      vterm_parser_సమితి_callbacks(vt, &parser_cbs, ఖాళీ);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "WANTSTATE") && (పంక్తి[9] == '\0' || పంక్తి[9] == ' ')) {
      పూర్ణాంకం i = 9;
      పూర్ణాంకం sense = 1;
      ప్రతిజ్ఞా(vt);
      ఒకవేళ(!state) {
        state = vterm_obtain_state(vt);
        vterm_state_సమితి_callbacks(state, &state_cbs, ఖాళీ);
        vterm_state_callbacks_has_premove(state);
        /* In some tests we want to check the behaviour of overflowing the
         * buffer, so make it nicely small
         */
        vterm_state_సమితి_selection_callbacks(state, &selection_cbs, ఖాళీ, ఖాళీ, 16);
        vterm_state_సమితి_bold_highbright(state, 1);
        vterm_state_reset(state, 1);
      }

      అయితే(పంక్తి[i] == ' ')
        i++;
      కొరకు( ; పంక్తి[i]; i++)
        ఎంపిక(పంక్తి[i]) {
        స్థితి '+':
          sense = 1;
          ఆపు;
        స్థితి '-':
          sense = 0;
          ఆపు;
        స్థితి 'g':
          want_state_putglyph = sense;
          ఆపు;
        స్థితి 's':
          want_scrollrect = sense;
          ఆపు;
        స్థితి 'P':
          want_premove = sense;
          ఆపు;
        స్థితి 'm':
          want_moverect = sense;
          ఆపు;
        స్థితి 'e':
          want_state_erase = sense;
          ఆపు;
        స్థితి 'p':
          want_settermprop = sense;
          ఆపు;
        స్థితి 'f':
          vterm_state_సమితి_unrecognised_fallbacks(state, sense ? &fallbacks : ఖాళీ, ఖాళీ);
          ఆపు;
        స్థితి 'b':
          want_state_scrollback = sense;
          ఆపు;
        అప్రమేయ:
          fprintf(stderr, "Unrecognised WANTSTATE flag '%c'\n", పంక్తి[i]);
        }
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "WANTSCREEN") && (పంక్తి[10] == '\0' || పంక్తి[10] == ' ')) {
      పూర్ణాంకం i = 10;
      పూర్ణాంకం sense = 1;
      ప్రతిజ్ఞా(vt);
      ఒకవేళ(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_సమితి_callbacks(screen, &screen_cbs, ఖాళీ);
      vterm_screen_callbacks_has_pushline4(screen);

      అయితే(పంక్తి[i] == ' ')
        i++;
      కొరకు( ; పంక్తి[i]; i++)
        ఎంపిక(పంక్తి[i]) {
        స్థితి '-':
          sense = 0;
          ఆపు;
        స్థితి 'a':
          vterm_screen_enable_altscreen(screen, 1);
          ఆపు;
        స్థితి 'd':
          want_screen_damage = sense;
          ఆపు;
        స్థితి 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          ఆపు;
        స్థితి 'm':
          want_moverect = sense;
          ఆపు;
        స్థితి 'c':
          want_movecursor = sense;
          ఆపు;
        స్థితి 'p':
          want_settermprop = 1;
          ఆపు;
        స్థితి 'b':
          want_screen_scrollback = sense;
          ఆపు;
        స్థితి 'r':
          vterm_screen_enable_reflow(screen, sense);
          ఆపు;
        అప్రమేయ:
          fprintf(stderr, "Unrecognised WANTSCREEN flag '%c'\n", పంక్తి[i]);
        }
    }

    లేకపోతే ఒకవేళ(అక్షరశ్రేణి_చదువు(పంక్తి, "UTF8 %d", &flag)) {
      vterm_సమితి_utf8(vt, flag);
    }

    లేకపోతే ఒకవేళ(streq(పంక్తి, "RESET")) {
      ఒకవేళ(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      ఒకవేళ(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "RESIZE ")) {
      పూర్ణాంకం అడ్డువరుసలు_2, నిలువుల_సంఖ్య;
      అక్షరం *linep = పంక్తి + 7;
      అయితే(linep[0] == ' ')
        linep++;
      అక్షరశ్రేణి_చదువు(linep, "%d, %d", &అడ్డువరుసలు_2, &నిలువుల_సంఖ్య);
      vterm_సమితి_కొలత(vt, అడ్డువరుసలు_2, నిలువుల_సంఖ్య);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "PUSH ")) {
      అక్షరం *బైట్లు = పంక్తి + 5;
      size_t len = inplace_hex2bytes(బైట్లు);
      ప్రతిజ్ఞా(len);

      size_t written = vterm_ఆదానం_write(vt, బైట్లు, len);
      ఒకవేళ(written < len)
        fprintf(stderr, "! short write\n");
    }

    లేకపోతే ఒకవేళ(streq(పంక్తి, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      ఒకవేళ(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.దత్తాంశం);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "ENCIN ")) {
      అక్షరం *బైట్లు = పంక్తి + 6;
      size_t len = inplace_hex2bytes(బైట్లు);
      ప్రతిజ్ఞా(len);

      uint32_t cp[1024];
      పూర్ణాంకం cpi = 0;
      size_t pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.దత్తాంశం,
          cp, &cpi, len, బైట్లు, &pos, len);

      ఒకవేళ(cpi > 0) {
	పూర్ణాంకం i;
        రాయి("encout ");
        కొరకు(i = 0; i < cpi; i++) {
          రాయి(i ? ",%x" : "%x", cp[i]);
        }
        రాయి("\n");
      }
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "INCHAR ")) {
      అక్షరం *linep = పంక్తి + 7;
      గుర్తులేని పూర్ణాంకం c = 0;
      VTermModifier mod;
      అయితే(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      అక్షరశ్రేణి_చదువు(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "INKEY ")) {
      VTermModifier mod;
      VTerm_కీలకం కీలకం;
      అక్షరం *linep = పంక్తి + 6;
      అయితే(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      అయితే(linep[0] == ' ')
        linep++;
      కీలకం = strp_కీలకం(linep);

      vterm_keyboard_కీలకం(vt, కీలకం, mod);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "PASTE ")) {
      అక్షరం *linep = పంక్తి + 6;
      ఒకవేళ(streq(linep, "START"))
        vterm_keyboard_ప్రారంభం_paste(vt);
      లేకపోతే ఒకవేళ(streq(linep, "END"))
        vterm_keyboard_చివర_paste(vt);
      లేకపోతే
        వెళ్ళు abort_పంక్తి;
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "FOCUS ")) {
      ప్రతిజ్ఞా(state);
      అక్షరం *linep = పంక్తి + 6;
      ఒకవేళ(streq(linep, "IN"))
        vterm_state_focus_లోపల(state);
      లేకపోతే ఒకవేళ(streq(linep, "OUT"))
        vterm_state_focus_బహిర్గతం(state);
      లేకపోతే
        వెళ్ళు abort_పంక్తి;
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "MOUSEMOVE ")) {
      అక్షరం *linep = పంక్తి + 10;
      పూర్ణాంకం అడ్డువరుస, నిలువు, len;
      VTermModifier mod;
      అయితే(linep[0] == ' ')
        linep++;
      అక్షరశ్రేణి_చదువు(linep, "%d,%d%n", &అడ్డువరుస, &నిలువు, &len);
      linep += len;
      అయితే(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, అడ్డువరుస, నిలువు, mod);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "MOUSEBTN ")) {
      అక్షరం *linep = పంక్తి + 9;
      అక్షరం press;
      పూర్ణాంకం button, len;
      VTermModifier mod;
      అయితే(linep[0] == ' ')
        linep++;
      అక్షరశ్రేణి_చదువు(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      అయితే(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "SELECTION ")) {
      అక్షరం *linep = పంక్తి + 10;
      గుర్తులేని పూర్ణాంకం ముసుగు;
      పూర్ణాంకం len;
      VTermStringFragment frag = { 0 };
      అక్షరశ్రేణి_చదువు(linep, "%x%n", &ముసుగు, &len);
      linep += len;
      అయితే(linep[0] == ' ')
        linep++;
      ఒకవేళ(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        అయితే(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      ప్రతిజ్ఞా(frag.len);

      linep += frag.len * 2;
      అయితే(linep[0] == ' ')
        linep++;
      ఒకవేళ(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, ముసుగు, frag);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "DAMAGEMERGE ")) {
      ప్రతిజ్ఞా(screen);
      అక్షరం *linep = పంక్తి + 12;
      అయితే(linep[0] == ' ')
        linep++;
      ఒకవేళ(streq(linep, "CELL"))
        vterm_screen_సమితి_damage_విలీనం(screen, VTERM_DAMAGE_CELL);
      లేకపోతే ఒకవేళ(streq(linep, "ROW"))
        vterm_screen_సమితి_damage_విలీనం(screen, VTERM_DAMAGE_అడ్డువరుస);
      లేకపోతే ఒకవేళ(streq(linep, "SCREEN"))
        vterm_screen_సమితి_damage_విలీనం(screen, VTERM_DAMAGE_SCREEN);
      లేకపోతే ఒకవేళ(streq(linep, "SCROLL"))
        vterm_screen_సమితి_damage_విలీనం(screen, VTERM_DAMAGE_SCROLL);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "DAMAGEFLUSH")) {
      ప్రతిజ్ఞా(screen);
      vterm_screen_flush_damage(screen);
    }

    లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "SETDEFAULTCOL ")) {
      ప్రతిజ్ఞా(screen);
      అక్షరం *linep = పంక్తి + 14;
      అయితే(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      ఒకవేళ(linep[0]) {
        అయితే(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_సమితి_default_colors(screen, &fg, &bg);
      }
      లేకపోతే
        vterm_screen_సమితి_default_colors(screen, &fg, ఖాళీ);
    }

    లేకపోతే ఒకవేళ(పంక్తి[0] == '?') {
      ఒకవేళ(streq(పంక్తి, "?cursor")) {
        ప్రతిజ్ఞా(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        ఒకవేళ(pos.అడ్డువరుస != state_pos.అడ్డువరుస)
          రాయి("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.అడ్డువరుస, pos.నిలువు, state_pos.అడ్డువరుస, state_pos.నిలువు);
        లేకపోతే ఒకవేళ(pos.నిలువు != state_pos.నిలువు)
          రాయి("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.అడ్డువరుస, pos.నిలువు, state_pos.అడ్డువరుస, state_pos.నిలువు);
        లేకపోతే
          రాయి("%d,%d\n", state_pos.అడ్డువరుస, state_pos.నిలువు);
      }
      లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "?pen ")) {
        ప్రతిజ్ఞా(state);
        VTerm_విలువ val;
        అక్షరం *linep = పంక్తి + 5;
        అయితే(linep[0] == ' ')
          linep++;

#నిర్వచించు BOOLSTR(v) ((v) ? "on" : "off")

        ఒకవేళ(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          ఒకవేళ(val.boolean != state_pen.bold)
            రాయి("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          లేకపోతే
            రాయి("%s\n", BOOLSTR(state_pen.bold));
        }
        లేకపోతే ఒకవేళ(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          ఒకవేళ(val.boolean != state_pen.underline)
            రాయి("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          లేకపోతే
            రాయి("%d\n", state_pen.underline);
        }
        లేకపోతే ఒకవేళ(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          ఒకవేళ(val.boolean != state_pen.italic)
            రాయి("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          లేకపోతే
            రాయి("%s\n", BOOLSTR(state_pen.italic));
        }
        లేకపోతే ఒకవేళ(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          ఒకవేళ(val.boolean != state_pen.blink)
            రాయి("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          లేకపోతే
            రాయి("%s\n", BOOLSTR(state_pen.blink));
        }
        లేకపోతే ఒకవేళ(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_తిరగేయి, &val);
          ఒకవేళ(val.boolean != state_pen.తిరగేయి)
            రాయి("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.తిరగేయి));
          లేకపోతే
            రాయి("%s\n", BOOLSTR(state_pen.తిరగేయి));
        }
        లేకపోతే ఒకవేళ(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          ఒకవేళ(val.boolean != state_pen.font)
            రాయి("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          లేకపోతే
            రాయి("%d\n", state_pen.font);
        }
        లేకపోతే ఒకవేళ(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          ఒకవేళ(val.boolean != state_pen.small)
            రాయి("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          లేకపోతే
            రాయి("%s\n", BOOLSTR(state_pen.small));
        }
        లేకపోతే ఒకవేళ(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          ఒకవేళ(val.సంఖ్య != state_pen.baseline)
            రాయి("! pen baseline mismatch: state=%d, event=%d\n",
                val.సంఖ్య, state_pen.baseline);
          లేకపోతే
            రాయి("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_క్రింది ? "lower"
                         : "normal");
        }
        లేకపోతే ఒకవేళ(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          రాయి("\n");
        }
        లేకపోతే ఒకవేళ(streq(linep, "background")) {
          print_color(&state_pen.background);
          రాయి("\n");
        }
        లేకపోతే
          రాయి("?\n");
      }
      లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "?lineinfo ")) {
        ప్రతిజ్ఞా(state);
        అక్షరం *linep = పంక్తి + 10;
        పూర్ణాంకం అడ్డువరుస;
        స్థిరరాశి VTerm_పంక్తి_Info *info;
        అయితే(linep[0] == ' ')
          linep++;
        ఒకవేళ(అక్షరశ్రేణి_చదువు(linep, "%d", &అడ్డువరుస) < 1) {
          రాయి("! lineinfo unrecognised input\n");
          వెళ్ళు abort_పంక్తి;
        }
        info = vterm_state_get_lineinfo(state, అడ్డువరుస);
        ఒకవేళ(info->doublewidth)
          రాయి("dwl ");
        ఒకవేళ(info->doubleheight)
          రాయి("dhl ");
        ఒకవేళ(info->continuation)
          రాయి("cont ");
        రాయి("\n");
      }
      లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "?screen_chars ")) {
        ప్రతిజ్ఞా(screen);
        అక్షరం *linep = పంక్తి + 13;
        VTermRect rect;
        size_t len;
        అయితే(linep[0] == ' ')
          linep++;
        ఒకవేళ(అక్షరశ్రేణి_చదువు(linep, "%d,%d,%d,%d", &rect.ప్రారంభం_అడ్డువరుస, &rect.ప్రారంభం_నిలువు, &rect.చివర_అడ్డువరుస, &rect.చివర_నిలువు) == 4)
          ; // fine
        లేకపోతే ఒకవేళ(అక్షరశ్రేణి_చదువు(linep, "%d", &rect.ప్రారంభం_అడ్డువరుస) == 1) {
          rect.చివర_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస + 1;
          rect.ప్రారంభం_నిలువు = 0;
          vterm_get_కొలత(vt, ఖాళీ, &rect.చివర_నిలువు);
        }
        లేకపోతే {
          రాయి("! screen_chars unrecognised input\n");
          వెళ్ళు abort_పంక్తి;
        }
        len = vterm_screen_get_chars(screen, ఖాళీ, 0, rect);
        ఒకవేళ(len == (size_t)-1)
          రాయి("! screen_chars error\n");
        లేకపోతే ఒకవేళ(len == 0)
          రాయి("\n");
        లేకపోతే {
          uint32_t *chars = జ్ఞాపకం_ఇవ్వు(పరిమాణం(uint32_t) * len);
          size_t i;
          vterm_screen_get_chars(screen, chars, len, rect);
          కొరకు(i = 0; i < len; i++) {
            రాయి("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          విడుదల(chars);
        }
      }
      లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "?screen_text ")) {
        ప్రతిజ్ఞా(screen);
        అక్షరం *linep = పంక్తి + 12;
        VTermRect rect;
        size_t len;
        అయితే(linep[0] == ' ')
          linep++;
        ఒకవేళ(అక్షరశ్రేణి_చదువు(linep, "%d,%d,%d,%d", &rect.ప్రారంభం_అడ్డువరుస, &rect.ప్రారంభం_నిలువు, &rect.చివర_అడ్డువరుస, &rect.చివర_నిలువు) < 4) {
          రాయి("! screen_text unrecognised input\n");
          వెళ్ళు abort_పంక్తి;
        }
        len = vterm_screen_get_పాఠ్యం(screen, ఖాళీ, 0, rect);
        ఒకవేళ(len == (size_t)-1)
          రాయి("! screen_text error\n");
        లేకపోతే ఒకవేళ(len == 0)
          రాయి("\n");
        లేకపోతే {
          // Put an overwrite guard at both ends of the buffer
          గుర్తులేని అక్షరం *బఫరు = జ్ఞాపకం_ఇవ్వు(len + 4);
          గుర్తులేని అక్షరం *పాఠ్యం = బఫరు + 2;
          పాఠ్యం[-2] = 0x55; పాఠ్యం[-1] = 0xAA;
          పాఠ్యం[len] = 0x55; పాఠ్యం[len+1] = 0xAA;

          vterm_screen_get_పాఠ్యం(screen, (అక్షరం *)పాఠ్యం, len, rect);

          ఒకవేళ(పాఠ్యం[-2] != 0x55 || పాఠ్యం[-1] != 0xAA)
            రాయి("! screen_get_text buffer overrun left [%02x,%02x]\n", పాఠ్యం[-2], పాఠ్యం[-1]);
          లేకపోతే ఒకవేళ(పాఠ్యం[len] != 0x55 || పాఠ్యం[len+1] != 0xAA)
            రాయి("! screen_get_text buffer overrun right [%02x,%02x]\n", పాఠ్యం[len], పాఠ్యం[len+1]);
          లేకపోతే
	  {
	    size_t i;
            కొరకు(i = 0; i < len; i++) {
              రాయి("0x%02x%s", పాఠ్యం[i], i < len-1 ? "," : "\n");
            }
	  }

          విడుదల(బఫరు);
        }
      }
      లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "?screen_cell ")) {
        ప్రతిజ్ఞా(screen);
        అక్షరం *linep = పంక్తి + 12;
	పూర్ణాంకం i;
        VTermPos pos;
        VTermScreenCell cell;
        అయితే(linep[0] == ' ')
          linep++;
        ఒకవేళ(అక్షరశ్రేణి_చదువు(linep, "%d,%d\n", &pos.అడ్డువరుస, &pos.నిలువు) < 2) {
          రాయి("! screen_cell unrecognised input\n");
          వెళ్ళు abort_పంక్తి;
        }
        ఒకవేళ(!vterm_screen_get_cell(screen, pos, &cell))
          వెళ్ళు abort_పంక్తి;
        రాయి("{");
        కొరకు(i = 0; i < VTERM_MAX_CHARS_చొప్పున_CELL && cell.chars[i]; i++) {
          రాయి("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        రాయి("} width=%d attrs={", cell.వెడల్పు);
        ఒకవేళ(cell.attrs.bold)      రాయి("B");
        ఒకవేళ(cell.attrs.underline) రాయి("U%d", cell.attrs.underline);
        ఒకవేళ(cell.attrs.italic)    రాయి("I");
        ఒకవేళ(cell.attrs.blink)     రాయి("K");
        ఒకవేళ(cell.attrs.తిరగేయి)   రాయి("R");
        ఒకవేళ(cell.attrs.font)      రాయి("F%d", cell.attrs.font);
        ఒకవేళ(cell.attrs.small)     రాయి("S");
        ఒకవేళ(cell.attrs.baseline)  రాయి(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        రాయి("} ");
        ఒకవేళ(cell.attrs.dwl)       రాయి("dwl ");
        ఒకవేళ(cell.attrs.dhl)       రాయి("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        రాయి("fg=");
        vterm_screen_convert_color_గమ్యం_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        రాయి(" bg=");
        vterm_screen_convert_color_గమ్యం_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        రాయి("\n");
      }
      లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "?screen_eol ")) {
        ప్రతిజ్ఞా(screen);
        VTermPos pos;
        అక్షరం *linep = పంక్తి + 12;
        అయితే(linep[0] == ' ')
          linep++;
        ఒకవేళ(అక్షరశ్రేణి_చదువు(linep, "%d,%d\n", &pos.అడ్డువరుస, &pos.నిలువు) < 2) {
          రాయి("! screen_eol unrecognised input\n");
          వెళ్ళు abort_పంక్తి;
        }
        రాయి("%d\n", vterm_screen_is_eol(screen, pos));
      }
      లేకపోతే ఒకవేళ(strstartswith(పంక్తి, "?screen_attrs_extent ")) {
        ప్రతిజ్ఞా(screen);
        VTermPos pos;
        VTermRect rect;
        అక్షరం *linep = పంక్తి + 21;
        అయితే(linep[0] == ' ')
          linep++;
        ఒకవేళ(అక్షరశ్రేణి_చదువు(linep, "%d,%d\n", &pos.అడ్డువరుస, &pos.నిలువు) < 2) {
          రాయి("! screen_attrs_extent unrecognised input\n");
          వెళ్ళు abort_పంక్తి;
        }
	rect.ప్రారంభం_నిలువు = 0;
	rect.చివర_నిలువు   = -1;
        ఒకవేళ(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          రాయి("! screen_attrs_extent failed\n");
          వెళ్ళు abort_పంక్తి;
        }
        రాయి("%d,%d-%d,%d\n", rect.ప్రారంభం_అడ్డువరుస, rect.ప్రారంభం_నిలువు, rect.చివర_అడ్డువరుస, rect.చివర_నిలువు);
      }
      లేకపోతే
        రాయి("?\n");

      memset(పంక్తి, 0, పరిమాణం పంక్తి);
      కొనసాగు;
    }

    లేకపోతే
      abort_పంక్తి: err = 1;

    outlen = vterm_output_get_బఫరు_current(vt);
    ఒకవేళ(outlen > 0) {
      అక్షరం outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      పదం_output(outbuff, outlen, ఖాళీ);
    }

    రాయి(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  తిరిగి 0;
}
