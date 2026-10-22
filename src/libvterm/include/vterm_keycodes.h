#నిర్వచితం_కాకపోతే __VTERM_INPUT_H__
#నిర్వచించు __VTERM_INPUT_H__

రకనిర్వచనం గణన {
  VTERM_MOD_NONE  = 0x00,
  VTERM_MOD_జరుపు = 0x01,
  VTERM_MOD_ALT   = 0x02,
  VTERM_MOD_CTRL  = 0x04,

  VTERM_ALL_MODS_ముసుగు = 0x07
} VTermModifier;

// The order here must match keycodes[] in src/keyboard.c!
రకనిర్వచనం గణన {
  VTERM_కీలకం_NONE,

  VTERM_కీలకం_ENTER,
  VTERM_కీలకం_TAB,
  VTERM_కీలకం_BACKSPACE,
  VTERM_కీలకం_ESCAPE,

  VTERM_కీలకం_పైకి,
  VTERM_కీలకం_కిందికి,
  VTERM_కీలకం_ఎడమ,
  VTERM_కీలకం_కుడి,

  VTERM_కీలకం_INS,
  VTERM_కీలకం_DEL,
  VTERM_కీలకం_HOME,
  VTERM_కీలకం_చివర,
  VTERM_కీలకం_PAGEUP,
  VTERM_కీలకం_PAGEDOWN,

  // F1 is VTERM_KEY_FUNCTION(1), F2 VTERM_KEY_FUNCTION(2), etc.
  VTERM_కీలకం_FUNCTION_0   = 256,
  VTERM_కీలకం_FUNCTION_MAX = VTERM_కీలకం_FUNCTION_0 + 255,

  // keypad keys
  VTERM_కీలకం_KP_0,
  VTERM_కీలకం_KP_1,
  VTERM_కీలకం_KP_2,
  VTERM_కీలకం_KP_3,
  VTERM_కీలకం_KP_4,
  VTERM_కీలకం_KP_5,
  VTERM_కీలకం_KP_6,
  VTERM_కీలకం_KP_7,
  VTERM_కీలకం_KP_8,
  VTERM_కీలకం_KP_9,
  VTERM_కీలకం_KP_MULT,
  VTERM_కీలకం_KP_PLUS,
  VTERM_కీలకం_KP_COMMA,
  VTERM_కీలకం_KP_MINUS,
  VTERM_కీలకం_KP_PERIOD,
  VTERM_కీలకం_KP_DIVIDE,
  VTERM_కీలకం_KP_ENTER,
  VTERM_కీలకం_KP_EQUAL,

  VTERM_కీలకం_MAX, // Must be last
  VTERM_N_కీలకాలు = VTERM_కీలకం_MAX
} VTerm_కీలకం;

#నిర్వచించు VTERM_కీలకం_FUNCTION(n) (VTERM_కీలకం_FUNCTION_0+(n))

#అయితే_ముగింపు
