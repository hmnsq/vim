#شامل "vterm_internal.h"

#شامل <stdio.h>

#شامل "utf8.h"

// VIM: added
صحیح vterm_is_modify_other_کلیدها(VTerm *vt)
{
  بازگشت vt->state->mode.modify_other_کلیدها;
}

// VIM: added
صحیح vterm_is_kitty_keyboard(VTerm *vt)
{
  بازگشت vt->state->mode.kitty_keyboard;
}


تهی vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  اگر (vterm_is_modify_other_کلیدها(vt) && mod != 0) {
    vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    بازگشت;
  }

  // VIM: added kitty keyboard protocol support
  اگر (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    بازگشت;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  اگر(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  اگر(mod == 0) {
    // Normal text - ignore just shift
    نویسه str[6];
    صحیح seqlen = fill_utf8(c, str);
    vterm_افزودن_output_bytes(vt, str, seqlen);
    بازگشت;
  }

  صحیح needs_CSIu;
  انتخاب(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    حالت 'i': حالت 'j': حالت 'm': حالت '[':
      needs_CSIu = 1;
      بشکن;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    حالت '\\': حالت ']': حالت '^': حالت '_':
      needs_CSIu = 0;
      بشکن;
    /* Shift-space needs CSIu */
    حالت ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      بشکن;
    /* All other characters needs CSIu except for letters a-z */
    پیشفرض:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  اگر(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    بازگشت;
  }

  اگر(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_افزودن_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

تعریفنوع ساختار {
  شمارش {
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
  نویسه literal;
  صحیح csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
ایستا keycodes_s keycodes[] = {
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

ایستا keycodes_s keycodes_fn[] = {
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

ایستا keycodes_s keycodes_kp[] = {
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

تهی vterm_keyboard_کلید(VTerm *vt, VTerm_کلید کلید, VTermModifier mod)
{
  اگر(کلید == VTERM_کلید_NONE)
    بازگشت;

  keycodes_s k;
  اگر(کلید < VTERM_کلید_FUNCTION_0) {
    اگر(کلید >= اندازه(keycodes)/اندازه(keycodes[0]))
      بازگشت;
    k = keycodes[کلید];
  }
  وگرنه اگر(کلید >= VTERM_کلید_FUNCTION_0 && کلید <= VTERM_کلید_FUNCTION_MAX) {
    اگر((کلید - VTERM_کلید_FUNCTION_0) >= اندازه(keycodes_fn)/اندازه(keycodes_fn[0]))
      بازگشت;
    k = keycodes_fn[کلید - VTERM_کلید_FUNCTION_0];
  }
  وگرنه اگر(کلید >= VTERM_کلید_KP_0) {
    اگر((کلید - VTERM_کلید_KP_0) >= اندازه(keycodes_kp)/اندازه(keycodes_kp[0]))
      بازگشت;
    k = keycodes_kp[کلید - VTERM_کلید_KP_0];
  }

  انتخاب(k.type) {
  حالت KEYCODE_NONE:
    بشکن;

  حالت KEYCODE_TAB:
    اگر (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    وگرنه اگر(mod == VTERM_MOD_SHIFT)
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "Z");
    وگرنه اگر(mod & VTERM_MOD_SHIFT)
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    وگرنه
      برو case_LITERAL;
    بشکن;

  حالت KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    اگر(vt->state->mode.newline)
      vterm_افزودن_output_sprintf(vt, "\r\n");
    وگرنه
      برو case_LITERAL;
    بشکن;

  حالت KEYCODE_LITERAL: case_LITERAL:
    اگر (vterm_is_modify_other_کلیدها(vt) && mod != 0)
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    وگرنه اگر (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    وگرنه اگر ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    وگرنه
      vterm_افزودن_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    بشکن;

  حالت KEYCODE_SS3: case_SS3:
    اگر(mod == 0)
      vterm_افزودن_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    وگرنه
      برو case_CSI;
    بشکن;

  حالت KEYCODE_CSI: case_CSI:
    اگر(mod == 0)
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    وگرنه
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    بشکن;

  حالت KEYCODE_CSINUM:
    اگر(mod == 0)
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    وگرنه
      vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    بشکن;

  حالت KEYCODE_CSI_CURSOR:
    اگر(vt->state->mode.cursor)
      برو case_SS3;
    وگرنه
      برو case_CSI;

  حالت KEYCODE_KEYPAD:
    اگر(vt->state->mode.keypad) {
      k.literal = k.csinum;
      برو case_SS3;
    }
    وگرنه
      برو case_LITERAL;
  }
}

تهی vterm_keyboard_start_paste(VTerm *vt)
{
  اگر(vt->state->mode.bracketpaste)
    vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

تهی vterm_keyboard_end_paste(VTerm *vt)
{
  اگر(vt->state->mode.bracketpaste)
    vterm_افزودن_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
