#ଅନ୍ତର୍ଭାଵ "vterm_internal.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>

#ଅନ୍ତର୍ଭାଵ "utf8.h"

// VIM: added
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_is_modify_other_ଚାବିଗୁଡ଼ିକ(VTerm *vt)
{
  ଫେରନ୍ତୁ vt->state->mode.modify_other_ଚାବିଗୁଡ଼ିକ;
}

// VIM: added
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_is_kitty_keyboard(VTerm *vt)
{
  ଫେରନ୍ତୁ vt->state->mode.kitty_keyboard;
}


ଶୂନ୍ୟ vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  ଯଦି (vterm_is_modify_other_ଚାବିଗୁଡ଼ିକ(vt) && mod != 0) {
    vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    ଫେରନ୍ତୁ;
  }

  // VIM: added kitty keyboard protocol support
  ଯଦି (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ଫେରନ୍ତୁ;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  ଯଦି(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  ଯଦି(mod == 0) {
    // Normal text - ignore just shift
    ଅକ୍ଷର str[6];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା seqlen = fill_utf8(c, str);
    vterm_ଠେଲ_output_bytes(vt, str, seqlen);
    ଫେରନ୍ତୁ;
  }

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା needs_CSIu;
  ବାଛ(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    ସ୍ଥିତି 'i': ସ୍ଥିତି 'j': ସ୍ଥିତି 'm': ସ୍ଥିତି '[':
      needs_CSIu = 1;
      ଭାଙ୍ଗ;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    ସ୍ଥିତି '\\': ସ୍ଥିତି ']': ସ୍ଥିତି '^': ସ୍ଥିତି '_':
      needs_CSIu = 0;
      ଭାଙ୍ଗ;
    /* Shift-space needs CSIu */
    ସ୍ଥିତି ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      ଭାଙ୍ଗ;
    /* All other characters needs CSIu except for letters a-z */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  ଯଦି(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ଫେରନ୍ତୁ;
  }

  ଯଦି(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_ଠେଲ_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
  ଗଣନା {
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
  ଅକ୍ଷର literal;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
ସ୍ଥିର keycodes_s keycodes[] = {
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

ସ୍ଥିର keycodes_s keycodes_fn[] = {
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

ସ୍ଥିର keycodes_s keycodes_kp[] = {
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

ଶୂନ୍ୟ vterm_keyboard_ଚାବି(VTerm *vt, VTerm_ଚାବି ଚାବି, VTermModifier mod)
{
  ଯଦି(ଚାବି == VTERM_ଚାବି_NONE)
    ଫେରନ୍ତୁ;

  keycodes_s k;
  ଯଦି(ଚାବି < VTERM_ଚାବି_FUNCTION_0) {
    ଯଦି(ଚାବି >= ଆକାର(keycodes)/ଆକାର(keycodes[0]))
      ଫେରନ୍ତୁ;
    k = keycodes[ଚାବି];
  }
  ଅନ୍ୟଥା ଯଦି(ଚାବି >= VTERM_ଚାବି_FUNCTION_0 && ଚାବି <= VTERM_ଚାବି_FUNCTION_MAX) {
    ଯଦି((ଚାବି - VTERM_ଚାବି_FUNCTION_0) >= ଆକାର(keycodes_fn)/ଆକାର(keycodes_fn[0]))
      ଫେରନ୍ତୁ;
    k = keycodes_fn[ଚାବି - VTERM_ଚାବି_FUNCTION_0];
  }
  ଅନ୍ୟଥା ଯଦି(ଚାବି >= VTERM_ଚାବି_KP_0) {
    ଯଦି((ଚାବି - VTERM_ଚାବି_KP_0) >= ଆକାର(keycodes_kp)/ଆକାର(keycodes_kp[0]))
      ଫେରନ୍ତୁ;
    k = keycodes_kp[ଚାବି - VTERM_ଚାବି_KP_0];
  }

  ବାଛ(k.type) {
  ସ୍ଥିତି KEYCODE_NONE:
    ଭାଙ୍ଗ;

  ସ୍ଥିତି KEYCODE_TAB:
    ଯଦି (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    ଅନ୍ୟଥା ଯଦି(mod == VTERM_MOD_SHIFT)
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "Z");
    ଅନ୍ୟଥା ଯଦି(mod & VTERM_MOD_SHIFT)
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    ଅନ୍ୟଥା
      ଯାଅ case_LITERAL;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    ଯଦି(vt->state->mode.newline)
      vterm_ଠେଲ_output_sprintf(vt, "\r\n");
    ଅନ୍ୟଥା
      ଯାଅ case_LITERAL;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି KEYCODE_LITERAL: case_LITERAL:
    ଯଦି (vterm_is_modify_other_ଚାବିଗୁଡ଼ିକ(vt) && mod != 0)
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    ଅନ୍ୟଥା ଯଦି (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    ଅନ୍ୟଥା ଯଦି ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    ଅନ୍ୟଥା
      vterm_ଠେଲ_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି KEYCODE_SS3: case_SS3:
    ଯଦି(mod == 0)
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    ଅନ୍ୟଥା
      ଯାଅ case_CSI;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି KEYCODE_CSI: case_CSI:
    ଯଦି(mod == 0)
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    ଅନ୍ୟଥା
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି KEYCODE_CSINUM:
    ଯଦି(mod == 0)
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    ଅନ୍ୟଥା
      vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି KEYCODE_CSI_CURSOR:
    ଯଦି(vt->state->mode.cursor)
      ଯାଅ case_SS3;
    ଅନ୍ୟଥା
      ଯାଅ case_CSI;

  ସ୍ଥିତି KEYCODE_KEYPAD:
    ଯଦି(vt->state->mode.keypad) {
      k.literal = k.csinum;
      ଯାଅ case_SS3;
    }
    ଅନ୍ୟଥା
      ଯାଅ case_LITERAL;
  }
}

ଶୂନ୍ୟ vterm_keyboard_start_paste(VTerm *vt)
{
  ଯଦି(vt->state->mode.bracketpaste)
    vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

ଶୂନ୍ୟ vterm_keyboard_end_paste(VTerm *vt)
{
  ଯଦି(vt->state->mode.bracketpaste)
    vterm_ଠେଲ_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
