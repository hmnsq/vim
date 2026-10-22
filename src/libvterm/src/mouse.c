#અન્તર્ભાવ "vterm_internal.h"

#અન્તર્ભાવ "utf8.h"

સ્થિર શૂન્ય output_mouse(VTermState *state, પૂર્ણાંક code, પૂર્ણાંક pressed, પૂર્ણાંક modifiers, પૂર્ણાંક col, પૂર્ણાંક row)
{
  modifiers <<= 2;

  પસંદગી(state->mouse_protocol) {
  સ્થિતિ MOUSE_X10:
    જો(col + 0x21 > 0xff)
      col = 0xff - 0x21;
    જો(row + 0x21 > 0xff)
      row = 0xff - 0x21;

    જો(!pressed)
      code = 3;

    vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (code | modifiers) + 0x20, col + 0x21, row + 0x21);
    તોડો;

  સ્થિતિ MOUSE_UTF8:
    {
      અક્ષર utf8[18]; size_t len = 0;

      જો(!pressed)
        code = 3;

      len += fill_utf8((code | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(col + 0x21, utf8 + len);
      len += fill_utf8(row + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    તોડો;

  સ્થિતિ MOUSE_SGR:
    vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        code | modifiers, col + 1, row + 1, pressed ? 'M' : 'm');
    તોડો;

  સ્થિતિ MOUSE_RXVT:
    જો(!pressed)
      code = 3;

    vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        code | modifiers, col + 1, row + 1);
    તોડો;
  }
}

શૂન્ય vterm_mouse_move(VTerm *vt, પૂર્ણાંક row, પૂર્ણાંક col, VTermModifier mod)
{
  VTermState *state = vt->state;

  જો(col == state->mouse_col && row == state->mouse_row)
    પાછા;

  state->mouse_col = col;
  state->mouse_row = row;

  જો((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    પૂર્ણાંક button = state->mouse_buttons & MOUSE_BUTTON_ડાબું ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_મધ્ય ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_જમણું ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, col, row);
  }
}

શૂન્ય vterm_mouse_button(VTerm *vt, પૂર્ણાંક button, પૂર્ણાંક pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  પૂર્ણાંક old_buttons = state->mouse_buttons;

  જો(button > 0 && button <= 3) {
    જો(pressed)
      state->mouse_buttons |= (1 << (button-1));
    નહીંતર
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  જો(state->mouse_buttons == old_buttons && button < 4)
    પાછા;
  જો (!(state->mouse_flags & MOUSE_WANT_CLICK))
    પાછા;

  જો(!state->mouse_flags)
    પાછા;

  જો(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_col, state->mouse_row);
  }
  નહીંતર જો(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_col, state->mouse_row);
  }
}
