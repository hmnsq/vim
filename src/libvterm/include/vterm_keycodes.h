#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

تعريفنوع تعداد {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
تعريفنوع تعداد {
  VTERM_کونچي_NONE,

  VTERM_کونچي_ENTER,
  VTERM_کونچي_TAB,
  VTERM_کونچي_BACKSPACE,
  VTERM_کونچي_ESCAPE,

  VTERM_کونچي_UP,
  VTERM_کونچي_DOWN,
  VTERM_کونچي_کيري,
  VTERM_کونچي_کانن,

  VTERM_کونچي_INS,
  VTERM_کونچي_DEL,
  VTERM_کونچي_HOME,
  VTERM_کونچي_END,
  VTERM_کونچي_PAGEUP,
  VTERM_کونچي_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_کونچي_FUNCTION_0   = 256,
  VTERM_کونچي_FUNCTION_MAX = VTERM_کونچي_FUNCTION_0 + 255,

  // keypad keys
  VTERM_کونچي_KP_0,
  VTERM_کونچي_KP_1,
  VTERM_کونچي_KP_2,
  VTERM_کونچي_KP_3,
  VTERM_کونچي_KP_4,
  VTERM_کونچي_KP_5,
  VTERM_کونچي_KP_6,
  VTERM_کونچي_KP_7,
  VTERM_کونچي_KP_8,
  VTERM_کونچي_KP_9,
  VTERM_کونچي_KP_MULT,
  VTERM_کونچي_KP_PLUS,
  VTERM_کونچي_KP_COMMA,
  VTERM_کونچي_KP_MINUS,
  VTERM_کونچي_KP_PERIOD,
  VTERM_کونچي_KP_DIVIDE,
  VTERM_کونچي_KP_ENTER,
  VTERM_کونچي_KP_EQUAL,

  VTERM_کونچي_MAX, // Must be last
  VTERM_N_کونچي_سموا = VTERM_کونچي_MAX
} VTerm_کونچي;

#define VTERM_کونچي_FUNCTION(n) (VTERM_کونچي_FUNCTION_0+(n))

#endif
