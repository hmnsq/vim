#અન્તર્ભાવ "vterm_internal.h"

#અન્તર્ભાવ <stdio.h>

#અન્તર્ભાવ "utf8.h"

// VIM: added
પૂર્ણાંક vterm_is_modify_other_ચાવીઓ(VTerm *vt)
{
  પાછા vt->state->mode.modify_other_ચાવીઓ;
}

// VIM: added
પૂર્ણાંક vterm_is_kitty_keyboard(VTerm *vt)
{
  પાછા vt->state->mode.kitty_keyboard;
}


શૂન્ય vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  જો (vterm_is_modify_other_ચાવીઓ(vt) && mod != 0) {
    vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    પાછા;
  }

  // VIM: added kitty keyboard protocol support
  જો (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    પાછા;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  જો(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  જો(mod == 0) {
    // Normal text - ignore just shift
    અક્ષર str[6];
    પૂર્ણાંક seqlen = fill_utf8(c, str);
    vterm_ધકેલો_output_bytes(vt, str, seqlen);
    પાછા;
  }

  પૂર્ણાંક needs_CSIu;
  પસંદગી(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    સ્થિતિ 'i': સ્થિતિ 'j': સ્થિતિ 'm': સ્થિતિ '[':
      needs_CSIu = 1;
      તોડો;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    સ્થિતિ '\\': સ્થિતિ ']': સ્થિતિ '^': સ્થિતિ '_':
      needs_CSIu = 0;
      તોડો;
    /* Shift-space needs CSIu */
    સ્થિતિ ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      તોડો;
    /* All other characters needs CSIu except for letters a-z */
    મૂળભૂત:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  જો(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    પાછા;
  }

  જો(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ધકેલો_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

પ્રકારવ્યાખ્યા સંરચના {
  ગણના {
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
  અક્ષર literal;
  પૂર્ણાંક csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
સ્થિર keycodes_s keycodes[] = {
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

સ્થિર keycodes_s keycodes_fn[] = {
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

સ્થિર keycodes_s keycodes_kp[] = {
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

શૂન્ય vterm_keyboard_ચાવી(VTerm *vt, VTerm_ચાવી ચાવી, VTermModifier mod)
{
  જો(ચાવી == VTERM_ચાવી_NONE)
    પાછા;

  keycodes_s k;
  જો(ચાવી < VTERM_ચાવી_FUNCTION_0) {
    જો(ચાવી >= કદ(keycodes)/કદ(keycodes[0]))
      પાછા;
    k = keycodes[ચાવી];
  }
  નહીંતર જો(ચાવી >= VTERM_ચાવી_FUNCTION_0 && ચાવી <= VTERM_ચાવી_FUNCTION_MAX) {
    જો((ચાવી - VTERM_ચાવી_FUNCTION_0) >= કદ(keycodes_fn)/કદ(keycodes_fn[0]))
      પાછા;
    k = keycodes_fn[ચાવી - VTERM_ચાવી_FUNCTION_0];
  }
  નહીંતર જો(ચાવી >= VTERM_ચાવી_KP_0) {
    જો((ચાવી - VTERM_ચાવી_KP_0) >= કદ(keycodes_kp)/કદ(keycodes_kp[0]))
      પાછા;
    k = keycodes_kp[ચાવી - VTERM_ચાવી_KP_0];
  }

  પસંદગી(k.type) {
  સ્થિતિ KEYCODE_NONE:
    તોડો;

  સ્થિતિ KEYCODE_TAB:
    જો (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    નહીંતર જો(mod == VTERM_MOD_SHIFT)
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "Z");
    નહીંતર જો(mod & VTERM_MOD_SHIFT)
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    નહીંતર
      જાઓ case_LITERAL;
    તોડો;

  સ્થિતિ KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    જો(vt->state->mode.newline)
      vterm_ધકેલો_output_sprintf(vt, "\r\n");
    નહીંતર
      જાઓ case_LITERAL;
    તોડો;

  સ્થિતિ KEYCODE_LITERAL: case_LITERAL:
    જો (vterm_is_modify_other_ચાવીઓ(vt) && mod != 0)
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    નહીંતર જો (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    નહીંતર જો ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    નહીંતર
      vterm_ધકેલો_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    તોડો;

  સ્થિતિ KEYCODE_SS3: case_SS3:
    જો(mod == 0)
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    નહીંતર
      જાઓ case_CSI;
    તોડો;

  સ્થિતિ KEYCODE_CSI: case_CSI:
    જો(mod == 0)
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    નહીંતર
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    તોડો;

  સ્થિતિ KEYCODE_CSINUM:
    જો(mod == 0)
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    નહીંતર
      vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    તોડો;

  સ્થિતિ KEYCODE_CSI_CURSOR:
    જો(vt->state->mode.cursor)
      જાઓ case_SS3;
    નહીંતર
      જાઓ case_CSI;

  સ્થિતિ KEYCODE_KEYPAD:
    જો(vt->state->mode.keypad) {
      k.literal = k.csinum;
      જાઓ case_SS3;
    }
    નહીંતર
      જાઓ case_LITERAL;
  }
}

શૂન્ય vterm_keyboard_start_paste(VTerm *vt)
{
  જો(vt->state->mode.bracketpaste)
    vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

શૂન્ય vterm_keyboard_end_paste(VTerm *vt)
{
  જો(vt->state->mode.bracketpaste)
    vterm_ધકેલો_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
