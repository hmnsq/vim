#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

प्रकारनिर्देश गणना {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
प्रकारनिर्देश गणना {
  VTERM_किल्ली_NONE,

  VTERM_किल्ली_ENTER,
  VTERM_किल्ली_TAB,
  VTERM_किल्ली_BACKSPACE,
  VTERM_किल्ली_ESCAPE,

  VTERM_किल्ली_UP,
  VTERM_किल्ली_DOWN,
  VTERM_किल्ली_डावा,
  VTERM_किल्ली_उजवा,

  VTERM_किल्ली_INS,
  VTERM_किल्ली_DEL,
  VTERM_किल्ली_HOME,
  VTERM_किल्ली_END,
  VTERM_किल्ली_PAGEUP,
  VTERM_किल्ली_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_किल्ली_FUNCTION_0   = 256,
  VTERM_किल्ली_FUNCTION_MAX = VTERM_किल्ली_FUNCTION_0 + 255,

  // keypad keys
  VTERM_किल्ली_KP_0,
  VTERM_किल्ली_KP_1,
  VTERM_किल्ली_KP_2,
  VTERM_किल्ली_KP_3,
  VTERM_किल्ली_KP_4,
  VTERM_किल्ली_KP_5,
  VTERM_किल्ली_KP_6,
  VTERM_किल्ली_KP_7,
  VTERM_किल्ली_KP_8,
  VTERM_किल्ली_KP_9,
  VTERM_किल्ली_KP_MULT,
  VTERM_किल्ली_KP_PLUS,
  VTERM_किल्ली_KP_COMMA,
  VTERM_किल्ली_KP_MINUS,
  VTERM_किल्ली_KP_PERIOD,
  VTERM_किल्ली_KP_DIVIDE,
  VTERM_किल्ली_KP_ENTER,
  VTERM_किल्ली_KP_EQUAL,

  VTERM_किल्ली_MAX, // Must be last
  VTERM_N_किल्ल्या = VTERM_किल्ली_MAX
} VTerm_किल्ली;

#define VTERM_किल्ली_FUNCTION(n) (VTERM_किल्ली_FUNCTION_0+(n))

#endif
