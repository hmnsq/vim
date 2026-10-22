#جيڪڏهن_وصف_نه_ٿيل __VTERM_INPUT_H__
#وصف_ڪريو __VTERM_INPUT_H__

تعريفنوع تعداد {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_سرڪايو = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_نقاب = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
تعريفنوع تعداد {
  VTERM_ڪنجي_NONE,

  VTERM_ڪنجي_ENTER,
  VTERM_ڪنجي_TAB,
  VTERM_ڪنجي_BACKSPACE,
  VTERM_ڪنجي_ESCAPE,

  VTERM_ڪنجي_مٿي,
  VTERM_ڪنجي_هيٺ_ڏانهن,
  VTERM_ڪنجي_کاٻو,
  VTERM_ڪنجي_ساڄو,

  VTERM_ڪنجي_INS,
  VTERM_ڪنجي_DEL,
  VTERM_ڪنجي_HOME,
  VTERM_ڪنجي_پڇاڙي,
  VTERM_ڪنجي_PAGEUP,
  VTERM_ڪنجي_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_ڪنجي_FUNCTION_0   = 256,
  VTERM_ڪنجي_FUNCTION_MAX = VTERM_ڪنجي_FUNCTION_0 + 255,

  // keypad keys
  VTERM_ڪنجي_KP_0,
  VTERM_ڪنجي_KP_1,
  VTERM_ڪنجي_KP_2,
  VTERM_ڪنجي_KP_3,
  VTERM_ڪنجي_KP_4,
  VTERM_ڪنجي_KP_5,
  VTERM_ڪنجي_KP_6,
  VTERM_ڪنجي_KP_7,
  VTERM_ڪنجي_KP_8,
  VTERM_ڪنجي_KP_9,
  VTERM_ڪنجي_KP_MULT,
  VTERM_ڪنجي_KP_PLUS,
  VTERM_ڪنجي_KP_COMMA,
  VTERM_ڪنجي_KP_MINUS,
  VTERM_ڪنجي_KP_PERIOD,
  VTERM_ڪنجي_KP_DIVIDE,
  VTERM_ڪنجي_KP_ENTER,
  VTERM_ڪنجي_KP_EQUAL,

  VTERM_ڪنجي_MAX, // Must be last
  VTERM_N_ڪنجيون = VTERM_ڪنجي_MAX
} VTerm_ڪنجي;

#وصف_ڪريو VTERM_ڪنجي_FUNCTION(n) (VTERM_ڪنجي_FUNCTION_0+(n))

#جيڪڏهن_پورو
