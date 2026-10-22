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

  unsigned puur_naa_nka bold      : 1;
  unsigned puur_naa_nka underline : 2;
  unsigned puur_naa_nka italic    : 1;
  unsigned puur_naa_nka blink     : 1;
  unsigned puur_naa_nka reverse   : 1;
  unsigned puur_naa_nka conceal   : 1;
  unsigned puur_naa_nka strike    : 1;
  unsigned puur_naa_nka font      : 4; /* 0 to 9 */
  unsigned puur_naa_nka small     : 1;
  unsigned puur_naa_nka baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned puur_naa_nka protected_cell : 1;
  unsigned puur_naa_nka dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned puur_naa_nka dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
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
  puur_naa_nka callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  puur_naa_nka pending_scroll_downward, pending_scroll_rightward;

  puur_naa_nka rows;
  puur_naa_nka cols;

  unsigned puur_naa_nka global_reverse : 1;
  unsigned puur_naa_nka reflow : 1;

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

static ScreenCell *getcell(const VTermScreen *screen, puur_naa_nka row, puur_naa_nka col)
{
  ᨀᨘ(row < 0 || row >= screen->rows)
    ᨀᨘᨒ NULL;
  ᨀᨘ(col < 0 || col >= screen->cols)
    ᨀᨘᨒ NULL;
  ᨀᨘ (screen->buffer == NULL)
    ᨀᨘᨒ NULL;
  ᨀᨘᨒ screen->buffer + (screen->cols * row) + col;
}

