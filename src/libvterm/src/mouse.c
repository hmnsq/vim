#ضمن "vterm_internal.h"

#ضمن "utf8.h"

ساكن فراغ output_mouse(VTermState *state, صحيح کوډ, صحيح pressed, صحيح modifiers, صحيح ستون, صحيح لیکه)
{
  modifiers <<= 2;

  بدل(state->mouse_protocol) {
  حالة MOUSE_X10:
    إذا(ستون + 0x21 > 0xff)
      ستون = 0xff - 0x21;
    إذا(لیکه + 0x21 > 0xff)
      لیکه = 0xff - 0x21;

    إذا(!pressed)
      کوډ = 3;

    vterm_ټېل_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (کوډ | modifiers) + 0x20, ستون + 0x21, لیکه + 0x21);
    اكسر;

  حالة MOUSE_UTF8:
    {
      حرف utf8[18]; size_t len = 0;

      إذا(!pressed)
        کوډ = 3;

      len += fill_utf8((کوډ | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(ستون + 0x21, utf8 + len);
      len += fill_utf8(لیکه + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ټېل_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    اكسر;

  حالة MOUSE_SGR:
    vterm_ټېل_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        کوډ | modifiers, ستون + 1, لیکه + 1, pressed ? 'M' : 'm');
    اكسر;

  حالة MOUSE_RXVT:
    إذا(!pressed)
      کوډ = 3;

    vterm_ټېل_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        کوډ | modifiers, ستون + 1, لیکه + 1);
    اكسر;
  }
}

فراغ vterm_mouse_move(VTerm *vt, صحيح لیکه, صحيح ستون, VTermModifier mod)
{
  VTermState *state = vt->state;

  إذا(ستون == state->mouse_ستون && لیکه == state->mouse_لیکه)
    أرجع;

  state->mouse_ستون = ستون;
  state->mouse_لیکه = لیکه;

  إذا((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    صحيح button = state->mouse_buttons & MOUSE_BUTTON_کیڼ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_منځ ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ښی ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, ستون, لیکه);
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
    output_mouse(state, button-1, pressed, mod, state->mouse_ستون, state->mouse_لیکه);
  }
  وإلا إذا(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_ستون, state->mouse_لیکه);
  }
}
