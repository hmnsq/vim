#include "vterm_internal.h"

#include "utf8.h"

static void output_mouse(VTermState *state, 整数 code, 整数 pressed, 整数 modifiers, 整数 col, 整数 row)
{
  modifiers <<= 2;

  switch(state->mouse_protocol) {
  case MOUSE_X10:
    如果(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    如果(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    如果(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    break;

  case MOUSE_UTF8:
    {
      char utf8[18]; size_t len = 0;

      如果(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    break;

  case MOUSE_SGR:
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    break;

  case MOUSE_RXVT:
    如果(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    break;
  }
}

void vterm_mouse_move(VTerm *vt, 整数 row, 整数 col, VTermModifier mod)
{
  VTermState *state = vt->state;

  如果(col == state->mouse_col && row == state->mouse_row)
    返回;

  state->mouse_col = col;
  state->mouse_row = row;

  如果((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    整数 button = state->mouse_buttons & MOUSE_BUTTON_LEFT ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_MIDDLE ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_RIGHT ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

void vterm_mouse_button(VTerm *vt, 整数 button, 整数 pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  整数 old_buttons = state->mouse_buttons;

  如果(button > 0 && button <= 3) {
    如果(pressed)
      state->mouse_buttons |= (1 << (button-1));
    否则
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  如果(state->mouse_buttons == old_buttons && button < 4)
    返回;
  如果 (!(state->mouse_flags & MOUSE_WANT_CLICK))
    返回;

  如果(!state->mouse_flags)
    返回;

  如果(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  否则 如果(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
