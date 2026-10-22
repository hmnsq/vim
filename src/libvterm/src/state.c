#include "vterm_internal.h"

#include <stdio.h>
#include <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#အကယ်၍ defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

static ကိန်းပြည့် on_resize(ကိန်းပြည့် rows, ကိန်းပြည့် cols, void *user);

/* Some convenient wrappers to make callback functions easier */

static void putglyph(VTermState *state, const uint32_t chars[], ကိန်းပြည့် width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  အကယ်၍(state->callbacks && state->callbacks->putglyph)
    အကယ်၍((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      ပြန်ပေး;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

static void updatecursor(VTermState *state, VTermPos *oldpos, ကိန်းပြည့် cancel_phantom)
{
  အကယ်၍(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    ပြန်ပေး;

  အကယ်၍(cancel_phantom)
    state->at_phantom = 0;

  အကယ်၍(state->callbacks && state->callbacks->movecursor)
    အကယ်၍((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      ပြန်ပေး;
}

static void erase(VTermState *state, VTermRect rect, ကိန်းပြည့် selective)
{
  အကယ်၍(rect.end_col == state->cols) {
    ကိန်းပြည့် row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    အတွက်(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  အကယ်၍(state->callbacks && state->callbacks->erase)
    အကယ်၍((*state->callbacks->erase)(rect, selective, state->cbdata))
      ပြန်ပေး;
}

static VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, parimaa_na(VTermState));

  အကယ်၍ (state == NULL)
    ပြန်ပေး NULL;
  state->vt = vt;

  state->rows = vt->rows;
  state->cols = vt->cols;

  state->mouse_col     = 0;
  state->mouse_row     = 0;
  state->mouse_buttons = 0;

  state->mouse_protocol = MOUSE_X10;

  state->callbacks = NULL;
  state->cbdata    = NULL;
  state->callbacks_has_premove = 0;

  state->selection.callbacks = NULL;
  state->selection.user      = NULL;
  state->selection.buffer    = NULL;

  vterm_state_newpen(state);

  state->bold_is_highbright = 0;

  state->combine_chars_size = 16;
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * parimaa_na(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * parimaa_na(VTermLineInfo));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * parimaa_na(VTermLineInfo));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  အကယ်၍(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  ပြန်ပေး state;
}

INTERNAL void vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  အကယ်၍(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

static void scroll(VTermState *state, VTermRect rect, ကိန်းပြည့် downward, ကိန်းပြည့် rightward)
{
  ကိန်းပြည့် rows;
  ကိန်းပြည့် cols;
  အကယ်၍(!downward && !rightward)
    ပြန်ပေး;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  အကယ်၍(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    ပြန်ပေး;

  rows = rect.end_row - rect.start_row;
  အကယ်၍(downward > rows)
    downward = rows;
  မဟုတ်ပါက အကယ်၍(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  အကယ်၍(rightward > cols)
    rightward = cols;
  မဟုတ်ပါက အကယ်၍(rightward < -cols)
    rightward = -cols;

  အကယ်၍(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    အကယ်၍(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    မဟုတ်ပါက အကယ်၍(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    အကယ်၍(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    မဟုတ်ပါက အကယ်၍(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  အကယ်၍(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    ကိန်းပြည့် height = rect.end_row - rect.start_row - abs(downward);
    ကိန်းပြည့် row;
    VTermLineInfo zeroLineInfo = {0x0};

    အကယ်၍(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * parimaa_na(state->lineinfo[0]));
      အတွက်(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zeroLineInfo;
    }
    မဟုတ်ပါက {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * parimaa_na(state->lineinfo[0]));
      အတွက်(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zeroLineInfo;
    }
  }

  အကယ်၍(state->callbacks && state->callbacks->scrollrect)
    အကယ်၍((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      ပြန်ပေး;

  အကယ်၍(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

static void linefeed(VTermState *state)
{
  အကယ်၍(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 1, 0);
  }
  မဟုတ်ပါက အကယ်၍(state->pos.row < state->rows-1)
    state->pos.row++;
}

static void grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * parimaa_na(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * parimaa_na(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

static void set_col_tabstop(VTermState *state, ကိန်းပြည့် col)
{
  unsigned var_na mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

static void clear_col_tabstop(VTermState *state, ကိန်းပြည့် col)
{
  unsigned var_na mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

static ကိန်းပြည့် is_col_tabstop(VTermState *state, ကိန်းပြည့် col)
{
  unsigned var_na mask = 1 << (col & 7);
  ပြန်ပေး state->tabstops[col >> 3] & mask;
}

static ကိန်းပြည့် is_cursor_in_scrollregion(const VTermState *state)
{
  အကယ်၍(state->pos.row < state->scrollregion_top ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    ပြန်ပေး 0;
  အကယ်၍(state->pos.col < SCROLLREGION_LEFT(state) ||
     state->pos.col >= SCROLLREGION_RIGHT(state))
    ပြန်ပေး 0;

  ပြန်ပေး 1;
}

static void tab(VTermState *state, ကိန်းပြည့် count, ကိန်းပြည့် direction)
{
  နေစဉ်(count > 0) {
    အကယ်၍(direction > 0) {
      အကယ်၍(state->pos.col >= THISROWWIDTH(state)-1)
        ပြန်ပေး;

      state->pos.col++;
    }
    မဟုတ်ပါက အကယ်၍(direction < 0) {
      အကယ်၍(state->pos.col < 1)
        ပြန်ပေး;

      state->pos.col--;
    }

    အကယ်၍(is_col_tabstop(state, state->pos.col))
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

static void set_lineinfo(VTermState *state, ကိန်းပြည့် row, ကိန်းပြည့် force, ကိန်းပြည့် dwl, ကိန်းပြည့် dhl)
{
  VTermLineInfo info = state->lineinfo[row];

  အကယ်၍(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  မဟုတ်ပါက အကယ်၍(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  အကယ်၍(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  မဟုတ်ပါက အကယ်၍(dhl == DHL_TOP)
    info.doubleheight = DHL_TOP;
  မဟုတ်ပါက အကယ်၍(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  အကယ်၍((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

static ကိန်းပြည့် on_text(const var_na bytes[], size_t len, void *user)
{
  VTermState *state = user;
  ကိန်းပြည့် npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  ကိန်းပြည့် i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / parimaa_na(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  အကယ်၍ (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (ကိန်းပြည့်)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  အကယ်၍(!npoints)
  {
    ပြန်ပေး (ကိန်းပြည့်)eaten;
  }

  အကယ်၍(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  အကယ်၍(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    အကယ်၍(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      ကိန်းပြည့် printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      အတွက်(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      ကိန်းပြည့် saved_i = 0;
      နေစဉ်(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      နေစဉ်(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        အကယ်၍(saved_i >= (ကိန်းပြည့်)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      အကယ်၍(saved_i >= (ကိန်းပြည့်)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      အတွက်(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    မဟုတ်ပါက {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  အတွက်(; i < npoints; i++) {
    // Try to find combining characters following this
    ကိန်းပြည့် glyph_starts = i;
    ကိန်းပြည့် glyph_ends;
    ကိန်းပြည့် width = 0;

    အတွက်(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      အကယ်၍(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        bha_nga;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * parimaa_na(uint32_t));
    အကယ်၍ (chars == NULL)
      bha_nga;

    အတွက်( ; i < glyph_ends; i++) {
      ကိန်းပြည့် this_width;
      အကယ်၍(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        အကယ်၍(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      အကယ်၍(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      အကယ်၍ (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    အကယ်၍ (width < 0)
      width = 0;

    နေစဉ်(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    ကိန်းပြည့် printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    အတွက်(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    အကယ်၍(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    အကယ်၍(state->mode.insert) {
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

    အကယ်၍(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      ကိန်းပြည့် save_i;
      အတွက်(save_i = 0; chars[save_i]; save_i++) {
        အကယ်၍(save_i >= (ကိန်းပြည့်)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      အကယ်၍(save_i >= (ကိန်းပြည့်)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    အကယ်၍(state->pos.col + width >= THISROWWIDTH(state)) {
      အကယ်၍(state->mode.autowrap)
        state->at_phantom = 1;
    }
    မဟုတ်ပါက {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  အကယ်၍(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  ပြန်ပေး (ကိန်းပြည့်)eaten;
}

static ကိန်းပြည့် on_control(unsigned var_na control, void *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  switch(control) {
  pak_sa 0x07: // BEL - ECMA-48 8.3.3
    အကယ်၍(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    bha_nga;

  pak_sa 0x08: // BS - ECMA-48 8.3.5
    အကယ်၍(state->pos.col > 0)
      state->pos.col--;
    အကယ်၍(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      အကယ်၍(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    bha_nga;

  pak_sa 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    bha_nga;

  pak_sa 0x0a: // LF - ECMA-48 8.3.74
  pak_sa 0x0b: // VT
  pak_sa 0x0c: // FF
    linefeed(state);
    အကယ်၍(state->mode.newline)
      state->pos.col = 0;
    bha_nga;

  pak_sa 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    bha_nga;

  pak_sa 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    bha_nga;

  pak_sa 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    bha_nga;

  pak_sa 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    bha_nga;

  pak_sa 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    bha_nga;

  pak_sa 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    bha_nga;

  pak_sa 0x8d: // RI - ECMA-48 8.3.104
    အကယ်၍(state->pos.row == state->scrollregion_top) {
      VTermRect rect;
      rect.start_row = state->scrollregion_top;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_LEFT(state);
      rect.end_col   = SCROLLREGION_RIGHT(state);

      scroll(state, rect, -1, 0);
    }
    မဟုတ်ပါက အကယ်၍(state->pos.row > 0)
        state->pos.row--;
    bha_nga;

  pak_sa 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    bha_nga;

  pak_sa 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    bha_nga;

  default:
    အကယ်၍(state->fallbacks && state->fallbacks->control)
      အကယ်၍((*state->fallbacks->control)(control, state->fbdata))
        ပြန်ပေး 1;

    ပြန်ပေး 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  အကယ်၍(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  ပြန်ပေး 1;
}

static ကိန်းပြည့် settermprop_bool(VTermState *state, VTermProp prop, ကိန်းပြည့် v)
{
  VTermValue val;
  val.boolean = v;
  ပြန်ပေး vterm_state_set_termprop(state, prop, &val);
}

static ကိန်းပြည့် settermprop_int(VTermState *state, VTermProp prop, ကိန်းပြည့် v)
{
  VTermValue val;
  val.number = v;
  ပြန်ပေး vterm_state_set_termprop(state, prop, &val);
}

static ကိန်းပြည့် settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTermValue val;

  val.string = frag;
  ပြန်ပေး vterm_state_set_termprop(state, prop, &val);
}

static void savecursor(VTermState *state, ကိန်းပြည့် save)
{
  အကယ်၍(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  မဟုတ်ပါက {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

static ကိန်းပြည့် on_escape(const var_na *bytes, size_t len, void *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  switch(bytes[0]) {
  pak_sa ' ':
    အကယ်၍(len != 2)
      ပြန်ပေး 0;

    switch(bytes[1]) {
      pak_sa 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        bha_nga;

      pak_sa 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        bha_nga;

      default:
        ပြန်ပေး 0;
    }
    ပြန်ပေး 2;

  pak_sa '#':
    အကယ်၍(len != 2)
      ပြန်ပေး 0;

    switch(bytes[1]) {
      pak_sa '3': // DECDHL top
        အကယ်၍(state->mode.leftrightmargin)
          bha_nga;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_TOP);
        bha_nga;

      pak_sa '4': // DECDHL bottom
        အကယ်၍(state->mode.leftrightmargin)
          bha_nga;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        bha_nga;

      pak_sa '5': // DECSWL
        အကယ်၍(state->mode.leftrightmargin)
          bha_nga;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        bha_nga;

      pak_sa '6': // DECDWL
        အကယ်၍(state->mode.leftrightmargin)
          bha_nga;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        bha_nga;

      pak_sa '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        အတွက်(pos.row = 0; pos.row < state->rows; pos.row++)
          အတွက်(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        bha_nga;
      }

      default:
        ပြန်ပေး 0;
    }
    ပြန်ပေး 2;

  pak_sa '(': pak_sa ')': pak_sa '*': pak_sa '+': // SCS
    အကယ်၍(len != 2)
      ပြန်ပေး 0;

    {
      ကိန်းပြည့် setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      အကယ်၍(newenc) {
        state->encoding[setnum].enc = newenc;

        အကယ်၍(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    ပြန်ပေး 2;

  pak_sa '7': // DECSC
    savecursor(state, 1);
    ပြန်ပေး 1;

  pak_sa '8': // DECRC
    savecursor(state, 0);
    ပြန်ပေး 1;

  pak_sa '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    ပြန်ပေး 1;

  pak_sa '=': // DECKPAM
    state->mode.keypad = 1;
    ပြန်ပေး 1;

  pak_sa '>': // DECKPNM
    state->mode.keypad = 0;
    ပြန်ပေး 1;

  pak_sa 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    အကယ်၍(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    ပြန်ပေး 1;
  }

  pak_sa 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    ပြန်ပေး 1;

  pak_sa 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    ပြန်ပေး 1;

  pak_sa '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    ပြန်ပေး 1;

  pak_sa '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    ပြန်ပေး 1;

  pak_sa '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    ပြန်ပေး 1;

  default:
    ပြန်ပေး 0;
  }
}

static void set_mode(VTermState *state, ကိန်းပြည့် num, ကိန်းပြည့် val)
{
  switch(num) {
  pak_sa 4: // IRM - ECMA-48 7.2.10
    state->mode.insert = val;
    bha_nga;

  pak_sa 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    bha_nga;

  default:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", num);
    ပြန်ပေး;
  }
}

static void set_dec_mode(VTermState *state, ကိန်းပြည့် num, ကိန်းပြည့် val)
{
  switch(num) {
  pak_sa 1:
    state->mode.cursor = val;
    bha_nga;

  pak_sa 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    bha_nga;

  pak_sa 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_top : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_LEFT(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    bha_nga;

  pak_sa 7:
    state->mode.autowrap = val;
    bha_nga;

  pak_sa 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    bha_nga;

  pak_sa 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    bha_nga;

  pak_sa 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    အကယ်၍(val) {
      ကိန်းပြည့် row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      အတွက်(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    bha_nga;

  pak_sa 1000:
  pak_sa 1002:
  pak_sa 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (num == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (num == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    bha_nga;

  pak_sa 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    bha_nga;

  pak_sa 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    bha_nga;

  pak_sa 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    bha_nga;

  pak_sa 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    bha_nga;

  pak_sa 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    bha_nga;

  pak_sa 1048:
    savecursor(state, val);
    bha_nga;

  pak_sa 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    bha_nga;

  pak_sa 2004:
    state->mode.bracketpaste = val;
    bha_nga;

  default:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", num);
    ပြန်ပေး;
  }
}

static void request_dec_mode(VTermState *state, ကိန်းပြည့် num)
{
  ကိန်းပြည့် reply;

  switch(num) {
    pak_sa 1:
      reply = state->mode.cursor;
      bha_nga;

    pak_sa 5:
      reply = state->mode.screen;
      bha_nga;

    pak_sa 6:
      reply = state->mode.origin;
      bha_nga;

    pak_sa 7:
      reply = state->mode.autowrap;
      bha_nga;

    pak_sa 12:
      reply = state->mode.cursor_blink;
      bha_nga;

    pak_sa 25:
      reply = state->mode.cursor_visible;
      bha_nga;

    pak_sa 69:
      reply = state->mode.leftrightmargin;
      bha_nga;

    pak_sa 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      bha_nga;

    pak_sa 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      bha_nga;

    pak_sa 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      bha_nga;

    pak_sa 1004:
      reply = state->mode.report_focus;
      bha_nga;

    pak_sa 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      bha_nga;

    pak_sa 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      bha_nga;

    pak_sa 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      bha_nga;

    pak_sa 1047:
      reply = state->mode.alt_screen;
      bha_nga;

    pak_sa 2004:
      reply = state->mode.bracketpaste;
      bha_nga;

    default:
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", num, 0);
      ပြန်ပေး;
  }

  vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", num, reply ? 1 : 2);
}

static void request_version_string(VTermState *state)
{
  vterm_push_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

static ကိန်းပြည့် on_csi(const var_na *leader, const long args[], ကိန်းပြည့် argcount, const var_na *intermed, var_na command, void *user)
{
  VTermState *state = user;
  ကိန်းပြည့် leader_byte = 0;
  ကိန်းပြည့် intermed_byte = 0;
  ကိန်းပြည့် cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  ကိန်းပြည့် handled = 1;

  // Some temporaries for later code
  ကိန်းပြည့် count, val;
  ကိန်းပြည့် row, col;
  VTermRect rect;
  ကိန်းပြည့် selective;

  အကယ်၍(leader && leader[0]) {
    အကယ်၍(leader[1]) // longer than 1 char
      ပြန်ပေး 0;

    switch(leader[0]) {
    pak_sa '?':
    pak_sa '>':
    pak_sa '<':
      leader_byte = leader[0];
      bha_nga;
    default:
      ပြန်ပေး 0;
    }
  }

  အကယ်၍(intermed && intermed[0]) {
    အကယ်၍(intermed[1]) // longer than 1 char
      ပြန်ပေး 0;

    switch(intermed[0]) {
    pak_sa ' ':
    pak_sa '!':
    pak_sa '"':
    pak_sa '$':
    pak_sa '\'':
      intermed_byte = intermed[0];
      bha_nga;
    default:
      ပြန်ပေး 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) အကယ်၍((v) < (min)) (v) = (min)
#define UBOUND(v,max) အကယ်၍((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  switch(intermed_byte << 16 | leader_byte << 8 | command) {
  pak_sa 0x40: // ICH - ECMA-48 8.3.64
    count = CSI_ARG_COUNT(args[0]);

    အကယ်၍(!is_cursor_in_scrollregion(state))
      bha_nga;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    အကယ်၍(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_RIGHT(state);
    မဟုတ်ပါက
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -count);

    bha_nga;

  pak_sa 0x41: // CUU - ECMA-48 8.3.22
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row -= count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x42: // CUD - ECMA-48 8.3.19
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row += count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x43: // CUF - ECMA-48 8.3.20
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col += count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x44: // CUB - ECMA-48 8.3.18
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col -= count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x45: // CNL - ECMA-48 8.3.12
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col = 0;
    state->pos.row += count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x46: // CPL - ECMA-48 8.3.13
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col = 0;
    state->pos.row -= count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    အကယ်၍(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      အကယ်၍(state->pos.row == row - 1) {
        ကိန်းပြည့် cnt, ptr = 0;
        အတွက်(cnt = 0; cnt < col - 1; ++cnt) {
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
    အကယ်၍(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x49: // CHT - ECMA-48 8.3.10
    count = CSI_ARG_COUNT(args[0]);
    tab(state, count, +1);
    bha_nga;

  pak_sa 0x4a: // ED - ECMA-48 8.3.39
  pak_sa LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    switch(CSI_ARG(args[0])) {
    pak_sa CSI_ARG_MISSING:
    pak_sa 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      အကယ်၍(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      အတွက်(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      အကယ်၍(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      bha_nga;

    pak_sa 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      အတွက်(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      အကယ်၍(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      အကယ်၍(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      bha_nga;

    pak_sa 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      အတွက်(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      bha_nga;

    pak_sa 3:
      အကယ်၍(state->callbacks && state->callbacks->sb_clear)
        အကယ်၍((*state->callbacks->sb_clear)(state->cbdata))
          ပြန်ပေး 1;
      bha_nga;
    }
    bha_nga;

  pak_sa 0x4b: // EL - ECMA-48 8.3.41
  pak_sa LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    switch(CSI_ARG(args[0])) {
    pak_sa CSI_ARG_MISSING:
    pak_sa 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); bha_nga;
    pak_sa 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; bha_nga;
    pak_sa 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); bha_nga;
    default:
      ပြန်ပေး 0;
    }

    အကယ်၍(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    bha_nga;

  pak_sa 0x4c: // IL - ECMA-48 8.3.67
    count = CSI_ARG_COUNT(args[0]);

    အကယ်၍(!is_cursor_in_scrollregion(state))
      bha_nga;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, -count, 0);

    bha_nga;

  pak_sa 0x4d: // DL - ECMA-48 8.3.32
    count = CSI_ARG_COUNT(args[0]);

    အကယ်၍(!is_cursor_in_scrollregion(state))
      bha_nga;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, count, 0);

    bha_nga;

  pak_sa 0x50: // DCH - ECMA-48 8.3.26
    count = CSI_ARG_COUNT(args[0]);

    အကယ်၍(!is_cursor_in_scrollregion(state))
      bha_nga;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    အကယ်၍(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_RIGHT(state);
    မဟုတ်ပါက
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, count);

    bha_nga;

  pak_sa 0x53: // SU - ECMA-48 8.3.147
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, count, 0);

    bha_nga;

  pak_sa 0x54: // SD - ECMA-48 8.3.113
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, -count, 0);

    bha_nga;

  pak_sa 0x58: // ECH - ECMA-48 8.3.38
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + count;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    bha_nga;

  pak_sa 0x5a: // CBT - ECMA-48 8.3.7
    count = CSI_ARG_COUNT(args[0]);
    tab(state, count, -1);
    bha_nga;

  pak_sa 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x61: // HPR - ECMA-48 8.3.59
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col += count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x62: { // REP - ECMA-48 8.3.103
    const ကိန်းပြည့် row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    အကယ်၍(state->combine_width < 1)
      bha_nga;

    count = CSI_ARG_COUNT(args[0]);
    col = state->pos.col + count;
    UBOUND(col, row_width);
    နေစဉ် (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    အကယ်၍ (state->pos.col + state->combine_width >= row_width) {
      အကယ်၍ (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    bha_nga;
  }

  pak_sa 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    အကယ်၍(val == 0)
      // DEC VT100 response
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    bha_nga;

  pak_sa LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    bha_nga;

  pak_sa 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    အကယ်၍(state->mode.origin)
      state->pos.row += state->scrollregion_top;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x65: // VPR - ECMA-48 8.3.160
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row += count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    အကယ်၍(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    switch(val) {
    pak_sa 0:
      clear_col_tabstop(state, state->pos.col);
      bha_nga;
    pak_sa 3:
    pak_sa 5:
      အတွက်(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      bha_nga;
    pak_sa 1:
    pak_sa 2:
    pak_sa 4:
      bha_nga;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    default:
      ပြန်ပေး 0;
    }
    bha_nga;

  pak_sa 0x68: // SM - ECMA-48 8.3.125
    အကယ်၍(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    bha_nga;

  pak_sa LEADER('?', 0x68): // DEC private mode set
    အတွက်(ကိန်းပြည့် i = 0; i < argcount; i++) {
      အကယ်၍(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    bha_nga;

  pak_sa 0x6a: // HPB - ECMA-48 8.3.58
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col -= count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x6b: // VPB - ECMA-48 8.3.159
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row -= count;
    state->at_phantom = 0;
    bha_nga;

  pak_sa 0x6c: // RM - ECMA-48 8.3.106
    အကယ်၍(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    bha_nga;

  pak_sa LEADER('?', 0x6c): // DEC private mode reset
    အတွက်(ကိန်းပြည့် i = 0; i < argcount; i++) {
      အကယ်၍(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    bha_nga;

  pak_sa 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    bha_nga;

  pak_sa LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    အကယ်၍ (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_keys ? 2 : 0);
      bha_nga;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    အတွက်(ကိန်းပြည့် argi = 0; argi < argcount; argi++) {
      long arg;
      switch(arg = CSI_ARG(args[argi])) {
        pak_sa 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          bha_nga;
        pak_sa 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          bha_nga;
        pak_sa 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          bha_nga;
      }
    }
    bha_nga;

  pak_sa LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    အကယ်၍ (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_keys = CSI_ARG(args[1]) == 2;
    }
    bha_nga;

  pak_sa LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    အကယ်၍ (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_keys = 0;

      state->mode.kitty_keyboard = 1;
    }
    bha_nga;

  pak_sa LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    အကယ်၍ (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    bha_nga;

  pak_sa LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    အကယ်၍ (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    bha_nga;

  pak_sa 0x6e: // DSR - ECMA-48 8.3.35
  pak_sa LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      var_na *qmark = (leader_byte == '?') ? "?" : "";

      switch(val) {
      pak_sa 0: pak_sa 1: pak_sa 2: pak_sa 3: pak_sa 4:
        // ignore - these are replies
        bha_nga;
      pak_sa 5:
        vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        bha_nga;
      pak_sa 6: // CPR - cursor position report
        vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        bha_nga;
      }
    }
    bha_nga;


  pak_sa INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    bha_nga;

  pak_sa LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    bha_nga;

  pak_sa LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    bha_nga;

  pak_sa INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    switch(val) {
    pak_sa 0: pak_sa 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      bha_nga;
    pak_sa 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      bha_nga;
    pak_sa 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      bha_nga;
    pak_sa 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      bha_nga;
    pak_sa 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_LEFT);
      bha_nga;
    pak_sa 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_LEFT);
      bha_nga;
    }

    bha_nga;

  pak_sa INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    switch(val) {
    pak_sa 0: pak_sa 2:
      state->protected_cell = 0;
      bha_nga;
    pak_sa 1:
      state->protected_cell = 1;
      bha_nga;
    }

    bha_nga;

  pak_sa 0x72: // DECSTBM - DEC custom
    state->scrollregion_top = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_top, 0);
    UBOUND(state->scrollregion_top, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    အကယ်၍(state->scrollregion_top == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    မဟုတ်ပါက
      UBOUND(state->scrollregion_bottom, state->rows);

    အကယ်၍(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
      // Invalid
      state->scrollregion_top    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    အကယ်၍(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }

    bha_nga;

  pak_sa 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_left = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_right = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_left, 0);
    UBOUND(state->scrollregion_left, state->cols);
    LBOUND(state->scrollregion_right, -1);
    အကယ်၍(state->scrollregion_left == 0 && state->scrollregion_right == state->cols)
      state->scrollregion_right = -1;
    မဟုတ်ပါက
      UBOUND(state->scrollregion_right, state->cols);

    အကယ်၍(state->scrollregion_right > -1 &&
       state->scrollregion_right <= state->scrollregion_left) {
      // Invalid
      state->scrollregion_left  = 0;
      state->scrollregion_right = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    အကယ်၍(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }

    bha_nga;

  pak_sa 0x74:
    switch(CSI_ARG(args[0])) {
      pak_sa 8: // CSI 8 ; rows ; cols t  set size
        အကယ်၍ (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	bha_nga;
      default:
	handled = 0;
	bha_nga;
    }
    bha_nga;

  pak_sa INTERMED('\'', 0x7D): // DECIC
    count = CSI_ARG_COUNT(args[0]);

    အကယ်၍(!is_cursor_in_scrollregion(state))
      bha_nga;

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 0, -count);

    bha_nga;

  pak_sa INTERMED('\'', 0x7E): // DECDC
    count = CSI_ARG_COUNT(args[0]);

    အကယ်၍(!is_cursor_in_scrollregion(state))
      bha_nga;

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 0, count);

    bha_nga;

  default:
    handled = 0;
    bha_nga;
  }

  အကယ်၍ (!handled) {
    အကယ်၍(state->fallbacks && state->fallbacks->csi)
      အကယ်၍((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        ပြန်ပေး 1;

    ပြန်ပေး 0;
  }

  အကယ်၍(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_top);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_LEFT(state));
    UBOUND(state->pos.col, SCROLLREGION_RIGHT(state)-1);
  }
  မဟုတ်ပါက {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  အကယ်၍(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  အကယ်၍(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_top);
    abort();
  }

  အကယ်၍(SCROLLREGION_RIGHT(state) <= SCROLLREGION_LEFT(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_RIGHT(state), SCROLLREGION_LEFT(state));
    abort();
  }
#endif

  ပြန်ပေး 1;
}

static var_na base64_one(uint8_t b)
{
  အကယ်၍(b < 26)
    ပြန်ပေး 'A' + b;
  မဟုတ်ပါက အကယ်၍(b < 52)
    ပြန်ပေး 'a' + b - 26;
  မဟုတ်ပါက အကယ်၍(b < 62)
    ပြန်ပေး '0' + b - 52;
  မဟုတ်ပါက အကယ်၍(b == 62)
    ပြန်ပေး '+';
  မဟုတ်ပါက အကယ်၍(b == 63)
    ပြန်ပေး '/';
  ပြန်ပေး 0;
}

static uint8_t unbase64one(var_na c)
{
  အကယ်၍(c >= 'A' && c <= 'Z')
    ပြန်ပေး c - 'A';
  မဟုတ်ပါက အကယ်၍(c >= 'a' && c <= 'z')
    ပြန်ပေး c - 'a' + 26;
  မဟုတ်ပါက အကယ်၍(c >= '0' && c <= '9')
    ပြန်ပေး c - '0' + 52;
  မဟုတ်ပါက အကယ်၍(c == '+')
    ပြန်ပေး 62;
  မဟုတ်ပါက အကယ်၍(c == '/')
    ပြန်ပေး 63;

  ပြန်ပေး 0xFF;
}

static void osc_selection(VTermState *state, VTermStringFragment frag)
{
  အကယ်၍(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  နေစဉ်(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    switch(frag.str[0]) {
      pak_sa 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        bha_nga;
      pak_sa 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        bha_nga;
      pak_sa 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        bha_nga;
      pak_sa 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        bha_nga;
      pak_sa '0':
      pak_sa '1':
      pak_sa '2':
      pak_sa '3':
      pak_sa '4':
      pak_sa '5':
      pak_sa '6':
      pak_sa '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        bha_nga;

      pak_sa ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        အကယ်၍(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        bha_nga;
    }

    frag.str++;
    frag.len--;
  }

  အကယ်၍(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    အကယ်၍(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = NULL,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    ပြန်ပေး;
  }

  အကယ်၍(state->tmp.selection.state == SELECTION_SELECTED) {
    အကယ်၍(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    မဟုတ်ပါက {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  အကယ်၍(state->tmp.selection.state == SELECTION_QUERY) {
    အကယ်၍(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    ပြန်ပေး;
  }

  အကယ်၍(state->tmp.selection.state == SELECTION_INVALID)
    ပြန်ပေး;

  အကယ်၍(state->selection.callbacks->set) {
    size_t bufcur = 0;
    var_na *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    ကိန်းပြည့် n = 0;      /* Number of sextets consumed */

    အကယ်၍(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    နေစဉ်((state->selection.buflen - bufcur) >= 3 && frag.len) {
      အကယ်၍(frag.str[0] == '=') {
        အကယ်၍(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        အကယ်၍(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        နေစဉ်(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      မဟုတ်ပါက {
        uint8_t b = unbase64one(frag.str[0]);
        အကယ်၍(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          အကယ်၍(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = NULL,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          bha_nga;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        အကယ်၍(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      အကယ်၍(!frag.len || (state->selection.buflen - bufcur) < 3) {
        အကယ်၍(bufcur) {
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

    အကယ်၍(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

static ကိန်းပြည့် on_osc(ကိန်းပြည့် command, VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  switch(command) {
    pak_sa 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ပြန်ပေး 1;

    pak_sa 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      ပြန်ပေး 1;

    pak_sa 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      ပြန်ပေး 1;

    pak_sa 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        ကိန်းပြည့် red = state->default_fg.red;
        ကိန်းပြည့် blue = state->default_fg.blue;
        ကိန်းပြည့် green = state->default_fg.green;
        vterm_push_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        ပြန်ပေး 1;
      }

    pak_sa 11:
      {
	// request background color: <Esc>]11;?<0x07>
	ကိန်းပြည့် red = state->default_bg.red;
	ကိန်းပြည့် blue = state->default_bg.blue;
	ကိန်းပြည့် green = state->default_bg.green;
	vterm_push_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	ပြန်ပေး 1;
      }
    pak_sa 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      ပြန်ပေး 1;

    pak_sa 52:
      အကယ်၍(state->selection.callbacks)
        osc_selection(state, frag);

      ပြန်ပေး 1;

    default:
      အကယ်၍(state->fallbacks && state->fallbacks->osc)
        အကယ်၍((*state->fallbacks->osc)(command, frag, state->fbdata))
          ပြန်ပေး 1;
  }

  ပြန်ပေး 0;
}

static void request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  var_na *tmp = state->tmp.decrqss;

  အကယ်၍(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  နေစဉ်(i < parimaa_na(state->tmp.decrqss)-1 && tmp[i])
    i++;
  နေစဉ်(i < parimaa_na(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  အကယ်၍(!frag.final)
    ပြန်ပေး;

  switch(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    pak_sa 'm': {
      // Query SGR
      long args[20];
      ကိန်းပြည့် argc = vterm_state_getpen(state, args, parimaa_na(args)/parimaa_na(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      အကယ်၍(cur >= vt->tmpbuffer_len)
        ပြန်ပေး;

      အတွက်(ကိန်းပြည့် argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        အကယ်၍(cur >= vt->tmpbuffer_len)
          ပြန်ပေး;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      အကယ်၍(cur >= vt->tmpbuffer_len)
        ပြန်ပေး;

      vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
      ပြန်ပေး;
    }

    pak_sa 'r':
      // Query DECSTBM
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_top+1, SCROLLREGION_BOTTOM(state));
      ပြန်ပေး;

    pak_sa 's':
      // Query DECSLRM
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_LEFT(state)+1, SCROLLREGION_RIGHT(state));
      ပြန်ပေး;

    pak_sa ' '|('q'<<8): {
      // Query DECSCUSR
      ကိန်းပြည့် reply;
      switch(state->mode.cursor_shape) {
        pak_sa VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; bha_nga;
        pak_sa VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; bha_nga;
	default: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; bha_nga;
      }
      အကယ်၍(state->mode.cursor_blink)
        reply--;
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      ပြန်ပေး;
    }

    pak_sa '\"'|('q'<<8):
      // Query DECSCA
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      ပြန်ပေး;
  }

  vterm_push_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

static ကိန်းပြည့် on_dcs(const var_na *command, size_t commandlen, VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  အကယ်၍(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    ပြန်ပေး 1;
  }
  မဟုတ်ပါက အကယ်၍(state->fallbacks && state->fallbacks->dcs)
    အကယ်၍((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      ပြန်ပေး 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (ကိန်းပြည့်)commandlen, command);
  ပြန်ပေး 0;
}

static ကိန်းပြည့် on_apc(VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  အကယ်၍(state->fallbacks && state->fallbacks->apc)
    အကယ်၍((*state->fallbacks->apc)(frag, state->fbdata))
      ပြန်ပေး 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  ပြန်ပေး 0;
}

static ကိန်းပြည့် on_pm(VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  အကယ်၍(state->fallbacks && state->fallbacks->pm)
    အကယ်၍((*state->fallbacks->pm)(frag, state->fbdata))
      ပြန်ပေး 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  ပြန်ပေး 0;
}

static ကိန်းပြည့် on_sos(VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  အကယ်၍(state->fallbacks && state->fallbacks->sos)
    အကယ်၍((*state->fallbacks->sos)(frag, state->fbdata))
      ပြန်ပေး 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  ပြန်ပေး 0;
}

static ကိန်းပြည့် on_resize(ကိန်းပြည့် rows, ကိန်းပြည့် cols, void *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  အကယ်၍ (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  အကယ်၍ (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  အကယ်၍(cols != state->cols) {
    unsigned var_na *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    အကယ်၍ (newtabstops == NULL)
      ပြန်ပေး 0;

    /* TODO: This can all be done much more efficiently bytewise */
    ကိန်းပြည့် col;
    အတွက်(col = 0; col < state->cols && col < cols; col++) {
      unsigned var_na mask = 1 << (col & 7);
      အကယ်၍(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      မဟုတ်ပါက
        newtabstops[col >> 3] &= ~mask;
      }

    အတွက်( ; col < cols; col++) {
      unsigned var_na mask = 1 << (col & 7);
      အကယ်၍(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      မဟုတ်ပါက
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  အကယ်၍(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  အကယ်၍(state->scrollregion_right > -1)
    UBOUND(state->scrollregion_right, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_top, state->rows);
  UBOUND(state->scrollregion_left, state->cols);
  အကယ်၍(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
    state->scrollregion_top = 0;
    state->scrollregion_bottom = -1;
  }
  အကယ်၍(SCROLLREGION_RIGHT(state) <= state->scrollregion_left) {
    state->scrollregion_left = 0;
    state->scrollregion_right = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  အကယ်၍(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  မဟုတ်ပါက {
    အကယ်၍(rows != state->rows) {
      အတွက်(ကိန်းပြည့် bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTermLineInfo *oldlineinfo = state->lineinfos[bufidx];
        အကယ်၍(!oldlineinfo)
          continue;

        VTermLineInfo *newlineinfo = vterm_allocator_malloc(state->vt, rows * parimaa_na(VTermLineInfo));

        ကိန်းပြည့် row;
        အတွက်(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        အတွက်( ; row < rows; row++) {
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

  အကယ်၍(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  အကယ်၍(state->pos.row < 0)
    state->pos.row = 0;
  အကယ်၍(state->pos.row >= rows)
    state->pos.row = rows - 1;
  အကယ်၍(state->pos.col < 0)
    state->pos.col = 0;
  အကယ်၍(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  ပြန်ပေး 1;
}

static const VTermParserCallbacks parser_callbacks = {
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
  အကယ်၍(vt->state)
    ပြန်ပေး vt->state;

  VTermState *state = vterm_state_new(vt);
  အကယ်၍ (state == NULL)
    ပြန်ပေး NULL;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  ပြန်ပေး state;
}

void vterm_state_reset(VTermState *state, ကိန်းပြည့် hard)
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

  အတွက်(ကိန်းပြည့် col = 0; col < state->cols; col++)
    အကယ်၍(col % 8 == 0)
      set_col_tabstop(state, col);
    မဟုတ်ပါက
      clear_col_tabstop(state, col);

  အတွက်(ကိန်းပြည့် row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  အကယ်၍(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  အတွက်(ကိန်းပြည့် i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    အကယ်၍(default_enc->init)
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

  အကယ်၍(hard) {
    state->pos.row = 0;
    state->pos.col = 0;
    state->at_phantom = 0;

    VTermRect rect = { 0, 0, 0, 0 };
    rect.end_row = state->rows;
    rect.end_col =  state->cols;
    erase(state, rect, 0);
  }
}

void vterm_state_get_cursorpos(const VTermState *state, VTermPos *cursorpos)
{
  *cursorpos = state->pos;
}

void vterm_state_get_mousestate(const VTermState *state, VTermMouseState *mousestate)
{
  mousestate->pos.col = state->mouse_col;
  mousestate->pos.row = state->mouse_row;
  mousestate->buttons = state->mouse_buttons;
  mousestate->flags = state->mouse_flags;
}

void vterm_state_set_callbacks(VTermState *state, const VTermStateCallbacks *callbacks, void *user)
{
  အကယ်၍(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    အကယ်၍(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  မဟုတ်ပါက {
    state->callbacks = NULL;
    state->cbdata = NULL;
  }
}

void vterm_state_callbacks_has_premove(VTermState *state)
{
  state->callbacks_has_premove = 1;
}

void *vterm_state_get_cbdata(VTermState *state)
{
  ပြန်ပေး state->cbdata;
}

void vterm_state_set_unrecognised_fallbacks(VTermState *state, const VTermStateFallbacks *fallbacks, void *user)
{
  အကယ်၍(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  မဟုတ်ပါက {
    state->fallbacks = NULL;
    state->fbdata = NULL;
  }
}

void *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  ပြန်ပေး state->fbdata;
}

ကိန်းပြည့် vterm_state_set_termprop(VTermState *state, VTermProp prop, VTermValue *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  အကယ်၍(state->callbacks && state->callbacks->settermprop)
    အကယ်၍(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      ပြန်ပေး 0;

  switch(prop) {
  pak_sa VTERM_PROP_TITLE:
  pak_sa VTERM_PROP_ICONNAME:
  pak_sa VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    ပြန်ပေး 1;
  pak_sa VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    ပြန်ပေး 1;
  pak_sa VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    ပြန်ပေး 1;
  pak_sa VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->number;
    ပြန်ပေး 1;
  pak_sa VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    ပြန်ပေး 1;
  pak_sa VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    အကယ်၍(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    ပြန်ပေး 1;
  pak_sa VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    အကယ်၍(val->number)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    အကယ်၍(val->number == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    အကယ်၍(val->number == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    ပြန်ပေး 1;
  pak_sa VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    ပြန်ပေး 1;

  pak_sa VTERM_N_PROPS:
    ပြန်ပေး 0;
  }

  ပြန်ပေး 0;
}

void vterm_state_focus_in(VTermState *state)
{
  အကယ်၍(state->mode.report_focus)
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

void vterm_state_focus_out(VTermState *state)
{
  အကယ်၍(state->mode.report_focus)
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

const VTermLineInfo *vterm_state_get_lineinfo(const VTermState *state, ကိန်းပြည့် row)
{
  ပြန်ပေး state->lineinfo + row;
}

void vterm_state_set_selection_callbacks(VTermState *state, const VTermSelectionCallbacks *callbacks, void *user,
    var_na *buffer, size_t buflen)
{
  အကယ်၍(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

void vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  အကယ်၍(frag.initial) {
    /* TODO: support sending more than one mask bit */
    static var_na selection_chars[] = "cpqs";
    ကိန်းပြည့် idx;
    အတွက်(idx = 0; idx < 4; idx++)
      အကယ်၍(mask & (1 << idx))
        bha_nga;

    vterm_push_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  အကယ်၍(frag.len) {
    size_t bufcur = 0;
    var_na *buffer = state->selection.buffer;

    uint32_t x = 0;
    ကိန်းပြည့် n = 0;

    အကယ်၍(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    နေစဉ်((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      အကယ်၍(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      အကယ်၍(!frag.len || (state->selection.buflen - bufcur) < 4) {
        အကယ်၍(bufcur)
          vterm_push_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    အကယ်၍(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  အကယ်၍(frag.final) {
    အကယ်၍(state->tmp.selection.sendpartial) {
      ကိန်းပြည့် n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      var_na *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_push_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    မဟုတ်ပါက
      vterm_push_output_sprintf_str(vt, 0, TRUE, "");
  }
}
