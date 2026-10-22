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
typedef struct
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned цяло bold      : 1;
  unsigned цяло underline : 2;
  unsigned цяло italic    : 1;
  unsigned цяло blink     : 1;
  unsigned цяло reverse   : 1;
  unsigned цяло conceal   : 1;
  unsigned цяло strike    : 1;
  unsigned цяло font      : 4; /* 0 to 9 */
  unsigned цяло small     : 1;
  unsigned цяло baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned цяло protected_cell : 1;
  unsigned цяло dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned цяло dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
typedef struct
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

struct VTermScreen
{
  VTerm *vt;
  VTermState *state;

  const VTermScreenCallbacks *callbacks;
  void *cbdata;
  цяло callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  цяло pending_scroll_downward, pending_scroll_rightward;

  цяло rows;
  цяло cols;

  unsigned цяло global_reverse : 1;
  unsigned цяло reflow : 1;

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

static ScreenCell *getcell(const VTermScreen *screen, цяло row, цяло col)
{
  ако(row < 0 || row >= screen->rows)
    върни NULL;
  ако(col < 0 || col >= screen->cols)
    върни NULL;
  ако (screen->buffer == NULL)
    върни NULL;
  върни screen->buffer + (screen->cols * row) + col;
}

static ScreenCell *alloc_buffer(VTermScreen *screen, цяло rows, цяло cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, sizeof(ScreenCell) * rows * cols);

