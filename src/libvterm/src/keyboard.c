#ضمن "vterm_internal.h"

#ضمن <stdio.h>

#ضمن "utf8.h"

// VIM: added
صحيح vterm_is_modify_other_ئاچقۇچلار(VTerm *vt)
{
  أرجع vt->state->mode.modify_other_ئاچقۇچلار;
}

// VIM: added
صحيح vterm_is_kitty_keyboard(VTerm *vt)
{
  أرجع vt->state->mode.kitty_keyboard;
}


فراغ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  إذا (vterm_is_modify_other_ئاچقۇچلار(vt) && mod != 0) {
    vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    أرجع;
  }

  // VIM: added kitty keyboard protocol support
  إذا (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    أرجع;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  إذا(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  إذا(mod == 0) {
    // Normal text - ignore just shift
    حرف str[6];
    صحيح seqlen = fill_utf8(c, str);
    vterm_ئىتتىرىش_output_bytes(vt, str, seqlen);
    أرجع;
  }

  صحيح needs_CSIu;
  بدل(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    حالة 'i': حالة 'j': حالة 'm': حالة '[':
      needs_CSIu = 1;
      اكسر;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    حالة '\\': حالة ']': حالة '^': حالة '_':
      needs_CSIu = 0;
      اكسر;
    /* Shift-space needs CSIu */
    حالة ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      اكسر;
    /* All other characters needs CSIu except for letters a-z */
    افتراضي:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  إذا(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    أرجع;
  }

  إذا(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ئىتتىرىش_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

تعريفنوع بنية {
  تعداد {
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
  حرف literal;
  صحيح csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
ساكن keycodes_s keycodes[] = {
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

ساكن keycodes_s keycodes_fn[] = {
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

ساكن keycodes_s keycodes_kp[] = {
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

فراغ vterm_keyboard_ئاچقۇچ(VTerm *vt, VTerm_ئاچقۇچ ئاچقۇچ, VTermModifier mod)
{
  إذا(ئاچقۇچ == VTERM_ئاچقۇچ_NONE)
    أرجع;

  keycodes_s k;
  إذا(ئاچقۇچ < VTERM_ئاچقۇچ_FUNCTION_0) {
    إذا(ئاچقۇچ >= حجم(keycodes)/حجم(keycodes[0]))
      أرجع;
    k = keycodes[ئاچقۇچ];
  }
  وإلا إذا(ئاچقۇچ >= VTERM_ئاچقۇچ_FUNCTION_0 && ئاچقۇچ <= VTERM_ئاچقۇچ_FUNCTION_MAX) {
    إذا((ئاچقۇچ - VTERM_ئاچقۇچ_FUNCTION_0) >= حجم(keycodes_fn)/حجم(keycodes_fn[0]))
      أرجع;
    k = keycodes_fn[ئاچقۇچ - VTERM_ئاچقۇچ_FUNCTION_0];
  }
  وإلا إذا(ئاچقۇچ >= VTERM_ئاچقۇچ_KP_0) {
    إذا((ئاچقۇچ - VTERM_ئاچقۇچ_KP_0) >= حجم(keycodes_kp)/حجم(keycodes_kp[0]))
      أرجع;
    k = keycodes_kp[ئاچقۇچ - VTERM_ئاچقۇچ_KP_0];
  }

  بدل(k.type) {
  حالة KEYCODE_NONE:
    اكسر;

  حالة KEYCODE_TAB:
    إذا (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    وإلا إذا(mod == VTERM_MOD_SHIFT)
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "Z");
    وإلا إذا(mod & VTERM_MOD_SHIFT)
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    وإلا
      اذهب case_LITERAL;
    اكسر;

  حالة KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    إذا(vt->state->mode.newline)
      vterm_ئىتتىرىش_output_sprintf(vt, "\r\n");
    وإلا
      اذهب case_LITERAL;
    اكسر;

  حالة KEYCODE_LITERAL: case_LITERAL:
    إذا (vterm_is_modify_other_ئاچقۇچلار(vt) && mod != 0)
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    وإلا إذا (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    وإلا إذا ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    وإلا
      vterm_ئىتتىرىش_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    اكسر;

  حالة KEYCODE_SS3: case_SS3:
    إذا(mod == 0)
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    وإلا
      اذهب case_CSI;
    اكسر;

  حالة KEYCODE_CSI: case_CSI:
    إذا(mod == 0)
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    وإلا
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    اكسر;

  حالة KEYCODE_CSINUM:
    إذا(mod == 0)
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    وإلا
      vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    اكسر;

  حالة KEYCODE_CSI_CURSOR:
    إذا(vt->state->mode.cursor)
      اذهب case_SS3;
    وإلا
      اذهب case_CSI;

  حالة KEYCODE_KEYPAD:
    إذا(vt->state->mode.keypad) {
      k.literal = k.csinum;
      اذهب case_SS3;
    }
    وإلا
      اذهب case_LITERAL;
  }
}

فراغ vterm_keyboard_start_paste(VTerm *vt)
{
  إذا(vt->state->mode.bracketpaste)
    vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

فراغ vterm_keyboard_end_paste(VTerm *vt)
{
  إذا(vt->state->mode.bracketpaste)
    vterm_ئىتتىرىش_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
