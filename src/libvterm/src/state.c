#అన్తర్భావ "vterm_internal.h"

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>

#నిర్వచించు strneq(a,b,n) (అక్షరశ్రేణి_పోలిక(a,b,n)==0)

#ఒకవేళ defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#అయితే_ముగింపు

స్థిర పూర్ణాంకం on_resize(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, శూన్య *user);

/* Some convenient wrappers to make callback functions easier */

స్థిర శూన్య putglyph(VTermState *state, స్థిరరాశి uint32_t chars[], పూర్ణాంకం వెడల్పు, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.వెడల్పు = వెడల్పు;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.అడ్డువరుస].doublewidth;
  info.dhl = state->lineinfo[pos.అడ్డువరుస].doubleheight;

  ఒకవేళ(state->callbacks && state->callbacks->putglyph)
    ఒకవేళ((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      తిరిగి;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.నిలువు, pos.అడ్డువరుస);
}

స్థిర శూన్య updatecursor(VTermState *state, VTermPos *oldpos, పూర్ణాంకం cancel_phantom)
{
  ఒకవేళ(state->pos.నిలువు == oldpos->నిలువు && state->pos.అడ్డువరుస == oldpos->అడ్డువరుస)
    తిరిగి;

  ఒకవేళ(cancel_phantom)
    state->at_phantom = 0;

  ఒకవేళ(state->callbacks && state->callbacks->movecursor)
    ఒకవేళ((*state->callbacks->movecursor)(state->pos, *oldpos, state->బహుళకం.cursor_visible, state->cbdata))
      తిరిగి;
}

స్థిర శూన్య erase(VTermState *state, VTermRect rect, పూర్ణాంకం selective)
{
  ఒకవేళ(rect.చివర_నిలువు == state->నిలువుల_సంఖ్య) {
    పూర్ణాంకం అడ్డువరుస;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    కొరకు(అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస + 1; అడ్డువరుస < rect.చివర_అడ్డువరుస + 1 && అడ్డువరుస < state->అడ్డువరుసలు_2; అడ్డువరుస++)
      state->lineinfo[అడ్డువరుస].continuation = 0;
  }

  ఒకవేళ(state->callbacks && state->callbacks->erase)
    ఒకవేళ((*state->callbacks->erase)(rect, selective, state->cbdata))
      తిరిగి;
}

