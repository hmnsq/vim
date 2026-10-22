#شامل "vterm_internal.h"

#شامل "utf8.h"

ایستا تهی output_mouse(VTermState *state, صحیح code, صحیح pressed, صحیح modifiers, صحیح col, صحیح row)
{
  modifiers <<= 2;

  انتخاب(state->mouse_protocol) {
  حالت MOUSE_X10:
    اگر(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    اگر(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    اگر(!pressed)
      code = 3;

    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    بشکن;

  حالت MOUSE_UTF8:
    {
      نویسه utf8[18]; size_t len = 0;

      اگر(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    بشکن;

  حالت MOUSE_SGR:
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    بشکن;

  حالت MOUSE_RXVT:
    اگر(!pressed)
      code = 3;

    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    بشکن;
  }
}

تهی vterm_mouse_move(VTerm *vt, صحیح row, صحیح col, VTermModifier mod)
{
  VTermState *state = vt->state;

  اگر(col == state->mouse_col && row == state->mouse_row)
    بازگشت;

  state->mouse_col = col;
  state->mouse_row = row;

  اگر((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    صحیح button = state->mouse_buttons & MOUSE_BUTTON_چپ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_میانه ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_راست ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

تهی vterm_mouse_button(VTerm *vt, صحیح button, صحیح pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  صحیح old_buttons = state->mouse_buttons;

  اگر(button > 0 && button <= 3) {
    اگر(pressed)
      state->mouse_buttons |= (1 << (button-1));
    وگرنه
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  اگر(state->mouse_buttons == old_buttons && button < 4)
    بازگشت;
  اگر (!(state->mouse_flags & MOUSE_WANT_CLICK))
    بازگشت;

  اگر(!state->mouse_flags)
    بازگشت;

  اگر(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  وگرنه اگر(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
