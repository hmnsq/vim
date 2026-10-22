#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

// VIM: added
ਪੂਰਨ vterm_is_modify_other_ਕੁੰਜੀਆਂ(VTerm *vt)
{
  ਵਾਪਸ vt->state->ਬਹੁਲਕ.modify_other_ਕੁੰਜੀਆਂ;
}

// VIM: added
ਪੂਰਨ vterm_is_kitty_keyboard(VTerm *vt)
{
  ਵਾਪਸ vt->state->ਬਹੁਲਕ.kitty_keyboard;
}


ਖਾਲੀ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  ਜੇ (vterm_is_modify_other_ਕੁੰਜੀਆਂ(vt) && mod != 0) {
    vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    ਵਾਪਸ;
  }

  // VIM: added kitty keyboard protocol support
  ਜੇ (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ਵਾਪਸ;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  ਜੇ(c != ' ')
    mod &= ~VTERM_MOD_ਖਿਸਕਾਓ;

  ਜੇ(mod == 0) {
    // Normal text - ignore just shift
    ਅੱਖਰ str[6];
    ਪੂਰਨ seqlen = fill_utf8(c, str);
    vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(vt, str, seqlen);
    ਵਾਪਸ;
  }

  ਪੂਰਨ needs_CSIu;
  ਚੋਣ(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    ਹਾਲਤ 'i': ਹਾਲਤ 'j': ਹਾਲਤ 'm': ਹਾਲਤ '[':
      needs_CSIu = 1;
      ਰੋਕੋ;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    ਹਾਲਤ '\\': ਹਾਲਤ ']': ਹਾਲਤ '^': ਹਾਲਤ '_':
      needs_CSIu = 0;
      ਰੋਕੋ;
    /* Shift-space needs CSIu */
    ਹਾਲਤ ' ':
      needs_CSIu = !!(mod & VTERM_MOD_ਖਿਸਕਾਓ);
      ਰੋਕੋ;
    /* All other characters needs CSIu except for letters a-z */
    ਮੂਲ:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  ਜੇ(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ਵਾਪਸ;
  }

  ਜੇ(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ਧੱਕੋ_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
  ਗਿਣਤੀ {
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
  ਅੱਖਰ literal;
  ਪੂਰਨ csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
ਸਥਿਰ keycodes_s keycodes[] = {
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

ਸਥਿਰ keycodes_s keycodes_fn[] = {
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

ਸਥਿਰ keycodes_s keycodes_kp[] = {
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

ਖਾਲੀ vterm_keyboard_ਕੁੰਜੀ(VTerm *vt, VTerm_ਕੁੰਜੀ ਕੁੰਜੀ, VTermModifier mod)
{
  ਜੇ(ਕੁੰਜੀ == VTERM_ਕੁੰਜੀ_NONE)
    ਵਾਪਸ;

  keycodes_s k;
  ਜੇ(ਕੁੰਜੀ < VTERM_ਕੁੰਜੀ_FUNCTION_0) {
    ਜੇ(ਕੁੰਜੀ >= ਆਕਾਰ(keycodes)/ਆਕਾਰ(keycodes[0]))
      ਵਾਪਸ;
    k = keycodes[ਕੁੰਜੀ];
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਕੁੰਜੀ >= VTERM_ਕੁੰਜੀ_FUNCTION_0 && ਕੁੰਜੀ <= VTERM_ਕੁੰਜੀ_FUNCTION_MAX) {
    ਜੇ((ਕੁੰਜੀ - VTERM_ਕੁੰਜੀ_FUNCTION_0) >= ਆਕਾਰ(keycodes_fn)/ਆਕਾਰ(keycodes_fn[0]))
      ਵਾਪਸ;
    k = keycodes_fn[ਕੁੰਜੀ - VTERM_ਕੁੰਜੀ_FUNCTION_0];
  }
  ਨਹੀਂਤਾਂ ਜੇ(ਕੁੰਜੀ >= VTERM_ਕੁੰਜੀ_KP_0) {
    ਜੇ((ਕੁੰਜੀ - VTERM_ਕੁੰਜੀ_KP_0) >= ਆਕਾਰ(keycodes_kp)/ਆਕਾਰ(keycodes_kp[0]))
      ਵਾਪਸ;
    k = keycodes_kp[ਕੁੰਜੀ - VTERM_ਕੁੰਜੀ_KP_0];
  }

  ਚੋਣ(k.type) {
  ਹਾਲਤ KEYCODE_NONE:
    ਰੋਕੋ;

  ਹਾਲਤ KEYCODE_TAB:
    ਜੇ (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    ਨਹੀਂਤਾਂ ਜੇ(mod == VTERM_MOD_ਖਿਸਕਾਓ)
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "Z");
    ਨਹੀਂਤਾਂ ਜੇ(mod & VTERM_MOD_ਖਿਸਕਾਓ)
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    ਨਹੀਂਤਾਂ
      ਜਾਓ case_LITERAL;
    ਰੋਕੋ;

  ਹਾਲਤ KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    ਜੇ(vt->state->ਬਹੁਲਕ.newline)
      vterm_ਧੱਕੋ_output_sprintf(vt, "\r\n");
    ਨਹੀਂਤਾਂ
      ਜਾਓ case_LITERAL;
    ਰੋਕੋ;

  ਹਾਲਤ KEYCODE_LITERAL: case_LITERAL:
    ਜੇ (vterm_is_modify_other_ਕੁੰਜੀਆਂ(vt) && mod != 0)
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    ਨਹੀਂਤਾਂ ਜੇ (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    ਨਹੀਂਤਾਂ ਜੇ ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_ਖਿਸਕਾਓ|VTERM_MOD_CTRL)))
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    ਨਹੀਂਤਾਂ
      vterm_ਧੱਕੋ_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    ਰੋਕੋ;

  ਹਾਲਤ KEYCODE_SS3: case_SS3:
    ਜੇ(mod == 0)
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    ਨਹੀਂਤਾਂ
      ਜਾਓ case_CSI;
    ਰੋਕੋ;

  ਹਾਲਤ KEYCODE_CSI: case_CSI:
    ਜੇ(mod == 0)
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    ਨਹੀਂਤਾਂ
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    ਰੋਕੋ;

  ਹਾਲਤ KEYCODE_CSINUM:
    ਜੇ(mod == 0)
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    ਨਹੀਂਤਾਂ
      vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    ਰੋਕੋ;

  ਹਾਲਤ KEYCODE_CSI_CURSOR:
    ਜੇ(vt->state->ਬਹੁਲਕ.cursor)
      ਜਾਓ case_SS3;
    ਨਹੀਂਤਾਂ
      ਜਾਓ case_CSI;

  ਹਾਲਤ KEYCODE_KEYPAD:
    ਜੇ(vt->state->ਬਹੁਲਕ.keypad) {
      k.literal = k.csinum;
      ਜਾਓ case_SS3;
    }
    ਨਹੀਂਤਾਂ
      ਜਾਓ case_LITERAL;
  }
}

ਖਾਲੀ vterm_keyboard_ਸ਼ੁਰੂ_paste(VTerm *vt)
{
  ਜੇ(vt->state->ਬਹੁਲਕ.bracketpaste)
    vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

ਖਾਲੀ vterm_keyboard_ਸਿਰਾ_paste(VTerm *vt)
{
  ਜੇ(vt->state->ਬਹੁਲਕ.bracketpaste)
    vterm_ਧੱਕੋ_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
