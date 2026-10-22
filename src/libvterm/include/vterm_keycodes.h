#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  VTERM_ଚାବି_NONE,

  VTERM_ଚାବି_ENTER,
  VTERM_ଚାବି_TAB,
  VTERM_ଚାବି_BACKSPACE,
  VTERM_ଚାବି_ESCAPE,

  VTERM_ଚାବି_UP,
  VTERM_ଚାବି_DOWN,
  VTERM_ଚାବି_ବାମ,
  VTERM_ଚାବି_ଡାହାଣ,

  VTERM_ଚାବି_INS,
  VTERM_ଚାବି_DEL,
  VTERM_ଚାବି_HOME,
  VTERM_ଚାବି_END,
  VTERM_ଚାବି_PAGEUP,
  VTERM_ଚାବି_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_ଚାବି_FUNCTION_0   = 256,
  VTERM_ଚାବି_FUNCTION_MAX = VTERM_ଚାବି_FUNCTION_0 + 255,

  // keypad keys
  VTERM_ଚାବି_KP_0,
  VTERM_ଚାବି_KP_1,
  VTERM_ଚାବି_KP_2,
  VTERM_ଚାବି_KP_3,
  VTERM_ଚାବି_KP_4,
  VTERM_ଚାବି_KP_5,
  VTERM_ଚାବି_KP_6,
  VTERM_ଚାବି_KP_7,
  VTERM_ଚାବି_KP_8,
  VTERM_ଚାବି_KP_9,
  VTERM_ଚାବି_KP_MULT,
  VTERM_ଚାବି_KP_PLUS,
  VTERM_ଚାବି_KP_COMMA,
  VTERM_ଚାବି_KP_MINUS,
  VTERM_ଚାବି_KP_PERIOD,
  VTERM_ଚାବି_KP_DIVIDE,
  VTERM_ଚାବି_KP_ENTER,
  VTERM_ଚାବି_KP_EQUAL,

  VTERM_ଚାବି_MAX, // Must be last
  VTERM_N_ଚାବିଗୁଡ଼ିକ = VTERM_ଚାବି_MAX
} VTerm_ଚାବି;

#define VTERM_ଚାବି_FUNCTION(n) (VTERM_ଚାବି_FUNCTION_0+(n))

#endif
