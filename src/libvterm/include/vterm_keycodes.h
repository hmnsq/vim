#جے_بنا_تعریف __VTERM_INPUT_H__
#تعریف_کرو __VTERM_INPUT_H__

تعريفنوع تعداد {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_کھسکاؤ = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_نقاب = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
تعريفنوع تعداد {
  VTERM_کلید_NONE,

  VTERM_کلید_ENTER,
  VTERM_کلید_TAB,
  VTERM_کلید_BACKSPACE,
  VTERM_کلید_ESCAPE,

  VTERM_کلید_اتے,
  VTERM_کلید_تھلے,
  VTERM_کلید_بایاں,
  VTERM_کلید_دایاں,

  VTERM_کلید_INS,
  VTERM_کلید_DEL,
  VTERM_کلید_HOME,
  VTERM_کلید_سرا,
  VTERM_کلید_PAGEUP,
  VTERM_کلید_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_کلید_FUNCTION_0   = 256,
  VTERM_کلید_FUNCTION_MAX = VTERM_کلید_FUNCTION_0 + 255,

  // keypad keys
  VTERM_کلید_KP_0,
  VTERM_کلید_KP_1,
  VTERM_کلید_KP_2,
  VTERM_کلید_KP_3,
  VTERM_کلید_KP_4,
  VTERM_کلید_KP_5,
  VTERM_کلید_KP_6,
  VTERM_کلید_KP_7,
  VTERM_کلید_KP_8,
  VTERM_کلید_KP_9,
  VTERM_کلید_KP_MULT,
  VTERM_کلید_KP_PLUS,
  VTERM_کلید_KP_COMMA,
  VTERM_کلید_KP_MINUS,
  VTERM_کلید_KP_PERIOD,
  VTERM_کلید_KP_DIVIDE,
  VTERM_کلید_KP_ENTER,
  VTERM_کلید_KP_EQUAL,

  VTERM_کلید_MAX, // Must be last
  VTERM_N_کلیدیں = VTERM_کلید_MAX
} VTerm_کلید;

#تعریف_کرو VTERM_کلید_FUNCTION(n) (VTERM_کلید_FUNCTION_0+(n))

#جے_انت
