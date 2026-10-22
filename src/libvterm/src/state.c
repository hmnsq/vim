#ਅਨ੍ਤਰ੍ਭਾਵ "vterm_internal.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ strneq(a,b,n) (ਲੜੀ_ਤੁਲਨਾ(a,b,n)==0)

#ਜੇ defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ on_resize(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਖਾਲੀ *user);

/* Some convenient wrappers to make callback functions easier */

ਸਥਿਰ ਖਾਲੀ putglyph(VTermState *state, ਅਚਲ uint32_t chars[], ਪੂਰਨ ਚੌੜਾਈ, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.ਚੌੜਾਈ = ਚੌੜਾਈ;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.ਪੰਕਤੀ].doublewidth;
  info.dhl = state->lineinfo[pos.ਪੰਕਤੀ].doubleheight;

  ਜੇ(state->callbacks && state->callbacks->putglyph)
    ਜੇ((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      ਵਾਪਸ;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.ਖਾਨਾ, pos.ਪੰਕਤੀ);
}

ਸਥਿਰ ਖਾਲੀ updatecursor(VTermState *state, VTermPos *oldpos, ਪੂਰਨ cancel_phantom)
{
  ਜੇ(state->pos.ਖਾਨਾ == oldpos->ਖਾਨਾ && state->pos.ਪੰਕਤੀ == oldpos->ਪੰਕਤੀ)
    ਵਾਪਸ;

  ਜੇ(cancel_phantom)
    state->at_phantom = 0;

  ਜੇ(state->callbacks && state->callbacks->movecursor)
    ਜੇ((*state->callbacks->movecursor)(state->pos, *oldpos, state->ਬਹੁਲਕ.cursor_visible, state->cbdata))
      ਵਾਪਸ;
}

ਸਥਿਰ ਖਾਲੀ erase(VTermState *state, VTermRect rect, ਪੂਰਨ selective)
{
  ਜੇ(rect.ਸਿਰਾ_ਖਾਨਾ == state->ਖਾਨੇ_ਗਿਣਤੀ) {
    ਪੂਰਨ ਪੰਕਤੀ;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    ਲਈ(ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ + 1; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ + 1 && ਪੰਕਤੀ < state->ਪੰਕਤੀਆਂ_2; ਪੰਕਤੀ++)
      state->lineinfo[ਪੰਕਤੀ].continuation = 0;
  }

  ਜੇ(state->callbacks && state->callbacks->erase)
    ਜੇ((*state->callbacks->erase)(rect, selective, state->cbdata))
      ਵਾਪਸ;
}

