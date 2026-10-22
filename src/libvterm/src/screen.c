#شامل "vterm_internal.h"

// vim: set sw=2 :
#شامل <stdio.h>
#شامل <string.h>

#شامل "rect.h"
#شامل "utf8.h"

#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

#undef DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
تعریفنوع ساختار
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned صحیح bold      : 1;
  unsigned صحیح underline : 2;
  unsigned صحیح italic    : 1;
  unsigned صحیح blink     : 1;
  unsigned صحیح reverse   : 1;
  unsigned صحیح conceal   : 1;
  unsigned صحیح strike    : 1;
  unsigned صحیح font      : 4; /* 0 to 9 */
  unsigned صحیح small     : 1;
  unsigned صحیح baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned صحیح protected_cell : 1;
  unsigned صحیح dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned صحیح dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
تعریفنوع ساختار
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

ساختار VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ثابت VTermScreenCallbacks *callbacks;
  تهی *cbdata;
  صحیح callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  صحیح pending_scroll_downward, pending_scroll_rightward;

  صحیح rows;
  صحیح cols;

  unsigned صحیح global_reverse : 1;
  unsigned صحیح reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

ایستا تهی clearcell(ثابت VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ایستا ScreenCell *getcell(ثابت VTermScreen *screen, صحیح row, صحیح col)
{
  اگر(row < 0 || row >= screen->rows)
    بازگشت پوچ;
  اگر(col < 0 || col >= screen->cols)
    بازگشت پوچ;
  اگر (screen->buffer == پوچ)
    بازگشت پوچ;
  بازگشت screen->buffer + (screen->cols * row) + col;
}

ایستا ScreenCell *alloc_buffer(VTermScreen *screen, صحیح rows, صحیح cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, اندازه(ScreenCell) * rows * cols);

  برای(صحیح row = 0; row < rows; row++) {
    برای(صحیح col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  بازگشت new_buffer;
}

ایستا تهی damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  انتخاب(screen->damage_merge) {
  حالت VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    بشکن;

  حالت VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    اگر(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    وگرنه اگر(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      بازگشت;
    }
    وگرنه اگر(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      اگر(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      اگر(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      بازگشت;
    }
    وگرنه {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    بشکن;

  حالت VTERM_DAMAGE_SCREEN:
  حالت VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    اگر(screen->damaged.start_row == -1)
      screen->damaged = rect;
    وگرنه {
      rect_expand(&screen->damaged, &rect);
    }
    بازگشت;

  پیشفرض:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    بازگشت;
  }

  اگر(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ایستا تهی damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

ایستا صحیح putglyph(VTermGlyphInfo *info, VTermPos pos, تهی *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  اگر(!cell)
    بازگشت 0;

  صحیح i;
  برای(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  اگر(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  برای(صحیح col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    اگر (onecell == پوچ)
      بشکن;
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

  بازگشت 1;
}

ایستا تهی sb_pushline_from_row(VTermScreen *screen, صحیح row, صحیح continuation)
{
  VTermPos pos;
  pos.row = row;
  برای(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  اگر(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  وگرنه
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

ایستا صحیح premove(VTermRect rect, تهی *user)
{
  VTermScreen *screen = user;

  اگر(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    برای(صحیح row = 0; row < rect.end_row; row++) {
      ثابت VTerm_سطر_Info *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  بازگشت 1;
}

ایستا صحیح moverect_internal(VTermRect dest, VTermRect src, تهی *user)
{
  VTermScreen *screen = user;

  صحیح cols = src.end_col - src.start_col;
  صحیح downward = src.start_row - dest.start_row;

  صحیح init_row, test_row, inc_row;
  اگر(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  وگرنه {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  برای(صحیح row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * اندازه(ScreenCell));

  بازگشت 1;
}

ایستا صحیح moverect_user(VTermRect dest, VTermRect src, تهی *user)
{
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->moverect) {
    اگر(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    اگر((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      بازگشت 1;
  }

  damagerect(screen, dest);

  بازگشت 1;
}

ایستا صحیح erase_internal(VTermRect rect, صحیح selective, تهی *user)
{
  VTermScreen *screen = user;

  برای(صحیح row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    ثابت VTerm_سطر_Info *info = vterm_state_get_lineinfo(screen->state, row);

    برای(صحیح col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      اگر (cell == پوچ)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	بازگشت 1;
      }
      اگر(selective && cell->pen.protected_cell)
        ادامه;

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

  بازگشت 1;
}

ایستا صحیح erase_user(VTermRect rect, صحیح selective UNUSED, تهی *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  بازگشت 1;
}

ایستا صحیح erase(VTermRect rect, صحیح selective, تهی *user)
{
  erase_internal(rect, selective, user);
  بازگشت erase_user(rect, 0, user);
}

ایستا صحیح scrollrect(VTermRect rect, صحیح downward, صحیح rightward, تهی *user)
{
  VTermScreen *screen = user;

  اگر(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    بازگشت 1;
  }

  اگر(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  اگر(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  وگرنه اگر(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  وگرنه {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  اگر(screen->damaged.start_row == -1)
    بازگشت 1;

  اگر(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  وگرنه اگر(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    اگر(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      اگر(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      اگر(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    اگر(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      اگر(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      اگر(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  وگرنه {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  بازگشت 1;
}

ایستا صحیح movecursor(VTermPos pos, VTermPos oldpos, صحیح visible, تهی *user)
{
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->movecursor)
    بازگشت (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  بازگشت 0;
}

ایستا صحیح setpenattr(VTermAttr attr, VTerm_مقدار *val, تهی *user)
{
  VTermScreen *screen = user;

  انتخاب(attr) {
  حالت VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    بازگشت 1;
  حالت VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->عدد;
    بازگشت 1;
  حالت VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    بازگشت 1;
  حالت VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    بازگشت 1;
  حالت VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    بازگشت 1;
  حالت VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    بازگشت 1;
  حالت VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    بازگشت 1;
  حالت VTERM_ATTR_FONT:
    screen->pen.font = val->عدد;
    بازگشت 1;
  حالت VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    بازگشت 1;
  حالت VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    بازگشت 1;
  حالت VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    بازگشت 1;
  حالت VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->عدد;
    بازگشت 1;

  حالت VTERM_N_ATTRS:
    بازگشت 0;
  }

  بازگشت 0;
}

ایستا صحیح settermprop(VTermProp prop, VTerm_مقدار *val, تهی *user)
{
  VTermScreen *screen = user;

  انتخاب(prop) {
  حالت VTERM_PROP_ALTSCREEN:
    اگر(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      بازگشت 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    اگر(!val->boolean)
      damagescreen(screen);
    بشکن;
  حالت VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    بشکن;
  پیشفرض:
    ; /* ignore */
  }

  اگر(screen->callbacks && screen->callbacks->settermprop)
    بازگشت (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  بازگشت 1;
}

ایستا صحیح bell(تهی *user)
{
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->bell)
    بازگشت (*screen->callbacks->bell)(screen->cbdata);

  بازگشت 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ایستا صحیح سطر_popcount(ScreenCell *buffer, صحیح row, صحیح rows UNUSED, صحیح cols)
{
  صحیح col = cols - 1;
  وقتی(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  بازگشت col + 1;
}

#define REFLOW (screen->reflow)

ایستا تهی resize_buffer(VTermScreen *screen, صحیح bufidx, صحیح new_rows, صحیح new_cols, صحیح active, VTermStateFields *statefields)
{
  صحیح old_rows = screen->rows;
  صحیح old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTerm_سطر_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, اندازه(ScreenCell) * new_rows * new_cols);
  VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, اندازه(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  صحیح old_row = old_rows - 1;
  صحیح new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  صحیح final_blank_row = new_rows;

  وقتی(old_row >= 0) {
    صحیح old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    وقتی(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    صحیح old_row_start = old_row;

    صحیح width = 0;
    برای(صحیح row = old_row_start; row <= old_row_end; row++) {
      اگر(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      وگرنه
        width += سطر_popcount(old_buffer, row, old_rows, old_cols);
    }

    اگر(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    صحیح new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    صحیح new_row_end = new_row;
    صحیح new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    صحیح old_col = 0;

    صحیح spare_rows = new_rows - final_blank_row;

    اگر(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      صحیح downwards = -new_row_start;
      اگر(downwards > spare_rows)
        downwards = spare_rows;
      صحیح rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * اندازه(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * اندازه(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      اگر(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    اگر(new_row_start < 0) {
      اگر(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        اگر(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      بشکن;
    }

    برای(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      صحیح شمار = width >= new_cols ? new_cols : width;
      width -= شمار;

      صحیح new_col = 0;

      وقتی(شمار) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        اگر(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        اگر(old_col == old_cols) {
          old_row++;

          اگر(!REFLOW) {
            new_col++;
            بشکن;
          }
          old_col = 0;
        }

        new_col++;
        شمار--;
      }

      اگر(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        اگر(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      وقتی(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  اگر(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    اگر(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  اگر(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    اگر (new_cursor.row < 0)
      new_cursor.row = 0;
    اگر (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  اگر(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    اگر((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      برای(صحیح row = 0; row <= old_row; row++) {
        ثابت VTerm_سطر_Info *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    اگر(active)
      statefields->pos.row -= (old_row + 1);
  }
  اگر(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    وقتی(new_row >= 0) {
      VTermPos pos;
      اگر(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        بشکن;

      pos.row = new_row;
      برای(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        برای(صحیح i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          اگر(!src->chars[i])
            بشکن;
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

        اگر(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      برای( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      اگر(active)
        statefields->pos.row++;
    }
  }
  اگر(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    صحیح moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * اندازه(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * اندازه(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    برای(new_row = moverows; new_row < new_rows; new_row++) {
      برای(صحیح col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTerm_سطر_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  اگر(active)
    statefields->pos = new_cursor;

  بازگشت;
}

ایستا صحیح resize(صحیح new_rows, صحیح new_cols, VTermStateFields *fields, تهی *user)
{
  VTermScreen *screen = user;

  صحیح altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  صحیح old_rows = screen->rows;
  صحیح old_cols = screen->cols;

  اگر(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    اگر(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    اگر (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, اندازه(VTermScreenCell) * new_cols);
  }

  اگر (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  اگر(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  وگرنه اگر(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, اندازه(new_lineinfo[0]) * new_rows);
    برای(صحیح row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTerm_سطر_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  اگر(new_cols <= old_cols) {
    اگر(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, اندازه(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  اگر(screen->callbacks && screen->callbacks->resize)
    بازگشت (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  بازگشت 1;
}

ایستا صحیح setlineinfo(صحیح row, ثابت VTerm_سطر_Info *newinfo, ثابت VTerm_سطر_Info *oldinfo, تهی *user)
{
  VTermScreen *screen = user;

  اگر(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    برای(صحیح col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      اگر (cell == پوچ)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	بازگشت 1;
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

    اگر(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  بازگشت 1;
}

ایستا صحیح sb_clear(تهی *user) {
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->sb_clear)
    اگر((*screen->callbacks->sb_clear)(screen->cbdata))
      بازگشت 1;

  بازگشت 0;
}

ایستا VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  پوچ, // moverect
  &erase, // erase
  پوچ, // initpen
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
ایستا VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  اگر(!state)
    بازگشت پوچ;

  VTermScreen *screen = vterm_allocator_malloc(vt, اندازه(VTermScreen));
  اگر (screen == پوچ)
    بازگشت پوچ;
  صحیح rows, cols;

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

  screen->callbacks = پوچ;
  screen->cbdata    = پوچ;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_buffer(screen, rows, cols);

  screen->buffer = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, اندازه(VTermScreenCell) * cols);
  اگر (screen->buffer == پوچ || screen->sb_buffer == پوچ)
  {
    vterm_screen_free(screen);
    بازگشت پوچ;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  بازگشت screen;
}

INTERNAL تهی vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  اگر(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

تهی vterm_screen_reset(VTermScreen *screen, صحیح hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ایستا size_t _get_chars(ثابت VTermScreen *screen, ثابت صحیح utf8, تهی *buffer, size_t len, ثابت VTermRect rect)
{
  size_t outpos = 0;
  صحیح padding = 0;

#define PUT(c)                                             \
  اگر(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    اگر(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (نویسه *)buffer + outpos);   \
    وگرنه                                                   \
      outpos += thislen;                                   \
  }                                                        \
  وگرنه {                                                   \
    اگر(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    وگرنه                                                   \
      outpos++;                                            \
  }

  برای(صحیح row = rect.start_row; row < rect.end_row; row++) {
    برای(صحیح col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      اگر (cell == پوچ)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	بازگشت 1;
      }
      اگر(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      وگرنه اگر(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      وگرنه {
        وقتی(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        برای(صحیح i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    اگر(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  بازگشت outpos;
}

size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect)
{
  بازگشت _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(ثابت VTermScreen *screen, نویسه *str, size_t len, ثابت VTermRect rect)
{
  بازگشت _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
صحیح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  اگر(!intcell)
    بازگشت 0;

  برای(صحیح i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    اگر(!intcell->chars[i])
      بشکن;
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

  اگر(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    اگر(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      اگر(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } وگرنه اگر(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } وگرنه {
        cell->width = 2;
      }
    } وگرنه
      cell->width = 1;
  } وگرنه {
    اگر(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    وگرنه
      cell->width = 1;
  }

  بازگشت 1;
}

صحیح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  برای(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    اگر(cell->chars[0] != 0)
      بازگشت 0;
  }

  بازگشت 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  اگر(vt->screen)
    بازگشت vt->screen;

  vt->screen = screen_new(vt);
  بازگشت vt->screen;
}

تهی vterm_screen_enable_reflow(VTermScreen *screen, صحیح reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

تهی vterm_screen_enable_altscreen(VTermScreen *screen, صحیح altscreen)
{
  اگر(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    صحیح rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

تهی vterm_screen_set_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, تهی *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

تهی *vterm_screen_get_cbdata(VTermScreen *screen)
{
  بازگشت screen->cbdata;
}

تهی vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

تهی vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, تهی *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

تهی *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  بازگشت vterm_state_get_unrecognised_fbdata(screen->state);
}

تهی vterm_screen_flush_damage(VTermScreen *screen)
{
  اگر(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  اگر(screen->damaged.start_row != -1) {
    اگر(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

تهی vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

ایستا صحیح attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  اگر((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    بازگشت 1;
  اگر((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    بازگشت 1;

  بازگشت 0;
}

صحیح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  اگر(extent->start_col < 0)
    extent->start_col = 0;
  اگر(extent->end_col < 0)
    extent->end_col = screen->cols;

  صحیح col;

  برای(col = pos.col - 1; col >= extent->start_col; col--)
    اگر(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      بشکن;
  extent->start_col = col + 1;

  برای(col = pos.col + 1; col < extent->end_col; col++)
    اگر(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      بشکن;
  extent->end_col = col - 1;

  بازگشت 1;
}

تهی vterm_screen_convert_color_to_rgb(ثابت VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

ایستا تهی reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  برای(صحیح row = 0; row <= screen->rows - 1; row++)
    برای(صحیح col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      اگر(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      اگر(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

تهی vterm_screen_set_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  اگر(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  اگر(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  اگر(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
