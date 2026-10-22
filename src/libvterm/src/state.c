#அந்தர்பாவ "vterm_internal.h"

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#எனில் defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

நிலையான முழு on_resize(முழு rows, முழு cols, வெறுமை *user);

/* Some convenient wrappers to make callback functions easier */

நிலையான வெறுமை putglyph(VTermState *state, மாறிலி uint32_t chars[], முழு width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  எனில்(state->callbacks && state->callbacks->putglyph)
    எனில்((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      திரும்பு;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

நிலையான வெறுமை updatecursor(VTermState *state, VTermPos *oldpos, முழு cancel_phantom)
{
  எனில்(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    திரும்பு;

  எனில்(cancel_phantom)
    state->at_phantom = 0;

  எனில்(state->callbacks && state->callbacks->movecursor)
    எனில்((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      திரும்பு;
}

நிலையான வெறுமை erase(VTermState *state, VTermRect rect, முழு selective)
{
  எனில்(rect.end_col == state->cols) {
    முழு row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    க்கு(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  எனில்(state->callbacks && state->callbacks->erase)
    எனில்((*state->callbacks->erase)(rect, selective, state->cbdata))
      திரும்பு;
}

நிலையான VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, அளவு(VTermState));

  எனில் (state == வெற்று)
    திரும்பு வெற்று;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = வெற்று;
  state->cbdata    = வெற்று;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = வெற்று;
  state->selection.user      = வெற்று;
  state->selection.buffer    = வெற்று;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * அளவு(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * அளவு(VTerm_வரி_Info));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * அளவு(VTerm_வரி_Info));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  எனில்(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  திரும்பு state;
}

INTERNAL வெறுமை vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  எனில்(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

நிலையான வெறுமை scroll(VTermState *state, VTermRect rect, முழு downward, முழு rightward)
{
  முழு rows;
  முழு cols;
  எனில்(!downward && !rightward)
    திரும்பு;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  எனில்(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    திரும்பு;

  rows = rect.end_row - rect.start_row;
  எனில்(downward > rows)
    downward = rows;
  இல்லையெனில் எனில்(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  எனில்(rightward > cols)
    rightward = cols;
  இல்லையெனில் எனில்(rightward < -cols)
    rightward = -cols;

  எனில்(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    எனில்(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    இல்லையெனில் எனில்(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    எனில்(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    இல்லையெனில் எனில்(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  எனில்(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    முழு height = rect.end_row - rect.start_row - abs(downward);
    முழு row;
    VTerm_வரி_Info zero_வரி_Info = {0x0};

    எனில்(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * அளவு(state->lineinfo[0]));
      க்கு(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zero_வரி_Info;
    }
    இல்லையெனில் {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * அளவு(state->lineinfo[0]));
      க்கு(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zero_வரி_Info;
    }
  }

  எனில்(state->callbacks && state->callbacks->scrollrect)
    எனில்((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      திரும்பு;

  எனில்(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

நிலையான வெறுமை linefeed(VTermState *state)
{
  எனில்(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_உச்சி;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_இடது(state);
    rect.end_col   = SCROLLREGION_வலது(state);

    scroll(state, rect, 1, 0);
  }
  இல்லையெனில் எனில்(state->pos.row < state->rows-1)
    state->pos.row++;
}

நிலையான வெறுமை grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * அளவு(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * அளவு(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

நிலையான வெறுமை set_col_tabstop(VTermState *state, முழு col)
{
  unsigned எழுத்து mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

நிலையான வெறுமை clear_col_tabstop(VTermState *state, முழு col)
{
  unsigned எழுத்து mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

நிலையான முழு is_col_tabstop(VTermState *state, முழு col)
{
  unsigned எழுத்து mask = 1 << (col & 7);
  திரும்பு state->tabstops[col >> 3] & mask;
}

நிலையான முழு is_cursor_in_scrollregion(மாறிலி VTermState *state)
{
  எனில்(state->pos.row < state->scrollregion_உச்சி ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    திரும்பு 0;
  எனில்(state->pos.col < SCROLLREGION_இடது(state) ||
     state->pos.col >= SCROLLREGION_வலது(state))
    திரும்பு 0;

  திரும்பு 1;
}

நிலையான வெறுமை tab(VTermState *state, முழு மொத்தம், முழு direction)
{
  போது(மொத்தம் > 0) {
    எனில்(direction > 0) {
      எனில்(state->pos.col >= THISROWWIDTH(state)-1)
        திரும்பு;

      state->pos.col++;
    }
    இல்லையெனில் எனில்(direction < 0) {
      எனில்(state->pos.col < 1)
        திரும்பு;

      state->pos.col--;
    }

    எனில்(is_col_tabstop(state, state->pos.col))
      மொத்தம்--;
  }
}

#define NO_FORCE 0
#define FORCE    1

#define DWL_OFF 0
#define DWL_ON  1

#define DHL_OFF    0
#define DHL_உச்சி    1
#define DHL_BOTTOM 2

நிலையான வெறுமை set_lineinfo(VTermState *state, முழு row, முழு force, முழு dwl, முழு dhl)
{
  VTerm_வரி_Info info = state->lineinfo[row];

  எனில்(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  இல்லையெனில் எனில்(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  எனில்(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  இல்லையெனில் எனில்(dhl == DHL_உச்சி)
    info.doubleheight = DHL_உச்சி;
  இல்லையெனில் எனில்(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  எனில்((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

நிலையான முழு on_text(மாறிலி எழுத்து bytes[], size_t len, வெறுமை *user)
{
  VTermState *state = user;
  முழு npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  முழு i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / அளவு(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  எனில் (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (முழு)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  எனில்(!npoints)
  {
    திரும்பு (முழு)eaten;
  }

  எனில்(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  எனில்(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    எனில்(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      முழு printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      க்கு(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      முழு saved_i = 0;
      போது(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      போது(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        எனில்(saved_i >= (முழு)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      எனில்(saved_i >= (முழு)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      க்கு(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    இல்லையெனில் {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  க்கு(; i < npoints; i++) {
    // Try to find combining characters following this
    முழு glyph_starts = i;
    முழு glyph_ends;
    முழு width = 0;

    க்கு(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      எனில்(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        நிறுத்து;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * அளவு(uint32_t));
    எனில் (chars == வெற்று)
      நிறுத்து;

    க்கு( ; i < glyph_ends; i++) {
      முழு this_width;
      எனில்(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        எனில்(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      எனில்(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      எனில் (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    எனில் (width < 0)
      width = 0;

    போது(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    முழு printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    க்கு(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    எனில்(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    எனில்(state->mode.செருகு) {
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

    எனில்(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      முழு save_i;
      க்கு(save_i = 0; chars[save_i]; save_i++) {
        எனில்(save_i >= (முழு)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      எனில்(save_i >= (முழு)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    எனில்(state->pos.col + width >= THISROWWIDTH(state)) {
      எனில்(state->mode.autowrap)
        state->at_phantom = 1;
    }
    இல்லையெனில் {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  எனில்(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  திரும்பு (முழு)eaten;
}

நிலையான முழு on_control(unsigned எழுத்து control, வெறுமை *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  தேர்வு(control) {
  நிலை 0x07: // BEL - ECMA-48 8.3.3
    எனில்(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    நிறுத்து;

  நிலை 0x08: // BS - ECMA-48 8.3.5
    எனில்(state->pos.col > 0)
      state->pos.col--;
    எனில்(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      எனில்(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    நிறுத்து;

  நிலை 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    நிறுத்து;

  நிலை 0x0a: // LF - ECMA-48 8.3.74
  நிலை 0x0b: // VT
  நிலை 0x0c: // FF
    linefeed(state);
    எனில்(state->mode.newline)
      state->pos.col = 0;
    நிறுத்து;

  நிலை 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    நிறுத்து;

  நிலை 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    நிறுத்து;

  நிலை 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    நிறுத்து;

  நிலை 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    நிறுத்து;

  நிலை 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    நிறுத்து;

  நிலை 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    நிறுத்து;

  நிலை 0x8d: // RI - ECMA-48 8.3.104
    எனில்(state->pos.row == state->scrollregion_உச்சி) {
      VTermRect rect;
      rect.start_row = state->scrollregion_உச்சி;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_இடது(state);
      rect.end_col   = SCROLLREGION_வலது(state);

      scroll(state, rect, -1, 0);
    }
    இல்லையெனில் எனில்(state->pos.row > 0)
        state->pos.row--;
    நிறுத்து;

  நிலை 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    நிறுத்து;

  நிலை 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    நிறுத்து;

  இயல்பு:
    எனில்(state->fallbacks && state->fallbacks->control)
      எனில்((*state->fallbacks->control)(control, state->fbdata))
        திரும்பு 1;

    திரும்பு 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  எனில்(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  திரும்பு 1;
}

நிலையான முழு settermprop_bool(VTermState *state, VTermProp prop, முழு v)
{
  VTerm_மதிப்பு val;
  val.boolean = v;
  திரும்பு vterm_state_set_termprop(state, prop, &val);
}

நிலையான முழு settermprop_int(VTermState *state, VTermProp prop, முழு v)
{
  VTerm_மதிப்பு val;
  val.எண் = v;
  திரும்பு vterm_state_set_termprop(state, prop, &val);
}

நிலையான முழு settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTerm_மதிப்பு val;

  val.string = frag;
  திரும்பு vterm_state_set_termprop(state, prop, &val);
}

நிலையான வெறுமை savecursor(VTermState *state, முழு save)
{
  எனில்(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  இல்லையெனில் {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

நிலையான முழு on_escape(மாறிலி எழுத்து *bytes, size_t len, வெறுமை *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  தேர்வு(bytes[0]) {
  நிலை ' ':
    எனில்(len != 2)
      திரும்பு 0;

    தேர்வு(bytes[1]) {
      நிலை 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        நிறுத்து;

      நிலை 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        நிறுத்து;

      இயல்பு:
        திரும்பு 0;
    }
    திரும்பு 2;

  நிலை '#':
    எனில்(len != 2)
      திரும்பு 0;

    தேர்வு(bytes[1]) {
      நிலை '3': // DECDHL top
        எனில்(state->mode.leftrightmargin)
          நிறுத்து;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_உச்சி);
        நிறுத்து;

      நிலை '4': // DECDHL bottom
        எனில்(state->mode.leftrightmargin)
          நிறுத்து;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        நிறுத்து;

      நிலை '5': // DECSWL
        எனில்(state->mode.leftrightmargin)
          நிறுத்து;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        நிறுத்து;

      நிலை '6': // DECDWL
        எனில்(state->mode.leftrightmargin)
          நிறுத்து;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        நிறுத்து;

      நிலை '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        க்கு(pos.row = 0; pos.row < state->rows; pos.row++)
          க்கு(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        நிறுத்து;
      }

      இயல்பு:
        திரும்பு 0;
    }
    திரும்பு 2;

  நிலை '(': நிலை ')': நிலை '*': நிலை '+': // SCS
    எனில்(len != 2)
      திரும்பு 0;

    {
      முழு setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      எனில்(newenc) {
        state->encoding[setnum].enc = newenc;

        எனில்(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    திரும்பு 2;

  நிலை '7': // DECSC
    savecursor(state, 1);
    திரும்பு 1;

  நிலை '8': // DECRC
    savecursor(state, 0);
    திரும்பு 1;

  நிலை '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    திரும்பு 1;

  நிலை '=': // DECKPAM
    state->mode.keypad = 1;
    திரும்பு 1;

  நிலை '>': // DECKPNM
    state->mode.keypad = 0;
    திரும்பு 1;

  நிலை 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    எனில்(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    திரும்பு 1;
  }

  நிலை 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    திரும்பு 1;

  நிலை 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    திரும்பு 1;

  நிலை '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    திரும்பு 1;

  நிலை '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    திரும்பு 1;

  நிலை '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    திரும்பு 1;

  இயல்பு:
    திரும்பு 0;
  }
}

நிலையான வெறுமை set_mode(VTermState *state, முழு இலக்கம், முழு val)
{
  தேர்வு(இலக்கம்) {
  நிலை 4: // IRM - ECMA-48 7.2.10
    state->mode.செருகு = val;
    நிறுத்து;

  நிலை 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    நிறுத்து;

  இயல்பு:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", இலக்கம்);
    திரும்பு;
  }
}

நிலையான வெறுமை set_dec_mode(VTermState *state, முழு இலக்கம், முழு val)
{
  தேர்வு(இலக்கம்) {
  நிலை 1:
    state->mode.cursor = val;
    நிறுத்து;

  நிலை 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    நிறுத்து;

  நிலை 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_உச்சி : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_இடது(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    நிறுத்து;

  நிலை 7:
    state->mode.autowrap = val;
    நிறுத்து;

  நிலை 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    நிறுத்து;

  நிலை 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    நிறுத்து;

  நிலை 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    எனில்(val) {
      முழு row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      க்கு(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    நிறுத்து;

  நிலை 1000:
  நிலை 1002:
  நிலை 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (இலக்கம் == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (இலக்கம் == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    நிறுத்து;

  நிலை 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    நிறுத்து;

  நிலை 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    நிறுத்து;

  நிலை 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    நிறுத்து;

  நிலை 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    நிறுத்து;

  நிலை 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    நிறுத்து;

  நிலை 1048:
    savecursor(state, val);
    நிறுத்து;

  நிலை 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    நிறுத்து;

  நிலை 2004:
    state->mode.bracketpaste = val;
    நிறுத்து;

  இயல்பு:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", இலக்கம்);
    திரும்பு;
  }
}

நிலையான வெறுமை request_dec_mode(VTermState *state, முழு இலக்கம்)
{
  முழு reply;

  தேர்வு(இலக்கம்) {
    நிலை 1:
      reply = state->mode.cursor;
      நிறுத்து;

    நிலை 5:
      reply = state->mode.screen;
      நிறுத்து;

    நிலை 6:
      reply = state->mode.origin;
      நிறுத்து;

    நிலை 7:
      reply = state->mode.autowrap;
      நிறுத்து;

    நிலை 12:
      reply = state->mode.cursor_blink;
      நிறுத்து;

    நிலை 25:
      reply = state->mode.cursor_visible;
      நிறுத்து;

    நிலை 69:
      reply = state->mode.leftrightmargin;
      நிறுத்து;

    நிலை 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      நிறுத்து;

    நிலை 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      நிறுத்து;

    நிலை 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      நிறுத்து;

    நிலை 1004:
      reply = state->mode.report_focus;
      நிறுத்து;

    நிலை 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      நிறுத்து;

    நிலை 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      நிறுத்து;

    நிலை 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      நிறுத்து;

    நிலை 1047:
      reply = state->mode.alt_screen;
      நிறுத்து;

    நிலை 2004:
      reply = state->mode.bracketpaste;
      நிறுத்து;

    இயல்பு:
      vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", இலக்கம், 0);
      திரும்பு;
  }

  vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", இலக்கம், reply ? 1 : 2);
}

நிலையான வெறுமை request_version_string(VTermState *state)
{
  vterm_தள்ளு_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

நிலையான முழு on_csi(மாறிலி எழுத்து *leader, மாறிலி long args[], முழு argcount, மாறிலி எழுத்து *intermed, எழுத்து command, வெறுமை *user)
{
  VTermState *state = user;
  முழு leader_byte = 0;
  முழு intermed_byte = 0;
  முழு cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  முழு handled = 1;

  // Some temporaries for later code
  முழு மொத்தம், val;
  முழு row, col;
  VTermRect rect;
  முழு selective;

  எனில்(leader && leader[0]) {
    எனில்(leader[1]) // longer than 1 char
      திரும்பு 0;

    தேர்வு(leader[0]) {
    நிலை '?':
    நிலை '>':
    நிலை '<':
      leader_byte = leader[0];
      நிறுத்து;
    இயல்பு:
      திரும்பு 0;
    }
  }

  எனில்(intermed && intermed[0]) {
    எனில்(intermed[1]) // longer than 1 char
      திரும்பு 0;

    தேர்வு(intermed[0]) {
    நிலை ' ':
    நிலை '!':
    நிலை '"':
    நிலை '$':
    நிலை '\'':
      intermed_byte = intermed[0];
      நிறுத்து;
    இயல்பு:
      திரும்பு 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) எனில்((v) < (min)) (v) = (min)
#define UBOUND(v,max) எனில்((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  தேர்வு(intermed_byte << 16 | leader_byte << 8 | command) {
  நிலை 0x40: // ICH - ECMA-48 8.3.64
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    எனில்(!is_cursor_in_scrollregion(state))
      நிறுத்து;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    எனில்(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_வலது(state);
    இல்லையெனில்
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -மொத்தம்);

    நிறுத்து;

  நிலை 0x41: // CUU - ECMA-48 8.3.22
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.row -= மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x42: // CUD - ECMA-48 8.3.19
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.row += மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x43: // CUF - ECMA-48 8.3.20
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.col += மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x44: // CUB - ECMA-48 8.3.18
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.col -= மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x45: // CNL - ECMA-48 8.3.12
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.col = 0;
    state->pos.row += மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x46: // CPL - ECMA-48 8.3.13
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.col = 0;
    state->pos.row -= மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    எனில்(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      எனில்(state->pos.row == row - 1) {
        முழு cnt, ptr = 0;
        க்கு(cnt = 0; cnt < col - 1; ++cnt) {
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
    எனில்(state->mode.origin) {
      state->pos.row += state->scrollregion_உச்சி;
      state->pos.col += SCROLLREGION_இடது(state);
    }
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x49: // CHT - ECMA-48 8.3.10
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    tab(state, மொத்தம், +1);
    நிறுத்து;

  நிலை 0x4a: // ED - ECMA-48 8.3.39
  நிலை LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    தேர்வு(CSI_ARG(args[0])) {
    நிலை CSI_ARG_MISSING:
    நிலை 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      எனில்(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      க்கு(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      எனில்(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      நிறுத்து;

    நிலை 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      க்கு(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      எனில்(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      எனில்(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      நிறுத்து;

    நிலை 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      க்கு(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      நிறுத்து;

    நிலை 3:
      எனில்(state->callbacks && state->callbacks->sb_clear)
        எனில்((*state->callbacks->sb_clear)(state->cbdata))
          திரும்பு 1;
      நிறுத்து;
    }
    நிறுத்து;

  நிலை 0x4b: // EL - ECMA-48 8.3.41
  நிலை LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    தேர்வு(CSI_ARG(args[0])) {
    நிலை CSI_ARG_MISSING:
    நிலை 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); நிறுத்து;
    நிலை 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; நிறுத்து;
    நிலை 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); நிறுத்து;
    இயல்பு:
      திரும்பு 0;
    }

    எனில்(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    நிறுத்து;

  நிலை 0x4c: // IL - ECMA-48 8.3.67
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    எனில்(!is_cursor_in_scrollregion(state))
      நிறுத்து;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_இடது(state);
    rect.end_col   = SCROLLREGION_வலது(state);

    scroll(state, rect, -மொத்தம், 0);

    நிறுத்து;

  நிலை 0x4d: // DL - ECMA-48 8.3.32
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    எனில்(!is_cursor_in_scrollregion(state))
      நிறுத்து;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_இடது(state);
    rect.end_col   = SCROLLREGION_வலது(state);

    scroll(state, rect, மொத்தம், 0);

    நிறுத்து;

  நிலை 0x50: // DCH - ECMA-48 8.3.26
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    எனில்(!is_cursor_in_scrollregion(state))
      நிறுத்து;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    எனில்(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_வலது(state);
    இல்லையெனில்
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, மொத்தம்);

    நிறுத்து;

  நிலை 0x53: // SU - ECMA-48 8.3.147
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    rect.start_row = state->scrollregion_உச்சி;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_இடது(state);
    rect.end_col   = SCROLLREGION_வலது(state);

    scroll(state, rect, மொத்தம், 0);

    நிறுத்து;

  நிலை 0x54: // SD - ECMA-48 8.3.113
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    rect.start_row = state->scrollregion_உச்சி;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_இடது(state);
    rect.end_col   = SCROLLREGION_வலது(state);

    scroll(state, rect, -மொத்தம், 0);

    நிறுத்து;

  நிலை 0x58: // ECH - ECMA-48 8.3.38
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + மொத்தம்;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    நிறுத்து;

  நிலை 0x5a: // CBT - ECMA-48 8.3.7
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    tab(state, மொத்தம், -1);
    நிறுத்து;

  நிலை 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x61: // HPR - ECMA-48 8.3.59
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.col += மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x62: { // REP - ECMA-48 8.3.103
    மாறிலி முழு row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    எனில்(state->combine_width < 1)
      நிறுத்து;

    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    col = state->pos.col + மொத்தம்;
    UBOUND(col, row_width);
    போது (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    எனில் (state->pos.col + state->combine_width >= row_width) {
      எனில் (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    நிறுத்து;
  }

  நிலை 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    எனில்(val == 0)
      // DEC VT100 response
      vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    நிறுத்து;

  நிலை LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    நிறுத்து;

  நிலை 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    எனில்(state->mode.origin)
      state->pos.row += state->scrollregion_உச்சி;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x65: // VPR - ECMA-48 8.3.160
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.row += மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    எனில்(state->mode.origin) {
      state->pos.row += state->scrollregion_உச்சி;
      state->pos.col += SCROLLREGION_இடது(state);
    }
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    தேர்வு(val) {
    நிலை 0:
      clear_col_tabstop(state, state->pos.col);
      நிறுத்து;
    நிலை 3:
    நிலை 5:
      க்கு(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      நிறுத்து;
    நிலை 1:
    நிலை 2:
    நிலை 4:
      நிறுத்து;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    இயல்பு:
      திரும்பு 0;
    }
    நிறுத்து;

  நிலை 0x68: // SM - ECMA-48 8.3.125
    எனில்(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    நிறுத்து;

  நிலை LEADER('?', 0x68): // DEC private mode set
    க்கு(முழு i = 0; i < argcount; i++) {
      எனில்(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    நிறுத்து;

  நிலை 0x6a: // HPB - ECMA-48 8.3.58
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.col -= மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x6b: // VPB - ECMA-48 8.3.159
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);
    state->pos.row -= மொத்தம்;
    state->at_phantom = 0;
    நிறுத்து;

  நிலை 0x6c: // RM - ECMA-48 8.3.106
    எனில்(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    நிறுத்து;

  நிலை LEADER('?', 0x6c): // DEC private mode reset
    க்கு(முழு i = 0; i < argcount; i++) {
      எனில்(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    நிறுத்து;

  நிலை 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    நிறுத்து;

  நிலை LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    எனில் (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_சாவிகள் ? 2 : 0);
      நிறுத்து;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    க்கு(முழு argi = 0; argi < argcount; argi++) {
      long arg;
      தேர்வு(arg = CSI_ARG(args[argi])) {
        நிலை 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          நிறுத்து;
        நிலை 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          நிறுத்து;
        நிலை 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          நிறுத்து;
      }
    }
    நிறுத்து;

  நிலை LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    எனில் (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_சாவிகள் = CSI_ARG(args[1]) == 2;
    }
    நிறுத்து;

  நிலை LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    எனில் (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_சாவிகள் = 0;

      state->mode.kitty_keyboard = 1;
    }
    நிறுத்து;

  நிலை LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    எனில் (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    நிறுத்து;

  நிலை LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    எனில் (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    நிறுத்து;

  நிலை 0x6e: // DSR - ECMA-48 8.3.35
  நிலை LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      எழுத்து *qmark = (leader_byte == '?') ? "?" : "";

      தேர்வு(val) {
      நிலை 0: நிலை 1: நிலை 2: நிலை 3: நிலை 4:
        // ignore - these are replies
        நிறுத்து;
      நிலை 5:
        vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        நிறுத்து;
      நிலை 6: // CPR - cursor position report
        vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        நிறுத்து;
      }
    }
    நிறுத்து;


  நிலை INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    நிறுத்து;

  நிலை LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    நிறுத்து;

  நிலை LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    நிறுத்து;

  நிலை INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    தேர்வு(val) {
    நிலை 0: நிலை 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      நிறுத்து;
    நிலை 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      நிறுத்து;
    நிலை 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      நிறுத்து;
    நிலை 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      நிறுத்து;
    நிலை 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_இடது);
      நிறுத்து;
    நிலை 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_இடது);
      நிறுத்து;
    }

    நிறுத்து;

  நிலை INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    தேர்வு(val) {
    நிலை 0: நிலை 2:
      state->protected_cell = 0;
      நிறுத்து;
    நிலை 1:
      state->protected_cell = 1;
      நிறுத்து;
    }

    நிறுத்து;

  நிலை 0x72: // DECSTBM - DEC custom
    state->scrollregion_உச்சி = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_உச்சி, 0);
    UBOUND(state->scrollregion_உச்சி, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    எனில்(state->scrollregion_உச்சி == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    இல்லையெனில்
      UBOUND(state->scrollregion_bottom, state->rows);

    எனில்(SCROLLREGION_BOTTOM(state) <= state->scrollregion_உச்சி) {
      // Invalid
      state->scrollregion_உச்சி    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    எனில்(state->mode.origin) {
      state->pos.row += state->scrollregion_உச்சி;
      state->pos.col += SCROLLREGION_இடது(state);
    }

    நிறுத்து;

  நிலை 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_இடது = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_வலது = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_இடது, 0);
    UBOUND(state->scrollregion_இடது, state->cols);
    LBOUND(state->scrollregion_வலது, -1);
    எனில்(state->scrollregion_இடது == 0 && state->scrollregion_வலது == state->cols)
      state->scrollregion_வலது = -1;
    இல்லையெனில்
      UBOUND(state->scrollregion_வலது, state->cols);

    எனில்(state->scrollregion_வலது > -1 &&
       state->scrollregion_வலது <= state->scrollregion_இடது) {
      // Invalid
      state->scrollregion_இடது  = 0;
      state->scrollregion_வலது = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    எனில்(state->mode.origin) {
      state->pos.row += state->scrollregion_உச்சி;
      state->pos.col += SCROLLREGION_இடது(state);
    }

    நிறுத்து;

  நிலை 0x74:
    தேர்வு(CSI_ARG(args[0])) {
      நிலை 8: // CSI 8 ; rows ; cols t  set size
        எனில் (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	நிறுத்து;
      இயல்பு:
	handled = 0;
	நிறுத்து;
    }
    நிறுத்து;

  நிலை INTERMED('\'', 0x7D): // DECIC
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    எனில்(!is_cursor_in_scrollregion(state))
      நிறுத்து;

    rect.start_row = state->scrollregion_உச்சி;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_வலது(state);

    scroll(state, rect, 0, -மொத்தம்);

    நிறுத்து;

  நிலை INTERMED('\'', 0x7E): // DECDC
    மொத்தம் = CSI_ARG_மொத்தம்(args[0]);

    எனில்(!is_cursor_in_scrollregion(state))
      நிறுத்து;

    rect.start_row = state->scrollregion_உச்சி;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_வலது(state);

    scroll(state, rect, 0, மொத்தம்);

    நிறுத்து;

  இயல்பு:
    handled = 0;
    நிறுத்து;
  }

  எனில் (!handled) {
    எனில்(state->fallbacks && state->fallbacks->csi)
      எனில்((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        திரும்பு 1;

    திரும்பு 0;
  }

  எனில்(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_உச்சி);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_இடது(state));
    UBOUND(state->pos.col, SCROLLREGION_வலது(state)-1);
  }
  இல்லையெனில் {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  எனில்(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  எனில்(SCROLLREGION_BOTTOM(state) <= state->scrollregion_உச்சி) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_உச்சி);
    abort();
  }

  எனில்(SCROLLREGION_வலது(state) <= SCROLLREGION_இடது(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_வலது(state), SCROLLREGION_இடது(state));
    abort();
  }
#endif

  திரும்பு 1;
}

நிலையான எழுத்து base64_one(uint8_t b)
{
  எனில்(b < 26)
    திரும்பு 'A' + b;
  இல்லையெனில் எனில்(b < 52)
    திரும்பு 'a' + b - 26;
  இல்லையெனில் எனில்(b < 62)
    திரும்பு '0' + b - 52;
  இல்லையெனில் எனில்(b == 62)
    திரும்பு '+';
  இல்லையெனில் எனில்(b == 63)
    திரும்பு '/';
  திரும்பு 0;
}

நிலையான uint8_t unbase64one(எழுத்து c)
{
  எனில்(c >= 'A' && c <= 'Z')
    திரும்பு c - 'A';
  இல்லையெனில் எனில்(c >= 'a' && c <= 'z')
    திரும்பு c - 'a' + 26;
  இல்லையெனில் எனில்(c >= '0' && c <= '9')
    திரும்பு c - '0' + 52;
  இல்லையெனில் எனில்(c == '+')
    திரும்பு 62;
  இல்லையெனில் எனில்(c == '/')
    திரும்பு 63;

  திரும்பு 0xFF;
}

நிலையான வெறுமை osc_selection(VTermState *state, VTermStringFragment frag)
{
  எனில்(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  போது(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    தேர்வு(frag.str[0]) {
      நிலை 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        நிறுத்து;
      நிலை 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        நிறுத்து;
      நிலை 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        நிறுத்து;
      நிலை 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        நிறுத்து;
      நிலை '0':
      நிலை '1':
      நிலை '2':
      நிலை '3':
      நிலை '4':
      நிலை '5':
      நிலை '6':
      நிலை '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        நிறுத்து;

      நிலை ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        எனில்(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        நிறுத்து;
    }

    frag.str++;
    frag.len--;
  }

  எனில்(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    எனில்(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = வெற்று,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    திரும்பு;
  }

  எனில்(state->tmp.selection.state == SELECTION_SELECTED) {
    எனில்(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    இல்லையெனில் {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  எனில்(state->tmp.selection.state == SELECTION_QUERY) {
    எனில்(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    திரும்பு;
  }

  எனில்(state->tmp.selection.state == SELECTION_INVALID)
    திரும்பு;

  எனில்(state->selection.callbacks->set) {
    size_t bufcur = 0;
    எழுத்து *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    முழு n = 0;      /* Number of sextets consumed */

    எனில்(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    போது((state->selection.buflen - bufcur) >= 3 && frag.len) {
      எனில்(frag.str[0] == '=') {
        எனில்(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        எனில்(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        போது(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      இல்லையெனில் {
        uint8_t b = unbase64one(frag.str[0]);
        எனில்(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          எனில்(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = வெற்று,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          நிறுத்து;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        எனில்(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      எனில்(!frag.len || (state->selection.buflen - bufcur) < 3) {
        எனில்(bufcur) {
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

    எனில்(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

நிலையான முழு on_osc(முழு command, VTermStringFragment frag, வெறுமை *user)
{
  VTermState *state = user;

  தேர்வு(command) {
    நிலை 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      திரும்பு 1;

    நிலை 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      திரும்பு 1;

    நிலை 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      திரும்பு 1;

    நிலை 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        முழு red = state->default_fg.red;
        முழு blue = state->default_fg.blue;
        முழு green = state->default_fg.green;
        vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        திரும்பு 1;
      }

    நிலை 11:
      {
	// request background color: <Esc>]11;?<0x07>
	முழு red = state->default_bg.red;
	முழு blue = state->default_bg.blue;
	முழு green = state->default_bg.green;
	vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	திரும்பு 1;
      }
    நிலை 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      திரும்பு 1;

    நிலை 52:
      எனில்(state->selection.callbacks)
        osc_selection(state, frag);

      திரும்பு 1;

    இயல்பு:
      எனில்(state->fallbacks && state->fallbacks->osc)
        எனில்((*state->fallbacks->osc)(command, frag, state->fbdata))
          திரும்பு 1;
  }

  திரும்பு 0;
}

நிலையான வெறுமை request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  எழுத்து *tmp = state->tmp.decrqss;

  எனில்(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  போது(i < அளவு(state->tmp.decrqss)-1 && tmp[i])
    i++;
  போது(i < அளவு(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  எனில்(!frag.final)
    திரும்பு;

  தேர்வு(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    நிலை 'm': {
      // Query SGR
      long args[20];
      முழு argc = vterm_state_getpen(state, args, அளவு(args)/அளவு(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      எனில்(cur >= vt->tmpbuffer_len)
        திரும்பு;

      க்கு(முழு argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        எனில்(cur >= vt->tmpbuffer_len)
          திரும்பு;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      எனில்(cur >= vt->tmpbuffer_len)
        திரும்பு;

      vterm_தள்ளு_output_bytes(vt, vt->tmpbuffer, cur);
      திரும்பு;
    }

    நிலை 'r':
      // Query DECSTBM
      vterm_தள்ளு_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_உச்சி+1, SCROLLREGION_BOTTOM(state));
      திரும்பு;

    நிலை 's':
      // Query DECSLRM
      vterm_தள்ளு_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_இடது(state)+1, SCROLLREGION_வலது(state));
      திரும்பு;

    நிலை ' '|('q'<<8): {
      // Query DECSCUSR
      முழு reply;
      தேர்வு(state->mode.cursor_shape) {
        நிலை VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; நிறுத்து;
        நிலை VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; நிறுத்து;
	இயல்பு: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; நிறுத்து;
      }
      எனில்(state->mode.cursor_blink)
        reply--;
      vterm_தள்ளு_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      திரும்பு;
    }

    நிலை '\"'|('q'<<8):
      // Query DECSCA
      vterm_தள்ளு_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      திரும்பு;
  }

  vterm_தள்ளு_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

நிலையான முழு on_dcs(மாறிலி எழுத்து *command, size_t commandlen, VTermStringFragment frag, வெறுமை *user)
{
  VTermState *state = user;

  எனில்(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    திரும்பு 1;
  }
  இல்லையெனில் எனில்(state->fallbacks && state->fallbacks->dcs)
    எனில்((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      திரும்பு 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (முழு)commandlen, command);
  திரும்பு 0;
}

நிலையான முழு on_apc(VTermStringFragment frag, வெறுமை *user)
{
  VTermState *state = user;

  எனில்(state->fallbacks && state->fallbacks->apc)
    எனில்((*state->fallbacks->apc)(frag, state->fbdata))
      திரும்பு 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  திரும்பு 0;
}

நிலையான முழு on_pm(VTermStringFragment frag, வெறுமை *user)
{
  VTermState *state = user;

  எனில்(state->fallbacks && state->fallbacks->pm)
    எனில்((*state->fallbacks->pm)(frag, state->fbdata))
      திரும்பு 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  திரும்பு 0;
}

நிலையான முழு on_sos(VTermStringFragment frag, வெறுமை *user)
{
  VTermState *state = user;

  எனில்(state->fallbacks && state->fallbacks->sos)
    எனில்((*state->fallbacks->sos)(frag, state->fbdata))
      திரும்பு 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  திரும்பு 0;
}

நிலையான முழு on_resize(முழு rows, முழு cols, வெறுமை *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  எனில் (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  எனில் (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  எனில்(cols != state->cols) {
    unsigned எழுத்து *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    எனில் (newtabstops == வெற்று)
      திரும்பு 0;

    /* TODO: This can all be done much more efficiently bytewise */
    முழு col;
    க்கு(col = 0; col < state->cols && col < cols; col++) {
      unsigned எழுத்து mask = 1 << (col & 7);
      எனில்(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      இல்லையெனில்
        newtabstops[col >> 3] &= ~mask;
      }

    க்கு( ; col < cols; col++) {
      unsigned எழுத்து mask = 1 << (col & 7);
      எனில்(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      இல்லையெனில்
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  எனில்(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  எனில்(state->scrollregion_வலது > -1)
    UBOUND(state->scrollregion_வலது, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_உச்சி, state->rows);
  UBOUND(state->scrollregion_இடது, state->cols);
  எனில்(SCROLLREGION_BOTTOM(state) <= state->scrollregion_உச்சி) {
    state->scrollregion_உச்சி = 0;
    state->scrollregion_bottom = -1;
  }
  எனில்(SCROLLREGION_வலது(state) <= state->scrollregion_இடது) {
    state->scrollregion_இடது = 0;
    state->scrollregion_வலது = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  எனில்(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  இல்லையெனில் {
    எனில்(rows != state->rows) {
      க்கு(முழு bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTerm_வரி_Info *oldlineinfo = state->lineinfos[bufidx];
        எனில்(!oldlineinfo)
          தொடர்;

        VTerm_வரி_Info *newlineinfo = vterm_allocator_malloc(state->vt, rows * அளவு(VTerm_வரி_Info));

        முழு row;
        க்கு(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        க்கு( ; row < rows; row++) {
          newlineinfo[row] = (VTerm_வரி_Info){
            .doublewidth = 0,
          };
        }

        vterm_allocator_free(state->vt, state->lineinfos[bufidx]);
        state->lineinfos[bufidx] = newlineinfo;
      }
    }
  }

  state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];

  எனில்(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  எனில்(state->pos.row < 0)
    state->pos.row = 0;
  எனில்(state->pos.row >= rows)
    state->pos.row = rows - 1;
  எனில்(state->pos.col < 0)
    state->pos.col = 0;
  எனில்(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  திரும்பு 1;
}

நிலையான மாறிலி VTermParserCallbacks parser_callbacks = {
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
  எனில்(vt->state)
    திரும்பு vt->state;

  VTermState *state = vterm_state_new(vt);
  எனில் (state == வெற்று)
    திரும்பு வெற்று;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  திரும்பு state;
}

வெறுமை vterm_state_reset(VTermState *state, முழு hard)
{
  state->scrollregion_உச்சி = 0;
  state->scrollregion_bottom = -1;
  state->scrollregion_இடது = 0;
  state->scrollregion_வலது = -1;

  state->mode.keypad          = 0;
  state->mode.cursor          = 0;
  state->mode.autowrap        = 1;
  state->mode.செருகு          = 0;
  state->mode.newline         = 0;
  state->mode.alt_screen      = 0;
  state->mode.origin          = 0;
  state->mode.leftrightmargin = 0;
  state->mode.bracketpaste    = 0;
  state->mode.report_focus    = 0;

  state->mouse_flags = 0;

  state->vt->mode.ctrl8bit   = 0;

  க்கு(முழு col = 0; col < state->cols; col++)
    எனில்(col % 8 == 0)
      set_col_tabstop(state, col);
    இல்லையெனில்
      clear_col_tabstop(state, col);

  க்கு(முழு row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  எனில்(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  க்கு(முழு i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    எனில்(default_enc->init)
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

  எனில்(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

வெறுமை vterm_state_get_cursorpos(மாறிலி VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

வெறுமை vterm_state_get_mousestate(மாறிலி VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

வெறுமை vterm_state_set_callbacks(VTermState *state, மாறிலி VTermStateCallbacks *callbacks, வெறுமை *user)
{
  எனில்(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    எனில்(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  இல்லையெனில் {
    state->callbacks = வெற்று;
    state->cbdata = வெற்று;
  }
}

வெறுமை vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

வெறுமை *vterm_state_get_cbdata(VTermState *state)
{
  திரும்பு state->cbdata;
}

வெறுமை vterm_state_set_unrecognised_fallbacks(VTermState *state, மாறிலி VTermStateFallbacks *fallbacks, வெறுமை *user)
{
  எனில்(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  இல்லையெனில் {
    state->fallbacks = வெற்று;
    state->fbdata = வெற்று;
  }
}

வெறுமை *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  திரும்பு state->fbdata;
}

முழு vterm_state_set_termprop(VTermState *state, VTermProp prop, VTerm_மதிப்பு *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  எனில்(state->callbacks && state->callbacks->settermprop)
    எனில்(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      திரும்பு 0;

  தேர்வு(prop) {
  நிலை VTERM_PROP_TITLE:
  நிலை VTERM_PROP_ICONNAME:
  நிலை VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    திரும்பு 1;
  நிலை VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    திரும்பு 1;
  நிலை VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    திரும்பு 1;
  நிலை VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->எண்;
    திரும்பு 1;
  நிலை VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    திரும்பு 1;
  நிலை VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    எனில்(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    திரும்பு 1;
  நிலை VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    எனில்(val->எண்)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    எனில்(val->எண் == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    எனில்(val->எண் == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    திரும்பு 1;
  நிலை VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    திரும்பு 1;

  நிலை VTERM_N_PROPS:
    திரும்பு 0;
  }

  திரும்பு 0;
}

வெறுமை vterm_state_focus_in(VTermState *state)
{
  எனில்(state->mode.report_focus)
    vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

வெறுமை vterm_state_focus_out(VTermState *state)
{
  எனில்(state->mode.report_focus)
    vterm_தள்ளு_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

மாறிலி VTerm_வரி_Info *vterm_state_get_lineinfo(மாறிலி VTermState *state, முழு row)
{
  திரும்பு state->lineinfo + row;
}

வெறுமை vterm_state_set_selection_callbacks(VTermState *state, மாறிலி VTermSelectionCallbacks *callbacks, வெறுமை *user,
    எழுத்து *buffer, size_t buflen)
{
  எனில்(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

வெறுமை vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  எனில்(frag.initial) {
    /* TODO: support sending more than one mask bit */
    நிலையான எழுத்து selection_chars[] = "cpqs";
    முழு idx;
    க்கு(idx = 0; idx < 4; idx++)
      எனில்(mask & (1 << idx))
        நிறுத்து;

    vterm_தள்ளு_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  எனில்(frag.len) {
    size_t bufcur = 0;
    எழுத்து *buffer = state->selection.buffer;

    uint32_t x = 0;
    முழு n = 0;

    எனில்(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    போது((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      எனில்(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      எனில்(!frag.len || (state->selection.buflen - bufcur) < 4) {
        எனில்(bufcur)
          vterm_தள்ளு_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    எனில்(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  எனில்(frag.final) {
    எனில்(state->tmp.selection.sendpartial) {
      முழு n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      எழுத்து *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_தள்ளு_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    இல்லையெனில்
      vterm_தள்ளு_output_sprintf_str(vt, 0, TRUE, "");
  }
}
