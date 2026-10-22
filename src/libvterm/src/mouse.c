#شامل "vterm_internal.h"

#شامل "utf8.h"

ساکن خالی output_mouse(VTermState *state, صحیح رمز, صحیح pressed, صحیح modifiers, صحیح ستون, صحیح صف)
{
  modifiers <<= 2;

  انتخاب(state->mouse_protocol) {
  حالت MOUSE_X10:
    اگر(ستون + 0x21 > 0xff)
      ستون = 0xff - 0x21;
    اگر(صف + 0x21 > 0xff)
      صف = 0xff - 0x21;

    اگر(!pressed)
      رمز = 3;

    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (رمز | modifiers) + 0x20, ستون + 0x21, صف + 0x21);
    توڑو;

  حالت MOUSE_UTF8:
    {
      حرف utf8[18]; size_t len = 0;

      اگر(!pressed)
        رمز = 3;

      len += fill_utf8((رمز | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(ستون + 0x21, utf8 + len);
      len += fill_utf8(صف + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    توڑو;

  حالت MOUSE_SGR:
    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        رمز | modifiers, ستون + 1, صف + 1, pressed ? 'M' : 'm');
    توڑو;

  حالت MOUSE_RXVT:
    اگر(!pressed)
      رمز = 3;

    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        رمز | modifiers, ستون + 1, صف + 1);
    توڑو;
  }
}

خالی vterm_mouse_move(VTerm *vt, صحیح صف, صحیح ستون, VTermModifier mod)
{
  VTermState *state = vt->state;

  اگر(ستون == state->mouse_ستون && صف == state->mouse_صف)
    واپس;

  state->mouse_ستون = ستون;
  state->mouse_صف = صف;

  اگر((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    صحیح button = state->mouse_buttons & MOUSE_BUTTON_بایاں ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_درمیان ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_دایاں ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, ستون, صف);
  }
}

خالی vterm_mouse_button(VTerm *vt, صحیح button, صحیح pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  صحیح old_buttons = state->mouse_buttons;

  اگر(button > 0 && button <= 3) {
    اگر(pressed)
      state->mouse_buttons |= (1 << (button-1));
    ورنہ
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  اگر(state->mouse_buttons == old_buttons && button < 4)
    واپس;
  اگر (!(state->mouse_flags & MOUSE_WANT_CLICK))
    واپس;

  اگر(!state->mouse_flags)
    واپس;

  اگر(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_ستون, state->mouse_صف);
  }
  ورنہ اگر(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_ستون, state->mouse_صف);
  }
}
