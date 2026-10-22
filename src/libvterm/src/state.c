#شامل "vterm_internal.h"

#شامل <stdio.h>
#شامل <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#اگر defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

ایستا صحیح on_resize(صحیح rows, صحیح cols, تهی *user);

/* Some convenient wrappers to make callback functions easier */

ایستا تهی putglyph(VTermState *state, ثابت uint32_t chars[], صحیح width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  اگر(state->callbacks && state->callbacks->putglyph)
    اگر((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      بازگشت;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

ایستا تهی updatecursor(VTermState *state, VTermPos *oldpos, صحیح cancel_phantom)
{
  اگر(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    بازگشت;

  اگر(cancel_phantom)
    state->at_phantom = 0;

  اگر(state->callbacks && state->callbacks->movecursor)
    اگر((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      بازگشت;
}

ایستا تهی erase(VTermState *state, VTermRect rect, صحیح selective)
{
  اگر(rect.end_col == state->cols) {
    صحیح row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    برای(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  اگر(state->callbacks && state->callbacks->erase)
    اگر((*state->callbacks->erase)(rect, selective, state->cbdata))
      بازگشت;
}

ایستا VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, اندازه(VTermState));

  اگر (state == پوچ)
    بازگشت پوچ;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = پوچ;
  state->cbdata    = پوچ;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = پوچ;
  state->selection.user      = پوچ;
  state->selection.buffer    = پوچ;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * اندازه(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * اندازه(VTerm_سطر_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * اندازه(VTerm_سطر_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  اگر(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  بازگشت state;
}

INTERNAL تهی vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  اگر(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ایستا تهی scroll(VTermState *state, VTermRect rect, صحیح downward, صحیح rightward)
{
  صحیح rows;
  صحیح cols;
  اگر(!downward && !rightward)
    بازگشت;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  اگر(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    بازگشت;

  rows = rect.end_row - rect.start_row;
  اگر(downward > rows)
    downward = rows;
  وگرنه اگر(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  اگر(rightward > cols)
    rightward = cols;
  وگرنه اگر(rightward < -cols)
    rightward = -cols;

  اگر(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    اگر(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    وگرنه اگر(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    اگر(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    وگرنه اگر(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  اگر(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    صحیح height = rect.end_row - rect.start_row - abs(downward);
    صحیح row;
    VTerm_سطر_Info zero_سطر_Info = {0x0};

    اگر(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * اندازه(state->lineinfo[0]));
      برای(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_سطر_Info;
    }
    وگرنه {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * اندازه(state->lineinfo[0]));
      برای(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_سطر_Info;
    }
  }

  اگر(state->callbacks && state->callbacks->scrollrect)
    اگر((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      بازگشت;

  اگر(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ایستا تهی linefeed(VTermState *state)
{
  اگر(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_بالاترین;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_چپ(state);
    rect.end_col   = SCROLLREGION_راست(state);

    scroll(state, rect, 1, 0);
  }
  وگرنه اگر(state->pos.row < state->rows-1)
    state->pos.row++;
}

ایستا تهی grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * اندازه(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * اندازه(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

ایستا تهی set_col_tabstop(VTermState *state, صحیح col)
{
  unsigned نویسه mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

ایستا تهی clear_col_tabstop(VTermState *state, صحیح col)
{
  unsigned نویسه mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

ایستا صحیح is_col_tabstop(VTermState *state, صحیح col)
{
  unsigned نویسه mask = 1 << (col & 7);
  بازگشت state->tabstops[col >> 3] & mask;
}

ایستا صحیح is_cursor_in_scrollregion(ثابت VTermState *state)
{
  اگر(state->pos.row < state->scrollregion_بالاترین ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    بازگشت 0;
  اگر(state->pos.col < SCROLLREGION_چپ(state) ||
     state->pos.col >= SCROLLREGION_راست(state))
    بازگشت 0;

  بازگشت 1;
}

ایستا تهی tab(VTermState *state, صحیح شمار, صحیح direction)
{
  وقتی(شمار > 0) {
    اگر(direction > 0) {
      اگر(state->pos.col >= THISROWWIDTH(state)-1)
        بازگشت;

      state->pos.col++;
    }
    وگرنه اگر(direction < 0) {
      اگر(state->pos.col < 1)
        بازگشت;

      state->pos.col--;
    }

    اگر(is_col_tabstop(state, state->pos.col))
      شمار--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_بالاترین    1
#define DHL_BOTTOM 2

ایستا تهی set_lineinfo(VTermState *state, صحیح row, صحیح force, صحیح dwl, صحیح dhl)
{
  VTerm_سطر_Info info = state->lineinfo[row];

  اگر(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  وگرنه اگر(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  اگر(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  وگرنه اگر(dhl == DHL_بالاترین)
    info.doubleheight = DHL_بالاترین;
  وگرنه اگر(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  اگر((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

ایستا صحیح on_text(ثابت نویسه bytes[], size_t len, تهی *user)
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
  size_t maxpoints = (state->vt->tmpbuffer_len) / اندازه(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  اگر (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (صحیح)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  اگر(!npoints)
  {
    بازگشت (صحیح)eaten;
  }

  اگر(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  اگر(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    اگر(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      صحیح printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      برای(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      صحیح saved_i = 0;
      وقتی(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      وقتی(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        اگر(saved_i >= (صحیح)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      اگر(saved_i >= (صحیح)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      برای(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    وگرنه {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  برای(; i < npoints; i++) {
    // Try to find combining characters following this
    صحیح glyph_starts = i;
    صحیح glyph_ends;
    صحیح width = 0;

    برای(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      اگر(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        بشکن;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * اندازه(uint32_t));
    اگر (chars == پوچ)
      بشکن;

    برای( ; i < glyph_ends; i++) {
      صحیح this_width;
      اگر(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        اگر(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      اگر(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      اگر (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    اگر (width < 0)
      width = 0;

    وقتی(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    صحیح printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    برای(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    اگر(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    اگر(state->mode.درج) {
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

    اگر(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      صحیح save_i;
      برای(save_i = 0; chars[save_i]; save_i++) {
        اگر(save_i >= (صحیح)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      اگر(save_i >= (صحیح)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    اگر(state->pos.col + width >= THISROWWIDTH(state)) {
      اگر(state->mode.autowrap)
        state->at_phantom = 1;
    }
    وگرنه {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  اگر(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  بازگشت (صحیح)eaten;
}

ایستا صحیح on_control(unsigned نویسه control, تهی *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  انتخاب(control) {
  حالت 0x07: // BEL - ECMA-48 8.3.3
    اگر(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    بشکن;

  حالت 0x08: // BS - ECMA-48 8.3.5
    اگر(state->pos.col > 0)
      state->pos.col--;
    اگر(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      اگر(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    بشکن;

  حالت 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    بشکن;

  حالت 0x0a: // LF - ECMA-48 8.3.74
  حالت 0x0b: // VT
  حالت 0x0c: // FF
    linefeed(state);
    اگر(state->mode.newline)
      state->pos.col = 0;
    بشکن;

  حالت 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    بشکن;

  حالت 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    بشکن;

  حالت 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    بشکن;

  حالت 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    بشکن;

  حالت 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    بشکن;

  حالت 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    بشکن;

  حالت 0x8d: // RI - ECMA-48 8.3.104
    اگر(state->pos.row == state->scrollregion_بالاترین) {
      VTermRect rect;
      rect.start_row = state->scrollregion_بالاترین;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_چپ(state);
      rect.end_col   = SCROLLREGION_راست(state);

      scroll(state, rect, -1, 0);
    }
    وگرنه اگر(state->pos.row > 0)
        state->pos.row--;
    بشکن;

  حالت 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    بشکن;

  حالت 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    بشکن;

  پیشفرض:
    اگر(state->fallbacks && state->fallbacks->control)
      اگر((*state->fallbacks->control)(control, state->fbdata))
        بازگشت 1;

    بازگشت 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  اگر(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  بازگشت 1;
}

ایستا صحیح settermprop_bool(VTermState *state, VTermProp prop, صحیح v)
{
  VTerm_مقدار val;
  val.boolean = v;
  بازگشت vterm_state_set_termprop(state, prop, &val);
}

ایستا صحیح settermprop_int(VTermState *state, VTermProp prop, صحیح v)
{
  VTerm_مقدار val;
  val.عدد = v;
  بازگشت vterm_state_set_termprop(state, prop, &val);
}

ایستا صحیح settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_مقدار val;

  val.string = frag;
  بازگشت vterm_state_set_termprop(state, prop, &val);
}

ایستا تهی savecursor(VTermState *state, صحیح save)
{
  اگر(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  وگرنه {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

ایستا صحیح on_escape(ثابت نویسه *bytes, size_t len, تهی *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  انتخاب(bytes[0]) {
  حالت ' ':
    اگر(len != 2)
      بازگشت 0;

    انتخاب(bytes[1]) {
      حالت 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        بشکن;

      حالت 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        بشکن;

      پیشفرض:
        بازگشت 0;
    }
    بازگشت 2;

  حالت '#':
    اگر(len != 2)
      بازگشت 0;

    انتخاب(bytes[1]) {
      حالت '3': // DECDHL top
        اگر(state->mode.leftrightmargin)
          بشکن;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_بالاترین);
        بشکن;

      حالت '4': // DECDHL bottom
        اگر(state->mode.leftrightmargin)
          بشکن;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        بشکن;

      حالت '5': // DECSWL
        اگر(state->mode.leftrightmargin)
          بشکن;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        بشکن;

      حالت '6': // DECDWL
        اگر(state->mode.leftrightmargin)
          بشکن;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        بشکن;

      حالت '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        برای(pos.row = 0; pos.row < state->rows; pos.row++)
          برای(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        بشکن;
      }

      پیشفرض:
        بازگشت 0;
    }
    بازگشت 2;

  حالت '(': حالت ')': حالت '*': حالت '+': // SCS
    اگر(len != 2)
      بازگشت 0;

    {
      صحیح setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      اگر(newenc) {
        state->encoding[setnum].enc = newenc;

        اگر(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    بازگشت 2;

  حالت '7': // DECSC
    savecursor(state, 1);
    بازگشت 1;

  حالت '8': // DECRC
    savecursor(state, 0);
    بازگشت 1;

  حالت '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    بازگشت 1;

  حالت '=': // DECKPAM
    state->mode.keypad = 1;
    بازگشت 1;

  حالت '>': // DECKPNM
    state->mode.keypad = 0;
    بازگشت 1;

  حالت 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    اگر(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    بازگشت 1;
  }

  حالت 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    بازگشت 1;

  حالت 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    بازگشت 1;

  حالت '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    بازگشت 1;

  حالت '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    بازگشت 1;

  حالت '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    بازگشت 1;

  پیشفرض:
    بازگشت 0;
  }
}

ایستا تهی set_mode(VTermState *state, صحیح رقم, صحیح val)
{
  انتخاب(رقم) {
  حالت 4: // IRM - ECMA-48 7.2.10
    state->mode.درج = val;
    بشکن;

  حالت 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    بشکن;

  پیشفرض:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", رقم);
    بازگشت;
  }
}

ایستا تهی set_dec_mode(VTermState *state, صحیح رقم, صحیح val)
{
  انتخاب(رقم) {
  حالت 1:
    state->mode.cursor = val;
    بشکن;

  حالت 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    بشکن;

  حالت 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_بالاترین : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_چپ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    بشکن;

  حالت 7:
    state->mode.autowrap = val;
    بشکن;

  حالت 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    بشکن;

  حالت 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    بشکن;

  حالت 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    اگر(val) {
      صحیح row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      برای(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    بشکن;

  حالت 1000:
  حالت 1002:
  حالت 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (رقم == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (رقم == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    بشکن;

  حالت 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    بشکن;

  حالت 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    بشکن;

  حالت 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    بشکن;

  حالت 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    بشکن;

  حالت 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    بشکن;

  حالت 1048:
    savecursor(state, val);
    بشکن;

  حالت 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    بشکن;

  حالت 2004:
    state->mode.bracketpaste = val;
    بشکن;

  پیشفرض:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", رقم);
    بازگشت;
  }
}

ایستا تهی request_dec_mode(VTermState *state, صحیح رقم)
{
  صحیح reply;

  انتخاب(رقم) {
    حالت 1:
      reply = state->mode.cursor;
      بشکن;

    حالت 5:
      reply = state->mode.screen;
      بشکن;

    حالت 6:
      reply = state->mode.origin;
      بشکن;

    حالت 7:
      reply = state->mode.autowrap;
      بشکن;

    حالت 12:
      reply = state->mode.cursor_blink;
      بشکن;

    حالت 25:
      reply = state->mode.cursor_visible;
      بشکن;

    حالت 69:
      reply = state->mode.leftrightmargin;
      بشکن;

    حالت 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      بشکن;

    حالت 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      بشکن;

    حالت 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      بشکن;

    حالت 1004:
      reply = state->mode.report_focus;
      بشکن;

    حالت 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      بشکن;

    حالت 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      بشکن;

    حالت 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      بشکن;

    حالت 1047:
      reply = state->mode.alt_screen;
      بشکن;

    حالت 2004:
      reply = state->mode.bracketpaste;
      بشکن;

    پیشفرض:
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", رقم, 0);
      بازگشت;
  }

  vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", رقم, reply ? 1 : 2);
}

ایستا تهی request_version_string(VTermState *state)
{
  vterm_افزودن_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

ایستا صحیح on_csi(ثابت نویسه *leader, ثابت long args[], صحیح argcount, ثابت نویسه *intermed, نویسه command, تهی *user)
{
  VTermState *state = user;
  صحیح leader_byte = 0;
  صحیح intermed_byte = 0;
  صحیح cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  صحیح handled = 1;

  // Some temporaries for later code
  صحیح شمار, val;
  صحیح row, col;
  VTermRect rect;
  صحیح selective;

  اگر(leader && leader[0]) {
    اگر(leader[1]) // longer than 1 char
      بازگشت 0;

    انتخاب(leader[0]) {
    حالت '?':
    حالت '>':
    حالت '<':
      leader_byte = leader[0];
      بشکن;
    پیشفرض:
      بازگشت 0;
    }
  }

  اگر(intermed && intermed[0]) {
    اگر(intermed[1]) // longer than 1 char
      بازگشت 0;

    انتخاب(intermed[0]) {
    حالت ' ':
    حالت '!':
    حالت '"':
    حالت '$':
    حالت '\'':
      intermed_byte = intermed[0];
      بشکن;
    پیشفرض:
      بازگشت 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) اگر((v) < (min)) (v) = (min)
#define UBOUND(v,max) اگر((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  انتخاب(intermed_byte << 16 | leader_byte << 8 | command) {
  حالت 0x40: // ICH - ECMA-48 8.3.64
    شمار = CSI_ARG_شمار(args[0]);

    اگر(!is_cursor_in_scrollregion(state))
      بشکن;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    اگر(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_راست(state);
    وگرنه
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -شمار);

    بشکن;

  حالت 0x41: // CUU - ECMA-48 8.3.22
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.row -= شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x42: // CUD - ECMA-48 8.3.19
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.row += شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x43: // CUF - ECMA-48 8.3.20
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.col += شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x44: // CUB - ECMA-48 8.3.18
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.col -= شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x45: // CNL - ECMA-48 8.3.12
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.col = 0;
    state->pos.row += شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x46: // CPL - ECMA-48 8.3.13
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.col = 0;
    state->pos.row -= شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    بشکن;

  حالت 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    اگر(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      اگر(state->pos.row == row - 1) {
        صحیح cnt, ptr = 0;
        برای(cnt = 0; cnt < col - 1; ++cnt) {
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
    اگر(state->mode.origin) {
      state->pos.row += state->scrollregion_بالاترین;
      state->pos.col += SCROLLREGION_چپ(state);
    }
    state->at_phantom = 0;
    بشکن;

  حالت 0x49: // CHT - ECMA-48 8.3.10
    شمار = CSI_ARG_شمار(args[0]);
    tab(state, شمار, +1);
    بشکن;

  حالت 0x4a: // ED - ECMA-48 8.3.39
  حالت LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    انتخاب(CSI_ARG(args[0])) {
    حالت CSI_ARG_MISSING:
    حالت 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      اگر(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      برای(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      اگر(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      بشکن;

    حالت 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      برای(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      اگر(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      اگر(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      بشکن;

    حالت 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      برای(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      بشکن;

    حالت 3:
      اگر(state->callbacks && state->callbacks->sb_clear)
        اگر((*state->callbacks->sb_clear)(state->cbdata))
          بازگشت 1;
      بشکن;
    }
    بشکن;

  حالت 0x4b: // EL - ECMA-48 8.3.41
  حالت LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    انتخاب(CSI_ARG(args[0])) {
    حالت CSI_ARG_MISSING:
    حالت 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); بشکن;
    حالت 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; بشکن;
    حالت 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); بشکن;
    پیشفرض:
      بازگشت 0;
    }

    اگر(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    بشکن;

  حالت 0x4c: // IL - ECMA-48 8.3.67
    شمار = CSI_ARG_شمار(args[0]);

    اگر(!is_cursor_in_scrollregion(state))
      بشکن;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_چپ(state);
    rect.end_col   = SCROLLREGION_راست(state);

    scroll(state, rect, -شمار, 0);

    بشکن;

  حالت 0x4d: // DL - ECMA-48 8.3.32
    شمار = CSI_ARG_شمار(args[0]);

    اگر(!is_cursor_in_scrollregion(state))
      بشکن;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_چپ(state);
    rect.end_col   = SCROLLREGION_راست(state);

    scroll(state, rect, شمار, 0);

    بشکن;

  حالت 0x50: // DCH - ECMA-48 8.3.26
    شمار = CSI_ARG_شمار(args[0]);

    اگر(!is_cursor_in_scrollregion(state))
      بشکن;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    اگر(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_راست(state);
    وگرنه
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, شمار);

    بشکن;

  حالت 0x53: // SU - ECMA-48 8.3.147
    شمار = CSI_ARG_شمار(args[0]);

    rect.start_row = state->scrollregion_بالاترین;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_چپ(state);
    rect.end_col   = SCROLLREGION_راست(state);

    scroll(state, rect, شمار, 0);

    بشکن;

  حالت 0x54: // SD - ECMA-48 8.3.113
    شمار = CSI_ARG_شمار(args[0]);

    rect.start_row = state->scrollregion_بالاترین;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_چپ(state);
    rect.end_col   = SCROLLREGION_راست(state);

    scroll(state, rect, -شمار, 0);

    بشکن;

  حالت 0x58: // ECH - ECMA-48 8.3.38
    شمار = CSI_ARG_شمار(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + شمار;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    بشکن;

  حالت 0x5a: // CBT - ECMA-48 8.3.7
    شمار = CSI_ARG_شمار(args[0]);
    tab(state, شمار, -1);
    بشکن;

  حالت 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    بشکن;

  حالت 0x61: // HPR - ECMA-48 8.3.59
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.col += شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x62: { // REP - ECMA-48 8.3.103
    ثابت صحیح row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    اگر(state->combine_width < 1)
      بشکن;

    شمار = CSI_ARG_شمار(args[0]);
    col = state->pos.col + شمار;
    UBOUND(col, row_width);
    وقتی (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    اگر (state->pos.col + state->combine_width >= row_width) {
      اگر (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    بشکن;
  }

  حالت 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    اگر(val == 0)
      // DEC VT100 response
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    بشکن;

  حالت LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    بشکن;

  حالت 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    اگر(state->mode.origin)
      state->pos.row += state->scrollregion_بالاترین;
    state->at_phantom = 0;
    بشکن;

  حالت 0x65: // VPR - ECMA-48 8.3.160
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.row += شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    اگر(state->mode.origin) {
      state->pos.row += state->scrollregion_بالاترین;
      state->pos.col += SCROLLREGION_چپ(state);
    }
    state->at_phantom = 0;
    بشکن;

  حالت 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    انتخاب(val) {
    حالت 0:
      clear_col_tabstop(state, state->pos.col);
      بشکن;
    حالت 3:
    حالت 5:
      برای(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      بشکن;
    حالت 1:
    حالت 2:
    حالت 4:
      بشکن;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    پیشفرض:
      بازگشت 0;
    }
    بشکن;

  حالت 0x68: // SM - ECMA-48 8.3.125
    اگر(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    بشکن;

  حالت LEADER('?', 0x68): // DEC private mode set
    برای(صحیح i = 0; i < argcount; i++) {
      اگر(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    بشکن;

  حالت 0x6a: // HPB - ECMA-48 8.3.58
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.col -= شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x6b: // VPB - ECMA-48 8.3.159
    شمار = CSI_ARG_شمار(args[0]);
    state->pos.row -= شمار;
    state->at_phantom = 0;
    بشکن;

  حالت 0x6c: // RM - ECMA-48 8.3.106
    اگر(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    بشکن;

  حالت LEADER('?', 0x6c): // DEC private mode reset
    برای(صحیح i = 0; i < argcount; i++) {
      اگر(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    بشکن;

  حالت 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    بشکن;

  حالت LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    اگر (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_کلیدها ? 2 : 0);
      بشکن;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    برای(صحیح argi = 0; argi < argcount; argi++) {
      long arg;
      انتخاب(arg = CSI_ARG(args[argi])) {
        حالت 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          بشکن;
        حالت 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          بشکن;
        حالت 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          بشکن;
      }
    }
    بشکن;

  حالت LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    اگر (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_کلیدها = CSI_ARG(args[1]) == 2;
    }
    بشکن;

  حالت LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    اگر (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_کلیدها = 0;

      state->mode.kitty_keyboard = 1;
    }
    بشکن;

  حالت LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    اگر (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    بشکن;

  حالت LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    اگر (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    بشکن;

  حالت 0x6e: // DSR - ECMA-48 8.3.35
  حالت LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      نویسه *qmark = (leader_byte == '?') ? "?" : "";

      انتخاب(val) {
      حالت 0: حالت 1: حالت 2: حالت 3: حالت 4:
        // ignore - these are replies
        بشکن;
      حالت 5:
        vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        بشکن;
      حالت 6: // CPR - cursor position report
        vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        بشکن;
      }
    }
    بشکن;


  حالت INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    بشکن;

  حالت LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    بشکن;

  حالت LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    بشکن;

  حالت INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    انتخاب(val) {
    حالت 0: حالت 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      بشکن;
    حالت 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      بشکن;
    حالت 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      بشکن;
    حالت 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      بشکن;
    حالت 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_چپ);
      بشکن;
    حالت 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_چپ);
      بشکن;
    }

    بشکن;

  حالت INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    انتخاب(val) {
    حالت 0: حالت 2:
      state->protected_cell = 0;
      بشکن;
    حالت 1:
      state->protected_cell = 1;
      بشکن;
    }

    بشکن;

  حالت 0x72: // DECSTBM - DEC custom
    state->scrollregion_بالاترین = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_بالاترین, 0);
    UBOUND(state->scrollregion_بالاترین, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    اگر(state->scrollregion_بالاترین == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    وگرنه
      UBOUND(state->scrollregion_bottom, state->rows);

    اگر(SCROLLREGION_BOTTOM(state) <= state->scrollregion_بالاترین) {
      // Invalid
      state->scrollregion_بالاترین    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    اگر(state->mode.origin) {
      state->pos.row += state->scrollregion_بالاترین;
      state->pos.col += SCROLLREGION_چپ(state);
    }

    بشکن;

  حالت 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_چپ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_راست = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_چپ, 0);
    UBOUND(state->scrollregion_چپ, state->cols);
    LBOUND(state->scrollregion_راست, -1);
    اگر(state->scrollregion_چپ == 0 && state->scrollregion_راست == state->cols)
      state->scrollregion_راست = -1;
    وگرنه
      UBOUND(state->scrollregion_راست, state->cols);

    اگر(state->scrollregion_راست > -1 &&
       state->scrollregion_راست <= state->scrollregion_چپ) {
      // Invalid
      state->scrollregion_چپ  = 0;
      state->scrollregion_راست = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    اگر(state->mode.origin) {
      state->pos.row += state->scrollregion_بالاترین;
      state->pos.col += SCROLLREGION_چپ(state);
    }

    بشکن;

  حالت 0x74:
    انتخاب(CSI_ARG(args[0])) {
      حالت 8: // CSI 8 ; rows ; cols t  set size
        اگر (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	بشکن;
      پیشفرض:
	handled = 0;
	بشکن;
    }
    بشکن;

  حالت INTERMED('\'', 0x7D): // DECIC
    شمار = CSI_ARG_شمار(args[0]);

    اگر(!is_cursor_in_scrollregion(state))
      بشکن;

    rect.start_row = state->scrollregion_بالاترین;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_راست(state);

    scroll(state, rect, 0, -شمار);

    بشکن;

  حالت INTERMED('\'', 0x7E): // DECDC
    شمار = CSI_ARG_شمار(args[0]);

    اگر(!is_cursor_in_scrollregion(state))
      بشکن;

    rect.start_row = state->scrollregion_بالاترین;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_راست(state);

    scroll(state, rect, 0, شمار);

    بشکن;

  پیشفرض:
    handled = 0;
    بشکن;
  }

  اگر (!handled) {
    اگر(state->fallbacks && state->fallbacks->csi)
      اگر((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        بازگشت 1;

    بازگشت 0;
  }

  اگر(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_بالاترین);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_چپ(state));
    UBOUND(state->pos.col, SCROLLREGION_راست(state)-1);
  }
  وگرنه {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  اگر(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  اگر(SCROLLREGION_BOTTOM(state) <= state->scrollregion_بالاترین) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_بالاترین);
    abort();
  }

  اگر(SCROLLREGION_راست(state) <= SCROLLREGION_چپ(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_راست(state), SCROLLREGION_چپ(state));
    abort();
  }
#endif

  بازگشت 1;
}

ایستا نویسه base64_one(uint8_t b)
{
  اگر(b < 26)
    بازگشت 'A' + b;
  وگرنه اگر(b < 52)
    بازگشت 'a' + b - 26;
  وگرنه اگر(b < 62)
    بازگشت '0' + b - 52;
  وگرنه اگر(b == 62)
    بازگشت '+';
  وگرنه اگر(b == 63)
    بازگشت '/';
  بازگشت 0;
}

ایستا uint8_t unbase64one(نویسه c)
{
  اگر(c >= 'A' && c <= 'Z')
    بازگشت c - 'A';
  وگرنه اگر(c >= 'a' && c <= 'z')
    بازگشت c - 'a' + 26;
  وگرنه اگر(c >= '0' && c <= '9')
    بازگشت c - '0' + 52;
  وگرنه اگر(c == '+')
    بازگشت 62;
  وگرنه اگر(c == '/')
    بازگشت 63;

  بازگشت 0xFF;
}

ایستا تهی osc_selection(VTermState *state, VTermStringFragment frag)
{
  اگر(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  وقتی(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    انتخاب(frag.str[0]) {
      حالت 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        بشکن;
      حالت 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        بشکن;
      حالت 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        بشکن;
      حالت 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        بشکن;
      حالت '0':
      حالت '1':
      حالت '2':
      حالت '3':
      حالت '4':
      حالت '5':
      حالت '6':
      حالت '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        بشکن;

      حالت ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        اگر(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        بشکن;
    }

    frag.str++;
    frag.len--;
  }

  اگر(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    اگر(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = پوچ,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    بازگشت;
  }

  اگر(state->tmp.selection.state == SELECTION_SELECTED) {
    اگر(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    وگرنه {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  اگر(state->tmp.selection.state == SELECTION_QUERY) {
    اگر(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    بازگشت;
  }

  اگر(state->tmp.selection.state == SELECTION_INVALID)
    بازگشت;

  اگر(state->selection.callbacks->set) {
    size_t bufcur = 0;
    نویسه *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    صحیح n = 0;      /* Number of sextets consumed */

    اگر(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    وقتی((state->selection.buflen - bufcur) >= 3 && frag.len) {
      اگر(frag.str[0] == '=') {
        اگر(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        اگر(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        وقتی(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      وگرنه {
        uint8_t b = unbase64one(frag.str[0]);
        اگر(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          اگر(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = پوچ,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          بشکن;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        اگر(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      اگر(!frag.len || (state->selection.buflen - bufcur) < 3) {
        اگر(bufcur) {
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

    اگر(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

ایستا صحیح on_osc(صحیح command, VTermStringFragment frag, تهی *user)
{
  VTermState *state = user;

  انتخاب(command) {
    حالت 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      بازگشت 1;

    حالت 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      بازگشت 1;

    حالت 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      بازگشت 1;

    حالت 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        صحیح red = state->default_fg.red;
        صحیح blue = state->default_fg.blue;
        صحیح green = state->default_fg.green;
        vterm_افزودن_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        بازگشت 1;
      }

    حالت 11:
      {
	// request background color: <Esc>]11;?<0x07>
	صحیح red = state->default_bg.red;
	صحیح blue = state->default_bg.blue;
	صحیح green = state->default_bg.green;
	vterm_افزودن_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	بازگشت 1;
      }
    حالت 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      بازگشت 1;

    حالت 52:
      اگر(state->selection.callbacks)
        osc_selection(state, frag);

      بازگشت 1;

    پیشفرض:
      اگر(state->fallbacks && state->fallbacks->osc)
        اگر((*state->fallbacks->osc)(command, frag, state->fbdata))
          بازگشت 1;
  }

  بازگشت 0;
}

ایستا تهی request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  نویسه *tmp = state->tmp.decrqss;

  اگر(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  وقتی(i < اندازه(state->tmp.decrqss)-1 && tmp[i])
    i++;
  وقتی(i < اندازه(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  اگر(!frag.final)
    بازگشت;

  انتخاب(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    حالت 'm': {
      // Query SGR
      long args[20];
      صحیح argc = vterm_state_getpen(state, args, اندازه(args)/اندازه(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      اگر(cur >= vt->tmpbuffer_len)
        بازگشت;

      برای(صحیح argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        اگر(cur >= vt->tmpbuffer_len)
          بازگشت;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      اگر(cur >= vt->tmpbuffer_len)
        بازگشت;

      vterm_افزودن_output_bytes(vt, vt->tmpbuffer, cur);
      بازگشت;
    }

    حالت 'r':
      // Query DECSTBM
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_بالاترین+1, SCROLLREGION_BOTTOM(state));
      بازگشت;

    حالت 's':
      // Query DECSLRM
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_چپ(state)+1, SCROLLREGION_راست(state));
      بازگشت;

    حالت ' '|('q'<<8): {
      // Query DECSCUSR
      صحیح reply;
      انتخاب(state->mode.cursor_shape) {
        حالت VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; بشکن;
        حالت VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; بشکن;
	پیشفرض: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; بشکن;
      }
      اگر(state->mode.cursor_blink)
        reply--;
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      بازگشت;
    }

    حالت '\"'|('q'<<8):
      // Query DECSCA
      vterm_افزودن_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      بازگشت;
  }

  vterm_افزودن_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ایستا صحیح on_dcs(ثابت نویسه *command, size_t commandlen, VTermStringFragment frag, تهی *user)
{
  VTermState *state = user;

  اگر(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    بازگشت 1;
  }
  وگرنه اگر(state->fallbacks && state->fallbacks->dcs)
    اگر((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      بازگشت 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (صحیح)commandlen, command);
  بازگشت 0;
}

ایستا صحیح on_apc(VTermStringFragment frag, تهی *user)
{
  VTermState *state = user;

  اگر(state->fallbacks && state->fallbacks->apc)
    اگر((*state->fallbacks->apc)(frag, state->fbdata))
      بازگشت 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  بازگشت 0;
}

ایستا صحیح on_pm(VTermStringFragment frag, تهی *user)
{
  VTermState *state = user;

  اگر(state->fallbacks && state->fallbacks->pm)
    اگر((*state->fallbacks->pm)(frag, state->fbdata))
      بازگشت 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  بازگشت 0;
}

ایستا صحیح on_sos(VTermStringFragment frag, تهی *user)
{
  VTermState *state = user;

  اگر(state->fallbacks && state->fallbacks->sos)
    اگر((*state->fallbacks->sos)(frag, state->fbdata))
      بازگشت 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  بازگشت 0;
}

ایستا صحیح on_resize(صحیح rows, صحیح cols, تهی *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  اگر (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  اگر (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  اگر(cols != state->cols) {
    unsigned نویسه *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    اگر (newtabstops == پوچ)
      بازگشت 0;

    /* TODO: This can all be done much more efficiently bytewise */
    صحیح col;
    برای(col = 0; col < state->cols && col < cols; col++) {
      unsigned نویسه mask = 1 << (col & 7);
      اگر(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      وگرنه
        newtabstops[col >> 3] &= ~mask;
      }

    برای( ; col < cols; col++) {
      unsigned نویسه mask = 1 << (col & 7);
      اگر(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      وگرنه
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  اگر(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  اگر(state->scrollregion_راست > -1)
    UBOUND(state->scrollregion_راست, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_بالاترین, state->rows);
  UBOUND(state->scrollregion_چپ, state->cols);
  اگر(SCROLLREGION_BOTTOM(state) <= state->scrollregion_بالاترین) {
    state->scrollregion_بالاترین = 0;
    state->scrollregion_bottom = -1;
  }
  اگر(SCROLLREGION_راست(state) <= state->scrollregion_چپ) {
    state->scrollregion_چپ = 0;
    state->scrollregion_راست = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  اگر(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  وگرنه {
    اگر(rows != state->rows) {
      برای(صحیح bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_سطر_Info *oldlineinfo = state->lineinfos[bufidx];
        اگر(!oldlineinfo)
          ادامه;

        VTerm_سطر_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * اندازه(VTerm_سطر_Info));

        صحیح row;
        برای(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        برای( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_سطر_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  اگر(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  اگر(state->pos.row < 0)
    state->pos.row = 0;
  اگر(state->pos.row >= rows)
    state->pos.row = rows - 1;
  اگر(state->pos.col < 0)
    state->pos.col = 0;
  اگر(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  بازگشت 1;
}

ایستا ثابت VTermParserCallbacks parser_callbacks = {
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
  اگر(vt->state)
    بازگشت vt->state;

  VTermState *state = vterm_state_new(vt);
  اگر (state == پوچ)
    بازگشت پوچ;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  بازگشت state;
}

تهی vterm_state_reset(VTermState *state, صحیح hard)
{
  state->scrollregion_بالاترین = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_چپ = 0;
  state->scrollregion_راست = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.درج          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  برای(صحیح col = 0; col < state->cols; col++)
    اگر(col % 8 == 0)
      set_col_tabstop(state, col);
    وگرنه
      clear_col_tabstop(state, col);

  برای(صحیح row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  اگر(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  برای(صحیح i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    اگر(default_enc->init)
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

  اگر(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

تهی vterm_state_get_cursorpos(ثابت VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

تهی vterm_state_get_mousestate(ثابت VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

تهی vterm_state_set_callbacks(VTermState *state, ثابت VTermStateCallbacks *callbacks, تهی *user)
{
  اگر(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    اگر(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  وگرنه {
    state->callbacks = پوچ;
    state->cbdata = پوچ;
  }
}

تهی vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

تهی *vterm_state_get_cbdata(VTermState *state)
{
  بازگشت state->cbdata;
}

تهی vterm_state_set_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, تهی *user)
{
  اگر(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  وگرنه {
    state->fallbacks = پوچ;
    state->fbdata = پوچ;
  }
}

تهی *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  بازگشت state->fbdata;
}

صحیح vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_مقدار *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  اگر(state->callbacks && state->callbacks->settermprop)
    اگر(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      بازگشت 0;

  انتخاب(prop) {
  حالت VTERM_PROP_TITLE:
  حالت VTERM_PROP_ICONNAME:
  حالت VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    بازگشت 1;
  حالت VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    بازگشت 1;
  حالت VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    بازگشت 1;
  حالت VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->عدد;
    بازگشت 1;
  حالت VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    بازگشت 1;
  حالت VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    اگر(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    بازگشت 1;
  حالت VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    اگر(val->عدد)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    اگر(val->عدد == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    اگر(val->عدد == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    بازگشت 1;
  حالت VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    بازگشت 1;

  حالت VTERM_N_PROPS:
    بازگشت 0;
  }

  بازگشت 0;
}

تهی vterm_state_focus_in(VTermState *state)
{
  اگر(state->mode.report_focus)
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

تهی vterm_state_focus_out(VTermState *state)
{
  اگر(state->mode.report_focus)
    vterm_افزودن_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ثابت VTerm_سطر_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحیح row)
{
  بازگشت state->lineinfo + row;
}

تهی vterm_state_set_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, تهی *user,
    نویسه *buffer, size_t buflen)
{
  اگر(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

تهی vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  اگر(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ایستا نویسه selection_chars[] = "cpqs";
    صحیح idx;
    برای(idx = 0; idx < 4; idx++)
      اگر(mask & (1 << idx))
        بشکن;

    vterm_افزودن_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  اگر(frag.len) {
    size_t bufcur = 0;
    نویسه *buffer = state->selection.buffer;

    uint32_t x = 0;
    صحیح n = 0;

    اگر(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    وقتی((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      اگر(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      اگر(!frag.len || (state->selection.buflen - bufcur) < 4) {
        اگر(bufcur)
          vterm_افزودن_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
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
      نویسه *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_افزودن_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    وگرنه
      vterm_افزودن_output_sprintf_str(vt, 0, TRUE, "");
  }
}
