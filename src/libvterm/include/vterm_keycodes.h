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
  VTERM_कुंजी_NONE,

  VTERM_कुंजी_ENTER,
  VTERM_कुंजी_TAB,
  VTERM_कुंजी_BACKSPACE,
  VTERM_कुंजी_ESCAPE,

  VTERM_कुंजी_UP,
  VTERM_कुंजी_DOWN,
  VTERM_कुंजी_बायाँ,
  VTERM_कुंजी_दायाँ,

  VTERM_कुंजी_INS,
  VTERM_कुंजी_DEL,
  VTERM_कुंजी_HOME,
  VTERM_कुंजी_END,
  VTERM_कुंजी_PAGEUP,
  VTERM_कुंजी_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_कुंजी_FUNCTION_0   = 256,
  VTERM_कुंजी_FUNCTION_MAX = VTERM_कुंजी_FUNCTION_0 + 255,

  // keypad keys
  VTERM_कुंजी_KP_0,
  VTERM_कुंजी_KP_1,
  VTERM_कुंजी_KP_2,
  VTERM_कुंजी_KP_3,
  VTERM_कुंजी_KP_4,
  VTERM_कुंजी_KP_5,
  VTERM_कुंजी_KP_6,
  VTERM_कुंजी_KP_7,
  VTERM_कुंजी_KP_8,
  VTERM_कुंजी_KP_9,
  VTERM_कुंजी_KP_MULT,
  VTERM_कुंजी_KP_PLUS,
  VTERM_कुंजी_KP_COMMA,
  VTERM_कुंजी_KP_MINUS,
  VTERM_कुंजी_KP_PERIOD,
  VTERM_कुंजी_KP_DIVIDE,
  VTERM_कुंजी_KP_ENTER,
  VTERM_कुंजी_KP_EQUAL,

  VTERM_कुंजी_MAX, // Must be last
  VTERM_N_कुंजियाँ = VTERM_कुंजी_MAX
} VTerm_कुंजी;

#define VTERM_कुंजी_FUNCTION(n) (VTERM_कुंजी_FUNCTION_0+(n))

#endif
