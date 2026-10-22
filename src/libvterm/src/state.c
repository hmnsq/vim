#അന്തര്ഭാവ "vterm_internal.h"

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#എങ്കിൽ defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ on_resize(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, ശൂന്യം *user);

/* Some convenient wrappers to make callback functions easier */

സ്ഥിതം ശൂന്യം putglyph(VTermState *state, സ്ഥിരം uint32_t chars[], പൂർണ്ണസംഖ്യ width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  എങ്കിൽ(state->callbacks && state->callbacks->putglyph)
    എങ്കിൽ((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      തിരികെ;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

സ്ഥിതം ശൂന്യം updatecursor(VTermState *state, VTermPos *oldpos, പൂർണ്ണസംഖ്യ cancel_phantom)
{
  എങ്കിൽ(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    തിരികെ;

  എങ്കിൽ(cancel_phantom)
    state->at_phantom = 0;

  എങ്കിൽ(state->callbacks && state->callbacks->movecursor)
    എങ്കിൽ((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      തിരികെ;
}

സ്ഥിതം ശൂന്യം erase(VTermState *state, VTermRect rect, പൂർണ്ണസംഖ്യ selective)
{
  എങ്കിൽ(rect.end_col == state->cols) {
    പൂർണ്ണസംഖ്യ row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    വേണ്ടി(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  എങ്കിൽ(state->callbacks && state->callbacks->erase)
    എങ്കിൽ((*state->callbacks->erase)(rect, selective, state->cbdata))
      തിരികെ;
}

സ്ഥിതം VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, വലുപ്പം(VTermState));

  എങ്കിൽ (state == ശൂന്യ)
    തിരികെ ശൂന്യ;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ശൂന്യ;
  state->cbdata    = ശൂന്യ;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ശൂന്യ;
  state->selection.user      = ശൂന്യ;
  state->selection.buffer    = ശൂന്യ;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * വലുപ്പം(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * വലുപ്പം(VTerm_വരി_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * വലുപ്പം(VTerm_വരി_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  എങ്കിൽ(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  തിരികെ state;
}

INTERNAL ശൂന്യം vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  എങ്കിൽ(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

സ്ഥിതം ശൂന്യം scroll(VTermState *state, VTermRect rect, പൂർണ്ണസംഖ്യ downward, പൂർണ്ണസംഖ്യ rightward)
{
  പൂർണ്ണസംഖ്യ rows;
  പൂർണ്ണസംഖ്യ cols;
  എങ്കിൽ(!downward && !rightward)
    തിരികെ;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  എങ്കിൽ(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    തിരികെ;

  rows = rect.end_row - rect.start_row;
  എങ്കിൽ(downward > rows)
    downward = rows;
  അല്ലെങ്കിൽ എങ്കിൽ(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  എങ്കിൽ(rightward > cols)
    rightward = cols;
  അല്ലെങ്കിൽ എങ്കിൽ(rightward < -cols)
    rightward = -cols;

  എങ്കിൽ(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    എങ്കിൽ(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    അല്ലെങ്കിൽ എങ്കിൽ(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    എങ്കിൽ(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    അല്ലെങ്കിൽ എങ്കിൽ(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  എങ്കിൽ(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    പൂർണ്ണസംഖ്യ height = rect.end_row - rect.start_row - abs(downward);
    പൂർണ്ണസംഖ്യ row;
    VTerm_വരി_Info zero_വരി_Info = {0x0};

    എങ്കിൽ(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * വലുപ്പം(state->lineinfo[0]));
      വേണ്ടി(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_വരി_Info;
    }
    അല്ലെങ്കിൽ {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * വലുപ്പം(state->lineinfo[0]));
      വേണ്ടി(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_വരി_Info;
    }
  }

  എങ്കിൽ(state->callbacks && state->callbacks->scrollrect)
    എങ്കിൽ((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      തിരികെ;

  എങ്കിൽ(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

സ്ഥിതം ശൂന്യം linefeed(VTermState *state)
{
  എങ്കിൽ(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_മുകൾ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ഇടത്(state);
    rect.end_col   = SCROLLREGION_വലത്(state);

    scroll(state, rect, 1, 0);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(state->pos.row < state->rows-1)
    state->pos.row++;
}

സ്ഥിതം ശൂന്യം grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * വലുപ്പം(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * വലുപ്പം(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

സ്ഥിതം ശൂന്യം set_col_tabstop(VTermState *state, പൂർണ്ണസംഖ്യ col)
{
  unsigned അക്ഷരം mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

സ്ഥിതം ശൂന്യം clear_col_tabstop(VTermState *state, പൂർണ്ണസംഖ്യ col)
{
  unsigned അക്ഷരം mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ is_col_tabstop(VTermState *state, പൂർണ്ണസംഖ്യ col)
{
  unsigned അക്ഷരം mask = 1 << (col & 7);
  തിരികെ state->tabstops[col >> 3] & mask;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ is_cursor_in_scrollregion(സ്ഥിരം VTermState *state)
{
  എങ്കിൽ(state->pos.row < state->scrollregion_മുകൾ ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    തിരികെ 0;
  എങ്കിൽ(state->pos.col < SCROLLREGION_ഇടത്(state) ||
     state->pos.col >= SCROLLREGION_വലത്(state))
    തിരികെ 0;

  തിരികെ 1;
}

സ്ഥിതം ശൂന്യം tab(VTermState *state, പൂർണ്ണസംഖ്യ എണ്ണം, പൂർണ്ണസംഖ്യ direction)
{
  ആകുമ്പോൾ(എണ്ണം > 0) {
    എങ്കിൽ(direction > 0) {
      എങ്കിൽ(state->pos.col >= THISROWWIDTH(state)-1)
        തിരികെ;

      state->pos.col++;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(direction < 0) {
      എങ്കിൽ(state->pos.col < 1)
        തിരികെ;

      state->pos.col--;
    }

    എങ്കിൽ(is_col_tabstop(state, state->pos.col))
      എണ്ണം--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_മുകൾ    1
#define DHL_BOTTOM 2

സ്ഥിതം ശൂന്യം set_lineinfo(VTermState *state, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ force, പൂർണ്ണസംഖ്യ dwl, പൂർണ്ണസംഖ്യ dhl)
{
  VTerm_വരി_Info info = state->lineinfo[row];

  എങ്കിൽ(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  അല്ലെങ്കിൽ എങ്കിൽ(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  എങ്കിൽ(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  അല്ലെങ്കിൽ എങ്കിൽ(dhl == DHL_മുകൾ)
    info.doubleheight = DHL_മുകൾ;
  അല്ലെങ്കിൽ എങ്കിൽ(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  എങ്കിൽ((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_text(സ്ഥിരം അക്ഷരം bytes[], size_t len, ശൂന്യം *user)
{
  VTermState *state = user;
  പൂർണ്ണസംഖ്യ npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  പൂർണ്ണസംഖ്യ i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / വലുപ്പം(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  എങ്കിൽ (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (പൂർണ്ണസംഖ്യ)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  എങ്കിൽ(!npoints)
  {
    തിരികെ (പൂർണ്ണസംഖ്യ)eaten;
  }

  എങ്കിൽ(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  എങ്കിൽ(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    എങ്കിൽ(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      പൂർണ്ണസംഖ്യ printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      വേണ്ടി(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      പൂർണ്ണസംഖ്യ saved_i = 0;
      ആകുമ്പോൾ(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      ആകുമ്പോൾ(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        എങ്കിൽ(saved_i >= (പൂർണ്ണസംഖ്യ)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      എങ്കിൽ(saved_i >= (പൂർണ്ണസംഖ്യ)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      വേണ്ടി(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    അല്ലെങ്കിൽ {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  വേണ്ടി(; i < npoints; i++) {
    // Try to find combining characters following this
    പൂർണ്ണസംഖ്യ glyph_starts = i;
    പൂർണ്ണസംഖ്യ glyph_ends;
    പൂർണ്ണസംഖ്യ width = 0;

    വേണ്ടി(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      എങ്കിൽ(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        നിർത്തുക;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * വലുപ്പം(uint32_t));
    എങ്കിൽ (chars == ശൂന്യ)
      നിർത്തുക;

    വേണ്ടി( ; i < glyph_ends; i++) {
      പൂർണ്ണസംഖ്യ this_width;
      എങ്കിൽ(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        എങ്കിൽ(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      എങ്കിൽ(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      എങ്കിൽ (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    എങ്കിൽ (width < 0)
      width = 0;

    ആകുമ്പോൾ(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    പൂർണ്ണസംഖ്യ printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    വേണ്ടി(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    എങ്കിൽ(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    എങ്കിൽ(state->mode.തിരുകുക) {
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

    എങ്കിൽ(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      പൂർണ്ണസംഖ്യ save_i;
      വേണ്ടി(save_i = 0; chars[save_i]; save_i++) {
        എങ്കിൽ(save_i >= (പൂർണ്ണസംഖ്യ)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      എങ്കിൽ(save_i >= (പൂർണ്ണസംഖ്യ)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    എങ്കിൽ(state->pos.col + width >= THISROWWIDTH(state)) {
      എങ്കിൽ(state->mode.autowrap)
        state->at_phantom = 1;
    }
    അല്ലെങ്കിൽ {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  എങ്കിൽ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  തിരികെ (പൂർണ്ണസംഖ്യ)eaten;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_control(unsigned അക്ഷരം control, ശൂന്യം *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  തിരഞ്ഞെടുപ്പ്(control) {
  അവസ്ഥ 0x07: // BEL - ECMA-48 8.3.3
    എങ്കിൽ(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    നിർത്തുക;

  അവസ്ഥ 0x08: // BS - ECMA-48 8.3.5
    എങ്കിൽ(state->pos.col > 0)
      state->pos.col--;
    എങ്കിൽ(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      എങ്കിൽ(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    നിർത്തുക;

  അവസ്ഥ 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    നിർത്തുക;

  അവസ്ഥ 0x0a: // LF - ECMA-48 8.3.74
  അവസ്ഥ 0x0b: // VT
  അവസ്ഥ 0x0c: // FF
    linefeed(state);
    എങ്കിൽ(state->mode.newline)
      state->pos.col = 0;
    നിർത്തുക;

  അവസ്ഥ 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    നിർത്തുക;

  അവസ്ഥ 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    നിർത്തുക;

  അവസ്ഥ 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    നിർത്തുക;

  അവസ്ഥ 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    നിർത്തുക;

  അവസ്ഥ 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    നിർത്തുക;

  അവസ്ഥ 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    നിർത്തുക;

  അവസ്ഥ 0x8d: // RI - ECMA-48 8.3.104
    എങ്കിൽ(state->pos.row == state->scrollregion_മുകൾ) {
      VTermRect rect;
      rect.start_row = state->scrollregion_മുകൾ;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_ഇടത്(state);
      rect.end_col   = SCROLLREGION_വലത്(state);

      scroll(state, rect, -1, 0);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(state->pos.row > 0)
        state->pos.row--;
    നിർത്തുക;

  അവസ്ഥ 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    നിർത്തുക;

  അവസ്ഥ 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    എങ്കിൽ(state->fallbacks && state->fallbacks->control)
      എങ്കിൽ((*state->fallbacks->control)(control, state->fbdata))
        തിരികെ 1;

    തിരികെ 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  എങ്കിൽ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  തിരികെ 1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ settermprop_bool(VTermState *state, VTermProp prop, പൂർണ്ണസംഖ്യ v)
{
  VTerm_മൂല്യം val;
  val.boolean = v;
  തിരികെ vterm_state_set_termprop(state, prop, &val);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ settermprop_int(VTermState *state, VTermProp prop, പൂർണ്ണസംഖ്യ v)
{
  VTerm_മൂല്യം val;
  val.സംഖ്യ = v;
  തിരികെ vterm_state_set_termprop(state, prop, &val);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_മൂല്യം val;

  val.string = frag;
  തിരികെ vterm_state_set_termprop(state, prop, &val);
}

സ്ഥിതം ശൂന്യം savecursor(VTermState *state, പൂർണ്ണസംഖ്യ save)
{
  എങ്കിൽ(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  അല്ലെങ്കിൽ {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_escape(സ്ഥിരം അക്ഷരം *bytes, size_t len, ശൂന്യം *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  തിരഞ്ഞെടുപ്പ്(bytes[0]) {
  അവസ്ഥ ' ':
    എങ്കിൽ(len != 2)
      തിരികെ 0;

    തിരഞ്ഞെടുപ്പ്(bytes[1]) {
      അവസ്ഥ 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        നിർത്തുക;

      അവസ്ഥ 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        നിർത്തുക;

      സ്ഥിരസ്ഥിതി:
        തിരികെ 0;
    }
    തിരികെ 2;

  അവസ്ഥ '#':
    എങ്കിൽ(len != 2)
      തിരികെ 0;

    തിരഞ്ഞെടുപ്പ്(bytes[1]) {
      അവസ്ഥ '3': // DECDHL top
        എങ്കിൽ(state->mode.leftrightmargin)
          നിർത്തുക;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_മുകൾ);
        നിർത്തുക;

      അവസ്ഥ '4': // DECDHL bottom
        എങ്കിൽ(state->mode.leftrightmargin)
          നിർത്തുക;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        നിർത്തുക;

      അവസ്ഥ '5': // DECSWL
        എങ്കിൽ(state->mode.leftrightmargin)
          നിർത്തുക;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        നിർത്തുക;

      അവസ്ഥ '6': // DECDWL
        എങ്കിൽ(state->mode.leftrightmargin)
          നിർത്തുക;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        നിർത്തുക;

      അവസ്ഥ '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        വേണ്ടി(pos.row = 0; pos.row < state->rows; pos.row++)
          വേണ്ടി(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        നിർത്തുക;
      }

      സ്ഥിരസ്ഥിതി:
        തിരികെ 0;
    }
    തിരികെ 2;

  അവസ്ഥ '(': അവസ്ഥ ')': അവസ്ഥ '*': അവസ്ഥ '+': // SCS
    എങ്കിൽ(len != 2)
      തിരികെ 0;

    {
      പൂർണ്ണസംഖ്യ setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      എങ്കിൽ(newenc) {
        state->encoding[setnum].enc = newenc;

        എങ്കിൽ(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    തിരികെ 2;

  അവസ്ഥ '7': // DECSC
    savecursor(state, 1);
    തിരികെ 1;

  അവസ്ഥ '8': // DECRC
    savecursor(state, 0);
    തിരികെ 1;

  അവസ്ഥ '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    തിരികെ 1;

  അവസ്ഥ '=': // DECKPAM
    state->mode.keypad = 1;
    തിരികെ 1;

  അവസ്ഥ '>': // DECKPNM
    state->mode.keypad = 0;
    തിരികെ 1;

  അവസ്ഥ 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    എങ്കിൽ(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    തിരികെ 1;
  }

  അവസ്ഥ 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    തിരികെ 1;

  അവസ്ഥ 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    തിരികെ 1;

  അവസ്ഥ '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    തിരികെ 1;

  അവസ്ഥ '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    തിരികെ 1;

  അവസ്ഥ '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    തിരികെ 1;

  സ്ഥിരസ്ഥിതി:
    തിരികെ 0;
  }
}

സ്ഥിതം ശൂന്യം set_mode(VTermState *state, പൂർണ്ണസംഖ്യ അക്കം, പൂർണ്ണസംഖ്യ val)
{
  തിരഞ്ഞെടുപ്പ്(അക്കം) {
  അവസ്ഥ 4: // IRM - ECMA-48 7.2.10
    state->mode.തിരുകുക = val;
    നിർത്തുക;

  അവസ്ഥ 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", അക്കം);
    തിരികെ;
  }
}

സ്ഥിതം ശൂന്യം set_dec_mode(VTermState *state, പൂർണ്ണസംഖ്യ അക്കം, പൂർണ്ണസംഖ്യ val)
{
  തിരഞ്ഞെടുപ്പ്(അക്കം) {
  അവസ്ഥ 1:
    state->mode.cursor = val;
    നിർത്തുക;

  അവസ്ഥ 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    നിർത്തുക;

  അവസ്ഥ 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_മുകൾ : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_ഇടത്(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    നിർത്തുക;

  അവസ്ഥ 7:
    state->mode.autowrap = val;
    നിർത്തുക;

  അവസ്ഥ 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    നിർത്തുക;

  അവസ്ഥ 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    നിർത്തുക;

  അവസ്ഥ 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    എങ്കിൽ(val) {
      പൂർണ്ണസംഖ്യ row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      വേണ്ടി(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    നിർത്തുക;

  അവസ്ഥ 1000:
  അവസ്ഥ 1002:
  അവസ്ഥ 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (അക്കം == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (അക്കം == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    നിർത്തുക;

  അവസ്ഥ 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    നിർത്തുക;

  അവസ്ഥ 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    നിർത്തുക;

  അവസ്ഥ 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    നിർത്തുക;

  അവസ്ഥ 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    നിർത്തുക;

  അവസ്ഥ 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    നിർത്തുക;

  അവസ്ഥ 1048:
    savecursor(state, val);
    നിർത്തുക;

  അവസ്ഥ 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    നിർത്തുക;

  അവസ്ഥ 2004:
    state->mode.bracketpaste = val;
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", അക്കം);
    തിരികെ;
  }
}

സ്ഥിതം ശൂന്യം request_dec_mode(VTermState *state, പൂർണ്ണസംഖ്യ അക്കം)
{
  പൂർണ്ണസംഖ്യ reply;

  തിരഞ്ഞെടുപ്പ്(അക്കം) {
    അവസ്ഥ 1:
      reply = state->mode.cursor;
      നിർത്തുക;

    അവസ്ഥ 5:
      reply = state->mode.screen;
      നിർത്തുക;

    അവസ്ഥ 6:
      reply = state->mode.origin;
      നിർത്തുക;

    അവസ്ഥ 7:
      reply = state->mode.autowrap;
      നിർത്തുക;

    അവസ്ഥ 12:
      reply = state->mode.cursor_blink;
      നിർത്തുക;

    അവസ്ഥ 25:
      reply = state->mode.cursor_visible;
      നിർത്തുക;

    അവസ്ഥ 69:
      reply = state->mode.leftrightmargin;
      നിർത്തുക;

    അവസ്ഥ 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      നിർത്തുക;

    അവസ്ഥ 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      നിർത്തുക;

    അവസ്ഥ 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      നിർത്തുക;

    അവസ്ഥ 1004:
      reply = state->mode.report_focus;
      നിർത്തുക;

    അവസ്ഥ 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      നിർത്തുക;

    അവസ്ഥ 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      നിർത്തുക;

    അവസ്ഥ 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      നിർത്തുക;

    അവസ്ഥ 1047:
      reply = state->mode.alt_screen;
      നിർത്തുക;

    അവസ്ഥ 2004:
      reply = state->mode.bracketpaste;
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", അക്കം, 0);
      തിരികെ;
  }

  vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", അക്കം, reply ? 1 : 2);
}

സ്ഥിതം ശൂന്യം request_version_string(VTermState *state)
{
  vterm_തള്ളുക_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_csi(സ്ഥിരം അക്ഷരം *leader, സ്ഥിരം long args[], പൂർണ്ണസംഖ്യ argcount, സ്ഥിരം അക്ഷരം *intermed, അക്ഷരം command, ശൂന്യം *user)
{
  VTermState *state = user;
  പൂർണ്ണസംഖ്യ leader_byte = 0;
  പൂർണ്ണസംഖ്യ intermed_byte = 0;
  പൂർണ്ണസംഖ്യ cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  പൂർണ്ണസംഖ്യ handled = 1;

  // Some temporaries for later code
  പൂർണ്ണസംഖ്യ എണ്ണം, val;
  പൂർണ്ണസംഖ്യ row, col;
  VTermRect rect;
  പൂർണ്ണസംഖ്യ selective;

  എങ്കിൽ(leader && leader[0]) {
    എങ്കിൽ(leader[1]) // longer than 1 char
      തിരികെ 0;

    തിരഞ്ഞെടുപ്പ്(leader[0]) {
    അവസ്ഥ '?':
    അവസ്ഥ '>':
    അവസ്ഥ '<':
      leader_byte = leader[0];
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      തിരികെ 0;
    }
  }

  എങ്കിൽ(intermed && intermed[0]) {
    എങ്കിൽ(intermed[1]) // longer than 1 char
      തിരികെ 0;

    തിരഞ്ഞെടുപ്പ്(intermed[0]) {
    അവസ്ഥ ' ':
    അവസ്ഥ '!':
    അവസ്ഥ '"':
    അവസ്ഥ '$':
    അവസ്ഥ '\'':
      intermed_byte = intermed[0];
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      തിരികെ 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) എങ്കിൽ((v) < (min)) (v) = (min)
#define UBOUND(v,max) എങ്കിൽ((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  തിരഞ്ഞെടുപ്പ്(intermed_byte << 16 | leader_byte << 8 | command) {
  അവസ്ഥ 0x40: // ICH - ECMA-48 8.3.64
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    എങ്കിൽ(!is_cursor_in_scrollregion(state))
      നിർത്തുക;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    എങ്കിൽ(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_വലത്(state);
    അല്ലെങ്കിൽ
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -എണ്ണം);

    നിർത്തുക;

  അവസ്ഥ 0x41: // CUU - ECMA-48 8.3.22
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.row -= എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x42: // CUD - ECMA-48 8.3.19
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.row += എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x43: // CUF - ECMA-48 8.3.20
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.col += എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x44: // CUB - ECMA-48 8.3.18
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.col -= എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x45: // CNL - ECMA-48 8.3.12
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.col = 0;
    state->pos.row += എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x46: // CPL - ECMA-48 8.3.13
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.col = 0;
    state->pos.row -= എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    എങ്കിൽ(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      എങ്കിൽ(state->pos.row == row - 1) {
        പൂർണ്ണസംഖ്യ cnt, ptr = 0;
        വേണ്ടി(cnt = 0; cnt < col - 1; ++cnt) {
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
    എങ്കിൽ(state->mode.origin) {
      state->pos.row += state->scrollregion_മുകൾ;
      state->pos.col += SCROLLREGION_ഇടത്(state);
    }
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x49: // CHT - ECMA-48 8.3.10
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    tab(state, എണ്ണം, +1);
    നിർത്തുക;

  അവസ്ഥ 0x4a: // ED - ECMA-48 8.3.39
  അവസ്ഥ LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    തിരഞ്ഞെടുപ്പ്(CSI_ARG(args[0])) {
    അവസ്ഥ CSI_ARG_MISSING:
    അവസ്ഥ 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      എങ്കിൽ(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      വേണ്ടി(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      എങ്കിൽ(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      നിർത്തുക;

    അവസ്ഥ 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      വേണ്ടി(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      എങ്കിൽ(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      എങ്കിൽ(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      നിർത്തുക;

    അവസ്ഥ 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      വേണ്ടി(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      നിർത്തുക;

    അവസ്ഥ 3:
      എങ്കിൽ(state->callbacks && state->callbacks->sb_clear)
        എങ്കിൽ((*state->callbacks->sb_clear)(state->cbdata))
          തിരികെ 1;
      നിർത്തുക;
    }
    നിർത്തുക;

  അവസ്ഥ 0x4b: // EL - ECMA-48 8.3.41
  അവസ്ഥ LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    തിരഞ്ഞെടുപ്പ്(CSI_ARG(args[0])) {
    അവസ്ഥ CSI_ARG_MISSING:
    അവസ്ഥ 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); നിർത്തുക;
    അവസ്ഥ 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; നിർത്തുക;
    അവസ്ഥ 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      തിരികെ 0;
    }

    എങ്കിൽ(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    നിർത്തുക;

  അവസ്ഥ 0x4c: // IL - ECMA-48 8.3.67
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    എങ്കിൽ(!is_cursor_in_scrollregion(state))
      നിർത്തുക;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ഇടത്(state);
    rect.end_col   = SCROLLREGION_വലത്(state);

    scroll(state, rect, -എണ്ണം, 0);

    നിർത്തുക;

  അവസ്ഥ 0x4d: // DL - ECMA-48 8.3.32
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    എങ്കിൽ(!is_cursor_in_scrollregion(state))
      നിർത്തുക;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ഇടത്(state);
    rect.end_col   = SCROLLREGION_വലത്(state);

    scroll(state, rect, എണ്ണം, 0);

    നിർത്തുക;

  അവസ്ഥ 0x50: // DCH - ECMA-48 8.3.26
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    എങ്കിൽ(!is_cursor_in_scrollregion(state))
      നിർത്തുക;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    എങ്കിൽ(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_വലത്(state);
    അല്ലെങ്കിൽ
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, എണ്ണം);

    നിർത്തുക;

  അവസ്ഥ 0x53: // SU - ECMA-48 8.3.147
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    rect.start_row = state->scrollregion_മുകൾ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ഇടത്(state);
    rect.end_col   = SCROLLREGION_വലത്(state);

    scroll(state, rect, എണ്ണം, 0);

    നിർത്തുക;

  അവസ്ഥ 0x54: // SD - ECMA-48 8.3.113
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    rect.start_row = state->scrollregion_മുകൾ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ഇടത്(state);
    rect.end_col   = SCROLLREGION_വലത്(state);

    scroll(state, rect, -എണ്ണം, 0);

    നിർത്തുക;

  അവസ്ഥ 0x58: // ECH - ECMA-48 8.3.38
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + എണ്ണം;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    നിർത്തുക;

  അവസ്ഥ 0x5a: // CBT - ECMA-48 8.3.7
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    tab(state, എണ്ണം, -1);
    നിർത്തുക;

  അവസ്ഥ 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x61: // HPR - ECMA-48 8.3.59
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.col += എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x62: { // REP - ECMA-48 8.3.103
    സ്ഥിരം പൂർണ്ണസംഖ്യ row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    എങ്കിൽ(state->combine_width < 1)
      നിർത്തുക;

    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    col = state->pos.col + എണ്ണം;
    UBOUND(col, row_width);
    ആകുമ്പോൾ (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    എങ്കിൽ (state->pos.col + state->combine_width >= row_width) {
      എങ്കിൽ (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    നിർത്തുക;
  }

  അവസ്ഥ 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    എങ്കിൽ(val == 0)
      // DEC VT100 response
      vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    നിർത്തുക;

  അവസ്ഥ LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    നിർത്തുക;

  അവസ്ഥ 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    എങ്കിൽ(state->mode.origin)
      state->pos.row += state->scrollregion_മുകൾ;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x65: // VPR - ECMA-48 8.3.160
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.row += എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    എങ്കിൽ(state->mode.origin) {
      state->pos.row += state->scrollregion_മുകൾ;
      state->pos.col += SCROLLREGION_ഇടത്(state);
    }
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    തിരഞ്ഞെടുപ്പ്(val) {
    അവസ്ഥ 0:
      clear_col_tabstop(state, state->pos.col);
      നിർത്തുക;
    അവസ്ഥ 3:
    അവസ്ഥ 5:
      വേണ്ടി(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      നിർത്തുക;
    അവസ്ഥ 1:
    അവസ്ഥ 2:
    അവസ്ഥ 4:
      നിർത്തുക;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    സ്ഥിരസ്ഥിതി:
      തിരികെ 0;
    }
    നിർത്തുക;

  അവസ്ഥ 0x68: // SM - ECMA-48 8.3.125
    എങ്കിൽ(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    നിർത്തുക;

  അവസ്ഥ LEADER('?', 0x68): // DEC private mode set
    വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < argcount; i++) {
      എങ്കിൽ(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    നിർത്തുക;

  അവസ്ഥ 0x6a: // HPB - ECMA-48 8.3.58
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.col -= എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x6b: // VPB - ECMA-48 8.3.159
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);
    state->pos.row -= എണ്ണം;
    state->at_phantom = 0;
    നിർത്തുക;

  അവസ്ഥ 0x6c: // RM - ECMA-48 8.3.106
    എങ്കിൽ(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    നിർത്തുക;

  അവസ്ഥ LEADER('?', 0x6c): // DEC private mode reset
    വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < argcount; i++) {
      എങ്കിൽ(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    നിർത്തുക;

  അവസ്ഥ 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    നിർത്തുക;

  അവസ്ഥ LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    എങ്കിൽ (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_താക്കോലുകൾ ? 2 : 0);
      നിർത്തുക;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    വേണ്ടി(പൂർണ്ണസംഖ്യ argi = 0; argi < argcount; argi++) {
      long arg;
      തിരഞ്ഞെടുപ്പ്(arg = CSI_ARG(args[argi])) {
        അവസ്ഥ 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          നിർത്തുക;
        അവസ്ഥ 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          നിർത്തുക;
        അവസ്ഥ 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          നിർത്തുക;
      }
    }
    നിർത്തുക;

  അവസ്ഥ LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    എങ്കിൽ (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_താക്കോലുകൾ = CSI_ARG(args[1]) == 2;
    }
    നിർത്തുക;

  അവസ്ഥ LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    എങ്കിൽ (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_താക്കോലുകൾ = 0;

      state->mode.kitty_keyboard = 1;
    }
    നിർത്തുക;

  അവസ്ഥ LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    എങ്കിൽ (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    നിർത്തുക;

  അവസ്ഥ LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    എങ്കിൽ (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    നിർത്തുക;

  അവസ്ഥ 0x6e: // DSR - ECMA-48 8.3.35
  അവസ്ഥ LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      അക്ഷരം *qmark = (leader_byte == '?') ? "?" : "";

      തിരഞ്ഞെടുപ്പ്(val) {
      അവസ്ഥ 0: അവസ്ഥ 1: അവസ്ഥ 2: അവസ്ഥ 3: അവസ്ഥ 4:
        // ignore - these are replies
        നിർത്തുക;
      അവസ്ഥ 5:
        vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        നിർത്തുക;
      അവസ്ഥ 6: // CPR - cursor position report
        vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        നിർത്തുക;
      }
    }
    നിർത്തുക;


  അവസ്ഥ INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    നിർത്തുക;

  അവസ്ഥ LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    നിർത്തുക;

  അവസ്ഥ LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    നിർത്തുക;

  അവസ്ഥ INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    തിരഞ്ഞെടുപ്പ്(val) {
    അവസ്ഥ 0: അവസ്ഥ 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      നിർത്തുക;
    അവസ്ഥ 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      നിർത്തുക;
    അവസ്ഥ 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      നിർത്തുക;
    അവസ്ഥ 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      നിർത്തുക;
    അവസ്ഥ 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ഇടത്);
      നിർത്തുക;
    അവസ്ഥ 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ഇടത്);
      നിർത്തുക;
    }

    നിർത്തുക;

  അവസ്ഥ INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    തിരഞ്ഞെടുപ്പ്(val) {
    അവസ്ഥ 0: അവസ്ഥ 2:
      state->protected_cell = 0;
      നിർത്തുക;
    അവസ്ഥ 1:
      state->protected_cell = 1;
      നിർത്തുക;
    }

    നിർത്തുക;

  അവസ്ഥ 0x72: // DECSTBM - DEC custom
    state->scrollregion_മുകൾ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_മുകൾ, 0);
    UBOUND(state->scrollregion_മുകൾ, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    എങ്കിൽ(state->scrollregion_മുകൾ == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    അല്ലെങ്കിൽ
      UBOUND(state->scrollregion_bottom, state->rows);

    എങ്കിൽ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_മുകൾ) {
      // Invalid
      state->scrollregion_മുകൾ    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    എങ്കിൽ(state->mode.origin) {
      state->pos.row += state->scrollregion_മുകൾ;
      state->pos.col += SCROLLREGION_ഇടത്(state);
    }

    നിർത്തുക;

  അവസ്ഥ 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_ഇടത് = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_വലത് = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_ഇടത്, 0);
    UBOUND(state->scrollregion_ഇടത്, state->cols);
    LBOUND(state->scrollregion_വലത്, -1);
    എങ്കിൽ(state->scrollregion_ഇടത് == 0 && state->scrollregion_വലത് == state->cols)
      state->scrollregion_വലത് = -1;
    അല്ലെങ്കിൽ
      UBOUND(state->scrollregion_വലത്, state->cols);

    എങ്കിൽ(state->scrollregion_വലത് > -1 &&
       state->scrollregion_വലത് <= state->scrollregion_ഇടത്) {
      // Invalid
      state->scrollregion_ഇടത്  = 0;
      state->scrollregion_വലത് = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    എങ്കിൽ(state->mode.origin) {
      state->pos.row += state->scrollregion_മുകൾ;
      state->pos.col += SCROLLREGION_ഇടത്(state);
    }

    നിർത്തുക;

  അവസ്ഥ 0x74:
    തിരഞ്ഞെടുപ്പ്(CSI_ARG(args[0])) {
      അവസ്ഥ 8: // CSI 8 ; rows ; cols t  set size
        എങ്കിൽ (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
	handled = 0;
	നിർത്തുക;
    }
    നിർത്തുക;

  അവസ്ഥ INTERMED('\'', 0x7D): // DECIC
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    എങ്കിൽ(!is_cursor_in_scrollregion(state))
      നിർത്തുക;

    rect.start_row = state->scrollregion_മുകൾ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_വലത്(state);

    scroll(state, rect, 0, -എണ്ണം);

    നിർത്തുക;

  അവസ്ഥ INTERMED('\'', 0x7E): // DECDC
    എണ്ണം = CSI_ARG_എണ്ണം(args[0]);

    എങ്കിൽ(!is_cursor_in_scrollregion(state))
      നിർത്തുക;

    rect.start_row = state->scrollregion_മുകൾ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_വലത്(state);

    scroll(state, rect, 0, എണ്ണം);

    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    handled = 0;
    നിർത്തുക;
  }

  എങ്കിൽ (!handled) {
    എങ്കിൽ(state->fallbacks && state->fallbacks->csi)
      എങ്കിൽ((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        തിരികെ 1;

    തിരികെ 0;
  }

  എങ്കിൽ(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_മുകൾ);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_ഇടത്(state));
    UBOUND(state->pos.col, SCROLLREGION_വലത്(state)-1);
  }
  അല്ലെങ്കിൽ {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  എങ്കിൽ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  എങ്കിൽ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_മുകൾ) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_മുകൾ);
    abort();
  }

  എങ്കിൽ(SCROLLREGION_വലത്(state) <= SCROLLREGION_ഇടത്(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_വലത്(state), SCROLLREGION_ഇടത്(state));
    abort();
  }
#endif

  തിരികെ 1;
}

സ്ഥിതം അക്ഷരം base64_one(uint8_t b)
{
  എങ്കിൽ(b < 26)
    തിരികെ 'A' + b;
  അല്ലെങ്കിൽ എങ്കിൽ(b < 52)
    തിരികെ 'a' + b - 26;
  അല്ലെങ്കിൽ എങ്കിൽ(b < 62)
    തിരികെ '0' + b - 52;
  അല്ലെങ്കിൽ എങ്കിൽ(b == 62)
    തിരികെ '+';
  അല്ലെങ്കിൽ എങ്കിൽ(b == 63)
    തിരികെ '/';
  തിരികെ 0;
}

സ്ഥിതം uint8_t unbase64one(അക്ഷരം c)
{
  എങ്കിൽ(c >= 'A' && c <= 'Z')
    തിരികെ c - 'A';
  അല്ലെങ്കിൽ എങ്കിൽ(c >= 'a' && c <= 'z')
    തിരികെ c - 'a' + 26;
  അല്ലെങ്കിൽ എങ്കിൽ(c >= '0' && c <= '9')
    തിരികെ c - '0' + 52;
  അല്ലെങ്കിൽ എങ്കിൽ(c == '+')
    തിരികെ 62;
  അല്ലെങ്കിൽ എങ്കിൽ(c == '/')
    തിരികെ 63;

  തിരികെ 0xFF;
}

സ്ഥിതം ശൂന്യം osc_selection(VTermState *state, VTermStringFragment frag)
{
  എങ്കിൽ(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  ആകുമ്പോൾ(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    തിരഞ്ഞെടുപ്പ്(frag.str[0]) {
      അവസ്ഥ 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        നിർത്തുക;
      അവസ്ഥ 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        നിർത്തുക;
      അവസ്ഥ 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        നിർത്തുക;
      അവസ്ഥ 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        നിർത്തുക;
      അവസ്ഥ '0':
      അവസ്ഥ '1':
      അവസ്ഥ '2':
      അവസ്ഥ '3':
      അവസ്ഥ '4':
      അവസ്ഥ '5':
      അവസ്ഥ '6':
      അവസ്ഥ '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        നിർത്തുക;

      അവസ്ഥ ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        എങ്കിൽ(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        നിർത്തുക;
    }

    frag.str++;
    frag.len--;
  }

  എങ്കിൽ(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    എങ്കിൽ(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = ശൂന്യ,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    തിരികെ;
  }

  എങ്കിൽ(state->tmp.selection.state == SELECTION_SELECTED) {
    എങ്കിൽ(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    അല്ലെങ്കിൽ {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  എങ്കിൽ(state->tmp.selection.state == SELECTION_QUERY) {
    എങ്കിൽ(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    തിരികെ;
  }

  എങ്കിൽ(state->tmp.selection.state == SELECTION_INVALID)
    തിരികെ;

  എങ്കിൽ(state->selection.callbacks->set) {
    size_t bufcur = 0;
    അക്ഷരം *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    പൂർണ്ണസംഖ്യ n = 0;      /* Number of sextets consumed */

    എങ്കിൽ(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    ആകുമ്പോൾ((state->selection.buflen - bufcur) >= 3 && frag.len) {
      എങ്കിൽ(frag.str[0] == '=') {
        എങ്കിൽ(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        എങ്കിൽ(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        ആകുമ്പോൾ(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      അല്ലെങ്കിൽ {
        uint8_t b = unbase64one(frag.str[0]);
        എങ്കിൽ(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          എങ്കിൽ(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = ശൂന്യ,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          നിർത്തുക;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        എങ്കിൽ(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      എങ്കിൽ(!frag.len || (state->selection.buflen - bufcur) < 3) {
        എങ്കിൽ(bufcur) {
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

    എങ്കിൽ(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_osc(പൂർണ്ണസംഖ്യ command, VTermStringFragment frag, ശൂന്യം *user)
{
  VTermState *state = user;

  തിരഞ്ഞെടുപ്പ്(command) {
    അവസ്ഥ 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      തിരികെ 1;

    അവസ്ഥ 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      തിരികെ 1;

    അവസ്ഥ 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      തിരികെ 1;

    അവസ്ഥ 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        പൂർണ്ണസംഖ്യ red = state->default_fg.red;
        പൂർണ്ണസംഖ്യ blue = state->default_fg.blue;
        പൂർണ്ണസംഖ്യ green = state->default_fg.green;
        vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        തിരികെ 1;
      }

    അവസ്ഥ 11:
      {
	// request background color: <Esc>]11;?<0x07>
	പൂർണ്ണസംഖ്യ red = state->default_bg.red;
	പൂർണ്ണസംഖ്യ blue = state->default_bg.blue;
	പൂർണ്ണസംഖ്യ green = state->default_bg.green;
	vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	തിരികെ 1;
      }
    അവസ്ഥ 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      തിരികെ 1;

    അവസ്ഥ 52:
      എങ്കിൽ(state->selection.callbacks)
        osc_selection(state, frag);

      തിരികെ 1;

    സ്ഥിരസ്ഥിതി:
      എങ്കിൽ(state->fallbacks && state->fallbacks->osc)
        എങ്കിൽ((*state->fallbacks->osc)(command, frag, state->fbdata))
          തിരികെ 1;
  }

  തിരികെ 0;
}

സ്ഥിതം ശൂന്യം request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  അക്ഷരം *tmp = state->tmp.decrqss;

  എങ്കിൽ(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  ആകുമ്പോൾ(i < വലുപ്പം(state->tmp.decrqss)-1 && tmp[i])
    i++;
  ആകുമ്പോൾ(i < വലുപ്പം(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  എങ്കിൽ(!frag.final)
    തിരികെ;

  തിരഞ്ഞെടുപ്പ്(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    അവസ്ഥ 'm': {
      // Query SGR
      long args[20];
      പൂർണ്ണസംഖ്യ argc = vterm_state_getpen(state, args, വലുപ്പം(args)/വലുപ്പം(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      എങ്കിൽ(cur >= vt->tmpbuffer_len)
        തിരികെ;

      വേണ്ടി(പൂർണ്ണസംഖ്യ argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        എങ്കിൽ(cur >= vt->tmpbuffer_len)
          തിരികെ;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      എങ്കിൽ(cur >= vt->tmpbuffer_len)
        തിരികെ;

      vterm_തള്ളുക_output_bytes(vt, vt->tmpbuffer, cur);
      തിരികെ;
    }

    അവസ്ഥ 'r':
      // Query DECSTBM
      vterm_തള്ളുക_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_മുകൾ+1, SCROLLREGION_BOTTOM(state));
      തിരികെ;

    അവസ്ഥ 's':
      // Query DECSLRM
      vterm_തള്ളുക_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_ഇടത്(state)+1, SCROLLREGION_വലത്(state));
      തിരികെ;

    അവസ്ഥ ' '|('q'<<8): {
      // Query DECSCUSR
      പൂർണ്ണസംഖ്യ reply;
      തിരഞ്ഞെടുപ്പ്(state->mode.cursor_shape) {
        അവസ്ഥ VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; നിർത്തുക;
        അവസ്ഥ VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; നിർത്തുക;
	സ്ഥിരസ്ഥിതി: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; നിർത്തുക;
      }
      എങ്കിൽ(state->mode.cursor_blink)
        reply--;
      vterm_തള്ളുക_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      തിരികെ;
    }

    അവസ്ഥ '\"'|('q'<<8):
      // Query DECSCA
      vterm_തള്ളുക_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      തിരികെ;
  }

  vterm_തള്ളുക_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_dcs(സ്ഥിരം അക്ഷരം *command, size_t commandlen, VTermStringFragment frag, ശൂന്യം *user)
{
  VTermState *state = user;

  എങ്കിൽ(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    തിരികെ 1;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(state->fallbacks && state->fallbacks->dcs)
    എങ്കിൽ((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      തിരികെ 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (പൂർണ്ണസംഖ്യ)commandlen, command);
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_apc(VTermStringFragment frag, ശൂന്യം *user)
{
  VTermState *state = user;

  എങ്കിൽ(state->fallbacks && state->fallbacks->apc)
    എങ്കിൽ((*state->fallbacks->apc)(frag, state->fbdata))
      തിരികെ 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_pm(VTermStringFragment frag, ശൂന്യം *user)
{
  VTermState *state = user;

  എങ്കിൽ(state->fallbacks && state->fallbacks->pm)
    എങ്കിൽ((*state->fallbacks->pm)(frag, state->fbdata))
      തിരികെ 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_sos(VTermStringFragment frag, ശൂന്യം *user)
{
  VTermState *state = user;

  എങ്കിൽ(state->fallbacks && state->fallbacks->sos)
    എങ്കിൽ((*state->fallbacks->sos)(frag, state->fbdata))
      തിരികെ 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ on_resize(പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols, ശൂന്യം *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  എങ്കിൽ (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  എങ്കിൽ (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  എങ്കിൽ(cols != state->cols) {
    unsigned അക്ഷരം *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    എങ്കിൽ (newtabstops == ശൂന്യ)
      തിരികെ 0;

    /* TODO: This can all be done much more efficiently bytewise */
    പൂർണ്ണസംഖ്യ col;
    വേണ്ടി(col = 0; col < state->cols && col < cols; col++) {
      unsigned അക്ഷരം mask = 1 << (col & 7);
      എങ്കിൽ(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      അല്ലെങ്കിൽ
        newtabstops[col >> 3] &= ~mask;
      }

    വേണ്ടി( ; col < cols; col++) {
      unsigned അക്ഷരം mask = 1 << (col & 7);
      എങ്കിൽ(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      അല്ലെങ്കിൽ
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  എങ്കിൽ(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  എങ്കിൽ(state->scrollregion_വലത് > -1)
    UBOUND(state->scrollregion_വലത്, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_മുകൾ, state->rows);
  UBOUND(state->scrollregion_ഇടത്, state->cols);
  എങ്കിൽ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_മുകൾ) {
    state->scrollregion_മുകൾ = 0;
    state->scrollregion_bottom = -1;
  }
  എങ്കിൽ(SCROLLREGION_വലത്(state) <= state->scrollregion_ഇടത്) {
    state->scrollregion_ഇടത് = 0;
    state->scrollregion_വലത് = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  എങ്കിൽ(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  അല്ലെങ്കിൽ {
    എങ്കിൽ(rows != state->rows) {
      വേണ്ടി(പൂർണ്ണസംഖ്യ bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_വരി_Info *oldlineinfo = state->lineinfos[bufidx];
        എങ്കിൽ(!oldlineinfo)
          തുടരുക;

        VTerm_വരി_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * വലുപ്പം(VTerm_വരി_Info));

        പൂർണ്ണസംഖ്യ row;
        വേണ്ടി(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        വേണ്ടി( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_വരി_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  എങ്കിൽ(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  എങ്കിൽ(state->pos.row < 0)
    state->pos.row = 0;
  എങ്കിൽ(state->pos.row >= rows)
    state->pos.row = rows - 1;
  എങ്കിൽ(state->pos.col < 0)
    state->pos.col = 0;
  എങ്കിൽ(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം VTermParserCallbacks parser_callbacks = {
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
  എങ്കിൽ(vt->state)
    തിരികെ vt->state;

  VTermState *state = vterm_state_new(vt);
  എങ്കിൽ (state == ശൂന്യ)
    തിരികെ ശൂന്യ;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  തിരികെ state;
}

ശൂന്യം vterm_state_reset(VTermState *state, പൂർണ്ണസംഖ്യ hard)
{
  state->scrollregion_മുകൾ = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_ഇടത് = 0;
  state->scrollregion_വലത് = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.തിരുകുക          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  വേണ്ടി(പൂർണ്ണസംഖ്യ col = 0; col < state->cols; col++)
    എങ്കിൽ(col % 8 == 0)
      set_col_tabstop(state, col);
    അല്ലെങ്കിൽ
      clear_col_tabstop(state, col);

  വേണ്ടി(പൂർണ്ണസംഖ്യ row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  എങ്കിൽ(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  വേണ്ടി(പൂർണ്ണസംഖ്യ i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    എങ്കിൽ(default_enc->init)
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

  എങ്കിൽ(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

ശൂന്യം vterm_state_get_cursorpos(സ്ഥിരം VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

ശൂന്യം vterm_state_get_mousestate(സ്ഥിരം VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

ശൂന്യം vterm_state_set_callbacks(VTermState *state, സ്ഥിരം VTermStateCallbacks *callbacks, ശൂന്യം *user)
{
  എങ്കിൽ(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    എങ്കിൽ(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  അല്ലെങ്കിൽ {
    state->callbacks = ശൂന്യ;
    state->cbdata = ശൂന്യ;
  }
}

ശൂന്യം vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

ശൂന്യം *vterm_state_get_cbdata(VTermState *state)
{
  തിരികെ state->cbdata;
}

ശൂന്യം vterm_state_set_unrecognised_fallbacks(VTermState *state, സ്ഥിരം VTermStateFallbacks *fallbacks, ശൂന്യം *user)
{
  എങ്കിൽ(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  അല്ലെങ്കിൽ {
    state->fallbacks = ശൂന്യ;
    state->fbdata = ശൂന്യ;
  }
}

ശൂന്യം *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  തിരികെ state->fbdata;
}

പൂർണ്ണസംഖ്യ vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_മൂല്യം *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  എങ്കിൽ(state->callbacks && state->callbacks->settermprop)
    എങ്കിൽ(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      തിരികെ 0;

  തിരഞ്ഞെടുപ്പ്(prop) {
  അവസ്ഥ VTERM_PROP_TITLE:
  അവസ്ഥ VTERM_PROP_ICONNAME:
  അവസ്ഥ VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    തിരികെ 1;
  അവസ്ഥ VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    തിരികെ 1;
  അവസ്ഥ VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    തിരികെ 1;
  അവസ്ഥ VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->സംഖ്യ;
    തിരികെ 1;
  അവസ്ഥ VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    തിരികെ 1;
  അവസ്ഥ VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    എങ്കിൽ(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    തിരികെ 1;
  അവസ്ഥ VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    എങ്കിൽ(val->സംഖ്യ)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    എങ്കിൽ(val->സംഖ്യ == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    എങ്കിൽ(val->സംഖ്യ == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    തിരികെ 1;
  അവസ്ഥ VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    തിരികെ 1;

  അവസ്ഥ VTERM_N_PROPS:
    തിരികെ 0;
  }

  തിരികെ 0;
}

ശൂന്യം vterm_state_focus_in(VTermState *state)
{
  എങ്കിൽ(state->mode.report_focus)
    vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

ശൂന്യം vterm_state_focus_out(VTermState *state)
{
  എങ്കിൽ(state->mode.report_focus)
    vterm_തള്ളുക_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

സ്ഥിരം VTerm_വരി_Info *vterm_state_get_lineinfo(സ്ഥിരം VTermState *state, പൂർണ്ണസംഖ്യ row)
{
  തിരികെ state->lineinfo + row;
}

ശൂന്യം vterm_state_set_selection_callbacks(VTermState *state, സ്ഥിരം VTermSelectionCallbacks *callbacks, ശൂന്യം *user,
    അക്ഷരം *buffer, size_t buflen)
{
  എങ്കിൽ(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

ശൂന്യം vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  എങ്കിൽ(frag.initial) {
    /* TODO: support sending more than one mask bit */
    സ്ഥിതം അക്ഷരം selection_chars[] = "cpqs";
    പൂർണ്ണസംഖ്യ idx;
    വേണ്ടി(idx = 0; idx < 4; idx++)
      എങ്കിൽ(mask & (1 << idx))
        നിർത്തുക;

    vterm_തള്ളുക_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  എങ്കിൽ(frag.len) {
    size_t bufcur = 0;
    അക്ഷരം *buffer = state->selection.buffer;

    uint32_t x = 0;
    പൂർണ്ണസംഖ്യ n = 0;

    എങ്കിൽ(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    ആകുമ്പോൾ((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      എങ്കിൽ(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      എങ്കിൽ(!frag.len || (state->selection.buflen - bufcur) < 4) {
        എങ്കിൽ(bufcur)
          vterm_തള്ളുക_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    എങ്കിൽ(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  എങ്കിൽ(frag.final) {
    എങ്കിൽ(state->tmp.selection.sendpartial) {
      പൂർണ്ണസംഖ്യ n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      അക്ഷരം *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_തള്ളുക_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    അല്ലെങ്കിൽ
      vterm_തള്ളുക_output_sprintf_str(vt, 0, TRUE, "");
  }
}
