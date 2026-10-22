#شامل "vterm_internal.h"

// vim: set sw=2 :
#شامل <stdio.h>
#شامل <string.h>

#شامل "rect.h"
#شامل "utf8.h"

#تعریف_کرو UNICODE_SPACE 0x20
#تعریف_کرو UNICODE_LINEFEED 0x0a

#تعریف_منسوخ DEBUG_REFLOW

/* State of the pen at some moment in time, also used in a cell */
قسمتعریف ساخت
{
  /* After the bitfield */
  VTermColor   fg, bg;

  غیرعلامتی صحیح bold      : 1;
  غیرعلامتی صحیح underline : 2;
  غیرعلامتی صحیح italic    : 1;
  غیرعلامتی صحیح blink     : 1;
  غیرعلامتی صحیح الٹ   : 1;
  غیرعلامتی صحیح conceal   : 1;
  غیرعلامتی صحیح strike    : 1;
  غیرعلامتی صحیح font      : 4; /* 0 to 9 */
  غیرعلامتی صحیح small     : 1;
  غیرعلامتی صحیح baseline  : 2;

  /* Extra state storage that isn't strictly pen-related */
  غیرعلامتی صحیح protected_cell : 1;
  غیرعلامتی صحیح dwl            : 1; /* on a DECDWL or DECDHL line */
  غیرعلامتی صحیح dhl            : 2; /* on a DECDHL line (1=top 2=bottom) */
} ScreenPen;

/* Internal representation of a screen cell */
قسمتعریف ساخت
{
  uint32_t chars[VTERM_MAX_CHARS_فی_CELL];
  ScreenPen pen;
} ScreenCell;

ساخت VTermScreen
{
  VTerm *vt;
  VTermState *state;

  ثابت VTermScreenCallbacks *callbacks;
  خالی *cbdata;
  صحیح callbacks_has_pushline4;

  VTerm_Damage_سائز damage_ملاؤ;
  /* start_row == -1 => no damage */
  VTermRect damaged;
  VTermRect pending_scrollrect;
  صحیح pending_scroll_downward, pending_scroll_rightward;

  صحیح صفیں_2;
  صحیح ستون_تعداد;

  غیرعلامتی صحیح global_الٹ : 1;
  غیرعلامتی صحیح reflow : 1;

  /* Primary and Altscreen. buffers[1] is lazily allocated as needed */
  ScreenCell *buffers[2];

  /* buffer will == buffers[0] or buffers[1], depending on altscreen */
  ScreenCell *بفر;

  /* buffer for a single screen row used in scrollback storage callbacks */
  VTermScreenCell *sb_بفر;

  ScreenPen pen;
};

ساکن خالی clearcell(ثابت VTermScreen *screen, ScreenCell *cell)
{
  cell->chars[0] = 0;
  cell->pen = screen->pen;
}

ساکن ScreenCell *getcell(ثابت VTermScreen *screen, صحیح صف, صحیح ستون)
{
  اگر(صف < 0 || صف >= screen->صفیں_2)
    واپس تہی;
  اگر(ستون < 0 || ستون >= screen->ستون_تعداد)
    واپس تہی;
  اگر (screen->بفر == تہی)
    واپس تہی;
  واپس screen->بفر + (screen->ستون_تعداد * صف) + ستون;
}

ساکن ScreenCell *alloc_بفر(VTermScreen *screen, صحیح صفیں_2, صحیح ستون_تعداد)
{
  ScreenCell *new_بفر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * صفیں_2 * ستون_تعداد);

  برائے(صحیح صف = 0; صف < صفیں_2; صف++) {
    برائے(صحیح ستون = 0; ستون < ستون_تعداد; ستون++) {
      clearcell(screen, &new_بفر[صف * ستون_تعداد + ستون]);
    }
  }

  واپس new_بفر;
}

ساکن خالی damagerect(VTermScreen *screen, VTermRect rect)
{
  VTermRect emit;

  انتخاب(screen->damage_ملاؤ) {
  حالت VTERM_DAMAGE_CELL:
    /* Always emit damage event */
    emit = rect;
    توڑو;

  حالت VTERM_DAMAGE_صف:
    /* Emit damage longer than one row. Try to merge with existing damage in
     * the same row */
    اگر(rect.انتہا_صف > rect.آغاز_صف + 1) {
      // Bigger than 1 line - flush existing, emit this
      vterm_screen_flush_damage(screen);
      emit = rect;
    }
    ورنہ اگر(screen->damaged.آغاز_صف == -1) {
      // None stored yet
      screen->damaged = rect;
      واپس;
    }
    ورنہ اگر(rect.آغاز_صف == screen->damaged.آغاز_صف) {
      // Merge with the stored line
      اگر(screen->damaged.آغاز_ستون > rect.آغاز_ستون)
        screen->damaged.آغاز_ستون = rect.آغاز_ستون;
      اگر(screen->damaged.انتہا_ستون < rect.انتہا_ستون)
        screen->damaged.انتہا_ستون = rect.انتہا_ستون;
      واپس;
    }
    ورنہ {
      // Emit the currently stored line, store a new one
      emit = screen->damaged;
      screen->damaged = rect;
    }
    توڑو;

  حالت VTERM_DAMAGE_SCREEN:
  حالت VTERM_DAMAGE_SCROLL:
    /* Never emit damage event */
    اگر(screen->damaged.آغاز_صف == -1)
      screen->damaged = rect;
    ورنہ {
      rect_expand(&screen->damaged, &rect);
    }
    واپس;

  طےشدہ:
    DEBUG_LOG1("TODO: Maybe merge damage for level %d\n", screen->damage_ملاؤ);
    واپس;
  }

  اگر(screen->callbacks && screen->callbacks->damage)
    (*screen->callbacks->damage)(emit, screen->cbdata);
}

ساکن خالی damagescreen(VTermScreen *screen)
{
  VTermRect rect = {0,0,0,0};
  rect.انتہا_صف = screen->صفیں_2;
  rect.انتہا_ستون = screen->ستون_تعداد;

  damagerect(screen, rect);
}

