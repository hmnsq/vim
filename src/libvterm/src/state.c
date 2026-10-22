#ضمن "vterm_internal.h"

#ضمن <stdio.h>
#ضمن <string.h>

#تعریف_کن strneq(a,b,n) (مقایسه_رشته(a,b,n)==0)

#إذا defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#پایان_اگر

ساكن صحيح on_resize(صحيح ردیفها_2, صحيح تعداد_ستون, فراغ *user);

/* Some convenient wrappers to make callback functions easier */

ساكن فراغ putglyph(VTermState *state, ثابت uint32_t chars[], صحيح عرض, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.عرض = عرض;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.ردیف].doublewidth;
  info.dhl = state->lineinfo[pos.ردیف].doubleheight;

  إذا(state->callbacks && state->callbacks->putglyph)
    إذا((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      أرجع;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.ستون, pos.ردیف);
}

ساكن فراغ updatecursor(VTermState *state, VTermPos *oldpos, صحيح cancel_phantom)
{
  إذا(state->pos.ستون == oldpos->ستون && state->pos.ردیف == oldpos->ردیف)
    أرجع;

  إذا(cancel_phantom)
    state->at_phantom = 0;

  إذا(state->callbacks && state->callbacks->movecursor)
    إذا((*state->callbacks->movecursor)(state->pos, *oldpos, state->نما.cursor_visible, state->cbdata))
      أرجع;
}

ساكن فراغ erase(VTermState *state, VTermRect rect, صحيح selective)
{
  إذا(rect.انتها_ستون == state->تعداد_ستون) {
    صحيح ردیف;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    لكل(ردیف = rect.شروع_ردیف + 1; ردیف < rect.انتها_ردیف + 1 && ردیف < state->ردیفها_2; ردیف++)
      state->lineinfo[ردیف].continuation = 0;
  }

  إذا(state->callbacks && state->callbacks->erase)
    إذا((*state->callbacks->erase)(rect, selective, state->cbdata))
      أرجع;
}

