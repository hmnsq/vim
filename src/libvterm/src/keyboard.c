#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>

#ಅನ್ತರ್ಭಾವ "utf8.h"

// VIM: added
ಪೂರ್ಣಾಂಕ vterm_is_modify_other_ಕೀಲಿಗಳು(VTerm *vt)
{
  ಹಿಂತಿರುಗಿ vt->state->ಬಹುಲಕ.modify_other_ಕೀಲಿಗಳು;
}

// VIM: added
ಪೂರ್ಣಾಂಕ vterm_is_kitty_keyboard(VTerm *vt)
{
  ಹಿಂತಿರುಗಿ vt->state->ಬಹುಲಕ.kitty_keyboard;
}


ಶೂನ್ಯ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  ಆದರೆ (vterm_is_modify_other_ಕೀಲಿಗಳು(vt) && mod != 0) {
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    ಹಿಂತಿರುಗಿ;
  }

  // VIM: added kitty keyboard protocol support
  ಆದರೆ (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ಹಿಂತಿರುಗಿ;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  ಆದರೆ(c != ' ')
    mod &= ~VTERM_MOD_ಸರಿಸು;

  ಆದರೆ(mod == 0) {
    // Normal text - ignore just shift
    ಅಕ್ಷರ str[6];
    ಪೂರ್ಣಾಂಕ seqlen = fill_utf8(c, str);
    vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(vt, str, seqlen);
    ಹಿಂತಿರುಗಿ;
  }

  ಪೂರ್ಣಾಂಕ needs_CSIu;
  ಆಯ್ಕೆ(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    ಸ್ಥಿತಿ 'i': ಸ್ಥಿತಿ 'j': ಸ್ಥಿತಿ 'm': ಸ್ಥಿತಿ '[':
      needs_CSIu = 1;
      ನಿಲ್ಲಿಸು;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    ಸ್ಥಿತಿ '\\': ಸ್ಥಿತಿ ']': ಸ್ಥಿತಿ '^': ಸ್ಥಿತಿ '_':
      needs_CSIu = 0;
      ನಿಲ್ಲಿಸು;
    /* Shift-space needs CSIu */
    ಸ್ಥಿತಿ ' ':
      needs_CSIu = !!(mod & VTERM_MOD_ಸರಿಸು);
      ನಿಲ್ಲಿಸು;
    /* All other characters needs CSIu except for letters a-z */
    ಪೂರ್ವನಿಯೋಜಿತ:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  ಆದರೆ(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ತಳ್ಳು_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
  ಎಣಿಕೆ {
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
  ಅಕ್ಷರ literal;
  ಪೂರ್ಣಾಂಕ csinum;
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
  ಆದರೆ(ಕೀಲಿ == VTERM_ಕೀಲಿ_NONE)
    ಹಿಂತಿರುಗಿ;

  keycodes_s k;
  ಆದರೆ(ಕೀಲಿ < VTERM_ಕೀಲಿ_FUNCTION_0) {
    ಆದರೆ(ಕೀಲಿ >= ಗಾತ್ರ(keycodes)/ಗಾತ್ರ(keycodes[0]))
      ಹಿಂತಿರುಗಿ;
    k = keycodes[ಕೀಲಿ];
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಕೀಲಿ >= VTERM_ಕೀಲಿ_FUNCTION_0 && ಕೀಲಿ <= VTERM_ಕೀಲಿ_FUNCTION_MAX) {
    ಆದರೆ((ಕೀಲಿ - VTERM_ಕೀಲಿ_FUNCTION_0) >= ಗಾತ್ರ(keycodes_fn)/ಗಾತ್ರ(keycodes_fn[0]))
      ಹಿಂತಿರುಗಿ;
    k = keycodes_fn[ಕೀಲಿ - VTERM_ಕೀಲಿ_FUNCTION_0];
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಕೀಲಿ >= VTERM_ಕೀಲಿ_KP_0) {
    ಆದರೆ((ಕೀಲಿ - VTERM_ಕೀಲಿ_KP_0) >= ಗಾತ್ರ(keycodes_kp)/ಗಾತ್ರ(keycodes_kp[0]))
      ಹಿಂತಿರುಗಿ;
    k = keycodes_kp[ಕೀಲಿ - VTERM_ಕೀಲಿ_KP_0];
  }

  ಆಯ್ಕೆ(k.type) {
  ಸ್ಥಿತಿ KEYCODE_NONE:
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ KEYCODE_TAB:
    ಆದರೆ (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(mod == VTERM_MOD_ಸರಿಸು)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "Z");
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(mod & VTERM_MOD_ಸರಿಸು)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೋಗು case_LITERAL;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    ಆದರೆ(vt->state->ಬಹುಲಕ.newline)
      vterm_ತಳ್ಳು_output_sprintf(vt, "\r\n");
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೋಗು case_LITERAL;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ KEYCODE_LITERAL: case_LITERAL:
    ಆದರೆ (vterm_is_modify_other_ಕೀಲಿಗಳು(vt) && mod != 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_ಸರಿಸು|VTERM_MOD_CTRL)))
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    ಇಲ್ಲದಿದ್ದರೆ
      vterm_ತಳ್ಳು_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ KEYCODE_SS3: case_SS3:
    ಆದರೆ(mod == 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೋಗು case_CSI;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ KEYCODE_CSI: case_CSI:
    ಆದರೆ(mod == 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    ಇಲ್ಲದಿದ್ದರೆ
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ KEYCODE_CSINUM:
    ಆದರೆ(mod == 0)
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    ಇಲ್ಲದಿದ್ದರೆ
      vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ KEYCODE_CSI_CURSOR:
    ಆದರೆ(vt->state->ಬಹುಲಕ.cursor)
      ಹೋಗು case_SS3;
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೋಗು case_CSI;

  ಸ್ಥಿತಿ KEYCODE_KEYPAD:
    ಆದರೆ(vt->state->ಬಹುಲಕ.keypad) {
      k.literal = k.csinum;
      ಹೋಗು case_SS3;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹೋಗು case_LITERAL;
  }
}

ಶೂನ್ಯ vterm_keyboard_ಆರಂಭ_paste(VTerm *vt)
{
  ಆದರೆ(vt->state->ಬಹುಲಕ.bracketpaste)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

ಶೂನ್ಯ vterm_keyboard_ಕೊನೆ_paste(VTerm *vt)
{
  ಆದರೆ(vt->state->ಬಹುಲಕ.bracketpaste)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