ساکن صحیح putglyph(VTermGlyphInfo *info, VTermPos pos, خالی *user)
{
  VTermScreen *screen = user;
  ScreenCell *cell = getcell(screen, pos.صف, pos.ستون);

  اگر(!cell)
    واپس 0;

  صحیح i;
  برائے(i = 0; i < VTERM_MAX_CHARS_فی_CELL && info->chars[i]; i++) {
    cell->chars[i] = info->chars[i];
    cell->pen = screen->pen;
  }
  اگر(i < VTERM_MAX_CHARS_فی_CELL)
    cell->chars[i] = 0;

  برائے(صحیح ستون = 1; ستون < info->چوڑائی; ستون++)
  {
    ScreenCell *onecell = getcell(screen, pos.صف, pos.ستون + ستون);
    اگر (onecell == تہی)
      توڑو;
    onecell->chars[0] = (uint32_t)-1;
  }

  VTermRect rect;
  rect.آغاز_صف = pos.صف;
  rect.انتہا_صف   = pos.صف+1;
  rect.آغاز_ستون = pos.ستون;
  rect.انتہا_ستون   = pos.ستون+info->چوڑائی;

  cell->pen.protected_cell = info->protected_cell;
  cell->pen.dwl            = info->dwl;
  cell->pen.dhl            = info->dhl;

  damagerect(screen, rect);

  واپس 1;
}

ساکن خالی sb_pushline_from_صف(VTermScreen *screen, صحیح صف, صحیح continuation)
{
  VTermPos pos;
  pos.صف = صف;
  برائے(pos.ستون = 0; pos.ستون < screen->ستون_تعداد; pos.ستون++)
    vterm_screen_get_cell(screen, pos, screen->sb_بفر + pos.ستون);

  اگر(screen->callbacks_has_pushline4 && screen->callbacks->sb_pushline4)
    (screen->callbacks->sb_pushline4)(screen->ستون_تعداد, screen->sb_بفر, continuation, screen->cbdata);
  ورنہ
    (screen->callbacks->sb_pushline)(screen->ستون_تعداد, screen->sb_بفر, screen->cbdata);
}

ساکن صحیح premove(VTermRect rect, خالی *user)
{
  VTermScreen *screen = user;

  اگر(((screen->callbacks && screen->callbacks->sb_pushline) ||
        (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4)) &&
     rect.آغاز_صف == 0 && rect.آغاز_ستون == 0 &&        // starts top-left corner
     rect.انتہا_ستون == screen->ستون_تعداد &&                      // full width
     screen->بفر == screen->buffers[BUFIDX_PRIMARY]) { // not altscreen
    برائے(صحیح صف = 0; صف < rect.انتہا_صف; صف++) {
      ثابت VTerm_سطر_Info *lineinfo = vterm_state_get_lineinfo(screen->state, صف);
      sb_pushline_from_صف(screen, صف, lineinfo->continuation);
    }
  }

  واپس 1;
}

ساکن صحیح moverect_اندرونی(VTermRect dest, VTermRect src, خالی *user)
{
  VTermScreen *screen = user;

  صحیح ستون_تعداد = src.انتہا_ستون - src.آغاز_ستون;
  صحیح downward = src.آغاز_صف - dest.آغاز_صف;

  صحیح init_صف, test_صف, inc_صف;
  اگر(downward < 0) {
    init_صف = dest.انتہا_صف - 1;
    test_صف = dest.آغاز_صف - 1;
    inc_صف  = -1;
  }
  ورنہ {
    init_صف = dest.آغاز_صف;
    test_صف = dest.انتہا_صف;
    inc_صف  = +1;
  }

  برائے(صحیح صف = init_صف; صف != test_صف; صف += inc_صف)
    memmove(getcell(screen, صف, dest.آغاز_ستون),
            getcell(screen, صف + downward, src.آغاز_ستون),
            ستون_تعداد * حجم(ScreenCell));

  واپس 1;
}

ساکن صحیح moverect_user(VTermRect dest, VTermRect src, خالی *user)
{
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->moverect) {
    اگر(screen->damage_ملاؤ != VTERM_DAMAGE_SCROLL)
      // Avoid an infinite loop
      vterm_screen_flush_damage(screen);

    اگر((*screen->callbacks->moverect)(dest, src, screen->cbdata))
      واپس 1;
  }

  damagerect(screen, dest);

  واپس 1;
}

