#include "vterm_internal.h"

#include <stdio.h>
#include <string.h>

#define strneq(a,b,n) (strncmp(a,b,n)==0)

#якщо defined(DEBUG) && DEBUG > 1
# define DEBUG_GLYPH_COMBINE
#endif

static ціле on_resize(ціле rows, ціле cols, void *user);

/* Some convenient wrappers to make callback functions easier */

static void putglyph(VTermState *state, const uint32_t chars[], ціле width, VTermPos pos)
{
  VTermGlyphInfo info;

  info.chars = chars;
  info.width = width;
  info.protected_cell = state->protected_cell;
  info.dwl = state->lineinfo[pos.row].doublewidth;
  info.dhl = state->lineinfo[pos.row].doubleheight;

  якщо(state->callbacks && state->callbacks->putglyph)
    якщо((*state->callbacks->putglyph)(&info, pos, state->cbdata))
      повернути;

  DEBUG_LOG3("libvterm: Unhandled putglyph U+%04x at (%d,%d)\n", chars[0], pos.col, pos.row);
}

static void updatecursor(VTermState *state, VTermPos *oldpos, ціле cancel_phantom)
{
  якщо(state->pos.col == oldpos->col && state->pos.row == oldpos->row)
    повернути;

  якщо(cancel_phantom)
    state->at_phantom = 0;

  якщо(state->callbacks && state->callbacks->movecursor)
    якщо((*state->callbacks->movecursor)(state->pos, *oldpos, state->mode.cursor_visible, state->cbdata))
      повернути;
}

static void erase(VTermState *state, VTermRect rect, ціле selective)
{
  якщо(rect.end_col == state->cols) {
    ціле row;
    /* If we're erasing the final cells of any lines, cancel the continuation
     * marker on the subsequent line
     */
    для(row = rect.start_row + 1; row < rect.end_row + 1 && row < state->rows; row++)
      state->lineinfo[row].continuation = 0;
  }

  якщо(state->callbacks && state->callbacks->erase)
    якщо((*state->callbacks->erase)(rect, selective, state->cbdata))
      повернути;
}

static VTermState *vterm_state_new(VTerm *vt)
{
  VTermState *state = vterm_allocator_malloc(vt, sizeof(VTermState));

  якщо (state == NULL)
    повернути NULL;
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
  state->combine_chars = vterm_allocator_malloc(state->vt, state->combine_chars_size * sizeof(state->combine_chars[0]));

  state->tabstops = vterm_allocator_malloc(state->vt, (state->cols + 7) / 8);

  state->lineinfos[BUFIDX_PRIMARY]   = vterm_allocator_malloc(state->vt, state->rows * sizeof(VTermLineInfo));
  /* TODO: Make an 'enable' function */
  state->lineinfos[BUFIDX_ALTSCREEN] = vterm_allocator_malloc(state->vt, state->rows * sizeof(VTermLineInfo));
  state->lineinfo = state->lineinfos[BUFIDX_PRIMARY];

  state->encoding_utf8.enc = vterm_lookup_encoding(ENC_UTF8, 'u');
  якщо(state->encoding_utf8.enc->init)
    (*state->encoding_utf8.enc->init)(state->encoding_utf8.enc, state->encoding_utf8.data);

  повернути state;
}

INTERNAL void vterm_state_free(VTermState *state)
{
  vterm_allocator_free(state->vt, state->tabstops);
  vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_PRIMARY]);
  якщо(state->lineinfos[BUFIDX_ALTSCREEN])
    vterm_allocator_free(state->vt, state->lineinfos[BUFIDX_ALTSCREEN]);
  vterm_allocator_free(state->vt, state->combine_chars);
  vterm_allocator_free(state->vt, state);
}

static void scroll(VTermState *state, VTermRect rect, ціле downward, ціле rightward)
{
  ціле rows;
  ціле cols;
  якщо(!downward && !rightward)
    повернути;

  // A degenerate rectangle makes "rows" or "cols" below negative, which
  // inverts the clamping of "downward" and "rightward" and results in a
  // negative height being passed to memmove().  This happens when a scroll
  // region survives a resize that made the terminal smaller.
  якщо(rect.end_row <= rect.start_row || rect.end_col <= rect.start_col)
    повернути;

  rows = rect.end_row - rect.start_row;
  якщо(downward > rows)
    downward = rows;
  інакше якщо(downward < -rows)
    downward = -rows;

  cols = rect.end_col - rect.start_col;
  якщо(rightward > cols)
    rightward = cols;
  інакше якщо(rightward < -cols)
    rightward = -cols;

  якщо(state->callbacks_has_premove && state->callbacks && state->callbacks->premove) {
    // TODO: technically this logic is wrong if both downward != 0 and rightward != 0

    /* Work out what subsection of the destination area is about to be destroyed */
    якщо(downward > 0)
      /* about to destroy the top */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.start_row + downward,
          .start_col = rect.start_col, .end_col = rect.end_col}, state->cbdata);
    інакше якщо(downward < 0)
      /* about to destroy the bottom */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.end_row + downward, .end_row = rect.end_row,
          .start_col = rect.start_col,          .end_col = rect.end_col}, state->cbdata);

    якщо(rightward > 0)
      /* about to destroy the left */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row, .end_row = rect.end_row,
          .start_col = rect.start_col, .end_col = rect.start_col + rightward}, state->cbdata);
    інакше якщо(rightward < 0)
      /* about to destroy the right */
      (*state->callbacks->premove)((VTermRect){
          .start_row = rect.start_row,           .end_row = rect.end_row,
          .start_col = rect.end_col + rightward, .end_col = rect.end_col}, state->cbdata);
  }

  // Update lineinfo if full line
  якщо(rect.start_col == 0 && rect.end_col == state->cols && rightward == 0) {
    ціле height = rect.end_row - rect.start_row - abs(downward);
    ціле row;
    VTermLineInfo zeroLineInfo = {0x0};

    якщо(downward > 0) {
      memmove(state->lineinfo + rect.start_row,
              state->lineinfo + rect.start_row + downward,
              height * sizeof(state->lineinfo[0]));
      для(row = rect.end_row - downward; row < rect.end_row; row++)
        state->lineinfo[row] = zeroLineInfo;
    }
    інакше {
      memmove(state->lineinfo + rect.start_row - downward,
              state->lineinfo + rect.start_row,
              height * sizeof(state->lineinfo[0]));
      для(row = rect.start_row; row < rect.start_row - downward; row++)
        state->lineinfo[row] = zeroLineInfo;
    }
  }

  якщо(state->callbacks && state->callbacks->scrollrect)
    якщо((*state->callbacks->scrollrect)(rect, downward, rightward, state->cbdata))
      повернути;

  якщо(state->callbacks)
    vterm_scroll_rect(rect, downward, rightward,
        state->callbacks->moverect, state->callbacks->erase, state->cbdata);
}

static void linefeed(VTermState *state)
{
  якщо(state->pos.row == SCROLLREGION_BOTTOM(state) - 1) {
    VTermRect rect;
    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 1, 0);
  }
  інакше якщо(state->pos.row < state->rows-1)
    state->pos.row++;
}

static void grow_combine_buffer(VTermState *state)
{
  size_t    new_size = state->combine_chars_size * 2;
  uint32_t *new_chars = vterm_allocator_malloc(state->vt, new_size * sizeof(new_chars[0]));

  memcpy(new_chars, state->combine_chars, state->combine_chars_size * sizeof(new_chars[0]));

  vterm_allocator_free(state->vt, state->combine_chars);

  state->combine_chars = new_chars;
  state->combine_chars_size = new_size;
}

static void set_col_tabstop(VTermState *state, ціле col)
{
  unsigned char mask = 1 << (col & 7);
  state->tabstops[col >> 3] |= mask;
}

static void clear_col_tabstop(VTermState *state, ціле col)
{
  unsigned char mask = 1 << (col & 7);
  state->tabstops[col >> 3] &= ~mask;
}

static ціле is_col_tabstop(VTermState *state, ціле col)
{
  unsigned char mask = 1 << (col & 7);
  повернути state->tabstops[col >> 3] & mask;
}

static ціле is_cursor_in_scrollregion(const VTermState *state)
{
  якщо(state->pos.row < state->scrollregion_top ||
     state->pos.row >= SCROLLREGION_BOTTOM(state))
    повернути 0;
  якщо(state->pos.col < SCROLLREGION_LEFT(state) ||
     state->pos.col >= SCROLLREGION_RIGHT(state))
    повернути 0;

  повернути 1;
}