ساكن VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, حجم(VTermState));

  إذا (state == عدم)
    أرجع عدم;
  state->vt = vt;

  state->ردیفها_2 = vt->ردیفها_2;
  state->تعداد_ستون = vt->تعداد_ستون;

  state->mouse_ستون     = 0;
  state->mouse_ردیف     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = عدم;
  state->cbdata    = عدم;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = عدم;
  state->selection.user      = عدم;
  state->selection.میانگیر    = عدم;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_اندازه = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_اندازه * حجم(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->تعداد_ستون + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->ردیفها_2 * حجم(VTerm_سطر_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->ردیفها_2 * حجم(VTerm_سطر_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  إذا(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.داده);

  أرجع state;
}

داخلی فراغ vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  إذا(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ساكن فراغ scroll(VTermState *state, VTermRect rect, صحيح downward, صحيح rightward)
{
  صحيح ردیفها_2;
  صحيح تعداد_ستون;
  إذا(!downward && !rightward)
    أرجع;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  إذا(rect.انتها_ردیف <= rect.شروع_ردیف || rect.انتها_ستون <= rect.شروع_ستون)
    أرجع;

  ردیفها_2 = rect.انتها_ردیف - rect.شروع_ردیف;
  إذا(downward > ردیفها_2)
    downward = ردیفها_2;
  وإلا إذا(downward < -ردیفها_2)
    downward = -ردیفها_2;

  تعداد_ستون = rect.انتها_ستون - rect.شروع_ستون;
  إذا(rightward > تعداد_ستون)
    rightward = تعداد_ستون;
  وإلا إذا(rightward < -تعداد_ستون)
    rightward = -تعداد_ستون;

  إذا(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    إذا(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .شروع_ردیف = rect.شروع_ردیف, .انتها_ردیف = rect.شروع_ردیف + downward,
          .شروع_ستون = rect.شروع_ستون, .انتها_ستون = rect.انتها_ستون}, state->cbdata);
    وإلا إذا(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .شروع_ردیف = rect.انتها_ردیف + downward, .انتها_ردیف = rect.انتها_ردیف,
          .شروع_ستون = rect.شروع_ستون,          .انتها_ستون = rect.انتها_ستون}, state->cbdata);

    إذا(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .شروع_ردیف = rect.شروع_ردیف, .انتها_ردیف = rect.انتها_ردیف,
          .شروع_ستون = rect.شروع_ستون, .انتها_ستون = rect.شروع_ستون + rightward}, state->cbdata);
    وإلا إذا(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .شروع_ردیف = rect.شروع_ردیف,           .انتها_ردیف = rect.انتها_ردیف,
          .شروع_ستون = rect.انتها_ستون + rightward, .انتها_ستون = rect.انتها_ستون}, state->cbdata);
  }

  // Update lineinfo if full line
  إذا(rect.شروع_ستون == 0 && rect.انتها_ستون == state->تعداد_ستون && rightward == 0) {
    صحيح ارتفاع = rect.انتها_ردیف - rect.شروع_ردیف - قدر_مطلق(downward);
    صحيح ردیف;
    VTerm_سطر_Info zero_سطر_Info = {0x0};

    إذا(downward > 0) {
      memmove(state->lineinfo + rect.شروع_ردیف,
              state->lineinfo + rect.شروع_ردیف + downward,
              ارتفاع * حجم(state->lineinfo[0]));
      لكل(ردیف = rect.انتها_ردیف - downward; ردیف < rect.انتها_ردیف; ردیف++)
        state->lineinfo[ردیف] = zero_سطر_Info;
    }
    وإلا {
      memmove(state->lineinfo + rect.شروع_ردیف - downward,
              state->lineinfo + rect.شروع_ردیف,
              ارتفاع * حجم(state->lineinfo[0]));
      لكل(ردیف = rect.شروع_ردیف; ردیف < rect.شروع_ردیف - downward; ردیف++)
        state->lineinfo[ردیف] = zero_سطر_Info;
    }
  }

  إذا(state->callbacks && state->callbacks->scrollrect)
    إذا((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      أرجع;

  إذا(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ساكن فراغ linefeed(VTermState *state)
{
  إذا(state->pos.ردیف == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.شروع_ردیف = state->scrollregion_بالاترین;
    rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_ستون = SCROLLREGION_چپ(state);
    rect.انتها_ستون   = SCROLLREGION_راست(state);

    scroll(state, rect, 1, 0);
  }
  وإلا إذا(state->pos.ردیف < state->ردیفها_2-1)
    state->pos.ردیف++;
}

ساكن فراغ grow_combine_میانگیر(VTermState *state)
{
  size_t    new_اندازه = state->combine_chars_اندازه * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_اندازه * حجم(new_chars[0]));

  کپی_حافظه(new_chars, state->combine_chars, state->combine_chars_اندازه * حجم(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_اندازه = new_اندازه;
}

ساكن فراغ مجموعه_ستون_tabstop(VTermState *state, صحيح ستون)
{
  بیعلامت حرف ماسک = 1 << (ستون & 7);
  state->tabstops[ستون >> 3] |= ماسک;
}

ساكن فراغ clear_ستون_tabstop(VTermState *state, صحيح ستون)
{
  بیعلامت حرف ماسک = 1 << (ستون & 7);
  state->tabstops[ستون >> 3] &= ~ماسک;
}

ساكن صحيح is_ستون_tabstop(VTermState *state, صحيح ستون)
{
  بیعلامت حرف ماسک = 1 << (ستون & 7);
  أرجع state->tabstops[ستون >> 3] & ماسک;
}

ساكن صحيح is_cursor_درون_scrollregion(ثابت VTermState *state)
{
  إذا(state->pos.ردیف < state->scrollregion_بالاترین ||
     state->pos.ردیف >= SCROLLREGION_BOTTOM(state))
    أرجع 0;
  إذا(state->pos.ستون < SCROLLREGION_چپ(state) ||
     state->pos.ستون >= SCROLLREGION_راست(state))
    أرجع 0;

  أرجع 1;
}

ساكن فراغ tab(VTermState *state, صحيح شمار, صحيح direction)
{
  طالما(شمار > 0) {
    إذا(direction > 0) {
      إذا(state->pos.ستون >= THISROWWIDTH(state)-1)
        أرجع;

      state->pos.ستون++;
    }
    وإلا إذا(direction < 0) {
      إذا(state->pos.ستون < 1)
        أرجع;

      state->pos.ستون--;
    }

    إذا(is_ستون_tabstop(state, state->pos.ستون))
      شمار--;
  }
}

#تعریف_کن NO_FORCE 0
#تعریف_کن FORCE    1

#تعریف_کن DWL_OFF 0
#تعریف_کن DWL_ON  1

#تعریف_کن DHL_OFF    0
#تعریف_کن DHL_بالاترین    1
#تعریف_کن DHL_BOTTOM 2

ساكن فراغ مجموعه_lineinfo(VTermState *state, صحيح ردیف, صحيح force, صحيح dwl, صحيح dhl)
{
  VTerm_سطر_Info info = state->lineinfo[ردیف];

  إذا(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  وإلا إذا(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  إذا(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  وإلا إذا(dhl == DHL_بالاترین)
    info.doubleheight = DHL_بالاترین;
  وإلا إذا(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  إذا((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(ردیف, &info, state->lineinfo + ردیف, state->cbdata))
      || force)
    state->lineinfo[ردیف] = info;
}

ساكن صحيح on_متن(ثابت حرف بایتها[], size_t len, فراغ *user)
{
  VTermState *state = user;
  صحيح npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  صحيح i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / حجم(uint32_t);

  encoding =
    state->gsingle_مجموعه     ? &state->encoding[state->gsingle_مجموعه] :
    !(بایتها[eaten] & 0x80) ? &state->encoding[state->gl_مجموعه] :
    state->vt->نما.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_مجموعه];
  إذا (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->داده,
      codepoints, &npoints, state->gsingle_مجموعه ? 1 : (صحيح)maxpoints,
      بایتها, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  إذا(!npoints)
  {
    أرجع (صحيح)eaten;
  }

  إذا(state->gsingle_مجموعه && npoints)
    state->gsingle_مجموعه = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  إذا(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    إذا(state->pos.ردیف == state->combine_pos.ردیف && state->pos.ستون == state->combine_pos.ستون + state->combine_عرض) {
#اگر_تعریف_شده DEBUG_GLYPH_COMBINE
      صحيح printpos;
      چاپ_کن("DEBUG: COMBINING SPLIT GLYPH of chars {");
      لكل(printpos = 0; state->combine_chars[printpos]; printpos++)
        چاپ_کن("U+%04x ", state->combine_chars[printpos]);
      چاپ_کن("} + {");
#پایان_اگر

      /* Find where we need to append these combining chars */
      صحيح saved_i = 0;
      طالما(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      طالما(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        إذا(saved_i >= (صحيح)state->combine_chars_اندازه)
          grow_combine_میانگیر(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      إذا(saved_i >= (صحيح)state->combine_chars_اندازه)
        grow_combine_میانگیر(state);
      state->combine_chars[saved_i] = 0;

#اگر_تعریف_شده DEBUG_GLYPH_COMBINE
      لكل(; state->combine_chars[printpos]; printpos++)
        چاپ_کن("U+%04x ", state->combine_chars[printpos]);
      چاپ_کن("}\n");
#پایان_اگر

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_عرض, state->combine_pos);
    }
    وإلا {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  لكل(; i < npoints; i++) {
    // Try to find combining characters following this
    صحيح glyph_starts = i;
    صحيح glyph_دوسر;
    صحيح عرض = 0;

    لكل(glyph_دوسر = i + 1;
        (glyph_دوسر < npoints) && (glyph_دوسر < glyph_starts + VTERM_MAX_CHARS_در_هر_CELL);
        glyph_دوسر++)
      إذا(!vterm_unicode_is_combining(codepoints[glyph_دوسر]))
        اكسر;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_در_هر_CELL + 1) * حجم(uint32_t));
    إذا (chars == عدم)
      اكسر;

    لكل( ; i < glyph_دوسر; i++) {
      صحيح this_عرض;
      إذا(vterm_get_special_pty_type() == 2) {
        state->vt->درون_backspace -= (state->vt->درون_backspace > 0) ? 1 : 0;
        إذا(state->vt->درون_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_عرض = vterm_unicode_عرض(codepoints[i]);
#اگر_تعریف_شده DEBUG
      إذا(this_عرض < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#پایان_اگر
      إذا (i == glyph_starts || this_عرض > عرض)
	عرض = this_عرض;  // TODO: should be += ?
    }

    إذا (عرض < 0)
      عرض = 0;

    طالما(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_دوسر - glyph_starts] = 0;
    i--;

#اگر_تعریف_شده DEBUG_GLYPH_COMBINE
    صحيح printpos;
    چاپ_کن("DEBUG: COMBINED GLYPH of %d chars {", glyph_دوسر - glyph_starts);
    لكل(printpos = 0; printpos < glyph_دوسر - glyph_starts; printpos++)
      چاپ_کن("U+%04x ", chars[printpos]);
    چاپ_کن("}, onscreen width %d\n", عرض);
#پایان_اگر

    إذا(state->at_phantom || state->pos.ستون + عرض > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.ستون = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.ردیف].continuation = 1;
    }

    إذا(state->نما.درج) {
      // TODO: This will be a little inefficient for large bodies of text, as
      // it'll have to 'ICH' effectively before every glyph. We should scan
      // ahead and ICH as many times as required
      VTermRect rect;
      rect.شروع_ردیف = state->pos.ردیف;
      rect.انتها_ردیف   = state->pos.ردیف + 1;
      rect.شروع_ستون = state->pos.ستون;
      rect.انتها_ستون   = THISROWWIDTH(state);
      scroll(state, rect, 0, -1);
    }

    putglyph(state, chars, عرض, state->pos);

    إذا(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      صحيح save_i;
      لكل(save_i = 0; chars[save_i]; save_i++) {
        إذا(save_i >= (صحيح)state->combine_chars_اندازه)
          grow_combine_میانگیر(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      إذا(save_i >= (صحيح)state->combine_chars_اندازه)
        grow_combine_میانگیر(state);
      state->combine_chars[save_i] = 0;
      state->combine_عرض = عرض;
      state->combine_pos = state->pos;
    }

    إذا(state->pos.ستون + عرض >= THISROWWIDTH(state)) {
      إذا(state->نما.autowrap)
        state->at_phantom = 1;
    }
    وإلا {
      state->pos.ستون += عرض;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#اگر_تعریف_شده DEBUG
  إذا(state->pos.ردیف < 0 || state->pos.ردیف >= state->ردیفها_2 ||
     state->pos.ستون < 0 || state->pos.ستون >= state->تعداد_ستون) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.ردیف, state->pos.ستون);
    abort();
  }
#پایان_اگر

  أرجع (صحيح)eaten;
}

ساكن صحيح on_control(بیعلامت حرف control, فراغ *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.ستون -= (leadpos.ستون >= 2 ? 2 : 0);

  بدل(control) {
  حالة 0x07: // BEL - ECMA-48 8.3.3
    إذا(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    اكسر;

  حالة 0x08: // BS - ECMA-48 8.3.5
    إذا(state->pos.ستون > 0)
      state->pos.ستون--;
    إذا(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      إذا(vterm_unicode_عرض(cell.chars[0]) == 2)
        state->pos.ستون--;
    }
    اكسر;

  حالة 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    اكسر;

  حالة 0x0a: // LF - ECMA-48 8.3.74
  حالة 0x0b: // VT
  حالة 0x0c: // FF
    linefeed(state);
    إذا(state->نما.newline)
      state->pos.ستون = 0;
    اكسر;

  حالة 0x0d: // CR - ECMA-48 8.3.15
    state->pos.ستون = 0;
    اكسر;

  حالة 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_مجموعه = 1;
    اكسر;

  حالة 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_مجموعه = 0;
    اكسر;

  حالة 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    اكسر;

  حالة 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.ستون = 0;
    اكسر;

  حالة 0x88: // HTS - ECMA-48 8.3.62
    مجموعه_ستون_tabstop(state, state->pos.ستون);
    اكسر;

  حالة 0x8d: // RI - ECMA-48 8.3.104
    إذا(state->pos.ردیف == state->scrollregion_بالاترین) {
      VTermRect rect;
      rect.شروع_ردیف = state->scrollregion_بالاترین;
      rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
      rect.شروع_ستون = SCROLLREGION_چپ(state);
      rect.انتها_ستون   = SCROLLREGION_راست(state);

      scroll(state, rect, -1, 0);
    }
    وإلا إذا(state->pos.ردیف > 0)
        state->pos.ردیف--;
    اكسر;

  حالة 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_مجموعه = 2;
    اكسر;

  حالة 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_مجموعه = 3;
    اكسر;

  افتراضي:
    إذا(state->fallbacks && state->fallbacks->control)
      إذا((*state->fallbacks->control)(control, state->fbdata))
        أرجع 1;

    أرجع 0;
  }

  updatecursor(state, &oldpos, 1);

#اگر_تعریف_شده DEBUG
  إذا(state->pos.ردیف < 0 || state->pos.ردیف >= state->ردیفها_2 ||
     state->pos.ستون < 0 || state->pos.ستون >= state->تعداد_ستون) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.ردیف, state->pos.ستون);
    abort();
  }
#پایان_اگر

  أرجع 1;
}

ساكن صحيح settermprop_bool(VTermState *state, VTermProp prop, صحيح v)
{
  VTerm_مقدار val;
  val.boolean = v;
  أرجع vterm_state_مجموعه_termprop(state, prop, &val);
}

ساكن صحيح settermprop_int(VTermState *state, VTermProp prop, صحيح v)
{
  VTerm_مقدار val;
  val.عدد = v;
  أرجع vterm_state_مجموعه_termprop(state, prop, &val);
}

ساكن صحيح settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_مقدار val;

  val.string = frag;
  أرجع vterm_state_مجموعه_termprop(state, prop, &val);
}

ساكن فراغ savecursor(VTermState *state, صحيح save)
{
  إذا(save) {
    state->saved.pos = state->pos;
    state->saved.نما.cursor_visible = state->نما.cursor_visible;
    state->saved.نما.cursor_blink   = state->نما.cursor_blink;
    state->saved.نما.cursor_shape   = state->نما.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  وإلا {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.نما.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.نما.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.نما.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ساكن صحيح on_escape(ثابت حرف *بایتها, size_t len, فراغ *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  بدل(بایتها[0]) {
  حالة ' ':
    إذا(len != 2)
      أرجع 0;

    بدل(بایتها[1]) {
      حالة 'F': // S7C1T
        state->vt->نما.ctrl8bit = 0;
        اكسر;

      حالة 'G': // S8C1T
        state->vt->نما.ctrl8bit = 1;
        اكسر;

      افتراضي:
        أرجع 0;
    }
    أرجع 2;

  حالة '#':
    إذا(len != 2)
      أرجع 0;

    بدل(بایتها[1]) {
      حالة '3': // DECDHL top
        إذا(state->نما.leftrightmargin)
          اكسر;
        مجموعه_lineinfo(state, state->pos.ردیف, NO_FORCE, DWL_ON, DHL_بالاترین);
        اكسر;

      حالة '4': // DECDHL bottom
        إذا(state->نما.leftrightmargin)
          اكسر;
        مجموعه_lineinfo(state, state->pos.ردیف, NO_FORCE, DWL_ON, DHL_BOTTOM);
        اكسر;

      حالة '5': // DECSWL
        إذا(state->نما.leftrightmargin)
          اكسر;
        مجموعه_lineinfo(state, state->pos.ردیف, NO_FORCE, DWL_OFF, DHL_OFF);
        اكسر;

      حالة '6': // DECDWL
        إذا(state->نما.leftrightmargin)
          اكسر;
        مجموعه_lineinfo(state, state->pos.ردیف, NO_FORCE, DWL_ON, DHL_OFF);
        اكسر;

      حالة '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        لكل(pos.ردیف = 0; pos.ردیف < state->ردیفها_2; pos.ردیف++)
          لكل(pos.ستون = 0; pos.ستون < ROWWIDTH(state, pos.ردیف); pos.ستون++)
            putglyph(state, E, 1, pos);
        اكسر;
      }

      افتراضي:
        أرجع 0;
    }
    أرجع 2;

  حالة '(': حالة ')': حالة '*': حالة '+': // SCS
    إذا(len != 2)
      أرجع 0;

    {
      صحيح setnum = بایتها[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, بایتها[1]);

      إذا(newenc) {
        state->encoding[setnum].enc = newenc;

        إذا(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].داده);
      }
    }

    أرجع 2;

  حالة '7': // DECSC
    savecursor(state, 1);
    أرجع 1;

  حالة '8': // DECRC
    savecursor(state, 0);
    أرجع 1;

  حالة '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    أرجع 1;

  حالة '=': // DECKPAM
    state->نما.keypad = 1;
    أرجع 1;

  حالة '>': // DECKPNM
    state->نما.keypad = 0;
    أرجع 1;

  حالة 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    إذا(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->نما.cursor_visible, state->cbdata);
    أرجع 1;
  }

  حالة 'n': // LS2 - ECMA-48 8.3.78
    state->gl_مجموعه = 2;
    أرجع 1;

  حالة 'o': // LS3 - ECMA-48 8.3.80
    state->gl_مجموعه = 3;
    أرجع 1;

  حالة '~': // LS1R - ECMA-48 8.3.77
    state->gr_مجموعه = 1;
    أرجع 1;

  حالة '}': // LS2R - ECMA-48 8.3.79
    state->gr_مجموعه = 2;
    أرجع 1;

  حالة '|': // LS3R - ECMA-48 8.3.81
    state->gr_مجموعه = 3;
    أرجع 1;

  افتراضي:
    أرجع 0;
  }
}

ساكن فراغ مجموعه_نما(VTermState *state, صحيح رقم, صحيح val)
{
  بدل(رقم) {
  حالة 4: // IRM - ECMA-48 7.2.10
    state->نما.درج = val;
    اكسر;

  حالة 20: // LNM - ANSI X3.4-1977
    state->نما.newline = val;
    اكسر;

  افتراضي:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", رقم);
    أرجع;
  }
}

ساكن فراغ مجموعه_dec_نما(VTermState *state, صحيح رقم, صحيح val)
{
  بدل(رقم) {
  حالة 1:
    state->نما.cursor = val;
    اكسر;

  حالة 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_معکوس, val);
    اكسر;

  حالة 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->نما.origin = val;
      state->pos.ردیف = state->نما.origin ? state->scrollregion_بالاترین : 0;
      state->pos.ستون = state->نما.origin ? SCROLLREGION_چپ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    اكسر;

  حالة 7:
    state->نما.autowrap = val;
    اكسر;

  حالة 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    اكسر;

  حالة 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    اكسر;

  حالة 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->نما.leftrightmargin = val;
    إذا(val) {
      صحيح ردیف;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      لكل(ردیف = 0; ردیف < state->ردیفها_2; ردیف++)
        مجموعه_lineinfo(state, ردیف, FORCE, DWL_OFF, DHL_OFF);
    }

    اكسر;

  حالة 1000:
  حالة 1002:
  حالة 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (رقم == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (رقم == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    اكسر;

  حالة 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->نما.report_focus = val;
    اكسر;

  حالة 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    اكسر;

  حالة 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    اكسر;

  حالة 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    اكسر;

  حالة 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    اكسر;

  حالة 1048:
    savecursor(state, val);
    اكسر;

  حالة 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    اكسر;

  حالة 2004:
    state->نما.bracketpaste = val;
    اكسر;

  افتراضي:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", رقم);
    أرجع;
  }
}

ساكن فراغ request_dec_نما(VTermState *state, صحيح رقم)
{
  صحيح پاسخ;

  بدل(رقم) {
    حالة 1:
      پاسخ = state->نما.cursor;
      اكسر;

    حالة 5:
      پاسخ = state->نما.screen;
      اكسر;

    حالة 6:
      پاسخ = state->نما.origin;
      اكسر;

    حالة 7:
      پاسخ = state->نما.autowrap;
      اكسر;

    حالة 12:
      پاسخ = state->نما.cursor_blink;
      اكسر;

    حالة 25:
      پاسخ = state->نما.cursor_visible;
      اكسر;

    حالة 69:
      پاسخ = state->نما.leftrightmargin;
      اكسر;

    حالة 1000:
      پاسخ = state->mouse_flags == MOUSE_WANT_CLICK;
      اكسر;

    حالة 1002:
      پاسخ = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      اكسر;

    حالة 1003:
      پاسخ = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      اكسر;

    حالة 1004:
      پاسخ = state->نما.report_focus;
      اكسر;

    حالة 1005:
      پاسخ = state->mouse_protocol == MOUSE_UTF8;
      اكسر;

    حالة 1006:
      پاسخ = state->mouse_protocol == MOUSE_SGR;
      اكسر;

    حالة 1015:
      پاسخ = state->mouse_protocol == MOUSE_RXVT;
      اكسر;

    حالة 1047:
      پاسخ = state->نما.alt_screen;
      اكسر;

    حالة 2004:
      پاسخ = state->نما.bracketpaste;
      اكسر;

    افتراضي:
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", رقم, 0);
      أرجع;
  }

  vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", رقم, پاسخ ? 1 : 2);
}

ساكن فراغ request_نسخه_string(VTermState *state)
{
  vterm_افزودن_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_نسخه_MAJOR, VTERM_نسخه_MINOR);
}

ساكن صحيح on_csi(ثابت حرف *leader, ثابت دراز آرگومانها[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user)
{
  VTermState *state = user;
  صحيح leader_byte = 0;
  صحيح intermed_byte = 0;
  صحيح cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  صحيح handled = 1;

  // Some temporaries for later code
  صحيح شمار, val;
  صحيح ردیف, ستون;
  VTermRect rect;
  صحيح selective;

  إذا(leader && leader[0]) {
    إذا(leader[1]) // longer than 1 char
      أرجع 0;

    بدل(leader[0]) {
    حالة '?':
    حالة '>':
    حالة '<':
      leader_byte = leader[0];
      اكسر;
    افتراضي:
      أرجع 0;
    }
  }

  إذا(intermed && intermed[0]) {
    إذا(intermed[1]) // longer than 1 char
      أرجع 0;

    بدل(intermed[0]) {
    حالة ' ':
    حالة '!':
    حالة '"':
    حالة '$':
    حالة '\'':
      intermed_byte = intermed[0];
      اكسر;
    افتراضي:
      أرجع 0;
    }
  }

  oldpos = state->pos;

#تعریف_کن LBOUND(v,min) إذا((v) < (min)) (v) = (min)
#تعریف_کن UBOUND(v,max) إذا((v) > (max)) (v) = (max)

#تعریف_کن LEADER(l,b) ((l << 8) | b)
#تعریف_کن INTERMED(i,b) ((i << 16) | b)

  بدل(intermed_byte << 16 | leader_byte << 8 | command) {
  حالة 0x40: // ICH - ECMA-48 8.3.64
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    إذا(!is_cursor_درون_scrollregion(state))
      اكسر;

    rect.شروع_ردیف = state->pos.ردیف;
    rect.انتها_ردیف   = state->pos.ردیف + 1;
    rect.شروع_ستون = state->pos.ستون;
    إذا(state->نما.leftrightmargin)
      rect.انتها_ستون = SCROLLREGION_راست(state);
    وإلا
      rect.انتها_ستون = THISROWWIDTH(state);

    scroll(state, rect, 0, -شمار);

    اكسر;

  حالة 0x41: // CUU - ECMA-48 8.3.22
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ردیف -= شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x42: // CUD - ECMA-48 8.3.19
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ردیف += شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x43: // CUF - ECMA-48 8.3.20
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ستون += شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x44: // CUB - ECMA-48 8.3.18
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ستون -= شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x45: // CNL - ECMA-48 8.3.12
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ستون = 0;
    state->pos.ردیف += شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x46: // CPL - ECMA-48 8.3.13
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ستون = 0;
    state->pos.ردیف -= شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(آرگومانها[0], 1);
    state->pos.ستون = val-1;
    state->at_phantom = 0;
    اكسر;

  حالة 0x48: // CUP - ECMA-48 8.3.21
    ردیف = CSI_ARG_OR(آرگومانها[0], 1);
    ستون = argcount < 2 || CSI_ARG_IS_MISSING(آرگومانها[1]) ? 1 : CSI_ARG(آرگومانها[1]);
    // zero-based
    إذا(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      إذا(state->pos.ردیف == ردیف - 1) {
        صحيح cnt, ptr = 0;
        لكل(cnt = 0; cnt < ستون - 1; ++cnt) {
	  VTermPos p;
	  VTermScreenCell c0, c1;
	  p.ردیف = ردیف - 1;
	  p.ستون = ptr;
	  vterm_screen_get_cell(state->vt->screen, p, &c0);
	  p.ستون++;
	  vterm_screen_get_cell(state->vt->screen, p, &c1);
	  ptr += (c1.chars[0] == (uint32_t)-1)		    // double cell?
	     ? (vterm_unicode_is_ambiguous(c0.chars[0]))    // is ambiguous?
	     ? vterm_unicode_عرض(0x00a1) : 1		    // &ambiwidth
	     : 1;					    // not ambiguous
        }
        ستون = ptr + 1;
      }
    }
    state->pos.ردیف = ردیف-1;
    state->pos.ستون = ستون-1;
    إذا(state->نما.origin) {
      state->pos.ردیف += state->scrollregion_بالاترین;
      state->pos.ستون += SCROLLREGION_چپ(state);
    }
    state->at_phantom = 0;
    اكسر;

  حالة 0x49: // CHT - ECMA-48 8.3.10
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    tab(state, شمار, +1);
    اكسر;

  حالة 0x4a: // ED - ECMA-48 8.3.39
  حالة LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    بدل(CSI_ARG(آرگومانها[0])) {
    حالة CSI_ARG_MISSING:
    حالة 0:
      rect.شروع_ردیف = state->pos.ردیف; rect.انتها_ردیف = state->pos.ردیف + 1;
      rect.شروع_ستون = state->pos.ستون; rect.انتها_ستون = state->تعداد_ستون;
      إذا(rect.انتها_ستون > rect.شروع_ستون)
        erase(state, rect, selective);

      rect.شروع_ردیف = state->pos.ردیف + 1; rect.انتها_ردیف = state->ردیفها_2;
      rect.شروع_ستون = 0;
      لكل(ردیف = rect.شروع_ردیف; ردیف < rect.انتها_ردیف; ردیف++)
	مجموعه_lineinfo(state, ردیف, FORCE, DWL_OFF, DHL_OFF);
      إذا(rect.انتها_ردیف > rect.شروع_ردیف)
        erase(state, rect, selective);
      اكسر;

    حالة 1:
      rect.شروع_ردیف = 0; rect.انتها_ردیف = state->pos.ردیف;
      rect.شروع_ستون = 0; rect.انتها_ستون = state->تعداد_ستون;
      لكل(ردیف = rect.شروع_ردیف; ردیف < rect.انتها_ردیف; ردیف++)
	مجموعه_lineinfo(state, ردیف, FORCE, DWL_OFF, DHL_OFF);
      إذا(rect.انتها_ستون > rect.شروع_ستون)
        erase(state, rect, selective);

      rect.شروع_ردیف = state->pos.ردیف; rect.انتها_ردیف = state->pos.ردیف + 1;
                          rect.انتها_ستون = state->pos.ستون + 1;
      إذا(rect.انتها_ردیف > rect.شروع_ردیف)
        erase(state, rect, selective);
      اكسر;

    حالة 2:
      rect.شروع_ردیف = 0; rect.انتها_ردیف = state->ردیفها_2;
      rect.شروع_ستون = 0; rect.انتها_ستون = state->تعداد_ستون;
      لكل(ردیف = rect.شروع_ردیف; ردیف < rect.انتها_ردیف; ردیف++)
	مجموعه_lineinfo(state, ردیف, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      اكسر;

    حالة 3:
      إذا(state->callbacks && state->callbacks->sb_clear)
        إذا((*state->callbacks->sb_clear)(state->cbdata))
          أرجع 1;
      اكسر;
    }
    اكسر;

  حالة 0x4b: // EL - ECMA-48 8.3.41
  حالة LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.شروع_ردیف = state->pos.ردیف;
    rect.انتها_ردیف   = state->pos.ردیف + 1;

    بدل(CSI_ARG(آرگومانها[0])) {
    حالة CSI_ARG_MISSING:
    حالة 0:
      rect.شروع_ستون = state->pos.ستون; rect.انتها_ستون = THISROWWIDTH(state); اكسر;
    حالة 1:
      rect.شروع_ستون = 0; rect.انتها_ستون = state->pos.ستون + 1; اكسر;
    حالة 2:
      rect.شروع_ستون = 0; rect.انتها_ستون = THISROWWIDTH(state); اكسر;
    افتراضي:
      أرجع 0;
    }

    إذا(rect.انتها_ستون > rect.شروع_ستون)
      erase(state, rect, selective);

    اكسر;

  حالة 0x4c: // IL - ECMA-48 8.3.67
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    إذا(!is_cursor_درون_scrollregion(state))
      اكسر;

    rect.شروع_ردیف = state->pos.ردیف;
    rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_ستون = SCROLLREGION_چپ(state);
    rect.انتها_ستون   = SCROLLREGION_راست(state);

    scroll(state, rect, -شمار, 0);

    اكسر;

  حالة 0x4d: // DL - ECMA-48 8.3.32
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    إذا(!is_cursor_درون_scrollregion(state))
      اكسر;

    rect.شروع_ردیف = state->pos.ردیف;
    rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_ستون = SCROLLREGION_چپ(state);
    rect.انتها_ستون   = SCROLLREGION_راست(state);

    scroll(state, rect, شمار, 0);

    اكسر;

  حالة 0x50: // DCH - ECMA-48 8.3.26
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    إذا(!is_cursor_درون_scrollregion(state))
      اكسر;

    rect.شروع_ردیف = state->pos.ردیف;
    rect.انتها_ردیف   = state->pos.ردیف + 1;
    rect.شروع_ستون = state->pos.ستون;
    إذا(state->نما.leftrightmargin)
      rect.انتها_ستون = SCROLLREGION_راست(state);
    وإلا
      rect.انتها_ستون = THISROWWIDTH(state);

    scroll(state, rect, 0, شمار);

    اكسر;

  حالة 0x53: // SU - ECMA-48 8.3.147
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    rect.شروع_ردیف = state->scrollregion_بالاترین;
    rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_ستون = SCROLLREGION_چپ(state);
    rect.انتها_ستون   = SCROLLREGION_راست(state);

    scroll(state, rect, شمار, 0);

    اكسر;

  حالة 0x54: // SD - ECMA-48 8.3.113
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    rect.شروع_ردیف = state->scrollregion_بالاترین;
    rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_ستون = SCROLLREGION_چپ(state);
    rect.انتها_ستون   = SCROLLREGION_راست(state);

    scroll(state, rect, -شمار, 0);

    اكسر;

  حالة 0x58: // ECH - ECMA-48 8.3.38
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    rect.شروع_ردیف = state->pos.ردیف;
    rect.انتها_ردیف   = state->pos.ردیف + 1;
    rect.شروع_ستون = state->pos.ستون;
    rect.انتها_ستون   = state->pos.ستون + شمار;
    UBOUND(rect.انتها_ستون, THISROWWIDTH(state));

    erase(state, rect, 0);
    اكسر;

  حالة 0x5a: // CBT - ECMA-48 8.3.7
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    tab(state, شمار, -1);
    اكسر;

  حالة 0x60: // HPA - ECMA-48 8.3.57
    ستون = CSI_ARG_OR(آرگومانها[0], 1);
    state->pos.ستون = ستون-1;
    state->at_phantom = 0;
    اكسر;

  حالة 0x61: // HPR - ECMA-48 8.3.59
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ستون += شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x62: { // REP - ECMA-48 8.3.103
    ثابت صحيح ردیف_عرض = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    إذا(state->combine_عرض < 1)
      اكسر;

    شمار = CSI_ARG_شمار(آرگومانها[0]);
    ستون = state->pos.ستون + شمار;
    UBOUND(ستون, ردیف_عرض);
    طالما (state->pos.ستون < ستون) {
      putglyph(state, state->combine_chars, state->combine_عرض, state->pos);
      state->pos.ستون += state->combine_عرض;
    }
    إذا (state->pos.ستون + state->combine_عرض >= ردیف_عرض) {
      إذا (state->نما.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    اكسر;
  }

  حالة 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(آرگومانها[0], 0);
    إذا(val == 0)
      // DEC VT100 response
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    اكسر;

  حالة LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    اكسر;

  حالة 0x64: // VPA - ECMA-48 8.3.158
    ردیف = CSI_ARG_OR(آرگومانها[0], 1);
    state->pos.ردیف = ردیف-1;
    إذا(state->نما.origin)
      state->pos.ردیف += state->scrollregion_بالاترین;
    state->at_phantom = 0;
    اكسر;

  حالة 0x65: // VPR - ECMA-48 8.3.160
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ردیف += شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x66: // HVP - ECMA-48 8.3.63
    ردیف = CSI_ARG_OR(آرگومانها[0], 1);
    ستون = argcount < 2 || CSI_ARG_IS_MISSING(آرگومانها[1]) ? 1 : CSI_ARG(آرگومانها[1]);
    // zero-based
    state->pos.ردیف = ردیف-1;
    state->pos.ستون = ستون-1;
    إذا(state->نما.origin) {
      state->pos.ردیف += state->scrollregion_بالاترین;
      state->pos.ستون += SCROLLREGION_چپ(state);
    }
    state->at_phantom = 0;
    اكسر;

  حالة 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(آرگومانها[0], 0);

    بدل(val) {
    حالة 0:
      clear_ستون_tabstop(state, state->pos.ستون);
      اكسر;
    حالة 3:
    حالة 5:
      لكل(ستون = 0; ستون < state->تعداد_ستون; ستون++)
        clear_ستون_tabstop(state, ستون);
      اكسر;
    حالة 1:
    حالة 2:
    حالة 4:
      اكسر;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    افتراضي:
      أرجع 0;
    }
    اكسر;

  حالة 0x68: // SM - ECMA-48 8.3.125
    إذا(!CSI_ARG_IS_MISSING(آرگومانها[0]))
      مجموعه_نما(state, CSI_ARG(آرگومانها[0]), 1);
    اكسر;

  حالة LEADER('?', 0x68): // DEC private mode set
    لكل(صحيح i = 0; i < argcount; i++) {
      إذا(!CSI_ARG_IS_MISSING(آرگومانها[i]))
        مجموعه_dec_نما(state, CSI_ARG(آرگومانها[i]), 1);
    }
    اكسر;

  حالة 0x6a: // HPB - ECMA-48 8.3.58
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ستون -= شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6b: // VPB - ECMA-48 8.3.159
    شمار = CSI_ARG_شمار(آرگومانها[0]);
    state->pos.ردیف -= شمار;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6c: // RM - ECMA-48 8.3.106
    إذا(!CSI_ARG_IS_MISSING(آرگومانها[0]))
      مجموعه_نما(state, CSI_ARG(آرگومانها[0]), 0);
    اكسر;

  حالة LEADER('?', 0x6c): // DEC private mode reset
    لكل(صحيح i = 0; i < argcount; i++) {
      إذا(!CSI_ARG_IS_MISSING(آرگومانها[i]))
        مجموعه_dec_نما(state, CSI_ARG(آرگومانها[i]), 0);
    }
    اكسر;

  حالة 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, آرگومانها, argcount);
    اكسر;

  حالة LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    إذا (argcount == 1 && CSI_ARG(آرگومانها[0]) == 4)
    {
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->نما.modify_other_کلیدها ? 2 : 0);
      اكسر;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    لكل(صحيح argi = 0; argi < argcount; argi++) {
      دراز arg;
      بدل(arg = CSI_ARG(آرگومانها[argi])) {
        حالة 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          اكسر;
        حالة 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          اكسر;
        حالة 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          اكسر;
      }
    }
    اكسر;

  حالة LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    إذا (argcount == 2 && CSI_ARG(آرگومانها[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->نما.kitty_keyboard = 0;

      state->نما.modify_other_کلیدها = CSI_ARG(آرگومانها[1]) == 2;
    }
    اكسر;

  حالة LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    إذا (argcount == 1 && CSI_ARG(آرگومانها[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->نما.modify_other_کلیدها = 0;

      state->نما.kitty_keyboard = 1;
    }
    اكسر;

  حالة LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    إذا (argcount <= 1)
      state->نما.kitty_keyboard = 0;
    اكسر;

  حالة LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    إذا (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->نما.kitty_keyboard);
    اكسر;

  حالة 0x6e: // DSR - ECMA-48 8.3.35
  حالة LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(آرگومانها[0], 0);

    {
      حرف *qmark = (leader_byte == '?') ? "?" : "";

      بدل(val) {
      حالة 0: حالة 1: حالة 2: حالة 3: حالة 4:
        // ignore - these are replies
        اكسر;
      حالة 5:
        vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        اكسر;
      حالة 6: // CPR - cursor position report
        vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.ردیف + 1, state->pos.ستون + 1);
        اكسر;
      }
    }
    اكسر;


  حالة INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    اكسر;

  حالة LEADER('?', INTERMED('$', 0x70)):
    request_dec_نما(state, CSI_ARG(آرگومانها[0]));
    اكسر;

  حالة LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_نسخه_string(state);
    اكسر;

  حالة INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(آرگومانها[0], 1);

    بدل(val) {
    حالة 0: حالة 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_بلوک);
      اكسر;
    حالة 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_بلوک);
      اكسر;
    حالة 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      اكسر;
    حالة 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      اكسر;
    حالة 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_چپ);
      اكسر;
    حالة 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_چپ);
      اكسر;
    }

    اكسر;

  حالة INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(آرگومانها[0], 0);

    بدل(val) {
    حالة 0: حالة 2:
      state->protected_cell = 0;
      اكسر;
    حالة 1:
      state->protected_cell = 1;
      اكسر;
    }

    اكسر;

  حالة 0x72: // DECSTBM - DEC custom
    state->scrollregion_بالاترین = CSI_ARG_OR(آرگومانها[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(آرگومانها[1]) ? -1 : CSI_ARG(آرگومانها[1]);
    LBOUND(state->scrollregion_بالاترین, 0);
    UBOUND(state->scrollregion_بالاترین, state->ردیفها_2);
    LBOUND(state->scrollregion_bottom, -1);
    إذا(state->scrollregion_بالاترین == 0 && state->scrollregion_bottom == state->ردیفها_2)
      state->scrollregion_bottom = -1;
    وإلا
      UBOUND(state->scrollregion_bottom, state->ردیفها_2);

    إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_بالاترین) {
      // Invalid
      state->scrollregion_بالاترین    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.ردیف = 0;
    state->pos.ستون = 0;
    إذا(state->نما.origin) {
      state->pos.ردیف += state->scrollregion_بالاترین;
      state->pos.ستون += SCROLLREGION_چپ(state);
    }

    اكسر;

  حالة 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_چپ = CSI_ARG_OR(آرگومانها[0], 1) - 1;
    state->scrollregion_راست = argcount < 2 || CSI_ARG_IS_MISSING(آرگومانها[1]) ? -1 : CSI_ARG(آرگومانها[1]);
    LBOUND(state->scrollregion_چپ, 0);
    UBOUND(state->scrollregion_چپ, state->تعداد_ستون);
    LBOUND(state->scrollregion_راست, -1);
    إذا(state->scrollregion_چپ == 0 && state->scrollregion_راست == state->تعداد_ستون)
      state->scrollregion_راست = -1;
    وإلا
      UBOUND(state->scrollregion_راست, state->تعداد_ستون);

    إذا(state->scrollregion_راست > -1 &&
       state->scrollregion_راست <= state->scrollregion_چپ) {
      // Invalid
      state->scrollregion_چپ  = 0;
      state->scrollregion_راست = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.ردیف = 0;
    state->pos.ستون = 0;
    إذا(state->نما.origin) {
      state->pos.ردیف += state->scrollregion_بالاترین;
      state->pos.ستون += SCROLLREGION_چپ(state);
    }

    اكسر;

  حالة 0x74:
    بدل(CSI_ARG(آرگومانها[0])) {
      حالة 8: // CSI 8 ; rows ; cols t  set size
        إذا (argcount == 3 &&
            !CSI_ARG_IS_MISSING(آرگومانها[1]) && !CSI_ARG_IS_MISSING(آرگومانها[2]) &&
            CSI_ARG(آرگومانها[1]) > 0 && CSI_ARG(آرگومانها[2]) > 0)
	  on_resize(CSI_ARG(آرگومانها[1]), CSI_ARG(آرگومانها[2]), state);
	اكسر;
      افتراضي:
	handled = 0;
	اكسر;
    }
    اكسر;

  حالة INTERMED('\'', 0x7D): // DECIC
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    إذا(!is_cursor_درون_scrollregion(state))
      اكسر;

    rect.شروع_ردیف = state->scrollregion_بالاترین;
    rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_ستون = state->pos.ستون;
    rect.انتها_ستون   = SCROLLREGION_راست(state);

    scroll(state, rect, 0, -شمار);

    اكسر;

  حالة INTERMED('\'', 0x7E): // DECDC
    شمار = CSI_ARG_شمار(آرگومانها[0]);

    إذا(!is_cursor_درون_scrollregion(state))
      اكسر;

    rect.شروع_ردیف = state->scrollregion_بالاترین;
    rect.انتها_ردیف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_ستون = state->pos.ستون;
    rect.انتها_ستون   = SCROLLREGION_راست(state);

    scroll(state, rect, 0, شمار);

    اكسر;

  افتراضي:
    handled = 0;
    اكسر;
  }

  إذا (!handled) {
    إذا(state->fallbacks && state->fallbacks->csi)
      إذا((*state->fallbacks->csi)(leader, آرگومانها, argcount, intermed, command, state->fbdata))
        أرجع 1;

    أرجع 0;
  }

  إذا(state->نما.origin) {
    LBOUND(state->pos.ردیف, state->scrollregion_بالاترین);
    UBOUND(state->pos.ردیف, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.ستون, SCROLLREGION_چپ(state));
    UBOUND(state->pos.ستون, SCROLLREGION_راست(state)-1);
  }
  وإلا {
    LBOUND(state->pos.ردیف, 0);
    UBOUND(state->pos.ردیف, state->ردیفها_2-1);
    LBOUND(state->pos.ستون, 0);
    UBOUND(state->pos.ستون, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#اگر_تعریف_شده DEBUG
  إذا(state->pos.ردیف < 0 || state->pos.ردیف >= state->ردیفها_2 ||
     state->pos.ستون < 0 || state->pos.ستون >= state->تعداد_ستون) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.ردیف, state->pos.ستون);
    abort();
  }

  إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_بالاترین) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_بالاترین);
    abort();
  }

  إذا(SCROLLREGION_راست(state) <= SCROLLREGION_چپ(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_راست(state), SCROLLREGION_چپ(state));
    abort();
  }
#پایان_اگر

  أرجع 1;
}

ساكن حرف base64_one(uint8_t b)
{
  إذا(b < 26)
    أرجع 'A' + b;
  وإلا إذا(b < 52)
    أرجع 'a' + b - 26;
  وإلا إذا(b < 62)
    أرجع '0' + b - 52;
  وإلا إذا(b == 62)
    أرجع '+';
  وإلا إذا(b == 63)
    أرجع '/';
  أرجع 0;
}

ساكن uint8_t unbase64one(حرف c)
{
  إذا(c >= 'A' && c <= 'Z')
    أرجع c - 'A';
  وإلا إذا(c >= 'a' && c <= 'z')
    أرجع c - 'a' + 26;
  وإلا إذا(c >= '0' && c <= '9')
    أرجع c - '0' + 52;
  وإلا إذا(c == '+')
    أرجع 62;
  وإلا إذا(c == '/')
    أرجع 63;

  أرجع 0xFF;
}

ساكن فراغ osc_selection(VTermState *state, VTermStringFragment frag)
{
  إذا(frag.initial) {
    state->tmp.selection.ماسک = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  طالما(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    بدل(frag.str[0]) {
      حالة 'c':
        state->tmp.selection.ماسک |= VTERM_SELECTION_CLIPBOARD;
        اكسر;
      حالة 'p':
        state->tmp.selection.ماسک |= VTERM_SELECTION_PRIMARY;
        اكسر;
      حالة 'q':
        state->tmp.selection.ماسک |= VTERM_SELECTION_SECONDARY;
        اكسر;
      حالة 's':
        state->tmp.selection.ماسک |= VTERM_SELECTION_SELECT;
        اكسر;
      حالة '0':
      حالة '1':
      حالة '2':
      حالة '3':
      حالة '4':
      حالة '5':
      حالة '6':
      حالة '7':
        state->tmp.selection.ماسک |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        اكسر;

      حالة ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        إذا(!state->tmp.selection.ماسک)
          state->tmp.selection.ماسک = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        اكسر;
    }

    frag.str++;
    frag.len--;
  }

  إذا(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    إذا(frag.final && state->selection.callbacks->مجموعه_2) {
      (*state->selection.callbacks->مجموعه_2)(state->tmp.selection.ماسک, (VTermStringFragment){
              .str     = عدم,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_مجموعه,
              .final   = TRUE,
            }, state->selection.user);
    }
    أرجع;
  }

  إذا(state->tmp.selection.state == SELECTION_SELECTED) {
    إذا(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    وإلا {
      state->tmp.selection.state = SELECTION_مجموعه_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  إذا(state->tmp.selection.state == SELECTION_QUERY) {
    إذا(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.ماسک, state->selection.user);
    أرجع;
  }

  إذا(state->tmp.selection.state == SELECTION_INVALID)
    أرجع;

  إذا(state->selection.callbacks->مجموعه_2) {
    size_t bufcur = 0;
    حرف *میانگیر = state->selection.میانگیر;

    uint32_t x = 0; /* Current decoding value */
    صحيح n = 0;      /* Number of sextets consumed */

    إذا(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    طالما((state->selection.buflen - bufcur) >= 3 && frag.len) {
      إذا(frag.str[0] == '=') {
        إذا(n == 2) {
          میانگیر[0] = (x >> 4) & 0xFF;
          میانگیر += 1, bufcur += 1;
        }
        إذا(n == 3) {
          میانگیر[0] = (x >> 10) & 0xFF;
          میانگیر[1] = (x >>  2) & 0xFF;
          میانگیر += 2, bufcur += 2;
        }

        طالما(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      وإلا {
        uint8_t b = unbase64one(frag.str[0]);
        إذا(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          إذا(state->selection.callbacks->مجموعه_2) {
            (*state->selection.callbacks->مجموعه_2)(state->tmp.selection.ماسک, (VTermStringFragment){
                .str     = عدم,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          اكسر;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        إذا(n == 4) {
          میانگیر[0] = (x >> 16) & 0xFF;
          میانگیر[1] = (x >>  8) & 0xFF;
          میانگیر[2] = (x >>  0) & 0xFF;

          میانگیر += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      إذا(!frag.len || (state->selection.buflen - bufcur) < 3) {
        إذا(bufcur) {
	  VTermStringFragment setfrag = {
	    state->selection.میانگیر, // str
	    bufcur, // len
	    state->tmp.selection.state == SELECTION_مجموعه_INITIAL, // initial
	    frag.final && !frag.len // final
	  };
          (*state->selection.callbacks->مجموعه_2)(state->tmp.selection.ماسک,
	      setfrag, state->selection.user);
          state->tmp.selection.state = SELECTION_مجموعه;
        }

        میانگیر = state->selection.میانگیر;
        bufcur = 0;
      }
    }

    إذا(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

ساكن صحيح on_osc(صحيح command, VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  بدل(command) {
    حالة 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      أرجع 1;

    حالة 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      أرجع 1;

    حالة 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      أرجع 1;

    حالة 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        صحيح red = state->default_fg.red;
        صحيح blue = state->default_fg.blue;
        صحيح green = state->default_fg.green;
        vterm_افزودن_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        أرجع 1;
      }

    حالة 11:
      {
	// request background color: <Esc>]11;?<0x07>
	صحيح red = state->default_bg.red;
	صحيح blue = state->default_bg.blue;
	صحيح green = state->default_bg.green;
	vterm_افزودن_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	أرجع 1;
      }
    حالة 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      أرجع 1;

    حالة 52:
      إذا(state->selection.callbacks)
        osc_selection(state, frag);

      أرجع 1;

    افتراضي:
      إذا(state->fallbacks && state->fallbacks->osc)
        إذا((*state->fallbacks->osc)(command, frag, state->fbdata))
          أرجع 1;
  }

  أرجع 0;
}

ساكن فراغ request_وضعیت_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  حرف *tmp = state->tmp.decrqss;

  إذا(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  طالما(i < حجم(state->tmp.decrqss)-1 && tmp[i])
    i++;
  طالما(i < حجم(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  إذا(!frag.final)
    أرجع;

  بدل(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    حالة 'm': {
      // Query SGR
      دراز آرگومانها[20];
      صحيح argc = vterm_state_getpen(state, آرگومانها, حجم(آرگومانها)/حجم(آرگومانها[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->نما.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      إذا(cur >= vt->tmpbuffer_len)
        أرجع;

      لكل(صحيح argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_بیشتر(آرگومانها[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(آرگومانها[argi]));
        إذا(cur >= vt->tmpbuffer_len)
          أرجع;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->نما.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      إذا(cur >= vt->tmpbuffer_len)
        أرجع;

      vterm_افزودن_output_بایتها(vt, vt->tmpbuffer, cur);
      أرجع;
    }

    حالة 'r':
      // Query DECSTBM
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_بالاترین+1, SCROLLREGION_BOTTOM(state));
      أرجع;

    حالة 's':
      // Query DECSLRM
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_چپ(state)+1, SCROLLREGION_راست(state));
      أرجع;

    حالة ' '|('q'<<8): {
      // Query DECSCUSR
      صحيح پاسخ;
      بدل(state->نما.cursor_shape) {
        حالة VTERM_PROP_CURSORSHAPE_بلوک:     پاسخ = 2; اكسر;
        حالة VTERM_PROP_CURSORSHAPE_UNDERLINE: پاسخ = 4; اكسر;
	افتراضي: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ پاسخ = 6; اكسر;
      }
      إذا(state->نما.cursor_blink)
        پاسخ--;
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", پاسخ);
      أرجع;
    }

    حالة '\"'|('q'<<8):
      // Query DECSCA
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      أرجع;
  }

  vterm_افزودن_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ساكن صحيح on_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(commandlen == 2 && strneq(command, "$q", 2)) {
    request_وضعیت_string(state, frag);
    أرجع 1;
  }
  وإلا إذا(state->fallbacks && state->fallbacks->dcs)
    إذا((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      أرجع 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (صحيح)commandlen, command);
  أرجع 0;
}

ساكن صحيح on_apc(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(state->fallbacks && state->fallbacks->apc)
    إذا((*state->fallbacks->apc)(frag, state->fbdata))
      أرجع 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  أرجع 0;
}

ساكن صحيح on_pm(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(state->fallbacks && state->fallbacks->pm)
    إذا((*state->fallbacks->pm)(frag, state->fbdata))
      أرجع 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  أرجع 0;
}

ساكن صحيح on_sos(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(state->fallbacks && state->fallbacks->sos)
    إذا((*state->fallbacks->sos)(frag, state->fbdata))
      أرجع 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  أرجع 0;
}

ساكن صحيح on_resize(صحيح ردیفها_2, صحيح تعداد_ستون, فراغ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  إذا (تعداد_ستون > VTERM_MAX_تعداد_ستون)
    تعداد_ستون = VTERM_MAX_تعداد_ستون;

  إذا (ردیفها_2 > VTERM_MAX_ردیفها)
    ردیفها_2 = VTERM_MAX_ردیفها;

  إذا(تعداد_ستون != state->تعداد_ستون) {
    بیعلامت حرف *newtabstops = vterm_allocator_malloc(state->vt, (تعداد_ستون + 7) / 8);
    إذا (newtabstops == عدم)
      أرجع 0;

    /* TODO: This can all be done much more efficiently bytewise */
    صحيح ستون;
    لكل(ستون = 0; ستون < state->تعداد_ستون && ستون < تعداد_ستون; ستون++) {
      بیعلامت حرف ماسک = 1 << (ستون & 7);
      إذا(state->tabstops[ستون >> 3] & ماسک)
        newtabstops[ستون >> 3] |= ماسک;
      وإلا
        newtabstops[ستون >> 3] &= ~ماسک;
      }

    لكل( ; ستون < تعداد_ستون; ستون++) {
      بیعلامت حرف ماسک = 1 << (ستون & 7);
      إذا(ستون % 8 == 0)
        newtabstops[ستون >> 3] |= ماسک;
      وإلا
        newtabstops[ستون >> 3] &= ~ماسک;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->ردیفها_2 = ردیفها_2;
  state->تعداد_ستون = تعداد_ستون;

  إذا(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->ردیفها_2);
  إذا(state->scrollregion_راست > -1)
    UBOUND(state->scrollregion_راست, state->تعداد_ستون);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_بالاترین, state->ردیفها_2);
  UBOUND(state->scrollregion_چپ, state->تعداد_ستون);
  إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_بالاترین) {
    state->scrollregion_بالاترین = 0;
    state->scrollregion_bottom = -1;
  }
  إذا(SCROLLREGION_راست(state) <= state->scrollregion_چپ) {
    state->scrollregion_چپ = 0;
    state->scrollregion_راست = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  إذا(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(ردیفها_2, تعداد_ستون, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  وإلا {
    إذا(ردیفها_2 != state->ردیفها_2) {
      لكل(صحيح bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_سطر_Info *oldlineinfo = state->lineinfos[bufidx];
        إذا(!oldlineinfo)
          استمر;

        VTerm_سطر_Info *newlineinfo = vterm_allocator_malloc(state->vt, ردیفها_2 * حجم(VTerm_سطر_Info));

        صحيح ردیف;
        لكل(ردیف = 0; ردیف < state->ردیفها_2 && ردیف < ردیفها_2; ردیف++) {
          newlineinfo[ردیف] = oldlineinfo[ردیف];
        }

        لكل( ; ردیف < ردیفها_2; ردیف++) {
          newlineinfo[ردیف] = (VTerm_سطر_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->نما.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  إذا(state->at_phantom && state->pos.ستون < تعداد_ستون-1) {
    state->at_phantom = 0;
    state->pos.ستون++;
  }

  إذا(state->pos.ردیف < 0)
    state->pos.ردیف = 0;
  إذا(state->pos.ردیف >= ردیفها_2)
    state->pos.ردیف = ردیفها_2 - 1;
  إذا(state->pos.ستون < 0)
    state->pos.ستون = 0;
  إذا(state->pos.ستون >= تعداد_ستون)
    state->pos.ستون = تعداد_ستون - 1;

  updatecursor(state, &oldpos, 1);

  أرجع 1;
}

ساكن ثابت VTermParserCallbacks parser_callbacks = {
  on_متن, //متن
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
  إذا(vt->state)
    أرجع vt->state;

  VTermState *state = vterm_state_new(vt);
  إذا (state == عدم)
    أرجع عدم;
  vt->state = state;

  vterm_parser_مجموعه_callbacks(vt, &parser_callbacks, state);

  أرجع state;
}

فراغ vterm_state_reset(VTermState *state, صحيح hard)
{
  state->scrollregion_بالاترین = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_چپ = 0;
  state->scrollregion_راست = -1;

  state->نما.keypad          = 0;
  state->نما.cursor          = 0;
  state->نما.autowrap        = 1;
  state->نما.درج          = 0;
  state->نما.newline         = 0;
  state->نما.alt_screen      = 0;
  state->نما.origin          = 0;
  state->نما.leftrightmargin = 0;
  state->نما.bracketpaste    = 0;
  state->نما.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->نما.ctrl8bit   = 0;

  لكل(صحيح ستون = 0; ستون < state->تعداد_ستون; ستون++)
    إذا(ستون % 8 == 0)
      مجموعه_ستون_tabstop(state, ستون);
    وإلا
      clear_ستون_tabstop(state, ستون);

  لكل(صحيح ردیف = 0; ردیف < state->ردیفها_2; ردیف++)
    مجموعه_lineinfo(state, ردیف, FORCE, DWL_OFF, DHL_OFF);

  إذا(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->نما.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  لكل(صحيح i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    إذا(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].داده);
  }

  state->gl_مجموعه = 0;
  state->gr_مجموعه = 1;
  state->gsingle_مجموعه = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_بلوک);

  إذا(hard) {
    state->pos.ردیف = 0;
    state->pos.ستون = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.انتها_ردیف = state->ردیفها_2;
    rect.انتها_ستون =  state->تعداد_ستون;
    erase(state, rect, 0);
  }
}

فراغ vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

فراغ vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.ستون = state->mouse_ستون;
  mousestate->pos.ردیف = state->mouse_ردیف;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

فراغ vterm_state_مجموعه_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, فراغ *user)
{
  إذا(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    إذا(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  وإلا {
    state->callbacks = عدم;
    state->cbdata = عدم;
  }
}

فراغ vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

فراغ *vterm_state_get_cbdata(VTermState *state)
{
  أرجع state->cbdata;
}

فراغ vterm_state_مجموعه_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
{
  إذا(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  وإلا {
    state->fallbacks = عدم;
    state->fbdata = عدم;
  }
}

فراغ *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  أرجع state->fbdata;
}

صحيح vterm_state_مجموعه_termprop(VTermState *state, VTermProp prop, VTerm_مقدار *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  إذا(state->callbacks && state->callbacks->settermprop)
    إذا(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      أرجع 0;

  بدل(prop) {
  حالة VTERM_PROP_TITLE:
  حالة VTERM_PROP_ICONNAME:
  حالة VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    أرجع 1;
  حالة VTERM_PROP_CURSORVISIBLE:
    state->نما.cursor_visible = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_CURSORBLINK:
    state->نما.cursor_blink = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_CURSORSHAPE:
    state->نما.cursor_shape = val->عدد;
    أرجع 1;
  حالة VTERM_PROP_معکوس:
    state->نما.screen = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_ALTSCREEN:
    state->نما.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->نما.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    إذا(state->نما.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.انتها_ردیف = state->ردیفها_2;
      rect.انتها_ستون = state->تعداد_ستون;
      erase(state, rect, 0);
    }
    أرجع 1;
  حالة VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    إذا(val->عدد)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    إذا(val->عدد == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    إذا(val->عدد == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    أرجع 1;
  حالة VTERM_PROP_FOCUSREPORT:
    state->نما.report_focus = val->boolean;
    أرجع 1;

  حالة VTERM_N_PROPS:
    أرجع 0;
  }

  أرجع 0;
}

فراغ vterm_state_focus_درون(VTermState *state)
{
  إذا(state->نما.report_focus)
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

فراغ vterm_state_focus_بیرون(VTermState *state)
{
  إذا(state->نما.report_focus)
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ثابت VTerm_سطر_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحيح ردیف)
{
  أرجع state->lineinfo + ردیف;
}

فراغ vterm_state_مجموعه_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, فراغ *user,
    حرف *میانگیر, size_t buflen)
{
  إذا(buflen && !میانگیر)
    میانگیر = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.میانگیر    = میانگیر;
  state->selection.buflen    = buflen;
}

فراغ vterm_state_send_selection(VTermState *state, VTerm_Selection_ماسک ماسک, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  إذا(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ساكن حرف selection_chars[] = "cpqs";
    صحيح idx;
    لكل(idx = 0; idx < 4; idx++)
      إذا(ماسک & (1 << idx))
        اكسر;

    vterm_افزودن_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  إذا(frag.len) {
    size_t bufcur = 0;
    حرف *میانگیر = state->selection.میانگیر;

    uint32_t x = 0;
    صحيح n = 0;

    إذا(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    طالما((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      إذا(n == 3) {
        میانگیر[0] = base64_one((x >> 18) & 0x3F);
        میانگیر[1] = base64_one((x >> 12) & 0x3F);
        میانگیر[2] = base64_one((x >>  6) & 0x3F);
        میانگیر[3] = base64_one((x >>  0) & 0x3F);

        میانگیر += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      إذا(!frag.len || (state->selection.buflen - bufcur) < 4) {
        إذا(bufcur)
          vterm_افزودن_output_بایتها(vt, state->selection.میانگیر, bufcur);

        میانگیر = state->selection.میانگیر;
        bufcur = 0;
      }
    }

    إذا(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  إذا(frag.final) {
    إذا(state->tmp.selection.sendpartial) {
      صحيح n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      حرف *میانگیر = state->selection.میانگیر;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      میانگیر[0] = base64_one((x >> 18) & 0x3F);
      میانگیر[1] = base64_one((x >> 12) & 0x3F);
      میانگیر[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      میانگیر[3] = '=';

      vterm_افزودن_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, میانگیر);
    }
    وإلا
      vterm_افزودن_output_sprintf_str(vt, 0, TRUE, "");
  }
}
