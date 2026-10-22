#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __VTERM_INPUT_H__
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __VTERM_INPUT_H__

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_ਖਿਸਕਾਓ = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_ਨਕਾਬ = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  VTERM_ਕੁੰਜੀ_NONE,

  VTERM_ਕੁੰਜੀ_ENTER,
  VTERM_ਕੁੰਜੀ_TAB,
  VTERM_ਕੁੰਜੀ_BACKSPACE,
  VTERM_ਕੁੰਜੀ_ESCAPE,

  VTERM_ਕੁੰਜੀ_ਉੱਪਰ,
  VTERM_ਕੁੰਜੀ_ਥੱਲੇ,
  VTERM_ਕੁੰਜੀ_ਖੱਬਾ,
  VTERM_ਕੁੰਜੀ_ਸੱਜਾ,

  VTERM_ਕੁੰਜੀ_INS,
  VTERM_ਕੁੰਜੀ_DEL,
  VTERM_ਕੁੰਜੀ_HOME,
  VTERM_ਕੁੰਜੀ_ਸਿਰਾ,
  VTERM_ਕੁੰਜੀ_PAGEUP,
  VTERM_ਕੁੰਜੀ_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_ਕੁੰਜੀ_FUNCTION_0   = 256,
  VTERM_ਕੁੰਜੀ_FUNCTION_MAX = VTERM_ਕੁੰਜੀ_FUNCTION_0 + 255,

  // keypad keys
  VTERM_ਕੁੰਜੀ_KP_0,
  VTERM_ਕੁੰਜੀ_KP_1,
  VTERM_ਕੁੰਜੀ_KP_2,
  VTERM_ਕੁੰਜੀ_KP_3,
  VTERM_ਕੁੰਜੀ_KP_4,
  VTERM_ਕੁੰਜੀ_KP_5,
  VTERM_ਕੁੰਜੀ_KP_6,
  VTERM_ਕੁੰਜੀ_KP_7,
  VTERM_ਕੁੰਜੀ_KP_8,
  VTERM_ਕੁੰਜੀ_KP_9,
  VTERM_ਕੁੰਜੀ_KP_MULT,
  VTERM_ਕੁੰਜੀ_KP_PLUS,
  VTERM_ਕੁੰਜੀ_KP_COMMA,
  VTERM_ਕੁੰਜੀ_KP_MINUS,
  VTERM_ਕੁੰਜੀ_KP_PERIOD,
  VTERM_ਕੁੰਜੀ_KP_DIVIDE,
  VTERM_ਕੁੰਜੀ_KP_ENTER,
  VTERM_ਕੁੰਜੀ_KP_EQUAL,

  VTERM_ਕੁੰਜੀ_MAX, // Must be last
  VTERM_N_ਕੁੰਜੀਆਂ = VTERM_ਕੁੰਜੀ_MAX
} VTerm_ਕੁੰਜੀ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTERM_ਕੁੰਜੀ_FUNCTION(n) (VTERM_ਕੁੰਜੀ_FUNCTION_0+(n))

#ਜੇ_ਅੰਤ
