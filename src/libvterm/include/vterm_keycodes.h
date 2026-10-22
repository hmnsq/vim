#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
  VTERM_ಕೀಲಿ_NONE,

  VTERM_ಕೀಲಿ_ENTER,
  VTERM_ಕೀಲಿ_TAB,
  VTERM_ಕೀಲಿ_BACKSPACE,
  VTERM_ಕೀಲಿ_ESCAPE,

  VTERM_ಕೀಲಿ_UP,
  VTERM_ಕೀಲಿ_DOWN,
  VTERM_ಕೀಲಿ_ಎಡ,
  VTERM_ಕೀಲಿ_ಬಲ,

  VTERM_ಕೀಲಿ_INS,
  VTERM_ಕೀಲಿ_DEL,
  VTERM_ಕೀಲಿ_HOME,
  VTERM_ಕೀಲಿ_END,
  VTERM_ಕೀಲಿ_PAGEUP,
  VTERM_ಕೀಲಿ_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_ಕೀಲಿ_FUNCTION_0   = 256,
  VTERM_ಕೀಲಿ_FUNCTION_MAX = VTERM_ಕೀಲಿ_FUNCTION_0 + 255,

  // keypad keys
  VTERM_ಕೀಲಿ_KP_0,
  VTERM_ಕೀಲಿ_KP_1,
  VTERM_ಕೀಲಿ_KP_2,
  VTERM_ಕೀಲಿ_KP_3,
  VTERM_ಕೀಲಿ_KP_4,
  VTERM_ಕೀಲಿ_KP_5,
  VTERM_ಕೀಲಿ_KP_6,
  VTERM_ಕೀಲಿ_KP_7,
  VTERM_ಕೀಲಿ_KP_8,
  VTERM_ಕೀಲಿ_KP_9,
  VTERM_ಕೀಲಿ_KP_MULT,
  VTERM_ಕೀಲಿ_KP_PLUS,
  VTERM_ಕೀಲಿ_KP_COMMA,
  VTERM_ಕೀಲಿ_KP_MINUS,
  VTERM_ಕೀಲಿ_KP_PERIOD,
  VTERM_ಕೀಲಿ_KP_DIVIDE,
  VTERM_ಕೀಲಿ_KP_ENTER,
  VTERM_ಕೀಲಿ_KP_EQUAL,

  VTERM_ಕೀಲಿ_MAX, // Must be last
  VTERM_N_ಕೀಲಿಗಳು = VTERM_ಕೀಲಿ_MAX
} VTerm_ಕೀಲಿ;

#define VTERM_ಕೀಲಿ_FUNCTION(n) (VTERM_ಕೀಲಿ_FUNCTION_0+(n))

#endif
