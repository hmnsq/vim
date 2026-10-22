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

  unsigned පූර්ණ_සංඛ්‍යා bold      : 1;
  unsigned පූර්ණ_සංඛ්‍යා underline : 2;
  unsigned පූර්ණ_සංඛ්‍යා italic    : 1;
  unsigned පූර්ණ_සංඛ්‍යා blink     : 1;
  unsigned පූර්ණ_සංඛ්‍යා reverse   : 1;
  unsigned පූර්ණ_සංඛ්‍යා conceal   : 1;
  unsigned පූර්ණ_සංඛ්‍යා strike    : 1;
  unsigned පූර්ණ_සංඛ්‍යා font      : 4; /* 0 to 9 */
  unsigned පූර්ණ_සංඛ්‍යා small     : 1;
  unsigned පූර්ණ_සංඛ්‍යා baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned පූර්ණ_සංඛ්‍යා protected_cell : 1;
  unsigned පූර්ණ_සංඛ්‍යා dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned පූර්ණ_සංඛ්‍යා dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
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
  පූර්ණ_සංඛ්‍යා callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  පූර්ණ_සංඛ්‍යා pending_scroll_downward, pending_scroll_rightward;

  පූර්ණ_සංඛ්‍යා rows;
  පූර්ණ_සංඛ්‍යා cols;

  unsigned පූර්ණ_සංඛ්‍යා global_reverse : 1;
  unsigned පූර්ණ_සංඛ්‍යා reflow : 1;

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

static ScreenCell *getcell(const VTermScreen *screen, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා col)
{
  නම්(row < 0 || row >= screen->rows)
    ආපසු NULL;
  නම්(col < 0 || col >= screen->cols)
    ආපසු NULL;
  නම් (screen->buffer == NULL)
    ආපසු NULL;
  ආපසු screen->buffer + (screen->cols * row) + col;
}

