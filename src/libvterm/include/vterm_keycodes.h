#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

തരനിർവചനം എണ്ണൽ {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
തരനിർവചനം എണ്ണൽ {
  VTERM_താക്കോൽ_NONE,

  VTERM_താക്കോൽ_ENTER,
  VTERM_താക്കോൽ_TAB,
  VTERM_താക്കോൽ_BACKSPACE,
  VTERM_താക്കോൽ_ESCAPE,

  VTERM_താക്കോൽ_UP,
  VTERM_താക്കോൽ_DOWN,
  VTERM_താക്കോൽ_ഇടത്,
  VTERM_താക്കോൽ_വലത്,

  VTERM_താക്കോൽ_INS,
  VTERM_താക്കോൽ_DEL,
  VTERM_താക്കോൽ_HOME,
  VTERM_താക്കോൽ_END,
  VTERM_താക്കോൽ_PAGEUP,
  VTERM_താക്കോൽ_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_താക്കോൽ_FUNCTION_0   = 256,
  VTERM_താക്കോൽ_FUNCTION_MAX = VTERM_താക്കോൽ_FUNCTION_0 + 255,

  // keypad keys
  VTERM_താക്കോൽ_KP_0,
  VTERM_താക്കോൽ_KP_1,
  VTERM_താക്കോൽ_KP_2,
  VTERM_താക്കോൽ_KP_3,
  VTERM_താക്കോൽ_KP_4,
  VTERM_താക്കോൽ_KP_5,
  VTERM_താക്കോൽ_KP_6,
  VTERM_താക്കോൽ_KP_7,
  VTERM_താക്കോൽ_KP_8,
  VTERM_താക്കോൽ_KP_9,
  VTERM_താക്കോൽ_KP_MULT,
  VTERM_താക്കോൽ_KP_PLUS,
  VTERM_താക്കോൽ_KP_COMMA,
  VTERM_താക്കോൽ_KP_MINUS,
  VTERM_താക്കോൽ_KP_PERIOD,
  VTERM_താക്കോൽ_KP_DIVIDE,
  VTERM_താക്കോൽ_KP_ENTER,
  VTERM_താക്കോൽ_KP_EQUAL,

  VTERM_താക്കോൽ_MAX, // Must be last
  VTERM_N_താക്കോലുകൾ = VTERM_താക്കോൽ_MAX
} VTerm_താക്കോൽ;

#define VTERM_താക്കോൽ_FUNCTION(n) (VTERM_താക്കോൽ_FUNCTION_0+(n))

#endif
