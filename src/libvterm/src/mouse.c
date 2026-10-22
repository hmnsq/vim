#അന്തര്ഭാവ "vterm_internal.h"

#അന്തര്ഭാവ "utf8.h"

സ്ഥിതം ശൂന്യം output_mouse(VTermState *state, പൂർണ്ണസംഖ്യ code, പൂർണ്ണസംഖ്യ pressed, പൂർണ്ണസംഖ്യ modifiers, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ row)
{
  modifiers <<= 2;

  തിരഞ്ഞെടുപ്പ്(state->mouse_protocol) {
  അവസ്ഥ MOUSE_X10:
    എങ്കിൽ(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    എങ്കിൽ(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    എങ്കിൽ(!pressed)
      code = 3;

    vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    നിർത്തുക;

  അവസ്ഥ MOUSE_UTF8:
    {
      അക്ഷരം utf8[18]; size_t len = 0;

      എങ്കിൽ(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    നിർത്തുക;

  അവസ്ഥ MOUSE_SGR:
    vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    നിർത്തുക;

  അവസ്ഥ MOUSE_RXVT:
    എങ്കിൽ(!pressed)
      code = 3;

    vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    നിർത്തുക;
  }
}

ശൂന്യം vterm_mouse_move(VTerm *vt, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, VTermModifier mod)
{
  VTermState *state = vt->state;

  എങ്കിൽ(col == state->mouse_col && row == state->mouse_row)
    തിരികെ;

  state->mouse_col = col;
  state->mouse_row = row;

  എങ്കിൽ((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    പൂർണ്ണസംഖ്യ button = state->mouse_buttons & MOUSE_BUTTON_ഇടത് ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_നടുവ് ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_വലത് ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

ശൂന്യം vterm_mouse_button(VTerm *vt, പൂർണ്ണസംഖ്യ button, പൂർണ്ണസംഖ്യ pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  പൂർണ്ണസംഖ്യ old_buttons = state->mouse_buttons;

  എങ്കിൽ(button > 0 && button <= 3) {
    എങ്കിൽ(pressed)
      state->mouse_buttons |= (1 << (button-1));
    അല്ലെങ്കിൽ
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  എങ്കിൽ(state->mouse_buttons == old_buttons && button < 4)
    തിരികെ;
  എങ്കിൽ (!(state->mouse_flags & MOUSE_WANT_CLICK))
    തിരികെ;

  എങ്കിൽ(!state->mouse_flags)
    തിരികെ;

  എങ്കിൽ(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
