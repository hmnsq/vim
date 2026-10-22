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
  إذا(row < 0 || row >= screen->rows)
    إرجاع عدم;
  إذا(col < 0 || col >= screen->cols)
    إرجاع عدم;
  إذا (screen->buffer == عدم)
    إرجاع عدم;
  إرجاع screen->buffer + (screen->cols * row) + col;
}

ساكن ScreenCell *alloc_buffer(VTermScreen *screen, صحيح rows, صحيح cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * rows * cols);

  لكل(صحيح row = 0; row < rows; row++) {
    لكل(صحيح col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  إرجاع new_buffer;
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
    إذا(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    وإلا إذا(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      إرجاع;
    }
    وإلا إذا(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      إذا(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      إذا(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      إرجاع;
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
    إذا(screen->damaged.start_row == -1)
      screen->damaged = rect;
    وإلا {
      rect_expand(&screen->damaged, &rect);
    }
    إرجاع;

  افتراضي:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    إرجاع;
  }

  إذا(screen->callbacks && screen->callbacks->damage)
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

  إذا(!cell)
    إرجاع 0;

  صحيح i;
  لكل(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  إذا(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  لكل(صحيح col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    إذا (onecell == عدم)
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

  إرجاع 1;
}

ساكن فراغ sb_pushline_from_row(VTermScreen *screen, صحيح row, صحيح continuation)
{
  VTermPos pos;
  pos.row = row;
  لكل(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  إذا(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  وإلا
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

ساكن صحيح premove(VTermRect rect, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    لكل(صحيح row = 0; row < rect.end_row; row++) {
      ثابت VTerm_سطر_Info *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  إرجاع 1;
}

ساكن صحيح moverect_internal(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح cols = src.end_col - src.start_col;
  صحيح downward = src.start_row - dest.start_row;

  صحيح init_row, test_row, inc_row;
  إذا(downward < 0) {
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

  إرجاع 1;
}

ساكن صحيح moverect_user(VTermRect dest, VTermRect src, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->moverect) {
    إذا(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    إذا((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      إرجاع 1;
  }

  damagerect(screen, dest);

  إرجاع 1;
}

ساكن صحيح erase_internal(VTermRect rect, صحيح selective, فراغ *user)
{
  VTermScreen *screen = user;

  لكل(صحيح row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    ثابت VTerm_سطر_Info *info = vterm_state_get_lineinfo(screen->state, row);

    لكل(صحيح col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	إرجاع 1;
      }
      إذا(selective && cell->pen.protected_cell)
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

  إرجاع 1;
}

ساكن صحيح erase_user(VTermRect rect, صحيح selective UNUSED, فراغ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  إرجاع 1;
}

ساكن صحيح erase(VTermRect rect, صحيح selective, فراغ *user)
{
  erase_internal(rect, selective, user);
  إرجاع erase_user(rect, 0, user);
}

ساكن صحيح scrollrect(VTermRect rect, صحيح downward, صحيح rightward, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    إرجاع 1;
  }

  إذا(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  إذا(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  وإلا إذا(rect_equal(&screen->pending_scrollrect, &rect) &&
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

  إذا(screen->damaged.start_row == -1)
    إرجاع 1;

  إذا(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  وإلا إذا(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    إذا(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      إذا(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      إذا(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    إذا(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      إذا(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      إذا(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  وإلا {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  إرجاع 1;
}

ساكن صحيح movecursor(VTermPos pos, VTermPos oldpos, صحيح visible, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->movecursor)
    إرجاع (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  إرجاع 0;
}

ساكن صحيح setpenattr(VTermAttr attr, VTerm_قيمة *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(attr) {
  حالة VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    إرجاع 1;
  حالة VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->العدد;
    إرجاع 1;
  حالة VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    إرجاع 1;
  حالة VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    إرجاع 1;
  حالة VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    إرجاع 1;
  حالة VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    إرجاع 1;
  حالة VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    إرجاع 1;
  حالة VTERM_ATTR_FONT:
    screen->pen.font = val->العدد;
    إرجاع 1;
  حالة VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    إرجاع 1;
  حالة VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    إرجاع 1;
  حالة VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    إرجاع 1;
  حالة VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->العدد;
    إرجاع 1;

  حالة VTERM_N_ATTRS:
    إرجاع 0;
  }

  إرجاع 0;
}

ساكن صحيح settermprop(VTermProp prop, VTerm_قيمة *val, فراغ *user)
{
  VTermScreen *screen = user;

  بدل(prop) {
  حالة VTERM_PROP_ALTSCREEN:
    إذا(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      إرجاع 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    إذا(!val->boolean)
      damagescreen(screen);
    اكسر;
  حالة VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    اكسر;
  افتراضي:
    ; /* ignore */
  }

  إذا(screen->callbacks && screen->callbacks->settermprop)
    إرجاع (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  إرجاع 1;
}

ساكن صحيح bell(فراغ *user)
{
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->bell)
    إرجاع (*screen->callbacks->bell)(screen->cbdata);

  إرجاع 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ساكن صحيح سطر_popcount(ScreenCell *buffer, صحيح row, صحيح rows UNUSED, صحيح cols)
{
  صحيح col = cols - 1;
  بينما(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  إرجاع col + 1;
}

#define REFLOW (screen->reflow)

ساكن فراغ resize_buffer(VTermScreen *screen, صحيح bufidx, صحيح new_rows, صحيح new_cols, صحيح active, VTermStateFields *statefields)
{
  صحيح old_rows = screen->rows;
  صحيح old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTerm_سطر_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * new_rows * new_cols);
  VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_rows);

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

  بينما(old_row >= 0) {
    صحيح old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    بينما(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    صحيح old_row_start = old_row;

    صحيح width = 0;
    لكل(صحيح row = old_row_start; row <= old_row_end; row++) {
      إذا(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      وإلا
        width += سطر_popcount(old_buffer, row, old_rows, old_cols);
    }

    إذا(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    صحيح new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    صحيح new_row_end = new_row;
    صحيح new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    صحيح old_col = 0;

    صحيح spare_rows = new_rows - final_blank_row;

    إذا(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      صحيح downwards = -new_row_start;
      إذا(downwards > spare_rows)
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

      إذا(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    إذا(new_row_start < 0) {
      إذا(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        إذا(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      اكسر;
    }

    لكل(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      صحيح تعداد_2 = width >= new_cols ? new_cols : width;
      width -= تعداد_2;

      صحيح new_col = 0;

      بينما(تعداد_2) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        إذا(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        إذا(old_col == old_cols) {
          old_row++;

          إذا(!REFLOW) {
            new_col++;
            اكسر;
          }
          old_col = 0;
        }

        new_col++;
        تعداد_2--;
      }

      إذا(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        إذا(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      بينما(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  إذا(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    إذا(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  إذا(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    إذا (new_cursor.row < 0)
      new_cursor.row = 0;
    إذا (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  إذا(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    إذا((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      لكل(صحيح row = 0; row <= old_row; row++) {
        ثابت VTerm_سطر_Info *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    إذا(active)
      statefields->pos.row -= (old_row + 1);
  }
  إذا(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    بينما(new_row >= 0) {
      VTermPos pos;
      إذا(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        اكسر;

      pos.row = new_row;
      لكل(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          إذا(!src->chars[i])
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

        إذا(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      لكل( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      إذا(active)
        statefields->pos.row++;
    }
  }
  إذا(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    صحيح moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * حجم(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * حجم(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    لكل(new_row = moverows; new_row < new_rows; new_row++) {
      لكل(صحيح col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTerm_سطر_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  إذا(active)
    statefields->pos = new_cursor;

  إرجاع;
}

ساكن صحيح resize(صحيح new_rows, صحيح new_cols, VTermStateFields *fields, فراغ *user)
{
  VTermScreen *screen = user;

  صحيح altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  صحيح old_rows = screen->rows;
  صحيح old_cols = screen->cols;

  إذا(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    إذا(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    إذا (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_cols);
  }

  إذا (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  وإلا إذا(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_rows);
    لكل(صحيح row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTerm_سطر_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  إذا(new_cols <= old_cols) {
    إذا(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  إذا(screen->callbacks && screen->callbacks->resize)
    إرجاع (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  إرجاع 1;
}

ساكن صحيح setlineinfo(صحيح row, ثابت VTerm_سطر_Info *newinfo, ثابت VTerm_سطر_Info *oldinfo, فراغ *user)
{
  VTermScreen *screen = user;

  إذا(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    لكل(صحيح col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	إرجاع 1;
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

    إذا(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  إرجاع 1;
}

ساكن صحيح sb_clear(فراغ *user) {
  VTermScreen *screen = user;

  إذا(screen->callbacks && screen->callbacks->sb_clear)
    إذا((*screen->callbacks->sb_clear)(screen->cbdata))
      إرجاع 1;

  إرجاع 0;
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
  إذا(!state)
    إرجاع عدم;

  VTermScreen *screen = vterm_allocator_malloc(vt, حجم(VTermScreen));
  إذا (screen == عدم)
    إرجاع عدم;
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
  إذا (screen->buffer == عدم || screen->sb_buffer == عدم)
  {
    vterm_screen_free(screen);
    إرجاع عدم;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  إرجاع screen;
}

INTERNAL فراغ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  إذا(screen->buffers[BUFIDX_ALTSCREEN])
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
  إذا(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    إذا(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (حرف *)buffer + outpos);   \
    وإلا                                                   \
      outpos += thislen;                                   \
  }                                                        \
  وإلا {                                                   \
    إذا(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    وإلا                                                   \
      outpos++;                                            \
  }

  لكل(صحيح row = rect.start_row; row < rect.end_row; row++) {
    لكل(صحيح col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      إذا (cell == عدم)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	إرجاع 1;
      }
      إذا(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      وإلا إذا(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      وإلا {
        بينما(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        لكل(صحيح i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    إذا(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  إرجاع outpos;
}

size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect)
{
  إرجاع _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(ثابت VTermScreen *screen, حرف *str, size_t len, ثابت VTermRect rect)
{
  إرجاع _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
صحيح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  إذا(!intcell)
    إرجاع 0;

  لكل(صحيح i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    إذا(!intcell->chars[i])
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

  إذا(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    إذا(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      إذا(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } وإلا إذا(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } وإلا {
        cell->width = 2;
      }
    } وإلا
      cell->width = 1;
  } وإلا {
    إذا(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    وإلا
      cell->width = 1;
  }

  إرجاع 1;
}

صحيح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  لكل(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    إذا(cell->chars[0] != 0)
      إرجاع 0;
  }

  إرجاع 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  إذا(vt->screen)
    إرجاع vt->screen;

  vt->screen = screen_new(vt);
  إرجاع vt->screen;
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
  إذا(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
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
  إرجاع screen->cbdata;
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
  إرجاع vterm_state_get_unrecognised_fbdata(screen->state);
}

فراغ vterm_screen_flush_damage(VTermScreen *screen)
{
  إذا(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  إذا(screen->damaged.start_row != -1) {
    إذا(screen->callbacks && screen->callbacks->damage)
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
  إذا((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    إرجاع 1;
  إذا((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    إرجاع 1;

  إرجاع 0;
}

صحيح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  إذا(extent->start_col < 0)
    extent->start_col = 0;
  إذا(extent->end_col < 0)
    extent->end_col = screen->cols;

  صحيح col;

  لكل(col = pos.col - 1; col >= extent->start_col; col--)
    إذا(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      اكسر;
  extent->start_col = col + 1;

  لكل(col = pos.col + 1; col < extent->end_col; col++)
    إذا(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      اكسر;
  extent->end_col = col - 1;

  إرجاع 1;
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
      إذا(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      إذا(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

فراغ vterm_screen_set_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  إذا(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  إذا(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  إذا(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
