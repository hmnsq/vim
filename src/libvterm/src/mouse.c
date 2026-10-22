#include "vterm_internal.h"

#include "utf8.h"

static void output_mouse(VTermState *state, ຈຳນວນເຕັມ code, ຈຳນວນເຕັມ pressed, ຈຳນວນເຕັມ modifiers, ຈຳນວນເຕັມ col, ຈຳນວນເຕັມ row)
{
  modifiers <<= 2;

  switch(state->mouse_protocol) {
  pak_sa MOUSE_X10:
    ຖ້າ(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    ຖ້າ(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    ຖ້າ(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    bha_nga;

  pak_sa MOUSE_UTF8:
    {
      var_na utf8[18]; size_t len = 0;

      ຖ້າ(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    bha_nga;

  pak_sa MOUSE_SGR:
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    bha_nga;

  pak_sa MOUSE_RXVT:
    ຖ້າ(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    bha_nga;
  }
}

void vterm_mouse_move(VTerm *vt, ຈຳນວນເຕັມ row, ຈຳນວນເຕັມ col, VTermModifier mod)
{
  VTermState *state = vt->state;

  ຖ້າ(col == state->mouse_col && row == state->mouse_row)
    ສົ່ງຄືນ;

  state->mouse_col = col;
  state->mouse_row = row;

  ຖ້າ((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    ຈຳນວນເຕັມ button = state->mouse_buttons & MOUSE_BUTTON_LEFT ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_MIDDLE ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_RIGHT ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

void vterm_mouse_button(VTerm *vt, ຈຳນວນເຕັມ button, ຈຳນວນເຕັມ pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  ຈຳນວນເຕັມ old_buttons = state->mouse_buttons;

  ຖ້າ(button > 0 && button <= 3) {
    ຖ້າ(pressed)
      state->mouse_buttons |= (1 << (button-1));
    ບໍ່ດັ່ງນັ້ນ
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  ຖ້າ(state->mouse_buttons == old_buttons && button < 4)
    ສົ່ງຄືນ;
  ຖ້າ (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ສົ່ງຄືນ;

  ຖ້າ(!state->mouse_flags)
    ສົ່ງຄືນ;

  ຖ້າ(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  ບໍ່ດັ່ງນັ້ນ ຖ້າ(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
