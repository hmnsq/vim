#include "vterm_internal.h"

#include "utf8.h"

static void output_mouse(VTermState *state, ཧྲིལ code, ཧྲིལ pressed, ཧྲིལ modifiers, ཧྲིལ col, ཧྲིལ row)
{
  modifiers <<= 2;

  switch(state->mouse_protocol) {
  pak_sa MOUSE_X10:
    གལ་སྲིད(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    གལ་སྲིད(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    གལ་སྲིད(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    bha_nga;

  pak_sa MOUSE_UTF8:
    {
      var_na utf8[18]; size_t len = 0;

      གལ་སྲིད(!pressed)
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
    གལ་སྲིད(!pressed)
      code = 3;

    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    bha_nga;
  }
}

void vterm_mouse_move(VTerm *vt, ཧྲིལ row, ཧྲིལ col, VTermModifier mod)
{
  VTermState *state = vt->state;

  གལ་སྲིད(col == state->mouse_col && row == state->mouse_row)
    ལོག;

  state->mouse_col = col;
  state->mouse_row = row;

  གལ་སྲིད((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    ཧྲིལ button = state->mouse_buttons & MOUSE_BUTTON_LEFT ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_MIDDLE ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_RIGHT ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

void vterm_mouse_button(VTerm *vt, ཧྲིལ button, ཧྲིལ pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  ཧྲིལ old_buttons = state->mouse_buttons;

  གལ་སྲིད(button > 0 && button <= 3) {
    གལ་སྲིད(pressed)
      state->mouse_buttons |= (1 << (button-1));
    དེ་མིན
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  གལ་སྲིད(state->mouse_buttons == old_buttons && button < 4)
    ལོག;
  གལ་སྲིད (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ལོག;

  གལ་སྲིད(!state->mouse_flags)
    ལོག;

  གལ་སྲིད(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  དེ་མིན གལ་སྲིད(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
