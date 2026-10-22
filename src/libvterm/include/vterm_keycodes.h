#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

પ્રકારવ્યાખ્યા ગણના {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
પ્રકારવ્યાખ્યા ગણના {
  VTERM_ચાવી_NONE,

  VTERM_ચાવી_ENTER,
  VTERM_ચાવી_TAB,
  VTERM_ચાવી_BACKSPACE,
  VTERM_ચાવી_ESCAPE,

  VTERM_ચાવી_UP,
  VTERM_ચાવી_DOWN,
  VTERM_ચાવી_ડાબું,
  VTERM_ચાવી_જમણું,

  VTERM_ચાવી_INS,
  VTERM_ચાવી_DEL,
  VTERM_ચાવી_HOME,
  VTERM_ચાવી_END,
  VTERM_ચાવી_PAGEUP,
  VTERM_ચાવી_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_ચાવી_FUNCTION_0   = 256,
  VTERM_ચાવી_FUNCTION_MAX = VTERM_ચાવી_FUNCTION_0 + 255,

  // keypad keys
  VTERM_ચાવી_KP_0,
  VTERM_ચાવી_KP_1,
  VTERM_ચાવી_KP_2,
  VTERM_ચાવી_KP_3,
  VTERM_ચાવી_KP_4,
  VTERM_ચાવી_KP_5,
  VTERM_ચાવી_KP_6,
  VTERM_ચાવી_KP_7,
  VTERM_ચાવી_KP_8,
  VTERM_ચાવી_KP_9,
  VTERM_ચાવી_KP_MULT,
  VTERM_ચાવી_KP_PLUS,
  VTERM_ચાવી_KP_COMMA,
  VTERM_ચાવી_KP_MINUS,
  VTERM_ચાવી_KP_PERIOD,
  VTERM_ચાવી_KP_DIVIDE,
  VTERM_ચાવી_KP_ENTER,
  VTERM_ચાવી_KP_EQUAL,

  VTERM_ચાવી_MAX, // Must be last
  VTERM_N_ચાવીઓ = VTERM_ચાવી_MAX
} VTerm_ચાવી;

#define VTERM_ચાવી_FUNCTION(n) (VTERM_ચાવી_FUNCTION_0+(n))

#endif
