#include "vterm_internal.h"

#include <stdio.h>

#include "utf8.h"

// VIM: added
puur_naa_nka vterm_is_modify_other_keys(VTerm *vt)
{
  ᯀᯮᯞ vt->state->mode.modify_other_keys;
}

// VIM: added
puur_naa_nka vterm_is_kitty_keyboard(VTerm *vt)
{
  ᯀᯮᯞ vt->state->mode.kitty_keyboard;
}


void vterm_keyboard_unichar(VTerm *vt, uint32_t c, VTermModifier mod)
{
  // VIM: added modifyOtherKeys support
  ᯀᯮᯝ (vterm_is_modify_other_keys(vt) && mod != 0) {
    vterm_push_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, c);
    ᯀᯮᯞ;
  }

  // VIM: added kitty keyboard protocol support
  ᯀᯮᯝ (vterm_is_kitty_keyboard(vt) && mod != 0) {
    vterm_push_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ᯀᯮᯞ;
  }

  /* The shift modifier is never important for Unicode characters
   * apart from Space
   */
  ᯀᯮᯝ(c != ' ')
    mod &= ~VTERM_MOD_SHIFT;

  ᯀᯮᯝ(mod == 0) {
    // Normal text - ignore just shift
    var_na str[6];
    puur_naa_nka seqlen = fill_utf8(c, str);
    vterm_push_output_bytes(vt, str, seqlen);
    ᯀᯮᯞ;
  }

  puur_naa_nka needs_CSIu;
  switch(c) {
    /* Special Ctrl- letters that can't be represented elsewise */
    pak_sa 'i': pak_sa 'j': pak_sa 'm': pak_sa '[':
      needs_CSIu = 1;
      bha_nga;
    /* Ctrl-\ ] ^ _ don't need CSUu */
    pak_sa '\\': pak_sa ']': pak_sa '^': pak_sa '_':
      needs_CSIu = 0;
      bha_nga;
    /* Shift-space needs CSIu */
    pak_sa ' ':
      needs_CSIu = !!(mod & VTERM_MOD_SHIFT);
      bha_nga;
    /* All other characters needs CSIu except for letters a-z */
    default:
      needs_CSIu = (c < 'a' || c > 'z');
  }

  /* ALT we can just prefix with ESC; anything else requires CSI u */
  ᯀᯮᯝ(needs_CSIu && (mod & ~VTERM_MOD_ALT)) {
    vterm_push_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", c, mod+1);
    ᯀᯮᯞ;
  }

  ᯀᯮᯝ(mod & VTERM_MOD_CTRL)
    c &= 0x1f;

  vterm_push_output_sprintf(vt, "%s%c", mod & VTERM_MOD_ALT ? ESC_S : "", c);
}

