#অন্তর্ভাৱ "vterm_internal.h"

// vim: set sw=2 :
#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "rect.h"
#অন্তর্ভাৱ "utf8.h"

#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

#undef DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
প্রকারনির্দেশ সংরচনা
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned পূর্ণাঙ্ক bold      : 1;
  unsigned পূর্ণাঙ্ক underline : 2;
  unsigned পূর্ণাঙ্ক italic    : 1;
  unsigned পূর্ণাঙ্ক blink     : 1;
  unsigned পূর্ণাঙ্ক reverse   : 1;
  unsigned পূর্ণাঙ্ক conceal   : 1;
  unsigned পূর্ণাঙ্ক strike    : 1;
  unsigned পূর্ণাঙ্ক font      : 4; /* 0 to 9 */
  unsigned পূর্ণাঙ্ক small     : 1;
  unsigned পূর্ণাঙ্ক baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned পূর্ণাঙ্ক protected_cell : 1;
  unsigned পূর্ণাঙ্ক dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned পূর্ণাঙ্ক dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
প্রকারনির্দেশ সংরচনা
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

সংরচনা VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ধ্রুৱ VTermScreenCallbacks *callbacks;
  শূন্য *cbdata;
  পূর্ণাঙ্ক callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  পূর্ণাঙ্ক pending_scroll_downward, pending_scroll_rightward;

  পূর্ণাঙ্ক rows;
  পূর্ণাঙ্ক cols;

  unsigned পূর্ণাঙ্ক global_reverse : 1;
  unsigned পূর্ণাঙ্ক reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

