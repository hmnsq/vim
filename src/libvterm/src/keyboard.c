#అన్తర్భావ "vterm_internal.h"

#అన్తర్భావ <stdio.h>

#అన్తర్భావ "utf8.h"

// VIM: added
పూర్ణాంకం vterm_is_modify_other_కీలకాలు(VTerm *vt)
{
  తిరిగి vt->state->బహుళకం.modify_other_కీలకాలు;
}

// VIM: added
పూర్ణాంకం vterm_is_kitty_keyboard(VTerm *vt)
{
  తిరిగి vt->state->బహుళకం.kitty_keyboard;
}


శూన్య vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  ఒకవేళ (vterm_is_modify_other_కీలకాలు(vt) && mod != 0) {
    vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    తిరిగి;
  }

  // VIM: added kitty keyboard protocol support
  ఒకవేళ (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    తిరిగి;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  ఒకవేళ(c != ' ')
    mod &= ~VTERM_MOD_జరుపు;

  ఒకవేళ(mod == 0) {
    // Normal text - ignore just shift
    అక్షరం str[6];
    పూర్ణాంకం seqlen = fill_utf8(c, str);
    vterm_నెట్టు_output_బైట్లు(vt, str, seqlen);
    తిరిగి;
  }

  పూర్ణాంకం needs_CSIu;
  ఎంపిక(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    స్థితి 'i': స్థితి 'j': స్థితి 'm': స్థితి '[':
      needs_CSIu = 1;
      ఆపు;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    స్థితి '\\': స్థితి ']': స్థితి '^': స్థితి '_':
      needs_CSIu = 0;
      ఆపు;
    /* Shift-space needs CSIu */
    స్థితి ' ':
      needs_CSIu = !!(mod & VTERM_MOD_జరుపు);
      ఆపు;
    /* All other characters needs CSIu except for letters a-z */
    అప్రమేయ:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  ఒకవేళ(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    తిరిగి;
  }

  ఒకవేళ(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_నెట్టు_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

రకనిర్వచనం నిర్మాణం {
  గణన {
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
  అక్షరం literal;
  పూర్ణాంకం csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
స్థిర keycodes_s keycodes[] = {
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

స్థిర keycodes_s keycodes_fn[] = {
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

స్థిర keycodes_s keycodes_kp[] = {
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

శూన్య vterm_keyboard_కీలకం(VTerm *vt, VTerm_కీలకం కీలకం, VTermModifier mod)
{
  ఒకవేళ(కీలకం == VTERM_కీలకం_NONE)
    తిరిగి;

  keycodes_s k;
  ఒకవేళ(కీలకం < VTERM_కీలకం_FUNCTION_0) {
    ఒకవేళ(కీలకం >= పరిమాణం(keycodes)/పరిమాణం(keycodes[0]))
      తిరిగి;
    k = keycodes[కీలకం];
  }
  లేకపోతే ఒకవేళ(కీలకం >= VTERM_కీలకం_FUNCTION_0 && కీలకం <= VTERM_కీలకం_FUNCTION_MAX) {
    ఒకవేళ((కీలకం - VTERM_కీలకం_FUNCTION_0) >= పరిమాణం(keycodes_fn)/పరిమాణం(keycodes_fn[0]))
      తిరిగి;
    k = keycodes_fn[కీలకం - VTERM_కీలకం_FUNCTION_0];
  }
  లేకపోతే ఒకవేళ(కీలకం >= VTERM_కీలకం_KP_0) {
    ఒకవేళ((కీలకం - VTERM_కీలకం_KP_0) >= పరిమాణం(keycodes_kp)/పరిమాణం(keycodes_kp[0]))
      తిరిగి;
    k = keycodes_kp[కీలకం - VTERM_కీలకం_KP_0];
  }

  ఎంపిక(k.type) {
  స్థితి KEYCODE_NONE:
    ఆపు;

  స్థితి KEYCODE_TAB:
    ఒకవేళ (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    లేకపోతే ఒకవేళ(mod == VTERM_MOD_జరుపు)
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "Z");
    లేకపోతే ఒకవేళ(mod & VTERM_MOD_జరుపు)
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    లేకపోతే
      వెళ్ళు case_LITERAL;
    ఆపు;

  స్థితి KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    ఒకవేళ(vt->state->బహుళకం.newline)
      vterm_నెట్టు_output_sprintf(vt, "\r\n");
    లేకపోతే
      వెళ్ళు case_LITERAL;
    ఆపు;

  స్థితి KEYCODE_LITERAL: case_LITERAL:
    ఒకవేళ (vterm_is_modify_other_కీలకాలు(vt) && mod != 0)
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    లేకపోతే ఒకవేళ (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    లేకపోతే ఒకవేళ ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_జరుపు|VTERM_MOD_CTRL)))
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    లేకపోతే
      vterm_నెట్టు_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    ఆపు;

  స్థితి KEYCODE_SS3: case_SS3:
    ఒకవేళ(mod == 0)
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    లేకపోతే
      వెళ్ళు case_CSI;
    ఆపు;

  స్థితి KEYCODE_CSI: case_CSI:
    ఒకవేళ(mod == 0)
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    లేకపోతే
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    ఆపు;

  స్థితి KEYCODE_CSINUM:
    ఒకవేళ(mod == 0)
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    లేకపోతే
      vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    ఆపు;

  స్థితి KEYCODE_CSI_CURSOR:
    ఒకవేళ(vt->state->బహుళకం.cursor)
      వెళ్ళు case_SS3;
    లేకపోతే
      వెళ్ళు case_CSI;

  స్థితి KEYCODE_KEYPAD:
    ఒకవేళ(vt->state->బహుళకం.keypad) {
      k.literal = k.csinum;
      వెళ్ళు case_SS3;
    }
    లేకపోతే
      వెళ్ళు case_LITERAL;
  }
}

శూన్య vterm_keyboard_ప్రారంభం_paste(VTerm *vt)
{
  ఒకవేళ(vt->state->బహుళకం.bracketpaste)
    vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

శూన్య vterm_keyboard_చివర_paste(VTerm *vt)
{
  ఒకవేళ(vt->state->బహుళకం.bracketpaste)
    vterm_నెట్టు_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
