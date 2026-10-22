#شامل "vterm_internal.h"

#شامل <stdio.h>
#شامل <string.h>

#تعریف_کرو strneq(a,b,n) (لڑی_موازنہ(a,b,n)==0)

#اگر defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#اگر_ختم

ساکن صحیح on_resize(صحیح صفیں_2, صحیح ستون_تعداد, خالی *user);

/* Some convenient wrappers to make callback functions easier */

ساکن خالی putglyph(VTermState *state, ثابت uint32_t chars[], صحیح چوڑائی, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.چوڑائی = چوڑائی;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.صف].doublewidth;
  info.dhl = state->lineinfo[pos.صف].doubleheight;

  اگر(state->callbacks && state->callbacks->putglyph)
    اگر((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      واپس;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.ستون, pos.صف);
}

ساکن خالی updatecursor(VTermState *state, VTermPos *oldpos, صحیح cancel_phantom)
{
  اگر(state->pos.ستون == oldpos->ستون && state->pos.صف == oldpos->صف)
    واپس;

  اگر(cancel_phantom)
    state->at_phantom = 0;

  اگر(state->callbacks && state->callbacks->movecursor)
    اگر((*state->callbacks->movecursor)(state->pos, *oldpos, state->منوال.cursor_visible, state->cbdata))
      واپس;
}

ساکن خالی erase(VTermState *state, VTermRect rect, صحیح selective)
{
  اگر(rect.انتہا_ستون == state->ستون_تعداد) {
    صحیح صف;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    برائے(صف = rect.آغاز_صف + 1; صف < rect.انتہا_صف + 1 && صف < state->صفیں_2; صف++)
      state->lineinfo[صف].continuation = 0;
  }

  اگر(state->callbacks && state->callbacks->erase)
    اگر((*state->callbacks->erase)(rect, selective, state->cbdata))
      واپس;
}