ਸਥਿਰ VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, ਆਕਾਰ(VTermState));

  ਜੇ (state == ਨਿਰਾ)
    ਵਾਪਸ ਨਿਰਾ;
  state->vt = vt;

  state->ਪੰਕਤੀਆਂ_2 = vt->ਪੰਕਤੀਆਂ_2;
  state->ਖਾਨੇ_ਗਿਣਤੀ = vt->ਖਾਨੇ_ਗਿਣਤੀ;

  state->mouse_ਖਾਨਾ     = 0;
  state->mouse_ਪੰਕਤੀ     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ਨਿਰਾ;
  state->cbdata    = ਨਿਰਾ;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ਨਿਰਾ;
  state->selection.user      = ਨਿਰਾ;
  state->selection.ਬਫਰ    = ਨਿਰਾ;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_ਮਾਪ = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_ਮਾਪ * ਆਕਾਰ(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->ਖਾਨੇ_ਗਿਣਤੀ + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->ਪੰਕਤੀਆਂ_2 * ਆਕਾਰ(VTerm_ਸਤਰ_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->ਪੰਕਤੀਆਂ_2 * ਆਕਾਰ(VTerm_ਸਤਰ_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  ਜੇ(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.ਅੰਕੜੇ);

  ਵਾਪਸ state;
}

ਅੰਦਰੂਨੀ ਖਾਲੀ vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  ਜੇ(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ਸਥਿਰ ਖਾਲੀ scroll(VTermState *state, VTermRect rect, ਪੂਰਨ downward, ਪੂਰਨ rightward)
{
  ਪੂਰਨ ਪੰਕਤੀਆਂ_2;
  ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ;
  ਜੇ(!downward && !rightward)
    ਵਾਪਸ;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  ਜੇ(rect.ਸਿਰਾ_ਪੰਕਤੀ <= rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ || rect.ਸਿਰਾ_ਖਾਨਾ <= rect.ਸ਼ੁਰੂ_ਖਾਨਾ)
    ਵਾਪਸ;

  ਪੰਕਤੀਆਂ_2 = rect.ਸਿਰਾ_ਪੰਕਤੀ - rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ;
  ਜੇ(downward > ਪੰਕਤੀਆਂ_2)
    downward = ਪੰਕਤੀਆਂ_2;
  ਨਹੀਂਤਾਂ ਜੇ(downward < -ਪੰਕਤੀਆਂ_2)
    downward = -ਪੰਕਤੀਆਂ_2;

  ਖਾਨੇ_ਗਿਣਤੀ = rect.ਸਿਰਾ_ਖਾਨਾ - rect.ਸ਼ੁਰੂ_ਖਾਨਾ;
  ਜੇ(rightward > ਖਾਨੇ_ਗਿਣਤੀ)
    rightward = ਖਾਨੇ_ਗਿਣਤੀ;
  ਨਹੀਂਤਾਂ ਜੇ(rightward < -ਖਾਨੇ_ਗਿਣਤੀ)
    rightward = -ਖਾਨੇ_ਗਿਣਤੀ;

  ਜੇ(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    ਜੇ(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, .ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ + downward,
          .ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ, .ਸਿਰਾ_ਖਾਨਾ = rect.ਸਿਰਾ_ਖਾਨਾ}, state->cbdata);
    ਨਹੀਂਤਾਂ ਜੇ(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ + downward, .ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ,
          .ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ,          .ਸਿਰਾ_ਖਾਨਾ = rect.ਸਿਰਾ_ਖਾਨਾ}, state->cbdata);

    ਜੇ(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ, .ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ,
          .ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ, .ਸਿਰਾ_ਖਾਨਾ = rect.ਸ਼ੁਰੂ_ਖਾਨਾ + rightward}, state->cbdata);
    ਨਹੀਂਤਾਂ ਜੇ(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .ਸ਼ੁਰੂ_ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ,           .ਸਿਰਾ_ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ,
          .ਸ਼ੁਰੂ_ਖਾਨਾ = rect.ਸਿਰਾ_ਖਾਨਾ + rightward, .ਸਿਰਾ_ਖਾਨਾ = rect.ਸਿਰਾ_ਖਾਨਾ}, state->cbdata);
  }

  // Update lineinfo if full line
  ਜੇ(rect.ਸ਼ੁਰੂ_ਖਾਨਾ == 0 && rect.ਸਿਰਾ_ਖਾਨਾ == state->ਖਾਨੇ_ਗਿਣਤੀ && rightward == 0) {
    ਪੂਰਨ ਉਚਾਈ = rect.ਸਿਰਾ_ਪੰਕਤੀ - rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ - ਨਿਰਪੇਖ(downward);
    ਪੂਰਨ ਪੰਕਤੀ;
    VTerm_ਸਤਰ_Info zero_ਸਤਰ_Info = {0x0};

    ਜੇ(downward > 0) {
      memmove(state->lineinfo + rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ,
              state->lineinfo + rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ + downward,
              ਉਚਾਈ * ਆਕਾਰ(state->lineinfo[0]));
      ਲਈ(ਪੰਕਤੀ = rect.ਸਿਰਾ_ਪੰਕਤੀ - downward; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++)
        state->lineinfo[ਪੰਕਤੀ] = zero_ਸਤਰ_Info;
    }
    ਨਹੀਂਤਾਂ {
      memmove(state->lineinfo + rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ - downward,
              state->lineinfo + rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ,
              ਉਚਾਈ * ਆਕਾਰ(state->lineinfo[0]));
      ਲਈ(ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ - downward; ਪੰਕਤੀ++)
        state->lineinfo[ਪੰਕਤੀ] = zero_ਸਤਰ_Info;
    }
  }

  ਜੇ(state->callbacks && state->callbacks->scrollrect)
    ਜੇ((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      ਵਾਪਸ;

  ਜੇ(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ਸਥਿਰ ਖਾਲੀ linefeed(VTermState *state)
{
  ਜੇ(state->pos.ਪੰਕਤੀ == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->scrollregion_ਸਿਖਰ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = SCROLLREGION_ਖੱਬਾ(state);
    rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

    scroll(state, rect, 1, 0);
  }
  ਨਹੀਂਤਾਂ ਜੇ(state->pos.ਪੰਕਤੀ < state->ਪੰਕਤੀਆਂ_2-1)
    state->pos.ਪੰਕਤੀ++;
}

ਸਥਿਰ ਖਾਲੀ grow_combine_ਬਫਰ(VTermState *state)
{
  size_t    new_ਮਾਪ = state->combine_chars_ਮਾਪ * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_ਮਾਪ * ਆਕਾਰ(new_chars[0]));

  ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_chars, state->combine_chars, state->combine_chars_ਮਾਪ * ਆਕਾਰ(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_ਮਾਪ = new_ਮਾਪ;
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_ਖਾਨਾ_tabstop(VTermState *state, ਪੂਰਨ ਖਾਨਾ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਨਕਾਬ = 1 << (ਖਾਨਾ & 7);
  state->tabstops[ਖਾਨਾ >> 3] |= ਨਕਾਬ;
}

ਸਥਿਰ ਖਾਲੀ clear_ਖਾਨਾ_tabstop(VTermState *state, ਪੂਰਨ ਖਾਨਾ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਨਕਾਬ = 1 << (ਖਾਨਾ & 7);
  state->tabstops[ਖਾਨਾ >> 3] &= ~ਨਕਾਬ;
}

ਸਥਿਰ ਪੂਰਨ is_ਖਾਨਾ_tabstop(VTermState *state, ਪੂਰਨ ਖਾਨਾ)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਨਕਾਬ = 1 << (ਖਾਨਾ & 7);
  ਵਾਪਸ state->tabstops[ਖਾਨਾ >> 3] & ਨਕਾਬ;
}

ਸਥਿਰ ਪੂਰਨ is_cursor_ਵਿੱਚ_scrollregion(ਅਚਲ VTermState *state)
{
  ਜੇ(state->pos.ਪੰਕਤੀ < state->scrollregion_ਸਿਖਰ ||
     state->pos.ਪੰਕਤੀ >= SCROLLREGION_BOTTOM(state))
    ਵਾਪਸ 0;
  ਜੇ(state->pos.ਖਾਨਾ < SCROLLREGION_ਖੱਬਾ(state) ||
     state->pos.ਖਾਨਾ >= SCROLLREGION_ਸੱਜਾ(state))
    ਵਾਪਸ 0;

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਖਾਲੀ tab(VTermState *state, ਪੂਰਨ ਗਿਣਤੀ_2, ਪੂਰਨ direction)
{
  ਜਦੋਂ(ਗਿਣਤੀ_2 > 0) {
    ਜੇ(direction > 0) {
      ਜੇ(state->pos.ਖਾਨਾ >= THISROWWIDTH(state)-1)
        ਵਾਪਸ;

      state->pos.ਖਾਨਾ++;
    }
    ਨਹੀਂਤਾਂ ਜੇ(direction < 0) {
      ਜੇ(state->pos.ਖਾਨਾ < 1)
        ਵਾਪਸ;

      state->pos.ਖਾਨਾ--;
    }

    ਜੇ(is_ਖਾਨਾ_tabstop(state, state->pos.ਖਾਨਾ))
      ਗਿਣਤੀ_2--;
  }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ NO_FORCE 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FORCE    1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DWL_OFF 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DWL_ON  1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DHL_OFF    0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DHL_ਸਿਖਰ    1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DHL_BOTTOM 2

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_lineinfo(VTermState *state, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ force, ਪੂਰਨ dwl, ਪੂਰਨ dhl)
{
  VTerm_ਸਤਰ_Info info = state->lineinfo[ਪੰਕਤੀ];

  ਜੇ(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  ਨਹੀਂਤਾਂ ਜੇ(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  ਜੇ(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  ਨਹੀਂਤਾਂ ਜੇ(dhl == DHL_ਸਿਖਰ)
    info.doubleheight = DHL_ਸਿਖਰ;
  ਨਹੀਂਤਾਂ ਜੇ(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  ਜੇ((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(ਪੰਕਤੀ, &info, state->lineinfo + ਪੰਕਤੀ, state->cbdata))
      || force)
    state->lineinfo[ਪੰਕਤੀ] = info;
}

ਸਥਿਰ ਪੂਰਨ on_ਪਾਠ(ਅਚਲ ਅੱਖਰ ਬਾਈਟਾਂ[], size_t len, ਖਾਲੀ *user)
{
  VTermState *state = user;
  ਪੂਰਨ npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  ਪੂਰਨ i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / ਆਕਾਰ(uint32_t);

  encoding =
    state->gsingle_ਸੈੱਟ     ? &state->encoding[state->gsingle_ਸੈੱਟ] :
    !(ਬਾਈਟਾਂ[eaten] & 0x80) ? &state->encoding[state->gl_ਸੈੱਟ] :
    state->vt->ਬਹੁਲਕ.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_ਸੈੱਟ];
  ਜੇ (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->ਅੰਕੜੇ,
      codepoints, &npoints, state->gsingle_ਸੈੱਟ ? 1 : (ਪੂਰਨ)maxpoints,
      ਬਾਈਟਾਂ, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  ਜੇ(!npoints)
  {
    ਵਾਪਸ (ਪੂਰਨ)eaten;
  }

  ਜੇ(state->gsingle_ਸੈੱਟ && npoints)
    state->gsingle_ਸੈੱਟ = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  ਜੇ(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    ਜੇ(state->pos.ਪੰਕਤੀ == state->combine_pos.ਪੰਕਤੀ && state->pos.ਖਾਨਾ == state->combine_pos.ਖਾਨਾ + state->combine_ਚੌੜਾਈ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_GLYPH_COMBINE
      ਪੂਰਨ printpos;
      ਲਿਖੋ("DEBUG: COMBINING SPLIT GLYPH of chars {");
      ਲਈ(printpos = 0; state->combine_chars[printpos]; printpos++)
        ਲਿਖੋ("U+%04x ", state->combine_chars[printpos]);
      ਲਿਖੋ("} + {");
#ਜੇ_ਅੰਤ

      /* Find where we need to append these combining chars */
      ਪੂਰਨ saved_i = 0;
      ਜਦੋਂ(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      ਜਦੋਂ(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        ਜੇ(saved_i >= (ਪੂਰਨ)state->combine_chars_ਮਾਪ)
          grow_combine_ਬਫਰ(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      ਜੇ(saved_i >= (ਪੂਰਨ)state->combine_chars_ਮਾਪ)
        grow_combine_ਬਫਰ(state);
      state->combine_chars[saved_i] = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_GLYPH_COMBINE
      ਲਈ(; state->combine_chars[printpos]; printpos++)
        ਲਿਖੋ("U+%04x ", state->combine_chars[printpos]);
      ਲਿਖੋ("}\n");
#ਜੇ_ਅੰਤ

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_ਚੌੜਾਈ, state->combine_pos);
    }
    ਨਹੀਂਤਾਂ {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  ਲਈ(; i < npoints; i++) {
    // Try to find combining characters following this
    ਪੂਰਨ glyph_starts = i;
    ਪੂਰਨ glyph_ਸਿਰੇ;
    ਪੂਰਨ ਚੌੜਾਈ = 0;

    ਲਈ(glyph_ਸਿਰੇ = i + 1;
        (glyph_ਸਿਰੇ < npoints) && (glyph_ਸਿਰੇ < glyph_starts + VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL);
        glyph_ਸਿਰੇ++)
      ਜੇ(!vterm_unicode_is_combining(codepoints[glyph_ਸਿਰੇ]))
        ਰੋਕੋ;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_ਪ੍ਰਤੀ_CELL + 1) * ਆਕਾਰ(uint32_t));
    ਜੇ (chars == ਨਿਰਾ)
      ਰੋਕੋ;

    ਲਈ( ; i < glyph_ਸਿਰੇ; i++) {
      ਪੂਰਨ this_ਚੌੜਾਈ;
      ਜੇ(vterm_get_special_pty_type() == 2) {
        state->vt->ਵਿੱਚ_backspace -= (state->vt->ਵਿੱਚ_backspace > 0) ? 1 : 0;
        ਜੇ(state->vt->ਵਿੱਚ_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_ਚੌੜਾਈ = vterm_unicode_ਚੌੜਾਈ(codepoints[i]);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
      ਜੇ(this_ਚੌੜਾਈ < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#ਜੇ_ਅੰਤ
      ਜੇ (i == glyph_starts || this_ਚੌੜਾਈ > ਚੌੜਾਈ)
	ਚੌੜਾਈ = this_ਚੌੜਾਈ;  // TODO: should be += ?
    }

    ਜੇ (ਚੌੜਾਈ < 0)
      ਚੌੜਾਈ = 0;

    ਜਦੋਂ(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ਸਿਰੇ - glyph_starts] = 0;
    i--;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG_GLYPH_COMBINE
    ਪੂਰਨ printpos;
    ਲਿਖੋ("DEBUG: COMBINED GLYPH of %d chars {", glyph_ਸਿਰੇ - glyph_starts);
    ਲਈ(printpos = 0; printpos < glyph_ਸਿਰੇ - glyph_starts; printpos++)
      ਲਿਖੋ("U+%04x ", chars[printpos]);
    ਲਿਖੋ("}, onscreen width %d\n", ਚੌੜਾਈ);
#ਜੇ_ਅੰਤ

    ਜੇ(state->at_phantom || state->pos.ਖਾਨਾ + ਚੌੜਾਈ > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.ਖਾਨਾ = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.ਪੰਕਤੀ].continuation = 1;
    }

    ਜੇ(state->ਬਹੁਲਕ.ਪਾਓ) {
      // TODO: This will be a little inefficient for large bodies of text, as
      // it'll have to 'ICH' effectively before every glyph. We should scan
      // ahead and ICH as many times as required
      VTermRect rect;
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
      rect.ਸਿਰਾ_ਪੰਕਤੀ   = state->pos.ਪੰਕਤੀ + 1;
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ;
      rect.ਸਿਰਾ_ਖਾਨਾ   = THISROWWIDTH(state);
      scroll(state, rect, 0, -1);
    }

    putglyph(state, chars, ਚੌੜਾਈ, state->pos);

    ਜੇ(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      ਪੂਰਨ save_i;
      ਲਈ(save_i = 0; chars[save_i]; save_i++) {
        ਜੇ(save_i >= (ਪੂਰਨ)state->combine_chars_ਮਾਪ)
          grow_combine_ਬਫਰ(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      ਜੇ(save_i >= (ਪੂਰਨ)state->combine_chars_ਮਾਪ)
        grow_combine_ਬਫਰ(state);
      state->combine_chars[save_i] = 0;
      state->combine_ਚੌੜਾਈ = ਚੌੜਾਈ;
      state->combine_pos = state->pos;
    }

    ਜੇ(state->pos.ਖਾਨਾ + ਚੌੜਾਈ >= THISROWWIDTH(state)) {
      ਜੇ(state->ਬਹੁਲਕ.autowrap)
        state->at_phantom = 1;
    }
    ਨਹੀਂਤਾਂ {
      state->pos.ਖਾਨਾ += ਚੌੜਾਈ;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
  ਜੇ(state->pos.ਪੰਕਤੀ < 0 || state->pos.ਪੰਕਤੀ >= state->ਪੰਕਤੀਆਂ_2 ||
     state->pos.ਖਾਨਾ < 0 || state->pos.ਖਾਨਾ >= state->ਖਾਨੇ_ਗਿਣਤੀ) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.ਪੰਕਤੀ, state->pos.ਖਾਨਾ);
    abort();
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ (ਪੂਰਨ)eaten;
}

ਸਥਿਰ ਪੂਰਨ on_control(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ control, ਖਾਲੀ *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.ਖਾਨਾ -= (leadpos.ਖਾਨਾ >= 2 ? 2 : 0);

  ਚੋਣ(control) {
  ਹਾਲਤ 0x07: // BEL - ECMA-48 8.3.3
    ਜੇ(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    ਰੋਕੋ;

  ਹਾਲਤ 0x08: // BS - ECMA-48 8.3.5
    ਜੇ(state->pos.ਖਾਨਾ > 0)
      state->pos.ਖਾਨਾ--;
    ਜੇ(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      ਜੇ(vterm_unicode_ਚੌੜਾਈ(cell.chars[0]) == 2)
        state->pos.ਖਾਨਾ--;
    }
    ਰੋਕੋ;

  ਹਾਲਤ 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    ਰੋਕੋ;

  ਹਾਲਤ 0x0a: // LF - ECMA-48 8.3.74
  ਹਾਲਤ 0x0b: // VT
  ਹਾਲਤ 0x0c: // FF
    linefeed(state);
    ਜੇ(state->ਬਹੁਲਕ.newline)
      state->pos.ਖਾਨਾ = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x0d: // CR - ECMA-48 8.3.15
    state->pos.ਖਾਨਾ = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_ਸੈੱਟ = 1;
    ਰੋਕੋ;

  ਹਾਲਤ 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_ਸੈੱਟ = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    ਰੋਕੋ;

  ਹਾਲਤ 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.ਖਾਨਾ = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x88: // HTS - ECMA-48 8.3.62
    ਸੈੱਟ_ਖਾਨਾ_tabstop(state, state->pos.ਖਾਨਾ);
    ਰੋਕੋ;

  ਹਾਲਤ 0x8d: // RI - ECMA-48 8.3.104
    ਜੇ(state->pos.ਪੰਕਤੀ == state->scrollregion_ਸਿਖਰ) {
      VTermRect rect;
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->scrollregion_ਸਿਖਰ;
      rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = SCROLLREGION_ਖੱਬਾ(state);
      rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

      scroll(state, rect, -1, 0);
    }
    ਨਹੀਂਤਾਂ ਜੇ(state->pos.ਪੰਕਤੀ > 0)
        state->pos.ਪੰਕਤੀ--;
    ਰੋਕੋ;

  ਹਾਲਤ 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_ਸੈੱਟ = 2;
    ਰੋਕੋ;

  ਹਾਲਤ 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_ਸੈੱਟ = 3;
    ਰੋਕੋ;

  ਮੂਲ:
    ਜੇ(state->fallbacks && state->fallbacks->control)
      ਜੇ((*state->fallbacks->control)(control, state->fbdata))
        ਵਾਪਸ 1;

    ਵਾਪਸ 0;
  }

  updatecursor(state, &oldpos, 1);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
  ਜੇ(state->pos.ਪੰਕਤੀ < 0 || state->pos.ਪੰਕਤੀ >= state->ਪੰਕਤੀਆਂ_2 ||
     state->pos.ਖਾਨਾ < 0 || state->pos.ਖਾਨਾ >= state->ਖਾਨੇ_ਗਿਣਤੀ) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.ਪੰਕਤੀ, state->pos.ਖਾਨਾ);
    abort();
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਪੂਰਨ settermprop_bool(VTermState *state, VTermProp prop, ਪੂਰਨ v)
{
  VTerm_ਮੁੱਲ val;
  val.boolean = v;
  ਵਾਪਸ vterm_state_ਸੈੱਟ_termprop(state, prop, &val);
}

ਸਥਿਰ ਪੂਰਨ settermprop_int(VTermState *state, VTermProp prop, ਪੂਰਨ v)
{
  VTerm_ਮੁੱਲ val;
  val.ਸੰਖਿਆ = v;
  ਵਾਪਸ vterm_state_ਸੈੱਟ_termprop(state, prop, &val);
}

ਸਥਿਰ ਪੂਰਨ settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_ਮੁੱਲ val;

  val.string = frag;
  ਵਾਪਸ vterm_state_ਸੈੱਟ_termprop(state, prop, &val);
}

ਸਥਿਰ ਖਾਲੀ savecursor(VTermState *state, ਪੂਰਨ save)
{
  ਜੇ(save) {
    state->saved.pos = state->pos;
    state->saved.ਬਹੁਲਕ.cursor_visible = state->ਬਹੁਲਕ.cursor_visible;
    state->saved.ਬਹੁਲਕ.cursor_blink   = state->ਬਹੁਲਕ.cursor_blink;
    state->saved.ਬਹੁਲਕ.cursor_shape   = state->ਬਹੁਲਕ.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  ਨਹੀਂਤਾਂ {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.ਬਹੁਲਕ.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.ਬਹੁਲਕ.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.ਬਹੁਲਕ.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ਸਥਿਰ ਪੂਰਨ on_escape(ਅਚਲ ਅੱਖਰ *ਬਾਈਟਾਂ, size_t len, ਖਾਲੀ *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  ਚੋਣ(ਬਾਈਟਾਂ[0]) {
  ਹਾਲਤ ' ':
    ਜੇ(len != 2)
      ਵਾਪਸ 0;

    ਚੋਣ(ਬਾਈਟਾਂ[1]) {
      ਹਾਲਤ 'F': // S7C1T
        state->vt->ਬਹੁਲਕ.ctrl8bit = 0;
        ਰੋਕੋ;

      ਹਾਲਤ 'G': // S8C1T
        state->vt->ਬਹੁਲਕ.ctrl8bit = 1;
        ਰੋਕੋ;

      ਮੂਲ:
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ 2;

  ਹਾਲਤ '#':
    ਜੇ(len != 2)
      ਵਾਪਸ 0;

    ਚੋਣ(ਬਾਈਟਾਂ[1]) {
      ਹਾਲਤ '3': // DECDHL top
        ਜੇ(state->ਬਹੁਲਕ.leftrightmargin)
          ਰੋਕੋ;
        ਸੈੱਟ_lineinfo(state, state->pos.ਪੰਕਤੀ, NO_FORCE, DWL_ON, DHL_ਸਿਖਰ);
        ਰੋਕੋ;

      ਹਾਲਤ '4': // DECDHL bottom
        ਜੇ(state->ਬਹੁਲਕ.leftrightmargin)
          ਰੋਕੋ;
        ਸੈੱਟ_lineinfo(state, state->pos.ਪੰਕਤੀ, NO_FORCE, DWL_ON, DHL_BOTTOM);
        ਰੋਕੋ;

      ਹਾਲਤ '5': // DECSWL
        ਜੇ(state->ਬਹੁਲਕ.leftrightmargin)
          ਰੋਕੋ;
        ਸੈੱਟ_lineinfo(state, state->pos.ਪੰਕਤੀ, NO_FORCE, DWL_OFF, DHL_OFF);
        ਰੋਕੋ;

      ਹਾਲਤ '6': // DECDWL
        ਜੇ(state->ਬਹੁਲਕ.leftrightmargin)
          ਰੋਕੋ;
        ਸੈੱਟ_lineinfo(state, state->pos.ਪੰਕਤੀ, NO_FORCE, DWL_ON, DHL_OFF);
        ਰੋਕੋ;

      ਹਾਲਤ '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        ਲਈ(pos.ਪੰਕਤੀ = 0; pos.ਪੰਕਤੀ < state->ਪੰਕਤੀਆਂ_2; pos.ਪੰਕਤੀ++)
          ਲਈ(pos.ਖਾਨਾ = 0; pos.ਖਾਨਾ < ROWWIDTH(state, pos.ਪੰਕਤੀ); pos.ਖਾਨਾ++)
            putglyph(state, E, 1, pos);
        ਰੋਕੋ;
      }

      ਮੂਲ:
        ਵਾਪਸ 0;
    }
    ਵਾਪਸ 2;

  ਹਾਲਤ '(': ਹਾਲਤ ')': ਹਾਲਤ '*': ਹਾਲਤ '+': // SCS
    ਜੇ(len != 2)
      ਵਾਪਸ 0;

    {
      ਪੂਰਨ setnum = ਬਾਈਟਾਂ[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, ਬਾਈਟਾਂ[1]);

      ਜੇ(newenc) {
        state->encoding[setnum].enc = newenc;

        ਜੇ(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].ਅੰਕੜੇ);
      }
    }

    ਵਾਪਸ 2;

  ਹਾਲਤ '7': // DECSC
    savecursor(state, 1);
    ਵਾਪਸ 1;

  ਹਾਲਤ '8': // DECRC
    savecursor(state, 0);
    ਵਾਪਸ 1;

  ਹਾਲਤ '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    ਵਾਪਸ 1;

  ਹਾਲਤ '=': // DECKPAM
    state->ਬਹੁਲਕ.keypad = 1;
    ਵਾਪਸ 1;

  ਹਾਲਤ '>': // DECKPNM
    state->ਬਹੁਲਕ.keypad = 0;
    ਵਾਪਸ 1;

  ਹਾਲਤ 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    ਜੇ(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->ਬਹੁਲਕ.cursor_visible, state->cbdata);
    ਵਾਪਸ 1;
  }

  ਹਾਲਤ 'n': // LS2 - ECMA-48 8.3.78
    state->gl_ਸੈੱਟ = 2;
    ਵਾਪਸ 1;

  ਹਾਲਤ 'o': // LS3 - ECMA-48 8.3.80
    state->gl_ਸੈੱਟ = 3;
    ਵਾਪਸ 1;

  ਹਾਲਤ '~': // LS1R - ECMA-48 8.3.77
    state->gr_ਸੈੱਟ = 1;
    ਵਾਪਸ 1;

  ਹਾਲਤ '}': // LS2R - ECMA-48 8.3.79
    state->gr_ਸੈੱਟ = 2;
    ਵਾਪਸ 1;

  ਹਾਲਤ '|': // LS3R - ECMA-48 8.3.81
    state->gr_ਸੈੱਟ = 3;
    ਵਾਪਸ 1;

  ਮੂਲ:
    ਵਾਪਸ 0;
  }
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_ਬਹੁਲਕ(VTermState *state, ਪੂਰਨ ਅੰਕ, ਪੂਰਨ val)
{
  ਚੋਣ(ਅੰਕ) {
  ਹਾਲਤ 4: // IRM - ECMA-48 7.2.10
    state->ਬਹੁਲਕ.ਪਾਓ = val;
    ਰੋਕੋ;

  ਹਾਲਤ 20: // LNM - ANSI X3.4-1977
    state->ਬਹੁਲਕ.newline = val;
    ਰੋਕੋ;

  ਮੂਲ:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", ਅੰਕ);
    ਵਾਪਸ;
  }
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_dec_ਬਹੁਲਕ(VTermState *state, ਪੂਰਨ ਅੰਕ, ਪੂਰਨ val)
{
  ਚੋਣ(ਅੰਕ) {
  ਹਾਲਤ 1:
    state->ਬਹੁਲਕ.cursor = val;
    ਰੋਕੋ;

  ਹਾਲਤ 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_ਉਲਟ, val);
    ਰੋਕੋ;

  ਹਾਲਤ 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->ਬਹੁਲਕ.origin = val;
      state->pos.ਪੰਕਤੀ = state->ਬਹੁਲਕ.origin ? state->scrollregion_ਸਿਖਰ : 0;
      state->pos.ਖਾਨਾ = state->ਬਹੁਲਕ.origin ? SCROLLREGION_ਖੱਬਾ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    ਰੋਕੋ;

  ਹਾਲਤ 7:
    state->ਬਹੁਲਕ.autowrap = val;
    ਰੋਕੋ;

  ਹਾਲਤ 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    ਰੋਕੋ;

  ਹਾਲਤ 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    ਰੋਕੋ;

  ਹਾਲਤ 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->ਬਹੁਲਕ.leftrightmargin = val;
    ਜੇ(val) {
      ਪੂਰਨ ਪੰਕਤੀ;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      ਲਈ(ਪੰਕਤੀ = 0; ਪੰਕਤੀ < state->ਪੰਕਤੀਆਂ_2; ਪੰਕਤੀ++)
        ਸੈੱਟ_lineinfo(state, ਪੰਕਤੀ, FORCE, DWL_OFF, DHL_OFF);
    }

    ਰੋਕੋ;

  ਹਾਲਤ 1000:
  ਹਾਲਤ 1002:
  ਹਾਲਤ 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (ਅੰਕ == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (ਅੰਕ == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    ਰੋਕੋ;

  ਹਾਲਤ 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->ਬਹੁਲਕ.report_focus = val;
    ਰੋਕੋ;

  ਹਾਲਤ 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    ਰੋਕੋ;

  ਹਾਲਤ 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    ਰੋਕੋ;

  ਹਾਲਤ 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    ਰੋਕੋ;

  ਹਾਲਤ 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    ਰੋਕੋ;

  ਹਾਲਤ 1048:
    savecursor(state, val);
    ਰੋਕੋ;

  ਹਾਲਤ 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    ਰੋਕੋ;

  ਹਾਲਤ 2004:
    state->ਬਹੁਲਕ.bracketpaste = val;
    ਰੋਕੋ;

  ਮੂਲ:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", ਅੰਕ);
    ਵਾਪਸ;
  }
}

ਸਥਿਰ ਖਾਲੀ request_dec_ਬਹੁਲਕ(VTermState *state, ਪੂਰਨ ਅੰਕ)
{
  ਪੂਰਨ ਜਵਾਬ;

  ਚੋਣ(ਅੰਕ) {
    ਹਾਲਤ 1:
      ਜਵਾਬ = state->ਬਹੁਲਕ.cursor;
      ਰੋਕੋ;

    ਹਾਲਤ 5:
      ਜਵਾਬ = state->ਬਹੁਲਕ.screen;
      ਰੋਕੋ;

    ਹਾਲਤ 6:
      ਜਵਾਬ = state->ਬਹੁਲਕ.origin;
      ਰੋਕੋ;

    ਹਾਲਤ 7:
      ਜਵਾਬ = state->ਬਹੁਲਕ.autowrap;
      ਰੋਕੋ;

    ਹਾਲਤ 12:
      ਜਵਾਬ = state->ਬਹੁਲਕ.cursor_blink;
      ਰੋਕੋ;

    ਹਾਲਤ 25:
      ਜਵਾਬ = state->ਬਹੁਲਕ.cursor_visible;
      ਰੋਕੋ;

    ਹਾਲਤ 69:
      ਜਵਾਬ = state->ਬਹੁਲਕ.leftrightmargin;
      ਰੋਕੋ;

    ਹਾਲਤ 1000:
      ਜਵਾਬ = state->mouse_flags == MOUSE_WANT_CLICK;
      ਰੋਕੋ;

    ਹਾਲਤ 1002:
      ਜਵਾਬ = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      ਰੋਕੋ;

    ਹਾਲਤ 1003:
      ਜਵਾਬ = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      ਰੋਕੋ;

    ਹਾਲਤ 1004:
      ਜਵਾਬ = state->ਬਹੁਲਕ.report_focus;
      ਰੋਕੋ;

    ਹਾਲਤ 1005:
      ਜਵਾਬ = state->mouse_protocol == MOUSE_UTF8;
      ਰੋਕੋ;

    ਹਾਲਤ 1006:
      ਜਵਾਬ = state->mouse_protocol == MOUSE_SGR;
      ਰੋਕੋ;

    ਹਾਲਤ 1015:
      ਜਵਾਬ = state->mouse_protocol == MOUSE_RXVT;
      ਰੋਕੋ;

    ਹਾਲਤ 1047:
      ਜਵਾਬ = state->ਬਹੁਲਕ.alt_screen;
      ਰੋਕੋ;

    ਹਾਲਤ 2004:
      ਜਵਾਬ = state->ਬਹੁਲਕ.bracketpaste;
      ਰੋਕੋ;

    ਮੂਲ:
      vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ਅੰਕ, 0);
      ਵਾਪਸ;
  }

  vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ਅੰਕ, ਜਵਾਬ ? 1 : 2);
}

ਸਥਿਰ ਖਾਲੀ request_ਸੰਸਕਰਣ_string(VTermState *state)
{
  vterm_ਧੱਕੋ_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_ਸੰਸਕਰਣ_MAJOR, VTERM_ਸੰਸਕਰਣ_MINOR);
}

ਸਥਿਰ ਪੂਰਨ on_csi(ਅਚਲ ਅੱਖਰ *leader, ਅਚਲ ਲੰਮਾ ਦਲੀਲਾਂ[], ਪੂਰਨ argcount, ਅਚਲ ਅੱਖਰ *intermed, ਅੱਖਰ command, ਖਾਲੀ *user)
{
  VTermState *state = user;
  ਪੂਰਨ leader_byte = 0;
  ਪੂਰਨ intermed_byte = 0;
  ਪੂਰਨ cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  ਪੂਰਨ handled = 1;

  // Some temporaries for later code
  ਪੂਰਨ ਗਿਣਤੀ_2, val;
  ਪੂਰਨ ਪੰਕਤੀ, ਖਾਨਾ;
  VTermRect rect;
  ਪੂਰਨ selective;

  ਜੇ(leader && leader[0]) {
    ਜੇ(leader[1]) // longer than 1 char
      ਵਾਪਸ 0;

    ਚੋਣ(leader[0]) {
    ਹਾਲਤ '?':
    ਹਾਲਤ '>':
    ਹਾਲਤ '<':
      leader_byte = leader[0];
      ਰੋਕੋ;
    ਮੂਲ:
      ਵਾਪਸ 0;
    }
  }

  ਜੇ(intermed && intermed[0]) {
    ਜੇ(intermed[1]) // longer than 1 char
      ਵਾਪਸ 0;

    ਚੋਣ(intermed[0]) {
    ਹਾਲਤ ' ':
    ਹਾਲਤ '!':
    ਹਾਲਤ '"':
    ਹਾਲਤ '$':
    ਹਾਲਤ '\'':
      intermed_byte = intermed[0];
      ਰੋਕੋ;
    ਮੂਲ:
      ਵਾਪਸ 0;
    }
  }

  oldpos = state->pos;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LBOUND(v,min) ਜੇ((v) < (min)) (v) = (min)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UBOUND(v,max) ਜੇ((v) > (max)) (v) = (max)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LEADER(l,b) ((l << 8) | b)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INTERMED(i,b) ((i << 16) | b)

  ਚੋਣ(intermed_byte << 16 | leader_byte << 8 | command) {
  ਹਾਲਤ 0x40: // ICH - ECMA-48 8.3.64
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    ਜੇ(!is_cursor_ਵਿੱਚ_scrollregion(state))
      ਰੋਕੋ;

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = state->pos.ਪੰਕਤੀ + 1;
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ;
    ਜੇ(state->ਬਹੁਲਕ.leftrightmargin)
      rect.ਸਿਰਾ_ਖਾਨਾ = SCROLLREGION_ਸੱਜਾ(state);
    ਨਹੀਂਤਾਂ
      rect.ਸਿਰਾ_ਖਾਨਾ = THISROWWIDTH(state);

    scroll(state, rect, 0, -ਗਿਣਤੀ_2);

    ਰੋਕੋ;

  ਹਾਲਤ 0x41: // CUU - ECMA-48 8.3.22
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਪੰਕਤੀ -= ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x42: // CUD - ECMA-48 8.3.19
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਪੰਕਤੀ += ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x43: // CUF - ECMA-48 8.3.20
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਖਾਨਾ += ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x44: // CUB - ECMA-48 8.3.18
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਖਾਨਾ -= ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x45: // CNL - ECMA-48 8.3.12
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਖਾਨਾ = 0;
    state->pos.ਪੰਕਤੀ += ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x46: // CPL - ECMA-48 8.3.13
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਖਾਨਾ = 0;
    state->pos.ਪੰਕਤੀ -= ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1);
    state->pos.ਖਾਨਾ = val-1;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x48: // CUP - ECMA-48 8.3.21
    ਪੰਕਤੀ = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1);
    ਖਾਨਾ = argcount < 2 || CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[1]) ? 1 : CSI_ARG(ਦਲੀਲਾਂ[1]);
    // zero-based
    ਜੇ(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      ਜੇ(state->pos.ਪੰਕਤੀ == ਪੰਕਤੀ - 1) {
        ਪੂਰਨ cnt, ptr = 0;
        ਲਈ(cnt = 0; cnt < ਖਾਨਾ - 1; ++cnt) {
	  VTermPos p;
	  VTermScreenCell c0, c1;
	  p.ਪੰਕਤੀ = ਪੰਕਤੀ - 1;
	  p.ਖਾਨਾ = ptr;
	  vterm_screen_get_cell(state->vt->screen, p, &c0);
	  p.ਖਾਨਾ++;
	  vterm_screen_get_cell(state->vt->screen, p, &c1);
	  ptr += (c1.chars[0] == (uint32_t)-1)		    // double cell?
	     ? (vterm_unicode_is_ambiguous(c0.chars[0]))    // is ambiguous?
	     ? vterm_unicode_ਚੌੜਾਈ(0x00a1) : 1		    // &ambiwidth
	     : 1;					    // not ambiguous
        }
        ਖਾਨਾ = ptr + 1;
      }
    }
    state->pos.ਪੰਕਤੀ = ਪੰਕਤੀ-1;
    state->pos.ਖਾਨਾ = ਖਾਨਾ-1;
    ਜੇ(state->ਬਹੁਲਕ.origin) {
      state->pos.ਪੰਕਤੀ += state->scrollregion_ਸਿਖਰ;
      state->pos.ਖਾਨਾ += SCROLLREGION_ਖੱਬਾ(state);
    }
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x49: // CHT - ECMA-48 8.3.10
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    tab(state, ਗਿਣਤੀ_2, +1);
    ਰੋਕੋ;

  ਹਾਲਤ 0x4a: // ED - ECMA-48 8.3.39
  ਹਾਲਤ LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    ਚੋਣ(CSI_ARG(ਦਲੀਲਾਂ[0])) {
    ਹਾਲਤ CSI_ARG_MISSING:
    ਹਾਲਤ 0:
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ; rect.ਸਿਰਾ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ + 1;
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ; rect.ਸਿਰਾ_ਖਾਨਾ = state->ਖਾਨੇ_ਗਿਣਤੀ;
      ਜੇ(rect.ਸਿਰਾ_ਖਾਨਾ > rect.ਸ਼ੁਰੂ_ਖਾਨਾ)
        erase(state, rect, selective);

      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ + 1; rect.ਸਿਰਾ_ਪੰਕਤੀ = state->ਪੰਕਤੀਆਂ_2;
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0;
      ਲਈ(ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++)
	ਸੈੱਟ_lineinfo(state, ਪੰਕਤੀ, FORCE, DWL_OFF, DHL_OFF);
      ਜੇ(rect.ਸਿਰਾ_ਪੰਕਤੀ > rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ)
        erase(state, rect, selective);
      ਰੋਕੋ;

    ਹਾਲਤ 1:
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = 0; rect.ਸਿਰਾ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0; rect.ਸਿਰਾ_ਖਾਨਾ = state->ਖਾਨੇ_ਗਿਣਤੀ;
      ਲਈ(ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++)
	ਸੈੱਟ_lineinfo(state, ਪੰਕਤੀ, FORCE, DWL_OFF, DHL_OFF);
      ਜੇ(rect.ਸਿਰਾ_ਖਾਨਾ > rect.ਸ਼ੁਰੂ_ਖਾਨਾ)
        erase(state, rect, selective);

      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ; rect.ਸਿਰਾ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ + 1;
                          rect.ਸਿਰਾ_ਖਾਨਾ = state->pos.ਖਾਨਾ + 1;
      ਜੇ(rect.ਸਿਰਾ_ਪੰਕਤੀ > rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ)
        erase(state, rect, selective);
      ਰੋਕੋ;

    ਹਾਲਤ 2:
      rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = 0; rect.ਸਿਰਾ_ਪੰਕਤੀ = state->ਪੰਕਤੀਆਂ_2;
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0; rect.ਸਿਰਾ_ਖਾਨਾ = state->ਖਾਨੇ_ਗਿਣਤੀ;
      ਲਈ(ਪੰਕਤੀ = rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ; ਪੰਕਤੀ < rect.ਸਿਰਾ_ਪੰਕਤੀ; ਪੰਕਤੀ++)
	ਸੈੱਟ_lineinfo(state, ਪੰਕਤੀ, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      ਰੋਕੋ;

    ਹਾਲਤ 3:
      ਜੇ(state->callbacks && state->callbacks->sb_clear)
        ਜੇ((*state->callbacks->sb_clear)(state->cbdata))
          ਵਾਪਸ 1;
      ਰੋਕੋ;
    }
    ਰੋਕੋ;

  ਹਾਲਤ 0x4b: // EL - ECMA-48 8.3.41
  ਹਾਲਤ LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = state->pos.ਪੰਕਤੀ + 1;

    ਚੋਣ(CSI_ARG(ਦਲੀਲਾਂ[0])) {
    ਹਾਲਤ CSI_ARG_MISSING:
    ਹਾਲਤ 0:
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ; rect.ਸਿਰਾ_ਖਾਨਾ = THISROWWIDTH(state); ਰੋਕੋ;
    ਹਾਲਤ 1:
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0; rect.ਸਿਰਾ_ਖਾਨਾ = state->pos.ਖਾਨਾ + 1; ਰੋਕੋ;
    ਹਾਲਤ 2:
      rect.ਸ਼ੁਰੂ_ਖਾਨਾ = 0; rect.ਸਿਰਾ_ਖਾਨਾ = THISROWWIDTH(state); ਰੋਕੋ;
    ਮੂਲ:
      ਵਾਪਸ 0;
    }

    ਜੇ(rect.ਸਿਰਾ_ਖਾਨਾ > rect.ਸ਼ੁਰੂ_ਖਾਨਾ)
      erase(state, rect, selective);

    ਰੋਕੋ;

  ਹਾਲਤ 0x4c: // IL - ECMA-48 8.3.67
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    ਜੇ(!is_cursor_ਵਿੱਚ_scrollregion(state))
      ਰੋਕੋ;

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = SCROLLREGION_ਖੱਬਾ(state);
    rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

    scroll(state, rect, -ਗਿਣਤੀ_2, 0);

    ਰੋਕੋ;

  ਹਾਲਤ 0x4d: // DL - ECMA-48 8.3.32
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    ਜੇ(!is_cursor_ਵਿੱਚ_scrollregion(state))
      ਰੋਕੋ;

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = SCROLLREGION_ਖੱਬਾ(state);
    rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

    scroll(state, rect, ਗਿਣਤੀ_2, 0);

    ਰੋਕੋ;

  ਹਾਲਤ 0x50: // DCH - ECMA-48 8.3.26
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    ਜੇ(!is_cursor_ਵਿੱਚ_scrollregion(state))
      ਰੋਕੋ;

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = state->pos.ਪੰਕਤੀ + 1;
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ;
    ਜੇ(state->ਬਹੁਲਕ.leftrightmargin)
      rect.ਸਿਰਾ_ਖਾਨਾ = SCROLLREGION_ਸੱਜਾ(state);
    ਨਹੀਂਤਾਂ
      rect.ਸਿਰਾ_ਖਾਨਾ = THISROWWIDTH(state);

    scroll(state, rect, 0, ਗਿਣਤੀ_2);

    ਰੋਕੋ;

  ਹਾਲਤ 0x53: // SU - ECMA-48 8.3.147
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->scrollregion_ਸਿਖਰ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = SCROLLREGION_ਖੱਬਾ(state);
    rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

    scroll(state, rect, ਗਿਣਤੀ_2, 0);

    ਰੋਕੋ;

  ਹਾਲਤ 0x54: // SD - ECMA-48 8.3.113
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->scrollregion_ਸਿਖਰ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = SCROLLREGION_ਖੱਬਾ(state);
    rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

    scroll(state, rect, -ਗਿਣਤੀ_2, 0);

    ਰੋਕੋ;

  ਹਾਲਤ 0x58: // ECH - ECMA-48 8.3.38
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->pos.ਪੰਕਤੀ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = state->pos.ਪੰਕਤੀ + 1;
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ;
    rect.ਸਿਰਾ_ਖਾਨਾ   = state->pos.ਖਾਨਾ + ਗਿਣਤੀ_2;
    UBOUND(rect.ਸਿਰਾ_ਖਾਨਾ, THISROWWIDTH(state));

    erase(state, rect, 0);
    ਰੋਕੋ;

  ਹਾਲਤ 0x5a: // CBT - ECMA-48 8.3.7
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    tab(state, ਗਿਣਤੀ_2, -1);
    ਰੋਕੋ;

  ਹਾਲਤ 0x60: // HPA - ECMA-48 8.3.57
    ਖਾਨਾ = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1);
    state->pos.ਖਾਨਾ = ਖਾਨਾ-1;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x61: // HPR - ECMA-48 8.3.59
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਖਾਨਾ += ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x62: { // REP - ECMA-48 8.3.103
    ਅਚਲ ਪੂਰਨ ਪੰਕਤੀ_ਚੌੜਾਈ = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    ਜੇ(state->combine_ਚੌੜਾਈ < 1)
      ਰੋਕੋ;

    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    ਖਾਨਾ = state->pos.ਖਾਨਾ + ਗਿਣਤੀ_2;
    UBOUND(ਖਾਨਾ, ਪੰਕਤੀ_ਚੌੜਾਈ);
    ਜਦੋਂ (state->pos.ਖਾਨਾ < ਖਾਨਾ) {
      putglyph(state, state->combine_chars, state->combine_ਚੌੜਾਈ, state->pos);
      state->pos.ਖਾਨਾ += state->combine_ਚੌੜਾਈ;
    }
    ਜੇ (state->pos.ਖਾਨਾ + state->combine_ਚੌੜਾਈ >= ਪੰਕਤੀ_ਚੌੜਾਈ) {
      ਜੇ (state->ਬਹੁਲਕ.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    ਰੋਕੋ;
  }

  ਹਾਲਤ 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 0);
    ਜੇ(val == 0)
      // DEC VT100 response
      vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    ਰੋਕੋ;

  ਹਾਲਤ 0x64: // VPA - ECMA-48 8.3.158
    ਪੰਕਤੀ = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1);
    state->pos.ਪੰਕਤੀ = ਪੰਕਤੀ-1;
    ਜੇ(state->ਬਹੁਲਕ.origin)
      state->pos.ਪੰਕਤੀ += state->scrollregion_ਸਿਖਰ;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x65: // VPR - ECMA-48 8.3.160
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਪੰਕਤੀ += ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x66: // HVP - ECMA-48 8.3.63
    ਪੰਕਤੀ = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1);
    ਖਾਨਾ = argcount < 2 || CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[1]) ? 1 : CSI_ARG(ਦਲੀਲਾਂ[1]);
    // zero-based
    state->pos.ਪੰਕਤੀ = ਪੰਕਤੀ-1;
    state->pos.ਖਾਨਾ = ਖਾਨਾ-1;
    ਜੇ(state->ਬਹੁਲਕ.origin) {
      state->pos.ਪੰਕਤੀ += state->scrollregion_ਸਿਖਰ;
      state->pos.ਖਾਨਾ += SCROLLREGION_ਖੱਬਾ(state);
    }
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 0);

    ਚੋਣ(val) {
    ਹਾਲਤ 0:
      clear_ਖਾਨਾ_tabstop(state, state->pos.ਖਾਨਾ);
      ਰੋਕੋ;
    ਹਾਲਤ 3:
    ਹਾਲਤ 5:
      ਲਈ(ਖਾਨਾ = 0; ਖਾਨਾ < state->ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++)
        clear_ਖਾਨਾ_tabstop(state, ਖਾਨਾ);
      ਰੋਕੋ;
    ਹਾਲਤ 1:
    ਹਾਲਤ 2:
    ਹਾਲਤ 4:
      ਰੋਕੋ;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    ਮੂਲ:
      ਵਾਪਸ 0;
    }
    ਰੋਕੋ;

  ਹਾਲਤ 0x68: // SM - ECMA-48 8.3.125
    ਜੇ(!CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[0]))
      ਸੈੱਟ_ਬਹੁਲਕ(state, CSI_ARG(ਦਲੀਲਾਂ[0]), 1);
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('?', 0x68): // DEC private mode set
    ਲਈ(ਪੂਰਨ i = 0; i < argcount; i++) {
      ਜੇ(!CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[i]))
        ਸੈੱਟ_dec_ਬਹੁਲਕ(state, CSI_ARG(ਦਲੀਲਾਂ[i]), 1);
    }
    ਰੋਕੋ;

  ਹਾਲਤ 0x6a: // HPB - ECMA-48 8.3.58
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਖਾਨਾ -= ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x6b: // VPB - ECMA-48 8.3.159
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);
    state->pos.ਪੰਕਤੀ -= ਗਿਣਤੀ_2;
    state->at_phantom = 0;
    ਰੋਕੋ;

  ਹਾਲਤ 0x6c: // RM - ECMA-48 8.3.106
    ਜੇ(!CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[0]))
      ਸੈੱਟ_ਬਹੁਲਕ(state, CSI_ARG(ਦਲੀਲਾਂ[0]), 0);
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('?', 0x6c): // DEC private mode reset
    ਲਈ(ਪੂਰਨ i = 0; i < argcount; i++) {
      ਜੇ(!CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[i]))
        ਸੈੱਟ_dec_ਬਹੁਲਕ(state, CSI_ARG(ਦਲੀਲਾਂ[i]), 0);
    }
    ਰੋਕੋ;

  ਹਾਲਤ 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, ਦਲੀਲਾਂ, argcount);
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    ਜੇ (argcount == 1 && CSI_ARG(ਦਲੀਲਾਂ[0]) == 4)
    {
      vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->ਬਹੁਲਕ.modify_other_ਕੁੰਜੀਆਂ ? 2 : 0);
      ਰੋਕੋ;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    ਲਈ(ਪੂਰਨ argi = 0; argi < argcount; argi++) {
      ਲੰਮਾ arg;
      ਚੋਣ(arg = CSI_ARG(ਦਲੀਲਾਂ[argi])) {
        ਹਾਲਤ 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          ਰੋਕੋ;
        ਹਾਲਤ 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          ਰੋਕੋ;
        ਹਾਲਤ 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          ਰੋਕੋ;
      }
    }
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    ਜੇ (argcount == 2 && CSI_ARG(ਦਲੀਲਾਂ[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->ਬਹੁਲਕ.kitty_keyboard = 0;

      state->ਬਹੁਲਕ.modify_other_ਕੁੰਜੀਆਂ = CSI_ARG(ਦਲੀਲਾਂ[1]) == 2;
    }
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    ਜੇ (argcount == 1 && CSI_ARG(ਦਲੀਲਾਂ[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->ਬਹੁਲਕ.modify_other_ਕੁੰਜੀਆਂ = 0;

      state->ਬਹੁਲਕ.kitty_keyboard = 1;
    }
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    ਜੇ (argcount <= 1)
      state->ਬਹੁਲਕ.kitty_keyboard = 0;
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    ਜੇ (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->ਬਹੁਲਕ.kitty_keyboard);
    ਰੋਕੋ;

  ਹਾਲਤ 0x6e: // DSR - ECMA-48 8.3.35
  ਹਾਲਤ LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 0);

    {
      ਅੱਖਰ *qmark = (leader_byte == '?') ? "?" : "";

      ਚੋਣ(val) {
      ਹਾਲਤ 0: ਹਾਲਤ 1: ਹਾਲਤ 2: ਹਾਲਤ 3: ਹਾਲਤ 4:
        // ignore - these are replies
        ਰੋਕੋ;
      ਹਾਲਤ 5:
        vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        ਰੋਕੋ;
      ਹਾਲਤ 6: // CPR - cursor position report
        vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.ਪੰਕਤੀ + 1, state->pos.ਖਾਨਾ + 1);
        ਰੋਕੋ;
      }
    }
    ਰੋਕੋ;


  ਹਾਲਤ INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('?', INTERMED('$', 0x70)):
    request_dec_ਬਹੁਲਕ(state, CSI_ARG(ਦਲੀਲਾਂ[0]));
    ਰੋਕੋ;

  ਹਾਲਤ LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_ਸੰਸਕਰਣ_string(state);
    ਰੋਕੋ;

  ਹਾਲਤ INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1);

    ਚੋਣ(val) {
    ਹਾਲਤ 0: ਹਾਲਤ 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_ਖੰਡ);
      ਰੋਕੋ;
    ਹਾਲਤ 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_ਖੰਡ);
      ਰੋਕੋ;
    ਹਾਲਤ 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ਰੋਕੋ;
    ਹਾਲਤ 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ਰੋਕੋ;
    ਹਾਲਤ 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ਖੱਬਾ);
      ਰੋਕੋ;
    ਹਾਲਤ 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ਖੱਬਾ);
      ਰੋਕੋ;
    }

    ਰੋਕੋ;

  ਹਾਲਤ INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 0);

    ਚੋਣ(val) {
    ਹਾਲਤ 0: ਹਾਲਤ 2:
      state->protected_cell = 0;
      ਰੋਕੋ;
    ਹਾਲਤ 1:
      state->protected_cell = 1;
      ਰੋਕੋ;
    }

    ਰੋਕੋ;

  ਹਾਲਤ 0x72: // DECSTBM - DEC custom
    state->scrollregion_ਸਿਖਰ = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[1]) ? -1 : CSI_ARG(ਦਲੀਲਾਂ[1]);
    LBOUND(state->scrollregion_ਸਿਖਰ, 0);
    UBOUND(state->scrollregion_ਸਿਖਰ, state->ਪੰਕਤੀਆਂ_2);
    LBOUND(state->scrollregion_bottom, -1);
    ਜੇ(state->scrollregion_ਸਿਖਰ == 0 && state->scrollregion_bottom == state->ਪੰਕਤੀਆਂ_2)
      state->scrollregion_bottom = -1;
    ਨਹੀਂਤਾਂ
      UBOUND(state->scrollregion_bottom, state->ਪੰਕਤੀਆਂ_2);

    ਜੇ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ਸਿਖਰ) {
      // Invalid
      state->scrollregion_ਸਿਖਰ    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.ਪੰਕਤੀ = 0;
    state->pos.ਖਾਨਾ = 0;
    ਜੇ(state->ਬਹੁਲਕ.origin) {
      state->pos.ਪੰਕਤੀ += state->scrollregion_ਸਿਖਰ;
      state->pos.ਖਾਨਾ += SCROLLREGION_ਖੱਬਾ(state);
    }

    ਰੋਕੋ;

  ਹਾਲਤ 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_ਖੱਬਾ = CSI_ARG_OR(ਦਲੀਲਾਂ[0], 1) - 1;
    state->scrollregion_ਸੱਜਾ = argcount < 2 || CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[1]) ? -1 : CSI_ARG(ਦਲੀਲਾਂ[1]);
    LBOUND(state->scrollregion_ਖੱਬਾ, 0);
    UBOUND(state->scrollregion_ਖੱਬਾ, state->ਖਾਨੇ_ਗਿਣਤੀ);
    LBOUND(state->scrollregion_ਸੱਜਾ, -1);
    ਜੇ(state->scrollregion_ਖੱਬਾ == 0 && state->scrollregion_ਸੱਜਾ == state->ਖਾਨੇ_ਗਿਣਤੀ)
      state->scrollregion_ਸੱਜਾ = -1;
    ਨਹੀਂਤਾਂ
      UBOUND(state->scrollregion_ਸੱਜਾ, state->ਖਾਨੇ_ਗਿਣਤੀ);

    ਜੇ(state->scrollregion_ਸੱਜਾ > -1 &&
       state->scrollregion_ਸੱਜਾ <= state->scrollregion_ਖੱਬਾ) {
      // Invalid
      state->scrollregion_ਖੱਬਾ  = 0;
      state->scrollregion_ਸੱਜਾ = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.ਪੰਕਤੀ = 0;
    state->pos.ਖਾਨਾ = 0;
    ਜੇ(state->ਬਹੁਲਕ.origin) {
      state->pos.ਪੰਕਤੀ += state->scrollregion_ਸਿਖਰ;
      state->pos.ਖਾਨਾ += SCROLLREGION_ਖੱਬਾ(state);
    }

    ਰੋਕੋ;

  ਹਾਲਤ 0x74:
    ਚੋਣ(CSI_ARG(ਦਲੀਲਾਂ[0])) {
      ਹਾਲਤ 8: // CSI 8 ; rows ; cols t  set size
        ਜੇ (argcount == 3 &&
            !CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[1]) && !CSI_ARG_IS_MISSING(ਦਲੀਲਾਂ[2]) &&
            CSI_ARG(ਦਲੀਲਾਂ[1]) > 0 && CSI_ARG(ਦਲੀਲਾਂ[2]) > 0)
	  on_resize(CSI_ARG(ਦਲੀਲਾਂ[1]), CSI_ARG(ਦਲੀਲਾਂ[2]), state);
	ਰੋਕੋ;
      ਮੂਲ:
	handled = 0;
	ਰੋਕੋ;
    }
    ਰੋਕੋ;

  ਹਾਲਤ INTERMED('\'', 0x7D): // DECIC
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    ਜੇ(!is_cursor_ਵਿੱਚ_scrollregion(state))
      ਰੋਕੋ;

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->scrollregion_ਸਿਖਰ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ;
    rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

    scroll(state, rect, 0, -ਗਿਣਤੀ_2);

    ਰੋਕੋ;

  ਹਾਲਤ INTERMED('\'', 0x7E): // DECDC
    ਗਿਣਤੀ_2 = CSI_ARG_ਗਿਣਤੀ(ਦਲੀਲਾਂ[0]);

    ਜੇ(!is_cursor_ਵਿੱਚ_scrollregion(state))
      ਰੋਕੋ;

    rect.ਸ਼ੁਰੂ_ਪੰਕਤੀ = state->scrollregion_ਸਿਖਰ;
    rect.ਸਿਰਾ_ਪੰਕਤੀ   = SCROLLREGION_BOTTOM(state);
    rect.ਸ਼ੁਰੂ_ਖਾਨਾ = state->pos.ਖਾਨਾ;
    rect.ਸਿਰਾ_ਖਾਨਾ   = SCROLLREGION_ਸੱਜਾ(state);

    scroll(state, rect, 0, ਗਿਣਤੀ_2);

    ਰੋਕੋ;

  ਮੂਲ:
    handled = 0;
    ਰੋਕੋ;
  }

  ਜੇ (!handled) {
    ਜੇ(state->fallbacks && state->fallbacks->csi)
      ਜੇ((*state->fallbacks->csi)(leader, ਦਲੀਲਾਂ, argcount, intermed, command, state->fbdata))
        ਵਾਪਸ 1;

    ਵਾਪਸ 0;
  }

  ਜੇ(state->ਬਹੁਲਕ.origin) {
    LBOUND(state->pos.ਪੰਕਤੀ, state->scrollregion_ਸਿਖਰ);
    UBOUND(state->pos.ਪੰਕਤੀ, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.ਖਾਨਾ, SCROLLREGION_ਖੱਬਾ(state));
    UBOUND(state->pos.ਖਾਨਾ, SCROLLREGION_ਸੱਜਾ(state)-1);
  }
  ਨਹੀਂਤਾਂ {
    LBOUND(state->pos.ਪੰਕਤੀ, 0);
    UBOUND(state->pos.ਪੰਕਤੀ, state->ਪੰਕਤੀਆਂ_2-1);
    LBOUND(state->pos.ਖਾਨਾ, 0);
    UBOUND(state->pos.ਖਾਨਾ, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUG
  ਜੇ(state->pos.ਪੰਕਤੀ < 0 || state->pos.ਪੰਕਤੀ >= state->ਪੰਕਤੀਆਂ_2 ||
     state->pos.ਖਾਨਾ < 0 || state->pos.ਖਾਨਾ >= state->ਖਾਨੇ_ਗਿਣਤੀ) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.ਪੰਕਤੀ, state->pos.ਖਾਨਾ);
    abort();
  }

  ਜੇ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ਸਿਖਰ) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_ਸਿਖਰ);
    abort();
  }

  ਜੇ(SCROLLREGION_ਸੱਜਾ(state) <= SCROLLREGION_ਖੱਬਾ(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_ਸੱਜਾ(state), SCROLLREGION_ਖੱਬਾ(state));
    abort();
  }
