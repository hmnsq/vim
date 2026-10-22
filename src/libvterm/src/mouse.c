#অন্তর্ভাৱ "vterm_internal.h"

#অন্তর্ভাৱ "utf8.h"

স্থির শূন্য output_mouse(VTermState *state, পূর্ণাঙ্ক code, পূর্ণাঙ্ক pressed, পূর্ণাঙ্ক modifiers, পূর্ণাঙ্ক col, পূর্ণাঙ্ক row)
{
  modifiers <<= 2;

  ৱিকল্প(state->mouse_protocol) {
  পক্ষ MOUSE_X10:
    যদি(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    যদি(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    যদি(!pressed)
      code = 3;

    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    ভঙ্গ;

  পক্ষ MOUSE_UTF8:
    {
      ৱর্ণ utf8[18]; size_t len = 0;

      যদি(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    ভঙ্গ;

  পক্ষ MOUSE_SGR:
    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    ভঙ্গ;

  পক্ষ MOUSE_RXVT:
    যদি(!pressed)
      code = 3;

    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    ভঙ্গ;
  }
}

শূন্য vterm_mouse_move(VTerm *vt, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col, VTermModifier mod)
{
  VTermState *state = vt->state;

  যদি(col == state->mouse_col && row == state->mouse_row)
    ঘূৰাই;

  state->mouse_col = col;
  state->mouse_row = row;

  যদি((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    পূর্ণাঙ্ক button = state->mouse_buttons & MOUSE_BUTTON_বাম ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_মধ্য ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ডান ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

শূন্য vterm_mouse_button(VTerm *vt, পূর্ণাঙ্ক button, পূর্ণাঙ্ক pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  পূর্ণাঙ্ক old_buttons = state->mouse_buttons;

  যদি(button > 0 && button <= 3) {
    যদি(pressed)
      state->mouse_buttons |= (1 << (button-1));
    অন্যথা
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  যদি(state->mouse_buttons == old_buttons && button < 4)
    ঘূৰাই;
  যদি (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ঘূৰাই;

  যদি(!state->mouse_flags)
    ঘূৰাই;

  যদি(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  অন্যথা যদি(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
