#अन्तर्भाव "vterm.h"
#अन्तर्भाव "../src/vterm_internal.h" // We pull in some internal bits too

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#define streq(a,b) (!म_भेद(a,b))
#define strstartswith(a,b) (!म_भेदन(a,b,म_माप(b)))

स्थिर माप_प्रकार inplace_hex2bytes(वर्ण *s)
{
  वर्ण *inpos = s, *outpos = s;

  यावत्(*inpos) {
    अचिन्हित पूर्णाङ्क ch;
    यदि(माला_पूछो(inpos, "%2x", &ch) < 1)
      भङ्ग;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  प्रत्यागम outpos - s;
}

स्थिर VTermModifier strpe_modifiers(वर्ण **strp)
{
  VTermModifier state = 0;

  यावत्((*strp)[0]) {
    विकल्प(((*strp)++)[0]) {
      पक्ष 'S': state |= VTERM_MOD_SHIFT; भङ्ग;
      पक्ष 'C': state |= VTERM_MOD_CTRL;  भङ्ग;
      पक्ष 'A': state |= VTERM_MOD_ALT;   भङ्ग;
      स्वभाव: प्रत्यागम state;
    }
  }

  प्रत्यागम state;
}

स्थिर VTerm_कुंजी strp_कुंजी(वर्ण *str)
{
  स्थिर संरचना {
    वर्ण *name;
    VTerm_कुंजी कुंजी;
  } कुंजियाँ[] = {
    { "Up",    VTERM_कुंजी_UP },
    { "Tab",   VTERM_कुंजी_TAB },
    { "Enter", VTERM_कुंजी_ENTER },
    { "KP0",   VTERM_कुंजी_KP_0 },
    { "F1",    VTERM_कुंजी_FUNCTION(1) },
    { रिक्त,    VTERM_कुंजी_NONE },
  };
  पूर्णाङ्क i;

  प्रति(i = 0; कुंजियाँ[i].name; i++) {
    यदि(streq(str, कुंजियाँ[i].name))
      प्रत्यागम कुंजियाँ[i].कुंजी;
  }

  प्रत्यागम VTERM_कुंजी_NONE;
}

स्थिर शून्य print_color(ध्रुव VTermColor *col)
{
  यदि (VTERM_COLOR_IS_RGB(col)) {
    म_लिखो("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  अन्यथा यदि (VTERM_COLOR_IS_INDEXED(col)) {
    म_लिखो("idx(%d", col->index);
  }
  अन्यथा {
    म_लिखो("invalid(%d", col->type);
  }
  यदि (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    म_लिखो(",is_default_fg");
  }
  यदि (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    म_लिखो(",is_default_bg");
  }
  म_लिखो(")");
}

स्थिर VTermColor strpe_color(वर्ण **strp)
{
  uint8_t r, g, b, idx;
  पूर्णाङ्क len = 0;
  VTermColor col;

  यदि(माला_पूछो(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  अन्यथा यदि(माला_पूछो(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  अन्यथा
    vterm_color_rgb(&col, 127, 127, 127);

  प्रत्यागम col;
}

स्थिर VTerm *vt;
स्थिर VTermState *state;
स्थिर VTermScreen *screen;

स्थिर VTermEncodingInstance encoding;

स्थिर शून्य term_output(ध्रुव वर्ण *s, माप_प्रकार len, शून्य *user UNUSED)
{
  माप_प्रकार i;

  म_लिखो("output ");
  प्रति(i = 0; i < len; i++)
    म_लिखो("%x%s", (अचिन्हित वर्ण)s[i], i < len-1 ? "," : "\n");
}

स्थिर शून्य printhex(ध्रुव वर्ण *s, माप_प्रकार len)
{
  यावत्(len--)
    म_लिखो("%02x", (uint8_t)(s++)[0]);
}

स्थिर पूर्णाङ्क parser_text(ध्रुव वर्ण bytes[], माप_प्रकार len, शून्य *user UNUSED)
{
  माप_प्रकार i;

  म_लिखो("text ");
  प्रति(i = 0; i < len; i++) {
    अचिन्हित वर्ण b = bytes[i];
    यदि(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      भङ्ग;
    म_लिखो(i ? ",%x" : "%x", b);
  }
  म_लिखो("\n");

  प्रत्यागम i;
}

स्थिर पूर्णाङ्क parser_control(अचिन्हित वर्ण control, शून्य *user UNUSED)
{
  म_लिखो("control %02x\n", control);

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क parser_escape(ध्रुव वर्ण bytes[], माप_प्रकार len, शून्य *user UNUSED)
{
  यदि(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    यदि(len < 2)
      प्रत्यागम -1;
    len = 2;
  }
  अन्यथा {
    len = 1;
  }

  म_लिखो("escape ");
  printhex(bytes, len);
  म_लिखो("\n");

  प्रत्यागम len;
}

स्थिर पूर्णाङ्क parser_csi(ध्रुव वर्ण *leader, ध्रुव दीर्घ args[], पूर्णाङ्क argcount, ध्रुव वर्ण *intermed, वर्ण command, शून्य *user UNUSED)
{
  पूर्णाङ्क i;
  म_लिखो("csi %02x", command);

  यदि(leader && leader[0]) {
    म_लिखो(" L=");
    प्रति(i = 0; leader[i]; i++)
      म_लिखो("%02x", leader[i]);
  }

  प्रति(i = 0; i < argcount; i++) {
    वर्ण sep = i ? ',' : ' ';

    यदि(args[i] == CSI_ARG_MISSING)
      म_लिखो("%c*", sep);
    अन्यथा
      म_लिखो("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  यदि(intermed && intermed[0]) {
    म_लिखो(" I=");
    प्रति(i = 0; intermed[i]; i++)
      म_लिखो("%02x", intermed[i]);
  }

  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क parser_osc(पूर्णाङ्क command, VTermStringFragment frag, शून्य *user UNUSED)
{

  म_लिखो("osc ");

  यदि(frag.initial) {
    यदि(command == -1)
      म_लिखो("[");
    अन्यथा
      म_लिखो("[%d;", command);
  }

  printhex(frag.str, frag.len);

  यदि(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क parser_dcs(ध्रुव वर्ण *command, माप_प्रकार commandlen, VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("dcs ");

  यदि(frag.initial) {
    माप_प्रकार i;
    म_लिखो("[");
    प्रति(i = 0; i < commandlen; i++)
      म_लिखो("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  यदि(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क parser_apc(VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("apc ");

  यदि(frag.initial)
    म_लिखो("[");

  printhex(frag.str, frag.len);

  यदि(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क parser_pm(VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("pm ");

  यदि(frag.initial)
    म_लिखो("[");

  printhex(frag.str, frag.len);

  यदि(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क parser_sos(VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("sos ");

  यदि(frag.initial)
    म_लिखो("[");

  printhex(frag.str, frag.len);

  यदि(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  प्रत्यागम 1;
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

  यदि(want_movecursor)
    म_लिखो("movecursor %d,%d\n", pos.row, pos.col);

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क want_premove = 0;
स्थिर पूर्णाङ्क premove(VTermRect rect, शून्य *user UNUSED)
{
  यदि(!want_premove)
    प्रत्यागम 0;

  म_लिखो("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क want_scrollrect = 0;
स्थिर पूर्णाङ्क scrollrect(VTermRect rect, पूर्णाङ्क downward, पूर्णाङ्क rightward, शून्य *user UNUSED)
{
  यदि(!want_scrollrect)
    प्रत्यागम 0;

  म_लिखो("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क want_moverect = 0;
स्थिर पूर्णाङ्क moverect(VTermRect dest, VTermRect src, शून्य *user UNUSED)
{
  यदि(!want_moverect)
    प्रत्यागम 0;

  म_लिखो("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क want_settermprop = 0;
स्थिर पूर्णाङ्क settermprop(VTermProp prop, VTerm_मूल्य *val, शून्य *user UNUSED)
{
  VTerm_मूल्य_Type type;
  यदि(!want_settermprop)
    प्रत्यागम 1;

  type = vterm_get_prop_type(prop);
  विकल्प(type) {
  पक्ष VTERM_VALUETYPE_BOOL:
    म_लिखो("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    प्रत्यागम 1;
  पक्ष VTERM_VALUETYPE_INT:
    म_लिखो("settermprop %d %d\n", prop, val->संख्या);
    प्रत्यागम 1;
  पक्ष VTERM_VALUETYPE_STRING:
    म_लिखो("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    प्रत्यागम 1;
  पक्ष VTERM_VALUETYPE_COLOR:
    म_लिखो("settermprop %d ", prop);
    print_color(&val->color);
    म_लिखो("\n");
    प्रत्यागम 1;

  पक्ष VTERM_N_VALUETYPES:
    प्रत्यागम 0;
  }

  प्रत्यागम 0;
}

// These callbacks are for State

स्थिर पूर्णाङ्क want_state_putglyph = 0;
स्थिर पूर्णाङ्क state_putglyph(VTermGlyphInfo *info, VTermPos pos, शून्य *user UNUSED)
{
  पूर्णाङ्क i;
  यदि(!want_state_putglyph)
    प्रत्यागम 1;

  म_लिखो("putglyph ");
  प्रति(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    म_लिखो(i ? ",%x" : "%x", info->chars[i]);
  म_लिखो(" %d %d,%d", info->width, pos.row, pos.col);
  यदि(info->protected_cell)
    म_लिखो(" prot");
  यदि(info->dwl)
    म_लिखो(" dwl");
  यदि(info->dhl)
    म_लिखो(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क want_state_erase = 0;
स्थिर पूर्णाङ्क state_erase(VTermRect rect, पूर्णाङ्क selective, शून्य *user UNUSED)
{
  यदि(!want_state_erase)
    प्रत्यागम 1;

  म_लिखो("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  प्रत्यागम 1;
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
स्थिर पूर्णाङ्क state_setpenattr(VTermAttr attr, VTerm_मूल्य *val, शून्य *user UNUSED)
{
  विकल्प(attr) {
  पक्ष VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->संख्या;
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
    state_pen.font = val->संख्या;
    भङ्ग;
  पक्ष VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    भङ्ग;
  पक्ष VTERM_ATTR_BASELINE:
    state_pen.baseline = val->संख्या;
    भङ्ग;
  पक्ष VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    भङ्ग;
  पक्ष VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    भङ्ग;

  पक्ष VTERM_N_ATTRS:
    प्रत्यागम 0;
  }

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क state_setlineinfo(पूर्णाङ्क row UNUSED, ध्रुव VTerm_पंक्ति_Info *newinfo UNUSED, ध्रुव VTerm_पंक्ति_Info *oldinfo UNUSED, शून्य *user UNUSED)
{
  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क want_state_scrollback = 0;
स्थिर पूर्णाङ्क state_sb_clear(शून्य *user UNUSED) {
  यदि(!want_state_scrollback)
    प्रत्यागम 1;

  म_लिखो("sb_clear\n");
  प्रत्यागम 0;
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
  म_लिखो("selection-set mask=%04X ", mask);
  यदि(frag.initial)
    म_लिखो("[");
  printhex(frag.str, frag.len);
  यदि(frag.final)
    म_लिखो("]");
  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क selection_query(VTermSelectionMask mask, शून्य *user UNUSED)
{
  म_लिखो("selection-query mask=%04X\n", mask);

  प्रत्यागम 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

स्थिर पूर्णाङ्क want_screen_damage = 0;
स्थिर पूर्णाङ्क want_screen_damage_cells = 0;
स्थिर पूर्णाङ्क screen_damage(VTermRect rect, शून्य *user UNUSED)
{
  यदि(!want_screen_damage)
    प्रत्यागम 1;

  म_लिखो("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  यदि(want_screen_damage_cells) {
    पूर्णाङ्क equals = FALSE;
    पूर्णाङ्क row;
    पूर्णाङ्क col;

    प्रति(row = rect.start_row; row < rect.end_row; row++) {
      पूर्णाङ्क eol = rect.end_col;
      यावत्(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        यदि(cell.chars[0])
          भङ्ग;

        eol--;
      }

      यदि(eol == rect.start_col)
        भङ्ग;

      यदि(!equals)
        म_लिखो(" ="), equals = TRUE;

      म_लिखो(" %d<", row);
      प्रति(col = rect.start_col; col < eol; col++) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = col;
        vterm_screen_get_cell(screen, pos, &cell);
        म_लिखो(col == rect.start_col ? "%02X" : " %02X", cell.chars[0]);
      }
      म_लिखो(">");
    }
  }

  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क want_screen_scrollback = 0;
स्थिर पूर्णाङ्क screen_sb_pushline4(पूर्णाङ्क cols, ध्रुव VTermScreenCell *cells, पूर्णाङ्क continuation, शून्य *user UNUSED)
{
  पूर्णाङ्क eol;
  पूर्णाङ्क c;

  यदि(!want_screen_scrollback)
    प्रत्यागम 1;

  eol = cols;
  यावत्(eol && !cells[eol-1].chars[0])
    eol--;

  म_लिखो("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  प्रति(c = 0; c < eol; c++)
    म_लिखो(" %02X", cells[c].chars[0]);
  म_लिखो("\n");

  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क screen_sb_popline(पूर्णाङ्क cols, VTermScreenCell *cells, शून्य *user UNUSED)
{
  पूर्णाङ्क col;

  यदि(!want_screen_scrollback)
    प्रत्यागम 0;

  // All lines of scrollback contain "ABCDE"
  प्रति(col = 0; col < cols; col++) {
    यदि(col < 5)
      cells[col].chars[0] = 'A' + col;
    अन्यथा
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  म_लिखो("sb_popline %d\n", cols);
  प्रत्यागम 1;
}

स्थिर पूर्णाङ्क screen_sb_clear(शून्य *user UNUSED)
{
  यदि(!want_screen_scrollback)
    प्रत्यागम 1;

  म_लिखो("sb_clear\n");
  प्रत्यागम 0;
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
  वर्ण पंक्ति[1024] = {0};
  पूर्णाङ्क flag;

  पूर्णाङ्क err;

  रखो_भबफ(मानक_निकास, रिक्त, _IONBF, 0);

  यावत्(ख_माला_लो(पंक्ति, परिमाण पंक्ति, मानक_निवेश)) {
    वर्ण *nl;
    माप_प्रकार outlen;
    err = 0;

    यदि((nl = म_अक्षर(पंक्ति, '\n')))
      *nl = '\0';

    यदि(streq(पंक्ति, "INIT")) {
      यदि(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    अन्यथा यदि(streq(पंक्ति, "WANTPARSER")) {
      प्रतिज्ञा(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, रिक्त);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "WANTSTATE") && (पंक्ति[9] == '\0' || पंक्ति[9] == ' ')) {
      पूर्णाङ्क i = 9;
      पूर्णाङ्क sense = 1;
      प्रतिज्ञा(vt);
      यदि(!state) {
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

      यावत्(पंक्ति[i] == ' ')
        i++;
      प्रति( ; पंक्ति[i]; i++)
        विकल्प(पंक्ति[i]) {
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
          ख_लिखो(मानक_त्रुटि, "Unrecognised WANTSTATE flag '%c'\n", पंक्ति[i]);
        }
    }

    अन्यथा यदि(strstartswith(पंक्ति, "WANTSCREEN") && (पंक्ति[10] == '\0' || पंक्ति[10] == ' ')) {
      पूर्णाङ्क i = 10;
      पूर्णाङ्क sense = 1;
      प्रतिज्ञा(vt);
      यदि(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, रिक्त);
      vterm_screen_callbacks_has_pushline4(screen);

      यावत्(पंक्ति[i] == ' ')
        i++;
      प्रति( ; पंक्ति[i]; i++)
        विकल्प(पंक्ति[i]) {
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
          ख_लिखो(मानक_त्रुटि, "Unrecognised WANTSCREEN flag '%c'\n", पंक्ति[i]);
        }
    }

    अन्यथा यदि(माला_पूछो(पंक्ति, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    अन्यथा यदि(streq(पंक्ति, "RESET")) {
      यदि(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      यदि(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    अन्यथा यदि(strstartswith(पंक्ति, "RESIZE ")) {
      पूर्णाङ्क rows, cols;
      वर्ण *linep = पंक्ति + 7;
      यावत्(linep[0] == ' ')
        linep++;
      माला_पूछो(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "PUSH ")) {
      वर्ण *bytes = पंक्ति + 5;
      माप_प्रकार len = inplace_hex2bytes(bytes);
      प्रतिज्ञा(len);

      माप_प्रकार written = vterm_निवेश_write(vt, bytes, len);
      यदि(written < len)
        ख_लिखो(मानक_त्रुटि, "! short write\n");
    }

    अन्यथा यदि(streq(पंक्ति, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      यदि(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "ENCIN ")) {
      वर्ण *bytes = पंक्ति + 6;
      माप_प्रकार len = inplace_hex2bytes(bytes);
      प्रतिज्ञा(len);

      uint32_t cp[1024];
      पूर्णाङ्क cpi = 0;
      माप_प्रकार pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      यदि(cpi > 0) {
	पूर्णाङ्क i;
        म_लिखो("encout ");
        प्रति(i = 0; i < cpi; i++) {
          म_लिखो(i ? ",%x" : "%x", cp[i]);
        }
        म_लिखो("\n");
      }
    }

    अन्यथा यदि(strstartswith(पंक्ति, "INCHAR ")) {
      वर्ण *linep = पंक्ति + 7;
      अचिन्हित पूर्णाङ्क c = 0;
      VTermModifier mod;
      यावत्(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      माला_पूछो(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "INKEY ")) {
      VTermModifier mod;
      VTerm_कुंजी कुंजी;
      वर्ण *linep = पंक्ति + 6;
      यावत्(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      यावत्(linep[0] == ' ')
        linep++;
      कुंजी = strp_कुंजी(linep);

      vterm_keyboard_कुंजी(vt, कुंजी, mod);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "PASTE ")) {
      वर्ण *linep = पंक्ति + 6;
      यदि(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      अन्यथा यदि(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      अन्यथा
        गच्छ abort_पंक्ति;
    }

    अन्यथा यदि(strstartswith(पंक्ति, "FOCUS ")) {
      प्रतिज्ञा(state);
      वर्ण *linep = पंक्ति + 6;
      यदि(streq(linep, "IN"))
        vterm_state_focus_in(state);
      अन्यथा यदि(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      अन्यथा
        गच्छ abort_पंक्ति;
    }

    अन्यथा यदि(strstartswith(पंक्ति, "MOUSEMOVE ")) {
      वर्ण *linep = पंक्ति + 10;
      पूर्णाङ्क row, col, len;
      VTermModifier mod;
      यावत्(linep[0] == ' ')
        linep++;
      माला_पूछो(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      यावत्(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "MOUSEBTN ")) {
      वर्ण *linep = पंक्ति + 9;
      वर्ण press;
      पूर्णाङ्क button, len;
      VTermModifier mod;
      यावत्(linep[0] == ' ')
        linep++;
      माला_पूछो(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      यावत्(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "SELECTION ")) {
      वर्ण *linep = पंक्ति + 10;
      अचिन्हित पूर्णाङ्क mask;
      पूर्णाङ्क len;
      VTermStringFragment frag = { 0 };
      माला_पूछो(linep, "%x%n", &mask, &len);
      linep += len;
      यावत्(linep[0] == ' ')
        linep++;
      यदि(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        यावत्(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      प्रतिज्ञा(frag.len);

      linep += frag.len * 2;
      यावत्(linep[0] == ' ')
        linep++;
      यदि(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "DAMAGEMERGE ")) {
      प्रतिज्ञा(screen);
      वर्ण *linep = पंक्ति + 12;
      यावत्(linep[0] == ' ')
        linep++;
      यदि(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      अन्यथा यदि(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      अन्यथा यदि(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      अन्यथा यदि(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "DAMAGEFLUSH")) {
      प्रतिज्ञा(screen);
      vterm_screen_flush_damage(screen);
    }

    अन्यथा यदि(strstartswith(पंक्ति, "SETDEFAULTCOL ")) {
      प्रतिज्ञा(screen);
      वर्ण *linep = पंक्ति + 14;
      यावत्(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      यदि(linep[0]) {
        यावत्(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      अन्यथा
        vterm_screen_set_default_colors(screen, &fg, रिक्त);
    }

    अन्यथा यदि(पंक्ति[0] == '?') {
      यदि(streq(पंक्ति, "?cursor")) {
        प्रतिज्ञा(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        यदि(pos.row != state_pos.row)
          म_लिखो("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        अन्यथा यदि(pos.col != state_pos.col)
          म_लिखो("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        अन्यथा
          म_लिखो("%d,%d\n", state_pos.row, state_pos.col);
      }
      अन्यथा यदि(strstartswith(पंक्ति, "?pen ")) {
        प्रतिज्ञा(state);
        VTerm_मूल्य val;
        वर्ण *linep = पंक्ति + 5;
        यावत्(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        यदि(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          यदि(val.boolean != state_pen.bold)
            म_लिखो("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.bold));
        }
        अन्यथा यदि(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          यदि(val.boolean != state_pen.underline)
            म_लिखो("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          अन्यथा
            म_लिखो("%d\n", state_pen.underline);
        }
        अन्यथा यदि(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          यदि(val.boolean != state_pen.italic)
            म_लिखो("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.italic));
        }
        अन्यथा यदि(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          यदि(val.boolean != state_pen.blink)
            म_लिखो("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.blink));
        }
        अन्यथा यदि(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          यदि(val.boolean != state_pen.reverse)
            म_लिखो("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.reverse));
        }
        अन्यथा यदि(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          यदि(val.boolean != state_pen.font)
            म_लिखो("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          अन्यथा
            म_लिखो("%d\n", state_pen.font);
        }
        अन्यथा यदि(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          यदि(val.boolean != state_pen.small)
            म_लिखो("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.small));
        }
        अन्यथा यदि(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          यदि(val.संख्या != state_pen.baseline)
            म_लिखो("! pen baseline mismatch: state=%d, event=%d\n",
                val.संख्या, state_pen.baseline);
          अन्यथा
            म_लिखो("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        अन्यथा यदि(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          म_लिखो("\n");
        }
        अन्यथा यदि(streq(linep, "background")) {
          print_color(&state_pen.background);
          म_लिखो("\n");
        }
        अन्यथा
          म_लिखो("?\n");
      }
      अन्यथा यदि(strstartswith(पंक्ति, "?lineinfo ")) {
        प्रतिज्ञा(state);
        वर्ण *linep = पंक्ति + 10;
        पूर्णाङ्क row;
        ध्रुव VTerm_पंक्ति_Info *info;
        यावत्(linep[0] == ' ')
          linep++;
        यदि(माला_पूछो(linep, "%d", &row) < 1) {
          म_लिखो("! lineinfo unrecognised input\n");
          गच्छ abort_पंक्ति;
        }
        info = vterm_state_get_lineinfo(state, row);
        यदि(info->doublewidth)
          म_लिखो("dwl ");
        यदि(info->doubleheight)
          म_लिखो("dhl ");
        यदि(info->continuation)
          म_लिखो("cont ");
        म_लिखो("\n");
      }
      अन्यथा यदि(strstartswith(पंक्ति, "?screen_chars ")) {
        प्रतिज्ञा(screen);
        वर्ण *linep = पंक्ति + 13;
        VTermRect rect;
        माप_प्रकार len;
        यावत्(linep[0] == ' ')
          linep++;
        यदि(माला_पूछो(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        अन्यथा यदि(माला_पूछो(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, रिक्त, &rect.end_col);
        }
        अन्यथा {
          म_लिखो("! screen_chars unrecognised input\n");
          गच्छ abort_पंक्ति;
        }
        len = vterm_screen_get_chars(screen, रिक्त, 0, rect);
        यदि(len == (माप_प्रकार)-1)
          म_लिखो("! screen_chars error\n");
        अन्यथा यदि(len == 0)
          म_लिखो("\n");
        अन्यथा {
          uint32_t *chars = दो_स्मृति(परिमाण(uint32_t) * len);
          माप_प्रकार i;
          vterm_screen_get_chars(screen, chars, len, rect);
          प्रति(i = 0; i < len; i++) {
            म_लिखो("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          मुक्त(chars);
        }
      }
      अन्यथा यदि(strstartswith(पंक्ति, "?screen_text ")) {
        प्रतिज्ञा(screen);
        वर्ण *linep = पंक्ति + 12;
        VTermRect rect;
        माप_प्रकार len;
        यावत्(linep[0] == ' ')
          linep++;
        यदि(माला_पूछो(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          म_लिखो("! screen_text unrecognised input\n");
          गच्छ abort_पंक्ति;
        }
        len = vterm_screen_get_text(screen, रिक्त, 0, rect);
        यदि(len == (माप_प्रकार)-1)
          म_लिखो("! screen_text error\n");
        अन्यथा यदि(len == 0)
          म_लिखो("\n");
        अन्यथा {
          // Put an overwrite guard at both ends of the buffer
          अचिन्हित वर्ण *buffer = दो_स्मृति(len + 4);
          अचिन्हित वर्ण *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (वर्ण *)text, len, rect);

          यदि(text[-2] != 0x55 || text[-1] != 0xAA)
            म_लिखो("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          अन्यथा यदि(text[len] != 0x55 || text[len+1] != 0xAA)
            म_लिखो("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          अन्यथा
	  {
	    माप_प्रकार i;
            प्रति(i = 0; i < len; i++) {
              म_लिखो("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          मुक्त(buffer);
        }
      }
      अन्यथा यदि(strstartswith(पंक्ति, "?screen_cell ")) {
        प्रतिज्ञा(screen);
        वर्ण *linep = पंक्ति + 12;
	पूर्णाङ्क i;
        VTermPos pos;
        VTermScreenCell cell;
        यावत्(linep[0] == ' ')
          linep++;
        यदि(माला_पूछो(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          म_लिखो("! screen_cell unrecognised input\n");
          गच्छ abort_पंक्ति;
        }
        यदि(!vterm_screen_get_cell(screen, pos, &cell))
          गच्छ abort_पंक्ति;
        म_लिखो("{");
        प्रति(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          म_लिखो("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        म_लिखो("} width=%d attrs={", cell.width);
        यदि(cell.attrs.bold)      म_लिखो("B");
        यदि(cell.attrs.underline) म_लिखो("U%d", cell.attrs.underline);
        यदि(cell.attrs.italic)    म_लिखो("I");
        यदि(cell.attrs.blink)     म_लिखो("K");
        यदि(cell.attrs.reverse)   म_लिखो("R");
        यदि(cell.attrs.font)      म_लिखो("F%d", cell.attrs.font);
        यदि(cell.attrs.small)     म_लिखो("S");
        यदि(cell.attrs.baseline)  म_लिखो(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        म_लिखो("} ");
        यदि(cell.attrs.dwl)       म_लिखो("dwl ");
        यदि(cell.attrs.dhl)       म_लिखो("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        म_लिखो("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        म_लिखो(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        म_लिखो("\n");
      }
      अन्यथा यदि(strstartswith(पंक्ति, "?screen_eol ")) {
        प्रतिज्ञा(screen);
        VTermPos pos;
        वर्ण *linep = पंक्ति + 12;
        यावत्(linep[0] == ' ')
          linep++;
        यदि(माला_पूछो(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          म_लिखो("! screen_eol unrecognised input\n");
          गच्छ abort_पंक्ति;
        }
        म_लिखो("%d\n", vterm_screen_is_eol(screen, pos));
      }
      अन्यथा यदि(strstartswith(पंक्ति, "?screen_attrs_extent ")) {
        प्रतिज्ञा(screen);
        VTermPos pos;
        VTermRect rect;
        वर्ण *linep = पंक्ति + 21;
        यावत्(linep[0] == ' ')
          linep++;
        यदि(माला_पूछो(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          म_लिखो("! screen_attrs_extent unrecognised input\n");
          गच्छ abort_पंक्ति;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        यदि(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          म_लिखो("! screen_attrs_extent failed\n");
          गच्छ abort_पंक्ति;
        }
        म_लिखो("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      अन्यथा
        म_लिखो("?\n");

      स_रखो(पंक्ति, 0, परिमाण पंक्ति);
      अनुवर्त;
    }

    अन्यथा
      abort_पंक्ति: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    यदि(outlen > 0) {
      वर्ण outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, रिक्त);
    }

    म_लिखो(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  प्रत्यागम 0;
}
