#অন্তর্ভাব "vterm_internal.h"

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "utf8.h"

// VIM: added
পূর্ণাঙ্ক vterm_is_modify_other_চাবিগুলি(VTerm *vt)
{
  প্রত্যাগম vt->state->mode.modify_other_চাবিগুলি;
}

// VIM: added
পূর্ণাঙ্ক vterm_is_kitty_keyboard(VTerm *vt)
{
  প্রত্যাগম vt->state->mode.kitty_keyboard;
}


শূন্য vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  যদি (vterm_is_modify_other_চাবিগুলি(vt) && mod != 0) {
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    প্রত্যাগম;
  }

  // VIM: added kitty keyboard protocol support
  যদি (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    প্রত্যাগম;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  যদি(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  যদি(mod == 0) {
    // Normal text - ignore just shift
    বর্ণ str[6];
    পূর্ণাঙ্ক seqlen = fill_utf8(c, str);
    vterm_ঠেলো_output_bytes(vt, str, seqlen);
    প্রত্যাগম;
  }

  পূর্ণাঙ্ক needs_CSIu;
  বিকল্প(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    পক্ষ 'i': পক্ষ 'j': পক্ষ 'm': পক্ষ '[':
      needs_CSIu = 1;
      ভঙ্গ;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    পক্ষ '\\': পক্ষ ']': পক্ষ '^': পক্ষ '_':
      needs_CSIu = 0;
      ভঙ্গ;
    /* Shift-space needs CSIu */
    পক্ষ ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      ভঙ্গ;
    /* All other characters needs CSIu except for letters a-z */
    স্বভাব:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  যদি(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    প্রত্যাগম;
  }

  যদি(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ঠেলো_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

প্রকারনির্দেশ সংরচনা {
  গণনা {
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
  বর্ণ literal;
  পূর্ণাঙ্ক csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
স্থির keycodes_s keycodes[] = {
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

স্থির keycodes_s keycodes_fn[] = {
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

স্থির keycodes_s keycodes_kp[] = {
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

শূন্য vterm_keyboard_চাবি(VTerm *vt, VTerm_চাবি চাবি, VTermModifier mod)
{
  যদি(চাবি == VTERM_চাবি_NONE)
    প্রত্যাগম;

  keycodes_s k;
  যদি(চাবি < VTERM_চাবি_FUNCTION_0) {
    যদি(চাবি >= পরিমাণ(keycodes)/পরিমাণ(keycodes[0]))
      প্রত্যাগম;
    k = keycodes[চাবি];
  }
  অন্যথা যদি(চাবি >= VTERM_চাবি_FUNCTION_0 && চাবি <= VTERM_চাবি_FUNCTION_MAX) {
    যদি((চাবি - VTERM_চাবি_FUNCTION_0) >= পরিমাণ(keycodes_fn)/পরিমাণ(keycodes_fn[0]))
      প্রত্যাগম;
    k = keycodes_fn[চাবি - VTERM_চাবি_FUNCTION_0];
  }
  অন্যথা যদি(চাবি >= VTERM_চাবি_KP_0) {
    যদি((চাবি - VTERM_চাবি_KP_0) >= পরিমাণ(keycodes_kp)/পরিমাণ(keycodes_kp[0]))
      প্রত্যাগম;
    k = keycodes_kp[চাবি - VTERM_চাবি_KP_0];
  }

  বিকল্প(k.type) {
  পক্ষ KEYCODE_NONE:
    ভঙ্গ;

  পক্ষ KEYCODE_TAB:
    যদি (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    অন্যথা যদি(mod == VTERM_MOD_SHIFT)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "Z");
    অন্যথা যদি(mod & VTERM_MOD_SHIFT)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    অন্যথা
      গচ্ছ case_LITERAL;
    ভঙ্গ;

  পক্ষ KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    যদি(vt->state->mode.newline)
      vterm_ঠেলো_output_sprintf(vt, "\r\n");
    অন্যথা
      গচ্ছ case_LITERAL;
    ভঙ্গ;

  পক্ষ KEYCODE_LITERAL: case_LITERAL:
    যদি (vterm_is_modify_other_চাবিগুলি(vt) && mod != 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    অন্যথা যদি (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    অন্যথা যদি ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    অন্যথা
      vterm_ঠেলো_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    ভঙ্গ;

  পক্ষ KEYCODE_SS3: case_SS3:
    যদি(mod == 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    অন্যথা
      গচ্ছ case_CSI;
    ভঙ্গ;

  পক্ষ KEYCODE_CSI: case_CSI:
    যদি(mod == 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    অন্যথা
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    ভঙ্গ;

  পক্ষ KEYCODE_CSINUM:
    যদি(mod == 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    অন্যথা
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    ভঙ্গ;

  পক্ষ KEYCODE_CSI_CURSOR:
    যদি(vt->state->mode.cursor)
      গচ্ছ case_SS3;
    অন্যথা
      গচ্ছ case_CSI;

  পক্ষ KEYCODE_KEYPAD:
    যদি(vt->state->mode.keypad) {
      k.literal = k.csinum;
      গচ্ছ case_SS3;
    }
    অন্যথা
      গচ্ছ case_LITERAL;
  }
}

শূন্য vterm_keyboard_start_paste(VTerm *vt)
{
  যদি(vt->state->mode.bracketpaste)
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

শূন্য vterm_keyboard_end_paste(VTerm *vt)
{
  যদি(vt->state->mode.bracketpaste)
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