స్థిర VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, పరిమాణం(VTermState));

  ఒకవేళ (state == ఖాళీ)
    తిరిగి ఖాళీ;
  state->vt = vt;

  state->అడ్డువరుసలు_2 = vt->అడ్డువరుసలు_2;
  state->నిలువుల_సంఖ్య = vt->నిలువుల_సంఖ్య;

  state->mouse_నిలువు     = 0;
  state->mouse_అడ్డువరుస     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ఖాళీ;
  state->cbdata    = ఖాళీ;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ఖాళీ;
  state->selection.user      = ఖాళీ;
  state->selection.బఫరు    = ఖాళీ;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_కొలత = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_కొలత * పరిమాణం(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->నిలువుల_సంఖ్య + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->అడ్డువరుసలు_2 * పరిమాణం(VTerm_పంక్తి_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->అడ్డువరుసలు_2 * పరిమాణం(VTerm_పంక్తి_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  ఒకవేళ(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.దత్తాంశం);

  తిరిగి state;
}

అంతర్గత శూన్య vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  ఒకవేళ(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

స్థిర శూన్య scroll(VTermState *state, VTermRect rect, పూర్ణాంకం downward, పూర్ణాంకం rightward)
{
  పూర్ణాంకం అడ్డువరుసలు_2;
  పూర్ణాంకం నిలువుల_సంఖ్య;
  ఒకవేళ(!downward && !rightward)
    తిరిగి;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  ఒకవేళ(rect.చివర_అడ్డువరుస <= rect.ప్రారంభం_అడ్డువరుస || rect.చివర_నిలువు <= rect.ప్రారంభం_నిలువు)
    తిరిగి;

  అడ్డువరుసలు_2 = rect.చివర_అడ్డువరుస - rect.ప్రారంభం_అడ్డువరుస;
  ఒకవేళ(downward > అడ్డువరుసలు_2)
    downward = అడ్డువరుసలు_2;
  లేకపోతే ఒకవేళ(downward < -అడ్డువరుసలు_2)
    downward = -అడ్డువరుసలు_2;

  నిలువుల_సంఖ్య = rect.చివర_నిలువు - rect.ప్రారంభం_నిలువు;
  ఒకవేళ(rightward > నిలువుల_సంఖ్య)
    rightward = నిలువుల_సంఖ్య;
  లేకపోతే ఒకవేళ(rightward < -నిలువుల_సంఖ్య)
    rightward = -నిలువుల_సంఖ్య;

  ఒకవేళ(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    ఒకవేళ(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .ప్రారంభం_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస, .చివర_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస + downward,
          .ప్రారంభం_నిలువు = rect.ప్రారంభం_నిలువు, .చివర_నిలువు = rect.చివర_నిలువు}, state->cbdata);
    లేకపోతే ఒకవేళ(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .ప్రారంభం_అడ్డువరుస = rect.చివర_అడ్డువరుస + downward, .చివర_అడ్డువరుస = rect.చివర_అడ్డువరుస,
          .ప్రారంభం_నిలువు = rect.ప్రారంభం_నిలువు,          .చివర_నిలువు = rect.చివర_నిలువు}, state->cbdata);

    ఒకవేళ(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .ప్రారంభం_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస, .చివర_అడ్డువరుస = rect.చివర_అడ్డువరుస,
          .ప్రారంభం_నిలువు = rect.ప్రారంభం_నిలువు, .చివర_నిలువు = rect.ప్రారంభం_నిలువు + rightward}, state->cbdata);
    లేకపోతే ఒకవేళ(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .ప్రారంభం_అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస,           .చివర_అడ్డువరుస = rect.చివర_అడ్డువరుస,
          .ప్రారంభం_నిలువు = rect.చివర_నిలువు + rightward, .చివర_నిలువు = rect.చివర_నిలువు}, state->cbdata);
  }

  // Update lineinfo if full line
  ఒకవేళ(rect.ప్రారంభం_నిలువు == 0 && rect.చివర_నిలువు == state->నిలువుల_సంఖ్య && rightward == 0) {
    పూర్ణాంకం ఎత్తు = rect.చివర_అడ్డువరుస - rect.ప్రారంభం_అడ్డువరుస - నిరపేక్ష(downward);
    పూర్ణాంకం అడ్డువరుస;
    VTerm_పంక్తి_Info zero_పంక్తి_Info = {0x0};

    ఒకవేళ(downward > 0) {
      memmove(state->lineinfo + rect.ప్రారంభం_అడ్డువరుస,
              state->lineinfo + rect.ప్రారంభం_అడ్డువరుస + downward,
              ఎత్తు * పరిమాణం(state->lineinfo[0]));
      కొరకు(అడ్డువరుస = rect.చివర_అడ్డువరుస - downward; అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++)
        state->lineinfo[అడ్డువరుస] = zero_పంక్తి_Info;
    }
    లేకపోతే {
      memmove(state->lineinfo + rect.ప్రారంభం_అడ్డువరుస - downward,
              state->lineinfo + rect.ప్రారంభం_అడ్డువరుస,
              ఎత్తు * పరిమాణం(state->lineinfo[0]));
      కొరకు(అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస; అడ్డువరుస < rect.ప్రారంభం_అడ్డువరుస - downward; అడ్డువరుస++)
        state->lineinfo[అడ్డువరుస] = zero_పంక్తి_Info;
    }
  }

  ఒకవేళ(state->callbacks && state->callbacks->scrollrect)
    ఒకవేళ((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      తిరిగి;

  ఒకవేళ(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

స్థిర శూన్య linefeed(VTermState *state)
{
  ఒకవేళ(state->pos.అడ్డువరుస == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.ప్రారంభం_అడ్డువరుస = state->scrollregion_శిఖరం;
    rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
    rect.ప్రారంభం_నిలువు = SCROLLREGION_ఎడమ(state);
    rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

    scroll(state, rect, 1, 0);
  }
  లేకపోతే ఒకవేళ(state->pos.అడ్డువరుస < state->అడ్డువరుసలు_2-1)
    state->pos.అడ్డువరుస++;
}

స్థిర శూన్య grow_combine_బఫరు(VTermState *state)
{
  size_t    new_కొలత = state->combine_chars_కొలత * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_కొలత * పరిమాణం(new_chars[0]));

  జ్ఞాపకం_నకలు(new_chars, state->combine_chars, state->combine_chars_కొలత * పరిమాణం(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_కొలత = new_కొలత;
}

స్థిర శూన్య సమితి_నిలువు_tabstop(VTermState *state, పూర్ణాంకం నిలువు)
{
  గుర్తులేని అక్షరం ముసుగు = 1 << (నిలువు & 7);
  state->tabstops[నిలువు >> 3] |= ముసుగు;
}

స్థిర శూన్య clear_నిలువు_tabstop(VTermState *state, పూర్ణాంకం నిలువు)
{
  గుర్తులేని అక్షరం ముసుగు = 1 << (నిలువు & 7);
  state->tabstops[నిలువు >> 3] &= ~ముసుగు;
}

స్థిర పూర్ణాంకం is_నిలువు_tabstop(VTermState *state, పూర్ణాంకం నిలువు)
{
  గుర్తులేని అక్షరం ముసుగు = 1 << (నిలువు & 7);
  తిరిగి state->tabstops[నిలువు >> 3] & ముసుగు;
}

స్థిర పూర్ణాంకం is_cursor_లోపల_scrollregion(స్థిరరాశి VTermState *state)
{
  ఒకవేళ(state->pos.అడ్డువరుస < state->scrollregion_శిఖరం ||
     state->pos.అడ్డువరుస >= SCROLLREGION_BOTTOM(state))
    తిరిగి 0;
  ఒకవేళ(state->pos.నిలువు < SCROLLREGION_ఎడమ(state) ||
     state->pos.నిలువు >= SCROLLREGION_కుడి(state))
    తిరిగి 0;

  తిరిగి 1;
}

స్థిర శూన్య tab(VTermState *state, పూర్ణాంకం లెక్క, పూర్ణాంకం direction)
{
  అయితే(లెక్క > 0) {
    ఒకవేళ(direction > 0) {
      ఒకవేళ(state->pos.నిలువు >= THISROWWIDTH(state)-1)
        తిరిగి;

      state->pos.నిలువు++;
    }
    లేకపోతే ఒకవేళ(direction < 0) {
      ఒకవేళ(state->pos.నిలువు < 1)
        తిరిగి;

      state->pos.నిలువు--;
    }

    ఒకవేళ(is_నిలువు_tabstop(state, state->pos.నిలువు))
      లెక్క--;
  }
}

#నిర్వచించు NO_FORCE 0
#నిర్వచించు FORCE    1

#నిర్వచించు DWL_OFF 0
#నిర్వచించు DWL_ON  1

#నిర్వచించు DHL_OFF    0
#నిర్వచించు DHL_శిఖరం    1
#నిర్వచించు DHL_BOTTOM 2

స్థిర శూన్య సమితి_lineinfo(VTermState *state, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం force, పూర్ణాంకం dwl, పూర్ణాంకం dhl)
{
  VTerm_పంక్తి_Info info = state->lineinfo[అడ్డువరుస];

  ఒకవేళ(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  లేకపోతే ఒకవేళ(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  ఒకవేళ(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  లేకపోతే ఒకవేళ(dhl == DHL_శిఖరం)
    info.doubleheight = DHL_శిఖరం;
  లేకపోతే ఒకవేళ(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  ఒకవేళ((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(అడ్డువరుస, &info, state->lineinfo + అడ్డువరుస, state->cbdata))
      || force)
    state->lineinfo[అడ్డువరుస] = info;
}

స్థిర పూర్ణాంకం on_పాఠ్యం(స్థిరరాశి అక్షరం బైట్లు[], size_t len, శూన్య *user)
{
  VTermState *state = user;
  పూర్ణాంకం npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  పూర్ణాంకం i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / పరిమాణం(uint32_t);

  encoding =
    state->gsingle_సమితి     ? &state->encoding[state->gsingle_సమితి] :
    !(బైట్లు[eaten] & 0x80) ? &state->encoding[state->gl_సమితి] :
    state->vt->బహుళకం.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_సమితి];
  ఒకవేళ (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->దత్తాంశం,
      codepoints, &npoints, state->gsingle_సమితి ? 1 : (పూర్ణాంకం)maxpoints,
      బైట్లు, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  ఒకవేళ(!npoints)
  {
    తిరిగి (పూర్ణాంకం)eaten;
  }

  ఒకవేళ(state->gsingle_సమితి && npoints)
    state->gsingle_సమితి = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  ఒకవేళ(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    ఒకవేళ(state->pos.అడ్డువరుస == state->combine_pos.అడ్డువరుస && state->pos.నిలువు == state->combine_pos.నిలువు + state->combine_వెడల్పు) {
#నిర్వచితమైతే DEBUG_GLYPH_COMBINE
      పూర్ణాంకం printpos;
      రాయి("DEBUG: COMBINING SPLIT GLYPH of chars {");
      కొరకు(printpos = 0; state->combine_chars[printpos]; printpos++)
        రాయి("U+%04x ", state->combine_chars[printpos]);
      రాయి("} + {");
#అయితే_ముగింపు

      /* Find where we need to append these combining chars */
      పూర్ణాంకం saved_i = 0;
      అయితే(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      అయితే(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        ఒకవేళ(saved_i >= (పూర్ణాంకం)state->combine_chars_కొలత)
          grow_combine_బఫరు(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      ఒకవేళ(saved_i >= (పూర్ణాంకం)state->combine_chars_కొలత)
        grow_combine_బఫరు(state);
      state->combine_chars[saved_i] = 0;

#నిర్వచితమైతే DEBUG_GLYPH_COMBINE
      కొరకు(; state->combine_chars[printpos]; printpos++)
        రాయి("U+%04x ", state->combine_chars[printpos]);
      రాయి("}\n");
#అయితే_ముగింపు

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_వెడల్పు, state->combine_pos);
    }
    లేకపోతే {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  కొరకు(; i < npoints; i++) {
    // Try to find combining characters following this
    పూర్ణాంకం glyph_starts = i;
    పూర్ణాంకం glyph_చివర్లు;
    పూర్ణాంకం వెడల్పు = 0;

    కొరకు(glyph_చివర్లు = i + 1;
        (glyph_చివర్లు < npoints) && (glyph_చివర్లు < glyph_starts + VTERM_MAX_CHARS_చొప్పున_CELL);
        glyph_చివర్లు++)
      ఒకవేళ(!vterm_unicode_is_combining(codepoints[glyph_చివర్లు]))
        ఆపు;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_చొప్పున_CELL + 1) * పరిమాణం(uint32_t));
    ఒకవేళ (chars == ఖాళీ)
      ఆపు;

    కొరకు( ; i < glyph_చివర్లు; i++) {
      పూర్ణాంకం this_వెడల్పు;
      ఒకవేళ(vterm_get_special_pty_type() == 2) {
        state->vt->లోపల_backspace -= (state->vt->లోపల_backspace > 0) ? 1 : 0;
        ఒకవేళ(state->vt->లోపల_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_వెడల్పు = vterm_unicode_వెడల్పు(codepoints[i]);
#నిర్వచితమైతే DEBUG
      ఒకవేళ(this_వెడల్పు < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#అయితే_ముగింపు
      ఒకవేళ (i == glyph_starts || this_వెడల్పు > వెడల్పు)
	వెడల్పు = this_వెడల్పు;  // TODO: should be += ?
    }

    ఒకవేళ (వెడల్పు < 0)
      వెడల్పు = 0;

    అయితే(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_చివర్లు - glyph_starts] = 0;
    i--;

#నిర్వచితమైతే DEBUG_GLYPH_COMBINE
    పూర్ణాంకం printpos;
    రాయి("DEBUG: COMBINED GLYPH of %d chars {", glyph_చివర్లు - glyph_starts);
    కొరకు(printpos = 0; printpos < glyph_చివర్లు - glyph_starts; printpos++)
      రాయి("U+%04x ", chars[printpos]);
    రాయి("}, onscreen width %d\n", వెడల్పు);
#అయితే_ముగింపు

    ఒకవేళ(state->at_phantom || state->pos.నిలువు + వెడల్పు > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.నిలువు = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.అడ్డువరుస].continuation = 1;
    }

    ఒకవేళ(state->బహుళకం.చొప్పించు) {
      // TODO: This will be a little inefficient for large bodies of text, as
      // it'll have to 'ICH' effectively before every glyph. We should scan
      // ahead and ICH as many times as required
      VTermRect rect;
      rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస;
      rect.చివర_అడ్డువరుస   = state->pos.అడ్డువరుస + 1;
      rect.ప్రారంభం_నిలువు = state->pos.నిలువు;
      rect.చివర_నిలువు   = THISROWWIDTH(state);
      scroll(state, rect, 0, -1);
    }

    putglyph(state, chars, వెడల్పు, state->pos);

    ఒకవేళ(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      పూర్ణాంకం save_i;
      కొరకు(save_i = 0; chars[save_i]; save_i++) {
        ఒకవేళ(save_i >= (పూర్ణాంకం)state->combine_chars_కొలత)
          grow_combine_బఫరు(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      ఒకవేళ(save_i >= (పూర్ణాంకం)state->combine_chars_కొలత)
        grow_combine_బఫరు(state);
      state->combine_chars[save_i] = 0;
      state->combine_వెడల్పు = వెడల్పు;
      state->combine_pos = state->pos;
    }

    ఒకవేళ(state->pos.నిలువు + వెడల్పు >= THISROWWIDTH(state)) {
      ఒకవేళ(state->బహుళకం.autowrap)
        state->at_phantom = 1;
    }
    లేకపోతే {
      state->pos.నిలువు += వెడల్పు;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#నిర్వచితమైతే DEBUG
  ఒకవేళ(state->pos.అడ్డువరుస < 0 || state->pos.అడ్డువరుస >= state->అడ్డువరుసలు_2 ||
     state->pos.నిలువు < 0 || state->pos.నిలువు >= state->నిలువుల_సంఖ్య) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.అడ్డువరుస, state->pos.నిలువు);
    abort();
  }
#అయితే_ముగింపు

  తిరిగి (పూర్ణాంకం)eaten;
}

స్థిర పూర్ణాంకం on_control(గుర్తులేని అక్షరం control, శూన్య *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.నిలువు -= (leadpos.నిలువు >= 2 ? 2 : 0);

  ఎంపిక(control) {
  స్థితి 0x07: // BEL - ECMA-48 8.3.3
    ఒకవేళ(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    ఆపు;

  స్థితి 0x08: // BS - ECMA-48 8.3.5
    ఒకవేళ(state->pos.నిలువు > 0)
      state->pos.నిలువు--;
    ఒకవేళ(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      ఒకవేళ(vterm_unicode_వెడల్పు(cell.chars[0]) == 2)
        state->pos.నిలువు--;
    }
    ఆపు;

  స్థితి 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    ఆపు;

  స్థితి 0x0a: // LF - ECMA-48 8.3.74
  స్థితి 0x0b: // VT
  స్థితి 0x0c: // FF
    linefeed(state);
    ఒకవేళ(state->బహుళకం.newline)
      state->pos.నిలువు = 0;
    ఆపు;

  స్థితి 0x0d: // CR - ECMA-48 8.3.15
    state->pos.నిలువు = 0;
    ఆపు;

  స్థితి 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_సమితి = 1;
    ఆపు;

  స్థితి 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_సమితి = 0;
    ఆపు;

  స్థితి 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    ఆపు;

  స్థితి 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.నిలువు = 0;
    ఆపు;

  స్థితి 0x88: // HTS - ECMA-48 8.3.62
    సమితి_నిలువు_tabstop(state, state->pos.నిలువు);
    ఆపు;

  స్థితి 0x8d: // RI - ECMA-48 8.3.104
    ఒకవేళ(state->pos.అడ్డువరుస == state->scrollregion_శిఖరం) {
      VTermRect rect;
      rect.ప్రారంభం_అడ్డువరుస = state->scrollregion_శిఖరం;
      rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
      rect.ప్రారంభం_నిలువు = SCROLLREGION_ఎడమ(state);
      rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

      scroll(state, rect, -1, 0);
    }
    లేకపోతే ఒకవేళ(state->pos.అడ్డువరుస > 0)
        state->pos.అడ్డువరుస--;
    ఆపు;

  స్థితి 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_సమితి = 2;
    ఆపు;

  స్థితి 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_సమితి = 3;
    ఆపు;

  అప్రమేయ:
    ఒకవేళ(state->fallbacks && state->fallbacks->control)
      ఒకవేళ((*state->fallbacks->control)(control, state->fbdata))
        తిరిగి 1;

    తిరిగి 0;
  }

  updatecursor(state, &oldpos, 1);

#నిర్వచితమైతే DEBUG
  ఒకవేళ(state->pos.అడ్డువరుస < 0 || state->pos.అడ్డువరుస >= state->అడ్డువరుసలు_2 ||
     state->pos.నిలువు < 0 || state->pos.నిలువు >= state->నిలువుల_సంఖ్య) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.అడ్డువరుస, state->pos.నిలువు);
    abort();
  }
#అయితే_ముగింపు

  తిరిగి 1;
}

స్థిర పూర్ణాంకం settermprop_bool(VTermState *state, VTermProp prop, పూర్ణాంకం v)
{
  VTerm_విలువ val;
  val.boolean = v;
  తిరిగి vterm_state_సమితి_termprop(state, prop, &val);
}

స్థిర పూర్ణాంకం settermprop_int(VTermState *state, VTermProp prop, పూర్ణాంకం v)
{
  VTerm_విలువ val;
  val.సంఖ్య = v;
  తిరిగి vterm_state_సమితి_termprop(state, prop, &val);
}

స్థిర పూర్ణాంకం settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_విలువ val;

  val.string = frag;
  తిరిగి vterm_state_సమితి_termprop(state, prop, &val);
}

స్థిర శూన్య savecursor(VTermState *state, పూర్ణాంకం save)
{
  ఒకవేళ(save) {
    state->saved.pos = state->pos;
    state->saved.బహుళకం.cursor_visible = state->బహుళకం.cursor_visible;
    state->saved.బహుళకం.cursor_blink   = state->బహుళకం.cursor_blink;
    state->saved.బహుళకం.cursor_shape   = state->బహుళకం.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  లేకపోతే {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.బహుళకం.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.బహుళకం.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.బహుళకం.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

స్థిర పూర్ణాంకం on_escape(స్థిరరాశి అక్షరం *బైట్లు, size_t len, శూన్య *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  ఎంపిక(బైట్లు[0]) {
  స్థితి ' ':
    ఒకవేళ(len != 2)
      తిరిగి 0;

    ఎంపిక(బైట్లు[1]) {
      స్థితి 'F': // S7C1T
        state->vt->బహుళకం.ctrl8bit = 0;
        ఆపు;

      స్థితి 'G': // S8C1T
        state->vt->బహుళకం.ctrl8bit = 1;
        ఆపు;

      అప్రమేయ:
        తిరిగి 0;
    }
    తిరిగి 2;

  స్థితి '#':
    ఒకవేళ(len != 2)
      తిరిగి 0;

    ఎంపిక(బైట్లు[1]) {
      స్థితి '3': // DECDHL top
        ఒకవేళ(state->బహుళకం.leftrightmargin)
          ఆపు;
        సమితి_lineinfo(state, state->pos.అడ్డువరుస, NO_FORCE, DWL_ON, DHL_శిఖరం);
        ఆపు;

      స్థితి '4': // DECDHL bottom
        ఒకవేళ(state->బహుళకం.leftrightmargin)
          ఆపు;
        సమితి_lineinfo(state, state->pos.అడ్డువరుస, NO_FORCE, DWL_ON, DHL_BOTTOM);
        ఆపు;

      స్థితి '5': // DECSWL
        ఒకవేళ(state->బహుళకం.leftrightmargin)
          ఆపు;
        సమితి_lineinfo(state, state->pos.అడ్డువరుస, NO_FORCE, DWL_OFF, DHL_OFF);
        ఆపు;

      స్థితి '6': // DECDWL
        ఒకవేళ(state->బహుళకం.leftrightmargin)
          ఆపు;
        సమితి_lineinfo(state, state->pos.అడ్డువరుస, NO_FORCE, DWL_ON, DHL_OFF);
        ఆపు;

      స్థితి '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        కొరకు(pos.అడ్డువరుస = 0; pos.అడ్డువరుస < state->అడ్డువరుసలు_2; pos.అడ్డువరుస++)
          కొరకు(pos.నిలువు = 0; pos.నిలువు < ROWWIDTH(state, pos.అడ్డువరుస); pos.నిలువు++)
            putglyph(state, E, 1, pos);
        ఆపు;
      }

      అప్రమేయ:
        తిరిగి 0;
    }
    తిరిగి 2;

  స్థితి '(': స్థితి ')': స్థితి '*': స్థితి '+': // SCS
    ఒకవేళ(len != 2)
      తిరిగి 0;

    {
      పూర్ణాంకం setnum = బైట్లు[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, బైట్లు[1]);

      ఒకవేళ(newenc) {
        state->encoding[setnum].enc = newenc;

        ఒకవేళ(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].దత్తాంశం);
      }
    }

    తిరిగి 2;

  స్థితి '7': // DECSC
    savecursor(state, 1);
    తిరిగి 1;

  స్థితి '8': // DECRC
    savecursor(state, 0);
    తిరిగి 1;

  స్థితి '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    తిరిగి 1;

  స్థితి '=': // DECKPAM
    state->బహుళకం.keypad = 1;
    తిరిగి 1;

  స్థితి '>': // DECKPNM
    state->బహుళకం.keypad = 0;
    తిరిగి 1;

  స్థితి 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    ఒకవేళ(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->బహుళకం.cursor_visible, state->cbdata);
    తిరిగి 1;
  }

  స్థితి 'n': // LS2 - ECMA-48 8.3.78
    state->gl_సమితి = 2;
    తిరిగి 1;

  స్థితి 'o': // LS3 - ECMA-48 8.3.80
    state->gl_సమితి = 3;
    తిరిగి 1;

  స్థితి '~': // LS1R - ECMA-48 8.3.77
    state->gr_సమితి = 1;
    తిరిగి 1;

  స్థితి '}': // LS2R - ECMA-48 8.3.79
    state->gr_సమితి = 2;
    తిరిగి 1;

  స్థితి '|': // LS3R - ECMA-48 8.3.81
    state->gr_సమితి = 3;
    తిరిగి 1;

  అప్రమేయ:
    తిరిగి 0;
  }
}

స్థిర శూన్య సమితి_బహుళకం(VTermState *state, పూర్ణాంకం అంకె, పూర్ణాంకం val)
{
  ఎంపిక(అంకె) {
  స్థితి 4: // IRM - ECMA-48 7.2.10
    state->బహుళకం.చొప్పించు = val;
    ఆపు;

  స్థితి 20: // LNM - ANSI X3.4-1977
    state->బహుళకం.newline = val;
    ఆపు;

  అప్రమేయ:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", అంకె);
    తిరిగి;
  }
}

స్థిర శూన్య సమితి_dec_బహుళకం(VTermState *state, పూర్ణాంకం అంకె, పూర్ణాంకం val)
{
  ఎంపిక(అంకె) {
  స్థితి 1:
    state->బహుళకం.cursor = val;
    ఆపు;

  స్థితి 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_తిరగేయి, val);
    ఆపు;

  స్థితి 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->బహుళకం.origin = val;
      state->pos.అడ్డువరుస = state->బహుళకం.origin ? state->scrollregion_శిఖరం : 0;
      state->pos.నిలువు = state->బహుళకం.origin ? SCROLLREGION_ఎడమ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    ఆపు;

  స్థితి 7:
    state->బహుళకం.autowrap = val;
    ఆపు;

  స్థితి 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    ఆపు;

  స్థితి 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    ఆపు;

  స్థితి 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->బహుళకం.leftrightmargin = val;
    ఒకవేళ(val) {
      పూర్ణాంకం అడ్డువరుస;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      కొరకు(అడ్డువరుస = 0; అడ్డువరుస < state->అడ్డువరుసలు_2; అడ్డువరుస++)
        సమితి_lineinfo(state, అడ్డువరుస, FORCE, DWL_OFF, DHL_OFF);
    }

    ఆపు;

  స్థితి 1000:
  స్థితి 1002:
  స్థితి 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (అంకె == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (అంకె == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    ఆపు;

  స్థితి 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->బహుళకం.report_focus = val;
    ఆపు;

  స్థితి 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    ఆపు;

  స్థితి 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    ఆపు;

  స్థితి 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    ఆపు;

  స్థితి 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    ఆపు;

  స్థితి 1048:
    savecursor(state, val);
    ఆపు;

  స్థితి 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    ఆపు;

  స్థితి 2004:
    state->బహుళకం.bracketpaste = val;
    ఆపు;

  అప్రమేయ:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", అంకె);
    తిరిగి;
  }
}

స్థిర శూన్య request_dec_బహుళకం(VTermState *state, పూర్ణాంకం అంకె)
{
  పూర్ణాంకం జవాబు;

  ఎంపిక(అంకె) {
    స్థితి 1:
      జవాబు = state->బహుళకం.cursor;
      ఆపు;

    స్థితి 5:
      జవాబు = state->బహుళకం.screen;
      ఆపు;

    స్థితి 6:
      జవాబు = state->బహుళకం.origin;
      ఆపు;

    స్థితి 7:
      జవాబు = state->బహుళకం.autowrap;
      ఆపు;

    స్థితి 12:
      జవాబు = state->బహుళకం.cursor_blink;
      ఆపు;

    స్థితి 25:
      జవాబు = state->బహుళకం.cursor_visible;
      ఆపు;

    స్థితి 69:
      జవాబు = state->బహుళకం.leftrightmargin;
      ఆపు;

    స్థితి 1000:
      జవాబు = state->mouse_flags == MOUSE_WANT_CLICK;
      ఆపు;

    స్థితి 1002:
      జవాబు = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      ఆపు;

    స్థితి 1003:
      జవాబు = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      ఆపు;

    స్థితి 1004:
      జవాబు = state->బహుళకం.report_focus;
      ఆపు;

    స్థితి 1005:
      జవాబు = state->mouse_protocol == MOUSE_UTF8;
      ఆపు;

    స్థితి 1006:
      జవాబు = state->mouse_protocol == MOUSE_SGR;
      ఆపు;

    స్థితి 1015:
      జవాబు = state->mouse_protocol == MOUSE_RXVT;
      ఆపు;

    స్థితి 1047:
      జవాబు = state->బహుళకం.alt_screen;
      ఆపు;

    స్థితి 2004:
      జవాబు = state->బహుళకం.bracketpaste;
      ఆపు;

    అప్రమేయ:
      vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", అంకె, 0);
      తిరిగి;
  }

  vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", అంకె, జవాబు ? 1 : 2);
}

స్థిర శూన్య request_సంస్కరణ_string(VTermState *state)
{
  vterm_నెట్టు_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_సంస్కరణ_MAJOR, VTERM_సంస్కరణ_MINOR);
}

స్థిర పూర్ణాంకం on_csi(స్థిరరాశి అక్షరం *leader, స్థిరరాశి దీర్ఘ వాదనలు[], పూర్ణాంకం argcount, స్థిరరాశి అక్షరం *intermed, అక్షరం command, శూన్య *user)
{
  VTermState *state = user;
  పూర్ణాంకం leader_byte = 0;
  పూర్ణాంకం intermed_byte = 0;
  పూర్ణాంకం cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  పూర్ణాంకం handled = 1;

  // Some temporaries for later code
  పూర్ణాంకం లెక్క, val;
  పూర్ణాంకం అడ్డువరుస, నిలువు;
  VTermRect rect;
  పూర్ణాంకం selective;

  ఒకవేళ(leader && leader[0]) {
    ఒకవేళ(leader[1]) // longer than 1 char
      తిరిగి 0;

    ఎంపిక(leader[0]) {
    స్థితి '?':
    స్థితి '>':
    స్థితి '<':
      leader_byte = leader[0];
      ఆపు;
    అప్రమేయ:
      తిరిగి 0;
    }
  }

  ఒకవేళ(intermed && intermed[0]) {
    ఒకవేళ(intermed[1]) // longer than 1 char
      తిరిగి 0;

    ఎంపిక(intermed[0]) {
    స్థితి ' ':
    స్థితి '!':
    స్థితి '"':
    స్థితి '$':
    స్థితి '\'':
      intermed_byte = intermed[0];
      ఆపు;
    అప్రమేయ:
      తిరిగి 0;
    }
  }

  oldpos = state->pos;

#నిర్వచించు LBOUND(v,min) ఒకవేళ((v) < (min)) (v) = (min)
#నిర్వచించు UBOUND(v,max) ఒకవేళ((v) > (max)) (v) = (max)

#నిర్వచించు LEADER(l,b) ((l << 8) | b)
#నిర్వచించు INTERMED(i,b) ((i << 16) | b)

  ఎంపిక(intermed_byte << 16 | leader_byte << 8 | command) {
  స్థితి 0x40: // ICH - ECMA-48 8.3.64
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    ఒకవేళ(!is_cursor_లోపల_scrollregion(state))
      ఆపు;

    rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస;
    rect.చివర_అడ్డువరుస   = state->pos.అడ్డువరుస + 1;
    rect.ప్రారంభం_నిలువు = state->pos.నిలువు;
    ఒకవేళ(state->బహుళకం.leftrightmargin)
      rect.చివర_నిలువు = SCROLLREGION_కుడి(state);
    లేకపోతే
      rect.చివర_నిలువు = THISROWWIDTH(state);

    scroll(state, rect, 0, -లెక్క);

    ఆపు;

  స్థితి 0x41: // CUU - ECMA-48 8.3.22
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.అడ్డువరుస -= లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x42: // CUD - ECMA-48 8.3.19
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.అడ్డువరుస += లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x43: // CUF - ECMA-48 8.3.20
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.నిలువు += లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x44: // CUB - ECMA-48 8.3.18
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.నిలువు -= లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x45: // CNL - ECMA-48 8.3.12
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.నిలువు = 0;
    state->pos.అడ్డువరుస += లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x46: // CPL - ECMA-48 8.3.13
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.నిలువు = 0;
    state->pos.అడ్డువరుస -= లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(వాదనలు[0], 1);
    state->pos.నిలువు = val-1;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x48: // CUP - ECMA-48 8.3.21
    అడ్డువరుస = CSI_ARG_OR(వాదనలు[0], 1);
    నిలువు = argcount < 2 || CSI_ARG_IS_MISSING(వాదనలు[1]) ? 1 : CSI_ARG(వాదనలు[1]);
    // zero-based
    ఒకవేళ(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      ఒకవేళ(state->pos.అడ్డువరుస == అడ్డువరుస - 1) {
        పూర్ణాంకం cnt, ptr = 0;
        కొరకు(cnt = 0; cnt < నిలువు - 1; ++cnt) {
	  VTermPos p;
	  VTermScreenCell c0, c1;
	  p.అడ్డువరుస = అడ్డువరుస - 1;
	  p.నిలువు = ptr;
	  vterm_screen_get_cell(state->vt->screen, p, &c0);
	  p.నిలువు++;
	  vterm_screen_get_cell(state->vt->screen, p, &c1);
	  ptr += (c1.chars[0] == (uint32_t)-1)		    // double cell?
	     ? (vterm_unicode_is_ambiguous(c0.chars[0]))    // is ambiguous?
	     ? vterm_unicode_వెడల్పు(0x00a1) : 1		    // &ambiwidth
	     : 1;					    // not ambiguous
        }
        నిలువు = ptr + 1;
      }
    }
    state->pos.అడ్డువరుస = అడ్డువరుస-1;
    state->pos.నిలువు = నిలువు-1;
    ఒకవేళ(state->బహుళకం.origin) {
      state->pos.అడ్డువరుస += state->scrollregion_శిఖరం;
      state->pos.నిలువు += SCROLLREGION_ఎడమ(state);
    }
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x49: // CHT - ECMA-48 8.3.10
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    tab(state, లెక్క, +1);
    ఆపు;

  స్థితి 0x4a: // ED - ECMA-48 8.3.39
  స్థితి LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    ఎంపిక(CSI_ARG(వాదనలు[0])) {
    స్థితి CSI_ARG_MISSING:
    స్థితి 0:
      rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస; rect.చివర_అడ్డువరుస = state->pos.అడ్డువరుస + 1;
      rect.ప్రారంభం_నిలువు = state->pos.నిలువు; rect.చివర_నిలువు = state->నిలువుల_సంఖ్య;
      ఒకవేళ(rect.చివర_నిలువు > rect.ప్రారంభం_నిలువు)
        erase(state, rect, selective);

      rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస + 1; rect.చివర_అడ్డువరుస = state->అడ్డువరుసలు_2;
      rect.ప్రారంభం_నిలువు = 0;
      కొరకు(అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస; అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++)
	సమితి_lineinfo(state, అడ్డువరుస, FORCE, DWL_OFF, DHL_OFF);
      ఒకవేళ(rect.చివర_అడ్డువరుస > rect.ప్రారంభం_అడ్డువరుస)
        erase(state, rect, selective);
      ఆపు;

    స్థితి 1:
      rect.ప్రారంభం_అడ్డువరుస = 0; rect.చివర_అడ్డువరుస = state->pos.అడ్డువరుస;
      rect.ప్రారంభం_నిలువు = 0; rect.చివర_నిలువు = state->నిలువుల_సంఖ్య;
      కొరకు(అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస; అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++)
	సమితి_lineinfo(state, అడ్డువరుస, FORCE, DWL_OFF, DHL_OFF);
      ఒకవేళ(rect.చివర_నిలువు > rect.ప్రారంభం_నిలువు)
        erase(state, rect, selective);

      rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస; rect.చివర_అడ్డువరుస = state->pos.అడ్డువరుస + 1;
                          rect.చివర_నిలువు = state->pos.నిలువు + 1;
      ఒకవేళ(rect.చివర_అడ్డువరుస > rect.ప్రారంభం_అడ్డువరుస)
        erase(state, rect, selective);
      ఆపు;

    స్థితి 2:
      rect.ప్రారంభం_అడ్డువరుస = 0; rect.చివర_అడ్డువరుస = state->అడ్డువరుసలు_2;
      rect.ప్రారంభం_నిలువు = 0; rect.చివర_నిలువు = state->నిలువుల_సంఖ్య;
      కొరకు(అడ్డువరుస = rect.ప్రారంభం_అడ్డువరుస; అడ్డువరుస < rect.చివర_అడ్డువరుస; అడ్డువరుస++)
	సమితి_lineinfo(state, అడ్డువరుస, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      ఆపు;

    స్థితి 3:
      ఒకవేళ(state->callbacks && state->callbacks->sb_clear)
        ఒకవేళ((*state->callbacks->sb_clear)(state->cbdata))
          తిరిగి 1;
      ఆపు;
    }
    ఆపు;

  స్థితి 0x4b: // EL - ECMA-48 8.3.41
  స్థితి LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస;
    rect.చివర_అడ్డువరుస   = state->pos.అడ్డువరుస + 1;

    ఎంపిక(CSI_ARG(వాదనలు[0])) {
    స్థితి CSI_ARG_MISSING:
    స్థితి 0:
      rect.ప్రారంభం_నిలువు = state->pos.నిలువు; rect.చివర_నిలువు = THISROWWIDTH(state); ఆపు;
    స్థితి 1:
      rect.ప్రారంభం_నిలువు = 0; rect.చివర_నిలువు = state->pos.నిలువు + 1; ఆపు;
    స్థితి 2:
      rect.ప్రారంభం_నిలువు = 0; rect.చివర_నిలువు = THISROWWIDTH(state); ఆపు;
    అప్రమేయ:
      తిరిగి 0;
    }

    ఒకవేళ(rect.చివర_నిలువు > rect.ప్రారంభం_నిలువు)
      erase(state, rect, selective);

    ఆపు;

  స్థితి 0x4c: // IL - ECMA-48 8.3.67
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    ఒకవేళ(!is_cursor_లోపల_scrollregion(state))
      ఆపు;

    rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస;
    rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
    rect.ప్రారంభం_నిలువు = SCROLLREGION_ఎడమ(state);
    rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

    scroll(state, rect, -లెక్క, 0);

    ఆపు;

  స్థితి 0x4d: // DL - ECMA-48 8.3.32
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    ఒకవేళ(!is_cursor_లోపల_scrollregion(state))
      ఆపు;

    rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస;
    rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
    rect.ప్రారంభం_నిలువు = SCROLLREGION_ఎడమ(state);
    rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

    scroll(state, rect, లెక్క, 0);

    ఆపు;

  స్థితి 0x50: // DCH - ECMA-48 8.3.26
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    ఒకవేళ(!is_cursor_లోపల_scrollregion(state))
      ఆపు;

    rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస;
    rect.చివర_అడ్డువరుస   = state->pos.అడ్డువరుస + 1;
    rect.ప్రారంభం_నిలువు = state->pos.నిలువు;
    ఒకవేళ(state->బహుళకం.leftrightmargin)
      rect.చివర_నిలువు = SCROLLREGION_కుడి(state);
    లేకపోతే
      rect.చివర_నిలువు = THISROWWIDTH(state);

    scroll(state, rect, 0, లెక్క);

    ఆపు;

  స్థితి 0x53: // SU - ECMA-48 8.3.147
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    rect.ప్రారంభం_అడ్డువరుస = state->scrollregion_శిఖరం;
    rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
    rect.ప్రారంభం_నిలువు = SCROLLREGION_ఎడమ(state);
    rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

    scroll(state, rect, లెక్క, 0);

    ఆపు;

  స్థితి 0x54: // SD - ECMA-48 8.3.113
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    rect.ప్రారంభం_అడ్డువరుస = state->scrollregion_శిఖరం;
    rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
    rect.ప్రారంభం_నిలువు = SCROLLREGION_ఎడమ(state);
    rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

    scroll(state, rect, -లెక్క, 0);

    ఆపు;

  స్థితి 0x58: // ECH - ECMA-48 8.3.38
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    rect.ప్రారంభం_అడ్డువరుస = state->pos.అడ్డువరుస;
    rect.చివర_అడ్డువరుస   = state->pos.అడ్డువరుస + 1;
    rect.ప్రారంభం_నిలువు = state->pos.నిలువు;
    rect.చివర_నిలువు   = state->pos.నిలువు + లెక్క;
    UBOUND(rect.చివర_నిలువు, THISROWWIDTH(state));

    erase(state, rect, 0);
    ఆపు;

  స్థితి 0x5a: // CBT - ECMA-48 8.3.7
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    tab(state, లెక్క, -1);
    ఆపు;

  స్థితి 0x60: // HPA - ECMA-48 8.3.57
    నిలువు = CSI_ARG_OR(వాదనలు[0], 1);
    state->pos.నిలువు = నిలువు-1;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x61: // HPR - ECMA-48 8.3.59
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.నిలువు += లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x62: { // REP - ECMA-48 8.3.103
    స్థిరరాశి పూర్ణాంకం అడ్డువరుస_వెడల్పు = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    ఒకవేళ(state->combine_వెడల్పు < 1)
      ఆపు;

    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    నిలువు = state->pos.నిలువు + లెక్క;
    UBOUND(నిలువు, అడ్డువరుస_వెడల్పు);
    అయితే (state->pos.నిలువు < నిలువు) {
      putglyph(state, state->combine_chars, state->combine_వెడల్పు, state->pos);
      state->pos.నిలువు += state->combine_వెడల్పు;
    }
    ఒకవేళ (state->pos.నిలువు + state->combine_వెడల్పు >= అడ్డువరుస_వెడల్పు) {
      ఒకవేళ (state->బహుళకం.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    ఆపు;
  }

  స్థితి 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(వాదనలు[0], 0);
    ఒకవేళ(val == 0)
      // DEC VT100 response
      vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    ఆపు;

  స్థితి LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    ఆపు;

  స్థితి 0x64: // VPA - ECMA-48 8.3.158
    అడ్డువరుస = CSI_ARG_OR(వాదనలు[0], 1);
    state->pos.అడ్డువరుస = అడ్డువరుస-1;
    ఒకవేళ(state->బహుళకం.origin)
      state->pos.అడ్డువరుస += state->scrollregion_శిఖరం;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x65: // VPR - ECMA-48 8.3.160
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.అడ్డువరుస += లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x66: // HVP - ECMA-48 8.3.63
    అడ్డువరుస = CSI_ARG_OR(వాదనలు[0], 1);
    నిలువు = argcount < 2 || CSI_ARG_IS_MISSING(వాదనలు[1]) ? 1 : CSI_ARG(వాదనలు[1]);
    // zero-based
    state->pos.అడ్డువరుస = అడ్డువరుస-1;
    state->pos.నిలువు = నిలువు-1;
    ఒకవేళ(state->బహుళకం.origin) {
      state->pos.అడ్డువరుస += state->scrollregion_శిఖరం;
      state->pos.నిలువు += SCROLLREGION_ఎడమ(state);
    }
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(వాదనలు[0], 0);

    ఎంపిక(val) {
    స్థితి 0:
      clear_నిలువు_tabstop(state, state->pos.నిలువు);
      ఆపు;
    స్థితి 3:
    స్థితి 5:
      కొరకు(నిలువు = 0; నిలువు < state->నిలువుల_సంఖ్య; నిలువు++)
        clear_నిలువు_tabstop(state, నిలువు);
      ఆపు;
    స్థితి 1:
    స్థితి 2:
    స్థితి 4:
      ఆపు;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    అప్రమేయ:
      తిరిగి 0;
    }
    ఆపు;

  స్థితి 0x68: // SM - ECMA-48 8.3.125
    ఒకవేళ(!CSI_ARG_IS_MISSING(వాదనలు[0]))
      సమితి_బహుళకం(state, CSI_ARG(వాదనలు[0]), 1);
    ఆపు;

  స్థితి LEADER('?', 0x68): // DEC private mode set
    కొరకు(పూర్ణాంకం i = 0; i < argcount; i++) {
      ఒకవేళ(!CSI_ARG_IS_MISSING(వాదనలు[i]))
        సమితి_dec_బహుళకం(state, CSI_ARG(వాదనలు[i]), 1);
    }
    ఆపు;

  స్థితి 0x6a: // HPB - ECMA-48 8.3.58
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.నిలువు -= లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x6b: // VPB - ECMA-48 8.3.159
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);
    state->pos.అడ్డువరుస -= లెక్క;
    state->at_phantom = 0;
    ఆపు;

  స్థితి 0x6c: // RM - ECMA-48 8.3.106
    ఒకవేళ(!CSI_ARG_IS_MISSING(వాదనలు[0]))
      సమితి_బహుళకం(state, CSI_ARG(వాదనలు[0]), 0);
    ఆపు;

  స్థితి LEADER('?', 0x6c): // DEC private mode reset
    కొరకు(పూర్ణాంకం i = 0; i < argcount; i++) {
      ఒకవేళ(!CSI_ARG_IS_MISSING(వాదనలు[i]))
        సమితి_dec_బహుళకం(state, CSI_ARG(వాదనలు[i]), 0);
    }
    ఆపు;

  స్థితి 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, వాదనలు, argcount);
    ఆపు;

  స్థితి LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    ఒకవేళ (argcount == 1 && CSI_ARG(వాదనలు[0]) == 4)
    {
      vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->బహుళకం.modify_other_కీలకాలు ? 2 : 0);
      ఆపు;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    కొరకు(పూర్ణాంకం argi = 0; argi < argcount; argi++) {
      దీర్ఘ arg;
      ఎంపిక(arg = CSI_ARG(వాదనలు[argi])) {
        స్థితి 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          ఆపు;
        స్థితి 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          ఆపు;
        స్థితి 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          ఆపు;
      }
    }
    ఆపు;

  స్థితి LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    ఒకవేళ (argcount == 2 && CSI_ARG(వాదనలు[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->బహుళకం.kitty_keyboard = 0;

      state->బహుళకం.modify_other_కీలకాలు = CSI_ARG(వాదనలు[1]) == 2;
    }
    ఆపు;

  స్థితి LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    ఒకవేళ (argcount == 1 && CSI_ARG(వాదనలు[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->బహుళకం.modify_other_కీలకాలు = 0;

      state->బహుళకం.kitty_keyboard = 1;
    }
    ఆపు;

  స్థితి LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    ఒకవేళ (argcount <= 1)
      state->బహుళకం.kitty_keyboard = 0;
    ఆపు;

  స్థితి LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    ఒకవేళ (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->బహుళకం.kitty_keyboard);
    ఆపు;

  స్థితి 0x6e: // DSR - ECMA-48 8.3.35
  స్థితి LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(వాదనలు[0], 0);

    {
      అక్షరం *qmark = (leader_byte == '?') ? "?" : "";

      ఎంపిక(val) {
      స్థితి 0: స్థితి 1: స్థితి 2: స్థితి 3: స్థితి 4:
        // ignore - these are replies
        ఆపు;
      స్థితి 5:
        vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        ఆపు;
      స్థితి 6: // CPR - cursor position report
        vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.అడ్డువరుస + 1, state->pos.నిలువు + 1);
        ఆపు;
      }
    }
    ఆపు;


  స్థితి INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    ఆపు;

  స్థితి LEADER('?', INTERMED('$', 0x70)):
    request_dec_బహుళకం(state, CSI_ARG(వాదనలు[0]));
    ఆపు;

  స్థితి LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_సంస్కరణ_string(state);
    ఆపు;

  స్థితి INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(వాదనలు[0], 1);

    ఎంపిక(val) {
    స్థితి 0: స్థితి 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_ఖండం);
      ఆపు;
    స్థితి 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_ఖండం);
      ఆపు;
    స్థితి 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ఆపు;
    స్థితి 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ఆపు;
    స్థితి 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ఎడమ);
      ఆపు;
    స్థితి 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ఎడమ);
      ఆపు;
    }

    ఆపు;

  స్థితి INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(వాదనలు[0], 0);

    ఎంపిక(val) {
    స్థితి 0: స్థితి 2:
      state->protected_cell = 0;
      ఆపు;
    స్థితి 1:
      state->protected_cell = 1;
      ఆపు;
    }

    ఆపు;

  స్థితి 0x72: // DECSTBM - DEC custom
    state->scrollregion_శిఖరం = CSI_ARG_OR(వాదనలు[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(వాదనలు[1]) ? -1 : CSI_ARG(వాదనలు[1]);
    LBOUND(state->scrollregion_శిఖరం, 0);
    UBOUND(state->scrollregion_శిఖరం, state->అడ్డువరుసలు_2);
    LBOUND(state->scrollregion_bottom, -1);
    ఒకవేళ(state->scrollregion_శిఖరం == 0 && state->scrollregion_bottom == state->అడ్డువరుసలు_2)
      state->scrollregion_bottom = -1;
    లేకపోతే
      UBOUND(state->scrollregion_bottom, state->అడ్డువరుసలు_2);

    ఒకవేళ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_శిఖరం) {
      // Invalid
      state->scrollregion_శిఖరం    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.అడ్డువరుస = 0;
    state->pos.నిలువు = 0;
    ఒకవేళ(state->బహుళకం.origin) {
      state->pos.అడ్డువరుస += state->scrollregion_శిఖరం;
      state->pos.నిలువు += SCROLLREGION_ఎడమ(state);
    }

    ఆపు;

  స్థితి 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_ఎడమ = CSI_ARG_OR(వాదనలు[0], 1) - 1;
    state->scrollregion_కుడి = argcount < 2 || CSI_ARG_IS_MISSING(వాదనలు[1]) ? -1 : CSI_ARG(వాదనలు[1]);
    LBOUND(state->scrollregion_ఎడమ, 0);
    UBOUND(state->scrollregion_ఎడమ, state->నిలువుల_సంఖ్య);
    LBOUND(state->scrollregion_కుడి, -1);
    ఒకవేళ(state->scrollregion_ఎడమ == 0 && state->scrollregion_కుడి == state->నిలువుల_సంఖ్య)
      state->scrollregion_కుడి = -1;
    లేకపోతే
      UBOUND(state->scrollregion_కుడి, state->నిలువుల_సంఖ్య);

    ఒకవేళ(state->scrollregion_కుడి > -1 &&
       state->scrollregion_కుడి <= state->scrollregion_ఎడమ) {
      // Invalid
      state->scrollregion_ఎడమ  = 0;
      state->scrollregion_కుడి = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.అడ్డువరుస = 0;
    state->pos.నిలువు = 0;
    ఒకవేళ(state->బహుళకం.origin) {
      state->pos.అడ్డువరుస += state->scrollregion_శిఖరం;
      state->pos.నిలువు += SCROLLREGION_ఎడమ(state);
    }

    ఆపు;

  స్థితి 0x74:
    ఎంపిక(CSI_ARG(వాదనలు[0])) {
      స్థితి 8: // CSI 8 ; rows ; cols t  set size
        ఒకవేళ (argcount == 3 &&
            !CSI_ARG_IS_MISSING(వాదనలు[1]) && !CSI_ARG_IS_MISSING(వాదనలు[2]) &&
            CSI_ARG(వాదనలు[1]) > 0 && CSI_ARG(వాదనలు[2]) > 0)
	  on_resize(CSI_ARG(వాదనలు[1]), CSI_ARG(వాదనలు[2]), state);
	ఆపు;
      అప్రమేయ:
	handled = 0;
	ఆపు;
    }
    ఆపు;

  స్థితి INTERMED('\'', 0x7D): // DECIC
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    ఒకవేళ(!is_cursor_లోపల_scrollregion(state))
      ఆపు;

    rect.ప్రారంభం_అడ్డువరుస = state->scrollregion_శిఖరం;
    rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
    rect.ప్రారంభం_నిలువు = state->pos.నిలువు;
    rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

    scroll(state, rect, 0, -లెక్క);

    ఆపు;

  స్థితి INTERMED('\'', 0x7E): // DECDC
    లెక్క = CSI_ARG_లెక్క(వాదనలు[0]);

    ఒకవేళ(!is_cursor_లోపల_scrollregion(state))
      ఆపు;

    rect.ప్రారంభం_అడ్డువరుస = state->scrollregion_శిఖరం;
    rect.చివర_అడ్డువరుస   = SCROLLREGION_BOTTOM(state);
    rect.ప్రారంభం_నిలువు = state->pos.నిలువు;
    rect.చివర_నిలువు   = SCROLLREGION_కుడి(state);

    scroll(state, rect, 0, లెక్క);

    ఆపు;

  అప్రమేయ:
    handled = 0;
    ఆపు;
  }

  ఒకవేళ (!handled) {
    ఒకవేళ(state->fallbacks && state->fallbacks->csi)
      ఒకవేళ((*state->fallbacks->csi)(leader, వాదనలు, argcount, intermed, command, state->fbdata))
        తిరిగి 1;

    తిరిగి 0;
  }

  ఒకవేళ(state->బహుళకం.origin) {
    LBOUND(state->pos.అడ్డువరుస, state->scrollregion_శిఖరం);
    UBOUND(state->pos.అడ్డువరుస, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.నిలువు, SCROLLREGION_ఎడమ(state));
    UBOUND(state->pos.నిలువు, SCROLLREGION_కుడి(state)-1);
  }
  లేకపోతే {
    LBOUND(state->pos.అడ్డువరుస, 0);
    UBOUND(state->pos.అడ్డువరుస, state->అడ్డువరుసలు_2-1);
    LBOUND(state->pos.నిలువు, 0);
    UBOUND(state->pos.నిలువు, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#నిర్వచితమైతే DEBUG
  ఒకవేళ(state->pos.అడ్డువరుస < 0 || state->pos.అడ్డువరుస >= state->అడ్డువరుసలు_2 ||
     state->pos.నిలువు < 0 || state->pos.నిలువు >= state->నిలువుల_సంఖ్య) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.అడ్డువరుస, state->pos.నిలువు);
    abort();
  }

  ఒకవేళ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_శిఖరం) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_శిఖరం);
    abort();
  }

  ఒకవేళ(SCROLLREGION_కుడి(state) <= SCROLLREGION_ఎడమ(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_కుడి(state), SCROLLREGION_ఎడమ(state));
    abort();
  }
#అయితే_ముగింపు

  తిరిగి 1;
}

స్థిర అక్షరం base64_one(uint8_t b)
{
  ఒకవేళ(b < 26)
    తిరిగి 'A' + b;
  లేకపోతే ఒకవేళ(b < 52)
    తిరిగి 'a' + b - 26;
  లేకపోతే ఒకవేళ(b < 62)
    తిరిగి '0' + b - 52;
  లేకపోతే ఒకవేళ(b == 62)
    తిరిగి '+';
  లేకపోతే ఒకవేళ(b == 63)
    తిరిగి '/';
  తిరిగి 0;
}

స్థిర uint8_t unbase64one(అక్షరం c)
{
  ఒకవేళ(c >= 'A' && c <= 'Z')
    తిరిగి c - 'A';
  లేకపోతే ఒకవేళ(c >= 'a' && c <= 'z')
    తిరిగి c - 'a' + 26;
  లేకపోతే ఒకవేళ(c >= '0' && c <= '9')
    తిరిగి c - '0' + 52;
  లేకపోతే ఒకవేళ(c == '+')
    తిరిగి 62;
  లేకపోతే ఒకవేళ(c == '/')
    తిరిగి 63;

  తిరిగి 0xFF;
}

స్థిర శూన్య osc_selection(VTermState *state, VTermStringFragment frag)
{
  ఒకవేళ(frag.initial) {
    state->tmp.selection.ముసుగు = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  అయితే(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    ఎంపిక(frag.str[0]) {
      స్థితి 'c':
        state->tmp.selection.ముసుగు |= VTERM_SELECTION_CLIPBOARD;
        ఆపు;
      స్థితి 'p':
        state->tmp.selection.ముసుగు |= VTERM_SELECTION_PRIMARY;
        ఆపు;
      స్థితి 'q':
        state->tmp.selection.ముసుగు |= VTERM_SELECTION_SECONDARY;
        ఆపు;
      స్థితి 's':
        state->tmp.selection.ముసుగు |= VTERM_SELECTION_SELECT;
        ఆపు;
      స్థితి '0':
      స్థితి '1':
      స్థితి '2':
      స్థితి '3':
      స్థితి '4':
      స్థితి '5':
      స్థితి '6':
      స్థితి '7':
        state->tmp.selection.ముసుగు |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        ఆపు;

      స్థితి ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        ఒకవేళ(!state->tmp.selection.ముసుగు)
          state->tmp.selection.ముసుగు = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        ఆపు;
    }

    frag.str++;
    frag.len--;
  }

  ఒకవేళ(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    ఒకవేళ(frag.final && state->selection.callbacks->సమితి_2) {
      (*state->selection.callbacks->సమితి_2)(state->tmp.selection.ముసుగు, (VTermStringFragment){
              .str     = ఖాళీ,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_సమితి,
              .final   = TRUE,
            }, state->selection.user);
    }
    తిరిగి;
  }

  ఒకవేళ(state->tmp.selection.state == SELECTION_SELECTED) {
    ఒకవేళ(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    లేకపోతే {
      state->tmp.selection.state = SELECTION_సమితి_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  ఒకవేళ(state->tmp.selection.state == SELECTION_QUERY) {
    ఒకవేళ(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.ముసుగు, state->selection.user);
    తిరిగి;
  }

  ఒకవేళ(state->tmp.selection.state == SELECTION_INVALID)
    తిరిగి;

  ఒకవేళ(state->selection.callbacks->సమితి_2) {
    size_t bufcur = 0;
    అక్షరం *బఫరు = state->selection.బఫరు;

    uint32_t x = 0; /* Current decoding value */
    పూర్ణాంకం n = 0;      /* Number of sextets consumed */

    ఒకవేళ(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    అయితే((state->selection.buflen - bufcur) >= 3 && frag.len) {
      ఒకవేళ(frag.str[0] == '=') {
        ఒకవేళ(n == 2) {
          బఫరు[0] = (x >> 4) & 0xFF;
          బఫరు += 1, bufcur += 1;
        }
        ఒకవేళ(n == 3) {
          బఫరు[0] = (x >> 10) & 0xFF;
          బఫరు[1] = (x >>  2) & 0xFF;
          బఫరు += 2, bufcur += 2;
        }

        అయితే(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      లేకపోతే {
        uint8_t b = unbase64one(frag.str[0]);
        ఒకవేళ(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          ఒకవేళ(state->selection.callbacks->సమితి_2) {
            (*state->selection.callbacks->సమితి_2)(state->tmp.selection.ముసుగు, (VTermStringFragment){
                .str     = ఖాళీ,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          ఆపు;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        ఒకవేళ(n == 4) {
          బఫరు[0] = (x >> 16) & 0xFF;
          బఫరు[1] = (x >>  8) & 0xFF;
          బఫరు[2] = (x >>  0) & 0xFF;

          బఫరు += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      ఒకవేళ(!frag.len || (state->selection.buflen - bufcur) < 3) {
        ఒకవేళ(bufcur) {
	  VTermStringFragment setfrag = {
	    state->selection.బఫరు, // str
	    bufcur, // len
	    state->tmp.selection.state == SELECTION_సమితి_INITIAL, // initial
	    frag.final && !frag.len // final
	  };
          (*state->selection.callbacks->సమితి_2)(state->tmp.selection.ముసుగు,
	      setfrag, state->selection.user);
          state->tmp.selection.state = SELECTION_సమితి;
        }

        బఫరు = state->selection.బఫరు;
        bufcur = 0;
      }
    }

    ఒకవేళ(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

స్థిర పూర్ణాంకం on_osc(పూర్ణాంకం command, VTermStringFragment frag, శూన్య *user)
{
  VTermState *state = user;

  ఎంపిక(command) {
    స్థితి 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      తిరిగి 1;

    స్థితి 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      తిరిగి 1;

    స్థితి 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      తిరిగి 1;

    స్థితి 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        పూర్ణాంకం red = state->default_fg.red;
        పూర్ణాంకం blue = state->default_fg.blue;
        పూర్ణాంకం green = state->default_fg.green;
        vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        తిరిగి 1;
      }

    స్థితి 11:
      {
	// request background color: <Esc>]11;?<0x07>
	పూర్ణాంకం red = state->default_bg.red;
	పూర్ణాంకం blue = state->default_bg.blue;
	పూర్ణాంకం green = state->default_bg.green;
	vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	తిరిగి 1;
      }
    స్థితి 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      తిరిగి 1;

    స్థితి 52:
      ఒకవేళ(state->selection.callbacks)
        osc_selection(state, frag);

      తిరిగి 1;

    అప్రమేయ:
      ఒకవేళ(state->fallbacks && state->fallbacks->osc)
        ఒకవేళ((*state->fallbacks->osc)(command, frag, state->fbdata))
          తిరిగి 1;
  }

  తిరిగి 0;
}

స్థిర శూన్య request_స్థితి_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  అక్షరం *tmp = state->tmp.decrqss;

  ఒకవేళ(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  అయితే(i < పరిమాణం(state->tmp.decrqss)-1 && tmp[i])
    i++;
  అయితే(i < పరిమాణం(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  ఒకవేళ(!frag.final)
    తిరిగి;

  ఎంపిక(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    స్థితి 'm': {
      // Query SGR
      దీర్ఘ వాదనలు[20];
      పూర్ణాంకం argc = vterm_state_getpen(state, వాదనలు, పరిమాణం(వాదనలు)/పరిమాణం(వాదనలు[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->బహుళకం.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      ఒకవేళ(cur >= vt->tmpbuffer_len)
        తిరిగి;

      కొరకు(పూర్ణాంకం argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_మరిన్ని(వాదనలు[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(వాదనలు[argi]));
        ఒకవేళ(cur >= vt->tmpbuffer_len)
          తిరిగి;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->బహుళకం.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      ఒకవేళ(cur >= vt->tmpbuffer_len)
        తిరిగి;

      vterm_నెట్టు_output_బైట్లు(vt, vt->tmpbuffer, cur);
      తిరిగి;
    }

    స్థితి 'r':
      // Query DECSTBM
      vterm_నెట్టు_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_శిఖరం+1, SCROLLREGION_BOTTOM(state));
      తిరిగి;

    స్థితి 's':
      // Query DECSLRM
      vterm_నెట్టు_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_ఎడమ(state)+1, SCROLLREGION_కుడి(state));
      తిరిగి;

    స్థితి ' '|('q'<<8): {
      // Query DECSCUSR
      పూర్ణాంకం జవాబు;
      ఎంపిక(state->బహుళకం.cursor_shape) {
        స్థితి VTERM_PROP_CURSORSHAPE_ఖండం:     జవాబు = 2; ఆపు;
        స్థితి VTERM_PROP_CURSORSHAPE_UNDERLINE: జవాబు = 4; ఆపు;
	అప్రమేయ: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ జవాబు = 6; ఆపు;
      }
      ఒకవేళ(state->బహుళకం.cursor_blink)
        జవాబు--;
      vterm_నెట్టు_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", జవాబు);
      తిరిగి;
    }

    స్థితి '\"'|('q'<<8):
      // Query DECSCA
      vterm_నెట్టు_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      తిరిగి;
  }

  vterm_నెట్టు_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

స్థిర పూర్ణాంకం on_dcs(స్థిరరాశి అక్షరం *command, size_t commandlen, VTermStringFragment frag, శూన్య *user)
{
  VTermState *state = user;

  ఒకవేళ(commandlen == 2 && strneq(command, "$q", 2)) {
    request_స్థితి_string(state, frag);
    తిరిగి 1;
  }
  లేకపోతే ఒకవేళ(state->fallbacks && state->fallbacks->dcs)
    ఒకవేళ((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      తిరిగి 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (పూర్ణాంకం)commandlen, command);
  తిరిగి 0;
}

స్థిర పూర్ణాంకం on_apc(VTermStringFragment frag, శూన్య *user)
{
  VTermState *state = user;

  ఒకవేళ(state->fallbacks && state->fallbacks->apc)
    ఒకవేళ((*state->fallbacks->apc)(frag, state->fbdata))
      తిరిగి 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  తిరిగి 0;
}

స్థిర పూర్ణాంకం on_pm(VTermStringFragment frag, శూన్య *user)
{
  VTermState *state = user;

  ఒకవేళ(state->fallbacks && state->fallbacks->pm)
    ఒకవేళ((*state->fallbacks->pm)(frag, state->fbdata))
      తిరిగి 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  తిరిగి 0;
}

స్థిర పూర్ణాంకం on_sos(VTermStringFragment frag, శూన్య *user)
{
  VTermState *state = user;

  ఒకవేళ(state->fallbacks && state->fallbacks->sos)
    ఒకవేళ((*state->fallbacks->sos)(frag, state->fbdata))
      తిరిగి 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  తిరిగి 0;
}

స్థిర పూర్ణాంకం on_resize(పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య, శూన్య *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  ఒకవేళ (నిలువుల_సంఖ్య > VTERM_MAX_నిలువుల_సంఖ్య)
    నిలువుల_సంఖ్య = VTERM_MAX_నిలువుల_సంఖ్య;

  ఒకవేళ (అడ్డువరుసలు_2 > VTERM_MAX_అడ్డువరుసలు)
    అడ్డువరుసలు_2 = VTERM_MAX_అడ్డువరుసలు;

  ఒకవేళ(నిలువుల_సంఖ్య != state->నిలువుల_సంఖ్య) {
    గుర్తులేని అక్షరం *newtabstops = vterm_allocator_malloc(state->vt, (నిలువుల_సంఖ్య + 7) / 8);
    ఒకవేళ (newtabstops == ఖాళీ)
      తిరిగి 0;

    /* TODO: This can all be done much more efficiently bytewise */
    పూర్ణాంకం నిలువు;
    కొరకు(నిలువు = 0; నిలువు < state->నిలువుల_సంఖ్య && నిలువు < నిలువుల_సంఖ్య; నిలువు++) {
      గుర్తులేని అక్షరం ముసుగు = 1 << (నిలువు & 7);
      ఒకవేళ(state->tabstops[నిలువు >> 3] & ముసుగు)
        newtabstops[నిలువు >> 3] |= ముసుగు;
      లేకపోతే
        newtabstops[నిలువు >> 3] &= ~ముసుగు;
      }

    కొరకు( ; నిలువు < నిలువుల_సంఖ్య; నిలువు++) {
      గుర్తులేని అక్షరం ముసుగు = 1 << (నిలువు & 7);
      ఒకవేళ(నిలువు % 8 == 0)
        newtabstops[నిలువు >> 3] |= ముసుగు;
      లేకపోతే
        newtabstops[నిలువు >> 3] &= ~ముసుగు;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->అడ్డువరుసలు_2 = అడ్డువరుసలు_2;
  state->నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;

  ఒకవేళ(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->అడ్డువరుసలు_2);
  ఒకవేళ(state->scrollregion_కుడి > -1)
    UBOUND(state->scrollregion_కుడి, state->నిలువుల_సంఖ్య);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_శిఖరం, state->అడ్డువరుసలు_2);
  UBOUND(state->scrollregion_ఎడమ, state->నిలువుల_సంఖ్య);
  ఒకవేళ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_శిఖరం) {
    state->scrollregion_శిఖరం = 0;
    state->scrollregion_bottom = -1;
  }
  ఒకవేళ(SCROLLREGION_కుడి(state) <= state->scrollregion_ఎడమ) {
    state->scrollregion_ఎడమ = 0;
    state->scrollregion_కుడి = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  ఒకవేళ(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(అడ్డువరుసలు_2, నిలువుల_సంఖ్య, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  లేకపోతే {
    ఒకవేళ(అడ్డువరుసలు_2 != state->అడ్డువరుసలు_2) {
      కొరకు(పూర్ణాంకం bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_పంక్తి_Info *oldlineinfo = state->lineinfos[bufidx];
        ఒకవేళ(!oldlineinfo)
          కొనసాగు;

        VTerm_పంక్తి_Info *newlineinfo = vterm_allocator_malloc(state->vt, అడ్డువరుసలు_2 * పరిమాణం(VTerm_పంక్తి_Info));

        పూర్ణాంకం అడ్డువరుస;
        కొరకు(అడ్డువరుస = 0; అడ్డువరుస < state->అడ్డువరుసలు_2 && అడ్డువరుస < అడ్డువరుసలు_2; అడ్డువరుస++) {
          newlineinfo[అడ్డువరుస] = oldlineinfo[అడ్డువరుస];
        }

        కొరకు( ; అడ్డువరుస < అడ్డువరుసలు_2; అడ్డువరుస++) {
          newlineinfo[అడ్డువరుస] = (VTerm_పంక్తి_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->బహుళకం.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  ఒకవేళ(state->at_phantom && state->pos.నిలువు < నిలువుల_సంఖ్య-1) {
    state->at_phantom = 0;
    state->pos.నిలువు++;
  }

  ఒకవేళ(state->pos.అడ్డువరుస < 0)
    state->pos.అడ్డువరుస = 0;
  ఒకవేళ(state->pos.అడ్డువరుస >= అడ్డువరుసలు_2)
    state->pos.అడ్డువరుస = అడ్డువరుసలు_2 - 1;
  ఒకవేళ(state->pos.నిలువు < 0)
    state->pos.నిలువు = 0;
  ఒకవేళ(state->pos.నిలువు >= నిలువుల_సంఖ్య)
    state->pos.నిలువు = నిలువుల_సంఖ్య - 1;

  updatecursor(state, &oldpos, 1);

  తిరిగి 1;
}

స్థిర స్థిరరాశి VTermParserCallbacks parser_callbacks = {
  on_పాఠ్యం, //పాఠ్యం
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
  ఒకవేళ(vt->state)
    తిరిగి vt->state;

  VTermState *state = vterm_state_new(vt);
  ఒకవేళ (state == ఖాళీ)
    తిరిగి ఖాళీ;
  vt->state = state;

  vterm_parser_సమితి_callbacks(vt, &parser_callbacks, state);

  తిరిగి state;
}

శూన్య vterm_state_reset(VTermState *state, పూర్ణాంకం hard)
{
  state->scrollregion_శిఖరం = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_ఎడమ = 0;
  state->scrollregion_కుడి = -1;

  state->బహుళకం.keypad          = 0;
  state->బహుళకం.cursor          = 0;
  state->బహుళకం.autowrap        = 1;
  state->బహుళకం.చొప్పించు          = 0;
  state->బహుళకం.newline         = 0;
  state->బహుళకం.alt_screen      = 0;
  state->బహుళకం.origin          = 0;
  state->బహుళకం.leftrightmargin = 0;
  state->బహుళకం.bracketpaste    = 0;
  state->బహుళకం.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->బహుళకం.ctrl8bit   = 0;

  కొరకు(పూర్ణాంకం నిలువు = 0; నిలువు < state->నిలువుల_సంఖ్య; నిలువు++)
    ఒకవేళ(నిలువు % 8 == 0)
      సమితి_నిలువు_tabstop(state, నిలువు);
    లేకపోతే
      clear_నిలువు_tabstop(state, నిలువు);

  కొరకు(పూర్ణాంకం అడ్డువరుస = 0; అడ్డువరుస < state->అడ్డువరుసలు_2; అడ్డువరుస++)
    సమితి_lineinfo(state, అడ్డువరుస, FORCE, DWL_OFF, DHL_OFF);

  ఒకవేళ(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->బహుళకం.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  కొరకు(పూర్ణాంకం i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    ఒకవేళ(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].దత్తాంశం);
  }

  state->gl_సమితి = 0;
  state->gr_సమితి = 1;
  state->gsingle_సమితి = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_ఖండం);

  ఒకవేళ(hard) {
    state->pos.అడ్డువరుస = 0;
    state->pos.నిలువు = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.చివర_అడ్డువరుస = state->అడ్డువరుసలు_2;
    rect.చివర_నిలువు =  state->నిలువుల_సంఖ్య;
    erase(state, rect, 0);
  }
}

శూన్య vterm_state_get_cursorpos(స్థిరరాశి VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

శూన్య vterm_state_get_mousestate(స్థిరరాశి VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.నిలువు = state->mouse_నిలువు;
  mousestate->pos.అడ్డువరుస = state->mouse_అడ్డువరుస;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

శూన్య vterm_state_సమితి_callbacks(VTermState *state, స్థిరరాశి VTermStateCallbacks *callbacks, శూన్య *user)
{
  ఒకవేళ(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    ఒకవేళ(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  లేకపోతే {
    state->callbacks = ఖాళీ;
    state->cbdata = ఖాళీ;
  }
}

శూన్య vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

శూన్య *vterm_state_get_cbdata(VTermState *state)
{
  తిరిగి state->cbdata;
}

శూన్య vterm_state_సమితి_unrecognised_fallbacks(VTermState *state, స్థిరరాశి VTermStateFallbacks *fallbacks, శూన్య *user)
{
  ఒకవేళ(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  లేకపోతే {
    state->fallbacks = ఖాళీ;
    state->fbdata = ఖాళీ;
  }
}

శూన్య *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  తిరిగి state->fbdata;
}

పూర్ణాంకం vterm_state_సమితి_termprop(VTermState *state, VTermProp prop, VTerm_విలువ *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  ఒకవేళ(state->callbacks && state->callbacks->settermprop)
    ఒకవేళ(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      తిరిగి 0;

  ఎంపిక(prop) {
  స్థితి VTERM_PROP_TITLE:
  స్థితి VTERM_PROP_ICONNAME:
  స్థితి VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    తిరిగి 1;
  స్థితి VTERM_PROP_CURSORVISIBLE:
    state->బహుళకం.cursor_visible = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_PROP_CURSORBLINK:
    state->బహుళకం.cursor_blink = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_PROP_CURSORSHAPE:
    state->బహుళకం.cursor_shape = val->సంఖ్య;
    తిరిగి 1;
  స్థితి VTERM_PROP_తిరగేయి:
    state->బహుళకం.screen = val->boolean;
    తిరిగి 1;
  స్థితి VTERM_PROP_ALTSCREEN:
    state->బహుళకం.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->బహుళకం.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    ఒకవేళ(state->బహుళకం.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.చివర_అడ్డువరుస = state->అడ్డువరుసలు_2;
      rect.చివర_నిలువు = state->నిలువుల_సంఖ్య;
      erase(state, rect, 0);
    }
    తిరిగి 1;
  స్థితి VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    ఒకవేళ(val->సంఖ్య)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    ఒకవేళ(val->సంఖ్య == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    ఒకవేళ(val->సంఖ్య == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    తిరిగి 1;
  స్థితి VTERM_PROP_FOCUSREPORT:
    state->బహుళకం.report_focus = val->boolean;
    తిరిగి 1;

  స్థితి VTERM_N_PROPS:
    తిరిగి 0;
  }

  తిరిగి 0;
}

శూన్య vterm_state_focus_లోపల(VTermState *state)
{
  ఒకవేళ(state->బహుళకం.report_focus)
    vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

శూన్య vterm_state_focus_బహిర్గతం(VTermState *state)
{
  ఒకవేళ(state->బహుళకం.report_focus)
    vterm_నెట్టు_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

స్థిరరాశి VTerm_పంక్తి_Info *vterm_state_get_lineinfo(స్థిరరాశి VTermState *state, పూర్ణాంకం అడ్డువరుస)
{
  తిరిగి state->lineinfo + అడ్డువరుస;
}

శూన్య vterm_state_సమితి_selection_callbacks(VTermState *state, స్థిరరాశి VTermSelectionCallbacks *callbacks, శూన్య *user,
    అక్షరం *బఫరు, size_t buflen)
{
  ఒకవేళ(buflen && !బఫరు)
    బఫరు = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.బఫరు    = బఫరు;
  state->selection.buflen    = buflen;
}

శూన్య vterm_state_send_selection(VTermState *state, VTerm_Selection_ముసుగు ముసుగు, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ఒకవేళ(frag.initial) {
    /* TODO: support sending more than one mask bit */
    స్థిర అక్షరం selection_chars[] = "cpqs";
    పూర్ణాంకం idx;
    కొరకు(idx = 0; idx < 4; idx++)
      ఒకవేళ(ముసుగు & (1 << idx))
        ఆపు;

    vterm_నెట్టు_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  ఒకవేళ(frag.len) {
    size_t bufcur = 0;
    అక్షరం *బఫరు = state->selection.బఫరు;

    uint32_t x = 0;
    పూర్ణాంకం n = 0;

    ఒకవేళ(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    అయితే((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      ఒకవేళ(n == 3) {
        బఫరు[0] = base64_one((x >> 18) & 0x3F);
        బఫరు[1] = base64_one((x >> 12) & 0x3F);
        బఫరు[2] = base64_one((x >>  6) & 0x3F);
        బఫరు[3] = base64_one((x >>  0) & 0x3F);

        బఫరు += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      ఒకవేళ(!frag.len || (state->selection.buflen - bufcur) < 4) {
        ఒకవేళ(bufcur)
          vterm_నెట్టు_output_బైట్లు(vt, state->selection.బఫరు, bufcur);

        బఫరు = state->selection.బఫరు;
        bufcur = 0;
      }
    }

    ఒకవేళ(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  ఒకవేళ(frag.final) {
    ఒకవేళ(state->tmp.selection.sendpartial) {
      పూర్ణాంకం n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      అక్షరం *బఫరు = state->selection.బఫరు;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      బఫరు[0] = base64_one((x >> 18) & 0x3F);
      బఫరు[1] = base64_one((x >> 12) & 0x3F);
      బఫరు[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      బఫరు[3] = '=';

      vterm_నెట్టు_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, బఫరు);
    }
    లేకపోతే
      vterm_నెట్టు_output_sprintf_str(vt, 0, TRUE, "");
  }
}
