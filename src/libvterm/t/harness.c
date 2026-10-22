#अन्तर्भाव "vterm.h"
#अन्तर्भाव "../src/vterm_internal.h" // We pull in some internal bits too

#अन्तर्भाव <assert.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#define streq(a,b) (!म_भेद(a,b))
#define strstartswith(a,b) (!म_भेदन(a,b,म_माप(b)))

स्थिर माप_प्रकार inplace_hex2bytes(अक्षर *s)
{
  अक्षर *inpos = s, *outpos = s;

  पर्यंत(*inpos) {
    अचिन्हित पूर्णांक ch;
    जर(माला_पूछो(inpos, "%2x", &ch) < 1)
      थांबा;
    *outpos = ch;
    outpos += 1; inpos += 2;
  }

  परत outpos - s;
}

स्थिर VTermModifier strpe_modifiers(अक्षर **strp)
{
  VTermModifier state = 0;

  पर्यंत((*strp)[0]) {
    निवड(((*strp)++)[0]) {
      स्थिती 'S': state |= VTERM_MOD_SHIFT; थांबा;
      स्थिती 'C': state |= VTERM_MOD_CTRL;  थांबा;
      स्थिती 'A': state |= VTERM_MOD_ALT;   थांबा;
      पूर्वनिर्धारित: परत state;
    }
  }

  परत state;
}

स्थिर VTerm_किल्ली strp_किल्ली(अक्षर *str)
{
  स्थिर संरचना {
    अक्षर *name;
    VTerm_किल्ली किल्ली;
  } किल्ल्या[] = {
    { "Up",    VTERM_किल्ली_UP },
    { "Tab",   VTERM_किल्ली_TAB },
    { "Enter", VTERM_किल्ली_ENTER },
    { "KP0",   VTERM_किल्ली_KP_0 },
    { "F1",    VTERM_किल्ली_FUNCTION(1) },
    { रिक्त,    VTERM_किल्ली_NONE },
  };
  पूर्णांक i;

  साठी(i = 0; किल्ल्या[i].name; i++) {
    जर(streq(str, किल्ल्या[i].name))
      परत किल्ल्या[i].किल्ली;
  }

  परत VTERM_किल्ली_NONE;
}

स्थिर शून्य print_color(स्थिरांक VTermColor *col)
{
  जर (VTERM_COLOR_IS_RGB(col)) {
    म_लिखो("rgb(%d,%d,%d", col->red, col->green, col->blue);
  }
  अन्यथा जर (VTERM_COLOR_IS_INDEXED(col)) {
    म_लिखो("idx(%d", col->index);
  }
  अन्यथा {
    म_लिखो("invalid(%d", col->type);
  }
  जर (VTERM_COLOR_IS_DEFAULT_FG(col)) {
    म_लिखो(",is_default_fg");
  }
  जर (VTERM_COLOR_IS_DEFAULT_BG(col)) {
    म_लिखो(",is_default_bg");
  }
  म_लिखो(")");
}

स्थिर VTermColor strpe_color(अक्षर **strp)
{
  uint8_t r, g, b, idx;
  पूर्णांक len = 0;
  VTermColor col;

  जर(माला_पूछो(*strp, "rgb(%hhu,%hhu,%hhu)%n", &r, &g, &b, &len) == 3 && len > 0) {
    *strp += len;
    vterm_color_rgb(&col, r, g, b);
  }
  अन्यथा जर(माला_पूछो(*strp, "idx(%hhu)%n", &idx, &len) == 1 && len > 0) {
    *strp += len;
    vterm_color_indexed(&col, idx);
  }
  अन्यथा
    vterm_color_rgb(&col, 127, 127, 127);

  परत col;
}

स्थिर VTerm *vt;
स्थिर VTermState *state;
स्थिर VTermScreen *screen;

स्थिर VTermEncodingInstance encoding;

स्थिर शून्य term_output(स्थिरांक अक्षर *s, माप_प्रकार len, शून्य *user UNUSED)
{
  माप_प्रकार i;

  म_लिखो("output ");
  साठी(i = 0; i < len; i++)
    म_लिखो("%x%s", (अचिन्हित अक्षर)s[i], i < len-1 ? "," : "\n");
}

स्थिर शून्य printhex(स्थिरांक अक्षर *s, माप_प्रकार len)
{
  पर्यंत(len--)
    म_लिखो("%02x", (uint8_t)(s++)[0]);
}

स्थिर पूर्णांक parser_text(स्थिरांक अक्षर bytes[], माप_प्रकार len, शून्य *user UNUSED)
{
  माप_प्रकार i;

  म_लिखो("text ");
  साठी(i = 0; i < len; i++) {
    अचिन्हित अक्षर b = bytes[i];
    जर(b < 0x20 || b == 0x7f || (b >= 0x80 && b < 0xa0))
      थांबा;
    म_लिखो(i ? ",%x" : "%x", b);
  }
  म_लिखो("\n");

  परत i;
}

स्थिर पूर्णांक parser_control(अचिन्हित अक्षर control, शून्य *user UNUSED)
{
  म_लिखो("control %02x\n", control);

  परत 1;
}

