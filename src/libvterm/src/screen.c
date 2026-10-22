#include "vterm_internal.h"

// vim: set sw=2 :
#include <stdio.h>
#include <string.h>

#include "rect.h"
#include "utf8.h"

#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

#undef DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
typedef sa_nracanaa
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned ཧྲིལ bold      : 1;
  unsigned ཧྲིལ underline : 2;
  unsigned ཧྲིལ italic    : 1;
  unsigned ཧྲིལ blink     : 1;
  unsigned ཧྲིལ reverse   : 1;
  unsigned ཧྲིལ conceal   : 1;
  unsigned ཧྲིལ strike    : 1;
  unsigned ཧྲིལ font      : 4; /* 0 to 9 */
  unsigned ཧྲིལ small     : 1;
  unsigned ཧྲིལ baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned ཧྲིལ protected_cell : 1;
  unsigned ཧྲིལ dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned ཧྲིལ dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
typedef sa_nracanaa
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

sa_nracanaa VTermScreen
{
  VTerm *vt;
  VTermState *state;

  const VTermScreenCallbacks *callbacks;
  void *cbdata;
  ཧྲིལ callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  ཧྲིལ pending_scroll_downward, pending_scroll_rightward;

  ཧྲིལ rows;
  ཧྲིལ cols;

  unsigned ཧྲིལ global_reverse : 1;
  unsigned ཧྲིལ reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

static void clearcell(const VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

static ScreenCell *getcell(const VTermScreen *screen, ཧྲིལ row, ཧྲིལ col)
{
  གལ་སྲིད(row < 0 || row >= screen->rows)
    ལོག NULL;
  གལ་སྲིད(col < 0 || col >= screen->cols)
    ལོག NULL;
  གལ་སྲིད (screen->buffer == NULL)
    ལོག NULL;
  ལོག screen->buffer + (screen->cols * row) + col;
}

static ScreenCell *alloc_buffer(VTermScreen *screen, ཧྲིལ rows, ཧྲིལ cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(ScreenCell) * rows * cols);

  དོན་ལུ(ཧྲིལ row = 0; row < rows; row++) {
    དོན་ལུ(ཧྲིལ col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  ལོག new_buffer;
}

static void damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  switch(screen->damage_merge) {
  pak_sa VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    bha_nga;

  pak_sa VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    གལ་སྲིད(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    དེ་མིན གལ་སྲིད(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      ལོག;
    }
    དེ་མིན གལ་སྲིད(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      གལ་སྲིད(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      གལ་སྲིད(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      ལོག;
    }
    དེ་མིན {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    bha_nga;

  pak_sa VTERM_DAMAGE_SCREEN:
  pak_sa VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    གལ་སྲིད(screen->damaged.start_row == -1)
      screen->damaged = rect;
    དེ་མིན {
      rect_expand(&screen->damaged, &rect);
    }
    ལོག;

  default:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    ལོག;
  }

  གལ་སྲིད(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

static void damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

static ཧྲིལ putglyph(VTermGlyphInfo *info, VTermPos pos, void *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  གལ་སྲིད(!cell)
    ལོག 0;

  ཧྲིལ i;
  དོན་ལུ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  གལ་སྲིད(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  དོན་ལུ(ཧྲིལ col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    གལ་སྲིད (onecell == NULL)
      bha_nga;
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

  ལོག 1;
}

static void sb_pushline_from_row(VTermScreen *screen, ཧྲིལ row, ཧྲིལ continuation)
{
  VTermPos pos;
  pos.row = row;
  དོན་ལུ(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  གལ་སྲིད(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  དེ་མིན
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

static ཧྲིལ premove(VTermRect rect, void *user)
{
  VTermScreen *screen = user;

  གལ་སྲིད(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    དོན་ལུ(ཧྲིལ row = 0; row < rect.end_row; row++) {
      const VTermLineInfo *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  ལོག 1;
}

static ཧྲིལ moverect_internal(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  ཧྲིལ cols = src.end_col - src.start_col;
  ཧྲིལ downward = src.start_row - dest.start_row;

  ཧྲིལ init_row, test_row, inc_row;
  གལ་སྲིད(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  དེ་མིན {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  དོན་ལུ(ཧྲིལ row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * parimaa_na(ScreenCell));

  ལོག 1;
}

static ཧྲིལ moverect_user(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  གལ་སྲིད(screen->callbacks && screen->callbacks->moverect) {
    གལ་སྲིད(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    གལ་སྲིད((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      ལོག 1;
  }

  damagerect(screen, dest);

  ལོག 1;
}

static ཧྲིལ erase_internal(VTermRect rect, ཧྲིལ selective, void *user)
{
  VTermScreen *screen = user;

  དོན་ལུ(ཧྲིལ row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    const VTermLineInfo *info = vterm_state_get_lineinfo(screen->state, row);

    དོན་ལུ(ཧྲིལ col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      གལ་སྲིད (cell == NULL)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	ལོག 1;
      }
      གལ་སྲིད(selective && cell->pen.protected_cell)
        continue;

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

  ལོག 1;
}

static ཧྲིལ erase_user(VTermRect rect, ཧྲིལ selective UNUSED, void *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  ལོག 1;
}

static ཧྲིལ erase(VTermRect rect, ཧྲིལ selective, void *user)
{
  erase_internal(rect, selective, user);
  ལོག erase_user(rect, 0, user);
}

static ཧྲིལ scrollrect(VTermRect rect, ཧྲིལ downward, ཧྲིལ rightward, void *user)
{
  VTermScreen *screen = user;

  གལ་སྲིད(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    ལོག 1;
  }

  གལ་སྲིད(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  གལ་སྲིད(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  དེ་མིན གལ་སྲིད(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  དེ་མིན {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  གལ་སྲིད(screen->damaged.start_row == -1)
    ལོག 1;

  གལ་སྲིད(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  དེ་མིན གལ་སྲིད(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    གལ་སྲིད(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      གལ་སྲིད(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      གལ་སྲིད(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    གལ་སྲིད(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      གལ་སྲིད(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      གལ་སྲིད(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  དེ་མིན {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  ལོག 1;
}

static ཧྲིལ movecursor(VTermPos pos, VTermPos oldpos, ཧྲིལ visible, void *user)
{
  VTermScreen *screen = user;

  གལ་སྲིད(screen->callbacks && screen->callbacks->movecursor)
    ལོག (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  ལོག 0;
}

static ཧྲིལ setpenattr(VTermAttr attr, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(attr) {
  pak_sa VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    ལོག 1;
  pak_sa VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->number;
    ལོག 1;
  pak_sa VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    ལོག 1;
  pak_sa VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    ལོག 1;
  pak_sa VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    ལོག 1;
  pak_sa VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    ལོག 1;
  pak_sa VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    ལོག 1;
  pak_sa VTERM_ATTR_FONT:
    screen->pen.font = val->number;
    ལོག 1;
  pak_sa VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    ལོག 1;
  pak_sa VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    ལོག 1;
  pak_sa VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    ལོག 1;
  pak_sa VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->number;
    ལོག 1;

  pak_sa VTERM_N_ATTRS:
    ལོག 0;
  }

  ལོག 0;
}

static ཧྲིལ settermprop(VTermProp prop, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(prop) {
  pak_sa VTERM_PROP_ALTSCREEN:
    གལ་སྲིད(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      ལོག 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    གལ་སྲིད(!val->boolean)
      damagescreen(screen);
    bha_nga;
  pak_sa VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    bha_nga;
  default:
    ; /* ignore */
  }

  གལ་སྲིད(screen->callbacks && screen->callbacks->settermprop)
    ལོག (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  ལོག 1;
}

static ཧྲིལ bell(void *user)
{
  VTermScreen *screen = user;

  གལ་སྲིད(screen->callbacks && screen->callbacks->bell)
    ལོག (*screen->callbacks->bell)(screen->cbdata);

  ལོག 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
static ཧྲིལ line_popcount(ScreenCell *buffer, ཧྲིལ row, ཧྲིལ rows UNUSED, ཧྲིལ cols)
{
  ཧྲིལ col = cols - 1;
  སྐབས(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  ལོག col + 1;
}

#define REFLOW (screen->reflow)

static void resize_buffer(VTermScreen *screen, ཧྲིལ bufidx, ཧྲིལ new_rows, ཧྲིལ new_cols, ཧྲིལ active, VTermStateFields *statefields)
{
  ཧྲིལ old_rows = screen->rows;
  ཧྲིལ old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTermLineInfo *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(ScreenCell) * new_rows * new_cols);
  VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, parimaa_na(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  ཧྲིལ old_row = old_rows - 1;
  ཧྲིལ new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  ཧྲིལ final_blank_row = new_rows;

  སྐབས(old_row >= 0) {
    ཧྲིལ old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    སྐབས(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    ཧྲིལ old_row_start = old_row;

    ཧྲིལ width = 0;
    དོན་ལུ(ཧྲིལ row = old_row_start; row <= old_row_end; row++) {
      གལ་སྲིད(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      དེ་མིན
        width += line_popcount(old_buffer, row, old_rows, old_cols);
    }

    གལ་སྲིད(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    ཧྲིལ new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    ཧྲིལ new_row_end = new_row;
    ཧྲིལ new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    ཧྲིལ old_col = 0;

    ཧྲིལ spare_rows = new_rows - final_blank_row;

    གལ་སྲིད(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      ཧྲིལ downwards = -new_row_start;
      གལ་སྲིད(downwards > spare_rows)
        downwards = spare_rows;
      ཧྲིལ rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * parimaa_na(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * parimaa_na(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      གལ་སྲིད(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    གལ་སྲིད(new_row_start < 0) {
      གལ་སྲིད(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        གལ་སྲིད(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      bha_nga;
    }

    དོན་ལུ(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      ཧྲིལ count = width >= new_cols ? new_cols : width;
      width -= count;

      ཧྲིལ new_col = 0;

      སྐབས(count) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        གལ་སྲིད(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        གལ་སྲིད(old_col == old_cols) {
          old_row++;

          གལ་སྲིད(!REFLOW) {
            new_col++;
            bha_nga;
          }
          old_col = 0;
        }

        new_col++;
        count--;
      }

      གལ་སྲིད(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        གལ་སྲིད(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      སྐབས(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  གལ་སྲིད(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    གལ་སྲིད(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  གལ་སྲིད(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    གལ་སྲིད (new_cursor.row < 0)
      new_cursor.row = 0;
    གལ་སྲིད (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  གལ་སྲིད(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    གལ་སྲིད((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      དོན་ལུ(ཧྲིལ row = 0; row <= old_row; row++) {
        const VTermLineInfo *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    གལ་སྲིད(active)
      statefields->pos.row -= (old_row + 1);
  }
  གལ་སྲིད(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    སྐབས(new_row >= 0) {
      VTermPos pos;
      གལ་སྲིད(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        bha_nga;

      pos.row = new_row;
      དོན་ལུ(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        དོན་ལུ(ཧྲིལ i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          གལ་སྲིད(!src->chars[i])
            bha_nga;
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

        གལ་སྲིད(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      དོན་ལུ( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      གལ་སྲིད(active)
        statefields->pos.row++;
    }
  }
  གལ་སྲིད(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    ཧྲིལ moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * parimaa_na(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * parimaa_na(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    དོན་ལུ(new_row = moverows; new_row < new_rows; new_row++) {
      དོན་ལུ(ཧྲིལ col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTermLineInfo){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  གལ་སྲིད(active)
    statefields->pos = new_cursor;

  ལོག;
}

static ཧྲིལ resize(ཧྲིལ new_rows, ཧྲིལ new_cols, VTermStateFields *fields, void *user)
{
  VTermScreen *screen = user;

  ཧྲིལ altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  ཧྲིལ old_rows = screen->rows;
  ཧྲིལ old_cols = screen->cols;

  གལ་སྲིད(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    གལ་སྲིད(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    གལ་སྲིད (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(VTermScreenCell) * new_cols);
  }

  གལ་སྲིད (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  གལ་སྲིད(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  དེ་མིན གལ་སྲིད(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, parimaa_na(new_lineinfo[0]) * new_rows);
    དོན་ལུ(ཧྲིལ row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTermLineInfo){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  གལ་སྲིད(new_cols <= old_cols) {
    གལ་སྲིད(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  གལ་སྲིད(screen->callbacks && screen->callbacks->resize)
    ལོག (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  ལོག 1;
}

static ཧྲིལ setlineinfo(ཧྲིལ row, const VTermLineInfo *newinfo, const VTermLineInfo *oldinfo, void *user)
{
  VTermScreen *screen = user;

  གལ་སྲིད(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    དོན་ལུ(ཧྲིལ col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      གལ་སྲིད (cell == NULL)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	ལོག 1;
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

    གལ་སྲིད(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  ལོག 1;
}

static ཧྲིལ sb_clear(void *user) {
  VTermScreen *screen = user;

  གལ་སྲིད(screen->callbacks && screen->callbacks->sb_clear)
    གལ་སྲིད((*screen->callbacks->sb_clear)(screen->cbdata))
      ལོག 1;

  ལོག 0;
}

static VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  NULL, // moverect
  &erase, // erase
  NULL, // initpen
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
static VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  གལ་སྲིད(!state)
    ལོག NULL;

  VTermScreen *screen = vterm_allocator_malloc(vt, parimaa_na(VTermScreen));
  གལ་སྲིད (screen == NULL)
    ལོག NULL;
  ཧྲིལ rows, cols;

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

  screen->callbacks = NULL;
  screen->cbdata    = NULL;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_buffer(screen, rows, cols);

  screen->buffer = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(VTermScreenCell) * cols);
  གལ་སྲིད (screen->buffer == NULL || screen->sb_buffer == NULL)
  {
    vterm_screen_free(screen);
    ལོག NULL;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  ལོག screen;
}

INTERNAL void vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  གལ་སྲིད(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

void vterm_screen_reset(VTermScreen *screen, ཧྲིལ hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

static size_t _get_chars(const VTermScreen *screen, const ཧྲིལ utf8, void *buffer, size_t len, const VTermRect rect)
{
  size_t outpos = 0;
  ཧྲིལ padding = 0;

#define PUT(c)                                             \
  གལ་སྲིད(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    གལ་སྲིད(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (var_na *)buffer + outpos);   \
    དེ་མིན                                                   \
      outpos += thislen;                                   \
  }                                                        \
  དེ་མིན {                                                   \
    གལ་སྲིད(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    དེ་མིན                                                   \
      outpos++;                                            \
  }

  དོན་ལུ(ཧྲིལ row = rect.start_row; row < rect.end_row; row++) {
    དོན་ལུ(ཧྲིལ col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      གལ་སྲིད (cell == NULL)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	ལོག 1;
      }
      གལ་སྲིད(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      དེ་མིན གལ་སྲིད(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      དེ་མིན {
        སྐབས(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        དོན་ལུ(ཧྲིལ i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    གལ་སྲིད(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  ལོག outpos;
}

size_t vterm_screen_get_chars(const VTermScreen *screen, uint32_t *chars, size_t len, const VTermRect rect)
{
  ལོག _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(const VTermScreen *screen, var_na *str, size_t len, const VTermRect rect)
{
  ལོག _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
ཧྲིལ vterm_screen_get_cell(const VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  གལ་སྲིད(!intcell)
    ལོག 0;

  དོན་ལུ(ཧྲིལ i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    གལ་སྲིད(!intcell->chars[i])
      bha_nga;
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

  གལ་སྲིད(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    གལ་སྲིད(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      གལ་སྲིད(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } དེ་མིན གལ་སྲིད(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } དེ་མིན {
        cell->width = 2;
      }
    } དེ་མིན
      cell->width = 1;
  } དེ་མིན {
    གལ་སྲིད(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    དེ་མིན
      cell->width = 1;
  }

  ལོག 1;
}

ཧྲིལ vterm_screen_is_eol(const VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  དོན་ལུ(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    གལ་སྲིད(cell->chars[0] != 0)
      ལོག 0;
  }

  ལོག 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  གལ་སྲིད(vt->screen)
    ལོག vt->screen;

  vt->screen = screen_new(vt);
  ལོག vt->screen;
}

void vterm_screen_enable_reflow(VTermScreen *screen, ཧྲིལ reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

void vterm_screen_enable_altscreen(VTermScreen *screen, ཧྲིལ altscreen)
{
  གལ་སྲིད(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    ཧྲིལ rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

void vterm_screen_set_callbacks(VTermScreen *screen, const VTermScreenCallbacks *callbacks, void *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

void *vterm_screen_get_cbdata(VTermScreen *screen)
{
  ལོག screen->cbdata;
}

void vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

void vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, const VTermStateFallbacks *fallbacks, void *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

void *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  ལོག vterm_state_get_unrecognised_fbdata(screen->state);
}

void vterm_screen_flush_damage(VTermScreen *screen)
{
  གལ་སྲིད(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  གལ་སྲིད(screen->damaged.start_row != -1) {
    གལ་སྲིད(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

void vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

static ཧྲིལ attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  གལ་སྲིད((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    ལོག 1;
  གལ་སྲིད((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    ལོག 1;

  ལོག 0;
}

ཧྲིལ vterm_screen_get_attrs_extent(const VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  གལ་སྲིད(extent->start_col < 0)
    extent->start_col = 0;
  གལ་སྲིད(extent->end_col < 0)
    extent->end_col = screen->cols;

  ཧྲིལ col;

  དོན་ལུ(col = pos.col - 1; col >= extent->start_col; col--)
    གལ་སྲིད(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      bha_nga;
  extent->start_col = col + 1;

  དོན་ལུ(col = pos.col + 1; col < extent->end_col; col++)
    གལ་སྲིད(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      bha_nga;
  extent->end_col = col - 1;

  ལོག 1;
}

void vterm_screen_convert_color_to_rgb(const VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

static void reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  དོན་ལུ(ཧྲིལ row = 0; row <= screen->rows - 1; row++)
    དོན་ལུ(ཧྲིལ col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      གལ་སྲིད(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      གལ་སྲིད(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

void vterm_screen_set_default_colors(VTermScreen *screen, const VTermColor *default_fg, const VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  གལ་སྲིད(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  གལ་སྲིད(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  གལ་སྲིད(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