ساکن صحیح erase_اندرونی(VTermRect rect, صحیح selective, خالی *user)
{
  VTermScreen *screen = user;

  برائے(صحیح صف = rect.آغاز_صف; صف < screen->state->صفیں_2 && صف < rect.انتہا_صف; صف++) {
    ثابت VTerm_سطر_Info *info = vterm_state_get_lineinfo(screen->state, صف);

    برائے(صحیح ستون = rect.آغاز_ستون; ستون < rect.انتہا_ستون; ستون++) {
      ScreenCell *cell = getcell(screen, صف, ستون);

      اگر (cell == تہی)
      {
        DEBUG_LOG2("libvterm: erase_internal() position invalid: %d / %d",
								     صف, ستون);
	واپس 1;
      }
      اگر(selective && cell->pen.protected_cell)
        جاری;

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

  واپس 1;
}

ساکن صحیح erase_user(VTermRect rect, صحیح selective غیر_مستعمل, خالی *user)
{
  VTermScreen *screen = user;

  damagerect(screen, rect);

  واپس 1;
}

ساکن صحیح erase(VTermRect rect, صحیح selective, خالی *user)
{
  erase_اندرونی(rect, selective, user);
  واپس erase_user(rect, 0, user);
}

ساکن صحیح scrollrect(VTermRect rect, صحیح downward, صحیح rightward, خالی *user)
{
  VTermScreen *screen = user;

  اگر(screen->damage_ملاؤ != VTERM_DAMAGE_SCROLL) {
    vterm_scroll_rect(rect, downward, rightward,
        moverect_اندرونی, erase_اندرونی, screen);

    vterm_screen_flush_damage(screen);

    vterm_scroll_rect(rect, downward, rightward,
        moverect_user, erase_user, screen);

    واپس 1;
  }

  اگر(screen->damaged.آغاز_صف != -1 &&
     !rect_intersects(&rect, &screen->damaged)) {
    vterm_screen_flush_damage(screen);
  }

  اگر(screen->pending_scrollrect.آغاز_صف == -1) {
    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }
  ورنہ اگر(rect_equal(&screen->pending_scrollrect, &rect) &&
     ((screen->pending_scroll_downward  == 0 && downward  == 0) ||
      (screen->pending_scroll_rightward == 0 && rightward == 0))) {
    screen->pending_scroll_downward  += downward;
    screen->pending_scroll_rightward += rightward;
  }
  ورنہ {
    vterm_screen_flush_damage(screen);

    screen->pending_scrollrect = rect;
    screen->pending_scroll_downward  = downward;
    screen->pending_scroll_rightward = rightward;
  }

  vterm_scroll_rect(rect, downward, rightward,
      moverect_اندرونی, erase_اندرونی, screen);

  اگر(screen->damaged.آغاز_صف == -1)
    واپس 1;

  اگر(rect_شامل_ہے(&rect, &screen->damaged)) {
    /* Scroll region entirely contains the damage; just move it */
    vterm_rect_move(&screen->damaged, -downward, -rightward);
    rect_clip(&screen->damaged, &rect);
  }
  /* There are a number of possible cases here, but lets restrict this to only
   * the common case where we might actually gain some performance by
   * optimising it. Namely, a vertical scroll that neatly cuts the damage
   * region in half.
   */
  ورنہ اگر(rect.آغاز_ستون <= screen->damaged.آغاز_ستون &&
          rect.انتہا_ستون   >= screen->damaged.انتہا_ستون &&
          rightward == 0) {
    اگر(screen->damaged.آغاز_صف >= rect.آغاز_صف &&
       screen->damaged.آغاز_صف  < rect.انتہا_صف) {
      screen->damaged.آغاز_صف -= downward;
      اگر(screen->damaged.آغاز_صف < rect.آغاز_صف)
        screen->damaged.آغاز_صف = rect.آغاز_صف;
      اگر(screen->damaged.آغاز_صف > rect.انتہا_صف)
        screen->damaged.آغاز_صف = rect.انتہا_صف;
    }
    اگر(screen->damaged.انتہا_صف >= rect.آغاز_صف &&
       screen->damaged.انتہا_صف  < rect.انتہا_صف) {
      screen->damaged.انتہا_صف -= downward;
      اگر(screen->damaged.انتہا_صف < rect.آغاز_صف)
        screen->damaged.انتہا_صف = rect.آغاز_صف;
      اگر(screen->damaged.انتہا_صف > rect.انتہا_صف)
        screen->damaged.انتہا_صف = rect.انتہا_صف;
    }
  }
  ورنہ {
    DEBUG_LOG2("TODO: Just flush and redo damaged=" STRFrect " rect=" STRFrect "\n",
        ARGSrect(screen->damaged), ARGSrect(rect));
  }

  واپس 1;
}

ساکن صحیح movecursor(VTermPos pos, VTermPos oldpos, صحیح visible, خالی *user)
{
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->movecursor)
    واپس (*screen->callbacks->movecursor)(pos, oldpos, visible, screen->cbdata);

  واپس 0;
}

ساکن صحیح setpenattr(VTermAttr attr, VTerm_قدر *val, خالی *user)
{
  VTermScreen *screen = user;

  انتخاب(attr) {
  حالت VTERM_ATTR_BOLD:
    screen->pen.bold = val->boolean;
    واپس 1;
  حالت VTERM_ATTR_UNDERLINE:
    screen->pen.underline = val->نمبر;
    واپس 1;
  حالت VTERM_ATTR_ITALIC:
    screen->pen.italic = val->boolean;
    واپس 1;
  حالت VTERM_ATTR_BLINK:
    screen->pen.blink = val->boolean;
    واپس 1;
  حالت VTERM_ATTR_الٹ:
    screen->pen.الٹ = val->boolean;
    واپس 1;
  حالت VTERM_ATTR_CONCEAL:
    screen->pen.conceal = val->boolean;
    واپس 1;
  حالت VTERM_ATTR_STRIKE:
    screen->pen.strike = val->boolean;
    واپس 1;
  حالت VTERM_ATTR_FONT:
    screen->pen.font = val->نمبر;
    واپس 1;
  حالت VTERM_ATTR_FOREGROUND:
    screen->pen.fg = val->color;
    واپس 1;
  حالت VTERM_ATTR_BACKGROUND:
    screen->pen.bg = val->color;
    واپس 1;
  حالت VTERM_ATTR_SMALL:
    screen->pen.small = val->boolean;
    واپس 1;
  حالت VTERM_ATTR_BASELINE:
    screen->pen.baseline = val->نمبر;
    واپس 1;

  حالت VTERM_N_ATTRS:
    واپس 0;
  }

  واپس 0;
}

ساکن صحیح settermprop(VTermProp prop, VTerm_قدر *val, خالی *user)
{
  VTermScreen *screen = user;

  انتخاب(prop) {
  حالت VTERM_PROP_ALTSCREEN:
    اگر(val->boolean && !screen->buffers[BUFIDX_ALTSCREEN])
      واپس 0;

    screen->بفر = val->boolean ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];
    /* only send a damage event on disable; because during enable there's an
     * erase that sends a damage anyway
     */
    اگر(!val->boolean)
      damagescreen(screen);
    توڑو;
  حالت VTERM_PROP_الٹ:
    screen->global_الٹ = val->boolean;
    damagescreen(screen);
    توڑو;
  طےشدہ:
    ; /* ignore */
  }

  اگر(screen->callbacks && screen->callbacks->settermprop)
    واپس (*screen->callbacks->settermprop)(prop, val, screen->cbdata);

  واپس 1;
}

ساکن صحیح bell(خالی *user)
{
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->bell)
    واپس (*screen->callbacks->bell)(screen->cbdata);

  واپس 0;
}

/* How many cells are non-blank
 * Returns the position of the first blank cell in the trailing blank end */
ساکن صحیح سطر_popcount(ScreenCell *بفر, صحیح صف, صحیح صفیں_2 غیر_مستعمل, صحیح ستون_تعداد)
{
  صحیح ستون = ستون_تعداد - 1;
  جب(ستون >= 0 && بفر[صف * ستون_تعداد + ستون].chars[0] == 0)
    ستون--;
  واپس ستون + 1;
}

#تعریف_کرو REFLOW (screen->reflow)

