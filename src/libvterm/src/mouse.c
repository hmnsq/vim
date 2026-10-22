#ضمن "vterm_internal.h"

#ضمن "utf8.h"

ساكن فراغ output_mouse(VTermState *state, صحيح کد, صحيح pressed, صحيح modifiers, صحيح ستون, صحيح ردیف)
{
  modifiers <<= 2;

  بدل(state->mouse_protocol) {
  حالة MOUSE_X10:
    إذا(ستون + 0x21 > 0xff)
      ستون = 0xff - 0x21;
    إذا(ردیف + 0x21 > 0xff)
      ردیف = 0xff - 0x21;

    إذا(!pressed)
      کد = 3;

    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (کد | modifiers) + 0x20, ستون + 0x21, ردیف + 0x21);
    اكسر;

  حالة MOUSE_UTF8:
    {
      حرف utf8[18]; size_t len = 0;

      إذا(!pressed)
        کد = 3;

      len += fill_utf8((کد | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(ستون + 0x21, utf8 + len);
      len += fill_utf8(ردیف + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    اكسر;

  حالة MOUSE_SGR:
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        کد | modifiers, ستون + 1, ردیف + 1, pressed ? 'M' : 'm');
    اكسر;

  حالة MOUSE_RXVT:
    إذا(!pressed)
      کد = 3;

    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        کد | modifiers, ستون + 1, ردیف + 1);
    اكسر;
  }
}

فراغ vterm_mouse_move(VTerm *vt, صحيح ردیف, صحيح ستون, VTermModifier mod)
{
  VTermState *state = vt->state;

  إذا(ستون == state->mouse_ستون && ردیف == state->mouse_ردیف)
    أرجع;

  state->mouse_ستون = ستون;
  state->mouse_ردیف = ردیف;

  إذا((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    صحيح button = state->mouse_buttons & MOUSE_BUTTON_چپ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_میانه ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_راست ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, ستون, ردیف);
  }
}

فراغ vterm_mouse_button(VTerm *vt, صحيح button, صحيح pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  صحيح old_buttons = state->mouse_buttons;

  إذا(button > 0 && button <= 3) {
    إذا(pressed)
      state->mouse_buttons |= (1 << (button-1));
    وإلا
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  إذا(state->mouse_buttons == old_buttons && button < 4)
    أرجع;
  إذا (!(state->mouse_flags & MOUSE_WANT_CLICK))
    أرجع;

  إذا(!state->mouse_flags)
    أرجع;

  إذا(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_ستون, state->mouse_ردیف);
  }
  وإلا إذا(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_ستون, state->mouse_ردیف);
  }
}