static void tab(VTermState *state, ціле count, ціле direction)
{
  поки(count > 0) {
    якщо(direction > 0) {
      якщо(state->pos.col >= THISROWWIDTH(state)-1)
        повернути;

      state->pos.col++;
    }
    інакше якщо(direction < 0) {
      якщо(state->pos.col < 1)
        повернути;

      state->pos.col--;
    }

    якщо(is_col_tabstop(state, state->pos.col))
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

static void set_lineinfo(VTermState *state, ціле row, ціле force, ціле dwl, ціле dhl)
{
  VTermLineInfo info = state->lineinfo[row];

  якщо(dwl == DWL_OFF)
    info.doublewidth = DWL_OFF;
  інакше якщо(dwl == DWL_ON)
    info.doublewidth = DWL_ON;
  // else -1 to ignore

  якщо(dhl == DHL_OFF)
    info.doubleheight = DHL_OFF;
  інакше якщо(dhl == DHL_TOP)
    info.doubleheight = DHL_TOP;
  інакше якщо(dhl == DHL_BOTTOM)
    info.doubleheight = DHL_BOTTOM;

  якщо((state->callbacks &&
      state->callbacks->setlineinfo &&
      (*state->callbacks->setlineinfo)(row, &info, state->lineinfo + row, state->cbdata))
      || force)
    state->lineinfo[row] = info;
}

static ціле on_text(const char bytes[], size_t len, void *user)
{
  VTermState *state = user;
  ціле npoints = 0;
  size_t eaten = 0;
  VTermEncodingInstance *encoding;
  ціле i = 0;

  VTermPos oldpos = state->pos;

  // We'll have at most len codepoints, plus one from a previous incomplete
  // sequence.
  uint32_t *codepoints = (uint32_t *)(state->vt->tmpbuffer);
  size_t maxpoints = (state->vt->tmpbuffer_len) / sizeof(uint32_t);

  encoding =
    state->gsingle_set     ? &state->encoding[state->gsingle_set] :
    !(bytes[eaten] & 0x80) ? &state->encoding[state->gl_set] :
    state->vt->mode.utf8   ? &state->encoding_utf8 :
                             &state->encoding[state->gr_set];
  якщо (encoding->enc == state->encoding_utf8.enc)
    encoding = &state->encoding_utf8;  // Only use one UTF-8 encoding state.

  (*encoding->enc->decode)(encoding->enc, encoding->data,
      codepoints, &npoints, state->gsingle_set ? 1 : (ціле)maxpoints,
      bytes, &eaten, len);

  /* There's a chance an encoding (e.g. UTF-8) hasn't found enough bytes yet
   * for even a single codepoint
   */
  якщо(!npoints)
  {
    повернути (ціле)eaten;
  }

  якщо(state->gsingle_set && npoints)
    state->gsingle_set = 0;

  /* This is a combining char. that needs to be merged with the previous
   * glyph output */
  якщо(vterm_unicode_is_combining(codepoints[i])) {
    /* See if the cursor has moved since */
    якщо(state->pos.row == state->combine_pos.row && state->pos.col == state->combine_pos.col + state->combine_width) {
#ifdef DEBUG_GLYPH_COMBINE
      ціле printpos;
      printf("DEBUG: COMBINING SPLIT GLYPH of chars {");
      для(printpos = 0; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("} + {");
#endif

      /* Find where we need to append these combining chars */
      ціле saved_i = 0;
      поки(state->combine_chars[saved_i])
        saved_i++;

      /* Add extra ones */
      поки(i < npoints && vterm_unicode_is_combining(codepoints[i])) {
        якщо(saved_i >= (ціле)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[saved_i++] = codepoints[i++];
      }
      якщо(saved_i >= (ціле)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[saved_i] = 0;

#ifdef DEBUG_GLYPH_COMBINE
      для(; state->combine_chars[printpos]; printpos++)
        printf("U+%04x ", state->combine_chars[printpos]);
      printf("}\n");
#endif

      /* Now render it */
      putglyph(state, state->combine_chars, state->combine_width, state->combine_pos);
    }
    інакше {
      DEBUG_LOG("libvterm: TODO: Skip over split char+combining\n");
    }
  }

  для(; i < npoints; i++) {
    // Try to find combining characters following this
    ціле glyph_starts = i;
    ціле glyph_ends;
    ціле width = 0;

    для(glyph_ends = i + 1;
        (glyph_ends < npoints) && (glyph_ends < glyph_starts + VTERM_MAX_CHARS_PER_CELL);
        glyph_ends++)
      якщо(!vterm_unicode_is_combining(codepoints[glyph_ends]))
        break;

    uint32_t *chars = vterm_allocator_malloc(state->vt, (VTERM_MAX_CHARS_PER_CELL + 1) * sizeof(uint32_t));
    якщо (chars == NULL)
      break;

    для( ; i < glyph_ends; i++) {
      ціле this_width;
      якщо(vterm_get_special_pty_type() == 2) {
        state->vt->in_backspace -= (state->vt->in_backspace > 0) ? 1 : 0;
        якщо(state->vt->in_backspace == 1)
          codepoints[i] = 0; // codepoints under this condition must be 0
      }
      chars[i - glyph_starts] = codepoints[i];
      this_width = vterm_unicode_width(codepoints[i]);
#ifdef DEBUG
      якщо(this_width < 0) {
        fprintf(stderr, "Text with negative-width codepoint U+%04x\n", codepoints[i]);
        abort();
      }
#endif
      якщо (i == glyph_starts || this_width > width)
	width = this_width;  // TODO: should be += ?
    }

    якщо (width < 0)
      width = 0;

    поки(i < npoints && vterm_unicode_is_combining(codepoints[i]))
      i++;

    chars[glyph_ends - glyph_starts] = 0;
    i--;

#ifdef DEBUG_GLYPH_COMBINE
    ціле printpos;
    printf("DEBUG: COMBINED GLYPH of %d chars {", glyph_ends - glyph_starts);
    для(printpos = 0; printpos < glyph_ends - glyph_starts; printpos++)
      printf("U+%04x ", chars[printpos]);
    printf("}, onscreen width %d\n", width);
#endif

    якщо(state->at_phantom || state->pos.col + width > THISROWWIDTH(state)) {
      linefeed(state);
      state->pos.col = 0;
      state->at_phantom = 0;
      state->lineinfo[state->pos.row].continuation = 1;
    }

    якщо(state->mode.insert) {
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

    якщо(i == npoints - 1) {
      /* End of the buffer. Save the chars in case we have to combine with
       * more on the next call */
      ціле save_i;
      для(save_i = 0; chars[save_i]; save_i++) {
        якщо(save_i >= (ціле)state->combine_chars_size)
          grow_combine_buffer(state);
        state->combine_chars[save_i] = chars[save_i];
      }
      якщо(save_i >= (ціле)state->combine_chars_size)
        grow_combine_buffer(state);
      state->combine_chars[save_i] = 0;
      state->combine_width = width;
      state->combine_pos = state->pos;
    }

    якщо(state->pos.col + width >= THISROWWIDTH(state)) {
      якщо(state->mode.autowrap)
        state->at_phantom = 1;
    }
    інакше {
      state->pos.col += width;
    }
    vterm_allocator_free(state->vt, chars);
  }

  updatecursor(state, &oldpos, 0);

#ifdef DEBUG
  якщо(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after text: (%d,%d)\n",
        state->pos.row, state->pos.col);
    abort();
  }
#endif

  повернути (ціле)eaten;
}

static ціле on_control(unsigned char control, void *user)
{
  VTermState *state = user;

  VTermPos oldpos = state->pos;

  VTermScreenCell cell;

  // Preparing to see the leading byte
  VTermPos leadpos = state->pos;
  leadpos.col -= (leadpos.col >= 2 ? 2 : 0);

  switch(control) {
  case 0x07: // BEL - ECMA-48 8.3.3
    якщо(state->callbacks && state->callbacks->bell)
      (*state->callbacks->bell)(state->cbdata);
    break;

  case 0x08: // BS - ECMA-48 8.3.5
    якщо(state->pos.col > 0)
      state->pos.col--;
    якщо(vterm_get_special_pty_type() == 2) {
      // In 2 cell letters, go back 2 cells
      vterm_screen_get_cell(state->vt->screen, leadpos, &cell);
      якщо(vterm_unicode_width(cell.chars[0]) == 2)
        state->pos.col--;
    }
    break;

  case 0x09: // HT - ECMA-48 8.3.60
    tab(state, 1, +1);
    break;

  case 0x0a: // LF - ECMA-48 8.3.74
  case 0x0b: // VT
  case 0x0c: // FF
    linefeed(state);
    якщо(state->mode.newline)
      state->pos.col = 0;
    break;

  case 0x0d: // CR - ECMA-48 8.3.15
    state->pos.col = 0;
    break;

  case 0x0e: // LS1 - ECMA-48 8.3.76
    state->gl_set = 1;
    break;

  case 0x0f: // LS0 - ECMA-48 8.3.75
    state->gl_set = 0;
    break;

  case 0x84: // IND - DEPRECATED but implemented for completeness
    linefeed(state);
    break;

  case 0x85: // NEL - ECMA-48 8.3.86
    linefeed(state);
    state->pos.col = 0;
    break;

  case 0x88: // HTS - ECMA-48 8.3.62
    set_col_tabstop(state, state->pos.col);
    break;

  case 0x8d: // RI - ECMA-48 8.3.104
    якщо(state->pos.row == state->scrollregion_top) {
      VTermRect rect;
      rect.start_row = state->scrollregion_top;
      rect.end_row   = SCROLLREGION_BOTTOM(state);
      rect.start_col = SCROLLREGION_LEFT(state);
      rect.end_col   = SCROLLREGION_RIGHT(state);

      scroll(state, rect, -1, 0);
    }
    інакше якщо(state->pos.row > 0)
        state->pos.row--;
    break;

  case 0x8e: // SS2 - ECMA-48 8.3.141
    state->gsingle_set = 2;
    break;

  case 0x8f: // SS3 - ECMA-48 8.3.142
    state->gsingle_set = 3;
    break;

  default:
    якщо(state->fallbacks && state->fallbacks->control)
      якщо((*state->fallbacks->control)(control, state->fbdata))
        повернути 1;

    повернути 0;
  }

  updatecursor(state, &oldpos, 1);

#ifdef DEBUG
  якщо(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after Ctrl %02x: (%d,%d)\n",
        control, state->pos.row, state->pos.col);
    abort();
  }
#endif

  повернути 1;
}

static ціле settermprop_bool(VTermState *state, VTermProp prop, ціле v)
{
  VTermValue val;
  val.boolean = v;
  повернути vterm_state_set_termprop(state, prop, &val);
}

static ціле settermprop_int(VTermState *state, VTermProp prop, ціле v)
{
  VTermValue val;
  val.number = v;
  повернути vterm_state_set_termprop(state, prop, &val);
}

static ціле settermprop_string(VTermState *state, VTermProp prop, VTermStringFragment frag)
{
  VTermValue val;

  val.string = frag;
  повернути vterm_state_set_termprop(state, prop, &val);
}

static void savecursor(VTermState *state, ціле save)
{
  якщо(save) {
    state->saved.pos = state->pos;
    state->saved.mode.cursor_visible = state->mode.cursor_visible;
    state->saved.mode.cursor_blink   = state->mode.cursor_blink;
    state->saved.mode.cursor_shape   = state->mode.cursor_shape;

    vterm_state_savepen(state, 1);
  }
  інакше {
    VTermPos oldpos = state->pos;

    state->pos = state->saved.pos;

    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, state->saved.mode.cursor_visible);
    settermprop_bool(state, VTERM_PROP_CURSORBLINK,   state->saved.mode.cursor_blink);
    settermprop_int (state, VTERM_PROP_CURSORSHAPE,   state->saved.mode.cursor_shape);

    vterm_state_savepen(state, 0);

    updatecursor(state, &oldpos, 1);
  }
}

static ціле on_escape(const char *bytes, size_t len, void *user)
{
  VTermState *state = user;

  /* Easier to decode this from the first byte, even though the final
   * byte terminates it
   */
  switch(bytes[0]) {
  case ' ':
    якщо(len != 2)
      повернути 0;

    switch(bytes[1]) {
      case 'F': // S7C1T
        state->vt->mode.ctrl8bit = 0;
        break;

      case 'G': // S8C1T
        state->vt->mode.ctrl8bit = 1;
        break;

      default:
        повернути 0;
    }
    повернути 2;

  case '#':
    якщо(len != 2)
      повернути 0;

    switch(bytes[1]) {
      case '3': // DECDHL top
        якщо(state->mode.leftrightmargin)
          break;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_TOP);
        break;

      case '4': // DECDHL bottom
        якщо(state->mode.leftrightmargin)
          break;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_BOTTOM);
        break;

      case '5': // DECSWL
        якщо(state->mode.leftrightmargin)
          break;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_OFF, DHL_OFF);
        break;

      case '6': // DECDWL
        якщо(state->mode.leftrightmargin)
          break;
        set_lineinfo(state, state->pos.row, NO_FORCE, DWL_ON, DHL_OFF);
        break;

      case '8': // DECALN
      {
        VTermPos pos;
        uint32_t E[] = { 'E', 0 };
        для(pos.row = 0; pos.row < state->rows; pos.row++)
          для(pos.col = 0; pos.col < ROWWIDTH(state, pos.row); pos.col++)
            putglyph(state, E, 1, pos);
        break;
      }

      default:
        повернути 0;
    }
    повернути 2;

  case '(': case ')': case '*': case '+': // SCS
    якщо(len != 2)
      повернути 0;

    {
      ціле setnum = bytes[0] - 0x28;
      VTermEncoding *newenc = vterm_lookup_encoding(ENC_SINGLE_94, bytes[1]);

      якщо(newenc) {
        state->encoding[setnum].enc = newenc;

        якщо(newenc->init)
          (*newenc->init)(newenc, state->encoding[setnum].data);
      }
    }

    повернути 2;

  case '7': // DECSC
    savecursor(state, 1);
    повернути 1;

  case '8': // DECRC
    savecursor(state, 0);
    повернути 1;

  case '<': // Ignored by VT100. Used in VT52 mode to switch up to VT100
    повернути 1;

  case '=': // DECKPAM
    state->mode.keypad = 1;
    повернути 1;

  case '>': // DECKPNM
    state->mode.keypad = 0;
    повернути 1;

  case 'c': // RIS - ECMA-48 8.3.105
  {
    VTermPos oldpos = state->pos;
    vterm_state_reset(state, 1);
    якщо(state->callbacks && state->callbacks->movecursor)
      (*state->callbacks->movecursor)(state->pos, oldpos, state->mode.cursor_visible, state->cbdata);
    повернути 1;
  }

  case 'n': // LS2 - ECMA-48 8.3.78
    state->gl_set = 2;
    повернути 1;

  case 'o': // LS3 - ECMA-48 8.3.80
    state->gl_set = 3;
    повернути 1;

  case '~': // LS1R - ECMA-48 8.3.77
    state->gr_set = 1;
    повернути 1;

  case '}': // LS2R - ECMA-48 8.3.79
    state->gr_set = 2;
    повернути 1;

  case '|': // LS3R - ECMA-48 8.3.81
    state->gr_set = 3;
    повернути 1;

  default:
    повернути 0;
  }
}

