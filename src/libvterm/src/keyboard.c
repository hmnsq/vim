#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "utf8.h"

// VIM: added
पूर्णांक vterm_is_modify_other_किल्ल्या(VTerm *vt)
{
  परत vt->state->mode.modify_other_किल्ल्या;
}

// VIM: added
पूर्णांक vterm_is_kitty_keyboard(VTerm *vt)
{
  परत vt->state->mode.kitty_keyboard;
}


शून्य vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  जर (vterm_is_modify_other_किल्ल्या(vt) && mod != 0) {
    vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    परत;
  }

  // VIM: added kitty keyboard protocol support
  जर (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    परत;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  जर(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  जर(mod == 0) {
    // Normal text - ignore just shift
    अक्षर str[6];
    पूर्णांक seqlen = fill_utf8(c, str);
    vterm_ढकला_output_bytes(vt, str, seqlen);
    परत;
  }

  पूर्णांक needs_CSIu;
  निवड(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    स्थिती 'i': स्थिती 'j': स्थिती 'm': स्थिती '[':
      needs_CSIu = 1;
      थांबा;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    स्थिती '\\': स्थिती ']': स्थिती '^': स्थिती '_':
      needs_CSIu = 0;
      थांबा;
    /* Shift-space needs CSIu */
    स्थिती ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      थांबा;
    /* All other characters needs CSIu except for letters a-z */
    पूर्वनिर्धारित:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  जर(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    परत;
  }

  जर(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ढकला_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

प्रकारव्याख्या संरचना {
  गणना {
    KEYCODE_NONE,
    KEYCODE_LITERAL,
    KEYCODE_TAB,
    KEYCODE_ENTER,
    KEYCODE_SS3,
    KEYCODE_CSI,
    KEYCODE_CSI_CURSOR,
    KEYCODE_CSINUM,
    KEYCODE_KEYPAD,
  } type;
  अक्षर literal;
  पूर्णांक csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
स्थिर keycodes_s keycodes[] = {
  { KEYCODE_NONE,       0, 0 }, // NONE

  { KEYCODE_ENTER,      '\r', 0 }, // ENTER
  { KEYCODE_TAB,        '\t',  0 }, // TAB
  { KEYCODE_LITERAL,    '\x7f', 0 }, // BACKSPACE == ASCII DEL
  { KEYCODE_LITERAL,    '\x1b', 0 }, // ESCAPE

  { KEYCODE_CSI_CURSOR, 'A', 0 }, // UP
  { KEYCODE_CSI_CURSOR, 'B', 0 }, // DOWN
  { KEYCODE_CSI_CURSOR, 'D', 0 }, // LEFT
  { KEYCODE_CSI_CURSOR, 'C', 0 }, // RIGHT

  { KEYCODE_CSINUM, '~', 2 },  // INS
  { KEYCODE_CSINUM, '~', 3 },  // DEL
  { KEYCODE_CSI_CURSOR, 'H', 0 }, // HOME
  { KEYCODE_CSI_CURSOR, 'F', 0 }, // END
  { KEYCODE_CSINUM, '~', 5 },  // PAGEUP
  { KEYCODE_CSINUM, '~', 6 },  // PAGEDOWN
};

स्थिर keycodes_s keycodes_fn[] = {
  { KEYCODE_NONE,       0, 0 },   // F0 - shouldn't happen
  { KEYCODE_SS3,	'P', 0 }, // F1
  { KEYCODE_SS3,	'Q', 0 }, // F2
  { KEYCODE_SS3,	'R', 0 }, // F3
  { KEYCODE_SS3,	'S', 0 }, // F4
  { KEYCODE_CSINUM, '~', 15 }, // F5
  { KEYCODE_CSINUM, '~', 17 }, // F6
  { KEYCODE_CSINUM, '~', 18 }, // F7
  { KEYCODE_CSINUM, '~', 19 }, // F8
  { KEYCODE_CSINUM, '~', 20 }, // F9
  { KEYCODE_CSINUM, '~', 21 }, // F10
  { KEYCODE_CSINUM, '~', 23 }, // F11
  { KEYCODE_CSINUM, '~', 24 }, // F12
};

स्थिर keycodes_s keycodes_kp[] = {
  { KEYCODE_KEYPAD, '0', 'p' }, // KP_0
  { KEYCODE_KEYPAD, '1', 'q' }, // KP_1
  { KEYCODE_KEYPAD, '2', 'r' }, // KP_2
  { KEYCODE_KEYPAD, '3', 's' }, // KP_3
  { KEYCODE_KEYPAD, '4', 't' }, // KP_4
  { KEYCODE_KEYPAD, '5', 'u' }, // KP_5
  { KEYCODE_KEYPAD, '6', 'v' }, // KP_6
  { KEYCODE_KEYPAD, '7', 'w' }, // KP_7
  { KEYCODE_KEYPAD, '8', 'x' }, // KP_8
  { KEYCODE_KEYPAD, '9', 'y' }, // KP_9
  { KEYCODE_KEYPAD, '*', 'j' }, // KP_MULT
  { KEYCODE_KEYPAD, '+', 'k' }, // KP_PLUS
  { KEYCODE_KEYPAD, ',', 'l' }, // KP_COMMA
  { KEYCODE_KEYPAD, '-', 'm' }, // KP_MINUS
  { KEYCODE_KEYPAD, '.', 'n' }, // KP_PERIOD
  { KEYCODE_KEYPAD, '/', 'o' }, // KP_DIVIDE
  { KEYCODE_KEYPAD, '\n', 'M' }, // KP_ENTER
  { KEYCODE_KEYPAD, '=', 'X' }, // KP_EQUAL
};

शून्य vterm_keyboard_किल्ली(VTerm *vt, VTerm_किल्ली किल्ली, VTermModifier mod)
{
  जर(किल्ली == VTERM_किल्ली_NONE)
    परत;

  keycodes_s k;
  जर(किल्ली < VTERM_किल्ली_FUNCTION_0) {
    जर(किल्ली >= आकार(keycodes)/आकार(keycodes[0]))
      परत;
    k = keycodes[किल्ली];
  }
  अन्यथा जर(किल्ली >= VTERM_किल्ली_FUNCTION_0 && किल्ली <= VTERM_किल्ली_FUNCTION_MAX) {
    जर((किल्ली - VTERM_किल्ली_FUNCTION_0) >= आकार(keycodes_fn)/आकार(keycodes_fn[0]))
      परत;
    k = keycodes_fn[किल्ली - VTERM_किल्ली_FUNCTION_0];
  }
  अन्यथा जर(किल्ली >= VTERM_किल्ली_KP_0) {
    जर((किल्ली - VTERM_किल्ली_KP_0) >= आकार(keycodes_kp)/आकार(keycodes_kp[0]))
      परत;
    k = keycodes_kp[किल्ली - VTERM_किल्ली_KP_0];
  }

  निवड(k.type) {
  स्थिती KEYCODE_NONE:
    थांबा;

  स्थिती KEYCODE_TAB:
    जर (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    अन्यथा जर(mod == VTERM_MOD_SHIFT)
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "Z");
    अन्यथा जर(mod & VTERM_MOD_SHIFT)
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    अन्यथा
      जा case_LITERAL;
    थांबा;

  स्थिती KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    जर(vt->state->mode.newline)
      vterm_ढकला_output_sprintf(vt, "\r\n");
    अन्यथा
      जा case_LITERAL;
    थांबा;

  स्थिती KEYCODE_LITERAL: case_LITERAL:
    जर (vterm_is_modify_other_किल्ल्या(vt) && mod != 0)
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    अन्यथा जर (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    अन्यथा जर ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    अन्यथा
      vterm_ढकला_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    थांबा;

  स्थिती KEYCODE_SS3: case_SS3:
    जर(mod == 0)
      vterm_ढकला_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    अन्यथा
      जा case_CSI;
    थांबा;

  स्थिती KEYCODE_CSI: case_CSI:
    जर(mod == 0)
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    अन्यथा
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    थांबा;

  स्थिती KEYCODE_CSINUM:
    जर(mod == 0)
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    अन्यथा
      vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    थांबा;

  स्थिती KEYCODE_CSI_CURSOR:
    जर(vt->state->mode.cursor)
      जा case_SS3;
    अन्यथा
      जा case_CSI;

  स्थिती KEYCODE_KEYPAD:
    जर(vt->state->mode.keypad) {
      k.literal = k.csinum;
      जा case_SS3;
    }
    अन्यथा
      जा case_LITERAL;
  }
}

शून्य vterm_keyboard_start_paste(VTerm *vt)
{
  जर(vt->state->mode.bracketpaste)
    vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

शून्य vterm_keyboard_end_paste(VTerm *vt)
{
  जर(vt->state->mode.bracketpaste)
    vterm_ढकला_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
