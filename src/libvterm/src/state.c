#ضمن "vterm_internal.h"

#ضمن <stdio.h>
#ضمن <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#إذا defined(DEBUG) && DEBUG > 1
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

  إذا(state->callbacks && state->callbacks->putglyph)
    إذا((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      أرجع;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

ساكن فراغ updatecursor(VTermState *state, VTermPos *oldpos, صحيح cancel_phantom)
{
  إذا(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    أرجع;

  إذا(cancel_phantom)
    state->at_phantom = 0;

  إذا(state->callbacks && state->callbacks->movecursor)
    إذا((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      أرجع;
}

ساكن فراغ erase(VTermState *state, VTermRect rect, صحيح selective)
{
  إذا(rect.end_col == state->cols) {
    صحيح row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    لكل(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  إذا(state->callbacks && state->callbacks->erase)
    إذا((*state->callbacks->erase)(rect, selective, state->cbdata))
      أرجع;
}

ساكن VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, حجم(VTermState));

  إذا (state == عدم)
    أرجع عدم;
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

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * حجم(VTerm_باريس_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * حجم(VTerm_باريس_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  إذا(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  أرجع state;
}

INTERNAL فراغ vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  إذا(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

ساكن فراغ scroll(VTermState *state, VTermRect rect, صحيح downward, صحيح rightward)
{
  صحيح rows;
  صحيح cols;
  إذا(!downward && !rightward)
    أرجع;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  إذا(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    أرجع;

  rows = rect.end_row - rect.start_row;
  إذا(downward > rows)
    downward = rows;
  وإلا إذا(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  إذا(rightward > cols)
    rightward = cols;
  وإلا إذا(rightward < -cols)
    rightward = -cols;

  إذا(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    إذا(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    وإلا إذا(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    إذا(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    وإلا إذا(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  إذا(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    صحيح height = rect.end_row - rect.start_row - abs(downward);
    صحيح row;
    VTerm_باريس_Info zero_باريس_Info = {0x0};

    إذا(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * حجم(state->lineinfo[0]));
      لكل(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_باريس_Info;
    }
    وإلا {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * حجم(state->lineinfo[0]));
      لكل(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_باريس_Info;
    }
  }

  إذا(state->callbacks && state->callbacks->scrollrect)
    إذا((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      أرجع;

  إذا(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

ساكن فراغ linefeed(VTermState *state)
{
  إذا(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_اتس;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_کيري(state);
    rect.end_col   = SCROLLREGION_کانن(state);

    scroll(state, rect, 1, 0);
  }
  وإلا إذا(state->pos.row < state->rows-1)
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
  أرجع state->tabstops[col >> 3] & mask;
}

ساكن صحيح is_cursor_in_scrollregion(ثابت VTermState *state)
{
  إذا(state->pos.row < state->scrollregion_اتس ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    أرجع 0;
  إذا(state->pos.col < SCROLLREGION_کيري(state) ||
     state->pos.col >= SCROLLREGION_کانن(state))
    أرجع 0;

  أرجع 1;
}

ساكن فراغ tab(VTermState *state, صحيح کيراءن, صحيح direction)
{
  طالما(کيراءن > 0) {
    إذا(direction > 0) {
      إذا(state->pos.col >= THISROWWIDTH(state)-1)
        أرجع;

      state->pos.col++;
    }
    وإلا إذا(direction < 0) {
      إذا(state->pos.col < 1)
        أرجع;

      state->pos.col--;
    }

    إذا(is_col_tabstop(state, state->pos.col))
      کيراءن--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_اتس    1
#define DHL_BOTTOM 2

ساكن فراغ set_lineinfo(VTermState *state, صحيح row, صحيح force, صحيح dwl, صحيح dhl)
{
  VTerm_باريس_Info info = state->lineinfo[row];

  إذا(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  وإلا إذا(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  إذا(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  وإلا إذا(dhl == DHL_اتس)
    info.doubleheight = DHL_اتس;
  وإلا إذا(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  إذا((state->callbacks &&
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
  إذا (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (صحيح)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  إذا(!npoints)
  {
    أرجع (صحيح)eaten;
  }

  إذا(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  إذا(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    إذا(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      صحيح printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      لكل(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      صحيح saved_i = 0;
      طالما(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      طالما(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        إذا(saved_i >= (صحيح)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      إذا(saved_i >= (صحيح)state->combine_chars_size)
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
    وإلا {
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
      إذا(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        اكسر;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * حجم(uint32_t));
    إذا (chars == عدم)
      اكسر;

    لكل( ; i < glyph_ends; i++) {
      صحيح this_width;
      إذا(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        إذا(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      إذا(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      إذا (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    إذا (width < 0)
      width = 0;

    طالما(i < npoints && vterm_unicode_is_combining(codepoints[i]))
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

    إذا(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    إذا(state->mode.سيسيڤ) {
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

    إذا(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      صحيح save_i;
      لكل(save_i = 0; chars[save_i]; save_i++) {
        إذا(save_i >= (صحيح)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      إذا(save_i >= (صحيح)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    إذا(state->pos.col + width >= THISROWWIDTH(state)) {
      إذا(state->mode.autowrap)
        state->at_phantom = 1;
    }
    وإلا {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  إذا(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  أرجع (صحيح)eaten;
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
    إذا(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    اكسر;

  حالة 0x08: // BS - ECMA-48 8.3.5
    إذا(state->pos.col > 0)
      state->pos.col--;
    إذا(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      إذا(vterm_unicode_width(cell.chars[0]) == 2)
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
    إذا(state->mode.newline)
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
    إذا(state->pos.row == state->scrollregion_اتس) {
      VTermRect rect;
      rect.start_row = state->scrollregion_اتس;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_کيري(state);
      rect.end_col   = SCROLLREGION_کانن(state);

      scroll(state, rect, -1, 0);
    }
    وإلا إذا(state->pos.row > 0)
        state->pos.row--;
    اكسر;

  حالة 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    اكسر;

  حالة 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    اكسر;

  افتراضي:
    إذا(state->fallbacks && state->fallbacks->control)
      إذا((*state->fallbacks->control)(control, state->fbdata))
        أرجع 1;

    أرجع 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  إذا(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  أرجع 1;
}

ساكن صحيح settermprop_bool(VTermState *state, VTermProp prop, صحيح v)
{
  VTerm_نيلاي val;
  val.boolean = v;
  أرجع vterm_state_set_termprop(state, prop, &val);
}

ساكن صحيح settermprop_int(VTermState *state, VTermProp prop, صحيح v)
{
  VTerm_نيلاي val;
  val.نومبور = v;
  أرجع vterm_state_set_termprop(state, prop, &val);
}

ساكن صحيح settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_نيلاي val;

  val.string = frag;
  أرجع vterm_state_set_termprop(state, prop, &val);
}

ساكن فراغ savecursor(VTermState *state, صحيح save)
{
  إذا(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  وإلا {
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
    إذا(len != 2)
      أرجع 0;

    بدل(bytes[1]) {
      حالة 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        اكسر;

      حالة 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        اكسر;

      افتراضي:
        أرجع 0;
    }
    أرجع 2;

  حالة '#':
    إذا(len != 2)
      أرجع 0;

    بدل(bytes[1]) {
      حالة '3': // DECDHL top
        إذا(state->mode.leftrightmargin)
          اكسر;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_اتس);
        اكسر;

      حالة '4': // DECDHL bottom
        إذا(state->mode.leftrightmargin)
          اكسر;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        اكسر;

      حالة '5': // DECSWL
        إذا(state->mode.leftrightmargin)
          اكسر;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        اكسر;

      حالة '6': // DECDWL
        إذا(state->mode.leftrightmargin)
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
        أرجع 0;
    }
    أرجع 2;

  حالة '(': حالة ')': حالة '*': حالة '+': // SCS
    إذا(len != 2)
      أرجع 0;

    {
      صحيح setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      إذا(newenc) {
        state->encoding[setnum].enc = newenc;

        إذا(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    أرجع 2;

  حالة '7': // DECSC
    savecursor(state, 1);
    أرجع 1;

  حالة '8': // DECRC
    savecursor(state, 0);
    أرجع 1;

  حالة '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    أرجع 1;

  حالة '=': // DECKPAM
    state->mode.keypad = 1;
    أرجع 1;

  حالة '>': // DECKPNM
    state->mode.keypad = 0;
    أرجع 1;

  حالة 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    إذا(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    أرجع 1;
  }

  حالة 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    أرجع 1;

  حالة 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    أرجع 1;

  حالة '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    أرجع 1;

  حالة '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    أرجع 1;

  حالة '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    أرجع 1;

  افتراضي:
    أرجع 0;
  }
}

ساكن فراغ set_mode(VTermState *state, صحيح اڠک, صحيح val)
{
  بدل(اڠک) {
  حالة 4: // IRM - ECMA-48 7.2.10
    state->mode.سيسيڤ = val;
    اكسر;

  حالة 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    اكسر;

  افتراضي:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", اڠک);
    أرجع;
  }
}

ساكن فراغ set_dec_mode(VTermState *state, صحيح اڠک, صحيح val)
{
  بدل(اڠک) {
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
      state->pos.row = state->mode.origin ? state->scrollregion_اتس : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_کيري(state) : 0;
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
    إذا(val) {
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
        (اڠک == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (اڠک == 1002) ? VTERM_PROP_MOUSE_DRAG  :
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
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", اڠک);
    أرجع;
  }
}

ساكن فراغ request_dec_mode(VTermState *state, صحيح اڠک)
{
  صحيح reply;

  بدل(اڠک) {
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
      vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", اڠک, 0);
      أرجع;
  }

  vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", اڠک, reply ? 1 : 2);
}

ساكن فراغ request_version_string(VTermState *state)
{
  vterm_تولق_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
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
  صحيح کيراءن, val;
  صحيح row, col;
  VTermRect rect;
  صحيح selective;

  إذا(leader && leader[0]) {
    إذا(leader[1]) // longer than 1 char
      أرجع 0;

    بدل(leader[0]) {
    حالة '?':
    حالة '>':
    حالة '<':
      leader_byte = leader[0];
      اكسر;
    افتراضي:
      أرجع 0;
    }
  }

  إذا(intermed && intermed[0]) {
    إذا(intermed[1]) // longer than 1 char
      أرجع 0;

    بدل(intermed[0]) {
    حالة ' ':
    حالة '!':
    حالة '"':
    حالة '$':
    حالة '\'':
      intermed_byte = intermed[0];
      اكسر;
    افتراضي:
      أرجع 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) إذا((v) < (min)) (v) = (min)
#define UBOUND(v,max) إذا((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  بدل(intermed_byte << 16 | leader_byte << 8 | command) {
  حالة 0x40: // ICH - ECMA-48 8.3.64
    کيراءن = CSI_ARG_کيراءن(args[0]);

    إذا(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    إذا(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_کانن(state);
    وإلا
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -کيراءن);

    اكسر;

  حالة 0x41: // CUU - ECMA-48 8.3.22
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.row -= کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x42: // CUD - ECMA-48 8.3.19
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.row += کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x43: // CUF - ECMA-48 8.3.20
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.col += کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x44: // CUB - ECMA-48 8.3.18
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.col -= کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x45: // CNL - ECMA-48 8.3.12
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.col = 0;
    state->pos.row += کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x46: // CPL - ECMA-48 8.3.13
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.col = 0;
    state->pos.row -= کيراءن;
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
    إذا(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      إذا(state->pos.row == row - 1) {
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
    إذا(state->mode.origin) {
      state->pos.row += state->scrollregion_اتس;
      state->pos.col += SCROLLREGION_کيري(state);
    }
    state->at_phantom = 0;
    اكسر;

  حالة 0x49: // CHT - ECMA-48 8.3.10
    کيراءن = CSI_ARG_کيراءن(args[0]);
    tab(state, کيراءن, +1);
    اكسر;

  حالة 0x4a: // ED - ECMA-48 8.3.39
  حالة LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    بدل(CSI_ARG(args[0])) {
    حالة CSI_ARG_MISSING:
    حالة 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      إذا(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      لكل(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      إذا(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      اكسر;

    حالة 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      لكل(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      إذا(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      إذا(rect.end_row > rect.start_row)
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
      إذا(state->callbacks && state->callbacks->sb_clear)
        إذا((*state->callbacks->sb_clear)(state->cbdata))
          أرجع 1;
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
      أرجع 0;
    }

    إذا(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    اكسر;

  حالة 0x4c: // IL - ECMA-48 8.3.67
    کيراءن = CSI_ARG_کيراءن(args[0]);

    إذا(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_کيري(state);
    rect.end_col   = SCROLLREGION_کانن(state);

    scroll(state, rect, -کيراءن, 0);

    اكسر;

  حالة 0x4d: // DL - ECMA-48 8.3.32
    کيراءن = CSI_ARG_کيراءن(args[0]);

    إذا(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_کيري(state);
    rect.end_col   = SCROLLREGION_کانن(state);

    scroll(state, rect, کيراءن, 0);

    اكسر;

  حالة 0x50: // DCH - ECMA-48 8.3.26
    کيراءن = CSI_ARG_کيراءن(args[0]);

    إذا(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    إذا(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_کانن(state);
    وإلا
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, کيراءن);

    اكسر;

  حالة 0x53: // SU - ECMA-48 8.3.147
    کيراءن = CSI_ARG_کيراءن(args[0]);

    rect.start_row = state->scrollregion_اتس;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_کيري(state);
    rect.end_col   = SCROLLREGION_کانن(state);

    scroll(state, rect, کيراءن, 0);

    اكسر;

  حالة 0x54: // SD - ECMA-48 8.3.113
    کيراءن = CSI_ARG_کيراءن(args[0]);

    rect.start_row = state->scrollregion_اتس;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_کيري(state);
    rect.end_col   = SCROLLREGION_کانن(state);

    scroll(state, rect, -کيراءن, 0);

    اكسر;

  حالة 0x58: // ECH - ECMA-48 8.3.38
    کيراءن = CSI_ARG_کيراءن(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + کيراءن;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    اكسر;

  حالة 0x5a: // CBT - ECMA-48 8.3.7
    کيراءن = CSI_ARG_کيراءن(args[0]);
    tab(state, کيراءن, -1);
    اكسر;

  حالة 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    اكسر;

  حالة 0x61: // HPR - ECMA-48 8.3.59
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.col += کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x62: { // REP - ECMA-48 8.3.103
    ثابت صحيح row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    إذا(state->combine_width < 1)
      اكسر;

    کيراءن = CSI_ARG_کيراءن(args[0]);
    col = state->pos.col + کيراءن;
    UBOUND(col, row_width);
    طالما (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    إذا (state->pos.col + state->combine_width >= row_width) {
      إذا (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    اكسر;
  }

  حالة 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    إذا(val == 0)
      // DEC VT100 response
      vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    اكسر;

  حالة LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    اكسر;

  حالة 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    إذا(state->mode.origin)
      state->pos.row += state->scrollregion_اتس;
    state->at_phantom = 0;
    اكسر;

  حالة 0x65: // VPR - ECMA-48 8.3.160
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.row += کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    إذا(state->mode.origin) {
      state->pos.row += state->scrollregion_اتس;
      state->pos.col += SCROLLREGION_کيري(state);
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
      أرجع 0;
    }
    اكسر;

  حالة 0x68: // SM - ECMA-48 8.3.125
    إذا(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    اكسر;

  حالة LEADER('?', 0x68): // DEC private mode set
    لكل(صحيح i = 0; i < argcount; i++) {
      إذا(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    اكسر;

  حالة 0x6a: // HPB - ECMA-48 8.3.58
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.col -= کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6b: // VPB - ECMA-48 8.3.159
    کيراءن = CSI_ARG_کيراءن(args[0]);
    state->pos.row -= کيراءن;
    state->at_phantom = 0;
    اكسر;

  حالة 0x6c: // RM - ECMA-48 8.3.106
    إذا(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    اكسر;

  حالة LEADER('?', 0x6c): // DEC private mode reset
    لكل(صحيح i = 0; i < argcount; i++) {
      إذا(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    اكسر;

  حالة 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    اكسر;

  حالة LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    إذا (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_کونچي_سموا ? 2 : 0);
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
    إذا (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_کونچي_سموا = CSI_ARG(args[1]) == 2;
    }
    اكسر;

  حالة LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    إذا (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_کونچي_سموا = 0;

      state->mode.kitty_keyboard = 1;
    }
    اكسر;

  حالة LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    إذا (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    اكسر;

  حالة LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    إذا (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
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
        vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        اكسر;
      حالة 6: // CPR - cursor position report
        vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
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
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_کيري);
      اكسر;
    حالة 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_کيري);
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
    state->scrollregion_اتس = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_اتس, 0);
    UBOUND(state->scrollregion_اتس, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    إذا(state->scrollregion_اتس == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    وإلا
      UBOUND(state->scrollregion_bottom, state->rows);

    إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_اتس) {
      // Invalid
      state->scrollregion_اتس    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    إذا(state->mode.origin) {
      state->pos.row += state->scrollregion_اتس;
      state->pos.col += SCROLLREGION_کيري(state);
    }

    اكسر;

  حالة 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_کيري = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_کانن = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_کيري, 0);
    UBOUND(state->scrollregion_کيري, state->cols);
    LBOUND(state->scrollregion_کانن, -1);
    إذا(state->scrollregion_کيري == 0 && state->scrollregion_کانن == state->cols)
      state->scrollregion_کانن = -1;
    وإلا
      UBOUND(state->scrollregion_کانن, state->cols);

    إذا(state->scrollregion_کانن > -1 &&
       state->scrollregion_کانن <= state->scrollregion_کيري) {
      // Invalid
      state->scrollregion_کيري  = 0;
      state->scrollregion_کانن = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    إذا(state->mode.origin) {
      state->pos.row += state->scrollregion_اتس;
      state->pos.col += SCROLLREGION_کيري(state);
    }

    اكسر;

  حالة 0x74:
    بدل(CSI_ARG(args[0])) {
      حالة 8: // CSI 8 ; rows ; cols t  set size
        إذا (argcount == 3 &&
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
    کيراءن = CSI_ARG_کيراءن(args[0]);

    إذا(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->scrollregion_اتس;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_کانن(state);

    scroll(state, rect, 0, -کيراءن);

    اكسر;

  حالة INTERMED('\'', 0x7E): // DECDC
    کيراءن = CSI_ARG_کيراءن(args[0]);

    إذا(!is_cursor_in_scrollregion(state))
      اكسر;

    rect.start_row = state->scrollregion_اتس;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_کانن(state);

    scroll(state, rect, 0, کيراءن);

    اكسر;

  افتراضي:
    handled = 0;
    اكسر;
  }

  إذا (!handled) {
    إذا(state->fallbacks && state->fallbacks->csi)
      إذا((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        أرجع 1;

    أرجع 0;
  }

  إذا(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_اتس);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_کيري(state));
    UBOUND(state->pos.col, SCROLLREGION_کانن(state)-1);
  }
  وإلا {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  إذا(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_اتس) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_اتس);
    abort();
  }

  إذا(SCROLLREGION_کانن(state) <= SCROLLREGION_کيري(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_کانن(state), SCROLLREGION_کيري(state));
    abort();
  }
#endif

  أرجع 1;
}

ساكن حرف base64_one(uint8_t b)
{
  إذا(b < 26)
    أرجع 'A' + b;
  وإلا إذا(b < 52)
    أرجع 'a' + b - 26;
  وإلا إذا(b < 62)
    أرجع '0' + b - 52;
  وإلا إذا(b == 62)
    أرجع '+';
  وإلا إذا(b == 63)
    أرجع '/';
  أرجع 0;
}

ساكن uint8_t unbase64one(حرف c)
{
  إذا(c >= 'A' && c <= 'Z')
    أرجع c - 'A';
  وإلا إذا(c >= 'a' && c <= 'z')
    أرجع c - 'a' + 26;
  وإلا إذا(c >= '0' && c <= '9')
    أرجع c - '0' + 52;
  وإلا إذا(c == '+')
    أرجع 62;
  وإلا إذا(c == '/')
    أرجع 63;

  أرجع 0xFF;
}

ساكن فراغ osc_selection(VTermState *state, VTermStringFragment frag)
{
  إذا(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  طالما(!state->tmp.selection.state && frag.len) {
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
        إذا(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        اكسر;
    }

    frag.str++;
    frag.len--;
  }

  إذا(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    إذا(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = عدم,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    أرجع;
  }

  إذا(state->tmp.selection.state == SELECTION_SELECTED) {
    إذا(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    وإلا {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  إذا(state->tmp.selection.state == SELECTION_QUERY) {
    إذا(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    أرجع;
  }

  إذا(state->tmp.selection.state == SELECTION_INVALID)
    أرجع;

  إذا(state->selection.callbacks->set) {
    size_t bufcur = 0;
    حرف *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    صحيح n = 0;      /* Number of sextets consumed */

    إذا(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    طالما((state->selection.buflen - bufcur) >= 3 && frag.len) {
      إذا(frag.str[0] == '=') {
        إذا(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        إذا(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        طالما(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      وإلا {
        uint8_t b = unbase64one(frag.str[0]);
        إذا(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          إذا(state->selection.callbacks->set) {
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

        إذا(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      إذا(!frag.len || (state->selection.buflen - bufcur) < 3) {
        إذا(bufcur) {
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

    إذا(n)
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
      أرجع 1;

    حالة 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      أرجع 1;

    حالة 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      أرجع 1;

    حالة 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        صحيح red = state->default_fg.red;
        صحيح blue = state->default_fg.blue;
        صحيح green = state->default_fg.green;
        vterm_تولق_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        أرجع 1;
      }

    حالة 11:
      {
	// request background color: <Esc>]11;?<0x07>
	صحيح red = state->default_bg.red;
	صحيح blue = state->default_bg.blue;
	صحيح green = state->default_bg.green;
	vterm_تولق_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	أرجع 1;
      }
    حالة 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      أرجع 1;

    حالة 52:
      إذا(state->selection.callbacks)
        osc_selection(state, frag);

      أرجع 1;

    افتراضي:
      إذا(state->fallbacks && state->fallbacks->osc)
        إذا((*state->fallbacks->osc)(command, frag, state->fbdata))
          أرجع 1;
  }

  أرجع 0;
}

ساكن فراغ request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  حرف *tmp = state->tmp.decrqss;

  إذا(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  طالما(i < حجم(state->tmp.decrqss)-1 && tmp[i])
    i++;
  طالما(i < حجم(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  إذا(!frag.final)
    أرجع;

  بدل(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    حالة 'm': {
      // Query SGR
      long args[20];
      صحيح argc = vterm_state_getpen(state, args, حجم(args)/حجم(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      إذا(cur >= vt->tmpbuffer_len)
        أرجع;

      لكل(صحيح argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        إذا(cur >= vt->tmpbuffer_len)
          أرجع;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      إذا(cur >= vt->tmpbuffer_len)
        أرجع;

      vterm_تولق_output_bytes(vt, vt->tmpbuffer, cur);
      أرجع;
    }

    حالة 'r':
      // Query DECSTBM
      vterm_تولق_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_اتس+1, SCROLLREGION_BOTTOM(state));
      أرجع;

    حالة 's':
      // Query DECSLRM
      vterm_تولق_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_کيري(state)+1, SCROLLREGION_کانن(state));
      أرجع;

    حالة ' '|('q'<<8): {
      // Query DECSCUSR
      صحيح reply;
      بدل(state->mode.cursor_shape) {
        حالة VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; اكسر;
        حالة VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; اكسر;
	افتراضي: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; اكسر;
      }
      إذا(state->mode.cursor_blink)
        reply--;
      vterm_تولق_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      أرجع;
    }

    حالة '\"'|('q'<<8):
      // Query DECSCA
      vterm_تولق_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      أرجع;
  }

  vterm_تولق_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

ساكن صحيح on_dcs(ثابت حرف *command, size_t commandlen, VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    أرجع 1;
  }
  وإلا إذا(state->fallbacks && state->fallbacks->dcs)
    إذا((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      أرجع 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (صحيح)commandlen, command);
  أرجع 0;
}

ساكن صحيح on_apc(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(state->fallbacks && state->fallbacks->apc)
    إذا((*state->fallbacks->apc)(frag, state->fbdata))
      أرجع 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  أرجع 0;
}

ساكن صحيح on_pm(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(state->fallbacks && state->fallbacks->pm)
    إذا((*state->fallbacks->pm)(frag, state->fbdata))
      أرجع 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  أرجع 0;
}

ساكن صحيح on_sos(VTermStringFragment frag, فراغ *user)
{
  VTermState *state = user;

  إذا(state->fallbacks && state->fallbacks->sos)
    إذا((*state->fallbacks->sos)(frag, state->fbdata))
      أرجع 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  أرجع 0;
}

ساكن صحيح on_resize(صحيح rows, صحيح cols, فراغ *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  إذا (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  إذا (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  إذا(cols != state->cols) {
    unsigned حرف *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    إذا (newtabstops == عدم)
      أرجع 0;

    /* TODO: This can all be done much more efficiently bytewise */
    صحيح col;
    لكل(col = 0; col < state->cols && col < cols; col++) {
      unsigned حرف mask = 1 << (col & 7);
      إذا(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      وإلا
        newtabstops[col >> 3] &= ~mask;
      }

    لكل( ; col < cols; col++) {
      unsigned حرف mask = 1 << (col & 7);
      إذا(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      وإلا
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  إذا(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  إذا(state->scrollregion_کانن > -1)
    UBOUND(state->scrollregion_کانن, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_اتس, state->rows);
  UBOUND(state->scrollregion_کيري, state->cols);
  إذا(SCROLLREGION_BOTTOM(state) <= state->scrollregion_اتس) {
    state->scrollregion_اتس = 0;
    state->scrollregion_bottom = -1;
  }
  إذا(SCROLLREGION_کانن(state) <= state->scrollregion_کيري) {
    state->scrollregion_کيري = 0;
    state->scrollregion_کانن = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  إذا(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  وإلا {
    إذا(rows != state->rows) {
      لكل(صحيح bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_باريس_Info *oldlineinfo = state->lineinfos[bufidx];
        إذا(!oldlineinfo)
          استمر;

        VTerm_باريس_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * حجم(VTerm_باريس_Info));

        صحيح row;
        لكل(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        لكل( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_باريس_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  إذا(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  إذا(state->pos.row < 0)
    state->pos.row = 0;
  إذا(state->pos.row >= rows)
    state->pos.row = rows - 1;
  إذا(state->pos.col < 0)
    state->pos.col = 0;
  إذا(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  أرجع 1;
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
  إذا(vt->state)
    أرجع vt->state;

  VTermState *state = vterm_state_new(vt);
  إذا (state == عدم)
    أرجع عدم;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  أرجع state;
}

فراغ vterm_state_reset(VTermState *state, صحيح hard)
{
  state->scrollregion_اتس = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_کيري = 0;
  state->scrollregion_کانن = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.سيسيڤ          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  لكل(صحيح col = 0; col < state->cols; col++)
    إذا(col % 8 == 0)
      set_col_tabstop(state, col);
    وإلا
      clear_col_tabstop(state, col);

  لكل(صحيح row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  إذا(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  لكل(صحيح i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    إذا(default_enc->init)
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

  إذا(hard) {
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
  إذا(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    إذا(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  وإلا {
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
  أرجع state->cbdata;
}

فراغ vterm_state_set_unrecognised_fallbacks(VTermState *state, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
{
  إذا(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  وإلا {
    state->fallbacks = عدم;
    state->fbdata = عدم;
  }
}

فراغ *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  أرجع state->fbdata;
}

صحيح vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_نيلاي *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  إذا(state->callbacks && state->callbacks->settermprop)
    إذا(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      أرجع 0;

  بدل(prop) {
  حالة VTERM_PROP_TITLE:
  حالة VTERM_PROP_ICONNAME:
  حالة VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    أرجع 1;
  حالة VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->نومبور;
    أرجع 1;
  حالة VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    أرجع 1;
  حالة VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    إذا(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    أرجع 1;
  حالة VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    إذا(val->نومبور)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    إذا(val->نومبور == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    إذا(val->نومبور == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    أرجع 1;
  حالة VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    أرجع 1;

  حالة VTERM_N_PROPS:
    أرجع 0;
  }

  أرجع 0;
}

فراغ vterm_state_focus_in(VTermState *state)
{
  إذا(state->mode.report_focus)
    vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

فراغ vterm_state_focus_out(VTermState *state)
{
  إذا(state->mode.report_focus)
    vterm_تولق_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

ثابت VTerm_باريس_Info *vterm_state_get_lineinfo(ثابت VTermState *state, صحيح row)
{
  أرجع state->lineinfo + row;
}

فراغ vterm_state_set_selection_callbacks(VTermState *state, ثابت VTermSelectionCallbacks *callbacks, فراغ *user,
    حرف *buffer, size_t buflen)
{
  إذا(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

فراغ vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  إذا(frag.initial) {
    /* TODO: support sending more than one mask bit */
    ساكن حرف selection_chars[] = "cpqs";
    صحيح idx;
    لكل(idx = 0; idx < 4; idx++)
      إذا(mask & (1 << idx))
        اكسر;

    vterm_تولق_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  إذا(frag.len) {
    size_t bufcur = 0;
    حرف *buffer = state->selection.buffer;

    uint32_t x = 0;
    صحيح n = 0;

    إذا(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    طالما((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      إذا(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      إذا(!frag.len || (state->selection.buflen - bufcur) < 4) {
        إذا(bufcur)
          vterm_تولق_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    إذا(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  إذا(frag.final) {
    إذا(state->tmp.selection.sendpartial) {
      صحيح n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      حرف *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_تولق_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    وإلا
      vterm_تولق_output_sprintf_str(vt, 0, TRUE, "");
  }
}
