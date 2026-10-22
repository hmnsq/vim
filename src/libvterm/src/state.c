#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#define strneq(a,b,n) (म_भेदन(a,b,n)==0)

#यदि defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

स्थिर पूर्णाङ्क on_resize(पूर्णाङ्क rows, पूर्णाङ्क cols, शून्य *user);

/* Some convenient wrappers to make callback functions easier */

स्थिर शून्य putglyph(VTermState *state, अचर uint32_t chars[], पूर्णाङ्क width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  यदि(state->callbacks && state->callbacks->putglyph)
    यदि((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      फर्काऊ;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

स्थिर शून्य updatecursor(VTermState *state, VTermPos *oldpos, पूर्णाङ्क cancel_phantom)
{
  यदि(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    फर्काऊ;

  यदि(cancel_phantom)
    state->at_phantom = 0;

  यदि(state->callbacks && state->callbacks->movecursor)
    यदि((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      फर्काऊ;
}

स्थिर शून्य erase(VTermState *state, VTermRect rect, पूर्णाङ्क selective)
{
  यदि(rect.end_col == state->cols) {
    पूर्णाङ्क row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    लागि(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  यदि(state->callbacks && state->callbacks->erase)
    यदि((*state->callbacks->erase)(rect, selective, state->cbdata))
      फर्काऊ;
}

स्थिर VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, परिमाण(VTermState));

  यदि (state == रिक्त)
    फर्काऊ रिक्त;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = रिक्त;
  state->cbdata    = रिक्त;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = रिक्त;
  state->selection.user      = रिक्त;
  state->selection.buffer    = रिक्त;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * परिमाण(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * परिमाण(VTerm_पङ्क्ति_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * परिमाण(VTerm_पङ्क्ति_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  यदि(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  फर्काऊ state;
}

INTERNAL शून्य vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  यदि(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

स्थिर शून्य scroll(VTermState *state, VTermRect rect, पूर्णाङ्क downward, पूर्णाङ्क rightward)
{
  पूर्णाङ्क rows;
  पूर्णाङ्क cols;
  यदि(!downward && !rightward)
    फर्काऊ;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  यदि(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    फर्काऊ;

  rows = rect.end_row - rect.start_row;
  यदि(downward > rows)
    downward = rows;
  अन्यथा यदि(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  यदि(rightward > cols)
    rightward = cols;
  अन्यथा यदि(rightward < -cols)
    rightward = -cols;

  यदि(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    यदि(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    अन्यथा यदि(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    यदि(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    अन्यथा यदि(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  यदि(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    पूर्णाङ्क height = rect.end_row - rect.start_row - असल(downward);
    पूर्णाङ्क row;
    VTerm_पङ्क्ति_Info zero_पङ्क्ति_Info = {0x0};

    यदि(downward > 0) {
      स_हटाओ(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * परिमाण(state->lineinfo[0]));
      लागि(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_पङ्क्ति_Info;
    }
    अन्यथा {
      स_हटाओ(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * परिमाण(state->lineinfo[0]));
      लागि(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_पङ्क्ति_Info;
    }
  }

  यदि(state->callbacks && state->callbacks->scrollrect)
    यदि((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      फर्काऊ;

  यदि(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

स्थिर शून्य linefeed(VTermState *state)
{
  यदि(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_बायाँ(state);
    rect.end_col   = SCROLLREGION_दायाँ(state);

    scroll(state, rect, 1, 0);
  }
  अन्यथा यदि(state->pos.row < state->rows-1)
    state->pos.row++;
}

स्थिर शून्य grow_combine_buffer(VTermState *state)
{
  माप_प्रकार    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * परिमाण(new_chars[0]));

  स_नकल(new_chars, state->combine_chars, state->combine_chars_size * परिमाण(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

स्थिर शून्य set_col_tabstop(VTermState *state, पूर्णाङ्क col)
{
  अचिन्हित वर्ण mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

स्थिर शून्य clear_col_tabstop(VTermState *state, पूर्णाङ्क col)
{
  अचिन्हित वर्ण mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

स्थिर पूर्णाङ्क is_col_tabstop(VTermState *state, पूर्णाङ्क col)
{
  अचिन्हित वर्ण mask = 1 << (col & 7);
  फर्काऊ state->tabstops[col >> 3] & mask;
}

स्थिर पूर्णाङ्क is_cursor_in_scrollregion(अचर VTermState *state)
{
  यदि(state->pos.row < state->scrollregion_शीर्ष ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    फर्काऊ 0;
  यदि(state->pos.col < SCROLLREGION_बायाँ(state) ||
     state->pos.col >= SCROLLREGION_दायाँ(state))
    फर्काऊ 0;

  फर्काऊ 1;
}

स्थिर शून्य tab(VTermState *state, पूर्णाङ्क गन्ती, पूर्णाङ्क direction)
{
  जबसम्म(गन्ती > 0) {
    यदि(direction > 0) {
      यदि(state->pos.col >= THISROWWIDTH(state)-1)
        फर्काऊ;

      state->pos.col++;
    }
    अन्यथा यदि(direction < 0) {
      यदि(state->pos.col < 1)
        फर्काऊ;

      state->pos.col--;
    }

    यदि(is_col_tabstop(state, state->pos.col))
      गन्ती--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_शीर्ष    1
#define DHL_BOTTOM 2

स्थिर शून्य set_lineinfo(VTermState *state, पूर्णाङ्क row, पूर्णाङ्क force, पूर्णाङ्क dwl, पूर्णाङ्क dhl)
{
  VTerm_पङ्क्ति_Info info = state->lineinfo[row];

  यदि(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  अन्यथा यदि(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  यदि(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  अन्यथा यदि(dhl == DHL_शीर्ष)
    info.doubleheight = DHL_शीर्ष;
  अन्यथा यदि(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  यदि((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

स्थिर पूर्णाङ्क on_text(अचर वर्ण bytes[], माप_प्रकार len, शून्य *user)
{
  VTermState *state = user;
  पूर्णाङ्क npoints = 0;
  माप_प्रकार eaten = 0;
  VTermEncodingInstance *encoding;
  पूर्णाङ्क i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  माप_प्रकार maxpoints = (state->vt->tmpbuffer_len) / परिमाण(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  यदि (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (पूर्णाङ्क)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  यदि(!npoints)
  {
    फर्काऊ (पूर्णाङ्क)eaten;
  }

  यदि(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  यदि(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    यदि(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      पूर्णाङ्क printpos;
      म_लिखो("DEBUG: COMBINING SPLIT GLYPH of chars {");
      लागि(printpos = 0; state->combine_chars[printpos]; printpos++)
        म_लिखो("U+%04x ", state->combine_chars[printpos]);
      म_लिखो("} + {");
#endif

      /* Find where we need to append these combining chars */
      पूर्णाङ्क saved_i = 0;
      जबसम्म(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      जबसम्म(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        यदि(saved_i >= (पूर्णाङ्क)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      यदि(saved_i >= (पूर्णाङ्क)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      लागि(; state->combine_chars[printpos]; printpos++)
        म_लिखो("U+%04x ", state->combine_chars[printpos]);
      म_लिखो("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    अन्यथा {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  लागि(; i < npoints; i++) {
    // Try to find combining characters following this
    पूर्णाङ्क glyph_starts = i;
    पूर्णाङ्क glyph_ends;
    पूर्णाङ्क width = 0;

    लागि(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      यदि(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        रोक;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * परिमाण(uint32_t));
    यदि (chars == रिक्त)
      रोक;

    लागि( ; i < glyph_ends; i++) {
      पूर्णाङ्क this_width;
      यदि(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        यदि(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      यदि(this_width < 0) {
        ख_लिखो(मानक_त्रुटि, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        पात();
      }
#endif
      यदि (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    यदि (width < 0)
      width = 0;

    जबसम्म(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    पूर्णाङ्क printpos;
    म_लिखो("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    लागि(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      म_लिखो("U+%04x ", chars[printpos]);
    म_लिखो("}, onscreen width %d\n", width);
#endif

    यदि(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    यदि(state->mode.घुसाउ) {
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

    यदि(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      पूर्णाङ्क save_i;
      लागि(save_i = 0; chars[save_i]; save_i++) {
        यदि(save_i >= (पूर्णाङ्क)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      यदि(save_i >= (पूर्णाङ्क)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    यदि(state->pos.col + width >= THISROWWIDTH(state)) {
      यदि(state->mode.autowrap)
        state->at_phantom = 1;
    }
    अन्यथा {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  यदि(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    ख_लिखो(मानक_त्रुटि, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    पात();
  }
#endif

  फर्काऊ (पूर्णाङ्क)eaten;
}

स्थिर पूर्णाङ्क on_control(अचिन्हित वर्ण control, शून्य *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  विकल्प(control) {
  पक्ष 0x07: // BEL - ECMA-48 8.3.3
    यदि(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    रोक;

  पक्ष 0x08: // BS - ECMA-48 8.3.5
    यदि(state->pos.col > 0)
      state->pos.col--;
    यदि(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      यदि(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    रोक;

  पक्ष 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    रोक;

  पक्ष 0x0a: // LF - ECMA-48 8.3.74
  पक्ष 0x0b: // VT
  पक्ष 0x0c: // FF
    linefeed(state);
    यदि(state->mode.newline)
      state->pos.col = 0;
    रोक;

  पक्ष 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    रोक;

  पक्ष 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    रोक;

  पक्ष 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    रोक;

  पक्ष 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    रोक;

  पक्ष 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    रोक;

  पक्ष 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    रोक;

  पक्ष 0x8d: // RI - ECMA-48 8.3.104
    यदि(state->pos.row == state->scrollregion_शीर्ष) {
      VTermRect rect;
      rect.start_row = state->scrollregion_शीर्ष;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_बायाँ(state);
      rect.end_col   = SCROLLREGION_दायाँ(state);

      scroll(state, rect, -1, 0);
    }
    अन्यथा यदि(state->pos.row > 0)
        state->pos.row--;
    रोक;

  पक्ष 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    रोक;

  पक्ष 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    रोक;

  स्वभाव:
    यदि(state->fallbacks && state->fallbacks->control)
      यदि((*state->fallbacks->control)(control, state->fbdata))
        फर्काऊ 1;

    फर्काऊ 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  यदि(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    ख_लिखो(मानक_त्रुटि, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    पात();
  }
#endif

  फर्काऊ 1;
}

स्थिर पूर्णाङ्क settermprop_bool(VTermState *state, VTermProp prop, पूर्णाङ्क v)
{
  VTerm_मान val;
  val.boolean = v;
  फर्काऊ vterm_state_set_termprop(state, prop, &val);
}

स्थिर पूर्णाङ्क settermprop_int(VTermState *state, VTermProp prop, पूर्णाङ्क v)
{
  VTerm_मान val;
  val.सङ्ख्या = v;
  फर्काऊ vterm_state_set_termprop(state, prop, &val);
}

स्थिर पूर्णाङ्क settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_मान val;

  val.string = frag;
  फर्काऊ vterm_state_set_termprop(state, prop, &val);
}

स्थिर शून्य savecursor(VTermState *state, पूर्णाङ्क save)
{
  यदि(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  अन्यथा {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

स्थिर पूर्णाङ्क on_escape(अचर वर्ण *bytes, माप_प्रकार len, शून्य *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  विकल्प(bytes[0]) {
  पक्ष ' ':
    यदि(len != 2)
      फर्काऊ 0;

    विकल्प(bytes[1]) {
      पक्ष 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        रोक;

      पक्ष 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        रोक;

      स्वभाव:
        फर्काऊ 0;
    }
    फर्काऊ 2;

  पक्ष '#':
    यदि(len != 2)
      फर्काऊ 0;

    विकल्प(bytes[1]) {
      पक्ष '3': // DECDHL top
        यदि(state->mode.leftrightmargin)
          रोक;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_शीर्ष);
        रोक;

      पक्ष '4': // DECDHL bottom
        यदि(state->mode.leftrightmargin)
          रोक;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        रोक;

      पक्ष '5': // DECSWL
        यदि(state->mode.leftrightmargin)
          रोक;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        रोक;

      पक्ष '6': // DECDWL
        यदि(state->mode.leftrightmargin)
          रोक;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        रोक;

      पक्ष '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        लागि(pos.row = 0; pos.row < state->rows; pos.row++)
          लागि(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        रोक;
      }

      स्वभाव:
        फर्काऊ 0;
    }
    फर्काऊ 2;

  पक्ष '(': पक्ष ')': पक्ष '*': पक्ष '+': // SCS
    यदि(len != 2)
      फर्काऊ 0;

    {
      पूर्णाङ्क setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      यदि(newenc) {
        state->encoding[setnum].enc = newenc;

        यदि(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    फर्काऊ 2;

  पक्ष '7': // DECSC
    savecursor(state, 1);
    फर्काऊ 1;

  पक्ष '8': // DECRC
    savecursor(state, 0);
    फर्काऊ 1;

  पक्ष '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    फर्काऊ 1;

  पक्ष '=': // DECKPAM
    state->mode.keypad = 1;
    फर्काऊ 1;

  पक्ष '>': // DECKPNM
    state->mode.keypad = 0;
    फर्काऊ 1;

  पक्ष 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    यदि(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    फर्काऊ 1;
  }

  पक्ष 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    फर्काऊ 1;

  पक्ष 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    फर्काऊ 1;

  पक्ष '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    फर्काऊ 1;

  पक्ष '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    फर्काऊ 1;

  पक्ष '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    फर्काऊ 1;

  स्वभाव:
    फर्काऊ 0;
  }
}

स्थिर शून्य set_mode(VTermState *state, पूर्णाङ्क अङ्क, पूर्णाङ्क val)
{
  विकल्प(अङ्क) {
  पक्ष 4: // IRM - ECMA-48 7.2.10
    state->mode.घुसाउ = val;
    रोक;

  पक्ष 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    रोक;

  स्वभाव:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", अङ्क);
    फर्काऊ;
  }
}

स्थिर शून्य set_dec_mode(VTermState *state, पूर्णाङ्क अङ्क, पूर्णाङ्क val)
{
  विकल्प(अङ्क) {
  पक्ष 1:
    state->mode.cursor = val;
    रोक;

  पक्ष 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    रोक;

  पक्ष 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_शीर्ष : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_बायाँ(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    रोक;

  पक्ष 7:
    state->mode.autowrap = val;
    रोक;

  पक्ष 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    रोक;

  पक्ष 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    रोक;

  पक्ष 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    यदि(val) {
      पूर्णाङ्क row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      लागि(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    रोक;

  पक्ष 1000:
  पक्ष 1002:
  पक्ष 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (अङ्क == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (अङ्क == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    रोक;

  पक्ष 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    रोक;

  पक्ष 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    रोक;

  पक्ष 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    रोक;

  पक्ष 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    रोक;

  पक्ष 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    रोक;

  पक्ष 1048:
    savecursor(state, val);
    रोक;

  पक्ष 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    रोक;

  पक्ष 2004:
    state->mode.bracketpaste = val;
    रोक;

  स्वभाव:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", अङ्क);
    फर्काऊ;
  }
}

स्थिर शून्य request_dec_mode(VTermState *state, पूर्णाङ्क अङ्क)
{
  पूर्णाङ्क reply;

  विकल्प(अङ्क) {
    पक्ष 1:
      reply = state->mode.cursor;
      रोक;

    पक्ष 5:
      reply = state->mode.screen;
      रोक;

    पक्ष 6:
      reply = state->mode.origin;
      रोक;

    पक्ष 7:
      reply = state->mode.autowrap;
      रोक;

    पक्ष 12:
      reply = state->mode.cursor_blink;
      रोक;

    पक्ष 25:
      reply = state->mode.cursor_visible;
      रोक;

    पक्ष 69:
      reply = state->mode.leftrightmargin;
      रोक;

    पक्ष 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      रोक;

    पक्ष 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      रोक;

    पक्ष 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      रोक;

    पक्ष 1004:
      reply = state->mode.report_focus;
      रोक;

    पक्ष 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      रोक;

    पक्ष 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      रोक;

    पक्ष 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      रोक;

    पक्ष 1047:
      reply = state->mode.alt_screen;
      रोक;

    पक्ष 2004:
      reply = state->mode.bracketpaste;
      रोक;

    स्वभाव:
      vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", अङ्क, 0);
      फर्काऊ;
  }

  vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", अङ्क, reply ? 1 : 2);
}

स्थिर शून्य request_version_string(VTermState *state)
{
  vterm_थप_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

स्थिर पूर्णाङ्क on_csi(अचर वर्ण *leader, अचर दीर्घ args[], पूर्णाङ्क argcount, अचर वर्ण *intermed, वर्ण command, शून्य *user)
{
  VTermState *state = user;
  पूर्णाङ्क leader_byte = 0;
  पूर्णाङ्क intermed_byte = 0;
  पूर्णाङ्क cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  पूर्णाङ्क handled = 1;

  // Some temporaries for later code
  पूर्णाङ्क गन्ती, val;
  पूर्णाङ्क row, col;
  VTermRect rect;
  पूर्णाङ्क selective;

  यदि(leader && leader[0]) {
    यदि(leader[1]) // longer than 1 char
      फर्काऊ 0;

    विकल्प(leader[0]) {
    पक्ष '?':
    पक्ष '>':
    पक्ष '<':
      leader_byte = leader[0];
      रोक;
    स्वभाव:
      फर्काऊ 0;
    }
  }

  यदि(intermed && intermed[0]) {
    यदि(intermed[1]) // longer than 1 char
      फर्काऊ 0;

    विकल्प(intermed[0]) {
    पक्ष ' ':
    पक्ष '!':
    पक्ष '"':
    पक्ष '$':
    पक्ष '\'':
      intermed_byte = intermed[0];
      रोक;
    स्वभाव:
      फर्काऊ 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) यदि((v) < (min)) (v) = (min)
#define UBOUND(v,max) यदि((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  विकल्प(intermed_byte << 16 | leader_byte << 8 | command) {
  पक्ष 0x40: // ICH - ECMA-48 8.3.64
    गन्ती = CSI_ARG_गन्ती(args[0]);

    यदि(!is_cursor_in_scrollregion(state))
      रोक;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    यदि(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_दायाँ(state);
    अन्यथा
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -गन्ती);

    रोक;

  पक्ष 0x41: // CUU - ECMA-48 8.3.22
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.row -= गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x42: // CUD - ECMA-48 8.3.19
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.row += गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x43: // CUF - ECMA-48 8.3.20
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.col += गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x44: // CUB - ECMA-48 8.3.18
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.col -= गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x45: // CNL - ECMA-48 8.3.12
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.col = 0;
    state->pos.row += गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x46: // CPL - ECMA-48 8.3.13
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.col = 0;
    state->pos.row -= गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    यदि(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      यदि(state->pos.row == row - 1) {
        पूर्णाङ्क cnt, ptr = 0;
        लागि(cnt = 0; cnt < col - 1; ++cnt) {
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
    यदि(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_बायाँ(state);
    }
    state->at_phantom = 0;
    रोक;

  पक्ष 0x49: // CHT - ECMA-48 8.3.10
    गन्ती = CSI_ARG_गन्ती(args[0]);
    tab(state, गन्ती, +1);
    रोक;

  पक्ष 0x4a: // ED - ECMA-48 8.3.39
  पक्ष LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    विकल्प(CSI_ARG(args[0])) {
    पक्ष CSI_ARG_MISSING:
    पक्ष 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      यदि(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      लागि(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      यदि(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      रोक;

    पक्ष 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      लागि(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      यदि(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      यदि(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      रोक;

    पक्ष 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      लागि(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      रोक;

    पक्ष 3:
      यदि(state->callbacks && state->callbacks->sb_clear)
        यदि((*state->callbacks->sb_clear)(state->cbdata))
          फर्काऊ 1;
      रोक;
    }
    रोक;

  पक्ष 0x4b: // EL - ECMA-48 8.3.41
  पक्ष LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    विकल्प(CSI_ARG(args[0])) {
    पक्ष CSI_ARG_MISSING:
    पक्ष 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); रोक;
    पक्ष 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; रोक;
    पक्ष 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); रोक;
    स्वभाव:
      फर्काऊ 0;
    }

    यदि(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    रोक;

  पक्ष 0x4c: // IL - ECMA-48 8.3.67
    गन्ती = CSI_ARG_गन्ती(args[0]);

    यदि(!is_cursor_in_scrollregion(state))
      रोक;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_बायाँ(state);
    rect.end_col   = SCROLLREGION_दायाँ(state);

    scroll(state, rect, -गन्ती, 0);

    रोक;

  पक्ष 0x4d: // DL - ECMA-48 8.3.32
    गन्ती = CSI_ARG_गन्ती(args[0]);

    यदि(!is_cursor_in_scrollregion(state))
      रोक;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_बायाँ(state);
    rect.end_col   = SCROLLREGION_दायाँ(state);

    scroll(state, rect, गन्ती, 0);

    रोक;

  पक्ष 0x50: // DCH - ECMA-48 8.3.26
    गन्ती = CSI_ARG_गन्ती(args[0]);

    यदि(!is_cursor_in_scrollregion(state))
      रोक;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    यदि(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_दायाँ(state);
    अन्यथा
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, गन्ती);

    रोक;

  पक्ष 0x53: // SU - ECMA-48 8.3.147
    गन्ती = CSI_ARG_गन्ती(args[0]);

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_बायाँ(state);
    rect.end_col   = SCROLLREGION_दायाँ(state);

    scroll(state, rect, गन्ती, 0);

    रोक;

  पक्ष 0x54: // SD - ECMA-48 8.3.113
    गन्ती = CSI_ARG_गन्ती(args[0]);

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_बायाँ(state);
    rect.end_col   = SCROLLREGION_दायाँ(state);

    scroll(state, rect, -गन्ती, 0);

    रोक;

  पक्ष 0x58: // ECH - ECMA-48 8.3.38
    गन्ती = CSI_ARG_गन्ती(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + गन्ती;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    रोक;

  पक्ष 0x5a: // CBT - ECMA-48 8.3.7
    गन्ती = CSI_ARG_गन्ती(args[0]);
    tab(state, गन्ती, -1);
    रोक;

  पक्ष 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x61: // HPR - ECMA-48 8.3.59
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.col += गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x62: { // REP - ECMA-48 8.3.103
    अचर पूर्णाङ्क row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    यदि(state->combine_width < 1)
      रोक;

    गन्ती = CSI_ARG_गन्ती(args[0]);
    col = state->pos.col + गन्ती;
    UBOUND(col, row_width);
    जबसम्म (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    यदि (state->pos.col + state->combine_width >= row_width) {
      यदि (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    रोक;
  }

  पक्ष 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    यदि(val == 0)
      // DEC VT100 response
      vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    रोक;

  पक्ष LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    रोक;

  पक्ष 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    यदि(state->mode.origin)
      state->pos.row += state->scrollregion_शीर्ष;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x65: // VPR - ECMA-48 8.3.160
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.row += गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    यदि(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_बायाँ(state);
    }
    state->at_phantom = 0;
    रोक;

  पक्ष 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    विकल्प(val) {
    पक्ष 0:
      clear_col_tabstop(state, state->pos.col);
      रोक;
    पक्ष 3:
    पक्ष 5:
      लागि(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      रोक;
    पक्ष 1:
    पक्ष 2:
    पक्ष 4:
      रोक;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    स्वभाव:
      फर्काऊ 0;
    }
    रोक;

  पक्ष 0x68: // SM - ECMA-48 8.3.125
    यदि(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    रोक;

  पक्ष LEADER('?', 0x68): // DEC private mode set
    लागि(पूर्णाङ्क i = 0; i < argcount; i++) {
      यदि(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    रोक;

  पक्ष 0x6a: // HPB - ECMA-48 8.3.58
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.col -= गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x6b: // VPB - ECMA-48 8.3.159
    गन्ती = CSI_ARG_गन्ती(args[0]);
    state->pos.row -= गन्ती;
    state->at_phantom = 0;
    रोक;

  पक्ष 0x6c: // RM - ECMA-48 8.3.106
    यदि(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    रोक;

  पक्ष LEADER('?', 0x6c): // DEC private mode reset
    लागि(पूर्णाङ्क i = 0; i < argcount; i++) {
      यदि(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    रोक;

  पक्ष 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    रोक;

  पक्ष LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    यदि (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_साँचोहरू ? 2 : 0);
      रोक;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    लागि(पूर्णाङ्क argi = 0; argi < argcount; argi++) {
      दीर्घ arg;
      विकल्प(arg = CSI_ARG(args[argi])) {
        पक्ष 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          रोक;
        पक्ष 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          रोक;
        पक्ष 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          रोक;
      }
    }
    रोक;

  पक्ष LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    यदि (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_साँचोहरू = CSI_ARG(args[1]) == 2;
    }
    रोक;

  पक्ष LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    यदि (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_साँचोहरू = 0;

      state->mode.kitty_keyboard = 1;
    }
    रोक;

  पक्ष LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    यदि (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    रोक;

  पक्ष LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    यदि (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    रोक;

  पक्ष 0x6e: // DSR - ECMA-48 8.3.35
  पक्ष LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      वर्ण *qmark = (leader_byte == '?') ? "?" : "";

      विकल्प(val) {
      पक्ष 0: पक्ष 1: पक्ष 2: पक्ष 3: पक्ष 4:
        // ignore - these are replies
        रोक;
      पक्ष 5:
        vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        रोक;
      पक्ष 6: // CPR - cursor position report
        vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        रोक;
      }
    }
    रोक;


  पक्ष INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    रोक;

  पक्ष LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    रोक;

  पक्ष LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    रोक;

  पक्ष INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    विकल्प(val) {
    पक्ष 0: पक्ष 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      रोक;
    पक्ष 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      रोक;
    पक्ष 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      रोक;
    पक्ष 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      रोक;
    पक्ष 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_बायाँ);
      रोक;
    पक्ष 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_बायाँ);
      रोक;
    }

    रोक;

  पक्ष INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    विकल्प(val) {
    पक्ष 0: पक्ष 2:
      state->protected_cell = 0;
      रोक;
    पक्ष 1:
      state->protected_cell = 1;
      रोक;
    }

    रोक;

  पक्ष 0x72: // DECSTBM - DEC custom
    state->scrollregion_शीर्ष = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_शीर्ष, 0);
    UBOUND(state->scrollregion_शीर्ष, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    यदि(state->scrollregion_शीर्ष == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    अन्यथा
      UBOUND(state->scrollregion_bottom, state->rows);

    यदि(SCROLLREGION_BOTTOM(state) <= state->scrollregion_शीर्ष) {
      // Invalid
      state->scrollregion_शीर्ष    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    यदि(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_बायाँ(state);
    }

    रोक;

  पक्ष 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_बायाँ = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_दायाँ = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_बायाँ, 0);
    UBOUND(state->scrollregion_बायाँ, state->cols);
    LBOUND(state->scrollregion_दायाँ, -1);
    यदि(state->scrollregion_बायाँ == 0 && state->scrollregion_दायाँ == state->cols)
      state->scrollregion_दायाँ = -1;
    अन्यथा
      UBOUND(state->scrollregion_दायाँ, state->cols);

    यदि(state->scrollregion_दायाँ > -1 &&
       state->scrollregion_दायाँ <= state->scrollregion_बायाँ) {
      // Invalid
      state->scrollregion_बायाँ  = 0;
      state->scrollregion_दायाँ = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    यदि(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_बायाँ(state);
    }

    रोक;

  पक्ष 0x74:
    विकल्प(CSI_ARG(args[0])) {
      पक्ष 8: // CSI 8 ; rows ; cols t  set size
        यदि (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	रोक;
      स्वभाव:
	handled = 0;
	रोक;
    }
    रोक;

  पक्ष INTERMED('\'', 0x7D): // DECIC
    गन्ती = CSI_ARG_गन्ती(args[0]);

    यदि(!is_cursor_in_scrollregion(state))
      रोक;

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_दायाँ(state);

    scroll(state, rect, 0, -गन्ती);

    रोक;

  पक्ष INTERMED('\'', 0x7E): // DECDC
    गन्ती = CSI_ARG_गन्ती(args[0]);

    यदि(!is_cursor_in_scrollregion(state))
      रोक;

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_दायाँ(state);

    scroll(state, rect, 0, गन्ती);

    रोक;

  स्वभाव:
    handled = 0;
    रोक;
  }

  यदि (!handled) {
    यदि(state->fallbacks && state->fallbacks->csi)
      यदि((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        फर्काऊ 1;

    फर्काऊ 0;
  }

  यदि(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_शीर्ष);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_बायाँ(state));
    UBOUND(state->pos.col, SCROLLREGION_दायाँ(state)-1);
  }
  अन्यथा {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  यदि(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    ख_लिखो(मानक_त्रुटि, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    पात();
  }

  यदि(SCROLLREGION_BOTTOM(state) <= state->scrollregion_शीर्ष) {
    ख_लिखो(मानक_त्रुटि, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_शीर्ष);
    पात();
  }

  यदि(SCROLLREGION_दायाँ(state) <= SCROLLREGION_बायाँ(state)) {
    ख_लिखो(मानक_त्रुटि, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_दायाँ(state), SCROLLREGION_बायाँ(state));
    पात();
  }
#endif

  फर्काऊ 1;
}

स्थिर वर्ण base64_one(uint8_t b)
{
  यदि(b < 26)
    फर्काऊ 'A' + b;
  अन्यथा यदि(b < 52)
    फर्काऊ 'a' + b - 26;
  अन्यथा यदि(b < 62)
    फर्काऊ '0' + b - 52;
  अन्यथा यदि(b == 62)
    फर्काऊ '+';
  अन्यथा यदि(b == 63)
    फर्काऊ '/';
  फर्काऊ 0;
}

स्थिर uint8_t unbase64one(वर्ण c)
{
  यदि(c >= 'A' && c <= 'Z')
    फर्काऊ c - 'A';
  अन्यथा यदि(c >= 'a' && c <= 'z')
    फर्काऊ c - 'a' + 26;
  अन्यथा यदि(c >= '0' && c <= '9')
    फर्काऊ c - '0' + 52;
  अन्यथा यदि(c == '+')
    फर्काऊ 62;
  अन्यथा यदि(c == '/')
    फर्काऊ 63;

  फर्काऊ 0xFF;
}

स्थिर शून्य osc_selection(VTermState *state, VTermStringFragment frag)
{
  यदि(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  जबसम्म(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    विकल्प(frag.str[0]) {
      पक्ष 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        रोक;
      पक्ष 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        रोक;
      पक्ष 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        रोक;
      पक्ष 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        रोक;
      पक्ष '0':
      पक्ष '1':
      पक्ष '2':
      पक्ष '3':
      पक्ष '4':
      पक्ष '5':
      पक्ष '6':
      पक्ष '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        रोक;

      पक्ष ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        यदि(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        रोक;
    }

    frag.str++;
    frag.len--;
  }

  यदि(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    यदि(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = रिक्त,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    फर्काऊ;
  }

  यदि(state->tmp.selection.state == SELECTION_SELECTED) {
    यदि(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    अन्यथा {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  यदि(state->tmp.selection.state == SELECTION_QUERY) {
    यदि(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    फर्काऊ;
  }

  यदि(state->tmp.selection.state == SELECTION_INVALID)
    फर्काऊ;

  यदि(state->selection.callbacks->set) {
    माप_प्रकार bufcur = 0;
    वर्ण *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    पूर्णाङ्क n = 0;      /* Number of sextets consumed */

    यदि(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    जबसम्म((state->selection.buflen - bufcur) >= 3 && frag.len) {
      यदि(frag.str[0] == '=') {
        यदि(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        यदि(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        जबसम्म(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      अन्यथा {
        uint8_t b = unbase64one(frag.str[0]);
        यदि(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          यदि(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = रिक्त,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          रोक;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        यदि(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      यदि(!frag.len || (state->selection.buflen - bufcur) < 3) {
        यदि(bufcur) {
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

    यदि(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

स्थिर पूर्णाङ्क on_osc(पूर्णाङ्क command, VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  विकल्प(command) {
    पक्ष 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      फर्काऊ 1;

    पक्ष 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      फर्काऊ 1;

    पक्ष 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      फर्काऊ 1;

    पक्ष 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        पूर्णाङ्क red = state->default_fg.red;
        पूर्णाङ्क blue = state->default_fg.blue;
        पूर्णाङ्क green = state->default_fg.green;
        vterm_थप_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        फर्काऊ 1;
      }

    पक्ष 11:
      {
	// request background color: <Esc>]11;?<0x07>
	पूर्णाङ्क red = state->default_bg.red;
	पूर्णाङ्क blue = state->default_bg.blue;
	पूर्णाङ्क green = state->default_bg.green;
	vterm_थप_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	फर्काऊ 1;
      }
    पक्ष 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      फर्काऊ 1;

    पक्ष 52:
      यदि(state->selection.callbacks)
        osc_selection(state, frag);

      फर्काऊ 1;

    स्वभाव:
      यदि(state->fallbacks && state->fallbacks->osc)
        यदि((*state->fallbacks->osc)(command, frag, state->fbdata))
          फर्काऊ 1;
  }

  फर्काऊ 0;
}

स्थिर शून्य request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  वर्ण *tmp = state->tmp.decrqss;

  यदि(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  माप_प्रकार i = 0;
  जबसम्म(i < परिमाण(state->tmp.decrqss)-1 && tmp[i])
    i++;
  जबसम्म(i < परिमाण(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  यदि(!frag.final)
    फर्काऊ;

  विकल्प(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    पक्ष 'm': {
      // Query SGR
      दीर्घ args[20];
      पूर्णाङ्क argc = vterm_state_getpen(state, args, परिमाण(args)/परिमाण(args[0]));
      माप_प्रकार cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      यदि(cur >= vt->tmpbuffer_len)
        फर्काऊ;

      लागि(पूर्णाङ्क argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        यदि(cur >= vt->tmpbuffer_len)
          फर्काऊ;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      यदि(cur >= vt->tmpbuffer_len)
        फर्काऊ;

      vterm_थप_output_bytes(vt, vt->tmpbuffer, cur);
      फर्काऊ;
    }

    पक्ष 'r':
      // Query DECSTBM
      vterm_थप_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_शीर्ष+1, SCROLLREGION_BOTTOM(state));
      फर्काऊ;

    पक्ष 's':
      // Query DECSLRM
      vterm_थप_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_बायाँ(state)+1, SCROLLREGION_दायाँ(state));
      फर्काऊ;

    पक्ष ' '|('q'<<8): {
      // Query DECSCUSR
      पूर्णाङ्क reply;
      विकल्प(state->mode.cursor_shape) {
        पक्ष VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; रोक;
        पक्ष VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; रोक;
	स्वभाव: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; रोक;
      }
      यदि(state->mode.cursor_blink)
        reply--;
      vterm_थप_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      फर्काऊ;
    }

    पक्ष '\"'|('q'<<8):
      // Query DECSCA
      vterm_थप_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      फर्काऊ;
  }

  vterm_थप_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

स्थिर पूर्णाङ्क on_dcs(अचर वर्ण *command, माप_प्रकार commandlen, VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  यदि(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    फर्काऊ 1;
  }
  अन्यथा यदि(state->fallbacks && state->fallbacks->dcs)
    यदि((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      फर्काऊ 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (पूर्णाङ्क)commandlen, command);
  फर्काऊ 0;
}

स्थिर पूर्णाङ्क on_apc(VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  यदि(state->fallbacks && state->fallbacks->apc)
    यदि((*state->fallbacks->apc)(frag, state->fbdata))
      फर्काऊ 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  फर्काऊ 0;
}

स्थिर पूर्णाङ्क on_pm(VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  यदि(state->fallbacks && state->fallbacks->pm)
    यदि((*state->fallbacks->pm)(frag, state->fbdata))
      फर्काऊ 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  फर्काऊ 0;
}

स्थिर पूर्णाङ्क on_sos(VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  यदि(state->fallbacks && state->fallbacks->sos)
    यदि((*state->fallbacks->sos)(frag, state->fbdata))
      फर्काऊ 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  फर्काऊ 0;
}

स्थिर पूर्णाङ्क on_resize(पूर्णाङ्क rows, पूर्णाङ्क cols, शून्य *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  यदि (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  यदि (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  यदि(cols != state->cols) {
    अचिन्हित वर्ण *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    यदि (newtabstops == रिक्त)
      फर्काऊ 0;

    /* TODO: This can all be done much more efficiently bytewise */
    पूर्णाङ्क col;
    लागि(col = 0; col < state->cols && col < cols; col++) {
      अचिन्हित वर्ण mask = 1 << (col & 7);
      यदि(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      अन्यथा
        newtabstops[col >> 3] &= ~mask;
      }

    लागि( ; col < cols; col++) {
      अचिन्हित वर्ण mask = 1 << (col & 7);
      यदि(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      अन्यथा
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  यदि(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  यदि(state->scrollregion_दायाँ > -1)
    UBOUND(state->scrollregion_दायाँ, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_शीर्ष, state->rows);
  UBOUND(state->scrollregion_बायाँ, state->cols);
  यदि(SCROLLREGION_BOTTOM(state) <= state->scrollregion_शीर्ष) {
    state->scrollregion_शीर्ष = 0;
    state->scrollregion_bottom = -1;
  }
  यदि(SCROLLREGION_दायाँ(state) <= state->scrollregion_बायाँ) {
    state->scrollregion_बायाँ = 0;
    state->scrollregion_दायाँ = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  यदि(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  अन्यथा {
    यदि(rows != state->rows) {
      लागि(पूर्णाङ्क bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_पङ्क्ति_Info *oldlineinfo = state->lineinfos[bufidx];
        यदि(!oldlineinfo)
          जारी;

        VTerm_पङ्क्ति_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * परिमाण(VTerm_पङ्क्ति_Info));

        पूर्णाङ्क row;
        लागि(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        लागि( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_पङ्क्ति_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  यदि(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  यदि(state->pos.row < 0)
    state->pos.row = 0;
  यदि(state->pos.row >= rows)
    state->pos.row = rows - 1;
  यदि(state->pos.col < 0)
    state->pos.col = 0;
  यदि(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  फर्काऊ 1;
}

स्थिर अचर VTermParserCallbacks parser_callbacks = {
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
  यदि(vt->state)
    फर्काऊ vt->state;

  VTermState *state = vterm_state_new(vt);
  यदि (state == रिक्त)
    फर्काऊ रिक्त;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  फर्काऊ state;
}

शून्य vterm_state_reset(VTermState *state, पूर्णाङ्क hard)
{
  state->scrollregion_शीर्ष = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_बायाँ = 0;
  state->scrollregion_दायाँ = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.घुसाउ          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  लागि(पूर्णाङ्क col = 0; col < state->cols; col++)
    यदि(col % 8 == 0)
      set_col_tabstop(state, col);
    अन्यथा
      clear_col_tabstop(state, col);

  लागि(पूर्णाङ्क row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  यदि(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  लागि(पूर्णाङ्क i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    यदि(default_enc->init)
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

  यदि(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

शून्य vterm_state_get_cursorpos(अचर VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

शून्य vterm_state_get_mousestate(अचर VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

शून्य vterm_state_set_callbacks(VTermState *state, अचर VTermStateCallbacks *callbacks, शून्य *user)
{
  यदि(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    यदि(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  अन्यथा {
    state->callbacks = रिक्त;
    state->cbdata = रिक्त;
  }
}

शून्य vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

शून्य *vterm_state_get_cbdata(VTermState *state)
{
  फर्काऊ state->cbdata;
}

शून्य vterm_state_set_unrecognised_fallbacks(VTermState *state, अचर VTermStateFallbacks *fallbacks, शून्य *user)
{
  यदि(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  अन्यथा {
    state->fallbacks = रिक्त;
    state->fbdata = रिक्त;
  }
}

शून्य *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  फर्काऊ state->fbdata;
}

पूर्णाङ्क vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_मान *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  यदि(state->callbacks && state->callbacks->settermprop)
    यदि(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      फर्काऊ 0;

  विकल्प(prop) {
  पक्ष VTERM_PROP_TITLE:
  पक्ष VTERM_PROP_ICONNAME:
  पक्ष VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    फर्काऊ 1;
  पक्ष VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    फर्काऊ 1;
  पक्ष VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    फर्काऊ 1;
  पक्ष VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->सङ्ख्या;
    फर्काऊ 1;
  पक्ष VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    फर्काऊ 1;
  पक्ष VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    यदि(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    फर्काऊ 1;
  पक्ष VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    यदि(val->सङ्ख्या)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    यदि(val->सङ्ख्या == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    यदि(val->सङ्ख्या == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    फर्काऊ 1;
  पक्ष VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    फर्काऊ 1;

  पक्ष VTERM_N_PROPS:
    फर्काऊ 0;
  }

  फर्काऊ 0;
}

शून्य vterm_state_focus_in(VTermState *state)
{
  यदि(state->mode.report_focus)
    vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

शून्य vterm_state_focus_out(VTermState *state)
{
  यदि(state->mode.report_focus)
    vterm_थप_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

अचर VTerm_पङ्क्ति_Info *vterm_state_get_lineinfo(अचर VTermState *state, पूर्णाङ्क row)
{
  फर्काऊ state->lineinfo + row;
}

शून्य vterm_state_set_selection_callbacks(VTermState *state, अचर VTermSelectionCallbacks *callbacks, शून्य *user,
    वर्ण *buffer, माप_प्रकार buflen)
{
  यदि(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

शून्य vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  यदि(frag.initial) {
    /* TODO: support sending more than one mask bit */
    स्थिर वर्ण selection_chars[] = "cpqs";
    पूर्णाङ्क idx;
    लागि(idx = 0; idx < 4; idx++)
      यदि(mask & (1 << idx))
        रोक;

    vterm_थप_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  यदि(frag.len) {
    माप_प्रकार bufcur = 0;
    वर्ण *buffer = state->selection.buffer;

    uint32_t x = 0;
    पूर्णाङ्क n = 0;

    यदि(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    जबसम्म((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      यदि(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      यदि(!frag.len || (state->selection.buflen - bufcur) < 4) {
        यदि(bufcur)
          vterm_थप_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    यदि(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  यदि(frag.final) {
    यदि(state->tmp.selection.sendpartial) {
      पूर्णाङ्क n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      वर्ण *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_थप_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    अन्यथा
      vterm_थप_output_sprintf_str(vt, 0, TRUE, "");
  }
}
