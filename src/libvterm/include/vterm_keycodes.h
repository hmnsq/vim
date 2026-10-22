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
  VTERM_مفتاح_NONE,

  VTERM_مفتاح_ENTER,
  VTERM_مفتاح_TAB,
  VTERM_مفتاح_BACKSPACE,
  VTERM_مفتاح_ESCAPE,

  VTERM_مفتاح_UP,
  VTERM_مفتاح_DOWN,
  VTERM_مفتاح_يسار,
  VTERM_مفتاح_يمين,

  VTERM_مفتاح_INS,
  VTERM_مفتاح_DEL,
  VTERM_مفتاح_HOME,
  VTERM_مفتاح_END,
  VTERM_مفتاح_PAGEUP,
  VTERM_مفتاح_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_مفتاح_FUNCTION_0   = 256,
  VTERM_مفتاح_FUNCTION_MAX = VTERM_مفتاح_FUNCTION_0 + 255,

  // keypad keys
  VTERM_مفتاح_KP_0,
  VTERM_مفتاح_KP_1,
  VTERM_مفتاح_KP_2,
  VTERM_مفتاح_KP_3,
  VTERM_مفتاح_KP_4,
  VTERM_مفتاح_KP_5,
  VTERM_مفتاح_KP_6,
  VTERM_مفتاح_KP_7,
  VTERM_مفتاح_KP_8,
  VTERM_مفتاح_KP_9,
  VTERM_مفتاح_KP_MULT,
  VTERM_مفتاح_KP_PLUS,
  VTERM_مفتاح_KP_COMMA,
  VTERM_مفتاح_KP_MINUS,
  VTERM_مفتاح_KP_PERIOD,
  VTERM_مفتاح_KP_DIVIDE,
  VTERM_مفتاح_KP_ENTER,
  VTERM_مفتاح_KP_EQUAL,

  VTERM_مفتاح_MAX, // Must be last
  VTERM_N_مفاتيح = VTERM_مفتاح_MAX
} VTerm_مفتاح;

#define VTERM_مفتاح_FUNCTION(n) (VTERM_مفتاح_FUNCTION_0+(n))

#endif
