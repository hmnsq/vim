#ضمن "vterm_internal.h"

#ضمن "utf8.h"

ساكن فراغ output_mouse(VTermState *state, صحيح code, صحيح pressed, صحيح modifiers, صحيح col, صحيح row)
{
  modifiers <<= 2;

  بدل(state->mouse_protocol) {
  حالة MOUSE_X10:
    אם(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    אם(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    אם(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    اكسر;

  حالة MOUSE_UTF8:
    {
      حرف utf8[18]; size_t len = 0;

      אם(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    اكسر;

  حالة MOUSE_SGR:
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    اكسر;

  حالة MOUSE_RXVT:
    אם(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    اكسر;
  }
}

فراغ vterm_mouse_move(VTerm *vt, صحيح row, صحيح col, VTermModifier mod)
{
  VTermState *state = vt->state;

  אם(col == state->mouse_col && row == state->mouse_row)
    החזר;

  state->mouse_col = col;
  state->mouse_row = row;

  אם((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    صحيح button = state->mouse_buttons & MOUSE_BUTTON_LEFT ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_MIDDLE ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_RIGHT ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

فراغ vterm_mouse_button(VTerm *vt, صحيح button, صحيح pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  صحيح old_buttons = state->mouse_buttons;

  אם(button > 0 && button <= 3) {
    אם(pressed)
      state->mouse_buttons |= (1 << (button-1));
    אחרת
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  אם(state->mouse_buttons == old_buttons && button < 4)
    החזר;
  אם (!(state->mouse_flags & MOUSE_WANT_CLICK))
    החזר;

  אם(!state->mouse_flags)
    החזר;

  אם(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  אחרת אם(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
