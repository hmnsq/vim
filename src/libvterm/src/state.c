#ضمن "vterm_internal.h"

#ضمن <stdio.h>
#ضمن <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#ܐܢ defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

ساكن صحيح on_resize(صحيح rows, صحيح cols, فراغ *user);

/* Some convenient wrappers to make callback functions easier */

ساكن فراغ putglyph(VTermState *state, ثابت uint32_t chars[], صحيح width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  ܐܢ(state->callbacks && state->callbacks->putglyph)
    ܐܢ((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      ܦܢܐ;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

ساكن فراغ updatecursor(VTermState *state, VTermPos *oldpos, صحيح cancel_phantom)
{
  ܐܢ(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    ܦܢܐ;

  ܐܢ(cancel_phantom)
    state->at_phantom = 0;

  ܐܢ(state->callbacks && state->callbacks->movecursor)
    ܐܢ((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      ܦܢܐ;
}

ساكن فراغ erase(VTermState *state, VTermRect rect, صحيح selective)
{
  ܐܢ(rect.end_col == state->cols) {
    صحيح row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    لكل(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  ܐܢ(state->callbacks && state->callbacks->erase)
    ܐܢ((*state->callbacks->erase)(rect, selective, state->cbdata))
      ܦܢܐ;
}

ساكن VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, حجم(VTermState));

  ܐܢ (state == عدم)
    ܦܢܐ عدم;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = عدم;
  state->cbdata    = عدم;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = عدم;
  state->selection.user      = عدم;
  state->selection.buffer    = عدم;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * حجم(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * حجم(VTermLineInfo));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * حجم(VTermLineInfo));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  ܐܢ(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  ܦܢܐ state;
}

INTERNAL فراغ vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  ܐܢ(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ساكن فراغ scroll(VTermState *state, VTermRect rect, صحيح downward, صحيح rightward)
{
  صحيح rows;
  صحيح cols;
  ܐܢ(!downward && !rightward)
    ܦܢܐ;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  ܐܢ(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    ܦܢܐ;

  rows = rect.end_row - rect.start_row;
  ܐܢ(downward > rows)
    downward = rows;
  ܐܚܪܢܐ ܐܢ(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  ܐܢ(rightward > cols)
    rightward = cols;
  ܐܚܪܢܐ ܐܢ(rightward < -cols)
    rightward = -cols;

  ܐܢ(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    ܐܢ(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    ܐܚܪܢܐ ܐܢ(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    ܐܢ(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    ܐܚܪܢܐ ܐܢ(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  ܐܢ(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    صحيح height = rect.end_row - rect.start_row - abs(downward);
    صحيح row;
    VTermLineInfo zeroLineInfo = {0x0};

    ܐܢ(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * حجم(state->lineinfo[0]));
      لكل(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zeroLineInfo;
    }
    ܐܚܪܢܐ {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * حجم(state->lineinfo[0]));
      لكل(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zeroLineInfo;
    }
  }

  ܐܢ(state->callbacks && state->callbacks->scrollrect)
    ܐܢ((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      ܦܢܐ;

  ܐܢ(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ساكن فراغ linefeed(VTermState *state)
{
  ܐܢ(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 1, 0);
  }
  ܐܚܪܢܐ ܐܢ(state->pos.row < state->rows-1)
    state->pos.row++;
}

ساكن فراغ grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * حجم(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * حجم(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

ساكن فراغ set_col_tabstop(VTermState *state, صحيح col)
{
  unsigned حرف mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

ساكن فراغ clear_col_tabstop(VTermState *state, صحيح col)
{
  unsigned حرف mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

ساكن صحيح is_col_tabstop(VTermState *state, صحيح col)
{
  unsigned حرف mask = 1 << (col & 7);
  ܦܢܐ state->tabstops[col >> 3] & mask;
}

ساكن صحيح is_cursor_in_scrollregion(ثابت VTermState *state)
{
  ܐܢ(state->pos.row < state->scrollregion_top ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    ܦܢܐ 0;
  ܐܢ(state->pos.col < SCROLLREGION_LEFT(state) ||
     state->pos.col >= SCROLLREGION_RIGHT(state))
    ܦܢܐ 0;

  ܦܢܐ 1;
}

ساكن فراغ tab(VTermState *state, صحيح count, صحيح direction)
{
  ܟܡܐ(count > 0) {
    ܐܢ(direction > 0) {
      ܐܢ(state->pos.col >= THISROWWIDTH(state)-1)
        ܦܢܐ;

      state->pos.col++;
    }
    ܐܚܪܢܐ ܐܢ(direction < 0) {
      ܐܢ(state->pos.col < 1)
        ܦܢܐ;

      state->pos.col--;
    }

    ܐܢ(is_col_tabstop(state, state->pos.col))
      count--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_TOP    1
#define DHL_BOTTOM 2

ساكن فراغ set_lineinfo(VTermState *state, صحيح row, صحيح force, صحيح dwl, صحيح dhl)
{
  VTermLineInfo info = state->lineinfo[row];

  ܐܢ(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  ܐܚܪܢܐ ܐܢ(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  ܐܢ(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  ܐܚܪܢܐ ܐܢ(dhl == DHL_TOP)
    info.doubleheight = DHL_TOP;
  ܐܚܪܢܐ ܐܢ(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  ܐܢ((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

ساكن صحيح on_text(ثابت حرف bytes[], size_t len, فراغ *user)
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
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  ܐܢ (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (صحيح)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  ܐܢ(!npoints)
  {
    ܦܢܐ (صحيح)eaten;
  }

  ܐܢ(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  ܐܢ(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    ܐܢ(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      صحيح printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      لكل(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      صحيح saved_i = 0;
      ܟܡܐ(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      ܟܡܐ(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        ܐܢ(saved_i >= (صحيح)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      ܐܢ(saved_i >= (صحيح)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      لكل(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    ܐܚܪܢܐ {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  لكل(; i < npoints; i++) {
    // Try to find combining characters following this
    صحيح glyph_starts = i;
    صحيح glyph_ends;
    صحيح width = 0;

    لكل(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      ܐܢ(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        اكسر;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * حجم(uint32_t));
    ܐܢ (chars == عدم)
      اكسر;

    لكل( ; i < glyph_ends; i++) {
      صحيح this_width;
      ܐܢ(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        ܐܢ(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      ܐܢ(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      ܐܢ (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    ܐܢ (width < 0)
      width = 0;

    ܟܡܐ(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    صحيح printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    لكل(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    ܐܢ(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    ܐܢ(state->mode.insert) {
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

    ܐܢ(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      صحيح save_i;
      لكل(save_i = 0; chars[save_i]; save_i++) {
        ܐܢ(save_i >= (صحيح)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      ܐܢ(save_i >= (صحيح)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    ܐܢ(state->pos.col + width >= THISROWWIDTH(state)) {
      ܐܢ(state->mode.autowrap)
        state->at_phantom = 1;
    }
    ܐܚܪܢܐ {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  ܐܢ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  ܦܢܐ (صحيح)eaten;
}

ساكن صحيح on_control(unsigned حرف control, فراغ *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  بدل(control) {
  حالة 0x07: // BEL - ECMA-48 8.3.3
    ܐܢ(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    اكسر;

  حالة 0x08: // BS - ECMA-48 8.3.5
    ܐܢ(state->pos.col > 0)
      state->pos.col--;
    ܐܢ(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      ܐܢ(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    اكسر;

  حالة 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    اكسر;

  حالة 0x0a: // LF - ECMA-48 8.3.74
  حالة 0x0b: // VT
  حالة 0x0c: // FF
    linefeed(state);
    ܐܢ(state->mode.newline)
      state->pos.col = 0;
    اكسر;

  حالة 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    اكسر;

  حالة 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    اكسر;

  حالة 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    اكسر;

  حالة 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    اكسر;

  حالة 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    اكسر;

  حالة 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    اكسر;

  حالة 0x8d: // RI - ECMA-48 8.3.104
    ܐܢ(state->pos.row == state->scrollregion_top) {
      VTermRect rect;
      rect.start_row = state->scrollregion_top;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_LEFT(state);
      rect.end_col   = SCROLLREGION_RIGHT(state);

      scroll(state, rect, -1, 0);
    }
    ܐܚܪܢܐ ܐܢ(state->pos.row > 0)
        state->pos.row--;
    اكسر;

  حالة 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    اكسر;

  حالة 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    اكسر;

  افتراضي:
    ܐܢ(state->fallbacks && state->fallbacks->control)
      ܐܢ((*state->fallbacks->control)(control, state->fbdata))
        ܦܢܐ 1;

    ܦܢܐ 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  ܐܢ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  ܦܢܐ 1;
}

ساكن صحيح settermprop_bool(VTermState *state, VTermProp prop, صحيح v)
{
  VTermValue val;
  val.boolean = v;
  ܦܢܐ vterm_state_set_termprop(state, prop, &val);
}

ساكن صحيح settermprop_int(VTermState *state, VTermProp prop, صحيح v)
{
  VTermValue val;
  val.number = v;
  ܦܢܐ vterm_state_set_termprop(state, prop, &val);
}

ساكن صحيح settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTermValue val;

  val.string = frag;
  ܦܢܐ vterm_state_set_termprop(state, prop, &val);
}

ساكن فراغ savecursor(VTermState *state, صحيح save)
{
  ܐܢ(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  ܐܚܪܢܐ {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ساكن صحيح on_escape(ثابت حرف *bytes, size_t len, فراغ *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  بدل(bytes[0]) {
  حالة ' ':
    ܐܢ(len != 2)
      ܦܢܐ 0;

    بدل(bytes[1]) {
      حالة 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        اكسر;

      حالة 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        اكسر;

      افتراضي:
        ܦܢܐ 0;
    }
    ܦܢܐ 2;

  حالة '#':
    ܐܢ(len != 2)
      ܦܢܐ 0;

    بدل(bytes[1]) {
      حالة '3': // DECDHL top
        ܐܢ(state->mode.leftrightmargin)
          اكسر;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_TOP);
        اكسر;

      حالة '4': // DECDHL bottom
        ܐܢ(state->mode.leftrightmargin)
          اكسر;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        اكسر;

      حالة '5': // DECSWL
        ܐܢ(state->mode.leftrightmargin)
          اكسر;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        اكسر;

      حالة '6': // DECDWL
        ܐܢ(state->mode.leftrightmargin)
          اكسر;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        اكسر;

      حالة '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        لكل(pos.row = 0; pos.row < state->rows; pos.row++)
          لكل(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        اكسر;
      }

      افتراضي:
        ܦܢܐ 0;
    }
    ܦܢܐ 2;

  حالة '(': حالة ')': حالة '*': حالة '+': // SCS
    ܐܢ(len != 2)
      ܦܢܐ 0;

    {
      صحيح setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      ܐܢ(newenc) {
        state->encoding[setnum].enc = newenc;

        ܐܢ(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    ܦܢܐ 2;

  حالة '7': // DECSC
    savecursor(state, 1);
    ܦܢܐ 1;

  حالة '8': // DECRC
    savecursor(state, 0);
    ܦܢܐ 1;

  حالة '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    ܦܢܐ 1;

  حالة '=': // DECKPAM
    state->mode.keypad = 1;
    ܦܢܐ 1;

  حالة '>': // DECKPNM
    state->mode.keypad = 0;
    ܦܢܐ 1;

  حالة 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    ܐܢ(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    ܦܢܐ 1;
  }

  حالة 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    ܦܢܐ 1;

  حالة 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    ܦܢܐ 1;

  حالة '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    ܦܢܐ 1;

  حالة '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    ܦܢܐ 1;

  حالة '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    ܦܢܐ 1;

  افتراضي:
    ܦܢܐ 0;
  }
}

ساكن فراغ set_mode(VTermState *state, صحيح num, صحيح val)
{
  بدل(num) {
  حالة 4: // IRM - ECMA-48 7.2.10
    state->mode.insert = val;
    اكسر;

  حالة 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    اكسر;

  افتراضي:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", num);
    ܦܢܐ;
  }
}

ساكن فراغ set_dec_mode(VTermState *state, صحيح num, صحيح val)
{
  بدل(num) {
  حالة 1:
    state->mode.cursor = val;
    اكسر;

  حالة 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    اكسر;

  حالة 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_top : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_LEFT(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    اكسر;

  حالة 7:
    state->mode.autowrap = val;
    اكسر;

  حالة 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    اكسر;

  حالة 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    اكسر;

  حالة 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    ܐܢ(val) {
      صحيح row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      لكل(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    اكسر;

  حالة 1000:
  حالة 1002:
  حالة 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (num == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (num == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    اكسر;

  حالة 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
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
    state->mode.bracketpaste = val;
    اكسر;

  افتراضي:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", num);
    ܦܢܐ;
  }
}

ساكن فراغ request_dec_mode(VTermState *state, صحيح num)
{
  صحيح reply;

  بدل(num) {
    حالة 1:
      reply = state->mode.cursor;
      اكسر;

    حالة 5:
      reply = state->mode.screen;
      اكسر;

    حالة 6:
      reply = state->mode.origin;
      اكسر;

    حالة 7:
      reply = state->mode.autowrap;
      اكسر;

    حالة 12:
      reply = state->mode.cursor_blink;
      اكسر;

    حالة 25:
      reply = state->mode.cursor_visible;
      اكسر;

    حالة 69:
      reply = state->mode.leftrightmargin;
      اكسر;

    حالة 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      اكسر;

    حالة 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      اكسر;

    حالة 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      اكسر;

    حالة 1004:
      reply = state->mode.report_focus;
      اكسر;

    حالة 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      اكسر;

    حالة 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      اكسر;

    حالة 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      اكسر;

    حالة 1047:
      reply = state->mode.alt_screen;
      اكسر;

    حالة 2004:
      reply = state->mode.bracketpaste;
      اكسر;

    افتراضي:
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", num, 0);
      ܦܢܐ;
  }

  vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", num, reply ? 1 : 2);
}

ساكن فراغ request_version_string(VTermState *state)
{
  vterm_push_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

ساكن صحيح on_csi(ثابت حرف *leader, ثابت long args[], صحيح argcount, ثابت حرف *intermed, حرف command, فراغ *user)
{
  VTermState *state = user;
  صحيح leader_byte = 0;
  صحيح intermed_byte = 0;
  صحيح cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  صحيح handled = 1;

  // Some temporaries for later code
  صحيح count, val;
  صحيح row, col;
  VTermRect rect;
  صحيح selective;

  ܐܢ(leader && leader[0]) {
    ܐܢ(leader[1]) // longer than 1 char
      ܦܢܐ 0;

    بدل(leader[0]) {
    حالة '?':
    حالة '>':
    حالة '<':
      leader_byte = leader[0];
      اكسر;
    افتراضي:
      ܦܢܐ 0;
    }
  }

  ܐܢ(intermed && intermed[0]) {
    ܐܢ(intermed[1]) // longer than 1 char
      ܦܢܐ 0;

    بدل(intermed[0]) {
    حالة ' ':
    حالة '!':
    حالة '"':
    حالة '$':
    حالة '\'':
      intermed_byte = intermed[0];
      اكسر;
    افتراضي:
      ܦܢܐ 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) ܐܢ((v) < (min)) (v) = (min)
#define UBOUND(v,max) ܐܢ((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  بدل(intermed_byte << 16 | leader_byte << 8 | command) {
  حالة 0x40: // ICH - ECMA-48 8.3.64
    count = CSI_ARG_COUNT(args[0]);

    ܐܢ(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    ܐܢ(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_RIGHT(state);
    ܐܚܪܢܐ
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -count);

    اكسر;

  حالة 0x41: // CUU - ECMA-48 8.3.22
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row -= count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x42: // CUD - ECMA-48 8.3.19
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row += count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x43: // CUF - ECMA-48 8.3.20
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col += count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x44: // CUB - ECMA-48 8.3.18
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col -= count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x45: // CNL - ECMA-48 8.3.12
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col = 0;
    state->pos.row += count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x46: // CPL - ECMA-48 8.3.13
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col = 0;
    state->pos.row -= count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    اكسر;

  حالة 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    ܐܢ(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      ܐܢ(state->pos.row == row - 1) {
        صحيح cnt, ptr = 0;
        لكل(cnt = 0; cnt < col - 1; ++cnt) {
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
    ܐܢ(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }
    state->at_phantom = 0;
    اكسر;

  حالة 0x49: // CHT - ECMA-48 8.3.10
    count = CSI_ARG_COUNT(args[0]);
    tab(state, count, +1);
    اكسر;

  حالة 0x4a: // ED - ECMA-48 8.3.39
  حالة LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    بدل(CSI_ARG(args[0])) {
    حالة CSI_ARG_MISSING:
    حالة 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      ܐܢ(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      لكل(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      ܐܢ(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      اكسر;

    حالة 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      لكل(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      ܐܢ(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      ܐܢ(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      اكسر;

    حالة 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      لكل(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      اكسر;

    حالة 3:
      ܐܢ(state->callbacks && state->callbacks->sb_clear)
        ܐܢ((*state->callbacks->sb_clear)(state->cbdata))
          ܦܢܐ 1;
      اكسر;
    }
    اكسر;

  حالة 0x4b: // EL - ECMA-48 8.3.41
  حالة LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    بدل(CSI_ARG(args[0])) {
    حالة CSI_ARG_MISSING:
    حالة 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); اكسر;
    حالة 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; اكسر;
    حالة 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); اكسر;
    افتراضي:
      ܦܢܐ 0;
    }

    ܐܢ(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    اكسر;

  حالة 0x4c: // IL - ECMA-48 8.3.67
    count = CSI_ARG_COUNT(args[0]);

    ܐܢ(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, -count, 0);

    اكسر;

  حالة 0x4d: // DL - ECMA-48 8.3.32
    count = CSI_ARG_COUNT(args[0]);

    ܐܢ(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, count, 0);

    اكسر;

  حالة 0x50: // DCH - ECMA-48 8.3.26
    count = CSI_ARG_COUNT(args[0]);

    ܐܢ(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    ܐܢ(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_RIGHT(state);
    ܐܚܪܢܐ
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, count);

    اكسر;

  حالة 0x53: // SU - ECMA-48 8.3.147
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, count, 0);

    اكسر;

  حالة 0x54: // SD - ECMA-48 8.3.113
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, -count, 0);

    اكسر;

  حالة 0x58: // ECH - ECMA-48 8.3.38
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + count;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    اكسر;

  حالة 0x5a: // CBT - ECMA-48 8.3.7
    count = CSI_ARG_COUNT(args[0]);
    tab(state, count, -1);
    اكسر;

  حالة 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    اكسر;

  حالة 0x61: // HPR - ECMA-48 8.3.59
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col += count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x62: { // REP - ECMA-48 8.3.103
    ثابت صحيح row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    ܐܢ(state->combine_width < 1)
      اكسر;

    count = CSI_ARG_COUNT(args[0]);
    col = state->pos.col + count;
    UBOUND(col, row_width);
    ܟܡܐ (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    ܐܢ (state->pos.col + state->combine_width >= row_width) {
      ܐܢ (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    اكسر;
  }

  حالة 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    ܐܢ(val == 0)
      // DEC VT100 response
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    اكسر;

  حالة LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    اكسر;

  حالة 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    ܐܢ(state->mode.origin)
      state->pos.row += state->scrollregion_top;
    state->at_phantom = 0;
    اكسر;

  حالة 0x65: // VPR - ECMA-48 8.3.160
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row += count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    ܐܢ(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }
    state->at_phantom = 0;
    اكسر;

  حالة 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    بدل(val) {
    حالة 0:
      clear_col_tabstop(state, state->pos.col);
      اكسر;
    حالة 3:
    حالة 5:
      لكل(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      اكسر;
    حالة 1:
    حالة 2:
    حالة 4:
      اكسر;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    افتراضي:
      ܦܢܐ 0;
    }
    اكسر;

  حالة 0x68: // SM - ECMA-48 8.3.125
    ܐܢ(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    اكسر;

  حالة LEADER('?', 0x68): // DEC private mode set
    لكل(صحيح i = 0; i < argcount; i++) {
      ܐܢ(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    اكسر;

  حالة 0x6a: // HPB - ECMA-48 8.3.58
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col -= count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6b: // VPB - ECMA-48 8.3.159
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row -= count;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6c: // RM - ECMA-48 8.3.106
    ܐܢ(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    اكسر;

  حالة LEADER('?', 0x6c): // DEC private mode reset
    لكل(صحيح i = 0; i < argcount; i++) {
      ܐܢ(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    اكسر;

  حالة 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    اكسر;

  حالة LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    ܐܢ (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_keys ? 2 : 0);
      اكسر;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    لكل(صحيح argi = 0; argi < argcount; argi++) {
      long arg;
      بدل(arg = CSI_ARG(args[argi])) {
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
    ܐܢ (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_keys = CSI_ARG(args[1]) == 2;
    }
    اكسر;

  حالة LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    ܐܢ (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_keys = 0;

      state->mode.kitty_keyboard = 1;
    }
    اكسر;

  حالة LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    ܐܢ (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    اكسر;

  حالة LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    ܐܢ (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    اكسر;

  حالة 0x6e: // DSR - ECMA-48 8.3.35
  حالة LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      حرف *qmark = (leader_byte == '?') ? "?" : "";

      بدل(val) {
      حالة 0: حالة 1: حالة 2: حالة 3: حالة 4:
        // ignore - these are replies
        اكسر;
      حالة 5:
        vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        اكسر;
      حالة 6: // CPR - cursor position report
        vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        اكسر;
      }
    }
    اكسر;


  حالة INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    اكسر;

  حالة LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    اكسر;

  حالة LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    اكسر;

  حالة INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    بدل(val) {
    حالة 0: حالة 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      اكسر;
    حالة 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
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
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_LEFT);
      اكسر;
    حالة 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_LEFT);
      اكسر;
    }

    اكسر;

  حالة INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

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
    state->scrollregion_top = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_top, 0);
    UBOUND(state->scrollregion_top, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    ܐܢ(state->scrollregion_top == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    ܐܚܪܢܐ
      UBOUND(state->scrollregion_bottom, state->rows);

    ܐܢ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
      // Invalid
      state->scrollregion_top    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    ܐܢ(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }

    اكسر;

  حالة 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_left = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_right = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_left, 0);
    UBOUND(state->scrollregion_left, state->cols);
    LBOUND(state->scrollregion_right, -1);
    ܐܢ(state->scrollregion_left == 0 && state->scrollregion_right == state->cols)
      state->scrollregion_right = -1;
    ܐܚܪܢܐ
      UBOUND(state->scrollregion_right, state->cols);

    ܐܢ(state->scrollregion_right > -1 &&
       state->scrollregion_right <= state->scrollregion_left) {
      // Invalid
      state->scrollregion_left  = 0;
      state->scrollregion_right = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    ܐܢ(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }

    اكسر;

  حالة 0x74:
    بدل(CSI_ARG(args[0])) {
      حالة 8: // CSI 8 ; rows ; cols t  set size
        ܐܢ (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	اكسر;
      افتراضي:
	handled = 0;
	اكسر;
    }
    اكسر;

  حالة INTERMED('\'', 0x7D): // DECIC
    count = CSI_ARG_COUNT(args[0]);

    ܐܢ(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 0, -count);

    اكسر;

  حالة INTERMED('\'', 0x7E): // DECDC
    count = CSI_ARG_COUNT(args[0]);

    ܐܢ(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 0, count);

    اكسر;

  افتراضي:
    handled = 0;
    اكسر;
  }

  ܐܢ (!handled) {
    ܐܢ(state->fallbacks && state->fallbacks->csi)
      ܐܢ((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        ܦܢܐ 1;

    ܦܢܐ 0;
  }

  ܐܢ(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_top);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_LEFT(state));
    UBOUND(state->pos.col, SCROLLREGION_RIGHT(state)-1);
  }
  ܐܚܪܢܐ {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  ܐܢ(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  ܐܢ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_top);
    abort();
  }

  ܐܢ(SCROLLREGION_RIGHT(state) <= SCROLLREGION_LEFT(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_RIGHT(state), SCROLLREGION_LEFT(state));
    abort();
  }
#endif

  ܦܢܐ 1;
}

ساكن حرف base64_one(uint8_t b)
{
  ܐܢ(b < 26)
    ܦܢܐ 'A' + b;
  ܐܚܪܢܐ ܐܢ(b < 52)
    ܦܢܐ 'a' + b - 26;
  ܐܚܪܢܐ ܐܢ(b < 62)
    ܦܢܐ '0' + b - 52;
  ܐܚܪܢܐ ܐܢ(b == 62)
    ܦܢܐ '+';
  ܐܚܪܢܐ ܐܢ(b == 63)
    ܦܢܐ '/';
  ܦܢܐ 0;
}

ساكن uint8_t unbase64one(حرف c)
{
  ܐܢ(c >= 'A' && c <= 'Z')
    ܦܢܐ c - 'A';
  ܐܚܪܢܐ ܐܢ(c >= 'a' && c <= 'z')
    ܦܢܐ c - 'a' + 26;
  ܐܚܪܢܐ ܐܢ(c >= '0' && c <= '9')
    ܦܢܐ c - '0' + 52;
  ܐܚܪܢܐ ܐܢ(c == '+')
    ܦܢܐ 62;
  ܐܚܪܢܐ ܐܢ(c == '/')
    ܦܢܐ 63;

  ܦܢܐ 0xFF;
}

ساكن فراغ osc_selection(VTermState *state, VTermStringFragment frag)
{
  ܐܢ(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  ܟܡܐ(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    بدل(frag.str[0]) {
      حالة 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        اكسر;
      حالة 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        اكسر;
      حالة 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        اكسر;
      حالة 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        اكسر;
      حالة '0':
      حالة '1':
      حالة '2':
      حالة '3':
      حالة '4':
      حالة '5':
      حالة '6':
      حالة '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        اكسر;

      حالة ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        ܐܢ(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        اكسر;
    }

    frag.str++;
    frag.len--;
  }

  ܐܢ(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    ܐܢ(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = عدم,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    ܦܢܐ;
  }

  ܐܢ(state->tmp.selection.state == SELECTION_SELECTED) {
    ܐܢ(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    ܐܚܪܢܐ {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  ܐܢ(state->tmp.selection.state == SELECTION_QUERY) {
    ܐܢ(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    ܦܢܐ;
  }

  ܐܢ(state->tmp.selection.state == SELECTION_INVALID)
    ܦܢܐ;

  ܐܢ(state->selection.callbacks->set) {
    size_t bufcur = 0;
    حرف *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    صحيح n = 0;      /* Number of sextets consumed */

    ܐܢ(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    ܟܡܐ((state->selection.buflen - bufcur) >= 3 && frag.len) {
      ܐܢ(frag.str[0] == '=') {
        ܐܢ(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        ܐܢ(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        ܟܡܐ(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      ܐܚܪܢܐ {
        uint8_t b = unbase64one(frag.str[0]);
        ܐܢ(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          ܐܢ(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
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

        ܐܢ(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      ܐܢ(!frag.len || (state->selection.buflen - bufcur) < 3) {
        ܐܢ(bufcur) {
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

    ܐܢ(n)
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
      ܦܢܐ 1;

    حالة 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      ܦܢܐ 1;

    حالة 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ܦܢܐ 1;

    حالة 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        صحيح red = state->default_fg.red;
        صحيح blue = state->default_fg.blue;
        صحيح green = state->default_fg.green;
        vterm_push_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        ܦܢܐ 1;
      }

    حالة 11:
      {
	// request background color: <Esc>]11;?<0x07>
	صحيح red = state->default_bg.red;
	صحيح blue = state->default_bg.blue;
	صحيح green = state->default_bg.green;
	vterm_push_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	ܦܢܐ 1;
      }
    حالة 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      ܦܢܐ 1;

    حالة 52:
      ܐܢ(state->selection.callbacks)
        osc_selection(state, frag);

      ܦܢܐ 1;

    افتراضي:
      ܐܢ(state->fallbacks && state->fallbacks->osc)
        ܐܢ((*state->fallbacks->osc)(command, frag, state->fbdata))
          ܦܢܐ 1;
  }

  ܦܢܐ 0;
}

ساكن فراغ request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  حرف *tmp = state->tmp.decrqss;

  ܐܢ(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  ܟܡܐ(i < حجم(state->tmp.decrqss)-1 && tmp[i])
    i++;
  ܟܡܐ(i < حجم(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  ܐܢ(!frag.final)
    ܦܢܐ;

  بدل(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    حالة 'm': {
      // Query SGR
      long args[20];
      صحيح argc = vterm_state_getpen(state, args, حجم(args)/حجم(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      ܐܢ(cur >= vt->tmpbuffer_len)
        ܦܢܐ;

      لكل(صحيح argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        ܐܢ(cur >= vt->tmpbuffer_len)
          ܦܢܐ;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      ܐܢ(cur >= vt->tmpbuffer_len)
        ܦܢܐ;

      vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
      ܦܢܐ;
    }

    حالة 'r':
      // Query DECSTBM
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_top+1, SCROLLREGION_BOTTOM(state));
      ܦܢܐ;

    حالة 's':
      // Query DECSLRM
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_LEFT(state)+1, SCROLLREGION_RIGHT(state));
      ܦܢܐ;

    حالة ' '|('q'<<8): {
      // Query DECSCUSR
      صحيح reply;
      بدل(state->mode.cursor_shape) {
        حالة VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; اكسر;
        حالة VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; اكسر;
	افتراضي: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; اكسر;
      }
      ܐܢ(state->mode.cursor_blink)
        reply--;
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      ܦܢܐ;
    }

    حالة '\"'|('q'<<8):
      // Query DECSCA
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      ܦܢܐ;
  }

  vterm_push_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ساكن صحيح on_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  ܐܢ(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    ܦܢܐ 1;
  }
  ܐܚܪܢܐ ܐܢ(state->fallbacks && state->fallbacks->dcs)
    ܐܢ((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      ܦܢܐ 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (صحيح)commandlen, command);
  ܦܢܐ 0;
}

ساكن صحيح on_apc(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  ܐܢ(state->fallbacks && state->fallbacks->apc)
    ܐܢ((*state->fallbacks->apc)(frag, state->fbdata))
      ܦܢܐ 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  ܦܢܐ 0;
}

ساكن صحيح on_pm(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  ܐܢ(state->fallbacks && state->fallbacks->pm)
    ܐܢ((*state->fallbacks->pm)(frag, state->fbdata))
      ܦܢܐ 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  ܦܢܐ 0;
}

ساكن صحيح on_sos(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  ܐܢ(state->fallbacks && state->fallbacks->sos)
    ܐܢ((*state->fallbacks->sos)(frag, state->fbdata))
      ܦܢܐ 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  ܦܢܐ 0;
}

ساكن صحيح on_resize(صحيح rows, صحيح cols, فراغ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  ܐܢ (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  ܐܢ (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  ܐܢ(cols != state->cols) {
    unsigned حرف *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    ܐܢ (newtabstops == عدم)
      ܦܢܐ 0;

    /* TODO: This can all be done much more efficiently bytewise */
    صحيح col;
    لكل(col = 0; col < state->cols && col < cols; col++) {
      unsigned حرف mask = 1 << (col & 7);
      ܐܢ(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      ܐܚܪܢܐ
        newtabstops[col >> 3] &= ~mask;
      }

    لكل( ; col < cols; col++) {
      unsigned حرف mask = 1 << (col & 7);
      ܐܢ(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      ܐܚܪܢܐ
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  ܐܢ(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  ܐܢ(state->scrollregion_right > -1)
    UBOUND(state->scrollregion_right, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_top, state->rows);
  UBOUND(state->scrollregion_left, state->cols);
  ܐܢ(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
    state->scrollregion_top = 0;
    state->scrollregion_bottom = -1;
  }
  ܐܢ(SCROLLREGION_RIGHT(state) <= state->scrollregion_left) {
    state->scrollregion_left = 0;
    state->scrollregion_right = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  ܐܢ(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  ܐܚܪܢܐ {
    ܐܢ(rows != state->rows) {
      لكل(صحيح bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTermLineInfo *oldlineinfo = state->lineinfos[bufidx];
        ܐܢ(!oldlineinfo)
          استمر;

        VTermLineInfo *newlineinfo = vterm_allocator_malloc(state->vt, rows * حجم(VTermLineInfo));

        صحيح row;
        لكل(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        لكل( ; row < rows; row++) {
          newlineinfo[row] = (VTermLineInfo){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  ܐܢ(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  ܐܢ(state->pos.row < 0)
    state->pos.row = 0;
  ܐܢ(state->pos.row >= rows)
    state->pos.row = rows - 1;
  ܐܢ(state->pos.col < 0)
    state->pos.col = 0;
  ܐܢ(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  ܦܢܐ 1;
}

ساكن ثابت VTermParserCallbacks parser_callbacks = {
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
  ܐܢ(vt->state)
    ܦܢܐ vt->state;

  VTermState *state = vterm_state_new(vt);
  ܐܢ (state == عدم)
    ܦܢܐ عدم;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  ܦܢܐ state;
}

فراغ vterm_state_reset(VTermState *state, صحيح hard)
{
  state->scrollregion_top = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_left = 0;
  state->scrollregion_right = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.insert          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  لكل(صحيح col = 0; col < state->cols; col++)
    ܐܢ(col % 8 == 0)
      set_col_tabstop(state, col);
    ܐܚܪܢܐ
      clear_col_tabstop(state, col);

  لكل(صحيح row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  ܐܢ(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  لكل(صحيح i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    ܐܢ(default_enc->init)
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

  ܐܢ(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

فراغ vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

فراغ vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

فراغ vterm_state_set_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, فراغ *user)
{
  ܐܢ(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    ܐܢ(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  ܐܚܪܢܐ {
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
  ܦܢܐ state->cbdata;
}

فراغ vterm_state_set_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
{
  ܐܢ(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  ܐܚܪܢܐ {
    state->fallbacks = عدم;
    state->fbdata = عدم;
  }
}

فراغ *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  ܦܢܐ state->fbdata;
}

صحيح vterm_state_set_termprop(VTermState *state, VTermProp prop, VTermValue *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  ܐܢ(state->callbacks && state->callbacks->settermprop)
    ܐܢ(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      ܦܢܐ 0;

  بدل(prop) {
  حالة VTERM_PROP_TITLE:
  حالة VTERM_PROP_ICONNAME:
  حالة VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    ܦܢܐ 1;
  حالة VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    ܦܢܐ 1;
  حالة VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    ܦܢܐ 1;
  حالة VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->number;
    ܦܢܐ 1;
  حالة VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    ܦܢܐ 1;
  حالة VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    ܐܢ(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    ܦܢܐ 1;
  حالة VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    ܐܢ(val->number)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    ܐܢ(val->number == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    ܐܢ(val->number == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    ܦܢܐ 1;
  حالة VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    ܦܢܐ 1;

  حالة VTERM_N_PROPS:
    ܦܢܐ 0;
  }

  ܦܢܐ 0;
}

فراغ vterm_state_focus_in(VTermState *state)
{
  ܐܢ(state->mode.report_focus)
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

فراغ vterm_state_focus_out(VTermState *state)
{
  ܐܢ(state->mode.report_focus)
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ثابت VTermLineInfo *vterm_state_get_lineinfo(ثابت VTermState *state, صحيح row)
{
  ܦܢܐ state->lineinfo + row;
}

فراغ vterm_state_set_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, فراغ *user,
    حرف *buffer, size_t buflen)
{
  ܐܢ(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

فراغ vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  ܐܢ(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ساكن حرف selection_chars[] = "cpqs";
    صحيح idx;
    لكل(idx = 0; idx < 4; idx++)
      ܐܢ(mask & (1 << idx))
        اكسر;

    vterm_push_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  ܐܢ(frag.len) {
    size_t bufcur = 0;
    حرف *buffer = state->selection.buffer;

    uint32_t x = 0;
    صحيح n = 0;

    ܐܢ(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    ܟܡܐ((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      ܐܢ(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      ܐܢ(!frag.len || (state->selection.buflen - bufcur) < 4) {
        ܐܢ(bufcur)
          vterm_push_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    ܐܢ(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  ܐܢ(frag.final) {
    ܐܢ(state->tmp.selection.sendpartial) {
      صحيح n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      حرف *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_push_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    ܐܚܪܢܐ
      vterm_push_output_sprintf_str(vt, 0, TRUE, "");
  }
}
