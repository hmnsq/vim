#ଅନ୍ତର୍ଭାଵ "vterm_internal.h"

#ଅନ୍ତର୍ଭାଵ "utf8.h"

ସ୍ଥିର ଶୂନ୍ୟ output_mouse(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା code, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pressed, ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifiers, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row)
{
  modifiers <<= 2;

  ବାଛ(state->mouse_protocol) {
  ସ୍ଥିତି MOUSE_X10:
    ଯଦି(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    ଯଦି(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    ଯଦି(!pressed)
      code = 3;

    vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି MOUSE_UTF8:
    {
      ଅକ୍ଷର utf8[18]; size_t len = 0;

      ଯଦି(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି MOUSE_SGR:
    vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    ଭାଙ୍ଗ;

  ସ୍ଥିତି MOUSE_RXVT:
    ଯଦି(!pressed)
      code = 3;

    vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    ଭାଙ୍ଗ;
  }
}

ଶୂନ୍ୟ vterm_mouse_move(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, VTermModifier mod)
{
  VTermState *state = vt->state;

  ଯଦି(col == state->mouse_col && row == state->mouse_row)
    ଫେରନ୍ତୁ;

  state->mouse_col = col;
  state->mouse_row = row;

  ଯଦି((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା button = state->mouse_buttons & MOUSE_BUTTON_ବାମ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_ମଧ୍ୟ ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ଡାହାଣ ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

ଶୂନ୍ୟ vterm_mouse_button(VTerm *vt, ପୂର୍ଣ୍ଣସଂଖ୍ୟା button, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_buttons = state->mouse_buttons;

  ଯଦି(button > 0 && button <= 3) {
    ଯଦି(pressed)
      state->mouse_buttons |= (1 << (button-1));
    ଅନ୍ୟଥା
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  ଯଦି(state->mouse_buttons == old_buttons && button < 4)
    ଫେରନ୍ତୁ;
  ଯଦି (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ଫେରନ୍ତୁ;

  ଯଦି(!state->mouse_flags)
    ଫେରନ୍ତୁ;

  ଯଦି(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  ଅନ୍ୟଥା ଯଦି(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
