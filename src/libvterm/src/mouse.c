#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

ਸਥਿਰ ਖਾਲੀ output_mouse(VTermState *state, ਪੂਰਨ ਕੋਡ, ਪੂਰਨ pressed, ਪੂਰਨ modifiers, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਪੰਕਤੀ)
{
  modifiers <<= 2;

  ਚੋਣ(state->mouse_protocol) {
  ਹਾਲਤ MOUSE_X10:
    ਜੇ(ਖਾਨਾ + 0x21 > 0xff)
      ਖਾਨਾ = 0xff - 0x21;
    ਜੇ(ਪੰਕਤੀ + 0x21 > 0xff)
      ਪੰਕਤੀ = 0xff - 0x21;

    ਜੇ(!pressed)
      ਕੋਡ = 3;

    vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (ਕੋਡ | modifiers) + 0x20, ਖਾਨਾ + 0x21, ਪੰਕਤੀ + 0x21);
    ਰੋਕੋ;

  ਹਾਲਤ MOUSE_UTF8:
    {
      ਅੱਖਰ utf8[18]; size_t len = 0;

      ਜੇ(!pressed)
        ਕੋਡ = 3;

      len += fill_utf8((ਕੋਡ | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(ਖਾਨਾ + 0x21, utf8 + len);
      len += fill_utf8(ਪੰਕਤੀ + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    ਰੋਕੋ;

  ਹਾਲਤ MOUSE_SGR:
    vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        ਕੋਡ | modifiers, ਖਾਨਾ + 1, ਪੰਕਤੀ + 1, pressed ? 'M' : 'm');
    ਰੋਕੋ;

  ਹਾਲਤ MOUSE_RXVT:
    ਜੇ(!pressed)
      ਕੋਡ = 3;

    vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        ਕੋਡ | modifiers, ਖਾਨਾ + 1, ਪੰਕਤੀ + 1);
    ਰੋਕੋ;
  }
}

ਖਾਲੀ vterm_mouse_move(VTerm *vt, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, VTermModifier mod)
{
  VTermState *state = vt->state;

  ਜੇ(ਖਾਨਾ == state->mouse_ਖਾਨਾ && ਪੰਕਤੀ == state->mouse_ਪੰਕਤੀ)
    ਵਾਪਸ;

  state->mouse_ਖਾਨਾ = ਖਾਨਾ;
  state->mouse_ਪੰਕਤੀ = ਪੰਕਤੀ;

  ਜੇ((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    ਪੂਰਨ button = state->mouse_buttons & MOUSE_BUTTON_ਖੱਬਾ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_ਵਿਚਕਾਰ ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ਸੱਜਾ ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, ਖਾਨਾ, ਪੰਕਤੀ);
  }
}

ਖਾਲੀ vterm_mouse_button(VTerm *vt, ਪੂਰਨ button, ਪੂਰਨ pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  ਪੂਰਨ old_buttons = state->mouse_buttons;

  ਜੇ(button > 0 && button <= 3) {
    ਜੇ(pressed)
      state->mouse_buttons |= (1 << (button-1));
    ਨਹੀਂਤਾਂ
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  ਜੇ(state->mouse_buttons == old_buttons && button < 4)
    ਵਾਪਸ;
  ਜੇ (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ਵਾਪਸ;

  ਜੇ(!state->mouse_flags)
    ਵਾਪਸ;

  ਜੇ(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_ਖਾਨਾ, state->mouse_ਪੰਕਤੀ);
  }
  ਨਹੀਂਤਾਂ ਜੇ(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_ਖਾਨਾ, state->mouse_ਪੰਕਤੀ);
  }
}