static void set_mode(VTermState *state, ціле num, ціле val)
{
  switch(num) {
  case 4: // IRM - ECMA-48 7.2.10
    state->mode.insert = val;
    break;

  case 20: // LNM - ANSI X3.4-1977
    state->mode.newline = val;
    break;

  default:
    DEBUG_LOG1("libvterm: Unknown mode %d\n", num);
    повернути;
  }
}

static void set_dec_mode(VTermState *state, ціле num, ціле val)
{
  switch(num) {
  case 1:
    state->mode.cursor = val;
    break;

  case 5: // DECSCNM - screen mode
    settermprop_bool(state, VTERM_PROP_REVERSE, val);
    break;

  case 6: // DECOM - origin mode
    {
      VTermPos oldpos = state->pos;
      state->mode.origin = val;
      state->pos.row = state->mode.origin ? state->scrollregion_top : 0;
      state->pos.col = state->mode.origin ? SCROLLREGION_LEFT(state) : 0;
      updatecursor(state, &oldpos, 1);
    }
    break;

  case 7:
    state->mode.autowrap = val;
    break;

  case 12:
    settermprop_bool(state, VTERM_PROP_CURSORBLINK, val);
    break;

  case 25:
    settermprop_bool(state, VTERM_PROP_CURSORVISIBLE, val);
    break;

  case 69: // DECVSSM - vertical split screen mode
           // DECLRMM - left/right margin mode
    state->mode.leftrightmargin = val;
    якщо(val) {
      ціле row;

      // Setting DECVSSM must clear doublewidth/doubleheight state of every line
      для(row = 0; row < state->rows; row++)
        set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
    }

    break;

  case 1000:
  case 1002:
  case 1003:
    settermprop_int(state, VTERM_PROP_MOUSE,
        !val          ? VTERM_PROP_MOUSE_NONE  :
        (num == 1000) ? VTERM_PROP_MOUSE_CLICK :
        (num == 1002) ? VTERM_PROP_MOUSE_DRAG  :
                        VTERM_PROP_MOUSE_MOVE);
    break;

  case 1004:
    settermprop_bool(state, VTERM_PROP_FOCUSREPORT, val);
    state->mode.report_focus = val;
    break;

  case 1005:
    state->mouse_protocol = val ? MOUSE_UTF8 : MOUSE_X10;
    break;

  case 1006:
    state->mouse_protocol = val ? MOUSE_SGR : MOUSE_X10;
    break;

  case 1015:
    state->mouse_protocol = val ? MOUSE_RXVT : MOUSE_X10;
    break;

  case 1047:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    break;

  case 1048:
    savecursor(state, val);
    break;

  case 1049:
    settermprop_bool(state, VTERM_PROP_ALTSCREEN, val);
    savecursor(state, val);
    break;

  case 2004:
    state->mode.bracketpaste = val;
    break;

  default:
    DEBUG_LOG1("libvterm: Unknown DEC mode %d\n", num);
    повернути;
  }
}