#ਜੇ_ਅੰਤ

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਅੱਖਰ base64_one(uint8_t b)
{
  ਜੇ(b < 26)
    ਵਾਪਸ 'A' + b;
  ਨਹੀਂਤਾਂ ਜੇ(b < 52)
    ਵਾਪਸ 'a' + b - 26;
  ਨਹੀਂਤਾਂ ਜੇ(b < 62)
    ਵਾਪਸ '0' + b - 52;
  ਨਹੀਂਤਾਂ ਜੇ(b == 62)
    ਵਾਪਸ '+';
  ਨਹੀਂਤਾਂ ਜੇ(b == 63)
    ਵਾਪਸ '/';
  ਵਾਪਸ 0;
}

ਸਥਿਰ uint8_t unbase64one(ਅੱਖਰ c)
{
  ਜੇ(c >= 'A' && c <= 'Z')
    ਵਾਪਸ c - 'A';
  ਨਹੀਂਤਾਂ ਜੇ(c >= 'a' && c <= 'z')
    ਵਾਪਸ c - 'a' + 26;
  ਨਹੀਂਤਾਂ ਜੇ(c >= '0' && c <= '9')
    ਵਾਪਸ c - '0' + 52;
  ਨਹੀਂਤਾਂ ਜੇ(c == '+')
    ਵਾਪਸ 62;
  ਨਹੀਂਤਾਂ ਜੇ(c == '/')
    ਵਾਪਸ 63;

  ਵਾਪਸ 0xFF;
}

