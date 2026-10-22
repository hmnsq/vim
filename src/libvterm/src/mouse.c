#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ "utf8.h"

ಸ್ಥಿರ ಶೂನ್ಯ output_mouse(VTermState *state, ಪೂರ್ಣಾಂಕ ಸಂಕೇತ_2, ಪೂರ್ಣಾಂಕ pressed, ಪೂರ್ಣಾಂಕ modifiers, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
  modifiers <<= 2;

  ಆಯ್ಕೆ(state->mouse_protocol) {
  ಸ್ಥಿತಿ MOUSE_X10:
    ಆದರೆ(ಲಂಬಸಾಲು + 0x21 > 0xff)
      ಲಂಬಸಾಲು = 0xff - 0x21;
    ಆದರೆ(ಅಡ್ಡಸಾಲು + 0x21 > 0xff)
      ಅಡ್ಡಸಾಲು = 0xff - 0x21;

    ಆದರೆ(!pressed)
      ಸಂಕೇತ_2 = 3;

    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (ಸಂಕೇತ_2 | modifiers) + 0x20, ಲಂಬಸಾಲು + 0x21, ಅಡ್ಡಸಾಲು + 0x21);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ MOUSE_UTF8:
    {
      ಅಕ್ಷರ utf8[18]; size_t len = 0;

      ಆದರೆ(!pressed)
        ಸಂಕೇತ_2 = 3;

      len += fill_utf8((ಸಂಕೇತ_2 | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(ಲಂಬಸಾಲು + 0x21, utf8 + len);
      len += fill_utf8(ಅಡ್ಡಸಾಲು + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ MOUSE_SGR:
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        ಸಂಕೇತ_2 | modifiers, ಲಂಬಸಾಲು + 1, ಅಡ್ಡಸಾಲು + 1, pressed ? 'M' : 'm');
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ MOUSE_RXVT:
    ಆದರೆ(!pressed)
      ಸಂಕೇತ_2 = 3;

    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        ಸಂಕೇತ_2 | modifiers, ಲಂಬಸಾಲು + 1, ಅಡ್ಡಸಾಲು + 1);
    ನಿಲ್ಲಿಸು;
  }
}

ಶೂನ್ಯ vterm_mouse_move(VTerm *vt, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, VTermModifier mod)
{
  VTermState *state = vt->state;

  ಆದರೆ(ಲಂಬಸಾಲು == state->mouse_ಲಂಬಸಾಲು && ಅಡ್ಡಸಾಲು == state->mouse_ಅಡ್ಡಸಾಲು)
    ಹಿಂತಿರುಗಿ;

  state->mouse_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
  state->mouse_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;

  ಆದರೆ((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    ಪೂರ್ಣಾಂಕ button = state->mouse_buttons & MOUSE_BUTTON_ಎಡ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_ಮಧ್ಯ ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_ಬಲ ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು);
  }
}

ಶೂನ್ಯ vterm_mouse_button(VTerm *vt, ಪೂರ್ಣಾಂಕ button, ಪೂರ್ಣಾಂಕ pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  ಪೂರ್ಣಾಂಕ old_buttons = state->mouse_buttons;

  ಆದರೆ(button > 0 && button <= 3) {
    ಆದರೆ(pressed)
      state->mouse_buttons |= (1 << (button-1));
    ಇಲ್ಲದಿದ್ದರೆ
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  ಆದರೆ(state->mouse_buttons == old_buttons && button < 4)
    ಹಿಂತಿರುಗಿ;
  ಆದರೆ (!(state->mouse_flags & MOUSE_WANT_CLICK))
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(!state->mouse_flags)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_ಲಂಬಸಾಲು, state->mouse_ಅಡ್ಡಸಾಲು);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_ಲಂಬಸಾಲು, state->mouse_ಅಡ್ಡಸಾಲು);
  }
}