static ScreenCell *alloc_buffer(VTermScreen *screen, puur_naa_nka rows, puur_naa_nka cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(ScreenCell) * rows * cols);

  for(puur_naa_nka row = 0; row < rows; row++) {
    for(puur_naa_nka col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  ᨀᨘᨒ new_buffer;
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
    ᨀᨘ(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    ᨀᨘᨁ ᨀᨘ(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      ᨀᨘᨒ;
    }
    ᨀᨘᨁ ᨀᨘ(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      ᨀᨘ(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      ᨀᨘ(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      ᨀᨘᨒ;
    }
    ᨀᨘᨁ {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    bha_nga;

  pak_sa VTERM_DAMAGE_SCREEN:
  pak_sa VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    ᨀᨘ(screen->damaged.start_row == -1)
      screen->damaged = rect;
    ᨀᨘᨁ {
      rect_expand(&screen->damaged, &rect);
    }
    ᨀᨘᨒ;

  default:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    ᨀᨘᨒ;
  }

  ᨀᨘ(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

static void damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

static puur_naa_nka putglyph(VTermGlyphInfo *info, VTermPos pos, void *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  ᨀᨘ(!cell)
    ᨀᨘᨒ 0;

  puur_naa_nka i;
  for(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  ᨀᨘ(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  for(puur_naa_nka col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    ᨀᨘ (onecell == NULL)
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

  ᨀᨘᨒ 1;
}

static void sb_pushline_from_row(VTermScreen *screen, puur_naa_nka row, puur_naa_nka continuation)
{
  VTermPos pos;
  pos.row = row;
  for(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  ᨀᨘ(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  ᨀᨘᨁ
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

static puur_naa_nka premove(VTermRect rect, void *user)
{
  VTermScreen *screen = user;

  ᨀᨘ(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    for(puur_naa_nka row = 0; row < rect.end_row; row++) {
      const VTermLineInfo *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  ᨀᨘᨒ 1;
}

static puur_naa_nka moverect_internal(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  puur_naa_nka cols = src.end_col - src.start_col;
  puur_naa_nka downward = src.start_row - dest.start_row;

  puur_naa_nka init_row, test_row, inc_row;
  ᨀᨘ(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  ᨀᨘᨁ {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  for(puur_naa_nka row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * parimaa_na(ScreenCell));

  ᨀᨘᨒ 1;
}

static puur_naa_nka moverect_user(VTermRect dest, VTermRect src, void *user)
{
  VTermScreen *screen = user;

  ᨀᨘ(screen->callbacks && screen->callbacks->moverect) {
    ᨀᨘ(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    ᨀᨘ((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      ᨀᨘᨒ 1;
  }

  damagerect(screen, dest);

  ᨀᨘᨒ 1;
}

static puur_naa_nka erase_internal(VTermRect rect, puur_naa_nka selective, void *user)
{
  VTermScreen *screen = user;

  for(puur_naa_nka row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    const VTermLineInfo *info = vterm_state_get_lineinfo(screen->state, row);

    for(puur_naa_nka col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      ᨀᨘ (cell == NULL)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	ᨀᨘᨒ 1;
      }
      ᨀᨘ(selective && cell->pen.protected_cell)
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

  ᨀᨘᨒ 1;
}

static puur_naa_nka erase_user(VTermRect rect, puur_naa_nka selective UNUSED, void *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  ᨀᨘᨒ 1;
}

static puur_naa_nka erase(VTermRect rect, puur_naa_nka selective, void *user)
{
  erase_internal(rect, selective, user);
  ᨀᨘᨒ erase_user(rect, 0, user);
}

static puur_naa_nka scrollrect(VTermRect rect, puur_naa_nka downward, puur_naa_nka rightward, void *user)
{
  VTermScreen *screen = user;

  ᨀᨘ(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    ᨀᨘᨒ 1;
  }

  ᨀᨘ(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  ᨀᨘ(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  ᨀᨘᨁ ᨀᨘ(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  ᨀᨘᨁ {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  ᨀᨘ(screen->damaged.start_row == -1)
    ᨀᨘᨒ 1;

  ᨀᨘ(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  ᨀᨘᨁ ᨀᨘ(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    ᨀᨘ(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      ᨀᨘ(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      ᨀᨘ(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    ᨀᨘ(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      ᨀᨘ(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      ᨀᨘ(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  ᨀᨘᨁ {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  ᨀᨘᨒ 1;
}

static puur_naa_nka movecursor(VTermPos pos, VTermPos oldpos, puur_naa_nka visible, void *user)
{
  VTermScreen *screen = user;

  ᨀᨘ(screen->callbacks && screen->callbacks->movecursor)
    ᨀᨘᨒ (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  ᨀᨘᨒ 0;
}

static puur_naa_nka setpenattr(VTermAttr attr, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(attr) {
  pak_sa VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->number;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_FONT:
    screen->pen.font = val->number;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    ᨀᨘᨒ 1;
  pak_sa VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->number;
    ᨀᨘᨒ 1;

  pak_sa VTERM_N_ATTRS:
    ᨀᨘᨒ 0;
  }

  ᨀᨘᨒ 0;
}

static puur_naa_nka settermprop(VTermProp prop, VTermValue *val, void *user)
{
  VTermScreen *screen = user;

  switch(prop) {
  pak_sa VTERM_PROP_ALTSCREEN:
    ᨀᨘ(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      ᨀᨘᨒ 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    ᨀᨘ(!val->boolean)
      damagescreen(screen);
    bha_nga;
  pak_sa VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    bha_nga;
  default:
    ; /* ignore */
  }

  ᨀᨘ(screen->callbacks && screen->callbacks->settermprop)
    ᨀᨘᨒ (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  ᨀᨘᨒ 1;
}

static puur_naa_nka bell(void *user)
{
  VTermScreen *screen = user;

  ᨀᨘ(screen->callbacks && screen->callbacks->bell)
    ᨀᨘᨒ (*screen->callbacks->bell)(screen->cbdata);

  ᨀᨘᨒ 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
static puur_naa_nka line_popcount(ScreenCell *buffer, puur_naa_nka row, puur_naa_nka rows UNUSED, puur_naa_nka cols)
{
  puur_naa_nka col = cols - 1;
  ᨀᨛ(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  ᨀᨘᨒ col + 1;
}

#define REFLOW (screen->reflow)

static void resize_buffer(VTermScreen *screen, puur_naa_nka bufidx, puur_naa_nka new_rows, puur_naa_nka new_cols, puur_naa_nka active, VTermStateFields *statefields)
{
  puur_naa_nka old_rows = screen->rows;
  puur_naa_nka old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTermLineInfo *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(ScreenCell) * new_rows * new_cols);
  VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, parimaa_na(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  puur_naa_nka old_row = old_rows - 1;
  puur_naa_nka new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  puur_naa_nka final_blank_row = new_rows;

  ᨀᨛ(old_row >= 0) {
    puur_naa_nka old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    ᨀᨛ(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    puur_naa_nka old_row_start = old_row;

    puur_naa_nka width = 0;
    for(puur_naa_nka row = old_row_start; row <= old_row_end; row++) {
      ᨀᨘ(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      ᨀᨘᨁ
        width += line_popcount(old_buffer, row, old_rows, old_cols);
    }

    ᨀᨘ(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    puur_naa_nka new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    puur_naa_nka new_row_end = new_row;
    puur_naa_nka new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    puur_naa_nka old_col = 0;

    puur_naa_nka spare_rows = new_rows - final_blank_row;

    ᨀᨘ(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      puur_naa_nka downwards = -new_row_start;
      ᨀᨘ(downwards > spare_rows)
        downwards = spare_rows;
      puur_naa_nka rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * parimaa_na(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * parimaa_na(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      ᨀᨘ(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    ᨀᨘ(new_row_start < 0) {
      ᨀᨘ(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        ᨀᨘ(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      bha_nga;
    }

    for(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      puur_naa_nka count = width >= new_cols ? new_cols : width;
      width -= count;

      puur_naa_nka new_col = 0;

      ᨀᨛ(count) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        ᨀᨘ(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        ᨀᨘ(old_col == old_cols) {
          old_row++;

          ᨀᨘ(!REFLOW) {
            new_col++;
            bha_nga;
          }
          old_col = 0;
        }

        new_col++;
        count--;
      }

      ᨀᨘ(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        ᨀᨘ(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      ᨀᨛ(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  ᨀᨘ(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    ᨀᨘ(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  ᨀᨘ(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    ᨀᨘ (new_cursor.row < 0)
      new_cursor.row = 0;
    ᨀᨘ (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  ᨀᨘ(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    ᨀᨘ((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      for(puur_naa_nka row = 0; row <= old_row; row++) {
        const VTermLineInfo *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    ᨀᨘ(active)
      statefields->pos.row -= (old_row + 1);
  }
  ᨀᨘ(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    ᨀᨛ(new_row >= 0) {
      VTermPos pos;
      ᨀᨘ(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        bha_nga;

      pos.row = new_row;
      for(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        for(puur_naa_nka i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          ᨀᨘ(!src->chars[i])
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

        ᨀᨘ(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      for( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      ᨀᨘ(active)
        statefields->pos.row++;
    }
  }
  ᨀᨘ(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    puur_naa_nka moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * parimaa_na(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * parimaa_na(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    for(new_row = moverows; new_row < new_rows; new_row++) {
      for(puur_naa_nka col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTermLineInfo){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  ᨀᨘ(active)
    statefields->pos = new_cursor;

  ᨀᨘᨒ;
}

static puur_naa_nka resize(puur_naa_nka new_rows, puur_naa_nka new_cols, VTermStateFields *fields, void *user)
{
  VTermScreen *screen = user;

  puur_naa_nka altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  puur_naa_nka old_rows = screen->rows;
  puur_naa_nka old_cols = screen->cols;

  ᨀᨘ(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    ᨀᨘ(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    ᨀᨘ (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(VTermScreenCell) * new_cols);
  }

  ᨀᨘ (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  ᨀᨘ(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  ᨀᨘᨁ ᨀᨘ(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTermLineInfo *new_lineinfo = vterm_allocator_malloc(screen->vt, parimaa_na(new_lineinfo[0]) * new_rows);
    for(puur_naa_nka row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTermLineInfo){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  ᨀᨘ(new_cols <= old_cols) {
    ᨀᨘ(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, parimaa_na(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  ᨀᨘ(screen->callbacks && screen->callbacks->resize)
    ᨀᨘᨒ (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  ᨀᨘᨒ 1;
}

static puur_naa_nka setlineinfo(puur_naa_nka row, const VTermLineInfo *newinfo, const VTermLineInfo *oldinfo, void *user)
{
  VTermScreen *screen = user;

  ᨀᨘ(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    for(puur_naa_nka col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      ᨀᨘ (cell == NULL)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	ᨀᨘᨒ 1;
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

    ᨀᨘ(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  ᨀᨘᨒ 1;
}

static puur_naa_nka sb_clear(void *user) {
  VTermScreen *screen = user;

  ᨀᨘ(screen->callbacks && screen->callbacks->sb_clear)
    ᨀᨘ((*screen->callbacks->sb_clear)(screen->cbdata))
      ᨀᨘᨒ 1;

  ᨀᨘᨒ 0;
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
  ᨀᨘ(!state)
    ᨀᨘᨒ NULL;

  VTermScreen *screen = vterm_allocator_malloc(vt, parimaa_na(VTermScreen));
  ᨀᨘ (screen == NULL)
    ᨀᨘᨒ NULL;
  puur_naa_nka rows, cols;

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
  ᨀᨘ (screen->buffer == NULL || screen->sb_buffer == NULL)
  {
    vterm_screen_free(screen);
    ᨀᨘᨒ NULL;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  ᨀᨘᨒ screen;
}

INTERNAL void vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  ᨀᨘ(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

void vterm_screen_reset(VTermScreen *screen, puur_naa_nka hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

static size_t _get_chars(const VTermScreen *screen, const puur_naa_nka utf8, void *buffer, size_t len, const VTermRect rect)
{
  size_t outpos = 0;
  puur_naa_nka padding = 0;

#define PUT(c)                                             \
  ᨀᨘ(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    ᨀᨘ(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (var_na *)buffer + outpos);   \
    ᨀᨘᨁ                                                   \
      outpos += thislen;                                   \
  }                                                        \
  ᨀᨘᨁ {                                                   \
    ᨀᨘ(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    ᨀᨘᨁ                                                   \
      outpos++;                                            \
  }

  for(puur_naa_nka row = rect.start_row; row < rect.end_row; row++) {
    for(puur_naa_nka col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      ᨀᨘ (cell == NULL)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	ᨀᨘᨒ 1;
      }
      ᨀᨘ(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      ᨀᨘᨁ ᨀᨘ(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      ᨀᨘᨁ {
        ᨀᨛ(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        for(puur_naa_nka i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    ᨀᨘ(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  ᨀᨘᨒ outpos;
}

size_t vterm_screen_get_chars(const VTermScreen *screen, uint32_t *chars, size_t len, const VTermRect rect)
{
  ᨀᨘᨒ _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(const VTermScreen *screen, var_na *str, size_t len, const VTermRect rect)
{
  ᨀᨘᨒ _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
puur_naa_nka vterm_screen_get_cell(const VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  ᨀᨘ(!intcell)
    ᨀᨘᨒ 0;

  for(puur_naa_nka i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    ᨀᨘ(!intcell->chars[i])
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

  ᨀᨘ(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    ᨀᨘ(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      ᨀᨘ(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } ᨀᨘᨁ ᨀᨘ(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } ᨀᨘᨁ {
        cell->width = 2;
      }
    } ᨀᨘᨁ
      cell->width = 1;
  } ᨀᨘᨁ {
    ᨀᨘ(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    ᨀᨘᨁ
      cell->width = 1;
  }

  ᨀᨘᨒ 1;
}

puur_naa_nka vterm_screen_is_eol(const VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  for(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    ᨀᨘ(cell->chars[0] != 0)
      ᨀᨘᨒ 0;
  }

  ᨀᨘᨒ 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  ᨀᨘ(vt->screen)
    ᨀᨘᨒ vt->screen;

  vt->screen = screen_new(vt);
  ᨀᨘᨒ vt->screen;
}

void vterm_screen_enable_reflow(VTermScreen *screen, puur_naa_nka reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

void vterm_screen_enable_altscreen(VTermScreen *screen, puur_naa_nka altscreen)
{
  ᨀᨘ(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    puur_naa_nka rows, cols;
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
  ᨀᨘᨒ screen->cbdata;
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
  ᨀᨘᨒ vterm_state_get_unrecognised_fbdata(screen->state);
}

void vterm_screen_flush_damage(VTermScreen *screen)
{
  ᨀᨘ(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  ᨀᨘ(screen->damaged.start_row != -1) {
    ᨀᨘ(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

void vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

static puur_naa_nka attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  ᨀᨘ((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    ᨀᨘᨒ 1;
  ᨀᨘ((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    ᨀᨘᨒ 1;

  ᨀᨘᨒ 0;
}

puur_naa_nka vterm_screen_get_attrs_extent(const VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  ᨀᨘ(extent->start_col < 0)
    extent->start_col = 0;
  ᨀᨘ(extent->end_col < 0)
    extent->end_col = screen->cols;

  puur_naa_nka col;

  for(col = pos.col - 1; col >= extent->start_col; col--)
    ᨀᨘ(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      bha_nga;
  extent->start_col = col + 1;

  for(col = pos.col + 1; col < extent->end_col; col++)
    ᨀᨘ(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      bha_nga;
  extent->end_col = col - 1;

  ᨀᨘᨒ 1;
}

void vterm_screen_convert_color_to_rgb(const VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

static void reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  for(puur_naa_nka row = 0; row <= screen->rows - 1; row++)
    for(puur_naa_nka col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      ᨀᨘ(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      ᨀᨘ(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

void vterm_screen_set_default_colors(VTermScreen *screen, const VTermColor *default_fg, const VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  ᨀᨘ(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  ᨀᨘ(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  ᨀᨘ(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
