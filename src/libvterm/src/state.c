#ଅନ୍ତର୍ଭାଵ "vterm_internal.h"

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#ଯଦି defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_resize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଶୂନ୍ୟ *user);

/* Some convenient wrappers to make callback functions easier */

ସ୍ଥିର ଶୂନ୍ୟ putglyph(VTermState *state, ଅଚଳ uint32_t chars[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  ଯଦି(state->callbacks && state->callbacks->putglyph)
    ଯଦି((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      ଫେରନ୍ତୁ;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

ସ୍ଥିର ଶୂନ୍ୟ updatecursor(VTermState *state, VTermPos *oldpos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cancel_phantom)
{
  ଯଦି(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    ଫେରନ୍ତୁ;

  ଯଦି(cancel_phantom)
    state->at_phantom = 0;

  ଯଦି(state->callbacks && state->callbacks->movecursor)
    ଯଦି((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      ଫେରନ୍ତୁ;
}

ସ୍ଥିର ଶୂନ୍ୟ erase(VTermState *state, VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective)
{
  ଯଦି(rect.end_col == state->cols) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    ପାଇଁ(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  ଯଦି(state->callbacks && state->callbacks->erase)
    ଯଦି((*state->callbacks->erase)(rect, selective, state->cbdata))
      ଫେରନ୍ତୁ;
}

ସ୍ଥିର VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, ଆକାର(VTermState));

  ଯଦି (state == ଖାଲି)
    ଫେରନ୍ତୁ ଖାଲି;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ଖାଲି;
  state->cbdata    = ଖାଲି;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ଖାଲି;
  state->selection.user      = ଖାଲି;
  state->selection.buffer    = ଖାଲି;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * ଆକାର(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * ଆକାର(VTerm_ଧାଡ଼ି_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * ଆକାର(VTerm_ଧାଡ଼ି_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  ଯଦି(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  ଫେରନ୍ତୁ state;
}

INTERNAL ଶୂନ୍ୟ vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  ଯଦି(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ସ୍ଥିର ଶୂନ୍ୟ scroll(VTermState *state, VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rightward)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols;
  ଯଦି(!downward && !rightward)
    ଫେରନ୍ତୁ;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  ଯଦି(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    ଫେରନ୍ତୁ;

  rows = rect.end_row - rect.start_row;
  ଯଦି(downward > rows)
    downward = rows;
  ଅନ୍ୟଥା ଯଦି(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  ଯଦି(rightward > cols)
    rightward = cols;
  ଅନ୍ୟଥା ଯଦି(rightward < -cols)
    rightward = -cols;

  ଯଦି(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    ଯଦି(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    ଅନ୍ୟଥା ଯଦି(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    ଯଦି(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    ଅନ୍ୟଥା ଯଦି(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  ଯଦି(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା height = rect.end_row - rect.start_row - abs(downward);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;
    VTerm_ଧାଡ଼ି_Info zero_ଧାଡ଼ି_Info = {0x0};

    ଯଦି(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * ଆକାର(state->lineinfo[0]));
      ପାଇଁ(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_ଧାଡ଼ି_Info;
    }
    ଅନ୍ୟଥା {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * ଆକାର(state->lineinfo[0]));
      ପାଇଁ(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_ଧାଡ଼ି_Info;
    }
  }

  ଯଦି(state->callbacks && state->callbacks->scrollrect)
    ଯଦି((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      ଫେରନ୍ତୁ;

  ଯଦି(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ସ୍ଥିର ଶୂନ୍ୟ linefeed(VTermState *state)
{
  ଯଦି(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_ଶୀର୍ଷ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ବାମ(state);
    rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

    scroll(state, rect, 1, 0);
  }
  ଅନ୍ୟଥା ଯଦି(state->pos.row < state->rows-1)
    state->pos.row++;
}

ସ୍ଥିର ଶୂନ୍ୟ grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * ଆକାର(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * ଆକାର(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

ସ୍ଥିର ଶୂନ୍ୟ set_col_tabstop(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
  unsigned ଅକ୍ଷର mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

ସ୍ଥିର ଶୂନ୍ୟ clear_col_tabstop(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
  unsigned ଅକ୍ଷର mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_col_tabstop(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
  unsigned ଅକ୍ଷର mask = 1 << (col & 7);
  ଫେରନ୍ତୁ state->tabstops[col >> 3] & mask;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_cursor_in_scrollregion(ଅଚଳ VTermState *state)
{
  ଯଦି(state->pos.row < state->scrollregion_ଶୀର୍ଷ ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    ଫେରନ୍ତୁ 0;
  ଯଦି(state->pos.col < SCROLLREGION_ବାମ(state) ||
     state->pos.col >= SCROLLREGION_ଡାହାଣ(state))
    ଫେରନ୍ତୁ 0;

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ tab(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction)
{
  ଯେତେବେଳେ(ଗଣନା_2 > 0) {
    ଯଦି(direction > 0) {
      ଯଦି(state->pos.col >= THISROWWIDTH(state)-1)
        ଫେରନ୍ତୁ;

      state->pos.col++;
    }
    ଅନ୍ୟଥା ଯଦି(direction < 0) {
      ଯଦି(state->pos.col < 1)
        ଫେରନ୍ତୁ;

      state->pos.col--;
    }

    ଯଦି(is_col_tabstop(state, state->pos.col))
      ଗଣନା_2--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_ଶୀର୍ଷ    1
#define DHL_BOTTOM 2

ସ୍ଥିର ଶୂନ୍ୟ set_lineinfo(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା force, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dwl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dhl)
{
  VTerm_ଧାଡ଼ି_Info info = state->lineinfo[row];

  ଯଦି(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  ଅନ୍ୟଥା ଯଦି(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  ଯଦି(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  ଅନ୍ୟଥା ଯଦି(dhl == DHL_ଶୀର୍ଷ)
    info.doubleheight = DHL_ଶୀର୍ଷ;
  ଅନ୍ୟଥା ଯଦି(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  ଯଦି((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_text(ଅଚଳ ଅକ୍ଷର bytes[], size_t len, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / ଆକାର(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  ଯଦି (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  ଯଦି(!npoints)
  {
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eaten;
  }

  ଯଦି(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  ଯଦି(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    ଯଦି(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      ପାଇଁ(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_i = 0;
      ଯେତେବେଳେ(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      ଯେତେବେଳେ(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        ଯଦି(saved_i >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      ଯଦି(saved_i >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      ପାଇଁ(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    ଅନ୍ୟଥା {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  ପାଇଁ(; i < npoints; i++) {
    // Try to find combining characters following this
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା glyph_starts = i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା glyph_ends;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 0;

    ପାଇଁ(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      ଯଦି(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        ଭାଙ୍ଗ;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * ଆକାର(uint32_t));
    ଯଦି (chars == ଖାଲି)
      ଭାଙ୍ଗ;

    ପାଇଁ( ; i < glyph_ends; i++) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା this_width;
      ଯଦି(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        ଯଦି(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      ଯଦି(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      ଯଦି (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    ଯଦି (width < 0)
      width = 0;

    ଯେତେବେଳେ(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    ପାଇଁ(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    ଯଦି(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    ଯଦି(state->mode.ଭର୍ତ୍ତି) {
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

    ଯଦି(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_i;
      ପାଇଁ(save_i = 0; chars[save_i]; save_i++) {
        ଯଦି(save_i >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      ଯଦି(save_i >= (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    ଯଦି(state->pos.col + width >= THISROWWIDTH(state)) {
      ଯଦି(state->mode.autowrap)
        state->at_phantom = 1;
    }
    ଅନ୍ୟଥା {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  ଯଦି(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)eaten;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_control(unsigned ଅକ୍ଷର control, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  ବାଛ(control) {
  ସ୍ଥିତି 0x07: // BEL - ECMA-48 8.3.3
    ଯଦି(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x08: // BS - ECMA-48 8.3.5
    ଯଦି(state->pos.col > 0)
      state->pos.col--;
    ଯଦି(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      ଯଦି(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x0a: // LF - ECMA-48 8.3.74
  ସ୍ଥିତି 0x0b: // VT
  ସ୍ଥିତି 0x0c: // FF
    linefeed(state);
    ଯଦି(state->mode.newline)
      state->pos.col = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x8d: // RI - ECMA-48 8.3.104
    ଯଦି(state->pos.row == state->scrollregion_ଶୀର୍ଷ) {
      VTermRect rect;
      rect.start_row = state->scrollregion_ଶୀର୍ଷ;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_ବାମ(state);
      rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

      scroll(state, rect, -1, 0);
    }
    ଅନ୍ୟଥା ଯଦି(state->pos.row > 0)
        state->pos.row--;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଯଦି(state->fallbacks && state->fallbacks->control)
      ଯଦି((*state->fallbacks->control)(control, state->fbdata))
        ଫେରନ୍ତୁ 1;

    ଫେରନ୍ତୁ 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  ଯଦି(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା settermprop_bool(VTermState *state, VTermProp prop, ପୂର୍ଣ୍ଣସଂଖ୍ୟା v)
{
  VTerm_ମୂଲ୍ୟ val;
  val.boolean = v;
  ଫେରନ୍ତୁ vterm_state_set_termprop(state, prop, &val);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା settermprop_int(VTermState *state, VTermProp prop, ପୂର୍ଣ୍ଣସଂଖ୍ୟା v)
{
  VTerm_ମୂଲ୍ୟ val;
  val.ସଂଖ୍ୟା = v;
  ଫେରନ୍ତୁ vterm_state_set_termprop(state, prop, &val);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_ମୂଲ୍ୟ val;

  val.string = frag;
  ଫେରନ୍ତୁ vterm_state_set_termprop(state, prop, &val);
}

ସ୍ଥିର ଶୂନ୍ୟ savecursor(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା save)
{
  ଯଦି(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  ଅନ୍ୟଥା {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_escape(ଅଚଳ ଅକ୍ଷର *bytes, size_t len, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  ବାଛ(bytes[0]) {
  ସ୍ଥିତି ' ':
    ଯଦି(len != 2)
      ଫେରନ୍ତୁ 0;

    ବାଛ(bytes[1]) {
      ସ୍ଥିତି 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        ଭାଙ୍ଗ;

      ସ୍ଥିତି 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        ଭାଙ୍ଗ;

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ 2;

  ସ୍ଥିତି '#':
    ଯଦି(len != 2)
      ଫେରନ୍ତୁ 0;

    ବାଛ(bytes[1]) {
      ସ୍ଥିତି '3': // DECDHL top
        ଯଦି(state->mode.leftrightmargin)
          ଭାଙ୍ଗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_ଶୀର୍ଷ);
        ଭାଙ୍ଗ;

      ସ୍ଥିତି '4': // DECDHL bottom
        ଯଦି(state->mode.leftrightmargin)
          ଭାଙ୍ଗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        ଭାଙ୍ଗ;

      ସ୍ଥିତି '5': // DECSWL
        ଯଦି(state->mode.leftrightmargin)
          ଭାଙ୍ଗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        ଭାଙ୍ଗ;

      ସ୍ଥିତି '6': // DECDWL
        ଯଦି(state->mode.leftrightmargin)
          ଭାଙ୍ଗ;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        ଭାଙ୍ଗ;

      ସ୍ଥିତି '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        ପାଇଁ(pos.row = 0; pos.row < state->rows; pos.row++)
          ପାଇଁ(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        ଭାଙ୍ଗ;
      }

      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ 2;

  ସ୍ଥିତି '(': ସ୍ଥିତି ')': ସ୍ଥିତି '*': ସ୍ଥିତି '+': // SCS
    ଯଦି(len != 2)
      ଫେରନ୍ତୁ 0;

    {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      ଯଦି(newenc) {
        state->encoding[setnum].enc = newenc;

        ଯଦି(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    ଫେରନ୍ତୁ 2;

  ସ୍ଥିତି '7': // DECSC
    savecursor(state, 1);
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି '8': // DECRC
    savecursor(state, 0);
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି '=': // DECKPAM
    state->mode.keypad = 1;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି '>': // DECKPNM
    state->mode.keypad = 0;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    ଯଦି(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    ଫେରନ୍ତୁ 1;
  }

  ସ୍ଥିତି 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    ଫେରନ୍ତୁ 1;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ 0;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ set_mode(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
  ବାଛ(ଅଙ୍କ) {
  ସ୍ଥିତି 4: // IRM - ECMA-48 7.2.10
    state->mode.ଭର୍ତ୍ତି = val;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", ଅଙ୍କ);
    ଫେରନ୍ତୁ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ set_dec_mode(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା val)
{
  ବାଛ(ଅଙ୍କ) {
  ସ୍ଥିତି 1:
    state->mode.cursor = val;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_ଶୀର୍ଷ : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_ବାମ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 7:
    state->mode.autowrap = val;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    ଯଦି(val) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      ପାଇଁ(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1000:
  ସ୍ଥିତି 1002:
  ସ୍ଥିତି 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (ଅଙ୍କ == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (ଅଙ୍କ == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1048:
    savecursor(state, val);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 2004:
    state->mode.bracketpaste = val;
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", ଅଙ୍କ);
    ଫେରନ୍ତୁ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ request_dec_mode(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା reply;

  ବାଛ(ଅଙ୍କ) {
    ସ୍ଥିତି 1:
      reply = state->mode.cursor;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 5:
      reply = state->mode.screen;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 6:
      reply = state->mode.origin;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 7:
      reply = state->mode.autowrap;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 12:
      reply = state->mode.cursor_blink;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 25:
      reply = state->mode.cursor_visible;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 69:
      reply = state->mode.leftrightmargin;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1004:
      reply = state->mode.report_focus;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1047:
      reply = state->mode.alt_screen;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 2004:
      reply = state->mode.bracketpaste;
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ଅଙ୍କ, 0);
      ଫେରନ୍ତୁ;
  }

  vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", ଅଙ୍କ, reply ? 1 : 2);
}

ସ୍ଥିର ଶୂନ୍ୟ request_version_string(VTermState *state)
{
  vterm_ଠେଲ_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_csi(ଅଚଳ ଅକ୍ଷର *leader, ଅଚଳ long args[], ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ଅଚଳ ଅକ୍ଷର *intermed, ଅକ୍ଷର command, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା leader_byte = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା intermed_byte = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା handled = 1;

  // Some temporaries for later code
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, val;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, col;
  VTermRect rect;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective;

  ଯଦି(leader && leader[0]) {
    ଯଦି(leader[1]) // longer than 1 char
      ଫେରନ୍ତୁ 0;

    ବାଛ(leader[0]) {
    ସ୍ଥିତି '?':
    ସ୍ଥିତି '>':
    ସ୍ଥିତି '<':
      leader_byte = leader[0];
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ 0;
    }
  }

  ଯଦି(intermed && intermed[0]) {
    ଯଦି(intermed[1]) // longer than 1 char
      ଫେରନ୍ତୁ 0;

    ବାଛ(intermed[0]) {
    ସ୍ଥିତି ' ':
    ସ୍ଥିତି '!':
    ସ୍ଥିତି '"':
    ସ୍ଥିତି '$':
    ସ୍ଥିତି '\'':
      intermed_byte = intermed[0];
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) ଯଦି((v) < (min)) (v) = (min)
#define UBOUND(v,max) ଯଦି((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  ବାଛ(intermed_byte << 16 | leader_byte << 8 | command) {
  ସ୍ଥିତି 0x40: // ICH - ECMA-48 8.3.64
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    ଯଦି(!is_cursor_in_scrollregion(state))
      ଭାଙ୍ଗ;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    ଯଦି(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_ଡାହାଣ(state);
    ଅନ୍ୟଥା
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -ଗଣନା_2);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x41: // CUU - ECMA-48 8.3.22
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.row -= ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x42: // CUD - ECMA-48 8.3.19
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.row += ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x43: // CUF - ECMA-48 8.3.20
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.col += ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x44: // CUB - ECMA-48 8.3.18
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.col -= ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x45: // CNL - ECMA-48 8.3.12
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.col = 0;
    state->pos.row += ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x46: // CPL - ECMA-48 8.3.13
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.col = 0;
    state->pos.row -= ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    ଯଦି(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      ଯଦି(state->pos.row == row - 1) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt, ptr = 0;
        ପାଇଁ(cnt = 0; cnt < col - 1; ++cnt) {
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
    ଯଦି(state->mode.origin) {
      state->pos.row += state->scrollregion_ଶୀର୍ଷ;
      state->pos.col += SCROLLREGION_ବାମ(state);
    }
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x49: // CHT - ECMA-48 8.3.10
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    tab(state, ଗଣନା_2, +1);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x4a: // ED - ECMA-48 8.3.39
  ସ୍ଥିତି LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    ବାଛ(CSI_ARG(args[0])) {
    ସ୍ଥିତି CSI_ARG_MISSING:
    ସ୍ଥିତି 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      ଯଦି(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      ପାଇଁ(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      ଯଦି(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      ପାଇଁ(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      ଯଦି(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      ଯଦି(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      ପାଇଁ(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି 3:
      ଯଦି(state->callbacks && state->callbacks->sb_clear)
        ଯଦି((*state->callbacks->sb_clear)(state->cbdata))
          ଫେରନ୍ତୁ 1;
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x4b: // EL - ECMA-48 8.3.41
  ସ୍ଥିତି LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    ବାଛ(CSI_ARG(args[0])) {
    ସ୍ଥିତି CSI_ARG_MISSING:
    ସ୍ଥିତି 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ 0;
    }

    ଯଦି(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x4c: // IL - ECMA-48 8.3.67
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    ଯଦି(!is_cursor_in_scrollregion(state))
      ଭାଙ୍ଗ;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ବାମ(state);
    rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

    scroll(state, rect, -ଗଣନା_2, 0);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x4d: // DL - ECMA-48 8.3.32
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    ଯଦି(!is_cursor_in_scrollregion(state))
      ଭାଙ୍ଗ;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ବାମ(state);
    rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

    scroll(state, rect, ଗଣନା_2, 0);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x50: // DCH - ECMA-48 8.3.26
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    ଯଦି(!is_cursor_in_scrollregion(state))
      ଭାଙ୍ଗ;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    ଯଦି(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_ଡାହାଣ(state);
    ଅନ୍ୟଥା
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, ଗଣନା_2);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x53: // SU - ECMA-48 8.3.147
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    rect.start_row = state->scrollregion_ଶୀର୍ଷ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ବାମ(state);
    rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

    scroll(state, rect, ଗଣନା_2, 0);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x54: // SD - ECMA-48 8.3.113
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    rect.start_row = state->scrollregion_ଶୀର୍ଷ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ବାମ(state);
    rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

    scroll(state, rect, -ଗଣନା_2, 0);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x58: // ECH - ECMA-48 8.3.38
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + ଗଣନା_2;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x5a: // CBT - ECMA-48 8.3.7
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    tab(state, ଗଣନା_2, -1);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x61: // HPR - ECMA-48 8.3.59
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.col += ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x62: { // REP - ECMA-48 8.3.103
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    ଯଦି(state->combine_width < 1)
      ଭାଙ୍ଗ;

    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    col = state->pos.col + ଗଣନା_2;
    UBOUND(col, row_width);
    ଯେତେବେଳେ (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    ଯଦି (state->pos.col + state->combine_width >= row_width) {
      ଯଦି (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    ଭାଙ୍ଗ;
  }

  ସ୍ଥିତି 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    ଯଦି(val == 0)
      // DEC VT100 response
      vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    ଯଦି(state->mode.origin)
      state->pos.row += state->scrollregion_ଶୀର୍ଷ;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x65: // VPR - ECMA-48 8.3.160
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.row += ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    ଯଦି(state->mode.origin) {
      state->pos.row += state->scrollregion_ଶୀର୍ଷ;
      state->pos.col += SCROLLREGION_ବାମ(state);
    }
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    ବାଛ(val) {
    ସ୍ଥିତି 0:
      clear_col_tabstop(state, state->pos.col);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 3:
    ସ୍ଥିତି 5:
      ପାଇଁ(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
    ସ୍ଥିତି 2:
    ସ୍ଥିତି 4:
      ଭାଙ୍ଗ;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ 0;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x68: // SM - ECMA-48 8.3.125
    ଯଦି(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('?', 0x68): // DEC private mode set
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argcount; i++) {
      ଯଦି(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x6a: // HPB - ECMA-48 8.3.58
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.col -= ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x6b: // VPB - ECMA-48 8.3.159
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);
    state->pos.row -= ଗଣନା_2;
    state->at_phantom = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x6c: // RM - ECMA-48 8.3.106
    ଯଦି(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('?', 0x6c): // DEC private mode reset
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argcount; i++) {
      ଯଦି(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    ଯଦି (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_ଚାବିଗୁଡ଼ିକ ? 2 : 0);
      ଭାଙ୍ଗ;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argi = 0; argi < argcount; argi++) {
      long arg;
      ବାଛ(arg = CSI_ARG(args[argi])) {
        ସ୍ଥିତି 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          ଭାଙ୍ଗ;
        ସ୍ଥିତି 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          ଭାଙ୍ଗ;
      }
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    ଯଦି (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_ଚାବିଗୁଡ଼ିକ = CSI_ARG(args[1]) == 2;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    ଯଦି (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_ଚାବିଗୁଡ଼ିକ = 0;

      state->mode.kitty_keyboard = 1;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    ଯଦି (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    ଯଦି (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x6e: // DSR - ECMA-48 8.3.35
  ସ୍ଥିତି LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      ଅକ୍ଷର *qmark = (leader_byte == '?') ? "?" : "";

      ବାଛ(val) {
      ସ୍ଥିତି 0: ସ୍ଥିତି 1: ସ୍ଥିତି 2: ସ୍ଥିତି 3: ସ୍ଥିତି 4:
        // ignore - these are replies
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 5:
        vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 6: // CPR - cursor position report
        vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        ଭାଙ୍ଗ;
      }
    }
    ଭାଙ୍ଗ;


  ସ୍ଥିତି INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    ଭାଙ୍ଗ;

  ସ୍ଥିତି LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    ବାଛ(val) {
    ସ୍ଥିତି 0: ସ୍ଥିତି 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ବାମ);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ବାମ);
      ଭାଙ୍ଗ;
    }

    ଭାଙ୍ଗ;

  ସ୍ଥିତି INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    ବାଛ(val) {
    ସ୍ଥିତି 0: ସ୍ଥିତି 2:
      state->protected_cell = 0;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
      state->protected_cell = 1;
      ଭାଙ୍ଗ;
    }

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x72: // DECSTBM - DEC custom
    state->scrollregion_ଶୀର୍ଷ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_ଶୀର୍ଷ, 0);
    UBOUND(state->scrollregion_ଶୀର୍ଷ, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    ଯଦି(state->scrollregion_ଶୀର୍ଷ == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    ଅନ୍ୟଥା
      UBOUND(state->scrollregion_bottom, state->rows);

    ଯଦି(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ଶୀର୍ଷ) {
      // Invalid
      state->scrollregion_ଶୀର୍ଷ    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    ଯଦି(state->mode.origin) {
      state->pos.row += state->scrollregion_ଶୀର୍ଷ;
      state->pos.col += SCROLLREGION_ବାମ(state);
    }

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_ବାମ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_ଡାହାଣ = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_ବାମ, 0);
    UBOUND(state->scrollregion_ବାମ, state->cols);
    LBOUND(state->scrollregion_ଡାହାଣ, -1);
    ଯଦି(state->scrollregion_ବାମ == 0 && state->scrollregion_ଡାହାଣ == state->cols)
      state->scrollregion_ଡାହାଣ = -1;
    ଅନ୍ୟଥା
      UBOUND(state->scrollregion_ଡାହାଣ, state->cols);

    ଯଦି(state->scrollregion_ଡାହାଣ > -1 &&
       state->scrollregion_ଡାହାଣ <= state->scrollregion_ବାମ) {
      // Invalid
      state->scrollregion_ବାମ  = 0;
      state->scrollregion_ଡାହାଣ = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    ଯଦି(state->mode.origin) {
      state->pos.row += state->scrollregion_ଶୀର୍ଷ;
      state->pos.col += SCROLLREGION_ବାମ(state);
    }

    ଭାଙ୍ଗ;

  ସ୍ଥିତି 0x74:
    ବାଛ(CSI_ARG(args[0])) {
      ସ୍ଥିତି 8: // CSI 8 ; rows ; cols t  set size
        ଯଦି (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	handled = 0;
	ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି INTERMED('\'', 0x7D): // DECIC
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    ଯଦି(!is_cursor_in_scrollregion(state))
      ଭାଙ୍ଗ;

    rect.start_row = state->scrollregion_ଶୀର୍ଷ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

    scroll(state, rect, 0, -ଗଣନା_2);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି INTERMED('\'', 0x7E): // DECDC
    ଗଣନା_2 = CSI_ARG_ଗଣନା(args[0]);

    ଯଦି(!is_cursor_in_scrollregion(state))
      ଭାଙ୍ଗ;

    rect.start_row = state->scrollregion_ଶୀର୍ଷ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_ଡାହାଣ(state);

    scroll(state, rect, 0, ଗଣନା_2);

    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    handled = 0;
    ଭାଙ୍ଗ;
  }

  ଯଦି (!handled) {
    ଯଦି(state->fallbacks && state->fallbacks->csi)
      ଯଦି((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        ଫେରନ୍ତୁ 1;

    ଫେରନ୍ତୁ 0;
  }

  ଯଦି(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_ଶୀର୍ଷ);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_ବାମ(state));
    UBOUND(state->pos.col, SCROLLREGION_ଡାହାଣ(state)-1);
  }
  ଅନ୍ୟଥା {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  ଯଦି(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  ଯଦି(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ଶୀର୍ଷ) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_ଶୀର୍ଷ);
    abort();
  }

  ଯଦି(SCROLLREGION_ଡାହାଣ(state) <= SCROLLREGION_ବାମ(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_ଡାହାଣ(state), SCROLLREGION_ବାମ(state));
    abort();
  }
#endif

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅକ୍ଷର base64_one(uint8_t b)
{
  ଯଦି(b < 26)
    ଫେରନ୍ତୁ 'A' + b;
  ଅନ୍ୟଥା ଯଦି(b < 52)
    ଫେରନ୍ତୁ 'a' + b - 26;
  ଅନ୍ୟଥା ଯଦି(b < 62)
    ଫେରନ୍ତୁ '0' + b - 52;
  ଅନ୍ୟଥା ଯଦି(b == 62)
    ଫେରନ୍ତୁ '+';
  ଅନ୍ୟଥା ଯଦି(b == 63)
    ଫେରନ୍ତୁ '/';
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର uint8_t unbase64one(ଅକ୍ଷର c)
{
  ଯଦି(c >= 'A' && c <= 'Z')
    ଫେରନ୍ତୁ c - 'A';
  ଅନ୍ୟଥା ଯଦି(c >= 'a' && c <= 'z')
    ଫେରନ୍ତୁ c - 'a' + 26;
  ଅନ୍ୟଥା ଯଦି(c >= '0' && c <= '9')
    ଫେରନ୍ତୁ c - '0' + 52;
  ଅନ୍ୟଥା ଯଦି(c == '+')
    ଫେରନ୍ତୁ 62;
  ଅନ୍ୟଥା ଯଦି(c == '/')
    ଫେରନ୍ତୁ 63;

  ଫେରନ୍ତୁ 0xFF;
}

ସ୍ଥିର ଶୂନ୍ୟ osc_selection(VTermState *state, VTermStringFragment frag)
{
  ଯଦି(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  ଯେତେବେଳେ(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    ବାଛ(frag.str[0]) {
      ସ୍ଥିତି 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି '0':
      ସ୍ଥିତି '1':
      ସ୍ଥିତି '2':
      ସ୍ଥିତି '3':
      ସ୍ଥିତି '4':
      ସ୍ଥିତି '5':
      ସ୍ଥିତି '6':
      ସ୍ଥିତି '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        ଭାଙ୍ଗ;

      ସ୍ଥିତି ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        ଯଦି(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        ଭାଙ୍ଗ;
    }

    frag.str++;
    frag.len--;
  }

  ଯଦି(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    ଯଦି(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = ଖାଲି,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    ଫେରନ୍ତୁ;
  }

  ଯଦି(state->tmp.selection.state == SELECTION_SELECTED) {
    ଯଦି(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    ଅନ୍ୟଥା {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  ଯଦି(state->tmp.selection.state == SELECTION_QUERY) {
    ଯଦି(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    ଫେରନ୍ତୁ;
  }

  ଯଦି(state->tmp.selection.state == SELECTION_INVALID)
    ଫେରନ୍ତୁ;

  ଯଦି(state->selection.callbacks->set) {
    size_t bufcur = 0;
    ଅକ୍ଷର *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;      /* Number of sextets consumed */

    ଯଦି(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    ଯେତେବେଳେ((state->selection.buflen - bufcur) >= 3 && frag.len) {
      ଯଦି(frag.str[0] == '=') {
        ଯଦି(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        ଯଦି(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        ଯେତେବେଳେ(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      ଅନ୍ୟଥା {
        uint8_t b = unbase64one(frag.str[0]);
        ଯଦି(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          ଯଦି(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = ଖାଲି,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          ଭାଙ୍ଗ;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        ଯଦି(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      ଯଦି(!frag.len || (state->selection.buflen - bufcur) < 3) {
        ଯଦି(bufcur) {
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

    ଯଦି(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_osc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା command, VTermStringFragment frag, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;

  ବାଛ(command) {
    ସ୍ଥିତି 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ଫେରନ୍ତୁ 1;

    ସ୍ଥିତି 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      ଫେରନ୍ତୁ 1;

    ସ୍ଥିତି 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ଫେରନ୍ତୁ 1;

    ସ୍ଥିତି 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା red = state->default_fg.red;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା blue = state->default_fg.blue;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା green = state->default_fg.green;
        vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        ଫେରନ୍ତୁ 1;
      }

    ସ୍ଥିତି 11:
      {
	// request background color: <Esc>]11;?<0x07>
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା red = state->default_bg.red;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା blue = state->default_bg.blue;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା green = state->default_bg.green;
	vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	ଫେରନ୍ତୁ 1;
      }
    ସ୍ଥିତି 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      ଫେରନ୍ତୁ 1;

    ସ୍ଥିତି 52:
      ଯଦି(state->selection.callbacks)
        osc_selection(state, frag);

      ଫେରନ୍ତୁ 1;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଯଦି(state->fallbacks && state->fallbacks->osc)
        ଯଦି((*state->fallbacks->osc)(command, frag, state->fbdata))
          ଫେରନ୍ତୁ 1;
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଶୂନ୍ୟ request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ଅକ୍ଷର *tmp = state->tmp.decrqss;

  ଯଦି(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  ଯେତେବେଳେ(i < ଆକାର(state->tmp.decrqss)-1 && tmp[i])
    i++;
  ଯେତେବେଳେ(i < ଆକାର(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  ଯଦି(!frag.final)
    ଫେରନ୍ତୁ;

  ବାଛ(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    ସ୍ଥିତି 'm': {
      // Query SGR
      long args[20];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc = vterm_state_getpen(state, args, ଆକାର(args)/ଆକାର(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      ଯଦି(cur >= vt->tmpbuffer_len)
        ଫେରନ୍ତୁ;

      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        ଯଦି(cur >= vt->tmpbuffer_len)
          ଫେରନ୍ତୁ;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      ଯଦି(cur >= vt->tmpbuffer_len)
        ଫେରନ୍ତୁ;

      vterm_ଠେଲ_output_bytes(vt, vt->tmpbuffer, cur);
      ଫେରନ୍ତୁ;
    }

    ସ୍ଥିତି 'r':
      // Query DECSTBM
      vterm_ଠେଲ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_ଶୀର୍ଷ+1, SCROLLREGION_BOTTOM(state));
      ଫେରନ୍ତୁ;

    ସ୍ଥିତି 's':
      // Query DECSLRM
      vterm_ଠେଲ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_ବାମ(state)+1, SCROLLREGION_ଡାହାଣ(state));
      ଫେରନ୍ତୁ;

    ସ୍ଥିତି ' '|('q'<<8): {
      // Query DECSCUSR
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା reply;
      ବାଛ(state->mode.cursor_shape) {
        ସ୍ଥିତି VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; ଭାଙ୍ଗ;
        ସ୍ଥିତି VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; ଭାଙ୍ଗ;
      }
      ଯଦି(state->mode.cursor_blink)
        reply--;
      vterm_ଠେଲ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      ଫେରନ୍ତୁ;
    }

    ସ୍ଥିତି '\"'|('q'<<8):
      // Query DECSCA
      vterm_ଠେଲ_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      ଫେରନ୍ତୁ;
  }

  vterm_ଠେଲ_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_dcs(ଅଚଳ ଅକ୍ଷର *command, size_t commandlen, VTermStringFragment frag, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;

  ଯଦି(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    ଫେରନ୍ତୁ 1;
  }
  ଅନ୍ୟଥା ଯଦି(state->fallbacks && state->fallbacks->dcs)
    ଯଦି((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      ଫେରନ୍ତୁ 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)commandlen, command);
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_apc(VTermStringFragment frag, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;

  ଯଦି(state->fallbacks && state->fallbacks->apc)
    ଯଦି((*state->fallbacks->apc)(frag, state->fbdata))
      ଫେରନ୍ତୁ 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_pm(VTermStringFragment frag, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;

  ଯଦି(state->fallbacks && state->fallbacks->pm)
    ଯଦି((*state->fallbacks->pm)(frag, state->fbdata))
      ଫେରନ୍ତୁ 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_sos(VTermStringFragment frag, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;

  ଯଦି(state->fallbacks && state->fallbacks->sos)
    ଯଦି((*state->fallbacks->sos)(frag, state->fbdata))
      ଫେରନ୍ତୁ 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା on_resize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols, ଶୂନ୍ୟ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  ଯଦି (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  ଯଦି (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  ଯଦି(cols != state->cols) {
    unsigned ଅକ୍ଷର *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    ଯଦି (newtabstops == ଖାଲି)
      ଫେରନ୍ତୁ 0;

    /* TODO: This can all be done much more efficiently bytewise */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;
    ପାଇଁ(col = 0; col < state->cols && col < cols; col++) {
      unsigned ଅକ୍ଷର mask = 1 << (col & 7);
      ଯଦି(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      ଅନ୍ୟଥା
        newtabstops[col >> 3] &= ~mask;
      }

    ପାଇଁ( ; col < cols; col++) {
      unsigned ଅକ୍ଷର mask = 1 << (col & 7);
      ଯଦି(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      ଅନ୍ୟଥା
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  ଯଦି(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  ଯଦି(state->scrollregion_ଡାହାଣ > -1)
    UBOUND(state->scrollregion_ଡାହାଣ, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_ଶୀର୍ଷ, state->rows);
  UBOUND(state->scrollregion_ବାମ, state->cols);
  ଯଦି(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ଶୀର୍ଷ) {
    state->scrollregion_ଶୀର୍ଷ = 0;
    state->scrollregion_bottom = -1;
  }
  ଯଦି(SCROLLREGION_ଡାହାଣ(state) <= state->scrollregion_ବାମ) {
    state->scrollregion_ବାମ = 0;
    state->scrollregion_ଡାହାଣ = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  ଯଦି(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  ଅନ୍ୟଥା {
    ଯଦି(rows != state->rows) {
      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_ଧାଡ଼ି_Info *oldlineinfo = state->lineinfos[bufidx];
        ଯଦି(!oldlineinfo)
          ଜାରି;

        VTerm_ଧାଡ଼ି_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * ଆକାର(VTerm_ଧାଡ଼ି_Info));

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା row;
        ପାଇଁ(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        ପାଇଁ( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_ଧାଡ଼ି_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  ଯଦି(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  ଯଦି(state->pos.row < 0)
    state->pos.row = 0;
  ଯଦି(state->pos.row >= rows)
    state->pos.row = rows - 1;
  ଯଦି(state->pos.col < 0)
    state->pos.col = 0;
  ଯଦି(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ VTermParserCallbacks parser_callbacks = {
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
  ଯଦି(vt->state)
    ଫେରନ୍ତୁ vt->state;

  VTermState *state = vterm_state_new(vt);
  ଯଦି (state == ଖାଲି)
    ଫେରନ୍ତୁ ଖାଲି;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  ଫେରନ୍ତୁ state;
}

ଶୂନ୍ୟ vterm_state_reset(VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hard)
{
  state->scrollregion_ଶୀର୍ଷ = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_ବାମ = 0;
  state->scrollregion_ଡାହାଣ = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.ଭର୍ତ୍ତି          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0; col < state->cols; col++)
    ଯଦି(col % 8 == 0)
      set_col_tabstop(state, col);
    ଅନ୍ୟଥା
      clear_col_tabstop(state, col);

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  ଯଦି(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    ଯଦି(default_enc->init)
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

  ଯଦି(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

ଶୂନ୍ୟ vterm_state_get_cursorpos(ଅଚଳ VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

ଶୂନ୍ୟ vterm_state_get_mousestate(ଅଚଳ VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

ଶୂନ୍ୟ vterm_state_set_callbacks(VTermState *state, ଅଚଳ VTermStateCallbacks *callbacks, ଶୂନ୍ୟ *user)
{
  ଯଦି(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    ଯଦି(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  ଅନ୍ୟଥା {
    state->callbacks = ଖାଲି;
    state->cbdata = ଖାଲି;
  }
}

ଶୂନ୍ୟ vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

ଶୂନ୍ୟ *vterm_state_get_cbdata(VTermState *state)
{
  ଫେରନ୍ତୁ state->cbdata;
}

ଶୂନ୍ୟ vterm_state_set_unrecognised_fallbacks(VTermState *state, ଅଚଳ VTermStateFallbacks *fallbacks, ଶୂନ୍ୟ *user)
{
  ଯଦି(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  ଅନ୍ୟଥା {
    state->fallbacks = ଖାଲି;
    state->fbdata = ଖାଲି;
  }
}

ଶୂନ୍ୟ *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  ଫେରନ୍ତୁ state->fbdata;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_ମୂଲ୍ୟ *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  ଯଦି(state->callbacks && state->callbacks->settermprop)
    ଯଦି(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      ଫେରନ୍ତୁ 0;

  ବାଛ(prop) {
  ସ୍ଥିତି VTERM_PROP_TITLE:
  ସ୍ଥିତି VTERM_PROP_ICONNAME:
  ସ୍ଥିତି VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->ସଂଖ୍ୟା;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    ଯଦି(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    ଯଦି(val->ସଂଖ୍ୟା)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    ଯଦି(val->ସଂଖ୍ୟା == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    ଯଦି(val->ସଂଖ୍ୟା == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_N_PROPS:
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ vterm_state_focus_in(VTermState *state)
{
  ଯଦି(state->mode.report_focus)
    vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

ଶୂନ୍ୟ vterm_state_focus_out(VTermState *state)
{
  ଯଦି(state->mode.report_focus)
    vterm_ଠେଲ_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ଅଚଳ VTerm_ଧାଡ଼ି_Info *vterm_state_get_lineinfo(ଅଚଳ VTermState *state, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row)
{
  ଫେରନ୍ତୁ state->lineinfo + row;
}

ଶୂନ୍ୟ vterm_state_set_selection_callbacks(VTermState *state, ଅଚଳ VTermSelectionCallbacks *callbacks, ଶୂନ୍ୟ *user,
    ଅକ୍ଷର *buffer, size_t buflen)
{
  ଯଦି(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

ଶୂନ୍ୟ vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ଯଦି(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ସ୍ଥିର ଅକ୍ଷର selection_chars[] = "cpqs";
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
    ପାଇଁ(idx = 0; idx < 4; idx++)
      ଯଦି(mask & (1 << idx))
        ଭାଙ୍ଗ;

    vterm_ଠେଲ_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  ଯଦି(frag.len) {
    size_t bufcur = 0;
    ଅକ୍ଷର *buffer = state->selection.buffer;

    uint32_t x = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;

    ଯଦି(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    ଯେତେବେଳେ((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      ଯଦି(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      ଯଦି(!frag.len || (state->selection.buflen - bufcur) < 4) {
        ଯଦି(bufcur)
          vterm_ଠେଲ_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    ଯଦି(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  ଯଦି(frag.final) {
    ଯଦି(state->tmp.selection.sendpartial) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      ଅକ୍ଷର *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_ଠେଲ_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    ଅନ୍ୟଥା
      vterm_ଠେଲ_output_sprintf_str(vt, 0, TRUE, "");
  }
}
