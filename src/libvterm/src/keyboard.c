#അന്തര്ഭാവ "vterm_internal.h"

#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "utf8.h"

// VIM: added
പൂർണ്ണസംഖ്യ vterm_is_modify_other_താക്കോലുകൾ(VTerm *vt)
{
  തിരികെ vt->state->mode.modify_other_താക്കോലുകൾ;
}

// VIM: added
പൂർണ്ണസംഖ്യ vterm_is_kitty_keyboard(VTerm *vt)
{
  തിരികെ vt->state->mode.kitty_keyboard;
}


ശൂന്യം vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  എങ്കിൽ (vterm_is_modify_other_താക്കോലുകൾ(vt) && mod != 0) {
    vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    തിരികെ;
  }

  // VIM: added kitty keyboard protocol support
  എങ്കിൽ (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    തിരികെ;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  എങ്കിൽ(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  എങ്കിൽ(mod == 0) {
    // Normal text - ignore just shift
    അക്ഷരം str[6];
    പൂർണ്ണസംഖ്യ seqlen = fill_utf8(c, str);
    vterm_തള്ളുക_output_bytes(vt, str, seqlen);
    തിരികെ;
  }

  പൂർണ്ണസംഖ്യ needs_CSIu;
  തിരഞ്ഞെടുപ്പ്(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    അവസ്ഥ 'i': അവസ്ഥ 'j': അവസ്ഥ 'm': അവസ്ഥ '[':
      needs_CSIu = 1;
      നിർത്തുക;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    അവസ്ഥ '\\': അവസ്ഥ ']': അവസ്ഥ '^': അവസ്ഥ '_':
      needs_CSIu = 0;
      നിർത്തുക;
    /* Shift-space needs CSIu */
    അവസ്ഥ ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      നിർത്തുക;
    /* All other characters needs CSIu except for letters a-z */
    സ്ഥിരസ്ഥിതി:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  എങ്കിൽ(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    തിരികെ;
  }

  എങ്കിൽ(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_തള്ളുക_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

തരനിർവചനം ഘടന {
  എണ്ണൽ {
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
  അക്ഷരം literal;
  പൂർണ്ണസംഖ്യ csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
സ്ഥിതം keycodes_s keycodes[] = {
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

സ്ഥിതം keycodes_s keycodes_fn[] = {
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

സ്ഥിതം keycodes_s keycodes_kp[] = {
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

ശൂന്യം vterm_keyboard_താക്കോൽ(VTerm *vt, VTerm_താക്കോൽ താക്കോൽ, VTermModifier mod)
{
  എങ്കിൽ(താക്കോൽ == VTERM_താക്കോൽ_NONE)
    തിരികെ;

  keycodes_s k;
  എങ്കിൽ(താക്കോൽ < VTERM_താക്കോൽ_FUNCTION_0) {
    എങ്കിൽ(താക്കോൽ >= വലുപ്പം(keycodes)/വലുപ്പം(keycodes[0]))
      തിരികെ;
    k = keycodes[താക്കോൽ];
  }
  അല്ലെങ്കിൽ എങ്കിൽ(താക്കോൽ >= VTERM_താക്കോൽ_FUNCTION_0 && താക്കോൽ <= VTERM_താക്കോൽ_FUNCTION_MAX) {
    എങ്കിൽ((താക്കോൽ - VTERM_താക്കോൽ_FUNCTION_0) >= വലുപ്പം(keycodes_fn)/വലുപ്പം(keycodes_fn[0]))
      തിരികെ;
    k = keycodes_fn[താക്കോൽ - VTERM_താക്കോൽ_FUNCTION_0];
  }
  അല്ലെങ്കിൽ എങ്കിൽ(താക്കോൽ >= VTERM_താക്കോൽ_KP_0) {
    എങ്കിൽ((താക്കോൽ - VTERM_താക്കോൽ_KP_0) >= വലുപ്പം(keycodes_kp)/വലുപ്പം(keycodes_kp[0]))
      തിരികെ;
    k = keycodes_kp[താക്കോൽ - VTERM_താക്കോൽ_KP_0];
  }

  തിരഞ്ഞെടുപ്പ്(k.type) {
  അവസ്ഥ KEYCODE_NONE:
    നിർത്തുക;

  അവസ്ഥ KEYCODE_TAB:
    എങ്കിൽ (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    അല്ലെങ്കിൽ എങ്കിൽ(mod == VTERM_MOD_SHIFT)
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "Z");
    അല്ലെങ്കിൽ എങ്കിൽ(mod & VTERM_MOD_SHIFT)
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    അല്ലെങ്കിൽ
      പോകുക case_LITERAL;
    നിർത്തുക;

  അവസ്ഥ KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    എങ്കിൽ(vt->state->mode.newline)
      vterm_തള്ളുക_output_sprintf(vt, "\r\n");
    അല്ലെങ്കിൽ
      പോകുക case_LITERAL;
    നിർത്തുക;

  അവസ്ഥ KEYCODE_LITERAL: case_LITERAL:
    എങ്കിൽ (vterm_is_modify_other_താക്കോലുകൾ(vt) && mod != 0)
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    അല്ലെങ്കിൽ എങ്കിൽ (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    അല്ലെങ്കിൽ എങ്കിൽ ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    അല്ലെങ്കിൽ
      vterm_തള്ളുക_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    നിർത്തുക;

  അവസ്ഥ KEYCODE_SS3: case_SS3:
    എങ്കിൽ(mod == 0)
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    അല്ലെങ്കിൽ
      പോകുക case_CSI;
    നിർത്തുക;

  അവസ്ഥ KEYCODE_CSI: case_CSI:
    എങ്കിൽ(mod == 0)
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    അല്ലെങ്കിൽ
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    നിർത്തുക;

  അവസ്ഥ KEYCODE_CSINUM:
    എങ്കിൽ(mod == 0)
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    അല്ലെങ്കിൽ
      vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    നിർത്തുക;

  അവസ്ഥ KEYCODE_CSI_CURSOR:
    എങ്കിൽ(vt->state->mode.cursor)
      പോകുക case_SS3;
    അല്ലെങ്കിൽ
      പോകുക case_CSI;

  അവസ്ഥ KEYCODE_KEYPAD:
    എങ്കിൽ(vt->state->mode.keypad) {
      k.literal = k.csinum;
      പോകുക case_SS3;
    }
    അല്ലെങ്കിൽ
      പോകുക case_LITERAL;
  }
}

ശൂന്യം vterm_keyboard_start_paste(VTerm *vt)
{
  എങ്കിൽ(vt->state->mode.bracketpaste)
    vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

ശൂന്യം vterm_keyboard_end_paste(VTerm *vt)
{
  എങ്കിൽ(vt->state->mode.bracketpaste)
    vterm_തള്ളുക_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
