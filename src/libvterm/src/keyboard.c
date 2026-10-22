#অন্তর্ভাব "vterm_internal.h"

#অন্তর্ভাব <stdio.h>

#অন্তর্ভাব "utf8.h"

// VIM: added
পূর্ণসংখ্যা vterm_is_modify_other_চাবিগুলি(VTerm *vt)
{
  ফেরত vt->state->mode.modify_other_চাবিগুলি;
}

// VIM: added
পূর্ণসংখ্যা vterm_is_kitty_keyboard(VTerm *vt)
{
  ফেরত vt->state->mode.kitty_keyboard;
}


শূন্য vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  যদি (vterm_is_modify_other_চাবিগুলি(vt) && mod != 0) {
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    ফেরত;
  }

  // VIM: added kitty keyboard protocol support
  যদি (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ফেরত;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  যদি(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  যদি(mod == 0) {
    // Normal text - ignore just shift
    অক্ষর str[6];
    পূর্ণসংখ্যা seqlen = fill_utf8(c, str);
    vterm_ঠেলো_output_bytes(vt, str, seqlen);
    ফেরত;
  }

  পূর্ণসংখ্যা needs_CSIu;
  নির্বাচন(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    ক্ষেত্র 'i': ক্ষেত্র 'j': ক্ষেত্র 'm': ক্ষেত্র '[':
      needs_CSIu = 1;
      বিরতি;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    ক্ষেত্র '\\': ক্ষেত্র ']': ক্ষেত্র '^': ক্ষেত্র '_':
      needs_CSIu = 0;
      বিরতি;
    /* Shift-space needs CSIu */
    ক্ষেত্র ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      বিরতি;
    /* All other characters needs CSIu except for letters a-z */
    পূর্বনির্ধারিত:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  যদি(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ফেরত;
  }

  যদি(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ঠেলো_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

প্রকারলেখ কাঠামো {
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
  অক্ষর literal;
  পূর্ণসংখ্যা csinum;
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
    ফেরত;

  keycodes_s k;
  যদি(চাবি < VTERM_চাবি_FUNCTION_0) {
    যদি(চাবি >= আকার(keycodes)/আকার(keycodes[0]))
      ফেরত;
    k = keycodes[চাবি];
  }
  অন্যথা যদি(চাবি >= VTERM_চাবি_FUNCTION_0 && চাবি <= VTERM_চাবি_FUNCTION_MAX) {
    যদি((চাবি - VTERM_চাবি_FUNCTION_0) >= আকার(keycodes_fn)/আকার(keycodes_fn[0]))
      ফেরত;
    k = keycodes_fn[চাবি - VTERM_চাবি_FUNCTION_0];
  }
  অন্যথা যদি(চাবি >= VTERM_চাবি_KP_0) {
    যদি((চাবি - VTERM_চাবি_KP_0) >= আকার(keycodes_kp)/আকার(keycodes_kp[0]))
      ফেরত;
    k = keycodes_kp[চাবি - VTERM_চাবি_KP_0];
  }

  নির্বাচন(k.type) {
  ক্ষেত্র KEYCODE_NONE:
    বিরতি;

  ক্ষেত্র KEYCODE_TAB:
    যদি (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    অন্যথা যদি(mod == VTERM_MOD_SHIFT)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "Z");
    অন্যথা যদি(mod & VTERM_MOD_SHIFT)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    অন্যথা
      যাও case_LITERAL;
    বিরতি;

  ক্ষেত্র KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    যদি(vt->state->mode.newline)
      vterm_ঠেলো_output_sprintf(vt, "\r\n");
    অন্যথা
      যাও case_LITERAL;
    বিরতি;

  ক্ষেত্র KEYCODE_LITERAL: case_LITERAL:
    যদি (vterm_is_modify_other_চাবিগুলি(vt) && mod != 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    অন্যথা যদি (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    অন্যথা যদি ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    অন্যথা
      vterm_ঠেলো_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    বিরতি;

  ক্ষেত্র KEYCODE_SS3: case_SS3:
    যদি(mod == 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    অন্যথা
      যাও case_CSI;
    বিরতি;

  ক্ষেত্র KEYCODE_CSI: case_CSI:
    যদি(mod == 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    অন্যথা
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    বিরতি;

  ক্ষেত্র KEYCODE_CSINUM:
    যদি(mod == 0)
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    অন্যথা
      vterm_ঠেলো_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    বিরতি;

  ক্ষেত্র KEYCODE_CSI_CURSOR:
    যদি(vt->state->mode.cursor)
      যাও case_SS3;
    অন্যথা
      যাও case_CSI;

  ক্ষেত্র KEYCODE_KEYPAD:
    যদি(vt->state->mode.keypad) {
      k.literal = k.csinum;
      যাও case_SS3;
    }
    অন্যথা
      যাও case_LITERAL;
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
