#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव "utf8.h"

स्थिर शून्य output_mouse(VTermState *state, पूर्णाङ्क code, पूर्णाङ्क pressed, पूर्णाङ्क modifiers, पूर्णाङ्क col, पूर्णाङ्क row)
{
  modifiers <<= 2;

  विकल्प(state->mouse_protocol) {
  पक्ष MOUSE_X10:
    यदि(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    यदि(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    यदि(!pressed)
      code = 3;

    vterm_धकेलो_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    भङ्ग;

  पक्ष MOUSE_UTF8:
    {
      वर्ण utf8[18]; माप_प्रकार len = 0;

      यदि(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_धकेलो_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    भङ्ग;

  पक्ष MOUSE_SGR:
    vterm_धकेलो_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    भङ्ग;

  पक्ष MOUSE_RXVT:
    यदि(!pressed)
      code = 3;

    vterm_धकेलो_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    भङ्ग;
  }
}

शून्य vterm_mouse_move(VTerm *vt, पूर्णाङ्क row, पूर्णाङ्क col, VTermModifier mod)
{
  VTermState *state = vt->state;

  यदि(col == state->mouse_col && row == state->mouse_row)
    प्रत्यागम;

  state->mouse_col = col;
  state->mouse_row = row;

  यदि((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    पूर्णाङ्क button = state->mouse_buttons & MOUSE_BUTTON_बायाँ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_मध्य ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_दायाँ ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

शून्य vterm_mouse_button(VTerm *vt, पूर्णाङ्क button, पूर्णाङ्क pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  पूर्णाङ्क old_buttons = state->mouse_buttons;

  यदि(button > 0 && button <= 3) {
    यदि(pressed)
      state->mouse_buttons |= (1 << (button-1));
    अन्यथा
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  यदि(state->mouse_buttons == old_buttons && button < 4)
    प्रत्यागम;
  यदि (!(state->mouse_flags & MOUSE_WANT_CLICK))
    प्रत्यागम;

  यदि(!state->mouse_flags)
    प्रत्यागम;

  यदि(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  अन्यथा यदि(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
