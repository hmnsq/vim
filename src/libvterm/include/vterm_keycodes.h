#که_نه_تعریف_شوی __VTERM_INPUT_H__
#تعریف_کړه __VTERM_INPUT_H__

تعريفنوع تعداد {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_ښويدنه = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_ماسک = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
تعريفنوع تعداد {
  VTERM_کلی_NONE,

  VTERM_کلی_ENTER,
  VTERM_کلی_TAB,
  VTERM_کلی_BACKSPACE,
  VTERM_کلی_ESCAPE,

  VTERM_کلی_پورته,
  VTERM_کلی_ښکته,
  VTERM_کلی_کیڼ,
  VTERM_کلی_ښی,

  VTERM_کلی_INS,
  VTERM_کلی_DEL,
  VTERM_کلی_HOME,
  VTERM_کلی_پای_ټکی,
  VTERM_کلی_PAGEUP,
  VTERM_کلی_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_کلی_FUNCTION_0   = 256,
  VTERM_کلی_FUNCTION_MAX = VTERM_کلی_FUNCTION_0 + 255,

  // keypad keys
  VTERM_کلی_KP_0,
  VTERM_کلی_KP_1,
  VTERM_کلی_KP_2,
  VTERM_کلی_KP_3,
  VTERM_کلی_KP_4,
  VTERM_کلی_KP_5,
  VTERM_کلی_KP_6,
  VTERM_کلی_KP_7,
  VTERM_کلی_KP_8,
  VTERM_کلی_KP_9,
  VTERM_کلی_KP_MULT,
  VTERM_کلی_KP_PLUS,
  VTERM_کلی_KP_COMMA,
  VTERM_کلی_KP_MINUS,
  VTERM_کلی_KP_PERIOD,
  VTERM_کلی_KP_DIVIDE,
  VTERM_کلی_KP_ENTER,
  VTERM_کلی_KP_EQUAL,

  VTERM_کلی_MAX, // Must be last
  VTERM_N_کلیګانې = VTERM_کلی_MAX
} VTerm_کلی;

#تعریف_کړه VTERM_کلی_FUNCTION(n) (VTERM_کلی_FUNCTION_0+(n))

#د_که_پای
