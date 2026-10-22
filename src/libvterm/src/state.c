#ضمن "vterm_internal.h"

#ضمن <stdio.h>
#ضمن <string.h>

#تعریف_کرو strneq(a,b,n) (لڑی_مقابلہ(a,b,n)==0)

#إذا defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#جے_انت

ساكن صحيح on_resize(صحيح صفاں_2, صحيح خانے_گنتی, فراغ *user);

/* Some convenient wrappers to make callback functions easier */

ساكن فراغ putglyph(VTermState *state, ثابت uint32_t chars[], صحيح چوڑائی, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.چوڑائی = چوڑائی;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.صف].doublewidth;
  info.dhl = state->lineinfo[pos.صف].doubleheight;

  إذا(state->callbacks && state->callbacks->putglyph)
    إذا((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      أرجع;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.خانہ, pos.صف);
}

ساكن فراغ updatecursor(VTermState *state, VTermPos *oldpos, صحيح cancel_phantom)
{
  إذا(state->pos.خانہ == oldpos->خانہ && state->pos.صف == oldpos->صف)
    أرجع;

  إذا(cancel_phantom)
    state->at_phantom = 0;

  إذا(state->callbacks && state->callbacks->movecursor)
    إذا((*state->callbacks->movecursor)(state->pos, *oldpos, state->منوال.cursor_visible, state->cbdata))
      أرجع;
}

