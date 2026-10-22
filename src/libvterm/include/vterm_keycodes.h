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
  VTERM_مبودي_NONE,

  VTERM_مبودي_ENTER,
  VTERM_مبودي_TAB,
  VTERM_مبودي_BACKSPACE,
  VTERM_مبودي_ESCAPE,

  VTERM_مبودي_UP,
  VTERM_مبودي_DOWN,
  VTERM_مبودي_هغو,
  VTERM_مبودي_دام,

  VTERM_مبودي_INS,
  VTERM_مبودي_DEL,
  VTERM_مبودي_HOME,
  VTERM_مبودي_END,
  VTERM_مبودي_PAGEUP,
  VTERM_مبودي_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_مبودي_FUNCTION_0   = 256,
  VTERM_مبودي_FUNCTION_MAX = VTERM_مبودي_FUNCTION_0 + 255,

  // keypad keys
  VTERM_مبودي_KP_0,
  VTERM_مبودي_KP_1,
  VTERM_مبودي_KP_2,
  VTERM_مبودي_KP_3,
  VTERM_مبودي_KP_4,
  VTERM_مبودي_KP_5,
  VTERM_مبودي_KP_6,
  VTERM_مبودي_KP_7,
  VTERM_مبودي_KP_8,
  VTERM_مبودي_KP_9,
  VTERM_مبودي_KP_MULT,
  VTERM_مبودي_KP_PLUS,
  VTERM_مبودي_KP_COMMA,
  VTERM_مبودي_KP_MINUS,
  VTERM_مبودي_KP_PERIOD,
  VTERM_مبودي_KP_DIVIDE,
  VTERM_مبودي_KP_ENTER,
  VTERM_مبودي_KP_EQUAL,

  VTERM_مبودي_MAX, // Must be last
  VTERM_N_مبوداي = VTERM_مبودي_MAX
} VTerm_مبودي;

#define VTERM_مبودي_FUNCTION(n) (VTERM_مبودي_FUNCTION_0+(n))

#endif