स्थिर पूर्णांक parser_escape(स्थिरांक अक्षर bytes[], माप_प्रकार len, शून्य *user UNUSED)
{
  जर(bytes[0] >= 0x20 && bytes[0] < 0x30) {
    जर(len < 2)
      परत -1;
    len = 2;
  }
  अन्यथा {
    len = 1;
  }

  म_लिखो("escape ");
  printhex(bytes, len);
  म_लिखो("\n");

  परत len;
}

स्थिर पूर्णांक parser_csi(स्थिरांक अक्षर *leader, स्थिरांक दीर्घ args[], पूर्णांक argcount, स्थिरांक अक्षर *intermed, अक्षर command, शून्य *user UNUSED)
{
  पूर्णांक i;
  म_लिखो("csi %02x", command);

  जर(leader && leader[0]) {
    म_लिखो(" L=");
    साठी(i = 0; leader[i]; i++)
      म_लिखो("%02x", leader[i]);
  }

  साठी(i = 0; i < argcount; i++) {
    अक्षर sep = i ? ',' : ' ';

    जर(args[i] == CSI_ARG_MISSING)
      म_लिखो("%c*", sep);
    अन्यथा
      म_लिखो("%c%ld%s", sep, CSI_ARG(args[i]), CSI_ARG_HAS_MORE(args[i]) ? "+" : "");
  }

  जर(intermed && intermed[0]) {
    म_लिखो(" I=");
    साठी(i = 0; intermed[i]; i++)
      म_लिखो("%02x", intermed[i]);
  }

  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक parser_osc(पूर्णांक command, VTermStringFragment frag, शून्य *user UNUSED)
{

  म_लिखो("osc ");

  जर(frag.initial) {
    जर(command == -1)
      म_लिखो("[");
    अन्यथा
      म_लिखो("[%d;", command);
  }

  printhex(frag.str, frag.len);

  जर(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक parser_dcs(स्थिरांक अक्षर *command, माप_प्रकार commandlen, VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("dcs ");

  जर(frag.initial) {
    माप_प्रकार i;
    म_लिखो("[");
    साठी(i = 0; i < commandlen; i++)
      म_लिखो("%02x", command[i]);
  }

  printhex(frag.str, frag.len);

  जर(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक parser_apc(VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("apc ");

  जर(frag.initial)
    म_लिखो("[");

  printhex(frag.str, frag.len);

  जर(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक parser_pm(VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("pm ");

  जर(frag.initial)
    म_लिखो("[");

  printhex(frag.str, frag.len);

  जर(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक parser_sos(VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("sos ");

  जर(frag.initial)
    म_लिखो("[");

  printhex(frag.str, frag.len);

  जर(frag.final)
    म_लिखो("]");

  म_लिखो("\n");

  परत 1;
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

स्थिर पूर्णांक want_movecursor = 0;
स्थिर VTermPos state_pos;
स्थिर पूर्णांक movecursor(VTermPos pos, VTermPos oldpos UNUSED, पूर्णांक visible UNUSED, शून्य *user UNUSED)
{
  state_pos = pos;

  जर(want_movecursor)
    म_लिखो("movecursor %d,%d\n", pos.row, pos.col);

  परत 1;
}

स्थिर पूर्णांक want_premove = 0;
स्थिर पूर्णांक premove(VTermRect rect, शून्य *user UNUSED)
{
  जर(!want_premove)
    परत 0;

  म_लिखो("premove %d..%d,%d..%d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  परत 1;
}

स्थिर पूर्णांक want_scrollrect = 0;
स्थिर पूर्णांक scrollrect(VTermRect rect, पूर्णांक downward, पूर्णांक rightward, शून्य *user UNUSED)
{
  जर(!want_scrollrect)
    परत 0;

  म_लिखो("scrollrect %d..%d,%d..%d => %+d,%+d\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      downward, rightward);

  परत 1;
}

स्थिर पूर्णांक want_moverect = 0;
स्थिर पूर्णांक moverect(VTermRect dest, VTermRect src, शून्य *user UNUSED)
{
  जर(!want_moverect)
    परत 0;

  म_लिखो("moverect %d..%d,%d..%d -> %d..%d,%d..%d\n",
      src.start_row,  src.end_row,  src.start_col,  src.end_col,
      dest.start_row, dest.end_row, dest.start_col, dest.end_col);

  परत 1;
}

स्थिर पूर्णांक want_settermprop = 0;
स्थिर पूर्णांक settermprop(VTermProp prop, VTerm_मूल्य *val, शून्य *user UNUSED)
{
  VTerm_मूल्य_Type type;
  जर(!want_settermprop)
    परत 1;

  type = vterm_get_prop_type(prop);
  निवड(type) {
  स्थिती VTERM_VALUETYPE_BOOL:
    म_लिखो("settermprop %d %s\n", prop, val->boolean ? "true" : "false");
    परत 1;
  स्थिती VTERM_VALUETYPE_INT:
    म_लिखो("settermprop %d %d\n", prop, val->संख्या);
    परत 1;
  स्थिती VTERM_VALUETYPE_STRING:
    म_लिखो("settermprop %d %s\"%.*s\"%s\n", prop,
        val->string.initial ? "[" : "", val->string.len, val->string.str, val->string.final ? "]" : "");
    परत 1;
  स्थिती VTERM_VALUETYPE_COLOR:
    म_लिखो("settermprop %d ", prop);
    print_color(&val->color);
    म_लिखो("\n");
    परत 1;

  स्थिती VTERM_N_VALUETYPES:
    परत 0;
  }

  परत 0;
}

// These callbacks are for State

स्थिर पूर्णांक want_state_putglyph = 0;
स्थिर पूर्णांक state_putglyph(VTermGlyphInfo *info, VTermPos pos, शून्य *user UNUSED)
{
  पूर्णांक i;
  जर(!want_state_putglyph)
    परत 1;

  म_लिखो("putglyph ");
  साठी(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++)
    म_लिखो(i ? ",%x" : "%x", info->chars[i]);
  म_लिखो(" %d %d,%d", info->width, pos.row, pos.col);
  जर(info->protected_cell)
    म_लिखो(" prot");
  जर(info->dwl)
    म_लिखो(" dwl");
  जर(info->dhl)
    म_लिखो(" dhl-%s", info->dhl == 1 ? "top" : info->dhl == 2 ? "bottom" : "?" );
  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक want_state_erase = 0;
स्थिर पूर्णांक state_erase(VTermRect rect, पूर्णांक selective, शून्य *user UNUSED)
{
  जर(!want_state_erase)
    परत 1;

  म_लिखो("erase %d..%d,%d..%d%s\n",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col,
      selective ? " selective" : "");

  परत 1;
}

स्थिर संरचना {
  पूर्णांक bold;
  पूर्णांक underline;
  पूर्णांक italic;
  पूर्णांक blink;
  पूर्णांक reverse;
  पूर्णांक conceal;
  पूर्णांक strike;
  पूर्णांक font;
  पूर्णांक small;
  पूर्णांक baseline;
  VTermColor foreground;
  VTermColor background;
} state_pen;
स्थिर पूर्णांक state_setpenattr(VTermAttr attr, VTerm_मूल्य *val, शून्य *user UNUSED)
{
  निवड(attr) {
  स्थिती VTERM_ATTR_BOLD:
    state_pen.bold = val->boolean;
    थांबा;
  स्थिती VTERM_ATTR_UNDERLINE:
    state_pen.underline = val->संख्या;
    थांबा;
  स्थिती VTERM_ATTR_ITALIC:
    state_pen.italic = val->boolean;
    थांबा;
  स्थिती VTERM_ATTR_BLINK:
    state_pen.blink = val->boolean;
    थांबा;
  स्थिती VTERM_ATTR_REVERSE:
    state_pen.reverse = val->boolean;
    थांबा;
  स्थिती VTERM_ATTR_CONCEAL:
    state_pen.conceal = val->boolean;
    थांबा;
  स्थिती VTERM_ATTR_STRIKE:
    state_pen.strike = val->boolean;
    थांबा;
  स्थिती VTERM_ATTR_FONT:
    state_pen.font = val->संख्या;
    थांबा;
  स्थिती VTERM_ATTR_SMALL:
    state_pen.small = val->boolean;
    थांबा;
  स्थिती VTERM_ATTR_BASELINE:
    state_pen.baseline = val->संख्या;
    थांबा;
  स्थिती VTERM_ATTR_FOREGROUND:
    state_pen.foreground = val->color;
    थांबा;
  स्थिती VTERM_ATTR_BACKGROUND:
    state_pen.background = val->color;
    थांबा;

  स्थिती VTERM_N_ATTRS:
    परत 0;
  }

  परत 1;
}

स्थिर पूर्णांक state_setlineinfo(पूर्णांक row UNUSED, स्थिरांक VTerm_ओळ_Info *newinfo UNUSED, स्थिरांक VTerm_ओळ_Info *oldinfo UNUSED, शून्य *user UNUSED)
{
  परत 1;
}

स्थिर पूर्णांक want_state_scrollback = 0;
स्थिर पूर्णांक state_sb_clear(शून्य *user UNUSED) {
  जर(!want_state_scrollback)
    परत 1;

  म_लिखो("sb_clear\n");
  परत 0;
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

स्थिर पूर्णांक selection_set(VTermSelectionMask mask, VTermStringFragment frag, शून्य *user UNUSED)
{
  म_लिखो("selection-set mask=%04X ", mask);
  जर(frag.initial)
    म_लिखो("[");
  printhex(frag.str, frag.len);
  जर(frag.final)
    म_लिखो("]");
  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक selection_query(VTermSelectionMask mask, शून्य *user UNUSED)
{
  म_लिखो("selection-query mask=%04X\n", mask);

  परत 1;
}

VTermSelectionCallbacks selection_cbs = {
  .set   = selection_set,
  .query = selection_query,
};

स्थिर पूर्णांक want_screen_damage = 0;
स्थिर पूर्णांक want_screen_damage_cells = 0;
स्थिर पूर्णांक screen_damage(VTermRect rect, शून्य *user UNUSED)
{
  जर(!want_screen_damage)
    परत 1;

  म_लिखो("damage %d..%d,%d..%d",
      rect.start_row, rect.end_row, rect.start_col, rect.end_col);

  जर(want_screen_damage_cells) {
    पूर्णांक equals = FALSE;
    पूर्णांक row;
    पूर्णांक col;

    साठी(row = rect.start_row; row < rect.end_row; row++) {
      पूर्णांक eol = rect.end_col;
      पर्यंत(eol > rect.start_col) {
        VTermScreenCell cell;
	VTermPos pos;
	pos.row = row;
	pos.col = eol-1;
        vterm_screen_get_cell(screen, pos, &cell);
        जर(cell.chars[0])
          थांबा;

        eol--;
      }

      जर(eol == rect.start_col)
        थांबा;

      जर(!equals)
        म_लिखो(" ="), equals = TRUE;

      म_लिखो(" %d<", row);
      साठी(col = rect.start_col; col < eol; col++) {
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

  परत 1;
}

स्थिर पूर्णांक want_screen_scrollback = 0;
स्थिर पूर्णांक screen_sb_pushline4(पूर्णांक cols, स्थिरांक VTermScreenCell *cells, पूर्णांक continuation, शून्य *user UNUSED)
{
  पूर्णांक eol;
  पूर्णांक c;

  जर(!want_screen_scrollback)
    परत 1;

  eol = cols;
  पर्यंत(eol && !cells[eol-1].chars[0])
    eol--;

  म_लिखो("sb_pushline %d%s =", cols, continuation ? " cont" : "");
  साठी(c = 0; c < eol; c++)
    म_लिखो(" %02X", cells[c].chars[0]);
  म_लिखो("\n");

  परत 1;
}

स्थिर पूर्णांक screen_sb_popline(पूर्णांक cols, VTermScreenCell *cells, शून्य *user UNUSED)
{
  पूर्णांक col;

  जर(!want_screen_scrollback)
    परत 0;

  // All lines of scrollback contain "ABCDE"
  साठी(col = 0; col < cols; col++) {
    जर(col < 5)
      cells[col].chars[0] = 'A' + col;
    अन्यथा
      cells[col].chars[0] = 0;

    cells[col].width = 1;
  }

  म_लिखो("sb_popline %d\n", cols);
  परत 1;
}

स्थिर पूर्णांक screen_sb_clear(शून्य *user UNUSED)
{
  जर(!want_screen_scrollback)
    परत 1;

  म_लिखो("sb_clear\n");
  परत 0;
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

पूर्णांक main(पूर्णांक argc UNUSED, अक्षर **argv UNUSED)
{
  अक्षर ओळ[1024] = {0};
  पूर्णांक flag;

  पूर्णांक err;

  रखो_भबफ(मानक_निकास, रिक्त, _IONBF, 0);

  पर्यंत(ख_माला_लो(ओळ, आकार ओळ, मानक_निवेश)) {
    अक्षर *nl;
    माप_प्रकार outlen;
    err = 0;

    जर((nl = म_अक्षर(ओळ, '\n')))
      *nl = '\0';

    जर(streq(ओळ, "INIT")) {
      जर(!vt)
        vt = vterm_new(25, 80);

      // Somehow this makes tests fail
      // vterm_output_set_callback(vt, term_output, NULL);
    }

    अन्यथा जर(streq(ओळ, "WANTPARSER")) {
      प्रतिज्ञा(vt);
      vterm_parser_set_callbacks(vt, &parser_cbs, रिक्त);
    }

    अन्यथा जर(strstartswith(ओळ, "WANTSTATE") && (ओळ[9] == '\0' || ओळ[9] == ' ')) {
      पूर्णांक i = 9;
      पूर्णांक sense = 1;
      प्रतिज्ञा(vt);
      जर(!state) {
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

      पर्यंत(ओळ[i] == ' ')
        i++;
      साठी( ; ओळ[i]; i++)
        निवड(ओळ[i]) {
        स्थिती '+':
          sense = 1;
          थांबा;
        स्थिती '-':
          sense = 0;
          थांबा;
        स्थिती 'g':
          want_state_putglyph = sense;
          थांबा;
        स्थिती 's':
          want_scrollrect = sense;
          थांबा;
        स्थिती 'P':
          want_premove = sense;
          थांबा;
        स्थिती 'm':
          want_moverect = sense;
          थांबा;
        स्थिती 'e':
          want_state_erase = sense;
          थांबा;
        स्थिती 'p':
          want_settermprop = sense;
          थांबा;
        स्थिती 'f':
          vterm_state_set_unrecognised_fallbacks(state, sense ? &fallbacks : रिक्त, रिक्त);
          थांबा;
        स्थिती 'b':
          want_state_scrollback = sense;
          थांबा;
        पूर्वनिर्धारित:
          ख_लिखो(मानक_त्रुटि, "Unrecognised WANTSTATE flag '%c'\n", ओळ[i]);
        }
    }

    अन्यथा जर(strstartswith(ओळ, "WANTSCREEN") && (ओळ[10] == '\0' || ओळ[10] == ' ')) {
      पूर्णांक i = 10;
      पूर्णांक sense = 1;
      प्रतिज्ञा(vt);
      जर(!screen)
        screen = vterm_obtain_screen(vt);
      vterm_screen_set_callbacks(screen, &screen_cbs, रिक्त);
      vterm_screen_callbacks_has_pushline4(screen);

      पर्यंत(ओळ[i] == ' ')
        i++;
      साठी( ; ओळ[i]; i++)
        निवड(ओळ[i]) {
        स्थिती '-':
          sense = 0;
          थांबा;
        स्थिती 'a':
          vterm_screen_enable_altscreen(screen, 1);
          थांबा;
        स्थिती 'd':
          want_screen_damage = sense;
          थांबा;
        स्थिती 'D':
          want_screen_damage = sense;
          want_screen_damage_cells = sense;
          थांबा;
        स्थिती 'm':
          want_moverect = sense;
          थांबा;
        स्थिती 'c':
          want_movecursor = sense;
          थांबा;
        स्थिती 'p':
          want_settermprop = 1;
          थांबा;
        स्थिती 'b':
          want_screen_scrollback = sense;
          थांबा;
        स्थिती 'r':
          vterm_screen_enable_reflow(screen, sense);
          थांबा;
        पूर्वनिर्धारित:
          ख_लिखो(मानक_त्रुटि, "Unrecognised WANTSCREEN flag '%c'\n", ओळ[i]);
        }
    }

    अन्यथा जर(माला_पूछो(ओळ, "UTF8 %d", &flag)) {
      vterm_set_utf8(vt, flag);
    }

    अन्यथा जर(streq(ओळ, "RESET")) {
      जर(state) {
        vterm_state_reset(state, 1);
        vterm_state_get_cursorpos(state, &state_pos);
      }
      जर(screen) {
        vterm_screen_reset(screen, 1);
      }
    }

    अन्यथा जर(strstartswith(ओळ, "RESIZE ")) {
      पूर्णांक rows, cols;
      अक्षर *linep = ओळ + 7;
      पर्यंत(linep[0] == ' ')
        linep++;
      माला_पूछो(linep, "%d, %d", &rows, &cols);
      vterm_set_size(vt, rows, cols);
    }

    अन्यथा जर(strstartswith(ओळ, "PUSH ")) {
      अक्षर *bytes = ओळ + 5;
      माप_प्रकार len = inplace_hex2bytes(bytes);
      प्रतिज्ञा(len);

      माप_प्रकार written = vterm_आदान_write(vt, bytes, len);
      जर(written < len)
        ख_लिखो(मानक_त्रुटि, "! short write\n");
    }

    अन्यथा जर(streq(ओळ, "WANTENCODING")) {
      // This isn't really external API but it's hard to get this out any
      // other way
      encoding.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
      जर(encoding.enc->init)
        (*encoding.enc->init)(encoding.enc, encoding.data);
    }

    अन्यथा जर(strstartswith(ओळ, "ENCIN ")) {
      अक्षर *bytes = ओळ + 6;
      माप_प्रकार len = inplace_hex2bytes(bytes);
      प्रतिज्ञा(len);

      uint32_t cp[1024];
      पूर्णांक cpi = 0;
      माप_प्रकार pos = 0;

      (*encoding.enc->decode)(encoding.enc, encoding.data,
          cp, &cpi, len, bytes, &pos, len);

      जर(cpi > 0) {
	पूर्णांक i;
        म_लिखो("encout ");
        साठी(i = 0; i < cpi; i++) {
          म_लिखो(i ? ",%x" : "%x", cp[i]);
        }
        म_लिखो("\n");
      }
    }

    अन्यथा जर(strstartswith(ओळ, "INCHAR ")) {
      अक्षर *linep = ओळ + 7;
      अचिन्हित पूर्णांक c = 0;
      VTermModifier mod;
      पर्यंत(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      माला_पूछो(linep, " %x", &c);

      vterm_keyboard_unichar(vt, c, mod);
    }

    अन्यथा जर(strstartswith(ओळ, "INKEY ")) {
      VTermModifier mod;
      VTerm_किल्ली किल्ली;
      अक्षर *linep = ओळ + 6;
      पर्यंत(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      पर्यंत(linep[0] == ' ')
        linep++;
      किल्ली = strp_किल्ली(linep);

      vterm_keyboard_किल्ली(vt, किल्ली, mod);
    }

    अन्यथा जर(strstartswith(ओळ, "PASTE ")) {
      अक्षर *linep = ओळ + 6;
      जर(streq(linep, "START"))
        vterm_keyboard_start_paste(vt);
      अन्यथा जर(streq(linep, "END"))
        vterm_keyboard_end_paste(vt);
      अन्यथा
        जा abort_ओळ;
    }

    अन्यथा जर(strstartswith(ओळ, "FOCUS ")) {
      प्रतिज्ञा(state);
      अक्षर *linep = ओळ + 6;
      जर(streq(linep, "IN"))
        vterm_state_focus_in(state);
      अन्यथा जर(streq(linep, "OUT"))
        vterm_state_focus_out(state);
      अन्यथा
        जा abort_ओळ;
    }

    अन्यथा जर(strstartswith(ओळ, "MOUSEMOVE ")) {
      अक्षर *linep = ओळ + 10;
      पूर्णांक row, col, len;
      VTermModifier mod;
      पर्यंत(linep[0] == ' ')
        linep++;
      माला_पूछो(linep, "%d,%d%n", &row, &col, &len);
      linep += len;
      पर्यंत(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_move(vt, row, col, mod);
    }

    अन्यथा जर(strstartswith(ओळ, "MOUSEBTN ")) {
      अक्षर *linep = ओळ + 9;
      अक्षर press;
      पूर्णांक button, len;
      VTermModifier mod;
      पर्यंत(linep[0] == ' ')
        linep++;
      माला_पूछो(linep, "%c %d%n", &press, &button, &len);
      linep += len;
      पर्यंत(linep[0] == ' ')
        linep++;
      mod = strpe_modifiers(&linep);
      vterm_mouse_button(vt, button, (press == 'd' || press == 'D'), mod);
    }

    अन्यथा जर(strstartswith(ओळ, "SELECTION ")) {
      अक्षर *linep = ओळ + 10;
      अचिन्हित पूर्णांक mask;
      पूर्णांक len;
      VTermStringFragment frag = { 0 };
      माला_पूछो(linep, "%x%n", &mask, &len);
      linep += len;
      पर्यंत(linep[0] == ' ')
        linep++;
      जर(linep[0] == '[') {
        frag.initial = TRUE;
        linep++;
        पर्यंत(linep[0] == ' ')
          linep++;
      }
      frag.len = inplace_hex2bytes(linep);
      frag.str = linep;
      प्रतिज्ञा(frag.len);

      linep += frag.len * 2;
      पर्यंत(linep[0] == ' ')
        linep++;
      जर(linep[0] == ']') {
        frag.final = TRUE;
      }
      vterm_state_send_selection(state, mask, frag);
    }

    अन्यथा जर(strstartswith(ओळ, "DAMAGEMERGE ")) {
      प्रतिज्ञा(screen);
      अक्षर *linep = ओळ + 12;
      पर्यंत(linep[0] == ' ')
        linep++;
      जर(streq(linep, "CELL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_CELL);
      अन्यथा जर(streq(linep, "ROW"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_ROW);
      अन्यथा जर(streq(linep, "SCREEN"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCREEN);
      अन्यथा जर(streq(linep, "SCROLL"))
        vterm_screen_set_damage_merge(screen, VTERM_DAMAGE_SCROLL);
    }

    अन्यथा जर(strstartswith(ओळ, "DAMAGEFLUSH")) {
      प्रतिज्ञा(screen);
      vterm_screen_flush_damage(screen);
    }

    अन्यथा जर(strstartswith(ओळ, "SETDEFAULTCOL ")) {
      प्रतिज्ञा(screen);
      अक्षर *linep = ओळ + 14;
      पर्यंत(linep[0] == ' ')
        linep++;
      VTermColor fg = strpe_color(&linep);
      जर(linep[0]) {
        पर्यंत(linep[0] == ' ')
          linep++;
        VTermColor bg = strpe_color(&linep);

        vterm_screen_set_default_colors(screen, &fg, &bg);
      }
      अन्यथा
        vterm_screen_set_default_colors(screen, &fg, रिक्त);
    }

    अन्यथा जर(ओळ[0] == '?') {
      जर(streq(ओळ, "?cursor")) {
        प्रतिज्ञा(state);
        VTermPos pos;
        vterm_state_get_cursorpos(state, &pos);
        जर(pos.row != state_pos.row)
          म_लिखो("! row mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        अन्यथा जर(pos.col != state_pos.col)
          म_लिखो("! col mismatch: state=%d,%d event=%d,%d\n",
              pos.row, pos.col, state_pos.row, state_pos.col);
        अन्यथा
          म_लिखो("%d,%d\n", state_pos.row, state_pos.col);
      }
      अन्यथा जर(strstartswith(ओळ, "?pen ")) {
        प्रतिज्ञा(state);
        VTerm_मूल्य val;
        अक्षर *linep = ओळ + 5;
        पर्यंत(linep[0] == ' ')
          linep++;

#define BOOLSTR(v) ((v) ? "on" : "off")

        जर(streq(linep, "bold")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BOLD, &val);
          जर(val.boolean != state_pen.bold)
            म_लिखो("! pen bold mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.bold));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.bold));
        }
        अन्यथा जर(streq(linep, "underline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_UNDERLINE, &val);
          जर(val.boolean != state_pen.underline)
            म_लिखो("! pen underline mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.underline);
          अन्यथा
            म_लिखो("%d\n", state_pen.underline);
        }
        अन्यथा जर(streq(linep, "italic")) {
          vterm_state_get_penattr(state, VTERM_ATTR_ITALIC, &val);
          जर(val.boolean != state_pen.italic)
            म_लिखो("! pen italic mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.italic));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.italic));
        }
        अन्यथा जर(streq(linep, "blink")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BLINK, &val);
          जर(val.boolean != state_pen.blink)
            म_लिखो("! pen blink mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.blink));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.blink));
        }
        अन्यथा जर(streq(linep, "reverse")) {
          vterm_state_get_penattr(state, VTERM_ATTR_REVERSE, &val);
          जर(val.boolean != state_pen.reverse)
            म_लिखो("! pen reverse mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.reverse));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.reverse));
        }
        अन्यथा जर(streq(linep, "font")) {
          vterm_state_get_penattr(state, VTERM_ATTR_FONT, &val);
          जर(val.boolean != state_pen.font)
            म_लिखो("! pen font mismatch; state=%d, event=%d\n",
                val.boolean, state_pen.font);
          अन्यथा
            म_लिखो("%d\n", state_pen.font);
        }
        अन्यथा जर(streq(linep, "small")) {
          vterm_state_get_penattr(state, VTERM_ATTR_SMALL, &val);
          जर(val.boolean != state_pen.small)
            म_लिखो("! pen small mismatch; state=%s, event=%s\n",
                BOOLSTR(val.boolean), BOOLSTR(state_pen.small));
          अन्यथा
            म_लिखो("%s\n", BOOLSTR(state_pen.small));
        }
        अन्यथा जर(streq(linep, "baseline")) {
          vterm_state_get_penattr(state, VTERM_ATTR_BASELINE, &val);
          जर(val.संख्या != state_pen.baseline)
            म_लिखो("! pen baseline mismatch: state=%d, event=%d\n",
                val.संख्या, state_pen.baseline);
          अन्यथा
            म_लिखो("%s\n", state_pen.baseline == VTERM_BASELINE_RAISE ? "raise"
                         : state_pen.baseline == VTERM_BASELINE_LOWER ? "lower"
                         : "normal");
        }
        अन्यथा जर(streq(linep, "foreground")) {
          print_color(&state_pen.foreground);
          म_लिखो("\n");
        }
        अन्यथा जर(streq(linep, "background")) {
          print_color(&state_pen.background);
          म_लिखो("\n");
        }
        अन्यथा
          म_लिखो("?\n");
      }
      अन्यथा जर(strstartswith(ओळ, "?lineinfo ")) {
        प्रतिज्ञा(state);
        अक्षर *linep = ओळ + 10;
        पूर्णांक row;
        स्थिरांक VTerm_ओळ_Info *info;
        पर्यंत(linep[0] == ' ')
          linep++;
        जर(माला_पूछो(linep, "%d", &row) < 1) {
          म_लिखो("! lineinfo unrecognised input\n");
          जा abort_ओळ;
        }
        info = vterm_state_get_lineinfo(state, row);
        जर(info->doublewidth)
          म_लिखो("dwl ");
        जर(info->doubleheight)
          म_लिखो("dhl ");
        जर(info->continuation)
          म_लिखो("cont ");
        म_लिखो("\n");
      }
      अन्यथा जर(strstartswith(ओळ, "?screen_chars ")) {
        प्रतिज्ञा(screen);
        अक्षर *linep = ओळ + 13;
        VTermRect rect;
        माप_प्रकार len;
        पर्यंत(linep[0] == ' ')
          linep++;
        जर(माला_पूछो(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) == 4)
          ; // fine
        अन्यथा जर(माला_पूछो(linep, "%d", &rect.start_row) == 1) {
          rect.end_row = rect.start_row + 1;
          rect.start_col = 0;
          vterm_get_size(vt, रिक्त, &rect.end_col);
        }
        अन्यथा {
          म_लिखो("! screen_chars unrecognised input\n");
          जा abort_ओळ;
        }
        len = vterm_screen_get_chars(screen, रिक्त, 0, rect);
        जर(len == (माप_प्रकार)-1)
          म_लिखो("! screen_chars error\n");
        अन्यथा जर(len == 0)
          म_लिखो("\n");
        अन्यथा {
          uint32_t *chars = दो_स्मृति(आकार(uint32_t) * len);
          माप_प्रकार i;
          vterm_screen_get_chars(screen, chars, len, rect);
          साठी(i = 0; i < len; i++) {
            म_लिखो("0x%02x%s", chars[i], i < len-1 ? "," : "\n");
          }
          मुक्त(chars);
        }
      }
      अन्यथा जर(strstartswith(ओळ, "?screen_text ")) {
        प्रतिज्ञा(screen);
        अक्षर *linep = ओळ + 12;
        VTermRect rect;
        माप_प्रकार len;
        पर्यंत(linep[0] == ' ')
          linep++;
        जर(माला_पूछो(linep, "%d,%d,%d,%d", &rect.start_row, &rect.start_col, &rect.end_row, &rect.end_col) < 4) {
          म_लिखो("! screen_text unrecognised input\n");
          जा abort_ओळ;
        }
        len = vterm_screen_get_text(screen, रिक्त, 0, rect);
        जर(len == (माप_प्रकार)-1)
          म_लिखो("! screen_text error\n");
        अन्यथा जर(len == 0)
          म_लिखो("\n");
        अन्यथा {
          // Put an overwrite guard at both ends of the buffer
          अचिन्हित अक्षर *buffer = दो_स्मृति(len + 4);
          अचिन्हित अक्षर *text = buffer + 2;
          text[-2] = 0x55; text[-1] = 0xAA;
          text[len] = 0x55; text[len+1] = 0xAA;

          vterm_screen_get_text(screen, (अक्षर *)text, len, rect);

          जर(text[-2] != 0x55 || text[-1] != 0xAA)
            म_लिखो("! screen_get_text buffer overrun left [%02x,%02x]\n", text[-2], text[-1]);
          अन्यथा जर(text[len] != 0x55 || text[len+1] != 0xAA)
            म_लिखो("! screen_get_text buffer overrun right [%02x,%02x]\n", text[len], text[len+1]);
          अन्यथा
	  {
	    माप_प्रकार i;
            साठी(i = 0; i < len; i++) {
              म_लिखो("0x%02x%s", text[i], i < len-1 ? "," : "\n");
            }
	  }

          मुक्त(buffer);
        }
      }
      अन्यथा जर(strstartswith(ओळ, "?screen_cell ")) {
        प्रतिज्ञा(screen);
        अक्षर *linep = ओळ + 12;
	पूर्णांक i;
        VTermPos pos;
        VTermScreenCell cell;
        पर्यंत(linep[0] == ' ')
          linep++;
        जर(माला_पूछो(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          म_लिखो("! screen_cell unrecognised input\n");
          जा abort_ओळ;
        }
        जर(!vterm_screen_get_cell(screen, pos, &cell))
          जा abort_ओळ;
        म_लिखो("{");
        साठी(i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell.chars[i]; i++) {
          म_लिखो("%s0x%x", i ? "," : "", cell.chars[i]);
        }
        म_लिखो("} width=%d attrs={", cell.width);
        जर(cell.attrs.bold)      म_लिखो("B");
        जर(cell.attrs.underline) म_लिखो("U%d", cell.attrs.underline);
        जर(cell.attrs.italic)    म_लिखो("I");
        जर(cell.attrs.blink)     म_लिखो("K");
        जर(cell.attrs.reverse)   म_लिखो("R");
        जर(cell.attrs.font)      म_लिखो("F%d", cell.attrs.font);
        जर(cell.attrs.small)     म_लिखो("S");
        जर(cell.attrs.baseline)  म_लिखो(
            cell.attrs.baseline == VTERM_BASELINE_RAISE ? "^" :
                                                          "_");
        म_लिखो("} ");
        जर(cell.attrs.dwl)       म_लिखो("dwl ");
        जर(cell.attrs.dhl)       म_लिखो("dhl-%s ", cell.attrs.dhl == 2 ? "bottom" : "top");
        म_लिखो("fg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.fg);
        print_color(&cell.fg);
        म_लिखो(" bg=");
        vterm_screen_convert_color_to_rgb(screen, &cell.bg);
        print_color(&cell.bg);
        म_लिखो("\n");
      }
      अन्यथा जर(strstartswith(ओळ, "?screen_eol ")) {
        प्रतिज्ञा(screen);
        VTermPos pos;
        अक्षर *linep = ओळ + 12;
        पर्यंत(linep[0] == ' ')
          linep++;
        जर(माला_पूछो(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          म_लिखो("! screen_eol unrecognised input\n");
          जा abort_ओळ;
        }
        म_लिखो("%d\n", vterm_screen_is_eol(screen, pos));
      }
      अन्यथा जर(strstartswith(ओळ, "?screen_attrs_extent ")) {
        प्रतिज्ञा(screen);
        VTermPos pos;
        VTermRect rect;
        अक्षर *linep = ओळ + 21;
        पर्यंत(linep[0] == ' ')
          linep++;
        जर(माला_पूछो(linep, "%d,%d\n", &pos.row, &pos.col) < 2) {
          म_लिखो("! screen_attrs_extent unrecognised input\n");
          जा abort_ओळ;
        }
	rect.start_col = 0;
	rect.end_col   = -1;
        जर(!vterm_screen_get_attrs_extent(screen, &rect, pos, ~0)) {
          म_लिखो("! screen_attrs_extent failed\n");
          जा abort_ओळ;
        }
        म_लिखो("%d,%d-%d,%d\n", rect.start_row, rect.start_col, rect.end_row, rect.end_col);
      }
      अन्यथा
        म_लिखो("?\n");

      स_रखो(ओळ, 0, आकार ओळ);
      सुरूठेवा;
    }

    अन्यथा
      abort_ओळ: err = 1;

    outlen = vterm_output_get_buffer_current(vt);
    जर(outlen > 0) {
      अक्षर outbuff[1024];
      vterm_output_read(vt, outbuff, outlen);

      term_output(outbuff, outlen, रिक्त);
    }

    म_लिखो(err ? "?\n" : "DONE\n");
  }

  vterm_free(vt);

  परत 0;
}
