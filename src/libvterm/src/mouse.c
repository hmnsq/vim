#অন্তর্ভাব "vterm_internal.h"

#অন্তর্ভাব "utf8.h"

স্থির শূন্য output_mouse(VTermState *state, পূর্ণসংখ্যা code, পূর্ণসংখ্যা pressed, পূর্ণসংখ্যা modifiers, পূর্ণসংখ্যা col, পূর্ণসংখ্যা row)
{
  modifiers <<= 2;

  নির্বাচন(state->mouse_protocol) {
  ক্ষেত্র MOUSE_X10:
    যদি(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    যদি(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    যদি(!pressed)
      code = 3;

    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    বিরতি;

  ক্ষেত্র MOUSE_UTF8:
    {
      অক্ষর utf8[18]; size_t len = 0;

      যদি(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    বিরতি;

  ক্ষেত্র MOUSE_SGR:
    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    বিরতি;

  ক্ষেত্র MOUSE_RXVT:
    যদি(!pressed)
      code = 3;

    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    বিরতি;
  }
}

শূন্য vterm_mouse_move(VTerm *vt, পূর্ণসংখ্যা row, পূর্ণসংখ্যা col, VTermModifier mod)
{
  VTermState *state = vt->state;

  যদি(col == state->mouse_col && row == state->mouse_row)
    ফেরত;

  state->mouse_col = col;
  state->mouse_row = row;

  যদি((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    পূর্ণসংখ্যা button = state->mouse_buttons & MOUSE_BUTTON_বাম ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_মধ্য ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ডান ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

শূন্য vterm_mouse_button(VTerm *vt, পূর্ণসংখ্যা button, পূর্ণসংখ্যা pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  পূর্ণসংখ্যা old_buttons = state->mouse_buttons;

  যদি(button > 0 && button <= 3) {
    যদি(pressed)
      state->mouse_buttons |= (1 << (button-1));
    অন্যথা
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  যদি(state->mouse_buttons == old_buttons && button < 4)
    ফেরত;
  যদি (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ফেরত;

  যদি(!state->mouse_flags)
    ফেরত;

  যদি(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  অন্যথা যদি(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
