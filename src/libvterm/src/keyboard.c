#அந்தர்பாவ "vterm_internal.h"

#அந்தர்பாவ <stdio.h>

#அந்தர்பாவ "utf8.h"

// VIM: added
முழு vterm_is_modify_other_சாவிகள்(VTerm *vt)
{
  திரும்பு vt->state->mode.modify_other_சாவிகள்;
}

// VIM: added
முழு vterm_is_kitty_keyboard(VTerm *vt)
{
  திரும்பு vt->state->mode.kitty_keyboard;
}


வெறுமை vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  எனில் (vterm_is_modify_other_சாவிகள்(vt) && mod != 0) {
    vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    திரும்பு;
  }

  // VIM: added kitty keyboard protocol support
  எனில் (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    திரும்பு;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  எனில்(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  எனில்(mod == 0) {
    // Normal text - ignore just shift
    எழுத்து str[6];
    முழு seqlen = fill_utf8(c, str);
    vterm_தள்ளு_output_bytes(vt, str, seqlen);
    திரும்பு;
  }

  முழு needs_CSIu;
  தேர்வு(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    நிலை 'i': நிலை 'j': நிலை 'm': நிலை '[':
      needs_CSIu = 1;
      நிறுத்து;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    நிலை '\\': நிலை ']': நிலை '^': நிலை '_':
      needs_CSIu = 0;
      நிறுத்து;
    /* Shift-space needs CSIu */
    நிலை ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      நிறுத்து;
    /* All other characters needs CSIu except for letters a-z */
    இயல்பு:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  எனில்(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    திரும்பு;
  }

  எனில்(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_தள்ளு_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

வகைவரை கட்டமைப்பு {
  எண்ணிக்கை {
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
  எழுத்து literal;
  முழு csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
நிலையான keycodes_s keycodes[] = {
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

நிலையான keycodes_s keycodes_fn[] = {
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

நிலையான keycodes_s keycodes_kp[] = {
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

வெறுமை vterm_keyboard_சாவி(VTerm *vt, VTerm_சாவி சாவி, VTermModifier mod)
{
  எனில்(சாவி == VTERM_சாவி_NONE)
    திரும்பு;

  keycodes_s k;
  எனில்(சாவி < VTERM_சாவி_FUNCTION_0) {
    எனில்(சாவி >= அளவு(keycodes)/அளவு(keycodes[0]))
      திரும்பு;
    k = keycodes[சாவி];
  }
  இல்லையெனில் எனில்(சாவி >= VTERM_சாவி_FUNCTION_0 && சாவி <= VTERM_சாவி_FUNCTION_MAX) {
    எனில்((சாவி - VTERM_சாவி_FUNCTION_0) >= அளவு(keycodes_fn)/அளவு(keycodes_fn[0]))
      திரும்பு;
    k = keycodes_fn[சாவி - VTERM_சாவி_FUNCTION_0];
  }
  இல்லையெனில் எனில்(சாவி >= VTERM_சாவி_KP_0) {
    எனில்((சாவி - VTERM_சாவி_KP_0) >= அளவு(keycodes_kp)/அளவு(keycodes_kp[0]))
      திரும்பு;
    k = keycodes_kp[சாவி - VTERM_சாவி_KP_0];
  }

  தேர்வு(k.type) {
  நிலை KEYCODE_NONE:
    நிறுத்து;

  நிலை KEYCODE_TAB:
    எனில் (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    இல்லையெனில் எனில்(mod == VTERM_MOD_SHIFT)
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "Z");
    இல்லையெனில் எனில்(mod & VTERM_MOD_SHIFT)
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    இல்லையெனில்
      செல் case_LITERAL;
    நிறுத்து;

  நிலை KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    எனில்(vt->state->mode.newline)
      vterm_தள்ளு_output_sprintf(vt, "\r\n");
    இல்லையெனில்
      செல் case_LITERAL;
    நிறுத்து;

  நிலை KEYCODE_LITERAL: case_LITERAL:
    எனில் (vterm_is_modify_other_சாவிகள்(vt) && mod != 0)
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    இல்லையெனில் எனில் (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    இல்லையெனில் எனில் ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    இல்லையெனில்
      vterm_தள்ளு_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    நிறுத்து;

  நிலை KEYCODE_SS3: case_SS3:
    எனில்(mod == 0)
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    இல்லையெனில்
      செல் case_CSI;
    நிறுத்து;

  நிலை KEYCODE_CSI: case_CSI:
    எனில்(mod == 0)
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    இல்லையெனில்
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    நிறுத்து;

  நிலை KEYCODE_CSINUM:
    எனில்(mod == 0)
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    இல்லையெனில்
      vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    நிறுத்து;

  நிலை KEYCODE_CSI_CURSOR:
    எனில்(vt->state->mode.cursor)
      செல் case_SS3;
    இல்லையெனில்
      செல் case_CSI;

  நிலை KEYCODE_KEYPAD:
    எனில்(vt->state->mode.keypad) {
      k.literal = k.csinum;
      செல் case_SS3;
    }
    இல்லையெனில்
      செல் case_LITERAL;
  }
}

வெறுமை vterm_keyboard_start_paste(VTerm *vt)
{
  எனில்(vt->state->mode.bracketpaste)
    vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

வெறுமை vterm_keyboard_end_paste(VTerm *vt)
{
  எனில்(vt->state->mode.bracketpaste)
    vterm_தள்ளு_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
