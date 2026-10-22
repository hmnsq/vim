#અન્તર્ભાવ "vterm_internal.h"

// vim: set sw=2 :
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "rect.h"
#અન્તર્ભાવ "utf8.h"

#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

#undef DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
પ્રકારવ્યાખ્યા સંરચના
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned પૂર્ણાંક bold      : 1;
  unsigned પૂર્ણાંક underline : 2;
  unsigned પૂર્ણાંક italic    : 1;
  unsigned પૂર્ણાંક blink     : 1;
  unsigned પૂર્ણાંક reverse   : 1;
  unsigned પૂર્ણાંક conceal   : 1;
  unsigned પૂર્ણાંક strike    : 1;
  unsigned પૂર્ણાંક font      : 4; /* 0 to 9 */
  unsigned પૂર્ણાંક small     : 1;
  unsigned પૂર્ણાંક baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned પૂર્ણાંક protected_cell : 1;
  unsigned પૂર્ણાંક dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned પૂર્ણાંક dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
પ્રકારવ્યાખ્યા સંરચના
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

સંરચના VTermScreen
{
  VTerm *vt;
  VTermState *state;

  અચલ VTermScreenCallbacks *callbacks;
  શૂન્ય *cbdata;
  પૂર્ણાંક callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  પૂર્ણાંક pending_scroll_downward, pending_scroll_rightward;

  પૂર્ણાંક rows;
  પૂર્ણાંક cols;

  unsigned પૂર્ણાંક global_reverse : 1;
  unsigned પૂર્ણાંક reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

સ્થિર શૂન્ય clearcell(અચલ VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

સ્થિર ScreenCell *getcell(અચલ VTermScreen *screen, પૂર્ણાંક row, પૂર્ણાંક col)
{
  જો(row < 0 || row >= screen->rows)
    પાછા ખાલી;
  જો(col < 0 || col >= screen->cols)
    પાછા ખાલી;
  જો (screen->buffer == ખાલી)
    પાછા ખાલી;
  પાછા screen->buffer + (screen->cols * row) + col;
}

સ્થિર ScreenCell *alloc_buffer(VTermScreen *screen, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, કદ(ScreenCell) * rows * cols);

  માટે(પૂર્ણાંક row = 0; row < rows; row++) {
    માટે(પૂર્ણાંક col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  પાછા new_buffer;
}

સ્થિર શૂન્ય damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  પસંદગી(screen->damage_merge) {
  સ્થિતિ VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    તોડો;

  સ્થિતિ VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    જો(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    નહીંતર જો(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      પાછા;
    }
    નહીંતર જો(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      જો(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      જો(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      પાછા;
    }
    નહીંતર {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    તોડો;

  સ્થિતિ VTERM_DAMAGE_SCREEN:
  સ્થિતિ VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    જો(screen->damaged.start_row == -1)
      screen->damaged = rect;
    નહીંતર {
      rect_expand(&screen->damaged, &rect);
    }
    પાછા;

  મૂળભૂત:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    પાછા;
  }

  જો(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

સ્થિર શૂન્ય damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

સ્થિર પૂર્ણાંક putglyph(VTermGlyphInfo *info, VTermPos pos, શૂન્ય *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  જો(!cell)
    પાછા 0;

  પૂર્ણાંક i;
  માટે(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  જો(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  માટે(પૂર્ણાંક col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    જો (onecell == ખાલી)
      તોડો;
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

  પાછા 1;
}

સ્થિર શૂન્ય sb_pushline_from_row(VTermScreen *screen, પૂર્ણાંક row, પૂર્ણાંક continuation)
{
  VTermPos pos;
  pos.row = row;
  માટે(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  જો(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  નહીંતર
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

સ્થિર પૂર્ણાંક premove(VTermRect rect, શૂન્ય *user)
{
  VTermScreen *screen = user;

  જો(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    માટે(પૂર્ણાંક row = 0; row < rect.end_row; row++) {
      અચલ VTerm_લીટી_Info *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  પાછા 1;
}

સ્થિર પૂર્ણાંક moverect_internal(VTermRect dest, VTermRect src, શૂન્ય *user)
{
  VTermScreen *screen = user;

  પૂર્ણાંક cols = src.end_col - src.start_col;
  પૂર્ણાંક downward = src.start_row - dest.start_row;

  પૂર્ણાંક init_row, test_row, inc_row;
  જો(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  નહીંતર {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  માટે(પૂર્ણાંક row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * કદ(ScreenCell));

  પાછા 1;
}

સ્થિર પૂર્ણાંક moverect_user(VTermRect dest, VTermRect src, શૂન્ય *user)
{
  VTermScreen *screen = user;

  જો(screen->callbacks && screen->callbacks->moverect) {
    જો(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    જો((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      પાછા 1;
  }

  damagerect(screen, dest);

  પાછા 1;
}

સ્થિર પૂર્ણાંક erase_internal(VTermRect rect, પૂર્ણાંક selective, શૂન્ય *user)
{
  VTermScreen *screen = user;

  માટે(પૂર્ણાંક row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    અચલ VTerm_લીટી_Info *info = vterm_state_get_lineinfo(screen->state, row);

    માટે(પૂર્ણાંક col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      જો (cell == ખાલી)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	પાછા 1;
      }
      જો(selective && cell->pen.protected_cell)
        ચાલુ;

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

  પાછા 1;
}

સ્થિર પૂર્ણાંક erase_user(VTermRect rect, પૂર્ણાંક selective UNUSED, શૂન્ય *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  પાછા 1;
}

સ્થિર પૂર્ણાંક erase(VTermRect rect, પૂર્ણાંક selective, શૂન્ય *user)
{
  erase_internal(rect, selective, user);
  પાછા erase_user(rect, 0, user);
}

સ્થિર પૂર્ણાંક scrollrect(VTermRect rect, પૂર્ણાંક downward, પૂર્ણાંક rightward, શૂન્ય *user)
{
  VTermScreen *screen = user;

  જો(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    પાછા 1;
  }

  જો(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  જો(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  નહીંતર જો(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  નહીંતર {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  જો(screen->damaged.start_row == -1)
    પાછા 1;

  જો(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  નહીંતર જો(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    જો(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      જો(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      જો(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    જો(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      જો(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      જો(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  નહીંતર {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  પાછા 1;
}

સ્થિર પૂર્ણાંક movecursor(VTermPos pos, VTermPos oldpos, પૂર્ણાંક visible, શૂન્ય *user)
{
  VTermScreen *screen = user;

  જો(screen->callbacks && screen->callbacks->movecursor)
    પાછા (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  પાછા 0;
}

સ્થિર પૂર્ણાંક setpenattr(VTermAttr attr, VTerm_કિંમત *val, શૂન્ય *user)
{
  VTermScreen *screen = user;

  પસંદગી(attr) {
  સ્થિતિ VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->સંખ્યા;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_FONT:
    screen->pen.font = val->સંખ્યા;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    પાછા 1;
  સ્થિતિ VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->સંખ્યા;
    પાછા 1;

  સ્થિતિ VTERM_N_ATTRS:
    પાછા 0;
  }

  પાછા 0;
}

સ્થિર પૂર્ણાંક settermprop(VTermProp prop, VTerm_કિંમત *val, શૂન્ય *user)
{
  VTermScreen *screen = user;

  પસંદગી(prop) {
  સ્થિતિ VTERM_PROP_ALTSCREEN:
    જો(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      પાછા 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    જો(!val->boolean)
      damagescreen(screen);
    તોડો;
  સ્થિતિ VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    તોડો;
  મૂળભૂત:
    ; /* ignore */
  }

  જો(screen->callbacks && screen->callbacks->settermprop)
    પાછા (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  પાછા 1;
}

સ્થિર પૂર્ણાંક bell(શૂન્ય *user)
{
  VTermScreen *screen = user;

  જો(screen->callbacks && screen->callbacks->bell)
    પાછા (*screen->callbacks->bell)(screen->cbdata);

  પાછા 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
સ્થિર પૂર્ણાંક લીટી_popcount(ScreenCell *buffer, પૂર્ણાંક row, પૂર્ણાંક rows UNUSED, પૂર્ણાંક cols)
{
  પૂર્ણાંક col = cols - 1;
  જ્યારે(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  પાછા col + 1;
}

#define REFLOW (screen->reflow)

સ્થિર શૂન્ય resize_buffer(VTermScreen *screen, પૂર્ણાંક bufidx, પૂર્ણાંક new_rows, પૂર્ણાંક new_cols, પૂર્ણાંક active, VTermStateFields *statefields)
{
  પૂર્ણાંક old_rows = screen->rows;
  પૂર્ણાંક old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTerm_લીટી_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, કદ(ScreenCell) * new_rows * new_cols);
  VTerm_લીટી_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, કદ(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  પૂર્ણાંક old_row = old_rows - 1;
  પૂર્ણાંક new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  પૂર્ણાંક final_blank_row = new_rows;

  જ્યારે(old_row >= 0) {
    પૂર્ણાંક old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    જ્યારે(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    પૂર્ણાંક old_row_start = old_row;

    પૂર્ણાંક width = 0;
    માટે(પૂર્ણાંક row = old_row_start; row <= old_row_end; row++) {
      જો(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      નહીંતર
        width += લીટી_popcount(old_buffer, row, old_rows, old_cols);
    }

    જો(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    પૂર્ણાંક new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    પૂર્ણાંક new_row_end = new_row;
    પૂર્ણાંક new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    પૂર્ણાંક old_col = 0;

    પૂર્ણાંક spare_rows = new_rows - final_blank_row;

    જો(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      પૂર્ણાંક downwards = -new_row_start;
      જો(downwards > spare_rows)
        downwards = spare_rows;
      પૂર્ણાંક rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * કદ(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * કદ(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      જો(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    જો(new_row_start < 0) {
      જો(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        જો(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      તોડો;
    }

    માટે(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      પૂર્ણાંક ગણતરી = width >= new_cols ? new_cols : width;
      width -= ગણતરી;

      પૂર્ણાંક new_col = 0;

      જ્યારે(ગણતરી) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        જો(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        જો(old_col == old_cols) {
          old_row++;

          જો(!REFLOW) {
            new_col++;
            તોડો;
          }
          old_col = 0;
        }

        new_col++;
        ગણતરી--;
      }

      જો(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        જો(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      જ્યારે(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  જો(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    જો(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  જો(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    જો (new_cursor.row < 0)
      new_cursor.row = 0;
    જો (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  જો(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    જો((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      માટે(પૂર્ણાંક row = 0; row <= old_row; row++) {
        અચલ VTerm_લીટી_Info *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    જો(active)
      statefields->pos.row -= (old_row + 1);
  }
  જો(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    જ્યારે(new_row >= 0) {
      VTermPos pos;
      જો(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        તોડો;

      pos.row = new_row;
      માટે(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        માટે(પૂર્ણાંક i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          જો(!src->chars[i])
            તોડો;
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

        જો(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      માટે( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      જો(active)
        statefields->pos.row++;
    }
  }
  જો(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    પૂર્ણાંક moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * કદ(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * કદ(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    માટે(new_row = moverows; new_row < new_rows; new_row++) {
      માટે(પૂર્ણાંક col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTerm_લીટી_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  જો(active)
    statefields->pos = new_cursor;

  પાછા;
}

સ્થિર પૂર્ણાંક resize(પૂર્ણાંક new_rows, પૂર્ણાંક new_cols, VTermStateFields *fields, શૂન્ય *user)
{
  VTermScreen *screen = user;

  પૂર્ણાંક altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  પૂર્ણાંક old_rows = screen->rows;
  પૂર્ણાંક old_cols = screen->cols;

  જો(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    જો(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    જો (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, કદ(VTermScreenCell) * new_cols);
  }

  જો (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  જો(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  નહીંતર જો(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_લીટી_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, કદ(new_lineinfo[0]) * new_rows);
    માટે(પૂર્ણાંક row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTerm_લીટી_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  જો(new_cols <= old_cols) {
    જો(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, કદ(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  જો(screen->callbacks && screen->callbacks->resize)
    પાછા (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  પાછા 1;
}

સ્થિર પૂર્ણાંક setlineinfo(પૂર્ણાંક row, અચલ VTerm_લીટી_Info *newinfo, અચલ VTerm_લીટી_Info *oldinfo, શૂન્ય *user)
{
  VTermScreen *screen = user;

  જો(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    માટે(પૂર્ણાંક col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      જો (cell == ખાલી)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	પાછા 1;
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

    જો(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  પાછા 1;
}

સ્થિર પૂર્ણાંક sb_clear(શૂન્ય *user) {
  VTermScreen *screen = user;

  જો(screen->callbacks && screen->callbacks->sb_clear)
    જો((*screen->callbacks->sb_clear)(screen->cbdata))
      પાછા 1;

  પાછા 0;
}

સ્થિર VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  ખાલી, // moverect
  &erase, // erase
  ખાલી, // initpen
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
સ્થિર VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  જો(!state)
    પાછા ખાલી;

  VTermScreen *screen = vterm_allocator_malloc(vt, કદ(VTermScreen));
  જો (screen == ખાલી)
    પાછા ખાલી;
  પૂર્ણાંક rows, cols;

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

  screen->callbacks = ખાલી;
  screen->cbdata    = ખાલી;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_buffer(screen, rows, cols);

  screen->buffer = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, કદ(VTermScreenCell) * cols);
  જો (screen->buffer == ખાલી || screen->sb_buffer == ખાલી)
  {
    vterm_screen_free(screen);
    પાછા ખાલી;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  પાછા screen;
}

INTERNAL શૂન્ય vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  જો(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

શૂન્ય vterm_screen_reset(VTermScreen *screen, પૂર્ણાંક hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

સ્થિર size_t _get_chars(અચલ VTermScreen *screen, અચલ પૂર્ણાંક utf8, શૂન્ય *buffer, size_t len, અચલ VTermRect rect)
{
  size_t outpos = 0;
  પૂર્ણાંક padding = 0;

#define PUT(c)                                             \
  જો(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    જો(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (અક્ષર *)buffer + outpos);   \
    નહીંતર                                                   \
      outpos += thislen;                                   \
  }                                                        \
  નહીંતર {                                                   \
    જો(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    નહીંતર                                                   \
      outpos++;                                            \
  }

  માટે(પૂર્ણાંક row = rect.start_row; row < rect.end_row; row++) {
    માટે(પૂર્ણાંક col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      જો (cell == ખાલી)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	પાછા 1;
      }
      જો(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      નહીંતર જો(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      નહીંતર {
        જ્યારે(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        માટે(પૂર્ણાંક i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    જો(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  પાછા outpos;
}

size_t vterm_screen_get_chars(અચલ VTermScreen *screen, uint32_t *chars, size_t len, અચલ VTermRect rect)
{
  પાછા _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(અચલ VTermScreen *screen, અક્ષર *str, size_t len, અચલ VTermRect rect)
{
  પાછા _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
પૂર્ણાંક vterm_screen_get_cell(અચલ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  જો(!intcell)
    પાછા 0;

  માટે(પૂર્ણાંક i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    જો(!intcell->chars[i])
      તોડો;
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

  જો(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    જો(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      જો(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } નહીંતર જો(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } નહીંતર {
        cell->width = 2;
      }
    } નહીંતર
      cell->width = 1;
  } નહીંતર {
    જો(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    નહીંતર
      cell->width = 1;
  }

  પાછા 1;
}

પૂર્ણાંક vterm_screen_is_eol(અચલ VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  માટે(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    જો(cell->chars[0] != 0)
      પાછા 0;
  }

  પાછા 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  જો(vt->screen)
    પાછા vt->screen;

  vt->screen = screen_new(vt);
  પાછા vt->screen;
}

શૂન્ય vterm_screen_enable_reflow(VTermScreen *screen, પૂર્ણાંક reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

શૂન્ય vterm_screen_enable_altscreen(VTermScreen *screen, પૂર્ણાંક altscreen)
{
  જો(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    પૂર્ણાંક rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

શૂન્ય vterm_screen_set_callbacks(VTermScreen *screen, અચલ VTermScreenCallbacks *callbacks, શૂન્ય *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

શૂન્ય *vterm_screen_get_cbdata(VTermScreen *screen)
{
  પાછા screen->cbdata;
}

શૂન્ય vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

શૂન્ય vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, અચલ VTermStateFallbacks *fallbacks, શૂન્ય *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

શૂન્ય *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  પાછા vterm_state_get_unrecognised_fbdata(screen->state);
}

શૂન્ય vterm_screen_flush_damage(VTermScreen *screen)
{
  જો(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  જો(screen->damaged.start_row != -1) {
    જો(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

શૂન્ય vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

સ્થિર પૂર્ણાંક attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  જો((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    પાછા 1;
  જો((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    પાછા 1;
  જો((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    પાછા 1;
  જો((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    પાછા 1;
  જો((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    પાછા 1;
  જો((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    પાછા 1;
  જો((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    પાછા 1;
  જો((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    પાછા 1;
  જો((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    પાછા 1;
  જો((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    પાછા 1;
  જો((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    પાછા 1;
  જો((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    પાછા 1;

  પાછા 0;
}

પૂર્ણાંક vterm_screen_get_attrs_extent(અચલ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  જો(extent->start_col < 0)
    extent->start_col = 0;
  જો(extent->end_col < 0)
    extent->end_col = screen->cols;

  પૂર્ણાંક col;

  માટે(col = pos.col - 1; col >= extent->start_col; col--)
    જો(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      તોડો;
  extent->start_col = col + 1;

  માટે(col = pos.col + 1; col < extent->end_col; col++)
    જો(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      તોડો;
  extent->end_col = col - 1;

  પાછા 1;
}

શૂન્ય vterm_screen_convert_color_to_rgb(અચલ VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

સ્થિર શૂન્ય reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  માટે(પૂર્ણાંક row = 0; row <= screen->rows - 1; row++)
    માટે(પૂર્ણાંક col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      જો(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      જો(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

શૂન્ય vterm_screen_set_default_colors(VTermScreen *screen, અચલ VTermColor *default_fg, અચલ VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  જો(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  જો(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  જો(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
