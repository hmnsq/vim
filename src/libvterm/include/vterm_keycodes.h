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
  VTERM_कुञ्चिका_NONE,

  VTERM_कुञ्चिका_ENTER,
  VTERM_कुञ्चिका_TAB,
  VTERM_कुञ्चिका_BACKSPACE,
  VTERM_कुञ्चिका_ESCAPE,

  VTERM_कुञ्चिका_UP,
  VTERM_कुञ्चिका_DOWN,
  VTERM_कुञ्चिका_वामः,
  VTERM_कुञ्चिका_दक्षिणः,

  VTERM_कुञ्चिका_INS,
  VTERM_कुञ्चिका_DEL,
  VTERM_कुञ्चिका_HOME,
  VTERM_कुञ्चिका_END,
  VTERM_कुञ्चिका_PAGEUP,
  VTERM_कुञ्चिका_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_कुञ्चिका_FUNCTION_0   = 256,
  VTERM_कुञ्चिका_FUNCTION_MAX = VTERM_कुञ्चिका_FUNCTION_0 + 255,

  // keypad keys
  VTERM_कुञ्चिका_KP_0,
  VTERM_कुञ्चिका_KP_1,
  VTERM_कुञ्चिका_KP_2,
  VTERM_कुञ्चिका_KP_3,
  VTERM_कुञ्चिका_KP_4,
  VTERM_कुञ्चिका_KP_5,
  VTERM_कुञ्चिका_KP_6,
  VTERM_कुञ्चिका_KP_7,
  VTERM_कुञ्चिका_KP_8,
  VTERM_कुञ्चिका_KP_9,
  VTERM_कुञ्चिका_KP_MULT,
  VTERM_कुञ्चिका_KP_PLUS,
  VTERM_कुञ्चिका_KP_COMMA,
  VTERM_कुञ्चिका_KP_MINUS,
  VTERM_कुञ्चिका_KP_PERIOD,
  VTERM_कुञ्चिका_KP_DIVIDE,
  VTERM_कुञ्चिका_KP_ENTER,
  VTERM_कुञ्चिका_KP_EQUAL,

  VTERM_कुञ्चिका_MAX, // Must be last
  VTERM_N_कुञ्चिकाः = VTERM_कुञ्चिका_MAX
} VTerm_कुञ्चिका;

#define VTERM_कुञ्चिका_FUNCTION(n) (VTERM_कुञ्चिका_FUNCTION_0+(n))

#endif
