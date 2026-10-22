#ଅନ୍ତର୍ଭାଵ "vterm_internal.h"

// vim: set sw=2 :
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "rect.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

#define UNICODE_SPACE 0x20
#define UNICODE_LINEFEED 0x0a

#undef DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
  /* After the bitfield */
  VTermColor   fg, bg;

  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା bold      : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା underline : 2;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା italic    : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା blink     : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reverse   : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା conceal   : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା strike    : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା font      : 4; /* 0 to 9 */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା small     : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା protected_cell : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା dwl            : 1; /* on a DECDWL or DECDHL line */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
  uint32_t chars[VTERM_MAX_CHARS_PER_CELL];
  ScreenPen pen;
} ScreenCell;

ସଂରଚନା VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ଅଚଳ VTermScreenCallbacks *callbacks;
  ଶୂନ୍ୟ *cbdata;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା callbacks_has_pushline4;

  VTermDamageSize damage_merge;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା pending_scroll_downward, pending_scroll_rightward;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols;

  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା global_reverse : 1;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *buffer;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_buffer;

  ScreenPen pen;
};

ସ୍ଥିର ଶୂନ୍ୟ clearcell(ଅଚଳ VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ସ୍ଥିର ScreenCell *getcell(ଅଚଳ VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col)
{
  ଯଦି(row < 0 || row >= screen->rows)
    ଫେରନ୍ତୁ ଖାଲି;
  ଯଦି(col < 0 || col >= screen->cols)
    ଫେରନ୍ତୁ ଖାଲି;
  ଯଦି (screen->buffer == ଖାଲି)
    ଫେରନ୍ତୁ ଖାଲି;
  ଫେରନ୍ତୁ screen->buffer + (screen->cols * row) + col;
}

ସ୍ଥିର ScreenCell *alloc_buffer(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, ଆକାର(ScreenCell) * rows * cols);

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = 0; row < rows; row++) {
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0; col < cols; col++) {
      clearcell(screen, &new_buffer[row * cols + col]);
    }
  }

  ଫେରନ୍ତୁ new_buffer;
}

