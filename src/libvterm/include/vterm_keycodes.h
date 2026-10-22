#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

প্রকারনির্দেশ গণনা {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
প্রকারনির্দেশ গণনা {
  VTERM_চাবি_NONE,

  VTERM_চাবি_ENTER,
  VTERM_চাবি_TAB,
  VTERM_চাবি_BACKSPACE,
  VTERM_চাবি_ESCAPE,

  VTERM_চাবি_UP,
  VTERM_চাবি_DOWN,
  VTERM_চাবি_বাম,
  VTERM_চাবি_ডান,

  VTERM_চাবি_INS,
  VTERM_চাবি_DEL,
  VTERM_চাবি_HOME,
  VTERM_চাবি_END,
  VTERM_চাবি_PAGEUP,
  VTERM_চাবি_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_চাবি_FUNCTION_0   = 256,
  VTERM_চাবি_FUNCTION_MAX = VTERM_চাবি_FUNCTION_0 + 255,

  // keypad keys
  VTERM_চাবি_KP_0,
  VTERM_চাবি_KP_1,
  VTERM_চাবি_KP_2,
  VTERM_চাবি_KP_3,
  VTERM_চাবি_KP_4,
  VTERM_চাবি_KP_5,
  VTERM_চাবি_KP_6,
  VTERM_চাবি_KP_7,
  VTERM_চাবি_KP_8,
  VTERM_চাবি_KP_9,
  VTERM_চাবি_KP_MULT,
  VTERM_চাবি_KP_PLUS,
  VTERM_চাবি_KP_COMMA,
  VTERM_চাবি_KP_MINUS,
  VTERM_চাবি_KP_PERIOD,
  VTERM_চাবি_KP_DIVIDE,
  VTERM_চাবি_KP_ENTER,
  VTERM_চাবি_KP_EQUAL,

  VTERM_চাবি_MAX, // Must be last
  VTERM_N_চাবিগুলি = VTERM_চাবি_MAX
} VTerm_চাবি;

#define VTERM_চাবি_FUNCTION(n) (VTERM_চাবি_FUNCTION_0+(n))

#endif
