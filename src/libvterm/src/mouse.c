#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ "utf8.h"

ಸ್ಥಿರ ಶೂನ್ಯ output_mouse(VTermState *state, ಪೂರ್ಣಾಙ್ಕ code, ಪೂರ್ಣಾಙ್ಕ pressed, ಪೂರ್ಣಾಙ್ಕ modifiers, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ row)
{
  modifiers <<= 2;

  ವಿಕಲ್ಪ(state->mouse_protocol) {
  ಪಕ್ಷ MOUSE_X10:
    ಯದಿ(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    ಯದಿ(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    ಯದಿ(!pressed)
      code = 3;

    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    ಭಙ್ಗ;

  ಪಕ್ಷ MOUSE_UTF8:
    {
      ವರ್ಣ utf8[18]; size_t len = 0;

      ಯದಿ(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ MOUSE_SGR:
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    ಭಙ್ಗ;

  ಪಕ್ಷ MOUSE_RXVT:
    ಯದಿ(!pressed)
      code = 3;

    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    ಭಙ್ಗ;
  }
}

ಶೂನ್ಯ vterm_mouse_move(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, VTermModifier mod)
{
  VTermState *state = vt->state;

  ಯದಿ(col == state->mouse_col && row == state->mouse_row)
    ಪ್ರತ್ಯಾಗಮ;

  state->mouse_col = col;
  state->mouse_row = row;

  ಯದಿ((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    ಪೂರ್ಣಾಙ್ಕ button = state->mouse_buttons & MOUSE_BUTTON_ಎಡ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_ಮಧ್ಯ ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ಬಲ ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

ಶೂನ್ಯ vterm_mouse_button(VTerm *vt, ಪೂರ್ಣಾಙ್ಕ button, ಪೂರ್ಣಾಙ್ಕ pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  ಪೂರ್ಣಾಙ್ಕ old_buttons = state->mouse_buttons;

  ಯದಿ(button > 0 && button <= 3) {
    ಯದಿ(pressed)
      state->mouse_buttons |= (1 << (button-1));
    ಅನ್ಯಥಾ
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  ಯದಿ(state->mouse_buttons == old_buttons && button < 4)
    ಪ್ರತ್ಯಾಗಮ;
  ಯದಿ (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(!state->mouse_flags)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  ಅನ್ಯಥಾ ಯದಿ(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
