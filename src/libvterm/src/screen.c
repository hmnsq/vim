#ضمن "vterm_internal.h"

// vim: set sw=2 :
#ضمن <stdio.h>
#ضمن <string.h>

#ضمن "rect.h"
#ضمن "utf8.h"

#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

#undef DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
تعريفنوع بنية
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned صحيح bold      : 1;
  unsigned صحيح underline : 2;
  unsigned صحيح italic    : 1;
  unsigned صحيح blink     : 1;
  unsigned صحيح reverse   : 1;
  unsigned صحيح conceal   : 1;
  unsigned صحيح strike    : 1;
  unsigned صحيح font      : 4; /* 0 to 9 */
  unsigned صحيح small     : 1;
  unsigned صحيح baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned صحيح protected_cell : 1;
  unsigned صحيح dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned صحيح dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
تعريفنوع بنية
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

بنية VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ثابت VTermScreenCallbacks *callbacks;
  فراغ *cbdata;
  صحيح callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  صحيح pending_scroll_downward, pending_scroll_rightward;

  صحيح rows;
  صحيح cols;

  unsigned صحيح global_reverse : 1;
  unsigned صحيح reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

ساكن فراغ clearcell(ثابت VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ساكن ScreenCell *getcell(ثابت VTermScreen *screen, صحيح row, صحيح col)
{
  𐡄𐡍(row < 0 || row >= screen->rows)
    أرجع عدم;
  𐡄𐡍(col < 0 || col >= screen->cols)
    أرجع عدم;
  𐡄𐡍 (screen->buffer == عدم)
    أرجع عدم;
  أرجع screen->buffer + (screen->cols * row) + col;
}

ساكن ScreenCell *alloc_buffer(VTermScreen *screen, صحيح rows, صحيح cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * rows * cols);

  لكل(صحيح row = 0; row < rows; row++) {
    لكل(صحيح col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  أرجع new_buffer;
}

ساكن فراغ damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  بدل(screen->damage_merge) {
  حالة VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    اكسر;

  حالة VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    𐡄𐡍(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    وإلا 𐡄𐡍(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      أرجع;
    }
    وإلا 𐡄𐡍(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      𐡄𐡍(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      𐡄𐡍(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      أرجع;
    }
    وإلا {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    اكسر;

  حالة VTERM_DAMAGE_SCREEN:
  حالة VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    𐡄𐡍(screen->damaged.start_row == -1)
      screen->damaged = rect;
    وإلا {
      rect_expand(&screen->damaged, &rect);
    }
    أرجع;

  افتراضي:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    أرجع;
  }

  𐡄𐡍(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ساكن فراغ damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

ساكن صحيح putglyph(VTermGlyphInfo *info, VTermPos pos, فراغ *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  𐡄𐡍(!cell)
    أرجع 0;

  صحيح i;
  لكل(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  𐡄𐡍(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  لكل(صحيح col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    𐡄𐡍 (onecell == عدم)
      اكسر;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.start_row = pos.row;
  rect.end_row   = pos.row+1;
  rect.start_col = pos.col;
  rect.end_col   = pos.col+info->width;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن فراغ sb_pushline_from_row(VTermScreen *screen, صحيح row, صحيح continuation)
{
  VTermPos pos;
  pos.row = row;
  لكل(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  𐡄𐡍(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  وإلا
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

ساكن صحيح premove(VTermRect rect, فراغ *user)
{
  VTermScreen *screen = user;

  𐡄𐡍(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    لكل(صحيح row = 0; row < rect.end_row; row++) {
      ثابت VTermLineInfo *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  أرجع 1;
}

ساكن صحيح moverect_internal(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح cols = src.end_col - src.start_col;
  صحيح downward = src.start_row - dest.start_row;

  صحيح init_row, test_row, inc_row;
  𐡄𐡍(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  وإلا {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  لكل(صحيح row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * حجم(ScreenCell));

  أرجع 1;
}

ساكن صحيح moverect_user(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  𐡄𐡍(screen->callbacks && screen->callbacks->moverect) {
    𐡄𐡍(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    𐡄𐡍((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      أرجع 1;
  }

  damagerect(screen, dest);

  أرجع 1;
}

ساكن صحيح erase_internal(VTermRect rect, صحيح selective, فراغ *user)
{
  VTermScreen *screen = user;

  لكل(صحيح row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    ثابت VTermLineInfo *info = vterm_state_get_lineinfo(screen->state, row);

    لكل(صحيح col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      𐡄𐡍 (cell == عدم)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	أرجع 1;
      }
      𐡄𐡍(selective && cell->pen.protected_cell)
        استمر;

      cell->chars[0] = 0;
      cell->pen = (ScreenPen){
        /* Only copy .fg and .bg; leave things like rv in reset state */
        .fg = screen->pen.fg,
        .bg = screen->pen.bg,
      };
      cell->pen.dwl = info->doublewidth;
      cell->pen.dhl = info->doubleheight;
    }
  }

  أرجع 1;
}

ساكن صحيح erase_user(VTermRect rect, صحيح selective UNUSED, فراغ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  أرجع 1;
}

ساكن صحيح erase(VTermRect rect, صحيح selective, فراغ *user)
{
  erase_internal(rect, selective, user);
  أرجع erase_user(rect, 0, user);
}

ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user)
{
  VTermScreen *screen = user;

  𐡄𐡍(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    أرجع 1;
  }

  𐡄𐡍(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  𐡄𐡍(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  وإلا 𐡄𐡍(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  وإلا {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  𐡄𐡍(screen->damaged.start_row == -1)
    أرجع 1;

  𐡄𐡍(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  وإلا 𐡄𐡍(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    𐡄𐡍(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      𐡄𐡍(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      𐡄𐡍(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    𐡄𐡍(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      𐡄𐡍(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      𐡄𐡍(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  وإلا {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  أرجع 1;
}

ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos, صحيح visible, فراغ *user)
{
  VTermScreen *screen = user;

  𐡄𐡍(screen->callbacks && screen->callbacks->movecursor)
    أرجع (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  أرجع 0;
}

ساكن صحيح setpenattr(VTermAttr attr, VTermValue *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->number;
    أرجع 1;
  حالة VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_FONT:
    screen->pen.font = val->number;
    أرجع 1;
  حالة VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    أرجع 1;
  حالة VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    أرجع 1;
  حالة VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    أرجع 1;
  حالة VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->number;
    أرجع 1;

  حالة VTERM_N_ATTRS:
    أرجع 0;
  }

  أرجع 0;
}

ساكن صحيح settermprop(VTermProp prop, VTermValue *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(prop) {
  حالة VTERM_PROP_ALTSCREEN:
    𐡄𐡍(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      أرجع 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    𐡄𐡍(!val->boolean)
      damagescreen(screen);
    اكسر;
  حالة VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    اكسر;
  افتراضي:
    ; /* ignore */
  }

  𐡄𐡍(screen->callbacks && screen->callbacks->settermprop)
    أرجع (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  أرجع 1;
}

ساكن صحيح bell(فراغ *user)
{
  VTermScreen *screen = user;

  𐡄𐡍(screen->callbacks && screen->callbacks->bell)
    أرجع (*screen->callbacks->bell)(screen->cbdata);

  أرجع 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ساكن صحيح line_popcount(ScreenCell *buffer, صحيح row, صحيح rows UNUSED, صحيح cols)
{
  صحيح col = cols - 1;
  طالما(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  أرجع col + 1;
}

#define REFLOW (screen->reflow)

ساكن فراغ resize_buffer(VTermScreen *screen, صحيح bufidx, صحيح new_rows, صحيح new_cols, صحيح active, VTermStateFields *statefields)
{
  صحيح old_rows = screen->rows;
  صحيح old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTermLineInfo *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * new_rows * new_cols);
  VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  صحيح old_row = old_rows - 1;
  صحيح new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  صحيح final_blank_row = new_rows;

  طالما(old_row >= 0) {
    صحيح old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    طالما(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    صحيح old_row_start = old_row;

    صحيح width = 0;
    لكل(صحيح row = old_row_start; row <= old_row_end; row++) {
      𐡄𐡍(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      وإلا
        width += line_popcount(old_buffer, row, old_rows, old_cols);
    }

    𐡄𐡍(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    صحيح new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    صحيح new_row_end = new_row;
    صحيح new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    صحيح old_col = 0;

    صحيح spare_rows = new_rows - final_blank_row;

    𐡄𐡍(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      صحيح downwards = -new_row_start;
      𐡄𐡍(downwards > spare_rows)
        downwards = spare_rows;
      صحيح rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * حجم(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * حجم(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      𐡄𐡍(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    𐡄𐡍(new_row_start < 0) {
      𐡄𐡍(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        𐡄𐡍(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      اكسر;
    }

    لكل(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      صحيح count = width >= new_cols ? new_cols : width;
      width -= count;

      صحيح new_col = 0;

      طالما(count) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        𐡄𐡍(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        𐡄𐡍(old_col == old_cols) {
          old_row++;

          𐡄𐡍(!REFLOW) {
            new_col++;
            اكسر;
          }
          old_col = 0;
        }

        new_col++;
        count--;
      }

      𐡄𐡍(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        𐡄𐡍(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      طالما(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  𐡄𐡍(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    𐡄𐡍(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  𐡄𐡍(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    𐡄𐡍 (new_cursor.row < 0)
      new_cursor.row = 0;
    𐡄𐡍 (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  𐡄𐡍(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    𐡄𐡍((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      لكل(صحيح row = 0; row <= old_row; row++) {
        ثابت VTermLineInfo *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    𐡄𐡍(active)
      statefields->pos.row -= (old_row + 1);
  }
  𐡄𐡍(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    طالما(new_row >= 0) {
      VTermPos pos;
      𐡄𐡍(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        اكسر;

      pos.row = new_row;
      لكل(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          𐡄𐡍(!src->chars[i])
            اكسر;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.reverse   = src->attrs.reverse ^ screen->global_reverse;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        𐡄𐡍(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      لكل( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      𐡄𐡍(active)
        statefields->pos.row++;
    }
  }
  𐡄𐡍(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    صحيح moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * حجم(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * حجم(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    لكل(new_row = moverows; new_row < new_rows; new_row++) {
      لكل(صحيح col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTermLineInfo){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  𐡄𐡍(active)
    statefields->pos = new_cursor;

  أرجع;
}

ساكن صحيح resize(صحيح new_rows, صحيح new_cols, VTermStateFields *fields, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  صحيح old_rows = screen->rows;
  صحيح old_cols = screen->cols;

  𐡄𐡍(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    𐡄𐡍(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    𐡄𐡍 (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_cols);
  }

  𐡄𐡍 (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  𐡄𐡍(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  وإلا 𐡄𐡍(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_rows);
    لكل(صحيح row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTermLineInfo){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  𐡄𐡍(new_cols <= old_cols) {
    𐡄𐡍(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  𐡄𐡍(screen->callbacks && screen->callbacks->resize)
    أرجع (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  أرجع 1;
}

ساكن صحيح setlineinfo(صحيح row, ثابت VTermLineInfo *newinfo, ثابت VTermLineInfo *oldinfo, فراغ *user)
{
  VTermScreen *screen = user;

  𐡄𐡍(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    لكل(صحيح col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      𐡄𐡍 (cell == عدم)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	أرجع 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.start_row = row;
    rect.end_row   = row + 1;
    rect.start_col = 0;
    rect.end_col   = newinfo->doublewidth ? screen->cols / 2 : screen->cols;
    damagerect(screen, rect);

    𐡄𐡍(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  أرجع 1;
}

ساكن صحيح sb_clear(فراغ *user) {
  VTermScreen *screen = user;

  𐡄𐡍(screen->callbacks && screen->callbacks->sb_clear)
    𐡄𐡍((*screen->callbacks->sb_clear)(screen->cbdata))
      أرجع 1;

  أرجع 0;
}

ساكن VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  عدم, // moverect
  &erase, // erase
  عدم, // initpen
  &setpenattr, // setpenattr
  &settermprop, // settermprop
  &bell, // bell
  &resize, // resize
  &setlineinfo, // setlineinfo
  &sb_clear, //sb_clear
  &premove, // premove
};

/*
 * Allocate a new screen and return it.
 * Return NULL when out of memory.
 */
ساكن VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  𐡄𐡍(!state)
    أرجع عدم;

  VTermScreen *screen = vterm_allocator_malloc(vt, حجم(VTermScreen));
  𐡄𐡍 (screen == عدم)
    أرجع عدم;
  صحيح rows, cols;

  vterm_get_size(vt, &rows, &cols);

  screen->vt = vt;
  screen->state = state;

  screen->damage_merge = VTERM_DAMAGE_CELL;
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;

  screen->rows = rows;
  screen->cols = cols;

  screen->global_reverse = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = عدم;
  screen->cbdata    = عدم;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_buffer(screen, rows, cols);

  screen->buffer = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * cols);
  𐡄𐡍 (screen->buffer == عدم || screen->sb_buffer == عدم)
  {
    vterm_screen_free(screen);
    أرجع عدم;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  أرجع screen;
}

INTERNAL فراغ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  𐡄𐡍(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

فراغ vterm_screen_reset(VTermScreen *screen, صحيح hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ساكن size_t _get_chars(ثابت VTermScreen *screen, ثابت صحيح utf8, فراغ *buffer, size_t len, ثابت VTermRect rect)
{
  size_t outpos = 0;
  صحيح padding = 0;

#define PUT(c)                                             \
  𐡄𐡍(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    𐡄𐡍(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (حرف *)buffer + outpos);   \
    وإلا                                                   \
      outpos += thislen;                                   \
  }                                                        \
  وإلا {                                                   \
    𐡄𐡍(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    وإلا                                                   \
      outpos++;                                            \
  }

  لكل(صحيح row = rect.start_row; row < rect.end_row; row++) {
    لكل(صحيح col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      𐡄𐡍 (cell == عدم)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	أرجع 1;
      }
      𐡄𐡍(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      وإلا 𐡄𐡍(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      وإلا {
        طالما(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    𐡄𐡍(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  أرجع outpos;
}

size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect)
{
  أرجع _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(ثابت VTermScreen *screen, حرف *str, size_t len, ثابت VTermRect rect)
{
  أرجع _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
صحيح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  𐡄𐡍(!intcell)
    أرجع 0;

  لكل(صحيح i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    𐡄𐡍(!intcell->chars[i])
      اكسر;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.reverse   = intcell->pen.reverse ^ screen->global_reverse;
  cell->attrs.conceal   = intcell->pen.conceal;
  cell->attrs.strike    = intcell->pen.strike;
  cell->attrs.font      = intcell->pen.font;
  cell->attrs.small     = intcell->pen.small;
  cell->attrs.baseline  = intcell->pen.baseline;

  cell->attrs.dwl = intcell->pen.dwl;
  cell->attrs.dhl = intcell->pen.dhl;

  cell->fg = intcell->pen.fg;
  cell->bg = intcell->pen.bg;

  𐡄𐡍(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    𐡄𐡍(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      𐡄𐡍(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } وإلا 𐡄𐡍(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } وإلا {
        cell->width = 2;
      }
    } وإلا
      cell->width = 1;
  } وإلا {
    𐡄𐡍(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    وإلا
      cell->width = 1;
  }

  أرجع 1;
}

صحيح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  لكل(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    𐡄𐡍(cell->chars[0] != 0)
      أرجع 0;
  }

  أرجع 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  𐡄𐡍(vt->screen)
    أرجع vt->screen;

  vt->screen = screen_new(vt);
  أرجع vt->screen;
}

فراغ vterm_screen_enable_reflow(VTermScreen *screen, صحيح reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

فراغ vterm_screen_enable_altscreen(VTermScreen *screen, صحيح altscreen)
{
  𐡄𐡍(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    صحيح rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

فراغ vterm_screen_set_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, فراغ *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

فراغ *vterm_screen_get_cbdata(VTermScreen *screen)
{
  أرجع screen->cbdata;
}

فراغ vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

فراغ vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, فراغ *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

فراغ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  أرجع vterm_state_get_unrecognised_fbdata(screen->state);
}

فراغ vterm_screen_flush_damage(VTermScreen *screen)
{
  𐡄𐡍(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  𐡄𐡍(screen->damaged.start_row != -1) {
    𐡄𐡍(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

فراغ vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

ساكن صحيح attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  𐡄𐡍((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    أرجع 1;
  𐡄𐡍((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    أرجع 1;

  أرجع 0;
}

صحيح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  𐡄𐡍(extent->start_col < 0)
    extent->start_col = 0;
  𐡄𐡍(extent->end_col < 0)
    extent->end_col = screen->cols;

  صحيح col;

  لكل(col = pos.col - 1; col >= extent->start_col; col--)
    𐡄𐡍(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      اكسر;
  extent->start_col = col + 1;

  لكل(col = pos.col + 1; col < extent->end_col; col++)
    𐡄𐡍(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      اكسر;
  extent->end_col = col - 1;

  أرجع 1;
}

فراغ vterm_screen_convert_color_to_rgb(ثابت VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

ساكن فراغ reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  لكل(صحيح row = 0; row <= screen->rows - 1; row++)
    لكل(صحيح col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      𐡄𐡍(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      𐡄𐡍(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

فراغ vterm_screen_set_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  𐡄𐡍(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  𐡄𐡍(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  𐡄𐡍(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
