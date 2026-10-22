#ضمن "vterm_internal.h"

#ضمن "utf8.h"

ساكن فراغ output_mouse(VTermState *state, صحيح ڪوڊ, صحيح pressed, صحيح modifiers, صحيح کانو, صحيح صف)
{
  modifiers <<= 2;

  بدل(state->mouse_protocol) {
  حالة MOUSE_X10:
    إذا(کانو + 0x21 > 0xff)
      کانو = 0xff - 0x21;
    إذا(صف + 0x21 > 0xff)
      صف = 0xff - 0x21;

    إذا(!pressed)
      ڪوڊ = 3;

    vterm_ڌڪو_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (ڪوڊ | modifiers) + 0x20, کانو + 0x21, صف + 0x21);
    اكسر;

  حالة MOUSE_UTF8:
    {
      حرف utf8[18]; size_t len = 0;

      إذا(!pressed)
        ڪوڊ = 3;

      len += fill_utf8((ڪوڊ | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(کانو + 0x21, utf8 + len);
      len += fill_utf8(صف + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ڌڪو_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    اكسر;

  حالة MOUSE_SGR:
    vterm_ڌڪو_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        ڪوڊ | modifiers, کانو + 1, صف + 1, pressed ? 'M' : 'm');
    اكسر;

  حالة MOUSE_RXVT:
    إذا(!pressed)
      ڪوڊ = 3;

    vterm_ڌڪو_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        ڪوڊ | modifiers, کانو + 1, صف + 1);
    اكسر;
  }
}

فراغ vterm_mouse_move(VTerm *vt, صحيح صف, صحيح کانو, VTermModifier mod)
{
  VTermState *state = vt->state;

  إذا(کانو == state->mouse_کانو && صف == state->mouse_صف)
    أرجع;

  state->mouse_کانو = کانو;
  state->mouse_صف = صف;

  إذا((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    صحيح button = state->mouse_buttons & MOUSE_BUTTON_کاٻو ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_وچ ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ساڄو ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, کانو, صف);
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
    output_mouse(state, button-1, pressed, mod, state->mouse_کانو, state->mouse_صف);
  }
  وإلا إذا(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_کانو, state->mouse_صف);
  }
}
