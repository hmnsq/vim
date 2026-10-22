#अन्तर्भाव "vterm_internal.h"

// vim: set sw=2 :
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "rect.h"
#अन्तर्भाव "utf8.h"

#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

#undef DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
प्रकारनिर्देश संरचना
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned पूर्णाङ्क bold      : 1;
  unsigned पूर्णाङ्क underline : 2;
  unsigned पूर्णाङ्क italic    : 1;
  unsigned पूर्णाङ्क blink     : 1;
  unsigned पूर्णाङ्क reverse   : 1;
  unsigned पूर्णाङ्क conceal   : 1;
  unsigned पूर्णाङ्क strike    : 1;
  unsigned पूर्णाङ्क font      : 4; /* 0 to 9 */
  unsigned पूर्णाङ्क small     : 1;
  unsigned पूर्णाङ्क baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned पूर्णाङ्क protected_cell : 1;
  unsigned पूर्णाङ्क dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned पूर्णाङ्क dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
प्रकारनिर्देश संरचना
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

संरचना VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ध्रुव VTermScreenCallbacks *callbacks;
  शून्य *cbdata;
  पूर्णाङ्क callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  पूर्णाङ्क pending_scroll_downward, pending_scroll_rightward;

  पूर्णाङ्क rows;
  पूर्णाङ्क cols;

  unsigned पूर्णाङ्क global_reverse : 1;
  unsigned पूर्णाङ्क reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

