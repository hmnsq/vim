#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "utf8.h"

// VIM: added
ಪೂರ್ಣಾಙ್ಕ vterm_is_modify_other_ಕೀಲಿಗಳು(VTerm *vt)
{
  ಪ್ರತ್ಯಾಗಮ vt->state->mode.modify_other_ಕೀಲಿಗಳು;
}

// VIM: added
ಪೂರ್ಣಾಙ್ಕ vterm_is_kitty_keyboard(VTerm *vt)
{
  ಪ್ರತ್ಯಾಗಮ vt->state->mode.kitty_keyboard;
}


ಶೂನ್ಯ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  ಯದಿ (vterm_is_modify_other_ಕೀಲಿಗಳು(vt) && mod != 0) {
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    ಪ್ರತ್ಯಾಗಮ;
  }

  // VIM: added kitty keyboard protocol support
  ಯದಿ (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ಪ್ರತ್ಯಾಗಮ;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  ಯದಿ(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  ಯದಿ(mod == 0) {
    // Normal text - ignore just shift
    ವರ್ಣ str[6];
    ಪೂರ್ಣಾಙ್ಕ seqlen = fill_utf8(c, str);
    vterm_ತಳ್ಳು_output_bytes(vt, str, seqlen);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಪೂರ್ಣಾಙ್ಕ needs_CSIu;
  ವಿಕಲ್ಪ(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    ಪಕ್ಷ 'i': ಪಕ್ಷ 'j': ಪಕ್ಷ 'm': ಪಕ್ಷ '[':
      needs_CSIu = 1;
      ಭಙ್ಗ;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    ಪಕ್ಷ '\\': ಪಕ್ಷ ']': ಪಕ್ಷ '^': ಪಕ್ಷ '_':
      needs_CSIu = 0;
      ಭಙ್ಗ;
    /* Shift-space needs CSIu */
    ಪಕ್ಷ ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      ಭಙ್ಗ;
    /* All other characters needs CSIu except for letters a-z */
    ಸ್ವಭಾವ:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  ಯದಿ(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ತಳ್ಳು_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
  ಗಣನಾ {
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
  ವರ್ಣ literal;
  ಪೂರ್ಣಾಙ್ಕ csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
ಸ್ಥಿರ keycodes_s keycodes[] = {
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

ಸ್ಥಿರ keycodes_s keycodes_fn[] = {
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

ಸ್ಥಿರ keycodes_s keycodes_kp[] = {
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

ಶೂನ್ಯ vterm_keyboard_ಕೀಲಿ(VTerm *vt, VTerm_ಕೀಲಿ ಕೀಲಿ, VTermModifier mod)
{
  ಯದಿ(ಕೀಲಿ == VTERM_ಕೀಲಿ_NONE)
    ಪ್ರತ್ಯಾಗಮ;

  keycodes_s k;
  ಯದಿ(ಕೀಲಿ < VTERM_ಕೀಲಿ_FUNCTION_0) {
    ಯದಿ(ಕೀಲಿ >= ಪರಿಮಾಣ(keycodes)/ಪರಿಮಾಣ(keycodes[0]))
      ಪ್ರತ್ಯಾಗಮ;
    k = keycodes[ಕೀಲಿ];
  }
  ಅನ್ಯಥಾ ಯದಿ(ಕೀಲಿ >= VTERM_ಕೀಲಿ_FUNCTION_0 && ಕೀಲಿ <= VTERM_ಕೀಲಿ_FUNCTION_MAX) {
    ಯದಿ((ಕೀಲಿ - VTERM_ಕೀಲಿ_FUNCTION_0) >= ಪರಿಮಾಣ(keycodes_fn)/ಪರಿಮಾಣ(keycodes_fn[0]))
      ಪ್ರತ್ಯಾಗಮ;
    k = keycodes_fn[ಕೀಲಿ - VTERM_ಕೀಲಿ_FUNCTION_0];
  }
  ಅನ್ಯಥಾ ಯದಿ(ಕೀಲಿ >= VTERM_ಕೀಲಿ_KP_0) {
    ಯದಿ((ಕೀಲಿ - VTERM_ಕೀಲಿ_KP_0) >= ಪರಿಮಾಣ(keycodes_kp)/ಪರಿಮಾಣ(keycodes_kp[0]))
      ಪ್ರತ್ಯಾಗಮ;
    k = keycodes_kp[ಕೀಲಿ - VTERM_ಕೀಲಿ_KP_0];
  }

  ವಿಕಲ್ಪ(k.type) {
  ಪಕ್ಷ KEYCODE_NONE:
    ಭಙ್ಗ;

  ಪಕ್ಷ KEYCODE_TAB:
    ಯದಿ (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    ಅನ್ಯಥಾ ಯದಿ(mod == VTERM_MOD_SHIFT)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "Z");
    ಅನ್ಯಥಾ ಯದಿ(mod & VTERM_MOD_SHIFT)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    ಅನ್ಯಥಾ
      ಗಚ್ಛ case_LITERAL;
    ಭಙ್ಗ;

  ಪಕ್ಷ KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    ಯದಿ(vt->state->mode.newline)
      vterm_ತಳ್ಳು_output_sprintf(vt, "\r\n");
    ಅನ್ಯಥಾ
      ಗಚ್ಛ case_LITERAL;
    ಭಙ್ಗ;

  ಪಕ್ಷ KEYCODE_LITERAL: case_LITERAL:
    ಯದಿ (vterm_is_modify_other_ಕೀಲಿಗಳು(vt) && mod != 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    ಅನ್ಯಥಾ ಯದಿ (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    ಅನ್ಯಥಾ ಯದಿ ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    ಅನ್ಯಥಾ
      vterm_ತಳ್ಳು_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    ಭಙ್ಗ;

  ಪಕ್ಷ KEYCODE_SS3: case_SS3:
    ಯದಿ(mod == 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    ಅನ್ಯಥಾ
      ಗಚ್ಛ case_CSI;
    ಭಙ್ಗ;

  ಪಕ್ಷ KEYCODE_CSI: case_CSI:
    ಯದಿ(mod == 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    ಅನ್ಯಥಾ
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    ಭಙ್ಗ;

  ಪಕ್ಷ KEYCODE_CSINUM:
    ಯದಿ(mod == 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    ಅನ್ಯಥಾ
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    ಭಙ್ಗ;

  ಪಕ್ಷ KEYCODE_CSI_CURSOR:
    ಯದಿ(vt->state->mode.cursor)
      ಗಚ್ಛ case_SS3;
    ಅನ್ಯಥಾ
      ಗಚ್ಛ case_CSI;

  ಪಕ್ಷ KEYCODE_KEYPAD:
    ಯದಿ(vt->state->mode.keypad) {
      k.literal = k.csinum;
      ಗಚ್ಛ case_SS3;
    }
    ಅನ್ಯಥಾ
      ಗಚ್ಛ case_LITERAL;
  }
}

ಶೂನ್ಯ vterm_keyboard_start_paste(VTerm *vt)
{
  ಯದಿ(vt->state->mode.bracketpaste)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

ಶೂನ್ಯ vterm_keyboard_end_paste(VTerm *vt)
{
  ಯದಿ(vt->state->mode.bracketpaste)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
