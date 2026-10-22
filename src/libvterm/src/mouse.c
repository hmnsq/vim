#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव "utf8.h"

स्थिर शून्य output_mouse(VTermState *state, पूर्णांक code, पूर्णांक pressed, पूर्णांक modifiers, पूर्णांक col, पूर्णांक row)
{
  modifiers <<= 2;

  निवड(state->mouse_protocol) {
  स्थिती MOUSE_X10:
    जर(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    जर(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    जर(!pressed)
      code = 3;

    vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    थांबा;

  स्थिती MOUSE_UTF8:
    {
      अक्षर utf8[18]; माप_प्रकार len = 0;

      जर(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    थांबा;

  स्थिती MOUSE_SGR:
    vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    थांबा;

  स्थिती MOUSE_RXVT:
    जर(!pressed)
      code = 3;

    vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    थांबा;
  }
}

शून्य vterm_mouse_move(VTerm *vt, पूर्णांक row, पूर्णांक col, VTermModifier mod)
{
  VTermState *state = vt->state;

  जर(col == state->mouse_col && row == state->mouse_row)
    परत;

  state->mouse_col = col;
  state->mouse_row = row;

  जर((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    पूर्णांक button = state->mouse_buttons & MOUSE_BUTTON_डावा ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_मधला ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_उजवा ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

शून्य vterm_mouse_button(VTerm *vt, पूर्णांक button, पूर्णांक pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  पूर्णांक old_buttons = state->mouse_buttons;

  जर(button > 0 && button <= 3) {
    जर(pressed)
      state->mouse_buttons |= (1 << (button-1));
    अन्यथा
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  जर(state->mouse_buttons == old_buttons && button < 4)
    परत;
  जर (!(state->mouse_flags & MOUSE_WANT_CLICK))
    परत;

  जर(!state->mouse_flags)
    परत;

  जर(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  अन्यथा जर(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