ساکن VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, حجم(VTermState));

  اگر (state == تہی)
    واپس تہی;
  state->vt = vt;

  state->صفیں_2 = vt->صفیں_2;
  state->ستون_تعداد = vt->ستون_تعداد;

  state->mouse_ستون     = 0;
  state->mouse_صف     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = تہی;
  state->cbdata    = تہی;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = تہی;
  state->selection.user      = تہی;
  state->selection.بفر    = تہی;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_سائز = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_سائز * حجم(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->ستون_تعداد + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->صفیں_2 * حجم(VTerm_سطر_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->صفیں_2 * حجم(VTerm_سطر_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  اگر(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.ڈیٹا);

  واپس state;
}

اندرونی_2 خالی vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  اگر(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ساکن خالی scroll(VTermState *state, VTermRect rect, صحیح downward, صحیح rightward)
{
  صحیح صفیں_2;
  صحیح ستون_تعداد;
  اگر(!downward && !rightward)
    واپس;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  اگر(rect.انتہا_صف <= rect.آغاز_صف || rect.انتہا_ستون <= rect.آغاز_ستون)
    واپس;

  صفیں_2 = rect.انتہا_صف - rect.آغاز_صف;
  اگر(downward > صفیں_2)
    downward = صفیں_2;
  ورنہ اگر(downward < -صفیں_2)
    downward = -صفیں_2;

  ستون_تعداد = rect.انتہا_ستون - rect.آغاز_ستون;
  اگر(rightward > ستون_تعداد)
    rightward = ستون_تعداد;
  ورنہ اگر(rightward < -ستون_تعداد)
    rightward = -ستون_تعداد;

  اگر(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    اگر(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .آغاز_صف = rect.آغاز_صف, .انتہا_صف = rect.آغاز_صف + downward,
          .آغاز_ستون = rect.آغاز_ستون, .انتہا_ستون = rect.انتہا_ستون}, state->cbdata);
    ورنہ اگر(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .آغاز_صف = rect.انتہا_صف + downward, .انتہا_صف = rect.انتہا_صف,
          .آغاز_ستون = rect.آغاز_ستون,          .انتہا_ستون = rect.انتہا_ستون}, state->cbdata);

    اگر(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .آغاز_صف = rect.آغاز_صف, .انتہا_صف = rect.انتہا_صف,
          .آغاز_ستون = rect.آغاز_ستون, .انتہا_ستون = rect.آغاز_ستون + rightward}, state->cbdata);
    ورنہ اگر(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .آغاز_صف = rect.آغاز_صف,           .انتہا_صف = rect.انتہا_صف,
          .آغاز_ستون = rect.انتہا_ستون + rightward, .انتہا_ستون = rect.انتہا_ستون}, state->cbdata);
  }

  // Update lineinfo if full line
  اگر(rect.آغاز_ستون == 0 && rect.انتہا_ستون == state->ستون_تعداد && rightward == 0) {
    صحیح اونچائی = rect.انتہا_صف - rect.آغاز_صف - مطلق_قدر(downward);
    صحیح صف;
    VTerm_سطر_Info zero_سطر_Info = {0x0};

    اگر(downward > 0) {
      memmove(state->lineinfo + rect.آغاز_صف,
              state->lineinfo + rect.آغاز_صف + downward,
              اونچائی * حجم(state->lineinfo[0]));
      برائے(صف = rect.انتہا_صف - downward; صف < rect.انتہا_صف; صف++)
        state->lineinfo[صف] = zero_سطر_Info;
    }
    ورنہ {
      memmove(state->lineinfo + rect.آغاز_صف - downward,
              state->lineinfo + rect.آغاز_صف,
              اونچائی * حجم(state->lineinfo[0]));
      برائے(صف = rect.آغاز_صف; صف < rect.آغاز_صف - downward; صف++)
        state->lineinfo[صف] = zero_سطر_Info;
    }
  }

  اگر(state->callbacks && state->callbacks->scrollrect)
    اگر((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      واپس;

  اگر(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ساکن خالی linefeed(VTermState *state)
{
  اگر(state->pos.صف == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.آغاز_صف = state->scrollregion_چوٹی;
    rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
    rect.آغاز_ستون = SCROLLREGION_بایاں(state);
    rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

    scroll(state, rect, 1, 0);
  }
  ورنہ اگر(state->pos.صف < state->صفیں_2-1)
    state->pos.صف++;
}

ساکن خالی grow_combine_بفر(VTermState *state)
{
  size_t    new_سائز = state->combine_chars_سائز * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_سائز * حجم(new_chars[0]));

  حافظہ_نقل(new_chars, state->combine_chars, state->combine_chars_سائز * حجم(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_سائز = new_سائز;
}

ساکن خالی سیٹ_ستون_tabstop(VTermState *state, صحیح ستون)
{
  غیرعلامتی حرف نقاب = 1 << (ستون & 7);
  state->tabstops[ستون >> 3] |= نقاب;
}

ساکن خالی clear_ستون_tabstop(VTermState *state, صحیح ستون)
{
  غیرعلامتی حرف نقاب = 1 << (ستون & 7);
  state->tabstops[ستون >> 3] &= ~نقاب;
}

ساکن صحیح is_ستون_tabstop(VTermState *state, صحیح ستون)
{
  غیرعلامتی حرف نقاب = 1 << (ستون & 7);
  واپس state->tabstops[ستون >> 3] & نقاب;
}

ساکن صحیح is_cursor_اندر_scrollregion(ثابت VTermState *state)
{
  اگر(state->pos.صف < state->scrollregion_چوٹی ||
     state->pos.صف >= SCROLLREGION_BOTTOM(state))
    واپس 0;
  اگر(state->pos.ستون < SCROLLREGION_بایاں(state) ||
     state->pos.ستون >= SCROLLREGION_دایاں(state))
    واپس 0;

  واپس 1;
}

ساکن خالی tab(VTermState *state, صحیح گنتی, صحیح direction)
{
  جب(گنتی > 0) {
    اگر(direction > 0) {
      اگر(state->pos.ستون >= THISROWWIDTH(state)-1)
        واپس;

      state->pos.ستون++;
    }
    ورنہ اگر(direction < 0) {
      اگر(state->pos.ستون < 1)
        واپس;

      state->pos.ستون--;
    }

    اگر(is_ستون_tabstop(state, state->pos.ستون))
      گنتی--;
  }
}

#تعریف_کرو NO_FORCE 0
#تعریف_کرو FORCE    1

#تعریف_کرو DWL_OFF 0
#تعریف_کرو DWL_ON  1

#تعریف_کرو DHL_OFF    0
#تعریف_کرو DHL_چوٹی    1
#تعریف_کرو DHL_BOTTOM 2

ساکن خالی سیٹ_lineinfo(VTermState *state, صحیح صف, صحیح force, صحیح dwl, صحیح dhl)
{
  VTerm_سطر_Info info = state->lineinfo[صف];

  اگر(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  ورنہ اگر(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  اگر(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  ورنہ اگر(dhl == DHL_چوٹی)
    info.doubleheight = DHL_چوٹی;
  ورنہ اگر(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  اگر((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(صف, &info, state->lineinfo + صف, state->cbdata))
      || force)
    state->lineinfo[صف] = info;
}

ساکن صحیح on_متن(ثابت حرف بائٹیں[], size_t len, خالی *user)
{
  VTermState *state = user;
  صحیح npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  صحیح i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / حجم(uint32_t);

  encoding =
    state->gsingle_سیٹ     ? &state->encoding[state->gsingle_سیٹ] :
    !(بائٹیں[eaten] & 0x80) ? &state->encoding[state->gl_سیٹ] :
    state->vt->منوال.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_سیٹ];
  اگر (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->ڈیٹا,
      codepoints, &npoints, state->gsingle_سیٹ ? 1 : (صحیح)maxpoints,
      بائٹیں, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  اگر(!npoints)
  {
    واپس (صحیح)eaten;
  }

  اگر(state->gsingle_سیٹ && npoints)
    state->gsingle_سیٹ = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  اگر(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    اگر(state->pos.صف == state->combine_pos.صف && state->pos.ستون == state->combine_pos.ستون + state->combine_چوڑائی) {
#اگر_تعریف_شدہ DEBUG_GLYPH_COMBINE
      صحیح printpos;
      لکھو("DEBUG: COMBINING SPLIT GLYPH of chars {");
      برائے(printpos = 0; state->combine_chars[printpos]; printpos++)
        لکھو("U+%04x ", state->combine_chars[printpos]);
      لکھو("} + {");
#اگر_ختم

      /* Find where we need to append these combining chars */
      صحیح saved_i = 0;
      جب(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      جب(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        اگر(saved_i >= (صحیح)state->combine_chars_سائز)
          grow_combine_بفر(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      اگر(saved_i >= (صحیح)state->combine_chars_سائز)
        grow_combine_بفر(state);
      state->combine_chars[saved_i] = 0;

#اگر_تعریف_شدہ DEBUG_GLYPH_COMBINE
      برائے(; state->combine_chars[printpos]; printpos++)
        لکھو("U+%04x ", state->combine_chars[printpos]);
      لکھو("}\n");
#اگر_ختم

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_چوڑائی, state->combine_pos);
    }
    ورنہ {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  برائے(; i < npoints; i++) {
    // Try to find combining characters following this
    صحیح glyph_starts = i;
    صحیح glyph_سرے;
    صحیح چوڑائی = 0;

    برائے(glyph_سرے = i + 1;
        (glyph_سرے < npoints) && (glyph_سرے < glyph_starts + VTERM_MAX_CHARS_فی_CELL);
        glyph_سرے++)
      اگر(!vterm_unicode_is_combining(codepoints[glyph_سرے]))
        توڑو;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_فی_CELL + 1) * حجم(uint32_t));
    اگر (chars == تہی)
      توڑو;

    برائے( ; i < glyph_سرے; i++) {
      صحیح this_چوڑائی;
      اگر(vterm_get_special_pty_type() == 2) {
        state->vt->اندر_backspace -= (state->vt->اندر_backspace > 0) ? 1 : 0;
        اگر(state->vt->اندر_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_چوڑائی = vterm_unicode_چوڑائی(codepoints[i]);
#اگر_تعریف_شدہ DEBUG
      اگر(this_چوڑائی < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#اگر_ختم
      اگر (i == glyph_starts || this_چوڑائی > چوڑائی)
	چوڑائی = this_چوڑائی;  // TODO: should be += ?
    }

    اگر (چوڑائی < 0)
      چوڑائی = 0;

    جب(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_سرے - glyph_starts] = 0;
    i--;

#اگر_تعریف_شدہ DEBUG_GLYPH_COMBINE
    صحیح printpos;
    لکھو("DEBUG: COMBINED GLYPH of %d chars {", glyph_سرے - glyph_starts);
    برائے(printpos = 0; printpos < glyph_سرے - glyph_starts; printpos++)
      لکھو("U+%04x ", chars[printpos]);
    لکھو("}, onscreen width %d\n", چوڑائی);
#اگر_ختم

    اگر(state->at_phantom || state->pos.ستون + چوڑائی > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.ستون = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.صف].continuation = 1;
    }

    اگر(state->منوال.داخل_کرو) {
      // TODO: This will be a little inefficient for large bodies of text, as
      // it'll have to 'ICH' effectively before every glyph. We should scan
      // ahead and ICH as many times as required
      VTermRect rect;
      rect.آغاز_صف = state->pos.صف;
      rect.انتہا_صف   = state->pos.صف + 1;
      rect.آغاز_ستون = state->pos.ستون;
      rect.انتہا_ستون   = THISROWWIDTH(state);
      scroll(state, rect, 0, -1);
    }

    putglyph(state, chars, چوڑائی, state->pos);

    اگر(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      صحیح save_i;
      برائے(save_i = 0; chars[save_i]; save_i++) {
        اگر(save_i >= (صحیح)state->combine_chars_سائز)
          grow_combine_بفر(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      اگر(save_i >= (صحیح)state->combine_chars_سائز)
        grow_combine_بفر(state);
      state->combine_chars[save_i] = 0;
      state->combine_چوڑائی = چوڑائی;
      state->combine_pos = state->pos;
    }

    اگر(state->pos.ستون + چوڑائی >= THISROWWIDTH(state)) {
      اگر(state->منوال.autowrap)
        state->at_phantom = 1;
    }
    ورنہ {
      state->pos.ستون += چوڑائی;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#اگر_تعریف_شدہ DEBUG
  اگر(state->pos.صف < 0 || state->pos.صف >= state->صفیں_2 ||
     state->pos.ستون < 0 || state->pos.ستون >= state->ستون_تعداد) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.صف, state->pos.ستون);
    abort();
  }
#اگر_ختم

  واپس (صحیح)eaten;
}

ساکن صحیح on_control(غیرعلامتی حرف control, خالی *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.ستون -= (leadpos.ستون >= 2 ? 2 : 0);

  انتخاب(control) {
  حالت 0x07: // BEL - ECMA-48 8.3.3
    اگر(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    توڑو;

  حالت 0x08: // BS - ECMA-48 8.3.5
    اگر(state->pos.ستون > 0)
      state->pos.ستون--;
    اگر(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      اگر(vterm_unicode_چوڑائی(cell.chars[0]) == 2)
        state->pos.ستون--;
    }
    توڑو;

  حالت 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    توڑو;

  حالت 0x0a: // LF - ECMA-48 8.3.74
  حالت 0x0b: // VT
  حالت 0x0c: // FF
    linefeed(state);
    اگر(state->منوال.newline)
      state->pos.ستون = 0;
    توڑو;

  حالت 0x0d: // CR - ECMA-48 8.3.15
    state->pos.ستون = 0;
    توڑو;

  حالت 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_سیٹ = 1;
    توڑو;

  حالت 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_سیٹ = 0;
    توڑو;

  حالت 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    توڑو;

  حالت 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.ستون = 0;
    توڑو;

  حالت 0x88: // HTS - ECMA-48 8.3.62
    سیٹ_ستون_tabstop(state, state->pos.ستون);
    توڑو;

  حالت 0x8d: // RI - ECMA-48 8.3.104
    اگر(state->pos.صف == state->scrollregion_چوٹی) {
      VTermRect rect;
      rect.آغاز_صف = state->scrollregion_چوٹی;
      rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
      rect.آغاز_ستون = SCROLLREGION_بایاں(state);
      rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

      scroll(state, rect, -1, 0);
    }
    ورنہ اگر(state->pos.صف > 0)
        state->pos.صف--;
    توڑو;

  حالت 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_سیٹ = 2;
    توڑو;

  حالت 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_سیٹ = 3;
    توڑو;

  طےشدہ:
    اگر(state->fallbacks && state->fallbacks->control)
      اگر((*state->fallbacks->control)(control, state->fbdata))
        واپس 1;

    واپس 0;
  }

  updatecursor(state, &oldpos, 1);

#اگر_تعریف_شدہ DEBUG
  اگر(state->pos.صف < 0 || state->pos.صف >= state->صفیں_2 ||
     state->pos.ستون < 0 || state->pos.ستون >= state->ستون_تعداد) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.صف, state->pos.ستون);
    abort();
  }
#اگر_ختم

  واپس 1;
}

ساکن صحیح settermprop_bool(VTermState *state, VTermProp prop, صحیح v)
{
  VTerm_قدر val;
  val.boolean = v;
  واپس vterm_state_سیٹ_termprop(state, prop, &val);
}

ساکن صحیح settermprop_int(VTermState *state, VTermProp prop, صحیح v)
{
  VTerm_قدر val;
  val.نمبر = v;
  واپس vterm_state_سیٹ_termprop(state, prop, &val);
}

ساکن صحیح settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_قدر val;

  val.string = frag;
  واپس vterm_state_سیٹ_termprop(state, prop, &val);
}

ساکن خالی savecursor(VTermState *state, صحیح save)
{
  اگر(save) {
    state->saved.pos = state->pos;
    state->saved.منوال.cursor_visible = state->منوال.cursor_visible;
    state->saved.منوال.cursor_blink   = state->منوال.cursor_blink;
    state->saved.منوال.cursor_shape   = state->منوال.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  ورنہ {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.منوال.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.منوال.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.منوال.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ساکن صحیح on_escape(ثابت حرف *بائٹیں, size_t len, خالی *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  انتخاب(بائٹیں[0]) {
  حالت ' ':
    اگر(len != 2)
      واپس 0;

    انتخاب(بائٹیں[1]) {
      حالت 'F': // S7C1T
        state->vt->منوال.ctrl8bit = 0;
        توڑو;

      حالت 'G': // S8C1T
        state->vt->منوال.ctrl8bit = 1;
        توڑو;

      طےشدہ:
        واپس 0;
    }
    واپس 2;

  حالت '#':
    اگر(len != 2)
      واپس 0;

    انتخاب(بائٹیں[1]) {
      حالت '3': // DECDHL top
        اگر(state->منوال.leftrightmargin)
          توڑو;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_ON, DHL_چوٹی);
        توڑو;

      حالت '4': // DECDHL bottom
        اگر(state->منوال.leftrightmargin)
          توڑو;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_ON, DHL_BOTTOM);
        توڑو;

      حالت '5': // DECSWL
        اگر(state->منوال.leftrightmargin)
          توڑو;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_OFF, DHL_OFF);
        توڑو;

      حالت '6': // DECDWL
        اگر(state->منوال.leftrightmargin)
          توڑو;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_ON, DHL_OFF);
        توڑو;

      حالت '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        برائے(pos.صف = 0; pos.صف < state->صفیں_2; pos.صف++)
          برائے(pos.ستون = 0; pos.ستون < ROWWIDTH(state, pos.صف); pos.ستون++)
            putglyph(state, E, 1, pos);
        توڑو;
      }

      طےشدہ:
        واپس 0;
    }
    واپس 2;

  حالت '(': حالت ')': حالت '*': حالت '+': // SCS
    اگر(len != 2)
      واپس 0;

    {
      صحیح setnum = بائٹیں[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, بائٹیں[1]);

      اگر(newenc) {
        state->encoding[setnum].enc = newenc;

        اگر(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].ڈیٹا);
      }
    }

    واپس 2;

  حالت '7': // DECSC
    savecursor(state, 1);
    واپس 1;

  حالت '8': // DECRC
    savecursor(state, 0);
    واپس 1;

  حالت '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    واپس 1;

  حالت '=': // DECKPAM
    state->منوال.keypad = 1;
    واپس 1;

  حالت '>': // DECKPNM
    state->منوال.keypad = 0;
    واپس 1;

  حالت 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    اگر(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->منوال.cursor_visible, state->cbdata);
    واپس 1;
  }

  حالت 'n': // LS2 - ECMA-48 8.3.78
    state->gl_سیٹ = 2;
    واپس 1;

  حالت 'o': // LS3 - ECMA-48 8.3.80
    state->gl_سیٹ = 3;
    واپس 1;

  حالت '~': // LS1R - ECMA-48 8.3.77
    state->gr_سیٹ = 1;
    واپس 1;

  حالت '}': // LS2R - ECMA-48 8.3.79
    state->gr_سیٹ = 2;
    واپس 1;

  حالت '|': // LS3R - ECMA-48 8.3.81
    state->gr_سیٹ = 3;
    واپس 1;

  طےشدہ:
    واپس 0;
  }
}

ساکن خالی سیٹ_منوال(VTermState *state, صحیح عدد, صحیح val)
{
  انتخاب(عدد) {
  حالت 4: // IRM - ECMA-48 7.2.10
    state->منوال.داخل_کرو = val;
    توڑو;

  حالت 20: // LNM - ANSI X3.4-1977
    state->منوال.newline = val;
    توڑو;

  طےشدہ:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", عدد);
    واپس;
  }
}

ساکن خالی سیٹ_dec_منوال(VTermState *state, صحیح عدد, صحیح val)
{
  انتخاب(عدد) {
  حالت 1:
    state->منوال.cursor = val;
    توڑو;

  حالت 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_الٹ, val);
    توڑو;

  حالت 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->منوال.origin = val;
      state->pos.صف = state->منوال.origin ? state->scrollregion_چوٹی : 0;
      state->pos.ستون = state->منوال.origin ? SCROLLREGION_بایاں(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    توڑو;

  حالت 7:
    state->منوال.autowrap = val;
    توڑو;

  حالت 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    توڑو;

  حالت 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    توڑو;

  حالت 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->منوال.leftrightmargin = val;
    اگر(val) {
      صحیح صف;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      برائے(صف = 0; صف < state->صفیں_2; صف++)
        سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
    }

    توڑو;

  حالت 1000:
  حالت 1002:
  حالت 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (عدد == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (عدد == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    توڑو;

  حالت 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->منوال.report_focus = val;
    توڑو;

  حالت 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    توڑو;

  حالت 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    توڑو;

  حالت 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    توڑو;

  حالت 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    توڑو;

  حالت 1048:
    savecursor(state, val);
    توڑو;

  حالت 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    توڑو;

  حالت 2004:
    state->منوال.bracketpaste = val;
    توڑو;

  طےشدہ:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", عدد);
    واپس;
  }
}

ساکن خالی request_dec_منوال(VTermState *state, صحیح عدد)
{
  صحیح جواب;

  انتخاب(عدد) {
    حالت 1:
      جواب = state->منوال.cursor;
      توڑو;

    حالت 5:
      جواب = state->منوال.screen;
      توڑو;

    حالت 6:
      جواب = state->منوال.origin;
      توڑو;

    حالت 7:
      جواب = state->منوال.autowrap;
      توڑو;

    حالت 12:
      جواب = state->منوال.cursor_blink;
      توڑو;

    حالت 25:
      جواب = state->منوال.cursor_visible;
      توڑو;

    حالت 69:
      جواب = state->منوال.leftrightmargin;
      توڑو;

    حالت 1000:
      جواب = state->mouse_flags == MOUSE_WANT_CLICK;
      توڑو;

    حالت 1002:
      جواب = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      توڑو;

    حالت 1003:
      جواب = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      توڑو;

    حالت 1004:
      جواب = state->منوال.report_focus;
      توڑو;

    حالت 1005:
      جواب = state->mouse_protocol == MOUSE_UTF8;
      توڑو;

    حالت 1006:
      جواب = state->mouse_protocol == MOUSE_SGR;
      توڑو;

    حالت 1015:
      جواب = state->mouse_protocol == MOUSE_RXVT;
      توڑو;

    حالت 1047:
      جواب = state->منوال.alt_screen;
      توڑو;

    حالت 2004:
      جواب = state->منوال.bracketpaste;
      توڑو;

    طےشدہ:
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", عدد, 0);
      واپس;
  }

  vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", عدد, جواب ? 1 : 2);
}

ساکن خالی request_نسخہ_string(VTermState *state)
{
  vterm_ڈالو_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_نسخہ_MAJOR, VTERM_نسخہ_MINOR);
}

ساکن صحیح on_csi(ثابت حرف *leader, ثابت طویل دلائل[], صحیح argcount, ثابت حرف *intermed, حرف command, خالی *user)
{
  VTermState *state = user;
  صحیح leader_byte = 0;
  صحیح intermed_byte = 0;
  صحیح cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  صحیح handled = 1;

  // Some temporaries for later code
  صحیح گنتی, val;
  صحیح صف, ستون;
  VTermRect rect;
  صحیح selective;

  اگر(leader && leader[0]) {
    اگر(leader[1]) // longer than 1 char
      واپس 0;

    انتخاب(leader[0]) {
    حالت '?':
    حالت '>':
    حالت '<':
      leader_byte = leader[0];
      توڑو;
    طےشدہ:
      واپس 0;
    }
  }

  اگر(intermed && intermed[0]) {
    اگر(intermed[1]) // longer than 1 char
      واپس 0;

    انتخاب(intermed[0]) {
    حالت ' ':
    حالت '!':
    حالت '"':
    حالت '$':
    حالت '\'':
      intermed_byte = intermed[0];
      توڑو;
    طےشدہ:
      واپس 0;
    }
  }

  oldpos = state->pos;

#تعریف_کرو LBOUND(v,min) اگر((v) < (min)) (v) = (min)
#تعریف_کرو UBOUND(v,max) اگر((v) > (max)) (v) = (max)

#تعریف_کرو LEADER(l,b) ((l << 8) | b)
#تعریف_کرو INTERMED(i,b) ((i << 16) | b)

  انتخاب(intermed_byte << 16 | leader_byte << 8 | command) {
  حالت 0x40: // ICH - ECMA-48 8.3.64
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    اگر(!is_cursor_اندر_scrollregion(state))
      توڑو;

    rect.آغاز_صف = state->pos.صف;
    rect.انتہا_صف   = state->pos.صف + 1;
    rect.آغاز_ستون = state->pos.ستون;
    اگر(state->منوال.leftrightmargin)
      rect.انتہا_ستون = SCROLLREGION_دایاں(state);
    ورنہ
      rect.انتہا_ستون = THISROWWIDTH(state);

    scroll(state, rect, 0, -گنتی);

    توڑو;

  حالت 0x41: // CUU - ECMA-48 8.3.22
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.صف -= گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x42: // CUD - ECMA-48 8.3.19
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.صف += گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x43: // CUF - ECMA-48 8.3.20
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.ستون += گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x44: // CUB - ECMA-48 8.3.18
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.ستون -= گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x45: // CNL - ECMA-48 8.3.12
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.ستون = 0;
    state->pos.صف += گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x46: // CPL - ECMA-48 8.3.13
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.ستون = 0;
    state->pos.صف -= گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(دلائل[0], 1);
    state->pos.ستون = val-1;
    state->at_phantom = 0;
    توڑو;

  حالت 0x48: // CUP - ECMA-48 8.3.21
    صف = CSI_ARG_OR(دلائل[0], 1);
    ستون = argcount < 2 || CSI_ARG_IS_MISSING(دلائل[1]) ? 1 : CSI_ARG(دلائل[1]);
    // zero-based
    اگر(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      اگر(state->pos.صف == صف - 1) {
        صحیح cnt, ptr = 0;
        برائے(cnt = 0; cnt < ستون - 1; ++cnt) {
	  VTermPos p;
	  VTermScreenCell c0, c1;
	  p.صف = صف - 1;
	  p.ستون = ptr;
	  vterm_screen_get_cell(state->vt->screen, p, &c0);
	  p.ستون++;
	  vterm_screen_get_cell(state->vt->screen, p, &c1);
	  ptr += (c1.chars[0] == (uint32_t)-1)		    // double cell?
	     ? (vterm_unicode_is_ambiguous(c0.chars[0]))    // is ambiguous?
	     ? vterm_unicode_چوڑائی(0x00a1) : 1		    // &ambiwidth
	     : 1;					    // not ambiguous
        }
        ستون = ptr + 1;
      }
    }
    state->pos.صف = صف-1;
    state->pos.ستون = ستون-1;
    اگر(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.ستون += SCROLLREGION_بایاں(state);
    }
    state->at_phantom = 0;
    توڑو;

  حالت 0x49: // CHT - ECMA-48 8.3.10
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    tab(state, گنتی, +1);
    توڑو;

  حالت 0x4a: // ED - ECMA-48 8.3.39
  حالت LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    انتخاب(CSI_ARG(دلائل[0])) {
    حالت CSI_ARG_MISSING:
    حالت 0:
      rect.آغاز_صف = state->pos.صف; rect.انتہا_صف = state->pos.صف + 1;
      rect.آغاز_ستون = state->pos.ستون; rect.انتہا_ستون = state->ستون_تعداد;
      اگر(rect.انتہا_ستون > rect.آغاز_ستون)
        erase(state, rect, selective);

      rect.آغاز_صف = state->pos.صف + 1; rect.انتہا_صف = state->صفیں_2;
      rect.آغاز_ستون = 0;
      برائے(صف = rect.آغاز_صف; صف < rect.انتہا_صف; صف++)
	سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
      اگر(rect.انتہا_صف > rect.آغاز_صف)
        erase(state, rect, selective);
      توڑو;

    حالت 1:
      rect.آغاز_صف = 0; rect.انتہا_صف = state->pos.صف;
      rect.آغاز_ستون = 0; rect.انتہا_ستون = state->ستون_تعداد;
      برائے(صف = rect.آغاز_صف; صف < rect.انتہا_صف; صف++)
	سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
      اگر(rect.انتہا_ستون > rect.آغاز_ستون)
        erase(state, rect, selective);

      rect.آغاز_صف = state->pos.صف; rect.انتہا_صف = state->pos.صف + 1;
                          rect.انتہا_ستون = state->pos.ستون + 1;
      اگر(rect.انتہا_صف > rect.آغاز_صف)
        erase(state, rect, selective);
      توڑو;

    حالت 2:
      rect.آغاز_صف = 0; rect.انتہا_صف = state->صفیں_2;
      rect.آغاز_ستون = 0; rect.انتہا_ستون = state->ستون_تعداد;
      برائے(صف = rect.آغاز_صف; صف < rect.انتہا_صف; صف++)
	سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      توڑو;

    حالت 3:
      اگر(state->callbacks && state->callbacks->sb_clear)
        اگر((*state->callbacks->sb_clear)(state->cbdata))
          واپس 1;
      توڑو;
    }
    توڑو;

  حالت 0x4b: // EL - ECMA-48 8.3.41
  حالت LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.آغاز_صف = state->pos.صف;
    rect.انتہا_صف   = state->pos.صف + 1;

    انتخاب(CSI_ARG(دلائل[0])) {
    حالت CSI_ARG_MISSING:
    حالت 0:
      rect.آغاز_ستون = state->pos.ستون; rect.انتہا_ستون = THISROWWIDTH(state); توڑو;
    حالت 1:
      rect.آغاز_ستون = 0; rect.انتہا_ستون = state->pos.ستون + 1; توڑو;
    حالت 2:
      rect.آغاز_ستون = 0; rect.انتہا_ستون = THISROWWIDTH(state); توڑو;
    طےشدہ:
      واپس 0;
    }

    اگر(rect.انتہا_ستون > rect.آغاز_ستون)
      erase(state, rect, selective);

    توڑو;

  حالت 0x4c: // IL - ECMA-48 8.3.67
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    اگر(!is_cursor_اندر_scrollregion(state))
      توڑو;

    rect.آغاز_صف = state->pos.صف;
    rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
    rect.آغاز_ستون = SCROLLREGION_بایاں(state);
    rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

    scroll(state, rect, -گنتی, 0);

    توڑو;

  حالت 0x4d: // DL - ECMA-48 8.3.32
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    اگر(!is_cursor_اندر_scrollregion(state))
      توڑو;

    rect.آغاز_صف = state->pos.صف;
    rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
    rect.آغاز_ستون = SCROLLREGION_بایاں(state);
    rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

    scroll(state, rect, گنتی, 0);

    توڑو;

  حالت 0x50: // DCH - ECMA-48 8.3.26
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    اگر(!is_cursor_اندر_scrollregion(state))
      توڑو;

    rect.آغاز_صف = state->pos.صف;
    rect.انتہا_صف   = state->pos.صف + 1;
    rect.آغاز_ستون = state->pos.ستون;
    اگر(state->منوال.leftrightmargin)
      rect.انتہا_ستون = SCROLLREGION_دایاں(state);
    ورنہ
      rect.انتہا_ستون = THISROWWIDTH(state);

    scroll(state, rect, 0, گنتی);

    توڑو;

  حالت 0x53: // SU - ECMA-48 8.3.147
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    rect.آغاز_صف = state->scrollregion_چوٹی;
    rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
    rect.آغاز_ستون = SCROLLREGION_بایاں(state);
    rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

    scroll(state, rect, گنتی, 0);

    توڑو;

  حالت 0x54: // SD - ECMA-48 8.3.113
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    rect.آغاز_صف = state->scrollregion_چوٹی;
    rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
    rect.آغاز_ستون = SCROLLREGION_بایاں(state);
    rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

    scroll(state, rect, -گنتی, 0);

    توڑو;

  حالت 0x58: // ECH - ECMA-48 8.3.38
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    rect.آغاز_صف = state->pos.صف;
    rect.انتہا_صف   = state->pos.صف + 1;
    rect.آغاز_ستون = state->pos.ستون;
    rect.انتہا_ستون   = state->pos.ستون + گنتی;
    UBOUND(rect.انتہا_ستون, THISROWWIDTH(state));

    erase(state, rect, 0);
    توڑو;

  حالت 0x5a: // CBT - ECMA-48 8.3.7
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    tab(state, گنتی, -1);
    توڑو;

  حالت 0x60: // HPA - ECMA-48 8.3.57
    ستون = CSI_ARG_OR(دلائل[0], 1);
    state->pos.ستون = ستون-1;
    state->at_phantom = 0;
    توڑو;

  حالت 0x61: // HPR - ECMA-48 8.3.59
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.ستون += گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x62: { // REP - ECMA-48 8.3.103
    ثابت صحیح صف_چوڑائی = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    اگر(state->combine_چوڑائی < 1)
      توڑو;

    گنتی = CSI_ARG_گنتی(دلائل[0]);
    ستون = state->pos.ستون + گنتی;
    UBOUND(ستون, صف_چوڑائی);
    جب (state->pos.ستون < ستون) {
      putglyph(state, state->combine_chars, state->combine_چوڑائی, state->pos);
      state->pos.ستون += state->combine_چوڑائی;
    }
    اگر (state->pos.ستون + state->combine_چوڑائی >= صف_چوڑائی) {
      اگر (state->منوال.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    توڑو;
  }

  حالت 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(دلائل[0], 0);
    اگر(val == 0)
      // DEC VT100 response
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    توڑو;

  حالت LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    توڑو;

  حالت 0x64: // VPA - ECMA-48 8.3.158
    صف = CSI_ARG_OR(دلائل[0], 1);
    state->pos.صف = صف-1;
    اگر(state->منوال.origin)
      state->pos.صف += state->scrollregion_چوٹی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x65: // VPR - ECMA-48 8.3.160
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.صف += گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x66: // HVP - ECMA-48 8.3.63
    صف = CSI_ARG_OR(دلائل[0], 1);
    ستون = argcount < 2 || CSI_ARG_IS_MISSING(دلائل[1]) ? 1 : CSI_ARG(دلائل[1]);
    // zero-based
    state->pos.صف = صف-1;
    state->pos.ستون = ستون-1;
    اگر(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.ستون += SCROLLREGION_بایاں(state);
    }
    state->at_phantom = 0;
    توڑو;

  حالت 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(دلائل[0], 0);

    انتخاب(val) {
    حالت 0:
      clear_ستون_tabstop(state, state->pos.ستون);
      توڑو;
    حالت 3:
    حالت 5:
      برائے(ستون = 0; ستون < state->ستون_تعداد; ستون++)
        clear_ستون_tabstop(state, ستون);
      توڑو;
    حالت 1:
    حالت 2:
    حالت 4:
      توڑو;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    طےشدہ:
      واپس 0;
    }
    توڑو;

  حالت 0x68: // SM - ECMA-48 8.3.125
    اگر(!CSI_ARG_IS_MISSING(دلائل[0]))
      سیٹ_منوال(state, CSI_ARG(دلائل[0]), 1);
    توڑو;

  حالت LEADER('?', 0x68): // DEC private mode set
    برائے(صحیح i = 0; i < argcount; i++) {
      اگر(!CSI_ARG_IS_MISSING(دلائل[i]))
        سیٹ_dec_منوال(state, CSI_ARG(دلائل[i]), 1);
    }
    توڑو;

  حالت 0x6a: // HPB - ECMA-48 8.3.58
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.ستون -= گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x6b: // VPB - ECMA-48 8.3.159
    گنتی = CSI_ARG_گنتی(دلائل[0]);
    state->pos.صف -= گنتی;
    state->at_phantom = 0;
    توڑو;

  حالت 0x6c: // RM - ECMA-48 8.3.106
    اگر(!CSI_ARG_IS_MISSING(دلائل[0]))
      سیٹ_منوال(state, CSI_ARG(دلائل[0]), 0);
    توڑو;

  حالت LEADER('?', 0x6c): // DEC private mode reset
    برائے(صحیح i = 0; i < argcount; i++) {
      اگر(!CSI_ARG_IS_MISSING(دلائل[i]))
        سیٹ_dec_منوال(state, CSI_ARG(دلائل[i]), 0);
    }
    توڑو;

  حالت 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, دلائل, argcount);
    توڑو;

  حالت LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    اگر (argcount == 1 && CSI_ARG(دلائل[0]) == 4)
    {
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->منوال.modify_other_کلیدیں ? 2 : 0);
      توڑو;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    برائے(صحیح argi = 0; argi < argcount; argi++) {
      طویل arg;
      انتخاب(arg = CSI_ARG(دلائل[argi])) {
        حالت 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          توڑو;
        حالت 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          توڑو;
        حالت 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          توڑو;
      }
    }
    توڑو;

  حالت LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    اگر (argcount == 2 && CSI_ARG(دلائل[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->منوال.kitty_keyboard = 0;

      state->منوال.modify_other_کلیدیں = CSI_ARG(دلائل[1]) == 2;
    }
    توڑو;

  حالت LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    اگر (argcount == 1 && CSI_ARG(دلائل[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->منوال.modify_other_کلیدیں = 0;

      state->منوال.kitty_keyboard = 1;
    }
    توڑو;

  حالت LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    اگر (argcount <= 1)
      state->منوال.kitty_keyboard = 0;
    توڑو;

  حالت LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    اگر (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->منوال.kitty_keyboard);
    توڑو;

  حالت 0x6e: // DSR - ECMA-48 8.3.35
  حالت LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(دلائل[0], 0);

    {
      حرف *qmark = (leader_byte == '?') ? "?" : "";

      انتخاب(val) {
      حالت 0: حالت 1: حالت 2: حالت 3: حالت 4:
        // ignore - these are replies
        توڑو;
      حالت 5:
        vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        توڑو;
      حالت 6: // CPR - cursor position report
        vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.صف + 1, state->pos.ستون + 1);
        توڑو;
      }
    }
    توڑو;


  حالت INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    توڑو;

  حالت LEADER('?', INTERMED('$', 0x70)):
    request_dec_منوال(state, CSI_ARG(دلائل[0]));
    توڑو;

  حالت LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_نسخہ_string(state);
    توڑو;

  حالت INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(دلائل[0], 1);

    انتخاب(val) {
    حالت 0: حالت 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_بلاک);
      توڑو;
    حالت 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_بلاک);
      توڑو;
    حالت 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      توڑو;
    حالت 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      توڑو;
    حالت 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_بایاں);
      توڑو;
    حالت 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_بایاں);
      توڑو;
    }

    توڑو;

  حالت INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(دلائل[0], 0);

    انتخاب(val) {
    حالت 0: حالت 2:
      state->protected_cell = 0;
      توڑو;
    حالت 1:
      state->protected_cell = 1;
      توڑو;
    }

    توڑو;

  حالت 0x72: // DECSTBM - DEC custom
    state->scrollregion_چوٹی = CSI_ARG_OR(دلائل[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(دلائل[1]) ? -1 : CSI_ARG(دلائل[1]);
    LBOUND(state->scrollregion_چوٹی, 0);
    UBOUND(state->scrollregion_چوٹی, state->صفیں_2);
    LBOUND(state->scrollregion_bottom, -1);
    اگر(state->scrollregion_چوٹی == 0 && state->scrollregion_bottom == state->صفیں_2)
      state->scrollregion_bottom = -1;
    ورنہ
      UBOUND(state->scrollregion_bottom, state->صفیں_2);

    اگر(SCROLLREGION_BOTTOM(state) <= state->scrollregion_چوٹی) {
      // Invalid
      state->scrollregion_چوٹی    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.صف = 0;
    state->pos.ستون = 0;
    اگر(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.ستون += SCROLLREGION_بایاں(state);
    }

    توڑو;

  حالت 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_بایاں = CSI_ARG_OR(دلائل[0], 1) - 1;
    state->scrollregion_دایاں = argcount < 2 || CSI_ARG_IS_MISSING(دلائل[1]) ? -1 : CSI_ARG(دلائل[1]);
    LBOUND(state->scrollregion_بایاں, 0);
    UBOUND(state->scrollregion_بایاں, state->ستون_تعداد);
    LBOUND(state->scrollregion_دایاں, -1);
    اگر(state->scrollregion_بایاں == 0 && state->scrollregion_دایاں == state->ستون_تعداد)
      state->scrollregion_دایاں = -1;
    ورنہ
      UBOUND(state->scrollregion_دایاں, state->ستون_تعداد);

    اگر(state->scrollregion_دایاں > -1 &&
       state->scrollregion_دایاں <= state->scrollregion_بایاں) {
      // Invalid
      state->scrollregion_بایاں  = 0;
      state->scrollregion_دایاں = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.صف = 0;
    state->pos.ستون = 0;
    اگر(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.ستون += SCROLLREGION_بایاں(state);
    }

    توڑو;

  حالت 0x74:
    انتخاب(CSI_ARG(دلائل[0])) {
      حالت 8: // CSI 8 ; rows ; cols t  set size
        اگر (argcount == 3 &&
            !CSI_ARG_IS_MISSING(دلائل[1]) && !CSI_ARG_IS_MISSING(دلائل[2]) &&
            CSI_ARG(دلائل[1]) > 0 && CSI_ARG(دلائل[2]) > 0)
	  on_resize(CSI_ARG(دلائل[1]), CSI_ARG(دلائل[2]), state);
	توڑو;
      طےشدہ:
	handled = 0;
	توڑو;
    }
    توڑو;

  حالت INTERMED('\'', 0x7D): // DECIC
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    اگر(!is_cursor_اندر_scrollregion(state))
      توڑو;

    rect.آغاز_صف = state->scrollregion_چوٹی;
    rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
    rect.آغاز_ستون = state->pos.ستون;
    rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

    scroll(state, rect, 0, -گنتی);

    توڑو;

  حالت INTERMED('\'', 0x7E): // DECDC
    گنتی = CSI_ARG_گنتی(دلائل[0]);

    اگر(!is_cursor_اندر_scrollregion(state))
      توڑو;

    rect.آغاز_صف = state->scrollregion_چوٹی;
    rect.انتہا_صف   = SCROLLREGION_BOTTOM(state);
    rect.آغاز_ستون = state->pos.ستون;
    rect.انتہا_ستون   = SCROLLREGION_دایاں(state);

    scroll(state, rect, 0, گنتی);

    توڑو;

  طےشدہ:
    handled = 0;
    توڑو;
  }

  اگر (!handled) {
    اگر(state->fallbacks && state->fallbacks->csi)
      اگر((*state->fallbacks->csi)(leader, دلائل, argcount, intermed, command, state->fbdata))
        واپس 1;

    واپس 0;
  }

  اگر(state->منوال.origin) {
    LBOUND(state->pos.صف, state->scrollregion_چوٹی);
    UBOUND(state->pos.صف, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.ستون, SCROLLREGION_بایاں(state));
    UBOUND(state->pos.ستون, SCROLLREGION_دایاں(state)-1);
  }
  ورنہ {
    LBOUND(state->pos.صف, 0);
    UBOUND(state->pos.صف, state->صفیں_2-1);
    LBOUND(state->pos.ستون, 0);
    UBOUND(state->pos.ستون, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#اگر_تعریف_شدہ DEBUG
  اگر(state->pos.صف < 0 || state->pos.صف >= state->صفیں_2 ||
     state->pos.ستون < 0 || state->pos.ستون >= state->ستون_تعداد) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.صف, state->pos.ستون);
    abort();
  }

  اگر(SCROLLREGION_BOTTOM(state) <= state->scrollregion_چوٹی) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_چوٹی);
    abort();
  }

  اگر(SCROLLREGION_دایاں(state) <= SCROLLREGION_بایاں(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_دایاں(state), SCROLLREGION_بایاں(state));
    abort();
  }
#اگر_ختم

  واپس 1;
}

ساکن حرف base64_one(uint8_t b)
{
  اگر(b < 26)
    واپس 'A' + b;
  ورنہ اگر(b < 52)
    واپس 'a' + b - 26;
  ورنہ اگر(b < 62)
    واپس '0' + b - 52;
  ورنہ اگر(b == 62)
    واپس '+';
  ورنہ اگر(b == 63)
    واپس '/';
  واپس 0;
}

ساکن uint8_t unbase64one(حرف c)
{
  اگر(c >= 'A' && c <= 'Z')
    واپس c - 'A';
  ورنہ اگر(c >= 'a' && c <= 'z')
    واپس c - 'a' + 26;
  ورنہ اگر(c >= '0' && c <= '9')
    واپس c - '0' + 52;
  ورنہ اگر(c == '+')
    واپس 62;
  ورنہ اگر(c == '/')
    واپس 63;

  واپس 0xFF;
}

ساکن خالی osc_selection(VTermState *state, VTermStringFragment frag)
{
  اگر(frag.initial) {
    state->tmp.selection.نقاب = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  جب(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    انتخاب(frag.str[0]) {
      حالت 'c':
        state->tmp.selection.نقاب |= VTERM_SELECTION_CLIPBOARD;
        توڑو;
      حالت 'p':
        state->tmp.selection.نقاب |= VTERM_SELECTION_PRIMARY;
        توڑو;
      حالت 'q':
        state->tmp.selection.نقاب |= VTERM_SELECTION_SECONDARY;
        توڑو;
      حالت 's':
        state->tmp.selection.نقاب |= VTERM_SELECTION_SELECT;
        توڑو;
      حالت '0':
      حالت '1':
      حالت '2':
      حالت '3':
      حالت '4':
      حالت '5':
      حالت '6':
      حالت '7':
        state->tmp.selection.نقاب |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        توڑو;

      حالت ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        اگر(!state->tmp.selection.نقاب)
          state->tmp.selection.نقاب = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        توڑو;
    }

    frag.str++;
    frag.len--;
  }

  اگر(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    اگر(frag.final && state->selection.callbacks->سیٹ_2) {
      (*state->selection.callbacks->سیٹ_2)(state->tmp.selection.نقاب, (VTermStringFragment){
              .str     = تہی,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_سیٹ,
              .final   = TRUE,
            }, state->selection.user);
    }
    واپس;
  }

  اگر(state->tmp.selection.state == SELECTION_SELECTED) {
    اگر(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    ورنہ {
      state->tmp.selection.state = SELECTION_سیٹ_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  اگر(state->tmp.selection.state == SELECTION_QUERY) {
    اگر(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.نقاب, state->selection.user);
    واپس;
  }

  اگر(state->tmp.selection.state == SELECTION_INVALID)
    واپس;

  اگر(state->selection.callbacks->سیٹ_2) {
    size_t bufcur = 0;
    حرف *بفر = state->selection.بفر;

    uint32_t x = 0; /* Current decoding value */
    صحیح n = 0;      /* Number of sextets consumed */

    اگر(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    جب((state->selection.buflen - bufcur) >= 3 && frag.len) {
      اگر(frag.str[0] == '=') {
        اگر(n == 2) {
          بفر[0] = (x >> 4) & 0xFF;
          بفر += 1, bufcur += 1;
        }
        اگر(n == 3) {
          بفر[0] = (x >> 10) & 0xFF;
          بفر[1] = (x >>  2) & 0xFF;
          بفر += 2, bufcur += 2;
        }

        جب(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      ورنہ {
        uint8_t b = unbase64one(frag.str[0]);
        اگر(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          اگر(state->selection.callbacks->سیٹ_2) {
            (*state->selection.callbacks->سیٹ_2)(state->tmp.selection.نقاب, (VTermStringFragment){
                .str     = تہی,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          توڑو;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        اگر(n == 4) {
          بفر[0] = (x >> 16) & 0xFF;
          بفر[1] = (x >>  8) & 0xFF;
          بفر[2] = (x >>  0) & 0xFF;

          بفر += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      اگر(!frag.len || (state->selection.buflen - bufcur) < 3) {
        اگر(bufcur) {
	  VTermStringFragment setfrag = {
	    state->selection.بفر, // str
	    bufcur, // len
	    state->tmp.selection.state == SELECTION_سیٹ_INITIAL, // initial
	    frag.final && !frag.len // final
	  };
          (*state->selection.callbacks->سیٹ_2)(state->tmp.selection.نقاب,
	      setfrag, state->selection.user);
          state->tmp.selection.state = SELECTION_سیٹ;
        }

        بفر = state->selection.بفر;
        bufcur = 0;
      }
    }

    اگر(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

ساکن صحیح on_osc(صحیح command, VTermStringFragment frag, خالی *user)
{
  VTermState *state = user;

  انتخاب(command) {
    حالت 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      واپس 1;

    حالت 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      واپس 1;

    حالت 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      واپس 1;

    حالت 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        صحیح red = state->default_fg.red;
        صحیح blue = state->default_fg.blue;
        صحیح green = state->default_fg.green;
        vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        واپس 1;
      }

    حالت 11:
      {
	// request background color: <Esc>]11;?<0x07>
	صحیح red = state->default_bg.red;
	صحیح blue = state->default_bg.blue;
	صحیح green = state->default_bg.green;
	vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	واپس 1;
      }
    حالت 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      واپس 1;

    حالت 52:
      اگر(state->selection.callbacks)
        osc_selection(state, frag);

      واپس 1;

    طےشدہ:
      اگر(state->fallbacks && state->fallbacks->osc)
        اگر((*state->fallbacks->osc)(command, frag, state->fbdata))
          واپس 1;
  }

  واپس 0;
}

ساکن خالی request_حالت_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  حرف *tmp = state->tmp.decrqss;

  اگر(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  جب(i < حجم(state->tmp.decrqss)-1 && tmp[i])
    i++;
  جب(i < حجم(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  اگر(!frag.final)
    واپس;

  انتخاب(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    حالت 'm': {
      // Query SGR
      طویل دلائل[20];
      صحیح argc = vterm_state_getpen(state, دلائل, حجم(دلائل)/حجم(دلائل[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->منوال.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      اگر(cur >= vt->tmpbuffer_len)
        واپس;

      برائے(صحیح argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_مزید(دلائل[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(دلائل[argi]));
        اگر(cur >= vt->tmpbuffer_len)
          واپس;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->منوال.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      اگر(cur >= vt->tmpbuffer_len)
        واپس;

      vterm_ڈالو_output_بائٹیں(vt, vt->tmpbuffer, cur);
      واپس;
    }

    حالت 'r':
      // Query DECSTBM
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_چوٹی+1, SCROLLREGION_BOTTOM(state));
      واپس;

    حالت 's':
      // Query DECSLRM
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_بایاں(state)+1, SCROLLREGION_دایاں(state));
      واپس;

    حالت ' '|('q'<<8): {
      // Query DECSCUSR
      صحیح جواب;
      انتخاب(state->منوال.cursor_shape) {
        حالت VTERM_PROP_CURSORSHAPE_بلاک:     جواب = 2; توڑو;
        حالت VTERM_PROP_CURSORSHAPE_UNDERLINE: جواب = 4; توڑو;
	طےشدہ: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ جواب = 6; توڑو;
      }
      اگر(state->منوال.cursor_blink)
        جواب--;
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", جواب);
      واپس;
    }

    حالت '\"'|('q'<<8):
      // Query DECSCA
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      واپس;
  }

  vterm_ڈالو_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ساکن صحیح on_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, خالی *user)
{
  VTermState *state = user;

  اگر(commandlen == 2 && strneq(command, "$q", 2)) {
    request_حالت_string(state, frag);
    واپس 1;
  }
  ورنہ اگر(state->fallbacks && state->fallbacks->dcs)
    اگر((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      واپس 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (صحیح)commandlen, command);
  واپس 0;
}

ساکن صحیح on_apc(VTermStringFragment frag, خالی *user)
{
  VTermState *state = user;

  اگر(state->fallbacks && state->fallbacks->apc)
    اگر((*state->fallbacks->apc)(frag, state->fbdata))
      واپس 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  واپس 0;
}

ساکن صحیح on_pm(VTermStringFragment frag, خالی *user)
{
  VTermState *state = user;

  اگر(state->fallbacks && state->fallbacks->pm)
    اگر((*state->fallbacks->pm)(frag, state->fbdata))
      واپس 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  واپس 0;
}

ساکن صحیح on_sos(VTermStringFragment frag, خالی *user)
{
  VTermState *state = user;

  اگر(state->fallbacks && state->fallbacks->sos)
    اگر((*state->fallbacks->sos)(frag, state->fbdata))
      واپس 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  واپس 0;
}

ساکن صحیح on_resize(صحیح صفیں_2, صحیح ستون_تعداد, خالی *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  اگر (ستون_تعداد > VTERM_MAX_ستون_تعداد)
    ستون_تعداد = VTERM_MAX_ستون_تعداد;

  اگر (صفیں_2 > VTERM_MAX_صفیں)
    صفیں_2 = VTERM_MAX_صفیں;

  اگر(ستون_تعداد != state->ستون_تعداد) {
    غیرعلامتی حرف *newtabstops = vterm_allocator_malloc(state->vt, (ستون_تعداد + 7) / 8);
    اگر (newtabstops == تہی)
      واپس 0;

    /* TODO: This can all be done much more efficiently bytewise */
    صحیح ستون;
    برائے(ستون = 0; ستون < state->ستون_تعداد && ستون < ستون_تعداد; ستون++) {
      غیرعلامتی حرف نقاب = 1 << (ستون & 7);
      اگر(state->tabstops[ستون >> 3] & نقاب)
        newtabstops[ستون >> 3] |= نقاب;
      ورنہ
        newtabstops[ستون >> 3] &= ~نقاب;
      }

    برائے( ; ستون < ستون_تعداد; ستون++) {
      غیرعلامتی حرف نقاب = 1 << (ستون & 7);
      اگر(ستون % 8 == 0)
        newtabstops[ستون >> 3] |= نقاب;
      ورنہ
        newtabstops[ستون >> 3] &= ~نقاب;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->صفیں_2 = صفیں_2;
  state->ستون_تعداد = ستون_تعداد;

  اگر(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->صفیں_2);
  اگر(state->scrollregion_دایاں > -1)
    UBOUND(state->scrollregion_دایاں, state->ستون_تعداد);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_چوٹی, state->صفیں_2);
  UBOUND(state->scrollregion_بایاں, state->ستون_تعداد);
  اگر(SCROLLREGION_BOTTOM(state) <= state->scrollregion_چوٹی) {
    state->scrollregion_چوٹی = 0;
    state->scrollregion_bottom = -1;
  }
  اگر(SCROLLREGION_دایاں(state) <= state->scrollregion_بایاں) {
    state->scrollregion_بایاں = 0;
    state->scrollregion_دایاں = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  اگر(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(صفیں_2, ستون_تعداد, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  ورنہ {
    اگر(صفیں_2 != state->صفیں_2) {
      برائے(صحیح bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_سطر_Info *oldlineinfo = state->lineinfos[bufidx];
        اگر(!oldlineinfo)
          جاری;

        VTerm_سطر_Info *newlineinfo = vterm_allocator_malloc(state->vt, صفیں_2 * حجم(VTerm_سطر_Info));

        صحیح صف;
        برائے(صف = 0; صف < state->صفیں_2 && صف < صفیں_2; صف++) {
          newlineinfo[صف] = oldlineinfo[صف];
        }

        برائے( ; صف < صفیں_2; صف++) {
          newlineinfo[صف] = (VTerm_سطر_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->منوال.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  اگر(state->at_phantom && state->pos.ستون < ستون_تعداد-1) {
    state->at_phantom = 0;
    state->pos.ستون++;
  }

  اگر(state->pos.صف < 0)
    state->pos.صف = 0;
  اگر(state->pos.صف >= صفیں_2)
    state->pos.صف = صفیں_2 - 1;
  اگر(state->pos.ستون < 0)
    state->pos.ستون = 0;
  اگر(state->pos.ستون >= ستون_تعداد)
    state->pos.ستون = ستون_تعداد - 1;

  updatecursor(state, &oldpos, 1);

  واپس 1;
}

ساکن ثابت VTermParserCallbacks parser_callbacks = {
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
  اگر(vt->state)
    واپس vt->state;

  VTermState *state = vterm_state_new(vt);
  اگر (state == تہی)
    واپس تہی;
  vt->state = state;

  vterm_parser_سیٹ_callbacks(vt, &parser_callbacks, state);

  واپس state;
}

خالی vterm_state_reset(VTermState *state, صحیح hard)
{
  state->scrollregion_چوٹی = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_بایاں = 0;
  state->scrollregion_دایاں = -1;

  state->منوال.keypad          = 0;
  state->منوال.cursor          = 0;
  state->منوال.autowrap        = 1;
  state->منوال.داخل_کرو          = 0;
  state->منوال.newline         = 0;
  state->منوال.alt_screen      = 0;
  state->منوال.origin          = 0;
  state->منوال.leftrightmargin = 0;
  state->منوال.bracketpaste    = 0;
  state->منوال.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->منوال.ctrl8bit   = 0;

  برائے(صحیح ستون = 0; ستون < state->ستون_تعداد; ستون++)
    اگر(ستون % 8 == 0)
      سیٹ_ستون_tabstop(state, ستون);
    ورنہ
      clear_ستون_tabstop(state, ستون);

  برائے(صحیح صف = 0; صف < state->صفیں_2; صف++)
    سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);

  اگر(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->منوال.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  برائے(صحیح i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    اگر(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].ڈیٹا);
  }

  state->gl_سیٹ = 0;
  state->gr_سیٹ = 1;
  state->gsingle_سیٹ = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_بلاک);

  اگر(hard) {
    state->pos.صف = 0;
    state->pos.ستون = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.انتہا_صف = state->صفیں_2;
    rect.انتہا_ستون =  state->ستون_تعداد;
    erase(state, rect, 0);
  }
}

خالی vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

خالی vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.ستون = state->mouse_ستون;
  mousestate->pos.صف = state->mouse_صف;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

خالی vterm_state_سیٹ_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, خالی *user)
{
  اگر(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    اگر(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  ورنہ {
    state->callbacks = تہی;
    state->cbdata = تہی;
  }
}

خالی vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

خالی *vterm_state_get_cbdata(VTermState *state)
{
  واپس state->cbdata;
}

خالی vterm_state_سیٹ_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, خالی *user)
{
  اگر(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  ورنہ {
    state->fallbacks = تہی;
    state->fbdata = تہی;
  }
}

خالی *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  واپس state->fbdata;
}

صحیح vterm_state_سیٹ_termprop(VTermState *state, VTermProp prop, VTerm_قدر *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  اگر(state->callbacks && state->callbacks->settermprop)
    اگر(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      واپس 0;

  انتخاب(prop) {
  حالت VTERM_PROP_TITLE:
  حالت VTERM_PROP_ICONNAME:
  حالت VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    واپس 1;
  حالت VTERM_PROP_CURSORVISIBLE:
    state->منوال.cursor_visible = val->boolean;
    واپس 1;
  حالت VTERM_PROP_CURSORBLINK:
    state->منوال.cursor_blink = val->boolean;
    واپس 1;
  حالت VTERM_PROP_CURSORSHAPE:
    state->منوال.cursor_shape = val->نمبر;
    واپس 1;
  حالت VTERM_PROP_الٹ:
    state->منوال.screen = val->boolean;
    واپس 1;
  حالت VTERM_PROP_ALTSCREEN:
    state->منوال.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->منوال.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    اگر(state->منوال.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.انتہا_صف = state->صفیں_2;
      rect.انتہا_ستون = state->ستون_تعداد;
      erase(state, rect, 0);
    }
    واپس 1;
  حالت VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    اگر(val->نمبر)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    اگر(val->نمبر == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    اگر(val->نمبر == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    واپس 1;
  حالت VTERM_PROP_FOCUSREPORT:
    state->منوال.report_focus = val->boolean;
    واپس 1;

  حالت VTERM_N_PROPS:
    واپس 0;
  }

  واپس 0;
}

خالی vterm_state_focus_اندر(VTermState *state)
{
  اگر(state->منوال.report_focus)
    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

خالی vterm_state_focus_برآمد(VTermState *state)
{
  اگر(state->منوال.report_focus)
    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ثابت VTerm_سطر_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحیح صف)
{
  واپس state->lineinfo + صف;
}

خالی vterm_state_سیٹ_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, خالی *user,
    حرف *بفر, size_t buflen)
{
  اگر(buflen && !بفر)
    بفر = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.بفر    = بفر;
  state->selection.buflen    = buflen;
}

خالی vterm_state_send_selection(VTermState *state, VTerm_Selection_نقاب نقاب, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  اگر(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ساکن حرف selection_chars[] = "cpqs";
    صحیح idx;
    برائے(idx = 0; idx < 4; idx++)
      اگر(نقاب & (1 << idx))
        توڑو;

    vterm_ڈالو_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  اگر(frag.len) {
    size_t bufcur = 0;
    حرف *بفر = state->selection.بفر;

    uint32_t x = 0;
    صحیح n = 0;

    اگر(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    جب((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      اگر(n == 3) {
        بفر[0] = base64_one((x >> 18) & 0x3F);
        بفر[1] = base64_one((x >> 12) & 0x3F);
        بفر[2] = base64_one((x >>  6) & 0x3F);
        بفر[3] = base64_one((x >>  0) & 0x3F);

        بفر += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      اگر(!frag.len || (state->selection.buflen - bufcur) < 4) {
        اگر(bufcur)
          vterm_ڈالو_output_بائٹیں(vt, state->selection.بفر, bufcur);

        بفر = state->selection.بفر;
        bufcur = 0;
      }
    }

    اگر(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  اگر(frag.final) {
    اگر(state->tmp.selection.sendpartial) {
      صحیح n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      حرف *بفر = state->selection.بفر;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      بفر[0] = base64_one((x >> 18) & 0x3F);
      بفر[1] = base64_one((x >> 12) & 0x3F);
      بفر[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      بفر[3] = '=';

      vterm_ڈالو_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, بفر);
    }
    ورنہ
      vterm_ڈالو_output_sprintf_str(vt, 0, TRUE, "");
  }
}
