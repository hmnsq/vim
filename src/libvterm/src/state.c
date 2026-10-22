#ಅನ್ತರ್ಭಾವ "vterm_internal.h"

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#ಯದಿ defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_resize(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, ಶೂನ್ಯ *user);

/* Some convenient wrappers to make callback functions easier */

ಸ್ಥಿರ ಶೂನ್ಯ putglyph(VTermState *state, ಧ್ರುವ uint32_t chars[], ಪೂರ್ಣಾಙ್ಕ width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  ಯದಿ(state->callbacks && state->callbacks->putglyph)
    ಯದಿ((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      ಪ್ರತ್ಯಾಗಮ;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

ಸ್ಥಿರ ಶೂನ್ಯ updatecursor(VTermState *state, VTermPos *oldpos, ಪೂರ್ಣಾಙ್ಕ cancel_phantom)
{
  ಯದಿ(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(cancel_phantom)
    state->at_phantom = 0;

  ಯದಿ(state->callbacks && state->callbacks->movecursor)
    ಯದಿ((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ ಶೂನ್ಯ erase(VTermState *state, VTermRect rect, ಪೂರ್ಣಾಙ್ಕ selective)
{
  ಯದಿ(rect.end_col == state->cols) {
    ಪೂರ್ಣಾಙ್ಕ row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    ಪ್ರತಿ(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  ಯದಿ(state->callbacks && state->callbacks->erase)
    ಯದಿ((*state->callbacks->erase)(rect, selective, state->cbdata))
      ಪ್ರತ್ಯಾಗಮ;
}

ಸ್ಥಿರ VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, ಪರಿಮಾಣ(VTermState));

  ಯದಿ (state == ರಿಕ್ತ)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ರಿಕ್ತ;
  state->cbdata    = ರಿಕ್ತ;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ರಿಕ್ತ;
  state->selection.user      = ರಿಕ್ತ;
  state->selection.buffer    = ರಿಕ್ತ;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * ಪರಿಮಾಣ(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * ಪರಿಮಾಣ(VTerm_ಸಾಲು_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * ಪರಿಮಾಣ(VTerm_ಸಾಲು_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  ಯದಿ(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  ಪ್ರತ್ಯಾಗಮ state;
}

INTERNAL ಶೂನ್ಯ vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  ಯದಿ(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ಸ್ಥಿರ ಶೂನ್ಯ scroll(VTermState *state, VTermRect rect, ಪೂರ್ಣಾಙ್ಕ downward, ಪೂರ್ಣಾಙ್ಕ rightward)
{
  ಪೂರ್ಣಾಙ್ಕ rows;
  ಪೂರ್ಣಾಙ್ಕ cols;
  ಯದಿ(!downward && !rightward)
    ಪ್ರತ್ಯಾಗಮ;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  ಯದಿ(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    ಪ್ರತ್ಯಾಗಮ;

  rows = rect.end_row - rect.start_row;
  ಯದಿ(downward > rows)
    downward = rows;
  ಅನ್ಯಥಾ ಯದಿ(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  ಯದಿ(rightward > cols)
    rightward = cols;
  ಅನ್ಯಥಾ ಯದಿ(rightward < -cols)
    rightward = -cols;

  ಯದಿ(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    ಯದಿ(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    ಅನ್ಯಥಾ ಯದಿ(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    ಯದಿ(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    ಅನ್ಯಥಾ ಯದಿ(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  ಯದಿ(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    ಪೂರ್ಣಾಙ್ಕ height = rect.end_row - rect.start_row - abs(downward);
    ಪೂರ್ಣಾಙ್ಕ row;
    VTerm_ಸಾಲು_Info zero_ಸಾಲು_Info = {0x0};

    ಯದಿ(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * ಪರಿಮಾಣ(state->lineinfo[0]));
      ಪ್ರತಿ(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_ಸಾಲು_Info;
    }
    ಅನ್ಯಥಾ {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * ಪರಿಮಾಣ(state->lineinfo[0]));
      ಪ್ರತಿ(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_ಸಾಲು_Info;
    }
  }

  ಯದಿ(state->callbacks && state->callbacks->scrollrect)
    ಯದಿ((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ಸ್ಥಿರ ಶೂನ್ಯ linefeed(VTermState *state)
{
  ಯದಿ(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_ತುದಿ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ಎಡ(state);
    rect.end_col   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, 1, 0);
  }
  ಅನ್ಯಥಾ ಯದಿ(state->pos.row < state->rows-1)
    state->pos.row++;
}

ಸ್ಥಿರ ಶೂನ್ಯ grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * ಪರಿಮಾಣ(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * ಪರಿಮಾಣ(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

ಸ್ಥಿರ ಶೂನ್ಯ set_col_tabstop(VTermState *state, ಪೂರ್ಣಾಙ್ಕ col)
{
  unsigned ವರ್ಣ mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

ಸ್ಥಿರ ಶೂನ್ಯ clear_col_tabstop(VTermState *state, ಪೂರ್ಣಾಙ್ಕ col)
{
  unsigned ವರ್ಣ mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_col_tabstop(VTermState *state, ಪೂರ್ಣಾಙ್ಕ col)
{
  unsigned ವರ್ಣ mask = 1 << (col & 7);
  ಪ್ರತ್ಯಾಗಮ state->tabstops[col >> 3] & mask;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_cursor_in_scrollregion(ಧ್ರುವ VTermState *state)
{
  ಯದಿ(state->pos.row < state->scrollregion_ತುದಿ ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    ಪ್ರತ್ಯಾಗಮ 0;
  ಯದಿ(state->pos.col < SCROLLREGION_ಎಡ(state) ||
     state->pos.col >= SCROLLREGION_ಬಲ(state))
    ಪ್ರತ್ಯಾಗಮ 0;

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಶೂನ್ಯ tab(VTermState *state, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ direction)
{
  ಯಾವತ್(ಎಣಿಕೆ > 0) {
    ಯದಿ(direction > 0) {
      ಯದಿ(state->pos.col >= THISROWWIDTH(state)-1)
        ಪ್ರತ್ಯಾಗಮ;

      state->pos.col++;
    }
    ಅನ್ಯಥಾ ಯದಿ(direction < 0) {
      ಯದಿ(state->pos.col < 1)
        ಪ್ರತ್ಯಾಗಮ;

      state->pos.col--;
    }

    ಯದಿ(is_col_tabstop(state, state->pos.col))
      ಎಣಿಕೆ--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_ತುದಿ    1
#define DHL_BOTTOM 2

ಸ್ಥಿರ ಶೂನ್ಯ set_lineinfo(VTermState *state, ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ force, ಪೂರ್ಣಾಙ್ಕ dwl, ಪೂರ್ಣಾಙ್ಕ dhl)
{
  VTerm_ಸಾಲು_Info info = state->lineinfo[row];

  ಯದಿ(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  ಅನ್ಯಥಾ ಯದಿ(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  ಯದಿ(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  ಅನ್ಯಥಾ ಯದಿ(dhl == DHL_ತುದಿ)
    info.doubleheight = DHL_ತುದಿ;
  ಅನ್ಯಥಾ ಯದಿ(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  ಯದಿ((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_text(ಧ್ರುವ ವರ್ಣ bytes[], size_t len, ಶೂನ್ಯ *user)
{
  VTermState *state = user;
  ಪೂರ್ಣಾಙ್ಕ npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  ಪೂರ್ಣಾಙ್ಕ i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / ಪರಿಮಾಣ(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  ಯದಿ (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (ಪೂರ್ಣಾಙ್ಕ)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  ಯದಿ(!npoints)
  {
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)eaten;
  }

  ಯದಿ(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  ಯದಿ(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    ಯದಿ(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      ಪೂರ್ಣಾಙ್ಕ printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      ಪ್ರತಿ(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      ಪೂರ್ಣಾಙ್ಕ saved_i = 0;
      ಯಾವತ್(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      ಯಾವತ್(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        ಯದಿ(saved_i >= (ಪೂರ್ಣಾಙ್ಕ)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      ಯದಿ(saved_i >= (ಪೂರ್ಣಾಙ್ಕ)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      ಪ್ರತಿ(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    ಅನ್ಯಥಾ {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  ಪ್ರತಿ(; i < npoints; i++) {
    // Try to find combining characters following this
    ಪೂರ್ಣಾಙ್ಕ glyph_starts = i;
    ಪೂರ್ಣಾಙ್ಕ glyph_ends;
    ಪೂರ್ಣಾಙ್ಕ width = 0;

    ಪ್ರತಿ(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      ಯದಿ(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        ಭಙ್ಗ;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * ಪರಿಮಾಣ(uint32_t));
    ಯದಿ (chars == ರಿಕ್ತ)
      ಭಙ್ಗ;

    ಪ್ರತಿ( ; i < glyph_ends; i++) {
      ಪೂರ್ಣಾಙ್ಕ this_width;
      ಯದಿ(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        ಯದಿ(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      ಯದಿ(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      ಯದಿ (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    ಯದಿ (width < 0)
      width = 0;

    ಯಾವತ್(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    ಪೂರ್ಣಾಙ್ಕ printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    ಪ್ರತಿ(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    ಯದಿ(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    ಯದಿ(state->mode.ಸೇರಿಸು) {
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

    ಯದಿ(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      ಪೂರ್ಣಾಙ್ಕ save_i;
      ಪ್ರತಿ(save_i = 0; chars[save_i]; save_i++) {
        ಯದಿ(save_i >= (ಪೂರ್ಣಾಙ್ಕ)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      ಯದಿ(save_i >= (ಪೂರ್ಣಾಙ್ಕ)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    ಯದಿ(state->pos.col + width >= THISROWWIDTH(state)) {
      ಯದಿ(state->mode.autowrap)
        state->at_phantom = 1;
    }
    ಅನ್ಯಥಾ {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  ಯದಿ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)eaten;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_control(unsigned ವರ್ಣ control, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  ವಿಕಲ್ಪ(control) {
  ಪಕ್ಷ 0x07: // BEL - ECMA-48 8.3.3
    ಯದಿ(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x08: // BS - ECMA-48 8.3.5
    ಯದಿ(state->pos.col > 0)
      state->pos.col--;
    ಯದಿ(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      ಯದಿ(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x0a: // LF - ECMA-48 8.3.74
  ಪಕ್ಷ 0x0b: // VT
  ಪಕ್ಷ 0x0c: // FF
    linefeed(state);
    ಯದಿ(state->mode.newline)
      state->pos.col = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x8d: // RI - ECMA-48 8.3.104
    ಯದಿ(state->pos.row == state->scrollregion_ತುದಿ) {
      VTermRect rect;
      rect.start_row = state->scrollregion_ತುದಿ;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_ಎಡ(state);
      rect.end_col   = SCROLLREGION_ಬಲ(state);

      scroll(state, rect, -1, 0);
    }
    ಅನ್ಯಥಾ ಯದಿ(state->pos.row > 0)
        state->pos.row--;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    ಯದಿ(state->fallbacks && state->fallbacks->control)
      ಯದಿ((*state->fallbacks->control)(control, state->fbdata))
        ಪ್ರತ್ಯಾಗಮ 1;

    ಪ್ರತ್ಯಾಗಮ 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  ಯದಿ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ settermprop_bool(VTermState *state, VTermProp prop, ಪೂರ್ಣಾಙ್ಕ v)
{
  VTerm_ಮೌಲ್ಯ val;
  val.boolean = v;
  ಪ್ರತ್ಯಾಗಮ vterm_state_set_termprop(state, prop, &val);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ settermprop_int(VTermState *state, VTermProp prop, ಪೂರ್ಣಾಙ್ಕ v)
{
  VTerm_ಮೌಲ್ಯ val;
  val.ಸಂಖ್ಯೆ = v;
  ಪ್ರತ್ಯಾಗಮ vterm_state_set_termprop(state, prop, &val);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_ಮೌಲ್ಯ val;

  val.string = frag;
  ಪ್ರತ್ಯಾಗಮ vterm_state_set_termprop(state, prop, &val);
}

ಸ್ಥಿರ ಶೂನ್ಯ savecursor(VTermState *state, ಪೂರ್ಣಾಙ್ಕ save)
{
  ಯದಿ(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  ಅನ್ಯಥಾ {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_escape(ಧ್ರುವ ವರ್ಣ *bytes, size_t len, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  ವಿಕಲ್ಪ(bytes[0]) {
  ಪಕ್ಷ ' ':
    ಯದಿ(len != 2)
      ಪ್ರತ್ಯಾಗಮ 0;

    ವಿಕಲ್ಪ(bytes[1]) {
      ಪಕ್ಷ 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        ಭಙ್ಗ;

      ಪಕ್ಷ 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        ಭಙ್ಗ;

      ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ 2;

  ಪಕ್ಷ '#':
    ಯದಿ(len != 2)
      ಪ್ರತ್ಯಾಗಮ 0;

    ವಿಕಲ್ಪ(bytes[1]) {
      ಪಕ್ಷ '3': // DECDHL top
        ಯದಿ(state->mode.leftrightmargin)
          ಭಙ್ಗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_ತುದಿ);
        ಭಙ್ಗ;

      ಪಕ್ಷ '4': // DECDHL bottom
        ಯದಿ(state->mode.leftrightmargin)
          ಭಙ್ಗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        ಭಙ್ಗ;

      ಪಕ್ಷ '5': // DECSWL
        ಯದಿ(state->mode.leftrightmargin)
          ಭಙ್ಗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        ಭಙ್ಗ;

      ಪಕ್ಷ '6': // DECDWL
        ಯದಿ(state->mode.leftrightmargin)
          ಭಙ್ಗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        ಭಙ್ಗ;

      ಪಕ್ಷ '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        ಪ್ರತಿ(pos.row = 0; pos.row < state->rows; pos.row++)
          ಪ್ರತಿ(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        ಭಙ್ಗ;
      }

      ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ 2;

  ಪಕ್ಷ '(': ಪಕ್ಷ ')': ಪಕ್ಷ '*': ಪಕ್ಷ '+': // SCS
    ಯದಿ(len != 2)
      ಪ್ರತ್ಯಾಗಮ 0;

    {
      ಪೂರ್ಣಾಙ್ಕ setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      ಯದಿ(newenc) {
        state->encoding[setnum].enc = newenc;

        ಯದಿ(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    ಪ್ರತ್ಯಾಗಮ 2;

  ಪಕ್ಷ '7': // DECSC
    savecursor(state, 1);
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ '8': // DECRC
    savecursor(state, 0);
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ '=': // DECKPAM
    state->mode.keypad = 1;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ '>': // DECKPNM
    state->mode.keypad = 0;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    ಯದಿ(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    ಪ್ರತ್ಯಾಗಮ 1;
  }

  ಪಕ್ಷ 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ 0;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ set_mode(VTermState *state, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ val)
{
  ವಿಕಲ್ಪ(ಅಂಕೆ) {
  ಪಕ್ಷ 4: // IRM - ECMA-48 7.2.10
    state->mode.ಸೇರಿಸು = val;
    ಭಙ್ಗ;

  ಪಕ್ಷ 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", ಅಂಕೆ);
    ಪ್ರತ್ಯಾಗಮ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ set_dec_mode(VTermState *state, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, ಪೂರ್ಣಾಙ್ಕ val)
{
  ವಿಕಲ್ಪ(ಅಂಕೆ) {
  ಪಕ್ಷ 1:
    state->mode.cursor = val;
    ಭಙ್ಗ;

  ಪಕ್ಷ 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    ಭಙ್ಗ;

  ಪಕ್ಷ 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_ತುದಿ : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_ಎಡ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ 7:
    state->mode.autowrap = val;
    ಭಙ್ಗ;

  ಪಕ್ಷ 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    ಭಙ್ಗ;

  ಪಕ್ಷ 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    ಭಙ್ಗ;

  ಪಕ್ಷ 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    ಯದಿ(val) {
      ಪೂರ್ಣಾಙ್ಕ row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      ಪ್ರತಿ(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    ಭಙ್ಗ;

  ಪಕ್ಷ 1000:
  ಪಕ್ಷ 1002:
  ಪಕ್ಷ 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (ಅಂಕೆ == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (ಅಂಕೆ == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    ಭಙ್ಗ;

  ಪಕ್ಷ 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    ಭಙ್ಗ;

  ಪಕ್ಷ 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    ಭಙ್ಗ;

  ಪಕ್ಷ 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    ಭಙ್ಗ;

  ಪಕ್ಷ 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    ಭಙ್ಗ;

  ಪಕ್ಷ 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    ಭಙ್ಗ;

  ಪಕ್ಷ 1048:
    savecursor(state, val);
    ಭಙ್ಗ;

  ಪಕ್ಷ 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    ಭಙ್ಗ;

  ಪಕ್ಷ 2004:
    state->mode.bracketpaste = val;
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", ಅಂಕೆ);
    ಪ್ರತ್ಯಾಗಮ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ request_dec_mode(VTermState *state, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ)
{
  ಪೂರ್ಣಾಙ್ಕ reply;

  ವಿಕಲ್ಪ(ಅಂಕೆ) {
    ಪಕ್ಷ 1:
      reply = state->mode.cursor;
      ಭಙ್ಗ;

    ಪಕ್ಷ 5:
      reply = state->mode.screen;
      ಭಙ್ಗ;

    ಪಕ್ಷ 6:
      reply = state->mode.origin;
      ಭಙ್ಗ;

    ಪಕ್ಷ 7:
      reply = state->mode.autowrap;
      ಭಙ್ಗ;

    ಪಕ್ಷ 12:
      reply = state->mode.cursor_blink;
      ಭಙ್ಗ;

    ಪಕ್ಷ 25:
      reply = state->mode.cursor_visible;
      ಭಙ್ಗ;

    ಪಕ್ಷ 69:
      reply = state->mode.leftrightmargin;
      ಭಙ್ಗ;

    ಪಕ್ಷ 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      ಭಙ್ಗ;

    ಪಕ್ಷ 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      ಭಙ್ಗ;

    ಪಕ್ಷ 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      ಭಙ್ಗ;

    ಪಕ್ಷ 1004:
      reply = state->mode.report_focus;
      ಭಙ್ಗ;

    ಪಕ್ಷ 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      ಭಙ್ಗ;

    ಪಕ್ಷ 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      ಭಙ್ಗ;

    ಪಕ್ಷ 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      ಭಙ್ಗ;

    ಪಕ್ಷ 1047:
      reply = state->mode.alt_screen;
      ಭಙ್ಗ;

    ಪಕ್ಷ 2004:
      reply = state->mode.bracketpaste;
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ಅಂಕೆ, 0);
      ಪ್ರತ್ಯಾಗಮ;
  }

  vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ಅಂಕೆ, reply ? 1 : 2);
}

ಸ್ಥಿರ ಶೂನ್ಯ request_version_string(VTermState *state)
{
  vterm_ತಳ್ಳು_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_csi(ಧ್ರುವ ವರ್ಣ *leader, ಧ್ರುವ long args[], ಪೂರ್ಣಾಙ್ಕ argcount, ಧ್ರುವ ವರ್ಣ *intermed, ವರ್ಣ command, ಶೂನ್ಯ *user)
{
  VTermState *state = user;
  ಪೂರ್ಣಾಙ್ಕ leader_byte = 0;
  ಪೂರ್ಣಾಙ್ಕ intermed_byte = 0;
  ಪೂರ್ಣಾಙ್ಕ cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  ಪೂರ್ಣಾಙ್ಕ handled = 1;

  // Some temporaries for later code
  ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, val;
  ಪೂರ್ಣಾಙ್ಕ row, col;
  VTermRect rect;
  ಪೂರ್ಣಾಙ್ಕ selective;

  ಯದಿ(leader && leader[0]) {
    ಯದಿ(leader[1]) // longer than 1 char
      ಪ್ರತ್ಯಾಗಮ 0;

    ವಿಕಲ್ಪ(leader[0]) {
    ಪಕ್ಷ '?':
    ಪಕ್ಷ '>':
    ಪಕ್ಷ '<':
      leader_byte = leader[0];
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }

  ಯದಿ(intermed && intermed[0]) {
    ಯದಿ(intermed[1]) // longer than 1 char
      ಪ್ರತ್ಯಾಗಮ 0;

    ವಿಕಲ್ಪ(intermed[0]) {
    ಪಕ್ಷ ' ':
    ಪಕ್ಷ '!':
    ಪಕ್ಷ '"':
    ಪಕ್ಷ '$':
    ಪಕ್ಷ '\'':
      intermed_byte = intermed[0];
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) ಯದಿ((v) < (min)) (v) = (min)
#define UBOUND(v,max) ಯದಿ((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  ವಿಕಲ್ಪ(intermed_byte << 16 | leader_byte << 8 | command) {
  ಪಕ್ಷ 0x40: // ICH - ECMA-48 8.3.64
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    ಯದಿ(!is_cursor_in_scrollregion(state))
      ಭಙ್ಗ;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    ಯದಿ(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_ಬಲ(state);
    ಅನ್ಯಥಾ
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -ಎಣಿಕೆ);

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x41: // CUU - ECMA-48 8.3.22
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.row -= ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x42: // CUD - ECMA-48 8.3.19
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.row += ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x43: // CUF - ECMA-48 8.3.20
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.col += ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x44: // CUB - ECMA-48 8.3.18
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.col -= ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x45: // CNL - ECMA-48 8.3.12
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.col = 0;
    state->pos.row += ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x46: // CPL - ECMA-48 8.3.13
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.col = 0;
    state->pos.row -= ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    ಯದಿ(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      ಯದಿ(state->pos.row == row - 1) {
        ಪೂರ್ಣಾಙ್ಕ cnt, ptr = 0;
        ಪ್ರತಿ(cnt = 0; cnt < col - 1; ++cnt) {
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
    ಯದಿ(state->mode.origin) {
      state->pos.row += state->scrollregion_ತುದಿ;
      state->pos.col += SCROLLREGION_ಎಡ(state);
    }
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x49: // CHT - ECMA-48 8.3.10
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    tab(state, ಎಣಿಕೆ, +1);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x4a: // ED - ECMA-48 8.3.39
  ಪಕ್ಷ LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    ವಿಕಲ್ಪ(CSI_ARG(args[0])) {
    ಪಕ್ಷ CSI_ARG_MISSING:
    ಪಕ್ಷ 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      ಯದಿ(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      ಪ್ರತಿ(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      ಯದಿ(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      ಭಙ್ಗ;

    ಪಕ್ಷ 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      ಪ್ರತಿ(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      ಯದಿ(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      ಯದಿ(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      ಭಙ್ಗ;

    ಪಕ್ಷ 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      ಪ್ರತಿ(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      ಭಙ್ಗ;

    ಪಕ್ಷ 3:
      ಯದಿ(state->callbacks && state->callbacks->sb_clear)
        ಯದಿ((*state->callbacks->sb_clear)(state->cbdata))
          ಪ್ರತ್ಯಾಗಮ 1;
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x4b: // EL - ECMA-48 8.3.41
  ಪಕ್ಷ LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    ವಿಕಲ್ಪ(CSI_ARG(args[0])) {
    ಪಕ್ಷ CSI_ARG_MISSING:
    ಪಕ್ಷ 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); ಭಙ್ಗ;
    ಪಕ್ಷ 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; ಭಙ್ಗ;
    ಪಕ್ಷ 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಯದಿ(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x4c: // IL - ECMA-48 8.3.67
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    ಯದಿ(!is_cursor_in_scrollregion(state))
      ಭಙ್ಗ;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ಎಡ(state);
    rect.end_col   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, -ಎಣಿಕೆ, 0);

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x4d: // DL - ECMA-48 8.3.32
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    ಯದಿ(!is_cursor_in_scrollregion(state))
      ಭಙ್ಗ;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ಎಡ(state);
    rect.end_col   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, ಎಣಿಕೆ, 0);

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x50: // DCH - ECMA-48 8.3.26
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    ಯದಿ(!is_cursor_in_scrollregion(state))
      ಭಙ್ಗ;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    ಯದಿ(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_ಬಲ(state);
    ಅನ್ಯಥಾ
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, ಎಣಿಕೆ);

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x53: // SU - ECMA-48 8.3.147
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    rect.start_row = state->scrollregion_ತುದಿ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ಎಡ(state);
    rect.end_col   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, ಎಣಿಕೆ, 0);

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x54: // SD - ECMA-48 8.3.113
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    rect.start_row = state->scrollregion_ತುದಿ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ಎಡ(state);
    rect.end_col   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, -ಎಣಿಕೆ, 0);

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x58: // ECH - ECMA-48 8.3.38
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + ಎಣಿಕೆ;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x5a: // CBT - ECMA-48 8.3.7
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    tab(state, ಎಣಿಕೆ, -1);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x61: // HPR - ECMA-48 8.3.59
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.col += ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x62: { // REP - ECMA-48 8.3.103
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    ಯದಿ(state->combine_width < 1)
      ಭಙ್ಗ;

    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    col = state->pos.col + ಎಣಿಕೆ;
    UBOUND(col, row_width);
    ಯಾವತ್ (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    ಯದಿ (state->pos.col + state->combine_width >= row_width) {
      ಯದಿ (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    ಭಙ್ಗ;
  }

  ಪಕ್ಷ 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    ಯದಿ(val == 0)
      // DEC VT100 response
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    ಯದಿ(state->mode.origin)
      state->pos.row += state->scrollregion_ತುದಿ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x65: // VPR - ECMA-48 8.3.160
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.row += ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    ಯದಿ(state->mode.origin) {
      state->pos.row += state->scrollregion_ತುದಿ;
      state->pos.col += SCROLLREGION_ಎಡ(state);
    }
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    ವಿಕಲ್ಪ(val) {
    ಪಕ್ಷ 0:
      clear_col_tabstop(state, state->pos.col);
      ಭಙ್ಗ;
    ಪಕ್ಷ 3:
    ಪಕ್ಷ 5:
      ಪ್ರತಿ(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      ಭಙ್ಗ;
    ಪಕ್ಷ 1:
    ಪಕ್ಷ 2:
    ಪಕ್ಷ 4:
      ಭಙ್ಗ;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x68: // SM - ECMA-48 8.3.125
    ಯದಿ(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('?', 0x68): // DEC private mode set
    ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < argcount; i++) {
      ಯದಿ(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x6a: // HPB - ECMA-48 8.3.58
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.col -= ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x6b: // VPB - ECMA-48 8.3.159
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);
    state->pos.row -= ಎಣಿಕೆ;
    state->at_phantom = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x6c: // RM - ECMA-48 8.3.106
    ಯದಿ(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('?', 0x6c): // DEC private mode reset
    ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < argcount; i++) {
      ಯದಿ(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    ಯದಿ (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_ಕೀಲಿಗಳು ? 2 : 0);
      ಭಙ್ಗ;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ argi = 0; argi < argcount; argi++) {
      long arg;
      ವಿಕಲ್ಪ(arg = CSI_ARG(args[argi])) {
        ಪಕ್ಷ 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          ಭಙ್ಗ;
        ಪಕ್ಷ 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          ಭಙ್ಗ;
        ಪಕ್ಷ 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          ಭಙ್ಗ;
      }
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    ಯದಿ (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_ಕೀಲಿಗಳು = CSI_ARG(args[1]) == 2;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    ಯದಿ (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_ಕೀಲಿಗಳು = 0;

      state->mode.kitty_keyboard = 1;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    ಯದಿ (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    ಯದಿ (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    ಭಙ್ಗ;

  ಪಕ್ಷ 0x6e: // DSR - ECMA-48 8.3.35
  ಪಕ್ಷ LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      ವರ್ಣ *qmark = (leader_byte == '?') ? "?" : "";

      ವಿಕಲ್ಪ(val) {
      ಪಕ್ಷ 0: ಪಕ್ಷ 1: ಪಕ್ಷ 2: ಪಕ್ಷ 3: ಪಕ್ಷ 4:
        // ignore - these are replies
        ಭಙ್ಗ;
      ಪಕ್ಷ 5:
        vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        ಭಙ್ಗ;
      ಪಕ್ಷ 6: // CPR - cursor position report
        vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        ಭಙ್ಗ;
      }
    }
    ಭಙ್ಗ;


  ಪಕ್ಷ INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    ಭಙ್ಗ;

  ಪಕ್ಷ LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    ಭಙ್ಗ;

  ಪಕ್ಷ INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    ವಿಕಲ್ಪ(val) {
    ಪಕ್ಷ 0: ಪಕ್ಷ 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      ಭಙ್ಗ;
    ಪಕ್ಷ 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      ಭಙ್ಗ;
    ಪಕ್ಷ 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ಭಙ್ಗ;
    ಪಕ್ಷ 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ಭಙ್ಗ;
    ಪಕ್ಷ 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ಎಡ);
      ಭಙ್ಗ;
    ಪಕ್ಷ 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ಎಡ);
      ಭಙ್ಗ;
    }

    ಭಙ್ಗ;

  ಪಕ್ಷ INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    ವಿಕಲ್ಪ(val) {
    ಪಕ್ಷ 0: ಪಕ್ಷ 2:
      state->protected_cell = 0;
      ಭಙ್ಗ;
    ಪಕ್ಷ 1:
      state->protected_cell = 1;
      ಭಙ್ಗ;
    }

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x72: // DECSTBM - DEC custom
    state->scrollregion_ತುದಿ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_ತುದಿ, 0);
    UBOUND(state->scrollregion_ತುದಿ, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    ಯದಿ(state->scrollregion_ತುದಿ == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    ಅನ್ಯಥಾ
      UBOUND(state->scrollregion_bottom, state->rows);

    ಯದಿ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ತುದಿ) {
      // Invalid
      state->scrollregion_ತುದಿ    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    ಯದಿ(state->mode.origin) {
      state->pos.row += state->scrollregion_ತುದಿ;
      state->pos.col += SCROLLREGION_ಎಡ(state);
    }

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_ಎಡ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_ಬಲ = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_ಎಡ, 0);
    UBOUND(state->scrollregion_ಎಡ, state->cols);
    LBOUND(state->scrollregion_ಬಲ, -1);
    ಯದಿ(state->scrollregion_ಎಡ == 0 && state->scrollregion_ಬಲ == state->cols)
      state->scrollregion_ಬಲ = -1;
    ಅನ್ಯಥಾ
      UBOUND(state->scrollregion_ಬಲ, state->cols);

    ಯದಿ(state->scrollregion_ಬಲ > -1 &&
       state->scrollregion_ಬಲ <= state->scrollregion_ಎಡ) {
      // Invalid
      state->scrollregion_ಎಡ  = 0;
      state->scrollregion_ಬಲ = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    ಯದಿ(state->mode.origin) {
      state->pos.row += state->scrollregion_ತುದಿ;
      state->pos.col += SCROLLREGION_ಎಡ(state);
    }

    ಭಙ್ಗ;

  ಪಕ್ಷ 0x74:
    ವಿಕಲ್ಪ(CSI_ARG(args[0])) {
      ಪಕ್ಷ 8: // CSI 8 ; rows ; cols t  set size
        ಯದಿ (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	ಭಙ್ಗ;
      ಸ್ವಭಾವ:
	handled = 0;
	ಭಙ್ಗ;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ INTERMED('\'', 0x7D): // DECIC
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    ಯದಿ(!is_cursor_in_scrollregion(state))
      ಭಙ್ಗ;

    rect.start_row = state->scrollregion_ತುದಿ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, 0, -ಎಣಿಕೆ);

    ಭಙ್ಗ;

  ಪಕ್ಷ INTERMED('\'', 0x7E): // DECDC
    ಎಣಿಕೆ = CSI_ARG_ಎಣಿಕೆ(args[0]);

    ಯದಿ(!is_cursor_in_scrollregion(state))
      ಭಙ್ಗ;

    rect.start_row = state->scrollregion_ತುದಿ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_ಬಲ(state);

    scroll(state, rect, 0, ಎಣಿಕೆ);

    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    handled = 0;
    ಭಙ್ಗ;
  }

  ಯದಿ (!handled) {
    ಯದಿ(state->fallbacks && state->fallbacks->csi)
      ಯದಿ((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        ಪ್ರತ್ಯಾಗಮ 1;

    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಯದಿ(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_ತುದಿ);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_ಎಡ(state));
    UBOUND(state->pos.col, SCROLLREGION_ಬಲ(state)-1);
  }
  ಅನ್ಯಥಾ {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  ಯದಿ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  ಯದಿ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ತುದಿ) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_ತುದಿ);
    abort();
  }

  ಯದಿ(SCROLLREGION_ಬಲ(state) <= SCROLLREGION_ಎಡ(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_ಬಲ(state), SCROLLREGION_ಎಡ(state));
    abort();
  }
#endif

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ವರ್ಣ base64_one(uint8_t b)
{
  ಯದಿ(b < 26)
    ಪ್ರತ್ಯಾಗಮ 'A' + b;
  ಅನ್ಯಥಾ ಯದಿ(b < 52)
    ಪ್ರತ್ಯಾಗಮ 'a' + b - 26;
  ಅನ್ಯಥಾ ಯದಿ(b < 62)
    ಪ್ರತ್ಯಾಗಮ '0' + b - 52;
  ಅನ್ಯಥಾ ಯದಿ(b == 62)
    ಪ್ರತ್ಯಾಗಮ '+';
  ಅನ್ಯಥಾ ಯದಿ(b == 63)
    ಪ್ರತ್ಯಾಗಮ '/';
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ uint8_t unbase64one(ವರ್ಣ c)
{
  ಯದಿ(c >= 'A' && c <= 'Z')
    ಪ್ರತ್ಯಾಗಮ c - 'A';
  ಅನ್ಯಥಾ ಯದಿ(c >= 'a' && c <= 'z')
    ಪ್ರತ್ಯಾಗಮ c - 'a' + 26;
  ಅನ್ಯಥಾ ಯದಿ(c >= '0' && c <= '9')
    ಪ್ರತ್ಯಾಗಮ c - '0' + 52;
  ಅನ್ಯಥಾ ಯದಿ(c == '+')
    ಪ್ರತ್ಯಾಗಮ 62;
  ಅನ್ಯಥಾ ಯದಿ(c == '/')
    ಪ್ರತ್ಯಾಗಮ 63;

  ಪ್ರತ್ಯಾಗಮ 0xFF;
}

ಸ್ಥಿರ ಶೂನ್ಯ osc_selection(VTermState *state, VTermStringFragment frag)
{
  ಯದಿ(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  ಯಾವತ್(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    ವಿಕಲ್ಪ(frag.str[0]) {
      ಪಕ್ಷ 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        ಭಙ್ಗ;
      ಪಕ್ಷ 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        ಭಙ್ಗ;
      ಪಕ್ಷ 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        ಭಙ್ಗ;
      ಪಕ್ಷ 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        ಭಙ್ಗ;
      ಪಕ್ಷ '0':
      ಪಕ್ಷ '1':
      ಪಕ್ಷ '2':
      ಪಕ್ಷ '3':
      ಪಕ್ಷ '4':
      ಪಕ್ಷ '5':
      ಪಕ್ಷ '6':
      ಪಕ್ಷ '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        ಭಙ್ಗ;

      ಪಕ್ಷ ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        ಯದಿ(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        ಭಙ್ಗ;
    }

    frag.str++;
    frag.len--;
  }

  ಯದಿ(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    ಯದಿ(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = ರಿಕ್ತ,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(state->tmp.selection.state == SELECTION_SELECTED) {
    ಯದಿ(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    ಅನ್ಯಥಾ {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  ಯದಿ(state->tmp.selection.state == SELECTION_QUERY) {
    ಯದಿ(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    ಪ್ರತ್ಯಾಗಮ;
  }

  ಯದಿ(state->tmp.selection.state == SELECTION_INVALID)
    ಪ್ರತ್ಯಾಗಮ;

  ಯದಿ(state->selection.callbacks->set) {
    size_t bufcur = 0;
    ವರ್ಣ *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    ಪೂರ್ಣಾಙ್ಕ n = 0;      /* Number of sextets consumed */

    ಯದಿ(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    ಯಾವತ್((state->selection.buflen - bufcur) >= 3 && frag.len) {
      ಯದಿ(frag.str[0] == '=') {
        ಯದಿ(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        ಯದಿ(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        ಯಾವತ್(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      ಅನ್ಯಥಾ {
        uint8_t b = unbase64one(frag.str[0]);
        ಯದಿ(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          ಯದಿ(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = ರಿಕ್ತ,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          ಭಙ್ಗ;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        ಯದಿ(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      ಯದಿ(!frag.len || (state->selection.buflen - bufcur) < 3) {
        ಯದಿ(bufcur) {
	  VTermStringFragment setfrag = {
	    state->selection.buffer, // str
	    bufcur, // len
	    state->tmp.selection.state == SELECTION_SET_INITIAL, // initial
	    frag.final && !frag.len // final
	  };
          (*state->selection.callbacks->set)(state->tmp.selection.mask,
	      setfrag, state->selection.user);
          state->tmp.selection.state = SELECTION_SET;
        }

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    ಯದಿ(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_osc(ಪೂರ್ಣಾಙ್ಕ command, VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ವಿಕಲ್ಪ(command) {
    ಪಕ್ಷ 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ಪ್ರತ್ಯಾಗಮ 1;

    ಪಕ್ಷ 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      ಪ್ರತ್ಯಾಗಮ 1;

    ಪಕ್ಷ 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ಪ್ರತ್ಯಾಗಮ 1;

    ಪಕ್ಷ 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        ಪೂರ್ಣಾಙ್ಕ red = state->default_fg.red;
        ಪೂರ್ಣಾಙ್ಕ blue = state->default_fg.blue;
        ಪೂರ್ಣಾಙ್ಕ green = state->default_fg.green;
        vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        ಪ್ರತ್ಯಾಗಮ 1;
      }

    ಪಕ್ಷ 11:
      {
	// request background color: <Esc>]11;?<0x07>
	ಪೂರ್ಣಾಙ್ಕ red = state->default_bg.red;
	ಪೂರ್ಣಾಙ್ಕ blue = state->default_bg.blue;
	ಪೂರ್ಣಾಙ್ಕ green = state->default_bg.green;
	vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	ಪ್ರತ್ಯಾಗಮ 1;
      }
    ಪಕ್ಷ 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      ಪ್ರತ್ಯಾಗಮ 1;

    ಪಕ್ಷ 52:
      ಯದಿ(state->selection.callbacks)
        osc_selection(state, frag);

      ಪ್ರತ್ಯಾಗಮ 1;

    ಸ್ವಭಾವ:
      ಯದಿ(state->fallbacks && state->fallbacks->osc)
        ಯದಿ((*state->fallbacks->osc)(command, frag, state->fbdata))
          ಪ್ರತ್ಯಾಗಮ 1;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಶೂನ್ಯ request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ವರ್ಣ *tmp = state->tmp.decrqss;

  ಯದಿ(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  ಯಾವತ್(i < ಪರಿಮಾಣ(state->tmp.decrqss)-1 && tmp[i])
    i++;
  ಯಾವತ್(i < ಪರಿಮಾಣ(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  ಯದಿ(!frag.final)
    ಪ್ರತ್ಯಾಗಮ;

  ವಿಕಲ್ಪ(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    ಪಕ್ಷ 'm': {
      // Query SGR
      long args[20];
      ಪೂರ್ಣಾಙ್ಕ argc = vterm_state_getpen(state, args, ಪರಿಮಾಣ(args)/ಪರಿಮಾಣ(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      ಯದಿ(cur >= vt->tmpbuffer_len)
        ಪ್ರತ್ಯಾಗಮ;

      ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        ಯದಿ(cur >= vt->tmpbuffer_len)
          ಪ್ರತ್ಯಾಗಮ;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      ಯದಿ(cur >= vt->tmpbuffer_len)
        ಪ್ರತ್ಯಾಗಮ;

      vterm_ತಳ್ಳು_output_bytes(vt, vt->tmpbuffer, cur);
      ಪ್ರತ್ಯಾಗಮ;
    }

    ಪಕ್ಷ 'r':
      // Query DECSTBM
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_ತುದಿ+1, SCROLLREGION_BOTTOM(state));
      ಪ್ರತ್ಯಾಗಮ;

    ಪಕ್ಷ 's':
      // Query DECSLRM
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_ಎಡ(state)+1, SCROLLREGION_ಬಲ(state));
      ಪ್ರತ್ಯಾಗಮ;

    ಪಕ್ಷ ' '|('q'<<8): {
      // Query DECSCUSR
      ಪೂರ್ಣಾಙ್ಕ reply;
      ವಿಕಲ್ಪ(state->mode.cursor_shape) {
        ಪಕ್ಷ VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; ಭಙ್ಗ;
        ಪಕ್ಷ VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; ಭಙ್ಗ;
	ಸ್ವಭಾವ: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; ಭಙ್ಗ;
      }
      ಯದಿ(state->mode.cursor_blink)
        reply--;
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      ಪ್ರತ್ಯಾಗಮ;
    }

    ಪಕ್ಷ '\"'|('q'<<8):
      // Query DECSCA
      vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      ಪ್ರತ್ಯಾಗಮ;
  }

  vterm_ತಳ್ಳು_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_dcs(ಧ್ರುವ ವರ್ಣ *command, size_t commandlen, VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಯದಿ(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    ಪ್ರತ್ಯಾಗಮ 1;
  }
  ಅನ್ಯಥಾ ಯದಿ(state->fallbacks && state->fallbacks->dcs)
    ಯದಿ((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      ಪ್ರತ್ಯಾಗಮ 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (ಪೂರ್ಣಾಙ್ಕ)commandlen, command);
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_apc(VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಯದಿ(state->fallbacks && state->fallbacks->apc)
    ಯದಿ((*state->fallbacks->apc)(frag, state->fbdata))
      ಪ್ರತ್ಯಾಗಮ 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_pm(VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಯದಿ(state->fallbacks && state->fallbacks->pm)
    ಯದಿ((*state->fallbacks->pm)(frag, state->fbdata))
      ಪ್ರತ್ಯಾಗಮ 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_sos(VTermStringFragment frag, ಶೂನ್ಯ *user)
{
  VTermState *state = user;

  ಯದಿ(state->fallbacks && state->fallbacks->sos)
    ಯದಿ((*state->fallbacks->sos)(frag, state->fbdata))
      ಪ್ರತ್ಯಾಗಮ 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ on_resize(ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols, ಶೂನ್ಯ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  ಯದಿ (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  ಯದಿ (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  ಯದಿ(cols != state->cols) {
    unsigned ವರ್ಣ *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    ಯದಿ (newtabstops == ರಿಕ್ತ)
      ಪ್ರತ್ಯಾಗಮ 0;

    /* TODO: This can all be done much more efficiently bytewise */
    ಪೂರ್ಣಾಙ್ಕ col;
    ಪ್ರತಿ(col = 0; col < state->cols && col < cols; col++) {
      unsigned ವರ್ಣ mask = 1 << (col & 7);
      ಯದಿ(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      ಅನ್ಯಥಾ
        newtabstops[col >> 3] &= ~mask;
      }

    ಪ್ರತಿ( ; col < cols; col++) {
      unsigned ವರ್ಣ mask = 1 << (col & 7);
      ಯದಿ(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      ಅನ್ಯಥಾ
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  ಯದಿ(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  ಯದಿ(state->scrollregion_ಬಲ > -1)
    UBOUND(state->scrollregion_ಬಲ, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_ತುದಿ, state->rows);
  UBOUND(state->scrollregion_ಎಡ, state->cols);
  ಯದಿ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ತುದಿ) {
    state->scrollregion_ತುದಿ = 0;
    state->scrollregion_bottom = -1;
  }
  ಯದಿ(SCROLLREGION_ಬಲ(state) <= state->scrollregion_ಎಡ) {
    state->scrollregion_ಎಡ = 0;
    state->scrollregion_ಬಲ = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  ಯದಿ(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  ಅನ್ಯಥಾ {
    ಯದಿ(rows != state->rows) {
      ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_ಸಾಲು_Info *oldlineinfo = state->lineinfos[bufidx];
        ಯದಿ(!oldlineinfo)
          ಅನುವರ್ತ;

        VTerm_ಸಾಲು_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * ಪರಿಮಾಣ(VTerm_ಸಾಲು_Info));

        ಪೂರ್ಣಾಙ್ಕ row;
        ಪ್ರತಿ(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        ಪ್ರತಿ( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_ಸಾಲು_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  ಯದಿ(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  ಯದಿ(state->pos.row < 0)
    state->pos.row = 0;
  ಯದಿ(state->pos.row >= rows)
    state->pos.row = rows - 1;
  ಯದಿ(state->pos.col < 0)
    state->pos.col = 0;
  ಯದಿ(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ VTermParserCallbacks parser_callbacks = {
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
  ಯದಿ(vt->state)
    ಪ್ರತ್ಯಾಗಮ vt->state;

  VTermState *state = vterm_state_new(vt);
  ಯದಿ (state == ರಿಕ್ತ)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  ಪ್ರತ್ಯಾಗಮ state;
}

ಶೂನ್ಯ vterm_state_reset(VTermState *state, ಪೂರ್ಣಾಙ್ಕ hard)
{
  state->scrollregion_ತುದಿ = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_ಎಡ = 0;
  state->scrollregion_ಬಲ = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.ಸೇರಿಸು          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ col = 0; col < state->cols; col++)
    ಯದಿ(col % 8 == 0)
      set_col_tabstop(state, col);
    ಅನ್ಯಥಾ
      clear_col_tabstop(state, col);

  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  ಯದಿ(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  ಪ್ರತಿ(ಪೂರ್ಣಾಙ್ಕ i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    ಯದಿ(default_enc->init)
      (*default_enc->init)(default_enc, state->encoding[i].data);
  }

  state->gl_set = 0;
  state->gr_set = 1;
  state->gsingle_set = 0;

  state->protected_cell = 0;

  // Initialise the props
  settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, 1);
  settermprop_bool(state, VTERM_PROP_CURSORBLINK,   1);
  settermprop_int (state, VTERM_PROP_CURSORSHAPE,   VTERM_PROP_CURSORSHAPE_BLOCK);

  ಯದಿ(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

ಶೂನ್ಯ vterm_state_get_cursorpos(ಧ್ರುವ VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

ಶೂನ್ಯ vterm_state_get_mousestate(ಧ್ರುವ VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

ಶೂನ್ಯ vterm_state_set_callbacks(VTermState *state, ಧ್ರುವ VTermStateCallbacks *callbacks, ಶೂನ್ಯ *user)
{
  ಯದಿ(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    ಯದಿ(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  ಅನ್ಯಥಾ {
    state->callbacks = ರಿಕ್ತ;
    state->cbdata = ರಿಕ್ತ;
  }
}

ಶೂನ್ಯ vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

ಶೂನ್ಯ *vterm_state_get_cbdata(VTermState *state)
{
  ಪ್ರತ್ಯಾಗಮ state->cbdata;
}

ಶೂನ್ಯ vterm_state_set_unrecognised_fallbacks(VTermState *state, ಧ್ರುವ VTermStateFallbacks *fallbacks, ಶೂನ್ಯ *user)
{
  ಯದಿ(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  ಅನ್ಯಥಾ {
    state->fallbacks = ರಿಕ್ತ;
    state->fbdata = ರಿಕ್ತ;
  }
}

ಶೂನ್ಯ *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  ಪ್ರತ್ಯಾಗಮ state->fbdata;
}

ಪೂರ್ಣಾಙ್ಕ vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_ಮೌಲ್ಯ *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  ಯದಿ(state->callbacks && state->callbacks->settermprop)
    ಯದಿ(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      ಪ್ರತ್ಯಾಗಮ 0;

  ವಿಕಲ್ಪ(prop) {
  ಪಕ್ಷ VTERM_PROP_TITLE:
  ಪಕ್ಷ VTERM_PROP_ICONNAME:
  ಪಕ್ಷ VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->ಸಂಖ್ಯೆ;
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    ಯದಿ(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    ಯದಿ(val->ಸಂಖ್ಯೆ)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    ಯದಿ(val->ಸಂಖ್ಯೆ == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    ಯದಿ(val->ಸಂಖ್ಯೆ == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    ಪ್ರತ್ಯಾಗಮ 1;
  ಪಕ್ಷ VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    ಪ್ರತ್ಯಾಗಮ 1;

  ಪಕ್ಷ VTERM_N_PROPS:
    ಪ್ರತ್ಯಾಗಮ 0;
  }

  ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ vterm_state_focus_in(VTermState *state)
{
  ಯದಿ(state->mode.report_focus)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

ಶೂನ್ಯ vterm_state_focus_out(VTermState *state)
{
  ಯದಿ(state->mode.report_focus)
    vterm_ತಳ್ಳು_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ಧ್ರುವ VTerm_ಸಾಲು_Info *vterm_state_get_lineinfo(ಧ್ರುವ VTermState *state, ಪೂರ್ಣಾಙ್ಕ row)
{
  ಪ್ರತ್ಯಾಗಮ state->lineinfo + row;
}

ಶೂನ್ಯ vterm_state_set_selection_callbacks(VTermState *state, ಧ್ರುವ VTermSelectionCallbacks *callbacks, ಶೂನ್ಯ *user,
    ವರ್ಣ *buffer, size_t buflen)
{
  ಯದಿ(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

ಶೂನ್ಯ vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ಯದಿ(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ಸ್ಥಿರ ವರ್ಣ selection_chars[] = "cpqs";
    ಪೂರ್ಣಾಙ್ಕ idx;
    ಪ್ರತಿ(idx = 0; idx < 4; idx++)
      ಯದಿ(mask & (1 << idx))
        ಭಙ್ಗ;

    vterm_ತಳ್ಳು_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  ಯದಿ(frag.len) {
    size_t bufcur = 0;
    ವರ್ಣ *buffer = state->selection.buffer;

    uint32_t x = 0;
    ಪೂರ್ಣಾಙ್ಕ n = 0;

    ಯದಿ(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    ಯಾವತ್((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      ಯದಿ(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      ಯದಿ(!frag.len || (state->selection.buflen - bufcur) < 4) {
        ಯದಿ(bufcur)
          vterm_ತಳ್ಳು_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    ಯದಿ(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  ಯದಿ(frag.final) {
    ಯದಿ(state->tmp.selection.sendpartial) {
      ಪೂರ್ಣಾಙ್ಕ n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      ವರ್ಣ *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_ತಳ್ಳು_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    ಅನ್ಯಥಾ
      vterm_ತಳ್ಳು_output_sprintf_str(vt, 0, TRUE, "");
  }
}
