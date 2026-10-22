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
  VTERM_ئاچقۇچ_NONE,

  VTERM_ئاچقۇچ_ENTER,
  VTERM_ئاچقۇچ_TAB,
  VTERM_ئاچقۇچ_BACKSPACE,
  VTERM_ئاچقۇچ_ESCAPE,

  VTERM_ئاچقۇچ_UP,
  VTERM_ئاچقۇچ_DOWN,
  VTERM_ئاچقۇچ_سول,
  VTERM_ئاچقۇچ_ئوڭ,

  VTERM_ئاچقۇچ_INS,
  VTERM_ئاچقۇچ_DEL,
  VTERM_ئاچقۇچ_HOME,
  VTERM_ئاچقۇچ_END,
  VTERM_ئاچقۇچ_PAGEUP,
  VTERM_ئاچقۇچ_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_ئاچقۇچ_FUNCTION_0   = 256,
  VTERM_ئاچقۇچ_FUNCTION_MAX = VTERM_ئاچقۇچ_FUNCTION_0 + 255,

  // keypad keys
  VTERM_ئاچقۇچ_KP_0,
  VTERM_ئاچقۇچ_KP_1,
  VTERM_ئاچقۇچ_KP_2,
  VTERM_ئاچقۇچ_KP_3,
  VTERM_ئاچقۇچ_KP_4,
  VTERM_ئاچقۇچ_KP_5,
  VTERM_ئاچقۇچ_KP_6,
  VTERM_ئاچقۇچ_KP_7,
  VTERM_ئاچقۇچ_KP_8,
  VTERM_ئاچقۇچ_KP_9,
  VTERM_ئاچقۇچ_KP_MULT,
  VTERM_ئاچقۇچ_KP_PLUS,
  VTERM_ئاچقۇچ_KP_COMMA,
  VTERM_ئاچقۇچ_KP_MINUS,
  VTERM_ئاچقۇچ_KP_PERIOD,
  VTERM_ئاچقۇچ_KP_DIVIDE,
  VTERM_ئاچقۇچ_KP_ENTER,
  VTERM_ئاچقۇچ_KP_EQUAL,

  VTERM_ئاچقۇچ_MAX, // Must be last
  VTERM_N_ئاچقۇچلار = VTERM_ئاچقۇچ_MAX
} VTerm_ئاچقۇچ;

#define VTERM_ئاچقۇچ_FUNCTION(n) (VTERM_ئاچقۇچ_FUNCTION_0+(n))

#endif
