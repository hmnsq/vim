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
प्रकारव्याख्या संरचना
{
  /* After the bitfield */
  VTermColor   fg, bg;

  अचिन्हित पूर्णांक bold      : 1;
  अचिन्हित पूर्णांक underline : 2;
  अचिन्हित पूर्णांक italic    : 1;
  अचिन्हित पूर्णांक blink     : 1;
  अचिन्हित पूर्णांक reverse   : 1;
  अचिन्हित पूर्णांक conceal   : 1;
  अचिन्हित पूर्णांक strike    : 1;
  अचिन्हित पूर्णांक font      : 4; /* 0 to 9 */
  अचिन्हित पूर्णांक small     : 1;
  अचिन्हित पूर्णांक baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  अचिन्हित पूर्णांक protected_cell : 1;
  अचिन्हित पूर्णांक dwl            : 1; /* on a DECDWL or DECDHL line */
  अचिन्हित पूर्णांक dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
प्रकारव्याख्या संरचना
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

संरचना VTermScreen
{
  VTerm *vt;
  VTermState *state;

  स्थिरांक VTermScreenCallbacks *callbacks;
  शून्य *cbdata;
  पूर्णांक callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  पूर्णांक pending_scroll_downward, pending_scroll_rightward;

  पूर्णांक rows;
  पूर्णांक cols;

  अचिन्हित पूर्णांक global_reverse : 1;
  अचिन्हित पूर्णांक reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

स्थिर शून्य clearcell(स्थिरांक VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

स्थिर ScreenCell *getcell(स्थिरांक VTermScreen *screen, पूर्णांक row, पूर्णांक col)
{
  जर(row < 0 || row >= screen->rows)
    परत रिक्त;
  जर(col < 0 || col >= screen->cols)
    परत रिक्त;
  जर (screen->buffer == रिक्त)
    परत रिक्त;
  परत screen->buffer + (screen->cols * row) + col;
}

स्थिर ScreenCell *alloc_buffer(VTermScreen *screen, पूर्णांक rows, पूर्णांक cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, आकार(ScreenCell) * rows * cols);

  साठी(पूर्णांक row = 0; row < rows; row++) {
    साठी(पूर्णांक col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  परत new_buffer;
}

स्थिर शून्य damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  निवड(screen->damage_merge) {
  स्थिती VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    थांबा;

  स्थिती VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    जर(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    अन्यथा जर(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      परत;
    }
    अन्यथा जर(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      जर(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      जर(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      परत;
    }
    अन्यथा {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    थांबा;

  स्थिती VTERM_DAMAGE_SCREEN:
  स्थिती VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    जर(screen->damaged.start_row == -1)
      screen->damaged = rect;
    अन्यथा {
      rect_expand(&screen->damaged, &rect);
    }
    परत;

  पूर्वनिर्धारित:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    परत;
  }

  जर(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

स्थिर शून्य damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

स्थिर पूर्णांक putglyph(VTermGlyphInfo *info, VTermPos pos, शून्य *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  जर(!cell)
    परत 0;

  पूर्णांक i;
  साठी(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  जर(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  साठी(पूर्णांक col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    जर (onecell == रिक्त)
      थांबा;
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

  परत 1;
}

स्थिर शून्य sb_pushline_from_row(VTermScreen *screen, पूर्णांक row, पूर्णांक continuation)
{
  VTermPos pos;
  pos.row = row;
  साठी(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  जर(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  अन्यथा
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

स्थिर पूर्णांक premove(VTermRect rect, शून्य *user)
{
  VTermScreen *screen = user;

  जर(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    साठी(पूर्णांक row = 0; row < rect.end_row; row++) {
      स्थिरांक VTerm_ओळ_Info *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  परत 1;
}

स्थिर पूर्णांक moverect_internal(VTermRect dest, VTermRect src, शून्य *user)
{
  VTermScreen *screen = user;

  पूर्णांक cols = src.end_col - src.start_col;
  पूर्णांक downward = src.start_row - dest.start_row;

  पूर्णांक init_row, test_row, inc_row;
  जर(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  अन्यथा {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  साठी(पूर्णांक row = init_row; row != test_row; row += inc_row)
    स_हटाओ(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * आकार(ScreenCell));

  परत 1;
}

स्थिर पूर्णांक moverect_user(VTermRect dest, VTermRect src, शून्य *user)
{
  VTermScreen *screen = user;

  जर(screen->callbacks && screen->callbacks->moverect) {
    जर(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    जर((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      परत 1;
  }

  damagerect(screen, dest);

  परत 1;
}

स्थिर पूर्णांक erase_internal(VTermRect rect, पूर्णांक selective, शून्य *user)
{
  VTermScreen *screen = user;

  साठी(पूर्णांक row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    स्थिरांक VTerm_ओळ_Info *info = vterm_state_get_lineinfo(screen->state, row);

    साठी(पूर्णांक col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      जर (cell == रिक्त)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	परत 1;
      }
      जर(selective && cell->pen.protected_cell)
        सुरूठेवा;

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

  परत 1;
}

स्थिर पूर्णांक erase_user(VTermRect rect, पूर्णांक selective UNUSED, शून्य *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  परत 1;
}

स्थिर पूर्णांक erase(VTermRect rect, पूर्णांक selective, शून्य *user)
{
  erase_internal(rect, selective, user);
  परत erase_user(rect, 0, user);
}

स्थिर पूर्णांक scrollrect(VTermRect rect, पूर्णांक downward, पूर्णांक rightward, शून्य *user)
{
  VTermScreen *screen = user;

  जर(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    परत 1;
  }

  जर(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  जर(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  अन्यथा जर(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  अन्यथा {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  जर(screen->damaged.start_row == -1)
    परत 1;

  जर(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  अन्यथा जर(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    जर(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      जर(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      जर(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    जर(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      जर(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      जर(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  अन्यथा {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  परत 1;
}

स्थिर पूर्णांक movecursor(VTermPos pos, VTermPos oldpos, पूर्णांक visible, शून्य *user)
{
  VTermScreen *screen = user;

  जर(screen->callbacks && screen->callbacks->movecursor)
    परत (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  परत 0;
}

स्थिर पूर्णांक setpenattr(VTermAttr attr, VTerm_मूल्य *val, शून्य *user)
{
  VTermScreen *screen = user;

  निवड(attr) {
  स्थिती VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    परत 1;
  स्थिती VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->संख्या;
    परत 1;
  स्थिती VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    परत 1;
  स्थिती VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    परत 1;
  स्थिती VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    परत 1;
  स्थिती VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    परत 1;
  स्थिती VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    परत 1;
  स्थिती VTERM_ATTR_FONT:
    screen->pen.font = val->संख्या;
    परत 1;
  स्थिती VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    परत 1;
  स्थिती VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    परत 1;
  स्थिती VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    परत 1;
  स्थिती VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->संख्या;
    परत 1;

  स्थिती VTERM_N_ATTRS:
    परत 0;
  }

  परत 0;
}

स्थिर पूर्णांक settermprop(VTermProp prop, VTerm_मूल्य *val, शून्य *user)
{
  VTermScreen *screen = user;

  निवड(prop) {
  स्थिती VTERM_PROP_ALTSCREEN:
    जर(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      परत 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    जर(!val->boolean)
      damagescreen(screen);
    थांबा;
  स्थिती VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    थांबा;
  पूर्वनिर्धारित:
    ; /* ignore */
  }

  जर(screen->callbacks && screen->callbacks->settermprop)
    परत (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  परत 1;
}

स्थिर पूर्णांक bell(शून्य *user)
{
  VTermScreen *screen = user;

  जर(screen->callbacks && screen->callbacks->bell)
    परत (*screen->callbacks->bell)(screen->cbdata);

  परत 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
स्थिर पूर्णांक ओळ_popcount(ScreenCell *buffer, पूर्णांक row, पूर्णांक rows UNUSED, पूर्णांक cols)
{
  पूर्णांक col = cols - 1;
  पर्यंत(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  परत col + 1;
}

#define REFLOW (screen->reflow)

स्थिर शून्य resize_buffer(VTermScreen *screen, पूर्णांक bufidx, पूर्णांक new_rows, पूर्णांक new_cols, पूर्णांक active, VTermStateFields *statefields)
{
  पूर्णांक old_rows = screen->rows;
  पूर्णांक old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTerm_ओळ_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, आकार(ScreenCell) * new_rows * new_cols);
  VTerm_ओळ_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, आकार(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  पूर्णांक old_row = old_rows - 1;
  पूर्णांक new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  ख_लिखो(मानक_त्रुटि, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  पूर्णांक final_blank_row = new_rows;

  पर्यंत(old_row >= 0) {
    पूर्णांक old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    पर्यंत(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    पूर्णांक old_row_start = old_row;

    पूर्णांक width = 0;
    साठी(पूर्णांक row = old_row_start; row <= old_row_end; row++) {
      जर(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      अन्यथा
        width += ओळ_popcount(old_buffer, row, old_rows, old_cols);
    }

    जर(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    पूर्णांक new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    पूर्णांक new_row_end = new_row;
    पूर्णांक new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    पूर्णांक old_col = 0;

    पूर्णांक spare_rows = new_rows - final_blank_row;

    जर(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      पूर्णांक downwards = -new_row_start;
      जर(downwards > spare_rows)
        downwards = spare_rows;
      पूर्णांक rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      ख_लिखो(मानक_त्रुटि, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      स_हटाओ(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * आकार(ScreenCell));
      स_हटाओ(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * आकार(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      जर(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    ख_लिखो(मानक_त्रुटि, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    जर(new_row_start < 0) {
      जर(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        जर(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      थांबा;
    }

    साठी(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      पूर्णांक मोजणी = width >= new_cols ? new_cols : width;
      width -= मोजणी;

      पूर्णांक new_col = 0;

      पर्यंत(मोजणी) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        जर(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        जर(old_col == old_cols) {
          old_row++;

          जर(!REFLOW) {
            new_col++;
            थांबा;
          }
          old_col = 0;
        }

        new_col++;
        मोजणी--;
      }

      जर(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        जर(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      पर्यंत(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  जर(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    जर(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  जर(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    जर (new_cursor.row < 0)
      new_cursor.row = 0;
    जर (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  जर(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    जर((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      साठी(पूर्णांक row = 0; row <= old_row; row++) {
        स्थिरांक VTerm_ओळ_Info *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    जर(active)
      statefields->pos.row -= (old_row + 1);
  }
  जर(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    पर्यंत(new_row >= 0) {
      VTermPos pos;
      जर(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        थांबा;

      pos.row = new_row;
      साठी(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        साठी(पूर्णांक i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          जर(!src->chars[i])
            थांबा;
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

        जर(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      साठी( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      जर(active)
        statefields->pos.row++;
    }
  }
  जर(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    पूर्णांक moverows = new_rows - new_row - 1;
    स_हटाओ(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * आकार(ScreenCell));
    स_हटाओ(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * आकार(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    साठी(new_row = moverows; new_row < new_rows; new_row++) {
      साठी(पूर्णांक col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTerm_ओळ_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  जर(active)
    statefields->pos = new_cursor;

  परत;
}

स्थिर पूर्णांक resize(पूर्णांक new_rows, पूर्णांक new_cols, VTermStateFields *fields, शून्य *user)
{
  VTermScreen *screen = user;

  पूर्णांक altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  पूर्णांक old_rows = screen->rows;
  पूर्णांक old_cols = screen->cols;

  जर(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    जर(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    जर (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, आकार(VTermScreenCell) * new_cols);
  }

  जर (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  जर(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  अन्यथा जर(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_ओळ_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, आकार(new_lineinfo[0]) * new_rows);
    साठी(पूर्णांक row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTerm_ओळ_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  जर(new_cols <= old_cols) {
    जर(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, आकार(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  जर(screen->callbacks && screen->callbacks->resize)
    परत (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  परत 1;
}

स्थिर पूर्णांक setlineinfo(पूर्णांक row, स्थिरांक VTerm_ओळ_Info *newinfo, स्थिरांक VTerm_ओळ_Info *oldinfo, शून्य *user)
{
  VTermScreen *screen = user;

  जर(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    साठी(पूर्णांक col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      जर (cell == रिक्त)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	परत 1;
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

    जर(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  परत 1;
}

स्थिर पूर्णांक sb_clear(शून्य *user) {
  VTermScreen *screen = user;

  जर(screen->callbacks && screen->callbacks->sb_clear)
    जर((*screen->callbacks->sb_clear)(screen->cbdata))
      परत 1;

  परत 0;
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
  जर(!state)
    परत रिक्त;

  VTermScreen *screen = vterm_allocator_malloc(vt, आकार(VTermScreen));
  जर (screen == रिक्त)
    परत रिक्त;
  पूर्णांक rows, cols;

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

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, आकार(VTermScreenCell) * cols);
  जर (screen->buffer == रिक्त || screen->sb_buffer == रिक्त)
  {
    vterm_screen_free(screen);
    परत रिक्त;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  परत screen;
}

INTERNAL शून्य vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  जर(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

शून्य vterm_screen_reset(VTermScreen *screen, पूर्णांक hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

स्थिर माप_प्रकार _get_chars(स्थिरांक VTermScreen *screen, स्थिरांक पूर्णांक utf8, शून्य *buffer, माप_प्रकार len, स्थिरांक VTermRect rect)
{
  माप_प्रकार outpos = 0;
  पूर्णांक padding = 0;

#define PUT(c)                                             \
  जर(utf8) {                                               \
    माप_प्रकार thislen = utf8_seqlen(c);                       \
    जर(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (अक्षर *)buffer + outpos);   \
    अन्यथा                                                   \
      outpos += thislen;                                   \
  }                                                        \
  अन्यथा {                                                   \
    जर(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    अन्यथा                                                   \
      outpos++;                                            \
  }

  साठी(पूर्णांक row = rect.start_row; row < rect.end_row; row++) {
    साठी(पूर्णांक col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      जर (cell == रिक्त)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	परत 1;
      }
      जर(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      अन्यथा जर(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      अन्यथा {
        पर्यंत(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        साठी(पूर्णांक i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    जर(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  परत outpos;
}

माप_प्रकार vterm_screen_get_chars(स्थिरांक VTermScreen *screen, uint32_t *chars, माप_प्रकार len, स्थिरांक VTermRect rect)
{
  परत _get_chars(screen, 0, chars, len, rect);
}

माप_प्रकार vterm_screen_get_text(स्थिरांक VTermScreen *screen, अक्षर *str, माप_प्रकार len, स्थिरांक VTermRect rect)
{
  परत _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
पूर्णांक vterm_screen_get_cell(स्थिरांक VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  जर(!intcell)
    परत 0;

  साठी(पूर्णांक i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    जर(!intcell->chars[i])
      थांबा;
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

  जर(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    जर(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      जर(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } अन्यथा जर(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } अन्यथा {
        cell->width = 2;
      }
    } अन्यथा
      cell->width = 1;
  } अन्यथा {
    जर(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    अन्यथा
      cell->width = 1;
  }

  परत 1;
}

पूर्णांक vterm_screen_is_eol(स्थिरांक VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  साठी(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    जर(cell->chars[0] != 0)
      परत 0;
  }

  परत 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  जर(vt->screen)
    परत vt->screen;

  vt->screen = screen_new(vt);
  परत vt->screen;
}

शून्य vterm_screen_enable_reflow(VTermScreen *screen, पूर्णांक reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

शून्य vterm_screen_enable_altscreen(VTermScreen *screen, पूर्णांक altscreen)
{
  जर(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    पूर्णांक rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

शून्य vterm_screen_set_callbacks(VTermScreen *screen, स्थिरांक VTermScreenCallbacks *callbacks, शून्य *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

शून्य *vterm_screen_get_cbdata(VTermScreen *screen)
{
  परत screen->cbdata;
}

शून्य vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

शून्य vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, स्थिरांक VTermStateFallbacks *fallbacks, शून्य *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

शून्य *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  परत vterm_state_get_unrecognised_fbdata(screen->state);
}

शून्य vterm_screen_flush_damage(VTermScreen *screen)
{
  जर(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  जर(screen->damaged.start_row != -1) {
    जर(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

शून्य vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

स्थिर पूर्णांक attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  जर((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    परत 1;
  जर((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    परत 1;
  जर((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    परत 1;
  जर((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    परत 1;
  जर((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    परत 1;
  जर((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    परत 1;
  जर((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    परत 1;
  जर((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    परत 1;
  जर((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    परत 1;
  जर((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    परत 1;
  जर((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    परत 1;
  जर((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    परत 1;

  परत 0;
}

पूर्णांक vterm_screen_get_attrs_extent(स्थिरांक VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  जर(extent->start_col < 0)
    extent->start_col = 0;
  जर(extent->end_col < 0)
    extent->end_col = screen->cols;

  पूर्णांक col;

  साठी(col = pos.col - 1; col >= extent->start_col; col--)
    जर(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      थांबा;
  extent->start_col = col + 1;

  साठी(col = pos.col + 1; col < extent->end_col; col++)
    जर(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      थांबा;
  extent->end_col = col - 1;

  परत 1;
}

शून्य vterm_screen_convert_color_to_rgb(स्थिरांक VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

स्थिर शून्य reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  साठी(पूर्णांक row = 0; row <= screen->rows - 1; row++)
    साठी(पूर्णांक col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      जर(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      जर(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

शून्य vterm_screen_set_default_colors(VTermScreen *screen, स्थिरांक VTermColor *default_fg, स्थिरांक VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  जर(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  जर(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  जर(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