  за(цяло row = 0; row < rows; row++) {
    за(цяло col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  върни new_buffer;
}

static void damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  switch(screen->damage_merge) {
  case VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    break;

  case VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    ако(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    иначе ако(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      върни;
    }
    иначе ако(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      ако(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      ако(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      върни;
    }
    иначе {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    break;

  case VTERM_DAMAGE_SCREEN:
  case VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    ако(screen->damaged.start_row == -1)
      screen->damaged = rect;
    иначе {
      rect_expand(&screen->damaged, &rect);
    }
    върни;

  default:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    върни;
  }

  ако(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

static void damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

static цяло putglyph(VTermGlyphInfo *info, VTermPos pos, void *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  ако(!cell)
    върни 0;

  цяло i;
  за(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  ако(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  за(цяло col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    ако (onecell == NULL)
      break;
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

  върни 1;
}

static void sb_pushline_from_row(VTermScreen *screen, цяло row, цяло continuation)
{
  VTermPos pos;
  pos.row = row;
  за(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  ако(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  иначе
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

static цяло premove(VTermRect rect, void *user)
{
  VTermScreen *screen = user;

  ако(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    за(цяло row = 0; row < rect.end_row; row++) {
      const VTermLineInfo *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  върни 1;
}

static цяло moverect_internal(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  цяло cols = src.end_col - src.start_col;
  цяло downward = src.start_row - dest.start_row;

  цяло init_row, test_row, inc_row;
  ако(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  иначе {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  за(цяло row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * sizeof(ScreenCell));

  върни 1;
}

static цяло moverect_user(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  ако(screen->callbacks && screen->callbacks->moverect) {
    ако(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    ако((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      върни 1;
  }

  damagerect(screen, dest);

  върни 1;
}

static цяло erase_internal(VTermRect rect, цяло selective, void *user)
{
  VTermScreen *screen = user;

  за(цяло row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    const VTermLineInfo *info = vterm_state_get_lineinfo(screen->state, row);

    за(цяло col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      ако (cell == NULL)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	върни 1;
      }
      ако(selective && cell->pen.protected_cell)
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

  върни 1;
}

static цяло erase_user(VTermRect rect, цяло selective UNUSED, void *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  върни 1;
}

static цяло erase(VTermRect rect, цяло selective, void *user)
{
  erase_internal(rect, selective, user);
  върни erase_user(rect, 0, user);
}

static цяло scrollrect(VTermRect rect, цяло downward, цяло rightward, void *user)
{
  VTermScreen *screen = user;

  ако(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    върни 1;
  }

  ако(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  ако(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  иначе ако(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  иначе {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  ако(screen->damaged.start_row == -1)
    върни 1;

  ако(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  иначе ако(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    ако(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      ако(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      ако(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    ако(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      ако(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      ако(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  иначе {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  върни 1;
}

static цяло movecursor(VTermPos pos, VTermPos oldpos, цяло visible, void *user)
{
  VTermScreen *screen = user;

  ако(screen->callbacks && screen->callbacks->movecursor)
    върни (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  върни 0;
}

static цяло setpenattr(VTermAttr attr, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(attr) {
  case VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    върни 1;
  case VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->number;
    върни 1;
  case VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    върни 1;
  case VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    върни 1;
  case VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    върни 1;
  case VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    върни 1;
  case VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    върни 1;
  case VTERM_ATTR_FONT:
    screen->pen.font = val->number;
    върни 1;
  case VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    върни 1;
  case VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    върни 1;
  case VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    върни 1;
  case VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->number;
    върни 1;

  case VTERM_N_ATTRS:
    върни 0;
  }

  върни 0;
}

static цяло settermprop(VTermProp prop, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(prop) {
  case VTERM_PROP_ALTSCREEN:
    ако(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      върни 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    ако(!val->boolean)
      damagescreen(screen);
    break;
  case VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    break;
  default:
    ; /* ignore */
  }

  ако(screen->callbacks && screen->callbacks->settermprop)
    върни (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  върни 1;
}

static цяло bell(void *user)
{
  VTermScreen *screen = user;

  ако(screen->callbacks && screen->callbacks->bell)
    върни (*screen->callbacks->bell)(screen->cbdata);

  върни 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
static цяло line_popcount(ScreenCell *buffer, цяло row, цяло rows UNUSED, цяло cols)
{
  цяло col = cols - 1;
  докато(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  върни col + 1;
}

#define REFLOW (screen->reflow)

static void resize_buffer(VTermScreen *screen, цяло bufidx, цяло new_rows, цяло new_cols, цяло active, VTermStateFields *statefields)
{
  цяло old_rows = screen->rows;
  цяло old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTermLineInfo *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, sizeof(ScreenCell) * new_rows * new_cols);
  VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, sizeof(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  цяло old_row = old_rows - 1;
  цяло new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  цяло final_blank_row = new_rows;

  докато(old_row >= 0) {
    цяло old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    докато(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    цяло old_row_start = old_row;

    цяло width = 0;
    за(цяло row = old_row_start; row <= old_row_end; row++) {
      ако(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      иначе
        width += line_popcount(old_buffer, row, old_rows, old_cols);
    }

    ако(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    цяло new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    цяло new_row_end = new_row;
    цяло new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    цяло old_col = 0;

    цяло spare_rows = new_rows - final_blank_row;

    ако(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      цяло downwards = -new_row_start;
      ако(downwards > spare_rows)
        downwards = spare_rows;
      цяло rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * sizeof(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * sizeof(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      ако(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    ако(new_row_start < 0) {
      ако(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        ако(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      break;
    }

    за(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      цяло count = width >= new_cols ? new_cols : width;
      width -= count;

      цяло new_col = 0;

      докато(count) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        ако(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        ако(old_col == old_cols) {
          old_row++;

          ако(!REFLOW) {
            new_col++;
            break;
          }
          old_col = 0;
        }

        new_col++;
        count--;
      }

      ако(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        ако(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      докато(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  ако(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    ако(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  ако(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    ако (new_cursor.row < 0)
      new_cursor.row = 0;
    ако (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  ако(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    ако((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      за(цяло row = 0; row <= old_row; row++) {
        const VTermLineInfo *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    ако(active)
      statefields->pos.row -= (old_row + 1);
  }
  ако(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    докато(new_row >= 0) {
      VTermPos pos;
      ако(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        break;

      pos.row = new_row;
      за(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        за(цяло i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          ако(!src->chars[i])
            break;
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

        ако(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      за( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      ако(active)
        statefields->pos.row++;
    }
  }
  ако(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    цяло moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * sizeof(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * sizeof(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    за(new_row = moverows; new_row < new_rows; new_row++) {
      за(цяло col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTermLineInfo){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  ако(active)
    statefields->pos = new_cursor;

  върни;
}

static цяло resize(цяло new_rows, цяло new_cols, VTermStateFields *fields, void *user)
{
  VTermScreen *screen = user;

  цяло altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  цяло old_rows = screen->rows;
  цяло old_cols = screen->cols;

  ако(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    ако(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    ако (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, sizeof(VTermScreenCell) * new_cols);
  }

  ако (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  ако(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  иначе ако(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, sizeof(new_lineinfo[0]) * new_rows);
    за(цяло row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTermLineInfo){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  ако(new_cols <= old_cols) {
    ако(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, sizeof(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  ако(screen->callbacks && screen->callbacks->resize)
    върни (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  върни 1;
}

static цяло setlineinfo(цяло row, const VTermLineInfo *newinfo, const VTermLineInfo *oldinfo, void *user)
{
  VTermScreen *screen = user;

  ако(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    за(цяло col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      ако (cell == NULL)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	върни 1;
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

    ако(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  върни 1;
}

static цяло sb_clear(void *user) {
  VTermScreen *screen = user;

  ако(screen->callbacks && screen->callbacks->sb_clear)
    ако((*screen->callbacks->sb_clear)(screen->cbdata))
      върни 1;

  върни 0;
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
  ако(!state)
    върни NULL;

  VTermScreen *screen = vterm_allocator_malloc(vt, sizeof(VTermScreen));
  ако (screen == NULL)
    върни NULL;
  цяло rows, cols;

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

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, sizeof(VTermScreenCell) * cols);
  ако (screen->buffer == NULL || screen->sb_buffer == NULL)
  {
    vterm_screen_free(screen);
    върни NULL;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  върни screen;
}

INTERNAL void vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  ако(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

void vterm_screen_reset(VTermScreen *screen, цяло hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

static size_t _get_chars(const VTermScreen *screen, const цяло utf8, void *buffer, size_t len, const VTermRect rect)
{
  size_t outpos = 0;
  цяло padding = 0;

#define PUT(c)                                             \
  ако(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    ако(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (char *)buffer + outpos);   \
    иначе                                                   \
      outpos += thislen;                                   \
  }                                                        \
  иначе {                                                   \
    ако(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    иначе                                                   \
      outpos++;                                            \
  }

  за(цяло row = rect.start_row; row < rect.end_row; row++) {
    за(цяло col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      ако (cell == NULL)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	върни 1;
      }
      ако(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      иначе ако(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      иначе {
        докато(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        за(цяло i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    ако(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  върни outpos;
}

size_t vterm_screen_get_chars(const VTermScreen *screen, uint32_t *chars, size_t len, const VTermRect rect)
{
  върни _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(const VTermScreen *screen, char *str, size_t len, const VTermRect rect)
{
  върни _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
цяло vterm_screen_get_cell(const VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  ако(!intcell)
    върни 0;

  за(цяло i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    ако(!intcell->chars[i])
      break;
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

  ако(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    ако(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      ако(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } иначе ако(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } иначе {
        cell->width = 2;
      }
    } иначе
      cell->width = 1;
  } иначе {
    ако(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    иначе
      cell->width = 1;
  }

  върни 1;
}

цяло vterm_screen_is_eol(const VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  за(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    ако(cell->chars[0] != 0)
      върни 0;
  }

  върни 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  ако(vt->screen)
    върни vt->screen;

  vt->screen = screen_new(vt);
  върни vt->screen;
}

void vterm_screen_enable_reflow(VTermScreen *screen, цяло reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

void vterm_screen_enable_altscreen(VTermScreen *screen, цяло altscreen)
{
  ако(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    цяло rows, cols;
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
  върни screen->cbdata;
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
  върни vterm_state_get_unrecognised_fbdata(screen->state);
}

void vterm_screen_flush_damage(VTermScreen *screen)
{
  ако(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  ако(screen->damaged.start_row != -1) {
    ако(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

void vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

static цяло attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  ако((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    върни 1;
  ако((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    върни 1;
  ако((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    върни 1;
  ако((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    върни 1;
  ако((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    върни 1;
  ако((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    върни 1;
  ако((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    върни 1;
  ако((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    върни 1;
  ако((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    върни 1;
  ако((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    върни 1;
  ако((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    върни 1;
  ако((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    върни 1;

  върни 0;
}

цяло vterm_screen_get_attrs_extent(const VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  ако(extent->start_col < 0)
    extent->start_col = 0;
  ако(extent->end_col < 0)
    extent->end_col = screen->cols;

  цяло col;

  за(col = pos.col - 1; col >= extent->start_col; col--)
    ако(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      break;
  extent->start_col = col + 1;

  за(col = pos.col + 1; col < extent->end_col; col++)
    ако(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      break;
  extent->end_col = col - 1;

  върни 1;
}

void vterm_screen_convert_color_to_rgb(const VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

static void reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  за(цяло row = 0; row <= screen->rows - 1; row++)
    за(цяло col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      ако(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      ако(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

void vterm_screen_set_default_colors(VTermScreen *screen, const VTermColor *default_fg, const VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  ако(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  ако(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  ако(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