static ScreenCell *alloc_buffer(VTermScreen *screen, පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(ScreenCell) * rows * cols);

  සඳහා(පූර්ණ_සංඛ්‍යා row = 0; row < rows; row++) {
    සඳහා(පූර්ණ_සංඛ්‍යා col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  ආපසු new_buffer;
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
    නම්(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    නැතහොත් නම්(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      ආපසු;
    }
    නැතහොත් නම්(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      නම්(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      නම්(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      ආපසු;
    }
    නැතහොත් {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    bha_nga;

  pak_sa VTERM_DAMAGE_SCREEN:
  pak_sa VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    නම්(screen->damaged.start_row == -1)
      screen->damaged = rect;
    නැතහොත් {
      rect_expand(&screen->damaged, &rect);
    }
    ආපසු;

  default:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    ආපසු;
  }

  නම්(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

static void damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

static පූර්ණ_සංඛ්‍යා putglyph(VTermGlyphInfo *info, VTermPos pos, void *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  නම්(!cell)
    ආපසු 0;

  පූර්ණ_සංඛ්‍යා i;
  සඳහා(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  නම්(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  සඳහා(පූර්ණ_සංඛ්‍යා col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    නම් (onecell == NULL)
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

  ආපසු 1;
}

static void sb_pushline_from_row(VTermScreen *screen, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා continuation)
{
  VTermPos pos;
  pos.row = row;
  සඳහා(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  නම්(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  නැතහොත්
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

static පූර්ණ_සංඛ්‍යා premove(VTermRect rect, void *user)
{
  VTermScreen *screen = user;

  නම්(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    සඳහා(පූර්ණ_සංඛ්‍යා row = 0; row < rect.end_row; row++) {
      const VTermLineInfo *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා moverect_internal(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  පූර්ණ_සංඛ්‍යා cols = src.end_col - src.start_col;
  පූර්ණ_සංඛ්‍යා downward = src.start_row - dest.start_row;

  පූර්ණ_සංඛ්‍යා init_row, test_row, inc_row;
  නම්(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  නැතහොත් {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  සඳහා(පූර්ණ_සංඛ්‍යා row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * parimaa_na(ScreenCell));

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා moverect_user(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  නම්(screen->callbacks && screen->callbacks->moverect) {
    නම්(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    නම්((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      ආපසු 1;
  }

  damagerect(screen, dest);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා erase_internal(VTermRect rect, පූර්ණ_සංඛ්‍යා selective, void *user)
{
  VTermScreen *screen = user;

  සඳහා(පූර්ණ_සංඛ්‍යා row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    const VTermLineInfo *info = vterm_state_get_lineinfo(screen->state, row);

    සඳහා(පූර්ණ_සංඛ්‍යා col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      නම් (cell == NULL)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	ආපසු 1;
      }
      නම්(selective && cell->pen.protected_cell)
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

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා erase_user(VTermRect rect, පූර්ණ_සංඛ්‍යා selective UNUSED, void *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා erase(VTermRect rect, පූර්ණ_සංඛ්‍යා selective, void *user)
{
  erase_internal(rect, selective, user);
  ආපසු erase_user(rect, 0, user);
}

static පූර්ණ_සංඛ්‍යා scrollrect(VTermRect rect, පූර්ණ_සංඛ්‍යා downward, පූර්ණ_සංඛ්‍යා rightward, void *user)
{
  VTermScreen *screen = user;

  නම්(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    ආපසු 1;
  }

  නම්(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  නම්(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  නැතහොත් නම්(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  නැතහොත් {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  නම්(screen->damaged.start_row == -1)
    ආපසු 1;

  නම්(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  නැතහොත් නම්(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    නම්(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      නම්(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      නම්(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    නම්(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      නම්(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      නම්(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  නැතහොත් {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා movecursor(VTermPos pos, VTermPos oldpos, පූර්ණ_සංඛ්‍යා visible, void *user)
{
  VTermScreen *screen = user;

  නම්(screen->callbacks && screen->callbacks->movecursor)
    ආපසු (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා setpenattr(VTermAttr attr, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(attr) {
  pak_sa VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    ආපසු 1;
  pak_sa VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->number;
    ආපසු 1;
  pak_sa VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    ආපසු 1;
  pak_sa VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    ආපසු 1;
  pak_sa VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    ආපසු 1;
  pak_sa VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    ආපසු 1;
  pak_sa VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    ආපසු 1;
  pak_sa VTERM_ATTR_FONT:
    screen->pen.font = val->number;
    ආපසු 1;
  pak_sa VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    ආපසු 1;
  pak_sa VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    ආපසු 1;
  pak_sa VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    ආපසු 1;
  pak_sa VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->number;
    ආපසු 1;

  pak_sa VTERM_N_ATTRS:
    ආපසු 0;
  }

  ආපසු 0;
}

static පූර්ණ_සංඛ්‍යා settermprop(VTermProp prop, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(prop) {
  pak_sa VTERM_PROP_ALTSCREEN:
    නම්(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      ආපසු 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    නම්(!val->boolean)
      damagescreen(screen);
    bha_nga;
  pak_sa VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    bha_nga;
  default:
    ; /* ignore */
  }

  නම්(screen->callbacks && screen->callbacks->settermprop)
    ආපසු (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා bell(void *user)
{
  VTermScreen *screen = user;

  නම්(screen->callbacks && screen->callbacks->bell)
    ආපසු (*screen->callbacks->bell)(screen->cbdata);

  ආපසු 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
static පූර්ණ_සංඛ්‍යා line_popcount(ScreenCell *buffer, පූර්ණ_සංඛ්‍යා row, පූර්ණ_සංඛ්‍යා rows UNUSED, පූර්ණ_සංඛ්‍යා cols)
{
  පූර්ණ_සංඛ්‍යා col = cols - 1;
  අතරතුර(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  ආපසු col + 1;
}

#define REFLOW (screen->reflow)

static void resize_buffer(VTermScreen *screen, පූර්ණ_සංඛ්‍යා bufidx, පූර්ණ_සංඛ්‍යා new_rows, පූර්ණ_සංඛ්‍යා new_cols, පූර්ණ_සංඛ්‍යා active, VTermStateFields *statefields)
{
  පූර්ණ_සංඛ්‍යා old_rows = screen->rows;
  පූර්ණ_සංඛ්‍යා old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTermLineInfo *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(ScreenCell) * new_rows * new_cols);
  VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, parimaa_na(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  පූර්ණ_සංඛ්‍යා old_row = old_rows - 1;
  පූර්ණ_සංඛ්‍යා new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  පූර්ණ_සංඛ්‍යා final_blank_row = new_rows;

  අතරතුර(old_row >= 0) {
    පූර්ණ_සංඛ්‍යා old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    අතරතුර(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    පූර්ණ_සංඛ්‍යා old_row_start = old_row;

    පූර්ණ_සංඛ්‍යා width = 0;
    සඳහා(පූර්ණ_සංඛ්‍යා row = old_row_start; row <= old_row_end; row++) {
      නම්(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      නැතහොත්
        width += line_popcount(old_buffer, row, old_rows, old_cols);
    }

    නම්(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    පූර්ණ_සංඛ්‍යා new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    පූර්ණ_සංඛ්‍යා new_row_end = new_row;
    පූර්ණ_සංඛ්‍යා new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    පූර්ණ_සංඛ්‍යා old_col = 0;

    පූර්ණ_සංඛ්‍යා spare_rows = new_rows - final_blank_row;

    නම්(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      පූර්ණ_සංඛ්‍යා downwards = -new_row_start;
      නම්(downwards > spare_rows)
        downwards = spare_rows;
      පූර්ණ_සංඛ්‍යා rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * parimaa_na(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * parimaa_na(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      නම්(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    නම්(new_row_start < 0) {
      නම්(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        නම්(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      bha_nga;
    }

    සඳහා(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      පූර්ණ_සංඛ්‍යා count = width >= new_cols ? new_cols : width;
      width -= count;

      පූර්ණ_සංඛ්‍යා new_col = 0;

      අතරතුර(count) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        නම්(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        නම්(old_col == old_cols) {
          old_row++;

          නම්(!REFLOW) {
            new_col++;
            bha_nga;
          }
          old_col = 0;
        }

        new_col++;
        count--;
      }

      නම්(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        නම්(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      අතරතුර(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  නම්(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    නම්(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  නම්(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    නම් (new_cursor.row < 0)
      new_cursor.row = 0;
    නම් (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  නම්(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    නම්((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      සඳහා(පූර්ණ_සංඛ්‍යා row = 0; row <= old_row; row++) {
        const VTermLineInfo *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    නම්(active)
      statefields->pos.row -= (old_row + 1);
  }
  නම්(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    අතරතුර(new_row >= 0) {
      VTermPos pos;
      නම්(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        bha_nga;

      pos.row = new_row;
      සඳහා(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        සඳහා(පූර්ණ_සංඛ්‍යා i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          නම්(!src->chars[i])
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

        නම්(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      සඳහා( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      නම්(active)
        statefields->pos.row++;
    }
  }
  නම්(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    පූර්ණ_සංඛ්‍යා moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * parimaa_na(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * parimaa_na(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    සඳහා(new_row = moverows; new_row < new_rows; new_row++) {
      සඳහා(පූර්ණ_සංඛ්‍යා col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTermLineInfo){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  නම්(active)
    statefields->pos = new_cursor;

  ආපසු;
}

static පූර්ණ_සංඛ්‍යා resize(පූර්ණ_සංඛ්‍යා new_rows, පූර්ණ_සංඛ්‍යා new_cols, VTermStateFields *fields, void *user)
{
  VTermScreen *screen = user;

  පූර්ණ_සංඛ්‍යා altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  පූර්ණ_සංඛ්‍යා old_rows = screen->rows;
  පූර්ණ_සංඛ්‍යා old_cols = screen->cols;

  නම්(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    නම්(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    නම් (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(VTermScreenCell) * new_cols);
  }

  නම් (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  නම්(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  නැතහොත් නම්(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, parimaa_na(new_lineinfo[0]) * new_rows);
    සඳහා(පූර්ණ_සංඛ්‍යා row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTermLineInfo){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  නම්(new_cols <= old_cols) {
    නම්(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  නම්(screen->callbacks && screen->callbacks->resize)
    ආපසු (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා setlineinfo(පූර්ණ_සංඛ්‍යා row, const VTermLineInfo *newinfo, const VTermLineInfo *oldinfo, void *user)
{
  VTermScreen *screen = user;

  නම්(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    සඳහා(පූර්ණ_සංඛ්‍යා col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      නම් (cell == NULL)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	ආපසු 1;
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

    නම්(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  ආපසු 1;
}

static පූර්ණ_සංඛ්‍යා sb_clear(void *user) {
  VTermScreen *screen = user;

  නම්(screen->callbacks && screen->callbacks->sb_clear)
    නම්((*screen->callbacks->sb_clear)(screen->cbdata))
      ආපසු 1;

  ආපසු 0;
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
  නම්(!state)
    ආපසු NULL;

  VTermScreen *screen = vterm_allocator_malloc(vt, parimaa_na(VTermScreen));
  නම් (screen == NULL)
    ආපසු NULL;
  පූර්ණ_සංඛ්‍යා rows, cols;

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
  නම් (screen->buffer == NULL || screen->sb_buffer == NULL)
  {
    vterm_screen_free(screen);
    ආපසු NULL;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  ආපසු screen;
}

INTERNAL void vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  නම්(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

void vterm_screen_reset(VTermScreen *screen, පූර්ණ_සංඛ්‍යා hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

static size_t _get_chars(const VTermScreen *screen, const පූර්ණ_සංඛ්‍යා utf8, void *buffer, size_t len, const VTermRect rect)
{
  size_t outpos = 0;
  පූර්ණ_සංඛ්‍යා padding = 0;

#define PUT(c)                                             \
  නම්(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    නම්(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (var_na *)buffer + outpos);   \
    නැතහොත්                                                   \
      outpos += thislen;                                   \
  }                                                        \
  නැතහොත් {                                                   \
    නම්(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    නැතහොත්                                                   \
      outpos++;                                            \
  }

  සඳහා(පූර්ණ_සංඛ්‍යා row = rect.start_row; row < rect.end_row; row++) {
    සඳහා(පූර්ණ_සංඛ්‍යා col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      නම් (cell == NULL)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	ආපසු 1;
      }
      නම්(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      නැතහොත් නම්(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      නැතහොත් {
        අතරතුර(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        සඳහා(පූර්ණ_සංඛ්‍යා i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    නම්(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  ආපසු outpos;
}

size_t vterm_screen_get_chars(const VTermScreen *screen, uint32_t *chars, size_t len, const VTermRect rect)
{
  ආපසු _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(const VTermScreen *screen, var_na *str, size_t len, const VTermRect rect)
{
  ආපසු _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
පූර්ණ_සංඛ්‍යා vterm_screen_get_cell(const VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  නම්(!intcell)
    ආපසු 0;

  සඳහා(පූර්ණ_සංඛ්‍යා i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    නම්(!intcell->chars[i])
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

  නම්(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    නම්(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      නම්(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } නැතහොත් නම්(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } නැතහොත් {
        cell->width = 2;
      }
    } නැතහොත්
      cell->width = 1;
  } නැතහොත් {
    නම්(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    නැතහොත්
      cell->width = 1;
  }

  ආපසු 1;
}

පූර්ණ_සංඛ්‍යා vterm_screen_is_eol(const VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  සඳහා(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    නම්(cell->chars[0] != 0)
      ආපසු 0;
  }

  ආපසු 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  නම්(vt->screen)
    ආපසු vt->screen;

  vt->screen = screen_new(vt);
  ආපසු vt->screen;
}

void vterm_screen_enable_reflow(VTermScreen *screen, පූර්ණ_සංඛ්‍යා reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

void vterm_screen_enable_altscreen(VTermScreen *screen, පූර්ණ_සංඛ්‍යා altscreen)
{
  නම්(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    පූර්ණ_සංඛ්‍යා rows, cols;
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
  ආපසු screen->cbdata;
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
  ආපසු vterm_state_get_unrecognised_fbdata(screen->state);
}

void vterm_screen_flush_damage(VTermScreen *screen)
{
  නම්(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  නම්(screen->damaged.start_row != -1) {
    නම්(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

void vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

static පූර්ණ_සංඛ්‍යා attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  නම්((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    ආපසු 1;
  නම්((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    ආපසු 1;

  ආපසු 0;
}

පූර්ණ_සංඛ්‍යා vterm_screen_get_attrs_extent(const VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  නම්(extent->start_col < 0)
    extent->start_col = 0;
  නම්(extent->end_col < 0)
    extent->end_col = screen->cols;

  පූර්ණ_සංඛ්‍යා col;

  සඳහා(col = pos.col - 1; col >= extent->start_col; col--)
    නම්(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      bha_nga;
  extent->start_col = col + 1;

  සඳහා(col = pos.col + 1; col < extent->end_col; col++)
    නම්(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      bha_nga;
  extent->end_col = col - 1;

  ආපසු 1;
}

void vterm_screen_convert_color_to_rgb(const VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

static void reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  සඳහා(පූර්ණ_සංඛ්‍යා row = 0; row <= screen->rows - 1; row++)
    සඳහා(පූර්ණ_සංඛ්‍යා col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      නම්(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      නම්(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

void vterm_screen_set_default_colors(VTermScreen *screen, const VTermColor *default_fg, const VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  නම්(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  නම්(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  නම්(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