ساکن خالی resize_بفر(VTermScreen *screen, صحیح bufidx, صحیح new_صفیں, صحیح new_ستون_تعداد, صحیح فعال, VTermStateFields *statefields)
{
  صحیح old_صفیں = screen->صفیں_2;
  صحیح old_ستون_تعداد = screen->ستون_تعداد;

  ScreenCell *old_بفر = screen->buffers[bufidx];
  VTerm_سطر_Info *old_lineinfo = statefields->lineinfos[bufidx];

  ScreenCell *new_بفر = vterm_allocator_malloc(screen->vt, حجم(ScreenCell) * new_صفیں * new_ستون_تعداد);
  VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_صفیں);

  // Find the final row of old buffer content
  صحیح old_صف = old_صفیں - 1;
  صحیح new_صف = new_صفیں - 1;

  VTermPos old_cursor = statefields->pos;
  VTermPos new_cursor = { -1, -1 };

#اگر_تعریف_شدہ DEBUG_REFLOW
  fprintf(stderr, "Resizing from %dx%d to %dx%d; cursor was at (%d,%d)\n",
      old_ستون_تعداد, old_صفیں, new_ستون_تعداد, new_صفیں, old_cursor.ستون, old_cursor.صف);
#اگر_ختم

  /* Keep track of the final row that is knonw to be blank, so we know what
   * spare space we have for scrolling into
   */
  صحیح final_blank_صف = new_صفیں;

  جب(old_صف >= 0) {
    صحیح old_صف_انتہا = old_صف;
    /* TODO: Stop if dwl or dhl */
    جب(REFLOW && old_lineinfo && old_صف >= 0 && old_lineinfo[old_صف].continuation)
      old_صف--;
    صحیح old_صف_آغاز = old_صف;

    صحیح چوڑائی = 0;
    برائے(صحیح صف = old_صف_آغاز; صف <= old_صف_انتہا; صف++) {
      اگر(REFLOW && صف < (old_صفیں - 1) && old_lineinfo[صف + 1].continuation)
        چوڑائی += old_ستون_تعداد;
      ورنہ
        چوڑائی += سطر_popcount(old_بفر, صف, old_صفیں, old_ستون_تعداد);
    }

    اگر(final_blank_صف == (new_صف + 1) && چوڑائی == 0)
      final_blank_صف = new_صف;

    صحیح new_اونچائی = REFLOW
      ? چوڑائی ? (چوڑائی + new_ستون_تعداد - 1) / new_ستون_تعداد : 1
      : 1;

    صحیح new_صف_انتہا = new_صف;
    صحیح new_صف_آغاز = new_صف - new_اونچائی + 1;

    old_صف = old_صف_آغاز;
    صحیح old_ستون = 0;

    صحیح فالتو_صفیں = new_صفیں - final_blank_صف;

    اگر(new_صف_آغاز < 0 && /* we'd fall off the top */
        فالتو_صفیں >= 0 && /* we actually have spare rows */
        (!فعال || new_cursor.صف == -1 || (new_cursor.صف - new_صف_آغاز) < new_صفیں))
    {
      /* Attempt to scroll content down into the blank rows at the bottom to
       * make it fit
       */
      صحیح downwards = -new_صف_آغاز;
      اگر(downwards > فالتو_صفیں)
        downwards = فالتو_صفیں;
      صحیح rowcount = new_صفیں - downwards;

#اگر_تعریف_شدہ DEBUG_REFLOW
      fprintf(stderr, "  scroll %d rows +%d downwards\n", rowcount, downwards);
#اگر_ختم

      memmove(&new_بفر[downwards * new_ستون_تعداد], &new_بفر[0],   rowcount * new_ستون_تعداد * حجم(ScreenCell));
      memmove(&new_lineinfo[downwards],          &new_lineinfo[0], rowcount            * حجم(new_lineinfo[0]));

      new_صف += downwards;
      new_صف_آغاز += downwards;
      new_صف_انتہا += downwards;

      اگر(new_cursor.صف >= 0)
        new_cursor.صف += downwards;

      final_blank_صف += downwards;
    }

#اگر_تعریف_شدہ DEBUG_REFLOW
    fprintf(stderr, "  rows [%d..%d] <- [%d..%d] width=%d\n",
        new_صف_آغاز, new_صف_انتہا, old_صف_آغاز, old_صف_انتہا, چوڑائی);
#اگر_ختم

    اگر(new_صف_آغاز < 0) {
      اگر(old_صف_آغاز <= old_cursor.صف && old_cursor.صف < old_صف_انتہا) {
        new_cursor.صف = 0;
        new_cursor.ستون = old_cursor.ستون;
        اگر(new_cursor.ستون >= new_ستون_تعداد)
          new_cursor.ستون = new_ستون_تعداد-1;
      }
      توڑو;
    }

    برائے(new_صف = new_صف_آغاز, old_صف = old_صف_آغاز; new_صف <= new_صف_انتہا; new_صف++) {
      صحیح گنتی = چوڑائی >= new_ستون_تعداد ? new_ستون_تعداد : چوڑائی;
      چوڑائی -= گنتی;

      صحیح new_ستون = 0;

      جب(گنتی) {
        /* TODO: This could surely be done a lot faster by memcpy()'ing the entire range */
        new_بفر[new_صف * new_ستون_تعداد + new_ستون] = old_بفر[old_صف * old_ستون_تعداد + old_ستون];

        اگر(old_cursor.صف == old_صف && old_cursor.ستون == old_ستون)
          new_cursor.صف = new_صف, new_cursor.ستون = new_ستون;

        old_ستون++;
        اگر(old_ستون == old_ستون_تعداد) {
          old_صف++;

          اگر(!REFLOW) {
            new_ستون++;
            توڑو;
          }
          old_ستون = 0;
        }

        new_ستون++;
        گنتی--;
      }

      اگر(old_cursor.صف == old_صف && old_cursor.ستون >= old_ستون) {
        new_cursor.صف = new_صف, new_cursor.ستون = (old_cursor.ستون - old_ستون + new_ستون);
        اگر(new_cursor.ستون >= new_ستون_تعداد)
          new_cursor.ستون = new_ستون_تعداد-1;
      }

      جب(new_ستون < new_ستون_تعداد) {
        clearcell(screen, &new_بفر[new_صف * new_ستون_تعداد + new_ستون]);
        new_ستون++;
      }

      new_lineinfo[new_صف].continuation = (new_صف > new_صف_آغاز);
    }

    old_صف = old_صف_آغاز - 1;
    new_صف = new_صف_آغاز - 1;
  }

  اگر(old_cursor.صف <= old_صف) {
    /* cursor would have moved entirely off the top of the screen; lets just
     * bring it within range */
    new_cursor.صف = 0, new_cursor.ستون = old_cursor.ستون;
    اگر(new_cursor.ستون >= new_ستون_تعداد)
      new_cursor.ستون = new_ستون_تعداد-1;
  }

  /* We really expect the cursor position to be set by now */
  /* Unfortunately we do get here when "new_rows" is one.  We don't want
   * to crash, so until the above code is fixed let's just set the cursor. */
  اگر(فعال && (new_cursor.صف == -1 || new_cursor.ستون == -1)) {
    /* fprintf(stderr, "screen_resize failed to update cursor position\n");
     * abort(); */
    اگر (new_cursor.صف < 0)
      new_cursor.صف = 0;
    اگر (new_cursor.ستون < 0)
      new_cursor.ستون = 0;
  }

  اگر(old_صف >= 0 && bufidx == BUFIDX_PRIMARY) {
    /* Push spare lines to scrollback buffer */
    اگر((screen->callbacks && screen->callbacks->sb_pushline) ||
          (screen->callbacks_has_pushline4 && screen->callbacks && screen->callbacks->sb_pushline4))
      برائے(صحیح صف = 0; صف <= old_صف; صف++) {
        ثابت VTerm_سطر_Info *lineinfo = old_lineinfo + صف;
        sb_pushline_from_صف(screen, صف, lineinfo->continuation);
      }
    اگر(فعال)
      statefields->pos.صف -= (old_صف + 1);
  }
  اگر(new_صف >= 0 && bufidx == BUFIDX_PRIMARY &&
      screen->callbacks && screen->callbacks->sb_popline) {
    /* Try to backfill rows by popping scrollback buffer */
    جب(new_صف >= 0) {
      VTermPos pos;
      اگر(!(screen->callbacks->sb_popline(old_ستون_تعداد, screen->sb_بفر, screen->cbdata)))
        توڑو;

      pos.صف = new_صف;
      برائے(pos.ستون = 0; pos.ستون < old_ستون_تعداد && pos.ستون < new_ستون_تعداد; pos.ستون += screen->sb_بفر[pos.ستون].چوڑائی) {
        VTermScreenCell *src = &screen->sb_بفر[pos.ستون];
        ScreenCell *dst = &new_بفر[pos.صف * new_ستون_تعداد + pos.ستون];

        برائے(صحیح i = 0; i < VTERM_MAX_CHARS_فی_CELL; i++) {
          dst->chars[i] = src->chars[i];
          اگر(!src->chars[i])
            توڑو;
        }

        dst->pen.bold      = src->attrs.bold;
        dst->pen.underline = src->attrs.underline;
        dst->pen.italic    = src->attrs.italic;
        dst->pen.blink     = src->attrs.blink;
        dst->pen.الٹ   = src->attrs.الٹ ^ screen->global_الٹ;
        dst->pen.conceal   = src->attrs.conceal;
        dst->pen.strike    = src->attrs.strike;
        dst->pen.font      = src->attrs.font;
        dst->pen.small     = src->attrs.small;
        dst->pen.baseline  = src->attrs.baseline;

        dst->pen.fg = src->fg;
        dst->pen.bg = src->bg;

        اگر(src->چوڑائی == 2 && pos.ستون < (new_ستون_تعداد-1))
          (dst + 1)->chars[0] = (uint32_t) -1;
      }
      برائے( ; pos.ستون < new_ستون_تعداد; pos.ستون++)
        clearcell(screen, &new_بفر[pos.صف * new_ستون_تعداد + pos.ستون]);
      new_صف--;

      اگر(فعال)
        statefields->pos.صف++;
    }
  }
  اگر(new_صف >= 0) {
    /* Scroll new rows back up to the top and fill in blanks at the bottom */
    صحیح moverows = new_صفیں - new_صف - 1;
    memmove(&new_بفر[0], &new_بفر[(new_صف + 1) * new_ستون_تعداد], moverows * new_ستون_تعداد * حجم(ScreenCell));
    memmove(&new_lineinfo[0], &new_lineinfo[new_صف + 1], moverows * حجم(new_lineinfo[0]));

    new_cursor.صف -= (new_صف + 1);

    برائے(new_صف = moverows; new_صف < new_صفیں; new_صف++) {
      برائے(صحیح ستون = 0; ستون < new_ستون_تعداد; ستون++)
        clearcell(screen, &new_بفر[new_صف * new_ستون_تعداد + ستون]);
      new_lineinfo[new_صف] = (VTerm_سطر_Info){ 0 };
    }
  }

  vterm_allocator_free(screen->vt, old_بفر);
  screen->buffers[bufidx] = new_بفر;

  vterm_allocator_free(screen->vt, old_lineinfo);
  statefields->lineinfos[bufidx] = new_lineinfo;

  اگر(فعال)
    statefields->pos = new_cursor;

  واپس;
}