static void request_dec_mode(VTermState *state, ціле num)
{
  ціле reply;

  switch(num) {
    case 1:
      reply = state->mode.cursor;
      break;

    case 5:
      reply = state->mode.screen;
      break;

    case 6:
      reply = state->mode.origin;
      break;

    case 7:
      reply = state->mode.autowrap;
      break;

    case 12:
      reply = state->mode.cursor_blink;
      break;

    case 25:
      reply = state->mode.cursor_visible;
      break;

    case 69:
      reply = state->mode.leftrightmargin;
      break;

    case 1000:
      reply = state->mouse_flags == MOUSE_WANT_CLICK;
      break;

    case 1002:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_DRAG);
      break;

    case 1003:
      reply = state->mouse_flags == (MOUSE_WANT_CLICK|MOUSE_WANT_MOVE);
      break;

    case 1004:
      reply = state->mode.report_focus;
      break;

    case 1005:
      reply = state->mouse_protocol == MOUSE_UTF8;
      break;

    case 1006:
      reply = state->mouse_protocol == MOUSE_SGR;
      break;

    case 1015:
      reply = state->mouse_protocol == MOUSE_RXVT;
      break;

    case 1047:
      reply = state->mode.alt_screen;
      break;

    case 2004:
      reply = state->mode.bracketpaste;
      break;

    default:
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", num, 0);
      повернути;
  }

  vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%d;%d$y", num, reply ? 1 : 2);
}

static void request_version_string(VTermState *state)
{
  vterm_push_output_sprintf_str(state->vt, C1_DCS, TRUE, ">|libvterm(%d.%d)",
      VTERM_VERSION_MAJOR, VTERM_VERSION_MINOR);
}

