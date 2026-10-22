#అన్తర్భావ "vterm_internal.h"

#అన్తర్భావ "utf8.h"

స్థిర శూన్య output_mouse(VTermState *state, పూర్ణాంకం సంకేతం_2, పూర్ణాంకం pressed, పూర్ణాంకం modifiers, పూర్ణాంకం నిలువు, పూర్ణాంకం అడ్డువరుస)
{
  modifiers <<= 2;

  ఎంపిక(state->mouse_protocol) {
  స్థితి MOUSE_X10:
    ఒకవేళ(నిలువు + 0x21 > 0xff)
      నిలువు = 0xff - 0x21;
    ఒకవేళ(అడ్డువరుస + 0x21 > 0xff)
      అడ్డువరుస = 0xff - 0x21;

    ఒకవేళ(!pressed)
      సంకేతం_2 = 3;

    vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "M%c%c%c",
        (సంకేతం_2 | modifiers) + 0x20, నిలువు + 0x21, అడ్డువరుస + 0x21);
    ఆపు;

  స్థితి MOUSE_UTF8:
    {
      అక్షరం utf8[18]; size_t len = 0;

      ఒకవేళ(!pressed)
        సంకేతం_2 = 3;

      len += fill_utf8((సంకేతం_2 | modifiers) + 0x20, utf8 + len);
      len += fill_utf8(నిలువు + 0x21, utf8 + len);
      len += fill_utf8(అడ్డువరుస + 0x21, utf8 + len);
      utf8[len] = 0;

      vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "M%s", utf8);
    }
    ఆపు;

  స్థితి MOUSE_SGR:
    vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "<%d;%d;%d%c",
        సంకేతం_2 | modifiers, నిలువు + 1, అడ్డువరుస + 1, pressed ? 'M' : 'm');
    ఆపు;

  స్థితి MOUSE_RXVT:
    ఒకవేళ(!pressed)
      సంకేతం_2 = 3;

    vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "%d;%d;%dM",
        సంకేతం_2 | modifiers, నిలువు + 1, అడ్డువరుస + 1);
    ఆపు;
  }
}

శూన్య vterm_mouse_move(VTerm *vt, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, VTermModifier mod)
{
  VTermState *state = vt->state;

  ఒకవేళ(నిలువు == state->mouse_నిలువు && అడ్డువరుస == state->mouse_అడ్డువరుస)
    తిరిగి;

  state->mouse_నిలువు = నిలువు;
  state->mouse_అడ్డువరుస = అడ్డువరుస;

  ఒకవేళ((state->mouse_flags & MOUSE_WANT_DRAG && state->mouse_buttons) ||
     (state->mouse_flags & MOUSE_WANT_MOVE)) {
    పూర్ణాంకం button = state->mouse_buttons & MOUSE_BUTTON_ఎడమ ? 1 :
                 state->mouse_buttons & MOUSE_BUTTON_మధ్య ? 2 :
                 state->mouse_buttons & MOUSE_BUTTON_కుడి ? 3 : 4;
    output_mouse(state, button-1 + 0x20, 1, mod, నిలువు, అడ్డువరుస);
  }
}

శూన్య vterm_mouse_button(VTerm *vt, పూర్ణాంకం button, పూర్ణాంకం pressed, VTermModifier mod)
{
  VTermState *state = vt->state;

  పూర్ణాంకం old_buttons = state->mouse_buttons;

  ఒకవేళ(button > 0 && button <= 3) {
    ఒకవేళ(pressed)
      state->mouse_buttons |= (1 << (button-1));
    లేకపోతే
      state->mouse_buttons &= ~(1 << (button-1));
  }

  /* Most of the time we don't get button releases from 4/5/6/7 */
  ఒకవేళ(state->mouse_buttons == old_buttons && button < 4)
    తిరిగి;
  ఒకవేళ (!(state->mouse_flags & MOUSE_WANT_CLICK))
    తిరిగి;

  ఒకవేళ(!state->mouse_flags)
    తిరిగి;

  ఒకవేళ(button < 4) {
    output_mouse(state, button-1, pressed, mod, state->mouse_నిలువు, state->mouse_అడ్డువరుస);
  }
  లేకపోతే ఒకవేళ(button < 8) {
    output_mouse(state, button-4 + 0x40, pressed, mod, state->mouse_నిలువు, state->mouse_అడ్డువరుస);
  }
}
