#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ವ್ಯಾಖ್ಯಾನಿಸು strneq(a,b,n) (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(a,b,n)==0)

#ಆದರೆ defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_resize(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಶೂನ್ಯ *user);

/* Some convenient wrappers to make callback functions easier */

ಸ್ಥಿರ ಶೂನ್ಯ putglyph(VTermState *state, ಸ್ಥಿರಾಂಕ uint32_t chars[], ಪೂರ್ಣಾಂಕ ಅಗಲ, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.ಅಗಲ = ಅಗಲ;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.ಅಡ್ಡಸಾಲು].doublewidth;
  info.dhl = state->lineinfo[pos.ಅಡ್ಡಸಾಲು].doubleheight;

  ಆದರೆ(state->callbacks && state->callbacks->putglyph)
    ಆದರೆ((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      ಹಿಂತಿರುಗಿ;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.ಲಂಬಸಾಲು, pos.ಅಡ್ಡಸಾಲು);
}

ಸ್ಥಿರ ಶೂನ್ಯ updatecursor(VTermState *state, VTermPos *oldpos, ಪೂರ್ಣಾಂಕ cancel_phantom)
{
  ಆದರೆ(state->pos.ಲಂಬಸಾಲು == oldpos->ಲಂಬಸಾಲು && state->pos.ಅಡ್ಡಸಾಲು == oldpos->ಅಡ್ಡಸಾಲು)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(cancel_phantom)
    state->at_phantom = 0;

  ಆದರೆ(state->callbacks && state->callbacks->movecursor)
    ಆದರೆ((*state->callbacks->movecursor)(state->pos, *oldpos, state->ಬಹುಲಕ.cursor_visible, state->cbdata))
      ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ ಶೂನ್ಯ erase(VTermState *state, VTermRect rect, ಪೂರ್ಣಾಂಕ selective)
{
  ಆದರೆ(rect.ಕೊನೆ_ಲಂಬಸಾಲು == state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    ಗಾಗಿ(ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು + 1; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು + 1 && ಅಡ್ಡಸಾಲು < state->ಅಡ್ಡಸಾಲುಗಳು_2; ಅಡ್ಡಸಾಲು++)
      state->lineinfo[ಅಡ್ಡಸಾಲು].continuation = 0;
  }

  ಆದರೆ(state->callbacks && state->callbacks->erase)
    ಆದರೆ((*state->callbacks->erase)(rect, selective, state->cbdata))
      ಹಿಂತಿರುಗಿ;
}

ಸ್ಥಿರ VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, ಗಾತ್ರ(VTermState));

  ಆದರೆ (state == ಖಾಲಿ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  state->vt = vt;

  state->ಅಡ್ಡಸಾಲುಗಳು_2 = vt->ಅಡ್ಡಸಾಲುಗಳು_2;
  state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = vt->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  state->mouse_ಲಂಬಸಾಲು     = 0;
  state->mouse_ಅಡ್ಡಸಾಲು     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ಖಾಲಿ;
  state->cbdata    = ಖಾಲಿ;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ಖಾಲಿ;
  state->selection.user      = ಖಾಲಿ;
  state->selection.ಬಫರ್    = ಖಾಲಿ;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_ಗಾತ್ರ = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_ಗಾತ್ರ * ಗಾತ್ರ(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->ಅಡ್ಡಸಾಲುಗಳು_2 * ಗಾತ್ರ(VTerm_ಸಾಲು_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->ಅಡ್ಡಸಾಲುಗಳು_2 * ಗಾತ್ರ(VTerm_ಸಾಲು_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  ಆದರೆ(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ state;
}

ಆಂತರಿಕ ಶೂನ್ಯ vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  ಆದರೆ(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ಸ್ಥಿರ ಶೂನ್ಯ scroll(VTermState *state, VTermRect rect, ಪೂರ್ಣಾಂಕ downward, ಪೂರ್ಣಾಂಕ rightward)
{
  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2;
  ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
  ಆದರೆ(!downward && !rightward)
    ಹಿಂತಿರುಗಿ;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  ಆದರೆ(rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು <= rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು || rect.ಕೊನೆ_ಲಂಬಸಾಲು <= rect.ಆರಂಭ_ಲಂಬಸಾಲು)
    ಹಿಂತಿರುಗಿ;

  ಅಡ್ಡಸಾಲುಗಳು_2 = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು - rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು;
  ಆದರೆ(downward > ಅಡ್ಡಸಾಲುಗಳು_2)
    downward = ಅಡ್ಡಸಾಲುಗಳು_2;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(downward < -ಅಡ್ಡಸಾಲುಗಳು_2)
    downward = -ಅಡ್ಡಸಾಲುಗಳು_2;

  ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = rect.ಕೊನೆ_ಲಂಬಸಾಲು - rect.ಆರಂಭ_ಲಂಬಸಾಲು;
  ಆದರೆ(rightward > ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    rightward = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rightward < -ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    rightward = -ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಆದರೆ(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    ಆದರೆ(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, .ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು + downward,
          .ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು, .ಕೊನೆ_ಲಂಬಸಾಲು = rect.ಕೊನೆ_ಲಂಬಸಾಲು}, state->cbdata);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು + downward, .ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು,
          .ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು,          .ಕೊನೆ_ಲಂಬಸಾಲು = rect.ಕೊನೆ_ಲಂಬಸಾಲು}, state->cbdata);

    ಆದರೆ(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು, .ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು,
          .ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು, .ಕೊನೆ_ಲಂಬಸಾಲು = rect.ಆರಂಭ_ಲಂಬಸಾಲು + rightward}, state->cbdata);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .ಆರಂಭ_ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು,           .ಕೊನೆ_ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು,
          .ಆರಂಭ_ಲಂಬಸಾಲು = rect.ಕೊನೆ_ಲಂಬಸಾಲು + rightward, .ಕೊನೆ_ಲಂಬಸಾಲು = rect.ಕೊನೆ_ಲಂಬಸಾಲು}, state->cbdata);
  }

  // Update lineinfo if full line
  ಆದರೆ(rect.ಆರಂಭ_ಲಂಬಸಾಲು == 0 && rect.ಕೊನೆ_ಲಂಬಸಾಲು == state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ && rightward == 0) {
    ಪೂರ್ಣಾಂಕ ಎತ್ತರ = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು - rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು - ನಿರಪೇಕ್ಷ(downward);
    ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;
    VTerm_ಸಾಲು_Info zero_ಸಾಲು_Info = {0x0};

    ಆದರೆ(downward > 0) {
      memmove(state->lineinfo + rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು,
              state->lineinfo + rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು + downward,
              ಎತ್ತರ * ಗಾತ್ರ(state->lineinfo[0]));
      ಗಾಗಿ(ಅಡ್ಡಸಾಲು = rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು - downward; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++)
        state->lineinfo[ಅಡ್ಡಸಾಲು] = zero_ಸಾಲು_Info;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      memmove(state->lineinfo + rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು - downward,
              state->lineinfo + rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು,
              ಎತ್ತರ * ಗಾತ್ರ(state->lineinfo[0]));
      ಗಾಗಿ(ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು - downward; ಅಡ್ಡಸಾಲು++)
        state->lineinfo[ಅಡ್ಡಸಾಲು] = zero_ಸಾಲು_Info;
    }
  }

  ಆದರೆ(state->callbacks && state->callbacks->scrollrect)
    ಆದರೆ((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      ಹಿಂತಿರುಗಿ;

  ಆದರೆ(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ಸ್ಥಿರ ಶೂನ್ಯ linefeed(VTermState *state)
{
  ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->scrollregion_ತುದಿ;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = SCROLLREGION_ಎಡ(state);
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, 1, 0);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು < state->ಅಡ್ಡಸಾಲುಗಳು_2-1)
    state->pos.ಅಡ್ಡಸಾಲು++;
}

ಸ್ಥಿರ ಶೂನ್ಯ grow_combine_ಬಫರ್(VTermState *state)
{
  size_t    new_ಗಾತ್ರ = state->combine_chars_ಗಾತ್ರ * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_ಗಾತ್ರ * ಗಾತ್ರ(new_chars[0]));

  ಸ್ಮರಣೆ_ನಕಲು(new_chars, state->combine_chars, state->combine_chars_ಗಾತ್ರ * ಗಾತ್ರ(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_ಗಾತ್ರ = new_ಗಾತ್ರ;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ಲಂಬಸಾಲು_tabstop(VTermState *state, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮುಖವಾಡ = 1 << (ಲಂಬಸಾಲು & 7);
  state->tabstops[ಲಂಬಸಾಲು >> 3] |= ಮುಖವಾಡ;
}

ಸ್ಥಿರ ಶೂನ್ಯ clear_ಲಂಬಸಾಲು_tabstop(VTermState *state, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮುಖವಾಡ = 1 << (ಲಂಬಸಾಲು & 7);
  state->tabstops[ಲಂಬಸಾಲು >> 3] &= ~ಮುಖವಾಡ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_ಲಂಬಸಾಲು_tabstop(VTermState *state, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮುಖವಾಡ = 1 << (ಲಂಬಸಾಲು & 7);
  ಹಿಂತಿರುಗಿ state->tabstops[ಲಂಬಸಾಲು >> 3] & ಮುಖವಾಡ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_cursor_ಒಳ_scrollregion(ಸ್ಥಿರಾಂಕ VTermState *state)
{
  ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು < state->scrollregion_ತುದಿ ||
     state->pos.ಅಡ್ಡಸಾಲು >= SCROLLREGION_BOTTOM(state))
    ಹಿಂತಿರುಗಿ 0;
  ಆದರೆ(state->pos.ಲಂಬಸಾಲು < SCROLLREGION_ಎಡ(state) ||
     state->pos.ಲಂಬಸಾಲು >= SCROLLREGION_ಬಲ(state))
    ಹಿಂತಿರುಗಿ 0;

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ tab(VTermState *state, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ direction)
{
  ಆಗ(ಎಣಿಕೆ_2 > 0) {
    ಆದರೆ(direction > 0) {
      ಆದರೆ(state->pos.ಲಂಬಸಾಲು >= THISROWWIDTH(state)-1)
        ಹಿಂತಿರುಗಿ;

      state->pos.ಲಂಬಸಾಲು++;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(direction < 0) {
      ಆದರೆ(state->pos.ಲಂಬಸಾಲು < 1)
        ಹಿಂತಿರುಗಿ;

      state->pos.ಲಂಬಸಾಲು--;
    }

    ಆದರೆ(is_ಲಂಬಸಾಲು_tabstop(state, state->pos.ಲಂಬಸಾಲು))
      ಎಣಿಕೆ_2--;
  }
}

#ವ್ಯಾಖ್ಯಾನಿಸು NO_FORCE 0
#ವ್ಯಾಖ್ಯಾನಿಸು FORCE    1

#ವ್ಯಾಖ್ಯಾನಿಸು DWL_OFF 0
#ವ್ಯಾಖ್ಯಾನಿಸು DWL_ON  1

#ವ್ಯಾಖ್ಯಾನಿಸು DHL_OFF    0
#ವ್ಯಾಖ್ಯಾನಿಸು DHL_ತುದಿ    1
#ವ್ಯಾಖ್ಯಾನಿಸು DHL_BOTTOM 2

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_lineinfo(VTermState *state, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ force, ಪೂರ್ಣಾಂಕ dwl, ಪೂರ್ಣಾಂಕ dhl)
{
  VTerm_ಸಾಲು_Info info = state->lineinfo[ಅಡ್ಡಸಾಲು];

  ಆದರೆ(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  ಆದರೆ(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dhl == DHL_ತುದಿ)
    info.doubleheight = DHL_ತುದಿ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  ಆದರೆ((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(ಅಡ್ಡಸಾಲು, &info, state->lineinfo + ಅಡ್ಡಸಾಲು, state->cbdata))
      || force)
    state->lineinfo[ಅಡ್ಡಸಾಲು] = info;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_ಪಠ್ಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ ಬೈಟುಗಳು[], size_t len, ಶೂನ್ಯ *user)
{
  VTermState *state = user;
  ಪೂರ್ಣಾಂಕ npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  ಪೂರ್ಣಾಂಕ i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / ಗಾತ್ರ(uint32_t);

  encoding =
    state->gsingle_ಗಣ     ? &state->encoding[state->gsingle_ಗಣ] :
    !(ಬೈಟುಗಳು[eaten] & 0x80) ? &state->encoding[state->gl_ಗಣ] :
    state->vt->ಬಹುಲಕ.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_ಗಣ];
  ಆದರೆ (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->ದತ್ತಾಂಶ,
      codepoints, &npoints, state->gsingle_ಗಣ ? 1 : (ಪೂರ್ಣಾಂಕ)maxpoints,
      ಬೈಟುಗಳು, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  ಆದರೆ(!npoints)
  {
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)eaten;
  }

  ಆದರೆ(state->gsingle_ಗಣ && npoints)
    state->gsingle_ಗಣ = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  ಆದರೆ(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು == state->combine_pos.ಅಡ್ಡಸಾಲು && state->pos.ಲಂಬಸಾಲು == state->combine_pos.ಲಂಬಸಾಲು + state->combine_ಅಗಲ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_GLYPH_COMBINE
      ಪೂರ್ಣಾಂಕ printpos;
      ಬರೆ("DEBUG: COMBINING SPLIT GLYPH of chars {");
      ಗಾಗಿ(printpos = 0; state->combine_chars[printpos]; printpos++)
        ಬರೆ("U+%04x ", state->combine_chars[printpos]);
      ಬರೆ("} + {");
#ಆದರೆ_ಅಂತ್ಯ

      /* Find where we need to append these combining chars */
      ಪೂರ್ಣಾಂಕ saved_i = 0;
      ಆಗ(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      ಆಗ(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        ಆದರೆ(saved_i >= (ಪೂರ್ಣಾಂಕ)state->combine_chars_ಗಾತ್ರ)
          grow_combine_ಬಫರ್(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      ಆದರೆ(saved_i >= (ಪೂರ್ಣಾಂಕ)state->combine_chars_ಗಾತ್ರ)
        grow_combine_ಬಫರ್(state);
      state->combine_chars[saved_i] = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_GLYPH_COMBINE
      ಗಾಗಿ(; state->combine_chars[printpos]; printpos++)
        ಬರೆ("U+%04x ", state->combine_chars[printpos]);
      ಬರೆ("}\n");
#ಆದರೆ_ಅಂತ್ಯ

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_ಅಗಲ, state->combine_pos);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  ಗಾಗಿ(; i < npoints; i++) {
    // Try to find combining characters following this
    ಪೂರ್ಣಾಂಕ glyph_starts = i;
    ಪೂರ್ಣಾಂಕ glyph_ತುದಿಗಳು;
    ಪೂರ್ಣಾಂಕ ಅಗಲ = 0;

    ಗಾಗಿ(glyph_ತುದಿಗಳು = i + 1;
        (glyph_ತುದಿಗಳು < npoints) && (glyph_ತುದಿಗಳು < glyph_starts + VTERM_MAX_CHARS_ಪ್ರತಿ_CELL);
        glyph_ತುದಿಗಳು++)
      ಆದರೆ(!vterm_unicode_is_combining(codepoints[glyph_ತುದಿಗಳು]))
        ನಿಲ್ಲಿಸು;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_ಪ್ರತಿ_CELL + 1) * ಗಾತ್ರ(uint32_t));
    ಆದರೆ (chars == ಖಾಲಿ)
      ನಿಲ್ಲಿಸು;

    ಗಾಗಿ( ; i < glyph_ತುದಿಗಳು; i++) {
      ಪೂರ್ಣಾಂಕ this_ಅಗಲ;
      ಆದರೆ(vterm_get_special_pty_type() == 2) {
        state->vt->ಒಳ_backspace -= (state->vt->ಒಳ_backspace > 0) ? 1 : 0;
        ಆದರೆ(state->vt->ಒಳ_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_ಅಗಲ = vterm_unicode_ಅಗಲ(codepoints[i]);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
      ಆದರೆ(this_ಅಗಲ < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#ಆದರೆ_ಅಂತ್ಯ
      ಆದರೆ (i == glyph_starts || this_ಅಗಲ > ಅಗಲ)
	ಅಗಲ = this_ಅಗಲ;  // TODO: should be += ?
    }

    ಆದರೆ (ಅಗಲ < 0)
      ಅಗಲ = 0;

    ಆಗ(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ತುದಿಗಳು - glyph_starts] = 0;
    i--;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG_GLYPH_COMBINE
    ಪೂರ್ಣಾಂಕ printpos;
    ಬರೆ("DEBUG: COMBINED GLYPH of %d chars {", glyph_ತುದಿಗಳು - glyph_starts);
    ಗಾಗಿ(printpos = 0; printpos < glyph_ತುದಿಗಳು - glyph_starts; printpos++)
      ಬರೆ("U+%04x ", chars[printpos]);
    ಬರೆ("}, onscreen width %d\n", ಅಗಲ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ(state->at_phantom || state->pos.ಲಂಬಸಾಲು + ಅಗಲ > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.ಲಂಬಸಾಲು = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.ಅಡ್ಡಸಾಲು].continuation = 1;
    }

    ಆದರೆ(state->ಬಹುಲಕ.ಸೇರಿಸು) {
      // TODO: This will be a little inefficient for large bodies of text, as
      // it'll have to 'ICH' effectively before every glyph. We should scan
      // ahead and ICH as many times as required
      VTermRect rect;
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
      rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = state->pos.ಅಡ್ಡಸಾಲು + 1;
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು;
      rect.ಕೊನೆ_ಲಂಬಸಾಲು   = THISROWWIDTH(state);
      scroll(state, rect, 0, -1);
    }

    putglyph(state, chars, ಅಗಲ, state->pos);

    ಆದರೆ(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      ಪೂರ್ಣಾಂಕ save_i;
      ಗಾಗಿ(save_i = 0; chars[save_i]; save_i++) {
        ಆದರೆ(save_i >= (ಪೂರ್ಣಾಂಕ)state->combine_chars_ಗಾತ್ರ)
          grow_combine_ಬಫರ್(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      ಆದರೆ(save_i >= (ಪೂರ್ಣಾಂಕ)state->combine_chars_ಗಾತ್ರ)
        grow_combine_ಬಫರ್(state);
      state->combine_chars[save_i] = 0;
      state->combine_ಅಗಲ = ಅಗಲ;
      state->combine_pos = state->pos;
    }

    ಆದರೆ(state->pos.ಲಂಬಸಾಲು + ಅಗಲ >= THISROWWIDTH(state)) {
      ಆದರೆ(state->ಬಹುಲಕ.autowrap)
        state->at_phantom = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      state->pos.ಲಂಬಸಾಲು += ಅಗಲ;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
  ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು < 0 || state->pos.ಅಡ್ಡಸಾಲು >= state->ಅಡ್ಡಸಾಲುಗಳು_2 ||
     state->pos.ಲಂಬಸಾಲು < 0 || state->pos.ಲಂಬಸಾಲು >= state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.ಅಡ್ಡಸಾಲು, state->pos.ಲಂಬಸಾಲು);
    abort();
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)eaten;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_control(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ control, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.ಲಂಬಸಾಲು -= (leadpos.ಲಂಬಸಾಲು >= 2 ? 2 : 0);

  ಆಯ್ಕೆ(control) {
  ಸ್ಥಿತಿ 0x07: // BEL - ECMA-48 8.3.3
    ಆದರೆ(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x08: // BS - ECMA-48 8.3.5
    ಆದರೆ(state->pos.ಲಂಬಸಾಲು > 0)
      state->pos.ಲಂಬಸಾಲು--;
    ಆದರೆ(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      ಆದರೆ(vterm_unicode_ಅಗಲ(cell.chars[0]) == 2)
        state->pos.ಲಂಬಸಾಲು--;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x0a: // LF - ECMA-48 8.3.74
  ಸ್ಥಿತಿ 0x0b: // VT
  ಸ್ಥಿತಿ 0x0c: // FF
    linefeed(state);
    ಆದರೆ(state->ಬಹುಲಕ.newline)
      state->pos.ಲಂಬಸಾಲು = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x0d: // CR - ECMA-48 8.3.15
    state->pos.ಲಂಬಸಾಲು = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_ಗಣ = 1;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_ಗಣ = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.ಲಂಬಸಾಲು = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x88: // HTS - ECMA-48 8.3.62
    ಗಣ_ಲಂಬಸಾಲು_tabstop(state, state->pos.ಲಂಬಸಾಲು);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x8d: // RI - ECMA-48 8.3.104
    ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು == state->scrollregion_ತುದಿ) {
      VTermRect rect;
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->scrollregion_ತುದಿ;
      rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = SCROLLREGION_ಎಡ(state);
      rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

      scroll(state, rect, -1, 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು > 0)
        state->pos.ಅಡ್ಡಸಾಲು--;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_ಗಣ = 2;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_ಗಣ = 3;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಆದರೆ(state->fallbacks && state->fallbacks->control)
      ಆದರೆ((*state->fallbacks->control)(control, state->fbdata))
        ಹಿಂತಿರುಗಿ 1;

    ಹಿಂತಿರುಗಿ 0;
  }

  updatecursor(state, &oldpos, 1);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
  ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು < 0 || state->pos.ಅಡ್ಡಸಾಲು >= state->ಅಡ್ಡಸಾಲುಗಳು_2 ||
     state->pos.ಲಂಬಸಾಲು < 0 || state->pos.ಲಂಬಸಾಲು >= state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.ಅಡ್ಡಸಾಲು, state->pos.ಲಂಬಸಾಲು);
    abort();
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ settermprop_bool(VTermState *state, VTermProp prop, ಪೂರ್ಣಾಂಕ v)
{
  VTerm_ಮೌಲ್ಯ val;
  val.boolean = v;
  ಹಿಂತಿರುಗಿ vterm_state_ಗಣ_termprop(state, prop, &val);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ settermprop_int(VTermState *state, VTermProp prop, ಪೂರ್ಣಾಂಕ v)
{
  VTerm_ಮೌಲ್ಯ val;
  val.ಸಂಖ್ಯೆ = v;
  ಹಿಂತಿರುಗಿ vterm_state_ಗಣ_termprop(state, prop, &val);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_ಮೌಲ್ಯ val;

  val.string = frag;
  ಹಿಂತಿರುಗಿ vterm_state_ಗಣ_termprop(state, prop, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ savecursor(VTermState *state, ಪೂರ್ಣಾಂಕ save)
{
  ಆದರೆ(save) {
    state->saved.pos = state->pos;
    state->saved.ಬಹುಲಕ.cursor_visible = state->ಬಹುಲಕ.cursor_visible;
    state->saved.ಬಹುಲಕ.cursor_blink   = state->ಬಹುಲಕ.cursor_blink;
    state->saved.ಬಹುಲಕ.cursor_shape   = state->ಬಹುಲಕ.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.ಬಹುಲಕ.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.ಬಹುಲಕ.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.ಬಹುಲಕ.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೈಟುಗಳು, size_t len, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  ಆಯ್ಕೆ(ಬೈಟುಗಳು[0]) {
  ಸ್ಥಿತಿ ' ':
    ಆದರೆ(len != 2)
      ಹಿಂತಿರುಗಿ 0;

    ಆಯ್ಕೆ(ಬೈಟುಗಳು[1]) {
      ಸ್ಥಿತಿ 'F': // S7C1T
        state->vt->ಬಹುಲಕ.ctrl8bit = 0;
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ 'G': // S8C1T
        state->vt->ಬಹುಲಕ.ctrl8bit = 1;
        ನಿಲ್ಲಿಸು;

      ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ 2;

  ಸ್ಥಿತಿ '#':
    ಆದರೆ(len != 2)
      ಹಿಂತಿರುಗಿ 0;

    ಆಯ್ಕೆ(ಬೈಟುಗಳು[1]) {
      ಸ್ಥಿತಿ '3': // DECDHL top
        ಆದರೆ(state->ಬಹುಲಕ.leftrightmargin)
          ನಿಲ್ಲಿಸು;
        ಗಣ_lineinfo(state, state->pos.ಅಡ್ಡಸಾಲು, NO_FORCE, DWL_ON, DHL_ತುದಿ);
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ '4': // DECDHL bottom
        ಆದರೆ(state->ಬಹುಲಕ.leftrightmargin)
          ನಿಲ್ಲಿಸು;
        ಗಣ_lineinfo(state, state->pos.ಅಡ್ಡಸಾಲು, NO_FORCE, DWL_ON, DHL_BOTTOM);
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ '5': // DECSWL
        ಆದರೆ(state->ಬಹುಲಕ.leftrightmargin)
          ನಿಲ್ಲಿಸು;
        ಗಣ_lineinfo(state, state->pos.ಅಡ್ಡಸಾಲು, NO_FORCE, DWL_OFF, DHL_OFF);
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ '6': // DECDWL
        ಆದರೆ(state->ಬಹುಲಕ.leftrightmargin)
          ನಿಲ್ಲಿಸು;
        ಗಣ_lineinfo(state, state->pos.ಅಡ್ಡಸಾಲು, NO_FORCE, DWL_ON, DHL_OFF);
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        ಗಾಗಿ(pos.ಅಡ್ಡಸಾಲು = 0; pos.ಅಡ್ಡಸಾಲು < state->ಅಡ್ಡಸಾಲುಗಳು_2; pos.ಅಡ್ಡಸಾಲು++)
          ಗಾಗಿ(pos.ಲಂಬಸಾಲು = 0; pos.ಲಂಬಸಾಲು < ROWWIDTH(state, pos.ಅಡ್ಡಸಾಲು); pos.ಲಂಬಸಾಲು++)
            putglyph(state, E, 1, pos);
        ನಿಲ್ಲಿಸು;
      }

      ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ 2;

  ಸ್ಥಿತಿ '(': ಸ್ಥಿತಿ ')': ಸ್ಥಿತಿ '*': ಸ್ಥಿತಿ '+': // SCS
    ಆದರೆ(len != 2)
      ಹಿಂತಿರುಗಿ 0;

    {
      ಪೂರ್ಣಾಂಕ setnum = ಬೈಟುಗಳು[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, ಬೈಟುಗಳು[1]);

      ಆದರೆ(newenc) {
        state->encoding[setnum].enc = newenc;

        ಆದರೆ(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].ದತ್ತಾಂಶ);
      }
    }

    ಹಿಂತಿರುಗಿ 2;

  ಸ್ಥಿತಿ '7': // DECSC
    savecursor(state, 1);
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ '8': // DECRC
    savecursor(state, 0);
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ '=': // DECKPAM
    state->ಬಹುಲಕ.keypad = 1;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ '>': // DECKPNM
    state->ಬಹುಲಕ.keypad = 0;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    ಆದರೆ(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->ಬಹುಲಕ.cursor_visible, state->cbdata);
    ಹಿಂತಿರುಗಿ 1;
  }

  ಸ್ಥಿತಿ 'n': // LS2 - ECMA-48 8.3.78
    state->gl_ಗಣ = 2;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ 'o': // LS3 - ECMA-48 8.3.80
    state->gl_ಗಣ = 3;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ '~': // LS1R - ECMA-48 8.3.77
    state->gr_ಗಣ = 1;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ '}': // LS2R - ECMA-48 8.3.79
    state->gr_ಗಣ = 2;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ '|': // LS3R - ECMA-48 8.3.81
    state->gr_ಗಣ = 3;
    ಹಿಂತಿರುಗಿ 1;

  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ 0;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ಬಹುಲಕ(VTermState *state, ಪೂರ್ಣಾಂಕ ಅಂಕೆ, ಪೂರ್ಣಾಂಕ val)
{
  ಆಯ್ಕೆ(ಅಂಕೆ) {
  ಸ್ಥಿತಿ 4: // IRM - ECMA-48 7.2.10
    state->ಬಹುಲಕ.ಸೇರಿಸು = val;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 20: // LNM - ANSI X3.4-1977
    state->ಬಹುಲಕ.newline = val;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", ಅಂಕೆ);
    ಹಿಂತಿರುಗಿ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_dec_ಬಹುಲಕ(VTermState *state, ಪೂರ್ಣಾಂಕ ಅಂಕೆ, ಪೂರ್ಣಾಂಕ val)
{
  ಆಯ್ಕೆ(ಅಂಕೆ) {
  ಸ್ಥಿತಿ 1:
    state->ಬಹುಲಕ.cursor = val;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_ಹಿಮ್ಮುಖ, val);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->ಬಹುಲಕ.origin = val;
      state->pos.ಅಡ್ಡಸಾಲು = state->ಬಹುಲಕ.origin ? state->scrollregion_ತುದಿ : 0;
      state->pos.ಲಂಬಸಾಲು = state->ಬಹುಲಕ.origin ? SCROLLREGION_ಎಡ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 7:
    state->ಬಹುಲಕ.autowrap = val;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->ಬಹುಲಕ.leftrightmargin = val;
    ಆದರೆ(val) {
      ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      ಗಾಗಿ(ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < state->ಅಡ್ಡಸಾಲುಗಳು_2; ಅಡ್ಡಸಾಲು++)
        ಗಣ_lineinfo(state, ಅಡ್ಡಸಾಲು, FORCE, DWL_OFF, DHL_OFF);
    }

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1000:
  ಸ್ಥಿತಿ 1002:
  ಸ್ಥಿತಿ 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (ಅಂಕೆ == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (ಅಂಕೆ == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->ಬಹುಲಕ.report_focus = val;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1048:
    savecursor(state, val);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 2004:
    state->ಬಹುಲಕ.bracketpaste = val;
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", ಅಂಕೆ);
    ಹಿಂತಿರುಗಿ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ request_dec_ಬಹುಲಕ(VTermState *state, ಪೂರ್ಣಾಂಕ ಅಂಕೆ)
{
  ಪೂರ್ಣಾಂಕ ಉತ್ತರ;

  ಆಯ್ಕೆ(ಅಂಕೆ) {
    ಸ್ಥಿತಿ 1:
      ಉತ್ತರ = state->ಬಹುಲಕ.cursor;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 5:
      ಉತ್ತರ = state->ಬಹುಲಕ.screen;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 6:
      ಉತ್ತರ = state->ಬಹುಲಕ.origin;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 7:
      ಉತ್ತರ = state->ಬಹುಲಕ.autowrap;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 12:
      ಉತ್ತರ = state->ಬಹುಲಕ.cursor_blink;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 25:
      ಉತ್ತರ = state->ಬಹುಲಕ.cursor_visible;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 69:
      ಉತ್ತರ = state->ಬಹುಲಕ.leftrightmargin;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1000:
      ಉತ್ತರ = state->mouse_flags == MOUSE_WANT_CLICK;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1002:
      ಉತ್ತರ = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1003:
      ಉತ್ತರ = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1004:
      ಉತ್ತರ = state->ಬಹುಲಕ.report_focus;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1005:
      ಉತ್ತರ = state->mouse_protocol == MOUSE_UTF8;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1006:
      ಉತ್ತರ = state->mouse_protocol == MOUSE_SGR;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1015:
      ಉತ್ತರ = state->mouse_protocol == MOUSE_RXVT;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1047:
      ಉತ್ತರ = state->ಬಹುಲಕ.alt_screen;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 2004:
      ಉತ್ತರ = state->ಬಹುಲಕ.bracketpaste;
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ಅಂಕೆ, 0);
      ಹಿಂತಿರುಗಿ;
  }

  vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ಅಂಕೆ, ಉತ್ತರ ? 1 : 2);
}

ಸ್ಥಿರ ಶೂನ್ಯ request_ಆವೃತ್ತಿ_string(VTermState *state)
{
  vterm_ತಳ್ಳು_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_ಆವೃತ್ತಿ_MAJOR, VTERM_ಆವೃತ್ತಿ_MINOR);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_csi(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *leader, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ವಾದಗಳು[], ಪೂರ್ಣಾಂಕ argcount, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *intermed, ಅಕ್ಷರ command, ಶೂನ್ಯ *user)
{
  VTermState *state = user;
  ಪೂರ್ಣಾಂಕ leader_byte = 0;
  ಪೂರ್ಣಾಂಕ intermed_byte = 0;
  ಪೂರ್ಣಾಂಕ cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  ಪೂರ್ಣಾಂಕ handled = 1;

  // Some temporaries for later code
  ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, val;
  ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
  VTermRect rect;
  ಪೂರ್ಣಾಂಕ selective;

  ಆದರೆ(leader && leader[0]) {
    ಆದರೆ(leader[1]) // longer than 1 char
      ಹಿಂತಿರುಗಿ 0;

    ಆಯ್ಕೆ(leader[0]) {
    ಸ್ಥಿತಿ '?':
    ಸ್ಥಿತಿ '>':
    ಸ್ಥಿತಿ '<':
      leader_byte = leader[0];
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ 0;
    }
  }

  ಆದರೆ(intermed && intermed[0]) {
    ಆದರೆ(intermed[1]) // longer than 1 char
      ಹಿಂತಿರುಗಿ 0;

    ಆಯ್ಕೆ(intermed[0]) {
    ಸ್ಥಿತಿ ' ':
    ಸ್ಥಿತಿ '!':
    ಸ್ಥಿತಿ '"':
    ಸ್ಥಿತಿ '$':
    ಸ್ಥಿತಿ '\'':
      intermed_byte = intermed[0];
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ 0;
    }
  }

  oldpos = state->pos;

#ವ್ಯಾಖ್ಯಾನಿಸು LBOUND(v,min) ಆದರೆ((v) < (min)) (v) = (min)
#ವ್ಯಾಖ್ಯಾನಿಸು UBOUND(v,max) ಆದರೆ((v) > (max)) (v) = (max)

#ವ್ಯಾಖ್ಯಾನಿಸು LEADER(l,b) ((l << 8) | b)
#ವ್ಯಾಖ್ಯಾನಿಸು INTERMED(i,b) ((i << 16) | b)

  ಆಯ್ಕೆ(intermed_byte << 16 | leader_byte << 8 | command) {
  ಸ್ಥಿತಿ 0x40: // ICH - ECMA-48 8.3.64
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    ಆದರೆ(!is_cursor_ಒಳ_scrollregion(state))
      ನಿಲ್ಲಿಸು;

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = state->pos.ಅಡ್ಡಸಾಲು + 1;
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು;
    ಆದರೆ(state->ಬಹುಲಕ.leftrightmargin)
      rect.ಕೊನೆ_ಲಂಬಸಾಲು = SCROLLREGION_ಬಲ(state);
    ಇಲ್ಲದಿದ್ದರೆ
      rect.ಕೊನೆ_ಲಂಬಸಾಲು = THISROWWIDTH(state);

    scroll(state, rect, 0, -ಎಣಿಕೆ_2);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x41: // CUU - ECMA-48 8.3.22
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಅಡ್ಡಸಾಲು -= ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x42: // CUD - ECMA-48 8.3.19
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಅಡ್ಡಸಾಲು += ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x43: // CUF - ECMA-48 8.3.20
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಲಂಬಸಾಲು += ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x44: // CUB - ECMA-48 8.3.18
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಲಂಬಸಾಲು -= ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x45: // CNL - ECMA-48 8.3.12
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಲಂಬಸಾಲು = 0;
    state->pos.ಅಡ್ಡಸಾಲು += ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x46: // CPL - ECMA-48 8.3.13
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಲಂಬಸಾಲು = 0;
    state->pos.ಅಡ್ಡಸಾಲು -= ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(ವಾದಗಳು[0], 1);
    state->pos.ಲಂಬಸಾಲು = val-1;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x48: // CUP - ECMA-48 8.3.21
    ಅಡ್ಡಸಾಲು = CSI_ARG_OR(ವಾದಗಳು[0], 1);
    ಲಂಬಸಾಲು = argcount < 2 || CSI_ARG_IS_MISSING(ವಾದಗಳು[1]) ? 1 : CSI_ARG(ವಾದಗಳು[1]);
    // zero-based
    ಆದರೆ(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು == ಅಡ್ಡಸಾಲು - 1) {
        ಪೂರ್ಣಾಂಕ cnt, ptr = 0;
        ಗಾಗಿ(cnt = 0; cnt < ಲಂಬಸಾಲು - 1; ++cnt) {
	  VTermPos p;
	  VTermScreenCell c0, c1;
	  p.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು - 1;
	  p.ಲಂಬಸಾಲು = ptr;
	  vterm_screen_get_cell(state->vt->screen, p, &c0);
	  p.ಲಂಬಸಾಲು++;
	  vterm_screen_get_cell(state->vt->screen, p, &c1);
	  ptr += (c1.chars[0] == (uint32_t)-1)		    // double cell?
	     ? (vterm_unicode_is_ambiguous(c0.chars[0]))    // is ambiguous?
	     ? vterm_unicode_ಅಗಲ(0x00a1) : 1		    // &ambiwidth
	     : 1;					    // not ambiguous
        }
        ಲಂಬಸಾಲು = ptr + 1;
      }
    }
    state->pos.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು-1;
    state->pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು-1;
    ಆದರೆ(state->ಬಹುಲಕ.origin) {
      state->pos.ಅಡ್ಡಸಾಲು += state->scrollregion_ತುದಿ;
      state->pos.ಲಂಬಸಾಲು += SCROLLREGION_ಎಡ(state);
    }
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x49: // CHT - ECMA-48 8.3.10
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    tab(state, ಎಣಿಕೆ_2, +1);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x4a: // ED - ECMA-48 8.3.39
  ಸ್ಥಿತಿ LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    ಆಯ್ಕೆ(CSI_ARG(ವಾದಗಳು[0])) {
    ಸ್ಥಿತಿ CSI_ARG_MISSING:
    ಸ್ಥಿತಿ 0:
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು; rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು + 1;
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು; rect.ಕೊನೆ_ಲಂಬಸಾಲು = state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
      ಆದರೆ(rect.ಕೊನೆ_ಲಂಬಸಾಲು > rect.ಆರಂಭ_ಲಂಬಸಾಲು)
        erase(state, rect, selective);

      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು + 1; rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = state->ಅಡ್ಡಸಾಲುಗಳು_2;
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0;
      ಗಾಗಿ(ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++)
	ಗಣ_lineinfo(state, ಅಡ್ಡಸಾಲು, FORCE, DWL_OFF, DHL_OFF);
      ಆದರೆ(rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು > rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು)
        erase(state, rect, selective);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 1:
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = 0; rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0; rect.ಕೊನೆ_ಲಂಬಸಾಲು = state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
      ಗಾಗಿ(ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++)
	ಗಣ_lineinfo(state, ಅಡ್ಡಸಾಲು, FORCE, DWL_OFF, DHL_OFF);
      ಆದರೆ(rect.ಕೊನೆ_ಲಂಬಸಾಲು > rect.ಆರಂಭ_ಲಂಬಸಾಲು)
        erase(state, rect, selective);

      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು; rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು + 1;
                          rect.ಕೊನೆ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು + 1;
      ಆದರೆ(rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು > rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು)
        erase(state, rect, selective);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 2:
      rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = 0; rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = state->ಅಡ್ಡಸಾಲುಗಳು_2;
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0; rect.ಕೊನೆ_ಲಂಬಸಾಲು = state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
      ಗಾಗಿ(ಅಡ್ಡಸಾಲು = rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು < rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು; ಅಡ್ಡಸಾಲು++)
	ಗಣ_lineinfo(state, ಅಡ್ಡಸಾಲು, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ 3:
      ಆದರೆ(state->callbacks && state->callbacks->sb_clear)
        ಆದರೆ((*state->callbacks->sb_clear)(state->cbdata))
          ಹಿಂತಿರುಗಿ 1;
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x4b: // EL - ECMA-48 8.3.41
  ಸ್ಥಿತಿ LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = state->pos.ಅಡ್ಡಸಾಲು + 1;

    ಆಯ್ಕೆ(CSI_ARG(ವಾದಗಳು[0])) {
    ಸ್ಥಿತಿ CSI_ARG_MISSING:
    ಸ್ಥಿತಿ 0:
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು; rect.ಕೊನೆ_ಲಂಬಸಾಲು = THISROWWIDTH(state); ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0; rect.ಕೊನೆ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು + 1; ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
      rect.ಆರಂಭ_ಲಂಬಸಾಲು = 0; rect.ಕೊನೆ_ಲಂಬಸಾಲು = THISROWWIDTH(state); ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ 0;
    }

    ಆದರೆ(rect.ಕೊನೆ_ಲಂಬಸಾಲು > rect.ಆರಂಭ_ಲಂಬಸಾಲು)
      erase(state, rect, selective);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x4c: // IL - ECMA-48 8.3.67
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    ಆದರೆ(!is_cursor_ಒಳ_scrollregion(state))
      ನಿಲ್ಲಿಸು;

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = SCROLLREGION_ಎಡ(state);
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, -ಎಣಿಕೆ_2, 0);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x4d: // DL - ECMA-48 8.3.32
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    ಆದರೆ(!is_cursor_ಒಳ_scrollregion(state))
      ನಿಲ್ಲಿಸು;

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = SCROLLREGION_ಎಡ(state);
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, ಎಣಿಕೆ_2, 0);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x50: // DCH - ECMA-48 8.3.26
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    ಆದರೆ(!is_cursor_ಒಳ_scrollregion(state))
      ನಿಲ್ಲಿಸು;

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = state->pos.ಅಡ್ಡಸಾಲು + 1;
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು;
    ಆದರೆ(state->ಬಹುಲಕ.leftrightmargin)
      rect.ಕೊನೆ_ಲಂಬಸಾಲು = SCROLLREGION_ಬಲ(state);
    ಇಲ್ಲದಿದ್ದರೆ
      rect.ಕೊನೆ_ಲಂಬಸಾಲು = THISROWWIDTH(state);

    scroll(state, rect, 0, ಎಣಿಕೆ_2);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x53: // SU - ECMA-48 8.3.147
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->scrollregion_ತುದಿ;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = SCROLLREGION_ಎಡ(state);
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, ಎಣಿಕೆ_2, 0);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x54: // SD - ECMA-48 8.3.113
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->scrollregion_ತುದಿ;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = SCROLLREGION_ಎಡ(state);
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, -ಎಣಿಕೆ_2, 0);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x58: // ECH - ECMA-48 8.3.38
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->pos.ಅಡ್ಡಸಾಲು;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = state->pos.ಅಡ್ಡಸಾಲು + 1;
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು;
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = state->pos.ಲಂಬಸಾಲು + ಎಣಿಕೆ_2;
    UBOUND(rect.ಕೊನೆ_ಲಂಬಸಾಲು, THISROWWIDTH(state));

    erase(state, rect, 0);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x5a: // CBT - ECMA-48 8.3.7
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    tab(state, ಎಣಿಕೆ_2, -1);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x60: // HPA - ECMA-48 8.3.57
    ಲಂಬಸಾಲು = CSI_ARG_OR(ವಾದಗಳು[0], 1);
    state->pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು-1;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x61: // HPR - ECMA-48 8.3.59
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಲಂಬಸಾಲು += ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x62: { // REP - ECMA-48 8.3.103
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು_ಅಗಲ = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    ಆದರೆ(state->combine_ಅಗಲ < 1)
      ನಿಲ್ಲಿಸು;

    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು + ಎಣಿಕೆ_2;
    UBOUND(ಲಂಬಸಾಲು, ಅಡ್ಡಸಾಲು_ಅಗಲ);
    ಆಗ (state->pos.ಲಂಬಸಾಲು < ಲಂಬಸಾಲು) {
      putglyph(state, state->combine_chars, state->combine_ಅಗಲ, state->pos);
      state->pos.ಲಂಬಸಾಲು += state->combine_ಅಗಲ;
    }
    ಆದರೆ (state->pos.ಲಂಬಸಾಲು + state->combine_ಅಗಲ >= ಅಡ್ಡಸಾಲು_ಅಗಲ) {
      ಆದರೆ (state->ಬಹುಲಕ.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    ನಿಲ್ಲಿಸು;
  }

  ಸ್ಥಿತಿ 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(ವಾದಗಳು[0], 0);
    ಆದರೆ(val == 0)
      // DEC VT100 response
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x64: // VPA - ECMA-48 8.3.158
    ಅಡ್ಡಸಾಲು = CSI_ARG_OR(ವಾದಗಳು[0], 1);
    state->pos.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು-1;
    ಆದರೆ(state->ಬಹುಲಕ.origin)
      state->pos.ಅಡ್ಡಸಾಲು += state->scrollregion_ತುದಿ;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x65: // VPR - ECMA-48 8.3.160
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಅಡ್ಡಸಾಲು += ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x66: // HVP - ECMA-48 8.3.63
    ಅಡ್ಡಸಾಲು = CSI_ARG_OR(ವಾದಗಳು[0], 1);
    ಲಂಬಸಾಲು = argcount < 2 || CSI_ARG_IS_MISSING(ವಾದಗಳು[1]) ? 1 : CSI_ARG(ವಾದಗಳು[1]);
    // zero-based
    state->pos.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು-1;
    state->pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು-1;
    ಆದರೆ(state->ಬಹುಲಕ.origin) {
      state->pos.ಅಡ್ಡಸಾಲು += state->scrollregion_ತುದಿ;
      state->pos.ಲಂಬಸಾಲು += SCROLLREGION_ಎಡ(state);
    }
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(ವಾದಗಳು[0], 0);

    ಆಯ್ಕೆ(val) {
    ಸ್ಥಿತಿ 0:
      clear_ಲಂಬಸಾಲು_tabstop(state, state->pos.ಲಂಬಸಾಲು);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 3:
    ಸ್ಥಿತಿ 5:
      ಗಾಗಿ(ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++)
        clear_ಲಂಬಸಾಲು_tabstop(state, ಲಂಬಸಾಲು);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
    ಸ್ಥಿತಿ 2:
    ಸ್ಥಿತಿ 4:
      ನಿಲ್ಲಿಸು;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ 0;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x68: // SM - ECMA-48 8.3.125
    ಆದರೆ(!CSI_ARG_IS_MISSING(ವಾದಗಳು[0]))
      ಗಣ_ಬಹುಲಕ(state, CSI_ARG(ವಾದಗಳು[0]), 1);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('?', 0x68): // DEC private mode set
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < argcount; i++) {
      ಆದರೆ(!CSI_ARG_IS_MISSING(ವಾದಗಳು[i]))
        ಗಣ_dec_ಬಹುಲಕ(state, CSI_ARG(ವಾದಗಳು[i]), 1);
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x6a: // HPB - ECMA-48 8.3.58
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಲಂಬಸಾಲು -= ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x6b: // VPB - ECMA-48 8.3.159
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);
    state->pos.ಅಡ್ಡಸಾಲು -= ಎಣಿಕೆ_2;
    state->at_phantom = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x6c: // RM - ECMA-48 8.3.106
    ಆದರೆ(!CSI_ARG_IS_MISSING(ವಾದಗಳು[0]))
      ಗಣ_ಬಹುಲಕ(state, CSI_ARG(ವಾದಗಳು[0]), 0);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('?', 0x6c): // DEC private mode reset
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < argcount; i++) {
      ಆದರೆ(!CSI_ARG_IS_MISSING(ವಾದಗಳು[i]))
        ಗಣ_dec_ಬಹುಲಕ(state, CSI_ARG(ವಾದಗಳು[i]), 0);
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, ವಾದಗಳು, argcount);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    ಆದರೆ (argcount == 1 && CSI_ARG(ವಾದಗಳು[0]) == 4)
    {
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->ಬಹುಲಕ.modify_other_ಕೀಲಿಗಳು ? 2 : 0);
      ನಿಲ್ಲಿಸು;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    ಗಾಗಿ(ಪೂರ್ಣಾಂಕ argi = 0; argi < argcount; argi++) {
      ದೀರ್ಘ arg;
      ಆಯ್ಕೆ(arg = CSI_ARG(ವಾದಗಳು[argi])) {
        ಸ್ಥಿತಿ 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          ನಿಲ್ಲಿಸು;
      }
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    ಆದರೆ (argcount == 2 && CSI_ARG(ವಾದಗಳು[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->ಬಹುಲಕ.kitty_keyboard = 0;

      state->ಬಹುಲಕ.modify_other_ಕೀಲಿಗಳು = CSI_ARG(ವಾದಗಳು[1]) == 2;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    ಆದರೆ (argcount == 1 && CSI_ARG(ವಾದಗಳು[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->ಬಹುಲಕ.modify_other_ಕೀಲಿಗಳು = 0;

      state->ಬಹುಲಕ.kitty_keyboard = 1;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    ಆದರೆ (argcount <= 1)
      state->ಬಹುಲಕ.kitty_keyboard = 0;
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    ಆದರೆ (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->ಬಹುಲಕ.kitty_keyboard);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x6e: // DSR - ECMA-48 8.3.35
  ಸ್ಥಿತಿ LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(ವಾದಗಳು[0], 0);

    {
      ಅಕ್ಷರ *qmark = (leader_byte == '?') ? "?" : "";

      ಆಯ್ಕೆ(val) {
      ಸ್ಥಿತಿ 0: ಸ್ಥಿತಿ 1: ಸ್ಥಿತಿ 2: ಸ್ಥಿತಿ 3: ಸ್ಥಿತಿ 4:
        // ignore - these are replies
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 5:
        vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 6: // CPR - cursor position report
        vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.ಅಡ್ಡಸಾಲು + 1, state->pos.ಲಂಬಸಾಲು + 1);
        ನಿಲ್ಲಿಸು;
      }
    }
    ನಿಲ್ಲಿಸು;


  ಸ್ಥಿತಿ INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('?', INTERMED('$', 0x70)):
    request_dec_ಬಹುಲಕ(state, CSI_ARG(ವಾದಗಳು[0]));
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_ಆವೃತ್ತಿ_string(state);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(ವಾದಗಳು[0], 1);

    ಆಯ್ಕೆ(val) {
    ಸ್ಥಿತಿ 0: ಸ್ಥಿತಿ 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_ಖಂಡ);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_ಖಂಡ);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ಎಡ);
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ಎಡ);
      ನಿಲ್ಲಿಸು;
    }

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(ವಾದಗಳು[0], 0);

    ಆಯ್ಕೆ(val) {
    ಸ್ಥಿತಿ 0: ಸ್ಥಿತಿ 2:
      state->protected_cell = 0;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
      state->protected_cell = 1;
      ನಿಲ್ಲಿಸು;
    }

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x72: // DECSTBM - DEC custom
    state->scrollregion_ತುದಿ = CSI_ARG_OR(ವಾದಗಳು[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(ವಾದಗಳು[1]) ? -1 : CSI_ARG(ವಾದಗಳು[1]);
    LBOUND(state->scrollregion_ತುದಿ, 0);
    UBOUND(state->scrollregion_ತುದಿ, state->ಅಡ್ಡಸಾಲುಗಳು_2);
    LBOUND(state->scrollregion_bottom, -1);
    ಆದರೆ(state->scrollregion_ತುದಿ == 0 && state->scrollregion_bottom == state->ಅಡ್ಡಸಾಲುಗಳು_2)
      state->scrollregion_bottom = -1;
    ಇಲ್ಲದಿದ್ದರೆ
      UBOUND(state->scrollregion_bottom, state->ಅಡ್ಡಸಾಲುಗಳು_2);

    ಆದರೆ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ತುದಿ) {
      // Invalid
      state->scrollregion_ತುದಿ    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.ಅಡ್ಡಸಾಲು = 0;
    state->pos.ಲಂಬಸಾಲು = 0;
    ಆದರೆ(state->ಬಹುಲಕ.origin) {
      state->pos.ಅಡ್ಡಸಾಲು += state->scrollregion_ತುದಿ;
      state->pos.ಲಂಬಸಾಲು += SCROLLREGION_ಎಡ(state);
    }

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_ಎಡ = CSI_ARG_OR(ವಾದಗಳು[0], 1) - 1;
    state->scrollregion_ಬಲ = argcount < 2 || CSI_ARG_IS_MISSING(ವಾದಗಳು[1]) ? -1 : CSI_ARG(ವಾದಗಳು[1]);
    LBOUND(state->scrollregion_ಎಡ, 0);
    UBOUND(state->scrollregion_ಎಡ, state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
    LBOUND(state->scrollregion_ಬಲ, -1);
    ಆದರೆ(state->scrollregion_ಎಡ == 0 && state->scrollregion_ಬಲ == state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
      state->scrollregion_ಬಲ = -1;
    ಇಲ್ಲದಿದ್ದರೆ
      UBOUND(state->scrollregion_ಬಲ, state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

    ಆದರೆ(state->scrollregion_ಬಲ > -1 &&
       state->scrollregion_ಬಲ <= state->scrollregion_ಎಡ) {
      // Invalid
      state->scrollregion_ಎಡ  = 0;
      state->scrollregion_ಬಲ = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.ಅಡ್ಡಸಾಲು = 0;
    state->pos.ಲಂಬಸಾಲು = 0;
    ಆದರೆ(state->ಬಹುಲಕ.origin) {
      state->pos.ಅಡ್ಡಸಾಲು += state->scrollregion_ತುದಿ;
      state->pos.ಲಂಬಸಾಲು += SCROLLREGION_ಎಡ(state);
    }

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ 0x74:
    ಆಯ್ಕೆ(CSI_ARG(ವಾದಗಳು[0])) {
      ಸ್ಥಿತಿ 8: // CSI 8 ; rows ; cols t  set size
        ಆದರೆ (argcount == 3 &&
            !CSI_ARG_IS_MISSING(ವಾದಗಳು[1]) && !CSI_ARG_IS_MISSING(ವಾದಗಳು[2]) &&
            CSI_ARG(ವಾದಗಳು[1]) > 0 && CSI_ARG(ವಾದಗಳು[2]) > 0)
	  on_resize(CSI_ARG(ವಾದಗಳು[1]), CSI_ARG(ವಾದಗಳು[2]), state);
	ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
	handled = 0;
	ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ INTERMED('\'', 0x7D): // DECIC
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    ಆದರೆ(!is_cursor_ಒಳ_scrollregion(state))
      ನಿಲ್ಲಿಸು;

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->scrollregion_ತುದಿ;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು;
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, 0, -ಎಣಿಕೆ_2);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ INTERMED('\'', 0x7E): // DECDC
    ಎಣಿಕೆ_2 = CSI_ARG_ಎಣಿಕೆ(ವಾದಗಳು[0]);

    ಆದರೆ(!is_cursor_ಒಳ_scrollregion(state))
      ನಿಲ್ಲಿಸು;

    rect.ಆರಂಭ_ಅಡ್ಡಸಾಲು = state->scrollregion_ತುದಿ;
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು   = SCROLLREGION_BOTTOM(state);
    rect.ಆರಂಭ_ಲಂಬಸಾಲು = state->pos.ಲಂಬಸಾಲು;
    rect.ಕೊನೆ_ಲಂಬಸಾಲು   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, 0, ಎಣಿಕೆ_2);

    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    handled = 0;
    ನಿಲ್ಲಿಸು;
  }

  ಆದರೆ (!handled) {
    ಆದರೆ(state->fallbacks && state->fallbacks->csi)
      ಆದರೆ((*state->fallbacks->csi)(leader, ವಾದಗಳು, argcount, intermed, command, state->fbdata))
        ಹಿಂತಿರುಗಿ 1;

    ಹಿಂತಿರುಗಿ 0;
  }

  ಆದರೆ(state->ಬಹುಲಕ.origin) {
    LBOUND(state->pos.ಅಡ್ಡಸಾಲು, state->scrollregion_ತುದಿ);
    UBOUND(state->pos.ಅಡ್ಡಸಾಲು, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.ಲಂಬಸಾಲು, SCROLLREGION_ಎಡ(state));
    UBOUND(state->pos.ಲಂಬಸಾಲು, SCROLLREGION_ಬಲ(state)-1);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    LBOUND(state->pos.ಅಡ್ಡಸಾಲು, 0);
    UBOUND(state->pos.ಅಡ್ಡಸಾಲು, state->ಅಡ್ಡಸಾಲುಗಳು_2-1);
    LBOUND(state->pos.ಲಂಬಸಾಲು, 0);
    UBOUND(state->pos.ಲಂಬಸಾಲು, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUG
  ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು < 0 || state->pos.ಅಡ್ಡಸಾಲು >= state->ಅಡ್ಡಸಾಲುಗಳು_2 ||
     state->pos.ಲಂಬಸಾಲು < 0 || state->pos.ಲಂಬಸಾಲು >= state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.ಅಡ್ಡಸಾಲು, state->pos.ಲಂಬಸಾಲು);
    abort();
  }

  ಆದರೆ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ತುದಿ) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_ತುದಿ);
    abort();
  }

  ಆದರೆ(SCROLLREGION_ಬಲ(state) <= SCROLLREGION_ಎಡ(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_ಬಲ(state), SCROLLREGION_ಎಡ(state));
    abort();
  }
#ಆದರೆ_ಅಂತ್ಯ

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಅಕ್ಷರ base64_one(uint8_t b)
{
  ಆದರೆ(b < 26)
    ಹಿಂತಿರುಗಿ 'A' + b;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b < 52)
    ಹಿಂತಿರುಗಿ 'a' + b - 26;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b < 62)
    ಹಿಂತಿರುಗಿ '0' + b - 52;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b == 62)
    ಹಿಂತಿರುಗಿ '+';
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(b == 63)
    ಹಿಂತಿರುಗಿ '/';
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ uint8_t unbase64one(ಅಕ್ಷರ c)
{
  ಆದರೆ(c >= 'A' && c <= 'Z')
    ಹಿಂತಿರುಗಿ c - 'A';
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= 'a' && c <= 'z')
    ಹಿಂತಿರುಗಿ c - 'a' + 26;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c >= '0' && c <= '9')
    ಹಿಂತಿರುಗಿ c - '0' + 52;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '+')
    ಹಿಂತಿರುಗಿ 62;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '/')
    ಹಿಂತಿರುಗಿ 63;

  ಹಿಂತಿರುಗಿ 0xFF;
}

ಸ್ಥಿರ ಶೂನ್ಯ osc_selection(VTermState *state, VTermStringFragment frag)
{
  ಆದರೆ(frag.initial) {
    state->tmp.selection.ಮುಖವಾಡ = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  ಆಗ(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    ಆಯ್ಕೆ(frag.str[0]) {
      ಸ್ಥಿತಿ 'c':
        state->tmp.selection.ಮುಖವಾಡ |= VTERM_SELECTION_CLIPBOARD;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 'p':
        state->tmp.selection.ಮುಖವಾಡ |= VTERM_SELECTION_PRIMARY;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 'q':
        state->tmp.selection.ಮುಖವಾಡ |= VTERM_SELECTION_SECONDARY;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 's':
        state->tmp.selection.ಮುಖವಾಡ |= VTERM_SELECTION_SELECT;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ '0':
      ಸ್ಥಿತಿ '1':
      ಸ್ಥಿತಿ '2':
      ಸ್ಥಿತಿ '3':
      ಸ್ಥಿತಿ '4':
      ಸ್ಥಿತಿ '5':
      ಸ್ಥಿತಿ '6':
      ಸ್ಥಿತಿ '7':
        state->tmp.selection.ಮುಖವಾಡ |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        ಆದರೆ(!state->tmp.selection.ಮುಖವಾಡ)
          state->tmp.selection.ಮುಖವಾಡ = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        ನಿಲ್ಲಿಸು;
    }

    frag.str++;
    frag.len--;
  }

  ಆದರೆ(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    ಆದರೆ(frag.final && state->selection.callbacks->ಗಣ_2) {
      (*state->selection.callbacks->ಗಣ_2)(state->tmp.selection.ಮುಖವಾಡ, (VTermStringFragment){
              .str     = ಖಾಲಿ,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_ಗಣ,
              .final   = TRUE,
            }, state->selection.user);
    }
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(state->tmp.selection.state == SELECTION_SELECTED) {
    ಆದರೆ(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      state->tmp.selection.state = SELECTION_ಗಣ_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  ಆದರೆ(state->tmp.selection.state == SELECTION_QUERY) {
    ಆದರೆ(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.ಮುಖವಾಡ, state->selection.user);
    ಹಿಂತಿರುಗಿ;
  }

  ಆದರೆ(state->tmp.selection.state == SELECTION_INVALID)
    ಹಿಂತಿರುಗಿ;

  ಆದರೆ(state->selection.callbacks->ಗಣ_2) {
    size_t bufcur = 0;
    ಅಕ್ಷರ *ಬಫರ್ = state->selection.ಬಫರ್;

    uint32_t x = 0; /* Current decoding value */
    ಪೂರ್ಣಾಂಕ n = 0;      /* Number of sextets consumed */

    ಆದರೆ(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    ಆಗ((state->selection.buflen - bufcur) >= 3 && frag.len) {
      ಆದರೆ(frag.str[0] == '=') {
        ಆದರೆ(n == 2) {
          ಬಫರ್[0] = (x >> 4) & 0xFF;
          ಬಫರ್ += 1, bufcur += 1;
        }
        ಆದರೆ(n == 3) {
          ಬಫರ್[0] = (x >> 10) & 0xFF;
          ಬಫರ್[1] = (x >>  2) & 0xFF;
          ಬಫರ್ += 2, bufcur += 2;
        }

        ಆಗ(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        uint8_t b = unbase64one(frag.str[0]);
        ಆದರೆ(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          ಆದರೆ(state->selection.callbacks->ಗಣ_2) {
            (*state->selection.callbacks->ಗಣ_2)(state->tmp.selection.ಮುಖವಾಡ, (VTermStringFragment){
                .str     = ಖಾಲಿ,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          ನಿಲ್ಲಿಸು;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        ಆದರೆ(n == 4) {
          ಬಫರ್[0] = (x >> 16) & 0xFF;
          ಬಫರ್[1] = (x >>  8) & 0xFF;
          ಬಫರ್[2] = (x >>  0) & 0xFF;

          ಬಫರ್ += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      ಆದರೆ(!frag.len || (state->selection.buflen - bufcur) < 3) {
        ಆದರೆ(bufcur) {
	  VTermStringFragment setfrag = {
	    state->selection.ಬಫರ್, // str
	    bufcur, // len
	    state->tmp.selection.state == SELECTION_ಗಣ_INITIAL, // initial
	    frag.final && !frag.len // final
	  };
          (*state->selection.callbacks->ಗಣ_2)(state->tmp.selection.ಮುಖವಾಡ,
	      setfrag, state->selection.user);
          state->tmp.selection.state = SELECTION_ಗಣ;
        }

        ಬಫರ್ = state->selection.ಬಫರ್;
        bufcur = 0;
      }
    }

    ಆದರೆ(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_osc(ಪೂರ್ಣಾಂಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಆಯ್ಕೆ(command) {
    ಸ್ಥಿತಿ 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ಹಿಂತಿರುಗಿ 1;

    ಸ್ಥಿತಿ 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      ಹಿಂತಿರುಗಿ 1;

    ಸ್ಥಿತಿ 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ಹಿಂತಿರುಗಿ 1;

    ಸ್ಥಿತಿ 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        ಪೂರ್ಣಾಂಕ red = state->default_fg.red;
        ಪೂರ್ಣಾಂಕ blue = state->default_fg.blue;
        ಪೂರ್ಣಾಂಕ green = state->default_fg.green;
        vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        ಹಿಂತಿರುಗಿ 1;
      }

    ಸ್ಥಿತಿ 11:
      {
	// request background color: <Esc>]11;?<0x07>
	ಪೂರ್ಣಾಂಕ red = state->default_bg.red;
	ಪೂರ್ಣಾಂಕ blue = state->default_bg.blue;
	ಪೂರ್ಣಾಂಕ green = state->default_bg.green;
	vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	ಹಿಂತಿರುಗಿ 1;
      }
    ಸ್ಥಿತಿ 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      ಹಿಂತಿರುಗಿ 1;

    ಸ್ಥಿತಿ 52:
      ಆದರೆ(state->selection.callbacks)
        osc_selection(state, frag);

      ಹಿಂತಿರುಗಿ 1;

    ಪೂರ್ವನಿಯೋಜಿತ:
      ಆದರೆ(state->fallbacks && state->fallbacks->osc)
        ಆದರೆ((*state->fallbacks->osc)(command, frag, state->fbdata))
          ಹಿಂತಿರುಗಿ 1;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ request_ಸ್ಥಿತಿ_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ಅಕ್ಷರ *tmp = state->tmp.decrqss;

  ಆದರೆ(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  ಆಗ(i < ಗಾತ್ರ(state->tmp.decrqss)-1 && tmp[i])
    i++;
  ಆಗ(i < ಗಾತ್ರ(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  ಆದರೆ(!frag.final)
    ಹಿಂತಿರುಗಿ;

  ಆಯ್ಕೆ(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    ಸ್ಥಿತಿ 'm': {
      // Query SGR
      ದೀರ್ಘ ವಾದಗಳು[20];
      ಪೂರ್ಣಾಂಕ argc = vterm_state_getpen(state, ವಾದಗಳು, ಗಾತ್ರ(ವಾದಗಳು)/ಗಾತ್ರ(ವಾದಗಳು[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->ಬಹುಲಕ.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      ಆದರೆ(cur >= vt->tmpbuffer_len)
        ಹಿಂತಿರುಗಿ;

      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_ಇನ್ನಷ್ಟು(ವಾದಗಳು[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(ವಾದಗಳು[argi]));
        ಆದರೆ(cur >= vt->tmpbuffer_len)
          ಹಿಂತಿರುಗಿ;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->ಬಹುಲಕ.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      ಆದರೆ(cur >= vt->tmpbuffer_len)
        ಹಿಂತಿರುಗಿ;

      vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(vt, vt->tmpbuffer, cur);
      ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿತಿ 'r':
      // Query DECSTBM
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_ತುದಿ+1, SCROLLREGION_BOTTOM(state));
      ಹಿಂತಿರುಗಿ;

    ಸ್ಥಿತಿ 's':
      // Query DECSLRM
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_ಎಡ(state)+1, SCROLLREGION_ಬಲ(state));
      ಹಿಂತಿರುಗಿ;

    ಸ್ಥಿತಿ ' '|('q'<<8): {
      // Query DECSCUSR
      ಪೂರ್ಣಾಂಕ ಉತ್ತರ;
      ಆಯ್ಕೆ(state->ಬಹುಲಕ.cursor_shape) {
        ಸ್ಥಿತಿ VTERM_PROP_CURSORSHAPE_ಖಂಡ:     ಉತ್ತರ = 2; ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ VTERM_PROP_CURSORSHAPE_UNDERLINE: ಉತ್ತರ = 4; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ ಉತ್ತರ = 6; ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(state->ಬಹುಲಕ.cursor_blink)
        ಉತ್ತರ--;
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", ಉತ್ತರ);
      ಹಿಂತಿರುಗಿ;
    }

    ಸ್ಥಿತಿ '\"'|('q'<<8):
      // Query DECSCA
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      ಹಿಂತಿರುಗಿ;
  }

  vterm_ತಳ್ಳು_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_dcs(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಆದರೆ(commandlen == 2 && strneq(command, "$q", 2)) {
    request_ಸ್ಥಿತಿ_string(state, frag);
    ಹಿಂತಿರುಗಿ 1;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(state->fallbacks && state->fallbacks->dcs)
    ಆದರೆ((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      ಹಿಂತಿರುಗಿ 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (ಪೂರ್ಣಾಂಕ)commandlen, command);
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_apc(VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಆದರೆ(state->fallbacks && state->fallbacks->apc)
    ಆದರೆ((*state->fallbacks->apc)(frag, state->fbdata))
      ಹಿಂತಿರುಗಿ 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_pm(VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಆದರೆ(state->fallbacks && state->fallbacks->pm)
    ಆದರೆ((*state->fallbacks->pm)(frag, state->fbdata))
      ಹಿಂತಿರುಗಿ 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_sos(VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಆದರೆ(state->fallbacks && state->fallbacks->sos)
    ಆದರೆ((*state->fallbacks->sos)(frag, state->fbdata))
      ಹಿಂತಿರುಗಿ 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ on_resize(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, ಶೂನ್ಯ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > VTERM_MAX_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = VTERM_MAX_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು_2 > VTERM_MAX_ಅಡ್ಡಸಾಲುಗಳು)
    ಅಡ್ಡಸಾಲುಗಳು_2 = VTERM_MAX_ಅಡ್ಡಸಾಲುಗಳು;

  ಆದರೆ(ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *newtabstops = vterm_allocator_malloc(state->vt, (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ + 7) / 8);
    ಆದರೆ (newtabstops == ಖಾಲಿ)
      ಹಿಂತಿರುಗಿ 0;

    /* TODO: This can all be done much more efficiently bytewise */
    ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು;
    ಗಾಗಿ(ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ && ಲಂಬಸಾಲು < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮುಖವಾಡ = 1 << (ಲಂಬಸಾಲು & 7);
      ಆದರೆ(state->tabstops[ಲಂಬಸಾಲು >> 3] & ಮುಖವಾಡ)
        newtabstops[ಲಂಬಸಾಲು >> 3] |= ಮುಖವಾಡ;
      ಇಲ್ಲದಿದ್ದರೆ
        newtabstops[ಲಂಬಸಾಲು >> 3] &= ~ಮುಖವಾಡ;
      }

    ಗಾಗಿ( ; ಲಂಬಸಾಲು < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಮುಖವಾಡ = 1 << (ಲಂಬಸಾಲು & 7);
      ಆದರೆ(ಲಂಬಸಾಲು % 8 == 0)
        newtabstops[ಲಂಬಸಾಲು >> 3] |= ಮುಖವಾಡ;
      ಇಲ್ಲದಿದ್ದರೆ
        newtabstops[ಲಂಬಸಾಲು >> 3] &= ~ಮುಖವಾಡ;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->ಅಡ್ಡಸಾಲುಗಳು_2 = ಅಡ್ಡಸಾಲುಗಳು_2;
  state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

  ಆದರೆ(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->ಅಡ್ಡಸಾಲುಗಳು_2);
  ಆದರೆ(state->scrollregion_ಬಲ > -1)
    UBOUND(state->scrollregion_ಬಲ, state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_ತುದಿ, state->ಅಡ್ಡಸಾಲುಗಳು_2);
  UBOUND(state->scrollregion_ಎಡ, state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
  ಆದರೆ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ತುದಿ) {
    state->scrollregion_ತುದಿ = 0;
    state->scrollregion_bottom = -1;
  }
  ಆದರೆ(SCROLLREGION_ಬಲ(state) <= state->scrollregion_ಎಡ) {
    state->scrollregion_ಎಡ = 0;
    state->scrollregion_ಬಲ = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  ಆದರೆ(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(ಅಡ್ಡಸಾಲುಗಳು_2, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಆದರೆ(ಅಡ್ಡಸಾಲುಗಳು_2 != state->ಅಡ್ಡಸಾಲುಗಳು_2) {
      ಗಾಗಿ(ಪೂರ್ಣಾಂಕ bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_ಸಾಲು_Info *oldlineinfo = state->lineinfos[bufidx];
        ಆದರೆ(!oldlineinfo)
          ಮುಂದುವರಿ;

        VTerm_ಸಾಲು_Info *newlineinfo = vterm_allocator_malloc(state->vt, ಅಡ್ಡಸಾಲುಗಳು_2 * ಗಾತ್ರ(VTerm_ಸಾಲು_Info));

        ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು;
        ಗಾಗಿ(ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < state->ಅಡ್ಡಸಾಲುಗಳು_2 && ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು_2; ಅಡ್ಡಸಾಲು++) {
          newlineinfo[ಅಡ್ಡಸಾಲು] = oldlineinfo[ಅಡ್ಡಸಾಲು];
        }

        ಗಾಗಿ( ; ಅಡ್ಡಸಾಲು < ಅಡ್ಡಸಾಲುಗಳು_2; ಅಡ್ಡಸಾಲು++) {
          newlineinfo[ಅಡ್ಡಸಾಲು] = (VTerm_ಸಾಲು_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->ಬಹುಲಕ.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  ಆದರೆ(state->at_phantom && state->pos.ಲಂಬಸಾಲು < ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ-1) {
    state->at_phantom = 0;
    state->pos.ಲಂಬಸಾಲು++;
  }

  ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು < 0)
    state->pos.ಅಡ್ಡಸಾಲು = 0;
  ಆದರೆ(state->pos.ಅಡ್ಡಸಾಲು >= ಅಡ್ಡಸಾಲುಗಳು_2)
    state->pos.ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು_2 - 1;
  ಆದರೆ(state->pos.ಲಂಬಸಾಲು < 0)
    state->pos.ಲಂಬಸಾಲು = 0;
  ಆದರೆ(state->pos.ಲಂಬಸಾಲು >= ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
    state->pos.ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ - 1;

  updatecursor(state, &oldpos, 1);

  ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ VTermParserCallbacks parser_callbacks = {
  on_ಪಠ್ಯ, //ಪಠ್ಯ
  on_control, // control
  on_escape, // escape
  on_csi, // csi
  on_osc, // osc
  on_dcs, // dcs
  on_apc, // apc
  on_pm, // pm
  on_sos, // sos
  on_resize // resize
};

/*
 * Return the existing state or create a new one.
 * Returns NULL when out of memory.
 */
VTermState *vterm_obtain_state(VTerm *vt)
{
  ಆದರೆ(vt->state)
    ಹಿಂತಿರುಗಿ vt->state;

  VTermState *state = vterm_state_new(vt);
  ಆದರೆ (state == ಖಾಲಿ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  vt->state = state;

  vterm_parser_ಗಣ_callbacks(vt, &parser_callbacks, state);

  ಹಿಂತಿರುಗಿ state;
}

ಶೂನ್ಯ vterm_state_reset(VTermState *state, ಪೂರ್ಣಾಂಕ hard)
{
  state->scrollregion_ತುದಿ = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_ಎಡ = 0;
  state->scrollregion_ಬಲ = -1;

  state->ಬಹುಲಕ.keypad          = 0;
  state->ಬಹುಲಕ.cursor          = 0;
  state->ಬಹುಲಕ.autowrap        = 1;
  state->ಬಹುಲಕ.ಸೇರಿಸು          = 0;
  state->ಬಹುಲಕ.newline         = 0;
  state->ಬಹುಲಕ.alt_screen      = 0;
  state->ಬಹುಲಕ.origin          = 0;
  state->ಬಹುಲಕ.leftrightmargin = 0;
  state->ಬಹುಲಕ.bracketpaste    = 0;
  state->ಬಹುಲಕ.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->ಬಹುಲಕ.ctrl8bit   = 0;

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು = 0; ಲಂಬಸಾಲು < state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ; ಲಂಬಸಾಲು++)
    ಆದರೆ(ಲಂಬಸಾಲು % 8 == 0)
      ಗಣ_ಲಂಬಸಾಲು_tabstop(state, ಲಂಬಸಾಲು);
    ಇಲ್ಲದಿದ್ದರೆ
      clear_ಲಂಬಸಾಲು_tabstop(state, ಲಂಬಸಾಲು);

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು = 0; ಅಡ್ಡಸಾಲು < state->ಅಡ್ಡಸಾಲುಗಳು_2; ಅಡ್ಡಸಾಲು++)
    ಗಣ_lineinfo(state, ಅಡ್ಡಸಾಲು, FORCE, DWL_OFF, DHL_OFF);

  ಆದರೆ(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->ಬಹುಲಕ.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  ಗಾಗಿ(ಪೂರ್ಣಾಂಕ i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    ಆದರೆ(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].ದತ್ತಾಂಶ);
  }

  state->gl_ಗಣ = 0;
  state->gr_ಗಣ = 1;
  state->gsingle_ಗಣ = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_ಖಂಡ);

  ಆದರೆ(hard) {
    state->pos.ಅಡ್ಡಸಾಲು = 0;
    state->pos.ಲಂಬಸಾಲು = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = state->ಅಡ್ಡಸಾಲುಗಳು_2;
    rect.ಕೊನೆ_ಲಂಬಸಾಲು =  state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    erase(state, rect, 0);
  }
}

ಶೂನ್ಯ vterm_state_get_cursorpos(ಸ್ಥಿರಾಂಕ VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

ಶೂನ್ಯ vterm_state_get_mousestate(ಸ್ಥಿರಾಂಕ VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.ಲಂಬಸಾಲು = state->mouse_ಲಂಬಸಾಲು;
  mousestate->pos.ಅಡ್ಡಸಾಲು = state->mouse_ಅಡ್ಡಸಾಲು;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

ಶೂನ್ಯ vterm_state_ಗಣ_callbacks(VTermState *state, ಸ್ಥಿರಾಂಕ VTermStateCallbacks *callbacks, ಶೂನ್ಯ *user)
{
  ಆದರೆ(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    ಆದರೆ(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    state->callbacks = ಖಾಲಿ;
    state->cbdata = ಖಾಲಿ;
  }
}

ಶೂನ್ಯ vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

ಶೂನ್ಯ *vterm_state_get_cbdata(VTermState *state)
{
  ಹಿಂತಿರುಗಿ state->cbdata;
}

ಶೂನ್ಯ vterm_state_ಗಣ_unrecognised_fallbacks(VTermState *state, ಸ್ಥಿರಾಂಕ VTermStateFallbacks *fallbacks, ಶೂನ್ಯ *user)
{
  ಆದರೆ(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    state->fallbacks = ಖಾಲಿ;
    state->fbdata = ಖಾಲಿ;
  }
}

ಶೂನ್ಯ *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  ಹಿಂತಿರುಗಿ state->fbdata;
}

ಪೂರ್ಣಾಂಕ vterm_state_ಗಣ_termprop(VTermState *state, VTermProp prop, VTerm_ಮೌಲ್ಯ *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  ಆದರೆ(state->callbacks && state->callbacks->settermprop)
    ಆದರೆ(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      ಹಿಂತಿರುಗಿ 0;

  ಆಯ್ಕೆ(prop) {
  ಸ್ಥಿತಿ VTERM_PROP_TITLE:
  ಸ್ಥಿತಿ VTERM_PROP_ICONNAME:
  ಸ್ಥಿತಿ VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_PROP_CURSORVISIBLE:
    state->ಬಹುಲಕ.cursor_visible = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_PROP_CURSORBLINK:
    state->ಬಹುಲಕ.cursor_blink = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_PROP_CURSORSHAPE:
    state->ಬಹುಲಕ.cursor_shape = val->ಸಂಖ್ಯೆ;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_PROP_ಹಿಮ್ಮುಖ:
    state->ಬಹುಲಕ.screen = val->boolean;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_PROP_ALTSCREEN:
    state->ಬಹುಲಕ.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->ಬಹುಲಕ.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    ಆದರೆ(state->ಬಹುಲಕ.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.ಕೊನೆ_ಅಡ್ಡಸಾಲು = state->ಅಡ್ಡಸಾಲುಗಳು_2;
      rect.ಕೊನೆ_ಲಂಬಸಾಲು = state->ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
      erase(state, rect, 0);
    }
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    ಆದರೆ(val->ಸಂಖ್ಯೆ)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    ಆದರೆ(val->ಸಂಖ್ಯೆ == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    ಆದರೆ(val->ಸಂಖ್ಯೆ == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    ಹಿಂತಿರುಗಿ 1;
  ಸ್ಥಿತಿ VTERM_PROP_FOCUSREPORT:
    state->ಬಹುಲಕ.report_focus = val->boolean;
    ಹಿಂತಿರುಗಿ 1;

  ಸ್ಥಿತಿ VTERM_N_PROPS:
    ಹಿಂತಿರುಗಿ 0;
  }

  ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ vterm_state_focus_ಒಳ(VTermState *state)
{
  ಆದರೆ(state->ಬಹುಲಕ.report_focus)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

ಶೂನ್ಯ vterm_state_focus_ಹೊರಹೋಗು(VTermState *state)
{
  ಆದರೆ(state->ಬಹುಲಕ.report_focus)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ಸ್ಥಿರಾಂಕ VTerm_ಸಾಲು_Info *vterm_state_get_lineinfo(ಸ್ಥಿರಾಂಕ VTermState *state, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು)
{
  ಹಿಂತಿರುಗಿ state->lineinfo + ಅಡ್ಡಸಾಲು;
}

ಶೂನ್ಯ vterm_state_ಗಣ_selection_callbacks(VTermState *state, ಸ್ಥಿರಾಂಕ VTermSelectionCallbacks *callbacks, ಶೂನ್ಯ *user,
    ಅಕ್ಷರ *ಬಫರ್, size_t buflen)
{
  ಆದರೆ(buflen && !ಬಫರ್)
    ಬಫರ್ = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.ಬಫರ್    = ಬಫರ್;
  state->selection.buflen    = buflen;
}

ಶೂನ್ಯ vterm_state_send_selection(VTermState *state, VTerm_Selection_ಮುಖವಾಡ ಮುಖವಾಡ, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ಆದರೆ(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ಸ್ಥಿರ ಅಕ್ಷರ selection_chars[] = "cpqs";
    ಪೂರ್ಣಾಂಕ idx;
    ಗಾಗಿ(idx = 0; idx < 4; idx++)
      ಆದರೆ(ಮುಖವಾಡ & (1 << idx))
        ನಿಲ್ಲಿಸು;

    vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  ಆದರೆ(frag.len) {
    size_t bufcur = 0;
    ಅಕ್ಷರ *ಬಫರ್ = state->selection.ಬಫರ್;

    uint32_t x = 0;
    ಪೂರ್ಣಾಂಕ n = 0;

    ಆದರೆ(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    ಆಗ((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      ಆದರೆ(n == 3) {
        ಬಫರ್[0] = base64_one((x >> 18) & 0x3F);
        ಬಫರ್[1] = base64_one((x >> 12) & 0x3F);
        ಬಫರ್[2] = base64_one((x >>  6) & 0x3F);
        ಬಫರ್[3] = base64_one((x >>  0) & 0x3F);

        ಬಫರ್ += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      ಆದರೆ(!frag.len || (state->selection.buflen - bufcur) < 4) {
        ಆದರೆ(bufcur)
          vterm_ತಳ್ಳು_output_ಬೈಟುಗಳು(vt, state->selection.ಬಫರ್, bufcur);

        ಬಫರ್ = state->selection.ಬಫರ್;
        bufcur = 0;
      }
    }

    ಆದರೆ(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  ಆದರೆ(frag.final) {
    ಆದರೆ(state->tmp.selection.sendpartial) {
      ಪೂರ್ಣಾಂಕ n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      ಅಕ್ಷರ *ಬಫರ್ = state->selection.ಬಫರ್;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      ಬಫರ್[0] = base64_one((x >> 18) & 0x3F);
      ಬಫರ್[1] = base64_one((x >> 12) & 0x3F);
      ಬಫರ್[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      ಬಫರ್[3] = '=';

      vterm_ತಳ್ಳು_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, ಬಫರ್);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      vterm_ತಳ್ಳು_output_sprintf_str(vt, 0, TRUE, "");
  }
}
