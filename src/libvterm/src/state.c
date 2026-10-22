#અન્તર્ભાવ "vterm_internal.h"

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#જો defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

સ્થિર પૂર્ણાંક on_resize(પૂર્ણાંક rows, પૂર્ણાંક cols, શૂન્ય *user);

/* Some convenient wrappers to make callback functions easier */

સ્થિર શૂન્ય putglyph(VTermState *state, અચલ uint32_t chars[], પૂર્ણાંક width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  જો(state->callbacks && state->callbacks->putglyph)
    જો((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      પાછા;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

સ્થિર શૂન્ય updatecursor(VTermState *state, VTermPos *oldpos, પૂર્ણાંક cancel_phantom)
{
  જો(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    પાછા;

  જો(cancel_phantom)
    state->at_phantom = 0;

  જો(state->callbacks && state->callbacks->movecursor)
    જો((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      પાછા;
}

સ્થિર શૂન્ય erase(VTermState *state, VTermRect rect, પૂર્ણાંક selective)
{
  જો(rect.end_col == state->cols) {
    પૂર્ણાંક row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    માટે(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  જો(state->callbacks && state->callbacks->erase)
    જો((*state->callbacks->erase)(rect, selective, state->cbdata))
      પાછા;
}

સ્થિર VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, કદ(VTermState));

  જો (state == ખાલી)
    પાછા ખાલી;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = ખાલી;
  state->cbdata    = ખાલી;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = ખાલી;
  state->selection.user      = ખાલી;
  state->selection.buffer    = ખાલી;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * કદ(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * કદ(VTerm_લીટી_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * કદ(VTerm_લીટી_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  જો(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  પાછા state;
}

INTERNAL શૂન્ય vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  જો(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

સ્થિર શૂન્ય scroll(VTermState *state, VTermRect rect, પૂર્ણાંક downward, પૂર્ણાંક rightward)
{
  પૂર્ણાંક rows;
  પૂર્ણાંક cols;
  જો(!downward && !rightward)
    પાછા;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  જો(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    પાછા;

  rows = rect.end_row - rect.start_row;
  જો(downward > rows)
    downward = rows;
  નહીંતર જો(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  જો(rightward > cols)
    rightward = cols;
  નહીંતર જો(rightward < -cols)
    rightward = -cols;

  જો(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    જો(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    નહીંતર જો(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    જો(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    નહીંતર જો(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  જો(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    પૂર્ણાંક height = rect.end_row - rect.start_row - abs(downward);
    પૂર્ણાંક row;
    VTerm_લીટી_Info zero_લીટી_Info = {0x0};

    જો(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * કદ(state->lineinfo[0]));
      માટે(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_લીટી_Info;
    }
    નહીંતર {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * કદ(state->lineinfo[0]));
      માટે(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_લીટી_Info;
    }
  }

  જો(state->callbacks && state->callbacks->scrollrect)
    જો((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      પાછા;

  જો(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

સ્થિર શૂન્ય linefeed(VTermState *state)
{
  જો(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_ટોચ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ડાબું(state);
    rect.end_col   = SCROLLREGION_જમણું(state);

    scroll(state, rect, 1, 0);
  }
  નહીંતર જો(state->pos.row < state->rows-1)
    state->pos.row++;
}

સ્થિર શૂન્ય grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * કદ(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * કદ(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

સ્થિર શૂન્ય set_col_tabstop(VTermState *state, પૂર્ણાંક col)
{
  unsigned અક્ષર mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

સ્થિર શૂન્ય clear_col_tabstop(VTermState *state, પૂર્ણાંક col)
{
  unsigned અક્ષર mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

સ્થિર પૂર્ણાંક is_col_tabstop(VTermState *state, પૂર્ણાંક col)
{
  unsigned અક્ષર mask = 1 << (col & 7);
  પાછા state->tabstops[col >> 3] & mask;
}

સ્થિર પૂર્ણાંક is_cursor_in_scrollregion(અચલ VTermState *state)
{
  જો(state->pos.row < state->scrollregion_ટોચ ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    પાછા 0;
  જો(state->pos.col < SCROLLREGION_ડાબું(state) ||
     state->pos.col >= SCROLLREGION_જમણું(state))
    પાછા 0;

  પાછા 1;
}

સ્થિર શૂન્ય tab(VTermState *state, પૂર્ણાંક ગણતરી, પૂર્ણાંક direction)
{
  જ્યારે(ગણતરી > 0) {
    જો(direction > 0) {
      જો(state->pos.col >= THISROWWIDTH(state)-1)
        પાછા;

      state->pos.col++;
    }
    નહીંતર જો(direction < 0) {
      જો(state->pos.col < 1)
        પાછા;

      state->pos.col--;
    }

    જો(is_col_tabstop(state, state->pos.col))
      ગણતરી--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_ટોચ    1
#define DHL_BOTTOM 2

સ્થિર શૂન્ય set_lineinfo(VTermState *state, પૂર્ણાંક row, પૂર્ણાંક force, પૂર્ણાંક dwl, પૂર્ણાંક dhl)
{
  VTerm_લીટી_Info info = state->lineinfo[row];

  જો(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  નહીંતર જો(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  જો(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  નહીંતર જો(dhl == DHL_ટોચ)
    info.doubleheight = DHL_ટોચ;
  નહીંતર જો(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  જો((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

સ્થિર પૂર્ણાંક on_text(અચલ અક્ષર bytes[], size_t len, શૂન્ય *user)
{
  VTermState *state = user;
  પૂર્ણાંક npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  પૂર્ણાંક i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / કદ(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  જો (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (પૂર્ણાંક)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  જો(!npoints)
  {
    પાછા (પૂર્ણાંક)eaten;
  }

  જો(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  જો(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    જો(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      પૂર્ણાંક printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      માટે(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      પૂર્ણાંક saved_i = 0;
      જ્યારે(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      જ્યારે(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        જો(saved_i >= (પૂર્ણાંક)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      જો(saved_i >= (પૂર્ણાંક)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      માટે(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    નહીંતર {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  માટે(; i < npoints; i++) {
    // Try to find combining characters following this
    પૂર્ણાંક glyph_starts = i;
    પૂર્ણાંક glyph_ends;
    પૂર્ણાંક width = 0;

    માટે(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      જો(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        તોડો;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * કદ(uint32_t));
    જો (chars == ખાલી)
      તોડો;

    માટે( ; i < glyph_ends; i++) {
      પૂર્ણાંક this_width;
      જો(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        જો(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      જો(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      જો (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    જો (width < 0)
      width = 0;

    જ્યારે(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    પૂર્ણાંક printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    માટે(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    જો(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    જો(state->mode.ઉમેરો) {
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

    જો(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      પૂર્ણાંક save_i;
      માટે(save_i = 0; chars[save_i]; save_i++) {
        જો(save_i >= (પૂર્ણાંક)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      જો(save_i >= (પૂર્ણાંક)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    જો(state->pos.col + width >= THISROWWIDTH(state)) {
      જો(state->mode.autowrap)
        state->at_phantom = 1;
    }
    નહીંતર {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  જો(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  પાછા (પૂર્ણાંક)eaten;
}

સ્થિર પૂર્ણાંક on_control(unsigned અક્ષર control, શૂન્ય *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  પસંદગી(control) {
  સ્થિતિ 0x07: // BEL - ECMA-48 8.3.3
    જો(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    તોડો;

  સ્થિતિ 0x08: // BS - ECMA-48 8.3.5
    જો(state->pos.col > 0)
      state->pos.col--;
    જો(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      જો(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    તોડો;

  સ્થિતિ 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    તોડો;

  સ્થિતિ 0x0a: // LF - ECMA-48 8.3.74
  સ્થિતિ 0x0b: // VT
  સ્થિતિ 0x0c: // FF
    linefeed(state);
    જો(state->mode.newline)
      state->pos.col = 0;
    તોડો;

  સ્થિતિ 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    તોડો;

  સ્થિતિ 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    તોડો;

  સ્થિતિ 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    તોડો;

  સ્થિતિ 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    તોડો;

  સ્થિતિ 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    તોડો;

  સ્થિતિ 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    તોડો;

  સ્થિતિ 0x8d: // RI - ECMA-48 8.3.104
    જો(state->pos.row == state->scrollregion_ટોચ) {
      VTermRect rect;
      rect.start_row = state->scrollregion_ટોચ;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_ડાબું(state);
      rect.end_col   = SCROLLREGION_જમણું(state);

      scroll(state, rect, -1, 0);
    }
    નહીંતર જો(state->pos.row > 0)
        state->pos.row--;
    તોડો;

  સ્થિતિ 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    તોડો;

  સ્થિતિ 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    તોડો;

  મૂળભૂત:
    જો(state->fallbacks && state->fallbacks->control)
      જો((*state->fallbacks->control)(control, state->fbdata))
        પાછા 1;

    પાછા 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  જો(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  પાછા 1;
}

સ્થિર પૂર્ણાંક settermprop_bool(VTermState *state, VTermProp prop, પૂર્ણાંક v)
{
  VTerm_કિંમત val;
  val.boolean = v;
  પાછા vterm_state_set_termprop(state, prop, &val);
}

સ્થિર પૂર્ણાંક settermprop_int(VTermState *state, VTermProp prop, પૂર્ણાંક v)
{
  VTerm_કિંમત val;
  val.સંખ્યા = v;
  પાછા vterm_state_set_termprop(state, prop, &val);
}

સ્થિર પૂર્ણાંક settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_કિંમત val;

  val.string = frag;
  પાછા vterm_state_set_termprop(state, prop, &val);
}

સ્થિર શૂન્ય savecursor(VTermState *state, પૂર્ણાંક save)
{
  જો(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  નહીંતર {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

સ્થિર પૂર્ણાંક on_escape(અચલ અક્ષર *bytes, size_t len, શૂન્ય *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  પસંદગી(bytes[0]) {
  સ્થિતિ ' ':
    જો(len != 2)
      પાછા 0;

    પસંદગી(bytes[1]) {
      સ્થિતિ 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        તોડો;

      સ્થિતિ 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        તોડો;

      મૂળભૂત:
        પાછા 0;
    }
    પાછા 2;

  સ્થિતિ '#':
    જો(len != 2)
      પાછા 0;

    પસંદગી(bytes[1]) {
      સ્થિતિ '3': // DECDHL top
        જો(state->mode.leftrightmargin)
          તોડો;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_ટોચ);
        તોડો;

      સ્થિતિ '4': // DECDHL bottom
        જો(state->mode.leftrightmargin)
          તોડો;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        તોડો;

      સ્થિતિ '5': // DECSWL
        જો(state->mode.leftrightmargin)
          તોડો;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        તોડો;

      સ્થિતિ '6': // DECDWL
        જો(state->mode.leftrightmargin)
          તોડો;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        તોડો;

      સ્થિતિ '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        માટે(pos.row = 0; pos.row < state->rows; pos.row++)
          માટે(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        તોડો;
      }

      મૂળભૂત:
        પાછા 0;
    }
    પાછા 2;

  સ્થિતિ '(': સ્થિતિ ')': સ્થિતિ '*': સ્થિતિ '+': // SCS
    જો(len != 2)
      પાછા 0;

    {
      પૂર્ણાંક setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      જો(newenc) {
        state->encoding[setnum].enc = newenc;

        જો(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    પાછા 2;

  સ્થિતિ '7': // DECSC
    savecursor(state, 1);
    પાછા 1;

  સ્થિતિ '8': // DECRC
    savecursor(state, 0);
    પાછા 1;

  સ્થિતિ '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    પાછા 1;

  સ્થિતિ '=': // DECKPAM
    state->mode.keypad = 1;
    પાછા 1;

  સ્થિતિ '>': // DECKPNM
    state->mode.keypad = 0;
    પાછા 1;

  સ્થિતિ 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    જો(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    પાછા 1;
  }

  સ્થિતિ 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    પાછા 1;

  સ્થિતિ 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    પાછા 1;

  સ્થિતિ '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    પાછા 1;

  સ્થિતિ '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    પાછા 1;

  સ્થિતિ '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    પાછા 1;

  મૂળભૂત:
    પાછા 0;
  }
}

સ્થિર શૂન્ય set_mode(VTermState *state, પૂર્ણાંક અંક, પૂર્ણાંક val)
{
  પસંદગી(અંક) {
  સ્થિતિ 4: // IRM - ECMA-48 7.2.10
    state->mode.ઉમેરો = val;
    તોડો;

  સ્થિતિ 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    તોડો;

  મૂળભૂત:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", અંક);
    પાછા;
  }
}

સ્થિર શૂન્ય set_dec_mode(VTermState *state, પૂર્ણાંક અંક, પૂર્ણાંક val)
{
  પસંદગી(અંક) {
  સ્થિતિ 1:
    state->mode.cursor = val;
    તોડો;

  સ્થિતિ 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    તોડો;

  સ્થિતિ 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_ટોચ : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_ડાબું(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    તોડો;

  સ્થિતિ 7:
    state->mode.autowrap = val;
    તોડો;

  સ્થિતિ 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    તોડો;

  સ્થિતિ 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    તોડો;

  સ્થિતિ 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    જો(val) {
      પૂર્ણાંક row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      માટે(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    તોડો;

  સ્થિતિ 1000:
  સ્થિતિ 1002:
  સ્થિતિ 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (અંક == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (અંક == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    તોડો;

  સ્થિતિ 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    તોડો;

  સ્થિતિ 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    તોડો;

  સ્થિતિ 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    તોડો;

  સ્થિતિ 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    તોડો;

  સ્થિતિ 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    તોડો;

  સ્થિતિ 1048:
    savecursor(state, val);
    તોડો;

  સ્થિતિ 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    તોડો;

  સ્થિતિ 2004:
    state->mode.bracketpaste = val;
    તોડો;

  મૂળભૂત:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", અંક);
    પાછા;
  }
}

સ્થિર શૂન્ય request_dec_mode(VTermState *state, પૂર્ણાંક અંક)
{
  પૂર્ણાંક reply;

  પસંદગી(અંક) {
    સ્થિતિ 1:
      reply = state->mode.cursor;
      તોડો;

    સ્થિતિ 5:
      reply = state->mode.screen;
      તોડો;

    સ્થિતિ 6:
      reply = state->mode.origin;
      તોડો;

    સ્થિતિ 7:
      reply = state->mode.autowrap;
      તોડો;

    સ્થિતિ 12:
      reply = state->mode.cursor_blink;
      તોડો;

    સ્થિતિ 25:
      reply = state->mode.cursor_visible;
      તોડો;

    સ્થિતિ 69:
      reply = state->mode.leftrightmargin;
      તોડો;

    સ્થિતિ 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      તોડો;

    સ્થિતિ 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      તોડો;

    સ્થિતિ 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      તોડો;

    સ્થિતિ 1004:
      reply = state->mode.report_focus;
      તોડો;

    સ્થિતિ 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      તોડો;

    સ્થિતિ 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      તોડો;

    સ્થિતિ 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      તોડો;

    સ્થિતિ 1047:
      reply = state->mode.alt_screen;
      તોડો;

    સ્થિતિ 2004:
      reply = state->mode.bracketpaste;
      તોડો;

    મૂળભૂત:
      vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", અંક, 0);
      પાછા;
  }

  vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", અંક, reply ? 1 : 2);
}

સ્થિર શૂન્ય request_version_string(VTermState *state)
{
  vterm_ધકેલો_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

સ્થિર પૂર્ણાંક on_csi(અચલ અક્ષર *leader, અચલ long args[], પૂર્ણાંક argcount, અચલ અક્ષર *intermed, અક્ષર command, શૂન્ય *user)
{
  VTermState *state = user;
  પૂર્ણાંક leader_byte = 0;
  પૂર્ણાંક intermed_byte = 0;
  પૂર્ણાંક cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  પૂર્ણાંક handled = 1;

  // Some temporaries for later code
  પૂર્ણાંક ગણતરી, val;
  પૂર્ણાંક row, col;
  VTermRect rect;
  પૂર્ણાંક selective;

  જો(leader && leader[0]) {
    જો(leader[1]) // longer than 1 char
      પાછા 0;

    પસંદગી(leader[0]) {
    સ્થિતિ '?':
    સ્થિતિ '>':
    સ્થિતિ '<':
      leader_byte = leader[0];
      તોડો;
    મૂળભૂત:
      પાછા 0;
    }
  }

  જો(intermed && intermed[0]) {
    જો(intermed[1]) // longer than 1 char
      પાછા 0;

    પસંદગી(intermed[0]) {
    સ્થિતિ ' ':
    સ્થિતિ '!':
    સ્થિતિ '"':
    સ્થિતિ '$':
    સ્થિતિ '\'':
      intermed_byte = intermed[0];
      તોડો;
    મૂળભૂત:
      પાછા 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) જો((v) < (min)) (v) = (min)
#define UBOUND(v,max) જો((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  પસંદગી(intermed_byte << 16 | leader_byte << 8 | command) {
  સ્થિતિ 0x40: // ICH - ECMA-48 8.3.64
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    જો(!is_cursor_in_scrollregion(state))
      તોડો;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    જો(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_જમણું(state);
    નહીંતર
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -ગણતરી);

    તોડો;

  સ્થિતિ 0x41: // CUU - ECMA-48 8.3.22
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.row -= ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x42: // CUD - ECMA-48 8.3.19
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.row += ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x43: // CUF - ECMA-48 8.3.20
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.col += ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x44: // CUB - ECMA-48 8.3.18
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.col -= ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x45: // CNL - ECMA-48 8.3.12
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.col = 0;
    state->pos.row += ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x46: // CPL - ECMA-48 8.3.13
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.col = 0;
    state->pos.row -= ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    જો(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      જો(state->pos.row == row - 1) {
        પૂર્ણાંક cnt, ptr = 0;
        માટે(cnt = 0; cnt < col - 1; ++cnt) {
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
    જો(state->mode.origin) {
      state->pos.row += state->scrollregion_ટોચ;
      state->pos.col += SCROLLREGION_ડાબું(state);
    }
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x49: // CHT - ECMA-48 8.3.10
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    tab(state, ગણતરી, +1);
    તોડો;

  સ્થિતિ 0x4a: // ED - ECMA-48 8.3.39
  સ્થિતિ LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    પસંદગી(CSI_ARG(args[0])) {
    સ્થિતિ CSI_ARG_MISSING:
    સ્થિતિ 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      જો(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      માટે(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      જો(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      તોડો;

    સ્થિતિ 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      માટે(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      જો(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      જો(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      તોડો;

    સ્થિતિ 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      માટે(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      તોડો;

    સ્થિતિ 3:
      જો(state->callbacks && state->callbacks->sb_clear)
        જો((*state->callbacks->sb_clear)(state->cbdata))
          પાછા 1;
      તોડો;
    }
    તોડો;

  સ્થિતિ 0x4b: // EL - ECMA-48 8.3.41
  સ્થિતિ LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    પસંદગી(CSI_ARG(args[0])) {
    સ્થિતિ CSI_ARG_MISSING:
    સ્થિતિ 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); તોડો;
    સ્થિતિ 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; તોડો;
    સ્થિતિ 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); તોડો;
    મૂળભૂત:
      પાછા 0;
    }

    જો(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    તોડો;

  સ્થિતિ 0x4c: // IL - ECMA-48 8.3.67
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    જો(!is_cursor_in_scrollregion(state))
      તોડો;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ડાબું(state);
    rect.end_col   = SCROLLREGION_જમણું(state);

    scroll(state, rect, -ગણતરી, 0);

    તોડો;

  સ્થિતિ 0x4d: // DL - ECMA-48 8.3.32
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    જો(!is_cursor_in_scrollregion(state))
      તોડો;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ડાબું(state);
    rect.end_col   = SCROLLREGION_જમણું(state);

    scroll(state, rect, ગણતરી, 0);

    તોડો;

  સ્થિતિ 0x50: // DCH - ECMA-48 8.3.26
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    જો(!is_cursor_in_scrollregion(state))
      તોડો;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    જો(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_જમણું(state);
    નહીંતર
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, ગણતરી);

    તોડો;

  સ્થિતિ 0x53: // SU - ECMA-48 8.3.147
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    rect.start_row = state->scrollregion_ટોચ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ડાબું(state);
    rect.end_col   = SCROLLREGION_જમણું(state);

    scroll(state, rect, ગણતરી, 0);

    તોડો;

  સ્થિતિ 0x54: // SD - ECMA-48 8.3.113
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    rect.start_row = state->scrollregion_ટોચ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_ડાબું(state);
    rect.end_col   = SCROLLREGION_જમણું(state);

    scroll(state, rect, -ગણતરી, 0);

    તોડો;

  સ્થિતિ 0x58: // ECH - ECMA-48 8.3.38
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + ગણતરી;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    તોડો;

  સ્થિતિ 0x5a: // CBT - ECMA-48 8.3.7
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    tab(state, ગણતરી, -1);
    તોડો;

  સ્થિતિ 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x61: // HPR - ECMA-48 8.3.59
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.col += ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x62: { // REP - ECMA-48 8.3.103
    અચલ પૂર્ણાંક row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    જો(state->combine_width < 1)
      તોડો;

    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    col = state->pos.col + ગણતરી;
    UBOUND(col, row_width);
    જ્યારે (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    જો (state->pos.col + state->combine_width >= row_width) {
      જો (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    તોડો;
  }

  સ્થિતિ 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    જો(val == 0)
      // DEC VT100 response
      vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    તોડો;

  સ્થિતિ LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    તોડો;

  સ્થિતિ 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    જો(state->mode.origin)
      state->pos.row += state->scrollregion_ટોચ;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x65: // VPR - ECMA-48 8.3.160
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.row += ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    જો(state->mode.origin) {
      state->pos.row += state->scrollregion_ટોચ;
      state->pos.col += SCROLLREGION_ડાબું(state);
    }
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    પસંદગી(val) {
    સ્થિતિ 0:
      clear_col_tabstop(state, state->pos.col);
      તોડો;
    સ્થિતિ 3:
    સ્થિતિ 5:
      માટે(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      તોડો;
    સ્થિતિ 1:
    સ્થિતિ 2:
    સ્થિતિ 4:
      તોડો;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    મૂળભૂત:
      પાછા 0;
    }
    તોડો;

  સ્થિતિ 0x68: // SM - ECMA-48 8.3.125
    જો(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    તોડો;

  સ્થિતિ LEADER('?', 0x68): // DEC private mode set
    માટે(પૂર્ણાંક i = 0; i < argcount; i++) {
      જો(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    તોડો;

  સ્થિતિ 0x6a: // HPB - ECMA-48 8.3.58
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.col -= ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x6b: // VPB - ECMA-48 8.3.159
    ગણતરી = CSI_ARG_ગણતરી(args[0]);
    state->pos.row -= ગણતરી;
    state->at_phantom = 0;
    તોડો;

  સ્થિતિ 0x6c: // RM - ECMA-48 8.3.106
    જો(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    તોડો;

  સ્થિતિ LEADER('?', 0x6c): // DEC private mode reset
    માટે(પૂર્ણાંક i = 0; i < argcount; i++) {
      જો(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    તોડો;

  સ્થિતિ 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    તોડો;

  સ્થિતિ LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    જો (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_ચાવીઓ ? 2 : 0);
      તોડો;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    માટે(પૂર્ણાંક argi = 0; argi < argcount; argi++) {
      long arg;
      પસંદગી(arg = CSI_ARG(args[argi])) {
        સ્થિતિ 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          તોડો;
        સ્થિતિ 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          તોડો;
        સ્થિતિ 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          તોડો;
      }
    }
    તોડો;

  સ્થિતિ LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    જો (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_ચાવીઓ = CSI_ARG(args[1]) == 2;
    }
    તોડો;

  સ્થિતિ LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    જો (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_ચાવીઓ = 0;

      state->mode.kitty_keyboard = 1;
    }
    તોડો;

  સ્થિતિ LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    જો (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    તોડો;

  સ્થિતિ LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    જો (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    તોડો;

  સ્થિતિ 0x6e: // DSR - ECMA-48 8.3.35
  સ્થિતિ LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      અક્ષર *qmark = (leader_byte == '?') ? "?" : "";

      પસંદગી(val) {
      સ્થિતિ 0: સ્થિતિ 1: સ્થિતિ 2: સ્થિતિ 3: સ્થિતિ 4:
        // ignore - these are replies
        તોડો;
      સ્થિતિ 5:
        vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        તોડો;
      સ્થિતિ 6: // CPR - cursor position report
        vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        તોડો;
      }
    }
    તોડો;


  સ્થિતિ INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    તોડો;

  સ્થિતિ LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    તોડો;

  સ્થિતિ LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    તોડો;

  સ્થિતિ INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    પસંદગી(val) {
    સ્થિતિ 0: સ્થિતિ 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      તોડો;
    સ્થિતિ 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      તોડો;
    સ્થિતિ 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      તોડો;
    સ્થિતિ 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      તોડો;
    સ્થિતિ 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ડાબું);
      તોડો;
    સ્થિતિ 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_ડાબું);
      તોડો;
    }

    તોડો;

  સ્થિતિ INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    પસંદગી(val) {
    સ્થિતિ 0: સ્થિતિ 2:
      state->protected_cell = 0;
      તોડો;
    સ્થિતિ 1:
      state->protected_cell = 1;
      તોડો;
    }

    તોડો;

  સ્થિતિ 0x72: // DECSTBM - DEC custom
    state->scrollregion_ટોચ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_ટોચ, 0);
    UBOUND(state->scrollregion_ટોચ, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    જો(state->scrollregion_ટોચ == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    નહીંતર
      UBOUND(state->scrollregion_bottom, state->rows);

    જો(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ટોચ) {
      // Invalid
      state->scrollregion_ટોચ    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    જો(state->mode.origin) {
      state->pos.row += state->scrollregion_ટોચ;
      state->pos.col += SCROLLREGION_ડાબું(state);
    }

    તોડો;

  સ્થિતિ 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_ડાબું = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_જમણું = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_ડાબું, 0);
    UBOUND(state->scrollregion_ડાબું, state->cols);
    LBOUND(state->scrollregion_જમણું, -1);
    જો(state->scrollregion_ડાબું == 0 && state->scrollregion_જમણું == state->cols)
      state->scrollregion_જમણું = -1;
    નહીંતર
      UBOUND(state->scrollregion_જમણું, state->cols);

    જો(state->scrollregion_જમણું > -1 &&
       state->scrollregion_જમણું <= state->scrollregion_ડાબું) {
      // Invalid
      state->scrollregion_ડાબું  = 0;
      state->scrollregion_જમણું = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    જો(state->mode.origin) {
      state->pos.row += state->scrollregion_ટોચ;
      state->pos.col += SCROLLREGION_ડાબું(state);
    }

    તોડો;

  સ્થિતિ 0x74:
    પસંદગી(CSI_ARG(args[0])) {
      સ્થિતિ 8: // CSI 8 ; rows ; cols t  set size
        જો (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	તોડો;
      મૂળભૂત:
	handled = 0;
	તોડો;
    }
    તોડો;

  સ્થિતિ INTERMED('\'', 0x7D): // DECIC
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    જો(!is_cursor_in_scrollregion(state))
      તોડો;

    rect.start_row = state->scrollregion_ટોચ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_જમણું(state);

    scroll(state, rect, 0, -ગણતરી);

    તોડો;

  સ્થિતિ INTERMED('\'', 0x7E): // DECDC
    ગણતરી = CSI_ARG_ગણતરી(args[0]);

    જો(!is_cursor_in_scrollregion(state))
      તોડો;

    rect.start_row = state->scrollregion_ટોચ;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_જમણું(state);

    scroll(state, rect, 0, ગણતરી);

    તોડો;

  મૂળભૂત:
    handled = 0;
    તોડો;
  }

  જો (!handled) {
    જો(state->fallbacks && state->fallbacks->csi)
      જો((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        પાછા 1;

    પાછા 0;
  }

  જો(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_ટોચ);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_ડાબું(state));
    UBOUND(state->pos.col, SCROLLREGION_જમણું(state)-1);
  }
  નહીંતર {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  જો(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  જો(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ટોચ) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_ટોચ);
    abort();
  }

  જો(SCROLLREGION_જમણું(state) <= SCROLLREGION_ડાબું(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_જમણું(state), SCROLLREGION_ડાબું(state));
    abort();
  }
#endif

  પાછા 1;
}

સ્થિર અક્ષર base64_one(uint8_t b)
{
  જો(b < 26)
    પાછા 'A' + b;
  નહીંતર જો(b < 52)
    પાછા 'a' + b - 26;
  નહીંતર જો(b < 62)
    પાછા '0' + b - 52;
  નહીંતર જો(b == 62)
    પાછા '+';
  નહીંતર જો(b == 63)
    પાછા '/';
  પાછા 0;
}

સ્થિર uint8_t unbase64one(અક્ષર c)
{
  જો(c >= 'A' && c <= 'Z')
    પાછા c - 'A';
  નહીંતર જો(c >= 'a' && c <= 'z')
    પાછા c - 'a' + 26;
  નહીંતર જો(c >= '0' && c <= '9')
    પાછા c - '0' + 52;
  નહીંતર જો(c == '+')
    પાછા 62;
  નહીંતર જો(c == '/')
    પાછા 63;

  પાછા 0xFF;
}

સ્થિર શૂન્ય osc_selection(VTermState *state, VTermStringFragment frag)
{
  જો(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  જ્યારે(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    પસંદગી(frag.str[0]) {
      સ્થિતિ 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        તોડો;
      સ્થિતિ 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        તોડો;
      સ્થિતિ 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        તોડો;
      સ્થિતિ 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        તોડો;
      સ્થિતિ '0':
      સ્થિતિ '1':
      સ્થિતિ '2':
      સ્થિતિ '3':
      સ્થિતિ '4':
      સ્થિતિ '5':
      સ્થિતિ '6':
      સ્થિતિ '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        તોડો;

      સ્થિતિ ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        જો(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        તોડો;
    }

    frag.str++;
    frag.len--;
  }

  જો(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    જો(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = ખાલી,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    પાછા;
  }

  જો(state->tmp.selection.state == SELECTION_SELECTED) {
    જો(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    નહીંતર {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  જો(state->tmp.selection.state == SELECTION_QUERY) {
    જો(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    પાછા;
  }

  જો(state->tmp.selection.state == SELECTION_INVALID)
    પાછા;

  જો(state->selection.callbacks->set) {
    size_t bufcur = 0;
    અક્ષર *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    પૂર્ણાંક n = 0;      /* Number of sextets consumed */

    જો(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    જ્યારે((state->selection.buflen - bufcur) >= 3 && frag.len) {
      જો(frag.str[0] == '=') {
        જો(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        જો(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        જ્યારે(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      નહીંતર {
        uint8_t b = unbase64one(frag.str[0]);
        જો(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          જો(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = ખાલી,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          તોડો;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        જો(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      જો(!frag.len || (state->selection.buflen - bufcur) < 3) {
        જો(bufcur) {
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

    જો(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

સ્થિર પૂર્ણાંક on_osc(પૂર્ણાંક command, VTermStringFragment frag, શૂન્ય *user)
{
  VTermState *state = user;

  પસંદગી(command) {
    સ્થિતિ 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      પાછા 1;

    સ્થિતિ 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      પાછા 1;

    સ્થિતિ 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      પાછા 1;

    સ્થિતિ 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        પૂર્ણાંક red = state->default_fg.red;
        પૂર્ણાંક blue = state->default_fg.blue;
        પૂર્ણાંક green = state->default_fg.green;
        vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        પાછા 1;
      }

    સ્થિતિ 11:
      {
	// request background color: <Esc>]11;?<0x07>
	પૂર્ણાંક red = state->default_bg.red;
	પૂર્ણાંક blue = state->default_bg.blue;
	પૂર્ણાંક green = state->default_bg.green;
	vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	પાછા 1;
      }
    સ્થિતિ 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      પાછા 1;

    સ્થિતિ 52:
      જો(state->selection.callbacks)
        osc_selection(state, frag);

      પાછા 1;

    મૂળભૂત:
      જો(state->fallbacks && state->fallbacks->osc)
        જો((*state->fallbacks->osc)(command, frag, state->fbdata))
          પાછા 1;
  }

  પાછા 0;
}

સ્થિર શૂન્ય request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  અક્ષર *tmp = state->tmp.decrqss;

  જો(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  જ્યારે(i < કદ(state->tmp.decrqss)-1 && tmp[i])
    i++;
  જ્યારે(i < કદ(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  જો(!frag.final)
    પાછા;

  પસંદગી(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    સ્થિતિ 'm': {
      // Query SGR
      long args[20];
      પૂર્ણાંક argc = vterm_state_getpen(state, args, કદ(args)/કદ(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      જો(cur >= vt->tmpbuffer_len)
        પાછા;

      માટે(પૂર્ણાંક argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        જો(cur >= vt->tmpbuffer_len)
          પાછા;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      જો(cur >= vt->tmpbuffer_len)
        પાછા;

      vterm_ધકેલો_output_bytes(vt, vt->tmpbuffer, cur);
      પાછા;
    }

    સ્થિતિ 'r':
      // Query DECSTBM
      vterm_ધકેલો_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_ટોચ+1, SCROLLREGION_BOTTOM(state));
      પાછા;

    સ્થિતિ 's':
      // Query DECSLRM
      vterm_ધકેલો_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_ડાબું(state)+1, SCROLLREGION_જમણું(state));
      પાછા;

    સ્થિતિ ' '|('q'<<8): {
      // Query DECSCUSR
      પૂર્ણાંક reply;
      પસંદગી(state->mode.cursor_shape) {
        સ્થિતિ VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; તોડો;
        સ્થિતિ VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; તોડો;
	મૂળભૂત: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; તોડો;
      }
      જો(state->mode.cursor_blink)
        reply--;
      vterm_ધકેલો_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      પાછા;
    }

    સ્થિતિ '\"'|('q'<<8):
      // Query DECSCA
      vterm_ધકેલો_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      પાછા;
  }

  vterm_ધકેલો_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

સ્થિર પૂર્ણાંક on_dcs(અચલ અક્ષર *command, size_t commandlen, VTermStringFragment frag, શૂન્ય *user)
{
  VTermState *state = user;

  જો(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    પાછા 1;
  }
  નહીંતર જો(state->fallbacks && state->fallbacks->dcs)
    જો((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      પાછા 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (પૂર્ણાંક)commandlen, command);
  પાછા 0;
}

સ્થિર પૂર્ણાંક on_apc(VTermStringFragment frag, શૂન્ય *user)
{
  VTermState *state = user;

  જો(state->fallbacks && state->fallbacks->apc)
    જો((*state->fallbacks->apc)(frag, state->fbdata))
      પાછા 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  પાછા 0;
}

સ્થિર પૂર્ણાંક on_pm(VTermStringFragment frag, શૂન્ય *user)
{
  VTermState *state = user;

  જો(state->fallbacks && state->fallbacks->pm)
    જો((*state->fallbacks->pm)(frag, state->fbdata))
      પાછા 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  પાછા 0;
}

સ્થિર પૂર્ણાંક on_sos(VTermStringFragment frag, શૂન્ય *user)
{
  VTermState *state = user;

  જો(state->fallbacks && state->fallbacks->sos)
    જો((*state->fallbacks->sos)(frag, state->fbdata))
      પાછા 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  પાછા 0;
}

સ્થિર પૂર્ણાંક on_resize(પૂર્ણાંક rows, પૂર્ણાંક cols, શૂન્ય *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  જો (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  જો (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  જો(cols != state->cols) {
    unsigned અક્ષર *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    જો (newtabstops == ખાલી)
      પાછા 0;

    /* TODO: This can all be done much more efficiently bytewise */
    પૂર્ણાંક col;
    માટે(col = 0; col < state->cols && col < cols; col++) {
      unsigned અક્ષર mask = 1 << (col & 7);
      જો(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      નહીંતર
        newtabstops[col >> 3] &= ~mask;
      }

    માટે( ; col < cols; col++) {
      unsigned અક્ષર mask = 1 << (col & 7);
      જો(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      નહીંતર
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  જો(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  જો(state->scrollregion_જમણું > -1)
    UBOUND(state->scrollregion_જમણું, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_ટોચ, state->rows);
  UBOUND(state->scrollregion_ડાબું, state->cols);
  જો(SCROLLREGION_BOTTOM(state) <= state->scrollregion_ટોચ) {
    state->scrollregion_ટોચ = 0;
    state->scrollregion_bottom = -1;
  }
  જો(SCROLLREGION_જમણું(state) <= state->scrollregion_ડાબું) {
    state->scrollregion_ડાબું = 0;
    state->scrollregion_જમણું = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  જો(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  નહીંતર {
    જો(rows != state->rows) {
      માટે(પૂર્ણાંક bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_લીટી_Info *oldlineinfo = state->lineinfos[bufidx];
        જો(!oldlineinfo)
          ચાલુ;

        VTerm_લીટી_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * કદ(VTerm_લીટી_Info));

        પૂર્ણાંક row;
        માટે(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        માટે( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_લીટી_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  જો(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  જો(state->pos.row < 0)
    state->pos.row = 0;
  જો(state->pos.row >= rows)
    state->pos.row = rows - 1;
  જો(state->pos.col < 0)
    state->pos.col = 0;
  જો(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  પાછા 1;
}

સ્થિર અચલ VTermParserCallbacks parser_callbacks = {
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
  જો(vt->state)
    પાછા vt->state;

  VTermState *state = vterm_state_new(vt);
  જો (state == ખાલી)
    પાછા ખાલી;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  પાછા state;
}

શૂન્ય vterm_state_reset(VTermState *state, પૂર્ણાંક hard)
{
  state->scrollregion_ટોચ = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_ડાબું = 0;
  state->scrollregion_જમણું = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.ઉમેરો          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  માટે(પૂર્ણાંક col = 0; col < state->cols; col++)
    જો(col % 8 == 0)
      set_col_tabstop(state, col);
    નહીંતર
      clear_col_tabstop(state, col);

  માટે(પૂર્ણાંક row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  જો(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  માટે(પૂર્ણાંક i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    જો(default_enc->init)
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

  જો(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

શૂન્ય vterm_state_get_cursorpos(અચલ VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

શૂન્ય vterm_state_get_mousestate(અચલ VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

શૂન્ય vterm_state_set_callbacks(VTermState *state, અચલ VTermStateCallbacks *callbacks, શૂન્ય *user)
{
  જો(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    જો(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  નહીંતર {
    state->callbacks = ખાલી;
    state->cbdata = ખાલી;
  }
}

શૂન્ય vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

શૂન્ય *vterm_state_get_cbdata(VTermState *state)
{
  પાછા state->cbdata;
}

શૂન્ય vterm_state_set_unrecognised_fallbacks(VTermState *state, અચલ VTermStateFallbacks *fallbacks, શૂન્ય *user)
{
  જો(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  નહીંતર {
    state->fallbacks = ખાલી;
    state->fbdata = ખાલી;
  }
}

શૂન્ય *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  પાછા state->fbdata;
}

પૂર્ણાંક vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_કિંમત *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  જો(state->callbacks && state->callbacks->settermprop)
    જો(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      પાછા 0;

  પસંદગી(prop) {
  સ્થિતિ VTERM_PROP_TITLE:
  સ્થિતિ VTERM_PROP_ICONNAME:
  સ્થિતિ VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    પાછા 1;
  સ્થિતિ VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->સંખ્યા;
    પાછા 1;
  સ્થિતિ VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    જો(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    પાછા 1;
  સ્થિતિ VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    જો(val->સંખ્યા)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    જો(val->સંખ્યા == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    જો(val->સંખ્યા == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    પાછા 1;
  સ્થિતિ VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    પાછા 1;

  સ્થિતિ VTERM_N_PROPS:
    પાછા 0;
  }

  પાછા 0;
}

શૂન્ય vterm_state_focus_in(VTermState *state)
{
  જો(state->mode.report_focus)
    vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

શૂન્ય vterm_state_focus_out(VTermState *state)
{
  જો(state->mode.report_focus)
    vterm_ધકેલો_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

અચલ VTerm_લીટી_Info *vterm_state_get_lineinfo(અચલ VTermState *state, પૂર્ણાંક row)
{
  પાછા state->lineinfo + row;
}

શૂન્ય vterm_state_set_selection_callbacks(VTermState *state, અચલ VTermSelectionCallbacks *callbacks, શૂન્ય *user,
    અક્ષર *buffer, size_t buflen)
{
  જો(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

શૂન્ય vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  જો(frag.initial) {
    /* TODO: support sending more than one mask bit */
    સ્થિર અક્ષર selection_chars[] = "cpqs";
    પૂર્ણાંક idx;
    માટે(idx = 0; idx < 4; idx++)
      જો(mask & (1 << idx))
        તોડો;

    vterm_ધકેલો_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  જો(frag.len) {
    size_t bufcur = 0;
    અક્ષર *buffer = state->selection.buffer;

    uint32_t x = 0;
    પૂર્ણાંક n = 0;

    જો(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    જ્યારે((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      જો(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      જો(!frag.len || (state->selection.buflen - bufcur) < 4) {
        જો(bufcur)
          vterm_ધકેલો_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    જો(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  જો(frag.final) {
    જો(state->tmp.selection.sendpartial) {
      પૂર્ણાંક n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      અક્ષર *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_ધકેલો_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    નહીંતર
      vterm_ધકેલો_output_sprintf_str(vt, 0, TRUE, "");
  }
}