स्थिर शून्य clearcell(ध्रुव VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

स्थिर ScreenCell *getcell(ध्रुव VTermScreen *screen, पूर्णाङ्क row, पूर्णाङ्क col)
{
  सचे(row < 0 || row >= screen->rows)
    पटिनिवत्त रिक्त;
  सचे(col < 0 || col >= screen->cols)
    पटिनिवत्त रिक्त;
  सचे (screen->buffer == रिक्त)
    पटिनिवत्त रिक्त;
  पटिनिवत्त screen->buffer + (screen->cols * row) + col;
}

स्थिर ScreenCell *alloc_buffer(VTermScreen *screen, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, परिमाण(ScreenCell) * rows * cols);

  अत्थाय(पूर्णाङ्क row = 0; row < rows; row++) {
    अत्थाय(पूर्णाङ्क col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  पटिनिवत्त new_buffer;
}

स्थिर शून्य damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  विकल्प(screen->damage_merge) {
  पक्ष VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    भङ्ग;

  पक्ष VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    सचे(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    अञ्ञथा सचे(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      पटिनिवत्त;
    }
    अञ्ञथा सचे(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      सचे(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      सचे(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      पटिनिवत्त;
    }
    अञ्ञथा {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    भङ्ग;

  पक्ष VTERM_DAMAGE_SCREEN:
  पक्ष VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    सचे(screen->damaged.start_row == -1)
      screen->damaged = rect;
    अञ्ञथा {
      rect_expand(&screen->damaged, &rect);
    }
    पटिनिवत्त;

  स्वभाव:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    पटिनिवत्त;
  }

  सचे(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

स्थिर शून्य damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

स्थिर पूर्णाङ्क putglyph(VTermGlyphInfo *info, VTermPos pos, शून्य *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  सचे(!cell)
    पटिनिवत्त 0;

  पूर्णाङ्क i;
  अत्थाय(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  सचे(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  अत्थाय(पूर्णाङ्क col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    सचे (onecell == रिक्त)
      भङ्ग;
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

  पटिनिवत्त 1;
}

स्थिर शून्य sb_pushline_from_row(VTermScreen *screen, पूर्णाङ्क row, पूर्णाङ्क continuation)
{
  VTermPos pos;
  pos.row = row;
  अत्थाय(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  सचे(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  अञ्ञथा
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

स्थिर पूर्णाङ्क premove(VTermRect rect, शून्य *user)
{
  VTermScreen *screen = user;

  सचे(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    अत्थाय(पूर्णाङ्क row = 0; row < rect.end_row; row++) {
      ध्रुव VTerm_पङ्क्तिः_Info *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क moverect_internal(VTermRect dest, VTermRect src, शून्य *user)
{
  VTermScreen *screen = user;

  पूर्णाङ्क cols = src.end_col - src.start_col;
  पूर्णाङ्क downward = src.start_row - dest.start_row;

  पूर्णाङ्क init_row, test_row, inc_row;
  सचे(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  अञ्ञथा {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  अत्थाय(पूर्णाङ्क row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * परिमाण(ScreenCell));

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क moverect_user(VTermRect dest, VTermRect src, शून्य *user)
{
  VTermScreen *screen = user;

  सचे(screen->callbacks && screen->callbacks->moverect) {
    सचे(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    सचे((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      पटिनिवत्त 1;
  }

  damagerect(screen, dest);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क erase_internal(VTermRect rect, पूर्णाङ्क selective, शून्य *user)
{
  VTermScreen *screen = user;

  अत्थाय(पूर्णाङ्क row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    ध्रुव VTerm_पङ्क्तिः_Info *info = vterm_state_get_lineinfo(screen->state, row);

    अत्थाय(पूर्णाङ्क col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      सचे (cell == रिक्त)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	पटिनिवत्त 1;
      }
      सचे(selective && cell->pen.protected_cell)
        अनुवर्त;

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

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क erase_user(VTermRect rect, पूर्णाङ्क selective UNUSED, शून्य *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क erase(VTermRect rect, पूर्णाङ्क selective, शून्य *user)
{
  erase_internal(rect, selective, user);
  पटिनिवत्त erase_user(rect, 0, user);
}

स्थिर पूर्णाङ्क scrollrect(VTermRect rect, पूर्णाङ्क downward, पूर्णाङ्क rightward, शून्य *user)
{
  VTermScreen *screen = user;

  सचे(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    पटिनिवत्त 1;
  }

  सचे(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  सचे(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  अञ्ञथा सचे(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  अञ्ञथा {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  सचे(screen->damaged.start_row == -1)
    पटिनिवत्त 1;

  सचे(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  अञ्ञथा सचे(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    सचे(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      सचे(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      सचे(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    सचे(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      सचे(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      सचे(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  अञ्ञथा {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क movecursor(VTermPos pos, VTermPos oldpos, पूर्णाङ्क visible, शून्य *user)
{
  VTermScreen *screen = user;

  सचे(screen->callbacks && screen->callbacks->movecursor)
    पटिनिवत्त (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क setpenattr(VTermAttr attr, VTerm_मानम् *val, शून्य *user)
{
  VTermScreen *screen = user;

  विकल्प(attr) {
  पक्ष VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->सङ्ख्या;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_FONT:
    screen->pen.font = val->सङ्ख्या;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    पटिनिवत्त 1;
  पक्ष VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->सङ्ख्या;
    पटिनिवत्त 1;

  पक्ष VTERM_N_ATTRS:
    पटिनिवत्त 0;
  }

  पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क settermprop(VTermProp prop, VTerm_मानम् *val, शून्य *user)
{
  VTermScreen *screen = user;

  विकल्प(prop) {
  पक्ष VTERM_PROP_ALTSCREEN:
    सचे(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      पटिनिवत्त 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    सचे(!val->boolean)
      damagescreen(screen);
    भङ्ग;
  पक्ष VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    भङ्ग;
  स्वभाव:
    ; /* ignore */
  }

  सचे(screen->callbacks && screen->callbacks->settermprop)
    पटिनिवत्त (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क bell(शून्य *user)
{
  VTermScreen *screen = user;

  सचे(screen->callbacks && screen->callbacks->bell)
    पटिनिवत्त (*screen->callbacks->bell)(screen->cbdata);

  पटिनिवत्त 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
स्थिर पूर्णाङ्क पङ्क्तिः_popcount(ScreenCell *buffer, पूर्णाङ्क row, पूर्णाङ्क rows UNUSED, पूर्णाङ्क cols)
{
  पूर्णाङ्क col = cols - 1;
  याव(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  पटिनिवत्त col + 1;
}

#define REFLOW (screen->reflow)

स्थिर शून्य resize_buffer(VTermScreen *screen, पूर्णाङ्क bufidx, पूर्णाङ्क new_rows, पूर्णाङ्क new_cols, पूर्णाङ्क active, VTermStateFields *statefields)
{
  पूर्णाङ्क old_rows = screen->rows;
  पूर्णाङ्क old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTerm_पङ्क्तिः_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, परिमाण(ScreenCell) * new_rows * new_cols);
  VTerm_पङ्क्तिः_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, परिमाण(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  पूर्णाङ्क old_row = old_rows - 1;
  पूर्णाङ्क new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  पूर्णाङ्क final_blank_row = new_rows;

  याव(old_row >= 0) {
    पूर्णाङ्क old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    याव(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    पूर्णाङ्क old_row_start = old_row;

    पूर्णाङ्क width = 0;
    अत्थाय(पूर्णाङ्क row = old_row_start; row <= old_row_end; row++) {
      सचे(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      अञ्ञथा
        width += पङ्क्तिः_popcount(old_buffer, row, old_rows, old_cols);
    }

    सचे(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    पूर्णाङ्क new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    पूर्णाङ्क new_row_end = new_row;
    पूर्णाङ्क new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    पूर्णाङ्क old_col = 0;

    पूर्णाङ्क spare_rows = new_rows - final_blank_row;

    सचे(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      पूर्णाङ्क downwards = -new_row_start;
      सचे(downwards > spare_rows)
        downwards = spare_rows;
      पूर्णाङ्क rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * परिमाण(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * परिमाण(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      सचे(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    सचे(new_row_start < 0) {
      सचे(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        सचे(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      भङ्ग;
    }

    अत्थाय(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      पूर्णाङ्क गणनम् = width >= new_cols ? new_cols : width;
      width -= गणनम्;

      पूर्णाङ्क new_col = 0;

      याव(गणनम्) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        सचे(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        सचे(old_col == old_cols) {
          old_row++;

          सचे(!REFLOW) {
            new_col++;
            भङ्ग;
          }
          old_col = 0;
        }

        new_col++;
        गणनम्--;
      }

      सचे(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        सचे(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      याव(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  सचे(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    सचे(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  सचे(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    सचे (new_cursor.row < 0)
      new_cursor.row = 0;
    सचे (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  सचे(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    सचे((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      अत्थाय(पूर्णाङ्क row = 0; row <= old_row; row++) {
        ध्रुव VTerm_पङ्क्तिः_Info *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    सचे(active)
      statefields->pos.row -= (old_row + 1);
  }
  सचे(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    याव(new_row >= 0) {
      VTermPos pos;
      सचे(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        भङ्ग;

      pos.row = new_row;
      अत्थाय(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        अत्थाय(पूर्णाङ्क i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          सचे(!src->chars[i])
            भङ्ग;
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

        सचे(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      अत्थाय( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      सचे(active)
        statefields->pos.row++;
    }
  }
  सचे(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    पूर्णाङ्क moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * परिमाण(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * परिमाण(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    अत्थाय(new_row = moverows; new_row < new_rows; new_row++) {
      अत्थाय(पूर्णाङ्क col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTerm_पङ्क्तिः_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  सचे(active)
    statefields->pos = new_cursor;

  पटिनिवत्त;
}

स्थिर पूर्णाङ्क resize(पूर्णाङ्क new_rows, पूर्णाङ्क new_cols, VTermStateFields *fields, शून्य *user)
{
  VTermScreen *screen = user;

  पूर्णाङ्क altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  पूर्णाङ्क old_rows = screen->rows;
  पूर्णाङ्क old_cols = screen->cols;

  सचे(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    सचे(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    सचे (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, परिमाण(VTermScreenCell) * new_cols);
  }

  सचे (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  सचे(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  अञ्ञथा सचे(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_पङ्क्तिः_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, परिमाण(new_lineinfo[0]) * new_rows);
    अत्थाय(पूर्णाङ्क row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTerm_पङ्क्तिः_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  सचे(new_cols <= old_cols) {
    सचे(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, परिमाण(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  सचे(screen->callbacks && screen->callbacks->resize)
    पटिनिवत्त (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क setlineinfo(पूर्णाङ्क row, ध्रुव VTerm_पङ्क्तिः_Info *newinfo, ध्रुव VTerm_पङ्क्तिः_Info *oldinfo, शून्य *user)
{
  VTermScreen *screen = user;

  सचे(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    अत्थाय(पूर्णाङ्क col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      सचे (cell == रिक्त)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	पटिनिवत्त 1;
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

    सचे(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  पटिनिवत्त 1;
}

स्थिर पूर्णाङ्क sb_clear(शून्य *user) {
  VTermScreen *screen = user;

  सचे(screen->callbacks && screen->callbacks->sb_clear)
    सचे((*screen->callbacks->sb_clear)(screen->cbdata))
      पटिनिवत्त 1;

  पटिनिवत्त 0;
}

स्थिर VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  रिक्त, // moverect
  &erase, // erase
  रिक्त, // initpen
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
स्थिर VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  सचे(!state)
    पटिनिवत्त रिक्त;

  VTermScreen *screen = vterm_allocator_malloc(vt, परिमाण(VTermScreen));
  सचे (screen == रिक्त)
    पटिनिवत्त रिक्त;
  पूर्णाङ्क rows, cols;

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

  screen->callbacks = रिक्त;
  screen->cbdata    = रिक्त;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_buffer(screen, rows, cols);

  screen->buffer = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, परिमाण(VTermScreenCell) * cols);
  सचे (screen->buffer == रिक्त || screen->sb_buffer == रिक्त)
  {
    vterm_screen_free(screen);
    पटिनिवत्त रिक्त;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  पटिनिवत्त screen;
}

INTERNAL शून्य vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  सचे(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

शून्य vterm_screen_reset(VTermScreen *screen, पूर्णाङ्क hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

स्थिर size_t _get_chars(ध्रुव VTermScreen *screen, ध्रुव पूर्णाङ्क utf8, शून्य *buffer, size_t len, ध्रुव VTermRect rect)
{
  size_t outpos = 0;
  पूर्णाङ्क padding = 0;

#define PUT(c)                                             \
  सचे(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    सचे(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (वर्ण *)buffer + outpos);   \
    अञ्ञथा                                                   \
      outpos += thislen;                                   \
  }                                                        \
  अञ्ञथा {                                                   \
    सचे(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    अञ्ञथा                                                   \
      outpos++;                                            \
  }

  अत्थाय(पूर्णाङ्क row = rect.start_row; row < rect.end_row; row++) {
    अत्थाय(पूर्णाङ्क col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      सचे (cell == रिक्त)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	पटिनिवत्त 1;
      }
      सचे(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      अञ्ञथा सचे(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      अञ्ञथा {
        याव(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        अत्थाय(पूर्णाङ्क i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    सचे(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  पटिनिवत्त outpos;
}

size_t vterm_screen_get_chars(ध्रुव VTermScreen *screen, uint32_t *chars, size_t len, ध्रुव VTermRect rect)
{
  पटिनिवत्त _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(ध्रुव VTermScreen *screen, वर्ण *str, size_t len, ध्रुव VTermRect rect)
{
  पटिनिवत्त _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
पूर्णाङ्क vterm_screen_get_cell(ध्रुव VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  सचे(!intcell)
    पटिनिवत्त 0;

  अत्थाय(पूर्णाङ्क i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    सचे(!intcell->chars[i])
      भङ्ग;
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

  सचे(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    सचे(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      सचे(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } अञ्ञथा सचे(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } अञ्ञथा {
        cell->width = 2;
      }
    } अञ्ञथा
      cell->width = 1;
  } अञ्ञथा {
    सचे(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    अञ्ञथा
      cell->width = 1;
  }

  पटिनिवत्त 1;
}

पूर्णाङ्क vterm_screen_is_eol(ध्रुव VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  अत्थाय(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    सचे(cell->chars[0] != 0)
      पटिनिवत्त 0;
  }

  पटिनिवत्त 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  सचे(vt->screen)
    पटिनिवत्त vt->screen;

  vt->screen = screen_new(vt);
  पटिनिवत्त vt->screen;
}

शून्य vterm_screen_enable_reflow(VTermScreen *screen, पूर्णाङ्क reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

शून्य vterm_screen_enable_altscreen(VTermScreen *screen, पूर्णाङ्क altscreen)
{
  सचे(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    पूर्णाङ्क rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

शून्य vterm_screen_set_callbacks(VTermScreen *screen, ध्रुव VTermScreenCallbacks *callbacks, शून्य *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

शून्य *vterm_screen_get_cbdata(VTermScreen *screen)
{
  पटिनिवत्त screen->cbdata;
}

शून्य vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

शून्य vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ध्रुव VTermStateFallbacks *fallbacks, शून्य *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

शून्य *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  पटिनिवत्त vterm_state_get_unrecognised_fbdata(screen->state);
}

शून्य vterm_screen_flush_damage(VTermScreen *screen)
{
  सचे(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  सचे(screen->damaged.start_row != -1) {
    सचे(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

शून्य vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

स्थिर पूर्णाङ्क attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  सचे((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    पटिनिवत्त 1;
  सचे((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    पटिनिवत्त 1;

  पटिनिवत्त 0;
}

पूर्णाङ्क vterm_screen_get_attrs_extent(ध्रुव VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  सचे(extent->start_col < 0)
    extent->start_col = 0;
  सचे(extent->end_col < 0)
    extent->end_col = screen->cols;

  पूर्णाङ्क col;

  अत्थाय(col = pos.col - 1; col >= extent->start_col; col--)
    सचे(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      भङ्ग;
  extent->start_col = col + 1;

  अत्थाय(col = pos.col + 1; col < extent->end_col; col++)
    सचे(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      भङ्ग;
  extent->end_col = col - 1;

  पटिनिवत्त 1;
}

शून्य vterm_screen_convert_color_to_rgb(ध्रुव VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

स्थिर शून्य reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  अत्थाय(पूर्णाङ्क row = 0; row <= screen->rows - 1; row++)
    अत्थाय(पूर्णाङ्क col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      सचे(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      सचे(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

शून्य vterm_screen_set_default_colors(VTermScreen *screen, ध्रुव VTermColor *default_fg, ध्रुव VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  सचे(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  सचे(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  सचे(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