static ціле on_csi(const char *leader, const long args[], ціле argcount, const char *intermed, char command, void *user)
{
  VTermState *state = user;
  ціле leader_byte = 0;
  ціле intermed_byte = 0;
  ціле cancel_phantom = 1;
  VTermPos oldpos = state->pos;
  ціле handled = 1;

  // Some temporaries for later code
  ціле count, val;
  ціле row, col;
  VTermRect rect;
  ціле selective;

  якщо(leader && leader[0]) {
    якщо(leader[1]) // longer than 1 char
      повернути 0;

    switch(leader[0]) {
    case '?':
    case '>':
    case '<':
      leader_byte = leader[0];
      break;
    default:
      повернути 0;
    }
  }

  якщо(intermed && intermed[0]) {
    якщо(intermed[1]) // longer than 1 char
      повернути 0;

    switch(intermed[0]) {
    case ' ':
    case '!':
    case '"':
    case '$':
    case '\'':
      intermed_byte = intermed[0];
      break;
    default:
      повернути 0;
    }
  }

  oldpos = state->pos;

#define LBOUND(v,min) якщо((v) < (min)) (v) = (min)
#define UBOUND(v,max) якщо((v) > (max)) (v) = (max)

#define LEADER(l,b) ((l << 8) | b)
#define INTERMED(i,b) ((i << 16) | b)

  switch(intermed_byte << 16 | leader_byte << 8 | command) {
  case 0x40: // ICH - ECMA-48 8.3.64
    count = CSI_ARG_COUNT(args[0]);

    якщо(!is_cursor_in_scrollregion(state))
      break;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    якщо(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_RIGHT(state);
    інакше
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, -count);

    break;

  case 0x41: // CUU - ECMA-48 8.3.22
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row -= count;
    state->at_phantom = 0;
    break;

  case 0x42: // CUD - ECMA-48 8.3.19
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row += count;
    state->at_phantom = 0;
    break;

  case 0x43: // CUF - ECMA-48 8.3.20
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col += count;
    state->at_phantom = 0;
    break;

  case 0x44: // CUB - ECMA-48 8.3.18
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col -= count;
    state->at_phantom = 0;
    break;

  case 0x45: // CNL - ECMA-48 8.3.12
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col = 0;
    state->pos.row += count;
    state->at_phantom = 0;
    break;

  case 0x46: // CPL - ECMA-48 8.3.13
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col = 0;
    state->pos.row -= count;
    state->at_phantom = 0;
    break;

  case 0x47: // CHA - ECMA-48 8.3.9
    val = CSI_ARG_OR(args[0], 1);
    state->pos.col = val-1;
    state->at_phantom = 0;
    break;

  case 0x48: // CUP - ECMA-48 8.3.21
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    якщо(vterm_get_special_pty_type() == 2) {
      // Fix a sequence that is not correct right now
      якщо(state->pos.row == row - 1) {
        ціле cnt, ptr = 0;
        для(cnt = 0; cnt < col - 1; ++cnt) {
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
    якщо(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }
    state->at_phantom = 0;
    break;

  case 0x49: // CHT - ECMA-48 8.3.10
    count = CSI_ARG_COUNT(args[0]);
    tab(state, count, +1);
    break;

  case 0x4a: // ED - ECMA-48 8.3.39
  case LEADER('?', 0x4a): // DECSED - Selective Erase in Display
    selective = (leader_byte == '?');
    switch(CSI_ARG(args[0])) {
    case CSI_ARG_MISSING:
    case 0:
      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
      rect.start_col = state->pos.col; rect.end_col = state->cols;
      якщо(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row + 1; rect.end_row = state->rows;
      rect.start_col = 0;
      для(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      якщо(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      break;

    case 1:
      rect.start_row = 0; rect.end_row = state->pos.row;
      rect.start_col = 0; rect.end_col = state->cols;
      для(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      якщо(rect.end_col > rect.start_col)
        erase(state, rect, selective);

      rect.start_row = state->pos.row; rect.end_row = state->pos.row + 1;
                          rect.end_col = state->pos.col + 1;
      якщо(rect.end_row > rect.start_row)
        erase(state, rect, selective);
      break;

    case 2:
      rect.start_row = 0; rect.end_row = state->rows;
      rect.start_col = 0; rect.end_col = state->cols;
      для(row = rect.start_row; row < rect.end_row; row++)
	set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);
      erase(state, rect, selective);
      break;

    case 3:
      якщо(state->callbacks && state->callbacks->sb_clear)
        якщо((*state->callbacks->sb_clear)(state->cbdata))
          повернути 1;
      break;
    }
    break;

  case 0x4b: // EL - ECMA-48 8.3.41
  case LEADER('?', 0x4b): // DECSEL - Selective Erase in Line
    selective = (leader_byte == '?');
    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;

    switch(CSI_ARG(args[0])) {
    case CSI_ARG_MISSING:
    case 0:
      rect.start_col = state->pos.col; rect.end_col = THISROWWIDTH(state); break;
    case 1:
      rect.start_col = 0; rect.end_col = state->pos.col + 1; break;
    case 2:
      rect.start_col = 0; rect.end_col = THISROWWIDTH(state); break;
    default:
      повернути 0;
    }

    якщо(rect.end_col > rect.start_col)
      erase(state, rect, selective);

    break;

  case 0x4c: // IL - ECMA-48 8.3.67
    count = CSI_ARG_COUNT(args[0]);

    якщо(!is_cursor_in_scrollregion(state))
      break;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, -count, 0);

    break;

  case 0x4d: // DL - ECMA-48 8.3.32
    count = CSI_ARG_COUNT(args[0]);

    якщо(!is_cursor_in_scrollregion(state))
      break;

    rect.start_row = state->pos.row;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, count, 0);

    break;

  case 0x50: // DCH - ECMA-48 8.3.26
    count = CSI_ARG_COUNT(args[0]);

    якщо(!is_cursor_in_scrollregion(state))
      break;

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    якщо(state->mode.leftrightmargin)
      rect.end_col = SCROLLREGION_RIGHT(state);
    інакше
      rect.end_col = THISROWWIDTH(state);

    scroll(state, rect, 0, count);

    break;

  case 0x53: // SU - ECMA-48 8.3.147
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, count, 0);

    break;

  case 0x54: // SD - ECMA-48 8.3.113
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = SCROLLREGION_LEFT(state);
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, -count, 0);

    break;

  case 0x58: // ECH - ECMA-48 8.3.38
    count = CSI_ARG_COUNT(args[0]);

    rect.start_row = state->pos.row;
    rect.end_row   = state->pos.row + 1;
    rect.start_col = state->pos.col;
    rect.end_col   = state->pos.col + count;
    UBOUND(rect.end_col, THISROWWIDTH(state));

    erase(state, rect, 0);
    break;

  case 0x5a: // CBT - ECMA-48 8.3.7
    count = CSI_ARG_COUNT(args[0]);
    tab(state, count, -1);
    break;

  case 0x60: // HPA - ECMA-48 8.3.57
    col = CSI_ARG_OR(args[0], 1);
    state->pos.col = col-1;
    state->at_phantom = 0;
    break;

  case 0x61: // HPR - ECMA-48 8.3.59
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col += count;
    state->at_phantom = 0;
    break;

  case 0x62: { // REP - ECMA-48 8.3.103
    const ціле row_width = THISROWWIDTH(state);

    // ECMA-48 repeats the preceding graphic character; when none was
    // printed yet "combine_width" is zero and the loop below would never
    // advance the cursor.  Ignore the control then, like xterm does.
    якщо(state->combine_width < 1)
      break;

    count = CSI_ARG_COUNT(args[0]);
    col = state->pos.col + count;
    UBOUND(col, row_width);
    поки (state->pos.col < col) {
      putglyph(state, state->combine_chars, state->combine_width, state->pos);
      state->pos.col += state->combine_width;
    }
    якщо (state->pos.col + state->combine_width >= row_width) {
      якщо (state->mode.autowrap) {
        state->at_phantom = 1;
        cancel_phantom = 0;
      }
    }
    break;
  }

  case 0x63: // DA - ECMA-48 8.3.24
    val = CSI_ARG_OR(args[0], 0);
    якщо(val == 0)
      // DEC VT100 response
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?1;2c");
    break;

  case LEADER('>', 0x63): // DEC secondary Device Attributes
    // This returns xterm version number 100.
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, ">%d;%d;%dc", 0, 100, 0);
    break;

  case 0x64: // VPA - ECMA-48 8.3.158
    row = CSI_ARG_OR(args[0], 1);
    state->pos.row = row-1;
    якщо(state->mode.origin)
      state->pos.row += state->scrollregion_top;
    state->at_phantom = 0;
    break;

  case 0x65: // VPR - ECMA-48 8.3.160
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row += count;
    state->at_phantom = 0;
    break;

  case 0x66: // HVP - ECMA-48 8.3.63
    row = CSI_ARG_OR(args[0], 1);
    col = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? 1 : CSI_ARG(args[1]);
    // zero-based
    state->pos.row = row-1;
    state->pos.col = col-1;
    якщо(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }
    state->at_phantom = 0;
    break;

  case 0x67: // TBC - ECMA-48 8.3.154
    val = CSI_ARG_OR(args[0], 0);

    switch(val) {
    case 0:
      clear_col_tabstop(state, state->pos.col);
      break;
    case 3:
    case 5:
      для(col = 0; col < state->cols; col++)
        clear_col_tabstop(state, col);
      break;
    case 1:
    case 2:
    case 4:
      break;
    /* TODO: 1, 2 and 4 aren't meaningful yet without line tab stops */
    default:
      повернути 0;
    }
    break;

  case 0x68: // SM - ECMA-48 8.3.125
    якщо(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 1);
    break;

  case LEADER('?', 0x68): // DEC private mode set
    для(ціле i = 0; i < argcount; i++) {
      якщо(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 1);
    }
    break;

  case 0x6a: // HPB - ECMA-48 8.3.58
    count = CSI_ARG_COUNT(args[0]);
    state->pos.col -= count;
    state->at_phantom = 0;
    break;

  case 0x6b: // VPB - ECMA-48 8.3.159
    count = CSI_ARG_COUNT(args[0]);
    state->pos.row -= count;
    state->at_phantom = 0;
    break;

  case 0x6c: // RM - ECMA-48 8.3.106
    якщо(!CSI_ARG_IS_MISSING(args[0]))
      set_mode(state, CSI_ARG(args[0]), 0);
    break;

  case LEADER('?', 0x6c): // DEC private mode reset
    для(ціле i = 0; i < argcount; i++) {
      якщо(!CSI_ARG_IS_MISSING(args[i]))
        set_dec_mode(state, CSI_ARG(args[i]), 0);
    }
    break;

  case 0x6d: // SGR - ECMA-48 8.3.117
    vterm_state_setpen(state, args, argcount);
    break;

  case LEADER('?', 0x6d): // DECSGR and XTQMODKEYS
    // CSI ? 4 m  XTQMODKEYS: request modifyOtherKeys level
    якщо (argcount == 1 && CSI_ARG(args[0]) == 4)
    {
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, ">4;%dm",
					state->mode.modify_other_keys ? 2 : 0);
      break;
    }

    /* No actual DEC terminal recognised these, but some printers did. These
     * are alternative ways to request subscript/superscript/off
     */
    для(ціле argi = 0; argi < argcount; argi++) {
      long arg;
      switch(arg = CSI_ARG(args[argi])) {
        case 4: // Superscript on
          arg = 73;
          vterm_state_setpen(state, &arg, 1);
          break;
        case 5: // Subscript on
          arg = 74;
          vterm_state_setpen(state, &arg, 1);
          break;
        case 24: // Super+subscript off
          arg = 75;
          vterm_state_setpen(state, &arg, 1);
          break;
      }
    }
    break;

  case LEADER('>', 0x6d): // CSI > 4 ; Pv m   xterm resource modifyOtherKeys
    якщо (argcount == 2 && CSI_ARG(args[0]) == 4)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.kitty_keyboard = 0;

      state->mode.modify_other_keys = CSI_ARG(args[1]) == 2;
    }
    break;

  case LEADER('>', 0x75): // CSI > 1 u  enable kitty keyboard protocol
    якщо (argcount == 1 && CSI_ARG(args[0]) == 1)
    {
      // can't have both modify_other_keys and kitty_keyboard
      state->mode.modify_other_keys = 0;

      state->mode.kitty_keyboard = 1;
    }
    break;

  case LEADER('<', 0x75): // CSI < u  disable kitty keyboard protocol
    якщо (argcount <= 1)
      state->mode.kitty_keyboard = 0;
    break;

  case LEADER('?', 0x75): // CSI ? u  request kitty keyboard protocol state
    якщо (argcount <= 1)
      // TODO: this only uses the values zero and one.  The protocol specifies
      // more values, the progressive enhancement flags.
      vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "?%du",
						   state->mode.kitty_keyboard);
    break;

  case 0x6e: // DSR - ECMA-48 8.3.35
  case LEADER('?', 0x6e): // DECDSR
    val = CSI_ARG_OR(args[0], 0);

    {
      char *qmark = (leader_byte == '?') ? "?" : "";

      switch(val) {
      case 0: case 1: case 2: case 3: case 4:
        // ignore - these are replies
        break;
      case 5:
        vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%s0n", qmark);
        break;
      case 6: // CPR - cursor position report
        vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "%s%d;%dR", qmark, state->pos.row + 1, state->pos.col + 1);
        break;
      }
    }
    break;


  case INTERMED('!', 0x70): // DECSTR - DEC soft terminal reset
    vterm_state_reset(state, 0);
    break;

  case LEADER('?', INTERMED('$', 0x70)):
    request_dec_mode(state, CSI_ARG(args[0]));
    break;

  case LEADER('>', 0x71): // XTVERSION - xterm query version string
    request_version_string(state);
    break;

  case INTERMED(' ', 0x71): // DECSCUSR - DEC set cursor shape
    val = CSI_ARG_OR(args[0], 1);

    switch(val) {
    case 0: case 1:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      break;
    case 2:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BLOCK);
      break;
    case 3:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      break;
    case 4:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_UNDERLINE);
      break;
    case 5:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 1);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_LEFT);
      break;
    case 6:
      settermprop_bool(state, VTERM_PROP_CURSORBLINK, 0);
      settermprop_int (state, VTERM_PROP_CURSORSHAPE, VTERM_PROP_CURSORSHAPE_BAR_LEFT);
      break;
    }

    break;

  case INTERMED('"', 0x71): // DECSCA - DEC select character protection attribute
    val = CSI_ARG_OR(args[0], 0);

    switch(val) {
    case 0: case 2:
      state->protected_cell = 0;
      break;
    case 1:
      state->protected_cell = 1;
      break;
    }

    break;

  case 0x72: // DECSTBM - DEC custom
    state->scrollregion_top = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_bottom = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_top, 0);
    UBOUND(state->scrollregion_top, state->rows);
    LBOUND(state->scrollregion_bottom, -1);
    якщо(state->scrollregion_top == 0 && state->scrollregion_bottom == state->rows)
      state->scrollregion_bottom = -1;
    інакше
      UBOUND(state->scrollregion_bottom, state->rows);

    якщо(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
      // Invalid
      state->scrollregion_top    = 0;
      state->scrollregion_bottom = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    якщо(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }

    break;

  case 0x73: // DECSLRM - DEC custom
    // Always allow setting these margins, just they won't take effect without DECVSSM
    state->scrollregion_left = CSI_ARG_OR(args[0], 1) - 1;
    state->scrollregion_right = argcount < 2 || CSI_ARG_IS_MISSING(args[1]) ? -1 : CSI_ARG(args[1]);
    LBOUND(state->scrollregion_left, 0);
    UBOUND(state->scrollregion_left, state->cols);
    LBOUND(state->scrollregion_right, -1);
    якщо(state->scrollregion_left == 0 && state->scrollregion_right == state->cols)
      state->scrollregion_right = -1;
    інакше
      UBOUND(state->scrollregion_right, state->cols);

    якщо(state->scrollregion_right > -1 &&
       state->scrollregion_right <= state->scrollregion_left) {
      // Invalid
      state->scrollregion_left  = 0;
      state->scrollregion_right = -1;
    }

    // Setting the scrolling region restores the cursor to the home position
    state->pos.row = 0;
    state->pos.col = 0;
    якщо(state->mode.origin) {
      state->pos.row += state->scrollregion_top;
      state->pos.col += SCROLLREGION_LEFT(state);
    }

    break;

  case 0x74:
    switch(CSI_ARG(args[0])) {
      case 8: // CSI 8 ; rows ; cols t  set size
        якщо (argcount == 3 &&
            !CSI_ARG_IS_MISSING(args[1]) && !CSI_ARG_IS_MISSING(args[2]) &&
            CSI_ARG(args[1]) > 0 && CSI_ARG(args[2]) > 0)
	  on_resize(CSI_ARG(args[1]), CSI_ARG(args[2]), state);
	break;
      default:
	handled = 0;
	break;
    }
    break;

  case INTERMED('\'', 0x7D): // DECIC
    count = CSI_ARG_COUNT(args[0]);

    якщо(!is_cursor_in_scrollregion(state))
      break;

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 0, -count);

    break;

  case INTERMED('\'', 0x7E): // DECDC
    count = CSI_ARG_COUNT(args[0]);

    якщо(!is_cursor_in_scrollregion(state))
      break;

    rect.start_row = state->scrollregion_top;
    rect.end_row   = SCROLLREGION_BOTTOM(state);
    rect.start_col = state->pos.col;
    rect.end_col   = SCROLLREGION_RIGHT(state);

    scroll(state, rect, 0, count);

    break;

  default:
    handled = 0;
    break;
  }

  якщо (!handled) {
    якщо(state->fallbacks && state->fallbacks->csi)
      якщо((*state->fallbacks->csi)(leader, args, argcount, intermed, command, state->fbdata))
        повернути 1;

    повернути 0;
  }

  якщо(state->mode.origin) {
    LBOUND(state->pos.row, state->scrollregion_top);
    UBOUND(state->pos.row, SCROLLREGION_BOTTOM(state)-1);
    LBOUND(state->pos.col, SCROLLREGION_LEFT(state));
    UBOUND(state->pos.col, SCROLLREGION_RIGHT(state)-1);
  }
  інакше {
    LBOUND(state->pos.row, 0);
    UBOUND(state->pos.row, state->rows-1);
    LBOUND(state->pos.col, 0);
    UBOUND(state->pos.col, THISROWWIDTH(state)-1);
  }

  updatecursor(state, &oldpos, cancel_phantom);

