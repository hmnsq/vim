#अन्तर्भाव "vterm_internal.h"

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#define strneq(a,b,n) (म_भेदन(a,b,n)==0)

#जर defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

स्थिर पूर्णांक on_resize(पूर्णांक rows, पूर्णांक cols, शून्य *user);

/* Some convenient wrappers to make callback functions easier */

स्थिर शून्य putglyph(VTermState *state, स्थिरांक uint32_t chars[], पूर्णांक width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  जर(state->callbacks && state->callbacks->putglyph)
    जर((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      परत;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

स्थिर शून्य updatecursor(VTermState *state, VTermPos *oldpos, पूर्णांक cancel_phantom)
{
  जर(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    परत;

  जर(cancel_phantom)
    state->at_phantom = 0;

  जर(state->callbacks && state->callbacks->movecursor)
    जर((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      परत;
}

स्थिर शून्य erase(VTermState *state, VTermRect rect, पूर्णांक selective)
{
  जर(rect.end_col == state->cols) {
    पूर्णांक row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    साठी(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  जर(state->callbacks && state->callbacks->erase)
    जर((*state->callbacks->erase)(rect, selective, state->cbdata))
      परत;
}

स्थिर VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, आकार(VTermState));

  जर (state == रिक्त)
    परत रिक्त;
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
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * आकार(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * आकार(VTerm_ओळ_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * आकार(VTerm_ओळ_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  जर(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  परत state;
}

INTERNAL शून्य vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  जर(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

स्थिर शून्य scroll(VTermState *state, VTermRect rect, पूर्णांक downward, पूर्णांक rightward)
{
  पूर्णांक rows;
  पूर्णांक cols;
  जर(!downward && !rightward)
    परत;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  जर(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    परत;

  rows = rect.end_row - rect.start_row;
  जर(downward > rows)
    downward = rows;
  अन्यथा जर(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  जर(rightward > cols)
    rightward = cols;
  अन्यथा जर(rightward < -cols)
    rightward = -cols;

  जर(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    जर(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    अन्यथा जर(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    जर(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    अन्यथा जर(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  जर(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    पूर्णांक height = rect.end_row - rect.start_row - असल(downward);
    पूर्णांक row;
    VTerm_ओळ_Info zero_ओळ_Info = {0x0};

    जर(downward > 0) {
      स_हटाओ(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * आकार(state->lineinfo[0]));
      साठी(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_ओळ_Info;
    }
    अन्यथा {
      स_हटाओ(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * आकार(state->lineinfo[0]));
      साठी(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_ओळ_Info;
    }
  }

  जर(state->callbacks && state->callbacks->scrollrect)
    जर((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      परत;

  जर(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

स्थिर शून्य linefeed(VTermState *state)
{
  जर(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_डावा(state);
    rect.end_col   = SCROLLREGION_उजवा(state);

    scroll(state, rect, 1, 0);
  }
  अन्यथा जर(state->pos.row < state->rows-1)
    state->pos.row++;
}

स्थिर शून्य grow_combine_buffer(VTermState *state)
{
  माप_प्रकार    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * आकार(new_chars[0]));

  स_नकल(new_chars, state->combine_chars, state->combine_chars_size * आकार(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

स्थिर शून्य set_col_tabstop(VTermState *state, पूर्णांक col)
{
  अचिन्हित अक्षर mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

स्थिर शून्य clear_col_tabstop(VTermState *state, पूर्णांक col)
{
  अचिन्हित अक्षर mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

स्थिर पूर्णांक is_col_tabstop(VTermState *state, पूर्णांक col)
{
  अचिन्हित अक्षर mask = 1 << (col & 7);
  परत state->tabstops[col >> 3] & mask;
}

स्थिर पूर्णांक is_cursor_in_scrollregion(स्थिरांक VTermState *state)
{
  जर(state->pos.row < state->scrollregion_शीर्ष ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    परत 0;
  जर(state->pos.col < SCROLLREGION_डावा(state) ||
     state->pos.col >= SCROLLREGION_उजवा(state))
    परत 0;

  परत 1;
}

स्थिर शून्य tab(VTermState *state, पूर्णांक मोजणी, पूर्णांक direction)
{
  पर्यंत(मोजणी > 0) {
    जर(direction > 0) {
      जर(state->pos.col >= THISROWWIDTH(state)-1)
        परत;

      state->pos.col++;
    }
    अन्यथा जर(direction < 0) {
      जर(state->pos.col < 1)
        परत;

      state->pos.col--;
    }

    जर(is_col_tabstop(state, state->pos.col))
      मोजणी--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_शीर्ष    1
#define DHL_BOTTOM 2

स्थिर शून्य set_lineinfo(VTermState *state, पूर्णांक row, पूर्णांक force, पूर्णांक dwl, पूर्णांक dhl)
{
  VTerm_ओळ_Info info = state->lineinfo[row];

  जर(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  अन्यथा जर(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  जर(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  अन्यथा जर(dhl == DHL_शीर्ष)
    info.doubleheight = DHL_शीर्ष;
  अन्यथा जर(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  जर((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

स्थिर पूर्णांक on_text(स्थिरांक अक्षर bytes[], माप_प्रकार len, शून्य *user)
{
  VTermState *state = user;
  पूर्णांक npoints = 0;
  माप_प्रकार eaten = 0;
  VTermEncodingInstance *encoding;
  पूर्णांक i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  माप_प्रकार maxpoints = (state->vt->tmpbuffer_len) / आकार(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  जर (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (पूर्णांक)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  जर(!npoints)
  {
    परत (पूर्णांक)eaten;
  }

  जर(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  जर(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    जर(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      पूर्णांक printpos;
      म_लिखो("DEBUG: COMBINING SPLIT GLYPH of chars {");
      साठी(printpos = 0; state->combine_chars[printpos]; printpos++)
        म_लिखो("U+%04x ", state->combine_chars[printpos]);
      म_लिखो("} + {");
#endif

      /* Find where we need to append these combining chars */
      पूर्णांक saved_i = 0;
      पर्यंत(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      पर्यंत(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        जर(saved_i >= (पूर्णांक)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      जर(saved_i >= (पूर्णांक)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      साठी(; state->combine_chars[printpos]; printpos++)
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

  साठी(; i < npoints; i++) {
    // Try to find combining characters following this
    पूर्णांक glyph_starts = i;
    पूर्णांक glyph_ends;
    पूर्णांक width = 0;

    साठी(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      जर(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        थांबा;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * आकार(uint32_t));
    जर (chars == रिक्त)
      थांबा;

    साठी( ; i < glyph_ends; i++) {
      पूर्णांक this_width;
      जर(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        जर(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      जर(this_width < 0) {
        ख_लिखो(मानक_त्रुटि, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        पात();
      }
#endif
      जर (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    जर (width < 0)
      width = 0;

    पर्यंत(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    पूर्णांक printpos;
    म_लिखो("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    साठी(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      म_लिखो("U+%04x ", chars[printpos]);
    म_लिखो("}, onscreen width %d\n", width);
#endif

    जर(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    जर(state->mode.घाला) {
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

    जर(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      पूर्णांक save_i;
      साठी(save_i = 0; chars[save_i]; save_i++) {
        जर(save_i >= (पूर्णांक)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      जर(save_i >= (पूर्णांक)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    जर(state->pos.col + width >= THISROWWIDTH(state)) {
      जर(state->mode.autowrap)
        state->at_phantom = 1;
    }
    अन्यथा {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  जर(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    ख_लिखो(मानक_त्रुटि, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    पात();
  }
#endif

  परत (पूर्णांक)eaten;
}

स्थिर पूर्णांक on_control(अचिन्हित अक्षर control, शून्य *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  निवड(control) {
  स्थिती 0x07: // BEL - ECMA-48 8.3.3
    जर(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    थांबा;

  स्थिती 0x08: // BS - ECMA-48 8.3.5
    जर(state->pos.col > 0)
      state->pos.col--;
    जर(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      जर(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    थांबा;

  स्थिती 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    थांबा;

  स्थिती 0x0a: // LF - ECMA-48 8.3.74
  स्थिती 0x0b: // VT
  स्थिती 0x0c: // FF
    linefeed(state);
    जर(state->mode.newline)
      state->pos.col = 0;
    थांबा;

  स्थिती 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    थांबा;

  स्थिती 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    थांबा;

  स्थिती 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    थांबा;

  स्थिती 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    थांबा;

  स्थिती 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    थांबा;

  स्थिती 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    थांबा;

  स्थिती 0x8d: // RI - ECMA-48 8.3.104
    जर(state->pos.row == state->scrollregion_शीर्ष) {
      VTermRect rect;
      rect.start_row = state->scrollregion_शीर्ष;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_डावा(state);
      rect.end_col   = SCROLLREGION_उजवा(state);

      scroll(state, rect, -1, 0);
    }
    अन्यथा जर(state->pos.row > 0)
        state->pos.row--;
    थांबा;

  स्थिती 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    थांबा;

  स्थिती 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    थांबा;

  पूर्वनिर्धारित:
    जर(state->fallbacks && state->fallbacks->control)
      जर((*state->fallbacks->control)(control, state->fbdata))
        परत 1;

    परत 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  जर(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    ख_लिखो(मानक_त्रुटि, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    पात();
  }
#endif

  परत 1;
}

स्थिर पूर्णांक settermprop_bool(VTermState *state, VTermProp prop, पूर्णांक v)
{
  VTerm_मूल्य val;
  val.boolean = v;
  परत vterm_state_set_termprop(state, prop, &val);
}

स्थिर पूर्णांक settermprop_int(VTermState *state, VTermProp prop, पूर्णांक v)
{
  VTerm_मूल्य val;
  val.संख्या = v;
  परत vterm_state_set_termprop(state, prop, &val);
}

स्थिर पूर्णांक settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_मूल्य val;

  val.string = frag;
  परत vterm_state_set_termprop(state, prop, &val);
}

स्थिर शून्य savecursor(VTermState *state, पूर्णांक save)
{
  जर(save) {
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

स्थिर पूर्णांक on_escape(स्थिरांक अक्षर *bytes, माप_प्रकार len, शून्य *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  निवड(bytes[0]) {
  स्थिती ' ':
    जर(len != 2)
      परत 0;

    निवड(bytes[1]) {
      स्थिती 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        थांबा;

      स्थिती 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        थांबा;

      पूर्वनिर्धारित:
        परत 0;
    }
    परत 2;

  स्थिती '#':
    जर(len != 2)
      परत 0;

    निवड(bytes[1]) {
      स्थिती '3': // DECDHL top
        जर(state->mode.leftrightmargin)
          थांबा;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_शीर्ष);
        थांबा;

      स्थिती '4': // DECDHL bottom
        जर(state->mode.leftrightmargin)
          थांबा;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        थांबा;

      स्थिती '5': // DECSWL
        जर(state->mode.leftrightmargin)
          थांबा;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        थांबा;

      स्थिती '6': // DECDWL
        जर(state->mode.leftrightmargin)
          थांबा;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        थांबा;

      स्थिती '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        साठी(pos.row = 0; pos.row < state->rows; pos.row++)
          साठी(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        थांबा;
      }

      पूर्वनिर्धारित:
        परत 0;
    }
    परत 2;

  स्थिती '(': स्थिती ')': स्थिती '*': स्थिती '+': // SCS
    जर(len != 2)
      परत 0;

    {
      पूर्णांक setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      जर(newenc) {
        state->encoding[setnum].enc = newenc;

        जर(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    परत 2;

  स्थिती '7': // DECSC
    savecursor(state, 1);
    परत 1;

  स्थिती '8': // DECRC
    savecursor(state, 0);
    परत 1;

  स्थिती '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    परत 1;

  स्थिती '=': // DECKPAM
    state->mode.keypad = 1;
    परत 1;

  स्थिती '>': // DECKPNM
    state->mode.keypad = 0;
    परत 1;

  स्थिती 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    जर(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    परत 1;
  }

  स्थिती 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    परत 1;

  स्थिती 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    परत 1;

  स्थिती '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    परत 1;

  स्थिती '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    परत 1;

  स्थिती '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    परत 1;

  पूर्वनिर्धारित:
    परत 0;
  }
}

स्थिर शून्य set_mode(VTermState *state, पूर्णांक अंक, पूर्णांक val)
{
  निवड(अंक) {
  स्थिती 4: // IRM - ECMA-48 7.2.10
    state->mode.घाला = val;
    थांबा;

  स्थिती 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    थांबा;

  पूर्वनिर्धारित:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", अंक);
    परत;
  }
}

स्थिर शून्य set_dec_mode(VTermState *state, पूर्णांक अंक, पूर्णांक val)
{
  निवड(अंक) {
  स्थिती 1:
    state->mode.cursor = val;
    थांबा;

  स्थिती 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    थांबा;

  स्थिती 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_शीर्ष : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_डावा(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    थांबा;

  स्थिती 7:
    state->mode.autowrap = val;
    थांबा;

  स्थिती 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    थांबा;

  स्थिती 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    थांबा;

  स्थिती 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    जर(val) {
      पूर्णांक row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      साठी(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    थांबा;

  स्थिती 1000:
  स्थिती 1002:
  स्थिती 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (अंक == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (अंक == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    थांबा;

  स्थिती 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    थांबा;

  स्थिती 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    थांबा;

  स्थिती 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    थांबा;

  स्थिती 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    थांबा;

  स्थिती 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    थांबा;

  स्थिती 1048:
    savecursor(state, val);
    थांबा;

  स्थिती 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    थांबा;

  स्थिती 2004:
    state->mode.bracketpaste = val;
    थांबा;

  पूर्वनिर्धारित:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", अंक);
    परत;
  }
}

स्थिर शून्य request_dec_mode(VTermState *state, पूर्णांक अंक)
{
  पूर्णांक reply;

  निवड(अंक) {
    स्थिती 1:
      reply = state->mode.cursor;
      थांबा;

    स्थिती 5:
      reply = state->mode.screen;
      थांबा;

    स्थिती 6:
      reply = state->mode.origin;
      थांबा;

    स्थिती 7:
      reply = state->mode.autowrap;
      थांबा;

    स्थिती 12:
      reply = state->mode.cursor_blink;
      थांबा;

    स्थिती 25:
      reply = state->mode.cursor_visible;
      थांबा;

    स्थिती 69:
      reply = state->mode.leftrightmargin;
      थांबा;

    स्थिती 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      थांबा;

    स्थिती 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      थांबा;

    स्थिती 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      थांबा;

    स्थिती 1004:
      reply = state->mode.report_focus;
      थांबा;

    स्थिती 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      थांबा;

    स्थिती 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      थांबा;

    स्थिती 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      थांबा;

    स्थिती 1047:
      reply = state->mode.alt_screen;
      थांबा;

    स्थिती 2004:
      reply = state->mode.bracketpaste;
      थांबा;

    पूर्वनिर्धारित:
      vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", अंक, 0);
      परत;
  }

  vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", अंक, reply ? 1 : 2);
}

स्थिर शून्य request_version_string(VTermState *state)
{
  vterm_ढकला_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

स्थिर पूर्णांक on_csi(स्थिरांक अक्षर *leader, स्थिरांक दीर्घ args[], पूर्णांक argcount, स्थिरांक अक्षर *intermed, अक्षर command, शून्य *user)
{
  VTermState *state = user;
  पूर्णांक leader_byte = 0;
  पूर्णांक intermed_byte = 0;
  पूर्णांक cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  पूर्णांक handled = 1;

  // Some temporaries for later code
  पूर्णांक मोजणी, val;
  पूर्णांक row, col;
  VTermRect rect;
  पूर्णांक selective;

  जर(leader && leader[0]) {
    जर(leader[1]) // longer than 1 char
      परत 0;

    निवड(leader[0]) {
    स्थिती '?':
    स्थिती '>':
    स्थिती '<':
      leader_byte = leader[0];
      थांबा;
    पूर्वनिर्धारित:
      परत 0;
    }
  }

  जर(intermed && intermed[0]) {
    जर(intermed[1]) // longer than 1 char
      परत 0;

    निवड(intermed[0]) {
    स्थिती ' ':
    स्थिती '!':
    स्थिती '"':
    स्थिती '$':
    स्थिती '\'':
      intermed_byte = intermed[0];
      थांबा;
    पूर्वनिर्धारित:
      परत 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) जर((v) < (min)) (v) = (min)
#define UBOUND(v,max) जर((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  निवड(intermed_byte << 16 | leader_byte << 8 | command) {
  स्थिती 0x40: // ICH - ECMA-48 8.3.64
    मोजणी = CSI_ARG_मोजणी(args[0]);

    जर(!is_cursor_in_scrollregion(state))
      थांबा;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    जर(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_उजवा(state);
    अन्यथा
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -मोजणी);

    थांबा;

  स्थिती 0x41: // CUU - ECMA-48 8.3.22
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.row -= मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x42: // CUD - ECMA-48 8.3.19
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.row += मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x43: // CUF - ECMA-48 8.3.20
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.col += मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x44: // CUB - ECMA-48 8.3.18
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.col -= मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x45: // CNL - ECMA-48 8.3.12
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.col = 0;
    state->pos.row += मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x46: // CPL - ECMA-48 8.3.13
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.col = 0;
    state->pos.row -= मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    जर(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      जर(state->pos.row == row - 1) {
        पूर्णांक cnt, ptr = 0;
        साठी(cnt = 0; cnt < col - 1; ++cnt) {
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
    जर(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_डावा(state);
    }
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x49: // CHT - ECMA-48 8.3.10
    मोजणी = CSI_ARG_मोजणी(args[0]);
    tab(state, मोजणी, +1);
    थांबा;

  स्थिती 0x4a: // ED - ECMA-48 8.3.39
  स्थिती LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    निवड(CSI_ARG(args[0])) {
    स्थिती CSI_ARG_MISSING:
    स्थिती 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      जर(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      साठी(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      जर(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      थांबा;

    स्थिती 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      साठी(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      जर(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      जर(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      थांबा;

    स्थिती 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      साठी(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      थांबा;

    स्थिती 3:
      जर(state->callbacks && state->callbacks->sb_clear)
        जर((*state->callbacks->sb_clear)(state->cbdata))
          परत 1;
      थांबा;
    }
    थांबा;

  स्थिती 0x4b: // EL - ECMA-48 8.3.41
  स्थिती LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    निवड(CSI_ARG(args[0])) {
    स्थिती CSI_ARG_MISSING:
    स्थिती 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); थांबा;
    स्थिती 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; थांबा;
    स्थिती 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); थांबा;
    पूर्वनिर्धारित:
      परत 0;
    }

    जर(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    थांबा;

  स्थिती 0x4c: // IL - ECMA-48 8.3.67
    मोजणी = CSI_ARG_मोजणी(args[0]);

    जर(!is_cursor_in_scrollregion(state))
      थांबा;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_डावा(state);
    rect.end_col   = SCROLLREGION_उजवा(state);

    scroll(state, rect, -मोजणी, 0);

    थांबा;

  स्थिती 0x4d: // DL - ECMA-48 8.3.32
    मोजणी = CSI_ARG_मोजणी(args[0]);

    जर(!is_cursor_in_scrollregion(state))
      थांबा;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_डावा(state);
    rect.end_col   = SCROLLREGION_उजवा(state);

    scroll(state, rect, मोजणी, 0);

    थांबा;

  स्थिती 0x50: // DCH - ECMA-48 8.3.26
    मोजणी = CSI_ARG_मोजणी(args[0]);

    जर(!is_cursor_in_scrollregion(state))
      थांबा;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    जर(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_उजवा(state);
    अन्यथा
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, मोजणी);

    थांबा;

  स्थिती 0x53: // SU - ECMA-48 8.3.147
    मोजणी = CSI_ARG_मोजणी(args[0]);

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_डावा(state);
    rect.end_col   = SCROLLREGION_उजवा(state);

    scroll(state, rect, मोजणी, 0);

    थांबा;

  स्थिती 0x54: // SD - ECMA-48 8.3.113
    मोजणी = CSI_ARG_मोजणी(args[0]);

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_डावा(state);
    rect.end_col   = SCROLLREGION_उजवा(state);

    scroll(state, rect, -मोजणी, 0);

    थांबा;

  स्थिती 0x58: // ECH - ECMA-48 8.3.38
    मोजणी = CSI_ARG_मोजणी(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + मोजणी;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    थांबा;

  स्थिती 0x5a: // CBT - ECMA-48 8.3.7
    मोजणी = CSI_ARG_मोजणी(args[0]);
    tab(state, मोजणी, -1);
    थांबा;

  स्थिती 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x61: // HPR - ECMA-48 8.3.59
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.col += मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x62: { // REP - ECMA-48 8.3.103
    स्थिरांक पूर्णांक row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    जर(state->combine_width < 1)
      थांबा;

    मोजणी = CSI_ARG_मोजणी(args[0]);
    col = state->pos.col + मोजणी;
    UBOUND(col, row_width);
    पर्यंत (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    जर (state->pos.col + state->combine_width >= row_width) {
      जर (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    थांबा;
  }

  स्थिती 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    जर(val == 0)
      // DEC VT100 response
      vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    थांबा;

  स्थिती LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    थांबा;

  स्थिती 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    जर(state->mode.origin)
      state->pos.row += state->scrollregion_शीर्ष;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x65: // VPR - ECMA-48 8.3.160
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.row += मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    जर(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_डावा(state);
    }
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    निवड(val) {
    स्थिती 0:
      clear_col_tabstop(state, state->pos.col);
      थांबा;
    स्थिती 3:
    स्थिती 5:
      साठी(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      थांबा;
    स्थिती 1:
    स्थिती 2:
    स्थिती 4:
      थांबा;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    पूर्वनिर्धारित:
      परत 0;
    }
    थांबा;

  स्थिती 0x68: // SM - ECMA-48 8.3.125
    जर(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    थांबा;

  स्थिती LEADER('?', 0x68): // DEC private mode set
    साठी(पूर्णांक i = 0; i < argcount; i++) {
      जर(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    थांबा;

  स्थिती 0x6a: // HPB - ECMA-48 8.3.58
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.col -= मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x6b: // VPB - ECMA-48 8.3.159
    मोजणी = CSI_ARG_मोजणी(args[0]);
    state->pos.row -= मोजणी;
    state->at_phantom = 0;
    थांबा;

  स्थिती 0x6c: // RM - ECMA-48 8.3.106
    जर(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    थांबा;

  स्थिती LEADER('?', 0x6c): // DEC private mode reset
    साठी(पूर्णांक i = 0; i < argcount; i++) {
      जर(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    थांबा;

  स्थिती 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    थांबा;

  स्थिती LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    जर (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_किल्ल्या ? 2 : 0);
      थांबा;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    साठी(पूर्णांक argi = 0; argi < argcount; argi++) {
      दीर्घ arg;
      निवड(arg = CSI_ARG(args[argi])) {
        स्थिती 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          थांबा;
        स्थिती 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          थांबा;
        स्थिती 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          थांबा;
      }
    }
    थांबा;

  स्थिती LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    जर (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_किल्ल्या = CSI_ARG(args[1]) == 2;
    }
    थांबा;

  स्थिती LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    जर (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_किल्ल्या = 0;

      state->mode.kitty_keyboard = 1;
    }
    थांबा;

  स्थिती LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    जर (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    थांबा;

  स्थिती LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    जर (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    थांबा;

  स्थिती 0x6e: // DSR - ECMA-48 8.3.35
  स्थिती LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      अक्षर *qmark = (leader_byte == '?') ? "?" : "";

      निवड(val) {
      स्थिती 0: स्थिती 1: स्थिती 2: स्थिती 3: स्थिती 4:
        // ignore - these are replies
        थांबा;
      स्थिती 5:
        vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        थांबा;
      स्थिती 6: // CPR - cursor position report
        vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        थांबा;
      }
    }
    थांबा;


  स्थिती INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    थांबा;

  स्थिती LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    थांबा;

  स्थिती LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    थांबा;

  स्थिती INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    निवड(val) {
    स्थिती 0: स्थिती 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      थांबा;
    स्थिती 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      थांबा;
    स्थिती 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      थांबा;
    स्थिती 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      थांबा;
    स्थिती 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_डावा);
      थांबा;
    स्थिती 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_डावा);
      थांबा;
    }

    थांबा;

  स्थिती INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    निवड(val) {
    स्थिती 0: स्थिती 2:
      state->protected_cell = 0;
      थांबा;
    स्थिती 1:
      state->protected_cell = 1;
      थांबा;
    }

    थांबा;

  स्थिती 0x72: // DECSTBM - DEC custom
    state->scrollregion_शीर्ष = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_शीर्ष, 0);
    UBOUND(state->scrollregion_शीर्ष, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    जर(state->scrollregion_शीर्ष == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    अन्यथा
      UBOUND(state->scrollregion_bottom, state->rows);

    जर(SCROLLREGION_BOTTOM(state) <= state->scrollregion_शीर्ष) {
      // Invalid
      state->scrollregion_शीर्ष    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    जर(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_डावा(state);
    }

    थांबा;

  स्थिती 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_डावा = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_उजवा = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_डावा, 0);
    UBOUND(state->scrollregion_डावा, state->cols);
    LBOUND(state->scrollregion_उजवा, -1);
    जर(state->scrollregion_डावा == 0 && state->scrollregion_उजवा == state->cols)
      state->scrollregion_उजवा = -1;
    अन्यथा
      UBOUND(state->scrollregion_उजवा, state->cols);

    जर(state->scrollregion_उजवा > -1 &&
       state->scrollregion_उजवा <= state->scrollregion_डावा) {
      // Invalid
      state->scrollregion_डावा  = 0;
      state->scrollregion_उजवा = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    जर(state->mode.origin) {
      state->pos.row += state->scrollregion_शीर्ष;
      state->pos.col += SCROLLREGION_डावा(state);
    }

    थांबा;

  स्थिती 0x74:
    निवड(CSI_ARG(args[0])) {
      स्थिती 8: // CSI 8 ; rows ; cols t  set size
        जर (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	थांबा;
      पूर्वनिर्धारित:
	handled = 0;
	थांबा;
    }
    थांबा;

  स्थिती INTERMED('\'', 0x7D): // DECIC
    मोजणी = CSI_ARG_मोजणी(args[0]);

    जर(!is_cursor_in_scrollregion(state))
      थांबा;

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_उजवा(state);

    scroll(state, rect, 0, -मोजणी);

    थांबा;

  स्थिती INTERMED('\'', 0x7E): // DECDC
    मोजणी = CSI_ARG_मोजणी(args[0]);

    जर(!is_cursor_in_scrollregion(state))
      थांबा;

    rect.start_row = state->scrollregion_शीर्ष;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_उजवा(state);

    scroll(state, rect, 0, मोजणी);

    थांबा;

  पूर्वनिर्धारित:
    handled = 0;
    थांबा;
  }

  जर (!handled) {
    जर(state->fallbacks && state->fallbacks->csi)
      जर((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        परत 1;

    परत 0;
  }

  जर(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_शीर्ष);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_डावा(state));
    UBOUND(state->pos.col, SCROLLREGION_उजवा(state)-1);
  }
  अन्यथा {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  जर(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    ख_लिखो(मानक_त्रुटि, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    पात();
  }

  जर(SCROLLREGION_BOTTOM(state) <= state->scrollregion_शीर्ष) {
    ख_लिखो(मानक_त्रुटि, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_शीर्ष);
    पात();
  }

  जर(SCROLLREGION_उजवा(state) <= SCROLLREGION_डावा(state)) {
    ख_लिखो(मानक_त्रुटि, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_उजवा(state), SCROLLREGION_डावा(state));
    पात();
  }
#endif

  परत 1;
}

स्थिर अक्षर base64_one(uint8_t b)
{
  जर(b < 26)
    परत 'A' + b;
  अन्यथा जर(b < 52)
    परत 'a' + b - 26;
  अन्यथा जर(b < 62)
    परत '0' + b - 52;
  अन्यथा जर(b == 62)
    परत '+';
  अन्यथा जर(b == 63)
    परत '/';
  परत 0;
}

स्थिर uint8_t unbase64one(अक्षर c)
{
  जर(c >= 'A' && c <= 'Z')
    परत c - 'A';
  अन्यथा जर(c >= 'a' && c <= 'z')
    परत c - 'a' + 26;
  अन्यथा जर(c >= '0' && c <= '9')
    परत c - '0' + 52;
  अन्यथा जर(c == '+')
    परत 62;
  अन्यथा जर(c == '/')
    परत 63;

  परत 0xFF;
}

स्थिर शून्य osc_selection(VTermState *state, VTermStringFragment frag)
{
  जर(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  पर्यंत(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    निवड(frag.str[0]) {
      स्थिती 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        थांबा;
      स्थिती 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        थांबा;
      स्थिती 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        थांबा;
      स्थिती 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        थांबा;
      स्थिती '0':
      स्थिती '1':
      स्थिती '2':
      स्थिती '3':
      स्थिती '4':
      स्थिती '5':
      स्थिती '6':
      स्थिती '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        थांबा;

      स्थिती ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        जर(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        थांबा;
    }

    frag.str++;
    frag.len--;
  }

  जर(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    जर(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = रिक्त,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    परत;
  }

  जर(state->tmp.selection.state == SELECTION_SELECTED) {
    जर(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    अन्यथा {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  जर(state->tmp.selection.state == SELECTION_QUERY) {
    जर(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    परत;
  }

  जर(state->tmp.selection.state == SELECTION_INVALID)
    परत;

  जर(state->selection.callbacks->set) {
    माप_प्रकार bufcur = 0;
    अक्षर *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    पूर्णांक n = 0;      /* Number of sextets consumed */

    जर(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    पर्यंत((state->selection.buflen - bufcur) >= 3 && frag.len) {
      जर(frag.str[0] == '=') {
        जर(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        जर(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        पर्यंत(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      अन्यथा {
        uint8_t b = unbase64one(frag.str[0]);
        जर(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          जर(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = रिक्त,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          थांबा;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        जर(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      जर(!frag.len || (state->selection.buflen - bufcur) < 3) {
        जर(bufcur) {
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

    जर(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

स्थिर पूर्णांक on_osc(पूर्णांक command, VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  निवड(command) {
    स्थिती 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      परत 1;

    स्थिती 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      परत 1;

    स्थिती 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      परत 1;

    स्थिती 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        पूर्णांक red = state->default_fg.red;
        पूर्णांक blue = state->default_fg.blue;
        पूर्णांक green = state->default_fg.green;
        vterm_ढकला_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        परत 1;
      }

    स्थिती 11:
      {
	// request background color: <Esc>]11;?<0x07>
	पूर्णांक red = state->default_bg.red;
	पूर्णांक blue = state->default_bg.blue;
	पूर्णांक green = state->default_bg.green;
	vterm_ढकला_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	परत 1;
      }
    स्थिती 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      परत 1;

    स्थिती 52:
      जर(state->selection.callbacks)
        osc_selection(state, frag);

      परत 1;

    पूर्वनिर्धारित:
      जर(state->fallbacks && state->fallbacks->osc)
        जर((*state->fallbacks->osc)(command, frag, state->fbdata))
          परत 1;
  }

  परत 0;
}

स्थिर शून्य request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  अक्षर *tmp = state->tmp.decrqss;

  जर(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  माप_प्रकार i = 0;
  पर्यंत(i < आकार(state->tmp.decrqss)-1 && tmp[i])
    i++;
  पर्यंत(i < आकार(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  जर(!frag.final)
    परत;

  निवड(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    स्थिती 'm': {
      // Query SGR
      दीर्घ args[20];
      पूर्णांक argc = vterm_state_getpen(state, args, आकार(args)/आकार(args[0]));
      माप_प्रकार cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      जर(cur >= vt->tmpbuffer_len)
        परत;

      साठी(पूर्णांक argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        जर(cur >= vt->tmpbuffer_len)
          परत;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      जर(cur >= vt->tmpbuffer_len)
        परत;

      vterm_ढकला_output_bytes(vt, vt->tmpbuffer, cur);
      परत;
    }

    स्थिती 'r':
      // Query DECSTBM
      vterm_ढकला_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_शीर्ष+1, SCROLLREGION_BOTTOM(state));
      परत;

    स्थिती 's':
      // Query DECSLRM
      vterm_ढकला_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_डावा(state)+1, SCROLLREGION_उजवा(state));
      परत;

    स्थिती ' '|('q'<<8): {
      // Query DECSCUSR
      पूर्णांक reply;
      निवड(state->mode.cursor_shape) {
        स्थिती VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; थांबा;
        स्थिती VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; थांबा;
	पूर्वनिर्धारित: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; थांबा;
      }
      जर(state->mode.cursor_blink)
        reply--;
      vterm_ढकला_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      परत;
    }

    स्थिती '\"'|('q'<<8):
      // Query DECSCA
      vterm_ढकला_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      परत;
  }

  vterm_ढकला_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

स्थिर पूर्णांक on_dcs(स्थिरांक अक्षर *command, माप_प्रकार commandlen, VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  जर(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    परत 1;
  }
  अन्यथा जर(state->fallbacks && state->fallbacks->dcs)
    जर((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      परत 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (पूर्णांक)commandlen, command);
  परत 0;
}

स्थिर पूर्णांक on_apc(VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  जर(state->fallbacks && state->fallbacks->apc)
    जर((*state->fallbacks->apc)(frag, state->fbdata))
      परत 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  परत 0;
}

स्थिर पूर्णांक on_pm(VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  जर(state->fallbacks && state->fallbacks->pm)
    जर((*state->fallbacks->pm)(frag, state->fbdata))
      परत 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  परत 0;
}

स्थिर पूर्णांक on_sos(VTermStringFragment frag, शून्य *user)
{
  VTermState *state = user;

  जर(state->fallbacks && state->fallbacks->sos)
    जर((*state->fallbacks->sos)(frag, state->fbdata))
      परत 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  परत 0;
}

स्थिर पूर्णांक on_resize(पूर्णांक rows, पूर्णांक cols, शून्य *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  जर (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  जर (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  जर(cols != state->cols) {
    अचिन्हित अक्षर *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    जर (newtabstops == रिक्त)
      परत 0;

    /* TODO: This can all be done much more efficiently bytewise */
    पूर्णांक col;
    साठी(col = 0; col < state->cols && col < cols; col++) {
      अचिन्हित अक्षर mask = 1 << (col & 7);
      जर(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      अन्यथा
        newtabstops[col >> 3] &= ~mask;
      }

    साठी( ; col < cols; col++) {
      अचिन्हित अक्षर mask = 1 << (col & 7);
      जर(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      अन्यथा
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  जर(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  जर(state->scrollregion_उजवा > -1)
    UBOUND(state->scrollregion_उजवा, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_शीर्ष, state->rows);
  UBOUND(state->scrollregion_डावा, state->cols);
  जर(SCROLLREGION_BOTTOM(state) <= state->scrollregion_शीर्ष) {
    state->scrollregion_शीर्ष = 0;
    state->scrollregion_bottom = -1;
  }
  जर(SCROLLREGION_उजवा(state) <= state->scrollregion_डावा) {
    state->scrollregion_डावा = 0;
    state->scrollregion_उजवा = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  जर(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  अन्यथा {
    जर(rows != state->rows) {
      साठी(पूर्णांक bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_ओळ_Info *oldlineinfo = state->lineinfos[bufidx];
        जर(!oldlineinfo)
          सुरूठेवा;

        VTerm_ओळ_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * आकार(VTerm_ओळ_Info));

        पूर्णांक row;
        साठी(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        साठी( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_ओळ_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  जर(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  जर(state->pos.row < 0)
    state->pos.row = 0;
  जर(state->pos.row >= rows)
    state->pos.row = rows - 1;
  जर(state->pos.col < 0)
    state->pos.col = 0;
  जर(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  परत 1;
}

स्थिर स्थिरांक VTermParserCallbacks parser_callbacks = {
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
  जर(vt->state)
    परत vt->state;

  VTermState *state = vterm_state_new(vt);
  जर (state == रिक्त)
    परत रिक्त;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  परत state;
}

शून्य vterm_state_reset(VTermState *state, पूर्णांक hard)
{
  state->scrollregion_शीर्ष = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_डावा = 0;
  state->scrollregion_उजवा = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.घाला          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  साठी(पूर्णांक col = 0; col < state->cols; col++)
    जर(col % 8 == 0)
      set_col_tabstop(state, col);
    अन्यथा
      clear_col_tabstop(state, col);

  साठी(पूर्णांक row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  जर(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  साठी(पूर्णांक i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    जर(default_enc->init)
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

  जर(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

शून्य vterm_state_get_cursorpos(स्थिरांक VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

शून्य vterm_state_get_mousestate(स्थिरांक VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

शून्य vterm_state_set_callbacks(VTermState *state, स्थिरांक VTermStateCallbacks *callbacks, शून्य *user)
{
  जर(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    जर(state->callbacks && state->callbacks->initpen)
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
  परत state->cbdata;
}

शून्य vterm_state_set_unrecognised_fallbacks(VTermState *state, स्थिरांक VTermStateFallbacks *fallbacks, शून्य *user)
{
  जर(fallbacks) {
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
  परत state->fbdata;
}

पूर्णांक vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_मूल्य *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  जर(state->callbacks && state->callbacks->settermprop)
    जर(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      परत 0;

  निवड(prop) {
  स्थिती VTERM_PROP_TITLE:
  स्थिती VTERM_PROP_ICONNAME:
  स्थिती VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    परत 1;
  स्थिती VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    परत 1;
  स्थिती VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    परत 1;
  स्थिती VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->संख्या;
    परत 1;
  स्थिती VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    परत 1;
  स्थिती VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    जर(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    परत 1;
  स्थिती VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    जर(val->संख्या)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    जर(val->संख्या == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    जर(val->संख्या == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    परत 1;
  स्थिती VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    परत 1;

  स्थिती VTERM_N_PROPS:
    परत 0;
  }

  परत 0;
}

शून्य vterm_state_focus_in(VTermState *state)
{
  जर(state->mode.report_focus)
    vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

शून्य vterm_state_focus_out(VTermState *state)
{
  जर(state->mode.report_focus)
    vterm_ढकला_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

स्थिरांक VTerm_ओळ_Info *vterm_state_get_lineinfo(स्थिरांक VTermState *state, पूर्णांक row)
{
  परत state->lineinfo + row;
}

शून्य vterm_state_set_selection_callbacks(VTermState *state, स्थिरांक VTermSelectionCallbacks *callbacks, शून्य *user,
    अक्षर *buffer, माप_प्रकार buflen)
{
  जर(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

शून्य vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  जर(frag.initial) {
    /* TODO: support sending more than one mask bit */
    स्थिर अक्षर selection_chars[] = "cpqs";
    पूर्णांक idx;
    साठी(idx = 0; idx < 4; idx++)
      जर(mask & (1 << idx))
        थांबा;

    vterm_ढकला_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  जर(frag.len) {
    माप_प्रकार bufcur = 0;
    अक्षर *buffer = state->selection.buffer;

    uint32_t x = 0;
    पूर्णांक n = 0;

    जर(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    पर्यंत((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      जर(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      जर(!frag.len || (state->selection.buflen - bufcur) < 4) {
        जर(bufcur)
          vterm_ढकला_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    जर(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  जर(frag.final) {
    जर(state->tmp.selection.sendpartial) {
      पूर्णांक n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      अक्षर *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_ढकला_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    अन्यथा
      vterm_ढकला_output_sprintf_str(vt, 0, TRUE, "");
  }
}