স্থির শূন্য clearcell(ধ্রুৱ VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

স্থির ScreenCell *getcell(ধ্রুৱ VTermScreen *screen, পূর্ণাঙ্ক row, পূর্ণাঙ্ক col)
{
  যদি(row < 0 || row >= screen->rows)
    ঘূৰাই রিক্ত;
  যদি(col < 0 || col >= screen->cols)
    ঘূৰাই রিক্ত;
  যদি (screen->buffer == রিক্ত)
    ঘূৰাই রিক্ত;
  ঘূৰাই screen->buffer + (screen->cols * row) + col;
}

স্থির ScreenCell *alloc_buffer(VTermScreen *screen, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, পরিমাণ(ScreenCell) * rows * cols);

  বাবে(পূর্ণাঙ্ক row = 0; row < rows; row++) {
    বাবে(পূর্ণাঙ্ক col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  ঘূৰাই new_buffer;
}

স্থির শূন্য damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  ৱিকল্প(screen->damage_merge) {
  পক্ষ VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    ভঙ্গ;

  পক্ষ VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    যদি(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    অন্যথা যদি(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      ঘূৰাই;
    }
    অন্যথা যদি(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      যদি(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      যদি(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      ঘূৰাই;
    }
    অন্যথা {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    ভঙ্গ;

  পক্ষ VTERM_DAMAGE_SCREEN:
  পক্ষ VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    যদি(screen->damaged.start_row == -1)
      screen->damaged = rect;
    অন্যথা {
      rect_expand(&screen->damaged, &rect);
    }
    ঘূৰাই;

  স্ৱভাৱ:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    ঘূৰাই;
  }

  যদি(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

স্থির শূন্য damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

স্থির পূর্ণাঙ্ক putglyph(VTermGlyphInfo *info, VTermPos pos, শূন্য *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  যদি(!cell)
    ঘূৰাই 0;

  পূর্ণাঙ্ক i;
  বাবে(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  যদি(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  বাবে(পূর্ণাঙ্ক col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    যদি (onecell == রিক্ত)
      ভঙ্গ;
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

  ঘূৰাই 1;
}

স্থির শূন্য sb_pushline_from_row(VTermScreen *screen, পূর্ণাঙ্ক row, পূর্ণাঙ্ক continuation)
{
  VTermPos pos;
  pos.row = row;
  বাবে(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  যদি(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  অন্যথা
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

স্থির পূর্ণাঙ্ক premove(VTermRect rect, শূন্য *user)
{
  VTermScreen *screen = user;

  যদি(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    বাবে(পূর্ণাঙ্ক row = 0; row < rect.end_row; row++) {
      ধ্রুৱ VTerm_সাৰি_Info *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক moverect_internal(VTermRect dest, VTermRect src, শূন্য *user)
{
  VTermScreen *screen = user;

  পূর্ণাঙ্ক cols = src.end_col - src.start_col;
  পূর্ণাঙ্ক downward = src.start_row - dest.start_row;

  পূর্ণাঙ্ক init_row, test_row, inc_row;
  যদি(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  অন্যথা {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  বাবে(পূর্ণাঙ্ক row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * পরিমাণ(ScreenCell));

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক moverect_user(VTermRect dest, VTermRect src, শূন্য *user)
{
  VTermScreen *screen = user;

  যদি(screen->callbacks && screen->callbacks->moverect) {
    যদি(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    যদি((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      ঘূৰাই 1;
  }

  damagerect(screen, dest);

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক erase_internal(VTermRect rect, পূর্ণাঙ্ক selective, শূন্য *user)
{
  VTermScreen *screen = user;

  বাবে(পূর্ণাঙ্ক row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    ধ্রুৱ VTerm_সাৰি_Info *info = vterm_state_get_lineinfo(screen->state, row);

    বাবে(পূর্ণাঙ্ক col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      যদি (cell == রিক্ত)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	ঘূৰাই 1;
      }
      যদি(selective && cell->pen.protected_cell)
        অনুৱর্ত;

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

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক erase_user(VTermRect rect, পূর্ণাঙ্ক selective UNUSED, শূন্য *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক erase(VTermRect rect, পূর্ণাঙ্ক selective, শূন্য *user)
{
  erase_internal(rect, selective, user);
  ঘূৰাই erase_user(rect, 0, user);
}

স্থির পূর্ণাঙ্ক scrollrect(VTermRect rect, পূর্ণাঙ্ক downward, পূর্ণাঙ্ক rightward, শূন্য *user)
{
  VTermScreen *screen = user;

  যদি(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    ঘূৰাই 1;
  }

  যদি(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  যদি(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  অন্যথা যদি(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  অন্যথা {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  যদি(screen->damaged.start_row == -1)
    ঘূৰাই 1;

  যদি(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  অন্যথা যদি(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    যদি(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      যদি(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      যদি(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    যদি(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      যদি(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      যদি(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  অন্যথা {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক movecursor(VTermPos pos, VTermPos oldpos, পূর্ণাঙ্ক visible, শূন্য *user)
{
  VTermScreen *screen = user;

  যদি(screen->callbacks && screen->callbacks->movecursor)
    ঘূৰাই (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক setpenattr(VTermAttr attr, VTerm_মান *val, শূন্য *user)
{
  VTermScreen *screen = user;

  ৱিকল্প(attr) {
  পক্ষ VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->সংখ্যা;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_FONT:
    screen->pen.font = val->সংখ্যা;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    ঘূৰাই 1;
  পক্ষ VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->সংখ্যা;
    ঘূৰাই 1;

  পক্ষ VTERM_N_ATTRS:
    ঘূৰাই 0;
  }

  ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক settermprop(VTermProp prop, VTerm_মান *val, শূন্য *user)
{
  VTermScreen *screen = user;

  ৱিকল্প(prop) {
  পক্ষ VTERM_PROP_ALTSCREEN:
    যদি(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      ঘূৰাই 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    যদি(!val->boolean)
      damagescreen(screen);
    ভঙ্গ;
  পক্ষ VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    ভঙ্গ;
  স্ৱভাৱ:
    ; /* ignore */
  }

  যদি(screen->callbacks && screen->callbacks->settermprop)
    ঘূৰাই (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক bell(শূন্য *user)
{
  VTermScreen *screen = user;

  যদি(screen->callbacks && screen->callbacks->bell)
    ঘূৰাই (*screen->callbacks->bell)(screen->cbdata);

  ঘূৰাই 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
স্থির পূর্ণাঙ্ক সাৰি_popcount(ScreenCell *buffer, পূর্ণাঙ্ক row, পূর্ণাঙ্ক rows UNUSED, পূর্ণাঙ্ক cols)
{
  পূর্ণাঙ্ক col = cols - 1;
  যেতিয়ালৈ(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  ঘূৰাই col + 1;
}

#define REFLOW (screen->reflow)

স্থির শূন্য resize_buffer(VTermScreen *screen, পূর্ণাঙ্ক bufidx, পূর্ণাঙ্ক new_rows, পূর্ণাঙ্ক new_cols, পূর্ণাঙ্ক active, VTermStateFields *statefields)
{
  পূর্ণাঙ্ক old_rows = screen->rows;
  পূর্ণাঙ্ক old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTerm_সাৰি_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, পরিমাণ(ScreenCell) * new_rows * new_cols);
  VTerm_সাৰি_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, পরিমাণ(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  পূর্ণাঙ্ক old_row = old_rows - 1;
  পূর্ণাঙ্ক new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  পূর্ণাঙ্ক final_blank_row = new_rows;

  যেতিয়ালৈ(old_row >= 0) {
    পূর্ণাঙ্ক old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    যেতিয়ালৈ(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    পূর্ণাঙ্ক old_row_start = old_row;

    পূর্ণাঙ্ক width = 0;
    বাবে(পূর্ণাঙ্ক row = old_row_start; row <= old_row_end; row++) {
      যদি(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      অন্যথা
        width += সাৰি_popcount(old_buffer, row, old_rows, old_cols);
    }

    যদি(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    পূর্ণাঙ্ক new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    পূর্ণাঙ্ক new_row_end = new_row;
    পূর্ণাঙ্ক new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    পূর্ণাঙ্ক old_col = 0;

    পূর্ণাঙ্ক spare_rows = new_rows - final_blank_row;

    যদি(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      পূর্ণাঙ্ক downwards = -new_row_start;
      যদি(downwards > spare_rows)
        downwards = spare_rows;
      পূর্ণাঙ্ক rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * পরিমাণ(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * পরিমাণ(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      যদি(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    যদি(new_row_start < 0) {
      যদি(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        যদি(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      ভঙ্গ;
    }

    বাবে(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      পূর্ণাঙ্ক গুনতি = width >= new_cols ? new_cols : width;
      width -= গুনতি;

      পূর্ণাঙ্ক new_col = 0;

      যেতিয়ালৈ(গুনতি) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        যদি(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        যদি(old_col == old_cols) {
          old_row++;

          যদি(!REFLOW) {
            new_col++;
            ভঙ্গ;
          }
          old_col = 0;
        }

        new_col++;
        গুনতি--;
      }

      যদি(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        যদি(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      যেতিয়ালৈ(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  যদি(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    যদি(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  যদি(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    যদি (new_cursor.row < 0)
      new_cursor.row = 0;
    যদি (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  যদি(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    যদি((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      বাবে(পূর্ণাঙ্ক row = 0; row <= old_row; row++) {
        ধ্রুৱ VTerm_সাৰি_Info *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    যদি(active)
      statefields->pos.row -= (old_row + 1);
  }
  যদি(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    যেতিয়ালৈ(new_row >= 0) {
      VTermPos pos;
      যদি(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        ভঙ্গ;

      pos.row = new_row;
      বাবে(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        বাবে(পূর্ণাঙ্ক i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          যদি(!src->chars[i])
            ভঙ্গ;
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

        যদি(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      বাবে( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      যদি(active)
        statefields->pos.row++;
    }
  }
  যদি(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    পূর্ণাঙ্ক moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * পরিমাণ(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * পরিমাণ(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    বাবে(new_row = moverows; new_row < new_rows; new_row++) {
      বাবে(পূর্ণাঙ্ক col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTerm_সাৰি_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  যদি(active)
    statefields->pos = new_cursor;

  ঘূৰাই;
}

স্থির পূর্ণাঙ্ক resize(পূর্ণাঙ্ক new_rows, পূর্ণাঙ্ক new_cols, VTermStateFields *fields, শূন্য *user)
{
  VTermScreen *screen = user;

  পূর্ণাঙ্ক altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  পূর্ণাঙ্ক old_rows = screen->rows;
  পূর্ণাঙ্ক old_cols = screen->cols;

  যদি(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    যদি(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    যদি (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, পরিমাণ(VTermScreenCell) * new_cols);
  }

  যদি (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  যদি(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  অন্যথা যদি(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_সাৰি_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, পরিমাণ(new_lineinfo[0]) * new_rows);
    বাবে(পূর্ণাঙ্ক row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTerm_সাৰি_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  যদি(new_cols <= old_cols) {
    যদি(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, পরিমাণ(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  যদি(screen->callbacks && screen->callbacks->resize)
    ঘূৰাই (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক setlineinfo(পূর্ণাঙ্ক row, ধ্রুৱ VTerm_সাৰি_Info *newinfo, ধ্রুৱ VTerm_সাৰি_Info *oldinfo, শূন্য *user)
{
  VTermScreen *screen = user;

  যদি(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    বাবে(পূর্ণাঙ্ক col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      যদি (cell == রিক্ত)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	ঘূৰাই 1;
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

    যদি(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  ঘূৰাই 1;
}

স্থির পূর্ণাঙ্ক sb_clear(শূন্য *user) {
  VTermScreen *screen = user;

  যদি(screen->callbacks && screen->callbacks->sb_clear)
    যদি((*screen->callbacks->sb_clear)(screen->cbdata))
      ঘূৰাই 1;

  ঘূৰাই 0;
}

স্থির VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  রিক্ত, // moverect
  &erase, // erase
  রিক্ত, // initpen
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
স্থির VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  যদি(!state)
    ঘূৰাই রিক্ত;

  VTermScreen *screen = vterm_allocator_malloc(vt, পরিমাণ(VTermScreen));
  যদি (screen == রিক্ত)
    ঘূৰাই রিক্ত;
  পূর্ণাঙ্ক rows, cols;

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

  screen->callbacks = রিক্ত;
  screen->cbdata    = রিক্ত;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_buffer(screen, rows, cols);

  screen->buffer = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, পরিমাণ(VTermScreenCell) * cols);
  যদি (screen->buffer == রিক্ত || screen->sb_buffer == রিক্ত)
  {
    vterm_screen_free(screen);
    ঘূৰাই রিক্ত;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  ঘূৰাই screen;
}

INTERNAL শূন্য vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  যদি(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

শূন্য vterm_screen_reset(VTermScreen *screen, পূর্ণাঙ্ক hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

স্থির size_t _get_chars(ধ্রুৱ VTermScreen *screen, ধ্রুৱ পূর্ণাঙ্ক utf8, শূন্য *buffer, size_t len, ধ্রুৱ VTermRect rect)
{
  size_t outpos = 0;
  পূর্ণাঙ্ক padding = 0;

#define PUT(c)                                             \
  যদি(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    যদি(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (ৱর্ণ *)buffer + outpos);   \
    অন্যথা                                                   \
      outpos += thislen;                                   \
  }                                                        \
  অন্যথা {                                                   \
    যদি(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    অন্যথা                                                   \
      outpos++;                                            \
  }

  বাবে(পূর্ণাঙ্ক row = rect.start_row; row < rect.end_row; row++) {
    বাবে(পূর্ণাঙ্ক col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      যদি (cell == রিক্ত)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	ঘূৰাই 1;
      }
      যদি(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      অন্যথা যদি(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      অন্যথা {
        যেতিয়ালৈ(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        বাবে(পূর্ণাঙ্ক i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    যদি(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  ঘূৰাই outpos;
}

size_t vterm_screen_get_chars(ধ্রুৱ VTermScreen *screen, uint32_t *chars, size_t len, ধ্রুৱ VTermRect rect)
{
  ঘূৰাই _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(ধ্রুৱ VTermScreen *screen, ৱর্ণ *str, size_t len, ধ্রুৱ VTermRect rect)
{
  ঘূৰাই _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
পূর্ণাঙ্ক vterm_screen_get_cell(ধ্রুৱ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  যদি(!intcell)
    ঘূৰাই 0;

  বাবে(পূর্ণাঙ্ক i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    যদি(!intcell->chars[i])
      ভঙ্গ;
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

  যদি(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    যদি(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      যদি(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } অন্যথা যদি(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } অন্যথা {
        cell->width = 2;
      }
    } অন্যথা
      cell->width = 1;
  } অন্যথা {
    যদি(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    অন্যথা
      cell->width = 1;
  }

  ঘূৰাই 1;
}

পূর্ণাঙ্ক vterm_screen_is_eol(ধ্রুৱ VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  বাবে(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    যদি(cell->chars[0] != 0)
      ঘূৰাই 0;
  }

  ঘূৰাই 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  যদি(vt->screen)
    ঘূৰাই vt->screen;

  vt->screen = screen_new(vt);
  ঘূৰাই vt->screen;
}

শূন্য vterm_screen_enable_reflow(VTermScreen *screen, পূর্ণাঙ্ক reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

শূন্য vterm_screen_enable_altscreen(VTermScreen *screen, পূর্ণাঙ্ক altscreen)
{
  যদি(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    পূর্ণাঙ্ক rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

শূন্য vterm_screen_set_callbacks(VTermScreen *screen, ধ্রুৱ VTermScreenCallbacks *callbacks, শূন্য *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

শূন্য *vterm_screen_get_cbdata(VTermScreen *screen)
{
  ঘূৰাই screen->cbdata;
}

শূন্য vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

শূন্য vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ধ্রুৱ VTermStateFallbacks *fallbacks, শূন্য *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

শূন্য *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  ঘূৰাই vterm_state_get_unrecognised_fbdata(screen->state);
}

শূন্য vterm_screen_flush_damage(VTermScreen *screen)
{
  যদি(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  যদি(screen->damaged.start_row != -1) {
    যদি(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

শূন্য vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

স্থির পূর্ণাঙ্ক attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  যদি((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    ঘূৰাই 1;
  যদি((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    ঘূৰাই 1;

  ঘূৰাই 0;
}

পূর্ণাঙ্ক vterm_screen_get_attrs_extent(ধ্রুৱ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  যদি(extent->start_col < 0)
    extent->start_col = 0;
  যদি(extent->end_col < 0)
    extent->end_col = screen->cols;

  পূর্ণাঙ্ক col;

  বাবে(col = pos.col - 1; col >= extent->start_col; col--)
    যদি(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      ভঙ্গ;
  extent->start_col = col + 1;

  বাবে(col = pos.col + 1; col < extent->end_col; col++)
    যদি(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      ভঙ্গ;
  extent->end_col = col - 1;

  ঘূৰাই 1;
}

শূন্য vterm_screen_convert_color_to_rgb(ধ্রুৱ VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

স্থির শূন্য reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  বাবে(পূর্ণাঙ্ক row = 0; row <= screen->rows - 1; row++)
    বাবে(পূর্ণাঙ্ক col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      যদি(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      যদি(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

শূন্য vterm_screen_set_default_colors(VTermScreen *screen, ধ্রুৱ VTermColor *default_fg, ধ্রুৱ VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  যদি(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  যদি(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  যদি(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
