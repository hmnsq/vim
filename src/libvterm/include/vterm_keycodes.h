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
  VTERM_साँचो_NONE,

  VTERM_साँचो_ENTER,
  VTERM_साँचो_TAB,
  VTERM_साँचो_BACKSPACE,
  VTERM_साँचो_ESCAPE,

  VTERM_साँचो_UP,
  VTERM_साँचो_DOWN,
  VTERM_साँचो_बायाँ,
  VTERM_साँचो_दायाँ,

  VTERM_साँचो_INS,
  VTERM_साँचो_DEL,
  VTERM_साँचो_HOME,
  VTERM_साँचो_END,
  VTERM_साँचो_PAGEUP,
  VTERM_साँचो_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_साँचो_FUNCTION_0   = 256,
  VTERM_साँचो_FUNCTION_MAX = VTERM_साँचो_FUNCTION_0 + 255,

  // keypad keys
  VTERM_साँचो_KP_0,
  VTERM_साँचो_KP_1,
  VTERM_साँचो_KP_2,
  VTERM_साँचो_KP_3,
  VTERM_साँचो_KP_4,
  VTERM_साँचो_KP_5,
  VTERM_साँचो_KP_6,
  VTERM_साँचो_KP_7,
  VTERM_साँचो_KP_8,
  VTERM_साँचो_KP_9,
  VTERM_साँचो_KP_MULT,
  VTERM_साँचो_KP_PLUS,
  VTERM_साँचो_KP_COMMA,
  VTERM_साँचो_KP_MINUS,
  VTERM_साँचो_KP_PERIOD,
  VTERM_साँचो_KP_DIVIDE,
  VTERM_साँचो_KP_ENTER,
  VTERM_साँचो_KP_EQUAL,

  VTERM_साँचो_MAX, // Must be last
  VTERM_N_साँचोहरू = VTERM_साँचो_MAX
} VTerm_साँचो;

#define VTERM_साँचो_FUNCTION(n) (VTERM_साँचो_FUNCTION_0+(n))

#endif