ساکن صحیح resize(صحیح new_صفیں, صحیح new_ستون_تعداد, VTermStateFields *fields, خالی *user)
{
  VTermScreen *screen = user;

  صحیح altscreen_فعال = (screen->buffers[BUFIDX_ALTSCREEN] && screen->بفر == screen->buffers[BUFIDX_ALTSCREEN]);

  صحیح old_صفیں = screen->صفیں_2;
  صحیح old_ستون_تعداد = screen->ستون_تعداد;

  اگر(new_ستون_تعداد > old_ستون_تعداد) {
    /* Ensure that ->sb_buffer is large enough for a new or and old row */
    اگر(screen->sb_بفر)
      vterm_allocator_free(screen->vt, screen->sb_بفر);

    اگر (new_ستون_تعداد > VTERM_MAX_ستون_تعداد)
      new_ستون_تعداد = VTERM_MAX_ستون_تعداد;

    screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_ستون_تعداد);
  }

  اگر (new_صفیں > VTERM_MAX_صفیں)
    new_صفیں = VTERM_MAX_صفیں;

  resize_بفر(screen, 0, new_صفیں, new_ستون_تعداد, !altscreen_فعال, fields);
  اگر(screen->buffers[BUFIDX_ALTSCREEN])
    resize_بفر(screen, 1, new_صفیں, new_ستون_تعداد, altscreen_فعال, fields);
  ورنہ اگر(new_صفیں != old_صفیں) {
    /* We don't need a full resize of the altscreen because it isn't enabled
     * but we should at least keep the lineinfo the right size */
    vterm_allocator_free(screen->vt, fields->lineinfos[BUFIDX_ALTSCREEN]);

    VTerm_سطر_Info *new_lineinfo = vterm_allocator_malloc(screen->vt, حجم(new_lineinfo[0]) * new_صفیں);
    برائے(صحیح صف = 0; صف < new_صفیں; صف++)
      new_lineinfo[صف] = (VTerm_سطر_Info){ 0 };

    fields->lineinfos[BUFIDX_ALTSCREEN] = new_lineinfo;
  }

  screen->بفر = altscreen_فعال ? screen->buffers[BUFIDX_ALTSCREEN] : screen->buffers[BUFIDX_PRIMARY];

  screen->صفیں_2 = new_صفیں;
  screen->ستون_تعداد = new_ستون_تعداد;

  اگر(new_ستون_تعداد <= old_ستون_تعداد) {
    اگر(screen->sb_بفر)
      vterm_allocator_free(screen->vt, screen->sb_بفر);

    screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * new_ستون_تعداد);
  }

  /* TODO: Maaaaybe we can optimise this if there's no reflow happening */
  damagescreen(screen);

  اگر(screen->callbacks && screen->callbacks->resize)
    واپس (*screen->callbacks->resize)(new_صفیں, new_ستون_تعداد, screen->cbdata);

  واپس 1;
}