typedef sa_nracanaa {
  ga_nanaa {
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
  var_na literal;
  puur_naa_nka csinum;
} keycodes_s;

// Order here must be exactly the same as VTermKey enum!
static keycodes_s keycodes[] = {
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

static keycodes_s keycodes_fn[] = {
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

static keycodes_s keycodes_kp[] = {
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

void vterm_keyboard_key(VTerm *vt, VTermKey key, VTermModifier mod)
{
  ᯀᯮᯝ(key == VTERM_KEY_NONE)
    ᯀᯮᯞ;

  keycodes_s k;
  ᯀᯮᯝ(key < VTERM_KEY_FUNCTION_0) {
    ᯀᯮᯝ(key >= parimaa_na(keycodes)/parimaa_na(keycodes[0]))
      ᯀᯮᯞ;
    k = keycodes[key];
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(key >= VTERM_KEY_FUNCTION_0 && key <= VTERM_KEY_FUNCTION_MAX) {
    ᯀᯮᯝ((key - VTERM_KEY_FUNCTION_0) >= parimaa_na(keycodes_fn)/parimaa_na(keycodes_fn[0]))
      ᯀᯮᯞ;
    k = keycodes_fn[key - VTERM_KEY_FUNCTION_0];
  }
  ᯀᯮᯝᯂᯪ ᯀᯮᯝ(key >= VTERM_KEY_KP_0) {
    ᯀᯮᯝ((key - VTERM_KEY_KP_0) >= parimaa_na(keycodes_kp)/parimaa_na(keycodes_kp[0]))
      ᯀᯮᯞ;
    k = keycodes_kp[key - VTERM_KEY_KP_0];
  }

  switch(k.type) {
  pak_sa KEYCODE_NONE:
    bha_nga;

  pak_sa KEYCODE_TAB:
    ᯀᯮᯝ (vterm_is_kitty_keyboard(vt) && mod != 0)
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "9;%du", mod+1);
    /* Shift-Tab is CSI Z but plain Tab is 0x09 */
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(mod == VTERM_MOD_SHIFT)
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "Z");
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ(mod & VTERM_MOD_SHIFT)
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "1;%dZ", mod+1);
    ᯀᯮᯝᯂᯪ
      goto case_LITERAL;
    bha_nga;

  pak_sa KEYCODE_ENTER:
    /* Enter is CRLF in newline mode, but just LF in linefeed */
    ᯀᯮᯝ(vt->state->mode.newline)
      vterm_push_output_sprintf(vt, "\r\n");
    ᯀᯮᯝᯂᯪ
      goto case_LITERAL;
    bha_nga;

  pak_sa KEYCODE_LITERAL: case_LITERAL:
    ᯀᯮᯝ (vterm_is_modify_other_keys(vt) && mod != 0)
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "27;%d;%d~", mod+1, k.literal);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ (vterm_is_kitty_keyboard(vt) && mod == 0 && k.literal == '\x1b')
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "%du", k.literal);
    ᯀᯮᯝᯂᯪ ᯀᯮᯝ ((vterm_is_kitty_keyboard(vt) && mod != 0)
	|| (mod & (VTERM_MOD_SHIFT|VTERM_MOD_CTRL)))
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "%d;%du", k.literal, mod+1);
    ᯀᯮᯝᯂᯪ
      vterm_push_output_sprintf(vt, mod & VTERM_MOD_ALT ? ESC_S "%c" : "%c", k.literal);
    bha_nga;

  pak_sa KEYCODE_SS3: case_SS3:
    ᯀᯮᯝ(mod == 0)
      vterm_push_output_sprintf_ctrl(vt, C1_SS3, "%c", k.literal);
    ᯀᯮᯝᯂᯪ
      goto case_CSI;
    bha_nga;

  pak_sa KEYCODE_CSI: case_CSI:
    ᯀᯮᯝ(mod == 0)
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "%c", k.literal);
    ᯀᯮᯝᯂᯪ
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "1;%d%c", mod + 1, k.literal);
    bha_nga;

  pak_sa KEYCODE_CSINUM:
    ᯀᯮᯝ(mod == 0)
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "%d%c", k.csinum, k.literal);
    ᯀᯮᯝᯂᯪ
      vterm_push_output_sprintf_ctrl(vt, C1_CSI, "%d;%d%c", k.csinum, mod + 1, k.literal);
    bha_nga;

  pak_sa KEYCODE_CSI_CURSOR:
    ᯀᯮᯝ(vt->state->mode.cursor)
      goto case_SS3;
    ᯀᯮᯝᯂᯪ
      goto case_CSI;

  pak_sa KEYCODE_KEYPAD:
    ᯀᯮᯝ(vt->state->mode.keypad) {
      k.literal = k.csinum;
      goto case_SS3;
    }
    ᯀᯮᯝᯂᯪ
      goto case_LITERAL;
  }
}

void vterm_keyboard_start_paste(VTerm *vt)
{
  ᯀᯮᯝ(vt->state->mode.bracketpaste)
    vterm_push_output_sprintf_ctrl(vt, C1_CSI, "200~");
}

void vterm_keyboard_end_paste(VTerm *vt)
{
  ᯀᯮᯝ(vt->state->mode.bracketpaste)
    vterm_push_output_sprintf_ctrl(vt, C1_CSI, "201~");
}