#ifdef DEBUG
  якщо(state->pos.row < 0 || state->pos.row >= state->rows ||
     state->pos.col < 0 || state->pos.col >= state->cols) {
    fprintf(stderr, "Position out of bounds after CSI %c: (%d,%d)\n",
        command, state->pos.row, state->pos.col);
    abort();
  }

  якщо(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
    fprintf(stderr, "Scroll region height out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_BOTTOM(state), state->scrollregion_top);
    abort();
  }

  якщо(SCROLLREGION_RIGHT(state) <= SCROLLREGION_LEFT(state)) {
    fprintf(stderr, "Scroll region width out of bounds after CSI %c: %d <= %d\n",
        command, SCROLLREGION_RIGHT(state), SCROLLREGION_LEFT(state));
    abort();
  }
#endif

  повернути 1;
}

static char base64_one(uint8_t b)
{
  якщо(b < 26)
    повернути 'A' + b;
  інакше якщо(b < 52)
    повернути 'a' + b - 26;
  інакше якщо(b < 62)
    повернути '0' + b - 52;
  інакше якщо(b == 62)
    повернути '+';
  інакше якщо(b == 63)
    повернути '/';
  повернути 0;
}

static uint8_t unbase64one(char c)
{
  якщо(c >= 'A' && c <= 'Z')
    повернути c - 'A';
  інакше якщо(c >= 'a' && c <= 'z')
    повернути c - 'a' + 26;
  інакше якщо(c >= '0' && c <= '9')
    повернути c - '0' + 52;
  інакше якщо(c == '+')
    повернути 62;
  інакше якщо(c == '/')
    повернути 63;

  повернути 0xFF;
}