ساکن صحیح setlineinfo(صحیح صف, ثابت VTerm_سطر_Info *newinfo, ثابت VTerm_سطر_Info *oldinfo, خالی *user)
{
  VTermScreen *screen = user;

  اگر(newinfo->doublewidth != oldinfo->doublewidth ||
     newinfo->doubleheight != oldinfo->doubleheight) {
    برائے(صحیح ستون = 0; ستون < screen->ستون_تعداد; ستون++) {
      ScreenCell *cell = getcell(screen, صف, ستون);
      اگر (cell == تہی)
      {
        DEBUG_LOG2("libvterm: setlineinfo() position invalid: %d / %d",
								     صف, ستون);
	واپس 1;
      }
      cell->pen.dwl = newinfo->doublewidth;
      cell->pen.dhl = newinfo->doubleheight;
    }

    VTermRect rect;
    rect.آغاز_صف = صف;
    rect.انتہا_صف   = صف + 1;
    rect.آغاز_ستون = 0;
    rect.انتہا_ستون   = newinfo->doublewidth ? screen->ستون_تعداد / 2 : screen->ستون_تعداد;
    damagerect(screen, rect);

    اگر(newinfo->doublewidth) {
      rect.آغاز_ستون = screen->ستون_تعداد / 2;
      rect.انتہا_ستون   = screen->ستون_تعداد;

      erase_اندرونی(rect, 0, user);
    }
  }

  واپس 1;
}

ساکن صحیح sb_clear(خالی *user) {
  VTermScreen *screen = user;

  اگر(screen->callbacks && screen->callbacks->sb_clear)
    اگر((*screen->callbacks->sb_clear)(screen->cbdata))
      واپس 1;

  واپس 0;
}

ساکن VTermStateCallbacks state_cbs = {
  &putglyph, // putglyph
  &movecursor, // movecursor
  &scrollrect, // scrollrect
  تہی, // moverect
  &erase, // erase
  تہی, // initpen
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
ساکن VTermScreen *screen_new(VTerm *vt)
{
  VTermState *state = vterm_obtain_state(vt);
  اگر(!state)
    واپس تہی;

  VTermScreen *screen = vterm_allocator_malloc(vt, حجم(VTermScreen));
  اگر (screen == تہی)
    واپس تہی;
  صحیح صفیں_2, ستون_تعداد;

  vterm_get_سائز(vt, &صفیں_2, &ستون_تعداد);

  screen->vt = vt;
  screen->state = state;

  screen->damage_ملاؤ = VTERM_DAMAGE_CELL;
  screen->damaged.آغاز_صف = -1;
  screen->pending_scrollrect.آغاز_صف = -1;

  screen->صفیں_2 = صفیں_2;
  screen->ستون_تعداد = ستون_تعداد;

  screen->global_الٹ = FALSE;
  screen->reflow = FALSE;

  screen->callbacks = تہی;
  screen->cbdata    = تہی;
  screen->callbacks_has_pushline4 = 0;

  screen->buffers[BUFIDX_PRIMARY] = alloc_بفر(screen, صفیں_2, ستون_تعداد);

  screen->بفر = screen->buffers[BUFIDX_PRIMARY];

  screen->sb_بفر = vterm_allocator_malloc(screen->vt, حجم(VTermScreenCell) * ستون_تعداد);
  اگر (screen->بفر == تہی || screen->sb_بفر == تہی)
  {
    vterm_screen_free(screen);
    واپس تہی;
  }

  vterm_state_سیٹ_callbacks(screen->state, &state_cbs, screen);
  vterm_state_callbacks_has_premove(screen->state);

  واپس screen;
}

اندرونی_2 خالی vterm_screen_free(VTermScreen *screen)
{
  vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_PRIMARY]);
  اگر(screen->buffers[BUFIDX_ALTSCREEN])
    vterm_allocator_free(screen->vt, screen->buffers[BUFIDX_ALTSCREEN]);

  vterm_allocator_free(screen->vt, screen->sb_بفر);

  vterm_allocator_free(screen->vt, screen);
}

خالی vterm_screen_reset(VTermScreen *screen, صحیح hard)
{
  screen->damaged.آغاز_صف = -1;
  screen->pending_scrollrect.آغاز_صف = -1;
  vterm_state_reset(screen->state, hard);
  vterm_screen_flush_damage(screen);
}

