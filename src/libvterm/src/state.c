#অন্তর্ভাব "vterm_internal.h"

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#যদি defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

স্থির পূর্ণসংখ্যা on_resize(পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols, শূন্য *user);

/* Some convenient wrappers to make callback functions easier */

স্থির শূন্য putglyph(VTermState *state, ধ্রুবক uint32_t chars[], পূর্ণসংখ্যা width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  যদি(state->callbacks && state->callbacks->putglyph)
    যদি((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      ফেরত;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

স্থির শূন্য updatecursor(VTermState *state, VTermPos *oldpos, পূর্ণসংখ্যা cancel_phantom)
{
  যদি(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    ফেরত;

  যদি(cancel_phantom)
    state->at_phantom = 0;

  যদি(state->callbacks && state->callbacks->movecursor)
    যদি((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      ফেরত;
}

স্থির শূন্য erase(VTermState *state, VTermRect rect, পূর্ণসংখ্যা selective)
{
  যদি(rect.end_col == state->cols) {
    পূর্ণসংখ্যা row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    জন্য(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  যদি(state->callbacks && state->callbacks->erase)
    যদি((*state->callbacks->erase)(rect, selective, state->cbdata))
      ফেরত;
}

স্থির VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, আকার(VTermState));

  যদি (state == ফাঁকা)
    ফেরত ফাঁকা;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ফাঁকা;
  state->cbdata    = ফাঁকা;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ফাঁকা;
  state->selection.user      = ফাঁকা;
  state->selection.বাফার    = ফাঁকা;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_আকার = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_আকার * আকার(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * আকার(VTerm_সারি_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * আকার(VTerm_সারি_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  যদি(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.তথ্য);

  ফেরত state;
}

INTERNAL শূন্য vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  যদি(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

স্থির শূন্য scroll(VTermState *state, VTermRect rect, পূর্ণসংখ্যা downward, পূর্ণসংখ্যা rightward)
{
  পূর্ণসংখ্যা rows;
  পূর্ণসংখ্যা cols;
  যদি(!downward && !rightward)
    ফেরত;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  যদি(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    ফেরত;

  rows = rect.end_row - rect.start_row;
  যদি(downward > rows)
    downward = rows;
  অন্যথা যদি(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  যদি(rightward > cols)
    rightward = cols;
  অন্যথা যদি(rightward < -cols)
    rightward = -cols;

  যদি(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    যদি(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    অন্যথা যদি(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    যদি(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    অন্যথা যদি(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  যদি(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    পূর্ণসংখ্যা height = rect.end_row - rect.start_row - abs(downward);
    পূর্ণসংখ্যা row;
    VTerm_সারি_Info zero_সারি_Info = {0x0};

    যদি(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * আকার(state->lineinfo[0]));
      জন্য(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_সারি_Info;
    }
    অন্যথা {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * আকার(state->lineinfo[0]));
      জন্য(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_সারি_Info;
    }
  }

  যদি(state->callbacks && state->callbacks->scrollrect)
    যদি((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      ফেরত;

  যদি(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

স্থির শূন্য linefeed(VTermState *state)
{
  যদি(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_শীর্ষ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_বাম(state);
    rect.end_col   = SCROLLREGION_ডান(state);

    scroll(state, rect, 1, 0);
  }
  অন্যথা যদি(state->pos.row < state->rows-1)
    state->pos.row++;
}

স্থির শূন্য grow_combine_বাফার(VTermState *state)
{
  size_t    new_আকার = state->combine_chars_আকার * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_আকার * আকার(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_আকার * আকার(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_আকার = new_আকার;
}

স্থির শূন্য set_col_tabstop(VTermState *state, পূর্ণসংখ্যা col)
{
  unsigned অক্ষর mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

স্থির শূন্য clear_col_tabstop(VTermState *state, পূর্ণসংখ্যা col)
{
  unsigned অক্ষর mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

স্থির পূর্ণসংখ্যা is_col_tabstop(VTermState *state, পূর্ণসংখ্যা col)
{
  unsigned অক্ষর mask = 1 << (col & 7);
  ফেরত state->tabstops[col >> 3] & mask;
}

স্থির পূর্ণসংখ্যা is_cursor_in_scrollregion(ধ্রুবক VTermState *state)
{
  যদি(state->pos.row < state->scrollregion_শীর্ষ ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    ফেরত 0;
  যদি(state->pos.col < SCROLLREGION_বাম(state) ||
     state->pos.col >= SCROLLREGION_ডান(state))
    ফেরত 0;

  ফেরত 1;
}

স্থির শূন্য tab(VTermState *state, পূর্ণসংখ্যা গুনতি, পূর্ণসংখ্যা direction)
{
  যখন(গুনতি > 0) {
    যদি(direction > 0) {
      যদি(state->pos.col >= THISROWWIDTH(state)-1)
        ফেরত;

      state->pos.col++;
    }
    অন্যথা যদি(direction < 0) {
      যদি(state->pos.col < 1)
        ফেরত;

      state->pos.col--;
    }

    যদি(is_col_tabstop(state, state->pos.col))
      গুনতি--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_শীর্ষ    1
#define DHL_BOTTOM 2

স্থির শূন্য set_lineinfo(VTermState *state, পূর্ণসংখ্যা row, পূর্ণসংখ্যা force, পূর্ণসংখ্যা dwl, পূর্ণসংখ্যা dhl)
{
  VTerm_সারি_Info info = state->lineinfo[row];

  যদি(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  অন্যথা যদি(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  যদি(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  অন্যথা যদি(dhl == DHL_শীর্ষ)
    info.doubleheight = DHL_শীর্ষ;
  অন্যথা যদি(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  যদি((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

স্থির পূর্ণসংখ্যা on_text(ধ্রুবক অক্ষর bytes[], size_t len, শূন্য *user)
{
  VTermState *state = user;
  পূর্ণসংখ্যা npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  পূর্ণসংখ্যা i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / আকার(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  যদি (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->তথ্য,
      codepoints, &npoints, state->gsingle_set ? 1 : (পূর্ণসংখ্যা)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  যদি(!npoints)
  {
    ফেরত (পূর্ণসংখ্যা)eaten;
  }

  যদি(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  যদি(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    যদি(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      পূর্ণসংখ্যা printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      জন্য(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      পূর্ণসংখ্যা saved_i = 0;
      যখন(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      যখন(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        যদি(saved_i >= (পূর্ণসংখ্যা)state->combine_chars_আকার)
          grow_combine_বাফার(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      যদি(saved_i >= (পূর্ণসংখ্যা)state->combine_chars_আকার)
        grow_combine_বাফার(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      জন্য(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    অন্যথা {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  জন্য(; i < npoints; i++) {
    // Try to find combining characters following this
    পূর্ণসংখ্যা glyph_starts = i;
    পূর্ণসংখ্যা glyph_ends;
    পূর্ণসংখ্যা width = 0;

    জন্য(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      যদি(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        বিরতি;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * আকার(uint32_t));
    যদি (chars == ফাঁকা)
      বিরতি;

    জন্য( ; i < glyph_ends; i++) {
      পূর্ণসংখ্যা this_width;
      যদি(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        যদি(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      যদি(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      যদি (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    যদি (width < 0)
      width = 0;

    যখন(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    পূর্ণসংখ্যা printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    জন্য(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    যদি(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    যদি(state->mode.ঢোকাও) {
      // TODO: This will be a little inefficient for large bodies of text, as
      // it'll have to 'ICH' effectively before every glyph. We should scan
      // ahead and ICH as many times as required
      VTermRect rect;
      rect.start_row = state->pos.row;
      rect.end_row   = state->pos.row + 1;
      rect.start_col = state->pos.col;
      rect.end_col   = THISROWWIDTH(state);
      scroll(state, rect, 0, -1);
    }

    putglyph(state, chars, width, state->pos);

    যদি(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      পূর্ণসংখ্যা save_i;
      জন্য(save_i = 0; chars[save_i]; save_i++) {
        যদি(save_i >= (পূর্ণসংখ্যা)state->combine_chars_আকার)
          grow_combine_বাফার(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      যদি(save_i >= (পূর্ণসংখ্যা)state->combine_chars_আকার)
        grow_combine_বাফার(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    যদি(state->pos.col + width >= THISROWWIDTH(state)) {
      যদি(state->mode.autowrap)
        state->at_phantom = 1;
    }
    অন্যথা {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  যদি(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  ফেরত (পূর্ণসংখ্যা)eaten;
}

স্থির পূর্ণসংখ্যা on_control(unsigned অক্ষর control, শূন্য *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  নির্বাচন(control) {
  ক্ষেত্র 0x07: // BEL - ECMA-48 8.3.3
    যদি(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    বিরতি;

  ক্ষেত্র 0x08: // BS - ECMA-48 8.3.5
    যদি(state->pos.col > 0)
      state->pos.col--;
    যদি(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      যদি(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    বিরতি;

  ক্ষেত্র 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    বিরতি;

  ক্ষেত্র 0x0a: // LF - ECMA-48 8.3.74
  ক্ষেত্র 0x0b: // VT
  ক্ষেত্র 0x0c: // FF
    linefeed(state);
    যদি(state->mode.newline)
      state->pos.col = 0;
    বিরতি;

  ক্ষেত্র 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    বিরতি;

  ক্ষেত্র 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    বিরতি;

  ক্ষেত্র 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    বিরতি;

  ক্ষেত্র 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    বিরতি;

  ক্ষেত্র 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    বিরতি;

  ক্ষেত্র 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    বিরতি;

  ক্ষেত্র 0x8d: // RI - ECMA-48 8.3.104
    যদি(state->pos.row == state->scrollregion_শীর্ষ) {
      VTermRect rect;
      rect.start_row = state->scrollregion_শীর্ষ;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_বাম(state);
      rect.end_col   = SCROLLREGION_ডান(state);

      scroll(state, rect, -1, 0);
    }
    অন্যথা যদি(state->pos.row > 0)
        state->pos.row--;
    বিরতি;

  ক্ষেত্র 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    বিরতি;

  ক্ষেত্র 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    বিরতি;

  পূর্বনির্ধারিত:
    যদি(state->fallbacks && state->fallbacks->control)
      যদি((*state->fallbacks->control)(control, state->fbdata))
        ফেরত 1;

    ফেরত 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  যদি(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  ফেরত 1;
}

স্থির পূর্ণসংখ্যা settermprop_bool(VTermState *state, VTermProp prop, পূর্ণসংখ্যা v)
{
  VTerm_মান val;
  val.boolean = v;
  ফেরত vterm_state_set_termprop(state, prop, &val);
}

স্থির পূর্ণসংখ্যা settermprop_int(VTermState *state, VTermProp prop, পূর্ণসংখ্যা v)
{
  VTerm_মান val;
  val.সংখ্যা = v;
  ফেরত vterm_state_set_termprop(state, prop, &val);
}

স্থির পূর্ণসংখ্যা settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_মান val;

  val.string = frag;
  ফেরত vterm_state_set_termprop(state, prop, &val);
}

স্থির শূন্য savecursor(VTermState *state, পূর্ণসংখ্যা save)
{
  যদি(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  অন্যথা {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

স্থির পূর্ণসংখ্যা on_escape(ধ্রুবক অক্ষর *bytes, size_t len, শূন্য *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  নির্বাচন(bytes[0]) {
  ক্ষেত্র ' ':
    যদি(len != 2)
      ফেরত 0;

    নির্বাচন(bytes[1]) {
      ক্ষেত্র 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        বিরতি;

      ক্ষেত্র 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        বিরতি;

      পূর্বনির্ধারিত:
        ফেরত 0;
    }
    ফেরত 2;

  ক্ষেত্র '#':
    যদি(len != 2)
      ফেরত 0;

    নির্বাচন(bytes[1]) {
      ক্ষেত্র '3': // DECDHL top
        যদি(state->mode.leftrightmargin)
          বিরতি;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_শীর্ষ);
        বিরতি;

      ক্ষেত্র '4': // DECDHL bottom
        যদি(state->mode.leftrightmargin)
          বিরতি;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        বিরতি;

      ক্ষেত্র '5': // DECSWL
        যদি(state->mode.leftrightmargin)
          বিরতি;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        বিরতি;

      ক্ষেত্র '6': // DECDWL
        যদি(state->mode.leftrightmargin)
          বিরতি;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        বিরতি;

      ক্ষেত্র '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        জন্য(pos.row = 0; pos.row < state->rows; pos.row++)
          জন্য(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        বিরতি;
      }

      পূর্বনির্ধারিত:
        ফেরত 0;
    }
    ফেরত 2;

  ক্ষেত্র '(': ক্ষেত্র ')': ক্ষেত্র '*': ক্ষেত্র '+': // SCS
    যদি(len != 2)
      ফেরত 0;

    {
      পূর্ণসংখ্যা setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      যদি(newenc) {
        state->encoding[setnum].enc = newenc;

        যদি(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].তথ্য);
      }
    }

    ফেরত 2;

  ক্ষেত্র '7': // DECSC
    savecursor(state, 1);
    ফেরত 1;

  ক্ষেত্র '8': // DECRC
    savecursor(state, 0);
    ফেরত 1;

  ক্ষেত্র '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    ফেরত 1;

  ক্ষেত্র '=': // DECKPAM
    state->mode.keypad = 1;
    ফেরত 1;

  ক্ষেত্র '>': // DECKPNM
    state->mode.keypad = 0;
    ফেরত 1;

  ক্ষেত্র 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    যদি(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    ফেরত 1;
  }

  ক্ষেত্র 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    ফেরত 1;

  ক্ষেত্র 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    ফেরত 1;

  ক্ষেত্র '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    ফেরত 1;

  ক্ষেত্র '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    ফেরত 1;

  ক্ষেত্র '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    ফেরত 1;

  পূর্বনির্ধারিত:
    ফেরত 0;
  }
}

স্থির শূন্য set_mode(VTermState *state, পূর্ণসংখ্যা অঙ্ক, পূর্ণসংখ্যা val)
{
  নির্বাচন(অঙ্ক) {
  ক্ষেত্র 4: // IRM - ECMA-48 7.2.10
    state->mode.ঢোকাও = val;
    বিরতি;

  ক্ষেত্র 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    বিরতি;

  পূর্বনির্ধারিত:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", অঙ্ক);
    ফেরত;
  }
}

স্থির শূন্য set_dec_mode(VTermState *state, পূর্ণসংখ্যা অঙ্ক, পূর্ণসংখ্যা val)
{
  নির্বাচন(অঙ্ক) {
  ক্ষেত্র 1:
    state->mode.cursor = val;
    বিরতি;

  ক্ষেত্র 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    বিরতি;

  ক্ষেত্র 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_শীর্ষ : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_বাম(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    বিরতি;

  ক্ষেত্র 7:
    state->mode.autowrap = val;
    বিরতি;

  ক্ষেত্র 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    বিরতি;

  ক্ষেত্র 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    বিরতি;

  ক্ষেত্র 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    যদি(val) {
      পূর্ণসংখ্যা row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      জন্য(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    বিরতি;

  ক্ষেত্র 1000:
  ক্ষেত্র 1002:
  ক্ষেত্র 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (অঙ্ক == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (অঙ্ক == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    বিরতি;

  ক্ষেত্র 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    বিরতি;

  ক্ষেত্র 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    বিরতি;

  ক্ষেত্র 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    বিরতি;

  ক্ষেত্র 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    বিরতি;

  ক্ষেত্র 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    বিরতি;

  ক্ষেত্র 1048:
    savecursor(state, val);
    বিরতি;

  ক্ষেত্র 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    বিরতি;

  ক্ষেত্র 2004:
    state->mode.bracketpaste = val;
    বিরতি;

  পূর্বনির্ধারিত:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", অঙ্ক);
    ফেরত;
  }
}

স্থির শূন্য request_dec_mode(VTermState *state, পূর্ণসংখ্যা অঙ্ক)
{
  পূর্ণসংখ্যা reply;

  নির্বাচন(অঙ্ক) {
    ক্ষেত্র 1:
      reply = state->mode.cursor;
      বিরতি;

    ক্ষেত্র 5:
      reply = state->mode.screen;
      বিরতি;

    ক্ষেত্র 6:
      reply = state->mode.origin;
      বিরতি;

    ক্ষেত্র 7:
      reply = state->mode.autowrap;
      বিরতি;

    ক্ষেত্র 12:
      reply = state->mode.cursor_blink;
      বিরতি;

    ক্ষেত্র 25:
      reply = state->mode.cursor_visible;
      বিরতি;

    ক্ষেত্র 69:
      reply = state->mode.leftrightmargin;
      বিরতি;

    ক্ষেত্র 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      বিরতি;

    ক্ষেত্র 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      বিরতি;

    ক্ষেত্র 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      বিরতি;

    ক্ষেত্র 1004:
      reply = state->mode.report_focus;
      বিরতি;

    ক্ষেত্র 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      বিরতি;

    ক্ষেত্র 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      বিরতি;

    ক্ষেত্র 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      বিরতি;

    ক্ষেত্র 1047:
      reply = state->mode.alt_screen;
      বিরতি;

    ক্ষেত্র 2004:
      reply = state->mode.bracketpaste;
      বিরতি;

    পূর্বনির্ধারিত:
      vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", অঙ্ক, 0);
      ফেরত;
  }

  vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", অঙ্ক, reply ? 1 : 2);
}

স্থির শূন্য request_version_string(VTermState *state)
{
  vterm_ঠেলো_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

স্থির পূর্ণসংখ্যা on_csi(ধ্রুবক অক্ষর *leader, ধ্রুবক long args[], পূর্ণসংখ্যা argcount, ধ্রুবক অক্ষর *intermed, অক্ষর command, শূন্য *user)
{
  VTermState *state = user;
  পূর্ণসংখ্যা leader_byte = 0;
  পূর্ণসংখ্যা intermed_byte = 0;
  পূর্ণসংখ্যা cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  পূর্ণসংখ্যা handled = 1;

  // Some temporaries for later code
  পূর্ণসংখ্যা গুনতি, val;
  পূর্ণসংখ্যা row, col;
  VTermRect rect;
  পূর্ণসংখ্যা selective;

  যদি(leader && leader[0]) {
    যদি(leader[1]) // longer than 1 char
      ফেরত 0;

    নির্বাচন(leader[0]) {
    ক্ষেত্র '?':
    ক্ষেত্র '>':
    ক্ষেত্র '<':
      leader_byte = leader[0];
      বিরতি;
    পূর্বনির্ধারিত:
      ফেরত 0;
    }
  }

  যদি(intermed && intermed[0]) {
    যদি(intermed[1]) // longer than 1 char
      ফেরত 0;

    নির্বাচন(intermed[0]) {
    ক্ষেত্র ' ':
    ক্ষেত্র '!':
    ক্ষেত্র '"':
    ক্ষেত্র '$':
    ক্ষেত্র '\'':
      intermed_byte = intermed[0];
      বিরতি;
    পূর্বনির্ধারিত:
      ফেরত 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) যদি((v) < (min)) (v) = (min)
#define UBOUND(v,max) যদি((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  নির্বাচন(intermed_byte << 16 | leader_byte << 8 | command) {
  ক্ষেত্র 0x40: // ICH - ECMA-48 8.3.64
    গুনতি = CSI_ARG_গুনতি(args[0]);

    যদি(!is_cursor_in_scrollregion(state))
      বিরতি;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    যদি(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_ডান(state);
    অন্যথা
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -গুনতি);

    বিরতি;

  ক্ষেত্র 0x41: // CUU - ECMA-48 8.3.22
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.row -= গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x42: // CUD - ECMA-48 8.3.19
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.row += গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x43: // CUF - ECMA-48 8.3.20
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.col += গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x44: // CUB - ECMA-48 8.3.18
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.col -= গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x45: // CNL - ECMA-48 8.3.12
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.col = 0;
    state->pos.row += গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x46: // CPL - ECMA-48 8.3.13
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.col = 0;
    state->pos.row -= গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    যদি(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      যদি(state->pos.row == row - 1) {
        পূর্ণসংখ্যা cnt, ptr = 0;
        জন্য(cnt = 0; cnt < col - 1; ++cnt) {
	  VTermPos p;
	  VTermScreenCell c0, c1;
	  p.row = row - 1;
	  p.col = ptr;
	  vterm_screen_get_cell(state->vt->screen, p, &c0);
	  p.col++;
	  vterm_screen_get_cell(state->vt->screen, p, &c1);
	  ptr += (c1.chars[0] == (uint32_t)-1)		    // double cell?
	     ? (vterm_unicode_is_ambiguous(c0.chars[0]))    // is ambiguous?
	     ? vterm_unicode_width(0x00a1) : 1		    // &ambiwidth
	     : 1;					    // not ambiguous
        }
        col = ptr + 1;
      }
    }
    state->pos.row = row-1;
    state->pos.col = col-1;
    যদি(state->mode.origin) {
      state->pos.row += state->scrollregion_শীর্ষ;
      state->pos.col += SCROLLREGION_বাম(state);
    }
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x49: // CHT - ECMA-48 8.3.10
    গুনতি = CSI_ARG_গুনতি(args[0]);
    tab(state, গুনতি, +1);
    বিরতি;

  ক্ষেত্র 0x4a: // ED - ECMA-48 8.3.39
  ক্ষেত্র LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    নির্বাচন(CSI_ARG(args[0])) {
    ক্ষেত্র CSI_ARG_MISSING:
    ক্ষেত্র 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      যদি(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      জন্য(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      যদি(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      বিরতি;

    ক্ষেত্র 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      জন্য(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      যদি(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      যদি(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      বিরতি;

    ক্ষেত্র 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      জন্য(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      বিরতি;

    ক্ষেত্র 3:
      যদি(state->callbacks && state->callbacks->sb_clear)
        যদি((*state->callbacks->sb_clear)(state->cbdata))
          ফেরত 1;
      বিরতি;
    }
    বিরতি;

  ক্ষেত্র 0x4b: // EL - ECMA-48 8.3.41
  ক্ষেত্র LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    নির্বাচন(CSI_ARG(args[0])) {
    ক্ষেত্র CSI_ARG_MISSING:
    ক্ষেত্র 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); বিরতি;
    ক্ষেত্র 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; বিরতি;
    ক্ষেত্র 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); বিরতি;
    পূর্বনির্ধারিত:
      ফেরত 0;
    }

    যদি(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    বিরতি;

  ক্ষেত্র 0x4c: // IL - ECMA-48 8.3.67
    গুনতি = CSI_ARG_গুনতি(args[0]);

    যদি(!is_cursor_in_scrollregion(state))
      বিরতি;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_বাম(state);
    rect.end_col   = SCROLLREGION_ডান(state);

    scroll(state, rect, -গুনতি, 0);

    বিরতি;

  ক্ষেত্র 0x4d: // DL - ECMA-48 8.3.32
    গুনতি = CSI_ARG_গুনতি(args[0]);

    যদি(!is_cursor_in_scrollregion(state))
      বিরতি;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_বাম(state);
    rect.end_col   = SCROLLREGION_ডান(state);

    scroll(state, rect, গুনতি, 0);

    বিরতি;

  ক্ষেত্র 0x50: // DCH - ECMA-48 8.3.26
    গুনতি = CSI_ARG_গুনতি(args[0]);

    যদি(!is_cursor_in_scrollregion(state))
      বিরতি;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    যদি(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_ডান(state);
    অন্যথা
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, গুনতি);

    বিরতি;

  ক্ষেত্র 0x53: // SU - ECMA-48 8.3.147
    গুনতি = CSI_ARG_গুনতি(args[0]);

    rect.start_row = state->scrollregion_শীর্ষ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_বাম(state);
    rect.end_col   = SCROLLREGION_ডান(state);

    scroll(state, rect, গুনতি, 0);

    বিরতি;

  ক্ষেত্র 0x54: // SD - ECMA-48 8.3.113
    গুনতি = CSI_ARG_গুনতি(args[0]);

    rect.start_row = state->scrollregion_শীর্ষ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_বাম(state);
    rect.end_col   = SCROLLREGION_ডান(state);

    scroll(state, rect, -গুনতি, 0);

    বিরতি;

  ক্ষেত্র 0x58: // ECH - ECMA-48 8.3.38
    গুনতি = CSI_ARG_গুনতি(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + গুনতি;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    বিরতি;

  ক্ষেত্র 0x5a: // CBT - ECMA-48 8.3.7
    গুনতি = CSI_ARG_গুনতি(args[0]);
    tab(state, গুনতি, -1);
    বিরতি;

  ক্ষেত্র 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x61: // HPR - ECMA-48 8.3.59
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.col += গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x62: { // REP - ECMA-48 8.3.103
    ধ্রুবক পূর্ণসংখ্যা row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    যদি(state->combine_width < 1)
      বিরতি;

    গুনতি = CSI_ARG_গুনতি(args[0]);
    col = state->pos.col + গুনতি;
    UBOUND(col, row_width);
    যখন (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    যদি (state->pos.col + state->combine_width >= row_width) {
      যদি (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    বিরতি;
  }

  ক্ষেত্র 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    যদি(val == 0)
      // DEC VT100 response
      vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    বিরতি;

  ক্ষেত্র LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    বিরতি;

  ক্ষেত্র 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    যদি(state->mode.origin)
      state->pos.row += state->scrollregion_শীর্ষ;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x65: // VPR - ECMA-48 8.3.160
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.row += গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    যদি(state->mode.origin) {
      state->pos.row += state->scrollregion_শীর্ষ;
      state->pos.col += SCROLLREGION_বাম(state);
    }
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    নির্বাচন(val) {
    ক্ষেত্র 0:
      clear_col_tabstop(state, state->pos.col);
      বিরতি;
    ক্ষেত্র 3:
    ক্ষেত্র 5:
      জন্য(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      বিরতি;
    ক্ষেত্র 1:
    ক্ষেত্র 2:
    ক্ষেত্র 4:
      বিরতি;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    পূর্বনির্ধারিত:
      ফেরত 0;
    }
    বিরতি;

  ক্ষেত্র 0x68: // SM - ECMA-48 8.3.125
    যদি(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    বিরতি;

  ক্ষেত্র LEADER('?', 0x68): // DEC private mode set
    জন্য(পূর্ণসংখ্যা i = 0; i < argcount; i++) {
      যদি(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    বিরতি;

  ক্ষেত্র 0x6a: // HPB - ECMA-48 8.3.58
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.col -= গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x6b: // VPB - ECMA-48 8.3.159
    গুনতি = CSI_ARG_গুনতি(args[0]);
    state->pos.row -= গুনতি;
    state->at_phantom = 0;
    বিরতি;

  ক্ষেত্র 0x6c: // RM - ECMA-48 8.3.106
    যদি(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    বিরতি;

  ক্ষেত্র LEADER('?', 0x6c): // DEC private mode reset
    জন্য(পূর্ণসংখ্যা i = 0; i < argcount; i++) {
      যদি(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    বিরতি;

  ক্ষেত্র 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    বিরতি;

  ক্ষেত্র LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    যদি (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_চাবিগুলি ? 2 : 0);
      বিরতি;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    জন্য(পূর্ণসংখ্যা argi = 0; argi < argcount; argi++) {
      long arg;
      নির্বাচন(arg = CSI_ARG(args[argi])) {
        ক্ষেত্র 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          বিরতি;
        ক্ষেত্র 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          বিরতি;
        ক্ষেত্র 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          বিরতি;
      }
    }
    বিরতি;

  ক্ষেত্র LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    যদি (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_চাবিগুলি = CSI_ARG(args[1]) == 2;
    }
    বিরতি;

  ক্ষেত্র LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    যদি (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_চাবিগুলি = 0;

      state->mode.kitty_keyboard = 1;
    }
    বিরতি;

  ক্ষেত্র LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    যদি (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    বিরতি;

  ক্ষেত্র LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    যদি (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    বিরতি;

  ক্ষেত্র 0x6e: // DSR - ECMA-48 8.3.35
  ক্ষেত্র LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      অক্ষর *qmark = (leader_byte == '?') ? "?" : "";

      নির্বাচন(val) {
      ক্ষেত্র 0: ক্ষেত্র 1: ক্ষেত্র 2: ক্ষেত্র 3: ক্ষেত্র 4:
        // ignore - these are replies
        বিরতি;
      ক্ষেত্র 5:
        vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        বিরতি;
      ক্ষেত্র 6: // CPR - cursor position report
        vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        বিরতি;
      }
    }
    বিরতি;


  ক্ষেত্র INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    বিরতি;

  ক্ষেত্র LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    বিরতি;

  ক্ষেত্র LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    বিরতি;

  ক্ষেত্র INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    নির্বাচন(val) {
    ক্ষেত্র 0: ক্ষেত্র 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      বিরতি;
    ক্ষেত্র 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      বিরতি;
    ক্ষেত্র 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      বিরতি;
    ক্ষেত্র 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      বিরতি;
    ক্ষেত্র 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_বাম);
      বিরতি;
    ক্ষেত্র 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_বাম);
      বিরতি;
    }

    বিরতি;

  ক্ষেত্র INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    নির্বাচন(val) {
    ক্ষেত্র 0: ক্ষেত্র 2:
      state->protected_cell = 0;
      বিরতি;
    ক্ষেত্র 1:
      state->protected_cell = 1;
      বিরতি;
    }

    বিরতি;

  ক্ষেত্র 0x72: // DECSTBM - DEC custom
    state->scrollregion_শীর্ষ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_শীর্ষ, 0);
    UBOUND(state->scrollregion_শীর্ষ, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    যদি(state->scrollregion_শীর্ষ == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    অন্যথা
      UBOUND(state->scrollregion_bottom, state->rows);

    যদি(SCROLLREGION_BOTTOM(state) <= state->scrollregion_শীর্ষ) {
      // Invalid
      state->scrollregion_শীর্ষ    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    যদি(state->mode.origin) {
      state->pos.row += state->scrollregion_শীর্ষ;
      state->pos.col += SCROLLREGION_বাম(state);
    }

    বিরতি;

  ক্ষেত্র 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_বাম = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_ডান = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_বাম, 0);
    UBOUND(state->scrollregion_বাম, state->cols);
    LBOUND(state->scrollregion_ডান, -1);
    যদি(state->scrollregion_বাম == 0 && state->scrollregion_ডান == state->cols)
      state->scrollregion_ডান = -1;
    অন্যথা
      UBOUND(state->scrollregion_ডান, state->cols);

    যদি(state->scrollregion_ডান > -1 &&
       state->scrollregion_ডান <= state->scrollregion_বাম) {
      // Invalid
      state->scrollregion_বাম  = 0;
      state->scrollregion_ডান = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    যদি(state->mode.origin) {
      state->pos.row += state->scrollregion_শীর্ষ;
      state->pos.col += SCROLLREGION_বাম(state);
    }

    বিরতি;

  ক্ষেত্র 0x74:
    নির্বাচন(CSI_ARG(args[0])) {
      ক্ষেত্র 8: // CSI 8 ; rows ; cols t  set size
        যদি (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	বিরতি;
      পূর্বনির্ধারিত:
	handled = 0;
	বিরতি;
    }
    বিরতি;

  ক্ষেত্র INTERMED('\'', 0x7D): // DECIC
    গুনতি = CSI_ARG_গুনতি(args[0]);

    যদি(!is_cursor_in_scrollregion(state))
      বিরতি;

    rect.start_row = state->scrollregion_শীর্ষ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_ডান(state);

    scroll(state, rect, 0, -গুনতি);

    বিরতি;

  ক্ষেত্র INTERMED('\'', 0x7E): // DECDC
    গুনতি = CSI_ARG_গুনতি(args[0]);

    যদি(!is_cursor_in_scrollregion(state))
      বিরতি;

    rect.start_row = state->scrollregion_শীর্ষ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_ডান(state);

    scroll(state, rect, 0, গুনতি);

    বিরতি;

  পূর্বনির্ধারিত:
    handled = 0;
    বিরতি;
  }

  যদি (!handled) {
    যদি(state->fallbacks && state->fallbacks->csi)
      যদি((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        ফেরত 1;

    ফেরত 0;
  }

  যদি(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_শীর্ষ);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_বাম(state));
    UBOUND(state->pos.col, SCROLLREGION_ডান(state)-1);
  }
  অন্যথা {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  যদি(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  যদি(SCROLLREGION_BOTTOM(state) <= state->scrollregion_শীর্ষ) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_শীর্ষ);
    abort();
  }

  যদি(SCROLLREGION_ডান(state) <= SCROLLREGION_বাম(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_ডান(state), SCROLLREGION_বাম(state));
    abort();
  }
#endif

  ফেরত 1;
}

স্থির অক্ষর base64_one(uint8_t b)
{
  যদি(b < 26)
    ফেরত 'A' + b;
  অন্যথা যদি(b < 52)
    ফেরত 'a' + b - 26;
  অন্যথা যদি(b < 62)
    ফেরত '0' + b - 52;
  অন্যথা যদি(b == 62)
    ফেরত '+';
  অন্যথা যদি(b == 63)
    ফেরত '/';
  ফেরত 0;
}

স্থির uint8_t unbase64one(অক্ষর c)
{
  যদি(c >= 'A' && c <= 'Z')
    ফেরত c - 'A';
  অন্যথা যদি(c >= 'a' && c <= 'z')
    ফেরত c - 'a' + 26;
  অন্যথা যদি(c >= '0' && c <= '9')
    ফেরত c - '0' + 52;
  অন্যথা যদি(c == '+')
    ফেরত 62;
  অন্যথা যদি(c == '/')
    ফেরত 63;

  ফেরত 0xFF;
}

স্থির শূন্য osc_selection(VTermState *state, VTermStringFragment frag)
{
  যদি(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  যখন(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    নির্বাচন(frag.str[0]) {
      ক্ষেত্র 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        বিরতি;
      ক্ষেত্র 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        বিরতি;
      ক্ষেত্র 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        বিরতি;
      ক্ষেত্র 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        বিরতি;
      ক্ষেত্র '0':
      ক্ষেত্র '1':
      ক্ষেত্র '2':
      ক্ষেত্র '3':
      ক্ষেত্র '4':
      ক্ষেত্র '5':
      ক্ষেত্র '6':
      ক্ষেত্র '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        বিরতি;

      ক্ষেত্র ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        যদি(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        বিরতি;
    }

    frag.str++;
    frag.len--;
  }

  যদি(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    যদি(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = ফাঁকা,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    ফেরত;
  }

  যদি(state->tmp.selection.state == SELECTION_SELECTED) {
    যদি(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    অন্যথা {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  যদি(state->tmp.selection.state == SELECTION_QUERY) {
    যদি(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    ফেরত;
  }

  যদি(state->tmp.selection.state == SELECTION_INVALID)
    ফেরত;

  যদি(state->selection.callbacks->set) {
    size_t bufcur = 0;
    অক্ষর *বাফার = state->selection.বাফার;

    uint32_t x = 0; /* Current decoding value */
    পূর্ণসংখ্যা n = 0;      /* Number of sextets consumed */

    যদি(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    যখন((state->selection.buflen - bufcur) >= 3 && frag.len) {
      যদি(frag.str[0] == '=') {
        যদি(n == 2) {
          বাফার[0] = (x >> 4) & 0xFF;
          বাফার += 1, bufcur += 1;
        }
        যদি(n == 3) {
          বাফার[0] = (x >> 10) & 0xFF;
          বাফার[1] = (x >>  2) & 0xFF;
          বাফার += 2, bufcur += 2;
        }

        যখন(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      অন্যথা {
        uint8_t b = unbase64one(frag.str[0]);
        যদি(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          যদি(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = ফাঁকা,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          বিরতি;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        যদি(n == 4) {
          বাফার[0] = (x >> 16) & 0xFF;
          বাফার[1] = (x >>  8) & 0xFF;
          বাফার[2] = (x >>  0) & 0xFF;

          বাফার += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      যদি(!frag.len || (state->selection.buflen - bufcur) < 3) {
        যদি(bufcur) {
	  VTermStringFragment setfrag = {
	    state->selection.বাফার, // str
	    bufcur, // len
	    state->tmp.selection.state == SELECTION_SET_INITIAL, // initial
	    frag.final && !frag.len // final
	  };
          (*state->selection.callbacks->set)(state->tmp.selection.mask,
	      setfrag, state->selection.user);
          state->tmp.selection.state = SELECTION_SET;
        }

        বাফার = state->selection.বাফার;
        bufcur = 0;
      }
    }

    যদি(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

স্থির পূর্ণসংখ্যা on_osc(পূর্ণসংখ্যা command, VTermStringFragment frag, শূন্য *user)
{
  VTermState *state = user;

  নির্বাচন(command) {
    ক্ষেত্র 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ফেরত 1;

    ক্ষেত্র 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      ফেরত 1;

    ক্ষেত্র 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ফেরত 1;

    ক্ষেত্র 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        পূর্ণসংখ্যা red = state->default_fg.red;
        পূর্ণসংখ্যা blue = state->default_fg.blue;
        পূর্ণসংখ্যা green = state->default_fg.green;
        vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        ফেরত 1;
      }

    ক্ষেত্র 11:
      {
	// request background color: <Esc>]11;?<0x07>
	পূর্ণসংখ্যা red = state->default_bg.red;
	পূর্ণসংখ্যা blue = state->default_bg.blue;
	পূর্ণসংখ্যা green = state->default_bg.green;
	vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	ফেরত 1;
      }
    ক্ষেত্র 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      ফেরত 1;

    ক্ষেত্র 52:
      যদি(state->selection.callbacks)
        osc_selection(state, frag);

      ফেরত 1;

    পূর্বনির্ধারিত:
      যদি(state->fallbacks && state->fallbacks->osc)
        যদি((*state->fallbacks->osc)(command, frag, state->fbdata))
          ফেরত 1;
  }

  ফেরত 0;
}

স্থির শূন্য request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  অক্ষর *tmp = state->tmp.decrqss;

  যদি(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  যখন(i < আকার(state->tmp.decrqss)-1 && tmp[i])
    i++;
  যখন(i < আকার(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  যদি(!frag.final)
    ফেরত;

  নির্বাচন(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    ক্ষেত্র 'm': {
      // Query SGR
      long args[20];
      পূর্ণসংখ্যা argc = vterm_state_getpen(state, args, আকার(args)/আকার(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      যদি(cur >= vt->tmpbuffer_len)
        ফেরত;

      জন্য(পূর্ণসংখ্যা argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        যদি(cur >= vt->tmpbuffer_len)
          ফেরত;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      যদি(cur >= vt->tmpbuffer_len)
        ফেরত;

      vterm_ঠেলো_output_bytes(vt, vt->tmpbuffer, cur);
      ফেরত;
    }

    ক্ষেত্র 'r':
      // Query DECSTBM
      vterm_ঠেলো_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_শীর্ষ+1, SCROLLREGION_BOTTOM(state));
      ফেরত;

    ক্ষেত্র 's':
      // Query DECSLRM
      vterm_ঠেলো_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_বাম(state)+1, SCROLLREGION_ডান(state));
      ফেরত;

    ক্ষেত্র ' '|('q'<<8): {
      // Query DECSCUSR
      পূর্ণসংখ্যা reply;
      নির্বাচন(state->mode.cursor_shape) {
        ক্ষেত্র VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; বিরতি;
        ক্ষেত্র VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; বিরতি;
	পূর্বনির্ধারিত: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; বিরতি;
      }
      যদি(state->mode.cursor_blink)
        reply--;
      vterm_ঠেলো_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      ফেরত;
    }

    ক্ষেত্র '\"'|('q'<<8):
      // Query DECSCA
      vterm_ঠেলো_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      ফেরত;
  }

  vterm_ঠেলো_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

স্থির পূর্ণসংখ্যা on_dcs(ধ্রুবক অক্ষর *command, size_t commandlen, VTermStringFragment frag, শূন্য *user)
{
  VTermState *state = user;

  যদি(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    ফেরত 1;
  }
  অন্যথা যদি(state->fallbacks && state->fallbacks->dcs)
    যদি((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      ফেরত 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (পূর্ণসংখ্যা)commandlen, command);
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা on_apc(VTermStringFragment frag, শূন্য *user)
{
  VTermState *state = user;

  যদি(state->fallbacks && state->fallbacks->apc)
    যদি((*state->fallbacks->apc)(frag, state->fbdata))
      ফেরত 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা on_pm(VTermStringFragment frag, শূন্য *user)
{
  VTermState *state = user;

  যদি(state->fallbacks && state->fallbacks->pm)
    যদি((*state->fallbacks->pm)(frag, state->fbdata))
      ফেরত 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা on_sos(VTermStringFragment frag, শূন্য *user)
{
  VTermState *state = user;

  যদি(state->fallbacks && state->fallbacks->sos)
    যদি((*state->fallbacks->sos)(frag, state->fbdata))
      ফেরত 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  ফেরত 0;
}

স্থির পূর্ণসংখ্যা on_resize(পূর্ণসংখ্যা rows, পূর্ণসংখ্যা cols, শূন্য *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  যদি (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  যদি (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  যদি(cols != state->cols) {
    unsigned অক্ষর *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    যদি (newtabstops == ফাঁকা)
      ফেরত 0;

    /* TODO: This can all be done much more efficiently bytewise */
    পূর্ণসংখ্যা col;
    জন্য(col = 0; col < state->cols && col < cols; col++) {
      unsigned অক্ষর mask = 1 << (col & 7);
      যদি(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      অন্যথা
        newtabstops[col >> 3] &= ~mask;
      }

    জন্য( ; col < cols; col++) {
      unsigned অক্ষর mask = 1 << (col & 7);
      যদি(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      অন্যথা
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  যদি(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  যদি(state->scrollregion_ডান > -1)
    UBOUND(state->scrollregion_ডান, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_শীর্ষ, state->rows);
  UBOUND(state->scrollregion_বাম, state->cols);
  যদি(SCROLLREGION_BOTTOM(state) <= state->scrollregion_শীর্ষ) {
    state->scrollregion_শীর্ষ = 0;
    state->scrollregion_bottom = -1;
  }
  যদি(SCROLLREGION_ডান(state) <= state->scrollregion_বাম) {
    state->scrollregion_বাম = 0;
    state->scrollregion_ডান = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  যদি(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  অন্যথা {
    যদি(rows != state->rows) {
      জন্য(পূর্ণসংখ্যা bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_সারি_Info *oldlineinfo = state->lineinfos[bufidx];
        যদি(!oldlineinfo)
          চালু;

        VTerm_সারি_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * আকার(VTerm_সারি_Info));

        পূর্ণসংখ্যা row;
        জন্য(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        জন্য( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_সারি_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  যদি(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  যদি(state->pos.row < 0)
    state->pos.row = 0;
  যদি(state->pos.row >= rows)
    state->pos.row = rows - 1;
  যদি(state->pos.col < 0)
    state->pos.col = 0;
  যদি(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  ফেরত 1;
}

স্থির ধ্রুবক VTermParserCallbacks parser_callbacks = {
  on_text, // text
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
  যদি(vt->state)
    ফেরত vt->state;

  VTermState *state = vterm_state_new(vt);
  যদি (state == ফাঁকা)
    ফেরত ফাঁকা;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  ফেরত state;
}

শূন্য vterm_state_reset(VTermState *state, পূর্ণসংখ্যা hard)
{
  state->scrollregion_শীর্ষ = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_বাম = 0;
  state->scrollregion_ডান = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.ঢোকাও          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  জন্য(পূর্ণসংখ্যা col = 0; col < state->cols; col++)
    যদি(col % 8 == 0)
      set_col_tabstop(state, col);
    অন্যথা
      clear_col_tabstop(state, col);

  জন্য(পূর্ণসংখ্যা row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  যদি(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  জন্য(পূর্ণসংখ্যা i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    যদি(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].তথ্য);
  }

  state->gl_set = 0;
  state->gr_set = 1;
  state->gsingle_set = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_BLOCK);

  যদি(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

শূন্য vterm_state_get_cursorpos(ধ্রুবক VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

শূন্য vterm_state_get_mousestate(ধ্রুবক VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

শূন্য vterm_state_set_callbacks(VTermState *state, ধ্রুবক VTermStateCallbacks *callbacks, শূন্য *user)
{
  যদি(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    যদি(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  অন্যথা {
    state->callbacks = ফাঁকা;
    state->cbdata = ফাঁকা;
  }
}

শূন্য vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

শূন্য *vterm_state_get_cbdata(VTermState *state)
{
  ফেরত state->cbdata;
}

শূন্য vterm_state_set_unrecognised_fallbacks(VTermState *state, ধ্রুবক VTermStateFallbacks *fallbacks, শূন্য *user)
{
  যদি(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  অন্যথা {
    state->fallbacks = ফাঁকা;
    state->fbdata = ফাঁকা;
  }
}

শূন্য *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  ফেরত state->fbdata;
}

পূর্ণসংখ্যা vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_মান *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  যদি(state->callbacks && state->callbacks->settermprop)
    যদি(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      ফেরত 0;

  নির্বাচন(prop) {
  ক্ষেত্র VTERM_PROP_TITLE:
  ক্ষেত্র VTERM_PROP_ICONNAME:
  ক্ষেত্র VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    ফেরত 1;
  ক্ষেত্র VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    ফেরত 1;
  ক্ষেত্র VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    ফেরত 1;
  ক্ষেত্র VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->সংখ্যা;
    ফেরত 1;
  ক্ষেত্র VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    ফেরত 1;
  ক্ষেত্র VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    যদি(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    ফেরত 1;
  ক্ষেত্র VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    যদি(val->সংখ্যা)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    যদি(val->সংখ্যা == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    যদি(val->সংখ্যা == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    ফেরত 1;
  ক্ষেত্র VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    ফেরত 1;

  ক্ষেত্র VTERM_N_PROPS:
    ফেরত 0;
  }

  ফেরত 0;
}

শূন্য vterm_state_focus_in(VTermState *state)
{
  যদি(state->mode.report_focus)
    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

শূন্য vterm_state_focus_out(VTermState *state)
{
  যদি(state->mode.report_focus)
    vterm_ঠেলো_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ধ্রুবক VTerm_সারি_Info *vterm_state_get_lineinfo(ধ্রুবক VTermState *state, পূর্ণসংখ্যা row)
{
  ফেরত state->lineinfo + row;
}

শূন্য vterm_state_set_selection_callbacks(VTermState *state, ধ্রুবক VTermSelectionCallbacks *callbacks, শূন্য *user,
    অক্ষর *বাফার, size_t buflen)
{
  যদি(buflen && !বাফার)
    বাফার = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.বাফার    = বাফার;
  state->selection.buflen    = buflen;
}

শূন্য vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  যদি(frag.initial) {
    /* TODO: support sending more than one mask bit */
    স্থির অক্ষর selection_chars[] = "cpqs";
    পূর্ণসংখ্যা idx;
    জন্য(idx = 0; idx < 4; idx++)
      যদি(mask & (1 << idx))
        বিরতি;

    vterm_ঠেলো_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  যদি(frag.len) {
    size_t bufcur = 0;
    অক্ষর *বাফার = state->selection.বাফার;

    uint32_t x = 0;
    পূর্ণসংখ্যা n = 0;

    যদি(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    যখন((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      যদি(n == 3) {
        বাফার[0] = base64_one((x >> 18) & 0x3F);
        বাফার[1] = base64_one((x >> 12) & 0x3F);
        বাফার[2] = base64_one((x >>  6) & 0x3F);
        বাফার[3] = base64_one((x >>  0) & 0x3F);

        বাফার += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      যদি(!frag.len || (state->selection.buflen - bufcur) < 4) {
        যদি(bufcur)
          vterm_ঠেলো_output_bytes(vt, state->selection.বাফার, bufcur);

        বাফার = state->selection.বাফার;
        bufcur = 0;
      }
    }

    যদি(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  যদি(frag.final) {
    যদি(state->tmp.selection.sendpartial) {
      পূর্ণসংখ্যা n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      অক্ষর *বাফার = state->selection.বাফার;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      বাফার[0] = base64_one((x >> 18) & 0x3F);
      বাফার[1] = base64_one((x >> 12) & 0x3F);
      বাফার[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      বাফার[3] = '=';

      vterm_ঠেলো_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, বাফার);
    }
    অন্যথা
      vterm_ঠেলো_output_sprintf_str(vt, 0, TRUE, "");
  }
}
