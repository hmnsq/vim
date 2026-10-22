#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

வகைவரை எண்ணிக்கை {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
வகைவரை எண்ணிக்கை {
  VTERM_சாவி_NONE,

  VTERM_சாவி_ENTER,
  VTERM_சாவி_TAB,
  VTERM_சாவி_BACKSPACE,
  VTERM_சாவி_ESCAPE,

  VTERM_சாவி_UP,
  VTERM_சாவி_DOWN,
  VTERM_சாவி_இடது,
  VTERM_சாவி_வலது,

  VTERM_சாவி_INS,
  VTERM_சாவி_DEL,
  VTERM_சாவி_HOME,
  VTERM_சாவி_END,
  VTERM_சாவி_PAGEUP,
  VTERM_சாவி_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_சாவி_FUNCTION_0   = 256,
  VTERM_சாவி_FUNCTION_MAX = VTERM_சாவி_FUNCTION_0 + 255,

  // keypad keys
  VTERM_சாவி_KP_0,
  VTERM_சாவி_KP_1,
  VTERM_சாவி_KP_2,
  VTERM_சாவி_KP_3,
  VTERM_சாவி_KP_4,
  VTERM_சாவி_KP_5,
  VTERM_சாவி_KP_6,
  VTERM_சாவி_KP_7,
  VTERM_சாவி_KP_8,
  VTERM_சாவி_KP_9,
  VTERM_சாவி_KP_MULT,
  VTERM_சாவி_KP_PLUS,
  VTERM_சாவி_KP_COMMA,
  VTERM_சாவி_KP_MINUS,
  VTERM_சாவி_KP_PERIOD,
  VTERM_சாவி_KP_DIVIDE,
  VTERM_சாவி_KP_ENTER,
  VTERM_சாவி_KP_EQUAL,

  VTERM_சாவி_MAX, // Must be last
  VTERM_N_சாவிகள் = VTERM_சாவி_MAX
} VTerm_சாவி;

#define VTERM_சாவி_FUNCTION(n) (VTERM_சாவி_FUNCTION_0+(n))

#endif