static void osc_selection(VTermState *state, VTermStringFragment frag)
{
  якщо(frag.initial) {
    state->tmp.selection.mask = 0;
    state->tmp.selection.state = SELECTION_INITIAL;
  }

  поки(!state->tmp.selection.state && frag.len) {
    /* Parse selection parameter */
    switch(frag.str[0]) {
      case 'c':
        state->tmp.selection.mask |= VTERM_SELECTION_CLIPBOARD;
        break;
      case 'p':
        state->tmp.selection.mask |= VTERM_SELECTION_PRIMARY;
        break;
      case 'q':
        state->tmp.selection.mask |= VTERM_SELECTION_SECONDARY;
        break;
      case 's':
        state->tmp.selection.mask |= VTERM_SELECTION_SELECT;
        break;
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
        state->tmp.selection.mask |= (VTERM_SELECTION_CUT0 << (frag.str[0] - '0'));
        break;

      case ';':
        state->tmp.selection.state = SELECTION_SELECTED;
        якщо(!state->tmp.selection.mask)
          state->tmp.selection.mask = VTERM_SELECTION_SELECT|VTERM_SELECTION_CUT0;
        break;
    }

    frag.str++;
    frag.len--;
  }

  якщо(!frag.len) {
    /* Clear selection if we're already finished but didn't do anything */
    якщо(frag.final && state->selection.callbacks->set) {
      (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
              .str     = NULL,
              .len     = 0,
              .initial = state->tmp.selection.state != SELECTION_SET,
              .final   = TRUE,
            }, state->selection.user);
    }
    повернути;
  }

  якщо(state->tmp.selection.state == SELECTION_SELECTED) {
    якщо(frag.str[0] == '?') {
      state->tmp.selection.state = SELECTION_QUERY;
    }
    інакше {
      state->tmp.selection.state = SELECTION_SET_INITIAL;
      state->tmp.selection.recvpartial = 0;
    }
  }

  якщо(state->tmp.selection.state == SELECTION_QUERY) {
    якщо(state->selection.callbacks->query)
      (*state->selection.callbacks->query)(state->tmp.selection.mask, state->selection.user);
    повернути;
  }

  якщо(state->tmp.selection.state == SELECTION_INVALID)
    повернути;

  якщо(state->selection.callbacks->set) {
    size_t bufcur = 0;
    char *buffer = state->selection.buffer;

    uint32_t x = 0; /* Current decoding value */
    ціле n = 0;      /* Number of sextets consumed */

    якщо(state->tmp.selection.recvpartial) {
      n = state->tmp.selection.recvpartial >> 24;
      x = state->tmp.selection.recvpartial & 0x03FFFF; /* could be up to 18 bits of state in here */

      state->tmp.selection.recvpartial = 0;
    }

    поки((state->selection.buflen - bufcur) >= 3 && frag.len) {
      якщо(frag.str[0] == '=') {
        якщо(n == 2) {
          buffer[0] = (x >> 4) & 0xFF;
          buffer += 1, bufcur += 1;
        }
        якщо(n == 3) {
          buffer[0] = (x >> 10) & 0xFF;
          buffer[1] = (x >>  2) & 0xFF;
          buffer += 2, bufcur += 2;
        }

        поки(frag.len && frag.str[0] == '=')
          frag.str++, frag.len--;

        n = 0;
      }
      інакше {
        uint8_t b = unbase64one(frag.str[0]);
        якщо(b == 0xFF) {
          DEBUG_LOG1("base64decode bad input %02X\n", (uint8_t)frag.str[0]);

          state->tmp.selection.state = SELECTION_INVALID;
          якщо(state->selection.callbacks->set) {
            (*state->selection.callbacks->set)(state->tmp.selection.mask, (VTermStringFragment){
                .str     = NULL,
                .len     = 0,
                .initial = TRUE,
                .final   = TRUE,
                }, state->selection.user);
          }
          break;
        }

        x = (x << 6) | b;
        n++;
        frag.str++, frag.len--;

        якщо(n == 4) {
          buffer[0] = (x >> 16) & 0xFF;
          buffer[1] = (x >>  8) & 0xFF;
          buffer[2] = (x >>  0) & 0xFF;

          buffer += 3, bufcur += 3;
          x = 0;
          n = 0;
        }
      }

      якщо(!frag.len || (state->selection.buflen - bufcur) < 3) {
        якщо(bufcur) {
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

    якщо(n)
      state->tmp.selection.recvpartial = (n << 24) | x;
  }
}

static ціле on_osc(ціле command, VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  switch(command) {
    case 0:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      повернути 1;

    case 1:
      settermprop_string(state, VTERM_PROP_ICONNAME, frag);
      повернути 1;

    case 2:
      settermprop_string(state, VTERM_PROP_TITLE, frag);
      повернути 1;

    case 10:
      {
        // request foreground color: <Esc>]10;?<0x07>
        ціле red = state->default_fg.red;
        ціле blue = state->default_fg.blue;
        ціле green = state->default_fg.green;
        vterm_push_output_sprintf_ctrl(state->vt, C1_OSC, "10;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
        повернути 1;
      }

    case 11:
      {
	// request background color: <Esc>]11;?<0x07>
	ціле red = state->default_bg.red;
	ціле blue = state->default_bg.blue;
	ціле green = state->default_bg.green;
	vterm_push_output_sprintf_ctrl(state->vt, C1_OSC, "11;rgb:%02x%02x/%02x%02x/%02x%02x\x07", red, red, green, green, blue, blue);
	повернути 1;
      }
    case 12:
      settermprop_string(state, VTERM_PROP_CURSORCOLOR, frag);
      повернути 1;

    case 52:
      якщо(state->selection.callbacks)
        osc_selection(state, frag);

      повернути 1;

    default:
      якщо(state->fallbacks && state->fallbacks->osc)
        якщо((*state->fallbacks->osc)(command, frag, state->fbdata))
          повернути 1;
  }

  повернути 0;
}

static void request_status_string(VTermState *state, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  char *tmp = state->tmp.decrqss;

  якщо(frag.initial)
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;

  size_t i = 0;
  поки(i < sizeof(state->tmp.decrqss)-1 && tmp[i])
    i++;
  поки(i < sizeof(state->tmp.decrqss)-1 && frag.len--)
    tmp[i++] = (frag.str++)[0];
  tmp[i] = 0;

  якщо(!frag.final)
    повернути;

  switch(tmp[0] | tmp[1]<<8 | tmp[2]<<16) {
    case 'm': {
      // Query SGR
      long args[20];
      ціле argc = vterm_state_getpen(state, args, sizeof(args)/sizeof(args[0]));
      size_t cur = 0;

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "\x90" "1$r" : ESC_S "P" "1$r"); // DCS 1$r ...
      якщо(cur >= vt->tmpbuffer_len)
        повернути;

      для(ціле argi = 0; argi < argc; argi++) {
        cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
            argi == argc - 1             ? "%ld" :
            CSI_ARG_HAS_MORE(args[argi]) ? "%ld:" :
                                           "%ld;",
            CSI_ARG(args[argi]));
        якщо(cur >= vt->tmpbuffer_len)
          повернути;
      }

      cur += SNPRINTF(vt->tmpbuffer + cur, vt->tmpbuffer_len - cur,
          vt->mode.ctrl8bit ? "m" "\x9C" : "m" ESC_S "\\"); // ... m ST
      якщо(cur >= vt->tmpbuffer_len)
        повернути;

      vterm_push_output_bytes(vt, vt->tmpbuffer, cur);
      повернути;
    }

    case 'r':
      // Query DECSTBM
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%dr", state->scrollregion_top+1, SCROLLREGION_BOTTOM(state));
      повернути;

    case 's':
      // Query DECSLRM
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d;%ds", SCROLLREGION_LEFT(state)+1, SCROLLREGION_RIGHT(state));
      повернути;

    case ' '|('q'<<8): {
      // Query DECSCUSR
      ціле reply;
      switch(state->mode.cursor_shape) {
        case VTERM_PROP_CURSORSHAPE_BLOCK:     reply = 2; break;
        case VTERM_PROP_CURSORSHAPE_UNDERLINE: reply = 4; break;
	default: /* VTERM_PROP_CURSORSHAPE_BAR_LEFT */ reply = 6; break;
      }
      якщо(state->mode.cursor_blink)
        reply--;
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d q", reply);
      повернути;
    }

    case '\"'|('q'<<8):
      // Query DECSCA
      vterm_push_output_sprintf_str(vt, C1_DCS, TRUE,
          "1$r%d\"q", state->protected_cell ? 1 : 2);
      повернути;
  }

  vterm_push_output_sprintf_str(state->vt, C1_DCS, TRUE, "0$r");
}

static ціле on_dcs(const char *command, size_t commandlen, VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  якщо(commandlen == 2 && strneq(command, "$q", 2)) {
    request_status_string(state, frag);
    повернути 1;
  }
  інакше якщо(state->fallbacks && state->fallbacks->dcs)
    якщо((*state->fallbacks->dcs)(command, commandlen, frag, state->fbdata))
      повернути 1;

  DEBUG_LOG2("libvterm: Unhandled DCS %.*s\n", (ціле)commandlen, command);
  повернути 0;
}

static ціле on_apc(VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  якщо(state->fallbacks && state->fallbacks->apc)
    якщо((*state->fallbacks->apc)(frag, state->fbdata))
      повернути 1;

  /* No DEBUG_LOG because all APCs are unhandled */
  повернути 0;
}

static ціле on_pm(VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  якщо(state->fallbacks && state->fallbacks->pm)
    якщо((*state->fallbacks->pm)(frag, state->fbdata))
      повернути 1;

  /* No DEBUG_LOG because all PMs are unhandled */
  повернути 0;
}

static ціле on_sos(VTermStringFragment frag, void *user)
{
  VTermState *state = user;

  якщо(state->fallbacks && state->fallbacks->sos)
    якщо((*state->fallbacks->sos)(frag, state->fbdata))
      повернути 1;

  /* No DEBUG_LOG because all SOSs are unhandled */
  повернути 0;
}

static ціле on_resize(ціле rows, ціле cols, void *user)
{
  VTermState *state = user;
  VTermPos oldpos = state->pos;

  якщо (cols > VTERM_MAX_COLS)
    cols = VTERM_MAX_COLS;

  якщо (rows > VTERM_MAX_ROWS)
    rows = VTERM_MAX_ROWS;

  якщо(cols != state->cols) {
    unsigned char *newtabstops = vterm_allocator_malloc(state->vt, (cols + 7) / 8);
    якщо (newtabstops == NULL)
      повернути 0;

    /* TODO: This can all be done much more efficiently bytewise */
    ціле col;
    для(col = 0; col < state->cols && col < cols; col++) {
      unsigned char mask = 1 << (col & 7);
      якщо(state->tabstops[col >> 3] & mask)
        newtabstops[col >> 3] |= mask;
      інакше
        newtabstops[col >> 3] &= ~mask;
      }

    для( ; col < cols; col++) {
      unsigned char mask = 1 << (col & 7);
      якщо(col % 8 == 0)
        newtabstops[col >> 3] |= mask;
      інакше
        newtabstops[col >> 3] &= ~mask;
    }

    vterm_allocator_free(state->vt, state->tabstops);
    state->tabstops = newtabstops;
  }

  state->rows = rows;
  state->cols = cols;

  якщо(state->scrollregion_bottom > -1)
    UBOUND(state->scrollregion_bottom, state->rows);
  якщо(state->scrollregion_right > -1)
    UBOUND(state->scrollregion_right, state->cols);

  // The near edges need clamping as well, otherwise a scroll region that
  // was set before the terminal was made smaller can start past the last
  // row or column.  Drop a region that no longer makes sense, just like
  // DECSTBM and DECSLRM do when it is set.
  UBOUND(state->scrollregion_top, state->rows);
  UBOUND(state->scrollregion_left, state->cols);
  якщо(SCROLLREGION_BOTTOM(state) <= state->scrollregion_top) {
    state->scrollregion_top = 0;
    state->scrollregion_bottom = -1;
  }
  якщо(SCROLLREGION_RIGHT(state) <= state->scrollregion_left) {
    state->scrollregion_left = 0;
    state->scrollregion_right = -1;
  }

  VTermStateFields fields;
  fields.pos = state->pos;
  fields.lineinfos[0] = state->lineinfos[0];
  fields.lineinfos[1] = state->lineinfos[1];

  якщо(state->callbacks && state->callbacks->resize) {
    (*state->callbacks->resize)(rows, cols, &fields, state->cbdata);
    state->pos = fields.pos;

    state->lineinfos[0] = fields.lineinfos[0];
    state->lineinfos[1] = fields.lineinfos[1];
  }
  інакше {
    якщо(rows != state->rows) {
      для(ціле bufidx = BUFIDX_PRIMARY; bufidx <= BUFIDX_ALTSCREEN; bufidx++) {
        VTermLineInfo *oldlineinfo = state->lineinfos[bufidx];
        якщо(!oldlineinfo)
          continue;

        VTermLineInfo *newlineinfo = vterm_allocator_malloc(state->vt, rows * sizeof(VTermLineInfo));

        ціле row;
        для(row = 0; row < state->rows && row < rows; row++) {
          newlineinfo[row] = oldlineinfo[row];
        }

        для( ; row < rows; row++) {
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

  якщо(state->at_phantom && state->pos.col < cols-1) {
    state->at_phantom = 0;
    state->pos.col++;
  }

  якщо(state->pos.row < 0)
    state->pos.row = 0;
  якщо(state->pos.row >= rows)
    state->pos.row = rows - 1;
  якщо(state->pos.col < 0)
    state->pos.col = 0;
  якщо(state->pos.col >= cols)
    state->pos.col = cols - 1;

  updatecursor(state, &oldpos, 1);

  повернути 1;
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
  якщо(vt->state)
    повернути vt->state;

  VTermState *state = vterm_state_new(vt);
  якщо (state == NULL)
    повернути NULL;
  vt->state = state;

  vterm_parser_set_callbacks(vt, &parser_callbacks, state);

  повернути state;
}

void vterm_state_reset(VTermState *state, ціле hard)
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

  для(ціле col = 0; col < state->cols; col++)
    якщо(col % 8 == 0)
      set_col_tabstop(state, col);
    інакше
      clear_col_tabstop(state, col);

  для(ціле row = 0; row < state->rows; row++)
    set_lineinfo(state, row, FORCE, DWL_OFF, DHL_OFF);

  якщо(state->callbacks && state->callbacks->initpen)
    (*state->callbacks->initpen)(state->cbdata);

  vterm_state_resetpen(state);

  VTermEncoding *default_enc = state->vt->mode.utf8 ?
      vterm_lookup_encoding(ENC_UTF8,      'u') :
      vterm_lookup_encoding(ENC_SINGLE_94, 'B');

  для(ціле i = 0; i < 4; i++) {
    state->encoding[i].enc = default_enc;
    якщо(default_enc->init)
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

  якщо(hard) {
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
  якщо(callbacks) {
    state->callbacks = callbacks;
    state->cbdata = user;

    якщо(state->callbacks && state->callbacks->initpen)
      (*state->callbacks->initpen)(state->cbdata);
  }
  інакше {
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
  повернути state->cbdata;
}

void vterm_state_set_unrecognised_fallbacks(VTermState *state, const VTermStateFallbacks *fallbacks, void *user)
{
  якщо(fallbacks) {
    state->fallbacks = fallbacks;
    state->fbdata = user;
  }
  інакше {
    state->fallbacks = NULL;
    state->fbdata = NULL;
  }
}

void *vterm_state_get_unrecognised_fbdata(VTermState *state)
{
  повернути state->fbdata;
}

ціле vterm_state_set_termprop(VTermState *state, VTermProp prop, VTermValue *val)
{
  /* Only store the new value of the property if usercode said it was happy.
   * This is especially important for altscreen switching */
  якщо(state->callbacks && state->callbacks->settermprop)
    якщо(!(*state->callbacks->settermprop)(prop, val, state->cbdata))
      повернути 0;

  switch(prop) {
  case VTERM_PROP_TITLE:
  case VTERM_PROP_ICONNAME:
  case VTERM_PROP_CURSORCOLOR:
    // we don't store these, just transparently pass through
    повернути 1;
  case VTERM_PROP_CURSORVISIBLE:
    state->mode.cursor_visible = val->boolean;
    повернути 1;
  case VTERM_PROP_CURSORBLINK:
    state->mode.cursor_blink = val->boolean;
    повернути 1;
  case VTERM_PROP_CURSORSHAPE:
    state->mode.cursor_shape = val->number;
    повернути 1;
  case VTERM_PROP_REVERSE:
    state->mode.screen = val->boolean;
    повернути 1;
  case VTERM_PROP_ALTSCREEN:
    state->mode.alt_screen = val->boolean;
    state->lineinfo = state->lineinfos[state->mode.alt_screen ? BUFIDX_ALTSCREEN : BUFIDX_PRIMARY];
    якщо(state->mode.alt_screen) {
      VTermRect rect = {0, 0, 0, 0};
      rect.end_row = state->rows;
      rect.end_col = state->cols;
      erase(state, rect, 0);
    }
    повернути 1;
  case VTERM_PROP_MOUSE:
    state->mouse_flags = 0;
    якщо(val->number)
      state->mouse_flags |= MOUSE_WANT_CLICK;
    якщо(val->number == VTERM_PROP_MOUSE_DRAG)
      state->mouse_flags |= MOUSE_WANT_DRAG;
    якщо(val->number == VTERM_PROP_MOUSE_MOVE)
      state->mouse_flags |= MOUSE_WANT_MOVE;
    повернути 1;
  case VTERM_PROP_FOCUSREPORT:
    state->mode.report_focus = val->boolean;
    повернути 1;

  case VTERM_N_PROPS:
    повернути 0;
  }

  повернути 0;
}

void vterm_state_focus_in(VTermState *state)
{
  якщо(state->mode.report_focus)
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "I");
}

void vterm_state_focus_out(VTermState *state)
{
  якщо(state->mode.report_focus)
    vterm_push_output_sprintf_ctrl(state->vt, C1_CSI, "O");
}

const VTermLineInfo *vterm_state_get_lineinfo(const VTermState *state, ціле row)
{
  повернути state->lineinfo + row;
}

void vterm_state_set_selection_callbacks(VTermState *state, const VTermSelectionCallbacks *callbacks, void *user,
    char *buffer, size_t buflen)
{
  якщо(buflen && !buffer)
    buffer = vterm_allocator_malloc(state->vt, buflen);

  state->selection.callbacks = callbacks;
  state->selection.user      = user;
  state->selection.buffer    = buffer;
  state->selection.buflen    = buflen;
}

void vterm_state_send_selection(VTermState *state, VTermSelectionMask mask, VTermStringFragment frag)
{
  VTerm *vt = state->vt;

  якщо(frag.initial) {
    /* TODO: support sending more than one mask bit */
    static char selection_chars[] = "cpqs";
    ціле idx;
    для(idx = 0; idx < 4; idx++)
      якщо(mask & (1 << idx))
        break;

    vterm_push_output_sprintf_str(vt, C1_OSC, FALSE, "52;%c;", selection_chars[idx]);

    state->tmp.selection.sendpartial = 0;
  }

  якщо(frag.len) {
    size_t bufcur = 0;
    char *buffer = state->selection.buffer;

    uint32_t x = 0;
    ціле n = 0;

    якщо(state->tmp.selection.sendpartial) {
      n = state->tmp.selection.sendpartial >> 24;
      x = state->tmp.selection.sendpartial & 0xFFFFFF;

      state->tmp.selection.sendpartial = 0;
    }

    поки((state->selection.buflen - bufcur) >= 4 && frag.len) {
      x = (x << 8) | frag.str[0];
      n++;
      frag.str++, frag.len--;

      якщо(n == 3) {
        buffer[0] = base64_one((x >> 18) & 0x3F);
        buffer[1] = base64_one((x >> 12) & 0x3F);
        buffer[2] = base64_one((x >>  6) & 0x3F);
        buffer[3] = base64_one((x >>  0) & 0x3F);

        buffer += 4, bufcur += 4;
        x = 0;
        n = 0;
      }

      якщо(!frag.len || (state->selection.buflen - bufcur) < 4) {
        якщо(bufcur)
          vterm_push_output_bytes(vt, state->selection.buffer, bufcur);

        buffer = state->selection.buffer;
        bufcur = 0;
      }
    }

    якщо(n)
      state->tmp.selection.sendpartial = (n << 24) | x;
  }

  якщо(frag.final) {
    якщо(state->tmp.selection.sendpartial) {
      ціле n      = state->tmp.selection.sendpartial >> 24;
      uint32_t x = state->tmp.selection.sendpartial & 0xFFFFFF;
      char *buffer = state->selection.buffer;

      /* n is either 1 or 2 now */
      x <<= (n == 1) ? 16 : 8;

      buffer[0] = base64_one((x >> 18) & 0x3F);
      buffer[1] = base64_one((x >> 12) & 0x3F);
      buffer[2] = (n == 1) ? '=' : base64_one((x >>  6) & 0x3F);
      buffer[3] = '=';

      vterm_push_output_sprintf_str(vt, 0, TRUE, "%.*s", 4, buffer);
    }
    інакше
      vterm_push_output_sprintf_str(vt, 0, TRUE, "");
  }
}