ساكن فراغ erase(VTermState *state, VTermRect rect, صحيح selective)
{
  إذا(rect.سرا_خانہ == state->خانے_گنتی) {
    صحيح صف;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    لكل(صف = rect.شروع_صف + 1; صف < rect.سرا_صف + 1 && صف < state->صفاں_2; صف++)
      state->lineinfo[صف].continuation = 0;
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

  state->صفاں_2 = vt->صفاں_2;
  state->خانے_گنتی = vt->خانے_گنتی;

  state->mouse_خانہ     = 0;
  state->mouse_صف     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = عدم;
  state->cbdata    = عدم;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = عدم;
  state->selection.user      = عدم;
  state->selection.بفر    = عدم;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_ناپ = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_ناپ * حجم(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->خانے_گنتی + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->صفاں_2 * حجم(VTerm_سطر_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->صفاں_2 * حجم(VTerm_سطر_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  إذا(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.انکڑے);

  أرجع state;
}

اندرونی فراغ vterm_state_free(VTermState *state)
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
  صحيح صفاں_2;
  صحيح خانے_گنتی;
  إذا(!downward && !rightward)
    أرجع;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  إذا(rect.سرا_صف <= rect.شروع_صف || rect.سرا_خانہ <= rect.شروع_خانہ)
    أرجع;

  صفاں_2 = rect.سرا_صف - rect.شروع_صف;
  إذا(downward > صفاں_2)
    downward = صفاں_2;
  وإلا إذا(downward < -صفاں_2)
    downward = -صفاں_2;

  خانے_گنتی = rect.سرا_خانہ - rect.شروع_خانہ;
  إذا(rightward > خانے_گنتی)
    rightward = خانے_گنتی;
  وإلا إذا(rightward < -خانے_گنتی)
    rightward = -خانے_گنتی;

  إذا(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    إذا(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .شروع_صف = rect.شروع_صف, .سرا_صف = rect.شروع_صف + downward,
          .شروع_خانہ = rect.شروع_خانہ, .سرا_خانہ = rect.سرا_خانہ}, state->cbdata);
    وإلا إذا(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .شروع_صف = rect.سرا_صف + downward, .سرا_صف = rect.سرا_صف,
          .شروع_خانہ = rect.شروع_خانہ,          .سرا_خانہ = rect.سرا_خانہ}, state->cbdata);

    إذا(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .شروع_صف = rect.شروع_صف, .سرا_صف = rect.سرا_صف,
          .شروع_خانہ = rect.شروع_خانہ, .سرا_خانہ = rect.شروع_خانہ + rightward}, state->cbdata);
    وإلا إذا(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .شروع_صف = rect.شروع_صف,           .سرا_صف = rect.سرا_صف,
          .شروع_خانہ = rect.سرا_خانہ + rightward, .سرا_خانہ = rect.سرا_خانہ}, state->cbdata);
  }

  // Update lineinfo if full line
  إذا(rect.شروع_خانہ == 0 && rect.سرا_خانہ == state->خانے_گنتی && rightward == 0) {
    صحيح اچائی = rect.سرا_صف - rect.شروع_صف - مطلق_قدر(downward);
    صحيح صف;
    VTerm_سطر_Info zero_سطر_Info = {0x0};

    إذا(downward > 0) {
      memmove(state->lineinfo + rect.شروع_صف,
              state->lineinfo + rect.شروع_صف + downward,
              اچائی * حجم(state->lineinfo[0]));
      لكل(صف = rect.سرا_صف - downward; صف < rect.سرا_صف; صف++)
        state->lineinfo[صف] = zero_سطر_Info;
    }
    وإلا {
      memmove(state->lineinfo + rect.شروع_صف - downward,
              state->lineinfo + rect.شروع_صف,
              اچائی * حجم(state->lineinfo[0]));
      لكل(صف = rect.شروع_صف; صف < rect.شروع_صف - downward; صف++)
        state->lineinfo[صف] = zero_سطر_Info;
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
  إذا(state->pos.صف == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.شروع_صف = state->scrollregion_چوٹی;
    rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_خانہ = SCROLLREGION_بایاں(state);
    rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

    scroll(state, rect, 1, 0);
  }
  وإلا إذا(state->pos.صف < state->صفاں_2-1)
    state->pos.صف++;
}

ساكن فراغ grow_combine_بفر(VTermState *state)
{
  size_t    new_ناپ = state->combine_chars_ناپ * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_ناپ * حجم(new_chars[0]));

  حافظہ_نقل(new_chars, state->combine_chars, state->combine_chars_ناپ * حجم(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_ناپ = new_ناپ;
}

ساكن فراغ سیٹ_خانہ_tabstop(VTermState *state, صحيح خانہ)
{
  بنانشانی حرف نقاب = 1 << (خانہ & 7);
  state->tabstops[خانہ >> 3] |= نقاب;
}

ساكن فراغ clear_خانہ_tabstop(VTermState *state, صحيح خانہ)
{
  بنانشانی حرف نقاب = 1 << (خانہ & 7);
  state->tabstops[خانہ >> 3] &= ~نقاب;
}

ساكن صحيح is_خانہ_tabstop(VTermState *state, صحيح خانہ)
{
  بنانشانی حرف نقاب = 1 << (خانہ & 7);
  أرجع state->tabstops[خانہ >> 3] & نقاب;
}

ساكن صحيح is_cursor_وچ_scrollregion(ثابت VTermState *state)
{
  إذا(state->pos.صف < state->scrollregion_چوٹی ||
     state->pos.صف >= SCROLLREGION_BOTTOM(state))
    أرجع 0;
  إذا(state->pos.خانہ < SCROLLREGION_بایاں(state) ||
     state->pos.خانہ >= SCROLLREGION_دایاں(state))
    أرجع 0;

  أرجع 1;
}

ساكن فراغ tab(VTermState *state, صحيح گنتی, صحيح direction)
{
  طالما(گنتی > 0) {
    إذا(direction > 0) {
      إذا(state->pos.خانہ >= THISROWWIDTH(state)-1)
        أرجع;

      state->pos.خانہ++;
    }
    وإلا إذا(direction < 0) {
      إذا(state->pos.خانہ < 1)
        أرجع;

      state->pos.خانہ--;
    }

    إذا(is_خانہ_tabstop(state, state->pos.خانہ))
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

ساكن فراغ سیٹ_lineinfo(VTermState *state, صحيح صف, صحيح force, صحيح dwl, صحيح dhl)
{
  VTerm_سطر_Info info = state->lineinfo[صف];

  إذا(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  وإلا إذا(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  إذا(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  وإلا إذا(dhl == DHL_چوٹی)
    info.doubleheight = DHL_چوٹی;
  وإلا إذا(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  إذا((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(صف, &info, state->lineinfo + صف, state->cbdata))
      || force)
    state->lineinfo[صف] = info;
}

ساكن صحيح on_متن(ثابت حرف بائٹاں[], size_t len, فراغ *user)
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
    state->gsingle_سیٹ     ? &state->encoding[state->gsingle_سیٹ] :
    !(بائٹاں[eaten] & 0x80) ? &state->encoding[state->gl_سیٹ] :
    state->vt->منوال.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_سیٹ];
  إذا (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->انکڑے,
      codepoints, &npoints, state->gsingle_سیٹ ? 1 : (صحيح)maxpoints,
      بائٹاں, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  إذا(!npoints)
  {
    أرجع (صحيح)eaten;
  }

  إذا(state->gsingle_سیٹ && npoints)
    state->gsingle_سیٹ = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  إذا(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    إذا(state->pos.صف == state->combine_pos.صف && state->pos.خانہ == state->combine_pos.خانہ + state->combine_چوڑائی) {
#جے_تعریف_شدہ DEBUG_GLYPH_COMBINE
      صحيح printpos;
      لکھو("DEBUG: COMBINING SPLIT GLYPH of chars {");
      لكل(printpos = 0; state->combine_chars[printpos]; printpos++)
        لکھو("U+%04x ", state->combine_chars[printpos]);
      لکھو("} + {");
#جے_انت

      /* Find where we need to append these combining chars */
      صحيح saved_i = 0;
      طالما(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      طالما(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        إذا(saved_i >= (صحيح)state->combine_chars_ناپ)
          grow_combine_بفر(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      إذا(saved_i >= (صحيح)state->combine_chars_ناپ)
        grow_combine_بفر(state);
      state->combine_chars[saved_i] = 0;

#جے_تعریف_شدہ DEBUG_GLYPH_COMBINE
      لكل(; state->combine_chars[printpos]; printpos++)
        لکھو("U+%04x ", state->combine_chars[printpos]);
      لکھو("}\n");
#جے_انت

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_چوڑائی, state->combine_pos);
    }
    وإلا {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  لكل(; i < npoints; i++) {
    // Try to find combining characters following this
    صحيح glyph_starts = i;
    صحيح glyph_سرے;
    صحيح چوڑائی = 0;

    لكل(glyph_سرے = i + 1;
        (glyph_سرے < npoints) && (glyph_سرے < glyph_starts + VTERM_MAX_CHARS_فی_CELL);
        glyph_سرے++)
      إذا(!vterm_unicode_is_combining(codepoints[glyph_سرے]))
        اكسر;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_فی_CELL + 1) * حجم(uint32_t));
    إذا (chars == عدم)
      اكسر;

    لكل( ; i < glyph_سرے; i++) {
      صحيح this_چوڑائی;
      إذا(vterm_get_special_pty_type() == 2) {
        state->vt->وچ_backspace -= (state->vt->وچ_backspace > 0) ? 1 : 0;
        إذا(state->vt->وچ_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_چوڑائی = vterm_unicode_چوڑائی(codepoints[i]);
#جے_تعریف_شدہ DEBUG
      إذا(this_چوڑائی < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#جے_انت
      إذا (i == glyph_starts || this_چوڑائی > چوڑائی)
	چوڑائی = this_چوڑائی;  // TODO: should be += ?
    }

    إذا (چوڑائی < 0)
      چوڑائی = 0;

    طالما(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_سرے - glyph_starts] = 0;
    i--;

#جے_تعریف_شدہ DEBUG_GLYPH_COMBINE
    صحيح printpos;
    لکھو("DEBUG: COMBINED GLYPH of %d chars {", glyph_سرے - glyph_starts);
    لكل(printpos = 0; printpos < glyph_سرے - glyph_starts; printpos++)
      لکھو("U+%04x ", chars[printpos]);
    لکھو("}, onscreen width %d\n", چوڑائی);
#جے_انت

    إذا(state->at_phantom || state->pos.خانہ + چوڑائی > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.خانہ = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.صف].continuation = 1;
    }

    إذا(state->منوال.داخل_کرو) {
      // TODO: This will be a little inefficient for large bodies of text, as
      // it'll have to 'ICH' effectively before every glyph. We should scan
      // ahead and ICH as many times as required
      VTermRect rect;
      rect.شروع_صف = state->pos.صف;
      rect.سرا_صف   = state->pos.صف + 1;
      rect.شروع_خانہ = state->pos.خانہ;
      rect.سرا_خانہ   = THISROWWIDTH(state);
      scroll(state, rect, 0, -1);
    }

    putglyph(state, chars, چوڑائی, state->pos);

    إذا(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      صحيح save_i;
      لكل(save_i = 0; chars[save_i]; save_i++) {
        إذا(save_i >= (صحيح)state->combine_chars_ناپ)
          grow_combine_بفر(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      إذا(save_i >= (صحيح)state->combine_chars_ناپ)
        grow_combine_بفر(state);
      state->combine_chars[save_i] = 0;
      state->combine_چوڑائی = چوڑائی;
      state->combine_pos = state->pos;
    }

    إذا(state->pos.خانہ + چوڑائی >= THISROWWIDTH(state)) {
      إذا(state->منوال.autowrap)
        state->at_phantom = 1;
    }
    وإلا {
      state->pos.خانہ += چوڑائی;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#جے_تعریف_شدہ DEBUG
  إذا(state->pos.صف < 0 || state->pos.صف >= state->صفاں_2 ||
     state->pos.خانہ < 0 || state->pos.خانہ >= state->خانے_گنتی) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.صف, state->pos.خانہ);
    abort();
  }
#جے_انت

  أرجع (صحيح)eaten;
}

ساكن صحيح on_control(بنانشانی حرف control, فراغ *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.خانہ -= (leadpos.خانہ >= 2 ? 2 : 0);

  بدل(control) {
  حالة 0x07: // BEL - ECMA-48 8.3.3
    إذا(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    اكسر;

  حالة 0x08: // BS - ECMA-48 8.3.5
    إذا(state->pos.خانہ > 0)
      state->pos.خانہ--;
    إذا(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      إذا(vterm_unicode_چوڑائی(cell.chars[0]) == 2)
        state->pos.خانہ--;
    }
    اكسر;

  حالة 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    اكسر;

  حالة 0x0a: // LF - ECMA-48 8.3.74
  حالة 0x0b: // VT
  حالة 0x0c: // FF
    linefeed(state);
    إذا(state->منوال.newline)
      state->pos.خانہ = 0;
    اكسر;

  حالة 0x0d: // CR - ECMA-48 8.3.15
    state->pos.خانہ = 0;
    اكسر;

  حالة 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_سیٹ = 1;
    اكسر;

  حالة 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_سیٹ = 0;
    اكسر;

  حالة 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    اكسر;

  حالة 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.خانہ = 0;
    اكسر;

  حالة 0x88: // HTS - ECMA-48 8.3.62
    سیٹ_خانہ_tabstop(state, state->pos.خانہ);
    اكسر;

  حالة 0x8d: // RI - ECMA-48 8.3.104
    إذا(state->pos.صف == state->scrollregion_چوٹی) {
      VTermRect rect;
      rect.شروع_صف = state->scrollregion_چوٹی;
      rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
      rect.شروع_خانہ = SCROLLREGION_بایاں(state);
      rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

      scroll(state, rect, -1, 0);
    }
    وإلا إذا(state->pos.صف > 0)
        state->pos.صف--;
    اكسر;

  حالة 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_سیٹ = 2;
    اكسر;

  حالة 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_سیٹ = 3;
    اكسر;

  افتراضي:
    إذا(state->fallbacks && state->fallbacks->control)
      إذا((*state->fallbacks->control)(control, state->fbdata))
        أرجع 1;

    أرجع 0;
  }

  updatecursor(state, &oldpos, 1);

#جے_تعریف_شدہ DEBUG
  إذا(state->pos.صف < 0 || state->pos.صف >= state->صفاں_2 ||
     state->pos.خانہ < 0 || state->pos.خانہ >= state->خانے_گنتی) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.صف, state->pos.خانہ);
    abort();
  }
#جے_انت

  أرجع 1;
}

ساكن صحيح settermprop_bool(VTermState *state, VTermProp prop, صحيح v)
{
  VTerm_قدر val;
  val.boolean = v;
  أرجع vterm_state_سیٹ_termprop(state, prop, &val);
}

ساكن صحيح settermprop_int(VTermState *state, VTermProp prop, صحيح v)
{
  VTerm_قدر val;
  val.نمبر = v;
  أرجع vterm_state_سیٹ_termprop(state, prop, &val);
}

ساكن صحيح settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_قدر val;

  val.string = frag;
  أرجع vterm_state_سیٹ_termprop(state, prop, &val);
}

ساكن فراغ savecursor(VTermState *state, صحيح save)
{
  إذا(save) {
    state->saved.pos = state->pos;
    state->saved.منوال.cursor_visible = state->منوال.cursor_visible;
    state->saved.منوال.cursor_blink   = state->منوال.cursor_blink;
    state->saved.منوال.cursor_shape   = state->منوال.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  وإلا {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.منوال.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.منوال.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.منوال.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ساكن صحيح on_escape(ثابت حرف *بائٹاں, size_t len, فراغ *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  بدل(بائٹاں[0]) {
  حالة ' ':
    إذا(len != 2)
      أرجع 0;

    بدل(بائٹاں[1]) {
      حالة 'F': // S7C1T
        state->vt->منوال.ctrl8bit = 0;
        اكسر;

      حالة 'G': // S8C1T
        state->vt->منوال.ctrl8bit = 1;
        اكسر;

      افتراضي:
        أرجع 0;
    }
    أرجع 2;

  حالة '#':
    إذا(len != 2)
      أرجع 0;

    بدل(بائٹاں[1]) {
      حالة '3': // DECDHL top
        إذا(state->منوال.leftrightmargin)
          اكسر;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_ON, DHL_چوٹی);
        اكسر;

      حالة '4': // DECDHL bottom
        إذا(state->منوال.leftrightmargin)
          اكسر;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_ON, DHL_BOTTOM);
        اكسر;

      حالة '5': // DECSWL
        إذا(state->منوال.leftrightmargin)
          اكسر;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_OFF, DHL_OFF);
        اكسر;

      حالة '6': // DECDWL
        إذا(state->منوال.leftrightmargin)
          اكسر;
        سیٹ_lineinfo(state, state->pos.صف, NO_FORCE, DWL_ON, DHL_OFF);
        اكسر;

      حالة '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        لكل(pos.صف = 0; pos.صف < state->صفاں_2; pos.صف++)
          لكل(pos.خانہ = 0; pos.خانہ < ROWWIDTH(state, pos.صف); pos.خانہ++)
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
      صحيح setnum = بائٹاں[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, بائٹاں[1]);

      إذا(newenc) {
        state->encoding[setnum].enc = newenc;

        إذا(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].انکڑے);
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
    state->منوال.keypad = 1;
    أرجع 1;

  حالة '>': // DECKPNM
    state->منوال.keypad = 0;
    أرجع 1;

  حالة 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    إذا(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->منوال.cursor_visible, state->cbdata);
    أرجع 1;
  }

  حالة 'n': // LS2 - ECMA-48 8.3.78
    state->gl_سیٹ = 2;
    أرجع 1;

  حالة 'o': // LS3 - ECMA-48 8.3.80
    state->gl_سیٹ = 3;
    أرجع 1;

  حالة '~': // LS1R - ECMA-48 8.3.77
    state->gr_سیٹ = 1;
    أرجع 1;

  حالة '}': // LS2R - ECMA-48 8.3.79
    state->gr_سیٹ = 2;
    أرجع 1;

  حالة '|': // LS3R - ECMA-48 8.3.81
    state->gr_سیٹ = 3;
    أرجع 1;

  افتراضي:
    أرجع 0;
  }
}

ساكن فراغ سیٹ_منوال(VTermState *state, صحيح عدد, صحيح val)
{
  بدل(عدد) {
  حالة 4: // IRM - ECMA-48 7.2.10
    state->منوال.داخل_کرو = val;
    اكسر;

  حالة 20: // LNM - ANSI X3.4-1977
    state->منوال.newline = val;
    اكسر;

  افتراضي:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", عدد);
    أرجع;
  }
}

ساكن فراغ سیٹ_dec_منوال(VTermState *state, صحيح عدد, صحيح val)
{
  بدل(عدد) {
  حالة 1:
    state->منوال.cursor = val;
    اكسر;

  حالة 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_الٹ, val);
    اكسر;

  حالة 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->منوال.origin = val;
      state->pos.صف = state->منوال.origin ? state->scrollregion_چوٹی : 0;
      state->pos.خانہ = state->منوال.origin ? SCROLLREGION_بایاں(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    اكسر;

  حالة 7:
    state->منوال.autowrap = val;
    اكسر;

  حالة 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    اكسر;

  حالة 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    اكسر;

  حالة 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->منوال.leftrightmargin = val;
    إذا(val) {
      صحيح صف;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      لكل(صف = 0; صف < state->صفاں_2; صف++)
        سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
    }

    اكسر;

  حالة 1000:
  حالة 1002:
  حالة 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (عدد == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (عدد == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    اكسر;

  حالة 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->منوال.report_focus = val;
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
    state->منوال.bracketpaste = val;
    اكسر;

  افتراضي:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", عدد);
    أرجع;
  }
}

ساكن فراغ request_dec_منوال(VTermState *state, صحيح عدد)
{
  صحيح جواب;

  بدل(عدد) {
    حالة 1:
      جواب = state->منوال.cursor;
      اكسر;

    حالة 5:
      جواب = state->منوال.screen;
      اكسر;

    حالة 6:
      جواب = state->منوال.origin;
      اكسر;

    حالة 7:
      جواب = state->منوال.autowrap;
      اكسر;

    حالة 12:
      جواب = state->منوال.cursor_blink;
      اكسر;

    حالة 25:
      جواب = state->منوال.cursor_visible;
      اكسر;

    حالة 69:
      جواب = state->منوال.leftrightmargin;
      اكسر;

    حالة 1000:
      جواب = state->mouse_flags == MOUSE_WANT_CLICK;
      اكسر;

    حالة 1002:
      جواب = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      اكسر;

    حالة 1003:
      جواب = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      اكسر;

    حالة 1004:
      جواب = state->منوال.report_focus;
      اكسر;

    حالة 1005:
      جواب = state->mouse_protocol == MOUSE_UTF8;
      اكسر;

    حالة 1006:
      جواب = state->mouse_protocol == MOUSE_SGR;
      اكسر;

    حالة 1015:
      جواب = state->mouse_protocol == MOUSE_RXVT;
      اكسر;

    حالة 1047:
      جواب = state->منوال.alt_screen;
      اكسر;

    حالة 2004:
      جواب = state->منوال.bracketpaste;
      اكسر;

    افتراضي:
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", عدد, 0);
      أرجع;
  }

  vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", عدد, جواب ? 1 : 2);
}

ساكن فراغ request_نسخہ_string(VTermState *state)
{
  vterm_ڈالو_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_نسخہ_MAJOR, VTERM_نسخہ_MINOR);
}

ساكن صحيح on_csi(ثابت حرف *leader, ثابت لما دلیلاں[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user)
{
  VTermState *state = user;
  صحيح leader_byte = 0;
  صحيح intermed_byte = 0;
  صحيح cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  صحيح handled = 1;

  // Some temporaries for later code
  صحيح گنتی, val;
  صحيح صف, خانہ;
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

#تعریف_کرو LBOUND(v,min) إذا((v) < (min)) (v) = (min)
#تعریف_کرو UBOUND(v,max) إذا((v) > (max)) (v) = (max)

#تعریف_کرو LEADER(l,b) ((l << 8) | b)
#تعریف_کرو INTERMED(i,b) ((i << 16) | b)

  بدل(intermed_byte << 16 | leader_byte << 8 | command) {
  حالة 0x40: // ICH - ECMA-48 8.3.64
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    إذا(!is_cursor_وچ_scrollregion(state))
      اكسر;

    rect.شروع_صف = state->pos.صف;
    rect.سرا_صف   = state->pos.صف + 1;
    rect.شروع_خانہ = state->pos.خانہ;
    إذا(state->منوال.leftrightmargin)
      rect.سرا_خانہ = SCROLLREGION_دایاں(state);
    وإلا
      rect.سرا_خانہ = THISROWWIDTH(state);

    scroll(state, rect, 0, -گنتی);

    اكسر;

  حالة 0x41: // CUU - ECMA-48 8.3.22
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.صف -= گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x42: // CUD - ECMA-48 8.3.19
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.صف += گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x43: // CUF - ECMA-48 8.3.20
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.خانہ += گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x44: // CUB - ECMA-48 8.3.18
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.خانہ -= گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x45: // CNL - ECMA-48 8.3.12
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.خانہ = 0;
    state->pos.صف += گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x46: // CPL - ECMA-48 8.3.13
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.خانہ = 0;
    state->pos.صف -= گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(دلیلاں[0], 1);
    state->pos.خانہ = val-1;
    state->at_phantom = 0;
    اكسر;

  حالة 0x48: // CUP - ECMA-48 8.3.21
    صف = CSI_ARG_OR(دلیلاں[0], 1);
    خانہ = argcount < 2 || CSI_ARG_IS_MISSING(دلیلاں[1]) ? 1 : CSI_ARG(دلیلاں[1]);
    // zero-based
    إذا(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      إذا(state->pos.صف == صف - 1) {
        صحيح cnt, ptr = 0;
        لكل(cnt = 0; cnt < خانہ - 1; ++cnt) {
	  VTermPos p;
	  VTermScreenCell c0, c1;
	  p.صف = صف - 1;
	  p.خانہ = ptr;
	  vterm_screen_get_cell(state->vt->screen, p, &c0);
	  p.خانہ++;
	  vterm_screen_get_cell(state->vt->screen, p, &c1);
	  ptr += (c1.chars[0] == (uint32_t)-1)		    // double cell?
	     ? (vterm_unicode_is_ambiguous(c0.chars[0]))    // is ambiguous?
	     ? vterm_unicode_چوڑائی(0x00a1) : 1		    // &ambiwidth
	     : 1;					    // not ambiguous
        }
        خانہ = ptr + 1;
      }
    }
    state->pos.صف = صف-1;
    state->pos.خانہ = خانہ-1;
    إذا(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.خانہ += SCROLLREGION_بایاں(state);
    }
    state->at_phantom = 0;
    اكسر;

  حالة 0x49: // CHT - ECMA-48 8.3.10
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    tab(state, گنتی, +1);
    اكسر;

  حالة 0x4a: // ED - ECMA-48 8.3.39
  حالة LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    بدل(CSI_ARG(دلیلاں[0])) {
    حالة CSI_ARG_MISSING:
    حالة 0:
      rect.شروع_صف = state->pos.صف; rect.سرا_صف = state->pos.صف + 1;
      rect.شروع_خانہ = state->pos.خانہ; rect.سرا_خانہ = state->خانے_گنتی;
      إذا(rect.سرا_خانہ > rect.شروع_خانہ)
        erase(state, rect, selective);

      rect.شروع_صف = state->pos.صف + 1; rect.سرا_صف = state->صفاں_2;
      rect.شروع_خانہ = 0;
      لكل(صف = rect.شروع_صف; صف < rect.سرا_صف; صف++)
	سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
      إذا(rect.سرا_صف > rect.شروع_صف)
        erase(state, rect, selective);
      اكسر;

    حالة 1:
      rect.شروع_صف = 0; rect.سرا_صف = state->pos.صف;
      rect.شروع_خانہ = 0; rect.سرا_خانہ = state->خانے_گنتی;
      لكل(صف = rect.شروع_صف; صف < rect.سرا_صف; صف++)
	سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
      إذا(rect.سرا_خانہ > rect.شروع_خانہ)
        erase(state, rect, selective);

      rect.شروع_صف = state->pos.صف; rect.سرا_صف = state->pos.صف + 1;
                          rect.سرا_خانہ = state->pos.خانہ + 1;
      إذا(rect.سرا_صف > rect.شروع_صف)
        erase(state, rect, selective);
      اكسر;

    حالة 2:
      rect.شروع_صف = 0; rect.سرا_صف = state->صفاں_2;
      rect.شروع_خانہ = 0; rect.سرا_خانہ = state->خانے_گنتی;
      لكل(صف = rect.شروع_صف; صف < rect.سرا_صف; صف++)
	سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);
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
    rect.شروع_صف = state->pos.صف;
    rect.سرا_صف   = state->pos.صف + 1;

    بدل(CSI_ARG(دلیلاں[0])) {
    حالة CSI_ARG_MISSING:
    حالة 0:
      rect.شروع_خانہ = state->pos.خانہ; rect.سرا_خانہ = THISROWWIDTH(state); اكسر;
    حالة 1:
      rect.شروع_خانہ = 0; rect.سرا_خانہ = state->pos.خانہ + 1; اكسر;
    حالة 2:
      rect.شروع_خانہ = 0; rect.سرا_خانہ = THISROWWIDTH(state); اكسر;
    افتراضي:
      أرجع 0;
    }

    إذا(rect.سرا_خانہ > rect.شروع_خانہ)
      erase(state, rect, selective);

    اكسر;

  حالة 0x4c: // IL - ECMA-48 8.3.67
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    إذا(!is_cursor_وچ_scrollregion(state))
      اكسر;

    rect.شروع_صف = state->pos.صف;
    rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_خانہ = SCROLLREGION_بایاں(state);
    rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

    scroll(state, rect, -گنتی, 0);

    اكسر;

  حالة 0x4d: // DL - ECMA-48 8.3.32
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    إذا(!is_cursor_وچ_scrollregion(state))
      اكسر;

    rect.شروع_صف = state->pos.صف;
    rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_خانہ = SCROLLREGION_بایاں(state);
    rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

    scroll(state, rect, گنتی, 0);

    اكسر;

  حالة 0x50: // DCH - ECMA-48 8.3.26
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    إذا(!is_cursor_وچ_scrollregion(state))
      اكسر;

    rect.شروع_صف = state->pos.صف;
    rect.سرا_صف   = state->pos.صف + 1;
    rect.شروع_خانہ = state->pos.خانہ;
    إذا(state->منوال.leftrightmargin)
      rect.سرا_خانہ = SCROLLREGION_دایاں(state);
    وإلا
      rect.سرا_خانہ = THISROWWIDTH(state);

    scroll(state, rect, 0, گنتی);

    اكسر;

  حالة 0x53: // SU - ECMA-48 8.3.147
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    rect.شروع_صف = state->scrollregion_چوٹی;
    rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_خانہ = SCROLLREGION_بایاں(state);
    rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

    scroll(state, rect, گنتی, 0);

    اكسر;

  حالة 0x54: // SD - ECMA-48 8.3.113
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    rect.شروع_صف = state->scrollregion_چوٹی;
    rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_خانہ = SCROLLREGION_بایاں(state);
    rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

    scroll(state, rect, -گنتی, 0);

    اكسر;

  حالة 0x58: // ECH - ECMA-48 8.3.38
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    rect.شروع_صف = state->pos.صف;
    rect.سرا_صف   = state->pos.صف + 1;
    rect.شروع_خانہ = state->pos.خانہ;
    rect.سرا_خانہ   = state->pos.خانہ + گنتی;
    UBOUND(rect.سرا_خانہ, THISROWWIDTH(state));

    erase(state, rect, 0);
    اكسر;

  حالة 0x5a: // CBT - ECMA-48 8.3.7
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    tab(state, گنتی, -1);
    اكسر;

  حالة 0x60: // HPA - ECMA-48 8.3.57
    خانہ = CSI_ARG_OR(دلیلاں[0], 1);
    state->pos.خانہ = خانہ-1;
    state->at_phantom = 0;
    اكسر;

  حالة 0x61: // HPR - ECMA-48 8.3.59
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.خانہ += گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x62: { // REP - ECMA-48 8.3.103
    ثابت صحيح صف_چوڑائی = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    إذا(state->combine_چوڑائی < 1)
      اكسر;

    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    خانہ = state->pos.خانہ + گنتی;
    UBOUND(خانہ, صف_چوڑائی);
    طالما (state->pos.خانہ < خانہ) {
      putglyph(state, state->combine_chars, state->combine_چوڑائی, state->pos);
      state->pos.خانہ += state->combine_چوڑائی;
    }
    إذا (state->pos.خانہ + state->combine_چوڑائی >= صف_چوڑائی) {
      إذا (state->منوال.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    اكسر;
  }

  حالة 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(دلیلاں[0], 0);
    إذا(val == 0)
      // DEC VT100 response
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    اكسر;

  حالة LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    اكسر;

  حالة 0x64: // VPA - ECMA-48 8.3.158
    صف = CSI_ARG_OR(دلیلاں[0], 1);
    state->pos.صف = صف-1;
    إذا(state->منوال.origin)
      state->pos.صف += state->scrollregion_چوٹی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x65: // VPR - ECMA-48 8.3.160
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.صف += گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x66: // HVP - ECMA-48 8.3.63
    صف = CSI_ARG_OR(دلیلاں[0], 1);
    خانہ = argcount < 2 || CSI_ARG_IS_MISSING(دلیلاں[1]) ? 1 : CSI_ARG(دلیلاں[1]);
    // zero-based
    state->pos.صف = صف-1;
    state->pos.خانہ = خانہ-1;
    إذا(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.خانہ += SCROLLREGION_بایاں(state);
    }
    state->at_phantom = 0;
    اكسر;

  حالة 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(دلیلاں[0], 0);

    بدل(val) {
    حالة 0:
      clear_خانہ_tabstop(state, state->pos.خانہ);
      اكسر;
    حالة 3:
    حالة 5:
      لكل(خانہ = 0; خانہ < state->خانے_گنتی; خانہ++)
        clear_خانہ_tabstop(state, خانہ);
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
    إذا(!CSI_ARG_IS_MISSING(دلیلاں[0]))
      سیٹ_منوال(state, CSI_ARG(دلیلاں[0]), 1);
    اكسر;

  حالة LEADER('?', 0x68): // DEC private mode set
    لكل(صحيح i = 0; i < argcount; i++) {
      إذا(!CSI_ARG_IS_MISSING(دلیلاں[i]))
        سیٹ_dec_منوال(state, CSI_ARG(دلیلاں[i]), 1);
    }
    اكسر;

  حالة 0x6a: // HPB - ECMA-48 8.3.58
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.خانہ -= گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6b: // VPB - ECMA-48 8.3.159
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);
    state->pos.صف -= گنتی;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6c: // RM - ECMA-48 8.3.106
    إذا(!CSI_ARG_IS_MISSING(دلیلاں[0]))
      سیٹ_منوال(state, CSI_ARG(دلیلاں[0]), 0);
    اكسر;

  حالة LEADER('?', 0x6c): // DEC private mode reset
    لكل(صحيح i = 0; i < argcount; i++) {
      إذا(!CSI_ARG_IS_MISSING(دلیلاں[i]))
        سیٹ_dec_منوال(state, CSI_ARG(دلیلاں[i]), 0);
    }
    اكسر;

  حالة 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, دلیلاں, argcount);
    اكسر;

  حالة LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    إذا (argcount == 1 && CSI_ARG(دلیلاں[0]) == 4)
    {
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->منوال.modify_other_کلیدیں ? 2 : 0);
      اكسر;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    لكل(صحيح argi = 0; argi < argcount; argi++) {
      لما arg;
      بدل(arg = CSI_ARG(دلیلاں[argi])) {
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
    إذا (argcount == 2 && CSI_ARG(دلیلاں[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->منوال.kitty_keyboard = 0;

      state->منوال.modify_other_کلیدیں = CSI_ARG(دلیلاں[1]) == 2;
    }
    اكسر;

  حالة LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    إذا (argcount == 1 && CSI_ARG(دلیلاں[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->منوال.modify_other_کلیدیں = 0;

      state->منوال.kitty_keyboard = 1;
    }
    اكسر;

  حالة LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    إذا (argcount <= 1)
      state->منوال.kitty_keyboard = 0;
    اكسر;

  حالة LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    إذا (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->منوال.kitty_keyboard);
    اكسر;

  حالة 0x6e: // DSR - ECMA-48 8.3.35
  حالة LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(دلیلاں[0], 0);

    {
      حرف *qmark = (leader_byte == '?') ? "?" : "";

      بدل(val) {
      حالة 0: حالة 1: حالة 2: حالة 3: حالة 4:
        // ignore - these are replies
        اكسر;
      حالة 5:
        vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        اكسر;
      حالة 6: // CPR - cursor position report
        vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.صف + 1, state->pos.خانہ + 1);
        اكسر;
      }
    }
    اكسر;


  حالة INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    اكسر;

  حالة LEADER('?', INTERMED('$', 0x70)):
    request_dec_منوال(state, CSI_ARG(دلیلاں[0]));
    اكسر;

  حالة LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_نسخہ_string(state);
    اكسر;

  حالة INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(دلیلاں[0], 1);

    بدل(val) {
    حالة 0: حالة 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_بلاک);
      اكسر;
    حالة 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_بلاک);
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
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_بایاں);
      اكسر;
    حالة 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_بایاں);
      اكسر;
    }

    اكسر;

  حالة INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(دلیلاں[0], 0);

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
    state->scrollregion_چوٹی = CSI_ARG_OR(دلیلاں[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(دلیلاں[1]) ? -1 : CSI_ARG(دلیلاں[1]);
    LBOUND(state->scrollregion_چوٹی, 0);
    UBOUND(state->scrollregion_چوٹی, state->صفاں_2);
    LBOUND(state->scrollregion_bottom, -1);
    إذا(state->scrollregion_چوٹی == 0 && state->scrollregion_bottom == state->صفاں_2)
      state->scrollregion_bottom = -1;
    وإلا
      UBOUND(state->scrollregion_bottom, state->صفاں_2);

    إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_چوٹی) {
      // Invalid
      state->scrollregion_چوٹی    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.صف = 0;
    state->pos.خانہ = 0;
    إذا(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.خانہ += SCROLLREGION_بایاں(state);
    }

    اكسر;

  حالة 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_بایاں = CSI_ARG_OR(دلیلاں[0], 1) - 1;
    state->scrollregion_دایاں = argcount < 2 || CSI_ARG_IS_MISSING(دلیلاں[1]) ? -1 : CSI_ARG(دلیلاں[1]);
    LBOUND(state->scrollregion_بایاں, 0);
    UBOUND(state->scrollregion_بایاں, state->خانے_گنتی);
    LBOUND(state->scrollregion_دایاں, -1);
    إذا(state->scrollregion_بایاں == 0 && state->scrollregion_دایاں == state->خانے_گنتی)
      state->scrollregion_دایاں = -1;
    وإلا
      UBOUND(state->scrollregion_دایاں, state->خانے_گنتی);

    إذا(state->scrollregion_دایاں > -1 &&
       state->scrollregion_دایاں <= state->scrollregion_بایاں) {
      // Invalid
      state->scrollregion_بایاں  = 0;
      state->scrollregion_دایاں = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.صف = 0;
    state->pos.خانہ = 0;
    إذا(state->منوال.origin) {
      state->pos.صف += state->scrollregion_چوٹی;
      state->pos.خانہ += SCROLLREGION_بایاں(state);
    }

    اكسر;

  حالة 0x74:
    بدل(CSI_ARG(دلیلاں[0])) {
      حالة 8: // CSI 8 ; rows ; cols t  set size
        إذا (argcount == 3 &&
            !CSI_ARG_IS_MISSING(دلیلاں[1]) && !CSI_ARG_IS_MISSING(دلیلاں[2]) &&
            CSI_ARG(دلیلاں[1]) > 0 && CSI_ARG(دلیلاں[2]) > 0)
	  on_resize(CSI_ARG(دلیلاں[1]), CSI_ARG(دلیلاں[2]), state);
	اكسر;
      افتراضي:
	handled = 0;
	اكسر;
    }
    اكسر;

  حالة INTERMED('\'', 0x7D): // DECIC
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    إذا(!is_cursor_وچ_scrollregion(state))
      اكسر;

    rect.شروع_صف = state->scrollregion_چوٹی;
    rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_خانہ = state->pos.خانہ;
    rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

    scroll(state, rect, 0, -گنتی);

    اكسر;

  حالة INTERMED('\'', 0x7E): // DECDC
    گنتی = CSI_ARG_گنتی(دلیلاں[0]);

    إذا(!is_cursor_وچ_scrollregion(state))
      اكسر;

    rect.شروع_صف = state->scrollregion_چوٹی;
    rect.سرا_صف   = SCROLLREGION_BOTTOM(state);
    rect.شروع_خانہ = state->pos.خانہ;
    rect.سرا_خانہ   = SCROLLREGION_دایاں(state);

    scroll(state, rect, 0, گنتی);

    اكسر;

  افتراضي:
    handled = 0;
    اكسر;
  }

  إذا (!handled) {
    إذا(state->fallbacks && state->fallbacks->csi)
      إذا((*state->fallbacks->csi)(leader, دلیلاں, argcount, intermed, command, state->fbdata))
        أرجع 1;

    أرجع 0;
  }

  إذا(state->منوال.origin) {
    LBOUND(state->pos.صف, state->scrollregion_چوٹی);
    UBOUND(state->pos.صف, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.خانہ, SCROLLREGION_بایاں(state));
    UBOUND(state->pos.خانہ, SCROLLREGION_دایاں(state)-1);
  }
  وإلا {
    LBOUND(state->pos.صف, 0);
    UBOUND(state->pos.صف, state->صفاں_2-1);
    LBOUND(state->pos.خانہ, 0);
    UBOUND(state->pos.خانہ, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#جے_تعریف_شدہ DEBUG
  إذا(state->pos.صف < 0 || state->pos.صف >= state->صفاں_2 ||
     state->pos.خانہ < 0 || state->pos.خانہ >= state->خانے_گنتی) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.صف, state->pos.خانہ);
    abort();
  }

  إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_چوٹی) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_چوٹی);
    abort();
  }

  إذا(SCROLLREGION_دایاں(state) <= SCROLLREGION_بایاں(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_دایاں(state), SCROLLREGION_بایاں(state));
    abort();
  }
#جے_انت

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
    state->tmp.selection.نقاب = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  طالما(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    بدل(frag.str[0]) {
      حالة 'c':
        state->tmp.selection.نقاب |= VTERM_SELECTION_CLIPBOARD;
        اكسر;
      حالة 'p':
        state->tmp.selection.نقاب |= VTERM_SELECTION_PRIMARY;
        اكسر;
      حالة 'q':
        state->tmp.selection.نقاب |= VTERM_SELECTION_SECONDARY;
        اكسر;
      حالة 's':
        state->tmp.selection.نقاب |= VTERM_SELECTION_SELECT;
        اكسر;
      حالة '0':
      حالة '1':
      حالة '2':
      حالة '3':
      حالة '4':
      حالة '5':
      حالة '6':
      حالة '7':
        state->tmp.selection.نقاب |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        اكسر;

      حالة ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        إذا(!state->tmp.selection.نقاب)
          state->tmp.selection.نقاب = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        اكسر;
    }

    frag.str++;
    frag.len--;
  }

  إذا(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    إذا(frag.final && state->selection.callbacks->سیٹ_2) {
      (*state->selection.callbacks->سیٹ_2)(state->tmp.selection.نقاب, (VTermStringFragment){
              .str     = عدم,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_سیٹ,
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
      state->tmp.selection.state = SELECTION_سیٹ_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  إذا(state->tmp.selection.state == SELECTION_QUERY) {
    إذا(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.نقاب, state->selection.user);
    أرجع;
  }

  إذا(state->tmp.selection.state == SELECTION_INVALID)
    أرجع;

  إذا(state->selection.callbacks->سیٹ_2) {
    size_t bufcur = 0;
    حرف *بفر = state->selection.بفر;

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
          بفر[0] = (x >> 4) & 0xFF;
          بفر += 1, bufcur += 1;
        }
        إذا(n == 3) {
          بفر[0] = (x >> 10) & 0xFF;
          بفر[1] = (x >>  2) & 0xFF;
          بفر += 2, bufcur += 2;
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
          إذا(state->selection.callbacks->سیٹ_2) {
            (*state->selection.callbacks->سیٹ_2)(state->tmp.selection.نقاب, (VTermStringFragment){
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
          بفر[0] = (x >> 16) & 0xFF;
          بفر[1] = (x >>  8) & 0xFF;
          بفر[2] = (x >>  0) & 0xFF;

          بفر += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      إذا(!frag.len || (state->selection.buflen - bufcur) < 3) {
        إذا(bufcur) {
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
        vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        أرجع 1;
      }

    حالة 11:
      {
	// request background color: <Esc>]11;?<0x07>
	صحيح red = state->default_bg.red;
	صحيح blue = state->default_bg.blue;
	صحيح green = state->default_bg.green;
	vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
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

ساكن فراغ request_حالت_string(VTermState *state, VTermStringFragment frag)
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
      لما دلیلاں[20];
      صحيح argc = vterm_state_getpen(state, دلیلاں, حجم(دلیلاں)/حجم(دلیلاں[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->منوال.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      إذا(cur >= vt->tmpbuffer_len)
        أرجع;

      لكل(صحيح argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_ہور(دلیلاں[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(دلیلاں[argi]));
        إذا(cur >= vt->tmpbuffer_len)
          أرجع;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->منوال.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      إذا(cur >= vt->tmpbuffer_len)
        أرجع;

      vterm_ڈالو_output_بائٹاں(vt, vt->tmpbuffer, cur);
      أرجع;
    }

    حالة 'r':
      // Query DECSTBM
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_چوٹی+1, SCROLLREGION_BOTTOM(state));
      أرجع;

    حالة 's':
      // Query DECSLRM
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_بایاں(state)+1, SCROLLREGION_دایاں(state));
      أرجع;

    حالة ' '|('q'<<8): {
      // Query DECSCUSR
      صحيح جواب;
      بدل(state->منوال.cursor_shape) {
        حالة VTERM_PROP_CURSORSHAPE_بلاک:     جواب = 2; اكسر;
        حالة VTERM_PROP_CURSORSHAPE_UNDERLINE: جواب = 4; اكسر;
	افتراضي: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ جواب = 6; اكسر;
      }
      إذا(state->منوال.cursor_blink)
        جواب--;
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", جواب);
      أرجع;
    }

    حالة '\"'|('q'<<8):
      // Query DECSCA
      vterm_ڈالو_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      أرجع;
  }

  vterm_ڈالو_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ساكن صحيح on_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(commandlen == 2 && strneq(command, "$q", 2)) {
    request_حالت_string(state, frag);
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

ساكن صحيح on_resize(صحيح صفاں_2, صحيح خانے_گنتی, فراغ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  إذا (خانے_گنتی > VTERM_MAX_خانے_گنتی)
    خانے_گنتی = VTERM_MAX_خانے_گنتی;

  إذا (صفاں_2 > VTERM_MAX_صفاں)
    صفاں_2 = VTERM_MAX_صفاں;

  إذا(خانے_گنتی != state->خانے_گنتی) {
    بنانشانی حرف *newtabstops = vterm_allocator_malloc(state->vt, (خانے_گنتی + 7) / 8);
    إذا (newtabstops == عدم)
      أرجع 0;

    /* TODO: This can all be done much more efficiently bytewise */
    صحيح خانہ;
    لكل(خانہ = 0; خانہ < state->خانے_گنتی && خانہ < خانے_گنتی; خانہ++) {
      بنانشانی حرف نقاب = 1 << (خانہ & 7);
      إذا(state->tabstops[خانہ >> 3] & نقاب)
        newtabstops[خانہ >> 3] |= نقاب;
      وإلا
        newtabstops[خانہ >> 3] &= ~نقاب;
      }

    لكل( ; خانہ < خانے_گنتی; خانہ++) {
      بنانشانی حرف نقاب = 1 << (خانہ & 7);
      إذا(خانہ % 8 == 0)
        newtabstops[خانہ >> 3] |= نقاب;
      وإلا
        newtabstops[خانہ >> 3] &= ~نقاب;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->صفاں_2 = صفاں_2;
  state->خانے_گنتی = خانے_گنتی;

  إذا(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->صفاں_2);
  إذا(state->scrollregion_دایاں > -1)
    UBOUND(state->scrollregion_دایاں, state->خانے_گنتی);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_چوٹی, state->صفاں_2);
  UBOUND(state->scrollregion_بایاں, state->خانے_گنتی);
  إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_چوٹی) {
    state->scrollregion_چوٹی = 0;
    state->scrollregion_bottom = -1;
  }
  إذا(SCROLLREGION_دایاں(state) <= state->scrollregion_بایاں) {
    state->scrollregion_بایاں = 0;
    state->scrollregion_دایاں = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  إذا(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(صفاں_2, خانے_گنتی, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  وإلا {
    إذا(صفاں_2 != state->صفاں_2) {
      لكل(صحيح bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_سطر_Info *oldlineinfo = state->lineinfos[bufidx];
        إذا(!oldlineinfo)
          استمر;

        VTerm_سطر_Info *newlineinfo = vterm_allocator_malloc(state->vt, صفاں_2 * حجم(VTerm_سطر_Info));

        صحيح صف;
        لكل(صف = 0; صف < state->صفاں_2 && صف < صفاں_2; صف++) {
          newlineinfo[صف] = oldlineinfo[صف];
        }

        لكل( ; صف < صفاں_2; صف++) {
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

  إذا(state->at_phantom && state->pos.خانہ < خانے_گنتی-1) {
    state->at_phantom = 0;
    state->pos.خانہ++;
  }

  إذا(state->pos.صف < 0)
    state->pos.صف = 0;
  إذا(state->pos.صف >= صفاں_2)
    state->pos.صف = صفاں_2 - 1;
  إذا(state->pos.خانہ < 0)
    state->pos.خانہ = 0;
  إذا(state->pos.خانہ >= خانے_گنتی)
    state->pos.خانہ = خانے_گنتی - 1;

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

  vterm_parser_سیٹ_callbacks(vt, &parser_callbacks, state);

  أرجع state;
}

فراغ vterm_state_reset(VTermState *state, صحيح hard)
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

  لكل(صحيح خانہ = 0; خانہ < state->خانے_گنتی; خانہ++)
    إذا(خانہ % 8 == 0)
      سیٹ_خانہ_tabstop(state, خانہ);
    وإلا
      clear_خانہ_tabstop(state, خانہ);

  لكل(صحيح صف = 0; صف < state->صفاں_2; صف++)
    سیٹ_lineinfo(state, صف, FORCE, DWL_OFF, DHL_OFF);

  إذا(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->منوال.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  لكل(صحيح i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    إذا(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].انکڑے);
  }

  state->gl_سیٹ = 0;
  state->gr_سیٹ = 1;
  state->gsingle_سیٹ = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_بلاک);

  إذا(hard) {
    state->pos.صف = 0;
    state->pos.خانہ = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.سرا_صف = state->صفاں_2;
    rect.سرا_خانہ =  state->خانے_گنتی;
    erase(state, rect, 0);
  }
}

فراغ vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

فراغ vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.خانہ = state->mouse_خانہ;
  mousestate->pos.صف = state->mouse_صف;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

فراغ vterm_state_سیٹ_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, فراغ *user)
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

فراغ vterm_state_سیٹ_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
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

صحيح vterm_state_سیٹ_termprop(VTermState *state, VTermProp prop, VTerm_قدر *val)
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
    state->منوال.cursor_visible = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_CURSORBLINK:
    state->منوال.cursor_blink = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_CURSORSHAPE:
    state->منوال.cursor_shape = val->نمبر;
    أرجع 1;
  حالة VTERM_PROP_الٹ:
    state->منوال.screen = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_ALTSCREEN:
    state->منوال.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->منوال.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    إذا(state->منوال.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.سرا_صف = state->صفاں_2;
      rect.سرا_خانہ = state->خانے_گنتی;
      erase(state, rect, 0);
    }
    أرجع 1;
  حالة VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    إذا(val->نمبر)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    إذا(val->نمبر == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    إذا(val->نمبر == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    أرجع 1;
  حالة VTERM_PROP_FOCUSREPORT:
    state->منوال.report_focus = val->boolean;
    أرجع 1;

  حالة VTERM_N_PROPS:
    أرجع 0;
  }

  أرجع 0;
}

فراغ vterm_state_focus_وچ(VTermState *state)
{
  إذا(state->منوال.report_focus)
    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

فراغ vterm_state_focus_باہرلا(VTermState *state)
{
  إذا(state->منوال.report_focus)
    vterm_ڈالو_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ثابت VTerm_سطر_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحيح صف)
{
  أرجع state->lineinfo + صف;
}

فراغ vterm_state_سیٹ_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, فراغ *user,
    حرف *بفر, size_t buflen)
{
  إذا(buflen && !بفر)
    بفر = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.بفر    = بفر;
  state->selection.buflen    = buflen;
}

فراغ vterm_state_send_selection(VTermState *state, VTerm_Selection_نقاب نقاب, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  إذا(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ساكن حرف selection_chars[] = "cpqs";
    صحيح idx;
    لكل(idx = 0; idx < 4; idx++)
      إذا(نقاب & (1 << idx))
        اكسر;

    vterm_ڈالو_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  إذا(frag.len) {
    size_t bufcur = 0;
    حرف *بفر = state->selection.بفر;

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
        بفر[0] = base64_one((x >> 18) & 0x3F);
        بفر[1] = base64_one((x >> 12) & 0x3F);
        بفر[2] = base64_one((x >>  6) & 0x3F);
        بفر[3] = base64_one((x >>  0) & 0x3F);

        بفر += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      إذا(!frag.len || (state->selection.buflen - bufcur) < 4) {
        إذا(bufcur)
          vterm_ڈالو_output_بائٹاں(vt, state->selection.بفر, bufcur);

        بفر = state->selection.بفر;
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
      حرف *بفر = state->selection.بفر;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      بفر[0] = base64_one((x >> 18) & 0x3F);
      بفر[1] = base64_one((x >> 12) & 0x3F);
      بفر[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      بفر[3] = '=';

      vterm_ڈالو_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, بفر);
    }
    وإلا
      vterm_ڈالو_output_sprintf_str(vt, 0, TRUE, "");
  }
}
