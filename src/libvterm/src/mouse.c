#அந்தர்பாவ "vterm_internal.h"

#அந்தர்பாவ "utf8.h"

நிலையான வெறுமை output_mouse(VTermState *state, முழு code, முழு pressed, முழு modifiers, முழு col, முழு row)
{
  modifiers <<= 2;

  தேர்வு(state->mouse_protocol) {
  நிலை MOUSE_X10:
    எனில்(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    எனில்(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    எனில்(!pressed)
      code = 3;

    vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    நிறுத்து;

  நிலை MOUSE_UTF8:
    {
      எழுத்து utf8[18]; size_t len = 0;

      எனில்(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    நிறுத்து;

  நிலை MOUSE_SGR:
    vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    நிறுத்து;

  நிலை MOUSE_RXVT:
    எனில்(!pressed)
      code = 3;

    vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    நிறுத்து;
  }
}

வெறுமை vterm_mouse_move(VTerm *vt, முழு row, முழு col, VTermModifier mod)
{
  VTermState *state = vt->state;

  எனில்(col == state->mouse_col && row == state->mouse_row)
    திரும்பு;

  state->mouse_col = col;
  state->mouse_row = row;

  எனில்((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    முழு button = state->mouse_buttons & MOUSE_BUTTON_இடது ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_நடு ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_வலது ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

வெறுமை vterm_mouse_button(VTerm *vt, முழு button, முழு pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  முழு old_buttons = state->mouse_buttons;

  எனில்(button > 0 && button <= 3) {
    எனில்(pressed)
      state->mouse_buttons |= (1 << (button-1));
    இல்லையெனில்
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  எனில்(state->mouse_buttons == old_buttons && button < 4)
    திரும்பு;
  எனில் (!(state->mouse_flags & MOUSE_WANT_CLICK))
    திரும்பு;

  எனில்(!state->mouse_flags)
    திரும்பு;

  எனில்(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  இல்லையெனில் எனில்(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