ساکن size_t _get_chars(ثابت VTermScreen *screen, ثابت صحیح utf8, خالی *بفر, size_t len, ثابت VTermRect rect)
{
  size_t outpos = 0;
  صحیح padding = 0;

#تعریف_کرو PUT(c)                                             \
  اگر(utf8) {                                               \
    size_t thislen = utf8_seqlen(c);                       \
    اگر(بفر && outpos + thislen <= len)                  \
      outpos += fill_utf8((c), (حرف *)بفر + outpos);   \
    ورنہ                                                   \
      outpos += thislen;                                   \
  }                                                        \
  ورنہ {                                                   \
    اگر(بفر && outpos + 1 <= len)                        \
      ((uint32_t*)بفر)[outpos++] = (c);                 \
    ورنہ                                                   \
      outpos++;                                            \
  }

  برائے(صحیح صف = rect.آغاز_صف; صف < rect.انتہا_صف; صف++) {
    برائے(صحیح ستون = rect.آغاز_ستون; ستون < rect.انتہا_ستون; ستون++) {
      ScreenCell *cell = getcell(screen, صف, ستون);

      اگر (cell == تہی)
      {
        DEBUG_LOG2("libvterm: _get_chars() position invalid: %d / %d",
								     صف, ستون);
	واپس 1;
      }
      اگر(cell->chars[0] == 0)
        // Erased cell, might need a space
        padding++;
      ورنہ اگر(cell->chars[0] == (uint32_t)-1)
        // Gap behind a double-width char, do nothing
        ;
      ورنہ {
        جب(padding) {
          PUT(UNICODE_SPACE);
          padding--;
        }
        برائے(صحیح i = 0; i < VTERM_MAX_CHARS_فی_CELL && cell->chars[i]; i++) {
          PUT(cell->chars[i]);
        }
      }
    }

    اگر(صف < rect.انتہا_صف - 1) {
      PUT(UNICODE_LINEFEED);
      padding = 0;
    }
  }

  واپس outpos;
}

size_t vterm_screen_get_chars(ثابت VTermScreen *screen, uint32_t *chars, size_t len, ثابت VTermRect rect)
{
  واپس _get_chars(screen, 0, chars, len, rect);
}

size_t vterm_screen_get_متن(ثابت VTermScreen *screen, حرف *str, size_t len, ثابت VTermRect rect)
{
  واپس _get_chars(screen, 1, str, len, rect);
}

/* Copy internal to external representation of a screen cell */
صحیح vterm_screen_get_cell(ثابت VTermScreen *screen, VTermPos pos, VTermScreenCell *cell)
{
  ScreenCell *intcell = getcell(screen, pos.صف, pos.ستون);

  اگر(!intcell)
    واپس 0;

  برائے(صحیح i = 0; i < VTERM_MAX_CHARS_فی_CELL; i++) {
    cell->chars[i] = intcell->chars[i];
    اگر(!intcell->chars[i])
      توڑو;
  }

  cell->attrs.bold      = intcell->pen.bold;
  cell->attrs.underline = intcell->pen.underline;
  cell->attrs.italic    = intcell->pen.italic;
  cell->attrs.blink     = intcell->pen.blink;
  cell->attrs.الٹ   = intcell->pen.الٹ ^ screen->global_الٹ;
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
    اگر(pos.ستون < (screen->ستون_تعداد - 1) &&
       getcell(screen, pos.صف, pos.ستون + 1)->chars[0] == (uint32_t)-1) {
      اگر(getcell(screen, pos.صف, pos.ستون)->chars[0] == 0x20) {
        getcell(screen, pos.صف, pos.ستون)->chars[0] = 0;
        cell->چوڑائی = 2;
      } ورنہ اگر(getcell(screen, pos.صف, pos.ستون)->chars[0] == 0) {
        getcell(screen, pos.صف, pos.ستون + 1)->chars[0] = 0;
        cell->چوڑائی = 1;
      } ورنہ {
        cell->چوڑائی = 2;
      }
    } ورنہ
      cell->چوڑائی = 1;
  } ورنہ {
    اگر(pos.ستون < (screen->ستون_تعداد - 1) &&
       getcell(screen, pos.صف, pos.ستون + 1)->chars[0] == (uint32_t)-1)
      cell->چوڑائی = 2;
    ورنہ
      cell->چوڑائی = 1;
  }

  واپس 1;
}

صحیح vterm_screen_is_eol(ثابت VTermScreen *screen, VTermPos pos)
{
  /* This cell is EOL if this and every cell to the right is black */
  برائے(; pos.ستون < screen->ستون_تعداد; pos.ستون++) {
    ScreenCell *cell = getcell(screen, pos.صف, pos.ستون);
    اگر(cell->chars[0] != 0)
      واپس 0;
  }

  واپس 1;
}

VTermScreen *vterm_obtain_screen(VTerm *vt)
{
  اگر(vt->screen)
    واپس vt->screen;

  vt->screen = screen_new(vt);
  واپس vt->screen;
}

خالی vterm_screen_enable_reflow(VTermScreen *screen, صحیح reflow)
{
  screen->reflow = reflow;
}

// Removed, causes a compiler warning and isn't used
// #undef vterm_screen_set_reflow
// void vterm_screen_set_reflow(VTermScreen *screen, int reflow)
// {
//   vterm_screen_enable_reflow(screen, reflow);
// }

خالی vterm_screen_enable_altscreen(VTermScreen *screen, صحیح altscreen)
{
  اگر(!screen->buffers[BUFIDX_ALTSCREEN] && altscreen) {
    صحیح صفیں_2, ستون_تعداد;
    vterm_get_سائز(screen->vt, &صفیں_2, &ستون_تعداد);

    screen->buffers[BUFIDX_ALTSCREEN] = alloc_بفر(screen, صفیں_2, ستون_تعداد);
  }
}

خالی vterm_screen_سیٹ_callbacks(VTermScreen *screen, ثابت VTermScreenCallbacks *callbacks, خالی *user)
{
  screen->callbacks = callbacks;
  screen->cbdata = user;
}

خالی *vterm_screen_get_cbdata(VTermScreen *screen)
{
  واپس screen->cbdata;
}

خالی vterm_screen_callbacks_has_pushline4(VTermScreen *screen)
{
  screen->callbacks_has_pushline4 = 1;
}

خالی vterm_screen_سیٹ_unrecognised_fallbacks(VTermScreen *screen, ثابت VTermStateFallbacks *fallbacks, خالی *user)
{
  vterm_state_سیٹ_unrecognised_fallbacks(screen->state, fallbacks, user);
}