ସ୍ଥିର ଶୂନ୍ୟ damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  ବାଛ(screen->damage_merge) {
  ସ୍ଥିତି VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    ଭାଙ୍ଗ;

  ସ୍ଥିତି VTERM_DAMAGE_ROW:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    ଯଦି(rect.end_row > rect.start_row + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    ଅନ୍ୟଥା ଯଦି(screen->damaged.start_row == -1) {
      // None stored yet
      screen->damaged = rect;
      ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା ଯଦି(rect.start_row == screen->damaged.start_row) {
      // Merge with the stored line
      ଯଦି(screen->damaged.start_col > rect.start_col)
        screen->damaged.start_col = rect.start_col;
      ଯଦି(screen->damaged.end_col < rect.end_col)
        screen->damaged.end_col = rect.end_col;
      ଫେରନ୍ତୁ;
    }
    ଅନ୍ୟଥା {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି VTERM_DAMAGE_SCREEN:
  ସ୍ଥିତି VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    ଯଦି(screen->damaged.start_row == -1)
      screen->damaged = rect;
    ଅନ୍ୟଥା {
      rect_expand(&screen->damaged, &rect);
    }
    ଫେରନ୍ତୁ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_merge);
    ଫେରନ୍ତୁ;
  }

  ଯଦି(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ସ୍ଥିର ଶୂନ୍ୟ damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.end_row = screen->rows;
  rect.end_col = screen->cols;

  damagerect(screen, rect);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା putglyph(VTermGlyphInfo *info, VTermPos pos, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.row, pos.col);

  ଯଦି(!cell)
    ଫେରନ୍ତୁ 0;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ପାଇଁ(i = 0; i < VTERM_MAX_CHARS_PER_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  ଯଦି(i < VTERM_MAX_CHARS_PER_CELL)
    cell->chars[i] = 0;

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 1; col < info->width; col++)
  {
    ScreenCell *onecell = getcell(screen, pos.row, pos.col + col);
    ଯଦି (onecell == ଖାଲି)
      ଭାଙ୍ଗ;
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

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଶୂନ୍ୟ sb_pushline_from_row(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା continuation)
{
  VTermPos pos;
  pos.row = row;
  ପାଇଁ(pos.col = 0; pos.col < screen->cols; pos.col++)
    vterm_screen_get_cell(screen, pos, screen->sb_buffer + pos.col);

  ଯଦି(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->cols, screen->sb_buffer, continuation, screen->cbdata);
  ଅନ୍ୟଥା
    (screen->callbacks->sb_pushline)(screen->cols, screen->sb_buffer, screen->cbdata);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା premove(VTermRect rect, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ଯଦି(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.start_row == 0 && rect.start_col == 0 &&        // starts top-left corner
     rect.end_col == screen->cols &&                      // full width
     screen->buffer == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = 0; row < rect.end_row; row++) {
      ଅଚଳ VTerm_ଧାଡ଼ି_Info *lineinfo = vterm_state_get_lineinfo(screen->state, row);
      sb_pushline_from_row(screen, row, lineinfo->continuation);
    }
  }

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା moverect_internal(VTermRect dest, VTermRect src, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols = src.end_col - src.start_col;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward = src.start_row - dest.start_row;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା init_row, test_row, inc_row;
  ଯଦି(downward < 0) {
    init_row = dest.end_row - 1;
    test_row = dest.start_row - 1;
    inc_row  = -1;
  }
  ଅନ୍ୟଥା {
    init_row = dest.start_row;
    test_row = dest.end_row;
    inc_row  = +1;
  }

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = init_row; row != test_row; row += inc_row)
    memmove(getcell(screen, row, dest.start_col),
            getcell(screen, row + downward, src.start_col),
            cols * ଆକାର(ScreenCell));

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା moverect_user(VTermRect dest, VTermRect src, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ଯଦି(screen->callbacks && screen->callbacks->moverect) {
    ଯଦି(screen->damage_merge != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    ଯଦି((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      ଫେରନ୍ତୁ 1;
  }

  damagerect(screen, dest);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା erase_internal(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = rect.start_row; row < screen->state->rows && row < rect.end_row; row++) {
    ଅଚଳ VTerm_ଧାଡ଼ି_Info *info = vterm_state_get_lineinfo(screen->state, row);

    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      ଯଦି (cell == ଖାଲି)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     row, col);
	ଫେରନ୍ତୁ 1;
      }
      ଯଦି(selective && cell->pen.protected_cell)
        ଜାରି;

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

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା erase_user(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective UNUSED, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା erase(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା selective, ଶୂନ୍ୟ *user)
{
  erase_internal(rect, selective, user);
  ଫେରନ୍ତୁ erase_user(rect, 0, user);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scrollrect(VTermRect rect, ପୂର୍ଣ୍ଣସଂଖ୍ୟା downward, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rightward, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ଯଦି(screen->damage_merge != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_internal, erase_internal, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    ଫେରନ୍ତୁ 1;
  }

  ଯଦି(screen->damaged.start_row != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  ଯଦି(screen->pending_scrollrect.start_row == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  ଅନ୍ୟଥା ଯଦି(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  ଅନ୍ୟଥା {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_internal, erase_internal, screen);

  ଯଦି(screen->damaged.start_row == -1)
    ଫେରନ୍ତୁ 1;

  ଯଦି(rect_contains(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  ଅନ୍ୟଥା ଯଦି(rect.start_col <= screen->damaged.start_col &&
          rect.end_col   >= screen->damaged.end_col &&
          rightward == 0) {
    ଯଦି(screen->damaged.start_row >= rect.start_row &&
       screen->damaged.start_row  < rect.end_row) {
      screen->damaged.start_row -= downward;
      ଯଦି(screen->damaged.start_row < rect.start_row)
        screen->damaged.start_row = rect.start_row;
      ଯଦି(screen->damaged.start_row > rect.end_row)
        screen->damaged.start_row = rect.end_row;
    }
    ଯଦି(screen->damaged.end_row >= rect.start_row &&
       screen->damaged.end_row  < rect.end_row) {
      screen->damaged.end_row -= downward;
      ଯଦି(screen->damaged.end_row < rect.start_row)
        screen->damaged.end_row = rect.start_row;
      ଯଦି(screen->damaged.end_row > rect.end_row)
        screen->damaged.end_row = rect.end_row;
    }
  }
  ଅନ୍ୟଥା {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା movecursor(VTermPos pos, VTermPos oldpos, ପୂର୍ଣ୍ଣସଂଖ୍ୟା visible, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ଯଦି(screen->callbacks && screen->callbacks->movecursor)
    ଫେରନ୍ତୁ (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା setpenattr(VTermAttr attr, VTerm_ମୂଲ୍ୟ *val, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ବାଛ(attr) {
  ସ୍ଥିତି VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->ସଂଖ୍ୟା;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_REVERSE:
    screen->pen.reverse = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_FONT:
    screen->pen.font = val->ସଂଖ୍ୟା;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    ଫେରନ୍ତୁ 1;
  ସ୍ଥିତି VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->ସଂଖ୍ୟା;
    ଫେରନ୍ତୁ 1;

  ସ୍ଥିତି VTERM_N_ATTRS:
    ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା settermprop(VTermProp prop, VTerm_ମୂଲ୍ୟ *val, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ବାଛ(prop) {
  ସ୍ଥିତି VTERM_PROP_ALTSCREEN:
    ଯଦି(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      ଫେରନ୍ତୁ 0;

    screen->buffer = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    ଯଦି(!val->boolean)
      damagescreen(screen);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି VTERM_PROP_REVERSE:
    screen->global_reverse = val->boolean;
    damagescreen(screen);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ; /* ignore */
  }

  ଯଦି(screen->callbacks && screen->callbacks->settermprop)
    ଫେରନ୍ତୁ (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା bell(ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ଯଦି(screen->callbacks && screen->callbacks->bell)
    ଫେରନ୍ତୁ (*screen->callbacks->bell)(screen->cbdata);

  ଫେରନ୍ତୁ 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି_popcount(ScreenCell *buffer, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = cols - 1;
  ଯେତେବେଳେ(col >= 0 && buffer[row * cols + col].chars[0] == 0)
    col--;
  ଫେରନ୍ତୁ col + 1;
}

#define REFLOW (screen->reflow)

ସ୍ଥିର ଶୂନ୍ୟ resize_buffer(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufidx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_cols, ପୂର୍ଣ୍ଣସଂଖ୍ୟା active, VTermStateFields *statefields)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_rows = screen->rows;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_cols = screen->cols;

  ScreenCell *old_buffer = screen->buffers[bufidx];
  VTerm_ଧାଡ଼ି_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_buffer = vterm_allocator_malloc(screen->vt, ଆକାର(ScreenCell) * new_rows * new_cols);
  VTerm_ଧାଡ଼ି_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, ଆକାର(new_lineinfo[0]) * new_rows);

  // Find the final row of old buffer content
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_row = old_rows - 1;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_row = new_rows - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#ifdef DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_cols, old_rows, new_cols, new_rows, old_cursor.col, old_cursor.row);
#endif

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା final_blank_row = new_rows;

  ଯେତେବେଳେ(old_row >= 0) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_row_end = old_row;
    /* TODO: Stop if dwl or dhl */
    ଯେତେବେଳେ(REFLOW && old_lineinfo && old_row >= 0 && old_lineinfo[old_row].continuation)
      old_row--;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_row_start = old_row;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା width = 0;
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = old_row_start; row <= old_row_end; row++) {
      ଯଦି(REFLOW && row < (old_rows - 1) && old_lineinfo[row + 1].continuation)
        width += old_cols;
      ଅନ୍ୟଥା
        width += ଧାଡ଼ି_popcount(old_buffer, row, old_rows, old_cols);
    }

    ଯଦି(final_blank_row == (new_row + 1) && width == 0)
      final_blank_row = new_row;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_height = REFLOW
      ? width ? (width + new_cols - 1) / new_cols : 1
      : 1;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_row_end = new_row;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_row_start = new_row - new_height + 1;

    old_row = old_row_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_col = 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା spare_rows = new_rows - final_blank_row;

    ଯଦି(new_row_start < 0 && /* we'd fall off the top */
        spare_rows >= 0 && /* we actually have spare rows */
        (!active || new_cursor.row == -1 || (new_cursor.row - new_row_start) < new_rows))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା downwards = -new_row_start;
      ଯଦି(downwards > spare_rows)
        downwards = spare_rows;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା rowcount = new_rows - downwards;

#ifdef DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#endif

      memmove(&new_buffer[downwards * new_cols], &new_buffer[0],   rowcount * new_cols * ଆକାର(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * ଆକାର(new_lineinfo[0]));

      new_row += downwards;
      new_row_start += downwards;
      new_row_end += downwards;

      ଯଦି(new_cursor.row >= 0)
        new_cursor.row += downwards;

      final_blank_row += downwards;
    }

#ifdef DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_row_start, new_row_end, old_row_start, old_row_end, width);
#endif

    ଯଦି(new_row_start < 0) {
      ଯଦି(old_row_start <= old_cursor.row && old_cursor.row < old_row_end) {
        new_cursor.row = 0;
        new_cursor.col = old_cursor.col;
        ଯଦି(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }
      ଭାଙ୍ଗ;
    }

    ପାଇଁ(new_row = new_row_start, old_row = old_row_start; new_row <= new_row_end; new_row++) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = width >= new_cols ? new_cols : width;
      width -= ଗଣନା_2;

      ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_col = 0;

      ଯେତେବେଳେ(ଗଣନା_2) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_buffer[new_row * new_cols + new_col] = old_buffer[old_row * old_cols + old_col];

        ଯଦି(old_cursor.row == old_row && old_cursor.col == old_col)
          new_cursor.row = new_row, new_cursor.col = new_col;

        old_col++;
        ଯଦି(old_col == old_cols) {
          old_row++;

          ଯଦି(!REFLOW) {
            new_col++;
            ଭାଙ୍ଗ;
          }
          old_col = 0;
        }

        new_col++;
        ଗଣନା_2--;
      }

      ଯଦି(old_cursor.row == old_row && old_cursor.col >= old_col) {
        new_cursor.row = new_row, new_cursor.col = (old_cursor.col - old_col + new_col);
        ଯଦି(new_cursor.col >= new_cols)
          new_cursor.col = new_cols-1;
      }

      ଯେତେବେଳେ(new_col < new_cols) {
        clearcell(screen, &new_buffer[new_row * new_cols + new_col]);
        new_col++;
      }

      new_lineinfo[new_row].continuation = (new_row > new_row_start);
    }

    old_row = old_row_start - 1;
    new_row = new_row_start - 1;
  }

  ଯଦି(old_cursor.row <= old_row) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.row = 0, new_cursor.col = old_cursor.col;
    ଯଦି(new_cursor.col >= new_cols)
      new_cursor.col = new_cols-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  ଯଦି(active && (new_cursor.row == -1 || new_cursor.col == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    ଯଦି (new_cursor.row < 0)
      new_cursor.row = 0;
    ଯଦି (new_cursor.col < 0)
      new_cursor.col = 0;
  }

  ଯଦି(old_row >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    ଯଦି((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = 0; row <= old_row; row++) {
        ଅଚଳ VTerm_ଧାଡ଼ି_Info *lineinfo = old_lineinfo + row;
        sb_pushline_from_row(screen, row, lineinfo->continuation);
      }
    ଯଦି(active)
      statefields->pos.row -= (old_row + 1);
  }
  ଯଦି(new_row >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    ଯେତେବେଳେ(new_row >= 0) {
      VTermPos pos;
      ଯଦି(!(screen->callbacks->sb_popline(old_cols, screen->sb_buffer, screen->cbdata)))
        ଭାଙ୍ଗ;

      pos.row = new_row;
      ପାଇଁ(pos.col = 0; pos.col < old_cols && pos.col < new_cols; pos.col += screen->sb_buffer[pos.col].width) {
        VTermScreenCell *src = &screen->sb_buffer[pos.col];
        ScreenCell *dst = &new_buffer[pos.row * new_cols + pos.col];

        ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
          dst->chars[i] = src->chars[i];
          ଯଦି(!src->chars[i])
            ଭାଙ୍ଗ;
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

        ଯଦି(src->width == 2 && pos.col < (new_cols-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      ପାଇଁ( ; pos.col < new_cols; pos.col++)
        clearcell(screen, &new_buffer[pos.row * new_cols + pos.col]);
      new_row--;

      ଯଦି(active)
        statefields->pos.row++;
    }
  }
  ଯଦି(new_row >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା moverows = new_rows - new_row - 1;
    memmove(&new_buffer[0], &new_buffer[(new_row + 1) * new_cols], moverows * new_cols * ଆକାର(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_row + 1], moverows * ଆକାର(new_lineinfo[0]));

    new_cursor.row -= (new_row + 1);

    ପାଇଁ(new_row = moverows; new_row < new_rows; new_row++) {
      ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0; col < new_cols; col++)
        clearcell(screen, &new_buffer[new_row * new_cols + col]);
      new_lineinfo[new_row] = (VTerm_ଧାଡ଼ି_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_buffer);
  screen->buffers[bufidx] = new_buffer;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  ଯଦି(active)
    statefields->pos = new_cursor;

  ଫେରନ୍ତୁ;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା resize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା new_cols, VTermStateFields *fields, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା altscreen_active = (screen->buffers[BUFIDX_ALTSCREEN] && screen->buffer == screen->buffers[BUFIDX_ALTSCREEN]);

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_rows = screen->rows;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_cols = screen->cols;

  ଯଦି(new_cols > old_cols) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    ଯଦି(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    ଯଦି (new_cols > VTERM_MAX_COLS)
      new_cols = VTERM_MAX_COLS;

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, ଆକାର(VTermScreenCell) * new_cols);
  }

  ଯଦି (new_rows > VTERM_MAX_ROWS)
    new_rows = VTERM_MAX_ROWS;

  resize_buffer(screen, 0, new_rows, new_cols, !altscreen_active, fields);
  ଯଦି(screen->buffers[BUFIDX_ALTSCREEN])
    resize_buffer(screen, 1, new_rows, new_cols, altscreen_active, fields);
  ଅନ୍ୟଥା ଯଦି(new_rows != old_rows) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_ଧାଡ଼ି_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, ଆକାର(new_lineinfo[0]) * new_rows);
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = 0; row < new_rows; row++)
      new_lineinfo[row] = (VTerm_ଧାଡ଼ି_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->buffer = altscreen_active ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->rows = new_rows;
  screen->cols = new_cols;

  ଯଦି(new_cols <= old_cols) {
    ଯଦି(screen->sb_buffer)
      vterm_allocator_free(screen->vt, screen->sb_buffer);

    screen->sb_buffer = vterm_allocator_malloc(screen->vt, ଆକାର(VTermScreenCell) * new_cols);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  ଯଦି(screen->callbacks && screen->callbacks->resize)
    ଫେରନ୍ତୁ (*screen->callbacks->resize)(new_rows, new_cols, screen->cbdata);

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା setlineinfo(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ଅଚଳ VTerm_ଧାଡ଼ି_Info *newinfo, ଅଚଳ VTerm_ଧାଡ଼ି_Info *oldinfo, ଶୂନ୍ୟ *user)
{
  VTermScreen *screen = user;

  ଯଦି(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0; col < screen->cols; col++) {
      ScreenCell *cell = getcell(screen, row, col);
      ଯଦି (cell == ଖାଲି)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     row, col);
	ଫେରନ୍ତୁ 1;
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

    ଯଦି(newinfo->doublewidth) {
      rect.start_col = screen->cols / 2;
      rect.end_col   = screen->cols;

      erase_internal(rect, 0, user);
    }
  }

  ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sb_clear(ଶୂନ୍ୟ *user) {
  VTermScreen *screen = user;

  ଯଦି(screen->callbacks && screen->callbacks->sb_clear)
    ଯଦି((*screen->callbacks->sb_clear)(screen->cbdata))
      ଫେରନ୍ତୁ 1;

  ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  ଖାଲି, // moverect
  &erase, // erase
  ଖାଲି, // initpen
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
ସ୍ଥିର VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  ଯଦି(!state)
    ଫେରନ୍ତୁ ଖାଲି;

  VTermScreen *screen = vterm_allocator_malloc(vt, ଆକାର(VTermScreen));
  ଯଦି (screen == ଖାଲି)
    ଫେରନ୍ତୁ ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, cols;

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

  screen->callbacks = ଖାଲି;
  screen->cbdata    = ଖାଲି;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_buffer(screen, rows, cols);

  screen->buffer = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_buffer = vterm_allocator_malloc(screen->vt, ଆକାର(VTermScreenCell) * cols);
  ଯଦି (screen->buffer == ଖାଲି || screen->sb_buffer == ଖାଲି)
  {
    vterm_screen_free(screen);
    ଫେରନ୍ତୁ ଖାଲି;
  }

  vterm_state_set_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  ଫେରନ୍ତୁ screen;
}

INTERNAL ଶୂନ୍ୟ vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  ଯଦି(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_buffer);

  vterm_allocator_free(screen->vt, screen);
}

ଶୂନ୍ୟ vterm_screen_reset(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hard)
{
  screen->damaged.start_row = -1;
  screen->pending_scrollrect.start_row = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ସ୍ଥିର size_t _get_chars(ଅଚଳ VTermScreen *screen, ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା utf8, ଶୂନ୍ୟ *buffer, size_t len, ଅଚଳ VTermRect rect)
{
  size_t outpos = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା padding = 0;

#define PUT(c)                                             \
  ଯଦି(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    ଯଦି(buffer && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (ଅକ୍ଷର *)buffer + outpos);   \
    ଅନ୍ୟଥା                                                   \
      outpos += thislen;                                   \
  }                                                        \
  ଅନ୍ୟଥା {                                                   \
    ଯଦି(buffer && outpos + 1 <= len)                        \
      ((uint32_t*)buffer)[outpos++] = (c);                 \
    ଅନ୍ୟଥା                                                   \
      outpos++;                                            \
  }

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = rect.start_row; row < rect.end_row; row++) {
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = rect.start_col; col < rect.end_col; col++) {
      ScreenCell *cell = getcell(screen, row, col);

      ଯଦି (cell == ଖାଲି)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     row, col);
	ଫେରନ୍ତୁ 1;
      }
      ଯଦି(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      ଅନ୍ୟଥା ଯଦି(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      ଅନ୍ୟଥା {
        ଯେତେବେଳେ(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < VTERM_MAX_CHARS_PER_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    ଯଦି(row < rect.end_row - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  ଫେରନ୍ତୁ outpos;
}

size_t vterm_screen_get_chars(ଅଚଳ VTermScreen *screen, uint32_t *chars, size_t len, ଅଚଳ VTermRect rect)
{
  ଫେରନ୍ତୁ _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_text(ଅଚଳ VTermScreen *screen, ଅକ୍ଷର *str, size_t len, ଅଚଳ VTermRect rect)
{
  ଫେରନ୍ତୁ _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_screen_get_cell(ଅଚଳ VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.row, pos.col);

  ଯଦି(!intcell)
    ଫେରନ୍ତୁ 0;

  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < VTERM_MAX_CHARS_PER_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    ଯଦି(!intcell->chars[i])
      ଭାଙ୍ଗ;
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

  ଯଦି(vterm_get_special_pty_type() == 2) {
    // Get correct cell width from cell information contained in line buffer
    ଯଦି(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1) {
      ଯଦି(getcell(screen, pos.row, pos.col)->chars[0] == 0x20) {
        getcell(screen, pos.row, pos.col)->chars[0] = 0;
        cell->width = 2;
      } ଅନ୍ୟଥା ଯଦି(getcell(screen, pos.row, pos.col)->chars[0] == 0) {
        getcell(screen, pos.row, pos.col + 1)->chars[0] = 0;
        cell->width = 1;
      } ଅନ୍ୟଥା {
        cell->width = 2;
      }
    } ଅନ୍ୟଥା
      cell->width = 1;
  } ଅନ୍ୟଥା {
    ଯଦି(pos.col < (screen->cols - 1) &&
       getcell(screen, pos.row, pos.col + 1)->chars[0] == (uint32_t)-1)
      cell->width = 2;
    ଅନ୍ୟଥା
      cell->width = 1;
  }

  ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_screen_is_eol(ଅଚଳ VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  ପାଇଁ(; pos.col < screen->cols; pos.col++) {
    ScreenCell *cell = getcell(screen, pos.row, pos.col);
    ଯଦି(cell->chars[0] != 0)
      ଫେରନ୍ତୁ 0;
  }

  ଫେରନ୍ତୁ 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  ଯଦି(vt->screen)
    ଫେରନ୍ତୁ vt->screen;

  vt->screen = screen_new(vt);
  ଫେରନ୍ତୁ vt->screen;
}

ଶୂନ୍ୟ vterm_screen_enable_reflow(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

ଶୂନ୍ୟ vterm_screen_enable_altscreen(VTermScreen *screen, ପୂର୍ଣ୍ଣସଂଖ୍ୟା altscreen)
{
  ଯଦି(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, cols;
    vterm_get_size(screen->vt, &rows, &cols);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_buffer(screen, rows, cols);
  }
}

ଶୂନ୍ୟ vterm_screen_set_callbacks(VTermScreen *screen, ଅଚଳ VTermScreenCallbacks *callbacks, ଶୂନ୍ୟ *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

ଶୂନ୍ୟ *vterm_screen_get_cbdata(VTermScreen *screen)
{
  ଫେରନ୍ତୁ screen->cbdata;
}

ଶୂନ୍ୟ vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

ଶୂନ୍ୟ vterm_screen_set_unrecognised_fallbacks(VTermScreen *screen, ଅଚଳ VTermStateFallbacks *fallbacks, ଶୂନ୍ୟ *user)
{
  vterm_state_set_unrecognised_fallbacks(screen->state, fallbacks, user);
}

ଶୂନ୍ୟ *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  ଫେରନ୍ତୁ vterm_state_get_unrecognised_fbdata(screen->state);
}

ଶୂନ୍ୟ vterm_screen_flush_damage(VTermScreen *screen)
{
  ଯଦି(screen->pending_scrollrect.start_row != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.start_row = -1;
  }

  ଯଦି(screen->damaged.start_row != -1) {
    ଯଦି(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.start_row = -1;
  }
}

ଶୂନ୍ୟ vterm_screen_set_damage_merge(VTermScreen *screen, VTermDamageSize size)
{
  vterm_screen_flush_damage(screen);
  screen->damage_merge = size;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା attrs_differ(VTermAttrMask attrs, ScreenCell *a, ScreenCell *b)
{
  ଯଦି((attrs & VTERM_ATTR_BOLD_MASK)       && (a->pen.bold != b->pen.bold))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_UNDERLINE_MASK)  && (a->pen.underline != b->pen.underline))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_ITALIC_MASK)     && (a->pen.italic != b->pen.italic))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_BLINK_MASK)      && (a->pen.blink != b->pen.blink))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_REVERSE_MASK)    && (a->pen.reverse != b->pen.reverse))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_CONCEAL_MASK)    && (a->pen.conceal != b->pen.conceal))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_STRIKE_MASK)     && (a->pen.strike != b->pen.strike))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_FONT_MASK)       && (a->pen.font != b->pen.font))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_FOREGROUND_MASK) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_BACKGROUND_MASK) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_SMALL_MASK)    && (a->pen.small != b->pen.small))
    ଫେରନ୍ତୁ 1;
  ଯଦି((attrs & VTERM_ATTR_BASELINE_MASK)    && (a->pen.baseline != b->pen.baseline))
    ଫେରନ୍ତୁ 1;

  ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vterm_screen_get_attrs_extent(ଅଚଳ VTermScreen *screen, VTermRect *extent, VTermPos pos, VTermAttrMask attrs)
{
  ScreenCell *target = getcell(screen, pos.row, pos.col);

  // TODO: bounds check
  extent->start_row = pos.row;
  extent->end_row   = pos.row + 1;

  ଯଦି(extent->start_col < 0)
    extent->start_col = 0;
  ଯଦି(extent->end_col < 0)
    extent->end_col = screen->cols;

  ପୂର୍ଣ୍ଣସଂଖ୍ୟା col;

  ପାଇଁ(col = pos.col - 1; col >= extent->start_col; col--)
    ଯଦି(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      ଭାଙ୍ଗ;
  extent->start_col = col + 1;

  ପାଇଁ(col = pos.col + 1; col < extent->end_col; col++)
    ଯଦି(attrs_differ(attrs, target, getcell(screen, pos.row, col)))
      ଭାଙ୍ଗ;
  extent->end_col = col - 1;

  ଫେରନ୍ତୁ 1;
}

ଶୂନ୍ୟ vterm_screen_convert_color_to_rgb(ଅଚଳ VTermScreen *screen, VTermColor *col)
{
  vterm_state_convert_color_to_rgb(screen->state, col);
}

ସ୍ଥିର ଶୂନ୍ୟ reset_default_colours(VTermScreen *screen, ScreenCell *buffer)
{
  ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row = 0; row <= screen->rows - 1; row++)
    ପାଇଁ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା col = 0; col <= screen->cols - 1; col++) {
      ScreenCell *cell = &buffer[row * screen->cols + col];
      ଯଦି(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      ଯଦି(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

ଶୂନ୍ୟ vterm_screen_set_default_colors(VTermScreen *screen, ଅଚଳ VTermColor *default_fg, ଅଚଳ VTermColor *default_bg)
{
  vterm_state_set_default_colors(screen->state, default_fg, default_bg);

  ଯଦି(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  ଯଦି(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_MASK)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  ଯଦି(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
