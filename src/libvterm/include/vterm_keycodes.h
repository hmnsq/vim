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
  VTERM_کلیل_NONE,

  VTERM_کلیل_ENTER,
  VTERM_کلیل_TAB,
  VTERM_کلیل_BACKSPACE,
  VTERM_کلیل_ESCAPE,

  VTERM_کلیل_UP,
  VTERM_کلیل_DOWN,
  VTERM_کلیل_چەپ,
  VTERM_کلیل_ڕاست,

  VTERM_کلیل_INS,
  VTERM_کلیل_DEL,
  VTERM_کلیل_HOME,
  VTERM_کلیل_END,
  VTERM_کلیل_PAGEUP,
  VTERM_کلیل_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_کلیل_FUNCTION_0   = 256,
  VTERM_کلیل_FUNCTION_MAX = VTERM_کلیل_FUNCTION_0 + 255,

  // keypad keys
  VTERM_کلیل_KP_0,
  VTERM_کلیل_KP_1,
  VTERM_کلیل_KP_2,
  VTERM_کلیل_KP_3,
  VTERM_کلیل_KP_4,
  VTERM_کلیل_KP_5,
  VTERM_کلیل_KP_6,
  VTERM_کلیل_KP_7,
  VTERM_کلیل_KP_8,
  VTERM_کلیل_KP_9,
  VTERM_کلیل_KP_MULT,
  VTERM_کلیل_KP_PLUS,
  VTERM_کلیل_KP_COMMA,
  VTERM_کلیل_KP_MINUS,
  VTERM_کلیل_KP_PERIOD,
  VTERM_کلیل_KP_DIVIDE,
  VTERM_کلیل_KP_ENTER,
  VTERM_کلیل_KP_EQUAL,

  VTERM_کلیل_MAX, // Must be last
  VTERM_N_کلیلەکان = VTERM_کلیل_MAX
} VTerm_کلیل;

#define VTERM_کلیل_FUNCTION(n) (VTERM_کلیل_FUNCTION_0+(n))

#endif