ਸਥਿਰ ਖਾਲੀ osc_selection(VTermState *state, VTermStringFragment frag)
{
  ਜੇ(frag.initial) {
    state->tmp.selection.ਨਕਾਬ = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  ਜਦੋਂ(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    ਚੋਣ(frag.str[0]) {
      ਹਾਲਤ 'c':
        state->tmp.selection.ਨਕਾਬ |= VTERM_SELECTION_CLIPBOARD;
        ਰੋਕੋ;
      ਹਾਲਤ 'p':
        state->tmp.selection.ਨਕਾਬ |= VTERM_SELECTION_PRIMARY;
        ਰੋਕੋ;
      ਹਾਲਤ 'q':
        state->tmp.selection.ਨਕਾਬ |= VTERM_SELECTION_SECONDARY;
        ਰੋਕੋ;
      ਹਾਲਤ 's':
        state->tmp.selection.ਨਕਾਬ |= VTERM_SELECTION_SELECT;
        ਰੋਕੋ;
      ਹਾਲਤ '0':
      ਹਾਲਤ '1':
      ਹਾਲਤ '2':
      ਹਾਲਤ '3':
      ਹਾਲਤ '4':
      ਹਾਲਤ '5':
      ਹਾਲਤ '6':
      ਹਾਲਤ '7':
        state->tmp.selection.ਨਕਾਬ |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        ਰੋਕੋ;

      ਹਾਲਤ ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        ਜੇ(!state->tmp.selection.ਨਕਾਬ)
          state->tmp.selection.ਨਕਾਬ = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        ਰੋਕੋ;
    }

    frag.str++;
    frag.len--;
  }

  ਜੇ(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    ਜੇ(frag.final && state->selection.callbacks->ਸੈੱਟ_2) {
      (*state->selection.callbacks->ਸੈੱਟ_2)(state->tmp.selection.ਨਕਾਬ, (VTermStringFragment){
              .str     = ਨਿਰਾ,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_ਸੈੱਟ,
              .final   = TRUE,
            }, state->selection.user);
    }
    ਵਾਪਸ;
  }

  ਜੇ(state->tmp.selection.state == SELECTION_SELECTED) {
    ਜੇ(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    ਨਹੀਂਤਾਂ {
      state->tmp.selection.state = SELECTION_ਸੈੱਟ_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  ਜੇ(state->tmp.selection.state == SELECTION_QUERY) {
    ਜੇ(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.ਨਕਾਬ, state->selection.user);
    ਵਾਪਸ;
  }

  ਜੇ(state->tmp.selection.state == SELECTION_INVALID)
    ਵਾਪਸ;

  ਜੇ(state->selection.callbacks->ਸੈੱਟ_2) {
    size_t bufcur = 0;
    ਅੱਖਰ *ਬਫਰ = state->selection.ਬਫਰ;

    uint32_t x = 0; /* Current decoding value */
    ਪੂਰਨ n = 0;      /* Number of sextets consumed */

    ਜੇ(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    ਜਦੋਂ((state->selection.buflen - bufcur) >= 3 && frag.len) {
      ਜੇ(frag.str[0] == '=') {
        ਜੇ(n == 2) {
          ਬਫਰ[0] = (x >> 4) & 0xFF;
          ਬਫਰ += 1, bufcur += 1;
        }
        ਜੇ(n == 3) {
          ਬਫਰ[0] = (x >> 10) & 0xFF;
          ਬਫਰ[1] = (x >>  2) & 0xFF;
          ਬਫਰ += 2, bufcur += 2;
        }

        ਜਦੋਂ(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      ਨਹੀਂਤਾਂ {
        uint8_t b = unbase64one(frag.str[0]);
        ਜੇ(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          ਜੇ(state->selection.callbacks->ਸੈੱਟ_2) {
            (*state->selection.callbacks->ਸੈੱਟ_2)(state->tmp.selection.ਨਕਾਬ, (VTermStringFragment){
                .str     = ਨਿਰਾ,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          ਰੋਕੋ;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        ਜੇ(n == 4) {
          ਬਫਰ[0] = (x >> 16) & 0xFF;
          ਬਫਰ[1] = (x >>  8) & 0xFF;
          ਬਫਰ[2] = (x >>  0) & 0xFF;

          ਬਫਰ += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      ਜੇ(!frag.len || (state->selection.buflen - bufcur) < 3) {
        ਜੇ(bufcur) {
	  VTermStringFragment setfrag = {
	    state->selection.ਬਫਰ, // str
	    bufcur, // len
	    state->tmp.selection.state == SELECTION_ਸੈੱਟ_INITIAL, // initial
	    frag.final && !frag.len // final
	  };
          (*state->selection.callbacks->ਸੈੱਟ_2)(state->tmp.selection.ਨਕਾਬ,
	      setfrag, state->selection.user);
          state->tmp.selection.state = SELECTION_ਸੈੱਟ;
        }

        ਬਫਰ = state->selection.ਬਫਰ;
        bufcur = 0;
      }
    }

    ਜੇ(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

ਸਥਿਰ ਪੂਰਨ on_osc(ਪੂਰਨ command, VTermStringFragment frag, ਖਾਲੀ *user)
{
  VTermState *state = user;

  ਚੋਣ(command) {
    ਹਾਲਤ 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ਵਾਪਸ 1;

    ਹਾਲਤ 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      ਵਾਪਸ 1;

    ਹਾਲਤ 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ਵਾਪਸ 1;

    ਹਾਲਤ 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        ਪੂਰਨ red = state->default_fg.red;
        ਪੂਰਨ blue = state->default_fg.blue;
        ਪੂਰਨ green = state->default_fg.green;
        vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        ਵਾਪਸ 1;
      }

    ਹਾਲਤ 11:
      {
	// request background color: <Esc>]11;?<0x07>
	ਪੂਰਨ red = state->default_bg.red;
	ਪੂਰਨ blue = state->default_bg.blue;
	ਪੂਰਨ green = state->default_bg.green;
	vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	ਵਾਪਸ 1;
      }
    ਹਾਲਤ 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      ਵਾਪਸ 1;

    ਹਾਲਤ 52:
      ਜੇ(state->selection.callbacks)
        osc_selection(state, frag);

      ਵਾਪਸ 1;

    ਮੂਲ:
      ਜੇ(state->fallbacks && state->fallbacks->osc)
        ਜੇ((*state->fallbacks->osc)(command, frag, state->fbdata))
          ਵਾਪਸ 1;
  }

  ਵਾਪਸ 0;
}

ਸਥਿਰ ਖਾਲੀ request_ਸਥਿਤੀ_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ਅੱਖਰ *tmp = state->tmp.decrqss;

  ਜੇ(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  ਜਦੋਂ(i < ਆਕਾਰ(state->tmp.decrqss)-1 && tmp[i])
    i++;
  ਜਦੋਂ(i < ਆਕਾਰ(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  ਜੇ(!frag.final)
    ਵਾਪਸ;

  ਚੋਣ(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    ਹਾਲਤ 'm': {
      // Query SGR
      ਲੰਮਾ ਦਲੀਲਾਂ[20];
      ਪੂਰਨ argc = vterm_state_getpen(state, ਦਲੀਲਾਂ, ਆਕਾਰ(ਦਲੀਲਾਂ)/ਆਕਾਰ(ਦਲੀਲਾਂ[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->ਬਹੁਲਕ.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      ਜੇ(cur >= vt->tmpbuffer_len)
        ਵਾਪਸ;

      ਲਈ(ਪੂਰਨ argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_ਹੋਰ(ਦਲੀਲਾਂ[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(ਦਲੀਲਾਂ[argi]));
        ਜੇ(cur >= vt->tmpbuffer_len)
          ਵਾਪਸ;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->ਬਹੁਲਕ.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      ਜੇ(cur >= vt->tmpbuffer_len)
        ਵਾਪਸ;

      vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(vt, vt->tmpbuffer, cur);
      ਵਾਪਸ;
    }

    ਹਾਲਤ 'r':
      // Query DECSTBM
      vterm_ਧੱਕੋ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_ਸਿਖਰ+1, SCROLLREGION_BOTTOM(state));
      ਵਾਪਸ;

    ਹਾਲਤ 's':
      // Query DECSLRM
      vterm_ਧੱਕੋ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_ਖੱਬਾ(state)+1, SCROLLREGION_ਸੱਜਾ(state));
      ਵਾਪਸ;

    ਹਾਲਤ ' '|('q'<<8): {
      // Query DECSCUSR
      ਪੂਰਨ ਜਵਾਬ;
      ਚੋਣ(state->ਬਹੁਲਕ.cursor_shape) {
        ਹਾਲਤ VTERM_PROP_CURSORSHAPE_ਖੰਡ:     ਜਵਾਬ = 2; ਰੋਕੋ;
        ਹਾਲਤ VTERM_PROP_CURSORSHAPE_UNDERLINE: ਜਵਾਬ = 4; ਰੋਕੋ;
	ਮੂਲ: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ ਜਵਾਬ = 6; ਰੋਕੋ;
      }
      ਜੇ(state->ਬਹੁਲਕ.cursor_blink)
        ਜਵਾਬ--;
      vterm_ਧੱਕੋ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", ਜਵਾਬ);
      ਵਾਪਸ;
    }

    ਹਾਲਤ '\"'|('q'<<8):
      // Query DECSCA
      vterm_ਧੱਕੋ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      ਵਾਪਸ;
  }

  vterm_ਧੱਕੋ_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ਸਥਿਰ ਪੂਰਨ on_dcs(ਅਚਲ ਅੱਖਰ *command, size_t commandlen, VTermStringFragment frag, ਖਾਲੀ *user)
{
  VTermState *state = user;

  ਜੇ(commandlen == 2 && strneq(command, "$q", 2)) {
    request_ਸਥਿਤੀ_string(state, frag);
    ਵਾਪਸ 1;
  }
  ਨਹੀਂਤਾਂ ਜੇ(state->fallbacks && state->fallbacks->dcs)
    ਜੇ((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      ਵਾਪਸ 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (ਪੂਰਨ)commandlen, command);
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ on_apc(VTermStringFragment frag, ਖਾਲੀ *user)
{
  VTermState *state = user;

  ਜੇ(state->fallbacks && state->fallbacks->apc)
    ਜੇ((*state->fallbacks->apc)(frag, state->fbdata))
      ਵਾਪਸ 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ on_pm(VTermStringFragment frag, ਖਾਲੀ *user)
{
  VTermState *state = user;

  ਜੇ(state->fallbacks && state->fallbacks->pm)
    ਜੇ((*state->fallbacks->pm)(frag, state->fbdata))
      ਵਾਪਸ 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ on_sos(VTermStringFragment frag, ਖਾਲੀ *user)
{
  VTermState *state = user;

  ਜੇ(state->fallbacks && state->fallbacks->sos)
    ਜੇ((*state->fallbacks->sos)(frag, state->fbdata))
      ਵਾਪਸ 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ on_resize(ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ, ਖਾਲੀ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ > VTERM_MAX_ਖਾਨੇ_ਗਿਣਤੀ)
    ਖਾਨੇ_ਗਿਣਤੀ = VTERM_MAX_ਖਾਨੇ_ਗਿਣਤੀ;

  ਜੇ (ਪੰਕਤੀਆਂ_2 > VTERM_MAX_ਪੰਕਤੀਆਂ)
    ਪੰਕਤੀਆਂ_2 = VTERM_MAX_ਪੰਕਤੀਆਂ;

  ਜੇ(ਖਾਨੇ_ਗਿਣਤੀ != state->ਖਾਨੇ_ਗਿਣਤੀ) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *newtabstops = vterm_allocator_malloc(state->vt, (ਖਾਨੇ_ਗਿਣਤੀ + 7) / 8);
    ਜੇ (newtabstops == ਨਿਰਾ)
      ਵਾਪਸ 0;

    /* TODO: This can all be done much more efficiently bytewise */
    ਪੂਰਨ ਖਾਨਾ;
    ਲਈ(ਖਾਨਾ = 0; ਖਾਨਾ < state->ਖਾਨੇ_ਗਿਣਤੀ && ਖਾਨਾ < ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਨਕਾਬ = 1 << (ਖਾਨਾ & 7);
      ਜੇ(state->tabstops[ਖਾਨਾ >> 3] & ਨਕਾਬ)
        newtabstops[ਖਾਨਾ >> 3] |= ਨਕਾਬ;
      ਨਹੀਂਤਾਂ
        newtabstops[ਖਾਨਾ >> 3] &= ~ਨਕਾਬ;
      }

    ਲਈ( ; ਖਾਨਾ < ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਨਕਾਬ = 1 << (ਖਾਨਾ & 7);
      ਜੇ(ਖਾਨਾ % 8 == 0)
        newtabstops[ਖਾਨਾ >> 3] |= ਨਕਾਬ;
      ਨਹੀਂਤਾਂ
        newtabstops[ਖਾਨਾ >> 3] &= ~ਨਕਾਬ;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->ਪੰਕਤੀਆਂ_2 = ਪੰਕਤੀਆਂ_2;
  state->ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;

  ਜੇ(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->ਪੰਕਤੀਆਂ_2);
  ਜੇ(state->scrollregion_ਸੱਜਾ > -1)
    UBOUND(state->scrollregion_ਸੱਜਾ, state->ਖਾਨੇ_ਗਿਣਤੀ);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_ਸਿਖਰ, state->ਪੰਕਤੀਆਂ_2);
  UBOUND(state->scrollregion_ਖੱਬਾ, state->ਖਾਨੇ_ਗਿਣਤੀ);
  ਜੇ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ਸਿਖਰ) {
    state->scrollregion_ਸਿਖਰ = 0;
    state->scrollregion_bottom = -1;
  }
  ਜੇ(SCROLLREGION_ਸੱਜਾ(state) <= state->scrollregion_ਖੱਬਾ) {
    state->scrollregion_ਖੱਬਾ = 0;
    state->scrollregion_ਸੱਜਾ = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  ਜੇ(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(ਪੰਕਤੀਆਂ_2, ਖਾਨੇ_ਗਿਣਤੀ, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  ਨਹੀਂਤਾਂ {
    ਜੇ(ਪੰਕਤੀਆਂ_2 != state->ਪੰਕਤੀਆਂ_2) {
      ਲਈ(ਪੂਰਨ bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_ਸਤਰ_Info *oldlineinfo = state->lineinfos[bufidx];
        ਜੇ(!oldlineinfo)
          ਜਾਰੀ;

        VTerm_ਸਤਰ_Info *newlineinfo = vterm_allocator_malloc(state->vt, ਪੰਕਤੀਆਂ_2 * ਆਕਾਰ(VTerm_ਸਤਰ_Info));

        ਪੂਰਨ ਪੰਕਤੀ;
        ਲਈ(ਪੰਕਤੀ = 0; ਪੰਕਤੀ < state->ਪੰਕਤੀਆਂ_2 && ਪੰਕਤੀ < ਪੰਕਤੀਆਂ_2; ਪੰਕਤੀ++) {
          newlineinfo[ਪੰਕਤੀ] = oldlineinfo[ਪੰਕਤੀ];
        }

        ਲਈ( ; ਪੰਕਤੀ < ਪੰਕਤੀਆਂ_2; ਪੰਕਤੀ++) {
          newlineinfo[ਪੰਕਤੀ] = (VTerm_ਸਤਰ_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->ਬਹੁਲਕ.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  ਜੇ(state->at_phantom && state->pos.ਖਾਨਾ < ਖਾਨੇ_ਗਿਣਤੀ-1) {
    state->at_phantom = 0;
    state->pos.ਖਾਨਾ++;
  }

  ਜੇ(state->pos.ਪੰਕਤੀ < 0)
    state->pos.ਪੰਕਤੀ = 0;
  ਜੇ(state->pos.ਪੰਕਤੀ >= ਪੰਕਤੀਆਂ_2)
    state->pos.ਪੰਕਤੀ = ਪੰਕਤੀਆਂ_2 - 1;
  ਜੇ(state->pos.ਖਾਨਾ < 0)
    state->pos.ਖਾਨਾ = 0;
  ਜੇ(state->pos.ਖਾਨਾ >= ਖਾਨੇ_ਗਿਣਤੀ)
    state->pos.ਖਾਨਾ = ਖਾਨੇ_ਗਿਣਤੀ - 1;

  updatecursor(state, &oldpos, 1);

  ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ VTermParserCallbacks parser_callbacks = {
  on_ਪਾਠ, //ਪਾਠ
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
  ਜੇ(vt->state)
    ਵਾਪਸ vt->state;

  VTermState *state = vterm_state_new(vt);
  ਜੇ (state == ਨਿਰਾ)
    ਵਾਪਸ ਨਿਰਾ;
  vt->state = state;

  vterm_parser_ਸੈੱਟ_callbacks(vt, &parser_callbacks, state);

  ਵਾਪਸ state;
}

ਖਾਲੀ vterm_state_reset(VTermState *state, ਪੂਰਨ hard)
{
  state->scrollregion_ਸਿਖਰ = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_ਖੱਬਾ = 0;
  state->scrollregion_ਸੱਜਾ = -1;

  state->ਬਹੁਲਕ.keypad          = 0;
  state->ਬਹੁਲਕ.cursor          = 0;
  state->ਬਹੁਲਕ.autowrap        = 1;
  state->ਬਹੁਲਕ.ਪਾਓ          = 0;
  state->ਬਹੁਲਕ.newline         = 0;
  state->ਬਹੁਲਕ.alt_screen      = 0;
  state->ਬਹੁਲਕ.origin          = 0;
  state->ਬਹੁਲਕ.leftrightmargin = 0;
  state->ਬਹੁਲਕ.bracketpaste    = 0;
  state->ਬਹੁਲਕ.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->ਬਹੁਲਕ.ctrl8bit   = 0;

  ਲਈ(ਪੂਰਨ ਖਾਨਾ = 0; ਖਾਨਾ < state->ਖਾਨੇ_ਗਿਣਤੀ; ਖਾਨਾ++)
    ਜੇ(ਖਾਨਾ % 8 == 0)
      ਸੈੱਟ_ਖਾਨਾ_tabstop(state, ਖਾਨਾ);
    ਨਹੀਂਤਾਂ
      clear_ਖਾਨਾ_tabstop(state, ਖਾਨਾ);

  ਲਈ(ਪੂਰਨ ਪੰਕਤੀ = 0; ਪੰਕਤੀ < state->ਪੰਕਤੀਆਂ_2; ਪੰਕਤੀ++)
    ਸੈੱਟ_lineinfo(state, ਪੰਕਤੀ, FORCE, DWL_OFF, DHL_OFF);

  ਜੇ(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->ਬਹੁਲਕ.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  ਲਈ(ਪੂਰਨ i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    ਜੇ(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].ਅੰਕੜੇ);
  }

  state->gl_ਸੈੱਟ = 0;
  state->gr_ਸੈੱਟ = 1;
  state->gsingle_ਸੈੱਟ = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_ਖੰਡ);

  ਜੇ(hard) {
    state->pos.ਪੰਕਤੀ = 0;
    state->pos.ਖਾਨਾ = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.ਸਿਰਾ_ਪੰਕਤੀ = state->ਪੰਕਤੀਆਂ_2;
    rect.ਸਿਰਾ_ਖਾਨਾ =  state->ਖਾਨੇ_ਗਿਣਤੀ;
    erase(state, rect, 0);
  }
}

ਖਾਲੀ vterm_state_get_cursorpos(ਅਚਲ VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

ਖਾਲੀ vterm_state_get_mousestate(ਅਚਲ VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.ਖਾਨਾ = state->mouse_ਖਾਨਾ;
  mousestate->pos.ਪੰਕਤੀ = state->mouse_ਪੰਕਤੀ;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

ਖਾਲੀ vterm_state_ਸੈੱਟ_callbacks(VTermState *state, ਅਚਲ VTermStateCallbacks *callbacks, ਖਾਲੀ *user)
{
  ਜੇ(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    ਜੇ(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  ਨਹੀਂਤਾਂ {
    state->callbacks = ਨਿਰਾ;
    state->cbdata = ਨਿਰਾ;
  }
}

ਖਾਲੀ vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

ਖਾਲੀ *vterm_state_get_cbdata(VTermState *state)
{
  ਵਾਪਸ state->cbdata;
}

ਖਾਲੀ vterm_state_ਸੈੱਟ_unrecognised_fallbacks(VTermState *state, ਅਚਲ VTermStateFallbacks *fallbacks, ਖਾਲੀ *user)
{
  ਜੇ(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  ਨਹੀਂਤਾਂ {
    state->fallbacks = ਨਿਰਾ;
    state->fbdata = ਨਿਰਾ;
  }
}

ਖਾਲੀ *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  ਵਾਪਸ state->fbdata;
}

ਪੂਰਨ vterm_state_ਸੈੱਟ_termprop(VTermState *state, VTermProp prop, VTerm_ਮੁੱਲ *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  ਜੇ(state->callbacks && state->callbacks->settermprop)
    ਜੇ(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      ਵਾਪਸ 0;

  ਚੋਣ(prop) {
  ਹਾਲਤ VTERM_PROP_TITLE:
  ਹਾਲਤ VTERM_PROP_ICONNAME:
  ਹਾਲਤ VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_PROP_CURSORVISIBLE:
    state->ਬਹੁਲਕ.cursor_visible = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_PROP_CURSORBLINK:
    state->ਬਹੁਲਕ.cursor_blink = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_PROP_CURSORSHAPE:
    state->ਬਹੁਲਕ.cursor_shape = val->ਸੰਖਿਆ;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_PROP_ਉਲਟ:
    state->ਬਹੁਲਕ.screen = val->boolean;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_PROP_ALTSCREEN:
    state->ਬਹੁਲਕ.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->ਬਹੁਲਕ.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    ਜੇ(state->ਬਹੁਲਕ.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.ਸਿਰਾ_ਪੰਕਤੀ = state->ਪੰਕਤੀਆਂ_2;
      rect.ਸਿਰਾ_ਖਾਨਾ = state->ਖਾਨੇ_ਗਿਣਤੀ;
      erase(state, rect, 0);
    }
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    ਜੇ(val->ਸੰਖਿਆ)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    ਜੇ(val->ਸੰਖਿਆ == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    ਜੇ(val->ਸੰਖਿਆ == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    ਵਾਪਸ 1;
  ਹਾਲਤ VTERM_PROP_FOCUSREPORT:
    state->ਬਹੁਲਕ.report_focus = val->boolean;
    ਵਾਪਸ 1;

  ਹਾਲਤ VTERM_N_PROPS:
    ਵਾਪਸ 0;
  }

  ਵਾਪਸ 0;
}

ਖਾਲੀ vterm_state_focus_ਵਿੱਚ(VTermState *state)
{
  ਜੇ(state->ਬਹੁਲਕ.report_focus)
    vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

ਖਾਲੀ vterm_state_focus_ਬਾਹਰਲਾ(VTermState *state)
{
  ਜੇ(state->ਬਹੁਲਕ.report_focus)
    vterm_ਧੱਕੋ_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ਅਚਲ VTerm_ਸਤਰ_Info *vterm_state_get_lineinfo(ਅਚਲ VTermState *state, ਪੂਰਨ ਪੰਕਤੀ)
{
  ਵਾਪਸ state->lineinfo + ਪੰਕਤੀ;
}

ਖਾਲੀ vterm_state_ਸੈੱਟ_selection_callbacks(VTermState *state, ਅਚਲ VTermSelectionCallbacks *callbacks, ਖਾਲੀ *user,
    ਅੱਖਰ *ਬਫਰ, size_t buflen)
{
  ਜੇ(buflen && !ਬਫਰ)
    ਬਫਰ = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.ਬਫਰ    = ਬਫਰ;
  state->selection.buflen    = buflen;
}

ਖਾਲੀ vterm_state_send_selection(VTermState *state, VTerm_Selection_ਨਕਾਬ ਨਕਾਬ, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ਜੇ(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ਸਥਿਰ ਅੱਖਰ selection_chars[] = "cpqs";
    ਪੂਰਨ idx;
    ਲਈ(idx = 0; idx < 4; idx++)
      ਜੇ(ਨਕਾਬ & (1 << idx))
        ਰੋਕੋ;

    vterm_ਧੱਕੋ_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  ਜੇ(frag.len) {
    size_t bufcur = 0;
    ਅੱਖਰ *ਬਫਰ = state->selection.ਬਫਰ;

    uint32_t x = 0;
    ਪੂਰਨ n = 0;

    ਜੇ(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    ਜਦੋਂ((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      ਜੇ(n == 3) {
        ਬਫਰ[0] = base64_one((x >> 18) & 0x3F);
        ਬਫਰ[1] = base64_one((x >> 12) & 0x3F);
        ਬਫਰ[2] = base64_one((x >>  6) & 0x3F);
        ਬਫਰ[3] = base64_one((x >>  0) & 0x3F);

        ਬਫਰ += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      ਜੇ(!frag.len || (state->selection.buflen - bufcur) < 4) {
        ਜੇ(bufcur)
          vterm_ਧੱਕੋ_output_ਬਾਈਟਾਂ(vt, state->selection.ਬਫਰ, bufcur);

        ਬਫਰ = state->selection.ਬਫਰ;
        bufcur = 0;
      }
    }

    ਜੇ(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  ਜੇ(frag.final) {
    ਜੇ(state->tmp.selection.sendpartial) {
      ਪੂਰਨ n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      ਅੱਖਰ *ਬਫਰ = state->selection.ਬਫਰ;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      ਬਫਰ[0] = base64_one((x >> 18) & 0x3F);
      ਬਫਰ[1] = base64_one((x >> 12) & 0x3F);
      ਬਫਰ[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      ਬਫਰ[3] = '=';

      vterm_ਧੱਕੋ_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, ਬਫਰ);
    }
    ਨਹੀਂਤਾਂ
      vterm_ਧੱਕੋ_output_sprintf_str(vt, 0, TRUE, "");
  }
}