خالی *vterm_screen_get_unrecognised_fbdata(VTermScreen *screen)
{
  واپس vterm_state_get_unrecognised_fbdata(screen->state);
}

خالی vterm_screen_flush_damage(VTermScreen *screen)
{
  اگر(screen->pending_scrollrect.آغاز_صف != -1) {
    vterm_scroll_rect(screen->pending_scrollrect, screen->pending_scroll_downward, screen->pending_scroll_rightward,
        moverect_user, erase_user, screen);

    screen->pending_scrollrect.آغاز_صف = -1;
  }

  اگر(screen->damaged.آغاز_صف != -1) {
    اگر(screen->callbacks && screen->callbacks->damage)
      (*screen->callbacks->damage)(screen->damaged, screen->cbdata);

    screen->damaged.آغاز_صف = -1;
  }
}

خالی vterm_screen_سیٹ_damage_ملاؤ(VTermScreen *screen, VTerm_Damage_سائز سائز)
{
  vterm_screen_flush_damage(screen);
  screen->damage_ملاؤ = سائز;
}

ساکن صحیح attrs_فرق(VTerm_Attr_نقاب attrs, ScreenCell *a, ScreenCell *b)
{
  اگر((attrs & VTERM_ATTR_BOLD_نقاب)       && (a->pen.bold != b->pen.bold))
    واپس 1;
  اگر((attrs & VTERM_ATTR_UNDERLINE_نقاب)  && (a->pen.underline != b->pen.underline))
    واپس 1;
  اگر((attrs & VTERM_ATTR_ITALIC_نقاب)     && (a->pen.italic != b->pen.italic))
    واپس 1;
  اگر((attrs & VTERM_ATTR_BLINK_نقاب)      && (a->pen.blink != b->pen.blink))
    واپس 1;
  اگر((attrs & VTERM_ATTR_الٹ_نقاب)    && (a->pen.الٹ != b->pen.الٹ))
    واپس 1;
  اگر((attrs & VTERM_ATTR_CONCEAL_نقاب)    && (a->pen.conceal != b->pen.conceal))
    واپس 1;
  اگر((attrs & VTERM_ATTR_STRIKE_نقاب)     && (a->pen.strike != b->pen.strike))
    واپس 1;
  اگر((attrs & VTERM_ATTR_FONT_نقاب)       && (a->pen.font != b->pen.font))
    واپس 1;
  اگر((attrs & VTERM_ATTR_FOREGROUND_نقاب) && !vterm_color_is_equal(&a->pen.fg, &b->pen.fg))
    واپس 1;
  اگر((attrs & VTERM_ATTR_BACKGROUND_نقاب) && !vterm_color_is_equal(&a->pen.bg, &b->pen.bg))
    واپس 1;
  اگر((attrs & VTERM_ATTR_SMALL_نقاب)    && (a->pen.small != b->pen.small))
    واپس 1;
  اگر((attrs & VTERM_ATTR_BASELINE_نقاب)    && (a->pen.baseline != b->pen.baseline))
    واپس 1;

  واپس 0;
}

صحیح vterm_screen_get_attrs_extent(ثابت VTermScreen *screen, VTermRect *extent, VTermPos pos, VTerm_Attr_نقاب attrs)
{
  ScreenCell *ہدف = getcell(screen, pos.صف, pos.ستون);

  // TODO: bounds check
  extent->آغاز_صف = pos.صف;
  extent->انتہا_صف   = pos.صف + 1;

  اگر(extent->آغاز_ستون < 0)
    extent->آغاز_ستون = 0;
  اگر(extent->انتہا_ستون < 0)
    extent->انتہا_ستون = screen->ستون_تعداد;

  صحیح ستون;

  برائے(ستون = pos.ستون - 1; ستون >= extent->آغاز_ستون; ستون--)
    اگر(attrs_فرق(attrs, ہدف, getcell(screen, pos.صف, ستون)))
      توڑو;
  extent->آغاز_ستون = ستون + 1;

  برائے(ستون = pos.ستون + 1; ستون < extent->انتہا_ستون; ستون++)
    اگر(attrs_فرق(attrs, ہدف, getcell(screen, pos.صف, ستون)))
      توڑو;
  extent->انتہا_ستون = ستون - 1;

  واپس 1;
}

خالی vterm_screen_convert_color_مقصد_rgb(ثابت VTermScreen *screen, VTermColor *ستون)
{
  vterm_state_convert_color_مقصد_rgb(screen->state, ستون);
}

ساکن خالی reset_default_colours(VTermScreen *screen, ScreenCell *بفر)
{
  برائے(صحیح صف = 0; صف <= screen->صفیں_2 - 1; صف++)
    برائے(صحیح ستون = 0; ستون <= screen->ستون_تعداد - 1; ستون++) {
      ScreenCell *cell = &بفر[صف * screen->ستون_تعداد + ستون];
      اگر(VTERM_COLOR_IS_DEFAULT_FG(&cell->pen.fg))
        cell->pen.fg = screen->pen.fg;
      اگر(VTERM_COLOR_IS_DEFAULT_BG(&cell->pen.bg))
        cell->pen.bg = screen->pen.bg;
    }
}

خالی vterm_screen_سیٹ_default_colors(VTermScreen *screen, ثابت VTermColor *default_fg, ثابت VTermColor *default_bg)
{
  vterm_state_سیٹ_default_colors(screen->state, default_fg, default_bg);

  اگر(default_fg && VTERM_COLOR_IS_DEFAULT_FG(&screen->pen.fg)) {
    screen->pen.fg = *default_fg;
    screen->pen.fg.type = (screen->pen.fg.type & ~VTERM_COLOR_DEFAULT_نقاب)
                        | VTERM_COLOR_DEFAULT_FG;
  }

  اگر(default_bg && VTERM_COLOR_IS_DEFAULT_BG(&screen->pen.bg)) {
    screen->pen.bg = *default_bg;
    screen->pen.bg.type = (screen->pen.bg.type & ~VTERM_COLOR_DEFAULT_نقاب)
                        | VTERM_COLOR_DEFAULT_BG;
  }

  reset_default_colours(screen, screen->buffers[0]);
  اگر(screen->buffers[1])
    reset_default_colours(screen, screen->buffers[1]);
}
