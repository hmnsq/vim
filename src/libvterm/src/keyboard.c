#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव <stdio.h>

#अन्तर्भाव "utf8.h"

// VIM: added
पूर्णाङ्क vterm_is_modify_other_कुंजियाँ(VTerm *vt)
{
  प्रत्यागम vt->state->mode.modify_other_कुंजियाँ;
}

// VIM: added
पूर्णाङ्क vterm_is_kitty_keyboard(VTerm *vt)
{
  प्रत्यागम vt->state->mode.kitty_keyboard;
}


शून्य vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  यदि (vterm_is_modify_other_कुंजियाँ(vt) && mod != 0) {
    vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    प्रत्यागम;
  }

  // VIM: added kitty keyboard protocol support
  यदि (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    प्रत्यागम;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  यदि(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  यदि(mod == 0) {
    // Normal text - ignore just shift
    वर्ण str[6];
    पूर्णाङ्क seqlen = fill_utf8(c, str);
    vterm_धकेलो_output_bytes(vt, str, seqlen);
    प्रत्यागम;
  }

  पूर्णाङ्क needs_CSIu;
  विकल्प(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    पक्ष 'i': पक्ष 'j': पक्ष 'm': पक्ष '[':
      needs_CSIu = 1;
      भङ्ग;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    पक्ष '\\': पक्ष ']': पक्ष '^': पक्ष '_':
      needs_CSIu = 0;
      भङ्ग;
    /* Shift-space needs CSIu */
    पक्ष ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      भङ्ग;
    /* All other characters needs CSIu except for letters a-z */
    स्वभाव:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  यदि(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    प्रत्यागम;
  }

  यदि(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_धकेलो_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

प्रकारनिर्देश संरचना {
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
  वर्ण literal;
  पूर्णाङ्क csinum;
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

शून्य vterm_keyboard_कुंजी(VTerm *vt, VTerm_कुंजी कुंजी, VTermModifier mod)
{
  यदि(कुंजी == VTERM_कुंजी_NONE)
    प्रत्यागम;

  keycodes_s k;
  यदि(कुंजी < VTERM_कुंजी_FUNCTION_0) {
    यदि(कुंजी >= परिमाण(keycodes)/परिमाण(keycodes[0]))
      प्रत्यागम;
    k = keycodes[कुंजी];
  }
  अन्यथा यदि(कुंजी >= VTERM_कुंजी_FUNCTION_0 && कुंजी <= VTERM_कुंजी_FUNCTION_MAX) {
    यदि((कुंजी - VTERM_कुंजी_FUNCTION_0) >= परिमाण(keycodes_fn)/परिमाण(keycodes_fn[0]))
      प्रत्यागम;
    k = keycodes_fn[कुंजी - VTERM_कुंजी_FUNCTION_0];
  }
  अन्यथा यदि(कुंजी >= VTERM_कुंजी_KP_0) {
    यदि((कुंजी - VTERM_कुंजी_KP_0) >= परिमाण(keycodes_kp)/परिमाण(keycodes_kp[0]))
      प्रत्यागम;
    k = keycodes_kp[कुंजी - VTERM_कुंजी_KP_0];
  }

  विकल्प(k.type) {
  पक्ष KEYCODE_NONE:
    भङ्ग;

  पक्ष KEYCODE_TAB:
    यदि (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    अन्यथा यदि(mod == VTERM_MOD_SHIFT)
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "Z");
    अन्यथा यदि(mod & VTERM_MOD_SHIFT)
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    अन्यथा
      गच्छ case_LITERAL;
    भङ्ग;

  पक्ष KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    यदि(vt->state->mode.newline)
      vterm_धकेलो_output_sprintf(vt, "\r\n");
    अन्यथा
      गच्छ case_LITERAL;
    भङ्ग;

  पक्ष KEYCODE_LITERAL: case_LITERAL:
    यदि (vterm_is_modify_other_कुंजियाँ(vt) && mod != 0)
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    अन्यथा यदि (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    अन्यथा यदि ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    अन्यथा
      vterm_धकेलो_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    भङ्ग;

  पक्ष KEYCODE_SS3: case_SS3:
    यदि(mod == 0)
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    अन्यथा
      गच्छ case_CSI;
    भङ्ग;

  पक्ष KEYCODE_CSI: case_CSI:
    यदि(mod == 0)
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    अन्यथा
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    भङ्ग;

  पक्ष KEYCODE_CSINUM:
    यदि(mod == 0)
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    अन्यथा
      vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    भङ्ग;

  पक्ष KEYCODE_CSI_CURSOR:
    यदि(vt->state->mode.cursor)
      गच्छ case_SS3;
    अन्यथा
      गच्छ case_CSI;

  पक्ष KEYCODE_KEYPAD:
    यदि(vt->state->mode.keypad) {
      k.literal = k.csinum;
      गच्छ case_SS3;
    }
    अन्यथा
      गच्छ case_LITERAL;
  }
}

शून्य vterm_keyboard_start_paste(VTerm *vt)
{
  यदि(vt->state->mode.bracketpaste)
    vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

शून्य vterm_keyboard_end_paste(VTerm *vt)
{
  यदि(vt->state->mode.bracketpaste)
    vterm_धकेलो_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
