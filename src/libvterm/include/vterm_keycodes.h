#ifndef __VTERM_INPUT_H__
#define __VTERM_INPUT_H__

typedef enum {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_SHIFT = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_MASK = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
typedef enum {
  VTERM_सुसि_NONE,

  VTERM_सुसि_ENTER,
  VTERM_सुसि_TAB,
  VTERM_सुसि_BACKSPACE,
  VTERM_सुसि_ESCAPE,

  VTERM_सुसि_UP,
  VTERM_सुसि_DOWN,
  VTERM_सुसि_कालिवा,
  VTERM_सुसि_कानान,

  VTERM_सुसि_INS,
  VTERM_सुसि_DEL,
  VTERM_सुसि_HOME,
  VTERM_सुसि_END,
  VTERM_सुसि_PAGEUP,
  VTERM_सुसि_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_सुसि_FUNCTION_0   = 256,
  VTERM_सुसि_FUNCTION_MAX = VTERM_सुसि_FUNCTION_0 + 255,

  // keypad keys
  VTERM_सुसि_KP_0,
  VTERM_सुसि_KP_1,
  VTERM_सुसि_KP_2,
  VTERM_सुसि_KP_3,
  VTERM_सुसि_KP_4,
  VTERM_सुसि_KP_5,
  VTERM_सुसि_KP_6,
  VTERM_सुसि_KP_7,
  VTERM_सुसि_KP_8,
  VTERM_सुसि_KP_9,
  VTERM_सुसि_KP_MULT,
  VTERM_सुसि_KP_PLUS,
  VTERM_सुसि_KP_COMMA,
  VTERM_सुसि_KP_MINUS,
  VTERM_सुसि_KP_PERIOD,
  VTERM_सुसि_KP_DIVIDE,
  VTERM_सुसि_KP_ENTER,
  VTERM_सुसि_KP_EQUAL,

  VTERM_सुसि_MAX, // Must be last
  VTERM_N_मगा_सुसि = VTERM_सुसि_MAX
} VTerm_सुसि;

#define VTERM_सुसि_FUNCTION(n) (VTERM_सुसि_FUNCTION_0+(n))

#endif
